/*
** $Id: bitmap.c,v 1.43 2005/01/04 07:11:16 limei Exp $
**
** Bitmap operations of GDI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002  Wei Yongming.
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
#include "cursor.h"
#include "gdidecl.h"

extern BOOL dc_GenerateECRgn(PDC pdc, BOOL fForce);

/****************************** Bitmap Support *******************************/
void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    RECT rcOutput;

    if (w == 0 || h == 0)
        return;

    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return);

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

    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    LOCK_GDI();
   if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);
    
    // set graphics context.
    GAL_SetGC (pdc->gc);
    GAL_SetFgColor (pdc->gc, pdc->brushcolor);
           
    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);
        
            GAL_FillBox (pdc->gc, x, y, w, h, pdc->brushcolor);
        }

        pClipRect = pClipRect->next;
    }

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

BOOL GUIAPI FillBoxWithBitmap (HDC hdc, int x, int y, int w, int h, 
                              const BITMAP* pBitmap)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    void* scaledBitmap;
    int sw, sh;
    RECT rcOutput;
   
    if (!pBitmap || pBitmap->bmWidth <= 0 || pBitmap->bmHeight <= 0 || pBitmap->bmBits == NULL)
        return FALSE;

    sw = pBitmap->bmWidth;
    sh = pBitmap->bmHeight;
    if (w <= 0 || h <= 0) {
        w = sw;
        h = sh;
    }

    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return TRUE);

    // Transfer logical to device to screen here.
    w += x; h += y;
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &w, &h);
    rcOutput.left = x;
    rcOutput.top = y;
    rcOutput.right = w;
    rcOutput.bottom = h;
    NormalizeRect (&rcOutput);
    w = RECTW (rcOutput); h = RECTH (rcOutput);

    LOCK_GDI();

    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    if(w == sw && h == sh)
        scaledBitmap = pBitmap->bmBits;
    else
    {
        if ((scaledBitmap = malloc (GAL_BoxSize (pdc->gc, w, h))) == NULL)
            goto free_ret;
        GAL_ScaleBox (pdc->gc, sw, sh, pBitmap->bmBits, w, h, scaledBitmap);
    }
         
    // set graphics context.
    GAL_SetGC (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while (pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            if (pBitmap->bmType != BMP_TYPE_COLORKEY)
                GAL_PutBox (pdc->gc, x, y, w, h, scaledBitmap);
            else
                GAL_PutBoxMask (pdc->gc, x, y, w, h, scaledBitmap, pBitmap->bmColorKey);
        }

        pClipRect = pClipRect->next;
    }

free_ret:
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();

    if (w != sw || h != sh) {
        free (scaledBitmap);
    }

    return TRUE;
}

static void bmpGetBoxPart (int bpp, int w, int h, unsigned char* part, 
                           int ow, int oh, unsigned char* full, int xo, int yo)
{
    int i;
    int offset;
    int lineoffset;
    int linebytes;

    lineoffset = ow * bpp;
    linebytes = w * bpp;
    offset = (ow * yo + xo) * bpp;
    
    for (i=0; i<h; i++) {
        memcpy (part, full + offset, linebytes);
        part += linebytes;
        offset += lineoffset;
    }
}

BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc, int x, int y, int w, int h,
                int bw, int bh, const BITMAP* pBitmap, int xo, int yo)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    void* scaledBitmap;
    void* partBitmap;
    int sw, sh;
    RECT rcOutput;
    int bpp;
   
    scaledBitmap = NULL;
    partBitmap = NULL;
    sw = pBitmap->bmWidth;
    sh = pBitmap->bmHeight;
    
    if (pBitmap->bmWidth <= 0 || pBitmap->bmHeight <= 0 || pBitmap->bmBits == NULL)
        return FALSE;

    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return TRUE);
    bpp = GAL_BytesPerPixel (pdc->gc);

    // Transfer logical to device to screen here.
    w += x; h += y;
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &w, &h);
    rcOutput.left = x;
    rcOutput.top = y;
    rcOutput.right = w;
    rcOutput.bottom = h;
    NormalizeRect (&rcOutput);
    w = RECTW (rcOutput); h = RECTH (rcOutput);

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC(pdc->gc);

    if (bw <= 0) bw = sw;
    if (bh <= 0) bh = sh;

    if (bw == sw && bh == sh)
        scaledBitmap = pBitmap->bmBits;
    else {
        if ((scaledBitmap = malloc (GAL_BoxSize (pdc->gc, bw, bh))) == NULL)
            goto free_ret;
        GAL_ScaleBox (pdc->gc, sw, sh, pBitmap->bmBits, bw, bh, scaledBitmap);
    }

    // extract part box
    if ((partBitmap = malloc (GAL_BoxSize (pdc->gc, w, h))) == NULL)
        goto free_ret;
    bmpGetBoxPart (bpp, w, h, partBitmap, bw, bh, scaledBitmap, xo, yo);
   
    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {        
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            if (pBitmap->bmType != BMP_TYPE_COLORKEY)
                GAL_PutBox (pdc->gc, x, y, w, h, partBitmap);
            else
                GAL_PutBoxMask (pdc->gc, x, y, w, h, partBitmap, pBitmap->bmColorKey);
        }

        pClipRect = pClipRect->next;
    }

free_ret:
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();

    if (bw != sw || bh != sh)
        free (scaledBitmap);
    free (partBitmap);
    return TRUE;
}

