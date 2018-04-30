/*
** $Id: menu.h,v 1.12 2003/12/11 05:07:04 weiym Exp $
**
** menu.h: the head file of menu module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/04/08
**
** Used abbreviations:
**  Menu: mnu
**  Popup: ppp
**  Identifier: id
**  Mnemonic: mnic
**  Normal: nml
**  Item: itm
*/

#ifndef GUI_MENU_H
    #define GUI_MENU_H

typedef struct _MENUITEM {
    short               category;
    short               type;
    UINT                mnutype;
    UINT                mnustate;
    int                 id;
    PBITMAP             hbmpChecked;
    PBITMAP             hbmpUnchecked;
    DWORD               itemdata;
    DWORD               typedata;
    int                 h;
    struct _MENUITEM*   submenu;
    struct _MENUITEM*   next;
} MENUITEM;
typedef MENUITEM* PMENUITEM;

typedef struct _MENUBAR
{
    short               category;       // category of data.
    short               type;           // type of data.
    HWND                hwnd;           // owner.
    int                 hilitedItem;    // hilited item.
    UINT                hiliteFlag;     // hilite flag.
    PMENUITEM           head;           // head of menu item list
} MENUBAR;
typedef MENUBAR* PMENUBAR;

typedef struct _TRACKMENUINFO
{
    RECT                    rc;
    PMENUITEM               pmi;
    PMENUBAR                pmb;
    int                     barPos;
    PMENUITEM               philite;
    HWND                    hwnd;
    UINT                    flags;
#ifdef _USE_NEWGAL
    BITMAP                  savedbox;
#else
    void*                   savedbox;
#endif
    struct _TRACKMENUINFO*  prev;
    struct _TRACKMENUINFO*  next;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    unsigned int scr_idx;
                        // idex of server clipping rect.
#endif

} TRACKMENUINFO;

/* consts for block data heaps */
#ifdef __uClinux__
    #define SIZE_MI_HEAP   8
    #define SIZE_MB_HEAP   1
    #define SIZE_TMI_HEAP  2
#else
  #ifdef _LITE_VERSION
    #define SIZE_MI_HEAP   16
    #define SIZE_MB_HEAP   2
    #define SIZE_TMI_HEAP  4
  #else
    #define SIZE_MI_HEAP   512
    #define SIZE_MB_HEAP   64
    #define SIZE_TMI_HEAP  16
  #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef _DEBUG
void DumpMenu (HMENU hmnu);
#endif  // _DEBUG

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_MENU_H

