/*
 * pc8477.c - dp8473/pc8477 emulation for the 1992 disk drive.
 *
 * Written by
 *  Kajtar Zsolt <soci@c64.rulez.org>
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
#include <string.h>

#include "clkguard.h"
#include "diskimage.h"
#include "drive.h"
#include "drivecpu.h"
#include "drivetypes.h"
#include "log.h"
#include "types.h"
#include "pc8477.h"

#define debug(...) log_message(pc8477_log, __VA_ARGS__)

/* wd1770 disk controller structure.  */
pc8477_t pc8477[DRIVE_NUM];

static log_t pc8477_log = LOG_ERR;

/*-----------------------------------------------------------------------*/

static void pc8477_store(WORD addr, BYTE byte, unsigned int dnr);
static BYTE pc8477_read(WORD addr, unsigned int dnr);
static void pc8477_reset(int unsigned dnr);

/* Clock overflow handling.  */
static void clk_overflow_callback(CLOCK sub, void *data)
{
}

/* Functions using drive context.  */
void pc8477d_init(drive_context_t *drv)
{
    if (pc8477_log == LOG_ERR)
        pc8477_log = log_open("PC8477");

    clk_guard_add_callback(drv->cpu->clk_guard, clk_overflow_callback,
                           uint_to_void_ptr(drv->mynumber));

    memset(&pc8477[drv->mynumber], 0, sizeof(pc8477_t));
    pc8477[drv->mynumber].fdds[1] = fdd_init(1);
    pc8477[drv->mynumber].mycontext = drv;
}

void pc8477d_store(drive_context_t *drv, WORD addr, BYTE byte)
{
    pc8477_store((WORD)(addr & 7), byte, drv->mynumber);
}

BYTE pc8477d_read(drive_context_t *drv, WORD addr)
{
    return pc8477_read((WORD)(addr & 7), drv->mynumber);
}

void pc8477d_reset(drive_context_t *drv)
{
    pc8477_reset(drv->mynumber);
}


/*-----------------------------------------------------------------------*/
/* WD1770 register read/write access.  */

static void pc8477_software_reset(pc8477_t *drv)
{
    drv->st[0] = 0xc0;
    drv->st[1] = 0x00;
    drv->st[2] = 0x00;
    drv->st[3] = 0x00;
    drv->state = PC8477_WAIT;
}

static void pc8477_result(pc8477_t *drv)
{
    switch (drv->command) {
    case PC8477_CMD_SPECIFY:
        return;
    case PC8477_CMD_SENSE_INTERRUPT:
        drv->res[0] = drv->st[0];
        drv->res[1] = drv->track[drv->st[0] & 3];
        return;
    case PC8477_CMD_VERSION:
        drv->res[0] = 0x90;
        return;
    case PC8477_CMD_NSC:
        drv->res[0] = 0x72;
        return;
    case PC8477_CMD_SENSE_DRIVE_STATUS:
        drv->res[0] = drv->st[3] | 0x20;
	if (drv->fdd) {
	    drv->res[0] |= (drv->fdd->track0? 0x10 : 0) | (drv->fdd->write_protect? 0x40 : 0);
	}
        return;
    case PC8477_CMD_READ_ID:
        memcpy(drv->res, drv->st, 3);
        return;
    case PC8477_CMD_RECALIBRATE:
        return;
    case PC8477_CMD_SEEK:
        return;
    case PC8477_CMD_DUMPREG:
        drv->res[0] = drv->track[0];
        drv->res[1] = drv->track[1];
        drv->res[2] = drv->track[2];
        drv->res[3] = drv->track[3];
        drv->res[4] = (drv->step_rate << 4) | drv->motor_off_time;
        drv->res[5] = (drv->motor_on_time << 1) | drv->nodma;
        drv->res[6] = drv->sector;
        /* TODO */ 
        return;
    case PC8477_CMD_SET_TRACK:
        if (drv->cmd[1] & 4) {
            drv->res[0] = drv->track[drv->cmd[1] & 3] >> 8;
        } else {
            drv->res[0] = drv->track[drv->cmd[1] & 3];
        }
        return;
    case PC8477_CMD_READ_DATA:
    case PC8477_CMD_WRITE_DATA:
        memcpy(drv->res, drv->st, 3);
        memcpy(drv->res + 3, drv->cmd + 2, 4);
        return;
    default:
        drv->res[0] = drv->st[0];
    }
}

