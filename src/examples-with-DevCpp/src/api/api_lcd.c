/*
*********************************************************************************************************
*                                           LCD
*                                      DEVICE IMPLEMENTATION
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#include "../common/44b.h"
#include "../common/44blib.h"
#include "../common/def.h"
#include "../common/option.h"

#include "api_lcd.h"

/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/

#define M5D(n) ((n) & 0x1fffff)

#define ARRAY_SIZE_MONO 	(SCR_XSIZE/8*SCR_YSIZE)
#define ARRAY_SIZE_G4   	(SCR_XSIZE/4*SCR_YSIZE)
#define ARRAY_SIZE_G16  	(SCR_XSIZE/2*SCR_YSIZE)
#define ARRAY_SIZE_COLOR 	(SCR_XSIZE/1*SCR_YSIZE)

#define HOZVAL			(LCD_XSIZE/4-1)
#define HOZVAL_COLOR	(LCD_XSIZE*3/8-1)
#define LINEVAL			(LCD_YSIZE-1)
#define MVAL			(13)

#define CLKVAL_MONO 	(6)
#define CLKVAL_G4 		(6)
#define CLKVAL_G16 		(6)
#define CLKVAL_COLOR 	(6)
#define MVAL_USED 0

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


//SCR_XSIZE,SCR_YSIZE,LCD_XSIZE,LCD_YSIZE are defined in lcd.h


extern void ARMEnableInt(void); 
extern void ARMDisableInt(void); 

void LcdInit(int color);
void (*PutPixel)(U32,U32,U8);

unsigned int (*frameBuffer1)[SCR_XSIZE/32];
unsigned int (*frameBuffer4)[SCR_XSIZE/16];
unsigned int (*frameBuffer16)[SCR_XSIZE/8];
unsigned int (*frameBuffer256)[SCR_XSIZE/4];

unsigned char Ascii[]=
{
/*0x00*/  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
/*0x10*/  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
/*0x20*/  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/* */
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x3c,0x3c,0x3c,0x18,0x18,/*!*/
0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
0x00,0x66,0x66,0x66,0x24,0x00,0x00,0x00,/*"*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x6c,0x6c,0xfe,0x6c,0x6c,
0x6c,0xfe,0x6c,0x6c,0x00,0x00,0x00,0x00,
0x18,0x18,0x7c,0xc6,0xc2,0xc0,0x7c,0x06,
0x86,0xc6,0x7c,0x18,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xc2,0xc6,0x0c,0x18,
0x30,0x60,0xc6,0x86,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x6c,0x6c,0x38,0x76,0xdc,
0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x30,0x30,0x30,0x60,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x0c,0x18,0x30,0x30,0x30,0x30,
0x30,0x30,0x18,0x0c,0x00,0x00,0x00,0x00,
0x00,0x00,0x30,0x18,0x0c,0x0c,0x0c,0x0c,
0x0c,0x0c,0x18,0x30,0x00,0x00,0x00,0x00,

0x00,0x00,0x00,0x00,0x00,0x66,0x3c,0xff,
0x3c,0x66,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x7e,
0x18,0x18,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x18,0x18,0x18,0x30,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x02,0x06,0x0c,0x18,
0x30,0x60,0xc0,0x80,0x00,0x00,0x00,0x00,

0x00,0x00,0x7c,0xc6,0xc6,0xce,0xd6,0xd6,
0xe6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x38,0x78,0x18,0x18,0x18,/*1*/
0x18,0x18,0x18,0x7e,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0x06,0x0c,0x18,0x30,
0x60,0xc0,0xc6,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0x06,0x06,0x3c,0x06,
0x06,0x06,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x0c,0x1c,0x3c,0x6c,0xcc,0xfe,
0x0c,0x0c,0x0c,0x1e,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0xc0,0xc0,0xc0,0xfc,0x0e,
0x06,0x06,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x60,0xc0,0xc0,0xfc,0xc6,
0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0xc6,0x06,0x06,0x0c,0x18,
0x30,0x30,0x30,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0xc6,0x7c,0xc6,
0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0xc6,0x7e,0x06,
0x06,0x06,0x0c,0x78,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,
0x00,0x18,0x18,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x18,0x18,0x00,0x00,
0x00,0x18,0x18,0x30,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x06,0x0c,0x18,0x30,0x60,
0x30,0x18,0x0c,0x06,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,
0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x60,0x30,0x18,0x0c,0x06,
0x0c,0x18,0x30,0x60,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0x0c,0x18,0x18,
0x18,0x00,0x18,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7c,0xc6,0xc6,0xde,0xde,
0xde,0xdc,0xc0,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x10,0x38,0x6c,0xc6,0xc6,0xfe,
0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x66,
0x66,0x66,0x66,0xfc,0x00,0x00,0x00,0x00,
0x00,0x00,0x3c,0x66,0xc2,0xc0,0xc0,0xc0,
0xc0,0xc2,0x66,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0xf8,0x6c,0x66,0x66,0x66,0x66,
0x66,0x66,0x6c,0xf8,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0x66,0x62,0x68,0x78,0x68,
0x60,0x62,0x66,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0x66,0x62,0x68,0x78,0x68,
0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,
0x00,0x00,0x3c,0x66,0xc2,0xc0,0xc0,0xde,
0xc6,0xc6,0x66,0x3a,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xfe,0xc6,
0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x3c,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0x1e,0x0c,0x0c,0x0c,0x0c,0x0c,
0xcc,0xcc,0xcc,0x78,0x00,0x00,0x00,0x00,
0x00,0x00,0xe6,0x66,0x6c,0x6c,0x78,0x78,
0x6c,0x66,0x66,0xe6,0x00,0x00,0x00,0x00,
0x00,0x00,0xf0,0x60,0x60,0x60,0x60,0x60,
0x60,0x62,0x66,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xee,0xfe,0xfe,0xd6,0xc6,
0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xe6,0xf6,0xfe,0xde,0xce,
0xc6,0xc6,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x6c,0xc6,0xc6,0xc6,0xc6,
0xc6,0xc6,0x6c,0x38,0x00,0x00,0x00,0x00,
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x60,
0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0xc6,0xc6,0xc6,
0xc6,0xd6,0xde,0x7c,0x0c,0x0e,0x00,0x00,
0x00,0x00,0xfc,0x66,0x66,0x66,0x7c,0x6c,
0x66,0x66,0x66,0xe6,0x00,0x00,0x00,0x00,
0x00,0x00,0x7c,0xc6,0xc6,0x60,0x38,0x0c,
0x06,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x7e,0x7e,0x5a,0x18,0x18,0x18,
0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,
0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xc6,
0xc6,0x6c,0x38,0x10,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xc6,0xc6,0xc6,0xc6,0xd6,
0xd6,0xfe,0x6c,0x6c,0x00,0x00,0x00,0x00,
0x00,0x00,0xc6,0xc6,0x6c,0x6c,0x38,0x38,
0x6c,0x6c,0xc6,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x66,0x66,0x66,0x66,0x3c,0x18,
0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0xfe,0xc6,0x86,0x0c,0x18,0x30,
0x60,0xc2,0xc6,0xfe,0x00,0x00,0x00,0x00,
0x00,0x00,0x3c,0x30,0x30,0x30,0x30,0x30,
0x30,0x30,0x30,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x80,0xc0,0xe0,0x70,0x38,
0x1c,0x0e,0x06,0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x3c,0x0c,0x0c,0x0c,0x0c,0x0c,
0x0c,0x0c,0x0c,0x3c,0x00,0x00,0x00,0x00,
0x10,0x38,0x6c,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xff,0x00,0x00,
0x30,0x30,0x18,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x78,0x0c,0x7c,
0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x00,0xe0,0x60,0x60,0x78,0x6c,0x66,
0x66,0x66,0x66,0xdc,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc0,
0xc0,0xc0,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x1c,0x0c,0x0c,0x3c,0x6c,0xcc,
0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xfe,
0xc0,0xc0,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x6c,0x64,0x60,0xf0,0x60,
0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x76,0xcc,0xcc,
0xcc,0xcc,0xcc,0x7c,0x0c,0xcc,0x78,0x00,
0x00,0x00,0xe0,0x60,0x60,0x6c,0x76,0x66,
0x66,0x66,0x66,0xe6,0x00,0x00,0x00,0x00,
0x00,0x00,0x18,0x18,0x00,0x38,0x18,0x18,
0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0x06,0x06,0x00,0x0e,0x06,0x06,
0x06,0x06,0x06,0x06,0x66,0x66,0x3c,0x00,
0x00,0x00,0xe0,0x60,0x60,0x66,0x6c,0x78,
0x78,0x6c,0x66,0xe6,0x00,0x00,0x00,0x00,
0x00,0x00,0x38,0x18,0x18,0x18,0x18,0x18,
0x18,0x18,0x18,0x3c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xec,0xfe,0xd6,
0xd6,0xd6,0xd6,0xd6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xdc,0x66,0x66,
0x66,0x66,0x66,0x66,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0xc6,
0xc6,0xc6,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xdc,0x66,0x66,
0x66,0x66,0x66,0x7c,0x60,0x60,0xf0,0x00,
0x00,0x00,0x00,0x00,0x00,0x76,0xcc,0xcc,
0xcc,0xcc,0xcc,0x7c,0x0c,0x0c,0x1e,0x00,
0x00,0x00,0x00,0x00,0x00,0xdc,0x76,0x62,
0x60,0x60,0x60,0xf0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7c,0xc6,0x60,
0x38,0x0c,0xc6,0x7c,0x00,0x00,0x00,0x00,
0x00,0x00,0x10,0x30,0x30,0xfc,0x30,0x30,
0x30,0x30,0x36,0x1c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xcc,0xcc,0xcc,
0xcc,0xcc,0xcc,0x76,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x66,0x66,0x66,
0x66,0x66,0x3c,0x18,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,0xc6,
0xd6,0xd6,0xfe,0x6c,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xc6,0x6c,0x38,
0x38,0x38,0x6c,0xc6,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xc6,0xc6,0xc6,
0xc6,0xc6,0xc6,0x7e,0x06,0x0c,0xf8,0x00,
0x00,0x00,0x00,0x00,0x00,0xfe,0xcc,0x18,
0x30,0x60,0xc6,0xfe,0x00,0x00,0x00,0x00,
0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,
0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
0x55,0xaa,0x55,0xaa,0x55,0xaa,0x55,0xaa,
0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,
0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe,0xfe
}; 

