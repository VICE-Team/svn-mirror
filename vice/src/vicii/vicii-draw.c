/*
 * vicii-draw.c - Rendering for the MOS6569 (VIC-II) emulation.
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

#include "vice.h"

#include "vicii.h"

#include "vicii-draw.h"



#define GFX_MSK_LEFTBORDER_SIZE   ((VIC_II_MAX_SPRITE_WIDTH +           \
                                    VIC_II_SCREEN_BORDERWIDTH) / 8 + 1)
#define GFX_MSK_SIZE              ((VIC_II_SCREEN_WIDTH                 \
                                    + VIC_II_MAX_SPRITE_WIDTH) / 8 + 1)



/* The following tables are used to speed up the drawing.  We do not use
   multi-dimensional arrays as we can optimize better this way...  */

/* foreground(4) | background(4) | nibble(4) -> 4 pixels.  */
#ifdef AVOID_STATIC_ARRAYS
static PIXEL4 *hr_table;
#else
static PIXEL4 hr_table[16 * 16 * 16];
#endif

#ifdef VIC_II_NEED_2X
/* foreground(4) | background(4) | idx(2) | nibble(4) -> 4 pixels.  */
#ifdef AVOID_STATIC_ARRAYS
static PIXEL4 *hr_table_2x;
#else
static PIXEL4 hr_table_2x[16 * 16 * 2 * 16];
#endif
#endif

/* mc flag(1) | idx(2) | byte(8) -> index into double-pixel table.  */
#ifdef AVOID_STATIC_ARRAYS
static WORD *mc_table;
static WORD *mcmsktable;
#else
static WORD mc_table[2 * 4 * 256];
static WORD mcmsktable[512];
#endif



/* These functions draw the background from `start_pixel' to `end_pixel'.  */

static void
draw_std_background (unsigned int start_pixel,
                     unsigned int end_pixel)
{
  vid_memset (vic_ii.raster.frame_buffer_ptr + start_pixel,
              RASTER_PIXEL (&vic_ii.raster,
                            vic_ii.raster.overscan_background_color),
              end_pixel - start_pixel + 1);
}

#ifdef VIC_II_NEED_2X
static void
draw_std_background_2x (unsigned int start_pixel,
                        unsigned int end_pixel)
{
  vid_memset (vic_ii.raster.frame_buffer_ptr + 2 * start_pixel,
              RASTER_PIXEL (&vic_ii.raster,
                            vic_ii.raster.overscan_background_color),
              2 * (end_pixel - start_pixel + 1));
}
#endif



/* If unaligned 32-bit access is not allowed, the graphics is stored in a
   temporary aligned buffer, and later copied to the real frame buffer.  This
   is ugly, but should be hopefully faster than accessing 8 bits at a time
   anyway.  */

#ifndef ALLOW_UNALIGNED_ACCESS
static PIXEL4 _aligned_line_buffer[VIC_II_SCREEN_XPIX / 2 + 1];
static PIXEL *const aligned_line_buffer = (PIXEL *) _aligned_line_buffer;
#endif

/* Pointer to the start of the graphics area on the frame buffer.  */
#define GFX_PTR(pixel_width)                            \
    (vic_ii.raster.frame_buffer_ptr                            \
     + ((VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth)    \
        * (pixel_width)))

#ifdef ALLOW_UNALIGNED_ACCESS
#define ALIGN_DRAW_FUNC(name, xs, xe, gfx_msk_ptr, pixel_width) \
   name(GFX_PTR(pixel_width), (xs), (xe), (gfx_msk_ptr))
#else
#define ALIGN_DRAW_FUNC(name, xs, xe, gfx_msk_ptr, pixel_width)         \
   do                                                                   \
     {                                                                  \
       name (aligned_line_buffer, (xs), (xe), (gfx_msk_ptr));           \
       vid_memcpy (GFX_PTR(pixel_width) + (xs) * 8 * (pixel_width),     \
                   aligned_line_buffer + (xs) * 8 * (pixel_width),      \
                   ((xe) - (xs) + 1) * 8 * (pixel_width));              \
     }                                                                  \
   while (0)
#endif



/* FIXME: in the cache, we store the foreground bitmap values for the
   characters, but we do not use them when drawing and this is slow!  */

/* Standard text mode.  */

static int
get_std_text (raster_cache_t *cache,
              int *xs,
              int *xe,
              int rr)
{
  int r;

  if (vic_ii.raster.background_color != cache->background_data[0]
      || cache->chargen_ptr != vic_ii.chargen_ptr)
    {
      cache->background_data[0] = vic_ii.raster.background_color;
      cache->chargen_ptr = vic_ii.chargen_ptr;
      *xs = 0;
      *xe = VIC_II_SCREEN_TEXTCOLS;
      rr = 1;
    }

  r = raster_cache_data_fill_text (cache->foreground_data,
                                   vic_ii.vbuf,
                                   vic_ii.chargen_ptr,
                                   8,   /* FIXME */
                                   VIC_II_SCREEN_TEXTCOLS,
                                   vic_ii.raster.ycounter,
                                   xs, xe,
                                   rr);

  r |= raster_cache_data_fill (cache->color_data_1,
                               vic_ii.cbuf,
                               VIC_II_SCREEN_TEXTCOLS,
                               1,
                               xs, xe,
                               rr);

  if (!r)
    {
      vic_ii.sprite_sprite_collisions |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions |= cache->sprite_background_collisions;
    }

  return r;
}

inline static void
_draw_std_text (PIXEL *p,
                unsigned int xs,
                unsigned int xe,
                BYTE *gfx_msk_ptr)
{
  PIXEL4 *table_ptr;
  BYTE *char_ptr;
  unsigned int i;

  table_ptr = hr_table + (vic_ii.raster.background_color << 4);
  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  for (i = xs; i <= xe; i++)
    {
      PIXEL4 *ptr = table_ptr + (vic_ii.cbuf[i] << 8);
      int d = (*(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i)
               = *(char_ptr + vic_ii.vbuf[i] * 8));

      *((PIXEL4 *) p + i * 2) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + i * 2 + 1) = *(ptr + (d & 0xf));
    }
}

static void
draw_std_text_cached (raster_cache_t *cache,
                      unsigned int xs,
                      unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_std_text, xs, xe, cache->gfx_msk, 1);
}

