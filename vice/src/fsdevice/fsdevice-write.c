/*
 * fsdevice-write.c - File system device.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  Andre Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  pottendo <pottendo@gmx.net>
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

#include "fileio.h"
#include "fsdevice-flush.h"
#include "fsdevice-read.h"
#include "fsdevice-write.h"
#include "fsdevicetypes.h"
#include "types.h"
#include "vdrive.h"
#include "vdrive/vdrive-command.h"

/* #define REL_DEBUG */

#ifdef REL_DEBUG
# define DBG(x) log_debug x
# include "log.h"
#else
# define DBG(x)
#endif

static void fsdevice_rel_listen(vdrive_t *vdrive, bufinfo_t *bufinfo, unsigned int secondary)
{
    DBG(("fsdevice_rel_listen: dirty record: %d", bufinfo->record_is_dirty));
    /*
     * Only move to next record if the sector is dirty (indicates
     * we just wrote something) and if this is a REL file.
     * All "overflows" are handled in the write routine.
     * This should probably be better located in an "unlisten"
     * callback, but there is none.
     */
    if (bufinfo->record_is_dirty) {
        fsdevice_relative_switch_record(vdrive, bufinfo, bufinfo->current_record + 1, 0);
    }
}

void fsdevice_listen(vdrive_t *vdrive, unsigned int secondary)
{
    bufinfo_t *bufinfo = &fsdevice_dev[vdrive->unit - 8].bufinfo[secondary];

    if (bufinfo->mode == Relative) {
        fsdevice_rel_listen(vdrive, bufinfo, secondary);
    }
}

int fsdevice_write(struct vdrive_s *vdrive, uint8_t data, unsigned int secondary)
{
    bufinfo_t *bufinfo;

    bufinfo = &fsdevice_dev[vdrive->unit - 8].bufinfo[secondary];

    if (secondary == 15) {
        return fsdevice_flush_write_byte(vdrive, data);
    }

    switch (bufinfo->mode) {
        case Write:
        case Append:
        case Relative:
            break;
        default:
            return SERIAL_ERROR;
    }

    if (bufinfo->fileio_info != NULL) {
        unsigned int len;

        if (bufinfo->mode == Relative) {
            if (bufinfo->position_in_record >= bufinfo->reclen) {
                DBG(("fsdevice_write: OVERFLOW in recno:%d position_in_record:%d reclen:%d %02x '%c'",
                    bufinfo->current_record, bufinfo->position_in_record,
                    bufinfo->reclen, data, data));
                fsdevice_error(vdrive, CBMDOS_IPE_OVERFLOW);
                return SERIAL_ERROR;
            }

            /*
             * Mixing reading and writing of a single record isn't
             * allowed on a real drive, but try to support it anyway.
             */
            if (bufinfo->isbuffered) {
                fileio_seek(bufinfo->fileio_info, -1, SEEK_CUR);
                bufinfo->position_in_record--;
                bufinfo->isbuffered = 0;
            }

            bufinfo->record_is_dirty++;
            bufinfo->position_in_record++;

            if (bufinfo->position_in_record > bufinfo->current_record_length) {
                bufinfo->current_record_length = bufinfo->position_in_record;
            }
        }

        DBG(("fsdevice_write: recno:%d position_in_record:%d %02x '%c'",
                bufinfo->current_record, bufinfo->position_in_record-1,
                data, data));
        len = fileio_write(bufinfo->fileio_info, &data, 1);

        if (len == 0) {
            return SERIAL_ERROR;
        }

        return SERIAL_OK;
    }

    return SERIAL_ERROR;
}
