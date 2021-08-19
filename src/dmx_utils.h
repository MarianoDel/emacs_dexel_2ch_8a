//-----------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_UTILS.H #############################
//-----------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX_UTILS_H_
#define _DMX_UTILS_H_


// Exported Types --------------------------------------------------------------


// Exported Constants or Macros ------------------------------------------------


// Exported Functions ----------------------------------------------------------
void Percentage (unsigned char, unsigned char *, unsigned char *);
void ColorTemp1 (unsigned char dmx_value, unsigned short * temp);
void ColorTemp2 (unsigned char dmx_value, unsigned short * temp);

    
#endif    /* _DMX_UTILS_H_ */

//--- end of file ---//

