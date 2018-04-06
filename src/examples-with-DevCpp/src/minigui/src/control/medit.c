/*
** $Id: medit.c,v 1.78 2005/01/31 09:12:48 clear Exp $
**
** medit.c: the Multi-Line Edit control
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Chen Lei and Wei Yongming.
** 
** Current maintainer: Chen Lei (leon@minigui.org).
**
** Create date: 1999/8/26
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
**    * Selection.
**    * Undo.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "minigui.h"
#include "gdi.h"
#include "window.h"
#include "ctrl/edit.h"
#include "cliprect.h"
#include "internals.h"
#include "ctrlclass.h"

#ifdef _CTRL_OLDMEDIT

#include "ctrlmisc.h"
#include "medit_impl.h"

static int MLEditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

BOOL RegisterMLEditControl (void)
{
    // This control class has two names: "medit" and "mledit"

    WNDCLASS WndClass;

    WndClass.spClassName = CTRL_OLDMEDIT;
    WndClass.dwStyle     = WS_NONE;
    WndClass.dwExStyle   = WS_EX_NONE;
    WndClass.hCursor     = GetSystemCursor (IDC_IBEAM);
    WndClass.iBkColor    = GetWindowElementColor (BKC_EDIT_DEF);
    WndClass.WinProc     = MLEditCtrlProc;

    return AddNewControlClass (&WndClass) == ERR_OK;
}

#if 0
void MLEditControlCleanup (void)
{
    // do nothing
    return;
}
#endif

//FIXME
static inline int edtGetOutWidth (HWND hWnd)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    RECT rc;
    GetClientRect(hWnd,&rc);

    return RECTW(rc) - pMLEditData->leftMargin - pMLEditData->rightMargin;
}

static int edtGetLineNO (const MLEDITDATA* pMLEditData, int y)
{
    int nline = 0;
    if(y>=0)
    {
        nline = (y - pMLEditData->topMargin) / pMLEditData->lineHeight;
        if (nline <= pMLEditData->linesDisp)
               return nline;
    }
    return -1;
}

int GetRETURNPos(char *str, int len)
{
    int i;

    for(i=0;i<len;i++)
    {    
        if(str[i]==10)
            return i;
    }
    return -1;
}

void MLEditEmptyBuffer(PMLEDITDATA pMLEditData)
{
    PLINEDATA temp;
    PLINEDATA  pLineData;
    pLineData = pMLEditData->head;
    while(pLineData)
    {
        temp = pLineData->next;
        free(pLineData);
        pLineData = temp;
    }
    pMLEditData->head = NULL;
}

// get next wrap line start address according to startPos
// > 0 : next start position
// -1  : end of current line
static int edtGetnextStartPos (HWND hWnd, PMLEDITDATA pMLEditData, int startPos)
{
    int        i = 0;
    
    if (!pMLEditData->dx_chars) return -1;

    if (pMLEditData->sz.cx - pMLEditData->dx_chars[startPos] 
                                            <= edtGetOutWidth (hWnd))
        return -1;
    
    for (i = startPos; i < pMLEditData->fit_chars+1; i++) {
        if (pMLEditData->dx_chars[i]-pMLEditData->dx_chars[startPos] >= edtGetOutWidth (hWnd))
            return i-1;
    }
    return -1;
}

static void edtGetLineInfoEx (HWND hWnd, PMLEDITDATA pMLEditData, PLINEDATA pLineData)
{
    HDC hdc = GetClientDC (hWnd);
    if (!pMLEditData) pMLEditData = (PMLEDITDATA) GetWindowAdditionalData2(hWnd);

#if 0
    if (pMLEditData->logfont)
        SelectFont (hdc, pMLEditData->logfont);
#endif

    GetTextExtentPoint (hdc,
                        pLineData->buffer, 
                        pLineData->dataEnd,  
                        GetMaxFontWidth (hdc)*LEN_MLEDIT_BUFFER,
                        &(pMLEditData->fit_chars), 
                        pMLEditData->pos_chars, 
                        pMLEditData->dx_chars, 
                        &(pMLEditData->sz));
    ReleaseDC (hdc);

    if (pMLEditData->pos_chars)
        pMLEditData->pos_chars[pMLEditData->fit_chars] = pLineData->dataEnd;
    if (pMLEditData->dx_chars)
        pMLEditData->dx_chars[pMLEditData->fit_chars] = pMLEditData->sz.cx;
}

static void edtGetLineInfo (HWND hWnd, PLINEDATA pLineData)
{
    edtGetLineInfoEx (hWnd, NULL, pLineData);
}

static int edtGetStartDispPosAtEnd (HWND hWnd,
            PLINEDATA pLineData)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    int            i = 0;
    int         dist = MAX_IMPOSSIBLE;
    int         newdist = 0;
    
    edtGetLineInfo (hWnd, pLineData);

    if (pMLEditData->sz.cx <= edtGetOutWidth (hWnd)) 
        return 0;
    
    for (i = 0; i < pMLEditData->fit_chars-1; i++) {
        newdist = (pMLEditData->sz.cx - edtGetOutWidth (hWnd)) - pMLEditData->dx_chars[i];
        if (newdist >= 0 && newdist < dist) {
            dist = newdist;
            if (dist == 0) { return i; } 
        }else { return i; }
    }
    return 0;
}

void calcLineInfo (HWND hWnd, PMLEDITDATA pMLEditData, PLINEDATA pLineData)
{
    int i;

    edtGetLineInfoEx (hWnd, pMLEditData, pLineData);

    for ( i=0,pLineData->nwrapline=1,pLineData->wrapStartPos[0]=0; 
                    (i = edtGetnextStartPos(hWnd, pMLEditData,i)) > 0; 
                    pLineData->nwrapline++ )
    {
        pLineData->wrapStartPos[pLineData->nwrapline] = i;
    }
    pLineData->wrapStartPos[pLineData->nwrapline] = pMLEditData->fit_chars; 
}

void MLEditInitBuffer (HWND hWnd, PMLEDITDATA pMLEditData, const char *spcaption, DWORD dwStyle)
{
    char *caption=(char*)spcaption; 
    int off1;
    int lineNO=0;
    int pace=0;
    int totallen=0;

    PLINEDATA  pLineData;

    if (!(pMLEditData->head = malloc (sizeof (LINEDATA)))) {
        return ;
    }
    pMLEditData->head->previous = NULL;
    pLineData = pMLEditData->head;    
    pMLEditData->wraplines = 0;

    totallen = strlen(caption);

    while ( (off1 = GetRETURNPos(caption, totallen)) != -1)
    {
        int off;
        off1 = MIN (off1, LEN_MLEDIT_BUFFER);
        if ((char)caption[off1-1] == '\r')
            off = off1-1;
        else
            off = off1;
#if 0
        if ((pMLEditData->curtotalLen + MIN (off,LEN_MLEDIT_BUFFER) >= pMLEditData->totalLimit)
            && pMLEditData->totalLimit != -1)
        {
            pLineData->dataEnd = pMLEditData->totalLimit-pMLEditData->curtotalLen;

            pace = MIN (pLineData->dataEnd, LEN_MLEDIT_BUFFER) + 1;
            caption += pace;
			totallen -= pace;

            if (pLineData->dataEnd-2 > 0 && (char)caption[pLineData->dataEnd-2] == '\r')
                pLineData->dataEnd -= 2;
            else if (pLineData->dataEnd-1 > 0 && (char)caption[pLineData->dataEnd-1] == '\n')
                pLineData->dataEnd --;

            printf ("dateEnd = %d\n", pLineData->dataEnd);

            memcpy(pLineData->buffer,caption, pLineData->dataEnd);
            pLineData->buffer[pLineData->dataEnd] = '\0';
            pLineData->lineNO  = lineNO++;
            pMLEditData->curtotalLen += pLineData->dataEnd;
            
            if (!pLineData->dataEnd)
            {
                pLineData->nwrapline = 1;
                pLineData->wrapStartPos[0] = 0;
                pLineData->wrapStartPos[1] = 0;
            } else
                calcLineInfo (hWnd, pMLEditData, pLineData);

            pMLEditData->wraplines += pLineData->nwrapline;
            pLineData->lineNO  = lineNO++;    

            if (pMLEditData->curtotalLen == pMLEditData->totalLimit)
            {
                pLineData->next = NULL;
                break;
            } else if (pMLEditData->curtotalLen + 1 < pMLEditData->totalLimit)
                pMLEditData->curtotalLen ++;
            pLineData->next    = malloc (sizeof (LINEDATA));
            pLineData->next->previous = pLineData; 
            pLineData->next->next = NULL;
            pLineData = pLineData->next;
            break;
        }
#endif
        memcpy(pLineData->buffer,caption, off1);
        if (pLineData->buffer[off1-1] == '\r')
            pLineData->buffer[off1-1] = '\0';
        else
            pLineData->buffer[off1] = '\0';
        
        pace = MIN (off1,LEN_MLEDIT_BUFFER)+1;
        caption += pace;
        
        pLineData->lineNO  = lineNO++;    
        pLineData->dataEnd = strlen(pLineData->buffer); 
        if (!pLineData->dataEnd)
        {
            pLineData->nwrapline = 1;
            pLineData->wrapStartPos[0] = 0;
            pLineData->wrapStartPos[1] = 0;
        } else
            calcLineInfo (hWnd, pMLEditData, pLineData);

        pMLEditData->wraplines += pLineData->nwrapline;

        pMLEditData->curtotalLen += pLineData->dataEnd;
        if (pMLEditData->curtotalLen == pMLEditData->totalLimit)
        {
            pLineData->next = NULL;
            break;
        } else if (pMLEditData->curtotalLen + 1 < pMLEditData->totalLimit)
            pMLEditData->curtotalLen ++;

        pLineData->next    = malloc (sizeof (LINEDATA));
        pLineData->next->previous = pLineData; 
        pLineData->next->next = NULL;
        pLineData = pLineData->next;
    }
    if (pMLEditData->totalLimit > 0 && pMLEditData->curtotalLen < pMLEditData->totalLimit)
    {
        int off=0;
        off1 = MIN (strlen(caption),LEN_MLEDIT_BUFFER);
        if (off1 && (char)caption[off1-1] == '\r')
            off = off1-1;
        else
            off = off1;
        if (pMLEditData->curtotalLen + MIN (off,LEN_MLEDIT_BUFFER) >= pMLEditData->totalLimit)
        {
            pLineData->dataEnd = pMLEditData->totalLimit-pMLEditData->curtotalLen;
            if (pLineData->dataEnd-2 > 0 && (char)caption[pLineData->dataEnd-2] == '\r')
                pLineData->dataEnd -= 2;
            else if (pLineData->dataEnd-1 > 0 && (char)caption[pLineData->dataEnd-1] == '\n')
                pLineData->dataEnd --;

            memcpy(pLineData->buffer,caption, pLineData->dataEnd);
            pLineData->buffer[pLineData->dataEnd] = '\0';

        }else {
            memcpy(pLineData->buffer,caption,off1);
            if (off1 && pLineData->buffer[off1-1] == '\r')
                pLineData->buffer[off1-1] = '\0';
            else
                pLineData->buffer[off1] = '\0';
            pLineData->dataEnd         = strlen(pLineData->buffer); 
            pLineData->next            = NULL; 

        }
        pMLEditData->curtotalLen += pLineData->dataEnd;
        if (pMLEditData->curtotalLen == pMLEditData->totalLimit-1)
            pMLEditData->curtotalLen = pMLEditData->totalLimit;
        if (!pLineData->dataEnd)
        {
            pLineData->nwrapline = 1;
            pLineData->wrapStartPos[0] = 0;
            pLineData->wrapStartPos[1] = 0;
        } else
              calcLineInfo (hWnd, pMLEditData, pLineData);

        pMLEditData->wraplines += pLineData->nwrapline;
        pLineData->lineNO = lineNO++;
    }

    pMLEditData->lines      = lineNO; 

    if (dwStyle & ES_AUTOWRAP)
        pMLEditData->linesDisp     =  MIN (pMLEditData->wraplines, pMLEditData->MaxlinesDisp);
    else
        pMLEditData->linesDisp     =  MIN (lineNO , pMLEditData->MaxlinesDisp);
    pMLEditData->EndlineDisp = pMLEditData->StartlineDisp + pMLEditData->linesDisp -1;
}

PLINEDATA GetLineData(PMLEDITDATA pMLEditData,int lineNO)
{
    PLINEDATA pLineData=pMLEditData->head;
    for (; pLineData; pLineData=pLineData->next)
    {
        if(pLineData->lineNO==lineNO)
            return pLineData;
    }
    return NULL;
}

int GetMaxLen (HWND hWnd, PMLEDITDATA pMLEditData)
{
    int i;
    PLINEDATA pLineData = pMLEditData->head;
    
    i = 0;
    while (pLineData)
    {
        edtGetLineInfo (hWnd, pLineData);
        if (pMLEditData->sz.cx > i) 
            i = pMLEditData->sz.cx;
        pLineData = pLineData->next;
    }
    return i;
}

// get the valid unit position in the array 
// according to the pos 
// which is relative to the original point of current line (0)  
static int  edtGetNewvPos (HWND hWnd, PLINEDATA pLineData , int pos)
{
    int i = 0;
    int dist = MAX_IMPOSSIBLE;
    int newdist = 0;
    
    PMLEDITDATA pMLEditData = (PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    edtGetLineInfo (hWnd, pLineData);

    if (pos < 0) return -1;
    if (pos > pMLEditData->sz.cx) return -2;
    for (i=0; i<pMLEditData->fit_chars + 1; i++) {
        newdist = pos - pMLEditData->dx_chars[i];
        if (newdist > 0 && newdist < dist)
            dist = newdist;
        else 
             return i;
    }
    return 0;
}

static void edtSetScrollInfo (HWND hWnd, PMLEDITDATA pMLEditData, BOOL fRedraw)
{
    SCROLLINFO     si;
    PLINEDATA     pLineData;
    RECT         rc;
    int         viLn;

    // Vscroll
    GetClientRect (hWnd, &rc);
    viLn = (rc.bottom - rc.top - pMLEditData->topMargin - pMLEditData->bottomMargin) / pMLEditData->lineHeight;
    
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    
    if ((GetWindowStyle(hWnd)) & ES_AUTOWRAP) 
        si.nMax = pMLEditData->wraplines - 1;
    else
        si.nMax = pMLEditData->lines - 1;

    si.nMin = 0;
    si.nPage = MIN (pMLEditData->EndlineDisp - pMLEditData->StartlineDisp + 1, viLn);
    
    si.nPos = pMLEditData->StartlineDisp;
    
    if ((si.nMax < viLn) && (!si.nPos)) {
        si.nMax = 100;
        si.nMin = 0;
        si.nPage = 10;
        si.nPos = 0;
        SetScrollInfo (hWnd, SB_VERT, &si, fRedraw);
//        EnableScrollBar (hWnd, SB_VERT, FALSE);
    } else {
        SetScrollInfo (hWnd, SB_VERT, &si, fRedraw);
//        EnableScrollBar (hWnd, SB_VERT, TRUE);
    }
    
    if ((GetWindowStyle(hWnd)) & ES_AUTOWRAP) 
    {
        EnableScrollBar (hWnd, SB_HORZ, FALSE);
        return;
    }
    // Hscroll
    pLineData = GetLineData(pMLEditData, pMLEditData->editLine);
    si.nMax = GetMaxLen(hWnd, pMLEditData) - 1;
    si.nMin = 0;
    si.nPage = edtGetOutWidth (hWnd);
    edtGetLineInfo (hWnd, pLineData);
    si.nPos = pMLEditData->dispPos;
            
    if (si.nMax < si.nPage && (!si.nPos)) {
        si.nMax = 100;
        si.nMin = 0;
        si.nPage = 10;
        si.nPos = 0;
        SetScrollInfo (hWnd, SB_HORZ, &si, fRedraw);
//        EnableScrollBar (hWnd, SB_HORZ, FALSE);
    } else {
        SetScrollInfo (hWnd, SB_HORZ, &si, fRedraw);
//        EnableScrollBar (hWnd, SB_HORZ, TRUE);
    }
}

int edtGetLineNums (PMLEDITDATA pMLEditData, int curline, int* line, int* wrapLine)
{
       PLINEDATA      pLineData = pMLEditData->head;
    int startline = 0;

    if (!curline)
    {
        *line = 0;
        *wrapLine = 0;
        return 0;
    }

    for (; pLineData; pLineData=pLineData->next)
    {
        if (startline+pLineData->nwrapline-1 >= curline)
        {
            *line = pLineData->lineNO;
            *wrapLine = curline - startline;
            return 0;
        }
        startline +=pLineData->nwrapline;
    }
    return -1;
}

// to set the Caret to a valid pos
BOOL edtGetCaretValid (HWND hWnd, int lineNO, int olddispPos, int newOff/*of caret*/)
{
    int line, wrapline;
    int dist = MAX_IMPOSSIBLE;
    int i, newdist = 0;
    PLINEDATA temp;
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
        
    if (GetWindowStyle(hWnd) & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, lineNO, &line, &wrapline);
        temp = GetLineData (pMLEditData, line);
        edtGetLineInfo (hWnd, temp);

        if( newOff >= pMLEditData->dx_chars[temp->wrapStartPos[wrapline+1]]
                            - pMLEditData->dx_chars[temp->wrapStartPos[wrapline]])
        {
            if (wrapline < temp->nwrapline-1)
                pMLEditData->veditPos = temp->wrapStartPos[wrapline+1]-1;
            else
                pMLEditData->veditPos = temp->wrapStartPos[wrapline+1];
        }else {
            newOff += pMLEditData->dx_chars[temp->wrapStartPos[wrapline]];
            for (i=temp->wrapStartPos[wrapline]; i<temp->wrapStartPos[wrapline+1]+1; i++) {
                newdist = newOff - pMLEditData->dx_chars[i];
                if (newdist > 0 && newdist < dist)
                    dist = newdist;
                else { 
                      pMLEditData->veditPos =i;
                    return FALSE;
                }
            }
        }
    }else {        
        temp = GetLineData (pMLEditData, lineNO);
        edtGetLineInfo (hWnd, temp);

        if( olddispPos > pMLEditData->sz.cx ) 
        {
            pMLEditData->veditPos = pMLEditData->fit_chars;
               pMLEditData->vdispPos = edtGetStartDispPosAtEnd (hWnd, temp);
            return TRUE;
        }else {
            pMLEditData->veditPos = edtGetNewvPos (hWnd, temp, newOff);
            if (pMLEditData->veditPos == -2)
                pMLEditData->veditPos = pMLEditData->fit_chars;
            pMLEditData->vdispPos = edtGetNewvPos (hWnd, temp, olddispPos);
            if (olddispPos != pMLEditData->dx_chars[pMLEditData->vdispPos])
                return TRUE;
        }
    }
    return FALSE;
}

