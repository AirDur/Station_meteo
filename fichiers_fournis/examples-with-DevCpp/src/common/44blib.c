#include "44b.h"
#include "44blib.h"
#include "def.h"
#include "option.h"

#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define STACKSIZE    0xa00 //SVC satck size(do not use user stack)//
#define HEAPEND     (_ISR_STARTADDRESS-STACKSIZE-0x500) // = 0xc7ff000//
#define NBR_MAX_MEM_ALLOC 0x400
//SVC Stack Area:0xc(e)7ff000-0xc(e)7ffaff//

extern char Image_RW_Limit[];
extern char Image_RW_Base[];
volatile unsigned char *downPt;
unsigned int fileSize;

//void *mallocPt=Image_RW_Limit;
int LastAlloc;

typedef struct {
   int size;
   void *ptr;
   int free;
} Alloc;

Alloc TabAlloc[NBR_MAX_MEM_ALLOC];

void (*restart)(void)=(void (*)(void))0x0;
void (*run)(void)=(void (*)(void))DOWNLOAD_ADDRESS;

//--------------------------------SYSTEM---------------------------------//
static int delayLoopCount=400;

void Delay(int time)
// time=0: adjust the Delay function by WatchDog timer.//
// time>0: the number of loop time//
// 100us resolution.//
{
	int i,adjust=0;
	if(time==0)
	{
		time=200;
		adjust=1;
		delayLoopCount=400;
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3);	// 1M/64,Watch-dog,nRESET,interrupt disable//
		rWTDAT=0xffff;
		rWTCNT=0xffff;	 
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3)|(1<<5); // 1M/64,Watch-dog enable,nRESET,interrupt disable //
	}
	for(;time>0;time--)
		for(i=0;i<delayLoopCount;i++);
	if(adjust==1)
	{
		rWTCON=((MCLK/1000000-1)<<8)|(2<<3);
		i=0xffff-rWTCNT;   //  1count/16us?????????//
		delayLoopCount=8000000/(i*64);	//400*100/(i*64/200)   //
	}
}

