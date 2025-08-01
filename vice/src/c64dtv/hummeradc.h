/*
 * hummeradc.h - Hummer ADC emulation
 *
 * Written by
 *  Hannu Nuotio <hannu.nuotio@tut.fi>
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

#ifndef VICE_HUMMERADC_H
#define VICE_HUMMERADC_H

#include "types.h"
#include "snapshot.h"

void hummeradc_init(void);
void hummeradc_reset(void);
void hummeradc_shutdown(void);

uint8_t hummeradc_read(void);
void hummeradc_store(uint8_t value);

int hummeradc_enable(int value);

extern int c64dtv_hummer_adc_enabled;

int hummeradc_cmdline_options_init(void);
int hummeradc_resources_init(void);

int hummeradc_snapshot_write_module(snapshot_t *s);
int hummeradc_snapshot_read_module(snapshot_t *s);

#endif
