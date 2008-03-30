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

/* FIXME: wd1770 support is far from being complete.  */

#undef WD_DEBUG

#include "vice.h"

#include "clkguard.h"
#include "diskimage.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "interrupt.h"
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
#define WD_STAT_NOT_READY     0x80
#define WD_STAT_WRITE_PROTECT 0x40
#define WD_STAT_HEAD_LOADED   0x20
#define WD_STAT_SEEK_ERROR    0x10
#define WD_STAT_CRC_ERROR     0x08
#define WD_STAT_TRACK0        0x04
#define WD_STAT_INDEX_PULSE   0x02
#define WD_STAT_BUSY          0x01

/*-----------------------------------------------------------------------*/

static void wd1770_store(ADDRESS addr, BYTE byte, unsigned int dnr);
static BYTE wd1770_read(ADDRESS addr, unsigned int dnr);
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

/* wd1770 disk controller structure.  */
static wd1770_t wd1770[2];

static log_t wd1770_log = LOG_ERR;

/*-----------------------------------------------------------------------*/
/* WD1770 external interface.  */

void REGPARM2 wd1770d0_store(ADDRESS addr, BYTE byte)
{
    wd1770_store((ADDRESS)(addr & 3), byte, 0);
}

BYTE REGPARM1 wd1770d0_read(ADDRESS addr)
{
    return wd1770_read((ADDRESS)(addr & 3), 0);
}

void wd1770d0_reset(void)
{
    wd1770_reset(0);
}

void REGPARM2 wd1770d1_store(ADDRESS addr, BYTE byte)
{
    wd1770_store((ADDRESS)(addr & 3), byte, 1);
}

BYTE REGPARM1 wd1770d1_read(ADDRESS addr)
{
    return wd1770_read((ADDRESS)(addr & 3), 1);
}

void wd1770d1_reset(void)
{
    wd1770_reset(1);
}

/* Clock overflow handling.  */
static void d0_clk_overflow_callback(CLOCK sub, void *data)
{
    if (wd1770[0].busy_clk > (CLOCK) 0)
        wd1770[0].busy_clk -= sub;
    if (wd1770[0].motor_spinup_clk > (CLOCK) 0)
        wd1770[0].motor_spinup_clk -= sub;
    if (wd1770[0].led_delay_clk > (CLOCK) 0)
        wd1770[0].led_delay_clk -= sub;
    if (wd1770[0].set_drq > (CLOCK) 0)
        wd1770[0].set_drq -= sub;
}

static void d1_clk_overflow_callback(CLOCK sub, void *data)
{
    if (wd1770[1].busy_clk > (CLOCK) 0)
        wd1770[1].busy_clk -= sub;
    if (wd1770[1].motor_spinup_clk > (CLOCK) 0)
        wd1770[1].motor_spinup_clk -= sub;
    if (wd1770[1].led_delay_clk > (CLOCK) 0)
        wd1770[1].led_delay_clk -= sub;
    if (wd1770[1].set_drq > (CLOCK) 0)
        wd1770[1].set_drq -= sub;
}

static void wd1770d0_init(void)
{
    clk_guard_add_callback(drive0_context.cpu.clk_guard,
                           d0_clk_overflow_callback, NULL);

    if (wd1770_log == LOG_ERR)
        wd1770_log = log_open("WD1770");
}

static void wd1770d1_init(void)
{
    clk_guard_add_callback(drive1_context.cpu.clk_guard,
                           d1_clk_overflow_callback, NULL);
}


/* functions using drive context */

void wd1770d_init(drive_context_t *drv)
{
    if (drv->mynumber == 0)
        wd1770d0_init();
    else
        wd1770d1_init();
}

void REGPARM3 wd1770d_store(drive_context_t *drv, ADDRESS addr, BYTE byte)
{
    wd1770_store((ADDRESS)(addr & 3), byte, drv->mynumber);
}

