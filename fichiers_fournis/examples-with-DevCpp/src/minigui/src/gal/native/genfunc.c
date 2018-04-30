/*
** $Id: genfunc.c,v 1.13 2004/12/30 07:39:11 snig Exp $
**
** genfunc.c: Native Low Level Graphics Engine 's commone file
**
** Copyright (C) 2003 Feynman Software
** Copyright (C) 2000 Song Lixin
**
** Create by Song Lixin, 2000/10/18
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
#include "gal.h"
#include "native.h"


//==================================================================================================
//general functions
//

static void drawrow(PSD psd, int x1, int x2, int y);
static void drawcol(PSD psd, int x,int y1,int y2);
static inline void setpixel(PSD psd, int x, int y, int c);
static void setcirclepixels(PSD psd, int x, int y, int sx, int sy, int c);
static void setcirclepixels(PSD psd, int x, int y, int sx, int sy, int c);
static inline int muldiv64(int m1, int m2, int d);

/* initialize memory device with passed parms*/
void native_gen_initmemgc
(PSD mempsd,int w,int h,int planes,int bpp,int linelen, int size,void *addr) {

	mempsd->xres = w;
	mempsd->yres = h;
	mempsd->xvirtres = w;
	mempsd->yvirtres = h;
	mempsd->planes = planes;
	mempsd->bpp = bpp;
	mempsd->linelen = linelen;
	mempsd->size = size;
	mempsd->addr = addr;
}

/* allocate a memory screen device*/
PSD native_gen_allocatememgc(PSD psd)
{
	PSD	mempsd;

	mempsd = malloc(sizeof(SCREENDEVICE));
	if (!mempsd)
		return NULL;

	/* copy passed device get initial values*/
	*mempsd = *psd;

	/* initialize*/
	mempsd->flags |= PSF_MEMORY;
	mempsd->flags &= ~PSF_SCREEN;
	mempsd->addr = NULL;

	return mempsd;
}

void native_gen_freememgc(PSD mempsd)
{

	/* note: mempsd->addr must be freed elsewhere*/

	free(mempsd);
}

void native_gen_fillrect(PSD psd,int x, int y, int w, int h, gal_pixel c)
{
#if 1
	while (h--)
		psd->DrawHLine(psd, x, y++, w, c);
#else
	while (w--)
		psd->DrawVLine(psd, x++, y , h, c);
#endif
}


int native_gen_clippoint(PSD psd, int x ,int y)
{
	if(psd->doclip) {
		if ((x >= psd->clipminx) && (x < psd->clipmaxx) &&
			(y >= psd->clipminy) && (y < psd->clipmaxy)) 
			return CLIP_VISIBLE;
	} else {
		if ((x >= 0) && (x < psd->xres) && (y >= 0) && (y < psd->yres)) 
			return CLIP_VISIBLE;
	}

	return CLIP_INVISIBLE;
}


int native_gen_cliphline(PSD psd,int * px,int * py, int * pw)
{
	if (psd->doclip) {
		if ( (*px >= psd->clipmaxx) || (*py >= psd->clipmaxy) || 
		     (*px + *pw - 1 < psd->clipminx) || (*py < psd->clipminy) )
				return CLIP_INVISIBLE;

		if ( (*px >= psd->clipminx) && (*py >= psd->clipminy) && 
		     (*px + *pw -1 < psd->clipmaxx) && (*py < psd->clipmaxy) )
				return CLIP_VISIBLE;
			
		if (*px < psd->clipminx) {
			*pw -= psd->clipminx - *px;
			*px = psd->clipminx;
		}
		if (*px + *pw - 1 >= psd->clipmaxx)	
			*pw = psd->clipmaxx - *px;
	} else {
		if ( (*px >= psd->xres) || (*py >= psd->yres) || (*px + *pw - 1 < 0) || (*py < 0) )
				return CLIP_INVISIBLE;

		if ( (*px >= 0) && (*py >= 0) && (*px + *pw -1 < psd->xres) && (*py < psd->yres) )
				return CLIP_VISIBLE;
			
		if (*px < 0) {
			*pw += *px;	
			*px = 0;
		}
		if (*px + *pw - 1 >= psd->xres)	
			*pw = psd->xres - *px;
	}

    if (*pw <= 0)
        return CLIP_INVISIBLE;

	return CLIP_PARTIAL;		
}

