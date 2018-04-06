/*
** $Id: gdi.c,v 1.55 2005/01/10 07:30:45 weiym Exp $
**
** The graphics display interface module of MiniGUI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
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
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "cursor.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "sysfont.h"
#include "devfont.h"
#include "drawtext.h"
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    #include "ourhdr.h"
    #include "client.h"
    #include "sharedres.h"
    #include "drawsemop.h"
#endif

/************************* global data define ********************************/
DC __mg_screen_dc;

gal_pixel SysPixelIndex [17];

// This should be the standard EGA palette.
const RGB SysPixelColor [] = {
    {0x00, 0x00, 0x00},     // transparent   --0
    {0x00, 0x00, 0x80},     // dark blue     --1
    {0x00, 0x80, 0x00},     // dark green    --2
    {0x00, 0x80, 0x80},     // dark cyan     --3
    {0x80, 0x00, 0x00},     // dark red      --4
    {0x80, 0x00, 0x80},     // dark magenta  --5
    {0x80, 0x80, 0x00},     // dark yellow   --6
    {0x80, 0x80, 0x80},     // dark gray     --7
    {0xC0, 0xC0, 0xC0},     // light gray    --8
    {0x00, 0x00, 0xFF},     // blue          --9
    {0x00, 0xFF, 0x00},     // green         --10
    {0x00, 0xFF, 0xFF},     // cyan          --11
    {0xFF, 0x00, 0x00},     // red           --12
    {0xFF, 0x00, 0xFF},     // magenta       --13
    {0xFF, 0xFF, 0x00},     // yellow        --14
    {0xFF, 0xFF, 0xFF},     // light white   --15
    {0x00, 0x00, 0x00}      // black         --16
};

#ifndef _LITE_VERSION
// mutex ensuring exclusive access to gdi. 
pthread_mutex_t __mg_gdilock;
#endif

/************************* global functions declaration **********************/
// the following functions, which defined in other module
// but used in this module.
extern PGCRINFO GetGCRgnInfo (HWND hWnd);

/**************************** static data ************************************/
// General DC
static DC DCSlot [DCSLOTNUMBER];

#ifndef _LITE_VERSION
// mutex ensuring exclusive access to DC slot. 
static pthread_mutex_t dcslot;
#endif

static BLOCKHEAP sg_FreeClipRectList;

/************************* static functions declaration **********************/
static void dc_InitClipRgnInfo (void);
static void dc_InitDC (PDC pdc, HWND hWnd, BOOL bIsClient);
static void dc_Init_ScreenDC (void);

/************************** inline functions *********************************/
inline void WndRect(HWND hWnd, PRECT prc)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    pParent = (PCONTROL) hWnd;
    pCtrl = (PCONTROL) hWnd;

    if (hWnd == HWND_DESKTOP) {
        *prc = g_rcScr;
        return;
    }

    prc->left = pCtrl->left;
    prc->top  = pCtrl->top;
    prc->right = pCtrl->right;
    prc->bottom = pCtrl->bottom;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

