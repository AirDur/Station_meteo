/*
** $Id: fblin4.c,v 1.19 2003/11/22 11:49:29 weiym Exp $
** 
** fblin4.c: Frame buffer 4 bpp Linear Video Driver for MiniGUI
**  This driver is suitable for the most significant bit being right.
** 
** Copyright (C) 2003, Feynman Software.
** Copyright (C) 2001, Song Lixin and Wei Yongming.
**
** 2001/01/17: Created by Song Lixin 
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
#include <stdio.h>
#include <string.h>

#include "native.h"
#include "fb.h"

#ifdef _FBLIN4R_SUPPORT

#define SHIFT(x) ((x&1)?4:0)
static unsigned char pixmask [] = {0x0f, 0xf0};
static unsigned char pixnotmask [] = {0xf0, 0x0f};

static inline gal_uint8* get_pixel_address (PSD psd, int x, int y)
{
    return (gal_uint8 *) psd->addr + y*psd->linelen + (x >> 1);
}

static inline void xsetpixel_in_byte (int x, gal_pixel c, gal_uint8 *byte)
{
    *byte = (*byte & (pixnotmask[x&1])) | ((c << SHIFT(x))^(*byte & pixmask[x&1]));
}

static inline void setpixel_in_byte (int x, gal_pixel c, gal_uint8 *byte)
{
    *byte = (*byte & (pixnotmask[x&1])) | (c << SHIFT(x));
}

static inline gal_pixel getpixel_in_byte (int x, const gal_uint8 *byte)
{
    gal_uint8 tmp = *byte;
    tmp = tmp & pixmask [x&1];
    return (tmp >> SHIFT(x));
}

static int fblin4_init (PSD psd)
{
    if (!psd->size) {
        psd->size = psd->yres * psd->linelen;
        return 1;
    }

    return 1;
}

/* Set pixel at x, y, to gal_pixel c*/
static void fblin4_drawpixel (PSD psd, int x, int y, gal_pixel c)
{
    if (psd->gr_mode == MODE_XOR)
        xsetpixel_in_byte (x, c, get_pixel_address (psd, x, y));
    else
        setpixel_in_byte (x, c, get_pixel_address (psd, x, y));
}

/* Read pixel at x, y*/
static gal_pixel fblin4_readpixel (PSD psd, int x, int y)
{
    return getpixel_in_byte (x, get_pixel_address (psd, x, y));
}

/* Draw horizontal line from x1,y to x2,y including final point*/
static void fblin4_drawhline (PSD psd, int x, int y, int w, gal_pixel c)
{
    gal_uint8 *addr;
    gal_uint8 cc;

    c = c & 0x0f;
    cc = c | (c << 4);

    addr = get_pixel_address (psd, x, y);

    if (psd->gr_mode == MODE_XOR) {
        if (x & 1) {
            xsetpixel_in_byte (1, c, addr);
            addr++;
            w--;
        }

        while (w > 1) {
            *addr++ ^= cc ;
            w -= 2;
        }

        if (w > 0) 
            xsetpixel_in_byte (0, c, addr);

    } else {
        if (x & 1) {
            setpixel_in_byte (1, c, addr);
            addr++;
            w--;
        }

        while (w > 1) {
            *addr++ = cc ;
            w -= 2;
        }

        if (w > 0) 
            setpixel_in_byte (0, c, addr);
    }
}

static void fblin4_drawvline (PSD psd, int x, int y, int h, gal_pixel c)
{
    gal_uint8* addr;

    addr = get_pixel_address (psd, x, y);

    if (psd->gr_mode == MODE_XOR) {
        while (h--) {
            xsetpixel_in_byte (x, c, addr);
            addr += psd->linelen;
        }
    }
    else {
        while (h--) {
            setpixel_in_byte (x, c, addr);
            addr += psd->linelen;
        }
    }
}

