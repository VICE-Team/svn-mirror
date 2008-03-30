
/*
 * fdc.c - 1001/8x50 FDC emulation
 *
 * Written by
 *  Andre' Fachat (fachat@physik.tu-chemnitz.de)
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
#include "types.h"
#include "log.h"
#include "alarm.h"
#include "fdc.h"
#include "drive.h"
#include "drivecpu.h"
#include "vdrive.h"

#undef FDC_DEBUG

static void clk_overflow_callback(int fnum, CLOCK sub, void *data);
static int int_fdc(int fnum, long offset);

static void clk_overflow_callback0(CLOCK sub, void *data)
{
    clk_overflow_callback(0, sub, data);
}

static void clk_overflow_callback1(CLOCK sub, void *data)
{
    clk_overflow_callback(1, sub, data);
}

static int int_fdc0(long offset)
{
    return int_fdc(0, offset);
}

static int int_fdc1(long offset)
{
    return int_fdc(1, offset);
}

/************************************************************************/

#define	NUM_FDC	2

static log_t fdc_log = LOG_ERR;

static BYTE *buffer[NUM_FDC];
static alarm_t fdc_alarm[NUM_FDC];
static int fdc_state[NUM_FDC];
static BYTE *iprom[NUM_FDC];

void fdc_reset(int fnum, int enabled)
{
#ifdef FDC_DEBUG
    log_message(fdc_log, "fdc_reset: enabled=%d\n",enabled);
#endif

    if (enabled) {
	fdc_state[fnum] = FDC_RESET0;
	alarm_set(&fdc_alarm[fnum], drive_clk[fnum] + 20);
    } else {
	alarm_unset(&fdc_alarm[fnum]);
	fdc_state[fnum] = FDC_UNUSED;
    }
}