int native_gen_clipvline(PSD psd,int * px,int * py, int *ph)
{
	if (psd->doclip) {
		if ( (*px >= psd->clipmaxx) || (*py >= psd->clipmaxy) || 
		     (*px < psd->clipminx) || (*py + *ph - 1 < psd->clipminy) )
				return CLIP_INVISIBLE;

		if ( (*px >= psd->clipminx) && (*py >= psd->clipminy) && 
		     (*px < psd->clipmaxx) && (*py + *ph - 1 < psd->clipmaxy) )
				return CLIP_VISIBLE;
			
		if (*py < psd->clipminy) {
			*ph -= psd->clipminy - *py;
			*py = psd->clipminy;
		}
		if (*py + *ph - 1 >= psd->clipmaxy)	
			*ph = psd->clipmaxy - *py;
	} else {
		if ( (*py >= psd->yres) || (*px >= psd->xres) || (*py + *ph - 1 < 0) || (*px < 0) )
				return CLIP_INVISIBLE;

		if ( (*py >= 0) && (*px >= 0) && (*py + *ph -1 < psd->yres) && (*px < psd->xres) )
				return CLIP_VISIBLE;
			
		if (*py < 0) {
			*ph += *py;	
			*py = 0;
		}
		if (*py + *ph - 1 >= psd->yres)	
			*ph = psd->yres - *py;
	}

    if (*ph <= 0)
        return CLIP_INVISIBLE;

	return CLIP_PARTIAL;		
}

int native_gen_clipline (PSD psd,int * px1,int * py1, int * px2,int *py2)
{
	int w,h;
	int r1;
	int clip_first;
	int clip_last;

	if(*px1 == *px2) {
		if (*py1 == *py2)
			return native_gen_clippoint (psd,*px1,*px2);
		if (*py1 > *py2) {
			h = *py1 - *py2 + 1;
			r1 = native_gen_clipvline (psd, px1, py2, &h);
			if (r1 == CLIP_PARTIAL)	
				*py1 = *py2 + h - 1;	
			return r1;
		} else {
			h = *py2 - *py1 + 1;
			r1 = native_gen_clipvline (psd, px1, py1, &h);
			if (r1 == CLIP_PARTIAL)	
				*py2 = *py1 + h - 1;	
			return r1;
		}	
	}
	
	if(*py1 == *py2) {
		if (*px1 > *px2) {
			w = *px1 - *px2 + 1;
			r1 = native_gen_clipvline (psd, px2, py1, &w);
			if (r1 == CLIP_PARTIAL)	
				*px1 = *px2 + w - 1;	
			return r1;
		} else {
			w = *px2 - *px1 + 1;
			r1 = native_gen_clipvline (psd, px2, py2, &w);
			if (r1 == CLIP_PARTIAL)	
				*px2 = *px1 + w - 1;	
			return r1;
		}	
	}
	
	return cs_clipline(psd,px1,py1,px2,py2,&clip_first,&clip_last);	
}

/*
 *	You should give a normalized rect. that is 
					*pw > 0
					*ph > 0 
 * 
 * 	returned value:
 *	CLIP_VISIBLE		The whole rectangle is visible
 *	CLIP_INVISIBLE		The whole rectangle is invisible
 *	CLIP_PARTIAL		The rectangle may be partially visible
 * 	
 *      if returned CLIP_PARTIAL,the parameter is modified to hold the part
 *	that is in the clip region.
 */
