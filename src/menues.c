//-----------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### MENUES.C ################################
//-----------------------------------------------

// Includes --------------------------------------------------------------------
#include "menues.h"
// #include "hard.h"
// #include "tim.h"
#include "lcd_utils.h"
#include "temperatures.h"


// Module Private Types & Macros -----------------------------------------------
typedef enum {
    MENU_INIT = 0,
    MENU_SHOW_MANUAL_OR_DMX,    
    MENU_SHOW_CHANNEL,
    MENU_SHOW_MAX_CURRENT,
    MENU_SHOW_OPERATION_MODE,
    MENU_SHOW_TEMP,    
    MENU_SHOW_END_CONF,
    MENU_CONF_MANUAL_OR_DMX,
    MENU_CONF_CHANNEL,
    MENU_CONF_MAX_CURRENT,
    MENU_CONF_OPERATION_MODE,
    MENU_CONF_TEMP,    
    MENU_END_CONF

} menu_state_t;


typedef enum {
    MENU_ENC_INIT = 0,
    MENU_ENC_TIEMPO_ENCENDIDO

} menu_enc_state_t;


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
menu_state_t menu_state = MENU_INIT;
menu_enc_state_t menu_enc_state = MENU_ENC_INIT;


// Module Private Functions ----------------------------------------------------
sw_actions_t MENU_Check_Actions (void);
unsigned char ConvertCurrentFromMemory (mem_bkp_t * config);
void ConvertCurrentToMemory (mem_bkp_t * config, unsigned char current);


// Module Functions ------------------------------------------------------------
void MENU_Main_Reset (void)
{
    menu_state = MENU_INIT;
}

