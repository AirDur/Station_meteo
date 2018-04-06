/*
** $Id: systext.c,v 1.7 2004/07/16 02:39:33 linxs Exp $
**
** systext.c: GetSysText function.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/12/31
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

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"

/*
 * This function translates system strings.
 * You can use gettext to return the text.
 *
 * System text as follows:
 *
 const char* SysText [] =
 {
    "Windows...",           // 0
    "Start...",             // 1
    "Refresh Background",   // 2
    "Close All Windows",    // 3
    "End Session",          // 4
    "Operations...",        // 5
    "Minimize",             // 6
    "Maximize",             // 7
    "Restore",              // 8
    "Close",                // 9
    "OK",                   // 10
    "Next",                 // 11
    "Cancel",               // 12
    "Previous",             // 13
    "Yes",                  // 14
    "No",                   // 15
    "Abort",                // 16
    "Retry",                // 17
    "Ignore",               // 18
    "About MiniGUI...",     // 19
    "Open File",            // 20
    "Save File",            // 21
    "Color Selection",      // 22
    NULL
 };
 */

const char** local_SysText;
const char* GetSysText (const char* text)
{
    int i;

    if (local_SysText == SysText || local_SysText == NULL)
        return text;

    i = 0;
    while (SysText [i]) {
        if (strcmp (text, SysText [i]) == 0)
            return local_SysText [i];
        i ++;
    }

    return text;
}

