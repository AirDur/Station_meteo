/*
** $Id: auto.h,v 1.1 2004/01/29 06:12:18 weiym Exp $
**
** auto.h: head file for automatic Input Engine
**
** Copyright (C) 2004 Feynman Software.
**
** Created by Wei YongMing, 2004/01/29
*/

#ifndef _IAL_AUTO_H
#define _IAL_AUTO_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL InitAutoInput (INPUT* input, const char* mdev, const char* mtype);
void TermAutoInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _IAL_AUTO_H */

