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

#include "vice.h"

#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "wimp.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "videoarch.h"
#include "vsync.h"
#include "ui.h"

/* module includes */
#include "raster/raster.h"





#define STATUS_LINE_SIZE	64


/* Colour translation table, only used in 16/32bpp modes */
canvas_list_t *CanvasList = NULL;
/* Active canvas */
video_canvas_t *ActiveCanvas = NULL;

/* Full screen variables */
int FullScreenMode = 0;
int FullScreenStatLine = 1;

static int NumberOfCanvases = 0;
static screen_mode_t newScreenMode;
static screen_mode_t oldScreenMode;
static int newScreenValid = 0;
static int oldSingleTask;
static int newModesAvailable;
static RO_Screen FullScrDesc;
static int FullUseEigen;
static int *SpriteArea;
static int *SpriteLED0=NULL;
static int *SpriteLED1=NULL;
static int SpeedPercentage;
static int FrameRate;
static int WarpModeEnabled;
static int SpriteTranslationTable[256];
static int SpriteLEDWidth = 0;
static int SpriteLEDHeight = 0;
static int SpriteLEDMode = 0;
static char LastStatusLine[STATUS_LINE_SIZE];
static char CurrentDriveImage[64] = "";

static const int StatusBackColour = 0xcccccc10;
static const int StatusForeColour = 0x22222210;
/* Sizes in pixels */
static const int StatusLineHeight = 20;
static const int StatusCharSize = 8;
static const int StatusLEDSpace = 16;



static char *ScreenModeString = NULL;
static int ScreenSetPalette;
static int UseBPlotModule;

static void video_full_screen_colours(void);


static int set_screen_mode(resource_value_t v, void *param)
{
  char *str, *rest;
  int modenum, resx, resy, depth;

  str = (char*)v;

  if ((ScreenModeString != NULL) && (str != NULL) && (strcmp(str, ScreenModeString) == 0))
    return 0;

  resx = 0; resy = 0; depth = 0;
  modenum = strtol(str, &rest, 0);
  if (rest == str) return -1;
  if (*rest == ':')
  {
    str = rest+1;
    resx = strtol(str, &rest, 0);
    if ((rest == str) || (*rest != ',')) return -1;
    str = rest+1;
    resy = strtol(str, &rest, 0);
    if ((rest == str) || (*rest != ',')) return -1;
    str = rest+1;
    depth = strtol(str, &rest, 0);
    if ((rest == str) || (*rest != '\0')) return -1;
  }

  newModesAvailable = CheckNewModes();

  if ((newModesAvailable == 0) || (resx == 0))
  {
    newScreenMode.mode.mode_num = modenum;
  }
  else
  {
    int i;

    newScreenMode.mode.mode_ptr = newScreenMode.new_mode;
    newScreenMode.new_mode[0] = 1;
    newScreenMode.new_mode[1] = resx;
    newScreenMode.new_mode[2] = resy;
    newScreenMode.new_mode[3] = depth;
    newScreenMode.new_mode[4] = -1;

    i = 5;
    if (depth < 4)
    {
      /* Set the number of palette entries according to the mode depth */
      newScreenMode.new_mode[i++] = 3;
      newScreenMode.new_mode[i++] = (1 << (1 << depth) ) - 1;
    }
    newScreenMode.new_mode[i++] = -1;
  }

  util_string_set(&ScreenModeString, (char*)v);

  newScreenValid = 1;

  return 0;
}

static int set_screen_palette(resource_value_t v, void *param)
{
  ScreenSetPalette = (int)v;
  return 0;
}

static int set_bplot_status(resource_value_t v, void *param)
{
  UseBPlotModule = (int)v;
  return 0;
}


static resource_t resources[] = {
  {"ScreenMode", RES_STRING, (resource_value_t)"28:640,480,3",
    (resource_value_t *)&ScreenModeString, set_screen_mode, NULL },
  {"ScreenSetPalette", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *)&ScreenSetPalette, set_screen_palette, NULL },
  {"UseBPlot", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *)&UseBPlotModule, set_bplot_status, NULL },
  {NULL}
};


