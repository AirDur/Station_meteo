#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)

/* 
 * Operations for server clipping rects information
 */

inline void scr_make_signature (void)
{
    SHAREDRES_SCRINFO.signature ++;
}

int scr_add_clip_rect (const RECT* rc)
{
    int i;
    RECT* crcs;
    
    crcs = SHAREDRES_SCRINFO.clip_rects;

    if (SHAREDRES_SCRINFO.nr_crc >= MAX_SRV_CLIP_RECTS)
        return -1;

    lock_scr_sem ();

    for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
        if (IsRectEmpty (crcs + i)) {
            crcs [i] = *rc;
            break;
        }
    }
    SHAREDRES_SCRINFO.nr_crc ++;
    scr_make_signature ();

    unlock_scr_sem ();

    return i;
}

int scr_del_clip_rect (int idx)
{
    RECT dirty;

    if (idx >= MAX_SRV_CLIP_RECTS || idx < 0)
        return -1;

    lock_scr_sem ();

    dirty = SHAREDRES_SCRINFO.clip_rects [idx];
    SetRectEmpty (SHAREDRES_SCRINFO.clip_rects + idx);
    SHAREDRES_SCRINFO.nr_crc --;
    scr_make_signature ();

    unlock_scr_sem ();

    UpdateTopmostLayer (&dirty);

    return 0;
}

#endif

void TerminateDesktop (void)
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    TerminateSharedSysRes ();
#else
    TerminateSystemRes ();
#endif

    DestroyFreeClipRectList (&sg_FreeClipRectList);
    DestroyFreeClipRectList (&sg_FreeInvRectList);
}

PGCRINFO GetGCRgnInfo(HWND hWnd)
{
    if (hWnd == HWND_DESKTOP)
        return &sg_ScrGCRInfo;

    return ((PMAINWIN)hWnd)->pGCRInfo;
}

inline void DesktopSetActiveWindow (PMAINWIN pWin)
{
#ifndef _LITE_VERSION
    if (__mg_ime_wnd)
        SendNotifyMessage (__mg_ime_wnd, MSG_IME_SETTARGET, (WPARAM)pWin, 0);
#endif

    __mg_active_mainwnd = pWin;
}

static HWND DesktopSetCapture(HWND hwnd)
{
    HWND hTemp;

    hTemp = __mg_capture_wnd;
    __mg_capture_wnd = hwnd;

    return hTemp;
}

static void dskScreenToClient (PMAINWIN pWin, const RECT* rcScreen, RECT* rcClient)
{
    PCONTROL pParent;

    rcClient->top = rcScreen->top - pWin->ct;
    rcClient->left = rcScreen->left - pWin->cl;
    rcClient->right = rcScreen->right - pWin->cl;
    rcClient->bottom = rcScreen->bottom - pWin->ct;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcClient->top -= pParent->ct;
        rcClient->left -= pParent->cl;
        rcClient->right -= pParent->cl;
        rcClient->bottom -= pParent->ct;
    }
}

static void dskScreenToWindow (PMAINWIN pWin, const RECT* rcScreen, RECT* rcWindow)
{
    PCONTROL pParent;

    rcWindow->top = rcScreen->top - pWin->top;
    rcWindow->left = rcScreen->left - pWin->left;
    rcWindow->right = rcScreen->right - pWin->left;
    rcWindow->bottom = rcScreen->bottom - pWin->top;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcWindow->top -= pParent->ct;
        rcWindow->left -= pParent->cl;
        rcWindow->right -= pParent->cl;
        rcWindow->bottom -= pParent->ct;
    }
}

static void dskClientToScreen (PMAINWIN pWin, const RECT* rcClient, RECT* rcScreen)
{
    PCONTROL pParent;

    rcScreen->top = rcClient->top + pWin->ct;
    rcScreen->left = rcClient->left + pWin->cl;
    rcScreen->right = rcClient->right + pWin->cl;
    rcScreen->bottom = rcClient->bottom + pWin->ct;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcScreen->top += pParent->ct;
        rcScreen->left += pParent->cl;
        rcScreen->right += pParent->cl;
        rcScreen->bottom += pParent->ct;
    }
}

#if 0
static void dskWindowToScreen (PMAINWIN pWin, const RECT* rcWindow, RECT* rcScreen)
{
    PCONTROL pParent;

    rcScreen->top = rcWindow->top + pWin->top;
    rcScreen->left = rcWindow->left + pWin->left;
    rcScreen->right = rcWindow->right + pWin->left;
    rcScreen->bottom = rcWindow->bottom + pWin->top;

    pParent = (PCONTROL) pWin;
    while ((pParent = pParent->pParent)) {
        rcScreen->top += pParent->ct;
        rcScreen->left += pParent->cl;
        rcScreen->right += pParent->cl;
        rcScreen->bottom += pParent->ct;
    }
}
#endif

static void dskGetWindowRectInScreen (PMAINWIN pWin, RECT* prc)
{
    PCONTROL pParent;
    PCONTROL pCtrl;

    pParent = (PCONTROL)pWin;
    pCtrl = (PCONTROL)pWin;

    prc->left = pCtrl->left;
    prc->top  = pCtrl->top;
    prc->right = pCtrl->right;
    prc->bottom = pCtrl->bottom;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

static void dskGetClientRectInScreen (PMAINWIN pWin, RECT* prc)
{
    PCONTROL pCtrl;
    PCONTROL pParent;
    pParent = (PCONTROL) pWin;
    pCtrl = (PCONTROL) pWin;

    prc->left = pCtrl->cl;
    prc->top  = pCtrl->ct;
    prc->right = pCtrl->cr;
    prc->bottom = pCtrl->cb;
    while ((pParent = pParent->pParent)) {
        prc->left += pParent->cl;
        prc->top  += pParent->ct;
        prc->right += pParent->cl;
        prc->bottom += pParent->ct;
    }
}

/************************* ZOrder operation **********************************/
static void InitZOrderInfo(PZORDERINFO pZOrderInfo, HWND hHost)
{
    pZOrderInfo->nNumber = 0;
    pZOrderInfo->hWnd = hHost;
    pZOrderInfo->pTopMost = NULL;
}

// reset clip info of window
static void reset_window (PMAINWIN pWin, RECT* rcWin)
{
    PGCRINFO pGCRInfo;
    RECT rcTemp;

    pGCRInfo = pWin->pGCRInfo;
    IntersectRect (&rcTemp, rcWin, &g_rcDesktop);
    SetClipRgn (&pGCRInfo->crgn, &rcTemp);
}

inline void start_clip_window (PMAINWIN pWin)
{
#ifndef _LITE_VERSION
    pthread_mutex_lock (&pWin->pGCRInfo->lock);
#endif
}

inline static void end_clip_window (PMAINWIN pWin)
{
    pWin->pGCRInfo->age ++;
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&pWin->pGCRInfo->lock);
#endif
}

// clip by all windows in specified zorder
// call start_clip_window and end_clip_window.
static void clip_by_windows (ZORDERINFO* zorder, PMAINWIN pWin)
{
    PZORDERNODE pNode;
    PMAINWIN pTemp;
    RECT rcTemp;
    
    pNode = zorder->pTopMost;
    while (pNode)
    {
        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            SubtractClipRect (&pWin->pGCRInfo->crgn, &rcTemp);
        }

        pNode = pNode->pNext;
    }
}

// clip all windows in specified zorder
static void clip_windows (ZORDERINFO* zorder, RECT* rcWin)
{
    PZORDERNODE pNode;
    PGCRINFO pGCRInfo;
    
    pNode = zorder->pTopMost;
    while (pNode)
    {
        if (((PMAINWIN)(pNode->hWnd))->dwStyle & WS_VISIBLE) {
            pGCRInfo = ((PMAINWIN)(pNode->hWnd))->pGCRInfo;
        
#ifndef _LITE_VERSION
            pthread_mutex_lock (&pGCRInfo->lock);
#endif
            SubtractClipRect (&pGCRInfo->crgn, rcWin);
            pGCRInfo->age ++;
#ifndef _LITE_VERSION
            pthread_mutex_unlock (&pGCRInfo->lock);
#endif
        }

        pNode = pNode->pNext;
    }
}

// clip desktop
static void clip_desktop (RECT* rcWin)
{
#ifndef _LITE_VERSION
    pthread_mutex_lock (&sg_ScrGCRInfo.lock);
#endif
    SubtractClipRect (&sg_ScrGCRInfo.crgn, rcWin);
    sg_ScrGCRInfo.age ++;
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&sg_ScrGCRInfo.lock);
#endif
}

// clip all windows under this window.
static void clip_windows_under_this (ZORDERINFO* zorder, PMAINWIN pWin, RECT* rcWin)
{
    PZORDERNODE pNode;
    PGCRINFO pGCRInfo;
    
    pNode = zorder->pTopMost;
    while (pNode->hWnd != (HWND)pWin)
        pNode = pNode->pNext;
    pNode = pNode->pNext;

    while (pNode)
    {
        if (((PMAINWIN)(pNode->hWnd))->dwStyle & WS_VISIBLE) {
            pGCRInfo = ((PMAINWIN)(pNode->hWnd))->pGCRInfo;
        
#ifndef _LITE_VERSION
            pthread_mutex_lock (&pGCRInfo->lock);
#endif
            SubtractClipRect (&pGCRInfo->crgn, rcWin);
            pGCRInfo->age ++;
#ifndef _LITE_VERSION
            pthread_mutex_unlock (&pGCRInfo->lock);
#endif
        }

        pNode = pNode->pNext;
    }
}

static void clip_by_all_above_this (ZORDERINFO* zorder, PMAINWIN pWin)
{
    PZORDERNODE pNode;
    PGCRINFO pGCRInfo;
    PMAINWIN pTemp;
    RECT rcTemp;
    
    pGCRInfo = pWin->pGCRInfo;
    pNode = zorder->pTopMost;
    while (pNode)
    {
        if (pNode->hWnd == (HWND)pWin)
            break;
            
        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            SubtractClipRect (&pGCRInfo->crgn, &rcTemp);
        }

        pNode = pNode->pNext;
    }
}

// When show an invisible new main window, 
// call this function to update all other windows' GCRInfo.
//
// Window functions which lead to calling this function:
// ShowWindow: show an invisible main window with a SW_SHOW parameter.
// 
// this main window is a normal main window.
static void dskUpdateGCRInfoOnShowNewMainWin (MAINWIN* pWin)
{
    RECT rcWin;

    dskGetWindowRectInScreen (pWin, &rcWin);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        pWin->scr_idx = scr_add_clip_rect (&rcWin);
#endif

    start_clip_window (pWin);
    clip_by_windows (&sg_TopMostWinZOrder, pWin);
    end_clip_window (pWin);

    clip_windows_under_this (&sg_MainWinZOrder, pWin, &rcWin);
    clip_desktop (&rcWin);
}

// this main window is a top most window.
static void dskUpdateGCRInfoOnShowNewMainWinEx(MAINWIN* pWin)
{
    RECT rcWin;
    
    dskGetWindowRectInScreen (pWin, &rcWin);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        pWin->scr_idx = scr_add_clip_rect (&rcWin);
#endif

    clip_windows_under_this (&sg_TopMostWinZOrder, pWin, &rcWin);
    clip_windows (&sg_MainWinZOrder, &rcWin);
    clip_desktop (&rcWin);
}

// When show a main window, all main windows which are covered by
// this showing main window will be updated.
// 
// Window functions which lead to calling this function:
// ShowWindow: show an invisible main window with a SW_SHOW parameter.
//
// this is a normal main window.
static void dskUpdateGCRInfoOnShowMainWin (MAINWIN* pWin)
{
    RECT rcWin;
    
    dskGetWindowRectInScreen (pWin, &rcWin);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        pWin->scr_idx = scr_add_clip_rect (&rcWin);
#endif

    start_clip_window (pWin);
    reset_window (pWin, &rcWin);
    clip_by_windows (&sg_TopMostWinZOrder, pWin);
    clip_by_all_above_this (&sg_MainWinZOrder, pWin);
    end_clip_window (pWin);

    clip_windows_under_this (&sg_MainWinZOrder, pWin, &rcWin);

    clip_desktop (&rcWin);
}

// this window is a main window with WS_EX style
static void dskUpdateGCRInfoOnShowMainWinEx(MAINWIN* pWin)
{
    RECT rcWin;
    
    // update this showing window's clip region info.
    dskGetWindowRectInScreen (pWin, &rcWin);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        pWin->scr_idx = scr_add_clip_rect (&rcWin);
#endif

    start_clip_window (pWin);
    reset_window (pWin, &rcWin);
    clip_by_all_above_this (&sg_TopMostWinZOrder, pWin);
    end_clip_window (pWin);
    
    clip_windows_under_this (&sg_TopMostWinZOrder, pWin, &rcWin);
    clip_windows (&sg_MainWinZOrder, &rcWin);
    clip_desktop (&rcWin);
}

static BOOL dskDoesCoverOther (PMAINWIN pWin1, PMAINWIN pWin2)
{
    PZORDERNODE pNode;

    if (pWin1 == NULL) return FALSE;

    if (pWin2 == NULL) return TRUE;

    pNode = sg_MainWinZOrder.pTopMost;
    while(pNode)
    {
        if (pNode->hWnd == (HWND)pWin1) return TRUE;
        if (pNode->hWnd == (HWND)pWin2) return FALSE;
        
        pNode = pNode->pNext;
    }

    return FALSE;
}