U16	Cursor=0;

void Lcd_InitCtrl(void)
{
    frameBuffer1=0;
    frameBuffer4=0;
    frameBuffer16=0;
    frameBuffer256=0;
    Cursor=0;
}

void Lcd_InitMode(int depth)
{
    ARMDisableInt();
    //320x240 1bit/1pixel LCD
    rPDATC = ( rPDATC & (~(1<<8)) );

    switch(depth)
    {
    case 1:    
	if((U32)frameBuffer1==0)
	{
	    //The total frame memory should be inside 4MB.
	    //For example, if total memory is 8MB, the frame memory 
	    //should be in 0xc000000~0xc3fffff or c400000~c7fffff.
	    //But, the following code doesn't meet this condition(4MB) 
	    //if the code size & location is changed..
	    frameBuffer1=(unsigned int (*)[SCR_XSIZE/32])malloc(ARRAY_SIZE_MONO); 
	}

	rLCDCON1=(0)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_MONO<<12);
	    // disable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21);  
	    //LINEBLANK=10 (without any calculation) 
	rLCDSADDR1= (0x0<<27) | ( ((U32)frameBuffer1>>22)<<21 ) | M5D((U32)frameBuffer1>>1);
	    // monochrome, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D( (((U32)frameBuffer1+(SCR_XSIZE*LCD_YSIZE/8))>>1) ) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/16) | ( ((SCR_XSIZE-LCD_XSIZE)/16)<<9 );

	rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_MONO<<12);
	    // enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	break;

    case 4:
	if((U32)frameBuffer4==0)
	{
	    //The total frame memory should be inside 4MB.
	    //For example, if total memory is 8MB, the frame memory 
	    //should be in 0xc000000~0xc3fffff or c400000~c7fffff.
	    //But, the following code doesn't meet this condition(4MB) 
	    //if the code size & location is changed..
	    frameBuffer4=(unsigned int (*)[SCR_XSIZE/16])malloc(ARRAY_SIZE_G4); 
	}

	rLCDCON1=(0)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_G4<<12);
	    // disable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21);  
	    //LINEBLANK=10 (without any calculation) 
	rLCDSADDR1= (0x1<<27) | ( ((U32)frameBuffer4>>22)<<21 ) | M5D((U32)frameBuffer4>>1);
	    // 4-gray, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D((((U32)frameBuffer4+(SCR_XSIZE*LCD_YSIZE/4))>>1)) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/8) | ( ((SCR_XSIZE-LCD_XSIZE)/8)<<9 );

	//The following value has to be changed for better display.
	//Select 4 levels among 16 gray levels.

	rBLUELUT=0xfa40; 
	rDITHMODE=0x0;
	rDP1_2 =0xa5a5;      
	rDP4_7 =0xba5da65;
	rDP3_5 =0xa5a5f;
	rDP2_3 =0xd6b;
	rDP5_7 =0xeb7b5ed;
	rDP3_4 =0x7dbe;
	rDP4_5 =0x7ebdf;
	rDP6_7 =0x7fdfbfe;

	rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_G4<<12);
	    // enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	break;

    case 16:
	if((U32)frameBuffer16==0)
	{
	    //The total frame memory should be inside 4MB.
	    //For example, if total memory is 8MB, the frame memory 
	    //should be in 0xc000000~0xc3fffff or c400000~c7fffff.
	    //But, the following code doesn't meet this condition(4MB) 
	    //if the code size & location is changed..
	    frameBuffer16=(unsigned int (*)[SCR_XSIZE/8])malloc(ARRAY_SIZE_G16); 
	}

	//The following value has to be changed for better display.
