/*
 * pc8477.h - dp8473/pc8477 emulation for the 4000 disk drive.
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

#ifndef VICE_PC8477_H
#define VICE_PC8477_H

#include "types.h"
#include "fdd.h"
#include "alarm.h"

struct disk_image_s;
struct drive_context_s;
typedef void (*pc8477_motor_on_callback_t)(void *data, int signal);

typedef enum pc8477_state_e {
    PC8477_WAIT, PC8477_COMMAND, PC8477_READ, PC8477_WRITE, PC8477_EXEC, PC8477_RESULT
} pc8477_state_t;

typedef enum pc8477_cmd_e {
    PC8477_CMD_INVALID            = 0x00,
    PC8477_CMD_MODE               = 0x01,
    PC8477_CMD_READ_A_TRACK       = 0x02,
    PC8477_CMD_SPECIFY            = 0x03,
    PC8477_CMD_SENSE_DRIVE_STATUS = 0x04,
    PC8477_CMD_WRITE_DATA         = 0x05,
    PC8477_CMD_READ_DATA          = 0x06,
    PC8477_CMD_RECALIBRATE        = 0x07,
    PC8477_CMD_SENSE_INTERRUPT    = 0x08,
    PC8477_CMD_WRITE_DELETED_DATA = 0x09,
    PC8477_CMD_READ_ID            = 0x0a,
    PC8477_CMD_READ_DELETED_DATA  = 0x0c,
    PC8477_CMD_FORMAT_A_TRACK     = 0x0d,
    PC8477_CMD_DUMPREG            = 0x0e,
    PC8477_CMD_SEEK               = 0x0f,
    PC8477_CMD_VERSION            = 0x10,
    PC8477_CMD_SCAN_EQUAL         = 0x11,
    PC8477_CMD_PERPENDICULAR_MODE = 0x12,
    PC8477_CMD_CONFIGURE          = 0x13,
    PC8477_CMD_LOCK               = 0x14,
    PC8477_CMD_VERIFY             = 0x16,
    PC8477_CMD_NSC                = 0x18,
    PC8477_CMD_SCAN_LOW_OR_EQUAL  = 0x19,
    PC8477_CMD_SCAN_HIGH_OR_EQUAL = 0x1d,
    PC8477_CMD_SET_TRACK          = 0x21,
    PC8477_CMD_RELATIVE_SEEK      = 0x8f
} pc8477_cmd_t;

typedef struct pc8477_s {
    char *myname;
    pc8477_cmd_t command;
    pc8477_state_t state;
    int int_step, sub_step;
    struct drive_context_s *mycontext;

    /* Floppy drives */
    struct {
        fd_drive_t *fdd;
        int seeking; /* seeking status */
        int recalibrating; 
        int track; /* actual track register */
        int perpendicular; /* mode */
        int seek_pulses;
        int num;
        pc8477_motor_on_callback_t motor_on;
        void *motor_on_data;
        int motor_on_out;
    } fdds[4], *current;
    int seeking_active;
    fd_drive_t *fdd;
    int head_sel;
    int cmd_flags;
    int irq;

    CLOCK clk, motor_clk;

    /* Registers */
    BYTE st[4];
    BYTE dor, tdr;
    int step_rate, motor_off_time, motor_on_time, nodma;
    int rate;

    int sector; /* sector register */
    int is8477; /* dp8473 or pc 8477 */

    alarm_t *seek_alarm;
    int byte_count;
    int fifop, fifop2, fifo_size, fifo_fill;
    BYTE fifo[16];
    int cmdp, cmd_size;
    BYTE cmd[9];
    int resp, res_size;
    BYTE res[7];
} pc8477_t;

extern void pc8477d_init(struct drive_context_s *drv);
extern void pc8477_setup_context(struct drive_context_s *drv);
extern void pc8477d_store(struct drive_context_s *drv, WORD addr, BYTE byte);
extern BYTE pc8477d_read(struct drive_context_s *drv, WORD addr);
extern void pc8477d_reset(struct drive_context_s *drv, int is8477);

extern int pc8477_attach_image(struct disk_image_s *image, unsigned int unit);
extern int pc8477_detach_image(struct disk_image_s *image, unsigned int unit);

#endif 

