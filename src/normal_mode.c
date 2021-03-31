//------------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board #####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### NORMAL_MODE.C ############################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "normal_mode.h"
#include "lcd_utils.h"
#include "title_conf.h"

#include "hard.h"

#include <stdio.h>



// Module Private Types & Macros -----------------------------------------------
typedef enum {
    NORMAL_INIT = 0,
    NORMAL_STANDBY
    
} normal_mode_t;


// Externals -------------------------------------------------------------------
// extern char s_blank [];


// Globals ---------------------------------------------------------------------
unsigned char normal_internal = 0;
normal_mode_t normal_mode_state = NORMAL_INIT;
volatile unsigned short tt_normal_internal = 0;

// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Normal_Mode_Standby_Reset (void)
{
    normal_mode_state = NORMAL_INIT;
}

void Normal_Mode_Standby (mem_bkp_t * configurations)
{
    char s_lcd [20] = { 0 };
    
    switch (normal_mode_state)
    {
    case NORMAL_INIT:
#ifdef INITIAL_MSG_WURTH
        LCD_Writel1("WURTH ARGENTINA ");
#endif
#ifdef INITIAL_MSG_GEN_O3
        LCD_Writel1("GENERADOR OZONO ");        
#endif
        normal_internal = 0;
        ChangeLed(LED_TREATMENT_STANDBY);
        normal_mode_state++;
        break;

    case NORMAL_STANDBY:
        switch (normal_internal)
        {
        case 0:
            sprintf (s_lcd, "Tiempo: %3d     ",
                     configurations->treatment_time_min);

            LCD_Writel2(s_lcd);
            tt_normal_internal = 1200;
            normal_internal++;
            break;

        case 1:
            if (!tt_normal_internal)
            {
                LCD_Writel2("O3: Inicia      ");
                tt_normal_internal = 1200;
                normal_internal++;
            }
            break;

        case 2:
            if (!tt_normal_internal)
            {
                LCD_Writel2("Set: conf Tiempo");    
                tt_normal_internal = 1200;
                normal_internal++;
            }
            break;

        case 3:
            if (!tt_normal_internal)
                normal_internal = 0;

            break;
                
        default:
            normal_internal = 0;
            break;
        }
        break;
            
    default:
        normal_mode_state = NORMAL_INIT;        
        break;
    }
}


void Normal_Mode_Timeouts (void)
{
    if (tt_normal_internal)
        tt_normal_internal--;
}

//--- end of file ---//
