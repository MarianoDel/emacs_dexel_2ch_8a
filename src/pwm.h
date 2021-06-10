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

// #include "tim.h"

// Exported Types --------------------------------------------------------------



// Exported Constants or Macros ------------------------------------------------
#define POWER_CONTROL_GENERAL_THRESHOLD    512    // este es un valor total de dmx no pwm

#define EnablePreload_PWM1    EnablePreload_TIM3_CH1
#define EnablePreload_PWM2    EnablePreload_TIM3_CH2

#define DisablePreload_PWM1    DisablePreload_TIM3_CH1
#define DisablePreload_PWM2    DisablePreload_TIM3_CH2

#define PWM_Update_CH1(X)    Update_TIM3_CH1(X)
#define PWM_Update_CH2(X)    Update_TIM3_CH2(X)



// Exported Functions ----------------------------------------------------------
void PWMChannelsReset (void);
unsigned short PWM_Map_From_Dmx (unsigned char);
// void PWM_Set_PwrCtrl (unsigned char *, unsigned char, unsigned short);


#endif    /* __PWM_H_ */

//--- end of file ---//