static BOOL dskIsTopMost (PMAINWIN pWin)
{
    if (pWin->dwExStyle & WS_EX_TOPMOST)
        return ((HWND)pWin == sg_TopMostWinZOrder.pTopMost->hWnd);
    else
        return ((HWND)pWin == sg_MainWinZOrder.pTopMost->hWnd);
}

static PMAINWIN dskChangActiveWindow (PMAINWIN pWin)
{
    PMAINWIN pOldActive;

    if (__mg_ime_wnd && (__mg_ime_wnd == (HWND)pWin))
        return __mg_active_mainwnd;

    if ((pWin == __mg_active_mainwnd) 
                    || (pWin && (pWin->dwExStyle & WS_EX_TOOLWINDOW)))
        return __mg_active_mainwnd;

    pOldActive = __mg_active_mainwnd;
    DesktopSetActiveWindow (pWin);

    if (pOldActive && (pOldActive->dwStyle & WS_VISIBLE)) {
        SendAsyncMessage ((HWND)pOldActive, MSG_NCACTIVATE, FALSE, 0);
        SendNotifyMessage ((HWND)pOldActive, MSG_ACTIVE, FALSE, 0);
        SendNotifyMessage ((HWND)pOldActive, MSG_KILLFOCUS, (HWND)pWin, 0);
    }

    if (pWin) {
        SendAsyncMessage ((HWND)pWin, MSG_NCACTIVATE, TRUE, 0);
        SendNotifyMessage ((HWND)pWin, MSG_ACTIVE, TRUE, 0);
        SendNotifyMessage ((HWND)pWin, MSG_SETFOCUS, (HWND)pOldActive, 0);
    }

    return pOldActive;
}

// This function called when a window was shown.
// return: the new active window.
// when return value is NULL, the active window not changed.
//
static PMAINWIN dskShowMainWindow(PMAINWIN pWin, BOOL bActive)
{
    if (pWin->dwStyle & WS_VISIBLE)
        return NULL;

    if (pWin->dwExStyle & WS_EX_TOPMOST)
        dskUpdateGCRInfoOnShowMainWinEx (pWin);
    else
        dskUpdateGCRInfoOnShowMainWin (pWin);

    pWin->dwStyle |= WS_VISIBLE;

    SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

    InvalidateRect ((HWND)pWin, NULL, TRUE);

    if (pWin->dwExStyle & WS_EX_TOPMOST)
        return dskChangActiveWindow (pWin);

    // if the showing window cover the current active window
    // set this window as the active one. 
    if (bActive && dskDoesCoverOther (pWin, __mg_active_mainwnd))
        return dskChangActiveWindow (pWin); 

    return NULL; 
}

// Main window hosting.
//
// Add new hosted main window.
//
void dskAddNewHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;
    
    pHosted->pNextHosted = NULL;

    head = pHosting->pFirstHosted;
    if (head)
    {
        while (head) {
            prev = head;
            head = head->pNextHosted;
        }

        prev->pNextHosted = pHosted;
    }
    else
        pHosting->pFirstHosted = pHosted;

    return;
}

// Main Window hosting.
//
// Remove a hosted main window.
//
void dskRemoveHostedMainWindow (PMAINWIN pHosting, PMAINWIN pHosted)
{
    PMAINWIN head, prev;
    
    head = pHosting->pFirstHosted;
    if (head == pHosted)
    {
        pHosting->pFirstHosted = head->pNextHosted;

        return;
    }

    while (head) {
        prev = head;
        head = head->pNextHosted;
            
        if (head == pHosted) {
            prev->pNextHosted = head->pNextHosted;
            return;
        }
    }

    return;
}

static void init_gcrinfo (PMAINWIN pWin)
{
    RECT rcWin, rcTemp;

    dskGetWindowRectInScreen (pWin, &rcWin);

#ifndef _LITE_VERSION
    pthread_mutex_init (&pWin->pGCRInfo->lock, NULL);
#endif
    pWin->pGCRInfo->age = 0;
    InitClipRgn (&pWin->pGCRInfo->crgn, &sg_FreeClipRectList);
    IntersectRect (&rcTemp, &rcWin, &g_rcDesktop);
    SetClipRgn (&pWin->pGCRInfo->crgn, &rcTemp);
}

static void init_invrgn (PMAINWIN pWin)
{
#ifndef _LITE_VERSION
    pthread_mutex_init (&pWin->InvRgn.lock, NULL);
#endif
    pWin->InvRgn.frozen = 0;
    InitClipRgn (&pWin->InvRgn.rgn, &sg_FreeInvRectList);
    EmptyClipRgn (&pWin->InvRgn.rgn);
}

static void add_new_window (ZORDERINFO* zorder, PZORDERNODE pNode)
{
    pNode->pNext = zorder->pTopMost;
    zorder->pTopMost = pNode;
    zorder->nNumber++;
}

static void remove_window (ZORDERINFO* zorder, PMAINWIN pWin)
{
    PZORDERNODE pNode, pTemp;

    pNode = zorder->pTopMost;
    if (pNode->hWnd == (HWND)pWin) {
        zorder->pTopMost = pNode->pNext;
    }
    else {
        while (pNode->pNext) {
            if (pNode->pNext->hWnd == (HWND)pWin) {
               pTemp = pNode->pNext->pNext;
               pNode->pNext = pTemp;
       
               break;
            }
            pNode = pNode->pNext;
        }
    }
    zorder->nNumber--;
}

// this funciton add the new main window to the main window list.
// if new main window is a visible window,
// this new main window becomes the active window and this function
// return the old main window.
// otherwise, return NULL.
static PMAINWIN dskAddNewMainWindow (PMAINWIN pWin, PZORDERNODE pNode)
{
    // Handle main window hosting.
    if (pWin->pHosting)
        dskAddNewHostedMainWindow (pWin->pHosting, pWin);
    
    // Init Global Clip Region info.
    init_gcrinfo (pWin);

    // Init Invalid Region info.
    init_invrgn (pWin);

    // Update Z Order info.
    pNode->hWnd = (HWND)pWin;
    if (pWin->dwExStyle & WS_EX_TOPMOST)
        add_new_window (&sg_TopMostWinZOrder, pNode);
    else
        add_new_window (&sg_MainWinZOrder, pNode);

    // show and active this main window.
    if ( pWin->dwStyle & WS_VISIBLE ) {

        if (pWin->dwExStyle & WS_EX_TOPMOST)
            dskUpdateGCRInfoOnShowNewMainWinEx (pWin);
        else
            dskUpdateGCRInfoOnShowNewMainWin (pWin);

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

        SendNotifyMessage ((HWND)pWin, MSG_SHOWWINDOW, SW_SHOWNORMAL, 0);

        InvalidateRect ((HWND)pWin, NULL, TRUE);

        return dskChangActiveWindow (pWin);
    }

    return NULL;
}

// This function defined in gui/window.c
BOOL wndInvalidateRect (HWND hWnd, const RECT* prc, BOOL bEraseBkgnd);

// this function called when a visible main win becomes as a top most window.
// this is a normal main window.
static BOOL update_on_change_topmost (ZORDERINFO* zorder, PMAINWIN pNew, RECT* rcNew)
{
    PGCRINFO pGCRInfo;
    PZORDERNODE pNode;
    RECT rcInter;
    RECT rcOther;
    BOOL inved;
    
    inved = FALSE;

    // clip all main windows which will covered by this new top most window
    pNode = zorder->pTopMost;
    while (pNode)
    {
        if ( pNode->hWnd == (HWND)pNew )
            break;

        if (((PMAINWIN)(pNode->hWnd))->dwStyle & WS_VISIBLE) {
        
            dskGetWindowRectInScreen ((PMAINWIN)pNode->hWnd, &rcOther);
            if (IntersectRect(&rcInter, rcNew, &rcOther)) {
                pGCRInfo = ((PMAINWIN)(pNode->hWnd))->pGCRInfo;
        
#ifndef _LITE_VERSION
                pthread_mutex_lock (&pGCRInfo->lock);
#endif
                SubtractClipRect (&pGCRInfo->crgn, rcNew);
                pGCRInfo->age ++;
#ifndef _LITE_VERSION
                pthread_mutex_unlock (&pGCRInfo->lock);
#endif

                // update this window's client area.
                dskScreenToClient (pNew, &rcInter, &rcInter);
                wndInvalidateRect ((HWND)pNew, &rcInter, TRUE);
                inved = TRUE;
            }
        }

        pNode = pNode->pNext;
    }

    return inved;
}

static void dskUpdateGCRInfoOnChangeTopMost(PMAINWIN pNew)
{
    RECT rcNew;

    dskGetWindowRectInScreen (pNew, &rcNew);

    if (update_on_change_topmost (&sg_MainWinZOrder, pNew, &rcNew));
        PostMessage ((HWND)pNew, MSG_PAINT, 0, 0);
}

// this is a top most window
static void dskUpdateGCRInfoOnChangeTopMostEx (PMAINWIN pNew)
{
    RECT rcNew;

    dskGetWindowRectInScreen (pNew, &rcNew);

    if (update_on_change_topmost (&sg_TopMostWinZOrder, pNew, &rcNew))
        PostMessage ((HWND)pNew, MSG_PAINT, 0, 0);
}

static void move_to_top (ZORDERINFO* zorder, PMAINWIN pWin)
{
    PZORDERNODE pNode, pTemp;

    pNode = zorder->pTopMost;
    if (pNode->hWnd != (HWND)pWin) {
        while (pNode->pNext) {
            if (pNode->pNext->hWnd == (HWND)pWin) {
               pTemp = pNode->pNext;
               pNode->pNext = pNode->pNext->pNext;

               pTemp->pNext = zorder->pTopMost;
               zorder->pTopMost = pTemp;

               break;
            }
            pNode = pNode->pNext;
        }
    }
}

static PMAINWIN dskHideMainWindow (PMAINWIN pWin);

static void dskSetPrimitiveChildren (PMAINWIN pWin, BOOL bSet)
{
    PMAINWIN pParent;
    
    pParent = (PMAINWIN) pWin->hParent;

    if (bSet) {
        while (pParent) {
            pParent->hPrimitive = (HWND)pWin;

            pWin = pParent;
            pParent = (PMAINWIN) pWin->hParent;
        }
    }
    else {
        while (pParent) {
            pParent->hPrimitive = 0;

            pWin = pParent;
            pParent = (PMAINWIN) pWin->hParent;
        }
    }
}

// this function called when a main window becomes a visible top most window.
// Functions which lead to call this function:
//      ShowWindow: show a invisible window with SW_SHOWNORMAL parameters.
//
// return: old active window.
//
static PMAINWIN dskMoveToTopMost (PMAINWIN pWin, int reason, LPARAM lParam)
{
    RECT rcWin;
    PMAINWIN pOldTopMost;

    if (!pWin) return __mg_active_mainwnd;

    if (pWin->dwExStyle & WS_EX_TOPMOST)
        pOldTopMost = (PMAINWIN) sg_TopMostWinZOrder.pTopMost->hWnd;
    else 
        pOldTopMost = (PMAINWIN) sg_MainWinZOrder.pTopMost->hWnd;

    if (pOldTopMost && (pOldTopMost->WinType == TYPE_CONTROL)
                    && (pOldTopMost->dwStyle & WS_VISIBLE)) {
        dskHideMainWindow (pOldTopMost);
        dskSetPrimitiveChildren (pOldTopMost, FALSE);
        SendNotifyMessage (pOldTopMost->hParent, 
                        MSG_CHILDHIDDEN, reason, lParam);
    }

    if (dskIsTopMost (pWin) && (pWin->dwStyle & WS_VISIBLE))
        return __mg_active_mainwnd;

    // update this main window's global clip region info.
    dskGetWindowRectInScreen (pWin, &rcWin);
    start_clip_window (pWin);
    reset_window (pWin, &rcWin);
    if (!(pWin->dwExStyle & WS_EX_TOPMOST))
        clip_by_windows (&sg_TopMostWinZOrder, pWin);
    end_clip_window (pWin);

    // activate this main window.
    if ( !(pWin->dwStyle & WS_VISIBLE) ) {
        // Update Z Order first.
        if (pWin->dwExStyle & WS_EX_TOPMOST)
            move_to_top (&sg_TopMostWinZOrder, pWin);
        else
            move_to_top (&sg_MainWinZOrder, pWin);

        if (pWin->dwExStyle & WS_EX_TOPMOST)
            dskUpdateGCRInfoOnShowNewMainWinEx (pWin);
        else
            dskUpdateGCRInfoOnShowNewMainWin (pWin);
        
        pWin->dwStyle |= WS_VISIBLE;

        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
    
        InvalidateRect ((HWND)pWin, NULL, TRUE);
    }
    else {
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);

        if (pWin->dwExStyle & WS_EX_TOPMOST)
            dskUpdateGCRInfoOnChangeTopMostEx (pWin);
        else
            dskUpdateGCRInfoOnChangeTopMost (pWin);

        // then update Z Order.
        if (pWin->dwExStyle & WS_EX_TOPMOST)
            move_to_top (&sg_TopMostWinZOrder, pWin);
        else
            move_to_top (&sg_MainWinZOrder, pWin);
    }

    if (reason != RCTM_SHOWCTRL)
        return dskChangActiveWindow (pWin);
    else
        return __mg_active_mainwnd;
}

