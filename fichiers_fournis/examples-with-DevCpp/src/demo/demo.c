/*
 * File:
 *
 * uC/OS Real-time multitasking kernel for the ARM processor.
 *
 * Demo for S3C44BOX capabilities
 *
 */

//#include    <stdio.h>
#include    "../common/44b.h"
#include    "../common/uhal.h"
#include    "api_led.h"
#include    "api_keyboard.h"
#include    "api_serial.h"
#include    "api_flash.h"
#include    "api_iis.h"
#include    "api_lcd.h"
#include    "api_timers.h"
#include    "api_rtc.h"



/* S3C44BOX DIALOG COMMANDES */
#define CMD_CONNECT         1
#define CMD_UNCONNECT       2
#define CMD_TOGGLE_LED_1    3
#define CMD_TOGGLE_LED_2    4
#define CMD_TOGGLE_LED_3    5
#define CMD_STATE_BUTTON    6
#define CMD_WR_RAM          7
#define CMD_RD_RAM          8
#define CMD_WR_FLASH        9
#define CMD_RD_FLASH       10
#define CMD_WR_EEPROM      11
#define CMD_RD_EEPROM      12
#define CMD_ERASE_FLASH    13
#define CMD_START_TIMER_2  14
#define CMD_START_TIMER_3  15
#define CMD_START_TIMER_4  16
#define CMD_STOP_TIMER_2   17
#define CMD_STOP_TIMER_3   18
#define CMD_STOP_TIMER_4   19
#define CMD_MODIFY_TIME_2  20
#define CMD_MODIFY_TIME_3  21
#define CMD_MODIFY_TIME_4  22
#define CMD_MODIFY_DUTY_2  23
#define CMD_MODIFY_DUTY_3  24
#define CMD_MODIFY_DUTY_4  25
#define CMD_LCD_CLRSCR     26
#define CMD_LCD_MOVEVP     27
#define CMD_LCD_ADDOBJ     28
#define CMD_LCD_INFOS      29
#define CMD_RTC_GET_INFOS  30
#define CMD_RTC_SET_INFOS  31
#define CMD_MEM_DOWNLOAD   32
#define CMD_MEM_UPLOAD     33
#define CMD_WAVE_LOAD      34
#define CMD_WAVE_PLAY      35

#define LCD_POINT          1
#define LCD_LINE           2
#define LCD_RECT           3
#define LCD_FILLRECT       4
#define LCD_TEXT           5
#define LCD_BMP            6

#ifdef LCD_COLOR
#define XT0   12
#define YT0   3
#else
#define XT0   6
#define YT0   6
#endif

#define FACTOR			2.44140625    

char st0[100];
char st1[100];
int adr,ipadr;
unsigned short data;
unsigned short x1,y1,x2,y2,color;
unsigned short mode_drawing;
unsigned char year,mon,day,date,hour,min,sec;
int KbStatus;
int WaveFileLen;
unsigned int delay_val = 3000 ;
int value ;
int voltage_val = 2441 ;
int adccon_value ;
int i_temp ;


void proc_timer0(void) __attribute__((interrupt("IRQ")));


void Aff_Init(void)
{
 Lcd_InitCtrl();
#ifdef LCD_COLOR
   Lcd_InitMode(MODE_COLOR);
#else
   Lcd_InitMode(MODE_G16);
#endif
   Lcd_DispON();
               
#ifdef LCD_COLOR
   Glib_Init(MODE_COLOR);
#else
   Glib_Init(MODE_G16);
#endif
}


void Aff_Infos(void)
{

   Glib_ClearScr(COLOR_WHITE);
   Slib_SetCursor(YT0,XT0);
   Slib_PutStr_Color(" FS4BOX DEV KIT \n           ** STATION METEO **\n \n            ESIL 1 ere ANNEE",COLOR_BLACK);
   Slib_SetCursor(YT0+6,XT0-3);
   Slib_PutStr_Color("BOUTON S2: LECTURE ADC",COLOR_BLACK);
      Slib_SetCursor(YT0+7,XT0-3);
   Slib_PutStr_Color("BOUTON S4: ACQUISITION ",COLOR_BLACK);
#ifdef LCD_COLOR
   Glib_Rectangle( 60, 20, 260, 220, 1 );
   Glib_Rectangle( 55, 15, 265, 225, 1 );
#else
   Glib_Rectangle( 20, 60, 220, 260, 1 );
   Glib_Rectangle( 15, 55, 225, 265, 1 );
#endif
   mode_drawing = FALSE;
}


void Init_timer0(void)
{
int status ;
    status = TIMER_SET(ID_TIMER0,MODE_UPDATE,100,1250,2500,0); 
    rINTCON=0x0; // Non-vectored,IRQ disable,FIQ disable   	  	
    rINTMOD=0x0; // All=IRQ mode	
    rINTMSK=BIT_GLOBAL;	 // All interrupt is masked.   
   	pISR_TIMER0=(int)proc_timer0;
    rINTCON=0x5;	//Non-vectored,IRQ enable,FIQ disable 
    rINTMSK =~ BIT_GLOBAL;
}


void proc_timer0(void)
{
    Aff_ADC_Values() ;
     if((KEYBOARD_STATUS() & 0x8)==8) 
        {
        TIMER_CHANGE_MODE(ID_TIMER0,MODE_STOP) ;
        sprintf(st0,"\nS5 Stop timer detected\n") ;
        uHALr_printf(st0) ;       
        }
     
    LED_TOGGLE(0);
    rI_ISPC=BIT_TIMER0;
}


