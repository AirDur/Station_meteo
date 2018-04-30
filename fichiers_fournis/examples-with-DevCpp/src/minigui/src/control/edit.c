/*
** $Id: edit.c,v 1.105 2005/01/31 09:12:48 clear Exp $
**
** edit.c: the Single Line Edit Control module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
** 
** Current maintainer: Wei Yongming.
**
** Note:
**  Although there was a version by Zhao Jianghua, this version of
**  EDIT control is written by Wei Yongming from scratch.
**
** Create date: 1999/8/26
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
**    * Replace
**    * Undo.
**/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/ctrlhelper.h"
#include "ctrl/edit.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "clipboard.h"

#ifdef _CTRL_SLEDIT

#include "ctrlmisc.h"
#include "text.h"
#include "edit_impl.h"


static int SLEditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#define check_caret() \
            if(sled->selStart != sled->selEnd) \
                HideCaret(hWnd); \
            else \
                ShowCaret(hWnd);


#define shift_one_char_r(pos) \
        { \
            int len = CHLENNEXT( (sled->content.string + sled->pos), (sled->content.txtlen - sled->pos)); \
            sled->pos += len; \
        }

#define shift_one_char_l(pos) \
        { \
            int len = CHLENPREV( sled->content.string, (sled->content.string + sled->pos) ); \
            sled->pos -= len; \
        }


BOOL RegisterSLEditControl (void)
{
    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_SLEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementColor (BKC_EDIT_DEF);
    WndClass.WinProc     = SLEditCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

/* -------------------------------------------------------------------------- */

static void setup_dc (HWND hWnd, SLEDITDATA *sled, HDC hdc, BOOL bSel)
{
    DWORD dwStyle = GetWindowStyle (hWnd);
    if (!bSel) {
        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DISABLED));
        else
            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL));
        SetBkMode (hdc, BM_TRANSPARENT);
    }
    else {
        if (dwStyle & WS_DISABLED)
            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_HILIGHT_DISABLED));
        else
            SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_HILIGHT_NORMAL));
        if (sled->status & EST_FOCUSED)
            SetBkColor (hdc, GetWindowElementColorEx (hWnd, BKC_HILIGHT_NORMAL));
        else
            SetBkColor (hdc, GetWindowElementColorEx (hWnd, BKC_HILIGHT_LOSTFOCUS));
        SetBkMode (hdc, BM_OPAQUE);
    }
}

/* -------------------------------------------------------------------------- */

static int sledit_settext (PSLEDITDATA sled, const char *newtext)
{
    int len, txtlen;

    txtlen = strlen (newtext);
    len = (txtlen <= 0) ? sled->nBlockSize : txtlen;

    if (sled->hardLimit >= 0) {
        len = MIN (len, sled->hardLimit);
        txtlen = MIN (txtlen, sled->hardLimit);
    }
 
    /* free the old text */
    if (sled->content.string)
        testr_free (&sled->content);

    if (!testr_alloc (&sled->content, len, sled->nBlockSize))
        return -1;

    if (newtext && txtlen > 0)
        testr_setstr (&sled->content, newtext, txtlen);
    else
        sled->content.txtlen = 0;

    return 0;
}

static void get_content_width (HWND hWnd, PSLEDITDATA sled)
{
    GetClientRect (hWnd, &sled->rcCont);

    sled->rcCont.left += sled->leftMargin;
    sled->rcCont.top += sled->topMargin;
    sled->rcCont.right -= sled->rightMargin;
    sled->rcCont.bottom -= sled->bottomMargin;

    sled->starty  = sled->topMargin + ( sled->rcCont.bottom - 
                    sled->rcCont.top - GetWindowFont (hWnd)->size - 1 ) / 2;
}

