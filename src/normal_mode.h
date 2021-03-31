//------------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board #####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### NORMAL_MODE.H ############################
//------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _NORMAL_MODE_H_
#define _NORMAL_MODE_H_

#include "menues.h"
// Exported Types --------------------------------------------------------------


// Exported Constants or Macros ------------------------------------------------


// Exported Functions ----------------------------------------------------------
void Normal_Mode_Standby_Reset (void);
void Normal_Mode_Standby (mem_bkp_t *);
void Normal_Mode_Timeouts (void);


#endif    /* _NORMAL_MODE_H_ */

//--- end of file ---//

