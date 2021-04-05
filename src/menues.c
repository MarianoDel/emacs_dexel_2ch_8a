//-----------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board ####
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
resp_t MENU_Main (mem_bkp_t * configurations)
{
    resp_t resp = resp_continue;
    sw_actions_t actions = selection_none;
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
            if (Check_SW_SEL() == SW_NO)
            {
                LCD_ShowSelectv2Reset();
                menu_state++;
            }
        }
        break;

        // Options Menues        
    case MENU_SHOW_MODE:
        actions = MENU_Check_Actions();
        
        resp = LCD_ShowSelectv2((const char *) "Operation Mode  ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_END_CONF;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_CHANNEL;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_MODE;
        }

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;
        
    case MENU_SHOW_CHANNEL:
        actions = MENU_Check_Actions();
        
        resp = LCD_ShowSelectv2((const char *) "DMX channel sel ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_MODE;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_HARD;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_CHANNEL;
        }

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;

    case MENU_SHOW_HARD:
        actions = MENU_Check_Actions();
        
        resp = LCD_ShowSelectv2((const char *) "Hardware config ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_CHANNEL;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_END_CONF;
        
        if (resp == resp_selected)
        {
            LCD_EncoderChangeReset();
            menu_state = MENU_CONF_HARD;
        }

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;
        
    case MENU_SHOW_END_CONF:
        actions = MENU_Check_Actions();

        resp = LCD_ShowSelectv2((const char *) "Save new config ",
                                actions);

        if (resp == resp_change)
            menu_state = MENU_SHOW_HARD;

        if (resp == resp_change_all_up)
            menu_state = MENU_SHOW_MODE;

        if (resp == resp_selected)
        {
            LCD_EncoderOptionsOnOffReset();
            menu_state = MENU_END_CONF;
        }

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;
        
        break;
        
        // Configuration Menues
    case MENU_CONF_MODE:
        actions = MENU_Check_Actions();

        resp = LCD_EncoderShowSelect("Mode:  Manual   ",
                                     "Mode:  DMX      ",
                                     actions,
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

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;

    case MENU_CONF_CHANNEL:
        actions = MENU_Check_Actions();

        // resp = LCD_EncoderChange("minutos:        ",
        resp = LCD_EncoderChange("first ch:       ",                                 
                                 &fchannel,
                                 1,
                                 511,
                                 actions);

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

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;


    case MENU_CONF_HARD:
        actions = MENU_Check_Actions();        

        resp = LCD_EncoderChange("Max current:    ",
                                 &fchannel,
                                 1,
                                 10,
                                 actions);
                                     
        if (resp == resp_finish)
        {
            // if (onoff == 2)
            //     configurations->operation_mode = CARD_MODE;
            // else
            //     configurations->operation_mode = NORMAL_MODE;

            menu_state = MENU_SHOW_HARD;
            resp = resp_continue;
        }

        if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;
        
        break;

        
    case MENU_END_CONF:
        actions = MENU_Check_Actions();

        onoff = 1;
        resp = LCD_EncoderOptionsOnOff("Save & Exit     ",
                                       &onoff,
                                       actions);

        if (resp == resp_finish)
        {
            if (onoff)
                resp = resp_need_to_save;
            else
                resp = resp_finish;

            menu_state = MENU_INIT;
        }
        else if (actions != selection_none)    //algo se cambio, aviso
            resp = resp_change;

        break;
        
    default:
        menu_state = MENU_INIT;
        break;
    }

    UpdateSwitches();
    
    return resp;
}


sw_actions_t MENU_Check_Actions (void)
{
    sw_actions_t actions = selection_none;
    
    if (Check_SW_SEL() > SW_NO)
        actions = selection_enter;

    if (Check_SW_DWN())
        actions = selection_dwn;

    if (Check_SW_UP())
        actions = selection_up;

    return actions;
}

// void MENU_Encendido_Reset (void)
// {
//     menu_enc_state = MENU_ENC_INIT;
// }


// resp_t MENU_Encendido (mem_bkp_t * configurations)
// {
//     resp_t resp = resp_continue;
//     sw_actions_t actions = selection_none;
//     unsigned short time = 0;

//     switch (menu_enc_state)
//     {
//     case MENU_ENC_INIT:
//         resp = LCD_ShowBlink ("Entrando en conf",
//                               "Tiempo Encendido",
//                               1,
//                               BLINK_DIRECT);

//         if (resp == resp_finish)
//         {
//             resp = resp_continue;
//             if (CheckSET() == SW_NO)
//             {
//                 LCD_EncoderChangeReset();
//                 menu_enc_state++;
//             }
//         }
//         break;
        
//     case MENU_ENC_TIEMPO_ENCENDIDO:
//         if (CheckSET() > SW_NO)
//             actions = selection_enter;

//         if (CheckCCW())
//             actions = selection_dwn;

//         if (CheckCW())
//             actions = selection_up;

//         time = configurations->treatment_time_min;
        
//         resp = LCD_EncoderChange("Encendido:      ",
//                                  &time,
//                                  MINIMUN_TIME_ALLOWED,
//                                  MAXIMUN_TIME_ALLOWED,
//                                  actions);

//         if (resp == resp_finish)
//         {
//             configurations->treatment_time_min = time;
//             menu_enc_state = MENU_ENC_INIT;
//         }
//         else if (actions != selection_none)    //algo se cambio, aviso
//             resp = resp_change;
                
//         break;

//     default:
//         menu_enc_state = MENU_ENC_INIT;
//         break;
//     }

//     UpdateEncoder();
    
//     return resp;
// }


//--- end of file ---//