static int sledit_init (HWND hWnd, PSLEDITDATA sled)
{
    if (!sled)
        return -1;

    SetWindowAdditionalData2 (hWnd,(DWORD)sled);

    sled->status = 0;
    sled->editPos = 0;
    sled->selStart = 0;
    sled->selEnd = 0;

    sled->leftMargin     = MARGIN_EDIT_LEFT;
    sled->topMargin      = MARGIN_EDIT_TOP;
    sled->rightMargin    = MARGIN_EDIT_RIGHT;
    sled->bottomMargin   = MARGIN_EDIT_BOTTOM;
    get_content_width (hWnd, sled);

    sled->nContX = 0;
    sled->nContW = sled->rcCont.right - sled->rcCont.left;

    sled->passwdChar     = '*';

    sled->nBlockSize = DEF_LINE_BLOCK_SIZE;
    sled->hardLimit      = -1;
    
    if (GetWindowStyle(hWnd) & ES_TIP) {
        sled->tiptext = FixStrAlloc (DEF_TIP_LEN + 1);
        sled->tiptext[0] = 0;
    }
    else
        sled->tiptext = NULL;

    sled->content.string = NULL;
    sled->content.buffsize = 0;
    sled->content.txtlen = 0;

    sledit_settext (sled, GetWindowCaption(hWnd));

    CreateCaret (hWnd, NULL, 1, GetWindowFont (hWnd)->size);
    SetCaretPos (hWnd, sled->leftMargin, sled->starty);

    return 0;
}

static void sledit_destroy (HWND hWnd, PSLEDITDATA sled)
{
    DestroyCaret (hWnd);
    if ( (GetWindowStyle(hWnd) & ES_TIP) && sled->tiptext)
        FreeFixStr (sled->tiptext);
    testr_free (&sled->content);
}

static void slePaint (HWND hWnd, HDC hdc, PSLEDITDATA sled)
{
    char*   dispBuffer, *passwdBuffer = NULL;
    DWORD   dwStyle = GetWindowStyle(hWnd);
    StrBuffer *content = &sled->content;
    int starty = sled->starty;
    int outw = 0;
   
    if (dwStyle & ES_TIP && content->txtlen <= 0 && 
                    GetFocus(GetParent(hWnd)) != hWnd) {
        setup_dc (hWnd, sled, hdc, FALSE);
        TextOut (hdc, sled->leftMargin, starty, 
                        sled->tiptext);
        return;
    }

    if (dwStyle & ES_PASSWORD) {
        dispBuffer = FixStrAlloc (content->txtlen);
        memset (dispBuffer, sled->passwdChar, content->txtlen);
        passwdBuffer = dispBuffer;
    }
    else {
        dispBuffer = content->string;
    }

    if (dwStyle & ES_BASELINE) {
        SetPenColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_NORMAL));
#ifdef _PHONE_WINDOW_STYLE
        MoveTo (hdc, sled->leftMargin, sled->rcCont.bottom);
        LineTo (hdc, sled->rcCont.right, sled->rcCont.bottom);
#else
        DrawHDotLine (hdc, 
                    sled->leftMargin, 
                    sled->rcCont.bottom,
                    sled->rcCont.right - sled->rcCont.left);
#endif
    }

    ClipRectIntersect (hdc, &sled->rcCont);

    if (sled->selStart != sled->selEnd) {//select chars
        int startx = sled->leftMargin - sled->nContX;
        /* draw first normal chars */
        if (sled->selStart > 0) {
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += TextOutLen (hdc, startx, starty,
                        dispBuffer, sled->selStart);
            dispBuffer += sled->selStart;
        }

        /* draw selected chars */
        setup_dc (hWnd, sled, hdc, TRUE);
        outw += TextOutLen (hdc, startx + outw, starty, dispBuffer, 
                        sled->selEnd - sled->selStart);
        dispBuffer += sled->selEnd - sled->selStart;
        
        /* draw others */
        if (sled->selEnd < content->txtlen) {
            setup_dc (hWnd, sled, hdc, FALSE);
            outw += TextOutLen (hdc, startx + outw, starty, dispBuffer, content->txtlen - sled->selEnd);
        }
    }
    else {
        setup_dc (hWnd, sled, hdc, FALSE);
        outw += TextOutLen (hdc, sled->leftMargin - sled->nContX, starty,
                        dispBuffer, content->txtlen);
    }

    sled->nContW = outw;
    if (sled->nContW < sled->rcCont.right - sled->rcCont.left)
        sled->nContW = sled->rcCont.right - sled->rcCont.left;

    if (dwStyle & ES_PASSWORD)
        FreeFixStr (passwdBuffer);
}

