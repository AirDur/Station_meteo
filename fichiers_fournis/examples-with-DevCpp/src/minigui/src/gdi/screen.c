/*
** $Id: screen.c,v 1.16 2005/01/04 02:07:02 snig Exp $
**
** Screen operations of GDI
**
** Copyright (C) 2000, Wei Yongming.
**
** Current maintainer: Wei Yongming.
** Create date: 2000/06/12, derived from original gdi.c
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
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "cursor.h"

extern BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);

void SaveScreenBox (int x, int y, int w, int h, void* vbuf)
{
    RECT rcOutput;

    rcOutput.left = x;
    rcOutput.top  = y;
    rcOutput.right = x + w;
    rcOutput.bottom = y + h;

#ifndef _LITE_VERSION
    pthread_mutex_lock (&__mg_gdilock);
#endif
    ShowCursorForGDI (FALSE, &rcOutput);

    GAL_SetGC (PHYSICALGC);
    GAL_GetBox (PHYSICALGC, x, y, w, h, vbuf);

    ShowCursorForGDI (TRUE, &rcOutput);
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&__mg_gdilock);
#endif
}

void* SaveCoveredScreenBox (int x, int y, int w, int h)
{
    void* vbuf;

    if (!(vbuf = malloc (GAL_BoxSize (PHYSICALGC, w, h)))) {
        return NULL;
    }
    
    SaveScreenBox (x, y, w, h, vbuf);

    return vbuf;
}

#ifdef _MISC_SAVESCREEN
BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename)
{
    RECT rcScreen;
    RECT rcWin;
    BITMAP bitmap;
    int save_ret;

    SetRect (&rcScreen, 0, 0, WIDTHOFPHYGC, HEIGHTOFPHYGC);
    if (hWnd) {
        GetWindowRect (hWnd, &rcWin);
        if (!IntersectRect (&rcWin, &rcWin, &rcScreen))
            return FALSE;
    }
    else
        rcWin = rcScreen;

    bitmap.bmType = BMP_TYPE_NORMAL;
    bitmap.bmBitsPerPixel = BITSPERPHYPIXEL;
    bitmap.bmBytesPerPixel = BYTESPERPHYPIXEL;
    bitmap.bmWidth = RECTW (rcWin);
    bitmap.bmHeight = RECTH (rcWin);
    bitmap.bmPitch = bitmap.bmWidth * bitmap.bmBytesPerPixel;

    if (bitmap.bmWidth == 0 || bitmap.bmHeight == 0) {
#ifdef _DEBUG
        uHALr_printf( "SaveContent: Empty Rect.\n");
#endif
        return FALSE;
    }
    
    bitmap.bmBits = SaveCoveredScreenBox (rcWin.left, rcWin.top, 
                                    RECTW (rcWin), RECTH (rcWin));
    if (!bitmap.bmBits) {
#ifdef _DEBUG
        uHALr_printf( "SaveContent: SaveBox error.\n");
#endif
        return FALSE;
    }
    
    save_ret = SaveBitmap (HDC_SCREEN, &bitmap, filename);
    free (bitmap.bmBits);
    return (save_ret == 0);
}
#endif /* _MISC_SAVESCREEN */

void PutSavedBoxOnScreen (int x, int y, int w, int h, void* vbuf)
{
    RECT rcOutput;

    rcOutput.left = x;
    rcOutput.top  = y;
    rcOutput.right = x + w;
    rcOutput.bottom = y + h;

#ifndef _LITE_VERSION
    pthread_mutex_lock (&__mg_gdilock);
#endif
    ShowCursorForGDI (FALSE, &rcOutput);

    GAL_SetGC (PHYSICALGC);
    GAL_SetClipping (PHYSICALGC, 0, 0, WIDTHOFPHYGC - 1, HEIGHTOFPHYGC - 1);
    GAL_PutBox (PHYSICALGC, x, y, w, h, vbuf);

    ShowCursorForGDI (TRUE, &rcOutput);
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&__mg_gdilock);
#endif
}

