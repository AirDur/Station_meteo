/*
** $Id: gal.c,v 1.41 2005/01/07 03:46:23 weiym Exp $
** 
** The Graphics Abstract Layer of MiniGUI.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000, 2001, 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/11
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
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gal.h"

#ifdef _NATIVE_GAL_ENGINE
#include "native/native.h"
#endif

#ifdef _VGA16_GAL
    #include "vga16.h"
#endif
#ifdef _SVGALIB
    #include "svgalib.h"
#endif
#ifdef _LIBGGI
    #include "libggi.h"
#endif
#ifdef _EP7211_GAL
	#include "ep7211.h"
#endif
#ifdef _ADS_GAL
	#include "ads.h"
#endif


static GFX gfxes [] = 
{
#ifdef _NATIVE_GAL_ENGINE
#ifdef _NATIVE_GAL_FBCON
    {"fbcon", InitNative, TermNative},
#endif
#ifdef _NATIVE_GAL_QVFB
    {"qvfb", InitNative, TermNative},
#endif
#ifdef _NATIVE_GAL_COMMLCD
    {"commlcd", InitNative, TermNative},
#endif
#endif
#ifdef _VGA16_GAL
    {"VGA16", InitVGA16, TermVGA16},
#endif
#ifdef _SVGALIB
    {"SVGALib", InitSVGALib, TermSVGALib},
#endif
#ifdef _LIBGGI
    {"LibGGI", InitLibGGI, TermLibGGI},
#endif
#ifdef _EP7211_GAL
	{"EP7211",InitEP7211, TermEP7211},
#endif
#ifdef _ADS_GAL
	{"ADS", InitADS, TermADS},
#endif
};

GFX* __mg_cur_gfx;

#define NR_GFXES  (sizeof (gfxes) / sizeof (GFX))

#define LEN_ENGINE_NAME  16

int InitGAL (void)
{
    __mg_cur_gfx =  gfxes;
    if (!GAL_InitGfx (__mg_cur_gfx)) {
        uHALr_printf("GAL: Init GAL engine failure: %s.\n", __mg_cur_gfx->id);
        return ERR_GFX_ENGINE;
    }
    return 0;
}

void TerminateGAL (void)
{
    GAL_TermGfx (__mg_cur_gfx);
}

