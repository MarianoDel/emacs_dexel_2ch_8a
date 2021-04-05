//------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MAIN.C ###################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"
#include "gpio.h"

#include "core_cm0.h"
#include "tim.h"
#include "flash_program.h"

#include "switches_answers.h"
#include "lcd.h"
#include "lcd_utils.h"
#include "test_functions.h"
#include "parameters.h"
#include "dmx_receiver.h"

#include "spi.h"
#include "uart.h"

#include <stdio.h>
#include <string.h>


// Externals -------------------------------------------------------------------
extern char s_blank [];
parameters_typedef * pmem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;    //in flash
parameters_typedef mem_conf;    //in ram
volatile unsigned char usart1_have_data = 0;

// for the dmx
volatile unsigned char dmx_buff_data[SIZEOF_DMX_BUFFER_DATA];
volatile unsigned char Packet_Detected_Flag = 0;
volatile unsigned short DMX_channel_selected = 0;
volatile unsigned char DMX_channel_quantity = 0;

// Globals ---------------------------------------------------------------------
//-- Timers globals ----------------------------------
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;



// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void EXTI4_15_IRQHandler(void);
void SysTickError (void);


// Module Functions ------------------------------------------------------------
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //Systick Timer
    if (SysTick_Config(48000))
        SysTickError();

    // Begin Hardware Tests - check test_functions module
    TF_Led();    //simple led functionality
    // TF_SW_UP();
    // TF_SW_DWN();
    // TF_SW_SEL();    
    // TF_lcdE();
    // TF_lcdRS();
    // TF_lcdData();
    // TF_lcdBklight();
    // TF_lcdBlink();
    // TF_lcdScroll();
    TF_Dmx_Packet ();
    // TF_MenuFunction();
// #ifdef WITH_EXTI    
//     TF_zcd_by_int();
// #endif
    // End Hard Tests -------------------------------

    
    // Production Program ---------------------------
//     TIM16_Init();    //synchro with relay
    
//     main_state_t main_state = MAIN_INIT;
//     char s_lcd [100] = { 0 };
//     resp_t resp = resp_continue;
//     unsigned char lcd_l1_was_on = 0;
//     unsigned char last_secs = 0;
//     unsigned char barrita = 0;

//     // para el lector de tarjetas
//     Usart1Config();
//     Usart1Send("usart on\n");
//     SPI_Config();
//     Usart1Send("spi on\n");


//     LCD_UtilsInit();
//     LCD_BigNumbersInit();
    
//     while (1)
//     {
//         switch(main_state)
//         {
//         case MAIN_INIT:    //arranque inicial
//             LCD_ClearScreen();

//             // configuracion desde la memoria
//             memcpy(&configurations_in_mem, pmem, sizeof(mem_bkp_t));

//             // check empty mem
//             if (configurations_in_mem.treatment_time_min == 0xff)
//             {
//                 //mem empty go for defaults
//                 configurations_in_mem.treatment_time_min = 10;
//                 configurations_in_mem.alarms_onoff = 1;
//                 configurations_in_mem.ticker_onoff = 1;        
//                 // configurations_in_mem.ticker_time = 60000;
//                 configurations_in_mem.ticker_time = 2000;
// #ifdef ON_EMPTY_MEM_INIT_CARD_MODE
//                 // configurations_in_mem.operation_mode = NORMAL_MODE;
//                 configurations_in_mem.operation_mode = CARD_MODE;
// #endif
// #ifdef ON_EMPTY_MEM_INIT_NORMAL_MODE
//                 configurations_in_mem.operation_mode = NORMAL_MODE;
//                 // configurations_in_mem.operation_mode = CARD_MODE;
// #endif
//                 //el dummy1 lo uso como habilitacion de O3 en tarjeta
//                 configurations_in_mem.dummy1 = 0;
//             }
            
//             if (configurations_in_mem.operation_mode == CARD_MODE)
//             {
//                 Card_Mode_Standby_Reset ();
//                 main_state = MAIN_CARD_MODE_STANDBY;
//             }
//             else
//             {
//                 Normal_Mode_Standby_Reset ();
//                 main_state = MAIN_NORMAL_MODE_STANDBY;
//             }
            
//             TIM16Enable();
//             break;

//         case MAIN_INIT_FROM_TREATMENT:    //arranque cuando termino sesiones
//             if (configurations_in_mem.operation_mode == CARD_MODE)
//             {
//                 Card_Mode_Standby_Init ();
//                 main_state = MAIN_CARD_MODE_STANDBY;
//             }
//             else
//             {
//                 Normal_Mode_Standby_Reset ();
//                 main_state = MAIN_NORMAL_MODE_STANDBY;
//             }
//             break;
            
//         case MAIN_NORMAL_MODE_STANDBY:
//             Normal_Mode_Standby (&configurations_in_mem);
            
//             if (CheckO3() > SW_NO)
//             {
//                 treatment_running_mins = configurations_in_mem.treatment_time_min;
//                 treatment_running_secs = 0;
                
