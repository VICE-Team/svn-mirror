/*
 * video.c - RISC OS graphics routines.
 *
 * Written by
 *  Andreas Dehmel <dehmel@forwiss.tu-muenchen.de>
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

#include <stdlib.h>
#include <string.h>

#include "ROlib.h"
#include "wimp.h"
#include "config.h"
#include "types.h"
#include "video.h"
#include "ui.h"





/* Colour translation table, only used in 16/32bpp modes */
unsigned int ColourTable[256];
frame_buffer_t *FrameBuffer;
canvas_t EmuCanvas = NULL;



int video_init_resources(void)
{
  return 0;
}


int video_init_cmdline_options(void)
{
  return 0;
}


int video_init(void)
{
  return 0;
}


/*void video_free(void)
{
}*/


int frame_buffer_alloc(frame_buffer_t *i, unsigned int width, unsigned int height)
{
  PIXEL *data;

  FrameBuffer = i;
  width = (width + 3) & ~3;

  if ((data = (PIXEL*)malloc(width * height * sizeof(PIXEL))) == NULL)
    return -1;

  i->tmpframebuffer = data;
  i->tmpframebufferlinesize = width;
  i->width = width; i->height = height;

  return 0;
}


void frame_buffer_free(frame_buffer_t *i)
{
  free(i->tmpframebuffer);
}


void frame_buffer_clear(frame_buffer_t *i, PIXEL value)
{
  if (ModeChanging == 0)
    memset(i->tmpframebuffer, value, i->width * i->height);
}


int canvas_set_palette(canvas_t canvas, const palette_t *palette, PIXEL *pixel_return)
{
  int i;
  palette_entry_t *p;

  p = palette->entries;

  switch (ScreenMode.ldbpp)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      {
        for (i=0; i<palette->num_entries; i++)
        {
          pixel_return[i] = ModeColourNumber((p[i].red << 8) | (p[i].green << 16) | (p[i].blue << 24));
          ColourTable[i] = pixel_return[i];
        }
      }
      break;
    case 4:
      {
        for (i=0; i<palette->num_entries; i++)
        {
          int r,g,b;

          r = (p[i].red   + 4) & 0x1f8; if (r > 0xff) r = 0xff;
          g = (p[i].green + 4) & 0x1f8; if (g > 0xff) g = 0xff;
          b = (p[i].blue  + 4) & 0x1f8; if (b > 0xff) b = 0xff;
          pixel_return[i] = i;
          ColourTable[i] = (r>>3) | (g << 2) | (b << 7);
        }
      }
      break;
    case 5:
      {
        for (i=0; i<palette->num_entries; i++)
        {
          pixel_return[i] = i;
          ColourTable[i] = p[i].red | (p[i].green << 8) | (p[i].blue << 16);
        }
      }
      break;
  }
  canvas->pixel_translation = pixel_return;

  return 0;
}


canvas_t canvas_create(const char *win_name, unsigned int *width, unsigned int *height, int mapped, canvas_redraw_t exposure_handler, const palette_t *palette, PIXEL *pixel_return)
{
  canvas_t canvas;

  if ((canvas = (canvas_t)malloc(sizeof(struct _canvas))) == NULL)
    return (canvas_t)0;

  canvas->width = *width; canvas->height = *height;
  canvas->emuwindow = EmuWindow->Handle;
  canvas->drawable = 0;	/* Who gives a fuck... */
  canvas_set_palette(canvas, palette, pixel_return);

  EmuCanvas = canvas;

  wimp_window_set_extent(EmuWindow, 0, - *height << UseEigen, *width << UseEigen, 0);
  ui_open_emu_window(NULL);

  Wimp_GetCaretPosition(&LastCaret);
  Wimp_SetCaretPosition(EmuWindow->Handle, -1, -100, 100, -1, -1);

  return canvas;
}


void canvas_destroy(canvas_t s)
{
  ui_close_emu_window(NULL);

  free(s);
}


void canvas_map(canvas_t s)
{
}


void canvas_unmap(canvas_t s)
{
}


void canvas_resize(canvas_t s, unsigned int width, unsigned int height)
{
  /* Make a note of the resize, too */
  s->width = width; s->height = height;
  wimp_window_set_extent(EmuWindow, 0, -height << UseEigen, width << UseEigen, 0);
  Wimp_GetWindowState((int*)EmuWindow);
  /* Only open window if it was open to begin with */
  if ((EmuWindow->wflags & (1<<16)) != 0)
  {
    Wimp_OpenWindow((int*)EmuWindow);
  }
}


void canvas_refresh(canvas_t canvas, frame_buffer_t frame_buffer,
			unsigned int xs, unsigned int ys,
			unsigned int xi, unsigned int yi,
			unsigned int w, unsigned int h)
{
  int block[11];
  int more;
  int shiftx, shifty;
  graph_env ge;

  if (ModeChanging != 0) return;

  FrameBufferUpdate = 0;

  block[0] = canvas->emuwindow;
  /* The canvas size is only used for the clipping */
  block[1] = (xi << UseEigen) * EmuZoom;
  block[2] = (- (yi + h) << UseEigen) * EmuZoom;
  block[3] = ((xi + w) << UseEigen) * EmuZoom;
  block[4] = (-yi << UseEigen) * EmuZoom;
  ge.dimx = frame_buffer.width; ge.dimy = frame_buffer.height;
  canvas->shiftx = (xi - xs); canvas->shifty = - (yi - ys);
  shiftx = (canvas->shiftx << UseEigen) * EmuZoom;
  shifty = (canvas->shifty << UseEigen) * EmuZoom;

  more = Wimp_UpdateWindow(block);
  while (more != 0)
  {
    /* Coordinates of top left corner of canvas */
    ge.x = block[RedrawB_VMinX] - block[RedrawB_ScrollX] + shiftx;
    ge.y = block[RedrawB_VMaxY] - block[RedrawB_ScrollY] + shifty;

    if (EmuZoom == 1)
    {
      PlotZoom1(&ge, block + RedrawB_CMinX, frame_buffer.tmpframebuffer, ColourTable);
    }
    else
    {
      PlotZoom2(&ge, block + RedrawB_CMinX, frame_buffer.tmpframebuffer, ColourTable);
    }
    more = Wimp_GetRectangle(block);
  }
}


void text_enable(void)
{
}


void text_disable(void)
{
}


int num_text_lines(void)
{
  return 0;
}


void enable_text(void)
{
}


void disable_text(void)
{
}
