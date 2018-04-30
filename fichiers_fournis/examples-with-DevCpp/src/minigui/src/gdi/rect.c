/* 
** $Id: rect.c,v 1.10 2003/09/04 03:09:52 weiym Exp $
**
** Rectangle operations of GDI.
**
** Copyright (C) 2000, Wei Yongming.
**
** Current maintainer: Wei Yongming.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/**************************** Rectangle support ******************************/
void GUIAPI SetRect(RECT* prc, int left, int top, int right, int bottom)
{
    prc->left = left;
    prc->top = top;
    prc->right = right;
    prc->bottom = bottom;
}

void GUIAPI SetRectEmpty(RECT* prc)
{
    memset(prc, 0, sizeof(RECT));
}

void GUIAPI CopyRect(RECT* pdrc, const RECT* psrc)
{
    memcpy(pdrc, psrc, sizeof(RECT));
}

BOOL GUIAPI IsRectEmpty(const RECT* prc)
{
    if( prc->left == prc->right ) return TRUE;
    if( prc->top == prc->bottom ) return TRUE;
    return FALSE;
}

BOOL GUIAPI EqualRect(const RECT* prc1, const RECT* prc2)
{
    if(prc1->left != prc2->left) return FALSE;
    if(prc1->top != prc2->top) return FALSE;
    if(prc1->right != prc2->right) return FALSE;
    if(prc1->bottom != prc2->bottom) return FALSE;

    return TRUE;
}

void GUIAPI NormalizeRect(RECT* pRect)
{
    int iTemp;

    if(pRect->left > pRect->right)
    {
         iTemp = pRect->left;
         pRect->left = pRect->right;
         pRect->right = iTemp;
    }

    if(pRect->top > pRect->bottom)
    {
         iTemp = pRect->top;
         pRect->top = pRect->bottom;
         pRect->bottom = iTemp;
    }
}

BOOL GUIAPI IsCovered(const RECT* prc1, const RECT* prc2)
{
    if (prc1->left < prc2->left
            || prc1->top < prc2->top
            || prc1->right > prc2->right
            || prc1->bottom > prc2->bottom)
        return FALSE;

    return TRUE;
}

BOOL GUIAPI IntersectRect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
{

    pdrc->left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
    pdrc->top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
    pdrc->right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    pdrc->bottom = (psrc1->bottom < psrc2->bottom) 
                   ? psrc1->bottom : psrc2->bottom;

    if(pdrc->left >= pdrc->right || pdrc->top >= pdrc->bottom)
        return FALSE;

    return TRUE;
}

BOOL GUIAPI DoesIntersect (const RECT* psrc1, const RECT* psrc2)
{
    int left, top, right, bottom;
    
//    uHALr_printf("DoesIntersect BEGIN\n");
//    uHALr_printf("RECT1 %d,%d,%d,%d\n",psrc1->left,psrc1->top,psrc1->right,psrc1->bottom);
//    uHALr_printf("RECT2 %d,%d,%d,%d\n",psrc2->left,psrc2->top,psrc2->right,psrc2->bottom);
    left = (psrc1->left > psrc2->left) ? psrc1->left : psrc2->left;
    top  = (psrc1->top > psrc2->top) ? psrc1->top : psrc2->top;
    right = (psrc1->right < psrc2->right) ? psrc1->right : psrc2->right;
    bottom = (psrc1->bottom < psrc2->bottom) 
                   ? psrc1->bottom : psrc2->bottom;
//    uHALr_printf("DoesIntersect %d %d %d %d END\n",left,top,right,bottom);
    
    if(left >= right || top >= bottom)
        return FALSE;

    return TRUE;
}

