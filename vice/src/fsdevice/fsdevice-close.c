/*
 * fsdevice-close.c - File system device.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
 *
 * Based on old code by
 *  Teemu Rantanen <tvr@cs.hut.fi>
 *  Jarkko Sonninen <sonninen@lut.fi>
 *  Jouko Valta <jopi@stekt.oulu.fi>
 *  Olaf Seibert <rhialto@mbfys.kun.nl>
 *  André Fachat <a.fachat@physik.tu-chemnitz.de>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#include "fsdevice-close.h"
#include "fsdevicetypes.h"
#include "ioutil.h"
#include "vdrive-command.h"
#include "vdrive.h"


int fsdevice_close(vdrive_t *vdrive, unsigned int secondary)
{
#ifdef __riscos
    archdep_set_drive_leds(vdrive->unit - 8, 0);
#endif

    if (secondary == 15) {
        fsdevice_error(vdrive, IPE_OK);
        return FLOPPY_COMMAND_OK;
    }

    switch (fs_info[secondary].mode) {
      case Write:
      case Read:
      case Append:
        if (fs_info[secondary].tape.name) {
            tape_image_close(&(fs_info[secondary].tape));
        } else if (fs_info[secondary].fd) {
            fclose(fs_info[secondary].fd);
            fs_info[secondary].fd = NULL;
        } else
            return FLOPPY_ERROR;
        break;
      case Directory:
        if (fs_info[secondary].ioutil_dir == NULL)
            return FLOPPY_ERROR;

        ioutil_closedir(fs_info[secondary].ioutil_dir);
        fs_info[secondary].ioutil_dir = NULL;
        break;
    }

    return FLOPPY_COMMAND_OK;
}

