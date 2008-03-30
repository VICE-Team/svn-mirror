/*
 * kbd.c - Acorn keyboard driver.
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

#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "posix.h"

#include "kbd.h"
#include "vsyncarch.h"
#include "joy.h"
#include "keyboard.h"
#include "log.h"
#include "machine.h"
#include "ui.h"
#include "interrupt.h"
#include "resources.h"
#include "types.h"
#include "videoarch.h"
#include "vice.h"
#include "util.h"





#define MAXIMUM_KEYMAPS	4
#define BUFFERSIZE	256


#define IntKey_MinCode   3	/* Scan from ShiftLeft (leave out Shift, Ctrl, Alt) */
#define IntKey_MaxCode   124



typedef struct keymap_desc {
  int total_number;
  kbd_keymap_t keymaps[MAXIMUM_KEYMAPS];
} keymap_desc;


keymap_desc ViceKeymap = {
  0
};


/* Special keycodes that have to be processed seperately: */
#define IntKey_ShiftL	3
#define IntKey_ShiftR	6
#define IntKey_CtrlL	4
#define IntKey_CtrlR	7
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
#define IntKey_F9	119
#define IntKey_F10	30
#define IntKey_F12	29
#define IntKey_Print	32
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




/*BYTE joystick_value[3];*/

Joy_Keys JoystickKeys[2] = {
  {KeyJoy1_Up, KeyJoy1_Down, KeyJoy1_Left, KeyJoy1_Right, KeyJoy1_Fire},
  {KeyJoy2_Up, KeyJoy2_Down, KeyJoy2_Left, KeyJoy2_Right, KeyJoy2_Fire}
};

/* 40/80 column key.  */
static key_ctrl_column4080_func_t key_ctrl_column4080_func = NULL;

/* CAPS key.  */
static key_ctrl_caps_func_t key_ctrl_caps_func = NULL;

static unsigned char last_keys[32];

static int LastTasking = 0;
static int LastMousePtr = 0;

static int keymap_index;

static int keyboard_int_num;



/* Lookup table internal keynumbers to descriptive strings */
static const char *IntKeyToString[KEYMAP_ENTRIES] = {
  "Shft", "Ctrl", "Alt", "ShftL",	/*   0 -   3 */
  "CtrlL", "AltL", "ShftR", "CtrlR",	/*   4 -   7 */
  "AltR", "Slct", "Menu", "Adjst",	/*   8 -  11 */
  NULL, NULL, NULL, NULL,		/*  12 -  15 */
  "q", "3", "4", "5",			/*  16 -  19 */
  "F4", "8", "F7", "-",			/*  20 -  23 */
  "6", "CrsrL", "num6", "num7",		/*  24 -  27 */
  "F11", "F12", "F10", "ScrLck",	/*  28 -  31 */
  "Print", "w", "e", "t",		/*  32 -  35 */
  "7", "i", "9", "0",			/*  36 -  39 */
  "-", "CrsrD", "num8", "num9",		/*  40 -  43 */
  "Break", "`", "£", "BackSpc",		/*  44 -  47 */
  "1", "2", "d", "r",			/*  48 -  51 */
  "6", "u", "o", "p",			/*  52 -  55 */
  "[", "CrsrU", "num+", "num-",		/*  56 -  59 */
  "numEntr", "Insert", "Home", "PgUp",	/*  60 -  63 */
  "CpsLck", "a", "x", "f",		/*  64 -  67 */
  "y", "j", "k", "2",			/*  68 -  71 */
  ";", "Return", "num/", NULL,		/*  72 -  75 */
  "num.", "numLck", "PgDown", "\'",	/*  76 -  79 */
  NULL, "s", "c", "g",			/*  80 -  83 */
  "h", "n", "l", ";",			/*  84 -  87 */
  "]", "Delete", "num#", "num*",	/*  88 -  91 */
  NULL, "=", "Extra", NULL,		/*  92 -  95 */
  "Tab", "z", "Space", "v",		/*  96 -  99 */
  "b", "m", ",", ".",			/* 100 - 103 */
  "/", "Copy", "num0", "num1",		/* 104 - 107 */
  "num3", NULL, NULL, NULL,		/* 108 - 111 */
  "Esc", "F1", "F2", "F3",		/* 112 - 115 */
  "F5", "F6", "F8", "F9",		/* 116 - 119 */
  "\\", "CrsrR", "num4", "num5",	/* 120 - 123 */
  "num2", NULL, NULL, NULL		/* 124 - 127 */
};


