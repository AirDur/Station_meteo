/*
*********************************************************************************************************
*                                           TIMERS
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
#include "api_timers.h"

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

void(*callback_0)(void);
void(*callback_1)(void);
void(*callback_2)(void);
void(*callback_3)(void);
void(*callback_4)(void);

/*
*********************************************************************************************************
*                                        FUNCTIONS IMPLEMENTATION
*********************************************************************************************************
*/

void Timer0Done(void) __attribute__((interrupt("IRQ")));
void Timer1Done(void) __attribute__((interrupt("IRQ")));
void Timer2Done(void) __attribute__((interrupt("IRQ")));
void Timer3Done(void) __attribute__((interrupt("IRQ")));
void Timer4Done(void) __attribute__((interrupt("IRQ")));

int TIMER_SET(int ID,unsigned short MODE,unsigned short QUANTUM_US,unsigned short CMP_VALUE,unsigned short CPT_VALUE,void * CALLBACK)
{
   int div,mux;

   div = (MCLK / 2 / 1000000) * QUANTUM_US;
   mux = 0;
   while(div > 255)
      {
      div /= 2;
      mux ++;
      }

   if(mux > 4)
      return -1;
               
   switch  (ID)
      {
      case ID_TIMER0:

         rTCFG0 &= 0xFFFFFF00;
         rTCFG0 |= div;
         rTCFG1 &= 0xFFFFFFF0;
         rTCFG1 |= mux;
            
         rTCMPB0 = CMP_VALUE;
         rTCNTB0 = CPT_VALUE;

         rTCON &= 0xFFFFFF00;
         rTCON |= MODE;

         if(CALLBACK)
            {
            callback_0 = CALLBACK;
            pISR_TIMER0=(int)Timer0Done;
            rINTMSK &= ~BIT_TIMER0;
            }

         break;

      case ID_TIMER1:

         rTCFG0 &= 0xFFFFFF00;
         rTCFG0 |= div;
         rTCFG1 &= 0xFFFFFF0F;
         rTCFG1 |= ((mux << 4)& 0xF0);
            
         rTCMPB1 = CMP_VALUE;
         rTCNTB1 = CPT_VALUE;
         rTCON &= 0xFFFFF0FF;
         rTCON |= ((MODE << 8)& 0xF00);

         if(CALLBACK)
            {
            callback_1 = CALLBACK;
            pISR_TIMER1=(int)Timer1Done;
            rINTMSK &= ~BIT_TIMER1;
            }
            
         break;

      case ID_TIMER2:

         rTCFG0 &= 0xFFFF00FF;
         rTCFG0 |= ((div << 8)& 0xFF00);
         rTCFG1 &= 0xFFFFF0FF;
         rTCFG1 |= ((mux << 8)& 0xF00);
            
         rTCMPB2 = CMP_VALUE;
         rTCNTB2 = CPT_VALUE;
         rTCON &= 0xFFFF0FFF;
         rTCON |= ((MODE << 12)& 0xF000);

         if(CALLBACK)
            {
            callback_2 = CALLBACK;
            pISR_TIMER2=(int)Timer2Done;
            rINTMSK &= ~BIT_TIMER2;
            }
            
         break;

      case ID_TIMER3:

         rTCFG0 &= 0xFFFF00FF;
         rTCFG0 |= ((div << 8)& 0xFF00);
         rTCFG1 &= 0xFFFF0FFF;
         rTCFG1 |= ((mux << 12)& 0xF000);
            
         rTCMPB3 = CMP_VALUE;
         rTCNTB3 = CPT_VALUE;
         rTCON &= 0xFFF0FFFF;
         rTCON |= ((MODE << 16)& 0xF0000);

         if(CALLBACK)
            {
            callback_3 = CALLBACK;
            pISR_TIMER3=(int)Timer3Done;
            rINTMSK &= ~BIT_TIMER3;
            }
            
         break;

      case ID_TIMER4:
         if(mux > 3)
            return -1;

         rTCFG0 &= 0xFF00FFFF;
         rTCFG0 |= ((div << 16)& 0xFF0000);
         rTCFG1 &= 0xFFF0FFFF;
         rTCFG1 |= ((mux << 16)& 0xF0000);
            
         rTCMPB4 = CMP_VALUE;
         rTCNTB4 = CPT_VALUE;
         rTCON &= 0xFF0FFFFF;
         rTCON |= ((MODE << 20)& 0xF00000);

         if(CALLBACK)
            {
            callback_4 = CALLBACK;
            pISR_TIMER4=(int)Timer4Done;
            rINTMSK &= ~BIT_TIMER4;
            }
            
         break;
         
      default:
         break;
      }
}

void TIMER_CHANGE_MODE(int ID,unsigned short MODE)
{
   switch  (ID)
      {
      case ID_TIMER0:
         rTCON &= 0xFFFFFF00;
         rTCON |= MODE;
         break;

      case ID_TIMER1:
         rTCON &= 0xFFFFF0FF;
         rTCON |= ((MODE << 8)& 0xF00);
         break;

      case ID_TIMER2:
         rTCON &= 0xFFFF0FFF;
         rTCON |= ((MODE << 12)& 0xF000);
         break;

      case ID_TIMER3:
         rTCON &= 0xFFF0FFFF;
         rTCON |= ((MODE << 16)& 0xF0000);
         break;

      case ID_TIMER4:
         rTCON &= 0xFF0FFFFF;
         rTCON |= ((MODE << 20)& 0xF00000);
         break;

      default:
         break;
      }
}

