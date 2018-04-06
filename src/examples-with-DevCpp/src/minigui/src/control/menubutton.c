/*
** $Id: menubutton.c,v 1.30 2005/01/31 09:12:48 clear Exp $
**
** button.c: the Menu Button Control module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/16
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
#include "ctrl/menubutton.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "ctrlmisc.h"

#ifdef _CTRL_MENUBUTTON

#define _USE_FIXSTR  1

#include "menubutton_impl.h"

#ifdef _FLAT_WINDOW_STYLE
#   define _WIDTH_BORDER       1
#define myDrawButton(hdc, hwnd, x0, y0, x1, y1, flags, fillc) \
                 DrawFlatControlFrameEx(hdc, hwnd, x0, y0, x1, y1, 3, flags, fillc)
#else
#   define _WIDTH_BORDER       4
#define myDrawButton Draw3DControlFrameEx
#endif

#ifdef _PHONE_WINDOW_STYLE

const BITMAP* bmp_downarrow;

#   define _WIDTH_MENUBAR   (bmp_downarrow->bmWidth)
#   define _HEIGHT_MENUBAR  (bmp_downarrow->bmHeight)

#else

#   define _WIDTH_MENUBAR   12
#   define _HEIGHT_MENUBAR  8

#endif

#define _INTER_BARTEXT   4

static int MenuButtonCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMenuButtonControl (void)
{
    WNDCLASS WndClass;

#ifdef _PHONE_WINDOW_STYLE
    if ((bmp_downarrow = GetStockBitmap (STOCKBMP_DOWNARROW, 0, 0)) == NULL)
        return FALSE;
#endif

    WndClass.spClassName = CTRL_MENUBUTTON;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_BUTTON_DEF);
    WndClass.WinProc     = MenuButtonCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#if 0
void MenuButtonControlCleanup (void)
{
    return;
}
#endif

static BOOL mbInitMenuButtonData (MENUBTNDATA* mb_data, int len)
{
    int i;
    PMBITEM pmbi;
    
    mb_data->str_cmp = strncmp;

    mb_data->cur_sel = MB_INV_ITEM;

    // init item buffer.
    if (!(mb_data->buff_head = malloc (len * sizeof (MBITEM))))
        return FALSE;

    mb_data->buff_len = len;
    mb_data->buff_tail = mb_data->buff_head + len;
    mb_data->free_list = mb_data->buff_head;

    pmbi = mb_data->free_list;
    for (i = 0; i < len - 1; i++) {
        pmbi->next = pmbi + 1;
        pmbi ++;
    }
    pmbi->next = NULL;

    return TRUE;
}

static void mbMenuButtonCleanUp (MENUBTNDATA* mb_data)
{
    PMBITEM pmbi;
    PMBITEM next;

    pmbi = mb_data->first_item;
    while (pmbi) {
#if _USE_FIXSTR
        FreeFixStr (pmbi->text);
#else
        free (pmbi->text);
#endif
        next = pmbi->next;
        if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
            free (pmbi);

        pmbi = next;
    }
    
    free (mb_data->buff_head);
}

static void mbResetMenuButtonContent (PMENUBTNDATA mb_data)
{
    int i;
    PMBITEM pmbi, next;

    mb_data->item_count = 0;
    mb_data->cur_sel    = MB_INV_ITEM;

    pmbi = mb_data->first_item;
    while (pmbi) {
#if _USE_FIXSTR
        FreeFixStr (pmbi->text);
#else
        free (pmbi->text);
#endif
        next = pmbi->next;
        if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
            free (pmbi);

        pmbi = next;
    }

    mb_data->first_item = NULL;
    mb_data->free_list = mb_data->buff_head;

    pmbi = mb_data->free_list;
    for (i = 0; i < mb_data->buff_len - 1; i++) {
        pmbi->next = pmbi + 1;
        pmbi ++;
    }
    pmbi->next = NULL;
}

static PMBITEM mbAllocItem (PMENUBTNDATA mb_data)
{
    PMBITEM pmbi;

    if (mb_data->free_list) {
        pmbi = mb_data->free_list;
        mb_data->free_list = pmbi->next;
    }
    else
        pmbi = (PMBITEM) malloc (sizeof (MBITEM));
    
    return pmbi;
}

static void mbFreeItem (PMENUBTNDATA mb_data, PMBITEM pmbi)
{
    if (pmbi < mb_data->buff_head || pmbi > mb_data->buff_tail)
        free (pmbi);
    else {
        pmbi->next = mb_data->free_list;
        mb_data->free_list = pmbi;
    }
}

static int mbAddNewItem (DWORD dwStyle, 
        PMENUBTNDATA mb_data, PMBITEM newItem, int pos)
{
    PMBITEM pmbi;
    PMBITEM insPosItem = NULL;
    int insPos = 0;

    newItem->next = NULL;
    if (!mb_data->first_item)
        insPosItem = NULL;
    else if (dwStyle & MBS_SORT) {
        pmbi = mb_data->first_item;

        if (mb_data->str_cmp (newItem->text, pmbi->text, (size_t)-1) < 0) {
            insPosItem = NULL;
            insPos = 0;
        }
        else {
            while (pmbi->next) {
                if (mb_data->str_cmp (newItem->text, pmbi->next->text, (size_t)-1) <= 0)
                    break;
            
                pmbi = pmbi->next;
                insPos ++;
            }
            insPosItem = pmbi;
        }
    }
    else {
        pmbi = mb_data->first_item;

        if (pos < 0) {
            while (pmbi->next) {
                pmbi = pmbi->next;
                insPos ++;
            }
            insPosItem = pmbi;
        }
        else if (pos > 0) {
            int index = 0;

            while (pmbi->next) {
                if (pos == index)
                    break;
                pmbi = pmbi->next;
                index ++;
                insPos ++;
            }
            insPosItem = pmbi;
        }
    }

    if (insPosItem) {
        pmbi = insPosItem->next;
        insPosItem->next = newItem;
        newItem->next = pmbi;

        insPos ++;
    }
    else {
        pmbi = mb_data->first_item;
        mb_data->first_item = newItem;
        newItem->next = pmbi;
    }

    mb_data->item_count ++;

    return insPos;
}

static PMBITEM mbRemoveItem (PMENUBTNDATA mb_data, int* pos)
{
    int index = 0;
    PMBITEM pmbi, prev;

    if (!mb_data->first_item)
        return NULL;

    if (*pos < 0) {
        prev = mb_data->first_item;
        pmbi = mb_data->first_item;
        while (pmbi->next) {
            prev = pmbi;
            pmbi = pmbi->next;
            index ++;
        }

        if (pmbi == mb_data->first_item) {
            mb_data->first_item = mb_data->first_item->next;
            *pos = 0;
            return pmbi;
        }
        else {
            prev->next = pmbi->next;
            *pos = index;
            return pmbi;
        }
    }
    else if (*pos == 0) {
        pmbi = mb_data->first_item;
        mb_data->first_item = pmbi->next;
        return pmbi;
    }
    else {
        index = 0;
        prev = mb_data->first_item;
        pmbi = mb_data->first_item;
        while (pmbi->next) {
            if (*pos == index)
                break;

            prev = pmbi;
            pmbi = pmbi->next;
            index ++;
        }

        if (pmbi == mb_data->first_item) {
            mb_data->first_item = mb_data->first_item->next;
            *pos = 0;
            return pmbi;
        }
        else {
            prev->next = pmbi->next;
            *pos = index;
            return pmbi;
        }
    }

    return NULL;
}

static PMBITEM mbGetItem (PMENUBTNDATA mb_data, int pos)
{
    int index = 0;
    PMBITEM pmbi;

    pmbi = mb_data->first_item;

    while (pmbi) {
        if (index == pos)
            break;

        index ++;
        pmbi = pmbi->next;
    }

    return pmbi;
}

static void mbGetRects (HWND hWnd, DWORD dwStyle,
                                    RECT* prcClient,
                                    RECT* prcText,
                                    RECT* prcMenuBar)
{
    GetClientRect (hWnd, prcClient);
    prcClient->right --;
    prcClient->bottom --;

    if (dwStyle & MBS_LEFTARROW) {
        SetRect (prcMenuBar, prcClient->left + _WIDTH_BORDER,
                      (prcClient->top + prcClient->bottom - _HEIGHT_MENUBAR) >> 1,
                      prcClient->left + _WIDTH_BORDER + _WIDTH_MENUBAR,
                      (prcClient->top + prcClient->bottom + _HEIGHT_MENUBAR) >> 1);

        SetRect (prcText, prcMenuBar->right + _INTER_BARTEXT,
                      prcClient->top + _WIDTH_BORDER,
                      prcClient->right - _INTER_BARTEXT,
                      prcClient->bottom - _WIDTH_BORDER);

    }
    else {
        SetRect (prcText, prcClient->left + _WIDTH_BORDER,
                      prcClient->top + _WIDTH_BORDER,
                      prcClient->right - _WIDTH_MENUBAR - (_INTER_BARTEXT << 1),
                      prcClient->bottom - _WIDTH_BORDER);

        SetRect (prcMenuBar, prcText->right + _INTER_BARTEXT,
                      (prcClient->top + prcClient->bottom - _HEIGHT_MENUBAR) >> 1,
                      prcClient->right - _INTER_BARTEXT,
                      (prcClient->top + prcClient->bottom + _HEIGHT_MENUBAR) >> 1);
    }
}

#define _OFF_CMDID      100

static HMENU mbPopupMenu (HWND hWnd)
{
    HMENU hmnu;
    MENUITEMINFO mii;
    PMENUBTNDATA mb_data;
    int index = 0;
    PMBITEM pmbi;
    RECT rc;

    mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetWindowCaption (hWnd);
    if (!(hmnu = CreatePopupMenu (&mii)))
        return 0;

    pmbi = mb_data->first_item;
    while (pmbi) {

        memset (&mii, 0, sizeof(MENUITEMINFO));
        mii.state       = 0;
        mii.id          = index + _OFF_CMDID;
        if (pmbi->text && pmbi->bmp) {
            mii.type        = MFT_BMPSTRING;
            mii.hbmpChecked = pmbi->bmp;
            mii.hbmpUnchecked = pmbi->bmp;
            mii.typedata    = (DWORD)pmbi->text;
        }
        else if (pmbi->text) {
            mii.type        = MFT_STRING;
            mii.typedata    = (DWORD)pmbi->text;
        }
        else if (pmbi->bmp) {
            mii.type        = MFT_BITMAP;
            mii.hbmpChecked = pmbi->bmp;
            mii.hbmpUnchecked = pmbi->bmp;
        }
        else
            goto error;

        if (InsertMenuItem (hmnu, index, TRUE, &mii))
            goto error;

        index ++;
        pmbi = pmbi->next;
    }

    if (index == 0) goto error;

    GetClientRect (hWnd, &rc);
    ClientToScreen (hWnd, &rc.left, &rc.top);
    TrackPopupMenu (GetPopupSubMenu (hmnu), TPM_LEFTALIGN,
                                    rc.left, rc.top, hWnd);

    return hmnu;

error:
    DestroyMenu (hmnu);
    return 0;
}

static int MenuButtonCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    PMENUBTNDATA mb_data;

    switch (message) {
    case MSG_CREATE:
        if ((mb_data = (MENUBTNDATA *) malloc (sizeof(MENUBTNDATA))) == NULL)
            return -1;
        memset(mb_data,0,sizeof(MENUBTNDATA));

        SetWindowAdditionalData2 (hWnd, (DWORD) mb_data);
        if (!mbInitMenuButtonData (mb_data, DEF_MB_BUFFER_LEN)) {
            free (mb_data);
            return -1;
        }
        break;
       
    case MSG_DESTROY:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbMenuButtonCleanUp (mb_data);
        free (mb_data);
        break;
        
    case MBM_SETSTRCMPFUNC:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (mb_data->item_count == 0 && lParam) {
            mb_data->str_cmp = (STRCMP)lParam;
            return MB_OKAY;
        }
        return MB_ERR;

    case MBM_ADDITEM:
    {
        PMBITEM newItem;
        int pos;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;
        DWORD style = GetWindowStyle (hWnd);
#if _USE_FIXSTR
        int len;
#endif
        
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        newItem = mbAllocItem (mb_data);
        if (!newItem) {
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_ERRSPACE);
            return MB_ERR_SPACE;
        }

#if _USE_FIXSTR
        len = strlen (want->text);
        newItem->text = FixStrAlloc (len);
        if (!newItem->text) {
            mbFreeItem (mb_data, newItem);
            return MB_ERR_SPACE;
        }
        if (len > 0)
            strcpy (newItem->text, want->text);
#else
        newItem->text = strdup (want->text);
        if (!newItem->text) {
            mbFreeItem (mb_data, newItem);
            return MB_ERR_SPACE;
        }
#endif
        
        newItem->bmp  = want->bmp;
        newItem->data = want->data;
        pos = mbAddNewItem (style, mb_data, newItem, (int)wParam);
        return pos;
    }
        
    case MBM_DELITEM:
    {
        PMBITEM removed;
        int delete = (int)wParam;

        mb_data = (PMENUBTNDATA) GetWindowAdditionalData2 (hWnd);
        removed = mbRemoveItem (mb_data, &delete);
        if (removed) {
#if _USE_FIXSTR
            FreeFixStr (removed->text);
#else
            free (removed->text);
#endif
            mbFreeItem (mb_data, removed);

            mb_data->item_count --;
            if (mb_data->cur_sel == delete) {
                mb_data->cur_sel = MB_INV_ITEM;
                InvalidateRect (hWnd, NULL, TRUE);
            }

            return MB_OKAY;
        }
        return MB_INV_ITEM;
    }

    case MBM_RESETCTRL:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbResetMenuButtonContent (mb_data);
        InvalidateRect (hWnd, NULL, TRUE);
        return MB_OKAY;
        
    case MBM_GETCURITEM:
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        return mb_data->cur_sel;
        
    case MBM_SETCURITEM:
    {
        int old, new;

        new = (int)wParam;
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        old = mb_data->cur_sel;
        if (new >= 0 && new < mb_data->item_count) {
            mb_data->cur_sel = new;
            InvalidateRect (hWnd, NULL, TRUE);
        }

        return old;
    } 

    case MBM_SETITEMDATA:
    {
        PMBITEM pmbi;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        pmbi = mbGetItem (mb_data, (int)wParam);
        if (pmbi == NULL)
            return MB_INV_ITEM;

        if (want->which & MB_WHICH_TEXT) {
#if _USE_FIXSTR
            int len;

            FreeFixStr (pmbi->text);
            len = strlen (want->text);
            pmbi->text = FixStrAlloc (len);
            if (!pmbi->text) {
                pmbi->text = "";
                return MB_ERR_SPACE;
            }
            if (len > 0)
                strcpy (pmbi->text, want->text);
#else
            free (pmbi->text);
            pmbi->text = strdup (want->text);
            if (!pmbi->text) {
                pmbi->text = "";
                return MB_ERR_SPACE;
            }
#endif
        }
        if (want->which & MB_WHICH_BMP) {
            pmbi->bmp = want->bmp;
        }
        if (want->which & MB_WHICH_ATTDATA) {
            pmbi->data = want->data;
        }

        if ((int)wParam == mb_data->cur_sel)
            InvalidateRect (hWnd, NULL, TRUE);
        return MB_OKAY;
    }

    case MBM_GETITEMDATA:
    {
        PMBITEM pmbi;
        PMENUBUTTONITEM want = (PMENUBUTTONITEM)lParam;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        pmbi = mbGetItem (mb_data, (int)wParam);
        if (pmbi == NULL)
            return MB_INV_ITEM;

        if (want->which & MB_WHICH_TEXT) {
            want->text = pmbi->text;
        }
        if (want->which & MB_WHICH_BMP) {
            want->bmp = pmbi->bmp;
        }
        if (want->which & MB_WHICH_ATTDATA) {
            want->data = pmbi->data;
        }
        return MB_OKAY;
    }

    case MSG_CHAR:
        if (wParam == ' ') {
            mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
            mb_data->hmnu = mbPopupMenu (hWnd);
            if (mb_data->hmnu)
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_STARTMENU);
        }
        break;
        
    case MSG_LBUTTONDOWN:
        SetCapture (hWnd);
        break;
            
    case MSG_LBUTTONUP:
    {
        int x, y;
        RECT rcClient, rcText, rcMenuBar;

        if (GetCapture() != hWnd)
            break;
        ReleaseCapture ();

        x = LOSWORD(lParam);
        y = HISWORD(lParam);
        ScreenToClient (hWnd, &x, &y);
        mbGetRects (hWnd, GetWindowStyle (hWnd),
                        &rcClient, &rcText, &rcMenuBar);
        if (PtInRect (&rcMenuBar, x, y)) {
            mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
            mb_data->hmnu = mbPopupMenu (hWnd);
            if (mb_data->hmnu)
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_STARTMENU);
        }
        else if (PtInRect (&rcClient, x, y)) {
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_CLICKED);
        }
        break;
    }

    case MSG_ENDTRACKMENU:
    {
        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (mb_data->hmnu) {
            DestroyMenu (mb_data->hmnu);
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_ENDMENU);
            mb_data->hmnu = 0;
        }
        break;
    }

    case MSG_COMMAND:
    {
        int index = wParam - _OFF_CMDID;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        if (index < mb_data->item_count && index >= 0) {
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_SELECTED);
            if (index != mb_data->cur_sel) {
                mb_data->cur_sel = index;
                NotifyParent (hWnd, GetDlgCtrlID (hWnd), MBN_CHANGED);
                InvalidateRect (hWnd, NULL, TRUE);
            }
        }

        break;
    }

    case MSG_PAINT:
    {
        HDC hdc;
        RECT rcClient, rcText, rcMenuBar;
        UINT uFormat;
        DWORD dwStyle = GetWindowStyle (hWnd);
        int bk_color = GetWindowBkColor (hWnd);
        const char* text;
        BITMAP* bmp;

        mb_data = (PMENUBTNDATA)GetWindowAdditionalData2 (hWnd);
        mbGetRects (hWnd, dwStyle,
                        &rcClient, &rcText, &rcMenuBar);

        if (mb_data->cur_sel < 0) {
            text = GetWindowCaption (hWnd);
            bmp = NULL;
        }
        else {
            PMBITEM pmbi = mbGetItem (mb_data, mb_data->cur_sel);
            text = pmbi->text;
            bmp = pmbi->bmp;
        }

        hdc = BeginPaint (hWnd);
        if (!(dwStyle & MBS_NOBUTTON))
            myDrawButton (hdc, hWnd, rcClient.left, rcClient.top,
                        rcClient.right, rcClient.bottom, 
                        DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                        GetWindowBkColor (hWnd));

        if (bmp) {
            if (dwStyle & MBS_LEFTARROW) {
                FillBoxWithBitmap (hdc, 
                            rcMenuBar.right + _INTER_BARTEXT, 
                            (rcText.top + rcText.bottom - bmp->bmHeight) >> 1, 
                            0, 0, bmp);
                rcText.left += bmp->bmWidth + _INTER_BARTEXT;
            }
            else {
                FillBoxWithBitmap (hdc, 
                            rcMenuBar.left - _INTER_BARTEXT -  bmp->bmWidth, 
                            (rcText.top + rcText.bottom - bmp->bmHeight) >> 1, 
                            0, 0, bmp);
                rcText.right -= bmp->bmWidth + _INTER_BARTEXT;
            }
        }

        rcText.left +=  (_INTER_BARTEXT >> 1);

        if (text) {
            uFormat = DT_SINGLELINE | DT_VCENTER;
            switch (dwStyle & MBS_ALIGNMASK) {
                case MBS_ALIGNRIGHT:
                    uFormat |= DT_RIGHT;
                    break;
                case MBS_ALIGNCENTER:
                    uFormat |= DT_CENTER;
                    break;
                default:
                    uFormat |= DT_LEFT;
                    break;
            }
            SetBkColor (hdc, bk_color);
            SetBkMode (hdc, BM_TRANSPARENT);
            if (dwStyle & WS_DISABLED) {
                RECT rc = rcText;

                SetTextColor (hdc, PIXEL_lightwhite);
                OffsetRect (&rc, 1, 1);
                DrawText (hdc, text, -1, &rc, uFormat);

                SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DISABLED));
                DrawText (hdc, text, -1, &rcText, uFormat);
            }
            else {
                SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL));
                DrawText (hdc, text, -1, &rcText, uFormat);
            }
        }

#ifdef _FLAT_WINDOW_STYLE
        DrawFlatControlFrameEx (hdc, hWnd, rcMenuBar.left, rcMenuBar.top, 
                    rcMenuBar.right, rcMenuBar.bottom, 0, DF_3DBOX_NORMAL | DF_3DBOX_FILL, bk_color);
#elif defined (_PHONE_WINDOW_STYLE)
        FillBoxWithBitmap (hdc, rcMenuBar.left, rcMenuBar.top, 0, 0, bmp_downarrow);
#else
        Draw3DThinFrameEx (hdc, hWnd, rcMenuBar.left, rcMenuBar.top, 
                    rcMenuBar.right, rcMenuBar.bottom, DF_3DBOX_NORMAL | DF_3DBOX_FILL, bk_color);
#endif
        EndPaint (hWnd, hdc);
        return 0;
    }

    default:
        break;
    }
    
    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _CTRL_MENUBUTTON */