int video_arch_init_resources(void)
{
  return resources_register(resources);
}


int video_init_cmdline_options(void)
{
  return 0;
}


int video_init(void)
{
  return 0;
}



static int video_frame_buffer_alloc(video_canvas_t *canvas, BYTE **draw_buffer, unsigned int fb_width, unsigned int fb_height, unsigned int *fb_pitch)
{
  unsigned int palette[256];
  unsigned int i;
  sprite_area_t *sarea;

  for (i=0; i<256; i++) palette[i] = 0x10;
  /*log_message(LOG_DEFAULT, "Alloc %d x %d", fb_width, fb_height);*/

  if ((sarea = SpriteCreateAreaSprite(fb_width, fb_height, 8, "viceframe", palette, 0)) != NULL)
  {
    sprite_desc_t *sprite = SpriteGetSprite(sarea);

    if (canvas != NULL)
    {
      canvas->fb.width = fb_width;
      canvas->fb.height = fb_height;
      canvas->fb.depth = 8;
      canvas->fb.pitch = (sprite->wwidth+1)*4;
      canvas->fb.framedata = (BYTE*)SpriteGetImage(sprite);
      canvas->fb.spritebase = sarea;

      /*log_message(LOG_DEFAULT, "width = %d, height = %d, pitch = %d", canvas->fb.width, canvas->fb.height, canvas->fb.pitch);*/
    }
    *fb_pitch = (sprite->wwidth+1)*4;

    *draw_buffer = (BYTE*)SpriteGetImage(sprite);

    return 0;
  }
  return -1;
}


static void video_frame_buffer_free(video_canvas_t *canvas, BYTE *draw_buffer)
{
  if (canvas->fb.spritebase != NULL)
  {
    free(canvas->fb.spritebase);
    canvas->fb.spritebase = NULL;
    canvas->fb.framedata = NULL;
  }
}


static void video_frame_buffer_clear(video_canvas_t *canvas, BYTE *draw_buffer, BYTE value, unsigned int fb_width, unsigned int fb_height, unsigned int fb_pitch)
{
  memset(draw_buffer, value, fb_pitch * fb_height);
}




static void video_set_clipping_rectangle(int x1, int y1, int x2, int y2)
{
  unsigned short osclip[5];

  osclip[0] = (24<<8); /* relies on little endian */
  osclip[1] = (unsigned short)x1;
  osclip[2] = (unsigned short)y1;
  osclip[3] = (unsigned short)(x2 - (1<<FullScrDesc.eigx));
  osclip[4] = (unsigned short)(y2 - (1<<FullScrDesc.eigy));
  OS_WriteN(((const char*)osclip) + 1, 9);
}



