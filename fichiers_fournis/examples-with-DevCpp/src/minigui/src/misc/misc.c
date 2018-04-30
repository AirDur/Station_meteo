/*
** $Id: misc.c,v 1.57.4.2 2005/03/02 02:59:49 panweiguo Exp $
**
** misc.c: This file include some miscelleous functions. 
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1998/12/31
**
** Current maintainer: Wei Yongming.
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
#include <errno.h>
#include <limits.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "misc.h"

#include <unistd.h>
#include <time.h>
#include <pwd.h>

#ifdef _CLIPBOARD_SUPPORT
#include "clipboard.h"
#endif

/* Handle of MiniGUI etc file object */
GHANDLE hMgEtc = 0;

#ifndef _INCORE_RES

/* Initialize MiniGUI etc file object, call before accessing MiniGUI etc value */
BOOL InitMgEtc (void)
{
    if (hMgEtc)
        return TRUE;

    if ( !(hMgEtc = LoadEtcFile (ETCFILEPATH)) )
        return FALSE;
    return TRUE;
}

/* Terminate MiniGUI etc file object */
void TerminateMgEtc (void)
{
    UnloadEtcFile (hMgEtc);
    hMgEtc = 0;
}

#else

extern ETC_S MGETC;

BOOL InitMgEtc (void)
{
//    extern ETC_S MGETC;
//    hMgEtc = (GHANDLE) &MGETC;
    return TRUE;
}

void TerminateMgEtc (void) { }

#endif /* _INCORE_RES */
#ifndef _INCORE_RES

char ETCFILEPATH [MAX_PATH + 1];
#define ETCFILENAME "MiniGUI.cfg"

static BOOL LookForEtcFile (void)
{
    char etcfile [MAX_PATH + 1];
    char buff [10];
    struct passwd *pwd;

    if ((pwd = getpwuid (geteuid ())) != NULL) {
        strcpy (etcfile, pwd->pw_dir);
        if (etcfile [strlen (etcfile) - 1] != '/')
            strcat (etcfile, "/");
        strcat (etcfile, ".");
        strcat (etcfile, ETCFILENAME);

        if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
            strcpy (ETCFILEPATH, etcfile);
            return TRUE;
        }
    }

    strcpy (etcfile, "/usr/local/etc/" ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    strcpy (etcfile, "/etc/" ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    getcwd (etcfile, MAX_PATH);
    strcat (etcfile, "/");
    strcat (etcfile, ETCFILENAME);
    if (GetValueFromEtcFile (etcfile, "system", "gal_engine", buff, 8) ==  ETC_OK) {
        strcpy (ETCFILEPATH, etcfile);
        return TRUE;
    }

    return FALSE;
}

#endif /* _INCORE_RES */

BOOL InitMisc (void)
{
#ifndef _INCORE_RES
    if (!LookForEtcFile ())
    {
        fprintf (stderr, "MISC: Can not locate your MiniGUI.cfg file or bad files!\n");
        return FALSE;
    }
#endif

#ifdef _CLIPBOARD_SUPPORT
      uHALr_printf("Clipboard not initialized!!!\n");
//    InitClipBoard ();
#endif

    return InitMgEtc ();
}

void TerminateMisc (void)
{
#ifdef _CLIPBOARD_SUPPORT
    TerminateClipBoard ();
#endif

    TerminateMgEtc ();
}

/****************************** ETC file support ******************************/

static char* get_section_name (char *section_line)
{
    char* current;
    char* tail;
    char* name;

    if (!section_line)
        return NULL;

    current = section_line;

    while (*current == ' ' ||  *current == '\t') current++; 

    if (*current == ';' || *current == '#')
        return NULL;

    if (*current++ == '[')
        while (*current == ' ' ||  *current == '\t') current ++;
    else
        return NULL;

    name = tail = current;
    while (*tail != ']' && *tail != '\n' &&
          *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;
    *tail = '\0';
    while (*tail == ' ' || *tail == '\t') {
        *tail = '\0';
        tail--; 
    }

    return name;
}

static int get_key_value (char *key_line, char **mykey, char **myvalue)
{
    char* current;
    char* tail;
    char* value;

    if (!key_line)
        return -1;

    current = key_line;

    while (*current == ' ' ||  *current == '\t') current++; 

    if (*current == ';' || *current == '#')
        return -1;

    if (*current == '[')
        return 1;

    if (*current == '\n' || *current == '\0')
        return -1;

    tail = current;
    while (*tail != '=' && *tail != '\n' &&
          *tail != ';' && *tail != '#' && *tail != '\0')
          tail++;

    value = tail + 1;
    if (*tail != '=')
        *value = '\0'; 

    *tail-- = '\0';
    while (*tail == ' ' || *tail == '\t') {
        *tail = '\0';
        tail--; 
    }
        
    tail = value;
    while (*tail != '\n' && *tail != '\0') tail++;
    *tail = '\0'; 

    if (mykey)
        *mykey = current;
    if (myvalue)
        *myvalue = value;

    return 0;
}


/* This function locate the specified section in the etc file. */
static int etc_LocateSection(FILE* fp, const char* pSection, FILE* bak_fp)
{
    char szBuff[ETC_MAXLINE + 1];
    char *name;

    while (TRUE) {
        if (!fgets(szBuff, ETC_MAXLINE, fp)) {
            if (feof (fp))
                return ETC_SECTIONNOTFOUND;
            else
                return ETC_FILEIOFAILED;
        }
        else if (bak_fp && fputs (szBuff, bak_fp) == EOF)
            return ETC_FILEIOFAILED;
        
        name = get_section_name (szBuff);
        if (!name)
            continue;

        if (strcmp (name, pSection) == 0)
            return ETC_OK; 
    }

    return ETC_SECTIONNOTFOUND;
}

/* This function locate the specified key in the etc file. */
static int etc_LocateKeyValue(FILE* fp, const char* pKey, 
                               BOOL bCurSection, char* pValue, int iLen,
                               FILE* bak_fp, char* nextSection)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* current;
    char* value;
    int ret;

    while (TRUE) {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp))
            return ETC_FILEIOFAILED;
        bufflen = strlen (szBuff);
        if (szBuff [bufflen - 1] == '\n')
            szBuff [bufflen - 1] = '\0';

        ret = get_key_value (szBuff, &current, &value);
        if (ret < 0)
            continue;
        else if (ret > 0) {
            fseek (fp, -bufflen, SEEK_CUR);
            return ETC_KEYNOTFOUND;
        }
            
        if (strcmp (current, pKey) == 0) {
            if (pValue)
                strncpy (pValue, value, iLen);

            return ETC_OK; 
        }
        else if (bak_fp && *current != '\0')
            fprintf (bak_fp, "%s=%s\n", current, value);
    }

    return ETC_KEYNOTFOUND;
}

