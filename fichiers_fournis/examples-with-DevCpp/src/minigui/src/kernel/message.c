/*
** $Id: message.c,v 1.22.6.1 2005/02/15 08:27:56 weiym Exp $
**
** message.c: The Messaging module.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Current maintainer: Wei Yongming.
**
** Create date: 2000/11/05
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

#include "blockheap.h"
#include "cliprect.h"
#include "gal.h"
#include "internals.h"
#include "ctrlclass.h"
#include "timer.h"

/****************************** Message support ******************************/
static BLOCKHEAP QMSGHeap;

/* QMSG allocation */
BOOL InitFreeQMSGList (void)
{
    InitBlockDataHeap (&QMSGHeap, sizeof (QMSG), SIZE_QMSG_HEAP);
    return TRUE;
}

void DestroyFreeQMSGList (void)
{
    DestroyBlockDataHeap (&QMSGHeap);
}

inline static PQMSG QMSGAlloc (void)
{
    return (PQMSG) BlockDataAlloc (&QMSGHeap);
}

inline static void FreeQMSG (PQMSG pqmsg)
{
    BlockDataFree (&QMSGHeap, pqmsg);
}

BOOL InitMsgQueue (PMSGQUEUE pMsgQueue, int iBufferLen)
{
    int i;
    
    pMsgQueue->dwState = QS_EMPTY;

#ifndef _LITE_VERSION
    pthread_mutex_init (&pMsgQueue->lock, NULL);
    sem_init (&pMsgQueue->wait, 0, 0);
#endif

    pMsgQueue->pFirstNotifyMsg = NULL;
    pMsgQueue->pLastNotifyMsg = NULL;

    pMsgQueue->readpos = 0;
    pMsgQueue->writepos = 0;
#ifndef _LITE_VERSION
    pMsgQueue->pFirstSyncMsg = NULL;
    pMsgQueue->pLastSyncMsg = NULL;
#endif

    if (iBufferLen <= 0)
        iBufferLen = DEF_MSGQUEUE_LEN;
        
    pMsgQueue->msg = malloc (sizeof (MSG) * iBufferLen);
    pMsgQueue->len = iBufferLen;

#ifndef _LITE_VERSION
    pMsgQueue->TimerMask = 0xFF;
#else
    pMsgQueue->TimerMask = 0xFFFF;
#endif
    for (i = 0; i < DEF_NR_TIMERS; i++) {
        pMsgQueue->TimerOwner [i] = HWND_DESKTOP;
        pMsgQueue->TimerID [i] = 0;
    }

    return pMsgQueue->msg != NULL;
}

void DestroyMsgQueue (PMSGQUEUE pMsgQueue)
{
    PQMSG head;
    PQMSG next;

    head = next = pMsgQueue->pFirstNotifyMsg;
    while (head) {
        next = head->next;

        FreeQMSG (head);
        head = next;
    }

#ifndef _LITE_VERSION
    pthread_mutex_destroy (&pMsgQueue->lock);
    sem_destroy (&pMsgQueue->wait);
#endif

    free (pMsgQueue->msg);
    pMsgQueue->msg = NULL;
}

#ifdef _LITE_VERSION
BOOL InitDskMsgQueue (void)
{
    return InitMsgQueue (&__mg_dsk_msgs, 0);
}

void DestroyDskMsgQueue (void)
{
    DestroyMsgQueue (&__mg_dsk_msgs);
}
#endif

PMAINWIN CheckAndGetMainWindowPtr (HWND hWnd)
{
    PMAINWIN pWin;

    pWin = (PMAINWIN)hWnd;
    if(pWin->DataType != TYPE_HWND)
        return NULL;

    if (pWin->WinType == TYPE_MAINWIN)
        return pWin;

    return NULL; 
}

PMAINWIN GetMainWindowPtrOfControl (HWND hWnd)
{
    PMAINWIN pWin;

    if (hWnd == HWND_DESKTOP || hWnd == HWND_INVALID)
        return NULL;

    pWin = (PMAINWIN)hWnd;
    if(pWin->DataType != TYPE_HWND)
        return NULL;

    if (pWin->WinType == TYPE_MAINWIN)
        return pWin;

    return ((PCONTROL)hWnd)->pMainWin;
}

PMSGQUEUE GetMsgQueue (HWND hWnd)
{
    if (hWnd == HWND_DESKTOP) return &__mg_dsk_msgs;

    return GetMainWindowPtrOfControl (hWnd)->pMessages;
}

static inline WNDPROC GetWndProc (HWND hWnd)
{
     PMAINWIN  pMainWin;
     
     pMainWin = (PMAINWIN)hWnd;

     if (hWnd == HWND_DESKTOP)
         return DesktopWinProc;

     return pMainWin->MainWindowProc;
}

