#include "RC522.h"
#include <stdint.h> //khai bao thu vien uint8_t
#include <unistd.h>
#include <mysql.h>
#include <stdio.h>
#include <wiringPi.h>

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW *row;

char *server = "localhost";
char *user = "TRAN";
char *password = "pi";
char *database = "RC522";

#define channel 0
#define buzz 2
#define leddo 3
#define ledxanh 0
void MFRC522_init()
{
 
  //SPI
  wiringPiSPISetup(channel, 8000000); //SPI0, 13MHz

 
  writeMFRC522(CommandReg, PCD_RESETPHASE);
 
  writeMFRC522(TModeReg, 0x8D);   //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
  writeMFRC522(TPrescalerReg, 0x3E);  //TModeReg[3..0] + TPrescalerReg
  writeMFRC522(TReloadRegL, 30);
  writeMFRC522(TReloadRegH, 0);
  writeMFRC522(TxAutoReg, 0x40);    //100%ASK
  writeMFRC522(ModeReg, 0x3D);    // CRC valor inicial de 0x6363
 
  RFID_antennaOn();    
}
 
void writeMFRC522(unsigned char Address, unsigned char value)
{
    char buff[2];
 
    buff[0] = (char)((Address<<1)&0x7E);
    buff[1] = (char)value;
     
    wiringPiSPIDataRW(channel,buff,2);  
}
 
unsigned char readMFRC522(unsigned char Address)
{
    char buff[2];
    buff[0] = ((Address<<1)&0x7E)|0x80;
    wiringPiSPIDataRW(channel,buff,2);
    return (uint8_t)buff[1];
}
 
void RFID_setBitMask(unsigned char reg, unsigned char mask)
{
  unsigned char tmp;
  tmp = readMFRC522(reg);
  writeMFRC522(reg, tmp | mask);  // set bit mask
}
 
void RFID_clearBitMask(unsigned char reg, unsigned char mask)
{
  unsigned char tmp;
  tmp = readMFRC522(reg);
  writeMFRC522(reg, tmp & (~mask));  // clear bit mask
}

void RFID_antennaOn(void)
{
  unsigned char temp;
 
  temp = readMFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    RFID_setBitMask(TxControlReg, 0x03);
  }
}
 
void RFID_antennaOff(void)
{
  unsigned char temp;
 
  temp = readMFRC522(TxControlReg);
  if (!(temp & 0x03))
  {
    RFID_clearBitMask(TxControlReg, 0x03);
  }
}
 
void RFID_calculateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
  unsigned char i, n;
 
  RFID_clearBitMask(DivIrqReg, 0x04);      //CRCIrq = 0
  RFID_setBitMask(FIFOLevelReg, 0x80);     
  //Write_MFRC522(CommandReg, PCD_IDLE);
 
  for (i=0; i<len; i++)
    writeMFRC522(FIFODataReg, *(pIndata+i));
  writeMFRC522(CommandReg, PCD_CALCCRC);
 
  i = 0xFF;
  do
  {
    n = readMFRC522(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));      //CRCIrq = 1
 
  pOutData[0] = readMFRC522(CRCResultRegL);
  pOutData[1] = readMFRC522(CRCResultRegM);
}
 