static const char keymap_clear_table[] = "!CLEAR";




void kbd_init_keymap(int number)
{
  ViceKeymap.total_number = number;
}

int kbd_add_keymap(kbd_keymap_t *map, int number)
{
  if (number >= MAXIMUM_KEYMAPS) return -1;
  memcpy(ViceKeymap.keymaps + number, map, sizeof(kbd_keymap_t));
  /*{
    char buffer[64];
    sprintf(buffer, "keymap%d", number);
    kbd_dump_keymap(buffer, number);
  }*/
  return 0;
}

/* inits shifted, and the sflags members to default values that are OK for all emulator */
int kbd_default_keymap(kbd_keymap_t *map)
{
  memset(map->shifted, 0xff, KEYMAP_ENTRIES);	/* no special shifted keys */
  memset(map->shift_sflag, 0, KEYMAP_ENTRIES/8);
  memset(map->norm_sflag, 0, KEYMAP_ENTRIES/8);
  map->norm_sflag[IntKey_CrsrL >> 3] |= (1 << (IntKey_CrsrL & 7));
  map->norm_sflag[IntKey_CrsrU >> 3] |= (1 << (IntKey_CrsrU & 7));
  map->norm_sflag[IntKey_Insert >> 3] |= (1 << (IntKey_Insert & 7));

  return 0;
}

const char *kbd_intkey_to_string(int intkey)
{
  if ((intkey < 0) || (intkey >= KEYMAP_ENTRIES)) return NULL;
  return IntKeyToString[intkey];
}

int kbd_init(void)
{
  /*kbd_resources_init();*/
  memset(last_keys, 0, 32);
  keyboard_int_num = interrupt_cpu_status_int_new(maincpu_int_status,
                                                  "RestoreKEY");
  return 0;
}

/* PET-specific stuff */
int pet_kbd_resources_init(void)
{
    return kbd_resources_init();
}

int pet_kbd_cmdline_options_init(void)
{
    return kbd_cmdline_options_init();
}

static const char Rsrc_JoyKeys1[] = "JoystickKeys1";
static const char Rsrc_JoyKeys2[] = "JoystickKeys2";


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
  const char *rsrc;

  sprintf(b, "%02x%02x%02x%02x%02x", JoystickKeys[port].up, JoystickKeys[port].down, JoystickKeys[port].left, JoystickKeys[port].right, JoystickKeys[port].fire);
  rsrc = (port == 0) ? Rsrc_JoyKeys1 : Rsrc_JoyKeys2;
  return resources_set_value(rsrc, b);
}

static int set_keymap_index(resource_value_t v, void *param)
{
  keymap_index = (int)v;
  memset(last_keys, 0, 32);
  return 0;
}

static int set_joystick_keys1(const char *name, void *param)
{
  if ((JoyKeyString1 != NULL) && (name != NULL) && (strcmp(name, JoyKeyString1) == 0))
    return 0;

  if (kbd_parse_joykeys(name, JoystickKeys) != 0) return 1;

  util_string_set(&JoyKeyString1, name);

  return 0;
}

static int set_joystick_keys2(const char *name, void *param)
{
  if ((JoyKeyString2 != NULL) && (name != NULL) && (strcmp(name, JoyKeyString2) == 0))
    return 0;

  if (kbd_parse_joykeys((char*)v, JoystickKeys+1) != 0) return 1;

  util_string_set(&JoyKeyString2, name);

  return 0;
}

static const resource_string_t resources_string[] = {
  {Rsrc_JoyKeys1, DefaultJoyKeys1, RES_EVENT_NO, NULL,
    &JoyKeyString1, set_joystick_keys1, NULL},
  {Rsrc_JoyKeys2, DefaultJoyKeys2, RES_EVENT_NO, NULL,
    &JoyKeyString2, set_joystick_keys2, NULL},
  {NULL}
};

static const resource_int_t resources_int[] = {
  {"KeymapIndex", 0, RES_EVENT_NO, NULL,
    &keymap_index, set_keymap_index, NULL },
  {NULL}
};

int kbd_resources_init(void)
{
  sprintf(DefaultJoyKeys1, "%02x%02x%02x%02x%02x", KeyJoy1_Up, KeyJoy1_Down, KeyJoy1_Left, KeyJoy1_Right, KeyJoy1_Fire);
  sprintf(DefaultJoyKeys2, "%02x%02x%02x%02x%02x", KeyJoy2_Up, KeyJoy2_Down, KeyJoy2_Left, KeyJoy2_Right, KeyJoy2_Fire);

  if (resources_register_string(resources_string) < 0)
    return -1;

  return resources_register_int(resources_int);
}


