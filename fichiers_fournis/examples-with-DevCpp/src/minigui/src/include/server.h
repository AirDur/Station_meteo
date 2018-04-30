/*
** $Id: server.h,v 1.14 2003/08/12 07:46:18 weiym Exp $
**
** server.h: routines for server.
**
** Copyright (C) 2003 Feynman Software.
** Copyright (C) 2000 ~ 2002 Wei Yongming.
**
** Create date: 2000/12/xx
*/

#ifndef GUI_SERVER_H
    #define GUI_SERVER_H

#define LOCKFILE    "/var/tmp/mginit"
#define CS_PATH     "/var/tmp/minigui" /* well-known name */ 

#define THRES_LIVE  200                 /* 2 seconds */
#define TO_SOCKIO   200                 /* 2 seconds */

#define SIZE_SPARERECTHEAP  20

extern BLOCKHEAP __mg_free_spare_rect_list;

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MINIGUI_H
int serv_listen (const char* name);
int serv_accept (int listenfd, pid_t *pidptr, uid_t *uidptr);
#endif

int  client_add (int fd, pid_t pid, uid_t uid);
void client_del (int cli);

void release_global_res (int cli);

void remove_client (int cli, int clifd);

int handle_request (int clifd, int req_id, int cli);

int my_send2client (MSG* msg, MG_Client* client);
void set_active_client (MG_Client* client);

MG_Layer* get_layer (const char* layer_name, BOOL does_create);
void get_layer_info (int cli, const char* layer_name, LAYERINFO* info);
void client_join_layer (int cli, 
                const JOINLAYERINFO* info, JOINEDCLIENTINFO* joined_info);
BOOL remove_layer (MG_Layer* layer);
BOOL is_valid_layer (MG_Layer* layer);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // GUI_SERVER_H