static void
draw_std_text (void)
{
  ALIGN_DRAW_FUNC (_draw_std_text, 0,
                   VIC_II_SCREEN_TEXTCOLS - 1, vic_ii.raster.gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X
inline static void
_draw_std_text_2x (PIXEL *p,
                   unsigned int xs,
                   unsigned int xe,
                   BYTE *gfx_msk_ptr)
{
  PIXEL4 *table_ptr;
  BYTE *char_ptr;
  unsigned int i;

  table_ptr = hr_table_2x + (vic_ii.raster.background_color << 5);
  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  for (i = xs; i <= xe; i++)
    {
      PIXEL4 *ptr = table_ptr + (vic_ii.cbuf[i] << 9);
      int d = (*(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i)
               = *(char_ptr + vic_ii.vbuf[i] * 8));

      *((PIXEL4 *) p + i * 4) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + i * 4 + 1) = *(ptr + 0x10 + (d >> 4));
      *((PIXEL4 *) p + i * 4 + 2) = *(ptr + (d & 0xf));
      *((PIXEL4 *) p + i * 4 + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void
draw_std_text_cached_2x (raster_cache_t *cache,
                         unsigned int xs,
                         unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_std_text_2x, xs, xe, cache->gfx_msk, 2);
}

static void
draw_std_text_2x (void)
{
  ALIGN_DRAW_FUNC (_draw_std_text_2x, 0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 2);
}

#endif /* VIC_II_NEED_2X */

#define DRAW_STD_TEXT_BYTE(p, b, f)             \
  do                                            \
    {                                           \
      if ((b) & 0x80) *(p) = (f);               \
      if ((b) & 0x40) *((p) + 1) = (f);         \
      if ((b) & 0x20) *((p) + 2) = (f);         \
      if ((b) & 0x10) *((p) + 3) = (f);         \
      if ((b) & 0x08) *((p) + 4) = (f);         \
      if ((b) & 0x04) *((p) + 5) = (f);         \
      if ((b) & 0x02) *((p) + 6) = (f);         \
      if ((b) & 0x01) *((p) + 7) = (f);         \
    }                                           \
  while (0)

static void
draw_std_text_foreground (unsigned int start_char,
                          unsigned int end_char)
{
  unsigned int i;
  BYTE *char_ptr;
  PIXEL *p;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  p = (vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b;
      PIXEL f;

      b = char_ptr[vic_ii.vbuf[i] * 8];
      f = RASTER_PIXEL (&vic_ii.raster, vic_ii.cbuf[i]);

      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;
      DRAW_STD_TEXT_BYTE (p, b, f);
    }
}

#ifdef VIC_II_NEED_2X
static void
draw_std_text_foreground_2x (unsigned int start_char,
                             unsigned int end_char)
{
  unsigned int i;
  BYTE *char_ptr;
  PIXEL2 *p;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  p = ((PIXEL2 *) (vic_ii.raster.frame_buffer_ptr
                   + 2 * VIC_II_SCREEN_BORDERWIDTH
                   + 2 * vic_ii.raster.xsmooth)
       + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b;
      PIXEL2 f;

      b = char_ptr[vic_ii.vbuf[i] * 8];
      f = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i]);

      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;

      /* Notice that we are always aligned on 2-bytes boundaries here.  */
      DRAW_STD_TEXT_BYTE (p, b, f);
    }
}
#endif /* VIC_II_NEED_2X */



/* Hires Bitmap mode.  */

static int
get_hires_bitmap (raster_cache_t *cache,
                  int *xs,
                  int *xe,
                  int rr)
{
  int r = 0;

  r |= raster_cache_data_fill_nibbles (cache->color_data_1,
                                       cache->background_data,
                                       vic_ii.vbuf,
                                       VIC_II_SCREEN_TEXTCOLS,
                                       1,
                                       xs, xe,
                                       rr);
  r |= raster_cache_data_fill (cache->foreground_data,
                               (vic_ii.bitmap_ptr + vic_ii.memptr * 8
                                + vic_ii.raster.ycounter),
                               VIC_II_SCREEN_TEXTCOLS,
                               8,
                               xs, xe,
                               rr);

  if (!r)
    {
      vic_ii.sprite_sprite_collisions
        |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= cache->sprite_background_collisions;
    }

  return r;
}

inline static void
_draw_hires_bitmap (PIXEL *p,
                    unsigned int xs,
                    unsigned int xe,
                    BYTE *gfx_msk_ptr)
{
  BYTE *bmptr;
  unsigned int i, j;

  bmptr = vic_ii.bitmap_ptr;

  for (j = ((vic_ii.memptr << 3)
            + vic_ii.raster.ycounter + xs * 8) & 0x1fff,
         i = xs;
       i <= xe;
       i++, j = (j + 8) & 0x1fff)
    {
      PIXEL4 *ptr = hr_table + (vic_ii.vbuf[i] << 4);
      int d;

      d = *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = bmptr[j];
      *((PIXEL4 *) p + i * 2) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + i * 2 + 1) = *(ptr + (d & 0xf));
    }
}

static void
draw_hires_bitmap (void)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap, 0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 1);
}

static void
draw_hires_bitmap_cached (raster_cache_t *cache,
                          unsigned int xs,
                          unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap, xs, xe, cache->gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X

inline static void
_draw_hires_bitmap_2x (PIXEL *p,
                       unsigned int xs,
                       unsigned int xe,
                       BYTE *gfx_msk_ptr)
{
  BYTE *bmptr = vic_ii.bitmap_ptr;
  unsigned int i, j;

  for (j = ((vic_ii.memptr << 3)
            + vic_ii.raster.ycounter + xs * 8) & 0x1fff,
         i = xs;
       i <= xe;
       i++, j = (j + 8) & 0x1fff)
    {
      PIXEL4 *ptr;
      int d;

      ptr = hr_table_2x + (vic_ii.vbuf[i] << 5);
      d = *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = bmptr[j];
      *((PIXEL4 *) p + i * 4) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + i * 4 + 1) = *(ptr + 0x10 + (d >> 4));
      *((PIXEL4 *) p + i * 4 + 2) = *(ptr + (d & 0xf));
      *((PIXEL4 *) p + i * 4 + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void
draw_hires_bitmap_2x (void)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap_2x,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 2);
}

static void
draw_hires_bitmap_cached_2x (raster_cache_t *cache,
                             unsigned int xs,
                             unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap_2x, xs, xe, cache->gfx_msk, 2);
}

#endif /* VIC_II_NEED_2X */

static void
draw_hires_bitmap_foreground (unsigned int start_char,
                              unsigned int end_char)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap, start_char, end_char,
                   vic_ii.raster.gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X