static void recalc_windows_under_this (ZORDERINFO* zorder, PMAINWIN pWin, RECT* rcWin)
{
    PZORDERNODE pNode, pAffected;
    RECT rcTemp, rcInv;
    PMAINWIN pTemp;

    // where is the hidding main window?
    pNode = zorder->pTopMost;
    while (pNode->hWnd != (HWND)pWin)
        pNode = pNode->pNext;
    pAffected = pNode->pNext;

    while (pAffected){
        pTemp = (PMAINWIN)(pAffected->hWnd);

        if (pTemp->dwStyle & WS_VISIBLE) {

            start_clip_window (pTemp);
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            reset_window (pTemp, &rcTemp);
            if (zorder != &sg_TopMostWinZOrder) {
                clip_by_windows (&sg_TopMostWinZOrder, pTemp);
                clip_by_all_above_this (&sg_MainWinZOrder, pTemp);
            }
            else
                clip_by_all_above_this (&sg_TopMostWinZOrder, pTemp);
            end_clip_window (pTemp);

            if (IntersectRect (&rcTemp, rcWin, &rcTemp)) {
                dskScreenToWindow (pTemp, &rcTemp, &rcInv);
                SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                dskScreenToClient (pTemp, &rcTemp, &rcInv);
                InvalidateRect ((HWND)pTemp, &rcInv, TRUE);
            }
        }

        pAffected = pAffected->pNext;
    }
}

static void recalc_windows (ZORDERINFO* zorder, RECT* rcWin)
{
    PZORDERNODE pAffected;
    RECT rcTemp, rcInv;
    PMAINWIN pTemp;

    pAffected = zorder->pTopMost;
    while (pAffected){
        pTemp = (PMAINWIN)(pAffected->hWnd);

        if (pTemp->dwStyle & WS_VISIBLE) {

            start_clip_window (pTemp);
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            reset_window (pTemp, &rcTemp);
            clip_by_windows (&sg_TopMostWinZOrder, pTemp);
            clip_by_all_above_this (&sg_MainWinZOrder, pTemp);
            end_clip_window (pTemp);

            if (IntersectRect (&rcTemp, rcWin, &rcTemp)) {
                dskScreenToWindow (pTemp, &rcTemp, &rcInv);
                SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                dskScreenToClient (pTemp, &rcTemp, &rcInv);
                InvalidateRect ((HWND)pTemp, &rcInv, TRUE);
            }
        }

        pAffected = pAffected->pNext;
    }
}

static void recalc_desktop (PTRACKMENUINFO ptmi)
{
    PZORDERNODE pNode;
    RECT rcTemp;
    PMAINWIN pTemp;

    // update desktop's global clip region.
#ifndef _LITE_VERSION
    pthread_mutex_lock (&sg_ScrGCRInfo.lock);
#endif
    SetClipRgn (&sg_ScrGCRInfo.crgn, &g_rcDesktop);
    
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode) {
    
        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            SubtractClipRect (&sg_ScrGCRInfo.crgn, &rcTemp);
        }

        pNode = pNode->pNext;
    }
    
    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode) {
    
        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            SubtractClipRect (&sg_ScrGCRInfo.crgn, &rcTemp);
        }

        pNode = pNode->pNext;
    }

    while (ptmi) {
        SubtractClipRect (&sg_ScrGCRInfo.crgn, &ptmi->rc);
        ptmi = ptmi->next;
    }

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer && mgTopmostLayer) {
        MG_Client* client = mgTopmostLayer->cli_head;
        while (client) {
            SubtractClipRect (&sg_ScrGCRInfo.crgn, &client->rc);
            client = client->next;
        }
    }
#endif

    sg_ScrGCRInfo.age ++;
#ifndef _LITE_VERSION
    pthread_mutex_unlock (&sg_ScrGCRInfo.lock);
#endif
}

// When hide a main win, all main win which is covered by
// this hidding main win will be regenerated.
//
// Functions which lead to call this function:
//  ShowWindow: hide a visible window with SW_HIDE parameters.
//  DestroyWindow: destroy a visible window.
//
static void dskUpdateGCRInfoOnHideMainWin (MAINWIN* pWin)
{
    RECT rcWin, rcTemp;

    start_clip_window (pWin);
    pWin->dwStyle &= ~WS_VISIBLE;
    dskGetWindowRectInScreen (pWin, &rcWin);
    recalc_windows_under_this (&sg_MainWinZOrder, pWin, &rcWin);
    recalc_desktop (NULL);
    end_clip_window (pWin);

    IntersectRect (&rcTemp, &rcWin, &g_rcDesktop);
    DesktopWinProc (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, (LPARAM)(&rcTemp));

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        scr_del_clip_rect (pWin->scr_idx);
#endif

}

// this is a top most window
static void dskUpdateGCRInfoOnHideMainWinEx(MAINWIN* pWin)
{
    RECT rcWin, rcTemp;
    
    start_clip_window (pWin);
    pWin->dwStyle &= ~WS_VISIBLE;
    dskGetWindowRectInScreen (pWin, &rcWin);
    recalc_windows_under_this (&sg_TopMostWinZOrder, pWin, &rcWin);
    recalc_windows (&sg_MainWinZOrder, &rcWin);
    recalc_desktop (NULL);
    end_clip_window (pWin);

    IntersectRect (&rcTemp, &rcWin, &g_rcDesktop);
    DesktopWinProc (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, (LPARAM)(&rcTemp));

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        scr_del_clip_rect (pWin->scr_idx);
#endif
}

static PMAINWIN dskGetFirstActivableWindow (void)
{
    PZORDERNODE pNode;
    PMAINWIN pTemp;

    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode) {
    
        pTemp = (PMAINWIN)(pNode->hWnd);

        if ((pTemp->WinType != TYPE_CONTROL)
                && (pTemp->dwStyle & WS_VISIBLE)
                && !(pTemp->dwStyle & WS_DISABLED)
                && !(pTemp->dwExStyle & WS_EX_TOOLWINDOW))
            return pTemp;

        pNode = pNode->pNext;
    }
    
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode) {
    
        pTemp = (PMAINWIN)(pNode->hWnd);

        if ((pTemp->WinType != TYPE_CONTROL)
                && (pTemp->dwStyle & WS_VISIBLE)
                && !(pTemp->dwStyle & WS_DISABLED)
                && !(pTemp->dwExStyle & WS_EX_TOOLWINDOW))
            return pTemp;

        pNode = pNode->pNext;
    }

    return NULL;
}

static PMAINWIN dskGetNextVisibleMainWindow (PMAINWIN pWin)
{
    PZORDERNODE pNode;
    PMAINWIN pTemp;

    if (pWin)
        pNode = pWin->pZOrderNode->pNext;
    else
        pNode = sg_MainWinZOrder.pTopMost;

    while (pNode) {

        pTemp = (PMAINWIN)(pNode->hWnd);

        if ((pTemp->WinType != TYPE_CONTROL)
                && (pTemp->dwStyle & WS_VISIBLE) 
                && !(pTemp->dwStyle & WS_DISABLED))
            return pTemp;

        pNode = pNode->pNext;
    }
    
    return NULL;
}

static PMAINWIN dskHideMainWindow (PMAINWIN pWin)
{
    if (!(pWin->dwStyle & WS_VISIBLE))
        return NULL;

    // Update all affected windows' GCR info.
    if (pWin->dwExStyle & WS_EX_TOPMOST)
        dskUpdateGCRInfoOnHideMainWinEx (pWin);
    else
        dskUpdateGCRInfoOnHideMainWin (pWin);

    // if this is the avtive window,
    // must choose another window as the active one. 
    if (__mg_active_mainwnd == pWin)
        dskChangActiveWindow (dskGetFirstActivableWindow ());

    return __mg_active_mainwnd;
}

// When destroy a main win, all main win which is covered by
// this destroying main win will be redraw.
//
// Functions which lead to call this function:
//  DestroyWindow: destroy a visible window.
//
// return: the new active window.
static PMAINWIN dskRemoveMainWindow (PMAINWIN pWin)
{
    // Handle main window hosting.
    if (pWin->pHosting)
        dskRemoveHostedMainWindow (pWin->pHosting, pWin);

    // Update all affected windows' GCR info.
    if (pWin->dwStyle & WS_VISIBLE) {
        if (pWin->dwExStyle & WS_EX_TOPMOST)
            dskUpdateGCRInfoOnHideMainWinEx (pWin);
        else
            dskUpdateGCRInfoOnHideMainWin (pWin);

        if (__mg_active_mainwnd == pWin)
            dskChangActiveWindow (dskGetFirstActivableWindow ());

        if (__mg_active_mainwnd == pWin)
            __mg_active_mainwnd = NULL;
    }

    // Update window Z order list.
    if (pWin->dwExStyle & WS_EX_TOPMOST)
        remove_window (&sg_TopMostWinZOrder, pWin);
    else
        remove_window (&sg_MainWinZOrder, pWin);

    return __mg_active_mainwnd;
}

static void dskUpdateAllGCRInfoOnShowMenu (RECT* prc)
{
    clip_windows (&sg_TopMostWinZOrder, prc);
    clip_windows (&sg_MainWinZOrder, prc);

    clip_desktop (prc);
}

static void dskUpdateAllGCRInfoOnHideMenu (void)
{
    PZORDERNODE pAffected;
    RECT     rcTemp;
    PMAINWIN pTemp;
    PTRACKMENUINFO ptmi;
    
    pAffected = sg_TopMostWinZOrder.pTopMost;
    while (pAffected){

        pTemp = (PMAINWIN)(pAffected->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {

            start_clip_window (pTemp);
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            reset_window (pTemp, &rcTemp);
            clip_by_all_above_this (&sg_TopMostWinZOrder, pTemp);
            end_clip_window (pTemp);

            ptmi = __mg_ptmi;
            while (ptmi) {
                SubtractClipRect (&pTemp->pGCRInfo->crgn, &ptmi->rc);
                ptmi = ptmi->next;
            }

        }

       pAffected = pAffected->pNext;
    }
    
    pAffected = sg_MainWinZOrder.pTopMost;
    while (pAffected){

        pTemp = (PMAINWIN)(pAffected->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {

            start_clip_window (pTemp);
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            reset_window (pTemp, &rcTemp);
            clip_by_windows (&sg_TopMostWinZOrder, pTemp);
            clip_by_all_above_this (&sg_MainWinZOrder, pTemp);
            end_clip_window (pTemp);

            ptmi = __mg_ptmi;
            while (ptmi) {
                SubtractClipRect (&pTemp->pGCRInfo->crgn, &ptmi->rc);
                ptmi = ptmi->next;
            }
        }

        pAffected = pAffected->pNext;
    }

    recalc_desktop (__mg_ptmi);
}

// call back proc of tracking menu.
// defined in Menu module.
int PopupMenuTrackProc (PTRACKMENUINFO ptmi, int message, WPARAM wParam, LPARAM lParam);

static int dskTrackPopupMenu(PTRACKMENUINFO ptmi)
{
    PTRACKMENUINFO plast;
    
    if (__mg_ptmi) {
        plast = __mg_ptmi;
        while (plast->next) {
            plast = plast->next;
        }

        plast->next = ptmi;
        ptmi->prev = plast;
        ptmi->next = NULL;
    }
    else {
        __mg_ptmi = ptmi;
        ptmi->next = NULL;
        ptmi->prev = NULL;
    }

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        ptmi->scr_idx = scr_add_clip_rect (&ptmi->rc);
#endif

    PopupMenuTrackProc (ptmi, MSG_INITMENU, 0, 0);

    // Update all main windows' GCR info.
    dskUpdateAllGCRInfoOnShowMenu (&ptmi->rc);

    PopupMenuTrackProc (ptmi, MSG_SHOWMENU, 0, 0);

    return 0;
}

static int dskEndTrackMenu (PTRACKMENUINFO ptmi)
{
    PZORDERNODE pNode;
    PMAINWIN pTemp;
    PTRACKMENUINFO plast;
    RECT rcInvalid, rcTemp;
    
    if (__mg_ptmi == ptmi) {
        __mg_ptmi = NULL;
    }
    else {
        plast = __mg_ptmi;
        while (plast->next) {
            plast = plast->next;
        }
        plast->prev->next = NULL;
    }

    // Update all main windows' GCR info.
    dskUpdateAllGCRInfoOnHideMenu ();

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
        scr_del_clip_rect (ptmi->scr_idx);
#endif

    PopupMenuTrackProc (ptmi, MSG_HIDEMENU, 0, 0);

    // Invalidate rect of affected main window.
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode) {

        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {
            
            dskGetWindowRectInScreen (pTemp, &rcTemp);
            if (IntersectRect (&rcInvalid, &ptmi->rc, &rcTemp)) {
                dskScreenToClient (pTemp, &rcInvalid, &rcInvalid);
                InvalidateRect ((HWND)pTemp, &rcInvalid, FALSE);
            }
        }

        pNode = pNode->pNext;
    }
    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode) {
    
        pTemp = (PMAINWIN)(pNode->hWnd);

        pTemp = (PMAINWIN)(pNode->hWnd);
        if (pTemp->dwStyle & WS_VISIBLE) {

            dskGetWindowRectInScreen (pTemp, &rcTemp);
            if (IntersectRect (&rcInvalid, &ptmi->rc, &rcTemp)) {
                dskScreenToClient (pTemp, &rcInvalid, &rcInvalid);
                InvalidateRect ((HWND)pTemp, &rcInvalid, FALSE);
            }
        }

        pNode = pNode->pNext;
    }

    PopupMenuTrackProc (ptmi, MSG_ENDTRACKMENU, 0, 0);

    return __mg_ptmi == ptmi;
}

