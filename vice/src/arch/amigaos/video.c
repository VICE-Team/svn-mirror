/*
 * video.c
 *
 * Written by
 *  Mathias Roslund <vice.emu@amidog.se>
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
#include <stdlib.h>
#include <string.h>

#include "videoarch.h"
#include "palette.h"
#include "video.h"
#include "viewport.h"
#include "kbd.h"
#include "keyboard.h"
#include "lib.h"
#include "fullscreenarch.h"
#include "pointer.h"

#define __USE_INLINE__

#undef BYTE
#undef WORD
#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#ifdef AMIGA_M68K
#define _INLINE_MUIMASTER_H
#endif
#include <proto/muimaster.h>
#ifdef AMIGA_M68K
#include <libraries/mui.h>
#endif
#include <proto/graphics.h>

#ifdef HAVE_PROTO_CYBERGRAPHICS_H
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#if !defined(AMIGA_MORPHOS) && !defined(AMIGA_AROS)
#include <inline/cybergraphics.h>
#endif
#else
#ifdef HAVE_PROTO_PICASSO96_H
#include <proto/Picasso96.h>
#else
#include <proto/Picasso96API.h>
#endif
#endif

#include "private.h"
#include "statusbar.h"
#include "mui/mui.h"

#ifdef AMIGA_OS4
struct Library *GadToolsBase = NULL;
struct GadToolsIFace *IGadTools = NULL;
struct MUIMasterIFace *IMUIMaster = NULL;
#endif
struct Library *MUIMasterBase = NULL;

video_canvas_t *canvaslist = NULL;

int video_init_cmdline_options(void)
{
  return 0;
}
#ifdef AMIGA_OS4
struct Library *SocketBase  = NULL;
struct SocketIFace *ISocket = NULL;
#else
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
struct Library *CyberGfxBase = NULL;
#else
struct Library *P96Base = NULL;
#endif
#endif

int video_init(void)
{
#ifndef AMIGA_OS4
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
  if ((CyberGfxBase=OpenLibrary(CYBERGFXNAME,41))) {
#else
  if ((P96Base=OpenLibrary("Picasso96API.library",2))) {
#endif
#ifdef AMIGA_AROS
    if ((MUIMasterBase = OpenLibrary("muimaster.library", 11))) {
#else
    if ((MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN))) {
#endif
      if (mui_init() == 0) {
        return 0;
      }
    }
  }
#else
  if ((SocketBase = OpenLibrary("bsdsocket.library", 4))) {
    if ((ISocket = (struct SocketIFace *)GetInterface(SocketBase, "main", 1, NULL))) {

      if ((GadToolsBase = OpenLibrary("gadtools.library", 39))) {
        if ((IGadTools = (struct GadToolsIFace *)GetInterface(GadToolsBase, "main", 1, NULL))) {
          if ((MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN))) {
            if ((IMUIMaster = (struct MUIMasterIFace *)GetInterface(MUIMasterBase, "main", 1, NULL))) {
              if (mui_init() == 0) {
                return 0;
              }
            }
          }
        }
      }
    }
  }
#endif

  return -1;
}

void video_shutdown(void)
{
  mui_exit();
#ifndef AMIGA_OS4
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
  if (CyberGfxBase) {
    CloseLibrary(CyberGfxBase);
  }
#else
  if (P96Base) {
    CloseLibrary(P96Base);
  }
#endif
  if (MUIMasterBase) {
    CloseLibrary(MUIMasterBase);
  }
#else
  if (IMUIMaster) {
    DropInterface((struct Interface *)IMUIMaster);
  }
  if (MUIMasterBase) {
    CloseLibrary(MUIMasterBase);
  }
  if (IGadTools) {
    DropInterface((struct Interface *)IGadTools);
  }
  if (GadToolsBase) {
    CloseLibrary(GadToolsBase);
  }

  if (ISocket) {
    DropInterface((struct Interface *)ISocket);
  }
  if (SocketBase) {
    CloseLibrary(SocketBase);
  }
#endif
}

static ULONG lock;
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
static ULONG cgx_base_addy;
#else
static struct RenderInfo ri;
#endif

static struct video_canvas_s *reopen(struct video_canvas_s *canvas, int width, int height, int fullscreen)
{
  static int current_fullscreen = 0; /* remember previous state */
  int amiga_width, amiga_height;
  unsigned long dispid;

  /* -1 means "no change" */
  if (fullscreen == -1) {
    fullscreen = current_fullscreen;
  }

  if (canvas == NULL) {
    return NULL;
  }

  /* if changing to/from fullscreen, close screen and window */
  if ((current_fullscreen != fullscreen) || (fullscreen == 1)) {
    pointer_show();
    ui_menu_destroy(canvas);
    statusbar_destroy(canvas);
    if (canvas->os->window != NULL) {
      CloseWindow(canvas->os->window);
      canvas->os->window = NULL;
    }
    if (canvas->os->screen != NULL) {
      CloseScreen(canvas->os->screen);
      canvas->os->screen = NULL;
    }
  }

  /* free bitmap */
  if (canvas->os->window_bitmap != NULL) {
    FreeBitMap(canvas->os->window_bitmap);
    canvas->os->window_bitmap = NULL;
  }

  /* try to get screenmode to use */
  if (fullscreen) {
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
    dispid = BestCModeIDTags(CYBRBIDTG_Depth, 16,
                             CYBRBIDTG_NominalWidth, width,
                             CYBRBIDTG_NominalHeight, (height+ statusbar_get_status_height()),
                             TAG_DONE);
#else

    unsigned long cmodels = RGBFF_R5G5B5 | RGBFF_R5G6B5 | RGBFF_R5G5B5PC | RGBFF_R5G6B5PC;
    dispid = p96BestModeIDTags(P96BIDTAG_NominalWidth, width,
/* FIXME: only ask for statusbar height if it should be shown */
                               P96BIDTAG_NominalHeight, (height + statusbar_get_status_height()),
                               P96BIDTAG_FormatsAllowed, cmodels,
                               TAG_DONE);
#endif

    if (dispid == INVALID_ID) {
      fullscreen = 0;
    }
  }

  /* if fullscreen, open the screen */
  if (fullscreen) {
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
    amiga_width = GetCyberIDAttr(CYBRIDATTR_WIDTH, dispid);
    amiga_height = GetCyberIDAttr(CYBRIDATTR_HEIGHT, dispid);
#else
    amiga_width = p96GetModeIDAttr(dispid, P96IDA_WIDTH);
    amiga_height = p96GetModeIDAttr(dispid, P96IDA_HEIGHT);
#endif

    /* open screen */
    canvas->os->screen = OpenScreenTags(NULL,
             SA_Width, amiga_width,
             SA_Height, amiga_height,
             SA_Depth, 8,
             SA_Quiet, TRUE,
             SA_ShowTitle, FALSE,
             SA_Type, CUSTOMSCREEN,
             SA_DisplayID, dispid,
             SA_Pens, -1,
             SA_FullPalette, TRUE,
             TAG_DONE);

    /* could the screen be opened? */
    if (canvas->os->screen == NULL) {
      return NULL;
    }

    /* open window */
    canvas->os->window = OpenWindowTags(NULL,
             WA_CustomScreen, (ULONG)canvas->os->screen,
             WA_Width, canvas->os->screen->Width,
             WA_Height, canvas->os->screen->Height,
             WA_IDCMP, IDCMP_RAWKEY|IDCMP_CHANGEWINDOW|IDCMP_MENUPICK|IDCMP_MENUVERIFY,
             WA_Backdrop, TRUE,
             WA_Borderless, TRUE,
             WA_Activate, TRUE,
             WA_NewLookMenus, TRUE,
             TAG_DONE);

#ifdef HAVE_PROTO_CYBERGRAPHICS_H
    FillPixelArray(&canvas->os->screen->RastPort, 0, 0,
                   canvas->os->screen->Width, canvas->os->screen->Height, 0);
#else
    p96RectFill(&canvas->os->screen->RastPort, 0, 0,
                canvas->os->screen->Width, canvas->os->screen->Height, 0);
#endif

    pointer_set_default(POINTER_HIDE);
    pointer_hide();

    canvas->os->visible_width = canvas->os->screen->Width;
/* FIXME: only ask for statusbar height if it should be shown */
    canvas->os->visible_height = canvas->os->screen->Height - statusbar_get_status_height();
  } else {
    /* if window already is open, just resize it, otherwise, open it */
    if (canvas->os->window != NULL) {
      ChangeWindowBox(canvas->os->window,
            canvas->os->window->LeftEdge,
            canvas->os->window->TopEdge,
            canvas->os->window->BorderLeft+width+canvas->os->window->BorderRight,
            canvas->os->window->BorderTop+height+statusbar_get_status_height()+
                                canvas->os->window->BorderBottom);
      canvas->os->waiting_for_resize = 1;
    } else {
      canvas->os->window = OpenWindowTags(NULL,
               WA_Title, (ULONG)canvas->os->window_name,
               WA_Flags, WFLG_NOCAREREFRESH|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_GIMMEZEROZERO,
               WA_IDCMP, IDCMP_CLOSEWINDOW|IDCMP_RAWKEY|IDCMP_CHANGEWINDOW|IDCMP_MENUPICK|IDCMP_MENUVERIFY,
               WA_Left, 100,
               WA_Top, 100,
               WA_InnerWidth, width,
               WA_InnerHeight, height + statusbar_get_status_height(),
               WA_Activate, TRUE,
               WA_NewLookMenus, TRUE,
               TAG_DONE);
    }

    pointer_set_default(POINTER_SHOW);
    pointer_show();

    canvas->os->visible_width = width;
    canvas->os->visible_height = height;
  }

  if (canvas->os->window == NULL) {
    return NULL;
  }

  statusbar_create(canvas);

  canvas->os->window_bitmap = AllocBitMap(width, height, 8,
                                          BMF_CLEAR|BMF_DISPLAYABLE|BMF_INTERLEAVED|BMF_MINPLANES,
                                          canvas->os->window->RPort->BitMap);

