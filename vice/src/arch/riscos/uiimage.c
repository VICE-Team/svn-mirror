/*
 * uiimage.c - image contents handling on RISC OS
 *
 * Written by
 *  Andreas Dehmel <zarquon@t-online.de>
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
#include <string.h>
#include <limits.h>

#include "wimp.h"
#include "posix.h"

#include "archdep.h"
#include "attach.h"
#include "autostart.h"
#include "imagecontents.h"
#include "lib.h"
#include "ui.h"
#include "uiimage.h"
#include "vsync.h"



#define IMAGE_CONTENT_NONE	0
#define IMAGE_CONTENT_TAPE	1
#define IMAGE_CONTENT_DISK	2
#define IMAGE_CONTENT_DIR	3

static image_contents_t *contents;
static int image_content_type = IMAGE_CONTENT_NONE;
static char image_content_file[256];

static int FontHandle = -1;
static int FontHandleEm = -1;

static int LineHeight;
static int LineOffset;
static int NumberOfLines;
static int PosCol2, PosCol3;
static int WindowWidth;
static int MarkedLine;
static int OStoMpts;

static const int FontSizeX = 12;
static const int FontSizeY = 12;
static const int WindowBorder = 16;
static const int MaxWindowHeight = 512;

static const int BackgroundColour = 0xeeeeee00;
static const int ForegroundColour = 0x00000000;

static const char FontName[] = "corpus.medium";
static const char FontNameEm[] = "corpus.bold";



static image_contents_t *ui_image_contents_read_dir(const char *dir_name)
{
  image_contents_t *new;
  DIR *dirp;
  struct dirent *de;
  image_contents_file_list_t *lp;
  unsigned int maxlen;

  dirp = opendir(dir_name);
  if (dirp == NULL)
    return NULL;

  new = image_contents_new();
  memcpy(new->name, dir_name, IMAGE_CONTENTS_NAME_LEN);
  (new->name)[IMAGE_CONTENTS_NAME_LEN] = 0;

  memcpy(new->id, dir_name, IMAGE_CONTENTS_ID_LEN);
  (new->id)[IMAGE_CONTENTS_ID_LEN] = 0;

  new->blocks_free = -1;

  new->file_list = NULL;
  lp = NULL;

  maxlen = (MAX_FILENAME_LENGTH > IMAGE_CONTENTS_FILE_NAME_LEN) ?
             IMAGE_CONTENTS_FILE_NAME_LEN : MAX_FILENAME_LENGTH;

  while ((de = readdir(dirp)) != NULL)
  {
    image_contents_file_list_t *new_list;

    new_list = (image_contents_file_list_t*)lib_malloc(sizeof(image_contents_file_list_t));
    new_list->size = (de->d_reclen) / 254;
    strncpy(new_list->name, de->d_name, maxlen);
    (new_list->name)[maxlen] = '\0';
    strcpy(new_list->type, "PRG");

    new_list->next = NULL;

    if (lp == NULL)
    {
      new_list->prev = NULL;
      new->file_list = new_list;
    }
    else
    {
      new_list->prev = lp;
      lp->next = new_list;
    }
    lp = new_list;
  }
  closedir(dirp);

  return new;
}


static int setup_window(const char *title)
{
  char dummy[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  char *str;
  int offx, offy, len;
  int cblock[9];
  image_contents_file_list_t *file;
  RO_Window *win;

  if (contents == NULL)
  {
    Wimp_CloseWindow((int*)ImgContWindow); return -1;
  }

  if (FontHandle < 0)
  {
    int resx, resy;

    resx = 0; resy = 0;
    FontHandle = Font_FindFont(FontName, FontSizeX << 4, FontSizeY << 4, &resx, &resy);
  }
  if (FontHandleEm < 0)
  {
    int resx, resy;

    resx = 0; resy = 0;
    FontHandleEm = Font_FindFont(FontNameEm, FontSizeX << 4, FontSizeY << 4, &resx, &resy);
  }

  /* Scan size */
  str = dummy; offx = INT_MAX; offy = INT_MAX;
  memset(cblock, 0, 16); cblock[4] = -1;
  Font_ScanString(FontHandle, &str, 0x320 | (1<<18), &offx, &offy, cblock, NULL, &len);
  offx = cblock[7] - cblock[5]; offy = cblock[8] - cblock[6];
  len = strlen(dummy);
  PosCol2 = (offx * 6) / len;
  PosCol3 = PosCol2 + (offx * (4 + IMAGE_CONTENTS_FILE_NAME_LEN)) / len;
  offx = PosCol3 + (offx * (2 + IMAGE_CONTENTS_TYPE_LEN)) / len;
  Font_ConverttoOS(&offx, &offy);
  WindowWidth = offx;
  LineOffset = offy >> 1; LineHeight = (3*offy) >> 1;
  offx = 1;
  Font_Converttopoints(&offx, &offy);
  OStoMpts = offx;

  NumberOfLines = 0; file = contents->file_list;
  while (file != NULL)
  {
    NumberOfLines++; file = file->next;
  }

  win = ImgContWindow;
  wimp_window_write_title(win, "");
  offx = WindowWidth + 2*WindowBorder;
  offy = (NumberOfLines + 2) * LineHeight + 2*WindowBorder;
  wimp_window_set_extent(win, 0, -offy, offx, 0);
  wimp_window_write_title(win, title);

  if (wimp_window_open_status(win) == 0)
  {
    offx = win->wmaxx - win->wminx + WindowBorder;
    offy = win->wmaxy - win->wminy + WindowBorder;
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

  MarkedLine = -1;

  return 0;
}


