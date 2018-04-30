/*
** $Id: attr.c,v 1.11 2003/12/23 12:39:33 weiym Exp $
**
** Drawing attributes of GDI
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/12, derived from original gdi.c.
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
#include "gal.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

/******************* General drawing attributes *******************************/
gal_pixel GUIAPI GetBkColor (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    return pdc->bkcolor;
}

int GUIAPI GetBkMode(HDC hdc)
{

    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    return pdc->bkmode;
}

gal_pixel GUIAPI GetTextColor(HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    return pdc->textcolor;
}

gal_pixel GUIAPI SetBkColor (HDC hdc, gal_pixel color)
{
    gal_pixel oldcolor;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldcolor = pdc->bkcolor;

    pdc->bkcolor = color;

    return oldcolor;
} 

int GUIAPI SetBkMode (HDC hdc, int bkmode)
{

    int oldmode;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldmode = pdc->bkmode;

    pdc->bkmode = bkmode;

    return oldmode;
}

gal_pixel GUIAPI SetTextColor (HDC hdc, gal_pixel color)
{
    gal_pixel oldcolor;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldcolor = pdc->textcolor;

    pdc->textcolor = color;

    return oldcolor;
}

int GUIAPI GetTabStop (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    return pdc->tabstop;
}

int GUIAPI SetTabStop (HDC hdc, int new_value)
{
    int old_value;
    PDC pdc;

    pdc = dc_HDC2PDC(hdc);

    old_value = pdc->tabstop;

    if (new_value > 0)
        pdc->tabstop = new_value;
    
    return old_value;
}

/************************** Pen and brush support ****************************/
gal_pixel GUIAPI GetPenColor (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    return pdc->pencolor;
}

gal_pixel GUIAPI GetBrushColor (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    return pdc->brushcolor;
}

gal_pixel GUIAPI SetPenColor (HDC hdc, gal_pixel color)
{
    gal_pixel oldvalue;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldvalue = pdc->pencolor;
    pdc->pencolor = color;

    return oldvalue;
}

gal_pixel GUIAPI SetBrushColor (HDC hdc, gal_pixel color)
{
    gal_pixel oldvalue;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldvalue = pdc->brushcolor;
    pdc->brushcolor = color;

    return oldvalue;
}

int GUIAPI GetTextCharacterExtra (HDC hdc)
{
    return dc_HDC2PDC (hdc)->cExtra;
}

int GUIAPI SetTextCharacterExtra (HDC hdc, int extra)
{
    int oldvalue;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldvalue = pdc->cExtra;
    pdc->cExtra = extra;

    return oldvalue;
}

int GUIAPI GetTextAboveLineExtra (HDC hdc)
{
    return dc_HDC2PDC (hdc)->alExtra;
}

int GUIAPI SetTextAboveLineExtra (HDC hdc, int extra)
{
    int oldvalue;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldvalue = pdc->alExtra;
    pdc->alExtra = extra;

    return oldvalue;
}

int GUIAPI GetTextBellowLineExtra (HDC hdc)
{
    return dc_HDC2PDC (hdc)->blExtra;
}

int GUIAPI SetTextBellowLineExtra (HDC hdc, int extra)
{
    int oldvalue;
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);

    oldvalue = pdc->blExtra;
    pdc->blExtra = extra;

    return oldvalue;
}