void edtSetCaretPos (HWND hWnd)
{
    int line = 0, wrapline = 0;
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA temp;

    if (GetWindowStyle(hWnd) & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        temp = GetLineData (pMLEditData, line);
        edtGetLineInfo (hWnd, temp);
        SetCaretPos (hWnd, 
                pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[temp->wrapStartPos[wrapline]]
                    + pMLEditData->leftMargin, 
                (pMLEditData->editLine-pMLEditData->StartlineDisp) * pMLEditData->lineHeight
                    + pMLEditData->topMargin);
        
    }else {
        temp = GetLineData (pMLEditData, pMLEditData->editLine);
        edtGetLineInfo (hWnd, temp);
        SetCaretPos (hWnd, 
                pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[pMLEditData->vdispPos]
                    + pMLEditData->leftMargin, 
                (pMLEditData->editLine-pMLEditData->StartlineDisp) * pMLEditData->lineHeight
                    + pMLEditData->topMargin);
    }
}

void edtPosProc (HWND hWnd)
{
    int line, wrapline;
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA pLineData;

    if (pMLEditData->diff) {
        pMLEditData->editLine = pMLEditData->realeditLine;
        pMLEditData->vdispPos = pMLEditData->realdispPos;
        pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
        pMLEditData->StartlineDisp = pMLEditData->realStartLine;
        pMLEditData->EndlineDisp   = pMLEditData->realEndLine;
        pMLEditData->diff = FALSE;
        if (GetWindowStyle(hWnd) & ES_AUTOWRAP)
        {
            edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
            pLineData = GetLineData(pMLEditData, line);
        }else
            pLineData= GetLineData(pMLEditData, pMLEditData->editLine);
        //FIXME
           edtGetLineInfo (hWnd, pLineData);
        ActiveCaret (hWnd);
        pMLEditData->bSCROLL = TRUE;
    }else
        pMLEditData->bSCROLL = FALSE;
}

/* add end*/

static int edtOnCreate (HWND hWnd)
{
    PMLEDITDATA pMLEditData = NULL;
    DWORD dwStyle     = GetWindowStyle(hWnd);

    if (!(pMLEditData = malloc (sizeof (MLEDITDATA)))) {
        return -1;
    }
    SetWindowAdditionalData2 (hWnd,(DWORD)pMLEditData);

    pMLEditData->lineHeight = GetWindowFont (hWnd)->size;
    if (dwStyle & ES_BASELINE)
        pMLEditData->lineHeight += 2;
        
    if (!CreateCaret (hWnd, NULL, 1, pMLEditData->lineHeight))
    {
        free (pMLEditData);
        return -1;
    }

    pMLEditData->status         = 0;
    pMLEditData->curtotalLen    = 0;
    pMLEditData->totalLimit     = 100*LEN_MLEDIT_BUFFER;
    pMLEditData->lineLimit      = LEN_MLEDIT_BUFFER;
    pMLEditData->editLine       = 0;
    pMLEditData->veditPos       = 0;
    pMLEditData->dispPos        = 0;
    pMLEditData->vdispPos       = 0;

    pMLEditData->MaxlinesDisp   = 0;
    pMLEditData->linesDisp      = 0;
    pMLEditData->StartlineDisp  = 0;
    pMLEditData->EndlineDisp    = 0;
    
    MLEditInitBuffer (hWnd, pMLEditData, GetWindowCaption(hWnd), dwStyle);    

#if 0
    pMLEditData->selStartPos    = 0;
    pMLEditData->selEndPos      = 0;
#endif

    pMLEditData->passwdChar     = '*';

    pMLEditData->leftMargin     = MARGIN_MEDIT_LEFT;
    pMLEditData->topMargin      = MARGIN_MEDIT_TOP;
    pMLEditData->rightMargin    = MARGIN_MEDIT_RIGHT;
    pMLEditData->bottomMargin   = MARGIN_MEDIT_BOTTOM;

    pMLEditData->pos_chars = (int*)malloc(LEN_MLEDIT_BUFFER*sizeof(int));
    pMLEditData->dx_chars = (int*)malloc(LEN_MLEDIT_BUFFER*sizeof(int));

    memset (pMLEditData->pos_chars, 0, LEN_MLEDIT_BUFFER*sizeof(int));
    memset (pMLEditData->dx_chars , 0, LEN_MLEDIT_BUFFER*sizeof(int));

    pMLEditData->fit_chars      = 0;
    pMLEditData->realdispPos    = 0;
    pMLEditData->realStartLine  = 0;
    pMLEditData->realEndLine    = 0;
    pMLEditData->diff           = FALSE;
    pMLEditData->bSCROLL        = FALSE;

    pMLEditData->wraplines      = 0;
    
    //SetWindowAdditionalData2 (hWnd,(DWORD)pMLEditData);

    return 0;
}

static void edtOnSizeChanged (HWND hWnd, RECT *clientRect)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);

    pMLEditData->MaxlinesDisp   = (RECTHP(clientRect) - pMLEditData->topMargin
                                    - pMLEditData->bottomMargin)
                                / pMLEditData->lineHeight;

    if (pMLEditData->MaxlinesDisp < 0)
        pMLEditData->MaxlinesDisp = 0;
    if (dwStyle & ES_AUTOWRAP && pMLEditData->head)
    {
        PLINEDATA  pLineData = pMLEditData->head;
        pMLEditData->wraplines = 0;
        for (; pLineData; pLineData = pLineData->next)
        {
            calcLineInfo (hWnd, pMLEditData, pLineData);
            pMLEditData->wraplines += pLineData->nwrapline;
        }
    }
    if (dwStyle & ES_AUTOWRAP)
        pMLEditData->linesDisp    = MIN (pMLEditData->MaxlinesDisp, pMLEditData->wraplines);
    else
        pMLEditData->linesDisp    = MIN (pMLEditData->MaxlinesDisp, pMLEditData->lines);
    pMLEditData->StartlineDisp  = 0;
    pMLEditData->EndlineDisp    = pMLEditData->StartlineDisp + pMLEditData->linesDisp - 1;
    pMLEditData->editLine       = 0;
    pMLEditData->dispPos        = 0;
    
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
}