inline void WndClientRect(HWND hWnd, PRECT prc)
{
    PCONTROL pCtrl;
    PCONTROL pParent;
    
    pParent = (PCONTROL) hWnd;
    pCtrl = (PCONTROL) hWnd;

    if (hWnd == HWND_DESKTOP) {
        *prc = g_rcScr;
        return;
    }

    prc->left = pCtrl->cl;
    prc->top  = pCtrl->ct;
    prc->right = pCtrl->cr;
    prc->bottom = pCtrl->cb;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

static void RestrictControlECRGN (CLIPRGN* crgn, PCONTROL pCtrl)
{
    RECT rc;
    PCONTROL pRoot;
    int off_x, off_y;
    
    pRoot = (PCONTROL) (pCtrl->pMainWin);
    off_x = 0;
    off_y = 0;

    do {
        PCONTROL pParent;
        pParent = pCtrl;

        rc.left = pRoot->cl + off_x;
        rc.top  = pRoot->ct + off_y;
        rc.right = pRoot->cr + off_x;
        rc.bottom = pRoot->cb + off_y;
        IntersectClipRect (crgn, &rc);
        if (IsEmptyClipRgn (crgn))
            return;

        if (pRoot == pCtrl->pParent)
            break;

        off_x += pRoot->cl;
        off_y += pRoot->ct;

        while (TRUE) {
            if (pRoot->children == pParent->pParent->children) {
                pRoot = pParent;
                break;
            }
            pParent = pParent->pParent;
        }
    } while (TRUE);
}

#ifdef _LITE_VERSION
#if !defined(_STAND_ALONE)
static void _clipped_by_server (PDC pdc)
{
    /*
     * Subtract the server clipping rects.
     */
    if (pdc->scr_sig != SHAREDRES_SCRINFO.signature) {
        int i;
        RECT* crcs;
        crcs = SHAREDRES_SCRINFO.clip_rects;

        SetClipRgn (&__mg_screen_dc.ecrgn, &__mg_screen_dc.DevRC);
        for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
            if (!IsRectEmpty (crcs + i))
                SubtractClipRect (&pdc->ecrgn, crcs + i);
        }
        pdc->scr_sig = SHAREDRES_SCRINFO.signature;
    }
}
#endif

PDC __mg_check_ecrgn (HDC hdc)
{
    PDC pdc; 
    pdc = dc_HDC2PDC(hdc);

    if (dc_IsGeneralDC (pdc)) {
        if (!dc_GenerateECRgn (pdc, FALSE)) {
            return NULL;
        }
    }

#if !defined(_STAND_ALONE)
    if (!mgIsServer && pdc == &__mg_screen_dc)
        _clipped_by_server (pdc);
#endif

    return pdc;
}
#endif

/******************* Initialization and termination of GDI *******************/
BOOL InitScreenDC (void)
{
    InitFreeClipRectList (&sg_FreeClipRectList, SIZE_CLIPRECTHEAP);
    
#ifndef _LITE_VERSION
    pthread_mutex_init (&__mg_gdilock, NULL);
    pthread_mutex_init (&dcslot, NULL);
#endif

    dc_InitClipRgnInfo();
    dc_Init_ScreenDC ();
    return TRUE;
}

void TerminateScreenDC (void)
{
    DestroyFreeClipRectList (&sg_FreeClipRectList);

#ifndef _LITE_VERSION
    pthread_mutex_destroy (&__mg_gdilock);
    pthread_mutex_destroy (&dcslot);
#endif
}

BOOL InitGDI (void)
{      

    if (!InitTextBitmapBuffer ()) {
       uHALr_printf("GDI: Can not initialize text bitmap buffer!\n");
       return FALSE;
    }

    ResetDevFont ();
    
#ifdef _RBF_SUPPORT
#ifdef _INCORE_RES
    if (!InitIncoreRBFonts ()) {
       uHALr_printf("GDI: Can not initialize incore RBF fonts!\n");
       return FALSE;
    }
#endif
#endif

#ifdef _VBF_SUPPORT
    if (!InitIncoreVBFonts ()) {
       uHALr_printf("GDI: Can not initialize incore VBF fonts!\n");
       return FALSE;
    }
#endif

#ifndef _INCORE_RES

#ifdef _RBF_SUPPORT
    if (!InitRawBitmapFonts ()) {
       uHALr_printf("GDI: Can not initialize raw bitmap fonts!\n");
       return FALSE;
    }
#endif

#ifdef _VBF_SUPPORT
    if (!InitVarBitmapFonts ()) {
       uHALr_printf("GDI: Can not initialize var bitmap fonts!\n");
       return FALSE;
    }
#endif

#ifdef _QPF_SUPPORT
    if (!InitQPFonts ()) {
       uHALr_printf("GDI: Can not initialize QPF fonts!\n");
       return FALSE;
    }
#endif

#ifndef _LITE_VERSION
#ifdef _TTF_SUPPORT
    if (!InitFreeTypeFonts ()) {
       uHALr_printf("GDI: Can not initialize TrueType fonts!\n");
       return FALSE;
    }
#endif

#ifdef _TYPE1_SUPPORT
    if (!InitType1Fonts ()) {
       uHALr_printf("GDI: Can not initialize Type1 fonts!\n");
       return FALSE;
    }
#endif
#endif /* !_LITE_VERSION */

#endif /* !_INCORE_RES */

    /* TODO: add other font support here */

#ifdef _DEBUG
    dumpDevFonts ();
#endif

    if (!InitSysFont ()) {
       uHALr_printf("GDI: Can not create system fonts!\n");
       return FALSE;
    }

    return TRUE;
}

