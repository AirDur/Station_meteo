/*
** $Id: native.c,v 1.64 2005/01/08 07:54:15 panweiguo Exp $
**
** native.c: Native Low Level Graphics Engine's interface file
**
** Copyright (C) 2002, 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Song Lixin, Wei Yongming, and Chen Lei.
**
** Add code for MiniGUI-Lite by Wei Yongming, 2001/01/01
** Clean code for MiniGUI 1.1.x by Wei Yongming, 2001/09/18
** Coordinates transfering for iPAQ by Chen Lei, 2001/12/12
** Cleanup for coordinates transfering for iPAQ by Wei Yongming, 2003/05/21
**
** Created by Song Lixin, 2000/10/18
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
#include <signal.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "gal.h"
#include "native.h"

#ifndef _COOR_TRANS
#define _COOR_TRANS     0
#define _ROT_DIR_CW    0
#endif

#if defined(_LITE_VERSION) && !(_STAND_ALONE)

#include "ourhdr.h"
#include "client.h"
#include "sharedres.h"
#include "drawsemop.h"

static void rotatecoor (int *x1, int *y1, int *x2, int *y2, int Tw, int dir);

#define BLOCK_DRAW_SEM                                  \
    if (!mgIsServer && __mg_cur_gfx->phygc.psd == gc.psd)    \
        lock_draw_sem ();                               \
    if (((!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) || __mg_switch_away)) \
        goto leave_drawing;

#define UNBLOCK_DRAW_SEM                                \
    if (!mgIsServer && __mg_cur_gfx->phygc.psd == gc.psd)    \
leave_drawing:                                          \
        unlock_draw_sem ()

#else

#define BLOCK_DRAW_SEM
#define UNBLOCK_DRAW_SEM

#endif

#ifdef _DEBUG
#define debug(fmt, args...)        fprintf(stderr, " <native>-%s\t" fmt, __FUNCTION__, ##args);
#else
#define debug(fmt, args...)
#endif

/*
 * leon -- the following drafts will help us
 */

#if 0

    Drafts:
    rects:

          -/-------------------  Tw  --------------------/-
    
    
          Porg                    Px1      Px2          Vorg
                                  Vy2      Vy1
        Vy <--------------------------------------------->  Px   -/-
           |                      |        |             |        |
           |                      |   Pw   |             |        |
      Py1  |---------------------------------------------|Vx1     |
           |                      |        |             |        |
           |                      |        |             |        |
           |                   Ph |        |Vw           |
           |                      |        |             |        Th
           |                      |        |             |
           |                      |  Vh    |             |        |
      Py2  |---------------------------------------------|Vx2     |
           |                                             |        |
           |                                             |        |
           |                                             |        |
           |                                             |       -/-
    
           Py                                            Vx


        Tw: Total width
        Th: Total height

        Porg:   Physical origal               Vorg:    Virtual origal
        Px1:    Physical x1                   Vx1:     Virtual x1
        Px2:    Physical x2                   Vx2:     Virtual x2
        Py1:    Physical y1                   Vy1:     Virtual y1
        Py2:    Physical y2                   Vy2:     Virtual y2
        Pw:     Physical width                Vw:      Virtual width
        Ph:     Physical height               Vh:      Virtual height

//////////////////////////////////////////////////////////////////////

    for the buffer refered by putbox, getbox:
        
       -/---      Tw     ---/-
              Px0
        Po    Vy0           Vo
    Vy    <-------------------> Px  -/-
        |     |             |        |
        |     |             |        |
        |     |             |        |
        |     | P0          |         
    Py0    |_____._____________| Vx0    Th
        |                   |         
        |-------------------|        |
        |                   |        |
        Py                  Vx      -/-

#endif

/*
 * help rutines for coordinate transfer begin
 */
static void swapval (int* v1, int* v2)
{
    int tmp;

    tmp = *v1;
    *v1 = *v2;
    *v2 = tmp;
}

static void rotatecoor (int *x1, int *y1, int *x2, int *y2, int Tw, int dir /* 0: V->P, 1: P->V */)
{
    int tmp1, tmp2;

    switch (dir) {
        case 0:
            tmp1 = *x1;
            tmp2 = *x2;
            *x1 = Tw - 1 - *y2;
            *x2 = Tw - 1 - *y1;
            *y1 = tmp1;
            *y2 = tmp2;
            break;
        case 1:
            tmp1 = *y1;
            tmp2 = *y2;
            *y1 = Tw - 1 - *x2;
            *y2 = Tw - 1 - *x1;
            *x1 = tmp1;
            *x2 = tmp2;
            break;
    }
}

static void rotaterect (int* x, int* y, int* w, int* h, int Tw, int dir /* 0: V->P, 1: P->V */)
{
    int tmp;

    switch (dir) {
        case 0:
            tmp = *y;
            *y = *x;
            *x = Tw - (tmp + *h);
            swapval (w, h);
            break;
        case 1:
            tmp = *x;
            *x = *y;
            *y = Tw - (tmp + *w);
            swapval (w, h);
            break;
    }
}

static void rotatepoint (int* x, int* y, int Tw , int dir /* 0: V->P, 1: P->V */)
{
    int tmp;

    switch (dir) {
        case 0:
            tmp = *x;
            *x = Tw  - 1 - *y;
            *y = tmp;
            break;

        case 1:
            tmp = *y;
            *y = Tw - 1 - *x;
            *x = tmp;
            break;
    }
}

