/*
 * ted-color.c - Colors for the MOS 6569 (VIC-II) emulation.
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *
 * Research about the YUV values by
 *  Philip Timmermann <pepto@pepto.de>
 *  John Selck <graham@cruise.de>
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

#include "machine.h"
#include "tedtypes.h"
#include "ted-color.h"
#include "ted-resources.h"
#include "video.h"


/* base saturation of all colors except the grey tones */

#define TED_SATURATION	48.0f

/* phase shift of all colors */

#define TED_PHASE		-4.5f

/* chroma angles in UV space */

#define ANGLE_RED	 112.5f
#define ANGLE_GRN	-135.0f
#define ANGLE_BLU	   0.0f
#define ANGLE_ORN	 -45.0f	/* negative orange (orange is at +135.0 degree) */
#define ANGLE_BRN	 157.5f

/* new luminances */

#define LUMN0	  0.0f
#define LUMN1	 56.0f
#define LUMN2	 74.0f
#define LUMN3	 92.0f
#define LUMN4	117.0f
#define LUMN5	128.0f
#define LUMN6	163.0f
#define LUMN7	199.0f
#define LUMN8	256.0f

/* default dithering */

static char vic_ii_color_dither[16]=
{
	0x00,0x0E,0x04,0x0C,
	0x08,0x04,0x04,0x0C,
	0x04,0x04,0x08,0x04,
	0x08,0x08,0x08,0x0C
};

/* the wellknown vic-ii palette used for 99% of all vic-ii chips */

static video_cbm_color_t ted_colors[TED_NUM_COLORS]=
{
    { LUMN0, ANGLE_ORN, -0, "Black"       },
    { LUMN8, ANGLE_BRN,  0, "White"       },
    { LUMN2, ANGLE_RED,  1, "Red"         },
    { LUMN6, ANGLE_RED, -1, "Cyan"        },
    { LUMN3, ANGLE_GRN, -1, "Purple"      },
    { LUMN5, ANGLE_GRN,  1, "Green"       },
    { LUMN1, ANGLE_BLU,  1, "Blue"        },
    { LUMN7, ANGLE_BLU, -1, "Yellow"      },
    { LUMN3, ANGLE_ORN, -1, "Orange"      },
    { LUMN1, ANGLE_BRN,  1, "Brown"       },
    { LUMN5, ANGLE_RED,  1, "Yellow-Green"},
    { LUMN2, ANGLE_RED, -1, "Pink"        },
    { LUMN4, ANGLE_GRN, -1, "Blue-Green"  },
    { LUMN7, ANGLE_GRN,  1, "Light Blue"  },
    { LUMN4, ANGLE_BLU,  1, "Dark Blue"   },
    { LUMN6, ANGLE_BLU, -1, "Light Green" }
};

static video_cbm_palette_t ted_palette=
{
	TED_NUM_COLORS,
	ted_colors,
	TED_SATURATION,
	TED_PHASE,
};

int ted_update_palette(void)
{
	video_color_set_palette(&ted_palette);
	return video_color_update_palette();
}