static BOOL dskForceCloseMenu (void)
{
    if (__mg_ptmi == NULL) return FALSE;

    SendNotifyMessage (__mg_ptmi->hwnd, MSG_DEACTIVEMENU, 
                       (WPARAM)__mg_ptmi->pmb, (LPARAM)__mg_ptmi->pmi);
                       
    PopupMenuTrackProc (__mg_ptmi, MSG_CLOSEMENU, 0, 0);

    __mg_ptmi = NULL;

    // Update all main windows' GCR info.
    dskUpdateAllGCRInfoOnHideMenu ();

    return TRUE;
}

static BOOL dskUpdateOldMainWindow (PMAINWIN pWin, 
        const RECT* prcInv, const RECT* prcWin, const RECT* prcClient)
{
    RECT rcInter;

    if (IntersectRect (&rcInter, prcInv, prcWin)) {

        rcInter.left -= prcWin->left;
        rcInter.top  -= prcWin->top;
        rcInter.right -= prcWin->left;
        rcInter.bottom -= prcWin->top;
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, (LPARAM)(&rcInter));

        if (IntersectRect (&rcInter, prcInv, prcClient)) {
            rcInter.left -= prcClient->left;
            rcInter.top  -= prcClient->top;
            rcInter.right -= prcClient->left;
            rcInter.bottom -= prcClient->top;
            InvalidateRect ((HWND)pWin, &rcInter, TRUE);

            return TRUE;
        }
    }

    return FALSE;
}

static BOOL dskUpdateWindow (PMAINWIN pWin, 
        const RECT* prcInv, const RECT* prcWin, const RECT* prcClient)
{
    RECT rcInter, rcInv;

    if (IntersectRect (&rcInter, prcInv, prcWin)) {

        dskScreenToWindow (pWin, &rcInter, &rcInv);
        SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, (LPARAM)(&rcInv));

        if (IntersectRect (&rcInter, prcInv, prcClient)) {
            dskScreenToClient (pWin, &rcInter, &rcInv);
            InvalidateRect ((HWND)pWin, &rcInv, TRUE);

            return TRUE;
        }
    }

    return FALSE;
}

static int dskScrollMainWindow (PMAINWIN pWin, PSCROLLWINDOWINFO pswi)
{
    HDC hdc;
    RECT rcClient, rcScreen, rcInvalid, rcScroll;
    BOOL inved;
    PCLIPRECT pcrc;

    inved = FALSE;
    rcClient.left = 0;
    rcClient.top  = 0;
    rcClient.right = pWin->cr - pWin->cl;
    rcClient.bottom = pWin->cb - pWin->ct;

    if (pswi->rc1)
        IntersectRect (&rcScroll, pswi->rc1, &rcClient);
    else
        rcScroll = rcClient;

    if (pswi->rc2)
        IntersectRect (&rcScroll, pswi->rc2, &rcScroll);
        
    dskClientToScreen (pWin, &rcScroll, &rcScreen);

    hdc = GetClientDC ((HWND)pWin);

    pcrc = GetGCRgnInfo ((HWND)pWin)->crgn.head;
    while (pcrc) {
        RECT rcMove;

        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);
        if (!IntersectRect (&rcMove, &rcMove, &rcScroll)) {
            pcrc = pcrc->next;
            continue;
        }

        SelectClipRect (hdc, &rcMove);

        BitBlt (hdc, rcMove.left, rcMove.top, 
            rcMove.right - rcMove.left,
            rcMove.bottom - rcMove.top,
            hdc, pswi->iOffx + rcMove.left, pswi->iOffy + rcMove.top, 0);

        pcrc = pcrc->next;
    }
    ReleaseDC (hdc);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    // update region covered by the server clipping rects.
    if (!mgIsServer) {
        int i;
        RECT* crcs;
        
        crcs = SHAREDRES_SCRINFO.clip_rects;

        for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
            if (!IsRectEmpty (crcs + i)) {
                if (IntersectRect (&rcInvalid, crcs + i, &rcScreen)) {
                    dskScreenToClient (pWin, &rcInvalid, &rcInvalid);
                    OffsetRect (&rcInvalid, pswi->iOffx, pswi->iOffy);
                    wndInvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
                    inved = TRUE;
                }
            }
        }
    }
#endif

    pcrc = GetGCRgnInfo ((HWND)pWin)->crgn.head;
    while (pcrc) {
        RECT rcMove;
        if (!IntersectRect (&rcMove, &pcrc->rc, &rcScreen)) {
            pcrc = pcrc->next;
            continue;
        }

        dskScreenToClient (pWin, &rcMove, &rcMove);

        rcInvalid = rcMove;
        if (pswi->iOffx < 0) {
            rcInvalid.left = rcInvalid.right + pswi->iOffx;
            wndInvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }
        else if (pswi->iOffx > 0) {
            rcInvalid.right = rcInvalid.left + pswi->iOffx;
            wndInvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }
    
        rcInvalid = rcMove;
        if (pswi->iOffy < 0) {
            rcInvalid.top = rcInvalid.bottom + pswi->iOffy;
            wndInvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }
        else if (pswi->iOffy > 0) {
            rcInvalid.bottom = rcInvalid.top + pswi->iOffy;
            wndInvalidateRect ((HWND)pWin, &rcInvalid, TRUE);
            inved = TRUE;
        }

        pcrc = pcrc->next;
    }

    if (inved)
        PostMessage ((HWND)pWin, MSG_PAINT, 0, 0);

    return 0;
}

static void dskMoveMainWindow (PMAINWIN pWin, const RECT* prcExpect)
{
    RECT oldWinRect, oldClientRect, src;       // old window rect.
    RECT rc[4], rcInv[4];       // invalid rects.
    RECT rcInter, rcTemp, rcTempClient;
    int nCount, nInvCount; // number of invalid rects.
    int i;
    PZORDERNODE pNode;
    PMAINWIN pTemp;
    HDC hdc;

    memcpy (&src, &pWin->left, sizeof (RECT));
    memcpy (&oldClientRect, &pWin->cl, sizeof (RECT));
    oldWinRect = src;

    // calculate invalid rect of other windows
    nCount = SubtractRect (rc, &src, prcExpect);

    // calculate invalid rect of moving window
    nInvCount = SubtractRect (rcInv, &src, &g_rcScr);

    // Update all affected windows' GCR info.
    SendAsyncMessage ((HWND)pWin, MSG_CHANGESIZE, (WPARAM)(prcExpect), 0);
    dskUpdateAllGCRInfoOnHideMenu ();

    // Move window content.
    hdc = GetDC ((HWND)pWin);
    BitBlt (HDC_SCREEN, oldWinRect.left, oldWinRect.top, 
                    RECTW (oldWinRect), RECTH (oldWinRect),
                    hdc, 0, 0, 0);
    ReleaseDC (hdc);

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer) {
        scr_del_clip_rect (pWin->scr_idx);
        dskGetWindowRectInScreen (pWin, &rcTemp);
        pWin->scr_idx = scr_add_clip_rect (&rcTemp);
    }
#endif

    // when old window rect out of date.
    for (i=0; i<nInvCount; i++)
        dskUpdateOldMainWindow (pWin, rcInv + i, &oldWinRect, &oldClientRect);
    
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    // update region covered by the server clipping rects.
    if (!mgIsServer) {
        int i;
        RECT* crcs;
        
        crcs = SHAREDRES_SCRINFO.clip_rects;

        for (i = 0; i < MAX_SRV_CLIP_RECTS; i++) {
            if (!IsRectEmpty (crcs + i))
                dskUpdateOldMainWindow (pWin, crcs + i, &oldWinRect, &oldClientRect);
        }
    }
#endif

    // update region covered by the topmost windows.
    if (!(pWin->dwExStyle & WS_EX_TOPMOST)) {
        pNode = sg_TopMostWinZOrder.pTopMost;
        while (pNode) {

            pTemp = (PMAINWIN)(pNode->hWnd);
            if (pTemp != pWin && pTemp->dwStyle & WS_VISIBLE) {
                dskGetWindowRectInScreen (pTemp, &rcTemp);
                dskUpdateOldMainWindow (pWin, &rcTemp, &oldWinRect, &oldClientRect);
            }

            pNode = pNode->pNext;
        }

    }
   
    // when other windows run out of date. 
    for (i=0; i<nCount; i++) {
        if (pWin->dwExStyle & WS_EX_TOPMOST) {
            pNode = sg_TopMostWinZOrder.pTopMost;
            while (pNode) {

                pTemp = (PMAINWIN)(pNode->hWnd);
                if (pTemp != pWin && pTemp->dwStyle & WS_VISIBLE) {
                    dskGetWindowRectInScreen (pTemp, &rcTemp);
                    dskGetClientRectInScreen (pTemp, &rcTempClient);
                    dskUpdateWindow (pTemp, rc + i, &rcTemp, &rcTempClient);
                }

                pNode = pNode->pNext;
            }
        }
        
        pNode = sg_MainWinZOrder.pTopMost;
        while (pNode) {
    
            pTemp = (PMAINWIN)(pNode->hWnd);
            if (pTemp != pWin && pTemp->dwStyle & WS_VISIBLE) {
                dskGetWindowRectInScreen (pTemp, &rcTemp);
                dskGetClientRectInScreen (pTemp, &rcTempClient);
                dskUpdateWindow (pTemp, rc + i, &rcTemp, &rcTempClient);
            }

            pNode = pNode->pNext;
        }

        IntersectRect (&rcInter, rc + i, &g_rcDesktop);
        DesktopWinProc (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, (LPARAM)(&rcInter));
    }
    
    {
        MSG msg;
        // remove any mouse moving messages in mesasge queue.
        while (PeekPostMessage (&msg, (HWND)pWin, 
                    MSG_MOUSEMOVE, MSG_MOUSEMOVE, PM_REMOVE))
        { };
    }
}

/*********************** Hook support ****************************************/
static HOOKINFO keyhook;
static HOOKINFO mousehook;

static MSGHOOK dskRegisterKeyHook (void* context, MSGHOOK hook)
{
    MSGHOOK old_hook;

    old_hook = keyhook.hook;
    keyhook.context = context;
    keyhook.hook = hook;
    return old_hook;
}

static MSGHOOK dskRegisterMouseHook (void* context, MSGHOOK hook)
{
    MSGHOOK old_hook;

    old_hook = mousehook.hook;
    mousehook.context = context;
    mousehook.hook = hook;
    return old_hook;
}

static int dskHandleKeyHooks (HWND dst_wnd, int message, WPARAM wParam, LPARAM lParam)
{
    int ret;
    
    ret = HOOK_GOON;

    if (keyhook.hook) {
        ret = keyhook.hook (keyhook.context, dst_wnd, message, wParam, lParam);
    }

    return ret;
}

static int dskHandleMouseHooks (HWND dst_wnd, int message, WPARAM wParam, LPARAM lParam)
{
    int ret;
    
    ret = HOOK_GOON;

    if (mousehook.hook) {
        ret = mousehook.hook (mousehook.context, dst_wnd, message, wParam, lParam);
    }

    return ret;
}

/*********************** Desktop window support ******************************/
PMAINWIN GetMainWindowPtrUnderPoint (int x, int y)
{
    PZORDERNODE pNode;
    PMAINWIN pTemp;
    RECT rcTemp;

    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode)
    {
        pTemp = (PMAINWIN)pNode->hWnd;
        dskGetWindowRectInScreen (pTemp, &rcTemp);
        if (PtInRect (&rcTemp, x, y) 
                && (pTemp->dwStyle & WS_VISIBLE)) {
                return pTemp;
        }

        pNode = pNode->pNext; 
    }

    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode)
    {
        pTemp = (PMAINWIN)pNode->hWnd;
        dskGetWindowRectInScreen (pTemp, &rcTemp);
        if (PtInRect (&rcTemp, x, y) 
                && (pTemp->dwStyle & WS_VISIBLE)) {
                return pTemp;
        }
            
        pNode = pNode->pNext; 
    }
    return NULL;
}

static int HandleSpecialKey (int scancode)
{
    switch (scancode) {
    case SCANCODE_BACKSPACE:
        TerminateGUI (-1);
#ifndef __NOUNIX__
        exit (2);
#endif
        break;
    }

    return 0;
}

void GUIAPI ExitGUISafely (int exitcode)
{
    TerminateGUI ((exitcode > 0) ? -exitcode : exitcode);
#ifndef __NOUNIX__
    exit (2);
#endif
}

