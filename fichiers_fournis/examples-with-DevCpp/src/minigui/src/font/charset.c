/*
** $Id: charset.c,v 1.48 2004/11/16 08:52:28 snig Exp $
** 
** charset.c: The charset operation set.
** 
** Copyright (C) 2003 Feynman Software
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/06/13
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

/*
** TODO:
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "charset.h"

/*************** Common Operations for single byte charsets ******************/
static int sb_len_first_char (const unsigned char* mstr, int len)
{
    if (len < 1) return 0;
    if (*mstr != '\0')  return 1;
    return 0;
}

static unsigned int sb_char_offset (const unsigned char* mchar)
{
    return (int)(*mchar);
}

static int sb_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    return mstrlen;
}

static int sb_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    return mstrlen;
}

static int sb_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    return 0;
}

static const unsigned char* sb_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    int i;

    if (mstrlen == 0) return NULL;

    word_info->len = 0;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    for (i = 0; i < mstrlen; i++) {
        if (mstr[i] > 127) {
            if (word_info->len > 0) {
                word_info->delimiter = mstr[i];
                word_info->nr_delimiters ++;
            }
            else {
                word_info->len ++;
                word_info->delimiter = ' ';
                word_info->nr_delimiters ++;
            }
            break;
        }

        switch (mstr[i]) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            if (word_info->delimiter == '\0') {
                word_info->delimiter = mstr[i];
                word_info->nr_delimiters ++;
            }
            else if (word_info->delimiter == mstr[i])
                word_info->nr_delimiters ++;
            else
                return mstr + word_info->len + word_info->nr_delimiters;
        break;

        default:
            if (word_info->delimiter != '\0')
                break;

            word_info->len ++;
        }
    }
            
    return mstr + word_info->len + word_info->nr_delimiters;
}

/************************* US-ASCII Specific Operations **********************/
static int ascii_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ASCII"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short ascii_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 128)
        return (unsigned short) (*mchar);
    else
        return '?';
}
#endif

static CHARSETOPS CharsetOps_ascii = {
    128,
    1,
    1,
    FONT_CHARSET_US_ASCII,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    ascii_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    ascii_conv_to_uc16
#endif
};

/************************* ISO8859-1 Specific Operations **********************/
static int iso8859_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-1"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_1_conv_to_uc16 (const unsigned char* mchar, int len)
{
    return (unsigned short) (*mchar);
}
#endif

static CHARSETOPS CharsetOps_iso8859_1 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_1,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_1_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_1_conv_to_uc16
#endif
};

#ifdef _LATIN2_SUPPORT

/************************* ISO8859-2 Specific Operations **********************/
static int iso8859_2_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-2"))
        return 0;

    if (strstr (name, "LATIN2"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_2_unicode_map [] =
{
    0x0104, 0x02D8,
    0x0141, 0x00A4, 0x013D,
    0x015A, 0x00A7, 0x00A8,
    0x0160, 0x015E, 0x0164,
    0x0179, 0x00AD, 0x017D,
    0x017B, 0x00B0, 0x0105,
    0x02DB, 0x0142, 0x00B4,
    0x013E, 0x015B, 0x02C7,
    0x00B8, 0x0161, 0x015F,
    0x0165, 0x017A, 0x02DD,
    0x017E, 0x017C, 0x0154,
    0x00C1, 0x00C2, 0x0102,
    0x00C4, 0x0139, 0x0106,
    0x00C7, 0x010C, 0x00C9,
    0x0118, 0x00CB, 0x011A,
    0x00CD, 0x00CE, 0x010E,
    0x0110, 0x0143, 0x0147,
    0x00D3, 0x00D4, 0x0150,
    0x00D6, 0x00D7, 0x0158,
    0x016E, 0x00DA, 0x0170,
    0x00DC, 0x00DD, 0x0162,
    0x00DF, 0x0155, 0x00E1,
    0x00E2, 0x0103, 0x00E4,
    0x013A, 0x0107, 0x00E7,
    0x010D, 0x00E9, 0x0119,
    0x00EB, 0x011B, 0x00ED,
    0x00EE, 0x010F, 0x0111,
    0x0144, 0x0148, 0x00F3,
    0x00F4, 0x0151, 0x00F6,
    0x00F7, 0x0159, 0x016F,
    0x00FA, 0x0171, 0x00FC,
    0x00FD, 0x0163, 0x02D9
};

static unsigned short iso8859_2_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
    return iso8859_2_unicode_map [*mchar - 0xA1];
   
}
#endif

static CHARSETOPS CharsetOps_iso8859_2 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_2,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_2_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_2_conv_to_uc16
#endif
};

#endif /* _LATIN2_SUPPORT */

#ifdef _LATIN3_SUPPORT

/************************* ISO8859-3 Specific Operations **********************/
static int iso8859_3_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-3"))
        return 0;

    if (strstr (name, "LATIN3"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_3_unicode_map [] =
{
    0x0126, 0x02D8, 0x00A3,
    0x00A4, 0x00A5, 0x0124,
    0x00A7, 0x00A8, 0x0130,
    0x015E, 0x011E, 0x0134,
    0x00AD, 0x00AE, 0x017B,
    0x00B0, 0x0127, 0x00B2,
    0x00B3, 0x00B4, 0x00B5,
    0x0125, 0x00B7, 0x00B8,
    0x0131, 0x015F, 0x011F,
    0x0135, 0x00BD, 0x00BE,
    0x017C, 0x00C0, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x010A, 0x0108, 0x00C7,
    0x00C8, 0x00C9, 0x00CA,
    0x00CB, 0x00CC, 0x00CD,
    0x00CE, 0x00CF, 0x00D0,
    0x00D1, 0x00D2, 0x00D3,
    0x00D4, 0x0120, 0x00D6,
    0x00D7, 0x011C, 0x00D9,
    0x00DA, 0x00DB, 0x00DC,
    0x016C, 0x015C, 0x00DF,
    0x00E0, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x010B,
    0x0109, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB,
    0x00EC, 0x00ED, 0x00EE,
    0x00EF, 0x00F0, 0x00F1,
    0x00F2, 0x00F3, 0x00F4,
    0x0121, 0x00F6, 0x00F7,
    0x011D, 0x00F9, 0x00FA,
    0x00FB, 0x00FC, 0x016D,
    0x015D, 0x02D9
};

static unsigned short iso8859_3_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
    return iso8859_3_unicode_map [*mchar - 0xA1];
   
}
#endif

static CHARSETOPS CharsetOps_iso8859_3 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_3,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_3_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_3_conv_to_uc16
#endif
};

