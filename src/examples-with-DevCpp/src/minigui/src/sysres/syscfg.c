/*
** $Id: syscfg.c,v 1.8 2004/06/26 07:49:32 weiym Exp $
**
** sysfg.c: This file include some functions for getting system configuration value. 
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2003/09/06
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
#include "minigui.h"
#include "gdi.h"
#include "window.h"

#include "misc.h"
#include "cursor.h"
#include "icon.h"
#include "event.h"
#include "sysres.h"

gal_pixel WinElementColors [WEC_ITEM_NUMBER];

#ifndef _INCORE_RES

int WinMainMetrics [MWM_ITEM_NUMBER];

static const char* szMWMKeyNames [MWM_ITEM_NUMBER] = {
        "minwidth",
        "minheight",
        "border",
        "thickframe",
        "thinframe",
        "captiony",
        "iconx",
        "icony",
        "menubary",
        "menubaroffx",
        "menubaroffy",
        "menuitemy",
        "intermenuitemx",
        "intermenuitemy",
        "menuitemoffx",
        "menutopmargin",
        "menubottommargin",
        "menuleftmargin",
        "menurightmargin",
        "menuitemminx",
        "menuseparatory",
        "menuseparatorx",
        "sb_height",
        "sb_width",
        "sb_interx",
        "cxvscroll",
        "cyvscroll",
        "cxhscroll",
        "cyhscroll",
        "minbarlen",
        "defbarlen"
};

BOOL InitMainWinMetrics (void)
{
    int i, tmp, height_sysfont;
    char buff [21];

    height_sysfont = GetSysCharHeight ();

    for (i = 0; i < MWM_ITEM_NUMBER; i++) {
        if (GetMgEtcValue ("mainwinmetrics", szMWMKeyNames [i], 
                buff, 20) != ETC_OK)
            return FALSE;

        if (i < 8)
            height_sysfont = GetSystemFont (SYSLOGFONT_CAPTION)->size;
        else if (i < 22)
            height_sysfont = GetSystemFont (SYSLOGFONT_MENU)->size;

        tmp = atoi (buff + 1);
        switch (buff [0]) {
        case '+':
            WinMainMetrics [i] = height_sysfont + tmp;
            break;
        case '-':
            WinMainMetrics [i] = height_sysfont - tmp;
            break;
        case '*':
            WinMainMetrics [i] = height_sysfont * tmp;
            break;
        case '/':
            WinMainMetrics [i] = height_sysfont / tmp;
            break;
        case '%':
            WinMainMetrics [i] = height_sysfont % tmp;
            break;
        default:
            WinMainMetrics [i] = atoi (buff);
            break;
        }
    }

    return TRUE;
}

static const char* szWECKeyNames [WEC_ITEM_NUMBER] =
{
        "bkc_caption_normal",
        "fgc_caption_normal",
        "bkc_caption_actived",
        "fgc_caption_actived",
        "bkc_caption_disabled",
        "fgc_caption_disabled",
        "wec_frame_normal",
        "wec_frame_actived",
        "wec_frame_disabled",
        "bkc_menubar_normal",
        "fgc_menubar_normal",
        "bkc_menubar_hilite",
        "fgc_menubar_hilite",
        "fgc_menubar_disabled",
        "bkc_menuitem_normal",
        "fgc_menuitem_normal",
        "bkc_menuitem_hilite",
        "fgc_menuitem_hilite",
        "fgc_menuitem_disabled",
        "bkc_pppmenutitle",
        "fgc_pppmenutitle",
        "fgc_menuitem_frame",
        "wec_3dbox_normal",
        "wec_3dbox_reverse",
        "wec_3dbox_light",
        "wec_3dbox_dark",
        "wec_flat_border",
        "bkc_control_def",
        "fgc_control_normal",
        "fgc_control_disabled",
        "bkc_hilight_normal",
        "bkc_hilight_lostfocus",
        "fgc_hilight_normal",
        "fgc_hilight_disabled",
        "bkc_desktop",
        "bkc_dialog",
        "bkc_tip",
};

BOOL InitWindowElementColors (void)
{
    int i;
    unsigned long int rgb;

    for (i = 0; i < WEC_ITEM_NUMBER; i++) {

        char buff [20];

        if (GetMgEtcValue ("windowelementcolors", szWECKeyNames [i], 
                buff, 12) != ETC_OK)
            return FALSE;

        if ((rgb = strtoul (buff, NULL, 0)) > 0x00FFFFFF)
            return FALSE;
        WinElementColors [i] = RGB2Pixel (HDC_SCREEN, 
                GetRValue (rgb), GetGValue(rgb), GetBValue (rgb));
    }

    return TRUE;
}
#else /* _INCORE_RES */

