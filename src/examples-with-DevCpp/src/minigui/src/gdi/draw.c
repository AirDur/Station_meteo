/*
** $Id: draw.c,v 1.27 2005/01/04 07:11:16 limei Exp $
**
** General drawing of GDI
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
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

extern BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);

/*********************** Generl drawing support ******************************/
void GUIAPI SetPixel(HDC hdc, int x, int y, gal_pixel c)
{
    PDC pdc;
    PCLIPRECT pClipRect;
    RECT rcOutput;

    pdc = dc_HDC2PDC (hdc);

    LOCK_ECRGN(return);

    coor_LP2SP (pdc, &x, &y);

    rcOutput.left = x - 1;
    rcOutput.top  = y - 1;
    rcOutput.right = x + 1;
    rcOutput.bottom = y + 1;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if (!dc_IsMemHDC(hdc)) ShowCursorForGDI (FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);
    GAL_SetFgColor (pdc->gc, c);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if(PtInRect(&(pClipRect->rc), x, y)) {
            GAL_DrawPixel (pdc->gc, x, y, c);
            break;
        }
        pClipRect = pClipRect->next;
    }

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI SetPixelRGB(HDC hdc, int x, int y, int r, int g, int b)
{
    PDC pdc;
    PCLIPRECT pClipRect;
    RECT rcOutput;
    gal_pixel pixel;
    GAL_Color color;

    pdc = dc_HDC2PDC (hdc);

    LOCK_ECRGN(return);

    coor_LP2SP (pdc, &x, &y);

    rcOutput.left = x - 1;
    rcOutput.top  = y - 1;
    rcOutput.right = x + 1;
    rcOutput.bottom = y + 1;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);

    color.r = r; color.g = g, color.b = b;
    pixel = GAL_MapColor (pdc->gc, &color);
    GAL_SetFgColor (pdc->gc, pixel);

    pClipRect = pdc->ecrgn.head;
    while (pClipRect)
    {
        if(PtInRect(&(pClipRect->rc), x, y)) {
            GAL_DrawPixel (pdc->gc, x, y, pixel);
            break;
        }
        pClipRect = pClipRect->next;
    }
    
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

gal_pixel GUIAPI GetPixel(HDC hdc, int x, int y)
{
    PDC pdc;
    PCLIPRECT pClipRect;
    int color;
    RECT rcOutput;

    color = 0;
    pdc = dc_HDC2PDC(hdc);

    LOCK_ECRGN(return PIXEL_invalid);

    coor_LP2SP (pdc, &x, &y);

    rcOutput.left = x - 1;
    rcOutput.top  = y - 1;
    rcOutput.right = x + 1;
    rcOutput.bottom = y + 1;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if (!dc_IsMemHDC (hdc)) ShowCursorForGDI (FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if(PtInRect(&(pClipRect->rc), x, y)) {
            GAL_GetPixel (pdc->gc, x, y, &color);
            break;
        }
        pClipRect = pClipRect->next;
    }

    if (!dc_IsMemHDC (hdc)) ShowCursorForGDI (TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();

    return color;
}

void GUIAPI GetPixelRGB(HDC hdc, int x, int y, int* r, int* g, int* b)
{
    PDC pdc;
    PCLIPRECT pClipRect;
    gal_pixel pixel;
    GAL_Color color;

    RECT rcOutput;

    pdc = dc_HDC2PDC(hdc);
    LOCK_ECRGN(return);

    coor_LP2SP(pdc, &x, &y);

    rcOutput.left = x - 1;
    rcOutput.top  = y - 1;
    rcOutput.right = x + 1;
    rcOutput.bottom = y + 1;

    *r = 0;
    *g = 0;
    *b = 0;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if (!dc_IsMemHDC (hdc)) ShowCursorForGDI (FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (PtInRect (&(pClipRect->rc), x, y) ) {
            GAL_GetPixel (pdc->gc, x, y, &pixel);
            GAL_UnmapPixel (pdc->gc, pixel, &color);
            *r = color.r;
            *g = color.g;
            *b = color.b;
            break;
        }
        pClipRect = pClipRect->next;
    }

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI Pixel2RGB(HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b)
{
    PDC pdc;
    GAL_Color color;

    pdc = dc_HDC2PDC (hdc);
    LOCK_GDI();

    GAL_SetGC (pdc->gc);
    GAL_UnmapPixel (pdc->gc, pixel, &color);

    *r = color.r;
    *g = color.g;
    *b = color.b;

    UNLOCK_GDI();
}

gal_pixel GUIAPI RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b)
{
    PDC pdc;
    gal_pixel pixel;
    GAL_Color color;

    color.r = r;
    color.g = g;
    color.b = b;
    
    pdc = dc_HDC2PDC (hdc);
    LOCK_GDI();

    GAL_SetGC (pdc->gc);
    pixel = GAL_MapColor (pdc->gc, &color);

    UNLOCK_GDI();
 
    return pixel;
}

void GUIAPI MoveTo (HDC hdc, int x, int y)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    pdc->CurPenPos.x = x;
    pdc->CurPenPos.y = y;
}

