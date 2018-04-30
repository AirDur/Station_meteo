/*
** $Id: propsheet.c,v 1.39 2005/01/31 09:12:48 clear Exp $
**
** propsheet.c: the Property Sheet (Tab) control.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming and others.
**
** NOTE: Originally by Wang Jian and Jiang Jun.
**
** Create date: 2001/11/20
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
#include "ctrl/static.h"
#include "ctrl/button.h"
#include "ctrl/propsheet.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _CTRL_PROPSHEET

#include "propsheet_impl.h"
#include "ctrlmisc.h"
    
static int PropSheetCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterPropSheetControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_PROPSHEET;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (0);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = PropSheetCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/**************************internal functions******************************/

#define _ICON_OFFSET 2
#define _GAP_ICON_TEXT 2
#define _MIN_TAB_WIDTH  10

/* destroy a page */
static void destroy_page (PROPPAGE * page)
{
    FreeFixStr (page->title);
    DestroyWindow (page->hwnd);
}

/* change page title */
static int set_page_title (PROPPAGE * page, char *ptr)
{
    int len = 0;
    
    if (page->title) {
        FreeFixStr (page->title);
    }

    if (ptr) {
        len = strlen (ptr);
        if (!(page->title = FixStrAlloc (len)))
            return PS_ERR;

        strcpy (page->title, ptr);
    }
    else {
        page->title = NULL;
    }
 
    return PS_OKAY;
}

/* create a new page */
static BOOL create_page (HWND hwnd, PPROPSHEETDATA pData, PROPPAGE* page, PDLGTEMPLATE pDlgTemplate, WNDPROC proc)
{
    int i, len;
    PCTRLDATA pCtrlData;
    HWND hCtrl;
    RECT rcPage;

    if (!pDlgTemplate->controls)
        return FALSE;

    GetClientRect (hwnd, &rcPage);
    page->hwnd = CreateWindowEx (CTRL_STATIC, "", SS_LEFT, WS_EX_NONE,
                                IDC_STATIC,
                                2, pData->head_rc.bottom + 2, 
                                rcPage.right - 4, rcPage.bottom - 4 - pData->head_rc.bottom,
                                hwnd, pDlgTemplate->dwAddData);

    if (page->hwnd == HWND_INVALID)
        return FALSE;

    len = strlen (pDlgTemplate->caption);
    page->title = FixStrAlloc (len);
    if (page->title)
        strcpy (page->title, pDlgTemplate->caption);

    page->proc = proc;
    page->icon = pDlgTemplate->hIcon;

    for (i = 0; i < pDlgTemplate->controlnr; i++) {
        pCtrlData = pDlgTemplate->controls + i;
        hCtrl = CreateWindowEx (pCtrlData->class_name,
                              pCtrlData->caption,
                              pCtrlData->dwStyle | WS_CHILD,
                              pCtrlData->dwExStyle,
                              pCtrlData->id,
                              pCtrlData->x, pCtrlData->y,
                              pCtrlData->w, pCtrlData->h,
                              page->hwnd,
                              pCtrlData->dwAddData);

        if (hCtrl == HWND_INVALID) {
            DestroyWindow (page->hwnd);
            if (page->title)
                FreeFixStr (page->title);
            return FALSE;
        }
    }

    SetWindowCallbackProc (page->hwnd, page->proc);

    SendMessage (page->hwnd, MSG_INITPAGE, 0, pDlgTemplate->dwAddData);
    return TRUE;
}

static void show_hide_page (PPROPPAGE page, int show_cmd)
{
    HWND focus;

    ShowWindow (page->hwnd, show_cmd);

    focus = GetNextDlgTabItem (page->hwnd, (HWND)0, 0);
    if (SendMessage (page->hwnd, MSG_SHOWPAGE, focus, show_cmd) && show_cmd == SW_SHOW) {
        if (focus)
            SetFocus (focus);
    }
}

