/**
 * \file minigui.h
 * \author Wei Yongming <ymwei@minigui.org>
 * \date 2002/01/06
 * 
 * This file includes global and miscellaneous interfaces of MiniGUI.
 *
 \verbatim
    Copyright (C) 2002-2005 Feynman Software.
    Copyright (C) 1998-2002 Wei Yongming.

    This file is part of MiniGUI, a compact cross-platform Graphics 
    User Interface (GUI) support system for real-time embedded systems.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    If you are using MiniGUI for developing commercial, proprietary, or other
    software not covered by the GPL terms, you must have a commercial license
    for MiniGUI. Please see http://www.minigui.com/product/index.html for 
    how to obtain this. If you are interested in the commercial MiniGUI 
    licensing, please write to sales@minigui.com. 

 \endverbatim
 */

/*
 * $Id: minigui.h,v 1.107 2005/02/15 05:00:08 weiym Exp $
 * 
 *             MiniGUI for Linux/uClinux, eCos, uC/OS-II, VxWorks, 
 *                     and ThreadX version 1.6.x
 *             Copyright (C) 2002-2005 Feynman Software.
 *             Copyright (C) 1998-2002 Wei Yongming.
 */

#ifndef _MGUI_MINIGUI_H
  #define _MGUI_MINIGUI_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

    /**
     * \addtogroup global_vars Global variables
     * @{
     */

    /**
     * \defgroup rect_vars Global Rectangles
     * @{
     */

/**
 * \var RECT g_rcScr
 * \brief Contains the rectangle of the whole screen.
 */
extern RECT g_rcScr;         /* The RECT of screen. */

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)

/**
 * \var RECT g_rcDesktop
 * \brief Contains the rectangle of desktop of the application.
 *
 * \note Only available for MiniGUI-Lite as an actual global variable.
 * And \a g_rcDesktop is defined as an alias (macro) of \a g_rcScr for MiniGUI-Threads.
 *
 * \sa g_rcScr
 */
extern RECT g_rcDesktop;     /* The RECT of desktop. */

#else
#define g_rcDesktop     g_rcScr
#endif

    /** @} end of rect_vars */

    /**
     * \defgroup lite_vars MiniGUI-Lite specific variables
     * @{
     */

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)

#include <sys/types.h>

/**
 * \var BOOL mgIsServer
 * \brief Indicates whether the process is the server or a client on MiniGUI-Lite.
 *
 * \note Only defined for MiniGUI-Lite.
 */
extern BOOL mgIsServer;      /* Is the server or a client. */

/**
 * \var void* mgSharedRes
 * \brief Contains the pointer to the shared resource of MiniGUI-Lite.
 *
 * \note Not defined for MiniGUI-Threads, and the shared resource is
 * read-only for all clients.
 *
 * \sa mgSizeRes
 */
extern void* mgSharedRes;    /* The pointer to shared resource. */

/**
 * \var void* mgSizeRes
 * \brief Contains the length of shared resource of MiniGUI-Lite.
 *
 * \note Only defined for MiniGUI-Lite. 
 *
 * \sa mgSharedRes
 */
extern size_t mgSizeRes;     /* The size of shared resource. */

#define LEN_LAYER_NAME      14
#define LEN_CLIENT_NAME     14

#define INV_LAYER_HANDLE    0

/* variables only available for the server */

struct MG_Layer;

/** Client information. */
typedef struct MG_Client
{
    /** The name of the client. */
    char    name [LEN_CLIENT_NAME + 1];

    /** PID of the client process. */
    pid_t   pid;
    /** UID of the client process. */
    uid_t   uid;
    /** The file descriptor of the socket connected to the client. */
    int     fd;

    /** The desktop rectangle of the client. */
    RECT    rc;
    /** The last active tick count of the client. */
    DWORD   last_live_time;
    /** The additional data of the client. */
    DWORD   dwAddData;

    /** The pointer to the next client in the same layer. */
    struct  MG_Client* next;
    /** The pointer to the previous client in the same layer. */
    struct  MG_Client* prev;

    /** The pointer to the layer on which the client lays. */
    struct  MG_Layer* layer;

    struct GlobalRes* global_res;
} MG_Client;

struct _CLIPRGN;

/** Layer information. */
typedef struct MG_Layer
{
    /** The name of the layer. */
    char    name [LEN_LAYER_NAME + 1];

    /** The pointer to the list of clients which lay on the layer. */
    MG_Client* cli_head;
    /** The pointer to the active client on the layer. */
    MG_Client* cli_active;
    /** The additional data of the layer. */
    DWORD   dwAddData;

    /** The pointer to the next layer. */
    struct MG_Layer* next;
    /** The pointer to the previous layer. */
    struct MG_Layer* prev;

    /** The pointer to the region contains the spare rectangles of the layer. */
    struct _CLIPRGN* spare_rects;
} MG_Layer;

/**
 * \var int mgClientSize
 * \brief The current size of the array \a mgClients.
 *
 * \sa mgClients
 */
extern int mgClientSize;

/**
 * \var MG_Client* mgClients
 * \brief The pointer to the array contains all clients' information.
 *
 * You can access the elements in \a mgClients as a normal array. If the 
 * field \a fd of one element is not less than zero, then the element
 * will be a vaild client.
 *
 * \sa MG_Client
 */
extern MG_Client* mgClients;

/**
 * \var MG_Layer* mgTopmostLayer
 * \brief The pointer to the topmost layer.
 *
 * \sa MG_Layer
 */
extern MG_Layer* mgTopmostLayer;

/**
 * \var MG_Layer* mgLayers
 * \brief The pointer to the list of layers.
 *
 * \sa MG_Layer
 */
extern MG_Layer* mgLayers;

#endif /* _LITE_VERSION && !_STAND_ALONE */

    /** @} end of lite_vars */

    /** @} end of global_vars */

//#ifndef _LITE_VERSION
MG_EXPORT BOOL GUIAPI InitGUI (void);
MG_EXPORT void GUIAPI TerminateGUI (int rcByGUI);
MG_EXPORT void GUIAPI MiniGUIPanic (int exitcode);
//#endif

    /**
     * \addtogroup fns Functions
     * @{
     */

    /**
     * \addtogroup global_fns Global/general functions
     * @{
     */

    /**
     * \defgroup lite_fns MiniGUI-Lite specific functions
     * @{
     */

#ifdef _LITE_VERSION

    /**
     * \defgroup lite_listenfd_fns Listening a file descriptor
     *
     * Register/Unregister a listen fd to MiniGUI.
     *
     * When you need to listen a file descriptor, you can use \a select(2)
     * system call. In MiniGUI, you can also register it to MiniGUI to 
     * be a listened fd, and when there is a read/write/except event on 
     * the registered fd , MiniGUI will sent a notification message to 
     * the registered window.
     *
     * Example:
     *
     * \include listenfd.c
     *
     * @{
     */

#define MAX_NR_LISTEN_FD   5

/** 
 * \fn BOOL GUIAPI RegisterListenFD (int fd, int type, HWND hwnd, void* context)
 * \brief Registers a listened file descriptor to MiniGUI-Lite.
 * This function registers the file desciptor \a fd to MiniGUI-Lite for listening.
 *
 * When there is a read/write/except event on this \a fd, MiniGUI
 * will post a MSG_FDEVENT message with wParam being equal to
 * MAKELONG (fd, type), and the lParam being set to \a context
 * to the target window.
 *
 * \param fd The file descriptor to be listened.
 * \param type The type of the event to be listened, can be POLLIN, POLLOUT, or POLLERR.
 * \param hwnd The handle to the window will receive MSG_FDEVENT message.
 * \param context The value will be passed to the window as lParam of MSG_FDEVENT message.
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available on MiniGUI-Lite.
 *
 * \sa UnregisterListenFD, system_msgs
 */
MG_EXPORT BOOL GUIAPI RegisterListenFD (int fd, int type, HWND hwnd, void* context);

/** 
 * \fn BOOL GUIAPI UnregisterListenFD (int fd)
 * \brief Unregisters a being listened file descriptor.
 *
 * This function unregisters the being listened file descriptor \a fd.
 *
 * \param fd The file descriptor to be unregistered, should be a being 
 *        listened file descriptor.
 * \return TRUE if all OK, and FALSE on error.
 *
 * \note Only available on MiniGUI-Lite.
 *
 * \sa RegisterListenFD
 */
MG_EXPORT BOOL GUIAPI UnregisterListenFD (int fd);

    /** @} end of lite_listenfd_fns */

#ifdef _STAND_ALONE

#define SetDesktopRect(lx, ty, rx, by) {}

#else

    /**
     * \defgroup lite_layer_fns Layer operations
     *
     * A client in MiniGUI-Lite can create a new layer or join an existed layer 
     * in order to get the visible desktop rectangle on the screen of it.
     *
     * Example:
     *
     * \include client_startup.c
     *
     * @{
     */

