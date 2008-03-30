/*
 * vdrive-iec.c - Virtual disk-drive IEC implementation.
 *
 * Written by
 *  Teemu Rantanen      (tvr@cs.hut.fi)
 *  Jarkko Sonninen     (sonninen@lut.fi)
 *  Jouko Valta         (jopi@stekt.oulu.fi)
 *  Olaf Seibert        (rhialto@mbfys.kun.nl)
 *  André Fachat        (a.fachat@physik.tu-chemnitz.de)
 *  Ettore Perazzoli    (ettore@comm2000.it)
 *  Martin Pottendorfer (Martin.Pottendorfer@aut.alcatel.at)
 *  Andreas Boose       (boose@unixserv.rz.fh-hannover.de)
 *
 * Patches by
 *  Dan Miner           (dminer@nyx10.cs.du.edu)
 *  Germano Caronni     (caronni@tik.ethz.ch)
 *  Daniel Fandrich     (dan@fch.wimsey.bc.ca)	/DF/
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

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#ifdef __riscos
#include "ROlib.h"
#include "ui.h"
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <memory.h>
#endif
#include <errno.h>
#endif

#include "vdrive.h"
#include "vdrive-iec.h"
#include "utils.h"

extern char *slot_type[]; /* FIXME: Away with this!  */

/*
 * Create directory listing. (called from vdrive_open)
 * If filetype is 0, match for all files.  Return the length in bytes
 * if successful, -1 if the directory is not valid.
 */