static HWND msgCheckInvalidRegion (PMAINWIN pWin)
{
    PCONTROL pCtrl;
    HWND hwnd;

    pCtrl = (PCONTROL)pWin;

    if (pCtrl->InvRgn.rgn.head)
        return (HWND)pCtrl;

    pCtrl = pCtrl->children;
    while (pCtrl) {

        if ((hwnd = msgCheckInvalidRegion ((PMAINWIN) pCtrl)))
            return hwnd;

        pCtrl = pCtrl->next;
    }

    return 0;
}

static PMAINWIN msgGetHostingRoot (PMAINWIN pHosted)
{
    PMAINWIN pHosting;
    
    pHosting = pHosted->pHosting;
    if (pHosting)
        return msgGetHostingRoot (pHosting);

    return pHosted;
}

static HWND msgCheckHostedTree (PMAINWIN pHosting)
{
    HWND hNeedPaint;
    PMAINWIN pHosted;

    if ( (hNeedPaint = msgCheckInvalidRegion (pHosting)) )
        return hNeedPaint;

    pHosted = pHosting->pFirstHosted;
    while (pHosted) {
        if ( (hNeedPaint = msgCheckHostedTree (pHosted)) )
            return hNeedPaint;

        pHosted = pHosted->pNextHosted;
    }

    return 0;
}

static void CheckCapturedMouseMessage (PMSG pMsg)
{
    if (pMsg->message >= MSG_FIRSTMOUSEMSG 
            && pMsg->message <= MSG_LASTMOUSEMSG) {
#ifndef _LITE_VERSION
        if (__mg_capture_wnd && (__mg_capture_wnd != HWND_INVALID)
                && (GetMsgQueue (__mg_capture_wnd) == GetMsgQueue (pMsg->hwnd))) {
#else
        if (__mg_capture_wnd && (__mg_capture_wnd != HWND_INVALID) 
                && pMsg->hwnd != HWND_DESKTOP) {
#endif
            if (!(pMsg->wParam | KS_CAPTURED)) {
                int x, y;
                x = LOSWORD (pMsg->lParam);
                y = HISWORD (pMsg->lParam);

                ClientToScreen (pMsg->hwnd, &x, &y);

                pMsg->lParam = MAKELONG (x, y);
                pMsg->wParam |= KS_CAPTURED;
            }
            pMsg->hwnd = __mg_capture_wnd;
        }
    }
}

BOOL GUIAPI HavePendingMessage (HWND hWnd)
{
    PMSGQUEUE pMsgQueue;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return FALSE;
    
    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
        if (pMsgQueue->pFirstNotifyMsg)
            return TRUE;
    }

#ifndef _LITE_VERSION
    if (pMsgQueue->dwState & QS_SYNCMSG) {
        if (pMsgQueue->pFirstSyncMsg)
            return TRUE;
    }
#endif

    if (pMsgQueue->dwState & QS_POSTMSG) {
        if (pMsgQueue->readpos != pMsgQueue->writepos)
            return TRUE;
    }

#ifndef _LITE_VERSION
    if (pMsgQueue->dwState & (QS_QUIT | QS_PAINT | QS_TIMER))
#else
    if (pMsgQueue->dwState & (QS_QUIT | QS_PAINT | QS_TIMER | QS_DESKTIMER))
#endif
        return TRUE;

#ifdef _LITE_VERSION
    return pMsgQueue->OnIdle (NULL);
#else
    return FALSE;
#endif
}

int GUIAPI BroadcastMessage (int iMsg, WPARAM wParam, LPARAM lParam)
{
    MSG msg;
    
    msg.message = iMsg;
    msg.wParam = wParam;
    msg.lParam = lParam;

    return SendMessage (HWND_DESKTOP, MSG_BROADCASTMSG, 0, (LPARAM)(&msg));
}

#ifdef _MSG_STRING
#include "msgstr.h"

const char* GUIAPI Message2Str (int message)
{
    if (message >= 0x0000 && message <= 0x006F)
        return msgstr1 [message];
    else if (message >= 0x00A0 && message <= 0x010F)
        return msgstr2 [message - 0x00A0];
    else if (message >= 0x0120 && message <= 0x017F)
        return msgstr3 [message - 0x0120];
    else if (message >= 0xF000)
        return "Control Messages";
    else
        return "MSG_USER";
}

void GUIAPI PrintMessage (FILE* fp, HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    fprintf (fp, "Message %s: hWnd: %#x, wP: %x, lP: %lx.\n",
        Message2Str (iMsg), hWnd, wParam, lParam);
}
#endif

