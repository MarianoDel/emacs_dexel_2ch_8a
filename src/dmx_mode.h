//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_MODE.H ###############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX_MODE_H_
#define _DMX_MODE_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Exported Types Constants and Macros -----------------------------------------



// Exported Module Functions ---------------------------------------------------
resp_t DMXMode (unsigned char *, sw_actions_t);
void DMXModeReset (void);
void DMXMode_UpdateTimers (void);
unsigned short DMXGetPacketTimer (void);

#endif    /* _DMX_MODE_H_ */

//--- end of file ---//
