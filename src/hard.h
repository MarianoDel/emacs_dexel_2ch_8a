//----------------------------------------------
// #### PROYECTO GAUSSTEK O3 - Custom Board ####
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### HARD.H #################################
//----------------------------------------------
#ifndef _HARD_H_
#define _HARD_H_

#include "stm32f0xx.h"
#include "title_conf.h"
#include "switches_answers.h"

//-- Defines For Configuration -------------------
//---- Configuration for Hardware Versions -------
#define HARDWARE_VERSION_2_0    //micro F030R8T6
// #define HARDWARE_VERSION_1_0    //micro F030K6T6


// #define SOFTWARE_VERSION_1_1
#define SOFTWARE_VERSION_1_0


//---- Features Configuration -----------------
#ifdef INITIAL_MSG_GEN_O3    
#define ON_EMPTY_MEM_INIT_NORMAL_MODE
// #define ON_EMPTY_MEM_INIT_CARD_MODE
#endif

#ifdef INITIAL_MSG_WURTH
// #define ON_EMPTY_MEM_INIT_NORMAL_MODE
#define ON_EMPTY_MEM_INIT_CARD_MODE
#endif



//---- End of Features Configuration ----------

// Exported Pinout Names -------------------------------------------------------
#ifdef HARDWARE_VERSION_2_0
// GPIOC pin13
// GPIOC pin14
// GPIOC pin15    NC
// GPIOF pin0    
// GPIOF pin1    NC

//GPIOC pin0
//GPIOC pin1
//GPIOC pin2    ADC input V_SENSE_48V

//GPIOC pin3    
#define CTRL_BKL    ((GPIOC->ODR & 0x0008) != 0)
#define CTRL_BKL_ON    (GPIOC->BSRR = 0x00000008)
#define CTRL_BKL_OFF    (GPIOC->BSRR = 0x00080000)

//GPIOA pin0    
//GPIOA pin1    
//GPIOA pin2    
//GPIOA pin3    Lcd interface

//GPIOF pin4
#define LCD_E    ((GPIOF->ODR & 0x0010) != 0)
#define LCD_E_ON    (GPIOF->BSRR = 0x00000010)
#define LCD_E_OFF    (GPIOF->BSRR = 0x00100000)

//GPIOF pin5    
#define LCD_RS    ((GPIOF->ODR & 0x0020) != 0)
#define LCD_RS_ON    (GPIOF->BSRR = 0x00000020)
#define LCD_RS_OFF    (GPIOF->BSRR = 0x00200000)

//GPIOA pin4
#define LED    ((GPIOA->ODR & 0x0010) != 0)
#define LED_ON    (GPIOA->BSRR = 0x00000010)
#define LED_OFF    (GPIOA->BSRR = 0x00100000)

//GPIOA pin5    SPI1 clk
//GPIOA pin6    SPI1 miso
//GPIOA pin7    SPI1 mosi

//GPIOC pin4    RFID Reset
#define RFID_RST ((GPIOC->ODR & 0x0010) != 0)
#define RFID_RST_ON (GPIOC->BSRR = 0x00000010)
#define RFID_RST_OFF (GPIOC->BSRR = 0x00100000)

//GPIOC pin5    RFID Irq

//GPIOB pin0    RFID CS
#define RFID_CS ((GPIOB->ODR & 0x0001) != 0)
#define RFID_CS_ON (GPIOB->BSRR = 0x00000001)
#define RFID_CS_OFF (GPIOB->BSRR = 0x00010000)

//GPIOB pin1    UVC OUT
#define UVC_OUT ((GPIOB->IDR & 0x0002) != 0)

//GPIOB pin2    NC

//GPIOB pin10    
#define ACT_R4 ((GPIOB->ODR & 0x0400) != 0)
#define ACT_R4_ON (GPIOB->BSRR = 0x00000400)
#define ACT_R4_OFF (GPIOB->BSRR = 0x04000000)

//GPIOB pin11    
#define ACT_R3 ((GPIOB->ODR & 0x0800) != 0)
#define ACT_R3_ON (GPIOB->BSRR = 0x00000800)
#define ACT_R3_OFF (GPIOB->BSRR = 0x08000000)