static int etc_ReadSection (FILE* fp, PETCSECTION psect)
{
    char szBuff[ETC_MAXLINE + 1 + 1];
    char* sect_name;
    int max_key_nr = 5;

    psect->name = NULL;
    psect->key_nr = 0;

    while (TRUE) {
        int bufflen;

        if (!fgets(szBuff, ETC_MAXLINE, fp)) {
            if (feof (fp)) {
                if (psect->name)
                    break;
                else
                    return ETC_SECTIONNOTFOUND;
            }
            else
                return ETC_FILEIOFAILED;
        }

        bufflen = strlen (szBuff);
        if (szBuff [bufflen - 1] == '\n')
            szBuff [bufflen - 1] = '\0';

        if (!psect->name) { /* read section name */
            sect_name = get_section_name (szBuff);
            if (!sect_name)
                continue;

            psect->name = strdup (sect_name);
            psect->key_nr = 0;
            psect->keys = malloc (sizeof(char*)*max_key_nr);
            psect->values = malloc (sizeof(char*)*max_key_nr);
        }
        else { /* read key and value */
            int ret;
            char *key, *value;

            if (psect->key_nr == max_key_nr) {
                max_key_nr += 5;
                psect->keys = (char **) realloc (psect->keys, sizeof(char*)*max_key_nr);
                psect->values = (char **) realloc (psect->values, sizeof(char*)*max_key_nr);
            }

            ret = get_key_value (szBuff, &key, &value);
            if (ret < 0)
                continue;
            else if (ret > 0) {  /* another section begins */
                fseek (fp, -bufflen, SEEK_CUR);
                break;
            }

            *(psect->keys + psect->key_nr) = strdup (key);
            *(psect->values + psect->key_nr) = strdup (value);
            psect->key_nr ++;
        }
    }

    return ETC_OK;
}

GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
{
    FILE* fp;
    int max_sect_nr = 15;
    ETC_S *petc;

    if (!(fp = fopen(pEtcFile, "r")))
         return ETC_FILENOTFOUND;

    petc = (ETC_S*) malloc (sizeof(ETC_S));
    petc->section_nr = 0;
    /* we allocate 15 sections first */
    petc->sections = (PETCSECTION) malloc (sizeof(ETCSECTION)*max_sect_nr);

    while (etc_ReadSection (fp, petc->sections + petc->section_nr) == ETC_OK) {
        petc->section_nr ++;
        if (petc->section_nr == max_sect_nr) {
            /* add 5 sections each time we realloc */
            max_sect_nr += 5;
            petc->sections = realloc (petc->sections, sizeof(ETCSECTION)*max_sect_nr);
        }
    }

    fclose (fp);
    return (GHANDLE)petc;
}

