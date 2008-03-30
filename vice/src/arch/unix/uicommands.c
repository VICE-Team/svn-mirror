/*
 * uicommands.c - Implementation of common UI commands.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "archdep.h"
#include "attach.h"
#include "autostart.h"
#include "fliplist.h"
#include "imagecontents.h"
#include "info.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "maincpu.h"
#include "mem.h"
#include "mon.h"
#include "mos6510.h"
#include "resources.h"
#include "sound.h"
#include "tape.h"
#include "types.h"
#include "uicommands.h"
#include "uiedisk.h"
#include "uimenu.h"
#include "uisettings.h"
#include "uisnapshot.h"
#include "uidrive.h"
#include "utils.h"
#include "vsync.h"
#ifdef USE_XF86_EXTENSIONS
#include "fullscreen.h"
#endif

static int selection_from_image = 0;

/* ------------------------------------------------------------------------- */
void ui_set_selected_file(int num)
{
    selection_from_image = num;
}

/* ------------------------------------------------------------------------- */

static char *read_disk_image_contents(const char *name)
{
    image_contents_t *contents;
    char *s;

    contents = image_contents_read_disk(name);
    if (contents == NULL)
        return NULL;

#ifdef USE_GNOMEUI
    s = image_contents_to_string(contents, IMAGE_CONTENTS_STRING_PETSCII);
#else
    s = image_contents_to_string(contents, IMAGE_CONTENTS_STRING_ASCII);
#endif

    image_contents_destroy(contents);

    return s;
}

