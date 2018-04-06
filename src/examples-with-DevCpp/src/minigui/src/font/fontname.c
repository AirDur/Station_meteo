/*
** $Id: fontname.c,v 1.14 2005/02/03 05:00:44 snig Exp $
** 
** fontname.c: Font name parser.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
** 
** Created by Wei Yongming, 2000/07/11
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
#include "fontname.h"

/* 
 * Font name format:
 * type-family-style-width-height-charset-encoding1[,encoding2,...]
 */

#define NR_LOOP_FOR_STYLE   2
#define NR_LOOP_FOR_WIDTH   3
#define NR_LOOP_FOR_HEIGHT  4
#define NR_LOOP_FOR_CHARSET 5

int fontConvertFontType (const char* type)
{
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_RAW) == 0)
        return FONT_TYPE_BITMAP_RAW;
    if (strcasecmp (type, FONT_TYPE_NAME_BITMAP_VAR) == 0)
        return FONT_TYPE_BITMAP_VAR;
    if (strcasecmp (type, FONT_TYPE_NAME_SCALE_TTF) == 0)
        return FONT_TYPE_SCALE_TTF;
    if (strcasecmp (type, FONT_TYPE_NAME_SCALE_T1F) == 0)
        return FONT_TYPE_SCALE_T1F;
    if (strcasecmp (type, FONT_TYPE_NAME_ALL) == 0)
        return FONT_TYPE_ALL;

    return -1;
}

BOOL fontGetTypeNameFromName (const char* name, char* type)
{
    int i = 0;
    while (name [i]) {
        if (name [i] == '-') {
            type [i] = '\0';
            break;
        }

        type [i] = name [i];
        i++;
    }

    if (name [i] == '\0')
        return FALSE;

    return TRUE;
}

int fontGetFontTypeFromName (const char* name)
{
    char type [LEN_FONT_NAME + 1];

    if (!fontGetTypeNameFromName (name, type))
        return -1;

    return fontConvertFontType (type);
}

BOOL fontGetFamilyFromName (const char* name, char* family)
{
    int i = 0;
    const char* family_part;

    if ((family_part = strchr (name, '-')) == NULL)
        return FALSE;
    if (*(++family_part) == '\0')
        return FALSE;

    while (family_part [i] && i <= LEN_FONT_NAME) {
        if (family_part [i] == '-') {
            family [i] = '\0';
            break;
        }

        family [i] = family_part [i];
        i++;
    }

    return TRUE;
}

