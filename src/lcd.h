//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### LCD.H ################################
//---------------------------------------------

//--- LCD Configurations ---//
// Hardware configurations in lcd_hard_defs.h
//--- End of LCD Configurations ---//

#define CLEAR         0
#define RET_HOME      1
#define DISPLAY_ON    2
#define DISPLAY_OFF   3
#define CURSOR_ON     4
#define CURSOR_OFF    5
#define BLINK_ON      6
#define BLINK_OFF     7


#define gTimeBaseInterruptperms  1

void Lcd_WritePort (unsigned char);

void Lcd_Delay(unsigned short);
void Lcd_Command (unsigned char);
void Lcd_TransmitStr(const char * pStr);
// void LCDStartTransmit(unsigned char cData);
void Lcd_TransmitArray(unsigned char *pStr, unsigned char length);
void Lcd_sendcommand (unsigned char);
void Lcd_senddata (unsigned char);
// void Lcd_SetDDRAM (unsigned char addr);
#define Lcd_SetDDRAM(X)    Lcd_sendcommand((X)|0x80)
#define Lcd_SetCGRAM(X)    Lcd_sendcommand((X)|0x40)

#ifdef WITH_STATE_MACHINE
void LCDTransmitSM(void);
void LCDTransmitSMStr(char * send);
void LCD_callback (void);
void LcdSetDDRAMSM (unsigned char addr);
#endif

unsigned char SwapNibble (unsigned char);
void LCD_Init_Setup (void);

/*++++++++++++++++++++++++++++++ DON´T MODIFY +++++++++++++++++++++++++++++++*/

/* Possible status of LCD */
#define lcdStatusReady       1 /* LCD is ready to use                        */
#define lcdStatusError       2 /* Error ocurred when you are using the LCD   */
                               /*      and you try to execute other          */
                               /*      operation. You can change the state   */
                               /*      using LCDClear() function             */
#define lcdStatusPrinting    3 /* LCD is printing string                     */
#define lcdStatusInit        4 /* LCD is initializing                        */
#define lcdStatusWaiting     5 /* LCD status are waiting for ready mode      */
#define lcdStatusWaitingInit 0 /* LCD is waiting for initialization          */

/* Function declaration */
void  LCDInit(void);           /* Initialize the LCD                         */
void  LCDClear(void);          /* Clear the LCD                              */
void  LCD2L(void);             /* Go to second line of LCD                   */
void  LCDPrint(unsigned char *where, unsigned char length); /* Print from [where] memory     */
                               /*      address [length] characters           */
void  LCDTimeBase(void);       /* Time Base of LCD. This must be called      */
                               /*     each time that timerLCD is equal to 0  */
unsigned char LCDStatus(void);         /* Return the status of the LCD               */
void  LCDCursor(unsigned char ddramAddress); /* Send the address that you can put    */
                                     /*        the cursor                    */
void LCDSend(void);                                     
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
