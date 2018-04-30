/*
** $Id: native.h,v 1.20 2004/08/02 03:00:03 snig Exp $
**
** native.h: the head file of native Low Level Graphics Engine.
**
** Copyright (C) 2003 Feynman Software
** Copyright (C) 2001 ~ 2002 Wei Yongming
**
*/

#ifndef GUI_GAL_NATIVE_H
    #define GUI_GAL_NATIVE_H

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "gal.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/*
 * Interface to Screen Device Driver
 * This structure is also allocated for memory (offscreen) drawing and blitting.
 */
/* This structure is used to pass parameters into the low
 * level device driver functions.
 */

typedef struct _screendevice {
    int  doclip;
    int  clipminx;        /* minimum x value of cache rectangle */
    int  clipminy;        /* minimum y value of cache rectangle */
    int  clipmaxx;        /* maximum x value of cache rectangle */
    int  clipmaxy;        /* maximum y value of cache rectangle */

    int  xres;        /* X screen res (real) */
    int  yres;        /* Y screen res (real) */
    int  xvirtres;    /* X drawing res (will be flipped in portrait mode) */
    int  yvirtres;    /* Y drawing res (will be flipped in portrait mode) */
    int  planes;        /* # planes*/
    int  bpp;        /* # bits per pixel*/
    int  linelen;    /* line length in bytes for bpp 1,2,4,8*/
                /* line length in pixels for bpp 16, 24, 32*/
    int  size;        /* size of memory allocated*/

    gal_pixel gr_foreground;      /* current foreground color */
    gal_pixel gr_background;      /* current background color */
    int gr_mode;

    long ncolors;    /* # screen colors */
    int  pixtype;    /* format of pixel value */
    int  flags;        /* device flags */
    void *addr;        /* address of memory allocated (memdc or fb) */
    void *dev_spec;    /* device specific field. */

    PSD  (*Open)(PSD psd);
    void (*Close)(PSD psd);
    void (*SetPalette)(PSD psd,int first,int count,GAL_Color *cmap);
    void (*GetPalette)(PSD psd,int first,int count,GAL_Color *cmap);
    PSD  (*AllocateMemGC)(PSD psd);
    BOOL (*MapMemGC)(PSD mempsd,int w,int h,int planes,int bpp, int linelen,int size,void *addr);
    void (*FreeMemGC)(PSD mempsd);
    BOOL (*ClipPoint)(PSD psd,int x,int y);
    void (*FillRect)(PSD psd,int x,int y,int w,int h,gal_pixel c);
    
//==================================================================================//
    void (*DrawPixel)(PSD psd, int x, int y, gal_pixel c);
    gal_pixel (*ReadPixel)(PSD psd, int x, int y);
    void (*DrawHLine)(PSD psd, int x, int y, int w, gal_pixel c);
    void (*DrawVLine)(PSD psd, int x, int y, int w, gal_pixel c);
    void (*Blit)(PSD dstpsd, int dstx, int dsty, int w, int h, PSD srcpsd, int srcx, int srcy);
    void (*PutBox)( PSD psd, int x, int y, int w, int h, void* buf );
    void (*GetBox)( PSD psd, int x, int y, int w, int h, void* buf );
    void (*PutBoxMask)( PSD psd, int x, int y, int w, int h, void *buf, gal_pixel cxx);
    void (*CopyBox)(PSD psd,int x1, int y1, int w, int h, int x2, int y2);
    void (*UpdateRect) (PSD psd, int l, int t, int r, int b);
//===================================================================================//
} SCREENDEVICE;

#ifdef _NATIVE_GAL_FBCON
extern SCREENDEVICE    scrdev;    
#endif

#ifdef _NATIVE_GAL_QVFB
extern SCREENDEVICE    qvfbdev;    
#endif

#ifdef _NATIVE_GAL_COMMLCD
extern SCREENDEVICE    commlcd;    
#endif


BOOL    InitNative(GFX* gfx);
void    TermNative(GFX* gfx);

/* genfunc.c*/
void native_gen_initmemgc(PSD mempsd,int w,int h,int planes,int bpp,int linelen, int size,void *addr);
PSD native_gen_allocatememgc(PSD psd);
void native_gen_freememgc(PSD mempsd);
void native_gen_fillrect(PSD psd,int x, int y, int w, int h, gal_pixel c);
int native_gen_clippoint(PSD psd, int x ,int y);
int native_gen_cliphline(PSD psd,int * px,int * py, int * pw);
int native_gen_clipvline(PSD psd,int * px,int * py, int *ph);
int native_gen_clipline (PSD psd,int * px1,int * py1, int * px2,int *py2);
int cs_clipline(PSD psd,int *_x0, int *_y0, int *_x1, int *_y1, int *clip_first, int *clip_last);
int native_gen_clipbox(PSD psd,int * px,int * py, int * pw,int *ph);
int native_gen_calcmemgcalloc(PSD psd, unsigned int width, unsigned int height, int planes, int bpp, int *psize, int *plinelen);
void native_gen_line(PSD psd, int x1, int y1, int x2, int y2, BOOL bDrawLastPoint);
void native_gen_rect(PSD psd , int l, int t, int r, int b);
void generate_palette(GAL_Color *pal, int num);
void native_gen_circle(PSD psd, int sx, int sy, int r, int c);
int native_gen_scalebox (PSD psd, int w1, int h1, void *_dp1, int w2, int h2, void *_dp2);
/* end of genfunc.c*/

/* Drawing modes*/
#define MODE_SET    0    /* draw pixels as given (default) */
#define MODE_XOR    1    /* draw pixels using XOR */
#define MODE_OR     2    /* draw pixels using OR (notimp)*/
#define MODE_AND    3    /* draw pixels using AND (notimp)*/
#define MODE_MAX    3

