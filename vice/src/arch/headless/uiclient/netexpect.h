/** \file   netexpect.h
 * \brief   Structured network IO helper
 *
 * \author  David Hogan <david.q.hogan@gmail.com>
 */

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

#ifndef NET_EXPECT_H
#define NET_EXPECT_H

#include <stdint.h>

/* This callback function is expected to return the number of bytes that can be read without blocking */
typedef int (*netexpect_available_cb)(void *context);

/* This callback function is expected to wrap a blocking recv */
typedef int (*netexpect_recv_cb)(void *context, void *buffer, unsigned int size);

/* IO completion handler callback type */
typedef void (*netexpect_completion_cb)(void *context);

typedef struct netexpect_s netexpect_t;

/* Create and initialise a new netexpect */
netexpect_t *netexpect_new(
    netexpect_available_cb available_callback,
    netexpect_recv_cb recv_callback,
    void *callback_context);

/* Expect a byte and optionally call on_complete(on_complete_context) */
void netexpect_u8(netexpect_t *netexpect, uint8_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect two bytes and optionally call on_complete(on_complete_context) */
void netexpect_u16(netexpect_t *netexpect, uint16_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect four bytes and optionally call on_complete(on_complete_context) */
void netexpect_u32(netexpect_t *netexpect, uint32_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect eight bytes and optionally call on_complete(on_complete_context) */
void netexpect_u64(netexpect_t *netexpect, uint64_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect buffer_size bytes, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
void netexpect_byte_array(netexpect_t *netexpect, void *buffer, uint8_t buffer_size, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect buffer_size, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
void netexpect_u8_prefixed_byte_array(netexpect_t *netexpect, void *buffer, uint8_t *buffer_size, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect buffer_size, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
void netexpect_u16_prefixed_byte_array(netexpect_t *netexpect, void *buffer, uint16_t *buffer_size, netexpect_completion_cb on_complete, void *on_complete_context);

/* Expect string_size, then expect string_size chars into string, add '\0', then optionally call on_complete(on_complete_context) */
void netexpect_u16_prefixed_string(netexpect_t *netexpect, char *string, uint16_t *string_size, netexpect_completion_cb on_complete, void *on_complete_context);

/* Call on_complete(on_complete_context) after previously queued reads have completed */
void netexpect_callback(netexpect_t *netexpect, netexpect_completion_cb on_complete, void *on_complete_context);

/* Recv from socket, filling expected data structures and calling completion callbacks. */
int netexpect_do_recv(netexpect_t *netexpect);

/* Shutdown and free netexpect */
void netexpect_destroy(netexpect_t *netexpect);

#endif /* #ifndef NET_EXPECT_H */
