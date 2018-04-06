/*
** $Id: hh2410r3.h,v 1.2 2004/04/01 08:57:13 weiym Exp $
**
** hh2410r3.h: the head file of Low Level Input Engine for HHARM2410R3.
**
** Copyright (C) 2004 Feynman Software.
*/

#ifndef GUI_IAL_HH2410R3_H
#define GUI_IAL_HH2410R3_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus */

BOOL	InitHH2410R3Input (INPUT* input, const char* mdev, const char* mtype);
void	TermHH2410R3Input (void);

#ifdef __cplusplus
}
#endif	/* __cplusplus */

#endif	/* GUI_IAL_HH2410R3_H */


