/*
 * kbd.c - Acorn keyboard driver.
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

#include <string.h>

#include "kbd.h"
#include "vsync.h"
#include "ROlib.h"
#include "joystick.h"
#include "ui.h"
#include "vmachine.h"
#include "interrupt.h"
#include "resources.h"
#include "vice.h"
#include "utils.h"





#define MAXIMUM_KEYMAPS	4

#define IntKey_MinCode   3	/* Scan from ShiftLeft (leave out Shift, Ctrl, Alt) */
#define IntKey_MaxCode   124



typedef struct keymap_desc {
  int total_number;
  unsigned char *keymaps[MAXIMUM_KEYMAPS];
} keymap_desc;


keymap_desc ViceKeymap = {
  0
};



/* Special keycodes that have to be processed seperately: */
#define IntKey_ShiftL	3
#define IntKey_ShiftR	6
#define IntKey_CapsLock	64
#define IntKey_CrsrL	25
#define IntKey_CrsrR	121
#define IntKey_CrsrU	57
#define IntKey_CrsrD	41
#define IntKey_Insert	61
#define IntKey_NumLock	77
#define IntKey_F5	116
#define IntKey_F6	117
#define IntKey_F7	22
#define IntKey_F8	118
#define IntKey_PageUp	63
#define IntKey_PageDown	78
#define IntKey_NumSlash	74
#define IntKey_NumStar	91
#define IntKey_NumCross	90
#define IntKey_Copy	105

#define KeyJoy1_Up	108	/* num3		*/
#define KeyJoy1_Down	76	/* num.		*/
#define KeyJoy1_Left	107	/* num1		*/
#define KeyJoy1_Right	124	/* num2		*/
#define KeyJoy1_Fire	60	/* numReturn	*/
#define KeyJoy2_Up	67	/* "f"		*/
#define KeyJoy2_Down	82	/* "c"		*/
#define KeyJoy2_Left	97	/* "z"		*/
#define KeyJoy2_Right	66	/* "x"		*/
#define KeyJoy2_Fire	83	/* "g"		*/




int keyarr[KBD_ROWS], rev_keyarr[KBD_COLS];
BYTE joy[3];

Joy_Keys JoystickKeys[2] = {
  {KeyJoy1_Up, KeyJoy1_Down, KeyJoy1_Left, KeyJoy1_Right, KeyJoy1_Fire},
  {KeyJoy2_Up, KeyJoy2_Down, KeyJoy2_Left, KeyJoy2_Right, KeyJoy2_Fire}
};

static unsigned char last_keys[32];

static int LastTasking = 0;
static int LastMousePtr = 0;

static int keymap_index;



void kbd_init_keymap(int number)
{
  ViceKeymap.total_number = number;
}

int kbd_add_keymap(unsigned char *map, int number)
{
  if (number >= MAXIMUM_KEYMAPS) return -1;
  ViceKeymap.keymaps[number] = map;
  return 0;
}

int kbd_init(void)
{
  /*kbd_init_resources();*/
  memset(last_keys, 0, 32);
  return 0;
}



static char Rsrc_JoyKeys1[] = "JoystickKeys1";
static char Rsrc_JoyKeys2[] = "JoystickKeys2";


static char DefaultJoyKeys1[12];
static char DefaultJoyKeys2[12];
static char *JoyKeyString1 = NULL;
static char *JoyKeyString2 = NULL;

#define READ_HEX_NIBBLE \
  if ((*b >= '0') && (*b <= '9')) h = (*b - '0'); \
  else if ((*b >= 'a') && (*b <= 'f')) h = (*b - 'a' + 10); \
  else if ((*b >= 'A') && (*b <= 'F')) h = (*b - 'A' + 10); \
  else return -1;

#define READ_HEX_BYTE \
  READ_HEX_NIBBLE; b++; val = h<<4; READ_HEX_NIBBLE; val |= h; b++;

static int kbd_parse_joykeys(const char *desc, Joy_Keys *jk)
{
  char *b = (char*)desc;
  unsigned char h, val;

  READ_HEX_BYTE; jk->up = val;
  READ_HEX_BYTE; jk->down = val;
  READ_HEX_BYTE; jk->left = val;
  READ_HEX_BYTE; jk->right = val;
  READ_HEX_BYTE; jk->fire = val;
  return 0;
}

int kbd_update_joykeys(int port)
{
  char b[12];
  char *rsrc;

  sprintf(b, "%2x%2x%2x%2x%2x", JoystickKeys[port].up, JoystickKeys[port].down, JoystickKeys[port].left, JoystickKeys[port].right, JoystickKeys[port].fire);
  rsrc = (port == 0) ? Rsrc_JoyKeys1 : Rsrc_JoyKeys2;
  return resources_set_value(rsrc, b);
}

static int set_keymap_index(resource_value_t v)
{
  keymap_index = (int)v;
  memset(last_keys, 0, 32);
  return 0;
}

static int set_joystick_keys1(resource_value_t v)
{
  const char *name = (const char *)v;

  if ((JoyKeyString1 != NULL) && (name != NULL) && (strcmp(name, JoyKeyString1) == 0))
    return 0;

  if (kbd_parse_joykeys(name, JoystickKeys) != 0) return 1;

  string_set(&JoyKeyString1, name);

  return 0;
}

static int set_joystick_keys2(resource_value_t v)
{
  const char *name = (const char *)v;

  if ((JoyKeyString2 != NULL) && (name != NULL) && (strcmp(name, JoyKeyString2) == 0))
    return 0;

  if (kbd_parse_joykeys((char*)v, JoystickKeys+1) != 0) return 1;

  string_set(&JoyKeyString2, name);

  return 0;
}

