/*
** $Id: varbitmap.c,v 1.37 2005/01/05 04:07:11 weiym Exp $
** 
** varbitmap.c: The Var Bitmap Font operation set.
**
** Copyright (C) 2003, 2004, 2005 Feynman Software
** Copyright (C) 2000, 2001, 2002 Wei Yongming
**
** Create date: 2000/06/13
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
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "common.h"

#ifdef _VBF_SUPPORT

#include "minigui.h"
#include "gdi.h"
#include "endianrw.h"
#include "misc.h"

#ifdef HAVE_MMAP
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <sys/mman.h>
    #include <sys/stat.h>
#endif

#include "devfont.h"
#include "varbitmap.h"
#include "charset.h"
#include "fontname.h"

#ifdef _INCOREFONT_SANSSERIF
VBFINFO    vbf_SansSerif11x13;
#endif
#ifdef _INCOREFONT_COURIER
VBFINFO    vbf_Courier8x13;
#include  "vbf_Courier8x13_font.c"
#endif
#ifdef _INCOREFONT_SYMBOL
VBFINFO    vbf_symb12;
#endif
#ifdef _INCOREFONT_VGAS
VBFINFO    vbf_VGAOEM8x8;
VBFINFO    vbf_Terminal8x12;
VBFINFO    vbf_System14x16;
VBFINFO    vbf_Fixedsys8x15;
#endif
#ifdef _INCOREFONT_HELV
VBFINFO    vbf_helvR16;
VBFINFO    vbf_helvR21;
VBFINFO    vbf_helvR27;
#endif

/*************** Variable bitmap font operations *********************************/
static int get_char_width (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len)
{
    VBFINFO* vbf_info;
    
    vbf_info = VARFONT_INFO_P (devfont);

    if (vbf_info->width == NULL)
        return vbf_info->max_width;

    assert (len == 1);

    if (*mchar < vbf_info->first_char || *mchar > vbf_info->last_char)
        return vbf_info->width [vbf_info->def_char - vbf_info->first_char];

    return vbf_info->width [*mchar - vbf_info->first_char];
}

static int get_max_width (LOGFONT* logfont, DEVFONT* devfont)
{
   return VARFONT_INFO_P (devfont)->max_width; 
}

static int get_str_width (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mstr, int n, int cExtra)
{
    int i;
    int str_width;
    VBFINFO* vbf_info;
    
    vbf_info = VARFONT_INFO_P (devfont);
    
    if (vbf_info->width == NULL)
        return n * (vbf_info->max_width + cExtra);

    str_width = 0;
    for (i = 0; i < n; i++) {
        if (mstr [i] < vbf_info->first_char || mstr [i] > vbf_info->last_char)
            str_width += vbf_info->width [vbf_info->def_char - vbf_info->first_char];
        else
            str_width += vbf_info->width [mstr[i] - vbf_info->first_char];
        str_width += cExtra;
    }

    return str_width;
}

static int get_ave_width (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P(devfont)->ave_width;
}

static int get_font_height (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P (devfont)->height;
}

static int get_font_size (LOGFONT* logfont, DEVFONT* devfont, int expect)
{
    return VARFONT_INFO_P (devfont)->height;
}

static int get_font_ascent (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P (devfont)->height - VARFONT_INFO_P (devfont)->descent;
}

static int get_font_descent (LOGFONT* logfont, DEVFONT* devfont)
{
    return VARFONT_INFO_P (devfont)->descent;
}

static size_t char_bitmap_size (LOGFONT* logfont, DEVFONT* devfont, 
                const unsigned char* mchar, int len)
{
    int width;
    VBFINFO* vbf_info;
    
    vbf_info = VARFONT_INFO_P (devfont);

    if (vbf_info->width == NULL)
        width = vbf_info->max_width;
    else if (*mchar < vbf_info->first_char || *mchar > vbf_info->last_char)
        width = vbf_info->width [vbf_info->def_char - vbf_info->first_char];
    else
        width = vbf_info->width [*mchar - vbf_info->first_char];   

    return ((width + 7) >> 3) * vbf_info->height;
}

static size_t max_bitmap_size (LOGFONT* logfont, DEVFONT* devfont)
{
    return (((size_t)VARFONT_INFO_P (devfont)->max_width + 7) >> 3) 
                * VARFONT_INFO_P (devfont)->height;
}