static pc8477_state_t pc8477_execute(pc8477_t *drv, int phase)
{
    switch (drv->command) {
    case PC8477_CMD_SPECIFY:
        debug("SPECIFY %d, %d, %d, %d", drv->cmd[1] >> 4, drv->cmd[1] & 0xf, drv->cmd[2] >> 1, drv->cmd[2] & 1);
        drv->step_rate = drv->cmd[1] >> 4;
        drv->motor_off_time = drv->cmd[1] & 0xf;
        drv->motor_on_time = drv->cmd[2] >> 1;
        drv->nodma = drv->cmd[2] & 1;
        return PC8477_WAIT;
    case PC8477_CMD_SENSE_INTERRUPT:
        debug("SENSE INTERRUPT");
        drv->seeking[drv->sel] = 0; /* TODO: Too early */
        return PC8477_RESULT;
    case PC8477_CMD_VERSION:
        if (!drv->is8477) break;
        debug("VERSION");
        return PC8477_RESULT;
    case PC8477_CMD_NSC:
        if (!drv->is8477) break;
        debug("NSC");
        return PC8477_RESULT;
    case PC8477_CMD_SENSE_DRIVE_STATUS:
        debug("SENSE DRIVE STATUS #%d", drv->cmd[1] & 3);
        return PC8477_RESULT;
    case PC8477_CMD_READ_ID:
        {
            int res = -1;
            if (!phase) {
                debug("READ ID #%d", drv->cmd[1] & 3);
                drv->index_count = 0;
                drv->st[1] = 0x01; /* no AM */
            }
            res = fdd_image_read_header(drv->fdd, &drv->res[3], &drv->res[4], &drv->res[5], &drv->res[6]);
            if (drv->fdd && drv->fdd->index) {
                drv->index_count++;
                if (drv->index_count > 1) {
                    drv->st[0] |= 0x40;
                    return PC8477_RESULT;
                }
            }
            if (res) {
                return PC8477_EXEC;
            }
            drv->st[1] = 0x00;
            return PC8477_RESULT;
        }
    case PC8477_CMD_RECALIBRATE:
        if (!phase) {
	    debug("RECALIBRATE #%d", drv->cmd[1] & 3);
	    drv->seeking[drv->cmd[1] & 3] = 1;
	}
        if (drv->fdd && drv->fdd->track0) {
            drv->track[drv->cmd[1] & 3] = 0;
            drv->st[0] |= 0x20; /* seek end */
            return PC8477_WAIT;
        } 
        fdd_seek_pulse(drv->fdd, 0);
	if (drv->fdd) {
	    drv->mycontext->drive->current_half_track = (drv->fdd->track + 1) * 2;
	}
        return PC8477_EXEC;
    case PC8477_CMD_SEEK:
        if (!phase) {
            debug("SEEK #%d %d", drv->cmd[1] & 3, drv->cmd[2]);
            drv->seeking[drv->cmd[1] & 3] = 1;
        }
        if (drv->cmd[2] == drv->track[drv->cmd[1] & 3]) {
            drv->st[0] |= 0x20; /* seek end */;
            return PC8477_WAIT;
        }
        if (drv->cmd[2] < drv->track[drv->cmd[1] & 3]) {
            fdd_seek_pulse(drv->fdd, 0);
            drv->track[drv->cmd[1] & 3]--;
        } else {
            fdd_seek_pulse(drv->fdd, 1);
            drv->track[drv->cmd[1] & 3]++;
        }
	if (drv->fdd) {
	    drv->mycontext->drive->current_half_track = (drv->fdd->track + 1) * 2;
	}
        return PC8477_EXEC;
    case PC8477_CMD_DUMPREG:
        if (!drv->is8477) break;
        debug("DUMPREG");
        return PC8477_RESULT;
    case PC8477_CMD_PERPENDICULAR_MODE:
        if (!drv->is8477) break;
        debug("PERPENDICULAR MODE");
        return PC8477_WAIT;
    case PC8477_CMD_SET_TRACK:
        if ((drv->cmd[1] & 0xf8) != 0x30) break;
        debug("SET TRACK #%d %d", drv->cmd[1] & 3, drv->cmd[2]);
        if (drv->cmd[0] & 0x40) {
            if (drv->cmd[1] & 4) {
                drv->track[drv->cmd[1] & 3] = (drv->track[drv->cmd[1] & 3] & 0xff) | (drv->cmd[2] << 8);
            } else {
                drv->track[drv->cmd[1] & 3] = (drv->track[drv->cmd[1] & 3] & 0xff00) | drv->cmd[2];
            }
        }
        return PC8477_RESULT;
    case PC8477_CMD_READ_DATA:
        {
            BYTE track, head, sector, bytes;
            int res = -1;
            if (!phase) {
                debug("READ DATA #%d (%d/%d/%d)-%d %d", drv->cmd[1] & 3, drv->cmd[2], drv->cmd[3], drv->cmd[4], drv->cmd[6], 128 << drv->cmd[5]);
                drv->index_count = 0;
                drv->sector = drv->cmd[4];
                drv->st[1] = 0x01; /* no AM */
            }
            if (drv->st[1] == 0x80) {
                drv->cmd[4] = drv->sector;
                drv->st[0] = drv->st[3] | 0x40;
                return PC8477_RESULT;
            }
            res = fdd_image_read_header(drv->fdd, &track, &head, &sector, &bytes);
            if (drv->fdd && drv->fdd->index) {
                drv->index_count++;
                if (drv->index_count > 1) {
                    drv->st[0] = drv->st[3] | 0x40;
                    return PC8477_RESULT;
                }
            }
            if (res) {
                return PC8477_EXEC;
            }
            if (drv->st[1] == 0x01) {
                drv->st[1] = 0x04; /* No data */
            }
            if (drv->cmd[2] != track) {
                drv->st[0] = drv->st[3] | 0x40;
                drv->st[2] = 0x10;
                return PC8477_RESULT;
            }
            if (drv->cmd[3] == head && drv->sector == sector && drv->cmd[5] == bytes) {
                fdd_image_read(drv->fdd, drv->buf);
		if (drv->cmd[6] == drv->sector) {
		    drv->st[1] = 0x80;
		    drv->sector = 1;
		    drv->cmd[2]++;
		} else {
		    drv->st[1] = 0x00;
		    drv->sector++;
		}
                drv->buf_size = 128 << bytes;
                drv->bufp = 0;
                drv->index_count = 0;
                return PC8477_READ;
            }
            return PC8477_EXEC;
        }
        break;
    case PC8477_CMD_WRITE_DATA:
        {
            BYTE track, head, sector, bytes;
            int res = -1;
            if (!phase) {
                debug("WRITE DATA #%d (%d/%d/%d)-%d %d", drv->cmd[1] & 3, drv->cmd[2], drv->cmd[3], drv->cmd[4], drv->cmd[6], 128 << drv->cmd[5]);
                drv->index_count = 0;
                drv->sector = drv->cmd[4];
                drv->st[1] = 0x01; /* no AM */
            }
	    if (drv->st[1] & 0x08) {
                fdd_image_write(drv->fdd, drv->buf);
		drv->st[1] &= 0xf7;
	    }
            if (drv->st[1] & 0x80) {
                drv->cmd[4] = drv->sector;
                drv->st[0] = drv->st[3] | 0x40;
                return PC8477_RESULT;
            }
            res = fdd_image_read_header(drv->fdd, &track, &head, &sector, &bytes);
            if (drv->fdd && drv->fdd->index) {
                drv->index_count++;
                if (drv->index_count > 1) {
                    drv->st[0] = drv->st[3] | 0x40;
                    return PC8477_RESULT;
                }
            }
            if (res) {
                return PC8477_EXEC;
            }
            if (drv->fdd && drv->fdd->write_protect) {
                drv->st[0] = drv->st[3] | 0x40;
                drv->st[1] = 0x02;
                return PC8477_RESULT;
            }
            if (drv->cmd[2] != track) {
                drv->st[0] = drv->st[3] | 0x40;
                drv->st[2] = 0x10;
                return PC8477_RESULT;
            }
            if (drv->cmd[3] == head && drv->sector == sector && drv->cmd[5] == bytes) {
		if (drv->cmd[6] == drv->sector) {
		    drv->st[1] = 0x88;
		    drv->sector = 1;
		    drv->cmd[2]++;
		} else {
		    drv->st[1] = 0x08;
		    drv->sector++;
		}
                drv->buf_size = 128 << bytes;
                drv->bufp = 0;
                drv->index_count = 0;
                return PC8477_WRITE;
            }
	    drv->st[1] = 0x00;
            return PC8477_EXEC;
        }
        break;
    default:
        break;
    }
    debug("invalid command %02x", drv->cmd[0]);
    drv->command = PC8477_CMD_INVALID;
    drv->st[0] = drv->st[3] | 0x80; /* invalid command */
    drv->res_size = 1;
    return PC8477_RESULT;
}