#endif /* _LATIN4_SUPPORT */

#ifdef _LATIN4_SUPPORT

/************************* ISO8859-4 Specific Operations **********************/
static int iso8859_4_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-4"))
        return 0;

    if (strstr (name, "LATIN4"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_4_unicode_map [] =
{
    0x0104, 0x0138, 0x0156,
    0x00A4, 0x0128, 0x013B,
    0x00A7, 0x00A8, 0x0160,
    0x0112, 0x0122, 0x0166,
    0x00AD, 0x017D, 0x00AF,
    0x00B0, 0x0105, 0x02DB,
    0x0157, 0x00B4, 0x0129,
    0x013C, 0x02C7, 0x00B8,
    0x0161, 0x0113, 0x0123,
    0x0167, 0x014A, 0x017E,
    0x014B, 0x0100, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x012E,
    0x010C, 0x00C9, 0x0118,
    0x00CB, 0x0116, 0x00CD,
    0x00CE, 0x012A, 0x0110,
    0x0145, 0x014C, 0x0136,
    0x00D4, 0x00D5, 0x00D6,
    0x00D7, 0x00D8, 0x0172,
    0x00DA, 0x00DB, 0x00DC,
    0x0168, 0x016A, 0x00DF,
    0x0101, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB,
    0x0117, 0x00ED, 0x00EE,
    0x012B, 0x0111, 0x0146,
    0x014D, 0x0137, 0x00F4,
    0x00F5, 0x00F6, 0x00F7,
    0x00F8, 0x0173, 0x00FA,
    0x00FB, 0x00FC, 0x0169,
    0x016B, 0x02D9
};

static unsigned short iso8859_4_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
        return iso8859_4_unicode_map [*mchar - 0xA1];
   
}
#endif

static CHARSETOPS CharsetOps_iso8859_4 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_4,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_4_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_4_conv_to_uc16
#endif
};

#endif /* _LATIN4_SUPPORT */

#ifdef _CYRILLIC_SUPPORT

/************************* ISO8859-5 Specific Operations **********************/
static int iso8859_5_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-5"))
        return 0;

    if (strstr (name, "CYRILLIC"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_5_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
   
    if (*mchar == 0xAD)
        return 0x00AD;  /* SOFT HYPHEN */

    if (*mchar == 0xF0)
        return 0x2116;  /* NUMERO SIGN */

    if (*mchar == 0xFD)
        return 0x00A7;  /* SECTION SIGN */

    return *mchar + (0x0401 - 0xA1);
}
#endif

static CHARSETOPS CharsetOps_iso8859_5 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_5,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_5_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_5_conv_to_uc16
#endif
};

#endif /* _CYRILLIC_SUPPORT */

#ifdef _ARABIC_SUPPORT

/************************* ISO8859-6 Specific Operations **********************/
static int iso8859_6_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-6"))
        return 0;

    if (strstr (name, "ARABIC"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_6_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar >= 0xC1 && *mchar <= 0xF2) {
        return  *mchar - 0xC1 + 0x0621;
    }
    else {
        switch (*mchar) {
            case 0xAC:
                return 0x060C;
            case 0xBB:
                return 0x061B;
            case 0xBF:
                return 0x061F;
        }

        return (unsigned short) (*mchar);
    }
}
#endif

static CHARSETOPS CharsetOps_iso8859_6 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_6,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_6_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_6_conv_to_uc16
#endif
};

#endif /* _ARABIC_SUPPORT */

#ifdef _GREEK_SUPPORT

/************************* ISO8859-7 Specific Operations **********************/
static int iso8859_7_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-7"))
        return 0;

    if (strstr (name, "GREEK"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_7_unicode_map [] =
{
    0x2018, 0x2019, 0x00A3,
    0x00A4, 0x00A5, 0x00A6,
    0x00A7, 0x00A8, 0x00A9,
    0x00AA, 0x00AB, 0x00AC,
    0x00AD, 0x00AE, 0x2015,
    0x00B0, 0x00B1, 0x00B2,
    0x00B3, 0x0384, 0x0385,
    0x0386, 0x00B7, 0x0388,
    0x0389, 0x038A, 0x00BB,
    0x038C, 0x00BD
};

static unsigned short iso8859_7_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar >= 0xBE && *mchar <= 0xFE) {
        return  *mchar - 0xBE + 0x038E;
    }
    else if (*mchar >= 0xA1 && *mchar <= 0xBD) {
        return iso8859_7_unicode_map [*mchar - 0xA1];
    }
    else {
        return (unsigned short) (*mchar);
    }
}
#endif

static CHARSETOPS CharsetOps_iso8859_7 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_7,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_7_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_7_conv_to_uc16
#endif
};

#endif /* _GREEK_SUPPORT */

#ifdef _HEBREW_SUPPORT

/************************* ISO8859-8 Specific Operations **********************/
static int iso8859_8_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-8"))
        return 0;

    if (strstr (name, "HEBREW"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_8_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar >= 0xE0 && *mchar <= 0xFA) {
        return  *mchar - 0xE0 + 0x05D0;
    }
    else {
        switch (*mchar) {
            case 0xAA:
                return 0x00D7;
            case 0xBA:
                return 0x00F7;
            case 0xDF:
                return 0x2017;
            case 0xFD:
                return 0x200E;
            case 0xFE:
                return 0x200F;
        }
        return (unsigned short) (*mchar);
    }
}
#endif

static CHARSETOPS CharsetOps_iso8859_8 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_8,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_8_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_8_conv_to_uc16
#endif
};

#endif /* _GREEK_SUPPORT */

#ifdef _LATIN5_SUPPORT