static int KeyMessageHandler (int message, int scancode, DWORD status)
{
    PMAINWIN pNextWin;
    static int mg_altdown;
    static int mg_modal;
    
    mg_altdown = 0;
    mg_modal = 0;

    if ((message == MSG_KEYDOWN) && (status & KS_ALT) && (status & KS_CTRL))
        return HandleSpecialKey (scancode);
        
    if (scancode == SCANCODE_LEFTALT ||
        scancode == SCANCODE_RIGHTALT) {
        if (message == MSG_KEYDOWN) {
            mg_altdown = 1;
            return 0;
        }
        else {
            mg_altdown = 0;
            if (mg_modal == 1) {
                mg_modal = 0;
                return 0;
            }
        }
    }

    if (mg_altdown) {
        if (message == MSG_KEYDOWN) {
            if( scancode == SCANCODE_TAB) {
            
                mg_modal = 1;
                
                if (__mg_ptmi)
                    dskForceCloseMenu ();

                pNextWin = dskGetNextVisibleMainWindow (__mg_active_mainwnd);
                dskMoveToTopMost (pNextWin, RCTM_KEY, 0);
                dskChangActiveWindow (pNextWin);
                return 0;
            }
            else if (scancode == SCANCODE_ESCAPE) {

                mg_modal = 1;

                if (__mg_active_mainwnd) {
#ifdef  _DEBUG
                    uHALr_printf( "Will be Closed: %p.\n", __mg_active_mainwnd);
#endif
                    PostMessage ((HWND)__mg_active_mainwnd, MSG_CLOSE, 0, 0);
                    return 0;
                }
            }
        }
        else if (mg_modal == 1)
            return 0;
    }
    
    if (scancode == SCANCODE_LEFTALT
             || scancode == SCANCODE_RIGHTALT || mg_altdown) {
        if (message == MSG_KEYDOWN)
            message = MSG_SYSKEYDOWN;
        else {
            message = MSG_SYSKEYUP;
            mg_altdown = 0;
        }
    }
#ifdef _LITE_VERSION
    if (__mg_ime_wnd) {
        if (dskHandleKeyHooks (__mg_ime_wnd, 
                                message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            PostMessage (__mg_ime_wnd, 
                            message, (WPARAM)scancode, (LPARAM)status);
        return 0;
    }

#else
    else if (__mg_ime_wnd && __mg_active_mainwnd) {
        if (dskHandleKeyHooks (__mg_ime_wnd, 
                                    message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            PostMessage (__mg_ime_wnd, 
                                message, (WPARAM)scancode, (LPARAM)status);

        return 0;
    }
#endif

    if (__mg_active_mainwnd) {
        if (dskHandleKeyHooks ((HWND)__mg_active_mainwnd, 
                                message, (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            PostMessage ((HWND)__mg_active_mainwnd, message, 
                (WPARAM)scancode, (LPARAM)status);
    }
    else {
        if (dskHandleKeyHooks (HWND_DESKTOP, message, 
                (WPARAM)scancode, (LPARAM)status) == HOOK_GOON)
            SendMessage (HWND_DESKTOP, MSG_DT_KEYOFF + message,
                (WPARAM)scancode, (LPARAM)status);
    }

    return 0;
}

static PMAINWIN __mgs_old_under_pointer = NULL;
static PMAINWIN __mgs_captured_main_win = (void*)HWND_INVALID;

/* defined in ../gui/window.c */
extern void __mg_reset_mainwin_capture_info (PCONTROL ctrl);

void __mg_reset_desktop_capture_info (PMAINWIN pWin)
{
    if (pWin == __mgs_old_under_pointer)
        __mgs_old_under_pointer = NULL;
    if (pWin == __mgs_captured_main_win)
        __mgs_captured_main_win = (void*)HWND_INVALID;
    if ((HWND)pWin == __mg_capture_wnd)
        __mg_capture_wnd = HWND_DESKTOP;

    __mg_reset_mainwin_capture_info ((PCONTROL)pWin);
}

static int MouseMessageHandler (int message, WPARAM flags, int x, int y)
{
    PMAINWIN pUnderPointer;
    PMAINWIN pCtrlPtrIn;
    int CapHitCode;
    int UndHitCode;
    int cx, cy;
    
    CapHitCode = HT_UNKNOWN;
    UndHitCode = HT_UNKNOWN;
    cx = 0;
    cy = 0;
   
    if (__mg_capture_wnd) {
        PostMessage (__mg_capture_wnd, message, flags | KS_CAPTURED, MAKELONG (x, y));
        return 0;
    }
   
    if (__mgs_captured_main_win != (void*)HWND_INVALID && __mgs_captured_main_win != NULL) {
        CapHitCode = SendAsyncMessage((HWND)__mgs_captured_main_win, MSG_HITTEST,
                                        (WPARAM)x, (LPARAM)y);
    }
   
    pCtrlPtrIn = GetMainWindowPtrUnderPoint (x, y);
    
    if (dskHandleMouseHooks ((HWND)pCtrlPtrIn, 
                            message, flags, MAKELONG (x, y)) == HOOK_STOP)
        return 0;
       
    if (pCtrlPtrIn && pCtrlPtrIn->WinType == TYPE_CONTROL) {
        pUnderPointer = pCtrlPtrIn->pMainWin;
        UndHitCode = HT_CLIENT;
        cx = x - pUnderPointer->cl;
        cy = y - pUnderPointer->ct;
    }
    else {
        pUnderPointer = pCtrlPtrIn;
        pCtrlPtrIn = NULL;

        if (pUnderPointer) {
            UndHitCode = SendAsyncMessage((HWND)pUnderPointer, MSG_HITTEST, 
                                        (WPARAM)x, (LPARAM)y);
            cx = x - pUnderPointer->cl;
            cy = y - pUnderPointer->ct;
        }
    }
    
    switch (message) {
        case MSG_MOUSEMOVE:
            if (__mgs_captured_main_win != (void *)HWND_INVALID) {
                if (__mgs_captured_main_win)
                    {
                    PostMessage((HWND)__mgs_captured_main_win, MSG_NCMOUSEMOVE, 
                                CapHitCode, MAKELONG (x, y));
                    }
                else
                    {
                    PostMessage(HWND_DESKTOP, MSG_DT_MOUSEMOVE,
                                pUnderPointer == NULL, MAKELONG (x, y));
                    }
                break;
            }

            if (pUnderPointer == NULL) {
                if (__mgs_old_under_pointer) {
                    PostMessage ((HWND)__mgs_old_under_pointer,
                        MSG_MOUSEMOVEIN, FALSE, 0);
                    PostMessage ((HWND)__mgs_old_under_pointer,
                        MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
                }
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
                if (!mgIsServer)
#endif
                    SetCursor (GetSystemCursor (IDC_ARROW));
                __mgs_old_under_pointer = NULL;
            }
            else {
                HCURSOR def_cursor;
                def_cursor = GetDefaultCursor ();
                
                if (__mgs_old_under_pointer != pUnderPointer) {
                    if (__mgs_old_under_pointer) {
                         PostMessage ((HWND)__mgs_old_under_pointer,
                            MSG_MOUSEMOVEIN, FALSE, (LPARAM)pUnderPointer);
                         PostMessage ((HWND)__mgs_old_under_pointer,
                            MSG_NCMOUSEMOVE, HT_OUT, MAKELONG (x, y));
                    }
                     PostMessage ((HWND)pUnderPointer,
                        MSG_MOUSEMOVEIN, TRUE, (LPARAM)__mgs_old_under_pointer);
                    __mgs_old_under_pointer = pUnderPointer;
                }

                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    if (def_cursor)
                        SetCursor (def_cursor);
                    break;
                }

                if (UndHitCode == HT_CLIENT) {
                    if (def_cursor)
                        SetCursor (def_cursor);
                    PostMessage ((HWND)pUnderPointer, MSG_SETCURSOR, 
                            UndHitCode, MAKELONG (cx, cy));
                    PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE, 
                            UndHitCode, MAKELONG (x, y));
                    PostMessage((HWND)pUnderPointer, MSG_MOUSEMOVE, 
                            flags, MAKELONG (cx, cy));
                }
                else {
                    if (def_cursor)
                        SetCursor (def_cursor);
                    PostMessage ((HWND)pUnderPointer, MSG_NCSETCURSOR, 
                            UndHitCode, MAKELONG (x, y));
                    PostMessage((HWND)pUnderPointer, MSG_NCMOUSEMOVE, 
                            UndHitCode, MAKELONG (x, y));
                }
            }
        break;

        case MSG_LBUTTONDOWN:
        case MSG_RBUTTONDOWN:
            if (pUnderPointer) {
                if (__mg_ptmi)
                    dskForceCloseMenu ();

                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    Ping ();
                    break;
                }

                if (pCtrlPtrIn == NULL) {
                    if (!dskIsTopMost (pUnderPointer)) {
                        dskMoveToTopMost (pUnderPointer, RCTM_CLICK, MAKELONG (x, y));
                    }
                
                    if (pUnderPointer != dskChangActiveWindow (pUnderPointer))
                        PostMessage ((HWND) pUnderPointer,
                                MSG_MOUSEACTIVE, UndHitCode, 0);
                }
                
                if (UndHitCode != HT_CLIENT) {
                    if (UndHitCode & HT_NEEDCAPTURE)
                        __mgs_captured_main_win = pUnderPointer;
                    else
                        __mgs_captured_main_win = (void*)HWND_INVALID;

                    PostMessage ((HWND)pUnderPointer, message + MSG_NCMOUSEOFF,
                            UndHitCode, MAKELONG (x, y));
                }
                else {
                    PostMessage((HWND)pUnderPointer, message, 
                        flags, MAKELONG(cx, cy));
                    __mgs_captured_main_win = (void*)HWND_INVALID;
                }
            }
            else {
                dskChangActiveWindow (NULL);
                __mgs_captured_main_win = NULL;
                PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                            flags, MAKELONG (x, y));
            }
        break;

        case MSG_LBUTTONUP:
        case MSG_RBUTTONUP:
            if (__mgs_captured_main_win != (void*)HWND_INVALID) {
                if (__mgs_captured_main_win)
                    PostMessage ((HWND)__mgs_captured_main_win, message + MSG_NCMOUSEOFF,
                        CapHitCode, MAKELONG (x, y));
                else if (!pUnderPointer)
                    PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                        flags, MAKELONG (x, y));
                
                __mgs_captured_main_win = (void*)HWND_INVALID;
                break;
            }
            else {
                if (pUnderPointer) {
                    if (pUnderPointer->dwStyle & WS_DISABLED) {
                        break;
                    }
                    
                    if (UndHitCode == HT_CLIENT) {
                        PostMessage((HWND)pUnderPointer, message, 
                            flags, MAKELONG (cx, cy));
                    }
                    else {
                        PostMessage((HWND)pUnderPointer, 
                            message + MSG_NCMOUSEOFF, 
                            UndHitCode, MAKELONG (x, y));
                    }
                }
                else
                    PostMessage (HWND_DESKTOP, message + MSG_DT_MOUSEOFF,
                        flags, MAKELONG (x, y));
            }
        break;
        
        case MSG_LBUTTONDBLCLK:
        case MSG_RBUTTONDBLCLK:
            if (pUnderPointer)
            {
                if (pUnderPointer->dwStyle & WS_DISABLED) {
                    Ping ();
                    break;
                }

                if(UndHitCode == HT_CLIENT)
                    PostMessage((HWND)pUnderPointer, message, 
                        flags, MAKELONG(cx, cy));
                else
                    PostMessage((HWND)pUnderPointer, message + MSG_NCMOUSEOFF, 
                        UndHitCode, MAKELONG (x, y));
            }
            else {
                PostMessage(HWND_DESKTOP, message + MSG_DT_MOUSEOFF, 
                        flags, MAKELONG (x, y));
            }
        break;

    }

    return 0;
}

static int WindowMessageHandler(int message, PMAINWIN pWin, LPARAM lParam)
{
    PMAINWIN pTemp;
    int iRet;
    iRet = 0;

    switch (message)
    {
        case MSG_ADDNEWMAINWIN:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            pWin->pGCRInfo = &pWin->GCRInfo;
            iRet = (int)dskAddNewMainWindow(pWin, (PZORDERNODE)lParam);
        break;

        case MSG_REMOVEMAINWIN:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            iRet = (int)dskRemoveMainWindow(pWin);
            __mg_reset_desktop_capture_info (pWin);
        break;

        case MSG_MOVETOTOPMOST:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            pTemp = dskMoveToTopMost(pWin, RCTM_MESSAGE, 0);
        break;

        case MSG_SHOWMAINWIN:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            iRet = (int)dskShowMainWindow(pWin, TRUE);

            if ((HWND)pWin == __mg_ime_wnd)
                return iRet;
        break;

        case MSG_HIDEMAINWIN:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            iRet = (int)dskHideMainWindow(pWin);
            __mg_reset_desktop_capture_info (pWin);
            if ((HWND)pWin == __mg_ime_wnd)
                return iRet;
        break;

        case MSG_MOVEMAINWIN:
            dskMoveToTopMost (pWin, RCTM_MESSAGE, 0);
            dskMoveMainWindow (pWin, (const RECT*)lParam);
            return 0;

        case MSG_GETACTIVEMAIN:
            return (int)__mg_active_mainwnd;
        
        case MSG_SETACTIVEMAIN:
            iRet = (int)dskChangActiveWindow (pWin);
            return iRet;

        case MSG_GETCAPTURE:
            return (int)__mg_capture_wnd;
        
        case MSG_SETCAPTURE:
            return (int)DesktopSetCapture ((HWND)pWin);
        
        case MSG_TRACKPOPUPMENU:
            return dskTrackPopupMenu((PTRACKMENUINFO)lParam);

        case MSG_ENDTRACKMENU:
            return dskEndTrackMenu((PTRACKMENUINFO)lParam);

        case MSG_CLOSEMENU:
            if (!dskForceCloseMenu ())
                return 0;
            if (!lParam)
                return 0;
        break;

        case MSG_SCROLLMAINWIN:
            iRet = dskScrollMainWindow (pWin, (PSCROLLWINDOWINFO)lParam);
            return iRet;

        case MSG_CARET_CREATE:
            sg_hCaretWnd = (HWND)pWin;
            sg_uCaretBTime = pWin->pCaretInfo->uTime;
            return 0;

        case MSG_CARET_DESTROY:
            sg_hCaretWnd = HWND_DESKTOP;
            return 0;

        case MSG_ENABLEMAINWIN:
            iRet = !(pWin->dwStyle & WS_DISABLED);

            if ( (!(pWin->dwStyle & WS_DISABLED) && !lParam)
                    || ((pWin->dwStyle & WS_DISABLED) && lParam) ) {
                if (lParam)
                    pWin->dwStyle &= ~WS_DISABLED;
                else
                    pWin->dwStyle |=  WS_DISABLED;

                if (pWin->dwStyle & WS_DISABLED) {
                
                    if (__mg_capture_wnd && GetMainWindowPtrOfControl (__mg_capture_wnd) == pWin) 
                        __mg_capture_wnd = HWND_DESKTOP;

                    if (__mg_active_mainwnd == pWin) {
                        dskChangActiveWindow (NULL);
                        break;
                    }
                }

                SendAsyncMessage ((HWND)pWin, MSG_NCPAINT, 0, 0);
            }
            break;
        
        case MSG_ISENABLED:
            return !(pWin->dwStyle & WS_DISABLED);
        
        case MSG_SETWINCURSOR:
        {
            HCURSOR old = pWin->hCursor;

            pWin->hCursor = (HCURSOR)lParam;
            return old;
        }

        case MSG_GETNEXTMAINWIN:
        {
            PZORDERNODE pNode;
            HWND hWnd = HWND_DESKTOP;
            while (1) {
                if (pWin) {
                    pNode = pWin->pZOrderNode->pNext;
                    if (pNode)
                        hWnd = pNode->hWnd;
                    else if (pWin->dwExStyle & WS_EX_TOPMOST)
                        hWnd = sg_MainWinZOrder.pTopMost->hWnd;
                }
                else {
                    if (sg_TopMostWinZOrder.pTopMost->hWnd)
                        hWnd = sg_TopMostWinZOrder.pTopMost->hWnd;
                    else
                        hWnd = sg_MainWinZOrder.pTopMost->hWnd;
                }
                pWin = CheckAndGetMainWindowPtr (hWnd);
                if (pWin && (pWin->dwExStyle & WS_EX_CTRLASMAINWIN))
                    continue;
                return hWnd;
            }
            break;
        }

        case MSG_SHOWGLOBALCTRL:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            dskMoveToTopMost (pWin, RCTM_SHOWCTRL, 0);
            dskSetPrimitiveChildren (pWin, TRUE);
            break;

        case MSG_HIDEGLOBALCTRL:
            if (__mg_ptmi)
                dskForceCloseMenu ();
            dskHideMainWindow (pWin);
            dskSetPrimitiveChildren (pWin, FALSE);
            break;
   }

   return iRet;
}

#define IDM_REDRAWBG    MINID_RESERVED
#define IDM_CLOSEALLWIN (MINID_RESERVED + 1)
#define IDM_ENDSESSION  (MINID_RESERVED + 2)

#define IDM_FIRSTWINDOW (MINID_RESERVED + 101)

#ifndef _LITE_VERSION

/*
 * When the user clicks right mouse button on desktop, 
 * MiniGUI will display a menu for user. You can use this 
 * function to customize the desktop menu. e.g. add a new 
 * menu item.
 * Please use an integer larger than IDM_DTI_FIRST as the 
 * command ID.
 */
#define IDC_DTI_ABOUT   (IDM_DTI_FIRST)
void CustomizeDesktopMenuDefault (HMENU hmnu, int iPos)
{
#ifdef _MISC_ABOUTDLG
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = IDC_DTI_ABOUT;
    mii.typedata    = (DWORD)GetSysText(SysText [19]);
    mii.hsubmenu    = 0;
    InsertMenuItem (hmnu, iPos, TRUE, &mii);
#endif
}

/*
 * When user choose a custom menu item on desktop menu,
 * MiniGUI will call this function, and pass the command ID
 * of selected menu item.
 */
int CustomDesktopCommandDefault (int id)
{
#ifdef _MISC_ABOUTDLG
#ifndef _LITE_VERSION
    if (id == IDC_DTI_ABOUT)
        OpenAboutDialog ();
#endif
#endif

    return 0;
}

CustomizeDesktopMenuFunc CustomizeDesktopMenu = CustomizeDesktopMenuDefault;
CustomDesktopCommandFunc CustomDesktopCommand = CustomDesktopCommandDefault;
#endif

const char* SysText [] =
{
    "Windows...",               // 0
    "Start...",                 // 1
    "Refresh Background",       // 2
    "Close All Windows",        // 3
    "End Session",              // 4
    "Operations...",            // 5
    "Minimize",                 // 6
    "Maximize",                 // 7
    "Restore",                  // 8
    "Close",                    // 9
    "OK",                       // 10
    "Next",                     // 11
    "Cancel",                   // 12
    "Previous",                 // 13
    "Yes",                      // 14
    "No",                       // 15
    "Abort",                    // 16
    "Retry",                    // 17
    "Ignore",                   // 18
    "About MiniGUI...",         // 19
    "Open File",                // 20
    "Save File",                // 21
    "Color Selection",          // 22
    NULL
};

#if defined(_GB_SUPPORT) | defined (_GBK_SUPPORT) | defined (_GB18030_SUPPORT)
static const char* SysText_GB [] =
{
    "窗口...",              // 0
    "开始...",              // 1
    "刷新背景",             // 2
    "关闭所有窗口",         // 3
    "结束会话",             // 4
    "窗口操作...",          // 5
    "最小化",               // 6
    "最大化",               // 7
    "恢复",                 // 8
    "关闭",                 // 9
    "确定",                 // 10
    "下一步",               // 11
    "取消",                 // 12
    "上一步",               // 13
    "是(Y)",                // 14
    "否(N)",                // 15
    "终止(A)",              // 16
    "重试(R)",              // 17
    "忽略(I)",              // 18
    "关于 MiniGUI...",      // 19
    "打开文件",             // 20
    "保存文件",             // 21
    "颜色选择",             // 22
};
#endif

#ifdef _BIG5_SUPPORT
static const char* SysText_BIG5 [] =
{
    "怠...",
    "邵﹍...",
    "穝璉春",
    "闽超┮Τ怠",
    "挡穦杠",
    "怠巨...",
    "程て",
    "程て",
    "蝋",
    "闽超",
    "谔﹚",
    "˙",
    "",
    "˙",
    "琌(Y)",
    "(N)",
    "沧ゎ(A)",
    "刚(R)",
    "┛菠(I)",
    "闽 MiniGUI..."
    "Open File",
    "Save File",
    "Color Selection",
};
#endif

static void init_local_sys_text (void)
{
    const char* charset;
    charset = GetSysCharset (TRUE);

    local_SysText = SysText;

    if (charset == NULL)
    	charset = GetSysCharset (FALSE);

#ifdef _GB_SUPPORT
    if (strcmp (charset, FONT_CHARSET_GB2312_0) == 0)
        local_SysText = SysText_GB;
#endif
#ifdef _GBK_SUPPORT
    if (strcmp (charset, FONT_CHARSET_GBK) == 0)
        local_SysText = SysText_GB;
#endif
#ifdef _GB18030_SUPPORT
    if (strcmp (charset, FONT_CHARSET_GB18030_0) == 0)
        local_SysText = SysText_GB;
#endif
#ifdef _BIG5_SUPPORT
    if (strcmp (charset, FONT_CHARSET_BIG5) == 0)
        local_SysText = SysText_BIG5;
#endif

}

static HMENU dskCreateWindowSubMenu (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(SysText [0]);

    hmnu = CreatePopupMenu (&mii);
    return hmnu;
}

static HMENU dskCreateDesktopMenu (void)
{
    HMENU hmnu;
    MENUITEMINFO mii;

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(SysText [1]);

    hmnu = CreatePopupMenu (&mii);

    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type        = MFT_STRING;
    mii.id          = IDM_REDRAWBG;
    mii.typedata    = (DWORD)GetSysText(SysText [2]); 
    InsertMenuItem (hmnu, 0, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = IDM_CLOSEALLWIN;
    mii.typedata    = (DWORD)GetSysText(SysText [3]); 
    InsertMenuItem (hmnu, 1, TRUE, &mii);
    
    mii.type        = MFT_STRING;
    mii.id          = IDM_ENDSESSION;
    mii.typedata    = (DWORD)GetSysText(SysText [4]);
    InsertMenuItem (hmnu, 2, TRUE, &mii);

    mii.type        = MFT_STRING;
    mii.id          = 0;
    mii.typedata    = (DWORD)GetSysText(SysText [0]);
    mii.hsubmenu     = dskCreateWindowSubMenu();
    InsertMenuItem (hmnu, 3, TRUE, &mii);
                        
    mii.type        = MFT_SEPARATOR;
    mii.id          = 0;
    mii.typedata    = 0;
    mii.hsubmenu    = 0;
    InsertMenuItem(hmnu, 4, TRUE, &mii);

    return hmnu;
}

static void dskUpdateDesktopMenu (HMENU hDesktopMenu)
{
    MENUITEMINFO mii;
    HMENU hWinMenu;
    int nCount, iPos;
    PZORDERNODE pNode;
    PMAINWIN    pWin;
    int id;

    hWinMenu = GetSubMenu (hDesktopMenu, 3);

    nCount = GetMenuItemCount (hWinMenu);

    for (iPos = nCount; iPos > 0; iPos --)
        DeleteMenu (hWinMenu, iPos - 1, MF_BYPOSITION);
    
    memset (&mii, 0, sizeof(MENUITEMINFO));
    mii.type = MFT_STRING;

    id = IDM_FIRSTWINDOW;
    iPos = 0;
    
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode)
    {
        pWin = (PMAINWIN)(pNode->hWnd);
        if ((pWin->WinType == TYPE_MAINWIN) && (pWin->dwStyle & WS_VISIBLE)) {
            mii.state       = MFS_ENABLED;
        }
        else
            mii.state       = MFS_DISABLED;
            
        mii.id              = id;
        mii.typedata        = (DWORD)pWin->spCaption; 
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);

        id++;
        iPos++;
        pNode = pNode->pNext;
    }
    
    if (iPos != 0) {
        mii.type            = MFT_SEPARATOR;
        mii.state           = 0;
        mii.id              = 0;
        mii.typedata        = 0;
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);
        iPos ++;
        mii.type            = MFT_STRING;
    }

    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode)
    {
        pWin = (PMAINWIN)(pNode->hWnd);
        if ((pWin->WinType == TYPE_MAINWIN) && (pWin->dwStyle & WS_VISIBLE)) {
            mii.state       = MFS_ENABLED;
        }
        else
            mii.state       = MFS_DISABLED;
            
        mii.id              = id;
        mii.typedata        = (DWORD)pWin->spCaption; 
        InsertMenuItem(hWinMenu, iPos, TRUE, &mii);

        id++;
        iPos++;
        pNode = pNode->pNext;
    }
    
    nCount = GetMenuItemCount (hDesktopMenu);
    for (iPos = nCount; iPos > 5; iPos --)
        DeleteMenu (hDesktopMenu, iPos - 1, MF_BYPOSITION);
        
#ifndef _LITE_VERSION
    CustomizeDesktopMenu (hDesktopMenu, 5);
#endif
}

