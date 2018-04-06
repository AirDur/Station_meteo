/*
** $Id: menu.c,v 1.53 2004/12/30 03:43:07 weiym Exp $
**
** menu.c: The Menu module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999.04.09
**
** Current maintainer: Wei Yongming.
**
** Used abbreviations in this file:
**      Menu: mnu
**      Popup: ppp
**      Identifier: id
**      Mnemonic: mnic
**      Normal: nml
**      Item: itm
**      Modify records:
*/

/*
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


/*
** TODO:
*/ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "control.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "blockheap.h"
#include "menu.h"

/************************* MENUBAR allocation ********************************/
static BLOCKHEAP MBHeap;
inline static void InitFreeMBList (void)
{
    InitBlockDataHeap (&MBHeap, sizeof (MENUBAR), SIZE_MB_HEAP);
}

inline static PMENUBAR MenuBarAlloc (void)
{
    return (PMENUBAR) BlockDataAlloc (&MBHeap);
}

inline static void FreeMenuBar (PMENUBAR pmb)
{
    BlockDataFree (&MBHeap, pmb);
}

inline static void DestroyFreeMBList (void)
{
    DestroyBlockDataHeap (&MBHeap);
}

/************************* MENUITEM allocation *******************************/
static BLOCKHEAP MIHeap;
inline static void InitFreeMIList (void)
{
    InitBlockDataHeap (&MIHeap, sizeof (MENUITEM), SIZE_MI_HEAP);
}

inline static PMENUITEM MenuItemAlloc (void)
{
    return (PMENUITEM) BlockDataAlloc (&MIHeap);
}

inline static void FreeMenuItem (MENUITEM* pmi)
{
    BlockDataFree (&MIHeap, pmi);
}

inline static void DestroyFreeMIList (void)
{
    DestroyBlockDataHeap (&MIHeap);
}

/************************* TRACKMENUITEM heap *******************************/
static BLOCKHEAP TMIHeap;
inline static void InitFreeTMIList (void)
{
    InitBlockDataHeap (&TMIHeap, sizeof (TRACKMENUINFO), SIZE_TMI_HEAP);
}

inline static PTRACKMENUINFO TrackMenuInfoAlloc (void)
{
    return (PTRACKMENUINFO) BlockDataAlloc (&TMIHeap);
}

inline static void FreeTrackMenuInfo (TRACKMENUINFO* ptmi)
{
    BlockDataFree (&TMIHeap, ptmi);
}

inline static void DestroyFreeTMIList(void)
{
    DestroyBlockDataHeap (&TMIHeap);
}

/************************* Module initialization *****************************/

static BITMAP bmp_menuitem;

BOOL InitMenu (void)
{
    InitFreeMBList ();
    InitFreeMIList ();
    InitFreeTMIList ();

    return InitBitmap (HDC_SCREEN, 16, 12, 0, NULL, &bmp_menuitem);
}

/************************* Module termination *******************************/
void TerminateMenu (void)
{
    DestroyFreeTMIList ();
    DestroyFreeMBList ();
    DestroyFreeMIList ();

    UnloadBitmap (&bmp_menuitem);
}

/***************************** Menu creation *******************************/
HMENU GUIAPI LoadMenuFromFile (const char* filename, int id)
{
    return 0;
}

HMENU GUIAPI CreateMenu (void)
{
    PMENUBAR pmb;

    if (!(pmb = MenuBarAlloc ()))
        return 0;
    
    pmb->category = TYPE_HMENU;
    pmb->type = TYPE_MENUBAR;
    pmb->head = NULL;

    return (HMENU)pmb;
}

HMENU GUIAPI CreatePopupMenu (PMENUITEMINFO pmii)
{
    PMENUITEM pmi;
    
    if (!(pmi = MenuItemAlloc ()))
        return 0;
   
    pmi->category = TYPE_HMENU;
    pmi->type = TYPE_PPPMENU;
    pmi->next = NULL;
    pmi->submenu = NULL;

    pmi->mnutype         = pmii->type;
    pmi->mnustate        = pmii->state;
    pmi->id              = pmii->id;
    pmi->hbmpChecked     = pmii->hbmpChecked;
    pmi->hbmpUnchecked   = pmii->hbmpUnchecked;
    pmi->itemdata        = pmii->itemdata;

    // copy string.
    if (pmii->type == MFT_STRING || pmii->type == MFT_BMPSTRING) {
        int len = strlen ((char*)pmii->typedata);
        pmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
    }
    else
        pmi->typedata = pmii->typedata;

    return (HMENU)pmi;
}

HMENU GUIAPI CreateSystemMenu (HWND hwnd, DWORD dwStyle)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(SysText[5]);

    hmnu = CreatePopupMenu (&mii);

    memset (&mii, 0, sizeof(MENUITEMINFO));

    if (dwStyle & WS_MINIMIZEBOX) {
        mii.type        = MFT_STRING;
        mii.state       = 0;
        mii.id          = SC_MINIMIZE;
        mii.typedata    = (DWORD)GetSysText(SysText[6]);
        InsertMenuItem(hmnu, 0, TRUE, &mii);
    }

    if (dwStyle & WS_MAXIMIZEBOX) {
        mii.type        = MFT_STRING;
        if (dwStyle & WS_MAXIMIZE)
            mii.state   = MFS_DISABLED;
        else
            mii.state   = 0;
        mii.id          = SC_MAXIMIZE;
        mii.typedata    = (DWORD)GetSysText(SysText[7]);
        InsertMenuItem(hmnu, 1, TRUE, &mii);

        mii.type        = MFT_STRING;
        if (dwStyle & WS_MAXIMIZE)
            mii.state   = 0;
        else
            mii.state   = MFS_DISABLED;
        mii.id          = SC_RESTORE;
        mii.typedata    = (DWORD)GetSysText(SysText[8]);
        InsertMenuItem(hmnu, 2, TRUE, &mii);
    }

    mii.type        = MFT_SEPARATOR;
    mii.state       = 0;
    mii.id          = 0;
    mii.typedata    = 0;
    InsertMenuItem(hmnu, 3, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.state       = 0;
    mii.id          = SC_CLOSE;
    mii.typedata    = (DWORD)GetSysText(SysText[9]);
    InsertMenuItem(hmnu, 4, TRUE, &mii);

    return hmnu;
}

static BOOL mnuInsertMenuItem (PMENUITEM pmi, PMENUITEM pnewmi,
                int item, BOOL flag)
{
    PMENUITEM ptmpmi;
    int index;
    
    if (flag) {
        // Insert this new menu item at the start.
        if (item == 0) {
            pnewmi->next = pmi;
            return TRUE;
        }
        
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pnewmi;
                pnewmi->next = ptmpmi;
                return FALSE;
            }
            
            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (item == pmi->id) {
            pnewmi->next = pmi;
            return TRUE;
        }
        
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pnewmi;
                pnewmi->next = ptmpmi;
                return FALSE;
            }
            
            pmi = pmi->next;
        }
    }

    // append this new menu item at the end.
    pmi->next = pnewmi;

    return FALSE;
}

int GUIAPI InsertMenuItem (HMENU hmnu, int item, 
                            BOOL flag, PMENUITEMINFO pmii)
{
    PMENUBAR pmb;
    PMENUITEM pnewmi, pmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE; 
    
    if (!(pnewmi = MenuItemAlloc ())) 
        return ERR_RES_ALLOCATION;

    pnewmi->category        = TYPE_HMENU;
    pnewmi->type            = TYPE_NMLMENU;
    pnewmi->mnutype         = pmii->type;
    pnewmi->mnustate        = pmii->state;
    pnewmi->id              = pmii->id;
    pnewmi->hbmpChecked     = pmii->hbmpChecked;
    pnewmi->hbmpUnchecked   = pmii->hbmpUnchecked;
    pnewmi->itemdata        = pmii->itemdata;
    pnewmi->next            = NULL;
    pnewmi->submenu         = (PMENUITEM)(pmii->hsubmenu);

    // copy string.
    if (pmii->type == MFT_STRING || pmii->type == MFT_BMPSTRING) {
        int len = strlen ((char*)pmii->typedata);
        pnewmi->typedata = (DWORD)FixStrAlloc (len);
        if (len > 0)
            strcpy ((char*)pnewmi->typedata, (char*)pmii->typedata);
    }
    else
        pnewmi->typedata = pmii->typedata;
    
    if (pmb->type == TYPE_MENUBAR) {
        pmi = pmb->head;
        
        if (!pmi)
            pmb->head = pnewmi;
        else {
            if (mnuInsertMenuItem (pmb->head, pnewmi, item, flag))
                pmb->head = pnewmi;
        }
    }
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM)hmnu;

        if (!pmi->submenu)
            pmi->submenu = pnewmi;
        else {
            if (mnuInsertMenuItem (pmi->submenu, pnewmi, item, flag))
                pmi->submenu = pnewmi;
        }
    }
    else
        return ERR_INVALID_HMENU;

   return 0;
}

static void mnuDeleteMenuItem (PMENUITEM pmi)
{
    PMENUITEM ptmpmi;
    PMENUITEM psubmi;

    if (pmi->submenu) {
        psubmi = pmi->submenu;
        while (psubmi) {
            ptmpmi = psubmi->next;
            mnuDeleteMenuItem (psubmi);
            psubmi = ptmpmi;
        }
    }

    if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
        FreeFixStr ((void*)pmi->typedata);
        
    FreeMenuItem (pmi);
}