//funcion de seleccion del menu principal
//devuelve nueva selección o estado anterior
resp_t MENU_Main (mem_bkp_t * configurations, sw_actions_t sw_action)
{
    resp_t resp = resp_continue;
    unsigned char onoff = 0;
    unsigned short fchannel = 0;

    switch (menu_state)
    {
    case MENU_INIT:
        resp = LCD_ShowBlink ("    Entering    ",
                              "    Main Menu   ",
                              1,
                              BLINK_DIRECT);

        if (resp == resp_finish)
        {
            resp = resp_continue;
            if (sw_action == selection_none)
            {
                LCD_ShowSelectv2Reset();
                menu_state++;
            }
        }
        break;

        // Options Menues        
    case MENU_SHOW_MANUAL_OR_DMX:
        
        resp = LCD_ShowSelectv2((const char *) "Manual or DMX?  ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_END_CONF;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_CHANNEL;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_MANUAL_OR_DMX;
        }

        // if (sw_action != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;
        
    case MENU_SHOW_CHANNEL:
        
        resp = LCD_ShowSelectv2((const char *) "DMX channel sel ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_MANUAL_OR_DMX;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_MAX_CURRENT;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_CHANNEL;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_SHOW_MAX_CURRENT:
        resp = LCD_ShowSelectv2((const char *) "Set max current ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_CHANNEL;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_OPERATION_MODE;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_MAX_CURRENT;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_SHOW_OPERATION_MODE:
        
        resp = LCD_ShowSelectv2((const char *) "Operation mode  ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_MAX_CURRENT;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_TEMP;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_OPERATION_MODE;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_SHOW_TEMP:
        
        resp = LCD_ShowSelectv2((const char *) "Temperature prot",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_OPERATION_MODE;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_END_CONF;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_TEMP;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;
        
    case MENU_SHOW_END_CONF:

        resp = LCD_ShowSelectv2((const char *) "Save new config ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_TEMP;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_MANUAL_OR_DMX;

        if (resp == resp_selected)
        {
            LCD_EncoderOptionsOnOffReset();
            menu_state = MENU_END_CONF;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;
        
        break;
        
        // Configuration Menues
    case MENU_CONF_MANUAL_OR_DMX:

        resp = LCD_EncoderShowSelect("Mode:  Manual   ",
                                     "Mode:  DMX      ",
                                     sw_action,
                                     &onoff);
                                     
        if (resp == resp_selected)
        {
            if (onoff == 2)
                configurations->program_type = DMX_MODE;
            else
                configurations->program_type = MANUAL_MODE;

            menu_state = MENU_SHOW_MANUAL_OR_DMX;
            resp = resp_continue;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_CONF_CHANNEL:
        fchannel = configurations->dmx_first_channel;

        resp = LCD_EncoderChange("first ch:       ",                                 
                                 &fchannel,
                                 1,
                                 510,
                                 sw_action);

        if (resp == resp_finish)
        {
            configurations->dmx_first_channel = fchannel;
            menu_state = MENU_SHOW_CHANNEL;
            resp = resp_continue;
        }

        break;


    case MENU_CONF_MAX_CURRENT:
        fchannel = ConvertCurrentFromMemory(configurations);
        
        resp = LCD_EncoderChange("Max current:    ",
                                 &fchannel,
                                 1,
                                 8,
                                 sw_action);
                                     
        if (resp == resp_finish)
        {
            ConvertCurrentToMemory(configurations, (unsigned char) fchannel);
            menu_state = MENU_SHOW_MAX_CURRENT;
            resp = resp_continue;
        }

        break;

    case MENU_CONF_OPERATION_MODE:

        resp = LCD_EncoderShowSelect("Bright and Temp ",
                                     "Direct channels ",
                                     sw_action,
                                     &onoff);
                                     
        if (resp == resp_selected)
        {
            if (onoff == 2)
                configurations->channels_operation_mode = 1;
            else
                configurations->channels_operation_mode = 0;

            menu_state = MENU_SHOW_OPERATION_MODE;
            resp = resp_continue;
        }
        
        break;

    case MENU_CONF_TEMP:

        fchannel = Temp_TempToDegrees(configurations->temp_prot);

        resp = LCD_EncoderChange("Max temp:       ",
                                 &fchannel,
                                 TEMP_DEG_MIN,
                                 TEMP_DEG_MAX,
                                 sw_action);
                                     
        if (resp == resp_finish)
        {
            configurations->temp_prot = Temp_DegreesToTemp(fchannel);

            menu_state = MENU_SHOW_TEMP;
            resp = resp_continue;
        }

        break;
        
        
        
    case MENU_END_CONF:

        onoff = 1;
        resp = LCD_EncoderOptionsOnOff("Save & Exit     ",
                                       &onoff,
                                       sw_action);

        if (resp == resp_finish)
        {
            if (onoff)
                resp = resp_need_to_save;
            else
                resp = resp_finish;

            menu_state = MENU_INIT;
        }
        
        break;
        
    default:
        menu_state = MENU_INIT;
        break;
    }

    return resp;
}


unsigned char ConvertCurrentFromMemory (mem_bkp_t * config)
{
    unsigned char curr = 0;

    if (config->max_current_channels[0] > 192)
        curr = 4;
    else if (config->max_current_channels[0] > 128)
        curr = 3;
    else if (config->max_current_channels[0] > 64)
        curr = 2;
    else
        curr = 1;
        
    if (config->current_eight_amps)
        curr += 4;

    return curr;
}

    
void ConvertCurrentToMemory (mem_bkp_t * config, unsigned char current)
{
    switch (current)
    {
    case 8:
        config->current_eight_amps = 1;
        config->max_current_channels[0] = 255;
        break;

    case 7:
        config->current_eight_amps = 1;
        config->max_current_channels[0] = 192;
        break;

    case 6:
        config->current_eight_amps = 1;
        config->max_current_channels[0] = 128;
        break;

    case 5:
        config->current_eight_amps = 1;
        config->max_current_channels[0] = 64;
        break;

    case 4:
        config->current_eight_amps = 0;
        config->max_current_channels[0] = 255;
        break;

    case 3:
        config->current_eight_amps = 0;
        config->max_current_channels[0] = 192;
        break;

    case 2:
        config->current_eight_amps = 0;
        config->max_current_channels[0] = 128;
        break;
 
    case 1:
    default:
        config->current_eight_amps = 0;
        config->max_current_channels[0] = 64;
        break;

    }
}
    
//--- end of file ---//