#ifdef HAVE_PROTO_CYBERGRAPHICS_H
  canvas->os->pixfmt = GetCyberMapAttr(canvas->os->window_bitmap, CYBRMATTR_PIXFMT);
  canvas->os->bpr = GetCyberMapAttr(canvas->os->window_bitmap, CYBRMATTR_XMOD);
  canvas->os->bpp = GetCyberMapAttr(canvas->os->window_bitmap, CYBRMATTR_DEPTH)/8;
#else
  canvas->os->pixfmt = p96GetBitMapAttr(canvas->os->window_bitmap, P96BMA_RGBFORMAT);
  canvas->os->bpr = p96GetBitMapAttr(canvas->os->window_bitmap, P96BMA_BYTESPERROW);
  canvas->os->bpp = p96GetBitMapAttr(canvas->os->window_bitmap, P96BMA_BYTESPERPIXEL);
#endif

  canvas->width = width;
  canvas->height = height;
  canvas->depth = (canvas->os->bpp * 8);
  canvas->bytes_per_line = canvas->os->bpr;
  canvas->use_triple_buffering = 0;

  video_canvas_set_palette(canvas, canvas->palette);

  /* refresh */
  video_canvas_refresh_all(canvas);

  /* remember previous state */
  current_fullscreen = fullscreen;

  return canvas;
}