static void reverse_buff (char* dst, const char* src, int size, int Bpp)
{
    int i;

    switch (Bpp) {
    case 1:
        for (i = 0; i < size; i++)
            dst [i] = src [size - i - 1];
        break;
    case 2:
        {
            Uint16* _dst;
            Uint16* _src;
            _dst = (Uint16*)dst;
            _src = (Uint16*)src;
            for (i = 0; i < size; i++)
                _dst [size - i - 1] = _src [i];
            break;
        }
    case 3:
        {
            for (i = 0; i < size * Bpp; i+=3) {
                dst [size - i - 1] = src [i];
                dst [size - i - 2] = src [i + 1];
                dst [size - i - 3] = src [i + 2];
            }
            break;
        }
    case 4:
        {
            Uint16* _dst;
            Uint16* _src;
            _dst = (Uint16*)dst;
            _src = (Uint16*)src;
            for (i = 0; i < size; i++)
                _dst [size - i - 1] = _src [i];
            break;
        }
    default:
        return;
    }
}

/*
 * help rutines for coordinate transfer end
 */


/*
 * Low Level Graphics Operations
 */
static int bytesperpixel (GAL_GC gc) 
{ 
    return (gc.psd->bpp + 7) / 8; 
}

static int bitsperpixel (GAL_GC gc) 
{ 
    return gc.psd->bpp; 
}

static int width (GAL_GC gc) 
{ 
    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        return gc.psd->yres; 
    else
        return gc.psd->xres; 
}

static int height (GAL_GC gc) 
{ 
    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        return gc.psd->xres; 
    else
        return gc.psd->yres; 
}

static int colors (GAL_GC gc) 
{    
    return gc.psd->ncolors; 
}

static int setclipping (GAL_GC gc, int x1, int y1, int x2, int y2)
{

    PSD psd;
    psd = gc.psd;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 > psd->yres - 1) x2 = psd->yres - 1;
        if (y2 > psd->xres - 1) y2 = psd->xres - 1;
    }
    else {
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 > psd->xres - 1) x2 = psd->xres - 1;
        if (y2 > psd->yres - 1) y2 = psd->yres - 1;
    }

    psd->doclip = 1;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotatecoor (&x1, &y1, &x2, &y2, _ROT_DIR_CW?psd->xres:psd->yres, _ROT_DIR_CW?0:1);
        
    psd->clipminx = x1;
    psd->clipminy = y1;
    psd->clipmaxx = x2 + 1;
    psd->clipmaxy = y2 + 1;

    return 0;
}

static void enableclipping (GAL_GC gc)
{
    PSD psd;
    psd = gc.psd;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        setclipping (gc, 0, 0, psd->yres - 1, psd->xres - 1);
    else
        setclipping (gc, 0, 0, psd->xres - 1, psd->yres - 1);
}

static void disableclipping (GAL_GC gc)
{
    PSD psd;
    psd = gc.psd;
    psd->doclip = 0;
}

static int getclipping (GAL_GC gc, int* x1, int* y1, int* x2, int* y2)
{
    PSD psd;
    psd = gc.psd;

    *x1 = psd->clipminx;
    *y1 = psd->clipminy;
    *x2 = psd->clipmaxx - 1;
    *y2 = psd->clipmaxy - 1;    

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotatecoor (x1, y1, x2, y2, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?1:0);

    return 0;
}

/*
 * Allocation and release of graphics context
 */
static int allocategc (GAL_GC gc, int width, int height, int depth, GAL_GC* newgc)
{
    int linelen, size;
    PSD newpsd;
    void* pixels;
    int bpp;

    bpp = gc.psd->bpp;

    newpsd = gc.psd->AllocateMemGC (gc.psd);
    if (!newpsd)
        return -1;
    
    if (!native_gen_calcmemgcalloc (newpsd, width, height, 0, bpp, &size, &linelen))
        goto fail;
    
    pixels = malloc(size);
    if (!pixels)
        goto fail;

    if (gc.psd->flags & PSF_MSBRIGHT)
        newpsd->flags |= PSF_MSBRIGHT;
    newpsd->flags |= PSF_ADDRMALLOC;

    if (!newpsd->MapMemGC (newpsd, width, height, gc.psd->planes, bpp, linelen, size, pixels))
        goto fail;

    newgc->psd = newpsd;

    setclipping (*newgc, 0, 0, width - 1, height - 1);
    return 0;

fail:
    newpsd->FreeMemGC (newpsd);
    return -1;
}

static void freegc (GAL_GC gc)
{
    PSD psd;
    psd = gc.psd;
    if(gc.psd->flags & PSF_ADDRMALLOC)
        free (gc.psd->addr);
    psd->FreeMemGC (psd);
}

/*
 * Background and foreground colors
 */
static int getbgcolor (GAL_GC gc, gal_pixel* color)
{
    PSD psd;
    psd = gc.psd;
    *color = psd->gr_background;    
    return 0;
}

static int setbgcolor (GAL_GC gc, gal_pixel color)
{
    PSD psd;
    psd = gc.psd;
    psd->gr_background = color;    
    return 0;
}

static int getfgcolor (GAL_GC gc, gal_pixel* color)
{
    PSD psd;
    psd = gc.psd;
    *color = psd->gr_foreground;    
    return 0;
}

