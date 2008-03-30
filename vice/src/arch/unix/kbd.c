/*
 * kbd.c - X11 keyboard driver.
 *
 * Written by
 *  Ettore Perazzoli   (ettore@comm2000.it)
 *  Jouko Valta        (jopi@stekt.oulu.fi)
 *  André Fachat       (fachat@physik.tu-chemnitz.de)
 *  Bernhard Kuhn      (kuhn@eikon.e-technik.tu-muenchen.de)
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README file for copyright notice.
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

/* X11 keyboard driver. */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "vmachine.h"
#include "video.h"
#include "ui.h"

#include "resources.h"
#include "memutils.h"

#include "kbd.h"
#include "kbdef.h"
#include "interrupt.h"
#include "findpath.h"

#ifdef HAS_JOYSTICK
#include "joystick.h"
#endif

/* -------------------------------------------------------------------------- */

/* #define DEBUG_KBD */

/* Keyboard array. */
int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];

/* Joystick status */

BYTE joy[3] = { 0, 0, 0 };

/* Shift status */
static short kbd_lshiftrow = 0;
static short kbd_lshiftcol = 0;
static short kbd_rshiftrow = 0;
static short kbd_rshiftcol = 0;

/* Each element in this array is set to 0 if the corresponding key in the
   numeric keypad is pressed. */
#if 0
static int numpad_status[10];
#else

static int joy1pad_status[10];
static int joy2pad_status[10];

static int joypad_bits[10] = {
	0x10, 0x06, 0x02, 0x0a, 0x04, 0x00, 0x08, 0x05, 0x01, 0x09
};

#endif

static int left_shift_down, right_shift_down, virtual_shift_down;


/* -------------------------------------------------------------------------- */

/* Joystick emulation via numeric keypad. VIC20 has one single joystick
   port. */
#if 0

static int handle_joy_emu(KeySym key, int event_type)
{
    int do_port_1, do_port_2;
    int value;
    int num;

#ifdef HAS_JOYSTICK
    do_port_1 = (app_resources.joyDevice1 == 5);
    do_port_2 = (app_resources.joyDevice2 == 5);
#else
    do_port_1 = (app_resources.joyPort == 1);
    do_port_2 = (app_resources.joyPort == 2);
#endif

    /* Check whether this is some joystick-related event.  The `XK_KP*'
       cursor movement keysyms are not present in X11R4, so we use them only
       if available.  */
    switch (event_type) {

      case KeyPress:
	switch (key) {

	  case XK_KP_7:
#ifdef XK_KP_Home
	  case XK_KP_Home:
#endif
	    value = 0x5; num = 7;
	    break;

	  case XK_KP_8:
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    value = 0x1; num = 8;
	    break;

	  case XK_KP_9:
#ifdef XK_KP_Page_Up
	  case XK_KP_Page_Up:
#endif
	    value = 0x9; num = 9;
	    break;

	  case XK_KP_4:
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
	    value = 0x4; num = 4;
	    break;

	  case XK_KP_6:
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
	    value = 0x8; num = 6;
	    break;

	  case XK_KP_1:
#ifdef XK_KP_End
	  case XK_KP_End:
#endif
	    value = 0x6; num = 1;
	    break;

	  case XK_KP_2:
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    value = 0x2; num = 2;
	    break;

	  case XK_KP_3:
#ifdef XK_KP_Page_Down
	  case XK_KP_Page_Down:
#endif
	    value = 0xa; num = 3;
	    break;

	  case XK_KP_0:
	  case XK_Control_R:
#ifdef XK_KP_Insert
	  case XK_KP_Insert:
#endif
	    value = 0x10; num = 0;
	    break;

	  default:
	    return 0;
	}
	if (do_port_1)
	    joy[1] |= value;
	if (do_port_2)
	    joy[2] |= value;
	numpad_status[num] = 1;
	break;			/* KeyPress */

      case KeyRelease:
	switch (key) {
	  case XK_KP_7:
#ifdef XK_KP_Home
	  case XK_KP_Home:
#endif
	    value = ((numpad_status[8] ? 0 : 0x1)
		     | (numpad_status[4] ? 0 : 0x4));
	    num = 7;
	    break;

	  case XK_KP_8:
#ifdef XK_KP_Up
	  case XK_KP_Up:
#endif
	    value = 0x1; num = 8;
	    break;

	  case XK_KP_9:
#ifdef XK_KP_Page_Up
	  case XK_KP_Page_Up:
#endif
	    value = ((numpad_status[8] ? 0 : 0x1)
		     | (numpad_status[6] ? 0 : 0x8));
	    num = 9;
	    break;

	  case XK_KP_4:
#ifdef XK_KP_Left
	  case XK_KP_Left:
#endif
	    value = 0x4; num = 4;
	    break;

	  case XK_KP_6:
#ifdef XK_KP_Right
	  case XK_KP_Right:
#endif
	    value = 0x8; num = 6;
	    break;

	  case XK_KP_1:
#ifdef XK_KP_End
	  case XK_KP_End:
#endif
	    value = ((numpad_status[2] ? 0 : 0x2)
		     | (numpad_status[4] ? 0 : 0x4));
	    num = 1;
	    break;

	  case XK_KP_2:
#ifdef XK_KP_Down
	  case XK_KP_Down:
#endif
	    value = 0x2; num = 2;
	    break;

	  case XK_KP_3:
#ifdef XK_KP_Page_Down
	  case XK_KP_Page_Down:
#endif
	    value = ((numpad_status[2] ? 0 : 0x2)
		     | (numpad_status[6] ? 0 : 0x8));
	    num = 3;
	    break;

	  case XK_KP_0:
	  case XK_Control_R:
#ifdef XK_KP_Insert
	  case XK_KP_Insert:
#endif
	    value = 0x10; num = 0;
	    break;

	  default:
	    return 0;
	}
	if (do_port_1)
	    joy[1] &= ~value;
	if (do_port_2)
	    joy[2] &= ~value;
	numpad_status[num] = 0;
	break;			/* KeyRelease */
    }

    return 0;
}
#endif