int GUIAPI UnloadEtcFile (GHANDLE hEtc)
{
    int i;
    ETC_S *petc = (ETC_S*) hEtc;

    if (!petc)
        return -1;

    for (i=0; i<petc->section_nr; i++) {
        PETCSECTION psect = petc->sections + i;
        int j;

        if (!psect)
           continue;

        for (j=0; j<psect->key_nr; j++) {
            free (psect->keys[j]);
            free (psect->values[j]);
        }
        free (psect->keys);
        free (psect->values);
        free (psect->name);
    }

    free (petc->sections);
    free (petc);

    return 0;
}

int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection,
                            const char* pKey, char* pValue, int iLen)
{
    int i;
    ETC_S *petc = (ETC_S*) hEtc;
    PETCSECTION psect = NULL;

    if (!petc || !pValue)
        return -1;

    for (i=0; i<petc->section_nr; i++) {
        psect = petc->sections + i;
        if (!psect)
           continue;

        if (strcmp (psect->name, pSection) == 0) {
            break;
        }
    }

    if (i >= petc->section_nr)
        return ETC_SECTIONNOTFOUND;

    for (i=0; i<psect->key_nr; i++) {
        if (strcmp (psect->keys[i], pKey) == 0) {
            break;
        }
    }

    if (i >= psect->key_nr)
        return ETC_KEYNOTFOUND;

    if (iLen > 0) { /* get value */
        strncpy (pValue, psect->values[i], iLen);
    }
    else { /* set value */
        free (psect->values[i]);
        psect->values[i] = strdup(pValue);
    }

    return ETC_OK;
}

int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection,
                               const char* pKey, int* value)
{
    int ret;
    char szBuff [51];

    ret = GetValueFromEtc (hEtc, pSection, pKey, szBuff, 50);
    if (ret < 0) {
        return ret;
    }

    *value = strtol (szBuff, NULL, 0);
    if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
        return ETC_INTCONV;

    return ETC_OK;
}

/* Function: GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue, int iLen);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   The buffer will store the value of the key.
 *     iLen:     The max length of value string.
 * Return:
 *     int               meaning
 *     ETC_FILENOTFOUND           The etc file not found. 
 *     ETC_SECTIONNOTFOUND        The section is not found. 
 *     ETC_EKYNOTFOUND        The Key is not found.
 *     ETC_OK            OK.
 */
int GUIAPI GetValueFromEtcFile(const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue, int iLen)
{
    FILE* fp;
    char tempSection [ETC_MAXLINE + 2];

    if (!(fp = fopen(pEtcFile, "r")))
         return ETC_FILENOTFOUND;

    if (pSection)
         if (etc_LocateSection (fp, pSection, NULL) != ETC_OK) {
             fclose (fp);
             return ETC_SECTIONNOTFOUND;
         }

    if (etc_LocateKeyValue (fp, pKey, pSection != NULL, 
                pValue, iLen, NULL, tempSection) != ETC_OK) {
         fclose (fp);
         return ETC_KEYNOTFOUND;
    }

    fclose (fp);
    return ETC_OK;
}

/* Function: GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 * Return:
 *     int                      meaning
 *     ETC_FILENOTFOUND             The etc file not found. 
 *     ETC_SECTIONNOTFOUND          The section is not found. 
 *     ETC_EKYNOTFOUND              The Key is not found.
 *     ETC_OK                       OK.
 */
int GUIAPI GetIntValueFromEtcFile(const char* pEtcFile, const char* pSection,
                               const char* pKey, int* value)
{
    int ret;
    char szBuff [51];

    ret = GetValueFromEtcFile (pEtcFile, pSection, pKey, szBuff, 50);
    if (ret < 0)
        return ret;

    *value = strtol (szBuff, NULL, 0);
    if ((*value == LONG_MIN || *value == LONG_MAX) && errno == ERANGE)
        return ETC_INTCONV;

    return ETC_OK;
}

static int etc_CopyAndLocate (FILE* etc_fp, FILE* tmp_fp, 
                const char* pSection, const char* pKey, char* tempSection)
{
    if (pSection && etc_LocateSection (etc_fp, pSection, tmp_fp) != ETC_OK)
        return ETC_SECTIONNOTFOUND;

    if (etc_LocateKeyValue (etc_fp, pKey, pSection != NULL, 
                NULL, 0, tmp_fp, tempSection) != ETC_OK)
        return ETC_KEYNOTFOUND;

    return ETC_OK;
}

static int etc_FileCopy (FILE* sf, FILE* df)
{
    char line [ETC_MAXLINE + 1];
    
    while (fgets (line, ETC_MAXLINE + 1, sf) != NULL)
        if (fputs (line, df) == EOF) {
            return ETC_FILEIOFAILED;
        }

    return ETC_OK;
}

