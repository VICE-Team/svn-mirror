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
#include "uihelp.h"
#include "uisharedef.h"
#include "vsidui.h"
#include "vsidarch.h"
#include "vsync.h"

#include "drive/drive.h"
#include "c64/psid.h"



extern void c64ui_grey_out_machine_icons(void);
extern void c64ui_bind_video_cache_menu(void);


#define FileType_SIDMusic	0x063

static RO_Window *VSidWindow = NULL;

static int NumTunes = 0;
static int VSidPaused = 0;
static int VSidStopped = 0;
static int VSidReady = 0;
static unsigned int PlayTimeSec = 0;

static const char IBarIconNameVSID[] = "!vicevsid";
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

static help_icon_t Help_VSidWindow[] = {
  {-1, "\\HelpVSidWindow"},
  {Icon_VSid_TotalTunes, "\\HelpVSidTotal"},
  {Icon_VSid_Tune, "\\HelpVSidTune"},
  {Icon_VSid_NextTune, "\\HelpVSidNext"},
  {Icon_VSid_PrevTune, "\\HelpVSidPrev"},
  {Icon_VSid_StopTune, "\\HelpVSidStop"},
  {Icon_VSid_Default, "\\HelpVSidDefault"},
  {Icon_VSid_PlayTime, "\\HelpVSidTime"},
  {Icon_VSid_Pause, "\\HelpVSidPause"},
  {Help_Icon_End, NULL}
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

  if (VSidWindow != NULL)
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

  if (VSidWindow != NULL)
  {
    wimp_window_set_icon_state(VSidWindow, Icon_VSid_Pause, iflg, IFlg_Grey);

    wimp_window_write_icon_text(VSidWindow, Icon_VSid_StopTune, text);
  }
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

  if (VSidWindow != NULL)
  {
    wimp_window_set_icon_state(VSidWindow, Icon_VSid_StopTune, iflg, IFlg_Grey);
    wimp_window_set_icon_state(VSidWindow, Icon_VSid_Tune, iflg, IFlg_Grey);
    wimp_window_set_icon_state(VSidWindow, Icon_VSid_NextTune, iflg, IFlg_Grey);
    wimp_window_set_icon_state(VSidWindow, Icon_VSid_PrevTune, iflg, IFlg_Grey);
  }

  vsid_ui_display_paused(VSidPaused);
  vsid_ui_display_stopped(VSidStopped);

  VSidReady = state;
}

void vsid_set_tune(int tune)
{
  resources_set_int(Rsrc_Tune, tune);
  vsid_ui_display_time(0);
}

static int vsid_ui_load_file(const char *file)
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

static const char *vsid_ui_get_machine_ibar_icon(void)
{
  return IBarIconNameVSID;
}

static int vsid_ui_mouse_click_event(int *block)
{
  if ((VSidWindow != NULL) && (block[MouseB_Window] == VSidWindow->Handle))
  {
    if (block[MouseB_Buttons] == 2)
    {
      ui_create_emulator_menu(block);
    }
    else if (VSidReady != 0)
    {
      int number;

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
          break;
      }
    }
    return 0;
  }
  return -1;
}

static int vsid_ui_mouse_click_ibar(int *block)
{
  ui_open_centered_or_raise_block(VSidWindow, block);
  Wimp_OpenWindow(block);
  return 0;
}

static int vsid_ui_key_pressed_event(int *block)
{
  if ((VSidWindow != NULL) && (block[KeyPB_Window] == VSidWindow->Handle))
  {
    if ((VSidReady != 0) && (block[KeyPB_Icon] == Icon_VSid_Tune))
    {
      int number;

      number = wimp_window_read_icon_number(VSidWindow, Icon_VSid_Tune);

      if ((number > 0) && (number <= NumTunes))
      {
        vsid_set_tune(number);
      }
    }
    return 0;
  }
  return -1;
}

static int vsid_ui_usr_msg_data_load(int *block)
{
  if (block[5] == VSidWindow->Handle)
  {
    return vsid_ui_load_file(((const char*)block)+44);
  }
  return -1;
}