//------------------------PORTS------------------------------//
void Port_Init(void)
{    
   int i;
    //CAUTION:Follow the configuration order for setting the ports. 
    // 1) setting value 
    // 2) setting control register 
    // 3) configure pull-up resistor.  

#ifdef BOARD_S3C44BOX	     
    //PORT A GROUP
    //GPA9 ADDR23 ADDR22 ADDR21 ADDR20 ADDR19 ADDR18 ADDR17 ADDR16 ADDR0		      
    //  0,    1,     1,    1,     1,     1,     1,      1,     1,    1
    rPCONA=0x1ff;	
    rPDATA=0x0;
    //PORT B GROUP
    //GPB10  GPB9 nGCS3 nGCS2 nGCS1 GPB5 GPB4 nSRAS nSCAS SCLK SCKE
    //  0,    0,    1,	1,    1,    0,    0,	1,    1,   1,   1	
    rPCONB=0x1Cf;
    
    //PORT C GROUP
    //IISLRCK  IISD0 IISDI IISCLK VD7 VD6 VD5 VD4 nXDACK1 nXDREQ1 GPC10 GPC11 TXD1 RXD1 GPC14 GPC15
    //All input
    //  11      11    11    11    11  11   11  11   11      11     01	01     11   11   01    01
    //rPDATC=0x8400;
    //rPCONC=0x5F5FFFFF;	
    //rPUPC=0x33ff;	//should be enabled	
    rPDATC=0xffff;	//All I/O Is High
    rPCONC=0x0f05ff55;	
    rPUPC=0x30f0;	//PULL UP RESISTOR should be enabled to I/O
     
    //PORT D GROUP
    //VFRAME VM VLINE VCLK VD3 VD2 VD1 VD0
    //    10,10,   10,	10, 10,	10, 10,	10  
    rPCOND=0xaaaa;	
    rPUPD=0xff;
	
	//PORT E GROUP
	//PE0:FOUT, PE1:TxD0, PE2:RxD0, GPE3, GPE4, GPE5,GPE6,GPE7, CODECLK   
	//    10       10,      10,    01  , 01,   01,  01,  01,   10
	rPCONE=0x2002B;//0x20002B;	
	rPCONE=0x2082B;
	rPUPE=0x00;	//0xff;	
	rPDATE=0xff;
	
	//PORT F GROUP
	//IICSCL IICSDA  nWAIT nXBACK0 nXDREQ0 GPF5 GPF6 GPF7 GPF8 
	//  10     10      10    10      10     0    0    0    0
	rPCONF=0x24916A;
	rPUPF=0xff;

    //PORT G GROUP
    //EINT0 EINT1 EINT2 EINT3 GPG4 GPG5 GPG6 GPG7
    //	  0x0
    //  11      11   11     11   01   01   01   01
    rPDATG=0x0;
    rPCONG=0x55FF;
    rPUPG=0xf;
    
    rSPUCR=0x7;  //pull-up disable
    rEXTINT=0x22222022;  //All EINT[7:0] will be falling edge triggered.
#endif

#ifdef BOARD_ST44BOX
	// PORT A GROUP
	/*  BIT 9	8	7	6	5	4	3	2	1	0	*/
	/*  O	  A23	A22	A21	A20	A19	A18	A17	A16	A0	*/	      
	/*  0		1	1	1	1	1	1	1	1	1	*/
	rPCONA = 0x1ff;	

	// PORT B GROUP
	/*  BIT 10	9		8		7		6		5		4		3		2		1		0		*/
	/*	/CS5	/CS4	/CS3	/CS2	/CS1	nWBE3	nWBE2	/SRAS	/SCAS	SCLK	SCKE	*/
	/*	NC		NC		8019	NC		NC		NC		NC		Sdram	Sdram	Sdram	Sdram	*/
	/*	0,		0,   	1,   	0,    	0,    	0,      0,		1,    	1,    	1,   	1       */
	rPDATB = 0x04f;
	rPCONB = 0x10f;    
    
	//PORT C GROUP
	//BUSWIDTH=16													*/
	/*  PC15	14		13		12		11		10		9		8	*/
	/*	o		o		RXD1	TXD1	o		o		o		o	*/
	/*	NC		NC		NC		NC		NC		NC		EL_ON	nDISP_ON	*/
	/*  00		00		00		00		00		00		01		01	*/

	/*  PC7		6		5		4		3		2		1		0	*/
	/*  o		o		o		o		o		o		o		o	*/
	/*  VD4		VD5		VD6		VD7		NC		NC		NC		NC	*/
	/*  11		11		11		11		00		00		00		00	*/
	rPDATC = 0x0000;	//All IO is low
	rPCONC = 0x0005ff00;	
	rPUPC  = 0x3000;	//PULL UP RESISTOR should be enabled to I/O

	//PORT D GROUP
	/*  PORT D GROUP(I/O OR LCD)										*/
	/*  BIT7		6		5		4		3		2		1		0	*/
	/*  VF			VM		VLINE	VCLK	VD3		VD2		VD1		VD0	*/
	/*	01			01		01		01		01		01		01		01	*/
	rPDATD= 0x55;
	rPCOND= 0xaaaa;	
	rPUPD = 0x00;

	//These pins must be set only after CPU's internal LCD controller is enable
	//PORT E GROUP 
	/*  Bit 8		7		6		5		4		3		2		1		0		*/
	/*	ENDLAN		NC		led2	led1	led0	Beep	RXD0	TXD0	NC		*/ 
	/*  00			00		01		01		01		01		10		10		00		*/	
	rPDATE	= 0x157;
	rPCONE	= 0x15A8;				
	rPUPE	= 0xff;
	
	//PORT F GROUP
	/*  Bit8		7		6		5		 4		3		2		1		0		*/   
	/*  NC			RXD1	TXD1	NC	 	NC		NC		NC		NC		NC		*/
	/*	000			010		010		000		 00		00		00		00		00		*/	
	rPDATF = 0x0;
	rPCONF = 0x024000;	 
	rPUPF  = 0x1d3;

	//PORT G GROUP
	/*  BIT7		6		5		4		3		2		1		0		*/
	/*	EINT7		EINT6	EINT5	EINT4	EINT3	EINT2	EINT1	EINT0	*/
	/*  KEY3		KEY2	KEY1	KEY0	NC		NC		ETH_INT	NC		*/
	/*	11 			11      11      11      00      00      11      00		*/
	rPDATG = 0xff;
	rPCONG = 0xff0c;	//enable extint0 extint1
	rPUPG  = 0x00;		//enabled pull up register
    
    rSPUCR=0x3;  //pull-up disable

    rEXTINT=0x22222222;  //All EINT[7:0] will be falling edge triggered.

#endif

#ifdef BOARD_FS44BOX
	// PORT A GROUP
	/*  BIT 9	8	7	6	5	4	3	2	1	0	*/
	/*  A24	  A23	A22	A21	A20	A19	A18	A17	A16	A0	*/	      
	/*  1		1	1	1	1	1	1	1	1	1	*/
	rPCONA = 0x3ff;	

	// PORT B GROUP
	/* BIT 10   9    8       7      6        5     4     3     2     1     0    */
	/*     /CS5 /CS4 /CS3    /CS2   /CS1     nWBE3 nWBE2 /SRAS /SCAS SCLS  SCKE	*/
	/*     NC   NC   RTL8019 USBD12 NV_Flash NC    NC    Sdram Sdram Sdram Sdram*/
	/*     1,   1,   1,      1,     1,       0,    0,    1,    1,    1,    1    */
	rPDATB = 0x7cf;
    
	//PORT C GROUP
	//BUSWIDTH=16													*/
	/*  PC15		14		13		12		11		10		9		8	*/
	/*	o		o		RXD1	TXD1	o		o		o		o	*/
	/*	NC		NC		Uart1	Uart1	NC		NC		NC		NC	*/
	/*   01		01		11		11		01		01		01		00	*/

	/*  PC7		6		5		4		3		2		1		0	*/
	/*   o		o		o		o		o		o		o		o	*/
	/*   NC		NC		NC		NC		NFALE 	NFCLE 	NFCE 	NFRB*/
	/*   01		01		01		01		01		01		01		00	*/
	rPDATC = 0x0100;	//All IO is low, PC8 = 1 LCD-Off, PC9 = 0 BackLight Off
	rPCONC = 0xfff5ff54;	//0->D12SUSPD, 1~3->LED, 4~7->VD7~VD4, 8->DISPON, 9->BACKLIGHT, 10~13->UART1, 14->RTS0, 15->CTS0
	rPUPC  = 0x0000;	//disable all pull-up

	//PORT D GROUP
	/*  PORT D GROUP(I/O OR LCD)										*/
	/*  BIT7		6		5		4		3		2		1		0	*/
	/*      VF		VM		VLINE	VCLK	VD3		VD2		VD1		VD0	*/
	/*	   10		10		10		10		10		10		10		10	*/
	rPCOND= 0xaaaa;	
	rPUPD = 0x00;
	//These pins must be set only after CPU's internal LCD controller is enable
	
	//PORT E GROUP 
	/*  Bit 8		7		6		5		4		3		2		1		0		*/
	/*		ENDLAN	LED3	LED2	LED1	LED0	BEEP	RXD0	TXD0	CLKOUT	*/ 
	/*      10		01		01		01		01		01		10		10		11		*/
	rPCONE	= 0x26568;	//0->input, 1 2->TXD0 RXD0, 3 4->input, 5->led, 6->buzzer, 7->led, 8->CODECLK
	rPDATE	= 0x60;
	rPUPE	= 0x000;	//disable all pull-up
	
	//PORT F GROUP
	/*  Bit8		7		6		5		 4		3		2		1		0		*/   
	/*  IISCLK	IISDI	IISDO	IISLRCK	Input	Input	Input	IICSDA	IICSCL	*/
	/*	100		100		100		100		00		01		01		10		10		*/
	rPCONF = 0x24900a;	//0 1->IICSCL IICSDA, 2~4->input 5~8->IIS PORT
	rPUPF  = 0x000;		//disable all pull-up

	//PORT G GROUP
	/*  BIT7		6		5		4		3		2		1		0	 */
	/*	INT7		INT6		INT5		INT4		INT3		INT2		INT1		INT0	*/
	/*    S3		S4		S5		S6		NIC		EXT		IDE		USB	*/
	/*      11      11      11      11      11      11      11      11       */
	rPCONG = 0xff3c;	//eint1 is eth interrupt in FS44B0, eint2 for ide
	rPUPG  = 0x00;		//disable all pull-up
	
//	rSPUCR = 0x7;  //D15-D0 pull-up disable
	rSPUCR = 0x6;  //D15-D0 pull-up enable

    rEXTINT=0x22222222;  //All EINT[7:0] will be falling edge triggered.
#endif

/* INIT ALLOC MEM */
for(i=0;i<NBR_MAX_MEM_ALLOC;i++)
   {
   TabAlloc[i].size = (HEAPEND - (int)Image_RW_Limit);
   TabAlloc[i].ptr = Image_RW_Limit;
   TabAlloc[i].free = TRUE;
   }
LastAlloc = -1;
}



