/*
** $Id: dummy.h,v 1.2 2003/09/04 03:38:25 weiym Exp $
**
** dummy.h:. the head file of Dummy IAL Engine.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Created by Wei YongMing, 2000/09/13
*/

#ifndef GUI_IAL_DUMMY_H
    #define GUI_IAL_DUMMY_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitDummyInput (INPUT* input, const char* mdev, const char* mtype);
void    TermDummyInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_DUMMY_H */