void GUIAPI LineTo (HDC hdc, int x, int y)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    RECT rcOutput;
    int startx, starty;

    pdc = dc_HDC2PDC(hdc);

    startx = pdc->CurPenPos.x;
    starty = pdc->CurPenPos.y;

    // Move the current pen pos.
    pdc->CurPenPos.x = x;
    pdc->CurPenPos.y = y;

    LOCK_ECRGN(return);

    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &x, &y);
    coor_LP2SP(pdc, &startx, &starty);
    rcOutput.left = startx;
    rcOutput.top  = starty;
    rcOutput.right = x;
    rcOutput.bottom = y;
    NormalizeRect (&rcOutput);
    InflateRect (&rcOutput, 1, 1);

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);
    GAL_SetFgColor (pdc->gc, pdc->pencolor);

    pClipRect = pdc->ecrgn.head;

    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping (pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);
           
            if(starty == y) {
                if (startx > x)
                    GAL_DrawHLine (pdc->gc, x, y, startx - x, pdc->pencolor);
                else
                    GAL_DrawHLine (pdc->gc, startx, y, x - startx, pdc->pencolor);
            }
            else
                GAL_Line (pdc->gc, startx, starty, x, y, pdc->pencolor);
        }
            
        pClipRect = pClipRect->next;
    }

    if (!dc_IsMemHDC (hdc)) ShowCursorForGDI (TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI PolyLineTo (HDC hdc, const POINT* point, int poinum)
{
    int   i;

    MoveTo (hdc, point[0].x, point[0].y);

    for (i=1; i<poinum; i++)
        LineTo (hdc, point[i].x, point[i].y);
}

/************************ Circle and Rectangle *******************************/
void GUIAPI Circle(HDC hdc, int x, int y, int r)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    RECT rcOutput;

    pdc = dc_HDC2PDC(hdc);
    LOCK_ECRGN(return);

    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &x, &y);

    rcOutput.left = x - r;
    rcOutput.top  = y - r;
    rcOutput.right = x + r + 1;
    rcOutput.bottom = y + r + 1;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);
    GAL_SetFgColor (pdc->gc, pdc->pencolor);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping (pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right -1 , pClipRect->rc.bottom - 1);
            GAL_Circle (pdc->gc, x, y, r, pdc->pencolor);
        }
            
        pClipRect = pClipRect->next;
    }

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI Rectangle(HDC hdc, int x0, int y0, int x1, int y1)
{

    PCLIPRECT pClipRect;
    PDC pdc;
    RECT rcOutput;

    pdc = dc_HDC2PDC(hdc);
    LOCK_ECRGN(return);
     
    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &x0, &y0); 
    coor_LP2SP(pdc, &x1, &y1); 

    rcOutput.left = x0;
    rcOutput.top = y0;
    rcOutput.right = x1;
    rcOutput.bottom = y1;
    NormalizeRect (&rcOutput);
    rcOutput.right ++;
    rcOutput.bottom ++;

    LOCK_GDI();

    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);

    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    // set graphics context.
    GAL_SetGC (pdc->gc);
    GAL_SetFgColor (pdc->gc, pdc->pencolor);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping (pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);
            GAL_Rectangle (pdc->gc, x0, y0, x1, y1, pdc->pencolor);
        }
            
        pClipRect = pClipRect->next;
    }
    
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
}

