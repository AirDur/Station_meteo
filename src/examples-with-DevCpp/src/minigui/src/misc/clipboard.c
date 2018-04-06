/*
** $Id: clipboard.c,v 1.14 2005/01/31 10:54:09 clear Exp $
**
** clipboard.c: The implementation of Clipboard.
**
** Copyright (C) 2003 Feynman Software.
**
** Create date: 2003/12/xx
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

#ifdef _CLIPBOARD_SUPPORT

#include "minigui.h"
#include "clipboard.h"

#ifndef _LITE_VERSION
#include <semaphore.h>
#elif !defined (_STAND_ALONE)
#include "client.h"
#endif


typedef struct _CLIPBOARD
{
    char name [LEN_CLIPBOARD_NAME + 1];
    unsigned char *buff;
    size_t buff_len;
    size_t data_len;
} CLIPBOARD;


#ifndef _LITE_VERSION
static sem_t cb_lock;
#endif

static CLIPBOARD cbs [NR_CLIPBOARDS];

static int create_clip_board (const char* cb_name, size_t size)
{
    int i;
    int empty = -1;

    if (size <= 0)
        return -1;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            uHALr_printf( "create clip board error: already existed name.\n");
            return CBERR_BADNAME;
        }
        if (empty < 0 && cbs [i].name [0] == '\0') {
            empty = i;
        }
    }

    if (empty >= 0) {
        if ((cbs [empty].buff = malloc (size)) == NULL) {
            return CBERR_NOMEM;
        }
        strncpy (cbs [empty].name, cb_name, LEN_CLIPBOARD_NAME);
        cbs[empty].name[LEN_CLIPBOARD_NAME] = 0;
        cbs [empty].buff_len = size;
        cbs [empty].data_len = 0;
    }

    return CBERR_OK;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
int GUIAPI CreateClipBoard (const char* cb_name, size_t size)
{
    return create_clip_board (cb_name, size);
}
#else

int GUIAPI CreateClipBoard (const char* cb_name, size_t size)
{
    if (mgIsServer) {
        return create_clip_board (cb_name, size);
    }
    else {
        REQ_CLIPBOARD req_data;
        REQUEST request = {REQID_CLIPBOARD, &req_data, sizeof (REQ_CLIPBOARD)};
        int ret;

        req_data.id = CBREQ_CREATE;

        strncpy (req_data.name, cb_name, LEN_CLIPBOARD_NAME);
        req_data.name[LEN_CLIPBOARD_NAME] = '\0';

        req_data.len_data = size;

        cli_request (&request, &ret, sizeof(ret));

        return ret;
    }
}
#endif

#else

int GUIAPI CreateClipBoard (const char* cb_name, size_t size)
{
    int ret;

    sem_wait (&cb_lock);
    ret = create_clip_board (cb_name, size);
    sem_post (&cb_lock);

    return ret;
}

#endif

int GUIAPI destroy_clip_board (const char* cb_name)
{
    int i;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strcmp (cb_name, cbs [i].name) == 0) {
            free (cbs [i].buff);
            cbs [i].name [0] = '\0';
            cbs [i].buff = NULL;
            cbs [i].buff_len = 0;
            cbs [i].data_len = 0;

            return CBERR_OK;
        }
    }

    return CBERR_BADNAME;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
int GUIAPI DestroyClipBoard (const char* cb_name)
{
    return destroy_clip_board (cb_name);
}
#else
int GUIAPI DestroyClipBoard (const char* cb_name)
{
    if (mgIsServer) {
        return destroy_clip_board (cb_name);
    }
    else {
        REQ_CLIPBOARD req_data;
        REQUEST request = {REQID_CLIPBOARD, &req_data, sizeof (REQ_CLIPBOARD)};
        int ret;

        req_data.id = CBREQ_DESTROY;

        strncpy (req_data.name, cb_name, LEN_CLIPBOARD_NAME);
        req_data.name[LEN_CLIPBOARD_NAME] = '\0';

        cli_request (&request, &ret, sizeof (ret));

        return ret;
    }
}
#endif

#else

int GUIAPI DestroyClipBoard (const char* cb_name)
{
    int ret;

    sem_wait (&cb_lock);
    ret = destroy_clip_board (cb_name);
    sem_post (&cb_lock);

    return ret;
}
#endif

static int set_clip_board_data (const char* cb_name, void* data, size_t n, int cbop)
{
    int i;
    CLIPBOARD* cb = NULL;
    int dataLen;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            cb = cbs + i;
            break;
        }
    }

    if (cb == NULL)
        return CBERR_BADNAME;

    if (cbop == CBOP_APPEND) {
        dataLen = MIN (n, cb->buff_len - cb->data_len);
        memcpy (cb->buff + cb->data_len, data, dataLen);
        cb->data_len += dataLen;
    }
    else {
        cb->data_len = MIN (n, cb->buff_len);
        if (data && cb->data_len > 0)
            memcpy (cb->buff, data, cb->data_len);
    }

    return CBERR_OK;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
int GUIAPI SetClipBoardData (const char* cb_name, void* data, size_t n, int cbop)
{
    return set_clip_board_data (cb_name, data, n, cbop);
}
#else
int GUIAPI SetClipBoardData (const char* cb_name, void* data, size_t n, int cbop)
{
    if (mgIsServer) {
        return set_clip_board_data (cb_name, data, n, cbop);
    }
    else {
        REQ_CLIPBOARD* req_data;
        REQUEST request; 
        int ret;

        req_data = ALLOCATE_LOCAL (sizeof (REQ_CLIPBOARD) + n);
        if (req_data == NULL)
            return CBERR_NOMEM;

        req_data->id = CBREQ_SETDATA;
        strncpy (req_data->name, cb_name, LEN_CLIPBOARD_NAME);
        req_data->name[LEN_CLIPBOARD_NAME] = '\0';

        req_data->len_data = n;
        memcpy ((char*)(req_data+1), data, n);

        req_data->cbop = cbop;

        request.id = REQID_CLIPBOARD;
        request.data = req_data;
        request.len_data = sizeof (REQ_CLIPBOARD) + n;

        cli_request (&request, &ret, sizeof (ret));

        DEALLOCATE_LOCAL (req_data);
        return ret;
    }
}
#endif

#else

int GUIAPI SetClipBoardData (const char* cb_name, void* data, size_t n, int cbop)
{
    int ret;

    sem_wait (&cb_lock);
    ret = set_clip_board_data (cb_name, data, n, cbop);
    sem_post (&cb_lock);

    return ret;
}

#endif

static size_t get_clip_board_data_len (const char* cb_name)
{
    int i;
    CLIPBOARD* cb = NULL;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            cb = cbs + i;
            break;
        }
    }

    if (cb)
        return cb->data_len;
    return 0;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
size_t GUIAPI GetClipBoardDataLen (const char* cb_name)
{
    return get_clip_board_data_len (cb_name);
}
#else
size_t GUIAPI GetClipBoardDataLen (const char* cb_name)
{
    if (mgIsServer) {
        return get_clip_board_data_len (cb_name);
    }
    else {
        REQ_CLIPBOARD req_data;
        REQUEST request = {REQID_CLIPBOARD, &req_data, sizeof (REQ_CLIPBOARD)};
        int reply[2];

        req_data.id = CBREQ_GETDATALEN;
        strncpy (req_data.name, cb_name, LEN_CLIPBOARD_NAME);
        req_data.name[LEN_CLIPBOARD_NAME] = '\0';
        req_data.len_data = 0;

        cli_request (&request, &reply, sizeof (reply));

        if (reply[0] == CBERR_OK)
            return reply[1];
        else
            return reply[0];
    }
}
#endif

#else

size_t GUIAPI GetClipBoardDataLen (const char* cb_name)
{
    int ret;

    sem_wait (&cb_lock);
    ret = get_clip_board_data_len (cb_name);
    sem_post (&cb_lock);

    return ret;
}

#endif

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
static CLIPBOARD* get_clipboard (const char *cb_name)
{
    int i;
    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            return (cbs + i);
        }
    }
    return NULL;
}
#endif

static size_t get_clip_board_data (const char* cb_name, void* data, size_t n)
{
    int i;
    CLIPBOARD* cb = NULL;
    int copied_len = 0;

    if (!data || n <= 0)
        return -1;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            cb = cbs + i;
            break;
        }
    }

    if (cb) {
        copied_len = MIN (n, cb->data_len);
        if (copied_len > 0) {
            memcpy (data, cb->buff, copied_len);
        }
    }

    return copied_len;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
size_t GUIAPI GetClipBoardData (const char* cb_name, void* data, size_t n)
{
    return get_clip_board_data (cb_name, data, n);
}
#else
size_t GUIAPI GetClipBoardData (const char* cb_name, void* data, size_t n)
{
    if (mgIsServer) {
        return get_clip_board_data (cb_name, data, n);
    }
    else {
        REQ_CLIPBOARD req_data;
        REQUEST request = {REQID_CLIPBOARD, &req_data, sizeof (REQ_CLIPBOARD)};
        int* reply;

        reply = (int*)ALLOCATE_LOCAL (2*sizeof(int) + n);

        req_data.id = CBREQ_GETDATA;
        strncpy (req_data.name, cb_name, LEN_CLIPBOARD_NAME);
        req_data.name[LEN_CLIPBOARD_NAME] = '\0';
        //FIXME
        req_data.len_data = n;

        cli_request (&request, (void*)reply, 2*sizeof(int) + n);

        if (*reply == CBERR_OK) {
            memcpy (data, (void*)(reply+2), *(reply+1));
        }

        DEALLOCATE_LOCAL (reply);
        return *(reply+1);
    }
}
#endif

#else

size_t GUIAPI GetClipBoardData (const char* cb_name, void* data, size_t n)
{
    int ret;

    sem_wait (&cb_lock);
    ret = get_clip_board_data (cb_name, data, n);
    sem_post (&cb_lock);

    return ret;
}

#endif

static int get_clip_board_byte (const char* cb_name, int index, unsigned char* byte)
{
    int i;
    CLIPBOARD* cb = NULL;

    for (i = 0; i < NR_CLIPBOARDS; i++) {
        if (strncmp (cb_name, cbs [i].name, LEN_CLIPBOARD_NAME) == 0) {
            cb = cbs + i;
            break;
        }
    }

    if (cb) {
        if (index >= cb->data_len)
            return CBERR_NOMEM;
        *byte = cb->buff [index];
    }
    else
        return CBERR_BADNAME;

    return CBERR_OK;
}

#ifdef _LITE_VERSION
#ifdef _STAND_ALONE
int GUIAPI GetClipBoardByte (const char* cb_name, int index, unsigned char* byte)
{
    return get_clip_board_byte (cb_name, index, byte);
}
#else
int GUIAPI GetClipBoardByte (const char* cb_name, int index, unsigned char* byte)
{
    if (mgIsServer) {
        return get_clip_board_byte (cb_name, index, byte);
    }
    else {
        REQ_CLIPBOARD req_data;
        REQUEST request = {REQID_CLIPBOARD, &req_data, sizeof (REQ_CLIPBOARD)};
        int reply[2];

        req_data.id = CBREQ_GETBYTE;
        strncpy (req_data.name, cb_name, LEN_CLIPBOARD_NAME);
        req_data.name[LEN_CLIPBOARD_NAME] = '\0';
        req_data.len_data = index;

        cli_request (&request, &reply, sizeof (reply));

        if (reply[0] == CBERR_OK)
            *byte = (unsigned char) reply[1];

        return reply[0];
    }
}
#endif

#else

int GUIAPI GetClipBoardByte (const char* cb_name, int index, unsigned char* byte)
{
    int ret;

    sem_wait (&cb_lock);
    ret = get_clip_board_byte (cb_name, index, byte);
    sem_post (&cb_lock);

    return ret;
}

#endif

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
int send_reply (int clifd, const void* reply, int len);

int clipboard_op (int cli, int clifd, void* buff, size_t len)
{
    REQ_CLIPBOARD* request = (REQ_CLIPBOARD*) buff;
    int ret;

    switch (request->id) {
        case CBREQ_CREATE:
            ret = create_clip_board (request->name, request->len_data);
            break;

        case CBREQ_DESTROY:
            ret = destroy_clip_board (request->name);
            break;

        case CBREQ_SETDATA:
            ret = set_clip_board_data (request->name, (char*)buff + sizeof(REQ_CLIPBOARD), 
                                       request->len_data, request->cbop);
            break;

        case CBREQ_GETDATALEN:
        {
            int attlen;
            attlen = get_clip_board_data_len (request->name);
            ret = CBERR_OK;
            send_reply (clifd, &ret, sizeof (ret));
            sock_write (clifd, &attlen, sizeof(int));
            return SOCKERR_OK;
        }

        case CBREQ_GETDATA:
        {
            CLIPBOARD *cb = get_clipboard (request->name);
            int dlen;
            if (!cb) {
                ret = -1;
                send_reply (clifd, &ret, sizeof (ret));
                return SOCKERR_OK;
            }
            ret = CBERR_OK;
            send_reply (clifd, &ret, sizeof (ret));

            dlen =  MIN(request->len_data, cb->data_len);
            sock_write (clifd, &dlen, sizeof (dlen));

            sock_write (clifd, cb->buff, dlen);
            return SOCKERR_OK;
        }

        case CBREQ_GETBYTE:
        {
            unsigned char ch;
            get_clip_board_byte (request->name, request->len_data, &ch);
            ret = CBERR_OK;
            send_reply (clifd, &ret, sizeof (ret));
            sock_write (clifd, &ch, sizeof(ch));
            return SOCKERR_OK;
        }
    }

    ret = CBERR_OK;
    send_reply (clifd, &ret, sizeof (ret));

    return SOCKERR_OK;
}

#endif


void InitClipBoard (void)
{
#ifndef _LITE_VERSION
    sem_init (&cb_lock, 0, 1);
#endif

#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
#endif
        create_clip_board (CBNAME_TEXT, 1024);

}

void TerminateClipBoard (void)
{
#if defined(_LITE_VERSION) && !defined(_STAND_ALONE)
    if (mgIsServer)
#endif
    {
        int i;

        for (i = 0; i < NR_CLIPBOARDS; i++) {
            if (cbs [i].name [0])
                free (cbs [i].buff);
        }
    }

#ifndef _LITE_VERSION
    sem_destroy (&cb_lock);
#endif
}

#endif /* _CLIPBOARD_SUPPORT */

