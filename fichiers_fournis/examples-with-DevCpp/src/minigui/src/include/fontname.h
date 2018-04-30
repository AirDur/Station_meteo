/*
** $Id: fontname.h,v 1.10 2003/08/12 07:46:18 weiym Exp $
**
** charset.h: the head file of charset operation set.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
*/

#ifndef GUI_FONT_NAME_H
    #define GUI_FONT_NAME_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

int fontGetFontTypeFromName (const char* name);
int fontGetTypeNameFromName (const char* name, char* type);
int fontConvertFontType (const char* type);

BOOL fontGetFamilyFromName (const char* name, char* family);

DWORD fontGetStyleFromName (const char* name);
DWORD fontConvertStyle (const char* style_part);

int fontGetWidthFromName (const char* name);
int fontGetHeightFromName (const char* name);
BOOL fontGetCharsetFromName (const char* name, char* charset);
BOOL fontGetCompatibleCharsetFromName (const char* name, char* charset);

BOOL fontGetCharsetPartFromName (const char* name, char* charset);
int charsetGetCharsetsNumber (const char* charsets);
BOOL charsetGetSpecificCharset (const char* charsets, int index, char* charset);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_FONT_NAME_H

