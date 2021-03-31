//-----------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board ####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### CARD_MODE.H #############################
//-----------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _CARD_MODE_H_
#define _CARD_MODE_H_

#include "menues.h"
// Exported Types --------------------------------------------------------------


// Exported Constants or Macros ------------------------------------------------


// Exported Functions ----------------------------------------------------------
void Card_Mode_Standby_Reset (void);
void Card_Mode_Standby_Init (void);
void Card_Mode_Standby (mem_bkp_t *);
void Card_Mode_Timeouts (void);


#endif    /* _CARD_MODE_H_ */

//--- end of file ---//

