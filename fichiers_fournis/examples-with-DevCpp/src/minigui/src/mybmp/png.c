/*
** $Id: png.c,v 1.14 2004/08/03 11:01:57 panweiguo Exp $
** 
** png.c: Low-level PNG file read routines.  
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming
** Originally by Zhang Yunfan 
**
** Create date: 2001/01/10
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

#ifdef _PNG_FILE_SUPPORT

#include <png.h>

typedef unsigned char uch;
typedef unsigned long ulg;

static uch *readpng_get_image( png_structp png_ptr, png_infop info_ptr,
     int *pChannels, ulg *pRowbytes);
static int readpng_init (png_structpp png_pptr, png_infopp info_pptr,
     MG_RWops* src, ulg* width, ulg* height, int* bitdepth, int* colortype);
static void readpng_cleanup(png_structp png_ptr, png_infop info_ptr,
    uch *image_data,int free_image_data);

static uch *readpng_get_image (png_structp png_ptr, png_infop info_ptr,
     int *pChannels, ulg *pRowbytes)
{
    png_uint_32 i, rowbytes;
    png_bytepp row_pointers = NULL;
    uch* image_data;

    /* setjmp() must be called in every function that calls a PNG-reading
     * libpng function */

    if (setjmp(png_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        return NULL;
    }

    /* all transformations have been registered; now update info_ptr data,
     * get rowbytes and channels, and allocate image memory */
    png_read_update_info (png_ptr, info_ptr);

    *pRowbytes = rowbytes = png_get_rowbytes (png_ptr, info_ptr);
    *pChannels = (int)png_get_channels (png_ptr, info_ptr);

    if ((image_data = (uch *)malloc (rowbytes * info_ptr->height)) == NULL) {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        return NULL;
    }
    if ((row_pointers = (png_bytepp)malloc (info_ptr->height*sizeof(png_bytep))) == NULL) {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        free (image_data);
        image_data = NULL;
        return NULL;
    }

    /* set the individual row_pointers to point at the correct offsets */
    for (i = 0;  i < info_ptr->height;  ++i)
        row_pointers[i] = image_data + i*rowbytes;

    /* now we can go ahead and just read the whole image */
    png_read_image (png_ptr, row_pointers);
    
    /* and we're done!  (png_read_end() can be omitted if no processing of
     * post-IDAT text/time/etc. is desired) */
    free (row_pointers);
    row_pointers = NULL;

    png_read_end (png_ptr, NULL);

    return image_data;
}

static void my_read_data_fn (png_structp png_ptr, png_bytep data, png_size_t length)
{
   png_size_t check;

   /* fread() returns 0 on error, so it is OK to store this in a png_size_t
    * instead of an int, which is what fread() actually returns.
    */
   check = (png_size_t)MGUI_RWread ((MG_RWops*)png_ptr->io_ptr, data, (png_size_t)1, length);

   if (check != length)
      png_error(png_ptr, "Read Error");
}

#define _PNG_SIG_ERROR  1
#define _PNG_MEM_ALLOC  2
#define _PNG_INIT_OK    0

static int readpng_init (png_structpp png_pptr, png_infopp info_pptr, MG_RWops* src,
     ulg* width, ulg* height, int* bitdepth, int* colortype)
{
    uch sig[9];

    MGUI_RWread (src, sig, 8, 1);
    if (!png_check_sig (sig, 8)) {
#ifdef PNG_DEBUG
        sig[8] = '\0';
        uHALr_printf( "%s: check error!\n", sig);
#endif
        return _PNG_SIG_ERROR;
    } else {
#ifdef PNG_DEBUG
        sig[8]='\0';
        uHALr_printf( "png sig is %s.\n", sig);
#endif
    }

    *png_pptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if (!*png_pptr){
        return _PNG_MEM_ALLOC;
    }

    *info_pptr = png_create_info_struct ( *png_pptr );
    if ( !*info_pptr ){
        png_destroy_read_struct (png_pptr, NULL,NULL);
        return _PNG_MEM_ALLOC;
    }

    if ( setjmp ( (*png_pptr)->jmpbuf ) ) {
        png_destroy_read_struct (png_pptr, info_pptr, NULL );
        return _PNG_MEM_ALLOC;
    }

    png_set_read_fn (*png_pptr, (void*)src, my_read_data_fn);

    png_set_sig_bytes ( *png_pptr, 8 );

    png_read_info ( *png_pptr, *info_pptr );

    png_get_IHDR ( *png_pptr, *info_pptr, width, height, bitdepth,
         colortype, NULL, NULL, NULL );

    return _PNG_INIT_OK;
}
        
static void readpng_cleanup (png_structp png_ptr, png_infop info_ptr,
    uch *image_data, int free_image_data)
{
    if (free_image_data && image_data) {
        free(image_data);
        image_data = NULL;
    }

    if (png_ptr && info_ptr) {
        png_destroy_read_struct (&png_ptr, &info_ptr, NULL);
        png_ptr = NULL;
        info_ptr = NULL;
    }
}