static const void* get_char_bitmap (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len)
{
    int offset;
    unsigned char eff_char;
    VBFINFO* vbf_info;

    eff_char = *mchar;
    vbf_info = VARFONT_INFO_P (devfont);

    if (*mchar < vbf_info->first_char || *mchar > vbf_info->last_char)
        eff_char = vbf_info->def_char;

    if (vbf_info->offset == NULL)
        offset = (((size_t)vbf_info->max_width + 7) >> 3) * vbf_info->height 
                    * (eff_char - vbf_info->first_char);
    else {
         offset = (((unsigned char *)vbf_info->offset) [2*(eff_char - vbf_info->first_char)+1])<<8;
         offset += ((unsigned char *)vbf_info->offset) [2*(eff_char - vbf_info->first_char)];
         
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
        if (vbf_info->font_size)
            offset = ArchSwap16 (offset);
#endif
    }
    return vbf_info->bits + offset;
}

static int get_char_bbox (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len,
            int* px, int* py, int* pwidth, int* pheight)
{
    int tempint;
    VBFINFO* vbf_info;
    
    vbf_info = VARFONT_INFO_P (devfont);
    
    if (vbf_info->version == 2)
    {
        int offset;
        unsigned char eff_char;
        eff_char = *mchar;

        if (*mchar < vbf_info->first_char || *mchar > vbf_info->last_char)
            eff_char = vbf_info->def_char;
        offset = eff_char - vbf_info->first_char;
    
        if (px)
        {
            tempint = vbf_info->bbox_x[offset];
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *px += tempint;
        }
        if (py)
        {
            tempint = vbf_info->bbox_y[offset];
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *py -= tempint;
        }
        if (pwidth)
        {
            tempint = vbf_info->bbox_w[offset];
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *pwidth = tempint;
        }
        if (pheight)
        {
            tempint = vbf_info->bbox_h[offset];
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
            tempint = ArchSwap32(tempint);
#endif
            *pheight = tempint;
        }
    }
    else
    {
        if(pwidth)
            *pwidth     = get_char_width(logfont, devfont, mchar, len);
        if(pheight)
            *pheight    = vbf_info->height;
        if(py)
            *py        -= get_font_ascent(logfont, devfont);
        //if(px)
        //    *px = 0;
    }
    return pwidth?*pwidth:-1;
}

static void get_char_advance (LOGFONT* logfont, DEVFONT* devfont,
                const unsigned char* mchar, int len, 
                int* px, int* py)
{
    int width;
    width = get_char_width(logfont, devfont, mchar, len);
    //printf("x %d, y %d, width %d\n", *px, *py, width);
	*px += width;
}


/**************************** Global data ************************************/
static FONTOPS var_bitmap_font_ops = {
    get_char_width,
    get_str_width,
    get_ave_width,
    get_max_width,
    get_font_height,
    get_font_size,
    get_font_ascent,
    get_font_descent,
    char_bitmap_size,
    max_bitmap_size,
    get_char_bitmap,
    NULL,
    NULL,
    get_char_bbox,
    get_char_advance,
    NULL,
    NULL
};

/******************* In-core var bitmap fonts ********************************/

static VBFINFO* incore_vbfonts [] = {
#ifdef _INCOREFONT_SANSSERIF
    &vbf_SansSerif11x13,
#endif
#ifdef _INCOREFONT_COURIER
    &vbf_Courier8x13,
#endif
#ifdef _INCOREFONT_SYMBOL
    &vbf_symb12,
#endif
#ifdef _INCOREFONT_VGAS
    &vbf_VGAOEM8x8,
    &vbf_Terminal8x12,
    &vbf_System14x16,
    &vbf_Fixedsys8x15,
#endif
#ifdef _INCOREFONT_HELV
    &vbf_helvR16,
    &vbf_helvR21,
    &vbf_helvR27,
#endif
};

#define NR_VBFONTS  (sizeof (incore_vbfonts) /sizeof (VBFINFO*))

static DEVFONT* incore_vbf_dev_font;

static CHARSETOPS* vbfGetCharsetOps (VBFINFO* vbfont)
{
    int count;
    const char* font_name;
    
    count = 0;
    font_name = vbfont->name;

    while (*font_name) {
        if (*font_name == '-') count ++;
        if (count == 4) break;

        font_name ++;
    }

    if (*font_name != '-') return NULL;
    font_name ++;

    return GetCharsetOps (font_name);
}