static PMAINWIN dskGetWindowFromZOrder (int iZOrder)
{
    int i;
    PZORDERNODE pNode;
    
    i = 0;
    
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode)
    {
        if (iZOrder == i)
            return (PMAINWIN)(pNode->hWnd);

        i ++;
        pNode = pNode->pNext;
    }

    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode)
    {
        if (iZOrder == i)
            return (PMAINWIN)(pNode->hWnd);

        i ++;
        pNode = pNode->pNext;
    }

    return NULL;
}

static int dskDesktopCommand (int id)
{
    if (id == IDM_REDRAWBG)
        SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
    else if (id == IDM_CLOSEALLWIN) {
        PZORDERNODE pNode;
        PMAINWIN pWin;
    
        pNode = sg_TopMostWinZOrder.pTopMost;
        while (pNode) {
            pWin = (PMAINWIN)pNode->hWnd;
            if ((pWin->WinType != TYPE_CONTROL)
                    && (pWin->pHosting == NULL))
                PostMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
            pNode = pNode->pNext;
        }

        pNode = sg_MainWinZOrder.pTopMost;
        while (pNode) {
            pWin = (PMAINWIN)pNode->hWnd;
            if ((pWin->WinType == TYPE_MAINWIN)
                    && (pWin->pHosting == NULL))
                PostMessage ((HWND)pWin, MSG_CLOSE, 0, 0);
            pNode = pNode->pNext;
        }
    }
    else if (id == IDM_ENDSESSION)
        PostMessage (HWND_DESKTOP, MSG_ENDSESSION, 0, 0);
    else if (id >= IDM_FIRSTWINDOW) {
        PMAINWIN pSelectedWin;

        pSelectedWin = dskGetWindowFromZOrder (id - IDM_FIRSTWINDOW);

        if (pSelectedWin 
                && pSelectedWin->WinType == TYPE_MAINWIN
                && !(pSelectedWin->dwStyle & WS_DISABLED)) {
            if (__mg_ptmi)
               dskForceCloseMenu ();

            dskMoveToTopMost (pSelectedWin, RCTM_MESSAGE, 0);
            dskChangActiveWindow (pSelectedWin);
        }
    }

    return 0;
}

static int dskBroadcastMessage (PMSG pMsg)
{
    PZORDERNODE pNode;
    PMAINWIN pWin;
    int count;
    
    count = 0;
    
    pNode = sg_TopMostWinZOrder.pTopMost;
    while (pNode) {
        pWin = (PMAINWIN)pNode->hWnd;
        if (pWin->WinType != TYPE_CONTROL) {
            PostMessage ((HWND)pWin, pMsg->message, pMsg->wParam, pMsg->lParam);
            count ++;
        }
        pNode = pNode->pNext;
    }

    pNode = sg_MainWinZOrder.pTopMost;
    while (pNode) {
        pWin = (PMAINWIN)pNode->hWnd;
        if (pWin->WinType != TYPE_CONTROL) {
            PostMessage ((HWND)pWin, pMsg->message, pMsg->wParam, pMsg->lParam);
            count ++;
        }
        pNode = pNode->pNext;
    }

    return count;
}