static void
draw_hires_bitmap_foreground_2x (unsigned int start_char,
                                 unsigned int end_char)
{
  ALIGN_DRAW_FUNC (_draw_hires_bitmap_2x, start_char, end_char,
                   vic_ii.raster.gfx_msk, 2);
}
#endif /* VIC_II_NEED_2X */



/* Multicolor text mode.  */

static int
get_mc_text (raster_cache_t *cache,
             int *xs,
             int *xe,
             int rr)
{
  int r = 0;

  if (vic_ii.raster.background_color != cache->background_data[0]
      || cache->color_data_1[0] != vic_ii.regs[0x22]
      || cache->color_data_1[1] != vic_ii.regs[0x23]
      || cache->chargen_ptr != vic_ii.chargen_ptr)
    {
      cache->background_data[0] = vic_ii.raster.background_color;
      cache->color_data_1[0] = vic_ii.regs[0x22];
      cache->color_data_1[1] = vic_ii.regs[0x23];
      cache->chargen_ptr = vic_ii.chargen_ptr;
      *xs = 0;
      *xe = VIC_II_SCREEN_TEXTCOLS - 1;
      rr = 1;
    }

  r = raster_cache_data_fill_text (cache->foreground_data,
                                   vic_ii.vbuf,
                                   vic_ii.chargen_ptr,
                                   8,   /* FIXME */
                                   VIC_II_SCREEN_TEXTCOLS,
                                   vic_ii.raster.ycounter,
                                   xs, xe,
                                   rr);
  r |= raster_cache_data_fill (cache->color_data_3,
                               vic_ii.cbuf,
                               VIC_II_SCREEN_TEXTCOLS,
                               1,
                               xs, xe,
                               rr);

  if (!r)
    {
      vic_ii.sprite_sprite_collisions
        |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= cache->sprite_background_collisions;
    }

  return r;
}

inline static void
_draw_mc_text (PIXEL *p,
               unsigned int xs,
               unsigned int xe,
               BYTE *gfx_msk_ptr)
{
  PIXEL2 c[7];
  BYTE *char_ptr;
  unsigned int i;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  c[0] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.raster.background_color);
  c[1] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.ext_background_color[0]);
  c[2] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.ext_background_color[1]);
  *((PIXEL *) c + 8) = *((PIXEL *) c + 11)
    = RASTER_PIXEL (&vic_ii.raster, vic_ii.raster.background_color);

  for (i = xs; i <= xe; i++)
    {
      unsigned int d = *(char_ptr + vic_ii.vbuf[i] * 8);
      unsigned int k = (vic_ii.cbuf[i] & 0x8) << 5;

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = mcmsktable[k | d];

#ifdef ALLOW_UNALIGNED_ACCESS
      c[3] = *((PIXEL2 *) ((PIXEL *) c + 9))
        = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
#else
      c[3] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
      *((PIXEL *) c + 9) = *((PIXEL *) c + 10)
        = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
#endif

      *((PIXEL2 *) p + 4 * i) = c[mc_table[k | d]];
      *((PIXEL2 *) p + 4 * i + 1) = c[mc_table[0x200 + (k | d)]];
      *((PIXEL2 *) p + 4 * i + 2) = c[mc_table[0x400 + (k | d)]];
      *((PIXEL2 *) p + 4 * i + 3) = c[mc_table[0x600 + (k | d)]];
    }
}

static void
draw_mc_text (void)
{
  ALIGN_DRAW_FUNC (_draw_mc_text,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 1);
}

static void
draw_mc_text_cached (raster_cache_t *cache,
                     unsigned int xs,
                     unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_mc_text, xs, xe, cache->gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X

inline static void
_draw_mc_text_2x (PIXEL *p,
                  unsigned int xs,
                  unsigned int xe,
                  BYTE *gfx_msk_ptr)
{
  PIXEL4 c[7];
  unsigned int i;
  BYTE *char_ptr;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  c[0] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.raster.background_color);
  c[1] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.ext_background_color[0]);
  c[2] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.ext_background_color[1]);

  *((PIXEL2 *) c + 8) = *((PIXEL2 *) c + 11)
    = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.raster.background_color);

  for (i = xs; i <= xe; i++)
    {
      unsigned int d, k;

      d = *(char_ptr + vic_ii.vbuf[i] * 8);
      k = (vic_ii.cbuf[i] & 0x8) << 5;

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = mcmsktable[k | d];

#ifdef ALLOW_UNALIGNED_ACCESS
      c[3] = *((PIXEL4 *) ((PIXEL2 *) c + 9))
        = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
#else
      c[3] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
      *((PIXEL2 *) c + 9) = *((PIXEL2 *) c + 10)
        = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i] & 0x7);
#endif
      *((PIXEL4 *) p + 4 * i) = c[mc_table[k | d]];
      *((PIXEL4 *) p + 4 * i + 1) = c[mc_table[0x200 + (k | d)]];
      *((PIXEL4 *) p + 4 * i + 2) = c[mc_table[0x400 + (k | d)]];
      *((PIXEL4 *) p + 4 * i + 3) = c[mc_table[0x600 + (k | d)]];
    }
}

static void
draw_mc_text_2x (void)
{
  ALIGN_DRAW_FUNC (_draw_mc_text_2x,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 2);
}

static void
draw_mc_text_cached_2x (raster_cache_t *cache,
                        unsigned int xs,
                        unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_mc_text_2x,
                   xs, xe,
                   cache->gfx_msk, 2);
}
#endif

/* FIXME: aligned/unaligned versions.  */
#define DRAW_MC_BYTE(p, b, f1, f2, f3)          \
  do                                            \
    {                                           \
      if ((b) & 0x80)                           \
        {                                       \
          if ((b) & 0x40)                       \
            *(p) = *((p) + 1) = (f3);           \
          else                                  \
            *(p) = *((p) + 1) = (f2);           \
        }                                       \
      else if ((b) & 0x40)                      \
        *(p) = *((p) + 1) = (f1);               \
                                                \
      if ((b) & 0x20)                           \
        {                                       \
          if ((b) & 0x10)                       \
            *((p) + 2) = *((p) + 3) = (f3);     \
          else                                  \
            *((p) + 2) = *((p) + 3) = (f2);     \
        }                                       \
      else if ((b) & 0x10)                      \
          *((p) + 2) = *((p) + 3) = (f1);       \
                                                \
      if ((b) & 0x08)                           \
        {                                       \
          if ((b) & 0x04)                       \
            *((p) + 4) = *((p) + 5) = (f3);     \
          else                                  \
            *((p) + 4) = *((p) + 5) = (f2);     \
        }                                       \
      else if ((b) & 0x04)                      \
          *((p) + 4) = *((p) + 5) = (f1);       \
                                                \
      if ((b) & 0x02)                           \
        {                                       \
          if ((b) & 0x01)                       \
            *((p) + 6) = *((p) + 7) = (f3);     \
          else                                  \
            *((p) + 6) = *((p) + 7) = (f2);     \
        }                                       \
      else if ((b) & 0x01)                      \
          *((p) + 6) = *((p) + 7) = (f1);       \
    }                                           \
  while (0)

