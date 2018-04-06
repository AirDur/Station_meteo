/*
*********************************************************************************************************
*                                           TIMERS
*                                      DEVICE INTERFACE
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          INCLUDES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          DEFINES
*********************************************************************************************************
*/

#define ID_TIMER0    0
#define ID_TIMER1    1
#define ID_TIMER2    2
#define ID_TIMER3    3
#define ID_TIMER4    4

#define MODE_STOP        0x0
#define MODE_RUN         0x1
#define MODE_UPDATE      0x2
#define MODE_INV_OUTPUT  0x4
#define MODE_RELOAD      0x8
#define MODE_DEADZONE    0x10

#define PARAM_MODE        0
#define PARAM_QUANTUM_US  1
#define PARAM_CMP_VALUE   2
#define PARAM_CPT_VALUE   3

/*
*********************************************************************************************************
*                                        TYPES DEFINITIONS
*********************************************************************************************************
*/



/*
*********************************************************************************************************
*                                        FUNCTIONS PROTOTYPES
*********************************************************************************************************
*/

int TIMER_SET(int ID,unsigned short MODE,unsigned short QUANTUM_US,unsigned short CMP_VALUE,unsigned short CPT_VALUE,void * CALLBACK);
void TIMER_CHANGE_CPT(int ID,unsigned short CPT_VALUE);
void TIMER_CHANGE_CMP(int ID,unsigned short CMP_VALUE);
void TIMER_CHANGE_MODE(int ID,unsigned short MODE);
int TIMER_CHANGE_QUANTUM(int ID,unsigned short QUANTUM_US);
unsigned short  TIMER_GET_CPT(int ID);
unsigned short  TIMER_GET_CMP(int ID);
unsigned short  TIMER_GET_VALUE(int ID);