static void edtOnPaint (HWND hWnd)
{
    int     i;
    char    dispBuffer [LEN_MLEDIT_BUFFER+1];
    RECT    rect;
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    HDC hdc;
    PLINEDATA   pLineData;
    
    hdc = BeginPaint (hWnd);

    GetClientRect (hWnd, &rect);

    if (dwStyle & ES_BASELINE) {
        SetPenColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DEF));
        for (i = 0; i < pMLEditData->MaxlinesDisp; i++)
            DrawHDotLine (hdc, 
                    pMLEditData->leftMargin, 
                    pMLEditData->topMargin + pMLEditData->lineHeight * (i + 1) - 1,
                    RECTW (rect) - pMLEditData->leftMargin - pMLEditData->rightMargin);
    }

    rect.left    += pMLEditData->leftMargin;
    rect.top     += pMLEditData->topMargin ;
    rect.right   -= pMLEditData->rightMargin;
    rect.bottom  -= pMLEditData->bottomMargin;

    ClipRectIntersect (hdc, &rect);

    if (dwStyle & WS_DISABLED)
        SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DIS));
    else
        SetTextColor (hdc, GetWindowElementColorEx (hWnd, FGC_CONTROL_DEF));
    SetBkMode (hdc, BM_TRANSPARENT);

    for (i = pMLEditData->StartlineDisp; 
            i <= pMLEditData->EndlineDisp; i++) {
        int y, line=i, wrapline=0;
        if (dwStyle & ES_AUTOWRAP)
            edtGetLineNums (pMLEditData, i, &line, &wrapline);
    
        pLineData= GetLineData(pMLEditData, line);
        if (pLineData == NULL)
            break;

        edtGetLineInfo (hWnd, pLineData);
        if (dwStyle & ES_AUTOWRAP) {
             if (pMLEditData->fit_chars == 0 && 
                            pMLEditData->EndlineDisp >= pMLEditData->wraplines) {
                //FIXME
                continue;
                }
        }
        else {
             if (pMLEditData->fit_chars == 0 && 
                            pMLEditData->EndlineDisp >= pMLEditData->lines) {
                //FIXME
                continue;
                }
        }

        if (dwStyle & ES_PASSWORD)
            memset (dispBuffer, pMLEditData->passwdChar, pLineData->dataEnd);
            memcpy (dispBuffer, 
                   pLineData->buffer,
                pLineData->dataEnd);
               dispBuffer[pLineData->dataEnd] = '\0';

        y = pMLEditData->lineHeight*
                        (i - pMLEditData->StartlineDisp)
                    + pMLEditData->topMargin;
        //fprint (stderr, "textOut-buffer = %s\n", dispBuffer+pLineData->wrapStartPos[wrapline]);
        if (dwStyle & ES_AUTOWRAP)
            TextOutLen (hdc, 
                pMLEditData->leftMargin - pMLEditData->dispPos, 
                y,
                dispBuffer+pMLEditData->pos_chars[pLineData->wrapStartPos[wrapline]],
                pMLEditData->pos_chars[pLineData->wrapStartPos[wrapline+1]]
                    -pMLEditData->pos_chars[pLineData->wrapStartPos[wrapline]]);
        else
            TextOut(hdc, 
                pMLEditData->leftMargin - pMLEditData->dispPos, y,
                dispBuffer);
    }

    EndPaint (hWnd, hdc);
}

