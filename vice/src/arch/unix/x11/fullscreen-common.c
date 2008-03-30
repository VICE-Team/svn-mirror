/*
 * fullscreen-common.c
 *
 * Written by
 *  Martin Pottendorfer <pottendo@utanet.at>
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

#ifdef USE_XF86_EXTENSIONS

#include "fullscreen-common.h"
#ifdef USE_XF86_VIDMODE_EXT
#include "vidmode.h"
#endif
#ifdef USE_XF86_DGA2_EXTENSIONS
#include "fullscreen.h"
#endif

int fullscreen_is_enabled;
raster_t *fs_cached_raster;

int fullscreen_available(void) 
{
#ifdef USE_XF86_DGA2_EXTENSIONS
    if (dga2_available())
	return 1;
#endif    
#ifdef USE_XF86_VIDMODE_EXT
    if (vidmode_available())
	return 1;
#endif    
    return 0;
}

void fullscreen_set_raster(struct raster_s *raster)
{
    fs_cached_raster = raster;
}

#endif /* USE_XF86_VIDMODE_EXT */