void TerminateGDI( void )
{
    TermSysFont ();

    /* TODO: add other font support here */

#ifndef _INCORE_RES

#ifndef _LITE_VERSION
#ifdef _TTF_SUPPORT
    TermFreeTypeFonts ();
#endif

#ifdef _TYPE1_SUPPORT
	TermType1Fonts();
#endif

#ifdef _QPF_SUPPORT
    TermQPFonts ();
#endif
#endif /* !_LITE_VERSION */

#ifdef _VBF_SUPPORT
    TermVarBitmapFonts ();
#endif

#ifdef _RBF_SUPPORT
    TermRawBitmapFonts ();
#endif

#ifdef _VBF_SUPPORT
    TermIncoreVBFonts ();
#endif

#ifdef _RBF_SUPPORT
#ifdef _INCORE_RES
    TermIncoreRBFonts ();
#endif
#endif

#endif /* _INCORE_RES */

    ResetDevFont ();

    TermTextBitmapBuffer ();
}

/*
 * Function: int GUIAPI GetGDCapability( int iItem) 
 *      This Function return DC parameters.
 * Parameters:
 *      The element want to retrive.
 * Return:
 *      The parameter.
 */
unsigned int GUIAPI GetGDCapability (HDC hdc, int iItem )
{
    PDC pdc;
    unsigned int iret;
    
    iret = 0xFFFFFFFF;
    pdc = dc_HDC2PDC (hdc);

#ifndef _LITE_VERSION
    pthread_mutex_lock (&__mg_gdilock);
#endif

    // set graphics context.
    GAL_SetGC (pdc->gc);

    switch (iItem)
    {
        case GDCAP_DEPTH:
            iret = GAL_BitsPerPixel (pdc->gc);
            break;

        case GDCAP_BPP:
            iret = GAL_BytesPerPixel (pdc->gc);
            break;

        case GDCAP_COLORNUM:
            iret = GAL_Colors (pdc->gc);
            break;
 
        case GDCAP_HPIXEL:
            iret = GAL_Width (pdc->gc);
            break;

        case GDCAP_VPIXEL:
            iret = GAL_Height (pdc->gc);
            break;

        case GDCAP_MAXX:
            iret = RECTW (pdc->DevRC) - 1;
            break;

        case GDCAP_MAXY:
            iret = RECTH (pdc->DevRC) - 1;
            break;
    }

#ifndef _LITE_VERSION
    pthread_mutex_unlock(&__mg_gdilock);
#endif
    return iret;
}

// This function init clip region in all DC slots.
static void dc_InitClipRgnInfo(void)
{
    int i;

    for (i=0; i<DCSLOTNUMBER; i++) {

        DCSlot[i].inuse = FALSE;

        // Local clip region
        InitClipRgn(&DCSlot[i].lcrgn, &sg_FreeClipRectList);

        // Global clip region info
        DCSlot[i].pGCRInfo = NULL;
        DCSlot[i].oldage = 0;

        // Effective clip region
        InitClipRgn(&DCSlot[i].ecrgn, &sg_FreeClipRectList);
    }
   
}

// This function generates effective clip region from
// local clip region and global clip region.
// if the global clip region has a new age,
// this function empty effective clip region first,
// and then intersect local clip region and global clip region.

BOOL dc_GenerateECRgn(PDC pdc, BOOL fForce)
{
    RECT rc, rcInter;
    PCLIPRECT pcr, pgcr;
    PCONTROL pCtrl;

//    uHALr_printf("dc_GenerateECRgn fForce=%d\n",fForce);

    // is global clip region is empty?
    if ((!fForce) && (!dc_IsVisible (pdc)))
            return FALSE;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer) lock_scr_sem ();
