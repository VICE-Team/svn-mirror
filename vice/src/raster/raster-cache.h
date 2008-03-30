/*
 * raster-cache.h - Raster line cache.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#ifndef _RASTER_CACHE_H
#define _RASTER_CACHE_H

#include "types.h"

#include "raster-sprite-cache.h"



/* Yeah, static allocation sucks.  But it's faster, and we are not wasting
   much space anyway.  */
#define RASTER_CACHE_MAX_TEXTCOLS 0x100
#define RASTER_CACHE_MAX_SPRITES  8
#define RASTER_CACHE_GFX_MSK_SIZE 0x100



/* This defines the screen cache.  It includes the sprite cache too.  */
struct _raster_cache
  {
    /* Number of line shown (referred to drawable area) */
    int n;

    /* If nonzero, it means that the cache entry is invalid.  */
    int is_dirty;

    /* Bitmap representation of the graphics in foreground.  */
    BYTE foreground_data[RASTER_CACHE_MAX_TEXTCOLS];

    /* Color information.  */
    BYTE border_color;
    BYTE background_data[RASTER_CACHE_MAX_TEXTCOLS];

    /* This is needed in the VIC-II for the area between the end of the left
       border and the start of the graphics, when the X smooth scroll
       register is > 0.  */
    BYTE overscan_background_color;

    /* The following are generic and are used differently by the video
       emulators.  */
    BYTE color_data_1[RASTER_CACHE_MAX_TEXTCOLS];
    BYTE color_data_2[RASTER_CACHE_MAX_TEXTCOLS];
    BYTE color_data_3[RASTER_CACHE_MAX_TEXTCOLS];

    /* X smooth scroll offset.  */
    int xsmooth;

    /* Video mode.  */
    unsigned int video_mode;

    /* Blank mode flag.  */
    int blank;

    /* This defines the borders.  */
    int display_xstart, display_xstop;

    /* Number of columns enabled on this line.  */
    unsigned int numcols;

    /* Number of sprites on this line.  */
    unsigned int numsprites;

    /* Bit mask for the sprites that are visible on this line.  */
    unsigned int sprmask;

    /* Sprite cache.  */
    raster_sprite_cache_t sprites[RASTER_CACHE_MAX_SPRITES];
    BYTE gfx_msk[RASTER_CACHE_GFX_MSK_SIZE];

    /* Sprite-sprite and sprite-background collisions that were detected on
       this line.  */
    BYTE sprite_sprite_collisions;
    BYTE sprite_background_collisions;

    /* Character memory pointer.  */
    BYTE *chargen_ptr;

    /* Character row counter.  */
    unsigned int ycounter;

    /* Flags for open left/right borders.  */
    int open_right_border, open_left_border;
  };
typedef struct _raster_cache raster_cache_t;



void raster_cache_init (raster_cache_t * cache);
raster_cache_t *raster_cache_new (void);
void raster_cache_free (raster_cache_t * cache);



/* Inlined functions.  These need to be *fast*.  */

/* Read length bytes from src and store them in dest, checking for
   differences between the two arrays.  The smallest interval that contains
   different bytes is returned as [*xs; *xe].  */
/* _fill_cache() */
inline static int
raster_cache_data_fill (BYTE *dest,
			const BYTE *src,
			int length,
			int src_step,
			int *xs,
			int *xe,
			int no_check)
{
  if (no_check)
    {
      int i;

      *xs = 0;
      *xe = length - 1;
      if (src_step == 1)
	memcpy (dest, src, length);
      else
	for (i = 0; i < length; i++, src += src_step)
	  dest[i] = src[0];
      return 1;
    }
  else
    {
      int x = 0, i;

      for (i = 0; i < length && dest[i] == src[0]; i++, src += src_step)
	/* do nothing */ ;

      if (i < length)
	{
	  if (*xs > i)
	    *xs = i;

	  for (; i < length; i++, src += src_step)
	    if (dest[i] != src[0])
	      {
		dest[i] = src[0];
		x = i;
	      }

	  if (*xe < x)
	    *xe = x;

	  return 1;
	}
      else
	return 0;
    }
}

/* Do as _fill_cache(), but split each byte into low and high nibble.  These
   are stored into different destinations.  */
