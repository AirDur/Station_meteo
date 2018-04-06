/*
** $Id: devfont.c,v 1.25 2004/02/25 02:34:49 weiym Exp $
** 
** defont.c: Device fonts management.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
** 
** Create Date: 2000/07/07
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

#include "sysfont.h"
#include "charset.h"
#include "devfont.h"
#include "fontname.h"
#include "rawbitmap.h"
#include "varbitmap.h"

#ifdef _LITE_VERSION

static int init_count = 0;
BOOL GUIAPI InitVectorialFonts (void)
{
    if (init_count > 0)
       goto success;

#ifdef _TTF_SUPPORT
    if (!InitFreeTypeFonts ()) {
        uHALr_printf( 
            "GDI: Can not initialize TrueType fonts!\n");
        return FALSE;
    }
#endif

#ifdef _TYPE1_SUPPORT
    if (!InitType1Fonts ()) {
        uHALr_printf( 
            "GDI: Can not initialize Type1 fonts!\n");
        goto error;
    }
#endif

success:
    init_count ++;
    return TRUE;

#ifdef _TYPE1_SUPPORT
error:
#endif

#ifdef _TTF_SUPPORT
    TermFreeTypeFonts ();
#endif
    return FALSE;
}

void GUIAPI TermVectorialFonts (void)
{
    init_count --;
    if (init_count != 0)
        return;

#ifdef _TYPE1_SUPPORT
	TermType1Fonts();
#endif

#ifdef _TTF_SUPPORT
    TermFreeTypeFonts ();
#endif
}

#endif /* _LITE_VERSION */

/**************************** Global data ************************************/
static DEVFONT* sb_dev_font_head;
static DEVFONT* mb_dev_font_head;
static int nr_sb_dev_fonts;
static int nr_mb_dev_fonts;

#define devfontAddSBCDevFont(head, new)         \
{                                               \
    if (head == NULL)                           \
        head = new;                             \
    else {                                      \
        DEVFONT* tail;                          \
        tail = head;                            \
        while (tail->sbc_next) tail = tail->sbc_next;   \
        tail->sbc_next = new;                       \
    }                                           \
                                                \
    new->sbc_next = NULL;                           \
}

#define devfontAddMBCDevFont(head, new)         \
{                                               \
    if (head == NULL)                           \
        head = new;                             \
    else {                                      \
        DEVFONT* tail;                          \
        tail = head;                            \
        while (tail->mbc_next) tail = tail->mbc_next;   \
        tail->mbc_next = new;                       \
    }                                           \
                                                \
    new->mbc_next = NULL;                           \
}

void AddSBDevFont (DEVFONT* dev_font)
{
    devfontAddSBCDevFont (sb_dev_font_head, dev_font);
    dev_font->style = fontGetStyleFromName (dev_font->name);
    nr_sb_dev_fonts ++;
}

void AddMBDevFont (DEVFONT* dev_font)
{
    devfontAddMBCDevFont (mb_dev_font_head, dev_font);
    dev_font->style = fontGetStyleFromName (dev_font->name);
    nr_mb_dev_fonts ++;
}

void ResetDevFont (void)
{
    sb_dev_font_head = mb_dev_font_head = NULL;
    nr_sb_dev_fonts = 0;
    nr_mb_dev_fonts = 0;
}

#define MATCHED_TYPE        0x01
#define MATCHED_FAMILY      0x02
#define MATCHED_CHARSET     0x04