static inline void set_keyarr(int row, int col, int value)
{
    if (value) {
#ifdef DEBUG_KBD
	printf("Set keyarr row %d col %d\n", row, col);
#endif
	keyarr[row] |= 1 << col;
	rev_keyarr[col] |= 1 << row;
    } else {
#ifdef DEBUG_KBD
	printf("Unset keyarr row %d col %d\n", row, col);
#endif
	keyarr[row] &= ~(1 << col);
	rev_keyarr[col] &= ~(1 << row);
    }
}

int joyreleaseval(int column, int *status) {
    int val = 0;
    switch (column) {
    case 1:
	val = ((status[2] ? 0 : 0x2) | (status[4] ? 0 : 0x4));
	break;
    case 3:
	val = ((status[2] ? 0 : 0x2) | (status[6] ? 0 : 0x8));
	break;
    case 7:
	val = ((status[4] ? 0 : 0x4) | (status[8] ? 0 : 0x1));
	break;
    case 9:
	val = ((status[8] ? 0 : 0x1) | (status[6] ? 0 : 0x8));
	break;
    default:
	val = joypad_bits[column];
	break;
    }
    return ~val;
}

void kbd_event_handler(Widget w, XtPointer client_data, XEvent *report,
		       Boolean *ctd)
{
    static char buffer[20];
    KeySym key;
    XComposeStatus compose;
    int count, i;

    count = XLookupString(&report->xkey, buffer, 20, &key, &compose);

#if 0
#if defined(PET)
    if (app_resources.numpadJoystick && handle_joy_emu(key, report->type))
#else
    if (handle_joy_emu(key, report->type))
#endif
	return;
#endif

#ifndef PET
    if (key == XK_Tab) {	/* Restore */
	if (report->type == KeyPress)
	    maincpu_set_nmi(I_RESTORE, 1);
	else if (report->type == KeyRelease)
	    maincpu_set_nmi(I_RESTORE, 0);
	return;
    }
#endif

    switch (report->type) {

      case KeyPress:
	for (i = 0; keyconvmap[i].sym != 0; ++i) {
	    if (key == keyconvmap[i].sym) {
		int row = keyconvmap[i].row;
		int column = keyconvmap[i].column;

		if(app_resources.numpadJoystick && row<0) {
		  if(row==-1) {
		    if(joypad_bits[column]) {
		      joy[1] |= joypad_bits[column];
		      joy1pad_status[column]=1;
		    } else {
		      joy[1] = 0;
	              memset(joy1pad_status, 0, sizeof(joy1pad_status));
		    }
		  } else
		  if(row==-2) {
		    if(joypad_bits[column]) {
		      joy[2] |= joypad_bits[column];
		      joy2pad_status[column]=1;
		    } else {
		      joy[2] = 0;
	              memset(joy2pad_status, 0, sizeof(joy2pad_status));
		    }
		  }
		  break;
		} 
		if(row>=0) {
		  set_keyarr(row, column, 1);

		  if (keyconvmap[i].shift == NO_SHIFT) {
		    set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
		    set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
		  } else {
		    if (keyconvmap[i].shift & VIRTUAL_SHIFT)
			virtual_shift_down++;
		    if (keyconvmap[i].shift & LEFT_SHIFT)
			left_shift_down++;
		    if (left_shift_down + virtual_shift_down > 0)
			set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 1);
		    if (keyconvmap[i].shift & RIGHT_SHIFT)
			right_shift_down++;
		    if (right_shift_down > 0)
			set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 1);
		  }
		  break;
		}
	    }
	}
	break;			/* KeyPress */

      case KeyRelease:
	if (IsModifierKey(key)) {
	    /* FIXME: This is a dirty kludge.  X11 can sometimes give the
               KeyPress event with the shifted KeySym, and the KeyRelease one
               with the same KeySym unshifted, so we loose control of what
               has been pressed and what has been released (all KeySyms are
               handled independently here).  For example, if the user does
               <Press Shift> <Press 1> <Release Shift> <Release 1>, we get
               <KeyPress Shift>, <KeyPress !>, <KeyRelease Shift>,
               <KeyRelease 1>.  To avoid disasters, we reset all the keyboard
               when a modifier has been released, but this heavily simplifies
               the behavior of multiple keys.  Does anybody know a way to
               avoid this X11 oddity? */
	    memset(keyarr, 0, sizeof(keyarr));
	    memset(rev_keyarr, 0, sizeof(rev_keyarr));
	    virtual_shift_down = 0;
	    /* TODO: do we have to cleanup joypads here too? */
	}

	for (i = 0; keyconvmap[i].sym != 0; i++) {
	    if (key == keyconvmap[i].sym) {
		int row = keyconvmap[i].row;
		int column = keyconvmap[i].column;

		if(row<0) {
		  if(row==-1) {
		    joy[1] &= joyreleaseval(column, joy1pad_status);
		    joy1pad_status[column] = 0;
		  } else
		  if(row==-2) {
		    joy[2] &= joyreleaseval(column, joy2pad_status);
		    joy2pad_status[column] = 0;
		  }
		} else {
		  set_keyarr(row, column, 0);
		  if (keyconvmap[i].shift & VIRTUAL_SHIFT)
		    virtual_shift_down--;
		  if (keyconvmap[i].shift & LEFT_SHIFT)
		    left_shift_down--;
		  if (keyconvmap[i].shift & RIGHT_SHIFT)
		    right_shift_down--;
		}
	    }
	}

	/* Map shift keys. */
	if (right_shift_down > 0)
	    set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 1);
	else
	    set_keyarr(kbd_rshiftrow, kbd_rshiftcol, 0);
	if (left_shift_down + virtual_shift_down > 0)
	    set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 1);
	else
	    set_keyarr(kbd_lshiftrow, kbd_lshiftcol, 0);
	break;			/* KeyRelease */

      case LeaveNotify:
	/* Clean up. */
	memset(keyarr, 0, sizeof(keyarr));
	memset(rev_keyarr, 0, sizeof(rev_keyarr));
	memset(joy, 0, sizeof(joy));
	virtual_shift_down = left_shift_down = right_shift_down = 0;
	memset(joy1pad_status, 0, sizeof(joy1pad_status));
	memset(joy2pad_status, 0, sizeof(joy2pad_status));
	break;			/* LeaveNotify */

      default:
	break;

    }				/* switch */
}

