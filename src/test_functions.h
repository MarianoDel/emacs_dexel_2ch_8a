//------------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board #####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.H #########################
//------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _TEST_FUNTIONS_H_
#define _TEST_FUNTIONS_H_

#include "hard.h"

// Exported Types Constants and Macros -----------------------------------------


// Exported Functions ----------------------------------------------------------
void TF_Led (void);
void TF_ENA_CH1_ENA_CH2_SW_SEL (void);
void TF_SW_UP (void);
void TF_SW_DWN (void);
void TF_SW_SEL (void);
void TF_lcdE (void);
void TF_lcdRS (void);
void TF_lcdBklight (void);
void TF_lcdData (void);
void TF_lcdBlink (void);
void TF_lcdScroll (void);
void TF_MenuFunction (void);
void TF_Dmx_Packet (void);
void TF_Dmx_Packet_Data (void);
void TF_Pwm_Channels (void);
void TF_F_Channels_As_Pwm (void);
void TF_F_Channels_As_ICapture_2 (void);
void TF_F_Channels_As_ICapture_4 (void);
void TF_Temp_Channel (void);

#endif    /* _TEST_FUNTIONS_H_ */

//--- end of file ---//