static void pc8477_store(WORD addr, BYTE byte, unsigned int dnr)
{
    pc8477_t *drv = &pc8477[dnr];
    int i;
    const static struct {
        BYTE code, mask; 
        pc8477_cmd_t command;
        BYTE len, rlen, sel;
    } commands[22]={
        {0x06, 0x1f, PC8477_CMD_READ_DATA,          9, 7,  2},
        {0x0a, 0xbf, PC8477_CMD_READ_ID,            2, 7,  2},
        {0x0d, 0xbf, PC8477_CMD_FORMAT_A_TRACK,     6, 7,  2},
        {0x0c, 0x1f, PC8477_CMD_READ_DELETED_DATA,  9, 7,  2},
        {0x05, 0x3f, PC8477_CMD_WRITE_DATA,         9, 7,  2},
        {0x11, 0x1f, PC8477_CMD_SCAN_EQUAL,         9, 7,  2},
        {0x02, 0x9f, PC8477_CMD_READ_A_TRACK,       9, 7,  2},
        {0x09, 0x3f, PC8477_CMD_WRITE_DELETED_DATA, 9, 7,  2},
        {0x19, 0x1f, PC8477_CMD_SCAN_LOW_OR_EQUAL,  9, 7,  2},
        {0x1d, 0x1f, PC8477_CMD_SCAN_HIGH_OR_EQUAL, 9, 7,  2},
        {0x04, 0xff, PC8477_CMD_SENSE_DRIVE_STATUS, 2, 1,  2},
        {0x03, 0xff, PC8477_CMD_SPECIFY,            3, 0,  0},
        {0x0f, 0xff, PC8477_CMD_SEEK,               3, 0,  2},
        {0x01, 0xff, PC8477_CMD_MODE,               5, 0,  0},
        {0x07, 0xff, PC8477_CMD_RECALIBRATE,        2, 0,  1},
        {0x21, 0xbf, PC8477_CMD_SET_TRACK,          3, 0,  1},
        {0x08, 0xff, PC8477_CMD_SENSE_INTERRUPT,    1, 2,  0},
        {0x10, 0xff, PC8477_CMD_VERSION,            1, 1,  0},
        {0x18, 0xff, PC8477_CMD_NSC,                1, 1,  0},
        {0x0e, 0xff, PC8477_CMD_DUMPREG,            1, 10, 0},
        {0x12, 0xff, PC8477_CMD_PERPENDICULAR_MODE, 2, 0,  0},
        {0x00, 0x00, PC8477_CMD_INVALID,            1, 1,  0}
    };

    switch (addr) {
        case 2: /* DCR */
            if (byte & 0x04) {
                pc8477_software_reset(drv);
            }
            fdd_set_motor(drv->fdds[0], byte >> 4);
            fdd_set_motor(drv->fdds[1], byte >> 5);
            fdd_set_motor(drv->fdds[2], byte >> 6);
            fdd_set_motor(drv->fdds[3], byte >> 7);
            drv->sel = byte & 3;
            break;
        case 5: /* DATA */
            switch (drv->state) {
            case PC8477_WAIT:
                drv->cmdp = 0;
                drv->resp = 0;
                drv->bufp = 0;
                drv->buf_size = 0;
                for (i = 0; i < 22; i++) {
                    if (commands[i].code == (commands[i].mask & byte)) {
                        break;
                    }
                }
                drv->command = commands[i].command;
                drv->cmd_size = commands[i].len;
                drv->res_size = commands[i].rlen;
                drv->state = PC8477_COMMAND;
                drv->cmd_sel = commands[i].sel;
                /* fall through */
            case PC8477_COMMAND:
                if (drv->cmdp < drv->cmd_size) {
                    drv->cmd[drv->cmdp++] = byte;
                }
                if (drv->cmdp < drv->cmd_size) {
                    return;
                }
                if (drv->command != PC8477_CMD_SENSE_INTERRUPT) {
                    drv->st[1] = 0;
                    drv->st[2] = 0;
                    if (drv->cmd_sel) {
                        drv->fdd = drv->fdds[drv->cmd[1] & 3];
                        drv->st[3] = drv->cmd[1] & 3;
                        if (drv->cmd_sel > 1) {
                            fdd_select_head(drv->fdd, drv->cmd[1] >> 2);
                        }
                    } else {
                        drv->fdd = drv->fdds[drv->sel];
                        drv->st[3] = drv->sel;
                    }
		    if (drv->fdd) {
			drv->st[3] |= drv->fdd->head << 2;
		    }
                    drv->st[0] = drv->st[3];
                }
                memset(drv->res, 0, sizeof(drv->res));
                drv->state = pc8477_execute(drv, 0);
                break;
            case PC8477_WRITE:
		drv->buf[drv->bufp++] = byte;
                if (drv->bufp >= drv->buf_size) {
		    drv->state = PC8477_EXEC;
                }
                return;
            case PC8477_EXEC:
		drv->st[0] |= 0x40;
                drv->state = PC8477_RESULT;
                return;
            case PC8477_RESULT:
            case PC8477_READ:
                return;
            }
            break;
        case 7: /* DRR */
            fdd_set_rate(drv->fdds[0], byte);
            fdd_set_rate(drv->fdds[1], byte);
            fdd_set_rate(drv->fdds[2], byte);
            fdd_set_rate(drv->fdds[3], byte);
            break;
    }
}

