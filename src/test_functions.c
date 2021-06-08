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
#include "pwm.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
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


void TF_Dmx_Packet (void)
{
    Usart1Config();
    TIM_14_Init();
    DMX_channel_selected = 1;
    DMX_channel_quantity = 2;
    DMX_EnableRx();

    while (1)
    {
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;
            LED_ON;
            Wait_ms(2);
            LED_OFF;
        }
    }
}


void TF_Dmx_Packet_Data (void)
{
    // Init LCD
    LCD_UtilsInit();
    CTRL_BKL_ON;
    LCD_ClearScreen();
    Wait_ms(1000);

    // Init DMX
    Usart1Config();
    TIM_14_Init();
    DMX_channel_selected = 1;
    DMX_channel_quantity = 2;
    DMX_EnableRx();

    unsigned char dmx_data1 = 0;
    unsigned char dmx_data2 = 0;    

    while (1)
    {
        if (Packet_Detected_Flag)
        {
            Packet_Detected_Flag = 0;
            LED_ON;

            if (dmx_buff_data[0] == 0)
            {
                char s_lcd [20] = { 0 };

                if (dmx_data1 != dmx_buff_data[1])
                {
                    sprintf(s_lcd, "ch1: %03d", dmx_buff_data[1]);
                    LCD_Writel1(s_lcd);
                    dmx_data1 = dmx_buff_data[1];
                }

                if (dmx_data2 != dmx_buff_data[2])
                {
                    sprintf(s_lcd, "ch2: %03d", dmx_buff_data[2]);
                    LCD_Writel2(s_lcd);
                    dmx_data2 = dmx_buff_data[2];
                }
            }
            
            LED_OFF;
        }
    }
}


void TF_Pwm_Channels (void)
{
    TIM_3_Init();

    PWM_Update_CH1(DUTY_20_PERCENT);
    PWM_Update_CH2(DUTY_50_PERCENT);
                   
    while (1);
    
}

//--- end of file ---//