int native_gen_clipbox (PSD psd,int * px,int * py, int * pw,int *ph)
{
	if (*pw == 1) 
		return native_gen_clipvline(psd, px, py, ph);	
	
	if (*ph == 1)
		return native_gen_cliphline(psd, px, py, pw);	

	if (psd->doclip) {
		if ( (*px >= psd->clipmaxx) || (*py >= psd->clipmaxy) || 
		     (*px + *pw - 1 < psd->clipminx) || (*py + *ph - 1 < psd->clipminy) )
				return CLIP_INVISIBLE;

		if ( (*px >= psd->clipminx) && (*py >= psd->clipminy) && 
		     (*px + *pw -1 < psd->clipmaxx) && (*py + *ph - 1 < psd->clipmaxy) )
				return CLIP_VISIBLE;
			
		if (*px < psd->clipminx) {
			*pw -= psd->clipminx - *px;
			*px = psd->clipminx;
		}
		if (*py < psd->clipminy) {
			*ph -= psd->clipminy - *py;
			*py = psd->clipminy;
		}
		if (*px + *pw - 1 >= psd->clipmaxx)	
			*pw = psd->clipmaxx - *px;
		if (*py + *ph - 1 >= psd->clipmaxy)	
			*ph = psd->clipmaxy - *py;
	} else {
		if ( (*px >= psd->xres) || (*py >= psd->yres) || 
	             (*px + *pw - 1 < 0) || (*py +*ph - 1 < 0) )
				return CLIP_INVISIBLE;

		if ( (*px >= 0) && (*py >= 0) && (*px + *pw -1 < psd->xres) 
			&& (*py + *ph - 1 < psd->yres) )
				return CLIP_VISIBLE;
			
		if (*px < 0) {
			*pw += *px;	
			*px = 0;
		}
		if (*px + *pw - 1 >= psd->xres)	
			*pw = psd->xres - *px;

		if (*py < 0) {
			*ph += *py;	
			*py = 0;
		}
		if (*py + *ph - 1 >= psd->yres)	
			*ph = psd->yres - *py;
		
	}

    if (*pw <= 0 || *ph <= 0)
        return CLIP_INVISIBLE;

	return CLIP_PARTIAL;		
}
//==================================================================================================
// help functions

/*
 * Calculate size and linelen of memory gc.
 * If bpp or planes is 0, use passed psd's bpp/planes.
 * Note: linelen is calculated to be DWORD aligned for speed
 * for bpp <= 8.  Linelen is converted to bytelen for bpp > 8.
 */
int native_gen_calcmemgcalloc(PSD psd, unsigned int width, unsigned int height, int planes,
	int bpp, int *psize, int *plinelen)
{
	int	bytelen, linelen;

	if(!planes)
		planes = psd->planes;

	if (!bpp)
		bpp = psd->bpp;

	/*
	 * use bpp and planes to create size and linelen.
	 * linelen is in bytes for bpp 1, 2, 4, 8, and pixels for bpp 16,24,32.
	 */
	if(planes == 1) {
		switch(bpp) {
		case 1:
		case 2:
		case 4:
		case 8:
			bytelen = linelen = (width+3) & ~3;
		break;
		case 16:
			linelen = width;
			bytelen = width * 2;
		break;
		case 24:
			linelen = width;
			bytelen = width * 3;
		break;
		case 32:
			linelen = width;
			bytelen = width * 4;
		break;
		default:
			return 0;
		}
	} else if(planes == 4) {
		/* FIXME assumes VGA 4 planes 4bpp*/
		/* we use 4bpp linear for memdc format*/
		bytelen = linelen = (width+3) & ~3;
	} else {
		*psize = *plinelen = 0;
		return 0;
	}

	*plinelen = linelen;
	*psize = bytelen * height;
	return 1;
}

