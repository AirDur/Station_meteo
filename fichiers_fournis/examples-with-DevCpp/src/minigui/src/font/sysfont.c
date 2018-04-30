/*
** $Id: sysfont.c,v 1.39 2004/04/01 08:57:13 weiym Exp $
** 
** sysfont.c: Load, create and handle system font and system charset.
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/01/03
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

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "misc.h"

#include "sysfont.h"
#include "rawbitmap.h"
#include "charset.h"
#include "devfont.h"
#include "fontname.h"

/**************************** Global data ************************************/
PLOGFONT g_SysLogFont [NR_SYSLOGFONTS];

/**************************** Static data ************************************/

static char* sys_font_name [] =
{
    "default",
    "wchar_def",
    "fixed",
    "caption",
    "menu",
    "control"
};

BOOL InitSysFont (void)
{
    int i;
    PLOGFONT* sys_fonts;
    int nr_fonts;
   
    nr_fonts = 1;
    if (nr_fonts < 1) return TRUE;
    if (nr_fonts > NR_SYSLOGFONTS) nr_fonts = NR_SYSLOGFONTS;

#ifdef HAVE_ALLOCA
    if ((sys_fonts = alloca (nr_fonts * sizeof (PLOGFONT))) == NULL)
#else
    if ((sys_fonts = malloc (nr_fonts * sizeof (PLOGFONT))) == NULL)
#endif
        return FALSE;

    memset (sys_fonts, 0, nr_fonts * sizeof (PLOGFONT));

    for (i = 0; i < nr_fonts; i++) {
        char key [11];
        char type[LEN_FONT_NAME + 1];
        char family[LEN_FONT_NAME + 1];
        char charset[LEN_FONT_NAME + 1];
        int height;
        char font_name [LEN_DEVFONT_NAME + 1];

        strcpy(font_name,"vbf-Courier-rrncnn-10-15-ISO8859-1\0");
        if (!fontGetTypeNameFromName (font_name, type) ||
                !fontGetFamilyFromName (font_name, family) ||
                !fontGetCharsetFromName (font_name, charset) ||
                ((height = fontGetHeightFromName (font_name)) == -1)) {

            uHALr_printf ("GDI: Invalid system logical font name: %s.\n", 
                            font_name);
            goto error_load;
        }

        uHALr_printf ("system font %d: %s-%s-%d-%s\n", i, type, family, height, charset);


        if (i == 0 && GetCharsetOps (charset)->bytes_maxlen_char > 1) {
            uHALr_printf ("GDI: First system font should be a single-byte charset. font_name: %s\n",
                            font_name);
            goto error_load;
        }

        if ((sys_fonts[i] = CreateLogFont (type, family, charset, 
                FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
                FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL,
                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                height, 0)) == NULL) {
            uHALr_printf ("GDI: Error when creating system logical font.\n");
            goto error_load;
        }

        if (i == 0)
            g_SysLogFont [0] = sys_fonts [0];
    }

    for (i = 0; i < NR_SYSLOGFONTS; i++)
        g_SysLogFont [i] = sys_fonts[0];

#ifndef HAVE_ALLOCA
    free (sys_fonts);
#endif
    return TRUE;

error_load:
    uHALr_printf ("GDI: Error in creating system logical fonts!\n");
    for (i = 0; i < nr_fonts; i++) {
        if (sys_fonts [i])
            DestroyLogFont (sys_fonts[i]);
    }

#ifndef HAVE_ALLOCA
    free (sys_fonts);
#endif
    return FALSE;
}

static inline BOOL is_freed_font (int id)
{
    int i;

    for (i = 0; i < id; i++) {
        if (g_SysLogFont[i] == g_SysLogFont[id])
            return TRUE;
    }

    return FALSE;
}

void TermSysFont (void)
{
    int i;

    for (i = 0; i < NR_SYSLOGFONTS; i++) {
        if (g_SysLogFont [i] && !is_freed_font (i))
            DestroyLogFont (g_SysLogFont [i]);
    }
}

/**************************** API: System Font Info *************************/
const char* GUIAPI GetSysCharset (BOOL wchar)
{
    if (wchar) {
        if (g_SysLogFont[1]->mbc_devfont)
            return MBC_DEVFONT_INFO_P (g_SysLogFont[1])->charset_ops->name;
        else
            return NULL;
    }
    else 
        return SBC_DEVFONT_INFO_P (g_SysLogFont[0])->charset_ops->name;
}

int GUIAPI GetSysCharWidth (void)
{
    DEVFONT* sbc_devfont = g_SysLogFont[0]->sbc_devfont;

    return sbc_devfont->font_ops->get_ave_width (g_SysLogFont[0], sbc_devfont);
}

int GUIAPI GetSysCCharWidth (void)
{
    DEVFONT* mbc_devfont = g_SysLogFont[1]->mbc_devfont;

    if (mbc_devfont)
    	return mbc_devfont->font_ops->get_ave_width (g_SysLogFont[1], mbc_devfont);
    else
        return GetSysCharWidth () * 2;
}

int GUIAPI GetSysCharHeight (void)
{
    return g_SysLogFont[0]->size;
}

int GUIAPI GetSysFontMaxWidth (int font_id)
{
    DEVFONT* sbc_devfont = g_SysLogFont[font_id]->sbc_devfont;

    return sbc_devfont->font_ops->get_max_width (g_SysLogFont[font_id], sbc_devfont);
}

int GUIAPI GetSysFontAveWidth (int font_id)
{
    DEVFONT* sbc_devfont = g_SysLogFont[font_id]->sbc_devfont;

    return sbc_devfont->font_ops->get_ave_width (g_SysLogFont[font_id], sbc_devfont);
}

int GUIAPI GetSysFontHeight (int font_id)
{
    return g_SysLogFont[font_id]->size;
}

