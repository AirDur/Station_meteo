/*
** $Id: ctrlclass.c,v 1.27 2004/04/15 09:04:35 snig Exp $
**
** ctrlclass.c: the Control Class module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
** Create date: 1999/5/21
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
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _CTRL_STATIC
extern BOOL RegisterStaticControl (void);
#endif
#ifdef _CTRL_BUTTON
extern BOOL RegisterButtonControl (void);
#endif
#ifdef _CTRL_SIMEDIT
extern BOOL RegisterSIMEditControl (void);
#endif
#ifdef _CTRL_SLEDIT
extern BOOL RegisterSLEditControl (void);
#endif
#ifdef _CTRL_PROGRESSBAR
extern BOOL RegisterProgressBarControl (void);
#endif
#ifdef _CTRL_LISTBOX
extern BOOL RegisterListboxControl (void);
#endif
#ifdef _CTRL_OLDMEDIT
extern BOOL RegisterMLEditControl (void);
#endif
#ifdef _CTRL_TOOLBAR
extern BOOL RegisterToolbarControl (void);
#endif
#ifdef _CTRL_NEWTOOLBAR
extern BOOL RegisterNewToolbarControl (void);
#endif
#ifdef _CTRL_MENUBUTTON
extern BOOL RegisterMenuButtonControl (void);
#endif
#ifdef _CTRL_TRACKBAR
extern BOOL RegisterTrackBarControl (void);
#endif
#ifdef _CTRL_COMBOBOX
extern BOOL RegisterComboBoxControl (void);
#endif
#ifdef _CTRL_PROPSHEET
extern BOOL RegisterPropSheetControl (void);
#endif
#ifdef _CTRL_SCROLLVIEW
extern BOOL RegisterScrollViewControl (void);
extern BOOL RegisterScrollWndControl (void);
#endif
#ifdef _CTRL_TEXTEDIT
extern BOOL RegisterTextEditControl (void);
#endif

#define LEN_CCITABLE    26

PCTRLCLASSINFO ccitable[LEN_CCITABLE];

PCONTROL Control (HWND hWnd)
{
    PCONTROL pCtrl;

    pCtrl = (PCONTROL) hWnd;

    if (pCtrl && pCtrl->WinType == TYPE_CONTROL)
        return pCtrl;

    return NULL;
}

BOOL InitControlClass ()
{
    int i;
    
    for (i=0; i<LEN_CCITABLE; i++)
        ccitable[i] = NULL;

    // Register system controls here.
#ifdef _CTRL_STATIC
    if (!RegisterStaticControl ())
        return FALSE;
#endif

#ifdef _CTRL_BUTTON
    if (!RegisterButtonControl())
        return FALSE;
#endif

#ifdef _CTRL_SIMEDIT
    if (!RegisterSIMEditControl())
        return FALSE;
#endif

#ifdef _CTRL_SLEDIT
    if (!RegisterSLEditControl())
        return FALSE;
#endif

#ifdef _CTRL_PROGRESSBAR
    if (!RegisterProgressBarControl())
        return FALSE;
#endif

#ifdef _CTRL_LISTBOX
    if (!RegisterListboxControl())
        return FALSE;
#endif

#ifdef _CTRL_MLEDIT
    if (!RegisterMLEditControl())
        return FALSE;
#endif

#ifdef _CTRL_TOOLBAR
    if (!RegisterToolbarControl())
        return FALSE;
#endif

#ifdef _CTRL_NEWTOOLBAR
    if (!RegisterNewToolbarControl())
        return FALSE;
#endif

#ifdef _CTRL_MENUBUTTON
    if (!RegisterMenuButtonControl())
        return FALSE;
#endif

#ifdef _CTRL_TRACKBAR
    if (!RegisterTrackBarControl())
        return FALSE;
#endif

#ifdef _CTRL_COMBOBOX
    if (!RegisterComboBoxControl())
        return FALSE;
#endif

#ifdef _CTRL_PROPSHEET
    if (!RegisterPropSheetControl())
        return FALSE;
#endif

#ifdef _CTRL_SCROLLVIEW
    if (!RegisterScrollViewControl ())
        return FALSE;
    if (!RegisterScrollWndControl ())
        return FALSE;
#endif

#ifdef _CTRL_TEXTEDIT
    if (!RegisterTextEditControl ())
        return FALSE;
#endif

    return TRUE;
}

void TerminateControlClass ()
{
#if 0

#ifdef _CTRL_STATIC
    StaticControlCleanup ();
#endif

#ifdef _CTRL_BUTTON
    ButtonControlCleanup ();
#endif

#ifdef _CTRL_SIMEDIT
    SIMEditControlCleanup ();
#endif

#ifdef _CTRL_SLEDIT
    SLEditControlCleanup ();
#endif

#ifdef _CTRL_PROGRESSBAR
    ProgressBarControlCleanup ();
#endif

#ifdef _CTRL_LISTBOX
    ListboxControlCleanup ();
#endif

#ifdef _CTRL_MLEDIT
    MLEditControlCleanup ();
#endif

#ifdef _CTRL_TOOLBAR
    ToolbarControlCleanup ();
#endif

#ifdef _CTRL_NEWTOOLBAR
    NewToolbarControlCleanup ();
#endif

#ifdef _CTRL_MENUBUTTON
    MenuButtonControlCleanup ();
#endif

#ifdef _CTRL_TRACKBAR
    TrackBarControlCleanup ();
#endif

#ifdef _CTRL_COMBOBOX
    ComboBoxControlCleanup ();
#endif

#endif

    EmptyControlClassInfoTable ();
}

PCTRLCLASSINFO GetControlClassInfo (const char* szClassName)
{
    PCTRLCLASSINFO cci;
    int i=0;
    char szName [MAXLEN_CLASSNAME + 1];

    if (szClassName == NULL) return NULL;

    strncpy (szName, szClassName, MAXLEN_CLASSNAME);

    if (!isalpha (szName[0])) return NULL;
    
    while (szName[i]) {
        szName[i] = toupper(szName[i]);

        i++;
    }
    
    cci = ccitable [szName[0] - 'A'];

    while (cci) {
    
        if (strcmp (cci->name, szName) == 0)
            break;

        cci = cci->next;
    }
    
    return cci;
}

int ControlClassDataOp (int Operation, PWNDCLASS pWndClass)
{
    PCTRLCLASSINFO cci;

    cci = GetControlClassInfo (pWndClass->spClassName);

    if (!cci)
        return ERR_CTRLCLASS_INVNAME;

    if (Operation ==  CCDOP_GETCCI) {
        if (pWndClass->opMask & COP_STYLE) {
            pWndClass->dwStyle      = cci->dwStyle;
            pWndClass->dwExStyle    = cci->dwExStyle;
        }
        if (pWndClass->opMask & COP_HCURSOR)
            pWndClass->hCursor      = cci->hCursor;
        if (pWndClass->opMask & COP_BKCOLOR)
            pWndClass->iBkColor     = cci->iBkColor;
        if (pWndClass->opMask & COP_WINPROC)
            pWndClass->WinProc      = cci->ControlProc;
        if (pWndClass->opMask & COP_ADDDATA)
            pWndClass->dwAddData    = cci->dwAddData;
    }
    else {
        if (pWndClass->opMask & COP_STYLE) {
            cci->dwStyle            = pWndClass->dwStyle;
            cci->dwExStyle          = pWndClass->dwExStyle;
        }
        if (pWndClass->opMask & COP_HCURSOR)
            cci->hCursor            = pWndClass->hCursor;
        if (pWndClass->opMask & COP_BKCOLOR)
            cci->iBkColor           = pWndClass->iBkColor;
        if (pWndClass->opMask & COP_WINPROC)
            cci->ControlProc        = pWndClass->WinProc;
        if (pWndClass->opMask & COP_ADDDATA)
            cci->dwAddData          = pWndClass->dwAddData;
    }

    return ERR_OK;
}

int GetCtrlClassAddData (const char* szClassName, DWORD* pAddData)
{
    PCTRLCLASSINFO cci;

    cci = GetControlClassInfo (szClassName);

    if (cci) {
        *pAddData = cci->dwAddData;
        return ERR_OK;
    }
    
    return ERR_CTRLCLASS_INVNAME;
}

int SetCtrlClassAddData (const char* szClassName, DWORD dwAddData)
{
    PCTRLCLASSINFO cci;

    cci = GetControlClassInfo (szClassName);

    if (cci) {
        cci->dwAddData = dwAddData;
        return ERR_OK;
    }
    
    return ERR_CTRLCLASS_INVNAME;
}

int AddNewControlClass (PWNDCLASS pWndClass)
{
    PCTRLCLASSINFO cci, newcci;
    char szClassName [MAXLEN_CLASSNAME + 2];
    int i=0;

    strncpy (szClassName, pWndClass->spClassName, MAXLEN_CLASSNAME + 1);

    if (!isalpha (szClassName[0])) return ERR_CTRLCLASS_INVNAME;
    
    while (szClassName[i]) {
        szClassName[i] = toupper(szClassName[i]);

        i++;
        if (i > MAXLEN_CLASSNAME)
            return ERR_CTRLCLASS_INVLEN;
    }
    
    i = szClassName[0] - 'A';
    cci = ccitable [i];
    if (cci) {

        while (cci) {
            if (strcmp (szClassName, cci->name) == 0)
                return ERR_CTRLCLASS_INVNAME;

            cci = cci->next;
        }
    }
    cci = ccitable[i];

    newcci = malloc (sizeof (CTRLCLASSINFO));
    
    if (newcci == NULL) return ERR_CTRLCLASS_MEM;

    newcci->next = NULL;
    strcpy (newcci->name, szClassName);
    newcci->dwStyle     = pWndClass->dwStyle;
    newcci->dwExStyle   = pWndClass->dwExStyle;
    newcci->hCursor     = pWndClass->hCursor;
    newcci->iBkColor    = pWndClass->iBkColor;
    newcci->ControlProc = pWndClass->WinProc;
    newcci->dwAddData   = pWndClass->dwAddData;
    newcci->nUseCount   = 0;

    if (cci) {
        while (cci->next)
            cci = cci->next;

        cci->next = newcci;
    }
    else
        ccitable [i] = newcci;
    
    return ERR_OK;
}

int DeleteControlClass (const char* szClassName)
{
    PCTRLCLASSINFO head, cci, prev;
    int i=0;
    char szName [MAXLEN_CLASSNAME + 1];

    if (szClassName == NULL) return ERR_CTRLCLASS_INVNAME;
    
    strncpy (szName, szClassName, MAXLEN_CLASSNAME);

    if (!isalpha (szName[0])) return ERR_CTRLCLASS_INVNAME;
    
    while (szName[i]) {
        szName[i] = toupper(szName[i]);

        i++;
    }
    
    i = szName[0] - 'A';
    head = ccitable [i];
    
    cci = head;
    prev = head;
    while (cci) {
    
        if (strcmp (cci->name, szName) == 0)
            break;

        prev = cci;
        cci = cci->next;
    }

    if (!cci)
        return ERR_CTRLCLASS_INVNAME;

    if (cci->nUseCount != 0)
        return ERR_CTRLCLASS_INVNAME;

    if (cci == head) {
        ccitable [i] = cci->next;
        free (cci);
    }
    else {
        prev->next = cci->next;
        free (cci);
    }

    return ERR_OK;
}

void EmptyControlClassInfoTable ()
{
    PCTRLCLASSINFO cci, temp;
    int i;

    for (i = 0; i<LEN_CCITABLE; i++) {
        cci = ccitable [i];
        while (cci) {
            temp = cci->next;

            free (cci);

            cci = temp;
        }
    }
}

BOOL SetWindowExStyle (HWND hWnd, DWORD dwExStyle)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;

    pWin = (PMAINWIN)hWnd;

    if (pWin->WinType == TYPE_MAINWIN)
         pWin->dwExStyle=dwExStyle;
    else if (pWin->WinType == TYPE_CONTROL) {
        pCtrl = (PCONTROL)hWnd;
        pCtrl->dwExStyle=dwExStyle;
    }
    else
        return FALSE;

    return TRUE;
}

#ifdef _DEBUG

void mnuDumpCtrlClassInfo (PCTRLCLASSINFO cci)
{
    printf ("\tClass Name:             %s\n", cci->name);
    printf ("\tClass Cursor:           %x\n", cci->hCursor);
    printf ("\tClass Background color: %d\n", cci->iBkColor);
    printf ("\tClass Control Proc:     %p\n", cci->ControlProc);
    printf ("\tClass Use Count:        %d\n", cci->nUseCount);
}

void DumpCtrlClassInfoTable()
{
    PCTRLCLASSINFO cci;
    int i;

    for (i = 0; i<LEN_CCITABLE; i++) {
        cci = ccitable [i];

        printf ("CCI Table Element: %d\n", i);
        while (cci) {
       
            mnuDumpCtrlClassInfo (cci);

            cci = cci->next;
        }
    }
}

#endif

