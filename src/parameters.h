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
typedef struct {

    unsigned char treatment_time_min;    //1
    unsigned char alarms_onoff;    //2
    unsigned char ticker_onoff;    //3
    unsigned short ticker_time;    //5

    unsigned int saved_psw;    //9
    unsigned int new_psw;    //13    
    unsigned char operation_mode;    //14
    
    unsigned char dummy1;    //15
    unsigned char dummy2;    //16
    // unsigned char dummy3;    //17


} parameters_typedef;

typedef enum {
    MODE_UNKNOW = 0,
    NORMAL_MODE,
    CARD_MODE
    
} operation_modes_t;

//-- End of Memory Struct to Save --------------------

#endif    /* _PARAMETERS_H_ */

//--- end of file ---//

