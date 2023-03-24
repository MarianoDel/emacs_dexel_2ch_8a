//----------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### FILTERS_AND_OFFSETS.C ########################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "filters_and_offsets.h"
#include "dsp.h"
#include "parameters.h"
#include "pwm.h"
#include "tim.h"


// Externals -------------------------------------------------------------------
extern parameters_typedef mem_conf;


// Globals ---------------------------------------------------------------------
ma32_u16_data_obj_t st_sp1;
ma32_u16_data_obj_t st_sp2;
ma32_u16_data_obj_t st_ena1;
ma32_u16_data_obj_t st_ena2;


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------

// Funcion original
typedef enum {
    FILTERS_BKP_CHANNELS,
    FILTERS_LIMIT_EACH_CHANNEL,
    FILTERS_OUTPUTS,
    FILTERS_DUMMY1,
    FILTERS_DUMMY2
    
} filters_and_offsets_e;


#define CHNL_ENA_DOWN    0
#define CHNL_ENA_POWERDWN    1
#define CHNL_ENA_POWERUP    2
#define CHNL_ENA_UP    3

unsigned char ch1_enable_state = 0;
unsigned char ch2_enable_state = 0;
unsigned short ch1_last_pwm = 0;
unsigned short ch2_last_pwm = 0;

filters_and_offsets_e filters_sm = FILTERS_BKP_CHANNELS;
unsigned short limit_output [2] = { 0 };
unsigned short ena_output [2] = { 0 };
#define CH1_VAL_OFFSET    0
#define CH2_VAL_OFFSET    1
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned int calc = 0;
    unsigned short ch1_pwm, ch2_pwm;
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        if ((mem_conf.channels_operation_mode == CCT1_MODE) ||
            (mem_conf.channels_operation_mode == CCT2_MODE))
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
#ifdef USE_PWM_4000_FREQ_4KHZ
            calc >>= 4;    //4000 pts
#endif
#ifdef USE_PWM_8000_FREQ_2KHZ            
            calc >>= 3;    //8000 pts
#endif
#ifdef USE_PWM_16000_FREQ_1KHZ
            calc >>= 2;    //16000 pts
#endif
            limit_output[0] = (unsigned short) calc;
        
            calc = temp1 * bright;
            // calc >>= 8;
            // limit_output[1] = (unsigned char) calc;
#ifdef USE_PWM_4000_FREQ_4KHZ
            calc >>= 4;    //4000 pts
#endif
#ifdef USE_PWM_8000_FREQ_2KHZ            
            calc >>= 3;    //8000 pts
#endif
#ifdef USE_PWM_16000_FREQ_1KHZ
            calc >>= 2;    //16000 pts
