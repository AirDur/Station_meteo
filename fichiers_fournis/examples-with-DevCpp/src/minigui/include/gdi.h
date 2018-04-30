/**
 * \file gdi.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 * This file includes graphics device interfaces of MiniGUI.
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
 * $Id: gdi.h,v 1.205 2005/02/15 05:00:07 weiym Exp $
 *
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_GDI_H
    #define _MGUI_GDI_H

/* include necessary headers */
#ifndef _LITE_VERSION
#include <pthread.h>
#endif /* !LITE_VERSION */
 
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup global_vars Global variables
     * @{
     */

    /**
     * \defgroup color_vars System colors and pixel values.
     * @{
     */

/**
 * \var gal_pixel SysPixelIndex []
 * \brief The pre-defined system pixel values.
 *
 * MiniGUI defines some system pixel values when initializing 
 * graphics sub-system. You can access the arrary to get the
 * system pixel values, or just use the following macros:
 *
 *  - PIXEL_black\n
 *    black
 *  - PIXEL_darkred\n
 *    dark red
 *  - PIXEL_darkgreen\n
 *    dark green
 *  - PIXEL_darkyellow\n
 *    dark yellow
 *  - PIXEL_darkblue\n
 *    dark blue
 *  - PIXEL_darkmagenta\n
 *    dark magenta
 *  - PIXEL_darkcyan\n
 *    dark cyan
 *  - PIXEL_lightgray\n
 *    light gray
 *  - PIXEL_darkgray\n
 *    dark gray
 *  - PIXEL_red\n
 *    red
 *  - PIXEL_green\n
 *    green
 *  - PIXEL_yellow\n
 *    yellow
 *  - PIXEL_blue\n
 *    blue
 *  - PIXEL_magenta\n
 *    magenta
 *  - PIXEL_cyan\n
 *    cyan
 *  - PIXEL_lightwhite\n
 *    light white
 */
#ifdef WIN32
#ifdef __MINIGUI_LIB__
MG_EXPORT  gal_pixel SysPixelIndex [];
#else
MG_IMPORT  gal_pixel SysPixelIndex [];
#endif
#else
extern gal_pixel SysPixelIndex [];
#endif

/**
 * \var RGB SysPixelColor []
 * \brief The pre-defined system RGB colors.
 *
 * The elements in this array are system colors in RGB triples.
 */
extern const RGB SysPixelColor [];

#define PIXEL_invalid       0
#define PIXEL_transparent   SysPixelIndex[0]
#define PIXEL_darkblue      SysPixelIndex[1]
#define PIXEL_darkgreen     SysPixelIndex[2]
#define PIXEL_darkcyan      SysPixelIndex[3]
#define PIXEL_darkred       SysPixelIndex[4] 
#define PIXEL_darkmagenta   SysPixelIndex[5]
#define PIXEL_darkyellow    SysPixelIndex[6] 
#define PIXEL_darkgray      SysPixelIndex[7] 
#define PIXEL_lightgray     SysPixelIndex[8]
#define PIXEL_blue          SysPixelIndex[9]
#define PIXEL_green         SysPixelIndex[10]
#define PIXEL_cyan          SysPixelIndex[11]
#define PIXEL_red           SysPixelIndex[12]
#define PIXEL_magenta       SysPixelIndex[13]
#define PIXEL_yellow        SysPixelIndex[14]
#define PIXEL_lightwhite    SysPixelIndex[15]
#define PIXEL_black         SysPixelIndex[16]

/* Compatiblity definitions */
#define COLOR_invalid       PIXEL_invalid
#define COLOR_transparent   PIXEL_transparent
#define COLOR_darkred       PIXEL_darkred
#define COLOR_darkgreen     PIXEL_darkgreen
#define COLOR_darkyellow    PIXEL_darkyellow
#define COLOR_darkblue      PIXEL_darkblue
#define COLOR_darkmagenta   PIXEL_darkmagenta
#define COLOR_darkcyan      PIXEL_darkcyan
#define COLOR_lightgray     PIXEL_lightgray
#define COLOR_darkgray      PIXEL_darkgray
#define COLOR_red           PIXEL_red
#define COLOR_green         PIXEL_green
#define COLOR_yellow        PIXEL_yellow
#define COLOR_blue          PIXEL_blue
#define COLOR_magenta       PIXEL_magenta
#define COLOR_cyan          PIXEL_cyan
#define COLOR_lightwhite    PIXEL_lightwhite
#define COLOR_black         PIXEL_black

#define SysColorIndex       SysPixelIndex

    /** @} end of color_vars */

    /** @} end of global_vars */

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup block_heap_fns Block data heap operations
     *
     * MiniGUI maintains some private block data heaps to allocate
     * data blocks which have fixed size, such as the clipping rectangles
     * in a region. By using the private heap, we can avoiding calling
     * \a malloc and \a free function frequently, so will have a slight 
     * performance enhancement.
     *
     * @{
     */

/**
 * MiniGUI's private block data heap.
 *
 * \sa InitBlockDataHeap, DestroyBlockDataHeap
 */
typedef struct _BLOCKHEAP
{
#ifndef _LITE_VERSION
    pthread_mutex_t lock;
#endif
    /**
     * size of one block element.
     */
    size_t          bd_size;
    /**
     * size of the heap in blocks.
     */
    size_t          heap_size;
    /**
     * the first free element in the heap.
     */
    int             free;
    /**
     * pointer to the pre-allocated heap.
     */
    void*           heap;
} BLOCKHEAP;

/**
 * \var typedef BLOCKHEAP* PBLOCKHEAP
 * \brief Data type of the pointer to a BLOCKHEAP.
 *
 * \sa BLOCKHEAP
 */
typedef BLOCKHEAP* PBLOCKHEAP;

/**
 * \fn void InitBlockDataHeap (PBLOCKHEAP heap, size_t bd_size, size_t heap_size)
 * \brief Initializes a private block data heap.
 *
 * This function initializes a block data heap pointed to by \a heap.
 * It will allocate the buffer used by the heap from system heap by using \a malloc(3).
 * Initially, the heap has \a heap_size blocks free, and each is \a bd_size bytes long.
 *
 * \param heap The pointer to the heap structure.
 * \param bd_size The size of one block in bytes.
 * \param heap_size The size of the heap in blocks.
 * \return \a heap->heap will contains a valid pointer on success, NULL on error.
 *
 * \note This function does not return anything. You should check the \a heap
 * field of the \a heap structure.
 *
 * \sa BLOCKHEAP
 */
MG_EXPORT   extern void  InitBlockDataHeap (PBLOCKHEAP heap, size_t bd_size, size_t heap_size);

/**
 * \fn void* BlockDataAlloc (PBLOCKHEAP heap)
 * \brief Allocates a data block from private heap.
 *
 * This function allocates a data block from an initialized 
 * block data heap. The allocated block will have the size of \a heap->bd_size.
 * If there is no free block in the heap, this function will try to allocate
 * the block from the system heap by using \a malloc(3) function.
 *
 * \param heap The pointer to the initialized heap.
 * \return Pointer to the allocated data block, NULL on error.
 *
 * \sa InitBlockDataHeap, BlockDataFree
 */

MG_EXPORT  extern  void*   BlockDataAlloc (PBLOCKHEAP heap);

/**
 * \fn void BlockDataFree (PBLOCKHEAP heap, void* data)
 * \brief Frees an allocated data block.
 *
 * This function frees the specified data block pointed to by \a data to the heap \a heap.
 * If the block was allocated by using \a malloc function, this function will
 * free the element by using \a free(3) function. 
 *
 * \param heap The pointer to the heap.
 * \param data The pointer to the element to be freed.
 *
 * \sa InitBlockDataHeap, BlockDataAlloc
 */
MG_EXPORT extern  void BlockDataFree (PBLOCKHEAP heap, void* data);

/**
 * \fn void DestroyBlockDataHeap (PBLOCKHEAP heap)
 * \brief Destroies a private block data heap.
 *
 * \param heap The pointer to the heap to be destroied.
 *
 * \sa InitBlockDataHeap, BLOCKHEAP
 */
MG_EXPORT  extern void DestroyBlockDataHeap (PBLOCKHEAP heap);

    /** @} end of block_heap_fns */

    /** @} end of global_fns */

    /**
     * \defgroup gdi_fns GDI functions
     * @{
     */

struct _BITMAP;
typedef struct _BITMAP BITMAP;
typedef BITMAP* PBITMAP;

struct _MYBITMAP;
typedef struct _MYBITMAP MYBITMAP;
typedef struct _MYBITMAP* PMYBITMAP;

   /**
    * \defgroup region_fns Region operations
    *
    * A Region is simply an area, as the name implies, and is implemented as 
    * a "y-x-banded" array of rectangles. To explain: Each Region is made up 
    * of a certain number of rectangles sorted by y coordinate first, 
    * and then by x coordinate.
    *
    * Furthermore, the rectangles are banded such that every rectangle with a
    * given upper-left y coordinate (y1) will have the same lower-right y
    * coordinate (y2) and vice versa. If a rectangle has scanlines in a band, it
    * will span the entire vertical distance of the band. This means that some
    * areas that could be merged into a taller rectangle will be represented as
    * several shorter rectangles to account for shorter rectangles to its left
    * or right but within its "vertical scope".
    *
    * An added constraint on the rectangles is that they must cover as much
    * horizontal area as possible. E.g. no two rectangles in a band are allowed
    * to touch.
    *
    * Whenever possible, bands will be merged together to cover a greater vertical
    * distance (and thus reduce the number of rectangles). Two bands can be merged
    * only if the bottom of one touches the top of the other and they have
    * rectangles in the same places (of the same width, of course). This maintains
    * the y-x-banding that's so nice to have...
    *
    * Example:
    *
    * \include region.c
    *
    * @{
    */

/**
 * Clipping rectangle structure.
 */
typedef struct _CLIPRECT
{
    /**
     * the clipping rectangle itself.
     */
    RECT rc;
    /**
     * the next clipping rectangle.
     */
    struct _CLIPRECT* next;
#ifdef _USE_NEWGAL
    /**
     * the previous clipping rectangle.
     * \note only defined for _USE_NEWGAL.
     */
    struct _CLIPRECT* prev;
#endif
} CLIPRECT;
typedef CLIPRECT* PCLIPRECT;

/* Clipping Region */
#ifdef _USE_NEWGAL
#define NULLREGION      0x00
#define SIMPLEREGION    0x01
#define COMPLEXREGION   0x02
#endif

/**
 * Clipping region structure, alos used for general regions.
 */
typedef struct _CLIPRGN
{
#ifdef _USE_NEWGAL
   /**
    * type of the region, can be one of the following:
    *   - NULLREGION\n
    *     a null region.
    *   - SIMPLEREGION\n
    *     a simple region.
    *   - COMPLEXREGION
    *     a complex region.
    *
    * \note only defined for _USE_NEWGAL.
    */
    BYTE            type;        /* type of region */
   /**
    * reserved for alignment.
    *
    * \note only defined for _USE_NEWGAL.
    */
    BYTE            reserved[3];
#endif
   /**
    * The bounding rect of the region.
    */
    RECT            rcBound;
   /**
    * head of the clipping rectangle list.
    */
    PCLIPRECT       head;
   /**
    * tail of the clipping rectangle list.
    */
    PCLIPRECT       tail;
   /**
    * The private block data heap used to allocate clipping rectangles.
    * \sa BLOCKHEAP
    */
    PBLOCKHEAP      heap;
} CLIPRGN;

/**
 * \var typedef CLIPRGN* PCLIPRGN
 * \brief Data type of the pointer to a CLIPRGN.
 *
 * \sa CLIPRGN
 */
typedef CLIPRGN* PCLIPRGN;

/**
 * \def InitFreeClipRectList(heap, size)
 * \brief Initializes the private block data heap used to allocate clipping rectangles.
 * \param heap The pointer to a BLOCKHEAP structure.
 * \param size The size of the heap.
 * 
 * \note This macro is defined to call \a InitBlockDataHeap function with \a bd_size set to 
 * \a sizeof(CLIPRECT).
 *
 * \sa InitBlockDataHeap
 */
#define InitFreeClipRectList(heap, size)    InitBlockDataHeap (heap, sizeof (CLIPRECT), size)

/**
 * \def ClipRectAlloc(heap)
 * \brief Allocates a clipping rectangles from the private block data heap.
 * \param heap The pointer to the initialized BLOCKHEAP structure.
 * 
 * \note This macro is defined to call \a BlockDataAlloc function.
 *
 * \sa BlockDataAlloc
 */
#define ClipRectAlloc(heap)     BlockDataAlloc (heap)

/**
 * \def FreeClipRect(heap, cr)
 * \brief Frees a clipping rectangle which is allocated from the private block data heap.
 * \param heap The pointer to the initialized BLOCKHEAP structure.
 * \param cr The pointer to the clipping rectangle to be freed.
 * 
 * \note This macro is defined to call \a BlockDataFree function.
 *
 * \sa BlockDataFree
 */
#define FreeClipRect(heap, cr)  BlockDataFree (heap, cr);

/**
 * \def DestroyFreeClipRectList(heap)
 * \brief Destroies the private block data heap used to allocate clipping rectangles.
 * \param heap The pointer to the BLOCKHEAP structure.
 * 
 * \note This macro is defined to call \a DestroyBlockDataHeap function.
 *
 * \sa DestroyBlockDataHeap
 */
#define DestroyFreeClipRectList(heap)   DestroyBlockDataHeap (heap);

/**
 * \fn void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP pFreeList)
 * \brief Initializes a clipping region.
 *
 * Before intializing a clipping region, you should initialize a private
 * block data heap first. The region operations, such as \a UnionRegion function,
 * will allocate/free the clipping rectangles from/to the heap.
 * This function will set the \a heap field of \a pRgn to be \a pFreeList,
 * and empty the region.
 *
 * \param pRgn The pointer to the CLIPRGN structure to be initialized.
 * \param pFreeList The pointer to the initialized private block data heap.
 *
 * \sa InitFreeClipRectList, EmptyClipRgn.
 *
 * Example:
 *
 * \include initcliprgn.c
 */

MG_EXPORT void GUIAPI InitClipRgn (PCLIPRGN pRgn, PBLOCKHEAP pFreeList);

/**
 * \fn void GUIAPI EmptyClipRgn (PCLIPRGN pRgn)
 * \brief Empties a clipping region.
 *
 * This function empties a clipping region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 *
 * \sa InitClipRgn
 */
MG_EXPORT void GUIAPI EmptyClipRgn (PCLIPRGN pRgn);

/**
 * \fn BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn)
 * \brief Copies one region to another.
 *
 * This function copies the region pointed to by \a pSrcRgn to the region pointed to by \a pDstRgn. 
 *
 * \param pDstRgn The destination region.
 * \param pSrcRgn The source region.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function will empty the region \a pDstRgn first.
 *
 * \sa EmptyClipRgn, ClipRgnIntersect, UnionRegion, SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI ClipRgnCopy (PCLIPRGN pDstRgn, const CLIPRGN* pSrcRgn);

/**
 * \fn BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn, const CLIPRGN* pRgn1, const CLIPRGN* pRgn2)
 * \brief Intersects two region.
 *
 * This function gets the intersection of two regions pointed to by \a pRgn1 and \a pRgn2 
 * respectively and puts the result to the region pointed to by \a pRstRgn.
 *
 * \param pRstRgn The intersected result region.
 * \param pRgn1 The first region.
 * \param pRgn2 The second region.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note If \a pRgn1 does not intersected with \a pRgn2, the result region will be
 * a emgty region.
 *
 * \sa EmptyClipRgn, ClipRgnCopy, UnionRegion, SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI ClipRgnIntersect (PCLIPRGN pRstRgn,
                       const CLIPRGN* pRgn1, const CLIPRGN* pRgn2);

/**
 * \fn void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect)
 * \brief Gets the bounding rectangle of a region.
 * 
 * This function gets the bounding rect of the region pointed to by \a pRgn, 
 * and returns the rect in the rect pointed to by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the result rect.
 *
 * \sa IsEmptyClipRgn
 */
MG_EXPORT void GUIAPI GetClipRgnBoundRect (PCLIPRGN pRgn, PRECT pRect);

/**
 * \fn BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Sets a region to contain only one rect.
 *
 * This function sets the region \a pRgn to contain only a rect pointed to by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rect.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function will empty the region \a pRgn first.
 *
 * \sa EmptyClipRgn
 */
MG_EXPORT BOOL GUIAPI SetClipRgn (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn)
 * \brief Determines whether a region is an empty region.
 *
 * This function determines whether the region pointed to by \a pRgn is an empty region.
 *
 * \param pRgn The pointer to the region.
 * \return TRUE for empty one, else for not empty region.
 *
 * \sa EmptyClipRgn
 */
MG_EXPORT BOOL GUIAPI IsEmptyClipRgn (const CLIPRGN* pRgn);

/**
 * \fn BOOL GUIAPI AddClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Unions one rectangle to a region.
 *
 * This function unions a rectangle to the region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rectangle.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa IntersectClipRect, SubtractClipRect
 */
MG_EXPORT BOOL GUIAPI AddClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI IntersectClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Intersects a rectangle with a region.
 *
 * This function intersects the region pointed to by \a pRgn with a rect pointed to by \a pRect.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rectangle.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa AddClipRect, SubtractClipRect
 */
MG_EXPORT BOOL GUIAPI IntersectClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI SubtractClipRect (PCLIPRGN pRgn, const RECT* pRect)
 * \brief Subtracts a rectangle from a region.
 *
 * This function subtracts a rect pointed to by \a pRect from the region pointed to by \a pRgn.
 *
 * \param pRgn The pointer to the region.
 * \param pRect The pointer to the rect.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa AddClipRect, IntersectClipRect
 */
MG_EXPORT BOOL GUIAPI SubtractClipRect (PCLIPRGN pRgn, const RECT* pRect);

/**
 * \fn BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y)
 * \brief Determines whether a point is in a region.
 *
 * This function determines whether a point \a (x,y) is in the region pointed to by \a region.
 *
 * \param region The pointer to the region.
 * \param x x,y: The point.
 * \param y x,y: The point.
 * \return TRUE for in the region, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa RectInRegion
 */
MG_EXPORT BOOL GUIAPI PtInRegion (PCLIPRGN region, int x, int y);

/**
 * \fn BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect)
 * \brief Determines whether a rectangle is intersected with a region.
 *
 * This function determines whether the rect \a rect is intersected with the region
 * pointed to by \a region.
 *
 * \param region The pointer to the region.
 * \param rect The pointer to the rect.
 * \return TRUE for in the region, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa PtInRegion
 */
MG_EXPORT BOOL GUIAPI RectInRegion (PCLIPRGN region, const RECT* rect);

/**
 * \fn void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y)
 * \brief Offsets the region.
 *
 * This function offsets a given region pointed to by region.
 *
 * \param region The pointer to the region.
 * \param x x,y: Offsets on x and y coodinates.
 * \param y x,y: Offsets on x and y coodinates.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 */
MG_EXPORT void GUIAPI OffsetRegion (PCLIPRGN region, int x, int y);

#ifdef _USE_NEWGAL

/**
 * \fn BOOL GUIAPI UnionRegion (PCLIPRGN dst, const CLIPRGN* src1, const CLIPRGN* src2)
 * \brief Unions two regions.
 *
 * This function unions two regions pointed to by \a src1 and \a src2 respectively and
 * puts the result to the region pointed to by \a dst.
 *
 * \param dst The pointer to the result region.
 * \param src1 src1,src2: Two regions will be unioned.
 * \param src2 src1,src2: Two regions will be unioned.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa SubtractRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI UnionRegion (PCLIPRGN dst, const CLIPRGN* src1, const CLIPRGN* src2);

/**
 * \fn BOOL GUIAPI SubtractRegion (CLIPRGN* rgnD, const CLIPRGN* rgnM, const CLIPRGN* rgnS)
 * \brief Substrcts a region from another.
 *
 * This function subtracts \a rgnS from \a rgnM and leave the result in \a rgnD.
 *
 * \param rgnD The pointer to the difference region.
 * \param rgnM The pointer to the minuend region.
 * \param rgnS The pointer to the subtrahend region.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa UnionRegion, XorRegion
 */
MG_EXPORT BOOL GUIAPI SubtractRegion (CLIPRGN* rgnD, const CLIPRGN* rgnM, const CLIPRGN* rgnS);

/**
 * \fn BOOL GUIAPI XorRegion (CLIPRGN *dst, const CLIPRGN *src1, const CLIPRGN *src2)
 * \brief Does the XOR operation between two regions.
 *
 * This function does the XOR operation between two regions pointed to by
 * \a src1 and \a src2 and puts the result to the region pointed to by \a dst.
 *
 * \param dst The pointer to the result region.
 * \param src1 src1,src2: Two regions will be xor'ed.
 * \param src2 src1,src2: Two regions will be xor'ed.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa UnionRegion, SubtractRegion
 */
MG_EXPORT BOOL GUIAPI XorRegion (CLIPRGN *dst, const CLIPRGN *src1, const CLIPRGN *src2);

/**
 * \def UnionRectWithRegion
 * \brief Is an alias of \a AddClipRect
 * \sa AddClipRect
 */
#define UnionRectWithRegion     AddClipRect

/**
 * \def CopyRegion
 * \brief Is an alias of \a ClipRgnCopy
 * \sa ClipRgnCopy
 */
#define CopyRegion              ClipRgnCopy

/**
 * \def IntersectRegion
 * \brief Is an alias of \a ClipRgnIntersect
 * \sa ClipRgnIntersect
 */
#define IntersectRegion         ClipRgnIntersect

/**
 * \fn BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r)
 * \brief Initializes a region to be an enclosed circle.
 *
 * \param dst The pointer to the region to be initialized.
 * \param x x,y: The center of the circle.
 * \param y x,y: The center of the circle.
 * \param r The radius of the circle.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa InitEllipseRegion, InitPolygonRegion
 */
MG_EXPORT BOOL GUIAPI InitCircleRegion (PCLIPRGN dst, int x, int y, int r);

/**
 * \fn BOOL GUIAPI InitEllipseRegion (PCLIPRGN dst, int x, int y, int rx, int ry)
 * \brief Initializes a region to be an enclosed ellipse.
 *
 * \param dst The pointer to the region to be initialized.
 * \param x x,y: The center of the ellipse.
 * \param y x,y: The center of the ellipse.
 * \param rx The x-radius of the ellipse.
 * \param ry The y-radius of the ellipse.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa InitCircleRegion, InitPolygonRegion
 */
MG_EXPORT BOOL GUIAPI InitEllipseRegion (PCLIPRGN dst, int x, int y, int rx, int ry);

/**
 * \fn BOOL GUIAPI InitPolygonRegion (PCLIPRGN dst, const POINT* pts, int vertices)
 * \brief Initializes a region to be an enclosed polygon.
 *
 * \param dst The pointer to the region to be initialized.
 * \param pts The vertex array of the polygon.
 * \param vertices The number of the vertices.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This fucntion defined only for _USE_NEWGAL.
 *
 * \sa InitCircleRegion, InitEllipseRegion
 */
MG_EXPORT BOOL GUIAPI InitPolygonRegion (PCLIPRGN dst, const POINT* pts, int vertices);

#endif

    /** @} end of region_fns */

    /**
     * \defgroup dc_fns General DC operations
     *
     * DC means Device Context, just like Graphics Context (GC) of X Lib.
     * DC represents a rectangle area on the actual screen or a virtual screen
     * created in memory. We call the rectangle area as "surface" of the DC.
     *
     * You can call a GDI function and pass a DC to the function to 
     * draw lines, circles, or text. Commonly, you should call a function
     * like \a GetClientDC or \a CreateCompatibleDC to get or create a
     * DC, then call GDI functions to draw objects, e.g. \a MoveTo and \a LineTo.
     * After finishing drawing, you should call \a ReleaseDC or \a DeleteMemDC function
     * to release or destroy the DC.
     *
     * MiniGUI reserved an global DC called \a HDC_SCREEN. You can
     * use this DC directly without getting/creating or releasing/destroying.
     *
     * For main windows or controls, MiniGUI will send a MSG_PAINT message to
     * the window when the whole or part of window area have been invalidated. 
     * You should call \a BegainPaint function to get the DC, then repaint the window, 
     * and call \a EndPaint function to release the DC at the last.
     *
     * Example:
     *
     * \include msg_paint.c
     *
     * @{
     */

/**
 * \def HDC_SCREEN
 * \brief Handle to the device context of the whole screen.
 *
 * This DC is a special one. MiniGUI uses it to draw popup menus and other global
 * objects. You can also use this DC to draw lines or text on the screen 
 * directly, and there is no need to get or release it.
 *
 * If you do not want to create any main window, but you want to draw on
 * the screen, you can use this DC.
 *
 * \note MiniGUI does not do any clipping operation for this DC, so use this DC 
 * may make a mess of other windows. 
 */
#define HDC_SCREEN          0

/**
 * \def HDC_INVALID
 * \brief Indicates an invalid handle to device context.
 */
#define HDC_INVALID         0

#define GDCAP_COLORNUM      0
#define GDCAP_HPIXEL        1
#define GDCAP_VPIXEL        2
#define GDCAP_MAXX          3
#define GDCAP_MAXY          4
#define GDCAP_DEPTH         5
#define GDCAP_BPP           6

/**
 * \fn unsigned int GUIAPI GetGDCapability (HDC hdc, int iItem)
 * \brief Returns a capability of a DC.
 *
 * This function returns the capability of the specified item \a iItem of the DC \a hdc.
 *
 * \param hdc The handle to the DC.
 * \param iItem An integer presents the capablity, can be one of the following values:
 *
 *  - GDCAP_COLORNUM\n
 *    Tell \a GetGDCapability to return the colors number of the DC. Note the for a DC
 *    with 32-bit depth, the function will return 0xFFFFFFFF, not 0x100000000.
 *  - GDCAP_HPIXEL\n
 *    Tell \a GetGCapability to return the horizontal resolution of the DC.
 *  - GDCAP_VPIXEL\n
 *    Tell \a GetGDCapability to return the vertical resolution of the DC.
 *  - GDCAP_MAXX\n
 *    Tell \a GetGDCapability to return the maximal visible x value of the DC.
 *  - GDCAP_MAXY\n
 *    Tell \a GetGDCapability to return the maximal visible y value of the DC.
 *  - GDCAP_DEPTH\n
 *    Tell \a GetGDCapability to return the color depth of the DC. The returned value can be 
 *    1, 4, 8, 15, 16, 24, or 32.
 *  - GDCAP_BPP\n
 *    Tell \a GetGDCapability to return the bytes number for storing a pixle in the DC.
 *
 * \return The capbility.
 */
MG_EXPORT unsigned int GUIAPI GetGDCapability (HDC hdc, int iItem);

/**
 * \fn HDC GUIAPI GetDC (HWND hwnd)
 * \brief Gets a window DC of a window.
 *
 * This function gets a window DC of the specified \a hwnd, and returns the handle to the DC.
 * MiniGUI will try to return an unused DC from the internal DC pool, rather than
 * allocate a new one from the system heap. Thus, you should release the DC when
 * you finish drawing as soon as possible.
 *
 * \param hwnd The handle to the window.
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetClientDC, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetClientDC (HWND hwnd)
 * \brief Gets a client DC of a window.
 *
 * This function gets a client DC of the specified \a hwnd, and returns the handle to the DC.
 * MiniGUI will try to return an unused DC from the internal DC pool, rather than
 * allocate a new one from the system heap. Thus, you should release the DC when
 * you finish drawing as soon as possible.
 *
 * \param hwnd The handle to the window.
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \note You should call \a ReleaseDC to release the DC when you are done.
 *
 * \sa GetDC, ReleaseDC
 */
MG_EXPORT HDC GUIAPI GetClientDC (HWND hwnd);

/**
 * \fn void GUIAPI ReleaseDC (HDC hdc)
 * \brief Releases a DC from DC pool.
 *
 * This function releases the DC returned by \a GetDC or \a GetClientDC.
 *
 * \param hdc The handle to the DC.
 *
 * \sa GetDC, GetClientDC 
 */
MG_EXPORT void GUIAPI ReleaseDC (HDC hdc);

/**
 * \fn HDC GUIAPI CreatePrivateDC (HWND hwnd)
 * \brief Creates a private window DC of a window.
 *
 * This function creates a private window DC of the window \a hwnd and returns the handle to the DC.
 *
 * When you calling \a CreatePrivateDC function to create a private DC, MiniGUI will create 
 * the DC in the system heap, rather than allocate one from the DC pool. Thus, you can 
 * keep up the private DC in the life cycle of the window, and are not needed to release it for 
 * using by other windows.
 *
 * \param hwnd The handle to the window.
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa DeletePrivateDC
 */
MG_EXPORT HDC GUIAPI CreatePrivateDC (HWND hwnd);

/**
 * \fn HDC GUIAPI CreatePrivateClientDC (HWND hwnd)
 * \brief Creates a private client DC of a window.
 *
 * This function creates a private client DC of the window \a hwnd 
 * and returns the handle to the DC.
 *
 * When you calling \a CreatePrivateClientDC function to create a private client DC, 
 * MiniGUI will create the DC in the system heap, rather than allocate one from the DC pool. 
 * Thus, you can keep up the DC in the life cycle of the window, and are not needed to release 
 * it for using by other windows.
 *
 * \param hwnd The handle to the window.
 * \return The handle to the DC, HDC_INVALID indicates an error.
 *
 * \sa DeletePrivateDC
 */
MG_EXPORT HDC GUIAPI CreatePrivateClientDC (HWND hwnd);

/**
 * \fn HDC GUIAPI GetPrivateClientDC (HWND hwnd)
 * \brief Returns the private client DC of a window.
 *
 * This function returns the private client DC of the window \a hwnd which 
 * have extended style \a WS_EX_USEPRIVATECDC.
 *
 * When a main window have the extended style \a WS_EX_USEPRIVATECDC, or a
 * control class have the style \a CS_OWNDC, MiniGUI will create a private client DC 
 * for this window in the creation progress of the window, and destroy the DC when you 
 * destroy the window, and use this private client DC in default message handlers. 
 * So there will be some improvments on drawing/repaint performance.
 * You can alos call this function to get the private client DC, and use it to
 * draw anything in your window.
 *
 * \param hwnd The handle to the window.
 * \return The handle to the private client DC, HDC_INVALID indicates an error.
 *
 * \sa CreatePrivateClientDC
 */
MG_EXPORT HDC GUIAPI GetPrivateClientDC (HWND hwnd);

/**
 * \fn void GUIAPI DeletePrivateDC (HDC hdc)
 * \brief Deletes the DC returned by \a CreatePrivateDC or \a CreatePrivateClientDC.
 *
 * \param hdc The handle to the DC.
 *
 * \sa CreatePrivateDC, CreatePrivateClientDC
 */
MG_EXPORT void GUIAPI DeletePrivateDC (HDC hdc);

#ifdef _USE_NEWGAL

#define MEMDC_FLAG_NONE         0x00000000          /* None. */
#define MEMDC_FLAG_SWSURFACE    0x00000000          /* DC is in system memory */
#define MEMDC_FLAG_HWSURFACE    0x00000001          /* DC is in video memory */
#define MEMDC_FLAG_SRCCOLORKEY  0x00001000          /* Blit uses a source color key */
#define MEMDC_FLAG_SRCALPHA     0x00010000          /* Blit uses source alpha blending */
#define MEMDC_FLAG_RLEACCEL     0x00004000          /* Surface is RLE encoded */

/**
 * \fn HDC GUIAPI CreateCompatibleDCEx (HDC hdc, int width, int height)
 * \brief Creates a memory DC which is compatible with the specified reference DC.
 *
 * This function creates a memory DC which have the same pixel format
 * as the specified reference DC \a hdc. The same pixel format means that the memory DC will have
 * the same pixel depth, the same RGB composition, or the same palette as the reference DC.
 *
 * \param hdc The handle to the reference DC.
 * \param width The expected width of the result memory DC. If it is zero, the width will
 *        be equal to the width of the reference DC.
 * \param height The expected height of the result memory DC. If it is zero, the height will
 *        be equal to the height of the reference DC.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateCompatibleDC
 */
MG_EXPORT HDC GUIAPI CreateCompatibleDCEx (HDC hdc, int width, int height);

/**
 * \fn HDC GUIAPI CreateMemDC (int width, int height, int depth, DWORD flags, Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask)
 * \brief Creates a memory DC.
 *
 * This function creates a memory DC which have the specified flags and pixel format.
 *
 * \param width The expected width of the result memory DC.
 * \param height The expected height of the result memory DC.
 * \param depth The expected color depth of the memory DC.
 * \param flags The memory DC flags, can be or'ed values of the following flags:
 *
 *   - MEMDC_FLAG_SWSURFACE\n
 *     Creates the surface of memory DC in the system memory.
 *   - MEMDC_FLAG_HWSURFACE\n
 *     Creates the surface of memory DC in the video memory.
 *   - MEMDC_FLAG_SRCCOLORKEY\n
 *     The created memory DC will use a source color key to blit to other DC.
 *   - MEMDC_FLAG_SRCALPHA\n
 *     The created memory DC will use a source alpha blending to blit to other DC.
 *   - MEMDC_FLAG_RLEACCEL\n
 *     The memory DC will be RLE encoded
 *
 * \param Rmask The bit-masks of the red components in a pixel value.
 * \param Gmask The bit-masks of the green components in a pixel value.
 * \param Bmask The bit-masks of the blue components in a pixel value.
 * \param Amask The bit-masks of the alpha components in a pixel value.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDCFromBitmap, CreateMemDCFromMyBitmap, CreateCompatibleDCEx
 */
MG_EXPORT HDC GUIAPI CreateMemDC (int width, int height, int depth, DWORD flags,
                Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);

/**
 * \fn HDC GUIAPI CreateMemDCFromBitmap (HDC hdc, BITMAP* bmp)
 * \brief Creates a memory DC from a reference DC and a BITMAP object.
 *
 * This function creates a memory DC compatible with the 
 * specified DC, and use the bits of the BITMAP object as the surface of 
 * the memory DC. The created memory DC will have the same geometry as the
 * BITMAP object.
 *
 * \param hdc The reference DC.
 * \param bmp The BITMAP object.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDCFromMyBitmap, DeleteMemDC, BITMAP
 */
MG_EXPORT HDC GUIAPI CreateMemDCFromBitmap (HDC hdc, BITMAP* bmp);

/**
 * \fn HDC GUIAPI CreateMemDCFromMyBitmap (const MYBITMAP* my_bmp, RGB* pal)
 * \brief Creates a memory DC from a device independent MYBITMAP object.
 *
 * This function creates a memory DC which have the same
 * pixel format as the MYBITMAP object \a my_bmp, and use the bits of
 * the MYBITMAP object as the surface of the memory DC. The created memory DC 
 * will have the same geometry as the MYBITMAP object. If the depth of \a my_bmp
 * is 8-bit, the function will use \a pal to initialize the palette of
 * the memory DC.
 *
 * \param my_bmp The device independent MYBITMAP object.
 * \param pal The palette of the MYBITMAP object. If the depth of \a my_bmp
 *        is larger than 8-bit, this argument can be NULL.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDCFromBitmap, DeleteMemDC, MYBITMAP
 */
MG_EXPORT HDC GUIAPI CreateMemDCFromMyBitmap (const MYBITMAP* my_bmp, RGB* pal);

/**
 * \fn BOOL GUIAPI ConvertMemDC (HDC mem_dc, HDC ref_dc, DWORD flags)
 * \brief Converts a memory DC to have a same format as a reference DC.
 *
 * This function converts a memory DC \a mem_dc in order to
 * let it have the same pixel format as the reference DC \a ref_dc.
 * This function will try to create a new surface for \a mem_dc, 
 * and then copies and maps the surface of \a ref_dc to it so the blit of 
 * the converted memory DC will be as fast as possible. 
 *
 * The \a flags parameter has the same semantics as \a CreateMemDC. 
 * You can also pass MEMDC_FLAG_RLEACCEL in the flags parameter and
 * MiniGUI will try to RLE accelerate colorkey and alpha blits in the 
 * resulting memory DC.
 *
 * \param mem_dc The device context to be converted.
 * \param ref_dc The reference device context.
 * \param flags The memory DC flags, has the same semantics as \a CreateMemDC.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDC, DeleteMemDC
 */
MG_EXPORT BOOL GUIAPI ConvertMemDC (HDC mem_dc, HDC ref_dc, DWORD flags);

/**
 * \fn BOOL GUIAPI SetMemDCAlpha (HDC mem_dc, DWORD flags, Uint8 alpha)
 * \brief Sets the alpha value for the entire surface of a DC, as opposed to
 *        using the alpha component of each pixel.
 *
 * This function sets the alpha value for the entire surface of the DC \a mem_dc, 
 * as opposed to using the alpha component of each pixel. This value measures 
 * the range of transparency of the surface, 0 being completely transparent to 255
 * being completely opaque. An \a alpha value of 255 causes blits to be
 * opaque, the source pixels copied to the destination (the default). Note
 * that per-surface alpha can be combined with colorkey transparency.
 * 
 * If \a flags is 0, alpha blending is disabled for the surface.
 * If \a flags is MEMDC_FLAG_SRCALPHA, alpha blending is enabled for the surface.
 * OR'ing the flag with MEMDC_FLAG_RLEACCEL requests RLE acceleration for the
 * surface; if MEMDC_FLAG_RLEACCEL is not specified, the RLE acceleration 
 * will be removed.
 *
 * \param mem_dc The device context.
 * \param flags The alpha value specific memory DC flags.
 * \param alpha the alpha value for the entire surface of the DC \a mem_dc.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa SetMemDCColorKey
 */ 
MG_EXPORT BOOL GUIAPI SetMemDCAlpha (HDC mem_dc, DWORD flags, Uint8 alpha);

/**
 * \fn BOOL GUIAPI SetMemDCColorKey (HDC mem_dc, DWORD flags, Uint32 color_key)
 * \brief Sets the color key (transparent pixel) of a memory DC.
 *
 * This function sets the color key (transparent pixel) of the memory DC \a mem_dc.
 * If \a flags is MEMDC_FLAG_SRCCOLORKEY (optionally OR'ed with MEMDC_FLAG_RLEACCEL),
 * \a color_key will be the transparent pixel in the source DC of a blit.
 * MEMDC_FLAG_RLEACCEL requests RLE acceleration for the source of the DC if present,
 * and removes RLE acceleration if absent. If \a flags is 0, this function clears 
 * any current color key.
 *
 * \param mem_dc The device context.
 * \param flags The color key specific memory DC flags.
 * \param color_key the color_key of the memory DC.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa SetMemDCAlpha
 */
MG_EXPORT BOOL GUIAPI SetMemDCColorKey (HDC mem_dc, DWORD flags, Uint32 color_key);

/**
 * \fn void GUIAPI DeleteMemDC (HDC mem_dc)
 * \brief Deletes a memory DC.
 *
 * This function deletes the memory DC \a mem_dc, and frees the surface of the DC.
 * For the memory DC created from BITMAP object or MYBITMAP object, the bits used
 * by the surface of the DC will be reserved.
 *
 * \param mem_dc The device context to be deleted.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa CreateMemDC, CreateMemDCFromBitmap, CreateMemDCFromMyBitmap
 */
MG_EXPORT void GUIAPI DeleteMemDC (HDC mem_dc);

/**
 * \def CreateCompatibleDC(hdc)
 * \brief Creates a memory DC which is compatible with a given DC.
 *
 * This function creates a memory DC fully compatible with the reference 
 *        DC \a hdc, including pixel format and geomatry.
 *
 * \param hdc The reference DC.
 * \return The handle to the memory DC, HDC_INVALID indicates an error.
 *
 * \note Only defined as macro calling \a CreateCompatibleDCEx for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a CreateCompatibleDC is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa CreateCompatibleDCEx, DeleteMemDC
 */
#define CreateCompatibleDC(hdc) CreateCompatibleDCEx(hdc, 0, 0);

/**
 * \def DeleteCompatibleDC(hdc)
 * \brief Deletes a memory DC.
 *
 * This function deletes a memory DC created by \a CreateCompatibleDC.
 *
 * \param hdc The device context to be deleted.
 *
 * \note Only defined as macro calling \a DeleteMemDC for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a DeleteCompatibleDC is defined 
 *       as a function, and have the same semantics as this macro.
 *
 * \sa CreateCompatibleDC, DeleteMemDC
 */
#define DeleteCompatibleDC(hdc) DeleteMemDC(hdc)

/**
 * \fn Uint8* GUIAPI LockDC (HDC hdc, const RECT* rw_rc, int* width, int* height, int* pitch)
 * \brief Locks a dc to get direct access to pixels in the DC.
 *
 * Calling this function will try to lock the DC \a hdc to directly access the pixels
 * of the DC. You should tell this function the rectangle to be accessed, and the function
 * will return the effective \a width, \a height and \a pitch of the DC. The access 
 * beyond effective width and height will be invalid. 
 *
 * Locking a DC which uses screen surface will lock some global objects, such as mouse
 * cursor, and so on. All GDI calls of other threads (in MiniGUI-Threads) or other process 
 * (in MiniGUI-Lite) will be blocked as well. So you should call \a UnlockDC to unlock 
 * the DC as soon as possible, and should not call any system function in the duration of 
 * locking the DC.
 *
 * \param hdc The handle to the device context.
 * \param rw_rc The rectangle in device coordinate system to be accessed in the DC.
 * \param width The width of the effective rectangle can access will be returned through 
 *        this pointer.
 * \param height The height of the effective rectangle can access will be returned through 
 *        this pointer.
 * \param pitch The pitch of the scan line of the DC will be returned through this pointer.
 *        Pitch means the length of the scan line in bytes.
 * \return The bits pointer to the upper-left corner of the effective rectangle, NULL on error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa UnlockDC
 *
 * Example:
 *
 * \include lockdc.c
 */
MG_EXPORT Uint8* GUIAPI LockDC (HDC hdc, const RECT* rw_rc, int* width, int* height, int* pitch);

/**
 * \fn void GUIAPI UnlockDC (HDC hdc)
 * \brief Unlocks a locked DC.
 *
 * \param hdc The locked DC.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa LockDC
 */
MG_EXPORT void GUIAPI UnlockDC (HDC hdc);

   /**
    * \defgroup yuv_fns YUV overlay operations
    *
    * For an explanation of common video overlay formats, see:
    *
    * http://www.webartz.com/fourcc/indexyuv.htm
    *
    * For information on the relationship between color spaces, see:
    *
    * http://www.neuro.sfc.keio.ac.jp/~aly/polygon/info/color-space-faq.html
    *
    * @{
    */

#define GAL_YV12_OVERLAY  0x32315659    /* Planar mode: Y + V + U  (3 planes) */
#define GAL_IYUV_OVERLAY  0x56555949    /* Planar mode: Y + U + V  (3 planes) */
#define GAL_YUY2_OVERLAY  0x32595559    /* Packed mode: Y0+U0+Y1+V0 (1 plane) */
#define GAL_UYVY_OVERLAY  0x59565955    /* Packed mode: U0+Y0+V0+Y1 (1 plane) */
#define GAL_YVYU_OVERLAY  0x55595659    /* Packed mode: Y0+V0+Y1+U0 (1 plane) */

/**
 * The YUV hardware video overlay
 */
typedef struct GAL_Overlay {
    /** The overlay format, read-only */
    Uint32 format;
    /** The width of the overlay, read-only */
    int w;
    /** The height of the overlay, read-only */
    int h;
    /** The number of the planes of the overlay, read-only */
    int planes;
    /** The pitches of planes, read-only */
    Uint16 *pitches;
    /** The YUV pixels of planse, read-write */
    Uint8 **pixels;

    /* Hardware-specific surface info */
    struct private_yuvhwfuncs *hwfuncs;
    struct private_yuvhwdata *hwdata;

    /** Does This overlay hardware accelerated? */
    Uint32 hw_overlay :1;
    Uint32 UnusedBits :31;
} GAL_Overlay;

/**
 * \fn GAL_Overlay* GUIAPI CreateYUVOverlay (int width, int height, Uint32 format, HDC hdc)
 * \brief Creates a video output overlay on a DC.
 *
 * This function creates a video output overlay on the given DC \a hdc.
 * Calling the returned surface an \a overlay is something of a misnomer because
 * the contents of the display DC \a hdc underneath the area where the overlay
 * is shown is undefined - it may be overwritten with the converted YUV data.
 *
 * \param width The expected width of the video overlay.
 * \param height The expected height of the video overlay.
 * \param format The expected video overlay format, can be one of the following values:
 *
 *      - GAL_YV12_OVERLAY\n
 *        Planar mode: Y + V + U  (3 planes)
 *      - GAL_IYUV_OVERLAY\n
 *        Planar mode: Y + U + V  (3 planes)
 *      - GAL_YUY2_OVERLAY\n
 *        Packed mode: Y0+U0+Y1+V0 (1 plane)
 *      - GAL_UYVY_OVERLAY\n
 *        Packed mode: U0+Y0+V0+Y1 (1 plane)
 *      - GAL_YVYU_OVERLAY\n
 *        Packed mode: Y0+V0+Y1+U0 (1 plane)
 *
 * \param hdc The device context.
 * \return A GAL_Overlay object on success, NULL on error.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_LockYUVOverlay, GAL_FreeYUVOverlay, http://www.webartz.com/fourcc/indexyuv.htm
 */
MG_EXPORT GAL_Overlay* GUIAPI CreateYUVOverlay (int width, int height,
                Uint32 format, HDC hdc);

/**
 * \fn int GAL_LockYUVOverlay (GAL_Overlay *overlay)
 * \brief Locks an overlay for direct access.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_UnlockYUVOverlay
 */
int GAL_LockYUVOverlay (GAL_Overlay *overlay);

/**
 * \fn void GAL_UnlockYUVOverlay (GAL_Overlay *overlay)
 * \brief Unlocks a locked overlay when you are done
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_LockYUVOverlay
 */
void GAL_UnlockYUVOverlay (GAL_Overlay *overlay);

/**
 * \def LockYUVOverlay
 * \brief Is an alias of \a GAL_LockYUVOverlay
 * \sa GAL_LockYUVOverlay
 */
#define LockYUVOverlay GAL_LockYUVOverlay

/**
 * \def UnlockYUVOverlay
 * \brief Is an alias of \a GAL_UnlockYUVOverlay
 * \sa GAL_UnlockYUVOverlay
 */
#define UnlockYUVOverlay GAL_UnlockYUVOverlay

/**
 * \fn void GAL_FreeYUVOverlay (GAL_Overlay *overlay)
 * \brief Frees a YUV overlay.
 *
 * This function frees a YUV overlay created by \a GAL_CreateYUVOverlay.
 *
 * \param overlay The video overlay to be freed.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_CreateYUVOverlay
 */
void GAL_FreeYUVOverlay (GAL_Overlay *overlay);

/**
 * \def FreeYUVOverlay
 * \brief Is an alias of \a GAL_FreeYUVOverlay
 * \sa GAL_FreeYUVOverlay
 */
#define FreeYUVOverlay GAL_FreeYUVOverlay

/**
 * \fn void GUIAPI DisplayYUVOverlay (GAL_Overlay* overlay, const RECT* dstrect)
 * \brief Blits a video overlay to the screen.
 *
 * Calling this function will blit the video overly \a overlay to the screen. 
 * The contents of the video surface underneath the blit destination are
 * not defined. The width and height of the destination rectangle may be 
 * different from that of the overlay, but currently only 2x scaling is supported.
 * And note that the \a dstrect is in screen coordinate system.
 *
 * \param overlay The video overlay to be displayed.
 * \param dstrect The destination rectangle. It may be different from that of 
 *        the overlay, but currently only 2x scaling is supported
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_CreateYUVOverlay, GAL_LockYUVOverlay
 */
MG_EXPORT void GUIAPI DisplayYUVOverlay (GAL_Overlay* overlay, const RECT* dstrect);

    /** @} end of yuv_fns */

    /**
     * \defgroup gamma_fns Gamma correction functions
     * @{
     */

/**
 * \fn int GAL_SetGamma (float red, float green, float blue)
 * \brief Sets the gamma correction for each of the color channels.
 *
 * This function sets the gamma correction for each of the color channels.
 * The gamma values range (approximately) between 0.1 and 10.0.
 * If this function isn't supported directly by the hardware, it will
 * be emulated using gamma ramps, if available. 
 *
 * \param red The gamma correction value of red channel.
 * \param green The gamma correction value of green channel.
 * \param blue The gamma correction value of blue channel.
 * \return If successful, this function returns 0, otherwise it returns -1.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_SetGammaRamp
 */
int GAL_SetGamma (float red, float green, float blue);

/**
 * \def SetGamma
 * \brief Is an alias of \a GAL_SetGamma
 * \sa GAL_SetGamma
 */
#define SetGamma GAL_SetGamma

/**
 * \fn int GAL_SetGammaRamp (Uint16 *red, Uint16 *green, Uint16 *blue)
 * \brief Sets the gamma translation table for the red, green, and blue channels of the video hardware.  
 *
 * This function sets the gamma translation table for the red, green, and blue channels of the video hardware.  
 * Each table is an array of 256 16-bit quantities, representing a mapping 
 * between the input and output for that channel. The input is the index into 
 * the array, and the output is the 16-bit gamma value at that index, scaled 
 * to the output color precision.
 * 
 * You may pass NULL for any of the channels to leave it unchanged.
 *
 * \param red The gamma translation table for the red channel.
 * \param green The gamma translation table for the green channel.
 * \param blue The gamma translation table for the blue channel.
 * \return If the call succeeds, it will return 0. If the display driver or hardware does 
 * not support gamma translation, or otherwise fails, this function will return -1.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_GetGammaRamp, GAL_SetGamma
 */
int GAL_SetGammaRamp (Uint16 *red, Uint16 *green, Uint16 *blue);

/**
 * \def SetGammaRamp
 * \brief Is an alias of \a GAL_SetGammaRamp
 * \sa GAL_SetGammaRamp
 */
#define SetGammaRamp GAL_SetGammaRamp

/**
 * \fn int GAL_GetGammaRamp (Uint16 *red, Uint16 *green, Uint16 *blue)
 * \brief Retrieves the current values of the gamma translation tables.
 * 
 * This function retrives the current values of the gamma translationn tables.
 * You must pass in valid pointers to arrays of 256 16-bit quantities.
 * Any of the pointers may be NULL to ignore that channel.
 *
 * \param red Pointers to the array of gamma translation tables for the red channel.
 * \param green Pointers to the array of gamma translation tables for the green channel.
 * \param blue Pointers to the array of gamma translation tables for the blue channel.
 * \return If the call succeeds, it will return 0. If the display driver or
 *         hardware does not support gamma translation, or otherwise fails, 
 *         this function will return -1.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GAL_SetGammaRamp
 */
int GAL_GetGammaRamp (Uint16 *red, Uint16 *green, Uint16 *blue);

/**
 * \def GetGammaRamp
 * \brief Is an alias of \a GAL_GetGammaRamp
 * \sa GAL_GetGammaRamp
 */
#define GetGammaRamp GAL_GetGammaRamp

    /** @} end of gamma_fns */

#else

MG_EXPORT HDC GUIAPI CreateCompatibleDC (HDC hdc);
MG_EXPORT void GUIAPI DeleteCompatibleDC (HDC hdc);

#endif /* _USE_NEWGAL */

    /** @} end of dc_fns */

    /**
     * \defgroup dc_attrs DC attribute operations
     * @{
     */

#define BM_TRANSPARENT          1
#define BM_OPAQUE               0

#ifdef _USE_NEWGAL

#define DC_ATTR_BK_COLOR        0
#define DC_ATTR_BK_MODE         1
#define DC_ATTR_PEN_COLOR       2
#define DC_ATTR_BRUSH_COLOR     3
#define DC_ATTR_TEXT_COLOR      4
#define DC_ATTR_TAB_STOP        5

#define DC_ATTR_CHAR_EXTRA      6
#define DC_ATTR_ALINE_EXTRA     7
#define DC_ATTR_BLINE_EXTRA     8

#define DC_ATTR_MAP_MODE        9

#ifdef _ADV_2DAPI

#define DC_ATTR_PEN_TYPE        10
#define DC_ATTR_PEN_CAP_STYLE   11
#define DC_ATTR_PEN_JOIN_STYLE  12
#define DC_ATTR_PEN_WIDTH       13

#define DC_ATTR_BRUSH_TYPE      14

#define NR_DC_ATTRS             15

#else

#define NR_DC_ATTRS             10

#endif


/**
 * \fn Uint32 GUIAPI GetDCAttr (HDC hdc, int attr)
 * \brief Gets a specified attribute value of a DC.
 *
 * This function retrives a specified attribute value of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param attr The attribute to be retrived, can be one of the following values:
 *
 *      - DC_ATTR_BK_COLOR\n
 *        Background color.
 *      - DC_ATTR_BK_MODE\n
 *        Background mode.
 *      - DC_ATTR_PEN_TYPE\n
 *        Pen type.
 *      - DC_ATTR_PEN_CAP_STYLE\n
 *        Cap style of pen.
 *      - DC_ATTR_PEN_JOIN_STYLE\n
 *        Join style of pen.
 *      - DC_ATTR_PEN_COLOR\n
 *        Pen color.
 *      - DC_ATTR_BRUSH_TYPE\n
 *        Brush type.
 *      - DC_ATTR_BRUSH_COLOR\n
 *        Brush color.
 *      - DC_ATTR_TEXT_COLOR\n
 *        Text color.
 *      - DC_ATTR_TAB_STOP\n
 *        Tabstop width.
 *
 * \return The attribute value.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa SetDCAttr
 */
MG_EXPORT Uint32 GUIAPI GetDCAttr (HDC hdc, int attr);

/**
 * \fn Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value)
 * \brief Sets a specified attribute value of a DC.
 *
 * This function sets a specified attribute value of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param attr The attribute to be set.
 * \param value The attribute value.
 * \return The old attribute value.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GetDCAttr
 */
MG_EXPORT Uint32 GUIAPI SetDCAttr (HDC hdc, int attr, Uint32 value);

/**
 * \def GetBkColor(hdc)
 * \brief Gets the background color of a DC.
 *
 * \param hdc The device context.
 * \return The background pixel value of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetBkColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetBkColor
 */
#define GetBkColor(hdc)     (gal_pixel) GetDCAttr (hdc, DC_ATTR_BK_COLOR)

/**
 * \def GetBkMode(hdc)
 * \brief Gets the background mode of a DC.
 *
 * \param hdc The device context.
 * \return The background mode of the DC \a hdc.
 *
 * \retval BM_TRANSPARENT   Indicate that reserve the background untouched when draw text. 
 * \retval BM_OPAQUE        Indicate that erase the background with background color when draw text.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetBkMode is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetBkMode
 */
#define GetBkMode(hdc)      (int)       GetDCAttr (hdc, DC_ATTR_BK_MODE)

/**
 * \def GetPenColor(hdc)
 * \brief Gets the pen color of a DC.
 *
 * \param hdc The device context.
 * \return The pen color (pixel value) of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetPenColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetPenColor
 */
#define GetPenColor(hdc)    (gal_pixel) GetDCAttr (hdc, DC_ATTR_PEN_COLOR)

/**
 * \def GetBrushColor(hdc)
 * \brief Gets the brush color of a DC.
 *
 * \param hdc The device context.
 * \return The brush color (pixel value) of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetBrushColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetBrushColor
 */
#define GetBrushColor(hdc)  (gal_pixel) GetDCAttr (hdc, DC_ATTR_BRUSH_COLOR)

/**
 * \def GetTextColor(hdc)
 * \brief Gets the text color of a DC.
 *
 * \param hdc The device context.
 * \return The text color (pixel value) of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetTextColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetTextColor
 */
#define GetTextColor(hdc)   (gal_pixel) GetDCAttr (hdc, DC_ATTR_TEXT_COLOR)

/**
 * \def GetTabStop(hdc)
 * \brief Gets the tabstop value of a DC.
 *
 * \param hdc The device context.
 * \return The tabstop value of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetTabStop is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetDCAttr, SetTabStop
 */
#define GetTabStop(hdc)     (int)       GetDCAttr (hdc, DC_ATTR_TAB_STOP)

/**
 * \def SetBkColor(hdc, color)
 * \brief Sets the background color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new background color (pixel value).
 * \return The old background pixel value of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetBkColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetBkColor
 */
#define SetBkColor(hdc, color)      (gal_pixel) SetDCAttr (hdc, DC_ATTR_BK_COLOR, (DWORD) color)

/**
 * \def SetBkMode(hdc, mode)
 * \brief Sets the background color of a DC to a new mode.
 *
 * \param hdc The device context.
 * \param mode The new background mode, be can one of the following values:
 *
 *      - BM_TRANSPARENT\n
 *        Indicate that reserve the background untouched when draw text. 
 *      - BM_OPAQUE\n
 *        Indicate that erase the background with background color when draw text.
 *
 * \return The old background mode of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetBkMode is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetBkMode
 */
#define SetBkMode(hdc, mode)        (int)       SetDCAttr (hdc, DC_ATTR_BK_MODE, (DWORD) mode)

/**
 * \def SetPenColor(hdc, color)
 * \brief Sets the pen color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new pen color (pixel value).
 * \return The old pen pixel value of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetPenColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetPenColor
 */
#define SetPenColor(hdc, color)     (gal_pixel) SetDCAttr (hdc, DC_ATTR_PEN_COLOR, (DWORD) color)

/**
 * \def SetBrushColor(hdc, color)
 * \brief Sets the brush color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new brush color (pixel value).
 * \return The old brush pixel value of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetBrushColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetBrushColor
 */
#define SetBrushColor(hdc, color)   (gal_pixel) SetDCAttr (hdc, DC_ATTR_BRUSH_COLOR, (DWORD) color)

/**
 * \def SetTextColor(hdc, color)
 * \brief Sets the text color of a DC to a new value.
 *
 * \param hdc The device context.
 * \param color The new text color (pixel value).
 * \return The old text color (pixel value) of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetTextColor is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetTextColor
 */
#define SetTextColor(hdc, color)    (gal_pixel) SetDCAttr (hdc, DC_ATTR_TEXT_COLOR, (DWORD) color)

/**
 * \def SetTabStop(hdc, value)
 * \brief Sets the tabstop of a DC to a new value.
 *
 * \param hdc The device context.
 * \param value The new tabstop value in pixels.
 * \return The old tabstop value in pixels of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetTabStop is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetDCAttr, GetTabStop
 */
#define SetTabStop(hdc, value)      (int)       SetDCAttr (hdc, DC_ATTR_TAB_STOP, (DWORD) value)

#define ROP_SET         0
#define ROP_AND         1
#define ROP_OR          2
#define ROP_XOR         3

/**
 * \fn int GUIAPI GetRasterOperation (HDC hdc)
 * \brief Gets the raster operation of a DC.
 *
 * This function gets the raster operation of the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The current raster operation of the DC \a hdc.
 *
 * \retval ROP_SET      Set to the new pixel value, erase original pixel on the surface.
 * \retval ROP_AND      AND'd the new pixel value with the original pixel on the surface.
 * \retval ROP_OR       OR'd the new pixel value with the original pixel on the surface.
 * \retval ROP_XOR      XOR'd the new pixel value with the original pixel on the surface.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa SetRasterOperation
 */
MG_EXPORT int GUIAPI GetRasterOperation (HDC hdc);

/**
 * \fn int GUIAPI SetRasterOperation (HDC hdc, int rop)
 * \brief Sets the raster operation of a DC to a new value.
 *
 * This function sets the raster operation of the DC \a hdc to the new value \a rop.
 *
 * \param hdc The device context.
 * \param rop The new raster operation, can be one of the following values:
 *
 *      - ROP_SET\n
 *        Set to the new pixel value, erase original pixel on the surface.
 *      - ROP_AND\n
 *        AND'd the new pixel value with the original pixel on the surface.
 *      - ROP_OR\n
 *        OR'd the new pixel value with the original pixel on the surface.
 *      - ROP_XOR\n
 *        XOR'd the new pixel value with the original pixel on the surface.
 *
 * \return The old raster operation of the DC \a hdc.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa GetRasterOperation
 */
MG_EXPORT int GUIAPI SetRasterOperation (HDC hdc, int rop);

#else

MG_EXPORT gal_pixel GUIAPI GetBkColor (HDC hdc);
MG_EXPORT int GUIAPI GetBkMode (HDC hdc);
MG_EXPORT gal_pixel GUIAPI GetTextColor (HDC hdc);
MG_EXPORT gal_pixel GUIAPI SetBkColor (HDC hdc, gal_pixel color);
MG_EXPORT int GUIAPI SetBkMode (HDC hdc, int bkmode);
MG_EXPORT gal_pixel GUIAPI SetTextColor (HDC hdc, gal_pixel color);

MG_EXPORT int GUIAPI GetTabStop (HDC hdc);
MG_EXPORT int GUIAPI SetTabStop (HDC hdc, int new_value);

/* Pen and brush support */
MG_EXPORT gal_pixel GUIAPI GetPenColor (HDC hdc);
MG_EXPORT gal_pixel GUIAPI GetBrushColor (HDC hdc);
MG_EXPORT gal_pixel GUIAPI SetPenColor (HDC hdc, gal_pixel color);
MG_EXPORT gal_pixel GUIAPI SetBrushColor (HDC hdc, gal_pixel color);

#endif /* _USE_NEWGAL */

    /** @} end of dc_attrs */

    /**
     * \defgroup pal_fns Palette operations
     * @{
     */

#ifdef _USE_NEWGAL

/**
 * \fn BOOL GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
 * \brief Gets palette entries of a DC.
 *
 * This function gets some palette entries of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param start The start entry of palette to be retrived.
 * \param len The length of entries to be retrived.
 * \param cmap The buffer receives the palette entries.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetPalette
 */
MG_EXPORT BOOL GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap);

/**
 * \fn BOOL GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap)
 * \brief Sets palette entries of a DC.
 *
 * This function sets some palette entries of the DC \a hdc.
 *
 * \param hdc The device context.
 * \param start The start entry of palette to be set.
 * \param len The length of entries to be set.
 * \param cmap Pointer to the palette entries.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa GetPalette
 */
MG_EXPORT BOOL GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap);

