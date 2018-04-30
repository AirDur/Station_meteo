/*
** $Id: about.c,v 1.28 2004/04/09 09:11:28 snig Exp $
**
** about.c: the About Dialog module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
** 
** Current maintainer: Wei Yongming.
**
** Create date: 1999/8/28
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

#include "common.h"

#ifdef _MISC_ABOUTDLG

#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"

static HWND sg_AboutWnd = HWND_DESKTOP;

static int AboutWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    const BITMAP* bmpLogo;
    RECT    rcClient;

    switch (message) {
        case MSG_CREATE:
            if ((bmpLogo = GetStockBitmap (STOCKBMP_LOGO, -1, -1)) == NULL)
                return -1;
            SetWindowAdditionalData (hWnd, (DWORD)bmpLogo);

            sg_AboutWnd = hWnd;
            GetClientRect (hWnd, &rcClient);
            CreateWindow ("button", "Close", 
                                WS_CHILD | BS_DEFPUSHBUTTON | WS_VISIBLE, 
                                IDOK, 
                                (RECTW(rcClient) - 80)>>1, 
                                rcClient.bottom - 40, 
                                80, 24, hWnd, 0);
        break;
        
        case MSG_COMMAND:
            if (LOWORD (wParam) == IDOK && HIWORD (wParam) == BN_CLICKED)
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
        break;
       
        case MSG_KEYDOWN:
            if (LOWORD (wParam) == SCANCODE_ESCAPE)
                PostMessage (hWnd, MSG_CLOSE, 0, 0);
        break;

        case MSG_PAINT:
        {
            HDC hdc;
            
            bmpLogo = (PBITMAP) GetWindowAdditionalData (hWnd);

            hdc = BeginPaint (hWnd);
            
            GetClientRect (hWnd, &rcClient);
            FillBoxWithBitmap (hdc, 
                    (RECTW(rcClient) - bmpLogo->bmWidth)>>1, 10, 0, 0, bmpLogo);
            
            rcClient.top = 60;
            rcClient.bottom -= 50;
            SetTextColor (hdc, PIXEL_lightgray);
            SetBkColor (hdc, PIXEL_black);
            DrawText (hdc, "MiniGUI -- a compact GUI support system.\n"
                           "MiniGUI is free software; you can\n"
                           "redistribute it and/or modify it under\n"
                           "the terms of the GNU GPL.\n"
                           "Copyright (C) 2003 Feynman Software.\n",
                           -1, &rcClient, DT_WORDBREAK | DT_CENTER);
            EndPaint (hWnd, hdc);
        }
        return 0;

        case MSG_CLOSE:
            DestroyAllControls (hWnd);
            DestroyMainWindow (hWnd);
            sg_AboutWnd = HWND_DESKTOP;
#ifndef _LITE_VERSION
            PostQuitMessage (hWnd);
#endif
        return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitAboutDialogCreateInfo (PMAINWINCREATE pCreateInfo, char* caption)
{
    sprintf (caption, "About MiniGUI (Ver %d.%d.%d)", 
                    MINIGUI_MAJOR_VERSION,
                    MINIGUI_MINOR_VERSION,
                    MINIGUI_MICRO_VERSION);

    pCreateInfo->dwStyle = WS_CAPTION | WS_SYSMENU | WS_VISIBLE 
                                      | WS_BORDER;
    pCreateInfo->dwExStyle = WS_EX_TOPMOST;
    pCreateInfo->spCaption = caption;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = AboutWinProc;
    pCreateInfo->lx = 10; 
    pCreateInfo->ty = 5;
    if (GetSysCharWidth () == 6) {
        pCreateInfo->rx = 260;
        pCreateInfo->by = 215;
    }
    else {
        pCreateInfo->rx = 340;
        pCreateInfo->by = 240;
    }
    pCreateInfo->iBkColor = PIXEL_black; 
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

#ifndef _LITE_VERSION
static void* AboutDialogThread (void* data)
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    char caption [256];

    InitAboutDialogCreateInfo (&CreateInfo, caption);

    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return NULL;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);

    while( GetMessage(&Msg, hMainWnd) ) {
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);
    return NULL;
}

void GUIAPI OpenAboutDialog (void)
{
    pthread_t thread;
    
    if (sg_AboutWnd != HWND_DESKTOP) {
        ShowWindow (sg_AboutWnd, SW_SHOWNORMAL);
        return;
    }

    CreateThreadForMainWindow (&thread, NULL, AboutDialogThread, 0);
}

#else

HWND GUIAPI OpenAboutDialog (HWND hHosting)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    char caption [256];

    if (sg_AboutWnd != HWND_DESKTOP) {
        ShowWindow (sg_AboutWnd, SW_SHOWNORMAL);
        return sg_AboutWnd;
    }

    InitAboutDialogCreateInfo (&CreateInfo, caption);
    CreateInfo.hHosting = hHosting;

    hMainWnd = CreateMainWindow (&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return HWND_INVALID;

    ShowWindow (hMainWnd, SW_SHOWNORMAL);
    return hMainWnd;
}

#endif /* _LITE_VERSION */

#endif /* _MISC_ABOUTDLG */

