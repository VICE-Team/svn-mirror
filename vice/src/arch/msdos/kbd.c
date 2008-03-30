/*
 * kbd.c - MS-DOS keyboard driver.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
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

#define _KBD_DOS_C

#include "vice.h"

#include <sys/farptr.h>
#include <pc.h>
#include <time.h>
#include <go32.h>
#include <dos.h>
#include <dpmi.h>
#include <stdlib.h>

#include "vmachine.h"
#include "kbd.h"
#include "interrupt.h"
#include "resources.h"
#include "vsync.h"
#include "mem.h"

/* #define DEBUG_KBD */

int keyarr[KBD_ROWS];
int rev_keyarr[KBD_COLS];

/* Joystick status. We use 3 elements to avoid `-1'.  */
BYTE joy[3] = { 0, 0, 0 };

/* This is nonzero if the user wants the menus.  */
int _escape_requested = 0;

/* Segment info for the custom keyboard handler.  */
static _go32_dpmi_seginfo std_kbd_handler_seginfo;

/* This takes account of the status of the modifier keys on the real PC
   keyboard.  */
static struct {
    unsigned int left_ctrl:1;
    unsigned int right_ctrl:1;
    unsigned int left_shift:1;
    unsigned int right_shift:1;
    unsigned int left_alt:1;
    unsigned int right_alt:1;
} modifiers;

/* Pointer to the keyboard conversion map.  */
static keyconv *keyconvmap;

/* What is the location of the virtual shift key in the keyboard matrix?  */
#if 0
static int virtual_shift_column = 0;
static int virtual_shift_row = 6;
#else
static int virtual_shift_column, virtual_shift_row;
#endif

#ifdef DEBUG_KBD
#define DEBUG(x)					\
    do {						\
        printf("%s, %d: ", __FUNCTION__, __LINE__);	\
        printf x;					\
        putchar('\n');					\
    } while (0)
#else
#define DEBUG(x)
#endif

/* ------------------------------------------------------------------------- */

/* These are the keyboard commands that cannot be handled within a keyboard
   interrupt.  They are dispatched via `kbd_flush_commands()'.  */

typedef enum {
    KCMD_RESET,
    KCMD_HARD_RESET,
    KCMD_RESTORE_PRESSED,
    KCMD_RESTORE_RELEASED
} kbd_command_t;

#define MAX_COMMAND_QUEUE_SIZE	256
static kbd_command_t command_queue[MAX_COMMAND_QUEUE_SIZE];
static int num_queued_commands;

/* Add a command to the queue.  */
static void queue_command(kbd_command_t c)
{
    if (num_queued_commands < MAX_COMMAND_QUEUE_SIZE)
	command_queue[num_queued_commands++] = c;
}
static void queue_command_end(void) { }

/* Dispatch all the pending keyboard commands.  */
void kbd_flush_commands(void)
{
    int i;

    if (num_queued_commands == 0)
	return;

    DEBUG(("%d command(s) in queue", num_queued_commands));

    for (i = 0; i < num_queued_commands; i++) {
	DEBUG(("Executing command #%d: %d", i, command_queue[i]));
	switch (command_queue[i]) {
	  case KCMD_HARD_RESET:
	    mem_powerup();
	    /* Fall through.  */
	  case KCMD_RESET:
	    suspend_speed_eval();
	    maincpu_trigger_reset();
	    break;

	  case KCMD_RESTORE_PRESSED:
	    maincpu_set_nmi(I_RESTORE, 1);
	    break;

	  case KCMD_RESTORE_RELEASED:
	    maincpu_set_nmi(I_RESTORE, 0);
	    break;

	  default:
	    fprintf(stderr, "Warning: Unknown keyboard command %d\n",
		    (int)command_queue[i]);
	}
    }
    num_queued_commands = 0;

    DEBUG(("Successful"));
}

/* ------------------------------------------------------------------------- */

/* The following functions handle the keyboard LEDs.  */