#endif
            limit_output[1] = (unsigned short) calc;
        }

        if (mem_conf.channels_operation_mode == ONECH_MODE)
        {
            calc = *(ch_dmx_val + 0);
#ifdef USE_PWM_4000_FREQ_4KHZ
            calc <<= 4;    //4000 pts
#endif
#ifdef USE_PWM_8000_FREQ_2KHZ            
            calc <<= 5;    //8000 pts
#endif
#ifdef USE_PWM_16000_FREQ_1KHZ
            calc <<= 6;    //16000 pts        
#endif
            limit_output[0] = (unsigned short) calc;
            limit_output[1] = limit_output[0];
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
        // channel 1
        ch1_pwm = MA32_U16Circular (&st_sp1, *(limit_output + CH1_VAL_OFFSET));
        if (ch1_enable_state != CHNL_ENA_POWERDWN)
            PWM_Update_CH1(ch1_pwm);

        // channel 2
        ch2_pwm = MA32_U16Circular (&st_sp2, *(limit_output + CH2_VAL_OFFSET));
        if (ch2_enable_state != CHNL_ENA_POWERDWN)
            PWM_Update_CH2(ch2_pwm);

        // check for led shutdown
        if ((!ch1_pwm) && (ch1_enable_state == CHNL_ENA_UP))
        {
            ch1_enable_state = CHNL_ENA_POWERDWN;
        }

        if ((ch1_pwm) && (ch1_enable_state == CHNL_ENA_DOWN))
        {
            ch1_enable_state = CHNL_ENA_POWERUP;
        }

        if ((!ch2_pwm) && (ch2_enable_state == CHNL_ENA_UP))
        {
            ch2_enable_state = CHNL_ENA_POWERDWN;
        }

        if ((ch2_pwm) && (ch2_enable_state == CHNL_ENA_DOWN))
        {
            ch2_enable_state = CHNL_ENA_POWERUP;
        }

        filters_sm++;
        break;
        
    case FILTERS_DUMMY1:
        // UP sequence
        if (ch1_enable_state == CHNL_ENA_POWERUP)
        {
            if (ch1_last_pwm <= 1000)
            {
                ch1_last_pwm += 10;
#ifdef USE_F_CHNLS_FOR_ENABLE
                PWM_Update_ENA1(ch1_last_pwm);
#else
                ENA_CH1_ON;
#endif
            }
            else
                ch1_enable_state = CHNL_ENA_UP;
        }

        // DWN sequence
        if (ch1_enable_state == CHNL_ENA_POWERDWN)
        {
            if (ch1_last_pwm > 0)
            {
                ch1_last_pwm -= 10;
#ifdef USE_F_CHNLS_FOR_ENABLE                
                PWM_Update_ENA1(ch1_last_pwm);
#else
                ENA_CH1_OFF;
#endif
            }
            else
                ch1_enable_state = CHNL_ENA_DOWN;
        }

        // UP sequence
        if (ch2_enable_state == CHNL_ENA_POWERUP)
        {
            if (ch2_last_pwm <= 1000)
            {
                ch2_last_pwm += 10;
#ifdef USE_F_CHNLS_FOR_ENABLE
                PWM_Update_ENA2(ch2_last_pwm);
#else
                ENA_CH2_ON;
#endif
            }
            else
                ch2_enable_state = CHNL_ENA_UP;
        }

        // DWN sequence
        if (ch2_enable_state == CHNL_ENA_POWERDWN)
        {
            if (ch2_last_pwm > 0)
            {
                ch2_last_pwm -= 10;
#ifdef USE_F_CHNLS_FOR_ENABLE
                PWM_Update_ENA2(ch2_last_pwm);
#else
                ENA_CH2_OFF;
#endif
            }
            else
                ch2_enable_state = CHNL_ENA_DOWN;
        }
        
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


void FiltersAndOffsets_Pre_Mapping_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned int calc = 0;
    unsigned short ch1_pwm, ch2_pwm;
#ifdef USE_F_CHNLS_FOR_ENABLE
    unsigned short ena1_pwm, ena2_pwm;
#endif
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        if ((mem_conf.channels_operation_mode == CCT1_MODE) ||
            (mem_conf.channels_operation_mode == CCT2_MODE))
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
            calc >>= 8;    // to 255 again
            limit_output[0] = (unsigned short) calc;
        
            calc = temp1 * bright;
            calc >>= 8;    // to 255 again            
            limit_output[1] = (unsigned short) calc;
        }

        if (mem_conf.channels_operation_mode == ONECH_MODE)
        {
            limit_output[0] = (unsigned short) calc;
            limit_output[1] = limit_output[0];
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

    case FILTERS_OUTPUTS:    // do mapping here, before the filters

        PWM_Map_Pre_Filter ((unsigned char) limit_output[0],
                            &ena_output[0],
                            &limit_output[0]);

        PWM_Map_Pre_Filter ((unsigned char) limit_output[1],
                            &ena_output[1],
                            &limit_output[1]);
        
        filters_sm++;
        break;
        
    case FILTERS_DUMMY1:
        // channel 1
        ch1_pwm = MA32_U16Circular (&st_sp1, *(limit_output + CH1_VAL_OFFSET));
        PWM_Update_CH1(ch1_pwm);

        // channel 2
        ch2_pwm = MA32_U16Circular (&st_sp2, *(limit_output + CH2_VAL_OFFSET));
        PWM_Update_CH2(ch2_pwm);

#ifdef USE_F_CHNLS_FOR_ENABLE
        // ena 1
        ena1_pwm = MA32_U16Circular (&st_ena1, *(ena_output + CH1_VAL_OFFSET));
        PWM_Update_ENA1(ena1_pwm);

        // ena 2
        ena2_pwm = MA32_U16Circular (&st_ena2, *(ena_output + CH2_VAL_OFFSET));
        PWM_Update_ENA2(ena2_pwm);
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


void FiltersAndOffsets_Post_Mapping_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned int calc = 0;
    unsigned short ch1_pwm, ch2_pwm, ena1_pwm, ena2_pwm;
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
        if ((mem_conf.channels_operation_mode == CCT1_MODE) ||
            (mem_conf.channels_operation_mode == CCT2_MODE))
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
            calc >>= 7;    // to 511
            limit_output[0] = (unsigned short) calc;
        
            calc = temp1 * bright;
            calc >>= 7;    // to 511            
            limit_output[1] = (unsigned short) calc;
        }

        if (mem_conf.channels_operation_mode == ONECH_MODE)
        {
            calc = *(ch_dmx_val + 0);
            calc <<= 1;    // to 511
            
            limit_output[0] = (unsigned short) calc;
            limit_output[1] = 0;
        }

        filters_sm++;
        break;

    case FILTERS_LIMIT_EACH_CHANNEL:        
        // the limit is the same for the two channels        
        calc = limit_output[0] * mem_conf.max_current_channels[0];
        calc >>= 7;    // to 1023
        limit_output[0] = (unsigned short) calc;

        calc = limit_output[1] * mem_conf.max_current_channels[1];
        calc >>= 7;    // to 1023
        limit_output[1] = (unsigned short) calc;

        filters_sm++;
        break;

    case FILTERS_OUTPUTS:    // apply filters before mapping        
        // channel 1
        limit_output[0] <<= 2;
        limit_output[0] = MA32_U16Circular (&st_sp1, *(limit_output + CH1_VAL_OFFSET));

        // channel 2
        limit_output[1] <<= 2;
        limit_output[1] = MA32_U16Circular (&st_sp2, *(limit_output + CH2_VAL_OFFSET));
        
        filters_sm++;
        break;
        
    case FILTERS_DUMMY1:
        PWM_Map_Post_Filter (limit_output[0],
                            &ena1_pwm,
                            &ch1_pwm);

