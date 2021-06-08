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
// #include "gpio.h"


// Module Private Types & Macros -----------------------------------------------
typedef enum {
    MENU_INIT = 0,
    MENU_SHOW_MODE,    
    MENU_SHOW_CHANNEL,
    MENU_SHOW_HARD,
    MENU_SHOW_END_CONF,
    MENU_CONF_MODE,
    MENU_CONF_CHANNEL,
    MENU_CONF_HARD,
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
    case MENU_SHOW_MODE:
        
        resp = LCD_ShowSelectv2((const char *) "Operation Mode  ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_END_CONF;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_CHANNEL;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_MODE;
        }

        // if (sw_action != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;
        
    case MENU_SHOW_CHANNEL:
        
        resp = LCD_ShowSelectv2((const char *) "DMX channel sel ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_MODE;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_HARD;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_CHANNEL;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_SHOW_HARD:
        
        resp = LCD_ShowSelectv2((const char *) "Hardware config ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_CHANNEL;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_END_CONF;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_HARD;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;
        
    case MENU_SHOW_END_CONF:

        resp = LCD_ShowSelectv2((const char *) "Save new config ",
                                sw_action);

        if (resp == resp_change)
            menu_state = MENU_SHOW_HARD;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_MODE;

        if (resp == resp_selected)
        {
            LCD_EncoderOptionsOnOffReset();
            menu_state = MENU_END_CONF;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;
        
        break;
        
        // Configuration Menues
    case MENU_CONF_MODE:

        resp = LCD_EncoderShowSelect("Mode:  Manual   ",
                                     "Mode:  DMX      ",
                                     sw_action,
                                     &onoff);
                                     
        if (resp == resp_selected)
        {
            // if (onoff == 2)
            //     configurations->operation_mode = DMX_MODE;
            // else
            //     configurations->operation_mode = MANUAL_MODE;

            menu_state = MENU_SHOW_MODE;
            resp = resp_continue;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;

    case MENU_CONF_CHANNEL:

        // resp = LCD_EncoderChange("minutos:        ",
        resp = LCD_EncoderChange("first ch:       ",                                 
                                 &fchannel,
                                 1,
                                 511,
                                 sw_action);

        if (resp == resp_finish)
        {
            // configurations->first_channel = fchannel;
            menu_state = MENU_SHOW_CHANNEL;
            resp = resp_continue;
        }

        // onoff = configurations->alarms_onoff;
        // resp = LCD_EncoderOptionsOnOff("Alarma          ",
        //                                &onoff,
        //                                actions);

        // if (resp == resp_finish)
        // {
        //     // configurations->alarms_onoff = onoff;            
        //     menu_state = MENU_SHOW_CHANNEL;
        //     resp = resp_continue;
        // }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;


    case MENU_CONF_HARD:

        resp = LCD_EncoderChange("Max current:    ",
                                 &fchannel,
                                 1,
                                 10,
                                 sw_action);
                                     
        if (resp == resp_finish)
        {
            // if (onoff == 2)
            //     configurations->operation_mode = CARD_MODE;
            // else
            //     configurations->operation_mode = NORMAL_MODE;

            menu_state = MENU_SHOW_HARD;
            resp = resp_continue;
        }

        // if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;
        
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
        // else if (actions != selection_none)    //algo se cambio, aviso
        //     resp = resp_change;

        break;
        
    default:
        menu_state = MENU_INIT;
        break;
    }

    return resp;
}


//--- end of file ---//
