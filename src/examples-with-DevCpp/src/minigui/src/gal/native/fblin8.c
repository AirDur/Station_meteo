/*
** $Id: fblin8.c,v 1.18 2005/01/08 11:05:14 panweiguo Exp $
**
** fblin8.c: 8bpp Linear Video Driver for MiniGUI
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 Song Lixin and Wei Yongming
** 
** 2000/10/20: Create by Song Lixin
**
** 2003/07/10: Cleanup by Wei Yongming.
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

#ifdef _FBLIN8_SUPPORT

extern unsigned int (*frameBuffer256)[640/4];

static int linear8_init (PSD psd)
{
    if (!psd->size) {
        psd->size = psd->yres * psd->linelen;
        /* convert linelen from byte to pixel len for bpp 16, 24, 32*/
    }
    return 1;
}

/* Set pixel at x, y, to gal_pixel c*/
static void linear8_drawpixel (PSD psd, int x, int y, gal_pixel c)
{
    if (psd->gr_mode == MODE_XOR)
       *((unsigned char *)&frameBuffer256[(y)][(x)>>2] + (3-(x&0x3))) = 
       *((unsigned char *)&frameBuffer256[(y)][(x)>>2] + (3-(x&0x3)))^c;
       else
       *((unsigned char *)&frameBuffer256[(y)][(x)>>2] + (3-(x&0x3))) =c;
}

/* Read pixel at x, y*/
static gal_pixel linear8_readpixel (PSD psd, int x, int y)
{
    return (gal_pixel)*((unsigned char *)&frameBuffer256[(y)][(x)>>2] + (3-(x&0x3)));
}

/* Draw horizontal line from x1,y to x2,y including final point*/
static void linear8_drawhline (PSD psd, int x, int y, int w, gal_pixel c)
{
    while (w --)
            linear8_drawpixel (psd,x++,y,c);
}

#if 0
/*do clip*/
static void linear8_puthline (PSD psd,int x, int y, int w, void *buf)
{
    gal_uint8    *mem;
    gal_uint8    *src;
    
    src = (gal_uint8 *) buf;
    
    if (psd->doclip) {
        if (x < psd->clipminx) {
            src += psd->clipminx - x;
            w   -= psd->clipminx - x; 
            x    = psd->clipminx;
        }
        if (x + w - 1 >= psd->clipmaxx) {
            w    = psd->clipmaxx - x ;
        }        

    } else {
        if (x < 0) {
            src -= x;
            w   += x;
            x    = 0;
        }
        if (x + w -1 >= psd->xres) {
            w    = psd->xres - x;
        }
    }

    mem = (gal_uint8 *)(psd->addr) + y * psd->linelen  + x;
    memcpy(mem,src,w);    
}

/*do clip*/
static void linear8_gethline (PSD psd, int x, int y, int w, void *buf)
{
    gal_uint8    *mem;
    gal_uint8    *dst;
    
    dst = (gal_uint8 *) buf;

    if (x < 0) {
        dst -= x;
        w   += x;
        x    = 0;
    }
    if (x + w -1 >= psd->xres) {
        w    = psd->xres - x;
    }

    mem = (gal_uint8 *)(psd->addr) + (y * psd->linelen + x);
    memcpy(dst,mem,w);    
}
#endif

static void linear8_drawvline(PSD psd, int x, int y, int h, gal_pixel c)
{
    while (h --)
            linear8_drawpixel (psd,x,y++,c);
}

#if 0
/*do clip*/
static void linear8_putvline (PSD psd,int x, int y, int h, void *buf)
{
    gal_uint8     *mem;
    gal_uint8    *src;
    
    src= (gal_uint8 *) buf;

    if (psd->doclip) {
        if (y < psd->clipminy) {
            src += psd->clipminy - y;
            h   -= psd->clipminy - y; 
            y    = psd->clipminy;
        }
        if (y + h - 1 >= psd->clipmaxy) {
            h    = psd->clipmaxy - y ;
        }        

    } else {
        if (y < 0) {
            src -= y;
            h   += y;
            y    = 0;
        }
        if (y + h -1 >= psd->yres) {
            h    = psd->yres - y;
        }
    }

    mem = (gal_uint8 *)(psd->addr) + y * psd->linelen + x;

    while (h --) {
        *mem = *src++;
        mem += psd->linelen;
    }
}

