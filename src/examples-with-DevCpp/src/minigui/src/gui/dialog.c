/*
** $Id: dialog.c,v 1.41 2004/08/12 09:35:37 snig Exp $
**
** dialog.c: The Dialog module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999.09.10
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
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"

void GUIAPI DestroyAllControls (HWND hDlg)
{
    PMAINWIN pDlg = (PMAINWIN)hDlg;
    PCONTROL pCtrl = (PCONTROL)(pDlg->hFirstChild);
    PCONTROL pNext;

    while (pCtrl) {
        pNext = pCtrl->next;
        DestroyWindow ((HWND)pCtrl);
        pCtrl = pNext;
    }
}

#define dlgDestroyAllControls   DestroyAllControls

HWND GUIAPI CreateMainWindowIndirectParam (PDLGTEMPLATE pDlgTemplate, 
                    HWND hOwner, WNDPROC WndProc, LPARAM lParam)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWin;
    int i;
    PCTRLDATA pCtrlData;
    HWND hCtrl;
    HWND hFocus;

    if (pDlgTemplate->controlnr > 0 && !pDlgTemplate->controls)
        return HWND_INVALID;

    hOwner = GetMainWindowHandle (hOwner);

    CreateInfo.dwReserved     = pDlgTemplate->dwAddData;

    CreateInfo.dwStyle        = pDlgTemplate->dwStyle & ~WS_VISIBLE;
    CreateInfo.dwExStyle      = pDlgTemplate->dwExStyle;
    CreateInfo.spCaption      = pDlgTemplate->caption;
    CreateInfo.hMenu          = pDlgTemplate->hMenu;
    CreateInfo.hCursor        = GetSystemCursor (IDC_ARROW);
    CreateInfo.hIcon          = pDlgTemplate->hIcon;
    CreateInfo.MainWindowProc = WndProc ? WndProc : DefaultMainWinProc;
    CreateInfo.lx             = pDlgTemplate->x;
    CreateInfo.ty             = pDlgTemplate->y;
    CreateInfo.rx             = pDlgTemplate->x + pDlgTemplate->w;
    CreateInfo.by             = pDlgTemplate->y + pDlgTemplate->h;
    CreateInfo.iBkColor       = GetWindowElementColor (BKC_DIALOG);
    CreateInfo.dwAddData      = pDlgTemplate->dwAddData;
    CreateInfo.hHosting       = hOwner;
    
    hMainWin = CreateMainWindow (&CreateInfo);
    if (hMainWin == HWND_INVALID)
        return HWND_INVALID;

    for (i = 0; i < pDlgTemplate->controlnr; i++) {
        pCtrlData = pDlgTemplate->controls + i;
        hCtrl = CreateWindowEx (pCtrlData->class_name,
                              pCtrlData->caption,
                              pCtrlData->dwStyle | WS_CHILD,
                              pCtrlData->dwExStyle,
                              pCtrlData->id,
                              pCtrlData->x,
                              pCtrlData->y,
                              pCtrlData->w,
                              pCtrlData->h,
                              hMainWin,
                              pCtrlData->dwAddData);
                              
        if (hCtrl == HWND_INVALID) {
            dlgDestroyAllControls (hMainWin);
            DestroyMainWindow (hMainWin);
            MainWindowThreadCleanup (hMainWin);
            return HWND_INVALID;
        }
    }

    hFocus = GetNextDlgTabItem (hMainWin, (HWND)0, FALSE);
    if (SendMessage (hMainWin, MSG_INITDIALOG, hFocus, lParam)) {
        if (hFocus)
            SetFocus (hFocus);
    }
    
    ShowWindow (hMainWin, SW_SHOWNORMAL);
    
    return hMainWin;
}

BOOL GUIAPI DestroyMainWindowIndirect (HWND hMainWin)
{
    dlgDestroyAllControls (hMainWin);
    DestroyMainWindow (hMainWin);
    ThrowAwayMessages (hMainWin);
    MainWindowThreadCleanup (hMainWin);

    return TRUE;
}

int GUIAPI DialogBoxIndirectParam (PDLGTEMPLATE pDlgTemplate, 
                    HWND hOwner, WNDPROC DlgProc, LPARAM lParam)
{
    MAINWINCREATE CreateInfo;
    HWND hDlg;
    int  retCode = IDCANCEL;
    HWND hFocus;
    MSG Msg;
    int isActive;

    if (pDlgTemplate->controlnr > 0 && !pDlgTemplate->controls)
        return -1;

    hOwner = GetMainWindowHandle (hOwner);

    //CreateInfo.dwReserved = pDlgTemplate->dwAddData;
    CreateInfo.dwReserved     = (DWORD)pDlgTemplate;
    
    CreateInfo.dwStyle        = pDlgTemplate->dwStyle & ~WS_VISIBLE;
    CreateInfo.dwExStyle      = pDlgTemplate->dwExStyle;
    CreateInfo.spCaption      = pDlgTemplate->caption;
    CreateInfo.hMenu          = pDlgTemplate->hMenu;
    CreateInfo.hCursor        = GetSystemCursor (IDC_ARROW);
    CreateInfo.hIcon          = pDlgTemplate->hIcon;
    CreateInfo.MainWindowProc = DlgProc;
    CreateInfo.lx             = pDlgTemplate->x;
    CreateInfo.ty             = pDlgTemplate->y;
    CreateInfo.rx             = pDlgTemplate->x + pDlgTemplate->w;
    CreateInfo.by             = pDlgTemplate->y + pDlgTemplate->h;
    CreateInfo.iBkColor       = GetWindowElementColor (BKC_DIALOG);
    //CreateInfo.dwAddData      = (DWORD)pDlgTemplate;
    CreateInfo.dwAddData      = (DWORD)pDlgTemplate->dwAddData;
    CreateInfo.hHosting       = hOwner;
    
    hDlg = CreateMainWindow (&CreateInfo);
    if (hDlg == HWND_INVALID)
        return -1;

    SetWindowAdditionalData2 (hDlg, (DWORD)(&retCode));

    if (hOwner)
        EnableWindow (hOwner, FALSE);
    
    hFocus = GetNextDlgTabItem (hDlg, (HWND)0, FALSE);
    if (SendMessage (hDlg, MSG_INITDIALOG, hFocus, lParam)) {
        if (hFocus)
            SetFocus (hFocus);
    }
    
    ShowWindow (hDlg, SW_SHOWNORMAL);
    
    while( GetMessage (&Msg, hDlg) ) {
        TranslateMessage (&Msg);
        DispatchMessage (&Msg);
    }

    isActive = (GetActiveWindow() == hDlg);

    dlgDestroyAllControls (hDlg);
    DestroyMainWindow (hDlg);
    ThrowAwayMessages (hDlg);
    MainWindowThreadCleanup (hDlg);
    
    if (hOwner) {
        EnableWindow (hOwner, TRUE);
        if(isActive)
        {
            ShowWindow (hOwner, SW_SHOWNORMAL);
            SetActiveWindow (hOwner);
        }
    }

    return retCode;
}

BOOL GUIAPI EndDialog (HWND hDlg, int endCode)
{
    DWORD dwAddData2;

    dwAddData2 = GetWindowAdditionalData2 (hDlg);

    if (dwAddData2 == 0)
        return FALSE;

    *((int *)(dwAddData2)) = endCode;

    PostQuitMessage (hDlg);

    return TRUE;
}

int GUIAPI DefaultDialogProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HWND hCurFocus;

    switch (message) {
    case MSG_CREATE:
    {
        int i;
        PCTRLDATA pCtrlData;
        HWND hCtrl;
            
        /*
        PDLGTEMPLATE pDlgTmpl 
                    = (PDLGTEMPLATE)(((PMAINWINCREATE)lParam)->dwAddData);
        */
        PDLGTEMPLATE pDlgTmpl 
                    = (PDLGTEMPLATE)(((PMAINWINCREATE)lParam)->dwReserved);
            
        for (i = 0; i < pDlgTmpl->controlnr; i++) {
            pCtrlData = pDlgTmpl->controls + i;
            if (pCtrlData->class_name) {
                hCtrl = CreateWindowEx (pCtrlData->class_name,
                              pCtrlData->caption,
                              pCtrlData->dwStyle | WS_CHILD,
                              pCtrlData->dwExStyle,
                              pCtrlData->id,
                              pCtrlData->x,
                              pCtrlData->y,
                              pCtrlData->w,
                              pCtrlData->h,
                              hWnd,
                              pCtrlData->dwAddData);
            }
            else
                break;
                              
            if (hCtrl == HWND_INVALID) {
                dlgDestroyAllControls (hWnd);
                return -1;
            }
        }

        /* set back dwAddData to be the value from dialog template */
        //SetWindowAdditionalData (hWnd, pDlgTmpl->dwAddData);
        return 0;
    }

    case MSG_DLG_GETDEFID:
    {
        HWND hDef;

        hDef = GetDlgDefPushButton (hWnd);
        if (hDef)
            return GetDlgCtrlID (hDef);
        return 0;
    }
    
    case MSG_DLG_SETDEFID:
    {
        HWND hOldDef;
        HWND hNewDef;

        hNewDef = GetDlgItem (hWnd, wParam);
        if (SendMessage (hNewDef, MSG_GETDLGCODE, 0, 0L) & DLGC_PUSHBUTTON) {
            hOldDef = GetDlgDefPushButton (hWnd);
            if (hOldDef) {
                ExcludeWindowStyle (hOldDef, BS_DEFPUSHBUTTON);
                InvalidateRect (hOldDef, NULL, TRUE);
            }
            IncludeWindowStyle (hNewDef, BS_DEFPUSHBUTTON);
            InvalidateRect (hNewDef, NULL, TRUE);

            return (int)hOldDef;
        }
        break;
    }
        
    case MSG_COMMAND:
        if (wParam == IDCANCEL) {
            HWND hCancel;
            
            hCancel = GetDlgItem (hWnd, IDCANCEL);
            if (hCancel && IsWindowEnabled (hCancel) 
                    && IsWindowVisible (hCancel))
                EndDialog (hWnd, IDCANCEL);
            else
                Ping ();
        }
        break;

    case MSG_CLOSE:
    {
        HWND hCancel;

        hCancel = GetDlgItem (hWnd, IDCANCEL);
        if (hCancel && IsWindowEnabled (hCancel) 
                    && IsWindowVisible (hCancel))
            EndDialog (hWnd, IDCANCEL);
        else
            Ping ();

        return 0;
    }

    case MSG_KEYDOWN:
        if ((hCurFocus = GetFocusChild (hWnd)) 
                && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_WANTALLKEYS)
            break;

        switch (wParam) {
        case SCANCODE_ESCAPE:
            SendMessage (hWnd, MSG_COMMAND, IDCANCEL, 0L);
            return 0;

        case SCANCODE_TAB:
        {
            HWND hNewFocus;
                
            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_WANTTAB)
                break;

            if (lParam & KS_SHIFT)
                hNewFocus = GetNextDlgTabItem (hWnd, hCurFocus, TRUE);
            else
                hNewFocus = GetNextDlgTabItem (hWnd, hCurFocus, FALSE);

            if (hNewFocus != hCurFocus) {
                SetFocus (hNewFocus);
//                SendMessage (hWnd, MSG_DLG_SETDEFID, GetDlgCtrlID (hNewFocus), 0L);
            }

            return 0;
        }

        case SCANCODE_ENTER:
        {
            HWND hDef;

            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_WANTENTER)
                break;

            if (SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_PUSHBUTTON)
                break;

            hDef = GetDlgDefPushButton (hWnd);
            if (hDef) {
                SendMessage (hWnd, MSG_COMMAND, GetDlgCtrlID (hDef), 0L);
                return 0;
            }
            break;
        }

        case SCANCODE_CURSORBLOCKDOWN:
        case SCANCODE_CURSORBLOCKRIGHT:
        case SCANCODE_CURSORBLOCKUP:
        case SCANCODE_CURSORBLOCKLEFT:
        {
            HWND hNewFocus;
                
            if (hCurFocus && SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_WANTARROWS)
                break;

            if (LOWORD (wParam) == SCANCODE_CURSORBLOCKDOWN
                    || LOWORD (wParam) == SCANCODE_CURSORBLOCKRIGHT)
                hNewFocus = GetNextDlgGroupItem (hWnd, hCurFocus, FALSE);
            else
                hNewFocus = GetNextDlgGroupItem (hWnd, hCurFocus, TRUE);
            
            if (hNewFocus != hCurFocus) {

                if (SendMessage (hCurFocus, MSG_GETDLGCODE, 0, 0L) & DLGC_STATIC)
                    return 0;

                SetFocus (hNewFocus);
//                SendMessage (hWnd, MSG_DLG_SETDEFID, GetDlgCtrlID (hNewFocus), 0L);

                if (SendMessage (hNewFocus, MSG_GETDLGCODE, 0, 0L)
                        & DLGC_RADIOBUTTON) {
                    SendMessage (hNewFocus, BM_CLICK, 0, 0L);
                    ExcludeWindowStyle (hCurFocus, WS_TABSTOP);
                    IncludeWindowStyle (hNewFocus, WS_TABSTOP);
                }
            }

            return 0;
        }
        }
        break;

    default:
        break;
    }
    
    return DefaultMainWinProc (hWnd, message, wParam, lParam);
}

