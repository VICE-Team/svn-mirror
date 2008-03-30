/*
 * uisnapshot.c - Snapshot dialog for the Xaw widget set.
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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/Toggle.h>

#ifndef ENABLE_TEXTFIELD
#include <X11/Xaw/AsciiText.h>
#else
#include "widgets/TextField.h"
#endif

#include "uisnapshot.h"

#include "machine.h"
#include "ui.h"

static Widget snapshot_dialog;
static Widget snapshot_dialog_pane;
static Widget file_name_form;
static Widget file_name_label;
static Widget file_name_field;
static Widget browse_button;
static Widget options_form;
static Widget options_label;
static Widget options_filling_box_left;
static Widget options_filling_box_right;
static Widget save_roms_on_button, save_roms_off_button;
static Widget save_roms_label;
static Widget save_disk_off_button, save_disk_button;
static Widget save_disk_label;
static Widget button_box;
static Widget save_button;
static Widget cancel_button;

#define FILL_BOX_WIDTH          10
#define OPTION_LABELS_WIDTH     50
#define OPTION_LABELS_JUSTIFY   XtJustifyLeft

UI_CALLBACK(browse_callback)
{
    ui_button_t button;

    char *f = ui_select_file("Save snapshot file", NULL, False, NULL, "*",
                             &button);

    if (button == UI_BUTTON_OK)
        XtVaSetValues(file_name_field, XtNstring, f, NULL);
}

UI_CALLBACK(cancel_callback)
{
    ui_popdown(snapshot_dialog);
}

UI_CALLBACK(save_callback)
{
    String name;
    Boolean save_roms;
    Boolean save_disks;
    
    ui_popdown(snapshot_dialog);

    XtVaGetValues(save_roms_on_button, XtNstate, &save_roms, NULL);
    XtVaGetValues(save_disk_button, XtNstate, &save_disks, NULL);
    XtVaGetValues(file_name_field, XtNstring, &name, NULL);
    
    if (machine_write_snapshot (name, save_roms, save_disks) < 0)
        ui_error("Cannot write snapshot file\n`%s'\n", name);
}

static void build_snapshot_dialog(void)
{
    if (snapshot_dialog != NULL)
        return;

    snapshot_dialog = ui_create_transient_shell(_ui_top_level,
                                                "snapshotDialog");

    snapshot_dialog_pane = XtVaCreateManagedWidget
        ("snapshotDialogPane",
         panedWidgetClass, snapshot_dialog,
         NULL);
    
    file_name_form = XtVaCreateManagedWidget
        ("fileNameForm",
         formWidgetClass, snapshot_dialog_pane,
         XtNshowGrip, False, NULL);
    
    file_name_label = XtVaCreateManagedWidget
        ("fileNameLabel",
         labelWidgetClass, file_name_form,
         XtNjustify, XtJustifyLeft,
         XtNlabel, "File name:",
         XtNborderWidth, 0,
         NULL);

#ifndef ENABLE_TEXTFIELD
    file_name_field = XtVaCreateManagedWidget
        ("fileNameField",
         asciiTextWidgetClass, file_name_form,
         XtNfromHoriz, file_name_label,
         XtNwidth, 200,
         XtNtype, XawAsciiString,
         XtNeditType, XawtextEdit,
         NULL);
#else
    file_name_field = XtVaCreateManagedWidget
        ("fileNameField",
         textfieldWidgetClass, file_name_form,
         XtNfromHoriz, file_name_label,
         XtNwidth, 200,
         XtNstring, "",         /* Otherwise, it does not work correctly.  */
         NULL);
