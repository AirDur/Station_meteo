
#include    "../common/44b.h"
#include    "../common/uhal.h"
#include    "api_led.h"
#include    "api_keyboard.h"
#include    "api_serial.h"
#include    "api_flash.h"
#include    "api_iis.h"
#include    "api_lcd.h"
#include    "api_timers.h"
#include    "api_rtc.h"

#include "../minigui/include/common.h"
#include "../minigui/include/minigui.h"
#include "../minigui/include/gdi.h"
#include "../minigui/include/window.h"
#include "../minigui/include/control.h"

extern unsigned char lib_icones_data[];
extern unsigned char icones_data[];

static MSG Msg;
static HWND hMainWnd1,hMainWnd2;
static MAINWINCREATE CreateInfo1,CreateInfo2;

static const char* en_text = "It is just a basic window\n to test if all is OK.";
static char msg_text [256];
static RECT msg_rc = {10, 50, 300, 80};
static RECT icone_rc = {200, 70, 232, 107};
static RECT desktop_rc = {10, 10, 310, 230};
static RECT test_icones_rc = {20, 10, 20+60*3, 20+60*4};
static char test_icones[12] = {84,14,13,49,56,34,25,82,66,65,87,51};
static const char* test_icones_text[12]={"Board","CPU","Ram","Serial","Net","Font","Backup","Wave","Power","Port","Time","Video"};
static const char* syskey = "";

static int last_key = -1;
static int last_key_count = 0;

int GUIAPI MyDialogBox (HWND hParentWnd, const char* pszText)
{
    CTRLDATA     CtrlData [2] = 
    {
        {"static", 
            WS_TABSTOP | WS_VISIBLE, 10, 10, 190, 140, 1000, 
            "BOARD FS44BOX-II\nCPU ARM7 66 MHz\n8 Mo SDRam\n2 Mo NOR FLASH\nEthernet 10 Mb/s\nUSB Dev\nPort IDE", 
            0L},
        {"button", 
            BS_PUSHBUTTON | WS_TABSTOP | WS_VISIBLE , 
            70, 140, 60, 25, 1000, "OK", 0L}
    };
    DLGTEMPLATE MsgBoxData = 
    {
        WS_CAPTION | WS_BORDER, WS_EX_NONE, 60, 10, 200, 200, pszText, 0, 0, 2, CtrlData, 0L
    };

    MsgBoxData.controls = CtrlData;

    return DialogBoxIndirectParam (&MsgBoxData, hParentWnd, DefaultDialogProc, 0 );
}