/************************* ISO8859-9 Specific Operations **********************/
static int iso8859_9_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-9"))
        return 0;

    if (strstr (name, "LATIN5"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_9_conv_to_uc16 (const unsigned char* mchar, int len)
{
    switch (*mchar) {
        case 0xD0:
                return 0x011E;
        case 0xDD:
                return 0x0130;
        case 0xDE:
                return 0x015E;
        case 0xF0:
                return 0x011F;
        case 0xFD:
                return 0x0131;
        case 0xFE:
                return 0x015F;
    }

    return (unsigned short) (*mchar);
}
#endif

static CHARSETOPS CharsetOps_iso8859_9 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_9,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_9_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_9_conv_to_uc16
#endif
};

#endif /* _LATIN5_SUPPORT */

#ifdef _LATIN6_SUPPORT

/************************* ISO8859-10 Specific Operations **********************/
static int iso8859_10_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-10"))
        return 0;

    if (strstr (name, "LATIN6"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_10_unicode_map [] =
{
    0x0104, 0x0112, 0x0122,
    0x012A, 0x0128, 0x0136,
    0x00A7, 0x013B, 0x0110,
    0x0160, 0x0166, 0x017D,
    0x00AD, 0x016A, 0x014A,
    0x00B0, 0x0105, 0x0113,
    0x0123, 0x012B, 0x0129,
    0x0137, 0x00B7, 0x013C,
    0x0111, 0x0161, 0x0167,
    0x017E, 0x2015, 0x016B,
    0x014B, 0x0100, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x012E,
    0x010C, 0x00C9, 0x0118,
    0x00CB, 0x0116, 0x00CD,
    0x00CE, 0x00CF, 0x00D0,
    0x0145, 0x014C, 0x00D3,
    0x00D4, 0x00D5, 0x00D6,
    0x0168, 0x00D8, 0x0172,
    0x00DA, 0x00DB, 0x00DC,
    0x00DD, 0x00DE, 0x00DF,
    0x0101, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x012F, 0x010D,
    0x00E9, 0x0119, 0x00EB,
    0x0117, 0x00ED, 0x00EE,
    0x00EF, 0x00F0, 0x0146,
    0x014D, 0x00F3, 0x00F4,
    0x00F5, 0x00F6, 0x0169,
    0x00F8, 0x0173, 0x00FA,
    0x00FB, 0x00FC, 0x00FD,
    0x00FE, 0x0138
};

static unsigned short iso8859_10_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
        return iso8859_10_unicode_map [*mchar - 0xA1];
}
#endif

static CHARSETOPS CharsetOps_iso8859_10 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_10,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_10_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_10_conv_to_uc16
#endif
};

#endif /* _LATIN6_SUPPORT */

#ifdef _THAI_SUPPORT

/************************* ISO8859-11 Specific Operations **********************/
static int iso8859_11_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-11"))
        return 0;

    if (strstr (name, "TIS") && strstr (name, "620"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_11_unicode_map [] =
{
    0x0E01, 0x0E02, 0x0E03,
    0x0E04, 0x0E05, 0x0E06,
    0x0E07, 0x0E08, 0x0E09,
    0x0E0A, 0x0E0B, 0x0E0C,
    0x0E0D, 0x0E0E, 0x0E0F,
    0x0E10, 0x0E11, 0x0E12,
    0x0E13, 0x0E14, 0x0E15,
    0x0E16, 0x0E17, 0x0E18,
    0x0E19, 0x0E1A, 0x0E1B,
    0x0E1C, 0x0E1D, 0x0E1E,
    0x0E1F, 0x0E20, 0x0E21,
    0x0E22, 0x0E23, 0x0E24,
    0x0E25, 0x0E26, 0x0E27,
    0x0E28, 0x0E29, 0x0E2A,
    0x0E2B, 0x0E2C, 0x0E2D,
    0x0E2E, 0x0E2F, 0x0E30,
    0x0E31, 0x0E32, 0x0E33,
    0x0E34, 0x0E35, 0x0E36,
    0x0E37, 0x0E38, 0x0E39,
    0x0E3A, 0x00DB, 0x00DC,
    0x00DD, 0x00DE, 0x0E3F,
    0x0E40, 0x0E41, 0x0E42,
    0x0E43, 0x0E44, 0x0E45,
    0x0E46, 0x0E47, 0x0E48,
    0x0E49, 0x0E4A, 0x0E4B,
    0x0E4C, 0x0E4D, 0x0E4E,
    0x0E4F, 0x0E50, 0x0E51,
    0x0E52, 0x0E53, 0x0E54,
    0x0E55, 0x0E56, 0x0E57,
    0x0E58, 0x0E59, 0x0E5A,
    0x0E5B, 0x00FC, 0x00FD,
    0x00FE, 0x00FF
};

static unsigned short iso8859_11_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
        return iso8859_11_unicode_map [*mchar - 0xA1];
}
#endif

static CHARSETOPS CharsetOps_iso8859_11 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_11,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_11_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_11_conv_to_uc16
#endif
};

#endif /* _THAI_SUPPORT */

#ifdef _LATIN7_SUPPORT

/************************* ISO8859-13 Specific Operations **********************/
static int iso8859_13_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-13"))
        return 0;

    if (strstr (name, "LATIN7"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_13_unicode_map [] =
{
    0x201D, 0x00A2, 0x00A3,
    0x00A4, 0x201E, 0x00A6,
    0x00A7, 0x00D8, 0x00A9,
    0x0156, 0x00AB, 0x00AC,
    0x00AD, 0x00AE, 0x00C6,
    0x00B0, 0x00B1, 0x00B2,
    0x00B3, 0x201C, 0x00B5,
    0x00B6, 0x00B7, 0x00F8,
    0x00B9, 0x0157, 0x00BB,
    0x00BC, 0x00BD, 0x00BE,
    0x00E6, 0x0104, 0x012E,
    0x0100, 0x0106, 0x00C4,
    0x00C5, 0x0118, 0x0112,
    0x010C, 0x00C9, 0x0179,
    0x0116, 0x0122, 0x0136,
    0x012A, 0x013B, 0x0160,
    0x0143, 0x0145, 0x00D3,
    0x014C, 0x00D5, 0x00D6,
    0x00D7, 0x0172, 0x0141,
    0x015A, 0x016A, 0x00DC,
    0x017B, 0x017D, 0x00DF,
    0x0105, 0x012F, 0x0101,
    0x0107, 0x00E4, 0x00E5,
    0x0119, 0x0113, 0x010D,
    0x00E9, 0x017A, 0x0117,
    0x0123, 0x0137, 0x012B,
    0x013C, 0x0161, 0x0144,
    0x0146, 0x00F3, 0x014D,
    0x00F5, 0x00F6, 0x00F7,
    0x0173, 0x0142, 0x015B,
    0x016B, 0x00FC, 0x017C,
    0x017E, 0x2019
};

static unsigned short iso8859_13_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
        return iso8859_13_unicode_map [*mchar - 0xA1];
}
#endif

static CHARSETOPS CharsetOps_iso8859_13 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_13,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_13_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_13_conv_to_uc16
#endif
};

#endif /* _LATIN7_SUPPORT */

#ifdef _LATIN8_SUPPORT

/************************* ISO8859-14 Specific Operations **********************/
static int iso8859_14_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-14"))
        return 0;

    if (strstr (name, "LATIN8"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_14_unicode_map [] =
{
    0x1E02, 0x1E03, 0x00A3,
    0x010A, 0x010B, 0x1E0A,
    0x00A7, 0x1E80, 0x00A9,
    0x1E82, 0x1E0B, 0x1EF2,
    0x00AD, 0x00AE, 0x0178,
    0x1E1E, 0x1E1F, 0x0120,
    0x0121, 0x1E40, 0x1E41,
    0x00B6, 0x1E56, 0x1E81,
    0x1E57, 0x1E83, 0x1E60,
    0x1EF3, 0x1E84, 0x1E85,
    0x1E61, 0x00C0, 0x00C1,
    0x00C2, 0x00C3, 0x00C4,
    0x00C5, 0x00C6, 0x00C7,
    0x00C8, 0x00C9, 0x00CA,
    0x00CB, 0x00CC, 0x00CD,
    0x00CE, 0x00CF, 0x0174,
    0x00D1, 0x00D2, 0x00D3,
    0x00D4, 0x00D5, 0x00D6,
    0x1E6A, 0x00D8, 0x00D9,
    0x00DA, 0x00DB, 0x00DC,
    0x00DD, 0x0176, 0x00DF,
    0x00E0, 0x00E1, 0x00E2,
    0x00E3, 0x00E4, 0x00E5,
    0x00E6, 0x00E7, 0x00E8,
    0x00E9, 0x00EA, 0x00EB,
    0x00EC, 0x00ED, 0x00EE,
    0x00EF, 0x0175, 0x00F1,
    0x00F2, 0x00F3, 0x00F4,
    0x00F5, 0x00F6, 0x1E6B,
    0x00F8, 0x00F9, 0x00FA,
    0x00FB, 0x00FC, 0x00FD,
    0x0177, 0x00FF
};

static unsigned short iso8859_14_conv_to_uc16 (const unsigned char* mchar, int len)
{
    if (*mchar < 0xA1)
        return (unsigned short) (*mchar);
    else
        return iso8859_14_unicode_map [*mchar - 0xA1];
}
#endif

static CHARSETOPS CharsetOps_iso8859_14 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_14,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_14_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_14_conv_to_uc16
#endif
};

#endif /* _LATIN8_SUPPORT */

#ifdef _LATIN9_SUPPORT

/************************* ISO8859-15 Specific Operations **********************/
static int iso8859_15_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-15"))
        return 0;

    if (strstr (name, "LATIN") && strstr (name, "9"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_15_conv_to_uc16 (const unsigned char* mchar, int len)
{
    switch (*mchar) {
        case 0xA4:
            return 0x20AC;  /* EURO SIGN */
        case 0xA6:
            return 0x0160;  /* LATIN CAPITAL LETTER S WITH CARO */
        case 0xA8:
            return 0x0161;  /* LATIN SMALL LETTER S WITH CARON */
        case 0xB4:
            return 0x017D;  /* LATIN CAPITAL LETTER Z WITH CARON */
        case 0xB8:
            return 0x017E;  /* LATIN SMALL LETTER Z WITH CARON */
        case 0xBC:
            return 0x0152;  /* LATIN CAPITAL LIGATURE OE */
        case 0xBD:
            return 0x0153;  /* LATIN SMALL LIGATURE OE */
        case 0xBE:
            return 0x0178;  /* LATIN CAPITAL LETTER Y WITH DIAERESIS */
        default:
            return *mchar;
    }
}
#endif

static CHARSETOPS CharsetOps_iso8859_15 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_15,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_15_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_15_conv_to_uc16
#endif
};
#endif /* _LATIN9_SUPPORT */

#ifdef _LATIN10_SUPPORT

/************************* ISO8859-16 Specific Operations **********************/
static int iso8859_16_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "ISO") && strstr (name, "8859-16"))
        return 0;

    if (strstr (name, "LATIN") && strstr (name, "10"))
        return 0;

    return 1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short iso8859_16_conv_to_uc16 (const unsigned char* mchar, int len)
{
    switch (*mchar) {
        case 0xA1: return 0x0104;
        case 0xA2: return 0x0105;
        case 0xA3: return 0x0141;
        case 0xA4: return 0x20AC;
        case 0xA5: return 0x201E;
        case 0xA6: return 0x0160;
        case 0xA8: return 0x0161;
        case 0xAA: return 0x0218;
        case 0xAC: return 0x0179;
        case 0xAE: return 0x017A;
        case 0xAF: return 0x017B;
        case 0xB2: return 0x010C;
        case 0xB3: return 0x0142;
        case 0xB4: return 0x017D;
        case 0xB5: return 0x201D;
        case 0xB8: return 0x017E;
        case 0xB9: return 0x010D;
        case 0xBA: return 0x0219;
        case 0xBC: return 0x0152;
        case 0xBD: return 0x0153;
        case 0xBE: return 0x0178;
        case 0xBF: return 0x017C;
        case 0xC3: return 0x0102;
        case 0xC5: return 0x0106;
        case 0xD0: return 0x0110;
        case 0xD1: return 0x0143;
        case 0xD5: return 0x0150;
        case 0xD7: return 0x015A;
        case 0xD8: return 0x0170;
        case 0xDD: return 0x0118;
        case 0xDE: return 0x021A;
        case 0xE3: return 0x0103;
        case 0xE5: return 0x0107;
        case 0xF0: return 0x0111;
        case 0xF1: return 0x0144;
        case 0xF5: return 0x0151;
        case 0xF7: return 0x015B;
        case 0xF8: return 0x0171;
        case 0xFD: return 0x0119;
        case 0xFE: return 0x021B;
    }

    return (unsigned short) (*mchar);
}
#endif

static CHARSETOPS CharsetOps_iso8859_16 = {
    256,
    1,
    1,
    FONT_CHARSET_ISO8859_16,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    iso8859_16_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    iso8859_16_conv_to_uc16
#endif
};

#endif /* _LATIN10_SUPPORT */

/*************** Common Operations for double bytes charsets ******************/
#if defined(_GB_SUPPORT) | defined(_GBK_SUPPORT) | defined(_BIG5_SUPPORT) \
        | defined(_EUCKR_SUPPORT) | defined(_EUCJP_SUPPORT) | defined(_SHIFTJIS_SUPPORT) \
        | defined(_UNICODE_SUPPORT)
static int db_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    assert ((mstrlen % 2) == 0);
    return mstrlen >> 1;
}