static void
draw_mc_text_foreground (unsigned int start_char,
                         unsigned int end_char)
{
  BYTE *char_ptr;
  PIXEL c1, c2;
  PIXEL *p;
  unsigned int i;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  c1 = RASTER_PIXEL (&vic_ii.raster, vic_ii.ext_background_color[0]);
  c2 = RASTER_PIXEL (&vic_ii.raster, vic_ii.ext_background_color[1]);
  p = (vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b, c;

      b = *(char_ptr + vic_ii.vbuf[i] * 8);
      c = vic_ii.cbuf[i];

      if (c & 0x8)
        {
          PIXEL c3;

          c3 = RASTER_PIXEL (&vic_ii.raster, c & 0x7);
          DRAW_MC_BYTE (p, b, c1, c2, c3);
          *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i)
            = mcmsktable[0x100 + b];
        }
      else
        {
          PIXEL c3;

          c3 = RASTER_PIXEL (&vic_ii.raster, c);
          DRAW_STD_TEXT_BYTE (p, b, c3);
          *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;
        }
    }
}

#ifdef VIC_II_NEED_2X

static void
draw_mc_text_foreground_2x (unsigned int start_char,
                            unsigned int end_char)
{
  BYTE *char_ptr;
  PIXEL2 c1, c2;
  PIXEL2 *p;
  unsigned int i;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  c1 = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.ext_background_color[0]);
  c2 = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.ext_background_color[1]);
  p = ((PIXEL2 *) (vic_ii.raster.frame_buffer_ptr
                   + 2 * VIC_II_SCREEN_BORDERWIDTH
                   + 2 * vic_ii.raster.xsmooth)
       + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b = *(char_ptr + vic_ii.vbuf[i] * 8);
      BYTE c = vic_ii.cbuf[i];

      if (c & 0x8)
        {
          PIXEL2 c3;

          c3 = RASTER_PIXEL2 (&vic_ii.raster, c & 0x7);
          DRAW_MC_BYTE (p, b, c1, c2, c3);
          *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i)
            = mcmsktable[0x100 + b];
        }
      else
        {
          PIXEL2 c3;

          c3 = RASTER_PIXEL2 (&vic_ii.raster, c);
          DRAW_STD_TEXT_BYTE (p, b, c3);
          *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;
        }
    }
}

#endif /* VIC_II_NEED_2X */



/* Multicolor Bitmap Mode.  */

static int
get_mc_bitmap (raster_cache_t *cache,
               int *xs,
               int *xe,
               int r)
{
  if (vic_ii.raster.background_color != cache->background_data[0])
    {
      cache->background_data[0] = vic_ii.raster.background_color;
      r = 1;
      *xs = 0;
      *xe = VIC_II_SCREEN_TEXTCOLS;
    }

  r = raster_cache_data_fill_nibbles (cache->color_data_1,
                                      cache->color_data_2,
                                      vic_ii.vbuf,
                                      VIC_II_SCREEN_TEXTCOLS,
                                      1,
                                      xs, xe,
                                      r);
  r = raster_cache_data_fill (cache->color_data_3,
                              vic_ii.cbuf,
                              VIC_II_SCREEN_TEXTCOLS,
                              1,
                              xs, xe,
                              r);
  r = raster_cache_data_fill (cache->foreground_data,
                              (vic_ii.bitmap_ptr + 8 * vic_ii.memptr
                               + vic_ii.raster.ycounter),
                              VIC_II_SCREEN_TEXTCOLS,
                              8,
                              xs, xe,
                              r);

  if (!r)
    {
      vic_ii.sprite_sprite_collisions
        |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= cache->sprite_background_collisions;
    }
  return r;
}

inline static void
_draw_mc_bitmap (PIXEL *p,
                 unsigned int xs,
                 unsigned int xe,
                 BYTE *gfx_msk_ptr)
{
  BYTE *colptr, *bmptr;
  PIXEL2 c[4];
  unsigned int i, j;

  colptr = vic_ii.cbuf;
  bmptr = vic_ii.bitmap_ptr;

  c[0] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.raster.background_color);

  for (j = ((vic_ii.memptr << 3) + vic_ii.raster.ycounter + xs * 8) & 0x1fff,
         i = xs;
       i <= xe;
       i++, j = (j + 8) & 0x1fff)
    {
      unsigned int d;

      d = bmptr[j];

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = mcmsktable[d | 0x100];

      c[1] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.vbuf[i] >> 4);
      c[2] = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.vbuf[i] & 0xf);
      c[3] = RASTER_PIXEL2 (&vic_ii.raster, colptr[i]);

      *((PIXEL2 *) p + 4 * i) = c[mc_table[0x100 + d]];
      *((PIXEL2 *) p + 4 * i + 1) = c[mc_table[0x300 + d]];
      *((PIXEL2 *) p + 4 * i + 2) = c[mc_table[0x500 + d]];
      *((PIXEL2 *) p + 4 * i + 3) = c[mc_table[0x700 + d]];
    }
}

static void
draw_mc_bitmap (void)
{
  ALIGN_DRAW_FUNC (_draw_mc_bitmap,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 1);
}

