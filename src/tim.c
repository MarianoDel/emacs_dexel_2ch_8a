//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TIM.C #################################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "tim.h"
#include "hard.h"


// Module Private Types Constants and Macros -----------------------------------
#define RCC_TIM1_CLK    (RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON    (RCC->APB2ENR |= 0x00000800)
#define RCC_TIM1_CLK_OFF    (RCC->APB2ENR &= ~0x00000800)

#define RCC_TIM3_CLK    (RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON    (RCC->APB1ENR |= 0x00000002)
#define RCC_TIM3_CLK_OFF    (RCC->APB1ENR &= ~0x00000002)

#define RCC_TIM6_CLK    (RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLK_ON    (RCC->APB1ENR |= 0x00000010)
#define RCC_TIM6_CLK_OFF    (RCC->APB1ENR &= ~0x00000010)

#define RCC_TIM14_CLK    (RCC->APB1ENR & 0x00000100)
#define RCC_TIM14_CLK_ON    (RCC->APB1ENR |= 0x00000100)
#define RCC_TIM14_CLK_OFF    (RCC->APB1ENR &= ~0x00000100)

#define RCC_TIM15_CLK    (RCC->APB2ENR & 0x00010000)
#define RCC_TIM15_CLK_ON    (RCC->APB2ENR |= 0x00010000)
#define RCC_TIM15_CLK_OFF    (RCC->APB2ENR &= ~0x00010000)

#define RCC_TIM16_CLK    (RCC->APB2ENR & 0x00020000)
#define RCC_TIM16_CLK_ON    (RCC->APB2ENR |= 0x00020000)
#define RCC_TIM16_CLK_OFF    (RCC->APB2ENR &= ~0x00020000)

#define RCC_TIM17_CLK    (RCC->APB2ENR & 0x00040000)
#define RCC_TIM17_CLK_ON    (RCC->APB2ENR |= 0x00040000)
#define RCC_TIM17_CLK_OFF    (RCC->APB2ENR &= ~0x00040000)


// Externals -------------------------------------------------------------------
extern volatile unsigned short wait_ms_var;


// Globals ---------------------------------------------------------------------
volatile unsigned char f_channel_2_int = 0;
volatile unsigned char f_channel_4_int = 0;


// Module Functions ------------------------------------------------------------
void Update_TIM3_CH1 (unsigned short a)
{
    TIM3->CCR1 = a;
}


void Update_TIM3_CH2 (unsigned short a)
{
    TIM3->CCR2 = a;
}


void Update_TIM3_CH3 (unsigned short a)
{
    TIM3->CCR3 = a;
}


void Update_TIM3_CH4 (unsigned short a)
{
    TIM3->CCR4 = a;
}


void Update_TIM1_CH1 (unsigned short a)
{
    TIM1->CCR1 = a;
}


void Update_TIM1_CH2 (unsigned short a)
{
    TIM1->CCR2 = a;
}


void Update_TIM1_CH4 (unsigned short a)
{
    TIM1->CCR4 = a;
}


void Wait_ms (unsigned short wait)
{
    wait_ms_var = wait;

    while (wait_ms_var);
}


