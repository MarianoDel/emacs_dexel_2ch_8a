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
    PWM_Update_CH1(DUTY_NONE);
    PWM_Update_CH2(DUTY_NONE);
#ifdef HARDWARE_VERSION_1_1
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

#define s_channel_cntr_roof    100
#define s_freq    1250
unsigned short s_channel_cntr = 0;
unsigned short s_channel1 = 0;
unsigned short s_channel2 = 0;
void PWM_Soft_Init (void)
{
    // Start the running timer on us
    TIM16_Init();
    
}


void PWM_Soft_Update_Channels (void)
{
    if (TIM16->CNT > s_freq)
    {
        TIM16->CNT = 0;

        if (LED)
            LED_OFF;
        else
            LED_ON;
        
        if (s_channel_cntr < s_channel_cntr_roof)
            s_channel_cntr++;
        else
            s_channel_cntr = 0;

        if (s_channel1 > s_channel_cntr)
            ENA_CH1_ON;
        else
            ENA_CH1_OFF;

        if (s_channel2 > s_channel_cntr)
            ENA_CH2_ON;
        else
            ENA_CH2_OFF;    
    }
}


void PWM_Soft_Update_Channel1 (unsigned short new_pwm)
{
    if (new_pwm > s_channel_cntr_roof)
        new_pwm = s_channel_cntr_roof;

    s_channel1 = new_pwm;
}


void PWM_Soft_Update_Channel2 (unsigned short new_pwm)
{
    if (new_pwm > s_channel_cntr_roof)
        new_pwm = s_channel_cntr_roof;

    s_channel2 = new_pwm;
}
//--- end of file ---//
