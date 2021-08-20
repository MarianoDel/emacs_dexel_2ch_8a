//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### GPIO.C ################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "gpio.h"
#include "stm32f0xx.h"
#include "hard.h"

//--- Defines for Configuration ---------------------------------
#define GPIOA_ENABLE
#define GPIOB_ENABLE
// #define GPIOC_ENABLE
// #define GPIOF_ENABLE

#define WITH_EXTI
//--- End of Defines for Configuration --------------------------

// Private Types Constants and Macros ------------------------------------------
#define GPIOA_CLK    (RCC->AHBENR & 0x00020000)
#define GPIOA_CLK_ON    (RCC->AHBENR |= 0x00020000)
#define GPIOA_CLK_OFF    (RCC->AHBENR &= ~0x00020000)

#define GPIOB_CLK    (RCC->AHBENR & 0x00040000)
#define GPIOB_CLK_ON    (RCC->AHBENR |= 0x00040000)
#define GPIOB_CLK_OFF    (RCC->AHBENR &= ~0x00040000)

#define GPIOC_CLK    (RCC->AHBENR & 0x00080000)
#define GPIOC_CLK_ON    (RCC->AHBENR |= 0x00080000)
#define GPIOC_CLK_OFF    (RCC->AHBENR &= ~0x00080000)

#define GPIOD_CLK    (RCC->AHBENR & 0x00100000)
#define GPIOD_CLK_ON    (RCC->AHBENR |= 0x00100000)
#define GPIOD_CLK_OFF    (RCC->AHBENR &= ~0x00100000)

#define GPIOF_CLK    (RCC->AHBENR & 0x00400000)
#define GPIOF_CLK_ON    (RCC->AHBENR |= 0x00400000)
#define GPIOF_CLK_OFF    (RCC->AHBENR &= ~0x00400000)

#define SYSCFG_CLK    (RCC->APB2ENR & 0x00000001)
#define SYSCFG_CLK_ON    (RCC->APB2ENR |= 0x00000001)
#define SYSCFG_CLK_OFF    (RCC->APB2ENR &= ~0x00000001)


// Module Functions ------------------------------------------------------------
void GPIO_Config (void)
{
    unsigned long temp;

    //--- MODER ---//
    //00: Input mode (reset state)
    //01: General purpose output mode
    //10: Alternate function mode
    //11: Analog mode

    //--- OTYPER ---//
    //These bits are written by software to configure the I/O output type.
    //0: Output push-pull (reset state)
    //1: Output open-drain

    //--- ORSPEEDR ---//
    //These bits are written by software to configure the I/O output speed.
    //x0: Low speed.
    //01: Medium speed.
    //11: High speed.
    //Note: Refer to the device datasheet for the frequency.

    //--- PUPDR ---//
    //These bits are written by software to configure the I/O pull-up or pull-down
    //00: No pull-up, pull-down
    //01: Pull-up
    //10: Pull-down
    //11: Reserved

#ifdef GPIOA_ENABLE
    //--- GPIO A ---//
    if (!GPIOA_CLK)
        GPIOA_CLK_ON;

    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0x3C000000;    //PA0 - PA5 output; PA6 - PA7 alternative
    temp |= 0x41A8A555;    //PA8 input exti; PA9 PA10 PA11 alternative; PA12 PA15 output
    GPIOA->MODER = temp;

    temp = GPIOA->OTYPER;    //1 bit por pin
    temp &= 0xFFFFFFFF;    //
    temp |= 0x00000000;
    GPIOA->OTYPER = temp;
    
    temp = GPIOA->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFF0000;    
    temp |= 0x00000000;    //PA0 - PA7 low speed
    GPIOA->OSPEEDR = temp;

    temp = GPIOA->PUPDR;	//2 bits por pin
    temp &= 0xFF33FFFF;    //PA9 PA11 pulldwn
    temp |= 0x00880000;
    GPIOA->PUPDR = temp;
    
#endif

#ifdef GPIOB_ENABLE

    //--- GPIO B ---//
    if (!GPIOB_CLK)
        GPIOB_CLK_ON;

    temp = GPIOB->MODER;    //2 bits por pin
    temp &= 0xFFFFC030;    //PB0 analog; PB1 output; PB3 - PB5 input; PB6 output
    temp |= 0x00001007;
    GPIOB->MODER = temp;

    temp = GPIOB->OTYPER;	//1 bit por pin
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOB->OTYPER = temp;

    temp = GPIOB->OSPEEDR;	//2 bits por pin
    temp &= 0xFFFF0FFF;
    temp |= 0x00000000;		//low speed
    GPIOB->OSPEEDR = temp;

    temp = GPIOB->PUPDR;	//2 bits por pin
    temp &= 0xFFFFF03F;    //PB3 PB4 PB5 pullup
    temp |= 0x00000540;
    GPIOB->PUPDR = temp;

#endif

#ifdef GPIOF_ENABLE

    //--- GPIO F ---//
    if (!GPIOF_CLK)
        GPIOF_CLK_ON;

    temp = GPIOF->MODER;
    temp &= 0xFFFFFFF0;
    temp |= 0x00000005;
    GPIOF->MODER = temp;

    temp = GPIOF->OTYPER;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->OTYPER = temp;

    temp = GPIOF->OSPEEDR;
    temp &= 0xFFFFFFF0;
    temp |= 0x00000000;
    GPIOF->OSPEEDR = temp;

    temp = GPIOF->PUPDR;
    temp &= 0xFFFFFFFF;
    temp |= 0x00000000;
    GPIOF->PUPDR = temp;

#endif

#ifdef WITH_EXTI
    //Interrupt on PA8
    if (!SYSCFG_CLK)
        SYSCFG_CLK_ON;

    SYSCFG->EXTICR[0] = 0x00000000; //Select Port A
    SYSCFG->EXTICR[1] = 0x00000000; //Select Port A
    EXTI->IMR |= 0x0100;    //Corresponding mask bit for interrupts PA8
    EXTI->EMR |= 0x0000;    //Corresponding mask bit for events
    EXTI->RTSR |= 0x0100;    //Pin Interrupt line on rising edge PA8
    EXTI->FTSR |= 0x0100;    //Pin Interrupt line on falling edge PA8

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 3);
#endif
    
    
}

inline void EXTIOff (void)
{
	EXTI->IMR &= ~0x00000100;
}

inline void EXTIOn (void)
{
	EXTI->IMR |= 0x00000100;
}

//--- end of file ---//