static int setfgcolor (GAL_GC gc, gal_pixel color)
{
    PSD psd;
    psd = gc.psd;
    psd->gr_foreground = color;    
    return 0;
}

/*
 * Convertion between GAL_Color and gal_pixel
 * borrowed  from gl lib.
 */
static gal_pixel mapcolor (GAL_GC gc, GAL_Color *color)
{
    unsigned int v;

    switch (gc.psd->bpp) {
    case 1:
        if ((color->b + color->g + color->r) > 128*3)
            v = 1;
        else
            v = 0;
        return v;

    case 2:
        v = 0;
        if (color->b >= 64)
            v += 1;
        if (color->g >= 64)
            v += 2;
        if (color->r >= 64)
            v += 4;
        if (color->b >= 192 || color->g >= 192 || color->r >= 192)
            v += 8;
        v>>=2;
        return v;

    case 4:
        /* Now this is real fun. Map to standard EGA palette. */
        v = 0;
        if (color->b >= 64)
            v += 1;
        if (color->g >= 64)
            v += 2;
        if (color->r >= 64)
            v += 4;
        if (color->b >= 192 || color->g >= 192 || color->r >= 192)
            v += 8;
        return v;
    case 8:
        return RGB2PIXEL332 (color->r, color->g, color->b);
    case 15:
        return RGB2PIXEL555 (color->r, color->g, color->b);
    case 16:
        return RGB2PIXEL565 (color->r, color->g, color->b);
    case 24:
    case 32:
        return RGB2PIXEL888 (color->r, color->g, color->b);
    }
    return -1;
}

static int unmappixel (GAL_GC gc, gal_pixel pixel, GAL_Color* color)
{

    switch (gc.psd->bpp) {
    case 1:
        if (pixel) {
            color->r = 255;
            color->g = 255;
            color->b = 255;
        }
        else {
            color->r = 0;
            color->g = 0;
            color->b = 0;
        }
        break;

    case 2:
    case 4:
        color->r = SysPixelColor [pixel].r;
        color->g = SysPixelColor [pixel].g;
        color->b = SysPixelColor [pixel].b;
        break;

    case 8:
        color->r = PIXEL332RED (pixel) << 5;
        color->g = PIXEL332GREEN (pixel) << 5;
        color->b = PIXEL332BLUE (pixel) << 6;
        break;

    case 15:
        color->r = PIXEL555RED (pixel) << 3;
        color->g = PIXEL555GREEN (pixel) << 3;
        color->b = PIXEL555BLUE (pixel) << 3;
        break;

    case 16:
        color->r = PIXEL565RED (pixel) << 3;
        color->g = PIXEL565GREEN (pixel) << 2;
        color->b = PIXEL565BLUE (pixel) << 3;
        break;

    case 24:
    case 32:
        color->r = PIXEL888RED (pixel);
        color->g = PIXEL888GREEN (pixel);
        color->b = PIXEL888BLUE (pixel);
        break;
    }
    
    return 0;
}

/*
 * Palette operations
 */
static int getpalette (GAL_GC gc, int s, int len, GAL_Color* cmap)
{
    gc.psd->GetPalette(gc.psd,s,len,cmap);
    return 0;
}

static int setpalette (GAL_GC gc, int s, int len, GAL_Color* cmap)
{
    gc.psd->SetPalette(gc.psd,s,len,cmap);
    return 0;
}

/*
 * Specical functions work for <=8 bit color mode.
 */
static int setcolorfulpalette (GAL_GC gc)
{
    int i;
    GAL_Color pal[256];

    if (gc.psd->bpp > 8)    return 0;

    switch ( gc.psd->bpp ) {
    case 1:
        for (i = 0; i < 2; i++) {
            pal[i].b = SysPixelColor[i*15].b;
            pal[i].g = SysPixelColor[i*15].g;
            pal[i].r = SysPixelColor[i*15].r;
            pal[i].a = 0;
        }
        gc.psd->SetPalette(gc.psd,0,2,pal);
        break;
    case 2:
        for (i = 0; i < 4; i++) {
            pal[i].b = SysPixelColor[i*4].b;
            pal[i].g = SysPixelColor[i*4].g;
            pal[i].r = SysPixelColor[i*4].r;
            pal[i].a = 0;
        }
        gc.psd->SetPalette(gc.psd,0,4,pal);
        break;
    case 4:
        for (i = 0; i < 16; i++) {
            pal[i].b = SysPixelColor[i].b;
            pal[i].g = SysPixelColor[i].g;
            pal[i].r = SysPixelColor[i].r;
            pal[i].a = 0;
        }
        gc.psd->SetPalette(gc.psd,0,16,pal);    
        break;
    case 8:
        for (i = 0; i < 256; i++) {
#if 1
            pal[i].r = PIXEL332RED (i) << 5;
            pal[i].g = PIXEL332GREEN (i) << 5;
            pal[i].b = PIXEL332BLUE (i) << 6;
#else
            pal[i].b = (i & 7) * (64 / 2);        /* 3 bits */
            pal[i].g = ((i & 56) >> 3) * (64 / 2);    /* 3 bits */
            pal[i].r = ((i & 192) >> 6) * (64);        /* 2 bits */
#endif
            pal[i].a = 0;
        }
        gc.psd->SetPalette (gc.psd, 0, 256, pal);    
        break;
    default:
        break;
    }        
    return 0;
}


