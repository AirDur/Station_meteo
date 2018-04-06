/*
** $Id: newtoolbar.c,v 1.20 2005/01/31 09:12:48 clear Exp $
**
** newtoolbar.c: the new-toolbar control module.
**
** This control is a substitute of old ToolBar control.
** New application should use the NewToolBar control
** instead of ToolBar control.
**
** Copyright (C) 2003 Feynman Software.
** 
** Authors:
**      Wei Yongming (ymwei@minigui.org)
**      Peng Yuan
**
** Create date: 2003/04/24
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
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/newtoolbar.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _CTRL_NEWTOOLBAR

#include "ctrlmisc.h"
#include "newtoolbar_impl.h"

static int NewToolbarCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterNewToolbarControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_NEWTOOLBAR;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = NewToolbarCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}


static void line_a2b (HDC hdc, POINT* a, POINT* b, int color)
{
    SetPenColor (hdc, color);
    MoveTo (hdc, a->x, a->y);
    LineTo (hdc, b->x, b->y);
}

static void draw_tool_bar_vert (HWND hwnd, HDC hdc, PNTBCTRLDATA ntb_data)
{
    int         l, t, r, b, w, h;
    RECT        rc;
    POINT       pta, ptb;
    NTBITEM*    item = NULL;

    GetClientRect (hwnd, &rc);

    /* draw right and left line */
#ifdef _FLAT_WINDOW_STYLE
    pta.x = 0; pta.y = 0;
    ptb.x = 0; ptb.y = rc.bottom;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    pta.x = rc.right - 1 ; pta.y = 0;
    ptb.x = rc.right - 1; ptb.y = rc.bottom;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    /* draw two separator lines */
    pta.y = 2; pta.x = MARGIN_V_HORZ;
    ptb.y = pta.y; ptb.x = rc.right - MARGIN_V_HORZ - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    pta.y = 5; pta.x = MARGIN_V_HORZ;
    ptb.y = pta.y; ptb.x = rc.right - MARGIN_V_HORZ - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    /* draw separator line between bitmap and text */
/*    if ((ntb_data->style & NTBS_WITHTEXT) && !(ntb_data->style & NTBS_TEXTRIGHT)) {
        pta.x = MARGIN_HORZ; pta.y = rc.bottom - GetFontHeight (hdc) - GAP_BMP_TEXT_VERT;
        ptb.x = rc.right; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_black);
    }*/
#else
    rc.bottom -= 1;
    rc.right -= 1;

    pta.x = 0; pta.y = 0;
    ptb.x = 0; ptb.y = rc.bottom;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.x += 1; ptb.x = pta.x;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    pta.x = rc.right - 1 ; pta.y = 0;
    ptb.x = rc.right - 1 ; ptb.y = rc.bottom;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.x += 1; ptb.x = pta.x;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    /* draw two separator lines */
    pta.y = 2; pta.x = MARGIN_V_HORZ;
    ptb.y = pta.y; ptb.x = rc.right - MARGIN_V_HORZ - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.y += 1; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    pta.y = 5; pta.x = MARGIN_V_HORZ;
    ptb.y = pta.y; ptb.x = rc.right - MARGIN_V_HORZ - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.y += 1; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    /* draw separator line between bitmap and text */
/*    if ((ntb_data->style & NTBS_WITHTEXT) && !(ntb_data->style & NTBS_TEXTRIGHT)) {
        pta.x = MARGIN_HORZ; pta.y = rc.bottom - GetFontHeight (hdc) - GAP_BMP_TEXT_VERT;
        ptb.x = rc.right; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
        pta.y += 1; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
    }*/