static int sleSetSel (HWND hWnd, PSLEDITDATA sled, int sel_start, int sel_end)
{
    if (sled->content.txtlen <= 0)
        return -1;

    if (sel_start < 0)
        sel_start = 0;
    if (sel_end < 0)
        sel_end = sled->content.txtlen;
    if (sel_start == sel_end)
        return -1;

    sled->selStart = sel_start;
    sled->selEnd = sel_end;

    HideCaret(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    return sled->selEnd - sled->selStart;
}

static void set_caret_pos (HWND hWnd, PSLEDITDATA sled, int x, BOOL bSel)
{
    int out_chars;
    HDC hdc;
    SIZE txtsize;
    
    hdc = GetClientDC (hWnd);

    if (x + sled->nContX <= 0) {
        out_chars = 0;
        txtsize.cx = 0;
    }
    else
        out_chars = GetTextExtentPoint (hdc, sled->content.string, sled->content.txtlen, 
                        x + sled->nContX, NULL, NULL, NULL, &txtsize);
    if (!bSel) {
        sled->selStart = sled->selEnd = 0;
        sled->editPos = out_chars;
        SetCaretPos (hWnd, txtsize.cx - sled->nContX, sled->starty);
    }
    else {
        if (out_chars > sled->editPos) {
            sled->selStart = sled->editPos;
            sled->selEnd = out_chars;
        }
        else {
            sled->selEnd = sled->editPos;
            sled->selStart = out_chars;
        }
    }

    ReleaseDC (hdc);
}

static BOOL make_pos_visible (HWND hWnd, PSLEDITDATA sled, int x)
{
    if (x - sled->nContX > sled->rcCont.right - sled->rcCont.left) {
        //FIXME
        sled->nContX = x - (sled->rcCont.right - sled->rcCont.left - 3);
        return TRUE;
    }
    else if (x < sled->nContX) {
        sled->nContX = x;
        return TRUE;
    }
    return FALSE;
}

static BOOL make_charpos_visible (HWND hWnd, PSLEDITDATA sled, int charPos, int *cx)
{
    SIZE txtsize;
    HDC hdc;

    if (charPos <= 0)
        txtsize.cx = 0;
    else {
        hdc = GetClientDC (hWnd);
        GetTextExtent (hdc, sled->content.string, charPos, &txtsize);
        ReleaseDC (hdc);
    }
    if (cx)
        *cx = txtsize.cx;
    return make_pos_visible (hWnd, sled, txtsize.cx);
}

static BOOL edtSetCaretPos (HWND hWnd, PSLEDITDATA sled)
{
    BOOL bRefresh;
    int cx;

    bRefresh = make_charpos_visible (hWnd, sled, sled->editPos, &cx);
    SetCaretPos (hWnd, sled->leftMargin + cx - sled->nContX, sled->starty);
    if (bRefresh)
        InvalidateRect (hWnd, NULL, TRUE);
    return bRefresh;
}

/*
static void get_cont_x (HWND hWnd, PSLEDITDATA sled)
{
    HDC hdc;
    SIZE txtsize;

    hdc = GetClientDC (hWnd);
    if (sled->startPos == 0)
        sled->nContX = 0;
    else {
        GetTextExtent (hdc, sled->content.string, sled->startPos, &txtsize);
        sled->nContX = txtsize.cx;
    }
    ReleaseDC (hdc);
}
*/

static int sleMouseMove (HWND hWnd, PSLEDITDATA sled, LPARAM lParam)
{
    int mouseX, mouseY;
    RECT rcClient;
    BOOL refresh = TRUE;

    mouseX = LOSWORD(lParam);
    mouseY = HISWORD(lParam);

    ScreenToClient(hWnd, &mouseX, &mouseY);
    GetClientRect(hWnd, &rcClient);
    
    if(mouseX >= 0 && mouseX < rcClient.right
       && mouseY >= 0 && mouseY < rcClient.bottom) {//in edit window
        set_caret_pos (hWnd, sled, mouseX, TRUE);
        if (sled->editPos == sled->selStart)
            make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
        else
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
    }
    else if (mouseY < 0) {
        sled->nContX = 0;
        sled->selStart = 0;
        sled->selEnd = sled->editPos;
    }
    else if(mouseY >= rcClient.bottom) {
        sled->selStart = sled->editPos;
        sled->selEnd = sled->content.txtlen;
        sled->nContX = sled->nContW - (sled->rcCont.right - sled->rcCont.left); 
    }
    else if (mouseX < 0) {
        if (sled->selEnd == sled->content.txtlen) {
            set_caret_pos (hWnd, sled, 0, TRUE);
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            goto quit;
        }

        if (sled->selStart == 0)
            return 0;

        shift_one_char_l (selStart);
        refresh = make_charpos_visible (hWnd, sled, sled->selStart, NULL);
    }
    else if (mouseX >= rcClient.right) {
        if (sled->selStart == 0) {
            set_caret_pos (hWnd, sled, rcClient.right - 1, TRUE);
            //make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
            //goto quit;
        }

        if (sled->selEnd == sled->content.txtlen)
            return 0;

        shift_one_char_r (selEnd);
        refresh = make_charpos_visible (hWnd, sled, sled->selEnd, NULL);
    }
 
quit:
    check_caret();

    if (refresh)
        InvalidateRect(hWnd, NULL, TRUE);

    return 0;
}

static int
sleInsertText (HWND hWnd, PSLEDITDATA sled, const char *newtext, int inserting)
{
    int  deleted;
    unsigned char *pIns, *content = sled->content.string;
            
    //delete the seleted
    if(sled->selStart != sled->selEnd) {
        deleted = sled->selEnd - sled->selStart;
        memmove (content + sled->selStart, content + sled->selEnd,
                        sled->content.txtlen - sled->selEnd);
        sled->content.txtlen -= deleted;

        sled->editPos = sled->selStart;
        sled->selEnd = sled->selStart;
        ShowCaret(hWnd);

        if (!newtext)
            inserting = 0;
    }

    if ( !(sled->status & EST_REPLACE) && inserting > 0 && sled->hardLimit >= 0 && 
                    ((sled->content.txtlen + inserting) > sled->hardLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->content.txtlen;
        //return 0;
    }
    else if ( (sled->status & EST_REPLACE) && inserting > 0 && sled->hardLimit >= 0
               && (sled->editPos + inserting > sled->hardLimit) ) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        inserting = sled->hardLimit - sled->editPos;
    }

    if (inserting > 0) {
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                  inserting);
        pIns = content + sled->editPos;
        if ( !(sled->status & EST_REPLACE) )
            memmove (pIns + inserting, pIns, sled->content.txtlen+1 - sled->editPos);
        memcpy (pIns, newtext, inserting);
    }
    else if (inserting < 0) {
        pIns = content + sled->editPos;
        memmove (pIns + inserting, pIns, sled->content.txtlen+1 - sled->editPos);
        content = testr_realloc (&sled->content, sled->content.txtlen + 
                                 inserting);
    }

    if (inserting <= 0 || !(sled->status & EST_REPLACE))
        sled->content.txtlen += inserting;
    else {
        int add_len = inserting - sled->content.txtlen + sled->editPos;
        if (add_len > 0) {
            sled->content.txtlen += add_len;
            sled->content.string[sled->content.txtlen] = '\0';
        }
    }
    sled->editPos += inserting;
    sled->selStart = sled->selEnd = sled->editPos;

    if (!edtSetCaretPos (hWnd, sled))
        InvalidateRect (hWnd, NULL, TRUE);
        InvalidateRect (hWnd, NULL, TRUE);
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    return 0;
}

