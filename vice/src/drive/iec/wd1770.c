/*
 * wd1770.c - WD1770 emulation for the 1571 and 1581 disk drives.
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

/*#define WD_DEBUG*/

#include "vice.h"

#include <stdio.h>
#include <string.h>

#include "clkguard.h"
#include "diskimage.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "log.h"
#include "types.h"
#include "wd1770.h"

/*-----------------------------------------------------------------------*/

/* WD1770 register.  */
#define WD1770_STATUS  0
#define WD1770_COMMAND 0
#define WD1770_TRACK   1
#define WD1770_SECTOR  2
#define WD1770_DATA    3

/* WD1770 status register.  */
#define WD_STAT_MOTOR_ON      0x80
#define WD_STAT_WRITE_PROTECT 0x40
#define WD_STAT_MOTOR_SPINUP  0x20
#define WD_STAT_SEEK_ERROR    0x10
#define WD_STAT_CRC_ERROR     0x08
#define WD_STAT_TRACK0        0x04
#define WD_STAT_INDEX_PULSE   0x02
#define WD_STAT_DATA_REQUEST  0x02
#define WD_STAT_BUSY          0x01

/*-----------------------------------------------------------------------*/

static void wd1770_store(WORD addr, BYTE byte, unsigned int dnr);
static BYTE wd1770_read(WORD addr, unsigned int dnr);
static void wd1770_reset(int unsigned dnr);

static void wd1770_command_restore(BYTE command, unsigned int dnr);
static void wd1770_command_seek(BYTE command, unsigned int dnr);
static void wd1770_command_step(BYTE command, unsigned int dnr);
static void wd1770_command_stepin(BYTE command, unsigned int dnr);
static void wd1770_command_stepout(BYTE command, unsigned int dnr);
static void wd1770_command_readsector(BYTE command, unsigned int dnr);
static void wd1770_command_writesector(BYTE command, unsigned int dnr);
static void wd1770_command_readaddress(BYTE command, unsigned int dnr);
static void wd1770_command_forceint(BYTE command, unsigned int dnr);
static void wd1770_command_readtrack(BYTE command, unsigned int dnr);
static void wd1770_command_writetrack(BYTE command, unsigned int dnr);

static void wd1770_write_buffer(unsigned int dnr);

/* wd1770 disk controller structure.  */
wd1770_t wd1770[DRIVE_NUM];

static log_t wd1770_log = LOG_ERR;

/*-----------------------------------------------------------------------*/
/* WD1770 external interface.  */

/* Clock overflow handling.  */
static void clk_overflow_callback(CLOCK sub, void *data)
{
    unsigned int dnr;

    dnr = vice_ptr_to_uint(data);

    if (wd1770[dnr].busy_clk > (CLOCK) 0)
        wd1770[dnr].busy_clk -= sub;
    if (wd1770[dnr].motor_spinup_clk > (CLOCK) 0)
        wd1770[dnr].motor_spinup_clk -= sub;
    if (wd1770[dnr].led_delay_clk > (CLOCK) 0)
        wd1770[dnr].led_delay_clk -= sub;
    if (wd1770[dnr].set_drq > (CLOCK) 0)
        wd1770[dnr].set_drq -= sub;
    if (wd1770[dnr].attach_clk > (CLOCK) 0)
        wd1770[dnr].attach_clk -= sub;
}

/* Functions using drive context.  */
void wd1770d_init(drive_context_t *drv)
{
    if (wd1770_log == LOG_ERR)
        wd1770_log = log_open("WD1770");

    clk_guard_add_callback(drv->cpu->clk_guard, clk_overflow_callback,
                           uint_to_void_ptr(drv->mynumber));
}

void wd1770d_store(drive_context_t *drv, WORD addr, BYTE byte)
{
    wd1770_store((WORD)(addr & 3), byte, drv->mynumber);
}

BYTE wd1770d_read(drive_context_t *drv, WORD addr)
{
    return wd1770_read((WORD)(addr & 3), drv->mynumber);
}

