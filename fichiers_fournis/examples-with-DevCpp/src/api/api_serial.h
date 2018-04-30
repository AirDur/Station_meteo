/*
*********************************************************************************************************
*                                   SERIAL COMMUNICATIONS
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

#define SERIAL_DATA_5B  0x0
#define SERIAL_DATA_6B  0x1
#define SERIAL_DATA_7B  0x2
#define SERIAL_DATA_8B  0x3

#define SERIAL_STOP_1B  0x0
#define SERIAL_STOP_2B  0x4

#define SERIAL_PARITY_NONE     0x0
#define SERIAL_PARITY_ODD      0x20
#define SERIAL_PARITY_EVEN     0x28
#define SERIAL_PARITY_1FORCED  0x30
#define SERIAL_PARITY_0FORCED  0x38

#define SERIAL_NO_BUFFER       0x0
#define SERIAL_BUFFER          0x1

#define SERIAL_NO_WAIT         0x0
#define SERIAL_WAIT            0x1

#define SERIAL_BUF_LEN 100

/*
*********************************************************************************************************
*                                        TYPES DEFINITIONS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                          FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void SERIAL_SET_CONF(int UART,int COM_MODE,int SPEED,int BUF_MODE);
void SERIAL_SELECT_UART(int UART);
void SERIAL_TX_BUF(char * BUFFER,int LEN);
void SERIAL_RX_BUF(char *BUFFER,int LEN);
void SERIAL_TX_CHAR(char CAR);
char SERIAL_RX_WAIT_CHAR();
char SERIAL_RX_NOWAIT_CHAR();
int  SERIAL_GET_BUF_FIFO(char * BUF,int LEN,int WAIT);
int SERIAL_PUT_BUF_FIFO(char * BUF,int LEN,int WAIT);
char SERIAL_GET_CHAR_FIFO(int WAIT);
void SERIAL_PUT_CHAR_FIFO(char CAR,int WAIT);
void SERIAL_FLUSH_FIFO_RX(void);
void SERIAL_FLUSH_FIFO_TX(void);
int SERIAL_LEN_RX_FIFO(void);
int SERIAL_LEN_TX_FIFO(void);