/*
 * Box operations
 * TODO: Currently we did not do clpping in putbox() or getbox() or other.
 * If we do clipping at early, we can spend time...
 */
static size_t boxsize (GAL_GC gc, int w, int h)
{
    if ((w <= 0) || (h <= 0)) return -1;

    return w * h * bytesperpixel (gc);
}

static int fillbox (GAL_GC gc, int x, int y, int w, int h, gal_pixel pixel)
{
    if ((w <= 0) || (h <= 0)) return -1;

    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

    if (native_gen_clipbox (gc.psd, &x, &y, &w, &h) == CLIP_INVISIBLE)
        goto ret;

    gc.psd->FillRect (gc.psd, x, y, w, h, pixel) ;

    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + w, y + h);

ret:
    UNBLOCK_DRAW_SEM;
    return 0;
}

static void putbox_helper (GAL_GC gc, int x, int y, int w, int h, void* buf, int pitch)
{
    int bpp;
    gal_uint8 *tmpptr;
    
    bpp = bytesperpixel (gc);
    tmpptr= (gal_uint8*) buf;

    rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?1:0);

    if ( y < 0 ) {
        h += y;
        tmpptr -= y * pitch;
        y = 0;
    }
    if ( x < 0 ) {
        w += x;
        tmpptr -= bpp * x;
        x = 0;
    }
    if ( x + w -1 >= gc.psd->yres) 
        w = gc.psd->yres - x;
    if ( y + h -1 >= gc.psd->xres) 
        h = gc.psd->xres - y;

    rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

    if (_ROT_DIR_CW) {
        while (h > 0) {
            gc.psd->PutBox (gc.psd, x, y, 1, w, tmpptr);

            tmpptr += pitch;
            x--;
            h--;
        }
    }
    else {
        char* reversi;
        reversi = malloc (w * bpp);

        y -= w - 1;
        while (h > 0) {
            reverse_buff (reversi, tmpptr, w, bpp);
            gc.psd->PutBox (gc.psd, x, y, 1, w, reversi);

            tmpptr += pitch;
            x++;
            h--;
        }
        free (reversi);
    }
}

static int putbox_wrapper (GAL_GC gc, int x, int y, int w, int h, void* buf, int pitch)
{
    if ((w <= 0) || (h <= 0)) return -1;
    
    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {

        rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

        if (gc.psd->doclip) {
            if ((x + w - 1 < gc.psd->clipminx) || (x >= gc.psd->clipmaxx))
                goto inv_args;
            if ((y + h - 1 < gc.psd->clipminy) || (y >= gc.psd->clipmaxy))
                goto inv_args;
        } else {
            if ((x + w - 1 < 0) || (x >= gc.psd->xres))
                goto inv_args;
            if ((y + h - 1 < 0) || (y >= gc.psd->yres))
                goto inv_args;
        }

        putbox_helper (gc, x, y, w, h, buf, pitch);
    }
    else {

        if (gc.psd->doclip) {
            if ((x + w - 1 < gc.psd->clipminx) || (x >= gc.psd->clipmaxx))
                goto inv_args;
            if ((y + h - 1 < gc.psd->clipminy) || (y >= gc.psd->clipmaxy))
                goto inv_args;
        } else {
            if ((x + w - 1 < 0) || (x >= gc.psd->xres))
                goto inv_args;
            if ((y + h - 1 < 0) || (y >= gc.psd->yres))
                goto inv_args;
        }

        gc.psd->PutBox (gc.psd, x, y, w, h, buf);   /* ignore pitch */
    }

    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + w, y + h);

inv_args:
    UNBLOCK_DRAW_SEM;
    return 0;
}

static int putbox (GAL_GC gc, int x, int y, int w, int h, void* buf)
{
    return putbox_wrapper (gc, x, y, w, h, buf, w * bytesperpixel (gc));
}

static int putboxmask ( GAL_GC gc, int x, int y, int w, int h, void* buf, gal_pixel cxx)
{
    int oldw, bpp;
    gal_uint8 *tmpptr;
    
    oldw = w;bpp = 0;
    tmpptr= (gal_uint8*) buf;

    if ((w <= 0) || (h <= 0)) return -1;

    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
        if (gc.psd->doclip) {
            if ((x + w - 1 < gc.psd->clipminx) || (x >= gc.psd->clipmaxx))
                goto inv_args;
            if ((y + h - 1 < gc.psd->clipminy) || (y >= gc.psd->clipmaxy))
                goto inv_args;
        } else {
            if ((x + w - 1 < 0) || (x >= gc.psd->xres))
                goto inv_args;
            if ((y + h - 1 < 0) || (y >= gc.psd->yres))
                goto inv_args;
        }
        rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?1:0);

        bpp = bytesperpixel (gc);

        if ( y < 0 ) {
            h += y;
            tmpptr -= y * bpp * w;
            y = 0;
        }
        if ( x < 0 ) {
            w += x;
            tmpptr -= bpp * x;
            x = 0;
        }        
        if ( x + w -1 >= gc.psd->yres) 
            w = gc.psd->yres - x ;
        if ( y + h -1 >= gc.psd->xres) 
            h = gc.psd->xres - y ;

        rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

        if (_ROT_DIR_CW) {
            while (h > 0) {
                gc.psd->PutBoxMask (gc.psd, x, y, 1, w, tmpptr, cxx);
    
                tmpptr += bpp * oldw;
                x--;
                h--;
            }
        }
        else {
            char* reversi;
            reversi = malloc (w * bpp);
    
            y -= w - 1;
            while (h > 0) {
                reverse_buff (reversi, tmpptr, w, bpp);
                gc.psd->PutBoxMask (gc.psd, x, y, 1, w, reversi, cxx);
    
                tmpptr += bpp * oldw;
                x++;
                h--;
            }
            free (reversi);
        }
    }
    else {

        if (gc.psd->doclip) {
            if ((x + w - 1 < gc.psd->clipminx) || (x >= gc.psd->clipmaxx))
                goto inv_args;
            if ((y + h - 1 < gc.psd->clipminy) || (y >= gc.psd->clipmaxy))
                goto inv_args;
        } else {
            if ((x + w - 1 < 0) || (x >= gc.psd->xres))
                goto inv_args;
            if ((y + h - 1 < 0) || (y >= gc.psd->yres))
                goto inv_args;
        }
        gc.psd->PutBoxMask (gc.psd, x, y, w, h, buf, cxx);
    }

    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + w, y + h);