/* ----------------------------------- key process --------------------------------------- */
static int edtKeyEnter (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA   pLineData;
    int     line, wrapline;
    char *  tempP = NULL;
    PLINEDATA temp = NULL;
    RECT    InvRect; 

    GetClientRect (hWnd, &InvRect);
    InvRect.left = pMLEditData->leftMargin - 1;

    if (dwStyle & ES_READONLY) {
        Ping();
        return 0;
    }

    edtPosProc (hWnd); 

    if ((pMLEditData->totalLimit >= 0) 
            && ((pMLEditData->curtotalLen + 1) > pMLEditData->totalLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        return 0;
    }else
        pMLEditData->curtotalLen++;

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);
    

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineInfo (hWnd, pLineData);

        if (pMLEditData->veditPos < pMLEditData->fit_chars)
            tempP = pLineData->buffer + 
                    pMLEditData->pos_chars[pMLEditData->veditPos];
        temp = pLineData->next;
        pLineData->next = malloc( sizeof(LINEDATA) );
        pLineData->next->previous = pLineData;
        pLineData->next->next = temp;
    
        if(temp)
            temp->previous = pLineData->next;
    
        temp = pLineData->next;
        temp->lineNO = line + 1;
        if(tempP) {
            memcpy(temp->buffer,tempP,strlen(tempP));
            temp->dataEnd = strlen(tempP);
        }
        else
            temp->dataEnd = 0;
        temp->buffer[temp->dataEnd] = '\0'; 
        pLineData->dataEnd = pMLEditData->pos_chars[pMLEditData->veditPos];
        pLineData->buffer[pLineData->dataEnd]='\0';
        temp = temp->next;
        while (temp)
        {
            temp->lineNO++;
            temp = temp->next;
        }
        if (!((pMLEditData->veditPos == pLineData->wrapStartPos[wrapline])
               && (wrapline)))
            pMLEditData->editLine++;
        pMLEditData->wraplines -= pLineData->nwrapline;
        calcLineInfo (hWnd, pMLEditData, pLineData);
        calcLineInfo (hWnd, pMLEditData, pLineData->next);
        edtGetLineInfo (hWnd, pLineData->next);
        pMLEditData->lines++;
        pMLEditData->wraplines += pLineData->nwrapline + pLineData->next->nwrapline;
        pMLEditData->veditPos=0;

        if (pMLEditData->editLine > pMLEditData->EndlineDisp)
        {
            pMLEditData->EndlineDisp = pMLEditData->editLine;
            if (pMLEditData->EndlineDisp > 
                            pMLEditData->StartlineDisp+pMLEditData->MaxlinesDisp-1)
            {
                pMLEditData->StartlineDisp = pMLEditData->EndlineDisp
                                            -pMLEditData->MaxlinesDisp + 1;
            }else {
                InvRect.top  = (pMLEditData->editLine 
                                - pMLEditData->StartlineDisp -1) 
                                    * pMLEditData->lineHeight
                                  + pMLEditData->topMargin;
                InvRect.bottom  -=  pMLEditData->lineHeight;
            }
        }else {
            pMLEditData->EndlineDisp = pMLEditData->StartlineDisp + MIN (pMLEditData->wraplines 
                                                                        - pMLEditData->StartlineDisp, 
                                                                    pMLEditData->MaxlinesDisp) - 1;
        }
        pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                - pMLEditData->StartlineDisp + 1;
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
        InvalidateRect (hWnd, &InvRect, TRUE);
        edtSetCaretPos (hWnd);
        edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }

    edtGetLineInfo (hWnd, pLineData);
    
    if (pMLEditData->veditPos < pMLEditData->fit_chars)
        tempP = pLineData->buffer + pMLEditData->pos_chars[pMLEditData->veditPos];
    temp = pLineData->next;
    pLineData->next = malloc( sizeof(LINEDATA) );
    pLineData->next->previous = pLineData;
    pLineData->next->next = temp;
    
    if(temp)
        temp->previous = pLineData->next;
    
    temp = pLineData->next;
    temp->lineNO  = pMLEditData->editLine + 1;
    if(tempP) {
        memcpy(temp->buffer,tempP,strlen(tempP));
        temp->dataEnd = strlen(tempP);
    }
    else
        temp->dataEnd = 0;
    temp->buffer[temp->dataEnd] = '\0'; 
    pLineData->dataEnd = pMLEditData->pos_chars[pMLEditData->veditPos];
    pLineData->buffer[pLineData->dataEnd]='\0';
    temp = temp->next;
    while (temp)
    {
        temp->lineNO++;
        temp = temp->next;
    }
    // added by leon to optimize display
    if ((pMLEditData->editLine - pMLEditData->StartlineDisp + 1) 
                    < pMLEditData->MaxlinesDisp) {
        if (!(pMLEditData->dx_chars[pMLEditData->veditPos] - pMLEditData->leftMargin
                 > edtGetOutWidth (hWnd))) {
            InvRect.top  = (pMLEditData->editLine - pMLEditData->StartlineDisp -1) 
                                * pMLEditData->lineHeight
                                  + pMLEditData->topMargin;
        }
                
     }else {
        if ((pMLEditData->vdispPos == 0) 
                && (pMLEditData->vdispPos == pMLEditData->veditPos))         
            InvRect.bottom  -=  pMLEditData->lineHeight;
     }
    // added by leon to optimize display
    pMLEditData->veditPos = 0;
    pMLEditData->vdispPos = 0;

    if(pMLEditData->linesDisp < pMLEditData->MaxlinesDisp)
    {
        pMLEditData->EndlineDisp++;
        pMLEditData->linesDisp++;
    }
    else if(pMLEditData->editLine == pMLEditData->EndlineDisp) 
    {
        pMLEditData->StartlineDisp++;
        pMLEditData->EndlineDisp++;
    }
    pMLEditData->editLine++;
    edtGetLineInfo (hWnd, GetLineData(pMLEditData,pMLEditData->editLine));    
    pMLEditData->lines++;
    
    pMLEditData->dispPos = 0;

    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetCaretPos (hWnd);
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    InvalidateRect (hWnd, &InvRect, TRUE); // modified by leon

    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyCursorblockLeft (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    BOOL         bScroll = FALSE;
    int          newStartPos, line, wrapline;
    PLINEDATA    temp;
    PLINEDATA    pLineData;

    edtPosProc (hWnd); 
    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }
    else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);

    if (dwStyle & ES_AUTOWRAP)
    {
        if (!pMLEditData->veditPos && !pMLEditData->editLine) return 0;
        edtGetLineInfo (hWnd, pLineData);
        if (!pMLEditData->veditPos)
        {
            if (pMLEditData->editLine == pMLEditData->StartlineDisp) {
                bScroll = TRUE;
                pMLEditData->StartlineDisp--;
                if (pMLEditData->EndlineDisp >= pMLEditData->MaxlinesDisp)
                    pMLEditData->EndlineDisp--;
            }
            pMLEditData->editLine--;
            temp = pLineData->previous;
            edtGetLineInfo (hWnd, temp);
            pMLEditData->veditPos = temp->wrapStartPos[temp->nwrapline];
        }else if (pMLEditData->veditPos == pLineData->wrapStartPos[wrapline]) {
            if (pMLEditData->editLine == pMLEditData->StartlineDisp) {
                bScroll = TRUE;
                pMLEditData->StartlineDisp--;
                if (pMLEditData->EndlineDisp >= pMLEditData->MaxlinesDisp)
                    pMLEditData->EndlineDisp--;
            }
            pMLEditData->editLine--;
            pMLEditData->veditPos--; 
        }else 
            pMLEditData->veditPos--;

        pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                - pMLEditData->StartlineDisp + 1;
        edtSetCaretPos (hWnd);
        if (bScroll)
        {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }

    
    if (!pMLEditData->veditPos)
    {
        temp = pLineData->previous;
        if (!temp) return 0;
        if(pMLEditData->editLine > 0 )
        {
            if (pMLEditData->editLine == pMLEditData->StartlineDisp) {
                bScroll = TRUE;
                pMLEditData->StartlineDisp--;
                if (pMLEditData->EndlineDisp >= pMLEditData->MaxlinesDisp)
                    pMLEditData->EndlineDisp--;
            }
            pMLEditData->editLine --;
            edtGetLineInfo (hWnd, GetLineData (pMLEditData, pMLEditData->editLine));
            pMLEditData->veditPos = pMLEditData->fit_chars; 
            newStartPos = edtGetStartDispPosAtEnd (hWnd, temp);
            if (pMLEditData->vdispPos != newStartPos)
            {
                pMLEditData->vdispPos = newStartPos;
                bScroll = TRUE;
            }
        }
    }
    else
    {    
        if (pMLEditData->veditPos == pMLEditData->vdispPos){
            pMLEditData->veditPos--;
            if (edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4) == -1)
                pMLEditData->vdispPos = 0;
            else {
                pMLEditData->vdispPos = edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4);
            }
            bScroll = TRUE;
        }else {
            pMLEditData->veditPos--;
        }
            
    }    
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetCaretPos (hWnd);
    //FIXME
    if (bScroll) {
        edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        InvalidateRect (hWnd, NULL, TRUE);
    }
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyCursorblockRight (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    BOOL         bScroll = FALSE;
    int          line, wrapline;
    PLINEDATA    temp;
    PLINEDATA    pLineData;

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }
    else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);

    if (dwStyle & ES_AUTOWRAP)
    {
        if (!(temp = pLineData->next) 
                    && pMLEditData->veditPos == pMLEditData->fit_chars) return 0;
        edtGetLineInfo (hWnd, pLineData);
        if (pMLEditData->veditPos == pMLEditData->fit_chars)
        {
            if (pMLEditData->editLine == pMLEditData->EndlineDisp) {
                bScroll = TRUE;
                pMLEditData->StartlineDisp++;
                pMLEditData->EndlineDisp++;
            }
            edtGetLineInfo (hWnd, temp);
            pMLEditData->editLine++;
            pMLEditData->veditPos = 0;
        }else if (pMLEditData->veditPos == pLineData->wrapStartPos[wrapline+1]-1)
        {
            if (pMLEditData->veditPos < 
                            pLineData->wrapStartPos[pLineData->nwrapline]-1)
            {
                if (pMLEditData->editLine == pMLEditData->EndlineDisp) {
                    bScroll = TRUE;
                    pMLEditData->StartlineDisp++;
                    pMLEditData->EndlineDisp++;
                }
                pMLEditData->editLine++;
            }
            pMLEditData->veditPos++;
        }
        else
            pMLEditData->veditPos++;

        pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                - pMLEditData->StartlineDisp + 1;

        edtSetCaretPos (hWnd);
        if (bScroll)
        {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }
            
    if (pMLEditData->veditPos == pMLEditData->fit_chars) 
    {
        temp = pLineData->next;
        if (temp && pMLEditData->editLine == pMLEditData->EndlineDisp) {
            bScroll = TRUE;
            pMLEditData->StartlineDisp++;
            pMLEditData->EndlineDisp++;
        }
        if(temp)
        {
            pMLEditData->editLine++;
            edtGetLineInfo (hWnd, GetLineData (pMLEditData, pMLEditData->editLine));
            pMLEditData->veditPos  = 0;
            if(pMLEditData->vdispPos !=0)
            {
                pMLEditData->vdispPos  = 0;
                bScroll = TRUE;
            }
        } else
            return 0;
    } else {    
        // caret is at the end of the display rect
        if ((pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[pMLEditData->vdispPos] 
                    < edtGetOutWidth (hWnd)) 
                && (pMLEditData->dx_chars[pMLEditData->veditPos + 1] 
                    - pMLEditData->dx_chars[pMLEditData->vdispPos] 
                    > edtGetOutWidth (hWnd))) {
            bScroll = TRUE;
            pMLEditData->veditPos++;
            if (edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4) == -1)
                pMLEditData->vdispPos = 0;
            else
                pMLEditData->vdispPos = edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4);

            if (pMLEditData->dx_chars[pMLEditData->vdispPos] 
                        + edtGetOutWidth (hWnd) 
                    > pMLEditData->sz.cx) 
                pMLEditData->vdispPos = 
                            edtGetStartDispPosAtEnd (hWnd, pLineData);
        }else 
            pMLEditData->veditPos++;
    }
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetCaretPos (hWnd);
    //FIXME
    if (bScroll) {
        edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        InvalidateRect (hWnd, NULL, TRUE);
    }
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyCursorblockUp (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    BOOL bScroll = FALSE;
    int  newStartPos, olddispPos, oldeditPos, line, wrapline;
    PLINEDATA temp;
    PLINEDATA    pLineData;

    edtPosProc (hWnd); 
    olddispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos];
    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }
    else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);

    temp = pLineData->previous; 
    if(pMLEditData->editLine == 0) return 0;
    if (pMLEditData->editLine == pMLEditData->StartlineDisp)
    {
        bScroll = TRUE;
        pMLEditData->StartlineDisp--;
        if (dwStyle & ES_AUTOWRAP)
            pMLEditData->EndlineDisp = MIN (pMLEditData->wraplines - 1,
                                pMLEditData->StartlineDisp + 
                                pMLEditData->MaxlinesDisp - 1);
        else
            pMLEditData->EndlineDisp--;    
    }
    pMLEditData->editLine--;

    if (dwStyle & ES_AUTOWRAP)
    {
        oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos] 
                        - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]];
        edtGetCaretValid (hWnd, pMLEditData->editLine,
                                           olddispPos, oldeditPos);

        pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                - pMLEditData->StartlineDisp + 1;
        edtSetCaretPos (hWnd);
        //FIXME
        if (bScroll) {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }    
    
    edtGetLineInfo (hWnd, temp);

    if( olddispPos > pMLEditData->sz.cx ) 
    {
        pMLEditData->veditPos = pMLEditData->fit_chars;
           newStartPos = edtGetStartDispPosAtEnd (hWnd, temp);
           pMLEditData->vdispPos =  newStartPos;
        bScroll = TRUE;
    }                
    else 
    {
        pMLEditData->veditPos = edtGetNewvPos (hWnd, temp, oldeditPos);
        if (pMLEditData->veditPos == -2)
            pMLEditData->veditPos = pMLEditData->fit_chars;
        pMLEditData->vdispPos = edtGetNewvPos (hWnd, temp, olddispPos);
        if (olddispPos != pMLEditData->dx_chars[pMLEditData->vdispPos])
            bScroll = TRUE;
    }
    
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    SetCaretPos (hWnd, 
        pMLEditData->dx_chars[pMLEditData->veditPos] 
            - pMLEditData->dx_chars[pMLEditData->vdispPos] 
            + pMLEditData->leftMargin, 
        (pMLEditData->editLine - pMLEditData->StartlineDisp) * pMLEditData->lineHeight
            + pMLEditData->topMargin);
    //FIXME
    if (bScroll) {
        edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        InvalidateRect(hWnd,NULL,TRUE);    
    }
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyCursorblockDown (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    BOOL bScroll = FALSE;
    int  newStartPos, olddispPos, oldeditPos, line, wrapline;
    PLINEDATA temp;
    PLINEDATA    pLineData;

    edtPosProc (hWnd); 
    oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos];
    olddispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }
    else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);

    temp = pLineData->next; 
    if (dwStyle & ES_AUTOWRAP) {
        if(pMLEditData->editLine == pMLEditData->wraplines-1) return 0;
    }else {
        if(pMLEditData->editLine == pMLEditData->lines-1) return 0;
    }
    if (pMLEditData->editLine == pMLEditData->EndlineDisp)
    {
        bScroll = TRUE;
        pMLEditData->StartlineDisp++;
        pMLEditData->EndlineDisp++;    
    }
    pMLEditData->editLine++;

    if (dwStyle & ES_AUTOWRAP)
    {
        oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos] 
                        - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]];
        edtGetCaretValid (hWnd, pMLEditData->editLine,
                                           olddispPos, oldeditPos);

        edtSetCaretPos (hWnd);
        //FIXME
        if (bScroll) {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }    

    edtGetLineInfo (hWnd, temp);

    if( olddispPos > pMLEditData->sz.cx ) 
    {
        pMLEditData->veditPos = pMLEditData->fit_chars;
        newStartPos = edtGetStartDispPosAtEnd (hWnd, temp);
        pMLEditData->vdispPos =  newStartPos;
        bScroll = TRUE;
    }                
    else 
    {
        pMLEditData->veditPos = edtGetNewvPos (hWnd, temp, oldeditPos);
        if (pMLEditData->veditPos == -2)
            pMLEditData->veditPos = pMLEditData->fit_chars;
        pMLEditData->vdispPos = edtGetNewvPos (hWnd, temp, olddispPos);
        if (olddispPos != pMLEditData->dx_chars[pMLEditData->vdispPos])
            bScroll = TRUE;
    }
    
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetCaretPos (hWnd);
    //FIXME
    if (bScroll) {
        edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        InvalidateRect(hWnd,NULL,TRUE);    
    }
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyRemove (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA    pLineData;
    PLINEDATA temp;
    int leftLen;
    int line, wrapline;
    int oldnwrapline;
    BOOL bScroll=FALSE;
    RECT    InvRect; 
    int     deleted, i;

    GetClientRect (hWnd, &InvRect);
    InvRect.left = pMLEditData->leftMargin - 1;

    if (dwStyle & ES_READONLY) {
        Ping();
        return 0;
    }

    edtPosProc (hWnd); 
    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    } else
        pLineData = GetLineData(pMLEditData, pMLEditData->editLine);
    
    edtGetLineInfo (hWnd,pLineData); 

    if (dwStyle & ES_AUTOWRAP)
    {
        oldnwrapline = pLineData->nwrapline;
        InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                        * pMLEditData->lineHeight
                        + pMLEditData->topMargin;
        if (pMLEditData->veditPos == pMLEditData->fit_chars) // end of the paragraph
        {
            bScroll = TRUE;
            if (pMLEditData->editLine == pMLEditData->wraplines-1) return 0;

               temp = pLineData->next; 

            pMLEditData->wraplines -= pLineData->nwrapline;
            pMLEditData->curtotalLen --;
            if (temp)
                pMLEditData->wraplines -= pLineData->next->nwrapline;

            if(pLineData->dataEnd + temp->dataEnd <= LEN_MLEDIT_BUFFER)
            {
                memcpy(pLineData->buffer+pLineData->dataEnd,temp->buffer,temp->dataEnd);    
                pLineData->dataEnd += temp->dataEnd;
                pLineData->buffer[pLineData->dataEnd] = '\0';
                if(temp->next)
                {
                    pLineData->next = temp->next;
                    temp->next->previous = pLineData;    
                }
                else
                    pLineData->next = NULL;
                free(temp);
                temp = pLineData->next;
                while (temp)
                {
                    temp->lineNO--;
                    temp = temp->next;
                }
                edtGetLineInfo (hWnd, pLineData);
    
            }
            else if (temp->dataEnd > 0)
            {
                leftLen = LEN_MLEDIT_BUFFER - pLineData->dataEnd;
                memcpy(pLineData->buffer+pLineData->dataEnd,temp->buffer,leftLen);
                pLineData->dataEnd +=leftLen;
                pLineData->buffer[pLineData->dataEnd] = '\0';
                memcpy(temp->buffer,temp->buffer+leftLen,temp->dataEnd-leftLen);  
                temp->dataEnd -= leftLen;
                temp->buffer[temp->dataEnd] = '\0';
            }
            calcLineInfo (hWnd, pMLEditData, pLineData);
            edtGetLineInfo (hWnd, pLineData);
            pMLEditData->lines--;
            pMLEditData->wraplines += pLineData->nwrapline;
            
        }else {
            deleted = pMLEditData->pos_chars[pMLEditData->veditPos + 1]
                        - pMLEditData->pos_chars[pMLEditData->veditPos];
            pMLEditData->curtotalLen -= deleted;

            for (i = pMLEditData->pos_chars[pMLEditData->veditPos]; 
                    i < pLineData->dataEnd - deleted;
                    i++)
                pLineData->buffer [i] 
                    = pLineData->buffer [i + deleted];

            pLineData->dataEnd -= deleted;
            if (pLineData->dataEnd == 0) {
                pMLEditData->fit_chars = 0;
                pMLEditData->pos_chars[0] = 0;
                pMLEditData->dx_chars[0] = 0;
            }
            pLineData->buffer[pLineData->dataEnd] = '\0';

            pMLEditData->wraplines -= pLineData->nwrapline;
            calcLineInfo (hWnd, pMLEditData, pLineData);
            pMLEditData->wraplines += pLineData->nwrapline;
            edtGetLineInfo (hWnd, pLineData);

            if (wrapline == oldnwrapline - 1)
                InvRect.left = pMLEditData->dx_chars[pMLEditData->veditPos]
                    - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]]
                    + pMLEditData->leftMargin - 1; 
            if (oldnwrapline == pLineData->nwrapline)
                InvRect.bottom = InvRect.top + pMLEditData->lineHeight
                                                *(pLineData->nwrapline-wrapline);
        }

        pMLEditData->EndlineDisp = MIN (pMLEditData->StartlineDisp
                                        + pMLEditData->MaxlinesDisp - 1,
                                       pMLEditData->wraplines - 1);
        pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                    - pMLEditData->StartlineDisp + 1;

        if (wrapline == pLineData->nwrapline /*&& pLineData->nwrapline != 1*/
                        && pMLEditData->veditPos == pMLEditData->fit_chars)
        {
            pMLEditData->editLine--;
            if (pMLEditData->editLine < pMLEditData->StartlineDisp)
            {
                bScroll = TRUE;
                pMLEditData->StartlineDisp--;
                pMLEditData->EndlineDisp = MIN (pMLEditData->StartlineDisp +
                                                    pMLEditData->MaxlinesDisp -1, 
                                                pMLEditData->EndlineDisp);
                pMLEditData->linesDisp = pMLEditData->EndlineDisp 
                                            - pMLEditData->StartlineDisp + 1;
            }
        }
        if (oldnwrapline != pLineData->nwrapline)
            bScroll = TRUE;
        edtSetCaretPos (hWnd);
        //FIXME
        if (bScroll) {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect (hWnd, &InvRect,TRUE); 
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
        return 0;
    }

       temp = pLineData->next; 
    if (pMLEditData->veditPos == pMLEditData->fit_chars && temp)
    {
        if(pLineData->dataEnd + temp->dataEnd <= LEN_MLEDIT_BUFFER)
        {
            pMLEditData->curtotalLen --;
            memcpy(pLineData->buffer+pLineData->dataEnd,temp->buffer,temp->dataEnd);    
            pLineData->dataEnd += temp->dataEnd;
            pLineData->buffer[pLineData->dataEnd] = '\0';
            if(temp->next)
            {
                pLineData->next = temp->next;
                temp->next->previous = pLineData;    
            }
            else
                pLineData->next = NULL;
            free(temp);
            temp = pLineData->next;
            while (temp)
            {
                temp->lineNO--;
                temp = temp->next;
            }
            edtGetLineInfo (hWnd, pLineData);
    
            if(pMLEditData->EndlineDisp >= pMLEditData->lines-1)        
            {
            // added by leon to optimize display
            if ((pMLEditData->EndlineDisp == pMLEditData->lines -1) 
                        && (pMLEditData->StartlineDisp != 0)) {
                InvRect.bottom = (pMLEditData->editLine - pMLEditData->StartlineDisp + 1) 
                                    * pMLEditData->lineHeight + pMLEditData->topMargin;
            }else {
                InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                        * pMLEditData->lineHeight + pMLEditData->topMargin;
            }
            // added by leon to optimize display
                if(pMLEditData->StartlineDisp !=0)
                {
                    pMLEditData->StartlineDisp--;
                    pMLEditData->EndlineDisp = MIN ( pMLEditData->lines-1,
                        pMLEditData->StartlineDisp+pMLEditData->MaxlinesDisp -1);
                } else
                    pMLEditData->EndlineDisp--;
                
                pMLEditData->linesDisp = pMLEditData->EndlineDisp -
                                pMLEditData->StartlineDisp+1;
            }
            else if (pMLEditData->lines <= pMLEditData->MaxlinesDisp)
            {
                pMLEditData->EndlineDisp--;
                pMLEditData->linesDisp = pMLEditData->EndlineDisp -
                                pMLEditData->StartlineDisp+1;
            }
            pMLEditData->lines--;
        } else if (temp->dataEnd > 0)
        {
            leftLen = LEN_MLEDIT_BUFFER - pLineData->dataEnd;
            memcpy(pLineData->buffer+pLineData->dataEnd,temp->buffer,leftLen);
            pLineData->dataEnd +=leftLen;
            pLineData->buffer[pLineData->dataEnd] = '\0';
            memcpy(temp->buffer,temp->buffer+leftLen,temp->dataEnd-leftLen);  
            temp->dataEnd -=leftLen;
            temp->buffer[temp->dataEnd] = '\0';
            // added by leon to optimize display
            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                    * pMLEditData->lineHeight + pMLEditData->topMargin;
            // added by leon to optimize display
        }
    }
    else if (pMLEditData->veditPos != pMLEditData->fit_chars)
    {    
        edtGetLineInfo (hWnd, GetLineData (pMLEditData, pMLEditData->editLine));

        deleted = pMLEditData->pos_chars[pMLEditData->veditPos + 1]
                    - pMLEditData->pos_chars[pMLEditData->veditPos];
        pMLEditData->curtotalLen -= deleted;

        for (i = pMLEditData->pos_chars[pMLEditData->veditPos]; 
                i < pLineData->dataEnd - deleted;
                i++)
            pLineData->buffer [i] 
                = pLineData->buffer [i + deleted];

        pLineData->dataEnd -= deleted;
        if (pLineData->dataEnd == 0) {
            pMLEditData->fit_chars = 0;
            pMLEditData->pos_chars[0] = 0;
            pMLEditData->dx_chars[0] = 0;
        }
        pLineData->buffer[pLineData->dataEnd] = '\0';
        // only current line to redraw
        if (pMLEditData->EndlineDisp <= pMLEditData->lines - 1) {
            InvRect.left = pMLEditData->dx_chars[pMLEditData->veditPos] 
                            - pMLEditData->dx_chars[pMLEditData->vdispPos]
                                + pMLEditData->leftMargin - 1; 
            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                            * pMLEditData->lineHeight
                            + pMLEditData->topMargin;
            InvRect.bottom = InvRect.top + pMLEditData->lineHeight * 2;
        }
    }
    else {
        InvRect.left = InvRect.top = InvRect.right = InvRect.bottom = 0;
        Ping ();
    }
    
    edtGetLineInfo (hWnd, GetLineData (pMLEditData, pMLEditData->editLine));
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];

    edtSetCaretPos (hWnd);
    InvalidateRect (hWnd, &InvRect,TRUE); 
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    return 0;
}

