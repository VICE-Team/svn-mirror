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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_NETWORK
#ifdef WIN32
#include <winsock.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#ifndef __MSDOS__
#include <sys/time.h>
#include <sys/select.h>
#include <unistd.h>
#endif

typedef unsigned int SOCKET;
typedef struct timeval TIMEVAL;

#define closesocket close

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (SOCKET)(~0)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)
#endif

#endif /* WIN32 */
#endif /* HAVE_NETWORK */

#include "archdep.h"
#include "event.h"
#include "interrupt.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mos6510.h"
#include "network.h"
#include "resources.h"
#ifdef HAS_TRANSLATION
#include "translate.h"
#endif
#include "types.h"
#include "ui.h"
#include "uiapi.h"
#include "util.h"
#include "vsync.h"
#include "vsyncapi.h"

/* #define NETWORK_DEBUG */

static network_mode_t network_mode = NETWORK_IDLE;

#ifdef HAVE_NETWORK
static int current_send_frame;
static int last_received_frame;
static SOCKET listen_socket;
static SOCKET network_socket;
static fd_set fdsockset;
static int network_init_done = 0;
static int suspended;

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

/*---------- Resources ------------------------------------------------*/

static const resource_t resources[] = {
    { "NetworkServerName", RES_STRING, (resource_value_t)"127.0.0.1",
      (void *)&server_name,
      set_server_name, NULL },
    { "NetworkServerPort", RES_INTEGER, (resource_value_t)6502,
      (void *)&server_port,
      set_server_port, NULL },
    { NULL }
};
#endif

int network_resources_init(void)
{
#ifdef HAVE_NETWORK
    return resources_register(resources);
#else
    return 0;
#endif
}
/*---------------------------------------------------------------------*/

#ifdef HAVE_NETWORK

static int network_init(void)
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
    event_destroy_image_list();
}

static void network_event_record_sync_test(WORD addr, void *data)
{
    unsigned int regbuf[5];
        
    regbuf[0] = maincpu_regs.pc;
    regbuf[1] = maincpu_regs.a;
    regbuf[2] = maincpu_regs.x;
    regbuf[3] = maincpu_regs.y;
    regbuf[4] = maincpu_regs.sp;

    network_event_record(EVENT_SYNC_TEST, (void *)regbuf, sizeof(regbuf));
}

static void network_init_frame_event_list(void)
{
    frame_event_list = lib_malloc(sizeof(event_list_state_t) * frame_delta);
    memset(frame_event_list, 0, sizeof(event_list_state_t) * frame_delta);
    current_frame = 0;
    frame_buffer_full = 0;
    event_register_event_list(&(frame_event_list[0]));
    event_init_image_list();
    interrupt_maincpu_trigger_trap(network_event_record_sync_test, (void *)0);
}

static void network_prepare_next_frame(void)
{
    current_frame = (current_frame + 1) % frame_delta;
    frame_to_play = (current_frame + 1) % frame_delta;
    event_clear_list(&(frame_event_list[current_frame]));
    event_register_event_list(&(frame_event_list[current_frame]));
    interrupt_maincpu_trigger_trap(network_event_record_sync_test, (void *)0);
}

static unsigned int network_create_event_buffer(BYTE **buf,
                                                event_list_state_t *list)
{
    int size;
    BYTE *bufptr;
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

    size = num_of_events * 3 * sizeof(DWORD) + data_len;
    
    *buf = lib_malloc(size);
    
    /* fill the buffer with the events */
    current_event = list->base;
    bufptr = *buf;
    do {
        util_dword_to_le_buf(&bufptr[0], (DWORD)(current_event->type));
        util_dword_to_le_buf(&bufptr[4], (DWORD)(current_event->clk));
        util_dword_to_le_buf(&bufptr[8], (DWORD)(current_event->size));
        memcpy(&bufptr[12], current_event->data, current_event->size);
        bufptr += 12 + current_event->size;
        last_event = current_event;
        current_event = current_event->next;
    } while (last_event->type != EVENT_LIST_END);

    return size;
}

static event_list_state_t *network_create_event_list(BYTE *remote_event_buffer)
{
    event_list_state_t *list;
    unsigned int type, size;
    CLOCK clk;
    BYTE *data;
    BYTE *bufptr = remote_event_buffer;

    list = lib_malloc(sizeof(event_list_state_t));
    event_register_event_list(list);

    do {
        type = util_le_buf_to_dword(&bufptr[0]);
        clk = util_le_buf_to_dword(&bufptr[4]);
        size = util_le_buf_to_dword(&bufptr[8]);
        data = &bufptr[12];
        bufptr += 12 + size;
        event_record_in_list(list, type, data, size);
    } while (type != EVENT_LIST_END);

    return list;
}