void wd1770d_reset(drive_context_t *drv)
{
    wd1770_reset(drv->mynumber);
}


/*-----------------------------------------------------------------------*/
/* WD1770 register read/write access.  */

static void wd1770_store(WORD addr, BYTE byte, unsigned int dnr)
{
#if 0
    log_debug("WD WRITE ADDR: %i DATA:%02x CLK:%i", addr, byte, drive_clk[dnr]);
#endif

    switch (addr) {
      case 0:
        switch (byte >> 5) {
          case 0:
            if (byte & 0x10)
                wd1770_command_seek(byte, dnr);
            else
                wd1770_command_restore(byte, dnr);
            break;
          case 1:
            wd1770_command_step(byte, dnr);
            break;
          case 2:
            wd1770_command_stepin(byte, dnr);
            break;
          case 3:
            wd1770_command_stepout(byte, dnr);
            break;
          case 4:
            wd1770_command_readsector(byte, dnr);
            break;
          case 5:
            wd1770_command_writesector(byte, dnr);
            break;
          case 6:
            if (byte & 0x10)
                wd1770_command_forceint(byte, dnr);
            else
                wd1770_command_readaddress(byte, dnr);
            break;
          case 7:
            if (byte & 0x10)
                wd1770_command_writetrack(byte, dnr);
            else
                wd1770_command_readtrack(byte, dnr);
            break;
        }
        break;
      case 1:
      case 2:
        wd1770[dnr].reg[addr] = byte;
        break;
      case 3:
        if (wd1770[dnr].write_pending) {
            wd1770[dnr].data_buffer[wd1770[dnr].data_buffer_offset] = byte;
            wd1770[dnr].data_buffer_index--;
            wd1770[dnr].data_buffer_offset++;
#if 0
            log_debug("WP%02x DATA %02x IDX%03i OFF%03i",
                      wd1770[dnr].write_pending, byte,
                      wd1770[dnr].data_buffer_index,
                      wd1770[dnr].data_buffer_offset);
#endif
            if (wd1770[dnr].data_buffer_index < 0) {
                wd1770_write_buffer(dnr);
                wd1770[dnr].data_buffer_index = -1;
                wd1770[dnr].data_buffer_offset = 0;
            }
        }
        wd1770[dnr].reg[addr] = byte;
        break;
    }
}

static void check_status(unsigned int dnr)
{
    if (wd1770[dnr].busy_clk != (CLOCK)0) {
        if (drive_clk[dnr] - wd1770[dnr].busy_clk < 100) {
            wd1770[dnr].busy = 1;
        } else {
            wd1770[dnr].busy = 0;
            wd1770[dnr].busy_clk = (CLOCK)0;
        }
    } else {
        wd1770[dnr].busy = 0;
    }

    if (wd1770[dnr].motor_spinup_clk != (CLOCK)0) {
        if (drive_clk[dnr] - wd1770[dnr].motor_spinup_clk > 50) {
            wd1770[dnr].motor_ready = 1;
            wd1770[dnr].motor_spinup_clk = (CLOCK)0;
        }
    }
}

static BYTE read_status_type1(unsigned int dnr)
{
    BYTE tmp;

    tmp = 0;

    check_status(dnr);

    /* 0 Busy */
    if (wd1770[dnr].busy)
        tmp |= WD_STAT_BUSY;

    /* 1 Index */
    if (wd1770[1].index_count == 0)
        tmp |= WD_STAT_INDEX_PULSE;

    /* 2 Track zero */
    if (wd1770[dnr].track > 0)
        tmp |= WD_STAT_TRACK0;

    /* 3 CRC Error */

    /* 4 Record not found */
    if (wd1770[dnr].record_not_found)
        tmp |= WD_STAT_SEEK_ERROR;

    /* 5 Spin-up */
    if (wd1770[dnr].motor_ready)
        tmp |= WD_STAT_MOTOR_SPINUP;

    /* 6 Write protect */
    if (wd1770[dnr].wp_status)
        tmp |= WD_STAT_WRITE_PROTECT;

    /* 7 Motor on */
    if (wd1770[dnr].motor)
        tmp |= WD_STAT_MOTOR_ON;

    return tmp;
}

