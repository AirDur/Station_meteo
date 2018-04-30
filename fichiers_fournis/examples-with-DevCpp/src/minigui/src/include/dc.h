/*
** $Id: dc.h,v 1.51 2004/07/31 08:48:24 weiym Exp $
**
** dc.h: this head file declares all internal types and data of GDI module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create Date: 1999/xx/xx
*/

#ifndef _DC_H
    #define _DC_H
    
#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifdef __uClinux__
    #define DCSLOTNUMBER        2
#else
  #ifdef _LITE_VERSION
    #define DCSLOTNUMBER        8
  #else
    #define DCSLOTNUMBER        16
  #endif
#endif

#ifdef _LITE_VERSION

#define INIT_LOCK(lock, attr)
#define LOCK(lock)
#define UNLOCK(lock)
#ifdef _STAND_ALONE
#define LOCK_GCRINFO(pdc)
#define UNLOCK_GCRINFO(pdc)
#else
#define LOCK_GCRINFO(pdc)       if (!mgIsServer) lock_scr_sem ()
#define UNLOCK_GCRINFO(pdc)     if (!mgIsServer && dc_IsGeneralDC(pdc)) unlock_scr_sem ()
#endif
#define DESTROY_LOCK(lock)

#else

#define INIT_LOCK(lock, attr)   pthread_mutex_init(lock, attr)
#define LOCK(lock)              pthread_mutex_lock(lock)
#define UNLOCK(lock)            pthread_mutex_unlock(lock)
#define LOCK_GCRINFO(pdc)       pthread_mutex_lock (&pdc->pGCRInfo->lock)
#define UNLOCK_GCRINFO(pdc)     if (dc_IsGeneralDC(pdc)) pthread_mutex_unlock (&pdc->pGCRInfo->lock)
#define DESTROY_LOCK(lock)      pthread_mutex_destroy(lock)

#endif

struct tagDC;
typedef struct tagDC DC;
typedef struct tagDC* PDC;

#ifdef _USE_NEWGAL

typedef void (* DC_SET_PIXEL) (PDC pdc);
typedef void (* DC_DRAW_HLINE) (PDC pdc, int w);
typedef void (* DC_PUT_HLINE) (PDC pdc, Uint8* src, int w);
typedef void (* DC_STEP_X) (PDC pdc, int step);
typedef void (* DC_MOVE_TO) (PDC pdc, int x, int y);

#endif /* _USE_NEWGAL */

// Device Context
struct tagDC
{
    short DataType;  /* the data type, always be TYPE_HDC */
    short DCType;    /* the dc type */
        
    BOOL inuse;
    HWND hwnd;

#ifdef _USE_NEWGAL
    /* surface of this DC */
    GAL_Surface* surface;
#else
    /* graphics context */
    GAL_GC gc;
#endif

    /* background color and mode */
    gal_pixel bkcolor;
    int bkmode;

    /* pen color */
    gal_pixel pencolor;

    /* solid brush color */
    gal_pixel brushcolor;

    /* text attibutes */
    gal_pixel textcolor;
    int tabstop;

    int cExtra;     /* Character extra */
    int alExtra;    /* Above line extra */
    int blExtra;    /* Bellow line extra */

    int mapmode;    /* mappping mode */

#ifdef _ADV_2DAPI
    /* pen attributes */
    int pen_type;
    int pen_cap_style;
    int pen_join_style;
    unsigned int pen_width;

    /* brush attributes */
    int brush_type;

    POINT brush_orig;
    const BITMAP* brush_tile;
    const STIPPLE* brush_stipple;

    /* custom dash info */
    int dash_offset;
    const unsigned char* dash_list;
    size_t dash_list_len;
#endif

    POINT CurPenPos;
    POINT CurTextPos;

    PLOGFONT pLogFont;

    POINT ViewOrig;
    POINT ViewExtent;
    POINT WindowOrig;
    POINT WindowExtent;

#ifdef _USE_NEWGAL
    /* raster operation */
    int rop;
    int step;

    /* context information */
    BYTE* cur_dst;
    gal_pixel skip_pixel;
    gal_pixel cur_pixel;
    CLIPRECT* cur_ban;
    RECT rc_output;

    /* pixel and line operation */
    DC_SET_PIXEL set_pixel;
    DC_DRAW_HLINE draw_hline;
    DC_PUT_HLINE put_hline;
    DC_MOVE_TO move_to;
    DC_STEP_X step_x;
#endif /* _USE_NEWGAL */
    
