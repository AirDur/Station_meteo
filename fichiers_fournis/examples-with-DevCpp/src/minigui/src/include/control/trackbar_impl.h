/*
** $Id: trackbar_impl.h,v 1.1 2005/01/31 09:12:48 clear Exp $
**
** trackbar.h: the head file of TrackBar(Slider) control.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming and others.
**
** Note:
**   Originally by Zheng Yiran.
**
** Create date: 2000/12/02
*/

#ifndef __TRACKBAR_IMPL_H_
#define __TRACKBAR_IMPL_H_

#ifdef  __cplusplus
extern  "C" {
#endif

typedef  struct tagTRACKBARDATA
{
    int nMin;
    int nMax;
    int nPos;
    int nLineSize;
    int nPageSize;
    char sStartTip [TBLEN_TIP + 1];
    char sEndTip [TBLEN_TIP + 1];
    int nTickFreq;
}TRACKBARDATA;
typedef TRACKBARDATA* PTRACKBARDATA;

BOOL RegisterTrackBarControl (void);
void TrackBarControlCleanup (void);

#ifdef  __cplusplus
}
#endif

#endif // __TRACKBAR_IMPL_H_

