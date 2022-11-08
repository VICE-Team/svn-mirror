/** \file   netexpect.c
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

#include <stdlib.h>
#include <string.h>

#include "netexpect.h"

#define QUEUE_INITIAL_SIZE 3

typedef struct expected_data_s {
    void *destination;
    unsigned int bytes_expected;
    netexpect_completion_cb on_complete;
    void *on_complete_context;
} expected_data_t;

typedef struct netexpect_s {
    netexpect_available_cb available_callback;
    netexpect_recv_cb recv_callback;
    void *callback_context;
    
    expected_data_t *queue_base;
    unsigned int queue_alloc_size;
    unsigned int queue_free;
    unsigned int queue_head_offset;
    unsigned int queue_length;
} netexpect_t;

netexpect_t *netexpect_new(
    netexpect_available_cb available_callback,
    netexpect_recv_cb recv_callback,
    void *callback_context)
{
    netexpect_t *netexpect;
    
    netexpect                       = malloc(sizeof(netexpect_t));
    netexpect->available_callback   = available_callback;
    netexpect->recv_callback        = recv_callback;
    netexpect->callback_context     = callback_context;
    netexpect->queue_base           = malloc(QUEUE_INITIAL_SIZE * sizeof(expected_data_t));
    netexpect->queue_alloc_size     = QUEUE_INITIAL_SIZE;
    netexpect->queue_free           = QUEUE_INITIAL_SIZE;
    netexpect->queue_head_offset    = 0;
    netexpect->queue_length         = 0;
    
    return netexpect;
}

static expected_data_t *next_unused(netexpect_t *netexpect, unsigned int how_many)
{
    expected_data_t *tail;
    unsigned int grow_by;
    
    /*
     * Returns a pointer to how_many expected_data_t at the tail of the queue.
     */
    
    if (netexpect->queue_free < how_many) {
        /* We need to grow the queue. TODO: If offset is 'big', memmove instead. */
        grow_by = how_many - netexpect->queue_free;
        netexpect->queue_alloc_size += grow_by;
        netexpect->queue_base =
            realloc(netexpect->queue_base,
                    netexpect->queue_alloc_size * sizeof(expected_data_t));
        netexpect->queue_free += grow_by;
    }
    
    tail = netexpect->queue_base + netexpect->queue_head_offset + netexpect->queue_length;
    
    netexpect->queue_length += how_many;
    netexpect->queue_free   -= how_many;
    
    return tail;
}

