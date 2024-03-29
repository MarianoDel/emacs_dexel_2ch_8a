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
#define HARDWARE_VERSION_1_3    // change on footprint and china manufacturer
// #define HARDWARE_VERSION_1_2    // cambio pines para aceptar placa colores
// #define HARDWARE_VERSION_1_1    //fuente 48V->12V en placa control
// #define HARDWARE_VERSION_1_0    //micro F030K6T6


#define SOFTWARE_VERSION_1_2    // fix leds overcurrent reseting the errors counters
// #define SOFTWARE_VERSION_1_1    // posibility for change sw up for sw dwn (harcoded)
// #define SOFTWARE_VERSION_1_0    // first version


//---- Features Configuration --------------------
#define USE_TEMP_PROT
// #define USE_CTROL_FAN_ALWAYS_ON
// #define USE_F_CHNLS_FOR_ENABLE        //this one or the later
// #define USE_F_CHNLS_FOR_FREQ_DETECT    //this one or the former
#define USE_SW_UP_FOR_SW_DWN    // front panel error, change UP for DWN

// #define USE_TEST_ON_ENA_CH1
// #define USE_TEST_ON_ENA_CH2    //ch2 with pwm in ena connection LD always on
#define USE_OVERCURRENT_PROT    // protection on less than 40khz detection

// -- Freq for the timers
// #define USE_PWM_16000_FREQ_1KHZ
#define USE_PWM_4000_FREQ_4KHZ
// #define USE_PWM_8000_FREQ_2KHZ
// #define USE_


//---- End of Features Configuration -------------

//--- Sanity Checks ------------------------------
#if (defined HARDWARE_VERSION_1_1) && (defined USE_OVERCURRENT_PROT)
#ifndef USE_F_CHNLS_FOR_FREQ_DETECT
#error "must use f channels for detect overcurrent"
#endif
#endif

#if (defined HARDWARE_VERSION_1_3) || (defined HARDWARE_VERSION_1_2)
#if (defined USE_F_CHNLS_FOR_FREQ_DETECT) || (defined USE_F_CHNLS_FOR_ENABLE)
#error "f channels not used for freq nor channels on version 1.2"
#endif
#endif

#if (!defined HARDWARE_VERSION_1_3) && \
    (!defined HARDWARE_VERSION_1_2) && \
    (!defined HARDWARE_VERSION_1_1) && \
    (!defined HARDWARE_VERSION_1_0)
#error "Not hardware defined on hard.h!"
#endif
//--- End of Sanity Checks -----------------------

// Exported Pinout Names -------------------------------------------------------
#if (defined HARDWARE_VERSION_1_3) || (defined HARDWARE_VERSION_1_2)
//GPIOF pin0
#define LCD_E    ((GPIOF->ODR & 0x0001) != 0)
#define LCD_E_ON    (GPIOF->BSRR = 0x00000001)
#define LCD_E_OFF    (GPIOF->BSRR = 0x00010000)

//GPIOF pin1
#define CTRL_BKL    ((GPIOF->ODR & 0x0002) != 0)
#define CTRL_BKL_ON    (GPIOF->BSRR = 0x00000002)
#define CTRL_BKL_OFF    (GPIOF->BSRR = 0x00020000)

//GPIOA pin0    
//GPIOA pin1    
//GPIOA pin2    
//GPIOA pin3    Lcd interface

//GPIOA pin4    Adc LM335_VO

//GPIOA pin5    
#define LCD_RS    ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON    (GPIOA->BSRR = 0x00000020)
#define LCD_RS_OFF    (GPIOA->BSRR = 0x00200000)

//GPIOA pin6    TIM3_CH1
//GPIOA pin7    TIM3_CH2

//GPIOB pin0
#define ENA_CH1    ((GPIOB->ODR & 0x0001) != 0)
#define ENA_CH1_ON    (GPIOB->BSRR = 0x00000001)
#define ENA_CH1_OFF    (GPIOB->BSRR = 0x00010000)

//GPIOB pin1
#define ENA_CH2    ((GPIOB->ODR & 0x0002) != 0)
#define ENA_CH2_ON    (GPIOB->BSRR = 0x00000002)
#define ENA_CH2_OFF    (GPIOB->BSRR = 0x00020000)

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
//GPIOA pin14    NC

//GPIOA pin15    
#define I_SEL    ((GPIOA->ODR & 0x8000) == 0)
#define I_SEL_ON    (GPIOA->BSRR = 0x00008000)
#define I_SEL_OFF    (GPIOA->BSRR = 0x80000000)

//GPIOB pin3
#define SW_SEL    ((GPIOB->IDR & 0x0008) == 0)

#ifdef USE_SW_UP_FOR_SW_DWN
//GPIOB pin4
#define SW_DWN    ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define SW_UP    ((GPIOB->IDR & 0x0020) == 0)
#else
//GPIOB pin4
#define SW_UP    ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define SW_DWN    ((GPIOB->IDR & 0x0020) == 0)
#endif

//GPIOB pin6
#define CTRL_FAN    ((GPIOB->ODR & 0x0040) != 0)
#define CTRL_FAN_ON    (GPIOB->BSRR = 0x00000040)
#define CTRL_FAN_OFF    (GPIOB->BSRR = 0x00400000)

//GPIOB pin7    double function SELECT COLOR Usart1 Rx

#endif    //HARDWARE_VER_1_2

#ifdef HARDWARE_VERSION_1_1
//GPIOF pin0
#define ENA_CH1    ((GPIOF->ODR & 0x0001) != 0)
#define ENA_CH1_ON    (GPIOF->BSRR = 0x00000001)
#define ENA_CH1_OFF    (GPIOF->BSRR = 0x00010000)

//GPIOF pin1
#define ENA_CH2    ((GPIOF->ODR & 0x0002) != 0)
#define ENA_CH2_ON    (GPIOF->BSRR = 0x00000002)
#define ENA_CH2_OFF    (GPIOF->BSRR = 0x00020000)

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
#define SW_UP    ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define SW_DWN    ((GPIOB->IDR & 0x0020) == 0)

//GPIOB pin6
#define CTRL_FAN    ((GPIOB->ODR & 0x0040) != 0)
#define CTRL_FAN_ON    (GPIOB->BSRR = 0x00000040)
#define CTRL_FAN_OFF    (GPIOB->BSRR = 0x00400000)

//GPIOB pin7

#endif    //HARDWARE_VER_1_1

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
#define SWITCHES_THRESHOLD_MIN_FAST    2    //10 ms


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

char * HARD_GetHardwareVersion (void);
char * HARD_GetSoftwareVersion (void);

#endif /* _HARD_H_ */

//--- end of file ---//