//GPIOB pin12    NC

//GPIOB pin13    
#define ACT_R1 ((GPIOB->ODR & 0x2000) != 0)
#define ACT_R1_ON (GPIOB->BSRR = 0x00002000)
#define ACT_R1_OFF (GPIOB->BSRR = 0x20000000)

//GPIOB pin14 

//GPIOB pin15    
#define ACT_R2 ((GPIOB->ODR & 0x8000) != 0)
#define ACT_R2_ON (GPIOB->BSRR = 0x00008000)
#define ACT_R2_OFF (GPIOB->BSRR = 0x80000000)

//GPIOC pin6    NC
//GPIOC pin7    NC
//GPIOC pin8    NC
//GPIOC pin9    NC
    
//GPIOA pin8    
#define RELAY    ((GPIOA->ODR & 0x0100) != 0)
#define RELAY_ON    (GPIOA->BSRR = 0x00000100)
#define RELAY_OFF    (GPIOA->BSRR = 0x01000000)

//GPIOA pin9    Encoder Switch
#define ENC_SW ((GPIOA->IDR & 0x0200) == 0)
#define SWITCH_SET    ENC_SW

//GPIOA pin10    Encoder Data
#define ENC_DT ((GPIOA->IDR & 0x0400) == 0)

//GPIOA pin11    Encoder Clock
#define ENC_CLK ((GPIOA->IDR & 0x0800) == 0)

//GPIOA pin12    
#define AC_SYNC ((GPIOA->IDR & 0x1000) == 0)

//GPIOA pin13    NC

//GPIOF pin6    
#define BUZZER ((GPIOF->ODR & 0x0040) != 0)
#define BUZZER_ON    (GPIOF->BSRR = 0x00000040)
#define BUZZER_OFF    (GPIOF->BSRR = 0x00400000)

//GPIOF pin7    IR Sensor Input
#define IR_INPUT ((GPIOF->IDR & 0x0080) == 0)

//GPIOA pin14    NC
//GPIOA pin15    NC

//GPIOC pin10    NC
//GPIOC pin11    NC
//GPIOC pin12    NC

//GPIOD pin2    NC
//GPIOB pin3    NC

//GPIOB pin4     S2_PIR
#define S2_PIR ((GPIOB->IDR & 0x0010) != 0)

//GPIOB pin5     NC

//GPIOB pin6     USART1 Tx
//GPIOB pin7     USART1 Rx

//GPIOB pin8    S1_O3
#define S1_O3 ((GPIOB->IDR & 0x0100) == 0)
#define SWITCH_O3    S1_O3

//GPIOB pin9     NC
#endif    //HARDWARE_VERSION_2_0

#ifdef HARDWARE_VERSION_1_0
//GPIOA pin0    
//GPIOA pin1    
//GPIOA pin2    
//GPIOA pin3    Lcd interface

//GPIOA pin4
#define LCD_E    ((GPIOA->ODR & 0x0010) != 0)
#define LCD_E_ON    (GPIOA->BSRR = 0x00000010)
#define LCD_E_OFF    (GPIOA->BSRR = 0x00100000)

//GPIOA pin5    
#define LCD_RS    ((GPIOA->ODR & 0x0020) != 0)
#define LCD_RS_ON    (GPIOA->BSRR = 0x00000020)
#define LCD_RS_OFF    (GPIOA->BSRR = 0x00200000)

//GPIOA pin6
#define AC_SYNC ((GPIOA->IDR & 0x0040) == 0)

//GPIOA pin7
//GPIOB pin0
//GPIOB pin1    

//GPIOA pin8
#define RELAY    ((GPIOA->ODR & 0x0100) != 0)
#define RELAY_ON    (GPIOA->BSRR = 0x00000100)
#define RELAY_OFF    (GPIOA->BSRR = 0x01000000)

//GPIOA pin9    
//GPIOA pin10    
//GPIOA pin11    

//GPIOA pin12
#define LED ((GPIOA->ODR & 0x1000) != 0)
#define LED_ON GPIOA->BSRR = 0x00001000
#define LED_OFF GPIOA->BSRR = 0x10000000