static UI_CALLBACK(attach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;
    char *filename, *title;
    ui_button_t button;
    static char *last_dir;
    int attach_wp = 0;

    vsync_suspend_speed_eval();
    title = xmsprintf(_("Attach Disk Image as unit #%d"), unit);
    filename = ui_select_file(title, read_disk_image_contents,
                              unit == 8 ? True : False, last_dir,
                              "*.[gdxGDX]*", &button, True, &attach_wp);

    free(title);
    if (attach_wp) {
        printf("Write protect attach requested.\n");
        resources_set_sprintf("AttachDevice%dReadonly",
                              (resource_value_t)attach_wp, unit);
    }

    switch (button) {
      case UI_BUTTON_OK:
        if (file_system_attach_disk(unit, filename) < 0)
            ui_error(_("Invalid Disk Image"));
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_disk(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Disk Image or Filename"));
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }

    if (filename != NULL)
        free(filename);
}

static UI_CALLBACK(attach_empty_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;
    /* Where does the 1024 come from?  */
    char filename[1024];

    vsync_suspend_speed_eval();

    /* The following code depends on a zeroed filename.  */
    memset(filename, 0, 1024);

    if (ui_empty_disk_dialog(filename) < 0)
        return;

    if (file_system_attach_disk(unit, filename) < 0)
        ui_error(_("Invalid Disk Image"));
}

static UI_CALLBACK(detach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();
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

#ifdef USE_GNOMEUI
    s = image_contents_to_string(contents, IMAGE_CONTENTS_STRING_PETSCII);
#else
    s = image_contents_to_string(contents, IMAGE_CONTENTS_STRING_ASCII);
#endif

    image_contents_destroy(contents);

    return s;
}

static UI_CALLBACK(attach_tape)
{
    char *filename;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Attach a tape image"),
                              read_tape_image_contents,
                              True, last_dir, "*.[tT]*", &button, True, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (tape_image_attach(1, filename) < 0)
            ui_error(_("Invalid Tape Image"));
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_tape(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Tape Image"));
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        free(filename);
}

static UI_CALLBACK(detach_tape)
{
    tape_image_detach(1);
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
    static char *last_dir;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Smart-attach a file"),
                              read_disk_or_tape_image_contents,
                              True, last_dir, NULL, &button, True, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (file_system_attach_disk(8, filename) < 0
            && tape_image_attach(1, filename) < 0) {
            ui_error(_("Unknown image type"));
        }
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_autodetect(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Unknown image type"));
        if (last_dir)
            free(last_dir);
        util_fname_split(filename, &last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        free(filename);
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(change_working_directory)
{
    char *wd;
    wd = xmalloc(MAXPATHLEN);

    getcwd(wd, MAXPATHLEN);
    vsync_suspend_speed_eval();
    if (ui_input_string(_("VICE setting"),
                        _("Change current working directory"),
                        wd, MAXPATHLEN) == UI_BUTTON_OK) {
        if (chdir(wd) < 0)
            ui_error(_("Directory not found"));
    }
    free(wd);
}

static void mon_trap(ADDRESS addr, void *unused_data)
{
    mon(addr);
}

static UI_CALLBACK(activate_monitor)
{
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_off();
#endif
    vsync_suspend_speed_eval();
    ui_dispatch_events();               /* popdown the menu */
    ui_autorepeat_on();

    if (!ui_emulation_is_paused())
        maincpu_trigger_trap(mon_trap, (void *)0);
    else
        mon_trap(MOS6510_REGS_GET_PC(&maincpu_regs), 0);
}

static UI_CALLBACK(run_c1541)
{
#ifdef USE_XF86_DGA2_EXTENSIONS
    fullscreen_mode_off();
#endif
    vsync_suspend_speed_eval();
    sound_close();
    switch (system("xterm -sb -e c1541 &")) {
      case 127:
        ui_error(_("Couldn't run /bin/sh???"));
        break;
      case -1:
        ui_error(_("Couldn't run xterm"));
        break;
      case 0:
        break;
      default:
        ui_error(_("Unknown error while running c1541"));
    }
}

static UI_CALLBACK(unit8_reset)
{
    vsync_suspend_speed_eval();
    drive0_trigger_reset();
}

static UI_CALLBACK(unit9_reset)
{
    vsync_suspend_speed_eval();
    drive1_trigger_reset();
}

static UI_CALLBACK(reset)
{
    vsync_suspend_speed_eval();
    maincpu_trigger_reset();
}

static UI_CALLBACK(powerup_reset)
{
    vsync_suspend_speed_eval();
    machine_powerup();
}

static UI_CALLBACK(browse_manual)
{
    const char *bcommand = NULL;

    resources_get_value("HTMLBrowserCommand", (resource_value_t *)&bcommand);

    if (bcommand == NULL ||
        *bcommand == '\0') {
        ui_error(_("No HTML browser is defined."));
    } else {
        /* FIXME: Argh.  Ugly!  */
#define BROWSE_CMD_BUF_MAX 16384
        char buf[BROWSE_CMD_BUF_MAX];
        static const char manual_path[] = DOCDIR "/vice_toc.html";
        char *res_ptr;
        int manual_path_len, cmd_len;

#ifdef USE_XF86_DGA2_EXTENSIONS
        fullscreen_mode_off();
#endif
        cmd_len = strlen(bcommand);
        manual_path_len = strlen(manual_path);

        res_ptr = strstr(bcommand, "%s");
        if (res_ptr == NULL) {
            /* No substitution. */
            if (cmd_len + 2 > BROWSE_CMD_BUF_MAX - 1) {
                ui_error(_("Browser command too long."));
                return;
            }
            sprintf(buf, "%s &", bcommand);
        } else {
            char *tmp_ptr, *cmd_ptr;
            int offs;

            /* Replace each occurrence of "%s" with the path of the HTML
               manual. */

            cmd_len += manual_path_len - 2;
            cmd_len += 2;       /* Trailing " &". */
            if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
                ui_error(_("Browser command too long."));
                return;
            }

            offs = res_ptr - bcommand;
            memcpy(buf, bcommand, offs);
            strcpy(buf + offs, manual_path);
            cmd_ptr = buf + offs + manual_path_len;
            res_ptr += 2;

            while ((tmp_ptr = strstr(res_ptr, "%s")) != NULL) {
                cmd_len += manual_path_len - 2;
                if (cmd_len > BROWSE_CMD_BUF_MAX - 1) {
                    ui_error(_("Browser command too long."));
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

        log_debug(_("Executing `%s'..."), buf);
        if (system(buf) != 0)
            ui_error(_("Cannot run HTML browser."));
    }
}

static UI_CALLBACK(do_exit)
{
    ui_exit();
}

/* ------------------------------------------------------------------------- */

static UI_CALLBACK(toggle_pause)
{
    static int pause = 0;

    if (!CHECK_MENUS) {
        pause = !pause;
        ui_update_menus();
        ui_pause_emulation(pause);
        return;
    }

    ui_menu_set_tick(w, pause);
}

/* ------------------------------------------------------------------------- */

/* Snapshot commands.  */

static void load_snapshot_trap(ADDRESS unused_addr, void *data)
{
    ui_button_t button;
    char *filename;
    static char *last_dir;

    vsync_suspend_speed_eval();

    if (data) {
        log_debug(_("Quickloading file %s."), (char *)data);
        filename = (char *)data;
    } else {
        filename = ui_select_file(_("Load snapshot"), NULL, False, last_dir,
                              "*.vsf", &button, False, NULL);
        if (button != UI_BUTTON_OK) {
            if (filename)
                free(filename);
            return;
        }
    }
    if (last_dir)
        free(last_dir);
    util_fname_split(filename, &last_dir, NULL);

    if (machine_read_snapshot(filename) < 0)
        ui_error(_("Cannot load snapshot file\n`%s'"), filename);
    ui_update_menus();

    if (filename != NULL)
        free(filename);
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
        maincpu_trigger_trap(load_snapshot_trap, (void *)fname);
    else
        load_snapshot_trap(0, (void *)fname);
}

static void save_snapshot_trap(ADDRESS unused_addr, void *data)
{
    if (data) {
        /* quick snapshot, save ROMs & disks (??) */
        log_debug(_("Quicksaving file %s."), (char *)data);
        if (machine_write_snapshot(data, 1, 1) < 0) {
            ui_error(_("Cannot write snapshot file\n`%s'\n"), data);
        }
        free(data);
    } else {
        ui_snapshot_dialog();
    }

    vsync_suspend_speed_eval();
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
    long data;                  /* should be enough for a pointer */
};

typedef enum {
    CBD_NEXT, CBD_PREV, CBD_ADD, CBD_REMOVE
} cbd_enum_t;

static UI_CALLBACK(attach_from_fliplist)
{
    flip_attach_head(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit,
                     (int) ((struct cb_data_t *) UI_MENU_CB_PARAM)->data);
}

static UI_CALLBACK(attach_from_fliplist2)
{
    file_system_attach_disk(flip_get_unit((void *) UI_MENU_CB_PARAM),
                            flip_get_image((void *) UI_MENU_CB_PARAM));
}

static UI_CALLBACK(attach_from_fliplist3)
{
    flip_attach_head(8, (int) UI_MENU_CB_PARAM);
}

static UI_CALLBACK(add2fliplist)
{
    flip_add_image(8);
    ui_update_flip_menus(8, 8);
}

static UI_CALLBACK(add2fliplist2)
{
    flip_set_current(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit,
                     (char *) ((struct cb_data_t *) UI_MENU_CB_PARAM)->data);
    flip_add_image(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit);
    ui_update_flip_menus(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit,
                         ((struct cb_data_t *) UI_MENU_CB_PARAM)->unit);
}

static UI_CALLBACK(remove_from_fliplist)
{
    flip_remove(8, NULL);
    ui_update_flip_menus(8, 8);
}

static UI_CALLBACK(remove_from_fliplist2)
{
    flip_remove(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit,
                (char *) ((struct cb_data_t *) UI_MENU_CB_PARAM)->data);
    ui_update_flip_menus(((struct cb_data_t *) UI_MENU_CB_PARAM)->unit,
                         ((struct cb_data_t *) UI_MENU_CB_PARAM)->unit);
}

static UI_CALLBACK(load_save_fliplist)
{
    char *filename, *title;
    int what = (int)UI_MENU_CB_PARAM;
    ui_button_t button;
    static char *last_dir;

    vsync_suspend_speed_eval();
    title = concat (what?_("Load "):_("Save"), _("Fliplist File"), NULL);
    filename = ui_select_file(title, 0, False, last_dir, "*.vfl", &button,
			      True, NULL);
    free (title);
    switch (button)
    {
    case UI_BUTTON_OK:
	if (what)
	{
 	    if (flip_load_list((unsigned int) -1, filename, 0) == 0)
		ui_message(_("Successfully read `%s'."), filename);
	    else
		ui_error(_("Error reading `%s'."), filename);
	}
	else
	{
 	    if (flip_save_list((unsigned int) -1, filename) == 0)
		ui_message(_("Successfully wrote `%s'."), filename);
	    else
		ui_error(_("Error writing `%s'."), filename);
	}
	if (last_dir)
	    free(last_dir);
	util_fname_split(filename, &last_dir, NULL);
	break;
    default:
	break;
    }
    
}

#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(AttachDevice8Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice9Readonly)
#endif

#define FLIPLIST_MENU_LIMIT 256
void ui_update_flip_menus(int from_unit, int to_unit)
{
    /* Yick, allocate dynamically */
    static ui_menu_entry_t flipmenu[NUM_DRIVES][FLIPLIST_MENU_LIMIT];
    static struct cb_data_t cb_data[NUM_DRIVES][sizeof(cbd_enum_t)];

    char *image = NULL, *t0 = NULL, *t1 = NULL, *t2 = NULL, *t3 = NULL;
    char *t4 = NULL, *t5 = NULL, *dir;
    void *fl_iterator;
    int i, drive, true_emu, fliplist_start = 0;
    static int name_count = 0;
    char *menuname;

    resources_get_value("DriveTrueEmulation",
                        (resource_value_t *)&true_emu);

    for (drive = from_unit - 8;
        (drive <= to_unit - 8) && (drive < NUM_DRIVES);
        drive++) {

        i = 0;
        t0 = t1 = t2 = t3 = t4 = t5 = NULL;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        t0 = xmsprintf(_("Attach #%d"), drive + 8);
        flipmenu[drive][i].string = t0;
        flipmenu[drive][i].callback = (ui_callback_t)attach_disk;
        flipmenu[drive][i].callback_data = (ui_callback_data_t)(drive + 8);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        t5 = xmsprintf(_("Detach #%d"), drive + 8);
        flipmenu[drive][i].string = t5;
        flipmenu[drive][i].callback = (ui_callback_t)detach_disk;
        flipmenu[drive][i].callback_data = (ui_callback_data_t)(drive + 8);
        i++;

#ifdef USE_GNOMEUI
        /* drivesettings */
        /* this won't work so far for Xaw, because the checkmarks
           aren't updated when a menu is destroyed, as the flipmenu is
           dynamically regenerated; The Gnome code is already fixed. */
        memcpy(&flipmenu[drive][i], (const char *)ui_drive_settings_menu,
               sizeof (ui_menu_entry_t));
        i++;
        /* Write protext UI controll */
        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = "*Write Protect";
        if (drive == 0)
            flipmenu[drive][i].callback = toggle_AttachDevice8Readonly;
        else
            flipmenu[drive][i].callback = toggle_AttachDevice9Readonly;
        i++;
#endif

        fliplist_start = i;     /* if we take the goto don't free anythin */

        /* don't update menu deeply when drive has not been enabled
           or nothing has been attached */
        if (true_emu) {
            if (! ((1 << drive) & enabled_drives))
                goto update_menu;
        } else {
            if (strcmp(last_attached_images[drive], "") == 0)
                goto update_menu;
        }

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = "--";
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(flip_get_next(drive + 8), &dir, &image);
        t1 = concat(_("Next: "), image ? image : _("<empty>"), NULL);
        flipmenu[drive][i].string = t1;
        flipmenu[drive][i].callback = (ui_callback_t)attach_from_fliplist;
        cb_data[drive][CBD_NEXT].unit = drive + 8;
        cb_data[drive][CBD_NEXT].data = 1;
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_NEXT]);
        if (dir)
            free(dir);
        if (image)
            free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(flip_get_prev(drive + 8), &dir, &image);
        t2 = concat(_("Previous: "), image ? image : _("<empty>"), NULL);
        flipmenu[drive][i].string = t2;
        flipmenu[drive][i].callback = (ui_callback_t)attach_from_fliplist;
        cb_data[drive][CBD_PREV].unit = drive + 8;
        cb_data[drive][CBD_PREV].data = 0;
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_PREV]);
        if (dir)
            free(dir);
        if (image)
            free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(last_attached_images[drive], &dir, &image);
        t3 = concat(_("Add: "), image, NULL);
        flipmenu[drive][i].string = t3;
        flipmenu[drive][i].callback = (ui_callback_t)add2fliplist2;
        cb_data[drive][CBD_ADD].unit = drive + 8;
        cb_data[drive][CBD_ADD].data = (long) last_attached_images[drive];
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_ADD]);
        if (dir)
            free(dir);
        if (image)
            free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(last_attached_images[drive], &dir, &image);
        t4 = concat(_("Remove: "), image, NULL);
        flipmenu[drive][i].string = t4;
        flipmenu[drive][i].callback = (ui_callback_t)remove_from_fliplist2;
        cb_data[drive][CBD_REMOVE].unit = drive + 8;
        cb_data[drive][CBD_REMOVE].data = (long) last_attached_images[drive];
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_REMOVE]);
        if (dir)
            free(dir);
        if (image)
            free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = "--";
        i++;

        /* Now collect current fliplist */
        fl_iterator=flip_init_iterate(drive + 8);
        fliplist_start = i;
        while (fl_iterator) {
            memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
            util_fname_split(flip_get_image(fl_iterator), &dir, &image);
            flipmenu[drive][i].string = concat(NO_TRANS, image, NULL);
            flipmenu[drive][i].callback =
                (ui_callback_t)attach_from_fliplist2;
            flipmenu[drive][i].callback_data =
                (ui_callback_data_t)fl_iterator;

            fl_iterator = flip_next_iterate(drive + 8);
            if (dir)
                free(dir);
            if (image)
                free(image);
            i++;
            if (i >= (FLIPLIST_MENU_LIMIT - 1)) {
                /* the end delimitor must fit */ 
                log_warning(LOG_DEFAULT,
                            "Number of fliplist menu entries exceeded."
                            "Cutting after %d entries.", i);
                break;
            }
        }

    update_menu:
        /* make sure the menu is well terminated */
        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));

        menuname = xmsprintf("LeftDrive%iMenu%i", drive + 8, name_count);

        /* ugly ... */
        if (drive == 0)
        {
            ui_destroy_drive8_menu();
            /* FIXME: Make sure the widget is really destroyed! */
            ui_set_drive8_menu(ui_menu_create(menuname /*"LeftDrive8Menu"*/,
                                              flipmenu[drive], NULL));
        }
        else
        {
            ui_destroy_drive9_menu();
            /* FIXME: Make sure the widget is really destroyed! */
            ui_set_drive9_menu(ui_menu_create(menuname /*"LeftDrive9Menu"*/,
                                              flipmenu[drive], NULL));
        }

        free(menuname);

        if (t0)
            free(t0);
        if (t1)
            free(t1);
        if (t2)
            free(t2);
        if (t3)
            free(t3);
        if (t4)
            free(t4);
        if (t5)
            free(t5);
        while (fliplist_start < i) {
            if (flipmenu[drive][fliplist_start].string)
                free(flipmenu[drive][fliplist_start].string);
            fliplist_start++;
        }
    }
}

