/*
** $Id: desktop-lite.c,v 1.49 2003/12/11 05:07:04 weiym Exp $
**
** desktop-lite.c: The desktop for MiniGUI-Lite
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2000 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/04/19
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
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "menu.h"
#include "timer.h"
#include "dc.h"
#include "icon.h"

#include "misc.h"

#ifndef _STAND_ALONE
  #include "ourhdr.h"
  #include "client.h"
  #include "server.h"
  #include "drawsemop.h"
#endif

/******************************* global data *********************************/
RECT g_rcScr;

HWND __mg_capture_wnd;
HWND __mg_ime_wnd;
MSGQUEUE __mg_dsk_msgs;
PMAINWIN __mg_active_mainwnd;
PTRACKMENUINFO __mg_ptmi;

BOOL __mg_switch_away; // always be zero for clients.

#ifndef _STAND_ALONE
GHANDLE __mg_layer;
RECT g_rcDesktop;
#endif

/********************* Window management support *****************************/
static BLOCKHEAP sg_FreeInvRectList;
static BLOCKHEAP sg_FreeClipRectList;
static ZORDERINFO sg_MainWinZOrder;
static ZORDERINFO sg_TopMostWinZOrder;

static HWND sg_hCaretWnd;
static UINT sg_uCaretBTime;

static GCRINFO sg_ScrGCRInfo;

#ifndef _STAND_ALONE
GHANDLE GUIAPI GetLayerInfo (const char* layer_name, RECT* max_rect,
                                int* nr_clients, BOOL* is_topmost, int* cli_active)
{
    if (layer_name) {
        LAYERINFO info;
        if (mgIsServer) {
            if (layer_name [0] == '\0') return INV_LAYER_HANDLE;
            get_layer_info (0, layer_name, &info);
        }
        else {
            REQUEST req;

            req.id = REQID_LAYERINFO;
            req.data = layer_name;
            req.len_data = strlen (layer_name) + 1;

            if (cli_request (&req, &info, sizeof (LAYERINFO)) < 0)
                goto ret;
        }

        if (info.handle) {
            if (max_rect) *max_rect = info.max_rect;
            if (nr_clients) *nr_clients = info.nr_clients;
            if (is_topmost) *is_topmost = info.is_topmost;
            if (cli_active) *cli_active = info.cli_active;
        }

        return info.handle;
    }

ret:
    return INV_LAYER_HANDLE;
}

BOOL GUIAPI BringLayer2Topmost (GHANDLE handle)
{
    BOOL ret = FALSE;

    if (handle != INV_LAYER_HANDLE) {
        if (mgIsServer) {
            MG_Layer* layer = (MG_Layer*) handle;

            if (is_valid_layer (layer) && layer->cli_head) {
                SetTopMostLayer (layer);
                ret = TRUE;
            }
        }
        else {
            REQUEST req;

            req.id = REQID_TOPMOSTLAYER;
            req.data = &handle;
            req.len_data = sizeof (GHANDLE);

            if (cli_request (&req, &ret, sizeof (BOOL)) < 0)
                ret = FALSE;
        }
    }

    return ret;
}

BOOL GUIAPI SetActiveClient (int active)
{
    BOOL ret = FALSE;

    if (mgIsServer) {
        if (active >= 0 && active < mgClientSize && mgClients [active].fd != -1) {
            set_active_client (mgClients + active);
            ret = TRUE;
        }
    }
    else {
        REQUEST req;

        req.id = REQID_ACTIVECLIENT;
        req.data = &active;
        req.len_data = sizeof (int);

        if (cli_request (&req, &ret, sizeof (BOOL)) < 0)
            ret = FALSE;
    }

    return ret;
}
 
