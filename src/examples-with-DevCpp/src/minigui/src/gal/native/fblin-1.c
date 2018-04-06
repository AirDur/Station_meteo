/*
** $Id: fblin-1.c,v 1.6 2004/04/19 09:08:20 snig Exp $
** 
** fblin-1.c: Frame buffer 1 bpp linear video driver for MiniGUI
**  This driver is suitable for the most significant bit being left.
**
** Copyright (C) 2003 Feynman Software.
**
** Create date: 2003/07/10
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

#ifdef _FBLIN1L_SUPPORT

#define SHIFT(x) (7 - (x & 7))
static unsigned char pixmask [] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
static unsigned char pixnotmask [] = {0x7f,0xbf,0xdf,0xef,0xf7,0xfb,0xfd,0xfe};

#define normalize_pixel(c) ((c&1)?1:0)

static inline gal_uint8* get_pixel_address (PSD psd, int x, int y)
{
    return (gal_uint8 *) psd->addr + y*psd->linelen + (x >> 3);
}

static inline void xsetpixel_in_byte (int x, gal_pixel c, gal_uint8 *byte)
{
    *byte = (*byte & (pixnotmask[x&7])) | ((c << SHIFT(x))^(*byte & pixmask[x&7]));
}

static inline void setpixel_in_byte (int x, gal_pixel c, gal_uint8 *byte)
{
    *byte = (*byte & (pixnotmask[x&7])) | (c << SHIFT(x));
}

static inline gal_pixel getpixel_in_byte (int x, const gal_uint8 *byte)
{
    gal_uint8 tmp = *byte;
    tmp = tmp & pixmask[x&7];
    return (tmp >> SHIFT(x));
}

static int fblin1_init (PSD psd)
{
    if (!psd->size) {
        psd->size = psd->yres * psd->linelen;
        return 1;
    }

    return 1;
}

/* Set pixel at x, y, to gal_pixel c */
static void fblin1_drawpixel (PSD psd, int x, int y, gal_pixel c)
{
    c = normalize_pixel (c);

    if (psd->gr_mode == MODE_XOR)
        xsetpixel_in_byte (x, c, get_pixel_address (psd, x, y));
    else
        setpixel_in_byte (x, c, get_pixel_address (psd, x, y));
}

/* Read pixel at x, y */
static gal_pixel fblin1_readpixel (PSD psd, int x, int y)
{
    return getpixel_in_byte (x, get_pixel_address (psd, x, y));
}

/* Draw horizontal line from x1,y to x2,y including final point */
static void fblin1_drawhline (PSD psd, int x, int y, int w, gal_pixel c)
{
    gal_uint8 *addr;
    int cc, i;

    c = normalize_pixel (c);
    cc = c  | (c << 1);
    cc = cc | (cc << 2);
    cc = cc | (cc << 4);
    
    addr = get_pixel_address (psd, x, y);

    if (psd->gr_mode == MODE_XOR) {
        if (x & 7) {
            for (i = x & 7; i & 7 && w > 0; i++) {
                xsetpixel_in_byte (i, c, addr);
                w--;
            }
            addr++;
        }
        
        while (w > 7) {
            *addr++ ^= cc;
            w -= 8;
        }
        
        if (w > 0) {
            for (i = 0; i < w; i++)
                xsetpixel_in_byte (i, c, addr);
        }
        
    } else {
        if (x&7) {
            for (i = x & 7; i & 7 && w > 0; i++) {
                setpixel_in_byte (i, c, addr);
                w--;
            }
            addr++;
        }
        
        while (w > 7) {
            *addr++ = cc;
            w -= 8;
        }
        
        if (w > 0) {
            for (i = 0; i < w; i++)
                setpixel_in_byte (i, c, addr);
        }
    }
}

static void fblin1_drawvline (PSD psd, int x, int y, int h, gal_pixel c)
{
    gal_uint8 *addr;

    c = normalize_pixel (c);
    addr = get_pixel_address(psd, x, y);    

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
static void fblin1_getbox (PSD psd, int x, int y, int w, int h, void* buf)
{
    int i;
    gal_uint8 *dst = (gal_uint8*) buf;
    const gal_uint8 *src;
    int d_pitch = w;

    if (y < 0) {
        h += y;
        dst -= y * d_pitch;
        y = 0;
    }
    if (x < 0) {
        w += x;
        dst -= x;
        x = 0;
    }
    if (y + h - 1 >= psd->yres) 
        h = psd->yres - y ;
    if (x + w - 1 >= psd->xres) 
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
            if (!(i&7)) s_line++;
        }

        src += psd->linelen;
        dst += d_pitch;
    }
}