/************************* UART ****************************/

void Uart_Init(int mclk,int baud)
{
    int i;
    if(mclk==0)
	mclk=MCLK;

    rUFCON0=0x0;     //FIFO disable
    rUFCON1=0x0;
    rUMCON0=0x0;
    rUMCON1=0x0;
//UART0
    rULCON0=0x3;     //Normal,No parity,1 stop,8 bit
    rUCON0=0x245;    //rx=edge,tx=level,disable timeout int.,enable rx error int.,normal,interrupt or polling
    rUBRDIV0=( (int)(mclk/16./baud + 0.5) -1 );
//UART1
    rULCON1=0x3;
    rUCON1=0x245;
    rUBRDIV1=( (int)(mclk/16./baud + 0.5) -1 );

    for(i=0;i<100;i++);
}

static int whichUart=0;

void Uart_Select(int ch)
{
    whichUart=ch;
}

void Uart_TxEmpty(int ch)
{
    if(ch==0)
		while(!(rUTRSTAT0 & 0x4)); //wait until tx shifter is empty.
    else
    	while(!(rUTRSTAT1 & 0x4)); //wait until tx shifter is empty.
}


char Uart_Getch(void)
{
    if(whichUart==0)
    {	    
		while(!(rUTRSTAT0 & 0x1)); //Receive data read
		return RdURXH0();
    }
    else
    {
		while(!(rUTRSTAT1 & 0x1)); //Receive data ready
		return	rURXH1;
    }
}


