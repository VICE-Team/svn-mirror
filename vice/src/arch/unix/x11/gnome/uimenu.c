/*
 * uimenu.c - Simple and ugly cascaded pop-up menu implementation.
 *
 * Written by
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Oliver Schaertel GTK+ port
 *  Martin Pottendorfer Gnome port
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

#include "lib.h"
#include "machine.h"
#include "resources.h"
#include "x11menu.h"
#include "uimenu.h"


/* Separator item.  */
ui_menu_entry_t ui_menu_separator[] = {
    { "--" },
    { NULL },
};

static int menu_popup = 0;

#if 0
#define MAX_SUBMENUS 1024
static struct {
    GtkWidget* widget;
    GtkWidget* parent;
    int level;
} submenus[MAX_SUBMENUS];
#endif

/* This keeps a list of the menus with a checkmark on the left.  Each time
   some setting is changed, we have to update them. */
#define MAX_UPDATE_MENU_LIST_SIZE 1024
typedef struct {
    char *name;
#ifdef GNOME_MENUS
    GnomeUIInfo *uiinfo;
#else
    GtkWidget *w;
#endif
    ui_callback_t cb;
    ui_menu_cb_obj obj;
    gint handlerid;
} checkmark_t;

static GList *checkmark_list = NULL;

int num_checkmark_menu_items = 0; /* !static because vsidui needs it. ugly! */

/* ------------------------------------------------------------------------- */

int ui_menu_init()
{
    return(0);
}

static void delete_checkmark_cb(GtkWidget *w, gpointer data)
{
    checkmark_t *cm;
    
    cm = (checkmark_t *) data;
    checkmark_list = g_list_remove(checkmark_list, data);
    lib_free(cm->name);
    lib_free(cm);
}

static void add_accelerator(GtkWidget *w, GtkAccelGroup *accel, guint accel_key, ui_hotkey_modifier_t mod)
{
    GtkAccelFlags flags = 0;

    if (mod & UI_HOTMOD_CONTROL)
        flags |= GDK_CONTROL_MASK;
    if (mod & UI_HOTMOD_META)
        flags |= GDK_MOD1_MASK;
    if (mod & UI_HOTMOD_SHIFT)
        flags |= GDK_SHIFT_MASK;

    gtk_widget_add_accelerator(w, "activate",
                               accel, accel_key,
                               flags, GTK_ACCEL_VISIBLE);
}

void ui_menu_create(GtkWidget *w, GtkAccelGroup *accel, const char *menu_name, ui_menu_entry_t *list)
{
    static int level = 0;
    unsigned int i, j;
    ui_menu_cb_obj *obj = NULL;

    level++;

#ifdef DEBUG_MENUS
    printf("allocate new: %s\t(%p)\t%s\n",
	   gtk_type_name(GTK_WIDGET_TYPE(w)), w,
	   menu_name);
#endif

        for (i = j = 0; list[i].string; i++) {
            GtkWidget *new_item = NULL;
            int do_right_justify = 0;

            char name[256];

            sprintf(name, "MenuItem%d", j);	/* ugly... */
            switch (*list[i].string) 
	    {
	    case '-':		/* line */
		new_item  = gtk_menu_item_new();
                break;
	    case '*':		/* toggle */
	    {
		/* Add this item to the list of calls to perform to update the
		   menu status. */
		if (list[i].callback) 
		{
		    checkmark_t *cmt;
		    new_item = gtk_check_menu_item_new_with_label(list[i].string+1);
		    
		    cmt = (checkmark_t *)lib_malloc(sizeof(checkmark_t));
		    cmt->name = lib_stralloc(list[i].string+1);
		    cmt->w = new_item;
		    cmt->cb = list[i].callback;
		    cmt->obj.value = (void*) list[i].callback_data;
		    cmt->obj.status = CB_NORMAL;
		    cmt->handlerid = 
			g_signal_connect(G_OBJECT(new_item),"activate",
					 G_CALLBACK(list[i].callback),
					 (gpointer) &(cmt->obj)); 
		    g_signal_connect(G_OBJECT(new_item), "destroy",
				     G_CALLBACK(delete_checkmark_cb),
				     (gpointer) cmt);
		    checkmark_list = g_list_prepend(checkmark_list, cmt);
		    obj = &cmt->obj;
		} 
		else 
		    new_item = gtk_menu_item_new_with_label(list[i].string+1);

		j++;
		break;
	    }
            case 0:
                break;
	    default:
	    {
                const char *item = list[i].string;
                if (strncmp(item, "RJ", 2) == 0)
                {
                    do_right_justify = 1;
                    item += 2;
                }

		new_item = gtk_menu_item_new_with_label(item);
		if (list[i].callback) {
		    obj = (ui_menu_cb_obj*)lib_malloc(sizeof(ui_menu_cb_obj));
		    obj->value = (void*) list[i].callback_data;
		    
		    g_signal_connect(G_OBJECT(new_item),"activate",
				     G_CALLBACK(list[i].callback),
				     (gpointer) obj); 
		}
		j++;
	    }
            }

            if (new_item)
            {
	        gtk_menu_shell_append(GTK_MENU_SHELL(w), new_item);
	        gtk_widget_show(new_item);
                if (do_right_justify)
                    gtk_menu_item_set_right_justified(GTK_MENU_ITEM(new_item), TRUE);
#ifdef DEBUG_MENUS
	    printf("allocate new: %s\t(%p)\t%s\n",
		   gtk_type_name(GTK_WIDGET_TYPE(new_item)), new_item,
		   list[i].string);
#endif
            }

            if (list[i].sub_menu) 
	    {
                GtkWidget *sub;
                if (new_item && *list[i].string != '-')
                {
                    sub = gtk_menu_new();
		    gtk_menu_item_set_submenu(GTK_MENU_ITEM(new_item),sub);
                }
                else
                {
                    sub = w;
                }
		ui_menu_create(sub, accel, list[i].string, 
				     list[i].sub_menu);
            } 
	    else 
	    {            /* no submenu */
	        if (accel && list[i].hotkey_keysym != (KeySym) 0
		    && list[i].callback != NULL && new_item != NULL)
                    add_accelerator(new_item,
                               accel, list[i].hotkey_keysym,
                               list[i].hotkey_modifier);
            }
        }
    
    level--;
}