/* do clip */
static void fblin1_putbox (PSD psd, int x, int y, int w, int h, void *buf)
{
    int i;
    gal_uint8 *src = (gal_uint8*) buf;
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
            setpixel_in_byte (i, normalize_pixel (*s_line), d_line);
            s_line ++; i ++;
            if (!(i&7)) d_line++;
        }
        dst += psd->linelen;
        src += s_pitch;
    }
}

/* do clip */
static void fblin1_putboxmask (PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx)
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

    cxx = normalize_pixel (cxx);

    w += x;
    dst = get_pixel_address (psd, x, y);
    while (h--) {
        gal_uint8 *s_line = src;
        gal_uint8 *d_line = dst;

        i = x;
        while (i < w) {
            gal_uint8 spix = normalize_pixel (*s_line);

            if (spix != cxx)
                setpixel_in_byte (i, spix, d_line);

            s_line++; i++;
            if (!(i&7)) d_line ++;
        }
        dst += psd->linelen;
        src += s_pitch;
    }
}

static unsigned char right_side_mask [] = {0xff,0x7f,0x3f,0x1f,0x0f,0x07,0x03,0x01,0x00};
static unsigned char left_side_mask [] = {0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};
static unsigned char right_ban_mask [] = {0x00,0x01,0x03,0x07,0x0f,0x1f,0x3f,0x7f,0xff};
static unsigned char left_ban_mask [] = {0x00,0x80,0xc0,0xe0,0xf0,0xf8,0xfc,0xfe,0xff};

/* copy line from left to right */
static void fblin1_copyline_lr (gal_uint8 *src, int x1, gal_uint8 *dst, int x2, int w)
{
    int s_head = x1 & 7;
    int d_head = x2 & 7;

    if (s_head == d_head) {
        if (d_head) {
            if (d_head + w > 8) {
                *dst = (*src & right_side_mask [d_head]) | (*dst & left_side_mask [d_head]);
                src ++; dst ++;
                w -= 8 - d_head;
            }
            else {
                gal_uint8 mask = left_ban_mask [w] >> d_head;
                *dst = (*src & mask) | (*dst & ~mask);
                return;
            }
        }

        while (w > 7) {
            *dst++ = *src++;
            w -= 8;
        }

        if (w) {
            *dst = (*src & left_side_mask [w]) | (*dst & (right_side_mask [w]));
        }
    }
    else if (d_head > s_head) {

        int shift = d_head - s_head;
        gal_uint8 sl_half = *src >> shift;
        gal_uint8 sr_half;
        gal_uint8 sb_new;

        if (d_head + w > 8) {
            *dst = (sl_half & right_side_mask [d_head]) | (*dst & left_side_mask [d_head]);
            dst++;
            w -= 8 - d_head;
        }
        else {
            gal_uint8 mask = left_ban_mask [w] >> d_head;
            *dst = (sl_half & mask) | (*dst & ~mask);
            return;
        }

        sr_half = *src & right_ban_mask [shift];
        while (w > 7) {
            src++;
            sl_half = *src >> shift;
            sb_new = sl_half | (sr_half << (8 - shift));
            sr_half = *src & right_ban_mask [shift];

            *dst = sb_new;
            dst++; 
            w -= 8;
        }

        if (w) {
            sr_half = *src & right_ban_mask [shift];
            if (w > shift) {
                src++;
                sl_half = *src >> shift;
                sb_new = sl_half | (sr_half << (8 - shift));
            }
            else
                sb_new = (sr_half << (8 - shift));

            *dst = (sb_new & left_side_mask [w]) | (*dst & right_side_mask[w]);
        }
    }
    else /* s_head > d_head */
    {
        int shift = s_head - d_head;

        gal_uint8 sl_half = *src << shift;
        gal_uint8 sr_half;
        gal_uint8 sb_new;

        if (s_head + w > 8) {
            sr_half = *(src + 1) & (left_ban_mask [shift]);
            sb_new = sl_half | (sr_half >> (8 - shift));
        }
        else
            sb_new = sl_half;

        if (d_head) {
            if (d_head + w > 8) {
                *dst = (sb_new & right_side_mask [d_head]) | (*dst & left_side_mask [d_head]);
                dst++; src++;
                w -= 8 - d_head;
            }
            else {
                gal_uint8 mask = left_ban_mask [w] >> d_head;
                *dst = (sb_new & mask) | (*dst & ~mask);
                return;
            }
        }

        while (w > 7) {
            sl_half = *src << shift;
            sr_half = *(++src) & (left_ban_mask [shift]);
            sb_new = sl_half | (sr_half >> (8 - shift));

            *dst = sb_new;
            dst++; 
            w -= 8;
        }

        if (w) {
            sl_half = *src << shift;
            if (w > shift) {
                sr_half = *(++src) & (left_ban_mask [shift]);
                sb_new = sl_half | (sr_half >> (8 - shift));
            }
            else
                sb_new = sl_half;

            *dst = (sb_new & left_side_mask [w]) | (*dst & right_side_mask[w]);
        }
    }
}