static void dskOnNewCtrlInstance (PCONTROL pParent, PCONTROL pNewCtrl)
{
    PCONTROL pFirstCtrl, pLastCtrl;
        
    pFirstCtrl = pParent->children;

    pNewCtrl->next = NULL;
    
    if (!pFirstCtrl) {
        pParent->children = pNewCtrl;
        pNewCtrl->prev = NULL;
    }
    else {
        pLastCtrl = pFirstCtrl;
        
        while (pLastCtrl->next)
            pLastCtrl = pLastCtrl->next;
            
        pLastCtrl->next = pNewCtrl;
        pNewCtrl->prev = pLastCtrl;
    }

    init_invrgn ((PMAINWIN)pNewCtrl);

    if (pNewCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        PZORDERNODE pNode = pNewCtrl->pZOrderNode;

        // Init Global Clip Region info.
        init_gcrinfo ((PMAINWIN)pNewCtrl);

        // Add to Z-Order list.
        pNode->hWnd = (HWND)pNewCtrl;
        if (pNewCtrl->dwExStyle & WS_EX_TOPMOST)
            add_new_window (&sg_TopMostWinZOrder, pNode);
        else
            add_new_window (&sg_MainWinZOrder, pNode);

        // Update others' GCRInfo.
        if (pNewCtrl->dwStyle & WS_VISIBLE ) {
            if (pNewCtrl->dwExStyle & WS_EX_TOPMOST)
                dskUpdateGCRInfoOnShowNewMainWinEx ((PMAINWIN)pNewCtrl);
            else
                dskUpdateGCRInfoOnShowNewMainWin ((PMAINWIN)pNewCtrl);
        }
    }
        
    pNewCtrl->pcci->nUseCount ++;
}

static BOOL dskOnRemoveCtrlInstance (PCONTROL pParent, PCONTROL pCtrl)
{
    PCONTROL pFirstCtrl;
    BOOL fFound;
    
    fFound = FALSE;

    pFirstCtrl = pParent->children;

    if (!pFirstCtrl)
        return FALSE;
    else {
        if (pFirstCtrl == pCtrl) {
            pParent->children = pCtrl->next;
            if (pCtrl->next)
                pCtrl->next->prev = NULL;
            fFound = TRUE;
        }
        else {
            while (pFirstCtrl->next) {
                if (pFirstCtrl->next == pCtrl) {
                    pFirstCtrl->next = pCtrl->next;
                    if (pFirstCtrl->next)
                        pFirstCtrl->next->prev = pCtrl->prev;
                    fFound = TRUE;
                    break;
                }

                pFirstCtrl = pFirstCtrl->next;
            }
        }
    }

    if (pCtrl->dwExStyle & WS_EX_CTRLASMAINWIN) {
        if (pCtrl->dwStyle & WS_VISIBLE) {
            if (pCtrl->dwExStyle & WS_EX_TOPMOST)
                dskUpdateGCRInfoOnHideMainWinEx ((PMAINWIN)pCtrl);
            else
                dskUpdateGCRInfoOnHideMainWin ((PMAINWIN)pCtrl);
        }

        if (pCtrl->dwExStyle & WS_EX_TOPMOST)
            remove_window (&sg_TopMostWinZOrder, (PMAINWIN)pCtrl);
        else
            remove_window (&sg_MainWinZOrder, (PMAINWIN)pCtrl);
    }

    if (fFound)
        pCtrl->pcci->nUseCount --;

    if ((HWND)pCtrl == __mg_capture_wnd)
        /* force release the capture */
        __mg_capture_wnd = HWND_DESKTOP;

    return fFound;
}

static int dskRegisterIMEWnd (HWND hwnd)
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (!mgIsServer)
        return ERR_INV_HWND;
#endif

    if (__mg_ime_wnd != HWND_DESKTOP)
        return ERR_IME_TOOMUCHIMEWND;

    if (!CheckAndGetMainWindowPtr (hwnd))
        return ERR_INV_HWND;

    __mg_ime_wnd = hwnd;

    SendNotifyMessage (__mg_ime_wnd, MSG_IME_CLOSE, 0, 0);

#ifndef _LITE_VERSION
    SendNotifyMessage (__mg_ime_wnd, MSG_IME_SETTARGET, 
            (WPARAM)__mg_active_mainwnd, 0);
#endif

    return ERR_OK;
}

static int dskUnregisterIMEWnd (HWND hwnd)
{
    if (__mg_ime_wnd != hwnd)
        return ERR_IME_NOSUCHIMEWND;

    __mg_ime_wnd = HWND_DESKTOP;

    return ERR_OK;
}

static int dskSetIMEStatus (int iIMEStatusCode, int Value)
{
    if (__mg_ime_wnd == HWND_DESKTOP)
        return ERR_IME_NOIMEWND;

    SendMessage (__mg_ime_wnd, 
        MSG_IME_SETSTATUS, (WPARAM)iIMEStatusCode, (LPARAM)Value);

    return ERR_OK;
}

static int dskGetIMEStatus (int iIMEStatusCode)
{
    if (__mg_ime_wnd == HWND_DESKTOP)
        return ERR_IME_NOIMEWND;

    return SendMessage (__mg_ime_wnd, MSG_IME_GETSTATUS, iIMEStatusCode, 0);
}

static int dskGetBgPicturePos (void)
{
    char szValue [21];
/*
    if( GetMgEtcValue ("bgpicture", "position", szValue, 20) < 0 ) {
        strcpy (szValue, "center");
    }

    if (!strcmp (szValue, "none"))
        return -1;
    if (!strcmp (szValue, "center"))
        return 0;
    if (!strcmp (szValue, "upleft"))
        return 1;
    if (!strcmp (szValue, "downleft"))
        return 2;
    if (!strcmp (szValue, "upright"))
        return 3;
    if (!strcmp (szValue, "downright"))
        return 4;
    if (!strcmp (szValue, "upcenter"))
        return 5;
    if (!strcmp (szValue, "downcenter"))
        return 6;
    if (!strcmp (szValue, "vcenterleft"))
        return 7;
    if (!strcmp (szValue, "vcenterright"))
        return 8;

    return -1;
*/
    return 0;
}

static void dskGetBgPictureXY (int pos, int w, int h, int* x, int* y)
{
    switch (pos) {
    case 0: // center
        *x = (g_rcScr.right - w) >> 1;
        *y = (g_rcScr.bottom - h) >> 1;
        break;
    case 1: // upleft
        *x = 0;
        *y = 0;
        break;
    case 2: // downleft
        *x = 0;
        *y = g_rcScr.bottom - h;
        break;
    case 3: // upright
        *x = g_rcScr.right - w;
        *y = 0;
        break;
    case 4: // downright
        *x = g_rcScr.right - w;
        *y = g_rcScr.bottom - h;
        break;
    case 5: // upcenter
        *x = (g_rcScr.right - w) >> 1;
        *y = 0;
        break;
    case 6: // downcenter
        *x = (g_rcScr.right - w) >> 1;
        *y = g_rcScr.bottom - h;
        break;
    case 7: // vcenterleft
        *x = 0;
        *y = (g_rcScr.bottom - h) >> 1;
        break;
    case 8: // vcenterright
        *x = g_rcScr.right - w;
        *y = (g_rcScr.bottom - h) >> 1;
        break;
    default:
        *x = 0;
        *y = 0;
        break;
    }
}

int DesktopWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    static PBITMAP bg_bmp;
    static int pic_x, pic_y;
    static HDC hDesktopDC;
    static HMENU hDesktopMenu;
    RECT* pInvalidRect;
    int flags, x, y;

#ifdef _LITE_VERSION
    static UINT sg_old_counter;
    sg_old_counter = 0;
#endif

    if (message >= MSG_FIRSTWINDOWMSG && message <= MSG_LASTWINDOWMSG)
        return WindowMessageHandler (message, (PMAINWIN)wParam, lParam);

    if (message == MSG_CHAR && __mg_active_mainwnd) {
        PostMessage ((HWND)__mg_active_mainwnd, message, wParam, lParam);
    }
    else if (message >= MSG_FIRSTKEYMSG && message <= MSG_LASTKEYMSG) {
        if (wParam == SCANCODE_PRINTSCREEN && message == MSG_KEYDOWN) {
#ifdef _MISC_SAVESCREEN
            static int n;
            char buffer[20];
            n = 1;
            
            sprintf (buffer, "%x-%d.bmp", (lParam & KS_CTRL)?
                                    (HWND)__mg_active_mainwnd:
                                    HWND_DESKTOP, n);
            if (SaveMainWindowContent ((lParam & KS_CTRL)?
                                    (HWND)__mg_active_mainwnd:
                                    HWND_DESKTOP,
                                    buffer)) {
                Ping ();
                n ++;
            }
#endif
        }
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        else if (mgIsServer && wParam == SCANCODE_ESCAPE && lParam & KS_CTRL) {
#else
        else if (wParam == SCANCODE_ESCAPE && lParam & KS_CTRL) {
#endif
            dskUpdateDesktopMenu (hDesktopMenu);
            TrackPopupMenu (hDesktopMenu, TPM_DEFAULT, 
                0, g_rcScr.bottom, HWND_DESKTOP);
        }

        if (__mg_ptmi) {
            if (PopupMenuTrackProc (__mg_ptmi, message, wParam, lParam))
                return KeyMessageHandler (message, (int)wParam, (DWORD)lParam);
        }
        else
            return KeyMessageHandler (message, (int)wParam, (DWORD)lParam);
    }
        
    if (message >= MSG_FIRSTMOUSEMSG && message <= MSG_LASTMOUSEMSG) {

        flags = (int)wParam;

        x = LOSWORD (lParam);
        y = HISWORD (lParam);

        if (__mg_ptmi) {
            if (PopupMenuTrackProc (__mg_ptmi, message, x, y))
                return MouseMessageHandler (message, flags, x, y);
        }
        else
        {
            return MouseMessageHandler (message, flags, x, y);
        }

    }
    
    if (message == MSG_COMMAND) {
        if (wParam <= MAXID_RESERVED && wParam >= MINID_RESERVED)
            return dskDesktopCommand ((int)wParam);
#ifndef _LITE_VERSION
        else
            return CustomDesktopCommand ((int)wParam);
#endif
    }

    switch (message) {
        case MSG_STARTSESSION:
            mousehook.hook=0;
            keyhook.hook=0;
            __mgs_old_under_pointer = NULL;
            __mgs_captured_main_win = (void*)HWND_INVALID;

            init_local_sys_text ();           
            hDesktopDC = CreatePrivateDC (HWND_DESKTOP);
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            if (mgIsServer)
#endif
            hDesktopMenu = dskCreateDesktopMenu ();
            //bg_bmp = NULL; /* TEST */

            if (dskGetBgPicturePos () < 0)
                bg_bmp = NULL;
            else
                bg_bmp = GetSystemBitmap (SYSBMP_BGPICTURE);

            if (bg_bmp)
                dskGetBgPictureXY (dskGetBgPicturePos (), 
                            bg_bmp->bmWidth, bg_bmp->bmHeight,
                            &pic_x, &pic_y);

        break;

        case MSG_REINITSESSION:
            if (wParam)
                init_local_sys_text ();

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            if (mgIsServer) {
#endif
                DestroyMenu (hDesktopMenu);
                hDesktopMenu = dskCreateDesktopMenu ();
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
            }
#endif
            SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, 0);
        break;

        case MSG_ENDSESSION:
            if ((sg_MainWinZOrder.nNumber + sg_TopMostWinZOrder.nNumber) == 0) {
                if (hDesktopDC) DeletePrivateDC (hDesktopDC);
                hDesktopDC = 0;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
                if (mgIsServer)
#endif
                    if (hDesktopMenu) {
                        DestroyMenu (hDesktopMenu);
                        hDesktopMenu = 0;
                    }

                PostQuitMessage (HWND_DESKTOP);
                return 1;
            }
       break;

        case MSG_ERASEDESKTOP:
            SetBrushColor (hDesktopDC, GetWindowElementColor (BKC_DESKTOP));
            pInvalidRect = (PRECT)lParam;
            if (pInvalidRect) {
                SelectClipRect (hDesktopDC, pInvalidRect);
                FillBox (hDesktopDC, pInvalidRect->left, pInvalidRect->top, 
                            RECTWP (pInvalidRect), RECTHP (pInvalidRect));
            }
            else {
                SelectClipRect (hDesktopDC, &g_rcDesktop);
                FillBox(hDesktopDC, g_rcDesktop.left, g_rcDesktop.top,
                       g_rcDesktop.right + 1,
                       g_rcDesktop.bottom + 1);
            }

            if (bg_bmp) {
                FillBoxWithBitmap (hDesktopDC, pic_x, pic_y,
                                  bg_bmp->bmWidth, bg_bmp->bmHeight, bg_bmp);
            }
        break;

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
        case MSG_NEWCLIENT:
            if (!mgIsServer)
                break;
            clip_desktop ((RECT*)lParam);
            dskChangActiveWindow (NULL);
            break;

        case MSG_DELCLIENT:
            if (!mgIsServer)
                break;
            recalc_desktop (__mg_ptmi);
            break;

        case MSG_TOPMOSTCHANGED:
            if (!mgIsServer)
                break;
            recalc_desktop (__mg_ptmi);
            break;
#endif

#ifdef _LITE_VERSION
        case MSG_SETFOCUS:
            if (__mg_active_mainwnd) {
                SendNotifyMessage ((HWND)__mg_active_mainwnd, MSG_SETFOCUS, 0, 0);
            }
            break;
#endif

        case MSG_PAINT:
        {
            PZORDERNODE pNode;
            PMAINWIN pTemp;
            RECT invrc;
            BOOL is_empty_invrc;
            
            is_empty_invrc = FALSE;
        
            invrc.left = LOWORD(wParam);
            invrc.top = HIWORD(wParam);
            invrc.right = LOWORD(lParam);
            invrc.bottom = HIWORD(lParam);
            if (invrc.top == invrc.bottom || invrc.left == invrc.right)
                is_empty_invrc = TRUE;

			// close the active menu first
			dskForceCloseMenu ();
            SendMessage (HWND_DESKTOP, MSG_ERASEDESKTOP, 0, (LPARAM)(is_empty_invrc?0:&invrc));
            pNode = sg_TopMostWinZOrder.pTopMost;
            while (pNode) {
                pTemp= (PMAINWIN)(pNode->hWnd);
                if (pTemp->WinType != TYPE_CONTROL 
                        && pTemp->dwStyle & WS_VISIBLE) {
                    if (is_empty_invrc) {
                        SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, 0);
                        InvalidateRect ((HWND)pTemp, NULL, TRUE);
                    }
                    else {
                        RECT rcTemp, rcInv;
                        if (IntersectRect (&rcTemp, (RECT*)(&pTemp->left), &invrc)) {
                            dskScreenToWindow (pTemp, &rcTemp, &rcInv);
                            SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                            dskScreenToClient (pTemp, &rcTemp, &rcInv);
                            InvalidateRect ((HWND)pTemp, &rcInv, TRUE);
                        }
                    }
                }
                pNode = pNode->pNext;
            }

            pNode = sg_MainWinZOrder.pTopMost;
            while (pNode) {
                pTemp= (PMAINWIN)(pNode->hWnd);
                if (pTemp->WinType != TYPE_CONTROL 
                        && pTemp->dwStyle & WS_VISIBLE) {
                    if (is_empty_invrc) {
                        SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, 0);
                        InvalidateRect ((HWND)pTemp, NULL, TRUE);
                    }
                    else {
                        RECT rcTemp, rcInv;
                        if (IntersectRect (&rcTemp, (RECT*)(&pTemp->left), &invrc)) {
                            dskScreenToWindow (pTemp, &rcTemp, &rcInv);
                            SendAsyncMessage ((HWND)pTemp, MSG_NCPAINT, 0, (LPARAM)(&rcInv));
                            dskScreenToClient (pTemp, &rcTemp, &rcInv);
                            InvalidateRect ((HWND)pTemp, &rcInv, TRUE);
                        }
                    }
                }
                pNode = pNode->pNext;
            }

        }
        break;

        case MSG_ADDTIMER:
