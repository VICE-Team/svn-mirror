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
static screen_mode_t newScreenModeNorm;
static screen_mode_t newScreenModePAL;
static screen_mode_t newScreenModeDouble;
static screen_mode_t oldScreenMode;
static int newScreenValidNorm = 0;
static int newScreenValidPAL = 0;
static int newScreenValidDouble = 0;
static int oldSingleTask;
static int newModesAvailable;
static RO_Screen FullScrDesc;
static int FullUseEigen;
sprite_area_t *SpriteArea;
sprite_desc_t *SpriteLED0=NULL;
sprite_desc_t *SpriteLED1=NULL;
static sprite_plotenv_t led0plot;
static sprite_plotenv_t led1plot;
static int SpeedPercentage;
static int FrameRate;
static int WarpModeEnabled;
static int SpriteLEDWidth = 0;
static int SpriteLEDHeight = 0;
static int SpriteLEDMode = 0;
static char LastStatusLine[STATUS_LINE_SIZE];
static char CurrentDriveImage[64] = "";
static int ActualPALDepth = 0;

static const int FullBackColour   = 0x00000010;
static const int StatusBackColour = 0xcccccc10;
static const int StatusForeColour = 0x22222210;
/* Sizes in pixels */
static const int StatusLineHeight = 20;
static const int StatusCharSize = 8;
static const int StatusLEDSpace = 16;



static char *ScreenModeNormString = NULL;
static char *ScreenModePALString = NULL;
static char *ScreenModeDoubleString = NULL;

static int ScreenSetPalette;
static int UseBPlotModule;
static int PALEmuDepth;
static int PALEmuDouble;


static void video_full_screen_colours(void);
static void canvas_redraw_all(void);


/*
 *  Some code to encapsulate canvas scaling.
 *  ``Full'' refers to the settings in videoconfig (all that's needed in full screen mode)
 *  ``Soft'' is the setting of the RISC OS plotter (at the very end of the rendering pipeline)
 */

static void video_canvas_get_full_scale(const video_canvas_t *canvas, int *sx, int *sy)
{
  const video_render_config_t *vc = canvas->videoconfig;
  int rendermode = vc->rendermode;

  *sx = 1; *sy = 1;
  if ((ActualPALDepth != 0)
      && ((rendermode == VIDEO_RENDER_PAL_1X1) || (rendermode == VIDEO_RENDER_PAL_2X2)))
  {
    if (vc->doublesizex != 0)
      *sx = 2;
    if (vc->doublesizey != 0)
      *sy = 2;
  }
  if (rendermode == VIDEO_RENDER_RGB_1X2)
  {
    if (vc->doublesizey != 0)
      *sy = 2;
  }
}


static void video_canvas_get_soft_scale(const video_canvas_t *canvas, int *sx, int *sy)
{
  if (canvas->scale == 1)
  {
    *sx = 1; *sy = 1;
  }
  else
  {
    *sx = 2; *sy = 2;
  }
  if (canvas->videoconfig->rendermode == VIDEO_RENDER_RGB_1X2)
  {
    *sy *= 2;
  }
}


static void video_canvas_apply_soft_scale(const video_canvas_t *canvas, int *sx, int *sy)
{
  if (canvas->scale != 1)
  {
    *sx *= 2; *sy *= 2;
  }
}


static void video_canvas_get_scale(const video_canvas_t *canvas, int *sx, int *sy)
{
  video_canvas_get_full_scale(canvas, sx, sy);
  video_canvas_apply_soft_scale(canvas, sx, sy);
}




/*
 *  Init raw RGB colours
 */

static void video_init_raw_rgb(void)
{
  if (ActualPALDepth > 8)
  {
    unsigned int i;
    /* only does something for true colour modes */
    if (ActualPALDepth == 16)
    {
      for (i=0; i<256; i++)
      {
        DWORD c = i & 0xf8;
        video_render_setrawrgb(i, (DWORD)(c>>3), (DWORD)(c<<2), (DWORD)(c<<7));
      }
    }
    else if (ActualPALDepth == 32)
    {
      for (i=0; i<256; i++)
      {
        video_render_setrawrgb(i, (DWORD)i, (DWORD)(i<<8), (DWORD)(i<<16));
      }
    }
    video_render_initraw();
  }
}


/*
 *  Convert a PAL depth handle to an actual depth
 */

static void video_init_pal_depth(void)
{
  switch (PALEmuDepth)
  {
    case PAL_EMU_DEPTH_NONE:
      ActualPALDepth = 0; break;
    case PAL_EMU_DEPTH_AUTO:
      {
        int ldbpp = (FullScreenMode == 0) ? ScreenMode.ldbpp : FullScrDesc.ldbpp;

        if (ldbpp >= 3)
          ActualPALDepth = (1 << ldbpp);
        else
          ActualPALDepth = 8;
      }
      break;
    case PAL_EMU_DEPTH_8:
      ActualPALDepth = 8; break;
    case PAL_EMU_DEPTH_16:
      ActualPALDepth = 16; break;
    case PAL_EMU_DEPTH_32:
      ActualPALDepth = 32; break;
    default:
      ActualPALDepth = 0; break;
  }
  video_init_raw_rgb();

  resources_set_value("PALEmulation", (resource_value_t)(ActualPALDepth != 0));
}


static int video_init_pal_videoconfig(video_render_config_t *vc)
{
  if ((vc->rendermode == VIDEO_RENDER_PAL_1X1) || (vc->rendermode == VIDEO_RENDER_PAL_2X2))
  {
    vc->doublesizex = PALEmuDouble;
    vc->doublesizey = PALEmuDouble;
    vc->doublescan  = PALEmuDouble;

    vc->rendermode = (PALEmuDouble == 0) ? VIDEO_RENDER_PAL_1X1 : VIDEO_RENDER_PAL_2X2;

    return 1;
  }
  return 0;
}


