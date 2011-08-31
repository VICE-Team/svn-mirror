/*
 * pc8477.h - dp8473/pc8477 emulation for the 1992 disk drive.
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

struct disk_image_s;

typedef enum pc8477_state_e {
    PC8477_WAIT, PC8477_COMMAND, PC8477_READ, PC8477_WRITE, PC8477_EXEC, PC8477_RESULT
} pc8477_state_t;

typedef enum pc8477_cmd_e {
    PC8477_CMD_MODE, PC8477_CMD_READ_A_TRACK, PC8477_CMD_SPECIFY, PC8477_CMD_SENSE_DRIVE_STATUS,
    PC8477_CMD_WRITE_DATA, PC8477_CMD_RECALIBRATE, PC8477_CMD_SENSE_INTERRUPT,
    PC8477_CMD_WRITE_DELETED_DATA, PC8477_CMD_READ_DATA, PC8477_CMD_SEEK, PC8477_CMD_SCAN_EQUAL,
    PC8477_CMD_SCAN_LOW_OR_EQUAL, PC8477_CMD_SCAN_HIGH_OR_EQUAL, PC8477_CMD_SET_TRACK,
    PC8477_CMD_READ_ID, PC8477_CMD_FORMAT_A_TRACK, PC8477_CMD_READ_DELETED_DATA,
    PC8477_CMD_VERSION, PC8477_CMD_NSC, PC8477_CMD_DUMPREG, PC8477_CMD_PERPENDICULAR_MODE,
    PC8477_CMD_INVALID
} pc8477_cmd_t;

typedef struct pc8477_s {
    pc8477_cmd_t command;
    pc8477_state_t state;
    drive_context_t *mycontext;

    /* Floppy drives */
    fd_drive_t *fdds[4], *fdd;
    int seeking[4];
    int motor[4]; /* motor signals */
    int sel;
    int cmd_sel;

    /* Registers */
    BYTE st[4];
    int track[4]; /* actual track of head */
    int step_rate, motor_off_time, motor_on_time, nodma;
    int index_count;

    int sector; /* sector register */
    int is8477; /* dp8473 or pc 8477 */

    int bufp, buf_size;
    BYTE buf[8192];
    int cmdp, cmd_size;
    BYTE cmd[9];
    int resp, res_size;
    BYTE res[7];
} pc8477_t;

extern pc8477_t pc8477[];

struct drive_context_s;

extern void pc8477d_init(struct drive_context_s *drv);
extern void pc8477d_store(struct drive_context_s *drv, WORD addr,
                                   BYTE byte);
extern BYTE pc8477d_read(struct drive_context_s *drv, WORD addr);
extern void pc8477d_reset(struct drive_context_s *drv);

extern void pc8477_vsync_hook(void);

extern int pc8477_attach_image(struct disk_image_s *image, unsigned int unit);
extern int pc8477_detach_image(struct disk_image_s *image, unsigned int unit);
extern int pc8477_disk_change(struct drive_context_s *drive_context);

#endif 

