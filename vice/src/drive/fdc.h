/*
 * fdc.h - 1001/8x50 FDC definitions
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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

#ifndef _FDC_H
#define _FDC_H

/* FDC states */
#define	FDC_UNUSED	0
#define	FDC_RESET0	1
#define	FDC_RESET1	2
#define	FDC_RESET2	3
#define	FDC_RUN		4

#define	FDC_LAST_STATE	4

/* fdc error codes to return to drive CPU */
#define	FDC_ERR_OK	1
#define	FDC_ERR_HEADER	2
#define	FDC_ERR_SYNC	3
#define	FDC_ERR_NOBLOCK	4
#define	FDC_ERR_DCHECK	5
#define	FDC_ERR_VERIFY	7
#define	FDC_ERR_WPROT	8
#define	FDC_ERR_HCHECK	9
#define	FDC_ERR_BLENGTH	10
#define	FDC_ERR_ID	11
#define	FDC_ERR_FSPEED	12
#define	FDC_ERR_DRIVE	15
#define	FDC_ERR_DECODE	16

#include "vice.h"
#include "log.h"
#include "alarm.h"
#include "snapshot.h"

void fdc_init(int fnum, BYTE *buffer_memory, BYTE *ipromp);
void fdc_reset(int fnum, int enabled);

int fdc_read_snapshot_module(snapshot_t *s, int drv);
int fdc_write_snapshot_module(snapshot_t *s, int drv);

#endif

