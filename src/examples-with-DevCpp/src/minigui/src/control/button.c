/*
** $Id: button.c,v 1.61 2005/01/31 08:52:29 clear Exp $
**
** button.c: the Button Control module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Note:
**   Originally by Zhao Jianghua.
**
** Create date: 1999/8/23
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
** Modify records:
**
**  Who             When        Where       For What                Status
**-----------------------------------------------------------------------------
**  WEI Yongming    1999/10/27  Tsinghua    Notify Message          Fininshed
**  WEI Yongming    2000/02/24  Tsinghua    Add MPL License         Finished
**
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
#include "ctrl/button.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _CTRL_BUTTON

#include "button_impl.h"
#include "ctrlmisc.h"

#ifdef _FLAT_WINDOW_STYLE

#define BTN_WIDTH_BORDER    1

#ifndef _GRAY_SCREEN
#define myDrawButton(hdc, hwnd, x0, y0, x1, y1, flags, fillc) \
                 DrawFlatControlFrameEx(hdc, hwnd, x0, y0, x1, y1, 0, flags, fillc)
#else
#define myDrawButton(hdc, hwnd, x0, y0, x1, y1, flags, fillc) \
                 DrawFlatControlFrameEx(hdc, hwnd, x0, y0, x1, y1, 3, flags, fillc)
#endif

#define FocusRect(hdc, x, y, w, h)

#elif defined (_PHONE_WINDOW_STYLE)

#define BTN_WIDTH_BORDER    2

#define myDrawButton btnDrawPhoneButton

#define FocusRect(hdc, x, y, w, h)

#else

#define BTN_WIDTH_BORDER    4

#define myDrawButton Draw3DControlFrameEx

#endif

#ifdef _PHONE_WINDOW_STYLE
#   define BTN_WIDTH_BMP       16
#   define BTN_HEIGHT_BMP      16
#   define BTN_INTER_BMPTEXT   2
#else
#   define BTN_WIDTH_BMP       14
#   define BTN_HEIGHT_BMP      13
#   define BTN_INTER_BMPTEXT   2
#endif

#define BUTTON_STATUS(pCtrl)   (((PBUTTONDATA)(pCtrl->dwAddData2))->status)
#define BUTTON_DATA(pCtrl)     (((PBUTTONDATA)(pCtrl->dwAddData2))->data)

static const BITMAP* bmp_button;

#define BUTTON_BMP              bmp_button

#ifdef _PHONE_WINDOW_STYLE

static const BITMAP* bmp_pushbutton;
static const BITMAP* bmp_pushedbutton;

#define PUSHBUTTON_BMP          bmp_pushbutton
#define PUSHEDBUTTON_BMP        bmp_pushedbutton
#define BTN_WIDTH_CORNER        2

#endif

static int ButtonCtrlProc (HWND hWnd, int uMsg, WPARAM wParam, LPARAM lParam);

BOOL RegisterButtonControl (void)
{
    WNDCLASS WndClass;

#ifdef _PHONE_WINDOW_STYLE
    if ((bmp_button = GetStockBitmap (STOCKBMP_BUTTON, 0, 0)) == NULL)
        return FALSE;

    if ((bmp_pushbutton = GetStockBitmap (STOCKBMP_PUSHBUTTON, 0, 0)) == NULL)
        return FALSE;

    if ((bmp_pushedbutton = GetStockBitmap (STOCKBMP_PUSHEDBUTTON, 0, 0)) == NULL)
        return FALSE;
#else
//    if ((bmp_button = GetStockBitmap (STOCKBMP_BUTTON, 0, 0)) == NULL)
//        return FALSE;
#endif

    WndClass.spClassName = CTRL_BUTTON;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_ARROW);
    WndClass.iBkColor    = GetWindowElementColor (BKC_CONTROL_DEF);
    WndClass.WinProc     = ButtonCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#if 0
void ButtonControlCleanup (void)
{
    // do nothing.
}
#endif

#ifdef _PHONE_WINDOW_STYLE

static void btnDrawPhoneButton (HDC hdc, HWND hwnd, int l, int t, int r, int b, DWORD flags, gal_pixel fillc)
{
    int x;
    const BITMAP* bmp = ((flags & DF_3DBOX_STATEMASK) == DF_3DBOX_NORMAL)?PUSHBUTTON_BMP:PUSHEDBUTTON_BMP;

    FillBoxWithBitmapPart (hdc, l, t,
                BTN_WIDTH_CORNER, bmp_pushbutton->bmHeight,
                0, 0, bmp, 0, 0);

    for (x = l + BTN_WIDTH_CORNER; x < r - BTN_WIDTH_CORNER; x += BTN_WIDTH_CORNER) {
        FillBoxWithBitmapPart (hdc, x, t,
                BTN_WIDTH_CORNER, bmp_pushbutton->bmHeight,
                0, 0, bmp, BTN_WIDTH_CORNER, 0);
    }

    FillBoxWithBitmapPart (hdc, r - BTN_WIDTH_CORNER, t,
                BTN_WIDTH_CORNER, bmp_pushbutton->bmHeight,
                0, 0, bmp, bmp_pushbutton->bmWidth - BTN_WIDTH_CORNER, 0);
}

#endif

static void btnGetRects (HWND hWnd, DWORD dwStyle,
                                    RECT* prcClient, 
                                    RECT* prcText, 
                                    RECT* prcBitmap)
{
    GetClientRect (hWnd, prcClient);
#ifndef _PHONE_WINDOW_STYLE
    prcClient->right --;
    prcClient->bottom --;
#endif

    SetRect (prcText, (prcClient->left   + BTN_WIDTH_BORDER),
                      (prcClient->top    + BTN_WIDTH_BORDER),
                      (prcClient->right  - BTN_WIDTH_BORDER),
                      (prcClient->bottom - BTN_WIDTH_BORDER));

    SetRectEmpty (prcBitmap);

    if ( ((dwStyle & BS_TYPEMASK) < BS_CHECKBOX)
            || (dwStyle & BS_PUSHLIKE))
        return;

    if (dwStyle & BS_LEFTTEXT) {
        SetRect (prcText, prcClient->left + 1,
                          prcClient->top + 1,
                          prcClient->right - BTN_WIDTH_BMP - BTN_INTER_BMPTEXT,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->right - BTN_WIDTH_BMP,
                            prcClient->top + 1,
                            prcClient->right - 1,
                            prcClient->bottom - 1);
    }
    else {
        SetRect (prcText, prcClient->left + BTN_WIDTH_BMP + BTN_INTER_BMPTEXT,
                          prcClient->top + 1,
                          prcClient->right - 1,
                          prcClient->bottom - 1);
        SetRect (prcBitmap, prcClient->left + 1,
                            prcClient->top + 1,
                            prcClient->left + BTN_WIDTH_BMP,
                            prcClient->bottom - 1);
    }

}

static void btnGetTextBoundsRect (PCONTROL pCtrl, HDC hdc, DWORD dwStyle, 
                const RECT* prcText, RECT* prcBounds)
{
    UINT uFormat;
  
    *prcBounds = *prcText;

    if (dwStyle & BS_MULTLINE)
        uFormat = DT_WORDBREAK;
    else
        uFormat = DT_SINGLELINE;

    if ((dwStyle & BS_TYPEMASK) == BS_PUSHBUTTON
            || (dwStyle & BS_TYPEMASK) == BS_DEFPUSHBUTTON
            || (dwStyle & BS_PUSHLIKE))
        uFormat |= DT_CENTER | DT_VCENTER;
    else {
        uFormat = DT_TOP | DT_LEFT;

        if ((dwStyle & BS_ALIGNMASK) == BS_LEFT)
            uFormat = DT_WORDBREAK | DT_LEFT;
        else if ((dwStyle & BS_ALIGNMASK) == BS_RIGHT)
            uFormat = DT_WORDBREAK | DT_RIGHT;
        else if ((dwStyle & BS_ALIGNMASK) == BS_CENTER)
            uFormat = DT_WORDBREAK | DT_CENTER;
            
        if ((dwStyle & BS_ALIGNMASK) == BS_TOP)
            uFormat = DT_SINGLELINE | DT_TOP;
        else if ((dwStyle & BS_ALIGNMASK) == BS_BOTTOM)
            uFormat = DT_SINGLELINE | DT_BOTTOM;
        else if ((dwStyle & BS_ALIGNMASK) == BS_VCENTER)
            uFormat = DT_SINGLELINE | DT_VCENTER;
    }

    uFormat |= DT_CALCRECT;
    
    DrawText (hdc, pCtrl->spCaption, -1, prcBounds, uFormat);
}

static void btnPaintContent (PCONTROL pCtrl, HDC hdc, DWORD dwStyle, 
                             RECT* prcText)
{
    BOOL pushbutton = FALSE;

    switch (dwStyle & BS_CONTENTMASK)
    {
        case BS_TEXT:
        case BS_LEFTTEXT:
        {
            UINT uFormat;
            
            if (dwStyle & BS_MULTLINE)
                uFormat = DT_WORDBREAK;
            else
                uFormat = DT_SINGLELINE;

            if ((dwStyle & BS_TYPEMASK) == BS_PUSHBUTTON
                    || (dwStyle & BS_TYPEMASK) == BS_DEFPUSHBUTTON
                    || (dwStyle & BS_PUSHLIKE)) {
                pushbutton = TRUE;
                uFormat |= DT_CENTER | DT_VCENTER;
            }
            else {
                uFormat = DT_TOP | DT_LEFT;

                if ((dwStyle & BS_ALIGNMASK) == BS_LEFT)
                    uFormat = DT_WORDBREAK | DT_LEFT;
                else if ((dwStyle & BS_ALIGNMASK) == BS_RIGHT)
                    uFormat = DT_WORDBREAK | DT_RIGHT;
                else if ((dwStyle & BS_ALIGNMASK) == BS_CENTER)
                    uFormat = DT_WORDBREAK | DT_CENTER;
            
                if ((dwStyle & BS_ALIGNMASK) == BS_TOP)
                    uFormat = DT_SINGLELINE | DT_TOP;
                else if ((dwStyle & BS_ALIGNMASK) == BS_BOTTOM)
                    uFormat = DT_SINGLELINE | DT_BOTTOM;
                else if ((dwStyle & BS_ALIGNMASK) == BS_VCENTER)
                    uFormat = DT_SINGLELINE | DT_VCENTER;
            }
            
            SetBkColor (hdc, GetWindowBkColor ((HWND)pCtrl));
            if (dwStyle & WS_DISABLED) {
                RECT rc = *prcText;
                
                SetBkMode (hdc, BM_TRANSPARENT);
#if 0
                SetTextColor (hdc, PIXEL_lightwhite);
                OffsetRect (prcText, 2, 2);
                DrawText (hdc, pCtrl->spCaption, -1, prcText, uFormat);
#endif
                SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_CONTROL_DISABLED));
                DrawText (hdc, pCtrl->spCaption, -1, &rc, uFormat);
            }
            else {
                SetBkMode (hdc, BM_TRANSPARENT);
                if (pushbutton && (BUTTON_STATUS(pCtrl) & BST_PUSHED
                            || BUTTON_STATUS(pCtrl) & BST_CHECKED)) {
                    SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_PUSHED));
                    SetBkColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_NORMAL));
                }
                else {
                    SetTextColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_NORMAL));
                    SetBkColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_BUTTON_PUSHED));
                }
                OffsetRect (prcText, 1, 1);
                DrawText (hdc, pCtrl->spCaption, -1, prcText, uFormat);
            }
        }
        break;
        
        case BS_BITMAP:
            if (BUTTON_DATA(pCtrl)) {
                int x = prcText->left;
                int y = prcText->top;
                int w = RECTWP (prcText);
                int h = RECTHP (prcText);
                PBITMAP bmp = (PBITMAP)(BUTTON_DATA(pCtrl));

                if (dwStyle & BS_REALSIZEIMAGE) {
                    x += (w - bmp->bmWidth) >> 1;
                    y += (h - bmp->bmHeight) >> 1;
                    w = h = 0;
                }
                FillBoxWithBitmap (hdc, x, y, w, h, bmp);
            }
        break;
         
        case BS_ICON:
            if (BUTTON_DATA(pCtrl)) {
                int x = prcText->left;
                int y = prcText->top;
                int w = RECTWP (prcText);
                int h = RECTHP (prcText);
                HICON icon = (HICON)(BUTTON_DATA(pCtrl));

                if (dwStyle & BS_REALSIZEIMAGE) {
                    int iconw, iconh;

                    GetIconSize (icon, &iconw, &iconh);
                    x += (w - iconw) >> 1;
                    y += (h - iconh) >> 1;
                    w = h = 0;
                }

                DrawIcon (hdc, x, y, w, h, icon);
            }
        break;
    }
}

static void btnPaintNormalButton (PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
    int bmp_left = prcBitmap->left;
    int bmp_top = prcBitmap->top + (prcBitmap->bottom - BTN_HEIGHT_BMP) / 2;

    switch (dwStyle & BS_TYPEMASK)
    {
        case BS_DEFPUSHBUTTON:
#ifndef _FLAT_WINDOW_STYLE
            myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
#else
            SetPenColor (hdc, GetWindowElementColorEx ((HWND)pCtrl, FGC_CONTROL_NORMAL));
            Rectangle (hdc, prcClient->left,
                            prcClient->top,
                            prcClient->right,
                            prcClient->bottom);
            myDrawButton (hdc, (HWND)pCtrl, prcClient->left + 1, prcClient->top + 1,
                                prcClient->right - 1, prcClient->bottom - 1, 
                                DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
#endif
        break;

        case BS_PUSHBUTTON:
            myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_NORMAL | DF_3DBOX_FILL,
                                GetWindowBkColor ((HWND)pCtrl));
        break;

        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
                break;
            }

            if (dwStyle & WS_DISABLED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top,
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       (BTN_WIDTH_BMP * 2), BTN_HEIGHT_BMP);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top,
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       0, BTN_HEIGHT_BMP);
            }
        break;

        case BS_3STATE:
        case BS_AUTO3STATE:
        case BS_AUTOCHECKBOX:
        case BS_CHECKBOX:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
                break;
            }

            if (dwStyle & WS_DISABLED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top,
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP * 2, 0);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top,
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       0, 0);
            }
        break;
/*
        case BS_OWNERDRAW:
        break;
*/
        default:
        break;
    }
}

