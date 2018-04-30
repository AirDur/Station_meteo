/*
** $Id: element.c,v 1.2 2004/06/26 07:49:31 weiym Exp $
**
** element.c: Implementation of the window element data.
**
** Copyright (C) 2004 Feynman Software.
**
** Current maintainer: Wei Yongming.
** Create date: 2004/05/10
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

#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "element.h"

int get_window_element_data (HWND hwnd, Uint16 type, Uint16 item, Uint32* data)
{
    PMAINWIN pwnd;
    list_t* me;

    pwnd = (PMAINWIN)hwnd;
    
    if (pwnd == NULL)
        return WED_INVARG;

    if (pwnd->wed == NULL)
        return WED_NODEFINED;

    list_for_each (me, &pwnd->wed->list) {
        WND_ELEMENT_DATA* wed;
        wed = list_entry (me, WND_ELEMENT_DATA, list);
        if (wed->type == type && wed->item == item) {
            *data = wed->data;
            return WED_OK;
        }
    }

    return WED_NOTFOUND;
}

int set_window_element_data (HWND hwnd, Uint16 type, Uint16 item, Uint32 new_data, Uint32* old_data)
{
    PMAINWIN pwnd;
    WND_ELEMENT_DATA* new_wed;
    list_t* me;

    pwnd = (PMAINWIN)hwnd;
    
    if (pwnd == NULL)
        return WED_INVARG;

    if (pwnd->wed == NULL) {
        pwnd->wed = malloc (sizeof (WND_ELEMENT_DATA));
        memset(pwnd->wed,0,sizeof (WND_ELEMENT_DATA));
        
        if (pwnd->wed == NULL)
            return WED_MEMERR;

        INIT_LIST_HEAD (&pwnd->wed->list);

        new_wed = malloc (sizeof (WND_ELEMENT_DATA));
        memset(new_wed,0,sizeof (WND_ELEMENT_DATA));
        if (new_wed == NULL)
            return WED_MEMERR;

        new_wed->type = type;
        new_wed->item = item;
        new_wed->data = new_data;
        list_add_tail (&new_wed->list, &pwnd->wed->list);
        return WED_NEW_DATA;
    }
    else {
        list_for_each (me, &pwnd->wed->list) {
            WND_ELEMENT_DATA* wed;
            wed = list_entry (me, WND_ELEMENT_DATA, list);
            if (wed->type == type && wed->item == item) {
                *old_data = wed->data;
                if (new_data == *old_data) {
                    return WED_NOT_CHANGED;
                }
                else {
                    wed->data = new_data;
                    return WED_OK;
                }
            }
        }

        new_wed = malloc (sizeof (WND_ELEMENT_DATA));
        if (new_wed == NULL)
            return WED_MEMERR;
        memset(new_wed,0,sizeof (WND_ELEMENT_DATA));

        new_wed->type = type;
        new_wed->item = item;
        new_wed->data = new_data;
        list_add_tail (&new_wed->list, &pwnd->wed->list);
        return WED_NEW_DATA;
    }
}

int free_window_element_data (HWND hwnd)
{
    PMAINWIN pwnd;
    WND_ELEMENT_DATA* wed;

    pwnd = (PMAINWIN)hwnd;
    
    if (pwnd == NULL)
        return WED_INVARG;

    if (pwnd->wed == NULL)
        return WED_OK;

    while (!list_empty (&pwnd->wed->list)) {
        wed = list_entry (pwnd->wed->list.next, WND_ELEMENT_DATA, list);
        list_del (&wed->list);
        free (wed);
    }
    
    free (pwnd->wed);
    pwnd->wed = NULL;
    return WED_OK;
}

