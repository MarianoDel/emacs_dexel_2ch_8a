//----------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEMPERATURES.C ###############################
//----------------------------------------------------

// Includes --------------------------------------------------------------------
#include "temperatures.h"


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
unsigned char Temp_TempToDegrees (unsigned short temp)
{
    if (temp < TEMP_IN_MIN)
        return TEMP_DEG_MIN;

    if (temp > TEMP_IN_MAX)
        return TEMP_DEG_MAX;
    
    unsigned int calc = 0;
    unsigned short dx = TEMP_IN_MAX - TEMP_IN_MIN;
    unsigned short dy = TEMP_DEG_MAX - TEMP_DEG_MIN;

    calc = temp * dy;
    calc = calc / dx;

    calc = calc - TEMP_DEG_OFFSET;

    return (unsigned char) calc;
    
}


unsigned short Temp_DegreesToTemp (unsigned char deg)
{
    if (deg < TEMP_DEG_MIN)
        return TEMP_IN_MIN;

    if (deg > TEMP_DEG_MAX)
        return TEMP_IN_MAX;
    
    unsigned int calc = 0;
    unsigned short dx = TEMP_DEG_MAX - TEMP_DEG_MIN;
    unsigned short dy = TEMP_IN_MAX - TEMP_IN_MIN;

    calc = deg * dy;
    calc = calc / dx;

    calc = calc + TEMP_IN_OFFSET;

    return (unsigned short) calc;
    
}

//--- end of file ---//
