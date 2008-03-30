/*
 * uifliplist.c
 *
 * Written by
 *  Martin Pottendorfer <Martin.Pottendorfer@aut.alcatel.at>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "attach.h"
#include "fliplist.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "uifliplist.h"
#include "uidrive.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"


struct cb_data_t {
    int unit;
    long data;                  /* should be enough for a pointer */
};

typedef enum {
    CBD_NEXT, CBD_PREV, CBD_ADD, CBD_REMOVE
} cbd_enum_t;


extern char last_attached_images[NUM_DRIVES][256];
extern ui_drive_enable_t enabled_drives;
extern UI_CALLBACK(attach_disk);
extern UI_CALLBACK(detach_disk);

static UI_CALLBACK(attach_from_fliplist3)
{
    fliplist_attach_head(8, (int)UI_MENU_CB_PARAM);
}

static UI_CALLBACK(add2fliplist)
{
    fliplist_add_image(8);
    uifliplist_update_menus(8, 8);
}

static UI_CALLBACK(remove_from_fliplist)
{
    fliplist_remove(8, NULL);
    uifliplist_update_menus(8, 8);
}

static char *load_save_fliplist_last_dir = NULL;

static UI_CALLBACK(load_save_fliplist)
{
    char *filename, *title;
    int what = (int)UI_MENU_CB_PARAM;
    ui_button_t button;

    vsync_suspend_speed_eval();
    title = util_concat(what ? _("Load ") : _("Save"), _("Fliplist File"),
                        NULL);
    filename = ui_select_file(title, NULL, 0, False,
                              load_save_fliplist_last_dir, "*.vfl",
                              &button, True, NULL);
    lib_free(title);
    switch (button) {
      case UI_BUTTON_OK:
        if (what) {
            if (fliplist_load_list((unsigned int)-1, filename, 0) == 0)
                ui_message(_("Successfully read `%s'."), filename);
            else
                ui_error(_("Error reading `%s'."), filename);
        } else {
            if (fliplist_save_list((unsigned int)-1, filename) == 0)
                ui_message(_("Successfully wrote `%s'."), filename);
            else
                ui_error(_("Error writing `%s'."), filename);
        }
        if (load_save_fliplist_last_dir)
            lib_free(load_save_fliplist_last_dir);
        util_fname_split(filename, &load_save_fliplist_last_dir, NULL);
        break;
      default:
        break;
    }

}

ui_menu_entry_t fliplist_submenu[] = {
    { N_("Add current image (Unit 8)"),
      (ui_callback_t)add2fliplist, (ui_callback_data_t)0, NULL,
      XK_i, UI_HOTMOD_META },
    { N_("Remove current image (Unit 8)"),
      (ui_callback_t)remove_from_fliplist, (ui_callback_data_t)0, NULL,
      XK_k, UI_HOTMOD_META },
    { N_("Attach next image (Unit 8)"),
      (ui_callback_t)attach_from_fliplist3, (ui_callback_data_t)1, NULL,
      XK_n, UI_HOTMOD_META },
    { N_("Attach previous image (Unit 8)"),
      (ui_callback_t)attach_from_fliplist3, (ui_callback_data_t)0, NULL,
      XK_N, UI_HOTMOD_META },
    { N_("Load fliplist file"),
      (ui_callback_t)load_save_fliplist, (ui_callback_data_t)1, NULL },
    { N_("Save fliplist file"),
      (ui_callback_t)load_save_fliplist, (ui_callback_data_t)0, NULL },
    { NULL }
};

static UI_CALLBACK(attach_from_fliplist2)
{
    file_system_attach_disk(fliplist_get_unit((void *)UI_MENU_CB_PARAM),
                            fliplist_get_image((void *)UI_MENU_CB_PARAM));
}

static UI_CALLBACK(remove_from_fliplist2)
{
    fliplist_remove(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit,
                    (char *) ((struct cb_data_t *)UI_MENU_CB_PARAM)->data);
    uifliplist_update_menus(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit,
                            ((struct cb_data_t *)UI_MENU_CB_PARAM)->unit);
}

static UI_CALLBACK(add2fliplist2)
{
    fliplist_set_current(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit,
                         (char *) ((struct cb_data_t *)UI_MENU_CB_PARAM)->data);
    fliplist_add_image(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit);
    uifliplist_update_menus(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit,
                            ((struct cb_data_t *)UI_MENU_CB_PARAM)->unit);
}

static UI_CALLBACK(attach_from_fliplist)
{
    fliplist_attach_head(((struct cb_data_t *)UI_MENU_CB_PARAM)->unit,
                         (int) ((struct cb_data_t *)UI_MENU_CB_PARAM)->data);
}

#ifdef USE_GNOMEUI
UI_MENU_DEFINE_TOGGLE(AttachDevice8Readonly)
UI_MENU_DEFINE_TOGGLE(AttachDevice9Readonly)
#endif