static int sleInsertCbText (HWND hWnd, PSLEDITDATA sled)
{
    int  inserting;
    unsigned char *txtBuffer;

    if (GetWindowStyle(hWnd) & ES_READONLY) {
        return 0;
    }

    inserting = GetClipBoardDataLen (CBNAME_TEXT);
    txtBuffer = ALLOCATE_LOCAL (inserting);
    GetClipBoardData (CBNAME_TEXT, txtBuffer, inserting);
    sleInsertText (hWnd, sled, txtBuffer, inserting);
    DEALLOCATE_LOCAL(txtBuffer);

    return 0;
}

static int sleKeyDown (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    BOOL    bChange = FALSE;
    DWORD   dwStyle = GetWindowStyle (hWnd);
    PCONTROL pCtrl   = Control(hWnd);
    PSLEDITDATA sled = (PSLEDITDATA) (pCtrl->dwAddData2);

    switch (LOWORD (wParam)) {

    case SCANCODE_ENTER:
        NotifyParent (hWnd, pCtrl->id, EN_ENTER);
        return 0;

    case SCANCODE_HOME:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            make_pos_visible (hWnd, sled, 0);
            sled->selStart = 0;
            sled->selEnd = sled->editPos;
            check_caret();
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }
            
        if (sled->selStart != sled->selEnd) {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        sled->editPos  = 0;
        sled->selStart = sled->selEnd = 0;

        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);

        return 0;
    }
   
    case SCANCODE_END:
    {
        BOOL refresh = FALSE;
       
        if(lParam & KS_SHIFT) {
            sled->selStart = sled->editPos;
            sled->selEnd = sled->content.txtlen;
            check_caret();
            if (make_charpos_visible (hWnd, sled, sled->selEnd, NULL))
                InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret(hWnd);
            refresh = TRUE;
        }

        sled->editPos = sled->content.txtlen;
        sled->selStart = sled->selEnd = sled->editPos;

        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);

        return 0;
    }

    case SCANCODE_CURSORBLOCKLEFT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            if (sled->selStart == sled->selEnd) {
                sled->selStart = sled->selEnd = sled->editPos;
            }

            if (sled->selStart < sled->editPos ||
                        sled->selStart == sled->selEnd) {
                if (sled->selStart > 0)
                    shift_one_char_l(selStart);
            }
            else {
                shift_one_char_l(selEnd);
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }

        if (sled->editPos > 0) {
            shift_one_char_l (editPos);
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        sled->selStart = sled->selEnd = sled->editPos;
        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);

        return 0;
    }
    
    case SCANCODE_CURSORBLOCKRIGHT:
    {
        BOOL refresh = FALSE;

        if(lParam & KS_SHIFT) {
            if(sled->selStart == sled->selEnd)
                sled->selStart = sled->selEnd = sled->editPos;

            if (sled->selStart == sled->selEnd || 
                            sled->selEnd > sled->editPos) {
                if (sled->selEnd < sled->content.txtlen)
                    shift_one_char_r (selEnd);
            }
            else {
                shift_one_char_r (selStart);
            }

            check_caret ();
            make_charpos_visible (hWnd, sled, sled->selStart, NULL);
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }

        if (sled->editPos < sled->content.txtlen) {
            shift_one_char_r (editPos);
        }

        if (sled->selStart != sled->selEnd) {
            ShowCaret (hWnd);
            ActiveCaret (hWnd);
            refresh = TRUE;
        }

        sled->selStart = sled->selEnd = sled->editPos;
        if (!edtSetCaretPos (hWnd, sled) && refresh)
            InvalidateRect (hWnd, NULL, TRUE);
    }
    return 0;
    
    case SCANCODE_REMOVE:
    {
        int oldpos = sled->editPos;

        if ((dwStyle & ES_READONLY))
            return 0;
        if (sled->editPos == sled->content.txtlen && sled->selStart == sled->selEnd)
            return 0;

        if (sled->selStart == sled->selEnd && 
                        sled->editPos < sled->content.txtlen) {
            shift_one_char_r(editPos);
        }

        sleInsertText (hWnd, sled, NULL, oldpos - sled->editPos);
    }
    break;

