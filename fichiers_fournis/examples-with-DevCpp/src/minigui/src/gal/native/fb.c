/*
** $Id: fb.c,v 1.13 2003/11/22 11:49:29 weiym Exp $
** 
** fb.c: Screen Driver Utilities
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 Song Lixin and Wei Yongming.
**
** Portions from Greg Haerr's MicroWindows.
** Copyright (c) 1999, 2000 Greg Haerr <greg@censoft.com>
** 
** Create Date: 2000/10/20 by Song Lixin.
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

#include "native.h"
#include "fb.h"

/* select a framebuffer subdriver based on planes and bpp */
/* modify this procedure to add a new framebuffer subdriver */

#ifdef _FBLIN1R_SUPPORT
extern SUBDRIVER fblinear1;
#endif
#ifdef _FBLIN1L_SUPPORT
extern SUBDRIVER fblinear_1;
#endif

#ifdef _FBLIN2R_SUPPORT
extern SUBDRIVER fblinear2;
#endif
#ifdef _FBLIN2L_SUPPORT
extern SUBDRIVER fblinear_2;
#endif

#ifdef _FBLIN4R_SUPPORT
extern SUBDRIVER fblinear4;
#endif
#ifdef _FBLIN4L_SUPPORT
extern SUBDRIVER fblinear_4;
#endif

#ifdef _FBLIN8_SUPPORT
extern SUBDRIVER fblinear8;
#endif

#ifdef _FBLIN16_SUPPORT
extern SUBDRIVER fblinear16;
#endif

#ifdef _FBLIN24_SUPPORT
extern SUBDRIVER fblinear24;
#endif

#ifdef _FBLIN32_SUPPORT
extern SUBDRIVER fblinear32;
#endif

#ifdef _FBVGA16_SUPPORT
extern SUBDRIVER fbvga16;
#endif

PSUBDRIVER select_fb_subdriver (PSD psd)
{
    PSUBDRIVER  driver;

    driver = NULL;
    
#ifdef _FBVGA16_SUPPORT
    /* FB_TYPE_VGA_PLANES */
    /* device and memory drivers are not same for vga16*/
    if (psd->planes == 4 && psd->bpp == 4) {
        driver = &fbvga16;
    }
#endif

    /* FB_TYPE_PACKED_PIXELS */
    /* device and memory drivers are the same for packed pixels*/
    if (psd->planes == 1) {
        switch (psd->bpp) {
        case 1:
            if (psd->flags & PSF_MSBRIGHT)
#ifdef _FBLIN1R_SUPPORT
                driver = &fblinear1;
#else
                driver = NULL;
#endif

            else
#ifdef _FBLIN1L_SUPPORT
                driver = &fblinear_1;
#else
                driver = NULL;
#endif
        break;

        case 2:
            if (psd->flags & PSF_MSBRIGHT)
#ifdef _FBLIN2R_SUPPORT
                driver = &fblinear2;
#else
                driver = NULL;
#endif
            else
#ifdef _FBLIN2L_SUPPORT
                driver = &fblinear_2;
#else
                driver = NULL;
#endif
        break;

        case 4:
            if (psd->flags & PSF_MSBRIGHT)
#ifdef _FBLIN4R_SUPPORT
                driver = &fblinear4;
#else
                driver = NULL;
#endif
            else
#ifdef _FBLIN4L_SUPPORT
                driver = &fblinear_4;
#else
                driver = NULL;
#endif
        break;

#ifdef _FBLIN8_SUPPORT
        case 8:
            driver = &fblinear8;
        break;
#endif

#ifdef _FBLIN16_SUPPORT
        case 16:
            driver = &fblinear16;
        break;
#endif

#ifdef _FBLIN24_SUPPORT
        case 24:
            driver = &fblinear24;
        break;
#endif

#ifdef _FBLIN32_SUPPORT
        case 32:
            driver = &fblinear32;
        break;
#endif
        }
    }

#ifdef _DEBUG
    uHALr_printf("PSD info: linelen %d xres %d yres %d\n", psd->linelen, psd->xres, psd->yres);
#endif
    /* return driver selected*/
    return driver;
}

/* 
 * Initialize memory device with passed parms,
 * select suitable framebuffer subdriver,
 * and set subdriver in memory device.
 */
int fb_mapmemgc (PSD mempsd,int w,int h,int planes,int bpp, int linelen,int size,void *addr)
{
    PSUBDRIVER subdriver;

    /* initialize mem screen driver */
    native_gen_initmemgc (mempsd, w, h, planes, bpp, linelen, size, addr);

    /* select the framebuffer subdriver */
    subdriver = select_fb_subdriver (mempsd);
    if(!subdriver)
        return 0;

    /* set and initialize subdriver into mem screen driver */
    if (!set_subdriver (mempsd, subdriver, TRUE))
        return 0;

    return 1;
}

/*
 * Set subdriver entry points in screen device
 * Initialize subdriver if init flag is TRUE
 * Return 0 on fail
 */
int set_subdriver(PSD psd, PSUBDRIVER subdriver, int init)
{
    /* set subdriver entry points in screen driver*/
    psd->DrawPixel  = subdriver->DrawPixel;
    psd->ReadPixel  = subdriver->ReadPixel;
    psd->DrawHLine  = subdriver->DrawHLine;
    psd->DrawVLine  = subdriver->DrawVLine;
    psd->Blit       = subdriver->Blit;
    psd->PutBox     = subdriver->PutBox;
    psd->GetBox     = subdriver->GetBox;
    psd->PutBoxMask = subdriver->PutBoxMask;
    psd->CopyBox    = subdriver->CopyBox;

    /* call driver init procedure to calc map size and linelen*/
    if (init && !subdriver->Init(psd))
        return 0;
    return 1;
}

/* fill in a subdriver struct from passed screen device*/
void get_subdriver(PSD psd, PSUBDRIVER subdriver)
{
    /* set subdriver entry points in screen driver*/
    subdriver->DrawPixel  = psd->DrawPixel;
    subdriver->ReadPixel  = psd->ReadPixel;
    subdriver->DrawHLine  = psd->DrawHLine;
    subdriver->DrawVLine  = psd->DrawVLine;
    subdriver->Blit       = psd->Blit;
    subdriver->PutBox     = psd->PutBox;
    subdriver->GetBox     = psd->GetBox;
    subdriver->PutBoxMask = psd->PutBoxMask;
    subdriver->CopyBox    = psd->CopyBox;
}

