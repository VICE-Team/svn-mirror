/*
 * wd1770.h - WD1770 emulation for the 1571 and 1581 disk drives.
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

#ifndef _WD1770_H
#define _WD1770_H

#include "types.h"

struct disk_image_s;

typedef struct wd1770_s {
    /* WD1770 register.  */
    BYTE reg[4];
    /* Busy bit clock counter.  */
    CLOCK busy_clk;
    /* Clock counter to control motor spinup.  */
    CLOCK motor_spinup_clk;
    /* Current track of the r/w head.  */
    int current_track;
    /* Current disk side.  */
    int side;
    /* Data register buffer.  */
    BYTE data_buffer[512];
    /* Data register buffer index.  */
    int data_buffer_index;
    /* WP bit status.  */
    unsigned int wp_status;
    /* LED delay.  */
    CLOCK led_delay_clk;
    /* Interrupt line.  */
    CLOCK set_drq;
    /* Pointer to the disk image.  */
    struct disk_image_s *image;
} wd1770_t;

extern void REGPARM2 wd1770d0_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 wd1770d0_read(ADDRESS addr);
extern void wd1770d0_reset(void);

extern void REGPARM2 wd1770d1_store(ADDRESS addr, BYTE byte);
extern BYTE REGPARM1 wd1770d1_read(ADDRESS addr);
extern void wd1770d1_reset(void);

/* alternative functions with drive context */
struct drive_context_s;
extern void wd1770d_init(struct drive_context_s *drv);
extern void REGPARM3 wd1770d_store(struct drive_context_s *drv, ADDRESS addr, BYTE byte);
extern BYTE REGPARM2 wd1770d_read(struct drive_context_s *drv, ADDRESS addr);
extern void wd1770d_reset(struct drive_context_s *drv);

extern void wd1770_handle_job_code(unsigned int dnr);
extern void wd1770_vsync_hook(void);

extern int wd1770_attach_image(struct disk_image_s *image, unsigned int unit);
extern int wd1770_detach_image(struct disk_image_s *image, unsigned int unit);

#endif 