static int edtKeyBackspace (HWND hWnd)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA    pLineData;
    PLINEDATA temp;
    int leftLen,tempEnd;
    int oldnwrapline;
    BOOL bScroll = FALSE;
    RECT    InvRect; 
    int     deleted, i;
    int     line, wrapline;

    GetClientRect (hWnd, &InvRect);
    //InvRect.left = pMLEditData->leftMargin - 1;

    if (dwStyle & ES_READONLY) {
        Ping ();
        return 0;
    }

    edtPosProc (hWnd); 
    
    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    } else
        pLineData = GetLineData(pMLEditData, pMLEditData->editLine);
    
    edtGetLineInfo (hWnd,pLineData); 

    temp = pLineData->previous;    

    if (!temp && !(pMLEditData->veditPos)) return 0;
    
    if (dwStyle & ES_AUTOWRAP)
    {
        oldnwrapline = pLineData->nwrapline;
/*-----------------------------*/                            
        if (pMLEditData->veditPos == 0 && temp)
        {
            pMLEditData->wraplines -= pLineData->nwrapline + temp->nwrapline;
            pMLEditData->curtotalLen --;
            
            tempEnd = temp->dataEnd;
            edtGetLineInfo (hWnd, temp);
                
            if(pLineData->dataEnd + temp->dataEnd <= LEN_MLEDIT_BUFFER)    
            {
                pMLEditData->veditPos = pMLEditData->fit_chars;
                memcpy(temp->buffer+temp->dataEnd,pLineData->buffer,pLineData->dataEnd);    
                temp->dataEnd +=pLineData->dataEnd;
                temp->buffer[temp->dataEnd] = '\0';
                if(pLineData->next)
                {
                    temp->next = pLineData->next;
                    pLineData->next->previous = temp;
                } else
                    temp->next = NULL;
                free(pLineData);
                pLineData = temp;
                temp = temp->next;
                while(temp)
                {
                    temp->lineNO--;
                    temp = temp->next;
                }
                pMLEditData->lines--;
                if (pMLEditData->editLine == pMLEditData->StartlineDisp)
                {
                    pMLEditData->StartlineDisp--;
                    bScroll = TRUE;
                } /*else if ( pMLEditData->EndlineDisp == pMLEditData->wraplines-1) {
                    if( pMLEditData->StartlineDisp != 0)
                        pMLEditData->StartlineDisp--;    
                }*/
                   calcLineInfo (hWnd, pMLEditData, pLineData);
                pMLEditData->wraplines += pLineData->nwrapline;
                   pMLEditData->EndlineDisp = MIN (pMLEditData->wraplines - 1,
                                               pMLEditData->StartlineDisp 
                                            + pMLEditData->MaxlinesDisp - 1 );
                pMLEditData->linesDisp = pMLEditData->EndlineDisp
                                - pMLEditData->StartlineDisp + 1;    
                pMLEditData->editLine--;
            }
            else if (pLineData->dataEnd > 0)
            {
                pMLEditData->wraplines -= pLineData->nwrapline + temp->nwrapline;
                pMLEditData->veditPos = pMLEditData->fit_chars;
                leftLen = LEN_MLEDIT_BUFFER - temp->dataEnd;
                memcpy(temp->buffer+temp->dataEnd,pLineData->buffer,leftLen);
                temp->dataEnd +=leftLen;
                temp->buffer[temp->dataEnd] = '\0';
                memcpy(pLineData->buffer,pLineData->buffer+leftLen,pLineData->dataEnd-leftLen);  
                pLineData->dataEnd -=leftLen;
                pLineData->buffer[pLineData->dataEnd] = '\0';
                pMLEditData->editLine--;
                   calcLineInfo (hWnd, pMLEditData, pLineData);
                   calcLineInfo (hWnd, pMLEditData, temp);
                pMLEditData->wraplines += pLineData->nwrapline + temp->nwrapline;
            }
            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp)
                               * pMLEditData->lineHeight
                            + pMLEditData->topMargin;
        } else if (pMLEditData->veditPos != 0 ) {// not the head 
            pMLEditData->wraplines -= pLineData->nwrapline;
            deleted = pMLEditData->pos_chars[pMLEditData->veditPos] 
                        - pMLEditData->pos_chars[pMLEditData->veditPos - 1];
            pMLEditData->curtotalLen -= deleted;
            for (i = pMLEditData->pos_chars[pMLEditData->veditPos]; 
                    i < pLineData->dataEnd;
                    i++)
                pLineData->buffer[i - deleted] = pLineData->buffer[i];

              pLineData->dataEnd -= deleted;
            pMLEditData->veditPos -= 1;
            pLineData->buffer[pLineData->dataEnd] = '\0';
               calcLineInfo (hWnd, pMLEditData, pLineData);

            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp)
                               * pMLEditData->lineHeight
                            + pMLEditData->topMargin;
            if (oldnwrapline == pLineData->nwrapline)
            {
                if (wrapline == oldnwrapline - 1)
                    InvRect.left = pMLEditData->dx_chars[pMLEditData->veditPos]
                           - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]]
                             + pMLEditData->leftMargin - 1; 
                InvRect.bottom = InvRect.top + pMLEditData->lineHeight
                                                *(pLineData->nwrapline-wrapline);
            }
            if ((pMLEditData->veditPos == pLineData->wrapStartPos[wrapline]
                    || pMLEditData->veditPos == pLineData->wrapStartPos[wrapline]-1)
                    && pMLEditData->editLine == pMLEditData->StartlineDisp
                    && oldnwrapline > 1 && wrapline != 0)
            {
                pMLEditData->StartlineDisp--;
                pMLEditData->editLine = pMLEditData->StartlineDisp;
                bScroll = TRUE;
            }else if ((pMLEditData->veditPos == pLineData->wrapStartPos[wrapline]
                || pMLEditData->veditPos == pLineData->wrapStartPos[wrapline]-1) 
                    && oldnwrapline > 1 && wrapline != 0)
            {
                //FIXME
                InvRect.top -= pMLEditData->lineHeight;
                pMLEditData->editLine--;
            }
               calcLineInfo (hWnd, pMLEditData, pLineData);
            pMLEditData->wraplines += pLineData->nwrapline;
            pMLEditData->EndlineDisp = MIN (pMLEditData->wraplines - 1,
                                           pMLEditData->StartlineDisp 
                                        + pMLEditData->MaxlinesDisp - 1 );
            pMLEditData->linesDisp = pMLEditData->EndlineDisp
                                    - pMLEditData->StartlineDisp + 1;    
        }
        else {
            InvRect.left = InvRect.top = InvRect.right = InvRect.bottom = 0;
            Ping ();
        }
        edtSetCaretPos (hWnd);
        if (bScroll)
        {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect (hWnd, NULL, TRUE);
        }
        else
            InvalidateRect (hWnd, &InvRect, TRUE);
        if (pMLEditData->bSCROLL)
        {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect (hWnd, NULL, TRUE);
        }
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
/*-----------------------------*/                            
        return 0;
    }
            
    //the caret is at the head of current line
    if (pMLEditData->veditPos == 0 && temp)
    {
        tempEnd = temp->dataEnd;
        edtGetLineInfo (hWnd, temp);
        pMLEditData->curtotalLen --;
        if ( pMLEditData->sz.cx < edtGetOutWidth (hWnd)) { 
            if (( pMLEditData->lines - 1 == pMLEditData->EndlineDisp) 
                    && (pMLEditData->StartlineDisp != 0)) {
                InvRect.bottom =(pMLEditData->editLine - 
                                pMLEditData->StartlineDisp + 1)
                               * pMLEditData->lineHeight 
                               + pMLEditData->topMargin;
            }else {
                //partly redraw
                InvRect.top = (pMLEditData->editLine - 
                                pMLEditData->StartlineDisp - 1 )
                               * pMLEditData->lineHeight 
                               + pMLEditData->topMargin;
            }
        }
                
        if(pLineData->dataEnd + temp->dataEnd <= LEN_MLEDIT_BUFFER)    
        {
            edtGetLineInfo (hWnd, temp);
            pMLEditData->veditPos = pMLEditData->fit_chars;
            memcpy(temp->buffer+temp->dataEnd,pLineData->buffer,pLineData->dataEnd);    
            temp->dataEnd +=pLineData->dataEnd;
            temp->buffer[temp->dataEnd] = '\0';
            if(pLineData->next)
            {
                temp->next = pLineData->next;
                pLineData->next->previous = temp;
            }
            else
                temp->next = NULL;
            free(pLineData);
            pLineData = temp;
            temp = temp->next;
            while(temp)
            {
                temp->lineNO--;
                temp = temp->next;
            }
            pMLEditData->lines--;
            if (pMLEditData->StartlineDisp == pMLEditData->editLine
                    && pMLEditData->StartlineDisp != 0)
            {
                pMLEditData->StartlineDisp--;
                if(pMLEditData->EndlineDisp == pMLEditData->lines)
                {
                    pMLEditData->EndlineDisp = MIN (pMLEditData->lines - 1,
                       pMLEditData->StartlineDisp + pMLEditData->MaxlinesDisp - 1 );
                }
                else 
                {
                    pMLEditData->EndlineDisp--;
                }
                    pMLEditData->linesDisp = pMLEditData->EndlineDisp
                                    - pMLEditData->StartlineDisp + 1;    
            }
            else if ( pMLEditData->EndlineDisp == pMLEditData->lines
                    && pMLEditData->editLine != pMLEditData->StartlineDisp )
            {
                if( pMLEditData->StartlineDisp != 0)
                    pMLEditData->StartlineDisp--;    

                pMLEditData->EndlineDisp--;
                pMLEditData->linesDisp = pMLEditData->EndlineDisp -
                                pMLEditData->StartlineDisp + 1;
            }
            pMLEditData->editLine--;
        }
        else if (pLineData->dataEnd > 0)
        {
            edtGetLineInfo (hWnd, temp);
            pMLEditData->veditPos = pMLEditData->fit_chars;
            leftLen = LEN_MLEDIT_BUFFER - temp->dataEnd;
            memcpy(temp->buffer+temp->dataEnd,pLineData->buffer,leftLen);
            temp->dataEnd +=leftLen;
            temp->buffer[temp->dataEnd] = '\0';
            memcpy(pLineData->buffer,pLineData->buffer+leftLen,pLineData->dataEnd-leftLen);  
            pLineData->dataEnd -=leftLen;
            pLineData->buffer[pLineData->dataEnd] = '\0';
        }
        /* added by leon to fix the 'roll window' bug */
        if (pMLEditData->veditPos > edtGetStartDispPosAtEnd(hWnd,pLineData))
            pMLEditData->vdispPos = edtGetStartDispPosAtEnd(hWnd,pLineData);
        else {
            if (edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4) == -1)
                pMLEditData->vdispPos = 0;
            else
                pMLEditData->vdispPos = edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4);
        }
        /*add end*/
        
           if (pMLEditData->vdispPos == pMLEditData->veditPos 
                && pMLEditData->veditPos != 0) {
            if (edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4) == -1)
                pMLEditData->vdispPos = 0;
            else
                pMLEditData->vdispPos = edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4);
            }
    }
    // not the head
    else if (pMLEditData->veditPos != 0 )
    {
        deleted = pMLEditData->pos_chars[pMLEditData->veditPos] 
                    - pMLEditData->pos_chars[pMLEditData->veditPos - 1];
        pMLEditData->curtotalLen -= deleted;
        for (i = pMLEditData->pos_chars[pMLEditData->veditPos]; 
                i < pLineData->dataEnd;
                i++)
            pLineData->buffer [i - deleted] 
                = pLineData->buffer [i];

          pLineData->dataEnd -= deleted;
        pMLEditData->veditPos -= 1;
        pLineData->buffer[pLineData->dataEnd] = '\0';
        if (pMLEditData->vdispPos == pMLEditData->veditPos) {
            if (edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4) == -1)
                pMLEditData->vdispPos = 0;
            else
                pMLEditData->vdispPos = edtGetNewvPos (hWnd,
                                GetLineData(pMLEditData,pMLEditData->editLine),
                                pMLEditData->dx_chars[pMLEditData->veditPos] 
                                - edtGetOutWidth (hWnd)/4);
        }else {
            InvRect.left = pMLEditData->dx_chars[pMLEditData->veditPos]
                               - pMLEditData->dx_chars[pMLEditData->vdispPos]    
                                + pMLEditData->leftMargin - 1; 
            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp)
                               * pMLEditData->lineHeight
                            + pMLEditData->topMargin;
            InvRect.bottom = InvRect.top + pMLEditData->lineHeight*2;

        }
    }
    else {
        InvRect.left = InvRect.top = InvRect.right = InvRect.bottom = 0;
        Ping ();
    }
    
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetCaretPos (hWnd);
    InvalidateRect (hWnd, &InvRect, TRUE);// modified by leon
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);

    return 0;
}