int kbd_cmdline_options_init(void)
{
  return 0;
}


void kbd_poll(void)
{
  int i, scan, code, row, col, status, joyset;
  unsigned char new_keys[32];
  int shr, shc;
  kbd_keymap_t *keymap;
  int shiftPressed = 0;
  int ctrlPressed = 0;

  /* Don't do anything if we don't have the input focus */
  if (EmuWindowHasInputFocus == 0) return;

  if (keymap_index >= ViceKeymap.total_number) return;

  keymap = &(ViceKeymap.keymaps[keymap_index]);

  shr = keymap->normal[IntKey_ShiftL]; shc = shr & 0x0f; shr >>= 4;

  do
  {

    for (i=0; i<KBD_ROWS; i++) keyarr[i] = 0;
    for (i=0; i<KBD_COLS; i++) rev_keyarr[i] = 0;
    joystick_value[0] = 0;
    /* If the joystick isn't a real one we clear it here */
    for (i=0; i<2; i++)
    {
      if ((joystick_port_map[i] != JOYDEV_JOY1) && (joystick_port_map[i] != JOYDEV_JOY2))
      {
        joystick_value[i+1] = 0;
      }
    }
    memset(new_keys, 0, 32);

    status = ReadKeyboardStatus();

    /* Caps lock? */
    if ((status & 16) == 0)
    {
      keyarr[shr] |= (1<<shc);
      rev_keyarr[shc] |= (1<<shr);
      shiftPressed = 1;
    }

    if (FullScreenMode == 0)
    {
      /* Scroll lock? (-> single tasking) */
      SingleTasking = ((status & 2) != 0);
    }

    scan = IntKey_MinCode;
    while (scan < IntKey_MaxCode)
    {
      if ((code = ScanKeys(scan)) != 0xff)
      {
        if ((code == IntKey_ShiftL) || (code == IntKey_ShiftR)) shiftPressed = 1;
        if ((code == IntKey_CtrlL) || (code == IntKey_CtrlR)) ctrlPressed = 1;

        new_keys[(code>>3)] |= (1<<(code&7));
        row = code;
        for (i=0; i<2; i++)
        {
          joyset = -1;
          if (joystick_port_map[i] == JOYDEV_KBD1) joyset = 0;
          else if (joystick_port_map[i] == JOYDEV_KBD2) joyset = 1;

          if (joyset >= 0)
          {
            if (code == JoystickKeys[joyset].up) {joystick_value[i+1] |= 1; row = 0xff;}
            else if (code == JoystickKeys[joyset].down) {joystick_value[i+1] |= 2; row = 0xff;}
            else if (code == JoystickKeys[joyset].left) {joystick_value[i+1] |= 4; row = 0xff;}
            else if (code == JoystickKeys[joyset].right) {joystick_value[i+1] |= 8; row = 0xff;}
            else if (code == JoystickKeys[joyset].fire) {joystick_value[i+1] |= 16; row = 0xff;}
          }
        }

        if (row != 0xff)
        {
          /* if no shift or no special code for the shifted key exists */
          if ((shiftPressed == 0) || (keymap->shifted[code] == 0xff))
          {
            row = keymap->normal[code];
            if (row != 0xff)
            {
              col = row & 15; row >>= 4;
              if ((col < KBD_COLS) && (row < KBD_ROWS))
              {
                keyarr[row] |= (1<<col);
                rev_keyarr[col] |= (1<<row);
              }
              /* if shift pressed or the key has the shift bit set */
              if ((shiftPressed != 0) || ((keymap->norm_sflag[code>>3] & (1<<(code&7))) != 0))
              {
                /* set the left shift key */
                keyarr[shr] |= (1<<shc);
                rev_keyarr[shc] |= (1<<shr);
              }
            }
          }
          else	/* implies shift pressed and special code for shifted key exists */
          {
            row = keymap->shifted[code];	/* because of the else branch != 0xff */
            col = row & 15; row >>= 4;
            if ((col < KBD_COLS) && (row < KBD_COLS))
            {
              keyarr[row] |= (1<<col);
              rev_keyarr[col] |= (1<<row);
            }
            /* note: should always be true, otherwise there's no reason for a separate map */
            if ((keymap->shift_sflag[code>>3] & (1<<(code&7))) == 0)
            {
              /* clear the left shift key */
              keyarr[shr] &= ~(1<<shc);
              rev_keyarr[shc] &= ~(1<<shr);
            }
          }
        }

        /* Special keys that shouldn't be used by the emulator but the UI only */
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
              maincpu_set_nmi(keyboard_int_num, 1);
              break;
            case IntKey_F8:
              machine_trigger_reset(MACHINE_RESET_MODE_SOFT);
              break;
            case IntKey_F9:
              if (ctrlPressed == 0)
              {
                if (FullScreenMode != 0)
                {
                  FullScreenStatLine = FullScreenStatLine ^ 1;
                  if (FullScreenStatLine == 0)
                    video_full_screen_refresh();
                  else
                    video_full_screen_init_status();
                }
              }
              else
              {
                ui_save_last_snapshot();
              }
              break;
            case IntKey_F10:
              if (ctrlPressed == 0)
              {
                if (FullScreenMode != 0)
                {
                  canvas_next_active(0);
                }
              }
              else
              {
                ui_trigger_snapshot_load();
              }
              break;
            case IntKey_F12:
              if (FullScreenMode != 0)
              {
                video_full_screen_off();
              }
              break;
            case IntKey_Copy:	/* Single tasking only! */
              if (SingleTasking != 0)
              {
                EmuPaused ^= 1;
                ui_display_paused(EmuPaused);
              }
              break;
            case IntKey_PageUp:
              ui_flip_iterate_and_attach(1);
              break;
            case IntKey_PageDown:
              ui_flip_iterate_and_attach(-1);
              break;
            case IntKey_Print:
              ui_make_last_screenshot();
              break;
            default:
              break;
          }
        }
      }
      scan = code+1;
    }

    memcpy(last_keys, new_keys, 32);

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
  }
  while ((SingleTasking != 0) && (EmuPaused != 0));
}

