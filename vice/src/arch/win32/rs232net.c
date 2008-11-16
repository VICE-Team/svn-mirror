/*
 * rs232net.c - RS232 emulation.
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
/* #define DEBUG */

#include "vice.h"

#include <errno.h>
#include <string.h>
#include <winsock.h>

#ifdef HAVE_IO_H
#include <io.h>
#endif

#include "log.h"
#include "rs232.h"
#include "types.h"
#include "util.h"

#ifdef DEBUG
# define DEBUG_LOG_MESSAGE(_xxx) log_message _xxx
#else
# define DEBUG_LOG_MESSAGE(_xxx)
#endif

/* ------------------------------------------------------------------------- */

int rs232net_resources_init(void)
{
    return 0;
}

void rs232net_resources_shutdown(void)
{
}

int rs232net_cmdline_options_init(void)
{
    return 0;
}

/* ------------------------------------------------------------------------- */

typedef struct rs232net {
    int inuse;
    SOCKET fd;
    char *file;
    int rts;
    int dtr;
} rs232net_t;

static rs232net_t fds[RS232_NUM_DEVICES];

static log_t rs232net_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

void rs232net_close(int fd);

/* initializes all RS232 stuff */
void rs232net_init(void)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;
    int i;

    WSAStartup(wVersionRequested, &wsaData);

    for (i = 0; i < RS232_NUM_DEVICES; i++)
        fds[i].inuse = 0;

    rs232net_log = log_open("RS232");
}

/* reset RS232 stuff */
void rs232net_reset(void)
{
    int i;

    for (i = 0; i < RS232_NUM_DEVICES; i++) {
        if (fds[i].inuse) {
            rs232net_close(i);
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
int rs232net_open(int device)
{
    struct sockaddr_in ad;
    int i;

    // parse the address
    if(getaddr(rs232_devfile[device], &ad) == -1) {
        log_error(rs232net_log, "Bad device name.  Should be ipaddr:port.");
        return -1;
    }

    for (i = 0; i < RS232_NUM_DEVICES; i++) {
        if (!fds[i].inuse)
            break;
    }
    if (i >= RS232_NUM_DEVICES) {
        log_error(rs232net_log, "No more devices available.");
        return -1;
    }

    DEBUG_LOG_MESSAGE((rs232net_log, "rs232net_open(device=%d).", device));

    // connect socket
    fds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fds[i].fd == INVALID_SOCKET ||
       connect(fds[i].fd, (struct sockaddr*)&ad, sizeof ad) == -1) {
        log_error(rs232net_log, "Cant open connection.");
        return -1;
    }

    fds[i].inuse = 1;
    fds[i].file = rs232_devfile[device];
    return i;
}

/* closes the rs232 window again */
void rs232net_close(int fd)
{
    DEBUG_LOG_MESSAGE((rs232net_log, "close(fd=%d).", fd));

    if (fd < 0 || fd >= RS232_NUM_DEVICES) {
        log_error(rs232net_log, "Attempt to close invalid fd %d.", fd);
        return;
    }
    if (!fds[fd].inuse) {
        log_error(rs232net_log, "Attempt to close non-open fd %d.", fd);
        return;
    }

    close(fds[fd].fd);
    fds[fd].inuse = 0;
}

/* sends a byte to the RS232 line */
int rs232net_putc(int fd, BYTE b)
{
    size_t n;

    if (fd < 0 || fd >= RS232_NUM_DEVICES) {
        log_error(rs232net_log, "Attempt to write to invalid fd %d.", fd);
        return -1;
    }
    if (!fds[fd].inuse) {
        log_error(rs232net_log, "Attempt to write to non-open fd %d.", fd);
        return -1;
    }

    /* silently drop if socket is shut */
    if (fds[fd].fd < 0)
        return 0;

    /* for the beginning... */
    DEBUG_LOG_MESSAGE((rs232net_log, "Output `%c'.", b));

    n = send(fds[fd].fd, &b, 1, 0);
    if (n != 1) {
        log_error(rs232net_log, "Error writing: %s.", strerror(errno));
        close(fds[fd].fd);
        fds[fd].fd = -1;
        return -1;
    }

    return 0;
}

/* gets a byte to the RS232 line, returns !=0 if byte received, byte in *b. */
int rs232net_getc(int fd, BYTE * b)
{
    int ret;
    size_t n;
    fd_set rdset;
    struct timeval ti;

    if (fd < 0 || fd >= RS232_NUM_DEVICES) {
        log_error(rs232net_log, "Attempt to read from invalid fd %d.", fd);
        return -1;
    }
    if (!fds[fd].inuse) {
        log_error(rs232net_log, "Attempt to read from non-open fd %d.", fd);
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
                log_error(rs232net_log, "Error reading: %s.", strerror(errno));
            else
                log_error(rs232net_log, "EOF");
            close(fds[fd].fd);
            fds[fd].fd = -1;
            return -1;
        }
        return 1;
    }
    return 0;
}

/* set the status lines of the RS232 device */
int rs232net_set_status(int fd, enum rs232handshake_out status)
{
    fds[fd].rts = (status & RS232_HSO_RTS) ? 1 : 0;
    fds[fd].dtr = (status & RS232_HSO_DTR) ? 1 : 0;

    /*! \todo signal the RS232 device the current status, too */

    return 0;
}

/* get the status lines of the RS232 device */
enum rs232handshake_in rs232net_get_status(int fd)
{
    /*! \todo dummy */
    return RS232_HSI_CTS | RS232_HSI_DSR;
}
