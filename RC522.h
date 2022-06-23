#ifndef MFRC522_H
#define MFRC522_H

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <stdio.h>

#define MAX_LEN 18

#define PCD_IDLE              0x00               
#define PCD_AUTHENT           0x0E              
#define PCD_RECEIVE           0x08               
#define PCD_TRANSMIT          0x04               
#define PCD_TRANSCEIVE        0x0C               
#define PCD_RESETPHASE        0x0F            
#define PCD_CALCCRC           0x03              


#define PICC_REQIDL           0x26               
#define PICC_REQALL           0x52               
#define PICC_ANTICOLL         0x93            
#define PICC_ANTICOLL1        0x93            
#define PICC_ANTICOLL2        0x95             
#define PICC_AUTHENT1A        0x60              
#define PICC_AUTHENT1B        0x61              
#define PICC_READ             0x30               
#define PICC_WRITE            0xA0          
#define PICC_DECREMENT        0xC0              
#define PICC_INCREMENT        0xC1               
#define PICC_RESTORE          0xC2               
#define PICC_TRANSFER         0xB0            
#define PICC_HALT             0x50              


#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte

// PAGE 0
#define     RFU00                 0x00   
#define     CommandReg            0x01   
#define     CommIEnReg            0x02   
#define     DivlEnReg             0x03   
#define     CommIrqReg            0x04   
#define     DivIrqReg             0x05
#define     ErrorReg              0x06   
#define     Status1Reg            0x07   
#define     Status2Reg            0x08   
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1    
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2   
#define     RFU20                 0x20 
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3     
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39 
#define     TestDAC2Reg           0x3A  
#define     TestADCReg            0x3B  
#define     RFU3C                 0x3C  
#define     RFU3D                 0x3D  
#define     RFU3E                 0x3E  
#define     RFU3F    0x3F

#define MI_OK                          0
#define MI_NOTAGERR                    (-1)
#define MI_ERR                         (-2)
 

 void MFRC522_init();
 

void writeMFRC522(unsigned char Address, unsigned char value);

unsigned char readMFRC522(unsigned char Address);


void RFID_setBitMask(unsigned char reg, unsigned char mask);

void RFID_clearBitMask(unsigned char reg, unsigned char mask);

void RFID_antennaOn(void);

void RFID_antennaOff(void);

void RFID_calculateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData);

unsigned char RFID_MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen);

unsigned char RFID_findCard(unsigned char reqMode, unsigned char *TagType);

unsigned char RFID_anticoll(unsigned char *serNum);

unsigned char RFID_auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum);

unsigned char RFID_read(unsigned char blockAddr, unsigned char *recvData);

unsigned char RFID_write(unsigned char blockAddr, unsigned char *writeData);

unsigned char RFID_selectTag(unsigned char *serNum);

void RFID_halt();
 
#endif