void GUIAPI FocusRect(HDC hdc, int x0, int y0, int x1, int y1)
{
    PCLIPRECT pClipRect;
    PDC pdc;
    int l, t, r, b, w, h;
    RECT rcOutput;
    size_t sizeh, sizev;
    BYTE* vbuff;
    BYTE* hline1, * hline2;
    BYTE* vline1, * vline2;
    int bpp;
    BYTE xor_byte;

    hline1 = NULL;hline2 = NULL;
    vline1 = NULL;vline2 = NULL;
    vbuff = NULL;
    pdc = dc_HDC2PDC(hdc);

    if (GAL_BitsPerPixel (pdc->gc) < 8)
        xor_byte = 0x0F;
    else
        xor_byte = 0xFF;

    bpp = GAL_BytesPerPixel (pdc->gc);
    LOCK_ECRGN(return);

    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &x0, &y0); 
    coor_LP2SP(pdc, &x1, &y1); 

    l = MIN (x0, x1); t = MIN (y0, y1); r = MAX (x0, x1); b = MAX (y0, y1);
    rcOutput.left = l; rcOutput.top = t;
    rcOutput.right = r + 1; rcOutput.bottom = b + 1;

    LOCK_GDI();
    IntersectRect (&rcOutput, &rcOutput, &pdc->ecrgn.rcBound);
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(FALSE, &rcOutput);

    GAL_SetGC (pdc->gc);

    w = r - l + 1;
    h = b - t - 1;

    if (w == 0 || h == 0)
        goto my_exit;

    sizeh = w * bpp;
    sizev = h * bpp;
#ifdef HAVE_ALLOCA
    if (!(vbuff = alloca ((sizeh << 1) + (sizev << 1))))
#else
    if (!(vbuff = malloc ((sizeh << 1) + (sizev << 1))))
#endif
        goto my_exit;

    if (w > 0) {
        int i, j;
        int offset;
       
        hline1 = vbuff;
        hline2 = vbuff + sizeh;

        GAL_GetBox (pdc->gc, l, t, w, 1, hline1);
        GAL_GetBox (pdc->gc, l, b, w, 1, hline2);

        offset = 0;
        for (i = 0; i < w; i += 2) {
            for (j = 0; j < bpp; j++) {
                hline1[offset + j] ^= xor_byte;
                hline2[offset + j] ^= xor_byte;
            }
            offset += bpp << 1; 
        }
    }
    
    if (h > 0) {
        int i, j, offset;
        
        vline1 = vbuff + (sizeh << 1);
        vline2 = vbuff + (sizeh << 1) + sizev;
            
        GAL_GetBox (pdc->gc, l, t + 1, 1, h, vline1);
        GAL_GetBox (pdc->gc, r, t + 1, 1, h, vline2);
        
        offset = 0;
        for (i = 0; i < h; i += 2) {
            for (j = 0; j < bpp; j++) {
                vline1[offset + j] ^= xor_byte;
                vline2[offset + j] ^= xor_byte;
            }
            offset += bpp << 1; 
        }
    }

    // set graphics context.
    GAL_SetGC (pdc->gc);

    pClipRect = pdc->ecrgn.head;
    while(pClipRect)
    {
        if (DoesIntersect (&rcOutput, &pClipRect->rc)) {
            GAL_SetClipping(pdc->gc, pClipRect->rc.left, pClipRect->rc.top,
                    pClipRect->rc.right - 1, pClipRect->rc.bottom - 1);
           
            if (hline1) {
                GAL_PutBox (pdc->gc, l, t, w, 1, hline1);
                GAL_PutBox (pdc->gc, l, b, w, 1, hline2);
            }
            
            if (vline1) {
                GAL_PutBox (pdc->gc, l, t + 1, 1, h, vline1);
                GAL_PutBox (pdc->gc, r, t + 1, 1, h, vline2);
            }
        }
            
        pClipRect = pClipRect->next;
    }
    
my_exit:
    if( !dc_IsMemHDC(hdc) ) ShowCursorForGDI(TRUE, &rcOutput);
    UNLOCK_GDI();
    UNLOCK_ECRGN();
#ifndef HAVE_ALLOCA
    free (vbuff);
#endif
}