char Uart_GetKey(void)
{
    if(whichUart==0)
    {	    
		if(rUTRSTAT0 & 0x1)    //Receive data ready
    	    return RdURXH0();
		else
		    return 0;
    }
    else
    {
		if(rUTRSTAT1 & 0x1)    //Receive data ready
		    return rURXH1;
		else
		    return 0;
    }
}


void Uart_GetString(char *string)
{
    char *string2=string;
    char c;
    while((c=Uart_Getch())!='\r')
    {
		if(c=='\b')
		{
		    if(	(int)string2 < (int)string )
		    {
				Uart_Printf("\b \b");
				string--;
		    }
		}
		else 
		{
		    *string++=c;
		    Uart_SendByte(c);
		}
    }
    *string='\0';
    Uart_SendByte('\n');
}


int Uart_GetIntNum(void)
{
    char str[30];
    char *string=str;
    int base=10;
    int minus=0;
    int lastIndex;
    int result=0;
    int i;
    
    Uart_GetString(string);
    
    if(string[0]=='-')
    {
        minus=1;
        string++;
    }
    
    if(string[0]=='0' && (string[1]=='x' || string[1]=='X'))
    {
		base=16;
		string+=2;
    }
    
    lastIndex=strlen(string)-1;
    if( string[lastIndex]=='h' || string[lastIndex]=='H' )
    {
		base=16;
		string[lastIndex]=0;
		lastIndex--;
    }

    if(base==10)
    {
		result=atoi(string);
		result=minus ? (-1*result):result;
    }
    else
    {
		for(i=0;i<=lastIndex;i++)
		{
    	    if(isalpha(string[i]))
			{
				if(isupper(string[i]))
					result=(result<<4)+string[i]-'A'+10;
				else
				    result=(result<<4)+string[i]-'a'+10;
			}
			else
			{
				result=(result<<4)+string[i]-'0';
			}
		}
		result=minus ? (-1*result):result;
    }
    return result;
}

void Uart_SendByte(int data)
{
	if(whichUart==0)
    {
		if(data=='\n')
		{
		    while(!(rUTRSTAT0 & 0x2));
		    Delay(10);	//because the slow response of hyper_terminal 
		    WrUTXH0('\r');
		}
		while(!(rUTRSTAT0 & 0x2)); //Wait until THR is empty.
		Delay(10);
		WrUTXH0(data);
   	}
	else
    {
		if(data=='\n')
	{
		while(!(rUTRSTAT1 & 0x2));
		Delay(10);	//because the slow response of hyper_terminal 
		rUTXH1='\r';
	}
	while(!(rUTRSTAT1 & 0x2));  //Wait until THR is empty.
	Delay(10);
	rUTXH1=data;
    }	
}		

