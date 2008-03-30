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
#include "raster.h"
#include "resources.h"
#include "types.h"
#include "utils.h"
#include "video.h"
#include "ui.h"






/* Colour translation table, only used in 16/32bpp modes */
canvas_list_t *CanvasList = NULL;
/* Active canvas */
canvas_t ActiveCanvas = NULL;

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
static char LastStatusLine[64];

static const int StatusBackColour = 0xcccccc10;
static const int StatusForeColour = 0x22222210;
/* Sizes in pixels */
static const int StatusLineHeight = 20;
static const int StatusCharSize = 8;
static const int StatusLEDSpace = 16;



static char *ScreenModeString = NULL;
static int ScreenSetPalette;

static void video_full_screen_colours(void);


static int set_screen_mode(resource_value_t v)
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

  string_set(&ScreenModeString, (char*)v);

  newScreenValid = 1;

  return 0;
}

static int set_screen_palette(resource_value_t v)
{
  ScreenSetPalette = (int)v;
  return 0;
}


static resource_t resources[] = {
  {"ScreenMode", RES_STRING, (resource_value_t)"28:640,480,3",
    (resource_value_t *)&ScreenModeString, set_screen_mode},
  {"ScreenSetPalette", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *)&ScreenSetPalette, set_screen_palette},
  {NULL}
};


int video_init_resources(void)
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


/*void video_free(void)
{
}*/


int frame_buffer_alloc(frame_buffer_t *i, unsigned int width, unsigned int height)
{
  PIXEL *data;

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
  unsigned int *ct;

  if (palette == NULL) return 0;

  p = palette->entries;
  ct = canvas->colour_table;

  switch (ScreenMode.ldbpp)
  {
    case 0:
    case 1:
    case 2:
    case 3:
      {
        for (i=0; i<palette->num_entries; i++)
        {
          pixel_return[i] = (PIXEL)ColourTrans_ReturnColourNumber((p[i].red << 8) | (p[i].green << 16) | (p[i].blue << 24));
          ct[i] = p[i].red | (p[i].green << 8) | (p[i].blue << 16);/*pixel_return[i];*/
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
          ct[i] = (r>>3) | (g << 2) | (b << 7);
        }
      }
      break;
    case 5:
      {
        for (i=0; i<palette->num_entries; i++)
        {
          pixel_return[i] = i;
          ct[i] = p[i].red | (p[i].green << 8) | (p[i].blue << 16);
        }
      }
      break;
  }
  if (canvas->pixel_translation == NULL)
  {
    if ((canvas->pixel_translation = (PIXEL*)malloc(palette->num_entries * sizeof(PIXEL))) == NULL)
      return -1;
  }
  memcpy(canvas->pixel_translation, pixel_return, palette->num_entries * sizeof(PIXEL));

  return 0;
}


canvas_t canvas_create(const char *win_name, unsigned int *width, unsigned int *height, int mapped, canvas_redraw_t exposure_handler, const palette_t *palette, PIXEL *pixel_return)
{
  canvas_t canvas;
  canvas_list_t *newCanvas;

  if ((canvas = (canvas_t)malloc(sizeof(struct _canvas))) == NULL)
    return (canvas_t)0;

  canvas->width = *width; canvas->height = *height;

  canvas->num_colours = (palette == NULL) ? 16 : palette->num_entries;
  canvas->pixel_translation = NULL;
  memset(canvas->colour_table, 0, 256*sizeof(int));
  canvas->shiftx = 0; canvas->shifty = 0; canvas->scale = 1;
  canvas->fb.tmpframebuffer = NULL;

  canvas_set_palette(canvas, palette, pixel_return);

  if ((newCanvas = (canvas_list_t*)malloc(sizeof(canvas_list_t))) == NULL)
  {
    free(canvas); return NULL;
  }

  newCanvas->next = NULL; newCanvas->canvas = canvas;

  if (CanvasList == NULL)
  {
    CanvasList = newCanvas;
    canvas->window = EmuWindow;
    ActiveCanvas = canvas;
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

  wimp_window_set_extent(canvas->window, 0, - *height << UseEigen, *width << UseEigen, 0);
  ui_open_emu_window(canvas->window, NULL);

  if (canvas->window == EmuWindow)
  {
    Wimp_GetCaretPosition(&LastCaret);
    Wimp_SetCaretPosition(EmuWindow->Handle, -1, -100, 100, -1, -1);
  }

  NumberOfCanvases++;

  return canvas;
}


void canvas_destroy(canvas_t s)
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
  if (FullScreenMode == 0)
  {
    wimp_window_set_extent(s->window, 0, -height << UseEigen, width << UseEigen, 0);
    Wimp_GetWindowState((int*)(s->window));
    /* Only open window if it was open to begin with */
    if ((s->window->wflags & (1<<16)) != 0)
    {
      Wimp_OpenWindow((int*)(s->window));
    }
  }
}


