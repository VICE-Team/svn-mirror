/*
 * uicommands.c - Implementation of common UI commands.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "archdep.h"
#include "attach.h"
#include "autostart.h"
#include "fliplist.h"
#include "imagecontents.h"
#include "info.h"
#include "interrupt.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "mon.h"
#include "resources.h"
#include "sound.h"
#include "tape.h"
#include "types.h"
#include "uicommands.h"
#include "uiedisk.h"
#include "uisnapshot.h"
#include "utils.h"
#include "vsync.h"

/* ------------------------------------------------------------------------- */

static char *read_disk_image_contents(const char *name)
{
    image_contents_t *contents;
    char *s;

    contents = image_contents_read_disk(name);
    if (contents == NULL)
        return NULL;

    s = image_contents_to_string(contents);

    image_contents_destroy(contents);

    return s;
}

static UI_CALLBACK(attach_disk)
{
    int unit = (int)client_data;
    char *filename;
    char title[1024];
    ui_button_t button;

    suspend_speed_eval();
    sprintf(title, "Attach Disk Image as unit #%d", unit);
    filename = ui_select_file(title, read_disk_image_contents,
                              unit == 8 ? True : False, NULL,
                              "*.[gdxGDX]*", &button);

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

static UI_CALLBACK(attach_empty_disk)
{
    int unit = (int)client_data;
    char filename[1024];
    /* ui_button_t button; */

    suspend_speed_eval();

    if (ui_empty_disk_dialog(filename)) 
	return;

    if (file_system_attach_disk(unit, filename) < 0)
        ui_error("Invalid Disk Image");
}

static UI_CALLBACK(detach_disk)
{
    int unit = (int)client_data;

    suspend_speed_eval();
    file_system_detach_disk(unit);
}

/* ------------------------------------------------------------------------- */

static char *read_tape_image_contents(const char *name)
{
    image_contents_t *contents;
    char *s;

    contents = image_contents_read_tape(name);
    if (contents == NULL)
        return NULL;

    s = image_contents_to_string(contents);

    image_contents_destroy(contents);

    return s;
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
	if (tape_attach_image(filename) < 0)
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
    tape_detach_image();
}

/* ------------------------------------------------------------------------- */

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
	    && tape_attach_image(filename) < 0) {
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

/* ------------------------------------------------------------------------- */

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

#ifdef USE_VIDMODE_EXTENSION
    int fullscreen;
#endif

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
#ifdef USE_VIDMODE_EXTENSION
    if(fullscreen) ui_set_fullscreenmode();
#endif
}

static UI_CALLBACK(activate_monitor)
{
#ifdef USE_VIDMODE_EXTENSION
    fullscreen = ui_set_windowmode();
#endif
    suspend_speed_eval();
    ui_dispatch_events();		/* popdown the menu */
    ui_autorepeat_on();

    if (!ui_emulation_is_paused()) 
        maincpu_trigger_trap(mon_trap, (void *) 0);
    else
        mon_trap(MOS6510_REGS_GET_PC(&maincpu_regs), 0);
}

static UI_CALLBACK(run_c1541)
{
#ifdef USE_VIDMODE_EXTENSION
    ui_set_windowmode();
#endif
    suspend_speed_eval();
    sound_close();
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
    machine_powerup();
}

static UI_CALLBACK(browse_manual)
{
    if (_ui_resources.html_browser_command == NULL ||
	*_ui_resources.html_browser_command == '\0') {
	ui_error("No HTML browser is defined.");
    } else {
	/* FIXME: Argh.  Ugly!  */
#define BROWSE_CMD_BUF_MAX 16384
	char buf[BROWSE_CMD_BUF_MAX];
	static const char manual_path[] = DOCDIR "/vice_toc.html";
	char *res_ptr;
	int manual_path_len, cmd_len;

#ifdef USE_VIDMODE_EXTENSION
        ui_set_windowmode();
#endif
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

	log_debug("Executing `%s'...", buf);
	if (system(buf) != 0)
	    ui_error("Cannot run HTML browser.");
    }
}

