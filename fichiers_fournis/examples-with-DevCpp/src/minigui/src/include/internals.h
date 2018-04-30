/*
** $Id: internals.h,v 1.37 2005/01/04 06:49:50 snig Exp $
**
** internals.h: this head file declares all internal types and data.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 1999/05/22
*/

#ifndef GUI_INTERNALS_H
    #define GUI_INTERNALS_H

/******************* Internal data *******************************************/
#ifdef __uClinux__
  #define DEF_NR_TIMERS       8
  #define DEF_MSGQUEUE_LEN    8
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    16
  #define SIZE_QMSG_HEAP      8
#else
 #ifdef _LITE_VERSION
  #define DEF_NR_TIMERS       16
  #define DEF_MSGQUEUE_LEN    16
  #define SIZE_CLIPRECTHEAP   16
  #define SIZE_INVRECTHEAP    32
  #define SIZE_QMSG_HEAP      16
 #else
  #define DEF_NR_TIMERS       8
  #define DEF_MSGQUEUE_LEN    16
  #define SIZE_CLIPRECTHEAP   128
  #define SIZE_INVRECTHEAP    128
  #define SIZE_QMSG_HEAP      8
 #endif
#endif

/******************* Internal data of fix string module **********************/
#ifdef __uClinux__
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
#else
 #ifndef _LITE_VERSION
  #define MAX_LEN_FIXSTR      2048
  #define NR_HEAP             10
  #define LEN_BITMAP          (1+2+4+8+16+32+64+128+256+512)
 #else
  #define MAX_LEN_FIXSTR      64
  #define NR_HEAP             5
  #define LEN_BITMAP          (1+2+4+8+16)
 #endif
#endif

/******************** Handle type and child type. ***************************/
#define TYPE_HWND           0x01
    #define TYPE_MAINWIN    0x11
    #define TYPE_CONTROL    0x12
#define TYPE_HMENU          0x02
    #define TYPE_MENUBAR    0x21
    #define TYPE_PPPMENU    0x22
    #define TYPE_NMLMENU    0x23
#define TYPE_HACCEL         0x03
#define TYPE_HCURSOR        0x05
#define TYPE_HICON          0x07
#define TYPE_HDC            0x08
    #define TYPE_SCRDC      0x81
    #define TYPE_GENDC      0x82
    #define TYPE_MEMDC      0x83

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

typedef struct _ZORDERNODE
{
     HWND hWnd;			// Handle of window
 
     struct _ZORDERNODE* pNext;	// Next window
}ZORDERNODE;
typedef ZORDERNODE* PZORDERNODE;

typedef struct _ZORDERINFO
{
    int nNumber;		// Number of windows

    HWND hWnd;			// Handle of host window

    PZORDERNODE pTopMost;	// the top most Z order node
}ZORDERINFO;
typedef ZORDERINFO* PZORDERINFO;

typedef struct _SCROLLWINDOWINFO
{
    int iOffx;
    int iOffy;
    const RECT* rc1;
    const RECT* rc2;
}SCROLLWINDOWINFO;
typedef SCROLLWINDOWINFO* PSCROLLWINDOWINFO;

#define SBS_NORMAL      0x00
#define SBS_DISABLED    0x01
#define SBS_HIDE        0x02
typedef struct _SCROLLBARINFO {
    int  minPos;        // min value of scroll range.
    int  maxPos;        // max value of scroll range.
    int  curPos;        // current scroll pos.
    int  pageStep;      // steps per page.
    int  barStart;      // start pixel of bar.
    int  barLen;        // length of bar.
    BYTE status;        // status of scroll bar.
}SCROLLBARINFO;
typedef SCROLLBARINFO* PSCROLLBARINFO;

typedef struct _CARETINFO {
    int     x;              // position of caret
    int     y;
    void*   pNormal;        // normal bitmap.
    void*   pXored;         // bit-Xored bitmap.
    
    PBITMAP pBitmap;        // user defined caret bitmap.

    int     nWidth;         // original size of caret
    int     nHeight;
    int     nBytesNr;       // number of bitmap bytes.

#ifdef _USE_NEWGAL
    BITMAP  caret_bmp;      // bitmap of caret.
#else
    int     nEffWidth;      // effective width
    int     nEffHeight;     // effective height;
#endif

    BOOL    fBlink;         // does blink?
    BOOL    fShow;          // show or hide currently.

    HWND    hOwner;         // the window owns the caret.
    UINT    uTime;          // the blink time.
}CARETINFO;
typedef CARETINFO* PCARETINFO;

