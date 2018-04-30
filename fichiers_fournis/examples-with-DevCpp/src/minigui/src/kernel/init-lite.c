/*
** $Id: init-lite.c,v 1.40 2005/01/31 06:41:57 weiym Exp $
**
** init.c: The Initialization/Termination routines
**
** Copyright (C) 1999, 2000, Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
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
#include <unistd.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "cursor.h"
#include "event.h"
#include "misc.h"
#include "menu.h"
#include "timer.h"
#include "accelkey.h"

#ifndef _STAND_ALONE
  #include "sharedres.h"
  #include "client.h"
  #include "server.h"

  void* mgSharedRes;
  size_t mgSizeRes;

#endif

BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
{
    return SendMessage (HWND_DESKTOP, MSG_REINITSESSION, init_sys_text, 0) == 0;
}

// static struct termios savedtermio;
/*
void* GUIAPI GetOriginalTermIO (void)
{
#ifdef _STAND_ALONE
    return &savedtermio;
#else
    return &SHAREDRES_TERMIOS;
#endif
}
*/

static BOOL InitResource (void)
{
    /*
    if (!InitFixStr ()) {
        uHALr_printf("MiniGUI: Can not initialize Fixed String heap failure!\n");
        goto failure;
    }
    */

#ifdef _CURSOR_SUPPORT
    if (!InitCursor ()) {
        uHALr_printf("MiniGUI: Count not initialize mouse cursor!\n");
        goto failure;
    }
#endif

    if (!InitMenu ()) {
        uHALr_printf("MiniGUI: Init Menu module failure!\n");
        goto failure;
    }
   
    if (!InitControlClass ()) {
        fprintf(stderr, "MiniGUI: Init Control Class failure!\n");
        goto failure;
    }
   
    if (!InitAccel ()) {
        fprintf(stderr, "MiniGUI: Init Accelerator failure!\n");
        goto failure;
    }
    
    if (!InitDesktop ()) {
        uHALr_printf("MiniGUI: Init Desktop error!\n");
        goto failure;
    }
    
    if (!InitFreeQMSGList ()) {
        uHALr_printf("MiniGUI: Init free QMSG list error!\n");
        goto failure;
    }
    
    if (!InitDskMsgQueue ()) {
        uHALr_printf("MiniGUI: Init MSG queue error!\n");
        goto failure;
    }
    
    return TRUE;

failure:
    return FALSE;
}

#define err_message(step, message) uHALr_printf("Error in step %d: %s\n", step, message)

#ifdef _STAND_ALONE
/*
static void segvsig_handler (int v)
{
    TerminateLWEvent ();
    TerminateGAL ();

    if (v == SIGSEGV)
        kill (getpid(), SIGABRT); // cause core dump
    else
        _exit (v);
}
*/
#else

// defined in kernel/sharedres.c.
void delete_sem (void);

static void segvsig_handler (int v)
{
    TerminateLWEvent ();
    TerminateGAL ();
    delete_sem ();

    unlink (LOCKFILE);
    unlink (CS_PATH);

    if (v == SIGSEGV)
        kill (getpid(), SIGABRT); // cause core dump
    else
        _exit (v);
}

#endif

#if 0
static BOOL InstallSEGVHandler (void)
{
    struct sigaction siga;
    
    siga.sa_handler = segvsig_handler;
    siga.sa_flags = 0;
    
    memset (&siga.sa_mask, 0, sizeof (sigset_t));
    sigaction (SIGSEGV, &siga, NULL);
    sigaction (SIGTERM, &siga, NULL);
    sigaction (SIGINT, &siga, NULL);

    /* ignore the SIGPIPE signal */
    siga.sa_handler = SIG_IGN;
    sigaction (SIGPIPE, &siga, NULL);

    return TRUE;
}
#endif

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

