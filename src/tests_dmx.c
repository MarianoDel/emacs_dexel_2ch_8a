//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_DMX.C ###########################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "dmx_receiver.h"
#include "tests_ok.h"

#include <stdio.h>
// #include <stdlib.h>
#include <string.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char usart1_have_data = 0;
char new_uart_msg [200] = { 0 };
char last_uart_sended [200] = { 0 };
int exti_is_on = 0;



// Module Auxialiary Functions -------------------------------------------------
void Usart1Send (char * msg);
unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len);
void EXTIOn (void);
void EXTIOff (void);


// Module Functions for testing ------------------------------------------------
void Test_Comms (void);
    

// Module Functions ------------------------------------------------------------


int main(int argc, char *argv[])
{

    Test_Comms ();
    
}


// give externals inputs to the module
volatile unsigned char dmx_buff_data[3] = { 0 };
volatile unsigned char Packet_Detected_Flag = 0;
volatile unsigned short DMX_channel_selected = 0;
volatile unsigned char DMX_channel_quantity = 1;
// get globals from module
extern unsigned char dmx_receive_flag;

void Test_Comms (void)
{
    // Test receiver routine
    dmx_receive_flag = 1;
    DMX_channel_selected = 1;
    DMX_channel_quantity = 2;
    for (int i = 0; i < 512; i++)
    {
        DMX_Int_Serial_Receiver_Handler((unsigned char) i);
    }

    printf("ch: %d buff: [%d] [%d] [%d]\n",
           DMX_channel_selected,
           dmx_buff_data[0],
           dmx_buff_data[1],
           dmx_buff_data[2]);

    // test not rx
    dmx_receive_flag = 0;
    for (int i = 0; i < 512; i++)
    {
        DMX_Int_Serial_Receiver_Handler(0);
    }

    printf("ch: %d buff: [%d] [%d] [%d]\n",
           DMX_channel_selected,
           dmx_buff_data[0],
           dmx_buff_data[1],
           dmx_buff_data[2]);
    

    // test last allowed channel
    dmx_receive_flag = 1;
    for (int i = 0; i < 512; i++)
    {
        DMX_Int_Serial_Receiver_Handler(0);
    }

    dmx_receive_flag = 1;    
    DMX_channel_selected = 510;
    DMX_channel_quantity = 2;
    unsigned char data = 0;
    for (int i = 0; i < 512; i++)
    {
        DMX_Int_Serial_Receiver_Handler(data);
        data++;
    }

    printf("ch: %d buff: [%d] [%d] [%d]\n",
           DMX_channel_selected,
           dmx_buff_data[0],
           dmx_buff_data[1],
           dmx_buff_data[2]);
    
    
}


unsigned char Usart1ReadBuffer (unsigned char * bout, unsigned short max_len)
{
    unsigned char len = 0;
    len = strlen(new_uart_msg);
    if (max_len > len)
        strcpy(bout, new_uart_msg);
    else
        printf("error on Usart1ReadBuffer max_len\n");

    return len;
}


void Usart1Send (char * msg)
{
    strcpy(last_uart_sended, msg);
}


void EXTIOn (void)
{
    exti_is_on = 1;
}


void EXTIOff (void)
{
    exti_is_on = 0;
}


//--- end of file ---//


