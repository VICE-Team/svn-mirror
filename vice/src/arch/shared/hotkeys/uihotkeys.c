/** \file   uihotkeys.c
 * \brief   UI-agnostic hotkeys
 *
 * \author  Bas Wassink <b.wassink@ziggo.nl>
 */

/*
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
 */

#include "vice.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#include "archdep_defs.h"
#include "archdep_exit.h"
#include "cmdline.h"
#include "hotkeystypes.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "parser.h"
#include "vhkkeysyms.h"
#include "uiapi.h"
#include "uiactions.h"
#include "util.h"
#include "version.h"
#ifdef USE_SVN_REVISION
#include "svnversion.h"
#endif

#include "uihotkeys.h"

/* to disable debugging, uncomment the #define, but keep the #include: */
/* #define DEBUG_VHK */
#include "vhkdebug.h"


/** \brief  Length of the filename string for VICE machine dir hotkeys files
 *
 * This excludes the prefix set by ui_hotkeys_init() and the terminating nul.
 *
 * "-hotkeys[-mac].vhk"
 */
#ifndef MACOS_COMPILE
#define VHK_FILENAME_LEN   (1 + 7 + 4)
#else
/* adds "-mac" */
#define VHK_FILENAME_LEN   (1 + 7 + 1 + 3 + 4)
#endif


/** \brief  Log for the hotkeys system
 */
log_t vhk_log;

/** \brief  Prefix for .vhk filenames
 *
 * Set with ui_hotkeys_init(), this string is used to differenciate between
 * UIs/archs, so each can have their own hotkeys on the same system.
 * For example, for the Gtk3 UI, this is "gtk3", which results in the default
 * filename being "gtk3-hotkeys.vhk".
 *
 * \note    Freed with ui_hotkeys_shutdown() on emulator shutdown.
 */
static char *vhk_prefix = NULL;

/** \brief  Path to current hotkey file
 *
 * \note    Freed with ui_hotkeys_shutdown() on emulator shutdown.
 */
static char *vhk_filename = NULL;

/** \brief  A .vhk file is pending
 *
 * When the 'HotkeyFile' resource is initially set the UI hasn't finished
 * building the menu and registering actions, so trying to parse a hotkeys
 * file and adding hotkeys for actions will fail. This flag is used to
 * indicate a hotkey file is pending to be parsed.
 */
static bool vhk_file_pending = false;

/** \brief  The hotkeys system is initialized
 *
 * This flag is used to determine if a new HotkeyFile resource value can be
 * used to parse a hotkeys file, or if it needs to be marked as pending,
 * waiting for the UI to be fully initialized.
 */
static bool vhk_init_done = false;


/* VICE resources, command line options and their handlers */

/** \brief  Set hotkey file and process its contents
 *
 * \param[in]   val     new hotkey file
 * \param[in]   param   extra argument (unused)
 *
 * \return  0 on success, -1 on error
 */
static int vhk_filename_set(const char *val, void *param)
{
    if (util_string_set(&vhk_filename, val) != 0) {
        /* new value was the same as the old value, don't do anything */
        return 0;
    }

    /* process hotkeys */
    if (help_requested) {
        return 0;
    }

    if (val != NULL && *val != '\0') {
        if (vhk_init_done) {
            /* UI is properly initialized, directly parse the hotkeys file */
            log_message(vhk_log, "Hotkeys: parsing '%s':", val);
            ui_hotkeys_parse(val);
            vhk_file_pending = false;
        } else {
            /* UI is not yet fully initialized, mark parsing of hotkeys file
             * pending */
            vhk_file_pending = true;
        }
    }
    return 0;
}

/** \brief  String type resources
 */
static resource_string_t resources_string[] = {
    { "HotkeyFile", "", RES_EVENT_NO, NULL,
      &vhk_filename, vhk_filename_set, NULL },
    RESOURCE_STRING_LIST_END
};

/** \brief  List of command line options
 */
static const cmdline_option_t cmdline_options[] = {
    { "-hotkeyfile", SET_RESOURCE, CMDLINE_ATTRIB_NEED_ARGS,
      NULL, NULL, "HotkeyFile", NULL,
      "<name>", "Specify name of hotkey file" },
    CMDLINE_LIST_END
};


/** \brief  Initialize resources used by the custom hotkeys
 *
 * \return  0 on success
 */