/** 
 * \fn GHANDLE GUIAPI JoinLayer (const char* layer_name, const char* client_name, int lx, int ty, int rx, int by)
 * \brief Joins to a layer.
 *
 * This function should be called by clients before calling any other MiniGUI
 * functions. You can call \a GetLayerInfo to get the layer information.
 * If the layer to be joined does not exist, the server, i.e. \a mginit, will
 * try to create a new one. If you passed a NULL pointer or a null string for 
 * \a layer_name, a new layer will be created as well.
 *
 * For the server of MiniGUI-Lite, this function will ignore the arguments of
 * \a layer_name and \a client_name. The rectangle defines a region in the 
 * screen, which is exclusively used by the server, no client can output
 * to this exclusive retangle.
 *
 * The server usually calls SetDesktopRect macro, which is defined as
 * the following:
 *
 * \code
 *
 * #define SetDesktopRect(lx, ty, rx, by) JoinLayer ("", "", lx, ty, rx, by)
 *
 * \endcode
 * 
 * Note that the server can define the exclusive retangle out of the actual
 * screen range.
 * 
 * \param layer_name The name of the layer.
 * \param client_name The name of the client.
 * \param lx lx,ty,rx,by: The expected desktop rect of the client.
 * \param ty lx,ty,rx,by: The expected desktop rect of the client.
 * \param rx lx,ty,rx,by: The expected desktop rect of the client.
 * \param by lx,ty,rx,by: The expected desktop rect of the client.
 * \return The handle to the layer on success, INV_LAYER_HANDLE on error.
 *
 * \sa GetLayerInfo, GetDesktopRect, ServerStartup
 */
MG_EXPORT GHANDLE GUIAPI JoinLayer (const char* layer_name, const char* client_name, 
                int lx, int ty, int rx, int by);

#define SetDesktopRect(lx, ty, rx, by) \
        JoinLayer ("", "", lx, ty, rx, by)

/** 
 * \fn void GUIAPI GetDesktopRect (int* lx, int* ty, int* rx, int* by)
 * \brief Gets the desktop rectangle.
 *
 * After joined to a layer, client can call this function to get the
 * actual desktop rect of itself. 
 *
 * \param lx lx,ty,rx,by: The desktop rect will be returned through these pointers.
 * \param ty lx,ty,rx,by: The desktop rect will be returned through these pointers.
 * \param rx lx,ty,rx,by: The desktop rect will be returned through these pointers.
 * \param by lx,ty,rx,by: The desktop rect will be returned through these pointers.
 *
 * \sa JoinLayer
 */
MG_EXPORT void GUIAPI GetDesktopRect (int* lx, int* ty, int* rx, int* by);

#define NAME_SELF_LAYER     ""

/** 
 * \fn GHANDLE GUIAPI GetLayerInfo (const char* layer_name, RECT* max_rect, int* nr_clients, BOOL* is_topmost, int* cli_active)
 * \brief Gets information of a layer.
 *
 * You can get the information of a layer through this function. 
 * The information will be returned through the pointer arguments 
 * if the specific pointer is not NULL.
 *
 * \param layer_name The name of the layer.
 * \param max_rect The max desktop rect can be obtained will be returned through this pointer.
 * \param nr_clients The number of clients in the layer will be returned through this pointer.
 * \param is_topmost A boolean which indicates whether the layer is the topmost layer will be returned.
 * \param cli_active The identifier of the active client in the layer.
 * \return Returns the handle to the layer on success, INV_LAYER_HANDLE on error.
 *
 * \sa JoinLayer
 */
MG_EXPORT GHANDLE GUIAPI GetLayerInfo (const char* layer_name, RECT* max_rect, 
                int* nr_clients, BOOL* is_topmost, int* cli_active);

/** 
 * \fn BOOL GUIAPI BringLayer2Topmost (GHANDLE handle)
 * \brief Brings a layer to be the topmost one.
 *
 * This function brings the specified layer \a handle to be the topmost layer.
 *
 * \param handle The handle to the layer.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa SetActiveClient
 */
MG_EXPORT BOOL GUIAPI BringLayer2Topmost (GHANDLE handle);

/** 
 * \fn BOOL GUIAPI SetActiveClient (int active)
 * \brief Sets a client as the ative one.
 *
 * This function sets the specified client \a active to be the active one.
 * It also bring the layer in which the client lays to be the topmost as well.
 *
 * \param active The identifier of the client.
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa BringLayer2Topmost
 */
MG_EXPORT BOOL GUIAPI SetActiveClient (int active);

    /** @} end of lite_layer_fns */

    /**
     * \defgroup lite_server_fns Server-only operations
     *
     * MiniGUI provides some server-only functions for you to create a
     * customized server for MiniGUI-Lite, i.e. \a mginit.
     *
     * Example:
     *
     * \include server_startup.c
     *
     * @{
     */

#define LCO_NEW_CLIENT      1
#define LCO_DEL_CLIENT      2

/**
 * \var typedef void (* ON_NEW_DEL_CLIENT) (int op, int cli)
 * \brief Client event callback.
 *
 * \sa OnNewDelClient, OnChangeLayer
 */
typedef void (* ON_NEW_DEL_CLIENT) (int op, int cli);

#define LCO_NEW_LAYER       1
#define LCO_DEL_LAYER       2
#define LCO_JOIN_CLIENT     3
#define LCO_REMOVE_CLIENT   4
#define LCO_TOPMOST_CHANGED 5
#define LCO_ACTIVE_CHANGED  6

/**
 * \var typedef void (* ON_CHANGE_LAYER) (int op, MG_Layer* layer, MG_Client* client)
 * \brief Layer event callback.
 *
 * \sa OnNewDelClient, OnChangeLayer
 */
typedef void (* ON_CHANGE_LAYER) (int op, MG_Layer* layer, MG_Client* client);

/**
 * \var ON_NEW_DEL_CLIENT OnNewDelClient
 * \brief Sets to a function to handle a comming in/going away connection of client.
 *
 * When a client is connecting to or disconnecting from the server, MiniGUI
 * will call this function to tell you the event and the client identifier. 
 * The event could be one of the following:
 *
 *  - LCO_NEW_CLIENT\n
 *    A new client is connecting to the server.
 *  - LCO_DEL_CLIENT\n
 *    A new client is disconnecting from the server.
 *
 * The event will be passed through the argument of \a op, and the client
 * identifier will be passed through the argument of \a cli.
 * You can get the information of the client by accessing \a mgClients with \a cli.
 *
 * \note Only available for the server of MiniGUI-Lite.
 * 
 * \sa ON_NEW_DEL_CLIENT, mgClients
 */
extern ON_NEW_DEL_CLIENT OnNewDelClient;

/**
 * \var ON_CHANGE_LAYER OnChangeLayer
 * \brief Sets to a function to handle events of layers.
 *
 * When a layer is changing, MiniGUI will call this function to tell 
 * you the event and the layer or the client which leads to the event.
 * The event could be one of the following:
 *
 *  - LCO_NEW_LAYER\n
 *    A new layer is creating.
 *  - LCO_DEL_LAYER\n
 *    A new layer is deleting.
 *  - LCO_JOIN_CLIENT\n
 *    A client is joining to the layer.
 *  - LCO_REMOVE_CLIENT\n
 *    A client is removing from the layer.
 *  - LCO_TOPMOST_CHANGED\n
 *    The topmost layer changed, the layer will be the topmost one.
 *  - LCO_ACTIVE_CHANGED\n
 *    The active client changed, the client will be the active one.
 *
 * The event will be passed through the argument of \a op, and the pointers to the relevant
 * layer and client will be passed through the argument of \a layer and \a client respectively.
 *
 * \note Only available for the server of MiniGUI-Lite.
 * 
 * \sa ON_NEW_DEL_CLIENT, mgClients
 */
extern ON_CHANGE_LAYER OnChangeLayer;

/** 
 * \fn BOOL GUIAPI ServerStartup (void)
 * \brief Initializes the server of MiniGUI-Lite.
 *
 * This function initializes the server, i.e. \a mginit. It creates
 * the shared resource, the listening socket, and other internal objects.
 * Your costomized \a mginit program should call this function before calling
 * any other function.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function, i.e. \em only can be called by \a mginit.
 */
MG_EXPORT BOOL GUIAPI ServerStartup (void);

/** 
 * \fn BOOL SetClientScreen (int lx, int ty, int rx, int by)
 * \brief Sets the screen rectangle can be used by clients.
 *
 * This function sets the screen rectangle can be used by clients.
 * All clients' drawing will be clipped out of the rectangle.
 *
 * The rectangle set by this function should be a subrectangle of
 * the server's exclusive rectangle defined by \a SetDesktopRect.
 *
 * \param lx lx,ty,rx,by: Specifies the screen rectangle.
 * \param ty lx,ty,rx,by: Specifies the screen rectangle.
 * \param rx lx,ty,rx,by: Specifies the screen rectangle.
 * \param by lx,ty,rx,by: Specifies the screen rectangle.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function, i.e. \em ONLY can be called by \a mginit.
 *
 * \note This function do nothing in MiniGUI v1.5.x and later.
 *
 * \sa JoinLayer
 */
static inline BOOL SetClientScreen (int lx, int ty, int rx, int by)
{
    return TRUE;
}

