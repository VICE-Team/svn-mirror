/*
 * vdc-snapshot.c - Snapshot functionality for the MOS 8563 (VDC)
 * emulation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Markus Brenner <markus@brenner.de>
 *  Andreas Boose <viceteam@t-online.de>
 *  Errol Smith <strobey@users.sourceforge.net>
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

#include "alarm.h"
#include "interrupt.h"
#include "log.h"
#include "raster-snapshot.h"
#include "snapshot.h"
#include "types.h"
#include "vdc-snapshot.h"
#include "vdc.h"
#include "vdctypes.h"
#include "videoarch.h"

/* Make sure all the VDC alarms are removed. */
void vdc_snapshot_prepare(void)
{
    alarm_unset(vdc.raster_draw_alarm);
}


/*
   VDC 0.0 snapshot module format:

   Type     | Name                  | Description
   --------------------------------------------------------
   WORD     | vdc.update_reg        |

   WORD     | vdc.screen_adr        |
   WORD     | vdc.cursor_adr        |
   WORD     | vdc.update_adr        |
   WORD     | vdc.attribute_adr     |
   WORD     | vdc.chargen_adr       |

   WORD     | vdc.mem_counter       |
   WORD     | vdc.bitmap_counter    |
   WORD     | vdc.bytes_per_char    |
   WORD     | vdc.charwidth         |

   WORD     | vdc.skip_after_line   |

   DWORD    | CLOCK offset          |

   WORD     | vdc.frame_counter     |

   WORD     | vdc.row_counter       |
   WORD     | vdc.row_counter_y     |
   WORD     | vdc.row_counter_latch |
   WORD     | vdc.attribute_offset  |
   WORD     | vdc.display_enable    |
   WORD     | vdc.prime_draw        |
   WORD     | vdc.draw_active       |
   WORD     | vdc.draw_finished     |
   WORD     | vdc.vsync             |
   WORD     | vdc.vsync_counter     |
   WORD     | vdc.vsync_height      |
   WORD     | vdc.draw_counter      |
   WORD     | vdc.draw_counter_y    |
   WORD     | vdc.draw_counter_latch|
   WORD     | vdc.vert_fine_adj     |
   WORD     | vdc.stable_size_count |
   WORD     | vdc.old_screen_adr    |
   WORD     | vdc.old_attribute_adr |
   WORD     | vdc.interlaced        |

   64*BYTE  | Registers             | VDC registers
   ARRAY    | RAM                   | 0x10000 BYTES of VDC RAM data
   ARRAY    | Raster                | VDC Raster
 */


static const char snap_module_name[] = "VDC";
#define SNAP_MAJOR 0
#define SNAP_MINOR 0


int vdc_snapshot_write_module(snapshot_t *s)
{
    int i;
    snapshot_module_t *m;

    m = snapshot_module_create (s, snap_module_name, SNAP_MAJOR, SNAP_MINOR);
    if (m == NULL) {
        return -1;
    }

    if (0
        || SMW_W(m, (int16_t)vdc.update_reg) < 0

        || SMW_W(m, (uint16_t)vdc.screen_adr) < 0
        || SMW_W(m, (uint16_t)vdc.cursor_adr) < 0
        || SMW_W(m, (uint16_t)vdc.update_adr) < 0
        || SMW_W(m, (uint16_t)vdc.attribute_adr) < 0
        || SMW_W(m, (uint16_t)vdc.chargen_adr) < 0

        || SMW_W(m, (uint16_t)vdc.mem_counter) < 0
        || SMW_W(m, (uint16_t)vdc.bitmap_counter) < 0
        || SMW_W(m, (uint16_t)vdc.bytes_per_char) < 0
        || SMW_W(m, (uint16_t)vdc.charwidth) < 0

        || SMW_W(m, (uint16_t)vdc.skip_after_line) < 0

        || SMW_CLOCK(m, alarm_context_next_pending_clk(vdc.raster_draw_alarm->context) - maincpu_clk) < 0

        || SMW_W(m, (int16_t)vdc.frame_counter) < 0

        || SMW_W(m, (uint16_t)vdc.row_counter) < 0
        || SMW_W(m, (uint16_t)vdc.row_counter_y) < 0
        || SMW_W(m, (uint16_t)vdc.row_counter_latch) < 0
        || SMW_W(m, (uint16_t)vdc.attribute_offset) < 0
        || SMW_W(m, (uint16_t)vdc.display_enable) < 0
        || SMW_W(m, (uint16_t)vdc.prime_draw) < 0
        || SMW_W(m, (uint16_t)vdc.draw_active) < 0
        || SMW_W(m, (uint16_t)vdc.draw_finished) < 0
        || SMW_W(m, (uint16_t)vdc.vsync) < 0
        || SMW_W(m, (uint16_t)vdc.vsync_counter) < 0
        || SMW_W(m, (uint16_t)vdc.vsync_height) < 0
        || SMW_W(m, (uint16_t)vdc.draw_counter) < 0
        || SMW_W(m, (uint16_t)vdc.draw_counter_y) < 0
        || SMW_W(m, (uint16_t)vdc.draw_counter_latch) < 0
        || SMW_W(m, (uint16_t)vdc.vert_fine_adj) < 0
        || SMW_W(m, (uint16_t)vdc.stable_size_count) < 0
        || SMW_W(m, (uint16_t)vdc.old_screen_adr) < 0
        || SMW_W(m, (uint16_t)vdc.old_attribute_adr) < 0
        || SMW_W(m, (uint16_t)vdc.interlaced) < 0 ) {
        goto fail;
    }

    /* VDC Registers */
    for (i = 0; i < 0x40; i++) {
        if (SMW_B(m, vdc.regs[i]) < 0) {
            goto fail;
        }
    }

    /* VDC RAM 64K */
    if (SMW_BA(m, vdc.ram, 0x10000) < 0) {
        goto fail;
    }

    /* VDC Raster */
    if (raster_snapshot_write(m, &vdc.raster)) {
        goto fail;
    }

    log_warning(LOG_DEFAULT, "FIXME: VDC snapshots are work in progress");
    return snapshot_module_close(m);

fail:
    if (m != NULL)
        snapshot_module_close(m);
    return -1;
}

