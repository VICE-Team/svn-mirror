/*
 * uimenu.c - Simple and ugly cascaded pop-up menu implementation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
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

/* Warning: this code sucks.  It does work, but it sucks.  */

#include "vice.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>

#include "checkmark.xbm"
#include "right_arrow.xbm"

#include "fullscreenarch.h"
#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "uiarch.h"
#include "uimenu.h"
#include "x11menu.h"
#include "util.h"


/* Separator item.  */
ui_menu_entry_t ui_menu_separator[] = {
    { "--" },
    { NULL },
};

/* Bitmaps for the menus: "tick" and right arrow (for submenus).  */
static Pixmap checkmark_bitmap, right_arrow_bitmap;

static int menu_popup = 0;

#define MAX_SUBMENUS 1024
static struct {
    Widget widget;
    Widget parent;
    int level;
} submenus[MAX_SUBMENUS];

static int num_submenus = 0;

static Widget active_submenu, active_entry;

static int submenu_popped_up = 0;

static Widget top_menu;

static Widget *checkmark_menu_items = NULL;
static int num_checkmark_menu_items_max = 0;
int num_checkmark_menu_items = 0;

static Display *my_display;
static int my_screen;

/* ------------------------------------------------------------------------- */

/* This makes sure the submenu is fully visible.  */
static void position_submenu(Widget w, Widget parent)
{
    Position parent_x, parent_y, my_x, my_y;
    Dimension parent_width, my_width, my_height;
    int foo;
    unsigned int root_width, root_height, ufoo;
    Window foowin;

    /* Make sure the widget is realized--otherwise, we get 0 as width and
       height.  */
    XtRealizeWidget(w);

    XtVaGetValues(parent, XtNx, &parent_x, XtNy, &parent_y,
                  XtNwidth, &parent_width, NULL);
    XtVaGetValues(w, XtNwidth, &my_width, XtNheight, &my_height, NULL);
    XtTranslateCoords(XtParent(parent), parent_x, parent_y,
                      &parent_x, &parent_y);
    my_x = parent_x + parent_width - 2;
    my_y = parent_y + 1;
    XGetGeometry(my_display, RootWindow(my_display, my_screen), &foowin, &foo,
                 &foo, &root_width, &root_height, &ufoo, &ufoo);
    if (my_x + my_width > (int) root_width)
        my_x -= my_width + parent_width - 2;
    if (my_y + my_height > (int) root_height)
        my_y = root_height - my_height;
    XtVaSetValues(w, XtNx, my_x, XtNy, my_y, NULL);
    XtPopup(w, XtGrabNonexclusive);
}

static UI_CALLBACK(menu_popup_callback)
{
    if (menu_popup == 0)
        top_menu = w;
    menu_popup++;
}

static UI_CALLBACK(menu_popdown_callback)
{
    if (menu_popup > 0)
        menu_popup--;
    else
        top_menu = NULL;
}

static UI_CALLBACK(submenu_popup_callback)
{
    submenu_popped_up++;
}

static UI_CALLBACK(submenu_popdown_callback)
{
    submenu_popped_up--;
    if (XawSimpleMenuGetActiveEntry(w))
        XtPopdown((Widget)client_data);
}

/* ------------------------------------------------------------------------- */

typedef struct {
    ui_hotkey_modifier_t modifier;
    ui_keysym_t keysym;
    ui_callback_t callback;
    ui_callback_data_t client_data;
} registered_hotkey_t;

static registered_hotkey_t *registered_hotkeys = NULL;
static int num_registered_hotkeys;
static int num_allocated_hotkeys;


/* ------------------------------------------------------------------------- */

static void ui_hotkey_register(ui_hotkey_modifier_t modifier, signed long keysym,
                        void *callback, void *client_data)
{
    registered_hotkey_t *p;

    if (registered_hotkeys == 0) {
        num_allocated_hotkeys = 32;
        registered_hotkeys = lib_malloc(num_allocated_hotkeys
                                        * sizeof(registered_hotkey_t));
        num_registered_hotkeys = 0;
    } else if (num_registered_hotkeys == num_allocated_hotkeys) {
        num_allocated_hotkeys *= 2;
        registered_hotkeys = lib_realloc(registered_hotkeys,
                                         (num_allocated_hotkeys
                                         * sizeof(registered_hotkey_t)));
    }

    p = registered_hotkeys + num_registered_hotkeys;

    p->modifier = modifier;
    p->keysym = (ui_keysym_t)keysym;
    p->callback = (ui_callback_t)callback;
    p->client_data = (ui_callback_data_t)client_data;

    num_registered_hotkeys++;
}

