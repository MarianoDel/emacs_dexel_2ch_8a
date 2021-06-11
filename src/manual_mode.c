//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MANUAL_MODE.C #############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "manual_mode.h"
#include "lcd_utils.h"
#include "parameters.h"

#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    MANUAL_MODE_INIT = 0,
    MANUAL_MODE_RUNNING

} manual_mode_e;


#define TT_MENU_TIMEOUT    30000


// variables re-use
#define manual_mode_state    mode_state
#define mm_menu_timer    mode_effect_timer

#define SHOW_ALL    0
#define SHOW_ONLY_TEMP    1
#define SHOW_ONLY_BRIGHT    2

#define TT_SHOW_OPTIONS    500
#define CNTR_TO_OUT    16

// Externals -------------------------------------------------------------------
// -- externals re-used
extern unsigned char mode_state;
extern volatile unsigned short mode_effect_timer;

#define mm_menu_timer    mode_effect_timer

extern parameters_typedef mem_conf;

// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void ManualMode_MenuReset (void);
resp_t ManualMode_Menu (unsigned char *, sw_actions_t);
void DataShow (unsigned char state, unsigned char bright, unsigned char temp, unsigned char ch_mode);

// Module Functions ------------------------------------------------------------
void ManualMode_UpdateTimers (void)
{
    if (mm_menu_timer)
        mm_menu_timer--;
}


void ManualModeReset (void)
{
    manual_mode_state = MANUAL_MODE_INIT;
}



resp_t ManualMode (unsigned char * ch_val, sw_actions_t action)
{
    resp_t resp = resp_continue;
    
    switch (manual_mode_state)
    {
    case MANUAL_MODE_INIT:
        ManualMode_MenuReset();
        manual_mode_state++;
        break;

    case MANUAL_MODE_RUNNING:
        resp = ManualMode_Menu(ch_val, action);

        break;

    default:
        manual_mode_state = MANUAL_MODE_INIT;
        break;
    }

    
    if (resp == resp_finish)
    {
        //end of changing ask for a memory save
        resp = resp_need_to_save;
        
    }
    
    return resp;
            
}


typedef enum {
    MM_MENU_INIT = 0,
    MM_MENU_CHECK_SWITCHES,
    MM_MENU_SELECT_BRIGHTNESS_START,
    MM_MENU_SELECT_BRIGHTNESS,
    MM_MENU_SELECT_TEMP_START,
    MM_MENU_SELECT_TEMP,
    MM_MENU_CLEAN_OUT
    
} manual_mode_menu_state_e;


manual_mode_menu_state_e mm_menu_state;
void ManualMode_MenuReset (void)
{
    mm_menu_state = MM_MENU_INIT;
}


