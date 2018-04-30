/*
** $Id: sharedres.h,v 1.12 2003/12/11 05:07:04 weiym Exp $
**
** sharedres.h: structure of shared resource.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SHAREDRES_H
    #define GUI_SHAREDRES_H

#include <sys/time.h>
#include <sys/termios.h>

#define MAX_SRV_CLIP_RECTS      8

#define _NR_SEM         2
#define _IDX_SEM_DRAW   0
#define _IDX_SEM_SCR    1

#ifdef _CURSOR_SUPPORT

#undef _NR_SEM
#define _NR_SEM             4
#define _IDX_SEM_CURSOR     2
#define _IDX_SEM_HIDECOUNT  3

#endif /* _IDX_SEM_CURSOR */

/* Information for Server Clipping Rectangles */
typedef struct tagSCRInfo {
    unsigned int  signature;
    int           nr_crc;
    RECT          clip_rects [MAX_SRV_CLIP_RECTS];
} SCRINFO;

typedef SCRINFO* PSCRINFO;

typedef struct tagG_RES {
    int semid;
    int shmid;
    GHANDLE topmost_layer;

#if 0
    RECT layer_rc;
    int cli_scr_lx;
    int cli_scr_ty;
    int cli_scr_rx;
    int cli_scr_by;
#else
    SCRINFO scr_info;
#endif

    unsigned int timer_counter;
    unsigned int tick_on_locksem;
    struct timeval timeout;
    struct termios savedtermio;
    int mousex, mousey;
    int mousebutton;
    int shiftstatus;
    
#ifdef _CURSOR_SUPPORT
    int cursorx, cursory;
    int oldboxleft, oldboxtop;
    HCURSOR csr_current;
    int xhotspot, yhotspot;
	int csr_show_count;
#endif

#ifdef _CURSOR_SUPPORT
	int csrnum;
#endif
	int iconnum;
	int bmpnum;
	int sysfontnum;
	int rbffontnum;
	int varfontnum;

#ifdef _CURSOR_SUPPORT
	unsigned long svdbitsoffset;
	unsigned long csroffset;
#endif
	unsigned long iconoffset;
	unsigned long sfontoffset;
	unsigned long rfontoffset;
	unsigned long vfontoffset;
	unsigned long bmpoffset;

} G_RES;
typedef G_RES* PG_RES;

#define SHAREDRES_TIMER_COUNTER (((PG_RES)mgSharedRes)->timer_counter)
#define SHAREDRES_TICK_ON_LOCKSEM  (((PG_RES)mgSharedRes)->tick_on_locksem)
#define SHAREDRES_TIMEOUT       (((PG_RES)mgSharedRes)->timeout)
#define SHAREDRES_TERMIOS       (((PG_RES)mgSharedRes)->savedtermio)
#define SHAREDRES_MOUSEX        (((PG_RES)mgSharedRes)->mousex)
#define SHAREDRES_MOUSEY        (((PG_RES)mgSharedRes)->mousey)
#define SHAREDRES_BUTTON        (((PG_RES)mgSharedRes)->mousebutton)
#define SHAREDRES_SHIFTSTATUS   (((PG_RES)mgSharedRes)->shiftstatus)
#define SHAREDRES_SEMID         (((PG_RES)mgSharedRes)->semid)
#define SHAREDRES_SHMID         (((PG_RES)mgSharedRes)->shmid)
#define SHAREDRES_TOPMOST_LAYER (((PG_RES)mgSharedRes)->topmost_layer)

#if 0
#define SHAREDRES_LAYER_RC      ((RECT*)(&((PG_RES)mgSharedRes)->layer_rc))
#define SHAREDRES_CLI_SCR_LX    (((PG_RES)mgSharedRes)->cli_scr_lx)
#define SHAREDRES_CLI_SCR_TY    (((PG_RES)mgSharedRes)->cli_scr_ty)
#define SHAREDRES_CLI_SCR_RX    (((PG_RES)mgSharedRes)->cli_scr_rx)
#define SHAREDRES_CLI_SCR_BY    (((PG_RES)mgSharedRes)->cli_scr_by)
#define SHAREDRES_CLI_SCR_RC    ((RECT*)(&((PG_RES)mgSharedRes)->cli_scr_lx))
#else
#define SHAREDRES_SCRINFO       (((PG_RES)mgSharedRes)->scr_info)
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


#ifdef __cplusplus

#endif  /* __cplusplus */

#endif // GUI_SHAREDRES_H