/* ------------------------------------------------------------------------- */

int ui_dispatch_hotkeys(int key)
{
    int i, ret = 0;
    registered_hotkey_t *p = registered_hotkeys;

    /* XXX: Notice that we don't actually check the hotkey modifiers
       here.  */
    for (i = 0; i < num_registered_hotkeys; i++, p++) {
	if (p->keysym == key) {
	    ((void *(*)(void *, void *, void *))
	     p->callback)(NULL, p->client_data, NULL);
	    ret = 1;
	    break;
	}
    }
    return ret;
}

/* ------------------------------------------------------------------------- */

/* Yes, this sucks.  Sorry.  */
static void position_submenu_action(Widget w, XEvent *event,
                                    String *params, Cardinal *num_params)
{
    Widget new_active_submenu, new_active_entry;

    new_active_entry = XawSimpleMenuGetActiveEntry(w);
    if (new_active_entry != active_entry) {
        int i, level;
        int level_found, active_found;

        new_active_submenu = NULL;

        /* Find the submenu for the current active menu item and the level of
           this submenu.  */
        for (level_found = active_found = 0, level = 0, i = 0;
             i < num_submenus && !(level_found && active_found);
             i++) {
            if (!active_found && submenus[i].parent == new_active_entry) {
                new_active_submenu = submenus[i].widget;
                active_found = 1;
            }
            if (!level_found && submenus[i].widget == w) {
                level = submenus[i].level;
                level_found = 1;
            }
        }

        /* Remove all the submenus whose level is higher than this submenu.  */
        for (i = 0; i < num_submenus; i++) {
            if (submenus[i].level > level)
                XtPopdown(submenus[i].widget);
        }

        /* Position the submenu for this menu item.  */
        if (new_active_submenu != NULL && new_active_entry != NULL)
            position_submenu(new_active_submenu, new_active_entry);

        active_submenu = new_active_submenu;
        active_entry = new_active_entry;
    }
}

static void popdown_submenus_action(Widget w, XEvent *event,
                                    String *params, Cardinal *num_params)
{
    int i;

    if (menu_popup == 0)
        return;

    /* Pop down all the submenus and the top ones.  */

    for (i = 0; i < num_submenus; i++)
        XtPopdown(submenus[i].widget);

    XtPopdown(top_menu);
    top_menu = NULL;

#ifdef USE_XF86_EXTENSIONS
    fullscreen_set_mouse_timeout();
#endif
    menu_popup = 0;
}

static void menu_unhighlight_action(Widget w, XEvent *event, String *params,
                                    Cardinal *num_params)
{
    XtCallActionProc(w, "unhighlight", event, params, *num_params);
}

static char *make_menu_label(ui_menu_entry_t *e)
{
    const char *key_string, *tmp = "";
    char *retstr, *trans;

    /* Check wether NO_TRANS prefix is there, if yes don't translate it */
    if (strncmp(e->string, NO_TRANS, strlen(NO_TRANS)) == 0)
        trans = lib_stralloc(e->string + strlen(NO_TRANS));
    else
        trans = lib_stralloc(_(e->string));

    if (e->hotkey_keysym == KEYSYM_NONE)
        return trans;

    if (e->hotkey_modifier & UI_HOTMOD_CONTROL)
        tmp = "C-";
    if (e->hotkey_modifier & UI_HOTMOD_META)
        tmp = "M-";
    if (e->hotkey_modifier & UI_HOTMOD_ALT)
        tmp = "A-";
    if (e->hotkey_modifier & UI_HOTMOD_SHIFT)
        tmp = "S-";

    key_string = strchr(XKeysymToString(e->hotkey_keysym), '_');
    if (key_string == NULL)
        key_string = XKeysymToString(e->hotkey_keysym);
    else
        key_string++;

    retstr = util_concat(trans, "    (", tmp, key_string, ")", NULL);

    lib_free(trans);

    return retstr;
}

