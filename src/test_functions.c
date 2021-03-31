//------------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board #####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C #########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "test_mfrc522.h"
#include "hard.h"
#include "tim.h"
#include "lcd_utils.h"
#include "lcd.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_standby;
extern volatile unsigned char usart1_have_data;

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void TF_Led (void)
{
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        Wait_ms(1000);
    }
}


void TF_Buzzer (void)
{
    while (1)
    {
        if (BUZZER)
            BUZZER_OFF;
        else
            BUZZER_ON;

        Wait_ms(1000);
    }
}


void TF_Buzzer_Functions (void)
{
    while (1)
    {
        if (!timer_standby)
        {
            timer_standby = 5000;
            BuzzerCommands(BUZZER_SHORT_CMD, 2);
        }
        
        UpdateBuzzer();
    }
}


void TF_switchO3 (void)
{
    while (1)
    {
        if (SWITCH_O3)
        {
            LED_ON;
            BUZZER_ON;
        }
        else
        {
            LED_OFF;
            BUZZER_OFF;
        }
    }
}


void TF_switchSET (void)
{
    while (1)
    {
        if (SWITCH_SET)
        {
            LED_ON;
            BUZZER_ON;
        }
        else
        {
            LED_OFF;
            BUZZER_OFF;
        }
    }    
}


void TF_lcdE (void)
{
    while (1)
    {
        if (LCD_E)
            LCD_E_OFF;
        else
            LCD_E_ON;

        Wait_ms(10);
    }
}


void TF_lcdRS (void)
{
    while (1)
    {
        if (LCD_RS)
            LCD_RS_OFF;
        else
            LCD_RS_ON;

        Wait_ms(10);
    }
}


void TF_lcdBklight (void)
{
    while (1)
    {
        if (CTRL_BKL)
            CTRL_BKL_OFF;
        else
            CTRL_BKL_ON;

        Wait_ms(1000);
    }
}


void TF_lcdData (void)
{
    // while (1)
    // {
    //     //pa0 a pa3
    //     GPIOA->BSRR = 0x0000000F;
    //     Wait_ms(10);
    //     GPIOA->BSRR = 0x000F0000;
    //     Wait_ms(10);
    // }

    // GPIOA->BSRR = 0x00000005;
    // while (1);

    GPIOA->BSRR = 0x0000000A;
    while (1);
}


void TF_lcdBlink (void)
{
    LCD_UtilsInit();
    CTRL_BKL_ON;

    while (1)
    {
        while (LCD_ShowBlink("Kirno Technology",
                             "  Smart Driver  ",
                             2,
                             BLINK_DIRECT) != resp_finish);

        LCD_ClearScreen();
        Wait_ms(1000);
    }
}


void TF_lcdScroll (void)
{
    resp_t resp = resp_continue;

    LCD_UtilsInit();
    CTRL_BKL_ON;
    
    while (1)
    {
        // LCD_ClearScreen();
        // Wait_ms(2000);
        do {
            resp = LCD_Scroll1 ("GESTALD Security & Protection Dept.");
        } while (resp != resp_finish);

        Wait_ms(2000);
    }
}


void TF_lcdBigNumbers (void)
{
    LCD_UtilsInit();
    LCD_BigNumbersInit();
    CTRL_BKL_ON;
    
    while (1)
    {
        LCD_ClearScreen();
        // Lcd_SetDDRAM(0x06);
        // Lcd_senddata(0xa5);
        // Lcd_SetDDRAM(0x46);
        // Lcd_senddata(0xa5);
        
        for (unsigned char i = 0; i < 10; i++)
        {
            LCD_BigNumbers(0,i);
            Wait_ms(1000);
        }
        for (unsigned char i = 0; i < 10; i++)
        {
            LCD_BigNumbers(3,i);
            Wait_ms(1000);
        }
        for (unsigned char i = 0; i < 10; i++)
        {
            LCD_BigNumbers(7,i);
            Wait_ms(1000);
        }
        for (unsigned char i = 0; i < 10; i++)
        {
            LCD_BigNumbers(10,i);
            Wait_ms(1000);
        }        
    }
}


void TF_zcd (void)
{
    while (1)
    {
        if (AC_SYNC)
            LED_ON;
        else
            LED_OFF;
    }
}


void TF_zcd_by_int (void)
{
    EXTIOn();
    while (1);
}


void TF_Relay (void)
{
    while (1)
    {
        if (RELAY)
            RELAY_OFF;
        else
            RELAY_ON;

        Wait_ms(2000);
    }
}

void TF_RelayBoardOutputs (void)
{
    while (1)
    {
        if (ACT_R1)
            ACT_R1_OFF;
        else
            ACT_R1_ON;

        if (ACT_R2)
            ACT_R2_OFF;
        else
            ACT_R2_ON;

        if (ACT_R3)
            ACT_R3_OFF;
        else
            ACT_R3_ON;

        if (ACT_R4)
            ACT_R4_OFF;
        else
            ACT_R4_ON;
        
        Wait_ms(1000);
    }
}


void TF_MenuFunction (void)
{
    char s_lcd[20] = { 0 };
    resp_t resp = resp_continue;
    
    LCD_UtilsInit();
    CTRL_BKL_ON;

    Wait_ms(500);
    LCD_PasswordReset ();
    unsigned int new_psw = 0;
    
    while (1)
    {
        sw_actions_t actions = selection_none;

        if (CheckSET() > SW_NO)
            actions = selection_enter;

        if (CheckCCW())
            actions = selection_dwn;

        if (CheckCW())
            actions = selection_up;
        
        resp = LCD_Password ("Ingrese Password", actions, &new_psw);

        if (resp == resp_selected)
        {
            if (new_psw != 0x00000022)
            {
                LCD_1ER_RENGLON;
                Lcd_TransmitStr("El nuevo pass:  ");
                sprintf(s_lcd, "0x%08x        ", new_psw);
                LCD_2DO_RENGLON;
                Lcd_TransmitStr(s_lcd);
                Wait_ms(5000);
            }
            else
            {
                LCD_1ER_RENGLON;
                Lcd_TransmitStr("    Password    ");                
                LCD_2DO_RENGLON;
                Lcd_TransmitStr("   Correcto!!!  ");
                Wait_ms(2000);
            }

            actions = selection_none;
            LCD_PasswordReset ();
        }
        UpdateSwitches();
        UpdateEncoder();
    }
}


void TF_Usart1_RxTx (void)
{
    for (unsigned char i = 0; i < 5; i++)
    {
        LED_ON;
        Wait_ms(250);
        LED_OFF;
        Wait_ms(250);
    }
    
    Usart1Config();

    char s_to_send [100] = { 0 };
    Usart1Send("Ready to test...\n");
    while (1)
    {
        if (usart1_have_data)
        {
            usart1_have_data = 0;
            
            if (LED)
                LED_OFF;
            else
                LED_ON;
            
            Usart1ReadBuffer((unsigned char *) s_to_send, 100);
            Wait_ms(1000);
            Usart1Send(s_to_send);
        }
    }
}


void TF_SPI (void)
{
    Usart1Config();
    Usart1Send("go to config spi...\n");
    
    SPI_Config();
    Usart1Send("spi on\n");
    
    while (1)
    {
        Wait_ms(500);
        LED_ON;
        SPI_Send_Single(0x77);
        LED_OFF;
    }
}


void TF_SPI_MFRC (void)
{
    Usart1Config();
    Usart1Send("usart on\n");
    SPI_Config();
    Usart1Send("spi on\n");
    
    TEST_Mfrc522();
}

//--- end of file ---//