/**
 * \fn BOOL GUIAPI SetColorfulPalette (HDC hdc)
 * \brief Sets a DC with colorfule palette.
 *
 * This function sets the DC specified by \a hdc with colorful palette.
 *
 * \param hdc The device context.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetPalette
 */
MG_EXPORT BOOL GUIAPI SetColorfulPalette (HDC hdc);

#else

MG_EXPORT int GUIAPI GetPalette (HDC hdc, int start, int len, GAL_Color* cmap);
MG_EXPORT int GUIAPI SetPalette (HDC hdc, int start, int len, GAL_Color* cmap);
MG_EXPORT int GUIAPI SetColorfulPalette (HDC hdc);

#endif /* _USE_NEWGAL */

    /** @} end of pal_fns */

    /**
     * \defgroup draw_fns General drawing functions
     * @{
     */

#ifdef _USE_NEWGAL

/**
 * \fn void GUIAPI SetPixel (HDC hdc, int x, int y, gal_pixel pixel)
 * \brief Sets the pixel with a new pixel value at the specified position on a DC.
 *
 * This function sets the pixel with a pixel value \a pixel at the specified position \a (x,y) 
 * on the DC \a hdc. You can the pre-defined standard system pixel values.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param pixel The pixel value.
 *
 * \sa GetPixel, color_vars
 */
