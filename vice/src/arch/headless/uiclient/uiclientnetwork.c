/*
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

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "uiclientnetwork.h"
#include "uiclientutil.h"

typedef struct socket_s {
    struct pollfd poll_fd;
} socket_t;

int uiclient_network_poll(socket_t *sock)
{
    int poll_result;
    
    /* Any data or errors availale? */
    poll_result = poll(&sock->poll_fd, 1, 0);

    if (poll_result == 0) {
        /* No. */
        return 0;
    }

    if (poll_result == -1) {
        ERR("UI Client: general poll error: %s", strerror(errno));
        return poll_result;
    }

    /* Something is waiting for us. */
    return 1;
}

ssize_t uiclient_network_available_bytes(socket_t *sock)
{
    int bytes_available;

    if (ioctl(sock->poll_fd.fd, FIONREAD, &bytes_available) >= 0) {
        return bytes_available;
    }

    return -1;
}

ssize_t uiclient_network_recv(socket_t *sock, void *buffer, uint32_t buffer_length)
{
    sock->poll_fd.revents = 0;
    
    return recv(sock->poll_fd.fd, buffer, buffer_length, 0);
}

ssize_t uiclient_network_send(socket_t *sock, void *buffer, uint32_t buffer_length)
{
    return send(sock->poll_fd.fd, buffer, buffer_length, 0);
}

socket_t *uiclient_network_connect(uint16_t server_port)
{
    int fd;
    struct sockaddr_in server_address = { 0 };
    socket_t *sock;

    fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd == -1) {
        ERR("Failed to create socket");
        return NULL;
    }

    /*
     * Initiate a connection to server_port on localhost.
     */

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(server_port);

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		ERR("Failed to initiate socket connection");
        close(fd);
		return NULL;
	}

    sock = calloc(1, sizeof(socket_t));
    sock->poll_fd.fd = fd;
    sock->poll_fd.events = POLLIN;

    return sock;
}

void uiclient_network_close(socket_t *sock)
{
    shutdown(sock->poll_fd.fd, SHUT_RDWR);
    close(sock->poll_fd.fd);

    free(sock);
}

void uiclient_network_init(void)
{
#ifdef _WIN32
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;

    WSAStartup(wVersionRequested, &wsaData);
#endif
}

void uiclient_network_shutdown(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}
