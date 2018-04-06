/*
** $Id: resource.c,v 1.26 2004/08/10 00:17:37 snig Exp $
**
** resource.c: This file include some functions for system resource loading. 
**           some functions are from misc.c.
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
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cursor.h"
#include "icon.h"
#include "sysres.h"
#include "misc.h"

BITMAP SystemBitmap [SYSBMP_ITEM_NUMBER];
HICON  LargeSystemIcon [SYSICO_ITEM_NUMBER] = {0};
HICON  SmallSystemIcon [SYSICO_ITEM_NUMBER] = {0};


#ifdef _CTRL_BUTTON
#   define _NEED_STOCKBMP_BUTTON            1
#   ifdef _PHONE_WINDOW_STYLE
#       define _NEED_STOCKBMP_PUSHBUTTON    1
#   endif
#endif

#if defined(_CTRL_COMBOBOX) || defined (_CTRL_MENUBUTTON)
#   define _NEED_STOCKBMP_DOWNARROW         1
#endif

#ifdef _CTRL_COMBOBOX
#   define _NEED_STOCKBMP_DOUBLEARROW       1
#endif

#ifdef _CTRL_LISTBOX
#   define _NEED_STOCKBMP_CHECKMARK         1
#endif

#if defined(_CTRL_TRACKBAR) && defined(_PHONE_WINDOW_STYLE)
#   define _NEED_STOCKBMP_TRACKBAR          1
#endif

#ifdef _EXT_CTRL_SPINBOX
#   define _NEED_STOCKBMP_SPINBOX           1
#endif

#ifdef _IME_GB2312
#   define _NEED_STOCKBMP_IME               1
#endif

#ifdef _MISC_ABOUTDLG
#   define _NEED_STOCKBMP_LOGO              1
#endif

#ifdef _EXT_CTRL_TREEVIEW
#   define _NEED_SYSICON_FOLD               1
#endif

#ifdef _EXT_CTRL_LISTVIEW
#   define _NEED_STOCKBMP_LVFOLD            1
#   define _NEED_SYSICON_FILETYPE           1
#endif

static struct _StockBitmaps {
    const char* name;
    BITMAP bmp;
} StockBitmaps [] =
{
#ifdef _NEED_STOCKBMP_BUTTON
    {STOCKBMP_BUTTON},
#endif
#ifdef _NEED_STOCKBMP_PUSHBUTTON
    {STOCKBMP_PUSHBUTTON},
    {STOCKBMP_PUSHEDBUTTON},
#endif
#ifdef _NEED_STOCKBMP_DOWNARROW
    {STOCKBMP_DOWNARROW},
#endif
#ifdef _NEED_STOCKBMP_DOUBLEARROW
    {STOCKBMP_UPDOWNARROW},
    {STOCKBMP_LEFTRIGHTARROW},
#endif
#ifdef _NEED_STOCKBMP_CHECKMARK
    {STOCKBMP_CHECKMARK},
#endif
#ifdef _NEED_STOCKBMP_TRACKBAR
    {STOCKBMP_TRACKBAR_HBG},
    {STOCKBMP_TRACKBAR_HSLIDER},
    {STOCKBMP_TRACKBAR_VBG},
    {STOCKBMP_TRACKBAR_VSLIDER},
#endif
#ifdef _NEED_STOCKBMP_SPINBOX
    {STOCKBMP_SPINBOX_HORZ},
    {STOCKBMP_SPINBOX_VERT},
#endif
#ifdef _NEED_STOCKBMP_LVFOLD
    {STOCKBMP_LVFOLD},
    {STOCKBMP_LVUNFOLD},
#endif
#ifdef _NEED_STOCKBMP_IME
    {STOCKBMP_IMECTRLBTN},
#endif
#ifdef _NEED_STOCKBMP_LOGO
    {STOCKBMP_LOGO}
#endif
};

const BITMAP* GUIAPI GetStockBitmap (const char* name, int ckey_x, int ckey_y)
{
    int i;

    if (!name || name [0] == '\0')
        return NULL;

    for (i = 0; i < TABLESIZE(StockBitmaps); i++) {
        if (strcmp (name, StockBitmaps [i].name) == 0) {
            PBITMAP bmp = &StockBitmaps [i].bmp;

            if (bmp->bmWidth == 0 || bmp->bmHeight == 0) {
                if (!LoadSystemBitmapEx (bmp, name, ckey_x, ckey_y))
                    return NULL;
            }

            return bmp;
        }
    }

    return NULL;
}

#ifndef _INCORE_RES

/****************************** System resource support *********************/

