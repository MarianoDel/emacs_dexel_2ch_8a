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
#include "tim.h"
#include "uart.h"
#include "adc.h"
#include "dma.h"
#include "flash_program.h"
#include "core_cm0.h"

#include "switches_answers.h"
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
    MAIN_ENTERING_MAIN_MENU,
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

// externals for ADC
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
// volatile unsigned char seq_ready;


// Globals ---------------------------------------------------------------------
//-- Timers globals ----------------------------------
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned short need_to_save_timer = 0;

//-- for the filters and outputs
volatile unsigned char channels_values_int [2] = { 0 };
volatile unsigned char enable_outputs_by_int = 0;
unsigned short ch1_pwm = 0;
unsigned short ch2_pwm = 0;

#ifdef USE_FILTER_LENGHT_16
ma16_u16_data_obj_t st_sp1;
ma16_u16_data_obj_t st_sp2;
#endif

#ifdef USE_FILTER_LENGHT_32
ma32_u16_data_obj_t st_sp1;
ma32_u16_data_obj_t st_sp2;
#endif



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
unsigned short OutputDeltaPos (unsigned short sp, unsigned short current);
unsigned short OutputDeltaNeg (unsigned short sp, unsigned short current);

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
    // TF_ENA_CH1_ENA_CH2_SW_SEL ();
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
    // TF_Pwm_Channels ();
    // TF_Soft_Pwm_Channels ();
    // TF_Temp_Channel ();    
    // End Hard Tests -------------------------------

    // Hardware Inits. ---------------------------
    // Timer for PWM
    TIM_3_Init ();
    PWMChannelsReset ();
    EnablePreload_PWM1;
    EnablePreload_PWM2;

    // Soft PWM init
    PWM_Soft_Init();
    PWM_Soft_Update_Channel1(0);
    PWM_Soft_Update_Channel2(0);


    // Usart and Timer for DMX
    Usart1Config ();
    TIM_14_Init ();
    DMX_DisableRx ();

    // ADC & DMA for temp sense
    AdcConfig();
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR_EN;
    ADC1->CR |= ADC_CR_ADSTART;
    
    // LCD Init and Welcome Code
    LCD_UtilsInit();
    CTRL_BKL_ON;

    while (LCD_ShowBlink("Kirno Technology",
                         "  Smart Driver  ",
                         1,
                         BLINK_NO) != resp_finish);

    while (LCD_ShowBlink(" Dexel          ",
                         "  Lighting      ",
                         2,
                         BLINK_NO) != resp_finish);

    
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
                mem_conf.current_eight_amps = 0;
                mem_conf.channels_operation_mode = 0;

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

            //reviso si es 4 o 8Amps
            if (mem_conf.current_eight_amps)
                I_SEL_ON;
            else
                I_SEL_OFF;

            main_state++;            
            break;

        case MAIN_CHECK_CONF:

            if (mem_conf.program_type == DMX_MODE)
            {
                // reception variables
                DMX_channel_selected = mem_conf.dmx_first_channel;
                DMX_channel_quantity = mem_conf.dmx_channel_quantity;

                // Force first screen
                Packet_Detected_Flag = 1;
                dmx_buff_data[0] = 0;
                dmx_buff_data[1] = 0;
                dmx_buff_data[2] = 0;

                // Mode Timeout enable
                ptFTT = &DMXMode_UpdateTimers;

                // packet reception enable
                DMX_EnableRx();

                // habilito salidas si estoy con int
                enable_outputs_by_int = 1;
                
                DMXModeReset();
                main_state = MAIN_IN_DMX_MODE;
            }

            if (mem_conf.program_type == MANUAL_MODE)
            {
                // habilito salidas si estoy con int
                enable_outputs_by_int = 1;

                // Mode Timeout enable
                ptFTT = &ManualMode_UpdateTimers;

                for (unsigned char n = 0; n < sizeof(ch_values); n++)
                    ch_values[n] = mem_conf.fixed_channels[n];
                
                ManualModeReset();                
                main_state = MAIN_IN_MANUAL_MODE;
            }
            break;

        case MAIN_IN_DMX_MODE:
            // Check encoder first
            action = CheckActions();

            if (action != selection_back)
            {
                
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
            }
            else
                main_state = MAIN_ENTERING_MAIN_MENU;
            
            break;

        case MAIN_IN_MANUAL_MODE:
            // Check encoder first
            action = CheckActions();

            if (action != selection_back)
            {
                resp = ManualMode (ch_values, action);

                if (resp == resp_change)
                {
                    for (unsigned char n = 0; n < sizeof(ch_values); n++)
                    {
                        mem_conf.fixed_channels[n] = ch_values[n];
                        channels_values_int[n] = ch_values[n];
                    }
                }

                if (resp == resp_need_to_save)
                {
                    need_to_save_timer = 10000;
                    need_to_save = 1;
                }
            }
            else
                main_state = MAIN_ENTERING_MAIN_MENU;

            break;

        case MAIN_IN_OVERTEMP:
            if (Temp_Channel < TEMP_RECONNECT)
            {
                //reconnect
                main_state = MAIN_INIT;
            }
            break;
            
        case MAIN_ENTERING_MAIN_MENU:
            //deshabilitar salidas hardware
            DMX_DisableRx();

            enable_outputs_by_int = 0;
            for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                channels_values_int[n] = 0;
            
            //reseteo canales
            PWMChannelsReset();

            MENU_Main_Reset();
            
            main_state++;
            break;

        case MAIN_IN_MAIN_MENU:
            action = CheckActions();
            
            resp = MENU_Main(&mem_conf, action);

            if (resp == resp_need_to_save)
            {
                need_to_save_timer = 0;
                need_to_save = 1;
                
                main_state = MAIN_HARD_INIT;
            }
            
            if (resp == resp_finish)
                main_state = MAIN_HARD_INIT;

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

        // things that not depends on the main status
        UpdateSwitches();

        PWM_Soft_Update_Channels ();