/*	rDITHMODE=0x1223a; //philips
	rDP1_2 =0x5a5a;      
	rDP4_7 =0x366cd9b;
	rDP3_5 =0xda5a7;
	rDP2_3 =0xad7;
	rDP5_7 =0xfeda5b7;
	rDP3_4 =0xebd7;
	rDP4_5 =0xebfd7;
	rDP6_7 =0x7efdfbf;*/

        //rDITHMODE=0x12210; 
/*  rDITHMODE=0x0; 	
    rDP1_2 =0xa5a5;      
	rDP4_7 =0xba5da65;
	rDP3_5 =0xa5a5f;
	rDP2_3 =0xd6b;
	rDP5_7 =0xeb7b5ed;
	rDP3_4 =0x7dbe;
	rDP4_5 =0x7ebdf;
	rDP6_7 =0x7fdfbfe;*/


        rDITHMODE=0x12210; 	
        //rDITHMODE=0x0;
    rDP1_2 =0xa5a5;      
	rDP4_7 =0xba5da65;
	rDP3_5 =0xa5a5f;
	rDP2_3 =0xd6b;
	rDP5_7 =0xeb7b5ed;
	rDP3_4 =0x7dbe;
	rDP4_5 =0x7ebdf;
	rDP6_7 =0x7fdfbfe;



	rLCDCON1=(0)|(1<<1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_G16<<12);
	    // disable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON2=(LINEVAL)|(HOZVAL<<10)|(10<<21);  
	    //LINEBLANK=10 (without any calculation) 
	rLCDSADDR1= (0x2<<27) | ( ((U32)frameBuffer16>>22)<<21 ) | M5D((U32)frameBuffer16>>1);
	    // 16-gray, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D((((U32)frameBuffer16+(SCR_XSIZE*LCD_YSIZE/2))>>1)) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/4) | ( ((SCR_XSIZE-LCD_XSIZE)/4)<<9 );
	rLCDCON1=(1)|(1<<1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_G16<<12);
	//FS LCD set
	
	//rLCDCON1=(1)|(1<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_G16<<12);
	//my set

	    // enable,4B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	break;

    case 256:
	if((U32)frameBuffer256==0)
	{
	    //The total frame memory should be inside 4MB.
	    //For example, if total memory is 8MB, the frame memory 
	    //should be in 0xc000000~0xc3fffff or c400000~c7fffff.
	    //But, the following code doesn't meet this condition(4MB) 
	    //if the code size & location is changed..
	    frameBuffer256=(unsigned int (*)[SCR_XSIZE/4])malloc(ARRAY_SIZE_COLOR); 
	}

	rLCDCON1=(0)|(2<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_COLOR<<12);
	    // disable,8B_SNGL_SCAN,WDLY=8clk,WLH=8clk,
	rLCDCON2=(LINEVAL)|(HOZVAL_COLOR<<10)|(10<<21);  
	    //LINEBLANK=10 (without any calculation) 
	rLCDSADDR1= (0x3<<27) | ( ((U32)frameBuffer256>>22)<<21 ) | M5D((U32)frameBuffer256>>1);
	    // 256-color, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D((((U32)frameBuffer256+(SCR_XSIZE*LCD_YSIZE))>>1)) | (MVAL<<21);
	rLCDSADDR3= (LCD_XSIZE/2) | ( ((SCR_XSIZE-LCD_XSIZE)/2)<<9 );

	//The following value has to be changed for better display.

	rREDLUT  =0xfdb96420;
	rGREENLUT=0xfdb96420;
	rBLUELUT =0xfb40;

	rDITHMODE=0x0;
	rDP1_2 =0xa5a5;      
	rDP4_7 =0xba5da65;
	rDP3_5 =0xa5a5f;
	rDP2_3 =0xd6b;
	rDP5_7 =0xeb7b5ed;
	rDP3_4 =0x7dbe;
	rDP4_5 =0x7ebdf;
	rDP6_7 =0x7fdfbfe;

	rLCDCON1=(1)|(2<<5)|(MVAL_USED<<7)|(0x3<<8)|(0x3<<10)|(CLKVAL_COLOR<<12);
	    // enable,8B_SNGL_SCAN,WDLY=8clk,WLH=8clk,

	break;

    default:
	break;
    }	
 	ARMEnableInt();
}