/** 
 * \fn BOOL GUIAPI OnlyMeCanDraw (void)
 * \brief Tells clients do not draw anything on screen.
 *
 * If the server want to output something out of its exclusive rectangle,
 * it can call this function to disable the clients' any drawing output.
 * When the server done, it can call \a ClientCanDrawNowEx function
 * to tell clients in the topmost layer to repaint themselves.
 *
 * Note that the clients is still running after the server calling 
 * this function.
 * 
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa ClientCanDrawNowEx, ClientCanDrawNow
 */
MG_EXPORT BOOL GUIAPI OnlyMeCanDraw (void);

/** 
 * \fn BOOL GUIAPI ClientCanDrawNowEx (BOOL bRepaint, const RECT* invrc)
 * \brief Tells clients that they can output to screen now.
 *
 * \param bRepaint Whether to repaint the clients in the topmost layer.
 * \param invrc The invalid screen rect. It can be NULL, 
 *        indicates the whole desktop of clients should be repainted.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa OnlyMeCanDraw, ClientCanDrawNow
 */
MG_EXPORT BOOL GUIAPI ClientCanDrawNowEx (BOOL bRepaint, const RECT* invrc);

/** 
 * \def ClientCanDrawNow()
 * \brief Tells clients that they can output to screen now, and 
 *        notify clients to repaint the whole desktop.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function, and defined as a macro 
 *       calling \a ClientCanDrawNowEx with \a bRepaint is TRUE and \a invrc is NULL.
 *
 * \sa ClientCanDrawNowEx
 */
#define ClientCanDrawNow()    ClientCanDrawNowEx (TRUE, NULL)

/**
 * \fn void GUIAPI UpdateTopmostLayer (const RECT* dirty_rc)
 * \brief Tells the clients in the topmost layer to update their windows.
 *
 * \param dirty_rc The dirty rectangle in screen coordinate system.
 *
 * \note Server-only function.
 *
 * \sa OnlyMeCanDraw, ClientCanDrawNowEx
 */
MG_EXPORT void GUIAPI UpdateTopmostLayer (const RECT* dirty_rc);

/** 
 * \fn BOOL GUIAPI SetTopMostClient (int cli)
 * \brief Sets topmost layer by a client identifier.
 *
 * This function sets the topmost layer by the specified client identifier \a cli.
 * It will bring the layer contains the client to be the topmost one.
 *
 * \param cli The identifier of the client.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa SetTopMostLayer, BringLayer2Topmost
 */
MG_EXPORT BOOL GUIAPI SetTopMostClient (int cli);

/** 
 * \fn BOOL GUIAPI SetTopMostLayer (MG_Layer* layer)
 * \brief Sets topmost layer.
 *
 * This functions sets the specified layer \a layer to be the topmost layer.
 *
 * \param layer The pointer to the layer.
 * \return TRUE on success, otherwise FALSE.
 *
 * \note Server-only function.
 *
 * \sa SetTopMostClient, BringLayer2Topmost
 */
MG_EXPORT BOOL GUIAPI SetTopMostLayer (MG_Layer* layer);

/** 
 * \fn int GUIAPI GetClientByPID (int pid)
 * \brief Returns the client identifier from PID of a client.
 *
 * This function gets the identifier of the sepcified client from the PID of it.
 *
 * \param pid The process ID of the client.
 * \return The client identifier on success, less than 0 on error.
 *
 * \note Server-only function.
 */
MG_EXPORT int GUIAPI GetClientByPID (int pid);

    /** @} end of lite_server_fns */

    /**
     * \defgroup lite_request_fns Simple request/reply interfaces
     * 
     * You can register a customized request handler to extend your server, i.e. 
     * \a mginit, of MiniGUI-Lite.
     *
     * A request consists of an identifier and the data associated with the request.
     * The identifier is used by MiniGUI to determine which handler should be called
     * when a request arrives. When MiniGUI finds one handler, it will call the handler
     * and pass the socket fd connected to the client, the data associated with the request,
     * and the length of the data. Eventually, the handler will sent the reply to
     * the client.
     *
     * After register a customized request handler in your server, you can call 
     * \a cli_request function in the client to send a request to 
     * the server and wait for the reply. On the other hand, the request handler in the server
     * will receive the request and call \a send_reply to send the reply to the client.
     * In this way, you can create a simple IPC (inter-process conmmunication) 
     * mechanism between clients and the server.
     *
     * Example:
     *
     * \include request.c
     *
     * @{
     */

/**
 * \def MAX_SYS_REQID
 * \brief Maximal system reserved request identifier.
 *
 * \sa RegisterRequestHandler
 */
#define MAX_SYS_REQID           0x0011

/**
 * \def MAX_REQID
 * \brief Maximal request identifier.
 *
 * \sa RegisterRequestHandler
 */
#define MAX_REQID               0x0018

/** A request will be sent to the server of MiniGUI-Lite. */
typedef struct _REQUEST {
    /** The identifier of the type of the request. */
    int id;
    /** The data will be sent to the server. */
    const void* data;
    /** The length of the data. */
    size_t len_data;
} REQUEST;
typedef REQUEST* PREQUEST;

/** 
 * \fn cli_request (PREQUEST request, void* result, int len_rslt)
 * \brief Sends a request to the server and wait reply.
 *
 * If \a result is NULL or \a len_rslt is zero, the function will return 
 * immediately after sent the data to the server.
 *
 * \param request The pointer to REQUEST, which contains the data of the request.
 * \param result The buffer receives the reply.
 * \param len_rslt The lenght of the buffer.
 * \return Zero on success, no-zero on error.
 *
 * \note Only used by clients to send a request to the server of MiniGUI-Lite.
 *
 * \sa send_reply
 */
int cli_request (PREQUEST request, void* result, int len_rslt);

/** 
 * \fn int get_sock_fd2srv (void)
 * \brief Gets the file descriptor of the socket connected to the server.
 *
 * This function returns the file descriptor of the socket connected to the server,
 * i.e. \a mginit.
 *
 * \return The file descriptor of the socket connected to the server.
 *
 * \note Only used by clients, no meaning for the server.
 */
int get_sock_fd2srv (void);

/** 
 * \fn send_reply (int clifd, const void* reply, int len)
 * \brief Sends the reply to the client.
 *
 * This function sends a replay pointed to by \a reply which is 
 * \a len bytes long to the client.
 *
 * \note Only used by the server to send the reply to the client.
 * This function typically called in your customized request handler.
 *
 * \param clifd The fd connected to the client.
 * \param reply The buffer contains the reply data.
 * \param len The length of the reply data in bytes.
 * \return Zero on success, no-zero on error.
 *
 * \sa cli_request, RegisterRequestHandler
 */
int send_reply (int clifd, const void* reply, int len);

/**
 * \var typedef int (* REQ_HANDLER)(int cli, int clifd, void* buff, size_t len)
 * \brief Request handler.
 *
 * \sa RegisterRequestHandler
 */
typedef int (* REQ_HANDLER) (int cli, int clifd, void* buff, size_t len);

/** 
 * \fn BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler)
 * \brief Registers a customize request handler.
 *
 * This function registers a request handler to the server, i.e. \a mginit.
 *
 * \param req_id The identifier of the customized request.
 * \param your_handler The handler of the request. Being NULL to unregister the request handler.
 * \return TRUE on success, FALSE on error.
 *
 * \note Only used by the server to register a request handler.
 *       And the identifier should be larger than \a MAX_SYS_REQID and 
 *       less than or equal to \a MAX_REQID.
 *
 * \sa cli_request, send_reply, MAX_SYS_REQID, MAX_REQID
 */
MG_EXPORT BOOL GUIAPI RegisterRequestHandler (int req_id, REQ_HANDLER your_handler);

/** 
 * \fn EQ_HANDLER GUIAPI GetRequestHandler (int req_id)
 * \brief Gets the request handler by request identifier.
 *
 * This function returns the request handler of the specified request identifier \a req_id.
 *
 * \param req_id The request identifier.
 * \return The handler on success, NULL on error.
 *
 * \note Only can be used by the server.
 *
 * \sa RegisterRequestHandler
 */
MG_EXPORT REQ_HANDLER GUIAPI GetRequestHandler (int req_id);

    /** @} end of lite_request_fns */

    /**
     * \defgroup lite_socket_fns General socket operations
     *
     * MiniGUI-Lite uses UNIX domain socket to build the communication
     * between the server and the clients.
     *
     * You can also use the underlay interfaces which MiniGUI uses to create
     * your own UNIX domain socket.
     *
     * Example:
     *
     * \include socket.c
     *
     * @{
     */

/**
 * \fn int serv_listen (const char* name)
 * \brief Creates a listen socket.
 *
 * This function is used by the server to create a listening socket. 
 * Any MiniGUI-Lite application can call this function to create a 
 * listening socket. The server, i.e. \a mginit, of MiniGUI-Lite uses 
 * this function to create its listening socket, and named the socket 
 * to '/var/tmp/minigui'.
 *
 * \param name The path name of the listening socket.
 * \return The file discriptor of the listening socket created, -1 on error.
 *
 * \note As a convention, you should located the socket in '/var/tmp/' directory.
 */