#ifdef _LITE_VERSION
int GUIAPI GetMessage (PMSG pMsg, HWND hWnd)
{
    PMSGQUEUE pMsgQueue;
    PQMSG phead;
    int slot;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    memset (pMsg, 0, sizeof(MSG));

checkagain:

    if (pMsgQueue->dwState & QS_QUIT) {
        pMsg->hwnd = hWnd;
        pMsg->message = MSG_QUIT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;

        pMsgQueue->dwState &= ~QS_QUIT;
        
        return 0;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
       
        if (pMsgQueue->pFirstNotifyMsg) {
            phead = pMsgQueue->pFirstNotifyMsg;
            pMsgQueue->pFirstNotifyMsg = phead->next;
            
            *pMsg = phead->Msg;

            FreeQMSG (phead);

            return 1;
        }
        else
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
        
    }

    if (pMsgQueue->dwState & QS_POSTMSG) {
//        uHALr_printf("QS_POSTMSG present\n");
        if (pMsgQueue->readpos != pMsgQueue->writepos) {

            *pMsg = pMsgQueue->msg[pMsgQueue->readpos];
            CheckCapturedMouseMessage (pMsg);

            pMsgQueue->readpos++;
            if (pMsgQueue->readpos >= pMsgQueue->len) pMsgQueue->readpos = 0;
            return 1;
        }
        else
            pMsgQueue->dwState &= ~QS_POSTMSG;

    }

    if (pMsgQueue->dwState & QS_PAINT) {
        PMAINWIN pHostingRoot;
        HWND hNeedPaint;
        PMAINWIN pWin;
        
        if (hWnd == HWND_DESKTOP) {
            pMsg->hwnd = hWnd;
            pMsg->message = MSG_PAINT;
            pMsg->wParam = 0;
            pMsg->lParam = 0;

            pMsgQueue->dwState &= ~QS_PAINT;
            return 1;
        }

        pMsg->message = MSG_PAINT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;

        pWin = GetMainWindowPtrOfControl (hWnd);
        pHostingRoot = msgGetHostingRoot (pWin);

        if ( (hNeedPaint = msgCheckHostedTree (pHostingRoot)) ) {
            pMsg->hwnd = hNeedPaint;
            pWin = (PMAINWIN) hNeedPaint;
            pMsg->lParam = (LPARAM)(&pWin->InvRgn.rgn);
            return 1;
        }
        
        pMsgQueue->dwState &= ~QS_PAINT;
    }
    
    if (pMsgQueue->dwState & QS_DESKTIMER) {
        pMsg->hwnd = HWND_DESKTOP;
        pMsg->message = MSG_TIMER;
        pMsg->wParam = 0;
        pMsg->lParam = 0;

        pMsgQueue->dwState &= ~QS_DESKTIMER;
        return 1;
    }

    if (pMsgQueue->dwState & QS_TIMER) {
        for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
            if (pMsgQueue->dwState & (0x01 << slot))
                break;
        }

        if (slot == DEF_NR_TIMERS) {
            pMsgQueue->dwState &= ~QS_TIMER;
        }
        else {
            pMsg->hwnd = pMsgQueue->TimerOwner[slot];
            pMsg->message = MSG_TIMER;
            pMsg->wParam = pMsgQueue->TimerID[slot];
            pMsg->lParam = 0;

            pMsgQueue->dwState &= ~(0x01 << slot);
            return 1;
        }
    }

    // no message, wait again.
    pMsgQueue->OnIdle (pMsgQueue);
    goto checkagain;

    return 1;
}

BOOL GUIAPI EmptyMessageQueue (HWND hWnd)
{
    int i;
    PMSGQUEUE   pMsgQueue;
    PQMSG       pQMsg, temp;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return FALSE;

    if (pMsgQueue->pFirstNotifyMsg) {
        pQMsg = pMsgQueue->pFirstNotifyMsg;
        while (pQMsg) {
            temp = pQMsg->next;
            FreeQMSG (pQMsg);
            pQMsg = temp;
        }
    }

    pMsgQueue->pFirstNotifyMsg = NULL;
    pMsgQueue->pLastNotifyMsg = NULL;

    pMsgQueue->readpos = 0;
    pMsgQueue->writepos = 0;

    for (i = 0; i < DEF_NR_TIMERS; i++) {
        pMsgQueue->TimerOwner [i] = HWND_DESKTOP;
        pMsgQueue->TimerID [i] = 0;
    }
    TerminateTimer ();

    pMsgQueue->dwState = QS_EMPTY;
    pMsgQueue->TimerMask = 0xFFFF;

    return TRUE;
}