#endif

    // need regenerate?
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (fForce || (pdc->oldage != pdc->pGCRInfo->age)
               || (!mgIsServer && pdc->scr_sig != SHAREDRES_SCRINFO.signature)) {
#else
    if (fForce || (pdc->oldage != pdc->pGCRInfo->age)) {
#endif
        EmptyClipRgn (&pdc->ecrgn);

//    uHALr_printf("ECRgn STEP1\n");
    
        pcr = pdc->lcrgn.head;
        while (pcr) {
            rc = pcr->rc;
            coor_LP2SP (pdc, &rc.left, &rc.top);
            coor_LP2SP (pdc, &rc.right, &rc.bottom);
            
            pgcr = pdc->pGCRInfo->crgn.head;
            while (pgcr) {

                if (IntersectRect (&rcInter, &rc, &pgcr->rc))
                    AddClipRect (&pdc->ecrgn, &rcInter);

//    uHALr_printf("ECRgn STEP2\n");
                pgcr = pgcr->next;
            }
            
            pcr = pcr->next;
        }
        
        if (pdc->lcrgn.head == NULL)
            ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);
//    uHALr_printf("ECRgn STEP3\n");
        // update the DevRC;
        if (pdc->bIsClient)
            WndClientRect (pdc->hwnd, &pdc->DevRC);
        else
            WndRect (pdc->hwnd, &pdc->DevRC);
            
        IntersectClipRect (&pdc->ecrgn, &pdc->DevRC);
//    uHALr_printf("ECRgn STEP4\n");
        pCtrl = Control (pdc->hwnd);
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))
            RestrictControlECRGN (&pdc->ecrgn, pCtrl);

        pdc->oldage = pdc->pGCRInfo->age;
//    uHALr_printf("ECRgn STEP5\n");
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        /*
         * Subtract the server clipping rects.
         */
        if (!mgIsServer) {
            int i;
            RECT* crcs = SHAREDRES_SCRINFO.clip_rects;

            for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
                if (!IsRectEmpty (crcs + i))
                    SubtractClipRect (&pdc->ecrgn, crcs + i);
            }

            pdc->scr_sig = SHAREDRES_SCRINFO.signature;
        }
#endif
    }

    return TRUE;
}

// This function init DC.
// set the default parameters.
static void dc_InitDC(PDC pdc, HWND hWnd, BOOL bIsClient)
{
    PCONTROL pCtrl;

    pdc->hwnd = hWnd;
    pdc->gc = PHYSICALGC;

    pdc->bkcolor = PIXEL_lightwhite;
    pdc->bkmode = 0;

    pdc->brushcolor = PIXEL_lightwhite;

    pdc->pencolor = PIXEL_black;
    pdc->CurPenPos.x = 0;
    pdc->CurPenPos.y = 0;

    pdc->textcolor = PIXEL_black;
    if (!(pdc->pLogFont = GetWindowFont (hWnd)))
        pdc->pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    pdc->tabstop = 8;
    pdc->CurTextPos.x = 0;
    pdc->CurTextPos.y = 0;
    pdc->cExtra = 0;
    pdc->alExtra = 0;
    pdc->blExtra = 0;

    pdc->mapmode = MM_TEXT;
    pdc->ViewOrig.x = 0;
    pdc->ViewOrig.y = 0;
    pdc->ViewExtent.x = 1;
    pdc->ViewExtent.y = 1;
    pdc->WindowOrig.x = 0;
    pdc->WindowOrig.y = 0;
    pdc->WindowExtent.x = 1;
    pdc->WindowExtent.y = 1;
   
    // assume that the local clip region is empty.
  
    // Get global clip region info and generate effective clip region.
    pdc->pGCRInfo = GetGCRgnInfo (hWnd);

#ifndef _LITE_VERSION
    pthread_mutex_lock (&pdc->pGCRInfo->lock);
#endif

    pdc->oldage = pdc->pGCRInfo->age;
    ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);

    if (bIsClient)
        WndClientRect (pdc->hwnd, &pdc->DevRC);
    else
        WndRect (pdc->hwnd, &pdc->DevRC);

    pdc->bIsClient = bIsClient;
    if (bIsClient)
        IntersectClipRect (&pdc->ecrgn, &pdc->DevRC);

    pCtrl = Control (pdc->hwnd);
    if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))
        RestrictControlECRGN (&pdc->ecrgn, pCtrl);

