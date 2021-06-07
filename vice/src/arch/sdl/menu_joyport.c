/*
 * menu_joyport.c - Joyport menu for SDL UI.
 *
 * Written by
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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

#include "types.h"

#include "menu_common.h"
#include "joyport.h"
#include "uimenu.h"
#include "lib.h"

#include "menu_joyport.h"

UI_MENU_DEFINE_RADIO(JoyPort1Device)
UI_MENU_DEFINE_RADIO(JoyPort2Device)
UI_MENU_DEFINE_RADIO(JoyPort3Device)
UI_MENU_DEFINE_RADIO(JoyPort4Device)
UI_MENU_DEFINE_RADIO(JoyPort5Device)
UI_MENU_DEFINE_RADIO(JoyPort6Device)
UI_MENU_DEFINE_RADIO(JoyPort7Device)
UI_MENU_DEFINE_RADIO(JoyPort8Device)
UI_MENU_DEFINE_RADIO(JoyPort9Device)
UI_MENU_DEFINE_RADIO(JoyPort10Device)
UI_MENU_DEFINE_RADIO(JoyPort11Device)

static ui_menu_entry_t joyport1_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport2_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport3_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport4_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport5_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport6_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport7_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport8_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport9_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport10_dyn_menu[JOYPORT_MAX_DEVICES + 1];
static ui_menu_entry_t joyport11_dyn_menu[JOYPORT_MAX_DEVICES + 1];

static int joyport1_dyn_menu_init = 0;
static int joyport2_dyn_menu_init = 0;
static int joyport3_dyn_menu_init = 0;
static int joyport4_dyn_menu_init = 0;
static int joyport5_dyn_menu_init = 0;
static int joyport6_dyn_menu_init = 0;
static int joyport7_dyn_menu_init = 0;
static int joyport8_dyn_menu_init = 0;
static int joyport9_dyn_menu_init = 0;
static int joyport10_dyn_menu_init = 0;
static int joyport11_dyn_menu_init = 0;

static void sdl_menu_joyport1_free(void)
{
    int i;

    for (i = 0; joyport1_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport1_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport2_free(void)
{
    int i;

    for (i = 0; joyport2_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport2_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport3_free(void)
{
    int i;

    for (i = 0; joyport3_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport3_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport4_free(void)
{
    int i;

    for (i = 0; joyport4_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport4_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport5_free(void)
{
    int i;

    for (i = 0; joyport5_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport5_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport6_free(void)
{
    int i;

    for (i = 0; joyport6_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport6_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport7_free(void)
{
    int i;

    for (i = 0; joyport7_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport7_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport8_free(void)
{
    int i;

    for (i = 0; joyport8_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport8_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport9_free(void)
{
    int i;

    for (i = 0; joyport9_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport9_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport10_free(void)
{
    int i;

    for (i = 0; joyport10_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport10_dyn_menu[i].string);
    }
}

static void sdl_menu_joyport11_free(void)
{
    int i;

    for (i = 0; joyport11_dyn_menu[i].string != NULL; i++) {
        lib_free(joyport11_dyn_menu[i].string);
    }
}

static UI_MENU_CALLBACK(JoyPort1Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_1);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport1_dyn_menu_init != 0) {
        sdl_menu_joyport1_free();
    } else {
        joyport1_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport1_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport1_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport1_dyn_menu[i].callback = radio_JoyPort1Device_callback;
        joyport1_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport1_dyn_menu[i].string = NULL;
    joyport1_dyn_menu[i].type = 0;
    joyport1_dyn_menu[i].callback = NULL;
    joyport1_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort2Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_2);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport2_dyn_menu_init != 0) {
        sdl_menu_joyport2_free();
    } else {
        joyport2_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport2_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport2_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport2_dyn_menu[i].callback = radio_JoyPort2Device_callback;
        joyport2_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport2_dyn_menu[i].string = NULL;
    joyport2_dyn_menu[i].type = 0;
    joyport2_dyn_menu[i].callback = NULL;
    joyport2_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort3Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_3);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport3_dyn_menu_init != 0) {
        sdl_menu_joyport3_free();
    } else {
        joyport3_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport3_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport3_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport3_dyn_menu[i].callback = radio_JoyPort3Device_callback;
        joyport3_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport3_dyn_menu[i].string = NULL;
    joyport3_dyn_menu[i].type = 0;
    joyport3_dyn_menu[i].callback = NULL;
    joyport3_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort4Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_4);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport4_dyn_menu_init != 0) {
        sdl_menu_joyport4_free();
    } else {
        joyport4_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport4_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport4_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport4_dyn_menu[i].callback = radio_JoyPort4Device_callback;
        joyport4_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport4_dyn_menu[i].string = NULL;
    joyport4_dyn_menu[i].type = 0;
    joyport4_dyn_menu[i].callback = NULL;
    joyport4_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}


static UI_MENU_CALLBACK(JoyPort5Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_5);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport5_dyn_menu_init != 0) {
        sdl_menu_joyport5_free();
    } else {
        joyport5_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport5_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport5_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport5_dyn_menu[i].callback = radio_JoyPort5Device_callback;
        joyport5_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport5_dyn_menu[i].string = NULL;
    joyport5_dyn_menu[i].type = 0;
    joyport5_dyn_menu[i].callback = NULL;
    joyport5_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort6Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_6);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport6_dyn_menu_init != 0) {
        sdl_menu_joyport6_free();
    } else {
        joyport6_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport6_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport6_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport6_dyn_menu[i].callback = radio_JoyPort6Device_callback;
        joyport6_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport6_dyn_menu[i].string = NULL;
    joyport6_dyn_menu[i].type = 0;
    joyport6_dyn_menu[i].callback = NULL;
    joyport6_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort7Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_7);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport7_dyn_menu_init != 0) {
        sdl_menu_joyport7_free();
    } else {
        joyport7_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport7_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport7_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport7_dyn_menu[i].callback = radio_JoyPort7Device_callback;
        joyport7_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport7_dyn_menu[i].string = NULL;
    joyport7_dyn_menu[i].type = 0;
    joyport7_dyn_menu[i].callback = NULL;
    joyport7_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort8Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_8);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport8_dyn_menu_init != 0) {
        sdl_menu_joyport8_free();
    } else {
        joyport8_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport8_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport8_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport8_dyn_menu[i].callback = radio_JoyPort8Device_callback;
        joyport5_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport8_dyn_menu[i].string = NULL;
    joyport8_dyn_menu[i].type = 0;
    joyport8_dyn_menu[i].callback = NULL;
    joyport8_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort9Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_9);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport9_dyn_menu_init != 0) {
        sdl_menu_joyport9_free();
    } else {
        joyport9_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport9_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport9_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport9_dyn_menu[i].callback = radio_JoyPort9Device_callback;
        joyport9_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport9_dyn_menu[i].string = NULL;
    joyport9_dyn_menu[i].type = 0;
    joyport9_dyn_menu[i].callback = NULL;
    joyport9_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort10Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_10);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport10_dyn_menu_init != 0) {
        sdl_menu_joyport10_free();
    } else {
        joyport10_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport10_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport10_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport10_dyn_menu[i].callback = radio_JoyPort10Device_callback;
        joyport10_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport10_dyn_menu[i].string = NULL;
    joyport10_dyn_menu[i].type = 0;
    joyport10_dyn_menu[i].callback = NULL;
    joyport10_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

static UI_MENU_CALLBACK(JoyPort11Device_dynmenu_callback)
{
    joyport_desc_t *devices = joyport_get_valid_devices(JOYPORT_11);
    int i;

    /* rebuild menu if it already exists. */
    if (joyport11_dyn_menu_init != 0) {
        sdl_menu_joyport11_free();
    } else {
        joyport11_dyn_menu_init = 1;
    }

    for (i = 0; devices[i].name; ++i) {
        joyport11_dyn_menu[i].string = (char *)lib_strdup(devices[i].name);
        joyport11_dyn_menu[i].type = MENU_ENTRY_RESOURCE_RADIO;
        joyport11_dyn_menu[i].callback = radio_JoyPort11Device_callback;
        joyport11_dyn_menu[i].data = (ui_callback_data_t)int_to_void_ptr(devices[i].id);
    }

    joyport11_dyn_menu[i].string = NULL;
    joyport11_dyn_menu[i].type = 0;
    joyport11_dyn_menu[i].callback = NULL;
    joyport11_dyn_menu[i].data = NULL;

    lib_free(devices);

    return MENU_SUBMENU_STRING;
}

