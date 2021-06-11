//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_LCD_MENU.C ############################
//-------------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx_lcd_menu.h"
#include "parameters.h"
#include "lcd_utils.h"


#include <stdio.h>
#include <string.h>


// Private Types Constants and Macros ------------------------------------------
typedef enum {
    DMX_MENU_INIT = 0,
    DMX_MENU_CHECK_CHNLS,
    DMX_MENU_CHECK_BRIGHTNESS

} dmx_menu_e;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------



static dmx_menu_e dmx_menu_state = DMX_MENU_INIT;


// Module Private Functions ----------------------------------------------------
void Percentage (unsigned char, unsigned char *, unsigned char *);


// Module Functions ------------------------------------------------------------
void DMXLcdMenuReset (void)
{
    dmx_menu_state = DMX_MENU_INIT;
}


resp_t DMXLcdMenu (dmx_menu_data_t * pmenu_data)
{
    resp_t resp = resp_continue;
    char s_temp[17] = { 0 };    //16 chars per line + '\0'

    switch (dmx_menu_state)
    {
    case DMX_MENU_INIT:
        if (pmenu_data->show_addres)
            sprintf(s_temp, "ADDR: %03d      D", *pmenu_data->dmx_first_chnl);
        else
            strcpy(s_temp, "ADDR:          D");                
        
        LCD_Writel1(s_temp);
        dmx_menu_state++;
        break;

    case DMX_MENU_CHECK_CHNLS:
        if (pmenu_data->channels_mode == 0)
        {
            sprintf(s_temp, "Brgt: %3d T: %3d",
                    *((pmenu_data->pchannels) + 0),
                    *((pmenu_data->pchannels) + 1));
        }
        else
        {
            sprintf(s_temp, "c1: %3d  c2: %3d",
                    *((pmenu_data->pchannels) + 0),
                    *((pmenu_data->pchannels) + 1));
        }
            
        LCD_Writel2(s_temp);
        dmx_menu_state++;
        break;

    case DMX_MENU_CHECK_BRIGHTNESS:
        dmx_menu_state = DMX_MENU_INIT;
        resp = resp_finish;
        break;
        
    default:
        dmx_menu_state = DMX_MENU_INIT;
        break;
    }

    return resp;
    
}


typedef enum {
    DO_NOTHING = 0,
    TO_CHANGE_WAIT_FREE,
    CHANGING,
    TO_DO_NOTHING_WAIT_FREE,
    TO_CLEAN_OUT
    
} dmx_address_e;


static dmx_address_e dmx_address_state = DO_NOTHING;
void DMXLcdMenu_ChangeAddressReset (void)
{
    dmx_address_state = DO_NOTHING;
}

#define TT_SHOW_ADDRESS    500
#define CNTR_TO_OUT    16
unsigned char dmx_address_cntr_out = 0;
resp_t DMXLcdMenu_ChangeAddress (dmx_menu_address_data_t * data)
{
    resp_t resp = resp_continue;
    sw_actions_t action = data->actions;
    unsigned short * address = &data->dmx_address;
    unsigned char channels = data->dmx_channels_qtty;
    unsigned short * timer_address = data->timer;
    unsigned char * address_show = data->address_show;
    
    switch (dmx_address_state)
    {
    case DO_NOTHING:
        if (action == selection_enter)
            dmx_address_state++;
        
        break;

    case TO_CHANGE_WAIT_FREE:
        if (action == selection_none)
        {
            dmx_address_cntr_out = CNTR_TO_OUT;
            dmx_address_state++;
        }
        break;
            
    case CHANGING:
        
        if (action == selection_up)
        {
            if (*address < (512 - channels))
            {
                *address += 1;
                *address_show = 1;
                
                *timer_address = TT_SHOW_ADDRESS;
                dmx_address_cntr_out = CNTR_TO_OUT;

                //force lcd update
                resp = resp_change;
            }
        }
        
        if (action == selection_dwn)
        {
            if (*address > 1)
            {
                *address -= 1;
                *address_show = 1;
                
                *timer_address = TT_SHOW_ADDRESS;
                dmx_address_cntr_out = CNTR_TO_OUT;
                
                //force lcd update
                resp = resp_change;
            }
        }

        if (action == selection_enter)
            dmx_address_state++;

        if (!*timer_address)
        {
            if (*address_show)
                *address_show = 0;
            else
                *address_show = 1;

            if (dmx_address_cntr_out)
                dmx_address_cntr_out--;
            
            *timer_address = TT_SHOW_ADDRESS;

            //force lcd update
            resp = resp_change;
        }

        if (!dmx_address_cntr_out)
            dmx_address_state = TO_CLEAN_OUT;
        
        break;

    case TO_DO_NOTHING_WAIT_FREE:
        if (action == selection_none)
            dmx_address_state++;
        
        break;

    case TO_CLEAN_OUT:
        resp = resp_finish;
        *address_show = 1;
        dmx_address_state = DO_NOTHING;
        break;
        
    default:
        dmx_address_state = DO_NOTHING;
        break;            
    }
    
    return resp;
}


void Percentage (unsigned char dmx_value, unsigned char * val_int, unsigned char * val_dec)
{
    unsigned int calc = 0;
    
    if (dmx_value == 0)
    {
        *val_int = 0;
        *val_dec = 0;
    }
    else if (dmx_value == 255)
    {
        *val_int = 100;
        *val_dec = 0;
    }
    else
    {
        calc = dmx_value * 1000;
        calc = calc / 255;        
        *val_int = calc / 10;
        *val_dec = calc - *val_int * 10;
    }
}


//--- end of file ---//