static BYTE fdc_do_job(int fnum, int buf, 
				int drv, BYTE job, BYTE *header)
{
    static int last_track = 1;

    int rc, dnr;
    int i;
    int track, sector;
    BYTE *base;
    BYTE sector_data[256];
    BYTE disk_id[2];

    track = header[2];
    sector = header[3];

    if (drv) {
	return FDC_ERR_SYNC;
    }
    dnr = fnum;

    rc = 0;
    base = &(buffer[fnum][(buf + 1) << 8]);

#ifdef FDC_DEBUG
    log_message(fdc_log, "do job %02x, buffer %d ($%04x): d%d t%d s%d, "
		"image=%p, type=%04d\n",
		job, buf, (buf + 1) << 8, dnr, track, sector,
		drive[dnr].drive_floppy,
		drive[dnr].drive_floppy 
			? drive[dnr].drive_floppy->ImageFormat : 0);
#endif

    if (drive[dnr].drive_floppy == NULL
           || (drive[dnr].drive_floppy->ImageFormat != 8050
		&& drive[dnr].drive_floppy->ImageFormat != 8250)
	) {
	return FDC_ERR_SYNC;
    }

    vdrive_bam_get_disk_id(drive[dnr].drive_floppy, disk_id);

    switch (job) {
    case 0x80:		/* read */
	if (header[0] != disk_id[0] || header[1] != disk_id[1]) {
	    rc = FDC_ERR_ID;
	    break;
	}
	rc = floppy_read_block(drive[dnr].drive_floppy->ActiveFd,
                           drive[dnr].drive_floppy->ImageFormat,
                           sector_data, track, sector,
                           drive[dnr].drive_floppy->D64_Header,
                           drive[dnr].drive_floppy->GCR_Header,
                           drive[dnr].drive_floppy->unit);
        if (rc < 0) {
            log_error(drive[dnr].log,
                  "Cannot read T:%d S:%d from disk image.",
                  track, sector);
            rc = FDC_ERR_DRIVE;
        } else {
	    memcpy(base, sector_data, 256);
	    rc = FDC_ERR_OK;
	}
	break;
    case 0x90:		/* write */
	if (header[0] != disk_id[0] || header[1] != disk_id[1]) {
	    rc = FDC_ERR_ID;
	    break;
	}
	if (drive[dnr].drive_floppy->ReadOnly) {
	    rc = FDC_ERR_WPROT;
	    break;
	}
	memcpy(sector_data, base, 256);
        rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
                           drive[dnr].drive_floppy->ImageFormat,
                           sector_data, track, sector,
                           drive[dnr].drive_floppy->D64_Header,
                           drive[dnr].drive_floppy->GCR_Header,
                           drive[dnr].drive_floppy->unit);
        if (rc < 0) {
            log_error(drive[dnr].log,
                  "Could not update T:%d S:%d on disk image.",
                  track, sector);
            rc = FDC_ERR_DRIVE;
        } else {
            rc = FDC_ERR_OK;
        }
        break;
    case 0xA0:		/* verify */
	if (header[0] != disk_id[0] || header[1] != disk_id[1]) {
	    rc = FDC_ERR_ID;
	    break;
	}
        rc = floppy_read_block(drive[dnr].drive_floppy->ActiveFd,
                           drive[dnr].drive_floppy->ImageFormat,
                           sector_data, track, sector,
                           drive[dnr].drive_floppy->D64_Header,
                           drive[dnr].drive_floppy->GCR_Header,
                           drive[dnr].drive_floppy->unit);
        if (rc < 0) {
            log_error(drive[dnr].log,
                  "Cannot read T:%d S:%d from disk image.",
                  track, sector);
            rc = FDC_ERR_DRIVE;
        } else {
            rc = FDC_ERR_OK;
            for (i = 0; i < 256; i++) {
                if (fnum) {
                    if (sector_data[i] != base[i]) {
			rc = FDC_ERR_VERIFY;
		    }
                } else {
                    if (sector_data[i] != base[i]) {
			rc = FDC_ERR_VERIFY;
		    }
                }
	    }
        }
        break;
    case 0xB0:		/* seek */
	header[0] = disk_id[0];
	header[1] = disk_id[1];
	header[2] = last_track;
	header[3] = 1;
	rc = FDC_ERR_OK;
	break;
    case 0xC0:		/* bump (to track 0) */
	rc = FDC_ERR_OK;
	break;
    case 0xD0:		/* jump to buffer :-( */
	rc = FDC_ERR_DRIVE;
	break;
    case 0xE0:		/* execute when drive/head ready */
	/* we have to check for standard format code that is copied 
	   to buffers 0-3 */
	if (!memcmp(iprom[fnum], &buffer[fnum][0x100], 0x300)) {
	    int ntracks, nsectors = 0;
	    /* detected format code */
#ifdef FDC_DEBUG
	    log_message(fdc_log, "format code: \n");
	    log_message(fdc_log, "     track for zones side 0: %d %d %d %d\n", 
		buffer[fnum][0xb0], buffer[fnum][0xb1],
		buffer[fnum][0xb2], buffer[fnum][0xb3]);
	    log_message(fdc_log, "     track for zones side 1: %d %d %d %d\n", 
		buffer[fnum][0xb4], buffer[fnum][0xb5],
		buffer[fnum][0xb6], buffer[fnum][0xb7]);
	    log_message(fdc_log, "     secs per track: %d %d %d %d\n",
		buffer[fnum][0x99], buffer[fnum][0x9a],
		buffer[fnum][0x9b], buffer[fnum][0x9c]);
	    log_message(fdc_log, "     vars: 870=%d 873=%d 875=%d\n",
		buffer[fnum][0x470], buffer[fnum][0x473],
		buffer[fnum][0x475]);
	    log_message(fdc_log, "     track=%d, sector=%d\n", 
		track, sector);
	    log_message(fdc_log, "     id=%02x,%02x (%c%c)\n", 
		header[0],header[1], header[0],header[1]);
	    log_message(fdc_log, "     sides=%d\n", 
		buffer[fnum][0xac]);
#endif
	    if (drive[dnr].drive_floppy->ReadOnly) {
	        rc = FDC_ERR_WPROT;
	        break;
	    }
	    ntracks = (buffer[fnum][0xac] > 1) ? 154 : 77;

	    memset(sector_data, 0, 256);

	    for (rc = 0, track = 1; rc == 0 && track <= ntracks; track ++) {
		if (track < 78) {
		    for (i=3; i >= 0; i--) {
			if (track < buffer[fnum][0xb0 + i]) {
			    nsectors = buffer[fnum][0x99 + i];
			    break;
			}
		    }
		} else {
		    for (i=3; i >= 0; i--) {
			if (track < buffer[fnum][0xb4 + i]) {
			    nsectors = buffer[fnum][0x99 + i];
			    break;
			}
		    }
		}
		for (sector = 0; sector < nsectors; sector ++) {
                    rc = floppy_write_block(drive[dnr].drive_floppy->ActiveFd,
                           drive[dnr].drive_floppy->ImageFormat,
                           sector_data, track, sector,
                           drive[dnr].drive_floppy->D64_Header,
                           drive[dnr].drive_floppy->GCR_Header,
                           drive[dnr].drive_floppy->unit);
                    if (rc < 0) {
                        log_error(drive[dnr].log,
                  		"Could not update T:%d S:%d on disk image.",
                  		track, sector);
            		rc = FDC_ERR_DCHECK;
			break;
		    }
		}
	    }

            vdrive_bam_set_disk_id(drive[dnr].drive_floppy, header);

	    if (!rc) 
	        rc = FDC_ERR_OK;
	} else {
	    rc = FDC_ERR_DRIVE;
	}
	break;
    case 0xF0:
	if (header[0] != disk_id[0] || header[1] != disk_id[1]) {
	    rc = FDC_ERR_ID;
	    break;
	}
	/* try to read block header from disk */
	rc = FDC_ERR_OK;
	break;
    }

    drive[dnr].current_half_track = 2 * track;
    last_track = track;

    return (BYTE) rc;
}