static PMENUITEM mnuRemoveMenuItem (PMENUITEM phead, int item, BOOL flag)
{
    int index;
    PMENUITEM pmi, ptmpmi;
    
    if (!phead) return NULL;
    
    if (flag) {
    
        if (item == 0) {
            pmi = phead->next;
            if (!phead->submenu) 
                mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                
                if (!ptmpmi->submenu)
                    mnuDeleteMenuItem (ptmpmi);

                return phead;
            }

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (phead->id == item) {
            pmi = phead->next;
            if (!phead->submenu) 
                mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                
                if (!ptmpmi->submenu)
                    mnuDeleteMenuItem (ptmpmi);

                return phead;
            }

            pmi = pmi->next;
        }
    }

    return phead;
}

int GUIAPI RemoveMenu (HMENU hmnu, int item, UINT flags)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmb->head = mnuRemoveMenuItem (pmb->head, item, 
            flags & MF_BYPOSITION);
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi->submenu = mnuRemoveMenuItem (pmi->submenu, item, 
            flags & MF_BYPOSITION);
    }
    else
        return ERR_INVALID_HMENU;   // this is a normal menu item. 

    return 0;
}

static PMENUITEM mnuDeleteMenu (PMENUITEM phead, int item, BOOL flag)
{
    int index;
    PMENUITEM pmi, ptmpmi;
    
    if (!phead) return NULL;
    
    if (flag) {
    
        if (item == 0) {
            pmi = phead->next;
            mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        index = 1;
        while (pmi->next) {

            if (index == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                mnuDeleteMenuItem (ptmpmi);
                return phead;
            }

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        if (phead->id == item) {
            pmi = phead->next;
            mnuDeleteMenuItem (phead);
            return pmi;
        }
        
        pmi = phead;
        while (pmi->next) {

            if (pmi->next->id == item) {
                ptmpmi = pmi->next;
                pmi->next = pmi->next->next;
                mnuDeleteMenuItem (ptmpmi);
                return phead;
            }

            pmi = pmi->next;
        }
    }

    return phead;
}

int GUIAPI DeleteMenu (HMENU hmnu, int item, UINT flags)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmb->head = mnuDeleteMenu (pmb->head, item, 
            flags & MF_BYPOSITION);
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi->submenu = mnuDeleteMenu (pmi->submenu, item, 
            flags & MF_BYPOSITION);
    }
    else
        return ERR_INVALID_HMENU;   // this is a normal menu item. 

    return 0;
}

int GUIAPI DestroyMenu (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi, ptmpmi;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR) {
        pmi = pmb->head;
        while (pmi) {
            ptmpmi = pmi->next;
            mnuDeleteMenuItem (pmi);
            pmi = ptmpmi;
        }

        FreeMenuBar (pmb);
    }
    else
        mnuDeleteMenuItem ((PMENUITEM)hmnu);

    return 0;
}

int GUIAPI IsMenu (HMENU hmnu)
{
    PMENUBAR pMenu = (PMENUBAR)hmnu;

    if (pMenu->category != TYPE_HMENU) return 0;

    return pMenu->type;
}

/*************************** Menu properties support *************************/
int GUIAPI GetMenuItemCount (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int count;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;
    
    count = 0;
    while (pmi) {
        count ++;
        pmi = pmi->next;
    }
    
    return count;
}

static PMENUITEM mnuGetMenuItem (HMENU hmnu, int item, BOOL flag)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return NULL; 
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;

    if (flag) {
        index = 0;
        while (pmi) {
            if (index == item)
                return pmi;

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        while (pmi) {
            if (pmi->id == item)
                return pmi;

            pmi = pmi->next;
        }
    }

    return NULL; 
}

int GUIAPI GetMenuItemID (HMENU hmnu, int pos)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return ERR_INVALID_HANDLE;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        pmi = (PMENUITEM) hmnu;
    
    index = 0;
    while (pmi) {

        if (index == pos)
            return pmi->id;

        index ++;
        pmi = pmi->next;
    }
    
    return ERR_INVALID_POS;
}

HMENU GUIAPI GetPopupSubMenu (HMENU hpppmnu)
{
    PMENUITEM pmi;

    pmi = (PMENUITEM) hpppmnu;

    if (pmi->category != TYPE_HMENU)
        return 0;
    
    if (pmi->type != TYPE_PPPMENU)
        return 0;

    return (HMENU)(pmi->submenu);
}

HMENU GUIAPI StripPopupHead (HMENU hpppmnu)
{
    PMENUITEM pmi;
    HMENU hsubmenu;

    pmi = (PMENUITEM) hpppmnu;

    if (pmi->category != TYPE_HMENU)
        return 0;
    
    if (pmi->type != TYPE_PPPMENU)
        return 0;

    hsubmenu = (HMENU)(pmi->submenu);

    if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
        FreeFixStr ((void*)pmi->typedata);
    FreeMenuItem (pmi);

    return hsubmenu;
}

HMENU GUIAPI GetSubMenu (HMENU hmnu, int pos)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int index;

    pmb = (PMENUBAR) hmnu;

    if (pmb->category != TYPE_HMENU)
        return 0;
    
    if (pmb->type == TYPE_MENUBAR)
        pmi = pmb->head;
    else if (pmb->type == TYPE_PPPMENU) {
        pmi = (PMENUITEM) hmnu;
        pmi = pmi->submenu;
    }
    else
        return 0;   // this is a normal menu item. 
    
    index = 0;
    while (pmi) {

        if (index == pos)
            return (HMENU)(pmi->submenu);

        index ++;
        pmi = pmi->next;
    }
    
    return 0;
}

int GUIAPI GetMenuItemInfo (HMENU hmnu, int item, 
                            BOOL flag, PMENUITEMINFO pmii)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flag))) return ERR_INVALID_HMENU;

    if (pmii->mask & MIIM_CHECKMARKS) {
        pmii->hbmpChecked   = pmi->hbmpChecked;
        pmii->hbmpUnchecked = pmi->hbmpUnchecked;
    }

    if (pmii->mask & MIIM_DATA) {
        pmii->itemdata      = pmi->itemdata;
    }

    if (pmii->mask & MIIM_ID) {
        pmii->id            = pmi->id;
    }

    if (pmii->mask & MIIM_STATE) {
        pmii->state         = pmi->mnustate;
    }

    if (pmii->mask & MIIM_SUBMENU) {
        pmii->hsubmenu      = (HMENU)(pmi->submenu);
    }

    if (pmii->mask & MIIM_TYPE) {
        pmii->type          = pmi->mnutype;
        if (pmii->type == MFT_STRING)
            strncpy ((char*)(pmii->typedata), (char*)(pmi->typedata), 
                pmii->cch);
        else
            pmii->typedata  = pmi->typedata;
    }

    return 0;
}

int GUIAPI SetMenuItemInfo (HMENU hmnu, int item, 
                            BOOL flag, PMENUITEMINFO pmii)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flag))) return ERR_INVALID_HMENU;

    if (pmii->mask & MIIM_CHECKMARKS) {
        pmi->hbmpChecked   = pmii->hbmpChecked;
        pmi->hbmpUnchecked = pmii->hbmpUnchecked;
    }

    if (pmii->mask & MIIM_DATA) {
        pmi->itemdata      = pmii->itemdata;
    }

    if (pmii->mask & MIIM_ID) {
        pmi->id            = pmii->id;
    }

    if (pmii->mask & MIIM_STATE) {
        pmi->mnustate         = pmii->state;
    }

    if (pmii->mask & MIIM_SUBMENU) {
        pmi->submenu      = (PMENUITEM)(pmii->hsubmenu);
    }

    if (pmii->mask & MIIM_TYPE) {

        if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING)
            FreeFixStr ((char*) pmi->typedata);
        
        pmi->mnutype          = pmii->type;
        if (pmi->mnutype == MFT_STRING || pmi->mnutype == MFT_BMPSTRING) {
            int len = strlen ((char*)pmii->typedata);
            pmi->typedata = (DWORD)FixStrAlloc (len);
            if (len > 0)
                strcpy ((char*)pmi->typedata, (char*)pmii->typedata);
        }
        else
            pmi->typedata = pmii->typedata;
    }

    return 0;
}

UINT GUIAPI EnableMenuItem (HMENU hmnu, int item, UINT flags)
{
    PMENUITEM pmi;
    UINT prevstate = 0xFFFFFFFF; 

    if (!(pmi = mnuGetMenuItem (hmnu, item, flags & MF_BYPOSITION)))
        return prevstate;

    prevstate = 0x0000000F & pmi->mnustate;

    pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | flags;

    return prevstate;
}

