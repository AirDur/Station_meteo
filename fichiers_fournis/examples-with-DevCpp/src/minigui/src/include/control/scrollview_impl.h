/*
** $Id: scrollview_impl.h,v 1.2 2005/02/05 03:19:44 snig Exp $ 
**
** scrollview.h: header file of ScrollView control.
**
** Copyright (C) 2004 Feynman Software.
*/

#ifndef _SCROLLVIEW_IMPL_H_
#define _SCROLLVIEW_IMPL_H_


#include "listmodel.h"


#ifdef __cplusplus
extern "C"
{
#endif


/* default size and position values */
#define HSCROLL                 5   // h scroll value
#define VSCROLL                 15  // v scroll value

#define SV_LEFTMARGIN           5
#define SV_TOPMARGIN            5
#define SV_RIGHTMARGIN          5
#define SV_BOTTOMMARGIN         5

#define SV_DEF_ITEMHEIGHT       16


/* scrollview item struct */
typedef struct _sv_itemdata
{
    MgItem mgitem;

    int       nItemHeight;   // height of an item
    //DWORD     flags;         // item flags
} SVITEMDATA;
typedef SVITEMDATA* PSVITEMDATA;

typedef struct _svlist_data
{
    MgList mglist;

//FIXME, to delete
    /* default line height */
    int nDefItemHeight;
    /* total item height */
    //int nTotalItemH;

    //DWORD flags;

} SVLIST;
typedef SVLIST* PSVLIST;

typedef struct _scrvwdata
{
    /* must be the first member */
    SCRDATA scrdata;

    /* scrollview item list, list data model */
    SVLIST svlist;

    //HWND hsv;

    /* scroll view flags, status ... */
    //DWORD flags;

} SVDATA;
typedef SVDATA *PSVDATA;


#define psvscr ((PSCRDATA)psvdata)
#define hsvwnd (psvscr->hSV)


//typedef int (*SVITEM_CMP_EX) (PSVDATA psvdata, HSVITEM hsvi1, HSVITEM hsvi2);

/* ------------------------- external api -------------------------- */

int     ScrollViewCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

int     scrollview_init               (HWND hWnd, PSVDATA psv);
void    scrollview_destroy            (PSVDATA psvdata);

HSVITEM scrollview_add_item           (HWND hWnd, PSVDATA psvdata, HSVITEM hsvi, PSVITEMINFO pii, int *idx);
HSVITEM scrollview_add_item_ex        (HWND hWnd, PSVDATA psvdata, HSVITEM hsvi, 
                                       HSVITEM nexthsvi, PSVITEMINFO pii, int *idx);
int     scrollview_del_item           (HWND hWnd, PSVDATA psvdata, int nItem, HSVITEM hsvi);

void    scrollview_reset_content      (HWND hWnd, PSVDATA psvdata);

int     scrollview_get_item_ypos      (PSVDATA psvdata, HSVITEM hsvi);

int     scrollview_is_in_item         (PSVDATA psvdata, int mousey, HSVITEM *phsvi, int *itemy);

void    scrollview_set_svlist         (HWND hWnd, PSCRDATA pscrdata, BOOL visChanged);

void    scrollview_draw               (HWND hWnd, HDC hdc, PSVDATA psvdata);

/* -------------------------------------------------------------------------- */
//for convenience

//FIXME
static inline void scrollview_refresh_content (PSVDATA psvdata)
{
    if ( mglist_is_frozen((MgList *)&psvdata->svlist) )
        return;

    scrolled_refresh_content ((PSCRDATA)psvdata);
}

static inline void scrollview_set_autosort (PSVDATA psvdata)
{
    mglist_set_autosort ((MgList *)&psvdata->svlist);
}

static inline int scrollview_get_total_item_height (PSVDATA psvdata)
{
    //return psvdata->svlist.nTotalItemH;
    return ((MgList *)&psvdata->svlist)->nTotalItemH;
}

static inline void scrollview_select_item (PSVDATA psvdata, HSVITEM hsvi, BOOL bSel)
{
    if (!hsvi) return;
    mglist_select_item ((MgList*)&psvdata->svlist, (MgItem *)hsvi, bSel);
}

/* unselect all and select this one */
static inline void scrollview_select_one_item (PSVDATA psvdata, HSVITEM hsvi)
{
    if (!hsvi) return;

    mglist_unselect_all ((MgList *)&psvdata->svlist);
    mglist_select_item ((MgList*)&psvdata->svlist, (MgItem *)hsvi, TRUE);
}

static inline void scrollview_unselect_all (PSVDATA psvdata)
{
    mglist_unselect_all ((MgList *)&psvdata->svlist);
}

static inline void scrollview_select_all (PSVDATA psvdata)
{
    mglist_select_all ((MgList*)&psvdata->svlist);
}

static inline int scrollview_get_item_count (PSVDATA psvdata)
{
    return mglist_get_item_count((MgList *)&psvdata->svlist);
}

static inline HWND scrollview_get_window (PSVDATA psvdata)
{
    return hsvwnd;
}

static inline HSVITEM scrollview_get_hilighted_item (PSVDATA psvdata)
{
    return (HSVITEM)mglist_get_hilighted_item((MgList *)&psvdata->svlist);
}

static inline void scrollview_set_itemops (PSVDATA psvdata, PSVITEMOPS iop)
{
    MGITEMOPS *myiop = &((MgList *)&psvdata->svlist)->iop;

    *(PSVITEMOPS)myiop = *iop;
}

static inline void scrollview_hilight_item (PSVDATA psvdata, HSVITEM hsvi)
{
    mglist_hilight_item ((MgList *)&psvdata->svlist, (MgItem *)hsvi);
}

static inline BOOL scrollview_make_item_visible (PSVDATA psvdata, HSVITEM hsvi)
{
    return mglist_make_item_visible ((MgList *)&psvdata->svlist, psvscr, hsvi);
}

static inline void scrollview_refresh_item (PSVDATA psvdata, HSVITEM hsvi)
{
    mglist_refresh_item ((MgList *)&psvdata->svlist, hsvi);
}

static inline void scrollview_freeze (HWND hWnd, PSVDATA psvdata, BOOL lock)
{
    mglist_freeze (hWnd, (MgList *)&psvdata->svlist, lock);
}

static inline HSVITEM scrollview_get_item_by_index (PSVDATA psvdata, int nItem)
{
    return (HSVITEM)mglist_getitem_byindex ((MgList *)&psvdata->svlist, nItem);
}

static inline HSVITEM scrollview_get_next_item (PSVDATA psvdata, HSVITEM hsvi)
{
    return (HSVITEM)mglist_get_next_item((MgList *)&psvdata->svlist, (MgItem *)hsvi);
}

static inline HSVITEM scrollview_get_prev_item (PSVDATA psvdata, HSVITEM hsvi)
{
    return (HSVITEM)mglist_get_prev_item((MgList *)&psvdata->svlist, (MgItem *)hsvi);
}

static inline int scrollview_sort_items (PSVDATA psvdata, void* pfn, int fn_type)
{
    return mglist_sort_items ((MgList *)&psvdata->svlist, pfn, fn_type);
}
    
static inline int scrollview_get_item_height (HSVITEM hsvi)
{
    return ((PSVITEMDATA)hsvi)->nItemHeight;
}

static inline void scrollview_redraw_item (PSVDATA psvdata, HSVITEM hsvi)
{
    mglist_redraw_item ((MgList *)&psvdata->svlist, hsvi);
}


/* -------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _SCROLLVIEW_IMPL_H_ */