#endif /* _FLAT_WINDOW_STYLE */

    item = ntb_data->head;
    while (item) {
        if ( (item->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
            item = item->next;

        l = item->rc_item.left;
        t = item->rc_item.top;
        r = item->rc_item.right;
        b = item->rc_item.bottom;
        w = ntb_data->w_cell;
        h = ntb_data->h_cell;

        if ((item->flags & NTBIF_TYPEMASK) == NTBIF_SEPARATOR) {
            if (ntb_data->style & NTBS_DRAWSEPARATOR) {
#ifdef _FLAT_WINDOW_STYLE
                pta.y = l ;
                pta.x = t + 2;
                ptb.y = pta.y;
                ptb.x = rc.right - MARGIN_V_HORZ;
                line_a2b (hdc, &pta, &ptb, COLOR_black);
#else
                pta.y = l ;
                pta.x = t + 1;
                ptb.y = pta.y;
                ptb.x = rc.left - MARGIN_V_HORZ;
                line_a2b (hdc, &pta, &ptb, COLOR_darkgray);

                pta.y += 1;
                pta.x = t + 1;
                ptb.y = pta.y;
                ptb.x = rc.left - MARGIN_V_HORZ;
                line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
#endif
            }

            item = item->next;
            continue;
        }

        if (item->flags & NTBIF_DISABLED) {
                if (ntb_data->nr_cols == 4)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                w * 3, h * item->bmp_cell);
                else
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                0, h * item->bmp_cell);
        }
        else if ( item->flags & NTBIF_CHECKED) {
                if (ntb_data->nr_cols >= 3)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w * 2, h * item->bmp_cell);
                else if ( ntb_data->nr_cols >=2 )
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                w, h * item->bmp_cell);
                else
                {
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,                     
                                0, h * item->bmp_cell);
                    r = l + w ;
                    b = t + h ;
                    
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                }
            
        }
        else {
            if (item == ntb_data->sel_item) {
                if (ntb_data->btn_down && ntb_data->nr_cols >= 3)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w * 2, h * item->bmp_cell);
                else if (!ntb_data->btn_down && ntb_data->nr_cols >= 2)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w, h * item->bmp_cell);
                else
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                0, h * item->bmp_cell);
            }
            else
                FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    0, h * item->bmp_cell);
        }

        if (ntb_data->style & NTBS_DRAWSTATES) {

            if ((ntb_data->style & NTBS_WITHTEXT) 
                            && !(ntb_data->style & NTBS_TEXTRIGHT)) {
                r = l + w;
                b = t + h;
            }
            else {
                r--; b--;
            }

            if (!(item->flags & NTBIF_DISABLED) && item == ntb_data->sel_item) {
                if (ntb_data->btn_down) {
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                }
                else {
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                }
            }
        }

        if (ntb_data->style & NTBS_WITHTEXT) {
            DWORD format = DT_SINGLELINE | DT_VCENTER;

            if (ntb_data->style & NTBS_TEXTRIGHT)
                format |= DT_LEFT;
            else
                format |= DT_CENTER;

            SetBkMode (hdc, BM_TRANSPARENT);
            DrawText (hdc, item->text, -1, &item->rc_text, format);
        }

        item = item->next;
    }
}
static void draw_tool_bar_horz (HWND hwnd, HDC hdc, PNTBCTRLDATA ntb_data)
{
    int         l, t, r, b, w, h;
    RECT        rc;
    POINT       pta, ptb;
    NTBITEM*    item = NULL;

    GetClientRect (hwnd, &rc);

    /* draw top and bottom line */
#ifdef _FLAT_WINDOW_STYLE
    pta.x = 0; pta.y = 0;
    ptb.x = rc.right; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    pta.x = 0; pta.y = rc.bottom - 1;
    ptb.x = rc.right; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    /* draw two separator lines */
    pta.x = 2; pta.y = MARGIN_VERT;
    ptb.x = pta.x; ptb.y = rc.bottom - MARGIN_VERT - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    pta.x = 5; pta.y = MARGIN_VERT;
    ptb.x = pta.x; ptb.y = rc.bottom - MARGIN_VERT - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_black);

    /* draw separator line between bitmap and text */
/*    if ((ntb_data->style & NTBS_WITHTEXT) && !(ntb_data->style & NTBS_TEXTRIGHT)) {
        pta.x = MARGIN_HORZ; pta.y = rc.bottom - GetFontHeight (hdc) - GAP_BMP_TEXT_VERT;
        ptb.x = rc.right; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_black);
    }*/
#else
    rc.bottom -= 1;
    rc.right -= 1;

    pta.x = 0; pta.y = 0;
    ptb.x = rc.right; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.y += 1; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    pta.x = 0; pta.y = rc.bottom - 1;
    ptb.x = rc.right; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.y += 1; ptb.y = pta.y;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    /* draw two separator lines */
    pta.x = 2; pta.y = MARGIN_VERT;
    ptb.x = pta.x; ptb.y = rc.bottom - MARGIN_VERT - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.x += 1; ptb.x = pta.x;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    pta.x = 5; pta.y = MARGIN_VERT;
    ptb.x = pta.x; ptb.y = rc.bottom - MARGIN_VERT - 1;
    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
    pta.x += 1; ptb.x = pta.x;
    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

    /* draw separator line between bitmap and text */
/*    if ((ntb_data->style & NTBS_WITHTEXT) && !(ntb_data->style & NTBS_TEXTRIGHT)) {
        pta.x = MARGIN_HORZ; pta.y = rc.bottom - GetFontHeight (hdc) - GAP_BMP_TEXT_VERT;
        ptb.x = rc.right; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
        pta.y += 1; ptb.y = pta.y;
        line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
    } */