static BYTE read_status_type2(unsigned int dnr)
{
    BYTE tmp;

    tmp = 0;

    check_status(dnr);

    /* 0 Busy */
    if (wd1770[dnr].busy || wd1770[dnr].data_buffer_index >= 0)
        tmp |= WD_STAT_BUSY;

    /* 1 Data request */
    if (wd1770[dnr].data_buffer_index >= 0)
        tmp |= WD_STAT_DATA_REQUEST;

    /* 2 Lost data */

    /* 3 CRC Error */

    /* 4 Record not found */
    if (wd1770[dnr].record_not_found)
        tmp |= WD_STAT_SEEK_ERROR;

    /* 5 Data type */

    /* 6 Write protect */
    if (wd1770[dnr].wp_status)
        tmp |= WD_STAT_WRITE_PROTECT;

    /* 7 Motor on */
    if (wd1770[dnr].motor)
        tmp |= WD_STAT_MOTOR_ON;

    return tmp;
}

static BYTE read_status_type3(unsigned int dnr)
{
    return read_status_type2(dnr);
}

static BYTE wd1770_read(WORD addr, unsigned int dnr)
{
    BYTE tmp = 0;

    switch (addr) {
      case WD1770_STATUS:
        switch (wd1770[dnr].type) {
          case 1:
            tmp = read_status_type1(dnr);
            break;
          case 2:
            tmp = read_status_type2(dnr);
            break;
          case 3:
            tmp = read_status_type3(dnr);
            break;
        }
        wd1770[dnr].reg[WD1770_STATUS] = tmp;
        break;
      case 1:
      case 2:
        tmp = wd1770[dnr].reg[addr];
        break;
      case 3:
        if (wd1770[dnr].data_buffer_index < 0) {
            tmp = wd1770[dnr].reg[addr];
        } else {
            tmp = wd1770[dnr].data_buffer[wd1770[dnr].data_buffer_offset];
            wd1770[dnr].data_buffer_index--;
            wd1770[dnr].data_buffer_offset++;
        }
        wd1770[dnr].reg[addr] = tmp;
        break;
    }
#if 0
    log_debug("WD READ ADDR: %i DATA:%02x CLK:%i", addr, tmp, drive_clk[dnr]);
#endif
    return tmp;
}

static void wd1770_clear_errors(unsigned int dnr)
{
    wd1770[dnr].record_not_found = 0;
}

static void wd1770_reset(unsigned int dnr)
{
    int i;

    wd1770[dnr].busy_clk = (CLOCK)0;
    wd1770[dnr].motor_spinup_clk = (CLOCK)0;
    wd1770[dnr].led_delay_clk = (CLOCK)0;
    wd1770[dnr].set_drq = (CLOCK)0;
    wd1770[dnr].track = 20;
    wd1770[dnr].data_buffer_index = -1;
    wd1770[dnr].data_buffer_offset = 0;
    wd1770[dnr].wp_status = 0;
    wd1770[dnr].type = 1;
    wd1770[dnr].motor = 0;
    wd1770[dnr].motor_ready = 0;
    wd1770[dnr].index_count = 0;
    wd1770[dnr].write_pending = 0;
    wd1770_clear_errors(dnr);

    for (i = 0; i < 4; i++)
        /* FIXME: Just a wild guess.  */
        wd1770[dnr].reg[i] = 0;

}

/*-----------------------------------------------------------------------*/
/* WD1770 commands, common routines.  No need to inline these functions, as
   the real WD1770 needs ages to execute commands anyway.  */

static void wd1770_update_track_register(BYTE command, int dnr)
{
    drive_t *drive;

    drive = drive_context[dnr]->drive;

    if (command & 0x10)
        wd1770[dnr].reg[WD1770_TRACK] = wd1770[dnr].track;

    drive->current_half_track = (wd1770[dnr].track + 1) * 2;
}

