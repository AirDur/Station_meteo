/*
** $Id: standalone.c,v 1.13 2004/09/28 03:53:02 weiym Exp $
** 
** standalone.c: low-level routines for MiniGUI-Lite standalone version.
** 
** Copyright (C) 2003 Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2003/08/15
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

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <limits.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "gal.h"
#include "ial.h"
#include "cursor.h"
#include "event.h"
#include "menu.h"

extern unsigned int __mg_timer_counter;
static unsigned int old_timer_counter;

#ifndef _NEWGAL_ENGINE_BF533
static struct sigaction old_alarm_handler;
static struct itimerval old_timer;

static void itimersig_handler (int v)
{
#ifdef __uClinux__
    __mg_timer_counter += 10;
#else
    __mg_timer_counter += 1;
#endif
}

static BOOL InstallTimer (void)
{
    old_timer_counter = 0;
/*
    struct itimerval timerv;
    struct sigaction siga;
    
    sigaction (SIGALRM, NULL, &old_alarm_handler);

    siga = old_alarm_handler;
    siga.sa_handler = itimersig_handler;
    siga.sa_flags = SA_RESTART | SA_NOMASK;
    sigaction (SIGALRM, &siga, NULL);

    timerv.it_interval.tv_sec = 0;
#ifdef __uClinux__
    timerv.it_interval.tv_usec = 100000;     // 100 ms
#else
    timerv.it_interval.tv_usec = 10000;     // 10 ms
#endif
    timerv.it_value = timerv.it_interval;

    if (setitimer (ITIMER_REAL, &timerv, &old_timer)) {
#ifdef _DEBUG
        log_sys ("setitimer call failed!\n");
#else
        return FALSE;
#endif
    }
*/
    return TRUE;
}

static BOOL UninstallTimer (void)
{
/*
    if (setitimer (ITIMER_REAL, &old_timer, 0) == -1) {
#ifdef _DEBUG
        log_sys ("setitimer call failed!\n");
#else
        return FALSE;
#endif

    }

    if (sigaction (SIGALRM, &old_alarm_handler, NULL) == -1) {
#ifdef _DEBUG
        log_sys ("sigaction call failed!\n");
#else
        return FALSE;
#endif
    }
*/
    return TRUE;
}

#else /* _NEWGAL_ENGINE_BF533 */

static BOOL InstallTimer (void)
{
    return TRUE;
}

static BOOL UninstallTimer (void)
{
    return TRUE;
}

#endif

static BOOL QueueMessage (PMSGQUEUE msg_que, PMSG msg)
{
    if ((msg_que->writepos + 1) % msg_que->len == msg_que->readpos) {
        return FALSE;
    }

    // Write the data and advance write pointer */
    msg_que->msg [msg_que->writepos] = *msg;

    msg_que->writepos++;
    if (msg_que->writepos >= msg_que->len) msg_que->writepos = 0;

    msg_que->dwState |= QS_POSTMSG;
    return TRUE;
}

static SRVEVTHOOK srv_evt_hook;

SRVEVTHOOK GUIAPI SetServerEventHook (SRVEVTHOOK SrvEvtHook)
{
    SRVEVTHOOK old_hook;
    
    old_hook = srv_evt_hook;
    srv_evt_hook = SrvEvtHook;
    return old_hook;
}

static void ParseEvent (PMSGQUEUE msg_que, int event)
{
    LWEVENT lwe;
    PMOUSEEVENT me;
    PKEYEVENT ke;
    MSG Msg;

//    uHALr_printf("ParseEvent\n");
    
    ke = &(lwe.data.ke);
    me = &(lwe.data.me);
    me->x = 0; me->y = 0;
    Msg.hwnd = HWND_DESKTOP;
    Msg.wParam = 0;
    Msg.lParam = 0;

    lwe.status = 0L;

    if (!GetLWEvent (event, &lwe))
        return;

    Msg.time = __mg_timer_counter;
    if (lwe.type == LWETYPE_TIMEOUT) {
        Msg.message = MSG_TIMEOUT;
        Msg.wParam = (WPARAM)lwe.count;
        Msg.lParam = 0;

        QueueMessage (msg_que, &Msg);
    }
    else if (lwe.type == LWETYPE_KEY) {
        Msg.wParam = ke->scancode;
        Msg.lParam = ke->status;
        if(ke->event == KE_KEYDOWN){
            Msg.message = MSG_KEYDOWN;
        }
        else if(ke->event == KE_KEYUP) {
            Msg.message = MSG_KEYUP;
        }

        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            QueueMessage (msg_que, &Msg);
        }
    }
    else if (lwe.type == LWETYPE_MOUSE) {
        Msg.wParam = me->status;
        switch (me->event) {
        case ME_MOVED:
            Msg.message = MSG_MOUSEMOVE;
            SetCursor (GetSystemCursor (IDC_ARROW));
            break;
        case ME_LEFTDOWN:
            Msg.message = MSG_LBUTTONDOWN;
            break;
        case ME_LEFTUP:
            Msg.message = MSG_LBUTTONUP;
            break;
        case ME_LEFTDBLCLICK:
            Msg.message = MSG_LBUTTONDBLCLK;
            break;
        case ME_RIGHTDOWN:
            Msg.message = MSG_RBUTTONDOWN;
            break;
        case ME_RIGHTUP:
            Msg.message = MSG_RBUTTONUP;
            break;
        case ME_RIGHTDBLCLICK:
            Msg.message = MSG_RBUTTONDBLCLK;
            break;
        }

        Msg.lParam = MAKELONG (me->x, me->y);
        if (!(srv_evt_hook && srv_evt_hook (&Msg))) {
            QueueMessage (msg_que, &Msg);
        }
    }
}

BOOL GUIAPI StandAloneStartup (void)
{
    srv_evt_hook = NULL;
    InstallTimer ();
    return TRUE;
}

void StandAloneCleanup (void)
{
    UninstallTimer ();
}

BOOL IdleHandler4StandAlone (PMSGQUEUE msg_queue)
{
    int    i, n;
        
    n = IAL_WaitEvent (IAL_MOUSEEVENT | IAL_KEYEVENT);

    /* handle intput event (mouse/touch-screen or keyboard) */
    if (n & IAL_MOUSEEVENT) ParseEvent (msg_queue, IAL_MOUSEEVENT);
    if (n & IAL_KEYEVENT) ParseEvent (msg_queue, IAL_KEYEVENT);
    if (n == 0) ParseEvent (msg_queue, 0);

    return TRUE;
}