/* clip to screen */
static void fblin4_getbox (PSD psd, int x, int y, int w, int h, void* buf)
{
    int i;
    gal_uint8 *dst = (gal_uint8*) buf;
    const gal_uint8 *src;
    int d_pitch = w;

    if (y < 0) {
        h += y;
        dst += -y * d_pitch;
        y = 0;
    }
    if (x < 0) {
        w += x;
        dst += -x;
        x = 0;
    }        
    if (y + h  - 1 >= psd->yres) 
        h = psd->yres - y ;
    if (x + w  - 1 >= psd->xres) 
        w = psd->xres - x ;

    if (w <= 0 || h <= 0) return;

    w += x;
    src = get_pixel_address (psd, x, y);
    while (h--) {
        const gal_uint8 *s_line = src;
        gal_uint8 *d_line = dst;

        i = x;
        while (i < w) {
            *d_line = getpixel_in_byte (i, s_line);
            d_line++; i++;
            if (!(i&1)) s_line++;
        }

        src += psd->linelen;
        dst += d_pitch;
    }
}

/* do clip */
static void fblin4_putbox (PSD psd, int x, int y, int w, int h, void *buf)
{
    int i;
    const gal_uint8 *src = (gal_uint8*) buf;
    gal_uint8 *dst;
    int s_pitch = w;

    if (psd->doclip) {
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * s_pitch;
            y = psd->clipminy;
        }
        if (x < psd->clipminx) {
            w -= psd->clipminx - x;
            src += psd->clipminx - x;
            x = psd->clipminx;
        }
        if (y + h - 1 >= psd->clipmaxy)
            h = psd->clipmaxy- y;
        if (x + w - 1 >= psd->clipmaxx)
            w = psd->clipmaxx- x;
    }
    else {
        if ( y < 0 ) {
            h += y;
            src += -y * s_pitch;
            y = 0;
        }
        if ( x < 0 ) {
            w += x;
            src += -x;
            x = 0;
        }
        if ( y + h  -1 >= psd->yres)
            h = psd->yres - y;
        if ( x + w  -1 >= psd->xres)
            w = psd->xres - x;
    }

    if (w <= 0 || h <= 0) return;

    w += x;
    dst = get_pixel_address (psd, x, y);
    while (h--) {
        const gal_uint8 *s_line = src;
        gal_uint8 *d_line = dst;

        i = x;
        while (i < w) {
            setpixel_in_byte (i, *s_line & 0x0F, d_line);
            s_line ++; i ++;
            if (!(i&1)) d_line++;
        }
        dst += psd->linelen;
        src += s_pitch;
    }
}

static void fblin4_putboxmask (PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx)
{
    int i;
    gal_uint8 *src = (gal_uint8*) buf;
    gal_uint8 *dst;
    int s_pitch =  w ;

    if (psd->doclip) {    
        if (y < psd->clipminy) {
            h -= psd->clipminy - y;
            src += (psd->clipminy - y) * s_pitch;
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
            src += -y * s_pitch;
            y = 0;
        }
        if ( x < 0 ) {
            w += x;
            src += -x;
            x = 0;
        }        
        if ( y + h  -1 >= psd->yres) 
            h = psd->yres - y ;
        if ( x + w  -1 >= psd->xres) 
            w = psd->xres - x ;
    }

    if (w <= 0 || h <= 0) return;

    cxx = cxx & 0x0f;
    w += x;
    dst = get_pixel_address (psd, x, y);
    while (h--) {
        gal_uint8 *s_line = src;
        gal_uint8 *d_line = dst;

        i = x;
        while (i < w) {
            gal_uint8 spix = *s_line & 0x0f;

            if (spix != cxx)
                setpixel_in_byte (i, spix, d_line);

            s_line++; i++;
            if (!(i&1)) d_line ++;
        }
        dst += psd->linelen;
        src += s_pitch;
    }
}

static unsigned char right_side_mask [] = {0xff, 0xf0};
static unsigned char left_side_mask [] = {0x00, 0x0f};
static unsigned char right_ban_mask [] = {0x00, 0xf0};
static unsigned char left_ban_mask [] = {0x00, 0x0f};