GHANDLE GUIAPI JoinLayer (const char* layer_name, const char* client_name,
                int lx, int ty, int rx, int by)
{
    GHANDLE layer_handle = INV_LAYER_HANDLE;

    g_rcDesktop.left = lx;
    g_rcDesktop.top = ty;
    g_rcDesktop.right = rx;
    g_rcDesktop.bottom = by;
    NormalizeRect (&g_rcDesktop);
    IntersectRect (&g_rcDesktop, &g_rcDesktop, &g_rcScr);

    if (mgIsServer) { // will ignore layer_name and app_name
        SetCursor (GetSystemCursor (IDC_ARROW));
        SetCursorPos (g_rcScr.right >> 1, g_rcScr.bottom >> 1);

        g_rcDesktop = g_rcScr;
        SHAREDRES_TOPMOST_LAYER = __mg_layer = 0;
        layer_handle = 1;
    }
    else {
        REQUEST req;
        JOINLAYERINFO info;
        JOINEDCLIENTINFO joined_info;

        if (layer_name) {
            strncpy (info.layer_name, layer_name, LEN_LAYER_NAME);
            info.layer_name [LEN_LAYER_NAME] = '\0';
        }
        else
            info.layer_name [0] = '\0';
        if (client_name) {
            strncpy (info.client_name, client_name, LEN_CLIENT_NAME);
            info.client_name [LEN_CLIENT_NAME] = '\0';
        }
        else
            info.client_name [0] = '\0';
        info.desktop_rc = g_rcDesktop;

        req.id = REQID_JOINLAYER;
        req.data = &info;
        req.len_data = sizeof (JOINLAYERINFO);

        if (cli_request (&req, &joined_info, sizeof (JOINEDCLIENTINFO)) < 0)
            goto ret;

        if (IsRectEmpty (&joined_info.desktop_rc)) {
            uHALr_printf( "JoinLayer: Empty desktop rect.\n");
            goto ret;
        }

        layer_handle = joined_info.layer_handle;
        g_rcDesktop = joined_info.desktop_rc;

        SelectClipRect (HDC_SCREEN, &g_rcDesktop);

        __mg_layer = layer_handle;
    }

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcDesktop);
    sg_ScrGCRInfo.age = 0;

    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

ret:
    return layer_handle;
}

#endif /* !STAND_ALONE */

static BOOL InitWndManagementInfo (void)
{
#ifndef _INCORE_RES
    if (!InitMainWinMetrics())
        return FALSE;
#endif

    __mg_capture_wnd = HWND_DESKTOP;
    __mg_active_mainwnd = NULL;

    __mg_ptmi = NULL;

    __mg_ime_wnd = HWND_DESKTOP;
    sg_hCaretWnd = HWND_DESKTOP;

    return TRUE;
}

#ifdef _STAND_ALONE

// #include "..\sysres\resource.c"

#else

PBITMAP SystemBitmap [SYSBMP_ITEM_NUMBER];
HICON  LargeSystemIcon [SYSICO_ITEM_NUMBER];
HICON  SmallSystemIcon [SYSICO_ITEM_NUMBER];

static void init_system_bitmap (int id)
{
    int i;
    void *temp;
	int bpp;

    bpp = BYTESPERPHYPIXEL;
    temp   = mgSharedRes + ((PG_RES)mgSharedRes)->bmpoffset;
    if (id < ((PG_RES)mgSharedRes)->bmpnum) {

        if (!(SystemBitmap [id] = (PBITMAP) malloc (sizeof (BITMAP))))
            return;

        for (i = 0; i < id; i++)
		    temp += sizeof(BITMAP) + ((PBITMAP)temp)->bmPitch * ((PBITMAP)temp)->bmHeight;

		SystemBitmap[id]->bmWidth  = ((PBITMAP)temp)->bmWidth;
		SystemBitmap[id]->bmHeight = ((PBITMAP)temp)->bmHeight;

		SystemBitmap[id]->bmType = BMP_TYPE_NORMAL;
		SystemBitmap[id]->bmBitsPerPixel = bpp << 3;
		SystemBitmap[id]->bmBytesPerPixel = bpp;
		SystemBitmap[id]->bmPitch  = ((PBITMAP)temp)->bmPitch;
		SystemBitmap[id]->bmColorKey = ((PBITMAP)temp)->bmColorKey;

		SystemBitmap[id]->bmBits = temp + sizeof(BITMAP);
    }
}

PBITMAP GUIAPI GetSystemBitmap (int id)
{
    if (id >= SYSBMP_ITEM_NUMBER || id < 0)
        return NULL;

    if (SystemBitmap [id] == NULL)
        init_system_bitmap (id);

    return SystemBitmap [id];
}