    /* context information */
    /* clip region information */
    CLIPRGN  lcrgn;
    CLIPRGN  ecrgn;

    /* device rect */
    BOOL bIsClient;
    RECT DevRC;

    PGCRINFO pGCRInfo;
    int      oldage;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    int      scr_sig;
#endif
};

extern DC __mg_screen_dc;

// This function convert HDC to PDC.
static inline PDC dc_HDC2PDC (HDC hdc)
{
    if (hdc == HDC_SCREEN) return &__mg_screen_dc;
    return (PDC) hdc;
}

static inline BOOL dc_IsMemHDC (HDC hdc)
{
    if (hdc == HDC_SCREEN) return FALSE;
    return (((PDC)hdc)->DCType == TYPE_MEMDC);
}

static inline BOOL dc_IsScreenHDC (HDC hdc)
{
    return (hdc == HDC_SCREEN);
}

static inline BOOL dc_IsGeneralHDC (HDC hdc)
{
    if (hdc == HDC_SCREEN) return FALSE;
    return (((PDC)hdc)->DCType == TYPE_GENDC);
}

static inline BOOL dc_IsMemDC (PDC pdc)
{
    return (pdc->DCType == TYPE_MEMDC);
}

static inline BOOL dc_IsScreenDC (PDC pdc)
{
    return (pdc->DCType == TYPE_SCRDC);
}

static inline BOOL dc_IsGeneralDC (PDC pdc)
{
    return (pdc->DCType == TYPE_GENDC);
}

static inline BOOL dc_IsVisible (PDC pdc)
{
    PCONTROL pCtrl;
    
    if (pdc->DCType != TYPE_GENDC)
        return TRUE;
    
    if (pdc->hwnd == HWND_DESKTOP)
        return TRUE;
 
    pCtrl = (PCONTROL)(pdc->hwnd);

    do {
        if (!(pCtrl->dwStyle & WS_VISIBLE))
            return FALSE;

        pCtrl = pCtrl->pParent;
    } while (pCtrl);

    return TRUE;
}

static inline void coor_DP2SP(PDC pdc, int* x, int* y)
{
    if (pdc->DCType != TYPE_GENDC)
        return;

    *x += pdc->DevRC.left;
    *y += pdc->DevRC.top;
}

static inline void coor_SP2DP(PDC pdc, int* x, int* y)
{
    if (pdc->DCType != TYPE_GENDC)
        return;

    *x -= pdc->DevRC.left;
    *y -= pdc->DevRC.top;
}

static inline void coor_LP2SP(PDC pdc, int* x, int* y)
{
    if (pdc == &__mg_screen_dc)
        return;

    if (pdc->mapmode == MM_TEXT) {
        *x += pdc->DevRC.left;
        *y += pdc->DevRC.top;
    }
    else {
        *x = pdc->DevRC.left + (*x - pdc->WindowOrig.x)
             * pdc->ViewExtent.x / pdc->WindowExtent.x
             + pdc->ViewOrig.x;

        *y = pdc->DevRC.top + (*y - pdc->WindowOrig.y)
             * pdc->ViewExtent.y / pdc->WindowExtent.y
             + pdc->ViewOrig.y;
    }
}

static inline void coor_SP2LP(PDC pdc, int* x, int* y)
{
    if (pdc == &__mg_screen_dc)
        return;

    *x -= pdc->DevRC.left;
    *y -= pdc->DevRC.top;

    if (pdc->mapmode != MM_TEXT) {
        *x = (*x - pdc->ViewOrig.x)
             * pdc->WindowExtent.x / pdc->ViewExtent.x
             + pdc->WindowOrig.x;

        *y = (*y - pdc->ViewOrig.y)
             * pdc->WindowExtent.y / pdc->ViewExtent.y
             + pdc->WindowOrig.y;
    }
}