static const unsigned char* db_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    assert ((mstrlen % 2) == 0);

    if (mstrlen < 2) return NULL;

    word_info->len = 2;
    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    return mstr + 2;
}
#endif

#ifdef _GB_SUPPORT
/************************* GB2312 Specific Operations ************************/
#define IS_GB2312_CHAR(ch1, ch2) \
        if (((ch1 >= 0xA1 && ch1 <= 0xA9) || (ch1 >= 0xB0 && ch1 <= 0xF7)) \
                        && ch2 >= 0xA1 && ch2 <= 0xFE)

static int gb2312_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GB2312_CHAR (ch1, ch2)
        return 2;

    return 0;
}

static unsigned int gb2312_0_char_offset (const unsigned char* mchar)
{
    int area = mchar [0] - 0xA1;

    if (area < 9) {
        return (area * 94 + mchar [1] - 0xA1);
    }
    else if (area >= 15)
        return ((area - 6)* 94 + mchar [1] - 0xA1);

    return 0;
}

static int gb2312_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GB") && strstr (name, "2312"))
        return 0;

    return 1;
}

static int gb2312_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        IS_GB2312_CHAR (ch1, ch2)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int gb2312_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        IS_GB2312_CHAR (ch1, ch2)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short gb2312_0_conv_to_uc16 (const unsigned char* mchar, int len)
{
    return gbunicode_map [gb2312_0_char_offset (mchar)];
}
#endif

