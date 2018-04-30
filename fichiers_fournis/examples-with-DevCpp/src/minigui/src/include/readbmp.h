/*
** $Id: readbmp.h,v 1.10 2004/08/04 06:41:10 weiym Exp $
**
** readbmp.h: Low Level bitmap file read/save routines.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2001 ~ 2002 Wei Yongming.
**
** Create date: 2001/xx/xx
*/

#ifndef GUI_GDI_READBMP_H
    #define GUI_GDI_READBMP_H

#include "endianrw.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int __mg_load_bmp (MG_RWops* fp, MYBITMAP *bmp, RGB *pal);
int __mg_save_bmp (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_bmp (MG_RWops* fp);

#ifdef _LBM_FILE_SUPPORT
int __mg_load_lbm (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_lbm (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_lbm (MG_RWops* fp);
#endif

#ifdef _PCX_FILE_SUPPORT
int __mg_load_pcx (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_pcx (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_pcx (MG_RWops* fp);
#endif

#ifdef _TGA_FILE_SUPPORT
int __mg_load_tga (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_tga (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_tga (MG_RWops* fp);
#endif

#ifdef _GIF_FILE_SUPPORT
int __mg_load_gif (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_gif (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_gif (MG_RWops* fp);
#endif

#ifdef _JPG_FILE_SUPPORT
int __mg_load_jpg (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_jpg (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_jpg (MG_RWops* fp);
#endif

#ifdef _PNG_FILE_SUPPORT
int __mg_load_png (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
int __mg_save_png (MG_RWops* fp, MYBITMAP* bmp, RGB* pal);
BOOL __mg_check_png (MG_RWops* fp);
#endif

int bmpComputePitch (int bpp, Uint32 width, Uint32 *pitch, BOOL does_round);

#define fp_getc(fp)     MGUI_RWgetc(fp)
#define fp_igetw(fp)    MGUI_ReadLE16(fp)
#define fp_igetl(fp)    MGUI_ReadLE32(fp)
#define fp_mgetw(fp)    MGUI_ReadBE16(fp)
#define fp_mgetl(fp)    MGUI_ReadBE32(fp)

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_GDI_READBMP_H