int ui_hotkeys_resources_init(void)
{
    return resources_register_string(resources_string);
}


/** \brief  Initialize command line options used by the custom hotkeys
 *
 * \return  0 on success
 */
int ui_hotkeys_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


/** \brief  Initialize hotkeys system
 *
 * \param[in]   prefix  prefix for .vhk filenames
 */
void ui_hotkeys_init(const char *prefix)
{
    vhk_log = log_open("Hotkeys");
    log_message(vhk_log, "Initializing hotkeys.");
    if (prefix == NULL || *prefix == '\0') {
        log_error(vhk_log,
                  "%s(): `prefix` cannot be empty or NULL.",
                  __func__);
        archdep_vice_exit(1);
    }
    vhk_prefix = lib_strdup(prefix);

    /* When we get to there the UI has been initialized */
    vhk_init_done = true;

    /* TODO: replace with `vhk_file` */
    if (vhk_filename == NULL || *vhk_filename == '\0') {
        ui_hotkeys_load_default();
    } else {
        if (vhk_file_pending) {
            /* We have a pending hotkeys file to parse */
            ui_hotkeys_parse(vhk_filename);
            vhk_file_pending = false;
        }
    }
}


/** \brief  Shut down hotkeys system
 *
 * Clear up all resources used by the hotkeys system and close the log.
 * The "virtual method" ui_hotkeys_arch_shutdown() is called first to allow the
 * current UI/arch to clean up its hotkeys data, if any.
 */
void ui_hotkeys_shutdown(void)
{
    log_message(vhk_log, "shutting down.");
    /* call virtual method before tearing down the generic hotkeys data */
    ui_hotkeys_arch_shutdown();
    lib_free(vhk_prefix);
    lib_free(vhk_filename);
    log_close(vhk_log);
}


/** \brief  Load the default hotkeys
 *
 * Parse the VICE-provided hotkey files, clearing any user-defined hotkeys.
 * Also set the "HotkeyFile" resource to "".
 */
void ui_hotkeys_load_default(void)
{
    char *filename = ui_hotkeys_vhk_filename_vice();

    log_message(vhk_log,
                "parsing default file '%s' for machine %s",
                filename, machine_name);

    if (ui_hotkeys_parse(filename)) {
        log_message(vhk_log, "OK.");
        /* clear the custom hotkeys file resource */
        resources_set_string("HotkeyFile", "");
    } else {
        log_message(vhk_log, "failed, continuing anyway.");
    }

    lib_free(filename);
}


/*
 * Export current hotkeys to file
 */

/** \brief  Helper: log I/O error
 *
 * Logs libc I/O errors to the hotkeys log, including errno and description.
 */
static void export_log_io_error(void)
{
    log_error(vhk_log,
              "Hotkeys: I/O error (%d: %s).",
               errno, strerror(errno));
}

/** \brief  Output the header for an exported hotkeys file
 *
 * \param[in]   fp  file descriptor
 *
 * \return  true on success
 */
static bool export_header(FILE *fp)
{
    const struct tm *tinfo;
    time_t           t;
    char             buffer[1024];
    int              result;

    result = fprintf(fp,
"# VICE hotkeys file for %s\n"
"#\n"
"# TODO: Add documentation of .vhk format\n"
"\n", machine_get_name());
    if (result < 0) {
        export_log_io_error();
        return false;
    }

    /* add current datetime */
    t = time(NULL);
    tinfo = localtime(&t);
    if (tinfo != NULL) {
        strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M%z", tinfo);
        result = fprintf(fp, "# Generated on %s\n", buffer);
        if (result < 0) {
            export_log_io_error();
            return false;
        }
    }

    /* add VICE version */
#ifdef USE_SVN_REVISION
    result = fprintf(fp, "# Generated by VICE %s r%s\n",
                     VERSION, VICE_SVN_REV_STRING);
#else
    result = fprintf(fp, "# Generated by VICE %s\n", VERSION);
#endif
     if (result < 0) {
        export_log_io_error();
        return false;
    }

    result = fprintf(fp,
"!debug disable\n"
"!clear\n"
"\n"
"# User-defined hotkeys:\n"
"\n");
    if (result < 0) {
        export_log_io_error();
        return false;
    }
    return true;
}