inline static void _led_set(unsigned char value)
{
    DEBUG(("Setting LED %s", value ? "on" : "off"));

    /* FIXME: Is this the 100% correct way to do it?  */
    outportb(0x60, 0xed);
    delay(1);
    outportb(0x60, value);
    delay(1);

    DEBUG(("Successful"));
}

void kbd_led_set(int status)
{
    _led_set(status ? 1 : 0);
}

/* ------------------------------------------------------------------------- */

inline static void set_keyarr(int row, int col, int value)
{
    if (row < 0 || col < 0)
        return;
    if (value) {
	keyarr[row] |= 1 << col;
	rev_keyarr[col] |= 1 << row;
    } else {
	keyarr[row] &= ~(1 << col);
	rev_keyarr[col] &= ~(1 << row);
    }
}

static void my_kbd_interrupt_handler(void)
{
    static int extended = 0;	/* Extended key count.  */
    unsigned int kcode;

    kcode = inportb(0x60);

    if (kcode == 0xe0) {
	/* Extended key: at the next interrupt we'll get its extended keycode
	   or 0xe0 again.  */
	extended++;
	outportb(0x20, 0x20);
	return;
    }

    if (!(kcode & 0x80)) {	/* Key pressed.  */

	/* Derive the extended keycode.  */
	if (extended == 1)
	    kcode = extended_key_tab[kcode];

	/* Handle modifiers.  */
	switch (kcode) {
	  case K_LEFTCTRL:
	    modifiers.left_ctrl = 1;
	    break;
	  case K_RIGHTCTRL:
	    modifiers.right_ctrl = 1;
	    break;
	  case K_LEFTSHIFT:
	    modifiers.left_shift = 1;
	    break;
	  case K_RIGHTSHIFT:
	    modifiers.right_shift = 1;
	    break;
	  case K_LEFTALT:
	    modifiers.left_alt = 1;
	    break;
	  case K_RIGHTALT:
	    modifiers.right_alt = 1;
	    break;
	}

	switch (kcode) {
	  case K_ESC:
	    _escape_requested = 1;
	    break;
#ifndef PET
	  case K_PGUP: /* Restore */
	    queue_command(KCMD_RESTORE_PRESSED);
	    break;
#endif
	  case K_F12:
	    /* F12 does a reset, Ctrl-F12 does a reset and clears the
               memory.  */
	    if (modifiers.left_ctrl || modifiers.right_ctrl)
		queue_command(KCMD_HARD_RESET);
	    else
		queue_command(KCMD_RESET);
	    break;
	  default:
	    if (!joystick_handle_key(kcode, 1)) {
		set_keyarr(keyconvmap[kcode].row, keyconvmap[kcode].column, 1);
		if (keyconvmap[kcode].vshift)
		    set_keyarr(virtual_shift_row, virtual_shift_column, 1);
	    }
	}

    } else {			/* Key released.  */

	/* Remove release bit.  */
	kcode &= 0x7F;

	/* Derive the extended keycode.  */
	if (extended == 1)
	    kcode = extended_key_tab[kcode];

	/* Handle modifiers.  */
	switch (kcode) {
	  case K_LEFTCTRL:
	    modifiers.left_ctrl = 0;
	    break;
	  case K_RIGHTCTRL:
	    modifiers.right_ctrl = 0;
	    break;
	  case K_LEFTSHIFT:
	    modifiers.left_shift = 0;
	    break;
	  case K_RIGHTSHIFT:
	    modifiers.right_shift = 0;
	    break;
	  case K_LEFTALT:
	    modifiers.left_alt = 0;
	    break;
	  case K_RIGHTALT:
	    modifiers.right_alt = 0;
	    break;
	}

	switch (kcode) {
          case K_ESC:
            break;
#ifndef PET
	  case K_PGUP:
	    queue_command(KCMD_RESTORE_RELEASED);
	    break;
#endif
	  default:
	    if (!joystick_handle_key(kcode, 0)) {
		set_keyarr(keyconvmap[kcode].row, keyconvmap[kcode].column, 0);
		if (keyconvmap[kcode].vshift)
		    set_keyarr(virtual_shift_row, virtual_shift_column, 0);
	    }
	}

    }

    extended = 0;
    outportb(0x20, 0x20);
}

