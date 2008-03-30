/*
 * c64ui.c - Implementation of the C64-specific part of the UI.
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

#include <wimp.h>

#include "ui.h"
#include "c64ui.h"
#include "kbd.h"
#include "machine.h"
#include "c64c128ui.h"
#include "uisharedef.h"
#include "uiconfig.h"
#include "uihelp.h"
#include "vsidarch.h"
#include "vsync.h"
#include "c64/c64mem.h"



#define FileType_SIDMusic	0x063

extern RO_Window *VSidWindow;

static const char IBarIconName64[] = "!vice64";
static const char IBarIconNameVSID[] = "!vicevsid";
static const char C64keyfile[] = "Vice:C64.ROdflt/vkm";

static const conf_iconid_t conf_grey_x64[] = {
  ICON_LIST_PET
  ICON_LIST_VIC
  {CONF_WIN_DEVICES, Icon_ConfDev_ACIAD67},
  {0xff, 0xff}
};


static const char Rsrc_VICIICache[] = "VICIIVideoCache";

static struct MenuVideoCache {
  RO_MenuHead head;
  RO_MenuItem item[1];
} MenuVideoCache = {
  MENU_HEADER("\\MenVCaT", 200),
  {
    MENU_ITEM_LAST("\\MenVCaVIC2")
  }
};

static struct MenuDisplayVideoCache {
  disp_desc_t dd;
  const char *values[1];
} MenuDisplayVideoCache = {
  {NULL, {CONF_WIN_VIDEO, 0},
    (RO_MenuHead*)&MenuVideoCache, 1, DISP_DESC_BITFIELD, 0},
  {Rsrc_VICIICache}
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


static void c64ui_grey_out_machine_icons(void)
{
  ui_set_icons_grey(NULL, conf_grey_x64, 0);
}

static void c64ui_bind_video_cache_menu(void)
{
  ConfigMenus[CONF_MENU_VIDCACHE].menu = (RO_MenuHead*)&MenuVideoCache;
  ConfigMenus[CONF_MENU_VIDCACHE].desc = (disp_desc_t*)&MenuDisplayVideoCache;
}

static const char *c64ui_get_machine_ibar_icon(void)
{
  return ((vsid_mode) ? IBarIconNameVSID : IBarIconName64);
}


static int c64ui_mouse_click_event(int *block)
{
  if (vsid_mode && (block[MouseB_Window] == VSidWindow->Handle))
  {
    if (block[MouseB_Buttons] == 2)
    {
      ui_create_emulator_menu(block);
    }
    else
    {
      vsid_ui_mouse_click(block);
    }
    return 0;
  }
  return -1;
}

static int c64ui_mouse_click_ibar(int *block)
{
  if (vsid_mode)
  {
    ui_open_centered_or_raise_block(VSidWindow, block);
    Wimp_OpenWindow(block);
    return 0;
  }
  return -1;
}

static int c64ui_key_pressed_event(int *block)
{
  if (vsid_mode && (block[KeyPB_Window] == VSidWindow->Handle))
  {
    vsid_ui_key_press(block);
    return 0;
  }
  return -1;
}

static int c64ui_usr_msg_data_load(int *block)
{
  if (vsid_mode && (block[5] == VSidWindow->Handle))
  {
    return vsid_ui_load_file(((const char*)block)+44);
  }
  return -1;
}

static int c64ui_usr_msg_data_open(int *block)
{
  if ((vsid_mode) && (block[10] == FileType_SIDMusic))
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

static int c64ui_load_prg_file(const char *filename)
{
  FILE *fp;

  vsync_suspend_speed_eval();

  if ((fp = fopen(filename, "rb")) != NULL)
  {
    BYTE lo, hi;
    int length;

    lo = fgetc(fp); hi = fgetc(fp); length = lo + (hi << 8);
    length += fread(mem_ram + length, 1, C64_RAM_SIZE - length, fp);
    fclose(fp);
    mem_ram[0xc3] = lo; mem_ram[0xc4] = hi;
    lo = length & 0xff; hi = (length >> 8) & 0xff;
    mem_ram[0xae] = lo; mem_ram[0x2d] = lo; mem_ram[0x2f] = lo; mem_ram[0x31] = lo; mem_ram[0x33] = lo;
    mem_ram[0xaf] = hi; mem_ram[0x2e] = hi; mem_ram[0x30] = hi; mem_ram[0x32] = hi; mem_ram[0x34] = hi;
    return 0;
  }
  return -1;
}

static int c64ui_display_speed(int percent, int framerate, int warp_flag)
{
  if (vsid_mode)
  {
    vsid_ui_display_speed(percent);
    return 0;
  }
  return -1;
}

help_icon_t *c64ui_help_for_window_icon(int handle, int icon)
{
  if ((vsid_mode) && (handle == VSidWindow->Handle))
  {
    return Help_VSidWindow;
  }
  return NULL;
}

static void c64ui_init_callbacks(void)
{
  c64c128_ui_cartridge_callbacks();
  ViceMachineCallbacks.mouse_click_event = c64ui_mouse_click_event;
  ViceMachineCallbacks.mouse_click_ibar = c64ui_mouse_click_ibar;
  ViceMachineCallbacks.key_pressed_event = c64ui_key_pressed_event;
  ViceMachineCallbacks.usr_msg_data_load = c64ui_usr_msg_data_load;
  ViceMachineCallbacks.usr_msg_data_open = c64ui_usr_msg_data_open;
  ViceMachineCallbacks.load_prg_file = c64ui_load_prg_file;
  ViceMachineCallbacks.display_speed = c64ui_display_speed;
  ViceMachineCallbacks.help_for_window_icon = c64ui_help_for_window_icon;
}


int c64ui_init(void)
{
  wimp_msg_desc *msg;

  WimpTaskName = (vsid_mode) ? "Vice VSID" : "Vice C64";
  c64ui_init_callbacks();
  c64ui_bind_video_cache_menu();
  msg = ui_emulator_init_prologue(c64ui_get_machine_ibar_icon());
  if (msg != NULL)
  {
    if (vsid_mode)
    {
      ui_load_template("VSidWindow", &VSidWindow, msg);
      vsid_ui_message_hook(msg);
    }

    ui_translate_icon_help_msgs(msg, Help_VSidWindow);
    ui_emulator_init_epilogue(msg);
    c64ui_grey_out_machine_icons();
    return 0;
  }
  return -1;
}

void c64ui_shutdown(void)
{
}

int c64_kbd_init(void)
{
  c64c128_ui_init_keyboard(C64keyfile);
  kbd_load_keymap(NULL, 0);
  return kbd_init();
}