//GPIOA pin13
//GPIOA pin14

//GPIOA pin15    
#define BUZZER ((GPIOA->ODR & 0x8000) != 0)
#define BUZZER_ON GPIOA->BSRR = 0x00008000
#define BUZZER_OFF GPIOA->BSRR = 0x80000000

//GPIOB pin3
#define SWITCH_O3 ((GPIOB->IDR & 0x0008) == 0)

//GPIOB pin4
#define ENC_CLK ((GPIOB->IDR & 0x0010) == 0)

//GPIOB pin5
#define ENC_DT ((GPIOB->IDR & 0x0020) == 0)

//GPIOB pin6
#define ENC_SW ((GPIOB->IDR & 0x0040) == 0)
#define SWITCH_SET    ENC_SW

//GPIOB pin7
#define CTRL_BKL    ((GPIOB->ODR & 0x0080) != 0)
#define CTRL_BKL_ON    (GPIOB->BSRR = 0x00000080)
#define CTRL_BKL_OFF    (GPIOB->BSRR = 0x00800000)

#endif    //HARDWARE_VER_1_0

// Exported Types & Macros -----------------------------------------------------
// Main Program States
typedef enum
{    
    MAIN_INIT = 0,
    MAIN_INIT_FROM_TREATMENT,
    MAIN_WELCOME,
    MAIN_NORMAL_MODE_STANDBY,
    MAIN_CARD_MODE_STANDBY,
    MAIN_START_TREATMENT,
    MAIN_IN_TREATMENT,
    MAIN_GO_PAUSE,
    MAIN_PAUSED,    
    MAIN_RESUMING,
    MAIN_ENDING_TREATMENT,
    MAIN_ENTERING_SET_OR_MENU,
    MAIN_WAIT_SET_OR_MENU,
    MAIN_SET_TIME,
    MAIN_IN_MAIN_MENU
    
} main_state_t;


// Buzzer Commands
#define BUZZER_STOP_CMD        0
#define BUZZER_LONG_CMD        1
#define BUZZER_HALF_CMD        2
#define BUZZER_SHORT_CMD        3
#define BUZZER_MULTI_CMD        4

// Buzzer Timeouts
#define TT_BUZZER_BIP_SHORT		100
#define TT_BUZZER_BIP_SHORT_WAIT	500
#define TT_BUZZER_BIP_HALF		600
#define TT_BUZZER_BIP_HALF_WAIT    	800
#define TT_BUZZER_BIP_LONG		2000
#define TT_BUZZER_BIP_LONG_WAIT        2000
// comando multiple tiempo
#define TT_BUZZER_BIP_MARK1    250
#define TT_BUZZER_BIP_SPACE1    500
#define TT_BUZZER_BIP_MARK2    250
#define TT_BUZZER_BIP_SPACE2    500
#define TT_BUZZER_BIP_MARK3    500
#define TT_BUZZER_BIP_SPACE3    500




//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_PAUSED    3
#define LED_NO_RFID    4

// Answers expected


#define SWITCHES_TIMER_RELOAD    5
#define SWITCHES_THRESHOLD_FULL	1000    //5 segundos
#define SWITCHES_THRESHOLD_HALF	50    //0.5 segundos
#define SWITCHES_THRESHOLD_MIN	5    //25 ms

#define ENCODER_COUNTER_ROOF    10
#define ENCODER_COUNTER_THRESHOLD    3

//--- Stringtify Utils -----------------------
#define str_macro(s) #s


// Module Exported Functions ---------------------------------------------------
void HARD_Timeouts (void);

void ChangeLed (unsigned char);
void UpdateLed (void);

void UpdateBuzzer (void);
void BuzzerCommands(unsigned char, unsigned char);

resp_sw_t CheckSET (void);
resp_sw_t CheckO3 (void);

void UpdateSwitches (void);

void UpdateEncoder (void);
unsigned char CheckCCW (void);
unsigned char CheckCW (void);

void RelayOn (void);
void RelayOff (void);
void UpdateRelay (void);
void RelaySyncHandler (void);
void RelayTimHandler (void);


#endif /* _HARD_H_ */

//--- end of file ---//