int InitGUI (void)
{
    int step = 1;
#if !defined(_STAND_ALONE)
    const char* name;

    if ((name = strrchr (agr [0], '/')) == NULL)
        name = agr [0];
    else
        name ++;

    if (!strcmp (name, "mginit"))
        mgIsServer = TRUE;
#endif

#ifdef HAVE_LOCALE_H
    setlocale (LC_ALL, "C");
#endif

    // Save original termio
#ifndef _STAND_ALONE
    if (mgIsServer)
#endif
//        tcgetattr (0, &savedtermio);

    uHALr_printf("InitFixStr\n");
    
    if (!InitFixStr ()) {
        err_message (step, "MiniGUI: Can not initialize Fixed String heap failure!\n");
        return step;
    }
    step++;

    uHALr_printf("InitMisc\n");
    
    if (!InitMisc ()) {
        err_message (step, "Can not initialize miscellous things!");
        return step;
    }
    step++;

#ifndef _STAND_ALONE
    if (mgIsServer && !IsOnlyMe ()) {
        err_message (step, "There is already an instance of 'mginit'!");
        return step;
    }
    step++;
#endif

    // Init GAL engine.
    switch (InitGAL ()) {
    case ERR_CONFIG_FILE:
        err_message (step, "Reading configuration failure!");
        return step;

    case ERR_NO_ENGINE:
        err_message (step, "No graphics engine defined!");
        return step;

    case ERR_NO_MATCH:
        err_message (step, "Can not get graphics engine information!");
        return step;

    case ERR_GFX_ENGINE:
        err_message (step, "Can not initialize graphics engine!");
        return step;
    }
    step++;
    atexit (TerminateGAL);

    /* install signal handlers */
#ifndef _STAND_ALONE
    if (mgIsServer)
#endif
//        InstallSEGVHandler ();

    if (!InitGDI ()) {
        err_message (step, "MiniGUI: Initialization of GDI resource failure!\n");
        return step;
    }
    step++;
    atexit (TerminateGDI);

    /* Init Screen DC here */
    if (!InitScreenDC ()) {
        err_message (step, "Can not initialize screen DC!");
        return step;
    }
    step++;
    atexit (TerminateScreenDC);

    g_rcScr.left = 0;
    g_rcScr.top = 0;
    g_rcScr.right = GetGDCapability (HDC_SCREEN, GDCAP_MAXX) + 1;
    g_rcScr.bottom = GetGDCapability (HDC_SCREEN, GDCAP_MAXY) + 1;

    if (!InitWindowElementColors ()) {
        err_message (step, "Can not initialize colors of window element!");
        return step;
    }
    step++;

    window_init(); /* TEST */
    
#ifndef _STAND_ALONE
    if (mgIsServer) {
        // Load shared resource and create shared memory.
        if ((mgSharedRes = LoadSharedResource ()) == NULL) {
            err_message (step, "Can not load shared resource!");
            return step;
        }
        atexit (UnloadSharedResource);
        SHAREDRES_TERMIOS = savedtermio;
    }
    else {
        if ((mgSharedRes = AttachSharedResource ()) == NULL) {
            err_message (step, "Can not attach shared resource!");
            return step;
        }
        atexit (UnattachSharedResource);
    }
    step++;

#endif

    // Initialize resource
    if (!InitResource ()) {
        err_message (step, "Can not initialize resource!");
        return step;
    }
    step++;

#ifdef _STAND_ALONE

    // Init IAL engine..
    if (!InitLWEvent ()) {
        err_message (step, "Can not initialize low level event!");
        return step;
    }
    step++;

    atexit (TerminateLWEvent);

    __mg_dsk_msgs.OnIdle = IdleHandler4StandAlone;

    if (!StandAloneStartup ()) {
        uHALr_printf("Can not start MiniGUI-Lite StandAlone version.\n");
        return step;
    }

#else

    if (mgIsServer) {
        // Init IAL engine..
        if (!InitLWEvent ()) {
            err_message (step, "Can not initialize low level event!");
            return step;
        }
        step++;

        atexit (TerminateLWEvent);

        __mg_dsk_msgs.OnIdle = IdleHandler4Server;

        if (!ServerStartup ()) {
            uHALr_printf("Can not start the server of MiniGUI-Lite: mginit.\n");
            return step;
        }
    }
    else {
        if (!ClientStartup ()) {
            err_message (step, "Can not start client!");
            return step;
        }
        step++;

        __mg_dsk_msgs.OnIdle = IdleHandler4Client;
    }
#endif

    SetKeyboardLayout ("default");

    TerminateMgEtc ();

    return 0;
}

void TerminateGUI (int rcByGUI)
{
    DestroyDskMsgQueue ();
    DestroyFreeQMSGList ();
    TerminateDesktop ();
    TerminateAccel ();
    TerminateControlClass ();
    TerminateMenu ();
#ifdef _CURSOR_SUPPORT
    TerminateCursor ();
#endif
    TerminateFixStr ();

#ifdef _STAND_ALONE
    SendMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);

    StandAloneCleanup ();
#else
    if (mgIsServer) {
        SendMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);

        /* Cleanup UNIX domain socket and other IPC objects. */
        ServerCleanup ();
    }
    else {
        ClientCleanup ();
    }
#endif
}