/*
    case SCANCODE_BACKSPACE:
    {
        int del;

        if (dwStyle & ES_READONLY)
            return 0;
        if (sled->editPos == 0 && sled->selStart == sled->selEnd)
            return 0;

        del = - CHLENPREV( sled->content.string, (sled->content.string + sled->editPos) );
        sleInsertText (hWnd, sled, NULL, del);
    }
    break;
*/

    case SCANCODE_A:
    {
        if (lParam & KS_CTRL) {
            sleSetSel (hWnd, sled, 0, sled->content.txtlen);
        }
        return 0;
    }

#ifdef _CLIPBOARD_SUPPORT
    case SCANCODE_C:
    case SCANCODE_X:
    {
        if (lParam & KS_CTRL && sled->selEnd - sled->selStart > 0) {
            SetClipBoardData (CBNAME_TEXT, sled->content.string + sled->selStart, 
                        sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (wParam == SCANCODE_X && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0);
            }
        }
        return 0;
    }

    case SCANCODE_V:
    {
        if (!(lParam & KS_CTRL))
            return 0;
        return sleInsertCbText (hWnd, sled);
    }
#endif /* _CLIPBOARD_SUPPORT */

    case SCANCODE_INSERT:
        sled->status ^= EST_REPLACE;
        break;

    default:
        break;
    } //end switch
       
    if (bChange)
        NotifyParent (hWnd, pCtrl->id, EN_CHANGE);

    return 0;
}