/* Operations for the Blitter/Area functions */
#define PSDOP_COPY    0
#define PSDOP_COPYALL    1
#define PSDOP_COPYTRANS 2
#define PSDOP_ALPHAMAP    3
#define PSDOP_ALPHACOL    4
#define PSDOP_PIXMAP_COPYALL    5

/* 
 * Pixel formats
 * Note the two pseudo pixel formats are never returned by display drivers,
 * but rather used as a data structure type in GrArea.  The other
 * types are both returned by display drivers and used as pixel packing
 * specifiers.
 */
#define PF_RGB              0    /* pseudo, convert from packed 32 bit RGB */
#define PF_gal_pixel        1    /* pseudo, no convert from packed gal_pixel */
#define PF_PALETTE          2    /* pixel is packed 8 bits 1, 4 or 8 pal index */
#define PF_TRUECOLOR0888    3    /* pixel is packed 32 bits 8/8/8 truecolor */
#define PF_TRUECOLOR888     4    /* pixel is packed 24 bits 8/8/8 truecolor */
#define PF_TRUECOLOR565     5    /* pixel is packed 16 bits 5/6/5 truecolor */
#define PF_TRUECOLOR332     6    /* pixel is packed 8 bits 3/3/2 truecolor */

/* Truecolor color conversion and extraction macros */
/*
 * Conversion from RGB to gal_pixel
 */
/* create 24 bit 8/8/8 format pixel (0x00RRGGBB) from RGB triplet*/
#define RGB2PIXEL888(r,g,b)    \
    (((r) << 16) | ((g) << 8) | (b))

/* create 16 bit 5/6/5 format pixel from RGB triplet */
#define RGB2PIXEL565(r,g,b)    \
    ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))

/* create 15 bit 5/5/5 format pixel from RGB triplet */
#define RGB2PIXEL555(r,g,b)    \
    ((((r) & 0xf8) << 7) | (((g) & 0xf8) << 2) | (((b) & 0xf8) >> 3))

/* create 8 bit 3/3/2 format pixel from RGB triplet*/
#define RGB2PIXEL332(r,g,b)    \
    (((r) & 0xe0) | (((g) & 0xe0) >> 3) | (((b) & 0xc0) >> 6))

/*
 * Conversion from COLORVAL to gal_pixel
 */
/* create 24 bit 8/8/8 format pixel from RGB colorval (0x00BBGGRR)*/
#define COLOR2PIXEL888(c)    \
    ((((c) & 0xff) << 16) | ((c) & 0xff00) | (((c) & 0xff0000) >> 16))

/* create 16 bit 5/6/5 format pixel from RGB colorval (0x00BBGGRR)*/
#define COLOR2PIXEL565(c)    \
    ((((c) & 0xf8) << 8) | (((c) & 0xfc00) >> 5) | (((c) & 0xf80000) >> 19))

/* create 15 bit 5/5/5 format pixel from RGB colorval (0x00BBGGRR)*/
#define COLOR2PIXEL555(c)    \
    ((((c) & 0xf8) << 7) | (((c) & 0xf800) >> 6) | (((c) & 0xf80000) >> 19))

/* create 8 bit 3/3/2 format pixel from RGB colorval (0x00BBGGRR)*/
#define COLOR2PIXEL332(c)    \
    (((c) & 0xe0) | (((c) & 0xe000) >> 11) | (((c) & 0xc00000) >> 22))

/*
 * Conversion from gal_pixel to red, green or blue components
 */
/* return 8/8/8 bit r, g or b component of 24 bit gal_pixel*/
#define PIXEL888RED(gal_pixel)        (((gal_pixel) >> 16) & 0xff)
#define PIXEL888GREEN(gal_pixel)      (((gal_pixel) >> 8) & 0xff)
#define PIXEL888BLUE(gal_pixel)       ((gal_pixel) & 0xff)

/* return 5/6/5 bit r, g or b component of 16 bit gal_pixel*/
#define PIXEL565RED(gal_pixel)        (((gal_pixel) >> 11) & 0x1f)
#define PIXEL565GREEN(gal_pixel)      (((gal_pixel) >> 5) & 0x3f)
#define PIXEL565BLUE(gal_pixel)       ((gal_pixel) & 0x1f)

/* return 5/5/5 bit r, g or b component of 16 bit gal_pixel*/
#define PIXEL555RED(gal_pixel)        (((gal_pixel) >> 10) & 0x1f)
#define PIXEL555GREEN(gal_pixel)      (((gal_pixel) >> 5) & 0x1f)
#define PIXEL555BLUE(gal_pixel)       ((gal_pixel) & 0x1f)

/* return 3/3/2 bit r, g or b component of 8 bit gal_pixel*/
#define PIXEL332RED(gal_pixel)        (((gal_pixel) >> 5) & 0x07)
#define PIXEL332GREEN(gal_pixel)      (((gal_pixel) >> 2) & 0x07)
#define PIXEL332BLUE(gal_pixel)       ((gal_pixel) & 0x03)

/* PSD flags */
#define PSF_SCREEN          0x0001    /* screen device */
#define PSF_MEMORY          0x0002    /* memory device */
#define PSF_ADDRMALLOC      0x0004    /* psd->addr was malloc'd */
#define PSF_ADDRSHAREDMEM   0x0008    /* psd->addr is shared memory */

#define PSF_MSBRIGHT        0x0010    /* Most significant bit is right */

#define CLIP_VISIBLE        1
#define CLIP_INVISIBLE      0
#define CLIP_PARTIAL        -1

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_GAL_NATIVE_H */