static int int_fdc(int fnum, long offset)
{
    CLOCK rclk = drive_clk[fnum] - offset;
    int i, j;

#ifdef FDC_DEBUG
    if (fdc_state[fnum] < FDC_RUN) {
	static int old_state[NUM_FDC] = { -1, -1 };
	if (fdc_state[fnum] != old_state[fnum])
	    log_message(fdc_log, "int_fdc%d %d: state=%d\n",
					fnum, rclk, fdc_state[fnum]);
	old_state[fnum] = fdc_state[fnum];
    }
#endif

    switch(fdc_state[fnum]) {
    case FDC_RESET0:
	buffer[fnum][0] = 2;
	fdc_state[fnum]++;
	alarm_set(&fdc_alarm[fnum], rclk + 2000);
	break;
    case FDC_RESET1:
	if (buffer[fnum][0] == 0) {
	    buffer[fnum][0] = 1;
	    fdc_state[fnum]++;
	}
	alarm_set(&fdc_alarm[fnum], rclk + 2000);
	break;
    case FDC_RESET2:
	if (buffer[fnum][0] == 0) {
	    /* emulate routine written to buffer RAM */
	    buffer[fnum][1] = 0x0e;
	    buffer[fnum][2] = 0x2d;
	    buffer[fnum][0xac] = 2;	/* number of sides on disk drive */
	    buffer[fnum][0xea] = 1;	/* 0 = 4040 (2A), 1 = 8x80 (2C) drive type */
	    buffer[fnum][0xee] = 5;	/* 3 for 4040, 5 for 8x50 */
	    buffer[fnum][0] = 3;	/* 5 for 4040, 3 for 8x50 */
	    fdc_state[fnum] = FDC_RUN;
	    alarm_set(&fdc_alarm[fnum], rclk + 10000);
	} else {
	    alarm_set(&fdc_alarm[fnum], rclk + 2000);
	}
	break;
    case FDC_RUN:
	/* check buffers */
	for (i=14; i >= 0; i--) {
	    /* job there? */
	    if (buffer[fnum][i + 3] > 127) {
		/* pointer to buffer/block header:
		    +0 = ID1
		    +1 = ID2
		    +2 = Track
		    +3 = Sector
		*/
		j = (i << 3) + 0x21;
#ifdef FDC_DEBUG
		log_message(fdc_log, "D/Buf %d/%x: Job code %02x t:%02d s:%02d",
			fnum, i, buffer[fnum][i+3],
			buffer[fnum][j+2],buffer[fnum][j+3]);
#endif
		buffer[fnum][i + 3] = 
			fdc_do_job(fnum, 			/* FDC# */
				i,				/* buffer# */
				buffer[fnum][i+3] & 1,		/* drive */
				buffer[fnum][i+3] & 0xfe,	/* job code */
				&(buffer[fnum][j]) 		/* header */
			);
	    }
	}
	/* check "move head", by half tracks I guess... */
	for (i = 0; i < 2; i++) {
	    if (buffer[fnum][i + 0xa1]) {
#ifdef FDC_DEBUG
		log_message(fdc_log, "D %d: move head %d",
		    fnum, buffer[fnum][i + 0xa1]);
#endif
		buffer[fnum][i + 0xa1] = 0;
	    }
	}
	alarm_set(&fdc_alarm[fnum], rclk + 30000);
	/* job loop */
	break;
    }

    return 0;
}

static void clk_overflow_callback(int fnum, CLOCK sub, void *data)
{
}

void fdc_init(int fnum, BYTE *buffermem, BYTE *ipromp)
{
    buffer[fnum] = buffermem;
    iprom[fnum] = ipromp;

    if (fdc_log == LOG_ERR)
        fdc_log = log_open("fdc");

    if (fnum == 0) {
        alarm_init(&fdc_alarm[fnum], &drive0_alarm_context,
               "fdc0", int_fdc0);
        clk_guard_add_callback(&drive0_clk_guard, clk_overflow_callback0, NULL);
    } else
    if (fnum == 1) {
        alarm_init(&fdc_alarm[fnum], &drive1_alarm_context,
               "fdc1", int_fdc1);
        clk_guard_add_callback(&drive1_clk_guard, clk_overflow_callback1, NULL);
    }
}


