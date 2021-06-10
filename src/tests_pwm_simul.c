//-------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_BOOST_SIMUL.C #######################
//-------------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "pwm.h"
#include "dsp.h"


// Includes tests helper modules for the tests ---------------------------------
#include "tests_vector_utils.h"


#include <stdio.h>
#include <math.h>
#include <stdint.h>


// Types Constants and Macros --------------------------------------------------
// Select simulation lenght in mseconds
#define SIMULATION_MS    2000

#define VECTOR_LENGTH    SIMULATION_MS
// #define VECTOR_LENGTH    ((int)(SAMPLING_FREQ * SIMULATION_LENGTH))


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned short dmx_data_ch1 [VECTOR_LENGTH] = { 0 };
unsigned short dmx_data_ch2 [VECTOR_LENGTH] = { 0 };
unsigned short pwm_data_ch1 [VECTOR_LENGTH] = { 0 };
unsigned short pwm_data_ch2 [VECTOR_LENGTH] = { 0 };



// Tests Functions -------------------------------------------------------------
void TestDMXPwm (void);
void Test_Dmx_Step (void);


// Main Function to Test -------------------------------------------------------
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val);


// Module Auxiliary Functions for Tests ----------------------------------------
void Update_TIM3_CH1 (unsigned short new_pwm);
void Update_TIM3_CH2 (unsigned short new_pwm);
void Update_PWM_Counters (void);

void UpdateFiltersTest_Reset (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of DMX simulations...\n");

    // TestDMXPwm ();
    Test_Dmx_Step ();

    return 0;
}


void TestDMXPwm (void)
{
    printf("test dmx -> pwm conversion\n");

    unsigned short pwm_buffer [256] = { 0 };
    for (int i = 0; i < 256; i++)
        pwm_buffer[i] = PWM_Map_From_Dmx(i);
    
    ShowVectorUShort("\nVector pwm output:\n", pwm_buffer, 256);
}


void Test_Dmx_Step (void)
{
    printf("\nTest dmx Step\n");

    // brillo 10% muevo color
    for (int i = 0; i < VECTOR_LENGTH; i++)
        dmx_data_ch1 [i] = 25;

    for (int i = 0; i < 200; i++)
        dmx_data_ch2 [i] = 64;

    for (int i = 200; i < 400; i++)
        dmx_data_ch2 [i] = 128;

    for (int i = 400; i < 600; i++)
        dmx_data_ch2 [i] = 192;

    for (int i = 600; i < VECTOR_LENGTH; i++)
        dmx_data_ch2 [i] = 255;


    // temp 50% brillo hasta 10%
    // for (int i = 0; i < VECTOR_LENGTH; i++)
    //     dmx_data_ch2 [i] = 255;

    // for (int i = 0; i < 200; i++)
    //     dmx_data_ch1 [i] = 6;

    // for (int i = 200; i < 400; i++)
    //     dmx_data_ch1 [i] = 12;

    // for (int i = 400; i < 600; i++)
    //     dmx_data_ch1 [i] = 18;

    // for (int i = 600; i < VECTOR_LENGTH; i++)
    //     dmx_data_ch1 [i] = 25;
    
    // step 1
    // for (int i = 0; i < (VECTOR_LENGTH/2); i++)
    //     dmx_data [i] = 0;

    // for (int i = (VECTOR_LENGTH/2); i < VECTOR_LENGTH; i++)
    //     dmx_data [i] = 1;
    
    // cuatro saltos de 64
    // for (int i = 0; i < 200; i++)
    //     dmx_data [i] = 64;

    // for (int i = 200; i < 400; i++)
    //     dmx_data [i] = 128;

    // for (int i = 400; i < 600; i++)
    //     dmx_data [i] = 192;

    // for (int i = 600; i < VECTOR_LENGTH; i++)
    //     dmx_data [i] = 255;

    // step 255
    // for (int i = 0; i < VECTOR_LENGTH; i++)
    //     dmx_data [i] = 255;
    
    UpdateFiltersTest_Reset ();
    for (int i = 0; i < VECTOR_LENGTH; i++)
    {
        unsigned char ch_dmx_val [2];
        ch_dmx_val[0] = dmx_data_ch1[i];
        ch_dmx_val[1] = dmx_data_ch2[i];
        CheckFiltersAndOffsets_SM (ch_dmx_val);
        Update_PWM_Counters();
    }

    ShowVectorUShort("\nVector dmx data ch1:\n", dmx_data_ch1, VECTOR_LENGTH);
    ShowVectorUShort("\nVector dmx data ch2:\n", dmx_data_ch2, VECTOR_LENGTH);    
    ShowVectorUShort("\nVector pwm data ch1:\n", pwm_data_ch1, VECTOR_LENGTH);
    ShowVectorUShort("\nVector pwm data ch2:\n", pwm_data_ch2, VECTOR_LENGTH);    

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_UShort_To_File (file, "dmx_ch1", dmx_data_ch1, VECTOR_LENGTH);
    Vector_UShort_To_File (file, "dmx_ch2", dmx_data_ch2, VECTOR_LENGTH);    
    Vector_UShort_To_File (file, "pwm_ch1", pwm_data_ch1, VECTOR_LENGTH);
    Vector_UShort_To_File (file, "pwm_ch2", pwm_data_ch2, VECTOR_LENGTH);    

    printf("\nRun by hand python3 simul_pwm_data.py\n");

}


