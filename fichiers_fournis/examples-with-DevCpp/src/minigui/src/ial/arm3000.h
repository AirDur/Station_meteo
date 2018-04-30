/*
** $Id: arm3000.h,v 1.2 2004/07/28 03:55:42 yaoyy Exp $
**
** arm3000.h:. the head file of Low Level Input Engine for NET-ARM3000 
**
** Copyright (C) 2003 Feynman Software.
**
** Created by Yao Yunyuan, 2004/07/23
*/

#ifndef GUI_IAL_ARM3000_H
    #define GUI_IAL_ARM3000_H

#define KEY_RELEASED    0x80 

#define KEYPAD_0        0x0B
#define KEYPAD_1        0x02
#define KEYPAD_2        0x0A
#define KEYPAD_3        0x12
#define KEYPAD_4        0x01
#define KEYPAD_5        0x09
#define KEYPAD_6        0x11
#define KEYPAD_7        0x00
#define KEYPAD_8        0x08
#define KEYPAD_9        0x10
#define KEYPAD_PERIOD      0x19
#define KEYPAD_DIVIDE      0x0C
#define KEYPAD_MULTIPLY    0x14
#define KEYPAD_MINUS       0x15
#define KEYPAD_PLUS        0x18     
#define KEYPAD_ENTER       0x1A
#define KEYPAD_NUMLOCK     0x04

#define MAX_KEYPAD_CODE     0x1C

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

BOOL    InitARM3000Input (INPUT* input, const char* mdev, const char* mtype);
void    TermARM3000Input (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* GUI_IAL_ARM3000_H */
