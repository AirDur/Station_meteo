/*
** $Id: hh2410r3.c,v 1.3 2004/10/13 08:07:40 cxzhang Exp $
**
** hh2410r3.c: Low Level Input Engine for Huaheng ARM S3C2410 R3 Dev Board.
** 
** Copyright (C) 2004 Feynman Software.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _HH2410R3_IAL

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kd.h>

#include "ial.h"
#include "hh2410r3.h"

/* for data reading from /dev/touchscreen/0raw */
typedef struct {
    unsigned short pressure;
    unsigned short x;
    unsigned short y;
} TS_EVENT;

static int ts = -1;

static TS_EVENT ts_event;
static int mousex = 0;
static int mousey = 0;
static int stylus = 0;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static inline int get_middle (int x, int y, int z)
{
    return ((x + y + z) / 3);
}

static int mouse_update (void)
{
    int i=0;
    int xa[3], ya[3];

    for (i=0; i<3;) {

        TS_EVENT cBuffer;
        read (ts, &cBuffer, sizeof (TS_EVENT));

        if (cBuffer.pressure) {
            if (cBuffer.y >= 1022 || cBuffer.x >= 1022
                    ||cBuffer.y <= 72 || cBuffer.x <= 90) {
                if (stylus) {
                    stylus = 0;
                    return 1;
                }
                else
                    return 0;
            }
            else {
                xa[i] = cBuffer.x;
                ya[i] = cBuffer.y;
                i++;
            }
        }
        else {
            if (stylus) {
                stylus = 0;
                return 1;
            }
            else
                return 0;
        }
    }

#if 0
    mousex = (get_middle (xa[0], xa[1], xa[2]) - 167) * (220-20)/(924-167)+20;
    mousey = 300 - (get_middle (ya[0], ya[1], ya[2]) - 128) * (300-20)/(945-128);
#else
    mousex = get_middle (xa[0], xa[1], xa[2]);
    mousey = get_middle (ya[0], ya[1], ya[2]);
#endif
    stylus = 1;
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = mousex;
    *y = mousey;
}

static int mouse_getbutton (void)
{
    return (stylus ? 4 : 0);
}

#ifdef _LITE_VERSION 
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#else
static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
#endif
{
    int retvalue = 0;
    int e;

#ifdef _LITE_VERSION
    e = select (maxfd + 1, in, out, except, timeout);
#else
    e = select (FD_SETSIZE, in, out, except, timeout);
#endif

    if (e < 0) {
        return -1;
    }
    else if ((which & IAL_MOUSEEVENT) && ts >= 0) {
        retvalue |= IAL_MOUSEEVENT;
    }

    return retvalue;
}

BOOL InitHH2410R3Input (INPUT* input, const char* mdev, const char* mtype)
{
    /* mdev should be /dev/touchscreen/0raw */
    ts = open (mdev, O_RDONLY);
    if (ts < 0) {
        fprintf (stderr, "HH2410R3: Can not open touch screen: %s!\n", mdev);
        return FALSE;
    }

    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->wait_event = wait_event;
    mousex = 0;
    mousey = 0;
    ts_event.x = ts_event.y = ts_event.pressure = 0;
    
    return TRUE;
}

void TermHH2410R3Input (void) 
{
    if (ts >= 0)
        close (ts);    
}

#endif /* _HH2410R3_IAL */