//                 main_state = MAIN_START_TREATMENT;
//             }

//             if (CheckSET() > SW_NO)
//                 main_state = MAIN_ENTERING_SET_OR_MENU;

//             break;

//         case MAIN_CARD_MODE_STANDBY:
//             Card_Mode_Standby (&configurations_in_mem);

//             if ((configurations_in_mem.dummy1) &&
//                 (CheckO3() > SW_NO))
//             {
//                 treatment_running_mins = configurations_in_mem.dummy1;
//                 configurations_in_mem.dummy1 = 0;
//                 treatment_running_secs = 0;
                
//                 main_state = MAIN_START_TREATMENT;
//             }

//             if (CheckSET() > SW_MIN)
//             {
//                 timer_standby = 20000;
//                 MENU_Main_Reset();
//                 main_state = MAIN_IN_MAIN_MENU;
//             }            
//             break;
            
//         case MAIN_START_TREATMENT:
//             if (CheckO3() == SW_NO)
//             {
//                 LCD_ClearScreen();
//                 Lcd_SetDDRAM(14);
//                 Lcd_TransmitStr("O3");
//                 if (configurations_in_mem.alarms_onoff)
//                     BuzzerCommands(BUZZER_LONG_CMD, 1);
                
//                 timer_ticker = configurations_in_mem.ticker_time;
//                 RelayOn();

//                 treatment_running = 1;
//                 barrita = 0;
//                 ChangeLed(LED_TREATMENT_GENERATING);
//                 main_state = MAIN_IN_TREATMENT;
//             }
//             break;

//         case MAIN_IN_TREATMENT:
//             if (configurations_in_mem.ticker_onoff)
//             {
//                 if (!timer_ticker)
//                 {
//                     BuzzerCommands(BUZZER_SHORT_CMD, 1);
//                     timer_ticker = configurations_in_mem.ticker_time;
//                 }
//             }

//             if (CheckO3() > SW_NO)
//             {
//                 //freno contador aca?
//                 treatment_running = 0;
//                 main_state = MAIN_GO_PAUSE;
//             }

//             //Update del Display timer
//             if (last_secs != treatment_running_secs)
//             {
//                 last_secs = treatment_running_secs;                
//                 sprintf(s_lcd, "%02d", treatment_running_mins);
//                 LCD_BigNumbers(0, s_lcd[0] - '0');
//                 LCD_BigNumbers(3, s_lcd[1] - '0');

//                 sprintf(s_lcd, "%02d", treatment_running_secs);
//                 LCD_BigNumbers(7, s_lcd[0] - '0');
//                 LCD_BigNumbers(10, s_lcd[1] - '0');

//                 // y los dos puntos en los impares
//                 if (last_secs & 0x01)
//                 {
//                     Lcd_SetDDRAM(0x06);
//                     Lcd_senddata(0xa5);
//                     Lcd_SetDDRAM(0x46);
//                     Lcd_senddata(0xa5);
//                 }
//                 else
//                 {
//                     Lcd_SetDDRAM(0x06);
//                     Lcd_senddata(' ');
//                     Lcd_SetDDRAM(0x46);
//                     Lcd_senddata(' ');
//                 }
//             }

//             //Update del Display barrita
//             if (!timer_standby)
//             {
//                 timer_standby = 300;
                
//                 switch(barrita)
//                 {
//                     case 0:
//                         Lcd_SetDDRAM (0x4e);
//                         Lcd_senddata(0xb0);
//                         Lcd_senddata(0xb0);                        
//                         barrita++;
//                         break;

//                     case 1:
//                         Lcd_SetDDRAM (0x4e);
//                         Lcd_senddata(0x2f);
//                         Lcd_senddata(0x2f);                        
//                         barrita++;
//                         break;

//                     case 2:
//                         Lcd_SetDDRAM (0x4e);
//                         Lcd_senddata(0x7c);
//                         Lcd_senddata(0x7c);                        
//                         barrita = 0;
//                         break;

//                 default:
//                     barrita = 0;
//                     break;
//                 }
//             }

//             //terminacion de tratamiento
//             if ((treatment_running_mins == 0) && (treatment_running_secs == 0))
//                 main_state = MAIN_ENDING_TREATMENT;
            
//             break;
            
//         case MAIN_GO_PAUSE:
//             if (CheckO3() == SW_NO)
//             {
//                 LCD_ClearScreen();
//                 if (configurations_in_mem.alarms_onoff)
//                     BuzzerCommands(BUZZER_SHORT_CMD, 3);
                
//                 RelayOff();
//                 // LCD_Scroll2Reset();
//                 barrita = 0;
//                 ChangeLed(LED_TREATMENT_PAUSED);
//                 main_state = MAIN_PAUSED;
//             }
//             break;

//         case MAIN_PAUSED:

