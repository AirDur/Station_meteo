/*
** $Id: genfunc_clipline.c,v 1.4 2003/09/04 02:57:09 weiym Exp $
** 
** genfunc_clipline.c: 
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 Song Lixin
** Copyright (C) 1998 Alexander Larsson   [alla@lysator.liu.se]
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
** This is a line-clipper using the algorithm by cohen-sutherland.
**
** It is modified to do pixel-perfect clipping. This means that it
** will generate the same endpoints that would be drawn if an ordinary
** bresenham line-drawer where used and only visible pixels drawn.
**
** It can be used with a bresenham-like linedrawer if it is modified to
** start with a correct error-term.
*/

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "minigui.h"
#include "gal.h"
#include "native.h"

#define OC_LEFT 1
#define OC_RIGHT 2
#define OC_TOP 4
#define OC_BOTTOM 8

/* Outcodes:
+-> x
|       |      | 
V  0101 | 0100 | 0110
y ---------------------
   0001 | 0000 | 0010
  ---------------------
   1001 | 1000 | 1010
        |      | 
 */
#define outcode(code, xx, yy) \
{\
  code = 0;\
 if (xx < tempclipminx)\
    code |= OC_LEFT;\
  else if (xx > tempclipmaxx)\
    code |= OC_RIGHT;\
  if (yy < tempclipminy)\
    code |= OC_TOP;\
  else if (yy>tempclipmaxy)\
    code |= OC_BOTTOM;\
}

/*
  Calculates |_ a/b _| with mathematically correct floor
  */
static int FloorDiv(int a, int b)
{
	int floor;
	if (b>0) {
		if (a>0) {
			return a /b;
		} else {
			floor = -((-a)/b);
			if ((-a)%b != 0)
				floor--;
		}
		return floor;
	} else {
		if (a>0) {
			floor = -(a/(-b));
			if (a%(-b) != 0)
				floor--;
			return floor;
		} else {
			return (-a)/(-b);
		}
	}
}
/*
  Calculates |^ a/b ^| with mathamatically correct floor
  */
static int CeilDiv(int a,int b)
{
	if (b>0)
		return FloorDiv(a-1,b)+1;
	else
		return FloorDiv(-a-1,-b)+1;
}

int cs_clipline(PSD psd,int *_x0, int *_y0, int *_x1, int *_y1,
		       int *clip_first, int *clip_last)
{
	int first,last, code;
	int x0,y0,x1,y1;
	int x,y;
	int dx,dy;
	int xmajor;
	int slope;
	
	int tempclipminx,tempclipminy,tempclipmaxx,tempclipmaxy;
	if (psd->doclip) {
		tempclipminx = psd->clipminx;
		tempclipminy = psd->clipminy;
		tempclipmaxx = psd->clipmaxx;
		tempclipmaxy = psd->clipmaxy;
	} else {
		tempclipminx = 0;
		tempclipminy = 0;
		tempclipmaxx = psd->xres - 1;
		tempclipmaxy = psd->yres - 1;
	}	

	first = 0;
	last = 0;
	outcode(first,*_x0,*_y0);
	outcode(last,*_x1,*_y1);

	if ((first | last) == 0) {
		return CLIP_VISIBLE; /* Trivially accepted! */
	}
	if ((first & last) != 0) {
		return CLIP_INVISIBLE; /* Trivially rejected! */
	}

	x0=*_x0; y0=*_y0;
	x1=*_x1; y1=*_y1;

	dx = x1 - x0;
	dy = y1 - y0;
  
	xmajor = (abs(dx) > abs(dy));
	slope = ((dx>=0) && (dy>=0)) || ((dx<0) && (dy<0));
  
	for (;;) {
		code = first;
		if (first==0)
			code = last;

		if (code&OC_LEFT) {
			x = tempclipminx;
			if (xmajor) {
				y = *_y0 +  FloorDiv(dy*(x - *_x0)*2 + dx,
						      2*dx);
			} else {
				if (slope) {
					y = *_y0 + CeilDiv(dy*((x - *_x0)*2
							       - 1), 2*dx);
				} else {
					y = *_y0 + FloorDiv(dy*((x - *_x0)*2
								- 1), 2*dx);
				}
			}
		} else if (code&OC_RIGHT) {
			x = tempclipmaxx;
			if (xmajor) {
				y = *_y0 +  FloorDiv(dy*(x - *_x0)*2 + dx, 2*dx);
			} else {
				if (slope) {
					y = *_y0 + CeilDiv(dy*((x - *_x0)*2
							       + 1), 2*dx)-1;
				} else {
					y = *_y0 + FloorDiv(dy*((x - *_x0)*2
								+ 1), 2*dx)+1;
				}
			}
		} else if (code&OC_TOP) {
			y = tempclipminy;
			if (xmajor) {
				if (slope) {
					x = *_x0 + CeilDiv(dx*((y - *_y0)*2
							       - 1), 2*dy);
				} else {
					x = *_x0 + FloorDiv(dx*((y - *_y0)*2
								- 1), 2*dy);
				}
			} else {
				x = *_x0 +  FloorDiv( dx*(y - *_y0)*2 + dy,
						      2*dy);
			}
		} else { /* OC_BOTTOM */
			y = tempclipmaxy;
			if (xmajor) {
				if (slope) {
					x = *_x0 + CeilDiv(dx*((y - *_y0)*2
							       + 1), 2*dy)-1;
				} else {
					x = *_x0 + FloorDiv(dx*((y - *_y0)*2
								+ 1), 2*dy)+1;
				}
			} else {
				x = *_x0 +  FloorDiv(dx*(y - *_y0)*2 + dy,
						     2*dy);
			}
		}

		if (first!=0) {
			x0 = x;
			y0 = y;
			outcode(first,x0,y0);
			*clip_first = 1;
		} else {
			x1 = x;
			y1 = y;
			last = code;
			outcode(last,x1,y1);
			*clip_last = 1;
		}
    
		if ((first & last) != 0) {
			return CLIP_INVISIBLE; /* Trivially rejected! */
		}
		if ((first | last) == 0) {
			*_x0=x0; *_y0=y0;
			*_x1=x1; *_y1=y1;
			return CLIP_PARTIAL; /* Trivially accepted! */
		}
	}
}

