//------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F303
// ##
// #### MFRC522.C ################################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "mfrc522.h"
#include "stm32f0xx.h"
#include "uart.h"
#include "hard.h"
#include "spi.h"
#include "tim.h"

#include <stdio.h>

// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------
#define MFRC522_CS_ON    RFID_CS_OFF
#define MFRC522_CS_OFF    RFID_CS_ON
#define MFRC522_RST_ON    RFID_RST_OFF
#define MFRC522_RST_OFF    RFID_RST_ON


// Module Private Functions ----------------------------------------------------
void MFRC522_CS_Enable (void);
void MFRC522_CS_Disable (void);
void MFRC522_Hard_Reset (void);


// Module Functions ------------------------------------------------------------
void MFRC522_Init (void)
{
    MFRC522_Hard_Reset();
    MFRC522_CS_Disable();
    MFRC522_SelfTest();

    MFRC522_Reset();
    MFRC522_Write(MFRC522_TModeReg, 0x8D);
    MFRC522_Write(MFRC522_TPrescalerReg, 0x3E);
    MFRC522_Write(MFRC522_TReloadRegL, 30);
    MFRC522_Write(MFRC522_TReloadRegH, 0);
    MFRC522_Write(MFRC522_RFCfgReg, 0x70);			// 48dB gain
    MFRC522_Write(MFRC522_TxASKReg, 0x40);
    MFRC522_Write(MFRC522_ModeReg, 0x3D);

    MFRC522_AntennaOn();
}

void MFRC522_SelfTest (void)
{
    char s_buf [10] = { 0 };
    
    MFRC522_Write(MFRC522_CommandReg, PCD_RESETPHASE);
    MFRC522_Write(MFRC522_FIFOLevelReg, 0x80);
    for (uint8_t i = 0; i < 25; i++)
    {
        MFRC522_Write(MFRC522_FIFODataReg, 0x00);
    }

    MFRC522_Write(MFRC522_CommandReg, 0x01);
    MFRC522_Write(MFRC522_AutoTestReg, 0x09);
    MFRC522_Write(MFRC522_FIFODataReg, 0x00);
    MFRC522_Write(MFRC522_CommandReg, PCD_CALCCRC);
    for (uint8_t i = 0; i < 0xff; i++)
    {
        if (MFRC522_Read(MFRC522_FIFOLevelReg) >= 64)
        {
            break;
        }
    }
    
    MFRC522_Write(MFRC522_CommandReg, PCD_IDLE);

    Usart1Send("Self Test Results:\n");
    for (uint8_t i = 0; i < 8; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            unsigned char val = 0;
            val = MFRC522_Read(MFRC522_FIFODataReg);
            sprintf(s_buf, "0x%02x ", val);
            Usart1Send (s_buf);
            Wait_ms(10);
        }
        Usart1Send("\n");
    }

    MFRC522_Write(MFRC522_AutoTestReg, 0x00);
}


unsigned char MFRC522_Verify_Version (void)
{
    unsigned char version = 0;
    version = MFRC522_Read(MFRC522_VersionReg);
    return version;
}


void MFRC522_CS_Enable(void)
{
    MFRC522_CS_ON;
}


void MFRC522_CS_Disable(void)
{
    MFRC522_CS_OFF;
}

void MFRC522_Hard_Reset (void)
{
    volatile unsigned char delay;

    MFRC522_RST_ON;
    for (delay = 0; delay < 100; delay++)
    {
        asm (	"nop \n\t"
                "nop \n\t"
                "nop \n\t" );
    }
    MFRC522_RST_OFF;
}


uint8_t MFRC522_Check(uint8_t* id)
{
    uint8_t status;
    status = MFRC522_Request(PICC_REQIDL, id);	// Find cards, return card type
    if (status == MI_OK) {
        status = MFRC522_Anticoll(id);	// Card detected. Anti-collision, return card serial number 4 bytes
    }
    MFRC522_Halt();		// Command card into hibernation

    return status;
}


uint8_t MFRC522_Check_NoHalt(uint8_t* id)
{
    uint8_t status;
    status = MFRC522_Request(PICC_REQIDL, id);	// Find cards, return card type
    if (status == MI_OK) {
        status = MFRC522_Anticoll(id);	// Card detected. Anti-collision, return card serial number 4 bytes
    }

    return status;
}



// Used to exit the PCD from its authenticated state.
// Remember to call this function after communicating with an authenticated PICC
// otherwise no new communications can start.
void MFRC522_StopCrypto1 (void)
{
    // Clear MFCrypto1On bit
    // Status2Reg[7..0] bits are: TempSensClear I2CForceHS reserved reserved MFCrypto1On ModemState[2:0]
    // PCD_ClearRegisterBitMask(Status2Reg, 0x08);
    MFRC522_ClearBitMask (MFRC522_Status2Reg, 0x08);
}