static UI_CALLBACK(do_exit)
{
    ui_exit();
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(toggle_pause)
{
    if (call_data == NULL) {
        if (ui_emulation_is_paused()) {
            ui_pause_emulation(0);
        } else {			/* !paused */
            ui_menu_set_tick(w, 1);
            ui_pause_emulation(1);
        }
    }

    ui_menu_set_tick(w, ui_emulation_is_paused());
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
             "Copyright © 1996-1999 Ettore Perazzoli",
             "Copyright © 1996-1999 André Fachat",
             "Copyright © 1993-1994, 1997-1999 Teemu Rantanen",
             "Copyright © 1997-1999 Daniel Sladic",
             "Copyright © 1998-1999 Andreas Boose",
             "Copyright © 1998-1999 Tibor Biczo",
             "Copyright © 1993-1996 Jouko Valta",
             "Copyright © 1993-1994 Jarkko Sonninen",
#ifdef HAVE_RESID
             "reSID engine",
             "Copyright © 1999 Dag Lem",
#endif
             "",
	     "Official VICE homepage:",
	     "http://www.cs.cmu.edu/~dsladic/vice/vice.html",
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

/* Snapshot commands.  */

static void load_snapshot_trap(ADDRESS unused_addr, void *data)
{
    ui_button_t button;
    char *filename;

    if (data) {
        log_debug("Quickloading file %s.", (char *)data);
	filename = data;
    } else {
        filename = ui_select_file("Load snapshot", NULL, False, NULL,
                              "*", &button);
        if (button != UI_BUTTON_OK)
            return;
    }

    if (machine_read_snapshot(filename) < 0)
        ui_error("Cannot load snapshot file\n`%s'", filename);
    ui_update_menus();

    if (data) free(data);
}

static UI_CALLBACK(load_snapshot)
{
    if (!ui_emulation_is_paused())
        maincpu_trigger_trap(load_snapshot_trap, (void *) 0);
    else
        load_snapshot_trap(0, 0);
}

static UI_CALLBACK(load_quicksnap)
{
    char *fname = concat(archdep_home_path(), "/", VICEUSERDIR, "/",
	machine_name, ".vsf", NULL);

    if (!ui_emulation_is_paused())
        maincpu_trigger_trap(load_snapshot_trap, (void *) fname);
    else
        load_snapshot_trap(0, fname);
}

static void save_snapshot_trap(ADDRESS unused_addr, void *data)
{
    if (data) {
	/* quick snapshot, save ROMs & disks (??) */
        log_debug("Quicksaving file %s.", (char *)data);
	if (machine_write_snapshot(data, 1, 1) < 0) {
            ui_error("Cannot write snapshot file\n`%s'\n", data);
	}
	free(data);
    } else {
        ui_snapshot_dialog();
    }
}

static UI_CALLBACK(save_snapshot)
{
    maincpu_trigger_trap(save_snapshot_trap, (void *) 0);
}

static UI_CALLBACK(save_quicksnap)
{
    char *fname = concat(archdep_home_path(), "/", VICEUSERDIR, "/",
	machine_name, ".vsf", NULL);

    maincpu_trigger_trap(save_snapshot_trap, (void *) fname);
}

/*  fliplist commands */
extern char last_attached_images[NUM_DRIVES][256];
extern ui_drive_enable_t enabled_drives;
struct cb_data_t {
    int unit;
    long data;			/* should be enough for a pointer */
};

typedef enum { 
    CBD_NEXT, CBD_PREV, CBD_ADD, CBD_REMOVE 
} cbd_enum_t;

static UI_CALLBACK(attach_from_fliplist)
{
    flip_attach_head(((struct cb_data_t *) client_data)->unit, 
		     (int) ((struct cb_data_t *) client_data)->data);
}

static UI_CALLBACK(attach_from_fliplist2)
{
    file_system_attach_disk(flip_get_unit((void *) client_data), 
			    flip_get_image((void *) client_data));
}

static UI_CALLBACK(attach_from_fliplist3)
{
    flip_attach_head(8, (int) client_data);
}

static UI_CALLBACK(add2fliplist)
{
    flip_add_image(8);
    ui_update_flip_menus(8, 8);
}

static UI_CALLBACK(add2fliplist2)
{
    flip_set_current(((struct cb_data_t *) client_data)->unit, 
		     (char *) ((struct cb_data_t *) client_data)->data);
    flip_add_image(((struct cb_data_t *) client_data)->unit);
    ui_update_flip_menus(((struct cb_data_t *) client_data)->unit,
			 ((struct cb_data_t *) client_data)->unit);
}

static UI_CALLBACK(remove_from_fliplist)
{
    flip_remove(8, NULL);
    ui_update_flip_menus(8, 8);
}

static UI_CALLBACK(remove_from_fliplist2)
{
    flip_remove(((struct cb_data_t *) client_data)->unit, 
		(char *) ((struct cb_data_t *) client_data)->data);
    ui_update_flip_menus(((struct cb_data_t *) client_data)->unit,
			 ((struct cb_data_t *) client_data)->unit);
}

void ui_update_flip_menus(int from_unit, int to_unit)
{
    /* Yick, allocate dynamically */
    static ui_menu_entry_t flipmenu[NUM_DRIVES][256]; 
    static struct cb_data_t cb_data[NUM_DRIVES][sizeof(cbd_enum_t)];
    
    char *image = NULL, *t0 = NULL, *t1 = NULL, *t2 = NULL, *t3 = NULL;
    char *t4 = NULL, *dir;
    void *fl_iterator;
    int i, drive, true_emu;

    resources_get_value("DriveTrueEmulation", 
			(resource_value_t *) &true_emu);

    for (drive = from_unit - 8 ; 
	 (drive <= to_unit - 8) && (drive < NUM_DRIVES); 
	 drive++) {

	printf ("updating flip menu for drive %d\n", drive);

	i = 0;
	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	t0 = xmalloc(16);
	sprintf(t0, "Unit #%d", drive + 8);
	flipmenu[drive][i].string = t0;
	flipmenu[drive][i].callback = (ui_callback_t) attach_disk;
	flipmenu[drive][i].callback_data = (ui_callback_data_t) drive + 8;
	i++;

	/* don't update menu deeply when drive has not been enabled 
	   or nothing has been attached */
	if (true_emu) {
	    if (! (1 << drive) & enabled_drives)
		goto update_menu;
	} else {
	    if (strcmp(last_attached_images[drive], "") == 0)
		goto update_menu;
	}

	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	flipmenu[drive][i].string = "--";
	i++;
    
	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	fname_split(flip_get_next(drive + 8), &dir, &image);
	t1 = concat("Next: ", image ? image : "<empty>", NULL);
	flipmenu[drive][i].string = t1;
	flipmenu[drive][i].callback = (ui_callback_t) attach_from_fliplist;
	cb_data[drive][CBD_NEXT].unit = drive + 8;
	cb_data[drive][CBD_NEXT].data = 1;
	flipmenu[drive][i].callback_data = 
	    (ui_callback_data_t) &(cb_data[drive][CBD_NEXT]);
	i++;
    
	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	fname_split(flip_get_prev(drive + 8), &dir, &image);
	t2 = concat("Previous: ", image ? image : "<empty>", NULL);
	flipmenu[drive][i].string = t2;
	flipmenu[drive][i].callback = (ui_callback_t) attach_from_fliplist;
	cb_data[drive][CBD_PREV].unit = drive + 8;
	cb_data[drive][CBD_PREV].data = 0;
	flipmenu[drive][i].callback_data = 
	    (ui_callback_data_t) &(cb_data[drive][CBD_PREV]);
	i++;
    
	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	fname_split(last_attached_images[drive], &dir, &image);
	t3 = concat("Add: ", image, NULL);
	flipmenu[drive][i].string = t3;
	flipmenu[drive][i].callback = (ui_callback_t) add2fliplist2;
	cb_data[drive][CBD_ADD].unit = drive + 8;
	cb_data[drive][CBD_ADD].data = (long) last_attached_images[drive];
	flipmenu[drive][i].callback_data = 
	    (ui_callback_data_t) &(cb_data[drive][CBD_ADD]);
	i++;

	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	fname_split(last_attached_images[drive], &dir, &image);
	t4 = concat("Remove: ", image, NULL);
	flipmenu[drive][i].string = t4;
	flipmenu[drive][i].callback = (ui_callback_t) remove_from_fliplist2;
	cb_data[drive][CBD_REMOVE].unit = drive + 8;
	cb_data[drive][CBD_REMOVE].data = (long) last_attached_images[drive];
	flipmenu[drive][i].callback_data = 
	    (ui_callback_data_t) &(cb_data[drive][CBD_REMOVE]);
	i++;

	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	flipmenu[drive][i].string = "--";
	i++;
    
	/* Now collect current fliplist */
	fl_iterator=flip_init_iterate(drive + 8);
	while (fl_iterator) {
	    memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
	    fname_split(flip_get_image(fl_iterator), &dir, &image);
	    flipmenu[drive][i].string = image;
	    flipmenu[drive][i].callback = 
		(ui_callback_t) attach_from_fliplist2;
	    flipmenu[drive][i].callback_data = 
		(ui_callback_data_t) fl_iterator;

	    fl_iterator = flip_next_iterate(drive + 8);
	    i++;
	}

    update_menu:
	/* make sure the menu is well terminated */
	memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));

	/* ugly ... */
	if (drive == 0)
	    ui_set_drive8_menu(ui_menu_create("LeftDrive8Menu", 
					      flipmenu[drive], NULL));
	else
	    ui_set_drive9_menu(ui_menu_create("LeftDrive9Menu", 
					      flipmenu[drive], NULL));
	free(t0);
	if (i > 1) {
	    free(t1);
	    free(t2);
	    free(t3);
	    free(t4);
	}
    }
}

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t attach_empty_disk_image_submenu[] = {
    { "Unit #8...",
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 8, NULL, },
    { "Unit #9...",
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 9, NULL, },
    { "Unit #10...",
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 10, NULL, },
    { "Unit #11...",
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 11, NULL, },
    { NULL }
};

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

