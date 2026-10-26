/*
 * cascom.h - BayCom-compatible TCM3105 cassette-port packet-radio modem.
 *
 * Written by
 *  Felix Erckenbrecht <code@eligs.de>
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

#ifndef VICE_CASCOM_H
#define VICE_CASCOM_H

#include "types.h"

/* operating modes (CascomMode resource); a mode whose backend is not compiled
   into the build (no PortAudio / no network) is rejected by the resource
   setter and hidden from the command line and the UIs */
#define CASCOM_MODE_LOOPBACK 0   /* reflect TX bits back onto the RX lines (self-test) */
#define CASCOM_MODE_AUDIO    1   /* AFSK modem to/from host soundcard (M2)             */
#define CASCOM_MODE_NET      2   /* AXUDP frame gateway (M4)                           */

#define CASCOM_MODE_MIN      CASCOM_MODE_LOOPBACK
#define CASCOM_MODE_MAX      CASCOM_MODE_NET

int cascom_resources_init(int amount);
int cascom_cmdline_options_init(void);
void cascom_resources_shutdown(void);

/* Device enumeration for the settings UI. Each returns a NULL-terminated array
   of newly-allocated strings; entry [0] is always "" (system default / none).
   Free the whole thing with cascom_device_list_free(). */
char **cascom_audio_device_list(int want_input);
char **cascom_serial_device_list(void);
void   cascom_device_list_free(char **list);

#endif