int ui_menu_any_open(void)
{
    return menu_popup;
}

static void menu_handle_block(gpointer data, gpointer user_data)
{
    checkmark_t *cm = (checkmark_t *)data;

    if (user_data)
	g_signal_handler_block(GTK_OBJECT(cm->w), cm->handlerid);
    else
	g_signal_handler_unblock(GTK_OBJECT(cm->w), cm->handlerid);
}

static void menu_update_checkmarks(gpointer data, gpointer user_data)
{
    checkmark_t *cm = (checkmark_t *) data;
    
    cm->obj.status = CB_REFRESH;
    ((void*(*)(GtkWidget*, ui_callback_data_t))
     cm->cb)(cm->w, (ui_callback_data_t) &cm->obj);
    cm->obj.status = CB_NORMAL;
}

void ui_menu_update_all_GTK(void)
{
    g_list_foreach(checkmark_list, menu_handle_block, (gpointer) 1);
    g_list_foreach(checkmark_list, menu_update_checkmarks, NULL);
    ui_dispatch_events();
    g_list_foreach(checkmark_list, menu_handle_block, (gpointer) 0);
}

void ui_menu_update_all(void)
{
}

void ui_menu_set_tick(GtkWidget *w, int flag) {
    if(GTK_IS_CHECK_MENU_ITEM(w))
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(w), flag != 0);
}

void ui_menu_set_sensitive(GtkWidget *w, int flag)
{
    gtk_widget_set_sensitive(w,flag);
}

/* ------------------------------------------------------------------------- */

/* These functions are called by radio and toggle menu items if the callback
   functions are defined through `UI_MENU_DEFINE_TOGGLE()',
   `UI_MENU_DEFINE_RADIO()' or `UI_MENU_DEFINE_STRING_RADIO()'.  */

void _ui_menu_toggle_helper(GtkWidget *w,
                            ui_callback_data_t event_data,
                            const char *resource_name)
{
    int current_value;

    if (resources_get_int(resource_name, &current_value) < 0)
        return;

    if(!CHECK_MENUS) {
        resources_set_int(resource_name, !current_value);
	ui_update_menus();
    } else {
        ui_menu_set_tick(w, current_value);
    }
}

void _ui_menu_radio_helper(GtkWidget *w,
                           ui_callback_data_t event_data,
                           const char *resource_name)
{
    int current_value;

    resources_get_int(resource_name, &current_value);

    if (!CHECK_MENUS) {
        if (current_value != (int)UI_MENU_CB_PARAM) {
            resources_set_int(resource_name, (int)UI_MENU_CB_PARAM);
            ui_update_menus();
        }
    } else {
	ui_menu_set_tick(w, current_value == (int)UI_MENU_CB_PARAM);
    }  
}

void _ui_menu_string_radio_helper(GtkWidget *w, 
                                  ui_callback_data_t event_data,
                                  const char *resource_name)
{
    const char *current_value;

    resources_get_string(resource_name, &current_value);

    if (current_value == NULL)
        return;

    if (!CHECK_MENUS) {
        if (strcmp(current_value, (const char *)UI_MENU_CB_PARAM) != 0) {
	    resources_set_string(resource_name,(const char *)UI_MENU_CB_PARAM);
	    ui_update_menus();
	}
    } else {
        ui_menu_set_tick(w, strcmp(current_value,
                         (const char *) UI_MENU_CB_PARAM) == 0);
    }
}

