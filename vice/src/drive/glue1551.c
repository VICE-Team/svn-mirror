/*
 * glue1551.c - 1551 glue logic.
 *
 * Written by
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "alarm.h"
#include "drive.h"
#include "drivetypes.h"
#include "glue1551.h"
#include "interrupt.h"
#include "types.h"


/*-----------------------------------------------------------------------*/

#define GLUE1551_ALARM_TICKS_ON   50
#define GLUE1551_ALARM_TICKS_OFF  99950

static alarm_t glue1551d0_timer_alarm;
static alarm_t glue1551d1_timer_alarm;

static int glue1551d0_irq_line;
static int glue1551d1_irq_line;

/*-----------------------------------------------------------------------*/

static void glue_pport_update(drive_context_t *drv)
{
    static BYTE old_output = 0;
    BYTE output, input;

    output = (drv->cpud.drive_ram[1] & drv->cpud.drive_ram[0])
             | ~(drv->cpud.drive_ram[0]);

    /* Stepper motor.  */
    if (((old_output ^ output) & 0x3) && (output & 0x4)) {
        if ((old_output & 0x3) == ((output + 1) & 0x3))
            drive_move_head(-1, drv->mynumber);
        else if ((old_output & 0x3) == ((output - 1) & 0x3))
            drive_move_head(+1, drv->mynumber);
    }

    /* Motor on/off.  */
    if ((old_output ^ output) & 0x04)
        drv->drive_ptr->byte_ready_active
            = (drv->drive_ptr->byte_ready_active & ~0x04) | (output & 0x04);

    /* Drive active LED.  */
    drv->drive_ptr->led_status = (output & 8) ? 1 : 0;

    if (drv->drive_ptr->byte_ready_active == 0x06)
        drive_rotate_disk(drv->drive_ptr);

    input = drive_write_protect_sense(drv->drive_ptr);

    drv->cpud.drive_ram[1] = (drv->cpud.drive_ram[1] & drv->cpud.drive_ram[0])
                             | (input & ~drv->cpud.drive_ram[0]);

    old_output = output;
}

BYTE glue1551_port0_read(drive_context_t *drv)
{
    glue_pport_update(drv);
    return drv->cpud.drive_ram[0];
}

BYTE glue1551_port1_read(drive_context_t *drv)
{
    glue_pport_update(drv);
    return drv->cpud.drive_ram[1];
}

void glue1551_port0_store(drive_context_t *drv, BYTE value)
{
    drv->cpud.drive_ram[0] = value;
    glue_pport_update(drv);
}

void glue1551_port1_store(drive_context_t *drv, BYTE value)
{
    drv->cpud.drive_ram[1] = value;
    glue_pport_update(drv);
}

/*-----------------------------------------------------------------------*/

static void glue1551d0_timer(CLOCK offset)
{
    if (glue1551d0_irq_line == 0) {
        alarm_set(&glue1551d0_timer_alarm,
                  *(drive0_context.clk_ptr) + GLUE1551_ALARM_TICKS_ON);
        interrupt_set_irq(&(drive0_context.cpu.int_status),
                          I_GLUE1551D0FL, IK_IRQ, *(drive0_context.clk_ptr));
    } else {
        alarm_set(&glue1551d0_timer_alarm,
                  *(drive0_context.clk_ptr) + GLUE1551_ALARM_TICKS_OFF);
        interrupt_set_irq(&(drive0_context.cpu.int_status),
                          I_GLUE1551D0FL, 0, *(drive0_context.clk_ptr));
    }
    glue1551d0_irq_line ^= 1;
}

static void glue1551d1_timer(CLOCK offset)
{
    if (glue1551d1_irq_line == 0) {
        alarm_set(&glue1551d1_timer_alarm,
                  *(drive1_context.clk_ptr) + GLUE1551_ALARM_TICKS_ON);
        interrupt_set_irq(&(drive1_context.cpu.int_status),
                          I_GLUE1551D1FL, IK_IRQ, *(drive1_context.clk_ptr));
    } else {
        alarm_set(&glue1551d1_timer_alarm,
                  *(drive1_context.clk_ptr) + GLUE1551_ALARM_TICKS_OFF);
        interrupt_set_irq(&(drive1_context.cpu.int_status),
                          I_GLUE1551D1FL, 0, *(drive1_context.clk_ptr));
    }
    glue1551d1_irq_line ^= 1;
}

void glue1551_init(drive_context_t *drv)
{
    if (drv->mynumber == 0)
        alarm_init(&glue1551d0_timer_alarm, drv->cpu.alarm_context,
                   "GLUE1551D0", glue1551d0_timer);
    else
        alarm_init(&glue1551d1_timer_alarm, drv->cpu.alarm_context,
                   "GLUE1551D1", glue1551d1_timer);
}

void glue1551_reset(drive_context_t *drv)
{
    if (drv->mynumber == 0) {
        alarm_unset(&glue1551d0_timer_alarm);
        alarm_set(&glue1551d0_timer_alarm,
                  *(drv->clk_ptr) + GLUE1551_ALARM_TICKS_OFF);
        glue1551d0_irq_line = 0;
    } else {
        alarm_unset(&glue1551d1_timer_alarm);
        alarm_set(&glue1551d1_timer_alarm,
                  *(drv->clk_ptr) + GLUE1551_ALARM_TICKS_OFF);
        glue1551d1_irq_line = 0;
    }
}

