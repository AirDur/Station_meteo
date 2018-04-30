/*
** $Id: fft7202.h,v 1.1 2004/08/19 07:02:54 yaoyy Exp $
**
** fft7202.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 Feynman Software.
**
** Created by Yao yunyuna, 2001/08/20
*/

#ifndef GUI_IAL_IPAQ_H
    #define GUI_IAL_IPAQ_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitFFTInput (INPUT* input, const char* mdev, const char* mtype);
void    TermFFTInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_IPAQ_H */


