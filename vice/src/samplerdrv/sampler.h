/*
 * sampler.h
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#ifndef VICE_SAMPLER_H
#define VICE_SAMPLER_H

#include "types.h"

typedef struct sampler_device_s {
    const char *name;
    void (*open)(void);
    void (*close)(void);
    BYTE (*get_sample)(void);
} sampler_device_t;

extern void sampler_init(void);
extern void sampler_start(void);
extern void sampler_stop(void);
extern BYTE sampler_get_sample(void);

extern void sampler_device_register(sampler_device_t *device);

extern int sampler_resources_init(void);
extern void sampler_resources_shutdown(void);
extern int sampler_cmdline_options_init(void);

#endif