int GUIAPI CheckMenuRadioItem (HMENU hmnu, int first, int last, 
                            int checkitem, UINT flags)
{
    PMENUITEM pmi;
    int index;

    if (!(pmi = mnuGetMenuItem (hmnu, first, flags & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    index = first;
    if (flags & MF_BYPOSITION) {
        while (pmi) {
            pmi->mnutype |= MFT_RADIOCHECK;

            if (index == checkitem)
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_CHECKED;
            else
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_UNCHECKED;
            
            if (index == last)
                break;

            index ++;
            pmi = pmi->next;
        }
    }
    else {
        while (pmi) {
            pmi->mnutype |= MFT_RADIOCHECK;
        
            if (pmi->id == checkitem)
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_CHECKED;
            else
                pmi->mnustate = (0xFFFFFFF0 & pmi->mnustate) | MFS_UNCHECKED;
            
            if (pmi->id == last)
                break;

            pmi = pmi->next;
        }
    }

    return 0;
}

int GUIAPI SetMenuItemBitmaps (HMENU hmnu, int item, UINT flags, 
                            PBITMAP hBmpUnchecked, PBITMAP hBmpChecked)
{
    PMENUITEM pmi;

    if (!(pmi = mnuGetMenuItem (hmnu, item, flags & MF_BYPOSITION)))
        return ERR_INVALID_HMENU;

    pmi->hbmpChecked = hBmpChecked;
    pmi->hbmpUnchecked = hBmpUnchecked;

    return 0;
}

#ifdef _DEBUG

void mnuDumpMenuItem (PMENUITEM pmi)
{
    PMENUITEM ptmpmi;
    
    printf ("Menu Item (0x%p) Information:\n", pmi);

    printf ("\tdata category:      %d\n", pmi->category);
    printf ("\tdata type:       %d\n", pmi->type);
    printf ("\tmenu type:       %#x\n", pmi->mnutype);
    printf ("\tmenu state:      %#x\n", pmi->mnustate);
    printf ("\tmenu id:         %d\n", pmi->id);
    printf ("\tchecked bitmap:  0x%p\n", pmi->hbmpChecked);
    printf ("\tunchecked bitmap: 0x%p\n", pmi->hbmpUnchecked);
    printf ("\titem data:       %lu\n", pmi->itemdata);
    if (pmi->mnutype == MFT_STRING)
        printf ("\tstring:         %s\n", (char*)pmi->typedata);
    else
        printf ("\ttype data:      %lu\n", pmi->typedata);
    printf ("\tnext item:       0x%p\n", pmi->next);
    
    if (pmi->submenu) {
        ptmpmi = pmi->submenu;
        while (ptmpmi) {
            mnuDumpMenuItem (ptmpmi);
            ptmpmi = ptmpmi->next;
        }
    }

    printf ("End of Info of Menu Item: 0x%p\n", pmi);
}

void DumpMenu (HMENU hmnu)
{
    PMENUBAR pmb;
    PMENUITEM pmi;

    pmb = (PMENUBAR) hmnu;
    if (pmb->category != TYPE_HMENU) {
        printf ("hmnu is not a valid menu handle.\n");
        return;
    }
    
    if (pmb->type == TYPE_MENUBAR) {
        printf ("hmnu is a menu bar.\n");
        
        pmi = pmb->head;
        while (pmi) {
            mnuDumpMenuItem (pmi);
            pmi = pmi->next;
        }
    }
    else if (pmb->type == TYPE_PPPMENU) {
        printf ("hmnu is a popup menu.\n");
        pmi = (PMENUITEM)hmnu;
        mnuDumpMenuItem (pmi);
    }
    else {
        printf ("hmnu is a normal menu item.\n");
        pmi = (PMENUITEM)hmnu;
        mnuDumpMenuItem (pmi);
    }
}
#endif  // _DEBUG

/***************************** Menu owner ***********************************/
// Global function defined in Desktop module.

HMENU GUIAPI SetMenu (HWND hwnd, HMENU hmnu)
{
    PMAINWIN pWin;
    HMENU hOld;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return 0;

    hOld = pWin->hMenu;
    pWin->hMenu = hmnu;

    ((PMENUBAR)hmnu)->hwnd = hwnd;

    return hOld;
}

HMENU GUIAPI GetMenu (HWND hwnd)
{
    PMAINWIN pWin;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return 0;

    return pWin->hMenu;
}

HMENU GUIAPI GetSystemMenu (HWND hwnd, BOOL flag)
{
    PMAINWIN pWin;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return 0;

    return pWin->hSysMenu;
}

/**************************** Menu drawing support ***************************/
static void mnuGetMenuBarItemSize (PMENUITEM pmi, SIZE* size)
{
    if (pmi->mnutype & MFT_BITMAP) {
        size->cx = pmi->hbmpChecked->bmWidth;
    }
    else if (pmi->mnutype & MFT_SEPARATOR) {
        size->cx = GetMainWinMetrics (MWM_MENUSEPARATORX); 
    }
    else if (pmi->mnutype & MFT_OWNERDRAW) {
    }
    else {
#if 0
        size->cx = strlen ((char*) pmi->typedata)*GetSysCharWidth();
#else
        SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
        GetTextExtent (HDC_SCREEN, (char*) pmi->typedata, -1, size);
#endif
        if (pmi->mnutype & MFT_BMPSTRING) {
            size->cx += pmi->hbmpChecked->bmWidth;
            size->cx += GetMainWinMetrics (MWM_INTERMENUITEMX)>>1;
        }
    }

    pmi->h = size->cx;
    size->cy = GetMainWinMetrics (MWM_MENUBARY);
    return;
}

static void mnuDrawMenuBarItem (HDC hdc, PMENUITEM pmi, int x, int y, int w, int h)
{
    PBITMAP bmp;
    int height;
    int inter;

    height = GetMainWinMetrics (MWM_MENUBARY);
    inter = GetMainWinMetrics (MWM_INTERMENUITEMX)>>1;

    SetBrushColor (hdc, GetWindowElementColor (BKC_MENUBAR_NORMAL));
    FillBox (hdc, x - inter, y - 3, w + (inter<<1), h + 3);

    if (pmi->mnutype & MFT_BITMAP) {
    
        if (pmi->mnustate & MFS_CHECKED)
            bmp = pmi->hbmpChecked;
        else
            bmp = pmi->hbmpUnchecked;
            
        FillBoxWithBitmap (hdc, x, y,
                        pmi->h, 
                        height, 
                        bmp);
    }
    else if (pmi->mnutype & MFT_SEPARATOR) {
        
        SetPenColor (hdc, GetWindowElementColor(FGC_MENUBAR_NORMAL));
        MoveTo (HDC_SCREEN, 
            x + (pmi->h>>1), y);
        LineTo (HDC_SCREEN, 
            x + (pmi->h>>1), y + height);
    }
    else if (pmi->mnutype & MFT_OWNERDRAW) {
    }
    else {
        if (pmi->mnutype & MFT_BMPSTRING) {
            if (pmi->mnustate & MFS_CHECKED)
                bmp = pmi->hbmpChecked;
            else
                bmp = pmi->hbmpUnchecked;
            
            FillBoxWithBitmap (hdc, x, y, height, height, bmp);
            x += bmp->bmWidth + inter;
        }

        SelectFont (hdc, GetSystemFont (SYSLOGFONT_MENU));
        SetTextColor (hdc, GetWindowElementColor(FGC_MENUBAR_NORMAL));
        SetBkColor (hdc, GetWindowElementColor (BKC_MENUBAR_NORMAL));
        TextOut (hdc, x, y, (char *)pmi->typedata); 
    }

    return;
}

static int mnuGetNextMenuBarItem (PMENUBAR pmb, int pos)
{
    PMENUITEM pmi;
    int number;

    number = 0;
    pmi = pmb->head;
    while (pmi) {
        number ++;
        pmi = pmi->next;
    }

    number--;

    if (pos == number)
        return 0;
    else
        return pos + 1;
}

static int mnuGetPrevMenuBarItem (PMENUBAR pmb, int pos)
{
    PMENUITEM pmi;
    int number;

    number = 0;
    pmi = pmb->head;
    while (pmi) {
        number ++;
        pmi = pmi->next;
    }

    number--;

    if (pos == 0)
        return number;
    else
        return pos - 1;
}

HMENU GUIAPI GetMenuBarItemRect (HWND hwnd, int pos, RECT* prc)
{
    PMAINWIN pWin;
    PMENUBAR pmb;
    PMENUITEM pmi;
    int x, y, inter, h;
    int count;
    SIZE size;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return 0;

    if (!pWin->hMenu) return 0;

    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) return 0;
    if (pmb->type != TYPE_MENUBAR) return 0;

    x = pWin->cl - pWin->left;
    x += GetMainWinMetrics (MWM_MENUBAROFFX);

    y = pWin->ct - pWin->top; 
    h = GetMainWinMetrics (MWM_MENUBARY)
        + GetMainWinMetrics (MWM_MENUBAROFFY); 

    y -= h;

    inter = GetMainWinMetrics (MWM_INTERMENUITEMX);

    count = 0;
    pmi = pmb->head;
    while (pmi) {

        mnuGetMenuBarItemSize (pmi, &size);

        if (count == pos)
            break;

        x = x + inter + size.cx;
        count ++;
        
        pmi = pmi->next;
    }

    if (pmi == NULL) return 0;

    prc->left = x;
    prc->top  = y;
    prc->bottom = y + h - 1;
    prc->right = x + size.cx - 1;

    return (HMENU) pmi;
}

static BOOL mnuGetMenuBarRect (HWND hwnd, RECT* prc)
{
    PMAINWIN pWin;
    int w, h;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return FALSE;
    
    prc->left = pWin->cl - pWin->left;
    w = pWin->cr - pWin->cl + 1;
    h = GetMainWinMetrics (MWM_MENUBARY) 
        + (GetMainWinMetrics (MWM_MENUBAROFFY)<<1);
    prc->top = pWin->ct - pWin->top; 
    prc->top -= GetMainWinMetrics (MWM_MENUBARY);
    prc->top -= GetMainWinMetrics (MWM_MENUBAROFFY)<<1;

    prc->right = prc->left + w - 1;
    prc->bottom = prc->top + h - 1;

    return TRUE;
}

BOOL GUIAPI HiliteMenuBarItem (HWND hwnd, int pos, UINT flags)
{
    PMENUITEM pmi;
    RECT rcBar;
    RECT rc;
    HDC hdc;
    int inter;

    pmi = (PMENUITEM)GetMenuBarItemRect (hwnd, pos, &rc);

    if (!pmi) return FALSE;
    
    inter = GetMainWinMetrics (MWM_INTERMENUITEMX)>>1;

    hdc = GetDC (hwnd);

    mnuGetMenuBarRect (hwnd, &rcBar);
    ClipRectIntersect (hdc, &rcBar);

    if (flags == HMF_UPITEM) {
       Draw3DThinFrameEx (hdc, hwnd, rc.left - inter, rc.top - 3, 
                           rc.right + inter, rc.bottom - 1, 
                           DF_3DBOX_NORMAL | DF_3DBOX_NOTFILL, 0);
    }
    else if (flags == HMF_DOWNITEM) {
       mnuDrawMenuBarItem (hdc, pmi, rc.left + 1, rc.top + 1, 
            RECTW (rc) - 2, RECTH (rc) - 2);
       Draw3DThinFrameEx (hdc, hwnd, rc.left - inter, rc.top - 2, 
                           rc.right + inter, rc.bottom - 2,
                           DF_3DBOX_PRESSED | DF_3DBOX_NOTFILL, 0);
    }
    else if (flags == HMF_DEFAULT)
        mnuDrawMenuBarItem (hdc, pmi, rc.left, rc.top, 
            RECTW (rc) + 3, RECTH (rc) + 1);

    ReleaseDC (hdc);

    return TRUE;
}

int MenuBarHitTest (HWND hwnd, int mx, int my)
{
    PMAINWIN pWin;
    PMENUBAR pmb;
    PMENUITEM pmi;
    int x, y, inter, h;
    RECT rc;
    int count;
    SIZE size;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return -1;

    if (!pWin->hMenu) return -1;

    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) return -1;
    if (pmb->type != TYPE_MENUBAR) return -1;

    x = pWin->left;
    x += GetMainWinMetrics (MWM_MENUBAROFFX);

    y = pWin->ct; 
    y -= GetMainWinMetrics (MWM_MENUBARY);
    y -= GetMainWinMetrics (MWM_MENUBAROFFY);

    h = GetMainWinMetrics (MWM_MENUBARY) 
        + GetMainWinMetrics (MWM_MENUBAROFFY);

    inter = GetMainWinMetrics (MWM_INTERMENUITEMX);

    rc.top  = y;
    rc.bottom = y + h;

    count = 0;
    pmi = pmb->head;
    while (pmi) {

        mnuGetMenuBarItemSize (pmi, &size);
        rc.left = x;
        rc.right = x + size.cx;

        if (PtInRect (&rc, mx, my))
            return count;

        x = x + inter + size.cx;
        count ++;
        
        pmi = pmi->next;
    }

    return -1;
}

void DrawMenuBarHelper (const MAINWIN* pWin, HDC hdc, const RECT* pClipRect)
{
    PMENUBAR pmb;
    PMENUITEM pmi;
    int x, y, inter;
    int w, h;
    SIZE size;
    RECT rc;

    if (!pWin->hMenu) return;

    pmb = (PMENUBAR) (pWin->hMenu);
    if (pmb->category != TYPE_HMENU) return;

    x = pWin->cl - pWin->left;
    w = pWin->right - pWin->cl - (pWin->cl - pWin->left);
    h = GetMainWinMetrics (MWM_MENUBARY) 
        + (GetMainWinMetrics (MWM_MENUBAROFFY)<<1);
    y = pWin->ct - pWin->top;
    y -= h;

    rc.left = x;
    rc.top = y;
    rc.right = x + w;
    rc.bottom = y + h;
    SelectClipRect (hdc, &rc);
    
    if (pClipRect)
        ClipRectIntersect (hdc, pClipRect);

    SetBrushColor (hdc, GetWindowElementColor(BKC_MENUBAR_NORMAL));
    FillBox (hdc, x, y, w + 3, h + 3);

    SelectFont (hdc, GetSystemFont (SYSLOGFONT_MENU));
    SetBkColor (hdc, GetWindowElementColor(BKC_MENUBAR_NORMAL));
    SetTextColor (hdc, GetWindowElementColor(FGC_MENUBAR_NORMAL));

    x += GetMainWinMetrics (MWM_MENUBAROFFX);
    y += GetMainWinMetrics (MWM_MENUBAROFFY);
    inter = GetMainWinMetrics (MWM_INTERMENUITEMX);
    pmi = pmb->head;
    while (pmi) {
        if (x > pWin->cr) break;

        mnuGetMenuBarItemSize (pmi, &size);
        mnuDrawMenuBarItem (hdc, pmi, x, y, size.cx, h);

        x = x + inter + size.cx;
        pmi = pmi->next;
    }
    
}

void GUIAPI DrawMenuBar (HWND hwnd)
{
    PMAINWIN pWin;
    HDC hdc;

    if (!(pWin = CheckAndGetMainWindowPtr (hwnd))) return;

    if (!pWin->hMenu) return;
    
    hdc = GetDC (hwnd);
    DrawMenuBarHelper (pWin, hdc, NULL);
    ReleaseDC (hdc);
}

static void mnuGetPopupMenuExtent (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO prevtmi;
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    int w = 0, h = 0;
    int mih;
    int inter;
    int mioffx, minx;
    SIZE size;
    int offx, offy;
    BOOL has_sub = FALSE;
    BOOL has_txt = FALSE;

    mih = GetMainWinMetrics (MWM_MENUITEMY);
    inter = GetMainWinMetrics (MWM_INTERMENUITEMY);
    mioffx = GetMainWinMetrics (MWM_MENUITEMOFFX);
    minx = GetMainWinMetrics (MWM_MENUITEMMINX);
    
    ptmi->rc.right = ptmi->rc.left;
    ptmi->rc.bottom = ptmi->rc.top + GetMainWinMetrics (MWM_MENUTOPMARGIN);
    
    //h = ptmi->rc.bottom - ptmi->rc.top;

    if (pmi->mnutype & MFT_OWNERDRAW) {
    }
    else if (pmi->mnutype & MFT_SEPARATOR) {
        pmi->h = GetMainWinMetrics (MWM_MENUSEPARATORY) + (inter<<1);
        if (minx > w)
            w = minx;
    }
    else if (pmi->mnutype & MFT_BITMAP) {
        pmi->h = mih + (inter<<1);
        if (pmi->hbmpChecked->bmWidth > w)
            w = pmi->hbmpChecked->bmWidth;
    }
    else {
        SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
        GetTabbedTextExtent (HDC_SCREEN, (char*)pmi->typedata, -1, &size);

        if (pmi->mnutype & MFT_BMPSTRING)
            size.cx += pmi->hbmpChecked->bmWidth;
        if (size.cx > w) w = size.cx;

        pmi->h = mih + (inter<<1);
        
        if (pmi->type == TYPE_PPPMENU)
            pmi->h += (inter<<1);

        //h += pmi->h;
        has_txt = TRUE;
    }
    ptmi->rc.bottom += pmi->h;
    
    if (pmi->type == TYPE_PPPMENU) {
        psubmi = pmi->submenu;
    }
    else
        psubmi = pmi->next;

    while (psubmi) {

        if (psubmi->mnutype & MFT_OWNERDRAW) {
        }
        else if (psubmi->mnutype & MFT_SEPARATOR) {
            psubmi->h = GetMainWinMetrics (MWM_MENUSEPARATORY) + (inter<<1);
            if (minx > w)
                w = minx;
        }
        else if (psubmi->mnutype & MFT_BITMAP) {
            psubmi->h = mih + (inter<<1);
            if (psubmi->hbmpChecked->bmWidth > w)
                w = psubmi->hbmpChecked->bmWidth;
        }
        else {
            psubmi->h = mih + (inter<<1);

            SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
            GetTabbedTextExtent (HDC_SCREEN, (char*)psubmi->typedata, -1, &size);

            if (pmi->mnutype & MFT_BMPSTRING)
                size.cx += pmi->hbmpChecked->bmWidth;

            if (size.cx > w) w = size.cx;
        }

        if (psubmi->submenu)
            has_sub = TRUE;

        ptmi->rc.bottom += psubmi->h;
        //h += psubmi->h;

        psubmi = psubmi->next;
    }
    ptmi->rc.bottom += GetMainWinMetrics (MWM_MENUBOTTOMMARGIN);
    //h += GetMainWinMetrics (MWM_MENUBOTTOMMARGIN);

    if (has_txt) w += mioffx;
    if (has_sub) w += mioffx;
    w += GetMainWinMetrics (MWM_MENULEFTMARGIN);
    w += GetMainWinMetrics (MWM_MENURIGHTMARGIN);

    ptmi->rc.right += w;

    /* adjust the rect according to the alignment flag */
    if (ptmi->flags & TPM_CENTERALIGN) {
        ptmi->rc.left -= w >> 1;
        ptmi->rc.right -= w >> 1;
    }

    if (ptmi->flags & TPM_RIGHTALIGN) {
        ptmi->rc.left -= w;
        ptmi->rc.right -= w;
    }

    h = ptmi->rc.bottom - ptmi->rc.top + 1;
    if (ptmi->flags & TPM_VCENTERALIGN) {
        ptmi->rc.top -= h >> 1;
        ptmi->rc.bottom -= h >> 1;
    }

    if (ptmi->flags & TPM_BOTTOMALIGN) {
        ptmi->rc.top -= h;
        ptmi->rc.bottom -= h;
    }

    /* adjust the rect according to the screen */

    if (ptmi->rc.right > g_rcScr.right) {

        if ((prevtmi = ptmi->prev))
            offx = prevtmi->rc.right - prevtmi->rc.left + w;
        else
            offx = ptmi->rc.right - g_rcScr.right;

        if (ptmi->rc.left - offx < 0) {
            ptmi->rc.left = 0;
            ptmi->rc.right = w;
        }
        else {
            ptmi->rc.left -= offx;
            ptmi->rc.right -= offx;
        }
    }

    if (ptmi->rc.bottom > g_rcScr.bottom) {
        offy = ptmi->rc.bottom - g_rcScr.bottom;
        if (ptmi->rc.top - offy < 0) {
            ptmi->rc.top = 0;
            ptmi->rc.bottom = h;
        }
        else {
            ptmi->rc.top -= offy;
            ptmi->rc.bottom -= offy;
        }
    }

#if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN)
    ptmi->rc.right += 6;
    ptmi->rc.bottom +=2;
#endif
}

static const char * menubmp_check = 
"++++++++++++++++"
"++++++++++++..++"
"+++++++++++...++"
"++++++++++...+++"
"+++++++++...++++"
"++..++++...+++++"
"++...++...++++++"
"+++......+++++++"
"++++....++++++++"
"+++++..+++++++++"
"++++++++++++++++"
"++++++++++++++++"
;

static const char * menubmp_radio =
"++++++++++++++++"
"++++++++++++++++"
"++++++....++++++"
"+++++......+++++"
"++++........++++"
"++++........++++"
"++++........++++"
"++++........++++"
"+++++......+++++"
"++++++....++++++"
"++++++++++++++++"
"++++++++++++++++"
;

static const char * menubmp_sub =
"++++++++++++++++"
"++++++++++++++++"
"++++++.+++++++++"
"++++++..++++++++"
"++++++...+++++++"
"++++++....++++++"
"++++++....++++++"
"++++++...+++++++"
"++++++..++++++++"
"++++++.+++++++++"
"++++++++++++++++"
"++++++++++++++++"
;

#define MENUBMP_RADIO 1
#define MENUBMP_CHECK 2
#define MENUBMP_SUB   3

static PBITMAP mnuGetMenuItemBitmap (int id, BOOL hilite)
{
    int i, j;
    const char* menu_bmp;
    gal_pixel fg, bg;
   
    if (hilite) {
        fg = GetWindowElementColor (FGC_MENUITEM_HILITE);
        bg = GetWindowElementColor (BKC_MENUITEM_HILITE);
    }
    else {
        fg = GetWindowElementColor (FGC_MENUITEM_NORMAL);
        bg = GetWindowElementColor (BKC_MENUITEM_NORMAL);
    }

    switch (id) {
        case MENUBMP_RADIO:
                menu_bmp = menubmp_radio;
                break;
        case MENUBMP_CHECK:
                menu_bmp = menubmp_check;
                break;
        case MENUBMP_SUB:
                menu_bmp = menubmp_sub;
                break;
        default:
                return NULL;
    }

    for (i = 0; i < 12; i++) {
        for (j = 0; j < 16; j++) {
            if (*menu_bmp == '+')
                SetPixelInBitmap (&bmp_menuitem, j, i, bg);
            else
                SetPixelInBitmap (&bmp_menuitem, j, i, fg);

            menu_bmp ++;
        }
    }

    return &bmp_menuitem;
}

static PBITMAP mnuGetCheckBitmap (PMENUITEM pmi)
{
    if (pmi->mnutype & MFT_BMPSTRING) {
        if (pmi->mnustate & MFS_CHECKED)
            return pmi->hbmpChecked;
        else
            return pmi->hbmpUnchecked;
    }
    else if (!(pmi->mnustate & MFS_CHECKED))
        return NULL;

    // default system check bitmap
    if (pmi->mnutype & MFT_RADIOCHECK) {
        if (pmi->mnustate & MFS_HILITE)
            return mnuGetMenuItemBitmap (MENUBMP_RADIO, TRUE);
        return mnuGetMenuItemBitmap (MENUBMP_RADIO, FALSE);
    }
    else {
        if (pmi->mnustate & MFS_HILITE)
            return mnuGetMenuItemBitmap (MENUBMP_CHECK, TRUE);
        return mnuGetMenuItemBitmap (MENUBMP_CHECK, FALSE);
    }

    return NULL;
}

static PBITMAP mnuGetSubMenuBitmap (PMENUITEM pmi)
{
    if (pmi->mnustate & MFS_HILITE)
        return mnuGetMenuItemBitmap (MENUBMP_SUB, TRUE);
    return mnuGetMenuItemBitmap (MENUBMP_SUB, FALSE);
}

static int mnuShowPopupMenu (PTRACKMENUINFO ptmi)
{
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    int x, y, offy;
    
    int inter;
    int mioffx;
    int marginlx, marginrx;
    int marginy;

    PBITMAP bmp;
    
    // save screen under this popup menu.
#ifdef _USE_NEWGAL
    memset (&ptmi->savedbox, 0, sizeof (BITMAP));
    if (!GetBitmapFromDC (HDC_SCREEN, ptmi->rc.left, ptmi->rc.top, 
                    RECTW (ptmi->rc), RECTH (ptmi->rc), &ptmi->savedbox))
        return -1;
#else
    ptmi->savedbox = SaveCoveredScreenBox (ptmi->rc.left, ptmi->rc.top, 
                    RECTW (ptmi->rc), RECTH (ptmi->rc));

    if (ptmi->savedbox == NULL)
        return -1;
#endif

    inter = GetMainWinMetrics (MWM_INTERMENUITEMY);
    mioffx = GetMainWinMetrics (MWM_MENUITEMOFFX);
    marginlx = GetMainWinMetrics (MWM_MENULEFTMARGIN);
    marginrx = GetMainWinMetrics (MWM_MENURIGHTMARGIN);
    marginy = GetMainWinMetrics (MWM_MENUTOPMARGIN);

    // draw background.
    x = ptmi->rc.left;
    y = ptmi->rc.top;
#ifdef _FLAT_WINDOW_STYLE
    SetBrushColor (HDC_SCREEN, GetWindowElementColor(BKC_MENUITEM_NORMAL));
    FillBox (HDC_SCREEN, ptmi->rc.left, ptmi->rc.top, 
                    RECTW (ptmi->rc) - 1, RECTH (ptmi->rc) - 1);
    SetPenColor (HDC_SCREEN, GetWindowElementColor (WEC_FLAT_BORDER));
    Rectangle (HDC_SCREEN, ptmi->rc.left, ptmi->rc.top, 
                               ptmi->rc.right - 2, ptmi->rc.bottom - 2);
#ifndef _GRAY_SCREEN
    MoveTo (HDC_SCREEN, ptmi->rc.left + 1, ptmi->rc.bottom - 1);
    LineTo (HDC_SCREEN, ptmi->rc.right - 1, ptmi->rc.bottom - 1);
    LineTo (HDC_SCREEN, ptmi->rc.right - 1,  ptmi->rc.top + 1);
#endif
#else
    Draw3DThickFrameEx (HDC_SCREEN, HWND_DESKTOP, 
                        ptmi->rc.left, ptmi->rc.top, 
                        ptmi->rc.right, ptmi->rc.bottom, 
                        DF_3DBOX_NORMAL | DF_3DBOX_FILL, 
                        GetWindowElementColor(BKC_MENUITEM_NORMAL));
#endif
    
    x = ptmi->rc.left + marginlx;
    y = ptmi->rc.top  + marginy;
    if (pmi->mnutype & MFT_BITMAP) {
    
        if (pmi->mnustate & MFS_CHECKED)
            bmp = pmi->hbmpChecked;
        else
            bmp = pmi->hbmpUnchecked;
            
        FillBoxWithBitmap (HDC_SCREEN, x, y,
                        bmp->bmWidth, 
                        bmp->bmHeight, 
                        bmp);
    }
    else if (pmi->mnutype & MFT_SEPARATOR) {
    
        SetPenColor (HDC_SCREEN, GetWindowElementColor (WEC_3DFRAME_BOTTOM)); 
        MoveTo (HDC_SCREEN, 
            x, y + (pmi->h>>1) - inter + 1);
        LineTo (HDC_SCREEN, 
            ptmi->rc.right - marginrx, y + (pmi->h>>1) - inter + 1);

        SetPenColor (HDC_SCREEN, GetWindowElementColor (WEC_3DFRAME_TOP));
        MoveTo (HDC_SCREEN, 
            x, y + (pmi->h>>1) - inter + 2);
        LineTo (HDC_SCREEN, 
            ptmi->rc.right - marginrx, y + (pmi->h>>1) - inter + 2);
    }
    else if (pmi->mnutype & MFT_OWNERDRAW) {
    }
    else {

        SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
        SetBkColor (HDC_SCREEN, GetWindowElementColor(BKC_MENUITEM_NORMAL));
        
        if (pmi->type == TYPE_PPPMENU) {
            SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_PPPMENUTITLE));
            TextOut (HDC_SCREEN, x + mioffx, y + inter, 
                (char*)pmi->typedata);

            SetPenColor (HDC_SCREEN, GetWindowElementColor (FGC_MENUITEM_NORMAL));
            MoveTo (HDC_SCREEN, 
                x, 
                y + pmi->h - (inter<<1));
            LineTo (HDC_SCREEN, 
                ptmi->rc.right - marginrx, 
                y + pmi->h - (inter<<1));
            MoveTo (HDC_SCREEN, 
                x, 
                y + pmi->h - inter);
            LineTo (HDC_SCREEN,
                ptmi->rc.right - marginrx, 
                y + pmi->h - inter);
        }
        else {
            int old_mode, bmp_w = mioffx;

            // draw check bitmap
            bmp = mnuGetCheckBitmap (pmi);
            if (bmp) {
                offy = (pmi->h - bmp->bmHeight)>>1;
                FillBoxWithBitmap (HDC_SCREEN, x, y + offy,
                        bmp->bmWidth, pmi->h - (offy<<1), bmp);
                bmp_w = MAX (bmp->bmWidth, mioffx);
            }

            old_mode = SetBkMode (HDC_SCREEN, BM_TRANSPARENT);
            if (pmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_DISABLED));
            else
                SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_NORMAL));
            TextOut (HDC_SCREEN, x + bmp_w, y + inter, 
                (char*)pmi->typedata);
            SetBkMode (HDC_SCREEN, old_mode);

            if (pmi->submenu) {
                bmp = mnuGetSubMenuBitmap (pmi);
                offy = (pmi->h - bmp->bmHeight)>>1;
                FillBoxWithBitmap (HDC_SCREEN, 
                    ptmi->rc.right - mioffx - marginrx,
                    y + offy,
                    bmp->bmWidth, pmi->h - (offy<<1), bmp);
            }
        }
    }
    y += pmi->h;
    
    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;

    while (psubmi) {

        if (psubmi->mnutype & MFT_BITMAP) {
    
            if (psubmi->mnustate & MFS_CHECKED)
                bmp = psubmi->hbmpChecked;
            else
                bmp = psubmi->hbmpUnchecked;
            
            FillBoxWithBitmap (HDC_SCREEN, x, y,
                        bmp->bmWidth, 
                        bmp->bmHeight,
                        bmp);
        }
        else if (psubmi->mnutype & MFT_SEPARATOR) {
    
            SetPenColor (HDC_SCREEN, GetWindowElementColor (WEC_3DFRAME_BOTTOM)); 
            MoveTo (HDC_SCREEN, 
                x, y + (psubmi->h>>1) - inter + 1);
            LineTo (HDC_SCREEN, 
                ptmi->rc.right - marginrx, y + (psubmi->h>>1) - inter + 1);
                
            SetPenColor (HDC_SCREEN, GetWindowElementColor (WEC_3DFRAME_TOP));
            MoveTo (HDC_SCREEN, 
                x, y + (psubmi->h>>1) - inter + 2);
            LineTo (HDC_SCREEN, 
                ptmi->rc.right - marginrx, y + (psubmi->h>>1) - inter + 2);
        }
        else if (pmi->mnutype & MFT_OWNERDRAW) {
        }
        else {
            int old_mode, bmp_w = mioffx;

            // draw check bitmap
            bmp = mnuGetCheckBitmap (psubmi);
            if (bmp) {
                offy = (psubmi->h - bmp->bmHeight)>>1;
                FillBoxWithBitmap (HDC_SCREEN, x, y + offy,
                        bmp->bmWidth, psubmi->h - (offy<<1), bmp);
                bmp_w = MAX (bmp->bmWidth, mioffx);
            }

            // draw text.
            SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
            SetBkColor (HDC_SCREEN, GetWindowElementColor(BKC_MENUITEM_NORMAL));
            old_mode = SetBkMode (HDC_SCREEN, BM_TRANSPARENT);
            if (psubmi->mnustate & MFS_DISABLED)
                SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_DISABLED));
            else
                SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_NORMAL));
            TextOut (HDC_SCREEN, x + bmp_w, y + inter, 
                (char*)psubmi->typedata);
            SetBkMode (HDC_SCREEN, old_mode);

            if (psubmi->submenu) {
                bmp = mnuGetSubMenuBitmap (psubmi);
                offy = (psubmi->h - bmp->bmHeight)>>1;
                FillBoxWithBitmap (HDC_SCREEN, 
                    ptmi->rc.right - mioffx - marginrx,
                    y + offy,
                    bmp->bmWidth, psubmi->h - (offy<<1), bmp);
            }
        }

        y += psubmi->h;

        psubmi = psubmi->next;
    }

    return 0;
}

