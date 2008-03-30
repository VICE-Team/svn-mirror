/*
 * network.c - Connecting emulators via network.
 *
 * Written by
 *  Andreas Matthies <andreas.matthies@gmx.net>
 *  
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */


#include "vice.h"

#ifdef HAVE_NETWORK

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>

#include "archdep.h"
#include "event.h"
#include "interrupt.h"
#include "lib.h"
#include "machine.h"
#include "monitor/montypes.h"
#include "network.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "ui.h"
#include "util.h"

static enum {
    NETWORK_IDLE,
    NETWORK_SERVER,
    NETWORK_SERVER_CONNECTED,
    NETWORK_CLIENT
} network_mode = NETWORK_IDLE;

static int current_send_frame;
static int last_received_frame;
static SOCKET listen_socket;
static SOCKET network_socket;
static fd_set fdsockset;
static int network_init_done = 0;

static char *server_name = NULL;
static unsigned int server_port;
static int frame_delta;

static int frame_buffer_full;
static int current_frame, frame_to_play;
static event_list_state_t *frame_event_list = NULL;

static int set_server_name(resource_value_t v, void *param)
{
    util_string_set(&server_name, (const char *)v);
    return 0;
}

static int set_server_port(resource_value_t v, void *param)
{
    server_port = (CLOCK)v;
    return 0;
}

static int set_frame_delta(resource_value_t v, void *param)
{
    frame_delta = (CLOCK)v;
    return 0;
}

/*---------- Resources ------------------------------------------------*/

static const resource_t resources[] = {
    { "NetworkServerName", RES_STRING, (resource_value_t)"127.0.0.1",
      (void *)&server_name,
      set_server_name, NULL },
    { "NetworkServerPort", RES_INTEGER, (resource_value_t)6502,
      (void *)&server_port,
      set_server_port, NULL },
    { "NetworkFrameDelta", RES_INTEGER, (resource_value_t)5,
      (void *)&frame_delta,
      set_frame_delta, NULL },
    { NULL }
};


int network_resources_init(void)
{
    return resources_register(resources);
}
/*---------------------------------------------------------------------*/


int network_init(void)
{
    if (network_init_done)
        return 0;

    network_mode = NETWORK_IDLE;
    network_init_done = 1;

    return 0;
}

static void network_free_frame_event_list(void)
{
    int i;

    if (frame_event_list != NULL) {
        for (i=0; i < frame_delta; i++)
            event_clear_list(&(frame_event_list[i]));
        lib_free(frame_event_list);
        frame_event_list = NULL;
    }
}

static void network_event_record_sync_test(void)
{
    unsigned int regbuf[5];
        
    regbuf[0] = monitor_cpu_type.mon_register_get_val(e_comp_space, e_PC);
    regbuf[1] = monitor_cpu_type.mon_register_get_val(e_comp_space, e_A);
    regbuf[2] = monitor_cpu_type.mon_register_get_val(e_comp_space, e_X);
    regbuf[3] = monitor_cpu_type.mon_register_get_val(e_comp_space, e_Y);
    regbuf[4] = monitor_cpu_type.mon_register_get_val(e_comp_space, e_SP);

    network_event_record(EVENT_SYNC_TEST, (void *)regbuf, sizeof(regbuf));
}

static void network_init_frame_event_list(void)
{
    network_free_frame_event_list();
    frame_event_list = lib_malloc(sizeof(event_list_state_t) * frame_delta);
    memset(frame_event_list, 0, sizeof(event_list_state_t) * frame_delta);
    current_frame = 0;
    frame_buffer_full = 0;
    event_register_event_list(&(frame_event_list[0]));
    network_event_record_sync_test();
}

static void network_prepare_next_frame(void)
{
    current_frame = (current_frame + 1) % frame_delta;
    frame_to_play = (current_frame + 1) % frame_delta;
    event_clear_list(&(frame_event_list[current_frame]));
    event_register_event_list(&(frame_event_list[current_frame]));
    network_event_record_sync_test();
}

void network_event_record(unsigned int type, void *data, unsigned int size)
{
    event_record_in_list(&(frame_event_list[current_frame]), type, data, size);
}