static void
draw_mc_bitmap_cached (raster_cache_t *cache, unsigned int xs, unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_mc_bitmap, xs, xe, cache->gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X

inline static void
_draw_mc_bitmap_2x (PIXEL *p,
                    unsigned int xs,
                    unsigned int xe,
                    BYTE *gfx_msk_ptr)
{
  BYTE *colptr, *bmptr;
  PIXEL4 c[4];
  unsigned int i, j;

  colptr = vic_ii.cbuf;
  bmptr = vic_ii.bitmap_ptr;

  c[0] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.raster.background_color);

  for (j = ((vic_ii.memptr << 3) + vic_ii.raster.ycounter + xs * 8) & 0x1fff,
         i = xs;
       i <= xe;
       j = (j + 8) & 0x1fff, i++)
    {
      unsigned int d;

      d = bmptr[j];

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = mcmsktable[d | 0x100];

      c[1] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.vbuf[i] >> 4);
      c[2] = RASTER_PIXEL4 (&vic_ii.raster, vic_ii.vbuf[i] & 0xf);
      c[3] = RASTER_PIXEL4 (&vic_ii.raster, colptr[i]);

      *((PIXEL4 *) p + 4 * i) = c[mc_table[0x100 + d]];
      *((PIXEL4 *) p + 4 * i + 1) = c[mc_table[0x300 + d]];
      *((PIXEL4 *) p + 4 * i + 2) = c[mc_table[0x500 + d]];
      *((PIXEL4 *) p + 4 * i + 3) = c[mc_table[0x700 + d]];
    }
}

static void
draw_mc_bitmap_2x (void)
{
  ALIGN_DRAW_FUNC (_draw_mc_bitmap_2x,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 2);
}

static void
draw_mc_bitmap_cached_2x (raster_cache_t *cache,
                          unsigned int xs,
                          unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_mc_bitmap_2x, xs, xe, cache->gfx_msk, 2);
}

#endif /* VIC_II_NEED_2X */

static void
draw_mc_bitmap_foreground (unsigned int start_char,
                           unsigned int end_char)
{
  PIXEL *p;
  BYTE *bmptr;
  unsigned int i, j;

  p = (vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth + 8 * start_char);
  bmptr = vic_ii.bitmap_ptr;

  for (j = ((vic_ii.memptr << 3)
            + vic_ii.raster.ycounter + 8 * start_char) & 0x1fff,
         i = start_char;
       i <= end_char;
       j = (j + 8) & 0x1fff, i++, p += 8)
    {
      PIXEL c1, c2, c3;
      BYTE b;

      c1 = RASTER_PIXEL (&vic_ii.raster, vic_ii.vbuf[i] >> 4);
      c2 = RASTER_PIXEL (&vic_ii.raster, vic_ii.vbuf[i] & 0xf);
      c3 = RASTER_PIXEL (&vic_ii.raster, vic_ii.cbuf[i]);
      b = bmptr[j];

      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i)
        = mcmsktable[0x100 + b];
      DRAW_MC_BYTE (p, b, c1, c2, c3);
    }
}

#ifdef VIC_II_NEED_2X

static void
draw_mc_bitmap_foreground_2x (unsigned int start_char, unsigned int end_char)
{
  PIXEL2 *p;
  BYTE *bmptr;
  unsigned int i, j;

  p = ((PIXEL2 *) vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth + 8 * start_char);
  bmptr = vic_ii.bitmap_ptr;

  for (j = ((vic_ii.memptr << 3)
            + vic_ii.raster.ycounter + 8 * start_char) & 0x1fff,
         i = start_char;
       i <= end_char;
       j = (j + 8) & 0x1fff, i++, p += 8)
    {
      PIXEL2 c1, c2, c3;
      BYTE b;

      c1 = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.vbuf[i] >> 4);
      c2 = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.vbuf[i] & 0xf);
      c3 = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i]);
      b = bmptr[j];

      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i)
        = mcmsktable[0x100 + b];
      DRAW_MC_BYTE (p, b, c1, c2, c3);
    }
}

#endif /* VIC_II_NEED_2X */



/* Extended Text Mode.  */

static int
get_ext_text (raster_cache_t *cache, int *xs, int *xe, int r)
{
  if (r
      || vic_ii.regs[0x21] != cache->color_data_2[0]
      || vic_ii.regs[0x22] != cache->color_data_2[1]
      || vic_ii.regs[0x23] != cache->color_data_2[2]
      || vic_ii.regs[0x24] != cache->color_data_2[3])
    {
      cache->color_data_2[0] = vic_ii.regs[0x21];
      cache->color_data_2[1] = vic_ii.regs[0x22];
      cache->color_data_2[2] = vic_ii.regs[0x23];
      cache->color_data_2[3] = vic_ii.regs[0x24];
      r = 1;
    }

  r = raster_cache_data_fill (cache->color_data_1,
                              vic_ii.cbuf,
                              VIC_II_SCREEN_TEXTCOLS,
                              1,
                              xs, xe,
                              r);
  r = raster_cache_data_fill (cache->color_data_2,
                              vic_ii.vbuf,
                              VIC_II_SCREEN_TEXTCOLS,
                              1,
                              xs, xe,
                              r);
  r = raster_cache_data_fill (cache->foreground_data,
                              vic_ii.vbuf,
                              VIC_II_SCREEN_TEXTCOLS,
                              1,
                              xs, xe,
                              r);

  if (!r)
    {
      vic_ii.sprite_sprite_collisions
        |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= cache->sprite_background_collisions;
    }

  return r;
}

inline static void
_draw_ext_text (PIXEL *p,
                unsigned int xs,
                unsigned int xe,
                BYTE *gfx_msk_ptr)
{
  BYTE *char_ptr;
  unsigned int i;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  for (i = xs; i <= xe; i++)
    {
      PIXEL4 *ptr;
      int bg_idx;
      int d;

      ptr = hr_table + (vic_ii.cbuf[i] << 8);
      bg_idx = vic_ii.vbuf[i] >> 6;
      d = *(char_ptr + (vic_ii.vbuf[i] & 0x3f) * 8);

      if (bg_idx == 0)
        ptr += vic_ii.raster.background_color << 4;
      else
        ptr += vic_ii.ext_background_color[bg_idx - 1] << 4;

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = d;
      *((PIXEL4 *) p + 2 * i) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + 2 * i + 1) = *(ptr + (d & 0xf));
    }
}

static void
draw_ext_text (void)
{
  ALIGN_DRAW_FUNC (_draw_ext_text,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 1);
}

static void
draw_ext_text_cached (raster_cache_t *cache, unsigned int xs, unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_ext_text, xs, xe, cache->gfx_msk, 1);
}

#ifdef VIC_II_NEED_2X