int GUIAPI GetDlgCtrlID (HWND hwndCtl)
{
    PCONTROL pCtrl = (PCONTROL)hwndCtl;

    if (pCtrl) {
        return pCtrl->id;
    }

    return -1;
}

HWND GUIAPI GetDlgItem (HWND hDlg, int nIDDlgItem)
{
    PCONTROL pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    pCtrl = (PCONTROL)(pMainWin->hFirstChild);

    while (pCtrl) {
        if (pCtrl->id == nIDDlgItem)
            return (HWND)pCtrl;

        pCtrl = pCtrl->next;
    }
   
    return 0;
}

HWND GUIAPI GetDlgDefPushButton (HWND hWnd)
{
    PCONTROL pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hWnd;

    pCtrl = (PCONTROL)(pMainWin->hFirstChild);

    while (pCtrl) {
        if (SendMessage ((HWND)pCtrl, 
                MSG_GETDLGCODE, 0, 0L) & DLGC_DEFPUSHBUTTON)
            return (HWND)pCtrl;

        pCtrl = pCtrl->next;
    }
   
    return 0;
}

HWND GetNextDlgGroupItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
{
    PCONTROL pStartCtrl, pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    if (hCtl)
        pStartCtrl = (PCONTROL)hCtl;
    else {
        pStartCtrl = (PCONTROL)(pMainWin->hFirstChild);
        if (bPrevious) {
            while (pStartCtrl->next) {
                pStartCtrl = pStartCtrl->next;
            }
        }
    }

    if (bPrevious) {
        if (pStartCtrl->dwStyle & WS_GROUP) {
            pCtrl = pStartCtrl->next;
            while (pCtrl && pCtrl->next) {
                if (pCtrl->dwStyle & WS_GROUP)
                    break;
                pCtrl = pCtrl->next;
            }
        }
        else
            pCtrl = pStartCtrl;
        
        if (pCtrl->dwStyle & WS_GROUP || pCtrl == pStartCtrl)
            pCtrl = pCtrl->prev; 

        while (pCtrl) {
            if (pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
    }
    else {
        pCtrl = pStartCtrl->next;
        while (pCtrl) {
            if ( !(pCtrl->dwStyle & WS_GROUP) ) {
                if (pCtrl->dwStyle & WS_VISIBLE
                        && !(pCtrl->dwStyle & WS_DISABLED) )
                    return (HWND)pCtrl;
            }
            else
                break;

            pCtrl = pCtrl->next;
        }
        
        pCtrl = pStartCtrl->prev;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_GROUP) {
                if (pCtrl->dwStyle & WS_VISIBLE
                        && !(pCtrl->dwStyle & WS_DISABLED) )
                    return (HWND)pCtrl;
                else
                    break;
            }

            pCtrl = pCtrl->prev;
        }
        
        if (pCtrl) {        // pCtrl is the first control in group.
            pCtrl = pCtrl->next;
            while (pCtrl) {
                if ( !(pCtrl->dwStyle & WS_GROUP) ) {
                    if (pCtrl->dwStyle & WS_VISIBLE
                            && !(pCtrl->dwStyle & WS_DISABLED) )
                        return (HWND)pCtrl;
                }
                else
                    break;

                pCtrl = pCtrl->next;
            }
        }
    }

    return hCtl;
}