/* ------------------------------------------------------------------------ */
/*
 *  read / write keyboard mapping files
 */

static int kbd_load_keyboard_position(char **buffer, unsigned char *map, unsigned char *flags, int idx)
{
  char *b = *buffer;
  int row = 15, col = 15, shifted = 0;

  while (isspace((unsigned int)(*b))) b++;

  /* Is it the keystr in quotes at the end of line? Yes ==> return immediately */
  if (*b == '\"')
  {
    *buffer = b; return 0;
  }

  /* An empty rest of line is not an error */
  if ((unsigned char)(*b) >= 0x20)
  {
    char *rest;

    row = strtol(b, &rest, 10);
    if (b == rest) return -1;
    b = rest;
    col = strtol(b, &rest, 10);
    if (b == rest) return -1;
    b = rest;
    while (isspace((unsigned int)(*b))) b++;
    if ((*b == 's') || (*b == 'S'))
    {
      shifted = 1; b++;
    }
  }
  map[idx] = (row << 4) | col;
  if (shifted == 0)
    flags[idx>>3] &= ~(1 << (idx & 7));
  else
    flags[idx>>3] |= (1 << (idx & 7));

  *buffer = b;

  return 0;
}

static void kbd_dump_keyboard_position(FILE *fp, const unsigned char *map, const unsigned char *flags, int idx)
{
  fprintf(fp, " %2d %2d", map[idx] >> 4, map[idx] & 15);
  if ((flags[idx>>3] & (1<<(idx&7))) != 0)
  {
    fputs(" s", fp);
  }
}