/* ------------------------------------------------------------------------- */
/*
 * Handling of different keyboard mappings
 *
 */

int keyc_mem = 0;   /* memory size of array in sizeof(keyconv), 0 = static */
int keyc_num = 0;   /* number of convs used in sizeof(keyconv) */

void kbd_init()
{
    /* In the X11 version, the keyboard handler is a callback
       function for the emulation window. We do only load the keymap
       file here and sort it into the keymap. */

    app_resources.numpadJoystick = 1;

    if(!app_resources.keymapFile) {
	resources_set_string(&app_resources.keymapFile, "vice.vkm");
    }

#if 0
    /* set40key() / set80key() is called before this */
#ifndef PET
    keyc_mem = 0;
    keyconvmap = default_keyconvmap;
    for(keyc_num=0;keyconvmap[keyc_num].sym;keyc_num++);

    kbd_lshiftrow = 1;
    kbd_lshiftcol = COL7;
    kbd_rshiftrow = 6;
    kbd_rshiftcol = 4;
#endif
#endif

fprintf(stderr,"kbd_init(): keymapFile=%s\n",app_resources.keymapFile);

    if(app_resources.keymapFile) {
	kbd_load_keymap(app_resources.keymapFile);
    } else {
	fprintf(stderr,"Couldn't load keymap file '%s', aborting!\n",
						app_resources.keymapFile);
	exit(1);
    }
}

