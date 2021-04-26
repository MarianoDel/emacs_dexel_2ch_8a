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
#include "hard.h"
#include "tim.h"
#include "lcd_utils.h"
#include "lcd.h"
#include "gpio.h"
#include "uart.h"
#include "dmx_receiver.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short timer_standby;
extern volatile unsigned char usart1_have_data;

extern volatile unsigned char dmx_buff_data[];
extern volatile unsigned char Packet_Detected_Flag;
extern volatile unsigned short DMX_channel_selected;
extern volatile unsigned char DMX_channel_quantity;


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


void TF_SW_UP (void)
{
    while (1)
    {
        if (SW_UP)
            LED_ON;
        else
            LED_OFF;
    }    
}


void TF_SW_SEL (void)
{
    while (1)
    {
        if (SW_SEL)
            LED_ON;
        else
            LED_OFF;
    }    
}


void TF_SW_DWN (void)
{
    while (1)
    {
        if (SW_DWN)
            LED_ON;
        else
            LED_OFF;
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
    while (1)
    {
        //pa0 a pa3
        LED_ON;
        GPIOA->BSRR = 0x0000000F;
        Wait_ms(2000);
        
        LED_OFF;
        GPIOA->BSRR = 0x000F0000;
        Wait_ms(2000);

        LED_ON;
        GPIOA->BSRR = 0x00000005;
        Wait_ms(2000);

        LED_OFF;
        GPIOA->BSRR = 0x00050000;
        Wait_ms(2000);

        LED_ON;
        GPIOA->BSRR = 0x0000000A;
        Wait_ms(2000);

        LED_OFF;
        GPIOA->BSRR = 0x000A0000;
        Wait_ms(2000);
    }
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
            resp = LCD_Scroll1 ("Dexel Lighting DMX 2 channels 8 amps controller.");
        } while (resp != resp_finish);

        Wait_ms(2000);
    }
}



// void TF_MenuFunction (void)
// {
//     char s_lcd[20] = { 0 };
//     resp_t resp = resp_continue;
    
//     LCD_UtilsInit();
//     CTRL_BKL_ON;

//     Wait_ms(500);
//     LCD_PasswordReset ();
//     unsigned int new_psw = 0;
    
//     while (1)
//     {
//         sw_actions_t actions = selection_none;

//         if (CheckSET() > SW_NO)
//             actions = selection_enter;

//         if (CheckCCW())
//             actions = selection_dwn;

//         if (CheckCW())
//             actions = selection_up;
        
//         resp = LCD_Password ("Ingrese Password", actions, &new_psw);

//         if (resp == resp_selected)
//         {
//             if (new_psw != 0x00000022)
//             {
//                 LCD_1ER_RENGLON;
//                 Lcd_TransmitStr("El nuevo pass:  ");
//                 sprintf(s_lcd, "0x%08x        ", new_psw);
//                 LCD_2DO_RENGLON;
//                 Lcd_TransmitStr(s_lcd);
//                 Wait_ms(5000);
//             }
//             else
//             {
//                 LCD_1ER_RENGLON;
//                 Lcd_TransmitStr("    Password    ");                
//                 LCD_2DO_RENGLON;
//                 Lcd_TransmitStr("   Correcto!!!  ");
//                 Wait_ms(2000);
//             }

//             actions = selection_none;
//             LCD_PasswordReset ();
//         }
//         UpdateSwitches();
//         UpdateEncoder();
//     }
// }


void TF_Dmx_Packet (void)
{
    Usart1Config();
    TIM_14_Init();
    DMX_channel_selected = 1;
    DMX_channel_quantity = 2;
    DMX_EnableRx();

    if (Packet_Detected_Flag)
    {
        Packet_Detected_Flag = 0;
        LED_ON;
        Wait_ms(2);
        LED_OFF;
    }
}



//--- end of file ---//