static int network_recv_buffer(SOCKET s, BYTE *buf, int len)
{
    int t;
    int received_total = 0;

    while (received_total < len) {
        t = recv(s, buf, len - received_total, 0);
        
        if (t < 0)
            return t;

        received_total += t;
        buf += t;
    }
    return 0;
}

static int network_send_buffer(SOCKET s, const BYTE *buf, int len)
{
    int t;
    int sent_total = 0;

    while (sent_total < len) {
        t = send(s, buf, len - sent_total, 0);
        
        if (t < 0)
            return t;

        sent_total += t;
        buf += t;
    }
    return 0;
}

#define NUM_OF_TESTPACKETS 50

static void network_test_delay(void)
{
    int i, j;
    BYTE new_frame_delta;
    BYTE buf[0x60];
    long packet_delay[NUM_OF_TESTPACKETS];
    char st[256];

    vsyncarch_init();

#ifdef HAS_TRANSLATION
    ui_display_statustext(translate_text(IDGS_TESTING_BEST_FRAME_DELAY), 0);
#else
    ui_display_statustext(_("Testing best frame delay..."), 0);
#endif

    if (network_mode == NETWORK_SERVER_CONNECTED) {
        for (i = 0; i < NUM_OF_TESTPACKETS; i++) {
            *((unsigned long*)buf) = vsyncarch_gettime();
            if (network_send_buffer(network_socket, buf, sizeof(buf)) <  0
                || network_recv_buffer(network_socket, buf, sizeof(buf)) <  0)
                return;
            packet_delay[i] = vsyncarch_gettime() - *((unsigned long*)buf);
        }
        /* Sort the packets delays*/
        for (i = 0; i < NUM_OF_TESTPACKETS - 1; i++) {
            for (j = i + 1; j < NUM_OF_TESTPACKETS; j++) {
                if (packet_delay[i] < packet_delay[j]) {
                    long d = packet_delay[i];
                    packet_delay[i] = packet_delay[j];
                    packet_delay[j] = d;
                }
            }
#ifdef NETWORK_DEBUG
            log_debug("packet_delay[%d]=%d",i,packet_delay[i]);
#endif
        }
#ifdef NETWORK_DEBUG
        log_debug("vsyncarch_frequency = %d", vsyncarch_frequency());
#endif
        /* calculate delay with 90% of packets beeing fast enough */
        /* FIXME: This needs some further investigation */
        new_frame_delta = 5 + 2 * (BYTE)(vsync_get_refresh_frequency()
                            * packet_delay[(int)(0.1 * NUM_OF_TESTPACKETS)]
                            / (float)vsyncarch_frequency());
        network_send_buffer(network_socket, &new_frame_delta, sizeof(new_frame_delta));
    } else {
        /* network_mode == NETWORK_CLIENT */
        for (i = 0; i < NUM_OF_TESTPACKETS; i++) {
            if (network_recv_buffer(network_socket, buf, sizeof(buf)) <  0
                || network_send_buffer(network_socket, buf, sizeof(buf)) < 0)
                return;
        }
        network_recv_buffer(network_socket, &new_frame_delta, sizeof(new_frame_delta));
    }
    network_free_frame_event_list();
    frame_delta = new_frame_delta;
    network_init_frame_event_list();
#ifdef HAS_TRANSLATION
    sprintf(st, translate_text(IDGS_USING_D_FRAMES_DELAY), frame_delta);
#else
    sprintf(st, _("Using %d frames delay."), frame_delta);
#endif
    log_debug("netplay connected with %d frames delta.", frame_delta);
    ui_display_statustext(st, 1);
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
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANNOT_LOAD_SNAPSHOT_TRANSFER));
#else
            ui_error(_("Cannot load snapshot file for transfer"));
#endif
            lib_free(filename);
            return;
        }
        buf_size = util_file_length(f);
        buf = lib_malloc(buf_size);
        fread(buf, 1, buf_size, f);
        fclose(f);

#ifdef HAS_TRANSLATION
        ui_display_statustext(translate_text(IDGS_SENDING_SNAPSHOT_TO_CLIENT), 0);
#else
        ui_display_statustext(_("Sending snapshot to client..."), 0);
