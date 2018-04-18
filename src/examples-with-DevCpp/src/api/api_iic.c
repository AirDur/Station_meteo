/*
*********************************************************************************************************
*                                           IIC
*                                      DEVICE IMPLEMENTATION
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/

#include <string.h>
#include "../common/44b.h"
#include "../common/def.h"
#include "api_iic.h"

/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/

#define WRDATA	    1
#define POLLACK    2
#define RDDATA	    3
#define SETRDADDR  4

#define IICBUFSIZE 0x20

/*
*********************************************************************************************************
*                                          GLOBAL VARIABLES
*********************************************************************************************************
*/

U8 _iicData[IICBUFSIZE];
volatile int _iicDataCount;
volatile int _iicStatus;
volatile int _iicMode;
int _iicPt;

/*
*********************************************************************************************************
*                                        FUNCTIONS IMPLEMENTATION
*********************************************************************************************************
*/


void IicInt(void) __attribute__((interrupt("IRQ")));

void Init_Iic(void)
{
    rPCONF |=0xa;	//PF0:IICSCL, PF1:IICSDA
    rPUPF |=0x3;	//pull-up disable

    pISR_IIC=(unsigned)IicInt;
    rINTMSK &= ~(BIT_GLOBAL|BIT_IIC);

    rIICCON=(1<<7)|(0<<6)|(1<<5)|(0xf);
    //Enable interrupt, IICCLK=MCLK/16, Enable ACK
    //40Mhz/16/(15+1) = 257Khz
    rIICADD=0x10;   // S3C44B0X slave address

    rIICSTAT=0x10;
}


void Wr24LCxx(U32 slvAddr,U32 addr,U8 data)
{
    _iicMode=WRDATA;
    _iicPt=0;
    _iicData[0]=(U8)addr;
    _iicData[1]=data;
    _iicDataCount=2;

    rIICDS=slvAddr;//0xa0
    rIICSTAT=0xf0; //MasTx,Start
    //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1);

    _iicMode=POLLACK;

    while(1)
    {
	rIICDS=slvAddr;
	_iicStatus=0x100;
	rIICSTAT=0xf0; //MasTx,Start
	rIICCON=0xaf;  //resumes IIC operation.
	while(_iicStatus==0x100);
	if(!(_iicStatus&0x1))
	    break; // when ACK is received
    }
    rIICSTAT=0xd0;  //stop MasTx condition
    rIICCON=0xaf;   //resumes IIC operation.
    Delay(1);	    //wait until stop condtion is in effect.

    //write is completed.
}


void Rd24LCxx(U32 slvAddr,U32 addr,U8 *data)
{
    _iicMode=SETRDADDR;
    _iicPt=0;
    _iicData[0]=(U8)addr;
    _iicDataCount=1;

    rIICDS=slvAddr;
    rIICSTAT=0xf0; //MasTx,Start
    //Clearing the pending bit isn't needed because the pending bit has been cleared.
    while(_iicDataCount!=-1);

    _iicMode=RDDATA;

    _iicPt=0;
    _iicDataCount=1;

    rIICDS=slvAddr;
    rIICSTAT=0xb0; //MasRx,Start
    rIICCON=0xaf;  //resumes IIC operation.
    while(_iicDataCount!=-1);

    *data=_iicData[1];
}



void IicInt(void)
{
    U32 iicSt,i;
    rI_ISPC=BIT_IIC;

    iicSt=rIICSTAT;
    if(iicSt&0x8){} // when bus arbitration is failed.
    if(iicSt&0x4){} // when a slave address is matched with IICADD
    if(iicSt&0x2){} // when a slave address is 0000000b
    if(iicSt&0x1){} // when ACK isn't received

    switch(_iicMode)
    {
	case POLLACK:
	    _iicStatus=iicSt;
	    break;

	case RDDATA:
	    if((_iicDataCount--)==0)
	    {
		_iicData[_iicPt++]=rIICDS;

		rIICSTAT=0x90;  //stop MasRx condition
		rIICCON=0xaf;   //resumes IIC operation.
		Delay(1);	//wait until stop condtion is in effect.
				//too long time...
		//The pending bit will not be set after issuing stop condition.
		break;
	    }
	    _iicData[_iicPt++]=rIICDS;
				//The last data has to be read with no ack.
	    if((_iicDataCount)==0)
		rIICCON=0x2f;	//resumes IIC operation with NOACK.
	    else
		rIICCON=0xaf;	//resumes IIC operation with ACK
	    break;

	case WRDATA:
	    if((_iicDataCount--)==0)
	    {
		rIICSTAT=0xd0;	//stop MasTx condition
		rIICCON=0xaf;	//resumes IIC operation.
		Delay(1);	//wait until stop condtion is in effect.
		//The pending bit will not be set after issuing stop condition.
		break;
	    }
	    rIICDS=_iicData[_iicPt++];  //_iicData[0] has dummy.
	    for(i=0;i<10;i++);	    //for setup time until rising edge of IICSCL
	    rIICCON=0xaf;	    //resumes IIC operation.
	    break;

	case SETRDADDR:
	    //Uart_Printf("[S%d]",_iicDataCount);
	    if((_iicDataCount--)==0)
	    {
		break;  //IIC operation is stopped because of IICCON[4]
	    }
	    rIICDS=_iicData[_iicPt++];
	    for(i=0;i<10;i++);  //for setup time until rising edge of IICSCL
	    rIICCON=0xaf;	    //resumes IIC operation.
	    break;

	default:
	    break;
    }
}