void native_gen_rect(PSD psd , int l, int t, int r, int b)
{
	drawrow(psd, l, r, t);
	drawrow(psd, l, r, b);
	drawcol(psd, l, t, b);
	drawcol(psd, r, t, b);
}
/*
 * Draw an arbitrary line using the current clipping region and foreground color
 * If bDrawLastPoint is FALSE, draw up to but not including point x2, y2.
 *
 * This routine is the only routine that adjusts coordinates for supporting
 * two different types of upper levels, those that draw the last point
 * in a line, and those that draw up to the last point.  All other local
 * routines draw the last point.  This gives this routine a bit more overhead,
 * but keeps overall complexity down.
 */
void native_gen_line(PSD psd, int x1, int y1, int x2, int y2, BOOL bDrawLastPoint)
{
  int xdelta;			/* width of rectangle around line */
  int ydelta;			/* height of rectangle around line */
  int xinc;			/* increment for moving x coordinate */
  int yinc;			/* increment for moving y coordinate */
  int rem;			/* current remainder */
  int temp;

  /* See if the line is horizontal or vertical. If so, then call
   * special routines.
   */
  if (y1 == y2) {
	/*
	 * Adjust coordinates if not drawing last point.  Tricky.
	 */
	if(!bDrawLastPoint) {
		if (x1 > x2) {
			temp = x1;
			x1 = x2 + 1;
			x2 = temp;
		} else
			--x2;
	}

	/* call faster line drawing routine*/
	drawrow(psd, x1, x2, y1);
	return;
  }
  if (x1 == x2) {
	/*
	 * Adjust coordinates if not drawing last point.  Tricky.
	 */
	if(!bDrawLastPoint) {
		if (y1 > y2) {
			temp = y1;
			y1 = y2 + 1;
			y2 = temp;
		} else
			--y2;
	}

	/* call faster line drawing routine*/
	drawcol(psd, x1, y1, y2);
	return;
  }

  /* The line may be partially obscured. Do the draw line algorithm
   * checking each point against the clipping regions.
   */
  xdelta = x2 - x1;
  ydelta = y2 - y1;
  if (xdelta < 0) xdelta = -xdelta;
  if (ydelta < 0) ydelta = -ydelta;
  xinc = (x2 > x1) ? 1 : -1;
  yinc = (y2 > y1) ? 1 : -1;
  if (psd->ClipPoint(psd, x1, y1))
	  psd->DrawPixel(psd, x1, y1, psd->gr_foreground);
  if (xdelta >= ydelta) {
	rem = xdelta / 2;
	for(;;) {
		if(!bDrawLastPoint && x1 == x2)
			break;
		x1 += xinc;
		rem += ydelta;
		if (rem >= xdelta) {
			rem -= xdelta;
			y1 += yinc;
		}
		if (psd->ClipPoint(psd, x1, y1))
			psd->DrawPixel(psd, x1, y1, psd->gr_foreground);
		if(bDrawLastPoint && x1 == x2)
			break;
	}
  } else {
	rem = ydelta / 2;
	for(;;) {
		if(!bDrawLastPoint && y1 == y2)
			break;
		y1 += yinc;
		rem += xdelta;
		if (rem >= ydelta) {
			rem -= ydelta;
			x1 += xinc;
		}
		if (psd->ClipPoint(psd, x1, y1))
			psd->DrawPixel(psd, x1, y1, psd->gr_foreground);
		if(bDrawLastPoint && y1 == y2)
			break;
	}
  }
}

/* Draw a horizontal line from x1 to and including x2 in the
 * foreground color, applying clipping if necessary.
 */