static void video_init_pal_double(void)
{
  canvas_list_t *cl = CanvasList;

  while (cl != NULL)
  {
    if (video_init_pal_videoconfig(cl->canvas->videoconfig) != 0)
    {
      video_canvas_update_size(cl->canvas);
    }
    cl = cl->next;
  }
}


static int parse_screen_mode_string(const char *str, char **modestr, screen_mode_t *mode)
{
  const char *rest;
  int modenum, resx, resy, depth;

  if ((modestr != NULL) && (str != NULL) && (strcmp(str, *modestr) == 0))
    return 0;

  resx = 0; resy = 0; depth = 0;
  modenum = strtol(str, (char**)&rest, 0);
  if (rest == str) return -1;
  if (*rest == ':')
  {
    const char *s = rest+1;

    resx = strtol(s, (char**)&rest, 0);
    if ((rest == s) || (*rest != ',')) return -1;
    s = rest+1;
    resy = strtol(s, (char**)&rest, 0);
    if ((rest == s) || (*rest != ',')) return -1;
    s = rest+1;
    depth = strtol(s, (char**)&rest, 0);
    if ((rest == s) || (*rest != '\0')) return -1;
  }

  newModesAvailable = CheckNewModes();

  if ((newModesAvailable == 0) || (resx == 0))
  {
    mode->mode.mode_num = modenum;
  }
  else
  {
    int i;

    mode->mode.mode_ptr = mode->new_mode;
    mode->new_mode[0] = 1;
    mode->new_mode[1] = resx;
    mode->new_mode[2] = resy;
    mode->new_mode[3] = depth;
    mode->new_mode[4] = -1;

    i = 5;
    if (depth < 4)
    {
      /* Set the number of palette entries according to the mode depth */
      mode->new_mode[i++] = 3;
      mode->new_mode[i++] = (1 << (1 << depth) ) - 1;
    }
    mode->new_mode[i++] = -1;
  }

  util_string_set(modestr, str);

  return 0;
}


static int set_screen_mode_norm(resource_value_t v, void *param)
{
  if (parse_screen_mode_string((const char *)v, &ScreenModeNormString, &newScreenModeNorm) == 0)
  {
    newScreenValidNorm = 1;
    return 0;
  }
  newScreenValidNorm = 0;
  return -1;
}

static int set_screen_mode_pal(resource_value_t v, void *param)
{
  if (parse_screen_mode_string((const char *)v, &ScreenModePALString, &newScreenModePAL) == 0)
  {
    newScreenValidPAL = 1;
    return 0;
  }
  newScreenValidPAL = 0;
  return -1;
}

static int set_screen_mode_double(resource_value_t v, void *param)
{
  if (parse_screen_mode_string((const char *)v, &ScreenModeDoubleString, &newScreenModeDouble) == 0)
  {
    newScreenValidDouble = 1;
    return 0;
  }
  newScreenValidDouble = 0;
  return -1;
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

static int set_pal_emu_depth(resource_value_t v, void *param)
{
  PALEmuDepth = (int)v;
  video_init_pal_depth();
  video_color_update_palette();
  /* to rescale canvases if necessary */
  video_init_pal_double();
  canvas_redraw_all();
  return 0;
}

static int set_pal_emu_double(resource_value_t v, void *param)
{
  PALEmuDouble = (int)v;
  video_init_pal_double();
  return 0;
}


static resource_t resources[] = {
  {"ScreenMode", RES_STRING, (resource_value_t)"28:640,480,3",
    (resource_value_t *)&ScreenModeNormString, set_screen_mode_norm, NULL },
  {"ScreenModePAL", RES_STRING, (resource_value_t)"28:640,480,5",
    (resource_value_t *)&ScreenModePALString, set_screen_mode_pal, NULL },
  {"ScreenModeDouble", RES_STRING, (resource_value_t)"31:800,600,5",
    (resource_value_t *)&ScreenModeDoubleString, set_screen_mode_double, NULL },
  {"ScreenSetPalette", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *)&ScreenSetPalette, set_screen_palette, NULL },
  {"UseBPlot", RES_INTEGER, (resource_value_t) 1,
    (resource_value_t *)&UseBPlotModule, set_bplot_status, NULL },
  {"PALEmuDepth", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *)&PALEmuDepth, set_pal_emu_depth, NULL },
  {"PALEmuDouble", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *)&PALEmuDouble, set_pal_emu_double, NULL },
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

  /* use a dummy palette here */
  for (i=0; i<256; i++) palette[i] = 0x10;
  /*log_message(LOG_DEFAULT, "Alloc %d x %d", fb_width, fb_height);*/

  if ((sarea = wlsprite_create_area_sprite(fb_width, fb_height, 8, "viceframe", palette, 0)) != NULL)
  {
    sprite_desc_t *sprite = wlsprite_area_get_sprite(sarea);

    if (canvas != NULL)
    {
      canvas->fb.width = fb_width;
      canvas->fb.height = fb_height;
      canvas->fb.depth = 8;
      canvas->fb.pitch = (sprite->wwidth+1)*4;
      canvas->fb.framedata = (BYTE*)wlsprite_get_image(sprite);
      /* mark the palette as dirty since we used a dummy above! */
      canvas->fb.paldirty = 1;

      wlsprite_plot_bind(&(canvas->fb.normplot), sarea);

      /*log_message(LOG_DEFAULT, "width = %d, height = %d, pitch = %d", canvas->fb.width, canvas->fb.height, canvas->fb.pitch);*/
    }
    *fb_pitch = (sprite->wwidth+1)*4;

    *draw_buffer = (BYTE*)wlsprite_get_image(sprite);

    return 0;
  }
  return -1;
}