#ifdef _FLAT_WINDOW_STYLE

int WinMainMetrics [MWM_ITEM_NUMBER] =
{
    50, 50,
    1, 2, 1,
    17, 16, 16,
    12, 8, 5, 12,
    12, 2, 18, 4, 4,
    4, 4, 64, 4, 4, 14,
    16, 2, 9, 9, 9, 9, 8,
    18
};

static const unsigned long __mg_wec [WEC_ITEM_NUMBER] =
{
    0x00808080, 0x00C0C0C0, 0x00CE2418,
    0x00FFFFFF, 0x00808080, 0x00C0C0C0,
    0x00000000, 0x00FFFFFF, 0x00000000,
    0x00FFFFFF, 0x00000000, 0x00000000,
    0x00FFFFFF, 0x00808080, 0x00FFFFFF,
    0x00000000, 0x00EFD3C6, 0x00000000,
    0x00808080, 0x00C0C0C0, 0x00CE2418,
    0x00C66931, 0x00FFFFFF, 0x00000000,
    0x00FFFFFF, 0x00000000, 0x00000000,
    0x00C0C0C0, 0x00000000, 0x00FFFFFF,
    0x00FF0000, 0x00BDA69C, 0x00FFFFFF,
    0x00C0C0C0, 0x00FF0000, 0x00FFFFFF,
    0x00C8FCF8,
};

#elif defined (_PHONE_WINDOW_STYLE)

int WinMainMetrics [MWM_ITEM_NUMBER] =
{
    50, 50,
    2, 2, 1,
    20, 16, 16,
    16, 8, 5, 16,
    16, 2, 18, 4, 4,
    4, 4, 64, 4, 4, 14,
    16, 2, 16, 16, 16, 16, 8,
    18,
};

static const unsigned long __mg_wec [WEC_ITEM_NUMBER] =
{
    0x00808080, 0x00C0C0C0, 0x00CE2418,
    0x00FFFFFF, 0x00808080, 0x00C0C0C0,
    0x00000000, 0x00FFFFFF, 0x00000000,
    0x00FFFFFF, 0x00000000, 0x00000000,
    0x00FFFFFF, 0x00808080, 0x00FFFFFF,
    0x00000000, 0x00EFD3C6, 0x00000000,
    0x00808080, 0x00C0C0C0, 0x00CE2418,
    0x00C66931, 0x00FFFFFF, 0x00000000,
    0x00FFFFFF, 0x00000000, 0x00000000,
    0x00FEF5E5, 0x00000000, 0x00C0C0C0,
    0x00FF0000, 0x00BDA69C, 0x00FFFFFF,
    0x00C0C0C0, 0x00E5F5FE, 0x00E5F5FE,
    0x00C8FCF8,
};

#else   /* _PC3D_WINDOW_STYLE */

int WinMainMetrics [MWM_ITEM_NUMBER] =
{
    50, 50,
    2, 2, 1,
    20, 16, 16,
    16, 8, 5, 16,
    16, 2, 18, 4, 4,
    4, 4, 64, 4, 4, 14,
    16, 2, 12, 12, 12, 12, 8,
    18,
};

static const unsigned long __mg_wec [WEC_ITEM_NUMBER] =
{
    0x00808080, 0x00C0C0C0, 0x00800000,
    0x00FFFFFF, 0x00808080, 0x00C0C0C0,
    0x00000000, 0x00FF0000, 0x00000000,
    0x00C0C0C0, 0x00000000, 0x00800000,
    0x00FFFFFF, 0x00808080, 0x00C0C0C0,
    0x00000000, 0x00800000, 0x00FFFFFF,
    0x00808080, 0x00C0C0C0, 0x00FF0000,
    0x00C66931, 0x00C0C0C0, 0x00000000,
    0x00FFFFFF, 0x00808080, 0x00808080,
    0x00C0C0C0, 0x00000000, 0x00C0C0C0,
    0x00FF0000, 0x00BDA69C, 0x00FFFFFF,
    0x00FF00FF, 0x00FF0000, 0x00C0C0C0,
    0x00C8FCF8,
};
#endif /* _PC3D_WINDOW_STYLE */

BOOL InitWindowElementColors (void)
{
    int i;
    unsigned long int rgb;

    for (i = 0; i < WEC_ITEM_NUMBER; i++) {
        rgb = __mg_wec[i];
        WinElementColors [i] = RGB2Pixel (HDC_SCREEN, 
                GetRValue (rgb), GetGValue(rgb), GetBValue (rgb));
    }

    return TRUE;
}


#endif /* _INCORE_RES */