/* ------------------------------------------------------------------------- */

static ui_menu_entry_t attach_empty_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 8, NULL, },
    { N_("Unit #9..."),
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 9, NULL, },
    { N_("Unit #10..."),
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 10, NULL, },
    { N_("Unit #11..."),
      (ui_callback_t) attach_empty_disk, (ui_callback_data_t) 11, NULL, },
    { NULL }
};

static ui_menu_entry_t attach_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t) attach_disk, (ui_callback_data_t) 8, NULL,
      XK_8, UI_HOTMOD_META },
    { N_("Unit #9..."),
      (ui_callback_t) attach_disk, (ui_callback_data_t) 9, NULL,
      XK_9, UI_HOTMOD_META },
    { N_("Unit #10..."),
      (ui_callback_t) attach_disk, (ui_callback_data_t) 10, NULL,
      XK_0, UI_HOTMOD_META },
    { N_("Unit #11..."),
      (ui_callback_t) attach_disk, (ui_callback_data_t) 11, NULL,
      XK_1, UI_HOTMOD_META },
    { NULL }
};

static ui_menu_entry_t detach_disk_image_submenu[] = {
    { N_("Unit #8"),
      (ui_callback_t) detach_disk, (ui_callback_data_t) 8, NULL },
    { N_("Unit #9"),
      (ui_callback_t) detach_disk, (ui_callback_data_t) 9, NULL },
    { N_("Unit #10"),
      (ui_callback_t) detach_disk, (ui_callback_data_t) 10, NULL },
    { N_("Unit #11"),
      (ui_callback_t) detach_disk, (ui_callback_data_t) 11, NULL },
    { "--" },
    { N_("All"),
      (ui_callback_t) detach_disk, (ui_callback_data_t) -1, NULL },
    { NULL }
};