MG_EXPORT void GUIAPI SetPixel (HDC hdc, int x, int y, gal_pixel pixel);

/**
 * \fn gal_pixel GUIAPI SetPixelRGB (HDC hdc, int x, int y, Uint8 r, Uint8 g, Uint8 b)
 * \brief Sets the pixel by a RGB triple at the specified position on a DC.
 *
 * This function sets the pixel with a RGB triple \a (r,g,b) at the specified position \a (x,y) 
 * on the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \return The pixel value of the RGB triple.
 *
 * \sa GetPixel, RGB2Pixel
 */
MG_EXPORT gal_pixel GUIAPI SetPixelRGB (HDC hdc, int x, int y, Uint8 r, Uint8 g, Uint8 b);

/**
 * \fn gal_pixel GUIAPI SetPixelRGBA (HDC hdc, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
 * \brief Sets the pixel by a RGBA quarter at the specified position on a DC.
 *
 * This function sets the pixel with a RGBA quarter \a (r,g,b,a) at the specified position \a (x,y) 
 * on the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \param a The alpha component of a color.
 * \return The pixel value of the RGBA quarter.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa GetPixel, RGBA2Pixel
 */
MG_EXPORT gal_pixel GUIAPI SetPixelRGBA (HDC hdc, int x, int y, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/**
 * \fn gal_pixel GUIAPI GetPixel (HDC hdc, int x, int y)
 * \brief Gets the pixel value at the specified position on a DC.
 *
 * This function gets the pixel value at the specified position \a (x,y) on the DC \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \return The pixel value.
 *
 * \sa SetPixel
 */
MG_EXPORT gal_pixel GUIAPI GetPixel (HDC hdc, int x, int y);

/**
 * \fn gal_pixel GUIAPI GetPixelRGB (HDC hdc, int x, int y, Uint8* r, Uint8* g, Uint8* b)
 * \brief Gets the pixel value at the specified position on a DC in RGB triple.
 *
 * This function gets the pixel value at the specified position \a (x,y) 
 * on the DC \a hdc in RGB triple.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of the pixel will be returned through these pointers.
 * \param g The green component of the pixel will be returned through these pointers.
 * \param b The blue component of the pixel will be returned through these pointers.
 * \return The pixel value.
 *
 * \sa SetPixelRGB
 */
MG_EXPORT gal_pixel GUIAPI GetPixelRGB (HDC hdc, int x, int y, Uint8* r, Uint8* g, Uint8* b);

/**
 * \fn gal_pixel GUIAPI GetPixelRGBA (HDC hdc, int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
 * \brief Gets the pixel value at the specified position on a DC in RGBA quarter.
 *
 * This function gets the pixel value at the specified position \a (x,y) on the DC \a hdc 
 * in RGBA quarter.
 *
 * \param hdc The device context.
 * \param x x,y: The pixel position.
 * \param y x,y: The pixel position.
 * \param r The red component of the pixel will be returned through these pointers.
 * \param g The green component of the pixel will be returned through these pointers.
 * \param b The blue component of the pixel will be returned through these pointers.
 * \param a The alpha component of the pixel will be returned through these pointers.
 * \return The pixel value.
 *
 * \sa SetPixelRGBA
 */
MG_EXPORT gal_pixel GUIAPI GetPixelRGBA (HDC hdc, int x, int y, Uint8* r, Uint8* g, Uint8* b, Uint8* a);

/**
 * \fn gal_pixel GUIAPI RGBA2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
 * \brief Gets the pixel value from a color in RGBA quarter under a DC.
 *
 * This function gets the pixel value from the RGB quarter \a (r,g,b,a) under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \param a The alpha component of a color.
 * \return The pixel value.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa Pixel2RGBA
 */
MG_EXPORT gal_pixel GUIAPI RGBA2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b, Uint8 a);

/**
 * \fn gal_pixel GUIAPI RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b)
 * \brief Gets the pixel value from a color in RGB triple under a DC.
 *
 * This function gets the pixel value from the RGB triple \a (r,g,b) under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param r The red component of a color.
 * \param g The green component of a color.
 * \param b The blue component of a color.
 * \return The pixel value.
 *
 * \sa Pixel2RGB
 */
MG_EXPORT gal_pixel GUIAPI RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b);

/**
 * \fn void GUIAPI Pixel2RGB (HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b)
 * \brief Gets the color in RGB triple from a pixel value under a DC.
 *
 * This function gets the color in RGB triple from the pixel value \a pixel under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixel The pixel value.
 * \param r The red component of the pixel will be returned through this pointer.
 * \param g The green component of the pixel will be returned through this pointer.
 * \param b The blue component of the pixel will be returned through this pointer.
 * \return The pixel value.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa RGB2Pixel
 */
MG_EXPORT void GUIAPI Pixel2RGB (HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b);

/**
 * \fn void GUIAPI Pixel2RGBA (HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b, Uint8* a)
 * \brief Gets the color in RGBA quarter from a pixel value under a DC.
 *
 * This function gets the color in RGBA quarter from the pixel value \a pixel under the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pixel The pixel value.
 * \param r The red component of the pixel will be returned through this pointer.
 * \param g The green component of the pixel will be returned through this pointer.
 * \param b The blue component of the pixel will be returned through this pointer.
 * \param a The alpha component of the pixel will be returned through this pointer.
 * \return The pixel value.
 *
 * \note Only defined for _USE_NEWGAL. 
 *
 * \sa RGB2Pixel
 */
MG_EXPORT void GUIAPI Pixel2RGBA (HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b, Uint8* a);

#else

MG_EXPORT void GUIAPI SetPixel (HDC hdc, int x, int y, gal_pixel c);
MG_EXPORT void GUIAPI SetPixelRGB (HDC hdc, int x, int y, int r, int g, int b);
MG_EXPORT gal_pixel GUIAPI GetPixel (HDC hdc, int x, int y);
MG_EXPORT void GUIAPI GetPixelRGB (HDC hdc, int x, int y, int* r, int* g, int* b);
MG_EXPORT gal_pixel GUIAPI RGB2Pixel (HDC hdc, Uint8 r, Uint8 g, Uint8 b);
MG_EXPORT void GUIAPI Pixel2RGB (HDC hdc, gal_pixel pixel, Uint8* r, Uint8* g, Uint8* b);

#endif /* USE_NEWGAL */

/**
 * \fn void GUIAPI FocusRect (HDC hdc, int x0, int y0, int x1, int y1)
 * \brief Draws a focus rectangle.
 *
 * This function uses XOR mode to draw the focus rectangle on the device context, i.e.
 * calling this function then calling this function with same arguments again 
 * will erase the focus rectangle and restore the pixels before the first call.
 * 
 * \param hdc The device context.
 * \param x0 x0,y0: The coordinates of upper-left corner of the rectangle.
 * \param y0 x0,y0: The coordinates of upper-left corner of the rectangle.
 * \param x1 x1,y1: The corrdinates of lower-right corner of the rectangle.
 * \param y1 x1,y1: The corrdinates of lower-right corner of the rectangle.
 * 
 * \sa DrawHVDotLine
 */
MG_EXPORT void GUIAPI FocusRect (HDC hdc, int x0, int y0, int x1, int y1);

/**
 * \fn void GUIAPI DrawHVDotLine (HDC hdc, int x, int y, int w_h, BOOL H_V)
 * \brief Draws a horizontal or vertical dot dash line.
 *
 * This function draws a horizontal or vertical dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param w_h The width of the line or horizontal dot line, or the height
 *            if you want to draw a vertical dot line.
 * \param H_V Specify whether you want to draw a horizontal or vertical line.
 *            TRUE for horizontal, FALSE for vertical.
 *
 * \sa FocusRect
 */
MG_EXPORT void GUIAPI DrawHVDotLine (HDC hdc, int x, int y, int w_h, BOOL H_V);

/**
 * \def DrawHDotLine(hdc, x, y, w)
 * \brief Draws a horizontal dot dash line.
 *
 * This function draws a horizontal dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param w The width of the horizontal dot line.
 *
 * \note Defined as a macro calling DrawHVDotLine.
 *
 * \sa DrawVDotLine, DrawHVDotLine
 */
#define DrawHDotLine(hdc, x, y, w) DrawHVDotLine (hdc, x, y, w, TRUE);

/**
 * \def DrawVDotLine(hdc, x, y, h)
 * \brief Draws a vertical dot line.
 *
 * This function draws a vertical dot dash line with
 * the zero pen.
 *
 * \param hdc The device context.
 * \param x x,y: The start point of the line.
 * \param y x,y: The start point of the line.
 * \param h The height of the horizontal dot line.
 *
 * \note Defined as a macro calling DrawHVDotLine.
 *
 * \sa DrawHDotLine, DrawHVDotLine
 */
#define DrawVDotLine(hdc, x, y, h) DrawHVDotLine (hdc, x, y, h, FALSE);

#ifdef _USE_NEWGAL

/**
 * \fn BOOL GUIAPI LineClipper (const RECT* cliprc, int *_x0, int *_y0, int *_x1, int *_y1)
 * \brief The line clipper using Cohen-Sutherland algorithm.
 *
 * This function clips a line from \a (*_x0,*_y0) to \a (*_x1, *_y1) 
 * with the specified clipping rectangle pointed to by \a cliprc. Then return the
 * clipped line throught the pointers \a (_x0, _y0, _x1, _y1).
 *
 * It is modified to do pixel-perfect clipping. This means that it
 * will generate the same endpoints that would be drawn if an ordinary
 * Bresenham line-drawer where used and only visible pixels drawn.
 *
 * \param cliprc The Pointer to the clipping rectangle.
 * \param _x0 _x0,_y0: The pointers contains the start point of the line.
 *        The start point after clipping will be returned through them as well as.
 * \param _y0 _x0,_y0: The pointers contains the start point of the line.
 *        The start point after clipping will be returned through them as well as.
 * \param _x1 _x1,_y1: The pointers contains the end point of the line.
 *        The end point after clipping will be returned through them as well as.
 * \param _y1 _x1,_y1: The pointers contains the end point of the line.
 *        The end point after clipping will be returned through them as well as.
 * \return TRUE for clipped by the clipping rectangle, FALSE for not clipped.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa LineGenerator
 */
MG_EXPORT BOOL GUIAPI LineClipper (const RECT* cliprc, int *_x0, int *_y0, int *_x1, int *_y1);

/**
 * \var typedef void (* CB_LINE) (void* context, int stepx, int stepy)
 * \brief The type of line generator callback.
 */
typedef void (* CB_LINE) (void* context, int stepx, int stepy);

/**
 * \fn void GUIAPI LineGenerator (void* context, int x1, int y1, int x2, int y2, CB_LINE cb)
 * \brief A line generator based-on Breshenham algorithm.
 *
 * This is a Breshenham line generator. When it generates a new point on the line,
 * it will call the callback \a cb and pass the context \a context and the step values
 * on x-coordinate and y-coodinate since the last point. The first generated point always 
 * be the start point you passed to this function. Thus, the first step values passed to 
 * \a cb will be (0,0).
 *
 * MiniGUI implements \a LineTo function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param x1 x1,y1: The start point of the line.
 * \param y1 x1,y1: The start point of the line.
 * \param x2 x2,y2: The end point of the line.
 * \param y2 x2,y2: The end point of the line.
 * \param cb The line generator callback.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa LineTo, LineClipper, CB_LINE
 */
MG_EXPORT void GUIAPI LineGenerator (void* context, int x1, int y1, int x2, int y2, CB_LINE cb);

/**
 * \var typedef void (* CB_CIRCLE) (void* context, int x1, int x2, int y)
 * \brief The type of circle generator callback.
 */
typedef void (* CB_CIRCLE) (void* context, int x1, int x2, int y);

/**
 * \fn void GUIAPI CircleGenerator (void* context, int sx, int sy, int r, CB_CIRCLE cb)
 * \brief A circle generator.
 *
 * This is a general circle generator, it will generate two points on the same 
 * horizontal scan line at every turn. When it generates the points on the circle,
 * it will call the callback \a cb and pass the context \a context, the y-coordinate
 * of the scan line, the x-coordinate of the left point on the scan line, and 
 * the x-coordinate of the right point. Note that in some cases, the left point and
 * the right point will be the same.
 *
 * MiniGUI implements \a Circle, \a FillCircle and \a InitCircleRegion functions
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 * \param cb The circle generator callback.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa Circle, FillCircle, InitCircleRegion, CB_CIRCLE
 *
 * Example:
 *
 * \include circlegenerator.c
 */
MG_EXPORT void GUIAPI CircleGenerator (void* context, int sx, int sy, int r, CB_CIRCLE cb);

/**
 * \var typedef void (* CB_ELLIPSE) (void* context, int x1, int x2, int y)
 * \brief The type of ellipse generator callback.
 */
typedef void (* CB_ELLIPSE) (void* context, int x1, int x2, int y);

/**
 * \fn void GUIAPI EllipseGenerator (void* context, int sx, int sy, int rx, int ry, CB_ELLIPSE cb)
 * \brief An ellipse generator.
 *
 * This is a general ellipse generator, it will generate two points on the same 
 * horizontal scan line at every turn. When it generates the points on the ellipse,
 * it will call the callback \a cb and pass the context \a context, the y-coordinate
 * of the scan line, the x-coordinate of the left point on the scan line, and 
 * the x-coordinate of the right point. Note that in some cases, the left point and
 * the right point will be the same.
 *
 * MiniGUI implements \a Ellipse, \a FillEllipse, and \a InitEllipseRegion functions
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the ellipse.
 * \param sy sx,sy: The center of the ellipse.
 * \param rx The x-radius of the ellipse.
 * \param ry The y-radius of the ellipse.
 * \param cb The ellipse generator callback.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa Ellipse, FillEllipse, InitEllipseRegion, CB_CIRCLE
 */
MG_EXPORT void GUIAPI EllipseGenerator (void* context, int sx, int sy, int rx, int ry, CB_ELLIPSE cb);

/**
 * \var typedef void (* CB_ARC) (void* context, int x, int y)
 * \brief The type of arc generator callback.
 */
typedef void (* CB_ARC) (void* context, int x, int y);

/**
 * \fn void GUIAPI CircleArcGenerator (void* context, int sx, int sy, int r, int ang1, int ang2, CB_ARC cb)
 * \brief An arc generator.
 *
 * This is a general arc generator. When it generates a point on the arc,
 * it will call the callback \a cb and pass the context \a context, the coordinates
 * of the point.
 *
 * MiniGUI implements \a CircleArc function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param sx sx,sy: The center of the arc.
 * \param sy sx,sy: The center of the arc.
 * \param r The radius of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position, 
 *              counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1, in 1/64ths of a degree.
 * \param cb The arc generator callback.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa CircleArc, CB_ARC, fixed_math_fns
 */
MG_EXPORT void GUIAPI CircleArcGenerator (void* context, int sx, int sy, int r, int ang1, int ang2, CB_ARC cb);

/**
 * \var typedef void (* CB_POLYGON) (void* context, int x1, int x2, int y)
 * \brief The type of polygon generator callback.
 */
typedef void (* CB_POLYGON) (void* context, int x1, int x2, int y);

/**
 * \fn BOOL GUIAPI MonotoneVerticalPolygonGenerator (void* context, const POINT* pts, int vertices, CB_POLYGON cb)
 * \brief A monotone vertical polygon generator.
 *
 * This is a monotone vertical polygon generator.
 *
 * "Monoton vertical" means "monotone with respect to a vertical line";
 * that is, every horizontal line drawn through the polygon at any point
 * would cross exactly two active edges (neither horizontal lines
 * nor zero-length edges count as active edges; both are acceptable
 * anywhere in the polygon). Right & left edges may cross (polygons may be nonsimple).
 * Polygons that are not convex according to this definition won't be drawn properly.
 *
 * You can call \a PolygonIsMonotoneVertical function to check one polygon is
 * monotone vertical or not.
 *
 * This function will generate two points on the same horizontal scan line at every turn. 
 * When it generates the points on the polygon, it will call the callback \a cb and 
 * pass the context \a context, the y-coordinate of the scan line, the x-coordinate of 
 * the left point on the scan line, and the x-coordinate of the right point. 
 * Note that in some cases, the left point and the right point will be the same.
 *
 * MiniGUI implements \a FillPolygon and \a InitPolygonRegion functions 
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex array.
 * \param cb The polygon generator callback.
 * \return TRUE on success, FALSE on error.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa FillPolygon, InitPolygonRegion, PolygonIsMonotoneVertical, CB_CIRCLE
 */
MG_EXPORT BOOL GUIAPI MonotoneVerticalPolygonGenerator (void* context, const POINT* pts, int vertices, CB_POLYGON cb);

/**
 * \fn BOOL GUIAPI PolygonIsMonotoneVertical (const POINT* pts, int vertices)
 * \brief Checks a polygon is monotone vertical or not.
 *
 * This function checks if the given polygon is monotone vertical.
 *
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex array.
 * \return TRUE if it is monotonoe vertical, otherwise FALSE.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa MonotoneVerticalPolygonGenerator
 */
MG_EXPORT BOOL GUIAPI PolygonIsMonotoneVertical (const POINT* pts, int vertices);

/**
 * \fn BOOL GUIAPI PolygonGenerator (void* context, const POINT* pts, int vertices, CB_POLYGON cb)
 * \brief A general polygon generator.
 *
 * This is a general polygon generator.
 *
 * This function will generate two points on the same horizontal scan line at every turn. 
 * When it generates the points on the polygon, it will call the callback \a cb and 
 * pass the context \a context, the y-coordinate of the scan line, the x-coordinate of 
 * the left point on the scan line, and the x-coordinate of the right point. 
 * Note that in some cases, the left point and the right point will be the same.
 *
 * MiniGUI implements \a FillPolygon and \a InitPolygonRegion functions 
 * by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex array.
 * \param cb The polygon generator callback.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa FillPolygon, InitPolygonRegion, CB_CIRCLE
 */