HWND GUIAPI GetNextDlgTabItem (HWND hDlg, HWND hCtl, BOOL bPrevious)
{
    PCONTROL pFirstCtrl, pLastCtrl, pCtrl;
    PMAINWIN pMainWin = (PMAINWIN)hDlg;

    pFirstCtrl = pCtrl = (PCONTROL)(pMainWin->hFirstChild);
    if (!pCtrl) return hCtl;

    while (pCtrl->next) {
        pCtrl = pCtrl->next;
    }
    pLastCtrl = pCtrl;
    
    if (hCtl) {
        pCtrl = (PCONTROL)hCtl;
        if (bPrevious)
            pCtrl = pCtrl->prev;
        else
            pCtrl = pCtrl->next;
    }
    else if (bPrevious)
        pCtrl = pLastCtrl;
    else
        pCtrl = pFirstCtrl;

    if (bPrevious) {
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
        
        pCtrl = pLastCtrl;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->prev;
        }
    }
    else {
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->next;
        }

        pCtrl = pFirstCtrl;
        while (pCtrl) {
            if (pCtrl->dwStyle & WS_TABSTOP 
                    && pCtrl->dwStyle & WS_VISIBLE
                    && !(pCtrl->dwStyle & WS_DISABLED) )
                return (HWND)pCtrl;

            pCtrl = pCtrl->next;
        }
    }

    return hCtl;
}

