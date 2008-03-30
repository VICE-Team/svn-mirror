/*
 * vdrive-iec.c - Virtual disk-drive IEC implementation.
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
 * Patches by
 *  Dan Miner <dminer@nyx10.cs.du.edu>
 *  Germano Caronni <caronni@tik.ethz.ch>
 *  Daniel Fandrich <dan@fch.wimsey.bc.ca>
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

/* #define DEBUG_DRIVE */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __riscos
#include "ui.h"
#endif

#include "diskimage.h"
#include "log.h"
#include "serial.h"
#include "types.h"
#include "utils.h"
#include "vdrive-bam.h"
#include "vdrive-command.h"
#include "vdrive-dir.h"
#include "vdrive-iec.h"
#include "vdrive-rel.h"
#include "vdrive.h"

static log_t vdrive_iec_log = LOG_ERR;

void vdrive_iec_init(void)
{
    vdrive_iec_log = log_open("VDriveIEC");
}

static int write_sequential_buffer(vdrive_t *vdrive, bufferinfo_t *bi,
                                   int length )
{
    unsigned int t_new, s_new;
    int retval;
    BYTE *buf = bi->buffer;
    BYTE *slot = bi->slot;

    /*
     * First block of a file ?
     */
    if (slot[SLOT_FIRST_TRACK] == 0) {
        retval = vdrive_bam_alloc_first_free_sector(vdrive, vdrive->bam, &t_new,
                                                    &s_new);
        if (retval < 0) {
            vdrive_command_set_error(vdrive, IPE_DISK_FULL, 0, 0);
            return -1;
        }
        slot[SLOT_FIRST_TRACK] = bi->track = t_new;
        slot[SLOT_FIRST_SECTOR] = bi->sector = s_new;
    }

    if (length == WRITE_BLOCK) {
        /*
         * Write current sector and allocate next
         */
        t_new = bi->track;
        s_new = bi->sector;
        retval = vdrive_bam_alloc_next_free_sector(vdrive, vdrive->bam, &t_new,
                                                   &s_new);
        if (retval < 0) {
            vdrive_command_set_error(vdrive, IPE_DISK_FULL, 0, 0);
            return -1;
        }
        buf[0] = t_new;
        buf[1] = s_new;

        disk_image_write_sector(vdrive->image, buf, bi->track, bi->sector);

        bi->track = t_new;
        bi->sector = s_new;
    } else {
        /*
         * Write last block
         */
        buf[0] = 0;
        buf[1] = length - 1;

        disk_image_write_sector(vdrive->image, buf, bi->track, bi->sector);
    }

    if (!(++slot[SLOT_NR_BLOCKS]))
        ++slot[SLOT_NR_BLOCKS + 1];

    return 0;
}

/* ------------------------------------------------------------------------- */

/*
 * Serial Bus Interface
 */

/*
 * Open a file on the disk image, and store the information on the
 * directory slot.
 */