/* Select between different PET keyboards. */
#ifdef PET

void set80key(void)
{

    if(!app_resources.keymapFile) {
	resources_set_string(&app_resources.keymapFile, "busi_uk.vkm");
    }
#if 0
    if(keyconvmap) {
	if(keyc_mem) free(keyconvmap);
    }
    keyc_mem = 0;
    keyconvmap = pet80map;
    for(keyc_num=0;keyconvmap[keyc_num].sym;keyc_num++);

    kbd_lshiftrow = 6;
    kbd_lshiftcol = 0;
    kbd_rshiftrow = 6;
    kbd_rshiftcol = 6;
#endif
}

void set40key(void)
{
    if(!app_resources.keymapFile) {
	resources_set_string(&app_resources.keymapFile, "graphics.vkm");
    }
#if 0
    if(keyconvmap) {
	if(keyc_mem) free(keyconvmap);
    }
    keyc_mem = 0;
    keyconvmap = pet40map;
    for(keyc_num=0;keyconvmap[keyc_num].sym;keyc_num++);

    kbd_lshiftrow = 8;
    kbd_lshiftcol = 0;
    kbd_rshiftrow = 8;
    kbd_rshiftcol = 5;
#endif
}

#endif /* PET */


static void kbd_parse_keymap(const char *filename);

static void kbd_parse_keyword(char *buffer) {
    char *key, *p;
    KeySym sym;
    int i;

    key = strtok(buffer+1, " \t:");
    if(!strcmp(key, "LSHIFT")) {
	p = strtok(NULL, " \t,");
	if(p) {
	  kbd_lshiftrow = atoi(p);
	  p = strtok(NULL, " \t,");
	  if(p) {
	    kbd_lshiftcol = atoi(p);
	  }
	}
    } else
    if(!strcmp(key, "RSHIFT")) {
	p = strtok(NULL, " \t,");
	if(p) {
	  kbd_rshiftrow = atoi(p);
	  p = strtok(NULL, " \t,");
	  if(p) {
	    kbd_rshiftcol = atoi(p);
	  }
	}
    } else
    if(!strcmp(key, "CLEAR")) {
	keyc_num = 0;
	keyconvmap[0].sym = 0;
    } else
    if(!strcmp(key, "INCLUDE")) {
	key = strtok(NULL, " \t");
	kbd_parse_keymap(key);
    } else
    if(!strcmp(key, "UNDEF")) {
	key = strtok(NULL, " \t");
	sym = XStringToKeysym(key);
	if(sym != NoSymbol) {
	  for(i=0;i<keyc_num;i++) {
	    if(keyconvmap[i].sym == sym) {
	      if(keyc_num) {
		keyconvmap[i] = keyconvmap[--keyc_num];
	      }
	      keyconvmap[keyc_num].sym = 0;
	      break;
	    }
	  }
	}
    }
    memset(joy1pad_status, 0, sizeof(joy1pad_status));
    memset(joy1pad_status, 0, sizeof(joy2pad_status));
}

static void kbd_parse_entry(char *buffer) {
    char *key, *p;
    KeySym sym;
    int row, col, shift, i;
    keyconv *kp;

    key = strtok(buffer, " \t:");
    sym = XStringToKeysym(key);
    if(sym==NoSymbol) {
	printf("Did not find KeySym '%s'!\n", key);
    } else {
	p = strtok(NULL, " \t,");
	if(p) {
	  row = strtol(p,NULL,10);
	  p = strtok(NULL, " \t,");
	  if(p) {
	    col = atoi(p);
	    p = strtok(NULL, " \t");
	    if(p || row<0) {
	      if(p) { shift = atoi(p); }
	      if(row>=0 || (row>=-2 && col>=0 && col<10)) {
	       for(i=0;keyconvmap[i].sym;i++) {
		if(sym == keyconvmap[i].sym) {
		  keyconvmap[i].row = row;
		  keyconvmap[i].column = col;
		  keyconvmap[i].shift = shift;
		  break;
		}
	       }

	       /* not in table -> add */
	       if(i>=keyc_num) {
	        /* table too small -> realloc */
		if(keyc_num>=keyc_mem) {
		  i = keyc_mem * 1.5;
		  kp= realloc(keyconvmap, (i+1) * sizeof(keyconv));
		  if(kp) {
		    keyconvmap = kp;
		    keyc_mem = i;
		  } else {
		    fprintf(stderr, "Couldn't realloc keymap table!\n");
		  }
		}
		if(keyc_num<keyc_mem) {
		  keyconvmap[keyc_num].sym = sym;
		  keyconvmap[keyc_num].row = row;
		  keyconvmap[keyc_num].column = col;
		  keyconvmap[keyc_num].shift = shift;
		  keyconvmap[++keyc_num].sym = 0;
		}
	       }
	      } else {
	       fprintf(stderr, "Bad row/column value (%d/%d) for keysym %s\n",
			row, col, key);
	      }
	    }
	  }
	}
    }
}