static ui_menu_entry_t flip_submenu[] = {
    { "Add current image",
      (ui_callback_t) add2fliplist, (ui_callback_data_t) 0, NULL,
      XK_i, UI_HOTMOD_META },
    { "Remove current image",
      (ui_callback_t) remove_from_fliplist, (ui_callback_data_t) 0, NULL,
      XK_k, UI_HOTMOD_META },
    { "Attach next image",
      (ui_callback_t) attach_from_fliplist3, (ui_callback_data_t) 1, NULL,
      XK_n, UI_HOTMOD_META },
    { "Attach previous image",
      (ui_callback_t) attach_from_fliplist3, (ui_callback_data_t) 0, NULL,
      XK_N, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_disk_commands_menu[] = {
    { "Attach a disk image",
      NULL, NULL, attach_disk_image_submenu },
    { "Create and attach an empty disk",
      NULL, NULL, attach_empty_disk_image_submenu },
    { "Detach disk image",
      NULL, NULL, detach_disk_image_submenu },
    { "Fliplist for drive #8",
      NULL, NULL, flip_submenu },
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

static ui_menu_entry_t ui_snapshot_commands_submenu[] = {
    { "Load snapshot...",
      (ui_callback_t) load_snapshot, NULL, NULL,
      XK_l, UI_HOTMOD_META },
    { "Save snapshot...",
      (ui_callback_t) save_snapshot, NULL, NULL,
      XK_s, UI_HOTMOD_META },
    { "--" },
    { "Quickload snapshot",
      (ui_callback_t) load_quicksnap, NULL, NULL,
      XK_F10, UI_HOTMOD_META },
    { "Quicksave snapshot",
      (ui_callback_t) save_quicksnap, NULL, NULL,
      XK_F11, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_snapshot_commands_menu[] = {
    { "Snapshot commands",
      NULL,  NULL, ui_snapshot_commands_submenu },
    { NULL }
};

ui_menu_entry_t ui_tool_commands_menu[] = {
    { "Activate monitor",
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
