//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F303
// ##
// #### MFRC522.C ################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_mfrc522.h"
#include "mfrc522.h"
#include "stm32f0xx.h"
#include "uart.h"
#include "hard.h"
#include "tim.h"
#include "card_utils.h"

#include <string.h>
#include <stdio.h>

// Externals -------------------------------------------------------------------
extern void MFRC522_CS_Enable(void);
extern void MFRC522_CS_Disable(void);
extern uint8_t Card_Mode_Check_And_Select (card_data_t *);

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TEST_Mfrc522 (void)
{
    unsigned char status = MI_OK;
    uint8_t id [20] = { 0 };
    char s_buf [100] = { 0 };
    RFID_CS_ON;
    RFID_RST_ON;
    Wait_ms(1000);
    MFRC522_Init();

    Wait_ms(2000);
    status = MFRC522_Verify_Version();
    sprintf(s_buf, "version: 0x%x\n", status);
    Usart1Send(s_buf);

    // if (INA4)
    //     Usart1Send("INA4 Enabled\n");

    card_data_t card_new;
    card_data_t card_harcoded;
    
    card_harcoded.sessions_left = 999;
    card_harcoded.sessions_orig = 999;
    card_harcoded.sessions_time = 1;            
    
    while (1)
    {
        Wait_ms(2000);
        Card_EmptyCard(&card_new);
        
        status = Card_Mode_Check_And_Select(&card_new);
        
        if (status == MI_OK)
        {
            Card_ShowCardIdent(&card_new);

            
            // autorizo el sector 1 bloque 0 = 4
#define BLOCK_TO_UNLOCK    4
            unsigned char s_key [] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
            *(id+0) = card_new.uid_bytes[0];
            *(id+1) = card_new.uid_bytes[1];
            *(id+2) = card_new.uid_bytes[2];
            *(id+3) = card_new.uid_bytes[3];
            *(id+4) = card_new.bcc;
            
            status = MFRC522_Auth(PICC_AUTHENT1A, BLOCK_TO_UNLOCK, s_key, id);
            if (status == MI_OK)
            {
                sprintf(s_buf, "MI_OK AuthA block %d\n", BLOCK_TO_UNLOCK);
                Usart1Send(s_buf);
                Wait_ms(30);

                unsigned char j = BLOCK_TO_UNLOCK;
                sprintf(s_buf, "\nreading block %d\n", j);
                Usart1Send(s_buf);
                Wait_ms(30);
                    
                unsigned char readblock[18] = { 0 };
                status = MFRC522_ReadBlock(j, readblock);
                if (status == MI_OK)
                {
                    status = Card_ProcessDataString(readblock, &card_new);
                    if (status == MI_OK)
                    {
                        Usart1Send("readed ok!!!\n");
                        if (Card_CompareCardData(&card_new, &card_harcoded) == MI_OK)
                            Usart1Send("same data as harcoded\n");
                        else
                        {
                            //no es bueno, cambiada o esta vacia, grabo
                            //write harcoded data
                            status = Card_CreateDataString(readblock, &card_harcoded);
                            status = MFRC522_WriteBlock(BLOCK_TO_UNLOCK, readblock);
                            if (status == MI_OK)
                                Usart1Send("\nsaved ok!!!\n");
                        }
                    }
                    else
                    {
                        //write harcoded data for the first time
                        status = Card_CreateDataString(readblock, &card_harcoded);
                        status = MFRC522_WriteBlock(BLOCK_TO_UNLOCK, readblock);
                        if (status == MI_OK)
                            Usart1Send("\nsaved ok!!!\n");
                    }
                }
                else
                {
                    if (status == MI_ERR)
                        Usart1Send("MI_ERR\n");
                    else if (status == MI_NOTAGERR)
                        Usart1Send("MI_NOTAGERR\n");
                    else
                        Usart1Send("Unknown\n");

                    break;                        
                }
                // }

                MFRC522_StopCrypto1();
            }
            else
            {
                if (status == MI_ERR)
                    Usart1Send("MI_ERR\n");
                else if (status == MI_NOTAGERR)
                    Usart1Send("MI_NOTAGERR\n");
                else
                    Usart1Send("Unknown\n");
            }

                
                // status = MFRC522_Auth(PICC_AUTHENT1B, BLOCK_TO_READ, s_key, id);
                // if (status == MI_OK)
                // {
                //     sprintf(s_buf, "MI_OK AuthB block %d\n", BLOCK_TO_READ);
                //     Usart1Send(s_buf);
                //     Wait_ms(100);
                // }
                // else
                // {
                //     if (status == MI_ERR)
                //         Usart1Send("MI_ERR\n");
                //     else if (status == MI_NOTAGERR)
                //         Usart1Send("MI_NOTAGERR\n");
                //     else
                //         Usart1Send("Unknown\n");
                    
                //     sprintf(s_buf, "AuthA & AuthB error block %d\n", BLOCK_TO_READ);
                //     Usart1Send(s_buf);
                //     MFRC522_Halt();
                //     Wait_ms(100);
                // }
            // }
        }
        

        // // read a data block
        // status = MFRC522_Check_NoHalt(id);
        // if (status == MI_OK)
        // {
        //     // show card id
        //     Usart1Send("MI_OK\n");            
        //     for (unsigned char i = 0; i < 4; i++)
        //     {                
        //         sprintf(s_buf, "i: %d id: 0x%x\n",
        //                 i,
        //                 *(id+i));
        //         Usart1Send(s_buf);
        //         Wait_ms(20);
        //     }

        //     //authenticate the card
        //     MFRC522_Auth();
        // }
    }
}


//--- end of file ---//
    