inline static void
_draw_ext_text_2x (PIXEL *p,
                   unsigned int xs,
                   unsigned int xe,
                   BYTE *gfx_msk_ptr)
{
  BYTE *char_ptr;
  unsigned int i;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;

  for (i = xs; i <= xe; i++)
    {
      PIXEL4 *ptr;
      int bg_idx, d;

      ptr = hr_table_2x + (vic_ii.cbuf[i] << 9);
      bg_idx = vic_ii.vbuf[i] >> 6;
      d = *(char_ptr + (vic_ii.vbuf[i] & 0x3f) * 8);

      if (bg_idx == 0)
        ptr += vic_ii.raster.background_color << 5;
      else
        ptr += vic_ii.ext_background_color[bg_idx - 1] << 5;

      *(gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE + i) = d;
      *((PIXEL4 *) p + 4 * i) = *(ptr + (d >> 4));
      *((PIXEL4 *) p + 4 * i + 1) = *(ptr + 0x10 + (d >> 4));
      *((PIXEL4 *) p + 4 * i + 2) = *(ptr + (d & 0xf));
      *((PIXEL4 *) p + 4 * i + 3) = *(ptr + 0x10 + (d & 0xf));
    }
}

static void
draw_ext_text_cached_2x (raster_cache_t *cache,
                         unsigned int xs,
                         unsigned int xe)
{
  ALIGN_DRAW_FUNC (_draw_ext_text_2x, xs, xe, cache->gfx_msk, 2);
}

static void
draw_ext_text_2x (void)
{
  ALIGN_DRAW_FUNC (_draw_ext_text_2x,
                   0, VIC_II_SCREEN_TEXTCOLS - 1,
                   vic_ii.raster.gfx_msk, 2);
}

#endif /* VIC_II_NEED_2X */

/* FIXME: This is *slow* and might not be 100% correct.  */
static void
draw_ext_text_foreground (unsigned int start_char,
                          unsigned int end_char)
{
  unsigned int i;
  BYTE *char_ptr;
  PIXEL *p;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  p = (vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b;
      PIXEL f;
      int bg_idx;

      b = char_ptr[(vic_ii.vbuf[i] & 0x3f) * 8];
      f = RASTER_PIXEL (&vic_ii.raster, vic_ii.cbuf[i]);
      bg_idx = vic_ii.vbuf[i] >> 6;

      if (bg_idx > 0)
        {
#ifdef ALLOW_UNALIGNED_ACCESS
          *((PIXEL4 *) p) = *((PIXEL4 *) p + 1) =
            RASTER_PIXEL4 (&vic_ii.raster,
                           vic_ii.ext_background_color[bg_idx - 1]);
#else
          p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] =
            RASTER_PIXEL (&vic_ii.raster,
                          vic_ii.ext_background_color[bg_idx - 1]);
#endif
        }

      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;
      DRAW_STD_TEXT_BYTE (p, b, f);
    }
}

#ifdef VIC_II_NEED_2X
static void
draw_ext_text_foreground_2x (unsigned int start_char,
                             unsigned int end_char)
{
  unsigned int i;
  BYTE *char_ptr;
  PIXEL2 *p;

  char_ptr = vic_ii.chargen_ptr + vic_ii.raster.ycounter;
  p = ((PIXEL2 *) (vic_ii.raster.frame_buffer_ptr
                   + 2 * VIC_II_SCREEN_BORDERWIDTH
                   + 2 * vic_ii.raster.xsmooth)
       + 8 * start_char);

  for (i = start_char; i <= end_char; i++, p += 8)
    {
      BYTE b;
      PIXEL2 f;
      int bg_idx;

      f = RASTER_PIXEL2 (&vic_ii.raster, vic_ii.cbuf[i]);
      b = char_ptr[(vic_ii.vbuf[i] & 0x3f) * 8];
      bg_idx = vic_ii.vbuf[i] >> 6;

      if (bg_idx > 0)
        {
#ifdef ALLOW_UNALIGNED_ACCESS
          *((PIXEL4 *) p) = *((PIXEL4 *) p + 1)
            = *((PIXEL4 *) p + 2) = *((PIXEL4 *) p + 3)
            = RASTER_PIXEL4 (&vic_ii.raster,
                             vic_ii.ext_background_color[bg_idx - 1]);
#else
          p[0] = p[1] = p[2] = p[3] = p[4] = p[5] = p[6] = p[7] =
            RASTER_PIXEL2 (&vic_ii.raster,
                           vic_ii.ext_background_color[bg_idx - 1]);
#endif
        }
      *(vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE + i) = b;
      DRAW_STD_TEXT_BYTE (p, b, f);
    }
}
#endif



/* Illegal mode.  Everything is black.  */

static int
get_black (raster_cache_t *cache, int *xs, int *xe, int r)
{
  /* Let's simplify here: if also the previous time we had the Black Mode,
     nothing has changed.  If we had not, the whole line has changed.  */

  if (r)
    {
      *xs = 0;
      *xe = VIC_II_SCREEN_TEXTCOLS - 1;
    }
  else
    {
      vic_ii.sprite_sprite_collisions
        |= cache->sprite_sprite_collisions;
      vic_ii.sprite_background_collisions
        |= cache->sprite_background_collisions;
    }

  return r;
}

static void
draw_black (void)
{
  PIXEL *p;

  p = (vic_ii.raster.frame_buffer_ptr
       + ((VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth)
          * vic_ii.raster.viewport.pixel_size.width));

  vid_memset (p, RASTER_PIXEL (&vic_ii.raster, 0),
         VIC_II_SCREEN_TEXTCOLS * 8 * vic_ii.raster.viewport.pixel_size.width);

  /* FIXME: this is not exact! */
  memset (vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE,
          0, VIC_II_SCREEN_TEXTCOLS);
}

static void
draw_black_cached (raster_cache_t *cache, unsigned int xs, unsigned int xe)
{
  PIXEL *p;

  p = (vic_ii.raster.frame_buffer_ptr
       + ((VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth)
          * vic_ii.raster.viewport.pixel_size.width));

  vid_memset (p, RASTER_PIXEL (&vic_ii.raster, 0),
      VIC_II_SCREEN_TEXTCOLS * 8 * vic_ii.raster.viewport.pixel_size.width);
  memset (vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE,
          0, VIC_II_SCREEN_TEXTCOLS);
}

static void
draw_black_foreground (unsigned int start_char, unsigned int end_char)
{
  PIXEL *p;

  p = (vic_ii.raster.frame_buffer_ptr
       + (VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth +
          8 * start_char) * vic_ii.raster.viewport.pixel_size.width);

  vid_memset (p, RASTER_PIXEL (&vic_ii.raster, 0),
    (end_char - start_char + 1) * 8 * vic_ii.raster.viewport.pixel_size.width);
  memset (vic_ii.raster.gfx_msk + GFX_MSK_LEFTBORDER_SIZE,
          0, VIC_II_SCREEN_TEXTCOLS);
}