int ReadADCChannel(int WChannel)
{  
/*  ADC CHANNELS
    WCHANNEL_0, AIN1 => +VDD3.3V/2
    WCHANNEL_1, AIN2 => +VDD5V/2
    WCHANNEL_2, AIN3 => Pressure sensor MPX4100 Vout/2
    WCHANNEL_3, AIN4 => Humidity sensor HIH 3605 Vout/2
*/	
int i ;
 
	rADCPSR = 20 ;					// PRESCALER register
	rADCCON = 0x0 | (WChannel<<2) ; 		// Setup Channel
    	for (i=0;i<150;i++) ;               		// min 15us
	rADCCON = 0x1 | (WChannel<<2) ; 		// Start the ADC converter
	while(rADCCON & 0x01)   ;			// ENABLE_START; 0 = No operation, 1 = A/D conversion start
	while(!(rADCCON & 0x40))  ;			// FLAG; 0 = A/D conversion in process, 1 = End of AD conversion
	for (i=0;i<rADCPSR;i++) ;			// Waiting for 20 clock cycles	
    //sprintf(st0,"Channel = %d\nrADCPSR = %x\nrADCCON = %x\nrADCDAT =%x\n",WChannel,rADCPSR,rADCCON,rADCDAT) ;
    //uHALr_printf(st0) ;
return rADCDAT ;
}

int Aff_ADC_Values(void)
{
#define FACTOR			2.44140625
int value ;
int voltage_val ;
int Int_Part ;
int Dec_Part ;
int i ;   
    
    //sprintf(st1,"ADC Channel %d  %.3f \n",i,voltage_val) ;
    //uHALr_printf(st1) ;
    //Delay(delay_val) ;
    rCLKCON = 0x7ff8 ;
    rADCCON = 0x0 | (0<<2) ; 	// Enable ADC
    Delay(100) ;
 
    for(i=0;i<4;i++)
      {
      Slib_SetCursor(YT0+(i),XT0-3); 
      value = ReadADCChannel(i) ;
      voltage_val = (value*FACTOR) ;
      Int_Part = voltage_val /1000 ;
      Dec_Part = voltage_val - (Int_Part*1000)  ;
      sprintf(st1,"ADC Channel %d  %d.%d",i,Int_Part,Dec_Part) ;
      //sprintf(st1,"ADC Channel %d  ",i) ;
      Slib_PutStr_Color(st1,COLOR_BLACK);
      }

return 0 ;
}
  
int Aff_ADC_Menu(void)  
{
    Glib_ClearScr(COLOR_WHITE);
    Slib_SetCursor(YT0+6,XT0-4);
    Slib_PutStr_Color("BOUTON S2: RELECTURE ADC",COLOR_BLACK);
    Slib_SetCursor(YT0+7,XT0-4);
    Slib_PutStr_Color("BOUTON S3: RETOUR AU MENU",COLOR_BLACK);
#ifdef LCD_COLOR
   Glib_Rectangle( 60, 20, 260, 220, 1 );
   Glib_Rectangle( 55, 15, 265, 225, 1 );
#else
   Glib_Rectangle( 20, 60, 220, 260, 1 );
   Glib_Rectangle( 15, 55, 225, 265, 1 );
#endif
   mode_drawing = FALSE; 
}
 
/*
 * Main function.
 */
int Main(int argc, char **argv)
{
    
    char com_test;
    int status ;

    rSYSCFG = CACHECFG;
    rNCACHBE0 = ((Non_Cache_End>>12)<<16)|(Non_Cache_Start>>12);
    Delay(0); /* Adjust delay */
    
    /* do target (uHAL based ARM system) initialisation */

	rSYSCFG=CACHECFG;				// Using 8KB Cache
	Port_Init();

	Uart_Init(0, 115200) ;
	Uart_Select(0);	 				// Select UART0
    
    sprintf(st0,"\n My DEMO Running\n") ;
    uHALr_printf(st0) ;
    
    Init_timer0() ;
    
    Aff_Init();
    Aff_Infos();
           
    LED_DISPLAY(0x0);

    while(1)
    {          
    KbStatus = KEYBOARD_STATUS();
    if((KbStatus & 0x1)==1) 
    	{
                   
    	uHALr_printf("Button Ox1, S2 pressed\n") ;
    	 while( KEYBOARD_STATUS() == KbStatus) ;
    	    	Delay(delay_val) ;
    	LED_TOGGLE(0);
	   	Aff_ADC_Menu() ;
	   	Aff_ADC_Values() ;
       	}	        
    	
    if((KbStatus & 0x2)==2) 
    	{
    	uHALr_printf("Button 0x2, S3 pressed\n") ;
    	while( KEYBOARD_STATUS() == KbStatus) ;
    	    	Delay(delay_val) ;
   		LED_TOGGLE(1);
	    Aff_Infos();
    	}	        
    	
    if((KbStatus & 0x4)==4) 
    	{
    	uHALr_printf("Button 0x4, S4 pressed\n") ;
    
    	while( KEYBOARD_STATUS() == KbStatus) ;
    	    	Delay(delay_val) ;
  	    Aff_ADC_Menu() ;
       	TIMER_CHANGE_MODE(ID_TIMER0,MODE_RELOAD+MODE_RUN) ;
   	    rINTMSK &= ~BIT_TIMER0;
  		LED_TOGGLE(2);
   	  
    	}	        
    	
    if((KbStatus & 0x8)==8) 
    	{
    	uHALr_printf("Button 0x8, S5 pressed\n") ;
    	LED_TOGGLE(3);
    	while( KEYBOARD_STATUS() == KbStatus) ;
    		Delay(delay_val) ;
  		TIMER_CHANGE_MODE(ID_TIMER0,MODE_STOP) ;
    	}
  
    }

	return 0;
}