void TIM_1_Init (void)
{
    unsigned long temp;

    if (!RCC_TIM1_CLK)
        RCC_TIM1_CLK_ON;

#ifdef HARDWARE_VERSION_1_2
    //Configuracion del timer.
    TIM1->CR1 = 0x00;		//clk int / 1; upcounting
    // TIM1->CR2 |= TIM_CR2_MMS_1;		//UEV -> TRG0

    TIM1->SMCR = 0x0000;
    // TIM1->CCMR1 = 0x2100;    //CH2 input filtered N=4 map IC2->TI2
    // TIM1->CCMR2 = 0x2100;    //CH4 input filtered N=4 map IC4->TI4
    TIM1->CCMR1 = 0x2d00;    //CH2 input filtered N=4 map IC2->TI2, pres / 8
    TIM1->CCMR2 = 0x2d00;    //CH4 input filtered N=4 map IC4->TI4, pres / 8
    
    TIM1->CCER |= TIM_CCER_CC4E | TIM_CCER_CC2E;    //CH4 & CH2 capture enable rising edge    
    TIM1->ARR = 0xFFFF;
    TIM1->CNT = 0;

    TIM1->PSC = 47;
#ifdef USE_OVERCURRENT_PROT
    // Enable timer interrupt see UDIS
    TIM1->DIER |= TIM_DIER_CC4IE | TIM_DIER_CC2IE;    // int on CC4 CC2 & overflow    
    // NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    // NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_SetPriority(TIM1_CC_IRQn, 8);
#endif    
    
    //Alternate Fuction Pin Configurations
    temp = GPIOA->AFR[1];
    temp &= 0xFFFF0F0F;    
    temp |= 0x00002020;    //PA11 -> AF2; PA9 -> AF2
    GPIOA->AFR[1] = temp;
    
    TIM1->CR1 |= TIM_CR1_CEN;
#endif    //HARDWARE_VERSION_1_2

    
#ifdef HARDWARE_VERSION_1_1
#ifdef USE_F_CHNLS_FOR_FREQ_DETECT
    //Configuracion del timer.
    TIM1->CR1 = 0x00;		//clk int / 1; upcounting
    // TIM1->CR2 |= TIM_CR2_MMS_1;		//UEV -> TRG0

    TIM1->SMCR = 0x0000;
    // TIM1->CCMR1 = 0x2100;    //CH2 input filtered N=4 map IC2->TI2
    // TIM1->CCMR2 = 0x2100;    //CH4 input filtered N=4 map IC4->TI4
    TIM1->CCMR1 = 0x2d00;    //CH2 input filtered N=4 map IC2->TI2, pres / 8
    TIM1->CCMR2 = 0x2d00;    //CH4 input filtered N=4 map IC4->TI4, pres / 8
    
    TIM1->CCER |= TIM_CCER_CC4E | TIM_CCER_CC2E;    //CH4 & CH2 capture enable rising edge    
    TIM1->ARR = 0xFFFF;
    TIM1->CNT = 0;

    TIM1->PSC = 47;
#ifdef USE_OVERCURRENT_PROT
    // Enable timer interrupt see UDIS
    TIM1->DIER |= TIM_DIER_CC4IE | TIM_DIER_CC2IE;    // int on CC4 CC2 & overflow    
    // NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
    // NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 1);
    NVIC_EnableIRQ(TIM1_CC_IRQn);
    NVIC_SetPriority(TIM1_CC_IRQn, 8);
#endif    
#endif

#ifdef USE_F_CHNLS_FOR_ENABLE
    //Configuracion del timer.
    TIM1->CR1 = 0x00;		//clk int / 1; upcounting
    // TIM1->CR2 |= TIM_CR2_MMS_1;		//UEV -> TRG0

    TIM1->SMCR = 0x0000;
    TIM1->CCMR1 = 0x6000;    //CH2 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)
    TIM1->CCMR2 = 0x6000;    //CH4 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)
    
    TIM1->CCER |= TIM_CCER_CC4E | TIM_CCER_CC2E;    //CH4 & CH2 enable on pin direct polarity

    TIM1->BDTR |= TIM_BDTR_MOE;
    
    TIM1->ARR = 4095;
    TIM1->CNT = 0;

    TIM1->PSC = 2;
#endif
    
    //Alternate Fuction Pin Configurations
    temp = GPIOA->AFR[1];
    temp &= 0xFFFF0F0F;    
    temp |= 0x00002020;    //PA11 -> AF2; PA9 -> AF2
    GPIOA->AFR[1] = temp;
    
    TIM1->CR1 |= TIM_CR1_CEN;
#endif    //HARDWARE_VERSION_1_1
}


