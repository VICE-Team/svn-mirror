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
#include "drivetypes.h"
#include "jobcode1581.h"
#include "log.h"
#include "types.h"
#include "wd1770.h"


/*#define JOBCODE1581_DEBUG*/

#define READ_DV     0x80
#define RESET_DV    0x82
#define MOTOFF_DV   0x86
#define WRTSD_DV    0x90
#define DISKIN_DV   0x92
#define SIDE_DV     0x9c
#define TRKWRT_DV   0xa2
#define TREAD_DV    0xaa
#define TWRT_DV     0xac
#define SEEKHD_DV   0xb0
#define DETWP_DV    0xb6
#define RESTORE_DV  0xc0
#define FORMATDK_DV 0xf0

#define OK_DV          0x00
#define MISHD_DV_ER    0x02
#define WRTPR_DV_ER    0x08
#define NODSKPRS_DV_ER 0x0f

#define OFFSET_SECPOS 0x9f
#define OFFSET_SIDS   0x1ce
#define OFFSET_BUFFER 0x300
#define OFFSET_TCACHE 0xc00

#define NUM_PHYS_TRACK  80
#define NUM_PHYS_SECTOR 10

static unsigned int track_cache_track[DRIVE_NUM];
static unsigned int track_cache_sector[DRIVE_NUM];
static unsigned int track_cache_valid[DRIVE_NUM];

static log_t jobcode1581_log = LOG_DEFAULT;


static void conv_phys_to_log(unsigned int dnr, unsigned int *track,
                             unsigned int *sector, unsigned int buffer)
{
    unsigned int sid;

    *track += 1;
    *sector = (*sector - 1) * 2;

    sid = drive_read(drive_context[dnr], (WORD)(OFFSET_SIDS + buffer));

    if (sid > 0)
        *sector += 20;
}

static void sector_to_mem(unsigned int dnr, BYTE *sector_data, WORD dst_base)
{
    unsigned int i;

    for (i = 0; i < 256; i++)
        drive_store(drive_context[dnr], (WORD)(dst_base + i), sector_data[i]);
}

static void mem_to_sector(unsigned int dnr, BYTE *sector_data, WORD src_base)
{
    unsigned int i;

    for (i = 0; i < 256; i++)
        sector_data[i] = drive_read(drive_context[dnr], (WORD)(src_base + i));
}

#if 0
static void mem_to_mem(unsigned int dnr, WORD src_base, WORD dst_base)
{
    unsigned int i;
    BYTE data;

    for (i = 0; i < 256; i++) {
        data = drive_read(drive_context[dnr], (WORD)(src_base + i));
        drive_store(drive_context[dnr], (WORD)(dst_base + i), data);
    }
}
#endif

static int sector_write(unsigned int dnr, unsigned int track,
                        unsigned int sector, WORD base)
{
    int rc;
    BYTE sector_data[256];

    mem_to_sector(dnr, sector_data, base);

    rc = disk_image_write_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(jobcode1581_log,
                  "Could not update T:%d S:%d on disk image.",
                  track, sector);
        return MISHD_DV_ER;
    }

    return OK_DV;
}

static int track_cache_read(unsigned int dnr, unsigned int track,
                            unsigned int sector, unsigned int buffer)
{
    int rc;
    unsigned int rsec, readsec, sideoffset = 0;
    BYTE sector_data[256], pos;
    WORD base;

    if (track_cache_sector[dnr] >= 20 && sector < 20)
        track_cache_valid[dnr] = 0;

    if (track_cache_sector[dnr] < 20 && sector >= 20)
        track_cache_valid[dnr] = 0;

    if (sector >= 20)
        sideoffset = 20;

    if (!track_cache_valid[dnr] || track_cache_track[dnr] != track) {
        for (rsec = 0; rsec < 20; rsec++) {
            readsec = ((sector + rsec) % 20) + sideoffset;
            rc = disk_image_read_sector(wd1770[dnr].image, sector_data, track,
                                        readsec);
            if (rc < 0) {
                log_error(jobcode1581_log,
                          "Cannot read T:%d S:%d from disk image.",
                          track, readsec);
                return MISHD_DV_ER;
            }

            base = (WORD)((rsec << 8) + OFFSET_TCACHE);
            sector_to_mem(dnr, sector_data, base);
        }

        track_cache_valid[dnr] = 1;
        track_cache_track[dnr] = track;
        track_cache_sector[dnr] = sector;
    }

    pos = ((sector + 40 - track_cache_sector[dnr]) % 20);

    drive_store(drive_context[dnr], (WORD)(OFFSET_SECPOS + buffer), pos);

    return OK_DV;
}

static int track_cache_write(unsigned int dnr, unsigned int track,
                             unsigned int sector, unsigned int buffer)
{
    WORD base;

    base = (WORD)((((track_cache_sector[dnr] + sector) % 20) << 8)
           + OFFSET_TCACHE);

    return sector_write(dnr, track, sector, base);
}

static int jobcode_read(unsigned int dnr, unsigned int track,
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

    base_src = (WORD)((((sector + track_cache_sector[dnr]) % 20) << 8)
               + OFFSET_TCACHE);
    base_trg = (WORD)((buffer << 8) + OFFSET_BUFFER);

    for (i = 0; i < 256; i++) {
        data = drive_read(drive_context[dnr], (WORD)(base_src + i));
        drive_store(drive_context[dnr], (WORD)(base_trg + i), data);
    }
#else
    WORD base;
    BYTE sector_data[256];

    disk_image_read_sector(wd1770[dnr].image, sector_data, track, sector);

    base = (WORD)((buffer << 8) + OFFSET_BUFFER);

    for (i = 0; i < 256; i++)
        drive_store(drive_context[dnr], (WORD)(base + i), sector_data[i]);
#endif

    return OK_DV;
}