static void btnPaintCheckedButton (PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
    int bmp_left = prcBitmap->left;
    int bmp_top = prcBitmap->top + (prcBitmap->bottom - BTN_HEIGHT_BMP) / 2;

    switch (dwStyle & BS_TYPEMASK)
    {
        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowElementColorEx ((HWND)pCtrl, WEC_3DBOX_DARK));
                break;
            }

            if (dwStyle & WS_DISABLED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       (BTN_WIDTH_BMP << 2), BTN_HEIGHT_BMP);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP);
            }
        break;

        case BS_3STATE:
        case BS_AUTO3STATE:
        case BS_AUTOCHECKBOX:
        case BS_CHECKBOX:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowElementColorEx ((HWND)pCtrl, WEC_3DBOX_DARK));
                break;
            }

            if (dwStyle & WS_DISABLED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP << 2, 0);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP, 0);
            }
        break;
/*
        case BS_DEFPUSHBUTTON:
        case BS_PUSHBUTTON:
        case BS_OWNERDRAW:
        break;
*/
        default:
        break;
   }
}

static void btnPaintInterminateButton (PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
    int bmp_left = prcBitmap->left;
    int bmp_top = prcBitmap->top + (prcBitmap->bottom - BTN_HEIGHT_BMP) / 2;

    switch (dwStyle & BS_TYPEMASK)
    {
        case BS_3STATE:
        case BS_AUTO3STATE:
            if (dwStyle & BS_PUSHLIKE) {
                break;
            }

            FillBoxWithBitmapPart (hdc, bmp_left, bmp_top,
                                   BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                   0, 0,
                                   BUTTON_BMP,
                                   BTN_WIDTH_BMP << 1, 0);
        break;

/*
        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
        case BS_AUTOCHECKBOX:
        case BS_CHECKBOX:
        case BS_DEFPUSHBUTTON:
        case BS_PUSHBUTTON:
        case BS_OWNERDRAW:
        break;
*/
        default:
        break;
    }
}