static void video_canvas_ensure_translation(video_canvas_t *canvas)
{
  /* the frame buffer palette is dirty if the palette changed while there was no
     frame buffer allocated, e.g. on startup */
  if ((canvas->fb.paldirty != 0) && (canvas->current_palette != NULL))
  {
    unsigned int i, numsprpal;
    sprite_area_t *sarea = (sprite_area_t*)(canvas->fb.spritebase);
    sprite_desc_t *sprite;
    unsigned int *sprpal;
    unsigned int *ct = canvas->current_palette;

    sprite = SpriteGetSprite(sarea);
    sprpal = (unsigned int*)(sprite + 1);
    numsprpal = (1 << canvas->fb.depth);
    for (i=0; i<canvas->num_colours; i++)
    {
      sprpal[2*i] = 0x10 | (ct[i] << 8);
      sprpal[2*i+1] = sprpal[2*i];
    }
    for (; i<numsprpal; i++)
    {
      sprpal[2*i] = 0x10;
      sprpal[2*i+1] = 0x10;
    }
    canvas->fb.paldirty = 0;

    if (canvas->fb.transtab != NULL)
    {
      free(canvas->fb.transtab);
      canvas->fb.transtab = NULL;
    }
    canvas->fb.transdirty = 1;
  }

  /* do we need to make a new sprite translation table? */
  if ((canvas->fb.transdirty != 0) && (canvas->fb.transtab == NULL))
  {
    sprite_area_t *sarea = (sprite_area_t*)(canvas->fb.spritebase);
    sprite_desc_t *sprite = SpriteGetSprite(sarea);
    unsigned int ldbpp;
    char *dummy = NULL;

    ColourTrans_SelectTable((int)sarea, (int)sprite, -1, -1, &dummy, 1, NULL, NULL);

    if ((int)dummy > 0)
    {
      canvas->fb.transtab = (char*)xmalloc((int)dummy);
      dummy = canvas->fb.transtab;
      ColourTrans_SelectTable((int)sarea, (int)sprite, -1, -1, &dummy, 1, NULL, NULL);
    }

    ldbpp = (FullScreenMode == 0) ? ScreenMode.ldbpp : FullScrDesc.ldbpp;

    if (ldbpp >= 4)
    {
      unsigned int *trans;
      unsigned int i;

      if (canvas->fb.bplot_trans == NULL)
      {
        canvas->fb.bplot_trans = xmalloc(256*sizeof(int));
      }
      trans = canvas->fb.bplot_trans;
      if (ldbpp == 4)
      {
        for (i=0; i<canvas->num_colours; i++)
        {
          unsigned int pix = (canvas->current_palette)[i];
          BYTE red, green, blue;

          red = pix&0xf8; green = (pix>>8)&0xf8; blue = (pix>>16)&0xf8;
          trans[i] = (red>>3) | (green << 2) | (blue << 7);
        }
      }
      else
      {
        for (i=0; i<canvas->num_colours; i++)
        {
          trans[i] = (canvas->current_palette)[i];
        }
      }
      for (; i<256; i++) trans[i] = 0;
    }

    canvas->fb.transdirty = 0;
  }
}


void video_canvas_redraw_core(video_canvas_t *canvas, graph_env *ge, int *block)
{
  if (canvas->fb.framedata != NULL)
  {
    int shiftx, shifty;

    shiftx = (canvas->shiftx << UseEigen) * (canvas->scale);
    shifty = (canvas->shifty << UseEigen) * (canvas->scale);

    /* Coordinates of top left corner of canvas */
    ge->x = block[RedrawB_VMinX] - block[RedrawB_ScrollX] + shiftx;
    ge->y = block[RedrawB_VMaxY] - block[RedrawB_ScrollY] + shifty;

    video_canvas_ensure_translation(canvas);

    /* Use bplot or sprite plot interface? (this code only called from desktop) */
    if ((UseBPlotModule != 0) && (ScreenMode.ldbpp >= 4))
    {
      if (canvas->scale == 1)
      {
        PlotZoom1(ge, block + RedrawB_CMinX, canvas->fb.framedata, canvas->fb.bplot_trans);
      }
      else
      {
        PlotZoom2(ge, block + RedrawB_CMinX, canvas->fb.framedata, canvas->fb.bplot_trans);
      }
    }
    else
    {
      sprite_area_t *sarea;
      sprite_desc_t *sprite;

      /* clipping rectangle already set by WIMP */
      sarea = (sprite_area_t*)(canvas->fb.spritebase);
      sprite = SpriteGetSprite(sarea);

      if (canvas->scale == 1)
      {
        OS_SpriteOp(512 + 52, (int)sarea, (int)sprite, ge->x, ge->y - (canvas->fb.height << UseEigen), 0, 0, (int)(canvas->fb.transtab));
      }
      else
      {
        int scale[4];

        scale[0] = 2; scale[1] = 2; scale[2] = 1; scale[3] = 1;
        OS_SpriteOp(512 + 52, (int)sarea, (int)sprite, ge->x, ge->y - 2*(canvas->fb.height << UseEigen), 0, (int)scale, (int)(canvas->fb.transtab));
      }
    }
  }
}



