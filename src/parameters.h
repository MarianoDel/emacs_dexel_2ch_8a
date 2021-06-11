//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### PARAMETERS.H ##########################
//---------------------------------------------

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

//-- Configuration for Struct --------------------
typedef enum {
    // program types (modes)
    DMX_MODE = 0,
    MANUAL_MODE
    
} programs_type_e;


//-- Memory Struct to Save --------------------
typedef struct parameters {

    //-- Program Type ----
    unsigned char program_type;

    //-- DMX mode configs. ----                  
    unsigned short dmx_first_channel;
    unsigned char dmx_channel_quantity;	

    //-- For Manual mode settings ----   //4
    unsigned char fixed_channels [2];

    //-- For Max Current Channels settings ----   //6
    unsigned char max_current_channels [2];    
    
    //-- For Temperature Protection ----    //8
    unsigned short temp_prot;

    //-- Current Set ----    //10
    unsigned char current_eight_amps;

    //-- Channels Operation Mode ----    //11
    unsigned char channels_operation_mode;
    
    // unsigned char dummy1;
    // unsigned char dummy2;          
    // unsigned char dummy3;       

    //-- End of Struct check for 4 bytes alignment ---- //12

} parameters_typedef;

//-- End of Memory Struct to Save --------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

