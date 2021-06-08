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
#include "temperatures.h"
#include "dsp.h"
#include "pwm.h"

#include "dmx_mode.h"
#include "manual_mode.h"
#include "menues.h"

#include "uart.h"

#include <stdio.h>
#include <string.h>


// Module Types Constants and Macros -------------------------------------------
typedef enum {
    MAIN_INIT,
    MAIN_HARD_INIT,
    MAIN_CHECK_CONF,
    MAIN_IN_DMX_MODE,
    MAIN_IN_MANUAL_MODE,
    MAIN_IN_OVERTEMP,
    MAIN_IN_MAIN_MENU
    
} main_state_e;


// Externals -------------------------------------------------------------------
extern char s_blank [];
parameters_typedef * pflash_mem = (parameters_typedef *) (unsigned int *) FLASH_PAGE_FOR_BKP;    //in flash
parameters_typedef mem_conf;    //in ram
// volatile unsigned char usart1_have_data = 0;

// for the dmx
volatile unsigned char dmx_buff_data[SIZEOF_DMX_BUFFER_DATA];
volatile unsigned char Packet_Detected_Flag = 0;
volatile unsigned short DMX_channel_selected = 0;
volatile unsigned char DMX_channel_quantity = 0;

// externals shared by modes
unsigned char mode_state;
volatile unsigned short mode_effect_timer;


// Globals ---------------------------------------------------------------------
//-- Timers globals ----------------------------------
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned short need_to_save_timer = 0;

//-- for the filters and outputs
volatile unsigned char channels_values_int [2] = { 0 };
volatile unsigned char enable_outputs_by_int = 0;
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;


// -- for the timeouts in the modes ----
void (* ptFTT ) (void) = NULL;

// -- for the memory -------------------
unsigned char need_to_save = 0;



// Module Private Functions ----------------------------------------------------
void TimingDelay_Decrement(void);
void EXTI4_15_IRQHandler(void);
void SysTickError (void);
void UpdateFiltersTest_Reset (void);
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val);


// Module Functions ------------------------------------------------------------
int main(void)
{
    //GPIO Configuration.
    GPIO_Config();

    //Systick Timer
    if (SysTick_Config(48000))
        SysTickError();

    // Begin Hardware Tests - check test_functions module
    // TF_Led();    //simple led functionality
    // TF_SW_UP();
    // TF_SW_DWN();
    // TF_SW_SEL();    
    // TF_lcdE();
    // TF_lcdRS();
    // TF_lcdData();
    // TF_lcdBklight();
    // TF_lcdBlink();
    // TF_lcdScroll();
    // TF_Dmx_Packet ();
    // TF_Dmx_Packet_Data ();
    TF_Pwm_Channels ();
    // End Hard Tests -------------------------------

    
    // Production Program ---------------------------
    sw_actions_t action = selection_none;
    resp_t resp = resp_continue;
    unsigned char ch_values [2] = { 0 };
    main_state_e main_state = MAIN_INIT;
    
    while (1)
    {
        switch (main_state)
        {
        case MAIN_INIT:
            // get saved config or create one for default
            if (pflash_mem->program_type != 0xff)
            {
                //memory with valid data
                memcpy(&mem_conf, pflash_mem, sizeof(parameters_typedef));
            }
            else
            {
                //hardware defaults
                mem_conf.temp_prot = TEMP_IN_70;    //70 degrees
                mem_conf.max_current_channels[0] = 255;
                mem_conf.max_current_channels[1] = 255;

                mem_conf.dmx_first_channel = 1;
                mem_conf.dmx_channel_quantity = 2;                
                mem_conf.program_type = DMX_MODE;
            }

            main_state++;
            break;

        case MAIN_HARD_INIT:

            //reseteo hardware
            DMX_DisableRx();

            //reseteo canales
            PWMChannelsReset();

            //limpio los filtros
            UpdateFiltersTest_Reset();

            main_state++;            
            break;

        case MAIN_CHECK_CONF:

            if (mem_conf.program_type == DMX_MODE)
            {
                //reception variables
                Packet_Detected_Flag = 0;
                DMX_channel_selected = mem_conf.dmx_first_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                // //Mode Timeout enable
                ptFTT = &DMXMode_UpdateTimers;

                //packet reception enable
                DMX_EnableRx();

#ifdef CHECK_FILTERS_BY_INT
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;
#endif
                
                DMXModeReset();
                main_state = MAIN_IN_DMX_MODE;
            }

            if (mem_conf.program_type == MANUAL_MODE)
            {
                //habilito salidas si estoy con int
                enable_outputs_by_int = 1;

                // //Mode Timeout enable
                ptFTT = &ManualMode_UpdateTimers;
                
                ManualModeReset();
                
                main_state = MAIN_IN_MANUAL_MODE;
            }
            break;

        case MAIN_IN_DMX_MODE:
            // Check encoder first
            action = CheckActions();
            
            resp = DMXMode (ch_values, action);

            if (resp == resp_change)
            {
                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = ch_values[n];
            }

            if (resp == resp_need_to_save)
            {
                need_to_save_timer = 10000;
                need_to_save = 1;
            }

            // if (CheckSET() > SW_MIN)
            //     main_state = MAIN_ENTERING_MAIN_MENU;
            
            break;

        case MAIN_IN_MANUAL_MODE:
            // Check encoder first
            action = CheckActions();

            resp = ManualMode (ch_values, action);

            if ((resp == resp_change) ||
                (resp == resp_change_all_up))    //fixed mode save and change
            {
                for (unsigned char n = 0; n < sizeof(ch_values); n++)
                    mem_conf.fixed_channels[n] = ch_values[n];

                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = ch_values[n];

                if (resp == resp_change_all_up)
                    resp = resp_need_to_save;
            }

            if (resp == resp_need_to_save)
            {
                need_to_save_timer = 10000;
                need_to_save = 1;
            }

            // if (CheckSET() > SW_MIN)
            //     main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_OVERTEMP:
            // main_state = MAIN_IN_OVERTEMP_B;
            break;

        // case MAIN_IN_OVERTEMP_B:
        //     if (CheckTempReconnect (Temp_Channel, mem_conf.temp_prot))
        //     {
        //         //reconnect
        //         main_state = MAIN_HARDWARE_INIT;
        //     }
        //     break;
            
//         case MAIN_ENTERING_MAIN_MENU:
//             //deshabilitar salidas hardware
//             DMX_DisableRx();

// #ifdef CHECK_FILTERS_BY_INT
//             enable_outputs_by_int = 0;
//             for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
//                 channels_values_int[n] = 0;
            
// #endif
//             //reseteo canales
//             PWMChannelsReset();

//             MainMenuReset();
            
//             main_state++;
//             break;

//         case MAIN_ENTERING_MAIN_MENU_WAIT_FREE:
//             if (CheckSET() == SW_NO)
//             {
//                 main_state++;
//             }
//             break;
            
        case MAIN_IN_MAIN_MENU:
            // Check encoder first
            // action = CheckActions();
            resp = MENU_Main(&mem_conf, action);

            if (resp == resp_need_to_save)
            {
#ifdef SAVE_FLASH_IMMEDIATE
                need_to_save_timer = 0;
#endif
#ifdef SAVE_FLASH_WITH_TIMEOUT
                need_to_save_timer = 10000;
#endif
                need_to_save = 1;
                main_state = MAIN_HARD_INIT;
            }
            
            if (resp == resp_finish)
                main_state = MAIN_HARD_INIT;

            // if (CheckSET() > SW_HALF)
            //     main_state = MAIN_ENTERING_HARDWARE_MENU;
            
            break;

        default:
            main_state = MAIN_INIT;
            break;
        }

        // memory savings after config
        if ((need_to_save) && (!need_to_save_timer))
        {
            __disable_irq();
            need_to_save = WriteConfigurations();
            __enable_irq();


            need_to_save = 0;
        }

        //the things that not depends on the main status
        UpdateSwitches();
        
    }    //end of while 1

    return 0;
}