int TIMER_CHANGE_QUANTUM(int ID,unsigned short QUANTUM_US)
{
   int div,mux;

   div = (MCLK / 2 / 1000000) * QUANTUM_US;
   mux = 0;
   while(div > 255)
      {
      div /= 2;
      mux ++;
      }

   if(mux > 4)
      return -1;

   switch  (ID)
      {
      case ID_TIMER0:
         rTCFG0 &= 0xFFFFFF00;
         rTCFG0 |= div;
         rTCFG1 &= 0xFFFFFFF0;
         rTCFG1 |= mux;
         break;

      case ID_TIMER1:
         rTCFG0 &= 0xFFFFFF00;
         rTCFG0 |= div;
         rTCFG1 &= 0xFFFFFF0F;
         rTCFG1 |= ((mux << 4)& 0xF0);
         break;

      case ID_TIMER2:
         rTCFG0 &= 0xFFFF00FF;
         rTCFG0 |= ((div << 8)& 0xFF00);
         rTCFG1 &= 0xFFFFF0FF;
         rTCFG1 |= ((mux << 8)& 0xF00);
         break;

      case ID_TIMER3:
         rTCFG0 &= 0xFFFF00FF;
         rTCFG0 |= ((div << 8)& 0xFF00);
         rTCFG1 &= 0xFFFF0FFF;
         rTCFG1 |= ((mux << 12)& 0xF000);
         break;

      case ID_TIMER4:
         if(mux > 3)
            return -1;

         rTCFG0 &= 0xFF00FFFF;
         rTCFG0 |= ((div << 16)& 0xFF0000);
         rTCFG1 &= 0xFFF0FFFF;
         rTCFG1 |= ((mux << 16)& 0xF0000);
         break;

      default:
         break;
      }
}

void TIMER_CHANGE_CMP(int ID,unsigned short CMP_VALUE)
{
   switch  (ID)
      {
      case ID_TIMER0:
         rTCMPB0 = CMP_VALUE;
         break;

      case ID_TIMER1:
         rTCMPB1 = CMP_VALUE;
         break;

      case ID_TIMER2:
         rTCMPB2 = CMP_VALUE;
         break;

      case ID_TIMER3:
         rTCMPB3 = CMP_VALUE;
         break;

      case ID_TIMER4:
         rTCMPB4 = CMP_VALUE;
         break;

      default:
         break;
      }
}

void TIMER_CHANGE_CPT(int ID,unsigned short CPT_VALUE)
{
   switch  (ID)
      {
      case ID_TIMER0:
         rTCNTB0 = CPT_VALUE;
         break;

      case ID_TIMER1:
         rTCNTB1 = CPT_VALUE;
         break;

      case ID_TIMER2:
         rTCNTB2 = CPT_VALUE;
         break;

      case ID_TIMER3:
         rTCNTB3 = CPT_VALUE;
         break;

      case ID_TIMER4:
         rTCNTB4 = CPT_VALUE;
         break;

      default:
         break;
      }
}

unsigned short TIMER_GET_CMP(int ID)
{
   switch  (ID)
      {
      case ID_TIMER0:
         return rTCMPB0;
         break;

      case ID_TIMER1:
         return rTCMPB1;
         break;

      case ID_TIMER2:
         return rTCMPB2;
         break;

      case ID_TIMER3:
         return rTCMPB3;
         break;

      case ID_TIMER4:
         return rTCMPB4;
         break;

      default:
         break;
      }
}

unsigned short TIMER_GET_CPT(int ID)
{
   switch  (ID)
      {
      case ID_TIMER0:
         return rTCNTB0;
         break;

      case ID_TIMER1:
         return rTCNTB1;
         break;

      case ID_TIMER2:
         return rTCNTB2;
         break;

      case ID_TIMER3:
         return rTCNTB3;
         break;

      case ID_TIMER4:
         return rTCNTB4;
         break;

      default:
         break;
      }
}

unsigned short TIMER_GET_VALUE(int ID)
{
   switch  (ID)
      {
      case ID_TIMER0:
         return rTCNTO0;
         break;

      case ID_TIMER1:
         return rTCNTO1;
         break;

      case ID_TIMER2:
         return rTCNTO2;
         break;

      case ID_TIMER3:
         return rTCNTO3;
         break;

      case ID_TIMER4:
         return rTCNTO4;
         break;

      default:
         break;
      }
}

void Timer0Done(void)
{
    (*callback_0)();
    rI_ISPC=BIT_TIMER0;
}

void Timer1Done(void)
{
    (*callback_1)();
    rI_ISPC=BIT_TIMER1;
}

void Timer2Done(void)
{
    (*callback_2)();
    rI_ISPC=BIT_TIMER2;
}

void Timer3Done(void)
{
    (*callback_3)();
    rI_ISPC=BIT_TIMER3;
}

void Timer4Done(void)
{
    (*callback_4)();
    rI_ISPC=BIT_TIMER4;
}

