/*
 * jobcode1581.c - 1581 job code handling.
 *
 * Written by
 *  Andreas Boose <viceteam@t-online.de>
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

#include "diskimage.h"
#include "drive.h"
#include "drivecpu.h"
#include "log.h"
#include "types.h"
#include "wd1770.h"


/* #define JOBCODE1581_DEBUG */

#define READ_DV  0x80
#define WRTSD_DV 0x90
#define TREAD_DV 0xaa


static unsigned int track_cache_track[2];
static unsigned int track_cache_sector[2];
static unsigned int track_cache_valid[2];

static log_t jobcode1581_log = LOG_DEFAULT;


static int track_cache_read(unsigned int dnr, unsigned int track,
                            unsigned int sector)
{
    WORD base;
    int rc;
    unsigned int i, rsec;
    BYTE sector_data[256];

    if (!track_cache_valid[dnr] || track_cache_track[dnr] != track) {
        for (rsec = 0; rsec < 20; rsec++) {
            rc = disk_image_read_sector(wd1770[dnr].image, sector_data,
                                        track, (rsec + sector) % 20);
            if (rc < 0) {
                log_error(jobcode1581_log,
                          "Cannot read T:%d S:%d from disk image.",
                          track, rsec);
                return 2;
            }
            base = (WORD)((rsec << 8) + 0xc00);
            for (i = 0; i < 256; i++) {
                if (dnr == 0)
                    drive_store(&drive0_context, (WORD)(base + i),
                                sector_data[i]);
                else
                    drive_store(&drive1_context, (WORD)(base + i),
                                sector_data[i]);
            }
        }

        track_cache_valid[dnr] = 1;
        track_cache_track[dnr] = track;
        track_cache_sector[dnr] = sector;
    }

    return 0;
}

static int wd1770_job_code_tread(unsigned int dnr, unsigned int track,
                                 unsigned int sector)
{
    return track_cache_read(dnr, track, sector);
}

static int wd1770_job_code_read(unsigned int dnr, unsigned int track,
                                unsigned int sector, unsigned int buffer)
{
    unsigned int i;
#if 0
    WORD base_src, base_trg;
    int rc;
    BYTE data;

    rc = track_cache_read(dnr, track, sector);

    if (rc != 0)
        return rc;

    base_src = (WORD)((((sector + track_cache_sector[dnr]) % 20) << 8) + 0xc00);
    base_trg = (WORD)((buffer << 8) + 0x300);

    for (i = 0; i < 256; i++) {
        if (dnr == 0) {
            data = drive_read(&drive0_context, (WORD)(base_src + i));
            drive_store(&drive0_context, (WORD)(base_trg + i), data);
        } else {
            data = drive_read(&drive1_context, (WORD)(base_src + i));
            drive_store(&drive1_context, (WORD)(base_trg + i), data);
        }
    }
#else
    WORD base;
    BYTE sector_data[256];

    disk_image_read_sector(wd1770[dnr].image, sector_data, track, sector);

    base = (WORD)((buffer << 8) + 0x300);

    for (i = 0; i < 256; i++) {
        if (dnr == 0) {
            drive_store(&drive0_context, (WORD)(base + i), sector_data[i]);
        } else {
            drive_store(&drive1_context, (WORD)(base + i), sector_data[i]);
        }
    }
#endif

    return 0;
}

static int wd1770_job_code_wrtsd(unsigned int dnr, unsigned int track,
                                 unsigned int sector, unsigned int buffer)
{
    WORD base;
    int rc, i;
    BYTE sector_data[256];

    base = (WORD)((buffer << 8) + 0x300);

    for (i = 0; i < 256; i++) {
        if (dnr == 0)
            sector_data[i] = drive_read(&drive0_context, (WORD)(base + i));
        else
            sector_data[i] = drive_read(&drive1_context, (WORD)(base + i));
    }

    rc = disk_image_write_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(jobcode1581_log,
                  "Could not update T:%d S:%d on disk image.",
                  track, sector);
        return 2;
    }
    return 0;
}

void jobcode1581_handle_job_code(unsigned int dnr)
{
    unsigned int buffer;
    BYTE command, track, sector;
    BYTE rcode = 0;

    for (buffer = 0; buffer <= 8; buffer++) {
        if (dnr == 0) {
            command = drive_read(&drive0_context, (WORD)(0x02 + buffer));
            track = drive_read(&drive0_context, (WORD)(0x0b + (buffer << 1)));
            sector = drive_read(&drive0_context, (WORD)(0x0c + (buffer << 1)));
        } else {
            command = drive_read(&drive1_context, (WORD)(0x02 + buffer));
            track = drive_read(&drive1_context, (WORD)(0x0b + (buffer << 1)));
            sector = drive_read(&drive1_context, (WORD)(0x0c + (buffer << 1)));
        }
        if (command & 0x80) {
#ifdef JOBCODE1581_DEBUG
            log_debug("WD1770 B:%i C:%x T:%i S:%i",
                      buffer, command, track, sector);
#endif
            if (wd1770[dnr].image != NULL
                && wd1770[dnr].image->type == DISK_IMAGE_TYPE_D81) {
                drive[dnr].current_half_track = track * 2;
                switch (command) {
                  case READ_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = wd1770_job_code_read(dnr, track, sector, buffer);
                    break;
                  case WRTSD_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = wd1770_job_code_wrtsd(dnr, track, sector, buffer);
                    break;
                  case TREAD_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = wd1770_job_code_tread(dnr, track, sector);
                  break;
                  default:
                    rcode = 0;
                }
            } else
                rcode = 2;

            if (dnr == 0)
                drive_store(&drive0_context, (WORD)(2 + buffer), rcode);
            else
                drive_store(&drive1_context, (WORD)(2 + buffer), rcode);
        }
    }
}

void jobcode1581_init(void)
{
    jobcode1581_log = log_open("JOBCODE1581");
    track_cache_valid[0] = 0;
    track_cache_valid[1] = 0;
}