/* Get the transparent info */
static void get_transparent_info (png_structp png_ptr, png_infop info_ptr, 
                int bit_depth, int color_type, MYBITMAP* bmp)
{
    png_bytep trans;
    int num_trans;
    png_color_16p trans_values;

    if (png_get_tRNS (png_ptr, info_ptr, &trans, &num_trans, &trans_values)
                        && num_trans) {
        gal_uint8 r = 0, g = 0, b = 0;

        switch (color_type) {
            case PNG_COLOR_TYPE_GRAY:
                if (bit_depth < 8)
                    trans_values->gray = trans_values->gray << (8 - bit_depth);
                r = g = b = trans_values->gray;
                break;

            case PNG_COLOR_TYPE_PALETTE:
            {
                png_colorp pal;
                int num_pal;

                if (png_get_PLTE (png_ptr, info_ptr, &pal, &num_pal) && *trans < num_pal) {
                    r = pal [*trans].red;
                    g = pal [*trans].green;
                    b = pal [*trans].blue;
                }
                break;
            }

            case PNG_COLOR_TYPE_RGB:
                r = trans_values->red >> 8;
                g = trans_values->green >> 8;
                b = trans_values->blue >> 8;
                break;
        }

        bmp->flags |= MYBMP_TRANSPARENT;
        bmp->transparent = MakeRGB (r, g, b);
   }
}

int __mg_load_png (MG_RWops* area, MYBITMAP* bmp, RGB* pal)
{
    int rc;
    png_structp png_ptr;
    png_infop info_ptr;
    ulg width;
    ulg height;
    int bit_depth, intent, color_type, chanels;
    double screen_gamma;

    if ((rc = readpng_init(&png_ptr, &info_ptr, area, &width, &height, &bit_depth, &color_type)) != 0) {
        switch (rc) {
        case _PNG_SIG_ERROR:
            return ERR_BMP_IMAGE_TYPE;

        case _PNG_MEM_ALLOC:
            return ERR_BMP_MEM;

        default:
            return ERR_BMP_LOAD;
        }
    }

    bmp->flags = MYBMP_FLOW_DOWN | MYBMP_TYPE_RGB | MYBMP_RGBSIZE_3;
    bmp->depth = 24;
    bmp->w = width;
    bmp->h = height;
    bmp->frames = 1;

    /* tell libpng to strip 16 bit/color files down to 8 bits/color */
    if (bit_depth == 16)
        png_set_strip_16 (png_ptr);

    /* Extract multiple pixels with bit depths of 1, 2, and 4 from a single
     * byte into separate bytes (useful for paletted and grayscale images).
     */
    if (bit_depth < 8)
        png_set_packing (png_ptr);

    /* Expand grayscale images to the full 8 bits from 1, 2, or 4 bits/pixel */
    if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
        png_set_gray_1_2_4_to_8 (png_ptr);

    /* Expand paletted colors into true RGB triplets */
    if (color_type == PNG_COLOR_TYPE_PALETTE)
        png_set_palette_to_rgb (png_ptr);
    else if (color_type == PNG_COLOR_TYPE_GRAY)
        png_set_gray_to_rgb (png_ptr);

#ifdef _USE_NEWGAL
    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
        get_transparent_info (png_ptr, info_ptr, bit_depth, color_type, bmp);
    }

    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        bmp->flags = MYBMP_FLOW_DOWN | MYBMP_TYPE_RGBA | MYBMP_RGBSIZE_4 | MYBMP_ALPHA;
        bmp->depth = 32;
    }

    if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
        png_set_gray_to_rgb (png_ptr);
        bmp->flags = MYBMP_FLOW_DOWN | MYBMP_TYPE_RGBA | MYBMP_RGBSIZE_4 | MYBMP_ALPHA;
        bmp->depth = 32;
    }
#else
    if (png_get_valid (png_ptr, info_ptr, PNG_INFO_tRNS)) {
        get_transparent_info (png_ptr, info_ptr, bit_depth, color_type, bmp);
        png_set_strip_alpha (png_ptr);
    }
    else if (color_type & PNG_COLOR_MASK_ALPHA) {
        png_set_strip_alpha (png_ptr);
    }

#endif

#if 1
    screen_gamma = 2.2;  /* A good guess for a PC monitors in a dimly room */
#else
    screen_gamma = 1.7 or 1.0;  /* A good guess for Mac systems */
#endif

    /* Tell libpng to handle the gamma conversion */
    if (png_get_sRGB (png_ptr, info_ptr, &intent))
        png_set_gamma (png_ptr, screen_gamma, 0.45455);
    else
    {
        double image_gamma;
        if (png_get_gAMA (png_ptr, info_ptr, &image_gamma))
            png_set_gamma (png_ptr, screen_gamma, image_gamma);
        else
            png_set_gamma( png_ptr, screen_gamma, 0.45455);
    }

    bmp->bits = (unsigned char*) readpng_get_image (png_ptr, 
            info_ptr, &chanels, (ulg *)&bmp->pitch);

    readpng_cleanup (png_ptr, info_ptr, bmp->bits, FALSE);

    if (bmp->bits)
        return ERR_BMP_OK;

    return ERR_BMP_LOAD;
}

BOOL __mg_check_png (MG_RWops* fp)
{
    char sig [9];

    MGUI_RWread (fp, sig, 8, 1);
    return png_check_sig (sig, 8);
}

#endif /* _PNG_FILE_SUPPORT */