#ifndef _LITE_VERSION
    pthread_mutex_unlock (&pdc->pGCRInfo->lock);
#endif

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    /*
     * Subtract the server clipping rects.
     */
    if (!mgIsServer) {
        int i;
        RECT* crcs;
        crcs = SHAREDRES_SCRINFO.clip_rects;

        lock_scr_sem ();
        for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
            if (!IsRectEmpty (crcs + i))
                SubtractClipRect (&pdc->ecrgn, crcs + i);
        }
        pdc->scr_sig = SHAREDRES_SCRINFO.signature;
        unlock_scr_sem ();
    }
#endif
}

static void dc_Init_ScreenDC (void)
{
    __mg_screen_dc.DataType = TYPE_HDC;
    __mg_screen_dc.DCType   = TYPE_SCRDC;
    __mg_screen_dc.hwnd = 0;
    __mg_screen_dc.gc = PHYSICALGC;

    __mg_screen_dc.bkcolor = PIXEL_lightwhite;
    __mg_screen_dc.bkmode = 0;

    __mg_screen_dc.brushcolor = PIXEL_lightwhite;
    __mg_screen_dc.pencolor = PIXEL_black;
    __mg_screen_dc.CurPenPos.x = 0;
    __mg_screen_dc.CurPenPos.y = 0;

    __mg_screen_dc.textcolor = PIXEL_black;
    __mg_screen_dc.pLogFont = GetSystemFont (SYSLOGFONT_WCHAR_DEF);
    __mg_screen_dc.tabstop = 8;
    __mg_screen_dc.CurTextPos.x = 0;
    __mg_screen_dc.CurTextPos.y = 0;
    __mg_screen_dc.cExtra = 0;
    __mg_screen_dc.alExtra = 0;
    __mg_screen_dc.blExtra = 0;

    __mg_screen_dc.ViewOrig.x = 0;
    __mg_screen_dc.ViewOrig.y = 0;
    __mg_screen_dc.ViewExtent.x = 1;
    __mg_screen_dc.ViewExtent.y = 1;
    __mg_screen_dc.WindowOrig.x = 0;
    __mg_screen_dc.WindowOrig.y = 0;
    __mg_screen_dc.WindowExtent.x = 1;
    __mg_screen_dc.WindowExtent.y = 1;

    __mg_screen_dc.bIsClient = FALSE;

    // init local clip region
    InitClipRgn (&__mg_screen_dc.lcrgn, &sg_FreeClipRectList);
    // init effective clip region
    InitClipRgn (&__mg_screen_dc.ecrgn, &sg_FreeClipRectList);

    // init global clip region information
    __mg_screen_dc.pGCRInfo = NULL;
    __mg_screen_dc.oldage = 0;

    __mg_screen_dc.DevRC.left = 0;
    __mg_screen_dc.DevRC.top  = 0;
    __mg_screen_dc.DevRC.right = WIDTHOFPHYGC - 1;
    __mg_screen_dc.DevRC.bottom = HEIGHTOFPHYGC - 1;

    // Set local clip region and effetive clip region to the screen.
    SetClipRgn (&__mg_screen_dc.lcrgn, &__mg_screen_dc.DevRC);
    SetClipRgn (&__mg_screen_dc.ecrgn, &__mg_screen_dc.DevRC);
}
        
/*
 * Function: HDC GUIAPI GetClientDC(HWND hWnd)
 *     This function get the specified window client's DC.
 * Parameter:
 *     HWND hWnd: The window, 0 for screen.
 * Return:
 *     The handle of wanted DC.
 */

