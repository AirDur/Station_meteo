/*
** $Id: cliprect.c,v 1.5 2004/04/01 08:57:13 weiym Exp $
** 
** cliprect.c: the Clip Rect module.
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002, Wei Yongming
**
** Current maintainer: Wei Yongming.
**
** Create date: 1999/3/26 by Wei Yongming.
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
#include <errno.h>

#include "common.h"
#include "gdi.h"
#include "cliprect.h"
#include "gal.h"
#include "blockheap.h"

/************************** clip region operation ****************************/
static void EvaluateBoundRect (PCLIPRGN pRgn)
{
    PCLIPRECT pCRect, pTemp;

//    uHALr_printf("EvaluateBoundRect pRgn=%p\n",pRgn);
    
    pCRect = pRgn->head;
    if (pCRect == NULL) {
        SetRectEmpty (&pRgn->rcBound);
        return;
    }

    pRgn->rcBound = pCRect->rc;
    pCRect = pCRect->next;
    while (pCRect) {
        if(pRgn->rcBound.left > pCRect->rc.left)
            pRgn->rcBound.left = pCRect->rc.left;
   
        if(pRgn->rcBound.top > pCRect->rc.top)
            pRgn->rcBound.top = pCRect->rc.top;

        if(pRgn->rcBound.right < pCRect->rc.right)
            pRgn->rcBound.right = pCRect->rc.right;

        if(pRgn->rcBound.bottom < pCRect->rc.bottom)
            pRgn->rcBound.bottom = pCRect->rc.bottom;

        pCRect = pCRect->next;
    }

    // Remove empty clip rects.
    pCRect = pRgn->head;
    while (pCRect->next) {
        if (IsRectEmpty (&pCRect->next->rc)) {
            pTemp = pCRect->next;
            pCRect->next = pTemp->next;
            FreeClipRect (pRgn->heap, pTemp);
        }
        else
            pCRect = pCRect->next;
    }

    pRgn->tail = pCRect;

    if (IsRectEmpty (&pRgn->head->rc)) {
        pTemp = pRgn->head;
        pRgn->head = pTemp->next;
        FreeClipRect (pRgn->heap, pTemp);
    }
}

void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP heap)
{
//    uHALr_printf("InitClipRgn heap=%p pRgn=%p\n",heap,pRgn);
    
     SetRectEmpty (&pRgn->rcBound);
     pRgn->head = NULL;
     pRgn->tail = NULL;

     pRgn->heap = heap;   // read-only field.
}

void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect)
{
//    uHALr_printf("GetClipRgnBoundRect pRgn=%p pRect=%p\n",pRgn,pRect);
    
     *pRect = pRgn->rcBound;
}

/* return TRUE if point is in region */
BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y)
{
    PCLIPRECT cliprect;
 
//     uHALr_printf("PtInRegion region=%p\n",region);
     
    cliprect = region->head;

    /* check with bounding rect of clipping region */
    if (!PtInRect (&region->rcBound, x, y))
        return FALSE;

    /* find the ban in which this point lies */
    cliprect = region->head;
    while (cliprect) {
        if (PtInRect (&cliprect->rc, x, y))
            return TRUE;

        cliprect = cliprect->next;
    }

    return FALSE;
}

/* Returns TRUE if rect is at least partly inside region */
BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect)
{
    PCLIPRECT cliprect;

//    uHALr_printf("RectInRegion region=%p\n",region);
    
    cliprect = region->head;
    /* check with bounding rect of clipping region */
    if (!DoesIntersect (&region->rcBound, rect))
        return FALSE;

    /* find the ban in which this point lies */
    cliprect = region->head;
    while (cliprect) {
        if (DoesIntersect (&cliprect->rc, rect))
            return TRUE;

        cliprect = cliprect->next;
    }

    return FALSE;
}

void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y)
{
    CLIPRECT* cliprect;

//    uHALr_printf("OffsetRegion region=%p\n",region);
    
    cliprect = region->head;
    
    while (cliprect) {
        OffsetRect (&cliprect->rc, x, y);
        cliprect = cliprect->next;
    }

    if (region->head) {
        OffsetRect (&region->rcBound, x, y);
    }
}

BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect)
{
    PCLIPRECT pClipRect;
    RECT rcTemp;

//    uHALr_printf("SetClipRgn pRgn=%p pRect=%p\n",pRgn,pRect);
    
    rcTemp = *pRect;

    if (IsRectEmpty (&rcTemp))
        return FALSE;

    NormalizeRect (&rcTemp);

    // empty rgn first
    EmptyClipRgn (pRgn);

    // get a new clip rect from free list
    pClipRect = ClipRectAlloc (pRgn->heap);

    // set clip rect
    pClipRect->rc = rcTemp;
    pClipRect->next = NULL;

    pRgn->head = pRgn->tail = pClipRect;
    pRgn->rcBound = rcTemp;

    return TRUE;
} 

BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn)
{
//    uHALr_printf("IsEmptyClipRgn pRgn=%p\n",pRgn);
    
     if (pRgn->head == NULL)
         return TRUE;

     return FALSE;
}

void GUIAPI EmptyClipRgn (PCLIPRGN pRgn)
{
     PCLIPRECT pCRect, pTemp;

    // uHALr_printf("EmptyClipRgn pRgn=%p pRgn->head=%p\n",pRgn,pRgn->head);
    
     pCRect = pRgn->head;
     while (pCRect) {
         // uHALr_printf("RECT %d %d %d %d\n",pCRect->rc.left,pCRect->rc.top,pCRect->rc.right,pCRect->rc.bottom);
         pTemp = pCRect->next;
         FreeClipRect (pRgn->heap, pCRect);
         pCRect = pTemp;
     }

     SetRectEmpty (&pRgn->rcBound);
     pRgn->head = NULL;
     pRgn->tail = NULL;
}

BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn)
{
    PCLIPRECT pcr;
    PCLIPRECT pnewcr;

//    uHALr_printf("ClipRgnCopy pDstRgn=%p pSrcRgn=%p\n",pDstRgn,pSrcRgn);
    
    // return false if the destination region is not a empty one.
    if (pDstRgn->head) return FALSE;

    if (!(pcr = pSrcRgn->head)) return TRUE;

    pnewcr = ClipRectAlloc (pDstRgn->heap);
    pDstRgn->head = pnewcr;
    pnewcr->rc = pcr->rc;

    while (pcr->next) {

       pcr = pcr->next;

       pnewcr->next = ClipRectAlloc (pDstRgn->heap);
       pnewcr = pnewcr->next;

       pnewcr->rc = pcr->rc;
    }

    pnewcr->next = NULL;
    pDstRgn->tail = pnewcr;

    pDstRgn->rcBound = pSrcRgn->rcBound; 

    return TRUE;
}

BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn, 
                       const CLIPRGN* pRgn1, const CLIPRGN* pRgn2)
{
    RECT rc;
    PCLIPRECT pnewcr;
    PCLIPRECT pcr1, pcr2;

//    uHALr_printf("ClipRgnIntersect pRstRgn=%p pRgn1=%p pRgn2=%p\n",pRstRgn,pRgn1,pRgn2);
    
    if (pRstRgn->head) return FALSE;
    if (!(pcr1 = pRgn1->head)) return TRUE;
    if (!(pcr2 = pRgn2->head)) return TRUE;

    pnewcr = ClipRectAlloc (pRstRgn->heap);
    pnewcr->next = NULL;
    pRstRgn->head = pnewcr;

    while (pcr1) {

        while (pcr2) {
            if (IntersectRect (&rc, &pcr1->rc, &pcr2->rc)) {

               pnewcr->rc = rc;
               pnewcr->next = ClipRectAlloc (pRstRgn->heap);
            }
            pcr2 = pcr2->next;
        }

        pcr2 = pRgn2->head;
        pcr1 = pcr1->next;
    }

    pRstRgn->tail = pnewcr;
    if (pnewcr->next)
    {
        FreeClipRect(pRstRgn->heap, pnewcr->next);
        pnewcr->next = NULL;
    }

    EvaluateBoundRect(pRstRgn);

    return TRUE;
}