/* Helper functions.  */

int vdc_snapshot_read_module(snapshot_t *s)
{
    uint8_t major_version, minor_version;
    int i;
    CLOCK qw;
    snapshot_module_t *m;

    m = snapshot_module_open(s, snap_module_name,
                             &major_version, &minor_version);
    if (m == NULL) {
        return -1;
    }

    if (snapshot_version_is_bigger(major_version, minor_version, SNAP_MAJOR, SNAP_MINOR)) {
        log_error(vdc.log,
                  "Snapshot module version (%d.%d) newer than %d.%d.",
                  major_version, minor_version,
                  SNAP_MAJOR, SNAP_MINOR);
        goto fail;
    }

    if (0
        || SMR_W_INT(m, &vdc.update_reg) < 0

        || SMR_W_UINT(m, &vdc.screen_adr) < 0
        || SMR_W_UINT(m, &vdc.cursor_adr) < 0
        || SMR_W_UINT(m, &vdc.update_adr) < 0
        || SMR_W_UINT(m, &vdc.attribute_adr) < 0
        || SMR_W_UINT(m, &vdc.chargen_adr) < 0

        || SMR_W_UINT(m, &vdc.mem_counter) < 0
        || SMR_W_UINT(m, &vdc.bitmap_counter) < 0
        || SMR_W_UINT(m, &vdc.bytes_per_char) < 0
        || SMR_W_UINT(m, &vdc.charwidth) < 0

        || SMR_W_UINT(m, &vdc.skip_after_line) < 0

        || SMR_CLOCK(m, &qw) < 0

        || SMR_W_INT(m, &vdc.frame_counter) < 0

        || SMR_W_UINT(m, &vdc.row_counter) < 0
        || SMR_W_UINT(m, &vdc.row_counter_y) < 0
        || SMR_W_UINT(m, &vdc.row_counter_latch) < 0
        || SMR_W_UINT(m, &vdc.attribute_offset) < 0
        || SMR_W_UINT(m, &vdc.display_enable) < 0
        || SMR_W_UINT(m, &vdc.prime_draw) < 0
        || SMR_W_UINT(m, &vdc.draw_active) < 0
        || SMR_W_UINT(m, &vdc.draw_finished) < 0
        || SMR_W_UINT(m, &vdc.vsync) < 0
        || SMR_W_UINT(m, &vdc.vsync_counter) < 0
        || SMR_W_UINT(m, &vdc.vsync_height) < 0
        || SMR_W_UINT(m, &vdc.draw_counter) < 0
        || SMR_W_UINT(m, &vdc.draw_counter_y) < 0
        || SMR_W_UINT(m, &vdc.draw_counter_latch) < 0
        || SMR_W_UINT(m, &vdc.vert_fine_adj) < 0
        || SMR_W_UINT(m, &vdc.stable_size_count) < 0
        || SMR_W_UINT(m, &vdc.old_screen_adr) < 0
        || SMR_W_UINT(m, &vdc.old_attribute_adr) < 0
        || SMR_W_UINT(m, &vdc.interlaced) < 0 ) {
        goto fail;
    }

    /* VDC Registers */
    for (i = 0; i < 0x40; i++) {
        if (SMR_B(m, &vdc.regs[i]) < 0) {
            goto fail;
        }
    }

    /* VDC RAM 64K */
    if (SMR_BA(m, vdc.ram, 0x10000) < 0) {
        goto fail;
    }

    /* VDC Raster */
    if (raster_snapshot_read(m, &vdc.raster)) {
        goto fail;
    }

    /* Misc fixes */
    vdc.update_geometry = 1;
    vdc.force_repaint = 1;
    vdc.force_resize = 1;

    /* Reset the border colour, for some reason this doesn't get restored in the raster_snapshot_read */
    vdc.raster.border_color = (vdc.regs[26] & 0x0F);

    /* Re-enable interlace in the renderer if needed */
    vdc.raster.canvas->videoconfig->interlaced = (vdc.regs[8] & 1);

    /* Reset the draw alarm handler so the vdc doesn't freeze up due to a clock mismatch */
    alarm_set(vdc.raster_draw_alarm, maincpu_clk + qw);

    snapshot_module_close(m);
    return 0;

fail:
    if (m != NULL) {
        snapshot_module_close(m);
    }
    return -1;
}