HDC GUIAPI GetClientDC(HWND hWnd)
{
    int i;

#ifndef _LITE_VERSION
    pthread_mutex_lock (&dcslot);
#endif
    for (i = 0; i < DCSLOTNUMBER; i++) {
        if (!DCSlot[i].inuse) {
            DCSlot[i].inuse = TRUE;
            DCSlot[i].DataType = TYPE_HDC;
            DCSlot[i].DCType   = TYPE_GENDC;
            break;
        }
    }
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&dcslot);
#endif

    if (i >= DCSLOTNUMBER)
        return HDC_SCREEN;

    dc_InitDC (DCSlot + i, hWnd, TRUE);
    return (HDC) (DCSlot + i);
}

/*
 * Function: HDC GUIAPI GetDC(HWND hWnd)
 *     This function get the specified window's DC.
 * Parameter:
 *     HWND hWnd: The window, 0 for screen.
 * Return:
 *     The handle of wanted DC.
 */

HDC GUIAPI GetDC(HWND hWnd)
{
    int i;

#ifndef _LITE_VERSION
    // allocate an empty dc slot exclusively   
    pthread_mutex_lock (&dcslot);
#endif
    for(i = 0; i < DCSLOTNUMBER; i++) {
        if(!DCSlot[i].inuse) {
            DCSlot[i].inuse = TRUE;
            DCSlot[i].DataType = TYPE_HDC;
            DCSlot[i].DCType   = TYPE_GENDC;
            break;
        }
    }
#ifndef _LITE_VERSION
    pthread_mutex_unlock(&dcslot);
#endif

    if (i >= DCSLOTNUMBER)
        return HDC_SCREEN;

    dc_InitDC(DCSlot + i, hWnd, FALSE);
    return (HDC)(DCSlot + i);
}

/*
 * Function: void GUIAPI ReleaseDC(HDC hDC)
 *     This function release the specified DC.
 * Parameter:
 *     HDC hDC: The DC handle want to release.
 * Return:
 *     None. 
 */
void GUIAPI ReleaseDC (HDC hDC)
{
    PMAINWIN pWin;
    PDC pdc;
    PCONTROL pCtrl;

    pdc = dc_HDC2PDC(hDC);

    EmptyClipRgn (&pdc->lcrgn);

    pWin = (PMAINWIN)(pdc->hwnd);
    if (pWin && pWin->privCDC == hDC) {
#ifndef _LITE_VERSION
        /* for private DC, we reset the clip region info. */
        pthread_mutex_lock (&pdc->pGCRInfo->lock);
#endif

        pdc->oldage = pdc->pGCRInfo->age;
        ClipRgnCopy (&pdc->ecrgn, &pdc->pGCRInfo->crgn);

        if (pdc->bIsClient)
            WndClientRect (pdc->hwnd, &pdc->DevRC);
        else
            WndRect (pdc->hwnd, &pdc->DevRC);

        IntersectClipRect (&pdc->ecrgn, &pdc->DevRC);

        pCtrl = Control (pdc->hwnd);
        if (pCtrl && !(pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN))
            RestrictControlECRGN (&pdc->ecrgn, pCtrl);

#ifndef _LITE_VERSION
        pthread_mutex_unlock (&pdc->pGCRInfo->lock);
#endif
    }
    else {
        EmptyClipRgn (&pdc->ecrgn);
        pdc->pGCRInfo = NULL;
        pdc->oldage = 0;

#ifndef _LITE_VERSION
        pthread_mutex_lock (&dcslot);
#endif
        pdc->inuse = FALSE;
#ifndef _LITE_VERSION
        pthread_mutex_unlock(&dcslot);
#endif
    }

}

/*
 * Function: HDC GUIAPI CreateCompatibleDC (HDC hdc)
 *     This function create a memory dc, which is compatible 
 *     with specified hdc.
 * Parameters:
 *     HDC hdc: the specified DC handle.
 * Return:
 *     None.
 */
