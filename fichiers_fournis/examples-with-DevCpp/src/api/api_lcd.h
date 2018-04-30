/*
*********************************************************************************************************
*                                           LCD
*                                      DEVICE INTERFACE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/

#include "../common/option.h"

/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/


#define MLCD_240_320	(1)
#define MLCD_320_240	(2)
#define CLCD_240_320	(3)
#define CLCD_320_240	(4)

#ifdef LCD_COLOR
#define LCD_TYPE MLCD_320_240
#else
#define LCD_TYPE MLCD_240_320
#endif

#define MODE_MONO 	(1)
#define MODE_G4	  	(4)
#define MODE_G16  	(16)
#define MODE_COLOR 	(256)

#if (LCD_TYPE==MLCD_320_240)
	#define SCR_XSIZE 	(640)  
	#define SCR_YSIZE 	(480)
	
	#define LCD_XSIZE 	(320)
	#define LCD_YSIZE 	(240)
#elif (LCD_TYPE==MLCD_240_320)
	#define SCR_XSIZE 	(480)  
	#define SCR_YSIZE 	(640)
	
	#define LCD_XSIZE 	(240)
	#define LCD_YSIZE 	(320)
#elif (LCD_TYPE==CLCD_320_240)
	#define SCR_XSIZE 	(640)  
	#define SCR_YSIZE 	(480)
	
	#define LCD_XSIZE 	(320)
	#define LCD_YSIZE 	(240)
#elif (LCD_TYPE==CLCD_240_320)
	#define SCR_XSIZE 	(480)  
	#define SCR_YSIZE 	(640)
	
	#define LCD_XSIZE 	(240)
	#define LCD_YSIZE 	(320)
#endif

#ifdef LCD_COLOR
#define COLOR_BLACK      0
#define COLOR_WHITE      255
#else
#define COLOR_BLACK      0
#define COLOR_WHITE      15
#endif

/*
*********************************************************************************************************
*                                        TYPES DEFINITIONS
*********************************************************************************************************
*/

extern unsigned int (*frameBuffer1)[SCR_XSIZE/32];
extern unsigned int (*frameBuffer4)[SCR_XSIZE/16];
extern unsigned int (*frameBuffer16)[SCR_XSIZE/8];
extern unsigned int (*frameBuffer256)[SCR_XSIZE/4];

extern void (*PutPixel)(U32,U32,U8);

/*
*********************************************************************************************************
*                                        FUNCTIONS PROTOTYPES
*********************************************************************************************************
*/

void LcdG16_Bmp( unsigned char bmp[] ) ;
void LcdG16_Bmp_Overturn( unsigned char bmp[] ) ;
void Lcd_InitMode(int depth);
void Lcd_MoveViewPort(int vx,int vy,int depth);
void Lcd_DispON(void);
void Lcd_DispOFF(void);
void Lcd_Display(void);
void Lcd_PowerReset(void);
void Lcd_PowerUp(void);
void Lcd_InitCtrl(void);

void Glib_Init(int depth);
void Glib_Line(int x1,int y1,int x2,int y2,int color);
void Glib_Rectangle(int x1,int y1,int x2,int y2,int color);
void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
void Glib_ClearScr(U8 c);

void _PutPixelMono(U32 x,U32 y,U8 c);
void _PutPixelG4(U32 x,U32 y,U8 c);
void _PutPixelG16(U32 x,U32 y,U8 c);
void _PutPixelColor(U32 x,U32 y,U8 c);

void Slib_Init(void);
void Slib_SetCursor(U8 y,U8 x);
void Slib_PutChar(U8 y,U8 x,char *pchar);
void Slib_PutStr(char *pstr);
void Slib_Printf(char *fmt,...);
void Slib_PutChar_Color(U8 y,U8 x,char *pchar,U8 color);
void Slib_PutStr_Color(char *pstr,U8 color);
void Slib_Printf_Color(U8 color,char *fmt,...);
void Slib_ClearScr(void);
