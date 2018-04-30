/*
** $Id: fblin32.c,v 1.12 2005/01/08 11:05:14 panweiguo Exp $
**
** fblin32.c: 32bpp Linear Video Driver for MiniGUI
**
** Copyright (C) 2003 Feynman Software.
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

#include <stdlib.h>
#include <string.h>

#include "native.h"
#include "fb.h"

#ifdef _FBLIN32_SUPPORT

static int linear32_init (PSD psd)
{
    if (!psd->size) {
        psd->size = psd->yres * psd->linelen;
        /* convert linelen from byte to pixel len for bpp 16, 24, 32*/
        psd->linelen >>= 2;
    }
    return 1;
}

/* Set pixel at x, y, to gal_pixel c*/
static void linear32_drawpixel (PSD psd, int x, int y, gal_pixel c)
{
    gal_uint32* addr = (gal_uint32 *) psd->addr;

    if (psd->gr_mode == MODE_XOR) 
        addr[x+y*psd->linelen] ^= (gal_uint32) c;
    else 
        addr[x+y*psd->linelen] = (gal_uint32) c;
}

/* Read pixel at x, y*/
static gal_pixel linear32_readpixel (PSD psd, int x, int y)
{
    gal_uint32* addr = (gal_uint32 *) psd->addr;

    return (gal_pixel)addr[ x + y * psd->linelen ];
}

/* Draw horizontal line from x1,y to x2,y including final point*/
static void linear32_drawhline (PSD psd, int x, int y, int w, gal_pixel c)
{
    gal_uint32* addr=(gal_uint32*) psd->addr + x + y * psd->linelen;

    if (psd->gr_mode == MODE_XOR) 
        while (w --) 
            *addr++ ^= (gal_uint32) c;
    else 
        while (w --) 
            *addr++ = (gal_uint32) c;
}

static void linear32_drawvline(PSD psd, int x, int y, int h, gal_pixel c)
{    
    gal_uint32     *dst=(gal_uint32*) psd->addr;

    dst = ( gal_uint32*)(psd->addr) + y * psd->linelen + x;

    if (psd->gr_mode == MODE_XOR) 
        while (h --) {
            *dst ^= (gal_uint32)c;
            dst += psd->linelen;
        }
    else 
        while (h --)  {
            *dst = (gal_uint32)c;
            dst += psd->linelen;
        }
}

/* srccopy bitblt,not do clip*/
static void linear32_blit (PSD dstpsd, int dstx, int dsty, int w, int h,
    PSD srcpsd, int srcx, int srcy)
{
    gal_uint32    *dst = (gal_uint32 *) dstpsd->addr;
    gal_uint32    *src = (gal_uint32 *) srcpsd->addr;
    int    dlinelen = dstpsd->linelen;
    int    slinelen = srcpsd->linelen;

    dst += dstx + dsty * dlinelen;
    src += srcx + srcy * slinelen;

    while(--h >= 0) {
        /* a _fast_ memcpy is a _must_ in this routine*/
        memcpy(dst, src, w<<2);
        dst += dlinelen;
        src += slinelen;
    }
}

/*clip to screen*/
static void linear32_getbox ( PSD psd, int x, int y, int w, int h, void* buf )
{
    gal_uint32 *dst = (gal_uint32*) buf;
    int dstwidth = w;
    gal_uint8 *src;
    int srcwidth = psd->linelen << 2;
    int oldx = x;

    if ( y < 0 ) {
        h += y;
        dst -= y * dstwidth;
        y = 0;
    }
    if ( x < 0 ) {
        w += x;
        dst -= x;
        x = 0;
    }        
    if ( y + h  -1 >= psd->yres) 
        h = psd->yres - y ;
    if ( x + w  -1 >= psd->xres) 
        w = psd->xres - x ;

    src = (gal_uint8 *)(psd->addr) + (y*srcwidth + (x << 2));

    w <<= 2;

    if (w == dstwidth && psd->linelen == srcwidth && oldx == 0 && psd->clipminx == 0) {
        memcpy (dst, src, w*h);
        return;
    }

    while (h-- > 0) {
        memcpy(dst, src, w);
        dst += dstwidth;
        src += srcwidth;
    }
}