int serv_listen (const char* name);

/**
 * \fn int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr)
 * \brief Waits for a client connection to arrive, and accept it.
 *
 * This function is used by the server to wait a connection and accept it.
 *
 * After creating a listening socket by calling \a serv_listen, you can call this
 * function to create a connection with a client. We also obtain the client's PID 
 * and UID from the pathname that it must bind before calling us.
 *
 * \param listenfd The fd of listen socket.
 * \param pidptr The client PID will be saved to this buffer when this function returns.
 * \param uidptr The client UID will be saved to this buffer when this function returns.
 * \return The new connected fd if all OK, < 0 on error.
 *
 * \sa serv_listen, cli_conn
 */
int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr);

/**
 * \fn int cli_conn (const char* name, char project)
 * \brief Used by clients to connect to a server.
 *
 * This function is used by clients to connect to a server.
 *
 * The created socket will be located at the directory '/var/tmp',
 * and with name of '/var/tmp/xxxxx-c', where 'xxxxx' is the pid of client.
 * and 'c' is a character to distinguish different projects.
 * MiniGUI itself uses 'a' as the project character to create socket between
 * 'mginit' and clients.
 *
 * \param name The name of the well-known listen socket (created by server).
 * \param project A character to distinguish different projects (Do \em NOT use 'a').
 * \return The new connected fd if all OK, < 0 on error.
 *
 * \sa serv_listen, serv_accept
 */
int cli_conn (const char* name, char project);

#define SOCKERR_IO          -1
#define SOCKERR_CLOSED      -2
#define SOCKERR_INVARG      -3
#define SOCKERR_TIMEOUT     -4
#define SOCKERR_OK          0

/**
 * \fn int sock_write_t (int fd, const void* buff, int count, unsigned int timeout) 
 * \brief Writes data to socket.
 *
 * This function writes the data block pointed to by \a buff 
 * which is \a count bytes long to the socket \a fd.
 *
 * \param fd The file descriptor of the socket.
 * \param buff The buffer contains the data.
 * \param count The length in bytes of the buffer.
 * \param timeout An upper bound on the amount of time elapsed before 
 *        \a sock_write_t returns. When it is zero, \a sock_write_t can 
 *        block indefinitely. The timeout value is in tick count, and 
 *        tick count of MiniGUI is in unit of 10 milliseconds.
 * \return SOCKERR_OK if all OK, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 * \retval SOCKERR_TIMEOUT  Timeout.
 *
 * \note The \a timeout only goes into effect when this function called 
 *       by the server of MiniGUI-Lite, i.e. \a mginit. 
 *
 * \sa sock_read_t
 */
int sock_write_t (int fd, const void* buff, int count, unsigned int timeout);

/**
 * \fn int sock_read_t (int fd, void* buff, int count, unsigned int timeout)
 * \brief Reads data from socket.
 *
 * This function reads data which is \a count bytes long to the buffer \a buff
 * from the socket \a fd.
 *
 * \param fd The file descriptor of the socket.
 * \param buff The buffer used to save the data.
 * \param count The length in bytes of the buffer.
 * \param timeout An upper bound on the amount of time elapsed before 
 *        \a sock_read_t returns. When it is zero, \a sock_read_t can 
 *        block indefinitely. The timeout value is in the tick count of MiniGUI,
 *        and tick count of MiniGUI is in unit of 10 milliseconds.
 * \return SOCKERR_OK if all OK, < 0 on error.
 *
 * \retval SOCKERR_OK       Read data successfully.
 * \retval SOCKERR_IO       There are some I/O errors occurred.
 * \retval SOCKERR_CLOSED   The socket has been closed by the peer.
 * \retval SOCKERR_INVARG   You passed invalid arguments.
 * \retval SOCKERR_TIMEOUT  Timeout.
 *
 * \note The \a timeout only goes into effect when this function called 
 *       by the server of MiniGUI-Lite, i.e. \a mginit. 
 *
 * \sa sock_write_t
 */
int sock_read_t (int fd, void* buff, int count, unsigned int timeout);

/**
 * \def sock_write(fd, buff, count)
 * \brief The blocking version of \a sock_write_t function.
 *
 * \sa sock_write_t
 */
#define sock_write(fd, buff, count) sock_write_t(fd, buff, count, 0)

/**
 * \def sock_read(fd, buff, count)
 * \brief The blocking version of \a sock_read_t function.
 *
 * \sa sock_read_t
 */
#define sock_read(fd, buff, count) sock_read_t(fd, buff, count, 0)

    /** @} end of lite_socket_fns */

    /** @} end of lite_fns */

#endif /* !_STAND_ALONE */
#endif /* LITE_VERSION */

    /**
     * \defgroup init_fns Initialization and termination functions
     *
     * Normally, the only entry of any MiniGUI application is \a MiniGUIMain.
     * The application will terminate when you call \a exit(3) or just return from
     * \a MiniGUIMain.
     *
     * Example 1:
     *
     * \include miniguimain.c
     *
     * Example 2:
     *
     * \include hello_world.c
     *
     * @{
     */

/**
 * \fn BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text)
 * \brief Re-initializes the desktop.
 *
 * When you changed the charset or the background picture of the desktop,
 * you should call this function to re-initialize the local system text
 * (when \a init_sys_text is TRUE), the background picture, and the desktop
 * menu.
 *
 * \param init_sys_text Indicates whether to initialize the local system text.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \sa ReinitDesktop
 */
MG_EXPORT BOOL GUIAPI ReinitDesktopEx (BOOL init_sys_text);

/**
 * \def ReinitDesktop()
 * \brief Re-initializes the desktop including the local system text.
 *
 * \return TRUE on success, otherwise FALSE.
 *
 * \note This function defined as a macro calling \a ReinitDesktopEx with
 * \a init_sys_text set to TRUE.
 *
 * \sa ReinitDesktopEx
 */
#define ReinitDesktop()    ReinitDesktopEx (TRUE)

/*
 * We remove the SuspendGUI and ResumeGUI functions. 
 * Don't use these two functios any more.
 * void GUIAPI SuspendGUI (void);
 * BOOL GUIAPI ResumeGUI (void); 
 */

/**
 * \fn void GUIAPI ExitGUISafely (int exitcode)
 * \brief Exits your MiniGUI application safely.
 *
 * Calling this function will terminate your MiniGUI application. This
 * function will restore console attributes and call \a exit() function and 
 * pass \a exitcode to it.
 *
 * \param exitcode The exit status will be passed to exit(3) function.
 * \return This function does not return.
 *
 * \sa exit(3)
 */
MG_EXPORT void GUIAPI ExitGUISafely (int exitcode);

/**
 * \def int MiniGUIMain (int args, const char* arg[])
 * \brief The main entry of a MiniGUI application.
 *
 * This function should be defined by your application. Before Version 1.6.1, 
 * MiniGUI defines \a main() function in libminigui library for your application, 
 * and call \a MiniGUIMain() in this \a main() function. The \a main() defined 
 * by MiniGUI is responsible of initializing and terminating MiniGUI.
 *
 * After version 1.6.1, MiniGUI defines MiniGUIMain as a macro.
 *
 * \param args The number of arguments passed to \a main() by operating system.
 * \param arg The arguments passed to \a main() by operating system.
 * \return The exit status will be retured to the parent process.
 *
 */

#ifdef _USE_MINIGUIENTRY
  #define main_entry minigui_entry
  int minigui_entry (int args, const char* arg[]);
#else
  #define main_entry main
#endif

#define MiniGUIMain \
MiniGUIAppMain (int args, const char* argv[]); \
int main_entry (int args, const char* argv[]) \
{ \
    int iRet = 0; \
    if (InitGUI (args, argv) != 0) { \
        return 1; \
    } \
    iRet = MiniGUIAppMain (args, argv); \
    TerminateGUI (iRet); \
    return iRet; \
} \
int MiniGUIAppMain

#define IDM_DTI_FIRST   (300)

#ifndef _LITE_VERSION
/*
 * NOTE: The following two functions is only valid for MiniGUI-Threads
 * since version 1.0.01.
 */
MG_EXPORT typedef void GUIAPI (*CustomizeDesktopMenuFunc) (HMENU hDesktopMenu, int iPos);
MG_EXPORT typedef int GUIAPI (*CustomDesktopCommandFunc) (int id);

extern CustomizeDesktopMenuFunc CustomizeDesktopMenu;
extern CustomDesktopCommandFunc CustomDesktopCommand;
#endif

    /** @} end of init_fns */

    /**
     * \defgroup mouse_calibrate Mouse calibration.
     * @{
     */

#ifdef _MISC_MOUSECALIBRATE

