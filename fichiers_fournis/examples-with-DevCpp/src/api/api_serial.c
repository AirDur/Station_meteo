/*
*********************************************************************************************************
*                                      SERIAL COMMUNICATION
*                                      DEVICE IMPLEMENTATION
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/

#include "../common/44b.h"
#include "../common/44blib.h"
#include "../common/def.h"
#include "../common/option.h"
#include "api_serial.h"

/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

static int UART_Current;
static unsigned char Uart0RxBuf[SERIAL_BUF_LEN];
static unsigned char Uart1RxBuf[SERIAL_BUF_LEN];
static unsigned char Uart0TxBuf[SERIAL_BUF_LEN];
static unsigned char Uart1TxBuf[SERIAL_BUF_LEN];
volatile static int Uart0RxBufWrPt=0;
volatile static int Uart0TxBufRdPt=0;
volatile static int Uart0RxBufRdPt=0;
volatile static int Uart0TxBufWrPt=0;
volatile static int Uart1RxBufWrPt=0;
volatile static int Uart1TxBufRdPt=0;
volatile static int Uart1RxBufRdPt=0;
volatile static int Uart1TxBufWrPt=0;

/*
*********************************************************************************************************
*                                        FUNCTIONS IMPLEMENTATION
*********************************************************************************************************
*/

void Uart0_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart0_TxInt(void) __attribute__ ((interrupt ("IRQ")));

void Uart1_RxInt(void) __attribute__ ((interrupt ("IRQ")));
void Uart1_TxInt(void) __attribute__ ((interrupt ("IRQ")));

void Uart0_RxInt(void)
{
    Uart0RxBuf[Uart0RxBufWrPt++]=RdURXH0();
    if(Uart0RxBufWrPt == SERIAL_BUF_LEN)
		 Uart0RxBufWrPt = 0;
    rI_ISPC=BIT_URXD0;
}

void Uart0_TxInt(void)
{
   if( SERIAL_LEN_TX_FIFO() > 0)
      {
	   SERIAL_TX_CHAR(Uart0TxBuf[Uart0TxBufRdPt++]);
      if(Uart0TxBufRdPt == SERIAL_BUF_LEN)
	   	Uart0TxBufRdPt = 0;
      }
	rI_ISPC=BIT_UTXD0;
}

void Uart1_RxInt(void)
{
    Uart1RxBuf[Uart1RxBufWrPt++]=RdURXH1();
    if(Uart1RxBufWrPt == SERIAL_BUF_LEN)
		 Uart1RxBufWrPt = 0;
    rI_ISPC=BIT_URXD1;
}

void Uart1_TxInt(void)
{
   if( SERIAL_LEN_TX_FIFO() > 0)
      {
	   SERIAL_TX_CHAR(Uart1TxBuf[Uart1TxBufRdPt++]);
      if(Uart1TxBufRdPt == SERIAL_BUF_LEN)
		Uart1TxBufRdPt = 0;
      }
   rI_ISPC=BIT_UTXD1;
}

void SERIAL_SET_CONF(int UART,int COM_MODE,int SPEED,int BUF_MODE)
   {      
   if(UART==0)
      {
      rUFCON0=0x0;     //FIFO disable
      rUMCON0=0x0;
      rULCON0=COM_MODE;
      rUBRDIV0=( (int)(MCLK/16./SPEED + 0.5) -1 );
      UART_Current = 0;
      
      if(BUF_MODE == SERIAL_NO_BUFFER)
         {
         rUCON0=0x0245;
         }
         else
         {
         Uart0RxBufWrPt=0;
         Uart0TxBufWrPt=0;
         Uart0RxBufRdPt=0;
         Uart0TxBufRdPt=0;
         rUCON0=0x45;
         pISR_UTXD0=(unsigned)Uart0_TxInt;
         pISR_URXD0=(unsigned)Uart0_RxInt;
         rINTMSK &= ~(BIT_URXD0|BIT_UTXD0);
         }      
      }
   if(UART==1)
      {
      rUFCON1=0x0;
      rUMCON1=0x0;
      rULCON1=COM_MODE;
      rUBRDIV1=( (int)(MCLK/16./SPEED + 0.5) -1 );
      UART_Current = 1;
      if(BUF_MODE == SERIAL_NO_BUFFER)
         {
         rUCON1=0x0245;
         }
         else
         {
         Uart1RxBufWrPt=0;
         Uart1TxBufWrPt=0;
         Uart1RxBufRdPt=0;
         Uart1TxBufRdPt=0;
         rUCON1=0x45;
         pISR_UTXD1=(unsigned)Uart1_TxInt;
         pISR_URXD1=(unsigned)Uart1_RxInt;
         rINTMSK &= ~(BIT_URXD1|BIT_UTXD1);
         }
      }
   return;
   }

void SERIAL_SELECT_UART(int UART)
   {
   UART_Current = UART;
   return;
   }

void SERIAL_TX_CHAR(char CAR)
   {
   if(UART_Current == 0 )
      {
      while(!(rUTRSTAT0 & 0x2)); //Wait until THR is empty.
      WrUTXH0(CAR);
      }
      else
      {
      while(!(rUTRSTAT1 & 0x2)); //Wait until THR is empty.
      WrUTXH1(CAR);
      }
   return;
   }

char SERIAL_RX_WAIT_CHAR()
   {
   if(UART_Current==0)
      {	    
      while(!(rUTRSTAT0 & 0x1)); //Receive data read
	   return RdURXH0();
      }
      else
      {
	   while(!(rUTRSTAT1 & 0x1)); //Receive data read
		return rURXH1;
      }
   }