static void drawrow(PSD psd, int x1, int x2, int y)
{
	int temp;
	/* reverse endpoints if necessary*/
	if (x1 > x2) {
		temp = x1;
		x1 = x2;
		x2 = temp;
	}

	/* clip to physical device*/
	if ( psd->doclip) {
		if ( (x2 < psd->clipminx) || ( x1 >= psd->clipmaxx) )
			return;
		if ( (y < psd->clipminy) || (y >= psd->clipmaxy) )
			  return;
		if (x2 < psd->clipminx)
			  return ;
		if (x1 >= psd->clipmaxx)
			  return;

		if (x1 < psd->clipminx)
			  x1 = psd->clipminx;
		if (x2 >= psd->clipmaxx)
			  x2 = psd->clipmaxx - 1;
	} else {
		if ( (x2 < 0) || ( x1 >= psd->xres) )
			return;
		if ( (y < 0) || (y >= psd->yres) )
			  return;
		if (x1 < 0)
			  x1 = 0;
		if (x2 >= psd->xres)
			  x2 = psd->xres - 1;
	}
	psd->DrawHLine(psd, x1, y, x2-x1+1, psd->gr_foreground);
}

/* 
 * Draw a vertical line from y1 to and including y2 in the
 * foreground color, applying clipping if necessary.
 */