/* recalculate widths of the tabs after a page added or removed. */
static void recalc_tab_widths (HWND hwnd, PPROPSHEETDATA pData, DWORD dwStyle)
{
    PPROPPAGE page;

    if (pData->page_count == 0)
        return;

    if (dwStyle & PSS_COMPACTTAB) {
        HDC hdc;
        SIZE ext;
        int total_width = 0;

        hdc = GetClientDC (hwnd);
        page = pData->head;
        while (page) {
            page->width = 0;
            if (page->title) {
                GetTextExtent (hdc, page->title, -1, &ext);
                page->width = ext.cx + 4;
            }

            if (page->icon)
                page->width += GetMainWinMetrics (MWM_ICONX);

            if (page->width < _MIN_TAB_WIDTH)
                page->width = _MIN_TAB_WIDTH;

            total_width += page->width;
            page = page->next;
        }
        ReleaseDC (hdc);

        while (total_width > pData->head_rc.right) {
            int new_width = 0;

            page = pData->head;
            while (page) {

                page->width -= 2;
                if (page->width < _MIN_TAB_WIDTH)
                    page->width = _MIN_TAB_WIDTH;

                new_width += page->width;
                page = page->next;
            }

            total_width = new_width;
        }

        pData->head_width = total_width;
    }
    else {
        int width;

        width = pData->head_rc.right * 8 / (pData->page_count * 10);
        if (width < _MIN_TAB_WIDTH)
            width = pData->head_rc.right / pData->page_count;

        page = pData->head;
        while (page) {
            page->width = width;
            page = page->next;
        }

        pData->head_width = width * pData->page_count;
    }
}

/* resize children after the sheet resized. */
static void resize_children (PPROPSHEETDATA pData, const RECT* rcPage)
{
    PPROPPAGE page;

    page = pData->head;
    while (page) {
        MoveWindow (page->hwnd, 
                        0, pData->head_rc.bottom, 
                        rcPage->right, 
                        rcPage->bottom - pData->head_rc.bottom,
                        page == pData->active);

        page = page->next;
    }
}

/* return the page from index. */
static PPROPPAGE get_page (PPROPSHEETDATA pData, int index)
{
    int i = 0;
    PPROPPAGE page;

    page = pData->head;
    while (page) {
        if (i == index)
            return page;

        i++;
        page = page->next;
    }

    return NULL;
}

/* append a new page */
static int append_page (PPROPSHEETDATA pData, PPROPPAGE new_page)
{
    int i = 0;
    PPROPPAGE page;

    page = pData->head;
    while (page && page->next) {
        i++;
        page = page->next;
    }

    new_page->next = NULL;
    if (page) {
        i++;
        page->next = new_page;
    }
    else {
        pData->head = new_page;
    }

    pData->page_count++;
    return i;
}

/* remove a page */
static void remove_page (PPROPSHEETDATA pData, PPROPPAGE rm_page)
{
    PPROPPAGE page, prev;

    prev = NULL;
    page = pData->head;
    while (page) {
        if (page == rm_page) {
            if (prev) {
                prev->next = rm_page->next;
            }
            else {
                pData->head = rm_page->next;
            }

            pData->page_count--; 
            break;
        }

        prev = page;
        page = page->next;
    }
}