static int jobcode_reset(unsigned int dnr)
{
    return OK_DV;
}

static int jobcode_motoff(unsigned int dnr)
{
    return OK_DV;
}

static int jobcode_wrtsd(unsigned int dnr, unsigned int track,
                         unsigned int sector, unsigned int buffer)
{
    WORD base;

    base = (WORD)((buffer << 8) + OFFSET_BUFFER);

    return sector_write(dnr, track, sector, base);
}

static int jobcode_diskin(unsigned int dnr)
{
    if (wd1770[dnr].image == NULL
        || wd1770[dnr].image->type != DISK_IMAGE_TYPE_D81)
        return NODSKPRS_DV_ER;

    return OK_DV;
}

static int jobcode_side(unsigned int dnr)
{
    return OK_DV;
}

static int jobcode_trkwrt(unsigned int dnr)
{
    /* To be implemented. */
    return OK_DV;
}

static int jobcode_tread(unsigned int dnr, unsigned int track,
                         unsigned int sector, unsigned int buffer)
{
    return track_cache_read(dnr, track, sector, buffer);
}

static int jobcode_twrite(unsigned int dnr, unsigned int track,
                          unsigned int sector, unsigned int buffer)
{
    return track_cache_write(dnr, track, sector, buffer);
}

static int jobcode_seekhd(unsigned int dnr)
{
    return OK_DV;
}

int jobcode_tpread(unsigned int dnr, unsigned int track,
                   unsigned int sector, unsigned int buffer)
{
    conv_phys_to_log(dnr, &track, &sector, buffer);

    return track_cache_read(dnr, track, sector, buffer);
}

int jobcode_tpwrite(unsigned int dnr, unsigned int track,
                    unsigned int sector, unsigned int buffer)
{
    WORD base;
    int rc;
    unsigned int psec;

    psec = sector;

    if (psec == 0 || psec > NUM_PHYS_SECTOR)
        return MISHD_DV_ER;

    if (track >= NUM_PHYS_TRACK)
        return MISHD_DV_ER;

    conv_phys_to_log(dnr, &track, &sector, buffer);

    base = (WORD)(((psec - 1) << 9) + OFFSET_TCACHE);

    rc = sector_write(dnr, track, sector, base);

    if (rc != OK_DV)
        return rc;

    rc = sector_write(dnr, track, sector + 1, (WORD)(base + 0x100));

    if (rc != OK_DV)
        return rc;

    return OK_DV;
}

static int jobcode_detwp(unsigned int dnr)
{
    if (wd1770[dnr].image != NULL) {
        if (wd1770[dnr].image->read_only)
            return WRTPR_DV_ER;
        else
            return OK_DV;
    }

    return OK_DV;
}

static int jobcode_restore(unsigned int dnr)
{
    return OK_DV;
}

static int jobcode_format(unsigned int dnr)
{
    return OK_DV;
}

void jobcode1581_handle_job_code(drive_context_t *drv)
{
    unsigned int buffer;
    BYTE command, track, sector;
    BYTE rcode = 0;
    drive_t *drive;
    unsigned int dnr;

    drive = drv->drive;
    dnr = drive->mynumber;

    for (buffer = 0; buffer <= 8; buffer++) {
        command = drive_read(drv, (WORD)(0x02 + buffer));
        track = drive_read(drv, (WORD)(0x0b + (buffer << 1)));
        sector = drive_read(drv, (WORD)(0x0c + (buffer << 1)));

        if (command & 0x80) {
#ifdef JOBCODE1581_DEBUG
            log_debug("JOBCODE1581 C:%x T:%i S:%i B:%i",
                      command, track, sector, buffer);
#endif
            if ((wd1770[dnr].image != NULL
                && wd1770[dnr].image->type == DISK_IMAGE_TYPE_D81)
                /*|| command == DISKIN_DV*/) {
                drive->current_half_track = track * 2;
                switch (command) {
                  case READ_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_read(dnr, track, sector, buffer);
                    break;
                  case RESET_DV:
                    wd1770[dnr].led_delay_clk = (CLOCK)0;
                    rcode = jobcode_reset(dnr);
                    break;
                  case MOTOFF_DV:
                    rcode = jobcode_motoff(dnr);
                    break;
                  case WRTSD_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_wrtsd(dnr, track, sector, buffer);
                    break;
                  case DISKIN_DV:
                    rcode = jobcode_diskin(dnr);
                    break;
                  case SIDE_DV:
                    rcode = jobcode_side(dnr);
                    break;
                  case TRKWRT_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_trkwrt(dnr);
                    break;
                  case TREAD_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_tread(dnr, track, sector, buffer);
                    break;
                  case TWRT_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_twrite(dnr, track, sector, buffer);
                    break;
                  case SEEKHD_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_seekhd(dnr);
                    break;
                  case DETWP_DV:
                    rcode = jobcode_detwp(dnr);
                    break;
                  case RESTORE_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_restore(dnr);
                    break;
                  case FORMATDK_DV:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = jobcode_format(dnr);
                    break;
                  default:
#ifdef JOBCODE1581_DEBUG
                    log_debug("JOBCODE1581 Unknown job code %02x", command);
#endif
                    rcode = OK_DV;
                }
            } else
                rcode = MISHD_DV_ER;

            drive_store(drv, (WORD)(2 + buffer), rcode);
        }
    }
}

void jobcode1581_init(void)
{
    jobcode1581_log = log_open("JOBCODE1581");
    track_cache_valid[0] = 0;
    track_cache_valid[1] = 0;
}