BOOL InitIncoreVBFonts (void)
{
    int i;
    int len_header, len_offsets, len_widths, len_bits, len_bboxs ;
    int font_size;
    int buf_int[5];
    char version[LEN_VERSION_INFO + 1];

    VBFINFO* info;
    DEVFONT* file_vbf_dev_fonts;
    char * file_font, * file_font_tmp;

    info = (VBFINFO *)&vbf_Courier8x13;
    file_font = (char *)vbf_Courier8x13_font;
    file_font_tmp = file_font;
    len_bboxs = 0;
   
    strncpy(version,file_font_tmp,LEN_VERSION_INFO);
    version [LEN_VERSION_INFO] = '\0'; 

    if (!strcmp (version, VBF_VERSION2))
        info->version = 2;
    else
    {
        info->version = 1;
        if (strcmp (version, VBF_VERSION))
            uHALr_printf("Error on loading vbf, version: %s, invalid version.\n", version);
    }

    file_font_tmp += LEN_VERSION_INFO;
    len_header = *(short *)file_font_tmp;           
    file_font_tmp += 4;
    info->max_width = *file_font_tmp;
    file_font_tmp++;
    info->ave_width = *file_font_tmp;
    file_font_tmp++;
    info->height = *(int *)file_font_tmp;
    file_font_tmp += 4;
    info->descent = *(int *)file_font_tmp;
    file_font_tmp += 4;
    info->first_char = *file_font_tmp;
    file_font_tmp++;
    info->last_char = *file_font_tmp;
    file_font_tmp++;
    info->def_char = *file_font_tmp;
    file_font_tmp++;

    file_font_tmp = file_font + len_header - (((info->version == 2)?5:4)*sizeof (int));
    /* Recopie pour cadrage sur int */
    for(i=0;i<(5*sizeof(int));i++)
       *(((char *)buf_int)+ i) = *(file_font_tmp + i);
       
    len_offsets = buf_int[0];
    len_widths = buf_int[1];
    if(info->version == 2)
       {
       len_bboxs = buf_int[2];
       len_bits = buf_int[3];
       font_size = buf_int[4];
       }
       else
       {
       len_bits = buf_int[2];
       font_size = buf_int[3];
       }

    font_size -= len_header;
    file_font_tmp = file_font + len_header;
    info->name = file_font_tmp;
    info->offset = (unsigned short*) (file_font_tmp + LEN_DEVFONT_NAME + 1);
    info->width = (unsigned char*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets);
    if (info->version == 2)
    {
        info->bbox_x = (int*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
        info->bbox_y = (int*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + len_bboxs);
        info->bbox_w = (int*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 2*len_bboxs);
        info->bbox_h = (int*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 3*len_bboxs);
        info->bits = (unsigned char*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 4*len_bboxs);
    }
    else
        info->bits = (unsigned char*) (file_font_tmp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
    info->font_size = font_size;

    uHALr_printf ("VBF: %s-%dx%d-%d (%d~%d:%d).\n", 
            info->name, info->max_width, info->height, info->descent,
            info->first_char, info->last_char, info->def_char);

    if ((incore_vbf_dev_font = malloc (NR_VBFONTS * sizeof (DEVFONT))) == NULL)
        return FALSE;
    memset(incore_vbf_dev_font,0,NR_VBFONTS * sizeof (DEVFONT));

    for (i = 0; i < NR_VBFONTS; i++) {
        if ((incore_vbf_dev_font [i].charset_ops 
                = vbfGetCharsetOps (incore_vbfonts [i])) == NULL) {
            uHALr_printf ("GDI: Not supported charset for var-bitmap font %s.\n",
                incore_vbfonts[i]->name);
            free (incore_vbf_dev_font);
            return FALSE;
        }

        strncpy (incore_vbf_dev_font [i].name, incore_vbfonts [i]->name, LEN_DEVFONT_NAME);
        incore_vbf_dev_font [i].name [LEN_DEVFONT_NAME] = '\0';
        incore_vbf_dev_font [i].font_ops = &var_bitmap_font_ops;
        incore_vbf_dev_font [i].data     = incore_vbfonts [i];
    }

    for (i = 0; i < NR_VBFONTS; i++)
        AddSBDevFont (incore_vbf_dev_font + i);

    dumpDevFonts ();
    
    return TRUE;
}

void TermIncoreVBFonts (void)
{
    free (incore_vbf_dev_font);
    incore_vbf_dev_font = NULL;
}

#ifndef _INCORE_RES

/********************** Load/Unload of var bitmap font ***********************/
static BOOL LoadVarBitmapFont (const char* file, VBFINFO* info)
{
#ifdef HAVE_MMAP
    int fd;
#else
    FILE* fp;
#endif
    char* temp = NULL;
    int len_header, len_offsets, len_widths, len_bits, len_bboxs = 0;
    int font_size;
    char version[LEN_VERSION_INFO + 1];

#ifdef HAVE_MMAP
    if ((fd = open (file, O_RDONLY)) < 0)
        return FALSE;

    if (read (fd, version, LEN_VERSION_INFO) == -1)
        goto error;
    version[LEN_VERSION_INFO] = '\0';

    if (!strcmp (version, VBF_VERSION2))
        info->version = 2;
    else
    {
        info->version = 1;
        if (strcmp (version, VBF_VERSION))
            uHALr_printf( "Error on loading vbf: %s, version: %s, invalid version.\n", file, version);
    }

    if (read (fd, &len_header, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_header = ArchSwap32 (len_header);
#endif

    if (read (fd, &info->max_width, sizeof (char) * 2) == -1) goto error;
    if (read (fd, &info->height, sizeof (int) * 2) == -1) goto error;

#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif

    if (read (fd, &info->first_char, sizeof (char) * 3) == -1) goto error;

    if (lseek (fd, len_header - (((info->version == 2)?5:4) * sizeof (int)), SEEK_SET) == -1)
        goto error;

    if (read (fd, &len_offsets, sizeof (int)) == -1
            || read (fd, &len_widths, sizeof (int)) == -1
            || (info->version == 2 && read (fd, &len_bboxs, sizeof (int)) == -1)
            || read (fd, &len_bits, sizeof (int)) == -1
            || read (fd, &font_size, sizeof (int)) == -1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    len_bits = ArchSwap32 (len_bits);
    font_size = ArchSwap32 (font_size);
#endif

    if ((temp = mmap (NULL, font_size, PROT_READ, MAP_SHARED, 
            fd, 0)) == MAP_FAILED)
        goto error;

    temp += len_header;
    close (fd);
#else
    // Open font file and read information of font.
    if (!(fp = fopen (file, "rb")))
        return FALSE;

    if (fread (version, sizeof (char), LEN_VERSION_INFO, fp) < LEN_VERSION_INFO)
        goto error;
    version [LEN_VERSION_INFO] = '\0'; 

    if (!strcmp (version, VBF_VERSION2))
        info->version = 2;
    else
    {
        info->version = 1;
        if (strcmp (version, VBF_VERSION))
            uHALr_printf( "Error on loading vbf: %s, version: %s, invalid version.\n", file, version);
    }

    if (fread (&len_header, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_header = ArchSwap32 (len_header);
#endif

    if (fread (&info->max_width, sizeof (char), 2, fp) < 2) goto error;
    if (fread (&info->height, sizeof (int), 2, fp) < 2) goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    info->height = ArchSwap32 (info->height);
    info->descent = ArchSwap32 (info->descent);
#endif
    if (fread (&info->first_char, sizeof (char), 3, fp) < 3) goto error;

    if (fseek (fp, len_header - (((info->version == 2)?5:4)*sizeof (int)), SEEK_SET) != 0)
            goto error;

    if (fread (&len_offsets, sizeof (int), 1, fp) < 1
            || fread (&len_widths, sizeof (int), 1, fp) < 1
            || (info->version == 2 && fread (&len_bboxs, sizeof (int), 1, fp) < 1)
            || fread (&len_bits, sizeof (int), 1, fp) < 1
            || fread (&font_size, sizeof (int), 1, fp) < 1)
        goto error;
#if MGUI_BYTEORDER == MGUI_BIG_ENDIAN
    len_offsets = ArchSwap32 (len_offsets);
    len_widths = ArchSwap32 (len_widths);
    if (info->version == 2)
        len_bboxs = ArchSwap32 (len_bboxs);
    len_bits = ArchSwap32 (len_bits);
    font_size = ArchSwap32 (font_size);
#endif

    // Allocate memory for font data.
    font_size -= len_header;
    if ((temp = (char *)malloc (font_size)) == NULL)
        goto error;

    if (fseek (fp, len_header, SEEK_SET) != 0)
        goto error;

    if (fread (temp, sizeof (char), font_size, fp) < font_size)
        goto error;

    fclose (fp);
#endif
    info->name = temp;
    info->offset = (unsigned short*) (temp + LEN_DEVFONT_NAME + 1);
    info->width = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets);
    if (info->version == 2)
    {
        info->bbox_x = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
        info->bbox_y = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + len_bboxs);
        info->bbox_w = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 2*len_bboxs);
        info->bbox_h = (int*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 3*len_bboxs);
        info->bits = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths + 4*len_bboxs);
    }
    else
        info->bits = (unsigned char*) (temp + LEN_DEVFONT_NAME + 1 + len_offsets + len_widths);
    info->font_size = font_size;

#if 0
    uHALr_printf( "VBF: %s-%dx%d-%d (%d~%d:%d).\n", 
            info->name, info->max_width, info->height, info->descent,
            info->first_char, info->last_char, info->def_char);
#endif

    return TRUE;

error:
#ifdef HAVE_MMAP
    if (temp)
        munmap (temp, font_size);
    close (fd);
#else
    free (temp);
    fclose (fp);
#endif
    
    return FALSE;
}

static void UnloadVarBitmapFont (VBFINFO* info)
{
#ifdef HAVE_MMAP
    if (info->name)
        munmap ((void*)(info->name), info->font_size);
#else
    free ((void*)info->name);
#endif
}

/******************** Init/Term of var bitmap font in file *******************/
static int nr_fonts;
static VBFINFO* file_vbf_infos;
static DEVFONT* file_vbf_dev_fonts;

#define SECTION_NAME    "varbitmapfonts"

BOOL InitVarBitmapFonts (void)
{
    int i;
    char font_name [LEN_DEVFONT_NAME + 1];

    if (GetMgEtcIntValue (SECTION_NAME, "font_number", 
                           &nr_fonts) < 0 )
        return FALSE;
    if ( nr_fonts < 1) return TRUE;

    file_vbf_infos = malloc (nr_fonts * sizeof (VBFINFO));
    file_vbf_dev_fonts = malloc (nr_fonts * sizeof (DEVFONT));
    if (file_vbf_infos == NULL || file_vbf_dev_fonts == NULL) {
        free (file_vbf_infos);
        free (file_vbf_dev_fonts);
        return FALSE;
    }
    
    memset(file_vbf_infos,0,nr_fonts * sizeof (VBFINFO));
    memset(file_vbf_dev_fonts,0,nr_fonts * sizeof (DEVFONT));

    for (i = 0; i < nr_fonts; i++)
        file_vbf_infos [i].name = NULL;

    for (i = 0; i < nr_fonts; i++) {
        char key [11];
        char charset [LEN_FONT_NAME + 1];
        char file [MAX_PATH + 1];
        CHARSETOPS* charset_ops;

        sprintf (key, "name%d", i);
        if (GetMgEtcValue (SECTION_NAME, key, 
                           font_name, LEN_DEVFONT_NAME) < 0 )
            goto error_load;
        if (!fontGetCharsetFromName (font_name, charset)) {
            uHALr_printf( "GDI: Invalid font name (charset): %s.\n", 
                    font_name);
            goto error_load;
        }

        if ((charset_ops = GetCharsetOps (charset)) == NULL) {
            uHALr_printf( "GDI: Not supported charset: %s.\n", charset);
            goto error_load;
        }

        sprintf (key, "fontfile%d", i);
        if (GetMgEtcValue (SECTION_NAME, key, 
                           file, MAX_PATH) < 0)
            goto error_load;

        if (!LoadVarBitmapFont (file, file_vbf_infos + i))
            goto error_load;

        strncpy (file_vbf_dev_fonts[i].name, font_name, LEN_DEVFONT_NAME);
        file_vbf_dev_fonts[i].name [LEN_DEVFONT_NAME] = '\0';
        file_vbf_dev_fonts[i].font_ops = &var_bitmap_font_ops;
        file_vbf_dev_fonts[i].charset_ops = charset_ops;
        file_vbf_dev_fonts[i].data = file_vbf_infos + i;
#if 0
        uHALr_printf( "GDI: VBFDevFont %i: %s.\n", i, file_vbf_dev_fonts[i].name);
#endif
    }

    for (i = 0; i < nr_fonts; i++) {
        if (file_vbf_dev_fonts [i].charset_ops->bytes_maxlen_char > 1)
            AddMBDevFont (file_vbf_dev_fonts + i);
        else
            AddSBDevFont (file_vbf_dev_fonts + i);
    }

    return TRUE;

error_load:
    uHALr_printf( "GDI: Error in loading vbf fonts!\n");
    for (i = 0; i < nr_fonts; i++)
        UnloadVarBitmapFont (file_vbf_infos + i);
    
    free (file_vbf_infos);
    free (file_vbf_dev_fonts);
    file_vbf_infos = NULL;
    file_vbf_dev_fonts = NULL;
    return FALSE;
}

void TermVarBitmapFonts (void)
{
    int i;

    for (i = 0; i < nr_fonts; i++)
        UnloadVarBitmapFont (file_vbf_infos + i);
    
    free (file_vbf_infos);
    free (file_vbf_dev_fonts);

    file_vbf_infos = NULL;
    file_vbf_dev_fonts = NULL;
}

#endif /* _INCORE_RES */

#endif /* _VBF_SUPPORT */