#endif

    browse_button = XtVaCreateManagedWidget
        ("browseButton",
         commandWidgetClass, file_name_form,
         XtNfromHoriz, file_name_field,
         XtNlabel, "Browse...",
         NULL);
    XtAddCallback(browse_button, XtNcallback, browse_callback, NULL);

    options_form = XtVaCreateManagedWidget
        ("optionsForm",
         formWidgetClass, snapshot_dialog_pane,
         XtNskipAdjust, True,
         NULL);

    options_label = XtVaCreateManagedWidget
        ("optionsLabel",
         labelWidgetClass, options_form,
         XtNborderWidth, 0,
         XtNjustify, XtJustifyLeft,
         XtNlabel, "Snapshot options:",
         XtNleft, XawChainLeft,
         NULL);

    options_filling_box_left = XtVaCreateManagedWidget
        ("fillingBoxLeft",
         boxWidgetClass, options_form,
         XtNborderWidth, 0,
         XtNleft, XawChainLeft,
         XtNright, XawChainLeft,
         XtNfromVert, options_label,
         XtNwidth, FILL_BOX_WIDTH,
         NULL);

    save_disk_label = XtVaCreateManagedWidget
        ("saveDiskLabel",
         labelWidgetClass, options_form,
         XtNborderWidth, 0,
         XtNfromHoriz, options_filling_box_left,
         XtNfromVert, options_label,
         XtNjustify, OPTION_LABELS_JUSTIFY,
         XtNwidth, OPTION_LABELS_WIDTH,
         XtNleft, XawChainLeft,
         XtNright, XawChainRight,
         XtNheight, 20,
         XtNlabel, "Save currently attached disks:",
         NULL);

    save_disk_off_button = XtVaCreateManagedWidget
        ("saveDiskOffButton",
         toggleWidgetClass, options_form,
         XtNfromHoriz, save_disk_label,
         XtNfromVert, browse_button,
         XtNwidth, 40,
         XtNheight, 20,
         XtNright, XtChainRight,
         XtNleft, XtChainRight,
         XtNlabel, "Off",
         NULL);

    save_disk_button = XtVaCreateManagedWidget
        ("saveDiskButton",
         toggleWidgetClass, options_form,
         XtNfromHoriz, save_disk_off_button,
         XtNfromVert, browse_button,
         XtNwidth, 40,
         XtNheight, 20,
         XtNright, XtChainRight,
         XtNleft, XtChainRight,
         XtNlabel, "On",
         XtNradioGroup, save_disk_off_button,
         NULL);

    save_roms_label = XtVaCreateManagedWidget
        ("saveRomsLabel",
         labelWidgetClass, options_form,
         XtNborderWidth, 0,
         XtNfromHoriz, options_filling_box_left,
         XtNfromVert, save_disk_label,
         XtNjustify, OPTION_LABELS_JUSTIFY,
         XtNwidth, OPTION_LABELS_WIDTH,
         XtNheight, 20,
         XtNleft, XawChainLeft,
         XtNright, XawChainRight,
         XtNlabel, "Save currently loaded ROMs:",
         NULL);

    save_roms_off_button = XtVaCreateManagedWidget
        ("saveRomsOffButton",
         toggleWidgetClass, options_form,
         XtNfromHoriz, save_roms_label,
         XtNfromVert, save_disk_off_button,
         XtNwidth, 40,
         XtNheight, 20,
         XtNright, XtChainRight,
         XtNleft, XtChainRight,
         XtNlabel, "Off",
         NULL);

    save_roms_on_button = XtVaCreateManagedWidget
        ("saveRomsOffButton",
         toggleWidgetClass, options_form,
         XtNfromHoriz, save_roms_off_button,
         XtNfromVert, save_disk_button,
         XtNwidth, 40,
         XtNheight, 20,
         XtNright, XtChainRight,
         XtNleft, XtChainRight,
         XtNlabel, "On",
         XtNradioGroup, save_roms_off_button,
         NULL);

    button_box = XtVaCreateManagedWidget
        ("buttonBox",
         boxWidgetClass, snapshot_dialog_pane,
         XtNshowGrip, False, NULL);

    save_button = XtVaCreateManagedWidget
        ("saveButton",
         commandWidgetClass, button_box,
         XtNlabel, "Save",
         NULL);
    XtAddCallback(save_button, XtNcallback, save_callback, NULL);
    
    cancel_button = XtVaCreateManagedWidget
        ("cancelButton",
         commandWidgetClass, button_box,
         XtNlabel, "Cancel",
         NULL);
    XtAddCallback(cancel_button, XtNcallback, cancel_callback, NULL);

    XtVaSetValues(save_roms_off_button, XtNstate, True, NULL);
    XtVaSetValues(save_disk_off_button, XtNstate, True, NULL);

    XtSetKeyboardFocus(snapshot_dialog_pane, file_name_field);
}

void ui_snapshot_dialog(void)
{
    build_snapshot_dialog();
    ui_popup(snapshot_dialog, "Save snapshot", True);
}