void Lcd_MoveViewPort(int vx,int vy,int depth)
{
    U32 addr;
    
    ARMDisableInt();
    switch(depth)
    {
    case 1:
    	// LCDBASEU,LCDBASEL register has to be changed before 12 words before the end of VLINE.
    	// In mono mode, x=320 is 10 words, So, We can't change LCDBASEU,LCDBASEL 
    	// during LINECNT=1~0 at mono mode. 

	//The processor mode should be superviser mode.  
    	ARMDisableInt(); 

    #if (LCD_XSIZE<512)
    	while((rLCDCON1>>22)<=1); // if x<512
    #else	
    	while((rLCDCON1>>22)==0); // if x>512 ((12+4)*32) 
    #endif

        addr=(U32)frameBuffer1+(vx/8)+vy*(SCR_XSIZE/8);
	rLCDSADDR1= (0x0<<27) | ( (addr>>22)<<21 ) | M5D(addr>>1);
	    // monochrome, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D( ((addr+(SCR_XSIZE*LCD_YSIZE/8))>>1) ) | (MVAL<<21);

	ARMEnableInt();
       	break;

    case 4:
	//The processor mode should be superviser mode.  
    	ARMDisableInt(); 

    #if (LCD_XSIZE<256)
    	while((rLCDCON1>>22)<=1); // if x<256
    #else	
    	while((rLCDCON1>>22)==0); // if x>256
    #endif
        addr=(U32)frameBuffer4+(vx/4)+vy*(SCR_XSIZE/4);
	rLCDSADDR1= (0x1<<27) | ( (addr>>22)<<21 ) | M5D(addr>>1);
	    // 4-gray, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE*LCD_YSIZE/4))>>1)) | (MVAL<<21);

	ARMEnableInt();
    	break;

    case 16:
	//The processor mode should be superviser mode.  
    	ARMDisableInt(); 

    #if (LCD_XSIZE<128)
    	while((rLCDCON1>>22)<=1); // if x<128
    #else	
    	while((rLCDCON1>>22)==0); // if x>128
    #endif
        addr=(U32)frameBuffer16+(vx/2)+vy*(SCR_XSIZE/2);
	rLCDSADDR1= (0x2<<27) | ( (addr>>22)<<21 ) | M5D(addr>>1);
	    // 16-gray, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE*LCD_YSIZE/2))>>1)) | (MVAL<<21);

	ARMEnableInt();
    	break;

    case 256:
	//The processor mode should be superviser mode.  
    	ARMDisableInt(); 

    #if (LCD_XSIZE<64)
    	while((rLCDCON1>>22)<=1); // if x<64
    #else	
    	while((rLCDCON1>>22)==0); // if x>64
    #endif
        addr=(U32)frameBuffer256+(vx/1)+vy*(SCR_XSIZE/1);
	rLCDSADDR1= (0x3<<27) | ( (addr>>22)<<21 ) | M5D(addr>>1);
	    // 256-color, LCDBANK, LCDBASEU
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE*LCD_YSIZE))>>1)) | (MVAL<<21);

	ARMEnableInt();
    	break;
    }
 	ARMEnableInt();
}    