/* window procedure of the property sheet. */
static int PropSheetCtrlProc (HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    PCONTROL      pCtrl;
    PPROPSHEETDATA pData;

    pCtrl = Control (hwnd); 
    pData = (PROPSHEETDATA *) pCtrl->dwAddData2;

    switch (message) {
        case MSG_CREATE:
        {
            if (!(pData = malloc (sizeof (PROPSHEETDATA))))
                return -1;
            memset(pData,0,sizeof (PROPSHEETDATA));
            pCtrl->dwAddData2 = (DWORD)pData;

            pData->head_rc.top = 0;
            pData->head_rc.left = 0;
            pData->head_rc.right = 0;
            pData->head_rc.bottom = GetMainWinMetrics (MWM_ICONY) + 4 + _ICON_OFFSET * 2;
            break;
        }

        /* make the client size same as window size */
        case MSG_SIZECHANGED:
        {
            const RECT* rcWin = (RECT*)wParam;
            RECT* rcClient = (RECT*)lParam;

            *rcClient = *rcWin;
            pData->head_rc.right = RECTWP (rcClient);
            recalc_tab_widths (hwnd, pData, pCtrl->dwStyle);
            resize_children (pData, rcClient);
            InvalidateRect (hwnd, &pData->head_rc, TRUE);
            return 1;
        }

        case MSG_DESTROY:
        {
            PPROPPAGE page, temp;

            page = pData->head;
            while (page) {
                temp = page->next;
                destroy_page (page);
                free (page);
                page = temp;
            }

            free (pData);
            break;
        }

        case MSG_GETDLGCODE:
            return DLGC_WANTTAB | DLGC_WANTARROWS;

        case PSM_SHEETCMD:
        {
            int index = 0;
            PPROPPAGE page;

            page = pData->head;
            while (page) {
                if (SendMessage (page->hwnd, MSG_SHEETCMD, wParam, lParam))
                    /* when encounter an error, return page index plus 1. */
                    return index + 1;
                index++;
                page = page->next;
            }

            /* success, return 0 */
            return 0;
        }

        /* set the active page. */
        case PSM_SETACTIVEINDEX:
        {
            PPROPPAGE page;

            if ((page = get_page (pData, wParam)) && page != pData->active) {
                show_hide_page (pData->active, SW_HIDE);
                pData->active = page;
                NotifyParent (hwnd, pCtrl->id, PSN_ACTIVE_CHANGED);
                show_hide_page (page, SW_SHOW);
                InvalidateRect (hwnd, &pData->head_rc, TRUE);
                return PS_OKAY;
            }

            return PS_ERR;
        }

        /* Retrieves the index of the active page of the property sheet. */
        case PSM_GETACTIVEINDEX:
        {
            int index = 0;
            PPROPPAGE page;

            page = pData->head;
            while (page) {
                if (page == pData->active)
                    return index;

                index ++;
                page = page->next;
            }
            
            return PS_ERR;
        }

        case PSM_GETACTIVEPAGE:
        {
            if (pData->active)
                return pData->active->hwnd;
            else
                return HWND_INVALID;
        }

        /* Returns the page handle by index */
        case PSM_GETPAGE:
        {
            int index = 0;
            PPROPPAGE page;

            page = pData->head;
            while (page) {
                if (index == wParam)
                    return page->hwnd;

                index ++;
                page = page->next;
            }
            
            return HWND_INVALID;
        }

        /* Returns the page index by handle */
        case PSM_GETPAGEINDEX:
        {
            int index = 0;
            PPROPPAGE page;

            page = pData->head;
            while (page) {
                if (page->hwnd == wParam)
                    return index;

                index ++;
                page = page->next;
            }
            
            return PS_ERR;
        }

        /* Retrieves the number of pages in the property sheet */
        case PSM_GETPAGECOUNT:
            return pData->page_count;

        case PSM_GETTITLELENGTH:
        {
            int len = PS_ERR;
            PPROPPAGE page;

            if ((page = get_page (pData, wParam))) {
                len = strlen (page->title);
            }

            return len;
        }

        case PSM_GETTITLE:
        {
            char* buffer = (char*)lParam;
            PPROPPAGE page;

            if ((page = get_page (pData, wParam))) {
                strcpy (buffer, page->title);
                return PS_OKAY;
            }

            return PS_ERR;
        }

        case PSM_SETTITLE:
        {
            BOOL rc = PS_ERR;
            char* buffer = (char*)lParam;
            PPROPPAGE page;

            if ((page = get_page (pData, wParam))) {
                rc = set_page_title (page, buffer);
                recalc_tab_widths (hwnd, pData, pCtrl->dwStyle);
                InvalidateRect (hwnd, &pData->head_rc, TRUE);
            }
            return rc;
        }

        case PSM_ADDPAGE:
        {
            int index;
            PPROPPAGE page;

            if ((pData->head_rc.right / (pData->page_count + 1)) < _MIN_TAB_WIDTH)
                return PS_ERR;

            if (!(page = malloc (sizeof (PROPPAGE)))) {
                return PS_ERR;
            }
            memset(page,0,sizeof (PROPPAGE));

            if (!create_page (hwnd, pData, page, (DLGTEMPLATE *)wParam, (WNDPROC)lParam)) {
                free (page);
                return PS_ERR;
            }

            index = append_page (pData, page);

            if (pData->active)
                show_hide_page (pData->active, SW_HIDE);
            pData->active = page;
            NotifyParent (hwnd, pCtrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (page, SW_SHOW);

            recalc_tab_widths (hwnd, pData, pCtrl->dwStyle);
            InvalidateRect (hwnd, &pData->head_rc, TRUE);
            return index;
        }

        case PSM_REMOVEPAGE:
        {
            PPROPPAGE page;

            if ((page = get_page (pData, wParam))) {
                remove_page (pData, page);
                destroy_page (page);
                free (page);
                recalc_tab_widths (hwnd, pData, pCtrl->dwStyle);
            }
            else
                return PS_ERR;

            if (pData->active == page) {
                pData->active = pData->head;
                NotifyParent (hwnd, pCtrl->id, PSN_ACTIVE_CHANGED);
                if (pData->active)
                    show_hide_page (pData->active, SW_SHOW);
            }

            InvalidateRect (hwnd, &pData->head_rc, TRUE);
            return PS_OKAY;
        }

        case MSG_LBUTTONDOWN:
        {
            int x, y;
            PPROPPAGE page;
            RECT title_rc = {0, 0, 0, 0};
            
            title_rc.bottom = pData->head_rc.bottom;

            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            
            page = pData->head;
            while (page) {
                title_rc.left = title_rc.right;
                title_rc.right = title_rc.left + page->width;

                if (PtInRect (&title_rc, x, y) && page != pData->active) {
                    show_hide_page (pData->active, SW_HIDE);
                    pData->active = page;
                    NotifyParent (hwnd, pCtrl->id, PSN_ACTIVE_CHANGED);
                    show_hide_page (page, SW_SHOW);
                    InvalidateRect (hwnd, &pData->head_rc, TRUE);
                    break;
                }

                page = page->next;
            }

            break;
        }

        case MSG_KEYDOWN:
        {
            PPROPPAGE page, new_active = NULL;

            if (pData->head == NULL)
                break;

#if 0
            if (pData->active && SendMessage (pData->active->hwnd, MSG_GETDLGCODE, 0, 0L) 
                        & DLGC_WANTARROWS) {
                break;
            }
#else
            if (!(lParam & KS_CTRL))
                break;
#endif

            switch (LOWORD (wParam)) {
            case SCANCODE_CURSORBLOCKDOWN:
            case SCANCODE_CURSORBLOCKRIGHT:
                new_active = pData->active->next;
                if (new_active == NULL)
                    new_active = pData->head;
                break;

            case SCANCODE_CURSORBLOCKUP:
            case SCANCODE_CURSORBLOCKLEFT:
                page = pData->head;
                if (pData->head == pData->active) {
                    while (page && page->next) {
                        page = page->next;
                    }
                }
                else {
                    while (page) {
                        if (page->next == pData->active)
                            break;
                        page = page->next;
                    }
                }
                new_active = page;
                break;
            }

            if (new_active == NULL)
                break;

            show_hide_page (pData->active, SW_HIDE);
            pData->active = new_active;
            NotifyParent (hwnd, pCtrl->id, PSN_ACTIVE_CHANGED);
            show_hide_page (new_active, SW_SHOW);
            InvalidateRect (hwnd, &pData->head_rc, TRUE);
            return 0;
        }

        case MSG_NCPAINT:
        case MSG_ERASEBKGND:
            return 0;

        case MSG_PAINT:
        {
            int x, ty, by;
            HDC  hdc;
            int text_extent;
            PPROPPAGE page;
            RECT title_rc = {0, 0, 0, 0};

            title_rc.bottom = pData->head_rc.bottom;

            hdc = BeginPaint (hwnd);

#ifdef _FLAT_WINDOW_STYLE
            DrawFlatControlFrameEx (hdc, hwnd, 0, pData->head_rc.bottom - 2,
                                      pData->head_rc.right, 
                                      pCtrl->bottom - pCtrl->top, 1, 
                                      DF_3DBOX_NORMAL | DF_3DBOX_NOTFILL, 0);
#else
            Draw3DThickFrameEx (hdc, hwnd, 0, pData->head_rc.bottom - 2, 
                                      pData->head_rc.right, 
                                      pCtrl->bottom - pCtrl->top,
                                      DF_3DBOX_NORMAL | DF_3DBOX_NOTFILL, 0);
#endif

            SetBrushColor (hdc, GetWindowElementColorEx (hwnd, BKC_CONTROL_DEF));
            FillBox (hdc, 0, 0, pData->head_rc.right, pData->head_rc.bottom - 2);

            page = pData->head;
            while (page) {
                title_rc.left = title_rc.right;
                title_rc.right = title_rc.left + page->width;

                SelectClipRect (hdc, &title_rc);

                x = title_rc.left + _ICON_OFFSET;
                ty = title_rc.top;
                if (page != pData->active) {
                    ty += 2;
                    by = title_rc.bottom - 2;
#if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN)
                    SetBrushColor (hdc, PIXEL_lightgray);
                    FillBox (hdc, title_rc.left, ty, title_rc.right - title_rc.left, by - 2);
#endif
                }
                else {
                    by = title_rc.bottom;
#if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN)
                    ty += 1;
                    SetBrushColor (hdc, PIXEL_lightwhite);
#endif
                    FillBox (hdc, title_rc.left, ty, title_rc.right - title_rc.left, by);
                }

#ifdef _FLAT_WINDOW_STYLE
#ifdef _GRAY_SCREEN
                SetPenColor (hdc, GetWindowElementColorEx (hwnd, WEC_FLAT_BORDER));
                MoveTo (hdc, title_rc.left + 1, by);
                LineTo (hdc, title_rc.left + 1, ty);
                LineTo (hdc, title_rc.right - 3, ty);
                LineTo (hdc, title_rc.right - 1, ty + 2);
                LineTo (hdc, title_rc.right - 1, by);
#else
                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_LEFT_INNER));
                if (page == pData->head) {
                    MoveTo (hdc, title_rc.left, by);
                    LineTo (hdc, title_rc.left, ty + 2);
                }
                MoveTo (hdc, title_rc.left, ty + 2);
                LineTo (hdc, title_rc.left + 2, ty);
                LineTo (hdc, title_rc.right - 3, ty);
                LineTo (hdc, title_rc.right - 1, ty + 2);
                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_LEFT_OUTER));
                LineTo (hdc, title_rc.right - 1, by - 1);

#endif
#else
                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_LEFT_INNER));
                MoveTo (hdc, title_rc.left, by);
                LineTo (hdc, title_rc.left, ty + 2);
                LineTo (hdc, title_rc.left + 2, ty);
                LineTo (hdc, title_rc.right - 3, ty);
                LineTo (hdc, title_rc.right - 1, ty + 2);
                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_LEFT_OUTER));
                LineTo (hdc, title_rc.right - 1, by);

                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_RIGHT_OUTER));
                MoveTo (hdc, title_rc.left + 1, by);
                LineTo (hdc, title_rc.left + 1, ty + 3);
                LineTo (hdc, title_rc.left + 3, ty + 1);
                LineTo (hdc, title_rc.right - 4, ty + 1);
                LineTo (hdc, title_rc.right - 2, ty + 3);
                SetPenColor(hdc, GetWindowElementColorEx (hwnd, WEC_3DFRAME_LEFT_INNER));
                LineTo (hdc, title_rc.right - 2, by);
