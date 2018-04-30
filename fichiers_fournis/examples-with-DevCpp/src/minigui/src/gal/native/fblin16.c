/*
** $Id: fblin16.c,v 1.23 2005/01/08 11:05:14 panweiguo Exp $
**
** fblin16.c: 16bpp Linear Video Driver for MiniGUI
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 Song Lixin and Wei Yongming.
** 
** 2000/10/20: Created by Song Lixin.
**
** 2003/07/11: Cleanup by Wei Yongming.
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

#ifdef _FBLIN16_SUPPORT

static int linear16_init (PSD psd)
{
    if (!psd->size) {
        psd->size = psd->yres * psd->linelen;
        /* convert linelen from byte to pixel len for bpp 16, 24, 32*/
        psd->linelen /= 2;
    }
    return 1;
}

/* Set pixel at x, y, to gal_pixel c*/
static void linear16_drawpixel (PSD psd, int x, int y, gal_pixel c)
{
    gal_uint16  * addr = (gal_uint16 *) psd->addr;

    if (psd->gr_mode == MODE_XOR)
        addr[x + y * psd->linelen] ^= (gal_uint16) c; 
    else
        addr[x + y * psd->linelen] = (gal_uint16) c;
}

/* Read pixel at x, y*/
static gal_pixel linear16_readpixel (PSD psd, int x, int y)
{
    gal_uint16    *addr = (gal_uint16 *) psd->addr;

    return (gal_pixel) addr[x + y * psd->linelen];
}

/* Draw horizontal line from x1,y to x2,y including final point*/
static void linear16_drawhline (PSD psd, int x, int y, int w, gal_pixel c)
{
    gal_uint16    *dst;

    dst = (gal_uint16 *)(psd->addr) + y * psd->linelen + x;

    if (psd->gr_mode == MODE_XOR) {
        while (w --)
            *dst++ ^= (gal_uint16)c;
    }
    else {
        while (w --)
            *dst++ = (gal_uint16)c;
    }
}

static void linear16_drawvline(PSD psd, int x, int y, int h, gal_pixel c)
{
    gal_uint16    *dst;
    int    linelen = psd->linelen;

    dst = (gal_uint16 *)(psd->addr) + y * psd->linelen + x;

    if (psd->gr_mode == MODE_XOR) {
        while (h --) {
            *dst^= (gal_uint16)c;
            dst+= linelen;
        }
    }
    else {
        while (h --) {
            *dst = (gal_uint16)c;
            dst+= linelen;
        }
    }
}

/* srccopy bitblt, not do clip*/
static void linear16_blit (PSD dstpsd, int dstx, int dsty, int w, int h,
    PSD srcpsd, int srcx, int srcy)
{
    gal_uint16    *dst = (gal_uint16 *) dstpsd->addr;
    gal_uint16    *src = (gal_uint16 *) srcpsd->addr;
    int    dlinelen = dstpsd->linelen;
    int    slinelen = srcpsd->linelen;

    dst += dstx + dsty * dlinelen;
    src += srcx + srcy * slinelen;

    while(--h >= 0) {
        /* a _fast_ memcpy is a _must_ in this routine*/
        memcpy (dst, src, w<<1);
        dst += dlinelen;
        src += slinelen;
    }
}