typedef struct _QMSG
{
    MSG                 Msg;
    struct _QMSG*       next;
}QMSG;
typedef QMSG* PQMSG;

typedef struct _MSGQUEUE MSGQUEUE;
typedef MSGQUEUE* PMSGQUEUE;

#ifndef _LITE_VERSION
typedef struct _SYNCMSG
{
    MSG              Msg;
    int              retval;
    sem_t            sem_handle;
    struct _SYNCMSG* pNext;
}SYNCMSG;
typedef SYNCMSG* PSYNCMSG;
#else
typedef BOOL (* IDLEHANDLER) (PMSGQUEUE msg_que);
void SetIdleHandler (HWND hwnd, IDLEHANDLER idle_handler);
#endif

// the MSGQUEUE struct is a internal struct.
// using semaphores to implement message queue.
struct _MSGQUEUE
{
    DWORD dwState;              // message queue states

#ifndef _LITE_VERSION
    pthread_mutex_t lock;       // lock
    sem_t wait;                 // wait semaphores
#endif

    PQMSG  pFirstNotifyMsg;     // head of the notify message queue
    PQMSG  pLastNotifyMsg;      // tail of the notify message queue

#ifndef _LITE_VERSION
    PSYNCMSG pFirstSyncMsg;     // head of the sync message queue
    PSYNCMSG pLastSyncMsg;      // tail of the sync message queue
#else
    IDLEHANDLER OnIdle;         // Idle handler
#endif

    MSG* msg;                   // post message buffer
    int len;                    // buffer len
    int readpos, writepos;      // positions for reading and writing

#ifdef _LITE_VERSION
    /*
     * One MiniGUI application can support at most 16 timers.
     */
    HWND TimerOwner[DEF_NR_TIMERS];  // Timer owner
    int  TimerID[DEF_NR_TIMERS];     // timer identifiers.
    WORD TimerMask;             // used timer slots mask
#else
    /*
     * One thread can only support eight timers.
     * And number of all timers in a MiniGUI applicatoin is 16.
     */
    HWND TimerOwner[DEF_NR_TIMERS];  // Timer owner
    int  TimerID[DEF_NR_TIMERS];     // timer identifiers.
    BYTE TimerMask;             // used timer slots mask
#endif

};

BOOL InitFreeQMSGList (void);
void DestroyFreeQMSGList (void);
BOOL InitMsgQueue (PMSGQUEUE pMsgQueue, int iBufferLen);
void DestroyMsgQueue (PMSGQUEUE pMsgQueue);

struct _wnd_element_data;

// this struct is an internal struct
typedef struct _MAINWIN
{
    /*
     * These fields are similiar with CONTROL struct.
     */
    short DataType;		// the data type.
    short WinType;		// the window type.

    int left, top;		// the position and size of main window.
    int right, bottom;

    int cl, ct;			// the position and size of client area.
    int cr, cb;

    DWORD dwStyle;		// the styles of main window.
    DWORD dwExStyle;    // the extended styles of main window.
    
    int iBkColor;		// the background color.
    HMENU hMenu;		// handle of menu.
    HACCEL hAccel;      // handle of accelerator table.
    HCURSOR hCursor;	// handle of cursor.
    HICON hIcon;		// handle of icon.
    HMENU hSysMenu;     // handle of system menu.
    PLOGFONT pLogFont;  // pointer to logical font.

    HDC   privCDC;      // the private client DC.
    INVRGN InvRgn;      // the invalid region of this main window.
    PGCRINFO pGCRInfo;  // pointer to global clip region info struct.
    PZORDERNODE pZOrderNode;
                        // the Z order node.

    PCARETINFO pCaretInfo;
                        // pointer to system caret info struct.

    DWORD dwAddData;    // the additional data.
    DWORD dwAddData2;   // the second addtional data.

    int (*MainWindowProc)(HWND, int, WPARAM, LPARAM);
                       	// the address of main window procedure.

    char* spCaption;    // the caption of main window.
    int   id;           // the identifier of main window.

    SCROLLBARINFO vscroll;
                        // the vertical scroll bar information.
    SCROLLBARINFO hscroll;
                        // the horizital scroll bar information.

    struct _MAINWIN* pMainWin;
                        // the main window that contains this window.
                        // for main window, always be NULL.

    HWND hParent;       // the parent of this window. 
                        // for main window, always be HWND_DESKTOP.
    
    /*
     * Child windows.
     */
    HWND hFirstChild;	// the handle of first child window.
    HWND hActiveChild;  // the currently active child window.
    HWND hOldUnderPointer;  // the old child window under pointer.
    HWND hPrimitive;    // the premitive child of mouse event.

    NOTIFPROC NotifProc;    // the notification callback procedure.

    /*
     * window element data.
     */
    struct _wnd_element_data* wed;
    
    /*
     * Main Window hosting. 
     * The following members are only implemented for main window.
     */
    struct _MAINWIN* pHosting;
                        // the hosting main window.
    struct _MAINWIN* pFirstHosted;
                        // the first hosted main window.
    struct _MAINWIN* pNextHosted;
                        // the next hosted main window.

    PMSGQUEUE pMessages;
                        // the message queue.

    GCRINFO GCRInfo;
                        // the global clip region info struct.
                        // put here to avoid invoking malloc function.

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    unsigned int scr_idx;
                        // idex for server clipping rect.
#endif

#ifndef _LITE_VERSION
    pthread_t th;		// the thread which creates this main window.
#endif

}MAINWIN;
typedef MAINWIN* PMAINWIN;

