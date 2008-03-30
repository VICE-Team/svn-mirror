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

#include "diskimage.h"
#include "vdrive-bam.h"
#include "vdrive-dir.h"
#include "vdrive.h"

extern char *slot_type[]; /* FIXME: Away with this!  */

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

static void vdrive_dir_free_chain(vdrive_t *vdrive, int t, int s)
{
    BYTE buf[256];
    int disk_type = -1;

    switch (vdrive->image_format) {
      case VDRIVE_IMAGE_FORMAT_1541:
        disk_type = DISK_IMAGE_TYPE_D64;
        break;
      case VDRIVE_IMAGE_FORMAT_1571:
        disk_type = DISK_IMAGE_TYPE_D71;
        break;
      case VDRIVE_IMAGE_FORMAT_1581:
        disk_type = DISK_IMAGE_TYPE_D81;
        break;
      case VDRIVE_IMAGE_FORMAT_8050:
        disk_type = DISK_IMAGE_TYPE_D80;
        break;
      case VDRIVE_IMAGE_FORMAT_8250:
        disk_type = DISK_IMAGE_TYPE_D82;
        break;
    }

    while (t) {
        /* Check for illegal track or sector.  */
        if (disk_image_check_sector(disk_type, t, s) < 0)
            break;

        /* Check if this sector is really allocated.  */
        if (!vdrive_bam_free_sector(vdrive->image_format, vdrive->bam, t, s))
            break;

        /* FIXME: This seems to be redundant.  AB19981124  */
        vdrive_bam_free_sector(vdrive->image_format, vdrive->bam, t, s);
        disk_image_read_sector(vdrive->image, buf, t, s);
        t = (int)buf[0];
        s = (int)buf[1];
    }
}

static BYTE *find_next_directory_sector(vdrive_t *floppy, int track, int sector)
{
    if (vdrive_bam_allocate_sector(floppy->image_format, floppy->bam, track,
        sector)) {
        floppy->Dir_buffer[0] = track;
        floppy->Dir_buffer[1] = sector;
        disk_image_write_sector(floppy->image, floppy->Dir_buffer,
                                floppy->Curr_track, floppy->Curr_sector);
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

void vdrive_dir_remove_slot(vdrive_t *floppy, BYTE *slot)
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
        disk_image_write_sector(floppy->image, floppy->Dir_buffer,
                                floppy->Curr_track, floppy->Curr_sector);
    }
}

void vdrive_dir_find_first_slot(vdrive_t *floppy, const char *name, int length,
                                int type)
{
    floppy->find_name   = name;
    floppy->find_type   = type;
    floppy->find_length = length;

    floppy->Curr_track  = floppy->Dir_Track;
    floppy->Curr_sector = floppy->Dir_Sector;
    floppy->SlotNumber = -1;

    disk_image_read_sector(floppy->image, floppy->Dir_buffer,
                           floppy->Dir_Track, floppy->Dir_Sector);
}