/* Idle state.  */

static int
get_idle (raster_cache_t *cache, int *xs, int *xe, int rr)
{
  if (rr
      || vic_ii.raster.background_color != cache->color_data_1[0]
      || vic_ii.idle_data != cache->foreground_data[0])
    {
      cache->color_data_1[0] = vic_ii.raster.background_color;
      cache->foreground_data[0] = (BYTE) vic_ii.idle_data;
      *xs = 0;
      *xe = VIC_II_SCREEN_TEXTCOLS - 1;
      return 1;
    }
  else
    return 0;
}

inline static void
_draw_idle (unsigned int xs, unsigned int xe,
            int pixel_width,
            BYTE *gfx_msk_ptr)
{
  PIXEL *p;
  BYTE d;
  unsigned int i;

  d = (BYTE) vic_ii.idle_data;

#ifdef ALLOW_UNALIGNED_ACCESS
  p = (vic_ii.raster.frame_buffer_ptr
       + (VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth) * pixel_width);
#else
  p = aligned_line_buffer;
#endif

  if (VIC_II_IS_ILLEGAL_MODE (vic_ii.raster.video_mode))
    vid_memset (p, RASTER_PIXEL (&vic_ii.raster, 0),
                VIC_II_SCREEN_XPIX * pixel_width);
  else if (pixel_width == 1)
    {
      /* The foreground color is always black (0).  */
      unsigned int offs;
      PIXEL4 c1, c2;

      offs = vic_ii.raster.overscan_background_color << 4;
      c1 = *(hr_table + offs + (d >> 4));
      c2 = *(hr_table + offs + (d & 0xf));

      for (i = xs * 8; i <= xe * 8; i += 8)
        {
          *((PIXEL4 *) (p + i)) = c1;
          *((PIXEL4 *) (p + i + 4)) = c2;
        }
    }
#ifdef VIC_II_NEED_2X
  else if (pixel_width == 2)
    {
      unsigned int offs;
      PIXEL4 c1, c2, c3, c4;

      /* The foreground color is always black (0).  */
      offs = vic_ii.raster.overscan_background_color << 5;
      c1 = *(hr_table_2x + offs + (d >> 4));
      c2 = *(hr_table_2x + 0x10 + offs + (d >> 4));
      c3 = *(hr_table_2x + offs + (d & 0xf));
      c4 = *(hr_table_2x + 0x10 + offs + (d & 0xf));

      for (i = xs * 16; i <= xe * 16; i += 16)
        {
          *((PIXEL4 *) (p + i)) = c1;
          *((PIXEL4 *) (p + i + 4)) = c2;
          *((PIXEL4 *) (p + i + 8)) = c3;
          *((PIXEL4 *) (p + i + 12)) = c4;
        }
    }
#endif

#ifndef ALLOW_UNALIGNED_ACCESS
  vid_memcpy (vic_ii.raster.frame_buffer_ptr + (VIC_II_SCREEN_BORDERWIDTH
                                  + vic_ii.raster.xsmooth) * pixel_width,
              aligned_line_buffer + xs * 8 * pixel_width,
              (xe - xs + 1) * 8 * pixel_width);
#endif

  memset (gfx_msk_ptr + GFX_MSK_LEFTBORDER_SIZE, d, VIC_II_SCREEN_TEXTCOLS);
}

static void
draw_idle (void)
{
  _draw_idle (0, VIC_II_SCREEN_TEXTCOLS - 1, 1, vic_ii.raster.gfx_msk);
}

static void
draw_idle_cached (raster_cache_t *cache,
                  unsigned int xs,
                  unsigned int xe)
{
  _draw_idle (xs, xe, 1, cache->gfx_msk);
}

#ifdef VIC_II_NEED_2X

static void
draw_idle_2x (void)
{
  _draw_idle (0, VIC_II_SCREEN_TEXTCOLS - 1, 2, vic_ii.raster.gfx_msk);
}

static void
draw_idle_cached_2x (raster_cache_t *cache,
                     unsigned int xs,
                     unsigned int xe)
{
  _draw_idle (xs, xe, 2, cache->gfx_msk);
}

#endif /* VIC_II_NEED_2X */

static void
draw_idle_foreground (unsigned int start_char,
                      unsigned int end_char)
{
  PIXEL *p;
  PIXEL c;
  BYTE d;
  unsigned int i;

  p = (vic_ii.raster.frame_buffer_ptr + VIC_II_SCREEN_BORDERWIDTH
       + vic_ii.raster.xsmooth);
  c = RASTER_PIXEL (&vic_ii.raster, 0);
  d = (BYTE) vic_ii.idle_data;

  for (i = start_char; i <= end_char; i++)
    {
      DRAW_STD_TEXT_BYTE (p + i * 8, d, c);
      vic_ii.raster.gfx_msk[GFX_MSK_LEFTBORDER_SIZE + i] = d;
    }
}

#ifdef VIC_II_NEED_2X

static void
draw_idle_foreground_2x (unsigned int start_char,
                         unsigned int end_char)
{
  PIXEL2 *p;
  PIXEL2 c;
  unsigned int i, d;

  p = ((PIXEL2 *) vic_ii.raster.frame_buffer_ptr
       + VIC_II_SCREEN_BORDERWIDTH + vic_ii.raster.xsmooth);
  c = RASTER_PIXEL2 (&vic_ii.raster, 0);
  d = (BYTE) vic_ii.idle_data;

  for (i = start_char; i <= end_char; i++)
    {
      DRAW_STD_TEXT_BYTE (p + i * 8, d, c);
      vic_ii.raster.gfx_msk[GFX_MSK_LEFTBORDER_SIZE + i] = d;
    }
}

#endif /* VIC_II_NEED_2X */



#ifdef VIC_II_NEED_2X