#if (defined HARDWARE_VERSION_1_2) && (defined USE_OVERCURRENT_PROT)
volatile unsigned short capture_ch2 = 0;
volatile unsigned short capture_ch4 = 0;
void TIM1_CC_IRQHandler (void)
{
    // IC2 int
    if (TIM1->SR & (TIM_SR_CC2IF | TIM_SR_CC2OF))	//reset flag capture and overcapture
    {
        unsigned short interval = 0;
        
        if (TIM1->CCR2 < capture_ch2)
        {
            //overflow
            interval = 0xffff - capture_ch2;
            interval += TIM1->CCR2;
        }
        else
            interval = TIM1->CCR2 - capture_ch2;

        capture_ch2 = TIM1->CCR2;

        // check for fully enabled channel
        if (TIM3->CCR3 > 4095)            
        {
            // if (interval > 28)    //28us 35.7KHz
            if (interval > 224)    //28us 35.7KHz * 8 prescaler         
                f_channel_2_int++;
            else if (f_channel_2_int)
                f_channel_2_int--;
            
        }
        else
            f_channel_2_int = 0;

        TIM1->SR &= ~(TIM_SR_CC2IF | TIM_SR_CC2OF);
    }

    // IC4 int
    else if (TIM1->SR & (TIM_SR_CC4IF | TIM_SR_CC4OF))	//reset flag capture and overcapture
    {
        unsigned short interval = 0;
        
        if (TIM1->CCR4 < capture_ch4)
        {
            //overflow
            interval = 0xffff - capture_ch4;
            interval += TIM1->CCR4;
        }
        else
            interval = TIM1->CCR4 - capture_ch4;

        capture_ch4 = TIM1->CCR4;

        // check for fully enabled channel
        if (TIM3->CCR4 > 4095)
        {
            // if (interval > 28)    //28us 35.7KHz
            if (interval > 224)    //28us 35.7KHz * 8 prescaler                     
                f_channel_4_int++;
            else if (f_channel_4_int)
                f_channel_4_int--;
            
        }
        else
            f_channel_4_int = 0;

        TIM1->SR &= ~(TIM_SR_CC4IF | TIM_SR_CC4OF);
    }
}
#endif    //HARDWARE_VERSION_1_2


#ifdef HARDWARE_VERSION_1_1
#if (defined USE_F_CHNLS_FOR_FREQ_DETECT) && (defined USE_OVERCURRENT_PROT)
volatile unsigned short capture_ch2 = 0;
volatile unsigned short capture_ch4 = 0;
void TIM1_CC_IRQHandler (void)
{
    // IC2 int
    if (TIM1->SR & (TIM_SR_CC2IF | TIM_SR_CC2OF))	//reset flag capture and overcapture
    {
        unsigned short interval = 0;
        
        if (TIM1->CCR2 < capture_ch2)
        {
            //overflow
            interval = 0xffff - capture_ch2;
            interval += TIM1->CCR2;
        }
        else
            interval = TIM1->CCR2 - capture_ch2;

        capture_ch2 = TIM1->CCR2;

        // if (interval > 28)    //28us 35.7KHz
        if (interval > 224)    //28us 35.7KHz * 8 prescaler         
            f_channel_2_int++;
        else if (f_channel_2_int)
            f_channel_2_int--;

        TIM1->SR &= ~(TIM_SR_CC2IF | TIM_SR_CC2OF);
    }

    // IC4 int
    else if (TIM1->SR & (TIM_SR_CC4IF | TIM_SR_CC4OF))	//reset flag capture and overcapture
    {
        unsigned short interval = 0;
        
        if (TIM1->CCR4 < capture_ch4)
        {
            //overflow
            interval = 0xffff - capture_ch4;
            interval += TIM1->CCR4;
        }
        else
            interval = TIM1->CCR4 - capture_ch4;

        capture_ch4 = TIM1->CCR4;

        // if (interval > 28)    //28us 35.7KHz
        if (interval > 224)    //28us 35.7KHz * 8 prescaler                     
            f_channel_4_int++;
        else if (f_channel_4_int)
            f_channel_4_int--;

        TIM1->SR &= ~(TIM_SR_CC4IF | TIM_SR_CC4OF);
    }
}
#endif
#endif    //HARDWARE_VERSION_1_1


void TIM_3_Init (void)
{
    unsigned long temp;
    
    if (!RCC_TIM3_CLK)
        RCC_TIM3_CLK_ON;

    //Configuracion del timer.
    TIM3->CR1 = 0x00;		//clk int / 1; upcounting
    // TIM3->CR2 = 0x00;		//igual al reset    
    TIM3->CR2 |= TIM_CR2_MMS_1;		//UEV -> TRG0


    TIM3->SMCR = 0x0000;
    // TIM3->SMCR |= TIM_SMCR_SMS_2;			//trigger: reset mode; link timer 1
    // TIM3->SMCR |= TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1;	//trigger: trigger mode; link timer 1    
    TIM3->CCMR1 = 0x6060;      //CH1, CH2 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)
    TIM3->CCER |= TIM_CCER_CC2E | TIM_CCER_CC1E;    //CH2 y CH1 enable on pin direct polarity    
#ifdef HARDWARE_VERSION_1_2
    TIM3->CCMR2 = 0x6060;      //CH3, CH4 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)
    // TIM3->CCMR2 = 0x0000;
    TIM3->CCER |= TIM_CCER_CC4E | TIM_CCER_CC3E;    //CH4 y CH3 enable on pin direct polarity    
#endif
    
    TIM3->ARR = DUTY_100_PERCENT;        //tick cada 20.83us --> 48KHz
    TIM3->CNT = 0;

#if defined USE_TIM_FREQ_16KHZ
    TIM3->PSC = 2;    // prescales div = 3
#elif defined USE_TIM_FREQ_4_8KHZ
    TIM3->PSC = 9;    // prescales div = 10
#else
    #error "set freq on hard.h"
#endif
	
    //Alternate Fuction Pin Configurations
    temp = GPIOA->AFR[0];
    temp &= 0x00FFFFFF;	
    temp |= 0x11000000;    //PA7 -> AF1; PA6 -> AF1
    GPIOA->AFR[0] = temp;

#ifdef HARDWARE_VERSION_1_2    
    temp = GPIOB->AFR[0];
    temp &= 0xFFFFFF00;	
    temp |= 0x00000011;    //PB1 -> AF1; PB0 -> AF1
    GPIOB->AFR[0] = temp;
#endif
    
    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;    
}