DEVFONT* GetMatchedSBDevFont (LOGFONT* log_font)
{
    int i;
    char charset_req [LEN_FONT_NAME + 1];
    int min_error;
    DEVFONT* dev_font;
    DEVFONT* matched_font;

#ifdef HAVE_ALLOCA
    BYTE* match_bits = alloca (nr_sb_dev_fonts);
#else
    BYTE* match_bits = FixStrAlloc (nr_sb_dev_fonts);
#endif

    // is charset requested is a single byte charset?
    if (GetCharsetOps (log_font->charset)->bytes_maxlen_char > 1) {
        fontGetCharsetFromName (g_SysLogFont[0]->sbc_devfont->name, charset_req);
    }
    else
        strcpy (charset_req, log_font->charset);
        
    i = 0;
    dev_font = sb_dev_font_head;
    while (dev_font) {
        int type_req;
        char family [LEN_FONT_NAME + 1];
        
        // clear match_bits first.
        match_bits [i] = 0;

        // does match this font type?
        type_req = fontConvertFontType (log_font->type);
        if (type_req == FONT_TYPE_ALL)
            match_bits [i] |= MATCHED_TYPE;
        else if (type_req == fontGetFontTypeFromName (dev_font->name))
            match_bits [i] |= MATCHED_TYPE;

        // does match this family?
        fontGetFamilyFromName (dev_font->name, family);
        if (strcasecmp (family, log_font->family) == 0) {
#if 0
            uHALr_printf( "family: %s, requested: %s; ", family, log_font->family);
#endif
            match_bits [i] |= MATCHED_FAMILY;
        }

        // does match this charset 
        if (dev_font->charset_ops == GetCharsetOps (charset_req)) {
#if 0
            uHALr_printf( "charset: %s, requested: %s.\n", dev_font->charset_ops->name, charset_req);
#endif
            match_bits [i] |= MATCHED_CHARSET;
        }
#if 0
        else {
            if (fontGetCompatibleCharsetFromName (dev_font->name, charset)
                    && GetCharsetOps (charset) == GetCharsetOps (charset_req))
                match_bits [i] |= MATCHED_CHARSET;
        }
#endif

        // FIXME: ignore style

        dev_font = dev_font->sbc_next;
        i ++;
    }

    min_error = FONT_MAX_SIZE;
    matched_font = NULL;
    dev_font = sb_dev_font_head;
    for (i = 0; i < nr_sb_dev_fonts; i++) {
        int error;
        if ((match_bits [i] & MATCHED_TYPE)
                && (match_bits [i] & MATCHED_FAMILY)
                && (match_bits [i] & MATCHED_CHARSET)) {
            error = abs (log_font->size - 
                    (*dev_font->font_ops->get_font_size) (log_font, dev_font, 
                                                          log_font->size));
            if (min_error > error) {
                min_error = error;
                matched_font = dev_font;
            }
        }
        dev_font = dev_font->sbc_next;
    }

    if (matched_font) {
#if 0
        uHALr_printf( "matched font: %s\n", matched_font->name);
#endif
#ifndef HAVE_ALLOCA
        FreeFixStr (match_bits);
#endif
        return matched_font;
    }

    // check charset here.
    min_error = FONT_MAX_SIZE;
    dev_font = sb_dev_font_head;
    for (i = 0; i < nr_sb_dev_fonts; i++) {
        int error;
        if (match_bits [i] & MATCHED_CHARSET) {
            error = abs (log_font->size - 
                    (*dev_font->font_ops->get_font_size) (log_font, dev_font, log_font->size));
            if (min_error > error) {
                min_error = error;
                matched_font = dev_font;
            }
        }
        dev_font = dev_font->sbc_next;
    }

#if 0
        uHALr_printf( "select a font: %s\n", matched_font->name);
#endif
#ifndef HAVE_ALLOCA
    FreeFixStr (match_bits);
#endif
    return matched_font;
}