struct video_canvas_s *video_canvas_create(struct video_canvas_s *canvas,
                                 unsigned int *width, unsigned int *height,
                                 int mapped)
{
  int i;

  canvas->next = NULL;
  canvas->os = lib_malloc(sizeof(struct os_s));
  if (canvas->os == NULL) {
    return NULL;
  }
  memset(canvas->os, 0, sizeof(struct os_s));
  for (i=0; i<16; i++) {
    canvas->os->pens[i] = -1;
  }

  canvas->os->window_name = lib_stralloc(canvas->viewport->title);

  reopen(canvas, *width, *height, -1);

  if (canvaslist == NULL) {
    canvaslist = canvas;
  } else {
    video_canvas_t *node = canvaslist;
    while (node->next != NULL) { node = node->next; }
    node->next = canvas;
  }

  return canvas;
}

void video_arch_canvas_init(struct video_canvas_s *canvas)
{
  canvas->os = NULL;
  canvas->video_draw_buffer_callback = NULL;
}

void video_canvas_refresh(struct video_canvas_s *canvas,
                                 unsigned int xs, unsigned int ys,
                                 unsigned int xi, unsigned int yi,
                                 unsigned int w, unsigned int h)
{
  int dx, dy, sx, sy;

  if (canvas->videoconfig->doublesizex) {
    xi *= 2;
    w *= 2;
  }

  if (canvas->videoconfig->doublesizey) {
    yi *= 2;
    h *= 2;
  }

#ifdef HAVE_PROTO_CYBERGRAPHICS_H
  if ((lock = LockBitMapTags(canvas->os->window_bitmap,
                             LBMI_BASEADDRESS, (ULONG)&cgx_base_addy,
                             TAG_DONE))) {
#else
  if ((lock = p96LockBitMap(canvas->os->window_bitmap, (UBYTE *)&ri, sizeof(ri)))) {
#endif
    video_canvas_render(canvas,
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
                        (UBYTE *)cgx_base_addy,
#else
                        (UBYTE *)ri.Memory,
#endif
                        w, h,
                        xs, ys,
                        xi, yi,
                        canvas->bytes_per_line,
                        canvas->depth);
#ifdef HAVE_PROTO_CYBERGRAPHICS_H
    UnLockBitMap(lock);
#else
    p96UnlockBitMap(canvas->os->window_bitmap, lock);
#endif
  }

  sx = xi;
  sy = yi;
  dx = xi + ((canvas->os->visible_width - (int)canvas->width) / 2);
  dy = yi + ((canvas->os->visible_height - (int)canvas->height) / 2);
  if (dx < 0) {
    sx += -dx;
    w += dx;
    dx = 0;
  }
  if (dy < 0) {
    sy += -dy;
    h += dy;
    dy = 0;
  }
  if (w > canvas->os->visible_width) {
    w = canvas->os->visible_width;
  }
  if (h > canvas->os->visible_height) {
    h = canvas->os->visible_height;
  }

  if ((w > 0) && (h > 0)) {
    BltBitMapRastPort(canvas->os->window_bitmap, sx, sy, canvas->os->window->RPort, dx, dy, w, h, 0xc0);
  }
}

/* dummy */

int makecol_dummy(int r, int g, int b)
{
  return 0;
}

/* 16bit - BE */

int makecol_RGB565BE(int r, int g, int b)
{
  int c = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
  return c;
}
int makecol_BGR565BE(int r, int g, int b)
{
  int c = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | ((r & 0xf8) >> 3);
  return c;
}
int makecol_RGB555BE(int r, int g, int b)
{
  int c = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
  return c;
}
int makecol_BGR555BE(int r, int g, int b)
{
  int c = ((b & 0xf8) << 7) | ((g & 0xf8) << 2) | ((r & 0xf8) >> 3);
  return c;
}

/* 16bit - LE */

int makecol_RGB565LE(int r, int g, int b)
{
  int c = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
  c = ((c << 8) & 0xff00) | ((c >> 8) & 0x00ff);
  return c;
}
int makecol_BGR565LE(int r, int g, int b)
{
  int c = ((b & 0xf8) << 8) | ((g & 0xfc) << 3) | ((r & 0xf8) >> 3);
  c = ((c << 8) & 0xff00) | ((c >> 8) & 0x00ff);
  return c;
}
int makecol_RGB555LE(int r, int g, int b)
{
  int c = ((r & 0xf8) << 7) | ((g & 0xf8) << 2) | ((b & 0xf8) >> 3);
  c = ((c << 8) & 0xff00) | ((c >> 8) & 0x00ff);
  return c;
}
int makecol_BGR555LE(int r, int g, int b)
{
  int c = ((b & 0xf8) << 7) | ((g & 0xf8) << 2) | ((r & 0xf8) >> 3);
  c = ((c << 8) & 0xff00) | ((c >> 8) & 0x00ff);
  return c;
}

/* 24bit (swapped these as VICE read from LSB, *NOT* MSB when rendering) */

int makecol_RGB24(int r, int g, int b)
{
//  int c = (r << 16) | (g << 8) | b;
  int c = (b << 16) | (g << 8) | r;
  return c;
}
int makecol_BGR24(int r, int g, int b)
{
//  int c = (b << 16) | (g << 8) | r;
  int c = (r << 16) | (g << 8) | b;
  return c;
}

/* 32bit */

int makecol_ARGB32(int r, int g, int b)
{
  int c = (r << 16) | (g << 8) | b;
  return c;
}
int makecol_ABGR32(int r, int g, int b)
{
  int c = (b << 16) | (g << 8) | r;
  return c;
}
int makecol_RGBA32(int r, int g, int b)
{
  int c = (r << 24) | (g << 16) | (b << 8);
  return c;
}
int makecol_BGRA32(int r, int g, int b)
{
  int c = (b << 24) | (g << 16) | (r << 8);
  return c;
}

#ifdef HAVE_PROTO_CYBERGRAPHICS_H
struct {
  unsigned long color_format;
  int (*makecol)(int r, int g, int b);
} color_formats[] = {
  { PIXFMT_RGB16, makecol_RGB565BE },
  { PIXFMT_BGR16, makecol_BGR565BE },
  { PIXFMT_RGB16PC, makecol_RGB565LE },
  { PIXFMT_BGR16PC, makecol_BGR565LE },
  { PIXFMT_RGB24, makecol_RGB24 },
  { PIXFMT_BGR24, makecol_BGR24 },
  { PIXFMT_ARGB32, makecol_ARGB32 },
  { PIXFMT_BGRA32, makecol_BGRA32 },
  { PIXFMT_RGBA32, makecol_RGBA32 },
  { 0, NULL }
};
#else
struct {
  unsigned long color_format;
  int (*makecol)(int r, int g, int b);
} color_formats[] = {
  { RGBFB_R8G8B8, makecol_RGB24 }, /* TrueColor RGB (8 bit each) */
  { RGBFB_B8G8R8, makecol_BGR24 }, /* TrueColor BGR (8 bit each) */
  { RGBFB_R5G6B5PC, makecol_RGB565LE }, /* HiColor16 (5 bit R, 6 bit G, 5 bit B), format: gggbbbbbrrrrrggg */
  { RGBFB_R5G5B5PC, makecol_RGB555LE }, /* HiColor15 (5 bit each), format: gggbbbbb0rrrrrgg */
  { RGBFB_A8R8G8B8, makecol_ARGB32 }, /* 4 Byte TrueColor ARGB (A unused alpha channel) */
  { RGBFB_A8B8G8R8, makecol_ABGR32 }, /* 4 Byte TrueColor ABGR (A unused alpha channel) */
  { RGBFB_R8G8B8A8, makecol_RGBA32 }, /* 4 Byte TrueColor RGBA (A unused alpha channel) */
  { RGBFB_B8G8R8A8, makecol_BGRA32 }, /* 4 Byte TrueColor BGRA (A unused alpha channel) */
  { RGBFB_R5G6B5, makecol_RGB565BE }, /* HiColor16 (5 bit R, 6 bit G, 5 bit B), format: rrrrrggggggbbbbb */
  { RGBFB_R5G5B5, makecol_RGB555BE }, /* HiColor15 (5 bit each), format: 0rrrrrgggggbbbbb */
  { RGBFB_B5G6R5PC, makecol_BGR565LE }, /* HiColor16 (5 bit R, 6 bit G, 5 bit B), format: gggrrrrrbbbbbggg */
  { RGBFB_B5G5R5PC, makecol_BGR555LE }, /* HiColor15 (5 bit each), format: gggrrrrr0bbbbbbgg */
  /* END */
  { 0, NULL },
};
#endif

int video_canvas_set_palette(struct video_canvas_s *canvas,
                                    struct palette_s *palette)
{
  int (*makecol)(int r, int g, int b);
  int i;
  int col;

  canvas->palette = palette;

  i = 0;
  makecol = makecol_dummy;
  while (color_formats[i].makecol != NULL) {
    if (color_formats[i].color_format == canvas->os->pixfmt) {
      makecol = color_formats[i].makecol;
      break;
    }
    i++;
  }

  for (i = 0; i < canvas->palette->num_entries; i++) {
    if (canvas->depth == 8) {
      col = 0;
    } else {
      col = makecol(canvas->palette->entries[i].red,
                    canvas->palette->entries[i].green,
                    canvas->palette->entries[i].blue);
    }

    video_render_setphysicalcolor(canvas->videoconfig, i, col, canvas->depth);
  }

  if (canvas->depth > 8) {
    for (i = 0; i < 256; i++) {
      video_render_setrawrgb(i, makecol(i, 0, 0), makecol(0, i, 0), makecol(0, 0, i));
    }
    video_render_initraw();
  }

  return 0;
}

void video_canvas_destroy(struct video_canvas_s *canvas)
{
  if ((canvas != NULL) && (canvas->os != NULL)) {
    ui_menu_destroy(canvas);
    statusbar_destroy(canvas);
    lib_free(canvas->os->window_name);

    if (canvas->os->window != NULL) {
      CloseWindow(canvas->os->window);
      canvas->os->window = NULL;
    }
    if (canvas->os->screen != NULL) {
      CloseScreen(canvas->os->screen);
      canvas->os->screen = NULL;
    }
    if (canvas->os->window_bitmap != NULL) {
      FreeBitMap(canvas->os->window_bitmap);
      canvas->os->window_bitmap = NULL;
    }

    if (canvaslist == canvas) {
      canvaslist = canvas->next;
    } else {
      video_canvas_t *node = canvaslist;
      while (node->next != canvas) { node = node->next; }
      node->next = canvas->next;
    }
  }
}

void video_canvas_resize(struct video_canvas_s *canvas,
                                unsigned int width, unsigned int height)
{
  if (canvas->videoconfig->doublesizex)
    width *= 2;

  if (canvas->videoconfig->doublesizey)
    height *= 2;

  reopen(canvas, width, height, -1);
}

int video_arch_resources_init(void)
{
  return 0;
}

void video_arch_resources_shutdown(void)
{
}

void video_arch_fullscreen_toggle(int fullscreen)
{
  if (canvaslist != NULL) {
    reopen(canvaslist, canvaslist->width, canvaslist->height, fullscreen);
  }
}