static void my_kbd_interrupt_handler_end() { }

/* ------------------------------------------------------------------------- */

/* Install our custom keyboard interrupt.  */
void kbd_install(void)
{
    int r;
    static _go32_dpmi_seginfo my_kbd_handler_seginfo;

    DEBUG(("Allocating IRET wrapper for custom keyboard handler"));
    my_kbd_handler_seginfo.pm_offset = (int) my_kbd_interrupt_handler;
    my_kbd_handler_seginfo.pm_selector = _go32_my_cs();
    r = _go32_dpmi_allocate_iret_wrapper(&my_kbd_handler_seginfo);
    if (r) {
	fprintf(stderr,
	   "Cannot allocate IRET wrapper for the keyboard interrupt.\n");
	exit(-1);
    }
    DEBUG(("Installing custom keyboard handler in interrupt 9"));
    r = _go32_dpmi_set_protected_mode_interrupt_vector(9, &my_kbd_handler_seginfo);
    if (r) {
	fprintf(stderr, "Cannot install the keyboard interrupt handler.\n");
	exit(-1);
    }

    DEBUG(("Initializing keyboard data"));

    /* Initialize the keyboard matrix.  */
    memset(keyarr, 0, sizeof(keyarr));
    memset(rev_keyarr, 0, sizeof(rev_keyarr));
    /* Reset modifier status.  */
    memset(&modifiers, 0, sizeof(modifiers));

    DEBUG(("Successful"));
}

/* Restore the standard keyboard interrupt.  */
void kbd_uninstall(void)
{
    int r;

    DEBUG(("Restoring standard keyboard interrupt vector"));

    r = _go32_dpmi_set_protected_mode_interrupt_vector(9, &std_kbd_handler_seginfo);
    if (r)
	fprintf(stderr, "Aaargh! Couldn't restore the standard kbd interrupt vector!\n");
    else
	DEBUG(("Successful"));
}

static void kbd_exit(void)
{
    DEBUG(("Calling `kbd_uninstall()'"));
    kbd_uninstall();
}

/* Initialize the keyboard driver.  */
int kbd_init(keyconv *map, int sizeof_map,
	     int shift_column, int shift_row)
{
    DEBUG(("Getting standard int 9 seginfo"));
    _go32_dpmi_get_protected_mode_interrupt_vector(9, &std_kbd_handler_seginfo);
    atexit(kbd_exit);

    DEBUG(("Locking custom keyboard handler code"));
    _go32_dpmi_lock_code(my_kbd_interrupt_handler, (unsigned long)my_kbd_interrupt_handler_end - (unsigned long)my_kbd_interrupt_handler);
    _go32_dpmi_lock_code(queue_command, (unsigned long)queue_command_end - (unsigned long)queue_command);

    DEBUG(("Locking keyboard handler variables"));
    _go32_dpmi_lock_data(map, sizeof_map);
    _go32_dpmi_lock_data(keyconvmap, sizeof(keyconvmap));
    _go32_dpmi_lock_data(keyarr, sizeof(keyarr));
    _go32_dpmi_lock_data(rev_keyarr, sizeof(rev_keyarr));
    _go32_dpmi_lock_data(joy, sizeof(joy));
    _go32_dpmi_lock_data(&_escape_requested, sizeof(_escape_requested));
    _go32_dpmi_lock_data(&modifiers, sizeof(modifiers));
    _go32_dpmi_lock_data(&virtual_shift_row, sizeof(virtual_shift_row));
    _go32_dpmi_lock_data(&virtual_shift_column, sizeof(virtual_shift_column));
    _go32_dpmi_lock_data(&num_queued_commands, sizeof(num_queued_commands));
    _go32_dpmi_lock_data(&command_queue, sizeof(command_queue));

    kbd_install();
    num_queued_commands = 0;

    keyconvmap = map;
    virtual_shift_row = shift_row;
    virtual_shift_column = shift_column;

    DEBUG(("Successful"));

    return 0;
}
