/*
** $Id: event.c,v 1.36 2005/01/05 05:47:48 weiym Exp $
**
** event.c: Low level event handling module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.01.11
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

#include "common.h"

#ifndef __NOUNIX__
#    include <unistd.h>
#    include <sys/time.h>
#endif

#ifdef HAVE_LINUX_KEYBOARD_H
  #include <linux/keyboard.h>
#endif
    
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "internals.h"
#include "ial.h"
#include "cursor.h"
#include "event.h"
#include "misc.h"

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
#include "sharedres.h"
#endif

struct timeval __mg_event_timeout;

static int timeoutusec;
static int repeatusec;
static int dblclicktime;

#ifdef _LITE_VERSION
static unsigned int timeout_threshold;
static unsigned int repeat_threshold;
static unsigned int timeout_count;
#endif

static void GetDblclickTime(void)
{
    char szValue[11];
    int ms;

    dblclicktime = DEF_MSEC_DBLCLICK / 10;

    if( GetMgEtcValue (MOUSEPARA, MOUSEPARA_DBLCLICKTIME, szValue, 10) < 0 )
        return;

    ms = atoi(szValue);

    if (ms > 0 && ms < 1000) {
        dblclicktime = ms / 10;
    }
}

static void GetTimeout (void)
{
    char szValue [11];
    int mytimeoutusec, myrepeatusec;

    timeoutusec = DEF_USEC_TIMEOUT;
    repeatusec = DEF_REPEAT_TIME;

    if (GetMgEtcValue (EVENTPARA, EVENTPARA_REPEATUSEC, szValue, 10) < 0)
        return;
    myrepeatusec = atoi(szValue);

    if( GetMgEtcValue (EVENTPARA, EVENTPARA_TIMEOUTUSEC, szValue, 10) < 0 )
        return;
    mytimeoutusec = atoi(szValue);

    if (myrepeatusec >= 0 && mytimeoutusec > 0) {
        timeoutusec = mytimeoutusec;
        repeatusec = myrepeatusec;
    }
}

// Mouse event parameters.
static int oldbutton = 0;

static unsigned int time1;
static unsigned int time2;

// Key event parameters.
static unsigned char oldkeystate [MGUI_NR_KEYS + 1];
static unsigned char olddownkey = 0;
static DWORD status;
static int alt1 = 0;               /* left alt key state */
static int alt2 = 0;               /* right alt key state */
static int capslock = 0;           /* caps lock key state */
static int esc = 0;                /* escape scan code detected? */
static int caps_off = 1;           /* 1 = normal position, 0 = depressed */
static int numlock = 0;            /* number lock key state */
static int num_off = 1;            /* 1 = normal position, 0 = depressed */
static int slock = 0;              /* scroll lock key state */
static int slock_off = 1;          /* 1 = normal position, 0 = depressed */
static int control1 = 0;           /* left control key state */
static int control2 = 0;           /* right control key state */
static int shift1 = 0;             /* left shift key state */
static int shift2 = 0;             /* left shift key state */

static void ResetMouseEvent(void)
{
    IAL_UpdateMouse ();
    oldbutton = IAL_GetMouseButton ();
    time1 = 0;
    time2 = 0;
}

static void ResetKeyEvent(void)
{
#if 0
    memcpy (oldkeystate, IAL_GetKeyboardState (), IAL_UpdateKeyboard ());
#else
    memset (oldkeystate, 0, MGUI_NR_KEYS + 1);
#endif
    olddownkey  = 0;
    status      = 0;
    alt1        = 0;
    alt2        = 0;
    esc         = 0;
    control1    = 0;
    control2    = 0;
    shift1      = 0;
    shift2      = 0;
    capslock    = 0;
    caps_off    = 1;
    numlock     = 0;
    num_off     = 1;
    slock       = 0;
    slock_off   = 1;
    IAL_SetLeds (slock | (numlock << 1) | (capslock << 2));

    __mg_event_timeout.tv_sec = 0;
    __mg_event_timeout.tv_usec = timeoutusec;

#ifdef _LITE_VERSION
#ifndef _STAND_ALONE
    SHAREDRES_TIMEOUT = __mg_event_timeout;
    SHAREDRES_SHIFTSTATUS = status;
#endif

    timeout_threshold = timeoutusec / 10000;
    repeat_threshold = repeatusec / 10000;
    timeout_count = timeout_threshold;
#endif
}