static BYTE pc8477_read(WORD addr, unsigned int dnr)
{
    pc8477_t *drv = &pc8477[dnr];
    BYTE result = 0;

    switch (addr) {
        case 4: /* MSR */
            if (drv->state == PC8477_EXEC) {
                drv->state = pc8477_execute(drv, 1);
            }
            result |= drv->seeking[0] ? 0x01 : 0x00;
            result |= drv->seeking[1] ? 0x02 : 0x00;
            result |= drv->seeking[2] ? 0x04 : 0x00;
            result |= drv->seeking[3] ? 0x08 : 0x00;

            if (drv->state != PC8477_WAIT) {
                result |= 0x10;
            }
            if (drv->nodma && (drv->state == PC8477_READ || drv->state == PC8477_WRITE)) {
                result |= 0x20;
            }
            if (drv->state == PC8477_READ || drv->state == PC8477_RESULT) {
                result |= 0x40;
            }
            if (drv->state != PC8477_EXEC) {
                result |= 0x80;
            }
            return result;
        case 5: /* DATA */
            switch (drv->state) {
            case PC8477_WAIT:
            case PC8477_COMMAND:
            case PC8477_WRITE:
            case PC8477_EXEC:
                break;
            case PC8477_READ:
                result = drv->buf[drv->bufp++];
                if (drv->bufp >= drv->buf_size) {
                    drv->state = PC8477_EXEC;
                }
                return result;
            case PC8477_RESULT:
                if (!drv->resp) {
                    pc8477_result(drv);
                }
                result = drv->res[drv->resp++];
                if (drv->resp >= drv->res_size) {
                    drv->state = PC8477_WAIT;
                }
                return result;
            }
            break;
        case 7: /* DKR */
            result = (addr >> 8) & 0x7f;
	    if (drv->fdds[drv->sel]) {
		result |= drv->fdds[drv->sel]->disk_change ? 0x80 : 0;
	    }
            return result;
    }
    return addr >> 8; /* tri-state */
}

static void pc8477_reset(unsigned int dnr)
{
    pc8477_t *drv = &pc8477[dnr];

    drv->is8477 = 1;
    memset(drv->track, 0, sizeof(drv->track));
    drv->sel = 0;
    pc8477_software_reset(drv);
}

/*-----------------------------------------------------------------------*/

void pc8477_vsync_hook(void)
{
}

int pc8477_attach_image(disk_image_t *image, unsigned int unit)
{
    if (unit < 8 || unit > 8 + DRIVE_NUM)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_attach_log(image, pc8477_log, unit);
        break;
      default:
        return -1;
    }

    fdd_image_attach(pc8477[unit - 8].fdds[1], image);
    return 0;
}

int pc8477_detach_image(disk_image_t *image, unsigned int unit)
{
    if (image == NULL || unit < 8 || unit > 8 + DRIVE_NUM)
        return -1;

    switch(image->type) {
      case DISK_IMAGE_TYPE_D81:
        disk_image_detach_log(image, pc8477_log, unit);
        break;
      default:
        return -1;
    }

    fdd_image_detach(pc8477[unit - 8].fdds[1]);
    return 0;
}