int GUIAPI SendDlgItemMessage (HWND hDlg, int nIDDlgItem, 
            int message, WPARAM wParam, LPARAM lParam)
{
    HWND hCtrl;

    hCtrl = GetDlgItem (hDlg, nIDDlgItem);

    if (hCtrl)
        return SendMessage (hCtrl, message, wParam, lParam);

    return -1;
}

UINT GUIAPI GetDlgItemInt (HWND hDlg, int nIDDlgItem, BOOL *lpTranslated,
                    BOOL bSigned)
{
    HWND hCtrl;
    char buffer [101];
    unsigned long int result;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        goto error;

    if (SendMessage (hCtrl, MSG_GETTEXT, 100, (LPARAM)buffer) == 0)
        goto error;

    if (bSigned)
        result = strtoul (buffer, NULL, 0);
    else
        result = (unsigned long int)strtol (buffer, NULL, 0);
        
    if (lpTranslated)
        *lpTranslated = TRUE;

    return (UINT)result;
    
error:
    if (lpTranslated)
        *lpTranslated = FALSE;
    return 0;
}

BOOL GUIAPI SetDlgItemInt (HWND hDlg, int nIDDlgItem, UINT uValue, 
                    BOOL bSigned)
{
    HWND hCtrl;
    char buffer [101];

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return FALSE;

    if (bSigned)
        snprintf (buffer, 100, "%ld", (long)uValue);
    else
        snprintf (buffer, 100, "%u", uValue);
    buffer [100] = '\0';

    return SendMessage (hCtrl, MSG_SETTEXT, 0, (LPARAM)buffer) == 0;
}

