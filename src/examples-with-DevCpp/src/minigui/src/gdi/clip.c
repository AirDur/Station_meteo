/*
** $Id: clip.c,v 1.10.6.2 2005/02/16 07:46:46 weiym Exp $
**
** Clipping operations of GDI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c
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

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"
#include "gdidecl.h"

#ifndef _LITE_VERSION
extern BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);
#define LOCK_RGN() \
        pthread_mutex_lock (&pdc->pGCRInfo->lock);
#define UNLOCK_RGN() \
        pthread_mutex_unlock (&pdc->pGCRInfo->lock);
#else

#define LOCK_RGN()
#ifndef _STAND_ALONE
#define UNLOCK_RGN() \
        if (!mgIsServer) unlock_scr_sem ();
#else
#define UNLOCK_RGN()
#endif

#endif



/************************* Clipping support **********************************/
void GUIAPI ExcludeClipRect(HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

//    uHALr_printf("ExcludeClipRect prc=%p\n",prc);
    rc = *prc;
    pdc = dc_HDC2PDC(hdc);

    NormalizeRect(&rc);
    if (IsRectEmpty (&rc)) return;

    SubtractClipRect (&pdc->lcrgn, &rc);

    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &rc.left, &rc.top);
    coor_LP2SP(pdc, &rc.right, &rc.bottom);
    NormalizeRect(&rc);

    SubtractClipRect (&pdc->ecrgn, &rc);
}

void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

//    uHALr_printf("IncludeClipRect prc=%p\n",prc);
    rc = *prc;
    pdc = dc_HDC2PDC(hdc);

    NormalizeRect(&rc);
    if (IsRectEmpty (&rc)) return;

    SubtractClipRect (&pdc->lcrgn, &rc);
    AddClipRect (&pdc->lcrgn, &rc);
    
    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &rc.left, &rc.top);
    coor_LP2SP(pdc, &rc.right, &rc.bottom);
    NormalizeRect(&rc);

    if (dc_IsGeneralHDC(hdc)) {
        LOCK_RGN();
        dc_GenerateECRgn (pdc, TRUE);
        UNLOCK_RGN();
    }
    else {
        SubtractClipRect (&pdc->ecrgn, &rc);
        IntersectClipRect (&pdc->ecrgn, &rc);
    }
}

void GUIAPI ClipRectIntersect(HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;
    
//    uHALr_printf("ClipRectIntersect prc=%p\n",prc);
    pdc = dc_HDC2PDC(hdc);

    rc = *prc;
    NormalizeRect(&rc);
    if (IsRectEmpty (&rc)) return;

    IntersectClipRect (&pdc->lcrgn, &rc);

    // Transfer logical to device to screen here.
    coor_LP2SP(pdc, &rc.left, &rc.top);
    coor_LP2SP(pdc, &rc.right, &rc.bottom);
    NormalizeRect(&rc);

    IntersectClipRect (&pdc->ecrgn, &rc);
}

void GUIAPI SelectClipRect (HDC hdc, const RECT* prc)
{
    PDC pdc;
    RECT rc;

//    uHALr_printf("SelectClipRect prc=%p\n",prc);
    pdc = dc_HDC2PDC(hdc);

    if (prc) {
        rc = *prc;
        NormalizeRect(&rc);
        if (IsRectEmpty (&rc)) return;

        SetClipRgn (&pdc->lcrgn, &rc);
        coor_LP2SP(pdc, &rc.left, &rc.top);
        coor_LP2SP(pdc, &rc.right, &rc.bottom);
        NormalizeRect(&rc);
        
        if (!IntersectRect (&rc, &rc, &pdc->DevRC))
            return;
    }
    else {
        EmptyClipRgn (&pdc->lcrgn);
        rc = pdc->DevRC;
    }
    
    if (dc_IsGeneralHDC(hdc)) {
        LOCK_RGN();
        dc_GenerateECRgn (pdc, TRUE);
        UNLOCK_RGN();
    }
    else
        SetClipRgn (&pdc->ecrgn, &rc);
}

void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn)
{
    PDC pdc;
    PCLIPRECT pCRect;
    PCLIPRGN pLcrgn;

//    uHALr_printf("SelectClipRegion pRgn=%p\n",pRgn);
    pdc = dc_HDC2PDC(hdc);
    pLcrgn = &pdc->lcrgn;

    pCRect = pRgn->head;
    if (pCRect) {
        SetClipRgn (pLcrgn, &pCRect->rc);

        pCRect = pCRect->next;
        while (pCRect) {
            SubtractClipRect (pLcrgn, &pCRect->rc);
            AddClipRect (pLcrgn, &pCRect->rc);
            pCRect = pCRect->next;
        }
    }
    else
        return;

    if (dc_IsGeneralHDC(hdc)) {
        LOCK_RGN();
        dc_GenerateECRgn (pdc, TRUE);
        UNLOCK_RGN();
    }
    else {
        // not implemented.
    }

}

void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect)
{
    PDC pdc;

//    uHALr_printf("GetBoundsRect pRect=%p\n",pRect);
    pdc = dc_HDC2PDC(hdc);

    *pRect = pdc->lcrgn.rcBound;
}

BOOL GUIAPI PtVisible (HDC hdc, int x, int y)
{
    PCLIPRECT pClipRect;
    PDC pdc;

//    uHALr_printf("PtVisible\n");
    pdc = dc_HDC2PDC(hdc);

    pClipRect = pdc->lcrgn.head;
    if (pClipRect == NULL) {
        RECT rc = pdc->DevRC;
        coor_SP2LP (pdc, &rc.left, &rc.top);
        coor_SP2LP (pdc, &rc.right, &rc.bottom);
        return PtInRect (&rc, x, y);
    }
        
    while (pClipRect) {
        if (PtInRect (&(pClipRect->rc), x, y)) return TRUE;

        pClipRect = pClipRect->next;
    }

    return FALSE;
}

BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect)
{
    PCLIPRECT pClipRect;
    PDC pdc;

//    uHALr_printf("RectVisible pRect=%p\n",pRect);
    pdc = dc_HDC2PDC(hdc);

    pClipRect = pdc->lcrgn.head;
    if (pClipRect == NULL) {
        RECT rc = pdc->DevRC;
        coor_SP2LP (pdc, &rc.left, &rc.top);
        coor_SP2LP (pdc, &rc.right, &rc.bottom);
        return DoesIntersect (&rc, pRect);
    }

    while (pClipRect) {
        if (DoesIntersect (pRect, &(pClipRect->rc)))
            return TRUE;

        pClipRect = pClipRect->next;
    }

    return FALSE;
}