int video_canvas_set_palette(video_canvas_t *canvas, const palette_t *palette, BYTE *pixel_return)
{
  int i;
  palette_entry_t *p;
  unsigned int numsprpal;
  unsigned int *ct;

  if (palette == NULL) return 0;

  if (canvas->current_palette != NULL)
  {
    free(canvas->current_palette);
  }
  canvas->num_colours = palette->num_entries;
  canvas->current_palette = xmalloc((canvas->num_colours)*sizeof(int));

  p = palette->entries;
  numsprpal = (1<<canvas->fb.depth);

  /* adapt the palette stored in the frame buffer sprite */
  if (canvas->fb.spritebase != NULL)
  {
    sprite_area_t *sarea = (sprite_area_t*)(canvas->fb.spritebase);
    sprite_desc_t *sprite;
    unsigned int *sprpal;

    sprite = SpriteGetSprite(sarea);
    sprpal = (unsigned int*)(sprite+1);

    for (i=0; i<palette->num_entries; i++)
    {
      sprpal[2*i] = 0x10 | (p[i].red << 8) | (p[i].green << 16) | (p[i].blue << 24);
      sprpal[2*i+1] = sprpal[2*i];
    }
    for (; i<numsprpal; i++)
    {
      sprpal[2*i] = 0x10;
      sprpal[2*i+1] = 0x10;
    }
    canvas->fb.paldirty = 0;
  }
  else
  {
    /* no frame buffer allocated, make a note that the palette is dirty */
    canvas->fb.paldirty = 1;
  }

  if (canvas->fb.transtab != NULL)
  {
    free(canvas->fb.transtab);
    canvas->fb.transtab = NULL;
  }
  canvas->fb.transdirty = 1;

  /* remember the current palette in canvas->current_palette */
  ct = canvas->current_palette;
  for (i=0; i<canvas->num_colours; i++)
  {
    /* FIXME: will go entirely eventually */
    pixel_return[i] = i;

    ct[i] = p[i].red | (p[i].green << 8) | (p[i].blue << 16);
  }

  return 0;
}


video_canvas_t *video_canvas_create(const char *win_name, unsigned int *width, unsigned int *height, int mapped, void_t exposure_handler, const palette_t *palette, BYTE *pixel_return)
{
  video_canvas_t *canvas;
  canvas_list_t *newCanvas;

  if ((canvas = (video_canvas_t *)malloc(sizeof(struct video_canvas_s))) == NULL)
    return (video_canvas_t *)0;

  canvas->video_draw_buffer_callback = xmalloc(sizeof(video_draw_buffer_callback_t));
  canvas->video_draw_buffer_callback->draw_buffer_alloc = video_frame_buffer_alloc;
  canvas->video_draw_buffer_callback->draw_buffer_free = video_frame_buffer_free;
  canvas->video_draw_buffer_callback->draw_buffer_clear = video_frame_buffer_clear;

  canvas->width = *width; canvas->height = *height;

  canvas->num_colours = (palette == NULL) ? 16 : palette->num_entries;
  canvas->current_palette = NULL;
  canvas->shiftx = 0; canvas->shifty = 0; canvas->scale = 1;
  memset(&(canvas->fb), 0, sizeof(video_frame_buffer_t));
  canvas->fb.transdirty = 1;

  video_canvas_set_palette(canvas, palette, pixel_return);

  if ((newCanvas = (canvas_list_t*)malloc(sizeof(canvas_list_t))) == NULL)
  {
    free(canvas); return NULL;
  }

  newCanvas->next = NULL; newCanvas->canvas = canvas;

  if (CanvasList == NULL)
  {
    CanvasList = newCanvas;
    canvas->window = EmuWindow;
  }
  else
  {
    canvas_list_t *list = CanvasList;

    if ((canvas->window = wimp_window_clone(EmuWindow)) == NULL)
    {
      free(canvas); free(newCanvas); return NULL;
    }
    canvas->window->Handle = Wimp_CreateWindow(((int*)(canvas->window)) + 1);
    while (list->next != NULL) list = list->next;
    list->next = newCanvas;
  }

  ActiveCanvas = canvas;
  wimp_window_set_extent(canvas->window, 0, - *height << UseEigen, *width << UseEigen, 0);
  ui_open_emu_window(canvas->window, NULL);

  Wimp_GetCaretPosition(&LastCaret);
  Wimp_SetCaretPosition(canvas->window->Handle, -1, -100, 100, -1, -1);

  NumberOfCanvases++;

  return canvas;
}


