/*
** $Id: gal.h,v 1.28 2004/01/26 09:37:12 snig Exp $
**
** gal.h: the head file of Graphics Abstract Layer
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming
**
** Create date: 2001/10/07
*/

#ifndef GUI_GAL_H
    #define GUI_GAL_H

#ifdef _LITE_VERSION

extern BOOL __mg_switch_away; // always be zero for clients.

#ifndef _STAND_ALONE
extern GHANDLE __mg_layer;

void unlock_draw_sem (void);
void lock_draw_sem (void);
#endif

#endif

#ifdef _USE_NEWGAL
    #include "newgal.h"
#else
    #include "oldgal.h"
#endif

#ifdef _COOR_TRANS
  #define WIDTHOFPHYSCREEN      HEIGHTOFPHYGC
  #define HEIGHTOFPHYSCREEN     WIDTHOFPHYGC
#else
  #define WIDTHOFPHYSCREEN      WIDTHOFPHYGC
  #define HEIGHTOFPHYSCREEN     HEIGHTOFPHYGC
#endif

#endif  /* GUI_GAL_H */