void Lcd_DispON(void)
{
#ifdef BOARD_FS44BOX
   rPDATE=rPDATE&~(1<<5)|(1<<5);	//GPE5=H	 
   rPCONE=rPCONE&~(3<<10)|(1<<10);	//GPE5=output
   rPCONC=rPCONC&~(0xff<<8)|(0xff<<8);	//GPC[4:7] => VD[7:4]
	Delay(5);
   rPCONE = rPCONE & (~(0xf<<6)) | (0x5<<6) ;
   rPDATE = ( rPDATE | (3<<3) );
   Delay(5);
#else
   Delay(5000);
   rPDATC = ( rPDATC & (~(1<<8)) );
#ifdef BOARD_ST44BOX
   rPDATC =  ( rPDATC | (1<<9) );
#else
   rPDATC =  ( rPDATC & ~(1<<9) );
#endif
   Delay(5000);
#endif
}

void Lcd_DispOFF(void)
{
#ifdef BOARD_FS44BOX
   rPDATE=rPDATE&~(1<<5)|(1<<5);	//GPE5=H	 
   rPCONE=rPCONE&~(3<<10)|(1<<10);	//GPE5=output
   rPCONC=rPCONC&~(0xff<<8)|(0xff<<8);	//GPC[4:7] => VD[7:4]
#else
   Delay(5000);
   rPDATC = ( rPDATC | (1<<8) );
#ifdef BOARD_ST44BOX
   rPDATC =  ( rPDATC & ~(1<<9) );
#else
   rPDATC =  ( rPDATC | (1<<9) );
#endif
   Delay(5000);
#endif
}