static void btnPaintPushedButton (PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
    int bmp_left = prcBitmap->left;
    int bmp_top = prcBitmap->top + (prcBitmap->bottom - BTN_HEIGHT_BMP) / 2;

    switch (dwStyle & BS_TYPEMASK)
    {
        case BS_DEFPUSHBUTTON:
        case BS_PUSHBUTTON:
            myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowBkColor ((HWND)pCtrl));
            OffsetRect (prcText, 1, 1);
        break;

        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowElementColorEx ((HWND)pCtrl, WEC_3DBOX_DARK));
                OffsetRect (prcText, 1, 1);
                break;
            }

            if (BUTTON_STATUS(pCtrl) & BST_CHECKED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP * 3, BTN_HEIGHT_BMP);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP << 1, BTN_HEIGHT_BMP);
            }
        break;

        case BS_3STATE:
        case BS_AUTO3STATE:
        case BS_AUTOCHECKBOX:
        case BS_CHECKBOX:
            if (dwStyle & BS_PUSHLIKE) {
                myDrawButton (hdc, (HWND)pCtrl, prcClient->left, prcClient->top,
                                prcClient->right, prcClient->bottom, 
                                DF_3DBOX_PRESSED | DF_3DBOX_FILL, 
                                GetWindowElementColorEx ((HWND)pCtrl, WEC_3DBOX_DARK));
                OffsetRect (prcText, 1, 1); 
                break;
            }

            if (BUTTON_STATUS (pCtrl) & BST_CHECKED) {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP * 3, 0);
            }
            else {
                FillBoxWithBitmapPart (hdc, bmp_left, bmp_top, 
                                       BTN_WIDTH_BMP, BTN_HEIGHT_BMP,
                                       0, 0,
                                       BUTTON_BMP,
                                       BTN_WIDTH_BMP << 1, 0);
            }
        break;
