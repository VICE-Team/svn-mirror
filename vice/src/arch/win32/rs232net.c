/*
 * rs232net.c - RS232 emulation.
 *
 * Written by
 *  Tim Newsham
 *  Spiro Trikaliotis <spiro.trikaliotis@gmx.de>
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

#include "lib.h"
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
    int inuse; /*!< 0 if the connection has not been opened, 1 otherwise. */
    SOCKET fd; /*!< the SOCKET for the connection. If fd is INVALID_SOCKET
                    although inuse == 1, then the socket has been closed
                    because of a previous error. This prevents the error
                    log from being flooded with error messages. */
} rs232net_t;

static rs232net_t fds[RS232_NUM_DEVICES] = { 0 };

static log_t rs232net_log = LOG_ERR;

/* ------------------------------------------------------------------------- */

void rs232net_close(int fd);

/* initializes all RS232 stuff */
void rs232net_init(void)
{
    WORD wVersionRequested = MAKEWORD(1, 1);
    WSADATA wsaData;

    rs232net_log = log_open("RS232NET");

    if ( WSAStartup(wVersionRequested, &wsaData) != 0 ) {
        log_message(rs232net_log, "WSAStartup() failed!");
    }
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
getaddr(const char * const dev_in, struct sockaddr_in *ad)
{
    char *port;
    char *dev;
    int ret = -1;

    /*! \todo: Also accept HOSTNAME:port */

    do {
        struct hostent * hostentry;

        ad->sin_family = AF_INET;

        dev = lib_stralloc(dev_in);
        port = strchr(dev, ':');

        if ( ! port ) {
            break;
        }

        *port++ = 0;

        /* first, check if we have a host name */

        hostentry = gethostbyname(dev);

        if (hostentry != NULL && hostentry->h_addrtype == AF_INET) {
            /* yes, we have a host name: process it */
            if ( hostentry->h_length != sizeof ad->sin_addr.s_addr ) {
                /* something weird happened... SHOULD NOT HAPPEN! */
                log_error(rs232net_log,
                          "gethostbyname() returned an IPv4 address, "
                          "but the length is wrong: %u", hostentry->h_length );
                break;
            }

            memcpy(&ad->sin_addr.s_addr,
                   hostentry->h_addr_list[0],
                   sizeof ad->sin_addr.s_addr);
        }
        else {
            /* no host name: Assume it is an IP address */
            ad->sin_addr.s_addr = inet_addr(dev);
        }

        ad->sin_port = htons( (unsigned short) atoi(port) );

        if ( ntohl(ad->sin_addr.s_addr) != -1 && ntohs(ad->sin_port) != 0 ) {
            ret = 0;
        }

        log_message(rs232net_log, "Connecting to %s:%u", 
                                  inet_ntoa(ad->sin_addr), ntohs(ad->sin_port));

    } while (0);

    lib_free(dev);

    return ret;
}

/* opens a rs232 window, returns handle to give to functions below. */
int rs232net_open(int device)
{
    struct sockaddr_in ad = { 0 };
    int index = -1;

    do {
        int i;

        /* parse the address */
        if ( getaddr(rs232_devfile[device], &ad) == -1 ) {
            log_error(rs232net_log, "Bad device name.  Should be ipaddr:port, but is '%s'.",
                                    rs232_devfile[device]);
            break;
        }

        for (i = 0; i < RS232_NUM_DEVICES; i++) {
            if (!fds[i].inuse) {
                break;
            }
        }

        if (i >= RS232_NUM_DEVICES) {
            log_error(rs232net_log, "No more devices available.");
            break;
        }

        DEBUG_LOG_MESSAGE((rs232net_log, "rs232net_open(device=%d).", device));

        /* connect socket */
        fds[i].fd = socket(AF_INET, SOCK_STREAM, 0);
        if ( fds[i].fd == INVALID_SOCKET ) {
            log_error(rs232net_log, "Can't open socket.");
            break;
        }

        if ( connect(fds[i].fd, (struct sockaddr*) &ad, sizeof ad) == SOCKET_ERROR ) {
            log_error(rs232net_log, "Cant open connection.");
            break;
        }

        fds[i].inuse = 1;

        index = i;

    } while (0);

    return index;
}

static void rs232net_closesocket(int index)
{
    closesocket(fds[index].fd);
    fds[index].fd = INVALID_SOCKET;
}

/* closes the rs232 window again */
void rs232net_close(int fd)
{
    do {

        DEBUG_LOG_MESSAGE((rs232net_log, "close(fd=%d).", fd));

        if (fd < 0 || fd >= RS232_NUM_DEVICES) {
            log_error(rs232net_log, "Attempt to close invalid fd %d.", fd);
            break;
        }
        if (!fds[fd].inuse) {
            log_error(rs232net_log, "Attempt to close non-open fd %d.", fd);
            break;
        }

        rs232net_closesocket(fd);
        fds[fd].inuse = 0;

    } while (0);
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

    /* silently drop if socket is shut because of a previous error */
    if (fds[fd].fd == INVALID_SOCKET)
        return 0;

    /* for the beginning... */
    DEBUG_LOG_MESSAGE((rs232net_log, "Output `%c'.", b));

    n = send(fds[fd].fd, &b, 1, 0);
    if (n == SOCKET_ERROR) {
        log_error(rs232net_log, "Error writing: %u.", WSAGetLastError());
        rs232net_closesocket(fd);
        return -1;
    }

    return 0;
}

/* gets a byte to the RS232 line, returns !=0 if byte received, byte in *b. */
int rs232net_getc(int fd, BYTE * b)
{
    int ret;
    size_t no_of_read_byte = -1;

    do {
        fd_set rdset;
        struct timeval ti;

        if (fd < 0 || fd >= RS232_NUM_DEVICES) {
            log_error(rs232net_log, "Attempt to read from invalid fd %d.", fd);
            break;
        }

        if (!fds[fd].inuse) {
            log_error(rs232net_log, "Attempt to read from non-open fd %d.", fd);
            break;
        }

        /* from now on, assume everything is ok, 
           but we have not received any bytes */
        no_of_read_byte = 0;

        /* silently drop if socket is shut because of a previous error  */
        if (fds[fd].fd == INVALID_SOCKET) {
            break;
        }

        FD_ZERO(&rdset);
        FD_SET(fds[fd].fd, &rdset);
        ti.tv_sec = ti.tv_usec = 0;
        ret = select(fds[fd].fd + 1, &rdset, NULL, NULL, &ti);

        if (ret > 0 && (FD_ISSET(fds[fd].fd, &rdset))) {

            no_of_read_byte = recv(fds[fd].fd, b, 1, 0);

            if ( no_of_read_byte != 1 ) {
                if ( no_of_read_byte == SOCKET_ERROR ) {
                    log_error(rs232net_log, "Error reading: %u.", WSAGetLastError());
                }
                else {
                    log_error(rs232net_log, "EOF");
                }
                rs232net_closesocket(fd);
                no_of_read_byte = -1;
            }
        }
    } while (0);

    return no_of_read_byte;
}

/* set the status lines of the RS232 device */
int rs232net_set_status(int fd, enum rs232handshake_out status)
{
    /* unused */

    return 0;
}

/* get the status lines of the RS232 device */
enum rs232handshake_in rs232net_get_status(int fd)
{
    /*! \todo dummy */
    return RS232_HSI_CTS | RS232_HSI_DSR;
}