void Lcd_PowerReset(void)	
{
    U8 i;
    
    ARMDisableInt();
    rPDATC = ( rPDATC | 3<<4 );		//crtl=1,adj=1
    for(i=0;i<1;i++);
    rPDATC = ( rPDATC & (~(1<<5)) );	//ctrl=0
    for(i=0;i<2;i++);
    rPDATC = ( rPDATC | 1<<5 );		//ctrl=1
    for(i=0;i<1;i++);
    rPDATC = ( rPDATC & (~(1<<4)) );	//adj=0
	ARMEnableInt();
}

void Lcd_PowerUp(void)
{
    U8 i;
    
    ARMDisableInt();
    rPDATC = ( rPDATC | 2<<4 );		//ctrl=1,adj=0
    for(i=0;i<2;i++);
    rPDATC = ( rPDATC | 1<<4 );		//adj=1
    for(i=0;i<1;i++);
    rPDATC = ( rPDATC & (~(1<<4)) );	//adj=0
    for(i=0;i<2;i++);
	ARMEnableInt();
}

void LcdG16_Bmp( unsigned char bmp[] )
{
    int x, y, m ;
    
    ARMDisableInt();
    Lcd_InitMode(MODE_G16);
    Lcd_DispON();
    Glib_Init(MODE_G16);

    m = 0;
    for( y = 0; y < 320; y++ )
    {
    	for( x = 0; x < 240; x+=2 )
    	{
			PutPixel( (x+1), y, (bmp[m]&0x0f) );    	    
			PutPixel( (x+0), y, (bmp[m]>>4) );    	    
    	    m++ ;
    	}
    }    
	ARMEnableInt();
}