#ifdef USE_TEMP_PROT
        if (main_state != MAIN_IN_OVERTEMP)
        {
            if (Temp_Channel > mem_conf.temp_prot)
            {
                //deshabilitar salidas hardware
                DMX_DisableRx();

                enable_outputs_by_int = 0;
                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = 0;
            
                //reseteo canales
                PWMChannelsReset();

                CTRL_FAN_ON;

                while (LCD_ShowBlink("  Overtemp!!!   ",
                                     " LEDs shutdown  ",
                                     1,
                                     BLINK_NO) != resp_finish);
                
                main_state = MAIN_IN_OVERTEMP;
            }
            else if (Temp_Channel > TEMP_IN_35)
                CTRL_FAN_ON;
            else if (Temp_Channel < TEMP_IN_30)
                CTRL_FAN_OFF;
        }
#endif    //USE_TEMP_PROT

        
        
        
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

unsigned char ch1_last_pwm = 0;
unsigned char ch1_shutdown = 0;
unsigned char ch1_powerup = 0;
unsigned char ch2_last_pwm = 0;
unsigned char ch2_shutdown = 0;
unsigned char ch2_powerup = 0;

filters_and_offsets_e filters_sm = FILTERS_BKP_CHANNELS;
unsigned short limit_output [2] = { 0 };
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned int calc = 0;
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        if (mem_conf.channels_operation_mode == 0)
        {
            unsigned char bright = 0;
            unsigned char temp0 = 0;
            unsigned char temp1 = 0;

            // backup and bright temp calcs
            // ch0 the bright ch1 the temp
            bright = *(ch_dmx_val + 0);
            temp0 = 255 - *(ch_dmx_val + 1);
            temp1 = 255 - temp0;
        
            calc = temp0 * bright;
            // calc >>= 8;
            // limit_output[0] = (unsigned char) calc;
            // calc >>= 4;    //4000 pts
            // calc >>= 3;    //8000 pts
            calc >>= 2;    //16000 pts        
            limit_output[0] = (unsigned short) calc;
        
            calc = temp1 * bright;
            // calc >>= 8;
            // limit_output[1] = (unsigned char) calc;
            // calc >>= 4;    //4000 pts        
            // calc >>= 3;    //8000 pts
            calc >>= 2;    //16000 pts        
            limit_output[1] = (unsigned short) calc;
        }

        if (mem_conf.channels_operation_mode == 1)
        {
            calc = *(ch_dmx_val + 0);
            // calc <<= 4;    //4000 pts
            // calc <<= 5;    //8000 pts
            calc <<= 6;    //16000 pts        
            limit_output[0] = (unsigned short) calc;

            calc = *(ch_dmx_val + 1);
            // calc <<= 4;    //4000 pts
            // calc <<= 5;    //8000 pts
            calc <<= 6;    //16000 pts        
            limit_output[1] = (unsigned short) calc;
        }

        filters_sm++;
        break;

    case FILTERS_LIMIT_EACH_CHANNEL:        
        // the limit is the same for the two channels        
        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc >>= 8;
        limit_output[0] = (unsigned short) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc >>= 8;
        limit_output[1] = (unsigned short) calc;

        filters_sm++;
        break;

    case FILTERS_OUTPUTS:
#ifdef USE_FILTER_LENGHT_16
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
#endif
#ifdef USE_FILTER_LENGHT_32
        // channel 1
        ch1_pwm = MA32_U16Circular (&st_sp1, *(limit_output + CH1_VAL_OFFSET));
        PWM_Update_CH1(ch1_pwm);

        // channel 2
        ch2_pwm = MA32_U16Circular (&st_sp2, *(limit_output + CH2_VAL_OFFSET));
        PWM_Update_CH2(ch2_pwm);

        // check for led shutdown