static CHARSETOPS CharsetOps_gb2312_0 = {
    (0xFF-0xA1)*(72 + 9),
    2,
    2,
    FONT_CHARSET_GB2312_0,
    {'\xA1', '\xA1'},
    gb2312_0_len_first_char,
    gb2312_0_char_offset,
    db_nr_chars_in_str,
    gb2312_0_is_this_charset,
    gb2312_0_len_first_substr,
    db_get_next_word,
    gb2312_0_pos_first_char,
#ifdef _UNICODE_SUPPORT
    gb2312_0_conv_to_uc16
#endif
};
#endif /* _GB_SUPPORT */

#ifdef _GBK_SUPPORT
/************************* GBK Specific Operations ************************/

#define IS_GBK_CHAR(ch1, ch2) \
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)

static int gbk_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    IS_GBK_CHAR(ch1, ch2)
        return 2;

    return 0;
}

static unsigned int gbk_char_offset (const unsigned char* mchar)
{
    if (mchar [1] > 0x7F)
        return ((mchar [0] - 0x81) * 190 + mchar [1] - 0x41);
    else
        return ((mchar [0] - 0x81) * 190 + mchar [1] - 0x40);
}

static int gbk_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GBK"))
        return 0;

    return 1;
}

static int gbk_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        IS_GBK_CHAR(ch1, ch2)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int gbk_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        IS_GBK_CHAR(ch1, ch2)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short gbk_conv_to_uc16 (const unsigned char* mchar, int len)
{
    return gbkunicode_map [(mchar [0] - 0x81) * 192 + mchar [1] - 0x40];
}
#endif

static CHARSETOPS CharsetOps_gbk = {
    (0xFF - 0x80) * 190,
    2,
    2,
    FONT_CHARSET_GBK,
    {'\xA1', '\xA1'},
    gbk_len_first_char,
    gbk_char_offset,
    db_nr_chars_in_str,
    gbk_is_this_charset,
    gbk_len_first_substr,
    db_get_next_word,
    gbk_pos_first_char,
#ifdef _UNICODE_SUPPORT
    gbk_conv_to_uc16
#endif
};

#endif /* _GBK_SUPPORT */

#ifdef _GB18030_SUPPORT

/************************* GBK Specific Operations ************************/
static int gb18030_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
        return 2;

    if (len < 4) return 0;

    ch3 = mstr [2];
    ch4 = mstr [3];
    if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 >= 0x39
            && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE)
        return 4;

    return 0;
}

static unsigned int gb18030_0_char_offset (const unsigned char* mchar)
{
    unsigned char ch1;
    unsigned char ch2;
    unsigned char ch3;
    unsigned char ch4;

    ch1 = mchar [0];
    ch2 = mchar [1];
    if (ch2 >= 0x40)
        return ((ch1 - 0x81) * 192 + (ch2 - 0x40));

    ch3 = mchar [2];
    ch4 = mchar [3];
    return ((126 * 192) + 
            ((ch1 - 0x81) * 12600 + (ch2 - 0x30) * 1260 + 
             (ch3 - 0x81) * 10 + (ch4 - 0x30)));
}

static int gb18030_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "GB") && strstr (name, "18030"))
        return 0;

    return 1;
}

static int gb18030_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1, ch2, ch3, ch4;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F) {
            sub_len += 2;
            left -= 2;
        }
        else if (left >= 4) {
            
            ch3 = mstr [i + 2];
            ch4 = mstr [i + 3];
            if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 >= 0x39
                    && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE) {
                sub_len += 4;
                left -= 4;
                i += 2;
            }
        }
        else
            return sub_len;
    }

    return sub_len;
}

static int gb18030_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1, ch2, ch3, ch4;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0x81 && ch1 <= 0xFE && ch2 >= 0x40 && ch2 <= 0xFE && ch2 != 0x7F)
            return i;

        if (left < 4) return -1;

        ch3 = mstr [i + 2];
        ch4 = mstr [i + 3];
        if (ch2 >= 0x30 && ch2 <= 0x39 && ch4 >= 0x30 && ch4 >= 0x39
                && ch1 >= 0x81 && ch1 <= 0xFE && ch3 >= 0x81 && ch3 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

static int gb18030_0_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int i, left;
    int n;

    assert ((mstrlen % 2) == 0);

    n = 0;
    i = 0;
    left = mstrlen;
    while (left) {
        if (left >= 2 && mstr [i + 1] >= 0x40) {
            left -= 2;
            i += 2;
        }
        else if (left >= 4) {
            left -= 4;
            i += 4;
        }
        else
            break;

        n++;
    }

    return n;

}