static void mnuUnhiliteMenu (PMENUITEM pmi)
{
    if (pmi->type == TYPE_PPPMENU)
        pmi = pmi->submenu;
    else {
        if (pmi->mnustate & MFS_HILITE)
            pmi->mnustate &= ~MFS_HILITE;

        pmi = pmi->next;
    }

    while (pmi) {
        if (pmi->mnustate & MFS_HILITE)
            pmi->mnustate &= ~MFS_HILITE;

        pmi = pmi->next;
    }
}

static void mnuCloseMenu (PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO phead, plast, ptemp;
    HMENU hmnu;
    UINT flags;

    // menu bar info
    HWND hwnd;
    PMENUBAR pmb;
    int barPos;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;

    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
    pmb = phead->pmb;
    hwnd = phead->hwnd;
    barPos = phead->barPos;
    
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    while (plast) {
        ptemp = plast->prev;

        mnuUnhiliteMenu (plast->pmi);
        SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)plast);

        plast = ptemp;
    }

    if (pmb && barPos >= 0)
        HiliteMenuBarItem (hwnd, barPos, HMF_DEFAULT);
    
    if (flags & TPM_DESTROY)
        DestroyMenu (hmnu);
}

static PTRACKMENUINFO mnuHitTestMenu (PTRACKMENUINFO plast, int x, int y)
{
    PTRACKMENUINFO ptemp;
    
    // which popup menu mouse in.
    ptemp = plast;
    while (ptemp) {

        if ( PtInRect(&ptemp->rc, x, y))
            return ptemp;

        ptemp = ptemp->prev;
    }

    return NULL;
}