//             if (!timer_standby)
//             {
//                 if (!lcd_l1_was_on)
//                 {
//                     LCD_Writel1("  O3 en Pausa  ");
//                     timer_standby = 1000;
//                     lcd_l1_was_on = 1;
//                 }
//                 else
//                 {
//                     LCD_Writel1(s_blank);
//                     timer_standby = 1000;
//                     lcd_l1_was_on = 0;                    
//                 }
//             }

//             switch (barrita)
//             {
//             case 0:
//                 LCD_Writel2("O3 si continua  ");
//                 timer_barrita = 1200;
//                 barrita++;
//                 break;

//             case 1:
//                 if (!timer_barrita)
//                 {
//                     LCD_Writel2("SET si termina  ");
//                     timer_barrita = 1200;
//                     barrita++;
//                 }
//                 break;

//             case 2:
//                 if (!timer_barrita)
//                     barrita = 0;

//                 break;
                
//             default:
//                 barrita = 0;
//                 break;
//             }
            

//             if (CheckO3() > SW_NO)
//                 main_state = MAIN_RESUMING;

//             if (CheckSET() > SW_NO)
//                 main_state = MAIN_ENDING_TREATMENT;

//             break;

//         case MAIN_RESUMING:
//             if (CheckO3() == SW_NO)
//                 main_state = MAIN_START_TREATMENT;

//             break;

//         case MAIN_ENDING_TREATMENT:
//             if (CheckSET() == SW_NO)
//             {
//                 if (configurations_in_mem.alarms_onoff)
//                     // BuzzerCommands(BUZZER_LONG_CMD, 3);
//                     BuzzerCommands(BUZZER_MULTI_CMD, 0);

//                 RelayOff();
//                 ChangeLed(LED_TREATMENT_STANDBY);
//                 main_state = MAIN_INIT_FROM_TREATMENT;
//             }
//             break;

//         case MAIN_ENTERING_SET_OR_MENU:
//             LCD_1ER_RENGLON;
//             Lcd_TransmitStr(" Entrando en    ");    
//             LCD_2DO_RENGLON;
//             Lcd_TransmitStr(" configuracion  ");

//             timer_standby = 5000;
//             main_state = MAIN_WAIT_SET_OR_MENU;
//             break;
                
//         case MAIN_WAIT_SET_OR_MENU:
//             if ((CheckSET() == SW_NO) && (timer_standby))
//             {
//                 timer_standby = 20000;
//                 MENU_Encendido_Reset();
//                 main_state = MAIN_SET_TIME;
//             }

//             if (!timer_standby)
//             {
//                 if (CheckSET() > SW_NO)
//                 {
//                     timer_standby = 20000;
//                     MENU_Main_Reset();
//                     main_state = MAIN_IN_MAIN_MENU;
//                 }
//                 else
//                     main_state = MAIN_INIT;
//             }
//             break;

//         case MAIN_SET_TIME:
//             resp = MENU_Encendido(&configurations_in_mem);

//             if (resp == resp_change)
//                 timer_standby = 20000;
            
//             if (resp == resp_finish)
//             {
//                 timer_standby = 0;
//                 main_state = MAIN_INIT_FROM_TREATMENT;
//             }

//             if (!timer_standby)    //time for conf ended
//                 main_state = MAIN_INIT_FROM_TREATMENT;
            
//             break;

//         case MAIN_IN_MAIN_MENU:
//             resp = MENU_Main(&configurations_in_mem);

//             if (resp == resp_change)
//                 timer_standby = 20000;

//             if (!timer_standby)
//                 main_state = MAIN_INIT;

//             if (resp == resp_finish)
//             {
//                 timer_standby = 0;
//                 main_state = MAIN_INIT;
//             }
            
//             if (resp == resp_need_to_save)
//             {
//                 __disable_irq();
//                 resp = WriteConfigurations();
//                 __enable_irq();

//                 if (resp)
//                 {
//                     LCD_Writel1("Memoria grabada ");
//                     LCD_Writel2(" correctamente  ");
//                 }
//                 else
//                 {
//                     LCD_Writel1("  Problemas!!!  ");
//                     LCD_Writel2("Memoria no graba");
//                 }
//                 Wait_ms(1000);                    

//                 timer_standby = 0;
//                 main_state = MAIN_INIT;
//             }
//             break;
            
//         default:
//             main_state = MAIN_INIT;
//             break;
//         }

//         UpdateLed();
//         UpdateSwitches();
//     }    
}
//--- End of Main ---//


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    LCD_UpdateTimer();

    HARD_Timeouts();

    DMX_Int_Millis_Handler();
}


#ifdef WITH_EXTI
void EXTI4_15_IRQHandler(void)
{
    DMX_Int_Break_Handler();
    EXTI->PR |= 0x00000100;    //PA8
}
#endif


void SysTickError (void)
{
    //Capture systick error...
    while (1)
    {
        if (LED)
            LED_OFF;
        else
            LED_ON;

        for (unsigned char i = 0; i < 255; i++)
        {
            asm ("nop \n\t"
                 "nop \n\t"
                 "nop \n\t" );
        }
    }
}

//--- end of file ---//

