/*
** $Id: timer-lite.c,v 1.22 2003/09/04 03:46:47 weiym Exp $
**
** timer-lite.c: The timer handling for MiniGUI Lite Version.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.04.21
*/ 

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/*
** TODO:
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "timer.h"

#ifndef _STAND_ALONE
#include "client.h"
#include "sharedres.h"
#endif

unsigned int __mg_timer_counter;

static TIMER *timerstr [MAX_TIMERS];

BOOL InitTimer (void)
{
    __mg_timer_counter = 0;
    return TRUE;
}

void TerminateTimer (void)
{
    int i;

    for (i = 0; i < MAX_TIMERS; i++) {
        if (timerstr[i] != NULL)
            free (timerstr[i]);
        timerstr[i] = NULL;
    }
}

/************************* Functions run in desktop thread *******************/
void DispatchTimerMessage (unsigned int inter)
{
    PMAINWIN pWin;
    PMSGQUEUE pMsgQueue;
    int i, slot;

    for (i = 0; i < MAX_TIMERS; i++ ) {
        if ( timerstr[i] ) {
#if _TIMER_UNIT_10MS
    	    timerstr[i]->count += inter;
#else
    	    timerstr[i]->count += 1<<7;
#endif
            if ( timerstr[i]->count >= timerstr[i]->speed ) {

                pWin = GetMainWindowPtrOfControl (timerstr[i]->hWnd);
                if (pWin == NULL) continue;

                pMsgQueue = pWin->pMessages;

                for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
                    if (pMsgQueue->TimerID[slot] == timerstr[i]->id
                        && pMsgQueue->TimerOwner[slot] == timerstr[i]->hWnd)
                        break;
                }
                if (slot != DEF_NR_TIMERS) {
                    pMsgQueue->dwState |= (0x01 << slot);
                }

                timerstr[i]->count -= timerstr[i]->speed;
            }
        }
    }
}

BOOL AddTimer (HWND hWnd, int id, unsigned int speed)
{
    int i;
    PMAINWIN pWin;
    PMSGQUEUE pMsgQueue;
    int slot;

    if (!(pWin = GetMainWindowPtrOfControl (hWnd))) return FALSE;

    pMsgQueue = pWin->pMessages;
    // Is there an empty timer slot?
    for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
        if ((pMsgQueue->TimerMask >> slot) & 0x01)
            break;
    }
    if (slot == DEF_NR_TIMERS) goto badret;

    for (i = 0; i < MAX_TIMERS; i++)
        if ( timerstr[i] != NULL )
            if ( timerstr[i]->hWnd == hWnd && timerstr[i]->id == id)
                goto badret;

    for (i = 0; i < MAX_TIMERS; i++)
        if ( timerstr[i] == NULL )
            break;

    if (i == MAX_TIMERS)
        goto badret ;

    timerstr[i] = malloc (sizeof (TIMER));

#if _TIMER_UNIT_10MS
    timerstr[i]->speed = speed;
#else
    timerstr[i]->speed = (1000<<7)/speed;
#endif
    timerstr[i]->hWnd = hWnd;
    timerstr[i]->id = id;
    timerstr[i]->count = 0;

    pMsgQueue->TimerOwner[slot] = hWnd;
    pMsgQueue->TimerID[slot] = id;
    pMsgQueue->TimerMask &= ~(0x01 << slot);

#ifndef _STAND_ALONE
    if (!mgIsServer)
        set_select_timeout (USEC_10MS * speed);
#endif

    return TRUE;
    
badret:

    return FALSE;
}

BOOL RemoveTimer (HWND hWnd, int id)
{
    int i;
    PMAINWIN pWin;
    PMSGQUEUE pMsgQueue;
    int slot;
    void* temp;

    if (!(pWin = GetMainWindowPtrOfControl (hWnd))) return FALSE;

    pMsgQueue = pWin->pMessages;
    for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
        if (pMsgQueue->TimerOwner[slot] == hWnd && pMsgQueue->TimerID[slot] == id)
            break;
    }
    if (slot == DEF_NR_TIMERS) goto badret;

    for (i = 0; i < MAX_TIMERS; i++)
        if (timerstr[i] != NULL)
            if (timerstr[i]->hWnd == hWnd && timerstr[i]->id == id)
                break;

    if (i == MAX_TIMERS) goto badret;
    
    temp = timerstr[i];
    timerstr[i] = NULL;
    free (temp);

    pMsgQueue->TimerMask |= (0x01 << slot);

#ifndef _STAND_ALONE
    if (!pMsgQueue->TimerMask && !mgIsServer)
        set_select_timeout (0);
#endif
        
    return TRUE;

badret:
    return FALSE;
}

BOOL GUIAPI IsTimerInstalled (HWND hWnd, int id)
{
    int i;

    for (i=0; i<MAX_TIMERS; i++) {
        if ( timerstr[i] != NULL ) {
            if ( timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

BOOL SetTimerSpeed (HWND hWnd, int id, unsigned int speed)
{
    int i;
    
    for (i=0; i<MAX_TIMERS; i++)
	if (timerstr[i]->hWnd == hWnd && timerstr[i]->id == id) {
#if _TIMER_UNIT_10MS
		timerstr[i]->speed = speed;
#else
		timerstr[i]->speed = (1000<<7)/speed;
#endif
		timerstr[i]->count = 0;
        return TRUE;
	}

    return FALSE;
}

/****************** Timer Interfaces for applications ************************/
unsigned int GUIAPI GetTickCount ()
{
#ifdef _STAND_ALONE
    return __mg_timer_counter;
#else
    return SHAREDRES_TIMER_COUNTER;
#endif
}

BOOL GUIAPI SetTimer (HWND hWnd, int id, unsigned int speed)
{
    TIMER timer;
    
    timer.hWnd = hWnd;
    timer.id = id;
    timer.speed = speed;
    
    return SendMessage (HWND_DESKTOP, MSG_ADDTIMER, 0, (LPARAM)&timer);
}

BOOL GUIAPI KillTimer (HWND hWnd, int id)
{
    TIMER timer;
    
    timer.hWnd = hWnd;
    timer.id = id;
    
    return SendMessage (HWND_DESKTOP, MSG_REMOVETIMER, 0, (LPARAM)&timer);
}

BOOL GUIAPI ResetTimer (HWND hWnd, int id, unsigned int speed)
{
    TIMER timer;
    
    timer.hWnd = hWnd;
    timer.id = id;
    timer.speed = speed;
    
    return SendMessage (HWND_DESKTOP, MSG_RESETTIMER, 0, (LPARAM)&timer);
}