#endif /* _FLAT_WINDOW_STYLE */
    
    
    item = ntb_data->head;
    while (item) {
        if ( (item->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
            item = item->next;
        l = item->rc_item.left;
        t = item->rc_item.top;
        r = item->rc_item.right;
        b = item->rc_item.bottom;
        w = ntb_data->w_cell;
        h = ntb_data->h_cell;

        if ((item->flags & NTBIF_TYPEMASK) == NTBIF_SEPARATOR) {
            if (ntb_data->style & NTBS_DRAWSEPARATOR) {
#ifdef _FLAT_WINDOW_STYLE
                pta.x = l + 2;
                pta.y = t;
                ptb.x = pta.x;
                ptb.y = rc.bottom - MARGIN_VERT;
                line_a2b (hdc, &pta, &ptb, COLOR_black);
#else
                pta.x = l + 1;
                pta.y = t;
                ptb.x = pta.x;
                ptb.y = rc.bottom - MARGIN_VERT;
                line_a2b (hdc, &pta, &ptb, COLOR_darkgray);

                pta.x += 1;
                pta.y = t;
                ptb.x = pta.x;
                ptb.y = rc.bottom - MARGIN_VERT;
                line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
#endif
            }

            item = item->next;
            continue;
        }

        if (item->flags & NTBIF_DISABLED) {
                if (ntb_data->nr_cols == 4)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                w * 3, h * item->bmp_cell);
                else
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                0, h * item->bmp_cell);
        }
        else if ( item->flags & NTBIF_CHECKED) {
                if (ntb_data->nr_cols >= 3)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w * 2, h * item->bmp_cell);
                else if ( ntb_data->nr_cols >=2 )
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                w, h * item->bmp_cell);
                else
                {
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image, 
                                0, h * item->bmp_cell);
                    r = l + w ;
                    b = t + h ;
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_black);
                }
            
        }
        else {
            if (item == ntb_data->sel_item) {
                if (ntb_data->btn_down && ntb_data->nr_cols >= 3)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w * 2, h * item->bmp_cell);
                else if (!ntb_data->btn_down && ntb_data->nr_cols >= 2)
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    w, h * item->bmp_cell);
                else
                    FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                0, h * item->bmp_cell);
            }
            else
                FillBoxWithBitmapPart (hdc, l, t, w, h, 0, 0, ntb_data->image,
                                    0, h * item->bmp_cell);
        }
        
        if (ntb_data->style & NTBS_DRAWSTATES) {

            if ((ntb_data->style & NTBS_WITHTEXT) 
                            && !(ntb_data->style & NTBS_TEXTRIGHT)) {
                r = l + w;
                b = t + h;
            }
            else {
                r--; b--;
            }

            if (!(item->flags & NTBIF_DISABLED) && item == ntb_data->sel_item) {
                if (ntb_data->btn_down) {
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                }
                else {
                    pta.x = l; pta.y = t;
                    ptb.x = l; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);
                    pta.x = l; pta.y = t;
                    ptb.x = r; ptb.y = t;
                    line_a2b (hdc, &pta, &ptb, COLOR_lightwhite);

                    pta.x = r; pta.y = t;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                    pta.x = l; pta.y = b;
                    ptb.x = r; ptb.y = b;
                    line_a2b (hdc, &pta, &ptb, COLOR_darkgray);
                }
            }
        }

        if (ntb_data->style & NTBS_WITHTEXT) {
            DWORD format = DT_SINGLELINE | DT_VCENTER;

            if (ntb_data->style & NTBS_TEXTRIGHT)
                format |= DT_LEFT;
            else
                format |= DT_CENTER;

            SetBkMode (hdc, BM_TRANSPARENT);
            DrawText (hdc, item->text, -1, &item->rc_text, format);
        }

        item = item->next;
    }
}

static NTBITEM* get_item_by_pos (PNTBCTRLDATA ntb_data, int posx, int posy)
{
    NTBITEM* item;

    item = ntb_data->head;
    while (item) {
        if (!(item->flags & NTBIF_DISABLED) && PtInRect (&item->rc_item, posx, posy)) {
            return item;
        }

        item = item->next;
    }

    return NULL;
}