MG_EXPORT BOOL GUIAPI PolygonGenerator (void* context, const POINT* pts, int vertices, CB_POLYGON cb);

/**
 * \var typedef BOOL (* CB_EQUAL_PIXEL) (void* context, int x, int y)
 * \brief Flodd filling generator's equation callback.
 */
typedef BOOL (* CB_EQUAL_PIXEL) (void* context, int x, int y);

/**
 * \var typedef void (* CB_FLOOD_FILL) (void* context, int x1, int x2, int y)
 * \brief Flodd filling generator's scan line callback.
 */
typedef void (* CB_FLOOD_FILL) (void* context, int x1, int x2, int y);

/**
 * \fn BOOL GUIAPI FloodFillGenerator (void* context, const RECT* src_rc, int x, int y, CB_EQUAL_PIXEL cb_equal_pixel, CB_FLOOD_FILL cb_flood_fill)
 * \brief A flood filling generator.
 *
 * This function is a general flood filling generator.
 *
 * MiniGUI implements \a FloodFill function by using this generator.
 *
 * \param context The context, will be passed to the callback \a cb.
 * \param src_rc The filling bounding rectangle.
 * \param x x,y: The start filling point.
 * \param y x,y: The start filling point.
 * \param cb_equal_pixel The callback to check the pixel is equal with the start point or not.
 * \param cb_flood_fill The callback to fill a scan line.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa FloodFill, CB_EQUAL_PIXEL, CB_FLOOD_FILL
 */
MG_EXPORT BOOL GUIAPI FloodFillGenerator (void* context, const RECT* src_rc, int x, int y, 
                CB_EQUAL_PIXEL cb_equal_pixel, CB_FLOOD_FILL cb_flood_fill);

/**
 * \var typedef void* (* CB_GET_LINE_BUFF) (void* context, int y)
 * \brief Bitmap scaler's getting line buffer callback.
 */
typedef void* (* CB_GET_LINE_BUFF) (void* context, int y);

/**
 * \var typedef void* (* CB_LINE_SCALED) (void* context, const void* line, int y)
 * \brief Bitmap scaler's getting line buffer callback.
 */
typedef void (* CB_LINE_SCALED) (void* context, const void* line, int y);

/**
 * \fn BOOL GUIAPI BitmapDDAScaler (void* context, const BITMAP* src_bmp, int dst_w, int dst_h, CB_GET_LINE_BUFF cb_get_line_buff, CB_LINE_SCALED cb_line_scaled)
 * \brief A bitmap scaler using DDA algorithm.
 *
 * This function is a general bitmap scaler using DDA algorithm. This function scales
 * the bitmap from bottom to top.
 *
 * MiniGUI implements ScaleBitmap, FillBoxWithBitmap, FillBoxWithBitmapPart, and 
 * StretchBlt functions by using this scaler.
 *
 * \param context The context will be passed to the callbacks.
 * \param src_bmp The source BITMAP object.
 * \param dst_w The width of the destination BITMAP object.
 * \param dst_h The height of the destination BITMAP object.
 * \param cb_get_line_buff The callback to get the line buffer of the destination BITMAP object.
 * \param cb_line_scaled The callback to tell the line is scaled.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa ScaleBitmap, StretchBlt, BitmapDDAScaler2
 */
MG_EXPORT BOOL GUIAPI BitmapDDAScaler (void* context, const BITMAP* src_bmp, int dst_w, int dst_h, 
            CB_GET_LINE_BUFF cb_get_line_buff, CB_LINE_SCALED cb_line_scaled);

/**
 * \fn BOOL GUIAPI BitmapDDAScaler2 (void* context, const BITMAP* src_bmp, int dst_w, int dst_h, CB_GET_LINE_BUFF cb_get_line_buff, CB_LINE_SCALED cb_line_scaled)
 * \brief A bitmap scaler using DDA algorithm.
 *
 * This function is a general bitmap scaler using DDA algorithm. This function scales
 * the bitmap from bottom to top.
 *
 * MiniGUI implements StretchBlt functions by using this scaler.
 *
 * \param context The context will be passed to the callbacks.
 * \param src_bmp The source BITMAP object.
 * \param dst_w The width of the destination BITMAP object.
 * \param dst_h The height of the destination BITMAP object.
 * \param cb_get_line_buff The callback to get the line buffer of the destination BITMAP object.
 * \param cb_line_scaled The callback to tell the line is scaled.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa ScaleBitmap, StretchBlt, BitmapDDAScaler
 */
MG_EXPORT BOOL GUIAPI BitmapDDAScaler2 (void* context, const BITMAP* src_bmp, int dst_w, int dst_h, 
            CB_GET_LINE_BUFF cb_get_line_buff, CB_LINE_SCALED cb_line_scaled);

/**
 * \fn void GUIAPI MoveTo (HDC hdc, int x, int y)
 * \brief Moves the current zero pen position.
 *
 * This function moves the current pen position of the DC \a hdc to \a (x,y).
 * The next line or other pen drawing operation will start from the position.
 *
 * \param hdc The device context.
 * \param x x,y: The new pen position.
 * \param y x,y: The new pen position.
 *
 * \sa LineTo
 */
MG_EXPORT void GUIAPI MoveTo (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI LineTo (HDC hdc, int x, int y)
 * \brief Draws a zero line to a position.
 *
 * This function draws a line from the current zero pen position to \a (x,y),
 * and then moves the zero pen position to \a (x,y) by using the zero pen.
 * The next zero line drawing operation will start from the position.
 *
 * \param hdc The device context.
 * \param x x,y: The end point of the line.
 * \param y x,y: The end point of the line.
 *
 * \sa MoveTo, LineGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI LineTo (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1)
 * \brief Draws a rectangle.
 *
 * This function draws a rectangle by using \a MoveTo and \a LineTo.
 * The current zero pen position after calling this function will be \a (x1,y1).
 *
 * \param hdc The device context.
 * \param x0 x0,y0: The coordinates of the upper-left corner of the rectangle.
 * \param y0 x0,y0: The coordinates of the upper-left corner of the rectangle.
 * \param x1 x1,y1: The coordinates of the lower-right corner of the rectangle.
 * \param y1 x1,y1: The coordinates of the lower-right corner of the rectangle.
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1);

/** 
 * \fn void GUIAPI PolyLineTo (HDC hdc, const POINT* pts, int vertices)
 * \brief Draws a polyline.
 *
 * This function draws a polyline by using \a MoveTo and \a LineTo.
 * The current zero pen position after calling this function will be the
 * last vertex of the polyline.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polyline.
 * \param vertices The number of the vertices.
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI PolyLineTo (HDC hdc, const POINT* pts, int vertices);

/** 
 * \fn void GUIAPI SplineTo (HDC hdc, const POINT* pts)
 * \brief Draws a bezier spline.
 *
 * This function draws a bezier spline by using \a MoveTo and \a LineTo.
 * The current pen position after calling this function will be the
 * last control point of the spline. Note that the number of the 
 * control points should always be 4.
 *
 * \param hdc The device context.
 * \param pts The pointer to the control point array of the spline.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa MoveTo, LineTo
 */
MG_EXPORT void GUIAPI SplineTo (HDC hdc, const POINT* pts);

/**
 * \fn void GUIAPI Circle (HDC hdc, int sx, int sy, int r)
 * \brief Draws a circle.
 *
 * This function draws a circle with the zero pen on the DC \a hdc.
 * The center of the circle is at \a (sx, sy), and the radius is \a r.
 * 
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 *
 * \sa CircleGenerator
 */
MG_EXPORT void GUIAPI Circle (HDC hdc, int sx, int sy, int r);

/**
 * \fn void GUIAPI Ellipse (HDC hdc, int sx, int sy, int rx, int ry)
 * \brief Draws a ellipse.
 *
 * This function draws an ellipse with the zero pen on the DC \a hdc. 
 * The center of the ellipse is at \a (sx, sy), the x-coordinate radius is \a rx,
 * and the y-coordinate radius is \a ry.
 * 
 * \param hdc The device context.
 * \param sx sx,sy: The center of the ellipse.
 * \param sy sx,sy: The center of the ellipse.
 * \param rx The x-coordinate radius of the ellipse.
 * \param ry The y-coordinate radius of the ellipse.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa EllipseGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI Ellipse (HDC hdc, int sx, int sy, int rx, int ry);

/**
 * \fn void GUIAPI CircleArc (HDC hdc, int sx, int sy, int r, int ang1, int ang2)
 * \brief Draws an arc.
 *
 * This function draws an arc with the zero pen on the DC \a hdc. 
 * The center of the arc is at \a (sx, sy), the radius is \a r,
 * and the radians of start angle and end angle are \a ang1 and \a ang2 respectively.
 * 
 * \param hdc The device context.
 * \param sx sx,sy: The center of the arc.
 * \param sy sx,sy: The center of the arc.
 * \param r The radius of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position, 
 *              counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1, in 1/64ths of a degree.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa CircleArcGenerator, fixed_math_fns, dc_attrs
 */
MG_EXPORT void GUIAPI CircleArc (HDC hdc, int sx, int sy, int r, int ang1, int ang2);

/**
 * \fn void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h)
 * \brief Fills a rectangle box.
 *
 * This function fills a box with the current brush in the DC \a hdc. 
 * Note that MiniGUI only defined the color property for the brush objects so far.
 *
 * \param hdc The device context.
 * \param x x,y: The coorinates of the upper-left corner of the box.
 * \param y x,y: The coorinates of the upper-left corner of the box.
 * \param w The width of the box.
 * \param h The height of the box.
 *
 * \sa dc_attrs
 */
MG_EXPORT void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h);

/**
 * \fn void GUIAPI FillCircle (HDC hdc, int sx, int sy, int r)
 * \brief Fills a circle.
 *
 * This function fills a circle with the current brush in the DC \a hdc. 
 * Note that MiniGUI only defined the color property for the brush objects so far.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param r The radius of the circle.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa CircleGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI FillCircle (HDC hdc, int sx, int sy, int r);

/**
 * \fn void GUIAPI FillEllipse (HDC hdc, int sx, int sy, int rx, int ry)
 * \brief Fills an ellipse.
 *
 * This function fills an ellipse with the current brush in the DC \a hdc. 
 * Note that MiniGUI only defined the color property for the brush objects so far.
 *
 * \param hdc The device context.
 * \param sx sx,sy: The center of the circle.
 * \param sy sx,sy: The center of the circle.
 * \param rx The x-coordinate radius of the ellipse.
 * \param ry The y-coordinate radius of the ellipse.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa EllipseGenerator, dc_attrs
 */
MG_EXPORT void GUIAPI FillEllipse (HDC hdc, int sx, int sy, int rx, int ry);

/**
 * \fn BOOL GUIAPI FillPolygon (HDC hdc, const POINT* pts, int vertices)
 * \brief Fills an polygon.
 *
 * This function fills a polygon with the current brush in the DC \a hdc. 
 * Note that MiniGUI only defined the color property for the brush objects so far.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polygon.
 * \param vertices The number of the vertices, i.e. the size of the vertex array.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa PolygonGenerator, dc_attrs
 */
MG_EXPORT BOOL GUIAPI FillPolygon (HDC hdc, const POINT* pts, int vertices);

/**
 * \fn BOOL GUIAPI FloodFill (HDC hdc, int x, int y)
 * \brief Fills an enclosed area starting at point \a (x,y).
 *
 * This function fills an enclosed area staring at point \a (x,y), and 
 * stops when encountering a pixel different from the start point.
 *
 * \param hdc The device context.
 * \param x x,y: The start point.
 * \param y x,y: The start point.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa FloodFillGenerator, dc_attrs
 */
MG_EXPORT BOOL GUIAPI FloodFill (HDC hdc, int x, int y);

#ifdef _ADV_2DAPI

    /**
     * \addtogroup draw_adv_2d_fns Advanced 2-Dimension graphics functions.
     * @{
     */

/**
 * DC Line styles
 */
typedef enum
{
  /** 
   * The solid pen. 
   * We call the solid pen with zero wide as "zero pen".
   */
  PT_SOLID,
  /** 
   * The on/off dash pen, even segments are drawn; 
   * odd segments are not drawn.
   */
  PT_ON_OFF_DASH,
  /**
   * The double dash pen, even segments are normally. 
   * Odd segments are drawn in the brush color if 
   * the brush type is BT_SOLID, or in the brush color 
   * masked by the stipple if the brush type is BT_STIPPLED.
   */
  PT_DOUBLE_DASH,
} PenType;

/**
 * \def GetPenType (hdc)
 * \brief Gets the pen type of a DC.
 *
 * \param hdc The device context.
 * \return The pen type of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *
 * \sa PenType, GetDCAttr, SetPenType
 */
#define GetPenType(hdc)                 (PenType) GetDCAttr (hdc, DC_ATTR_PEN_TYPE)

/**
 * \def SetPenType(hdc, type)
 * \brief Sets the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param type The new pen type.
 * \return The old pen type of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *
 * \sa PenType, SetDCAttr, GetPenType
 */
#define SetPenType(hdc, type)         (PenType) SetDCAttr (hdc, DC_ATTR_PEN_TYPE, (DWORD) type)

/**
 * \fn void SetPenDashes (HDC hdc, int dash_offset, const unsigned char* dash_list, int n)
 * \brief Sets the way dashed-lines are drawn. 
 *
 * Sets the way dashed-lines are drawn. Lines will be drawn with 
 * alternating on and off segments of the lengths specified in dash_list. 
 * The manner in which the on and off segments are drawn is determined by the pen type of the DC.
 * (This can be changed with SetPenType function.)
 *
 * \param hdc The device context.
 * \param dash_offset The offset in the dash list.
 * \param dash_list The dash list.
 * \param n The lenght of the dash list.
 *
 * \return The old pen type of the DC \a hdc.
 *
 * \sa SetPenType
 */
MG_EXPORT void GUIAPI SetPenDashes (HDC hdc, int dash_offset, const unsigned char* dash_list, int n);

/**
 * DC Line cap styles
 */
typedef enum
{
  /**
   * the ends of the lines are drawn squared off 
   * and extending to the coordinates of the end point.
   */
  PT_CAP_BUTT,
  /**
   * the ends of the lines are drawn as semicircles with 
   * the diameter equal to the line width and centered at the end point.
   */
  PT_CAP_ROUND,
  /**
   * the ends of the lines are drawn squared off and 
   * extending half the width of the line beyond the end point.
   */
  PT_CAP_PROJECTING
} PTCapStyle;

/**
 * \def GetPenCapStyle(hdc)
 * \brief Gets the pen cap style of a DC.
 *
 * \param hdc The device context.
 * \return The pen cap style of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *
 * \sa PTCapStyle, GetDCAttr, SetPenCapStyle
 */
#define GetPenCapStyle(hdc)             (PTCapStyle) GetDCAttr (hdc, DC_ATTR_PEN_CAP_STYLE)

/**
 * \def SetPenCapStyle(hdc, style)
 * \brief Sets the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param style The new pen cap style.
 * \return The old pen cap style of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *
 * \sa PTCapStyle, SetDCAttr, GetPenCapStyle
 */
#define SetPenCapStyle(hdc, style)       (PTCapStyle) SetDCAttr (hdc, DC_ATTR_PEN_CAP_STYLE, (DWORD) style)

/**
 * DC Line join styles
 */
typedef enum
{
  /**
   * the sides of each line are extended to meet at an angle.
   */
  PT_JOIN_MITER,
  /**
   * the sides of the two lines are joined by a circular arc.
   */
  PT_JOIN_ROUND,
  /**
   * the sides of the two lines are joined by a straight line 
   * which makes an equal angle with each line.
   */
  PT_JOIN_BEVEL
} PTJoinStyle;

/**
 * \def GetPenJoinStyle(hdc)
 * \brief Gets the pen join style of a DC.
 *
 * \param hdc The device context.
 * \return The pen join style of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *
 * \sa PTJoinStyle, GetDCAttr, SetPenJoinStyle
 */
#define GetPenJoinStyle(hdc)           (PTJoinStyle) GetDCAttr (hdc, DC_ATTR_PEN_JOIN_STYLE)

/**
 * \def SetPenJoinStyle(hdc, style)
 * \brief Sets the pen type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param style The new pen join style.
 * \return The old pen join style of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *
 * \sa PTJoinStyle, SetDCAttr, GetPenJoinStyle
 */
#define SetPenJoinStyle(hdc, style)     (PTJoinStyle) SetDCAttr (hdc, DC_ATTR_PEN_JOIN_STYLE, (DWORD) style)

/**
 * \def GetPenWidth(hdc)
 * \brief Gets the pen width of a DC.
 *
 * \param hdc The device context.
 * \return The width of the current pen in the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *
 * \sa GetDCAttr, SetPenWidth
 */
#define GetPenWidth(hdc)                (unsigned int) GetDCAttr (hdc, DC_ATTR_PEN_WIDTH)

/**
 * \def SetPenWidth(hdc, width)
 * \brief Sets the pen width of a DC to a new width.
 *
 * \param hdc The device context.
 * \param width The new pen width.
 * \return The old pen width of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *
 * \sa SetDCAttr, GetPenWidth
 */
#define SetPenWidth(hdc, width)         (unsigned int) SetDCAttr (hdc, DC_ATTR_PEN_WIDTH, (DWORD) width)

/**
 * DC brush types.
 *  Solid:
 *  Tiled:
 *  Stippled:
 *  OpaqueStippled:
 */
typedef enum
{
  /**
   * draw with the current brush color.
   */
  BT_SOLID,
  /**
   * draw with a tiled bitmap.
   */
  BT_TILED,
  /**
   * draw using the stipple bitmap. 
   * Pixels corresponding to bits in the stipple bitmap that are set 
   * will be drawn in the brush color; pixels corresponding to bits 
   * that are not set will be left untouched.
   */
  BT_STIPPLED,
  /**
   * draw using the stipple bitmap. 
   * Pixels corresponding to bits in the stipple bitmap that are set 
   * will be drawn in the brush color; pixels corresponding to bits 
   * that are not set will be drawn with the background color.
   */
  BT_OPAQUE_STIPPLED
} BrushType;

/**
 * \def GetBrushType(hdc)
 * \brief Gets the brush type of a DC.
 *
 * \param hdc The device context.
 * \return The brush type of the DC \a hdc.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *
 * \sa BrushType, GetDCAttr, SetBrushType
 */
#define GetBrushType(hdc)               (BrushType) GetDCAttr (hdc, DC_ATTR_BRUSH_TYPE)

/**
 * \def SetBrushType(hdc, type)
 * \brief Sets the brush type of a DC to a new type.
 *
 * \param hdc The device context.
 * \param type The new brush type.
 * \return The old brush type of the DC \a hdc.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *
 * \sa BrushType, SetDCAttr, GetBrushType
 */
#define SetBrushType(hdc, type)         (BrushType) SetDCAttr (hdc, DC_ATTR_BRUSH_TYPE, (DWORD) type)

/** The stipple bitmap structure. */
typedef struct _STIPPLE
{
    /** The width of the stipple bitmap. */
    int width;
    /** The height of the stipple bitmap. */
    int height;

    /** The pitch of the stipple bitmap. */
    int pitch;

    /** The size of the stipple bitmap. */
    size_t size;

    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;
} STIPPLE;

/**
 * \fn void GUIAPI SetBrushInfo (HDC hdc, const BITMAP* tile, const STIPPLE* stipple)
 * \brief Set the tile or stipple with the DC. 
 *
 * \param hdc The device context.
 * \param tile The tile bitmap.
 * \param stipple The stipple bitmap.
 *
 * \sa SetBrushInfo
 */
MG_EXPORT void GUIAPI SetBrushInfo (HDC hdc, const BITMAP* tile, const STIPPLE* stipple);

/**
 * \fn void GUIAPI SetBrushOrigin (HDC hdc, int x, int y)
 * \brief Set the origin when using tiles or stipples with the DC. 
 *
 * Set the origin when using tiles or stipples with the DC. 
 * The tile or stipple will be aligned such that the upper left corner 
 * of the tile or stipple will coincide with this point.
 * 
 * \param hdc The device context.
 * \param x The x-coordinate of the origin.
 * \param y The y-coordinate of the origin.
 *
 * \sa SetBrushInfo
 */
MG_EXPORT void GUIAPI SetBrushOrigin (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI LineEx (HDC hdc, int x1, int y1, int x2, int y2)
 * \brief Draws a line with the current pen in the DC \a hdc.
 *
 * This function draws a line with the current pen in the DC \a hdc. 
 * The line draws from \a (x1, y1) to \a (x2, y2).
 * 
 * \param hdc The device context.
 * \param x1 x1,y1: The start point of the line.
 * \param y1 x1,y1: The start point of the line.
 * \param x2 x2,y2: The end point of the line.
 * \param y2 x2,y2: The end point of the line.
 *
 * \note Only defined for _USE_NEWGAL and _ADV_2DAPI
 *
 * \note This function does not change the pen position.
 *
 * \sa LineTo, dc_attrs
 */
MG_EXPORT void GUIAPI LineEx (HDC hdc, int x1, int y1, int x2, int y2);

/**
 * \fn void GUIAPI ArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2)
 * \brief Draws an arc with the current pen in a DC.
 *
 * This function draws an arc with the current pen in the DC \a hdc. 
 * The coordinates of the upper-left corner of the bounding rectanglecenter of 
 * the arc is \a (x, y), the bounding box of the arc is \a width wide
 * and \a height high, and the degree of start angle and end angle are \a ang1 
 * and \a ang2 respectively.
 * 
 * \param hdc The device context.
 * \param x the x coordinate of the left edge of the bounding rectangle.
 * \param y the y coordinate of the left edge of the bounding rectangle.
 * \param width The width of the bounding box of the arc.
 * \param height The height of the bounding box of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position, 
 *              counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1, in 1/64ths of a degree.
 *
 * \note Only defined for _USE_NEWGAL
 *
 * \sa dc_attrs
 */
MG_EXPORT void GUIAPI ArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2);

/**
 * \fn void GUIAPI FillArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2)
 * \brief Fills an arc with the current brush in a DC.
 *
 * This function fills an arc with the current brush in the DC \a hdc. 
 * The coordinates of the upper-left corner of the bounding rectanglecenter of 
 * the arc is \a (x, y), the bounding box of the arc is \a width wide
 * and \a height high, and the degree of start angle and end angle are \a ang1 
 * and \a ang2 respectively.
 *
 * \param hdc The device context.
 * \param x the x coordinate of the left edge of the bounding rectangle.
 * \param y the y coordinate of the left edge of the bounding rectangle.
 * \param width The width of the bounding box of the arc.
 * \param height The height of the bounding box of the arc.
 * \param ang1 The start angle of the arc, relative to the 3 o'clock position, 
 *              counter-clockwise, in 1/64ths of a degree.
 * \param ang2 The end angle of the arc, relative to angle1, in 1/64ths of a degree.
 *
 * \sa ArcEx, dc_attrs
 */
MG_EXPORT void GUIAPI FillArcEx (HDC hdc, int x, int y, int width, int height, int ang1, int ang2);

/**
 * \fn void GUIAPI PolyLineEx (HDC hdc, const POINT* pts, int nr_pts)
 * \brief Draws a polyline with the current pen in a DC.
 *
 * This function draws a polyline with the current pen in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param pts The pointer to the vertex array of the polyline.
 * \param nr_pts The number of the vertices.
 *
 * \sa LineEx 
 */
MG_EXPORT void GUIAPI PolyLineEx (HDC hdc, const POINT* pts, int nr_pts);

/**
 * Arc structure.
 */
typedef struct _ARC
{
    /** the x coordinate of the left edge of the bounding rectangle. */
    int x;
    /** the y coordinate of the left edge of the bounding rectangle. */
    int y;

    /** the width of the bounding box of the arc. */
    int width;
    /** the height of the bounding box of the arc. */
    int height;

    /**
     * The start angle of the arc, relative to the 3 o'clock position, 
     * counter-clockwise, in 1/64ths of a degree.
     */
    int angle1;
    /** 
     * The end angle of the arc, relative to angle1, in 1/64ths of a degree.
     */
    int angle2;
} ARC;

/**
 * \fn void GUIAPI PolyArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
 * \brief Draws a polyarc with the current pen in a DC.
 *
 * This function draws a polyarc with the current pen in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param arcs The pointer to an ARC array which defines the arcs.
 * \param nr_arcs The number of the arcs.
 *
 * \sa ArcEx
 */
MG_EXPORT void GUIAPI PolyArcEx (HDC hdc, const ARC* arcs, int nr_arcs);

/**
 * \fn void GUIAPI PolyFillArcEx (HDC hdc, const ARC* arcs, int nr_arcs)
 * \brief Fill a polyarc with the current brush in a DC.
 *
 * This function fill a polyarc with the current brush in the DC \a hdc.
 *
 * \param hdc The device context.
 * \param arcs The pointer to an ARC array which defines the arcs.
 * \param nr_arcs The number of the arcs.
 *
 * \sa ArcEx
 */
MG_EXPORT void GUIAPI PolyFillArcEx (HDC hdc, const ARC* arcs, int nr_arcs);

    /**
     * @} end of draw_adv_2d_fns
     */
#endif

#else

/* Old GDI drawing functions */
MG_EXPORT void GUIAPI MoveTo (HDC hdc, int x, int y);
MG_EXPORT void GUIAPI LineTo (HDC hdc, int x, int y);
MG_EXPORT void GUIAPI Rectangle (HDC hdc, int x0, int y0, int x1, int y1);
MG_EXPORT void GUIAPI PolyLineTo (HDC hdc, const POINT* pts, int vertices);
MG_EXPORT void GUIAPI Circle (HDC hdc, int sx, int sy, int r);
MG_EXPORT void GUIAPI FillBox (HDC hdc, int x, int y, int w, int h);

#endif

    /** @} end of draw_fns */

    /**
     * \defgroup map_fns Mapping Operations
     *
     * The mapping mode defines the unit of measure used to transform 
     * page-space units into device-space units, and also defines 
     * the orientation of the device's x and y axes. 
     *
     * So far, MiniGUI support only two mapping mode: 
     *
     *      - MM_TEXT\n
     *        Each logical unit is mapped to on device pixel. 
     *        Positive x is to the right; positive y is down.
     *      - MM_ANISOTROPIC\n
     *        Logical units are mapped to arbitrary units with arbitrarily scaled axes; 
     *        Use \a SetWindowExt and \a SetViewportExt functions to specify the units, 
     *        orientation, and scaling required.
     *
     * The following formula shows the math involved in converting a point 
     * from page space to device space:
     *
     * \code
     *      Dx = ((Lx - WOx) * VEx / WEx) + VOx
     * \endcode
     *
     * The following variables are involved:
     *
     *      - Dx    x value in device units
     *      - Lx    x value in logical units (also known as page space units)
     *      - WO    window x origin
     *      - VO    viewport x origin
     *      - WE    window x-extent
     *      - VE    viewport x-extent
     *
     * The same equation with y replacing x transforms the y component of a point.
     * The formula first offsets the point from its coordinate origin. 
     * This value, no longer biased by the  origin, is then scaled into 
     * the destination coordinate system by the ratio of the extents. 
     * Finally, the scaled value is offset by the destination origin to 
     * its final mapping.
     *
     * @{
     */