BYTE REGPARM2 wd1770d_read(drive_context_t *drv, ADDRESS addr)
{
    return wd1770_read((ADDRESS)(addr & 3), drv->mynumber);
}

void wd1770d_reset(drive_context_t *drv)
{
    wd1770_reset(drv->mynumber);
}


/*-----------------------------------------------------------------------*/
/* WD1770 register read/write access.  */

static void wd1770_store(ADDRESS addr, BYTE byte, unsigned int dnr)
{
#ifdef WD_DEBUG
    log_debug("WD READ ADDR: %i DATA:%x CLK:%i\n", addr, byte, drive_clk[dnr]);
#endif
    wd1770[dnr].busy_clk = drive_clk[dnr];

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
      case 3:
        wd1770[dnr].reg[addr] = byte;
        break;
    }
}

static BYTE wd1770_read(ADDRESS addr, unsigned int dnr)
{
    BYTE tmp = 0;

    switch (addr) {
      case WD1770_STATUS:
        wd1770[dnr].reg[addr] &= ~(WD_STAT_BUSY | WD_STAT_INDEX_PULSE
                                 | WD_STAT_WRITE_PROTECT);
        if (wd1770[dnr].data_buffer_index >= 0)
            wd1770[dnr].reg[WD1770_STATUS] |= WD_STAT_BUSY
                                              | WD_STAT_INDEX_PULSE;
        if (wd1770[dnr].busy_clk != (CLOCK)0) {
            if (drive_clk[dnr] - wd1770[dnr].busy_clk < 100)
                wd1770[dnr].reg[WD1770_STATUS] |= WD_STAT_BUSY;
            else
                wd1770[dnr].busy_clk = (CLOCK)0;
        }
        if (wd1770[dnr].motor_spinup_clk != (CLOCK)0
            && (wd1770[dnr].reg[WD1770_STATUS] & WD_STAT_NOT_READY)) {
            wd1770[dnr].reg[WD1770_STATUS] &= ~WD_STAT_HEAD_LOADED;
            if (drive_clk[dnr] - wd1770[dnr].motor_spinup_clk > 50) {
                wd1770[dnr].reg[WD1770_STATUS] |= WD_STAT_HEAD_LOADED;
                wd1770[dnr].motor_spinup_clk = (CLOCK)0;
            }
        }
        if (wd1770[dnr].set_drq != (CLOCK)0) {
            if (drive_clk[dnr] - wd1770[dnr].set_drq > 5000) {
                wd1770[dnr].reg[WD1770_STATUS] |= WD_STAT_INDEX_PULSE;
                wd1770[dnr].set_drq = (CLOCK)0;
            }
        }
        if (wd1770[dnr].wp_status)
            wd1770[dnr].reg[WD1770_STATUS] |= WD_STAT_WRITE_PROTECT;

        tmp = wd1770[dnr].reg[WD1770_STATUS];
        break;
      case 1:
      case 2:
        tmp = wd1770[dnr].reg[addr];
        break;
      case 3:
        if (wd1770[dnr].data_buffer_index < 0)
            tmp = wd1770[dnr].reg[addr];
        else
            tmp = wd1770[dnr].data_buffer[wd1770[dnr].data_buffer_index--];
        break;
    }
#ifdef WD_DEBUG
    log_debug("WD READ ADDR: %i DATA:%x CLK:%i\n", addr, tmp, drive_clk[dnr]);
#endif
    return tmp;
}

static void wd1770_reset(unsigned int dnr)
{
    int i;

    wd1770[dnr].busy_clk = (CLOCK)0;
    wd1770[dnr].motor_spinup_clk = (CLOCK)0;
    wd1770[dnr].led_delay_clk = (CLOCK)0;
    wd1770[dnr].set_drq = (CLOCK)0;
    wd1770[dnr].current_track = 20;
    wd1770[dnr].data_buffer_index = -1;
    wd1770[dnr].wp_status = 0;

    for (i = 0; i < 4; i++)
        /* FIXME: Just a wild guess.  */
        wd1770[dnr].reg[i] = 0;

}