static void
setup_double_size_modes (void)
{
  raster_modes_set (&vic_ii.raster.modes, VIC_II_NORMAL_TEXT_MODE,
                    get_std_text,
                    draw_std_text_cached_2x,
                    draw_std_text_2x,
                    draw_std_background_2x,
                    draw_std_text_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_MULTICOLOR_TEXT_MODE,
                    get_mc_text,
                    draw_mc_text_cached_2x,
                    draw_mc_text_2x,
                    draw_std_background_2x,
                    draw_mc_text_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_HIRES_BITMAP_MODE,
                    get_hires_bitmap,
                    draw_hires_bitmap_cached_2x,
                    draw_hires_bitmap_2x,
                    draw_std_background_2x,
                    draw_hires_bitmap_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_MULTICOLOR_BITMAP_MODE,
                    get_mc_bitmap,
                    draw_mc_bitmap_cached_2x,
                    draw_mc_bitmap_2x,
                    draw_std_background_2x,
                    draw_mc_bitmap_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_EXTENDED_TEXT_MODE,
                    get_ext_text,
                    draw_ext_text_cached_2x,
                    draw_ext_text_2x,
                    draw_std_background_2x,
                    draw_ext_text_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_IDLE_MODE,
                    get_idle,
                    draw_idle_cached_2x,
                    draw_idle_2x,
                    draw_std_background_2x,
                    draw_idle_foreground_2x);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_TEXT_MODE,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background_2x,
                    draw_black_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_BITMAP_MODE_1,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background_2x,
                    draw_black_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_BITMAP_MODE_2,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background_2x,
                    draw_black_foreground);
}

#endif /* VIC_II_NEED_2X */

static void
setup_single_size_modes (void)
{
  raster_modes_set (&vic_ii.raster.modes, VIC_II_NORMAL_TEXT_MODE,
                    get_std_text,
                    draw_std_text_cached,
                    draw_std_text,
                    draw_std_background,
                    draw_std_text_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_MULTICOLOR_TEXT_MODE,
                    get_mc_text,
                    draw_mc_text_cached,
                    draw_mc_text,
                    draw_std_background,
                    draw_mc_text_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_HIRES_BITMAP_MODE,
                    get_hires_bitmap,
                    draw_hires_bitmap_cached,
                    draw_hires_bitmap,
                    draw_std_background,
                    draw_hires_bitmap_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_MULTICOLOR_BITMAP_MODE,
                    get_mc_bitmap,
                    draw_mc_bitmap_cached,
                    draw_mc_bitmap,
                    draw_std_background,
                    draw_mc_bitmap_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_EXTENDED_TEXT_MODE,
                    get_ext_text,
                    draw_ext_text_cached,
                    draw_ext_text,
                    draw_std_background,
                    draw_ext_text_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_IDLE_MODE,
                    get_idle,
                    draw_idle_cached,
                    draw_idle,
                    draw_std_background,
                    draw_idle_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_TEXT_MODE,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background,
                    draw_black_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_BITMAP_MODE_1,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background,
                    draw_black_foreground);

  raster_modes_set (&vic_ii.raster.modes, VIC_II_ILLEGAL_BITMAP_MODE_2,
                    get_black,
                    draw_black_cached,
                    draw_black,
                    draw_std_background,
                    draw_black_foreground);
}



/* Initialize the drawing tables.  */
static void
init_drawing_tables (void)
{
  DWORD i;
  unsigned int f, b;
  WORD wmsk;
  DWORD lmsk;
  char tmptable[4] = { 0, 4, 5, 3 };

#ifdef AVOID_STATIC_ARRAYS
  if (!hr_table)
    {
      hr_table = xmalloc (sizeof (*hr_table) * 16 * 16 * 16);
#ifdef VIC_II_NEED_2X
      hr_table_2x = xmalloc (sizeof (*hr_table_2x) * 16 * 16 * 2 * 16);
#endif
      mc_table = xmalloc (sizeof (*mc_table) * 2 * 4 * 256);
      sprite_doubling_table = xmalloc (sizeof (*sprite_doubling_table) * 65536);
      mcmsktable = xmalloc (sizeof (*mcmsktable) * 512);
    }
#endif

  for (i = 0; i <= 0xf; i++)
    {
      for (f = 0; f <= 0xf; f++)
        {
          for (b = 0; b <= 0xf; b++)
            {
              PIXEL fp, bp;
              PIXEL *p;
              int offset;

              fp = RASTER_PIXEL (&vic_ii.raster, f);
              bp = RASTER_PIXEL (&vic_ii.raster, b);
              offset = (f << 8) | (b << 4);
              p = (PIXEL *) (hr_table + offset + i);

              *p = i & 0x8 ? fp : bp;
              *(p + 1) = i & 0x4 ? fp : bp;
              *(p + 2) = i & 0x2 ? fp : bp;
              *(p + 3) = i & 0x1 ? fp : bp;

#ifdef VIC_II_NEED_2X
              p = (PIXEL *) (hr_table_2x + (offset << 1) + i);
              *p = *(p + 1) = i & 0x8 ? fp : bp;
              *(p + 2) = *(p + 3) = i & 0x4 ? fp : bp;
              *(p + 0x40) = *(p + 0x41) = i & 0x2 ? fp : bp;
              *(p + 0x42) = *(p + 0x43) = i & 0x1 ? fp : bp;
#endif
            }
        }
    }

  for (i = 0; i <= 0xff; i++)
    {
      mc_table[i + 0x100] = i >> 6;
      mc_table[i + 0x300] = (i >> 4) & 0x3;
      mc_table[i + 0x500] = (i >> 2) & 0x3;
      mc_table[i + 0x700] = i & 0x3;
      mc_table[i] = tmptable[i >> 6];
      mc_table[i + 0x200] = tmptable[(i >> 4) & 0x3];
      mc_table[i + 0x400] = tmptable[(i >> 2) & 0x3];
      mc_table[i + 0x600] = tmptable[i & 0x3];
      mcmsktable[i + 0x100] = 0;
      mcmsktable[i + 0x100] |= ((i >> 6) & 0x2) ? 0xc0 : 0;
      mcmsktable[i + 0x100] |= ((i >> 4) & 0x2) ? 0x30 : 0;
      mcmsktable[i + 0x100] |= ((i >> 2) & 0x2) ? 0x0c : 0;
      mcmsktable[i + 0x100] |= (i & 0x2) ? 0x03 : 0;
      mcmsktable[i] = i;
    }
}



void
vic_ii_draw_init (void)
{
  init_drawing_tables ();

  raster_set_table_refresh_handler(&vic_ii.raster, init_drawing_tables);

#ifdef VIC_II_NEED_2X
  vic_ii_draw_set_double_size (0);
#endif

}

void
vic_ii_draw_set_double_size (int enabled)
{
#ifdef VIC_II_NEED_2X
  if (enabled)
    setup_double_size_modes ();
  else
#endif
    setup_single_size_modes ();
}

