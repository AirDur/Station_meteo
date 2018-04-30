/*
** $Id: qvfb.h,v 1.2 2003/08/01 09:50:43 weiym Exp $
**
** qvfb.h: headers for qvfb.
**
** Copyright (C) 2003 Feynman Software
*/

#ifndef _GAL_qvfb_h
#define _GAL_qvfb_h

#include <sys/types.h>

#define QT_VFB_MOUSE_PIPE	"/tmp/.qtvfb_mouse-%d"
#define QT_VFB_KEYBOARD_PIPE	"/tmp/.qtvfb_keyboard-%d"
#define QTE_PIPE "QtEmbedded"

struct QVFbHeader
{
    int width;
    int height;
    int depth;
    int linestep;
    int dataoffset;
    RECT update;
    BYTE dirty;
    int  numcols;
    unsigned int clut[256];
};

struct QVFbKeyData
{
    unsigned int unicode;
    unsigned int modifiers;
    BOOL press;
    BOOL repeat;
};

#endif /* _GAL_qvfb_h */