static int WinProc2(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rc;
    HICON my_icone;
    int i;
    syskey = "";
   
    switch (message) {
        case MSG_CREATE:
            break;
           
        case MSG_PAINT:
            hdc = BeginPaint (hWnd);
            for(i=0;i<12;i++)
               {
               my_icone = LoadIconFromMem (hdc, lib_icones_data + 766*test_icones[i], 0);
               DrawIcon(hdc,test_icones_rc.left + 60*(i%4), test_icones_rc.top + 60*(i/4), 32, 32, my_icone);
               DestroyIcon (my_icone);
               TextOut (hdc, test_icones_rc.left + 60*(i%4), test_icones_rc.top + 35 + 60*(i/4), test_icones_text[i]);
               }
            EndPaint (hWnd, hdc);
            return 0;

        case MSG_LBUTTONDOWN:
            break;

        case MSG_LBUTTONUP:
            //MyDialogBox (hWnd, "Board description");
            break;   

        case MSG_CLOSE:
            DestroyMainWindow (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static int WinProc1(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rc;
    syskey = "";
    HICON my_icone;
    static int id_icone = 0;
    static HWND HWND_BUTTON;
    static HWND HWND_EDIT;
   
    switch (message) {
        case MSG_CREATE:
            strcpy (msg_text, "No message so far.");
            HWND_BUTTON=CreateWindowEx("BUTTON","TEST",WS_CHILD | WS_VISIBLE,0,100,
            20, 90, 60, 25,hWnd, 0);
            HWND_EDIT=CreateWindowEx("EDIT","TEST",WS_CHILD | WS_BORDER | WS_VISIBLE,0,101,
            100, 90, 60, 25,hWnd, 0);
            break;
            
        case MSG_LBUTTONDOWN:
            strcpy (msg_text, "The left button pressed.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            InvalidateRect (hWnd, &icone_rc, TRUE);
            break;

        case MSG_LBUTTONUP:
            id_icone ++;
            strcpy (msg_text, "The left button released.");
            InvalidateRect (hWnd, &msg_rc, TRUE);
            InvalidateRect (hWnd, &icone_rc, TRUE);
            break;

        case MSG_PAINT:
            hdc = BeginPaint (hWnd);

            rc.left = 10; rc.top = 10;
            rc.right = 300; rc.bottom = 40;
            DrawText (hdc, en_text, -1, &rc, DT_LEFT | DT_WORDBREAK );
            DrawText (hdc, msg_text, -1, &msg_rc, DT_LEFT  | DT_WORDBREAK );
            my_icone = LoadIconFromMem (hdc, lib_icones_data + 766*(id_icone % 100), 0);
            DrawIcon(hdc,icone_rc.left, icone_rc.top, 32, 32, my_icone);
            DestroyIcon (my_icone);

            EndPaint (hWnd, hdc);
            return 0;

        case MSG_CLOSE:
            MessageBox (hWnd, "EXIT!","WARNING", MB_ICONEXCLAMATION);
            DestroyMainWindow (hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int DemoDesktopInit(void)
{   
    InitGUI ();
    
    SetDesktopRect(0, 0, 320, 240);

    Delay(30000);

    MessageBox (HWND_DESKTOP, "MINIGUI DEMO PROGRAM V1.0\n PRAGMATEC S.A.R.L.\n BUTTON 1 : Mouse X\n BUTTON 2 : Mouse Y\n BUTTON 3 : Mouse Click\n\nSTILL UNDER DEVELOPPEMENT.","BIENVENUE!", MB_ICONINFORMATION);
    
    CreateInfo1.dwStyle = WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CAPTION;
    CreateInfo1.dwExStyle = WS_EX_NONE;
    CreateInfo1.spCaption = "Hello, world!";
    CreateInfo1.hMenu = 0;
    CreateInfo1.hCursor = GetSystemCursor(0);
    CreateInfo1.hIcon = GetSmallSystemIcon (0);
    CreateInfo1.MainWindowProc = WinProc1;
    CreateInfo1.lx = 50;
    CreateInfo1.ty = 50;
    CreateInfo1.rx = 300;
    CreateInfo1.by = 200;
    CreateInfo1.iBkColor = COLOR_lightwhite;
    CreateInfo1.dwAddData = 0;
    CreateInfo1.hHosting = HWND_DESKTOP;

    hMainWnd1 = CreateMainWindow (&CreateInfo1);
        
    if (hMainWnd1 == HWND_INVALID)
        return -1;
        
    CreateInfo2.dwStyle = WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION;
    CreateInfo2.dwExStyle = WS_EX_NONE;
    CreateInfo2.spCaption = "HARDWARE TEST";
    CreateInfo2.hMenu = 0;
    CreateInfo2.hCursor = GetSystemCursor(0);
    CreateInfo2.hIcon = GetSmallSystemIcon (4);
    CreateInfo2.MainWindowProc = WinProc2;
    CreateInfo2.lx = 10;
    CreateInfo2.ty = 20;
    CreateInfo2.rx = 280;
    CreateInfo2.by = 230;
    CreateInfo2.iBkColor = COLOR_lightwhite;
    CreateInfo2.dwAddData = 0;
    CreateInfo2.hHosting = hMainWnd1;

    hMainWnd2 = CreateMainWindow (&CreateInfo2);
        
    if (hMainWnd2 == HWND_INVALID)
        return -1;
   
    ShowWindow(hMainWnd2, SW_SHOWNORMAL);
    ShowWindow(hMainWnd1, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd1))
    {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);        
    }

   MainWindowThreadCleanup (hMainWnd1); 
   TerminateGUI(0);
}   