static void fblin1_copybox (PSD psd, int x1, int y1, int w, int h, int x2, int y2)
{
    register gal_uint8 *src, *dst;
    register int linelen = psd->linelen;

    if (y1 == y2) {
        if (x1 < x2 && x1+w > x2) {
            //gal_uint8 *tmp = alloca (w >> 3);
            //FIXME
            gal_uint8 *tmp = malloc (w >> 3);

            src= (gal_uint8 *)psd->addr + y1 * linelen + (x1 >> 3);
            dst= (gal_uint8 *)psd->addr + y2 * linelen + (x2 >> 3);

            while (h) {
                fblin1_copyline_lr (src, x1, tmp, 0, w);
                fblin1_copyline_lr (tmp, 0, dst, x2, w);
                src += linelen;
                dst += linelen;
                h--;
            }
            free (tmp);
        }
        else {
            src= (gal_uint8 *)psd->addr + y1 * linelen + (x1 >> 3);
            dst= (gal_uint8 *)psd->addr + y2 * linelen + (x2 >> 3);

            while (h) {
                fblin1_copyline_lr (src, x1, dst, x2, w);
                src += linelen;
                dst += linelen;
                h--;
            }
        }
    }
    else if (y1 < y2 && y1 + h >= y2) {
        y1 += (h-1);
        y2 += (h-1);

        printf ("x1: %d, x2: %d, s_head: %d, d_head: %d\n", x1, x2, x1 & 7, x2 & 7);

        src= (gal_uint8 *)psd->addr + y1 * psd->linelen + (x1 >> 3);
        dst= (gal_uint8 *)psd->addr + y2 * psd->linelen + (x2 >> 3);

        while (h) {
            fblin1_copyline_lr (src, x1, dst, x2, w);
            src -= linelen;
            dst -= linelen;
            h--;
        }
    }
    else {
        src= (gal_uint8 *)psd->addr + y1 * psd->linelen + (x1 >> 3);
        dst= (gal_uint8 *)psd->addr + y2 * psd->linelen + (x2 >> 3);

        printf ("x1: %d, x2: %d, s_head: %d, d_head: %d\n", x1, x2, x1 & 7, x2 & 7);

        while (h) {
            fblin1_copyline_lr (src, x1, dst, x2, w);
            src += linelen;
            dst += linelen;
            h--;
        }
    }
}

/* blit, no clipping */
static void fblin1_blit (PSD dstpsd, int dstx, int dsty, int w, int h,
    PSD srcpsd, int srcx, int srcy)
{
    gal_uint8* dst;
    gal_uint8* src;

    dst = get_pixel_address (dstpsd, dstx, dsty);
    src = get_pixel_address (srcpsd, srcx, srcy);

    while (h) {
        fblin1_copyline_lr (src, srcx, dst, dstx, w);
        src += srcpsd->linelen;
        dst += dstpsd->linelen;
        h--;
    }
}

SUBDRIVER fblinear_1 = {
    fblin1_init,
    fblin1_drawpixel,
    fblin1_readpixel,
    fblin1_drawhline,
    fblin1_drawvline,
    fblin1_blit,
    fblin1_putbox,
    fblin1_getbox,
    fblin1_putboxmask,
    fblin1_copybox
};

#endif /* _FBLIN1L_SUPPORT */

