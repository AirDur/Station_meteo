/*
*********************************************************************************************************
*                                           IIS
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
#include "api_iis.h"

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

void Play_Wave(U32 addr, U32 size)
   {
	int i;
   unsigned char *pWave;
   unsigned short *pSteroWave;
   U32 samplesize,start;
   U32 save_PLLCON;
   LPWAVEFORMATEX pWav;
 
   if(size > MAX_WAVESIZE)
    	return;
		
	pWav = (LPWAVEFORMATEX)(addr+0x14);
	if(pWav->wFormatTag!=WAVE_FORMAT_PCM)
		return;
	
	if(pWav->nChannels!=2)
		return;
		  
	//	ChangePllValue(0x69,0x17,0x0);	//MCLK=45.1584MHz <-- 5.6448MHz*8
	save_PLLCON = rPLLCON;
	rPLLCON = (0x69<<12)|(0x17<<4)|0;

	pWave=(unsigned char *)addr;
	pWave+=0x28;	
   samplesize=*(pWave+0) | *(pWave+1)<<8 | *(pWave+2)<<16 | *(pWave+3)<<24;
   pWave+=4;
   start=(U32)(unsigned char *)pWave;	
   pSteroWave=(U16 *)start;
   samplesize=(samplesize>>1)<<1;

   /****** IIS Initialize ******/
   rIISCON=0x02;	//Tx DMA disable,Rx idle,prescaler enable
   rIISMOD=0x89;	//Master,Tx,L-ch=low,iis,16bit ch.,codeclk=256fs,lrck=32fs
   rIISPSR=0x33;	//Prescaler_A/B enable, value=3
   rIISFCON=0x200;	//Tx/Rx normal,Tx FIFO enable--> start piling....

    /****** IIS Tx Start ******/
   rIISCON |=0x1;
    while(((U32)pSteroWave-start) < samplesize)
    {
    	if(IIS_FIFOREADY)
    	{
    		for(i=0;i<8;i++)
    			*IISFIF = *pSteroWave++;    			
    	}
   }
   /****** IIS Tx Stop ******/
   rIISCON=0x0;
  	rPLLCON = save_PLLCON;
  	return;
   }


