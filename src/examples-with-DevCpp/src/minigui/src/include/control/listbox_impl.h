/*
** $Id: listbox_impl.h,v 1.1 2005/01/31 09:12:48 clear Exp $
**
** listbox.h: the head file of ListBox control.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002, Wei Yongming.
**
** Create date: 1999/8/31
*/

#ifndef __LISTBOX_IMPL_H_
#define __LISTBOX_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

#define LBIF_NORMAL         0x0000L
#define LBIF_SELECTED       0x0001L
#define LBIF_USEBITMAP      0x0002L

#define LBIF_BLANK          0x0000L
#define LBIF_CHECKED        0x0010L
#define LBIF_PARTCHECKED    0x0020L
#define LBIF_CHECKMARKMASK  0x00F0L

typedef struct _LISTBOXITEM 
{
    char*   key;                // item sort key
    DWORD   dwFlags;            // item flags
    DWORD   dwImage;            // item image
    DWORD   dwAddData;          // item additional data
    struct  _LISTBOXITEM* next; // next item
} LISTBOXITEM;
typedef LISTBOXITEM* PLISTBOXITEM;

#ifdef _LITE_VERSION
    #define DEF_LB_BUFFER_LEN       8
#else
    #define DEF_LB_BUFFER_LEN       16
#endif

#define LBF_FOCUS               0x0001

typedef struct tagLISTBOXDATA
{
    DWORD dwFlags;          // listbox flags

    STRCMP str_cmp;         // function to compare two string

    int itemCount;          // items count
    int itemTop;            // start display item
    int itemVisibles;       // number of visible items

    int itemHilighted;      // current hilighted item
    int itemHeight;         // item height

    LISTBOXITEM* head;      // items linked list head

    int buffLen;            // buffer length
    LISTBOXITEM* buffStart; // buffer start
    LISTBOXITEM* buffEnd;   // buffer end
    LISTBOXITEM* freeList;  // free list in buffer 
    SBPolicyType sbPolicy;  // scrollbar policy type
} LISTBOXDATA;
typedef LISTBOXDATA* PLISTBOXDATA;   

BOOL RegisterListboxControl (void);
void ListboxControlCleanup (void);

#ifdef  __cplusplus
}
#endif

#endif  // __LISTBOX_IMPL_H__

