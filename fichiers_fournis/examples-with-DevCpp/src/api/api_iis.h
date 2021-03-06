/*
*********************************************************************************************************
*                                           IIS
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

#define	MAX_WAVESIZE	   0xfffff
#define  IIS_FIFOREADY	   !((rIISCON>>7)&1)	
#define	WAVE_FORMAT_PCM	0x0001

/*
*********************************************************************************************************
*                                        TYPES DEFINITIONS
*********************************************************************************************************
*/

typedef struct tWAVEFORMATEX {
	U16 wFormatTag;
	U16 nChannels;
	U32 nSamplesPerSec;
	U32 nAvgBytesPerSec;
	U16 nBlockAlign;
	U16 wBitsPerSample;
	U16 cbSize;
} WAVEFORMATEX,* LPWAVEFORMATEX;

/*
*********************************************************************************************************
*                                        FUNCTIONS PROTOTYPES
*********************************************************************************************************
*/

void Play_Wave(U32 addr, U32 size);