/** \brief  Export all currently registered hotkeys to file \a path
 *
 * \param[in]   path    filename of .vhk file to generate
 *
 *
 * \return  bool
 */
bool ui_hotkeys_export(const char *path)
{
    FILE *fp;
    int   action;

    log_message(vhk_log, "Hotkeys: exporting current hotkeys to '%s'.", path);

    fp = fopen(path, "wb");
    if (fp == NULL) {
        log_error(vhk_log,
                  "Hotkeys: failed to open '%s' for writing (%d: %s).",
                  path, errno, strerror(errno));
        return false;
    }

    if (!export_header(fp)) {
        fclose(fp);
        return false;
    }

    /* iterate assigned hotkeys */
    for (action = 0; action < ACTION_ID_COUNT; action++) {
        ui_action_map_t *map = ui_action_map_get(action);
        if (map != NULL && map->vice_keysym != 0) {

            const char *action_name;
            const char *keysym_name;
            char       *modmask_name;
            int         result;

            action_name  = ui_action_get_name(map->action);
            keysym_name  = vhk_keysym_name(map->vice_keysym);
            modmask_name = vhk_modmask_name(map->vice_modmask);

            /* Print line with "<action>  [<modmask>]<keysym>".
             *
             * We can use the modmask name here unconditionally since it'll
             * be an empty string when no modifiers are present.
             */
            result = fprintf(fp, "%-30s  %s%s\n",
                             action_name, modmask_name, keysym_name);
            lib_free(modmask_name);

            if (result < 0) {
                export_log_io_error();
                fclose(fp);
                return false;
            }
        }
    }

    fclose(fp);
    return true;
}


/** \brief  Install hotkey for a UI action and its menu item(s), if any
 *
 * Calls the arch-specific virtual method ui_hotkeys_arch_install_by_map().
 *
 * \param[in]   map     vhk map object
 */
void ui_hotkeys_install_by_map(ui_action_map_t *map)
{
    ui_hotkeys_arch_install_by_map(map);
    /* so far no additional bookkeeping required */
}


/** \brief  Update hotkey for a UI action and its menu item(s), if any
 *
 * Update hotkey for \a map, setting the new hotkey to \a vice_keysym +
 * \a vice_modmask. Calls virtual methods ui_hotkeys_arch_remove_by_map() and
 * ui_hotkeys_arch_install_by_map() and updates VICE and arch keysysm and
 * modmasks in \a map.
 *
 * \param[in]   map             UI action map object
 * \param[in]   vice_keysym     new VICE keysym
 * \param[in]   vice_modmask    new VICE modifier mask
 */
void ui_hotkeys_update_by_map(ui_action_map_t *map,
                              uint32_t         vice_keysym,
                              uint32_t         vice_modmask)
{
    if (map != NULL) {
        uint32_t arch_keysym;
        uint32_t arch_modmask;

        /* call virtual method to remove old hotkey */
        ui_hotkeys_arch_remove_by_map(map);

        /* update map */
        arch_keysym  = ui_hotkeys_arch_keysym_to_arch(vice_keysym);
        arch_modmask = ui_hotkeys_arch_modmask_to_arch(vice_modmask);
        map->vice_keysym  = vice_keysym;
        map->vice_modmask = vice_modmask;
        map->arch_keysym  = arch_keysym;
        map->arch_modmask = arch_modmask;

        /* call virtual method to install new hotkey */
        ui_hotkeys_arch_install_by_map(map);
    }
}


/** \brief  Update hotkey for a UI action and its menu item(s), if any
 *
 * Update hotkey for \a action, setting the new hotkey to \a vice_keysym +
 * \a vice_modmask. Calls virtual methods ui_hotkeys_arch_remove_by_map() and
 * ui_hotkeys_arch_install_by_map() and updates VICE and arch keysysm and
 * modmasks for \a action.
 *
 * \param[in]   action          UI action ID
 * \param[in]   vice_keysym     new VICE keysym
 * \param[in]   vice_modmask    new VICE modifier mask
 */
void ui_hotkeys_update_by_action(int action,
                                 uint32_t vice_keysym,
                                 uint32_t vice_modmask)
{
    ui_action_map_t *map = ui_action_map_get(action);
    if (map != NULL) {
        ui_hotkeys_update_by_map(map, vice_keysym, vice_modmask);
    }
}


