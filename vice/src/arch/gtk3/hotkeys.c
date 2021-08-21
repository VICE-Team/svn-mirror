/** \file   hotkeys.c
 * \brief   Gtk3 custom hotkeys handling
 *
 * Provides custom keyboard shortcuts for the Gtk3 UI.
 *
 * (
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

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <errno.h>

#include "archdep.h"
#include "debug_gtk3.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "resources.h"
#include "sysfile.h"
#include "uimachinemenu.h"
#include "util.h"

#include "hotkeys.h"


/** \brief  Initial size of the buffer of the textfile reader
 *
 * \note    Currently set to a very small value to check the reallocation code.
 */
#define TEXTFILE_READER_BUFSIZE     32


/** \brief  File entry object for the textfile reader
 *
 */
typedef struct textfile_entry_s {
    char *                      path;       /**< full path of file */
    long                        pos;        /**< position in file */
    long                        linenum;    /**< current line number in file */
    struct textfile_entry_s *   next;       /**< next entry in stack */
} textfile_entry_t;


/** \brief  Object to read text files that can 'include' others
 */
typedef struct textfile_reader_s {
    char *              buffer;     /**< buffer holding a single line of text */
    size_t              bufsize;    /**< size of buffer */
    size_t              buflen;     /**< length of text in buffer */
    FILE *              fp;         /**< file pointer for the current file */
    textfile_entry_t *  entries;    /**< stack of files */
} textfile_reader_t;



/*
 * Forward declarations
 */
static int hotkey_file_set(const char *val, void *param);


/** \brief  Path to current hotkey file
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkey_file = NULL;


/** \brief  Default hotkey file name
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkey_file_default = NULL;

/** \brief  Log instance for hotkeys
 */
static log_t hotkey_log;


/** \brief  String type resources
 *
 * \note    Make sure "HotkeyFile" remains the first element in the list, the
 *          init code sets the factory value during runtime, using array index
 *          0.
 */