static ui_menu_entry_t reset_submenu[] = {
    { N_("Soft"),
      (ui_callback_t) reset, NULL, NULL,
      XK_F9, UI_HOTMOD_META },
    { N_("Hard"),
      (ui_callback_t) powerup_reset, NULL, NULL,
      XK_F12, UI_HOTMOD_META },
    { "--" },
    { N_("Unit #8"),
      (ui_callback_t) unit8_reset, NULL, NULL },
    { N_("Unit #9"),
      (ui_callback_t) unit9_reset, NULL, NULL },
    { NULL }
};

static ui_menu_entry_t flip_submenu[] = {
    { N_("Add current image (Unit 8)"),
      (ui_callback_t) add2fliplist, (ui_callback_data_t) 0, NULL,
      XK_i, UI_HOTMOD_META },
    { N_("Remove current image (Unit 8)"),
      (ui_callback_t) remove_from_fliplist, (ui_callback_data_t) 0, NULL,
      XK_k, UI_HOTMOD_META },
    { N_("Attach next image (Unit 8)"),
      (ui_callback_t) attach_from_fliplist3, (ui_callback_data_t) 1, NULL,
      XK_n, UI_HOTMOD_META },
    { N_("Attach previous image (Unit 8)"),
      (ui_callback_t) attach_from_fliplist3, (ui_callback_data_t) 0, NULL,
      XK_N, UI_HOTMOD_META },
    { N_("Load fliplist file"),
      (ui_callback_t) load_save_fliplist, (ui_callback_data_t) 1, NULL },
    { N_("Save fliplist file"),
      (ui_callback_t) load_save_fliplist, (ui_callback_data_t) 0, NULL },
    { NULL }
};