unsigned char mm_menu_cntr_out = 0;
unsigned char show_option = 0;
resp_t ManualMode_Menu (unsigned char * ch, sw_actions_t sw)
{
    resp_t resp = resp_continue;

    switch (mm_menu_state)
    {
    case MM_MENU_INIT:
        LCD_Writel1("  Manual Mode   ");

        DataShow (SHOW_ALL,
                  *(ch + 0),
                  *(ch + 1),
                  mem_conf.channels_operation_mode);
        
        mm_menu_state++;
        break;

    case MM_MENU_CHECK_SWITCHES:
        if (sw == selection_enter)
        {
            DataShow (SHOW_ONLY_TEMP,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);
            
            mm_menu_state++;
        }
        break;

    case MM_MENU_SELECT_BRIGHTNESS_START:
        if (sw == selection_none)
        {
            show_option = 0;
            mm_menu_timer = TT_SHOW_OPTIONS;
            mm_menu_cntr_out = CNTR_TO_OUT;
            mm_menu_state++;
        }
        break;

    case MM_MENU_SELECT_BRIGHTNESS:
        if (sw == selection_up)
        {
            if (*(ch + 0) < 255)
                *(ch + 0) += 1;

            show_option = 1;
            DataShow (SHOW_ALL,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);
            
            mm_menu_timer = TT_SHOW_OPTIONS;

            resp = resp_change;
        }

        if (sw == selection_dwn)
        {
            if (*(ch + 0) > 0)
                *(ch + 0) -= 1;

            show_option = 1;
            DataShow (SHOW_ALL,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);

            mm_menu_timer = TT_SHOW_OPTIONS;

            resp = resp_change;            
        }

        if (sw == selection_enter)
        {
            DataShow (SHOW_ONLY_BRIGHT,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);

            mm_menu_state = MM_MENU_SELECT_TEMP_START;
        }

        if (!mm_menu_timer)
        {
            if (show_option)
            {
                show_option = 0;
                DataShow (SHOW_ONLY_TEMP,
                          *(ch + 0),
                          *(ch + 1),
                          mem_conf.channels_operation_mode);
            }
            else
            {
                show_option = 1;
                DataShow (SHOW_ALL,
                          *(ch + 0),
                          *(ch + 1),
                          mem_conf.channels_operation_mode);
            }

            if (mm_menu_cntr_out)
                mm_menu_cntr_out--;
            
            mm_menu_timer = TT_SHOW_OPTIONS;
        }

        if (!mm_menu_cntr_out)
            mm_menu_state = MM_MENU_CLEAN_OUT;

        break;

    case MM_MENU_SELECT_TEMP_START:
        if (sw == selection_none)
        {
            show_option = 0;
            mm_menu_timer = TT_SHOW_OPTIONS;
            mm_menu_cntr_out = CNTR_TO_OUT;
            mm_menu_state++;
        }
        break;
        
    case MM_MENU_SELECT_TEMP:
        if (sw == selection_up)
        {
            if (*(ch + 1) < 255)
                *(ch + 1) += 1;

            show_option = 1;
            DataShow (SHOW_ALL,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);
            
            mm_menu_timer = TT_SHOW_OPTIONS;

            resp = resp_change;            
        }

        if (sw == selection_dwn)
        {
            if (*(ch + 1) > 0)
                *(ch + 1) -= 1;

            show_option = 1;
            DataShow (SHOW_ALL,
                      *(ch + 0),
                      *(ch + 1),
                      mem_conf.channels_operation_mode);
            
            mm_menu_timer = TT_SHOW_OPTIONS;

            resp = resp_change;            
        }

        if (sw == selection_enter)
        {
            mm_menu_state = MM_MENU_CLEAN_OUT;
        }

        if (!mm_menu_timer)
        {
            if (show_option)
            {
                show_option = 0;
                DataShow (SHOW_ONLY_BRIGHT,
                          *(ch + 0),
                          *(ch + 1),
                          mem_conf.channels_operation_mode);
            }
            else
            {
                show_option = 1;
                DataShow (SHOW_ALL,
                          *(ch + 0),
                          *(ch + 1),
                          mem_conf.channels_operation_mode);
            }

            if (mm_menu_cntr_out)
                mm_menu_cntr_out--;
            
            mm_menu_timer = TT_SHOW_OPTIONS;
        }

        if (!mm_menu_cntr_out)
            mm_menu_state = MM_MENU_CLEAN_OUT;

        break;
        
    case MM_MENU_CLEAN_OUT:
        // show_option = 0;
        mm_menu_state = MM_MENU_INIT;

        resp = resp_finish;
        break;

    default:
        mm_menu_state = MM_MENU_INIT;
        break;
    }

    return resp;
    
}


void DataShow (unsigned char state, unsigned char bright, unsigned char temp, unsigned char ch_mode)
{
    char s_temp[17] = { 0 };    //16 chars per line + '\0'

    switch (state)
    {
    case SHOW_ALL:
        if (ch_mode == 0)
        {
            sprintf(s_temp, "Brgt: %3d T: %3d",
                    bright,
                    temp);
        }
        else
        {
            sprintf(s_temp, "c1: %3d  c2: %3d",
                    bright,
                    temp);
        }
        break;

    case SHOW_ONLY_TEMP:
        if (ch_mode == 0)
        {
            sprintf(s_temp, "Brgt:     T: %3d",
                    temp);
        }
        else
        {
            sprintf(s_temp, "c1:      c2: %3d",
                    temp);
        }
        break;

    case SHOW_ONLY_BRIGHT:
        if (ch_mode == 0)
        {
            sprintf(s_temp, "Brgt: %3d T:    ",
                    bright);
        }
        else
        {
            sprintf(s_temp, "c1: %3d  c2:    ",        
                    bright);
        }
        break;
    }

    LCD_Writel2(s_temp);    
}


//--- end of file ---//
