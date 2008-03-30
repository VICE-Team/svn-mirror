/*
 * uicommands.c - Implementation of common UI commands.
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

#include "vice.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Command.h>

#include "uicommands.h"
#include "interrupt.h"
#include "vsync.h"
#include "vdrive.h"
#include "tapeunit.h"
#include "attach.h"
#include "autostart.h"
#include "mon.h"
#include "mem.h"
#include "info.h"
#include "resources.h"
#include "machine.h"
#include "utils.h"

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(attach_disk)
{
    int unit = (int)client_data;
    char *filename;
    char title[1024];
    ui_button_t button;

    suspend_speed_eval();
    sprintf(title, "Attach Disk Image as unit #%d", unit);
    filename = ui_select_file(title, read_disk_image_contents,
		      unit == 8 ? True : False, NULL, "*.[gdxGDX]64*", &button);

    switch (button) {
      case UI_BUTTON_OK:
 	if (file_system_attach_disk(unit, filename) < 0)
	    ui_error("Invalid Disk Image");
	break;
      case UI_BUTTON_AUTOSTART:
	if (autostart_disk(filename, NULL) < 0)
	    ui_error("Invalid Disk Image");
	break;
      default:
	/* Do nothing special.  */
        break;
    }
}

static UI_CALLBACK(detach_disk)
{
    int unit = (int)client_data;

    suspend_speed_eval();
    file_system_detach_disk(unit);
}

static UI_CALLBACK(attach_tape)
{
    char *filename;
    ui_button_t button;

    suspend_speed_eval();

    filename = ui_select_file("Attach a tape image", read_tape_image_contents,
			      True, NULL, NULL, &button);

    switch (button) {
      case UI_BUTTON_OK:
	if (serial_select_file(DT_TAPE, 1, filename) < 0)
	    ui_error("Invalid Tape Image");
	break;
      case UI_BUTTON_AUTOSTART:
	if (autostart_tape(filename, NULL) < 0)
	    ui_error("Invalid Tape Image");
	break;
      default:
	/* Do nothing special.  */
        break;
    }
}

static UI_CALLBACK(detach_tape)
{
    serial_remove(1);
}

static char *read_disk_or_tape_image_contents(const char *fname)
{
    char *tmp;

    tmp = read_disk_image_contents(fname);
    if (tmp)
	return tmp;
    return read_tape_image_contents(fname);
}

static UI_CALLBACK(smart_attach)
{
    char *filename;
    ui_button_t button;

    suspend_speed_eval();

    filename = ui_select_file("Smart-attach a file",
			      read_disk_or_tape_image_contents,
			      True, NULL, NULL, &button);

    switch (button) {
      case UI_BUTTON_OK:
 	if (file_system_attach_disk(8, filename) < 0
	    && serial_select_file(DT_TAPE, 1, filename) < 0) {
	    ui_error("Unknown image type");
	}
	break;
      case UI_BUTTON_AUTOSTART:
	if (autostart_autodetect(filename, NULL) < 0)
	    ui_error("Unknown image type");
	break;
      default:
	/* Do nothing special.  */
        break;
    }
}

static UI_CALLBACK(change_working_directory)
{
    PATH_VAR(wd);
    int path_max = GET_PATH_MAX;

    getcwd(wd, path_max);
    suspend_speed_eval();
    if (ui_input_string("VICE setting", "Change current working directory",
			wd, path_max) != UI_BUTTON_OK)
	return;
    else if (chdir(wd) < 0)
	ui_error("Directory not found");
}

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static UI_CALLBACK(activate_monitor)
{
    suspend_speed_eval();
    ui_dispatch_events();		/* popdown the menu */
    ui_autorepeat_on();
    maincpu_trigger_trap(mon_trap, (void *) 0);
}

static UI_CALLBACK(run_c1541)
{
    suspend_speed_eval();
    switch (system("xterm -sb -e c1541 &")) {
      case 127:
	ui_error("Couldn't run /bin/sh???");
	break;
      case -1:
	ui_error("Couldn't run xterm");
	break;
      case 0:
	break;
      default:
	ui_error("Unknown error while running c1541");
    }
}

static UI_CALLBACK(reset)
{
    suspend_speed_eval();
    maincpu_trigger_reset();
}

static UI_CALLBACK(powerup_reset)
{
    suspend_speed_eval();
    mem_powerup();
    maincpu_trigger_reset();
}

