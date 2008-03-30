/*
 * video-render.c - Implementation of framebuffer to physical screen copy
 *
 * Written by
 *  John Selck <graham@cruise.de>
 *  Dag Lem <resid@nimrod.no>
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

#include "render1x1.h"
#include "render1x1pal.h"
#include "render1x2.h"
#include "render2x2.h"
#include "render2x2pal.h"
#include "renderscale2x.h"
#include "renderyuv.h"
#include "types.h"
#include "video-render-pal.h"
#include "video-render.h"
#include "video-resources.h"
#include "video.h"


/*****************************************************************************/
/*****************************************************************************/

/* this function is the interface to the outer world */

int video_render_get_fake_pal_state(void)
{
    return video_resources.delayloop_emulation;
}

void video_render_initconfig(video_render_config_t *config)
{
    int i;

    config->rendermode = VIDEO_RENDER_NULL;
    config->doublescan = 0;

    for (i = 0; i < 256; i++)
        config->physical_colors[i] = 0;
}

void video_render_setphysicalcolor(video_render_config_t *config, int index,
                                   DWORD color, int depth)
{
    switch (depth) {
      case 8:
        color &= 0x000000FF;
        color = color | (color << 8);
        break;
      case 16:
        color &= 0x0000FFFF;
        color = color | (color << 16);
        break;
      case 24:
        color &= 0x00FFFFFF;
        break;
      case 32:
      break;
    }
    config->physical_colors[index] = color;
}

void video_render_main(video_render_config_t *config, BYTE *src, BYTE *trg,
                       int width, int height, int xs, int ys, int xt, int yt,
                       int pitchs, int pitcht, int depth)
{
    DWORD *colortab;
    int doublescan, rendermode, scale2x;

#if 0
printf("w:%i h:%i xs:%i ys:%i xt:%i yt:%i ps:%i pt:%i d%i\n",
        width, height, xs, ys, xt, yt, pitchs, pitcht, depth);
#endif

    if (width <= 0)
        return; /* some render routines don't like invalid width */

    rendermode = config->rendermode;
    doublescan = config->doublescan;
    colortab = config->physical_colors;
    scale2x = config->scale2x;

    switch (rendermode) {
      case VIDEO_RENDER_NULL:
        break;

      case VIDEO_RENDER_PAL_1X1:
      case VIDEO_RENDER_PAL_2X2:
        video_render_pal_main(config, src, trg, width, height, xs, ys, xt, yt,
                              pitchs, pitcht, depth);
        return;

      case VIDEO_RENDER_RGB_1X1:
        switch (depth) {
          case 8:
            render_08_1x1_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht);
            return;
          case 16:
            render_16_1x1_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht);
            return;
          case 24:
            render_24_1x1_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht);
            return;
          case 32:
            render_32_1x1_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht);
            return;
        }
        return;

      case VIDEO_RENDER_RGB_1X2:
        switch (depth) {
          case 8:
            render_08_1x2_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht, doublescan);
            return;
          case 16:
            render_16_1x2_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht, doublescan);
            return;
          case 24:
            render_24_1x2_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht, doublescan);
            return;
          case 32:
            render_32_1x2_04(colortab, src, trg, width, height,
                             xs, ys, xt, yt, pitchs, pitcht, doublescan);
            return;
        }
        return;

      case VIDEO_RENDER_RGB_2X2:
          if (scale2x) {
            switch (depth) {
              case 8:
                render_08_scale2x(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht);
                return;
              case 16:
                render_16_scale2x(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht);
                return;
              case 24:
                render_24_scale2x(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht);
                return;
              case 32:
                render_32_scale2x(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht);
                return;
            }
        } else {
            switch (depth) {
              case 8:
                render_08_2x2_04(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht, doublescan);
                return;
              case 16:
                render_16_2x2_04(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht, doublescan);
                return;
              case 24:
                render_24_2x2_04(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht, doublescan);
                return;
              case 32:
                render_32_2x2_04(colortab, src, trg, width, height,
                                 xs, ys, xt, yt, pitchs, pitcht, doublescan);
                return;
            }
        }
        return;
    }
}