/*
        case BS_OWNERDRAW:
        break;
*/
        default:
        break;
   }
}

static void btnPaintFocusButton (PCONTROL pCtrl, HDC hdc, DWORD dwStyle,
                RECT* prcClient, RECT* prcText, RECT* prcBitmap)
{
    RECT rcBounds;

    SetPenColor (hdc, PIXEL_darkgray);
    switch (dwStyle & BS_TYPEMASK)
    {
        case BS_DEFPUSHBUTTON:
        case BS_PUSHBUTTON:
            FocusRect (hdc, prcClient->left + BTN_WIDTH_BORDER, 
                            prcClient->top  + BTN_WIDTH_BORDER,
                            prcClient->right - BTN_WIDTH_BORDER,
                            prcClient->bottom - BTN_WIDTH_BORDER);
            break;

        case BS_3STATE:
        case BS_AUTO3STATE:
        case BS_AUTOCHECKBOX:
        case BS_CHECKBOX:
        case BS_AUTORADIOBUTTON:
        case BS_RADIOBUTTON:
            if (dwStyle & BS_PUSHLIKE) {
                FocusRect (hdc, prcClient->left + BTN_WIDTH_BORDER, 
                                prcClient->top  + BTN_WIDTH_BORDER,
                                prcClient->right - BTN_WIDTH_BORDER,
                                prcClient->bottom - BTN_WIDTH_BORDER);
                break;
            }

            btnGetTextBoundsRect (pCtrl, hdc, dwStyle, prcText, &rcBounds);
            FocusRect (hdc, rcBounds.left - 1,
                            rcBounds.top - 1,
                            rcBounds.right + 1,
                            rcBounds.bottom + 1);
            
        break;
/*
        case BS_OWNERDRAW:
        break;
*/
        default:
        break;
   }
}