static UI_CALLBACK(browse_manual)
{
    if (_ui_resources.html_browser_command == NULL ||
	*_ui_resources.html_browser_command == '\0') {
	ui_error("No HTML browser is defined.");
    } else {
	/* Argh.  Ugly!  */
#define BROWSE_CMD_BUF_MAX 16384
	char buf[BROWSE_CMD_BUF_MAX];
	static const char manual_path[] = DOCDIR "/vice_toc.html";
	char *res_ptr;
	int manual_path_len, cmd_len;

	cmd_len = strlen(_ui_resources.html_browser_command);
	manual_path_len = strlen(manual_path);

	res_ptr = strstr(_ui_resources.html_browser_command, "%s");
	if (res_ptr == NULL) {
	    /* No substitution. */
	    if (cmd_len + 2 > BROWSE_CMD_BUF_MAX - 1) {
		ui_error("Browser command too long.");
		return;
	    }
	    sprintf(buf, "%s &", _ui_resources.html_browser_command);
	} else {
	    char *tmp_ptr, *cmd_ptr;
	    int offs;

	    /* Replace each occurrence of "%s" with the path of the HTML
               manual. */

	    cmd_len += manual_path_len - 2;
	    cmd_len += 2;	/* Trailing " &". */
	    if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
		ui_error("Browser command too long.");
		return;
	    }

	    offs = res_ptr - _ui_resources.html_browser_command;
	    memcpy(buf, _ui_resources.html_browser_command, offs);
	    strcpy(buf + offs, manual_path);
	    cmd_ptr = buf + offs + manual_path_len;
	    res_ptr += 2;

	    while ((tmp_ptr = strstr(res_ptr, "%s")) != NULL) {
		cmd_len += manual_path_len - 2;
		if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
		    ui_error("Browser command too long.");
		    return;
		}
		offs = tmp_ptr - res_ptr;
		memcpy(cmd_ptr, res_ptr, offs);
		strcpy(cmd_ptr + offs, manual_path);
		cmd_ptr += manual_path_len + offs;
		res_ptr = tmp_ptr + 2;
	    }

	    sprintf(cmd_ptr, "%s &", res_ptr);
	}

	printf("Executing `%s'...\n", buf);
	if (system(buf) != 0)
	    ui_error("Cannot run HTML browser.");
    }
}

static UI_CALLBACK(toggle_pause)
{
    static int paused;

    if (paused) {
	if (call_data == NULL) {
	    ui_display_paused(0);
	    paused = 0;
	}
    } else {			/* !paused */
	if (call_data == NULL) {
	    paused = 1;
	    ui_menu_set_tick(w, 1);
	    ui_display_paused(1);
	    suspend_speed_eval();
	    while (paused)
		ui_dispatch_next_event();
	}
    }
    ui_menu_set_tick(w, paused);
    /* ui_display_speed(0.0, 0.0, 0); */
}

