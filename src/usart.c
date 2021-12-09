//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F030
// ##
// #### USART.C ###############################
//---------------------------------------------

// Includes --------------------------------------------------------------------
#include "hard.h"
#include "usart.h"
#include "dmx_receiver.h"

#include <string.h>


// Module Configs --------------------------------------------------------------
// #define USART_WITH_TX


// Module Private Types Constants and Macros -----------------------------------
#define USART_IN_DMX_MODE    0
#define USART_IN_MANUAL_MODE    1


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
#ifdef USART_WITH_TX
volatile unsigned char * ptx1;
volatile unsigned char * ptx1_pckt_index;
volatile unsigned char tx1buff[SIZEOF_TXDATA];
#endif

volatile unsigned char * prx1;
volatile unsigned char rx1buff[SIZEOF_RXDATA];

volatile unsigned char usart1_have_data = 0;
volatile unsigned char usart1_mode = USART_IN_DMX_MODE;



// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned int len;

    len = prx1 - rx1buff;

    if (len < max_len)
    {
        //el prx1 siempre llega adelantado desde la int, lo corto con un 0
        *prx1 = '\0';
        len += 1;
        memcpy(bout, (unsigned char *) rx1buff, len);
    }
    else
    {
        memcpy(bout, (unsigned char *) rx1buff, max_len);
        len = max_len;
    }

    //ajusto punteros de rx luego de la copia
    prx1 = rx1buff;

    return (unsigned char) len;
}


void USART1_IRQHandler(void)
{
    unsigned char dummy;

    // USART in mode Receiver --------------------------------------------------
    if (USART1->ISR & USART_ISR_RXNE)
    {
        dummy = USART1->RDR & 0x0FF;

        if (usart1_mode == USART_IN_DMX_MODE)
            DMX_Int_Serial_Receiver_Handler (dummy);
        else if (usart1_mode == USART_IN_MANUAL_MODE)
        {
            if (prx1 < &rx1buff[SIZEOF_RXDATA - 1])
            {
                if (dummy == '\n')
                {
                    *prx1 = '\0';
                    usart1_have_data = 1;
                }
                else
                {
                    *prx1 = dummy;
                    prx1++;
                }
            }
            else
                prx1 = rx1buff;    // fixes blocked with garbage problem
        }
    }

    // USART in Transmit mode --------------------------------------------------
#ifdef USART_WITH_TX
    if (USART1->CR1 & USART_CR1_TXEIE)
    {
        if (USART1->ISR & USART_ISR_TXE)
        {
            if ((ptx1 < &tx1buff[SIZEOF_TXDATA]) && (ptx1 < ptx1_pckt_index))
            {
                USART1->TDR = *ptx1;
                ptx1++;
            }
            else
            {
                ptx1 = tx1buff;
                ptx1_pckt_index = tx1buff;
                USART1->CR1 &= ~USART_CR1_TXEIE;
            }
        }
    }
#endif

    if ((USART1->ISR & USART_ISR_ORE) || (USART1->ISR & USART_ISR_NE) || (USART1->ISR & USART_ISR_FE))
    {
        USART1->ICR |= 0x0e;
        dummy = USART1->RDR;
    }
}


#ifdef USART_WITH_TX
void Usart1Send (char * send)
{
    unsigned char i;

    i = strlen(send);
    Usart1SendUnsigned((unsigned char *) send, i);
}

void Usart1SendUnsigned(unsigned char * send, unsigned char size)
{
    if ((ptx1_pckt_index + size) < &tx1buff[SIZEOF_TXDATA])
    {
        memcpy((unsigned char *)ptx1_pckt_index, send, size);
        ptx1_pckt_index += size;
        USART1->CR1 |= USART_CR1_TXEIE;
    }
}

void Usart1SendSingle(unsigned char tosend)
{
    Usart1SendUnsigned(&tosend, 1);
}
#endif


void Usart1Config(void)
{
    if (!USART1_CLK)
        USART1_CLK_ON;

#ifdef USART_WITH_TX
    ptx1 = tx1buff;
    ptx1_pckt_index = tx1buff;
#endif
    
    prx1 = rx1buff;

    Usart1Enable_PA10_250000 ();

    unsigned int temp;
    temp = GPIOA->AFR[1];
    temp &= 0xFFFFF0FF;
    temp |= 0x00000100;    //PA10 -> AF1
    GPIOA->AFR[1] = temp;

    temp = GPIOB->AFR[0];
    temp &= 0x0FFFFFFF;
    temp |= 0x00000000;    //PB7 -> AF0
    GPIOB->AFR[0] = temp;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 7);
}


void Usart1Enable_PA10_250000 (void)
{
    usart1_mode = USART_IN_DMX_MODE;
    
    USART1->CR1 &= ~(USART_CR1_UE);    //disable
    USART1->BRR = USART_250000;
    USART1->CR2 |= USART_CR2_STOP_1;	//2 bits stop

    unsigned int temp;
    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0xFFCFFFFF;    //PA10 alternative
    temp |= 0x00200000;    //
    GPIOA->MODER = temp;

    temp = GPIOB->MODER;    //2 bits por pin
    temp &= 0xFFFF3FFF;    //PB7 input
    temp |= 0x00000000;    //
    GPIOB->MODER = temp;

    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//no TX
    
}


void Usart1Enable_PB7_9600 (void)
{
    usart1_mode = USART_IN_MANUAL_MODE;
    
    USART1->CR1 &= ~(USART_CR1_UE);    //disable
    USART1->BRR = USART_9600;
    USART1->CR2 &= ~(USART_CR2_STOP_1);	//1 bits stop

    unsigned int temp;
    temp = GPIOB->MODER;    //2 bits por pin
    temp &= 0xFFFF3FFF;    //PB7 alternative
    temp |= 0x00008000;    //
    GPIOB->MODER = temp;

    temp = GPIOA->MODER;    //2 bits por pin
    temp &= 0xFFCFFFFF;    //PA10 input
    temp |= 0x00000000;    //
    GPIOA->MODER = temp;

    USART1->CR1 = USART_CR1_RXNEIE | USART_CR1_RE | USART_CR1_UE;	//no TX
    
}


unsigned char Usart1HaveData (void)
{
    return usart1_have_data;
}


void Usart1HaveDataReset (void)
{
    usart1_have_data = 0;
}

//--- end of file ---//