#endif

                ty += _ICON_OFFSET + 2;
                text_extent = RECTW (title_rc) - _ICON_OFFSET * 2;

                if (page->icon) {
                    DrawIcon (hdc, x, ty, GetMainWinMetrics (MWM_ICONX),
                                   GetMainWinMetrics (MWM_ICONY), page->icon);
                    x += GetMainWinMetrics (MWM_ICONX);
                    x += _GAP_ICON_TEXT;
                    text_extent -= GetMainWinMetrics (MWM_ICONX) + _GAP_ICON_TEXT;
                }

#if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN)
                if (page != pData->active) {
                    SetBkColor (hdc, PIXEL_lightgray);
                }
                else {
                    SetBkColor (hdc, PIXEL_lightwhite);
                }
#else
                SetBkColor (hdc, GetWindowElementColorEx (hwnd, BKC_CONTROL_DEF));
#endif
                {
                    SIZE size;
                    int eff_chars, eff_len;
                    eff_len = GetTextExtentPoint (hdc, page->title, strlen(page->title), text_extent,
                                    &eff_chars, NULL, NULL, &size);
                    TextOutLen (hdc, x, ty, page->title, eff_len);
                }

                page = page->next;
            }

            EndPaint (hwnd, hdc);
            return 0;
        }

        default:
            break;
    }

    return DefaultControlProc (hwnd, message, wParam, lParam);
}

#endif /* _CTRL_PROPSHEET*/