static int edtOnChar (HWND hWnd, WPARAM wParam)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    PLINEDATA    pLineData;
    unsigned char charBuffer [2];
    int  i, chars, inserting, oldeditPos;
    int  line, wrapline, oldnwrapline=0;
    RECT InvRect; 
    BOOL bScroll = FALSE;

    if (dwStyle & ES_READONLY) {
        Ping();
        return 0;
    }

    if (HIBYTE (wParam)) {
        charBuffer [0] = LOBYTE (wParam);
        charBuffer [1] = HIBYTE (wParam);
        chars = 2;
    }
    else {
        chars = 1;

        if (dwStyle & ES_UPPERCASE) {
            charBuffer [0] = toupper (LOBYTE (wParam));
        }
        else if (dwStyle & ES_LOWERCASE) {
            charBuffer [0] = tolower (LOBYTE (wParam));
        }
        else
            charBuffer [0] = LOBYTE (wParam);
    }
    
    if (chars == 1) {
#if 0
        switch (charBuffer [0])
        {
            case 0x00:  // NULL
            case 0x07:  // BEL
            case 0x08:  // BS
            case 0x09:  // HT
            case 0x0A:  // LF
            case 0x0B:  // VT
            case 0x0C:  // FF
            case 0x0D:  // CR
            case 0x1B:  // Escape
            return 0;
        }
#else
        if (charBuffer [0] < 0x20 || charBuffer [0] == 0x7F)
            return 0;
#endif
    }

    pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 

    GetClientRect (hWnd, &InvRect);
    InvRect.left = pMLEditData->leftMargin - 1;

    edtPosProc (hWnd); 

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }else
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);

    if (pMLEditData->status & EST_REPLACE) {
        if (pMLEditData->veditPos == pMLEditData->fit_chars)
            inserting = chars;
        else 
            inserting = chars - ( pMLEditData->pos_chars[pMLEditData->veditPos + 1]
                                - pMLEditData->pos_chars[pMLEditData->veditPos] );
    }
    else
        inserting = chars;
    // check space
    if (pLineData->dataEnd + inserting > pMLEditData->lineLimit) {
        Ping ();
        //NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        return 0;
    } 
    if ((pMLEditData->totalLimit >= 0) 
                && ((pMLEditData->curtotalLen + inserting) 
                    > pMLEditData->totalLimit)) {
        Ping ();
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_MAXTEXT);
        return 0;
    }else
        pMLEditData->curtotalLen += inserting;

    if (dwStyle & ES_AUTOWRAP)
    {
        pMLEditData->wraplines -= pLineData->nwrapline;
        oldnwrapline = pLineData->nwrapline;
    }

    edtGetLineInfo (hWnd, pLineData);
    oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos]
                    - pMLEditData->dx_chars[pMLEditData->vdispPos];

    if (inserting == -1) {
        for (i = pMLEditData->pos_chars[pMLEditData->veditPos];
                i < pLineData->dataEnd-1; i++)
                pLineData->buffer [i] = pLineData->buffer [i + 1];
    }
    else if (inserting > 0) {
        for (i = pLineData->dataEnd + inserting - 1; 
                i > pMLEditData->pos_chars[pMLEditData->veditPos] + inserting - 1; 
                i--)
            pLineData->buffer [i] 
                    = pLineData->buffer [i - inserting];
    }
    for (i = 0; i < chars; i++)
            pLineData->buffer [pMLEditData->pos_chars[pMLEditData->veditPos] + i] 
                = charBuffer [i];
    
    pMLEditData->veditPos += 1;
    pLineData->dataEnd += inserting;
    pLineData->buffer[pLineData->dataEnd] = '\0';
    
    edtGetLineInfo (hWnd, pLineData);

    if (dwStyle & ES_AUTOWRAP)
    {
           calcLineInfo (hWnd, pMLEditData, pLineData);
        pMLEditData->wraplines += pLineData->nwrapline;

        if ( (wrapline == pLineData->nwrapline-1
                && pMLEditData->veditPos > pLineData->wrapStartPos[wrapline+1])
            || (wrapline < pLineData->nwrapline-1 
                    && pMLEditData->veditPos >= pLineData->wrapStartPos[wrapline+1]) )
        {
            if (pMLEditData->editLine 
                        == pMLEditData->StartlineDisp+pMLEditData->MaxlinesDisp-1)
            {
                pMLEditData->StartlineDisp ++;
                bScroll = TRUE;
            }
            else
                InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                                    * pMLEditData->lineHeight
                                + pMLEditData->topMargin;
            pMLEditData->editLine++;
            pMLEditData->linesDisp =  MIN (pMLEditData->wraplines 
                                            - pMLEditData->StartlineDisp, 
                                        pMLEditData->MaxlinesDisp);
            pMLEditData->EndlineDisp = 
                            pMLEditData->StartlineDisp + pMLEditData->linesDisp - 1;

        }else {
            if (wrapline == pLineData->nwrapline-1)
                InvRect.left = pMLEditData->leftMargin
                                + pMLEditData->dx_chars[pMLEditData->veditPos-1] 
                                - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]]
                                - 1;
            InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                                * pMLEditData->lineHeight
                            + pMLEditData->topMargin;
            if (oldnwrapline == pLineData->nwrapline)
            {
                InvRect.bottom = InvRect.top
                                + (pLineData->nwrapline-wrapline)*pMLEditData->lineHeight;
            }
        }

        edtSetCaretPos (hWnd);
        InvalidateRect (hWnd, &InvRect,TRUE);
        NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
        if (bScroll)
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }

    
    //for display
    if (((pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[pMLEditData->vdispPos])
                >= edtGetOutWidth (hWnd))
            && (pMLEditData->dx_chars[pMLEditData->veditPos - 1] 
                    - pMLEditData->dx_chars[pMLEditData->vdispPos])
                < edtGetOutWidth (hWnd)) {

        bScroll = TRUE;
        pMLEditData->vdispPos = edtGetNewvPos (hWnd, pLineData, 
                                    pMLEditData->dx_chars[pMLEditData->veditPos]
                                    - edtGetOutWidth (hWnd)*3/4);
    }
    edtSetCaretPos (hWnd);
    if (!bScroll) {
        // only current line to redraw
        InvRect.left = pMLEditData->leftMargin + oldeditPos - 1;
        InvRect.top = (pMLEditData->editLine - pMLEditData->StartlineDisp) 
                        * pMLEditData->lineHeight
                           + pMLEditData->topMargin;
        InvRect.bottom = InvRect.top + pMLEditData->lineHeight;
    }
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    InvalidateRect (hWnd, &InvRect,TRUE);// modified by leon to display
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);
    NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_CHANGE);
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);

    return 0;
}

static int edtOnVScroll (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    int     LinesDisp,newTop;
    RECT     rc;
    int olddispPos;
    int newOff;

    pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 

    if (!(pMLEditData->diff)) {
        pMLEditData->realeditLine = pMLEditData->editLine;
        pMLEditData->realdispPos = pMLEditData->vdispPos;
        pMLEditData->realStartLine = pMLEditData->StartlineDisp;
        pMLEditData->realEndLine   = pMLEditData->EndlineDisp;
        pMLEditData->diff = TRUE;
    }

    GetClientRect (hWnd, &rc);

    LinesDisp = (rc.bottom - rc.top - pMLEditData->topMargin - pMLEditData->bottomMargin)
                   / pMLEditData->lineHeight;
    olddispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    newOff = pMLEditData->dx_chars[pMLEditData->veditPos];
       
    switch (wParam)
    {
        case SB_LINEDOWN:
        {
            if (dwStyle & ES_AUTOWRAP) {
                if (pMLEditData->EndlineDisp == pMLEditData->wraplines -1) return 0;
                if (pMLEditData->EndlineDisp < pMLEditData->wraplines - 1) {
                    pMLEditData->EndlineDisp++;
                    pMLEditData->StartlineDisp++;
                    pMLEditData->editLine++;
                }
            }else {
                if (pMLEditData->EndlineDisp == pMLEditData->lines -1) return 0;
                if (pMLEditData->EndlineDisp < pMLEditData->lines - 1) {
                    pMLEditData->EndlineDisp++;
                    pMLEditData->StartlineDisp++;
                    pMLEditData->editLine++;
                }
            }
        }
        break;
        case SB_LINEUP:
        {
            if (pMLEditData->StartlineDisp == 0)
                return 0;
            if (pMLEditData->StartlineDisp >  0) {
                pMLEditData->StartlineDisp--;
                pMLEditData->EndlineDisp--;
                pMLEditData->editLine--;
            }
        }
        break;
        case SB_PAGEDOWN:
        {
            if (dwStyle & ES_AUTOWRAP) {
                if (pMLEditData->EndlineDisp == pMLEditData->wraplines -1) return 0;
                if (pMLEditData->wraplines - pMLEditData->EndlineDisp > LinesDisp - 1) {
                    pMLEditData->StartlineDisp = pMLEditData->EndlineDisp;
                    pMLEditData->editLine += LinesDisp - 1;
                    pMLEditData->EndlineDisp += LinesDisp -1;
                }else {
                    pMLEditData->EndlineDisp = pMLEditData->wraplines - 1;
                    pMLEditData->editLine += (pMLEditData->EndlineDisp - (LinesDisp - 1))
                                               - pMLEditData->StartlineDisp;
                    pMLEditData->StartlineDisp = pMLEditData->EndlineDisp - (LinesDisp - 1);
                }
            }else {
                if (pMLEditData->EndlineDisp == pMLEditData->lines -1) return 0;
                if (pMLEditData->lines - pMLEditData->EndlineDisp > LinesDisp - 1) {
                    pMLEditData->StartlineDisp = pMLEditData->EndlineDisp;
                    pMLEditData->editLine += LinesDisp - 1;
                    pMLEditData->EndlineDisp += LinesDisp -1;
                }else {
                    pMLEditData->EndlineDisp = pMLEditData->lines - 1;
                    pMLEditData->editLine += (pMLEditData->EndlineDisp - (LinesDisp - 1))
                                               - pMLEditData->StartlineDisp;
                    pMLEditData->StartlineDisp = pMLEditData->EndlineDisp - (LinesDisp - 1);
                }
            }
        }
        break;
        case SB_PAGEUP:
        {
            
            if (pMLEditData->StartlineDisp == 0)
                return 0;
            if (pMLEditData->StartlineDisp > LinesDisp - 1) {
                pMLEditData->EndlineDisp = pMLEditData->StartlineDisp;
                pMLEditData->StartlineDisp -= LinesDisp - 1;
                pMLEditData->editLine -= LinesDisp - 1;
            }else {
                pMLEditData->editLine -= pMLEditData->StartlineDisp; 
                pMLEditData->EndlineDisp = LinesDisp -1;
                pMLEditData->StartlineDisp = 0;
            }
        }
        break;
        case SB_THUMBTRACK:
        {
            newTop = (int) lParam;
            if (dwStyle & ES_AUTOWRAP) {
                if ((newTop > pMLEditData->StartlineDisp  
                                && pMLEditData->EndlineDisp == pMLEditData->wraplines -1)
                        || (newTop == pMLEditData->StartlineDisp))
                    return 0;
                if (newTop + LinesDisp - 1 < pMLEditData->wraplines) {
                    pMLEditData->EndlineDisp += - (pMLEditData->StartlineDisp - newTop);
                    pMLEditData->editLine += - (pMLEditData->StartlineDisp - newTop);
                    pMLEditData->StartlineDisp = newTop;
                }
            }else {
                if ((newTop > pMLEditData->StartlineDisp  
                                    && pMLEditData->EndlineDisp == pMLEditData->lines -1)
                        || (newTop == pMLEditData->StartlineDisp))
                    return 0;
                if (newTop + LinesDisp - 1 < pMLEditData->lines) {
                    pMLEditData->EndlineDisp += - (pMLEditData->StartlineDisp - newTop);
                    pMLEditData->editLine += - (pMLEditData->StartlineDisp - newTop);
                    pMLEditData->StartlineDisp = newTop;
                }
            }
        }
        break;
    }


    if ((pMLEditData->StartlineDisp <= pMLEditData->realeditLine) 
                    && (pMLEditData->EndlineDisp >= pMLEditData->realeditLine)) {
        pMLEditData->diff = FALSE;
        pMLEditData->editLine = pMLEditData->realeditLine;
        edtSetCaretPos (hWnd);

    } else 
        HideCaret (hWnd);

    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    InvalidateRect (hWnd, NULL, TRUE);
        
    return 0;
}