static int vsid_ui_usr_msg_data_open(int *block)
{
  if (block[10] == FileType_SIDMusic)
  {
    if (vsid_ui_load_file(((const char*)block)+44) == 0)
    {
      block[MsgB_YourRef] = block[MsgB_MyRef]; block[MsgB_Action] = Message_DataLoadAck;
      Wimp_SendMessage(18, block, block[MsgB_Sender], block[6]);
      return 0;
    }
  }
  return -1;
}

static int vsid_ui_display_speed(int percent, int framerate, int warp_flag)
{
  if (VSidWindow != NULL)
  {
    char buffer[32];

    sprintf(buffer, "%d%%", percent);
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Speed, buffer);
  }
  return 0;
}

static help_icon_t *vsid_ui_help_for_window_icon(int handle, int icon)
{
  if (handle == VSidWindow->Handle)
  {
    return Help_VSidWindow;
  }
  return NULL;
}

static void vsid_ui_init_callbacks(void)
{
  ViceMachineCallbacks.mouse_click_event = vsid_ui_mouse_click_event;
  ViceMachineCallbacks.mouse_click_ibar = vsid_ui_mouse_click_ibar;
  ViceMachineCallbacks.key_pressed_event = vsid_ui_key_pressed_event;
  ViceMachineCallbacks.usr_msg_data_load = vsid_ui_usr_msg_data_load;
  ViceMachineCallbacks.usr_msg_data_open = vsid_ui_usr_msg_data_open;
  ViceMachineCallbacks.display_speed = vsid_ui_display_speed;
  ViceMachineCallbacks.help_for_window_icon = vsid_ui_help_for_window_icon;
}

static int vsid_ui_message_hook(struct wimp_msg_desc_s *msg)
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


int vsid_ui_init(void)
{
  wimp_msg_desc *msg;

  WimpTaskName = "Vice VSID";
  vsid_ui_init_callbacks();
  c64ui_bind_video_cache_menu();
  resources_load(NULL);

  msg = ui_emulator_init_prologue(vsid_ui_get_machine_ibar_icon());
  if (msg != NULL)
  {
    int block[WindowB_WFlags+1];

    ui_load_template("VSidWindow", &VSidWindow, msg);
    vsid_ui_message_hook(msg);
    ui_translate_icon_help_msgs(msg, Help_VSidWindow);
    ui_emulator_init_epilogue(msg);
    c64ui_grey_out_machine_icons();

    ui_open_centered_or_raise_block(VSidWindow, block);
    Wimp_OpenWindow(block);

    vsid_ui_set_ready(0);

    return 0;
  }

  return -1;
}

void vsid_ui_display_name(const char *name)
{
  if (VSidWindow != NULL)
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Name, name);
}

void vsid_ui_display_author(const char *author)
{
  if (VSidWindow != NULL)
    wimp_window_write_icon_text(VSidWindow, Icon_VSid_Author, author);
}

void vsid_ui_display_copyright(const char *copyright)
{
  if (VSidWindow != NULL)
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
  if (VSidWindow != NULL)
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Default, nr);
}

void vsid_ui_display_tune_nr(int nr)
{
  if (VSidWindow != NULL)
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_Tune, nr);
}

void vsid_ui_display_nr_of_tunes(int count)
{
  if (VSidWindow != NULL)
    wimp_window_write_icon_number(VSidWindow, Icon_VSid_TotalTunes, count);

    NumTunes = count;
}

void vsid_ui_display_time(unsigned int sec)
{
    if ((VSidWindow != NULL) && (PlayTimeSec != sec))
    {
      unsigned int minutes, seconds;
      char buffer[8];

      PlayTimeSec = sec;
      minutes = sec / 60; seconds = sec - 60*minutes;
      sprintf(buffer, "%2d:%02d", minutes, seconds);
      wimp_window_write_icon_text(VSidWindow, Icon_VSid_PlayTime, buffer);
    }
}

void vsid_ui_display_irqtype(const char *irq)
{
}

void vsid_ui_close(void)
{
}