void LcdG16_Bmp_Overturn( unsigned char bmp[] )
{
    int x, y, m ;
    
    ARMDisableInt();
    Lcd_InitMode(MODE_G16);
    Lcd_DispON();
    Glib_Init(MODE_G16);

    m = 0;
    for( y = 319; y >= 0; y-- )	
    {
    	for( x = 238; x >= 0; x-=2 )
    	{
			PutPixel( (x+0), y, (bmp[m]&0x0f) );    	    
			PutPixel( (x+1), y, (bmp[m]>>4) );    	    
    	    m++ ;
    	}
    }    
	ARMEnableInt();
}

U8 High_Low( U8 x )
{
	x = ( (x>>1)&0x1c ) | (x>>6) | (x<<5);
	return x ;
}

void LcdColor256_Bmp( unsigned char bmp[] )
{
    int i,j,k;

    ARMDisableInt();
    rPDATE=rPDATE&~(1<<5)|(1<<5);	//GPE5=H	 
    rPCONE=rPCONE&~(3<<10)|(1<<10);	//GPE5=output
    rPCONC=rPCONC&~(0xff<<8)|(0xff<<8);	//GPC[4:7] => VD[7:4]

    Lcd_InitMode(MODE_COLOR);
    Glib_Init(MODE_COLOR);

	k = 0 ;
    for(i=0;i<240;i++)
    {
    	for(j=0;j<320;j++)
    	{
    	    PutPixel( j, i, High_Low( bmp[k] ) );
    	    k++;
    	}
    }
 	ARMEnableInt();
}

void LcdColor256_Bmp_Overturn( unsigned char bmp[] )
{
    int i,j,k;

    ARMDisableInt();
    rPDATE=rPDATE&~(1<<5)|(1<<5);	//GPE5=H	 
    rPCONE=rPCONE&~(3<<10)|(1<<10);	//GPE5=output
    rPCONC=rPCONC&~(0xff<<8)|(0xff<<8);	//GPC[4:7] => VD[7:4]

    Lcd_InitMode(MODE_COLOR);
    Glib_Init(MODE_COLOR);

	k = 0 ;
    for(i=239;i>=0;i--)
    {
    	for(j=319;j>=0;j--)
    	{
    	    PutPixel( j, i, High_Low( bmp[k] ) );
    	    k++;
    	}
    }
	ARMEnableInt();
}

void Glib_Init(int depth)
{
    switch(depth)
    {
    case 1:
    	PutPixel=_PutPixelMono;
    	break;
    case 4:
       	PutPixel=_PutPixelG4;
       	break;
    case 16:
        PutPixel=_PutPixelG16;
        break;
    case 256:
    	PutPixel=_PutPixelColor;
    	break;   
    default: break;
    }
}


void _PutPixelMono(U32 x,U32 y,U8 c)
{
    ARMDisableInt();
    if(x<SCR_XSIZE && y<SCR_YSIZE)
	frameBuffer1[(y)][(x)/32]=( frameBuffer1[(y)][(x)/32] & ~(0x80000000>>((x)%32)*1) )
            | ( (c)<< ((32-1-((x)%32))*1) );
	ARMEnableInt();
}


void _PutPixelG4(U32 x,U32 y,U8 c)
{
    ARMDisableInt();
    if(x<SCR_XSIZE && y<SCR_YSIZE)
        frameBuffer4[(y)][(x)/16]=( frameBuffer4[(y)][x/16] & ~(0xc0000000>>((x)%16)*2) )
            | ( (c)<<((16-1-((x)%16))*2) );
	ARMEnableInt();
}


void _PutPixelG16(U32 x,U32 y,U8 c)
{
    ARMDisableInt();
    if(x<SCR_XSIZE && y<SCR_YSIZE)
        frameBuffer16[(y)][(x)/8]=( frameBuffer16[(y)][x/8] & ~(0xf0000000>>((x)%8)*4) )
            | ( (c)<<((8-1-((x)%8))*4) );
	ARMEnableInt();
}


void _PutPixelColor(U32 x,U32 y,U8 c)
{
    ARMDisableInt();
    if(x<SCR_XSIZE && y<SCR_YSIZE)
        frameBuffer256[(y)][(x)/4]=( frameBuffer256[(y)][x/4] & ~(0xff000000>>((x)%4)*8) )
            | ( (c)<<((4-1-((x)%4))*8) );
	ARMEnableInt();
}


void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    ARMDisableInt();
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
	ARMEnableInt();
}