static resource_t resources[] = {
  {"KeymapIndex", RES_INTEGER, (resource_value_t) 0,
    (resource_value_t *)&keymap_index, set_keymap_index},
  {Rsrc_JoyKeys1, RES_STRING, (resource_value_t) DefaultJoyKeys1,
    (resource_value_t *)&JoyKeyString1, set_joystick_keys1},
  {Rsrc_JoyKeys2, RES_STRING, (resource_value_t) DefaultJoyKeys2,
    (resource_value_t *)&JoyKeyString2, set_joystick_keys2},
  {NULL}
};

int kbd_init_resources(void)
{
  sprintf(DefaultJoyKeys1, "%2x%2x%2x%2x%2x", KeyJoy1_Up, KeyJoy1_Down, KeyJoy1_Left, KeyJoy1_Right, KeyJoy1_Fire);
  sprintf(DefaultJoyKeys2, "%2x%2x%2x%2x%2x", KeyJoy2_Up, KeyJoy2_Down, KeyJoy2_Left, KeyJoy2_Right, KeyJoy2_Fire);

  return resources_register(resources);
}


int kbd_init_cmdline_options(void)
{
  return 0;
}


void kbd_poll(void)
{
  int i, scan, code, row, col, status, joyset;
  unsigned char new_keys[32];
  int shr, shc;
  unsigned char *keymap;

  /* Don't do anything if we don't have the input focus */
  if (EmuWindowHasInputFocus == 0) return;

  if (keymap_index >= ViceKeymap.total_number) return;

  keymap = ViceKeymap.keymaps[keymap_index];

  shr = keymap[IntKey_ShiftL]; shc = shr & 0x0f; shr >>= 4;

  do
  {

    for (i=0; i<KBD_ROWS; i++) keyarr[i] = 0;
    for (i=0; i<KBD_COLS; i++) rev_keyarr[i] = 0;
    joy[0] = 0;
    /* If the joystick isn't a real one we clear it here */
    for (i=0; i<2; i++)
    {
      if ((joystick_port_map[i] != JOYDEV_JOY1) && (joystick_port_map[i] != JOYDEV_JOY2))
      {
        joy[i+1] = 0;
      }
    }
    memset(new_keys, 0, 32);

    status = ReadKeyboardStatus();

    /* Caps lock? */
    if ((status & 16) == 0)
    {
      keyarr[shr] |= (1<<shc);
      rev_keyarr[shc] |= (1<<shr);
    }

    /* Scroll lock? (-> single tasking) */
    SingleTasking = ((status & 2) != 0);

    if ((SingleTasking ^ LastTasking) != 0)
    {
      if (SingleTasking != 0)
      {
        LastMousePtr = SetMousePointer(0);
      }
      else
      {
        SetMousePointer(LastMousePtr);
        /* Flush mouse and keyboard buffers */
        OS_FlushBuffer(9); OS_FlushBuffer(0);
      }
      LastTasking = SingleTasking;
    }

    scan = IntKey_MinCode;
    while (scan < IntKey_MaxCode)
    {
      if ((code = ScanKeys(scan)) != 0xff)
      {
        new_keys[(code>>3)] |= (1<<(code&7));
        row = keymap[code];
        for (i=0; i<2; i++)
        {
          joyset = -1;
          if (joystick_port_map[i] == JOYDEV_KBD1) joyset = 0;
          else if (joystick_port_map[i] == JOYDEV_KBD2) joyset = 1;

          if (joyset >= 0)
          {
            if (code == JoystickKeys[joyset].up) {joy[i+1] |= 1; row = 0xff;}
            else if (code == JoystickKeys[joyset].down) {joy[i+1] |= 2; row = 0xff;}
            else if (code == JoystickKeys[joyset].left) {joy[i+1] |= 4; row = 0xff;}
            else if (code == JoystickKeys[joyset].right) {joy[i+1] |= 8; row = 0xff;}
            else if (code == JoystickKeys[joyset].fire) {joy[i+1] |= 16; row = 0xff;}
          }
        }

        if (row != 0xff)
        {
          col = row & 15; row >>= 4;
          if ((col < KBD_COLS) && (row < KBD_ROWS))
          {
            keyarr[row] |= (1<<col); rev_keyarr[col] |= (1<<row);
          }
        }
      }
      switch (code)
      {
        /* Keys that need shift on the C64 */
        case IntKey_CrsrL:
        case IntKey_CrsrU:
        case IntKey_Insert:
          {
            keyarr[shr] |= (1<<shc);
            rev_keyarr[shc] |= (1<<shr);
          }
          break;
        default: break;
      }

      /* Was key already pressed last time? */
      if ((last_keys[code>>3] & (1<<(code&7))) == 0)
      {
        switch (code)
        {
          case IntKey_NumSlash:
            ui_toggle_truedrv_emulation();
            break;
          case IntKey_F5:
            ui_toggle_sid_emulation();
            break;
          case IntKey_F6:
            ui_activate_monitor();
            break;
          case IntKey_F7:
            maincpu_set_nmi(I_RESTORE, 1);
            break;
          case IntKey_F8:
            maincpu_trigger_reset();
            break;
          case IntKey_Copy:	/* Single tasking only! */
            if (SingleTasking != 0)
            {
              EmuPaused ^= 1;
              ui_display_paused(EmuPaused);
            }
            break;
          default: break;
        }
      }

      scan = code+1;
    }

    memcpy(last_keys, new_keys, 32);
  }
  while ((SingleTasking != 0) && (EmuPaused != 0));
}




/* PET-specific stuff */
int pet_kbd_init_resources(void)
{
  return 0;
}


int pet_kbd_init_cmdline_options(void)
{
  return 0;
}