char SERIAL_RX_NOWAIT_CHAR()
   {
   if(UART_Current==0)
      {	    
	     if(rUTRSTAT0 & 0x1)    //Receive data ready
	        {
    	     return RdURXH0();
           }
		 else
	        {
		     return 0;
           }
      }
      else
      {
		if(rUTRSTAT1 & 0x1)    //Receive data read
	        {
           return rURXH1;
           }
		else
	        {
           return 0;
           }
      }
   }

void SERIAL_TX_BUF(char * BUFFER,int LENGTH)
   {
   int i;
   for(i=0;i<LENGTH;i++)
      SERIAL_TX_CHAR(BUFFER[i]);
   return;
   }

void SERIAL_RX_BUF(char *BUFFER,int LENGTH)
   {
   int i;
   for(i=0;i<LENGTH;i++)
      BUFFER[i]=SERIAL_RX_WAIT_CHAR();
   return;
   }

int  SERIAL_GET_BUF_FIFO(char * BUF,int LEN,int WAIT)
   {
   int i,j;
   do
      {
      i = SERIAL_LEN_RX_FIFO();
      if( i < LEN)
         j = i;
         else
         j = LEN;
      }
   while((WAIT == SERIAL_WAIT)&&(j != LEN));
   
   if(j>0)
      {
      for(i=0;i<j;i++)
         {
         if(UART_Current==0)
            {
            BUF[i]=Uart0RxBuf[Uart0RxBufRdPt++];
            if(Uart0RxBufRdPt == SERIAL_BUF_LEN)
               Uart0RxBufRdPt = 0;
            }
            else
            {
            BUF[i]=Uart1RxBuf[Uart1RxBufRdPt++];
            if(Uart1RxBufRdPt == SERIAL_BUF_LEN)
               Uart1RxBufRdPt = 0;
            }         
         }
      }
   return j;
   }

int SERIAL_PUT_BUF_FIFO(char * BUF,int LEN,int WAIT)
   {
   int i,j;
   do
      {
      i = SERIAL_LEN_TX_FIFO();
      if((SERIAL_BUF_LEN - i)< LEN)
         j = SERIAL_BUF_LEN - i;
         else
         j = LEN;
      }
   while((WAIT == SERIAL_WAIT)&&(j != LEN));

   if(j>0)
      {     
      for(i=0;i<j;i++)
         {
         if(UART_Current==0)
            {
            Uart0TxBuf[Uart0TxBufWrPt++]=BUF[i];
            if(Uart0TxBufWrPt == SERIAL_BUF_LEN)
               Uart0TxBufWrPt = 0;
            }
            else
            {
            Uart1TxBuf[Uart1TxBufWrPt++]=BUF[i];
            if(Uart1TxBufWrPt == SERIAL_BUF_LEN)
               Uart1TxBufWrPt = 0;
            }         
         }
   
      if( SERIAL_LEN_TX_FIFO() > 0)
         {
         if(UART_Current==0)
            {
   	      SERIAL_TX_CHAR(Uart0TxBuf[Uart0TxBufRdPt++]);
            if(Uart0TxBufRdPt == SERIAL_BUF_LEN)
               Uart0TxBufRdPt = 0;
            }
         else
            {
   	      SERIAL_TX_CHAR(Uart1TxBuf[Uart1TxBufRdPt++]);
            if(Uart1TxBufRdPt == SERIAL_BUF_LEN)
               Uart1TxBufRdPt = 0;
            }
         }
      }
   return j;
   }

void SERIAL_FLUSH_RX_FIFO(void)
   {
   if(UART_Current==0)
      {
      Uart0RxBufWrPt=0;
      Uart0RxBufRdPt=0;
      }
      else
      {
      Uart1RxBufWrPt=0;
      Uart1RxBufRdPt=0;
      }
   }

void SERIAL_FLUSH_TX_FIFO(void)
   {
   if(UART_Current==1)
      {
      Uart0TxBufWrPt=0;
      Uart0TxBufRdPt=0;
      }
      else
      {
      Uart1TxBufWrPt=0;
      Uart1TxBufRdPt=0;
      }
   }

int SERIAL_LEN_RX_FIFO(void)
   {
   int i;
   
   if(UART_Current==0)
      {
      i = (Uart0RxBufWrPt - Uart0RxBufRdPt);
      if(i<0)
         i =  SERIAL_BUF_LEN - (Uart0RxBufRdPt - Uart0RxBufWrPt);
      }
      else
      {
      i = (Uart1RxBufWrPt - Uart1RxBufRdPt);
      if(i<0)
         i =  SERIAL_BUF_LEN - (Uart1RxBufRdPt - Uart1RxBufWrPt);
      }

   return i;
   }

int SERIAL_LEN_TX_FIFO(void)
   {
   int i;

   if(UART_Current==0)
      {
      i = Uart0TxBufWrPt - Uart0TxBufRdPt;
      if(i<0)
         i = SERIAL_BUF_LEN - (Uart0TxBufRdPt - Uart0TxBufWrPt);
      }
      else
      {
      i = Uart1TxBufWrPt - Uart1TxBufRdPt;
      if(i<0)
         i = SERIAL_BUF_LEN - (Uart1TxBufRdPt - Uart1TxBufWrPt);
      }

   return i;
   }

char SERIAL_GET_CHAR_FIFO(int WAIT)
   {
   char c=0;
   SERIAL_GET_BUF_FIFO(&c,1,WAIT);
   return c;
   }

void SERIAL_PUT_CHAR_FIFO(char CAR,int WAIT)
   {
   char c;
   c = CAR;
   SERIAL_PUT_BUF_FIFO(&c,1,WAIT);
   }

