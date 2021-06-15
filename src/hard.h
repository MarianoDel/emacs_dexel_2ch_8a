//----------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #################################
//----------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"
#include "switches_answers.h"

//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define HARDWARE_VERSION_1_0    //micro F030K6T6


// #define SOFTWARE_VERSION_1_1
#define SOFTWARE_VERSION_1_0


//---- Features Configuration -----------------
#define USE_TEMP_PROT

// -- Freq for the timers
#define USE_PWM_16000_FREQ_1KHZ
// #define USE_PWM_8000_FREQ_2KHZ
// #define USE_

// -- Dmx to Pwm filter lenght --
// #define USE_FILTER_LENGHT_16
#define USE_FILTER_LENGHT_32
// #define USE_NO_FILTER

// -- Dmx to Pwm channels setup
#define USE_BRIGHT_AND_TEMP
// #define USE_DIRECT_CHANNELS


//---- End of Features Configuration ----------


// Exported Pinout Names -------------------------------------------------------
#ifdef HARDWARE_VERSION_1_0
//GPIOA pin0    
//GPIOA pin1    
//GPIOA pin2    
//GPIOA pin3    Lcd interface

//GPIOA pin4
#define LCD_E    ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON    (GPIOA->BSRR = 0x00000010)
#define LCD_E_OFF    (GPIOA->BSRR = 0x00100000)

//GPIOA pin5    
#define LCD_RS    ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON    (GPIOA->BSRR = 0x00000020)
#define LCD_RS_OFF    (GPIOA->BSRR = 0x00200000)

//GPIOA pin6    TIM3_CH1
//GPIOA pin7    TIM3_CH2

//GPIOB pin0    Adc LM335_VO

//GPIOB pin1
#define CTRL_BKL    ((GPIOB->ODR & 0x0002) != 0)
#define CTRL_BKL_ON    (GPIOB->BSRR = 0x00000002)
#define CTRL_BKL_OFF    (GPIOB->BSRR = 0x00020000)


//GPIOA pin8    Exti input
#define DMX_INPUT    ((GPIOA->IDR & 0x0100) != 0)

//GPIOA pin9    F_CH1

//GPIOA pin10    Usart1 Rx

//GPIOA pin11    F_CH2

//GPIOA pin12
#define LED    ((GPIOA->ODR & 0x1000) == 0)
#define LED_OFF    (GPIOA->BSRR = 0x00001000)
#define LED_ON    (GPIOA->BSRR = 0x10000000)


//GPIOA pin13
//GPIOA pin14

//GPIOA pin15    
#define I_SEL    ((GPIOA->ODR & 0x8000) == 0)
#define I_SEL_ON    (GPIOA->BSRR = 0x00008000)
#define I_SEL_OFF    (GPIOA->BSRR = 0x80000000)


//GPIOB pin3
#define SW_SEL    ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
#define SW_DWN    ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define SW_UP    ((GPIOB->IDR & 0x0020) == 0)

//GPIOB pin6
#define CTRL_FAN    ((GPIOB->ODR & 0x0040) != 0)
#define CTRL_FAN_ON    (GPIOB->BSRR = 0x00000040)
#define CTRL_FAN_OFF    (GPIOB->BSRR = 0x00400000)

//GPIOB pin7

#endif    //HARDWARE_VER_1_0

// Exported Types & Macros -----------------------------------------------------

// Answers expected


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000    //5 segundos
#define SWITCHES_THRESHOLD_HALF	50    //250 ms
#define SWITCHES_THRESHOLD_MIN	10    //50 ms


#define CH1_VAL_OFFSET    0
#define CH2_VAL_OFFSET    1

//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void HARD_Timeouts (void);

resp_sw_t Check_SW_UP (void);
resp_sw_t Check_SW_DWN (void);
resp_sw_t Check_SW_SEL (void);

void UpdateSwitches (void);
sw_actions_t CheckActions (void);


#endif /* _HARD_H_ */

//--- end of file ---//