static int floppy_create_directory(DRIVE *floppy, char *name,
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
    no_a0_pads(l, 16);
    l += 16;
    *l++ = '"';
    *l++ = ' ';
    memcpy(l, &floppy->bam[floppy->bam_id], 5);
    no_a0_pads(l, 5);
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

    set_find_first_slot(floppy, name, length, filetype);

    while ((p = find_next_slot(floppy))) {
        BYTE *tl;

        /* Check whether the directory exceeds the malloced memory.  We make
           sure there is enough space for two lines because we also have to
           add the final ``...BLOCKS FREE'' line.  */
        if ((l - origptr) >= DIR_MAXBUF - 64) {
            fprintf(errfile, "Directory too long: giving up.\n");
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

            no_a0_pads(l, 16);

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

    blocks = floppy_free_block_count(floppy);

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

static int write_sequential_buffer(DRIVE *floppy, bufferinfo_t *bi,
                                   int length )
{
    int t_new, s_new, e;
    BYTE *buf = bi->buffer;
    BYTE *slot = bi->slot;

    /*
     * First block of a file ?
     */
    if (slot[SLOT_FIRST_TRACK] == 0) {
        e = alloc_first_free_sector(floppy, floppy->bam, &t_new, &s_new);
        if (e < 0) {
            floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
            return -1;
        }
        slot[SLOT_FIRST_TRACK]  = bi->track  = t_new;
        slot[SLOT_FIRST_SECTOR] = bi->sector = s_new;
    }

    if (length == WRITE_BLOCK) {
        /*
         * Write current sector and allocate next
         */
        t_new = bi->track;
        s_new = bi->sector;
        e = alloc_next_free_sector(floppy, floppy->bam, &t_new, &s_new);
        if (e < 0) {
            floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
            return -1;
        }
        buf[0] = t_new;
        buf[1] = s_new;

        floppy_write_block(floppy->ActiveFd, floppy->ImageFormat, buf,
                           bi->track, bi->sector, floppy->D64_Header);

        bi->track = t_new;
        bi->sector = s_new;
    } else {
        /*
         * Write last block
         */
        buf[0] = 0;
        buf[1] = length - 1;

        floppy_write_block(floppy->ActiveFd, floppy->ImageFormat, buf,
                           bi->track, bi->sector, floppy->D64_Header);
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

int vdrive_open(void *flp, char *name, int length, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);
    char realname[256];
    int reallength, readmode, filetype, rl;
    int track, sector;
    BYTE *slot; /* Current directory entry */

    if ((!name || !*name) && p->mode != BUFFER_COMMAND_CHANNEL)  /* EP */
        return SERIAL_NO_DEVICE;	/* Routine was called incorrectly. */

    /*
     * No floppy in drive ?
     *
     * On systems with limited memory it may save resources not to keep
     * the image file open all the time.
     * If ActiveName exists, the file may have been temporarily closed.
     */

   if (floppy->ActiveFd == ILLEGAL_FILE_DESC
       && p->mode != BUFFER_COMMAND_CHANNEL
       && secondary != 15
       && *name != '#') {
       floppy_error(&floppy->buffers[15], IPE_NOT_READY, 18, 0);
       fprintf(logfile, "Drive not ready.\n");
       return SERIAL_ERROR;
   }

#ifdef DEBUG_DRIVE
    fprintf(logfile, "VDRIVE#%i: OPEN: FD = %d - Name '%s' (%d) on ch %d\n",
	        floppy->unit, (int)(floppy->ActiveFd), name, length, secondary);
#endif
#ifdef __riscos
    ui_set_drive_leds(floppy->unit - 8, 1);
#endif

    /*
     * If channel is command channel, name will be used as write. Return only
     * status of last write ...
     */
    if (p->mode == BUFFER_COMMAND_CHANNEL) {
        int n;
        int status = SERIAL_OK;

        for (n = 0; n < length; n++)
            status = vdrive_write(floppy, name[n], secondary);
        if (length)
            p->readmode = FAM_WRITE;
        else
            p->readmode = FAM_READ;
        return status;
    }

    /*
     * Clear error flag
     */
    floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);

    /*
     * In use ?
     */
    if (p->mode != BUFFER_NOT_IN_USE) {
#ifdef DEBUG_DRIVE
	printf (logfile, "Cannot open channel %d. Mode is %d.\n", secondary, p->mode);
#endif
	floppy_error(&floppy->buffers[15], IPE_NO_CHANNEL, 0, 0);
        return SERIAL_ERROR;
    }

    /*
     * Filemode / type
     */
    if (secondary == 1)
        readmode = FAM_WRITE;
    else
        readmode = FAM_READ;

    filetype = 0;
    rl = 0;  /* REL */

    if (floppy_parse_name(name, length, realname, &reallength,
                          &readmode, &filetype, &rl) != SERIAL_OK)
        return SERIAL_ERROR;

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
        if (secondary > 0) {
            track = floppy->Dir_Track;
            sector = 0;
            goto as_if_sequential;
        }

        p->mode = BUFFER_DIRECTORY_READ;
        p->buffer = (BYTE *)xmalloc(DIR_MAXBUF);
        p->length = floppy_create_directory(floppy,
                                            realname, reallength, filetype,
                                            secondary, p->buffer);
        if (p->length < 0) {
            /* Directory not valid. */
            p->mode = BUFFER_NOT_IN_USE;
            free(p->buffer);
            p->length = 0;
            floppy_error(&floppy->buffers[15], IPE_NOT_FOUND, 0, 0);
            return SERIAL_ERROR;
        }
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
    set_find_first_slot(floppy, realname, reallength, 0);

    /*
     * Find the first non-DEL entry in the directory (if it exists).
     */
    do
        slot = find_next_slot(floppy);
    while (slot && ((slot[SLOT_TYPE_OFFSET] & 0x07) == FT_DEL));

    p->readmode = readmode;
    p->slot = slot;

    /*
     * Open file for reading
     */

    if (readmode == FAM_READ) {
        int status, type;

        if (!slot) {
            vdrive_close(floppy, secondary);
            floppy_error(&floppy->buffers[15], IPE_NOT_FOUND, 0, 0);
            return SERIAL_ERROR;
        }

        type = slot[SLOT_TYPE_OFFSET] & 0x07;

        /*  I don't think that this one is needed - EP
        if (filetype && type != filetype) {
            floppy_error(&floppy->buffers[15], IPE_BAD_TYPE, 0, 0);
            return SERIAL_ERROR;
	    }
        */

        filetype = type;  /* EP */

        track = (int) slot[SLOT_FIRST_TRACK];
        sector = (int) slot[SLOT_FIRST_SECTOR];

        /*
         * Del, Seq, Prg, Usr (Rel not yet supported)
         */
        if (type != FT_REL) {
	        as_if_sequential:
	        p->mode = BUFFER_SEQUENTIAL;
	        p->bufptr = 2;
	        p->buffer = (BYTE *)xmalloc(256);

	        status = floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
                                       p->buffer, track, sector,
                                       floppy->D64_Header);
            if (status < 0) {
                vdrive_close(floppy, secondary);
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

    if (floppy->ReadOnly) {
        floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

    if (slot) {
        if (*name == '@')
            remove_slot(floppy, slot);
        else {
            vdrive_close(floppy, secondary);
            floppy_error(&floppy->buffers[15], IPE_FILE_EXISTS, 0, 0);
            return SERIAL_ERROR;
        }
    }

    /*
     * Create slot information.
     * Different front-ends can fetch the real filename from slot info.
     */

    p->slot = (BYTE *)xmalloc(32);
    memset(p->slot, 0, 32);
    memset(p->slot + SLOT_NAME_OFFSET, 0xa0, 16);
    memcpy(p->slot + SLOT_NAME_OFFSET, realname, reallength);
#ifdef DEBUG_DRIVE
    fprintf(logfile, "DIR: name (%d) '%s'\n", reallength, realname);
#endif
    p->slot[SLOT_TYPE_OFFSET] = filetype;	/* unclosed */

    p->buffer = (BYTE *)xmalloc(256);
    p->mode = BUFFER_SEQUENTIAL;
    p->bufptr = 2;

#if 0
    /* XXX keeping entry until close not implemented */
    /* Write the directory entry to disk as an UNCLOSED file. */

#ifdef DEBUG_DRIVE
    fprintf(logfile, "DEBUG: find empty DIR slot.\n");
#endif

    set_find_first_slot(floppy, NULL, -1, 0);
    e = find_next_slot(floppy);

    if (!e) {
        p->mode = BUFFER_NOT_IN_USE;
        free((char *)p->buffer);
        p->buffer = NULL;
        floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
        return SERIAL_ERROR;
    }

    memcpy(&floppy->Dir_buffer[floppy->SlotNumber * 32 + 2],
           p->slot + 2, 30);

#ifdef DEBUG_DRIVE
    fprintf(logfile, "DEBUG: create, write DIR slot (%d %d).\n",
            floppy->Curr_track, floppy->Curr_sector);
#endif
    floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                       floppy->Dir_buffer, floppy->Curr_track,
                       floppy->Curr_sector, floppy->D64_Header);

    /*floppy_write_bam(floppy);*/
#endif  /* BAM write */
    return SERIAL_OK;
}


int vdrive_close(void *flp, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    BYTE   *e;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

#ifdef DEBUG_DRIVE
    fprintf(logfile, "DEBUG: closing file #%d.\n", secondary);
#endif
#ifdef __riscos
    ui_set_drive_leds(floppy->unit - 8, 0);
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

            if (floppy->ReadOnly) {
                floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0, 0);
                return SERIAL_ERROR;
            }

#ifdef DEBUG_DRIVE
            fprintf(logfile, "DEBUG: flush.\n");
#endif
            write_sequential_buffer(floppy, p, p->bufptr);

#ifdef DEBUG_DRIVE
            fprintf(logfile, "DEBUG: find empty DIR slot.\n");
#endif
            set_find_first_slot(floppy, NULL, -1, 0);
            e = find_next_slot(floppy);

            if (!e) {
                p->mode = BUFFER_NOT_IN_USE;
                free((char *)p->buffer);
                p->buffer = NULL;

                floppy_error(&floppy->buffers[15], IPE_DISK_FULL, 0, 0);
                return SERIAL_ERROR;
            }
            p->slot[SLOT_TYPE_OFFSET] |= 0x80; /* Closed */

            memcpy(&floppy->Dir_buffer[floppy->SlotNumber * 32 + 2],
                   p->slot + 2, 30);

#ifdef DEBUG_DRIVE
            fprintf(logfile, "DEBUG: closing, write DIR slot (%d %d) and BAM.\n",
                    floppy->Curr_track, floppy->Curr_sector);
#endif
            floppy_write_block(floppy->ActiveFd, floppy->ImageFormat,
                               floppy->Dir_buffer, floppy->Curr_track,
                               floppy->Curr_sector, floppy->D64_Header);
            floppy_write_bam(floppy);
        }
        p->mode = BUFFER_NOT_IN_USE;
        free((char *)p->buffer);
        p->buffer = NULL;
        break;
      case BUFFER_COMMAND_CHANNEL:
        /* I'm not sure if this is correct, but really closing the buffer
           should reset the read pointer to the beginning for the next
           write! */
        floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
        floppy_close_all_channels(floppy);
        break;
      default:
        fprintf(errfile, "\nFatal: unknown floppy-close-mode: %i\n", p->mode);
        exit (-1);
    }
    return SERIAL_OK;
}


int vdrive_read(void *flp, BYTE *data, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

#ifdef DEBUG_DRIVE
    if (p->mode == BUFFER_COMMAND_CHANNEL)
	fprintf(logfile, "Disk read  %d [%02d %02d]\n", p->mode, 0, 0);
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
	floppy_error(&floppy->buffers[15], IPE_NOT_OPEN, 0, 0);
	return SERIAL_ERROR;

      case BUFFER_DIRECTORY_READ:
	if (p->bufptr >= p->length) {
            *data = 0xc7;
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
		floppy_read_block(floppy->ActiveFd, floppy->ImageFormat,
				  p->buffer,
				  (int) p->buffer[0],
				  (int) p->buffer[1],
				  floppy->D64_Header);
		p->bufptr = 2;
	    }
	} else {
	    if (p->bufptr > p->buffer[1]) {
                *data = 0xc7;
		return SERIAL_EOF;
            }
	}

	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      case BUFFER_COMMAND_CHANNEL:
	if (p->bufptr > p->length) {
	    floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
#ifdef DEBUG_DRIVE
	    fprintf(logfile, "end of buffer in command channel\n" );
#endif
            *data = 0xc7;
	    return SERIAL_EOF;
	}
	*data = p->buffer[p->bufptr];
	p->bufptr++;
	break;

      default:
	fprintf(errfile, "\nFatal: unknown buffermode on floppy-read\n");
	exit(-1);
    }

    return SERIAL_OK;
}