void GUIAPI PutSavedBoxOnDC (HDC hdc, int x, int y, int w, int h, void* vbuf)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    RECT rcOutput;

    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return);

    // Transfer logical to device to screen here.
    w += x; h += y;
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &w, &h);
    rcOutput.left = x;
    rcOutput.top = y;
    rcOutput.right = w;
    rcOutput.bottom = h;
    NormalizeRect (&rcOutput);
    w = RECTW (rcOutput); h = RECTH (rcOutput);

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    GAL_SetGC (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            GAL_PutBox (pdc->gc, x, y, w, h, vbuf);
        }

        pClipRect = pClipRect->next;
    }

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void ScreenCopy (int sx, int sy, HDC hdc, int dx, int dy)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    int offx, offy;
    int boxLeft, boxTop, boxWidth, boxHeight;
    RECT* prc;

    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return)
    //else return;

    coor_LP2SP(pdc, &dx, &dy);
    offx = sx - dx;
    offy = sy - dy;

    LOCK_GDI();
    ShowCursorForGDI(FALSE, &g_rcScr);

    GAL_SetGC (PHYSICALGC);
    GAL_DisableClipping (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while (pClipRect)
    {
        prc = &pClipRect->rc;
        boxLeft   = prc->left   + offx;
        boxTop    = prc->top    + offy;
        boxWidth  = prc->right  - prc->left;
        boxHeight = prc->bottom - prc->top;

        GAL_CopyBox (pdc->gc, boxLeft, boxTop, boxWidth, boxHeight, prc->left, prc->top);

        pClipRect = pClipRect->next;
    }

    ShowCursorForGDI(TRUE, &g_rcScr);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI BitBlt(HDC hsdc, int sx, int sy, int sw, int sh,
                   HDC hddc, int dx, int dy, DWORD dwRop)
{
    PCLIPRECT pClipRect;
    PDC psdc, pddc;
    RECT rcOutput;

    psdc = dc_HDC2PDC(hsdc);
    pddc = dc_HDC2PDC(hddc);

    LOCK_ECRGN_EX(pddc, hddc, return);

    if (sw <= 0) sw = RECTW (psdc->DevRC);
    if (sh <= 0) sh = RECTH (psdc->DevRC);

    // Transfer device to screen here.
    coor_DP2SP(psdc, &sx, &sy);
    coor_DP2SP(pddc, &dx, &dy);
    rcOutput.left = dx;
    rcOutput.top  = dy;
    rcOutput.right = dx + sw;
    rcOutput.bottom = dy + sh;
    
    LOCK_GDI();
    ShowCursorForGDI(FALSE, &g_rcScr);

    // set graphics context.
    GAL_SetGC (pddc->gc);

    pClipRect = pddc->ecrgn.head;

    while(pClipRect) {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pddc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            GAL_CrossBlit (psdc->gc, sx, sy, sw, sh, pddc->gc, dx, dy);
        }

        pClipRect = pClipRect->next;
    }

    ShowCursorForGDI(TRUE, &g_rcScr);
    UNLOCK_GDI();
    UNLOCK_ECRGN_EX(pddc, hddc);
}

void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh,
                       HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
{
    PCLIPRECT pClipRect;
    PDC psdc, pddc;
    void* srcBitmap; 
    void* scaledBitmap;
    RECT rcOutput;

    psdc = dc_HDC2PDC(hsdc);
    pddc = dc_HDC2PDC(hddc);
    srcBitmap = NULL; 
    scaledBitmap = NULL;

    LOCK_ECRGN_EX(pddc, hddc, return);

    if (sw <= 0) sw = RECTW (psdc->DevRC);
    if (sh <= 0) sh = RECTH (psdc->DevRC);
    if (dw <= 0) dw = RECTW (pddc->DevRC);
    if (dh <= 0) dh = RECTH (pddc->DevRC);

    // Transfer device to screen here.
    coor_DP2SP(psdc, &sx, &sy);
    coor_DP2SP(pddc, &dx, &dy);
    rcOutput.left = dx;
    rcOutput.top = dy;
    rcOutput.right = dx + dw;
    rcOutput.bottom = dy + dh;

    LOCK_GDI();
    if (!dc_IsMemHDC(hddc)) ShowCursorForGDI(FALSE, &g_rcScr);
    GAL_SetGC (psdc->gc);

    if ((srcBitmap = malloc (GAL_BoxSize (psdc->gc, sw, sh))) == NULL || 
        (scaledBitmap = malloc (GAL_BoxSize (pddc->gc, dw, dh))) == NULL)
        goto free_ret;

    GAL_GetBox (psdc->gc, sx, sy, sw, sh, srcBitmap);
    GAL_ScaleBox (psdc->gc, sw, sh, srcBitmap, dw, dh, scaledBitmap);

    GAL_SetGC (pddc->gc);

    pClipRect = pddc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping (pddc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);

            GAL_PutBox (pddc->gc, dx, dy, dw, dh, scaledBitmap);
        }

        pClipRect = pClipRect->next;
    }

free_ret:
    if (!dc_IsMemHDC(hddc)) ShowCursorForGDI (TRUE, &g_rcScr);
    UNLOCK_GDI();
    UNLOCK_ECRGN_EX(pddc, hddc);

    free (srcBitmap);
    free (scaledBitmap);
}

#include "bitmap-comm.c"