/**
 * \fn BOOL SetMouseCalibrationParameters (const POINT* src_pts, const POINT* dst_pts);
 * \brief Sets the parameters for doing mouse calibration.
 *
 * This function set the parameters for doing mouse calibration.
 * You should pass five source points and five destination points.
 *
 * Normally, the points should be the upper-left, upper-right, lower-right, lower-left,
 * and center points on the touch panel. The source point is the coordinates before
 * calibrating, and the destination point is the disired coordinates after calibrating.
 *
 * This function will try to evaluate a matrix to calibrate. If the points are okay,
 * MiniGUI will do the calibration after getting a point from the underlay IAL engine.
 *
 * \param src_pts The pointer to an array of five source points.
 * \param dst_pts The pointer to an array of five destination points.
 * \return TRUE for success, FALSE for bad arguments.
 *
 * \note This function is available when _MISC_MOUSECALIBRATE (option: --enable-mousecalibrate) defined.
 */
MG_EXPORT BOOL GUIAPI SetMouseCalibrationParameters (const POINT* src_pts, const POINT* dst_pts);

#endif /* _MISC_MOUSECALIBRATE */

    /** @} end of mouse_calibrate */

    /**
     * \defgroup about_dlg About MiniGUI dialog
     * @{
     */
#ifdef _MISC_ABOUTDLG
#ifndef _LITE_VERSION
  void GUIAPI OpenAboutDialog (void);
#else

/**
 * \fn HWND GUIAPI OpenAboutDialog (HWND hHosting)
 * \brief Opens or actives the 'About MiniGUI' dialog.
 *
 * Calling this function will create a main window displaying 
 * copyright and license information of MiniGUI. When the about dialog 
 * is displaying, calling this function again will bring the dialog to be 
 * the topmost main window, not create a new one.
 *
 * \param hHosting The hosting main window of the about dialog.
 * \return The handle to the about dialog box.
 *
 * \note This function is available for MiniGUI-Lite and when _MISC_ABOUTDLG defined.
 * For MiniGUI-Threads, you should call 'void GUIAPI OpenAboutDialog (void)' function
 * instead.
 */
  HWND GUIAPI OpenAboutDialog (HWND hHosting);
#endif /* _LITE_VERSION */
#endif /* _MISC_ABOUTDLG */

    /** @} end of about_dlg */

    /**
     * \defgroup etc_fns Configuration file operations
     *
     * The configuration file used by MiniGUI have a similiar format as M$ Windows INI file,
     * i.e. the file consists of sections, and the section consists of key-value pairs, like this:
     *
     * \code
     * [system]
     * # GAL engine
     * gal_engine=fbcon
     *
     * # IAL engine
     * ial_engine=console
     *
     * mdev=/dev/mouse
     * mtype=PS2
     * 
     * [fbcon]
     * defaultmode=1024x768-16bpp
     * 
     * [qvfb]
     * defaultmode=640x480-16bpp
     * display=0
     * \endcode
     *
     * Assume that the configuration file named \a my.cfg, if you want get the value of \a mdev
     * in \a system section, you can call \a GetValueFromEtcFile in the following way:
     *
     * \code
     * char buffer [51];
     *
     * GetValueFromEtcFile ("my.cfg", "system", "mdev", buffer, 51);
     * \endcode
     *
     * Example:
     *
     * \include cfgfile.c
     *
     * @{
     */

#define ETC_MAXLINE             1024
#define ETC_FILENOTFOUND        -1
#define ETC_SECTIONNOTFOUND     -2
#define ETC_KEYNOTFOUND         -3
#define ETC_TMPFILEFAILED       -4
#define ETC_FILEIOFAILED        -5
#define ETC_INTCONV             -6
#define ETC_OK                  0

#ifndef _INCORE_RES

/**
 * \var char* ETCFILEPATH
 * \brief The path name of MiniGUI configuration file.
 *
 * By default, the configuration file of MiniGUI must be installed in /etc,
 * /usr/local/etc or your home directory. When you install it in your
 * home directory, the name should be ".MiniGUI.cfg".
 *
 * MiniGUI will try to use \a ~/.MiniGUI.cfg, then \a /usr/local/etc/MiniGUI.cfg, 
 * and \a /etc/MiniGUI.cfg last.
 *
 * If MiniGUI can not find any \a MiniGUI.cfg file, or find a bad formated configure
 * file, the initialzation of MiniGUI will be canceled.
 */
extern char ETCFILEPATH [];

#endif /* _INCORE_RES */

/**
 * \fn int GUIAPI GetValueFromEtcFile (const char* pEtcFile, const char* pSection, const char* pKey, char* pValue, int iLen)
 * \brief Gets value from a configuration file.
 *
 * This function gets the value of the key \a pKey in the section \a pSection 
 * of the configuration file \a pEtcFile, and saves the value to the buffer
 * pointed to by \a pValue. 
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 * \param iLen The length in bytes of the buffer.
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILENOTFOUND     Can not find the specified configuration file.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 *
 * \note MiniGUI use \a strncpy to copy actual value to \a pValue. Thus, if the length of 
 * the actual value is larger than \a iLen, the result copied to \a pValue 
 * will \em NOT be null-terminated.
 *
 * \sa GetIntValueFromEtcFile, SetValueToEtcFile, strncpy(3)
 */
MG_EXPORT int GUIAPI GetValueFromEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue, int iLen);

/**
 * \fn int GUIAPI GetIntValueFromEtcFile (const char* pEtcFile, const char* pSection, const char* pKey, int* value)
 * \brief Gets integer value from a configuration file.
 *
 * This function gets the integer value of the key \a pKey in the section \a pSection 
 * of the configuration file \a pEtcFile, and returns the integer value through the buffer
 * pointed to by \a value. 
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param value The integer value will be saved in this buffer.
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILENOTFOUND     Can not find the specified configuration file.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_INTCONV          Can not convert the value string to an integer.
 *
 * \note MiniGUI uses \a strtol to convert the string value to an integer, and pass the base as 0.
 * Thus, the valid string value can be converted to integer should be in the following forms:
 *
 *  - [+|-]0x[0-9|A-F]*\n
 *    Will be read in base 16.
 *  - [+|-]0[0-7]*\n
 *    Will be read in base 8.
 *  - [+|-][1-9][0-9]*\n
 *    Will be read in base 10.
 *
 * \sa GetValueFromEtcFile, SetValueToEtcFile, strtol(3)
 */
MG_EXPORT int GUIAPI GetIntValueFromEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, int* value);

/**
 * \fn int GUIAPI SetValueToEtcFile (const char* pEtcFile, const char* pSection, const char* pKey, char* pValue)
 * \brief Sets a value in a configuration file.
 *
 * This function sets the value of the key \a pKey in the section \a pSection
 * of the configuration file \a pEtcFile to be the string pointed to by \a pValue.
 *
 * \param pEtcFile The path name of the configuration file.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The null-terminated value string.
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Sets value successfullly.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 * \retval ETC_TMPFILEFAILED    Can not create temporary file.
 *
 * \note If the specified configuration file does not exist, MiniGUI will try to
 * create this file.
 *
 * \sa GetValueFromEtcFile, GetIntValueFromEtcFile
 */
MG_EXPORT int GUIAPI SetValueToEtcFile (const char* pEtcFile, const char* pSection,
                               const char* pKey, char* pValue);

/**
 * \fn GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile)
 * \brief Loads an etc file into memory.
 *
 * This function loads the content of an etc file into the memory, later, you
 * can visit the content using \a GetValueFromEtc function.
 *
 * \param pEtcFile The path name of the configuration file.
 * \return Handle of the etc object on success, NULL on error.
 *
 * \sa UnloadEtcFile, GetValueFromEtc
 */
MG_EXPORT GHANDLE GUIAPI LoadEtcFile (const char * pEtcFile);

/**
 * \fn GUIAPI UnloadEtcFile (GHANDLE hEtc)
 * \brief Unloads an etc file.
 *
 * This function unloads the etc object generated by using \sa LoadEtcFile function.
 *
 * \param hEtc Handle of the etc object.
 * \return 0 on success, -1 on error.
 *
 * \sa LoadEtcFile, GetValueFromEtc
 */
MG_EXPORT int GUIAPI UnloadEtcFile (GHANDLE hEtc);

/**
 * \fn GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection, const char* pKey, char* pValue, int iLen)
 * \brief Gets value from a configuration etc object.
 *
 * This function gets value from an etc object, similar to GetValueFromEtcFile.
 * This function gets the value of the key \a pKey in the section \a pSection 
 * of the etc object \a hEtc, and saves the value to the buffer
 * pointed to by \a pValue. 
 *
 * \param hEtc Handle of the etc object.
 * \param pSection The section name in which the value located.
 * \param pKey The key name of the value.
 * \param pValue The value will be saved in this buffer.
 * \param iLen The length in bytes of the buffer.
 * \return ETC_OK on success, < 0 on error.
 *
 * \retval ETC_OK               Gets value successfullly.
 * \retval ETC_FILENOTFOUND     Can not find the specified configuration file.
 * \retval ETC_SECTIONNOTFOUND  Can not find the specified section in the configuration file.
 * \retval ETC_KEYNOTFOUND      Can not find the specified key in the section.
 * \retval ETC_FILEIOFAILED     File I/O operation error occurred.
 *
 * \sa LoadEtcFile, UnloadEtcFile
 */
MG_EXPORT int GUIAPI GetValueFromEtc (GHANDLE hEtc, const char* pSection,
                                            const char* pKey, char* pValue, int iLen);

