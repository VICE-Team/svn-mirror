/*
 * vdrive-dir.c - Virtual disk-drive implementation.
 *                Directory specific functions.
 *
 * Written by
 *  Andreas Boose       <boose@linux.rz.fh-hannover.de>
 *
 * Based on old code by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
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

#include <string.h>

#include "vdrive-bam.h"
#include "vdrive-dir.h"
#include "vdrive.h"

extern char sector_map_1541[43];
extern char sector_map_1571[71];
extern char pet_sector_map[78];

static int vdrive_dir_name_match(BYTE *slot, const char *name, int length,
                                 int type){
    int i;

    if (length < 0) {
        if (slot[SLOT_TYPE_OFFSET])
            return 0;
        else
            return 1;
    }
    if (!slot[SLOT_TYPE_OFFSET])
        return 0;

    if (length > 16)
        length = 16;

    for (i = 0; i < length; i++) {
        switch (name[i]) {
          case '?':             /* Match any character */
            break;

          case '*':             /* Make a match */
            i = 16;
            break;

          default:
            if ((BYTE)name[i] != slot[i + SLOT_NAME_OFFSET])
                return 0;
        }
    }

    /*
     * Got name match ?
     */
    if (i < 16 && slot[SLOT_NAME_OFFSET + i] != 0xa0)
        return 0;

    if (type && type != (slot[SLOT_TYPE_OFFSET] & 0x07))
        return 0;

    return 1;
}

static void vdrive_dir_free_chain(DRIVE *floppy, int t, int s)
{
    BYTE buf[256];

    while (t) {
        /* Check for illegal track or sector.  */
        if (vdrive_check_track_sector(floppy->ImageFormat, t, s) < 0)
        break;
        /* Check if this sector is really allocated.  */
        if (!vdrive_bam_free_sector(floppy->ImageFormat, floppy->bam, t, s))
        break;

        /* FIXME: This seems to be redundant.  AB19981124  */
        vdrive_bam_free_sector(floppy->ImageFormat, floppy->bam, t, s);
        floppy_read_block(floppy->ActiveFd, floppy->ImageFormat, buf, t, s,
                          floppy->D64_Header, floppy->GCR_Header, floppy->unit);        t = (int) buf[0];
        s = (int) buf[1];
    }
}

static BYTE *find_next_directory_sector(DRIVE *floppy, int track, int sector)
{
    if (vdrive_bam_allocate_sector(floppy->ImageFormat, floppy->bam, track,
        sector)) {
        floppy->Dir_buffer[0] = track;
        floppy->Dir_buffer[1] = sector;
        floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                           floppy->Dir_buffer, floppy->Curr_track,
                           floppy->Curr_sector, floppy->D64_Header,
                           floppy->GCR_Header, floppy->unit);
#ifdef DEBUG_DRIVE
        fprintf(logfile, "Found (%d %d) TR = %d SE = %d.\n",
                track, sector, floppy->Curr_track, floppy->Curr_sector);
#endif
        floppy->SlotNumber = 0;
        memset(floppy->Dir_buffer, 0, 256);
        floppy->Dir_buffer[1] = 0xff;
        floppy->Curr_sector = sector;
        return floppy->Dir_buffer;
    }
    return NULL;
}

/*
 * vdrive_dir_remove_slot() is called from vdrive_open() (in 'save and
 * replace') and from ip_execute() for 'SCRATCH'.
 */