void TIM3_IRQHandler (void)	//1 ms
{
	/*
	Usart_Time_1ms ();

	if (timer_1seg)
	{
		if (timer_1000)
			timer_1000--;
		else
		{
			timer_1seg--;
			timer_1000 = 1000;
		}
	}

	if (timer_led_comm)
		timer_led_comm--;

	if (timer_standby)
		timer_standby--;
	*/
	//bajar flag
	if (TIM3->SR & 0x01)	//bajo el flag
		TIM3->SR = 0x00;
}


void TIM14_IRQHandler (void)	//20us
{
    if (TIM14->SR & 0x01)
        TIM14->SR = 0x00;    //bajar flag

}


void TIM_14_Init (void)
{
    if (!RCC_TIM14_CLK)
        RCC_TIM14_CLK_ON;

    //Configuracion del timer.
    TIM14->CR1 = 0x00;		//clk int / 1; upcounting; uev
    TIM14->PSC = 47;		//tick cada 1us
    TIM14->ARR = 0xFFFF;
    TIM14->EGR |= 0x0001;

}


void TIM16_IRQHandler (void)	//tick on 1uS
{
    if (TIM16->SR & 0x01)
        //bajar flag
        TIM16->SR = 0x00;

    // RelayTimHandler();
}


void TIM16_Init (void)
{
    if (!RCC_TIM16_CLK)
        RCC_TIM16_CLK_ON;

    //Configuracion del timer.
    TIM16->CR1 = 0x00;		//clk int / 1; upcounting; uev
    TIM16->ARR = 0xFFFF;
    TIM16->CNT = 0;
    
    TIM16->PSC = 47;

    TIM16->CR1 |= TIM_CR1_CEN;

    // // Enable timer int Update Enable
    // TIM16->DIER |= TIM_DIER_UIE;
    
    // NVIC_EnableIRQ(TIM16_IRQn);
    // NVIC_SetPriority(TIM16_IRQn, 8);
}


void TIM16Enable (void)
{
    TIM16->CR1 |= TIM_CR1_CEN;
}


void TIM16Disable (void)
{
    TIM16->CR1 &= ~TIM_CR1_CEN;
}

//
// void TIM17_IRQHandler (void)	//100uS
// {
//
// 	//if (GPIOA_PIN0_OUT)
// 	//	GPIOA_PIN0_OFF;
// 	//else
// 	//	GPIOA_PIN0_ON;
//
// 	if (TIM17->SR & 0x01)
// 		//bajar flag
// 		TIM17->SR = 0x00;
// }
//
//
// void TIM_17_Init (void)
// {
//
// 	NVIC_InitTypeDef NVIC_InitStructure;
//
// 	if (!RCC_TIM17_CLK)
// 		RCC_TIM17_CLK_ON;
//
// 	//Configuracion del timer.
// 	TIM17->ARR = 2000; //10m
// 	TIM17->CNT = 0;
// 	TIM17->PSC = 479;
// 	TIM17->EGR = TIM_EGR_UG;
//
// 	// Enable timer ver UDIS
// 	TIM17->DIER |= TIM_DIER_UIE;
// 	TIM17->CR1 |= TIM_CR1_CEN;
//
// 	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
// 	NVIC_InitStructure.NVIC_IRQChannelPriority = 5;
// 	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
// 	NVIC_Init(&NVIC_InitStructure);
// }

//--- end of file ---//
