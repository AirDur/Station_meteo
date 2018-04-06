/*
** $Id: cliprect.h,v 1.6 2003/08/12 07:46:18 weiym Exp $
**
** cliprect.h: the head file of Clip Rect module.
** 
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/3/26
*/

#ifndef GUI_CLIPRECT_H
    #define GUI_CLIPRECT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct tagGCRINFO
{
#ifndef _LITE_VERSION
    pthread_mutex_t lock;
#endif
    CLIPRGN         crgn;
    int             age;
} GCRINFO;
typedef GCRINFO* PGCRINFO;

typedef struct tagINVRGN
{
#ifndef _LITE_VERSION
    pthread_mutex_t lock;
#endif
    CLIPRGN         rgn;
    int             frozen;
} INVRGN;
typedef INVRGN* PINVRGN;

/* Function definitions */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_CLIPRECT_H */


