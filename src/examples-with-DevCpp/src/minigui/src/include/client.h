/*
** $Id: client.h,v 1.19 2003/12/25 10:58:22 weiym Exp $
**
** client.h: routines for client.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_CLIENT_H
    #define GUI_CLIENT_H

#define CL_PATH "/var/tmp/"

#define REQID_LOADCURSOR        0x0001
#define REQID_CREATECURSOR      0x0002
#define REQID_DESTROYCURSOR     0x0003
#define REQID_CLIPCURSOR        0x0004
#define REQID_GETCLIPCURSOR     0x0005
#define REQID_SETCURSOR         0x0006
#define REQID_GETCURRENTCURSOR  0x0007
#define REQID_SHOWCURSOR        0x0008
#define REQID_SETCURSORPOS      0x0009
#define REQID_LAYERINFO         0x000A
#define REQID_JOINLAYER         0x000B
#define REQID_TOPMOSTLAYER      0x000C
#define REQID_ACTIVECLIENT      0x000D
#define REQID_IAMLIVE           0x000E
#define REQID_OPENIMEWND        0x000F

#ifdef _USE_NEWGAL
#define REQID_HWSURFACE         0x0010
#endif

#ifdef _CLIPBOARD_SUPPORT
#define REQID_CLIPBOARD         0x0011
#endif

typedef struct LayerInfo {
    GHANDLE handle;
    int nr_clients;
    int cli_active;
    BOOL is_topmost;
    RECT max_rect;
} LAYERINFO;

typedef struct JoinLayerInfo
{
    char layer_name [LEN_LAYER_NAME + 1];
    char client_name [LEN_CLIENT_NAME + 1];
    RECT desktop_rc;
} JOINLAYERINFO;

typedef struct JoinedClientInfo
{
    GHANDLE layer_handle;
    RECT desktop_rc;
} JOINEDCLIENTINFO;

#ifndef _MGUI_MINIGUI_H
typedef struct tagREQUEST {
    int id;
    const void* data;
    size_t len_data;
} REQUEST;
typedef REQUEST* PREQUEST;
#endif

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MINIGUI_H
int cli_conn (const char* name, char project);
int cli_request (PREQUEST request, void* result, int len_rslt);
#endif

void set_select_timeout (unsigned int usec);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_CLIENT_H

