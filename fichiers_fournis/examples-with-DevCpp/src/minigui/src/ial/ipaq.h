/*
** $Id: ipaq.h,v 1.6 2004/01/29 09:16:44 weiym Exp $
**
** ipaq.h:. the head file of Low Level Input Engine for iPAQ
**
** Copyright (C) 2003 Feynman Software.
**
** Created by Wei YongMing, 2001/08/20
*/

#ifndef GUI_IAL_IPAQ_H
    #define GUI_IAL_IPAQ_H

#ifdef __ECOS__

/* bit 7 = state bits0-3 = key number */
#define KEY_RELEASED    0x80
#define KEY_NUM	        0x0f
#define MAX_KEY_EVENTS  4

#define H3600_SCANCODE_RECORD   59
#define H3600_SCANCODE_CALENDAR 60
#define H3600_SCANCODE_CONTACTS 61
#define H3600_SCANCODE_Q        62
#define H3600_SCANCODE_START    63
#define H3600_SCANCODE_UP       103
#define H3600_SCANCODE_RIGHT    106
#define H3600_SCANCODE_LEFT     105
#define H3600_SCANCODE_DOWN     108
#define H3600_SCANCODE_ACTION   64
#define H3600_SCANCODE_SUSPEND  65

#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitIPAQInput (INPUT* input, const char* mdev, const char* mtype);
void    TermIPAQInput (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* GUI_IAL_IPAQ_H */


