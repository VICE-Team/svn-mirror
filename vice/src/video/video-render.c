/*
 * video-render.c - Implementation of framebuffer to physical screen copy
 *
 * Written by
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

#include "render1x1.h"
#include "render2x2.h"
#include "types.h"
#include "video-render.h"
#include "video-resources.h"

BYTE gammatable[1024+256+1024];
SDWORD  ytable[128];
SDWORD cbtable[128];
SDWORD crtable[128];

/*****************************************************************************/
/*****************************************************************************/

/* this function is the interface to the outer world */

int double_size_bad = 0; /* these two variables need to be removed */
int double_scan_bad = 0; /* dont forget to change vicii-resources.c when removing them */

int video_render_get_fake_pal_state(void)
{
	return video_resources.delayloop_emulation
               || video_resources.pal_emulation;
}

void video_render_main(DWORD *colortab, BYTE *src, BYTE *trg, int width,
                       int height, int xs, int ys, int xt, int yt, int pitchs,
                       int pitcht, int depth)
{
	int doublesize,doublescan,delayloop;

#ifndef VIDEO_REMOVE_2X
	doublesize=0;
	doublescan=0;
	delayloop=0;
#else /* VIDEO_REMOVE_2X */
	doublesize=double_size_bad;
	doublescan=double_scan_bad;
	delayloop=video_resources.delayloop_emulation;
#endif /* VIDEO_REMOVE_2X */

	if (width <= 0) return; /* some render routines don't like invalid width */

	if (delayloop)
	{
		if (doublesize)
		{
			switch (depth)
			{
			case 8:
				render_08_2x2_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 16:
				render_16_2x2_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 24:
				render_24_2x2_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 32:
				render_32_2x2_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			}
		}
		else
		{
			switch (depth)
			{
			case 8:
				render_08_1x1_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 16:
				render_16_1x1_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 24:
				render_24_1x1_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 32:
				render_32_1x1_08(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			}
		}
	}
	else
	{
		if (doublesize)
		{
			switch (depth)
			{
			case 8:
				render_08_2x2_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 16:
				render_16_2x2_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 24:
				render_24_2x2_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			case 32:
				render_32_2x2_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht,doublescan);
				break;
			}
		}
		else
		{
			switch (depth)
			{
			case 8:
				render_08_1x1_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 16:
				render_16_1x1_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 24:
				render_24_1x1_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			case 32:
				render_32_1x1_04(colortab,src,trg,width,height,xs,ys,xt,yt,pitchs,pitcht);
				break;
			}
		}
	}
}