int kbd_load_keymap(const char *filename, int number)
{
  FILE *fp;
  kbd_keymap_t *map;
  char buffer[BUFFERSIZE];
  int clear_length;
  int linenr;
  int useIndex;
  const char *useFile;

  useIndex = (number < 0) ? keymap_index : number;

  if (useIndex >= ViceKeymap.total_number)
  {
    log_warning(LOG_DEFAULT, "Bad keymap index %d", useIndex);
    return -1;
  }

  map = &ViceKeymap.keymaps[useIndex];

  useFile = (filename == NULL) ? map->default_file : filename;

  log_message(LOG_DEFAULT, "Loading keyboard map from file '%s'", useFile);

  if ((fp = fopen(useFile, "r")) == NULL)
  {
    log_warning(LOG_DEFAULT, "Unable to open file '%s'", useFile);
    return -1;
  }

  clear_length = strlen(keymap_clear_table);
  linenr = 0;

  while (!feof(fp))
  {
    char *b;

    if (fgets(buffer, BUFFERSIZE, fp) == NULL) break;
    linenr++;
    b = buffer;
    while (isspace((unsigned int)(*b))) b++;
    /* Commentary or empty line? */
    if ((*b != '#') && ((unsigned char)(*b) > 0x20))
    {
      if (strncasecmp(b, keymap_clear_table, clear_length) == 0)
      {
        memset(map->normal, 0xff, KEYMAP_ENTRIES);
        memset(map->shifted, 0xff, KEYMAP_ENTRIES);
        memset(map->norm_sflag, 0, KEYMAP_ENTRIES/8);
        memset(map->shift_sflag, 0, KEYMAP_ENTRIES/8);
      }
      else
      {
        char *rest;
        int idx;
        int status = -1;

        idx = (int)strtol(b, &rest, 10);
        if ((b != rest) || (idx < 0) || (idx >= KEYMAP_ENTRIES))
        {
          if (kbd_load_keyboard_position(&rest, map->normal, map->norm_sflag, idx) == 0)
          {
            if (kbd_load_keyboard_position(&rest, map->shifted, map->shift_sflag, idx) == 0)
            {
              status = 0;
            }
          }
        }
        if (status != 0)
          log_warning(LOG_DEFAULT, "kbd_load_keymap: parse error at line %d", linenr);
      }
    }
  }

  fclose(fp);

  return 0;
}


int kbd_dump_keymap(const char *filename, int number)
{
  FILE *fp;
  kbd_keymap_t *map;
  int i;
  int useIndex;
  const char *useFile;

  useIndex = (number < 0) ? keymap_index : number;

  if (useIndex >= ViceKeymap.total_number)
  {
    log_warning(LOG_DEFAULT, "Bad keymap index %d", useIndex);
    return -1;
  }

  map = &ViceKeymap.keymaps[useIndex];

  useFile = (filename == NULL) ? map->default_file : filename;

  log_message(LOG_DEFAULT, "Saving keyboard map to file '%s'", useFile);

  if ((fp = fopen(useFile, "w")) == NULL)
  {
    log_warning(LOG_DEFAULT, "Unable to open file '%s'", useFile);
    return -1;
  }

  fprintf(fp, "# VICE keyboard mapping file for RISC OS\n");
  fprintf(fp, "#\n");
  fprintf(fp, "# Different file format due to heavy dependency of Vice's on X11\n");
  fprintf(fp, "#\n");
  fprintf(fp, "# File format:\n");
  fprintf(fp, "# - comment lines start with '#'\n");
  fprintf(fp, "# - clear table using !CLEAR\n");
  fprintf(fp, "# - lineformat is keynum row col ['s' [srow scol ['s']]] [\"keystr\"]\n");
  fprintf(fp, "#   where 's' means activate shift key and srow/scol are keyboard\n");
  fprintf(fp, "#   positions for shifted keys (these should only be used if you want\n");
  fprintf(fp, "#   to map characters that need shift on your RISC OS machine to an\n");
  fprintf(fp, "#   unshifted position on the emulated machine (e.g. '*').\n");
  fprintf(fp, "#   Use row=15, col=15 or no specification to disable a key\n");
  fprintf(fp, "#\n");
  fprintf(fp, "# keynum is the internal key number (see RO3 PRM1-849). keystr is the\n");
  fprintf(fp, "# optional string representation for that key on an english keyboard.\n");
  fprintf(fp, "\n");
  fprintf(fp, "%s\n\n", keymap_clear_table);

  for (i=0; i<KEYMAP_ENTRIES; i++)
  {
    const char *str = IntKeyToString[i];

    if (str != NULL)
    {
      /* Output all keys, even unmapped ones; better for people who want to fiddle with
         the keymap file */
      fprintf(fp, "%3d", i);
      kbd_dump_keyboard_position(fp, map->normal, map->norm_sflag, i);
      if (map->shifted[i] != 0xff)
        kbd_dump_keyboard_position(fp, map->shifted, map->shift_sflag, i);
      fprintf(fp, " \"%s\"\n", str);
    }
  }
  fclose(fp);

  return 0;
}

void keyboard_register_column4080_key(key_ctrl_column4080_func_t func)
{
    key_ctrl_column4080_func = func;
}

void keyboard_register_caps_key(key_ctrl_caps_func_t func)
{
    key_ctrl_caps_func = func;
}

