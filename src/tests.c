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

typedef enum {
    SW_NO = 0,
    SW_MIN,
    SW_HALF,
    SW_FULL
    
} resp_sw_t;


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000    //5 segundos
#define SWITCHES_THRESHOLD_HALF	50    //250 ms
#define SWITCHES_THRESHOLD_MIN	10    //50 ms

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned short sw_sel_cntr = 0;


// Module Functions to Test ----------------------------------------------------
resp_sw_t Check_SW_SEL (void);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    resp_sw_t sw = SW_NO;
    int t = 0;

    printf("going up\n");
    for (int i = 0; i < 220; i++)
    {
        sw_sel_cntr = i;
        sw = Check_SW_SEL ();
        t += SWITCHES_TIMER_RELOAD;
        printf("t: %d cntr: %d sw: %d\n", t, sw_sel_cntr, sw);
    }
    
    printf("\ngoing down\n");
    
    for (int i = 221; i >= 0; i--)
    {
        sw_sel_cntr = i;
        sw = Check_SW_SEL ();
        t += SWITCHES_TIMER_RELOAD;
        printf("t: %d cntr: %d sw: %d\n", t, sw_sel_cntr, sw);
    }
    
        
    return 0;
}


// Embedded Functions to test ----

#define SWITCHES_PULSES_FULL    60    //3 segs
#define SWITCHES_PULSES_HALF    20    //1 seg
unsigned char sw_sel_pulses_cntr = 0;
resp_sw_t Check_SW_SEL (void)
{
    resp_sw_t sw = SW_NO;
    unsigned char current_pulses = 0;

    // check how many pulses on counter or reset pulses counter
    if (sw_sel_cntr)
    {
        for (unsigned char i = 0; i < SWITCHES_PULSES_FULL; i++)
        {
            if (sw_sel_cntr > (SWITCHES_THRESHOLD_MIN * i))
                current_pulses++;
            else
                i = SWITCHES_PULSES_FULL;
        }

        if (current_pulses > (sw_sel_pulses_cntr + 1))
        {
            sw_sel_pulses_cntr++;

            if (sw_sel_pulses_cntr > SWITCHES_PULSES_FULL)
                sw = SW_FULL;
            else if (sw_sel_pulses_cntr > SWITCHES_PULSES_HALF)
                sw = SW_HALF;
            else
                sw = SW_MIN;
        }
    }
    else
    {
        sw_sel_pulses_cntr = 0;
    }
    
    return sw;    
}



//--- end of file ---//