void video_canvas_destroy(video_canvas_t *s)
{
  canvas_list_t *clist, *last;

  last = NULL; clist = CanvasList;
  while (clist != NULL)
  {
    if (clist->canvas == s)
    {
      if (last == NULL)
        CanvasList = clist->next;
      else
        last->next = clist->next;

      free(clist); break;
    }
    last = clist; clist = clist->next;
  }

  ui_close_emu_window(s->window, NULL);

  NumberOfCanvases--;

  if (s->video_draw_buffer_callback != NULL)
    free(s->video_draw_buffer_callback);

  if (s->current_palette != NULL)
  {
    free(s->current_palette);
    s->current_palette = NULL;
  }
  if (s->fb.transtab != NULL)
  {
    free(s->fb.transtab);
    s->fb.transtab = NULL;
  }
  if (s->fb.bplot_trans != NULL)
  {
    free(s->fb.bplot_trans);
    s->fb.bplot_trans = NULL;
  }

  free(s);
}


void video_canvas_map(video_canvas_t *s)
{
}


void video_canvas_unmap(video_canvas_t *s)
{
}


void video_canvas_resize(video_canvas_t *s, unsigned int width, unsigned int height)
{
  /* Make a note of the resize, too */
  s->width = width; s->height = height;
  if (FullScreenMode == 0)
  {
    int w, h;

    w = (s->scale * width) << UseEigen; h = (s->scale * height) << UseEigen;
    wimp_window_set_extent(s->window, 0, -h, w, 0);
    Wimp_GetWindowState((int*)(s->window));
    /* Only open window if it was open to begin with */
    if ((s->window->wflags & (1<<16)) != 0)
    {
      Wimp_OpenWindow((int*)(s->window));
    }
  }
}


void video_canvas_refresh(video_canvas_t *canvas, BYTE *draw_buffer,
                          unsigned int draw_buffer_line_size,
 			  unsigned int xs, unsigned int ys,
			  unsigned int xi, unsigned int yi,
			  unsigned int w, unsigned int h)
{
  graph_env ge;

  if (ModeChanging != 0) return;

  if (canvas->fb.framedata == NULL) return;

  FrameBufferUpdate = 0;
  ge.dimx = canvas->fb.pitch; ge.dimy = canvas->fb.height;
  canvas->shiftx = (xi - xs); canvas->shifty = - (yi - ys);

  if (FullScreenMode == 0)
  {
    int block[11];
    int more;

    block[0] = canvas->window->Handle;
    /* The canvas size is only used for the clipping */
    block[1] = (xi << UseEigen) * (canvas->scale);
    block[2] = (- (yi + h) << UseEigen) * (canvas->scale);
    block[3] = ((xi + w) << UseEigen) * (canvas->scale);
    block[4] = (-yi << UseEigen) * (canvas->scale);

    more = Wimp_UpdateWindow(block);
    while (more != 0)
    {
      video_canvas_redraw_core(canvas, &ge, block);
      more = Wimp_GetRectangle(block);
    }
  }
  else if (canvas == ActiveCanvas)
  {
    int clip[4];
    int dx, dy;
    int clipYlow;
    int shiftx, shifty;

    video_canvas_ensure_translation(canvas);

    if (FullScreenStatLine == 0)
      clipYlow = 0;
    else
      clipYlow = (StatusLineHeight << FullScrDesc.eigy);

    dx = (canvas->width << FullUseEigen); dy = (canvas->height << FullUseEigen);
    shiftx = (canvas->shiftx << FullUseEigen);
    shifty = (canvas->shifty << FullUseEigen);
    ge.x = (FullScrDesc.resx - dx)/2 + shiftx;
    ge.y = (FullScrDesc.resy - dy)/2 + dy + shifty;
    clip[0] = (FullScrDesc.resx - dx) / 2 + (xi << FullUseEigen);
    clip[2] = clip[0] + (w << FullUseEigen);
    clip[1] = (FullScrDesc.resy - dy) / 2 + dy - ((yi + h) << FullUseEigen);
    clip[3] = clip[1] + (h << FullUseEigen);
    if ((clip[0] >= FullScrDesc.resx) || (clip[2] < 0)) return;
    if ((clip[1] >= FullScrDesc.resy) || (clip[3] < clipYlow)) return;
    if (clip[0] < 0) clip[0] = 0;
    if (clip[2] > FullScrDesc.resx) clip[2] = FullScrDesc.resx;
    if (clip[1] < clipYlow) clip[1] = clipYlow;
    if (clip[3] > FullScrDesc.resy) clip[3] = FullScrDesc.resy;

    /* Use BPlot module or sprite plots? */
    if ((UseBPlotModule != 0) && ((FullScrDesc.ldbpp >= 4) || (ScreenSetPalette != 0)))
    {
      PlotZoom1(&ge, clip, draw_buffer, canvas->fb.bplot_trans);
    }
    else
    {
      sprite_area_t *sarea;
      sprite_desc_t *sprite;

      /* Must explicitly set the clipping rectangle */
      video_set_clipping_rectangle(clip[0], clip[1], clip[2], clip[3]);

      sarea = (sprite_area_t*)(canvas->fb.spritebase);
      sprite = SpriteGetSprite(sarea);
      OS_SpriteOp(512 + 52, (int)sarea, (int)sprite, ge.x, ge.y - (canvas->fb.height << FullUseEigen), 0, 0, (int)(canvas->fb.transtab));
    }
  }
}