void netexpect_u8(netexpect_t *netexpect, uint8_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect a byte and optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    tail                        = next_unused(netexpect, 1);
    tail->destination           = buffer;
    tail->bytes_expected        = sizeof(uint8_t);
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

void netexpect_u16(netexpect_t *netexpect, uint16_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect two bytes and optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    tail                        = next_unused(netexpect, 1);
    tail->destination           = buffer;
    tail->bytes_expected        = sizeof(uint16_t);
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

void netexpect_u32(netexpect_t *netexpect, uint32_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect four bytes and optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    tail                        = next_unused(netexpect, 1);
    tail->destination           = buffer;
    tail->bytes_expected        = sizeof(uint32_t);
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

void netexpect_u64(netexpect_t *netexpect, uint64_t *buffer, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect eight bytes and optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    tail                        = next_unused(netexpect, 1);
    tail->destination           = buffer;
    tail->bytes_expected        = sizeof(uint64_t);
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

void netexpect_byte_array(netexpect_t *netexpect, void *buffer, uint8_t buffer_size, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect buffer_size bytes, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    tail                        = next_unused(netexpect, 1);
    tail->destination           = buffer;
    tail->bytes_expected        = buffer_size;
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

static void update_u8_prefixed_array_size(void *context)
{
    /*
     * Called when the size of the incoming array is known.
     * The size is copied to the expected_data_t that actually
     * reads the bytes.
     */
    
    expected_data_t *prefixed_array_expected_data = context;
    
    prefixed_array_expected_data[1].bytes_expected = *(uint8_t*)prefixed_array_expected_data[0].destination;
}

void netexpect_u8_prefixed_byte_array(netexpect_t *netexpect, void *buffer, uint8_t *buffer_size, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect buffer_size, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    /* We'll need two entries for this */
    tail                        = next_unused(netexpect, 2);
    tail[0].destination         = buffer_size;
    tail[0].bytes_expected      = sizeof(uint8_t);
    tail[0].on_complete         = update_u8_prefixed_array_size;
    tail[0].on_complete_context = tail;
    tail[1].destination         = buffer;
    tail[1].bytes_expected      = 0; /* this gets overwritten by update_u8_prefixed_array_size */
    tail[1].on_complete         = on_complete;
    tail[1].on_complete_context = on_complete_context;
}

static void update_u16_prefixed_array_size(void *context)
{
    /*
     * Called when the size of the incoming array is known.
     * The size is copied to the expected_data_t that actually
     * reads the bytes.
     */
    
    expected_data_t *prefixed_array_expected_data = context;
    
    prefixed_array_expected_data[1].bytes_expected = *(uint16_t*)prefixed_array_expected_data[0].destination;
}

void netexpect_u16_prefixed_byte_array(netexpect_t *netexpect, void *buffer, uint16_t *buffer_size, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect buffer_size, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    /* We'll need two entries for this */
    tail                        = next_unused(netexpect, 2);
    tail[0].destination         = buffer_size;
    tail[0].bytes_expected      = sizeof(uint16_t);
    tail[0].on_complete         = update_u16_prefixed_array_size;
    tail[0].on_complete_context = tail;
    tail[1].destination         = buffer;
    tail[1].bytes_expected      = 0; /* this gets overwritten by update_u16_prefixed_array_size */
    tail[1].on_complete         = on_complete;
    tail[1].on_complete_context = on_complete_context;
}

static void set_string_termination_byte(void *context)
{
    expected_data_t *string_expected_data = context;
    
    ((char*)string_expected_data->destination)[string_expected_data->bytes_expected] = '\0';
}

void netexpect_u16_prefixed_string(netexpect_t *netexpect, char *string, uint16_t *string_size, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Expect buffer_size, then expect buffer_size bytes into buffer, then optionally call on_complete(on_complete_context) */
    expected_data_t *tail;
    
    /* We'll need three entries for this */
    tail                        = next_unused(netexpect, 3);
    tail[0].destination         = string_size;
    tail[0].bytes_expected      = sizeof(uint16_t);
    tail[0].on_complete         = update_u16_prefixed_array_size;
    tail[0].on_complete_context = tail;
    
    tail[1].destination         = string;
    tail[1].bytes_expected      = 0; /* this gets overwritten by update_u16_prefixed_array_size */
    tail[1].on_complete         = set_string_termination_byte;
    tail[1].on_complete_context = &tail[1];

    tail[2].destination         = NULL;
    tail[2].bytes_expected      = 0;
    tail[2].on_complete         = on_complete;
    tail[2].on_complete_context = on_complete_context;
}

void netexpect_callback(netexpect_t *netexpect, netexpect_completion_cb on_complete, void *on_complete_context)
{
    /* Call on_complete(on_complete_context) after previously queued reads have completed */
    expected_data_t *tail;

    tail                        = next_unused(netexpect, 1);
    tail->destination           = NULL;
    tail->bytes_expected        = 0;
    tail->on_complete           = on_complete;
    tail->on_complete_context   = on_complete_context;
}

int netexpect_do_recv(netexpect_t *netexpect)
{
    /* Recv from socket, filling expected data structures and calling completion callbacks. */
    int bytes_available;
    int bytes_read;
    expected_data_t *head;
    
    bytes_available = netexpect->available_callback(netexpect->callback_context);
    if (bytes_available <= 0) {
        /* Socket closed, or error */
        return -1;
    }
    
    if (netexpect->queue_length == 0) {
        /* State machine error */
        return -1;
    }
    
    do {
        head = netexpect->queue_base + netexpect->queue_head_offset;
        
        /* Only read from the socket if the next job requires it */
        if (head->bytes_expected) {
            
            if (bytes_available == 0) {
                return 0;
            }
            
            bytes_read = netexpect->recv_callback(netexpect->callback_context,
                                                  head->destination,
                                                  head->bytes_expected);

            if (bytes_read <= 0) {
                /* Socket closed, or error */
                return -1;
            }
        
            if (bytes_read < head->bytes_expected) {
                /* Partial read */
                head->destination = ((uint8_t*)head->destination) + bytes_read;
                head->bytes_expected -= bytes_read;
                
                return 0;
            }

            bytes_available -= bytes_read;
            if (bytes_available < 0) {
                /* This can happen if more data becomes available after we check how much is there */
                bytes_available = 0;
            }
        }
            
        /* Got everything we needed for the queue head */
        netexpect->queue_length--;
            
        if (netexpect->queue_length == 0) {
            /* We can reset the queue to start at the base now */
            netexpect->queue_head_offset = 0;
            netexpect->queue_free = netexpect->queue_alloc_size;
        } else {
            netexpect->queue_head_offset++;
        }
            
        if (head->on_complete) {
            head->on_complete(head->on_complete_context);
        }
    } while (netexpect->queue_length);
    
    return 0;
}

void netexpect_destroy(netexpect_t *netexpect)
{
    free(netexpect->queue_base);
    free(netexpect);
}