/* ------------------------------------------------------------------------- */

int ui_menu_init(XtAppContext app_context, Display *d, int s)
{
    static XtActionsRec actions[] = {
	{ "PositionSubmenu", position_submenu_action },
	{ "Popdownsubmenus", popdown_submenus_action },
	{ "Unhighlight", menu_unhighlight_action }
    };

    my_display = d;
    my_screen = s;

    checkmark_bitmap = XCreateBitmapFromData(my_display,
                                             DefaultRootWindow(my_display),
                                             (char *)checkmark_bits,
                                             checkmark_width,
                                             checkmark_height);

    right_arrow_bitmap = XCreateBitmapFromData(my_display,
                                               DefaultRootWindow(my_display),
                                               (char *)right_arrow_bits,
                                               right_arrow_width,
                                               right_arrow_height);

    XtAppAddActions(app_context, actions, XtNumber(actions));
    XawSimpleMenuAddGlobalActions(app_context);

    if (registered_hotkeys != NULL) {
        lib_free(registered_hotkeys);
        num_registered_hotkeys = num_allocated_hotkeys = 0;
    }

    return 0;
}

Widget ui_menu_create(const char *menu_name, ...)
{
    static int level = 0, menulevel = 0;
    static Widget w;
    unsigned int i, j;
    ui_menu_entry_t *list;
    va_list ap;


    level++;
    if (level == 1) {
        w = ui_create_shell(_ui_top_level, menu_name, simpleMenuWidgetClass);
        menulevel++;
        XtAddCallback(w, XtNpopupCallback, menu_popup_callback, NULL);
        XtAddCallback(w, XtNpopdownCallback, menu_popdown_callback, NULL);
    }
    XtOverrideTranslations
        (w, XtParseTranslationTable
         ("<BtnMotion>: highlight() PositionSubmenu()\n"
          "@Num_Lock<BtnMotion>: highlight() PositionSubmenu()\n"
          "<LeaveWindow>: Unhighlight()\n"
          "<BtnUp>: Popdownsubmenus() MenuPopdown() notify() unhighlight()"));

    va_start(ap, menu_name);
    while ((list = va_arg(ap, ui_menu_entry_t *)) != NULL) {
        for (i = j = 0; list[i].string; i++) {
            Widget new_item = NULL;
            char *name;

            name = lib_msprintf("MenuItem%d", j);
            switch (*list[i].string) {
              case '-':         /* line */
                new_item = XtCreateManagedWidget("separator",
                                                 smeLineObjectClass, w,
                                                 NULL, 0);
                break;
              case '*':         /* toggle */
                {
                    char *label = make_menu_label(&list[i]);

                    new_item = XtVaCreateManagedWidget(name,
                                                       smeBSBObjectClass, w,
                                                       XtNrightMargin, 20,
                                                       XtNleftMargin, 20,
                                                       XtNlabel,
                                                       label + 1,
                                                       NULL);
                    /* Add this item to the list of calls to perform to update
                       the menu status. */
                    if (list[i].callback) {
                        if (num_checkmark_menu_items >=
                            num_checkmark_menu_items_max) {
                            num_checkmark_menu_items_max += 100;
                            checkmark_menu_items = lib_realloc(
                                checkmark_menu_items,
                                num_checkmark_menu_items_max
                                * sizeof(Widget));
                        }
                        checkmark_menu_items[num_checkmark_menu_items++]
                            = new_item;
                    }
                    j++;

                    lib_free(label);
                }
                break;
              case 0:
                break;
              default:
                {
                    char *label = make_menu_label(&list[i]);

                    new_item = XtVaCreateManagedWidget(name,
                                                       smeBSBObjectClass, w,
                                                       XtNleftMargin, 20,
                                                       XtNrightMargin, 20,
                                                       XtNlabel,
                                                       label,
                                                       NULL);
                    lib_free(label);
                    j++;
                }
            }
            lib_free(name);

            if (list[i].callback)
                XtAddCallback(new_item, XtNcallback,
                              (XtCallbackProc) list[i].callback,
                              list[i].callback_data);
            if (list[i].sub_menu) {
                if (num_submenus > MAX_SUBMENUS) {
                    fprintf(stderr,
                            "Maximum number of sub menus reached! "
                            "Please fix the code.\n");
                    exit(-1);
                }
                if (new_item != NULL && *list[i].string != '-') {
                    Widget oldw = w;

                    XtVaSetValues(new_item, XtNrightBitmap, right_arrow_bitmap,
                              NULL);
                    w = ui_create_shell(_ui_top_level, "SUB", simpleMenuWidgetClass);
                    menulevel++;

                    ui_menu_create("SUB", list[i].sub_menu, NULL);
                    submenus[num_submenus].widget = w;
                    submenus[num_submenus].parent = new_item;
                    submenus[num_submenus].level = menulevel;
                    XtAddCallback(w,
                              XtNpopupCallback, submenu_popup_callback,
                              submenus + num_submenus);
                    XtAddCallback(w,
                              XtNpopdownCallback, submenu_popdown_callback,
                              (XtPointer) w);
                    num_submenus++;

                    menulevel--;
                    w = oldw;
                }
                else {
                    ui_menu_create("SUB", list[i].sub_menu, NULL);
                }
            } else {            /* no submenu */
                if (list[i].hotkey_keysym != (KeySym) 0
                    && list[i].callback != NULL)
                    ui_hotkey_register(list[i].hotkey_modifier,
                                       (signed long)list[i].hotkey_keysym,
                                       (ui_callback_t)list[i].callback,
                                       (ui_callback_data_t)list[i].callback_data);
            }
        }
    }

    level--;
    if (level == 0)
        menulevel = 0;

#ifdef UI_MENU_DEBUG
    fprintf(stderr, "num_checkmark_menu_items: %d\tnum_submenus = %d.\n",
            num_checkmark_menu_items, num_submenus);
#endif
    va_end(ap);
    return w;
}