static UI_CALLBACK(do_exit)
{
    ui_exit();
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(info_dialog_license_callback)
{
    ui_show_text("VICE is FREE software!", license_text, -1, -1);
}

static UI_CALLBACK(info_dialog_no_warranty_callback)
{
    ui_show_text("No warranty!", warranty_text, -1, -1);
}

static UI_CALLBACK(info_dialog_contrib_callback)
{
    ui_show_text("Contributors to the VICE project", contrib_text, -1, -1);
}

static UI_CALLBACK(info_dialog_close_callback)
{
    *((ui_button_t *)client_data) = 1;
}

static Widget build_info_dialog(Widget parent,
                                int *return_flag, ...)
{
    Widget shell, pane, info_form, button_form, tmp, prevlabel = NULL;
    va_list arglist;
    String str;

    shell = ui_create_transient_shell(parent, "infoDialogShell");
    pane = XtVaCreateManagedWidget
	("infoDialog", panedWidgetClass, shell, NULL);
    info_form = XtVaCreateManagedWidget
	("textForm", formWidgetClass, pane, NULL);
    button_form = XtVaCreateManagedWidget
	("buttonBox", boxWidgetClass, pane, XtNshowGrip, False,
	 XtNskipAdjust, True, XtNorientation, XtorientHorizontal, NULL);
    va_start(arglist, return_flag);
    while ((str = va_arg(arglist, String))) {
	tmp = XtVaCreateManagedWidget
	    ("infoString", labelWidgetClass, info_form,
	     XtNlabel, str, XtNjustify, XtJustifyCenter, XtNresize, False,
	     XtNwidth, 220, NULL);
	if (prevlabel)
	    XtVaSetValues(tmp, XtNfromVert, prevlabel, NULL);
	prevlabel = tmp;
    }
    va_end(arglist);
    tmp = XtVaCreateManagedWidget
	("closeButton", commandWidgetClass, button_form, NULL);
    XtAddCallback(tmp, XtNcallback,
		  info_dialog_close_callback, (XtPointer)return_flag);
    tmp = XtVaCreateManagedWidget
	("licenseButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, info_dialog_license_callback, NULL);
    tmp = XtVaCreateManagedWidget
	("noWarrantyButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, info_dialog_no_warranty_callback, NULL);
    tmp = XtVaCreateManagedWidget
	("contribButton", commandWidgetClass, button_form,
	 XtNfromHoriz, tmp, NULL);
    XtAddCallback(tmp, XtNcallback, info_dialog_contrib_callback, NULL);
    return pane;
}

static UI_CALLBACK(about)
{
    static Widget info_dialog;
    static int is_closed;

    if (!info_dialog) {
	info_dialog = build_info_dialog
	    (_ui_top_level, &is_closed,
	     "",
             "V I C E",
             "the Versatile Commodore Emulator",
             "",
	     "Version " VERSION,
#ifdef UNSTABLE
	     "(unstable)",
#endif
             "",
             "Copyright © 1996-1998 Ettore Perazzoli",
             "Copyright © 1996-1998 André Fachat",
             "Copyright © 1993-1994, 1997-1998 Teemu Rantanen",
             "Copyright © 1997-1998 Daniel Sladic",
             "Copyright © 1998 Andreas Boose",
             "Copyright © 1993-1996 Jouko Valta",
             "Copyright © 1993-1994 Jarkko Sonninen",
             "",
	     "Official VICE homepage:",
	     "http://www.tu-chemnitz.de/~fachat/vice/vice.html",
             "",
             NULL);
    }
    suspend_speed_eval();
    ui_popup(XtParent(info_dialog), "VICE Information", False);

    is_closed = 0;
    while (!is_closed)
	ui_dispatch_next_event();
    ui_popdown(XtParent(info_dialog));
}

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t attach_disk_image_submenu[] = {
    { "Unit #8...",
      (ui_callback_t) attach_disk, (ui_callback_data_t) 8, NULL,
      XK_8, UI_HOTMOD_META },
    { "Unit #9...",
      (ui_callback_t) attach_disk, (ui_callback_data_t) 9, NULL,
      XK_9, UI_HOTMOD_META },
    { "Unit #10...",
      (ui_callback_t) attach_disk, (ui_callback_data_t) 10, NULL,
      XK_0, UI_HOTMOD_META },
    { "Unit #11...",
      (ui_callback_t) attach_disk, (ui_callback_data_t) 11, NULL,
      XK_1, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t detach_disk_image_submenu[] = {
    { "Unit #8",
      (ui_callback_t) detach_disk, (ui_callback_data_t) 8, NULL },
    { "Unit #9",
      (ui_callback_t) detach_disk, (ui_callback_data_t) 9, NULL },
    { "Unit #10",
      (ui_callback_t) detach_disk, (ui_callback_data_t) 10, NULL },
    { "Unit #11",
      (ui_callback_t) detach_disk, (ui_callback_data_t) 11, NULL },
    { "--" },
    { "All",
      (ui_callback_t) detach_disk, (ui_callback_data_t) -1, NULL },
    { NULL }
};

static ui_menu_entry_t reset_submenu[] = {
    { "Soft",
      (ui_callback_t) reset, NULL, NULL },
    { "Hard",
      (ui_callback_t) powerup_reset, NULL, NULL,
      XK_F12, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_disk_commands_menu[] = {
    { "Attach a disk image",
      NULL, NULL, attach_disk_image_submenu },
    { "Detach disk image",
      NULL, NULL, detach_disk_image_submenu },
    { NULL }
};

ui_menu_entry_t ui_tape_commands_menu[] = {
    { "Attach a tape image...",
      (ui_callback_t) attach_tape, NULL, NULL,
      XK_t, UI_HOTMOD_META},
    { "Detach tape image",
      (ui_callback_t) detach_tape, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_smart_attach_commands_menu[] = {
    { "Smart-attach disk/tape...",
      (ui_callback_t) smart_attach, NULL, NULL,
      XK_a, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_directory_commands_menu[] = {
    { "Change working directory...",
      (ui_callback_t) change_working_directory, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_tool_commands_menu[] = {
    { "Activate the Monitor",
      (ui_callback_t) activate_monitor, NULL, NULL },
    { "Run C1541",
      (ui_callback_t) run_c1541, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_help_commands_menu[] = {
    { "Browse manuals",
      (ui_callback_t) browse_manual, NULL, NULL },
    { "About VICE...",
      (ui_callback_t) about, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_run_commands_menu[] = {
    { "Reset",
      NULL, NULL, reset_submenu },
    { "*Pause",
      (ui_callback_t) toggle_pause, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_exit_commands_menu[] = {
    { "Exit emulator",
      (ui_callback_t) do_exit, NULL, NULL },
    { NULL }
};