HDC GUIAPI CreateCompatibleDC(HDC hdc)
{
    GAL_GC gc;
    PDC pdc;
    PDC pMemDC;
    
    pMemDC = NULL;
    pdc = dc_HDC2PDC(hdc);

    if (!(pMemDC = malloc (sizeof(DC)))) return HDC_INVALID;
#ifndef _LITE_VERSION
    pthread_mutex_lock (&__mg_gdilock);
#endif
    if (GAL_AllocateGC (pdc->gc, RECTW (pdc->DevRC), RECTH (pdc->DevRC), 
            GAL_BytesPerPixel (pdc->gc), &gc) != 0) {
#ifndef _LITE_VERSION
        pthread_mutex_unlock (&__mg_gdilock);
#endif
        return HDC_INVALID;
    }
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&__mg_gdilock);
#endif

    memcpy (pMemDC, pdc, sizeof(DC));

    pMemDC->DataType = TYPE_HDC;
    pMemDC->DCType   = TYPE_MEMDC;
    pMemDC->inuse = TRUE;
    pMemDC->gc = gc;

    // clip region info
    InitClipRgn (&pMemDC->lcrgn, &sg_FreeClipRectList);
    InitClipRgn (&pMemDC->ecrgn, &sg_FreeClipRectList);
    pMemDC->pGCRInfo = NULL;
    pMemDC->oldage = 0;

    pMemDC->DevRC.left = 0;
    pMemDC->DevRC.top  = 0;
    pMemDC->DevRC.right = RECTW(pdc->DevRC);
    pMemDC->DevRC.bottom = RECTH(pdc->DevRC);

    SetClipRgn (&pMemDC->ecrgn, &pMemDC->DevRC);
    
    return (HDC)pMemDC;
}

/*
 *  Function: DeleteCompatibleDC (HDC hdc)
 *      This function delete the memory DC, and free the associated memory.
 *  Parameters:
 *      HDC hdc: the DC handle want to delete.
 *  Return:
 *      FALSE: the specified HDC is not a valid memory DC.
 *      TRUE:  deleted.
 */
void GUIAPI DeleteCompatibleDC(HDC hdc)
{
    PDC pMemDC;
    
    pMemDC = dc_HDC2PDC(hdc);

    // free gl resource
    GAL_FreeGC (pMemDC->gc);

    // free clip region info
    EmptyClipRgn (&pMemDC->lcrgn);
    EmptyClipRgn (&pMemDC->ecrgn);

    // free DC slot
    free (pMemDC);
}

HDC GUIAPI CreatePrivateDC(HWND hwnd)
{
    PDC pdc;

    if (!(pdc = malloc (sizeof(DC)))) return HDC_INVALID;

    InitClipRgn (&pdc->lcrgn, &sg_FreeClipRectList);
    InitClipRgn (&pdc->ecrgn, &sg_FreeClipRectList);
    
    pdc->inuse = TRUE;
    pdc->DataType = TYPE_HDC;
    pdc->DCType   = TYPE_GENDC;
    dc_InitDC(pdc, hwnd, FALSE);
    return (HDC)(pdc);
}

HDC GUIAPI CreatePrivateClientDC(HWND hwnd)
{
    PDC pdc;

    if (!(pdc = malloc (sizeof(DC)))) return HDC_INVALID;

    InitClipRgn (&pdc->lcrgn, &sg_FreeClipRectList);
    InitClipRgn (&pdc->ecrgn, &sg_FreeClipRectList);
    
    pdc->inuse = TRUE;
    pdc->DataType = TYPE_HDC;
    pdc->DCType   = TYPE_GENDC;
    dc_InitDC(pdc, hwnd, TRUE);
    return (HDC)(pdc);
}

void GUIAPI DeletePrivateDC(HDC hdc)
{
    PDC pdc;

    pdc = (PDC)hdc;
    
    EmptyClipRgn (&pdc->lcrgn);
    EmptyClipRgn (&pdc->ecrgn);

    free (pdc);
}

HDC GUIAPI GetPrivateClientDC (HWND hwnd)
{
    PMAINWIN pWin;
    
    pWin = (PMAINWIN)hwnd;
    return pWin->privCDC;
}

