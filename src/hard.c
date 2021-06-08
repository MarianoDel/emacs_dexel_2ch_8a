//----------------------------------------------
// #### PROYECTO GAUSSTEK O3 - Custom Board ####
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
#define LED_BLINK_ON    LED_ON
#define LED_BLINK_OFF    LED_OFF

// Led Blinking States
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE
    
} led_state_t;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
//for timers or timeouts
volatile unsigned short timer_led = 0;
volatile unsigned char switches_timer = 0;


//for the led
led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}


void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_BLINK_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_BLINK_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_BLINK_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


void HARD_Timeouts (void)
{
    if (timer_led)
        timer_led--;

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
        sw = SW_MIN;

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
        sw = SW_MIN;

    return sw;    
}


resp_sw_t Check_SW_SEL (void)
{
    resp_sw_t sw = SW_NO;
    
    if (sw_sel_cntr > SWITCHES_THRESHOLD_FULL)
        sw = SW_FULL;
    else if (sw_sel_cntr > SWITCHES_THRESHOLD_HALF)
        sw = SW_HALF;
    else if (sw_sel_cntr > SWITCHES_THRESHOLD_MIN)
        sw = SW_MIN;

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

    if (Check_SW_SEL () > SW_NO)
        sw = selection_enter;

    if (Check_SW_SEL () > SW_MIN)
        sw = selection_back;
    
    return sw;
    
}

//--- end of file ---//