/* _fill_cache_nibbles */
inline static int
raster_cache_data_fill_nibbles (BYTE *dest_hi,
				BYTE *dest_lo,
				const BYTE *src,
				int length,
				int src_step,
				int *xs, int *xe,
				int no_check)
{
  if (no_check)
    {
      int i;

      *xs = 0;
      *xe = length - 1;

      for (i = 0; i < length; i++, src += src_step)
	{
	  dest_hi[i] = src[0] >> 4;
	  dest_lo[i] = src[0] & 0xf;
	}

      return 1;
    }
  else
    {
      int i, x = 0;
      BYTE b;

      for (i = 0;
	   dest_hi[i] == (src[0] >> 4)
	   && dest_lo[i] == (src[0] & 0xf) && i < length;
	   i++, src += src_step)
	/* do nothing */ ;

      if (i < length)
	{
	  if (*xs > i)
	    *xs = i;

	  for (; i < length; i++, src += src_step)
	    if (dest_hi[i] != (b = (src[0] >> 4)))
	      {
		dest_hi[i] = b;
		x = i;
	      }
	    else if (dest_lo[i] != (b = (src[0] & 0xf)))
	      {
		dest_lo[i] = b;
		x = i;
	      }

	  if (*xe < x)
	    *xe = x;

	  return 1;
	}
      else
	return 0;
    }
}

/* This function is used for text modes.  It checks for differences in the
   character memory too.  */
/* _fill_cache_text */
inline static int
raster_cache_data_fill_text (BYTE *dest,
			     const BYTE *src,
			     BYTE *char_mem,
			     int bytes_per_char,
			     int length,
			     int l,
			     int *xs, int *xe,
			     int no_check)
{
#define _GET_CHAR_DATA(c, l)  char_mem[((c) * bytes_per_char) + (l)]
  if (no_check)
    {
      int i;

      *xs = 0;
      *xe = length - 1;
      for (i = 0; i < length; i++, src++)
	dest[i] = _GET_CHAR_DATA (src[0], l);
      return 1;
    }
  else
    {
      BYTE b;
      int i;

      for (i = 0;
	   i < length && dest[i] == _GET_CHAR_DATA (src[0], l);
	   i++, src++)
	/* do nothing */ ;

      if (i < length)
	{
	  *xs = *xe = i;

	  for (; i < length; i++, src++)
	    if (dest[i] != (b = _GET_CHAR_DATA (src[0], l)))
	      {
		dest[i] = b;
		*xe = i;
	      }

	  return 1;
	}
      else
	return 0;
    }
#undef _GET_CHAR_DATA
}


/* This function is used for the VDC attributed text mode.  It checks for
   differences in the character memory too.
   This is just an _EXTREMLY UGLY HACK (TM)_ to make the VDC work
   (Markus Brenner) */

inline static int
raster_cache_data_fill_attr_text (BYTE *dest,
			     const BYTE *src,
                             BYTE *attr,
			     BYTE *char_mem,
			     int bytes_per_char,
			     int length,
			     int l,
			     int *xs, int *xe,
			     int no_check)
{
#define _GET_ATTR_CHAR_DATA(c, a, l) (((a) & 0x80)?char_mem+0x1000:char_mem)[((c) * bytes_per_char) + (l)]
  if (no_check)
    {
      int i;

      *xs = 0;
      *xe = length - 1;
      for (i = 0; i < length; i++, src++, attr++)
	dest[i] = _GET_ATTR_CHAR_DATA (src[0], attr[0], l);
      return 1;
    }
  else
    {
      BYTE b;
      int i;

      for (i = 0;
	   i < length && dest[i] == _GET_ATTR_CHAR_DATA (src[0], attr[0], l);
	   i++, src++, attr++)
	/* do nothing */ ;

      if (i < length)
	{
	  *xs = *xe = i;

	  for (; i < length; i++, src++, attr++)
	    if (dest[i] != (b = _GET_ATTR_CHAR_DATA (src[0], attr[0], l)))
	      {
		dest[i] = b;
		*xe = i;
	      }

	  return 1;
	}
      else
	return 0;
    }
#undef _GET_ATTR_CHAR_DATA
}

#endif