/* do clip */
static void linear16_putbox ( PSD psd, int x, int y, int w, int h, void *buf)
{
    gal_uint16    *src = (gal_uint16*) buf;
    int srcwidth = w;
    gal_uint8 *dst;
    int dstwidth = psd->linelen << 1;
    int oldx = x;

    if (psd->doclip) {    
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * srcwidth;
            y = psd->clipminy;
        }
        if (x < psd->clipminx) {
            w -= psd->clipminx - x;
            src += psd->clipminx - x;
            x = psd->clipminx;
        }        
        if (y + h - 1 >= psd->clipmaxy) 
            h =  psd->clipmaxy- y;
        if (x + w - 1 >= psd->clipmaxx) 
            w =  psd->clipmaxx- x;
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

    dst = (gal_uint8 *)(psd->addr) + (y*dstwidth + (x << 1));

    if (w <= 0 || h<=0) return;

    /* Width should be in bytes */
    w <<= 1;

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

/* clip to screen */
static void linear16_getbox ( PSD psd, int x, int y, int w, int h, void* buf )
{
    gal_uint16 *dst = (gal_uint16*) buf;
    int dstwidth = w;
    gal_uint8 *src;
    int srcwidth = psd->linelen << 1;
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


    src = (gal_uint8 *)(psd->addr) + (y*srcwidth + (x << 1));

    w <<= 1;
    
    if (w <= 0 || h<=0) return;

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

static void linear16_putboxmask ( PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx)
{
    gal_uint16 *src= (gal_uint16*) buf;
    gal_uint16 *endoflinesrc;
    gal_uint16 *src1;
    gal_uint8 *dst;
    int i;
    int srcwidth =  w;
    int dstwidth = psd->linelen << 1;

    if (psd->doclip) {    
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * srcwidth;
            y = psd->clipminy;
        }
        if (x < psd->clipminx) {
            w -= psd->clipminx - x;
            src += psd->clipminx - x;
            x = psd->clipminx;
        }        
        if (y + h - 1 >= psd->clipmaxy) 
            h =  psd->clipmaxy- y;
        if (x + w - 1 >= psd->clipmaxx) 
            w =  psd->clipmaxx- x;
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

    if (w <= 0 || h <= 0) return;

    dst = (gal_uint8 *)(psd->addr) + (y*dstwidth + x*2);

    for (i = 0; i < h; i++) {
        src1 = src ; 
        endoflinesrc = src +  w;

        while (src1 < endoflinesrc) {
            gal_uint16 c = *src1;
            if (c != cxx)
                *(gal_uint16 *) dst = c;
            src1 ++;
            dst += 2;
        }
        dst += psd->linelen * 2 - w * 2;

        src += srcwidth;
    }
}

static void linear16_copybox (PSD psd, int x1, int y1, int w, int h, int x2, int y2)
{
    gal_uint16 *svp, *dvp;

    if (y1 >= y2) {
        if (y1 == y2 && x2 >= x1) {    /* tricky */
            int i;
            if (x1 == x2)
                return;
            /* use a temporary buffer to store a line */
            /* using reversed movs would be much faster */
            svp = (gal_uint16 *) psd->addr + y1 * psd->linelen + x1;
            dvp = (gal_uint16 *) psd->addr + y2 * psd->linelen + x2;
            for (i = 0; i < h; i++) {
                gal_uint8 linebuf[2048];
                memcpy (linebuf, svp, w<<1);
                memcpy (dvp, linebuf, w<<1);
                svp += psd->linelen;
                dvp += psd->linelen;
            }
        } else {        /* copy from top to bottom */
            int i;
            svp = (gal_uint16 *) psd->addr + y1 * psd->linelen + x1;
            dvp = (gal_uint16 *) psd->addr + y2 * psd->linelen + x2;
            for (i = 0; i < h; i++) {
                memcpy(dvp, svp, w<<1);
                svp += psd->linelen;
                dvp += psd->linelen;
            }
        }
    } else {            /* copy from bottom to top */
        int i;

        svp = (gal_uint16 *) psd->addr + (y1 + h) * psd->linelen + x1;
        dvp = (gal_uint16 *) psd->addr + (y2 + h) * psd->linelen + x2;
        for (i = 0; i < h; i++) {
            svp -= psd->linelen;
            dvp -= psd->linelen;
            memcpy (dvp, svp, w<<1);
        }
    }
}

SUBDRIVER fblinear16 = {
    linear16_init,
    linear16_drawpixel,
    linear16_readpixel,
    linear16_drawhline,
    linear16_drawvline,
    linear16_blit,
    linear16_putbox,
    linear16_getbox,
    linear16_putboxmask,
    linear16_copybox
};

#endif /* _FBLIN16_SUPPORT */