void canvas_mode_change(void)
{
  canvas_list_t *clist = CanvasList;

  /* delete all sprite translation tables, will be recreated on demand */
  while (clist != NULL)
  {
    if (clist->canvas->fb.transtab != NULL)
    {
      free(clist->canvas->fb.transtab);
      clist->canvas->fb.transtab = NULL;
    }
    clist->canvas->fb.transdirty = 1;
    clist = clist->next;
  }
}


video_canvas_t *canvas_for_handle(int handle)
{
  canvas_list_t *clist = CanvasList;

  while (clist != NULL)
  {
    if (clist->canvas->window->Handle == handle) return clist->canvas;
    clist = clist->next;
  }
  return NULL;
}


unsigned int canvas_number_for_handle(int handle)
{
  canvas_list_t *clist = CanvasList;
  unsigned int num = 0;

  while (clist != NULL)
  {
    if (clist->canvas->window->Handle == handle) return num;
    clist = clist->next;
    num++;
  }
  return UINT_MAX;
}


void canvas_next_active(int moveCaret)
{
  canvas_list_t *clist = CanvasList;

  if (NumberOfCanvases <= 1) return;

  while (clist != NULL)
  {
    if (clist->canvas == ActiveCanvas) break;
    clist = clist->next;
  }
  if (clist != NULL)
  {
    int block[WindowB_WFlags+1];

    if (clist->next == NULL)
      ActiveCanvas = CanvasList->canvas;
    else
      ActiveCanvas = clist->next->canvas;

    block[WindowB_Handle] = ActiveCanvas->window->Handle;
    Wimp_GetWindowState(block);
    ui_open_emu_window(ActiveCanvas->window, block);

    ui_show_emu_scale();

    video_full_screen_colours();
    video_full_screen_refresh();

    if (moveCaret != 0) Wimp_SetCaretPosition(ActiveCanvas->window->Handle, -1, -100, 100, -1, -1);
  }
}


int canvas_get_number(void)
{
  return NumberOfCanvases;
}



static void video_full_screen_set_clip(void)
{
  video_set_clipping_rectangle(0, 0, FullScrDesc.resx, (StatusLineHeight << FullScrDesc.eigy));
}