static const unsigned char* gb18030_0_get_next_word (const unsigned char* mstr,
                int mstrlen, WORDINFO* word_info)
{
    assert ((mstrlen % 2) == 0);

    if (mstrlen < 2) return NULL;

    if (mstrlen >= 2 && mstr [1] >= 0x40) {
        word_info->len = 2;
    }
    else if (mstrlen >= 4) {
        word_info->len = 4;
    }

    word_info->delimiter = '\0';
    word_info->nr_delimiters = 0;

    return mstr + word_info->len;
}

#ifdef _UNICODE_SUPPORT
static unsigned short gb18030_0_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned int index = gb18030_0_char_offset (mchar);

    if (index > 63611)
        return '?';
    else
        return gb18030_0_unicode_map [index];
}
#endif

static CHARSETOPS CharsetOps_gb18030_0 = {
    1587600 + 23940,
    4,
    4,
    FONT_CHARSET_GB18030_0,
    {'\xA1', '\xA1'},
    gb18030_0_len_first_char,
    gb18030_0_char_offset,
    gb18030_0_nr_chars_in_str,
    gb18030_0_is_this_charset,
    gb18030_0_len_first_substr,
    gb18030_0_get_next_word,
    gb18030_0_pos_first_char,
#ifdef _UNICODE_SUPPORT
    gb18030_0_conv_to_uc16
#endif
};

#endif /* _GB18030_SUPPORT */

#ifdef _BIG5_SUPPORT

/************************** BIG5 Specific Operations ************************/
static int big5_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE && 
            ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
        return 2;

    return 0;
}

static unsigned int big5_char_offset (const unsigned char* mchar)
{
    if (mchar [1] & 0x80)
        return (mchar [0] - 0xA1) * 94 + mchar [1] - 0xA1;
    else
        return 94 * 94 + (mchar [0] - 0xa1) * 63 + (mchar [1] - 0x40);
}

static int big5_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "BIG5"))
        return 0;

    return 1;
}

static int big5_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && 
                ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int big5_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && 
                ((ch2 >=0x40 && ch2 <= 0x7E) || (ch2 >= 0xA1 && ch2 <= 0xFE)))
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short big5_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned short ucs_code = big5_unicode_map [big5_char_offset (mchar)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}
#endif

static CHARSETOPS CharsetOps_big5 = {
    14758,
    2,
    2,
    FONT_CHARSET_BIG5,
    {'\xA1', '\x40'},
    big5_len_first_char,
    big5_char_offset,
    db_nr_chars_in_str,
    big5_is_this_charset,
    big5_len_first_substr,
    db_get_next_word,
    big5_pos_first_char,
#ifdef _UNICODE_SUPPORT
    big5_conv_to_uc16
#endif
};

#endif /* _BIG5_SUPPORT */

#ifdef _EUCKR_SUPPORT

/************************* EUCKR Specific Operations ************************/
static int ksc5601_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
        return 2;

    return 0;
}

static unsigned int ksc5601_0_char_offset (const unsigned char* mchar)
{
#if 1
    return ((mchar [0] - 0xA1) * 94 + mchar [1] - 0xA1);
#else
    if(mchar [0] > 0xAD)
        return ((mchar [0] - 0xA4) * 94 + mchar [1] - 0xA1 - 0x8E);
    else
        return ((mchar [0] - 0xA1) * 94 + mchar [1] - 0xA1 - 0x8E);
#endif
}

static int ksc5601_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "KSC5601") && strstr (name, "-0")) || 
            (strstr (name, "EUC") && strstr (name, "KR")))
        return 0;

    return 1;
}

static int ksc5601_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int ksc5601_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short ksc5601_0_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned char ucs_code = ksc5601_0_unicode_map [ksc5601_0_char_offset (mchar)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}
#endif

static CHARSETOPS CharsetOps_ksc5601_0 = {
    8836,
    2,
    2,
    FONT_CHARSET_EUCKR,
    {'\xA1', '\xA1'},
    ksc5601_0_len_first_char,
    ksc5601_0_char_offset,
    db_nr_chars_in_str,
    ksc5601_0_is_this_charset,
    ksc5601_0_len_first_substr,
    db_get_next_word,
    ksc5601_0_pos_first_char,
#ifdef _UNICODE_SUPPORT
    ksc5601_0_conv_to_uc16
#endif
};
/************************* End of EUCKR *************************************/

#endif  /* _EUCKR_SUPPORT */

#ifdef _EUCJP_SUPPORT

/************************* EUCJP Specific Operations ************************/
static int jisx0201_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 == 0x8E && ch2 >= 0xA1 && ch2 <= 0xDF)
        return 2;
    else
        return 1;

    return 0;
}

static unsigned int jisx0201_0_char_offset (const unsigned char* mchar)
{
    if (mchar [0] == 0x8E)
        return mchar [1];
    else
        return mchar [0];
}

static int jisx0201_0_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int left;
    int nr_chars = 0;

    left = mstrlen;
    while (left > 0) {

        ch1 = mstr [0];
        if (ch1 == '\0') return nr_chars;

        ch2 = mstr [1];
        if (ch1 == 0x8E && ch2 >= 0xA1 && ch2 <= 0xDF) {
            nr_chars ++;
            left -= 2;
            mstr += 2;
        }
        else {
            nr_chars ++;
            left -= 1;
            mstr += 1;
        }
    }

    return nr_chars;
}

static int jisx0201_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "JISX0201") && strstr (name, "-0"))
        return 0;

    return 1;
}


#ifdef _UNICODE_SUPPORT
static unsigned short jisx0201_0_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    ch1 = mchar [0]; ch2 = mchar [1];
    if (ch1 == 0x8E && ch2 >= 0xA1 && ch2 <= 0xDF)
        return 0xFF61 + ch2 - 0xA1;
    else if (ch1 == 0x5C)
        return 0x00A5;
    else if (ch1 == 0x80)
        return 0x005C;
    return ch1;
}
#endif

