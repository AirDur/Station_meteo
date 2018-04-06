/*
** $Id: mybmp.c,v 1.10 2004/08/03 11:01:57 panweiguo Exp $
**
** Top-level bitmap file read/save function.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/12/17, derived from original ../newgdi/readbmp.c
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
#include "gdi.h"
#include "readbmp.h"

/*
 * The idea comes from readbmp.c in Allegro by Shawn Hargreaves.
 */

typedef struct
{
   char ext[8];
   int (*load) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal);
   int (*save) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal);
   BOOL (*check) (MG_RWops* fp);
} BITMAP_TYPE_INFO;

#define MAX_BITMAP_TYPES   8

static BITMAP_TYPE_INFO bitmap_types [MAX_BITMAP_TYPES] =
{
   { "bmp", __mg_load_bmp, 

#ifdef _SAVE_BITMAP
__mg_save_bmp, __mg_check_bmp },
#else
NULL, __mg_check_bmp },
#endif

#ifdef _LBM_FILE_SUPPORT
   { "lbm", __mg_load_lbm, NULL, __mg_check_lbm },
#else
   { "",    NULL,     NULL, NULL },
#endif
#ifdef _PCX_FILE_SUPPORT
   { "pcx", __mg_load_pcx, NULL, NULL },
#else
   { "",    NULL,     NULL, NULL },
#endif
#ifdef _TGA_FILE_SUPPORT
   { "tga", __mg_load_tga, NULL, NULL },
#else
   { "",    NULL,     NULL, NULL },
#endif
#ifdef _GIF_FILE_SUPPORT
   { "gif", __mg_load_gif, NULL, __mg_check_gif },
#else
   { "",    NULL,     NULL, NULL },
#endif
#ifdef _JPG_FILE_SUPPORT
   { "jpg", __mg_load_jpg, NULL, __mg_check_jpg },
#else
   { "",    NULL,     NULL, NULL },
#endif
#ifdef _PNG_FILE_SUPPORT
   { "png", __mg_load_png, NULL, __mg_check_png },
#else
   { "",    NULL,     NULL     },
#endif
   { "",    NULL,     NULL     },
};

/* RegisterBitmapFileType
 *  Informs Allegro of a new image file type, telling it how to load and
 *  save files of this format (either function may be NULL).
 */
BOOL GUIAPI RegisterBitmapFileType (const char *ext, 
            int (*load) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal), 
            int (*save) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal),
            BOOL (*check) (MG_RWops* fp))
{
    int i;

    for (i = 0; i < MAX_BITMAP_TYPES; i++) {
        if ((!bitmap_types[i].ext [0]) || (strcasecmp (bitmap_types[i].ext, ext) == 0)) {
	        strncpy (bitmap_types[i].ext, ext, sizeof (bitmap_types[i].ext) - 1);
	        bitmap_types[i].ext [sizeof (bitmap_types[i].ext) - 1] = 0;
	        bitmap_types[i].load = load;
	        bitmap_types[i].save = save;
	        bitmap_types[i].check = check;
	        return TRUE;
      }
   }

    return FALSE;
}

#define PIX2BYTES(n)    (((n)+7)/8)
/*
 * compute image line size and bytes per pixel
 * from bits per pixel and width
 */
int bmpComputePitch(int bpp, Uint32 width, Uint32 *pitch, BOOL does_round)
{
    int linesize;
    int bytespp;
    
    bytespp = 1;

    if(bpp == 1)
        linesize = PIX2BYTES (width);
    else if(bpp <= 4)
        linesize = PIX2BYTES (width << 2);
    else if (bpp <= 8)
        linesize = width;
    else if(bpp <= 16) {
        linesize = width * 2;
        bytespp = 2;
    } else if(bpp <= 24) {
        linesize = width * 3;
        bytespp = 3;
    } else {
        linesize = width * 4;
        bytespp = 4;
    }

    /* rows are DWORD right aligned */
    if (does_round)
        *pitch = (linesize + 3) & -4;
    else
        *pitch = linesize;
    return bytespp;
}

static const char* get_extension (const char* filename)
{
    const char* ext;

    ext = strrchr (filename, '.');

    if (ext)
        return ext + 1;

    return NULL;
}