static void fblin4_copyline_lr (gal_uint8 *src, int x1, gal_uint8 *dst, int x2, int w)
{
    int s_head = x1 & 1;
    int d_head = x2 & 1;

    if (s_head == d_head) {
        if (d_head) {
            *dst = (*src & right_side_mask [d_head]) | (*dst & left_side_mask [d_head]);
            src ++; dst ++; w--;
        }

        while (w > 1) {
            *dst++ = *src++;
            w -= 2;
        }

        if (w) {
            *dst = (*src & left_side_mask [w]) | (*dst & (right_side_mask [w]));
        }
    }
    else if (d_head > s_head) {

        gal_uint8 sl_half = *src << 4;
        gal_uint8 sr_half;
        gal_uint8 sb_new;

        *dst = (sl_half & right_side_mask [d_head]) | (*dst & left_side_mask [d_head]);
        dst++;
        w--;

        sr_half = *src & right_ban_mask [1];
        while (w > 1) {
            src++;
            sl_half = *src << 4;
            sb_new = sl_half | (sr_half >> 4);
            sr_half = *src & right_ban_mask [1];

            *dst = sb_new;
            dst++; 
            w -= 2;
        }

        if (w) {
            sr_half = *src & right_ban_mask [1];
            src++;
            sl_half = *src << 4;
            sb_new = sl_half | (sr_half >> 4);

            *dst = (sb_new & left_side_mask [w]) | (*dst & right_side_mask[w]);
        }
    }
    else /* s_head > d_head */
    {
        gal_uint8 sl_half;
        gal_uint8 sr_half;
        gal_uint8 sb_new;

        while (w > 1) {
            sl_half = *src >> 4;
            sr_half = *(++src) & (left_ban_mask [1]);
            sb_new = sl_half | (sr_half << 4);

            *dst = sb_new;
            dst++; 
            w -= 2;
        }

        if (w) {
            sl_half = *src >> 4;
            sr_half = *(++src) & (left_ban_mask [1]);
            sb_new = sl_half | (sr_half << 4);

            *dst = (sb_new & left_side_mask [w]) | (*dst & right_side_mask[w]);
        }
    }
}

static void fblin4_copybox (PSD psd,int x1, int y1, int w, int h, int x2, int y2)
{
    register gal_uint8 *src, *dst;
    register int linelen = psd->linelen;

    if (y1 == y2) {
        if (x1 < x2 && x1 + w > x2) {
            //gal_uint8 *tmp = alloca (w >> 1);
            gal_uint8 *tmp = malloc (w >> 1);

            src= psd->addr + y1 * linelen + (x1 >> 1);
            dst= psd->addr + y2 * linelen + (x2 >> 1);

            while (h) {
                fblin4_copyline_lr (src, x1, tmp, 0, w);
                fblin4_copyline_lr (tmp, 0, dst, x2, w);
                src += linelen;
                dst += linelen;
                h--;
            }
            free (tmp);
        }
        else {
            src= psd->addr + y1 * linelen + (x1 >> 1);
            dst= psd->addr + y2 * linelen + (x2 >> 1);

            while (h) {
                fblin4_copyline_lr (src, x1, dst, x2, w);
                src += linelen;
                dst += linelen;
            }
        }
    }
    else if (y1 < y2 && y1 + h >= y2) {
        y1 += (h-1);
        y2 += (h-1);

        src = psd->addr + y1 * linelen + (x1 >> 1);
        dst = psd->addr + y2 * linelen + (x2 >> 1);

        while (h) {
            fblin4_copyline_lr (src, x1, dst, x2, w);
            src -= linelen;
            dst -= linelen;
            h--;
        }
    }
    else {
        src= psd->addr + y1 * linelen + (x1 >> 1);
        dst= psd->addr + y2 * linelen + (x2 >> 1);

        while (h) {
            fblin4_copyline_lr (src, x1, dst, x2, w);
            src += linelen;
            dst += linelen;
            h--;
        }
    }
}

/* blit, no clipping */
static void fblin4_blit (PSD dstpsd, int dstx, int dsty, int w, int h,
    PSD srcpsd, int srcx, int srcy)
{
    gal_uint8* dst;
    gal_uint8* src;

    dst = get_pixel_address (dstpsd, dstx, dsty);
    src = get_pixel_address (srcpsd, srcx, srcy);

    while (h) {
        fblin4_copyline_lr (src, srcx, dst, dstx, w);
        src += srcpsd->linelen;
        dst += dstpsd->linelen;
        h--;
    }
}

SUBDRIVER fblinear4 = {
    fblin4_init,
    fblin4_drawpixel,
    fblin4_readpixel,
    fblin4_drawhline,
    fblin4_drawvline,
    fblin4_blit,
    fblin4_putbox,
    fblin4_getbox,
    fblin4_putboxmask,
    fblin4_copybox
};

#endif /* _FBLIN4R_SUPPORT */

