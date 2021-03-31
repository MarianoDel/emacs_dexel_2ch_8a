//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "card_utils.h"

#include <stdio.h>

// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------



// Module Functions to Test ----------------------------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    char card_str [17] = { 0 };
    card_data_t card_data;
    card_data_t card_data2;    
    
    unsigned char status = MI_ERR;

    // test for creation process and compare
    card_data.sessions_left = 125;
    card_data.sessions_orig = 999;

    printf("String creation\n");
    status = Card_CreateDataString(card_str, &card_data);
    if (status == MI_OK)
        printf("created ok %s\n", card_str);
    else
        printf("MI_ERR on left: %d orig: %d\n",               
               card_data.sessions_left,
               card_data.sessions_orig);

    printf("String process\n");
    status = Card_ProcessDataString(card_str, &card_data2);
    if (status == MI_OK)    
        printf("process ok left: %d orig: %d\n",
               card_data2.sessions_left,
               card_data2.sessions_orig);
    else
        printf("MI_ERR on left: %d orig: %d\n",               
               card_data2.sessions_left,
               card_data2.sessions_orig);

    printf("String compare\n");
    status = Card_CompareDataString(card_str, &card_data);
    if (status == MI_OK)    
        printf("process ok left: %d orig: %d\n",
               card_data.sessions_left,
               card_data.sessions_orig);
    else
        printf("MI_ERR on left: %d orig: %d\n",               
               card_data.sessions_left,
               card_data.sessions_orig);
    

    // test for string comparision
    // char card_str2 [] = {"S:125:999:"};
    
    // card_data.sessions_left = 125;
    // card_data.sessions_orig = 999;
    // printf("String comparision\n");

    // status = Card_CompareDataString(card_str2, &card_data);
    // if (status == MI_OK)
    //     printf("%s\n", card_str2);
    // else
    //     printf("MI_ERR on left: %d orig: %d\n",               
    //            card_data.sessions_left,
    //            card_data.sessions_orig);
    
    // test for string creation
    // printf("Creating card strings for orig: %d\n", card_data.sessions_orig);
    // for (int i = -10; i < 1020; i += 10)
    // {
    //     card_data.sessions_left = i;
    //     status = Card_CreateDataString(card_str, &card_data);
    //     if (status == MI_OK)
    //         printf("%s\n", card_str);
    //     else
    //         printf("MI_ERR on left: %d orig: %d\n",
    //                card_data.sessions_left,
    //                card_data.sessions_orig);
    // }
    
    return 0;
}






//--- end of file ---//