inv_args:
    UNBLOCK_DRAW_SEM;
    return 0;
}

static void getbox_helper (GAL_GC gc, int x, int y, int w, int h, void* buf, int pitch)
{
    int bpp;
    gal_uint8 *tmpptr;
    
    bpp = bytesperpixel (gc);
    tmpptr= (gal_uint8*) buf;

    if ( y < 0 ) {
        h += y;
        tmpptr -= y * pitch;
        y = 0;
    }
    if ( x < 0 ) {
        w += x;
        tmpptr -= bpp * x;
        x = 0;
    }        
    if ( x + w -1 >= gc.psd->yres) 
        w = gc.psd->yres - x ;
    if ( y + h -1 >= gc.psd->xres) 
        h = gc.psd->xres - y ;

    rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

    if (_ROT_DIR_CW) {
        while (h > 0) {
            gc.psd->GetBox (gc.psd, x, y, 1, w, tmpptr);

            tmpptr += pitch;
            x--;
            h--;
        }
    }
    else {
        char* reversi;
        reversi = malloc (w * bpp);

        y -= w - 1;
        while (h > 0) {
            gc.psd->GetBox (gc.psd, x, y, 1, w, reversi);
            reverse_buff (tmpptr, reversi, w, bpp);

            tmpptr += pitch;
            x++;
            h--;
        }
        free (reversi);
    }
}

static int getbox_wrapper (GAL_GC gc, int x, int y, int w, int h, void* buf, int pitch)
{
    if ((w <= 0) || (h <= 0)) return -1;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {

        if ((x + w - 1 < 0) || (x >= gc.psd->yres))
            return -1;
        if ((y + h - 1 < 0) || (y >= gc.psd->xres))
            return -1;

        getbox_helper (gc, x, y, w, h, buf, pitch);
    }
    else {

        if ((x + w - 1 < 0) || (x >= gc.psd->xres))
            return -1;
        if ((y + h - 1 < 0) || (y >= gc.psd->yres))
            return -1;

        gc.psd->GetBox (gc.psd, x, y, w, h, buf);   /* ignore pitch */
    }

    return 0;
}

static int getbox (GAL_GC gc, int x, int y, int w, int h, void* buf)
{
    return getbox_wrapper (gc, x, y, w, h, buf, w * bytesperpixel (gc));
}

static int scalebox (GAL_GC gc, int sw, int sh, void* srcbuf,
        int dw, int dh, void* dstbuf)
{
    return native_gen_scalebox (gc.psd, sw, sh, srcbuf, dw, dh, dstbuf);
}

