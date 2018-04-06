/*
*********************************************************************************************************
*                                           FLASH
*                                      DEVICE IMPLEMENTATION
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/

#include "../common/44b.h"
#include "../common/def.h"
#include "api_flash.h"

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


/*
*********************************************************************************************************
*                                        FUNCTIONS IMPLEMENTATION
*********************************************************************************************************
*/

int FLASH_CheckBlank(INT32U addr,INT32U Size) 
{
	INT32U i,temp;
	for (i=0;i<Size;i+=2)
	{
		temp=*(volatile INT16U *)(addr+i);
		if(temp!=0xffff)
			return 0;
	}
	return 1;	
}

int Waitfor_endofprg(void) //Check if the bit6 toggle ends.
{
	volatile INT16U flashStatus,old;
	old=*((volatile INT16U *)0x0);

	while(1)
	{
		flashStatus=*((volatile INT16U *)0x0);
		if( (old&0x40) == (flashStatus&0x40) )
			break;
		else
		old=flashStatus;
	}
	return 1;
}

int FLASH_WordWrite (INT32U addr,INT16U dat)
{
	Writeflash (0x5555,0xAA);
	Writeflash (0x2AAA,0x55);
	Writeflash (0x5555,0xA0);
	Writeflash (addr >> 1,dat);
	return(Waitfor_endofprg());
}

int FLASH_BufferWrite (INT32U addr,INT32U size,INT8U * buffer)
{
	INT32U i,temp;
	addr &= 0xFFFFFFFE; /* adress must be word aligned */
	size &= 0xFFFFFFFE; /* size must be word aligned */
	for (i=0;i<size;i+=2)
       FLASH_WordWrite (addr + i, *(INT16U *)(buffer + i));
	return 1;		
}

void  FLASH_SectorErase(INT32U SAaddr)
{
	Writeflash(0x5555,0xAA);
	Writeflash(0x2AAA,0x55);
	Writeflash(0x5555,0x80);
	Writeflash(0x5555,0xAA);
	Writeflash(0x2AAA,0x55);
	Writeflash(SAaddr >> 1,0x30);
   Waitfor_endofprg();
}
