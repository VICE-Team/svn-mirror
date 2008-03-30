/*
 * kbdbuf.c - Kernal keyboard buffer handling for VICE.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#include "maincpu.h"
#include "kbdbuf.h"
#include "types.h"
#include "vmachine.h"
#include "charsets.h"

#define QUEUE_SIZE	1024

static char queue[QUEUE_SIZE];
static int head_idx, num_pending;

/* FIXME: This should go into machine-specific description.  */
struct {
    int location;
    int num_pending_location;
    int size;
} kernal_kbd_buf = {
    631,
    198,
    10
};

int kbd_buf_feed(const char *s)
{
    int num = strlen(s);
    int i, p;

    if (num_pending + num > QUEUE_SIZE)
	return -1;

    num_pending += num;
    for (p = head_idx, i = 0; i < num; p = (p + 1) % QUEUE_SIZE, i++)
	queue[p] = s[i];

    return 0;
}

void kbd_buf_flush(void)
{
    BYTE *p;
    int i, q, n;
    
    if (num_pending == 0 || clk < CYCLES_PER_RFSH * RFSH_PER_SEC
	|| ram[kernal_kbd_buf.num_pending_location] != 0)
	return;

    n = num_pending > kernal_kbd_buf.size ? kernal_kbd_buf.size : num_pending;
    p = ram + kernal_kbd_buf.location;
    for (i = 0; i < n; head_idx = (head_idx + 1) % QUEUE_SIZE, i++)
	p[i] = p_topetcii(queue[head_idx]);

    ram[kernal_kbd_buf.num_pending_location] = n;
    num_pending -= n;
}
