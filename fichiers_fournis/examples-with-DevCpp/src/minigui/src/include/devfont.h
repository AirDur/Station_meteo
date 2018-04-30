/*
** $Id: devfont.h,v 1.12 2003/08/13 10:37:03 weiym Exp $
**
** devfont.h: the head file of Device Font Manager.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/xx/xx
*/

#ifndef GUI_DEVFONT_H
    #define GUI_DEVFONT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/* defined in varbitmap.c */
#ifdef _VBF_SUPPORT
BOOL InitIncoreVBFonts (void);
void TermIncoreVBFonts (void);

BOOL InitVarBitmapFonts (void);
void TermVarBitmapFonts (void);
#endif

/* defined in rawbitmap.c */
#ifdef _RBF_SUPPORT

#ifdef _INCORE_RES
BOOL InitIncoreRBFonts (void);
void TermIncoreRBFonts (void);
#else
BOOL InitRawBitmapFonts (void);
void TermRawBitmapFonts (void);
#endif /* _INCORE_RES */

#endif

/* defined in qpf.c */
#ifdef _QPF_SUPPORT
BOOL InitQPFonts (void);
void TermQPFonts (void);
#endif

/* defined in freetype.c */
#ifdef _TTF_SUPPORT
BOOL InitFreeTypeFonts (void);
void TermFreeTypeFonts (void);
#endif

/* defined in type1.c */
#ifdef _TYPE1_SUPPORT
BOOL InitType1Fonts (void);
void TermType1Fonts (void);
#endif

/* Device font management functions */
void AddSBDevFont (DEVFONT* dev_font);
void AddMBDevFont (DEVFONT* dev_font);
void ResetDevFont (void);

DEVFONT* GetMatchedSBDevFont (LOGFONT* log_font);
DEVFONT* GetMatchedMBDevFont (LOGFONT* log_font);

#ifdef _DEBUG
void dumpDevFonts (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_DEVFONT_H