uint8_t MFRC522_Compare(uint8_t * CardID, uint8_t * CompareID)
{
    uint8_t i;
    for (i = 0; i < 5; i++) {
        if (CardID[i] != CompareID[i])
            return MI_ERR;
    }

    return MI_OK;
}


void MFRC522_Write(uint8_t addr, uint8_t val)
{
    // Address format：0XXXXXX0
    addr = (addr << 1) & 0x7E;
    MFRC522_CS_Enable();

    //send the address
    SPI_Send_Single(addr);
    //send the data
    SPI_Send_Single(val);
    SPI_Busy_Wait();
    MFRC522_CS_Disable();
}


uint8_t MFRC522_Read(uint8_t addr)
{
    // assert_param(IS_RC522_REG(addr));    

    uint8_t val;
    // Address format：1XXXXXX0
    addr = ((addr << 1) & 0x7E) | 0x80;
    MFRC522_CS_Enable();
    //send the address to read
    SPI_Send_Receive(addr);
    val = SPI_Receive_Single();
    MFRC522_CS_Disable();

    return val;
}

void MFRC522_SetBitMask (uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = MFRC522_Read (reg);
    MFRC522_Write(reg, tmp | mask);  // set bit mask
}

void MFRC522_ClearBitMask (uint8_t reg, uint8_t mask)
{
    uint8_t tmp;
    tmp = MFRC522_Read(reg);
    MFRC522_Write(reg, tmp & (~mask));
}

void MFRC522_AntennaOn (void)
{
    uint8_t temp;

    temp = MFRC522_Read(MFRC522_TxControlReg);
    if (!(temp & 0x03)) {
        MFRC522_SetBitMask(MFRC522_TxControlReg, 0x03);
    }
}

void MFRC522_AntennaOff (void)
{
    MFRC522_ClearBitMask(MFRC522_TxControlReg, 0x03);
}

void MFRC522_Reset (void)
{
    MFRC522_Write(MFRC522_CommandReg, PCD_RESETPHASE);
}

uint8_t MFRC522_Request(uint8_t reqMode, uint8_t *TagType)
{
	uint8_t status;
	uint16_t backBits;	// The received data bits

	MFRC522_Write(MFRC522_BitFramingReg, 0x07);	// TxLastBists = BitFramingReg[2..0]
	TagType[0] = reqMode;
	status = MFRC522_ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
	if ((status != MI_OK) || (backBits != 0x10)) {
		status = MI_ERR;
	}

	return status;
}

uint8_t MFRC522_ToCard(uint8_t command,
                       uint8_t *sendData,
                       uint8_t sendLen,
                       uint8_t *backData,
                       uint16_t *backLen)
{
    uint8_t irqEn = 0x00;
    uint8_t waitIRq = 0x00;
    uint8_t lastBits;
    uint8_t n;
    uint16_t i;

    switch (command) {
    case PCD_AUTHENT: {
        irqEn = 0x12;
        waitIRq = 0x10;
        break;
    }
    case PCD_TRANSCEIVE: {
        irqEn = 0x77;
        waitIRq = 0x30;
        break;
    }
    default:
        break;
    }

    MFRC522_Write(MFRC522_ComlEnReg, irqEn | 0x80);
    MFRC522_ClearBitMask(MFRC522_ComIrqReg, 0x80);
    MFRC522_SetBitMask(MFRC522_FIFOLevelReg, 0x80);
    MFRC522_Write(MFRC522_CommandReg, PCD_IDLE);

    // Writing data to the FIFO
    for (i = 0; i < sendLen; i++) {
        MFRC522_Write(MFRC522_FIFODataReg, sendData[i]);
    }

    // Execute the command
    MFRC522_Write(MFRC522_CommandReg, command);
    if (command == PCD_TRANSCEIVE) MFRC522_SetBitMask(MFRC522_BitFramingReg, 0x80);					// StartSend=1,transmission of data starts

    // Waiting to receive data to complete
    i = 2000;	// i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms
    do {
        // CommIrqReg[7..0]
        // Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
        n = MFRC522_Read(MFRC522_ComIrqReg);
        i--;
    } while ((i!=0) && !(n & 0x01) && !(n & waitIRq));

    MFRC522_ClearBitMask(MFRC522_BitFramingReg, 0x80);	// StartSend=0

    if (i == 0) {
        return MI_ERR;
    }

    if ((MFRC522_Read(MFRC522_ErrorReg) & 0x1B)) {
        return MI_ERR;
    }

    uint8_t status = MI_OK;

    if (n & irqEn & 0x01) {
        status = MI_NOTAGERR;
    }
    if (command == PCD_TRANSCEIVE) {
        n = MFRC522_Read(MFRC522_FIFOLevelReg);
        lastBits = MFRC522_Read(MFRC522_ControlReg) & 0x07;
        if (lastBits) {
            *backLen = (n - 1) * 8 + lastBits;
        } else {
            *backLen = n * 8;
        }
        if (n == 0) {
            n = 1;
        }
        if (n > MFRC522_MAX_LEN) {
            n = MFRC522_MAX_LEN;
        }
        for (i = 0; i < n; i++) {
            backData[i] = MFRC522_Read(MFRC522_FIFODataReg);		// Reading the received data in FIFO
        }
    }

    return status;
}

