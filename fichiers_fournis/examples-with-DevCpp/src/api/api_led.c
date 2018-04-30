/*
*********************************************************************************************************
*                                             LED
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
#include "api_led.h"

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

static unsigned int LED_Status=0;

/*
*********************************************************************************************************
*                                        FUNCTIONS IMPLEMENTATION
*********************************************************************************************************
*/


void LED_ON(unsigned int ID)
   {
   LED_Status = (LED_Status | (1 << ID)) & 0x07;
   Led_Display(LED_Status);
   return;
   }

void LED_OFF(unsigned int ID)
   {
   LED_Status = (LED_Status & ~(1 << ID)) & 0x07;
   Led_Display(LED_Status);
   return;
   }

void LED_TOGGLE(unsigned int ID)
   {
   LED_Status = (LED_Status ^ (1 << ID)) & 0x07;
   Led_Display(LED_Status);
   return;
   }

void LED_DISPLAY(INT32U MASK)
   {
   LED_Status = (MASK & 0x7);
   Led_Display(LED_Status);
   return;
   }