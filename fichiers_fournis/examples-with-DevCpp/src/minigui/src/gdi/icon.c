/*
** $Id: icon.c,v 1.26 2005/01/04 02:53:47 snig Exp $
**
** icon.c Icon operations of GDI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
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
#include "icon.h"
#include "cursor.h"
#include "readbmp.h"
#include "gdidecl.h"

extern BOOL dc_GenerateECRgn(PDC pdc, BOOL fForce);

/************************* Icon support **************************************/
#include "loadicon.c"

void GUIAPI DrawIcon(HDC hdc, int x, int y, int w, int h, HICON hicon)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    PICON picon;
    int i;
    int imagesize;
    BYTE* iconimage;
    BYTE* andbits;
    BYTE* xorbits;
    RECT rcOutput;

    picon = (PICON)hicon;
    andbits = NULL;
    xorbits = NULL;
    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return);

    if(w <= 0) w = picon->width;
    if(h <= 0) h = picon->height;

    // Transfer logical to device to screen here.
    w += x; h += y;
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &w, &h);
    rcOutput.left = x;
    rcOutput.top  = y;
    rcOutput.right = w;
    rcOutput.bottom = h;
    NormalizeRect (&rcOutput);
    w = RECTW (rcOutput); h = RECTH (rcOutput);

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    GAL_SetGC (pdc->gc);

    imagesize = w * h * GAL_BytesPerPixel (pdc->gc);
    if ((iconimage = malloc (imagesize)) == NULL)
        goto free_ret;

    if (w != picon->width || h != picon->height) {
        andbits = malloc (imagesize);
        xorbits = malloc (imagesize);
        if (andbits == NULL || xorbits == NULL)
            goto free_ret;

        GAL_ScaleBox (pdc->gc, picon->width, picon->height, picon->AndBits, w, h, andbits);
        GAL_ScaleBox (pdc->gc, picon->width, picon->height, picon->XorBits, w, h, xorbits);
    }
    else {
        andbits = picon->AndBits;
        xorbits = picon->XorBits;
    }

    GAL_GetBox (pdc->gc, x, y, w, h, iconimage);

    for(i = 0; i < imagesize; i++) {
        iconimage[i] &= andbits [i];
        iconimage[i] ^= xorbits [i];
    }

    pClipRect = pdc->ecrgn.head;
    while (pClipRect) {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            GAL_PutBox(pdc->gc, x, y, w, h, iconimage);
        }

        pClipRect = pClipRect->next;
    }

free_ret:
    if (!dc_IsMemHDC(hdc)) ShowCursorForGDI (TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();

    free (iconimage);
    if (w != picon->width || h != picon->height) {
        free (andbits);
        free (xorbits);
    }
}

