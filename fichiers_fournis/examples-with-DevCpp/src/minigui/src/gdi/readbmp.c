/*
** $Id: readbmp.c,v 1.25 2004/04/16 08:39:36 weiym Exp $
**
** Top-level bitmap file read/save function.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/08/26, derived from original bitmap.c
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
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "readbmp.h"

/* Function: int ExpandMyBitmap (HDC hdc, const MYBITMAP* pMyBitmap, PBITMAP pBitmap);
 *      This function expand a mybitmap to compiled bitmap.
 */
int GUIAPI ExpandMyBitmap (HDC hdc, const MYBITMAP* pMyBitmap, const RGB* pal, PBITMAP pBitmap)
{
    PDC pdc;
    
    pdc = dc_HDC2PDC (hdc);

    if (!(pBitmap->bmBits = malloc (
                (size_t)pMyBitmap->w * pMyBitmap->h * GAL_BytesPerPixel (pdc->gc)))) {
        return ERR_BMP_MEM;
    }

    pBitmap->bmType = BMP_TYPE_NORMAL;
    pBitmap->bmBitsPerPixel = GAL_BitsPerPixel (pdc->gc);
    pBitmap->bmBytesPerPixel = GAL_BytesPerPixel (pdc->gc);
    pBitmap->bmWidth = pMyBitmap->w;
    pBitmap->bmHeight = pMyBitmap->h;
    pBitmap->bmPitch = pMyBitmap->w * pBitmap->bmBytesPerPixel;

    if (pMyBitmap->flags & MYBMP_TRANSPARENT) {
        if (pal && pMyBitmap->depth <= 8)
            pBitmap->bmColorKey = GAL_MapColor (pdc->gc, 
                            (GAL_Color*)(pal + pMyBitmap->transparent));
        else {
            GAL_Color color;
#if 1
            color.r = GetRValue (pMyBitmap->transparent);
            color.g = GetGValue (pMyBitmap->transparent);
            color.b = GetBValue (pMyBitmap->transparent);
#else
            Uint8* src = (Uint8*) (&pMyBitmap->transparent);
            if ((pMyBitmap->flags & MYBMP_TYPE_MASK) == MYBMP_TYPE_BGR) {
                color.b = *src++; color.g = *src++; color.r = *src++;
            }
            else {
                color.r = *src++; color.g = *src++; color.b = *src++;
            }
#endif
            pBitmap->bmColorKey = GAL_MapColor (pdc->gc, &color);
        }
        pBitmap->bmType = BMP_TYPE_COLORKEY;
    }

    switch (pMyBitmap->depth) {
    case 1:
        ExpandMonoBitmap (hdc, pMyBitmap->w, pMyBitmap->h,
                            pMyBitmap->bits, pMyBitmap->pitch, pMyBitmap->flags & MYBMP_FLOW_MASK, 
                            pBitmap->bmBits, pBitmap->bmPitch,
                            GAL_MapColor (pdc->gc, (GAL_Color*)pal),
                            GAL_MapColor (pdc->gc, (GAL_Color*)(pal + 1)));
        break;
    case 4:
        Expand16CBitmap (hdc, pMyBitmap->w, pMyBitmap->h,
                            pMyBitmap->bits, pMyBitmap->pitch, pMyBitmap->flags & MYBMP_FLOW_MASK, 
                            pBitmap->bmBits, pBitmap->bmPitch, pal);
        break;
    case 8:
        Expand256CBitmap (hdc, pMyBitmap->w, pMyBitmap->h,
                            pMyBitmap->bits, pMyBitmap->pitch, pMyBitmap->flags & MYBMP_FLOW_MASK, 
                            pBitmap->bmBits, pBitmap->bmPitch, pal);
        break;
    case 24:
        CompileRGBBitmap (hdc, pMyBitmap->w, pMyBitmap->h,
                            pMyBitmap->bits, pMyBitmap->pitch, pMyBitmap->flags & MYBMP_FLOW_MASK, 
                            pBitmap->bmBits, pBitmap->bmPitch,
                            pMyBitmap->flags & MYBMP_TYPE_MASK);
        break;
    default:
        return ERR_BMP_NOT_SUPPORTED;
    }

    return ERR_BMP_OK;
}

/* Function: int LoadBitmapEx (HDC hdc, PBITMAP bmp, MG_RWops* area, const char* ext)
 *      This function loads a bitmap from an image source and fills
 *      the specified BITMAP struct.
 */
int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP bmp, MG_RWops* area, const char* ext)
{
    MYBITMAP myBitmap;
    RGB pal [256];
    int ret;

    if ((ret = LoadMyBitmapEx (&myBitmap, pal, area, ext)) < 0)
        return ret;

    ret = ExpandMyBitmap (hdc, &myBitmap, pal, bmp);
    free (myBitmap.bits);

    return ret;
}

int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP bmp, const char* file_name)
{
    MYBITMAP myBitmap;
    RGB pal [256];
    int ret;

    if ((ret = LoadMyBitmapFromFile (&myBitmap, pal, file_name)) < 0)
        return ret;

    ret = ExpandMyBitmap (hdc, &myBitmap, pal, bmp);
    free (myBitmap.bits);

    return ret;
}

int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP bmp, const void* mem, int size, const char* ext)
{
    MYBITMAP myBitmap;
    RGB pal [256];
    int ret;
    
    if ((ret = LoadMyBitmapFromMem (&myBitmap, pal, mem, size, ext)) < 0)
        return ret;

    ret = ExpandMyBitmap (hdc, &myBitmap, pal, bmp);
    free (myBitmap.bits);

    return ret;
}

void GUIAPI UnloadBitmap (PBITMAP pBitmap)
{
     free (pBitmap->bmBits);
     pBitmap->bmBits = NULL;
}

#ifdef _SAVE_BITMAP

int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP pBitmap, const char* spFileName)
{
    int i;
    PDC pdc;
    MYBITMAP myBitmap;
    RGB pal [256];

    pdc = dc_HDC2PDC (hdc);
    switch (GAL_BitsPerPixel (pdc->gc)) {
    case 4:
        for (i = 0; i < 16; i++)
            GAL_GetPalette (pdc->gc, i, 1, (GAL_Color*)(pal + i));
        break;
    case 8:
        for (i = 0; i < 256; i++)
            GAL_GetPalette (pdc->gc, i, 1, (GAL_Color*)(pal + i));
        break;
    }

    myBitmap.flags = MYBMP_TYPE_NORMAL;
    myBitmap.frames = 1;
    myBitmap.depth = GAL_BitsPerPixel (pdc->gc);
    myBitmap.w = pBitmap->bmWidth;
    myBitmap.h = pBitmap->bmHeight;
    myBitmap.pitch = myBitmap.w * myBitmap.depth;
    myBitmap.size = myBitmap.w * myBitmap.h * GAL_BytesPerPixel (pdc->gc);
    myBitmap.bits = pBitmap->bmBits;

    return SaveMyBitmapToFile (&myBitmap, pal, spFileName);
}

#endif