void video_full_screen_colours(void)
{
  char *sdata, *limit;

  if (FullScreenMode == 0) return;

  /* Set the palette first thing */
  if (ScreenSetPalette != 0)
  {
    video_canvas_t *canvas = ActiveCanvas;
    unsigned int num_colours = ActiveCanvas->num_colours;

    if ((canvas != NULL) && ((1 << (1 << FullScrDesc.ldbpp)) >= num_colours) && (FullScrDesc.ldbpp <= 3))
    {
      unsigned int *ct;

      ct = canvas->current_palette;
      if (ct != NULL)
      {
        unsigned char entries[3 * num_colours];
        unsigned int i;
        for (i=0; i<num_colours; i++)
        {
          entries[3*i] = ct[i] & 0xff;
          entries[3*i+1] = (ct[i] & 0xff00) >> 8;
          entries[3*i+2] = (ct[i] & 0xff0000) >> 16;
        }
        InstallPaletteRange(entries, 0, num_colours);
        ColourTrans_InvalidateCache();
      }
    }
  }

  sdata = ((char*)SpriteArea) + SpriteArea[2];
  limit = ((char*)SpriteArea) + SpriteArea[0];
  while (sdata < limit)
  {
    if (strncmp(sdata+4, "led_off", 12) == 0) SpriteLED0 = (unsigned int *)sdata;
    else if (strncmp(sdata+4, "led_on", 12) == 0) SpriteLED1 = (unsigned int *)sdata;
    sdata += *((int*)sdata);
  }
  if (SpriteLED0 != NULL)
  {
    sdata = (char*)SpriteTranslationTable;
    ColourTrans_SelectTable((int)SpriteArea, (int)SpriteLED0, -1, -1, &sdata, 1, NULL, NULL);
    OS_SpriteInfo(0x0200, SpriteArea, SpriteLED0, &SpriteLEDWidth, &SpriteLEDHeight, &SpriteLEDMode);
    SpriteLEDWidth <<= OS_ReadModeVariable(SpriteLEDMode, 4);
    SpriteLEDHeight <<= OS_ReadModeVariable(SpriteLEDMode, 5);
  }
}


int video_full_screen_on(int *sprites)
{
  if (newScreenValid == 0) return -1;

  vsync_suspend_speed_eval();

  if (SwitchToMode(&newScreenMode, &oldScreenMode) != NULL)
    return -1;

  oldSingleTask = SingleTasking;
  SingleTasking = 1;
  FullScreenMode = 1;
  wimp_read_screen_mode(&FullScrDesc);
  FullUseEigen = (FullScrDesc.eigx < FullScrDesc.eigy) ? FullScrDesc.eigx : FullScrDesc.eigy;
  SpriteArea = sprites;

  /* Set text size */
  OS_WriteC(23); OS_WriteC(17); OS_WriteC(7); OS_WriteC(2);
  OS_WriteC(StatusCharSize); OS_WriteC(0); OS_WriteC(StatusCharSize); OS_WriteC(0);
  OS_WriteC(0); OS_WriteC(0);

  video_full_screen_colours();

  raster_mode_change();

  video_full_screen_refresh();

  return 0;
}


int video_full_screen_off(void)
{
  vsync_suspend_speed_eval();

  if (SwitchToMode(&oldScreenMode, NULL) != NULL)
    return -1;

  SingleTasking = oldSingleTask;
  FullScreenMode = 0;

  /* Flush keyboard and mouse buffer */
  OS_FlushBuffer(0);
  OS_FlushBuffer(9);

  return 0;
}


int video_full_screen_refresh(void)
{
  video_canvas_t *canvas = ActiveCanvas;

  if ((FullScreenMode == 0) || (canvas == NULL)) return -1;

  /* Clear screen and force a repaint of the entire bitmap */
  ColourTrans_SetGCOL(0, 0x100, 0);
  OS_Plot(0x04, 0, 0); OS_Plot(0x65, FullScrDesc.resx, FullScrDesc.resy);

  video_canvas_refresh(canvas, canvas->fb.framedata, canvas->fb.pitch, -canvas->shiftx, canvas->shifty, 0, 0, canvas->width, canvas->height);

  video_full_screen_init_status();

  return 0;
}


