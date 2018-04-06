/* $Id: map.c,v 1.6 2003/09/04 03:09:52 weiym Exp $
**
** Mapping operations of GDI
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
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

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

/****************************** Mapping support ******************************/
int GUIAPI GetMapMode(HDC hdc)
{
    PDC pdc;
    int mode;

    pdc = dc_HDC2PDC (hdc);
    mode = pdc->mapmode;

    return mode;
}

void GUIAPI GetViewportExt(HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    *pPt = pdc->ViewExtent;
}

void GUIAPI GetViewportOrg(HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    *pPt = pdc->ViewOrig;
}

void GUIAPI GetWindowExt(HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    *pPt = pdc->WindowExtent;
}

void GUIAPI GetWindowOrg(HDC hdc, POINT* pPt)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);
    *pPt = pdc->WindowExtent;
}

void GUIAPI SetMapMode(HDC hdc, int mapmode)
{
    PDC pdc;

    if( hdc == 0 ) return;

    pdc = dc_HDC2PDC(hdc);
    pdc->mapmode = mapmode;
}

void GUIAPI SetViewportExt(HDC hdc, POINT* pPt)
{
    PDC pdc;

    if( hdc == 0 ) return;

    pdc = dc_HDC2PDC(hdc);
    pdc->ViewExtent = *pPt;
}

void GUIAPI SetViewportOrg(HDC hdc, POINT* pPt)
{
    PDC pdc;

    if( hdc == 0 ) return;

    pdc = dc_HDC2PDC(hdc);
    pdc->ViewOrig = *pPt;
}

void GUIAPI SetWindowExt(HDC hdc, POINT* pPt)
{
    PDC pdc;

    if( hdc == 0 ) return;

    pdc = dc_HDC2PDC(hdc);
    pdc->WindowExtent = *pPt;
}

void GUIAPI SetWindowOrg(HDC hdc, POINT* pPt)
{
    PDC pdc;

    if( hdc == 0 ) return;

    pdc = dc_HDC2PDC(hdc);
    pdc->WindowOrig = *pPt;
}