DWORD fontConvertStyle (const char* style_part)
{
    DWORD style = 0;

    switch (style_part [0]) {
    case FONT_WEIGHT_BLACK:
        style |= FS_WEIGHT_BLACK;
        break;
    case FONT_WEIGHT_BOLD:
        style |= FS_WEIGHT_BOLD;
        break;
    case FONT_WEIGHT_BOOK:
        style |= FS_WEIGHT_BOOK;
        break;
    case FONT_WEIGHT_DEMIBOLD:
        style |= FS_WEIGHT_DEMIBOLD;
        break;
    case FONT_WEIGHT_LIGHT:
        style |= FS_WEIGHT_LIGHT;
        break;
    case FONT_WEIGHT_MEDIUM:
        style |= FS_WEIGHT_MEDIUM;
        break;
    case FONT_WEIGHT_REGULAR:
        style |= FS_WEIGHT_REGULAR;
        break;
    case FONT_WEIGHT_ALL:
        style |= FS_WEIGHT_MASK;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [1]) {
    case FONT_SLANT_ITALIC:
        style |= FS_SLANT_ITALIC;
        break;
    case FONT_SLANT_OBLIQUE:
        style |= FS_SLANT_OBLIQUE;
        break;
    case FONT_SLANT_ROMAN:
        style |= FS_SLANT_ROMAN;
        break;
    case FONT_SLANT_ALL:
        style |= FS_SLANT_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [2]) {
    case FONT_SETWIDTH_BOLD:
        style |= FS_SETWIDTH_BOLD;
        break;
    case FONT_SETWIDTH_CONDENSED:
        style |= FS_SETWIDTH_CONDENSED;
        break;
    case FONT_SETWIDTH_SEMICONDENSED:
        style |= FS_SETWIDTH_SEMICONDENSED;
        break;
    case FONT_SETWIDTH_NORMAL:
        style |= FS_SETWIDTH_NORMAL;
        break;
    case FONT_SETWIDTH_ALL:
        style |= FS_SETWIDTH_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [3]) {
    case FONT_SPACING_MONOSPACING:
        style |= FS_SPACING_MONOSPACING;
        break;
    case FONT_SPACING_PROPORTIONAL:
        style |= FS_SPACING_PROPORTIONAL;
        break;
    case FONT_SPACING_CHARCELL:
        style |= FS_SPACING_CHARCELL;
        break;
    case FONT_SPACING_ALL:
        style |= FS_SPACING_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [4]) {
    case FONT_UNDERLINE_LINE:
        style |= FS_UNDERLINE_LINE;
        break;
    case FONT_UNDERLINE_ALL:
        style |= FS_UNDERLINE_MASK;
        break;
    case FONT_UNDERLINE_NONE:
        style &= ~FS_UNDERLINE_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    switch (style_part [5]) {
    case FONT_STRUCKOUT_LINE:
        style |= FS_STRUCKOUT_LINE;
        break;
    case FONT_STRUCKOUT_ALL:
        style |= FS_STRUCKOUT_MASK;
        break;
    case FONT_STRUCKOUT_NONE:
        style &= ~FS_STRUCKOUT_MASK;
        break;
    default:
        return 0xFFFFFFFF;
    }

    return style;
}

DWORD fontGetStyleFromName (const char* name)
{
    int i;
    const char* style_part = name;
    char style_name[7];

    for (i = 0; i < NR_LOOP_FOR_STYLE; i++) {
        if ((style_part = strchr (style_part, '-')) == NULL)
            return 0xFFFFFFFF;

        if (*(++style_part) == '\0')
            return 0xFFFFFFFF;
    }

    strncpy (style_name, style_part, 6);
    style_name[6] = '\0';

    return fontConvertStyle (style_name);
}

int fontGetWidthFromName (const char* name)
{
    int i;
    const char* width_part = name;
    char width [LEN_FONT_NAME + 1];

    for (i = 0; i < NR_LOOP_FOR_WIDTH; i++) {
        if ((width_part = strchr (width_part, '-')) == NULL)
            return -1;

        if (*(++width_part) == '\0')
            return -1;
    }

    i = 0;
    while (width_part [i]) {
        if (width_part [i] == '-') {
            width [i] = '\0';
            break;
        }

        width [i] = width_part [i];
        i++;
    }

    if (width_part [i] == '\0')
        return -1;

    return atoi (width);
}

int fontGetHeightFromName (const char* name)
{
    int i;
    const char* height_part = name;
    char height [LEN_FONT_NAME + 1];

    for (i = 0; i < NR_LOOP_FOR_HEIGHT; i++) {
        if ((height_part = strchr (height_part, '-')) == NULL)
            return -1;
        if (*(++height_part) == '\0')
            return -1;
    }

    i = 0;
    while (height_part [i]) {
        if (height_part [i] == '-') {
            height [i] = '\0';
            break;
        }

        height [i] = height_part [i];
        i++;
    }

    if (height_part [i] == '\0')
        return -1;

    return atoi (height);
}

BOOL fontGetCharsetFromName (const char* name, char* charset)
{
    int i;
    char* delim;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
#if 0
        uHALr_printf( "%s\n", charset_part);
#endif
    }

    if ((delim = strchr (charset_part, ','))) {
        int len;
        len = delim - charset_part;
        strncpy (charset, charset_part, len);
        charset [len] = '\0';
        return TRUE;
    }

    strncpy (charset, charset_part, LEN_FONT_NAME);
    charset [LEN_FONT_NAME] = '\0';
    return TRUE;
}

BOOL fontGetCompatibleCharsetFromName (const char* name, char* charset)
{
    int i;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
#if 0
        uHALr_printf( "%s\n", charset_part);
#endif
    }

    if ((charset_part = strchr (charset_part, ',')) == NULL)
        return FALSE;

    if (*(++charset_part) == '\0')
        return FALSE;

    strncpy (charset, charset_part, LEN_FONT_NAME);
    charset [LEN_FONT_NAME] = '\0';
    return TRUE;
}

BOOL fontGetCharsetPartFromName (const char* name, char* charset)
{
    int i;
    const char* charset_part = name;

    for (i = 0; i < NR_LOOP_FOR_CHARSET; i++) {
        if ((charset_part = strchr (charset_part, '-')) == NULL)
            return FALSE;
        if (*(++charset_part) == '\0')
            return FALSE;
    }

    strncpy (charset, charset_part, LEN_DEVFONT_NAME);
    charset [LEN_DEVFONT_NAME] = '\0';
    return TRUE;
}

int charsetGetCharsetsNumber (const char* charsets)
{
    int n = 1;

    while (1) {
        if ((charsets = strchr (charsets, ',')) == NULL)
            break;

        charsets ++;
        n ++;
    }

    return n;
}

BOOL charsetGetSpecificCharset (const char* charsets, int index, char* charset)
{
    int i;
    char* delim;

    for (i = 0; i < index && charsets; i++) {
        charsets = strchr (charsets, ',');
        if (charsets)
            charsets ++;
    }

    if (charsets == NULL)
        return FALSE;

    if ((delim = strchr (charsets, ','))) {
        int len;
        len = delim - charsets;
        strncpy (charset, charsets, len);
        charset [len] = '\0';
        return TRUE;
    }

    strncpy (charset, charsets, LEN_FONT_NAME);
    charset [LEN_FONT_NAME] = '\0';
    return TRUE;
}