static int copybox (GAL_GC gc, int x, int y, int w, int h, int nx, int ny)
{
    int org_w, org_h;
    org_w = w;
    org_h = h;

    if ((w <= 0) || (h <= 0)) return -1;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        rotaterect (&x, &y, &w, &h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
        rotaterect (&nx, &ny, &org_w, &org_h, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
    }

    if ((x >= gc.psd->xres) || (x + w - 1 < 0)) return -1;
    if ((y >= gc.psd->yres) || (y + h - 1 < 0)) return -1;

    if (x < 0) { nx -= x; w += x; x = 0; }
    if (y < 0) { ny -= y; h += y; y = 0; }
    if (x + w - 1 >= gc.psd->xres) w = gc.psd->xres - x;
    if (y + h - 1 >= gc.psd->yres) h = gc.psd->yres - y;        

    BLOCK_DRAW_SEM;

    //dst do clip
    if (gc.psd->doclip) {
        if ((nx + w - 1 < gc.psd->clipminx) || (nx >= gc.psd->clipmaxx))
            goto inv_args;
        if ((ny + h - 1 < gc.psd->clipminy) || (ny >= gc.psd->clipmaxy))
            goto inv_args;
        if (nx < gc.psd->clipminx) {
            x += gc.psd->clipminx - nx;
            w -= gc.psd->clipminx - nx;
            nx = gc.psd->clipminx;
        } 
        if (nx + w - 1 >= gc.psd->clipmaxx) {
            w = gc.psd->clipmaxx - nx;
        }
        if (ny < gc.psd->clipminy) {
            y += gc.psd->clipminy - ny;
            h -= gc.psd->clipminy - ny;
            ny = gc.psd->clipminy;
        }
        if (ny + h - 1 >= gc.psd->clipmaxy) {
            h = gc.psd->clipmaxy - ny;
        }    
    } else {
        if ((nx + w - 1 < 0) || (nx >= gc.psd->xres))
            goto inv_args;
        if ((ny + h - 1 < 0) || (ny >= gc.psd->yres))
            goto inv_args;
        if (nx < 0) {
            x -= nx;
            w += nx;
            nx = 0;
        } 
        if (nx + w - 1 >= gc.psd->xres) {
            w = gc.psd->xres- nx;
        }
        if (ny < 0) {
            y -= ny;
            h += ny;
            ny = 0;
        }
        if (ny + h - 1 >= gc.psd->yres) {
            h = gc.psd->yres- ny;
        }    
    }
    
    if ((w <= 0) || (h <= 0))
        goto inv_args;

    if ((x < 0) || (x + w - 1 >= gc.psd->xres))
        goto inv_args;
    if ((y < 0) || (y + h - 1 >= gc.psd->yres))
        goto inv_args;

    if (gc.psd->doclip) {
        if ((nx < gc.psd->clipminx) || (nx + w - 1 >= gc.psd->clipmaxx)) 
            goto inv_args;
        if ((ny < gc.psd->clipminy) || (ny + h - 1 >= gc.psd->clipmaxy)) 
            goto inv_args;
    } else {
        if ((nx < 0) || (nx + w - 1 >= gc.psd->xres)) 
            goto inv_args;
        if ((ny < 0) || (ny + h - 1 >= gc.psd->yres)) 
            goto inv_args;
    }

    gc.psd->CopyBox (gc.psd, x, y, w, h, nx, ny);

    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, nx, ny, nx + w, ny + h);

inv_args:
    UNBLOCK_DRAW_SEM;
    return 0;
}

/* 
 * Must set destination graphics context
 */
static int crossblit (GAL_GC src, int sx, int sy, int w, int h,
        GAL_GC dst, int dx, int dy)
{
#if defined(_LITE_VERSION) && !(_STAND_ALONE)
    GAL_GC gc;
    gc = dst;
#endif

    int Bpp;
    Bpp = bytesperpixel (src);

    if ((w <= 0) || (h <= 0)) return -1;

    if (src.psd == dst.psd) {
        return copybox (src, sx, sy, w, h, dx, dy);
    }
    
    //src clip to screen
    if (_COOR_TRANS && src.psd == __mg_cur_gfx->phygc.psd) {

        /* dst is clipped and is a memory gc */
        if ((dx + w - 1 < dst.psd->clipminx) || (dx >= dst.psd->clipmaxx))
            return 0;
        if ((dy + h - 1 < dst.psd->clipminy) || (dy >= dst.psd->clipmaxy))
            return 0;

        if (dx < dst.psd->clipminx) {
            sx += dst.psd->clipminx - dx;
            w -= dst.psd->clipminx - dx;
            dx = dst.psd->clipminx;
        } 
        if (dx + w - 1 >= dst.psd->clipmaxx) {
            w = dst.psd->clipmaxx - dx;
        }
        if (dy < dst.psd->clipminy) {
            sy += dst.psd->clipminy - dy;
            h -= dst.psd->clipminy - dy;
            dy = dst.psd->clipminy;
        }
        if (dy + h - 1 >= dst.psd->clipmaxy) {
            h = dst.psd->clipmaxy - dy;
        }    

        if ((w <= 0) || (h <= 0))
            return 0;

        if ((dx < dst.psd->clipminx) || (dx + w - 1 >= dst.psd->clipmaxx)) 
            return 0;
        if ((dy < dst.psd->clipminy) || (dy + h - 1 >= dst.psd->clipmaxy))
            return 0;

        return getbox_wrapper (src, sx, sy, w, h, 
                        (BYTE *)dst.psd->addr + dst.psd->linelen * dy + Bpp * dx, 
                        dst.psd->xres * bytesperpixel (dst));
    }

    if (_COOR_TRANS && dst.psd == __mg_cur_gfx->phygc.psd) {

        /* src is a memory gc */
        if ((sx + w <= 0) || (sx >= src.psd->xres))
            return 0;
        if ((sy + h <= 0) || (sy >= src.psd->yres))
            return 0;

        if (sx < 0) {
            sx = 0;
            dx -= sx;
            w += sx;
        } 
        if (sx + w - 1 >= src.psd->xres) {
            w = src.psd->xres - sx;
        }
        if (sy < 0) {
            sy = 0;
            dy -= sy;
            h +=sy;
        }
        if (sy + h - 1 >= src.psd->yres) {
            h = src.psd->yres - sy;
        }    

        if ((w <= 0) || (h <= 0))
            return 0;

        return putbox_wrapper (dst, dx, dy, w, h, 
                            (BYTE *)src.psd->addr + src.psd->linelen * sy + Bpp * sx, 
                            src.psd->xres * bytesperpixel (src));
    }
    
    if ((sx >= src.psd->xres) || (sx + w - 1 < 0)) return -1;
    if ((sy >= src.psd->yres) || (sy + h - 1 < 0)) return -1;
    if (sx < 0) { dx -= sx; w += sx; sx = 0; }
    if (sy < 0) { dy -= sy; h += sy; sy = 0; }
    
    if (sx + w - 1 >= src.psd->xres) w = src.psd->xres - sx;
    if (sy + h - 1 >= src.psd->yres) h = src.psd->yres - sy;        

    BLOCK_DRAW_SEM;

    //dst do clip
    if (dst.psd->doclip) {
        if ((dx + w - 1 < dst.psd->clipminx) || (dx >= dst.psd->clipmaxx))
            goto inv_args;
        if ((dy + h - 1 < dst.psd->clipminy) || (dy >= dst.psd->clipmaxy))
            goto inv_args;
        if (dx < dst.psd->clipminx) {
            sx += dst.psd->clipminx - dx;
            w -= dst.psd->clipminx - dx;
            dx = dst.psd->clipminx;
        } 
        if (dx + w - 1 >= dst.psd->clipmaxx) {
            w = dst.psd->clipmaxx - dx;
        }
        if (dy < dst.psd->clipminy) {
            sy += dst.psd->clipminy - dy;
            h -= dst.psd->clipminy - dy;
            dy = dst.psd->clipminy;
        }
        if (dy + h - 1 >= dst.psd->clipmaxy) {
            h = dst.psd->clipmaxy - dy;
        }    
    } else {
        if ((dx + w - 1 < 0) || (dx >= dst.psd->xres))
            goto inv_args;
        if ((dy + h - 1 < 0) || (dy >= dst.psd->yres))
            goto inv_args;
        if (dx < 0) {
            sx -= dx;
            w += dx;
            dx = 0;
        } 
        if (dx + w - 1 >= dst.psd->xres) {
            w = dst.psd->xres- dx;
        }
        if (dy < 0) {
            sy -= dy;
            h += dy;
            dy = 0;
        }
        if (dy + h - 1 >= dst.psd->yres) {
            h = dst.psd->yres- dy;
        }    
    }
    
    if ((w <= 0) || (h <= 0))
        goto inv_args;

    if ((sx < 0) || (sx + w - 1 >= src.psd->xres))
        goto inv_args;
    if ((sy < 0) || (sy + h - 1 >= src.psd->yres))
        goto inv_args;

    if (dst.psd->doclip) {
        if ((dx < dst.psd->clipminx) || (dx + w - 1 >= dst.psd->clipmaxx)) 
            goto inv_args;
        if ((dy < dst.psd->clipminy) || (dy + h - 1 >= dst.psd->clipmaxy))
            goto inv_args;
    } else {
        if ((dx < 0) || (dx + w - 1 >= dst.psd->xres)) 
            goto inv_args;
        if ((dy < 0) || (dy + h - 1 >= dst.psd->yres)) 
            goto inv_args;
    }

    src.psd->Blit (dst.psd, dx, dy, w, h, src.psd, sx, sy);

    if (dst.psd->UpdateRect) dst.psd->UpdateRect (dst.psd, dx, dy, dx + w, dy + h);

inv_args:
    UNBLOCK_DRAW_SEM;
    return 0;
}