/*do clip*/
static void linear8_getvline (PSD psd, int x, int y, int h, void *buf)
{
    gal_uint8    *mem;
    gal_uint8    *dst;
    
    dst= (gal_uint8 *) buf;

    if (y < 0) {
        dst -= y;
        h   += y;
        y    = 0;
    }
    if (y + h -1 >= psd->yres) {
        h    = psd->yres - y;
    }

    mem = (gal_uint8 *)(psd->addr) + y * psd->linelen + x;

    while (h --) {
        *dst++  = *mem;
        mem+= psd->linelen;
    }
    return;
}
#endif

/* srccopy bitblt,not do clip*/
static void linear8_blit (PSD dstpsd, int dstx, int dsty, int w, int h,
    PSD srcpsd, int srcx, int srcy)
{
    gal_uint8 *buf;
    int i,j;

    buf=malloc(w*h);

    for(j=srcy;j<(srcy+h);j++)
       {
       for(i=srcx;i<(srcx+w);i++)
          *(buf +(j-srcy)*w+(i-srcx))=(gal_uint8) *((unsigned char *)&frameBuffer256[(j)][(i)>>2] + (3-(i&0x3)));
       }

    for(j=dsty;j<(dsty+h);j++)
       {
       for(i=dstx;i<(dstx+w);i++)
          *((unsigned char *)&frameBuffer256[(j)][(i)>>2] + (3-(i&0x3))) = *(buf +(j-dsty)*w+(i-dstx));
       }

    free(buf);
}

/*do clip*/
static void linear8_putbox ( PSD psd, int x, int y, int w, int h, void *buf)
{
    gal_uint8 *src;
    int srcwidth;
    int i,j;

    src = (gal_uint8*) buf;        
    srcwidth = w;

/*
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
*/
    if (w <= 0 || h<=0) return;

    for(j=y;j<(y+h);j++)
       {
       if(((j>=psd->clipminy)&&(j<=psd->clipmaxy))|| !(psd->doclip))
          {
          for(i=x;i<(x+w);i++)
             {
             if(((i>=psd->clipminx)&&(i<=psd->clipmaxx))|| !(psd->doclip))
                *((unsigned char *)&frameBuffer256[(j)][(i)>>2] + (3-(i&0x3))) = *(src +(j-y)*w+(i-x));
             }
          }
       }
}

/*clip to screen*/
static void linear8_getbox ( PSD psd, int x, int y, int w, int h, void* buf )
{
    gal_uint8 *dst;
    int dst_pitch;
    int i,j;

    dst = (gal_uint8*) buf;
    dst_pitch = w;
/*
    if ( y < 0 ) {
        h += y;
        dst -= y * dst_pitch;
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
*/
    if (w <= 0 || h <= 0) return;

    for(j=y;j<(y+h);j++)
       {
       if((j>=0)&&(j<=psd->yres))
          {
          for(i=x;i<(x+w);i++)
             {
             if((i>=0)&&(i<=psd->xres))
                *(dst +(j-y)*w+(i-x))=(gal_uint8) *((unsigned char *)&frameBuffer256[(j)][(i)>>2] + (3-(i&0x3)));
             }
          }
       }
}


static void linear8_putboxmask (PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx)
{
    gal_uint8 c;
    gal_uint8 *src;
    int i,j;
    int srcwidth;
     
    src= (gal_uint8*) buf;
    srcwidth =  w;
/*
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
*/
    if (w <= 0 || h<=0) return;

    for(j=y;j<(y+h);j++)
       {
       if(((j>=psd->clipminy)&&(j<=psd->clipmaxy))|| !(psd->doclip))
          {
          for(i=x;i<(x+w);i++)
             {
             if(((i>=psd->clipminx)&&(i<=psd->clipmaxx))|| !(psd->doclip))
                {
                c = *(src +(j-y)*w+(i-x));
                if(c != cxx )
                   *((unsigned char *)&frameBuffer256[(j)][(i)>>2] + (3-(i&0x3))) =c;
                }
             }
          }
       }
}


static    void linear8_copybox(PSD psd,int x1, int y1, int w, int h, int x2, int y2)
{
    gal_uint8 *buf;

    buf=malloc(w*h);
    linear8_getbox(psd,x1,y1,w,h,buf);
    linear8_putbox(psd,x2,y2,w,h,buf);
    free(buf);
}

SUBDRIVER fblinear8 = {
    linear8_init,
    linear8_drawpixel,
    linear8_readpixel,
    linear8_drawhline,
    linear8_drawvline,
    linear8_blit,
    linear8_putbox,
    linear8_getbox,
    linear8_putboxmask,
    linear8_copybox
};

#endif /* _FBLIN8_SUPPORT */