int vdrive_write(void *flp, BYTE data, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);

    if (floppy->ReadOnly && p->mode != BUFFER_COMMAND_CHANNEL) {
        floppy_error(&floppy->buffers[15], IPE_WRITE_PROTECT_ON, 0, 0);
        return SERIAL_ERROR;
    }

#ifdef DEBUG_DRIVE
    if (p -> mode == BUFFER_COMMAND_CHANNEL)
        printf("Disk write %d [%02d %02d] data %02x (%c)\n",
               p->mode, 0, 0, data, (isprint(data) ? data : '.') );
#endif

    switch (p->mode) {
      case BUFFER_NOT_IN_USE:
        floppy_error(&floppy->buffers[15], IPE_NOT_OPEN, 0, 0);
        return SERIAL_ERROR;
      case BUFFER_DIRECTORY_READ:
        floppy_error(&floppy->buffers[15], IPE_NOT_WRITE, 0, 0);
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
            if (write_sequential_buffer(floppy, p, WRITE_BLOCK) < 0)
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
        fprintf(errfile, "\nFatal: Unknown write mode\n");
        exit(-1);
    }
    return SERIAL_OK;
}

void vdrive_flush(void *flp, int secondary)
{
    DRIVE *floppy = (DRIVE *)flp;
    bufferinfo_t *p = &(floppy->buffers[secondary]);
    int status;

#ifdef DEBUG_DRIVE
printf("VDRIVE#%i: FLUSH:, secondary = %d, buffer=%s\n   bufptr=%d, length=%d, "
       "read?=%d\n", floppy->unit, secondary, p->buffer, p->bufptr, p->length,
       p->readmode==FAM_READ);
#endif

    if (p->mode != BUFFER_COMMAND_CHANNEL)
        return;

    if (p->readmode == FAM_READ)
        return;
    if (p->length) { /* if no command, do nothing - keep error code. */
        status = ip_execute(floppy, p->buffer, p->bufptr);
        p->bufptr = 0;
        if (status == IPE_OK)
            floppy_error(&floppy->buffers[15], IPE_OK, 0, 0);
    }
}

