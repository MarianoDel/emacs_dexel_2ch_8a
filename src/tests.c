//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------


#include <stdio.h>
// Module Types Constants and Macros -------------------------------------------
// Switches actions
typedef enum {
    selection_none = 0,
    selection_up,
    selection_dwn,
    selection_enter,
    selection_back

} sw_actions_t;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned char blinking_on = 0;


// Module Functions to Test ----------------------------------------------------
unsigned char SwapNibble (unsigned char a);
void switches_up_dwn (unsigned char sw, unsigned int * password_num);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    unsigned int pass = 0;
    
    printf("Test pass\n");
    printf("a: ");
    for (unsigned char i = 0; i < 8; i++)
    {
        switches_up_dwn(selection_up, &pass);
    }
    printf("\n");
    blinking_on++;

    printf("a: ");
    for (unsigned char i = 0; i < 2; i++)
    {
        switches_up_dwn(selection_up, &pass);
    }
    printf("\n");

    blinking_on++;

    for (unsigned char i = 0; i < 3; i++)
    {
        switches_up_dwn(selection_up, &pass);
    }

    blinking_on++;

    for (unsigned char i = 0; i < 3; i++)
    {
        switches_up_dwn(selection_up, &pass);
    }
    
    blinking_on = 7;

    for (unsigned char i = 0; i < 3; i++)
    {
        switches_up_dwn(selection_up, &pass);
    }
    
    printf("\npass: 0x%08x\n", pass);

    for (unsigned char j = 0; j < 8; j++)
    {
        blinking_on = j;
        switches_up_dwn(selection_dwn, &pass);
    }

    printf("\npass: 0x%08x\n", pass);
    
    printf("Test Ended\n");
    
    return 0;
}



unsigned int password_num = 0;
void switches_up_dwn (unsigned char sw_action, unsigned int * new_pass)
{
    if (sw_action == selection_up)
    {
        unsigned char a = (unsigned char) ((password_num >> (28 - blinking_on * 4)) & 0x0000000f);
        printf("%d ", a);
        if (a < 9)
        {
            a++;
            unsigned int temp = password_num;
            temp &= (~(0x0000000f << (28 - blinking_on * 4)));
            temp |= (a << (28 - blinking_on * 4));
            password_num = temp;
        }
    }

    if (sw_action == selection_dwn)
    {
        unsigned char a = (unsigned char) ((password_num >> (28 - blinking_on * 4)) & 0x0000000f);
        printf("%d ", a);            
        if (a)
        {
            a--;
            unsigned int temp = password_num;
            temp &= (~(0x0000000f << (28 - blinking_on * 4)));
            temp |= (a << (28 - blinking_on * 4));
            password_num = temp;
        }
    }

    *new_pass = password_num;
}


// int main (int argc, char *argv[])
// {
//     printf("Test Nibble swaping\n");
//     for (unsigned char i = 0; i < 0x0f; i++)
//     {
//         printf("0x%02x 0x%02x\n", i, SwapNibble(i));
//     }
    
//     printf("Test Ended\n");
    
//     return 0;
// }


unsigned char SwapNibble (unsigned char a)
{
	unsigned char result = 0;

	if (a & 0x01)
		result |= 0x08;
	if (a & 0x02)
		result |= 0x04;
	if (a & 0x04)
		result |= 0x02;
	if (a & 0x08)
		result |= 0x01;

	return result;
}

//--- end of file ---//


