//----------------------------------------
// Tests for gtk lib
//----------------------------------------

#include <gtk/gtk.h>
// #include <stdint.h>
// #include "cgrom.h"

// #include "lcd_utils.h"
// #include "menues.h"
#include "switches_answers.h"

// Externals -------------------------------------------------------------------
extern GMutex mutex;
extern sw_actions_t switch_actions;


// Nedded by menues module provided by hard module
void UpdateEncoder (void)
{
}

resp_sw_t CheckSET (void)
{
    resp_sw_t sw = SW_NO;
    
    g_mutex_lock (&mutex);

    if (switch_actions == selection_enter)
        sw = SW_MIN;
    
    g_mutex_unlock (&mutex);
    
    return sw;    
}

unsigned char CheckCCW (void)
{
    unsigned char a = 0;
    
    g_mutex_lock (&mutex);

    if (switch_actions == selection_dwn)
        a = 1;
    
    g_mutex_unlock (&mutex);
    
    return a;
}


unsigned char CheckCW (void)
{
    unsigned char a = 0;
    
    g_mutex_lock (&mutex);

    if (switch_actions == selection_up)
        a = 1;
    
    g_mutex_unlock (&mutex);
    
    return a;
}


//--- end of file ---//