int vdrive_iec_open(vdrive_t *vdrive, const char *name, int length,
                    unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    char realname[256];
    unsigned int reallength, readmode, filetype, record_length;
    int track, sector;
    BYTE *slot; /* Current directory entry */

    if ((!name || !*name) && p->mode != BUFFER_COMMAND_CHANNEL)  /* EP */
        return SERIAL_NO_DEVICE;        /* Routine was called incorrectly. */

   /* No floppy in drive?   */
   if (vdrive->image == NULL
       && p->mode != BUFFER_COMMAND_CHANNEL
       && secondary != 15
       && *name != '#') {
       vdrive_command_set_error(vdrive, IPE_NOT_READY, 18, 0);
       log_message(vdrive_iec_log, "Drive not ready.");
       return SERIAL_ERROR;
   }

#ifdef DEBUG_DRIVE
    log_debug("VDRIVE#%i: OPEN: FD = %p - Name '%s' (%d) on ch %d.",
                  vdrive->unit, vdrive->image->fd, name, length, secondary);
#endif
#ifdef __riscos
    ui_set_drive_leds(vdrive->unit - 8, 1);
#endif

    /*
     * If channel is command channel, name will be used as write. Return only
     * status of last write ...
     */
    if (p->mode == BUFFER_COMMAND_CHANNEL) {
        int n;
        int status = SERIAL_OK;

        for (n = 0; n < length; n++)
            status = vdrive_iec_write(vdrive, name[n], secondary);
        if (length)
            p->readmode = FAM_WRITE;
        else
            p->readmode = FAM_READ;
        return status;
    }

    /*
     * Clear error flag
     */
    vdrive_command_set_error(vdrive, IPE_OK, 0, 0);

    /*
     * In use ?
     */
    if (p->mode != BUFFER_NOT_IN_USE) {
#ifdef DEBUG_DRIVE
        log_debug("Cannot open channel %d. Mode is %d.", secondary, p->mode);
#endif
        vdrive_command_set_error(vdrive, IPE_NO_CHANNEL, 0, 0);
        return SERIAL_ERROR;
    }

    /* Default filemodes.  */
    readmode = (secondary == 1) ? FAM_WRITE : FAM_READ;

    filetype = 0;
    record_length = 0;

    if (vdrive_parse_name(name, length, realname, &reallength,
        &readmode, &filetype, &record_length) != SERIAL_OK)
        return SERIAL_ERROR;
#ifdef DEBUG_DRIVE
        log_debug("Raw file name: `%s', length: %i.", name, length);
        log_debug("Parsed file name: `%s', reallength: %i.", name, reallength);
#endif

    /* Override read mode if secondary is 0 or 1.  */
    if (secondary == 0)
        readmode = FAM_READ;
    if (secondary == 1)
        readmode = FAM_WRITE;

    /* Limit file name to 16 chars.  */
    reallength = (reallength > 16) ? 16 : reallength;

    /*
     * Internal buffer ?
     */
    if (*name == '#') {
        p->mode = BUFFER_MEMORY_BUFFER;
        p->buffer = (BYTE *)xmalloc(256);
        p->bufptr = 0;
        return SERIAL_OK;
    }

    /*
     * Directory read
     * A little-known feature of the 1541: open 1,8,2,"$" (or even 1,8,1).
     * It gives you the BAM+DIR as a sequential file, containing the data
     * just as it appears on disk.  -Olaf Seibert
     */

    if (*name == '$') {
        int retlen;

        if (secondary > 0) {
            track = vdrive->Dir_Track;
            sector = 0;
            goto as_if_sequential;
        }

        p->mode = BUFFER_DIRECTORY_READ;
        p->buffer = (BYTE *)xmalloc(DIR_MAXBUF);
        retlen = vdrive_dir_create_directory(vdrive, realname, reallength,
                                             filetype, p->buffer);

        if (retlen < 0) {
            /* Directory not valid.  */
            p->mode = BUFFER_NOT_IN_USE;
            free(p->buffer);
            p->length = 0;
            vdrive_command_set_error(vdrive, IPE_NOT_FOUND, 0, 0);
            return SERIAL_ERROR;
        }
        p->length = (unsigned int)retlen;
        p->bufptr = 0;
        return SERIAL_OK;
    }

    /*
     * Now, set filetype according secondary address, if it was not specified
     * on filename
     */

    if (!filetype)
        filetype = (secondary < 2) ? FT_PRG : FT_SEQ;

    /*
     * Check that there is room on directory.
     */
    vdrive_dir_find_first_slot(vdrive, realname, reallength, 0);

    /*
     * Find the first non-DEL entry in the directory (if it exists).
     */
    do
        slot = vdrive_dir_find_next_slot(vdrive);
    while (slot && ((slot[SLOT_TYPE_OFFSET] & 0x07) == FT_DEL));

    p->readmode = readmode;
    p->slot = slot;

    if (filetype == FT_REL)
        return vdrive_rel_open(vdrive, secondary, record_length);

    /*
     * Open file for reading
     */

    if (readmode == FAM_READ) {
        int status, type;

        if (!slot) {
            vdrive_iec_close(vdrive, secondary);
            vdrive_command_set_error(vdrive, IPE_NOT_FOUND, 0, 0);
            return SERIAL_ERROR;
        }

        type = slot[SLOT_TYPE_OFFSET] & 0x07;

        filetype = type;

        track = (int)slot[SLOT_FIRST_TRACK];
        sector = (int)slot[SLOT_FIRST_SECTOR];

        /*
         * Del, Seq, Prg, Usr (Rel not yet supported)
         */
        if (type != FT_REL) {
            as_if_sequential:
            p->mode = BUFFER_SEQUENTIAL;
            p->bufptr = 2;
            p->buffer = (BYTE *)xmalloc(256);

            status = disk_image_read_sector(vdrive->image, p->buffer,
                                            (unsigned int)track,
                                            (unsigned int)sector);
            vdrive_set_last_read((unsigned int)track, (unsigned int)sector,
                                 p->buffer);
            if (status != 0) {
                vdrive_iec_close(vdrive, secondary);
                return SERIAL_ERROR;
            }
            return SERIAL_OK;
        }
    /*
     * Unsupported filetype
     */
        return SERIAL_ERROR;
    }

    /*
     * Write
     */

    if (vdrive->image->read_only) {
        vdrive_command_set_error(vdrive, IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

    if (slot) {
        if (*name == '@')
            vdrive_dir_remove_slot(vdrive, slot);
        else {
            vdrive_iec_close(vdrive, secondary);
            vdrive_command_set_error(vdrive, IPE_FILE_EXISTS, 0, 0);
            return SERIAL_ERROR;
        }
    }

    vdrive_dir_create_slot(p, realname, reallength, filetype);

#if 0
    /* XXX keeping entry until close not implemented */
    /* Write the directory entry to disk as an UNCLOSED file. */

    vdrive_dir_find_first_slot(vdrive, NULL, -1, 0);
    e = vdrive_dir_find_next_slot(vdrive);

    if (!e) {
        p->mode = BUFFER_NOT_IN_USE;
        free((char *)p->buffer);
        p->buffer = NULL;
        vdrive_command_set_error(vdrive, IPE_DISK_FULL, 0, 0);
        return SERIAL_ERROR;
    }

    memcpy(&vdrive->Dir_buffer[vdrive->SlotNumber * 32 + 2],
           p->slot + 2, 30);

    disk_image_write_sector(vdrive->image, vdrive->Dir_buffer,
                            vdrive->Curr_track, vdrive->Curr_sector);
    /*vdrive_bam_write_bam(vdrive);*/
#endif  /* BAM write */
    return SERIAL_OK;
}


int vdrive_iec_close(vdrive_t *vdrive, unsigned int secondary)
{
    BYTE *e;
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

#ifdef __riscos
    ui_set_drive_leds(vdrive->unit - 8, 0);
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        return SERIAL_OK; /* FIXME: Is this correct? */

      case BUFFER_MEMORY_BUFFER:
      case BUFFER_DIRECTORY_READ:
        free((char *)p->buffer);
        p->mode = BUFFER_NOT_IN_USE;
        p->buffer = NULL;
        p->slot = NULL;
        break;
      case BUFFER_SEQUENTIAL:
        if (p->readmode & (FAM_WRITE | FAM_APPEND)) {
            /*
             * Flush bytes and write slot to directory
             */

            if (vdrive->image->read_only) {
                vdrive_command_set_error(vdrive, IPE_WRITE_PROTECT_ON, 0, 0);
                return SERIAL_ERROR;
            }

#ifdef DEBUG_DRIVE
            log_debug("DEBUG: flush.");
#endif
            write_sequential_buffer(vdrive, p, p->bufptr);

#ifdef DEBUG_DRIVE
            log_debug("DEBUG: find empty DIR slot.");
#endif
            vdrive_dir_find_first_slot(vdrive, NULL, -1, 0);
            e = vdrive_dir_find_next_slot(vdrive);

            if (!e) {
                p->mode = BUFFER_NOT_IN_USE;
                free((char *)p->buffer);
                p->buffer = NULL;

                vdrive_command_set_error(vdrive, IPE_DISK_FULL, 0, 0);
                return SERIAL_ERROR;
            }
            p->slot[SLOT_TYPE_OFFSET] |= 0x80; /* Closed */

            memcpy(&vdrive->Dir_buffer[vdrive->SlotNumber * 32 + 2],
                   p->slot + 2, 30);

#ifdef DEBUG_DRIVE
            log_debug("DEBUG: closing, write DIR slot (%d %d) and BAM.",
                      vdrive->Curr_track, vdrive->Curr_sector);
#endif
            disk_image_write_sector(vdrive->image, vdrive->Dir_buffer,
                                    vdrive->Curr_track, vdrive->Curr_sector);
            vdrive_bam_write_bam(vdrive);
        }
        p->mode = BUFFER_NOT_IN_USE;
        free((char *)p->buffer);
        p->buffer = NULL;
        break;
      case BUFFER_RELATIVE:
        /* FIXME !!! */
        p->mode = BUFFER_NOT_IN_USE;
        free((char *)p->buffer);
        p->buffer = NULL;
        break;
      case BUFFER_COMMAND_CHANNEL:
        /* I'm not sure if this is correct, but really closing the buffer
           should reset the read pointer to the beginning for the next
           write! */
        vdrive_command_set_error(vdrive, IPE_OK, 0, 0);
        vdrive_close_all_channels(vdrive);
        break;
      default:
        log_error(vdrive_iec_log, "Fatal: unknown floppy-close-mode: %i.",
                  p->mode);
        exit(-1);
    }
    return SERIAL_OK;
}


int vdrive_iec_read(vdrive_t *vdrive, BYTE *data, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        vdrive_command_set_error(vdrive, IPE_NOT_OPEN, 0, 0);
        return SERIAL_ERROR;

      case BUFFER_DIRECTORY_READ:
        if (p->bufptr >= p->length) {
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      case BUFFER_MEMORY_BUFFER:
        if (p->bufptr >= 256) {
            *data = 0xc7;
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      case BUFFER_SEQUENTIAL:
        if (p->readmode != FAM_READ)
            return SERIAL_ERROR;

        /*
         * Read next block if needed
         */
        if (p->buffer[0]) {
            if (p->bufptr >= 256) {
                int status;
                unsigned int track, sector;

                track = (unsigned int)p->buffer[0];
                sector = (unsigned int)p->buffer[1];

                status = disk_image_read_sector(vdrive->image, p->buffer,
                                                track, sector);
                vdrive_set_last_read(track, sector, p->buffer);

                if (status == 0) {
                    p->bufptr = 2;
                } else {
                    *data = 0xc7;
                    return SERIAL_EOF;
                }
            }
        } else {
            if (p->bufptr > p->buffer[1]) {
                *data = 0xc7;
#ifdef DEBUG_DRIVE
                if (p->mode == BUFFER_COMMAND_CHANNEL)
                    log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                              p->mode, 0, 0, *data, (isprint(*data)
                              ? *data : '.'));
#endif
                return SERIAL_EOF;
            }
        }

        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;
      case BUFFER_COMMAND_CHANNEL:
        if (p->bufptr > p->length) {
            vdrive_command_set_error(vdrive, IPE_OK, 0, 0);
#ifdef DEBUG_DRIVE
            log_debug("End of buffer in command channel.");
#endif
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;
      case BUFFER_RELATIVE:
        if (p->bufptr > p->length) {
            vdrive_command_set_error(vdrive, IPE_OK, 0, 0);
            *data = 0xc7;
#ifdef DEBUG_DRIVE
            if (p->mode == BUFFER_COMMAND_CHANNEL)
                log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                          p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
            return SERIAL_EOF;
        }
        *data = p->buffer[p->bufptr];
        p->bufptr++;
        break;

      default:
        log_error(vdrive_iec_log, "Fatal: unknown buffermode on floppy-read.");
        exit(-1);
    }

#ifdef DEBUG_DRIVE
    if (p->mode == BUFFER_COMMAND_CHANNEL)
        log_debug("Disk read  %d [%02d %02d] data %02x (%c).",
                  p->mode, 0, 0, *data, (isprint(*data) ? *data : '.'));
#endif
    return SERIAL_OK;
}


int vdrive_iec_write(vdrive_t *vdrive, BYTE data, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);

    if (vdrive->image->read_only && p->mode != BUFFER_COMMAND_CHANNEL) {
        vdrive_command_set_error(vdrive, IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

#ifdef DEBUG_DRIVE
    if (p -> mode == BUFFER_COMMAND_CHANNEL)
        log_debug("Disk write %d [%02d %02d] data %02x (%c).",
                  p->mode, 0, 0, data, (isprint(data) ? data : '.') );
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        vdrive_command_set_error(vdrive, IPE_NOT_OPEN, 0, 0);
        return SERIAL_ERROR;
      case BUFFER_DIRECTORY_READ:
        vdrive_command_set_error(vdrive, IPE_NOT_WRITE, 0, 0);
        return SERIAL_ERROR;
      case BUFFER_MEMORY_BUFFER:
        if (p->bufptr >= 256)
            return SERIAL_ERROR;
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        return SERIAL_OK;
      case BUFFER_SEQUENTIAL:
        if (p->readmode == FAM_READ)
            return SERIAL_ERROR;

        if (p->bufptr >= 256) {
            p->bufptr = 2;
            if (write_sequential_buffer(vdrive, p, WRITE_BLOCK) < 0)
                return SERIAL_ERROR;
        }
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        break;
      case BUFFER_COMMAND_CHANNEL:
        if (p->readmode == FAM_READ) {
            p->bufptr = 0;
            p->readmode = FAM_WRITE;
        }
        if (p->bufptr >= 256) /* Limits checked later */
            return SERIAL_ERROR;
        p->buffer[p->bufptr] = data;
        p->bufptr++;
        break;
      default:
        log_error(vdrive_iec_log, "Fatal: Unknown write mode.");
        exit(-1);
    }
    return SERIAL_OK;
}

void vdrive_iec_flush(vdrive_t *vdrive, unsigned int secondary)
{
    bufferinfo_t *p = &(vdrive->buffers[secondary]);
    int status;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH:, secondary = %d, buffer=%s\n "
                 "  bufptr=%d, length=%d, read?=%d.", secondary, p->buffer,
                 p->bufptr, p->length, p->readmode==FAM_READ);
#endif

    if (p->mode != BUFFER_COMMAND_CHANNEL)
        return;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH: COMMAND CHANNEL");
#endif

    if (p->readmode == FAM_READ)
        return;

#ifdef DEBUG_DRIVE
       log_debug("FLUSH: READ MODE");
#endif

    if (p->length) {
        /* If no command, do nothing - keep error code.  */
        status = vdrive_command_execute(vdrive, p->buffer, p->bufptr);
        p->bufptr = 0;
    }
}

int vdrive_iec_attach(unsigned int unit, const char *name)
{
    return serial_attach_device(unit, name, vdrive_iec_read, vdrive_iec_write,
                                vdrive_iec_open, vdrive_iec_close,
                                vdrive_iec_flush);
}