int GUIAPI ThrowAwayMessages (HWND hWnd)
{
    PMSG        pMsg;
    PMSGQUEUE   pMsgQueue;
    PQMSG       pQMsg;
    int         nCount = 0;
    int         readpos;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    if (pMsgQueue->pFirstNotifyMsg) {
        pQMsg = pMsgQueue->pFirstNotifyMsg;
        
        while (pQMsg) {
            pMsg = &pQMsg->Msg;

            if (pMsg->hwnd == hWnd
                    || GetMainWindowPtrOfControl (pMsg->hwnd) == (PMAINWIN)hWnd) {
                pMsg->hwnd = HWND_INVALID;
                nCount ++;
            }
            pQMsg = pQMsg->next;
        }
    }

    readpos = pMsgQueue->readpos;
    while (readpos != pMsgQueue->writepos) {
        pMsg = pMsgQueue->msg + readpos;

        if (pMsg->hwnd == hWnd
                || GetMainWindowPtrOfControl (pMsg->hwnd) == (PMAINWIN)hWnd) {
            pMsg->hwnd = HWND_INVALID;
            nCount ++;
        }
        
        readpos++;
        if (readpos >= pMsgQueue->len) 
            readpos = 0;
    }

    return nCount;
}

BOOL GUIAPI PeekPostMessage (PMSG pMsg, HWND hWnd, int iMsgFilterMin, 
                        int iMsgFilterMax, UINT uRemoveMsg)
{
    PMSGQUEUE pMsgQueue;
    PMSG pPostMsg;
    
    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return FALSE;

    memset (pMsg, 0, sizeof(MSG));

    if (pMsgQueue->dwState & QS_POSTMSG) {
    
        if (pMsgQueue->readpos != pMsgQueue->writepos) {

            pPostMsg = pMsgQueue->msg + pMsgQueue->readpos;

            if (iMsgFilterMin == 0 && iMsgFilterMax == 0)
                *pMsg = *pPostMsg;
            else if (pPostMsg->message <= iMsgFilterMax &&
                    pPostMsg->message >= iMsgFilterMin)
                *pMsg = *pPostMsg;
            else {
                return FALSE;
            }
            
            if (uRemoveMsg == PM_REMOVE) {
                pMsgQueue->readpos++;
                if (pMsgQueue->readpos >= pMsgQueue->len) 
                    pMsgQueue->readpos = 0;
            }

            return TRUE;
        }
    }

    return FALSE;
}
                        
int GUIAPI PostMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    PMSG pMsg;

//    uHALr_printf("Post Message %d\n",iMsg);
    
    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    if (iMsg == MSG_PAINT) {
        pMsgQueue->dwState |= QS_PAINT;
        goto goodret;
    }
    
    if ((pMsgQueue->writepos + 1) % pMsgQueue->len == pMsgQueue->readpos) {
        return ERR_QUEUE_FULL;
    }
    
    // Write the data and advance write pointer */
    pMsg = &(pMsgQueue->msg[pMsgQueue->writepos]);
    pMsg->hwnd = hWnd;
    pMsg->message = iMsg;
    pMsg->wParam = wParam;
    pMsg->lParam = lParam;

    pMsgQueue->writepos++;
    if (pMsgQueue->writepos >= pMsgQueue->len) pMsgQueue->writepos = 0;

    pMsgQueue->dwState |= QS_POSTMSG;

goodret:
    return ERR_OK;
}


int GUIAPI PostQuitMessage (HWND hWnd)
{
    PMSGQUEUE pMsgQueue;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    pMsgQueue->dwState |= QS_QUIT;
    return ERR_OK;
}

int GUIAPI SendMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC WndProc;

    if (hWnd == HWND_INVALID) return -1;
    WndProc = GetWndProc (hWnd);
    return (*WndProc)(hWnd, iMsg, wParam, lParam);
}