#define FLIPLIST_MENU_LIMIT 256
void uifliplist_update_menus(int from_unit, int to_unit)
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

    resources_get_value("DriveTrueEmulation", (void *)&true_emu);

    for (drive = from_unit - 8;
        (drive <= to_unit - 8) && (drive < NUM_DRIVES);
        drive++) {

        i = 0;
        t0 = t1 = t2 = t3 = t4 = t5 = NULL;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        t0 = lib_msprintf(_("Attach #%d"), drive + 8);
        flipmenu[drive][i].string = t0;
        flipmenu[drive][i].callback = (ui_callback_t)attach_disk;
        flipmenu[drive][i].callback_data = (ui_callback_data_t)(drive + 8);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        t5 = lib_msprintf(_("Detach #%d"), drive + 8);
        flipmenu[drive][i].string = t5;
        flipmenu[drive][i].callback = (ui_callback_t)detach_disk;
        flipmenu[drive][i].callback_data = (ui_callback_data_t)(drive + 8);
        i++;

#ifdef USE_GNOMEUI
        /* drivesettings */
        /* this won't work so far for Xaw, because the checkmarks
           aren't updated when a menu is destroyed, as the flipmenu is
           dynamically regenerated; The Gnome code is already fixed. */
        memcpy(&flipmenu[drive][i], (const char *)ui_drive_options_submenu,
               sizeof(ui_menu_entry_t));
        i++;
        /* Write protext UI controll */
        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = _("*Write Protect");
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
            if (!((1 << drive) & enabled_drives))
                goto update_menu;
        } else {
            if (strcmp(last_attached_images[drive], "") == 0)
                goto update_menu;
        }

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = "--";
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(fliplist_get_next(drive + 8), &dir, &image);
        t1 = util_concat(_("Next: "), image ? image : _("<empty>"), NULL);
        flipmenu[drive][i].string = t1;
        flipmenu[drive][i].callback = (ui_callback_t)attach_from_fliplist;
        cb_data[drive][CBD_NEXT].unit = drive + 8;
        cb_data[drive][CBD_NEXT].data = 1;
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_NEXT]);
        if (dir)
            lib_free(dir);
        if (image)
            lib_free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(fliplist_get_prev(drive + 8), &dir, &image);
        t2 = util_concat(_("Previous: "), image ? image : _("<empty>"), NULL);
        flipmenu[drive][i].string = t2;
        flipmenu[drive][i].callback = (ui_callback_t)attach_from_fliplist;
        cb_data[drive][CBD_PREV].unit = drive + 8;
        cb_data[drive][CBD_PREV].data = 0;
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_PREV]);
        if (dir)
            lib_free(dir);
        if (image)
            lib_free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(last_attached_images[drive], &dir, &image);
        t3 = util_concat(_("Add: "), image, NULL);
        flipmenu[drive][i].string = t3;
        flipmenu[drive][i].callback = (ui_callback_t)add2fliplist2;
        cb_data[drive][CBD_ADD].unit = drive + 8;
        cb_data[drive][CBD_ADD].data = (long) last_attached_images[drive];
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_ADD]);
        if (dir)
            lib_free(dir);
        if (image)
            lib_free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        util_fname_split(last_attached_images[drive], &dir, &image);
        t4 = util_concat(_("Remove: "), image, NULL);
        flipmenu[drive][i].string = t4;
        flipmenu[drive][i].callback = (ui_callback_t)remove_from_fliplist2;
        cb_data[drive][CBD_REMOVE].unit = drive + 8;
        cb_data[drive][CBD_REMOVE].data = (long) last_attached_images[drive];
        flipmenu[drive][i].callback_data =
            (ui_callback_data_t)&(cb_data[drive][CBD_REMOVE]);
        if (dir)
            lib_free(dir);
        if (image)
            lib_free(image);
        i++;

        memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
        flipmenu[drive][i].string = "--";
        i++;

        /* Now collect current fliplist */
        fl_iterator = fliplist_init_iterate(drive + 8);
        fliplist_start = i;
        while (fl_iterator) {
            memset(&(flipmenu[drive][i]), 0, sizeof(ui_menu_entry_t));
            util_fname_split(fliplist_get_image(fl_iterator), &dir, &image);
            flipmenu[drive][i].string = util_concat(NO_TRANS, image, NULL);
            flipmenu[drive][i].callback =
                (ui_callback_t)attach_from_fliplist2;
            flipmenu[drive][i].callback_data =
                (ui_callback_data_t)fl_iterator;

            fl_iterator = fliplist_next_iterate(drive + 8);
            if (dir)
                lib_free(dir);
            if (image)
                lib_free(image);
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

        menuname = lib_msprintf("LeftDrive%iMenu%i", drive + 8, name_count);

#ifdef USE_GNOMEUI
        ui_destroy_drive_menu(drive);
        ui_set_drive_menu(drive, ui_menu_create(menuname,
                                                flipmenu[drive], NULL));
#else
        /* ugly ... */
        if (drive == 0) {
            ui_destroy_drive8_menu();
            /* FIXME: Make sure the widget is really destroyed! */
            ui_set_drive8_menu(ui_menu_create(menuname /*"LeftDrive8Menu"*/,
                                              flipmenu[drive], NULL));
        } else {
            ui_destroy_drive9_menu();
            /* FIXME: Make sure the widget is really destroyed! */
            ui_set_drive9_menu(ui_menu_create(menuname /*"LeftDrive9Menu"*/,
                                              flipmenu[drive], NULL));
        }
#endif

        lib_free(menuname);

        if (t0)
            lib_free(t0);
        if (t1)
            lib_free(t1);
        if (t2)
            lib_free(t2);
        if (t3)
            lib_free(t3);
        if (t4)
            lib_free(t4);
        if (t5)
            lib_free(t5);
        while (fliplist_start < i) {
            if (flipmenu[drive][fliplist_start].string)
                lib_free(flipmenu[drive][fliplist_start].string);
            fliplist_start++;
        }
    }
}

void uifliplist_shutdown(void)
{
    lib_free(load_save_fliplist_last_dir);
}