static int drawhline (GAL_GC gc, int x, int y, int w, gal_pixel pixel)
{
    if (w <= 0 ) return -1;

    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
        if (!_ROT_DIR_CW) y -= w - 1;

        if (native_gen_clipvline (gc.psd, &x, &y, &w) == CLIP_INVISIBLE )
            goto ret;

        gc.psd->DrawVLine (gc.psd, x, y, w, pixel);
        if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + 1, y + w);
    }
    else {
        if (native_gen_cliphline (gc.psd, &x, &y, &w) == CLIP_INVISIBLE )
            goto ret;

        gc.psd->DrawHLine (gc.psd, x, y, w, pixel);
        if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + w, y + 1);
    }

ret:
    UNBLOCK_DRAW_SEM;
    return 0;
}

static int drawvline (GAL_GC gc, int x, int y, int h, gal_pixel pixel)
{
    if (h <= 0 ) return -1;
    
    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
        if (_ROT_DIR_CW) x -= h - 1;

        if (native_gen_clipvline (gc.psd, &x, &y, &h) == CLIP_INVISIBLE )
            goto ret;

        gc.psd->DrawVLine (gc.psd, x, y, h, pixel);
        if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + 1, y + h);
    }
    else {
        if (native_gen_cliphline (gc.psd, &x, &y, &h) == CLIP_INVISIBLE )
            goto ret;

        gc.psd->DrawHLine (gc.psd, x, y, h, pixel);
        if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + h, y + 1);
    }

ret:
    UNBLOCK_DRAW_SEM;
    return 0;
}

/*
 *  Pixel operations
 */
static int drawpixel (GAL_GC gc, int x, int y, gal_pixel pixel)
{
    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);

    if (native_gen_clippoint (gc.psd, x, y)) {
        gc.psd->DrawPixel (gc.psd, x, y, pixel);
        if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x, y, x + 1, y + 1);
    }

    UNBLOCK_DRAW_SEM;
    return 0;
}


static int getpixel (GAL_GC gc, int x, int y, gal_pixel* pixel)
{
    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        if ((x >= 0) && (x < gc.psd->yres) && (y >= 0) && (y < gc.psd->xres)) {
            rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
            *pixel = gc.psd->ReadPixel (gc.psd, x, y);
        } else 
            return -1;
    }
    else {
        if ((x >= 0) && (x < gc.psd->xres) && (y >= 0) && (y < gc.psd->yres))
            *pixel = gc.psd->ReadPixel (gc.psd, x, y);
        else 
            return -1;
    }

    return 0;
}