int GUIAPI GetDlgItemText (HWND hDlg, int nIDDlgItem, char* lpString, 
                    int nMaxCount)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return 0;

    return SendMessage (hCtrl, 
            MSG_GETTEXT, (WPARAM)nMaxCount, (LPARAM)lpString);
}

char* GUIAPI GetDlgItemText2 (HWND hDlg, int id, int* lenPtr)
{
    int len;
    char* buff;

    len = SendDlgItemMessage (hDlg, id,
                        MSG_GETTEXTLENGTH, 0, 0L);
    if (len > 0) {
        buff = malloc (len + 1);
        if (buff)
           SendDlgItemMessage (hDlg, id,
               MSG_GETTEXT, len, (LPARAM)(buff));
    }
    else
        buff = NULL;

    if (lenPtr) *lenPtr = len;

    return buff;
}

BOOL GUIAPI SetDlgItemText (HWND hDlg, int nIDDlgItem, const char* lpString)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return FALSE;

    return SendMessage (hCtrl, 
            MSG_SETTEXT, 0, (LPARAM)lpString) == 0;
}

#ifdef _CTRL_BUTTON
void GUIAPI CheckDlgButton (HWND hDlg, int nIDDlgItem, int nCheck)
{
    HWND hCtrl;
    int DlgCode;

    if ( !(hCtrl = GetDlgItem (hDlg, nIDDlgItem)))
        return;
    
    DlgCode = SendMessage (hCtrl, MSG_GETDLGCODE, 0, 0L);

    if (DlgCode & DLGC_BUTTON)
        SendMessage (hCtrl, BM_SETCHECK, nCheck?BST_CHECKED:BST_UNCHECKED, 0L);
    else if (DlgCode & DLGC_3STATE) {
        SendMessage (hCtrl, BM_SETCHECK, nCheck, 0L);
    }
}