void ui_image_contents_close(void)
{
  if (contents != NULL)
  {
    image_contents_destroy(contents); contents = NULL;
    Wimp_CloseWindow((int*)ImgContWindow);
  }
}


static void delete_contents(void)
{
  if (contents != NULL)
  {
    image_contents_destroy(contents); contents = NULL;
  }
}

int ui_image_contents_disk(const char *imagename)
{
  vsync_suspend_speed_eval();
  delete_contents();
  contents = image_contents_read(IMAGE_CONTENTS_DISK, imagename, 0);
  wimp_strcpy(image_content_file, imagename);
  image_content_type = IMAGE_CONTENT_DISK;
  return setup_window(imagename);
}


int ui_image_contents_tape(const char *imagename)
{
  vsync_suspend_speed_eval();
  delete_contents();
  contents = image_contents_read(IMAGE_CONTENTS_TAPE, imagename, 0);
  wimp_strcpy(image_content_file, imagename);
  image_content_type = IMAGE_CONTENT_TAPE;
  return setup_window(imagename);
}


int ui_image_contents_dir(const char *dirname)
{
  vsync_suspend_speed_eval();
  delete_contents();
  contents = ui_image_contents_read_dir(dirname);
  wimp_strcpy(image_content_file, dirname);
  image_content_type = IMAGE_CONTENT_DIR;
  return setup_window(dirname);
}


int ui_image_contents_generic(const char *imagename, int filetype)
{
  if (filetype == FileType_D64File)
    return ui_image_contents_disk(imagename);
  else if (filetype == FileType_Directory)
    return ui_image_contents_dir(imagename);
  else if (filetype == FileType_Data)
  {
    const char *b, *ext;

    b = imagename; ext = b;
    while (*b > ' ')
    {
      if (*b == '/') ext = b+1;
      b++;
    }
    if (ext != imagename)
    {
      if (wimp_strcasecmp(ext, "t64") == 0)
      {
        return ui_image_contents_tape(imagename);
      }
    }
    return ui_image_contents_disk(imagename);
  }
  return -1;
}


static void redraw_line(int handle, int posx, int posy, char *first, char *second, char *third)
{
  Font_Paint(handle, first, 0x300, posx, posy, NULL, NULL, 0);
  Font_Paint(handle, second, 0x300, posx + PosCol2, posy, NULL, NULL, 0);
  if (third != NULL)
  {
    Font_Paint(handle, third, 0x300, posx + PosCol3, posy, NULL, NULL, 0);
  }
}


static void transform_string(const char *str, char *buffer, int hnorm, int hemph)
{
  unsigned char *b, *d;

  b = (unsigned char *)str; d = (unsigned char *)buffer;
  *d++ = '\"';
  while (*b != 0)
  {
    if (*b < ' ')
    {
      *d++ = 26; *d++ = (unsigned char)hemph;
      *d++ = (*b) + 'a' - 1; *d++ = 26; *d++ = (unsigned char)hnorm;
    }
    else
    {
      *d++ = *b;
    }
    b++;
  }
  *d++ = '\"'; *d++ = '\0';
}


static void force_line_redraw(int line)
{
  if ((line >= 0) & (line < NumberOfLines+2))
  {
    int posy;

    posy = WindowBorder + (line + 1) * LineHeight + LineOffset;
    Wimp_ForceRedraw(ImgContWindow->Handle, WindowBorder, -posy, WindowBorder + WindowWidth, LineHeight - posy);
  }
}