static int line (GAL_GC gc, int x1, int y1, int x2, int y2, gal_pixel pixel)
{
    gal_pixel oldcolor;

    BLOCK_DRAW_SEM;

    getfgcolor(gc,&oldcolor);
    setfgcolor(gc,pixel);

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd) {
        rotatepoint (&x1, &y1, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
        rotatepoint (&x2, &y2, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
    }

    native_gen_line (gc.psd, x1, y1, x2, y2, TRUE);
    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x1, y1, x2, y2);

    setfgcolor(gc,oldcolor);

    UNBLOCK_DRAW_SEM;
    return 0;
}

/* 
 * NOTE: must be normalized rect.
 */
static int rectangle (GAL_GC gc, int l, int t, int r, int b, gal_pixel pixel)
{
    gal_pixel oldcolor;

    BLOCK_DRAW_SEM;

    getfgcolor(gc,&oldcolor);
    setfgcolor(gc,pixel);

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotatecoor (&l, &t, &r, &b, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
    
    native_gen_rect (gc.psd, l, t, r, b);
    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, l, t, r, b);

    setfgcolor (gc, oldcolor);

    UNBLOCK_DRAW_SEM;

    return 0;
}

static int circle (GAL_GC gc, int x, int y, int r, gal_pixel pixel)
{
    BLOCK_DRAW_SEM;

    if (_COOR_TRANS && gc.psd == __mg_cur_gfx->phygc.psd)
        rotatepoint (&x, &y, _ROT_DIR_CW?gc.psd->xres:gc.psd->yres, _ROT_DIR_CW?0:1);
    native_gen_circle (gc.psd, x, y, r, pixel);
    if (gc.psd->UpdateRect) gc.psd->UpdateRect (gc.psd, x - r, y - r, x + r, y + r);

    UNBLOCK_DRAW_SEM;
    return 0;
}

static void panic (int exitcode)
{
    uHALr_printf( "MiniGUI Panic. Exit Code: %d.\n", exitcode);

    __mg_cur_gfx->phygc.psd->Close (__mg_cur_gfx->phygc.psd);
}

/******************  Initialization and termination of Native IAL engine **************/
BOOL InitNative (GFX* gfx)
{
    int i;
    PSD psd;
    
    psd = NULL; 
    uHALr_printf("GAL: InitNative\n");
    
    psd = scrdev.Open (&scrdev);
    
    uHALr_printf("GAL: Dev open\n");

  
    if (!psd) return FALSE;

    gfx->phygc.psd = psd; 
    gfx->bytes_per_phypixel = (psd->bpp + 7 ) / 8;
    gfx->bits_per_phypixel  =  psd->bpp;

    if (_COOR_TRANS) {
        gfx->width_phygc        = psd->yres; 
        gfx->height_phygc       = psd->xres;
    }
    else {
        gfx->width_phygc        = psd->xres; 
        gfx->height_phygc       = psd->yres;
    }

    gfx->colors_phygc       = psd->ncolors;
    gfx->grayscale_screen = FALSE;
    
    gfx->bytesperpixel      = bytesperpixel;
    gfx->bitsperpixel       = bitsperpixel;
    gfx->width              = width;
    gfx->height             = height;
    gfx->colors             = colors;

    //now functions
    gfx->allocategc         = allocategc;
    gfx->freegc             = freegc;
    gfx->enableclipping     = enableclipping;
    gfx->disableclipping    = disableclipping;
    gfx->setclipping        = setclipping;
    gfx->getclipping        = getclipping;
    gfx->getbgcolor         = getbgcolor;
    gfx->setbgcolor         = setbgcolor;
    gfx->getfgcolor         = getfgcolor;
    gfx->setfgcolor         = setfgcolor;
    gfx->mapcolor           = mapcolor;
    gfx->unmappixel         = unmappixel;
    gfx->getpalette         = getpalette;
    gfx->setpalette         = setpalette;
    gfx->setcolorfulpalette = setcolorfulpalette;

    gfx->boxsize            = boxsize;
    gfx->fillbox            = fillbox;
    gfx->putbox             = putbox;
    gfx->putboxmask         = putboxmask;
    gfx->getbox             = getbox;
    gfx->scalebox           = scalebox;
    gfx->copybox            = copybox;
    gfx->crossblit          = crossblit;
    gfx->drawhline          = drawhline;
    gfx->drawvline          = drawvline;

    gfx->drawpixel          = drawpixel;
    gfx->getpixel           = getpixel;

    gfx->line               = line;
    gfx->rectangle          = rectangle;

    gfx->panic              = panic;
    gfx->circle             = circle;

    gfx->setgc              = NULL;
    
    setcolorfulpalette(gfx->phygc);
    
    for (i = 0; i < 17; i++)
        SysPixelIndex [i] = mapcolor (gfx->phygc, (GAL_Color*)(SysPixelColor + i));

    if (_COOR_TRANS)
        setclipping (gfx->phygc, 0, 0, psd->yres - 1, psd->xres - 1);
    else
        setclipping (gfx->phygc, 0, 0, psd->xres - 1, psd->yres - 1);
    
    return TRUE;
}

void TermNative (GFX* gfx)
{
    gfx->phygc.psd->Close(gfx->phygc.psd);
}