unsigned char RFID_MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
  unsigned char status = MI_ERR;
  unsigned char irqEn = 0x00;
  unsigned char waitIRq = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;
 
  switch (command)
  {
    case PCD_AUTHENT:   
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:  
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
  }
 
  writeMFRC522(CommIEnReg, irqEn|0x80); 
  RFID_clearBitMask(CommIrqReg, 0x80);       
  RFID_setBitMask(FIFOLevelReg, 0x80);       
 
  writeMFRC522(CommandReg, PCD_IDLE);  
 
  for (i=0; i<sendLen; i++)
    writeMFRC522(FIFODataReg, sendData[i]);
 
  writeMFRC522(CommandReg, command);
  if (command == PCD_TRANSCEIVE)
    RFID_setBitMask(BitFramingReg, 0x80);    //StartSend=1,transmission of data starts
 
  
  i = 2000; 
  do
  {
    //CommIrqReg[7..0]
    //Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
    n = readMFRC522(CommIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitIRq));
 
  RFID_clearBitMask(BitFramingReg, 0x80);      //StartSend=0
 
  if (i != 0)
  {
    if(!(readMFRC522(ErrorReg) & 0x1B)) //BufferOvfl Collerr CRCErr ProtecolErr
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
        status = MI_NOTAGERR;     //??
 
      if (command == PCD_TRANSCEIVE)
      {
        n = readMFRC522(FIFOLevelReg);
        lastBits = readMFRC522(ControlReg) & 0x07;
        if (lastBits)
          *backLen = (n-1)*8 + lastBits;
        else
          *backLen = n*8;
 
        if (n == 0)
          n = 1;
        if (n > MAX_LEN)
          n = MAX_LEN;
 
        
        for (i=0; i<n; i++)
          backData[i] = readMFRC522(FIFODataReg);
      }
    }
    else
      status = MI_ERR;
  }
 
  //SetBitMask(ControlReg,0x80);           //timer stops
  //Write_MFRC522(CommandReg, PCD_IDLE);
 
  return status;
}

unsigned char RFID_findCard(unsigned char reqMode, unsigned char *TagType)
{
  unsigned char status;
  unsigned int backBits;      
 
  writeMFRC522(BitFramingReg, 0x07);    //TxLastBists = BitFramingReg[2..0] ???
 
  TagType[0] = reqMode;
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
 
  if ((status != MI_OK) || (backBits != 0x10))
    status = MI_ERR;
 
  return status;
}
 
unsigned char RFID_anticoll(unsigned char *serNum)
{
  unsigned char status;
  unsigned char i;
  unsigned char serNumCheck=0;
  unsigned int unLen;
 
  RFID_clearBitMask(Status2Reg, 0x08);   //TempSensclear
  RFID_clearBitMask(CollReg,0x80);     //ValuesAfterColl
  writeMFRC522(BitFramingReg, 0x00);    //TxLastBists = BitFramingReg[2..0]
 
  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
 
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);
 
  if (status == MI_OK)
  {
    
    for (i=0; i<4; i++){
      *(serNum+i)  = serNum[i];
      serNumCheck ^= serNum[i];
    }
    if (serNumCheck != serNum[i]){
      status = MI_ERR;
    }
  }
 
  RFID_setBitMask(CollReg, 0x80);    //ValuesAfterColl=1
 
  return status;
}
 
unsigned char RFID_auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[12];
 
  
  buff[0] = authMode;
  buff[1] = BlockAddr;
  for (i=0; i<6; i++)
    buff[i+2] = *(Sectorkey+i);
  for (i=0; i<4; i++)
    buff[i+8] = *(serNum+i);
     
  status = RFID_MFRC522ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
  if ((status != MI_OK) || (!(readMFRC522(Status2Reg) & 0x08)))
    status = MI_ERR;
 
  return status;
}
 
unsigned char RFID_read(unsigned char blockAddr, unsigned char *recvData)
{
  unsigned char status;
  unsigned int unLen;
 
  recvData[0] = PICC_READ;
  recvData[1] = blockAddr;
  RFID_calculateCRC(recvData,2, &recvData[2]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
 
  if ((status != MI_OK) || (unLen != 0x90))
    status = MI_ERR;
 
  return status;
}
 
unsigned char RFID_write(unsigned char blockAddr, unsigned char *writeData)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[18];
 
  buff[0] = PICC_WRITE;
  buff[1] = blockAddr;
  RFID_calculateCRC(buff, 2, &buff[2]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);
 
  if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    status = MI_ERR;
 
  if (status == MI_OK)
  {
    for (i=0; i<16; i++)    //?FIFO?16Byte?? Datos a la FIFO 16Byte escribir
      buff[i] = *(writeData+i);
       
    RFID_calculateCRC(buff, 16, &buff[16]);
    status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);
 
    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
      status = MI_ERR;
  }
 
  return status;
}