static PMENUITEM mnuHitTestMenuItem (PTRACKMENUINFO ptmi, int x, int y)
{
    PMENUITEM pmi = ptmi->pmi;
    PMENUITEM psubmi;
    RECT rc;
    
    rc.left = ptmi->rc.left;
    rc.right = ptmi->rc.right;
    rc.top = ptmi->rc.top + GetMainWinMetrics (MWM_MENUTOPMARGIN);
    rc.bottom = rc.top + pmi->h;

    if (PtInRect (&rc, x, y))
        return pmi;

    rc.top += pmi->h;
    if (pmi->type == TYPE_PPPMENU)
        psubmi = pmi->submenu;
    else
        psubmi = pmi->next;
   
    while (psubmi) {
        
        rc.bottom = rc.top + psubmi->h;

        if (PtInRect (&rc, x, y))
            return psubmi;

        rc.top = rc.bottom;
        psubmi = psubmi->next;
    }

    return NULL;
}

static void mnuHiliteMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi, BOOL bHilite)
{
    RECT rc;
    PMENUITEM ptemp = ptmi->pmi;
    int inter;
    int mioffx;
    int marginlx, marginrx;
    int offy;

    PBITMAP bmp;
    
    if (pmi->type == TYPE_PPPMENU) return;

    inter = GetMainWinMetrics (MWM_INTERMENUITEMY);
    mioffx = GetMainWinMetrics (MWM_MENUITEMOFFX);
    marginlx = GetMainWinMetrics (MWM_MENULEFTMARGIN);
    marginrx = GetMainWinMetrics (MWM_MENURIGHTMARGIN);

    rc.left = ptmi->rc.left;
    rc.right = ptmi->rc.right;
    rc.top = ptmi->rc.top + GetMainWinMetrics (MWM_MENUTOPMARGIN);

    if (ptemp == pmi)
        ptemp = pmi;
    else {
        rc.top += ptemp->h;
        if (ptemp->type == TYPE_PPPMENU)
            ptemp = ptemp->submenu;
        else
            ptemp = ptemp->next;

        while (ptemp != pmi) {
            rc.top += ptemp->h;
            ptemp = ptemp->next;
        }
    }

    rc.bottom = rc.top + ptemp->h;

    if (ptemp->mnutype & MFT_SEPARATOR) return;

    if (bHilite)
        ptemp->mnustate |= MFS_HILITE;
    else
        ptemp->mnustate &= ~MFS_HILITE;

    SetBrushColor (HDC_SCREEN, 
        bHilite ? GetWindowElementColor(BKC_MENUITEM_HILITE)
                : GetWindowElementColor(BKC_MENUITEM_NORMAL));

    FillBox (HDC_SCREEN, rc.left + marginlx, rc.top, 
        RECTW (rc) - marginlx - marginrx, RECTH (rc)); 
#if defined(_FLAT_WINDOW_STYLE) && !defined(_GRAY_SCREEN)
    if (bHilite) {
	    SetPenColor(HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_FRAME));
	    Rectangle (HDC_SCREEN, rc.left + marginlx, rc.top, 
		rc.left + RECTW (rc) - marginrx, rc.top + RECTH (rc)); 
    }
    else {
	    SetPenColor(HDC_SCREEN, GetWindowElementColor(BKC_MENUITEM_NORMAL));
	    Rectangle (HDC_SCREEN, rc.left + marginlx, rc.top, 
		rc.left + RECTW (rc) - marginrx, rc.top + RECTH (rc)); 
    }