#ifdef HARDWARE_VERSION_1_1
        // if ((ch1_pwm == 0) && (!ch1_shutdowm))
        //     ch1_shutdowm = 1;

        // if ((ch2_pwm == 0) && (!ch2_shutdowm))
        //     ch2_shutdowm = 1;

        if (ch1_pwm == 0)
        {
            if (!ch1_shutdown)
            {
                ch1_last_pwm = 100;
                ch1_shutdown = 1;
            }
        }
        else
        {
            ch1_powerup = 1;
            ch1_shutdown = 0;
        }

        if (ch2_pwm == 0)
        {
            if (!ch2_shutdown)
            {
                ch2_last_pwm = 100;
                ch2_shutdown = 1;
            }
        }
        else
        {
            ch2_powerup = 1;
            ch2_shutdown = 0;
        }

#endif
        
#endif    //USE_FILTER_LENGHT_32
#ifdef USE_NO_FILTER
        // channel 1
        if (*(limit_output + CH1_VAL_OFFSET) > ch1_pwm)
        {
            unsigned short setpoint_ch1 = *(limit_output + CH1_VAL_OFFSET);
            ch1_pwm = OutputDeltaPos (setpoint_ch1, ch1_pwm);
        }
        else if (*(limit_output + CH1_VAL_OFFSET) < ch1_pwm)
        {
            unsigned short setpoint_ch1 = *(limit_output + CH1_VAL_OFFSET);
            ch1_pwm = OutputDeltaNeg (setpoint_ch1, ch1_pwm);
        }
            
        PWM_Update_CH1(ch1_pwm);

        // channel 2
        if (*(limit_output + CH2_VAL_OFFSET) > ch2_pwm)
        {
            unsigned short setpoint_ch2 = *(limit_output + CH2_VAL_OFFSET);
            ch2_pwm = OutputDeltaPos (setpoint_ch2, ch2_pwm);
        }
        else if (*(limit_output + CH2_VAL_OFFSET) < ch2_pwm)
        {
            unsigned short setpoint_ch2 = *(limit_output + CH2_VAL_OFFSET);
            ch2_pwm = OutputDeltaNeg (setpoint_ch2, ch2_pwm);
        }

        PWM_Update_CH2(ch2_pwm);        
#endif
        // if (LED)
        //     LED_OFF;
        // else
        //     LED_ON;

        filters_sm++;
        break;
        
    case FILTERS_DUMMY1:
#ifdef HARDWARE_VERSION_1_1
        // shutting down sequence
        if ((ch1_shutdown) && (ch1_last_pwm))
        {
            ch1_last_pwm--;
            PWM_Soft_Update_Channel1(ch1_last_pwm);
        }

        if ((ch2_shutdown) && (ch2_last_pwm))
        {
            ch2_last_pwm--;
            PWM_Soft_Update_Channel2(ch2_last_pwm);
        }

        // powerup sequence
        if ((ch1_powerup) && (ch1_last_pwm < 100))
        {
            ch1_last_pwm++;
            PWM_Soft_Update_Channel1(ch1_last_pwm);
        }

        if ((ch2_powerup) && (ch2_last_pwm < 100))
        {
            ch2_last_pwm++;
            PWM_Soft_Update_Channel2(ch2_last_pwm);
        }
        
#endif

        
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


unsigned short OutputDeltaPos (unsigned short sp, unsigned short current)
{
    if (current > 4000)
    {
        if (sp > (current + 5))
            current += 5;
        else
            current = sp;
    }
    else if (current > 2000)
    {
        if (sp > (current + 2))
            current += 2;
        else
            current = sp;
    }
    else
        current++;

    return current;
}


unsigned short OutputDeltaNeg (unsigned short sp, unsigned short current)
{
    if (current > 4000)
    {
        if (sp < (current - 5))
            current -= 5;
        else
            current = sp;
    }
    else if (current > 2000)
    {
        if (sp < (current - 2))
            current -= 2;
        else
            current = sp;
    }
    else
        current--;

    return current;
}



void UpdateFiltersTest_Reset (void)
{
#ifdef USE_FILTER_LENGHT_16    
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
#endif
#ifdef USE_FILTER_LENGHT_32
    MA32_U16Circular_Reset(&st_sp1);
    MA32_U16Circular_Reset(&st_sp2);
#endif    
}


void TimingDelay_Decrement(void)
{
    if (wait_ms_var)
        wait_ms_var--;

    if (timer_standby)
        timer_standby--;

    if (need_to_save_timer)
        need_to_save_timer--;
    
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