static void append_new_item (HWND hwnd, NTBCTRLDATA* ntb_data, NTBITEM* new_item)
{
    HDC hdc;

    hdc = GetClientDC (hwnd);
    if (! (ntb_data->style & NTBS_VERTICAL) )
    {
       //horizontal
        
        if (ntb_data->tail == NULL )
        {
            new_item->rc_item.top = MARGIN_VERT;
            new_item->rc_item.left = MARGIN_HORZ;
        }
        else if ( (new_item->flags & NTBIF_TYPEMASK ) == NTBIF_NEWLINE )
        {
            new_item->rc_item.top = ntb_data->tail->rc_item.bottom + GAP_ITEM_ITEM_VERT;
            new_item->rc_item.left = MARGIN_HORZ;
            new_item->rc_item.bottom = new_item->rc_item.top;
            new_item->rc_item.right = new_item->rc_item.left;
            goto  end;
        }
        else
        {
            new_item->rc_item.top = ntb_data->tail->rc_item.top;
            if ( (ntb_data->tail->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
                new_item->rc_item.left = MARGIN_HORZ;
            else
                new_item->rc_item.left = ntb_data->tail->rc_item.right + GAP_ITEM_ITEM_HORZ;
        }


        switch (new_item->flags & NTBIF_TYPEMASK) {
        case NTBIF_PUSHBUTTON:
        case NTBIF_CHECKBUTTON:
        case NTBIF_HOTSPOTBUTTON:
            if (ntb_data->style & NTBS_WITHTEXT) {
                if (ntb_data->style & NTBS_TEXTRIGHT) {

                    SIZE sz_text;
                    GetTextExtent (hdc, new_item->text, -1, &sz_text);

                    new_item->rc_item.bottom 
                            = new_item->rc_item.top + ntb_data->h_cell + 1;

                    new_item->rc_text.left = new_item->rc_item.left + ntb_data->w_cell + GAP_BMP_TEXT_HORZ;
                    new_item->rc_text.right = new_item->rc_text.left + sz_text.cx;
                    new_item->rc_text.top = new_item->rc_item.top;
                    new_item->rc_text.bottom = new_item->rc_item.bottom;

                    new_item->rc_item.right = new_item->rc_text.right + GAP_BMP_TEXT_HORZ + 1;
                }
                else {
                    new_item->rc_item.right = new_item->rc_item.left + ntb_data->w_cell + 1;

                    new_item->rc_text.left = new_item->rc_item.left;
                    new_item->rc_text.right = new_item->rc_item.right;
                    new_item->rc_text.top 
                            = new_item->rc_item.top + ntb_data->h_cell + GAP_BMP_TEXT_VERT;
                    new_item->rc_text.bottom = new_item->rc_text.top + GetFontHeight (hdc);

                    new_item->rc_item.bottom = new_item->rc_text.bottom + 1;
                }
            }
            else {
                new_item->rc_item.right = new_item->rc_item.left + ntb_data->w_cell + 1;
                new_item->rc_item.bottom = new_item->rc_item.top + ntb_data->h_cell + 1;
            }
            break;
            
        case NTBIF_NEWLINE:
        case NTBIF_SEPARATOR:
        default:
            if (ntb_data->style & NTBS_DRAWSEPARATOR)
                new_item->rc_item.right = new_item->rc_item.left + WIDTH_SEPARATOR;
            else
                new_item->rc_item.right = new_item->rc_item.left + WIDTH_SEPARATOR * 2;
            new_item->rc_item.bottom = new_item->rc_item.top + ntb_data->h_cell;
            break;
        }
    }
    else
    {
    // vertical 
        if (ntb_data->tail == NULL )
        {
            new_item->rc_item.top = MARGIN_V_VERT;
            new_item->rc_item.left = MARGIN_V_HORZ;
        }
        else if ( (new_item->flags & NTBIF_TYPEMASK ) == NTBIF_NEWLINE )
        {
            new_item->rc_item.left = ntb_data->tail->rc_item.right + GAP_ITEM_ITEM_HORZ;
            new_item->rc_item.top = MARGIN_V_VERT;
            new_item->rc_item.bottom = new_item->rc_item.top;
            new_item->rc_item.right = new_item->rc_item.left;
            goto  end;
        }
        else
        {
            if ( (ntb_data->tail->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
                new_item->rc_item.top = MARGIN_V_VERT; 
            else
                new_item->rc_item.top = ntb_data->tail->rc_item.bottom + GAP_ITEM_ITEM_VERT;
            new_item->rc_item.left = ntb_data->tail->rc_item.left;
        }
        
        switch (new_item->flags & NTBIF_TYPEMASK) {
        case NTBIF_PUSHBUTTON:
        case NTBIF_CHECKBUTTON:
        case NTBIF_HOTSPOTBUTTON:
            if (ntb_data->style & NTBS_WITHTEXT) {
                if (ntb_data->style & NTBS_TEXTRIGHT) {

                    SIZE sz_text;
                    GetTextExtent (hdc, new_item->text, -1, &sz_text);

                    new_item->rc_item.bottom 
                            = new_item->rc_item.top + ntb_data->h_cell + 1;

                    new_item->rc_text.left = new_item->rc_item.left + ntb_data->w_cell + GAP_BMP_TEXT_HORZ;
                    new_item->rc_text.right = new_item->rc_text.left + sz_text.cx;
                    new_item->rc_text.top = new_item->rc_item.top;
                    new_item->rc_text.bottom = new_item->rc_item.bottom;

                    new_item->rc_item.right = new_item->rc_text.right + GAP_BMP_TEXT_HORZ + 1;
                }
                else {
                    new_item->rc_item.right = new_item->rc_item.left + ntb_data->w_cell + 1;

                    new_item->rc_text.left = new_item->rc_item.left;
                    new_item->rc_text.right = new_item->rc_item.right;
                    new_item->rc_text.top 
                            = new_item->rc_item.top + ntb_data->h_cell + GAP_BMP_TEXT_VERT;
                    new_item->rc_text.bottom = new_item->rc_text.top + GetFontHeight (hdc);

                    new_item->rc_item.bottom = new_item->rc_text.bottom + 1;
                }
            }
            else {
                new_item->rc_item.right = new_item->rc_item.left + ntb_data->w_cell + 1;
                new_item->rc_item.bottom = new_item->rc_item.top + ntb_data->h_cell + 1;
            }
            break;
                        
        case NTBIF_SEPARATOR:
        default:
            if (ntb_data->style & NTBS_DRAWSEPARATOR)
                new_item->rc_item.bottom = new_item->rc_item.top + WIDTH_SEPARATOR;
            else
                new_item->rc_item.bottom = new_item->rc_item.top + WIDTH_SEPARATOR * 2;
            new_item->rc_item.right = new_item->rc_item.left + ntb_data->w_cell;
            break;
        }
    }

end:
    ReleaseDC (hdc);

    new_item->next = NULL;
    if (ntb_data->tail == NULL) {
        ntb_data->head = ntb_data->tail = new_item;
    }
    else {
        ntb_data->tail->next = new_item;
        ntb_data->tail = new_item;
    }
    ntb_data->nr_items++;
}

static void recalc_items (HWND hwnd, NTBCTRLDATA* ntb_data)
{
    HDC hdc;
    PNTBITEM item, prev = NULL;

    hdc = GetClientDC (hwnd);

    item = ntb_data->head;

    if(!( ntb_data->style & NTBS_VERTICAL ))
    {
        while (item) {
           //horizontal
            
            if (prev == NULL )
            {
                item->rc_item.top = MARGIN_VERT;
                item->rc_item.left = MARGIN_HORZ;
            }
            else if ( (item->flags & NTBIF_TYPEMASK ) == NTBIF_NEWLINE )
            {
                item->rc_item.top =  prev->rc_item.bottom + GAP_ITEM_ITEM_VERT;
                item->rc_item.left = MARGIN_HORZ;
                item->rc_item.bottom = item->rc_item.top;
                item->rc_item.right = item->rc_item.left;
                goto  end;
            }
            else
            {
                if ( (prev->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
                    item->rc_item.left = MARGIN_HORZ;
                else
                    item->rc_item.left = prev->rc_item.right + GAP_ITEM_ITEM_HORZ;
                item->rc_item.top =  prev->rc_item.top;
            }


            switch (item->flags & NTBIF_TYPEMASK) {
            case NTBIF_PUSHBUTTON:
            case NTBIF_CHECKBUTTON:
            case NTBIF_HOTSPOTBUTTON:
            if (ntb_data->style & NTBS_WITHTEXT) {
                if (ntb_data->style & NTBS_TEXTRIGHT) {

                    SIZE sz_text;
                    GetTextExtent (hdc, item->text, -1, &sz_text);

                    item->rc_item.bottom 
                            = item->rc_item.top + ntb_data->h_cell + 1;

                    item->rc_text.left = item->rc_item.left + ntb_data->w_cell + GAP_BMP_TEXT_HORZ + 1;
                    item->rc_text.right = item->rc_text.left + sz_text.cx;
                    item->rc_text.top = item->rc_item.top;
                    item->rc_text.bottom = item->rc_item.bottom;

                    item->rc_item.right = item->rc_text.right + GAP_BMP_TEXT_HORZ + 1;
                }
                else {
                    item->rc_item.right = item->rc_item.left + ntb_data->w_cell + 1;

                    item->rc_text.left = item->rc_item.left;
                    item->rc_text.right = item->rc_item.right;
                    item->rc_text.top 
                            = item->rc_item.top + ntb_data->h_cell + GAP_BMP_TEXT_VERT;
                    item->rc_text.bottom = item->rc_text.top + GetFontHeight (hdc);

                    item->rc_item.bottom = item->rc_text.bottom + 1;
                }
            }
            else {
                item->rc_item.right = item->rc_item.left + ntb_data->w_cell + 1;
                item->rc_item.bottom = item->rc_item.top + ntb_data->h_cell + 1;
            }
            break;
                        
            case NTBIF_SEPARATOR:
            default:
                if (ntb_data->style & NTBS_DRAWSEPARATOR)
                    item->rc_item.right = item->rc_item.left + WIDTH_SEPARATOR;
                else
                    item->rc_item.right = item->rc_item.left + WIDTH_SEPARATOR * 2;
                item->rc_item.bottom = item->rc_item.top + ntb_data->h_cell;
                break;
            }

            prev = item;
            item = item->next;
        }
    }
    else
    {
     //vertical
        while (item) {
            if (prev == NULL )
            {
                item->rc_item.top = MARGIN_V_VERT;
                item->rc_item.left = MARGIN_V_HORZ;
            }
            else if ( (item->flags & NTBIF_TYPEMASK ) == NTBIF_NEWLINE )
            {
                item->rc_item.left = prev->rc_item.right + GAP_ITEM_ITEM_HORZ;
                item->rc_item.top = MARGIN_V_VERT;
                item->rc_item.bottom = item->rc_item.top;
                item->rc_item.right = item->rc_item.left;
                goto  end;
            }
            else
            {
                if ( (prev->flags & NTBIF_TYPEMASK) == NTBIF_NEWLINE )
                    item->rc_item.top = MARGIN_V_VERT; 
                else
                    item->rc_item.top = prev->rc_item.bottom + GAP_ITEM_ITEM_VERT;
                item->rc_item.left = prev->rc_item.left;
            }

            switch (item->flags & NTBIF_TYPEMASK) {
            case NTBIF_PUSHBUTTON:
            case NTBIF_CHECKBUTTON:
            case NTBIF_HOTSPOTBUTTON:
            if (ntb_data->style & NTBS_WITHTEXT) {
                if (ntb_data->style & NTBS_TEXTRIGHT) {

                    SIZE sz_text;
                    GetTextExtent (hdc, item->text, -1, &sz_text);

                    item->rc_item.bottom 
                            = item->rc_item.top + ntb_data->h_cell + 1;

                    item->rc_text.left = item->rc_item.left + ntb_data->w_cell + GAP_BMP_TEXT_HORZ + 1;
                    item->rc_text.right = item->rc_text.left + sz_text.cx;
                    item->rc_text.top = item->rc_item.top;
                    item->rc_text.bottom = item->rc_item.bottom;

                    item->rc_item.right = item->rc_text.right + GAP_BMP_TEXT_HORZ + 1;
                }
                else {
                    item->rc_item.right = item->rc_item.left + ntb_data->w_cell + 1;

                    item->rc_text.left = item->rc_item.left;
                    item->rc_text.right = item->rc_item.right;
                    item->rc_text.top 
                            = item->rc_item.top + ntb_data->h_cell + GAP_BMP_TEXT_VERT;
                    item->rc_text.bottom = item->rc_text.top + GetFontHeight (hdc);

                    item->rc_item.bottom = item->rc_text.bottom + 1;
                }
            }
            else {
                item->rc_item.right = item->rc_item.left + ntb_data->w_cell + 1;
                item->rc_item.bottom = item->rc_item.top + ntb_data->h_cell + 1;
            }
            break;
                        
            case NTBIF_SEPARATOR:
            default:
                if (ntb_data->style & NTBS_DRAWSEPARATOR)
                    item->rc_item.bottom = item->rc_item.top + WIDTH_SEPARATOR;
                else
                    item->rc_item.bottom = item->rc_item.top + WIDTH_SEPARATOR * 2;
                item->rc_item.right = item->rc_item.left + ntb_data->w_cell;
                break;
            }

            prev = item;
            item = item->next;
        }
    }
end:

    ReleaseDC (hdc);
}

/*===========================================================================*/
static int NewToolbarCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL myself;
    PNTBCTRLDATA ntb_data;

    myself   = Control (hwnd);
    ntb_data = (PNTBCTRLDATA) myself->dwAddData2;

    switch (message) {
        case MSG_CREATE:
        {
            NTBINFO* data = (NTBINFO*)myself->dwAddData;

            ntb_data = (NTBCTRLDATA*) malloc (sizeof (NTBCTRLDATA));
            if (ntb_data == NULL)
                        return -1;
            memset(ntb_data,0,sizeof (NTBCTRLDATA));

            ntb_data->head = NULL;
            ntb_data->style = myself->dwStyle;

            ntb_data->image = data->image;
            ntb_data->nr_cells = data->nr_cells;
            ntb_data->nr_cols = data->nr_cols;
            if (data->nr_cols == 0)
                ntb_data->nr_cols = 4;

            ntb_data->w_cell = data->w_cell;
            if (data->w_cell == 0)
                ntb_data->w_cell = data->image->bmWidth / ntb_data->nr_cols;

            ntb_data->h_cell = data->h_cell;
            if (data->h_cell == 0)
                ntb_data->h_cell = data->image->bmHeight / ntb_data->nr_cells;

            ntb_data->nr_items = 0;
            ntb_data->sel_item = NULL;

            myself->dwAddData2 = (DWORD)ntb_data;
            break;
        }

        case MSG_DESTROY:
        {
            NTBITEM *item, *tmp;

            item = ntb_data->head;
            while (item) {
                tmp = item->next;
                free (item);
                item = tmp;
             }

            free (ntb_data);
            break;
        }

        case MSG_FONTCHANGED:
        {
            RECT rc;

            recalc_items (hwnd, ntb_data);

            GetWindowRect (hwnd, &rc);
            MoveWindow (hwnd, rc.left, rc.top, RECTW(rc), RECTH(rc), TRUE);
            break;
        }

        case MSG_NCPAINT:
            return 0;

        case MSG_SIZECHANGING:
        {
            const RECT* rcExpect = (const RECT*)wParam;
            RECT* rcResult = (RECT*)lParam;


            if ( ! (ntb_data->style & NTBS_VERTICAL ) )
            {
               // as  horizontal
                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->right = rcExpect->right;
                if (ntb_data->style & NTBS_MULTLINE )
                {
                    rcResult->bottom = rcExpect->bottom;
                }
                else if (ntb_data->style & NTBS_WITHTEXT) {
                    rcResult->bottom = rcExpect->top + ntb_data->h_cell + MARGIN_VERT * 2;
                    if (!(ntb_data->style & NTBS_TEXTRIGHT))
                        rcResult->bottom += GetWindowFont (hwnd)->size + GAP_BMP_TEXT_VERT;
                }
                else
                    rcResult->bottom = rcExpect->top + ntb_data->h_cell + MARGIN_VERT * 2;
            }
            else 
            {
                // as vertical ;
                rcResult->left = rcExpect->left;
                rcResult->top = rcExpect->top;
                rcResult->bottom = rcExpect->bottom;
                if (ntb_data->style & NTBS_WITHTEXT || ntb_data->style & NTBS_MULTLINE )  {
                    rcResult->right = rcExpect->right;
                }
                else
                    rcResult->right = rcExpect->left + ntb_data->w_cell + MARGIN_V_HORZ * 2;
            }

            return 0;
        }

        case NTBM_ADDITEM:
        {
            NTBITEMINFO* item_info = NULL;
            NTBITEM* new_item;

            item_info = (NTBITEMINFO*) lParam;

            if (!(new_item = (NTBITEM*)malloc (sizeof (NTBITEM))))
                return NTB_ERR;
            memset(new_item,0,sizeof (NTBITEM));

            new_item->flags = item_info->flags;
            new_item->id = item_info->id;
            new_item->bmp_cell = item_info->bmp_cell;

            if (item_info->text)
                strncpy (new_item->text, item_info->text, NTB_TEXT_LEN);
            else
                new_item->text [0] = '\0';

            if (item_info->tip)
                strncpy (new_item->tip, item_info->tip, NTB_TIP_LEN);
            else
                new_item->tip [0] = '\0';

            new_item->rc_hotspot = item_info->rc_hotspot;
            new_item->hotspot_proc = item_info->hotspot_proc;

            new_item->add_data = item_info->add_data;

            append_new_item (hwnd, ntb_data, new_item);
            InvalidateRect (hwnd, &new_item->rc_item, TRUE);
            return 0;
        }
        break;

        case NTBM_GETITEM:
        {
            int id = wParam;
            PNTBITEMINFO item_info = (PNTBITEMINFO) lParam;
            NTBITEM* item = NULL;

            if (!item_info)
                return NTB_ERR;

            item = ntb_data->head;

            while (item) {
                if (id == item->id) {
                    if (item_info->which & MTB_WHICH_FLAGS)
                        item_info->flags = item->flags;

                    if (item_info->which & MTB_WHICH_ID)
                        item_info->id = item->id;

                    if (item_info->which & MTB_WHICH_CELL)
                        item_info->bmp_cell = item->bmp_cell;

                    if (item_info->which & MTB_WHICH_HOTSPOT) {
                        item_info->hotspot_proc = item->hotspot_proc;
                        item_info->rc_hotspot = item->rc_hotspot;
                    }

                    if (item_info->which & MTB_WHICH_ADDDATA)
                        item_info->add_data = item->add_data;

                    if (item_info->which & MTB_WHICH_TEXT)
                        strncpy (item_info->text, item->text, NTB_TEXT_LEN);

                    if (item_info->which & MTB_WHICH_TIP)
                        strncpy (item_info->tip, item->tip, NTB_TIP_LEN);

                    return NTB_OKAY;
                }
                item = item->next;
            }

            return NTB_ERR;
        }

        case NTBM_SETITEM:
        {
            int id = wParam;
            PNTBITEMINFO item_info = (PNTBITEMINFO) lParam;
            NTBITEM* item = NULL;

            if (!item_info)
                return NTB_ERR;

            item = ntb_data->head;
            while (item) {
                if (id == item->id) {

                    if (item_info->which & MTB_WHICH_FLAGS) {
                        item->flags = item_info->flags;
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                    }

                    if (item_info->which & MTB_WHICH_ID)
                        item->id = item_info->id;

                    if (item_info->which & MTB_WHICH_CELL) {
                        item->bmp_cell = item_info->bmp_cell;
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                    }

                    if (item_info->which & MTB_WHICH_HOTSPOT) {
                        item->hotspot_proc = item_info->hotspot_proc;
                        item->rc_hotspot = item_info->rc_hotspot;
                    }

                    if (item_info->which & MTB_WHICH_ADDDATA)
                        item->add_data = item_info->add_data;

                    if (item_info->which & MTB_WHICH_TEXT) {
                        strncpy (item->text, item_info->text, NTB_TEXT_LEN);
                        if (ntb_data->style & NTBS_WITHTEXT) {
                            if (ntb_data->style & NTBS_TEXTRIGHT) {
                                recalc_items (hwnd, ntb_data);
                                InvalidateRect (hwnd, NULL, TRUE);
                            }
                            else {
                                InvalidateRect (hwnd, &item->rc_text, TRUE);
                            }
                        }
                    }

                    if (item_info->which & MTB_WHICH_TIP)
                        strncpy (item->tip, item_info->tip, NTB_TIP_LEN);

                    return NTB_OKAY;
                }
                item = item->next;
            }

            return NTB_ERR;
        }

        case NTBM_ENABLEITEM:
        {
            int id = wParam;
            BOOL enable = lParam;
            PNTBITEM item = NULL;

            item = ntb_data->head;
            while (item) {
                if (id == item->id) {
                    if (enable && (item->flags & NTBIF_DISABLED)) {
                        item->flags &= ~NTBIF_DISABLED;
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                    }
                    else if (!enable && !(item->flags & NTBIF_DISABLED)) {
                        item->flags |= NTBIF_DISABLED;
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                    }

                    return NTB_OKAY;
                }

                item = item->next;
            }

            return NTB_ERR;
        }
        
        case NTBM_SETBITMAP:
        {
            NTBINFO *ntb_newdata = NULL;

            ntb_newdata = (NTBINFO *) lParam;
            if (!ntb_newdata)
                return NTB_ERR;

            ntb_data->image = ntb_newdata->image;
            ntb_data->nr_cells =ntb_newdata->nr_cells;
            ntb_data->nr_cols = ntb_newdata->nr_cols;
            if (ntb_newdata->nr_cols == 0)
                ntb_data->nr_cols = 4;

            ntb_data->w_cell = ntb_newdata->w_cell;
            if (ntb_data->w_cell == 0)
                ntb_data->w_cell = ntb_newdata->image->bmWidth / ntb_data->nr_cols;

            ntb_data->h_cell = ntb_newdata->h_cell;
            if (ntb_newdata->h_cell == 0)
                ntb_data->h_cell = ntb_newdata->image->bmHeight / ntb_data->nr_cells;

            InvalidateRect(hwnd, NULL, FALSE);
            
            return NTB_OKAY;
        }

        case MSG_PAINT:
        {
            HDC hdc = BeginPaint (hwnd);
            if ( ! (ntb_data->style & NTBS_VERTICAL))
            {
                draw_tool_bar_horz (hwnd, hdc, ntb_data);
            }
            else
            {
                draw_tool_bar_vert (hwnd, hdc, ntb_data);
            }
            EndPaint (hwnd, hdc);
            return 0;
        }

        case MSG_MOUSEMOVEIN:
        {
            if (!wParam && ntb_data->sel_item) {
                InvalidateRect (hwnd, &ntb_data->sel_item->rc_item, TRUE);
                ntb_data->sel_item = NULL;
            }
            break;
        }

        case MSG_MOUSEMOVE:
        {
            PNTBITEM item;
            int x, y;
            x = LOSWORD(lParam);
            y = HISWORD(lParam);

            if (GetCapture () == hwnd)
                break;

            if ((item = get_item_by_pos (ntb_data, x, y))) {
                if (ntb_data->sel_item != item) {
                    if (ntb_data->sel_item)
                        InvalidateRect (hwnd, &ntb_data->sel_item->rc_item, TRUE);
                    ntb_data->sel_item = item;
                    InvalidateRect (hwnd, &ntb_data->sel_item->rc_item, TRUE);
                }
                break;
            }
            break;
        }

        case MSG_LBUTTONDBLCLK:
        {    
            int sx, sy, x, y;
            PNTBITEM item;

            sx = x = LOSWORD(lParam);
            sy = y = HISWORD(lParam);

            if ( (item = get_item_by_pos (ntb_data, x, y)) )  {
                NotifyParent (hwnd, myself->id, item->id | 0x4000 );
            }        
        }
        break;

        case MSG_LBUTTONDOWN:
        {
            int posx, posy;
            NTBITEM* item;

            posx = LOSWORD (lParam);
            posy = HISWORD (lParam);


            if ((item = get_item_by_pos (ntb_data, posx, posy)) == NULL)
                break;
                
            if (GetCapture () == hwnd)
                break;
            SetCapture (hwnd);

            ntb_data->sel_item = item;
            ntb_data->btn_down = TRUE;
            InvalidateRect (hwnd, &item->rc_item, TRUE);
        }
        break;

        case MSG_LBUTTONUP:
        {
            int sx, sy, x, y;
            PNTBITEM item;

            sx = x = LOSWORD(lParam);
            sy = y = HISWORD(lParam);

            ntb_data->btn_down = FALSE;

            if (GetCapture() != hwnd)
                break;
            ReleaseCapture ();

            ScreenToClient (hwnd, &x, &y);

            if ((item = get_item_by_pos (ntb_data, x, y)) 
                            && item == ntb_data->sel_item) {
                if ((item->flags & NTBIF_TYPEMASK) == NTBIF_HOTSPOTBUTTON 
                                && item->hotspot_proc) {

                    RECT rc_hotspot = item->rc_hotspot;
                    OffsetRect (&rc_hotspot, item->rc_item.left, item->rc_item.top);

                    if (PtInRect (&rc_hotspot, x, y)) {
                        RECT rc_item = item->rc_item;
                            
                        ClientToScreen (hwnd, &rc_item.left, &rc_item.top);
                        ClientToScreen (hwnd, &rc_item.right, &rc_item.bottom);

                        item->hotspot_proc (hwnd, item->id, &rc_item, sx, sy);
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                        break;
                    }
                }

                if ( ( item->flags&NTBIF_TYPEMASK ) == NTBIF_CHECKBUTTON ) {
                    if ( item->flags & NTBIF_CHECKED )
                        item->flags &= ~NTBIF_CHECKED;
                    else
                        item->flags |= NTBIF_CHECKED;
                }
                NotifyParent (hwnd, myself->id, item->id);

                InvalidateRect (hwnd, &item->rc_item, TRUE);
            }
            else if (ntb_data->sel_item) {
                InvalidateRect (hwnd, &ntb_data->sel_item->rc_item, TRUE);
                ntb_data->sel_item = NULL;
            }
            break;
        }

        case MSG_RBUTTONDOWN:
        {
            int posx, posy;
            NTBITEM* item;

            posx = LOSWORD (lParam);
            posy = HISWORD (lParam);


            if ((item = get_item_by_pos (ntb_data, posx, posy)) == NULL)
                break;

            if (GetCapture () == hwnd)
                break;
            SetCapture (hwnd);

            ntb_data->sel_item = item;
            ntb_data->btn_down = TRUE;
            InvalidateRect (hwnd, &item->rc_item, TRUE);
        }
        break;

        case MSG_RBUTTONUP:
        {
            int sx, sy, x, y;
            PNTBITEM item;

            sx = x = LOSWORD(lParam);
            sy = y = HISWORD(lParam);

            ntb_data->btn_down = FALSE;

            if (GetCapture() != hwnd)
                break;
            ReleaseCapture ();

            ScreenToClient (hwnd, &x, &y);

            if ((item = get_item_by_pos (ntb_data, x, y)) 
                            && item == ntb_data->sel_item) {
                if ((item->flags & NTBIF_TYPEMASK) == NTBIF_HOTSPOTBUTTON 
                                && item->hotspot_proc) {

                    RECT rc_hotspot = item->rc_hotspot;
                    OffsetRect (&rc_hotspot, item->rc_item.left, item->rc_item.top);

                    if (PtInRect (&rc_hotspot, x, y)) {
                        RECT rc_item = item->rc_item;
                            
                        ClientToScreen (hwnd, &rc_item.left, &rc_item.top);
                        ClientToScreen (hwnd, &rc_item.right, &rc_item.bottom);

                        item->hotspot_proc (hwnd, item->id, &rc_item, sx, sy);
                        InvalidateRect (hwnd, &item->rc_item, TRUE);
                        break;
                    }
                }

                NotifyParent (hwnd, myself->id, item->id | 0x8000);

                InvalidateRect (hwnd, &item->rc_item, TRUE);
            }
            else if (ntb_data->sel_item) {
                InvalidateRect (hwnd, &ntb_data->sel_item->rc_item, TRUE);
                ntb_data->sel_item = NULL;
            }
            break;
        }
        
        default:
            break;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}

#endif /* _CTRL_NEWTOOLBAR */

