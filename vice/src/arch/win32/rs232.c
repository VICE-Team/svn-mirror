/*
 * rs232.c - RS232 emulation.
 *
 * Written by
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

/*
 * The RS232 emulation captures the bytes sent to the RS232 interfaces
 * available (currently ACIA 6551, std C64 and Daniel Dallmanns fast RS232
 * with 9600 Baud).
 *
 * I/O is done to a socket.  If the socket isnt connected, no data
 * is read and written data is discarded.
 */

#undef        DEBUG

#include "vice.h"

#include <errno.h>
#include <string.h>
#include <winsock.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "types.h"
#include "util.h"

#define MAXRS232 4

/* ------------------------------------------------------------------------- */

/* resource handling */

#define NUM_DEVICES 4

static char *devfile[NUM_DEVICES] = { NULL, NULL, NULL, NULL };

static int set_devfile(resource_value_t v, void *param)
{
    util_string_set(&devfile[(int)param], (const char *)v);
    return 0;
}

/* ------------------------------------------------------------------------- */

static const resource_t resources[] = {
    { "RsDevice1", RES_STRING, (resource_value_t)"10.0.0.1:25232",
      (void *)&devfile[0], set_devfile, (void *)0 },
    { "RsDevice2", RES_STRING, (resource_value_t)"10.0.0.1:25232",
      (void *)&devfile[1], set_devfile, (void *)0 },
    { "RsDevice3", RES_STRING, (resource_value_t)"10.0.0.1:25232",
      (void *)&devfile[2], set_devfile, (void *)0 },
    { "RsDevice4", RES_STRING, (resource_value_t)"10.0.0.1:25232",
      (void *)&devfile[3], set_devfile, (void *)0 },
    { NULL }
};

int rs232_resources_init(void)
{
    return resources_register(resources);
}

void rs232_resources_shutdown(void)
{
    lib_free(devfile[0]);
}

static const cmdline_option_t cmdline_options[] = {
    { "-rsdev1", SET_RESOURCE, 1, NULL, NULL, "RsDevice1", NULL,
      "<name>", N_("Specify name of first RS232 device (10.0.0.1:25232)") },
    { "-rsdev2", SET_RESOURCE, 1, NULL, NULL, "RsDevice2", NULL,
      "<name>", N_("Specify name of second RS232 device (10.0.0.1:25232)") },
    { "-rsdev3", SET_RESOURCE, 1, NULL, NULL, "RsDevice3", NULL,
      "<name>", N_("Specify name of third RS232 device (10.0.0.1:25232)") },
    { "-rsdev4", SET_RESOURCE, 1, NULL, NULL, "RsDevice4", NULL,
      "<name>", N_("Specify name of fourth RS232 device (10.0.0.1:25232)") },
    { NULL }
};

int rs232_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

typedef struct rs232 {
    int inuse;
    SOCKET fd;
    char *file;
} rs232_t;

static rs232_t fds[MAXRS232];

static log_t rs232_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

void rs232_close(int fd);

/* initializes all RS232 stuff */
void rs232_init(void)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int i;

    WSAStartup(wVersionRequested, &wsaData);

    for (i = 0; i < MAXRS232; i++)
        fds[i].inuse = 0;

    rs232_log = log_open("RS232");
}

/* reset RS232 stuff */
void rs232_reset(void)
{
    int i;

    for (i = 0; i < MAXRS232; i++) {
        if (fds[i].inuse) {
            rs232_close(i);
        }
    }
}

static int
getaddr(char *dev, struct sockaddr_in *ad)
{
    char *p;

    memset(ad, 0, sizeof ad);
    ad->sin_family = AF_INET;

    dev = strdup(dev);
    p = strchr(dev, ':');
    if(!p) {
        free(dev);
        return -1;
    }

    *p = 0;
    ad->sin_addr.s_addr = inet_addr(dev);
    ad->sin_port = htons((unsigned short)atoi(p+1));
    free(dev);
    if(ad->sin_addr.s_addr == -1 || ad->sin_port == 0)
        return -1;

    return 0;
}

