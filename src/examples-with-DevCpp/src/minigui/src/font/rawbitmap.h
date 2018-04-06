/*
** $Id: rawbitmap.h,v 1.18 2004/09/21 01:44:34 snig Exp $
**
** rawbitmap.h: the head file of raw bitmap font operation set.
**
** Copyright (C) 2000 ~ 2002 Wei Yongming.
** Copyright (C) 2003 Feynman Software.
**
*/

#ifndef GUI_FONT_RAWBITMAP_H
    #define GUI_FONT_RAWBITMAP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct
{
    CHARSETOPS* charset_ops;
    int nr_chars;
    int width;
    int height;
    unsigned char* font;
    long font_size;
} RBFINFO;

extern FONTOPS raw_bitmap_font_ops;

BOOL LoadRawBitmapFont (const char* file, RBFINFO* RBFInfo);
void UnloadRawBitmapFont (RBFINFO* RBFInfo);

#define SBC_RBFONT_INFO(logfont) ((RBFINFO*)(((DEVFONT*) (logfont.sbc_devfont))->data))
#define MBC_RBFONT_INFO(logfont) ((RBFINFO*)(((DEVFONT*) (logfont.mbc_devfont))->data))

#define SBC_RBFONT_INFO_P(logfont) ((RBFINFO*)(((DEVFONT*) (logfont->sbc_devfont))->data))
#define MBC_RBFONT_INFO_P(logfont) ((RBFINFO*)(((DEVFONT*) (logfont->mbc_devfont))->data))

#define RBFONT_INFO_P(devfont) ((RBFINFO*)(devfont->data))
#define RBFONT_INFO(devfont) ((RBFINFO*)(devfont.data))

#ifdef _INCORE_RES

typedef struct
{
    const char* name;
    const unsigned char* data;
} INCORE_RBFINFO;

extern INCORE_RBFINFO fixed6x12_iso8859_1;

#ifdef _INCORERBF_GB12
extern INCORE_RBFINFO song12_gb2312;
#endif

#ifdef _INCORERBF_GB16
extern INCORE_RBFINFO song16_gb2312;
#endif

#ifdef _INCORERBF_GB24
extern INCORE_RBFINFO song24_gb2312;
#endif

#ifdef _INCORERBF_LATIN1_24
extern INCORE_RBFINFO fixed12x24_iso8859_1;
#endif

#ifdef _INCORERBF_LATIN1_16
extern INCORE_RBFINFO fixed8x16_iso8859_1;
#endif

#ifdef _INCORERBF_KJ12
extern INCORE_RBFINFO gothic6x12_jis;
#endif

#ifdef _INCORERBF_KJ14
extern INCORE_RBFINFO gothic7x14_jis;
#endif

#ifdef _INCORERBF_KJ16
extern INCORE_RBFINFO gothic8x16_jis;
#endif

#ifdef _INCORERBF_KJ12
extern INCORE_RBFINFO gothic_jiskan_12;
#endif

#ifdef _INCORERBF_KJ14
extern INCORE_RBFINFO kan14_jis;
#endif

#ifdef _INCORERBF_KJ16
extern INCORE_RBFINFO kan16_jis;
#endif


#endif /* _INCORE_RES */


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_RAWBITMAP_H