static void wd1770_motor_control(BYTE command, unsigned int dnr)
{
    if (command & 0x08) {
        if (wd1770[dnr].motor == 0) {
            wd1770[dnr].motor = 1;
            wd1770[dnr].motor_ready = 0;
            wd1770[dnr].motor_spinup_clk = drive_clk[dnr];
        }
    } else {
        wd1770[dnr].motor = 0;
        wd1770[dnr].motor_ready = 0;
        wd1770[dnr].motor_spinup_clk = (CLOCK)0;
    }
}

static void wd1770_wpstatus_set(unsigned int dnr)
{
    if (wd1770[dnr].image != NULL)
        wd1770[dnr].wp_status = wd1770[dnr].image->read_only;
    else
        wd1770[dnr].wp_status = 0;
}

static void wd1770_conv_phy2log(unsigned int dnr, unsigned int *track,
                                unsigned int *sector)
{
    *track += 1;
    *sector = (*sector - 1) * 2 + (wd1770[dnr].side ? 20 : 0);
}

static void wd1770_buffer_put(unsigned int dnr, BYTE *data, unsigned int count)
{
    memcpy(&(wd1770[dnr].data_buffer[wd1770[dnr].data_buffer_index + 1]),
           data, count);

    wd1770[dnr].data_buffer_index += count;
}

static void wd1770_buffer_get(unsigned int dnr, BYTE *data, unsigned int count)
{
    memcpy(data, &(wd1770[dnr].data_buffer[wd1770[dnr].data_buffer_index + 1]),
          count);

    wd1770[dnr].data_buffer_index += count;
}

static int wd1770_buffer_fb(unsigned int dnr)
{
    while (wd1770[dnr].data_buffer_index < WD1770_BUFFER_SIZE - 1) {
        if (wd1770[dnr].data_buffer[(wd1770[dnr].data_buffer_index++) + 1] 
            == 0xfb)
            return 0;
    }

    return -1;
}

static int wd1770_sector_read(unsigned int dnr, unsigned int track,
                              unsigned int sector)
{
    BYTE sector_data[256];
    int rc;

    wd1770_conv_phy2log(dnr, &track, &sector);

#ifdef WD_DEBUG
    log_debug("READ T%i S%i",track,sector);
#endif

    rc = disk_image_read_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(wd1770_log,
                  "Cannot read T:%d S:%d from disk image.", track, sector);
        return -1;
    }

    wd1770_buffer_put(dnr, sector_data, sizeof(sector_data));

    sector++;

#ifdef WD_DEBUG
    log_debug("READ T%i S%i",track,sector);
#endif

    rc = disk_image_read_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(wd1770_log,
                  "Cannot read T:%d S:%d from disk image.", track, sector);
        return -1;
    }

    wd1770_buffer_put(dnr, sector_data, sizeof(sector_data));

    return 0;
}

static int wd1770_sector_write(unsigned int dnr, unsigned int track,
                               unsigned int sector)
{
    BYTE sector_data[256];
    int rc;

    wd1770_conv_phy2log(dnr, &track, &sector);

    wd1770_buffer_get(dnr, sector_data, sizeof(sector_data));

#ifdef WD_DEBUG
    log_debug("WRITE T%i S%i",track,sector);
#endif

    rc = disk_image_write_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(wd1770_log,
                  "Cannot write T:%d S:%d to disk image.", track, sector);
        return -1;
    }

    sector++;

    wd1770_buffer_get(dnr, sector_data, sizeof(sector_data));

#ifdef WD_DEBUG
    log_debug("WRITE T%i S%i",track,sector);
#endif

    rc = disk_image_write_sector(wd1770[dnr].image, sector_data, track, sector);
    if (rc < 0) {
        log_error(wd1770_log,
                  "Cannot write T:%d S:%d to disk image.", track, sector);
        return -1;
    }

    return 0;
}