void GUIAPI CheckRadioButton (HWND hDlg, 
                int idFirstButton, int idLastButton, int idCheckButton)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, idCheckButton)))
        return;
        
    SendMessage (hCtrl, BM_SETCHECK, BST_CHECKED, 0L);
}

int GUIAPI IsDlgButtonChecked (HWND hDlg, int idButton)
{
    HWND hCtrl;

    if ( !(hCtrl = GetDlgItem (hDlg, idButton)))
        return -1;
    
    return SendMessage (hCtrl, BM_GETCHECK, 0, 0L);
}

#ifdef _CTRL_STATIC
static int MsgBoxProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case MSG_INITDIALOG:
    {
        HWND hFocus = GetDlgDefPushButton (hWnd);
        if (hFocus)
             SetFocus (hFocus);

        SetWindowAdditionalData (hWnd, (DWORD)lParam);
        return 0;
    }

    case MSG_COMMAND:
    {
        switch (wParam) {
        case IDOK:
        case IDCANCEL:
        case IDABORT:
        case IDRETRY:
        case IDIGNORE:
        case IDYES:
        case IDNO:
            if (GetDlgItem (hWnd, wParam))
                EndDialog (hWnd, wParam);
            break;
        }
        break;
    }

    case MSG_CHAR:
    {
        int id = 0;
        
        if (HIBYTE (wParam))
            break;
        switch (LOBYTE (wParam)) {
        case 'Y':
        case 'y':
            id = IDYES;
            break;
        case 'N':
        case 'n':
            id = IDNO;
            break;
        case 'A':
        case 'a':
            id = IDABORT;
            break;
        case 'R':
        case 'r':
            id = IDRETRY;
            break;
        case 'I':
        case 'i':
            id = IDIGNORE;
            break;
        }
        
        if (id != 0 && GetDlgItem (hWnd, id))
            EndDialog (hWnd, id);
        break;
    }

    case MSG_CLOSE:
        if (GetDlgItem (hWnd, IDCANCEL)) {
            EndDialog (hWnd, IDCANCEL);
        }
        else if (GetDlgItem (hWnd, IDIGNORE)) {
            EndDialog (hWnd, IDIGNORE);
        }
        else if (GetDlgItem (hWnd, IDNO)) {
            EndDialog (hWnd, IDNO);
        }
        else if (GetDlgItem (hWnd, IDOK)) {
            EndDialog (hWnd, IDOK);
        }
        break;

    default:
        break;
    }

    return DefaultDialogProc (hWnd, message, wParam, lParam);
}

#ifdef _TINY_SCREEN

#define MB_MARGIN    2
#define MB_BUTTONW   40
#define MB_BUTTONH   20
#define MB_TEXTW     120

#else

#define MB_MARGIN    10
#define MB_BUTTONW   80
#define MB_BUTTONH   26
#define MB_TEXTW     300

#endif

static void get_box_xy (HWND hParentWnd, DWORD dwStyle, DLGTEMPLATE* MsgBoxData)
{
    RECT rcTemp;

    if (dwStyle & MB_BASEDONPARENT) {
        GetWindowRect (hParentWnd, &rcTemp);
    }
    else {
        rcTemp = g_rcDesktop;
    }

    switch (dwStyle & MB_ALIGNMASK) {
        case MB_ALIGNCENTER:
            MsgBoxData->x = rcTemp.left + (RECTW(rcTemp) - MsgBoxData->w)/2;
            MsgBoxData->y = rcTemp.top + (RECTH(rcTemp) - MsgBoxData->h)/2;
            break;

        case MB_ALIGNTOPLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMLEFT:
            MsgBoxData->x = rcTemp.left;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;

        case MB_ALIGNTOPRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.top;
            break;

        case MB_ALIGNBTMRIGHT:
            MsgBoxData->x = rcTemp.right - MsgBoxData->w;
            MsgBoxData->y = rcTemp.bottom - MsgBoxData->h;
            break;
    }
}

