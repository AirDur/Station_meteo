/*
** $Id: propsheet_impl.h,v 1.1 2005/01/31 09:12:48 clear Exp $
**
** propsheet.h: the head file of PropSheet control.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming and others.
**
** NOTE: Originally by Wang Jian and Jiang Jun.
**
** Create date: 2001/11/19
*/

#ifndef GUI_PROPSHEET_IMPL_H_
#define GUI_PROPSHEET_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef struct tagPROPPAGE
{
    char* title;                /* title of page */
    HICON icon;                 /* icon of page */
    int  width;                 /* width of title */
    HWND hwnd;                  /* container control */
    WNDPROC proc;               /* callback of page */

    struct tagPROPPAGE* next;   /* next page */
} PROPPAGE;
typedef PROPPAGE* PPROPPAGE;

typedef struct tagPROPSHEETDATA
{
    RECT head_rc;               /* height of page title */
    int head_width;             /* the effective width of head */
    int page_count;             /* the number of pages */
    PROPPAGE* active;           /* index of active page */
    PROPPAGE* head;             /* head of page list */
} PROPSHEETDATA;
typedef PROPSHEETDATA* PPROPSHEETDATA;

BOOL RegisterPropSheetControl (void);
void PropSheetControlCleanup (void);

#ifdef __cplusplus
}
#endif

#endif // GUI_PROPSHEET_IMPL_H_