void ui_image_contents_redraw(int *block)
{
  int more, lineadd = 0;

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

    if (contents != NULL)
    {
      int lineoff, bottom;
      int posx, posy, line, osx, osy;
      image_contents_file_list_t *file;
      char buffer[256];
      char *name;

      lineoff = (block[RedrawB_VMaxY] - block[RedrawB_ScrollY] - block[RedrawB_CMaxY] - WindowBorder);
      if (lineoff < 0) lineoff = 0;
      lineoff /= LineHeight;

      osx = (block[RedrawB_VMinX] - block[RedrawB_ScrollX] + WindowBorder);
      osy = (block[RedrawB_VMaxY] - block[RedrawB_ScrollY] - WindowBorder - (lineoff + 1) * LineHeight);
      posx = osx * OStoMpts; posy = osy * OStoMpts;

      bottom = OStoMpts * (block[RedrawB_CMinY] - LineHeight);

      ColourTrans_SetGCOL(ForegroundColour, 0x100, 0);
      osy -= LineOffset;

      file = contents->file_list; line = 1;
      if (lineoff == 0)
      {
        name = buffer + 1 + sprintf(buffer, "0");
        OS_Plot(0x04, osx, osy);
        OS_Plot(0x61, WindowWidth, LineHeight);
        ColourTrans_SetFontColours(FontHandleEm, ForegroundColour, BackgroundColour, 14);
        transform_string(contents->name, name, FontHandleEm, FontHandle);
        redraw_line(FontHandleEm, posx, posy, buffer, name, contents->id);
        ColourTrans_SetFontColours(FontHandle, BackgroundColour, ForegroundColour, 14);
        posy -= lineadd; osy -= LineHeight;
      }
      else if (lineoff <= NumberOfLines)
      {
        for (; line < lineoff; line++)
        {
          if (file == NULL) break;
          file = file->next;
        }
      }
      else
      {
        file = NULL;
      }

      while (file != NULL)
      {
        if (posy <= bottom) break;
        name = buffer + 1 + sprintf(buffer, "%d", file->size);
        if (line == MarkedLine)
        {
          OS_Plot(0x04, osx, osy); OS_Plot(0x61, WindowWidth, LineHeight);
          ColourTrans_SetFontColours(FontHandle, ForegroundColour, BackgroundColour, 14);
        }
        transform_string(file->name, name, FontHandle, FontHandleEm);
        redraw_line(FontHandle, posx, posy, buffer, name, file->type);
        if (line == MarkedLine)
        {
          ColourTrans_SetFontColours(FontHandle, BackgroundColour, ForegroundColour, 14);
        }
        posy -= lineadd; osy -= LineHeight; file = file->next; line++;
      }
      if ((file == NULL) && (lineoff < NumberOfLines + 2))
      {
        if (contents->blocks_free == -1)
          name = buffer + 1 + sprintf(buffer, "???");
        else
          name = buffer + 1 + sprintf(buffer, "%d", contents->blocks_free);
        redraw_line(FontHandleEm, posx, posy, buffer, "blocks free", NULL);
      }
    }
    more = Wimp_GetRectangle(block);
  }
}


void ui_image_contents_exit(void)
{
  if (FontHandle >= 0)
  {
    Font_LoseFont(FontHandle); FontHandle = -1;
  }
  if (FontHandleEm >= 0)
  {
    Font_LoseFont(FontHandleEm); FontHandleEm = -1;
  }
  if (contents != NULL)
  {
    image_contents_destroy(contents); contents = NULL;
  }
}


void ui_image_contents_click(int *block)
{
  if ((block[MouseB_Buttons] == 1) || (block[MouseB_Buttons] == 4))
  {
    int winfo[WindowB_WFlags+1];
    int filenum;

    winfo[WindowB_Handle] = ImgContWindow->Handle;
    Wimp_GetWindowState(winfo);
    filenum = winfo[WindowB_VMaxY] - winfo[WindowB_ScrollY] - block[MouseB_PosY];
    filenum = (filenum - WindowBorder) / LineHeight;
    /* no load"*" in dir mode*/
    if ((filenum == 0) && (image_content_type == IMAGE_CONTENT_DIR))
      filenum = -1;
    /* double-click on title autostarts "*" (except for dir mode) */
    if ((filenum >= 0) && (filenum <= NumberOfLines))
    {
      vsync_suspend_speed_eval();
      force_line_redraw(MarkedLine);
      MarkedLine = filenum;
      force_line_redraw(MarkedLine);
      file_system_detach_disk(8);

      switch(image_content_type)
      {
        case IMAGE_CONTENT_DISK:
          autostart_disk(image_content_file, NULL, filenum, AUTOSTART_MODE_RUN);
          break;
        case IMAGE_CONTENT_TAPE:
          autostart_tape(image_content_file, NULL, filenum, AUTOSTART_MODE_RUN);
          break;
        case IMAGE_CONTENT_DIR:
          {
            image_contents_file_list_t *item = contents->file_list;
            int number;

            for (number=1; (number<filenum) && (item!=NULL); number++, item=item->next) ;

            if (item != NULL)
            {
              char buffer[256];

              strcpy(buffer, image_content_file);
              number = strlen(buffer);
              buffer[number++] = FSDEV_DIR_SEP_CHR;
              strcpy(buffer+number, item->name);
              if (autostart_prg(buffer, AUTOSTART_MODE_RUN) == 0)
              {
                ui_display_drive_dir(0, image_content_file);
              }
            }
          }
          break;
        default:
          break;
      }

      /* adjust double-click */
      if (block[MouseB_Buttons] == 1)
        ui_image_contents_close();
    }
  }
}