void vdrive_dir_remove_slot(DRIVE *floppy, BYTE *slot)
{
    int tmp, t, s;

    /* Find slot.  */
    for (tmp = 0; (tmp < 16) && slot[SLOT_NAME_OFFSET + tmp] != 0xa0; tmp++);

    vdrive_dir_find_first_slot(floppy,
                               (char *)&slot[SLOT_NAME_OFFSET], tmp,
                               slot[SLOT_TYPE_OFFSET] & 0x07);

    /* If slot slot found, remove.  */
    if (vdrive_dir_find_next_slot(floppy)) {

        /* Free all sector this file is using.  */
        t = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
            + SLOT_FIRST_TRACK];
        s = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
            + SLOT_FIRST_SECTOR];

        vdrive_dir_free_chain(floppy, t, s);

        /* Free side sectors.  */
        t = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
            + SLOT_SIDE_TRACK];
        s = (int) floppy->Dir_buffer[floppy->SlotNumber * 32
            + SLOT_SIDE_SECTOR];

        vdrive_dir_free_chain(floppy, t, s);

        /* Update bam */
        vdrive_bam_write_bam(floppy);

        /* Update directory entry */
        floppy->Dir_buffer[floppy->SlotNumber * 32 + SLOT_TYPE_OFFSET] = 0;
        floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                           floppy->Dir_buffer, floppy->Curr_track,
                           floppy->Curr_sector, floppy->D64_Header,
                           floppy->GCR_Header, floppy->unit);
    }
}

void vdrive_dir_find_first_slot(DRIVE *floppy, const char *name, int length,
                                int type)
{
    floppy->find_name   = name;
    floppy->find_type   = type;
    floppy->find_length = length;

    floppy->Curr_track  = floppy->Dir_Track;
    floppy->Curr_sector = floppy->Dir_Sector;
    floppy->SlotNumber = -1;

    floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                      floppy->Dir_buffer, floppy->Dir_Track,
                      floppy->Dir_Sector, floppy->D64_Header,
                      floppy->GCR_Header, floppy->unit);
}

BYTE *vdrive_dir_find_next_slot(DRIVE *floppy)
{
    static BYTE return_slot[32];
    int status;

    floppy->SlotNumber++;

    /*
     * Loop all directory blocks starting from track 18, sector 1 (1541).
     */

    do {
        /*
         * Load next(first) directory block ?
         */

        if (floppy->SlotNumber >= 8) {
            if (!(*(floppy->Dir_buffer))) {
                return NULL;
            }

            floppy->SlotNumber = 0;
            floppy->Curr_track  = (int) floppy->Dir_buffer[0];
            floppy->Curr_sector = (int) floppy->Dir_buffer[1];

            status = floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                                       floppy->Dir_buffer, floppy->Curr_track,
                                       floppy->Curr_sector, floppy->D64_Header,
                                       floppy->GCR_Header, floppy->unit);
        }
        while (floppy->SlotNumber < 8) {
            if (vdrive_dir_name_match(&floppy->Dir_buffer[floppy->SlotNumber * 32],
                                  floppy->find_name, floppy->find_length,
                                  floppy->find_type)) {
                /* FIXME: This reads two byte past the size of `Dir_buffer'
                          when `SlotNumber' is 7!  AB19981122 */
                memcpy(return_slot,
                           &floppy->Dir_buffer[floppy->SlotNumber * 32], 32);
                return return_slot;
            }
            floppy->SlotNumber++;
        }
    } while (*(floppy->Dir_buffer));

    /*
     * If length < 0, create new directory-entry if possible
     */

    if (floppy->find_length < 0) {
        int sector;
        switch (floppy->ImageFormat) {
          case 1541:
            for (sector = 1; sector < sector_map_1541[DIR_TRACK_1541];
                sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1541,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case 1571:
            for (sector = 1; sector < sector_map_1571[DIR_TRACK_1571];
                sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1571,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            for (sector = 0; sector < sector_map_1571[DIR_TRACK_1571 + 35];
                sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1571 + 35,                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case 1581:
            for (sector = 3; sector < NUM_SECTORS_1581; sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1581,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case 8050:
          case 8250:
            for (sector = 1; sector < pet_sector_map[DIR_TRACK_8050];
                sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_8050,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          default:
            log_error(vdrive_log, "Unknown disk type.");
            break;
        }
    } /* length */
    return NULL;
}

void vdrive_dir_no_a0_pads(BYTE *ptr, int l)
{
    while (l--) {
        if (*ptr == 0xa0)
            *ptr = 0x20;
        ptr++;
    }
}