ui_menu_entry_t ui_disk_commands_menu[] = {
    { N_("Attach a disk image"),
      NULL, NULL, attach_disk_image_submenu },
    { N_("Create and attach an empty disk"),
      NULL, NULL, attach_empty_disk_image_submenu },
    { N_("Detach disk image"),
      NULL, NULL, detach_disk_image_submenu },
    { N_("Fliplist"),
      NULL, NULL, flip_submenu },
    { NULL }
};

ui_menu_entry_t ui_tape_commands_menu[] = {
    { N_("Attach a tape image..."),
      (ui_callback_t) attach_tape, NULL, NULL,
      XK_t, UI_HOTMOD_META},
    { N_("Detach tape image"),
      (ui_callback_t) detach_tape, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_smart_attach_commands_menu[] = {
    { N_("Smart-attach disk/tape..."),
      (ui_callback_t) smart_attach, NULL, NULL,
      XK_a, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_directory_commands_menu[] = {
    { N_("Change working directory..."),
      (ui_callback_t) change_working_directory, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_snapshot_commands_submenu[] = {
    { N_("Load snapshot..."),
      (ui_callback_t) load_snapshot, NULL, NULL,
      XK_l, UI_HOTMOD_META },
    { N_("Save snapshot..."),
      (ui_callback_t) save_snapshot, NULL, NULL,
      XK_s, UI_HOTMOD_META },
    { "--" },
    { N_("Quickload snapshot"),
      (ui_callback_t) load_quicksnap, NULL, NULL,
      XK_F10, UI_HOTMOD_META },
    { N_("Quicksave snapshot"),
      (ui_callback_t) save_quicksnap, NULL, NULL,
      XK_F11, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_snapshot_commands_menu[] = {
    { N_("Snapshot commands"),
      NULL,  NULL, ui_snapshot_commands_submenu },
    { NULL }
};

ui_menu_entry_t ui_tool_commands_menu[] = {
    { N_("Activate monitor"),
      (ui_callback_t) activate_monitor, NULL, NULL },
    { N_("Run C1541"),
      (ui_callback_t) run_c1541, NULL, NULL },
    { NULL }
};

extern ui_callback_t about;

ui_menu_entry_t ui_help_commands_menu[] = {
    { N_("Browse manuals"),
      (ui_callback_t) browse_manual, NULL, NULL },
    { N_("About VICE..."),
      (ui_callback_t) ui_about, NULL, NULL },
    { NULL }
};

ui_menu_entry_t ui_run_commands_menu[] = {
    { N_("Reset"),
      NULL, NULL, reset_submenu },
    { N_("*Pause"),
      (ui_callback_t) toggle_pause, NULL, NULL,
      XK_p, UI_HOTMOD_META },
    { NULL }
};

ui_menu_entry_t ui_exit_commands_menu[] = {
    { N_("Exit emulator"),
      (ui_callback_t) do_exit, NULL, NULL,
      XK_q, UI_HOTMOD_META },
    { NULL }
};