static int btnUncheckOthers (PCONTROL pCtrl)
{
    PCONTROL pGroupLeader = pCtrl;
    PCONTROL pOthers;
    DWORD    type = pCtrl->dwStyle & BS_TYPEMASK;
    
    while (pGroupLeader) {
        if (pGroupLeader->dwStyle & WS_GROUP)
            break;

        pGroupLeader = pGroupLeader->prev;
    }

    pOthers = pGroupLeader;
    while (pOthers) {
        if ((pOthers->dwStyle & BS_TYPEMASK) != type)
            break;

        if ((pOthers != pCtrl) && (BUTTON_STATUS (pOthers) | BST_CHECKED)) {
            BUTTON_STATUS (pOthers) &= ~BST_CHECKED;
            InvalidateRect ((HWND)pOthers, NULL, TRUE);
        }

        pOthers = pOthers->next;
    }

    return pCtrl->id;
}

static int ButtonCtrlProc (HWND hWnd, int uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC         hdc;
    PCONTROL    pCtrl;
    DWORD       dwStyle;
    RECT        rcClient;
    RECT        rcText;
    RECT        rcBitmap;
    PBUTTONDATA pData;

    pCtrl   = Control(hWnd);
    dwStyle = pCtrl->dwStyle;

    switch(uMsg)
    {
        case MSG_CREATE:
            pData = (BUTTONDATA*) malloc (sizeof(BUTTONDATA));
            if (pData == NULL) 
                return -1;
            memset(pData,0,sizeof(BUTTONDATA));

            pData->status = 0;
            pData->data = pCtrl->dwAddData;
            pCtrl->dwAddData2 = (DWORD) pData;

            switch (dwStyle & BS_TYPEMASK) {
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                    SetWindowBkColor (hWnd, GetWindowElementColorEx (hWnd, BKC_BUTTON_DEF));
                    break;

                default:
                    if (dwStyle & BS_CHECKED)
                        pData->status |= BST_CHECKED;
                    break;
            }

        break;
       
        case MSG_DESTROY:
            free ((void*) pCtrl->dwAddData2);
        break;
        
#ifdef _PHONE_WINDOW_STYLE
		case MSG_SIZECHANGING:
		{
			const RECT* rcExpect = (const RECT*)wParam;
			RECT* rcResult = (RECT*)lParam;

			rcResult->left = rcExpect->left;
			rcResult->top = rcExpect->top;
			rcResult->right = rcExpect->right;
			rcResult->bottom = rcExpect->top + bmp_pushbutton->bmHeight;
		    return 0;
		}

        case MSG_SIZECHANGED:
        {
            RECT* rcWin = (RECT*)wParam;
            RECT* rcClient = (RECT*)lParam;
            *rcClient = *rcWin;
            return 1;
        }
#endif

        case BM_GETCHECK:
            switch (dwStyle & BS_TYPEMASK) {
                case BS_AUTOCHECKBOX:
                case BS_AUTORADIOBUTTON:
                case BS_CHECKBOX:
                case BS_RADIOBUTTON:
                    return (BUTTON_STATUS (pCtrl) & BST_CHECKED);
                
                case BS_3STATE:
                case BS_AUTO3STATE:
                    if (BUTTON_STATUS (pCtrl) & BST_INDETERMINATE)
                        return (BST_INDETERMINATE);
                    return (BUTTON_STATUS (pCtrl) & BST_CHECKED);
                
                default:
                    return 0;
            }
        break;
        
        case BM_GETSTATE:
            return (int)(BUTTON_STATUS (pCtrl));

        case BM_GETIMAGE:
        {
            int* image_type = (int*) wParam;

            if (dwStyle & BS_BITMAP) {
                if (image_type) {
                    *image_type = BM_IMAGE_BITMAP;
                }
                return (int)(BUTTON_DATA (pCtrl));
            }
            else if (dwStyle & BS_ICON) {
                if (image_type) {
                    *image_type = BM_IMAGE_ICON;
                }
                return (int)(BUTTON_DATA (pCtrl));
            }
        }
        return 0;
        
        case BM_SETIMAGE:
        {
            int oldImage = (int)BUTTON_DATA (pCtrl);

            if (wParam == BM_IMAGE_BITMAP) {
                pCtrl->dwStyle &= ~BS_ICON;
                pCtrl->dwStyle |= BS_BITMAP;
            }
            else if (wParam == BM_IMAGE_ICON){
                pCtrl->dwStyle &= ~BS_BITMAP;
                pCtrl->dwStyle |= BS_ICON;
            }

            if (lParam) {
                 BUTTON_DATA (pCtrl) = (DWORD)lParam;
                 InvalidateRect (hWnd, NULL, TRUE);
            }

            return oldImage;
        }
        break;

        case BM_CLICK:
        {
            DWORD dwState;
            
            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
                case BS_AUTORADIOBUTTON:
                    if (!(BUTTON_STATUS (pCtrl) & BST_CHECKED))
                        SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                break;
                    
                case BS_AUTOCHECKBOX:
                    if (BUTTON_STATUS (pCtrl) & BST_CHECKED)
                        dwState = BST_UNCHECKED;
                    else
                        dwState = BST_CHECKED;
                                
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
                    
                case BS_AUTO3STATE:
                    dwState = (BUTTON_STATUS (pCtrl) & 0x00000003L);
                    dwState = BST_INDETERMINATE - dwState;
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
    
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                break;
            }
                
            NotifyParent (hWnd, pCtrl->id, BN_CLICKED);
            InvalidateRect (hWnd, NULL, TRUE);
        }
        break;
        
        case MSG_SETTEXT:
        {
            int len = strlen((char*)lParam);
            FreeFixStr (pCtrl->spCaption);
            pCtrl->spCaption = FixStrAlloc (len);
            if (len > 0)
                strcpy (pCtrl->spCaption, (char*)lParam);
            InvalidateRect (hWnd, NULL, TRUE);
            break;
        }

        case BM_SETCHECK:
        {
            DWORD dwOldState = BUTTON_STATUS (pCtrl);

            switch (dwStyle & BS_TYPEMASK) {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                    if (wParam & BST_CHECKED)
                        BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                    else
                        BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                break;
            
                case BS_AUTORADIOBUTTON:
                case BS_RADIOBUTTON:
                    if ( ((BUTTON_STATUS(pCtrl) & BST_CHECKED) == 0)
                            && (wParam & BST_CHECKED) ) {
                        BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                        
                        btnUncheckOthers (pCtrl);
                    }
                    else if ( (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                            && (!(wParam & BST_CHECKED)) ) {
                        BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                    }
                break;
            
                case BS_3STATE:
                case BS_AUTO3STATE:
                    switch (wParam) { 
                        case BST_INDETERMINATE:
                        case BST_CHECKED:
                        case BST_UNCHECKED:
                            BUTTON_STATUS(pCtrl) &= 0xFFFFFFFC;
                            BUTTON_STATUS(pCtrl) += (wParam & 0x00000003);
                    }
                break;
            }
                
            if (dwOldState != BUTTON_STATUS(pCtrl))
                InvalidateRect (hWnd, NULL, TRUE);

            return dwOldState;
        }
        
        case BM_SETSTATE:
        {
            DWORD dwOldState = BUTTON_STATUS(pCtrl) & BST_PUSHED;
            
            if (wParam)
                BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            else
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;
            
            if (dwOldState != (BUTTON_STATUS(pCtrl) & BST_PUSHED))
                InvalidateRect (hWnd, NULL, TRUE);

            return dwOldState;
        }
                
        case BM_SETSTYLE:
            pCtrl->dwStyle &= 0xFFFF0000;
            pCtrl->dwStyle |= (DWORD)(wParam & 0x0000FFFF);
            if (LOWORD (lParam))
                InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_CHAR:
            if (HIBYTE (wParam) == 0 
                    && ((dwStyle & BS_TYPEMASK) == BS_CHECKBOX
                     || (dwStyle & BS_TYPEMASK) == BS_AUTOCHECKBOX)) {
                DWORD dwOldState = BUTTON_STATUS(pCtrl);
                
                if (LOBYTE(wParam) == '+' || LOBYTE(wParam) == '=')
                    BUTTON_STATUS(pCtrl) |= BST_CHECKED;
                else if (LOBYTE(wParam) == '-')
                    BUTTON_STATUS(pCtrl) &= ~BST_CHECKED;
                    
                if (dwOldState != BUTTON_STATUS(pCtrl))
                    InvalidateRect (hWnd, NULL, TRUE);
            }
        break;
        
        case MSG_ENABLE:
            if (wParam && (dwStyle & WS_DISABLED))
                pCtrl->dwStyle &= ~WS_DISABLED;
            else if (!wParam && !(dwStyle & WS_DISABLED))
                pCtrl->dwStyle |= WS_DISABLED;
            else
                return 0;
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;

        case MSG_GETDLGCODE:
            switch (dwStyle & BS_TYPEMASK) {
                case BS_CHECKBOX:
                case BS_AUTOCHECKBOX:
                return DLGC_WANTCHARS | DLGC_BUTTON;
                
                case BS_RADIOBUTTON:
                case BS_AUTORADIOBUTTON:
                return DLGC_RADIOBUTTON | DLGC_BUTTON;
                
                case BS_DEFPUSHBUTTON:
                return DLGC_DEFPUSHBUTTON;
                
                case BS_PUSHBUTTON:
                return DLGC_PUSHBUTTON;
                
                case BS_3STATE:
                case BS_AUTO3STATE:
                return DLGC_3STATE;

                default:
                return 0;
            }
        break;

        case MSG_NCHITTEST:
        {
            int x, y;
            
            x = (int)wParam;
            y = (int)lParam;
        
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
                return HT_CLIENT;
            else  
                return HT_OUT;
        }
        break;
    
        case MSG_KILLFOCUS:
            BUTTON_STATUS(pCtrl) &= (~BST_FOCUS);
            if (GetCapture() == hWnd) {
                ReleaseCapture ();
                BUTTON_STATUS(pCtrl) &= (~BST_PUSHED);
            }

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_KILLFOCUS);

            InvalidateRect (hWnd, NULL, TRUE);
        break;

        case MSG_SETFOCUS:
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_SETFOCUS);

            InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_KEYDOWN:
            if (wParam != SCANCODE_SPACE && wParam != SCANCODE_ENTER)
                break;

            if (GetCapture () == hWnd)
                break;
            
            SetCapture (hWnd);

            BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_PUSHED);

            InvalidateRect (hWnd, NULL, TRUE);
        break;
        
        case MSG_KEYUP:
        {
            DWORD dwState;
            
            if (wParam != SCANCODE_SPACE && wParam != SCANCODE_ENTER)
                break;
                
            if (GetCapture () != hWnd)
                break;

            BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;
            ReleaseCapture ();
            
            InvalidateRect (hWnd, NULL, TRUE);

            switch (pCtrl->dwStyle & BS_TYPEMASK)
            {
                case BS_AUTORADIOBUTTON:
                    if (!(BUTTON_STATUS(pCtrl) & BST_CHECKED))
                        SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                break;
                    
                case BS_AUTOCHECKBOX:
                    if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                        dwState = BST_UNCHECKED;
                    else
                        dwState = BST_CHECKED;
                                
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
                    
                case BS_AUTO3STATE:
                    dwState = (BUTTON_STATUS(pCtrl) & 0x00000003L);
                    dwState = (dwState + 1) % 3;
                    SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                break;
    
                case BS_PUSHBUTTON:
                case BS_DEFPUSHBUTTON:
                break;
            }

            NotifyParent (hWnd, pCtrl->id, BN_CLICKED);
            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
        }
        break;
      
        case MSG_LBUTTONDBLCLK:
            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_DBLCLK);
        break;
        
        case MSG_LBUTTONDOWN:
            if (GetCapture () == hWnd)
                break;
            
            SetCapture (hWnd);
                
            BUTTON_STATUS(pCtrl) |= BST_PUSHED;
            BUTTON_STATUS(pCtrl) |= BST_FOCUS;

            if (dwStyle & BS_NOTIFY)
                NotifyParent (hWnd, pCtrl->id, BN_PUSHED);

            InvalidateRect (hWnd, NULL, TRUE);
        break;
    
        case MSG_LBUTTONUP:
        {
            int x, y;
            DWORD dwState;

            if (GetCapture() != hWnd)
                break;

            ReleaseCapture ();

            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            ScreenToClient (GetParent (hWnd), &x, &y);
            
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
            {
                switch (pCtrl->dwStyle & BS_TYPEMASK)
                {
                    case BS_AUTORADIOBUTTON:
                        if (!(BUTTON_STATUS(pCtrl) & BST_CHECKED))
                            SendMessage (hWnd, BM_SETCHECK, BST_CHECKED, 0);
                    break;
                    
                    case BS_AUTOCHECKBOX:
                        if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                            dwState = BST_UNCHECKED;
                        else
                            dwState = BST_CHECKED;
                                
                        SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                    break;
                    
                    case BS_AUTO3STATE:
                        dwState = (BUTTON_STATUS(pCtrl) & 0x00000003L);
                        dwState = (dwState + 1) % 3;
                        SendMessage (hWnd, BM_SETCHECK, (WPARAM)dwState, 0);
                    break;
    
                    case BS_PUSHBUTTON:
                    case BS_DEFPUSHBUTTON:
                    break;
                }
                
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
                NotifyParent (hWnd, pCtrl->id, BN_CLICKED);

                InvalidateRect (hWnd, NULL, TRUE);
            }
            else if (BUTTON_STATUS(pCtrl) & BST_PUSHED) {
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);

                InvalidateRect (hWnd, NULL, TRUE);
            }
        }
        return 0;
                
        case MSG_MOUSEMOVE:
        {
            int x, y;

            if (GetCapture() != hWnd)
                break;

            x = LOSWORD(lParam);
            y = HISWORD(lParam);
            ScreenToClient (GetParent (hWnd), &x, &y);
            
            if (PtInRect ((PRECT) &(pCtrl->cl), x, y))
            {
                if ( !(BUTTON_STATUS(pCtrl) & BST_PUSHED) ) {
                    BUTTON_STATUS(pCtrl) |= BST_PUSHED;
                    
                    if (dwStyle & BS_NOTIFY)
                        NotifyParent (hWnd, pCtrl->id, BN_PUSHED);
                    InvalidateRect (hWnd, NULL, TRUE);
                }
            }
            else if (BUTTON_STATUS(pCtrl) & BST_PUSHED) {
                BUTTON_STATUS(pCtrl) &= ~BST_PUSHED;

                if (dwStyle & BS_NOTIFY)
                    NotifyParent (hWnd, pCtrl->id, BN_UNPUSHED);
                InvalidateRect (hWnd, NULL, TRUE);
            }
        }
        break;
    
        case MSG_PAINT:
        {
            hdc = BeginPaint (hWnd);
            btnGetRects (hWnd, dwStyle, &rcClient, &rcText, &rcBitmap);

            if (BUTTON_STATUS(pCtrl) & BST_PUSHED)
                btnPaintPushedButton (pCtrl, hdc, dwStyle,
                    &rcClient, &rcText, &rcBitmap);
            else if (BUTTON_STATUS(pCtrl) & BST_CHECKED)
                btnPaintCheckedButton (pCtrl, hdc, dwStyle, 
                    &rcClient, &rcText, &rcBitmap);
            else if (BUTTON_STATUS(pCtrl) & BST_INDETERMINATE)
                btnPaintInterminateButton (pCtrl, hdc, dwStyle,
                    &rcClient, &rcText, &rcBitmap);
            else
                btnPaintNormalButton (pCtrl, hdc, dwStyle,
                    &rcClient, &rcText, &rcBitmap);
            
            btnPaintContent (pCtrl, hdc, dwStyle, &rcText);

            if (BUTTON_STATUS(pCtrl) & BST_FOCUS)
                btnPaintFocusButton (pCtrl, hdc, dwStyle,
                    &rcClient, &rcText, &rcBitmap);
                
            EndPaint (hWnd, hdc);
        }
        return 0;

        default:
        break;
    }
    
    return DefaultControlProc (hWnd, uMsg, wParam, lParam);
}

#endif /* _CTRL_BUTTON */