#endif
        
    if (ptemp->mnutype & MFT_BITMAP) {
    
        if (ptemp->mnustate & MFS_CHECKED)
            bmp = ptemp->hbmpChecked;
        else
            bmp = ptemp->hbmpUnchecked;
            
        FillBoxWithBitmap (HDC_SCREEN, rc.left + marginlx, rc.top,
                        bmp->bmWidth, 
                        bmp->bmHeight, 
                        bmp);
    }
    else if (ptemp->mnutype & MFT_OWNERDRAW) {
    }
    else {
        int old_mode, bmp_w = mioffx;

        // draw check bitmap
        bmp = mnuGetCheckBitmap (ptemp);
        if (bmp) {
            offy = (ptemp->h - bmp->bmHeight)>>1;
            FillBoxWithBitmap (HDC_SCREEN, rc.left + marginlx, rc.top + offy,
                    bmp->bmWidth, ptemp->h - (offy<<1), bmp);
            bmp_w = MAX (bmp->bmWidth, mioffx);
        }

        SelectFont (HDC_SCREEN, GetSystemFont (SYSLOGFONT_MENU));
        SetBkColor (HDC_SCREEN, 
                bHilite ? GetWindowElementColor(BKC_MENUITEM_HILITE)
                        : GetWindowElementColor(BKC_MENUITEM_NORMAL));
        old_mode = SetBkMode (HDC_SCREEN, BM_TRANSPARENT);
        if (ptemp->mnustate & MFS_DISABLED) {
            SetTextColor (HDC_SCREEN, GetWindowElementColor(FGC_MENUITEM_DISABLED));
        }
        else {
            SetTextColor (HDC_SCREEN, 
                bHilite ? GetWindowElementColor(FGC_MENUITEM_HILITE)
                        : GetWindowElementColor(FGC_MENUITEM_NORMAL));
        }
        TextOut (HDC_SCREEN, rc.left + bmp_w + marginlx, rc.top + inter,
            (char*)ptemp->typedata);
        SetBkMode (HDC_SCREEN, old_mode);
        
        if (ptemp->submenu) {
            bmp = mnuGetSubMenuBitmap (ptemp);
            offy = (ptemp->h - bmp->bmHeight)>>1;
            FillBoxWithBitmap (HDC_SCREEN, 
                rc.right - mioffx - marginrx,
                rc.top + offy,
                bmp->bmWidth, ptemp->h - (offy<<1), bmp);
        }
    }
}

