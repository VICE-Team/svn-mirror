/*
 * tpicore.h - TPI 6525 template
 *
 * Written by
 *   André Fachat (a.fachat@physik.tu-chemnitz.de)
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

#ifdef STDC_HEADERS
#include <stdio.h>
#endif

#include "types.h"
#include "log.h"
#include "snapshot.h"
#include "tpi.h"

#if defined(NO_INLINE)
#define _TPI_FUNC       static
#else
#define _TPI_FUNC       static inline
#endif

static BYTE pow2[] = { 1, 2, 4, 8, 16 };

static BYTE irq_previous;
static BYTE irq_stack;

#define	irq_active	tpi[TPI_AIR]
#define	irq_latches	tpi[TPI_PC]
#define	irq_mask	tpi[TPI_DDPC]
#define	irq_mode	(tpi[TPI_CREG] & 1)
#define	irq_priority	(tpi[TPI_CREG] & 2)

#define	IS_CA_MODE()		((tpi[TPI_CREG] & 0x20) == 0x00)
#define	IS_CA_PULSE_MODE()	((tpi[TPI_CREG] & 0x30) == 0x10)
#define	IS_CA_TOGGLE_MODE()	((tpi[TPI_CREG] & 0x30) == 0x00)
#define	IS_CB_MODE()		((tpi[TPI_CREG] & 0x80) == 0x00)
#define	IS_CB_PULSE_MODE()	((tpi[TPI_CREG] & 0xc0) == 0x40)
#define	IS_CB_TOGGLE_MODE()	((tpi[TPI_CREG] & 0xc0) == 0x00)

static BYTE tpi[8];

static BYTE oldpa;
static BYTE oldpb;
static BYTE oldpc;

static BYTE ca_state;
static BYTE cb_state;

static log_t mytpi_log = LOG_ERR;


