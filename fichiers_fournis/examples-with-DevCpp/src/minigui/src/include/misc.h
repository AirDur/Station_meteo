/*
** $Id: misc.h,v 1.20 2005/01/31 10:54:09 clear Exp $
**
** misc.h: the head file for Miscellous module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/01/03
*/

#ifndef GUI_MISC_H
    #define GUI_MISC_H

/* Function definitions */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#if defined(__CC_ARM) || defined (__VXWORKS__) || defined(WIN32)

struct timeval {
    long    tv_sec;         /* seconds */
    long    tv_usec;        /* microseconds */
};

struct timezone {
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

#else
  #include <sys/time.h>
#endif

void __mg_os_time_delay (int ms);

typedef struct _ETCSECTION
{
    int key_nr;               /* key number in the section */
    char *name;               /* name of the section */
    char **keys;              /* key string arrays */
    char **values;            /* value string arrays */
} ETCSECTION;
typedef ETCSECTION* PETCSECTION;

typedef struct _ETC_S
{
    int section_nr;           /* number of sections */
    PETCSECTION sections;     /* pointer to section arrays */
} ETC_S;


extern GHANDLE hMgEtc;

BOOL InitMgEtc (void);
void TerminateMgEtc (void);

BOOL InitMisc (void);
void TerminateMisc (void);

BOOL InitSystemRes (void);
void TerminateSystemRes (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_MISC_H */