BOOL GUIAPI AddClipRect (PCLIPRGN pRgn, const RECT* pRect)
{
     PCLIPRECT pClipRect;
     RECT rcTemp;

//    uHALr_printf("AddClipRect pRgn=%p pRect=%p\n",pRgn,pRect);
    
     rcTemp = *pRect;

     if (IsRectEmpty (&rcTemp))
         return FALSE;

     NormalizeRect (&rcTemp);

     pClipRect = ClipRectAlloc (pRgn->heap);
     pClipRect->rc = rcTemp;
     pClipRect->next = NULL;

     if (pRgn->head) {
         pRgn->tail->next = pClipRect;
         pRgn->tail = pClipRect;
     }
     else {
         pRgn->head = pRgn->tail = pClipRect;
     }

     return TRUE;
}

BOOL GUIAPI IntersectClipRect (PCLIPRGN pRgn, const RECT* pRect)
{
    PCLIPRECT pCRect;
    RECT rcTemp;
 
//     uHALr_printf("IntersectClipRect pRgn=%p pRect=%p\n",pRgn,pRect);
     
    if (IsRectEmpty (pRect)) {
        EmptyClipRgn (pRgn);
        return TRUE;
    }

    rcTemp = *pRect;
    NormalizeRect (&rcTemp);

    // intersect
    pCRect = pRgn->head;
    while (pCRect) {
        if (!IntersectRect (&pCRect->rc, &pCRect->rc, &rcTemp))
            SetRectEmpty(&pCRect->rc);

        pCRect = pCRect->next;
    }

    EvaluateBoundRect (pRgn);

    return TRUE;
}

BOOL GUIAPI SubtractClipRect (PCLIPRGN pRgn, const RECT* pRect)
{
    PCLIPRECT pCRect, pSaved, pTemp;
    RECT rc[4], rcTemp, rcInflated;
    int nCount;
    PRECT src, erc;
    int i;

//    uHALr_printf("SubtractClipRect pRgn=%p Rect=%p\n",pRgn,pRect);
    if (IsRectEmpty (pRect))
        return FALSE;

    rcInflated = *pRect;
    erc = &rcInflated;

    NormalizeRect (erc);

    if (!DoesIntersect(&pRgn->rcBound, erc)) {
        return FALSE;
    }

    pCRect = pRgn->head;
    while (pCRect) {

        src = &pCRect->rc;
        if (!IntersectRect(&rcTemp, src, erc)) {
            pCRect = pCRect->next;
            continue;
        }

        pSaved = pCRect->next;

        nCount = 0;
        if(erc->top > src->top)
        {
            rc[nCount].left  = src->left;
            rc[nCount].top   = src->top;
            rc[nCount].right = src->right;
            rc[nCount].bottom = erc->top;
            nCount++;

            src->top = erc->top;
        }

        if(erc->bottom < src->bottom)
        {
            rc[nCount].top  = erc->bottom;
            rc[nCount].left   = src->left;
            rc[nCount].right = src->right;
            rc[nCount].bottom = src->bottom;
            nCount++;
 
            src->bottom = erc->bottom;
        }

        if(erc->left > src->left)
        {
            rc[nCount].left  = src->left;
            rc[nCount].top   = src->top;
            rc[nCount].right = erc->left;
            rc[nCount].bottom = src->bottom;
            nCount++;
        }

        if(erc->right < src->right)
        {
            rc[nCount].left  = erc->right;
            rc[nCount].top   = src->top;
            rc[nCount].right = src->right;
            rc[nCount].bottom = src->bottom;
            nCount++;
        }

        if (nCount == 0)
            SetRectEmpty (&pCRect->rc);
        else if(nCount > 0)
            pCRect->rc = rc[0];

        for(i = 1; i<nCount; i++)
        {
            pTemp = ClipRectAlloc (pRgn->heap); 
            pTemp->rc = rc[i];
            pCRect->next = pTemp;
            pCRect = pTemp;
        }

        pCRect->next = pSaved;

        pCRect = pSaved;
    }

    EvaluateBoundRect (pRgn);

    return TRUE;
}

