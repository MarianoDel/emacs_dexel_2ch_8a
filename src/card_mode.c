//-----------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CARD_MODE.C #############################
//-----------------------------------------------

// Includes --------------------------------------------------------------------
#include "card_mode.h"
#include "card_utils.h"
#include "lcd_utils.h"
#include "lcd.h"
#include "mfrc522.h"
#include "tim.h"
#include "uart.h"
#include "title_conf.h"
#include "hard.h"

#include <stdio.h>

// Module Private Types & Macros -----------------------------------------------
typedef enum {
    CARD_INIT_MFRC522 = 0,
    CARD_INIT,
    CARD_STANDBY,
    CARD_SHOW_SESSIONS,
    CARD_SHOW_SESSIONS_MOVE_AWAY,
    CARD_SHOW_SESSIONS_MOVE_CLOSE,
    CARD_SHOW_SESSIONS_MOVE_CLOSE_1,
    CARD_DISCOUNT,
    CARD_TO_INIT_SESSION,
    CARD_TO_INIT_SESSION_1,    
    CARD_ERROR_INVALID,
    CARD_ERROR_NO_CREDIT,
    CARD_ERROR_WRITING,
    CARD_ERROR_WAIT_TO_INIT,
    CARD_NO_RFID_DETECTED
    
} card_mode_t;


// Externals -------------------------------------------------------------------
extern char s_blank [];


// Globals ---------------------------------------------------------------------
unsigned char card_internal = 0;
unsigned char card_disc = 0;
card_mode_t card_mode_state = CARD_INIT_MFRC522;
volatile unsigned short tt_card_internal = 0;
volatile unsigned short tt_card_internal2 = 0;

// Module Private Functions ----------------------------------------------------
uint8_t Card_Mode_Check_And_Select (card_data_t *);


// Module Functions ------------------------------------------------------------
void Card_Mode_Standby_Reset (void)
{
    card_mode_state = CARD_INIT_MFRC522;
}


void Card_Mode_Standby_Init (void)
{
    card_mode_state = CARD_INIT;
}


