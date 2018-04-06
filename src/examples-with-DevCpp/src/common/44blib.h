/***********************************************
 * NAME    : 44BLIB.H                          *
 * Version : 17.Apr.00                         *
 ***********************************************/
#define DebugOut Uart_Printf

#define min(x1,x2) ((x1<x2)? x1:x2)
#define max(x1,x2) ((x1>x2)? x1:x2)

#define ONESEC0 (62500)	//16us resolution, max 1.04 sec
#define ONESEC1 (31250)	//32us resolution, max 2.09 sec
#define ONESEC2 (15625)	//64us resolution, max 4.19 sec
#define ONESEC3 (7812)	//128us resolution, max 8.38 sec
#define ONESEC4 (MCLK/128/(0xff+1))  //@60Mhz, 128*4us resolution, max 32.53 sec


#define EnterPWDN(clkcon) ((void (*)(int))0xe0)(clkcon)
#define DOWNLOAD_ADDRESS _RAM_STARTADDRESS



/*44blib.c*/
void Delay(int time); //Watchdog Timer is used.
void *malloc(unsigned nbyte); 
void free(void *pt);
void Port_Init(void);
void Cache_Flush(void);
void ChangeMemCon(unsigned *pMemCfg);
void Uart_Select(int ch);
void Uart_TxEmpty(int ch);
void Uart_Init(int mclk,int baud);
char Uart_Getch(void);
char Uart_GetKey(void);
int  Uart_GetIntNum(void);
void Uart_SendByte(int data);
void Uart_Printf(char *fmt,...);
void Uart_SendString(char *pt);
void Timer_Start(int divider); //Watchdog Timer is used.
int Timer_Stop(void);          //Watchdog Timer is used.
//void restart(void);
//void run(void);
void Led_Display(int LedStatus);
void Beep(int BeepStatus);
void ChangePllValue(int m,int p,int s);
