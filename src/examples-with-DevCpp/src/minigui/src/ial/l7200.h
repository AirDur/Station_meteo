/*
** $Id: l7200.h,v 1.2 2004/08/08 03:35:27 snig Exp $
**
** ipaq.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 Feynman Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_L7200_H
    #define GUI_IAL_L7200_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

BOOL InitL7200Input (INPUT* input, const char* mdev, const char* mtype);
void TermL7200Input (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* GUI_IAL_L7200_H */
