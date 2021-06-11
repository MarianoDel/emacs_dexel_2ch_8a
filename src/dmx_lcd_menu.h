//-------------------------------------------------
// #### DEXEL 2CH PROJECT - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### DMX_LCD_MENU.C ############################
//-------------------------------------------------

// Prevent recursive inclusion -------------------------------------------------
#ifndef _DMX_LCD_MENU_H_
#define _DMX_LCD_MENU_H_

//-- includes to help configurations --//
#include "switches_answers.h"


// Module Exported Types Constants and Macros ----------------------------------
typedef struct {
    const unsigned short * dmx_first_chnl;
    const unsigned char * pchannels;
    unsigned char show_addres;
    unsigned char channels_mode;
    
} dmx_menu_data_t;


typedef struct {
    unsigned short dmx_address;
    unsigned char dmx_channels_qtty;
    sw_actions_t actions;
    unsigned short * timer;
    unsigned char * address_show;
    
} dmx_menu_address_data_t;


// Module Exported Functions ---------------------------------------------------
void DMXLcdMenuReset (void);
resp_t DMXLcdMenu (dmx_menu_data_t *);
void DMXLcdMenu_ChangeAddressReset (void);
resp_t DMXLcdMenu_ChangeAddress (dmx_menu_address_data_t *);


#endif    /* _DMX_LCD_MENU_H_ */

//--- end of file ---//
