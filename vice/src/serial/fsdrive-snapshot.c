/*
 * fsdrive-snapshot.c - Snapshot for filesystem based serial emulation.
 *
 * Written by
 *  EmpathicQubit <empathicqubit@entan.gl>
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

#include <string.h>

#include "log.h"
#include "fsdrive.h"
#include "snapshot.h"

static log_t fsdrive_snapshot_log = LOG_ERR;

void fsdrive_snapshot_prepare(void)
{
}

/*
   This is the format of the FSDrive snapshot module.

   Name               Type   Size   Description

   SerialBuffer       BYTE   256    The serial buffer as it was when the machine saved
   SerialPtr          WORD   1      The current position in the serial buffer
 */

static char snap_module_name[] = "FSDRIVE";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0

int fsdrive_snapshot_write_module(struct snapshot_s *s)
{
    uint8_t snapshot_serial_buffer[SERIAL_NAMELENGTH + 1];
    snapshot_module_t *m;

    m = snapshot_module_create(s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL) {
        return -1;
    }

    memcpy(snapshot_serial_buffer, SerialBuffer, sizeof(SerialBuffer));

    if(0
        || SMW_BA(m, snapshot_serial_buffer, sizeof(snapshot_serial_buffer)) < 0
        || SMW_W(m, (uint16_t)SerialPtr) < 0) {
        goto fail;
    }

    return snapshot_module_close(m);

fail:
    if (m != NULL) {
        snapshot_module_close(m);
    }
    return -1;
}

int fsdrive_snapshot_read_module(struct snapshot_s *s)
{
    uint8_t snapshot_serial_buffer[SERIAL_NAMELENGTH + 1];
    uint8_t major_version, minor_version;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);

    if (m == NULL) {
        return -1;
    }

    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        log_error(fsdrive_snapshot_log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        snapshot_set_error(SNAPSHOT_MODULE_HIGHER_VERSION);
        goto fail;
    }

    if (0
        || SMR_BA(m, snapshot_serial_buffer, sizeof(snapshot_serial_buffer)) < 0
        || SMR_W_INT(m, &SerialPtr) < 0) {
        goto fail;
    }

    memcpy(SerialBuffer, snapshot_serial_buffer, sizeof(snapshot_serial_buffer));

    snapshot_module_close(m);
    return 0;

fail:
    if (m != NULL) {
        snapshot_module_close(m);
    }

    return -1;
}