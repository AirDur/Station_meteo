/*
** $Id: incoreres.h,v 1.5 2003/09/25 04:08:55 snig Exp $
**
** incoreres.h: definitions for incore resource.
**
** Copyright (C) 2003 Feynman Software.
**
** Create date: 2003/08/14
*/

#ifndef GUI_INCORERES_H
    #define GUI_INCORERES_H

/* Function definitions */

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* moved to mgetc.c */
#if 0

#ifdef __uClinux__

#define GAL_ENGINE      "fbcon"
#define GAL_DEF_MODE    "160x160-1bpp"

#define IAL_ENGINE      "MC68X328"
#define IAL_MDEV        "/dev/ts"
#define IAL_MTYPE       "def"

#else

#define GAL_ENGINE      "qvfb"
#define GAL_DEF_MODE    "640x480-16bpp"

#define IAL_ENGINE      "qvfb"
#define IAL_MDEV        "/dev/mouse"
#define IAL_MTYPE       "ps2"

#endif

#define NR_SYS_FONTS    3
#define SYS_FONT_NAME   static const char* sys_font_name [] = \
        { \
                    "rbf-fixed-rrncnn-6-12-ISO8859-1", \
                    "*-fixed-rrncnn-*-12-GB2312", \
                    "*-SansSerif-rrncnn-*-12-GB2312" \
        }

#define SYS_FONT_ID     static int sys_font_id [] = \
        { 0, 1, 1, 2, 2, 2 }

#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_INCORERES_H */