static int wd1770_track_write(unsigned int dnr, unsigned int track)
{
    unsigned int sector;

    for (sector = 1; sector <= 10; sector++) {
        if (wd1770_buffer_fb(dnr) < 0)
            break;
        if (wd1770[dnr].data_buffer_index >= WD1770_BUFFER_SIZE - 512)
            break;
        wd1770_sector_write(dnr, track, sector);
    }

    return 0;
}

static void wd1770_write_buffer(unsigned int dnr)
{
    unsigned int start, end, sector;

    if ((wd1770[dnr].write_pending & 0xe0) == 0xa0) {
        start = wd1770[dnr].reg[WD1770_SECTOR];
        if (wd1770[dnr].write_pending & 0x10)
            end = 10;
        else
            end = start;

        for (sector = start; sector <= end; sector++)
            wd1770_sector_write(dnr, wd1770[dnr].reg[WD1770_TRACK], sector);

        wd1770[dnr].write_pending = 0;
    }
    if ((wd1770[dnr].write_pending & 0xf0) == 0xf0) {
        wd1770_track_write(dnr, wd1770[dnr].reg[WD1770_TRACK]);
        wd1770[dnr].write_pending = 0;
    }
}

/*-----------------------------------------------------------------------*/
/* WD1770 commands.  */

