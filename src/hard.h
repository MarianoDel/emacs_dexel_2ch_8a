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
#define PIN_AUX1    ((GPIOB->IDR & 0x0002) == 0)

//GPIOA pin8    Exti input
#define DMX_INPUT    ((GPIOA->IDR & 0x0100) != 0)

//GPIOA pin9    NC

//GPIOA pin10    Usart1 Rx

//GPIOA pin11    
#define LED    ((GPIOA->ODR & 0x1000) == 0)
#define LED_OFF    (GPIOA->BSRR = 0x00001000)
#define LED_ON    (GPIOA->BSRR = 0x10000000)

//GPIOA pin12
#define PIN_AUX2    ((GPIOA->IDR & 0x1000) == 0)

//GPIOA pin13
//GPIOA pin14

//GPIOA pin15
#define PIN_AUX3    ((GPIOA->IDR & 0x8000) == 0)

//GPIOB pin3
#define SW_DWN    ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
#define SW_UP    ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define SW_SEL    ((GPIOB->IDR & 0x0020) == 0)

//GPIOB pin6
#define CTRL_FAN    ((GPIOB->ODR & 0x0040) != 0)
#define CTRL_FAN_ON    (GPIOB->BSRR = 0x00000040)
#define CTRL_FAN_OFF    (GPIOB->BSRR = 0x00400000)

//GPIOB pin7
#define CTRL_BKL    ((GPIOB->ODR & 0x0080) != 0)
#define CTRL_BKL_ON    (GPIOB->BSRR = 0x00000080)
#define CTRL_BKL_OFF    (GPIOB->BSRR = 0x00800000)

#endif    //HARDWARE_VER_1_0

// Exported Types & Macros -----------------------------------------------------
// Main Program States
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_INIT_FROM_TREATMENT,
    MAIN_WELCOME,
    MAIN_NORMAL_MODE_STANDBY,
    MAIN_CARD_MODE_STANDBY,
    MAIN_START_TREATMENT,
    MAIN_IN_TREATMENT,
    MAIN_GO_PAUSE,
    MAIN_PAUSED,    
    MAIN_RESUMING,
    MAIN_ENDING_TREATMENT,
    MAIN_ENTERING_SET_OR_MENU,
    MAIN_WAIT_SET_OR_MENU,
    MAIN_SET_TIME,
    MAIN_IN_MAIN_MENU
    
} main_state_t;



//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_PAUSED    3
#define LED_NO_RFID    4

// Answers expected


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000    //5 segundos
#define SWITCHES_THRESHOLD_HALF	50    //0.5 segundos
#define SWITCHES_THRESHOLD_MIN	5    //25 ms


//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void HARD_Timeouts (void);

void ChangeLed (unsigned char);
void UpdateLed (void);

resp_sw_t Check_SW_UP (void);
resp_sw_t Check_SW_DWN (void);
resp_sw_t Check_SW_SEL (void);

void UpdateSwitches (void);


#endif /* _HARD_H_ */

//--- end of file ---//

