/**
 * \file scrollview.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2001/12/29
 * 
 \verbatim
    Copyright (C) 2002-2005 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you are using MiniGUI for developing commercial, proprietary, or other
    software not covered by the GPL terms, you must have a commercial license
    for MiniGUI. Please see http://www.minigui.com/product/index.html for 
    how to obtain this. If you are interested in the commercial MiniGUI 
    licensing, please write to sales@minigui.com. 

 \endverbatim
 */

/*
 * $Id: scrollview.h,v 1.2 2005/02/15 05:00:08 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1999-2002 Wei Yongming.
 */

#ifndef _MGUI_CTRL_SCROLLVIEW_H
#define _MGUI_CTRL_SCROLLVIEW_H
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup controls
     * @{
     */

    /**
     * \defgroup ctrl_scrollview ScrollView control
     * @{
     *
     * Scrollview control is a scrollable window, which has a visible area and
     * normally a larger content area, user can browse content area using scrollbar.
     * Contents of the content area is totally user-defined, you can add controls in it,
     * add customed listed items, or anything you want.
     *
     * In fact, you can build a control needing scrolled window support on scrollview.
     */

/**
 * \def CTRL_SCROLLVIEW
 * \brief The class name of scrollview control, uses this name to create a scrollable
 *        window that consists of items.
 *
 */
#define CTRL_SCROLLVIEW           ("scrollview")

/**
 * \def CTRL_SCROLLWND
 * \brief The class name of scrollwnd control, uses this name to create a scrollable
 *        window to which you can add controls.
 *
 */
#define CTRL_SCROLLWND            ("scrollwnd")

/** default container window procedure */
MG_EXPORT int GUIAPI DefaultContainerProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

/** scrollview item object, use this handle to access a scrollview item */
typedef GHANDLE HSVITEM;

/**
 * Structure which defines a container.
 */
typedef struct _CONTAINERINFO
{
#if 0
    DWORD       dwStyle;                /** style of the container */
    DWORD       dwExStyle;              /** extended style of the container */
    int         x, y, w, h;             /** position and size of the container */
#endif

    WNDPROC     user_proc;              /** user-defined window procedure of the container */

    int         controlnr;              /** number of controls */
    PCTRLDATA   controls;               /** pointer to control array */

    DWORD       dwAddData;              /** additional data */
} CONTAINERINFO;
typedef CONTAINERINFO* PCONTAINERINFO;

/** 
 * \fn DWORD mglist_get_item_adddata (GHANDLE hi)
 * \brief use this to get additional data from a list item in a control, such as
 *        scrollview and iconview.
 */
MG_EXPORT DWORD mglist_get_item_adddata (GHANDLE hi);

/** 
 * \fn DWORD scrollview_get_item_adddata (HSVITEM hsvi)
 * \brief use this to get additional data from scrollview item 
 */
MG_EXPORT DWORD scrollview_get_item_adddata (HSVITEM hsvi);

/**
 * \fn int scrollview_get_item_index (HWND hWnd, HSVITEM hsvi);
 * \brief to get item index
 */ 
MG_EXPORT int scrollview_get_item_index (HWND hWnd, HSVITEM hsvi);

/**
 * \fn int scrollview_is_item_hilight (HWND hWnd, HSVITEM hsvi)
 * \brief to decide whether an item is the current hilighted item
 */
MG_EXPORT BOOL scrollview_is_item_hilight (HWND hWnd, HSVITEM hsvi);

/**
 * \fn int scrollview_is_item_selected (HSVITEM hsvi)
 * \brief to decide whether an item is a selected
 */
MG_EXPORT BOOL scrollview_is_item_selected (HSVITEM hsvi);

/**
 * \fn int scrollview_set_item_height (HWND hWnd, HSVITEM hsvi, int height)
 * \brief Sets the height of an item
 */
MG_EXPORT int scrollview_set_item_height (HWND hWnd, HSVITEM hsvi, int height);

/**
 * \var typedef void (* SVITEM_INITFUNC)(HWND hWnd, HSVITEM hsvi)
 * \brief Type of the scrollview item initialization callback procedure.
 *
 * \param hWnd handle of the scrollview control .
 * \param hsvi scrollview item handle.
 *
 * \return Zero on success; otherwise -1.
 */
typedef int  (*SVITEM_INITFUNC)    (HWND hWnd, HSVITEM hsvi);

/**
 * \var typedef void (* SVITEM_DESTROYFUNC)(HWND hWnd, HSVITEM hsvi)
 * \brief Type of the scrollview item destroy callback procedure.
 *
 * \param hWnd handle of the scrollview control .
 * \param hsvi scrollview item handle.
 */
typedef void (*SVITEM_DESTROYFUNC) (HWND hWnd, HSVITEM hsvi);

/**
 * \var typedef void (*SVITEM_DRAWFUNC)(HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw)
 * \brief Type of the scrollview item drawing callback procedure.
 *
 * \param hWnd handle of the scrollview control.
 * \param hsvi scrollview item handle.
 * \param hdc  drawing device context.
 * \param rcDraw drawing rect area.
 */
typedef void (*SVITEM_DRAWFUNC) (HWND hWnd, HSVITEM hsvi, HDC hdc, RECT *rcDraw);

/**
 * \var typedef int (*SVITEM_CMP) (HSVITEM hsvi1, HSVITEM hsvi2)
 * \brief Type of the scrollview item compare function
 */
typedef int (*SVITEM_CMP) (HSVITEM hsvi1, HSVITEM hsvi2);

/** structure of item operations */
typedef struct _svitem_operations
{
    SVITEM_INITFUNC     initItem;     /** called when an scrollview item is created */
    SVITEM_DESTROYFUNC  destroyItem;  /** called when an item is destroied */
    SVITEM_DRAWFUNC     drawItem;     /** call this to draw an item */
} SVITEMOPS;
typedef SVITEMOPS* PSVITEMOPS;

/** structure of the scrollview item info */
typedef struct _SCROLLVIEWITEMINFO
{
    int        nItem;           /** index of item */
    int        nItemHeight;     /** height of an item */
    DWORD      addData;         /** item additional data */
} SVITEMINFO;
typedef SVITEMINFO* PSVITEMINFO;

    /**
     * \defgroup ctrl_scrollview_styles Styles of scrollview control
     * @{
     */

#define SVS_NOTIFY              0x0001L
#define SVS_UPNOTIFY            0x0002L
#define SVS_AUTOSORT            0x0004L
#define SVS_LOOP                0x0008L

    /** @} end of ctrl_scrollview_styles */

    /**
     * \defgroup ctrl_scrollview_msgs Messages of scrollview control
     * @{
     */

/**
 * \def SVM_ADDITEM
 * \brief adds an item in the scrollview.
 *
 * \code
 * SVM_ADDITEM
 * SVITEMINFO svii;
 * HSVITEM *phsvi;
 *
 * wParam = phsvi;
 * lParam = (LPARAM)&svii;
 * \endcode
 *
 * \param &svii pointer to a scrollview item information structure.
 * \param phsvi pointer to a HSVITEM var, used to store the item handle
 *              returned.
 *
 * \return index of the scrollview item on success; otherwise -1.
 */
#define SVM_ADDITEM             0xF300

/**
 * \def SVM_DELITEM
 * \brief deletes an item from the scrollview.
 *
 * \code
 * SVM_DELITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem scrollview item index to delete.
 *              If hsvi is not zero, nItem will be ignored.
 * \param hsvi scrollview item handle to delete.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_DELITEM             0xF301

/**
 * \def SVM_SETITEMDRAW
 * \brief sets the drawing operation of an item.
 *
 * scrollview item drawing function will be called when doing with MSG_PAINT message,
 * scrollview window should define this function if it want to draw an customed item.
 *
 * \code
 * SVM_SETITEMDRAW
 * SVITEM_DRAWFUNC pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn scrollview item drawing function.
 *
 * \return old drawing function pointer; otherwise 0.
 */
#define SVM_SETITEMDRAW         0xF302

/**
 * \def SVM_ADDCTRLS
 * \brief adds controls to the scrollview.
 *
 * \code
 * SVM_ADDCTRLS
 * int itemNr;
 * PCTRLDATA pctrls;
 *
 * wParam = (WPARAM)itemNr;
 * lParam = (LPARAM)pctrls;
 * \endcode
 *
 * \param ctrlNr control number in the pctrls control array.
 * \param pctrls points to a CTRLDATA array that defines controls.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_ADDCTRLS            0xF303

/**
 * \def SVM_SETCONTWIDTH
 * \brief sets the scrollview content area (scrollable area) width.
 *
 * scrollable area of a scrolled window is always larger than the visible area.
 *
 * \code
 * SVM_SETCONTWIDTH
 * int cont_w;
 *
 * wParam = cont_w;
 * lParam = 0;
 * \endcode
 *
 * \param cont_w scrollview content width.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETCONTWIDTH        0xF306

/**
 * \def SVM_SETCONTHEIGHT
 * \brief sets the scrollview content area (scrollable area) height.
 *
 * scrollable area of a scrolled window is always larger than the visible area.
 *
 * \code
 * SVM_SETCONTHEIGHT
 * int cont_h;
 *
 * wParam = cont_h;
 * lParam = 0;
 * \endcode
 *
 * \param cont_h scrollview content height.
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETCONTHEIGHT       0xF307

/**
 * \def SVM_GETCTRL
 * \brief gets the control handle in the scrollview window by control id.
 *
 * \code
 * SVM_GETCTRL
 * int id;
 *
 * wParam = id;
 * lParam = 0;
 * \endcode
 *
 * \param id control id.
 *
 * \return control window handle on success; otherwise 0.
 */
#define SVM_GETCTRL             0xF308

/**
 * \def SVM_RESETCONTENT
 * \brief clears all the controls and the items added to the scrollview window.
 *
 * \code
 * SVM_RESETCONTENT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_RESETCONTENT        0xF309

/**
 * \def SVM_SETITEMOPS
 * \brief sets the item operations of the items in the scrollview.
 *
 * normally item operations should be set before adding items.
 *
 * \code
 * SVM_SETITEMOPS
 * SVITEMOPS *iop;
 *
 * wParam = 0;
 * lParam = (LPARAM)iop;
 * \endcode
 *
 * \param iop points to a SVITEMOPS structure that defines item operations
 *
 * \return Zero on success; otherwise -1.
 */
#define SVM_SETITEMOPS          0xF30a

/**
 * \def SVM_GETMARGINS
 * \brief Gets the margin values of the scrollview.
 *
 * Application should use a RECT structure to get left, top, right, and bottom margins.
 *
 * \code
 * SVM_GETMARGINS
 * RECT rcMargin;
 *
 * wParam = 0;
 * lParam = (LPARAM)&rcMargin;
 * \endcode
 *
 * \param rcMargin a RECT for storing 4 margin values.
 *
 * \return 0 on success.
 */
#define SVM_GETMARGINS            0xF30b

/**
 * \def SVM_SETMARGINS
 * \brief Sets the margin values of the scrollview.
 *
 * Application should use a RECT structure to give left, top, right, and bottom margins.
 * If you want to change a margin value, give a value large than zero, or else -1.
 *
 * \code
 * SVM_SETMARGINS
 * RECT *rcMargin;
 *
 * wParam = 0;
 * lParam = (LPARAM)rcMargin;
 * \endcode
 *
 * \param rcMargin a RECT Containing 4 margin values.
 *
 * \return 0 on success.
 */
#define SVM_SETMARGINS            0xF311

/**
 * \def SVM_GETLEFTMARGIN
 * \brief Gets the left margin value of the scrollview.
 *
 * \code
 * SVM_GETLEFTMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Left margin value on success, otherwise -1.
 */
#define SVM_GETLEFTMARGIN         0xF312

/**
 * \def SVM_GETTOPMARGIN
 * \brief Gets the top margin value of the scrollview.
 *
 * \code
 * SVM_GETTOPMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Top margin value on success, otherwise -1.
 */
#define SVM_GETTOPMARGIN          0xF313

/**
 * \def SVM_GETRIGHTMARGIN
 * \brief Gets the right margin value of the scrollview.
 *
 * \code
 * SVM_GETRIGHTMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Right margin value on success, otherwise -1.
 */
#define SVM_GETRIGHTMARGIN        0xF314

/**
 * \def SVM_GETBOTTOMMARGIN
 * \brief Gets the bottom margin value of the scrollview.
 *
 * \code
 * SVM_GETBOTTOMMARGIN
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Bottom margin value on success, otherwise -1.
 */
#define SVM_GETBOTTOMMARGIN       0xF315

/**
 * \def SVM_GETVISIBLEWIDTH
 * \brief Gets the width of the visible content area.
 *
 * \code
 * SVM_GETVISIBLEWIDTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Width of the visible content area on success, otherwise -1.
 */
#define SVM_GETVISIBLEWIDTH       0xF316

/**
 * \def SVM_GETVISIBLEHEIGHT
 * \brief Gets the height of the visible content area.
 *
 * \code
 * SVM_GETVISIBLEHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height of the visible content area on success, otherwise -1.
 */
#define SVM_GETVISIBLEHEIGHT      0xF317

/**
 * \def SVM_GETCONTWIDTH
 * \brief Gets the width of the content area.
 *
 * \code
 * SVM_GETCONTWIDTH
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Width of the content area on success, otherwise -1.
 */
#define SVM_GETCONTWIDTH          0xF318

/**
 * \def SVM_GETCONTHEIGHT
 * \brief Gets the height of the content area.
 *
 * \code
 * SVM_GETCONTHEIGHT
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Height of the content area on success, otherwise -1.
 */
#define SVM_GETCONTHEIGHT         0xF319

/**
 * \def SVM_SETCONTRANGE
 * \brief Sets the width and height of the content area.
 *
 * If you want to change width and height, give a value large than zero.
 * If you give -1, this value will remain untouched.
 *
 * \code
 * SVM_SETCONTRANGE
 * int cont_w, cont_h;
 *
 * wParam = cont_w;
 * lParam = cont_h;
 * \endcode
 *
 * \param cont_w Width of the content area.
 * \param cont_h Height of the content area.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SETCONTRANGE          0xF31a

/**
 * \def SVM_GETCONTENTX
 * \brief Gets the content x offset in the viewport.
 *
 * \code
 * SVM_GETCONTENTX
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return content x coordinate in the viewport on success, otherwise -1.
 */
#define SVM_GETCONTENTX           0xF31b

/**
 * \def SVM_GETCONTENTY
 * \brief Gets the content y offset in the viewport.
 *
 * \code
 * SVM_GETCONTENTY
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return content y coordinate in the viewport on success, otherwise -1.
 */
#define SVM_GETCONTENTY           0xF31c

/**
 * \def SVM_SETCONTPOS
 * \brief Gets the content offset position in the viewport.
 *
 * \code
 * SVM_SETCONTPOS
 * int cont_x, cont_y;
 *
 * wParam = cont_x;
 * lParam = cont_y;
 * \endcode
 *
 * \param cont_x new content x offset
 * \param cont_y new content y offset
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SETCONTPOS            0xF31d

/**
 * \def SVM_GETCURSEL
 * \brief Gets the index of the current hilighted scrollview item.
 *
 * An application sends an this message to a scrollview window to get the index of 
 * the currently selected item, if there is one, in a single-selection scrollview.
 * For multiple-selection case, appliction send this message to a scrollview
 * to get the index of the current highlighted item.
 *
 * \code
 * SVM_GETCURSEL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return index of current hilighted item.
 */
#define SVM_GETCURSEL             0xF31e

/**
 * \def SVM_SELECTITEM
 * \brief Selects or unselects an item in the scrollview item.
 *
 * \code
 * SVM_SELECTITEM
 * int nItem;
 *
 * wParam = nItem;
 * lParam = bSel;
 * \endcode
 *
 * \param nItem Index of the item to select or unselect.
 * \param bSel To select or unselect.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SELECTITEM            0xF31f

/**
 * \def SVM_SHOWITEM
 * \brief Makes an item visible in the scrollview item.
 *
 * \code
 * SVM_SHOWITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = nItem;
 * lParam = hsvi;
 * \endcode
 *
 * \param nItem Item index. Be useful when hsvi is zero.
 * \param hsvi Item handle.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_SHOWITEM              0xF320

/**
 * \def SVM_CHOOSEITEM
 * \brief Makes an item selected and visible in the scrollview item.
 *
 * \code
 * SVM_CHOOSEITEM
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = nItem;
 * lParam = hsvi;
 * \endcode
 *
 * \param nItem Item index. Be useful when hsvi is zero.
 * \param hsvi Item handle.
 *
 * \return 0 on success, otherwise -1.
 */
#define SVM_CHOOSEITEM            0xF321

/**
 * \def SVM_SETCURSEL
 * \brief Makes an item as the current hilighted item in the scrollview item.
 *
 * \code
 * SVM_SETCURSEL
 * int nItem;
 * BOOL bVisible;
 *
 * wParam = nItem;
 * lParam = bVisible;
 * \endcode
 *
 * \param nitem Item index.
 * \param bVisible If bVisible is TRUE, this item wille be made visible.
 *                 
 * \return old hilighted item index on success, otherwise -1.
 */
#define SVM_SETCURSEL             0xF322

/**
 * \def SVM_SETITEMINIT
 * \brief Sets the init operation of the items in the scrollview.
 *
 * normally item operations should be set before adding items.
 * The initialization callback function will be called when adding an item.
 *
 * \code
 * SVM_SETITEMINIT
 * SVITEM_INITFUNC *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn item init operation function
 *
 * \return old function on success; otherwise NULL.
 */
#define SVM_SETITEMINIT            0xF323

/**
 * \def SVM_SETITEMDESTROY
 * \brief Sets the destroy operation of the items in the scrollview.
 *
 * normally item operations should be set before adding items.
 * The destroy callback function will be called when deleting an item
 *
 * \code
 * SVM_SETITEMDESTROY
 * SVITEM_DESTROYFUNC *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn item destroy operation function
 *
 * \return old function on success; otherwise NULL.
 */
#define SVM_SETITEMDESTROY        0xF324

/**
 * \def SVM_SETITEMCMP
 * \brief Sets the item compare function.
 *
 * \code
 * SVM_SETITEMCMP
 * SVITEM_CMP *pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn new item compare function
 *
 * \return old function on success; otherwise NULL.
 */
#define SVM_SETITEMCMP            0xF327

/**
 * \def SVM_MAKEPOSVISIBLE
 * \brief Makes a position in the content area visible.
 *
 * \code
 * SVM_MAKEPOSVISIBLE
 * int pos_x, pos_y;
 *
 * wParam = pos_x;
 * lParam = pos_y;
 * \endcode
 *
 * \param pos_x x coordinate of the position to be made visible
 * \param pos_y y coordinate of the position to be made visible
 */
#define SVM_MAKEPOSVISIBLE        0xF328

/**
 * \def SVM_SETCONTAINERPROC
 * \brief Sets the window procedure of the container window in the scrollview.
 *
 * \code
 * SVM_SETCONTAINERPROC
 * WNDPROC pfn;
 *
 * wParam = 0;
 * lParam = (LPARAM)pfn;
 * \endcode
 *
 * \param pfn new container procedure function.
 *
 * \return old container procedure.
 */
#define SVM_SETCONTAINERPROC       0xF329

/**
 * \def SVM_GETFOCUSCHILD
 * \brief Gets the focus control in the scrollview.
 *
 * \code
 * SVM_GETFOCUSCHILD
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Handle of the focus control.
 */
#define SVM_GETFOCUSCHILD          0xF32a

/**
 * \def SVM_GETHSCROLLVAL
 * \brief Gets the horizontal scroll value.
 *
 * \code
 * SVM_GETHSCROLLVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current horizontal scroll value.
 */
#define SVM_GETHSCROLLVAL           0xF32b

/**
 * \def SVM_GETVSCROLLVAL
 * \brief Gets the vertical scroll value.
 *
 * \code
 * SVM_GETVSCROLLVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current vertical scroll value.
 */
#define SVM_GETVSCROLLVAL           0xF32c

/**
 * \def SVM_GETHSCROLLPAGEVAL
 * \brief Gets the horizontal page scroll value.
 *
 * \code
 * SVM_GETHSCROLLPAGEVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current horizontal page scroll value.
 */
#define SVM_GETHSCROLLPAGEVAL       0xF32d

/**
 * \def SVM_GETVSCROLLPAGEVAL
 * \brief Gets the vertical page scroll value.
 *
 * \code
 * SVM_GETVSCROLLPAGEVAL
 *
 * wParam = 0;
 * lParam = 0;
 * \endcode
 *
 * \return Current vertical page scroll value.
 */
#define SVM_GETVSCROLLPAGEVAL       0xF32e

/**
 * \def SVM_SETSCROLLVAL
 * \brief Sets the horizontal and vertical scroll value.
 *
 * \code
 * SVM_SETSCROLLVAL
 * int hval, vval;
 *
 * wParam = hval;
 * lParam = vval;
 * \endcode
 *
 * \param hval new horizontal scroll value.
 * \param vval new vertical scroll value.
 *
 * \return zero.
 */
#define SVM_SETSCROLLVAL           0xF32f

/**
 * \def SVM_SETSCROLLPAGEVAL
 * \brief Sets the horizontal and vertical page value.
 *
 * \code
 * SVM_SETSCROLLPAGEVAL
 * int hval, vval;
 *
 * wParam = hval;
 * lParam = vval;
 * \endcode
 *
 * \param hval new horizontal page value.
 * \param vval new vertical page value.
 *
 * \return zero.
 */
#define SVM_SETSCROLLPAGEVAL       0xF330

/**
 * \def SVM_SORTITEMS
 * \brief Sorts the items according to a specified comparision function.
 *
 * \code
 * SVM_SORTITEMS
 * SVITEM_CMP pfn;
 *
 * lParam = pfn;
 * \endcode
 *
 * \param pfn Comparision function.
 *
 * \return zero.
 */
#define SVM_SORTITEMS              0xF331

/**
 * \def SVM_GETITEMCOUNT
 * \brief Gets the total number of the items.
 *
 * \return number of the items.
 */
#define SVM_GETITEMCOUNT           0xF332

/**
 * \def SVM_GETITEMADDDATA
 * \brief Gets the additional data of the item
 *
 * \code
 * SVM_GETITEMADDDATA
 * int nItem;
 * HSVITEM hsvi;
 *
 * wParam = (WPARAM)nItem;
 * lParam = (LPARAM)hsvi;
 * \endcode
 *
 * \param nItem scrollview item index to access.
 *              If hsvi is not zero, nItem will be ignored.
 * \param hsvi scrollview item handle to access.
 *
 * \return item additional data.
 */
#define SVM_GETITEMADDDATA         0xF333

/**
 * \def SVM_SETITEMADDDATA
 * \brief Sets the additional data of the item
 *
 * \code
 * SVM_SETITEMADDDATA
 * int nitem;
 * int addData;
 *
 * wParam = nitem;
 * lParam = addData;
 * \endcode
 *
 * \param nitem item index.
 * \param addData additional data
 *
 * \return old item additional data.
 */
#define SVM_SETITEMADDDATA         0xF334

/**
 * \def SVM_REFRESHITEM
 * \brief refresh the item
 *
 * \code
 * SVM_REFRESHITEM
 * int nitem;
 * HSVITEM hsvi;
 *
 * wParam = nitem;
 * lParam = hsvi;
 * \endcode
 *
 * \param nitem item index. If hsvi is not zero, nItem will be ignored.
 * \param hsvi item handle.
 *
 * \return 0.
 */
#define SVM_REFRESHITEM            0xF335

/**
 * \def SVM_SETITEMHEIGHT
 * \brief Sets the height of an item
 *
 * \code
 * SVM_SETITEMHEIGHT
 * int nitem;
 * int height;
 *
 * wParam = nitem;
 * lParam = height;
 * \endcode
 *
 * \param nitem item index.
 * \param height new height of the item
 *
 * \return old item height.
 */
#define SVM_SETITEMHEIGHT         0xF336

/**
 * \def SVM_GETFIRSTVISIBLEITEM
 * \brief Gets the index of the first visible item
 *
 * \code
 * SVM_GETFIRSTVISIBLEITEM
 *
 * \return Item index.
 */
#define SVM_GETFIRSTVISIBLEITEM   0xF337


    /** @} end of ctrl_scrollview_msgs */

    /**
     * \defgroup ctrl_scrollview_ncs Notification codes of scrollview control
     * @{
     */

/** indicates clicking on the item */
#define SVN_CLICKED             1

/** indicates the hilighted item changed */
#define SVN_SELCHANGED          2

    /** @} end of ctrl_scrollview_ncs */

    /** @} end of ctrl_scrollview */

    /** @} end of controls */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_CTRL_SCROLLVIEW_H */

