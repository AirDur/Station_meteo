/*
** $Id: timer.h,v 1.10 2004/09/23 08:34:58 weiym Exp $
**
** tiemr.h: the head file of Timer module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/4/21
*/

#ifndef GUI_TIMER_H
    #define GUI_TIMER_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __uClinux__
    #define MAX_TIMERS      8
#else
    #define MAX_TIMERS      16
#endif

#define USEC_1S         1000000
#define USEC_10MS       10000

typedef struct _timer {
    HWND    hWnd;
    int     id;
	unsigned int speed;
	unsigned int count;
}TIMER;
typedef TIMER* PTIMER;

BOOL InitTimer (void);
void TerminateTimer (void);

BOOL AddTimer (HWND hWnd, int id, unsigned int speed);
BOOL RemoveTimer (HWND hWnd, int id);
BOOL IsTimerInstalled (HWND hWnd, int id);
BOOL SetTimerSpeed (HWND hWnd, int id, unsigned int speed);
void DispatchTimerMessage (unsigned int inter);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_TIMER_H */

