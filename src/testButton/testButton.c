/**
 * @file     	button.c
 * @author   	Patrick BREUGNON
 * @brief    	Fontion permettant de tester les buttons poussoirs situés
 *		situés sur la carte
 */

#include "44b.h"
//#include "unistd.h"

unsigned int KEYBOARD_STATUS(void){

   rPCONG=0x0;	//IN7~0
   rPUPG=0x0;   //pull up enable
   return (~((rPDATG&0xf0)>>4)&0xF);
 }

int main()
{
unsigned int KbStatus;
long int i;


   while(1)
    {
    KbStatus = KEYBOARD_STATUS();
    if((KbStatus & 0x1)==1)
    	{

    	printf("Button Ox1, S2 pressed\n") ;
    	 while( KEYBOARD_STATUS() == KbStatus) ;
		//Same as Delay(delay_val) ;
		for(i=0; i<10000; i++)
			{i++;}
       	}

    if((KbStatus & 0x2)==2)
    	{
    	printf("Button 0x2, S3 pressed\n") ;
    	while( KEYBOARD_STATUS() == KbStatus) ;
    	}

    if((KbStatus & 0x4)==4)
    	{
    	printf("Button 0x4, S4 pressed\n") ;
    	while( KEYBOARD_STATUS() == KbStatus) ;
    	}

    if((KbStatus & 0x8)==8)
    	{
    	printf("Button 0x8, S5 pressed\n") ;
    	while( KEYBOARD_STATUS() == KbStatus) ;
    	}

    }
return 0;
}