int GUIAPI MessageBox (HWND hParentWnd, const char* pszText, 
                      const char* pszCaption, DWORD dwStyle)
{
    DLGTEMPLATE MsgBoxData = 
    {
        WS_ABSSCRPOS | WS_CAPTION | WS_BORDER, WS_EX_NONE, 0, 0, 0, 0, NULL, 0, 0, 0, NULL, 0L
    };
    CTRLDATA     CtrlData [5] = 
    {
        {"button", 
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE | WS_GROUP, 
            0, 0, 0, 0, 0, NULL, 0L},
        {"button", 
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE, 
            0, 0, 0, 0, 0, NULL, 0L},
        {"button",
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE, 
            0, 0, 0, 0, 0, NULL, 0L}
    };

    int i, nButtons, buttonx;
    RECT rcText, rcButtons, rcIcon;
    int width, height;

    if (pszCaption)
        MsgBoxData.caption  = pszCaption;
    else
        MsgBoxData.caption  = "MiniGUI";

    switch (dwStyle & MB_TYPEMASK) {
    case MB_OK:
        MsgBoxData.controlnr = 1;
        CtrlData [0].caption = GetSysText (SysText[10]);
        CtrlData [0].id      = IDOK;
        break;
    case MB_OKCANCEL:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = GetSysText (SysText[10]);
        CtrlData [0].id      = IDOK;
        CtrlData [1].caption = (dwStyle & MB_CANCELASBACK) ? 
                        GetSysText (SysText[13]) : GetSysText (SysText[12]);
        CtrlData [1].id      = IDCANCEL;
        break;
    case MB_YESNO:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = GetSysText (SysText[14]);
        CtrlData [0].id      = IDYES;
        CtrlData [1].caption = GetSysText (SysText[15]);
        CtrlData [1].id      = IDNO;
        break;
    case MB_RETRYCANCEL:
        MsgBoxData.controlnr = 2;
        CtrlData [0].caption = GetSysText (SysText[17]);
        CtrlData [0].id      = IDRETRY;
        CtrlData [1].caption = (dwStyle & MB_CANCELASBACK) ? 
                        GetSysText (SysText[13]) : GetSysText (SysText[12]);
        CtrlData [1].id      = IDCANCEL;
        break;
    case MB_ABORTRETRYIGNORE:
        MsgBoxData.controlnr = 3;
        CtrlData [0].caption = GetSysText (SysText[16]);
        CtrlData [0].id      = IDABORT;
        CtrlData [1].caption = GetSysText (SysText[17]);
        CtrlData [1].id      = IDRETRY;
        CtrlData [2].caption = GetSysText (SysText[18]);
        CtrlData [2].id      = IDIGNORE;
        break;
    case MB_YESNOCANCEL:
        MsgBoxData.controlnr = 3;
        CtrlData [0].caption = GetSysText (SysText[14]);
        CtrlData [0].id      = IDYES;
        CtrlData [1].caption = GetSysText (SysText[15]);
        CtrlData [1].id      = IDNO;
        CtrlData [2].caption = (dwStyle & MB_CANCELASBACK) ?
                        GetSysText (SysText[13]) : GetSysText (SysText[12]);
        CtrlData [2].id      = IDCANCEL;
        break;
    }

    switch (dwStyle & MB_DEFMASK) {
    case MB_DEFBUTTON1:
        CtrlData [0].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    case MB_DEFBUTTON2:
        if (MsgBoxData.controlnr > 1)
            CtrlData [1].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    case MB_DEFBUTTON3:
        if (MsgBoxData.controlnr > 2)
            CtrlData [2].dwStyle |= BS_DEFPUSHBUTTON;
        break;
    }

    nButtons = MsgBoxData.controlnr;
    rcButtons.left   = 0;
    rcButtons.top    = 0;
    rcButtons.bottom = MB_BUTTONH;
    rcButtons.right  = MsgBoxData.controlnr * MB_BUTTONW + 
                (MsgBoxData.controlnr - 1) * (MB_MARGIN << 1);

    rcIcon.left   = 0;
    rcIcon.top    = 0;
    rcIcon.right  = 0;
    rcIcon.bottom = 0;
    if (dwStyle & MB_ICONMASK) {
        int id_icon = -1;
        i = MsgBoxData.controlnr;
        CtrlData [i].class_name= "static";
        CtrlData [i].dwStyle   = WS_VISIBLE | SS_ICON | WS_GROUP;
        CtrlData [i].x         = MB_MARGIN;
        CtrlData [i].y         = MB_MARGIN;
#ifdef _TINY_SCREEN
        CtrlData [i].w         = 16;
        CtrlData [i].h         = 16;
#else
        CtrlData [i].w         = 32;
        CtrlData [i].h         = 32;
#endif
        CtrlData [i].id        = IDC_STATIC;
        CtrlData [i].caption   = "Hello";
        switch (dwStyle & MB_ICONMASK) {
        case MB_ICONSTOP:
            id_icon = IDI_STOP;
            break;
        case MB_ICONINFORMATION:
            id_icon = IDI_INFORMATION;
            break;
        case MB_ICONEXCLAMATION:
            id_icon = IDI_EXCLAMATION;
            break;
        case MB_ICONQUESTION:
            id_icon = IDI_QUESTION;
            break;
        }

#ifdef _TINY_SCREEN
        if (id_icon != -1) {
            CtrlData [i].dwAddData = GetSmallSystemIcon (id_icon);
        }
        rcIcon.right  = 16;
        rcIcon.bottom = 16;
#else
        if (id_icon != -1) {
            CtrlData [i].dwAddData = GetLargeSystemIcon (id_icon);
            MsgBoxData.hIcon       = GetSmallSystemIcon (id_icon);
        }
        rcIcon.right  = 32;
        rcIcon.bottom = 32;
#endif

        MsgBoxData.controlnr ++;
    }

    rcText.left = 0;
    rcText.top  = 0;
    rcText.right = rcButtons.right + (MB_MARGIN << 1);
    rcText.right = MAX (rcText.right, MB_TEXTW);
    rcText.bottom = GetSysCharHeight ();

    SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_CONTROL));
    DrawText (HDC_SCREEN, pszText, -1, &rcText, 
                DT_LEFT | DT_TOP | DT_WORDBREAK | DT_EXPANDTABS | DT_CALCRECT);