unsigned char card_mode_internal_state = 0;
uint8_t id [20] = { 0 };
#define BLOCK_TO_UNLOCK    4    //sector 1 bloque 0
unsigned char s_key [] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
card_data_t card_data_last;
card_data_t card_data_current;
void Card_Mode_Standby (mem_bkp_t * configurations)
{
    unsigned char status = 0;
    // unsigned char size = 0;
    char s_lcd [40] = { 0 };
    
    switch (card_mode_state)
    {
    case CARD_INIT_MFRC522:
        RFID_CS_ON;
        RFID_RST_ON;
        LCD_Writel1(" Inicializando  ");
        Wait_ms(500);
        MFRC522_Init();
        Wait_ms(500);
        if (MFRC522_Verify_Version() != 0x92)
        {
            LCD_Writel2("No RFID detector");
            card_mode_state = CARD_NO_RFID_DETECTED;
            ChangeLed(LED_NO_RFID);
            tt_card_internal = 2000;
        }
        else
            card_mode_state++;
        
        break;

    case CARD_INIT:
#ifdef INITIAL_MSG_WURTH
        LCD_Writel1("WURTH ARGENTINA ");
#endif
#ifdef INITIAL_MSG_GEN_O3
        LCD_Writel1("GENERADOR OZONO ");        
#endif
        card_internal = 0;
        ChangeLed(LED_TREATMENT_STANDBY);
        card_mode_state++;
        break;

        
    case CARD_STANDBY:
        switch (card_internal)
        {
        case 0:
            if (!tt_card_internal)
            {
                LCD_Writel2("Acerque Tarjeta*");
                tt_card_internal = 1200;
                card_internal++;
            }
            break;

        case 1:
            if (!tt_card_internal)
            {
                LCD_Writel2(s_blank);
                tt_card_internal = 1200;
                card_internal = 0;
            }
            break;

        default:
            card_internal = 0;
            break;
        }

        // //si acerca tarjeta
        // status = MFRC522_Check_NoHalt(id);
        // // status = MFRC522_Check(id);        
        // if (status == MI_OK)
        // {
        //     // Card_SaveCardData(&card_data_current, id);
        //     card_data_current.uid_bytes[0] = *(id + 0);
        //     card_data_current.uid_bytes[1] = *(id + 1);
        //     card_data_current.uid_bytes[2] = *(id + 2);
        //     card_data_current.uid_bytes[3] = *(id + 3);
        //     card_data_current.bcc = *(id + 4);

        //     card_mode_state++;
        // }
        status = Card_Mode_Check_And_Select(&card_data_current);
        if (status == MI_OK)
        {
            Card_ShowCardIdent(&card_data_current);
            card_mode_state++;
        }
        break;

    case CARD_SHOW_SESSIONS:
        *(id+0) = card_data_current.uid_bytes[0];
        *(id+1) = card_data_current.uid_bytes[1];
        *(id+2) = card_data_current.uid_bytes[2];
        *(id+3) = card_data_current.uid_bytes[3];
        *(id+4) = card_data_current.bcc;
        status = MFRC522_Auth(PICC_AUTHENT1A, BLOCK_TO_UNLOCK, s_key, id);
        
        if (status == MI_OK)
        {
            unsigned char readblock[18] = { 0 };
            status = MFRC522_ReadBlock(BLOCK_TO_UNLOCK, readblock);
            if (status == MI_OK)
            {
                status = Card_ProcessDataString(readblock, &card_data_current);
                if (status == MI_OK)
                {
                    Card_ShowCardData(&card_data_current);
                    sprintf(s_lcd, "Sesiones %d/%d  ",
                            card_data_current.sessions_left,
                            card_data_current.sessions_orig);

                    LCD_Writel1(s_lcd);

                    // verifico credito
                    if (card_data_current.sessions_left > 0)
                    {
                        Card_CopyCard(&card_data_last, &card_data_current);
                        Card_EmptyCard(&card_data_current);
                        tt_card_internal = 3000;
                        card_mode_state++;
                    }
                    else
                    {
                        Usart1Send("no credit in card\n");
                        Wait_ms(1200);
                        card_mode_state = CARD_ERROR_NO_CREDIT;
                    }
                }
                else
                {
                    sprintf(s_lcd, "invalid data %s\n", readblock);
                    Usart1Send(s_lcd);
                }
            }
            else
                Usart1Send("cant read card data!\n");

            //desbloqueo rfid
            MFRC522_StopCrypto1();
        }
        else
            Usart1Send("cant authorize card!\n");
        
        if (status != MI_OK)
            card_mode_state = CARD_ERROR_INVALID;

        break;

    case CARD_SHOW_SESSIONS_MOVE_AWAY:
        // busco tener 3 segundos sin tarjeta
        status = Card_Mode_Check_And_Select(&card_data_current);
        if (status == MI_OK)
        {
            Card_ShowCardIdent(&card_data_current);
            if (Card_CompareCardIdent(&card_data_current, &card_data_last) == MI_OK)
                Usart1Send("tarjetas iguales\n");
            else
            {
                Usart1Send("tarjetas distintas\n");
                card_mode_state = CARD_ERROR_INVALID;
            }
            tt_card_internal = 3000;
        }

        if (!tt_card_internal2)
        {
            switch (card_mode_internal_state)
            {
            case 0:
                LCD_Writel2("Aleje y acerque*");
                tt_card_internal2 = 1200;
                card_mode_internal_state++;
                break;

            case 1:
                LCD_Writel2(s_blank);
                tt_card_internal2 = 1200;
                card_mode_internal_state--;
                break;

            default:
                card_mode_internal_state = 0;
                break;
            }
        }

        if (!tt_card_internal)    //pasaron 3 segundos sin tarjeta
        {
            card_mode_state = CARD_SHOW_SESSIONS_MOVE_CLOSE;
            tt_card_internal = 8000;
        }
        break;

    case CARD_SHOW_SESSIONS_MOVE_CLOSE:
        // busco que acerquen nuevamente y espero 8 segundos
        status = Card_Mode_Check_And_Select(&card_data_current);
        if (status == MI_OK)
        {
            Card_ShowCardIdent(&card_data_current);
            if (Card_CompareCardIdent(&card_data_current, &card_data_last) == MI_OK)
            {
                Usart1Send("tarjetas iguales\n");

                *(id+0) = card_data_current.uid_bytes[0];
                *(id+1) = card_data_current.uid_bytes[1];
                *(id+2) = card_data_current.uid_bytes[2];
                *(id+3) = card_data_current.uid_bytes[3];
                *(id+4) = card_data_current.bcc;
                status = MFRC522_Auth(PICC_AUTHENT1A, BLOCK_TO_UNLOCK, s_key, id);
                if (status == MI_OK)
                {
                    unsigned char readblock[18] = { 0 };
                    status = MFRC522_ReadBlock(BLOCK_TO_UNLOCK, readblock);
                    if (status == MI_OK)
                    {
                        status = Card_ProcessDataString(readblock, &card_data_current);
                        if (status == MI_OK)
                        {
                            status = Card_CompareCardData(&card_data_current, &card_data_last);
                            if (status == MI_OK)
                            {
                                Usart1Send("tarjetas con misma data\n");
                                LCD_Writel1(" Mantenga cerca ");
                                LCD_Writel2(s_blank);
                                LCD_2DO_RENGLON;
                                card_disc = 0;
                                tt_card_internal = 0;
                                card_mode_state = CARD_DISCOUNT;
                            }
                        }

                        if (status != MI_OK)
                        {
                            sprintf(s_lcd, "invalid data %s\n", readblock);
                            Usart1Send(s_lcd);
                        }
                    }
                    else
                        Usart1Send("cant read card data!\n");
                }

                if (status != MI_OK)
                {
                    //desbloqueo rfid
                    MFRC522_StopCrypto1();
                    card_mode_state = CARD_ERROR_INVALID;
                }
            }
            else
            {
                Usart1Send("tarjetas distintas\n");
                card_mode_state = CARD_ERROR_INVALID;
            }
        }

        if ((card_mode_state == CARD_SHOW_SESSIONS_MOVE_CLOSE) &&
            (!tt_card_internal2))
        {
            switch (card_mode_internal_state)
            {
            case 0:
                LCD_Writel2(" Mantenga cerca ");
                tt_card_internal2 = 1200;
                card_mode_internal_state++;
                break;

            case 1:
                LCD_Writel2(s_blank);
                tt_card_internal2 = 1200;
                card_mode_internal_state--;
                break;

            default:
                card_mode_internal_state = 0;
                break;
            }
        }

        //pasaron 8 segundos sin tarjeta
        if ((card_mode_state == CARD_SHOW_SESSIONS_MOVE_CLOSE) &&
            (!tt_card_internal))
            card_mode_state = CARD_ERROR_INVALID;

        break;

    case CARD_DISCOUNT:
        if (!tt_card_internal)
        {
            tt_card_internal = 100;

            if (card_disc < 16)
            {
                Lcd_TransmitStr(".");
                card_disc++;
            }
            else
            {
                // reviso si puedo grabar
                card_data_current.sessions_left--;
                Card_CreateDataString((unsigned char *)s_lcd, &card_data_current);
                if (MFRC522_WriteBlock(BLOCK_TO_UNLOCK, (uint8_t *)s_lcd) == MI_OK)
                {
                    if (MFRC522_ReadBlock(BLOCK_TO_UNLOCK, (uint8_t *)s_lcd) == MI_OK)
                    {
                        if (Card_CompareDataString((unsigned char *)s_lcd, &card_data_current) == MI_OK)
                            card_mode_state = CARD_TO_INIT_SESSION;
                        else
                            Usart1Send("card verify error!\n");
                    }
                    else
                        Usart1Send("cant re-read card data!\n");
                }
                else
                    Usart1Send("cant write card data!\n");

                if (card_mode_state == CARD_DISCOUNT)    //hubo algunos de los errores
                    card_mode_state = CARD_ERROR_WRITING;

                MFRC522_StopCrypto1();
            }
        }
        break;

    case CARD_TO_INIT_SESSION:
        if (!tt_card_internal)
        {
            // en dummy1 le paso el tiempo de sesion de tarjeta
            configurations->dummy1 = card_data_current.sessions_time;
            LCD_Writel1("Listo para      ");
            LCD_Writel2(" iniciar sesion ");
            tt_card_internal = 1000;
            card_mode_state++;
        }
        break;

    case CARD_TO_INIT_SESSION_1:
        if (!tt_card_internal)
        {
            LCD_Writel1("Presione O3 para");
            tt_card_internal = 1000;
            card_mode_state--;
        }
        break;
        
    case CARD_ERROR_INVALID:
        LCD_Writel2("Tarjeta invalida");
        tt_card_internal = 2000;
        card_mode_state = CARD_ERROR_WAIT_TO_INIT;
        break;

    case CARD_ERROR_NO_CREDIT:
        LCD_Writel1("Tarjeta agotada ");
        LCD_Writel2(s_blank);            
        tt_card_internal = 2000;
        card_mode_state = CARD_ERROR_WAIT_TO_INIT;
        break;
        
    case CARD_ERROR_WRITING:
        LCD_Writel1("Error al grabar ");
        LCD_Writel2(" la Tarjeta!!!  ");
        tt_card_internal = 2000;
        card_mode_state = CARD_ERROR_WAIT_TO_INIT;
        break;

    case CARD_ERROR_WAIT_TO_INIT:
        if (!tt_card_internal)
            card_mode_state = CARD_INIT;

        break;
        
    case CARD_NO_RFID_DETECTED:
        if (!tt_card_internal)
            card_mode_state = CARD_INIT_MFRC522;
        
        break;
        
    default:
        card_mode_state = CARD_INIT_MFRC522;
        break;
    }
}


uint8_t Card_Mode_Check_And_Select (card_data_t * cd)
{
    uint8_t id [18] = { 0 };
    uint8_t status = MI_ERR;

    status = MFRC522_Check_NoHalt(id);

    if (status == MI_OK)
    {
        status = MFRC522_SelectTag(id);

        if (status)
        {
            cd->uid_bytes[0] = *(id + 0);
            cd->uid_bytes[1] = *(id + 1);
            cd->uid_bytes[2] = *(id + 2);
            cd->uid_bytes[3] = *(id + 3);
            cd->bcc = *(id + 4);
            cd->type = status;

            status = MI_OK;
        }
    }

    return status;
}

void Card_Mode_Timeouts (void)
{
    if (tt_card_internal)
        tt_card_internal--;

    if (tt_card_internal2)
        tt_card_internal2--;
}

//--- end of file ---//
