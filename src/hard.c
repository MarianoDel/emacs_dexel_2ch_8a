//----------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.C #################################
//----------------------------------------------
#include "hard.h"
#include "stm32f0xx.h"


// Module Private Types & Macros -----------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
//for timers or timeouts
volatile unsigned char switches_timer = 0;



// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------


void HARD_Timeouts (void)
{
    if (switches_timer)
        switches_timer--;
}



unsigned short sw_up_cntr = 0;
unsigned short sw_dwn_cntr = 0;
unsigned short sw_sel_cntr = 0;
resp_sw_t Check_SW_UP (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_up_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_up_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_up_cntr > SWITCHES_THRESHOLD_MIN)
    {
        sw_up_cntr -= SWITCHES_THRESHOLD_MIN;
        sw = SW_MIN;
    }

    return sw;    
}


resp_sw_t Check_SW_DWN (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_dwn_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_dwn_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_dwn_cntr > SWITCHES_THRESHOLD_MIN)
    {
        sw_dwn_cntr -= SWITCHES_THRESHOLD_MIN;
        sw = SW_MIN;
    }

    return sw;    
}


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




void UpdateSwitches (void)
{
    if (!switches_timer)
    {
        if (SW_UP)
            sw_up_cntr++;
        else if (sw_up_cntr > 50)
            sw_up_cntr -= 50;
        else if (sw_up_cntr > 10)
            sw_up_cntr -= 5;
        else if (sw_up_cntr)
            sw_up_cntr--;

        if (SW_DWN)
            sw_dwn_cntr++;
        else if (sw_dwn_cntr > 50)
            sw_dwn_cntr -= 50;
        else if (sw_dwn_cntr > 10)
            sw_dwn_cntr -= 5;
        else if (sw_dwn_cntr)
            sw_dwn_cntr--;

        if (SW_SEL)
            sw_sel_cntr++;
        else if (sw_sel_cntr > 50)
            sw_sel_cntr -= 50;
        else if (sw_sel_cntr > 10)
            sw_sel_cntr -= 5;
        else if (sw_sel_cntr)
            sw_sel_cntr--;
        
        switches_timer = SWITCHES_TIMER_RELOAD;
    }       
}


sw_actions_t CheckActions (void)
{
    sw_actions_t sw = selection_none;
    
    if (Check_SW_UP () > SW_NO)
        sw = selection_up;

    if (Check_SW_DWN () > SW_NO)
        sw = selection_dwn;

    resp_sw_t s_sel = SW_NO;
    s_sel = Check_SW_SEL ();
    
    if (s_sel > SW_MIN)
        sw = selection_back;
    else if (s_sel > SW_NO)
        sw = selection_enter;
    
    return sw;
    
}

//--- end of file ---//