#ifdef USE_TEST_ON_ENA_CH1
        if (ch1_pwm > 3600)
            ch1_pwm = 3600;
        
        PWM_Update_ENA1(ch1_pwm);
        PWM_Update_CH1(DUTY_ALWAYS);
#else        
#if (defined HARDWARE_VERSION_1_3) || (defined HARDWARE_VERSION_1_2)
        PWM_Update_ENA1(ena1_pwm);
#elif (defined HARDWARE_VERSION_1_1)
#ifdef USE_F_CHNLS_FOR_ENABLE
        PWM_Update_ENA1(ena1_pwm);
#else
        if (ch1_pwm)
            ENA_CH1_ON;
        else
            ENA_CH1_OFF;
#endif  
#else
#error "no hardware version on filters_and_offsets.c"
#endif
        
        PWM_Update_CH1(ch1_pwm);
#endif    //USE_TEST_ON_ENA_CH1
        
        PWM_Map_Post_Filter (limit_output[1],
                            &ena2_pwm,
                            &ch2_pwm);

#ifdef USE_TEST_ON_ENA_CH2
        if (ch2_pwm > 3600)
            ch2_pwm = 3600;
        
        PWM_Update_ENA2(ch2_pwm);
        PWM_Update_CH2(DUTY_ALWAYS);
#else        
#if (defined HARDWARE_VERSION_1_3) || (defined HARDWARE_VERSION_1_2)
        PWM_Update_ENA2(ena2_pwm);
#elif (defined HARDWARE_VERSION_1_1)
#ifdef USE_F_CHNLS_FOR_ENABLE
        PWM_Update_ENA2(ena2_pwm);
#else
        if (ch2_pwm)
            ENA_CH2_ON;
        else
            ENA_CH2_OFF;
        
#endif
#else
#error "no hardware version on filters_and_offsets.c"
#endif

        PWM_Update_CH2(ch2_pwm);
#endif    //USE_TEST_ON_ENA_CH2
        
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


void FiltersAndOffsets_Filters_Reset (void)
{
    MA32_U16Circular_Reset(&st_sp1);
    MA32_U16Circular_Reset(&st_sp2);
    MA32_U16Circular_Reset(&st_ena1);
    MA32_U16Circular_Reset(&st_ena2);
}


#if (!defined HARDWARE_VERSION_1_3) && \
    (!defined HARDWARE_VERSION_1_2) && \
    (!defined HARDWARE_VERSION_1_1) && \
    (!defined HARDWARE_VERSION_1_0)
#error "Not hardware defined on filters_and_offsets.c!"
#endif



//--- end of file ---//