#ifdef _CURSOR_SUPPORT
PCURSOR LoadSystemCursor (int i)
{
    PCURSOR tempcsr;

    char szValue[MAX_NAME + 1];
    char szPathName[MAX_PATH + 1];
    char szKey[10];

    if (GetMgEtcValue (CURSORSECTION, "cursorpath", szPathName, MAX_PATH) < 0)
                 goto error;

    sprintf (szKey, "cursor%d", i);
    if (GetMgEtcValue (CURSORSECTION, szKey, szValue, MAX_NAME) < 0)
                goto error;

    strcat (szPathName, szValue);

    if (!(tempcsr = (PCURSOR)LoadCursorFromFile (szPathName)))
                     goto error;

    return tempcsr;

error:
    return 0;
}
#endif

BOOL GUIAPI LoadSystemBitmapEx (PBITMAP pBitmap, const char* szItemName, int ckey_x, int ckey_y)
{
    char szPathName[MAX_PATH + 1];
    char szValue[MAX_NAME + 1];
    
    if (GetMgEtcValue ("bitmapinfo", szItemName,
            szValue, MAX_NAME) < 0 ) {
        uHALr_printf( "LoadSystemBitmapEx: Get bitmap file name error!\n");
        return FALSE;
    }
    
    if (GetMgEtcValue ("bitmapinfo", "bitmappath",
            szPathName, MAX_PATH) < 0 ) {
        uHALr_printf( "LoadSystemBitmapEx: Get bitmap path error!\n");
        return FALSE;
    }

    if (strcmp (szValue, "none") == 0) {
        memset (pBitmap, 0, sizeof (BITMAP));
        return TRUE;
    }

    strcat(szPathName, szValue);
    
    if (LoadBitmap (HDC_SCREEN, pBitmap, szPathName) < 0) {
        uHALr_printf( "LoadSystemBitmapEx: Load bitmap error: %s!\n", szPathName);
        return FALSE;
    }
    
    if (ckey_x >= 0 && ckey_x < pBitmap->bmWidth
            && ckey_y >= 0 && ckey_y < pBitmap->bmHeight) {
        pBitmap->bmType = BMP_TYPE_COLORKEY;
        pBitmap->bmColorKey = GetPixelInBitmap (pBitmap, 0, 0);
    }

    return TRUE;
}

HICON GUIAPI LoadSystemIcon (const char* szItemName, int which)
{
    char szPathName[MAX_PATH + 1];
    char szValue[MAX_NAME + 1];
    HICON hIcon;
    
    if (GetMgEtcValue ("iconinfo", szItemName,
            szValue, MAX_NAME) < 0 ) {
        uHALr_printf( "LoadSystemIcon: Get icon file name error!\n");
        return 0;
    }
    
    if (GetMgEtcValue ("iconinfo", "iconpath",
            szPathName, MAX_PATH) < 0 ) {
        uHALr_printf( "LoadSystemIcon: Get icon path error!\n");
        return 0;
    }

    strcat (szPathName, szValue);
    
    if ((hIcon = LoadIconFromFile (HDC_SCREEN, szPathName, which)) == 0) {
        uHALr_printf( "LoadSystemIcon: Load icon error: %s!\n", szPathName);
        return 0;
    }
    
    return hIcon;
}