static CHARSETOPS CharsetOps_jisx0201_0 = {
    190,
    1,
    2,
    FONT_CHARSET_JISX0201_0,
    {' '},
    jisx0201_0_len_first_char,
    jisx0201_0_char_offset,
    jisx0201_0_nr_chars_in_str,
    jisx0201_0_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    jisx0201_0_conv_to_uc16
#endif
};

static int jisx0208_0_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
        return 2;

    return 0;
}

static unsigned int jisx0208_0_char_offset (const unsigned char* mchar)
{
#if 1
    return ((mchar [0] - 0xA1) * 94 + mchar [1] - 0xA1);
#else
    if(mchar [0] > 0xAA)
        return ((mchar [0] - 0xA6 - 0x02) * 94 + mchar [1] - 0xC1 - 0xC4);
    else
        return ((mchar [0] - 0xA1 - 0x02) * 83 + mchar [1] - 0xA1 + 0x7E);
#endif
}

static int jisx0208_0_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "JISX0208") && strstr (name, "-0")) ||
            (strstr (name, "EUC") && strstr (name, "JP")))
        return 0;

    return 1;
}

static int jisx0208_0_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int jisx0208_0_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (ch1 >= 0xA1 && ch1 <= 0xFE && ch2 >= 0xA1 && ch2 <= 0xFE)
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short jisx0208_0_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned short ucs_code = jisx0208_0_unicode_map [jisx0208_0_char_offset (mchar)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}
#endif

static CHARSETOPS CharsetOps_jisx0208_0 = {
    8836,
    2,
    2,
    FONT_CHARSET_JISX0208_0,
    {'\xA1', '\xA1'},
    jisx0208_0_len_first_char,
    jisx0208_0_char_offset,
    db_nr_chars_in_str,
    jisx0208_0_is_this_charset,
    jisx0208_0_len_first_substr,
    db_get_next_word,
    jisx0208_0_pos_first_char,
#ifdef _UNICODE_SUPPORT
    jisx0208_0_conv_to_uc16
#endif
};
/************************* End of EUCJP *************************************/

#endif /* _EUCJP_SUPPORT */

#ifdef _SHIFTJIS_SUPPORT

/************************* SHIFTJIS Specific Operations ************************/
static int jisx0201_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "JISX0201") && strstr (name, "-1"))
        return 0;

    return 1;
}


#ifdef _UNICODE_SUPPORT
static unsigned short jisx0201_1_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned char ch1 = mchar [0];

    if (ch1 >= 0xA1 && ch1 <= 0xDF)
        return 0xFF61 + ch1 - 0xA1;
    else if (ch1 == 0x5C)
        return 0x00A5;
    else if (ch1 == 0x80)
        return 0x005C;

    return ch1;
}
#endif

static CHARSETOPS CharsetOps_jisx0201_1 = {
    190,
    1,
    1,
    FONT_CHARSET_JISX0201_1,
    {' '},
    sb_len_first_char,
    sb_char_offset,
    sb_nr_chars_in_str,
    jisx0201_1_is_this_charset,
    sb_len_first_substr,
    sb_get_next_word,
    sb_pos_first_char,
#ifdef _UNICODE_SUPPORT
    jisx0201_1_conv_to_uc16
#endif
};

static int jisx0208_1_len_first_char (const unsigned char* mstr, int len)
{
    unsigned char ch1;
    unsigned char ch2;

    if (len < 2) return 0;

    ch1 = mstr [0];
    if (ch1 == '\0')
        return 0;

    ch2 = mstr [1];
    if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) && 
            ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
        return 2;

    return 0;
}

static unsigned int jisx0208_1_char_offset (const unsigned char* mchar)
{
    unsigned char ch1 = mchar [0];
    unsigned char ch2 = mchar [1];

#if 0
    if (ch1 >= 0x81 && ch1 <= 0x9F) {
        if (ch2 >= 0x40 && ch2 <= 0x7E) {
            return ((ch1 - 0x81) * 0x3F + ch2 - 0x40);
        }
        else {
            return (ch1 - 0x81) * 0x7D + ch2 - 0x80 + (0x1F * 0x3F);
        }
    }
    else {
        if (ch2 >= 0x40 && ch2 <= 0x7E) {
            return ((ch1 - 0xE0) * 0x3F + ch2 - 0x40) + 0x1F * (0x3F + 0x7D);
        }
        else {
            return ((ch1 - 0xE0) * 0x7D + ch2 - 0x80) + 0x1F * (0x3F + 0x7D) + 0x10 * 0x3F;
        }
    }
#else
    int adjust = ch2 < 159;
    int rowOffset = ch1 < 160 ? 112 : 176;
    int cellOffset = adjust ? (ch2 > 127 ? 32 : 31) : 126;

    ch1 = ((ch1 - rowOffset) << 1) - adjust;
    ch2 -= cellOffset;

    ch1 += 128;
    ch2 += 128;

    return ((ch1 - 0xA1) * 94 + ch2 - 0xA1);
#endif
}

static int jisx0208_1_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if ((strstr (name, "JISX0208") && strstr (name, "-1")) ||
            (strstr (name, "SHIFT") && strstr (name, "JIS")))
        return 0;

    return 1;
}

static int jisx0208_1_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;
    int sub_len = 0;

    left = mstrlen;
    for (i = 0; i < mstrlen; i += 2) {
        if (left < 2) return sub_len;

        ch1 = mstr [i];
        if (ch1 == '\0') return sub_len;

        ch2 = mstr [i + 1];
        if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) && 
                ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
            sub_len += 2;
        else
            return sub_len;

        left -= 2;
    }

    return sub_len;
}

static int jisx0208_1_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    unsigned char ch1;
    unsigned char ch2;
    int i, left;

    i = 0;
    left = mstrlen;
    while (left) {
        if (left < 2) return -1;

        ch1 = mstr [i];
        if (ch1 == '\0') return -1;

        ch2 = mstr [i + 1];
        if (((ch1 >= 0x81 && ch1 <= 0x9F) || (ch1 >= 0xE0 && ch1 <= 0xEF)) && 
                ((ch2 >= 0x40 && ch2 <= 0x7E) || (ch2 >= 0x80 && ch2 <= 0xFC)))
            return i;

        i += 1;
        left -= 1;
    }

    return -1;
}