unsigned char RFID_selectTag(unsigned char *serNum)
{
  unsigned char i;
  unsigned char status;
  unsigned char size;
  unsigned int recvBits;
  unsigned char buffer[9];
 
  //ClearBitMask(Status2Reg, 0x08);                        //MFCrypto1On=0
 
  //buffer[0] = PICC_SElECTTAG;
  buffer[0] = PICC_ANTICOLL1;
  buffer[1] = 0x70;
 
  for (i=0; i<5; i++)
    buffer[i+2] = *(serNum+i);
 
  RFID_calculateCRC(buffer, 7, &buffer[7]);
   
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
  if ((status == MI_OK) && (recvBits == 0x18))
    size = buffer[i];
  else
    size = 0;
  return size;
}

void RFID_halt()
{
  unsigned char status;
  unsigned int unLen;
  unsigned char buff[4];
 
  buff[0] = PICC_HALT;
  buff[1] = 0;
  RFID_calculateCRC(buff, 2, &buff[2]);
 
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}
//-----------------------------------------------------
//Chuong trinh chinh
char the1[6] = {0x71,0xF6,0x3C,0x1C,0xA7};
char the2[6] = {0x81,0x03,0xC9,0x1C,0x57};
char the3[6] = {0x7D,0xA6,0xCF,0x10,0x04};
char the4[6] = {0x90,0x66,0xC9,0x21,0x1E};
char the5[6] = {113,226,104,28,231};
char the6[6] = {129,72,7,28,210};
char the7[6] = {113,69,190,28,150};
char the8[6] = {113,252,12,28,157};
char the9[6] = {113,154,21,28,226};
char the10[6] = {97,186,208,28,23};
char the11[6] = {115,102,9,29,1};
char id[6];
char hoten[11][40]={"Võ Công Thức","Lê Quốc An","Nguyễn Hoàng Nhật Trường","Nguyễn Hoài Nam","Trần Ngô Minh Trí","Nguyễn Văn Tòng","Hà Tấn Phát","Huỳnh Lê Thuận Phát","Nguyễn Nhật Tiến","Huỳnh Đoàn Trung Tín","Hồ Bảo Trân"};
char mssv[11][10]={"19146272","19146142","19146292","19146219","19146033","19146279","19146231","19146232","19146299","19146277","19146280"};
char lop[11][10]={"19146CL2","19146LC2","19146CL3","19146CL2","19146CL2","19146CL2","19146CL2","19146CL2","19146CL2","19146CL2","19146CL2"};
int i,j,num;
int num1=0, num2=0, num3=0, num4=0, num5=0, num6=0, num7=0, num8=0, num9=0, num10=0, num11=0;
void kiemtra()
{
  if(id[i]==the1[i])
  {
    num1++;
    num=num1;
    j=0;
    printf(" \n ");
    printf("%s",hoten[0]);
    printf("%s",mssv[0]);
    printf("%s",lop[0]);
  }
  if(id[i]==the2[i])
  {
    num2++;
    num=num2;
    j=1;
    printf(" \n ");
    printf("%s",hoten[1]);
    printf("%s",mssv[1]);
    printf("%s",lop[1]);
  }
  if(id[i]==the3[i])
  {
    num3++;
    num=num3;
    j=2;
    printf(" \n ");
    printf("%s ",hoten[2]);
    printf("%s ",mssv[2]);
    printf("%s",lop[2]);
  }
  if(id[i]==the4[i])
  {
    num4++;
    num=num4;
    j=3;
    printf(" \n ");
    printf("%s",hoten[3]);
    printf("%s",mssv[3]);
    printf("%s",lop[3]);
  }
  if(id[i]==the5[i])
  {
    num5++;
    num=num5;
    j=4;
    printf(" \n ");
    printf("%s",hoten[4]);
    printf("%s",mssv[4]);
    printf("%s",lop[4]);
  }
  if(id[i]==the6[i])
  {
    num6++;
    num=num6;
    j=5;
    printf(" \n ");
    printf("%s",hoten[5]);
    printf("%s",mssv[5]);
    printf("%s",lop[5]);
  }
  if(id[i]==the7[i])
  {
    num7++;
    num=num7;
    j=6;
    printf(" \n ");
    printf("%s",hoten[6]);
    printf("%s",mssv[6]);
    printf("%s",lop[6]);
  }
  if(id[i]==the8[i])
  {
    num8++;
    num=num8;
    j=7;
    printf(" \n ");
    printf("%s",hoten[7]);
    printf("%s",mssv[7]);
    printf("%s",lop[7]);
  }
  if(id[i]==the9[i])
  {
    num9++;
    num=num9;
    j=8;
    printf(" \n ");
    printf("%s",hoten[8]);
    printf("%s",mssv[8]);
    printf("%s",lop[8]);
  }
  if(id[i]==the10[i])
  {
    num10++;
    num=num10;
    j=9;
    printf(" \n ");
    printf("%s",hoten[9]);
    printf("%s",mssv[9]);
    printf("%s",lop[9]);
  }
  if(id[i]==the11[i])
  {
    num11++;
    num=num11;
    j=10;
    printf(" \n ");
    printf("%s",hoten[10]);
    printf("%s",mssv[10]);
    printf("%s",lop[10]);
  }
  
  if(num<2) 
  {
    //if(lop[j]=="19146CL2")
    //{
      //Connect to database
      conn = mysql_init(NULL);
      mysql_real_connect(conn,server,user,password,database,0,NULL,0);
      // Create sql command
      char cmd[200];
      sprintf(cmd,"update diemdanh set timein = CURRENT_TIMESTAMP, ngay = CURRENT_TIMESTAMP where hoten = ('%s') ", hoten[j]);
      //sprintf(cmd,"update read_rc522b set (hoten,mssv) = ('%s','%s')", hoten[j], mssv[j]);
      // send SQL query
      mysql_query(conn, cmd);
      mysql_close(conn);
    //}
  }
  else
  {
    //Connect to database
    conn = mysql_init(NULL);
    mysql_real_connect(conn,server,user,password,database,0,NULL,0);
    // Create sql command
    char cmd1[200];
    //sprintf(cmd1,"insert into read_rc522d(hoten,mssv) values ('%s','%s')", hoten[j], mssv[j]);
    sprintf(cmd1,"update diemdanh set timeout = CURRENT_TIMESTAMP where hoten = ('%s') ", hoten[j]);
    // send SQL query
    mysql_query(conn, cmd1);
    mysql_close(conn);
  }
}
  
int main()
{
  unsigned char s;
  wiringPiSPISetup(channel, 8000000);
  MFRC522_init();
  wiringPiSetup();
  pinMode(buzz, OUTPUT);
  pinMode(ledxanh, OUTPUT);
  pinMode(leddo, OUTPUT);
  
  while(1)
  {
    digitalWrite(leddo, HIGH);
    if (RFID_findCard(0x52,&s)==MI_OK)
    {
       if ( RFID_anticoll(id)==MI_OK)
       {
         for(i=0;i<5;i++)  
         {
            printf("%d ",id[i]);
            if(i==4)
            {  
               kiemtra();
               		digitalWrite(buzz, HIGH);
                  digitalWrite(ledxanh, HIGH);
                  digitalWrite(leddo, LOW);
                  delay(30);
                  digitalWrite(buzz, LOW);
                  digitalWrite(ledxanh, LOW);
                  delay(100);
                  digitalWrite(leddo, HIGH);
            }
            
         }
            printf("\n");
       } 
       else printf("ERR\n");
    
    }
    sleep(1);
    
  }
  return 0;
}