/** \brief  Remove hotkey from UI action and its menu item(s), if any
 *
 * Clear hotkey associated with the UI action in \a map and remove any menu
 * item accelerator labels if present.
 * Calls virtual method ui_hotkeys_arch_remove_by_map().
 *
 * \param[in]   map     UI action map object
 */
void ui_hotkeys_remove_by_map(ui_action_map_t *map)
{
    if (map != NULL) {
        ui_hotkeys_arch_remove_by_map(map);
        ui_action_map_clear_hotkey(map);
    }
}


/** \brief  Remove hotkey from UI action and its menu item(s), if any
 *
 * Clear hotkey associated with UI \a action and remove any menu item accelerator
 * labels if present.
 * Indirectly calls virtual method ui_hotkeys_arch_remove_by_map().
 *
 * \param[in]   map     vhk map object
 */
void ui_hotkeys_remove_by_action(int action)
{
    ui_action_map_t *map = ui_action_map_get(action);
    if (map != NULL) {
        ui_hotkeys_remove_by_map(map);
    }
}


/** \brief  Remove all hotkeys
 *
 * Remove all hotkeys registered and clear all accelerator labels on menu items.
 * Calls virtual method ui_hotkeys_arch_remove_by_map().
 */
void ui_hotkeys_remove_all(void)
{
    int action;

    for (action = 0 ; action < ACTION_ID_COUNT; action++) {
        ui_action_map_t *map = ui_action_map_get(action);
        if (map != NULL && map->vice_keysym != 0) {
            debug_vhk("removing hotkeys for action %d (%s)",
                      action, ui_action_get_name(action));
            ui_hotkeys_arch_remove_by_map(map);
            ui_action_map_clear_hotkey(map);
        }
    }
}


/** \brief  Parse hotkeys file
 *
 * \param[in]   path    path to hotkeys file
 *
 * \return  bool
 */
bool ui_hotkeys_parse(const char *path)
{
    /* we might want to do something with `path` at a future time, but right
     * now just call the parse function */
    return vhk_parser_parse(path);
}


/** \brief  Generate filename for .vhk files in machine dirs
 *
 * Generate string in the form "$ARCH-hotkeys[-mac].vhk".
 *
 * \return  filename, free with lib_free()
 */
char *ui_hotkeys_vhk_filename_vice(void)
{
    char   *name;
    size_t  size;

    size = strlen(vhk_prefix) + VHK_FILENAME_LEN + 1;
    if (machine_class == VICE_MACHINE_VSID) {
        size += 5;  /* "vsid-" */
    }
    name = lib_malloc(size);

#ifndef MACOS_COMPILE
    if (machine_class == VICE_MACHINE_VSID) {
        snprintf(name, size, "%s-vsid-hotkeys.vhk", vhk_prefix);
    } else {
        snprintf(name, size, "%s-hotkeys.vhk", vhk_prefix);
    }
#else
    if (machine_class == VICE_MACHINE_VSID) {
        snprintf(name, size, "%s-vsid-hotkeys-mac.vhk", vhk_prefix);
    } else {
        snprintf(name, size, "%s-hotkeys-mac.vhk", vhk_prefix);
    }
#endif
    return name;
}


/** \brief  Generate default filename for user-defined .vhk files
 *
 * Generate string in the form "$ARCH-hotkeys[-mac]-$MACHINE.vhk".
 *
 * \return  filename, free with lib_free()
 */
char *ui_hotkeys_vhk_filename_user(void)
{
    const char *suffix;
    char       *name;
    size_t      plen;
    size_t      mlen;
    size_t      size;

    if (machine_class == VICE_MACHINE_VSID) {
        suffix = "VSID";    /* machine_name == "C64" */
    } else if (machine_class == VICE_MACHINE_C64SC) {
        suffix = "C64SC";   /* machine_name == "C64" */
    } else {
        suffix = machine_name;
    }
    plen = strlen(vhk_prefix);
    mlen = strlen(suffix);
    size = plen + VHK_FILENAME_LEN + 1 + mlen + 1;
    name = lib_malloc(size);
#ifndef MACOS_COMPILE
    snprintf(name, size, "%s-hotkeys-%s.vhk", vhk_prefix, suffix);
#else
    snprintf(name, size, "%s-hotkeys-mac-%s.vhk", vhk_prefix, suffix);
#endif
    return name;
}
