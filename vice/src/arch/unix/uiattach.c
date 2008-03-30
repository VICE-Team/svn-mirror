/*
 * uiattach.c
 *
 * Written by
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

#include "attach.h"
#include "autostart.h"
#include "imagecontents.h"
#include "lib.h"
#include "resources.h"
#include "tape.h"
#include "uiattach.h"
#include "uiedisk.h"
#include "uifliplist.h"
#include "uimenu.h"
#include "util.h"
#include "vsync.h"
#include "ioutil.h"


static int selection_from_image = 0;

void ui_set_selected_file(int num)
{
    selection_from_image = num;
}

static char *read_disk_image_contents(const char *name, unsigned int unit)
{
#ifdef USE_GNOMEUI
    return image_contents_read_string(IMAGE_CONTENTS_DISK, name, unit,
                                      IMAGE_CONTENTS_STRING_PETSCII);
#else
    return image_contents_read_string(IMAGE_CONTENTS_DISK, name, unit,
                                      IMAGE_CONTENTS_STRING_ASCII);
#endif
}

static char *attach_disk_last_dir = NULL;

UI_CALLBACK(attach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;
    char *filename, *title;
    ui_button_t button;
    int attach_wp = 0;

    vsync_suspend_speed_eval();
    title = lib_msprintf(_("Attach Disk Image as unit #%d"), unit);
    filename = ui_select_file(title, read_disk_image_contents, unit,
                              unit == 8 ? True : False, attach_disk_last_dir,
                              "*.[gdxGDX]*", &button, True, &attach_wp);

    lib_free(title);
    if (attach_wp) {
        printf("Write protect attach requested.\n");
        resources_set_sprintf("AttachDevice%dReadonly",
                              (resource_value_t)attach_wp, unit);
    }

    switch (button) {
      case UI_BUTTON_OK:
        if (file_system_attach_disk(unit, filename) < 0)
            ui_error(_("Invalid Disk Image"));
        if (attach_disk_last_dir)
            lib_free(attach_disk_last_dir);
        util_fname_split(filename, &attach_disk_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_disk(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Disk Image or Filename"));
        if (attach_disk_last_dir)
            lib_free(attach_disk_last_dir);
        util_fname_split(filename, &attach_disk_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }

    if (filename != NULL)
        lib_free(filename);
}

static ui_menu_entry_t attach_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)8, NULL,
      XK_8, UI_HOTMOD_META },
    { N_("Unit #9..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)9, NULL,
      XK_9, UI_HOTMOD_META },
    { N_("Unit #10..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)10, NULL,
      XK_0, UI_HOTMOD_META },
    { N_("Unit #11..."),
      (ui_callback_t)attach_disk, (ui_callback_data_t)11, NULL,
      XK_1, UI_HOTMOD_META },
    { NULL }
};

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

static ui_menu_entry_t attach_empty_disk_image_submenu[] = {
    { N_("Unit #8..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)8, NULL, },
    { N_("Unit #9..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)9, NULL, },
    { N_("Unit #10..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)10, NULL, },
    { N_("Unit #11..."),
      (ui_callback_t)attach_empty_disk, (ui_callback_data_t)11, NULL, },
    { NULL }
};

UI_CALLBACK(detach_disk)
{
    int unit = (int)UI_MENU_CB_PARAM;

    vsync_suspend_speed_eval();
    file_system_detach_disk(unit);
}

static ui_menu_entry_t detach_disk_image_submenu[] = {
    { N_("Unit #8"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)8, NULL },
    { N_("Unit #9"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)9, NULL },
    { N_("Unit #10"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)10, NULL },
    { N_("Unit #11"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)11, NULL },
    { "--" },
    { N_("All"),
      (ui_callback_t)detach_disk, (ui_callback_data_t)-1, NULL },
    { NULL }
};

ui_menu_entry_t uiattach_disk_menu[] = {
    { N_("Attach a disk image"),
      NULL, NULL, attach_disk_image_submenu },
    { N_("Create and attach an empty disk"),
      NULL, NULL, attach_empty_disk_image_submenu },
    { N_("Detach disk image"),
      NULL, NULL, detach_disk_image_submenu },
    { N_("Fliplist"),
      NULL, NULL, fliplist_submenu },
    { NULL }
};

static char *read_tape_image_contents(const char *name, unsigned int unit)
{
#ifdef USE_GNOMEUI
    return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, unit,
                                      IMAGE_CONTENTS_STRING_PETSCII);
#else
    return image_contents_read_string(IMAGE_CONTENTS_TAPE, name, unit,
                                      IMAGE_CONTENTS_STRING_ASCII);
#endif
}

static char *attach_tape_last_dir = NULL;

static UI_CALLBACK(attach_tape)
{
    char *filename;
    ui_button_t button;

    vsync_suspend_speed_eval();

    filename = ui_select_file(_("Attach a tape image"),
                              read_tape_image_contents, 0,
                              True, attach_tape_last_dir, "*.[tT]*",
                              &button, True, NULL);

    switch (button) {
      case UI_BUTTON_OK:
        if (tape_image_attach(1, filename) < 0)
            ui_error(_("Invalid Tape Image"));
        if (attach_tape_last_dir)
            lib_free(attach_tape_last_dir);
        util_fname_split(filename, &attach_tape_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_tape(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Invalid Tape Image"));
        if (attach_tape_last_dir)
            lib_free(attach_tape_last_dir);
        util_fname_split(filename, &attach_tape_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

static UI_CALLBACK(detach_tape)
{
    tape_image_detach(1);
}

ui_menu_entry_t uiattach_tape_menu[] = {
    { N_("Attach a tape image..."),
      (ui_callback_t)attach_tape, NULL, NULL,
      XK_t, UI_HOTMOD_META},
    { N_("Detach tape image"),
      (ui_callback_t)detach_tape, NULL, NULL },
    { NULL }
};

static char *read_disk_or_tape_image_contents(const char *fname,
                                              unsigned int unit)
{
    char *tmp;

    tmp = read_disk_image_contents(fname, unit);
    if (tmp)
        return tmp;
    return read_tape_image_contents(fname, unit);
}

static char *smart_attach_last_dir = NULL;

static UI_CALLBACK(smart_attach)
{
    char *filename;
    ui_button_t button;
    int is_unknown_image;
    int do_free_dir;
    char *dir;

    vsync_suspend_speed_eval();

    if (smart_attach_last_dir) {
        dir = smart_attach_last_dir;
        do_free_dir = 0;
    }
    else {
        dir = ioutil_current_dir();
        do_free_dir = 1;
    }
    filename = ui_select_file(_("Smart-attach a file"),
                              read_disk_or_tape_image_contents, 0,
                              True, dir, NULL, &button, True, NULL);
    if (do_free_dir)
        lib_free(dir);

    switch (button) {
      case UI_BUTTON_OK:
        is_unknown_image = 0;
        if (file_system_attach_disk(8, filename) < 0
            && tape_image_attach(1, filename) < 0) {
            is_unknown_image = 1;
        }
        if (is_unknown_image &&
            autostart_prg(filename, AUTOSTART_MODE_RUN) != 0) {
            ui_error(_("Unknown image type"));
        }
        if (smart_attach_last_dir)
            lib_free(smart_attach_last_dir);
        util_fname_split(filename, &smart_attach_last_dir, NULL);
        break;
      case UI_BUTTON_AUTOSTART:
        if (autostart_autodetect(filename, NULL, selection_from_image,
            AUTOSTART_MODE_RUN) < 0)
            ui_error(_("Unknown image type"));
        if (smart_attach_last_dir)
            lib_free(smart_attach_last_dir);
        util_fname_split(filename, &smart_attach_last_dir, NULL);
        break;
      default:
        /* Do nothing special.  */
        break;
    }
    if (filename != NULL)
        lib_free(filename);
}

ui_menu_entry_t uiattach_smart_attach_menu[] = {
    { N_("Smart-attach disk/tape..."),
      (ui_callback_t)smart_attach, NULL, NULL,
      XK_a, UI_HOTMOD_META },
    { NULL }
};

void uiattach_shutdown(void)
{
    lib_free(attach_disk_last_dir);
    lib_free(attach_tape_last_dir);
    lib_free(smart_attach_last_dir);
}