static int SLEditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{   
    DWORD       dwStyle;
    HDC         hdc;
    PSLEDITDATA sled = NULL;

    dwStyle     = GetWindowStyle(hWnd);

    if (message != MSG_CREATE)
        sled = (PSLEDITDATA) GetWindowAdditionalData2 (hWnd);

    switch (message) {

    case MSG_CREATE:
        if ( !(sled = malloc (sizeof (SLEDITDATA))) )
            return -1;
        
        memset(sled,0,sizeof (SLEDITDATA));
        
        if (sledit_init (hWnd, sled) < 0)
            return -1;
        break;

    case MSG_DESTROY:
        sledit_destroy (hWnd, sled);
        free (sled);
        break;

    case MSG_ERASEBKGND:
        EditOnEraseBackground (hWnd, (HDC)wParam, (const RECT*)lParam);
        return 0;

    case MSG_PAINT:
    {
        hdc = BeginPaint (hWnd);
        slePaint (hWnd, hdc, sled);
        EndPaint (hWnd, hdc);
        return 0;
    }

    case MSG_FONTCHANGING:
        return 0;

    case MSG_FONTCHANGED:
    {
        int starty;

        starty  = sled->topMargin + ( sled->rcCont.bottom - sled->rcCont.top - 
                    GetWindowFont (hWnd)->size - 1 ) / 2;

        sled->editPos = 0;

        DestroyCaret (hWnd);
        CreateCaret (hWnd, NULL, 1, GetWindowFont (hWnd)->size);
        SetCaretPos (hWnd, sled->leftMargin, starty);
        InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }

    case MSG_SETCURSOR:
        if (dwStyle & WS_DISABLED) {
            SetCursor (GetSystemCursor (IDC_ARROW));
            return 0;
        }
        break;

    case MSG_KILLFOCUS:
        if (sled->status & EST_FOCUSED) {
            BOOL refresh = FALSE;

            sled->status &= ~EST_FOCUSED;
            HideCaret (hWnd);
            if ( sled->selStart != sled->selEnd || (dwStyle & ES_TIP && sled->content.txtlen <= 0)) {
                refresh = TRUE;
            }
            if (sled->selStart != sled->selEnd && !(dwStyle & ES_NOHIDESEL))
                sled->selStart = sled->selEnd = sled->editPos;

            if (refresh)
                InvalidateRect (hWnd, NULL, TRUE);

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_KILLFOCUS);
        }
        break;
 
    case MSG_SETFOCUS:
        if (sled->status & EST_FOCUSED)
            break;
        
        sled->status |= EST_FOCUSED;

        ActiveCaret (hWnd);
        ShowCaret(hWnd);

        if (dwStyle & ES_AUTOSELECT) {
            if (sleSetSel (hWnd, sled, 0, sled->content.txtlen) <= 0 && dwStyle & ES_TIP)
                InvalidateRect (hWnd, NULL, TRUE);
            if (lParam == 1)
                sled->status |= EST_TMP;
        }
        else if ( (dwStyle & ES_NOHIDESEL && sled->selStart != sled->selEnd)
                    || (dwStyle & ES_TIP && sled->content.txtlen <= 0)) {
            InvalidateRect (hWnd, NULL, TRUE);
        }

        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SETFOCUS);
        break;
        
    case MSG_ENABLE:
        if ( (!(dwStyle & WS_DISABLED) && !wParam)
                || ((dwStyle & WS_DISABLED) && wParam) ) {
            if (wParam)
                ExcludeWindowStyle(hWnd,WS_DISABLED);
            else
                IncludeWindowStyle(hWnd,WS_DISABLED);

            InvalidateRect (hWnd, NULL, TRUE);
        }
        return 0;

    case MSG_GETTEXTLENGTH:
        return sled->content.txtlen;
    
    case MSG_GETTEXT:
    {
        char*   buffer = (char*)lParam;
        int     len;

        len = MIN ((int)wParam, sled->content.txtlen);
        memcpy (buffer, sled->content.string, len);
        buffer [len] = '\0';
        return len;
    }

    case MSG_SETTEXT:
    {
        if (lParam == 0)
            return -1;

        sledit_settext (sled, (const char*)lParam);

        sled->selStart = sled->selEnd = 0;

        if ( wParam == 1 && !(GetWindowStyle(hWnd) & ES_READONLY) ) {
            sled->editPos  = sled->content.txtlen;
            sled->selStart = sled->selEnd = sled->editPos;
            edtSetCaretPos (hWnd, sled);
        }
        else {
            sled->editPos  = 0;
            SetCaretPos (hWnd, sled->leftMargin, sled->starty);
        }

        InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }

    case MSG_KEYDOWN:
    {
        return sleKeyDown (hWnd, wParam, lParam);
    }

    case MSG_CHAR:
    {
        unsigned char charBuffer [2];
        int chars;

        if (wParam == 127) // BS
            wParam = '\b';

        if (dwStyle & ES_READONLY)
            return 0;
            
        if (HIBYTE (wParam)) {
            charBuffer [0] = LOBYTE (wParam);
            charBuffer [1] = HIBYTE (wParam);
            chars = 2;
        }
        else {
            chars = 1;

            if (dwStyle & ES_UPPERCASE) {
                charBuffer [0] = toupper (LOBYTE (wParam));
            }
            else if (dwStyle & ES_LOWERCASE) {
                charBuffer [0] = tolower (LOBYTE (wParam));
            }
            else
                charBuffer [0] = LOBYTE (wParam);
        }
    
        if (chars == 1) {
            if (charBuffer [0] < 0x20 && charBuffer[0] != '\b') //FIXME
                return 0;
        }

        if (wParam == '\b') { //backspace
            int del;
            if (sled->editPos == 0 && sled->selStart == sled->selEnd)
                return 0;

            del = - CHLENPREV( sled->content.string, (sled->content.string + sled->editPos) );
            sleInsertText (hWnd, sled, NULL, del);
        }
        else {
            sleInsertText (hWnd, sled, charBuffer, chars);
        }

        return 0;
    }

    case MSG_LBUTTONDBLCLK:
        sleSetSel (hWnd, sled, 0, sled->content.txtlen);
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_DBLCLK);
        break;
        
    case MSG_LBUTTONDOWN:
    {
        if ( sled->status & EST_TMP) {
            sled->status &= ~EST_TMP;
            break;
        }

        if (HISWORD(lParam) < sled->rcCont.top || HISWORD(lParam) > sled->rcCont.bottom)
            break;

        set_caret_pos (hWnd, sled, LOSWORD(lParam), FALSE);

        ActiveCaret (hWnd);
        ShowCaret(hWnd);
        SetCapture(hWnd);

        InvalidateRect(hWnd, NULL, TRUE);
        NotifyParent (hWnd,  GetDlgCtrlID(hWnd), EN_CLICKED);

        break;
    }
 
    case MSG_NCLBUTTONUP:
    case MSG_LBUTTONUP:
        if (GetCapture() == hWnd)
            ReleaseCapture();
        break;

    case MSG_MOUSEMOVE:
    {
        if (GetCapture () == hWnd)
            sleMouseMove (hWnd, sled, lParam);
        return 0;
    }
        
    case MSG_GETDLGCODE:
        return DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS | DLGC_WANTENTER;

    case MSG_DOESNEEDIME:
        if (dwStyle & ES_READONLY)
            return FALSE;
        return TRUE;
    
    case EM_GETCARETPOS:
    case EM_GETSELPOS:
    {
        int nr_chars, pos;
        int* line_pos = (int *)wParam;
        int* char_pos = (int *)lParam;

        if (message == EM_GETSELPOS)
            pos = (sled->editPos == sled->selStart) ? sled->selEnd : sled->selStart;
        else
            pos = sled->editPos;
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd), 
                                     sled->content.string, pos, NULL);

        if (line_pos) *line_pos = 0;
        if (char_pos) *char_pos = nr_chars;

        return pos;
    }

    case EM_SETCARETPOS:
    case EM_SETSEL:
    {
        int char_pos = (int)lParam;
        int nr_chars, pos;
        int *pos_chars;

        if (char_pos < 0)
            return -1;

        pos_chars = ALLOCATE_LOCAL (sled->content.txtlen * sizeof(int));
        nr_chars = GetTextMCharInfo (GetWindowFont (hWnd), 
                        sled->content.string, sled->content.txtlen, pos_chars);

        if (char_pos > nr_chars) {
            DEALLOCATE_LOCAL (pos_chars);
            return -1;
        }

        if (char_pos == nr_chars)
            pos = sled->content.txtlen;
        else
            pos = pos_chars[char_pos];

        DEALLOCATE_LOCAL (pos_chars);

        if (message == EM_SETCARETPOS) {
            sled->editPos = pos;
            sled->selStart = sled->selEnd = 0;
            edtSetCaretPos (hWnd, sled);
            return sled->editPos;
        }
        else {
            int start, end;
            if (sled->editPos < pos) {
                start = sled->editPos;
                end = pos;
            }
            else {
                start = pos;
                end = sled->editPos;
            }
            return sleSetSel (hWnd, sled, start, end);
        }
    }

    case EM_SETREADONLY:
        if (wParam)
            IncludeWindowStyle (hWnd, ES_READONLY);
        else
            ExcludeWindowStyle (hWnd, ES_READONLY);
        return 0;
 
    case EM_SETPASSWORDCHAR:
        if (sled->passwdChar != (int)wParam) {
            if (dwStyle & ES_PASSWORD) {
                sled->passwdChar = (int)wParam;
                InvalidateRect (hWnd, NULL, TRUE);
            }
        }
        return 0;
 
    case EM_GETPASSWORDCHAR:
    {
        if (lParam)
            *((int*)lParam) = sled->passwdChar;
        return 0;
    }
    
    case EM_LIMITTEXT:
    {
        int newLimit = (int)wParam;
        
        if (newLimit >= 0) {
            if (sled->content.buffsize < newLimit)
                sled->hardLimit = -1;
            else
                sled->hardLimit = newLimit;
        }
        return 0;
    }
    
    case EM_GETTIPTEXT:
    {
        int len, tip_len;
        char *buffer = (char *)lParam;

        if (!(dwStyle & ES_TIP))
            return -1;

        tip_len = strlen (sled->tiptext);

        if (!buffer)
            return tip_len;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;

        strncpy (buffer, sled->tiptext, len);
        buffer[len] = '\0';

        return tip_len;
    }

    case EM_SETTIPTEXT:
    {
        int len;

        if (!(dwStyle & ES_TIP) || !lParam)
            return -1;

        if (wParam >= 0)
            len = (wParam > DEF_TIP_LEN) ? DEF_TIP_LEN : wParam;
        else
            len = DEF_TIP_LEN;
        strncpy (sled->tiptext, (char *)lParam, len);
        sled->tiptext[len] = '\0';

        if (sled->content.txtlen <= 0)
            InvalidateRect (hWnd, NULL, TRUE);

        return strlen(sled->tiptext);
    }

    case EM_GETSEL:
    {
        char *buffer = (char *)lParam;
        int len;

        if (!buffer || sled->selEnd - sled->selStart <= 0)
            return 0;

        if (wParam < 0)
            len = sled->selEnd - sled->selStart;
        else
            len = MIN(sled->selEnd - sled->selStart, (int)wParam);

        strncpy (buffer, sled->content.string + sled->selStart, len); 

        return len;
    }

    case EM_INSERTCBTEXT:
    {
        return sleInsertCbText (hWnd, sled);
    }

    case EM_COPYTOCB:
    case EM_CUTTOCB:
    {
        if (sled->selEnd - sled->selStart > 0) {
            SetClipBoardData (CBNAME_TEXT, sled->content.string + sled->selStart, 
                        sled->selEnd - sled->selStart, CBOP_NORMAL);
            if (message == EM_CUTTOCB && !(GetWindowStyle(hWnd) & ES_READONLY)) {
                sleInsertText (hWnd, sled, NULL, 0);
            }
            return sled->selEnd - sled->selStart;
        }
        return 0;
    }

    default:
        break;

    } // end switch

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _CTRL_SLEDIT */
