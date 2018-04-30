/*
** $Id: gdidecl.h,v 1.3 2005/01/04 07:11:16 limei Exp $
**
** The GDI module internal header.
**
** Copyright (C) 2000 ~ 2002 Wei Yongming.
** Copyright (C) 2003 Feynman Software.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000.4.19
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


#ifndef _MG_GDI_DECL
#define _MG_GDI_DECL


#ifndef _LITE_VERSION

extern BOOL dc_GenerateECRgn (PDC pdc, BOOL fForce);

#define LOCK_GDI() \
    pthread_mutex_lock(&__mg_gdilock);

#define UNLOCK_GDI() \
    pthread_mutex_unlock(&__mg_gdilock);

#define LOCK_ECRGN_EX(pdc, hdc, action) \
    if (dc_IsGeneralHDC(hdc)) { \
        pthread_mutex_lock (&pdc->pGCRInfo->lock); \
        if (!dc_GenerateECRgn (pdc, FALSE)) { \
            pthread_mutex_unlock (&pdc->pGCRInfo->lock); \
            action; \
        } \
    }

#define UNLOCK_ECRGN_EX(pdc, hdc) \
    if (dc_IsGeneralHDC(hdc)) pthread_mutex_unlock (&pdc->pGCRInfo->lock);

#else

#define LOCK_GDI()
#define UNLOCK_GDI()

#ifndef _STAND_ALONE
#define LOCK_ECRGN_EX(pdc, hdc, action) \
    if (!(pdc = __mg_check_ecrgn(hdc))) \
        action;
#else
#define LOCK_ECRGN_EX(pdc, hdc, action)
#endif

#ifndef _STAND_ALONE
#define UNLOCK_ECRGN_EX(pdc, hdc) \
    if (!mgIsServer && dc_IsGeneralDC(pdc)) unlock_scr_sem ();
#else
#define UNLOCK_ECRGN_EX(pdc, hdc)
#endif

#endif

#define LOCK_ECRGN(action) LOCK_ECRGN_EX(pdc, hdc, action)
#define UNLOCK_ECRGN() UNLOCK_ECRGN_EX(pdc, hdc)


#endif /* _MG_GDI_DECL */