#define USE_BRIGHT_AND_TEMP
// #define USE_DIRECT_CHANNELS
#define USE_FILTER_LENGHT_32
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

#define CH1_VAL_OFFSET    0
#define CH2_VAL_OFFSET    1

typedef enum {
    FILTERS_BKP_CHANNELS,
    FILTERS_LIMIT_EACH_CHANNEL,
    FILTERS_OUTPUTS,
    FILTERS_DUMMY1,
    FILTERS_DUMMY2
    
} filters_and_offsets_e;

filters_and_offsets_e filters_sm = FILTERS_BKP_CHANNELS;
unsigned short limit_output [2] = { 0 };
void CheckFiltersAndOffsets_SM (volatile unsigned char * ch_dmx_val)
{
    unsigned short calc = 0;
    
#ifdef USE_BRIGHT_AND_TEMP    
    unsigned char bright = 0;
    unsigned char temp0 = 0;
    unsigned char temp1 = 0;
#endif
    
    switch (filters_sm)
    {
    case FILTERS_BKP_CHANNELS:
#ifdef USE_BRIGHT_AND_TEMP
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
        limit_output[0] = calc;
        
        calc = temp1 * bright;
        // calc >>= 8;
        // limit_output[1] = (unsigned char) calc;
        // calc >>= 4;    //4000 pts        
        // calc >>= 3;    //8000 pts
        calc >>= 2;    //16000 pts        
        limit_output[1] = calc;
        
#endif
        
#ifdef USE_DIRECT_CHANNELS
        calc = *(ch_dmx_val + 0);
        calc <<= 6;
        limit_output[0] = calc;

        calc = *(ch_dmx_val + 1);
        calc <<= 6;
        limit_output[1] = calc;
#endif

        filters_sm++;
        break;

    case FILTERS_LIMIT_EACH_CHANNEL:
        // calc = limit_output[0] * mem_conf.max_current_channels[0];
        // calc >>= 8;
        // limit_output[0] = (unsigned char) calc;

        // calc = limit_output[1] * mem_conf.max_current_channels[1];
        // calc >>= 8;
        // limit_output[1] = (unsigned char) calc;

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
#endif
        // if (LED)
        //     LED_OFF;
        // else
        //     LED_ON;

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


#ifdef USE_FILTER_LENGHT_16
void UpdateFiltersTest_Reset (void)
{
    MA16_U16Circular_Reset(&st_sp1);
    MA16_U16Circular_Reset(&st_sp2);
}
#endif
#ifdef USE_FILTER_LENGHT_32
void UpdateFiltersTest_Reset (void)
{
    MA32_U16Circular_Reset(&st_sp1);
    MA32_U16Circular_Reset(&st_sp2);
}
#endif



int pwm_cntr = 0;
int updated = 0;
void Update_PWM_Counters (void)
{
    if (pwm_cntr)
    {
        if (!updated)
        {
            pwm_data_ch1[pwm_cntr] = pwm_data_ch1[pwm_cntr - 1];
            pwm_data_ch2[pwm_cntr] = pwm_data_ch2[pwm_cntr - 1];
        }
        else
            updated = 0;
    }
    pwm_cntr++;
}


void Update_TIM3_CH1 (unsigned short new_pwm)
{
    pwm_data_ch1[pwm_cntr] = new_pwm;
    updated = 1;
}


void Update_TIM3_CH2 (unsigned short new_pwm)
{
    pwm_data_ch2[pwm_cntr] = new_pwm;
    updated = 1;
}


//--- end of file ---//