void canvas_refresh(canvas_t canvas, frame_buffer_t frame_buffer,
			unsigned int xs, unsigned int ys,
			unsigned int xi, unsigned int yi,
			unsigned int w, unsigned int h)
{
  graph_env ge;
  int shiftx, shifty;

  if (ModeChanging != 0) return;

  FrameBufferUpdate = 0;
  ge.dimx = frame_buffer.width; ge.dimy = frame_buffer.height;
  canvas->shiftx = (xi - xs); canvas->shifty = - (yi - ys);

  if (canvas->fb.tmpframebuffer == NULL)
  {
    memcpy(&(canvas->fb), &frame_buffer, sizeof(frame_buffer_t));
  }

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
    shiftx = (canvas->shiftx << UseEigen) * (canvas->scale);
    shifty = (canvas->shifty << UseEigen) * (canvas->scale);

    more = Wimp_UpdateWindow(block);
    while (more != 0)
    {
      /* Coordinates of top left corner of canvas */
      ge.x = block[RedrawB_VMinX] - block[RedrawB_ScrollX] + shiftx;
      ge.y = block[RedrawB_VMaxY] - block[RedrawB_ScrollY] + shifty;

      if (canvas->scale == 1)
      {
        PlotZoom1(&ge, block + RedrawB_CMinX, frame_buffer.tmpframebuffer, canvas->colour_table);
      }
      else
      {
        PlotZoom2(&ge, block + RedrawB_CMinX, frame_buffer.tmpframebuffer, canvas->colour_table);
      }
      more = Wimp_GetRectangle(block);
    }
  }
  else if (canvas == ActiveCanvas)
  {
    int clip[4];
    int dx, dy;
    int clipYlow;

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

    PlotZoom1(&ge, clip, frame_buffer.tmpframebuffer, canvas->colour_table);

  }
}


canvas_t canvas_for_handle(int handle)
{
  canvas_list_t *clist = CanvasList;

  while (clist != NULL)
  {
    if (clist->canvas->window->Handle == handle) return clist->canvas;
    clist = clist->next;
  }
  return NULL;
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



void video_full_screen_colours(void)
{
  char *sdata, *limit;

  if (FullScreenMode == 0) return;

  /* Set the palette first thing */
  if (ScreenSetPalette != 0)
  {
    canvas_t canvas = ActiveCanvas;
    unsigned int num_colours = ActiveCanvas->num_colours;

    if ((canvas != NULL) && ((1 << (1 << FullScrDesc.ldbpp)) >= num_colours) && (FullScrDesc.ldbpp <= 3))
    {
      unsigned char entries[3 * num_colours];
      unsigned int i;
      unsigned int *ct;

      ct = canvas->colour_table;
      if (ScreenMode.ldbpp == 4)
      {
        for (i=0; i<num_colours; i++)
        {
          /* Lossy, but shouldn't be too bad */
          entries[3*i] = (ct[i] & 0x1f) << 3;
          entries[3*i+1] = (ct[i] & 0x3e0) >> 2;
          entries[3*i+2] = (ct[i] & 0x7c00) >> 7;
        }
      }
      else
      {
        for (i=0; i<num_colours; i++)
        {
          entries[3*i] = ct[i] & 0xff;
          entries[3*i+1] = (ct[i] & 0xff00) >> 8;
          entries[3*i+2] = (ct[i] & 0xff0000) >> 16;
        }
      }
      InstallPaletteRange(entries, 0, num_colours);
      ColourTrans_InvalidateCache();
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
  if (SwitchToMode(&oldScreenMode, NULL) != NULL)
    return -1;

  SingleTasking = oldSingleTask;
  FullScreenMode = 0;

  return 0;
}


int video_full_screen_refresh(void)
{
  canvas_t canvas = ActiveCanvas;

  if ((FullScreenMode == 0) || (canvas == NULL)) return -1;

  /* Clear screen and force a repaint of the entire bitmap */
  ColourTrans_SetGCOL(0, 0x100, 0);
  OS_Plot(0x04, 0, 0); OS_Plot(0x65, FullScrDesc.resx, FullScrDesc.resy);

  canvas_refresh(canvas, canvas->fb, -canvas->shiftx, canvas->shifty, 0, 0, canvas->width, canvas->height);

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
    char statText[64];
    char *b;

    b = statText;
    b += sprintf(b, "%3d%% / %2d fps", SpeedPercentage, FrameRate);
    if (WarpModeEnabled != 0)
      b += sprintf(b, " (warp)");
    if (DriveTrackNumbers[0] != 0)
      b += sprintf(b, "; 8: %2d.%d", DriveTrackNumbers[0] >> 1, 5*(DriveTrackNumbers[0] & 1));
    if (DriveTrackNumbers[1] != 0)
      b += sprintf(b, "; 9: %2d.%d", DriveTrackNumbers[1] >> 1, 5*(DriveTrackNumbers[1] & 1));

    if (strcmp(statText, LastStatusLine) != 0)
    {
      int width;

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