/* Function: int LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, MG_RWops* area, const char* ext);
 *      This function loads a bitmap from an image source and fills
 *      the specified MYBITMAP struct and returns the palette in pal if any.
 */
int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, MG_RWops* area, const char* ext)
{
    int type;
    int load_ret;

    for (type = 0; type < MAX_BITMAP_TYPES; type++) {
        if ((bitmap_types[type].ext [0]) && (strcasecmp (bitmap_types[type].ext, ext) == 0))
            break;
    }

    if (type == MAX_BITMAP_TYPES) {
        return ERR_BMP_UNKNOWN_TYPE;
    }

    if (bitmap_types[type].load == NULL) {
        return ERR_BMP_CANT_READ;
    }

    my_bmp->flags = MYBMP_LOAD_NONE;
    my_bmp->bits = NULL;
    my_bmp->frames = 1;
    my_bmp->depth = GetGDCapability (HDC_SCREEN, GDCAP_DEPTH);

    if (my_bmp->depth <= 8)
        GetPalette (HDC_SCREEN, 0, 256, (GAL_Color*)pal);

#ifdef _GRAY_SCREEN
    my_bmp->flags |= MYBMP_LOAD_GRAYSCALE;
#endif

    if ((load_ret = bitmap_types[type].load (area, my_bmp, pal)) != 0) {
        free (my_bmp->bits);
        return load_ret;
    }

    if (my_bmp->depth == 0) {
        uHALr_printf( "A bad implementation of load_bmp: %s.\n", ext);
        return ERR_BMP_LOAD;
    }

    return ERR_BMP_OK;
}

int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal, const char* file_name)
{
    MG_RWops* area;
    const char* ext;
    int ret_val;

    if ((ext = get_extension (file_name)) == NULL)
        return ERR_BMP_UNKNOWN_TYPE;

    if (!(area = MGUI_RWFromFile (file_name, "rb"))) {
        return ERR_BMP_FILEIO;
    }

    ret_val = LoadMyBitmapEx (my_bmp, pal, area, ext);

    MGUI_RWclose (area);

    return ret_val;
}

int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal, const void* mem, int size, const char* ext)
{
    MG_RWops* area;
    int ret_val;

    if (!(area = MGUI_RWFromMem ((void*)mem, size))) {
        return ERR_BMP_MEM;
    }

    ret_val = LoadMyBitmapEx (my_bmp, pal, area, ext);

    MGUI_RWclose (area);

    return ret_val;
}

void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp)
{
    free (my_bmp->bits);
}

const char* GUIAPI CheckBitmapType (MG_RWops* fp)
{
    int type;
    int start_pos;
    
    start_pos = MGUI_RWseek (fp, 0, SEEK_CUR);

    for (type = 0; type < MAX_BITMAP_TYPES; type++) {
        if (bitmap_types[type].check) {
            if (bitmap_types[type].check (fp)) {
                MGUI_RWseek (fp, start_pos, SEEK_SET);
                return bitmap_types[type].ext;
            }
            else {
                MGUI_RWseek (fp, start_pos, SEEK_SET);
            }
        }
    }

    return NULL;
}

#ifdef _SAVE_BITMAP
int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal, const char* spFileName)
{
    MG_RWops* fp;
    const char* ext;
    int type;
    int save_ret;

    if ((ext = get_extension (spFileName)) == NULL)
        return ERR_BMP_UNKNOWN_TYPE;

    for (type = 0; type < MAX_BITMAP_TYPES; type++) {
        if ((bitmap_types[type].ext [0]) && (strcasecmp (bitmap_types[type].ext, ext) == 0))
            break;
    }

    if (type == MAX_BITMAP_TYPES)
        return ERR_BMP_UNKNOWN_TYPE;

    if (bitmap_types[type].save == NULL)
        return ERR_BMP_CANT_SAVE;

    if (!(fp = MGUI_RWFromFile (spFileName, "wb+"))) {
#ifdef _DEBUG
        uHALr_printf( "Saving BMP file: fopen error.\n");
#endif
        return ERR_BMP_FILEIO;
    }

	save_ret = bitmap_types[type].save (fp, my_bmp, pal);
    MGUI_RWclose (fp);
    return save_ret;
}

#endif