int GUIAPI SendNotifyMessage (HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    PQMSG pqmsg;

    if (hWnd == HWND_INVALID) return -1;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    pqmsg = QMSGAlloc();

    // queue the notification message.
    pqmsg->Msg.hwnd = hWnd;
    pqmsg->Msg.message = iMsg;
    pqmsg->Msg.wParam = wParam;
    pqmsg->Msg.lParam = lParam;
    pqmsg->next = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL) {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else {
        pMsgQueue->pLastNotifyMsg->next = pqmsg;
        pMsgQueue->pLastNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;

    return ERR_OK;
}

#ifdef _TRACE_MSG

int GUIAPI DispatchMessage (PMSG pMsg)
{
    WNDPROC WndProc;
    int iRet;

    uHALr_printf( "Message, %s: hWnd: %x, wP: %x, lP: %lx.\n",
        Message2Str (pMsg->message),
        pMsg->hwnd,
        pMsg->wParam,
        pMsg->lParam);

    if (pMsg->hwnd == HWND_INVALID)
        return HWND_INVALID;

    WndProc = GetWndProc (pMsg->hwnd);

    iRet = (*WndProc)(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);

    uHALr_printf( "Message, %s done, return value: %x\n",
        Message2Str (pMsg->message),
        iRet);

    return iRet;
}

#else

int GUIAPI DispatchMessage(PMSG pMsg)
{
    WNDPROC WndProc;

    if (pMsg->hwnd == HWND_INVALID)
        return HWND_INVALID;

//    uHALr_printf("Dispatch Message hwnd=%p\n",pMsg->hwnd);
    WndProc = GetWndProc(pMsg->hwnd);
    return (*WndProc)(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
}

#endif /* !TRACE_MSG */

#else
int GUIAPI GetMessage (PMSG pMsg, HWND hWnd)
{
    PMSGQUEUE pMsgQueue;
    PQMSG phead;
    int slot;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    memset (pMsg, 0, sizeof(MSG));

checkagain:

    if (pMsgQueue->dwState & QS_QUIT) {
        pMsg->hwnd = hWnd;
        pMsg->message = MSG_QUIT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;
        pMsg->pAdd = NULL;

        pMsgQueue->dwState &= ~QS_QUIT;
        
        return 0;
    }

    if (pMsgQueue->dwState & QS_NOTIFYMSG) {
       
        pthread_mutex_lock (&pMsgQueue->lock);
        if (pMsgQueue->pFirstNotifyMsg) {
            phead = pMsgQueue->pFirstNotifyMsg;
            pMsgQueue->pFirstNotifyMsg = phead->next;
            
            *pMsg = phead->Msg;
            pMsg->pAdd = NULL;

            FreeQMSG (phead);

            pthread_mutex_unlock (&pMsgQueue->lock);
            return 1;
        }
        else
            pMsgQueue->dwState &= ~QS_NOTIFYMSG;
        
        pthread_mutex_unlock (&pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_SYNCMSG) {
        pthread_mutex_lock (&pMsgQueue->lock);
        if (pMsgQueue->pFirstSyncMsg) {
            *pMsg = pMsgQueue->pFirstSyncMsg->Msg;
            pMsg->pAdd = pMsgQueue->pFirstSyncMsg;
            pMsgQueue->pFirstSyncMsg = pMsgQueue->pFirstSyncMsg->pNext;

            pthread_mutex_unlock (&pMsgQueue->lock);
            return 1;
        }
        else
            pMsgQueue->dwState &= ~QS_SYNCMSG;
            
        pthread_mutex_unlock (&pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_POSTMSG) {
    
        pthread_mutex_lock (&pMsgQueue->lock);
        if (pMsgQueue->readpos != pMsgQueue->writepos) {

            *pMsg = pMsgQueue->msg[pMsgQueue->readpos];
            CheckCapturedMouseMessage (pMsg);
            pMsg->pAdd = NULL;

            pMsgQueue->readpos++;
            if (pMsgQueue->readpos >= pMsgQueue->len) pMsgQueue->readpos = 0;

            pthread_mutex_unlock (&pMsgQueue->lock);
            return 1;
        }
        else
            pMsgQueue->dwState &= ~QS_POSTMSG;

        pthread_mutex_unlock (&pMsgQueue->lock);
    }

    if (pMsgQueue->dwState & QS_PAINT) {
        PMAINWIN pHostingRoot;
        HWND hNeedPaint;
        PMAINWIN pWin;
        
        if (hWnd == HWND_DESKTOP) {
            pMsg->hwnd = hWnd;
            pMsg->message = MSG_PAINT;
            pMsg->wParam = 0;
            pMsg->lParam = 0;
            pMsg->pAdd = NULL;

            pthread_mutex_lock (&pMsgQueue->lock);
            pMsgQueue->dwState &= ~QS_PAINT;
            pthread_mutex_unlock (&pMsgQueue->lock);
            return 1;
        }

        pMsg->message = MSG_PAINT;
        pMsg->wParam = 0;
        pMsg->lParam = 0;
        pMsg->pAdd = NULL;

        pWin = GetMainWindowPtrOfControl (hWnd);
        pHostingRoot = msgGetHostingRoot (pWin);

        if ( (hNeedPaint = msgCheckHostedTree (pHostingRoot)) ) {
            pMsg->hwnd = hNeedPaint;
            pWin = (PMAINWIN) hNeedPaint;
            pMsg->lParam = (LPARAM)(&pWin->InvRgn.rgn);
            return 1;
        }
        
        pthread_mutex_lock (&pMsgQueue->lock);
        pMsgQueue->dwState &= ~QS_PAINT;
        pthread_mutex_unlock (&pMsgQueue->lock);
    }
    
    if (pMsgQueue->dwState & QS_TIMER) {
        if (hWnd == HWND_DESKTOP) {
            pMsg->hwnd = hWnd;
            pMsg->message = MSG_TIMER;
            pMsg->wParam = 0;
            pMsg->lParam = 0;
            pMsg->pAdd = NULL;

            pthread_mutex_lock (&pMsgQueue->lock);
            pMsgQueue->dwState &= ~0x01;
            pthread_mutex_unlock (&pMsgQueue->lock);

            return 1;
        }
        
        pthread_mutex_lock (&pMsgQueue->lock);
        for (slot = 0; slot < DEF_NR_TIMERS; slot++) {
            if (pMsgQueue->dwState & (0x01 << slot))
                break;
        }

        if (slot == DEF_NR_TIMERS) {
            pMsgQueue->dwState &= ~QS_TIMER;
            pthread_mutex_unlock (&pMsgQueue->lock);
        }
        else {
            pMsg->hwnd = pMsgQueue->TimerOwner[slot];
            pMsg->message = MSG_TIMER;
            pMsg->wParam = pMsgQueue->TimerID[slot];
            pMsg->lParam = 0;
            pMsg->pAdd = NULL;

            pMsgQueue->dwState &= ~(0x01 << slot);
            pthread_mutex_unlock (&pMsgQueue->lock);

            return 1;
        }
    }

    // no message, wait again.
    sem_wait (&pMsgQueue->wait);
    goto checkagain;

    return 1;
}

int GUIAPI ThrowAwayMessages (HWND hWnd)
{
    PMSG        pMsg;
    PMSGQUEUE   pMsgQueue;
    PQMSG       pQMsg;
    PSYNCMSG    pSyncMsg;
    int         nCount = 0;
    int         readpos;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    pthread_mutex_lock (&pMsgQueue->lock);

    if (pMsgQueue->pFirstNotifyMsg) {
        pQMsg = pMsgQueue->pFirstNotifyMsg;
        
        while (pQMsg) {
            pMsg = &pQMsg->Msg;

            if (pMsg->hwnd == hWnd
                    || GetMainWindowPtrOfControl (pMsg->hwnd) == (PMAINWIN)hWnd) {
                pMsg->hwnd = HWND_INVALID;
                nCount ++;
            }

            pQMsg = pQMsg->next;
        }
    }

    /* FIXME: notify the waiting window */
    if (pMsgQueue->pFirstSyncMsg) {
        pSyncMsg = pMsgQueue->pFirstSyncMsg;
        
        while (pSyncMsg) {
            pMsg = &pSyncMsg->Msg;

            if (pMsg->hwnd == hWnd
                    || GetMainWindowPtrOfControl (pMsg->hwnd) == (PMAINWIN)hWnd) {
                pMsg->hwnd = HWND_INVALID;
                nCount ++;
            }

            pSyncMsg = pSyncMsg->pNext;
        }
    }

    readpos = pMsgQueue->readpos;
    while (readpos != pMsgQueue->writepos) {
        pMsg = pMsgQueue->msg + readpos;

        if (pMsg->hwnd == hWnd
                || GetMainWindowPtrOfControl (pMsg->hwnd) == (PMAINWIN)hWnd) {
            pMsg->hwnd = HWND_INVALID;
            nCount ++;
        }
        
        readpos++;
        if (readpos >= pMsgQueue->len) 
            readpos = 0;
    }

    pthread_mutex_unlock (&pMsgQueue->lock);

    return nCount;
}

BOOL GUIAPI PeekPostMessage (PMSG pMsg, HWND hWnd, int iMsgFilterMin, 
                        int iMsgFilterMax, UINT uRemoveMsg)
{
    PMSGQUEUE pMsgQueue;
    PMSG pPostMsg;
    
    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return FALSE;

    memset (pMsg, 0, sizeof(MSG));

    if (pMsgQueue->dwState & QS_POSTMSG) {
    
        pthread_mutex_lock (&pMsgQueue->lock);
        if (pMsgQueue->readpos != pMsgQueue->writepos) {

            pPostMsg = pMsgQueue->msg + pMsgQueue->readpos;

            if (iMsgFilterMin == 0 && iMsgFilterMax == 0)
                *pMsg = *pPostMsg;
            else if (pPostMsg->message <= iMsgFilterMax &&
                    pPostMsg->message >= iMsgFilterMin)
                *pMsg = *pPostMsg;
            else {
                pthread_mutex_unlock (&pMsgQueue->lock);
                return FALSE;
            }
            

            pMsg->pAdd = NULL;

            if (uRemoveMsg == PM_REMOVE) {
                pMsgQueue->readpos++;
                if (pMsgQueue->readpos >= pMsgQueue->len) 
                    pMsgQueue->readpos = 0;
            }

            pthread_mutex_unlock (&pMsgQueue->lock);
            return TRUE;
        }

        pthread_mutex_unlock (&pMsgQueue->lock);
    }

    return FALSE;
}
                        
int GUIAPI PostMessage(HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    PMSG pMsg;
    int sem_value;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    pthread_mutex_lock (&pMsgQueue->lock);

    if (iMsg == MSG_PAINT) {
        pMsgQueue->dwState |= QS_PAINT;
        goto goodret;
    }
    
    if ((pMsgQueue->writepos + 1) % pMsgQueue->len == pMsgQueue->readpos) {
        pthread_mutex_unlock (&pMsgQueue->lock);
        return ERR_QUEUE_FULL;
    }
    
    // Write the data and advance write pointer */
    pMsg = &(pMsgQueue->msg[pMsgQueue->writepos]);
    pMsg->hwnd = hWnd;
    pMsg->message = iMsg;
    pMsg->wParam = wParam;
    pMsg->lParam = lParam;

    pMsgQueue->writepos++;
    if (pMsgQueue->writepos >= pMsgQueue->len) pMsgQueue->writepos = 0;

    pMsgQueue->dwState |= QS_POSTMSG;

goodret:

    pthread_mutex_unlock (&pMsgQueue->lock);
    // Signal that the msg queue contains one more element for reading
    sem_getvalue (&pMsgQueue->wait, &sem_value);
    if (sem_value <= 0)
        sem_post(&pMsgQueue->wait);

    return ERR_OK;
}


int GUIAPI PostQuitMessage(HWND hWnd)
{
    PMSGQUEUE pMsgQueue;
    int sem_value;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    pMsgQueue->dwState |= QS_QUIT;

    // Signal that the msg queue contains one more element for reading
    sem_getvalue (&pMsgQueue->wait, &sem_value);
    if (sem_value <= 0)
        sem_post(&pMsgQueue->wait);

    return ERR_OK;
}

int GUIAPI PostSyncMessage(HWND hWnd, int msg, WPARAM wParam, LPARAM lParam)
{
    PMSGQUEUE pMsgQueue;
    SYNCMSG SyncMsg;
    int sem_value;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

    // queue the sync message.
    SyncMsg.Msg.hwnd = hWnd;
    SyncMsg.Msg.message = msg;
    SyncMsg.Msg.wParam = wParam;
    SyncMsg.Msg.lParam = lParam;
    SyncMsg.pNext = NULL;
    sem_init (&SyncMsg.sem_handle, 0, 0);

    pthread_mutex_lock (&pMsgQueue->lock);

    if (pMsgQueue->pFirstSyncMsg == NULL) {
        pMsgQueue->pFirstSyncMsg = pMsgQueue->pLastSyncMsg = &SyncMsg;
    }
    else {
        pMsgQueue->pLastSyncMsg->pNext = &SyncMsg;
        pMsgQueue->pLastSyncMsg = &SyncMsg;
    }

    pMsgQueue->dwState |= QS_SYNCMSG;

    pthread_mutex_unlock (&pMsgQueue->lock);

    // Signal that the msg queue contains one more element for reading
    sem_getvalue (&pMsgQueue->wait, &sem_value);
    if (sem_value <= 0)
        sem_post(&pMsgQueue->wait);

    // suspend until the message been handled.
    sem_wait(&SyncMsg.sem_handle);

    sem_destroy(&SyncMsg.sem_handle);

    return SyncMsg.retval;
}

int GUIAPI SendMessage(HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC WndProc;
    PMAINWIN pMainWin;

    if (hWnd == HWND_INVALID) return -1;

    if (hWnd == HWND_DESKTOP) {
        if (pthread_self() != __mg_desktop)
            return PostSyncMessage (HWND_DESKTOP, iMsg, wParam, lParam);
        else
            return DesktopWinProc (hWnd, iMsg, wParam, lParam);
    }

    pMainWin = GetMainWindowPtrOfControl (hWnd);

    if (pMainWin->th != pthread_self())
        return PostSyncMessage (hWnd, iMsg, wParam, lParam);
    
    WndProc = GetWndProc(hWnd);

    return (*WndProc)(hWnd, iMsg, wParam, lParam);

}

int GUIAPI SendNotifyMessage(HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
//    PMAINWIN pMainWin;
    PMSGQUEUE pMsgQueue;
    PQMSG pqmsg;
    int sem_value;

    if (hWnd == HWND_INVALID) return -1;

    if( !(pMsgQueue = GetMsgQueue(hWnd)) ) return ERR_INV_HWND;

/*
    if (hWnd == HWND_DESKTOP) {
        if (pthread_self() == __mg_desktop)
            return DesktopWinProc (hWnd, iMsg, wParam, lParam);
    }
    else {
        pMainWin = GetMainWindowPtrOfControl (hWnd);
        if (pMainWin->th == pthread_self()) {
            WNDPROC WndProc;
    
            WndProc = GetWndProc(hWnd);

            return (*WndProc)(hWnd, iMsg, wParam, lParam);
        }
    }
*/
    
    pqmsg = QMSGAlloc();

    pthread_mutex_lock (&pMsgQueue->lock);

    // queue the sync message.
    pqmsg->Msg.hwnd = hWnd;
    pqmsg->Msg.message = iMsg;
    pqmsg->Msg.wParam = wParam;
    pqmsg->Msg.lParam = lParam;
    pqmsg->next = NULL;

    if (pMsgQueue->pFirstNotifyMsg == NULL) {
        pMsgQueue->pFirstNotifyMsg = pMsgQueue->pLastNotifyMsg = pqmsg;
    }
    else {
        pMsgQueue->pLastNotifyMsg->next = pqmsg;
        pMsgQueue->pLastNotifyMsg = pqmsg;
    }

    pMsgQueue->dwState |= QS_NOTIFYMSG;

    pthread_mutex_unlock (&pMsgQueue->lock);

    // Signal that the msg queue contains one more element for reading
    sem_getvalue (&pMsgQueue->wait, &sem_value);
    if (sem_value <= 0)
        sem_post(&pMsgQueue->wait);

    return ERR_OK;
}

int GUIAPI SendAsyncMessage(HWND hWnd, int iMsg, WPARAM wParam, LPARAM lParam)
{
    WNDPROC WndProc;
    
    if (hWnd == HWND_INVALID) return -1;

    WndProc = GetWndProc(hWnd);

    return (*WndProc)(hWnd, iMsg, wParam, lParam);

}

#ifdef _TRACE_MSG

int GUIAPI DispatchMessage(PMSG pMsg)
{
    WNDPROC WndProc;
    PSYNCMSG pSyncMsg;
    int iRet;

    uHALr_printf( "Message, %s: hWnd: %x, wP: %x, lP: %lx. %s\n",
        Message2Str (pMsg->message),
        pMsg->hwnd,
        pMsg->wParam,
        pMsg->lParam,
        pMsg->pAdd?"Sync":"Normal");

    if (pMsg->hwnd == HWND_INVALID) {
        if (pMsg->pAdd) {
            pSyncMsg = (PSYNCMSG)pMsg->pAdd;
            pSyncMsg->retval = HWND_INVALID;
            sem_post(&pSyncMsg->sem_handle);
        }
        
        uHALr_printf( "Message have been thrown away: %s\n",
            Message2Str (pMsg->message));

        return HWND_INVALID;
    }

    WndProc = GetWndProc (pMsg->hwnd);

    if ( pMsg->pAdd ) // this is a sync message.
    {
         pSyncMsg = (PSYNCMSG)pMsg->pAdd;
         pSyncMsg->retval = (*WndProc)
                   (pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
         sem_post(&pSyncMsg->sem_handle);
         iRet = pSyncMsg->retval;
    }
    else
        iRet = (*WndProc)(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);

    uHALr_printf( "Message, %s done, return value: %x\n",
        Message2Str (pMsg->message),
        iRet);

    return iRet;
}

#else

int GUIAPI DispatchMessage(PMSG pMsg)
{
    WNDPROC WndProc;
    PSYNCMSG pSyncMsg;

    if (pMsg->hwnd == HWND_INVALID) {
        if (pMsg->pAdd) {
            pSyncMsg = (PSYNCMSG)pMsg->pAdd;
            pSyncMsg->retval = HWND_INVALID;
            sem_post(&pSyncMsg->sem_handle);
        }
        return HWND_INVALID;
    }

    WndProc = GetWndProc(pMsg->hwnd);

    if ( pMsg->pAdd ) // this is a sync message.
    {
        pSyncMsg = (PSYNCMSG)pMsg->pAdd;
        pSyncMsg->retval = (*WndProc)
                   (pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
        sem_post(&pSyncMsg->sem_handle);
        return pSyncMsg->retval;
    }

    return (*WndProc)(pMsg->hwnd, pMsg->message, pMsg->wParam, pMsg->lParam);
}

#endif /* !TRACE_MSG */

#endif /* !LITE_VERSION */
