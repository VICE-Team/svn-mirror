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
#include "uimenu.h"
#include "vsync.h"
#include "utils.h"

/* Separator item.  */
ui_menu_entry_t ui_menu_separator[] = {
    { "--" },
    { NULL },
};

static Display *display;
static int screen;

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

/* ------------------------------------------------------------------------- */

/* This makes sure the submenu is fully visible.  */
static void position_submenu(Widget w, Widget parent)
{
    Position parent_x, parent_y, my_x, my_y;
    Dimension parent_width, my_width, my_height;
    int root_width, root_height, foo;
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
    XGetGeometry(display, RootWindow(display, screen), &foowin, &foo,
		 &foo, &root_width, &root_height, &foo, &foo);
    if (my_x + my_width > root_width)
	my_x -= my_width + parent_width - 2;
    if (my_y + my_height > root_height)
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
    static char *p = NULL;
    const char *key_string;
    char *tmp = alloca(1024);

    if (e->hotkey_keysym == (KeySym) 0)
        return e->string;

    if (p != NULL)
        free(p);

    *tmp = '\0';
    if (e->hotkey_modifier & UI_HOTMOD_CTRL)
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

    display = d;
    screen = s;

    checkmark_bitmap = XCreateBitmapFromData(display,
                                             DefaultRootWindow(display),
                                             checkmark_bits,
                                             checkmark_width,
                                             checkmark_height);

    right_arrow_bitmap = XCreateBitmapFromData(display,
                                               DefaultRootWindow(display),
                                               right_arrow_bits,
                                               right_arrow_width,
                                               right_arrow_height);

    XtAppAddActions(app_context, actions, XtNumber(actions));
    XawSimpleMenuAddGlobalActions(app_context);

    return 0;
}

Widget ui_menu_create(const char *name, ...)
{
    static int level = 0;
    Widget w;
    unsigned int i, j;
    ui_menu_entry_t *list;
    va_list ap;

    level++;
    w = XtCreatePopupShell(name, simpleMenuWidgetClass, _ui_top_level,
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

    va_start(ap, name);
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
                new_item = XtVaCreateManagedWidget(name,
                                                   smeBSBObjectClass, w,
                                                   XtNrightMargin, 20,
                                                   XtNleftMargin, 20,
                                                   XtNlabel,
                                                   make_menu_label(&list[i]) + 1,
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
                break;
              default:
                new_item = XtVaCreateManagedWidget(name, smeBSBObjectClass, w,
                                                   XtNleftMargin, 20,
                                                   XtNrightMargin, 20,
                                                   XtNlabel,
                                                   make_menu_label(&list[i]),
                                                   NULL);
                j++;
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