/*-----------------------------------------------------------------------*/
/* WD1770 commands, common routines.  No need to inline these functions, as
   the real WD1770 needs ages to execute commands anyway.  */

static void wd1770_update_track_register(BYTE command, int dnr)
{
    if (command & 0x10)
        wd1770[dnr].reg[WD1770_TRACK] = wd1770[dnr].current_track;
}

static void wd1770_motor_control(BYTE command, unsigned int dnr)
{
    if (command & 0x08) {
        if ((wd1770[dnr].reg[WD1770_STATUS] & 0x80) == 0)
            wd1770[dnr].motor_spinup_clk = drive_clk[dnr];
        wd1770[dnr].reg[WD1770_STATUS] |= 0x80;
    } else {
        /* Clear motor and spinup flag.  */
        wd1770[dnr].reg[WD1770_STATUS] &= 0x5f;
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

/*-----------------------------------------------------------------------*/
/* WD1770 commands.  */

static void wd1770_command_restore(BYTE command, unsigned int dnr)
{
    wd1770[dnr].current_track = 0;
    wd1770_update_track_register(0x10, dnr);
    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_seek(BYTE command, unsigned int dnr)
{
    wd1770[dnr].set_drq = drive_clk[dnr];
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_step(BYTE command, unsigned int dnr)
{
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_stepin(BYTE command, unsigned int dnr)
{
    if (wd1770[dnr].current_track < 79)
        wd1770[dnr].current_track++;
    wd1770_update_track_register(command, dnr);
    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_stepout(BYTE command, unsigned int dnr)
{
    if (wd1770[dnr].current_track > 0)
        wd1770[dnr].current_track--;
    wd1770_update_track_register(command, dnr);
    wd1770_motor_control(command, dnr);
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_readsector(BYTE command, unsigned int dnr)
{
    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_writesector(BYTE command, unsigned int dnr)
{
    wd1770_wpstatus_set(dnr);
}

static void wd1770_command_readaddress(BYTE command, unsigned int dnr)
{
    wd1770[dnr].data_buffer[0] = 0xff;
    wd1770[dnr].data_buffer[1] = 0xff;
    wd1770[dnr].data_buffer[2] = 0x2;
    wd1770[dnr].data_buffer[3] = 0x1;
    wd1770[dnr].data_buffer[4] = 0x0;
    wd1770[dnr].data_buffer[5] = 0x0;

    wd1770[dnr].data_buffer_index = 5;
    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_forceint(BYTE command, unsigned int dnr)
{
    /* Abort any command immediately.  Clear status bits.  */
    wd1770[dnr].reg[WD1770_STATUS] = 0;
    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_readtrack(BYTE command, unsigned int dnr)
{
    wd1770[dnr].wp_status = 0;
}

static void wd1770_command_writetrack(BYTE command, unsigned int dnr)
{
    wd1770_wpstatus_set(dnr);
}

/*-----------------------------------------------------------------------*/
/* WD1770 job code emulation.  */

static int wd1770_job_code_read(unsigned int dnr, unsigned int track,
                                unsigned int sector, unsigned int buffer)
{
    ADDRESS base;
    int rc, i;
    BYTE sector_data[256];

    rc = disk_image_read_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(wd1770_log,
                  "Cannot read T:%d S:%d from disk image.",
                  track, sector);
        return 2;
    }

    base = (ADDRESS)((buffer << 8) + 0x300);

    for (i = 0; i < 256; i++) {
        if (dnr == 0)
            drive_store(&drive0_context, (ADDRESS)(base + i), sector_data[i]);
        else
            drive_store(&drive1_context, (ADDRESS)(base + i), sector_data[i]);
    }
    return 0;
}

static int wd1770_job_code_write(unsigned int dnr, unsigned int track,
                                 unsigned int sector, unsigned int buffer)
{
    ADDRESS base;
    int rc, i;
    BYTE sector_data[256];

    base = (ADDRESS)((buffer << 8) + 0x300);

    for (i = 0; i < 256; i++) {
        if (dnr == 0)
            sector_data[i] = drive_read(&drive0_context, (ADDRESS)(base + i));
        else
            sector_data[i] = drive_read(&drive1_context, (ADDRESS)(base + i));
    }

    rc = disk_image_write_sector(wd1770[dnr].image, sector_data, track, sector);

    if (rc < 0) {
        log_error(wd1770_log,
                  "Could not update T:%d S:%d on disk image.",
                  track, sector);
        return 2;
    }
    return 0;
}

void wd1770_handle_job_code(unsigned int dnr)
{
    unsigned int buffer;
    BYTE command, track, sector;
    BYTE rcode = 0;

    for (buffer = 0; buffer <= 8; buffer++) {
        if (dnr == 0) {
            command = drive_read(&drive0_context, (ADDRESS)(0x02 + buffer));
            track = drive_read(&drive0_context, (ADDRESS)(0x0b + (buffer << 1)));
            sector = drive_read(&drive0_context, (ADDRESS)(0x0c + (buffer << 1)));
        } else {
            command = drive_read(&drive1_context, (ADDRESS)(0x02 + buffer));
            track = drive_read(&drive1_context, (ADDRESS)(0x0b + (buffer << 1)));
            sector = drive_read(&drive1_context, (ADDRESS)(0x0c + (buffer << 1)));
        }
        if (command & 0x80) {
#ifdef WD_DEBUG
            log_debug("WD1770 Buffer:%i Command:%x T:%i S:%i\n",
                      buffer, command, track, sector);
#endif
            if (wd1770[dnr].image != NULL
                && wd1770[dnr].image->type == DISK_IMAGE_TYPE_D81) {
                drive[dnr].current_half_track = track * 2;
                switch (command) {
                  case 0x80:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = wd1770_job_code_read(dnr, track, sector, buffer);
                    break;
                  case 0x90:
                    wd1770[dnr].led_delay_clk = drive_clk[dnr];
                    rcode = wd1770_job_code_write(dnr, track, sector, buffer);
                    break;
                  default:
                    rcode = 0;
                }
            } else
                rcode = 2;

            if (dnr == 0)
                drive_store(&drive0_context, (ADDRESS)(2 + buffer), rcode);
            else
                drive_store(&drive1_context, (ADDRESS)(2 + buffer), rcode);
        }
    }
}

void wd1770_vsync_hook(void)
{
    if (drive[0].type == DRIVE_TYPE_1581) {
        if (wd1770[0].led_delay_clk != (CLOCK)0)
            if (drive_clk[0] - wd1770[0].led_delay_clk > 1000000)
                wd1770[0].led_delay_clk = (CLOCK)0;
        drive[0].led_status = (wd1770[0].led_delay_clk == (CLOCK)0) ? 0 : 1;
    }
    if (drive[1].type == DRIVE_TYPE_1581) {
        if (wd1770[1].led_delay_clk != (CLOCK)0)
            if (drive_clk[1] - wd1770[1].led_delay_clk > 1000000)
                wd1770[1].led_delay_clk = (CLOCK)0;
        drive[1].led_status = (wd1770[1].led_delay_clk == (CLOCK)0) ? 0 : 1;
    }
}

int wd1770_attach_image(disk_image_t *image, unsigned int unit)
{
    if (unit != 8 && unit != 9)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_attach_log(image, wd1770_log, unit, "D81");
        break;
      default:
        return -1;
    }

    wd1770[unit - 8].image = image;
    return 0;
}

int wd1770_detach_image(disk_image_t *image, unsigned int unit)
{
    if (unit != 8 && unit != 9)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_detach_log(image, wd1770_log, unit, "D81");
        break;
      default:
        return -1;
    }

    wd1770[unit - 8].image = NULL;
    return 0;
}