static void drawcol(PSD psd, int x,int y1,int y2)
{
	int temp;

	/* reverse endpoints if necessary*/
	if (y1 > y2) {
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	/* clip to physical device*/
	if ( psd->doclip) {
		if ( (x < psd->clipminx) || ( x >= psd->clipmaxx) )
			return;
		if ( (y2 < psd->clipminy) || (y1 >= psd->clipmaxy) )
			  return;

		if (y1 < psd->clipminy)
			  y1 = psd->clipminy;
		if (y2 >= psd->clipmaxy)
			  y2 = psd->clipmaxy - 1;
	} else {
		if ( (x < 0) || ( x >= psd->xres) )
			return;
		if ( (y2 < 0) || (y1 >= psd->yres) )
			return;
		if (y1 < 0)
			  y1 = 0;
		if (y2 >= psd->yres)
			  y2 = psd->yres - 1;
	}

	psd->DrawVLine(psd, x, y1, y2-y1+1, psd->gr_foreground);
}

#if 0
void generate_palette(GAL_Color *pal, int num)
{
	GAL_Color black  = { 0, 0, 0, 0 };
	GAL_Color white  = { 255, 255, 255, 0 };
	GAL_Color blue   = { 0, 0, 255, 0 };
	GAL_Color yellow = { 255, 255, 0, 0 };

	int i, depth, N;

	int rnum,  gnum,  bnum;
	int rmask, gmask, bmask;
	
	/* handle small palettes */

	if (num == 0) return;

	pal[0] = black;   if (num == 1) return;
	pal[1] = white;   if (num == 2) return;
	pal[2] = blue;    if (num == 3) return;
	pal[3] = yellow;  if (num == 4) return;

	/* handle large palettes. */

	depth = 0;  /* work out depth so that (1 << depth) >= num */

	for (N = num-1; N > 0; N /= 2) {
		depth++;
	}

	gnum = (depth + 2) / 3;  /* green has highest priority */
	rnum = (depth + 1) / 3;  /* red has second highest priority */
	bnum = (depth + 0) / 3;  /* blue has lowest priority */

	gmask = (1 << gnum) - 1;
	rmask = (1 << rnum) - 1;
	bmask = (1 << bnum) - 1;

	for (i=0; i < num; i++) {

		/* When num < (1 << depth), we interpolate the values so
		 * that we still get a good range.  There's probably a
		 * better way...
		 */
		
		int j = i * ((1 << depth) - 1) / (num - 1);
		int r, g, b;
		
		b = j & bmask;  j >>= bnum;
		r = j & rmask;  j >>= rnum;
		g = j & gmask;

		pal[i].r = r * 0xffff / rmask;
		pal[i].g = g * 0xffff / gmask;
		pal[i].b = b * 0xffff / bmask;
		pal[i].a = 0;
	}
}
#endif
//==================draw a circle =====================================

static inline void setpixel(PSD psd, int x, int y, int c)
{
	if( psd->ClipPoint (psd, x, y))
		psd->DrawPixel(psd, x, y, c);
}

		
			
static void setcirclepixels(PSD psd, int x, int y, int sx, int sy, int c)
{
	if ( psd->doclip ) {
		int z;
		z = MAX (x, y);
		if (sx - z < psd->clipminx || sx + z >= psd->clipmaxx || sy - z < psd->clipminy || sy + z >= psd->clipmaxy) {
		    	/* use setpixel clipping */
			setpixel(psd, sx + x, sy + y, c);
			setpixel(psd, sx - x, sy + y, c);
			setpixel(psd, sx + x, sy - y, c);
			setpixel(psd, sx - x, sy - y, c);
			setpixel(psd, sx + y, sy + x, c);
		 	setpixel(psd, sx - y, sy + x, c);
			setpixel(psd, sx + y, sy - x, c);
			setpixel(psd, sx - y, sy - x, c);
			return;
		}
	}
	psd->DrawPixel(psd, sx + x, sy + y, c);
	psd->DrawPixel(psd, sx - x, sy + y, c);
	psd->DrawPixel(psd, sx + x, sy - y, c);
	psd->DrawPixel(psd, sx - x, sy - y, c);
	psd->DrawPixel(psd, sx + y, sy + x, c);
	psd->DrawPixel(psd, sx - y, sy + x, c);
	psd->DrawPixel(psd, sx + y, sy - x, c);
	psd->DrawPixel(psd, sx - y, sy - x, c);
}

void native_gen_circle(PSD psd, int sx, int sy, int r, int c)
{
	int x, y, d;

	if (r < 1) {
		setpixel(psd, sx, sy, c);
		return;
	}

	if (psd->doclip){
		if (sx + r < psd->clipminx) return;
		if (sx - r >= psd->clipmaxx) return; 
		if (sy + r < psd->clipminy) return;
		if (sy - r >= psd->clipmaxy) return; 
	}

	x = 0;
	y = r;
	d = 1 - r;

	setcirclepixels(psd, x, y, sx, sy, c);
	while (x < y) {
		if (d < 0)
		    d += x * 2 + 3;
		else {
		    d += x * 2 - y * 2 + 5;
		    y--;
		}
		x++;
		setcirclepixels(psd, x, y, sx, sy, c);
	}
}

//=================================scale box =================================================
/*      scalebox comes from SVGALib, Copyright 1993 Harm Hanemaayer         */
typedef unsigned char uchar;

/* We use the 32-bit to 64-bit multiply and 64-bit to 32-bit divide of the */
/* 386 (which gcc doesn't know well enough) to efficiently perform integer */
/* scaling without having to worry about overflows. */
#ifdef USE_ASM
static inline int muldiv64(int m1, int m2, int d)
{
/* int32 * int32 -> int64 / int32 -> int32 */
    int result;
    __asm__(
           "imull %%edx\n\t"
           "idivl %3\n\t"
  :           "=a"(result)    /* out */
  :           "a"(m1), "d"(m2), "g"(d)        /* in */
  :           "ax", "dx"    /* mod */
    );
    return result;
}
#else
static inline int muldiv64 (int m1, int m2, int d)
{
    Sint64 mul = (Sint64) m1 * m2;

    return (int) (mul / d);
}

#endif

/* This is a DDA-based algorithm. */
/* Iteration over target bitmap. */
int native_gen_scalebox (PSD psd, int w1, int h1, void *_dp1, int w2, int h2, void *_dp2)
{
    uchar *dp1;
    uchar *dp2;
    int xfactor;
    int yfactor;

    dp1 = _dp1;
    dp2 = _dp2;
    
    if (w2 == 0 || h2 == 0)
        return 0;

    xfactor = muldiv64(w1, 65536, w2);        /* scaled by 65536 */
    yfactor = muldiv64(h1, 65536, h2);        /* scaled by 65536 */

    switch ((psd->bpp + 7) / 8) {
    case 1:
        {
            int y, sy;
            sy = 0;
            for (y = 0; y < h2;) {
                int sx;
                uchar *dp2old;
                int x;
                dp2old = dp2;
                sx = 0;
                x = 0;
#if 0
                while (x < w2 - 8) {
                    *(dp2 + x) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 1) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 2) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 3) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 4) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 5) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 6) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    *(dp2 + x + 7) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    x += 8;
                }