DEVFONT* GetMatchedMBDevFont (LOGFONT* log_font)
{
    DEVFONT* dev_font;
    int i;
    BYTE* match_bits;
    char charset_req [LEN_FONT_NAME + 1];
    int min_error;
    DEVFONT* matched_font;

    // is charset requested is a multiple-byte charset?
    if (GetCharsetOps (log_font->charset)->bytes_maxlen_char < 2)
        return NULL;

    match_bits = FixStrAlloc (nr_mb_dev_fonts);

    strcpy (charset_req, log_font->charset);
        
    i = 0;
    dev_font = mb_dev_font_head;
    while (dev_font) {
        int type_req;
        char family [LEN_FONT_NAME + 1];
        
        // clear match_bits first.
        match_bits [i] = 0;

        // does match this font type?
        type_req = fontConvertFontType (log_font->type);
        if (type_req == FONT_TYPE_ALL)
            match_bits [i] |= MATCHED_TYPE;
        else if (type_req == fontGetFontTypeFromName (dev_font->name))
            match_bits [i] |= MATCHED_TYPE;

        // does match this family?
        fontGetFamilyFromName (dev_font->name, family);
        if (strcasecmp (family, log_font->family) == 0) {
#if 0
            uHALr_printf( "family: %s, requested: %s; ", family, log_font->family);
#endif
            match_bits [i] |= MATCHED_FAMILY;
        }

        // does match this charset 
        if (dev_font->charset_ops == GetCharsetOps (charset_req)) {
#if 0
            uHALr_printf( "charset: %s, requested: %s\n", dev_font->charset_ops->name, charset_req);
#endif
            match_bits [i] |= MATCHED_CHARSET;
        }

        // FIXME: ignore style

        dev_font = dev_font->mbc_next;
        i ++;
    }

    matched_font = NULL;
    min_error = FONT_MAX_SIZE;
    dev_font = mb_dev_font_head;
    for (i = 0; i < nr_mb_dev_fonts; i++) {
        int error;
        if ((match_bits [i] & MATCHED_TYPE)
                && (match_bits [i] & MATCHED_FAMILY)
                && (match_bits [i] & MATCHED_CHARSET)) {
            error = abs (log_font->size - 
                    (*dev_font->font_ops->get_font_size) (log_font, dev_font, 
                                                          log_font->size));
            if (min_error > error) {
                min_error = error;
                matched_font = dev_font;
            }
        }
        dev_font = dev_font->mbc_next;
    }

    if (matched_font) {
#if 0
        uHALr_printf( "matched font: %s\n", matched_font->name);
#endif
        FreeFixStr (match_bits);
        return matched_font;
    }

    min_error = FONT_MAX_SIZE;
    dev_font = mb_dev_font_head;
    for (i = 0; i < nr_mb_dev_fonts; i++) {
        int error;
        if (match_bits [i] & MATCHED_CHARSET) {
            error = abs (log_font->size - 
                    (*dev_font->font_ops->get_font_size) (log_font, dev_font, 
                                                          log_font->size));
            if (min_error > error) {
                min_error = error;
                matched_font = dev_font;
            }
        }
        dev_font = dev_font->mbc_next;
    }

#if 0
        uHALr_printf( "select a font: %s\n", matched_font->name);
#endif
    FreeFixStr (match_bits);
    return matched_font;
}

const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font)
{
    if (dev_font == NULL) {
        return sb_dev_font_head;
    }
    else if (dev_font->charset_ops->bytes_per_char == 1) {
        dev_font = dev_font->sbc_next;
        if (dev_font == NULL)
            return mb_dev_font_head;

        return dev_font;
    }

    return dev_font->mbc_next;
}

void dumpDevFonts (void)
{
    int count = 0;
    DEVFONT* devfont;

    uHALr_printf ("============= SBDevFonts ============\n");
    devfont = sb_dev_font_head;
    while (devfont) {
        uHALr_printf ("  %d: %s, charsetname: %s, style: %#lx\n", 
                        count, 
                        devfont->name, devfont->charset_ops->name, devfont->style);
        devfont = devfont->sbc_next;
        count++;
    }
    uHALr_printf ("========== End of SBDevFonts =========\n");

    uHALr_printf ("\n============= MBDevFonts ============\n");
    devfont = mb_dev_font_head;
    while (devfont) {
        uHALr_printf ("  %d: %s, charsetname: %s, style: %#lx\n", 
                        count, 
                        devfont->name, devfont->charset_ops->name, devfont->style);
        devfont = devfont->mbc_next;
        count++;
    }
    uHALr_printf (stderr, "========== End of MBDevFonts =========\n");
}