//--- End of Main ---//


typedef enum {
    FILTERS_BKP_CHANNELS,
    FILTERS_LIMIT_EACH_CHANNEL,
    FILTERS_OUTPUTS,
    FILTERS_DUMMY1,
    FILTERS_DUMMY2
    
} filters_and_offsets_e;

filters_and_offsets_e filters_sm = FILTERS_BKP_CHANNELS;
unsigned char limit_output [2] = { 0 };
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned short calc = 0;    
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        limit_output[0] = *(ch_dmx_val + 0);
        limit_output[1] = *(ch_dmx_val + 1);
        filters_sm++;
        break;

    case FILTERS_LIMIT_EACH_CHANNEL:
        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc >>= 8;
        limit_output[0] = (unsigned char) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc >>= 8;
        limit_output[1] = (unsigned char) calc;

        filters_sm++;
        break;

    case FILTERS_OUTPUTS:
        // channel 1
        ch1_pwm = MA16_U16Circular (
            &st_sp1,
            PWM_Map_From_Dmx(*(limit_output + CH1_VAL_OFFSET))
            );
        PWM_Update_CH1(ch1_pwm);

        // channel 2
        ch2_pwm = MA16_U16Circular (
            &st_sp2,
            PWM_Map_From_Dmx(*(limit_output + CH2_VAL_OFFSET))
            );
        PWM_Update_CH2(ch2_pwm);

        filters_sm++;
        break;
        
    case FILTERS_DUMMY1:
        filters_sm++;
        break;

    case FILTERS_DUMMY2:
        filters_sm = FILTERS_BKP_CHANNELS;
        break;
        
    default:
        filters_sm = FILTERS_BKP_CHANNELS;
        break;
    }
}


void UpdateFiltersTest_Reset (void)
{
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
}


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    LCD_UpdateTimer();

    HARD_Timeouts();

    DMX_Int_Millis_Handler();

    // Modes Menus Timers
    if (ptFTT != NULL)
        ptFTT();

    if (enable_outputs_by_int)    
        CheckFiltersAndOffsets_SM(channels_values_int);
    
}


void EXTI4_15_IRQHandler(void)
{
    DMX_Int_Break_Handler();
    EXTI->PR |= 0x00000100;    //PA8
}


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