#ifdef _LITE_VERSION
BOOL GetLWEvent (int event, PLWEVENT lwe)
{
    static LWEVENT old_lwe = {0, 0};
    unsigned int interval;
    int button;
    PMOUSEEVENT me;
    PKEYEVENT ke;
    const unsigned char* keystate;
    int i;
    int make;       /* 0 = release, 1 = presse */
    
    old_lwe.type = 0;
    old_lwe.count = 0;
    me = &(lwe->data.me);
    ke = &(lwe->data.ke);
    
    if (event == 0) {
        if (__mg_timer_counter >= timeout_count) {

            timeout_count = __mg_timer_counter + repeat_threshold;

            // repeat last event
            if (old_lwe.type == LWETYPE_KEY 
                    && old_lwe.data.ke.event == KE_KEYDOWN) {
                memcpy (lwe, &old_lwe, sizeof (LWEVENT));
                lwe->data.ke.status |= KE_REPEATED;
                return 1;
            }

            if (!(old_lwe.type == LWETYPE_MOUSE
                    && (old_lwe.data.me.event == ME_LEFTDOWN ||
                        old_lwe.data.me.event == ME_RIGHTDOWN ||
                        old_lwe.data.me.event == ME_MIDDLEDOWN))) {
                // reset delay time
                timeout_count = __mg_timer_counter + timeout_threshold;
            }

            // reset delay time
            lwe->type = LWETYPE_TIMEOUT;
            lwe->count = __mg_timer_counter;
            return 1;
        }
        return 0;
    }

    timeout_count = __mg_timer_counter + timeout_threshold;
    // There was a event occurred.
    if (event & IAL_MOUSEEVENT) {
        if (!IAL_UpdateMouse ())
            return 0;

        lwe->type = LWETYPE_MOUSE;
        if (RefreshCursor(&me->x, &me->y, &button)) {
            me->event = ME_MOVED;
            time1 = 0;
            time2 = 0;

            if (oldbutton == button)
                {
                return 1;
                }
        }
  
        if ( !(oldbutton & IAL_MOUSE_LEFTBUTTON) && 
              (button & IAL_MOUSE_LEFTBUTTON) )
        {
            if (time1) {
                interval = __mg_timer_counter - time1;
                if (interval <= dblclicktime)
                    {
                    me->event = ME_LEFTDBLCLICK;
                    }
                else
                    me->event = ME_LEFTDOWN;
                time1 = 0;
            }
            else {
                time1 = __mg_timer_counter;
                me->event = ME_LEFTDOWN;
            }
            goto mouseret;
        }

        if ( (oldbutton & IAL_MOUSE_LEFTBUTTON) && 
             !(button & IAL_MOUSE_LEFTBUTTON) )
        {
            me->event = ME_LEFTUP;
            goto mouseret;
        }

        if ( !(oldbutton & IAL_MOUSE_RIGHTBUTTON) && 
              (button & IAL_MOUSE_RIGHTBUTTON) )
        {
            if (time2) {
                interval = __mg_timer_counter - time2;
                if (interval <= dblclicktime)
                    me->event = ME_RIGHTDBLCLICK;
                else
                    me->event = ME_RIGHTDOWN;
                time2 = 0;
            }
            else {
                time2 = __mg_timer_counter;
                me->event = ME_RIGHTDOWN;
            }
            goto mouseret;
        }

        if ( (oldbutton & IAL_MOUSE_RIGHTBUTTON) && 
            !(button & IAL_MOUSE_RIGHTBUTTON) )
        {
            me->event = ME_RIGHTUP;
            goto mouseret;
        }
    }

    if (event & IAL_KEYEVENT) {
        int nr_keys = IAL_UpdateKeyboard ();

        if (nr_keys == 0)
            return 0;

        lwe->type = LWETYPE_KEY;
        keystate = IAL_GetKeyboardState ();
        for(i = 0; i < nr_keys; i++) {
            if (!oldkeystate[i] && keystate[i]) {
                 ke->event = KE_KEYDOWN;
                 ke->scancode = i;
                 olddownkey = i;
                 break;
            }
            if (oldkeystate[i] && !keystate[i]) {
                 ke->event = KE_KEYUP;
                 ke->scancode = i;
                 break;
            }
        }
        if (i == nr_keys) {
            ke->event = KE_KEYDOWN;
            ke->scancode = olddownkey;
        }
        
        make = (ke->event == KE_KEYDOWN)?1:0;

        if (i != nr_keys) {
            unsigned leds;

            switch (ke->scancode) {
                case SCANCODE_CAPSLOCK:
                    if (make && caps_off) {
                        capslock = 1 - capslock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    caps_off = 1 - make;
                break;
                    
                case SCANCODE_NUMLOCK:
                    if (make && num_off) {
                        numlock = 1 - numlock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    num_off = 1 - make;
                break;
                
                case SCANCODE_SCROLLLOCK:
                    if (make & slock_off) {
                        slock = 1 - slock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    slock_off = 1 - make;
                    break;

                case SCANCODE_LEFTCONTROL:
                    control1 = make;
                    break;
                    
                case SCANCODE_RIGHTCONTROL:
                    control2 = make;
                    break;
                    
                case SCANCODE_LEFTSHIFT:
                    shift1 = make;
                    break;
                    
                case SCANCODE_RIGHTSHIFT:
                    shift2 = make;
                    break;
                    
                case SCANCODE_LEFTALT:
                    alt1 = make;
                    break;

                case SCANCODE_RIGHTALT:
                    alt2 = make;
                    break;
                    
            }

            status &= ~(MASK_KS_SHIFTKEYS);

            status |= (DWORD)((capslock << 8) |
                             (numlock << 7)   |
                             (slock << 6)     |
                             (control1 << 5)  |
                             (control2 << 4)  |
                             (alt1 << 3)      |
                             (alt2 << 2)      |
                             (shift1 << 1)    |
                             (shift2));
                             
            // Mouse button status
            if (oldbutton & IAL_MOUSE_LEFTBUTTON)
                status |= KS_LEFTBUTTON;
            else if (oldbutton & IAL_MOUSE_RIGHTBUTTON)
                status |= KS_RIGHTBUTTON;
        }
        ke->status = status;
#ifndef _STAND_ALONE
        SHAREDRES_SHIFTSTATUS = status;
#endif
        memcpy (oldkeystate, keystate, nr_keys);
        memcpy (&old_lwe, lwe, sizeof (LWEVENT));
        return 1;
    } 

    old_lwe.type = 0;
    return 0;

mouseret:
    status &= ~(MASK_KS_BUTTONS);
    oldbutton = button;
    if (oldbutton & IAL_MOUSE_LEFTBUTTON)
        status |= KS_LEFTBUTTON;
    if (oldbutton & IAL_MOUSE_RIGHTBUTTON)
        status |= KS_RIGHTBUTTON;
    me->status = status;
#ifndef _STAND_ALONE
    SHAREDRES_SHIFTSTATUS = status;
#endif
    memcpy (&old_lwe, lwe, sizeof (LWEVENT));
    return 1;
}

#else

BOOL GetLWEvent (int event, PLWEVENT lwe)
{
    static int timeout_count = 0;
    static LWEVENT old_lwe = {0, 0};

    int nr_keys = 0;
    unsigned int interval;
    int button;
    PMOUSEEVENT me = &(lwe->data.me);
    PKEYEVENT ke = &(lwe->data.ke);
    const unsigned char* keystate;
    int i;
    int make;       /* 0 = release, 1 = presse */

    if (event & IAL_MOUSEEVENT) {
        if (!IAL_UpdateMouse ())
            event &= ~IAL_MOUSEEVENT;
    }
    if (event & IAL_KEYEVENT) {
        if ((nr_keys = IAL_UpdateKeyboard ()) == 0)
            event &= ~IAL_KEYEVENT;
    }
    
    if (event == 0) {
        if (__mg_event_timeout.tv_sec == 0 && __mg_event_timeout.tv_usec == 0) {

            // set repeat time
            __mg_event_timeout.tv_sec = 0;
            __mg_event_timeout.tv_usec = repeatusec;

            // repeat last event
            if (old_lwe.type == LWETYPE_KEY 
                    && old_lwe.data.ke.event == KE_KEYDOWN) {
               memcpy (lwe, &old_lwe, sizeof (LWEVENT));
               lwe->data.ke.status |= KE_REPEATED;
               return 1;
            }

            if (!(old_lwe.type == LWETYPE_MOUSE
                    && (old_lwe.data.me.event == ME_LEFTDOWN ||
                        old_lwe.data.me.event == ME_RIGHTDOWN ||
                        old_lwe.data.me.event == ME_MIDDLEDOWN))) {
                // reset delay time
                __mg_event_timeout.tv_sec = 0;
                __mg_event_timeout.tv_usec = timeoutusec;
            }

            lwe->type = LWETYPE_TIMEOUT;
            lwe->count = ++timeout_count;
            return 1;
        }
        old_lwe.type = 0;
        return 0;
    }

    timeout_count = 0;
    // There was a event occurred.
    if (event & IAL_MOUSEEVENT) {
        lwe->type = LWETYPE_MOUSE;
        if (RefreshCursor(&me->x, &me->y, &button)) {

            me->event = ME_MOVED;
            time1 = 0;
            time2 = 0;

            if (oldbutton == button)
                return 1;
        }
   
        if ( !(oldbutton & IAL_MOUSE_LEFTBUTTON) && 
              (button & IAL_MOUSE_LEFTBUTTON) )
        {
            if (time1) {
                interval = __mg_timer_counter - time1;
                if (interval <= dblclicktime)
                    me->event = ME_LEFTDBLCLICK;
                else
                    me->event = ME_LEFTDOWN;
                time1 = 0;
            }
            else {
                time1 = __mg_timer_counter;
                me->event = ME_LEFTDOWN;
            }
            goto mouseret;
        }

        if ( (oldbutton & IAL_MOUSE_LEFTBUTTON) && 
             !(button & IAL_MOUSE_LEFTBUTTON) )
        {
            me->event = ME_LEFTUP;
            goto mouseret;
        }

        if ( !(oldbutton & IAL_MOUSE_RIGHTBUTTON) && 
              (button & IAL_MOUSE_RIGHTBUTTON) )
        {
            if (time2) {
                interval = __mg_timer_counter - time2;
                if (interval <= dblclicktime)
                    me->event = ME_RIGHTDBLCLICK;
                else
                    me->event = ME_RIGHTDOWN;
                time2 = 0;
            }
            else {
                time2 = __mg_timer_counter;
                me->event = ME_RIGHTDOWN;
            }
            goto mouseret;
        }

        if ( (oldbutton & IAL_MOUSE_RIGHTBUTTON) && 
            !(button & IAL_MOUSE_RIGHTBUTTON) )
        {
            me->event = ME_RIGHTUP;
            goto mouseret;
        }
    }

    if (event & IAL_KEYEVENT) {
        lwe->type = LWETYPE_KEY;
        keystate = IAL_GetKeyboardState ();
        for (i = 0; i < nr_keys; i++) {
            if(!oldkeystate[i] && keystate[i]) {
                 ke->event = KE_KEYDOWN;
                 ke->scancode = i;
                 olddownkey = i;
                 break;
            }
            if(oldkeystate[i] && !keystate[i]) {
                 ke->event = KE_KEYUP;
                 ke->scancode = i;
                 break;
            }
        }

        if (i == nr_keys) {
            ke->event = KE_KEYDOWN;
            ke->scancode = olddownkey;
        }
        
        make = (ke->event == KE_KEYDOWN) ? 1 : 0;

        if (i != nr_keys) {
            unsigned leds;

            switch (ke->scancode) {
                case SCANCODE_CAPSLOCK:
                    if (make && caps_off) {
                        capslock = 1 - capslock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    caps_off = 1 - make;
                break;
                    
                case SCANCODE_NUMLOCK:
                    if (make && num_off) {
                        numlock = 1 - numlock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    num_off = 1 - make;
                break;
                
                case SCANCODE_SCROLLLOCK:
                    if (make & slock_off) {
                        slock = 1 - slock;
                        leds = slock | (numlock << 1) | (capslock << 2);
                        IAL_SetLeds (leds);
                        status = (DWORD)leds << 16;
                    }
                    slock_off = 1 - make;
                    break;

                case SCANCODE_LEFTCONTROL:
                    control1 = make;
                    break;
                    
                case SCANCODE_RIGHTCONTROL:
                    control2 = make;
                    break;
                    
                case SCANCODE_LEFTSHIFT:
                    shift1 = make;
                    break;
                    
                case SCANCODE_RIGHTSHIFT:
                    shift2 = make;
                    break;
                    
                case SCANCODE_LEFTALT:
                    alt1 = make;
                    break;

                case SCANCODE_RIGHTALT:
                    alt2 = make;
                    break;
                    
            }

            status &= ~(MASK_KS_SHIFTKEYS);

            status |= (DWORD)((capslock << 8) |
                             (numlock << 7)   |
                             (slock << 6)     |
                             (control1 << 5)  |
                             (control2 << 4)  |
                             (alt1 << 3)      |
                             (alt2 << 2)      |
                             (shift1 << 1)    |
                             (shift2));
                             
            // Mouse button status
            if (oldbutton & IAL_MOUSE_LEFTBUTTON)
                status |= KS_LEFTBUTTON;
            else if (oldbutton & IAL_MOUSE_RIGHTBUTTON)
                status |= KS_RIGHTBUTTON;
        }
        ke->status = status;
        memcpy (oldkeystate, keystate, nr_keys);
        memcpy (&old_lwe, lwe, sizeof (LWEVENT));
        __mg_event_timeout.tv_sec = 0;
        __mg_event_timeout.tv_usec = timeoutusec;
        return 1;
    } 

    old_lwe.type = 0;
    return 0;

mouseret:
    status &= ~(MASK_KS_BUTTONS);
    oldbutton = button;
    if (oldbutton & IAL_MOUSE_LEFTBUTTON)
        status |= KS_LEFTBUTTON;
    if (oldbutton & IAL_MOUSE_RIGHTBUTTON)
        status |= KS_RIGHTBUTTON;
    me->status = status;
    memcpy (&old_lwe, lwe, sizeof (LWEVENT));
    __mg_event_timeout.tv_sec = 0;
    __mg_event_timeout.tv_usec = timeoutusec;
    return 1;
}
#endif

BOOL GUIAPI GetKeyStatus (UINT uKey)
{
#if defined (_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer) {
        if (uKey & 0xF000)      // this is a mouse key
            return SHAREDRES_BUTTON & (uKey >> 8);
        return 0;
    }
#endif

    if (uKey & 0xF000)      // this is a mouse key
        return oldbutton & (uKey >> 8);
    else if (uKey <= MGUI_NR_KEYS)
        return oldkeystate [uKey];

    return FALSE;
}

DWORD GUIAPI GetShiftKeyStatus (void)
{
#if defined (_LITE_VERSION) && !defined(_STAND_ALONE)
    return SHAREDRES_SHIFTSTATUS;
#else
    return status;
#endif
}

BOOL InitLWEvent (void)
{
//    GetDblclickTime ();
//    GetTimeout ();

    if (InitIAL ())
        return FALSE;

    ResetMouseEvent();
    dblclicktime = -1; /* -1 TEST */
//    ResetKeyEvent();

    return TRUE;
}

void TerminateLWEvent (void)
{
    ResetMouseEvent ();
//    ResetKeyEvent ();
    TerminateIAL ();
}