/**
 * \fn int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection, const char* pKey, int* pValue)
 * \brief Gets the integer value from a configuration etc object.
 *
 * \sa GetValueFromEtc, GetIntValueFromEtcFile
 */
MG_EXPORT int GUIAPI GetIntValueFromEtc (GHANDLE hEtc, const char* pSection,
                                            const char* pKey, int* pValue);
/**
 * \def SetValueToEtc (GHANDLE hEtc, const char* pSection, const char* pKey, char* pValue)
 * \brief Sets the value in the etc object.
 *
 * This fuctions sets the value in the etc object, somewhat similiar to \sa SetValueToEtcFile.
 */
#define SetValueToEtc(hEtc, pSection, pKey, pValue) \
        GetValueFromEtc(hEtc, pSection, pKey, pValue, -1)


/* global MiniGUI etc file object */
extern GHANDLE hMgEtc;

/* Gets value from MiniGUI configuration etc object */
static inline int GetMgEtcValue(const char* pSection, const char *pKey, char *pValue, int iLen) 
{
#ifndef _INCORE_RES
    if (!hMgEtc)
        return GetValueFromEtcFile (ETCFILEPATH, pSection, pKey, pValue, iLen);
#endif

    return GetValueFromEtc (hMgEtc, pSection, pKey, pValue, iLen);
}

/* Gets integer value from MiniGUI configuration etc object */
static inline int GetMgEtcIntValue (const char *pSection, const char* pKey, int *value)
{
#ifndef _INCORE_RES
    if (!hMgEtc)
        return GetIntValueFromEtcFile (ETCFILEPATH, pSection, pKey, value);
#endif

    return GetIntValueFromEtc (hMgEtc, pSection, pKey, value);
}

    /** @} end of etc_fns */

#ifdef _CLIPBOARD_SUPPORT

    /**
     * \addtogroup clipboard_fns ClipBoard Operations
     * @{
     */

#define LEN_CLIPBOARD_NAME      15
#define NR_CLIPBOARDS           4

#define CBNAME_TEXT             ("text")

#define CBERR_OK        0
#define CBERR_BADNAME   1
#define CBERR_NOMEM     2

#define CBOP_NORMAL     0
#define CBOP_APPEND     1

/**
 * \fn int GUIAPI CreateClipBoard (const char* cb_name, size_t size)
 * \brief Create a new clip board.
 *
 * This function creates a new clip board with the name \a cb_name.
 * MiniGUI itself creates a clip board for text copying/pasting
 * called CBNAME_TEXT.
 *
 * \param cb_name The name of the new clip board.
 * \param size The size of the clip board.
 *
 * \retval CBERR_OK         The clip board created.
 * \retval CBERR_BADNAME    Duplicated clip board name.
 * \retval CBERR_NOMEM      No enogh memory.
 *
 */
MG_EXPORT int GUIAPI CreateClipBoard (const char* cb_name, size_t size);

/**
 * \fn int GUIAPI DestroyClipBoard (const char* cb_name)
 * \brief Destroy a new clip board.
 *
 * This function destroies a clip board with the name \a cb_name.
 *
 * \param cb_name The name of the clip board.
 *
 * \retval CBERR_OK         The clip board created.
 * \retval CBERR_BADNAME    Can not find the clip board with the name.
 */
MG_EXPORT int GUIAPI DestroyClipBoard (const char* cb_name);

/**
 * \fn int GUIAPI SetClipBoardData (const char* cb_name, void* data, size_t n, int cbop)
 * \brief Set the data of a clip board.
 *
 * This function set the data into the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clip board.
 * \param data The pointer to the data.
 * \param n The length of the data.
 * \param cbop Type of clipboard operations, default is CBOP_NORMAL
 *
 * \retval CBERR_OK         Success.
 * \retval CBERR_BADNAME    Bad clip board name.
 * \retval CBERR_NOMEM      No enogh memory.
 */
MG_EXPORT int GUIAPI SetClipBoardData (const char* cb_name, void* data, size_t n, int cbop);

/**
 * \fn size_t GUIAPI GetClipBoardDataLen (const char* cb_name);
 * \brief Get the length of the data of a clip board.
 *
 * This function the data length of the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clip board.
 * \return The size of the data if success, otherwise zero.
 */
MG_EXPORT size_t GUIAPI GetClipBoardDataLen (const char* cb_name);

/**
 * \fn size_t GUIAPI GetClipBoardData (const char* cb_name, void* data, size_t n);
 * \brief Get the data of a clip board.
 *
 * This function get the all data from the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clip board.
 * \param data The pointer to a buffer will save the data.
 * \param n The length of the buffer.
 *
 * \return The size of the data got if success, otherwise zero.
 */
MG_EXPORT size_t GUIAPI GetClipBoardData (const char* cb_name, void* data, size_t n);

/**
 * \fn int GUIAPI GetClipBoardByte (const char* cb_name, int index, unsigned char* byte);
 * \brief Get a byte of from a clip board.
 *
 * This function gets a byte from the clipboard named \a cb_name.
 *
 * \param cb_name The name of the clip board.
 * \param index The index of the byte.
 * \param byte The buffer saving the returned byte.
 *
 * \retval CBERR_OK         The clip board created.
 * \retval CBERR_BADNAME    Duplicated clip board name.
 * \retval CBERR_NOMEM      The index is beyond the data in the clipboard.
 */
MG_EXPORT int GUIAPI GetClipBoardByte (const char* cb_name, int index, unsigned char* byte);

    /** @} end of clipboard_fns */

#endif /* _CLIPBOARD_SUPPORT */

    /**
     * \addtogroup misc_fns Miscellaneous functions
     * @{
     */

/**
 * \fn void GUIAPI Ping (void)
 * \brief Makes a beep sound.
 * \sa Beep
 */
MG_EXPORT void GUIAPI Ping (void);

/**
 * \def Beep
 * \brief Alias of Ping.
 * \sa Ping
 */
#define Beep Ping

/**
 * \fn void GUIAPI Tone (int frequency_hz, int duration_ms)
 * \brief Makes a tone.
 *
 * This function will return after the tone. Thus, your program
 * will be blocked when the tone is being played.
 *
 * \param frequency_hz The frequency of the tone in hertz.
 * \param duration_ms The duration of the tone in millisecond.
 *
 * \bug When MiniGUI runs on X Window, the tone can not be played correctly.
 *
 * \sa Ping
 */
MG_EXPORT void GUIAPI Tone (int frequency_hz, int duration_ms);

/**
 * \fn void* GUIAPI GetOriginalTermIO (void)
 * \brief Gets \a termios structure of the original terminal before initializing MiniGUI.
 *
 * \return The pointer to the original \a termios structure.
 */
MG_EXPORT void* GUIAPI GetOriginalTermIO (void);

    /** @} end of misc_fns */

    /**
     * \defgroup fixed_str Length-Fixed string operations
     *
     * MiniGUI maintains a private heap for length-fixed strings, and allocates
     * length-fixed strings from this heap for window caption, menu item text, 
     * and so on. You can also use this private heap to allocate length-fixed strings.
     *
     * \include fixstr.c
     *
     * @{
     */

/**
 * \fn char* GUIAPI FixStrAlloc (int len)
 * \brief Allocates a buffer for a length-fixed string.
 *
 * This function allocates a buffer from the length-fixed string heap
 * for a string which is \a len bytes long (does not include 
 * the null character of the string). 
 *
 * \note You can change the content of the string, but do not change the
 * length of this string (shorter is valid) via \a strcat function or 
 * other equivalent functions or operations.
 *
 * \param len The length of the string.
 * \return The pointer to the buffer on success, otherwise NULL.
 *
 * \sa FreeFixStr
 */
MG_EXPORT char* GUIAPI FixStrAlloc (int len);

/**
 * \fn void GUIAPI FreeFixStr (char* str)
 * \brief Frees a length-fixed string.
 *
 * This function frees the buffer used by the length-fixed string \a str.
 *
 * \param str The length-fixed string.
 *
 * \note Do not use \a free to free the length-fixed string.
 *
 * \sa FixStrAlloc
 */
MG_EXPORT void GUIAPI FreeFixStr (char* str);

    /** @} end of fixed_str */

    /**
     * \defgroup cursor_fns Cursor operations
     * @{
     */

#ifndef _CURSOR_SUPPORT
static inline void do_nothing (void) { return; }
#endif

#ifdef _CURSOR_SUPPORT

/**
 * \fn HCURSOR GUIAPI LoadCursorFromFile (const char* filename)
 * \brief Loads a cursor from a M$ Windows cursor file.
 *
 * This function loads a cursor from M$ Windows *.cur file 
 * named \a filename and returns the handle to loaded cursor. 
 * The returned handle can be used by \a SetCursor to set new mouse cursor.
 *
 * \param filename The path name of the cursor file.
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI does not support 256-color or animation cursor.
 *
 * \sa SetCursor
 */
  HCURSOR GUIAPI LoadCursorFromFile (const char* filename);