static void kbd_parse_keymap(const char *filename)
{
    FILE *fp;
    char *complete_path;
    char buffer[1000];

    printf("kbd_parse_keymap() called, file='%s'\n",
           filename ? filename : "(null)");

    if (!filename)
        return;

    complete_path = findpath(filename, app_resources.directory, R_OK);
    fp = fopen(complete_path, "r");

    if(!fp) {
        perror(complete_path);
    } else {
	do {
	  buffer[0] = 0;
	  if(fgets(buffer, 999, fp)) {
	    buffer[strlen(buffer)-1] = 0;
	    switch(*buffer) {
	    case 0:
	    case '#':
		break;
	    case '!':
		/* keyword handling */
		kbd_parse_keyword(buffer);
		break;
	    default:
		/* table entry handling */
		kbd_parse_entry(buffer);
		break;
	    }
	  }
	} while(!feof(fp));
	fclose(fp);
    }

    free(complete_path);
}


void kbd_load_keymap(const char *filename)
{
    keyconv *p;

    if(!filename) return;

    /* dynamicalize keymap table */
    if(!keyc_mem) {
      if(keyconvmap) {
	p = malloc((keyc_num+1) * sizeof(keyconv));
	if(!p) {
	  fprintf(stderr,"Couldn't alloc memory for keytable!\n");
	  return;
	} else {
	  memcpy(p, keyconvmap, (keyc_num+1) * sizeof(keyconv));
	  keyc_mem = keyc_num;
	  keyconvmap = p;
	}
      } else {
	keyconvmap = malloc((151) * sizeof(keyconv));
	keyc_num = 0;
	keyc_mem = 150;
	keyconvmap[0].sym = 0;
      }
    }
    if(keyc_mem) {
      resources_set_string(&app_resources.keymapFile,filename);
      kbd_parse_keymap(filename);
    }
}

int kbd_dump_keymap(const char *filename)
{
    FILE *fp;
    int i;

    if(!filename) return -1;
printf("kbd_dump_keymap() called, keyb file='%s'\n", filename);

    fp = fopen(filename, "w");

    if(!fp) {
        return -1;
    } else {
      fprintf(fp, "# VICE keyboard mapping file\n"
		  "#\n"
		  "# A Keyboard map is read in as patch to the current map.\n"
		  "#\n"
      		  "# File format:\n"
		  "# - comment lines start with '#'\n"
		  "# - keyword lines start with '!keyword'\n"
		  "# - normal line has 'keysym row column shiftflag'\n"
		  "#\n"
		  "# keywords and their lines are:\n"
/*		  "# '!CLEAR'               clear whole table\n" */
		  "# '!INCLUDE filename'    read file as mapping file\n"
		  "# '!LSHIFT row col'      left shift keyboard row/column\n"
		  "# '!RSHIFT row col'      right shift keyboard row/column\n"
		  "# '!UNDEF keysym'        remove keysym from table\n"
		  "#\n"
		  "# shiftflag can have the values:\n"
		  "# 0      key is not shifted for this keysym\n"
		  "# 1      key is shifted for this keysym\n"
		  "# 2      left shift\n"
		  "# 4      right shift\n"
		  "# 8      key can be shifted or not with this keysym\n"
		  "#\n"
      );
      fprintf(fp, "!CLEAR\n");
      fprintf(fp, "!LSHIFT %d %d\n", kbd_lshiftrow, kbd_lshiftcol);
      fprintf(fp, "!RSHIFT %d %d\n", kbd_rshiftrow, kbd_rshiftcol);
      for(i=0;keyconvmap[i].sym;i++) {
	fprintf(fp, "%s %d %d %d\n",
	  XKeysymToString(keyconvmap[i].sym),
		keyconvmap[i].row, keyconvmap[i].column, keyconvmap[i].shift);
      }
      fclose(fp);
    }
    return 0;
}