static void init_large_system_icon (int id)
{
    void *temp;
	int bpp;

    bpp = BYTESPERPHYPIXEL;
	temp = mgSharedRes + ((PG_RES)mgSharedRes)->iconoffset;

    if (id < ((PG_RES)mgSharedRes)->iconnum) {
        if (!(LargeSystemIcon[id] = (HICON)malloc (sizeof(ICON))))
            return;

		temp += (sizeof(ICON) * 2 + (2*32*32 + 2*16*16)*bpp) * id;
        ((PICON)LargeSystemIcon[id])->width   = ((PICON)temp)->width;
        ((PICON)LargeSystemIcon[id])->height  = ((PICON)temp)->height;
#ifdef _USE_NEWGAL
        ((PICON)LargeSystemIcon[id])->pitch   = ((PICON)temp)->pitch;
#endif
        ((PICON)LargeSystemIcon[id])->AndBits = temp + sizeof(ICON);
        ((PICON)LargeSystemIcon[id])->XorBits = temp + sizeof(ICON) + bpp*32*32;
    }
}

static void init_small_system_icon (int id)
{
    void *temp;
	int bpp;

    bpp = BYTESPERPHYPIXEL;
	temp = mgSharedRes + ((PG_RES)mgSharedRes)->iconoffset;

    if (id < ((PG_RES)mgSharedRes)->iconnum) {
        if (!(SmallSystemIcon[id] = (HICON)malloc (sizeof(ICON))))
            return;

		temp += (sizeof(ICON)*2 + (2*32*32 + 2*16*16)*bpp) * id;
		temp += sizeof(ICON) + 2*bpp*32*32;
        ((PICON)SmallSystemIcon[id])->width   = ((PICON)temp)->width;
        ((PICON)SmallSystemIcon[id])->height  = ((PICON)temp)->height;
#ifdef _USE_NEWGAL
        ((PICON)SmallSystemIcon[id])->pitch   = ((PICON)temp)->pitch;
#endif
        ((PICON)SmallSystemIcon[id])->AndBits = temp + sizeof(ICON);
        ((PICON)SmallSystemIcon[id])->XorBits = temp + sizeof(ICON) + bpp*16*16;
    }
}

HICON GUIAPI GetLargeSystemIcon (int id)
{
    if (id >= SYSICO_ITEM_NUMBER || id < 0)
        return 0;

    if (LargeSystemIcon [id] == 0)
        init_large_system_icon (id);

    return LargeSystemIcon [id];
}

HICON GUIAPI GetSmallSystemIcon (int id)
{
    if (id >= SYSICO_ITEM_NUMBER || id < 0)
        return 0;

    if (SmallSystemIcon [id] == 0)
        init_small_system_icon (id);

    return SmallSystemIcon [id];
}

static void TerminateSharedSysRes (void)
{
    int i;
    
    for (i=0; i<SYSBMP_ITEM_NUMBER; i++) {
        if (SystemBitmap [i]) {
            free (SystemBitmap [i]);
        }
    }

    for (i=0; i<SYSICO_ITEM_NUMBER; i++) {
        if (SmallSystemIcon [i])
            free ((PICON)SmallSystemIcon[i]);

        if (LargeSystemIcon [i])
            free ((PICON)LargeSystemIcon[i]);
    }
}

#endif /* !_STAND_ALONE */

static void InitZOrderInfo (PZORDERINFO pZOrderInfo, HWND hHost);

BOOL InitDesktop (void)
{
    /*
     * Init ZOrderInfo here.
     */
    InitZOrderInfo (&sg_MainWinZOrder, HWND_DESKTOP);
    InitZOrderInfo (&sg_TopMostWinZOrder, HWND_DESKTOP);
    
    /*
     * Init heap of clipping rects.
     */
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);

    /*
     * Init heap of invalid rects.
     */
    InitFreeClipRectList (&sg_FreeInvRectList, SIZE_INVRECTHEAP);

    // Init Window Management information.
    if (!InitWndManagementInfo ())
        return FALSE;

#ifdef _STAND_ALONE
    /*
     * Load system resource here.
     */

    if (!InitSystemRes ()) {
        uHALr_printf( "DESKTOP: Can not initialize system resource!\n");
        return FALSE;
    }

    InitClipRgn (&sg_ScrGCRInfo.crgn, &sg_FreeClipRectList);
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcScr);
    sg_ScrGCRInfo.age = 0;

    SendMessage (HWND_DESKTOP, MSG_STARTSESSION, 0, 0);
    SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);

#endif

    return TRUE;
}

#include "desktop-comm.c"