void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;
    
    ARMDisableInt();
    for(i=y1;i<=y2;i++)
       Glib_Line(x1,i,x2,i,color);
	ARMEnableInt();
}


// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//	 3I2
//	4 I 1
//      --+--   <-8 octants  mathematical cordinate
//      5 I 8
//	 6I7
void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	
    ARMDisableInt();    
    dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	ARMEnableInt();
}

	
void Glib_ClearScr(U8 c)
{	
    //Very inefficient function.
    
    int i,j;
    
    ARMDisableInt();	
    for(j=0;j<SCR_YSIZE;j++)
    	for(i=0;i<SCR_XSIZE;i++)
	    	PutPixel(i,j,c);
	ARMEnableInt();
}


void Slib_Init(void)
{
	Cursor=0;
}

void Slib_SetCursor(U8 y,U8 x)
{
	Cursor=y*40+x;	
}


void Slib_PutChar(U8 y,U8 x,char *pchar)
{
	U8 temp_char;
	U8 i;
	
    ARMDisableInt();	
	temp_char=(unsigned char)*pchar;
//	temp_char=0x30;
	
	for (i=0;i<16;i++)
	{
		frameBuffer1[y*16+i][(unsigned char)(x/4)] &= ~((U32)(0xff << (8*(3-(x%4)))));
		frameBuffer1[y*16+i][(unsigned char)(x/4)] |= (Ascii[temp_char*16+i]) << (8*(3-(x%4)));
	}
	ARMEnableInt();
}

void Slib_PutStr(char *pstr)
{
	U8 xx,yy;

    ARMDisableInt();
  	while(*pstr)
  	{
  	if(*pstr=='\n')
  	{
  		Cursor=((U8)(Cursor/40)+1)*40;
  		pstr++;
  	}
  	xx=(U8)(Cursor%40);
  	yy=(U8)(Cursor/40);
  	
    Slib_PutChar(yy,xx,pstr);
    pstr++;
    Cursor++;
    if(Cursor == 600)
    	Cursor=0;	
  	}
	ARMEnableInt();
}


void Slib_Printf(char *fmt,...) 
{
  va_list ap;
  char string[256];

  ARMDisableInt();
  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  Slib_PutStr(string);
  va_end(ap);
  //Reflash_LCD();
  ARMEnableInt();
}


void Slib_ClearScr(void)
{
    int i,j;

    ARMDisableInt();
    for(j=0;j<240;j++)
        for(i=0;i<80;i++)
        {
            
			frameBuffer1[j][i]=0x00000000;

        }
	ARMEnableInt();
}



void Slib_PutChar_Color(U8 y,U8 x,char *pchar,U8 C)
{
	U8 temp_char;
	U8 i,j;

   ARMDisableInt();
	
	temp_char=(unsigned char)*pchar;
	
	for (i=0;i<16;i++)
	{
		for(j=0;j<8;j++)
		{
      	if((Ascii[temp_char*16+i]&(0x80>>j))!=0)
      	   {
            PutPixel((8*x+j),(16*y+i),C);
            }
      	else
            {
            PutPixel((8*x+j),(16*y+i),COLOR_WHITE);
            }
		}
	}
	ARMEnableInt();
}


void Slib_PutStr_Color(char *pstr,U8 color)
{
	U8 xx,yy;
	//U8 str;

   ARMDisableInt();

  	while(*pstr)
  	{
	  	//str=(unsigned char)*pstr;
	  	if(*pstr=='\n')
	  	{
	  		Cursor=((U8)(Cursor/40)+1)*40;
	  		pstr++;
	  	}
	  	
	  	xx=(U8)(Cursor%40);
	  	yy=(U8)(Cursor/40);
	  	
	    Slib_PutChar_Color(yy,xx,/*str*/pstr,color);
	    pstr++;
	    Cursor++;
	    
	    if(Cursor == 600)
	    	Cursor=0;	
  	}
	ARMEnableInt();
}

void Slib_Printf_Color(U8 color,char *fmt,...) 
{
  va_list ap;
  char string[256];

  ARMDisableInt();

  va_start(ap,fmt);
  vsprintf(string,fmt,ap);
  Slib_PutStr_Color(string,color);
  va_end(ap);
  
  ARMEnableInt();
}