static inline void extent_x_LP2SP (PDC pdc, int* extent)
{
    if (pdc == &__mg_screen_dc || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->ViewExtent.x / pdc->WindowExtent.x;
}

static inline void extent_x_SP2LP (PDC pdc, int* extent)
{
    if (pdc == &__mg_screen_dc || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->WindowExtent.x / pdc->ViewExtent.x;
}

static inline void extent_y_LP2SP (PDC pdc, int* extent)
{
    if (pdc == &__mg_screen_dc || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->ViewExtent.y / pdc->WindowExtent.y;
}

static inline void extent_y_SP2LP (PDC pdc, int* extent)
{
    if (pdc == &__mg_screen_dc || pdc->mapmode == MM_TEXT)
        return;

    *extent = *extent * pdc->WindowExtent.y / pdc->ViewExtent.y;
}

static inline gal_pixel _mem_get_pixel (Uint8 *dst, int bpp)
{
    switch (bpp) {
        case 1:
            return *dst;
        case 2:
            return *(Uint16*)dst;
        case 3:
        {
            Uint32* p = (Uint32*)(dst);
            return *p & 0x00FFFFFF;
        }
        case 4:
            return *(Uint32*)dst;
    }

    return 0;
}

static inline BYTE* _mem_set_pixel (BYTE* dst, int bpp, Uint32 pixel)
{
    switch (bpp) {
        case 1:
                *dst= pixel;
                dst++;
                break;
        case 2:
                *(Uint16 *) dst = pixel;
                dst += 2;
                break;
        case 3:
                *(Uint16 *) dst = pixel;
                *(dst + 2) = pixel >> 16;
                dst += 3;
                break;
        case 4:
                *(Uint32 *) dst = pixel;
                dst += 4;
                break;
    }

    return dst;
}

extern BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);
extern PDC __mg_check_ecrgn (HDC hdc);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
#   include "ourhdr.h"
#   include "client.h"
#   include "sharedres.h"
#   include "drawsemop.h"
#endif

#ifdef _USE_NEWGAL

static inline BYTE* _dc_get_dst (PDC pdc, int x, int y)
{
    BYTE* dst = (BYTE*)pdc->surface->pixels + pdc->surface->pitch * y;
    switch (pdc->surface->format->BytesPerPixel) {
            case 1:
                    dst += x;
                    break;
            case 2:
                    dst += x<<1;
                    break;
            case 3:
                    dst += (x<<1) + x;
                    break;
            case 4:
                    dst += x<<2;
                    break;
    }
    return dst;
}

static inline void _dc_step_y (PDC pdc, int step)
{
    pdc->cur_dst += pdc->surface->pitch * step;
}

#undef _REGION_DEBUG

#ifdef _REGION_DEBUG
void dumpRegion (CLIPRGN* region);
#endif

#ifdef _LITE_VERSION

#ifdef _STAND_ALONE

#define BLOCK_DRAW_SEM(pdc)
#define UNBLOCK_DRAW_SEM(pdc)

#define CHECK_DRAWING(pdc)      \
        (__mg_switch_away && pdc->surface == __gal_screen)

#define CHECK_CLI_SCREEN(pdc, rcOutput) FALSE

#else

#define BLOCK_DRAW_SEM(pdc)     \
        if (!mgIsServer && pdc->surface == __gal_screen) lock_draw_sem ()
#define UNBLOCK_DRAW_SEM(pdc)   \
        if (!mgIsServer && pdc->surface == __gal_screen) unlock_draw_sem ()

#define CHECK_DRAWING(pdc)      \
        (((!mgIsServer && (SHAREDRES_TOPMOST_LAYER != __mg_layer)) \
            || __mg_switch_away) && pdc->surface == __gal_screen)

#define CHECK_CLI_SCREEN(pdc, rcOutput)  FALSE

#endif /* _STAND_ALONE */

#else

#define BLOCK_DRAW_SEM(pdc)
#define UNBLOCK_DRAW_SEM(pdc)

#endif /* _LITE_VERSION */

int __mg_enter_drawing (PDC pdc);
void __mg_enter_drawing_nocheck (PDC pdc);
void __mg_leave_drawing (PDC pdc);

#define ENTER_DRAWING(pdc)      \
    if (__mg_enter_drawing(pdc))     \
        goto leave_drawing

#define LEAVE_DRAWING(pdc)      \
    __mg_leave_drawing(pdc);         \
leave_drawing:

#define ENTER_DRAWING_NOCHECK(pdc) __mg_enter_drawing_nocheck(pdc)

#define LEAVE_DRAWING_NOCHECK(pdc) __mg_leave_drawing(pdc)

#define SET_GAL_CLIPRECT(pdc, cliprect)                             \
    pdc->surface->clip_rect.x = cliprect.left;                      \
    pdc->surface->clip_rect.y = cliprect.top;                       \
    pdc->surface->clip_rect.w = cliprect.right - cliprect.left;     \
    pdc->surface->clip_rect.h = cliprect.bottom - cliprect.top

#endif /* _USE_NEWGAL */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // _DC_H 

