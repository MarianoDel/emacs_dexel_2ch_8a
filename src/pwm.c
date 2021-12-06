//----------------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PWM.C ##############################################
//----------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "pwm.h"
#include "hard.h"
#include "tim.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void PWMChannelsReset (void)
{
#ifdef HARDWARE_VERSION_1_2
    PWM_Update_CH1(DUTY_NONE);
    PWM_Update_CH2(DUTY_NONE);
    PWM_Update_ENA1(DUTY_NONE);
    PWM_Update_ENA2(DUTY_NONE);
#endif
#if (defined HARDWARE_VERSION_1_1) || (defined HARDWARE_VERSION_1_0)
    PWM_Update_CH1(DUTY_NONE);
    PWM_Update_CH2(DUTY_NONE);
    ENA_CH1_OFF;
    ENA_CH2_OFF;
#endif
}


unsigned short PWM_Map_From_Dmx (unsigned char dmx_val)
{
    unsigned int pwm = 0;

#if (DUTY_100_PERCENT == 1000)    
    if (dmx_val)
    {
        pwm = dmx_val * 391;
        pwm = pwm / 100;
        pwm += 4;
    }
#endif
#if (DUTY_100_PERCENT == 4000)
    if (dmx_val)
    {
        pwm = dmx_val * 157;
        pwm = pwm / 10;
        pwm += 13;
    }
#endif

    return (unsigned short) pwm;

}


// void PWM_Set_PwrCtrl (unsigned char * ch_dmx_val, unsigned char chnls_qtty, unsigned short max_power)
// {
//     unsigned short total_dmx = 0;

//     //cuantos en total
//     for (unsigned char i = 0; i < chnls_qtty; i++)
//         total_dmx += *(ch_dmx_val + i);

//     if (total_dmx > max_power)
//     {
//         unsigned int new = 0;
//         for (unsigned char i = 0; i < chnls_qtty; i++)
//         {
//             // si el canal tiene algo
//             if (*(ch_dmx_val + i))
//             {
//                 new = *(ch_dmx_val + i) * max_power;
//                 new = new / total_dmx;

//                 // no dejo que se apaguen los canales
//                 if (new)
//                     *(ch_dmx_val + i) = (unsigned char) new;
//                 else
//                     *(ch_dmx_val + i) = 1;
                
//             }
//         }
//     }
// }


// get dmx_data from 0 to 255
// answer pwm_ena 0 to 4096
// answer pwm_ch 0 to 4096
void PWM_Map_Pre_Filter (unsigned char dmx_data, unsigned short * pwm_ena, unsigned short * pwm_ch)
{
    unsigned char dmx_ena = 0;
    unsigned short dmx_ch = 0;
    
    if (dmx_data > 4)
    {
        dmx_ena = 4;
        dmx_ch = dmx_data - 4;
    }
    else
    {
        dmx_ena = dmx_data;
        dmx_ch = 0;
    }

    // *pwm_ena = dmx_ena * 1024;
    *pwm_ena = dmx_ena << 10;
    
    dmx_ch = dmx_ch * 164;
    dmx_ch = dmx_ch / 10;
    *pwm_ch = dmx_ch;
    
}


// get dmx_filtered from 0 to 4095
// answer pwm_ena 0 to 4096
// answer pwm_ch 0 to 4096
void PWM_Map_Post_Filter (unsigned short dmx_filtered, unsigned short * pwm_ena, unsigned short * pwm_ch)
{
    unsigned short dmx_ena = 0;
    unsigned int dmx_ch = 0;
    
    if (dmx_filtered > 64)
    {
        dmx_ena = 64;
        dmx_ch = dmx_filtered - 64;
    }
    else
    {
        dmx_ena = dmx_filtered;
        dmx_ch = 0;
    }

    // *pwm_ena = dmx_ena * 64;
    *pwm_ena = dmx_ena << 6;
    
    dmx_ch = dmx_ch * 1016;
    dmx_ch = dmx_ch / 1000;
    *pwm_ch = (unsigned short) dmx_ch;
    
}



//--- end of file ---//