#endif
        network_send_buffer(network_socket, (char*)&buf_size, sizeof(long));
        i = network_send_buffer(network_socket, buf, buf_size);
        lib_free(buf);
        if (i < 0) {
#ifdef HAS_TRANSLATION
            ui_error(translate_text(IDGS_CANNOT_SEND_SNAPSHOT_TO_CLIENT));
#else
            ui_error(_("Cannot send snapshot to client"));
#endif
            ui_display_statustext("", 0);
            lib_free(filename);
            return;
        }

        current_send_frame = 0;
        last_received_frame = 0;

        network_mode = NETWORK_SERVER_CONNECTED;
        network_test_delay();
    } else {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S), filename);
#else
        ui_error(_("Cannot create snapshot file %s"), filename);
#endif
    }
    lib_free(filename);
}

static void network_client_connect_trap(WORD addr, void *data)
{
    char *directory, *filename;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "client", FSDEV_EXT_SEP_STR, "vsf", NULL);

    if (machine_read_snapshot(filename, 0) != 0) {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_OPEN_SNAPSHOT_FILE_S), filename);
#else
        ui_error(_("Cannot open snapshot file %s"), filename);
#endif
        lib_free(filename);
        return;
    }

    current_send_frame = 0;
    last_received_frame = 0;

    network_mode = NETWORK_CLIENT;
    network_test_delay();
    lib_free(filename);
}
#endif
/*-------------------------------------------------------------------------*/

void network_event_record(unsigned int type, void *data, unsigned int size)
{
#ifdef HAVE_NETWORK
    event_record_in_list(&(frame_event_list[current_frame]), type, data, size);
#endif
}

void network_attach_image(unsigned int unit, const char *filename)
{
#ifdef HAVE_NETWORK
    event_record_attach_in_list(&(frame_event_list[current_frame]), unit, filename, 1);
#endif
}

int network_get_mode(void)
{
    return network_mode;
}

int network_connected(void)
{
#ifdef HAVE_NETWORK
    if (network_mode == NETWORK_SERVER_CONNECTED 
        || network_mode ==  NETWORK_CLIENT)
        return 1;
    else
        return 0;
#else
    return 0;
#endif
}

int network_start_server(void)
{
#ifdef HAVE_NETWORK
    struct sockaddr_in server_addr;
    char *directory, *filename;
    FILE *fd;

    if (network_init() < 0)
        return -1;

    if (network_mode != NETWORK_IDLE)
        return -1;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "server", FSDEV_EXT_SEP_STR, "vsf", NULL);
    fd = fopen(filename, MODE_WRITE);
    if (fd == NULL) {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S), filename);
#else
        ui_error(_("Cannot create snapshot file %s. Select different history directory!"), filename);
#endif
        lib_free(filename);
        return -1;
    }

    lib_free(filename);
    fclose(fd);

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

    vsync_suspend_speed_eval();
#ifdef HAS_TRANSLATION
    ui_display_statustext(translate_text(IDGS_SERVER_IS_WAITING_FOR_CLIENT), 1);
#else
    ui_display_statustext(_("Server is waiting for a client..."), 1);
#endif
#endif
    return 0;
} 


int network_connect_client(void)
{
#ifdef HAVE_NETWORK
    struct sockaddr_in server_addr;
    struct hostent *server_hostent;
    FILE *f;
    BYTE *buf;
    long buf_size;
    char *directory, *filename;

    if (network_init() < 0)
        return -1;

    if (network_mode != NETWORK_IDLE)
        return -1;

    resources_get_value("EventSnapshotDir", (void *)&directory);
    filename = util_concat(directory, "client", FSDEV_EXT_SEP_STR, "vsf", NULL);
    f = fopen(filename, MODE_WRITE);
    if (f == NULL) {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_CREATE_SNAPSHOT_FILE_S), filename);
#else
        ui_error(_("Cannot create snapshot file %s. Select different history directory!"), filename);
#endif
        lib_free(filename);
        return -1;
    }

    server_hostent = gethostbyname(server_name);
    if (server_hostent == NULL) {
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_RESOLVE_S), server_name);
#else
        ui_error(_("Cannot resolve %s"), server_name);
#endif
        return -1;
    }
    server_addr.sin_port = htons(server_port);
    server_addr.sin_family = PF_INET;
    server_addr.sin_addr = *(struct in_addr *)server_hostent->h_addr_list[0];

    network_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (network_socket == INVALID_SOCKET) {
        lib_free(filename);
        return -1;
    }

    if (connect(network_socket, (struct sockaddr *)&server_addr, 
        sizeof(server_addr)) == SOCKET_ERROR) {
        closesocket(network_socket);
#ifdef HAS_TRANSLATION
        ui_error(translate_text(IDGS_CANNOT_CONNECT_TO_S),
                    server_name, server_port);
#else
        ui_error(_("Cannot connect to %s (no server running on port %d)."),
                    server_name, server_port);
#endif
        lib_free(filename);
        return -1;
    }

