/*
 * vdc-color.c - Colors for the VDC emulation.
 *
 * Written by
 *  groepaz <groepaz@gmx.net>
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

#include "vdctypes.h"
#include "vdc-color.h"
#include "vdc-resources.h"
#include "video.h"

/*
    FIXME: the VDC (CGA) colors are not evenly saturated, which
    means they can not be accurately defined with the current
    system
*/

/* base saturation of all colors except the grey tones */
#define VDC_SATURATION  150.0f

/* phase shift of all colors */

#define VDC_PHASE       -4.5f

/* chroma angles in UV space */

#define ANGLE_RED        112.5f
#define ANGLE_GRN       -135.0f
#define ANGLE_BLU          0.0f
#define ANGLE_ORN        -45.0f /* negative orange (orange is at +135.0 degree) */
#define ANGLE_BRN        157.5f

/* luminances */
#define VDC_LUMA_0      (128.0f*0.0f)
#define VDC_LUMA_1      (128.0f/3.0f)
#define VDC_LUMA_2      (128.0f/1.5f)
#define VDC_LUMA_3      (128.0f*1.0f)

/* the VDC palette converted to yuv space */

static video_cbm_color_t vdc_colors[VDC_NUM_COLORS]=
{
    { VDC_LUMA_0, ANGLE_BRN, -0, "Black"       }, /* 000000 */
    { VDC_LUMA_1, ANGLE_BRN,  0, "Medium Gray" }, /* 555555 */
    { VDC_LUMA_2, ANGLE_BLU,  1, "Blue"        }, /* 0000AA */
    { VDC_LUMA_3, ANGLE_BLU,  1, "Light Blue"  }, /* 5555FF */
    { VDC_LUMA_2, ANGLE_GRN,  1, "Green"       }, /* 00AA00 */
    { VDC_LUMA_3, ANGLE_GRN,  1, "Light Green" }, /* 55FF55 */
    { VDC_LUMA_2, ANGLE_RED, -1, "Cyan"        }, /* 00AAAA */
    { VDC_LUMA_3, ANGLE_RED, -1, "Light Cyan"  }, /* 55FFFF */
    { VDC_LUMA_2, ANGLE_RED,  1, "Red"         }, /* AA0000 */
    { VDC_LUMA_3, ANGLE_RED,  1, "Light Red"   }, /* FF5555 */
    { VDC_LUMA_2, ANGLE_GRN, -1, "Purple"      }, /* AA00AA */
    { VDC_LUMA_3, ANGLE_GRN, -1, "Light Purple"}, /* FF55FF */
    { VDC_LUMA_2, ANGLE_BRN,  1, "Brown"       }, /* AA5500 */
    { VDC_LUMA_3, ANGLE_BRN,  1, "Yellow"      }, /* FFFF55 */
    { VDC_LUMA_2, ANGLE_BRN, -0, "Light Gray"  }, /* AAAAAA */
    { VDC_LUMA_3, ANGLE_BRN,  0, "White"       }, /* FFFFFF */
};

static video_cbm_palette_t vdc_palette =
{
    VDC_NUM_COLORS,
    vdc_colors,
    VDC_SATURATION,
    VDC_PHASE
};

int vdc_color_update_palette(struct video_canvas_s *canvas)
{
    video_color_palette_internal(canvas, &vdc_palette);
    return video_color_update_palette(canvas);
}