/************************* Initialization/Termination ************************/
#ifdef _LITE_VERSION
/*
int InitGUI (void);
void TerminateGUI (int rcByGUI);
*/

// Common for server and client.
BOOL InitDskMsgQueue (void);
void DestroyDskMsgQueue (void);

#ifdef _STAND_ALONE

BOOL IdleHandler4StandAlone (PMSGQUEUE msg_que);
BOOL StandAloneStartup (void);
void StandAloneCleanup (void);

#else

// Only for server.
BOOL IsOnlyMe (void);
void* LoadSharedResource (void);
void UnloadSharedResource (void);
BOOL IdleHandler4Server (PMSGQUEUE msg_que);
BOOL ServerStartup (void);
void ServerCleanup (void);

// Only for client.
void* AttachSharedResource (void);
void UnattachSharedResource (void);
BOOL IdleHandler4Client (PMSGQUEUE msg_que);
BOOL ClientStartup (void);
void ClientCleanup (void);

#endif

#endif

BOOL InitScreenDC (void);
void TerminateScreenDC (void);

BOOL InitGDI (void);
void TerminateGDI (void);

BOOL InitFixStr (void);
void TerminateFixStr (void);

BOOL InitMenu (void);
void TerminateMenu (void);

BOOL InitMainWinMetrics (void);
BOOL InitWindowElementColors (void);

BOOL InitDesktop (void);
void TerminateDesktop (void);

/* return main window contains hWnd. */
PMAINWIN GetMainWindowPtrOfControl (HWND hWnd);

/* check whether hWnd is main window and return pointer to main window hWnd. */
PMAINWIN CheckAndGetMainWindowPtr (HWND hWnd);

/* return the main window under specified point (x, y). */
PMAINWIN GetMainWindowPtrUnderPoint (int x, int y);

/* return message queue of window. */
PMSGQUEUE GetMsgQueue (HWND hWnd); 

/* return global clipping region of window. */
PGCRINFO GetGCRgnInfo (HWND hWnd); 

/* internal variables */
typedef struct _TRACKMENUINFO* PTRACKMENUINFO;

extern unsigned int __mg_timer_counter;

extern HWND __mg_capture_wnd;
extern HWND __mg_ime_wnd;
extern MSGQUEUE __mg_dsk_msgs;
extern PMAINWIN __mg_active_mainwnd;
extern PTRACKMENUINFO __mg_ptmi;

extern int DesktopWinProc (HWND hWnd, int message, WPARAM wParam, LPARAM lParam);

#ifdef _LITE_VERSION
extern int __mg_csrimgsize;
#ifdef _USE_NEWGAL
extern int __mg_csrimgpitch;
#endif
#else
extern pthread_mutex_t __mg_gdilock, __mg_mouselock;
extern pthread_t __mg_desktop, __mg_parsor, __mg_timer;
#endif

#ifdef __UCOSII__
  extern int __ucos2_init_malloc (void);
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_INTERNALS_H