uint8_t MFRC522_Anticoll(uint8_t *serNum)
{
    uint8_t status;
    uint8_t i;
    uint8_t serNumCheck = 0;
    uint16_t unLen;

    MFRC522_Write(MFRC522_BitFramingReg, 0x00);												// TxLastBists = BitFramingReg[2..0]
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = 0x20;
    status = MFRC522_ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
    if (status == MI_OK) {
        // Check card serial number
        for (i = 0; i < 4; i++) {
            serNumCheck ^= serNum[i];
        }
        if (serNumCheck != serNum[i]) {
            status = MI_ERR;
        }
    }

    return status;
}

void MFRC522_CalculateCRC(uint8_t *pIndata, uint8_t len, uint8_t *pOutData)
{
    uint8_t i, n;

    MFRC522_ClearBitMask(MFRC522_DivIrqReg, 0x04);	// CRCIrq = 0
    MFRC522_SetBitMask(MFRC522_FIFOLevelReg, 0x80);	// Clear the FIFO pointer
    // Write_MFRC522(CommandReg, PCD_IDLE);

    // Writing data to the FIFO
    for (i = 0; i < len; i++) MFRC522_Write(MFRC522_FIFODataReg, *(pIndata+i));
    MFRC522_Write(MFRC522_CommandReg, PCD_CALCCRC);

    // Wait CRC calculation is complete
    i = 0xFF;
    do {
        n = MFRC522_Read(MFRC522_DivIrqReg);
        i--;
    } while ((i!=0) && !(n & 0x04));	// CRCIrq = 1

    // Read CRC calculation result
    pOutData[0] = MFRC522_Read(MFRC522_CRCResultRegL);
    pOutData[1] = MFRC522_Read(MFRC522_CRCResultRegH);
}

uint8_t MFRC522_SelectTag(uint8_t *serNum)
{
    uint8_t i;
    uint8_t status;
    uint8_t size;
    uint16_t recvBits;
    uint8_t buffer[9];

    buffer[0] = PICC_SElECTTAG;
    buffer[1] = 0x70;
    for (i = 0; i < 5; i++) {
        buffer[i+2] = *(serNum+i);
    }
    MFRC522_CalculateCRC(buffer, 7, &buffer[7]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
    if ((status == MI_OK) && (recvBits == 0x18)) {
        size = buffer[0];
    } else {
        size = 0;
    }

    return size;
}

uint8_t MFRC522_Auth(uint8_t authMode, uint8_t BlockAddr, uint8_t *Sectorkey, uint8_t *serNum)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[12];

    // Verify the command block address + sector + password + card serial number
    buff[0] = authMode;
    buff[1] = BlockAddr;
    for (i = 0; i < 6; i++) buff[i+2] = *(Sectorkey+i);
    for (i=0; i<4; i++) buff[i+8] = *(serNum+i);
    status = MFRC522_ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
    if (status != MI_OK || !(MFRC522_Read(MFRC522_Status2Reg) & 0x08)) {
        status = MI_ERR;
    }

    return status;
}

uint8_t MFRC522_ReadBlock(uint8_t blockAddr, uint8_t *recvData)
{
    uint8_t status;
    uint16_t unLen;

    recvData[0] = PICC_READ;
    recvData[1] = blockAddr;
    MFRC522_CalculateCRC(recvData,2, &recvData[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
    if ((status != MI_OK) || (unLen != 0x90)) {
        status = MI_ERR;
    }

    return status;
}

uint8_t MFRC522_WriteBlock(uint8_t blockAddr, uint8_t *writeData)
{
    uint8_t status;
    uint16_t recvBits;
    uint8_t i;
    uint8_t buff[18];

    buff[0] = PICC_WRITE;
    buff[1] = blockAddr;
    MFRC522_CalculateCRC(buff, 2, &buff[2]);
    status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
        status = MI_ERR;
    }
    if (status == MI_OK) {
        // Data to the FIFO write 16Byte
        for (i = 0; i < 16; i++) {
            buff[i] = *(writeData+i);
        }
        MFRC522_CalculateCRC(buff, 16, &buff[16]);
        status = MFRC522_ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
        if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
            status = MI_ERR;
        }
    }
    return status;
}


void MFRC522_Halt (void)
{
    uint16_t unLen;
    uint8_t buff[4];

    buff[0] = PICC_HALT;
    buff[1] = 0;
    MFRC522_CalculateCRC(buff, 2, &buff[2]);
    MFRC522_ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}

//--- end of file ---//

