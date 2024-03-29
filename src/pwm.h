//----------------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PWM.H ##############################################
//----------------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef __PWM_H_
#define __PWM_H_

#include "hard.h"

// Exported Types --------------------------------------------------------------



// Exported Constants or Macros ------------------------------------------------
#define POWER_CONTROL_GENERAL_THRESHOLD    512    // este es un valor total de dmx no pwm

#define EnablePreload_PWM1    EnablePreload_TIM3_CH1
#define EnablePreload_PWM2    EnablePreload_TIM3_CH2

#define DisablePreload_PWM1    DisablePreload_TIM3_CH1
#define DisablePreload_PWM2    DisablePreload_TIM3_CH2

#define PWM_Update_CH1(X)    Update_TIM3_CH1(X)
#define PWM_Update_CH2(X)    Update_TIM3_CH2(X)

#if (defined HARDWARE_VERSION_1_3) || (defined HARDWARE_VERSION_1_2)
#define EnablePreload_ENA1    EnablePreload_TIM3_CH3
#define EnablePreload_ENA2    EnablePreload_TIM3_CH4

#define DisablePreload_ENA1    DisablePreload_TIM3_CH3
#define DisablePreload_ENA2    DisablePreload_TIM3_CH4

#define PWM_Update_ENA1(X)    Update_TIM3_CH3(X)
#define PWM_Update_ENA2(X)    Update_TIM3_CH4(X)
#elif (defined HARDWARE_VERSION_1_1)
#define EnablePreload_ENA1    EnablePreload_TIM1_CH2
#define EnablePreload_ENA2    EnablePreload_TIM1_CH4

#define DisablePreload_ENA1    DisablePreload_TIM1_CH2
#define DisablePreload_ENA2    DisablePreload_TIM1_CH4

#define PWM_Update_ENA1(X)    Update_TIM1_CH2(X)
#define PWM_Update_ENA2(X)    Update_TIM1_CH4(X)
#else
#error "no hard version on pwm.h"
#endif



// Exported Functions ----------------------------------------------------------
void PWMChannelsReset (void);
unsigned short PWM_Map_From_Dmx (unsigned char);
// void PWM_Set_PwrCtrl (unsigned char *, unsigned char, unsigned short);
void PWM_Map_Pre_Filter (unsigned char dmx_data, unsigned short * pwm_ena, unsigned short * pwm_ch);
void PWM_Map_Post_Filter (unsigned short dmx_filtered, unsigned short * pwm_ena, unsigned short * pwm_ch);


#endif    /* __PWM_H_ */

//--- end of file ---//