static void wd1770_command_restore(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:RESTORE");
#endif
    wd1770[dnr].type = 1;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    wd1770[dnr].track = 0;
    wd1770_update_track_register(0x10, dnr);

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_seek(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:SEEK");
#endif
    wd1770[dnr].type = 1;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    wd1770[dnr].track = wd1770[dnr].reg[WD1770_DATA];
    wd1770_update_track_register(0x10, dnr);

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_step(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:STEP");
#endif
    wd1770[dnr].type = 1;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    wd1770_update_track_register(command, dnr);

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_stepin(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:STEPIN");
#endif
    wd1770[dnr].type = 1;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    if (wd1770[dnr].track < 79)
        wd1770[dnr].track++;

    wd1770_update_track_register(command, dnr);

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_stepout(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:STEPOUT");
#endif
    wd1770[dnr].type = 1;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    if (wd1770[dnr].track > 0)
        wd1770[dnr].track--;

    wd1770_update_track_register(command, dnr);

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_readsector(BYTE command, unsigned int dnr)
{
    unsigned int start, end, sector;

#ifdef WD_DEBUG
    log_debug("C:READ SECTOR");
#endif
    wd1770[dnr].type = 2;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    start = wd1770[dnr].reg[WD1770_SECTOR];
    if (command & 0x10)
        end = 10;
    else
        end = start;

    wd1770[dnr].data_buffer_offset = 0;
    wd1770[dnr].data_buffer_index = -1;

    for (sector = start; sector <= end; sector++)
        wd1770_sector_read(dnr, wd1770[dnr].reg[WD1770_TRACK], sector);

    wd1770_motor_control(command, dnr);
    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_writesector(BYTE command, unsigned int dnr)
{
    unsigned int start, end;

#ifdef WD_DEBUG
    log_debug("C:WRITE SECTOR");
#endif
    wd1770[dnr].type = 2;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    start = wd1770[dnr].reg[WD1770_SECTOR];
    if (command & 0x10)
        end = 10;
    else
        end = start;

    wd1770[dnr].data_buffer_offset = 0;
    wd1770[dnr].data_buffer_index = (1 + end - start) * 512 - 1;

    wd1770[dnr].write_pending = (unsigned int)command;

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_readaddress(BYTE command, unsigned int dnr)
{
    drive_t *drive;

    drive = drive_context[dnr]->drive;

#ifdef WD_DEBUG
    log_debug("C:READ ADDRESS");
#endif
    wd1770[dnr].type = 3;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    if (drive->type == DRIVE_TYPE_1570
        || drive->type == DRIVE_TYPE_1571
        || drive->type == DRIVE_TYPE_1571CR) {
        /* 1571 MFM disk images are not supported.  */
        wd1770[dnr].record_not_found = 1;
    } else {
        WORD crc;
        int i, j;

        wd1770[dnr].data_buffer[0] = wd1770[dnr].track;
        wd1770[dnr].data_buffer[1] = 0x0;
        wd1770[dnr].data_buffer[2] = 0x1;
        wd1770[dnr].data_buffer[3] = 0x2;

        crc = 0xb230;
        for (i = 0; i < 4; i++) {
            crc ^= wd1770[dnr].data_buffer[i] << 8;
            for (j = 0; j < 8; j++) {
                if (crc & 0x8000) {
                    crc <<= 1;
                    crc ^= 0x1021;
                } else {
                    crc <<= 1;
                }
            }
        }

        wd1770[dnr].data_buffer[4] = crc >> 8;
        wd1770[dnr].data_buffer[5] = crc;
        wd1770[dnr].data_buffer_index = 5;
        wd1770[dnr].data_buffer_offset = 0;

        wd1770_motor_control(command, dnr);
    }

    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_forceint(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:FORCE INT");
#endif
    wd1770[dnr].type = 1;
    wd1770_clear_errors(dnr);
    wd1770[dnr].write_pending = 0;

    wd1770[dnr].data_buffer_offset = 0;
    wd1770[dnr].data_buffer_index = - 1;

    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_readtrack(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:READ TRACK");
#endif
    wd1770[dnr].type = 3;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_writetrack(BYTE command, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("C:WRITEyy TRACK");
#endif
    wd1770[dnr].type = 3;
    wd1770[dnr].busy_clk = drive_clk[dnr];
    wd1770_clear_errors(dnr);

    wd1770[dnr].data_buffer_offset = 0;
    wd1770[dnr].data_buffer_index = WD1770_BUFFER_SIZE - 1;

    wd1770[dnr].write_pending = (unsigned int)command;

    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

/*-----------------------------------------------------------------------*/

void wd1770_vsync_hook(void)
{
    unsigned int dnr;
    drive_t *drive;

    for (dnr = 0; dnr < DRIVE_NUM; dnr++) {
        drive = drive_context[dnr]->drive;

        if (drive->type == DRIVE_TYPE_1581) {
            if (wd1770[dnr].led_delay_clk != (CLOCK)0)
                if (drive_clk[dnr] - wd1770[dnr].led_delay_clk > 1000000)
                    wd1770[dnr].led_delay_clk = (CLOCK)0;
/*drive->led_status = (wd1770[dnr].led_delay_clk == (CLOCK)0) ? 0 : 1;*/
            wd1770[dnr].index_count++;
            if (wd1770[dnr].index_count > 10)
                wd1770[dnr].index_count = 0;
        }
    }
}

int wd1770_attach_image(disk_image_t *image, unsigned int unit)
{
    if (unit < 8 || unit > 8 + DRIVE_NUM)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_attach_log(image, wd1770_log, unit);
        break;
      default:
        return -1;
    }

    wd1770[unit - 8].attach_clk = drive_clk[unit - 8];
    wd1770[unit - 8].image = image;
    return 0;
}

int wd1770_detach_image(disk_image_t *image, unsigned int unit)
{
    if (image == NULL || unit < 8 || unit > 8 + DRIVE_NUM)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_detach_log(image, wd1770_log, unit);
        break;
      default:
        return -1;
    }

    wd1770[unit - 8].image = NULL;
    return 0;
}

int wd1770_disk_change(drive_context_t *drive_context)
{
    unsigned int dnr;

    dnr = drive_context->mynumber;

    if (wd1770[dnr].image == NULL)
        return 1;

    if (wd1770[dnr].attach_clk != (CLOCK)0) {
        if (*(drive_context->clk_ptr) - wd1770[dnr].attach_clk
            < DRIVE_ATTACH_DELAY)
            return 1;
        wd1770[dnr].attach_clk = (CLOCK)0;
    }


    return 0;
}

