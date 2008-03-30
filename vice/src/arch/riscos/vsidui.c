/*
 * vsidui.c - UI for sidplayer
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

#include <stdlib.h>

#include "wimp.h"

#include "log.h"
#include "resources.h"
#include "ui.h"
#include "vsidui.h"
#include "vsidarch.h"
#include "vsync.h"

#include "drive/drive.h"
#include "c64/psid.h"



RO_Window *VSidWindow = NULL;

static int NumTunes = 0;
static int VSidPaused = 0;
static int VSidStopped = 0;
static int VSidReady = 0;
static unsigned int PlayTimeSec = 0;

static const char *Rsrc_Tune = "PSIDTune";


enum SymbolInstances {
  Symbol_Stop,
  Symbol_Play,
  Symbol_Pause,
  Symbol_Resume,
  Symbol_NumSymbols
};

static char *SymbolStrings[] = {
  "\\VSidStop",
  "\\VSidPlay",
  "\\VSidPause",
  "\\VSidResume",
  NULL
};


static void vsid_ui_display_paused(int state)
{
  const char *text;

  ui_display_paused(state);

  if (state == 0)
    text = SymbolStrings[Symbol_Pause];
  else
    text = SymbolStrings[Symbol_Resume];

  EmuPaused = state;
  ui_display_paused(EmuPaused);

  wimp_window_write_icon_text(VSidWindow, Icon_VSid_Pause, text);
}

static void vsid_ui_display_stopped(int state)
{
  const char *text;
  unsigned int iflg;

  if (state == 0)
  {
    iflg = 0;
    text = SymbolStrings[Symbol_Stop];
  }
  else
  {
    iflg = IFlg_Grey;
    text = SymbolStrings[Symbol_Play];
  }

  EmuPaused = state;
  ui_display_paused(EmuPaused);

  wimp_window_set_icon_state(VSidWindow, Icon_VSid_Pause, iflg, IFlg_Grey);

  wimp_window_write_icon_text(VSidWindow, Icon_VSid_StopTune, text);
}

static void vsid_ui_set_ready(int state)
{
  unsigned int iflg;

  if (state == 0)
  {
    iflg = IFlg_Grey;
    VSidStopped = 1;
    VSidPaused = 0;
  }
  else
  {
    iflg = 0;
    VSidStopped = 0;
    VSidPaused = 0;
  }

  wimp_window_set_icon_state(VSidWindow, Icon_VSid_StopTune, iflg, IFlg_Grey);
  wimp_window_set_icon_state(VSidWindow, Icon_VSid_Tune, iflg, IFlg_Grey);
  wimp_window_set_icon_state(VSidWindow, Icon_VSid_NextTune, iflg, IFlg_Grey);
  wimp_window_set_icon_state(VSidWindow, Icon_VSid_PrevTune, iflg, IFlg_Grey);

  vsid_ui_display_paused(VSidPaused);
  vsid_ui_display_stopped(VSidStopped);

  VSidReady = state;
}


int vsid_ui_init(void)
{
  int block[WindowB_WFlags+1];

  resources_load(NULL);

  ui_open_centered_or_raise_block(VSidWindow, block);
  Wimp_OpenWindow(block);

  vsid_ui_set_ready(0);

  return 0;
}

int vsid_ui_message_hook(struct wimp_msg_desc_s *msg)
{
  char *pool;
  int size;

  size = wimp_message_translate_symbols(msg, SymbolStrings, NULL);
  if ((pool = (char*)malloc(size)) != NULL)
  {
    wimp_message_translate_symbols(msg, SymbolStrings, pool);
    return 0;
  }
  return -1;
}

void vsid_set_tune(int tune)
{
  resources_set_value(Rsrc_Tune, (resource_value_t)tune);
  vsid_ui_display_time(0);
}


int vsid_ui_load_file(const char *file)
{
  vsync_suspend_speed_eval();

  if (psid_load_file(file) == 0)
  {
    vsid_ui_set_ready(1);
    psid_init_driver();
    psid_init_tune();
    vsid_set_tune(wimp_window_read_icon_number(VSidWindow, Icon_VSid_Default));
    return 0;
  }
  return -1;
}

int vsid_ui_mouse_click(int *block)
{
  int number;

  if (VSidReady == 0)
    return -1;

  number = wimp_window_read_icon_number(VSidWindow, Icon_VSid_Tune);

  switch (block[MouseB_Icon])
  {
    case Icon_VSid_NextTune:
      if (block[MouseB_Buttons] == 1)
      {
        if (--number > 0)
          vsid_set_tune(number);
      }
      else
      {
        if (++number <= NumTunes)
          vsid_set_tune(number);
      }
      break;;
    case Icon_VSid_PrevTune:
      if (block[MouseB_Buttons] == 1)
      {
        if (++number <= NumTunes)
          vsid_set_tune(number);
      }
      else
      {
        if (--number > 0)
          vsid_set_tune(number);
      }
      break;;
    case Icon_VSid_StopTune:
      VSidStopped ^= 1;
      vsid_ui_display_stopped(VSidStopped);
      vsid_set_tune(number);
      break;
    case Icon_VSid_Pause:
      VSidPaused ^= 1;
      vsid_ui_display_paused(VSidPaused);
      break;
    default:
      return -1;
  }
  return 0;
}

int vsid_ui_key_press(int *block)
{
  if ((VSidReady != 0) && (block[KeyPB_Icon] == Icon_VSid_Tune))
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
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Name, name);
}

void vsid_ui_display_author(const char *author)
{
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Author, author);
}

void vsid_ui_display_copyright(const char *copyright)
{
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Copyright, copyright);
}

void vsid_ui_display_sync(int sync)
{
}

void vsid_ui_display_sid_model(int model)
{
    /* FIXME */
    log_message(LOG_DEFAULT, "Using %s emulation",
		model == 0 ? "MOS6581" : "MOS8580");
}

void vsid_ui_set_default_tune(int nr)
{
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Default, nr);
}

void vsid_ui_display_tune_nr(int nr)
{
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Tune, nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_TotalTunes, count);

    NumTunes = count;
}

void vsid_ui_display_time(unsigned int sec)
{
    if (PlayTimeSec != sec)
    {
      unsigned int minutes, seconds;
      char buffer[8];

      PlayTimeSec = sec;
      minutes = sec / 60; seconds = sec - 60*minutes;
      sprintf(buffer, "%2d:%02d", minutes, seconds);
      wimp_window_write_icon_text(VSidWindow, Icon_VSid_PlayTime, buffer);
    }
}

void vsid_ui_display_speed(int percent)
{
    char buffer[32];

    sprintf(buffer, "%d%%", percent);
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Speed, buffer);
}

void vsid_ui_display_irqtype(const char *irq)
{
}

void vsid_ui_close(void)
{
}
