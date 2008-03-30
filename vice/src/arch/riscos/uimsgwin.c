/*
 * uimsgwin.c - a window displaying (long) textual messages
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "wimp.h"

#include "uimsgwin.h"
#include "ui.h"




static const char *WindowMessage;

static int NumberOfLines = 0;
static char **MessageLines = NULL;

static int FontHandle = -1;

static int LineHeight;
static int LineOffset;
static int WindowWidth;
static int OStoMpts;

static const int FontSizeX = 12;
static const int FontSizeY = 12;
static const int WindowBorder = 16;
static const int MaxWindowHeight = 768;

static const int BackgroundColour = 0xeeeeee00;
static const int ForegroundColour = 0x00000000;

static const char FontName[] = "corpus.medium";



static void ui_message_free_resources(void)
{
  if (MessageLines != NULL)
  {
    free(MessageLines); MessageLines = NULL;
  }
}


int ui_message_window_open(const char *title, const char *message)
{
  char strdummy[] = "0123456789ABCDEF";
  char *str;
  const char *b, *base;
  int cblock[9];
  int i, offx, offy, longestLine;
  RO_Window *win = MessageWindow;

  ui_message_free_resources();

  WindowMessage = message;

  if (FontHandle < 0)
  {
    int resx, resy;

    resx = 0; resy = 0;
    FontHandle = Font_FindFont(FontName, FontSizeX << 4, FontSizeY << 4, &resx, &resy);
  }

  b = WindowMessage; NumberOfLines = 0;
  while (*b != '\0')
  {
    NumberOfLines++;
    while ((*b != '\n') && (*b != '\0')) b++;
    if (*b == '\n') b++;
  }
  if (NumberOfLines == 0) return -1;

  if ((MessageLines = (char**)malloc((NumberOfLines+1) * sizeof(char*))) == NULL) return -1;
  memset(MessageLines, 0, (NumberOfLines+1) * sizeof(char*));
  b = WindowMessage; i = 0; longestLine = 0;
  while (*b != '\0')
  {
    base = b;
    MessageLines[i++] = (char*)b;
    while ((*b != '\n') && (*b != '\0')) b++;
    if (longestLine < (b-base)) longestLine = (b - base);
    if (*b == '\n') b++;
  }
  MessageLines[NumberOfLines] = (char*)b;
  if (longestLine == 0) return -1;

  memset(cblock, 0, 16); cblock[4] = -1;
  offx = INT_MAX; offy = INT_MAX; str = strdummy;
  Font_ScanString(FontHandle, &str, 0x320 | (1<<18), &offx, &offy, cblock, NULL, &i);
  offx = cblock[7] - cblock[5]; LineHeight = cblock[8] - cblock[6];
  WindowWidth = ((longestLine+1) * offx) / strlen(strdummy);

  Font_ConverttoOS(&WindowWidth, &LineHeight);
  WindowWidth += 2*WindowBorder;
  LineOffset = LineHeight >> 1; LineHeight = (3*LineHeight) >> 1;
  OStoMpts = 1; offy = 0;
  Font_Converttopoints(&OStoMpts, &offy);

  offy = 2*WindowBorder + NumberOfLines * LineHeight;
  wimp_window_set_extent(win, 0, -offy, WindowWidth, 0);
  wimp_window_write_title(win, title);

  if (wimp_window_open_status(win) == 0)
  {
    offx = win->wmaxx - win->wminx;
    offy = win->wmaxy - win->wminy;
    if (offy > MaxWindowHeight) offy = MaxWindowHeight;
    win->vminx = (ScreenMode.resx - offx) / 2; win->vmaxx = win->vminx + offx;
    win->vminy = (ScreenMode.resy - offy) / 2; win->vmaxy = win->vminy + offy;
    win->scrollx = 0; win->scrolly = 0;
  }
  else
  {
    Wimp_GetWindowState((int*)win);
    Wimp_ForceRedraw(win->Handle, win->wminx, win->wminy, win->wmaxx, win->wmaxy);
  }
  win->stackpos = -1;

  Wimp_OpenWindow((int*)win);

  return 0;
}


void ui_message_window_close(void)
{
  ui_message_free_resources();
  Wimp_CloseWindow((int*)MessageWindow);
}


void ui_message_window_redraw(int *block)
{
  int more, lineadd;

  if ((more = Wimp_RedrawWindow(block)) != 0)
  {
    ColourTrans_SetFontColours(FontHandle, BackgroundColour, ForegroundColour, 14);
    lineadd = LineHeight * OStoMpts;
  }
  while (more != 0)
  {
    ColourTrans_SetGCOL(BackgroundColour, 0x100, 0);
    OS_Plot(0x04, block[RedrawB_VMinX], block[RedrawB_VMinY]);
    OS_Plot(0x65, block[RedrawB_VMaxX], block[RedrawB_VMaxY]);

    if ((MessageLines != NULL) || (LineHeight == 0))
    {
      int lineoff, bottom;
      int posx, posy, osx, osy;

      lineoff = (block[RedrawB_VMaxY] - block[RedrawB_ScrollY] - block[RedrawB_CMaxY] - WindowBorder);
      if (lineoff < 0) lineoff = 0;
      lineoff /= LineHeight;

      osx = (block[RedrawB_VMinX] - block[RedrawB_ScrollX] + WindowBorder);
      osy = (block[RedrawB_VMaxY] - block[RedrawB_ScrollY] - WindowBorder - (lineoff + 1) * LineHeight);
      posx = osx * OStoMpts; posy = osy * OStoMpts;
      bottom = OStoMpts * (block[RedrawB_CMinY] - LineHeight);
      osy -= LineOffset;

      while (posy > bottom)
      {
        int len;

        if ((lineoff < NumberOfLines) && (MessageLines[lineoff] != NULL))
        {
          len = MessageLines[lineoff+1] - MessageLines[lineoff];
          Font_Paint(FontHandle, MessageLines[lineoff], 0x380, posx, posy, NULL, NULL, len);
        }
        posy -= lineadd; lineoff++;
      }
    }
    more = Wimp_GetRectangle(block);
  }
}


void ui_message_window_exit(void)
{
  if (FontHandle >= 0)
  {
    Font_LoseFont(FontHandle); FontHandle = -1;
  }
  ui_message_window_close();
}