#ifdef HAS_TRANSLATION
    ui_display_statustext(translate_text(IDGS_RECEIVING_SNAPSHOT_SERVER), 0);
#else
    ui_display_statustext(_("Receiving snapshot from server..."), 0);
#endif
    if (network_recv_buffer(network_socket, (char*)&buf_size, sizeof(long)) < 0)
    {
        lib_free(filename);
        return -1;
    }

    buf = lib_malloc(buf_size);

    if (network_recv_buffer(network_socket, buf, buf_size) <  0)
        return -1;

    fwrite(buf, 1, buf_size, f);
    fclose(f);
    lib_free(buf);
    lib_free(filename);

    interrupt_maincpu_trigger_trap(network_client_connect_trap, (void *)0);
    vsync_suspend_speed_eval();
#endif
    return 0;
}

void network_disconnect(void)
{
#ifdef HAVE_NETWORK
    closesocket(network_socket);
    closesocket(listen_socket);
    network_mode = NETWORK_IDLE;
#endif
}

void network_suspend(void)
{
#ifdef HAVE_NETWORK
    int dummy_buf_len = 0;

    if (!network_connected() || suspended == 1)
        return;

    network_send_buffer(network_socket, (char*)&dummy_buf_len, 
                        sizeof(unsigned int));
#endif
}

void network_hook(void)
{
#ifdef HAVE_NETWORK
    TIMEVAL time_out = {0, 0};

    if (network_mode == NETWORK_IDLE)
        return;

    if (network_mode == NETWORK_SERVER) {
        FD_ZERO(&fdsockset);
        FD_SET(listen_socket, &fdsockset);

        if (select(1, &fdsockset, 0, 0, &time_out) != 0) {
            network_socket = accept(listen_socket, NULL, NULL);
        
            if (network_socket != INVALID_SOCKET)
                interrupt_maincpu_trigger_trap(network_server_connect_trap,
                                               (void *)0);
        }
    }

    if (network_mode == NETWORK_SERVER_CONNECTED
        || network_mode == NETWORK_CLIENT) 
    {
        BYTE *local_event_buf = NULL, *remote_event_buf = NULL;
        unsigned int local_buf_len, remote_buf_len;
        event_list_state_t *remote_event_list;
        event_list_state_t *client_event_list, *server_event_list;

        suspended = 0;

        /* create and send current event buffer */
        network_event_record(EVENT_LIST_END, NULL, 0);
        local_buf_len = network_create_event_buffer(&local_event_buf, &(frame_event_list[current_frame]));
#ifdef NETWORK_DEBUG
        log_debug("network hook before send: %d",vsyncarch_gettime());
#endif
        network_send_buffer(network_socket, (char*)&local_buf_len, sizeof(unsigned int));
        network_send_buffer(network_socket, local_event_buf, local_buf_len);
#ifdef NETWORK_DEBUG
        log_debug("network hook after send : %d",vsyncarch_gettime());
#endif
        lib_free(local_event_buf);

        /* receive event buffer */
        if (current_frame == frame_delta - 1)
            frame_buffer_full = 1;

        if (frame_buffer_full) {
            do {
                if (network_recv_buffer(network_socket, (char*)&remote_buf_len,
                                        sizeof(unsigned int)) < 0)
                {
                    ui_display_statustext("Remote host disconnected.", 1);
                    network_disconnect();
                    return;
                }

                if (remote_buf_len == 0 && suspended == 0) {
                    /* remote host suspended emulation */
                    ui_display_statustext("Remote host suspending...", 0);
                    suspended = 1;
                    vsync_suspend_speed_eval();
                }
            } while(remote_buf_len == 0);

            if (suspended == 1)
                ui_display_statustext("", 0);

            remote_event_buf = lib_malloc(remote_buf_len);

            if (network_recv_buffer(network_socket, remote_event_buf,
                                    remote_buf_len) < 0)
                return;
#ifdef NETWORK_DEBUG
            log_debug("network hook after recv : %d",vsyncarch_gettime());
#endif
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
#ifdef HAS_TRANSLATION
                        ui_error(translate_text(IDGS_NETWORK_OUT_OF_SYNC));
#else
                        ui_error(_("Network out of sync - disconnecting."));
#endif
                        network_disconnect();
                        /* shouldn't happen but resyncing would be nicer */
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
#ifdef NETWORK_DEBUG
        log_debug("network hook end        : %d",vsyncarch_gettime());
#endif
    }
#endif
}

void network_shutdown(void)
{
#ifdef HAVE_NETWORK
    network_free_frame_event_list();
    lib_free(server_name);
#endif
}