#ifdef HAVE_XVIDEO
/* Render YUV 4:2:2 and 4:1:1 formats. */
void render_yuv_image(int double_size,
                      int double_scan,
                      int pal_mode,
                      int pal_scanline_shade,
                      fourcc_t format,
                      image_t* image,
                      unsigned char* src,
                      int src_pitch,
                      unsigned int* src_color,
                      int src_x, int src_y,
                      unsigned int src_w, unsigned int src_h,
                      int dest_x, int dest_y)
{
  int planar;
  int shift_y0, shift_u, shift_v, shift_y1;
  int plane_y, plane_u, plane_v;

  switch (format.id) {
  case FOURCC_UYVY:
    planar = 0;
    plane_y = plane_u = plane_v = 0;
#ifdef WORDS_BIGENDIAN
    shift_y0 = 16; shift_u = 24; shift_v = 8; shift_y1 = 0;
#else
    shift_y0 = 8; shift_u = 0; shift_v = 16; shift_y1 = 24;
#endif
    break;
  case FOURCC_YUY2:
    planar = 0;
    plane_y = plane_u = plane_v = 0;
#ifdef WORDS_BIGENDIAN
    shift_y0 = 24; shift_u = 16; shift_v = 0; shift_y1 = 8;
#else
    shift_y0 = 0; shift_u = 8; shift_v = 24; shift_y1 = 16;
#endif
    break;
  case FOURCC_YVYU:
    planar = 0;
    plane_y = plane_u = plane_v = 0;
#ifdef WORDS_BIGENDIAN
    shift_y0 = 24; shift_u = 0; shift_v = 16; shift_y1 = 8;
#else
    shift_y0 = 0; shift_u = 24; shift_v = 8; shift_y1 = 16;
#endif
    break;
  case FOURCC_YV12:
    planar = 1;
    shift_y0 = shift_u = shift_v = shift_y1 = 0;
    plane_y = 0; plane_u = 2; plane_v = 1;
    break;
  case FOURCC_I420:
  case FOURCC_IYUV:
    planar = 1;
    shift_y0 = shift_u = shift_v = shift_y1 = 0;
    plane_y = 0; plane_u = 1; plane_v = 2;
    break;
  default:
    return;
  }

  if (double_size) {
    /* 2x2 */
    if (planar) {
      switch(pal_mode) {
      case VIDEO_RESOURCE_PAL_MODE_FAST:
        renderyuv_2x_4_1_1(image, plane_y, plane_u, plane_v,
                           src, src_pitch, src_color,
                           src_x, src_y, src_w, src_h, dest_x, dest_y,
                           double_scan, pal_scanline_shade);
        break;
      default:
      case VIDEO_RESOURCE_PAL_MODE_SHARP:
      case VIDEO_RESOURCE_PAL_MODE_BLUR:
        renderyuv_2x_4_1_1_pal(image, plane_y, plane_u, plane_v,
                               src, src_pitch, src_color,
                               src_x, src_y, src_w, src_h, dest_x, dest_y,
                               pal_mode, double_scan, pal_scanline_shade);
        break;
      }
    }
    else {
      switch(pal_mode) {
      case VIDEO_RESOURCE_PAL_MODE_FAST:
        renderyuv_2x_4_2_2(image, shift_y0, shift_u, shift_v, shift_y1,
                           src, src_pitch, src_color,
                           src_x, src_y, src_w, src_h, dest_x, dest_y,
                           double_scan, pal_scanline_shade);
        break;
      default:
      case VIDEO_RESOURCE_PAL_MODE_SHARP:
      case VIDEO_RESOURCE_PAL_MODE_BLUR:
        renderyuv_2x_4_2_2_pal(image, shift_y0, shift_u, shift_v, shift_y1,
                               src, src_pitch, src_color,
                               src_x, src_y, src_w, src_h, dest_x, dest_y,
                               pal_mode, double_scan, pal_scanline_shade);
        break;
      }
    }
  }
  else {
    /* 1x1 */
    if (planar) {
      switch(pal_mode) {
      case VIDEO_RESOURCE_PAL_MODE_FAST:
        renderyuv_4_1_1(image, plane_y, plane_u, plane_v,
                        src, src_pitch, src_color,
                        src_x, src_y, src_w, src_h, dest_x, dest_y);
        break;
      default:
      case VIDEO_RESOURCE_PAL_MODE_SHARP:
      case VIDEO_RESOURCE_PAL_MODE_BLUR:
        renderyuv_4_1_1_pal(image, plane_y, plane_u, plane_v,
                            src, src_pitch, src_color,
                            src_x, src_y, src_w, src_h, dest_x, dest_y,
                            pal_mode);
        break;
      }
    }
    else {
      switch(pal_mode) {
      case VIDEO_RESOURCE_PAL_MODE_FAST:
        renderyuv_4_2_2(image, shift_y0, shift_u, shift_v, shift_y1,
                        src, src_pitch, src_color,
                        src_x, src_y, src_w, src_h, dest_x, dest_y);
        break;
      default:
      case VIDEO_RESOURCE_PAL_MODE_SHARP:
      case VIDEO_RESOURCE_PAL_MODE_BLUR:
        renderyuv_4_2_2_pal(image, shift_y0, shift_u, shift_v, shift_y1,
                            src, src_pitch, src_color,
                            src_x, src_y, src_w, src_h, dest_x, dest_y,
                            pal_mode);
        break;
      }
    }
  }
}
#endif

