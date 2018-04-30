/*
** $Id: auto.c,v 1.8 2004/08/02 04:11:57 snig Exp $
**
** auto.c: Automatic Input Engine
** 
** Copyright (C) 2004 Feynman Software
**
** Created by Wei Yongming, 2004/01/29
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

#include "common.h"

#ifdef _AUTO_IAL

#include "minigui.h"
#include "misc.h"
#include "ial.h"

#define IDF_MOUSE       0x1000

#define IDF_MLDOWN      IAL_MOUSE_LEFTBUTTON
#define IDF_MRDOWN      IAL_MOUSE_RIGHTBUTTON
#define IDF_MMDOWN      IAL_MOUSE_MIDDLEBUTTON

#define IDF_KEY         0x2000
#define IDF_KEYDOWN     0x0100

static struct _IN_DATA
{
    unsigned short flags;
    unsigned short mouse_x;
    unsigned short mouse_y;
    unsigned short key_code;
} in_data [] = 
{
    {IDF_MOUSE,                 160, 160, 0},
    {IDF_MOUSE | IDF_MLDOWN,    160, 160, 0},
    {IDF_MOUSE,                 160, 160, 0},
    {IDF_MOUSE,                 0, 0, 0},
    {IDF_KEY | IDF_KEYDOWN,     0, 0, SCANCODE_A},
    {IDF_KEY,                   0, 0, SCANCODE_A},
    {IDF_KEY | IDF_KEYDOWN,     0, 0, SCANCODE_B},
    {IDF_KEY,                   0, 0, SCANCODE_B},
    {IDF_KEY | IDF_KEYDOWN,     0, 0, SCANCODE_C},
    {IDF_KEY,                   0, 0, SCANCODE_C},
};

static int cur_index;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update (void)
{
    return 1;
}

static void mouse_getxy (int *x, int* y)
{
    *x = in_data [cur_index].mouse_x;
    *y = in_data [cur_index].mouse_y;
}

static int mouse_getbutton (void)
{
    int buttons = 0;

    if (in_data [cur_index].flags & IDF_MLDOWN)
        buttons |= IAL_MOUSE_LEFTBUTTON;
    if (in_data [cur_index].flags & IDF_MRDOWN)
        buttons |= IAL_MOUSE_RIGHTBUTTON;
    if (in_data [cur_index].flags & IDF_MMDOWN)
        buttons |= IAL_MOUSE_MIDDLEBUTTON;

    return buttons;
}

static unsigned char kbd_state [NR_KEYS];

static int keyboard_update (void)
{
    if (in_data [cur_index].flags & IDF_KEY) {
        if (in_data [cur_index].flags & IDF_KEYDOWN)
            kbd_state [in_data [cur_index].key_code] = 1;
        else
            kbd_state [in_data [cur_index].key_code] = 0;
    }

    return in_data [cur_index].key_code + 1;
}

static const char* keyboard_getstate (void)
{
    return kbd_state;
}

#ifdef _LITE_VERSION
static int wait_event (int which, int maxfd, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    fd_set rfds;
    int    retvalue;
    int    e;
    static int index = 0;

    if (!in) {
        in = &rfds;
        FD_ZERO (in);
    }

    e = select (maxfd + 1, in, out, except, timeout) ;

    if (in_data [index].flags & IDF_MOUSE) {
        retvalue = IAL_MOUSEEVENT;
    }
    else {
        retvalue = IAL_KEYEVENT;
    }

    cur_index = index;
    index ++;
    if (index == TABLESIZE (in_data))
        index = 0;

    return retvalue;
}

#else

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    int    retvalue;
    static int index = 0;
    static int count = 0;

    __mg_os_time_delay (300);

    if (in_data [index].flags & IDF_MOUSE) {
        retvalue = IAL_MOUSEEVENT;
    }
    else {
        retvalue = IAL_KEYEVENT;
    }

    cur_index = index;
    index ++;
    if (index == TABLESIZE (in_data))
        index = 0;

    count ++;
    if (count == 100) {
        printf ("auto engine : %d\n", retvalue);
        count = 0;
    }
    return retvalue;
}

#endif

BOOL InitAutoInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;
    input->suspend_mouse= NULL;
    input->resume_mouse = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_getstate;
    input->suspend_keyboard = NULL;
    input->resume_keyboard = NULL;
    input->set_leds = NULL;

    input->wait_event = wait_event;

    return TRUE;
}

void TermAutoInput (void)
{
}

#endif /* _AUTO_IAL */

