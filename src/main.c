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
#include "usart.h"
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
#include "filters_and_offsets.h"

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
    MAIN_DMX_MODE_INIT,
    MAIN_MANUAL_MODE_INIT,
    MAIN_IN_DMX_MODE,
    MAIN_IN_MANUAL_MODE,
    MAIN_IN_OVERTEMP,
    MAIN_IN_OVERCURRENT,
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
volatile unsigned char dmx_receive_flag = 0;

// externals shared by modes
unsigned char mode_state;
volatile unsigned short mode_effect_timer;

// externals for ADC
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];
// volatile unsigned char seq_ready;

// externals for overcurrent protection
extern volatile unsigned char f_channel_2_int;
extern volatile unsigned char f_channel_4_int;

// Globals ---------------------------------------------------------------------
//-- Timers globals ----------------------------------
volatile unsigned short timer_standby = 0;
volatile unsigned short wait_ms_var = 0;
volatile unsigned short need_to_save_timer = 0;

//-- for the filters and outputs
volatile unsigned char channels_values_int [2] = { 0 };
volatile unsigned char enable_outputs_by_int = 0;


// -- for the timeouts in the modes ----
void (* ptFTT ) (void) = NULL;

// -- for the memory -------------------
unsigned char need_to_save = 0;



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
    // TF_F_Channels_As_Pwm ();
    // TF_F_Channels_As_ICapture_2 ();
    // TF_F_Channels_As_ICapture_4 ();        
    // TF_Temp_Channel ();    
    // End Hard Tests -------------------------------

    // Hardware Inits. ---------------------------
    // Timer for PWM and ENA
    TIM_3_Init ();
    PWMChannelsReset ();
    EnablePreload_PWM1;
    EnablePreload_PWM2;
    
    // Timer for overcurrent protection
    TIM_1_Init ();
#ifdef HARDWARE_VERSION_1_2
    PWM_Update_ENA1 (0);
    PWM_Update_ENA2 (0);
    EnablePreload_ENA1;
    EnablePreload_ENA2;
#endif

#ifdef HARDWARE_VERSION_1_1
#ifndef USE_F_CHNLS_FOR_FREQ_DETECT
    PWM_Update_ENA1 (0);
    PWM_Update_ENA2 (0);
    EnablePreload_ENA1;
    EnablePreload_ENA2;
#endif
#endif
    
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
    unsigned char packet_cnt = 0;
    
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
                mem_conf.channels_operation_mode = CCT1_MODE;

                mem_conf.dmx_first_channel = 1;
                mem_conf.dmx_channel_quantity = 2;                
                // mem_conf.program_type = DMX_MODE;
            }

            main_state++;
            break;

        case MAIN_HARD_INIT:

            //reseteo hardware
            DMX_DisableRx();

            //reseteo canales
            PWMChannelsReset();

            //limpio los filtros
            FiltersAndOffsets_Filters_Reset();

            //reviso si es 4 o 8Amps
            if (mem_conf.current_eight_amps)
                I_SEL_ON;
            else
                I_SEL_OFF;

            // packet reception enable
            DMX_EnableRx();
            timer_standby = 1000;    //one second for dmx detection
            mem_conf.program_type = AUTODETECT_MODE;

            main_state++;
            break;

        case MAIN_CHECK_CONF:
            if (Packet_Detected_Flag)
            {
                Packet_Detected_Flag = 0;
                packet_cnt++;
            }

            if ((packet_cnt > 5) &&
                (timer_standby))
            {
                mem_conf.program_type = DMX_MODE;
                main_state = MAIN_DMX_MODE_INIT;
            }
            else if (!timer_standby)
            {
                mem_conf.program_type = MANUAL_MODE;
                main_state = MAIN_MANUAL_MODE_INIT;
            }
            break;

        case MAIN_DMX_MODE_INIT:
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
            packet_cnt = 0;    // reset packet counter for autodetection
            break;

        case MAIN_MANUAL_MODE_INIT:
            // packet reception disable, check for colors
            DMX_DisableRx();

            // habilito salidas si estoy con int
            enable_outputs_by_int = 1;

            // Mode Timeout enable
            ptFTT = &ManualMode_UpdateTimers;

            for (unsigned char n = 0; n < sizeof(ch_values); n++)
            {
                ch_values[n] = mem_conf.fixed_channels[n];
                channels_values_int[n] = ch_values[n];    // set the first values
            }
                
            ManualModeReset();                
            main_state = MAIN_IN_MANUAL_MODE;
            packet_cnt = 0;    // reset packet counter for autodetection
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

            // Manual mode autodetection
            if (DMXGetPacketTimer () == 0)
            {
                if (!timer_standby)
                {
                    if (packet_cnt < 2)
                    {
                        packet_cnt++;
                        timer_standby = 200;
                    }
                    else
                    {
                        // manual detection
                        main_state = MAIN_MANUAL_MODE_INIT;
                        mem_conf.program_type = MANUAL_MODE;
                    }
                }
            }
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

            // Dmx presence autodetection
            if (dmx_receive_flag)
            {
                dmx_receive_flag = 0;
                packet_cnt++;
                timer_standby = 1000;
            }

            if (packet_cnt > 5)
            {
                if (timer_standby)
                {
                    // dmx detection
                    main_state = MAIN_DMX_MODE_INIT;
                    mem_conf.program_type = DMX_MODE;                    
                }
                else
                {
                    // dmx not present, reset the counter
                    packet_cnt = 0;
                }
            }
            break;

        case MAIN_IN_OVERTEMP:
            if (Temp_Channel < TEMP_RECONNECT)
            {
                //reconnect
                main_state = MAIN_INIT;
            }
            break;

        case MAIN_IN_OVERCURRENT:
            
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
#ifdef USE_CTROL_FAN_ALWAYS_ON
        CTRL_FAN_ON;
#endif

#ifdef USE_OVERCURRENT_PROT
        if (main_state != MAIN_IN_OVERCURRENT)
        {
            if ((f_channel_2_int > 100) ||
                (f_channel_4_int > 100))
            {
                //deshabilitar salidas hardware
                DMX_DisableRx();

                enable_outputs_by_int = 0;
                for (unsigned char n = 0; n < sizeof(channels_values_int); n++)
                    channels_values_int[n] = 0;
            
                //reseteo canales
                PWMChannelsReset();

                CTRL_FAN_ON;

                while (LCD_ShowBlink("LEDs Overcurrent",
                                     " reset the unit!",
                                     1,
                                     BLINK_NO) != resp_finish);
                
                main_state = MAIN_IN_OVERCURRENT;
            }
        }        
#endif
        
    }    //end of while 1

    return 0;
}

//--- End of Main ---//


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
        // CheckFiltersAndOffsets_SM (channels_values_int);
        FiltersAndOffsets_Post_Mapping_SM (channels_values_int);
    
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