#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
            if (sg_old_counter == 0)
                sg_old_counter = __mg_timer_counter;
#else
            if (sg_old_counter == 0)
                sg_old_counter = SHAREDRES_TIMER_COUNTER;
#endif
#endif
            return AddTimer (((PTIMER)lParam)->hWnd,
                      ((PTIMER)lParam)->id,
                      ((PTIMER)lParam)->speed);
        break;

        case MSG_REMOVETIMER:
            return RemoveTimer (((PTIMER)lParam)->hWnd,
                      ((PTIMER)lParam)->id);
        break;

        case MSG_RESETTIMER:
            return SetTimerSpeed (((PTIMER)lParam)->hWnd,
                      ((PTIMER)lParam)->id,
                      ((PTIMER)lParam)->speed);
        break;

        case MSG_BROADCASTMSG:
            return dskBroadcastMessage ((PMSG)lParam);

        case MSG_REGISTERWNDCLASS:
            return AddNewControlClass ((PWNDCLASS)lParam);

        case MSG_UNREGISTERWNDCLASS:
            return DeleteControlClass ((const char*)lParam);

        case MSG_NEWCTRLINSTANCE:
            dskOnNewCtrlInstance ((PCONTROL)wParam, (PCONTROL)lParam);
            break;
            
        case MSG_REMOVECTRLINSTANCE:
            if (!dskOnRemoveCtrlInstance ((PCONTROL)wParam, (PCONTROL)lParam))
                return -1;
            break;
        
        case MSG_GETCTRLCLASSINFO:
            return (int)GetControlClassInfo ((const char*)lParam);

        case MSG_CTRLCLASSDATAOP:
            return (int)ControlClassDataOp (wParam, (WNDCLASS*)lParam);
            
        case MSG_REGISTERKEYHOOK:
            return (int)dskRegisterKeyHook ((void*)wParam, 
                            (MSGHOOK)lParam);

        case MSG_REGISTERMOUSEHOOK:
            return (int)dskRegisterMouseHook ((void*)wParam, 
                            (MSGHOOK)lParam);

        case MSG_IME_REGISTER:
            return dskRegisterIMEWnd ((HWND)wParam);
            
        case MSG_IME_UNREGISTER:
            return dskUnregisterIMEWnd ((HWND)wParam);
            
        case MSG_IME_SETSTATUS:
            return dskSetIMEStatus ((int)wParam, (int)lParam);

        case MSG_IME_GETSTATUS:
            return dskGetIMEStatus ((int)wParam);
            
#ifdef _LITE_VERSION
        case MSG_SRVNOTIFY:
            BroadcastMessage (MSG_SRVNOTIFY, wParam, lParam);
        break;
#endif

        case MSG_TIMEOUT:
            BroadcastMessage (MSG_IDLE, wParam, 0);
        break;

        case MSG_DT_KEYDOWN:
        case MSG_DT_CHAR:
        case MSG_DT_KEYUP:
        case MSG_DT_SYSKEYDOWN:
        case MSG_DT_SYSCHAR:
        case MSG_DT_SYSKEYUP:
        case MSG_DT_LBUTTONDOWN:
        case MSG_DT_LBUTTONUP:
        case MSG_DT_LBUTTONDBLCLK:
        case MSG_DT_MOUSEMOVE:
        case MSG_DT_RBUTTONDOWN:
        case MSG_DT_RBUTTONDBLCLK:
        break;
        
        case MSG_DT_RBUTTONUP:
#if defined(_LITE_VERSION) && !defined (_STAND_ALONE)
            if (mgIsServer && !mgTopmostLayer) {
                x = LOSWORD (lParam);
                y = HISWORD (lParam);

                dskUpdateDesktopMenu (hDesktopMenu);
                TrackPopupMenu (hDesktopMenu, TPM_DEFAULT, x, y, HWND_DESKTOP);
            }
#else
            x = LOSWORD (lParam);
            y = HISWORD (lParam);

            dskUpdateDesktopMenu (hDesktopMenu);
            TrackPopupMenu (hDesktopMenu, TPM_DEFAULT, x, y, HWND_DESKTOP);
#endif
        break;

        case MSG_TIMER:      // per 0.01s
        {
#ifdef _LITE_VERSION
            static UINT uCounter;
            static UINT blink_counter;
            
            uCounter = 0;
            blink_counter = 0;

#ifdef _STAND_ALONE
            DispatchTimerMessage (__mg_timer_counter - sg_old_counter);
            sg_old_counter = __mg_timer_counter;

            if (__mg_timer_counter < (blink_counter + 10))
                break;

            uCounter += (__mg_timer_counter - blink_counter) * 10;
            blink_counter = __mg_timer_counter;
#else
            DispatchTimerMessage (SHAREDRES_TIMER_COUNTER - sg_old_counter);
            sg_old_counter = SHAREDRES_TIMER_COUNTER;

            if (SHAREDRES_TIMER_COUNTER < (blink_counter + 10))
                break;

            uCounter += (SHAREDRES_TIMER_COUNTER - blink_counter) * 10;
            blink_counter = SHAREDRES_TIMER_COUNTER;
#endif

#else
            static UINT uCounter;
            uCounter = 0;

            DispatchTimerMessage (1);

            if (__mg_timer_counter % 10 != 0)
                break;

            uCounter += 100;
#endif
            if (sg_hCaretWnd != HWND_DESKTOP 
                    && GetMainWindowPtrOfControl (sg_hCaretWnd) == __mg_active_mainwnd
                    && uCounter >= sg_uCaretBTime) {
                PostMessage (sg_hCaretWnd, MSG_CARETBLINK, 0, 0);
                uCounter = 0;
            }
        }
        break;
    }

    return 0;
}

#ifdef _DEBUG
void GUIAPI DumpWindow (FILE* fp, HWND hWnd)
{
    PMAINWIN pWin;
    PCONTROL pCtrl;
    
    pWin = (PMAINWIN)hWnd;

    if (pWin->DataType != TYPE_HWND) {
        fprintf (fp, "DumpWindow: Invalid DataType!\n");
        return;
    }

    if (pWin->WinType == TYPE_MAINWIN) {
        fprintf (fp, "=============== Main Window %#x==================\n", hWnd);

        fprintf (fp, "Rect        -- (%d, %d, %d, %d)\n", pWin->left, pWin->top, pWin->right, pWin->bottom);
        fprintf (fp, "Client      -- (%d, %d, %d, %d)\n", pWin->cl, pWin->ct, pWin->cr, pWin->cb);

        fprintf (fp, "Style       -- %lx\n", pWin->dwStyle);
        fprintf (fp, "ExStyle     -- %lx\n", pWin->dwExStyle);

        fprintf (fp, "PrivCDC     -- %#x\n", pWin->privCDC);

        fprintf (fp, "AddData     -- %lx\n", pWin->dwAddData);
        fprintf (fp, "AddData2    -- %lx\n", pWin->dwAddData2);

        fprintf (fp, "WinProc     -- %p\n", pWin->MainWindowProc);

        fprintf (fp, "Caption     -- %s\n", pWin->spCaption);
        fprintf (fp, "ID          -- %d\n", pWin->id);

        fprintf (fp, "FirstChild  -- %#x\n", pWin->hFirstChild);
        pCtrl = (PCONTROL)pWin->hFirstChild;
        while (pCtrl) {
            fprintf (fp, "    Child   -- %p(%d), %s(%d)\n", pCtrl, pCtrl->id, 
                                pCtrl->pcci->name, pCtrl->pcci->nUseCount);
            pCtrl = pCtrl->next;
        }
        fprintf (fp, "ActiveChild -- %#x\n", pWin->hActiveChild);

        fprintf (fp, "Hosting     -- %p\n", pWin->pHosting);
        fprintf (fp, "FirstHosted -- %p\n", pWin->pFirstHosted);
        fprintf (fp, "NextHosted  -- %p\n", pWin->pNextHosted);
        fprintf (fp, "BkColor     -- %d\n",  pWin->iBkColor);
        fprintf (fp, "Menu        -- %#x\n", pWin->hMenu);
        fprintf (fp, "Accel       -- %#x\n", pWin->hAccel);
        fprintf (fp, "Cursor      -- %#x\n", pWin->hCursor);
        fprintf (fp, "Icon        -- %#x\n", pWin->hIcon);
        fprintf (fp, "SysMenu     -- %#x\n", pWin->hSysMenu);
        fprintf (fp, "MsgQueue    -- %p\n", pWin->pMessages);
#ifndef _LITE_VERSION
        //fprintf (fp, "thread      -- %#x\n", (int)pWin->th);
#endif
    }
    else {
        fprintf (fp, "=============== Control %#x==================\n", hWnd);
        pCtrl = (PCONTROL)hWnd;

        fprintf (fp, "Rect        -- (%d, %d, %d, %d)\n", pCtrl->left, pCtrl->top, pCtrl->right, pCtrl->bottom);
        fprintf (fp, "Client      -- (%d, %d, %d, %d)\n", pCtrl->cl, pCtrl->ct, pCtrl->cr, pCtrl->cb);

        fprintf (fp, "Style       -- %lx\n", pCtrl->dwStyle);
        fprintf (fp, "ExStyle     -- %lx\n", pCtrl->dwExStyle);

        fprintf (fp, "PrivCDC     -- %#x\n", pCtrl->privCDC);

        fprintf (fp, "AddData     -- %lx\n", pCtrl->dwAddData);
        fprintf (fp, "AddData2    -- %lx\n", pCtrl->dwAddData2);

        fprintf (fp, "WinProc     -- %p\n", pCtrl->ControlProc);

        fprintf (fp, "Caption     -- %s\n", pCtrl->spCaption);
        fprintf (fp, "ID          -- %d\n", pCtrl->id);

        fprintf (fp, "FirstChild  -- %p\n", pCtrl->children);
        fprintf (fp, "ActiveChild -- %p\n", pCtrl->active);
        fprintf (fp, "Parent      -- %p\n", pCtrl->pParent);
        fprintf (fp, "Next        -- %p\n", pCtrl->next);

        pCtrl = (PCONTROL)pCtrl->children;
        while (pCtrl) {
            fprintf (fp, "    Child   -- %p(%d), %s(%d)\n", pCtrl, pCtrl->id, 
                                pCtrl->pcci->name, pCtrl->pcci->nUseCount);
            pCtrl = pCtrl->next;
        }
    }

    fprintf (fp, "=================== End ==================\n");
    return; 
}
#endif