BYTE *vdrive_dir_find_next_slot(vdrive_t *floppy)
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

            status = disk_image_read_sector(floppy->image, floppy->Dir_buffer,
                                            floppy->Curr_track,
                                            floppy->Curr_sector);
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
        switch (floppy->image_format) {
          case VDRIVE_IMAGE_FORMAT_1541:
            for (sector = 1;
                sector < disk_image_sector_per_track(DISK_IMAGE_TYPE_D64,
                DIR_TRACK_1541); sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1541,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case VDRIVE_IMAGE_FORMAT_1571:
            for (sector = 1;
                sector < disk_image_sector_per_track(DISK_IMAGE_TYPE_D71,
                DIR_TRACK_1571); sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1571,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            for (sector = 0;
                sector < disk_image_sector_per_track(DISK_IMAGE_TYPE_D71,
                DIR_TRACK_1571 + 35); sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1571 + 35,                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case VDRIVE_IMAGE_FORMAT_1581:
            for (sector = 3; sector < NUM_SECTORS_1581; sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_1581,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          case VDRIVE_IMAGE_FORMAT_8050:
          case VDRIVE_IMAGE_FORMAT_8250:
            for (sector = 1;
                sector < disk_image_sector_per_track(DISK_IMAGE_TYPE_D80,
                DIR_TRACK_8050); sector++) {
                BYTE *dirbuf;
                dirbuf = find_next_directory_sector(floppy, DIR_TRACK_8050,
                                                    sector);
                if (dirbuf != NULL)
                    return dirbuf;
            }
            break;
          default:
            log_error(vdrive_log,
                      "Unknown disk type %i.  Cannout find directory slot.",
                      floppy->image_format);
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

/*
 * Create directory listing. (called from vdrive_open)
 * If filetype is 0, match for all files.  Return the length in bytes
 * if successful, -1 if the directory is not valid.
 */

int vdrive_dir_create_directory(vdrive_t *floppy, const char *name,
                                int length, int filetype, int secondary,
                                BYTE *outputptr)
{
    BYTE *l, *p;
    BYTE *origptr = outputptr;
    int blocks, addr, i;

    if (length) {
        if (*name == '$') {
            ++name;
            --length;
        }
        if (*name == ':') {
            ++name;
            --length;
        }
    }
    if (!*name || length < 1) {
        name = "*\0";
        length = 1;
    }

    /*
     * Start Address, Line Link and Line number 0
     */

    l = outputptr;
    addr = 0x401;
    SET_LO_HI(l, addr);

    l += 2;                     /* Leave space for Next Line Link */
    *l++ = 0;
    *l++ = 0;

    *l++ = (BYTE) 0x12;         /* Reverse on */

    *l++ = '"';

    memcpy(l, &floppy->bam[floppy->bam_name], 16);
    vdrive_dir_no_a0_pads(l, 16);
    l += 16;
    *l++ = '"';
    *l++ = ' ';
    memcpy(l, &floppy->bam[floppy->bam_id], 5);
    vdrive_dir_no_a0_pads(l, 5);
    l += 5;
    *l++ = 0;


    /*
     * Pointer to the next line
     */

    addr += ((l - outputptr) - 2);

    outputptr[2] = 1;   /* addr & 0xff; */
    outputptr[3] = 1;   /* (addr >>8) & 0xff; */
    outputptr = l;


    /*
     * Now, list files that match the pattern.
     * Wildcards can be used too.
     */

    vdrive_dir_find_first_slot(floppy, name, length, filetype);

    while ((p = vdrive_dir_find_next_slot(floppy))) {
        BYTE *tl;

        /* Check whether the directory exceeds the malloced memory.  We make
           sure there is enough space for two lines because we also have to
           add the final ``...BLOCKS FREE'' line.  */
        if ((l - origptr) >= DIR_MAXBUF - 64) {
            log_error(vdrive_log, "Directory too long: giving up.");
            return -1;
        }

        if (p[SLOT_TYPE_OFFSET]) {

            tl = l;
            l += 2;

            /*
             * Length and spaces
             */
            blocks = p[SLOT_NR_BLOCKS] + p[SLOT_NR_BLOCKS + 1] * 256;
            SET_LO_HI(l, blocks);

            if (blocks < 10)
                *l++ = ' ';
            if (blocks < 100)
                *l++ = ' ';
            /*
             * Filename
             */

            *l++ = ' ';
            *l++ = '"';

            memcpy(l, &p[SLOT_NAME_OFFSET], 16);

            for (i = 0; (i < 16) && (p[SLOT_NAME_OFFSET + i] != 0xa0);)
                i++;

            vdrive_dir_no_a0_pads(l, 16);

            l[16] = ' ';
            l[i] = '"';
            l += 17;

            /*
             * Type + End
             * There are 3 spaces or < and 2 spaces after the filetype.
             * Well, not exactly - the whole directory entry is 32 byte long
             * (including nullbyte).
             * Depending on the file size, there are more or less spaces
             */

            sprintf ((char *)l, "%c%s%c%c",
                    (p[SLOT_TYPE_OFFSET] & FT_CLOSED ? ' ' : '*'),
                    slot_type[p[SLOT_TYPE_OFFSET] & 0x07],
                    (p[SLOT_TYPE_OFFSET] & FT_LOCKED ? '<' : ' '),
                    0);
            l += 5;
            i = l - tl;

            while (i < 31) {
                *l++ = ' ';
                i++;
            }
            *l++ = '\0';

            /*
             * New address
             */

            addr += l - outputptr;
            outputptr[0] = 1;   /* addr & 0xff; */
            outputptr[1] = 1;   /* (addr >> 8) & 0xff; */
            outputptr = l;
        }
    }

    blocks = vdrive_bam_free_block_count(floppy);

    *l++ = 0;
    *l++ = 0;
    SET_LO_HI(l, blocks);
    memcpy(l, "BLOCKS FREE.", 12);
    l += 12;
    memset(l, ' ', 13);
    l += 13;
    *l++ = (char) 0;

    /* Line Address */
    addr += l - outputptr;
    outputptr[0] = 1;   /* addr & 0xff; */
    outputptr[1] = 1;   /* (addr / 256) & 0xff; */

    /*
     * end
     */
    *l++ = (char) 0;
    *l++ = (char) 0;
    *l   = (char) 0;

    return (l - origptr);
}