int ui_menu_any_open(void)
{
    return menu_popup;
}

void ui_menu_update_all(void)
{
    int i;

    for (i = 0; i < num_checkmark_menu_items; i++)
        XtCallCallbacks(checkmark_menu_items[i],
                        XtNcallback, (XtPointer)!NULL);
}

void ui_menu_set_tick(Widget w, int flag)
{
    XtVaSetValues(w, XtNleftBitmap, flag ? checkmark_bitmap : 0, NULL);
}

void ui_menu_set_sensitive(Widget w, int flag)
{
    XtVaSetValues(w, XtNsensitive, flag, NULL);
}

/* ------------------------------------------------------------------------- */

/* These functions are called by radio and toggle menu items if the callback
   functions are defined through `UI_MENU_DEFINE_TOGGLE()',
   `UI_MENU_DEFINE_RADIO()' or `UI_MENU_DEFINE_STRING_RADIO()'.  */

void _ui_menu_toggle_helper(Widget w,
                            ui_callback_data_t client_data,
                            ui_callback_data_t call_data,
                            const char *resource_name)
{
    int current_value;

    if (resources_get_int(resource_name, &current_value) < 0)
        return;

    if (!call_data) {
        resources_set_int(resource_name, !current_value);
        ui_update_menus();
    } else {
        ui_menu_set_tick(w, current_value);
    }
}

void _ui_menu_radio_helper(Widget w,
                           ui_callback_data_t client_data,
                           ui_callback_data_t call_data,
                           const char *resource_name)
{
    int current_value;

    resources_get_int(resource_name, &current_value);

    if (!call_data) {
        if (current_value != (int)client_data) {
            resources_set_int(resource_name, (int)client_data);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int)client_data);
    }
}

void _ui_menu_string_radio_helper(Widget w,
                                  ui_callback_data_t client_data,
                                  ui_callback_data_t call_data,
                                  const char *resource_name)
{
    const char *current_value;

    resources_get_string(resource_name, &current_value);

    if (current_value == 0)
        return;

    if (!call_data) {
        if (strcmp(current_value, (const char *)client_data) != 0) {
            resources_set_string(resource_name, (const char *)client_data);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, strcmp(current_value,
                         (const char *)client_data) == 0);
    }
}

void uimenu_shutdown(void)
{
    lib_free(registered_hotkeys);
    lib_free(checkmark_menu_items);
}