BOOL GUIAPI UnionRect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
{
    RECT src1, src2;
    memcpy(&src1, psrc1, sizeof(RECT));
    memcpy(&src2, psrc2, sizeof(RECT));

    NormalizeRect(&src1);
    NormalizeRect(&src2);

    if (src1.left == src2.left 
        && src1.right == src2.right) {
        if (src1.top <= src2.top && src2.top <= src1.bottom) {
            pdrc->left = src1.left;
            pdrc->right = src1.right;
            pdrc->top = src1.top;
            pdrc->bottom = MAX(src1.bottom, src2.bottom);

            return TRUE;
        }
        else if (src1.top >= src2.top && src2.bottom >= src1.top) {
            pdrc->left = src1.left;
            pdrc->right = src1.right;
            pdrc->top = src2.top;
            pdrc->bottom = MAX(src1.bottom, src2.bottom);

            return TRUE;
       }

       return FALSE;
    }

    if (src1.top == src2.top 
        && src1.bottom == src2.bottom) {
        if (src1.left <= src2.left && src2.left <= src1.right) {
            pdrc->top = src1.top;
            pdrc->bottom = src1.bottom;
            pdrc->left = src1.left;
            pdrc->right = MAX(src1.right, src2.right);

            return TRUE;
        }
        else if (src1.left >= src2.left && src2.right >= src1.left) {
            pdrc->top = src1.top;
            pdrc->bottom = src1.bottom;
            pdrc->left = src2.left;
            pdrc->right = MAX(src1.right, src2.right);

            return TRUE;
       }

       return FALSE;
    }

    return FALSE;
}

void GUIAPI GetBoundRect(PRECT pdrc,  const RECT* psrc1, const RECT* psrc2)
{
    RECT src1, src2;
    memcpy(&src1, psrc1, sizeof(RECT));
    memcpy(&src2, psrc2, sizeof(RECT));

    NormalizeRect(&src1);
    NormalizeRect(&src2);

    pdrc->left = (src1.left < src2.left) ? src1.left : src2.left;
    pdrc->top  = (src1.top < src2.top) ? src1.top : src2.top;
    pdrc->right = (src1.right > src2.right) ? src1.right : src2.right;
    pdrc->bottom = (src1.bottom > src2.bottom) 
                   ? src1.bottom : src2.bottom;
}

int GUIAPI SubtractRect(RECT* rc, const RECT* psrc1, const RECT* psrc2)
{
    RECT src, rcExpect, *prcExpect;
    int nCount;
    
    nCount = 0;
    src = *psrc1;
    rcExpect = *psrc2;
    prcExpect = &rcExpect;

    if (!DoesIntersect (&src, prcExpect)) {
        nCount = 1;
        rc[0] = src;
    }
    else {
        if(prcExpect->top > src.top)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = prcExpect->top;
            nCount++;
            src.top = prcExpect->top;
        }
        if(prcExpect->bottom < src.bottom)
        {
            rc[nCount].top  = prcExpect->bottom;
            rc[nCount].left   = src.left;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
            src.bottom = prcExpect->bottom;
        }
        if(prcExpect->left > src.left)
        {
            rc[nCount].left  = src.left;
            rc[nCount].top   = src.top;
            rc[nCount].right = prcExpect->left;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }
        if(prcExpect->right < src.right)
        {
            rc[nCount].left  = prcExpect->right;
            rc[nCount].top   = src.top;
            rc[nCount].right = src.right;
            rc[nCount].bottom = src.bottom;
            nCount++;
        }
    }

    return nCount;
}

void GUIAPI OffsetRect(RECT* prc, int x, int y)
{
    prc->left += x;
    prc->top += y;
    prc->right += x;
    prc->bottom += y;
}

void GUIAPI InflateRect(RECT* prc, int cx, int cy)
{
    prc->left -= cx;
    prc->top -= cy;
    prc->right += cx;
    prc->bottom += cy;
}

void GUIAPI InflateRectToPt (RECT* prc, int x, int y)
{
    if (x < prc->left) prc->left = x;
    if (y < prc->top) prc->top = y;
    if (x > prc->right) prc->right = x;
    if (y > prc->bottom) prc->bottom = y;
}

BOOL GUIAPI PtInRect(const RECT* prc, int x, int y)
{
    if (   x >= prc->left 
        && x < prc->right 
        && y >= prc->top 
        && y < prc->bottom) 
        return TRUE;
    
    return FALSE;
}