#define MM_TEXT               0
#define MM_ANISOTROPIC        1

#ifdef _USE_NEWGAL

/**
 * \def GetMapMode(hdc)
 * \brief Retrives the current mapping mode of a DC.
 *
 * This function retrives the current mapping mode of the DC \a hdc. 
 *
 * So far, MiniGUI support two mapping modes: MM_TEXT and MM_ANISOTROPIC.
 *
 * \param hdc The device context.
 * \return The current mapping mode, can be either \a MM_TEXT or \a MM_ANISOTROPIC.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetMapMode is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetWindowExt, SetViewportExt, SetMapMode
 */
#define GetMapMode(hdc)         (int)GetDCAttr (hdc, DC_ATTR_MAP_MODE)

/**
 * \def SetMapMode(hdc, mapmode)
 * \brief Sets the mapping mode of a display context.
 *
 * This function sets the mapping mode of the specified display context \a hdc.
 *
 * So far, MiniGUI support two mapping modes: MM_TEXT and MM_ANISOTROPIC.
 *
 * \param hdc The device context.
 * \param mapmode The new mapping mode, should be either \a MM_TEXT or \a MM_ANISOTROPIC.
 * \return The old mapping mode, either \a MM_TEXT or \a MM_ANISOTROPIC.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetMapMode is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetWindowExt, SetViewportExt, SetMapMode
 */
#define SetMapMode(hdc, mapmode)    (int)SetDCAttr (hdc, DC_ATTR_MAP_MODE, (DWORD)mapmode)

#else

MG_EXPORT int GUIAPI GetMapMode (HDC hdc);
MG_EXPORT void GUIAPI SetMapMode (HDC hdc, int mapmode);

#endif /* _USE_NEWGAL */

#ifdef _USE_NEWGAL

/**
 * \fn void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt)
 * \brief Retrives mapping parameters of a device context.
 *
 * This function retrives mapping paramters of the specified device context \a hdc
 * when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrive, can be one of the following values:
 *
 *      - DC_LCS_VORG\n
 *        Retrives the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Retrives the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Retrives the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Retrives the x-extents and y-extents of the window.
 *        
 * \param pt The coordinates or extents will be returned through this buffer.
 * 
 * \note Only defined for _USE_NEWGAL
 *
 * \sa GetMapMode, SetMapMode, SetDCLCS
 */
MG_EXPORT void GUIAPI GetDCLCS (HDC hdc, int which, POINT* pt);

/**
 * \fn void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt)
 * \brief Sets mapping parameters of a device context.
 *
 * This function sets mapping paramters of the specified device context \a hdc
 * when the mapping mode is not \a MM_TEXT.
 *
 * \param hdc The device context.
 * \param which Which parameter you want to retrive, can be one of the following values:
 *
 *      - DC_LCS_VORG\n
 *        Sets the x-coordinates and y-coordinates of the viewport origin.
 *      - DC_LCS_VEXT\n
 *        Sets the x-extents and y-extents of the current viewport.
 *      - DC_LCS_WORG\n
 *        Sets the x-coordinates and y-coordinates of the window origin.
 *      - DC_LCS_WEXT\n
 *        Sets the x-extents and y-extents of the window.
 *        
 * \param pt The coordinates or extents will be set.
 * 
 * \note Only defined for _USE_NEWGAL
 *
 * \sa GetMapMode, SetMapMode, GetDCLCS
 */
MG_EXPORT void GUIAPI SetDCLCS (HDC hdc, int which, const POINT* pt);

#define DC_LCS_VORG     0
#define DC_LCS_VEXT     1
#define DC_LCS_WORG     2
#define DC_LCS_WEXT     3
#define NR_DC_LCS_PTS   4

/**
 * \def GetViewportOrg(hdc, pPt)
 * \brief Retrieves the x-coordinates and y-coordinates of the viewport origin for a device context.
 *
 * This function retrives the x-coordinates and y-coordinates of 
 * the viewport origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport origin will be returned through this buffer.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetViewportOrg is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, SetViewportOrg
 */
#define GetViewportOrg(hdc, pPt)    GetDCLCS(hdc, DC_LCS_VORG, pPt)

/**
 * \def GetViewportExt(hdc, pPt)
 * \brief Retrieves the x-extents and y-extents of the current viewport for a device context.
 *
 * This function retrives the x-extents and y-extens of the current viewport of 
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport extents will be returned through this buffer.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetViewportExt is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, SetViewportExt
 */
#define GetViewportExt(hdc, pPt)    GetDCLCS(hdc, DC_LCS_VEXT, pPt)

/**
 * \def GetWindowOrg(hdc, pPt)
 * \brief Retrieves the x-coordinates and y-coordinates of the window for a device context.
 *
 * This function retrives the x-coordinates and y-coordinates of 
 * the window origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window origin will be returned through this buffer.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetWindowOrg is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, SetWindowOrg
 */
#define GetWindowOrg(hdc, pPt)      GetDCLCS(hdc, DC_LCS_WORG, pPt)

/**
 * \def GetWindowExt(hdc, pPt)
 * \brief Retrieves the x-extents and y-extents of the current window for a device context.
 *
 * This function retrives the x-extents and y-extens of the current window of 
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window extents will be returned through this buffer.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a GetWindowExt is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, SetWindowExt
 */
#define GetWindowExt(hdc, pPt)      GetDCLCS(hdc, DC_LCS_WEXT, pPt)

/**
 * \def SetViewportOrg(hdc, pPt)
 * \brief Sets the x-coordinates and y-coordinates of the viewport origin for a device context.
 *
 * This function sets the x-coordinates and y-coordinates of 
 * the viewport origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport origin will be set.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetViewportOrg is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, GetViewportOrg
 */
#define SetViewportOrg(hdc, pPt)    SetDCLCS(hdc, DC_LCS_VORG, pPt)

/**
 * \def SetViewportExt(hdc, pPt)
 * \brief Sets the x-extents and y-extents of the current viewport for a device context.
 *
 * This function sets the x-extents and y-extens of the current viewport of 
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The viewport extents will be set.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetViewportExt is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, GetViewportExt
 */
#define SetViewportExt(hdc, pPt)    SetDCLCS(hdc, DC_LCS_VEXT, pPt)

/**
 * \def SetWindowOrg(hdc, pPt)
 * \brief Sets the x-coordinates and y-coordinates of the window for a device context.
 *
 * This function sets the x-coordinates and y-coordinates of 
 * the window origin of the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window origin will be set.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetWindowOrg is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, GetWindowOrg
 */
#define SetWindowOrg(hdc, pPt)      SetDCLCS(hdc, DC_LCS_WORG, pPt)

/**
 * \def SetWindowExt(hdc, pPt)
 * \brief Sets the x-extents and y-extents of the current window for a device context.
 *
 * This function sets the x-extents and y-extens of the current window of 
 * the specified device context \a hdc.
 *
 * \param hdc The device context.
 * \param pPt The window extents will be set.
 *
 * \note Defined as a macro calling \a GetDCLCS for _USE_NEWGAL. 
 *       If _USE_NEWGAL is not defined, \a SetWindowExt is defined as 
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetMapMode, GetWindowExt
 */
#define SetWindowExt(hdc, pPt)      SetDCLCS(hdc, DC_LCS_WEXT, pPt)

#else