static int mnuOpenNewSubMenu (PTRACKMENUINFO ptmi, PMENUITEM pmi, 
                int x, int y)
{
    PTRACKMENUINFO pnewtmi;

    pnewtmi = TrackMenuInfoAlloc ();
    
    pnewtmi->rc.left = x;
    pnewtmi->rc.top  = y;
    pnewtmi->pmi = pmi;
    pnewtmi->pmb = ptmi->pmb;
    pnewtmi->philite = NULL;
    pnewtmi->hwnd = ptmi->hwnd;
    pnewtmi->flags = ptmi->flags;
    pnewtmi->next = NULL;
    pnewtmi->prev = NULL;
   
    return SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(pnewtmi));
}

static PMENUITEM mnuGetNextMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pmi;
    PMENUITEM next;

    if (head->type == TYPE_PPPMENU)
        head = head->submenu;

    if (pmi == NULL)
        next = head;
    else if (pmi->next)
        next = pmi->next;
    else
        next = head;

    while (next && next->mnutype & MFT_SEPARATOR)
        next = next->next;

    if (next == NULL)
        next = head;

    while (next && next->mnutype & MFT_SEPARATOR)
        next = next->next;

    return next;
}

static PMENUITEM mnuGetPrevItem (PMENUITEM head, PMENUITEM pmi)
{
    if (head == pmi)
        return NULL;

    while (head) {
        if (head->next == pmi)
            return head;

        head = head->next;
    }

    return NULL;
}

static PMENUITEM mnuGetPrevMenuItem (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pmi, tail;
    PMENUITEM prev;

    if (head->type == TYPE_PPPMENU)
        head = head->submenu;

    prev = pmi;

    do {
        prev = mnuGetPrevItem (head, prev);

        if (prev) {
            if (prev->mnutype & MFT_SEPARATOR)
                continue;
            else
                return prev;
        }
        else
            break;
    }while (TRUE);

    tail = head;
    while (tail && tail->next)
        tail = tail->next;

    prev = tail;
    do {
        if (prev) {
            if (!(prev->mnutype & MFT_SEPARATOR))
                return prev;
        }
        else
            break;
            
        prev = mnuGetPrevItem (head, prev);
    }while (TRUE);

    return NULL;
}

static int mnuGetMenuItemY (PTRACKMENUINFO ptmi, PMENUITEM pmi)
{
    PMENUITEM head = ptmi->pmi;
    int y = ptmi->rc.top + GetMainWinMetrics (MWM_MENUTOPMARGIN);

    if (pmi == head) return y;
    
    y += head->h;
    
    if (head->type == TYPE_PPPMENU)
        head = head->submenu;
    else
        head = head->next;

    while (head) {
        if (head == pmi)
            return y;

        y += head->h;

        head = head->next;
    }

    return 0;
}

static void mnuTrackMenuWithKey (PTRACKMENUINFO ptmi, 
                                int message, int scan, DWORD status)
{
    PTRACKMENUINFO phead, pcurtmi, pprevtmi;
    PMENUITEM pcurmi, pnewmi;
    HWND hwnd;
    HMENU hmnu;
    UINT flags;
    int barItem;
    int id;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;
    
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
    
    // get last tracking menu, the last menu is the current menu.
    pcurtmi = ptmi;
    while (pcurtmi->next) {
        pcurtmi = pcurtmi->next;
    }

    pprevtmi = pcurtmi->prev;

    pcurmi = pcurtmi->philite;

    if (message == MSG_KEYDOWN) {
        switch (scan) {
            case SCANCODE_CURSORBLOCKDOWN:
            case SCANCODE_CURSORBLOCKUP:
                if (scan == SCANCODE_CURSORBLOCKDOWN)
                    pnewmi = mnuGetNextMenuItem (pcurtmi, pcurmi);
                else
                    pnewmi = mnuGetPrevMenuItem (pcurtmi, pcurmi);

                if (pnewmi != pcurmi) {
                    if (pcurmi) 
                        mnuHiliteMenuItem (pcurtmi, pcurmi, FALSE);
                    if (pnewmi) {
                        mnuHiliteMenuItem (pcurtmi, pnewmi, TRUE);
                        pcurtmi->philite = pnewmi;
                    }
                }
                break;
                
                break;
                
            case SCANCODE_CURSORBLOCKRIGHT:
                if (pcurmi && pcurmi->submenu)
                    mnuOpenNewSubMenu (pcurtmi, 
                                pcurmi->submenu, 
                                pcurtmi->rc.right,
                                mnuGetMenuItemY (pcurtmi, pcurmi));
                else {
                    if (phead->pmb) {
                        barItem  = mnuGetNextMenuBarItem (phead->pmb, 
                                                          phead->barPos);
                        if (barItem != phead->barPos) {
                            hwnd = phead->hwnd;

                            // close current popup menu.
                            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                            // open new popup menu.
                            TrackMenuBar (hwnd, barItem);
                        }
                    }
                }
                break;
                
            case SCANCODE_ESCAPE:
                if (pprevtmi)
                    SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)pcurtmi);
                break;

            case SCANCODE_CURSORBLOCKLEFT:
                if (pprevtmi)
                    SendMessage (HWND_DESKTOP, 
                        MSG_ENDTRACKMENU, 0, (LPARAM)pcurtmi);
                else {
                    if (phead->pmb) {
                        barItem  = mnuGetPrevMenuBarItem (phead->pmb, 
                                                          phead->barPos);
                        if (barItem != phead->barPos) {
                            hwnd = phead->hwnd;

                            // close current popup menu.
                            SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                            // open new popup menu.
                            TrackMenuBar (hwnd, barItem);
                        }
                    }
                }
                break;
                
            case SCANCODE_ENTER:
                if (pcurmi == NULL) return;
                if (pcurmi->mnutype & MFT_SEPARATOR) return;
                if (pcurmi->mnustate & MFS_DISABLED) return;
                
                if (pcurmi->submenu) {
                    mnuOpenNewSubMenu (pcurtmi, 
                                pcurmi->submenu, 
                                pcurtmi->rc.right,
                                mnuGetMenuItemY (pcurtmi, pcurmi));
                    return;
                }

                if (pcurmi && pcurmi->type != TYPE_PPPMENU &&
                    pcurmi->submenu == NULL) {

                    hwnd = ptmi->hwnd;
                    id = pcurmi->id;
        
                    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);
       
                    if (flags & TPM_SYSCMD)
                        SendNotifyMessage (hwnd, MSG_SYSCOMMAND, id, 0);
                    else
                        SendNotifyMessage (hwnd, MSG_COMMAND, id, 0);

                    if (flags & TPM_DESTROY)
                        DestroyMenu (hmnu);
                }
                break;
        }
    }
    else if (message == MSG_KEYUP) {
        switch (scan) {
            case SCANCODE_LEFTALT:
            case SCANCODE_RIGHTALT:
                SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

                if (flags & TPM_DESTROY)
                    DestroyMenu (hmnu);
                break;
        }
    }
}