ui_menu_entry_t joyport_menu[JOYPORT_MAX_PORTS + 2];

UI_MENU_DEFINE_TOGGLE(BBRTCSave)

void uijoyport_menu_create(int port1, int port2, int port3_4, int port5_10, int port11)
{
    int j = 0;

    joyport_menu[j].string = "Save BBRTC data when changed";
    joyport_menu[j].type = MENU_ENTRY_RESOURCE_TOGGLE;
    joyport_menu[j].callback = toggle_BBRTCSave_callback;
    joyport_menu[j].data = NULL;
    ++j;

    if (port1) {
        joyport_menu[j].string = joyport_get_port_name(JOYPORT_1);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort1Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport1_dyn_menu;
        ++j;
    }

    if (port2) {
        joyport_menu[j].string = joyport_get_port_name(JOYPORT_2);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort2Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport2_dyn_menu;
        ++j;
    }

    if (port3_4) {
        joyport_menu[j].string = joyport_get_port_name(JOYPORT_3);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort3Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport3_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_4);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort4Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport4_dyn_menu;
        ++j;
    }

    if (port5_10) {
        joyport_menu[j].string = joyport_get_port_name(JOYPORT_5);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort5Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport5_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_6);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort6Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport6_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_7);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort7Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport7_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_8);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort8Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport8_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_9);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort9Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport9_dyn_menu;
        ++j;

        joyport_menu[j].string = joyport_get_port_name(JOYPORT_10);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort10Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport10_dyn_menu;
        ++j;
    }

    if (port11) {
        joyport_menu[j].string = joyport_get_port_name(JOYPORT_11);
        joyport_menu[j].type = MENU_ENTRY_DYNAMIC_SUBMENU;
        joyport_menu[j].callback = JoyPort11Device_dynmenu_callback;
        joyport_menu[j].data = (ui_callback_data_t)joyport11_dyn_menu;
        ++j;
    }
    joyport_menu[j].string = NULL;
    joyport_menu[j].type = MENU_ENTRY_TEXT;
    joyport_menu[j].callback = NULL;
    joyport_menu[j].data = NULL;
}


/** \brief  Clean up memory used by the dynamically created joyport menus
 */
void uijoyport_menu_shutdown(void)
{
    if (joyport1_dyn_menu_init) {
        sdl_menu_joyport1_free();
    }
    if (joyport2_dyn_menu_init) {
        sdl_menu_joyport2_free();
    }
    if (joyport3_dyn_menu_init) {
        sdl_menu_joyport3_free();
    }
    if (joyport4_dyn_menu_init) {
        sdl_menu_joyport4_free();
    }
    if (joyport5_dyn_menu_init) {
        sdl_menu_joyport5_free();
    }
    if (joyport6_dyn_menu_init) {
        sdl_menu_joyport6_free();
    }
    if (joyport7_dyn_menu_init) {
        sdl_menu_joyport7_free();
    }
    if (joyport8_dyn_menu_init) {
        sdl_menu_joyport8_free();
    }
    if (joyport9_dyn_menu_init) {
        sdl_menu_joyport9_free();
    }
    if (joyport10_dyn_menu_init) {
        sdl_menu_joyport10_free();
    }
    if (joyport11_dyn_menu_init) {
        sdl_menu_joyport11_free();
    }
}