MG_EXPORT void GUIAPI GetViewportExt (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI GetViewportOrg (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI GetWindowExt (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI GetWindowOrg (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI SetViewportExt (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI SetViewportOrg (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI SetWindowExt (HDC hdc, POINT* pPt);
MG_EXPORT void GUIAPI SetWindowOrg (HDC hdc, POINT* pPt);

#endif /* _USE_NEWGAL */

/**
 * \fn void GUIAPI DPtoLP (HDC hdc, POINT* pPt)
 * \brief Converts device coordinates into logical coordinates.
 * 
 * This function converts device coordinates into logical coordinates 
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context, 
 * the settings of the origins and extents for the window and viewport. 
 * The x-coordinate and y-coordinate contained in struct \a pPt will be 
 * transformed.
 * 
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa LPtoDP
 */
MG_EXPORT void GUIAPI DPtoLP (HDC hdc, POINT* pPt);

/**
 * \fn void GUIAPI LPtoDP (HDC hdc, POINT* pPt)
 * \brief Converts logical coordinates into device coordinates.
 * 
 * This function converts logical coordinates into device coordinates 
 * in the device context \a hdc.
 *
 * The conversion depends on the mapping mode of the display context, 
 * the settings of the origins and extents for the window and viewport.
 * The x-coordinate and y-coordinate contained in struct \a pPt will be 
 * transformed.
 * 
 * \param hdc The device context.
 * \param pPt The coordinates to be converted, and the transformed coordinates
 *        will be contained in this buffer after the function returns.
 *
 * \sa DPtoLP
 */
MG_EXPORT void GUIAPI LPtoDP (HDC hdc, POINT* pPt);

    /** @} end of map_fns */

    /**
     * \defgroup clip_fns Clipping operations
     * @{
     */

/**
 * \fn void GUIAPI ExcludeClipRect (HDC hdc, const RECT* prc)
 * \brief Excludes the specified rectangle from the current visible region of a DC.
 *
 * This function excludes the specified rect \a prc from the current visible region
 * of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc The rectangle to be excluded.
 *
 * \sa IncludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI ExcludeClipRect (HDC hdc, const RECT* prc);

/**
 * \fn void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc)
 * \brief Includes the specified rectangle to the current visible region of a DC.
 *
 * This function includes the specified rectangle \a prc to the current visible region
 * of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc The rectangle to be included.
 *
 * \sa ExcludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI IncludeClipRect (HDC hdc, const RECT* prc);

/**
 * \fn BOOL GUIAPI PtVisible (HDC hdc, int x, int y)
 * \brief Checks whether a point is visible.
 * 
 * This function checks whether the point specified by \a (x,y) is visible, i.e.
 * it is within the current visible clipping region of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param x x,y: The coordinates of the point.
 * \param y x,y: The coordinates of the point.
 * \return TRUE for visible, else for not.
 *
 * \sa RectVisible, region_fns
 */
MG_EXPORT BOOL GUIAPI PtVisible (HDC hdc, int x, int y);

/**
 * \fn void GUIAPI ClipRectIntersect (HDC hdc, const RECT* prc)
 * \brief Intersects the specified rectangle with the visible region of the DC.
 *
 * This function intersects the specified rectangle \a prc with the visible region
 * of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param prc Pointer to the rectangle.
 * 
 * \sa IncludeClipRect, ExcludeClipRect, region_fns
 */
MG_EXPORT void GUIAPI ClipRectIntersect (HDC hdc, const RECT* prc);

/**
 * \fn void GUIAPI SelectClipRect (HDC hdc, const RECT* prc)
 * \brief Sets the visible region of a DC to be a rectangle. 
 *
 * This function sets the visible region of the device context \a hdc 
 * to the rectangle pointed to by \a prc.
 *
 * \param hdc The device context.
 * \param prc Pointer to the rectangle.
 * 
 * \sa SelectClipRegion, region_fns
 */
MG_EXPORT void GUIAPI SelectClipRect (HDC hdc, const RECT* prc);

/**
 * \fn void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn)
 * \brief Sets the visible region of a DC to be a region. 
 *
 * This function sets the visible region of the device context \a hdc 
 * to the region pointed to by \a pRgn.
 *
 * \param hdc The device context.
 * \param pRgn Pointer to the region.
 * 
 * \sa SelectClipRect, region_fns
 */
MG_EXPORT void GUIAPI SelectClipRegion (HDC hdc, const CLIPRGN* pRgn);

/**
 * \fn void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect)
 * \brief Retrives the bounding rectangle of the current visible region of a DC.
 *
 * This function retrives the bounding rectangle of the current visible region 
 * of the specified device context \a hdc, and returned through \a pRect.
 *
 * \param hdc The device context.
 * \param pRect The bounding rectangle will be returned through this buffer.
 *
 * \sa region_fns
 */
MG_EXPORT void GUIAPI GetBoundsRect (HDC hdc, RECT* pRect);

/**
 * \fn BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect)
 * \brief Checks whether the specified rectangle is visible.
 *
 * This function checks whether the rectangle pointed to by \a pRect is visible, i.e.
 * it is intersected with the current visible region of the device context \a hdc.
 *
 * \param hdc The device context.
 * \param pRect Pointer to the rectangle.
 *
 * \sa PtVisible, region_fns
 *
 * Example:
 *
 * \include rectvisible.c
 */
MG_EXPORT BOOL GUIAPI RectVisible (HDC hdc, const RECT* pRect);

    /** @} end of clip_fns */

    /**
     * \defgroup bmp_fns BITMAP and blitting operations
     *
     * Example:
     *
     * \include blitting.c
     *
     * @{
     */

#ifdef _USE_NEWGAL

/**
 * \fn BOOL GUIAPI GetBitmapFromDC (HDC hdc, int x, int y, int w, int h, BITMAP* bmp)
 * \brief Gets image box on a DC and saves it into a BITMAP object. 
 *
 * This function gets image box on the specified device context \a hdc,
 * and saves the image bits into the BITMAP object pointed to by \a bmp. 
 * The image box begins at \a (x,y), and is \a w wide and \a h high. 
 * You must make sure that \a bmp->bits is big enough to store the image.
 *
 * \param hdc The device context.
 * \param x The x coordinate in pixels of upper-left corner of the image box.
 * \param y The y coordinate in pixels of upper-left corner of the image box.
 * \param w The width of the image box.
 * \param h The height of the image box.
 * \param bmp The pointer to the BITMAP object.
 *
 * \note Only defined for _USE_NEWGAL. If _USE_NEWGAL not defined, there is
 * will be an analogous function called \a SaveScreenBox:
 *
 * \code
 * void GUIAPI SaveScreenBox (int x, int y, int w, int h, void* vbuf)
 * \endcode
 *
 * \sa FillBoxWithBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI GetBitmapFromDC (HDC hdc, int x, int y, int w, int h, BITMAP* bmp);

#else

MG_EXPORT void GUIAPI SaveScreenBox (int x, int y, int w, int h, void* vbuf);
MG_EXPORT void* GUIAPI SaveCoveredScreenBox (int x, int y, int w, int h);
MG_EXPORT void GUIAPI PutSavedBoxOnScreen (int x, int y, int w, int h, void* vbuf);
MG_EXPORT void GUIAPI PutSavedBoxOnDC (HDC hdc, int x, int y, int w, int h, void* vbuf);

MG_EXPORT void GUIAPI ScreenCopy (int sx, int sy, HDC hdc, int dx, int dy);

#endif /* _USE_NEWGAL */

/**
 * \fn BOOL GUIAPI FillBoxWithBitmap (HDC hdc, int x, int y, int w, int h, const BITMAP *bmp)
 * \brief Fills a box with a BITMAP object.
 *
 * This function fills a box with a BITMAP object pointed to by \a bmp. 
 * \a (x,y) is the upper-left corner of the box, and \a w, \a h are 
 * the width and the height of the box respectively. 
 *
 * This function will scale the bitmap when necessary; that is, when 
 * the width or the height of the box is not equal to the with or 
 * the height of the BITMAP object.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box. Can be zero, means the width
 *        or the height will be equal to the width or the height of the BITMAP object.
 * \param h The height of the box. Can be zero, means the width
 *        or the height will be equal to the width or the height of the BITMAP object.
 * \param bmp The pointer to the BITMAP object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note You can specify the alpha value or the color key of the BITMAP object if 
 * _USE_NEWGAL defined, and the current raster operation was set by \a SetRasterOperation 
 * for the DC will override the alpha value of color key if ROP is not ROP_SET.
 * If _USE_NEWGAL is not defined, this function will not return anything, and
 * you can not specify the alpha value and the color key of the BITMAP object.
 *
 * \sa FillBoxWithBitmapPart, GetBitmapFromDC, bmp_load_fns
 */
MG_EXPORT BOOL GUIAPI FillBoxWithBitmap (HDC hdc, int x, int y, int w, int h,
                const BITMAP *bmp);

/**
 * \fn BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc, int x, int y, int w, int h, int bw, int bh, const BITMAP* bmp, int xo, int yo)
 * \brief Fills a box with a part of a bitmap oject.
 *
 * This function fills a box with a part of a bitmap object pointed to by \a bmp. 
 * \a (x,y) is the upper-left corner of the box, and \a w, \a h are the width and 
 * the height of the box respectively. \a (xo, yo) is the start position of the 
 * part box in the bitmap relative to upper-left corner of the bitmap, 
 * and \a bw, \a bh are the width and the height of the full bitmap expected. 
 *
 * If \a bw or \a bh is less than or equal to zero, this function will use 
 * the original width and height of the bitmap, else it will scale 
 * the BITMAP object when necessary; that is, when the width or the height of 
 * the box is not equal to the with or the height of the BITMAP object.
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box. 
 * \param h The height of the box. 
 * \param bw The width of the full bitmap expected. 
 *        Can be zero, means the width or the height will be equal to 
 *        the width or the height of the BITMAP object.
 * \param bh The height of the full bitmap expected. 
 *        Can be zero, means the width or the height will be equal to 
 *        the width or the height of the BITMAP object.
 * \param xo xo,yo: The start position of the part box in the bitmap 
 *        relative to upper-left corner of the BITMAP object.
 * \param yo xo,yo: The start position of the part box in the bitmap 
 *        relative to upper-left corner of the BITMAP object.
 * \param bmp The pointer to the BITMAP object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note You can specify the alpha value or the color key of the BITMAP object if 
 *       _USE_NEWGAL defined, and the current raster operation was set by 
 *       \a SetRasterOperation for the DC will override the alpha value or 
 *       the color key if ROP is not ROP_SET. If _USE_NEWGAL not defined, 
 *       this function will not return anything, and you can not specify 
 *       the alpha value and the color key of the BITMAP object.
 *
 * \sa FillBoxWithBitmap, GetBitmapFromDC, bmp_struct
 */
MG_EXPORT BOOL GUIAPI FillBoxWithBitmapPart (HDC hdc, int x, int y, int w, int h,
                int bw, int bh, const BITMAP* bmp, int xo, int yo);

/**
 * \fn void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh, HDC hddc, int dx, int dy, DWORD dwRop)
 * \brief Performs a bit-block transfer from a device context into another device context.
 *
 * This function performs a bit-block transfer of the color data cooresponding to 
 * a rectangle of pixels from the specified source device context \a hsdc into 
 * a destination device context \a hddc. \a (sx,sy,sw,sh) specifies the rectangle 
 * in the source DC, and \a (dx,dy) specifies the position of the rectangle 
 * in the destination DC. Note that the size of the two rectangles are identical.
 *
 * Note that all coordinates should be in the device space.
 *
 * \param hsdc The source device context.
 * \param sx The x coordinate of the upper-left corner of the rectangle in the source DC.
 * \param sy The y coordinate of the upper-left corner of the rectangle in the source DC.
 * \param sw The width of the source rectangle.
 * \param sh The height of the source rectangle.
 * \param hddc The destination device context \a hddc.
 * \param dx The x coordinate of the upper-left corner of the rectangle in the destination DC.
 * \param dy The y coordinate of the upper-left corner of the rectangle in the destination DC.
 * \param dwRop The raster operation, currently ignored.
 *
 * \note If _USE_NEWGAL defined, the alpha and color key settings of the source DC 
 *       will come into play.
 *
 * \sa StretchBlt, SetMemDCAlpha, SetMemDCColorKey
 */
MG_EXPORT void GUIAPI BitBlt (HDC hsdc, int sx, int sy, int sw, int sh, 
                HDC hddc, int dx, int dy, DWORD dwRop);

/**
 * \fn void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh, HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop)
 * \brief Copies a bitmap from a source rectangle into a destination rectangle, streches the bitmap if necessary.
 *
 * This function copies a bitmap from a source rectangle into a destination rectangle, 
 * streching or compressing the bitmap to fit the dimension of the destination rectangle, 
 * if necessary. This function is similar with \sa BitBlt function except the former scaling the bitmap. 
 * \a (dw,dh) specifies the size of the destination rectangle.
 * 
 * \param hsdc The source device context.
 * \param sx The x coordinate of the upper-left corner of the rectangle in the source DC.
 * \param sy The y coordinate of the upper-left corner of the rectangle in the source DC.
 * \param sw The width of the source rectangle.
 * \param sh The height of the source rectangle.
 * \param hddc The destination device context \a hddc.
 * \param dx The x coordinate of the upper-left corner of the rectangle in the destination DC.
 * \param dy The y coordinate of the upper-left corner of the rectangle in the destination DC.
 * \param dw The width of the destination rectangle.
 * \param dh The height of the destination rectangle.
 * \param dwRop The raster operation, currently ignored.
 *
 * \note The source rect should be contained in the device space entirely,
 * and all coordinates should be in the device space.
 *
 * \note The alpha and color key settings of the source DC will not come into play.
 * 
 * \sa BitBlt, SetMemDCAlpha, SetMemDCColorKey
 */
MG_EXPORT void GUIAPI StretchBlt (HDC hsdc, int sx, int sy, int sw, int sh, 
                HDC hddc, int dx, int dy, int dw, int dh, DWORD dwRop);

/**
 * \fn BOOL GUIAPI ScaleBitmap (BITMAP* dst, const BITMAP* src)
 * \brief Scales a BITMAP object into another BITMAP object by using DDA algorithm.
 *
 * This function scales a BITMAP object \a src into another BITMAO object \a dst
 * by using DDA algorithm. The source rectangle and the destination rectangle 
 * both are defined in the BITMAP objects.
 *
 * \param dst The destination BITMAP object.
 * \param src The srouce BITMAP object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa FillBoxWithBitmap, StretchBlt, bmp_struct
 */
MG_EXPORT BOOL GUIAPI ScaleBitmap (BITMAP* dst, const BITMAP* src);

/**
 * \fn gal_pixel GUIAPI GetPixelInBitmap (const BITMAP* bmp, int x, int y)
 * \brief Returns the pixel value in a BITMAP object.
 *
 * This function returns the pixel value at the position \a (x,y) in 
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to 
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to 
 *        the upper-left corner of the bitmap.
 * \return The pixel value, if the position is out of the bitmap,
 *         zero returned.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa SetPixelInBitmap, bmp_struct
 */
MG_EXPORT gal_pixel GUIAPI GetPixelInBitmap (const BITMAP* bmp, int x, int y);

/**
 * \fn BOOL GUIAPI SetPixelInBitmap (const BITMAP* bmp, int x, int y, gal_pixel pixel)
 * \brief Sets pixel value in a BITMAP object.
 *
 * This function sets the pixel value at the position \a (x,y) in 
 * the BITMAP object \a bmp.
 *
 * \param bmp The BITMAP object.
 * \param x x,y: The position of the pixel in the bitmap relative to 
 *        the upper-left corner of the bitmap.
 * \param y x,y: The position of the pixel in the bitmap relative to 
 *        the upper-left corner of the bitmap.
 * \param pixel The pixel value.
 * \return TRUE on success. If the position is out of the bitmap, FALSE returned.
 *
 * \note Only defined for _USE_NEWGAL.
 *
 * \sa GetPixelInBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI SetPixelInBitmap (const BITMAP* bmp, int x, int y, gal_pixel pixel);

#ifdef _MISC_SAVESCREEN

/**
 * \fn BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename)
 * \brief Saves content of a main window to a file.
 *
 * This function saves the content of the main window \a hWnd to the iamge file \a filename.
 * MiniGUI uses the extension name of the file to determine the format of the image file.
 *
 * \param hWnd Handle to the main window.
 * \param filename The name of the iamge file.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Only defined for _MISC_SAVESCREEN.
 *
 * \sa bmp_load_fns
 */
MG_EXPORT BOOL GUIAPI SaveMainWindowContent (HWND hWnd, const char* filename);
#endif

    /** @} end of bmp_fns */

    /**
     * \defgroup icon_fns Icon operations
     * @{
     */

/**
 * \fn HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename, int which)
 * \brief Loads an icon from a Windows ICO file.
 *
 * This function loads an icon from a Windows ICO file named \a filename 
 * and creates an icon object. This function can load mono- and 16-color icons. 
 * Some Windows ICO file contain two icons in different sizes. You can tell 
 * this function to load which icon though \a which, 0 for the first icon, 
 * and 1 for the second icon. Generally, the later icon is the larger icon.
 *
 * \param hdc The device context.
 * \param filename The file name of the ICO file.
 * \param which Tell the function to load which icon.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa CreateIcon
 */
MG_EXPORT HICON GUIAPI LoadIconFromFile (HDC hdc, const char* filename, int which);

/**
 * \fn HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which)
 * \brief Loads an icon from a memory area.
 *
 * This function loads an icon from a memroy area pointed to by \a area.
 * The memory area has the same layout as the M$ Windows ICO file.
 *
 * This function can load mono- and 16-color icons. 
 * Some Windows ICO file contain two icons in different sizes. You can tell 
 * this function to load which icon though \a which, 0 for the first icon, 
 * and 1 for the second icon. Generally, the later icon is the larger icon.
 *
 * \param hdc The device context.
 * \param area The pointer to the memory area.
 * \param which Tell the function to load which icon.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa CreateIcon
 */
MG_EXPORT HICON GUIAPI LoadIconFromMem (HDC hdc, const void* area, int which);

/**
 * \fn HICON GUIAPI CreateIcon (HDC hdc, int w, int h, const BYTE* AndBits, const BYTE* XorBits, int colornum)
 * \brief Creates an icon object from the memory.
 *
 * This function creates an icon from memory data rather than icon file. 
 * \a w and \a h are the width and the height of the icon respectively. 
 * \a pANDBits and \a pXORBits are AND bitmask and XOR bitmask of the icon. 
 * MiniGUI currently support mono-color cursor and 16-color icon, \a colornum 
 * specifies the cursor's color depth. For mono-color, it should be 1, and for
 * 16-color cursor, it should be 4.
 *
 * \param hdc The device context.
 * \param w The width of the icon.
 * \param h The height of the icon.
 * \param AndBits The pointer to the AND bits of the icon.
 * \param XorBits The pointer to the XOR bits of the icon.
 * \param colornum The bit-per-pixel of XOR bits.
 * \return The handle to the icon object, zero means error occurred.
 *
 * \sa LoadIconFromFile
 */
MG_EXPORT HICON GUIAPI CreateIcon (HDC hdc, int w, int h, 
                        const BYTE* AndBits, const BYTE* XorBits, int colornum);

/**
 * \fn BOOL GUIAPI DestroyIcon (HICON hicon)
 * \brief Destroys an icon object.
 *
 * This function destroys the icon object \a hicon.
 *
 * \param hicon The icon object.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateIcon, LoadIconFromFile
 */
MG_EXPORT BOOL GUIAPI DestroyIcon (HICON hicon);

/**
 * \fn BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h)
 * \brief Gets the size of an icon object.
 *
 * This function gets the size of the icon object \a hicon.
 *
 * \param hicon The icon object.
 * \param w The width of the icon will be returned throught this buffer.
 * \param h The height of the icon will be returned throught this buffer.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa CreateIcon, LoadIconFromFile
 */
MG_EXPORT BOOL GUIAPI GetIconSize (HICON hicon, int* w, int* h);

/**
 * \fn void GUIAPI DrawIcon (HDC hdc, int x, int y, int w, int h, HICON hicon)
 * \brief Draws an icon into a box.
 *
 * This function draws an icon object \a hicon into a box specified by \a (x,y,w,h).
 *
 * \param hdc The device context.
 * \param x The x coordinate of the upper-left corner of the box.
 * \param y The y coordinate of the upper-left corner of the box.
 * \param w The width of the box.
 * \param h The height of the box.
 * \param hicon The icon object.
 *
 * \sa CreateIcon, LoadIconFromFile
 */
MG_EXPORT void GUIAPI DrawIcon (HDC hdc, int x, int y, int w, int h, HICON hicon);

    /** @} end of icon_fns */

    /**
     * \defgroup rect_fns Rectangle operations
     * @{
     */

#ifdef _USE_NEWGAL

/**
 * \fn void SetRect (RECT* prc, int left, int top, int right, int bottom)
 * \brief Sets a rectangle.
 *
 * This function sets the rectangle with specified values.
 *
 * \param prc The pointer to the rectangle.
 * \param left The x coordinate of the upper-left corner of the rectangle.
 * \param top The y coordinate of the upper-left corner of the rectangle.
 * \param right The x coordinate of the lower-right corner of the rectangle.
 * \param bottom The y coordinate of the lower-right corner of the rectangle.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa SetRectEmpty
 */
static inline void SetRect (RECT* prc, int left, int top, int right, int bottom)
{
    (prc)->left = left; (prc)->top = top; (prc)->right = right; (prc)->bottom = bottom;
}

/**
 * \fn void SetRectEmpty (RECT* prc)
 * \brief Empties a rectangle.
 *
 * This function empties the rectangle pointed to by \a prc. 
 * An empty rectangle in MiniGUI is a rectangle whose width and height both are zero. 
 * This function will sets all coordinates of the rectangle to be zero.
 *
 * \param prc The pointer to the rectangle.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa SetRect
 */
static inline void SetRectEmpty (RECT* prc)
{
    (prc)->left = (prc)->top = (prc)->right = (prc)->bottom = 0;
}

/**
 * \fn void CopyRect (RECT* pdrc, const RECT* psrc)
 * \brief Copies one rectangle to another.
 *
 * This function copies the coordinates of the source rectangle 
 * pointed to by \a psrc to the destination rectangle pointed to by \a pdrc.
 *
 * \param pdrc The pointer to the destination rectangle.
 * \param psrc The pointer to the source rectangle.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa SetRect
 */
static inline void CopyRect (RECT* pdrc, const RECT* psrc)
{
    (pdrc)->left = (psrc)->left; (pdrc)->top = (psrc)->top;
    (pdrc)->right = (psrc)->right; (pdrc)->bottom = (psrc)->bottom;
}

/**
 * \fn void OffsetRect (RECT* prc, int x, int y)
 * \brief Moves a rectangle by offsets.
 *
 * This function moves the specified rectangle by the specified offsets. 
 * \a x and \a y specify the amount to move the rectangle left/right or up/down 
 * respectively. \a x must be a negative value to move the rectangle to
 * the left, and \a y must be a negative value to move the rectangle up.
 *
 * \param prc The pointer to the rectangle.
 * \param x The x offset.
 * \param y The y offset.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa InflateRect
 */
static inline void OffsetRect (RECT* prc, int x, int y)
{
    (prc)->left += x; (prc)->top += y; (prc)->right += x; (prc)->bottom += y;
}

/**
 * \fn void InflateRect (RECT* prc, int cx, int cy)
 * \brief Increases or decreases the width and height of an rectangle.
 *
 * This function increases or decreases the width and height of 
 * the specified rectangle \a prc. This function adds \a cx units 
 * to the left and right ends of the rectangle and \a cy units to the 
 * top and bottom. the \a cx and \a cy are signed values; positive values 
 * increases the width and height, and negative values decreases them.
 *
 * \param prc The pointer to the rectangle.
 * \param cx The inflating x value.
 * \param cy The inflating y value.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa InflateRectToPt
 */
static inline void InflateRect (RECT* prc, int cx, int cy)
{
    (prc)->left -= cx; (prc)->top -= cy; (prc)->right += cx; (prc)->bottom += cy;
}

/**
 * \fn void InflateRectToPt (RECT* prc, int x, int y)
 * \brief Inflates a rectangle to contain a point.
 *
 * This function inflates the rectangle \a prc to contain the specified point \a (x,y).
 *
 * \param prc The pointer to the rectangle.
 * \param x x,y: The point.
 * \param y x,y: The point.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 *
 * \sa InflateRect
 */
static inline void InflateRectToPt (RECT* prc, int x, int y)
{
    if ((x) < (prc)->left) (prc)->left = (x); 
    if ((y) < (prc)->top) (prc)->top = (y);
    if ((x) > (prc)->right) (prc)->right = (x);
    if ((y) > (prc)->bottom) (prc)->bottom = (y);
}

/**
 * \fn BOOL PtInRect(const RECT* prc, int x, int y)
 * \brief Determines whether a point lies within an rectangle.
 *
 * This function determines whether the specified point \a (x,y) lies within 
 * the specified rectangle \a prc.
 *
 * A point is within a rectangle if it lies on the left or top side or is 
 * within all four sides. A point on the right or bottom side is considered 
 * outside the rectangle.
 *
 * \param prc The pointer to the rectangle.
 * \param x x,y: The point.
 * \param y x,y: The point.
 *
 * \note Defined as an inline function for _USE_NEWGAL. 
 */
static inline BOOL PtInRect(const RECT* prc, int x, int y)
{
    if (x >= prc->left && x < prc->right && y >= prc->top && y < prc->bottom)
        return TRUE;
    return FALSE;
}

#else

MG_EXPORT void GUIAPI SetRect (RECT* prc, int left, int top, int right, int bottom);
MG_EXPORT void GUIAPI SetRectEmpty (RECT* prc);
MG_EXPORT void GUIAPI CopyRect (RECT* pdrc, const RECT* psrc);
MG_EXPORT void GUIAPI OffsetRect (RECT* prc, int x, int y);
MG_EXPORT void GUIAPI InflateRect (RECT* prc, int cx, int cy);
MG_EXPORT void GUIAPI InflateRectToPt (RECT* prc, int x, int y);
MG_EXPORT BOOL GUIAPI PtInRect (const RECT* prc, int x, int y);

#endif

/**
 * \fn BOOL GUIAPI IsRectEmpty (const RECT* prc)
 * \brief Determines whether an rectangle is empty.
 *
 * This function determines whether the specified rectangle \a prc is empty. 
 * An empty rectangle is one that has no area; that is, the coordinates 
 * of the right side is equal to the coordinate of the left side, or the
 * coordinates of the bottom side is equal to the coordinate of the top side.
 *
 * \param prc The pointer to the rectangle.
 * \return TRUE for empty, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI IsRectEmpty (const RECT* prc);

/**
 * \fn BOOL GUIAPI EqualRect (const RECT* prc1, const RECT* prc2)
 * \brief Determines whether two rectangles are equal.
 *
 * This function determines whether the two specified rectangles 
 * (\a prc1 and \a prc2) are equal by comparing the coordinates of 
 * the upper-left and lower-right corners.
 *
 * \param prc1 The pointers to the first rectangles.
 * \param prc2 The pointers to the second rectangles.
 * \return TRUE for equal, otherwise FALSE.
 */
MG_EXPORT BOOL GUIAPI EqualRect (const RECT* prc1, const RECT* prc2);

/**
 * \fn void GUIAPI NormalizeRect (RECT* pRect)
 * \brief Normalizes a rectangle.
 *
 * This function normalizes the rectangle pointed to by \a prc 
 * so that both the height and width are positive.
 *
 * \param pRect The pointer to the rectangle.
 */
MG_EXPORT void GUIAPI NormalizeRect (RECT* pRect);

/**
 * \fn BOOL GUIAPI IntersectRect (RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
 * \brief Calculates the intersection of two rectangles.
 *
 * This function calculates the intersection of two source rectangles (\a psrc1 and \a psrc2) 
 * and places the coordinates of the intersection rectangle into the destination rectangle
 * pointed to by \a pdrc. If the source rectangles do not intersect, and empty rectangle 
 * (in which all coordinates are set to zero) is placed into the destination rectangle.
 *
 * \param pdrc The pointer to the destination rectangle.
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 * \return TRUE if the source rectangles intersect, otherwise FALSE.
 *
 * \sa DoesIntersect, IsCovered
 */
MG_EXPORT BOOL GUIAPI IntersectRect (RECT* pdrc, const RECT* psrc1, const RECT* psrc2);

/**
 * \fn BOOL GUIAPI IsCovered (const RECT* prc1, const RECT* prc2)
 * \brief Determines whether one rectangle is covered by another.
 *
 * This function determines whether one rectangle (\a prc1) 
 * is covered by another rectangle (\a prc2).
 *
 * \param prc1 The first rectangles.
 * \param prc2 The second rectangles.
 * \return TRUE if the first rectangle is covered by the second, otherwise FALSE.
 *
 * \sa DoesIntersect
 */
MG_EXPORT BOOL GUIAPI IsCovered (const RECT* prc1, const RECT* prc2);

/**
 * \fn BOOL GUIAPI DoesIntersect (const RECT* psrc1, const RECT* psrc2)
 * \brief Determines whether two rectangles intersect.
 *
 * This function determines whether two rectangles (\a psrc1 and \a psrc2) intersect.
 *
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 * \return TRUE if the source rectangles intersect, otherwise FALSE.
 *
 * \sa IntersectRect
 */
MG_EXPORT BOOL GUIAPI DoesIntersect (const RECT* psrc1, const RECT* psrc2);

/**
 * \fn BOOL GUIAPI UnionRect (RECT* pdrc, const RECT* psrc1, const RECT* psrc2)
 * \brief Unions two source rectangles.
 *
 * This function creates the union (\a pdrc) of two rectangles (\a psrc1 and \a psrc2),
 * if the source rectangles are border upon and not stagger.
 *
 * \param pdrc The unioned rectangle.
 * \param psrc1 The first source rectangles.
 * \param psrc2 The second source rectangles.
 * \return TRUE if the source rectangles are border upon and not stagger, otherwise FALSE.
 *
 * \sa GetBoundRect
 */
MG_EXPORT BOOL GUIAPI UnionRect (RECT* pdrc, const RECT* psrc1, const RECT* psrc2);

/**
 * \fn void GUIAPI GetBoundRect (PRECT pdrc,  const RECT* psrc1, const RECT* psrc2)
 * \brief Gets the bound rectangle of two source rectangles.
 *
 * This function creates the bound rect (\a pdrc) of two rectangles (\a psrc1 and \a prsrc2). 
 * The bound rect is the smallest rectangle that contains both source rectangles.
 *
 * \param pdrc The destination rectangle.
 * \param psrc1 The first source rectangle.
 * \param psrc2 The second source rectangle.
 *
 * \sa UnionRect
 */
MG_EXPORT void GUIAPI GetBoundRect (PRECT pdrc,  const RECT* psrc1, const RECT* psrc2);

/**
 * \fn int GUIAPI SubtractRect (RECT* rc, const RECT* psrc1, const RECT* psrc2)
 * \brief Obtains the rectangles when substracting one rectangle from another.
 *
 * This function obtains the rectangles substracting the rectangle \a psrc1 
 * from the other \a psrc2. \a rc should be an array of RECT struct, and 
 * may contain at most four rectangles. This function returns 
 * the number of result rectangles.
 *
 * \param rc The pointer to the resule rectangle array.
 * \param psrc1 The pointer to the minuend rectangle.
 * \param psrc2 The pointer to the subtrahend rectangle.
 * \return The number of result rectangles.
 *
 * \sa UnionRect
 */
MG_EXPORT int GUIAPI SubtractRect (RECT* rc, const RECT* psrc1, const RECT* psrc2);

/**
 * \def RECTWP(prc)
 * \brief Gets the width of a RECT object by using the pointer to it.
 */
#define RECTWP(prc)  (prc->right - prc->left)
/**
 * \def RECTHP(prc)
 * \brief Gets the height of a RECT object by using the pointer to it.
 */
#define RECTHP(prc)  (prc->bottom - prc->top)
/**
 * \def RECTW(rc)
 * \brief Gets the width of a RECT object.
 */
#define RECTW(rc)    (rc.right - rc.left)
/**
 * \def RECTH(rc)
 * \brief Gets the height of a RECT object.
 */
#define RECTH(rc)    (rc.bottom - rc.top)

    /** @} end of rect_fns */

    /**
     * \defgroup font_fns Logical font operations
     *
     * MiniGUI uses logical font to render text in a DC. You can create a logical
     * font by using \a CreateLogFont and select it into a DC by using \a SelectFont,
     * then you can use this logical font to render text by using \a TextOutLen or \a DrawTextEx.
     *
     * @{
     */

/* Font-related structures */
#define LEN_FONT_NAME               15
#define LEN_DEVFONT_NAME            79
#define LEN_UNIDEVFONT_NAME         127

#define FONT_WEIGHT_NIL             '\0'
#define FONT_WEIGHT_ALL             '*'
#define FONT_WEIGHT_BLACK           'c'
#define FONT_WEIGHT_BOLD            'b'
#define FONT_WEIGHT_BOOK            'k'
#define FONT_WEIGHT_DEMIBOLD        'd'
#define FONT_WEIGHT_LIGHT           'l'
#define FONT_WEIGHT_MEDIUM          'm'
#define FONT_WEIGHT_REGULAR         'r'

#define FS_WEIGHT_MASK              0x000000FF
#define FS_WEIGHT_BLACK             0x00000001
#define FS_WEIGHT_BOLD              0x00000002
#define FS_WEIGHT_BOOK              0x00000004
#define FS_WEIGHT_DEMIBOLD          0x00000006 /* BOOK | BOLD */
#define FS_WEIGHT_LIGHT             0x00000010
#define FS_WEIGHT_MEDIUM            0x00000020
#define FS_WEIGHT_REGULAR           0x00000000

#define FONT_SLANT_NIL              '\0'
#define FONT_SLANT_ALL              '*'
#define FONT_SLANT_ITALIC           'i'
#define FONT_SLANT_OBLIQUE          'o'
#define FONT_SLANT_ROMAN            'r'

#define FS_SLANT_MASK               0x00000F00
#define FS_SLANT_ITALIC             0x00000100
#define FS_SLANT_OBLIQUE            0x00000200
#define FS_SLANT_ROMAN              0x00000000

#define FONT_SETWIDTH_NIL           '\0'
#define FONT_SETWIDTH_ALL           '*'
#define FONT_SETWIDTH_BOLD          'b'
#define FONT_SETWIDTH_CONDENSED     'c'
#define FONT_SETWIDTH_SEMICONDENSED 's'
#define FONT_SETWIDTH_NORMAL        'n'

#define FS_SETWIDTH_MASK            0x0000F000
#define FS_SETWIDTH_BOLD            0x00001000
#define FS_SETWIDTH_CONDENSED       0x00002000
#define FS_SETWIDTH_SEMICONDENSED   0x00004000
#define FS_SETWIDTH_NORMAL          0x00000000

#define FONT_SPACING_NIL            '\0'
#define FONT_SPACING_ALL            '*'
#define FONT_SPACING_MONOSPACING    'm'
#define FONT_SPACING_PROPORTIONAL   'p'
#define FONT_SPACING_CHARCELL       'c'

#define FS_SPACING_MASK             0x000F0000
#define FS_SPACING_MONOSPACING      0x00010000
#define FS_SPACING_PROPORTIONAL     0x00020000
#define FS_SPACING_CHARCELL         0x00000000

#define FONT_UNDERLINE_NIL          '\0'
#define FONT_UNDERLINE_ALL          '*'
#define FONT_UNDERLINE_LINE         'u'
#define FONT_UNDERLINE_NONE         'n'

#define FS_UNDERLINE_MASK           0x00F00000
#define FS_UNDERLINE_LINE           0x00100000
#define FS_UNDERLINE_NONE           0x00000000

#define FONT_STRUCKOUT_NIL          '\0'
#define FONT_STRUCKOUT_ALL          '*'
#define FONT_STRUCKOUT_LINE         's'
#define FONT_STRUCKOUT_NONE         'n'

#define FS_STRUCKOUT_MASK           0x0F000000
#define FS_STRUCKOUT_LINE           0x01000000
#define FS_STRUCKOUT_NONE           0x00000000

#define FONT_TYPE_NAME_BITMAP_RAW   "rbf"
#define FONT_TYPE_NAME_BITMAP_VAR   "vbf"
#define FONT_TYPE_NAME_BITMAP_QPF   "qpf"
#define FONT_TYPE_NAME_SCALE_TTF    "ttf"
#define FONT_TYPE_NAME_SCALE_T1F    "t1f"
#define FONT_TYPE_NAME_ALL          "*"

#define FONT_TYPE_BITMAP_RAW        0x0001  /* "rbf" */
#define FONT_TYPE_BITMAP_VAR        0x0002  /* "vbf" */
#define FONT_TYPE_BITMAP_QPF        0x0003  /* "qpf" */
#define FONT_TYPE_SCALE_TTF         0x0010  /* "ttf" */
#define FONT_TYPE_SCALE_T1F         0x0011  /* "t1f" */
#define FONT_TYPE_ALL               0xFFFF  /* "all" */

#define FONT_CHARSET_US_ASCII       "US-ASCII"

#define FONT_CHARSET_ISO8859_1      "ISO8859-1"
#define FONT_CHARSET_ISO8859_2      "ISO8859-2"
#define FONT_CHARSET_ISO8859_3      "ISO8859-3"
#define FONT_CHARSET_ISO8859_4      "ISO8859-4"
#define FONT_CHARSET_ISO8859_5      "ISO8859-5"
#define FONT_CHARSET_ISO8859_6      "ISO8859-6"
#define FONT_CHARSET_ISO8859_7      "ISO8859-7"
#define FONT_CHARSET_ISO8859_8      "ISO8859-8"
#define FONT_CHARSET_ISO8859_9      "ISO8859-9"
#define FONT_CHARSET_ISO8859_10     "ISO8859-10"
#define FONT_CHARSET_ISO8859_11     "ISO8859-11"
#define FONT_CHARSET_ISO8859_12     "ISO8859-12"
#define FONT_CHARSET_ISO8859_13     "ISO8859-13"
#define FONT_CHARSET_ISO8859_14     "ISO8859-14"
#define FONT_CHARSET_ISO8859_15     "ISO8859-15"
#define FONT_CHARSET_ISO8859_16     "ISO8859-16"

#define FONT_CHARSET_EUC_CN         "EUC-CN"
#define FONT_CHARSET_GB1988_0       "GB1988-0"      /* EUC encoding of GB1988 charset, treat as ISO8859-1 */
#define FONT_CHARSET_GB2312_0       "GB2312-0"      /* EUC encoding of GB2312 charset */

#define FONT_CHARSET_GBK            "GBK"
#define FONT_CHARSET_GB18030_0      "GB18030-0"     /* EUC encoding of GB18030 charset */

#define FONT_CHARSET_BIG5           "BIG5"
#define FONT_CHARSET_EUCTW          "EUC-TW"        /* EUC encoding of CNS11643 charset, not supported */

#define FONT_CHARSET_EUCKR          "EUC-KR"
#define FONT_CHARSET_KSC5636_0      "KSC5636-0"     /* EUC encoding of KSC5636 charset, treat as ISO8859-1 */
#define FONT_CHARSET_KSC5601_0      "KSC5601-0"     /* EUC encoding of KSC5601 charset */

#define FONT_CHARSET_EUCJP          "EUC-JP"
#define FONT_CHARSET_JISX0201_0     "JISX0201-0"    /* EUC encoding of JISX0201 charset */
#define FONT_CHARSET_JISX0208_0     "JISX0208-0"    /* EUC encoding of JISX0208 charset */

#define FONT_CHARSET_SHIFTJIS       "SHIFT-JIS"
#define FONT_CHARSET_JISX0201_1     "JISX0201-1"    /* Shift-JIS encoding of JISX0201 charset */
#define FONT_CHARSET_JISX0208_1     "JISX0208-1"    /* Shift-JIS encoding of JISX0208 charset */

#define FONT_CHARSET_ISO_10646_1    "ISO-10646-1"   /* UCS-2 encoding of UNICODE */
#define FONT_CHARSET_UTF8           "UTF-8"

#define FONT_MAX_SIZE               256
#define FONT_MIN_SIZE               4

struct _DEVFONT;
typedef struct _DEVFONT DEVFONT;

/** The logical font structure. */
typedef struct _LOGFONT {
    /** The type of the logical font. */
    char type [LEN_FONT_NAME + 1];
    /** The family name of the logical font. */
    char family [LEN_FONT_NAME + 1];
    /** The charset of the logical font. */
    char charset [LEN_FONT_NAME + 1];
    /** The styles of the logical font. */
    DWORD style;
    /** The size of the logical font. */
    int size;
    /** The rotation angle of the logical font. */
    int rotation;
    DEVFONT* sbc_devfont;
    DEVFONT* mbc_devfont;
} LOGFONT;
typedef LOGFONT*    PLOGFONT;

struct _WORDINFO;
typedef struct _WORDINFO WORDINFO;

#define MAX_LEN_MCHAR               4

typedef struct _CHARSETOPS
{
    int nr_chars;
    int bytes_per_char;
    int bytes_maxlen_char;
    const char* name;
    char def_char [MAX_LEN_MCHAR];

    int (*len_first_char) (const unsigned char* mstr, int mstrlen);
    unsigned int (*char_offset) (const unsigned char* mchar);

    int (*nr_chars_in_str) (const unsigned char* mstr, int mstrlen);

    int (*is_this_charset) (const unsigned char* charset);

    int (*len_first_substr) (const unsigned char* mstr, int mstrlen);
    const unsigned char* (*get_next_word) (const unsigned char* mstr, 
                int strlen, WORDINFO* word_info);

    int (*pos_first_char) (const unsigned char* mstr, int mstrlen);

#ifdef _UNICODE_SUPPORT
    unsigned short (*conv_to_uc16) (const unsigned char* mchar, int len);
#endif /* LITE_VERSION */
} CHARSETOPS;

typedef struct _FONTOPS
{
    int (*get_char_width) (LOGFONT* logfont, DEVFONT* devfont, 
            const unsigned char* mchar, int len);
    int (*get_str_width) (LOGFONT* logfont, DEVFONT* devfont, 
            const unsigned char* mstr, int n, int cExtra);
    int (*get_ave_width) (LOGFONT* logfont, DEVFONT* devfont);
    int (*get_max_width) (LOGFONT* logfont, DEVFONT* devfont);
    int (*get_font_height) (LOGFONT* logfont, DEVFONT* devfont);
    int (*get_font_size) (LOGFONT* logfont, DEVFONT* devfont, int expect);
    int (*get_font_ascent) (LOGFONT* logfont, DEVFONT* devfont);
    int (*get_font_descent) (LOGFONT* logfont, DEVFONT* devfont);

/* TODO: int (*get_font_ABC) (LOGFONT* logfont); */
    
    size_t (*char_bitmap_size) (LOGFONT* logfont, DEVFONT* devfont, 
            const unsigned char* mchar, int len);
    size_t (*max_bitmap_size) (LOGFONT* logfont, DEVFONT* devfont);
    const void* (*get_char_bitmap) (LOGFONT* logfont, DEVFONT* devfont, 
            const unsigned char* mchar, int len);
    
    const void* (*get_char_pixmap) (LOGFONT* logfont, DEVFONT* devfont, 
            const unsigned char* mchar, int len, int* pitch);
         /* Can be NULL */

    void (*start_str_output) (LOGFONT* logfont, DEVFONT* devfont);
         /* Can be NULL */
    int (*get_char_bbox) (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len,
            int* px, int* py, int* pwidth, int* pheight);
         /* Can be NULL */
    void (*get_char_advance) (LOGFONT* logfont, DEVFONT* devfont,
            const unsigned char* mchar, int len, int* px, int* py);
         /* Can be NULL */

    DEVFONT* (*new_instance) (LOGFONT* logfont, DEVFONT* devfont, 
            BOOL need_sbc_font);
         /* Can be NULL */
    void (*delete_instance) (DEVFONT* devfont);
         /* Can be NULL */
} FONTOPS;

struct _DEVFONT
{
    char             name [LEN_UNIDEVFONT_NAME + 1];
    DWORD            style;
    FONTOPS*         font_ops;
    CHARSETOPS*      charset_ops;
    struct _DEVFONT* sbc_next;
    struct _DEVFONT* mbc_next;
    void*            data;
};

#define SBC_DEVFONT_INFO(logfont) (logfont.sbc_devfont)
#define MBC_DEVFONT_INFO(logfont) (logfont.mbc_devfont)

#define SBC_DEVFONT_INFO_P(logfont) (logfont->sbc_devfont)
#define MBC_DEVFONT_INFO_P(logfont) (logfont->mbc_devfont)

#define INV_LOGFONT     0

/** The font metrics structure. */
typedef struct _FONTMETRICS
{
    /** the height of the logical font. */
    int font_height;
    /** the ascent of the logical font. */
    int ascent;
    /** the descent of the logical font. */
    int descent;

    /** the maximal width of the logical font. */
    int max_width;
    /** the average width of the logical font. */
    int ave_width;
} FONTMETRICS;

/**
 * \fn void GUIAPI GetFontMetrics (LOGFONT* log_font, FONTMETRICS* font_metrics)
 * \brief Gets metrics information of a logical font.
 *
 * This function returns the font metrics information of the specified
 * logical font \a log_font.
 *
 * \param log_font The pointer to the logical font.
 * \param font_metrics The buffer receives the metrics information.
 * \return None.
 *
 * \sa GetGlyphBitmap, FONTMETRICS
 */
MG_EXPORT void GUIAPI GetFontMetrics (LOGFONT* log_font, FONTMETRICS* font_metrics);

/** The glyph bitmap structure. */
typedef struct _GLYPHBITMAP
{
    /** The bounding box of the glyph. */
    int bbox_x, bbox_y, bbox_w, bbox_h;
    /** The advance value of the glyph. */
    int advance_x, advance_y;

    /** The size of the glyph bitmap. */
    size_t bmp_size;
    /** The pitch of the glyph bitmap. */
    int bmp_pitch;
    /** The pointer to the buffer of glyph bitmap bits. */
    const unsigned char* bits;
} GLYPHBITMAP;

/**
 * \fn void GUIAPI GetGlyphBitmap (LOGFONT* log_font, const unsigned char* mchar, int mchar_len, GLYPHBITMAP* glyph_bitmap)
 * \brief Gets the glyph bitmap information when uses a logical font to output a character.
 *
 * This function gets the glyph bitmap of one multi-byte character
 * (specified by \a mchar and \a mchar_len) and returns the bitmap information
 * through \a font_bitmap when using \a log_font to render the character.
 *
 * \param log_font The logical font used to render the character.
 * \param mchar The pointer to the multi-byte character.
 * \param mchar_len The length of the multi-byte character.
 * \param glyph_bitmap The buffer receives the glyph bitmap information.
 * \return None.
 *
 * Example:
 * \code
 *      GLYPHBITMAP glyph_bitmap = {0};
 *
 *      GetFontBitmap (log_font, "A", 1, &glyph_bitmap);
 * \endcode
 *
 * \sa GetFontMetrics, GLYPHBITMAP
 */
MG_EXPORT void GUIAPI GetGlyphBitmap (LOGFONT* log_font, const unsigned char* mchar, int mchar_len, 
                GLYPHBITMAP* glyph_bitmap);

#ifdef _LITE_VERSION

/**
 * \fn BOOL GUIAPI InitVectorialFonts (void)
 * \brief Initializes vectorial font renderer.
 *
 * This function initializes vectorial font renderer for MiniGUI-Lite application.
 * For the performance reason, MiniGUI-Lite does not load vetorical fonts, such as
 * TrueType or Adobe Type1, at startup. If you want to render text in vectorial fonts,
 * you must call this function to initialize TrueType and Type1 font renderer.
 *
 * \return TRUE on success, FALSE on error.
 *
 * \note Only defined for _LITE_VERSION. If your MiniGUI configured as MiniGUI-Threads,
 *       no need to initialize TrueType and Type1 font renderer explicitly.
 *
 * \sa TermVectorialFonts
 */
MG_EXPORT BOOL GUIAPI InitVectorialFonts (void);

/**
 * \fn void GUIAPI TermVectorialFonts (void)
 * \brief Terminates vectorial font renderer.
 *
 * This function terminates the vectorial font renderer.
 * When you are done with vectorial fonts, you should call this function to 
 * unload the vectorial fonts to save memory.
 *
 * \note Only defined for _LITE_VERSION. 
 *
 * \sa InitVectorialFonts
 */
MG_EXPORT void GUIAPI TermVectorialFonts (void);

#endif

/**
 * \fn PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family, const char* charset, char weight, char slant, char set_width, char spacing, char underline, char struckout, int size, int rotation)
 * \brief Creates a logical font.
 *
 * This function creates a logical font. 
 *
 * \param type The type of the logical font, can be one of the values:
 *      - FONT_TYPE_NAME_BITMAP_RAW\n
 *        Creates a logical font by using raw bitmap device font, i.e. mono-space bitmap font.
 *      - FONT_TYPE_NAME_BITMAP_VAR\n
 *        Creates a logical font by using var-width bitmap device font.
 *      - FONT_TYPE_NAME_SCALE_TTF\n
 *        Creates a logical font by using scalable TrueType device font.
 *      - FONT_TYPE_NAME_SCALE_T1F\n
 *        Creates a logical font by using scalable Adobe Type1 device font.
 *      - FONT_TYPE_NAME_ALL\n
 *        Creates a logical font by using any type device font.
 * \param family The family of the logical font, such as "Courier", "Helvetica", and so on.
 * \param charset The charset of the logical font. You can specify a sigle-byte charset
 *        like "ISO8859-1", or a multi-byte charset like "GB2312.1980-0".
 * \param weight The weight of the logical font, can be one of the values:
 *      - FONT_WEIGHT_ALL\n
 *        Any one.
 *      - FONT_WEIGHT_REGULAR\n
 *        Regular font.
 *      - FONT_WEIGHT_BOLD\n
 *        Bold font.
 * \param slant The slant of the logical font, can be one of the values:
 *      - FONT_SLANT_ALL\n
 *        Any one.
 *      - FONT_SLANT_ROMAN\n
 *        Regular font.
 *      - FONT_SLANT_ITALIC\n
 *        Italic font.
 * \param set_width Not used so far.
 * \param spacing Not used so far.
 * \param underline The underline of the logical font, can be one of the values:
 *      - FONT_UNDERLINE_ALL\n
 *        Any one.
 *      - FONT_UNDERLINE_LINE\n
 *        With underline.
 *      - FONT_UNDERLINE_NONE\n
 *        Without underline.
 * \param struckout The struckout line of the logical font, can be one of the values:
 *      - FONT_STRUCKOUT_ALL\n
 *        Any one.
 *      - FONT_STRUCKOUT_LINE\n
 *        With struckout line.
 *      - FONT_STRUCKOUT_NONE\n
 *        Without struckout line.
 * \param size The size, i.e. the height, of the logical font. Note that the size of 
 *        the created logical font may be different from the size expected.
 * \param rotation The rotation of the logical font, it is in units of tenth degrees.
 *        Note that you can specify rotation only for TrueType and Adobe Type1 fonts.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFontIndirect, SelectFont
 *
 * Example:
 *
 * \include createlogfont.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFont (const char* type, const char* family, 
        const char* charset, char weight, char slant, char set_width, 
        char spacing, char underline, char struckout, 
        int size, int rotation);

/**
 * \fn PLOGFONT GUIAPI CreateLogFontByName (const char* font_name)
 * \brief Creates a logical font by a font name.
 *
 * This function creates a logical font by a font name specified by \a font_name.
 *
 * \param font_name The name of the font.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFont, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontByName (const char* font_name);

/**
 * \fn PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT* logfont)
 * \brief Creates a logical font indirectly from a LOGFONT structure.
 *
 * This function creates a logical font from information in a LOGFONT object.
 *
 * \param logfont The pointer to the reference logical font structure.
 * \return The pointer to the logical font created, NULL on error.
 *
 * \sa CreateLogFont, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI CreateLogFontIndirect (LOGFONT* logfont);

/**
 * \fn void GUIAPI DestroyLogFont (PLOGFONT log_font)
 * \brief Destroies a logical font.
 *
 * This function destroies the specified logical font \a log_font.
 *
 * \param log_font The logical font will be destroied.
 *
 * \sa CreateLogFont, CreateLogFontIndirect
 */
MG_EXPORT void GUIAPI DestroyLogFont (PLOGFONT log_font);

/**
 * \fn void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font)
 * \brief Gets logical font information of a DC.
 *
 * This function gets the logical font information of the specified DC \a hdc,
 * and copies to the LOGFONT structure pointed to by \a log_font.
 *
 * \param hdc The device context.
 * \param log_font The pointer to the LOGFONT structure to save the logical font info.
 *
 * \sa GetCurFont
 */
MG_EXPORT void GUIAPI GetLogFontInfo (HDC hdc, LOGFONT* log_font);

/**
 * \fn PLOGFONT GUIAPI GetCurFont (HDC hdc)
 * \brief Gets the pointer to the current logical font of a DC.
 *
 * This function returns the pointer to the current logical font selected to the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The pointer to the current logical font.
 *
 * \sa GetLogFontInfo, SelectFont
 */
MG_EXPORT PLOGFONT GUIAPI GetCurFont (HDC hdc);

/**
 * \fn PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font)
 * \brief Selects a logical font into a DC.
 *
 * This function selects the specified logical font \a log_font into the device context \a hdc.
 *
 * \param hdc The device context.
 * \param log_font The pointer to the logical font to be selected into the DC.
 * \return The old logical font of the DC.
 *
 * \sa GetCurFont
 *
 * Example:
 *
 * \include drawtext.c
 *
 */
MG_EXPORT PLOGFONT GUIAPI SelectFont (HDC hdc, PLOGFONT log_font);

/**
 * \fn const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font)
 * \brief Get next devfont information.
 *
 * This function can be used to enumerate all device fonts loaded by MiniGUI.
 *
 * \param dev_font The pointer to the current device font, can be NULL to get
 *                 the first device font.
 * \return The next device font.
 */
MG_EXPORT const DEVFONT* GUIAPI GetNextDevFont (const DEVFONT* dev_font);

    /** @} end of font_fns */

    /**
     * \defgroup system_font System charset and font operations
     *
     * MiniGUI creates a few system fonts to draw menu text, window caption, or
     * other general items. MiniGUI at least creates two system fonts: one mono-space
     * logical font for single-byte charset, and one mono-space logical font for
     * multi-byte charset. For the multi-byte charset, 
     * the width of one multi-byte character should be equal to the width of 
     * two single-byte characters.
     *
     * @{
     */

#define SYSLOGFONT_DEFAULT          0
#define SYSLOGFONT_WCHAR_DEF        1
#define SYSLOGFONT_FIXED            2
#define SYSLOGFONT_CAPTION          3
#define SYSLOGFONT_MENU             4
#define SYSLOGFONT_CONTROL          5

#define NR_SYSLOGFONTS              6


#ifdef WIN32
#ifdef __MINIGUI_LIB__
MG_EXPORT  PLOGFONT g_SysLogFont [];
#else
MG_IMPORT  PLOGFONT g_SysLogFont [];
#endif
#else
extern PLOGFONT g_SysLogFont [];
#endif

/**
 * \fn PLOGFONT GUIAPI GetSystemFont (int font_id)
 * \brief Gets the system logical font through an font identifier.
 *
 * This function returns the system logical font through the font identifier \a font_id.
 *
 * \param font_id The identifier of a system font, can be one of the following values:
 *      - SYSLOGFONT_DEFAULT\n
 *        The default system logical font in single-byte charset, must be rbf.
 *      - SYSLOGFONT_WCHAR_DEF\n
 *        The default system logical font in multi-byte charset, must be rbf.
 *      - SYSLOGFONT_FIXED\n
 *        The fixed space system logical font.
 *      - SYSLOGFONT_CAPTION\n
 *        The system logical font used to draw caption text.
 *      - SYSLOGFONT_MENU\n
 *        The system logical font used to draw menu items.
 *      - SYSLOGFONT_CONTROL\n
 *        The system logical font used to draw controls.
 * \return The pointer to the system logical font. NULL on error.
 */
static inline PLOGFONT GUIAPI GetSystemFont (int font_id)
{
    if (font_id >= 0 && font_id < NR_SYSLOGFONTS)
        return g_SysLogFont [font_id];
    else
        return (PLOGFONT)NULL;
}

/**
 * \fn int GUIAPI GetSysFontMaxWidth (int font_id)
 * \brief Gets the maximal width of a single-byte character of a system font.
 *
 * This function returns the maximal width of a single-byte character of one system font.
 *
 * \param font_id The identifier of a system font.
 * \return The maximal width of single-byte character of the default system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontMaxWidth (int font_id);

/**
 * \fn int GUIAPI GetSysFontAveWidth (int font_id)
 * \brief Gets the average width of a single-byte character of a system font.
 *
 * This function returns the average width of a single-byte character of one system font.
 *
 * \param font_id The identifier of a system font.
 * \return The average width of single-byte character of the default system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontAveWidth (int font_id);

/**
 * \fn int GUIAPI GetSysFontHeight (int font_id)
 * \brief Gets the height of a single-byte character of a system font.
 *
 * This function returns the height of a single-byte character of one system font.
 *
 * \param font_id The identifier of a system font.
 * \return The height of single-byte character of the default system font.
 *
 * \sa GetSystemFont
 */
MG_EXPORT int GUIAPI GetSysFontHeight (int font_id);

/**
 * \fn const char* GUIAPI GetSysCharset (BOOL wchar)
 * \brief Gets the current system charset.
 *
 * This function gets the current system charset and returns the charset name.
 * By default, the system charset is ISO8859-1 (for single-byte charset) or 
 * GB2312.1980-0 (for wide charset), but you can change it by modifying \a MiniGUI.cfg. 
 *
 * \param wchar Whether to retrive the wide charset supported currently.
 * \return The read-only buffer of charset name. If you pass \a wchar TRUE,
 *         This function may return NULL, if there is not any wide charset supported.
 */
MG_EXPORT const char* GUIAPI GetSysCharset (BOOL wchar);

/**
 * \fn int GUIAPI GetSysCharHeight (void)
 * \brief Gets the height of a character of the default system font.
 *
 * This function returns the height of a character of 
 * the system default font. MiniGUI uses mono-space font as the default system font.
 *
 * Normally, the width of one multi-byte character is equal to 
 * the width of two single-byte character when using the default system font.
 *
 * \return The character height of the default system font.
 *
 * \sa GetSysCharWidth, GetSysCCharWidth
 */
MG_EXPORT int GUIAPI GetSysCharHeight (void);

/**
 * \fn int GUIAPI GetSysCharWidth (void)
 * \brief Gets the width of a single-byte character of the default system font.
 *
 * This function returns the width of a single-byte character of the default system font.
 * MiniGUI uses mono-space font as the default system font, but you can specify a 
 * different font to output text in windows of MiniGUI.
 *
 * \return The width of single-byte character of the default system font.
 *
 * \sa GetSysCharHeight, GetSysCCharWidth, SelectFont, CreateLogFont
 */
MG_EXPORT int GUIAPI GetSysCharWidth (void);

/**
 * \fn int GUIAPI GetSysCCharWidth (void)
 * \brief Gets the width of a multi-byte character of the default system font.
 *
 * This function returns the width of a multi-byte character of 
 * the default system font. MiniGUI uses mono-space font as the system default font.
 *
 * \return The width of multi-byte character of the default system font.
 *
 * \sa GetSysCharHeight, GetSysCharWidth
 */
MG_EXPORT int GUIAPI GetSysCCharWidth (void);

#if 0

/**
 * \fn void GUIAPI GetSysTabbedTextExtent (const char* spText, SIZE* pSize)
 * \brief Gets the output extent of a formatted string when using the system font.
 *
 * This function gets the output extent of the formatted string pointed to by \a spText 
 * when using the system font, nnd returns the extent in SIZE struct pointed to by \a pSize.
 *
 * \param spText The pointer to the formatted string.
 * \param pSize The output extent will be returned through this buffer.
 *
 * \note Obsolete
 *
 * \sa GetSysCharWidth, GetSysCharHeight
 */
MG_EXPORT void GUIAPI GetSysTabbedTextExtent (const char* spText, SIZE* pSize);

/**
 * \fn BOOL GUIAPI DoesSupportCharset (const char* charset)
 * \brief Determines whether MiniGUI supports a system charset.
 *
 * This function determines whether MiniGUI 
 * supports the specified system charset \a charset.
 *
 * \param charset The name of the charset.
 * \return TRUE for supported charset, otherwise FALSE.
 *
 * \note Obsolete
 *
 * \sa GetSysCharset
 */
MG_EXPORT BOOL GUIAPI DoesSupportCharset (const char* charset);

/**
 * \fn BOOL GUIAPI SetSysCharset (const char* charset)
 * \brief Sets a charset to be the system charset.
 *
 * This function sets the specified charset \a charset to be the system charset.
 *
 * \param charset The name of the charset.
 * \return 0 on success, -1 on error.
 *
 * \note Obsolete
 *
 * \sa GetSysCharset
 */
MG_EXPORT BOOL GUIAPI SetSysCharset (const char* charset);

#endif

    /** @} end of system_font */

    /**
     * \defgroup text_parse_fns Text parsing functions
     *
     * MiniGUI uses logical font to output text, also uses
     * logical font to parsing the multi-byte characters and
     * words in the text.
     *
     * @{
     */

/** Word information structure */
struct _WORDINFO
{
    /** the length of the word. */
    int len;
    /** the delimiter after the word. */
    unsigned char delimiter;
    /** the number of delimiters after the word. */
    int nr_delimiters;
};

/**
 * \fn int GUIAPI GetTextMCharInfo (PLOGFONT log_font, const char* mstr, int len, int* pos_chars)
 * \brief Retrives positions of multi-byte characters in a string.
 *
 * This function retrives position of multi-byte characters in 
 * the string \a mstr which is \a len bytes long. It returns the positions 
 * of characters in the string in an integer array pointed to by \a pos_chars.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param pos_chars The position of characters will be returned through this array if it is not NULL.
 * \return The number of characters in the multi-byte string.
 *
 * \sa GetTextWordInfo
 */
MG_EXPORT int GUIAPI GetTextMCharInfo (PLOGFONT log_font, const char* mstr, int len, 
                int* pos_chars);

/**
 * \fn int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr, int len, int* pos_words, WORDINFO* info_words)
 * \brief Retrives information of multi-byte words in a string.
 *
 * This function retrives information of multi-byte words in the string \a mstr which is 
 * \a len bytes long. It returns the positions of words in the string in an integer 
 * array pointed to by \a pos_words if the pointer is not NULL. This function also 
 * returns the information of words in a WORDINFO struct array pointed to by \a info_words 
 * when the pointer is not NULL.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param pos_words The position of words will be returned through this array if it is not NULL.
 * \param info_words The words' information will be returned through this array if it is not NULL.
 * \return The number of words in the multi-byte string.
 *
 * \sa GetTextMCharInfo, WORDINFO
 */
MG_EXPORT int GUIAPI GetTextWordInfo (PLOGFONT log_font, const char* mstr, int len, 
                int* pos_words, WORDINFO* info_words);

/**
 * \fn int GUIAPI GetFirstMCharLen (PLOGFONT log_font, const char* mstr, int len)
 * \brief Retrives the length of the first multi-byte character in a string.
 *
 * This function retrives and returns the length of the first multi-byte 
 * character in the string \a mstr which is \a len bytes long.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \return The length of the first multi-byte character.
 *
 * \sa GetFirstWord
 */
MG_EXPORT int GUIAPI GetFirstMCharLen (PLOGFONT log_font, const char* mstr, int len);

/**
 * \fn int GUIAPI GetFirstWord (PLOGFONT log_font, const char* mstr, int len, WORDINFO* word_info)
 * \brief Retrives the length and info of the first multi-byte word in a string.
 *
 * This function retrives the information of the first multi-byte character in the string \a mstr
 * which is \a len bytes long, and returns it through \a word_info. 
 * It also returns the full length of the word including the delimiters after the word.
 *
 * \param log_font The logical font used to parse the multi-byte string.
 * \param mstr The multi-byte string.
 * \param len The length of the string.
 * \param word_info The pointer to a WORDINFO structure used to return the word information.
 * \return The length of the first multi-byte word.
 *
 * \sa GetFirstMCharLen, WORDINFO
 */
MG_EXPORT int GUIAPI GetFirstWord (PLOGFONT log_font, const char* mstr, int len,
                            WORDINFO* word_info);

/**
 * \fn int GUIAPI GetTextExtentPoint (HDC hdc, const char* text, int len, int max_extent, int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
 * \brief Computes the extent of a string when output the string in a limited space.
 *
 * This function computes the extent of the specified string of text \a text which is 
 * \a len bytes long when output the text in a limited space (\a max_extent wide). 
 * If \a pos_chars and \a dx_chars are not NULL, this function will return the 
 * positions of each character in the text, and the output position of each character.
 * This function returns the text extent in a SIZE struct pointed to by \a size, and 
 * the width of text as return value.
 * 
 * \param hdc The device context.
 * \param text The multi-byte string.
 * \param len The length of the string.
 * \param max_extent The width of the limited space.
 * \param fit_chars The number of the characters actually outputed.
 * \param pos_chars The positions of each character in the text will be returned through this pointer.
 * \param dx_chars The output positions of each character in the text will be returned through this pointer.
 * \param size The output extent of the text in the limited space will be returned through this pointer.
 *
 * \return The number of the characters which can be fit to the limited space.
 *
 * \sa GetFirstMCharLen, GetFirstWord
 */
MG_EXPORT int GUIAPI GetTextExtentPoint (HDC hdc, const char* text, int len, int max_extent, 
                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size);


/**
 * \fn int GUIAPI GetTabbedTextExtentPoint (HDC hdc, const char* text, int len, int max_extent, int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size)
 *
 * \brief Computes the extent of a string when output the formatted string in a limited space.
 */
MG_EXPORT int GUIAPI GetTabbedTextExtentPoint (HDC hdc, const char* text, int len, int max_extent,
                                int* fit_chars, int* pos_chars, int* dx_chars, SIZE* size);

    /** @} end of text_parse_fns */

    /**
     * \defgroup text_output_fns Text output functions
     * @{
     */

/**
 * \fn int GUIAPI GetFontHeight (HDC hdc)
 * \brief Retrives the height of the current logical font in a DC.
 *
 * This function retrives the height of the current logical font in the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The height of the current logical font.
 *
 * \sa GetMaxFontWidth
 */
MG_EXPORT int GUIAPI GetFontHeight (HDC hdc);

/**
 * \fn int GUIAPI GetMaxFontWidth (HDC hdc)
 * \brief Retrives the maximal character width of the current logical font in a DC.
 *
 * This function retrives the maximal character width of the current logical font 
 * in the DC \a hdc.
 *
 * \param hdc The device context.
 * \return The maximal character width of the current logical font.
 *
 * \sa GetFontHeight
 */
MG_EXPORT int GUIAPI GetMaxFontWidth (HDC hdc);

/**
 * \fn void GUIAPI GetTextExtent (HDC hdc, const char* spText, int len, SIZE* pSize)
 * \brief Computes the output extent of a string of text.
 *
 * This function computes the output extent of the text (\a spText) with length of \a len. 
 * This function returns the extent in a SIZE struct pointed to by \a pSize and the width 
 * of text as the return value.
 *
 * \param hdc The device context.
 * \param spText The pointer to the string of text.
 * \param len The length of the text.
 * \param pSize The output extent will be returned through this buffer.
 *
 * \sa GetTabbedTextExtent
 */
MG_EXPORT void GUIAPI GetTextExtent (HDC hdc, const char* spText, int len, SIZE* pSize);

/**
 * \fn void GUIAPI GetTabbedTextExtent (HDC hdc, const char* spText, int len, SIZE* pSize)
 * \brief Computes the output extent of a formatted text.
 *
 * This function computes the output extent of the formatted text (\a spText) with length of \a len.
 * This function returns the extent in a SIZE struct pointed to by \a pSize and the width of text as 
 * the return value. The output extent will be affected by the default tab size if there are some 
 * TAB characters in the text.
 *
 * \param hdc The device context.
 * \param spText The pointer to the string of text.
 * \param len The length of the text.
 * \param pSize The output extent will be returned through this buffer.
 *
 * \sa GetTextExtent
 */
MG_EXPORT void GUIAPI GetTabbedTextExtent (HDC hdc, const char* spText, int len, SIZE* pSize);

#ifdef _USE_NEWGAL

/**
 * \def GetTextCharacterExtra(hdc)
 * \brief Retrives the current intercharacter spacing for the DC.
 * 
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a GetTextCharacterExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetTextCharacterExtra
 */
#define GetTextCharacterExtra(hdc)  (int)GetDCAttr (hdc, DC_ATTR_CHAR_EXTRA)

/**
 * \def GetTextAboveLineExtra(hdc)
 * \brief Retrives the current spacing above line for the DC.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a GetTextAboveLineExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetTextAboveLineExtra
 */
#define GetTextAboveLineExtra(hdc)  (int)GetDCAttr (hdc, DC_ATTR_ALINE_EXTRA)

/**
 * \def GetTextBellowLineExtra(hdc)
 * \brief Retrives the current spacing bellow line for the DC.
 *
 * \note Defined as a macro calling \a GetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a GetTextBellowLineExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa SetTextBellowLineExtra
 */
#define GetTextBellowLineExtra(hdc) (int)GetDCAttr (hdc, DC_ATTR_BLINE_EXTRA)

/**
 * \def SetTextCharacterExtra(hdc, extra)
 * \brief Sets the intercharacter spacing for the DC and returns the old spacing value.
 * 
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a SetTextCharacterExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetTextCharacterExtra
 */
#define SetTextCharacterExtra(hdc, extra)   (int)SetDCAttr (hdc, DC_ATTR_CHAR_EXTRA, (DWORD) extra)

/**
 * \def SetTextAboveLineExtra(hdc, extra)
 * \brief Sets the spacing above line for the DC and returns the old value.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a SetTextAboveLineExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetTextAboveLineExtra
 */
#define SetTextAboveLineExtra(hdc, extra)   (int)SetDCAttr (hdc, DC_ATTR_ALINE_EXTRA, (DWORD) extra)

/**
 * \def SetTextBellowLineExtra(hdc, extra)
 * \brief Sets the spacing bellow line for the DC and returns the old value.
 *
 * \note Defined as a macro calling \a SetDCAttr for _USE_NEWGAL.
 *       If _USE_NEWGAL is not defined, \a SetTextBellowLineExtra is defined as
 *       a function, and have the same semantics as this macro.
 *
 * \sa GetTextBellowLineExtra
 */
#define SetTextBellowLineExtra(hdc, extra)  (int)SetDCAttr (hdc, DC_ATTR_BLINE_EXTRA, (DWORD) extra)

#else

MG_EXPORT int GUIAPI GetTextCharacterExtra (HDC hdc);
MG_EXPORT int GUIAPI SetTextCharacterExtra (HDC hdc, int extra);
MG_EXPORT int GUIAPI GetTextAboveLineExtra (HDC hdc);
MG_EXPORT int GUIAPI SetTextAboveLineExtra (HDC hdc, int extra);
MG_EXPORT int GUIAPI GetTextBellowLineExtra (HDC hdc);
MG_EXPORT int GUIAPI SetTextBellowLineExtra (HDC hdc, int extra);

#endif /* _USE_NEWGAL */

/**
 * \fn int GUIAPI TextOutLen (HDC hdc, int x, int y, const char* spText, int len)
 * \brief Outputs a string of text.
 *
 * This function outputs the text \a spText with length of \a len at a (x,y). 
 * This function returns the width of text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The string of the text.
 * \param len The length of the text. If it is less than 0, MiniGUI will treat it as 
 *        a null-terminated string.
 * \return The output width of the text.
 *
 * \sa TabbedTextOutLen
 */
MG_EXPORT int GUIAPI TextOutLen (HDC hdc, int x, int y, const char* spText, int len);

/**
 * \fn int GUIAPI TabbedTextOutLen (HDC hdc, int x, int y, const char* spText, int len)
 * \brief Outputs a formatted text.
 *
 * This function outputs the formatted text \a spText with length of \a len at \a (x,y).
 * This function returns the width of text. The output will be affected by 
 * the default tab size if there are some TAB characters in the text.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The formatted text.
 * \param len The length of the text. If it is less than 0, MiniGUI will treat it as 
 *        a null-terminated string.
 * \return The output width of the text.
 *
 * \sa TextOutLen
 */
MG_EXPORT int GUIAPI TabbedTextOutLen (HDC hdc, int x, int y, const char* spText, int len); 

/**
 * \fn int GUIAPI TabbedTextOutEx (HDC hdc, int x, int y, const char* spText, int nCount, int nTabPositions, int *pTabPositions, int nTabOrigin)
 * \brief Writes a character string at a specified location, expanding tabs to the values
 *        specified in an anrry of tab-stop positions.
 *
 * This function writes the string \a spText with length of \a nCount at a specified locations, 
 * expanding tabs to the value spcified in the array \a pTabPositions of tab-stop positions.
 *
 * \param hdc The device context.
 * \param x x,y: The output start position.
 * \param y x,y: The output start position.
 * \param spText The formatted text.
 * \param nCount The length of the text. If it is less than 0, MiniGUI will treat it as 
 *        a null-terminated string.
 * \param nTabPositions The length of the array of the tab-stop positions.
 * \param pTabPositions The array of the tab-stop positions.
 * \param nTabOrigin The origin of the tab-stops.
 * \return The output width of the text.
 *
 * \sa TabbedTextOutLen
 */
MG_EXPORT int GUIAPI TabbedTextOutEx (HDC hdc, int x, int y, const char* spText, int nCount,
            int nTabPositions, int *pTabPositions, int nTabOrigin);

/**
 * \fn void GUIAPI GetLastTextOutPos (HDC hdc, POINT* pt)
 * \brief Retrives the last text output position.
 *
 * \param hdc The device context.
 * \param pt The last text output position will be returned through this pointer.
 *
 */
MG_EXPORT void GUIAPI GetLastTextOutPos (HDC hdc, POINT* pt);

/**
 * \def TextOut(hdc, x, y, text)
 * \brief Outputs text.
 *
 * Defined as a macro calling \a TextOutLen passing \a len as -1.
 *
 * \sa TextOutLen
 */
#define TextOut(hdc, x, y, text)    TextOutLen (hdc, x, y, text, -1)

/**
 * \def TabbedTextOut(hdc, x, y, text)
 * \brief Outputs formatted text.
 *
 * Defined as a macro calling \a TabbedTextOutLen passing \a len as -1.
 *
 * \sa TextOutLen
 */
#define TabbedTextOut(hdc, x, y, text)  TabbedTextOutLen (hdc, x, y, text, -1)

#define DT_TOP              0x00000000
#define DT_LEFT             0x00000000
#define DT_CENTER           0x00000001
#define DT_RIGHT            0x00000002
#define DT_VCENTER          0x00000004
#define DT_BOTTOM           0x00000008
#define DT_WORDBREAK        0x00000010
#define DT_SINGLELINE       0x00000020
#define DT_EXPANDTABS       0x00000040
#define DT_TABSTOP          0x00000080
#define DT_NOCLIP           0x00000100
#define DT_EXTERNALLPADING  0x00000200
#define DT_CALCRECT         0x00000400
#define DT_NOPREFIX         0x00000800
#define DT_INTERNAL         0x00001000

/**
 * First line information of DrawTextEx2
 */
typedef struct _DTFIRSTLINE
{   
    /** The number of the characters of first line in bytes. */
    int nr_chars;
    /** The output x-coordinate of the fist line. */
    int startx;
    /** The output y-coordinate of the fist line. */
    int starty;
    /** The output width of the fist line. */
    int width;
    /** The output height of the fist line. */
    int height;
} DTFIRSTLINE;

/**
 * \fn int GUIAPI DrawTextEx2 (HDC hdc, const char* pText, int nCount, RECT* pRect, int nIndent, UINT nFormat, DTFIRSTLINE* firstline)
 * \brief Draws a formatted text in a rectangle.
 *
 * This function draws formatted text (\a pText) in the specified rectangle (\a pRect).
 * It formats the text according to the specified method (through \a nFormat, 
 * including expanding tabs, justifying characters, breaking lines, and so forth). 
 *
 * \param hdc The device context.
 * \param pText The formatted text.
 * \param nCount The length of the text. If it is less than 0, MiniGUI will treat it as 
 *        a null-terminated string.
 * \param pRect The output rectangle.
 * \param nIndent The indent value of the first line.
 * \param nFormat The methods used to format the text. MiniGUI support 
 *        the following method to format text so far:
 *
 *  - DT_TOP\n
 *    Top-justifies text (single line only).
 *  - DT_LEFT\n
 *    Aligns text to the left.
 *  - DT_CENTER\n
 *    Centers text horizontally in the rectangle.
 *  - DT_RIGHT\n
 *    Aligns text to the right.
 *  - DT_VCENTER\n
 *    Centers text vertically (single line only).
 *  - DT_BOTTOM\n
 *    Justify the text to the bottom of the rectangle. This value must be combined with DT_SINGLELINE.
 *  - DT_WORDBREAK\n
 *    Break words. Lines are automatically broken beteween words if a word would extend past 
 *    the edge of the rectangle specified by the the pRect parameter. 
 *    A carriage return or linefeed also breaks the line.
 *  - DT_SINGLELINE\n
 *    Display text ont the single line only. Carriage returns and linefeeds do not break the line.
 *  - DT_EXPANDTABS\n
 *    Expands tab characters. The default number of character per tab is eight.
 *  - DT_TABSTOP\n
 *    Set tab stops. Bits 15-8 (hight-order byte of the low-order word) of the \a uForma parameter specify 
 *    the number of characters for each tab. The default number of characters per tab is eight.
 *  - DT_NOCLIP\n
 *    Draws without clipping. \a DrawText is somewhat faster when DT_NOCLIP is used.
 *  - DT_CALCRECT\n
 *    Determines the width and the height of the rectangle. If there are multiple lines of text,  
 *    \a DrawText uses the width of the rectangle pointed to by the \a lpRect parameter and extends 
 *    the base of the rectangle to bound the last line of text. If there is only one line of text, 
 *    \a DrawText modifies the right side of the rectangle so that it bounds 
 *    the last character in the line. In either case, \a DrawText returns the height of 
 *    the formatted text but does not draw the text.
 *
 * \param firstline If not null, DrawTextEx2 will only calculate the first line will be output in the
 *    rectangle, and return the number of the characters and the output width.
 *
 * \return The output height of the formatted text.
 *
 * \sa DrawText
 *
 * Example:
 *
 * \include drawtext.c
 */
MG_EXPORT int GUIAPI DrawTextEx2 (HDC hdc, const char* pText, int nCount, 
                RECT* pRect, int nIndent, UINT nFormat, DTFIRSTLINE *firstline);

/**
 * \def DrawText(hdc, text, n, rc, format)
 * \brief Draws a formatted text in a rectangle.
 *
 * Defined as a macro calling \a DrawTextEx2 passing \a nIndent as 0 and \a firstline as NULL.
 *
 * \sa DrawTextEx2
 */
#define DrawText(hdc, text, n, rc, format)      DrawTextEx2 (hdc, text, n, rc, 0, format, NULL)

/**
 * \def DrawTextEx(hdc, text, n, rc, indent, format)
 * \brief Draws a formatted text in a rectangle.
 *
 * Defined as a macro calling \a DrawTextEx2 passing \a firstline as NULL.
 *
 * \sa DrawTextEx2
 */
#define DrawTextEx(hdc, text, n, rc, indent, format)      DrawTextEx2 (hdc, text, n, rc, indent, format, NULL)

    /** @} end of text_output_fns */

    /**
     * \defgroup bmp_struct Bitmap structure
     *
     * MiniGUI uses a MYBITMAP structure to represent a device-independent bitmap,
     * and BITMAP structure to represent a device-dependent bitmap.
     *
     * Note that if _USE_NEWGAL is not defined, the BITMAP structure 
     * has a different definition:
     *
     * \code
     * struct _BITMAP
     * {
     *      Uint8   bmType;
     *      Uint8   bmBitsPerPixel;
     *      Uint8   bmBytesPerPixel;
     *      Uint8   bmReserved;
     *
     *      Uint32  bmColorKey;
     *
     *      Uint32  bmWidth;
     *      Uint32  bmHeight;
     *      Uint32  bmPitch;
     *
     *      void*   bmBits;
     *      Uint32  bmReserved1;
     * };
     * \endcode
     *
     * @{
     */

#define MYBMP_TYPE_NORMAL       0x00000000
#define MYBMP_TYPE_RLE4         0x00000001
#define MYBMP_TYPE_RLE8         0x00000002
#define MYBMP_TYPE_RGB          0x00000003
#define MYBMP_TYPE_BGR          0x00000004
#define MYBMP_TYPE_RGBA         0x00000005
#define MYBMP_TYPE_MASK         0x0000000F

#define MYBMP_FLOW_DOWN         0x00000010
#define MYBMP_FLOW_UP           0x00000020
#define MYBMP_FLOW_MASK         0x000000F0

#define MYBMP_TRANSPARENT       0x00000100
#define MYBMP_ALPHACHANNEL      0x00000200
#define MYBMP_ALPHA             0x00000400

#define MYBMP_RGBSIZE_3         0x00001000
#define MYBMP_RGBSIZE_4         0x00002000

#define MYBMP_LOAD_GRAYSCALE    0x00010000
#define MYBMP_LOAD_NONE         0x00000000

/** Device-independent bitmap structure. */
struct _MYBITMAP
{
    /**
     * Flags of the bitmap, can be OR'ed by the following values:
     *  - MYBMP_TYPE_NORMAL\n
     *    A normal palette bitmap.
     *  - MYBMP_TYPE_RGB\n
     *    A RGB bitmap.
     *  - MYBMP_TYPE_BGR\n
     *    A BGR bitmap.
     *  - MYBMP_TYPE_RGBA\n
     *    A RGBA bitmap.
     *  - MYBMP_FLOW_DOWN\n
     *    The scanline flows from top to bottom.
     *  - MYBMP_FLOW_UP\n
     *    The scanline flows from bottom to top.
     *  - MYBMP_TRANSPARENT\n
     *    Have a trasparent value.
     *  - MYBMP_ALPHACHANNEL\n
     *    Have a alpha channel.
     *  - MYBMP_ALPHA\n
     *    Have a per-pixel alpha value.
     *  - MYBMP_RGBSIZE_3\n
     *    Size of each RGB triple is 3 bytes.
     *  - MYBMP_RGBSIZE_4\n
     *    Size of each RGB triple is 4 bytes.
     *  - MYBMP_LOAD_GRAYSCALE\n
     *    Tell bitmap loader to load a grayscale bitmap.
     */
    DWORD flags;
    /** The number of the frames. */
    int   frames;
    /** The pixel depth. */
    Uint8 depth;
    /** The alpha channel value. */
    Uint8 alpha;
    Uint8 reserved [2];
    /** The transparent pixel. */
    Uint32 transparent;

    /** The width of the bitmap. */
    Uint32 w;
    /** The height of the bitmap. */
    Uint32 h;
    /** The pitch of the bitmap. */
    Uint32 pitch;
    /** The size of the bits of the bitmap. */
    Uint32 size;

    /** The pointer to the bits of the bitmap. */
    BYTE* bits;
};

#ifdef _USE_NEWGAL

#define BMP_TYPE_NORMAL         0x00
#define BMP_TYPE_RLE            0x01
#define BMP_TYPE_ALPHA          0x02
#define BMP_TYPE_ALPHACHANNEL   0x04
#define BMP_TYPE_COLORKEY       0x10
#define BMP_TYPE_PRIV_PIXEL     0x20

#ifdef _FOR_MONOBITMAP
  #define BMP_TYPE_MONOKEY      0x40
#endif

/** Expanded device-dependent bitmap structure. */
struct _BITMAP
{
    /**
     * Bitmap types, can be OR'ed by the following values:
     *  - BMP_TYPE_NORMAL\n
     *    A nomal bitmap, without alpha and color key.
     *  - BMP_TYPE_RLE\n
     *    A RLE encoded bitmap, not used so far.
     *  - BMP_TYPE_ALPHA\n
     *    Per-pixel alpha in the bitmap.
     *  - BMP_TYPE_ALPHACHANNEL\n
     *    The \a bmAlpha is a valid alpha channel value.
     *  - BMP_TYPE_COLORKEY\n
     *    The \a bmColorKey is a valid color key value.
     *  - BMP_TYPE_PRIV_PIXEL\n
     *    The bitmap have a private pixel format.
     */
    Uint8   bmType;
    /** The bits per piexel. */
    Uint8   bmBitsPerPixel;
    /** The bytes per piexel. */
    Uint8   bmBytesPerPixel;
    /** The alpha channel value. */
    Uint8   bmAlpha;
    /** The color key value. */
    Uint32  bmColorKey;
#ifdef _FOR_MONOBITMAP
    Uint32  bmColorRep;
#endif

    /** The width of the bitmap */
    Uint32  bmWidth;
    /** The height of the bitmap */
    Uint32  bmHeight;
    /** The pitch of the bitmap */
    Uint32  bmPitch;
    /** The bits of the bitmap */
    Uint8*  bmBits;

    /** The private pixel format */
    void*   bmAlphaPixelFormat;
};

#else

#define BMP_TYPE_NORMAL         0x00
#define BMP_TYPE_COLORKEY       0x01

/* expanded bitmap struct */
struct _BITMAP
{
    Uint8   bmType;
    Uint8   bmBitsPerPixel;
    Uint8   bmBytesPerPixel;
    Uint8   bmReserved;

    Uint32  bmColorKey;

    Uint32  bmWidth;
    Uint32  bmHeight;
    Uint32  bmPitch;

    void*   bmBits;
    Uint32  bmReserved1;
};

#endif /* _USE_NEWGAL */

    /** @} end of bmp_struct */

    /**
     * \defgroup bmp_load_fns Bitmap file load/save operations
     * @{
     *
     * Example:
     *
     * \include bitmap.c
     */

#include <stdio.h>
#include "endianrw.h"

/**
 * \fn BOOL GUIAPI RegisterBitmapFileType (const char *ext, int (*load) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal), int (*save) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal), BOOL (*check) (MG_RWops* fp))
 *
 * \brief Registers a bitmap file loader, saver, and checker.
 *
 * This function registers a new bitmap file loader, saver, and checker. 
 * You should pass the extension of the bitmap files, the function to 
 * load this type of bitmap file, the function to save, and the 
 * function to check the type.
 *
 * \param ext The extension name of the type of bitmap file, like "jpg" or "gif".
 * \param load The loader of the bitmap file.
 * \param save The saver of the bitmap file, can be NULL.
 * \param check The checker of the bitmap file.
 * \return TRUE on success, FALSE on error.
 *
 * \sa CheckBitmapType, LoadBitmapEx, general_rw_fns
 */
MG_EXPORT BOOL GUIAPI RegisterBitmapFileType (const char *ext,
            int (*load) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal),
            int (*save) (MG_RWops* fp, MYBITMAP *bmp, RGB *pal),
            BOOL (*check) (MG_RWops* fp));

/**
 * \fn const char* GUIAPI CheckBitmapType (MG_RWops* fp)
 * \brief Checks the type of the bitmap in a data source.
 * 
 * This function checks the type of the bitmap in the data source \a fp,
 * and returns the extension of this type of bitmap file.
 *
 * \param fp The pointer to the data source.
 * \return The extension of the type of bitmap file. NULL for not recongnized bitmap type.
 *
 * \sa RegisterBitmapFileType
 */
MG_EXPORT const char* GUIAPI CheckBitmapType (MG_RWops* fp);

#define ERR_BMP_OK              0
#define ERR_BMP_IMAGE_TYPE      -1
#define ERR_BMP_UNKNOWN_TYPE    -2
#define ERR_BMP_CANT_READ       -3
#define ERR_BMP_CANT_SAVE       -4
#define ERR_BMP_NOT_SUPPORTED   -5   
#define ERR_BMP_MEM             -6
#define ERR_BMP_LOAD            -7
#define ERR_BMP_FILEIO          -8
#define ERR_BMP_OTHER           -9
#define ERR_BMP_ERROR_SOURCE    -10

/**
 * \fn int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP pBitmap, MG_RWops* area, const char* ext)
 * \brief Loads a device-dependent bitmap from a general data source.
 * 
 * This function loads a device-dependent bitmap from the data source \a area.
 * 
 * \param hdc The device context.
 * \param pBitmap The pointer to the BITMAP object.
 * \param area The data source.
 * \param ext The extension of the type of this bitmap.
 * \return 0 on success, less than 0 on error.
 *
 * \retval ERR_BMP_OK Loading successfully
 * \retval ERR_BMP_IMAGE_TYPE Not a valid bitmap.
 * \retval ERR_BMP_UNKNOWN_TYPE Not recongnized bitmap type.
 * \retval ERR_BMP_CANT_READ Read error.
 * \retval ERR_BMP_CANT_SAVE Save error.
 * \retval ERR_BMP_NOT_SUPPORTED Not supported bitmap type.
 * \retval ERR_BMP_MEM Memory allocation error.
 * \retval ERR_BMP_LOAD Loading error.
 * \retval ERR_BMP_FILEIO I/O failed.
 * \retval ERR_BMP_OTHER Other error.
 * \retval ERR_BMP_ERROR_SOURCE A error data source.
 *
 * \sa LoadBitmapFromFile, LoadBitmapFromMemory
 */
MG_EXPORT int GUIAPI LoadBitmapEx (HDC hdc, PBITMAP pBitmap, MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap, const char* spFileName)
 * \brief Loads a device-dependent bitmap from a file.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadBitmapFromFile (HDC hdc, PBITMAP pBitmap, const char* spFileName);

/**
 * \def LoadBitmap
 * \brief An alias of \a LoadBitmapFromFile.
 *
 * \sa LoadBitmapFromFile
 */
#define LoadBitmap  LoadBitmapFromFile

/**
 * \fn int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap, const void* mem, int size, const char* ext)
 * \brief Loads a device-dependent bitmap from memory.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadBitmapFromMem (HDC hdc, PBITMAP pBitmap, const void* mem, int size, const char* ext);

#define LoadBitmapFromMemory LoadBitmapFromMem

/**
 * \fn void GUIAPI UnloadBitmap (PBITMAP pBitmap)
 * \brief Unloads a bitmap.
 *
 * This function unloads the specified bitmap \a pBitmap.
 * It will free the private pixel format and the bits of the bitmap.
 *
 * \param pBitmap The BITMAP object.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT void GUIAPI UnloadBitmap (PBITMAP pBitmap);

/**
 * \fn void GUIAPI ReplaceBitmapColor (HDC hdc, PBITMAP pBitmap, gal_pixel iOColor, gal_pixel iNColor)
 * \brief Replaces a specific pixels in a bitmap with another pixel.
 *
 * This function replaces the specific pixels with value \a iOColor with the other pixel value \a iNcolor
 * in the bitmap \a pBitmap.
 *
 * \param hdc The device context.
 * \param pBitmap The BITMAP object.
 * \param iOColor The pixel value of the color will be replaced.
 * \param iNColor The pixel value of the new color.
 *
 */
MG_EXPORT void GUIAPI ReplaceBitmapColor (HDC hdc, PBITMAP pBitmap, gal_pixel iOColor, gal_pixel iNColor);

/**
 * \fn int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, MG_RWops* area, const char* ext)
 * \brief Loads a MYBITMAP object from a data source.
 *
 * This function loads a MYBITMAP object from the data source \a area.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param area The data source.
 * \param pal The palette will be returned.
 * \param ext The extension of the type of this bitmap.
 * \return 0 on success, less than 0 on error.
 *
 * \sa LoadBitmapEx
 */
MG_EXPORT int GUIAPI LoadMyBitmapEx (PMYBITMAP my_bmp, RGB* pal, MG_RWops* area, const char* ext);

/**
 * \fn int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal, const char* file_name)
 * \brief Loads a MYBITMAP object from a file.
 *
 * \sa LoadMyBitmapEx
 */
MG_EXPORT int GUIAPI LoadMyBitmapFromFile (PMYBITMAP my_bmp, RGB* pal, const char* file_name);

/**
 * \def LoadMyBitmap
 * \brief Alias of \a LoadMyBitmapFromFile.
 *
 * \sa LoadMyBitmapFromFile
 */
#define LoadMyBitmap    LoadMyBitmapFromFile

/**
 * \fn int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal, const void* mem, int size, const char* ext)
 * \brief Loads a MYBITMAP object from memory.
 *
 * This function loads a MYBITMAP object from memory.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 * \param pal The palette will be retruned through this pointer.
 * \param mem The pointer to the memory area.
 * \param size The size of the memory area.
 * \param ext The extension name used to determine the type of the bitmap.
 *
 * \sa LoadMyBitmapEx, MYBITMAP
 */
MG_EXPORT int GUIAPI LoadMyBitmapFromMem (PMYBITMAP my_bmp, RGB* pal, const void* mem, int size, const char* ext);

#define LoadMyBitmapFromMemory LoadMyBitmapFromMem

/**
 * \fn void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp)
 * \brief Unloads a bitmap.
 *
 * This function unloads the specified MYBITMAP object \a my_bmp.
 * It will free the bits of the bitmap.
 *
 * \param my_bmp The pointer to the MYBITMAP object.
 *
 * \sa LoadMyBitmapEx
 */
MG_EXPORT void GUIAPI UnloadMyBitmap (PMYBITMAP my_bmp);

#ifdef _SAVE_BITMAP

/**
 * \fn int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal, const char* spFileName)
 * \brief Saves a MYBITMAP object to a bitmap file.
 * 
 * This function saves the MYBITMAP object \a my_bmp to the bitmap file named \a spFileName.
 *
 * \param my_bmp The MYBITMAP object.
 * \param pal The palette.
 * \param spFileName The file name.
 * \return 0 on success, less than 0 on error.
 *
 * \sa SaveBitmapToFile
 */
MG_EXPORT int GUIAPI SaveMyBitmapToFile (PMYBITMAP my_bmp, RGB* pal, const char* spFileName);

/**
 * \fn int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP pBitmap, const char* spFileName)
 * \brief Saves a BITMAP object to a bitmap file.
 * 
 * This function saves the BITMAP object \a pBitmap to the bitmap file named \a spFileName.
 *
 * \param hdc The device context.
 * \param pBitmap The BITMAP object.
 * \param spFileName The file name.
 * \return 0 on success, less than 0 on error.
 *
 * \sa SaveMyBitmapToFile
 */
MG_EXPORT int GUIAPI SaveBitmapToFile (HDC hdc, PBITMAP pBitmap, const char* spFileName);
#define SaveBitmap  SaveBitmapToFile 
#endif

/**
 * \fn BOOL GUIAPI InitBitmap (HDC hdc, Uint32 w, Uint32 h, Uint32 pitch, BYTE* bits, PBITMAP bmp)
 * \brief Initializes a BITMAP object as a normal bitmap.
 *
 * This function initializes the bitmap pointed to by \a bmp as a normal bitmap.
 * It sets the bitmap structure fields, and allocates the bits if \a bits is NULL.
 *
 * \param hdc The device context.
 * \param w The width of the bitmap.
 * \param h The height of the bitmap.
 * \param pitch The pitch of the bitmap.
 * \param bits The bits of the bitmap.
 * \param bmp The BITMAP object to be initialized.
 * \return TRUE on success, FALSE on error.
 *
 * \note \a LoadBitmapEx will initialize the BITMAP object itself.
 *
 * \sa InitBitmapPixelFormat, UnloadBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI InitBitmap (HDC hdc, Uint32 w, Uint32 h, Uint32 pitch, BYTE* bits, PBITMAP bmp);

#ifdef _USE_NEWGAL

/**
 * \fn BOOL GUIAPI InitBitmapPixelFormat (HDC hdc, PBITMAP bmp)
 * \brief Initializes the bitmap pixel format information of a BITMAP object.
 *
 * This function initializes the bitmap pixel format information of the BITMAP object
 * pointed to by \a bmp. This includes \a bmBitsPerPixel and \a bmBytesPerPixel fields,
 * and the private pixel format if the bitmap is a bitmap with alpha.
 *
 * \param hdc The device context.
 * \param bmp The BITMAP object to be initialized.
 * \return TRUE on success, FALSE on error.
 *
 * \sa InitBitmap, bmp_struct
 */
MG_EXPORT BOOL GUIAPI InitBitmapPixelFormat (HDC hdc, PBITMAP bmp);

/**
 * \fn void GUIAPI DeleteBitmapAlphaPixel (PBITMAP bmp)
 * \brief Deletes the bitmap alpha pixel format information of a BITMAP object.
 *
 * This function deletes the bitmap alpha pixel format information of the BITMAP object \a bmp.
 *
 * \sa InitBitmapPixelFormat
 */
MG_EXPORT void GUIAPI DeleteBitmapAlphaPixel (PBITMAP bmp);

/**
 * \fn int GUIAPI ExpandMyBitmap (HDC hdc, PBITMAP bmp, const MYBITMAP* my_bmp, const RGB* pal, int frame)
 * \brief Expands a MYBITMAP object to a BITMAP object.
 *
 * This function expands the MYBITMAP object pointed to by \a my_bmp
 * to a BITMAP object (\a bmp).
 *
 * \param hdc The device context.
 * \param bmp The expanded BITMAP object.
 * \param my_bmp The MYBITMAP object to be expanded.
 * \param pal The palette of the MYBITMAP object.
 * \param frame The frame of the MYBITMAP object.
 * \return 0 on success, non-zero on error.
 */
MG_EXPORT int GUIAPI ExpandMyBitmap (HDC hdc, PBITMAP bmp, const MYBITMAP* my_bmp, const RGB* pal, int frame);

MG_EXPORT void GUIAPI ExpandMonoBitmap (HDC hdc, BYTE* bits, Uint32 pitch, const BYTE* my_bits, Uint32 my_pitch, 
                Uint32 w, Uint32 h, DWORD flags, Uint32 bg, Uint32 fg);
MG_EXPORT void GUIAPI Expand16CBitmap (HDC hdc, BYTE* bits, Uint32 pitch, const BYTE* my_bits, Uint32 my_pitch, 
                Uint32 w, Uint32 h, DWORD flags, const RGB* pal);
MG_EXPORT void GUIAPI Expand256CBitmap (HDC hdc, BYTE* bits, Uint32 pitch, const BYTE* my_bits, Uint32 my_pitch, 
                Uint32 w, Uint32 h, DWORD flags, const RGB* pal);
MG_EXPORT void GUIAPI CompileRGBABitmap (HDC hdc, BYTE* bits, Uint32 pitch, const BYTE* my_bits, Uint32 my_pitch, 
                Uint32 w, Uint32 h, DWORD flags, void* pixel_format);

#define CompileRGBBitmap CompileRGBABitmap 

#else

MG_EXPORT int GUIAPI ExpandMyBitmap (HDC hdc, const MYBITMAP* pMyBitmap, const RGB* pal, PBITMAP pBitmap);
MG_EXPORT void GUIAPI ExpandMonoBitmap (HDC hdc, int w, int h, const BYTE* my_bits, int my_pitch, int bits_flow, 
                                BYTE* bitmap, int pitch, int bg, int fg);
MG_EXPORT void GUIAPI Expand16CBitmap (HDC hdc, int w, int h, const BYTE* my_bits, int my_pitch, int bits_flow,
                                BYTE* bitmap, int pitch, const RGB* pal);
MG_EXPORT void GUIAPI Expand256CBitmap (HDC hdc, int w, int h, const BYTE* my_bits, int my_pitch, int bits_flow,
                                BYTE* bitmap, int pitch, const RGB* pal);
MG_EXPORT void GUIAPI CompileRGBBitmap (HDC hdc, int w, int h, const BYTE* my_bits, int my_pitch, int bits_flow,
                             BYTE* bitmap, int pitch, int rgb_order);

#endif /* _USE_NEWGAL */

    /** @} end of bmp_load_fns */

    /** @} end of gdi_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* _MGUI_GDI_H */