static resource_string_t resources_string[] = {
    { "HotkeyFile", NULL, RES_EVENT_NO, NULL,
      &hotkey_file, hotkey_file_set, NULL },
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


/*
 * Resource getters/setters
 */

/** \brief  Set hotkey file and process its contents
 *
 * \param[in]   val     new hotkey file
 * \param[in]   param   extra argument (unused)
 *
 * \return  0 on success, -1 on error
 */
static int hotkey_file_set(const char *val, void *param)
{
    debug_gtk3("Got hotkey file '%s'", val);

    if (util_string_set(&hotkey_file, val) != 0) {
        /* new value was the same as the old value, don't do anything */
        return 0;
    }

    /* process hotkeys */
#if 0
    debug_gtk3("Faking hotkeys loading: swapping Alt+H/Alt+M.");
    ui_set_vice_menu_item_hotkey_by_name("monitor", "m", GDK_MOD1_MASK);
    ui_set_vice_menu_item_hotkey_by_name("toggle-mouse-grab", "h", GDK_MOD1_MASK);
#endif
    return 0;
}


/** \brief  Initialize resources used by the custom hotkeys
 *
 * \return  0 on success
 */
int hotkeys_resources_init(void)
{
    /* set the default filename */
    hotkey_file_default = archdep_default_hotkey_file_name();
    resources_string[0].factory_value = hotkey_file_default;

    /* register the resources */
    return resources_register_string(resources_string);
}


/** \brief  Initialize command line options used by the custom hotkeys
 *
 * \return  0 on success
 */
int hotkeys_cmdline_options_init(void)
{
    return cmdline_register_options(cmdline_options);
}


/** \brief  Initialize hotkeys
 *
 * Initialize hotkey handling resources such as logs and objects.
 *
 * \note    Does *not* initialize command line options or vice resources, that
 *          is done separately in hotkeys_cmdline_options_init() and
 *          hotkeys_resources_init().
 */
void hotkeys_init(void)
{
    hotkey_log = log_open("HOTKEYS");
    log_message(hotkey_log, "Init.");
}



/** \brief  Clean up resources used by the hotkeys
 *
 * \note    Call on emulator shutdown
 */
void hotkeys_shutdown(void)
{
    if (hotkey_file != NULL) {
        lib_free(hotkey_file);
        hotkey_file = NULL;
    }
    if (hotkey_file_default != NULL) {
        lib_free(hotkey_file_default);
        hotkey_file_default = NULL;
    }

    log_close(hotkey_log);
}





/*
 * Hotkeys parser
 */


/*
 * textfile_reader_t methods
 */



/** \brief  Create new textfile_entry_t instance
 *
 * Allocate memory for a textfile_entry_t instance and copy \a path.
 *
 * \param[in]   path    path to file
 *
 * \return  heap-allocated textfile_entry_t instance
 */
static textfile_entry_t *textfile_entry_new(const char *path)
{
    textfile_entry_t *entry = lib_malloc(sizeof *entry);

    entry->path = lib_strdup(path);
    entry->pos = 0;
    entry->linenum = 1;
    entry->next = NULL;

    return entry;
}


/** \brief  Free textfile entry and its members
 *
 * Free \a entry and its members.
 *
 * \param[in]   entry   textfile entry instance
 */
static void textfile_entry_free(textfile_entry_t *entry)
{
    if (entry != NULL) {
        lib_free(entry->path);
        lib_free(entry);
    }
}


/** \brief  Free textfile entry and its members recusively
 *
 * Free \a entry, its members and recursively its siblings.
 *
 * \param[in]   entry   textfile entry instance
 */
static void textfile_entry_free_all(textfile_entry_t *entry)
{
    textfile_entry_t *next = entry->next;

    textfile_entry_free(entry);
    if (next != NULL) {
        textfile_entry_free_all(next);
    }
}


/** \brief  Initialize \a reader
 *
 * Initialize the \a reader for use by setting all members and allocating the
 * buffer to its initial size.
 *
 * \param[in,out]   reader  textfile reader
 */
static void textfile_reader_init(textfile_reader_t *reader)
{
    reader->buffer = lib_malloc(TEXTFILE_READER_BUFSIZE);
    reader->bufsize = TEXTFILE_READER_BUFSIZE;
    reader->buflen = 0;
    reader->fp = NULL;
    reader->entries = NULL;
}


/** \brief  Free all memory used by \a reader
 *
 * Free all memory used by \a reader and close any open file.
 *
 * \param[in,out]   reader  textfile reader instance
 */
static void textfile_reader_free(textfile_reader_t *reader)
{
    lib_free(reader->buffer);
    if (reader->fp != NULL) {
        fclose(reader->fp);
    }
    /* unwind stack */
    if (reader->entries != NULL) {
        textfile_entry_free_all(reader->entries);
    }
}


static bool textfile_reader_open(textfile_reader_t *reader, const char *path)
{
    textfile_entry_t *current;
    char *complete_path = NULL;

    debug_gtk3("new file '%s' to open.", path);

    /* get top of stack */
    current = reader->entries;

    if (current != NULL) {


        debug_gtk3("old file present, remembering position.");

        /* remember position in current file */
        current->pos = ftell(reader->fp);
        /* close current file */
        fclose(reader->fp);
    }

    /* try to open new file */
    debug_gtk3("trying to opening new file '%s':", path);
    reader->fp = sysfile_open(path, NULL, &complete_path, "rb");
    if (reader->fp == NULL) {
        debug_gtk3("failed.");
        return false;
    } else {
        /* add new file to stack */
        debug_gtk3("OK, adding to stack.");
        textfile_entry_t *new = textfile_entry_new(complete_path);
        lib_free(complete_path);
        new->next = current;
        reader->entries = new;
    }

    return true;
}


static bool textfile_reader_close(textfile_reader_t *reader)
{
    textfile_entry_t *current = reader->entries;
    textfile_entry_t *next;

    if (current != NULL) {

        /* close current file and free memory */
        next = current->next;
        textfile_entry_free(current);
        current = next;
        reader->entries = current;
        fclose(reader->fp);

        /* reopen previous file on stack, if any */
        if (current != NULL) {
            reader->fp = fopen(current->path, "rb");
            if (reader->fp == NULL) {
                debug_gtk3("failed to open '%s'.", current->path);
                return false;
            }
            /* reposition stream to the position it had when a new file was
             * opened */
            fseek(reader->fp, current->pos, SEEK_SET);
        }
    }
    return true;
}


bool hotkeys_parse(const char *path)
{
    textfile_reader_t reader;

    textfile_reader_init(&reader);
    if (textfile_reader_open(&reader, path)) {
        textfile_reader_close(&reader);
    }
    textfile_reader_free(&reader);

    return true;
}