/* Function: SetValueToEtcFile(const char* pEtcFile, const char* pSection,
 *                               const char* pKey, char* pValue);
 * Parameter:
 *     pEtcFile: etc file path name.
 *     pSection: Section name.
 *     pKey:     Key name.
 *     pValue:   Value.
 * Return:
 *     int                      meaning
 *     ETC_FILENOTFOUND         The etc file not found.
 *     ETC_TMPFILEFAILED        Create tmp file failure.
 *     ETC_OK                   OK.
 */
int GUIAPI SetValueToEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue)
{
    FILE* etc_fp;
    FILE* tmp_fp;
    int rc;
    char tempSection [ETC_MAXLINE + 2];

#ifndef HAVE_TMPFILE
    char tmp_nam [256];

    sprintf (tmp_nam, "/tmp/mg-etc-tmp-%lx", time(NULL));
    if ((tmp_fp = fopen (tmp_nam, "w+")) == NULL)
        return ETC_TMPFILEFAILED;
#else
    if ((tmp_fp = tmpfile ()) == NULL)
        return ETC_TMPFILEFAILED;
#endif

    if (!(etc_fp = fopen (pEtcFile, "r+"))) {
        fclose (tmp_fp);
#ifndef HAVE_TMPFILE
        unlink (tmp_nam);
#endif
        if (!(etc_fp = fopen (pEtcFile, "w"))) {
            return ETC_FILEIOFAILED;
        }
        fprintf (etc_fp, "[%s]\n", pSection);
        fprintf (etc_fp, "%s=%s\n", pKey, pValue);
        fclose (etc_fp);
        return ETC_OK;
    }

    switch (etc_CopyAndLocate (etc_fp, tmp_fp, pSection, pKey, tempSection)) {
    case ETC_SECTIONNOTFOUND:
        fprintf (tmp_fp, "\n[%s]\n", pSection);
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;
    
    case ETC_KEYNOTFOUND:
        fprintf (tmp_fp, "%s=%s\n\n", pKey, pValue);
        fprintf (tmp_fp, "%s\n", tempSection);
    break;

    default:
        fprintf (tmp_fp, "%s=%s\n", pKey, pValue);
        break;
    }

    if ((rc = etc_FileCopy (etc_fp, tmp_fp)) != ETC_OK)
        goto error;
    
    // replace etc content with tmp file content
    // truncate etc content first
    fclose (etc_fp);
    if (!(etc_fp = fopen (pEtcFile, "w"))) {
        fclose (tmp_fp);
#ifndef HAVE_TMPFILE
        unlink (tmp_nam);
#endif
        return ETC_FILEIOFAILED;
    }
    
    rewind (tmp_fp);
    rc = etc_FileCopy (tmp_fp, etc_fp);

error:
    fclose (etc_fp);
    fclose (tmp_fp);
#ifndef HAVE_TMPFILE
    unlink (tmp_nam);
#endif
    return rc;
}

/****************************** Ping and Beep *********************************/
void GUIAPI Ping(void)
{
    putchar ('\a');
    fflush (stdout);
}

#if !defined (__NOUNIX__) && !defined(__CYGWIN__) && defined(i386)

#include <linux/kd.h>
#include <asm/param.h>

void GUIAPI Tone (int frequency_hz, int duration_ms)
{
    /* FIXME: Tone will not work in X Window */
    long argument = (1190000 / frequency_hz) | ((duration_ms / (1000/HZ)) << 16);

    ioctl (0, KDMKTONE, (long) argument);
}
#endif


char* strnchr (const char* s, size_t n, int c)
{
    size_t i;
    
    for (i=0; i<n; i++) {
        if ( *s == c)
            return (char *)s;

        s ++;
    }

    return NULL;
}

int substrlen (const char* text, int len, char delimiter, int* nr_delim)
{
    char* substr;

    *nr_delim = 0;

    if ( (substr = strnchr (text, len, delimiter)) == NULL)
        return len;

    len = substr - text;

    while (*substr == delimiter) {
        (*nr_delim) ++;
        substr ++;
    }

    return len;
}

char * strtrimall( char *src)
{
    int  nIndex1;
    int  nLen;

    if (src == NULL)
        return NULL;

    if (src [0] == '\0')
        return src;

    nLen = strlen (src);

    nIndex1 = 0;
    while (isspace (src [nIndex1]))
        nIndex1 ++;

    if (nIndex1 == nLen) {
        *src = '\0';
        return src;
    }

    strcpy (src, src + nIndex1);

    nLen = strlen (src);
    nIndex1 = nLen - 1;
    while (isspace (src [nIndex1]))
        nIndex1 --;

    src [nIndex1 + 1] = '\0';  

    return src;
}