static unsigned int network_create_event_buffer(char **buf, event_list_state_t *list)
{
    int size;
    char *bufptr;
    event_list_t *current_event, *last_event;
    int data_len = 0;
    int num_of_events;

    if (list == NULL)
        return 0;

    /* calculate the buffer length */
    num_of_events = 0;
    current_event = list->base;
    do {
        num_of_events++;
        data_len += current_event->size;
        last_event = current_event;
        current_event = current_event->next;
    } while (last_event->type != EVENT_LIST_END);

    size = num_of_events * (2 * sizeof(unsigned int) + sizeof(CLOCK)) + data_len;
    
    *buf = lib_malloc(size);
    
    /* fill the buffer with the events */
    current_event = list->base;
    bufptr = *buf;
    do {
        *((unsigned int*)bufptr)++ = current_event->type;
        *((CLOCK*)bufptr)++ = current_event->clk;
        *((unsigned int*)bufptr)++ = current_event->size;
        memcpy(bufptr, current_event->data, current_event->size);
        bufptr += current_event->size;
        last_event = current_event;
        current_event = current_event->next;
    } while (last_event->type != EVENT_LIST_END);

    return size;
}

static event_list_state_t *network_create_event_list(char *remote_event_buffer)
{
    event_list_state_t *list;
    unsigned int type, size;
    CLOCK clk;
    char *data;
    char *bufptr = remote_event_buffer;

    list = lib_malloc(sizeof(event_list_state_t));
    event_register_event_list(list);

    do {
        type = *((unsigned int*)bufptr)++;
        clk = *((CLOCK*)bufptr)++;
        size = *((unsigned int*)bufptr)++;
        data = bufptr;
        bufptr += size;
        event_record_in_list(list, type, data, size);
    } while (type != EVENT_LIST_END);

    return list;
}
/*-------------------------------------------------------------------------*/

int network_connected(void)
{
    if (network_mode == NETWORK_SERVER_CONNECTED 
        || network_mode ==  NETWORK_CLIENT)
        return 1;
    else
        return 0;
}

static void network_server_connect_trap(WORD addr, void *data)
{
    FILE *f;
    BYTE *buf;
    long buf_size;
    long i;
    char *directory, *filename;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "server", FSDEV_EXT_SEP_STR, "vsf", NULL);

    if (machine_write_snapshot(filename, 1, 1, 0) == 0) {
        f = fopen(filename, MODE_READ);
        if (f == NULL) {
            ui_error("Cannot load snapshot file for transfer");
            lib_free(filename);
            return;
        }
        buf_size = util_file_length(f);
        buf = lib_malloc(buf_size);
        fread(buf, 1, buf_size, f);
        fclose(f);
        send(network_socket, (char*)&buf_size, sizeof(long), 0);
        i = send(network_socket, buf, buf_size, 0);
        lib_free(buf);
        if (i != buf_size) {
            ui_error("Cannot send snapshot to client");
            lib_free(filename);
            return;
        }

        current_send_frame = 0;
        last_received_frame = 0;

        network_mode = NETWORK_SERVER_CONNECTED;
        network_init_frame_event_list();
        network_hook();
        /* ui_display_statustext("A Client has connected..."); */
        lib_free(filename);
    }
}

static void network_client_connect_trap(WORD addr, void *data)
{
    char *directory, *filename;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "client", FSDEV_EXT_SEP_STR, "vsf", NULL);

    if (machine_read_snapshot(filename, 0) != 0) {
        ui_error("Cannot open sbapshot file %s", filename);
        lib_free(filename);
        return;
    }

    current_send_frame = 0;
    last_received_frame = 0;

    network_mode = NETWORK_CLIENT;
    network_init_frame_event_list();
    network_hook();
    /* ui_error("Client connected"); */
    lib_free(filename);
}
                        

int network_start_server(void)
{
    struct sockaddr_in server_addr;

    if (network_init() < 0)
        return -1;

    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr.s_addr = htonl(0);
    server_addr.sin_family = PF_INET;
    memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

    listen_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listen_socket == INVALID_SOCKET)
        return -1;

    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(listen_socket);
        return -1;
    }

    if (listen(listen_socket, 2) == SOCKET_ERROR) {
        closesocket(listen_socket);
        return -1;
    }

    network_mode = NETWORK_SERVER;

    ui_error("Server is running...");

    return 0;
} 