static void mnuTrackMenu (PTRACKMENUINFO ptmi, int x, int y)
{
    PTRACKMENUINFO phead, plast;
    PTRACKMENUINFO pcurtmi, pclose, ptemp;
    PMENUITEM pcurmi;
    PMENUITEM philite;
    int barItem;
    HWND hwnd;
    
    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }
        
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL) {
        if (plast->philite)
            mnuHiliteMenuItem (plast, plast->philite, FALSE);

        // check to see if move to the other sub menu of menu bar.
        if (phead->pmb) {
            barItem = MenuBarHitTest (phead->hwnd, x, y);
            if (barItem >= 0 && barItem != phead->barPos) {
                    
                hwnd = phead->hwnd;

                // close current popup menu.
                SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 0);

                // open new popup menu.
                TrackMenuBar (hwnd, barItem);
            }
        }
            
        return;
    }

    pcurmi = mnuHitTestMenuItem (pcurtmi, x, y);

    if (pcurtmi != plast) {
        pclose = pcurtmi->next->next;
       
        if (pclose != NULL) {
            while (pclose != plast) {
                ptemp = plast->prev;
                SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
                plast = ptemp;
            }
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
            plast = pcurtmi->next;
        }
            
        if (plast->philite) {
            mnuHiliteMenuItem (plast, plast->philite, FALSE);
            plast->philite = NULL;
        }
    }

    philite = pcurtmi->philite;
    if (pcurmi == philite) return;

    // current hilite menu item is a bottom most item.
    if (philite) {
        if (pcurtmi != plast)
            SendMessage (HWND_DESKTOP, MSG_ENDTRACKMENU, 0, (LPARAM)plast);
        mnuHiliteMenuItem (pcurtmi, philite, FALSE);
    }

    pcurtmi->philite = pcurmi;
    if (pcurmi == NULL) return;
    if (pcurmi->type == TYPE_PPPMENU) return;

    mnuHiliteMenuItem (pcurtmi, pcurmi, TRUE);

    if (pcurmi->submenu)
        mnuOpenNewSubMenu (pcurtmi, pcurmi->submenu, 
            pcurtmi->rc.right, mnuGetMenuItemY (pcurtmi, pcurmi));
}

static int mnuTrackMenuOnButtonDown (PTRACKMENUINFO ptmi, 
        int message, int x, int y)
{
    PTRACKMENUINFO plast, phead;
    PTRACKMENUINFO pcurtmi;
    HMENU hmnu;
    UINT flags;
    int barItem;
    
    flags = ptmi->flags;
    hmnu = (HMENU)ptmi->pmi;
    
    // get first tracking menu.
    phead = ptmi;
    while (phead->prev) {
        phead = phead->prev;
    }

    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL) {      // close menu.
    
        if (phead->pmb) {
            barItem = MenuBarHitTest (phead->hwnd, x, y);
            if (barItem == phead->barPos)
                return 0;
        }

        SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

        if (flags & TPM_DESTROY)
            DestroyMenu (hmnu);

        return -1;
    }


    return 0;
}

static int mnuTrackMenuOnButtonUp (PTRACKMENUINFO ptmi, 
        int message, int x, int y)
{
    PTRACKMENUINFO plast;
    PTRACKMENUINFO pcurtmi;
    PMENUITEM pcurmi;
    HWND hwnd;
    UINT flags;
    int  id;
    HMENU hmnu;
    
    // get last tracking menu.
    plast = ptmi;
    while (plast->next) {
        plast = plast->next;
    }

    pcurtmi = mnuHitTestMenu (plast, x, y);

    if (pcurtmi == NULL || pcurtmi->philite == NULL) return 0;

    pcurmi = mnuHitTestMenuItem (pcurtmi, x, y);

    if (pcurmi == NULL) return 0;
    if (pcurmi->mnutype & MFT_SEPARATOR) return 0;
    if (pcurmi->mnustate & MFS_DISABLED) return 0;
        
    if (pcurmi->type != TYPE_PPPMENU &&
        pcurmi->submenu == NULL)
    {
        hwnd = ptmi->hwnd;
        flags = ptmi->flags;
        id = pcurmi->id;
        hmnu = (HMENU)ptmi->pmi;
        
        SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);
       
        if (flags & TPM_SYSCMD)
            SendNotifyMessage (hwnd, MSG_SYSCOMMAND, id, 0);
        else
            SendNotifyMessage (hwnd, MSG_COMMAND, id, 0);

        if (flags & TPM_DESTROY)
            DestroyMenu (hmnu);
            
        return id;
    }

    return 0;
}

int PopupMenuTrackProc (PTRACKMENUINFO ptmi, 
    int message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_INITMENU:
            mnuGetPopupMenuExtent (ptmi);
        break;

        case MSG_SHOWMENU:
            if (GetCurrentCursor() != GetSystemCursor(IDC_ARROW))
                SetCursor(GetSystemCursor(IDC_ARROW));
            return mnuShowPopupMenu (ptmi); 
        break;

        case MSG_HIDEMENU:
            mnuUnhiliteMenu (ptmi->pmi);
#ifdef _USE_NEWGAL
            FillBoxWithBitmap (HDC_SCREEN, ptmi->rc.left, ptmi->rc.top,
                        RECTW (ptmi->rc), RECTH (ptmi->rc), &ptmi->savedbox);
#else
            PutSavedBoxOnScreen (ptmi->rc.left, ptmi->rc.top,
                        RECTW (ptmi->rc), RECTH (ptmi->rc), ptmi->savedbox);
#endif
        break;

        case MSG_ENDTRACKMENU:
#ifdef _USE_NEWGAL
            free (ptmi->savedbox.bmBits);
#else
            free (ptmi->savedbox);
#endif
            if (ptmi->hwnd != HWND_DESKTOP)
                SendNotifyMessage (ptmi->hwnd, MSG_ENDTRACKMENU, 
                            (WPARAM)ptmi->pmi, (LPARAM)ptmi->pmb);
            FreeTrackMenuInfo (ptmi);
        break;

        case MSG_CLOSEMENU:
            mnuCloseMenu (ptmi);
        break;

        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (mnuTrackMenuOnButtonDown (ptmi, 
                    message, (int)wParam, (int)lParam) < 0 )
                return 1;
        break;

        case MSG_LBUTTONUP:
        case MSG_RBUTTONUP:
            mnuTrackMenuOnButtonUp (ptmi, message, (int)wParam, (int)lParam);
            return 1;

        case MSG_MOUSEMOVE:
            mnuTrackMenu (ptmi, (int)wParam, (int)lParam);
        break;

        case MSG_KEYDOWN:
        case MSG_KEYUP:
            mnuTrackMenuWithKey (ptmi, message, (int)wParam, (DWORD)lParam);
        break;

        default:
        return 1;
    }

    return 0;
}
    
int GUIAPI TrackPopupMenu (HMENU hmnu, UINT uFlags, int x, int y, HWND hwnd)
{
    PTRACKMENUINFO ptmi;

    // close current menu firstly.
    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

    ptmi = TrackMenuInfoAlloc ();
    
    ptmi->rc.left = x;
    ptmi->rc.top  = y;
    ptmi->pmi = (PMENUITEM)hmnu;
    ptmi->pmb = NULL;
    ptmi->barPos = -1;
    ptmi->philite = NULL;
    ptmi->hwnd = hwnd;
    ptmi->flags = uFlags;
    ptmi->next = NULL;
    ptmi->prev = NULL;
   
    if (ptmi->pmi->category != TYPE_HMENU) return ERR_INVALID_HANDLE;
    
    SendMessage (hwnd, MSG_ACTIVEMENU, 0, (LPARAM)hmnu);

    return SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(ptmi));
}

int GUIAPI TrackMenuBar (HWND hwnd, int pos)
{
    PMENUITEM pppMenu;
    PMENUBAR  menuBar;
    RECT rcMenuItem;
    PTRACKMENUINFO ptmi;

    menuBar = (PMENUBAR)GetMenu (hwnd);
    if (!menuBar) return ERR_INVALID_HANDLE;
    
    pppMenu = (PMENUITEM)GetSubMenu ((HMENU)menuBar, pos);
    if (!pppMenu) return ERR_INVALID_HANDLE;
    
    // close current menu firstly.
    SendMessage (HWND_DESKTOP, MSG_CLOSEMENU, 0, 1);

    ptmi = TrackMenuInfoAlloc ();
    
    GetMenuBarItemRect (hwnd, pos, &rcMenuItem);
    WindowToScreen (hwnd, &rcMenuItem.left, &rcMenuItem.bottom);
    
    ptmi->rc.left = rcMenuItem.left - (GetMainWinMetrics (MWM_MENUITEMOFFX)>>1);
    ptmi->rc.top  = rcMenuItem.bottom;
    ptmi->pmi = pppMenu;
    ptmi->pmb = menuBar;
    ptmi->barPos = pos;
    ptmi->philite = NULL;
    ptmi->hwnd = hwnd;
    ptmi->flags = TPM_DEFAULT;
    ptmi->next = NULL;
    ptmi->prev = NULL;
   
    HiliteMenuBarItem (hwnd, pos, HMF_DEFAULT);
    HiliteMenuBarItem (hwnd, pos, HMF_DOWNITEM);

    SendMessage (hwnd, MSG_ACTIVEMENU, pos, (LPARAM)pppMenu);

    return SendMessage (HWND_DESKTOP, MSG_TRACKPOPUPMENU,
                    0, (LPARAM)(ptmi));
}