BOOL InitSystemRes (void)
{
    int i;
    int nBmpNr, nIconNr;
    char szValue [12];
    
    /*
     * Load system bitmaps here.
     */
    if (GetMgEtcValue ("bitmapinfo", "bitmapnumber", 
                            szValue, 10) < 0)
        return FALSE;
    nBmpNr = atoi (szValue);
    if (nBmpNr <= 0) return FALSE;
    nBmpNr = nBmpNr < SYSBMP_ITEM_NUMBER ? nBmpNr : SYSBMP_ITEM_NUMBER;

    for (i = 0; i < nBmpNr; i++) {
        sprintf (szValue, "bitmap%d", i);

        if (!LoadSystemBitmap (SystemBitmap + i, szValue))
            return FALSE;
    }

    /*
     * Load system icons here.
     */
    if (GetMgEtcValue ("iconinfo", "iconnumber", 
                            szValue, 10) < 0 )
        return FALSE;
    nIconNr = atoi(szValue);
    if (nIconNr <= 0) return FALSE;
    nIconNr = nIconNr < SYSICO_ITEM_NUMBER ? nIconNr : SYSICO_ITEM_NUMBER;

    for (i = 0; i < nIconNr; i++) {
        sprintf(szValue, "icon%d", i);
        
        SmallSystemIcon [i] = LoadSystemIcon (szValue, 1);
        LargeSystemIcon [i] = LoadSystemIcon (szValue, 0);

        if (SmallSystemIcon [i] == 0 || LargeSystemIcon [i] == 0)
            return FALSE;
    }

    return TRUE;
}

#else /* _INCORE_RES */

#define NR_BMPS   7
#define NR_ICONS  5
#define SZ_ICON   1078
#define SZ_CURSOR   766

#ifdef _CURSOR_SUPPORT
#   include "cursors.c"
#endif

#ifdef _FLAT_WINDOW_STYLE

#ifdef _NEED_STOCKBMP_BUTTON
#   include "button-flat-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_DOWNARROW
#   include "downarrow-flat-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_DOUBLEARROW
#   include "updownarrow-flat-bmp.c"
#   include "leftrightarrow-flat-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_CHECKMARK
#   include "checkmark-flat-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_SPINBOX
#   include "spinbox-flat-bmp.c"
#endif

#ifdef __ECOS__
#   include "bmps-flat-ecos.c"
#else
#   include "bmps-flat.c"
#endif

#include "icons-flat.c"

#elif defined (_PHONE_WINDOW_STYLE)

#ifdef _NEED_STOCKBMP_BUTTON
#   include "button-phone-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_PUSHBUTTON
#   include "pushbutton-phone-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_DOWNARROW
#   include "downarrow-phone-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_DOUBLEARROW
#   include "updownarrow-phone-bmp.c"
#   include "leftrightarrow-phone-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_CHECKMARK
#   include "checkmark-3d-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_TRACKBAR
#   include "trackbar-phone-bmp.c"
#endif

#ifdef _NEED_STOCKBMP_SPINBOX
#include "spinbox-phone-bmp.c"
#endif

#include "bmps-phone.c"

#include "icons-3d.c"

#else

#ifdef _NEED_STOCKBMP_BUTTON
#   include "button_bmp_data.c"
#endif

#ifdef _NEED_STOCKBMP_DOWNARROW
#   include "downarrow_bmp_data.c"
#endif

#ifdef _NEED_STOCKBMP_DOUBLEARROW
#   include "updownarrow_bmp_data.c"
#   include "leftrightarrow_bmp_data.c"
#endif

#ifdef _NEED_STOCKBMP_CHECKMARK
#   include "checkmark_bmp_data.c"
#endif

#ifdef _NEED_STOCKBMP_SPINBOX
#   include "spinbox-3d-bmp.c"
#endif

#ifdef __ECOS__
#   include "bmps-3d-ecos.c"
#else
#   include "bmps_data.c"
#endif

#include "icons_data.c"

#endif /* _FLAT_WINDOW_STYLE */

#ifdef _NEED_SYSICON_FOLD
#   include "fold-ico.c"
#   include "unfold-ico.c"
#endif

#ifdef _NEED_SYSICON_FILETYPE
#   include "filetype-icons.c"
#endif

#ifdef _NEED_STOCKBMP_LVFOLD
#   include "lvfold-bmp.c"
#   include "lvunfold-bmp.c"
#endif

#include "icones_data.c"

static int sz_bmps[NR_BMPS]={566,694,0,0,0,0,77878};

