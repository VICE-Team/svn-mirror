/*
 * uiscreenshot.c - Screenshot UI.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Daniel Sladic <dsladic@cs.cmu.edu>
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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/* Xaw or Xaw3d */
#ifdef USE_XAW3D
#include <X11/Xaw3d/Box.h>
#include <X11/Xaw3d/Command.h>
#include <X11/Xaw3d/Form.h>
#include <X11/Xaw3d/MenuButton.h>
#include <X11/Xaw3d/Paned.h>
#include <X11/Xaw3d/SimpleMenu.h>
#include <X11/Xaw3d/SmeBSB.h>
#include <X11/Xaw3d/Toggle.h>
#else
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Toggle.h>
#endif

#ifndef ENABLE_TEXTFIELD
#ifdef USE_XAW3D
#include <X11/Xaw3d/AsciiText.h>
#else
#include <X11/Xaw/AsciiText.h>
#endif
#else
#include "widgets/TextField.h"
#endif

#include "gfxoutput.h"
#include "lib.h"
#include "screenshot.h"
#include "uilib.h"
#include "uimenu.h"
#include "uiscreenshot.h"
#include "util.h"
#include "uiapi.h"

static Widget screenshot_dialog;
static Widget screenshot_dialog_pane;
static Widget file_name_form;
static Widget file_name_label;
static Widget file_name_field;
static Widget browse_button;
static Widget options_form;
static Widget options_filling_box_left;

/* static Widget options_filling_box_right; */
static Widget *driver_buttons;
static Widget driver_label;

static Widget button_box;
static Widget save_button;
static Widget cancel_button;

static char *screenshot_file_name;

#define FILL_BOX_WIDTH        10
#define OPTION_LABELS_WIDTH   50
#define OPTION_LABELS_JUSTIFY XtJustifyLeft

extern Widget rec_button;

static UI_CALLBACK(browse_callback)
{
    ui_button_t button;
    uilib_file_filter_enum_t filter = UILIB_FILTER_ALL;
    char *filename;

    filename = ui_select_file(_("Save screenshot file"), NULL, False, NULL, &filter, 1, &button, 0, NULL, UI_FC_LOAD);

    if (button == UI_BUTTON_OK) {
        XtVaSetValues(file_name_field, XtNstring, filename, NULL);
    }

    lib_free(filename);
}

static UI_CALLBACK(cancel_callback)
{
    ui_popdown(screenshot_dialog);
}

static UI_CALLBACK(save_callback)
{
    int i, num_buttons;
    struct video_canvas_s *canvas;
    String name;
    Boolean driver_flag;
    gfxoutputdrv_t *driver;
    char *tmp;
    
    ui_popdown(screenshot_dialog);
    canvas = (struct video_canvas_s *)UI_MENU_CB_PARAM;

    num_buttons = gfxoutput_num_drivers();
    driver = gfxoutput_drivers_iter_init();

    for (i = 0; i < num_buttons; i++) {
        XtVaGetValues(driver_buttons[i], XtNstate, &driver_flag, NULL);
        if (driver_flag) {
           break;
        }

        driver = gfxoutput_drivers_iter_next();
    }

    if (!driver) {
        return;
    }

    XtVaGetValues(file_name_field, XtNstring, &name, NULL);
    tmp = lib_stralloc(name);   /* Make a copy of widget-managed string */
    if (!util_get_extension(tmp)) {
        util_add_extension(&tmp, driver->default_extension);
    }
    if (screenshot_save(driver->name, tmp, canvas) < 0) {
        ui_error(_("Couldn't write screenshot to `%s' with driver `%s'."), tmp, driver->name);
        lib_free(tmp);
        return;
    } else {
        if (screenshot_is_recording()) {
            XtManageChild(rec_button);
        }
        /* ui_message(_("Successfully wrote `%s'"), tmp); */
        lib_free(tmp);
    }
}

