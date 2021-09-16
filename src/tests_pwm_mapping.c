//-------------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_PWM_MAPPING.C #######################
//-------------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "pwm.h"


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
void Test_Dmx_Map_Pre (void);
void Test_Dmx_Map_Post (void);


// Main Function to Test -------------------------------------------------------


// Module Auxiliary Functions for Tests ----------------------------------------


// Module Mock Functions -------------------------------------------------------
void Update_TIM3_CH1 (unsigned short new_pwm);
void Update_TIM3_CH2 (unsigned short new_pwm);



// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    printf("Start of DMX simulations...\n");

    // Test_Dmx_Map_Pre ();
    Test_Dmx_Map_Post ();    

    return 0;
}


void Test_Dmx_Map_Pre (void)
{
    printf("test dmx -> pwm conversion for pwm_ena & pwm_ch\n");

    unsigned short pwm_ena [256] = { 0 };
    unsigned short pwm_ch [256] = { 0 };
    
    for (int i = 0; i < 256; i++)
        PWM_Map_Pre_Filter(i, &pwm_ena[i], &pwm_ch[i]);
    
    ShowVectorUShort("\nVector pwm ena:\n", pwm_ena, 256);
    ShowVectorUShort("\nVector pwm ch:\n", pwm_ch, 256);

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_UShort_To_File (file, "pwm_ena", pwm_ena, 256);
    Vector_UShort_To_File (file, "pwm_ch", pwm_ch, 256);    

    printf("\nRun by hand python3 simul_pwm_pre_map.py\n");
    
}


void Test_Dmx_Map_Post (void)
{
    printf("test dmx -> pwm conversion for pwm_ena & pwm_ch\n");

    unsigned short pwm_ena [4096] = { 0 };
    unsigned short pwm_ch [4096] = { 0 };
    
    for (int i = 0; i < 4096; i++)
    {
        PWM_Map_Post_Filter(i, &pwm_ena[i], &pwm_ch[i]);
    }
    
    ShowVectorUShort("\nVector pwm ena:\n", pwm_ena, 4096);
    ShowVectorUShort("\nVector pwm ch:\n", pwm_ch, 4096);

    ///////////////////////////
    // Backup Data to a file //
    ///////////////////////////
    FILE * file = fopen("data.txt", "w");

    if (file == NULL)
    {
        printf("data file not created!\n");
        return;
    }

    Vector_UShort_To_File (file, "pwm_ena", pwm_ena, 4096);
    Vector_UShort_To_File (file, "pwm_ch", pwm_ch, 4096);    

    printf("\nRun by hand python3 simul_pwm_post_map.py\n");
    
}


// Module Mocked Functions -----------------------------------------------------
void Update_TIM3_CH1 (unsigned short new_pwm)
{
    // pwm_data_ch1[pwm_cntr] = new_pwm;
    // updated = 1;
}


void Update_TIM3_CH2 (unsigned short new_pwm)
{
    // pwm_data_ch2[pwm_cntr] = new_pwm;
    // updated = 1;
}


//--- end of file ---//