#endif
                while (x < w2) {
                    *(dp2 + x) = *(dp1 + (sx >> 16));
                    sx += xfactor;
                    x++;
                }
                dp2 += w2;
                y++;
                while (y < h2) {
                    int l;
                    int syint;
                    syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    l = dp2 - dp2old;
                    memcpy(dp2, dp2old, l);
                    dp2old = dp2;
                    dp2 += l;
                    y++;
                }
                dp1 = (uchar *)_dp1 + (sy >> 16) * w1;
            }
        }
    break;
    case 2:
        {
            int y, sy;
            sy = 0;
            for (y = 0; y < h2;) {
                int sx;
                uchar *dp2old;
                int x;
                dp2old = dp2;
                x = 0;
                sx = 0;
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < w2) {
                    *(unsigned short *) (dp2 + x * 2) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 2);
                    sx += xfactor;
                    x++;
                }
                dp2 += w2 * 2;
                y++;
                while (y < h2) {
                    int l;
                    int syint;
                    syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    l = dp2 - dp2old;
                    memcpy(dp2, dp2old, l);
                    dp2old = dp2;
                    dp2 += l;
                    y++;
                }
                dp1 = (uchar *)_dp1 + (sy >> 16) * w1 * 2;
            }
        }
    break;
    case 3:
        {
            int y, sy;
            sy = 0;
            for (y = 0; y < h2;) {
                int sx;
                uchar *dp2old;
                int x;
                dp2old = dp2;
                x = 0;
                sx = 0;
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < w2) {
                    *(unsigned short *) (dp2 + x * 3) =
                        *(unsigned short *) (dp1 + (sx >> 16) * 3);
                    *(unsigned char *) (dp2 + x * 3 + 2) =
                        *(unsigned char *) (dp1 + (sx >> 16) * 3 + 2);
                    sx += xfactor;
                    x++;
                }
                dp2 += w2 * 3;
                y++;
                while (y < h2) {
                    int l;
                    int syint;
                    syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    l = dp2 - dp2old;
                    memcpy(dp2, dp2old, l);
                    dp2old = dp2;
                    dp2 += l;
                    y++;
                }
                dp1 = (uchar *)_dp1 + (sy >> 16) * w1 * 3;
            }
        }
    break;
    case 4:
        {
            int y, sy;
            sy = 0;
            for (y = 0; y < h2;) {
                int sx;
                uchar *dp2old;
                int x;
                dp2old = dp2;
                sx = 0;
                x = 0;
                /* This can be greatly optimized with loop */
                /* unrolling; omitted to save space. */
                while (x < w2) {
                    *(unsigned *) (dp2 + x * 4) =
                        *(unsigned *) (dp1 + (sx >> 16) * 4);
                    sx += xfactor;
                    x++;
                }
                dp2 += w2 * 4;
                y++;
                while (y < h2) {
                    int l;
                    int syint;
                    syint = sy >> 16;
                    sy += yfactor;
                    if ((sy >> 16) != syint)
                        break;
                    /* Copy identical lines. */
                    l = dp2 - dp2old;
                    memcpy(dp2, dp2old, l);
                    dp2old = dp2;
                    dp2 += l;
                    y++;
                }
                dp1 = (uchar *)_dp1 + (sy >> 16) * w1 * 4;
            }
        }
    break;
    }

    return 0;
}

