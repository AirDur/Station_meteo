/*
*********************************************************************************************************
*                                        REAL TIME CLOCK
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


void RTC_GET_DATE(unsigned char *DATE,unsigned char *DAY,unsigned char *MON,unsigned char *YEAR)
{
rRTCCON = 0x01;
*YEAR = rBCDYEAR;
*MON = rBCDMON;
*DAY= rBCDDAY;
*DATE = rBCDDATE;
}

void RTC_SET_DATE(unsigned char DATE,unsigned char DAY,unsigned char MON,unsigned char YEAR)
{
rRTCCON = 0x01;
rBCDYEAR = YEAR;
rBCDMON = MON;
rBCDDAY = DAY;
rBCDDATE = DATE;
}

void RTC_GET_TIME(unsigned char *HOUR,unsigned char *MIN,unsigned char *SEC)
{
rRTCCON = 0x01;
*HOUR = rBCDHOUR;
*MIN = rBCDMIN;
*SEC= rBCDSEC;
}

void RTC_SET_TIME(unsigned char HOUR,unsigned char MIN,unsigned char SEC)
{
rRTCCON = 0x01;
rBCDHOUR = HOUR;
rBCDMIN = MIN;
rBCDSEC = SEC;
}

