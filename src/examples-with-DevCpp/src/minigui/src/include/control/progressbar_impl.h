/*
** $Id: progressbar_impl.h,v 1.1 2005/01/31 09:12:48 clear Exp $
**
** prograssbar.h: the head file of PrograssBar control.
**
** Copyright (c) 2003 Feynman Software.
** Copyright (c) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/8/29
*/

#ifndef __PROGRESSBAR_IMPL_H_
#define __PROGRESSBAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef  struct tagPROGRESSDATA
{
    unsigned int nMin;
    unsigned int nMax;
    unsigned int nPos;
    unsigned int nStepInc;
}PROGRESSDATA;
typedef PROGRESSDATA* PPROGRESSDATA;

BOOL RegisterProgressBarControl (void);
void ProgressBarControlCleanup (void);

#ifdef  __cplusplus
}
#endif

#endif // __PROGRESSBAR_IMPL_H_

