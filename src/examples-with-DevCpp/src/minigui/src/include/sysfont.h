/*
** $Id: sysfont.h,v 1.8 2003/08/12 07:46:18 weiym Exp $
**
** sysfont.h: the head file of System Font module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SYSFONT_H
    #define GUI_SYSFONT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitSysFont (void);
void TermSysFont (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_SYSFONT_H

