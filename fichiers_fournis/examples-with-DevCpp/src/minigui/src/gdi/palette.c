/*
** $Id: palette.c,v 1.5 2003/09/04 03:09:52 weiym Exp $
**
** palette.c: Palette operations of GDI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2001/08/02
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
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "dc.h"

int GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    GAL_SetGC (pdc->gc);
    return GAL_GetPalette (pdc->gc, start, len, cmap);
}

int GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    GAL_SetGC (pdc->gc);
    return GAL_SetPalette (pdc->gc, start, len, cmap);
}

int GUIAPI SetColorfulPalette (HDC hdc)
{
    PDC pdc;

    pdc = dc_HDC2PDC (hdc);
    GAL_SetGC (pdc->gc);
    return GAL_SetColorfulePalette (pdc->gc);
}