void Uart_SendString(char *pt)
{
    while(*pt)
	Uart_SendByte(*pt++);
}


//if you don't use vsprintf(), the code size is reduced very much.
void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_SendString(string);
    va_end(ap);
}

/******************** S3C44B0X EV. BOARD LED(Old Ver.) **********************/

void Led_Display(int LedStatus)
{
#ifdef BOARD_ST44BOX
    rPDATE=(rPDATE & 0x10f) | ((LedStatus & 0xf)<<4);
#else
    rPDATC=(rPDATC & 0x1f1) | ((LedStatus & 0xf) <<1);
#endif
}

/************************* Timer ********************************/

void Timer_Start(int divider)  //0:16us,1:32us 2:64us 3:128us
{
    rWTCON=((MCLK/1000000-1)<<8)|(divider<<3);
    rWTDAT=0xffff;
    rWTCNT=0xffff;   

    // 1/16/(65+1),nRESET & interrupt  disable
    rWTCON=((MCLK/1000000-1)<<8)|(divider<<3)|(1<<5);	
}


int Timer_Stop(void)
{
    rWTCON=((MCLK/1000000-1)<<8);
    return (0xffff-rWTCNT);
}

void Beep(int BeepStatus)
{
	if (BeepStatus==0)
		rPDATE=rPDATE|0x8;
	else
		rPDATE=rPDATE&0x1f7;

}
/************************* PLL ********************************/
void ChangePllValue(int mdiv,int pdiv,int sdiv)
{
    rPLLCON=(mdiv<<12)|(pdiv<<4)|sdiv;
}


/************************* General Library **********************/

void * malloc(unsigned nbyte) 
{
   int i,j,find;
   i=0;j=0;find=FALSE;
/*
   if(nbyte > 0x10000)
      uHALr_printf("MALLOC WARNING: %d byte requested !!! \n",nbyte);   
*/
   while((i<LastAlloc) && (i<NBR_MAX_MEM_ALLOC))
      {
      if((TabAlloc[i].size >= nbyte)&&(TabAlloc[i].free == TRUE))
         {
         find = TRUE;
         if(TabAlloc[i].size <= TabAlloc[j].size)
            j = i;
         }
      i++;
      }
      
   if(i >= NBR_MAX_MEM_ALLOC)
      uHALr_printf("MALLOC ERROR: NUMBER OF MEMORY ALLOCATION OVERFLOW !!! \n");   

   if(find == FALSE)
      i = ++LastAlloc;
      else
      i = j;
      
   TabAlloc[i].free = FALSE;
   TabAlloc[i].size = nbyte/4+((nbyte%4)>0); //to align 4byte
   if(LastAlloc > 0)
      TabAlloc[i].ptr = (int *)TabAlloc[i-1].ptr + TabAlloc[i-1].size;

   if(((int)TabAlloc[i].ptr + TabAlloc[i].size) >= HEAPEND)
      {
      uHALr_printf("MALLOC %d byte at %8x  LasAlloc=%d\n",TabAlloc[i].size,TabAlloc[i].ptr,LastAlloc);
      uHALr_printf("MALLOC OUT OF MEMORY !!! \n");
      LastAlloc --;
      return NULL;
      }

// uHALr_printf("MALLOC %d byte at %8x  LasAlloc=%d\n",TabAlloc[i].size,TabAlloc[i].ptr,LastAlloc);
   return TabAlloc[i].ptr;
}


void free(void *pt)
{
   int i;
   i=0;
   while(TabAlloc[i].ptr != pt) i++;
   TabAlloc[i].free = TRUE;
   if(i == LastAlloc)
      {
      /* Dernier bloc allouer suppimer : cas general */
      TabAlloc[i].size = (HEAPEND - (int)TabAlloc[i].ptr);
      LastAlloc--;   
      }
// uHALr_printf("FREE at %8x  LasAlloc=%d\n",TabAlloc[i].ptr,LastAlloc);
}

void Cache_Flush(void)
{
    int i,saveSyscfg;
    
    saveSyscfg=rSYSCFG;

    rSYSCFG=SYSCFG_0KB; 		      
    for(i=0x10004000;i<0x10004800;i+=16)    
    {					   
		*((int *)i)=0x0;		   
    }
    rSYSCFG=saveSyscfg; 			    
}