/* opens a rs232 window, returns handle to give to functions below. */
int rs232_open(int device)
{
    struct sockaddr_in ad;
    int i;

    // parse the address
    if(getaddr(devfile[device], &ad) == -1) {
        log_error(rs232_log, "Bad device name.  Should be ipaddr:port.");
        return -1;
    }

    for (i = 0; i < MAXRS232; i++) {
        if (!fds[i].inuse)
            break;
    }
    if (i >= MAXRS232) {
        log_error(rs232_log, _("No more devices available."));
        return -1;
    }

#ifdef DEBUG
    log_message(rs232_log, _("rs232_open(device=%d)."), device);
#endif

    // connect socket
    fds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fds[i].fd == INVALID_SOCKET ||
       connect(fds[i].fd, (struct sockaddr*)&ad, sizeof ad) == -1) {
        log_error(rs232_log, "Cant open connection.");
        return -1;
    }

    fds[i].inuse = 1;
    fds[i].file = devfile[device];
    return i;
}

/* closes the rs232 window again */
void rs232_close(int fd)
{
#ifdef DEBUG
    log_debug(rs232_log, "close(fd=%d).", fd);
#endif

    if (fd < 0 || fd >= MAXRS232) {
        log_error(rs232_log, _("Attempt to close invalid fd %d."), fd);
        return;
    }
    if (!fds[fd].inuse) {
        log_error(rs232_log, _("Attempt to close non-open fd %d."), fd);
        return;
    }

    close(fds[fd].fd);
    fds[fd].inuse = 0;
}

/* sends a byte to the RS232 line */
int rs232_putc(int fd, BYTE b)
{
    size_t n;

    if (fd < 0 || fd >= MAXRS232) {
        log_error(rs232_log, _("Attempt to write to invalid fd %d."), fd);
        return -1;
    }
    if (!fds[fd].inuse) {
        log_error(rs232_log, _("Attempt to write to non-open fd %d."), fd);
        return -1;
    }

    /* silently drop if socket is shut */
    if (fds[fd].fd < 0)
        return 0;

    /* for the beginning... */
#ifdef DEBUG
    log_message(rs232_log, "Output `%c'.", b);
#endif

    n = send(fds[fd].fd, &b, 1, 0);
    if (n != 1) {
        log_error(rs232_log, _("Error writing: %s."), strerror(errno));
        close(fds[fd].fd);
        fds[fd].fd = -1;
        return -1;
    }

    return 0;
}

/* gets a byte to the RS232 line, returns !=0 if byte received, byte in *b. */
int rs232_getc(int fd, BYTE * b)
{
    int ret;
    size_t n;
    fd_set rdset;
    struct timeval ti;

    if (fd < 0 || fd >= MAXRS232) {
        log_error(rs232_log, _("Attempt to read from invalid fd %d."), fd);
        return -1;
    }
    if (!fds[fd].inuse) {
        log_error(rs232_log, _("Attempt to read from non-open fd %d."), fd);
        return -1;
    }

    /* silently drop if socket is shut */
    if (fds[fd].fd < 0)
        return 0;

    FD_ZERO(&rdset);
    FD_SET(fds[fd].fd, &rdset);
    ti.tv_sec = ti.tv_usec = 0;
    ret = select(fds[fd].fd + 1, &rdset, NULL, NULL, &ti);

    if (ret > 0 && (FD_ISSET(fds[fd].fd, &rdset))) {
        n = recv(fds[fd].fd, b, 1, 0);
        if (n != 1) {
            if(n < 0)
                log_error(rs232_log, _("Error reading: %s."), strerror(errno));
            else
                log_error(rs232_log, _("EOF"));
            close(fds[fd].fd);
            fds[fd].fd = -1;
            return -1;
        }
        return 1;
    }
    return 0;
}

