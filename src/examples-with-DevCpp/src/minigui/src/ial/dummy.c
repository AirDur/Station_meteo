/*
** $Id: dummy.c,v 1.12 2005/01/31 10:54:09 clear Exp $
**
** dummy.c: The dummy IAL engine.
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Created by Wei Yongming, 2001/09/13
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
#include "api_timers.h"

#ifdef _DUMMY_IAL

#include "gdi.h"
#include "misc.h"
#include "ial.h"
#include "dummy.h"

static int mouse_x, mouse_y, mouse_button;
static int old_mouse_x, old_mouse_y, old_mouse_button;
static int mouse_dep_x, mouse_dep_y;
static unsigned char old_kb;

typedef struct tagPOS
{
	short x;
	short y;
	short b;
} POS;

/************************  Low Level Input Operations **********************/
/*
 * Mouse operations -- Event
 */
static int mouse_update(void)
{
	return 1;
}

static void mouse_getxy (int* x, int* y)
{           
	*x = mouse_x;
    *y = mouse_y;
}

static int mouse_getbutton(void)
{
	return mouse_button;
}

static int keyboard_update(void)
{
	return 0;
}

static const char * keyboard_get_state (void)
{
	return NULL;
}

#ifdef _LITE_VERSION 
static int wait_event (int which)
{
   unsigned char kb;
   
//   uHALr_printf("IAL: Wait Event\n");
   
   if ( which & IAL_MOUSEEVENT )
      {
        if(TIMER_GET_VALUE(ID_TIMER3)>9600)
           return 0;
        TIMER_CHANGE_CPT(ID_TIMER3,10000);
        TIMER_CHANGE_MODE(ID_TIMER3,MODE_UPDATE);
        TIMER_CHANGE_MODE(ID_TIMER3,MODE_RELOAD + MODE_RUN);
                
        kb = KEYBOARD_STATUS();

        if(old_kb == kb)
           {
           if(kb & 0x6)
              {
              if(kb & 0x2)
                 {
                 mouse_x += mouse_dep_x;
                 if(mouse_x > GetGDCapability (HDC_SCREEN, GDCAP_MAXX))
                    mouse_x = GetGDCapability (HDC_SCREEN, GDCAP_MAXX);
                 if(mouse_x < 0)
                    mouse_x = 0;
                 }

              if(kb & 0x4)
                 {
                 mouse_y += mouse_dep_y;
                 if(mouse_y > GetGDCapability (HDC_SCREEN, GDCAP_MAXY))
                    mouse_y = GetGDCapability (HDC_SCREEN, GDCAP_MAXY);
                 if(mouse_y < 0)
                    mouse_y = 0;
                 }

              return IAL_MOUSEEVENT;
              }          
           else
              return 0;
           }
           else
           {
           old_kb =kb;
           if(kb & 0x6)
              {
              if(kb & 0x2)
                 mouse_dep_x = - mouse_dep_x;
              if(kb & 0x4)
                 mouse_dep_y = - mouse_dep_y;
              }
           if(kb & 0x8)
              {
              if(mouse_button == 0)
                 mouse_button = IAL_MOUSE_LEFTBUTTON;
              }
              else
              {
              if(mouse_button == IAL_MOUSE_LEFTBUTTON)
                 mouse_button = 0;
              }
           return IAL_MOUSEEVENT;
           }
      }
}
#else

static int wait_event (int which, fd_set *in, fd_set *out, fd_set *except,
                struct timeval *timeout)
{
    __mg_os_time_delay (300);
    return 0;
}
#endif

BOOL InitDummyInput (INPUT* input, const char* mdev, const char* mtype)
{
    input->update_mouse = mouse_update;
    input->get_mouse_xy = mouse_getxy;
    input->set_mouse_xy = NULL;
    input->get_mouse_button = mouse_getbutton;
    input->set_mouse_range = NULL;

    input->update_keyboard = keyboard_update;
    input->get_keyboard_state = keyboard_get_state;
    input->set_leds = NULL;

    input->wait_event = wait_event;
	mouse_x = 100;
	mouse_y = 100;
	mouse_button= 0;
	old_mouse_x = 100;
	old_mouse_y = 100;
	old_mouse_button= 0;
	old_kb = 0;
    mouse_dep_x = 2;
    mouse_dep_y = 2;
    TIMER_SET(ID_TIMER3,MODE_UPDATE,100,5,10000,NULL);
    TIMER_CHANGE_MODE(ID_TIMER3,MODE_RELOAD + MODE_RUN);
    return TRUE;
}

void TermDummyInput (void)
{
}

#endif /* _DUMMY_IAL */