static SYSRES sysres_data [] = {
#ifdef _NEED_STOCKBMP_BUTTON
        {STOCKBMP_BUTTON, (void*)button_bmp_data, sizeof(button_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_PUSHBUTTON
        {STOCKBMP_PUSHBUTTON, (void*)pushbutton_bmp_data, sizeof(pushbutton_bmp_data), 0},
        {STOCKBMP_PUSHEDBUTTON, (void*)pushedbutton_bmp_data, sizeof(pushedbutton_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_DOWNARROW
        {STOCKBMP_DOWNARROW, (void*)downarrow_bmp_data, sizeof(downarrow_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_DOUBLEARROW
        {STOCKBMP_UPDOWNARROW, (void*)updownarrow_bmp_data, sizeof(updownarrow_bmp_data), 0},
        {STOCKBMP_LEFTRIGHTARROW, (void*)leftrightarrow_bmp_data, sizeof(leftrightarrow_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_CHECKMARK
        {STOCKBMP_CHECKMARK, (void*)checkmark_bmp_data, sizeof(checkmark_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_TRACKBAR
        {STOCKBMP_TRACKBAR_HBG, (void*)trackbar_hbg_bmp, sizeof(trackbar_hbg_bmp), 0},
        {STOCKBMP_TRACKBAR_HSLIDER, (void*)trackbar_hslider_bmp, sizeof(trackbar_hslider_bmp), 0},
        {STOCKBMP_TRACKBAR_VBG, (void*)trackbar_vbg_bmp, sizeof(trackbar_vbg_bmp), 0},
        {STOCKBMP_TRACKBAR_VSLIDER, (void*)trackbar_vslider_bmp, sizeof(trackbar_vslider_bmp), 0},
#endif
#ifdef _NEED_STOCKBMP_SPINBOX
        {STOCKBMP_SPINBOX_VERT, (void*)spinbox_vert_bmp_data, sizeof(spinbox_vert_bmp_data), 0},
        {STOCKBMP_SPINBOX_HORZ, (void*)spinbox_horz_bmp_data, sizeof(spinbox_horz_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_LVFOLD
        {STOCKBMP_LVFOLD, (void*)lvfold_bmp_data, sizeof(lvfold_bmp_data), 0},
        {STOCKBMP_LVUNFOLD, (void*)lvunfold_bmp_data, sizeof(lvunfold_bmp_data), 0},
#endif
#ifdef _NEED_STOCKBMP_IME
        {STOCKBMP_IMECTRLBTN, NULL, 0, 0},
#endif
#ifdef _NEED_STOCKBMP_LOGO
        {STOCKBMP_LOGO, NULL, 0, 0},
#endif
#ifdef _NEED_SYSICON_FOLD
        {SYSICON_FOLD, (void*)fold_ico_data, sizeof(fold_ico_data), 0},
        {SYSICON_UNFOLD, (void*)unfold_ico_data, sizeof(unfold_ico_data), 0},
#endif
#ifdef _NEED_SYSICON_FILETYPE
        {SYSICON_FT_DIR, (void*)ft_dir_ico_data, sizeof(ft_dir_ico_data), 0},
        {SYSICON_FT_FILE, (void*)ft_file_ico_data, sizeof(ft_file_ico_data), 0},
#endif
        {"icons", (void*)icons_data, SZ_ICON, 0},
        {"bitmap", (void*)bmps_data, (int)sz_bmps, 1}
};

#define SYSRES_NR (sizeof(sysres_data) / sizeof(SYSRES))

static void* get_res_position (const char* szItemName, int *len)
{
    int i;
    int idx_len;
    int name_len;
    int item_idx;
    const char *pidx;

    i = 0;
    idx_len = 0;
    item_idx = 0;

    if (!szItemName || szItemName[0] == '\0')
        return NULL;

    name_len = strlen (szItemName);
    pidx = szItemName + name_len - 1;
    idx_len = 0;
    while ( isdigit(*pidx) )
    {
        idx_len ++;
        if (idx_len == name_len)
            return NULL;
        pidx --;
    }
    name_len -= idx_len;

    if (idx_len > 0)
        item_idx = atoi (szItemName+name_len);

    while ( strncmp(sysres_data[i].name, szItemName, name_len) != 0 && i < SYSRES_NR) i++;
    if (i >= SYSRES_NR)
        return NULL;

    if (sysres_data[i].flag > 0) {
        Uint8* pos = sysres_data[i].res_data;
        int j;
        for (j=0; j<item_idx; j++) {
            pos += *( (int*)sysres_data[i].data_len + j );
        }
        if (len)
            *len = *( (int*)sysres_data[i].data_len + item_idx );
        return pos;
    }
    if (len)
        *len = sysres_data[i].data_len;
    return (Uint8*) sysres_data[i].res_data + sysres_data[i].data_len *item_idx;
}

HICON GUIAPI LoadSystemIcon (const char* szItemName, int which)
{
    void *icon;

    icon = get_res_position (szItemName, NULL);
    if (!icon)
        return 0;

    return LoadIconFromMem (HDC_SCREEN, icon, which);
}

BOOL GUIAPI LoadSystemBitmapEx (PBITMAP pBitmap, const char* szItemName, int ckey_x, int ckey_y)
{
    Uint8* bmpdata;
    int len, nc;
   
    bmpdata = get_res_position (szItemName, &len);
    if (len == 0) {
        memset (pBitmap, 0, sizeof (BITMAP));
        return TRUE;
    }

    if (!bmpdata) {
        uHALr_printf("LoadSystemBitmapEx: Get bitmap data error!\n");
        return FALSE;
    }

    if ((nc = LoadBitmapFromMem (HDC_SCREEN, pBitmap,
                              bmpdata, len, "bmp"))) {
        uHALr_printf("LoadSystemBitmapEx: Load bitmap error: %p, %d: %d!\n", bmpdata, len, nc);
        return FALSE;
    }

    if (ckey_x >= 0 && ckey_x < pBitmap->bmWidth
            && ckey_y >= 0 && ckey_y < pBitmap->bmHeight) {
        pBitmap->bmType = BMP_TYPE_COLORKEY;
        pBitmap->bmColorKey = GetPixelInBitmap (pBitmap, 0, 0);
    }

    return TRUE;
}

BOOL InitSystemRes (void)
{
    int i;
    const Uint8* bmp;
    const Uint8* icon;

    bmp = bmps_data;
    icon = icones_data;

    for (i = 0; i < NR_BMPS; i++) {
        if (sz_bmps [i] > 0) {
            if (LoadBitmapFromMem (HDC_SCREEN, SystemBitmap + i, bmp, 
                                sz_bmps [i], "bmp")) {
                uHALr_printf("InitSystemRes: error when loading %d system bitmap.\n", i);
                return FALSE;
            }
            bmp += sz_bmps [i];
        }
    }

    for (i = 0; i < NR_ICONS; i++) {
        SmallSystemIcon [i] = LoadIconFromMem (HDC_SCREEN, icon, 1);
        LargeSystemIcon [i] = LoadIconFromMem (HDC_SCREEN, icon, 0);

        icon += SZ_ICON;

        if (SmallSystemIcon [i] == 0  || LargeSystemIcon [i] == 0 )  {
            uHALr_printf("InitSystemRes: error when loading %d system icon.\n", i);
            return FALSE;
        }
    }

    return TRUE;
}

#ifdef _CURSOR_SUPPORT
PCURSOR LoadSystemCursor (int i)
{
    return (PCURSOR)LoadCursorFromMem (cursors_data + SZ_CURSOR*i);
}
#endif

#endif /* _INCORE_RES */


void TerminateSystemRes (void)
{
    int i;
    
    for (i=0; i<SYSBMP_ITEM_NUMBER; i++)
        UnloadBitmap (SystemBitmap + i);

    for (i=0; i<SYSICO_ITEM_NUMBER; i++) {
        if (SmallSystemIcon [i])
            DestroyIcon (SmallSystemIcon [i]);

        if (LargeSystemIcon [i])
            DestroyIcon (LargeSystemIcon [i]);
    }
}