/**
 * \fn HCURSOR GUIAPI LoadCursorFromMem (const void* area)
 * \brief Loads a cursor from a memory area.
 *
 * This function loads a cursor from a memory area pointed to by \a area. 
 * The memory has the same layout as a M$ Windows CURSOR file.
 * The returned handle can be used by \a SetCursor to set new mouse cursor.
 *
 * \param area The pointer to the cursor data.
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI does not support 256-color or animation cursor.
 *
 * \sa SetCursor
 */
  HCURSOR GUIAPI LoadCursorFromMem (const void* area);

/**
 * \fn HCURSOR GUIAPI CreateCursor (int xhotspot, int yhotspot, int w, int h, const BYTE* pANDBits, const BYTE* pXORBits, int colornum)
 * \brief Creates a cursor from memory data.
 *
 * This function creates a cursor from memory data rather than cursor file. 
 * \a xhotspot and \a yhotspot specify the hotpot of the cursor, \a w and \a h are 
 * the width and the height of the cursor respectively. \a pANDBits and \a pXORBits
 * are AND bitmask and XOR bitmask of the cursor. 
 * MiniGUI currently support mono-color cursor and 16-color cursor, \a colornum 
 * specifies the cursor's color depth. For mono-color, it should be 1, and for
 * 16-color cursor, it should be 4.
 *
 * \param xhotspot The x-coordinate of the hotspot.
 * \param yhotspot The y-coordinate of the hotspot.
 * \param w The width of the cursor.
 * \param h The height of the cursor.
 * \param pANDBits The pointer to AND bits of the cursor.
 * \param pXORBits The pointer to XOR bits of the cursor.
 * \param colornum The bit-per-pixel of XOR bits.
 * \return Handle to the cursor, zero on error.
 *
 * \note MiniGUI only support 2-color or 16-color cursor.
 */
  HCURSOR GUIAPI CreateCursor (int xhotspot, int yhotspot, int w, int h, 
               const BYTE* pANDBits, const BYTE* pXORBits, int colornum);

/**
 * \fn BOOL GUIAPI DestroyCursor (HCURSOR hcsr)
 * \brief Destroies a cursor object.
 *
 * This function destroys a cursor object specified by \a hcsr.
 *
 * \param hcsr Handle to the cursor.
 * \return TRUE on success, otherwise FALSE.
 */
  BOOL GUIAPI DestroyCursor (HCURSOR hcsr);

/**
 * \fn HCURSOR GUIAPI GetSystemCursor (int csrid)
 * \brief Gets the handle to a system cursor by its identifier.
 *
 * MiniGUI creates (MAX_SYSCURSORINDEX + 1) system cursors for application.
 * You can use \a GetSystemCursor to get the handle to these system cursors.
 * The identifier can be one of the following:
 * 
 *  - IDC_ARROW\n
 *    Normal arrow cursor.
 *  - IDC_IBEAM\n
 *    'I' shaped beam cursor, indicate an input field.
 *  - IDC_PENCIL\n
 *    Pencil-shape cursor.
 *  - IDC_CROSS\n
 *    Cross cursor.
 *  - IDC_MOVE\n
 *    Moving cursor.
 *  - IDC_SIZENWSE\n
 *    Sizing cursor, along north-west and south-east.
 *  - IDC_SIZENESW\n
 *    Sizing cursor, along north-east and south-west.
 *  - IDC_SIZEWE\n
 *    Sizing cursor, along west and east.
 *  - IDC_SIZENS\n
 *    Sizing cursor, along north and south.
 *  - IDC_UPARROW\n
 *    Up arrow cursor.
 *  - IDC_NONE\n
 *    None cursor.
 *  - IDC_HELP\n
 *    Arrow with question.
 *  - IDC_BUSY\n
 *    Busy cursor.
 *  - IDC_WAIT\n
 *    Wait cursor.
 *  - IDC_RARROW\n
 *    Right arrow cursor.
 *  - IDC_COLOMN\n
 *    Cursor indicates column.
 *  - IDC_ROW\n
 *    Cursor indicates row.
 *  - IDC_DRAG\n
 *    Draging cursor.
 *  - IDC_NODROP\n
 *    No droping cursor.
 *  - IDC_HAND_POINT\n
 *    Hand point cursor.
 *  - IDC_HAND_SELECT\n
 *    Hand selection cursor.
 *  - IDC_SPLIT_HORZ\n
 *    Horizontal splitting cursor.
 *  - IDC_SPLIT_VERT\n
 *    Vertical splitting cursor.
 *
 * \param csrid The identifier of the system cursor.
 * \return Handle to the system cursor, otherwise zero.
 */
MG_EXPORT HCURSOR GUIAPI GetSystemCursor (int csrid);

/**
 * \fn HCURSOR GUIAPI GetCurrentCursor (void)
 * \brief Gets the handle to the current cursor.
 *
 * This function retrives the current cursor and returns its handle.
 *
 * \return Handle to the current system cursor, zero means no current cursor.
 */
MG_EXPORT HCURSOR GUIAPI GetCurrentCursor (void);
#else
  #define LoadCursorFromFile(filename)    (do_nothing(), 0)
  #define CreateCursor(x, y, w, h, ANDbs, XORbs, cr) (do_nothing(), 0)
  #define DestroyCursor(hcsr)             (do_nothing(), 0)
  #define GetSystemCursor(csrid)          (do_nothing(), 0)
  #define GetCurrentCursor()              (do_nothing(), 0)
#endif /* _CURSOR_SUPPORT */

#define MAX_SYSCURSORINDEX    22

/* System cursor index. */
#define IDC_ARROW       0
#define IDC_IBEAM       1
#define IDC_PENCIL      2
#define IDC_CROSS       3
#define IDC_MOVE        4
#define IDC_SIZENWSE    5
#define IDC_SIZENESW    6
#define IDC_SIZEWE      7
#define IDC_SIZENS      8
#define IDC_UPARROW     9
#define IDC_NONE        10
#define IDC_HELP        11
#define IDC_BUSY        12
#define IDC_WAIT        13
#define IDC_RARROW      14
#define IDC_COLOMN      15
#define IDC_ROW         16
#define IDC_DRAG        17
#define IDC_NODROP      18
#define IDC_HAND_POINT  19
#define IDC_HAND_SELECT 20
#define IDC_SPLIT_HORZ  21
#define IDC_SPLIT_VERT  22

/**
 * \fn void GUIAPI ClipCursor (const RECT* prc)
 * \brief Clips the cursor range.
 *
 * This function sets cursor's clipping rectangle. \a prc 
 * is the new clipping rectangle in screen coordinates. If \a prc is NULL, 
 * \a ClipCursor will disable cursor clipping.
 *
 * \param prc The clipping rectangle.
 * \return None.
 */
MG_EXPORT void GUIAPI ClipCursor (const RECT* prc);

/**
 * \fn void GUIAPI GetClipCursor (RECT* prc)
 * \brief Gets the current cursor clipping rectangle.
 *
 * This function copies the current clipping rectangle to 
 * a RECT pointed to by \a prc.
 *
 * \param prc The clipping rectangle will be saved to this rectangle.
 * \return None.
 */
MG_EXPORT void GUIAPI GetClipCursor (RECT* prc);

/**
 * \fn void GUIAPI GetCursorPos (POINT* ppt)
 * \brief Gets position of the current cursor.
 *
 * This function copies the current mouse cursor position to 
 * a POINT structure pointed to by \a ppt.
 *
 * \param ppt The position will be saved in this buffer.
 * \return None.
 *
 * \sa SetCursorPos, POINT
 */
MG_EXPORT void GUIAPI GetCursorPos (POINT* ppt);

/**
 * \fn void GUIAPI SetCursorPos (int x, int y)
 * \brief Sets position of the current cursor.
 *
 * This function sets mouse cursor position with the given 
 * arguments: \a (x,y).
 *
 * \param x The x-corrdinate of the expected poistion.
 * \param y The y-corrdinate of the expected poistion.
 * \return None.
 *
 * \sa GetCursorPos
 */
MG_EXPORT void GUIAPI SetCursorPos (int x, int y);

#ifdef _CURSOR_SUPPORT
/**
 * \fn HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL set_def)
 * \brief Changes the current cursor.
 *
 * This function changes the current cursor to be \a hcsr,
 * and/or sets it to be the default cursor.
 *
 * If you pass \a set_def as TRUE, the expected cursor will be the default cursor. 
 * The default cursor will be used when you move cursor to the desktop.
 *
 * \param hcsr The expected cursor handle.
 * \param set_def Indicates whether setting the cursor as the default cursor.
 * \return The old cursor handle.
 *
 * \sa SetCursor, SetDefaultCursor, GetDefaultCursor
 */
 MG_EXPORT HCURSOR GUIAPI SetCursorEx (HCURSOR hcsr, BOOL set_def);

/**
 * \def SetCursor(hcsr)
 * \brief Changes the current cursor.
 *
 * This function changes the current cursor to be \a hcsr.
 *
 * \param hcsr The expected cursor handle.
 * \return The old cursor handle.
 *
 * \note This function defined as a macro calling \a SetCursorEx with
 * passing \a set_def as FALSE.
 *
 * \sa SetCursorEx, SetDefaultCursor
 */
  #define SetCursor(hcsr) SetCursorEx (hcsr, FALSE)