int network_connect_client(void)
{
    struct sockaddr_in server_addr;
    struct hostent *server_hostent;
    FILE *f;
    BYTE *buf;
    long buf_size;
    char *directory, *filename;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "client", FSDEV_EXT_SEP_STR, "vsf", NULL);

    if (network_init() < 0)
        return -1;

    server_hostent = gethostbyname(server_name);
    if (server_hostent == NULL) {
        ui_error("Cannot resolve %s", server_name);
        return -1;
    }
    server_addr.sin_port = htons(server_port);
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr = *(struct in_addr *)server_hostent->h_addr_list[0];

    network_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (network_socket == INVALID_SOCKET)
        return -1;

    if(connect(network_socket, (struct sockaddr *)&server_addr, 
        sizeof(server_addr)) == SOCKET_ERROR)
    {
        closesocket(network_socket);
        return -1;
    }

    if (recv(network_socket, (char*)&buf_size, sizeof(long), 0) != sizeof(long))
        return -1;

    buf = lib_malloc(buf_size);

    if (recv(network_socket, buf, buf_size, 0) != buf_size)
        return -1;

    f = fopen(filename, MODE_WRITE);
    if (f == NULL) {
        ui_error("Cannot read received snapshot file");
        lib_free(filename);
        return -1;
    }
    fwrite(buf, 1, buf_size, f);
    fclose(f);
    lib_free(buf);
    lib_free(filename);

    interrupt_maincpu_trigger_trap(network_client_connect_trap, (void *)0);

    return 0;
}


void network_hook(void)
{
    const TIMEVAL time_out = {0, 0};

    if (network_mode == NETWORK_IDLE)
        return;

    if (network_mode == NETWORK_SERVER) {
        FD_ZERO(&fdsockset);
        FD_SET(listen_socket, &fdsockset);

        if (select(1, &fdsockset, 0, 0, &time_out) != 0) {
            network_socket = accept(listen_socket, NULL, NULL);
        
            if (network_socket != INVALID_SOCKET)
                interrupt_maincpu_trigger_trap(network_server_connect_trap, (void *)0);
        }
    }

    if (network_mode == NETWORK_SERVER_CONNECTED
        || network_mode == NETWORK_CLIENT) 
    {
        unsigned int temp;
        char *local_event_buf, *remote_event_buf;
        unsigned int local_buf_len, remote_buf_len;
        event_list_state_t *remote_event_list;
        event_list_state_t *client_event_list, *server_event_list;

        /* create and send current event buffer */
        network_event_record(EVENT_LIST_END, NULL, 0);
        local_buf_len = network_create_event_buffer(&local_event_buf, &(frame_event_list[current_frame]));
        send(network_socket, (char*)&local_buf_len, sizeof(unsigned int), 0);
        send(network_socket, local_event_buf, local_buf_len, 0);
        lib_free(local_event_buf);

        /* receive event buffer */
        if (current_frame == frame_delta - 1)
            frame_buffer_full = 1;

        if (frame_buffer_full) {
            temp = 0;
            while(temp < sizeof(unsigned int)) {
                int t;
                t = recv(network_socket, (char*)&remote_buf_len, sizeof(unsigned int), 0);
                if (t < 0)
                    return;
                temp += t;
                if (temp < sizeof(unsigned int))
                    ui_error("fragmented");
            }
            remote_event_buf = lib_malloc(remote_buf_len);
            temp = 0;
            while (temp < remote_buf_len) {
                int t;

                t = recv(network_socket, remote_event_buf + temp, remote_buf_len, 0);
                if (t < 0)
                    return;

                temp += t;
                if (temp < remote_buf_len)
                    ui_error("fragmented");
            }
            remote_event_list = network_create_event_list(remote_event_buf);
            lib_free(remote_event_buf);

            if (network_mode == NETWORK_SERVER_CONNECTED) {
                client_event_list = remote_event_list;
                server_event_list = &(frame_event_list[frame_to_play]);
            } else {
                server_event_list = remote_event_list;
                client_event_list = &(frame_event_list[frame_to_play]);
            }

            /* test for sync */
            if (client_event_list->base->type == EVENT_SYNC_TEST
                && server_event_list->base->type == EVENT_SYNC_TEST)
            {
                int i;
                
                for (i = 0; i < 5; i++)
                    if (((unsigned int*)client_event_list->base->data)[i]
                        != ((unsigned int*)server_event_list->base->data)[i])
                    {
                        ui_error("Network out of sync");
                        /* force resync */
                        break;
                    }
            }

            /* replay the event_lists; server first, then client */
            event_playback_event_list(server_event_list);
            event_playback_event_list(client_event_list);

            event_clear_list(remote_event_list);
            lib_free(remote_event_list);
        }
        network_prepare_next_frame();
    }

}

#endif

