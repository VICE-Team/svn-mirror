/*
 * vsidui.c - UI for sidplayer
 *
 * Written by
 *  Andreas Dehmel (dehmel@forwiss.tu-muenchen)
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

#include "wimp.h"

#include "log.h"
#include "resources.h"
#include "ui.h"
#include "vsidui.h"

#include "drive/drive.h"
#include "c64/psid.h"


#define Icon_VSid_Name		1
#define Icon_VSid_Author	2
#define Icon_VSid_Copyright	3
#define Icon_VSid_TotalTunes	7
#define Icon_VSid_Sync		8
#define Icon_VSid_Tune		11
#define Icon_VSid_NextTune	12
#define Icon_VSid_PrevTune	13
#define Icon_VSid_StopTune	14
#define Icon_VSid_Default	15



static int NumTunes = 0;

static const char *Rsrc_Tune = "PSIDTune";


int vsid_ui_init(void)
{
  int block[WindowB_WFlags+1];

  resources_load(NULL);

  ui_open_vsid_window(block);

  return 0;
}

void vsid_set_tune(int tune)
{
  resources_set_value(Rsrc_Tune, (resource_value_t)tune);
}


int vsid_ui_load_file(const char *file)
{
    if (psid_load_file(file) == 0)
    {
      psid_init_tune();
      vsid_set_tune(wimp_window_read_icon_number(VSidWindow, Icon_VSid_Default));
    }
    return -1;
}

int vsid_ui_mouse_click(int *block)
{
  int number = wimp_window_read_icon_number(VSidWindow, Icon_VSid_Tune);

  if (block[MouseB_Icon] == Icon_VSid_NextTune)
  {
    if (++number <= NumTunes)
      vsid_set_tune(number);
    return 0;
  }
  else if (block[MouseB_Icon] == Icon_VSid_PrevTune)
  {
    if (--number > 0)
      vsid_set_tune(number);
    return 0;
  }
  else if (block[MouseB_Icon] == Icon_VSid_StopTune)
  {
    vsid_set_tune(-1);
    return 0;
  }
  return -1;
}

int vsid_ui_key_press(int *block)
{
  if (block[KeyPB_Icon] == Icon_VSid_Tune)
  {
    int number;

    number = wimp_window_read_icon_number(VSidWindow, Icon_VSid_Tune);

    if ((number > 0) && (number <= NumTunes))
    {
      vsid_set_tune(number);
    }
    return 0;
  }
  return -1;
}

void vsid_ui_display_name(const char *name)
{
    log_message(LOG_DEFAULT, "Name: %s", name);

    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Name, name);
}

void vsid_ui_display_author(const char *author)
{
    log_message(LOG_DEFAULT, "Author: %s", author);

    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Author, author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    log_message(LOG_DEFAULT, "Copyright by: %s", copyright);

    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Copyright, copyright);
}

void vsid_ui_display_sync(int sync)
{
    log_message(LOG_DEFAULT, "Using %s sync", sync==DRIVE_SYNC_PAL?"PAL":"NTSC");

    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Sync, ((sync == DRIVE_SYNC_PAL) ? "PAL" : "NTSC"));
}

void vsid_ui_set_default_tune(int nr)
{
    log_message(LOG_DEFAULT, "Default Tune: %i", nr);

    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Default, nr);
}

void vsid_ui_display_tune_nr(int nr)
{
    log_message(LOG_DEFAULT, "Playing Tune: %i", nr);

    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Tune, nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
    log_message(LOG_DEFAULT, "Number of Tunes: %i", count);

    wimp_window_write_icon_number(VSidWindow, Icon_VSid_TotalTunes, count);

    NumTunes = count;
}