static void video_frame_buffer_flush_pal(video_canvas_t *canvas)
{
  sprite_area_t *sarea;

  if ((sarea = wlsprite_plot_get_sarea(&(canvas->fb.palplot))) != NULL)
  {
    free(sarea);
    wlsprite_plot_bind(&(canvas->fb.palplot), (sprite_area_t*)NULL);
    canvas->fb.paldata = NULL;
  }
}


static void video_frame_buffer_free(video_canvas_t *canvas, BYTE *draw_buffer)
{
  sprite_area_t *sarea;
  video_frame_buffer_t *fb = &(canvas->fb);

  if ((sarea = wlsprite_plot_get_sarea(&(canvas->fb.normplot))) != NULL)
  {
    free(sarea);
    wlsprite_plot_bind(&(canvas->fb.normplot), (sprite_area_t*)NULL);
    canvas->fb.framedata = NULL;
  }
  video_frame_buffer_flush_pal(canvas);

  if (fb->bplot_trans != NULL)
  {
    free(fb->bplot_trans);
    fb->bplot_trans = NULL;
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
  /* Frame buffer not allocated yet? Then can't do anything */
  if (wlsprite_plot_get_sarea(&(canvas->fb.normplot)) == NULL)
    return;

  /* the frame buffer palette is dirty if the palette changed while there was no
     frame buffer allocated, e.g. on startup */
  if ((canvas->fb.paldirty != 0) && (canvas->current_palette != NULL))
  {
    unsigned int i, numsprpal;
    sprite_desc_t *sprite;
    unsigned int *sprpal;
    unsigned int *ct = canvas->current_palette;

    sprite = wlsprite_plot_get_sprite(&(canvas->fb.normplot));
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

    canvas->fb.transdirty = 1;
  }

  /* do we need to make a new sprite translation table? */
  if (canvas->fb.transdirty != 0)
  {
    unsigned int ldbpp;

    wlsprite_plot_flush(&(canvas->fb.normplot));

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


static const char *palspritename = "palsprite";

static int video_ensure_pal_sprite(video_canvas_t *canvas, int *pitchs, int *pitcht)
{
  sprite_desc_t *sprite;

  if (wlsprite_plot_get_sarea(&(canvas->fb.palplot)) == NULL)
  {
    video_frame_buffer_t *fb = &(canvas->fb);
    sprite_area_t *sarea;
    int width, height;

    width  = (canvas->videoconfig->doublesizex == 0) ? canvas->width  : 2*canvas->width;
    height = (canvas->videoconfig->doublesizey == 0) ? canvas->height : 2*canvas->height;

    if (ActualPALDepth == 8)
    {
      unsigned int dummypal[256];

      memset(dummypal, 0, 256*sizeof(int));
      sarea = wlsprite_create_area_sprite(width, height, ActualPALDepth, palspritename, dummypal, 0);
    }
    else
    {
      sarea = wlsprite_create_area_sprite(width, height, ActualPALDepth, palspritename, NULL, 0);
    }
    if (sarea == NULL)
      return -1;

    wlsprite_plot_bind(&(canvas->fb.palplot), sarea);

    sprite = wlsprite_plot_get_sprite(&(canvas->fb.palplot));
    fb->paldata = wlsprite_get_image(sprite);
    memset(fb->paldata, 0, height * (sprite->wwidth+1)*4);

    /*log_message(LOG_DEFAULT, "Sprite width %d, height %d, mode %08x", wlsprite_get_width(sprite), wlsprite_get_height(sprite), sprite->sprmode);*/
  }
  sprite = wlsprite_plot_get_sprite(&(canvas->fb.palplot));
  *pitchs = canvas->fb.pitch;
  *pitcht = (sprite->wwidth+1)*4;

  return 0;
}


static void video_ensure_pal_colours(video_canvas_t *canvas)
{
  if (canvas->last_video_render_depth != ActualPALDepth)
  {
    video_render_config_t *config = canvas->videoconfig;
    unsigned int *ct;
    unsigned int i;

    log_message(LOG_DEFAULT, "Rebinding PAL colours for %s (%d colours)", canvas->name, canvas->num_colours);

    ct = canvas->current_palette;

    switch (ActualPALDepth)
    {
      case 8:
        for (i=0; i<canvas->num_colours; i++)
        {
          video_render_setphysicalcolor(config, (int)i, (DWORD)i, ActualPALDepth);
        }
        if (wlsprite_plot_get_sarea(&(canvas->fb.palplot)) != NULL)
        {
          sprite_desc_t *sprite;
          unsigned int *sprpal;

          sprite = wlsprite_plot_get_sprite(&(canvas->fb.palplot));
          sprpal = (unsigned int*)(sprite + 1);

          for (i=0; i<canvas->num_colours; i++)
          {
            sprpal[2*i] = 0x10 | (ct[i] << 8);
            sprpal[2*i+1] = sprpal[2*i];
          }
          for (; i<256; i++)
          {
            sprpal[2*i] = 0x10;
            sprpal[2*i+1] = 0x10;
          }
        }
        break;
      case 16:
        for (i=0; i<canvas->num_colours; i++)
        {
          unsigned int v = ct[i];
          BYTE r, g, b;

          r = v & 0xff; g = (v >> 8) & 0xff; b = (v >> 16) & 0xff;
          v = (r >> 3) | ((g & 0xf8) << 2) | ((b & 0xf8) << 7);
          video_render_setphysicalcolor(config, (int)i, (DWORD)v, ActualPALDepth);
        }
        break;
      case 32:
        for (i=0; i<canvas->num_colours; i++)
          video_render_setphysicalcolor(config, (int)i, (DWORD)(ct[i]), ActualPALDepth);
        break;
      default:
        break;
    }
    canvas->last_video_render_depth = ActualPALDepth;
  }
}




/*
 *  redraw cores
 */
static void video_redraw_wimp_sprite(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  if (canvas->scale == 1)
  {
    wlsprite_plot_plot(&(canvas->fb.normplot), vrd->ge.x, vrd->ge.y - (canvas->fb.height << ScreenMode.eigy), NULL);
  }
  else
  {
    int scale[4];

    scale[0] = 2; scale[1] = 2; scale[2] = 1; scale[3] = 1;
    wlsprite_plot_plot(&(canvas->fb.normplot), vrd->ge.x, vrd->ge.y - 2*(canvas->fb.height << ScreenMode.eigy), scale);
  }
}


static void video_redraw_wimp_sprite2(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  int basescale;
  int scale[4];

  basescale = (canvas->scale == 1) ? 1 : 2;
  scale[0] = basescale; scale[1] = 2*basescale; scale[2] = 1; scale[3] = 1;

  wlsprite_plot_plot(&(canvas->fb.normplot), vrd->ge.x, vrd->ge.y - ((2*basescale*canvas->fb.height) << ScreenMode.eigy), scale);
}


static void video_redraw_wimp_bplot(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  if (canvas->scale == 1)
  {
    PlotZoom1(&(vrd->ge), vrd->block + RedrawB_CMinX, canvas->fb.framedata, canvas->fb.bplot_trans);
  }
  else
  {
    PlotZoom2(&(vrd->ge), vrd->block + RedrawB_CMinX, canvas->fb.framedata, canvas->fb.bplot_trans);
  }
}


static void video_redraw_wimp_palemu(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  int pitchs, pitcht;
  video_frame_buffer_t *fb = &(canvas->fb);
  int xt, yt, px, py, w, h, pw, ph;
  int scalex, scaley, softx, softy;

  video_canvas_get_full_scale(canvas, &scalex, &scaley);

  pw = (canvas->width) * scalex;
  ph = (canvas->height) * scaley;

  /* new interface, only xt may be 2x */
  xt = (vrd->xs + canvas->shiftx) * scalex;
  yt = (vrd->ys - canvas->shifty) * scaley;
  if ((xt >= pw) || (yt >= ph))
    return;

  /* order is important here! */
  if (video_ensure_pal_sprite(canvas, &pitchs, &pitcht) != 0)
    return;

  video_ensure_pal_colours(canvas);

  w = vrd->w * scalex;
  h = vrd->h * scaley;
  if (xt < 0) xt = 0;
  if (xt + w > pw) w = pw - xt;
  if (yt < 0) yt = 0;
  if (yt + h > ph) h = ph - yt;
  /*log_message(LOG_DEFAULT, "s(%d,%d), t(%d,%d), d(%d,%d)", vrd->xs, vrd->ys, xt, yt, w, h);*/
  video_canvas_render(canvas, fb->paldata, w, h,
                    vrd->xs, vrd->ys, xt, yt, pitcht, ActualPALDepth);

  video_canvas_get_soft_scale(canvas, &softx, &softy);

  px = vrd->ge.x - (canvas->shiftx << ScreenMode.eigx) * softx;
  py = vrd->ge.y - ((canvas->shifty + ph) << ScreenMode.eigy) * softy;
  /*log_message(LOG_DEFAULT, "Plot at %d,%d (clip %d,%d)", px, py, vrd->block[RedrawB_CMinX], vrd->block[RedrawB_CMinY]);*/

  if (canvas->scale == 1)
  {
    wlsprite_plot_plot(&(fb->palplot), px, py, NULL);
  }
  else
  {
    int scale[4];

    scale[0] = 2; scale[1] = 2; scale[2] = 1; scale[3] = 1;
    wlsprite_plot_plot(&(fb->palplot), px, py, scale);
  }
}


static void video_redraw_full_sprite(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  const int *b = vrd->block;

  /* Must explicitly set the clipping rectangle */
  video_set_clipping_rectangle(b[0], b[1], b[2], b[3]);

  wlsprite_plot_plot(&(canvas->fb.normplot), vrd->ge.x, vrd->ge.y - (canvas->fb.height << FullScrDesc.eigy), NULL);
}


static void video_redraw_full_sprite2(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  const int *b = vrd->block;
  int scale[4];

  video_set_clipping_rectangle(b[0], b[1], b[2], b[3]);

  scale[0] = 1; scale[1] = 2; scale[2] = 1; scale[3] = 1;

  wlsprite_plot_plot(&(canvas->fb.normplot), vrd->ge.x, vrd->ge.y - ((2*canvas->fb.height) << FullScrDesc.eigy), scale);
}


static void video_redraw_full_bplot(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  PlotZoom1(&(vrd->ge), vrd->block, canvas->fb.framedata, canvas->fb.bplot_trans);
}


static void video_redraw_full_palemu(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  int pitchs, pitcht;
  video_frame_buffer_t *fb = &(canvas->fb);
  const int *b = vrd->block;
  int xt, yt, px, py, w, h, pw, ph;
  int scalex, scaley;

  video_canvas_get_full_scale(canvas, &scalex, &scaley);

  pw = (canvas->width) * scalex;
  ph = (canvas->height) * scaley;

  xt = (vrd->xs + canvas->shiftx) * scalex;
  yt = (vrd->ys - canvas->shifty) * scaley;
  if ((xt >= pw) || (yt >= ph))
    return;

  video_set_clipping_rectangle(b[0], b[1], b[2], b[3]);

  /* order is important here! */
  if (video_ensure_pal_sprite(canvas, &pitchs, &pitcht) != 0)
    return;

  video_ensure_pal_colours(canvas);

  w = vrd->w * scalex;
  h = vrd->h * scaley;
  if (xt < 0) xt = 0;
  if (xt + w > pw) w = pw - xt;
  if (yt < 0) yt = 0;
  if (yt + h > ph) h = ph - yt;

  video_canvas_render(canvas, fb->paldata, w, h,
                    vrd->xs, vrd->ys, xt, yt, pitcht, ActualPALDepth);

  px = vrd->ge.x - (canvas->shiftx << FullScrDesc.eigx) * scalex;
  py = vrd->ge.y - ((canvas->shifty * scaley + ph) << FullScrDesc.eigy);

  wlsprite_plot_plot(&(fb->palplot), px, py, NULL);
}



/*
 * Init redraw core functions
 */

static const char *redraw_core_name_pal8 = "PAL Emu 8";
static const char *redraw_core_name_pal16 = "PAL Emu 16";
static const char *redraw_core_name_pal32 = "PAL Emu 32";
static const char *redraw_core_name_bplot = "BPlot";
static const char *redraw_core_name_sprite = "SpriteOp";
static const char *redraw_core_name_sprite2 = "SpriteOp (1x2)";


static const char *video_get_palemu_name(int depth)
{
  switch (depth)
  {
    case 16:
      return redraw_core_name_pal16;
    case 32:
      return redraw_core_name_pal32;
    default:
      return redraw_core_name_pal8;
  }
}


static void video_get_redraw_wimp(video_canvas_t *canvas)
{
  int rendermode = canvas->videoconfig->rendermode;
  const char *corename = NULL;

  if (wlsprite_plot_get_sarea(&(canvas->fb.normplot)) == NULL)
    return;

  if ((ActualPALDepth != 0)
   && ((rendermode == VIDEO_RENDER_PAL_1X1) || (rendermode == VIDEO_RENDER_PAL_2X2)))
  {
    corename = video_get_palemu_name(ActualPALDepth);
    canvas->redraw_wimp = video_redraw_wimp_palemu;
  }
  else
  {
    /* Use bplot or sprite plot interface? (this code only called from desktop) */
    if ((UseBPlotModule != 0) && (ScreenMode.ldbpp >= 4) && (rendermode != VIDEO_RENDER_RGB_1X2))
    {
      corename = redraw_core_name_bplot;
      canvas->redraw_wimp = video_redraw_wimp_bplot;
    }
    else
    {
      if (rendermode == VIDEO_RENDER_RGB_1X2)
      {
        corename = redraw_core_name_sprite2;
        canvas->redraw_wimp = video_redraw_wimp_sprite2;
      }
      else
      {
        corename = redraw_core_name_sprite;
        canvas->redraw_wimp = video_redraw_wimp_sprite;
      }
    }
  }
  log_message(LOG_DEFAULT, "Using %s for WIMP redraws of %s.", corename, canvas->name);
}


static void video_get_redraw_full(video_canvas_t *canvas)
{
  int rendermode = canvas->videoconfig->rendermode;
  const char *corename = NULL;

  if (wlsprite_plot_get_sarea(&(canvas->fb.normplot)) == NULL)
    return;

  if ((ActualPALDepth != 0)
   && ((rendermode == VIDEO_RENDER_PAL_1X1) || (rendermode == VIDEO_RENDER_PAL_2X2)))
  {
    corename = video_get_palemu_name(ActualPALDepth);
    canvas->redraw_full = video_redraw_full_palemu;
  }
  else
  {
    if ((UseBPlotModule != 0) && (rendermode != VIDEO_RENDER_RGB_1X2)
     && ((FullScrDesc.ldbpp >= 4) || (ScreenSetPalette != 0)))
    {
      corename = redraw_core_name_bplot;
      canvas->redraw_full = video_redraw_full_bplot;
    }
    else
    {
      if (rendermode == VIDEO_RENDER_RGB_1X2)
      {
        corename = redraw_core_name_sprite2;
        canvas->redraw_full = video_redraw_full_sprite2;
      }
      else
      {
        corename = redraw_core_name_sprite;
        canvas->redraw_full = video_redraw_full_sprite;
      }
    }
  }
  log_message(LOG_DEFAULT, "Using %s for full screen redraws of %s.", corename, canvas->name);
}



void video_canvas_redraw_core(video_canvas_t *canvas, video_redraw_desc_t *vrd)
{
  if (canvas->fb.framedata != NULL)
  {
    int shiftx, shifty;
    const int *b = vrd->block;
    int scalex, scaley;

    video_canvas_get_soft_scale(canvas, &scalex, &scaley);

    shiftx = (canvas->shiftx << UseEigen) * scalex;
    shifty = (canvas->shifty << UseEigen) * scaley;

    /* Coordinates of top left corner of canvas */
    vrd->ge.x = b[RedrawB_VMinX] - b[RedrawB_ScrollX] + shiftx;
    vrd->ge.y = b[RedrawB_VMaxY] - b[RedrawB_ScrollY] + shifty;

    video_canvas_ensure_translation(canvas);

    if (canvas->redraw_wimp == NULL)
      video_get_redraw_wimp(canvas);

    if (canvas->redraw_wimp != NULL)
      (*canvas->redraw_wimp)(canvas, vrd);
  }
}



int video_canvas_set_palette(video_canvas_t *canvas, const palette_t *palette)
{
  video_frame_buffer_t *fb;
  palette_entry_t *p;
  unsigned int numsprpal;
  unsigned int *ct;
  int i;

  if (palette == NULL) return 0;

  if (canvas->current_palette != NULL)
  {
    free(canvas->current_palette);
  }
  canvas->num_colours = palette->num_entries;
  if (canvas->num_colours > 256)
    canvas->num_colours = 256;

  canvas->current_palette = xmalloc((canvas->num_colours)*sizeof(int));

  fb = &(canvas->fb);
  p = palette->entries;
  numsprpal = (1 << fb->depth);

  /*{FILE *fp = fopen("PALETTE", "a+"); fprintf(fp, "%s\n", canvas->name);
  for(i=0;i<canvas->num_colours; i++)
    fprintf(fp, "%3d: %2x.%2x.%2x\n", i, p[i].red, p[i].green, p[i].blue); fclose(fp);}*/

  /* adapt the palette stored in the frame buffer sprite */
  if (wlsprite_plot_get_sarea(&(fb->normplot)) != NULL)
  {
    sprite_desc_t *sprite;
    unsigned int *sprpal;

    sprite = wlsprite_plot_get_sprite(&(fb->normplot));
    sprpal = (unsigned int*)(sprite+1);

    for (i=0; i<canvas->num_colours; i++)
    {
      sprpal[2*i] = 0x10 | (p[i].red << 8) | (p[i].green << 16) | (p[i].blue << 24);
      sprpal[2*i+1] = sprpal[2*i];
    }
    for (; i<numsprpal; i++)
    {
      sprpal[2*i] = 0x10;
      sprpal[2*i+1] = 0x10;
    }
    fb->paldirty = 0;
  }
  else
  {
    /* no frame buffer allocated, make a note that the palette is dirty */
    fb->paldirty = 1;
  }

  wlsprite_plot_flush(&(fb->normplot));
  wlsprite_plot_flush(&(fb->palplot));

  fb->transdirty = 1;

  /* remember the current palette in canvas->current_palette */
  ct = canvas->current_palette;
  for (i=0; i<canvas->num_colours; i++)
  {
    ct[i] = p[i].red | (p[i].green << 8) | (p[i].blue << 16);
  }

  canvas->last_video_render_depth = -1;

  return 0;
}


void video_arch_canvas_init(struct video_canvas_s *canvas)
{
  canvas->video_draw_buffer_callback
        = xmalloc(sizeof(video_draw_buffer_callback_t));
  canvas->video_draw_buffer_callback->draw_buffer_alloc
        = video_frame_buffer_alloc;
  canvas->video_draw_buffer_callback->draw_buffer_free
        = video_frame_buffer_free;
  canvas->video_draw_buffer_callback->draw_buffer_clear
        = video_frame_buffer_clear;

  memset(&(canvas->fb), 0, sizeof(video_frame_buffer_t));
  wlsprite_plot_init(&(canvas->fb.normplot));
  wlsprite_plot_init(&(canvas->fb.palplot));
}

video_canvas_t *video_canvas_create(video_canvas_t *canvas, unsigned int *width, unsigned int *height, int mapped, const palette_t *palette)
{
  canvas_list_t *newCanvas;

  canvas->name = stralloc(canvas->viewport->title);

  canvas->width = *width; canvas->height = *height;

  if (canvas->videoconfig->doublesizex)
    canvas->width *= 2;

  if (canvas->videoconfig->doublesizey)
    canvas->height *= 2;

  canvas->num_colours = (palette == NULL) ? 16 : palette->num_entries;
  canvas->current_palette = NULL;
  canvas->shiftx = 0; canvas->shifty = 0; canvas->scale = 1;
  canvas->redraw_wimp = NULL;
  canvas->redraw_full = NULL;
  canvas->last_video_render_depth = -1;
  canvas->fb.transdirty = 1;

  video_init_pal_videoconfig(canvas->videoconfig);

  video_canvas_set_palette(canvas, palette);

  if ((newCanvas = (canvas_list_t*)malloc(sizeof(canvas_list_t))) == NULL)
  {
    free(canvas);
    return NULL;
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
      free(canvas); free(newCanvas);
      return NULL;
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
  video_frame_buffer_t *fb;

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

  free(s->name);
  s->name = NULL;
  fb = &(s->fb);

  if (s->video_draw_buffer_callback != NULL)
    free(s->video_draw_buffer_callback);

  if (s->current_palette != NULL)
  {
    free(s->current_palette);
    s->current_palette = NULL;
  }

  free(s);
}


void video_canvas_map(video_canvas_t *s)
{
}


void video_canvas_unmap(video_canvas_t *s)
{
}


void video_canvas_resize(video_canvas_t *canvas, unsigned int width, unsigned int height)
{
  /* Make a note of the resize, too */
  canvas->width = width; canvas->height = height;
  if (FullScreenMode == 0)
  {
    int scalex, scaley;
    int w, h;

    video_canvas_get_scale(canvas, &scalex, &scaley);

    w = (scalex * width) << UseEigen; h = (scaley * height) << UseEigen;
    wimp_window_set_extent(canvas->window, 0, -h, w, 0);
    Wimp_GetWindowState((int*)(canvas->window));
    /* Only open window if it was open to begin with */
    if ((canvas->window->wflags & (1<<16)) != 0)
    {
      Wimp_OpenWindow((int*)(canvas->window));
    }
  }
}


void video_canvas_refresh(video_canvas_t *canvas,
 			  unsigned int xs, unsigned int ys,
			  unsigned int xi, unsigned int yi,
			  unsigned int w, unsigned int h)
{
  video_redraw_desc_t vrd;

  if (ModeChanging != 0) return;

  if (canvas->fb.framedata == NULL) return;

  FrameBufferUpdate = 0;
  canvas->shiftx = (xi - xs); canvas->shifty = - (yi - ys);
  vrd.ge.dimx = canvas->fb.pitch; vrd.ge.dimy = canvas->fb.height;

  vrd.xs = xs; vrd.ys = ys; vrd.w = w; vrd.h = h;

  /*log_message(LOG_DEFAULT, "Refresh s(%d,%d), t(%d,%d), d(%d,%d)", xs, ys, xi, yi, w, h);*/

  if (FullScreenMode == 0)
  {
    int block[11];
    int scalex, scaley;
    int more;

    video_canvas_get_scale(canvas, &scalex, &scaley);

    block[0] = canvas->window->Handle;
    /* The canvas size is only used for the clipping */
    block[1] = (xi << UseEigen) * scalex;
    block[2] = (- (yi + h) << UseEigen) * scaley;
    block[3] = ((xi + w) << UseEigen) * scalex;
    block[4] = (-yi << UseEigen) * scaley;

    vrd.block = block;

    more = Wimp_UpdateWindow(block);
    while (more != 0)
    {
      video_canvas_redraw_core(canvas, &vrd);
      more = Wimp_GetRectangle(block);
    }
  }
  else if (canvas == ActiveCanvas)
  {
    int clip[4];
    int dx, dy, orgx, orgy;
    int clipYlow;
    int shiftx, shifty;
    int scalex, scaley;

    video_canvas_ensure_translation(canvas);

    if (FullScreenStatLine == 0)
      clipYlow = 0;
    else
      clipYlow = (StatusLineHeight << FullScrDesc.eigy);

    video_canvas_get_full_scale(canvas, &scalex, &scaley);

    dx = (canvas->width << FullScrDesc.eigx) * scalex;
    dy = (canvas->height << FullScrDesc.eigy) * scaley;
    shiftx = (canvas->shiftx << FullScrDesc.eigx) * scalex;
    shifty = (canvas->shifty << FullScrDesc.eigy) * scaley;
    orgx = (FullScrDesc.resx - dx)/2;
    orgy = (FullScrDesc.resy + dy)/2;
    vrd.ge.x = orgx + shiftx;
    vrd.ge.y = orgy + shifty;
    clip[0] = orgx + (xi << FullScrDesc.eigx) * scalex;
    clip[2] = clip[0] + (w << FullScrDesc.eigx) * scalex;
    clip[1] = orgy - ((yi + h) << FullScrDesc.eigy) * scaley;
    clip[3] = clip[1] + (h << FullScrDesc.eigy) * scaley;
    /*log_message(LOG_DEFAULT, "CLIP %d,%d,%d,%d", clip[0], clip[1], clip[2], clip[3]);*/

    if ((clip[0] >= FullScrDesc.resx) || (clip[2] < 0)) return;
    if ((clip[1] >= FullScrDesc.resy) || (clip[3] < clipYlow)) return;
    if (clip[0] < 0) clip[0] = 0;
    if (clip[2] > FullScrDesc.resx) clip[2] = FullScrDesc.resx;
    if (clip[1] < clipYlow) clip[1] = clipYlow;
    if (clip[3] > FullScrDesc.resy) clip[3] = FullScrDesc.resy;
    if ((clip[2] <= clip[0]) || (clip[3] <= clip[1])) return;

    vrd.block = clip;

    /*log_message(LOG_DEFAULT, "dx %d, dy %d, px %d, py %d, w %d, h %d, clip %d:%d:%d:%d", dx, dy, vrd.ge.x, vrd.ge.y, w, h, clip[0], clip[1], clip[2], clip[3]);*/

    if (canvas->redraw_full == NULL)
      video_get_redraw_full(canvas);

    if (canvas->redraw_full != NULL)
      (*canvas->redraw_full)(canvas, &vrd);
  }
}


static void canvas_force_redraw(video_canvas_t *canvas)
{
  int eigx, eigy;
  int scalex, scaley;

  if (FullScreenMode == 0)
  {
    eigx = ScreenMode.eigx; eigy = ScreenMode.eigy;
  }
  else
  {
    eigx = FullScrDesc.eigx; eigy = FullScrDesc.eigy;
  }
  video_canvas_get_scale(canvas, &scalex, &scaley);
  scalex <<= eigx; scaley <<= eigy;
  Wimp_ForceRedraw(canvas->window->Handle, 0, -canvas->height*scaley, canvas->width*scalex, 0);
}


static void canvas_redraw_all(void)
{
  canvas_list_t *clist = CanvasList;

  while (clist != NULL)
  {
    canvas_force_redraw(clist->canvas);
    clist = clist->next;
  }
}


void video_canvas_update_extent(video_canvas_t *canvas)
{
  int scalex, scaley;
  int dx, dy;

  UseEigen = (ScreenMode.eigx < ScreenMode.eigy) ? ScreenMode.eigx : ScreenMode.eigy;

  video_canvas_get_scale(canvas, &scalex, &scaley);

  dx = (canvas->width << UseEigen) * scalex;
  dy = (canvas->height << UseEigen) * scaley;

  wimp_window_set_extent(canvas->window, 0, -dy, dx, 0);
}


void video_canvas_update_size(video_canvas_t *canvas)
{
  RO_Window *win;
  int block[WindowB_WFlags+1];
  int dx, dy;

  win = canvas->window;
  video_canvas_update_extent(canvas);
  block[WindowB_Handle] = win->Handle;
  Wimp_GetWindowState(block);
  dx = win->wmaxx - win->wminx;
  dy = win->wmaxy - win->wminy;
  block[WindowB_VMaxX] = block[WindowB_VMinX] + dx;
  block[WindowB_VMinY] = block[WindowB_VMaxY] - dy;
  Wimp_OpenWindow(block);
  Wimp_GetWindowState(block);
  ui_open_emu_window(win, block);
  Wimp_ForceRedraw(win->Handle, 0, -dy, dx, 0);
}


void canvas_mode_change(void)
{
  canvas_list_t *clist = CanvasList;

  /* delete all sprite translation tables, will be recreated on demand */
  while (clist != NULL)
  {
    video_frame_buffer_t *fb = &(clist->canvas->fb);

    wlsprite_plot_flush(&(fb->normplot));
    wlsprite_plot_flush(&(fb->palplot));

    fb->transdirty = 1;
    clist->canvas->redraw_wimp = NULL;
    clist->canvas->redraw_full = NULL;
    clist = clist->next;
  }
  video_init_pal_depth();
  video_color_update_palette();
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


static void video_full_screen_colours(void)
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

  sdata = ((char*)SpriteArea) + SpriteArea->firstoff;
  limit = ((char*)SpriteArea) + SpriteArea->tsize;
  while (sdata < limit)
  {
    if (strncmp(sdata+4, "led_off", 12) == 0) SpriteLED0 = (sprite_desc_t *)sdata;
    else if (strncmp(sdata+4, "led_on", 12) == 0) SpriteLED1 = (sprite_desc_t *)sdata;
    sdata += *((int*)sdata);
  }
  if (SpriteLED0 != NULL)
  {
    OS_SpriteInfo(0x0200, (int*)SpriteArea, SpriteLED0, &SpriteLEDWidth, &SpriteLEDHeight, &SpriteLEDMode);
    SpriteLEDWidth <<= OS_ReadModeVariable(SpriteLEDMode, 4);
    SpriteLEDHeight <<= OS_ReadModeVariable(SpriteLEDMode, 5);
  }
}


int video_full_screen_on(int *sprites)
{
  screen_mode_t *usemode = NULL;

  if (ActualPALDepth == 0)
  {
    if (newScreenValidNorm != 0)
      usemode = &newScreenModeNorm;
  }
  else
  {
    if (ActiveCanvas->videoconfig->rendermode == VIDEO_RENDER_PAL_2X2)
    {
      if (newScreenValidDouble != 0)
        usemode = &newScreenModeDouble;
    }
    else
    {
      if (newScreenValidPAL != 0)
        usemode = &newScreenModePAL;
    }
  }

  if (usemode == NULL) return -1;

  vsync_suspend_speed_eval();

  if (SwitchToMode(usemode, &oldScreenMode) != NULL)
    return -1;

  oldSingleTask = SingleTasking;
  SingleTasking = 1;
  FullScreenMode = 1;
  wimp_read_screen_mode(&FullScrDesc);
  FullUseEigen = (FullScrDesc.eigx < FullScrDesc.eigy) ? FullScrDesc.eigx : FullScrDesc.eigy;
  SpriteArea = (sprite_area_t*)sprites;

  /* clear the background */
  ColourTrans_SetGCOL(FullBackColour, 0x100, 0);
  OS_Plot(0x04, 0, 0);
  OS_Plot(0x65, FullScrDesc.resx, FullScrDesc.resy);

  /* Set text size */
  OS_WriteC(23); OS_WriteC(17); OS_WriteC(7); OS_WriteC(2);
  OS_WriteC(StatusCharSize); OS_WriteC(0); OS_WriteC(StatusCharSize); OS_WriteC(0);
  OS_WriteC(0); OS_WriteC(0);

  video_full_screen_colours();

  wlsprite_plot_init(&led0plot);
  wlsprite_plot_init(&led1plot);
  wlsprite_plot_bind_sprite(&led0plot, SpriteArea, SpriteLED0);
  wlsprite_plot_bind_sprite(&led1plot, SpriteArea, SpriteLED1);

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

  wlsprite_plot_exit(&led0plot);
  wlsprite_plot_exit(&led1plot);

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

  video_canvas_refresh(canvas, -canvas->shiftx, canvas->shifty, 0, 0, canvas->width, canvas->height);

  video_full_screen_init_status();

  return 0;
}


/* DriveLEDStates is already updated in ui.c */
void video_full_screen_drive_leds(unsigned int drive)
{
  if ((FullScreenMode != 0) && (FullScreenStatLine != 0) && (drive < 4))
  {
    sprite_plotenv_t *sp;
    int posx, posy;

    video_full_screen_set_clip();

    posy = ((StatusLineHeight << FullScrDesc.eigy) - SpriteLEDHeight) >> 1;
    posx = FullScrDesc.resx - (4-drive)*(SpriteLEDWidth + (StatusLEDSpace << FullScrDesc.eigx));
    sp = (DriveLEDStates[drive] == 0) ? &led0plot : &led1plot;
    wlsprite_plot_plot(sp, posx, posy, NULL);
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




void video_pos_screen_to_canvas(video_canvas_t *canvas, int *block, int x, int y, int *cx, int *cy)
{
  int scalex, scaley;

  video_canvas_get_scale(canvas, &scalex, &scaley);

  *cx = (((x - (block[RedrawB_VMinX] - block[RedrawB_ScrollX])) >> ScreenMode.eigx) / scalex) - canvas->shiftx;
  *cy = ((((block[RedrawB_VMaxY] - block[RedrawB_ScrollY]) - y) >> ScreenMode.eigy) / scaley) + canvas->shifty;
}



/*
 *  Callbacks
 */

static void callback_canvas_modified(const char *name, void *callback_param)
{
  canvas_list_t *clist = CanvasList;

  /* invalidate all redraw function pointers */
  while (clist != NULL)
  {
    video_canvas_t *canvas = clist->canvas;

    canvas->redraw_wimp = NULL;
    canvas->redraw_full = NULL;
    canvas->last_video_render_depth = -1;

    video_frame_buffer_flush_pal(canvas);

    clist = clist->next;
  }
  canvas_redraw_all();
}


void video_register_callbacks(void)
{
  resources_register_callback("PALMode", callback_canvas_modified, NULL);
  resources_register_callback("PALEmuDepth", callback_canvas_modified, NULL);
  resources_register_callback("PALEmuDouble", callback_canvas_modified, NULL);
  resources_register_callback("UseBPlot", callback_canvas_modified, NULL);
  resources_register_callback("VDCDoubleSize", callback_canvas_modified, NULL);
  resources_register_callback("VDCDoubleScan", callback_canvas_modified, NULL);
  resources_register_callback("ScreenSetPalette", callback_canvas_modified, NULL);
}

void fullscreen_capability(cap_fullscreen_t *cap_fullscreen)
{
    cap_fullscreen->device_num = 0;
}

