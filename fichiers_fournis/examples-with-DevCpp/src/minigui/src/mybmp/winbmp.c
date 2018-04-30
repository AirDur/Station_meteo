/* $Id: winbmp.c,v 1.14 2004/10/19 07:34:54 weiym Exp $
**
** Low-level Windows bitmap read/save function.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/08/26, derived from original bitmap.c
**
** Current maintainer: Wei Yongming.
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
#include "gdi.h"
#include "readbmp.h"

/************************* Bitmap-related structures  ************************/
typedef struct tagRGBTRIPLE
{
    BYTE    rgbtBlue;
    BYTE    rgbtGreen;
    BYTE    rgbtRed;
} RGBTRIPLE;
typedef RGBTRIPLE* PRGBTRIPLE;

typedef struct tagRGBQUAD
{
    BYTE    rgbBlue;
    BYTE    rgbGreen;
    BYTE    rgbRed;
    BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD* PRGBQUAD;

#define SIZEOF_RGBQUAD      4

#define BI_RGB          0
#define BI_RLE8         1
#define BI_RLE4         2
#define BI_BITFIELDS    3

#define SIZEOF_BMFH     14
#define SIZEOF_BMIH     40

#define OS2INFOHEADERSIZE  12
#define WININFOHEADERSIZE  40

typedef struct BITMAPFILEHEADER
{
   unsigned short bfType;
   unsigned long  bfSize;
   unsigned short bfReserved1;
   unsigned short bfReserved2;
   unsigned long  bfOffBits;
} BITMAPFILEHEADER;


/* Used for both OS/2 and Windows BMP. 
 * Contains only the parameters needed to load the image 
 */
typedef struct BITMAPINFOHEADER
{
   unsigned long  biWidth;
   unsigned long  biHeight;
   unsigned short biBitCount;
   unsigned long  biCompression;
} BITMAPINFOHEADER;


typedef struct WINBMPINFOHEADER  /* size: 40 */
{
   unsigned long  biSize;
   unsigned long  biWidth;
   unsigned long  biHeight;
   unsigned short biPlanes;
   unsigned short biBitCount;
   unsigned long  biCompression;
   unsigned long  biSizeImage;
   unsigned long  biXPelsPerMeter;
   unsigned long  biYPelsPerMeter;
   unsigned long  biClrUsed;
   unsigned long  biClrImportant;
} WINBMPINFOHEADER;


typedef struct OS2BMPINFOHEADER  /* size: 12 */
{
   unsigned long  biSize;
   unsigned short biWidth;
   unsigned short biHeight;
   unsigned short biPlanes;
   unsigned short biBitCount;
} OS2BMPINFOHEADER;


/* read_bmfileheader:
 *  Reads a BMP file header and check that it has the BMP magic number.
 */
static int read_bmfileheader(MG_RWops *f, BITMAPFILEHEADER *fileheader)
{
   fileheader->bfType = fp_igetw(f);
   fileheader->bfSize= fp_igetl(f);
   fileheader->bfReserved1= fp_igetw(f);
   fileheader->bfReserved2= fp_igetw(f);
   fileheader->bfOffBits= fp_igetl(f);

   if (fileheader->bfType != 19778)
      return -1;

   return 0;
}


/* read_win_bminfoheader:
 *  Reads information from a BMP file header.
 */
static int read_win_bminfoheader(MG_RWops *f, BITMAPINFOHEADER *infoheader)
{
   WINBMPINFOHEADER win_infoheader;

   win_infoheader.biWidth = fp_igetl(f);
   win_infoheader.biHeight = fp_igetl(f);
   win_infoheader.biPlanes = fp_igetw(f);
   win_infoheader.biBitCount = fp_igetw(f);
   win_infoheader.biCompression = fp_igetl(f);
   win_infoheader.biSizeImage = fp_igetl(f);
   win_infoheader.biXPelsPerMeter = fp_igetl(f);
   win_infoheader.biYPelsPerMeter = fp_igetl(f);
   win_infoheader.biClrUsed = fp_igetl(f);
   win_infoheader.biClrImportant = fp_igetl(f);


   infoheader->biWidth = win_infoheader.biWidth;
   infoheader->biHeight = win_infoheader.biHeight;
   infoheader->biBitCount = win_infoheader.biBitCount;
   infoheader->biCompression = win_infoheader.biCompression;

   return 0;
}


/* read_os2_bminfoheader:
 *  Reads information from an OS/2 format BMP file header.
 */
static int read_os2_bminfoheader(MG_RWops *f, BITMAPINFOHEADER *infoheader)
{
   OS2BMPINFOHEADER os2_infoheader;

   os2_infoheader.biWidth = fp_igetw(f);
   os2_infoheader.biHeight = fp_igetw(f);
   os2_infoheader.biPlanes = fp_igetw(f);
   os2_infoheader.biBitCount = fp_igetw(f);

   infoheader->biWidth = os2_infoheader.biWidth;
   infoheader->biHeight = os2_infoheader.biHeight;
   infoheader->biBitCount = os2_infoheader.biBitCount;
   infoheader->biCompression = 0;

   return 0;
}


/* read_bmicolors:
 *  Loads the color palette for 1,4,8 bit formats.
 */
static void read_bmicolors (int ncols, RGB *pal, MG_RWops *f, int win_flag)
{
   int i;

   for (i=0; i<ncols; i++) {
      pal[i].b = fp_getc(f);
      pal[i].g = fp_getc(f);
      pal[i].r = fp_getc(f);
      if (win_flag)
	    fp_getc(f);
   }
}

/* read_RLE8_compressed_image:
 *  For reading the 8 bit RLE compressed BMP image format.
 */
static void read_RLE8_compressed_image (MG_RWops *f, BYTE *bits, int pitch, BITMAPINFOHEADER *infoheader)
{
    unsigned char count, val, val0;
    int j, pos, line;
    int eolflag, eopicflag;

    eopicflag = 0;
    line = infoheader->biHeight - 1;

   while (eopicflag == 0) {
      pos = 0;                               /* x position in bitmap */
      eolflag = 0;                           /* end of line flag */

      while ((eolflag == 0) && (eopicflag == 0)) {
	 count = fp_getc(f);
	 val = fp_getc(f);

	 if (count > 0) {                    /* repeat pixel count times */
	    for (j=0;j<count;j++) {
	       bits [pos] = val;
	       pos++;
	    }
	 }
	 else {
	    switch (val) {

	       case 0:                       /* end of line flag */
		  eolflag=1;
		  break;

	       case 1:                       /* end of picture flag */
		  eopicflag=1;
		  break;

	       case 2:                       /* displace picture */
		  count = fp_getc(f);
		  val = fp_getc(f);
		  pos += count;
		  line -= val;
		  break;

	       default:                      /* read in absolute mode */
		  for (j=0; j<val; j++) {
		     val0 = fp_getc(f);
             bits [pos] = val0;
		     pos++;
		  }

		  if (j%2 == 1)
		     val0 = fp_getc(f);    /* align on word boundary */
		  break;

	    }
	 }

	 if (pos-1 > (int)infoheader->biWidth)
	    eolflag=1;
      }

      bits += pitch;
      line--;
      if (line < 0)
	    eopicflag = 1;
   }
}

/* read_RLE4_compressed_image:
 *  For reading the 4 bit RLE compressed BMP image format.
 */
static void read_RLE4_compressed_image (MG_RWops *f, BYTE *bits, int pitch, BITMAPINFOHEADER *infoheader)
{
   unsigned char b[8];
   unsigned char count;
   unsigned short val0, val;
   int j, k, pos, line;
   int eolflag, eopicflag;

   eopicflag = 0;                            /* end of picture flag */
   line = infoheader->biHeight - 1;

   while (eopicflag == 0) {
      pos = 0;
      eolflag = 0;                           /* end of line flag */

      while ((eolflag == 0) && (eopicflag == 0)) {
	 count = fp_getc(f);
	 val = fp_getc(f);

	 if (count > 0) {                    /* repeat pixels count times */
	    b[1] = val & 15;
	    b[0] = (val >> 4) & 15;
	    for (j=0; j<count; j++) {
            if (pos % 2 == 0)
                bits [pos/2] = b[j%2] << 4;
            else
                bits [pos/2] = bits [pos/2] | b[j%2];
	       pos++;
	    }
	 }
	 else {
	    switch (val) {

	       case 0:                       /* end of line */
		  eolflag=1;
		  break;

	       case 1:                       /* end of picture */
		  eopicflag=1;
		  break;

	       case 2:                       /* displace image */
		  count = fp_getc(f);
		  val = fp_getc(f);
		  pos += count;
		  line -= val;
		  break;

	       default:                      /* read in absolute mode */
		  for (j=0; j<val; j++) {
		     if ((j%4) == 0) {
			val0 = fp_igetw(f);
			for (k=0; k<2; k++) {
			   b[2*k+1] = val0 & 15;
			   val0 = val0 >> 4;
			   b[2*k] = val0 & 15;
			   val0 = val0 >> 4;
			}
		     }
            if (pos % 2 == 0)
                bits [pos/2] = b[j%4] << 4;
            else
                bits [pos/2] = bits [pos/2] | b[j%4];
		     pos++;
		  }
		  break;
	    }
	 }

	 if (pos-1 > (int)infoheader->biWidth)
	    eolflag=1;
      }

      bits += pitch;
      line--;
      if (line < 0)
	    eopicflag = 1;
   }
}

/* read_16bit_image:
 *  For reading the 16-bit BMP image format.
 * This only support bit masks specific to Windows 95.
 */
static void read_16bit_image (MG_RWops *f, BYTE *bits, int pitch, BITMAPINFOHEADER *infoheader, DWORD gmask)
{
    int i, j;
    WORD pixel;
    BYTE *line;

    for (i = 0; i < infoheader->biHeight; i++) {

        line = bits;
        for (j = 0; j < infoheader->biWidth; j++) {
            pixel = fp_igetw (f);
            if (gmask == 0x03e0)    /* 5-5-5 */
            {
                line [2] = ((pixel >> 10) & 0x1f) << 3;
                line [1] = ((pixel >> 5) & 0x1f) << 3;
                line [0] = (pixel & 0x1f) << 3;
            }
            else                    /* 5-6-5 */
            {
                line [2] = ((pixel >> 11) & 0x1f) << 3;
                line [1] = ((pixel >> 5) & 0x3f) << 2;
                line [0] = (pixel & 0x1f) << 3;
            }

            line += 3;
        }

        if (infoheader->biWidth & 0x01)
            pixel = fp_igetw (f);   /* read the gap */

        bits += pitch;
    }
}

/* __mg_load_bmp:
 *  Loads a Windows BMP file, returning in the my_bitmap structure and storing
 *  the palette data in the specified palette (this should be an array of
 *  at least 256 RGB structures).
 *
 *  Thanks to Seymour Shlien for contributing this function.
 */
int __mg_load_bmp (MG_RWops* fp, MYBITMAP* bmp, RGB* pal)
{
    BITMAPFILEHEADER fileheader;
    BITMAPINFOHEADER infoheader;
    int effect_depth;
    DWORD rmask, gmask, bmask;
    unsigned long biSize;
    BYTE* bits;
    int pitch;
    int ncol;
    
    rmask = 0x001f;
    gmask = 0x03e0;
    bmask = 0x7c00;

    if (read_bmfileheader (fp, &fileheader) != 0) {
        return ERR_BMP_IMAGE_TYPE;
    }

    biSize = fp_igetl (fp);

    if (biSize >= WININFOHEADERSIZE) {
        if (read_win_bminfoheader (fp, &infoheader) != 0) {
	        return ERR_BMP_LOAD;
        }
        MGUI_RWseek (fp, biSize - WININFOHEADERSIZE, SEEK_CUR);
        /* compute number of colors recorded */
        ncol = (fileheader.bfOffBits - biSize - 14) / 4;
        read_bmicolors (ncol, pal, fp, 1);
    }
    else if (biSize == OS2INFOHEADERSIZE) {
        if (read_os2_bminfoheader (fp, &infoheader) != 0) {
	        return ERR_BMP_LOAD;
        }
        /* compute number of colors recorded */
        ncol = (fileheader.bfOffBits - 26) / 3;
        read_bmicolors (ncol, pal, fp, 0);
    }
    else
	    return ERR_BMP_LOAD;

    /* Compute the size and pitch of the image */
    if (infoheader.biBitCount == 16)
        effect_depth = 24;
    else
        effect_depth = infoheader.biBitCount;
        
    bmpComputePitch (effect_depth, infoheader.biWidth, &pitch, TRUE);
    biSize = pitch * infoheader.biHeight;
    
    if( !(bits = malloc (biSize)) ) {
        return ERR_BMP_MEM;
    }
    bmp->bits = bits;

    bmp->flags = MYBMP_TYPE_BGR | MYBMP_FLOW_UP;

    switch (infoheader.biCompression) {
    case BI_BITFIELDS: /* ignore the bit fileds */
        MGUI_RWseek (fp, -16, SEEK_CUR);
        rmask = fp_igetl (fp);
        gmask = fp_igetl (fp);
        bmask = fp_igetl (fp);
        //uHALr_printf( "__mg_load_bmp: discard the bit masks: %lx, %lx, %lx.\n", rmask, gmask, bmask);
    case BI_RGB:
        if (infoheader.biBitCount == 16) {
            bmp->flags |= MYBMP_RGBSIZE_3;
            read_16bit_image (fp, bits, pitch, &infoheader, gmask);
        }
        else if (infoheader.biBitCount == 32) {
            bmp->flags |= MYBMP_RGBSIZE_4;
            MGUI_RWread (fp, bits, sizeof(char), biSize);
        }
        else {
            bmp->flags |= MYBMP_RGBSIZE_3;
            MGUI_RWread (fp, bits, sizeof(char), biSize);
        }
	    break;

    case BI_RLE8:
	    read_RLE8_compressed_image (fp, bits, pitch, &infoheader);
	    break;

    case BI_RLE4:
	    read_RLE4_compressed_image (fp, bits, pitch, &infoheader);
	    break;

    default:
        return ERR_BMP_NOT_SUPPORTED;
    }

    bmp->depth = effect_depth;
    bmp->w     = infoheader.biWidth;
    bmp->h     = infoheader.biHeight;
    bmp->pitch = pitch;
    bmp->frames = 1;
    bmp->size  = biSize;

    return ERR_BMP_OK;
}

BOOL __mg_check_bmp (MG_RWops* fp)
{
   WORD bfType;
   
   bfType = fp_igetw (fp);

   if (bfType != 19778)
      return FALSE;

   return TRUE;
}

#ifdef _SAVE_BITMAP
static void bmpGet16CScanline (BYTE* bits, BYTE* scanline, 
                        int pixels)
{
    int i;

    for (i = 0; i < pixels; i++) {
        if (i % 2 == 0)
            *scanline = (bits [i] << 4) & 0xF0;
        else {
            *scanline |= bits [i] & 0x0F;
            scanline ++;
        }
    }
}

static inline void bmpGet256CScanline (BYTE* bits, BYTE* scanline, 
                        int pixels)
{
    memcpy (scanline, bits, pixels);
}

inline void pixel2rgb (gal_pixel pixel, GAL_Color* color, int depth)
{
    switch (depth) {
    case 24:
    case 32:
        color->r = (gal_uint8) ((pixel >> 16) & 0xFF);
        color->g = (gal_uint8) ((pixel >> 8) & 0xFF);
        color->b = (gal_uint8) (pixel & 0xFF);
        break;

    case 15:
        color->r = (gal_uint8)((pixel & 0x7C00) >> 7) | 0x07;
        color->g = (gal_uint8)((pixel & 0x03E0) >> 2) | 0x07;
        color->b = (gal_uint8)((pixel & 0x001F) << 3) | 0x07;
        break;

    case 16:
        color->r = (gal_uint8)((pixel & 0xF800) >> 8) | 0x07;
        color->g = (gal_uint8)((pixel & 0x07E0) >> 3) | 0x03;
        color->b = (gal_uint8)((pixel & 0x001F) << 3) | 0x07;
        break;
    }
}

static void bmpGetHighCScanline (BYTE* bits, BYTE* scanline, 
                        int pixels, int bpp, int depth)
{
    int i;
    gal_pixel c;
    GAL_Color color;

    for (i = 0; i < pixels; i++) {
        c = *((gal_pixel*)bits);

        pixel2rgb (c, &color, depth);
        *(scanline)     = color.b;
        *(scanline + 1) = color.g;
        *(scanline + 2) = color.r;

        bits += bpp;
        scanline += 3;
    }
}

inline static int depth2bpp (int depth)
{
    switch (depth) {
    case 4:
    case 8:
        return 1;
    case 15:
    case 16:
        return 2;
    case 24:
        return 3;
    case 32:
        return 4;
    }
    
    return 1;
}

int __mg_save_bmp (MG_RWops* fp, MYBITMAP* bmp, RGB* pal)
{
    BYTE* scanline;
    int i, bpp;
    int scanlinebytes;

    BITMAPFILEHEADER bmfh;
    WINBMPINFOHEADER bmih;
    
    scanline = NULL;

    memset (&bmfh, 0, sizeof (BITMAPFILEHEADER));
    bmfh.bfType         = MAKEWORD ('B', 'M');
    bmfh.bfReserved1    = 0;
    bmfh.bfReserved2    = 0;

    memset (&bmih, 0, sizeof (WINBMPINFOHEADER));
    bmih.biSize         = (DWORD)(WININFOHEADERSIZE);
    bmih.biWidth        = (DWORD)(bmp->w);
    bmih.biHeight       = (DWORD)(bmp->h);
    bmih.biPlanes       = 1;
    bmih.biCompression  = BI_RGB;

    bpp = depth2bpp (bmp->depth);
    switch (bmp->depth) {
        case 4:
            scanlinebytes       = (bmih.biWidth + 1)>>1;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = (DWORD)(bmih.biHeight*scanlinebytes);
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH
                                    + (SIZEOF_RGBQUAD<<4);
            bmfh.bfSize         = (DWORD)(bmfh.bfOffBits + bmih.biSizeImage);
            bmih.biBitCount     = 4;
            bmih.biClrUsed      = 16L;
            bmih.biClrImportant = 16L;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD), 1);
            
            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD), 1);

            for (i = 0; i < 16; i++) {
                RGBQUAD rgbquad;
                rgbquad.rgbRed = pal [i].r;
                rgbquad.rgbBlue = pal [i].b;
                rgbquad.rgbGreen = pal [i].g;
                MGUI_RWwrite (fp, &rgbquad, sizeof (char), sizeof (RGBQUAD));
            }
            
            for (i = bmp->h  - 1; i >= 0; i--) {
                bmpGet16CScanline (bmp->bits + i * bmp->pitch, scanline, bmp->w);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;

        case 8:
            scanlinebytes       = bmih.biWidth;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = bmih.biHeight*scanlinebytes;
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH
                                    + (SIZEOF_RGBQUAD<<8);
            bmfh.bfSize         = bmfh.bfOffBits + bmih.biSizeImage;
            bmih.biBitCount     = 8;
            bmih.biClrUsed      = 256;
            bmih.biClrImportant = 256;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD), 1);

            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD), 1);
            
            for (i = 0; i < 256; i++) {
                RGBQUAD rgbquad;
                rgbquad.rgbRed = pal [i].r;
                rgbquad.rgbBlue = pal [i].b;
                rgbquad.rgbGreen = pal [i].g;
                MGUI_RWwrite (fp, &rgbquad, sizeof (char), sizeof (RGBQUAD));
            }
            
            for (i = bmp->h - 1; i >= 0; i--) {
                bmpGet256CScanline (bmp->bits + bmp->pitch * i, scanline, bmp->w);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;

        default:
            scanlinebytes       = bmih.biWidth*3;
            scanlinebytes       = ((scanlinebytes + 3)>>2)<<2;

#ifdef HAVE_ALLOCA
            if (!(scanline = alloca (scanlinebytes))) return ERR_BMP_MEM;
#else
            if (!(scanline = malloc (scanlinebytes))) return ERR_BMP_MEM;
#endif
            memset (scanline, 0, scanlinebytes);

            bmih.biSizeImage    = bmih.biHeight*scanlinebytes;
            bmfh.bfOffBits      = SIZEOF_BMFH + SIZEOF_BMIH;
            bmfh.bfSize         = bmfh.bfOffBits + bmih.biSizeImage;
            bmih.biBitCount     = 24;

            MGUI_RWwrite (fp, &bmfh.bfType, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved1, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfReserved2, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmfh.bfOffBits, sizeof (DWORD), 1);
            
            MGUI_RWwrite (fp, &bmih.biSize, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biWidth, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biHeight, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biPlanes, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biBitCount, sizeof (WORD), 1);
            MGUI_RWwrite (fp, &bmih.biCompression, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biSizeImage, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biXPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biYPelsPerMeter, sizeof (LONG), 1);
            MGUI_RWwrite (fp, &bmih.biClrUsed, sizeof (DWORD), 1);
            MGUI_RWwrite (fp, &bmih.biClrImportant, sizeof (DWORD), 1);

            for (i = bmp->h - 1; i >= 0; i--) {
                bmpGetHighCScanline (bmp->bits + i * bmp->pitch, scanline, 
                                bmp->w, bpp, bmp->depth);
                MGUI_RWwrite (fp, scanline, sizeof (char), scanlinebytes);
            }
        break;
    }

#ifndef HAVE_ALLOCA
    free (scanline);
#endif

    return ERR_BMP_OK;
}

#endif