static int edtOnHScroll (HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    int       newPos, width;
    int       olddispPos;
    PLINEDATA temp;
    RECT      rc;

    GetClientRect (hWnd, &rc);

    pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 

    
    temp = GetLineData(pMLEditData, pMLEditData->realeditLine);

    if (!(pMLEditData->diff)) {
        pMLEditData->realeditLine = pMLEditData->editLine;
        pMLEditData->realdispPos = pMLEditData->vdispPos;
        pMLEditData->realStartLine = pMLEditData->StartlineDisp;
        pMLEditData->realEndLine   = pMLEditData->EndlineDisp;
        pMLEditData->diff = TRUE;
    }

    width = rc.right - rc.left - pMLEditData->leftMargin - pMLEditData->rightMargin; 
    
    
    olddispPos = pMLEditData->dispPos;

    switch (wParam)
    {
        case SB_LINERIGHT:
        {
            if (olddispPos + width  >= GetMaxLen (hWnd, pMLEditData))
                return 0;
            pMLEditData->vdispPos ++;
            if (pMLEditData->vdispPos > pMLEditData->fit_chars) {
                pMLEditData->vdispPos = pMLEditData->fit_chars;
                pMLEditData->dispPos += width/10;
                if (pMLEditData->dispPos > GetMaxLen (hWnd, pMLEditData))
                    pMLEditData->dispPos = GetMaxLen (hWnd, pMLEditData);
            }else {
                pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
            }

        }
        break;
        case SB_LINELEFT:
        {
            if (pMLEditData->dispPos == 0)
                return 0;
            if (pMLEditData->dispPos > pMLEditData->dx_chars[pMLEditData->fit_chars]) {
                pMLEditData->dispPos -= width/10;
                if (pMLEditData->dispPos < 0)
                    pMLEditData->dispPos = 0;
            }else {
                pMLEditData->vdispPos --;
                if (pMLEditData->vdispPos < 0)
                    pMLEditData->vdispPos = 0;
                pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
            }
        }
        break;
        case SB_PAGERIGHT:
        {
            if (olddispPos + width  >= GetMaxLen (hWnd, pMLEditData))
                return 0;
            if (olddispPos + width + width/4 <= GetMaxLen (hWnd, pMLEditData)) {
                pMLEditData->vdispPos = edtGetNewvPos (hWnd, temp, olddispPos + width/4);
                if (pMLEditData->vdispPos == -2) {
                    pMLEditData->vdispPos = pMLEditData->fit_chars;
                    pMLEditData->dispPos += width/4;
                } else
                    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];

            }
            else {
                pMLEditData->vdispPos = 
                        edtGetNewvPos (hWnd, temp, GetMaxLen(hWnd, pMLEditData) - width);
                if (pMLEditData->vdispPos == -2) {
                    pMLEditData->vdispPos = pMLEditData->fit_chars;
                    pMLEditData->dispPos = GetMaxLen (hWnd, pMLEditData) - width;
                    if (pMLEditData->dispPos < 0)
                        pMLEditData->dispPos = 0;
                } else
                    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
            }
        }
        break;
        case SB_PAGELEFT:
        {
            if (pMLEditData->dispPos == 0)
                return 0;
            if (olddispPos >= width/4) {
                if (pMLEditData->sz.cx < olddispPos - width/4)
                    pMLEditData->dispPos -= width/4;
                else {
                    pMLEditData->vdispPos = edtGetNewvPos (hWnd, temp, olddispPos - width/4);
                    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
                }
            } else {
                pMLEditData->vdispPos = 0;
                pMLEditData->dispPos = 0;
            }    
        }
        break;
        case SB_THUMBTRACK:
        {
            newPos = (int) lParam;
            
            if (newPos + width > GetMaxLen (hWnd, pMLEditData) 
                                || (newPos < 0))
                return 0;
            if (newPos + width <= GetMaxLen (hWnd, pMLEditData)){
                pMLEditData->vdispPos = 
                        edtGetNewvPos (hWnd, temp, newPos);
                if (pMLEditData->vdispPos == -2) {
                    pMLEditData->vdispPos = pMLEditData->fit_chars;
                    pMLEditData->dispPos = newPos;
                } else
                    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
            }
            
        }
        break;
    }

    
    if ((pMLEditData->dx_chars[pMLEditData->veditPos] >= pMLEditData->dispPos) 
            && (pMLEditData->dx_chars[pMLEditData->veditPos] 
                    <= pMLEditData->dispPos + width)) {
        pMLEditData->diff = FALSE;
        pMLEditData->realdispPos = pMLEditData->vdispPos;
        edtSetCaretPos (hWnd);
    } else 
        HideCaret (hWnd);
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyHome (HWND hWnd)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PLINEDATA    pLineData;
    int         line = 0, wrapline = 0;
    BOOL         bScroll = FALSE;

    if (pMLEditData->veditPos == 0)
        return 0;

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);

        pMLEditData->editLine -= wrapline;
        if (pMLEditData->editLine < pMLEditData->StartlineDisp)
        {
            pMLEditData->StartlineDisp = pMLEditData->editLine;
            pMLEditData->linesDisp =  MIN (pMLEditData->wraplines 
                                            - pMLEditData->StartlineDisp, 
                                        pMLEditData->MaxlinesDisp);
            pMLEditData->EndlineDisp = 
                    pMLEditData->StartlineDisp + pMLEditData->linesDisp - 1;
            bScroll = TRUE;
        }
        edtGetCaretValid (hWnd, pMLEditData->editLine, 0, 0);

        edtSetCaretPos (hWnd);
        if (bScroll)
        {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }
    
    pMLEditData->veditPos = 0;

    if (pMLEditData->vdispPos != 0)
    {
        pMLEditData->vdispPos = 0;
        InvalidateRect (hWnd, NULL, TRUE);
    }
    pMLEditData->dispPos = 0;
    edtSetCaretPos (hWnd);
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyEnd (HWND hWnd)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PLINEDATA    pLineData;

    int      newStartPos, oldeditPos, line, wrapline;
    BOOL     bScroll = FALSE;

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
    }else 
        pLineData = GetLineData(pMLEditData,pMLEditData->editLine);
    
    edtGetLineInfo (hWnd, pLineData);
    if (pMLEditData->veditPos == pMLEditData->fit_chars)
        return 0;
    
    if (dwStyle & ES_AUTOWRAP)
    {
        pMLEditData->veditPos = pMLEditData->fit_chars;
        pMLEditData->editLine += pLineData->nwrapline - 1 - wrapline;
        if (pMLEditData->editLine > pMLEditData->EndlineDisp)
        {
            pMLEditData->EndlineDisp = pMLEditData->editLine;
            pMLEditData->StartlineDisp = 
                    pMLEditData->EndlineDisp - pMLEditData->MaxlinesDisp + 1;
            bScroll = TRUE;
        }

        oldeditPos = pMLEditData->dx_chars[pMLEditData->veditPos] 
            - pMLEditData->dx_chars[pLineData->wrapStartPos[pLineData->nwrapline-1]];
        
        edtGetCaretValid (hWnd, pMLEditData->editLine, 0, oldeditPos);

        edtSetCaretPos (hWnd);
        if (bScroll) {
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect(hWnd,NULL,TRUE);    
        }
        if (pMLEditData->bSCROLL)
            InvalidateRect (hWnd, NULL, TRUE);
        return 0;
    }
    
    newStartPos = edtGetStartDispPosAtEnd (hWnd, pLineData);
    pMLEditData->veditPos = pMLEditData->fit_chars;

    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    if (pMLEditData->vdispPos != newStartPos) {
        pMLEditData->vdispPos = newStartPos;
        pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
        InvalidateRect (hWnd, NULL, TRUE);
    }
    edtSetCaretPos (hWnd);
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);

    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyPageup (HWND hWnd)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PLINEDATA    pLineData;

    int olddispPos, line, wrapline;
    int newOff;
    int LinesDisp = pMLEditData->MaxlinesDisp;
    
    olddispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    newOff = pMLEditData->dx_chars[pMLEditData->veditPos];
    
    if (pMLEditData->StartlineDisp == 0)
        return 0;

    if (dwStyle & ES_AUTOWRAP)
    {
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
        edtGetLineInfo (hWnd, pLineData);
        newOff = pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]];
    }

    if (pMLEditData->StartlineDisp > LinesDisp - 1) {
        pMLEditData->EndlineDisp = pMLEditData->StartlineDisp;
        pMLEditData->StartlineDisp -= LinesDisp - 1;
        pMLEditData->editLine -= LinesDisp - 1;
    }else {
        pMLEditData->editLine -= pMLEditData->StartlineDisp; 
        if (pMLEditData->EndlineDisp > LinesDisp -1)
            pMLEditData->EndlineDisp = LinesDisp -1;
        pMLEditData->StartlineDisp = 0;
    }
    edtGetCaretValid (hWnd, pMLEditData->editLine, olddispPos, newOff);
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
    InvalidateRect (hWnd, NULL, TRUE);
    edtSetCaretPos (hWnd);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