static void build_screenshot_dialog(struct video_canvas_s *canvas)
{
    int i, num_buttons;
    gfxoutputdrv_t *driver;
#ifndef ENABLE_TEXTFIELD
    static char *text_box_translations = "#override\n<Key>Return: no-op()";
#else
    static char *text_box_translations = "<Btn1Down>: select-start() focus-in()";
#endif

    char *button_title;
    char *filename = util_concat(_("File name"), ":", NULL);
    char *image_format = util_concat(_("Image Format"), ":", NULL);

    if (screenshot_dialog != NULL) {
        return;
    }

    screenshot_dialog = ui_create_transient_shell(_ui_top_level, "screenshotDialog");

    screenshot_dialog_pane = XtVaCreateManagedWidget("screenshotDialogPane",
                                                     panedWidgetClass, screenshot_dialog,
                                                     NULL);

    file_name_form = XtVaCreateManagedWidget("fileNameForm",
                                             formWidgetClass, screenshot_dialog_pane,
                                             XtNshowGrip, False,
                                             NULL);

    file_name_label = XtVaCreateManagedWidget("fileNameLabel",
                                              labelWidgetClass, file_name_form,
                                              XtNjustify, XtJustifyLeft,
                                              XtNlabel, filename,
                                              XtNborderWidth, 0,
                                              NULL);
    lib_free(filename);

#ifndef ENABLE_TEXTFIELD
    file_name_field = XtVaCreateManagedWidget("fileNameField",
                                              asciiTextWidgetClass, file_name_form,
                                              XtNfromHoriz, file_name_label,
                                              XtNwidth, 240,
                                              XtNtype, XawAsciiString,
                                              XtNeditType, XawtextEdit,
                                              NULL);
#else
    file_name_field = XtVaCreateManagedWidget("fileNameField",
                                              textfieldWidgetClass, file_name_form,
                                              XtNfromHoriz, file_name_label,
                                              XtNwidth, 240,
                                              XtNstring, "",         /* Otherwise, it does not work correctly.  */
                                              NULL);
#endif
    XtOverrideTranslations(file_name_field, XtParseTranslationTable(text_box_translations));

    button_title = util_concat(_("Browse"), "...", NULL);
    browse_button = XtVaCreateManagedWidget("browseButton",
                                            commandWidgetClass, file_name_form,
                                            XtNfromHoriz, file_name_field,
                                            XtNlabel, button_title,
                                            NULL);
    lib_free(button_title);
    XtAddCallback(browse_button, XtNcallback, browse_callback, NULL);

    options_form = XtVaCreateManagedWidget("optionsForm",
                                           formWidgetClass, screenshot_dialog_pane,
                                           XtNskipAdjust, True,
                                           NULL);

    driver_label = XtVaCreateManagedWidget("ImageTypeLabel",
                                           labelWidgetClass, options_form,
                                           XtNborderWidth, 0,
                                           XtNfromHoriz, options_filling_box_left,
                                           XtNjustify, OPTION_LABELS_JUSTIFY,
                                           XtNwidth, OPTION_LABELS_WIDTH,
                                           XtNleft, XawChainLeft,
                                           XtNright, XawChainRight,
                                           XtNheight, 20,
                                           XtNlabel, image_format,
                                           NULL);
    lib_free(image_format);

    num_buttons = gfxoutput_num_drivers();
    driver_buttons = lib_malloc(sizeof(Widget) * num_buttons);
    driver = gfxoutput_drivers_iter_init();

    driver_buttons[0] = XtVaCreateManagedWidget(driver->displayname,
                                                toggleWidgetClass, options_form,
                                                XtNfromHoriz, driver_label,
                                                XtNfromVert, browse_button,
                                                XtNwidth, 180,
                                                XtNheight, 20,
                                                XtNright, XtChainRight,
                                                XtNleft, XtChainRight,
                                                XtNlabel, driver->displayname,
                                                NULL);
    driver = gfxoutput_drivers_iter_next();
    for (i = 1; i < num_buttons; i++) {
        driver_buttons[i] = XtVaCreateManagedWidget(driver->displayname,
                                                    toggleWidgetClass, options_form,
                                                    XtNfromHoriz, driver_label,
                                                    XtNfromVert, driver_buttons[i - 1],
                                                    XtNwidth, 180,
                                                    XtNheight, 20,
                                                    XtNright, XtChainRight,
                                                    XtNleft, XtChainRight,
                                                    XtNlabel, driver->displayname,
                                                    XtNradioGroup, driver_buttons[i - 1],
                                                    NULL);
        driver = gfxoutput_drivers_iter_next();
    }

    button_box = XtVaCreateManagedWidget("buttonBox",
                                         boxWidgetClass, screenshot_dialog_pane,
                                         XtNshowGrip, False,
                                         NULL);

    save_button = XtVaCreateManagedWidget("saveButton",
                                          commandWidgetClass, button_box,
                                          XtNlabel, _("Save"),
                                          NULL);
    XtAddCallback(save_button, XtNcallback, save_callback, (XtPointer)canvas);

    cancel_button = XtVaCreateManagedWidget("cancelButton",
                                            commandWidgetClass, button_box,
                                            XtNlabel, _("Cancel"),
                                            NULL);
    XtAddCallback(cancel_button, XtNcallback, cancel_callback, NULL);

    XtVaSetValues(driver_buttons[0], XtNstate, True, NULL);
    XtSetKeyboardFocus(screenshot_dialog_pane, file_name_field);
}

int ui_screenshot_dialog(char *name, struct video_canvas_s *canvas)
{
    screenshot_file_name = name;
    *screenshot_file_name= 0;

    build_screenshot_dialog(canvas);
    ui_popup(screenshot_dialog, _("Screen Snapshot"), True);
    return *name ? 0 : -1;
}

void uiscreenshot_shutdown(void)
{
    lib_free(driver_buttons);
}