/*do clip*/
static void linear32_putbox ( PSD psd, int x, int y, int w, int h, void *buf)
{
    gal_uint32    *src = (gal_uint32*) buf;
    int srcwidth = w;
    gal_uint8 *dst;
    int dstwidth = psd->linelen <<2;
    int oldx = x;

    if (psd->doclip) {    
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * srcwidth;
            y = psd->clipminy;
        }
        if (x < psd->clipminx) {
            w -= (psd->clipminx - x);
            src += (psd->clipminx - x);
            x = psd->clipminx;
        }        
        if (y + h - 1 >= psd->clipmaxy) 
            h =  psd->clipmaxy- y;
        if (x + w - 1 >= psd->clipmaxx) 
            w =  (psd->clipmaxx- x);
    }
    else {
        if ( y < 0 ) {
            h += y;
            src -= y * srcwidth;
            y = 0;
        }
        if ( x < 0 ) {
            w += x;
            src -= x;
            x = 0;
        }        
        if ( y + h  -1 >= psd->yres) 
            h = psd->yres - y ;
        if ( x + w  -1 >= psd->xres) 
            w = (psd->xres - x) ;
    }

    dst = (gal_uint8 *)(psd->addr) + (y*dstwidth + x*4);

    w <<= 2;

    if (w == dstwidth && psd->linelen == srcwidth && oldx == 0 && psd->clipminx == 0) {
        memcpy (dst, src, w*h);
        return;
    }

    while (h > 0) {
        memcpy(dst, src, w);
        dst += dstwidth;
        src += srcwidth;
        h--;
    }
}
static void linear32_putboxmask ( PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx)
{
    gal_uint32 *src= (gal_uint32*) buf;
    gal_uint32 *src1;
    gal_uint32 *endoflinesrc;
    gal_uint8 *dst;
    int i;
    int srcwidth =  w ;
    int dstwidth = psd->linelen << 2;

    if (psd->doclip) {    
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * srcwidth;
            y = psd->clipminy;
        }
        if (x < psd->clipminx) {
            w -= (psd->clipminx - x);
            src += (psd->clipminx - x);
            x = psd->clipminx;
        }        
        if (y + h - 1 >= psd->clipmaxy) 
            h =  psd->clipmaxy- y;
        if (x + w - 1 >= psd->clipmaxx) 
            w =  (psd->clipmaxx- x);
    }
    else {
        if ( y < 0 ) {
            h += y;
            src -= y * srcwidth;
            y = 0;
        }
        if ( x < 0 ) {
            w += x;
            src -= x;
            x = 0;
        }        
        if ( y + h  -1 >= psd->yres) 
            h = psd->yres - y ;
        if ( x + w  -1 >= psd->xres) 
            w = psd->xres - x ;
    }

    if (w <= 0 || h<=0) return;

    dst= (gal_uint8 *)(psd->addr) + (y*dstwidth + x*4);

    for (i = 0; i < h; i++) {
        src1 = src ; 
        endoflinesrc = src + w;

        while (src1 < endoflinesrc) {
            gal_uint32 c = *src1;
            if (c != cxx)
                *(gal_uint32 *) dst = c;
            src1 ++;
            dst += 4;
        }
        dst += psd->linelen * 4 - w * 4;

        src += srcwidth;
    }
}

static void linear32_copybox(PSD psd,int x1, int y1, int w, int h, int x2, int y2)
{
    gal_uint32 *svp, *dvp;

    if (y1 >= y2) {
        if (y1 == y2 && x2 >= x1) {
            int i;
            if (x1 == x2)
                return;

            svp = (gal_uint32 *) psd->addr + y1 * psd->linelen + x1;
            dvp = (gal_uint32 *) psd->addr + y2 * psd->linelen + x2;
            for (i = 0; i < h; i++) {
                gal_uint8 linebuf[2048];
                memcpy (linebuf, svp, w<<2);
                memcpy (dvp, linebuf, w<<2);
                svp += psd->linelen;
                dvp += psd->linelen;
            }
        } else {
            int i;
            svp = (gal_uint32 *) psd->addr + y1 * psd->linelen + x1;
            dvp = (gal_uint32 *) psd->addr + y2 * psd->linelen + x2;
            for (i = 0; i < h; i++) {
                memcpy(dvp, svp, w<<2);
                svp += psd->linelen;
                dvp += psd->linelen;
            }
        }
    } else {
        int i;

        svp = (gal_uint32 *) psd->addr + (y1 + h) * psd->linelen + x1;
        dvp = (gal_uint32 *) psd->addr + (y2 + h) * psd->linelen + x2;
        for (i = 0; i < h; i++) {
            svp -= psd->linelen;
            dvp -= psd->linelen;
            memcpy (dvp, svp, w*4);
        }
    }
}

SUBDRIVER fblinear32 = {
    linear32_init,
    linear32_drawpixel,
    linear32_readpixel,
    linear32_drawhline,
    linear32_drawvline,
    linear32_blit,
    linear32_putbox,
    linear32_getbox,
    linear32_putboxmask,
    linear32_copybox
};

#endif /* _FBLIN32_SUPPORT */