/* DriveLEDStates is already updated in ui.c */
void video_full_screen_drive_leds(unsigned int drive)
{
  if ((FullScreenMode != 0) && (FullScreenStatLine != 0) && (drive < 4))
  {
    int posx, posy;
    int sptr;

    video_full_screen_set_clip();

    posy = ((StatusLineHeight << FullScrDesc.eigy) - SpriteLEDHeight) >> 1;
    posx = FullScrDesc.resx - (4-drive)*(SpriteLEDWidth + (StatusLEDSpace << FullScrDesc.eigx));
    sptr = (int)((DriveLEDStates[drive] == 0) ? SpriteLED0 : SpriteLED1);
    if (sptr != 0)
    {
      OS_SpriteOp(512 + 52, (int)SpriteArea, sptr, posx, posy, 8, 0, (int)SpriteTranslationTable);
    }
  }
}


void video_full_screen_speed(int percent, int framerate, int warp)
{
  SpeedPercentage = percent;
  FrameRate = framerate;
  WarpModeEnabled = warp;
}


void video_full_screen_init_status(void)
{
  if ((FullScreenMode != 0) && (FullScreenStatLine != 0))
  {
    unsigned int i;

    video_full_screen_set_clip();

    ColourTrans_SetGCOL(StatusBackColour, 0x100, 0);
    OS_Plot(0x04, 0, 0);
    OS_Plot(0x65, FullScrDesc.resx, (StatusLineHeight-1) << FullScrDesc.eigy);
    LastStatusLine[0] = 0;

    for (i=0; i<4; i++)
    {
      video_full_screen_drive_leds(i);
    }
  }
}


void video_full_screen_plot_status(void)
{
  if ((FullScreenMode != 0) && (FullScreenStatLine != 0))
  {
    char statText[STATUS_LINE_SIZE];
    char *b;

    b = statText; statText[STATUS_LINE_SIZE-1] = '\0';
    b += sprintf(b, "%3d%% / %2d fps", SpeedPercentage, FrameRate);
    if (WarpModeEnabled != 0)
      b += sprintf(b, " (warp)");
    if (DriveTrackNumbers[0] != 0)
      b += sprintf(b, "; 8: %2d.%d", DriveTrackNumbers[0] >> 1, 5*(DriveTrackNumbers[0] & 1));
    if (DriveTrackNumbers[1] != 0)
      b += sprintf(b, "; 9: %2d.%d", DriveTrackNumbers[1] >> 1, 5*(DriveTrackNumbers[1] & 1));
    strncpy(b, CurrentDriveImage, STATUS_LINE_SIZE - 1 - (b-statText));

    if (strcmp(statText, LastStatusLine) != 0)
    {
      int width;

      video_full_screen_set_clip();

      strcpy(LastStatusLine, statText);
      width = (StatusCharSize * strlen(statText)) << FullScrDesc.eigx;
      /* Clear background covered by text */
      ColourTrans_SetGCOL(StatusBackColour, 0x100, 0);
      OS_Plot(0x04, 0, 0);
      OS_Plot(0x65, width, (StatusLineHeight-1) << FullScrDesc.eigy);
      /* Position text cursor */
      ColourTrans_SetGCOL(StatusForeColour, 0x100, 0);
      OS_WriteC(0x05);
      OS_Plot(0x04, 0, ((StatusLineHeight + StatusCharSize) << FullScrDesc.eigx) >> 1);

      OS_Write0(statText);
    }
  }
}


void video_full_screen_display_image(unsigned int num, const char *img)
{
  video_full_screen_set_clip();

  if ((img == NULL) || (*img == '\0'))
    CurrentDriveImage[0] = '\0';
  else
    sprintf(CurrentDriveImage, "   [%d: %s]", num + 8, img);
}




/*
 *  Callbacks
 */

static void callback_canvas_modified(const char *name, void *callback_param)
{
    log_message(LOG_DEFAULT, "Resource callback for %s", name);
}



void video_register_callbacks(void)
{
    resources_register_callback("PALMode", callback_canvas_modified, NULL);
    resources_register_callback("UseBPlot", callback_canvas_modified, NULL);
    resources_register_callback("VDC_DoubleSize", callback_canvas_modified, NULL);
    resources_register_callback("VDC_DoubleScan", callback_canvas_modified, NULL);
}
