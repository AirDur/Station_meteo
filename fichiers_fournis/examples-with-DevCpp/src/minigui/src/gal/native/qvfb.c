/*
** $Id: qvfb.c,v 1.7 2003/11/22 11:49:29 weiym Exp $
** 
** qvfb.c: GAL driver for Qt Virtual FrameBuffer.
** 
** Copyright (C) 2003 Feynman Software.
**
** Create Date: 2003/07/04 by Wei Yongming
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
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>

#include "native.h"

#ifdef _NATIVE_GAL_QVFB

#include "misc.h"
#include "fb.h"
#include "qvfb.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

static int display;
static int lockid = -1;

static void init_lock (void)
{
    const char *logname = getenv("LOGNAME");
    char pipe [256];

    if (!logname)
        logname = "unknown";

    sprintf (pipe, "/tmp/qtembedded-%s/%s-%d", logname, QTE_PIPE, display);

    lockid = semget (ftok (pipe, 'd'), 0, 0);
}

static void lock (void)
{
    struct sembuf sops;
    int rv;

    if (lockid == -1)
        init_lock ();

    sops.sem_num = 0;
    sops.sem_flg = SEM_UNDO;
    sops.sem_op = -1;

    do {
        rv = semop (lockid, &sops, 1);
    } while (rv == -1 && errno == EINTR);

    if (rv == -1)
        lockid = -1;
}

static void unlock (void)
{
    if (lockid >= 0) {
        int rv;
        struct sembuf sops;

        sops.sem_num = 0;
        sops.sem_op = 1;
        sops.sem_flg = SEM_UNDO;
        do {
            rv = semop(lockid,&sops,1);
        } while ( rv == -1 && errno == EINTR );
    }
}

/* init framebuffer*/
static PSD fb_open(PSD psd)
{
    char file [50];
    key_t key;
    int shmid;
    struct QVFbHeader* hdr;
    PSUBDRIVER subdriver;

    if (GetMgEtcIntValue ("qvfb", "display", &display) < 0)
        return NULL;

    sprintf (file, QT_VFB_MOUSE_PIPE, display);
    key = ftok (file, 'b');

    shmid = shmget (key, 0, 0);
    if (shmid == -1)
        return NULL;

    hdr = (struct QVFbHeader *) shmat (shmid, 0, 0);
    if ((int) hdr == -1 || hdr == NULL)
        return NULL;

    init_lock ();

    psd->xres = psd->xvirtres = hdr->width;
    psd->yres = psd->yvirtres = hdr->height;
    psd->planes = 1;

    psd->bpp = hdr->depth;
    psd->ncolors = (psd->bpp >= 24) ? (1 << 24) : (1 << psd->bpp);

    /* set linelen to byte length, possibly converted later */
    psd->linelen = hdr->linestep;

    psd->size = 0;                /* force subdriver init of size */

    psd->flags = PSF_MEMORY;
    psd->flags |= PSF_MSBRIGHT; /* the most significant bit is right */

    /* set pixel format*/
    if (psd->bpp >= 8) {
        switch(psd->bpp) {
            case 8:
                psd->pixtype = PF_TRUECOLOR332;
                break;
            case 16:
                psd->pixtype = PF_TRUECOLOR565;
                break;
            case 24:
                psd->pixtype = PF_TRUECOLOR888;
                break;
            case 32:
                psd->pixtype = PF_TRUECOLOR0888;
                break;
            default:
                fprintf(stderr, "GAL qvfb engine: Unsupported FrameBuffer\n");
                goto fail;
        }
    }
    else
        psd->pixtype = PF_PALETTE;

    /* select a framebuffer subdriver based on planes and bpp*/
    subdriver = select_fb_subdriver (psd);
    if (!subdriver) {
        fprintf(stderr,"GAL qvfb engine: No driver for bpp %d\n", psd->bpp);
        goto fail;
    }

    /*
     * set and initialize subdriver into screen driver
     * psd->size is calculated by subdriver init
     */
    if (!set_subdriver (psd, subdriver, TRUE)) {
        fprintf(stderr,"GAL qvfb engine: Driver initialize failed for bpp %d\n", psd->bpp);
        goto fail;
    }

    psd->addr = (char*)hdr + hdr->dataoffset;

    psd->gr_mode = MODE_SET;
    psd->dev_spec = hdr;
    return psd;

fail:
    shmdt (hdr);
    return NULL;
}

/* close framebuffer*/
static void fb_close (PSD psd)
{
        shmdt (psd->dev_spec);
}

static void fb_setpalette (PSD psd, int first, int count, GAL_Color *palette)
{
    int i, entry = first;
    struct QVFbHeader* hdr;

    hdr = psd->dev_spec;

    for (i = 0; i < count; i++) {
        hdr->clut [entry++] = 
                (0xff << 24) | ((palette[i].r & 0xff) << 16) | ((palette[i].g & 0xff) << 8) | (palette[i].b & 0xff);
    }
}

static void fb_getpalette (PSD psd, int first, int count, GAL_Color *palette)
{
    int i, entry = first;
    struct QVFbHeader* hdr;
    unsigned int rgb;

    hdr = psd->dev_spec;

    for (i = 0; i < count; i++) {
        rgb = hdr->clut [entry++];

        palette[i].r = (rgb & 0x00FF0000) >> 16;
        palette[i].g = (rgb & 0x0000FF00) >> 8;
        palette[i].b = (rgb & 0x000000FF);
    }
}

static void update_rect (PSD psd, int l, int t, int r, int b)
{
    struct QVFbHeader* hdr = psd->dev_spec;
    RECT bound;
    RECT rc = {l, t, r, b};

    lock ();

    bound = hdr->update;
    if (bound.right == -1) bound.right = 0;
    if (bound.bottom == -1) bound.bottom = 0;

    NormalizeRect (&rc);
    GetBoundRect (&bound, &bound, &rc);

    hdr->update = bound;
    hdr->dirty = TRUE;

    unlock();
}

SCREENDEVICE qvfbdev = {
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 
    0, 0, 0, 0, 
    NULL, NULL,
    fb_open,
    fb_close,
    fb_setpalette,
    fb_getpalette,
    native_gen_allocatememgc,
    fb_mapmemgc,
    native_gen_freememgc,
    native_gen_clippoint,
    native_gen_fillrect,
    NULL,           /* DrawPixel subdriver */
    NULL,           /* ReadPixel subdriver */
    NULL,           /* DrawHLine subdriver */
    NULL,           /* DrawVLine subdriver */
    NULL,           /* Blit subdriver */
    NULL,           /* PutBox subdriver */
    NULL,           /* GetBox subdriver */
    NULL,           /* PutBoxMask subdriver */
    NULL,           /* CopyBox subdriver */
    update_rect     /* UpdateRect */
};

#endif /* _NATIVE_GAL_QVFB */

