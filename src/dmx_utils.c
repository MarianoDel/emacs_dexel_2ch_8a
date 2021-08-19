//-----------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_UTILS.C #############################
//-----------------------------------------------

// Includes --------------------------------------------------------------------
#include "dmx_utils.h"


// Module Private Types & Macros -----------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
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


void ColorTemp1 (unsigned char dmx_value, unsigned short * temp)
{
    unsigned int calc = 0;

    calc = dmx_value * 3800;    // 6500 - 2700
    calc = calc / 255;
    calc = calc + 2700;
    *temp = (unsigned short) calc;
}


void ColorTemp2 (unsigned char dmx_value, unsigned short * temp)
{
    unsigned int calc = 0;

    calc = dmx_value * 2500;    // 5700 - 3200
    calc = calc / 255;
    calc = calc + 3200;
    *temp = (unsigned short) calc;
}

    
//--- end of file ---//