#ifdef _UNICODE_SUPPORT
static unsigned short jisx0208_1_conv_to_uc16 (const unsigned char* mchar, int len)
{
    unsigned short ucs_code = jisx0208_1_unicode_map [jisx0208_1_char_offset (mchar)];

    if (ucs_code == 0)
        return '?';

    return ucs_code;
}
#endif

static CHARSETOPS CharsetOps_jisx0208_1 = {
    8836,
    2,
    2,
    FONT_CHARSET_JISX0208_1,
    {'\xA1', '\xA1'},
    jisx0208_1_len_first_char,
    jisx0208_1_char_offset,
    db_nr_chars_in_str,
    jisx0208_1_is_this_charset,
    jisx0208_1_len_first_substr,
    db_get_next_word,
    jisx0208_1_pos_first_char,
#ifdef _UNICODE_SUPPORT
    jisx0208_1_conv_to_uc16
#endif
};
/************************* End of SHIFTJIS *************************************/

#endif /* _SHIFTJIS_SUPPORT */

#ifdef _UNICODE_SUPPORT

/************************* UNICODE Specific Operations ************************/
static int utf8_len_first_char (const unsigned char* mstr, int len)
{
    int c = *((unsigned char *)mstr);
    int n = 1;

    if (c & 0x80) {
        while (c & (0x80 >> n))
            n++;
    }

    return n;
}

static unsigned short utf8_conv_to_uc16 (const unsigned char* mchar, int len)
{
    int c = *((unsigned char *)(mchar++));
    int n, t;

    if (c & 0x80) {
        n = 1;
        while (c & (0x80 >> n))
            n++;

        c &= (1 << (8-n)) - 1;
        while (--n > 0) {
            t = *((unsigned char *)(mchar++));

            if ((!(t & 0x80)) || (t & 0x40))
                return '^';

            c = (c << 6) | (t & 0x3F);
        }
    }

    return (unsigned short) c;
}

static unsigned int utf8_char_offset (const unsigned char* mchar)
{
    return utf8_conv_to_uc16 (mchar, 0);
}

static int utf8_nr_chars_in_str (const unsigned char* mstr, int mstrlen)
{
    int charlen;
    int n = 0;
    int left = mstrlen;

    while (left >= 0) {
        charlen = utf8_len_first_char (mstr, left);
        if (charlen > 0)
            n ++;

        left -= charlen;
        mstr += charlen;
    }

    return n;
}

static int utf8_is_this_charset (const unsigned char* charset)
{
    int i;
    char name [LEN_FONT_NAME + 1];

    for (i = 0; i < LEN_FONT_NAME + 1; i++) {
        if (charset [i] == '\0')
            break;
        name [i] = toupper (charset [i]);
    }
    name [i] = '\0';

    if (strstr (name, "UTF") && strstr (name, "8"))
        return 0;

    return 1;
}

static int utf8_len_first_substr (const unsigned char* mstr, int mstrlen)
{
    return mstrlen;
}

static int utf8_pos_first_char (const unsigned char* mstr, int mstrlen)
{
    return 0;
}

static CHARSETOPS CharsetOps_utf8 = {
    65535,
    2,
    4,
    FONT_CHARSET_UTF8,
    {'\xA1', '\xA1'},
    utf8_len_first_char,
    utf8_char_offset,
    utf8_nr_chars_in_str,
    utf8_is_this_charset,
    utf8_len_first_substr,
    sb_get_next_word,
    utf8_pos_first_char,
#ifdef _UNICODE_SUPPORT
    utf8_conv_to_uc16
#endif
};

/************************* End of UNICODE *************************************/
#endif /* _UNICODE_SUPPORT */

static CHARSETOPS* Charsets [] =
{
    &CharsetOps_ascii,
#ifdef _LATIN10_SUPPORT
    &CharsetOps_iso8859_16,
#endif
#ifdef _LATIN9_SUPPORT
    &CharsetOps_iso8859_15,
#endif
#ifdef _LATIN8_SUPPORT
    &CharsetOps_iso8859_14,
#endif
#ifdef _LATIN7_SUPPORT
    &CharsetOps_iso8859_13,
#endif
#ifdef _THAI_SUPPORT
    &CharsetOps_iso8859_11,
#endif
#ifdef _LATIN6_SUPPORT
    &CharsetOps_iso8859_10,
#endif
#ifdef _LATIN5_SUPPORT
    &CharsetOps_iso8859_9,
#endif
#ifdef _HEBREW_SUPPORT
    &CharsetOps_iso8859_8,
#endif
#ifdef _GREEK_SUPPORT
    &CharsetOps_iso8859_7,
#endif
#ifdef _ARABIC_SUPPORT
    &CharsetOps_iso8859_6,
#endif
#ifdef _CYRILLIC_SUPPORT
    &CharsetOps_iso8859_5,
#endif
#ifdef _LATIN4_SUPPORT
    &CharsetOps_iso8859_4,
#endif
#ifdef _LATIN3_SUPPORT
    &CharsetOps_iso8859_3,
#endif
#ifdef _LATIN2_SUPPORT
    &CharsetOps_iso8859_2,
#endif
    &CharsetOps_iso8859_1,
#ifdef _GB_SUPPORT
    &CharsetOps_gb2312_0,
#endif
#ifdef _GBK_SUPPORT
    &CharsetOps_gbk,
#endif
#ifdef _GB18030_SUPPORT
    &CharsetOps_gb18030_0,
#endif
#ifdef _BIG5_SUPPORT
    &CharsetOps_big5,
#endif
#ifdef _EUCKR_SUPPORT
    &CharsetOps_ksc5601_0,
#endif
#ifdef _EUCJP_SUPPORT
    &CharsetOps_jisx0201_0,
    &CharsetOps_jisx0208_0,
#endif
#ifdef _SHIFTJIS_SUPPORT
    &CharsetOps_jisx0201_1,
    &CharsetOps_jisx0208_1,
#endif
#ifdef _UNICODE_SUPPORT
    &CharsetOps_utf8,
#endif
};

#define NR_CHARSETS     (sizeof(Charsets)/sizeof(CHARSETOPS*))

CHARSETOPS* GetCharsetOps (const char* charset_name)
{
    int i;

    for (i = 0; i < NR_CHARSETS; i++) {
        if ((*Charsets [i]->is_this_charset) (charset_name) == 0)
            return Charsets [i];
    }

    return NULL;
}