/**
 * \def SetDefaultCursor(hcsr)
 * \brief Changes the current cursor, and set it as the default cursor.
 *
 * This function changes the current cursor to be \a hcsr, and set it as the default cursor.
 *
 * \param hcsr The expected cursor handle.
 * \return The old cursor handle.
 *
 * \note This function defined as a macro calling \a SetCursorEx with
 * passing \a set_def as TRUE.
 *
 * \sa SetCursorEx, SetCursor
 */
  #define SetDefaultCursor(hcsr) SetCursorEx (hcsr, TRUE)

/**
 * \fn HCURSOR GUIAPI GetDefaultCursor (void)
 * \brief Gets the default cursor.
 *
 * This function gets the current default cursor.
 *
 * \return The current default cursor handle.
 *
 * \sa SetCursorEx, SetDefaultCursor
 */
MG_EXPORT  HCURSOR GUIAPI GetDefaultCursor (void);

#else
  #define SetCursorEx(hcsr, set_def)    (do_nothing(), 0)
  #define SetCursor(hcsr)               (do_nothing(), 0)
  #define SetDefaultCursor(hcsr)        (do_nothing(), 0)
  #define GetDefaultCursor()            (do_nothing(), 0)
#endif /* _CURSOR_SUPPORT */

#ifdef _CURSOR_SUPPORT

/**
 * \fn int GUIAPI ShowCursor (BOOL fShow)
 * \brief Shows or hides cursor.
 *
 * This function shows or hides cursor according to the argument \a fShow. 
 * Show cursor when \a fShow is TRUE, and hide cursor when \a fShow is FALSE.
 * MiniGUI maintains a showing count value. Calling \a ShowCursor once, the count 
 * will increase when \a fShow is TRUE, or decrease one when FALSE.
 * When the count is less than 0, the cursor will disapear actually.
 *
 * \param fShow Indicates show or hide the cursor.
 * \return Cursor showing count value. 
 */
MG_EXPORT int GUIAPI ShowCursor (BOOL fShow);
#else
  #define ShowCursor(fShow)             (do_nothing(), 0)
#endif /* _CURSOR_SUPPORT */

    /** @} end of cursor_fns */

    /**
     * \defgroup key_status Asynchronous key status functions
     * @{
     */

/**
 * \fn BOOL GUIAPI GetKeyStatus (UINT uKey)
 * \brief Gets a key or a mouse button status.
 *
 * This function gets a key or a mouse button status, returns TRUE 
 * when pressed, or FALSE when released. \a uKey indicates 
 * the key or mouse button. For keys on keyboard, \a uKey should be 
 * the scancode of the key, for mouse button, \a uKey should be one 
 * value of the following:
 *
 *  - SCANCODE_LEFTBUTTON\n
 *    Left mouse button.
 *  - SCANCODE_MIDDLBUTTON\n
 *    Middle mouse button.
 *  - SCANCODE_RIGHTBUTTON\n
 *    Right mouse button.
 *
 * These constants and the scancodes of keys are defined in <minigui/common.h>.
 *
 * \param uKey Indicates the key or mouse button. 
 * \return Returns TRUE when pressed, or FALSE when released.
 *
 * \sa GetShiftKeyStatus
 */
MG_EXPORT BOOL GUIAPI GetKeyStatus (UINT uKey);

/**
 * \fn DWORD GUIAPI GetShiftKeyStatus (void)
 * \brief Gets status of the shift keys.
 *
 * This function gets ths status of the shift keys, the returned value 
 * indicates the status of shift keys -- CapsLock, ScrollLock, NumLock, Left Shift, 
 * Right Shift, Left Ctrl, Right Ctrl, Left Alt, and Right Alt. 
 * You can use KS_* ORed with the status value to determine one shift key's status:
 *
 *  - KS_CAPSLOCK\n
 *    Indicates that CapsLock is locked.
 *  - KS_NUMLOCK\n
 *    Indicates that NumLock is locked.
 *  - KS_SCROLLLOCK\n
 *    Indicates that ScrollLock is locked.
 *  - KS_LEFTCTRL\n
 *    Indicates that left Ctrl key is pressed.
 *  - KS_RIGHTCTRL\n
 *    Indicates that right Ctrl key is pressed.
 *  - KS_CTRL\n
 *    Indicates that either left or right Ctrl key is pressed.
 *  - KS_LEFTALT\n
 *    Indicates that left Alt key is pressed.
 *  - KS_RIGHTALT\n
 *    Indicates that right Alt key is pressed.
 *  - KS_ALT\n
 *    Indicates that either left or right Alt key is pressed.
 *  - KS_LEFTSHIFT\n
 *    Indicates that left Shift key is pressed.
 *  - KS_RIGHTSHIFT\n
 *    Indicates that right Shift key is pressed.
 *  - KS_SHIFT\n
 *    Indicates that either left or right Shift key is pressed.
 *
 * These constants are defined in <minigui/common.h>.
 *
 * \return The status of the shift keys.
 * \sa key_defs
 */
MG_EXPORT DWORD GUIAPI GetShiftKeyStatus (void);

    /** @} end of key_status */

    /**
     * \defgroup sys_text Internationlization of system text
     * @{
     */

/**
 * \fn const char* GetSysText (const char* text);
 * \brief Translates system text to localized text.
 *
 * When MiniGUI display some system messages, it will call \a GetSysText function 
 * to translate system text from English to other language. 
 * Global variable \a SysText contains all text used by MiniGUI in English.
 *
 * \a GetSysText function returns localized text from \a local_SysText. 
 * MiniGUI have already defined localized sytem text for en_US, zh_CN.GB2312 
 * and zh_TW.Big5 locales. MiniGUI initializes \a local_SysText to 
 * point one of above localized system text when startup.  You can also 
 * let \a local_SysText point to your customized string array.
 * 
 * \param text The system text in en_US locale.
 * \return The localized text.
 *
 * \sa SysText, local_SysText
 */
const char* GetSysText (const char* text);

/**
 * \var const char* SysText []
 * \brief Contains all text used by MiniGUI in English.
 *
 * System text defined as follows in MiniGUI:
 *
 * \code
 * const char* SysText [] =
 * {
 *    "Windows...",
 *    "Start...",
 *    "Refresh Background",
 *    "Close All Windows",
 *    "End Session",
 *    "Operations...",
 *    "Minimize",
 *    "Maximize",
 *    "Restore",
 *    "Close",
 *    "OK",
 *    "Next",
 *    "Cancel",
 *    "Previous",
 *    "Yes",
 *    "No",
 *    "Abort",
 *    "Retry",
 *    "Ignore",
 *    "About MiniGUI...",
 *    "Open File",
 *    "Save File",
 *    "Color Selection",
 *    NULL
 * };
 * \endcode
 *
 * \sa GetSysText, local_SysText
 */
extern const char* SysText [];

/**
 * \var const char** local_SysText
 * \brief The pointer to the current localized system text array.
 *
 * Changing \a local_SysText will lead to \a GetSysText returns a different 
 * localized system text. Please set it after calling \a SetDesktopRect, 
 * and send desktop a MSG_REINITSESSION message (call \a ReinitDesktop function) 
 * after assigned a different value to this variable.
 *
 * \sa GetSysText, SysText, ReinitDesktopEx
 */
extern const char** local_SysText;


    /** @} end of sys_text */

    /**
     * \defgroup str_helpers String operation helpers
     * @{
     */

/**
 * \fn char* strnchr (const char* s, size_t n, int c);
 * \brief Locates character in the first \a n characters of string \a s.
 *
 * \param s The pointer to the string.
 * \param n The number of first characters will be searched.
 * \param c The expected character.
 * \return Returns a pointer to the first occurrence of the character \a c in the string \a s
 *
 * \sa strchr(3)
 */
char* strnchr (const char* s, size_t n, int c);

/**
 * \fn int substrlen (const char* text, int len, char delimiter, int* nr_delim)
 * \brief Locates a substring delimited by one or more delimiters in the first \a len characters of string \a text.
 *
 * \param text The pointer to the string.
 * \param len The number of first characters will be searched.
 * \param delimiter The delimiter which delimites the substring from other.
 * \param nr_delim  The number of continuous delimiters will be returned through this pointer.
 * \return The length of the substring.
 *
 * \sa strstr(3)
 */
int substrlen (const char* text, int len, char delimiter, int* nr_delim);

/**
 * \fn char* strtrimall (char* src);
 * \brief deletes the space, form-feed('\\f'), newline('\\n'), carriage return('\\r'), horizontal tab('\\t'),and vertical tab('\\v') in the head and the tail of the string.
 *
 * \param src The pointer to the string.
 * \return Returns a pointer to the string.
 *
 * \sa strchr(3)
 */
char * strtrimall (char* src);


    /** @} end of str_helpers */

    /** @} end of global_fns */

    /** @} end of fns */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* _MGUI_MINIGUI_H */

