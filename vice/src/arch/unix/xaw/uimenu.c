/*
 * uimenu.c - Simple and ugly cascaded pop-up menu implementation.
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

/* Warning: this code sucks.  It does work, but it sucks.  */

#include "vice.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/SmeBSB.h>

#include "checkmark.xbm"
#include "right_arrow.xbm"

#include "resources.h"
#include "utils.h"
#include "vsync.h"

#include "uimenu.h"

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

/* This keeps a list of the menus with a checkmark on the left.  Each time
   some setting is changed, we have to update them. */
#define MAX_UPDATE_MENU_LIST_SIZE 1024
static Widget checkmark_menu_items[MAX_UPDATE_MENU_LIST_SIZE];
static int num_checkmark_menu_items = 0;

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
    suspend_speed_eval();
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

/* Yes, this sucks.  Sorry.  */
static void position_submenu_action(Widget w, XEvent * event,
                                    String * params, Cardinal * num_params)
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

static void popdown_submenus_action(Widget w, XEvent * event,
                                    String * params, Cardinal * num_params)
{
    int i;

    if (menu_popup == 0)
        return;

    /* Pop down all the submenus and the top ones.  */

    for (i = 0; i < num_submenus; i++)
	XtPopdown(submenus[i].widget);

    XtPopdown(top_menu);
    top_menu = NULL;

#ifdef USE_VIDMODE_EXTENSION
    ui_set_mouse_timeout();
#endif
    menu_popup = 0;
}

static void menu_unhighlight_action(Widget w, XEvent * event, String * params,
                                    Cardinal * num_params)
{
    XtCallActionProc(w, "unhighlight", event, params, *num_params);
}

/* ------------------------------------------------------------------------- */

static char *make_menu_label(ui_menu_entry_t *e)
{
    const char *key_string;
    char *tmp = alloca(1024);

    if (e->hotkey_keysym == (KeySym) 0)
        return stralloc(e->string);

    *tmp = '\0';
    if (e->hotkey_modifier & UI_HOTMOD_CONTROL)
        strcat(tmp, "C-");
    if (e->hotkey_modifier & UI_HOTMOD_META)
        strcat(tmp, "M-");
    if (e->hotkey_modifier & UI_HOTMOD_ALT)
        strcat(tmp, "A-");
    if (e->hotkey_modifier & UI_HOTMOD_SHIFT)
        strcat(tmp, "S-");

    key_string = strchr(XKeysymToString(e->hotkey_keysym), '_');
    if (key_string == NULL)
        key_string = XKeysymToString(e->hotkey_keysym);
    else
        key_string++;

    return concat(e->string, "    (", tmp, key_string, ")", NULL);
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

    ui_create_dynamic_menues();

    return 0;
}

Widget ui_menu_create(const char *menu_name, ...)
{
    static int level = 0;
    Widget w;
    unsigned int i, j;
    ui_menu_entry_t *list;
    va_list ap;


    level++;
    w = XtCreatePopupShell(menu_name, simpleMenuWidgetClass, _ui_top_level,
                           NULL, 0);
    if (level == 1) {
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
            Widget new_item;
            char name[256];

            sprintf(name, "MenuItem%d", j);	/* ugly... */
            switch (*list[i].string) {
              case '-':		/* line */
                new_item = XtCreateManagedWidget("separator",
                                                 smeLineObjectClass, w,
                                                 NULL, 0);
                break;
              case '*':		/* toggle */
                {
                    char *label = make_menu_label(&list[i]);
                    
                    new_item = XtVaCreateManagedWidget(name,
                                                       smeBSBObjectClass, w,
                                                       XtNrightMargin, 20,
                                                       XtNleftMargin, 20,
                                                       XtNlabel,
                                                       label + 1,
                                                       NULL);
                    /* Add this item to the list of calls to perform to update the
                       menu status. */
                    if (list[i].callback) {
                        if (num_checkmark_menu_items < MAX_UPDATE_MENU_LIST_SIZE)
                            checkmark_menu_items[num_checkmark_menu_items++] = new_item;
                        else {
                            fprintf(stderr,
                                    "Maximum number of menus reached!  "
                                    "Please fix the code.\n");
                            exit(-1);
                        }
                    }
                    j++;

                    free(label);
                }
                break;
              default:
                {
                    char *label = make_menu_label(&list[i]);
                    
                    new_item = XtVaCreateManagedWidget(name, smeBSBObjectClass, w,
                                                       XtNleftMargin, 20,
                                                       XtNrightMargin, 20,
                                                       XtNlabel,
                                                       label,
                                                       NULL);
                    free(label);
                    j++;
                }
            }
            if (list[i].callback)
                XtAddCallback(new_item, XtNcallback,
                              (XtCallbackProc) list[i].callback,
                              list[i].callback_data);
            if (list[i].sub_menu) {
                Widget sub;

                if (num_submenus > MAX_SUBMENUS) {
                    fprintf(stderr,
                            "Maximum number of sub menus reached! "
                            "Please fix the code.\n");
                    exit(-1);
                }
                XtVaSetValues(new_item, XtNrightBitmap, right_arrow_bitmap,
                              NULL);
                sub = ui_menu_create("SUB", list[i].sub_menu, NULL);
                submenus[num_submenus].widget = sub;
                submenus[num_submenus].parent = new_item;
                submenus[num_submenus].level = level;
                XtAddCallback(sub,
                              XtNpopupCallback, submenu_popup_callback,
                              submenus + num_submenus);
                XtAddCallback(sub,
                              XtNpopdownCallback, submenu_popdown_callback,
                              (XtPointer) w);
                num_submenus++;
            } else {            /* no submenu */
                if (list[i].hotkey_keysym != (KeySym) 0
                    && list[i].callback != NULL)
                    ui_hotkey_register(list[i].hotkey_modifier,
                                       list[i].hotkey_keysym,
                                       list[i].callback,
                                       list[i].callback_data);
            }
        }
    }

    level--;

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
			XtNcallback, (XtPointer) !NULL);
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

    if (resources_get_value(resource_name,
                            (resource_value_t *) &current_value) < 0)
        return;

    if (!call_data) {
        resources_set_value(resource_name, (resource_value_t) !current_value);
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

    resources_get_value(resource_name, (resource_value_t *) &current_value);

    if (!call_data) {
        if (current_value != (int) client_data) {
            resources_set_value(resource_name,
                                (resource_value_t) client_data);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, current_value == (int) client_data);
    }
}

void _ui_menu_string_radio_helper(Widget w, 
                                  ui_callback_data_t client_data,
                                  ui_callback_data_t call_data,
                                  const char *resource_name)
{
    resource_value_t current_value;

    resources_get_value(resource_name, &current_value);

    if( current_value == 0) return;

    if (!call_data) {
        if (strcmp((const char *) current_value,
                   (const char *) client_data) != 0) {
            resources_set_value(resource_name, (resource_value_t) client_data);
            ui_update_menus();
        }
    } else {
        ui_menu_set_tick(w, strcmp((const char *) current_value,
                                   (const char *) client_data) == 0);
    }
}