#ifdef _TINY_SCREEN
    rcText.right = MAX (rcText.right, MB_TEXTW);
#endif
    
    i = MsgBoxData.controlnr;
    CtrlData [i].class_name= "static";
    CtrlData [i].dwStyle   = WS_VISIBLE | SS_LEFT | WS_GROUP;
    CtrlData [i].x         = RECTW (rcIcon) + (MB_MARGIN << 1);
    CtrlData [i].y         = MB_MARGIN;
    CtrlData [i].w         = RECTW (rcText);
    CtrlData [i].h         = RECTH (rcText);
    CtrlData [i].id        = IDC_STATIC;
    CtrlData [i].caption   = pszText;
    CtrlData [i].dwAddData = 0;
    MsgBoxData.controlnr ++;

    width = MAX (RECTW (rcText), RECTW (rcButtons)) + RECTW (rcIcon)
                + (MB_MARGIN << 2)
                + (GetMainWinMetrics(MWM_BORDER) << 1);
    height = MAX (RECTH (rcText), RECTH (rcIcon)) + RECTH (rcButtons)
                + MB_MARGIN + (MB_MARGIN << 1) 
                + (GetMainWinMetrics (MWM_BORDER) << 1) 
                + GetMainWinMetrics (MWM_CAPTIONY);
    
    buttonx = (width - RECTW (rcButtons)) >> 1;
    for (i = 0; i < nButtons; i++) {
        CtrlData[i].x = buttonx + i*(MB_BUTTONW + MB_MARGIN);
        CtrlData[i].y = MAX (RECTH (rcIcon), RECTH (rcText)) + (MB_MARGIN<<1);
        CtrlData[i].w = MB_BUTTONW;
        CtrlData[i].h = MB_BUTTONH;
    }

    MsgBoxData.w = width;
    MsgBoxData.h = height;
    get_box_xy (hParentWnd, dwStyle, &MsgBoxData);

    MsgBoxData.controls = CtrlData;

    return DialogBoxIndirectParam (&MsgBoxData, hParentWnd, MsgBoxProc, 
                (LPARAM)dwStyle);
}

#endif /* _CTRL_STATIC */
#endif /* _CTRL_BUTTON */

void GUIAPI MessageBeep (DWORD dwBeep)
{
    Beep ();
}