static int edtKeyPagedown (HWND hWnd)
{
    PMLEDITDATA pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
    DWORD dwStyle     = GetWindowStyle(hWnd);
    PLINEDATA    pLineData;

    int olddispPos, line, wrapline;
    int newOff;
    int LinesDisp = pMLEditData->MaxlinesDisp;
    
    olddispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    newOff = pMLEditData->dx_chars[pMLEditData->veditPos];
    
    if (dwStyle & ES_AUTOWRAP)
    {
        if (pMLEditData->EndlineDisp == pMLEditData->wraplines -1) return 0;
        edtGetLineNums (pMLEditData, pMLEditData->editLine, &line, &wrapline);
        pLineData = GetLineData(pMLEditData, line);
        edtGetLineInfo (hWnd, pLineData);
        newOff = pMLEditData->dx_chars[pMLEditData->veditPos] 
                    - pMLEditData->dx_chars[pLineData->wrapStartPos[wrapline]];
        if (pMLEditData->wraplines - pMLEditData->EndlineDisp > LinesDisp - 1) {
            pMLEditData->StartlineDisp = pMLEditData->EndlineDisp;
            pMLEditData->editLine += LinesDisp - 1;
            pMLEditData->EndlineDisp += LinesDisp -1;
        }else {
            pMLEditData->EndlineDisp = pMLEditData->wraplines - 1;
            pMLEditData->editLine += (pMLEditData->EndlineDisp - (LinesDisp - 1))
                                       - pMLEditData->StartlineDisp;
            pMLEditData->StartlineDisp = pMLEditData->EndlineDisp - (LinesDisp - 1);
        }
    }else {
        if (pMLEditData->EndlineDisp == pMLEditData->lines -1) return 0;
    
        if (pMLEditData->lines - pMLEditData->EndlineDisp > LinesDisp - 1) {
            pMLEditData->StartlineDisp = pMLEditData->EndlineDisp;
            pMLEditData->editLine += LinesDisp - 1;
            pMLEditData->EndlineDisp += LinesDisp -1;
        }else {
            pMLEditData->EndlineDisp = pMLEditData->lines - 1;
            pMLEditData->editLine += (pMLEditData->EndlineDisp - (LinesDisp - 1))
                                       - pMLEditData->StartlineDisp;
            pMLEditData->StartlineDisp = pMLEditData->EndlineDisp - (LinesDisp - 1);
        }
    }
    edtGetCaretValid (hWnd, pMLEditData->editLine, olddispPos, newOff);
    pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
    edtSetScrollInfo (hWnd, pMLEditData, TRUE);
       InvalidateRect (hWnd, NULL, TRUE);
    edtSetCaretPos (hWnd);
    if (pMLEditData->bSCROLL)
        InvalidateRect (hWnd, NULL, TRUE);

    return 0;
}

/* --------------------------------------------------------------------------------- */

static int MLEditCtrlProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{   
    DWORD       dwStyle;
    //PLINEDATA   pLineData;
    PMLEDITDATA pMLEditData = NULL;
    dwStyle     = GetWindowStyle(hWnd);

    switch (message)
    {
        case MSG_CREATE:
        {
            if (edtOnCreate (hWnd) < 0)
                return -1;
            break;
        }

        case MSG_DESTROY:
        {
            pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
            DestroyCaret (hWnd);
            MLEditEmptyBuffer(pMLEditData);
            free (pMLEditData->pos_chars);
            free (pMLEditData->dx_chars);
            pMLEditData->pos_chars = NULL;
            pMLEditData->dx_chars = NULL;
            free(pMLEditData); 
        }
        break;
        
        case MSG_SIZECHANGED:
        {
            edtOnSizeChanged (hWnd, (RECT *)lParam);
        }
        break;

        case MSG_FONTCHANGED:
        {
            pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
            pMLEditData->vdispPos = 0;
            pMLEditData->veditPos = 0;
            edtSetCaretPos (hWnd);
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
            InvalidateRect (hWnd, NULL, TRUE);
            return 0;
        }
        
        case MSG_SETCURSOR:
            if (dwStyle & WS_DISABLED) {
                SetCursor (GetSystemCursor (IDC_ARROW));
                return 0;
            }
        break;

        case MSG_KILLFOCUS:
        {
            pMLEditData = (PMLEDITDATA) GetWindowAdditionalData2 (hWnd);
            pMLEditData->status &= ~EST_FOCUSED;
            HideCaret (hWnd);
            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_KILLFOCUS);
        }
        break;
        
        case MSG_SETFOCUS:
        {
            pMLEditData = (PMLEDITDATA) GetWindowAdditionalData2 (hWnd);
            if (pMLEditData->status & EST_FOCUSED)
                break;
            
            pMLEditData->status |= EST_FOCUSED;

            /* only implemented for ES_LEFT align format. */
            edtSetCaretPos (hWnd);

            ShowCaret (hWnd);
            ActiveCaret (hWnd);

            NotifyParent (hWnd, GetDlgCtrlID(hWnd), EN_SETFOCUS);
        }
        break;
        
        case MSG_ENABLE:
            if ( (!(dwStyle & WS_DISABLED) && !wParam)
                    || ((dwStyle & WS_DISABLED) && wParam) ) {
                if (wParam)
                    ExcludeWindowStyle(hWnd,WS_DISABLED);
                else
                    IncludeWindowStyle(hWnd,WS_DISABLED);

                InvalidateRect (hWnd, NULL, TRUE);
            }
            return 0;

        case MSG_ERASEBKGND:
            EditOnEraseBackground (hWnd, (HDC)wParam, (const RECT*)lParam);
            return 0;

        case MSG_PAINT:
            edtOnPaint (hWnd);
            return 0;

        case MSG_KEYDOWN:
        {
            switch (LOWORD (wParam)) {
            case SCANCODE_ENTER:
            {
                return edtKeyEnter (hWnd);
            }
            case SCANCODE_HOME:
            {
                return edtKeyHome (hWnd);
            }
            case SCANCODE_END:
            {
                return edtKeyEnd (hWnd);
            }

            case SCANCODE_CURSORBLOCKLEFT: 
            {
                return edtKeyCursorblockLeft (hWnd);
            }
            
            case SCANCODE_CURSORBLOCKRIGHT:
            {
                return edtKeyCursorblockRight (hWnd);
            }
               
            case SCANCODE_CURSORBLOCKUP:
            {
                return edtKeyCursorblockUp (hWnd);
            }
            case SCANCODE_CURSORBLOCKDOWN:
            {
                return edtKeyCursorblockDown (hWnd);
            }

            case SCANCODE_INSERT:
            {
                pMLEditData =(PMLEDITDATA) GetWindowAdditionalData2(hWnd);
                pMLEditData->status ^= EST_REPLACE;
            }
            break;

            case SCANCODE_REMOVE:
            {
                return edtKeyRemove (hWnd);
            }

            case SCANCODE_BACKSPACE:
            {
                return edtKeyBackspace (hWnd);
            }

            case SCANCODE_PAGEUP:
            {
                return edtKeyPageup (hWnd);
            }

            case SCANCODE_PAGEDOWN:
            {
                return edtKeyPagedown (hWnd);
            }

            default:
            break;
            }
       
            return 0;
        }

        case MSG_CHAR:
        {
            return edtOnChar (hWnd, wParam);
        }

        case MSG_GETTEXTLENGTH:
        {
            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
            return pMLEditData->curtotalLen;
        }
        
        case MSG_GETTEXT:
        {
            PLINEDATA temp;
            int len,total = 0,dataLen=0;
            char * buffer = (char*)lParam;
            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
            len = (int)wParam;
            temp = pMLEditData->head;
            // while (temp && total + temp->dataEnd <= len)  
            while (temp && total < len)  
            {
                if (temp != pMLEditData->head)
                {
                    buffer[total]='\012';
                    total++;
                }
                dataLen = MIN (temp->dataEnd, len-total);
                memcpy(buffer+total,temp->buffer,dataLen);
                total += dataLen;
                temp = temp->next;
            }    
            buffer[total]='\000';//finish flag.
        }
        return 0;

        case MSG_SETTEXT:
        {
            pMLEditData =(PMLEDITDATA)GetWindowAdditionalData2(hWnd);
            MLEditEmptyBuffer(pMLEditData);
            pMLEditData->veditPos        = 0;
            pMLEditData->editLine       = 0;
            pMLEditData->vdispPos         = 0;
            pMLEditData->dispPos        = 0;
            pMLEditData->vdispPos        = 0;
            pMLEditData->StartlineDisp  = 0;
            pMLEditData->wraplines        = 0;
            pMLEditData->curtotalLen    = 0;

            MLEditInitBuffer(hWnd, pMLEditData, (char *)lParam, dwStyle);

            pMLEditData->realdispPos = 0;
            pMLEditData->realStartLine = 0;
            pMLEditData->realEndLine = 0;
            pMLEditData->diff = FALSE;
            pMLEditData->bSCROLL = FALSE;

            edtGetLineInfo (hWnd, pMLEditData->head);
            SetCaretPos (hWnd, pMLEditData->leftMargin,pMLEditData->topMargin);
            InvalidateRect (hWnd, NULL,TRUE);
            edtSetScrollInfo (hWnd, pMLEditData, TRUE);
        }
        return 0;

// can i add it to message defined?
/*
        case MSG_GETLINETEXT:
        {
            PLINEDATA temp;
            char*   buffer = (char*)lParam;
            int     lineNO,len;

            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
            lineNO = (int)wParam;
            temp = GetLineData(pMLEditData,lineNO);
            if(temp)
            {
                len = MIN ((int)wParam, temp->dataEnd);
                memcpy (buffer, temp->buffer,len);
                buffer [len] = '\0';
                return 0;
            }
            return -1;
        }
        break;

        case MSG_SETLINETEXT:
        {
            int len,lineNO;
            PLINEDATA temp;

            if (dwStyle & ES_READONLY)
                return 0;

            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
            
            len = strlen ((char*)lParam);
            lineNO = (int)wParam;
            temp = pMLEditData->head;
            len = MIN (len, pMLEditData->totalLen);
            
            if (pMLEditData->totalLimit >= 0)
                len = MIN (len, pMLEditData->totalLimit);
              while (temp)
            {
                if(temp->lineNO == lineNO)
                {
                     temp->dataEnd = len;
                    memcpy (temp->buffer, (char*)lParam, len);
                }
                temp = temp->next;
            }
            pMLEditData->editPos        = 0;
            pMLEditData->caretPos       = 0;
            pMLEditData->dispPos        = 0;
            InvalidateRect (hWnd, NULL, TRUE);
        }
        return 0;
*/
        case MSG_LBUTTONDBLCLK:
            NotifyParent (hWnd, GetDlgCtrlID (hWnd), EN_DBLCLK);
        break;
        
        case MSG_LBUTTONDOWN:
        {
            int newOff, lineNO, olddispPos;
            BOOL bScroll = FALSE;
            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd);

            pMLEditData->diff = FALSE;
           
            olddispPos = pMLEditData->dispPos;
            lineNO = edtGetLineNO (pMLEditData, HIWORD (lParam));
            if (lineNO < 0) return 0;
            lineNO += pMLEditData->StartlineDisp;
            if (dwStyle & ES_AUTOWRAP) {
                if (lineNO > pMLEditData->wraplines - 1) return 0;
            }else {
                if (lineNO > pMLEditData->lines - 1) return 0;
            }
            pMLEditData->editLine = lineNO;
            
            newOff = LOSWORD (lParam) + olddispPos-pMLEditData->leftMargin;

            bScroll = edtGetCaretValid (hWnd,lineNO, olddispPos, newOff);

            pMLEditData->dispPos = pMLEditData->dx_chars[pMLEditData->vdispPos];
            
            edtSetCaretPos (hWnd);
            //FIXME
            if (bScroll) {
                edtSetScrollInfo (hWnd, pMLEditData, TRUE);
                InvalidateRect(hWnd,NULL,TRUE);    
            }
            break;
        }

#if 0
        case MSG_LBUTTONUP:
        case MSG_MOUSEMOVE:
        break;
#endif
        
        case MSG_GETDLGCODE:
            return DLGC_WANTCHARS | DLGC_HASSETSEL | DLGC_WANTARROWS | DLGC_WANTENTER;
        
        case MSG_DOESNEEDIME:
            if (dwStyle & ES_READONLY)
                return FALSE;
            return TRUE;
        
        case EM_GETCARETPOS:
        {
            int* line_pos = (int *)wParam;
            int* char_pos = (int *)lParam;

            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 

            if (line_pos) *line_pos = pMLEditData->editLine;
            if (char_pos) *char_pos = pMLEditData->veditPos;

            return pMLEditData->veditPos; 
        }

        case EM_SETREADONLY:
            if (wParam)
                IncludeWindowStyle(hWnd,ES_READONLY);
            else
                ExcludeWindowStyle(hWnd,ES_READONLY);
            return 0;
        
        case EM_SETPASSWORDCHAR:
            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 

            if (pMLEditData->passwdChar != (int)wParam) {
                if (dwStyle & ES_PASSWORD) {
                    pMLEditData->passwdChar = (int)wParam;
                    InvalidateRect (hWnd, NULL, TRUE);
                }
            }
            return 0;
    
        case EM_GETPASSWORDCHAR:
        {
            int* passwdchar;
            
            pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
            passwdchar = (int*) lParam;

            *passwdchar = pMLEditData->passwdChar;
            return 0;
        }
    
        case EM_LIMITTEXT:
        {
            int newLimit = (int)wParam;
            
            if (newLimit >= 0) {
                pMLEditData = (PMLEDITDATA)GetWindowAdditionalData2(hWnd); 
                if (newLimit < pMLEditData->lineLimit)
                {
                    pMLEditData->totalLimit = pMLEditData->lineLimit = newLimit;
                }else {
                    pMLEditData->totalLimit = newLimit;
                }
            }
            return 0;
        }
        
        case MSG_VSCROLL:
        {
            return edtOnVScroll (hWnd, wParam, lParam);
        }

        case MSG_HSCROLL:
        {
            return edtOnHScroll (hWnd, wParam, lParam);
        }
    
        default:
        break;
    } 

    return DefaultControlProc (hWnd, message, wParam, lParam);
}

#endif /* _CTRL_OLDMEDIT */

