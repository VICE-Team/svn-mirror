/** \file   hotkeys.c
 * \brief   Gtk3 custom hotkeys handling
 *
 * Provides custom keyboard shortcuts for the Gtk3 UI.
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

/* Resources manipulated in this file:
 *
 * $VICERES HotkeyFile all
 */

#include "vice.h"

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <inttypes.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

#include "archdep.h"
#include "debug_gtk3.h"
#include "cmdline.h"
#include "hotkeymap.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "resources.h"
#include "sysfile.h"
#include "ui.h"
#include "uiactions.h"
#include "uimenu.h"
#include "util.h"
#include "version.h"
#ifdef USE_SVN_REVISION
#include "svnversion.h"
#endif

#include "hotkeys.h"


/*
 * Forward declarations
 */
static int hotkeys_file_set(const char *val, void *param);


/** \brief  Array length helper
 *
 * \param[in]   arr array name
 */
#define ARRAY_LEN(arr)  (sizeof(arr) / sizeof(arr[0]))


#ifdef MACOS_COMPILE
/** \brief  Replacement of 'Primary' for MacOS */
#define PRIMARY_REPLACEMENT "Command"
#else
/** \brief  Replacement of 'Primary' for non-MacOS */
#define PRIMARY_REPLACEMENT "Control"
#endif


/** \brief  Initial size of the buffer of the textfile reader
 *
 * The buffer will be automatically doubled in size
 */
#define TEXTFILE_READER_BUFSIZE 1024

/** \brief  Comment token */
#define HOTKEYS_COMMENT         '#'

/** \brief  Alternative comment token */
#define HOTKEYS_COMMENT_ALT     ';'

/** \brief  Keyword token */
#define HOTKEYS_KEYWORD         '!'

/** \brief  Modifier opening token */
#define HOTKEYS_MODIFIER_OPEN   '<'

/** \brief  Modifier closing token */
#define HOTKEYS_MODIFIER_CLOSE  '>'

/** \brief  Quote token */
#define HOTKEYS_QUOTE           '"'

/** \brief  Token for 'escaping' the following token */
#define HOTKEYS_ESCAPE          '\\'


/** \brief  Keyword IDs
 */
typedef enum hotkeys_keyword_id_e {
    HOTKEYS_KW_ID_ILLEGAL = -1, /**< illegal directive */
    HOTKEYS_KW_ID_CLEAR,        /**< clear all hotkeys */
    HOTKEYS_KW_ID_DEBUG,        /**< enable/disable debuging messages */
    HOTKEYS_KW_ID_INCLUDE,      /**< include "file" */
    HOTKEYS_KW_ID_UNDEF,        /**< undefine a modifiers+key combo */
    HOTKEYS_KW_ID_IFDEF,        /**< check if modifiers+key is defined */
    HOTKEYS_KW_ID_IFNDEF,       /**< check if modifiers_key is not defined */
    HOTKEYS_KW_ID_ENDIF         /**< end of !IF[N]DEF block */
} hotkeys_keyword_id_t;





/** \brief  File entry object for the textfile reader
 *
 * Singly linked list to implement a stack.
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


/** \brief  Parser keyword data object
 */
typedef struct hotkeys_keyword_s {
    const char *            name;       /**< name */
    hotkeys_keyword_id_t    id;         /**< ID */
    int                     args_min;   /**< minimum number of arguments */
    int                     args_max;   /**< maximum number of arguments */
    const char *            syntax;     /**< syntax */
    const char *            desc;       /**< description */
} hotkeys_keyword_t;


/** \brief  Object for apping of !DEBUG arguments to boolean
 */
typedef struct debug_args_s {
    const char *symbol; /**< string literal */
    bool        value;  /**< boolean value */
} debug_args_t;


/** \brief  Mapping of !DEBUG arguments to boolean
 */
static const debug_args_t debug_arglist[] = {
    { "enable",     true },
    { "disable",    false },
    { "on",         true },
    { "off",        false }
};


/** \brief  Hotkeys parser keyword list
 *
 * List of parser keywords with ID and argument count.
 *
 * \note    The array needs to stay in alphabetical order.
 */
static const hotkeys_keyword_t hotkeys_keyword_list[] = {
    { "clear",      HOTKEYS_KW_ID_CLEAR,    0, 0,
      "!CLEAR",
      "Clear all hotkeys" },

    { "debug",      HOTKEYS_KW_ID_DEBUG,    1, 1,
      "!DEBUG <enable|disable|on|off>",
      "Turn debugging output on or off" },

    { "endif",      HOTKEYS_KW_ID_ENDIF,    0, 0,
      "!ENDIF",
      "End of !IF[N]DEF" },

    { "ifdef",      HOTKEYS_KW_ID_IFDEF,    1, 1,
      "!IFDEF <condition>",
      "Execute following statement(s) if <condition> is met" },

    { "ifndef",      HOTKEYS_KW_ID_IFNDEF,  1, 1,
      "!IFNDEF <condition>",
      "Execute following statement(s) if <condition> is not met" },

    { "include",    HOTKEYS_KW_ID_INCLUDE,  1, 1,
      "!INCLUDE <path>",
      "Start parsing hotkeys file <path>" },

    { "undef",      HOTKEYS_KW_ID_UNDEF,    1, 1,
      "!UNDEF <modifiers+key>",
      "Undefine a hotkey by <modifiers+key>" }
};


/** \brief  Mapping of modifier names to GDK modifier masks
 *
 * Contains mappings of modifier names used in hotkeys files to IDs and
 * GDK modifier masks.
 *
 * \note    The array needs to stay in alphabetical order.
 */
static const hotkeys_modifier_t hotkeys_modifier_list[] = {
    { "Alt",        HOTKEYS_MOD_ID_ALT,     GDK_MOD1_MASK,      "MOD1",     "Alt" },
    { "Command",    HOTKEYS_MOD_ID_COMMAND, GDK_META_MASK,      "META",     "Command \u2318" },
    { "Control",    HOTKEYS_MOD_ID_CONTROL, GDK_CONTROL_MASK,   "CONTROL",  "Control \u2303" },
    { "Hyper",      HOTKEYS_MOD_ID_HYPER,   GDK_HYPER_MASK,     "HYPER",    "Hyper" },
    { "Option",     HOTKEYS_MOD_ID_OPTION,  GDK_MOD1_MASK,      "MOD1",     "Option \u2325" },
    { "Shift",      HOTKEYS_MOD_ID_SHIFT,   GDK_SHIFT_MASK,     "SHIFT",    "Shift \u21e7" },
    { "Super",      HOTKEYS_MOD_ID_SUPER,   GDK_SUPER_MASK,     "SUPER",    "Super" },
    { NULL,         -1,                     0,                  NULL,       NULL }
};



/** \brief  Path to current hotkey file
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkeys_file = NULL;

/** \brief  Default hotkeys file name
 *
 * \note    Free with hotkeys_shutdown() on emulator shutdown.
 */
static char *hotkeys_file_default = NULL;

/** \brief  A .vhk file is pending
 *
 * When the 'HotkeyFile' resource is initially set the UI hasn't finished
 * building the menu and registering actions, so trying to parse a hotkeys
 * file and adding hotkeys for actions will fail. This flag is used to
 * indicate a hotkey file is pending to be parsed.
 */
static bool hotkeys_file_pending = false;

/** \brief  The hotkeys system is initialized
 *
 * This flag is used to determine if a new HotkeyFile resource value can be
 * used to parse a hotkeys file, or if it needs to be marked as pending,
 * waiting for the UI to be fully initialized.
 */
static bool hotkeys_init_done = false;

/** \brief  Log instance for hotkeys
 */
static log_t hotkeys_log;

/** \brief  Debugging enable flag
 *
 * Can be set via `!debug [enable|disable|on|off]` in a .vhk file.
 */
static bool hotkeys_debug = false;



/* {{{ VICE resources, command line options and their handlers */

/** \brief  String type resources
 */
static resource_string_t resources_string[] = {
    { "HotkeyFile", "", RES_EVENT_NO, NULL,
      &hotkeys_file, hotkeys_file_set, NULL },
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


/** \brief  Set hotkey file and process its contents
 *
 * \param[in]   val     new hotkey file
 * \param[in]   param   extra argument (unused)
 *
 * \return  0 on success, -1 on error
 */
static int hotkeys_file_set(const char *val, void *param)
{
    if (util_string_set(&hotkeys_file, val) != 0) {
        /* new value was the same as the old value, don't do anything */
        return 0;
    }

    /* process hotkeys */
    if (help_requested) {
        return 0;
    }

    if (val != NULL && *val != '\0') {
        if (hotkeys_init_done) {
            /* UI is properly initialized, directly parse the hotkeys file */
            log_message(hotkeys_log, "Hotkeys: parsing '%s':", val);
            ui_hotkeys_parse(val);
            hotkeys_file_pending = false;
        } else {
            /* UI is not yet fully initialized, mark parsing of hotkeys file
             * pending */
            hotkeys_file_pending = true;
        }
    }
    return 0;
}
/* }}} */


/*
 * Hotkey mappings data and functions
 */


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


/** \brief  Load the default hotkeys
 *
 * Parse the VICE-provided hotkey files, clearing any user-defined hotkeys.
 * Also set the "HotkeyFile" resource to "".
 */
void ui_hotkeys_load_default(void)
{
    bool result;

    /* kludge:  We use `machine_name` to determine where to load the default
     *          hotkeys from, and `machine_name` is C64 for VSID, so we load
     *          from ${DATADIR}/C64 for vsid.
     */
    if (machine_class == VICE_MACHINE_VSID) {
        log_message(hotkeys_log, "Hotkeys: Parsing VSID hotkeys file:");
        result = ui_hotkeys_parse(VHK_DEFAULT_NAME_VSID);
    } else {
        log_message(hotkeys_log, "Hotkeys: Parsing %s hotkeys file:", machine_name);
        result = ui_hotkeys_parse(VHK_DEFAULT_NAME);
    }
    if (result) {
        log_message(hotkeys_log, "Hotkeys: OK.");
        /* clear the custom hotkeys file resource */
        resources_set_string("HotkeyFile", "");
    } else {
        log_message(hotkeys_log, "Hotkeys: Failed, continuing anyway.");
    }
}


/** \brief  Initialize hotkeys
 *
 * Initialize hotkey resources such as logs and objects; load default hotkeys.
 *
 * \note    Does *not* initialize command line options or vice resources, that
 *          is done separately in hotkeys_cmdline_options_init() and
 *          hotkeys_resources_init().
 */
void ui_hotkeys_init(void)
{
    hotkeys_log = log_open("HOTKEYS");
    log_message(hotkeys_log, "Hotkeys: Initializing.");
    /* When we get to there the UI has been initialized */
    hotkeys_init_done = true;

    if (hotkeys_file == NULL || *hotkeys_file == '\0') {
        ui_hotkeys_load_default();
    } else {
        if (hotkeys_file_pending) {
            /* We have a peding hotkeys file to parse */
            ui_hotkeys_parse(hotkeys_file);
            hotkeys_file_pending = false;
        }
    }
}


/** \brief  Clean up resources used by the hotkeys
 *
 * \note    Call on emulator shutdown
 */
void ui_hotkeys_shutdown(void)
{
    if (hotkeys_file != NULL) {
        lib_free(hotkeys_file);
        hotkeys_file = NULL;
    }
    if (hotkeys_file_default != NULL) {
        lib_free(hotkeys_file_default);
        hotkeys_file_default = NULL;
    }
    hotkey_map_shutdown();
    log_close(hotkeys_log);
}


/* This fails on msys2 and I don't want to spend too much time (heh) on this.
 * Can be reenabled if required. */
#if 0
/** \brief  Log current time, including microseconds
 */
static void hotkeys_log_timestamp(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, NULL) == 0) {
        const struct tm *tm;
        char buffer[256];

        tm = localtime(&(tv.tv_sec));
        strftime(buffer, sizeof(buffer), "%H:%M.%S", tm);
        log_message(hotkeys_log, "Hotkeys: timestamp: %s.%lld", buffer, (long long)tv.tv_usec);
    }
}
#endif



/* {{{ textfile_reader_t methods */

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
    entry->linenum = 0;
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
        reader->fp = NULL;
    }
    /* unwind stack */
    if (reader->entries != NULL) {
        textfile_entry_free_all(reader->entries);
    }
}


/** \brief  Open a new file
 *
 * Open a new file and remember the position in the previous file, if any.
 *
 * If a previous file was open, close it and remember its stream position and
 * line number, open the new file and push it on top of the stack.
 *
 * \param[in]   reader  textfile reader
 * \param[in]   path    path to new file
 *
 * \return  bool
 */
static bool textfile_reader_open(textfile_reader_t *reader, const char *path)
{
    textfile_entry_t *current;
    char *complete_path = NULL;

    /* get top of stack */
    current = reader->entries;
    if (current != NULL) {

        if (reader->fp == NULL) {
            debug_gtk3("ERROR: file entry on stack, but no open FP.");
        }
        /* remember position in current file */
        current->pos = ftell(reader->fp);
        /* close current file */
        fclose(reader->fp);
        reader->fp = NULL;
    }

    /* try to open new file */
    /*  debug_gtk3("calling sysfile_open(%s, %s)", path, machine_name); */
    /* first try DATADIR/$MACHINE */
    reader->fp = sysfile_open(path, machine_name, &complete_path, "rb");
    if (reader->fp == NULL) {
        /*  debug_gtk3("Failed, trying without machine_name %s", machine_name); */
        /* try DATADIR (without $MACHINE) */
        reader->fp = sysfile_open(path, "", &complete_path, "rb");
        if (reader->fp == NULL) {
            /* give up */
            /*  debug_gtk3("Failed, giving up.");   */
            return false;
        }
    }
    /*  debug_gtk3("OK: %s", complete_path); */

    /* add new file to stack */
    textfile_entry_t *new = textfile_entry_new(complete_path);
    lib_free(complete_path);
    new->next = current;
    reader->entries = new;

    return true;
}


/** \brief  Close current file, reopen any previous file
 *
 * Closes the current file and reopens and repositions any previous file.
 *
 * \param[in]   reader  textfile reader
 *
 * \return  true if a previous file was reopened
 */
static bool textfile_reader_close(textfile_reader_t *reader)
{
    textfile_entry_t *current = reader->entries;
    textfile_entry_t *next;


    if (reader->fp != NULL) {
#if 0
        debug_gtk3("Current file getting closed: '%s'.", current->path);
        debug_gtk3("Closing reader->fp.");
        debug_gtk3("FCLOSE(%p)", (void*)(reader->fp));
#endif
        fclose(reader->fp);
        reader->fp = NULL;
    } else {
        debug_gtk3("CLOSE called when no file is open.");
        return false;
    }

    if (current != NULL) {

        /* close current file and free memory */
        next = current->next;
        textfile_entry_free(current);
        current = next;
        reader->entries = current;

        /* reopen previous file on stack, if any */
        if (current != NULL) {
            if (hotkeys_debug) {
                log_message(hotkeys_log,
                            "Hotkeys: Reopening previous file '%s'.",
                            current->path);
            }
            reader->fp = fopen(current->path, "rb");
            if (reader->fp == NULL) {
                log_message(hotkeys_log,
                            "failed to open '%s'.",
                            current->path);
                return false;
            }
            /* reposition stream to the position it had when a new file was
             * opened */

            if (fseek(reader->fp, current->pos, SEEK_SET) != 0) {
                debug_gtk3("FSEEK FAIL!");
                return false;
            }
            return true;
        }
        return false;
    }
    return false;
}


/** \brief  Read a single line of text
 *
 * Reads a single line of text from the file on top of the stack.
 *
 * \param[in]   reader  textfile reader
 *
 * \return  pointer to line buffer in \a reader or `NULL` when EOF
 *
 * \todo    Also returns `NULL` on I/O error, I'll need a way to differenciate
 *          between EOF and I/O errors, akin to feof(3).
 */
static const char *textfile_reader_read(textfile_reader_t *reader)
{
    reader->buflen = 0;

    if (reader->entries == NULL) {
        debug_gtk3("NO ENTRIES.");
        return NULL;
    }
    if (reader->fp == NULL) {
        debug_gtk3("NO FP!");
        return NULL;
    }

    while (true) {
        int ch = fgetc(reader->fp);

        if (ch == EOF) {
            if (feof(reader->fp)) {
                /* end of file: close file and get previous one */
                if (!textfile_reader_close(reader)) {
                    /* no more files */
                    return NULL;
                }
            } else {
                /* error */
                debug_gtk3("READ ERROR: %d: %s", errno, strerror(errno));
                return NULL;
            }
        } else {
            /* add character to buffer */
            if (reader->bufsize == reader->buflen) {
                /* resize buffer */
                reader->bufsize *= 2;
                reader->buffer = lib_realloc(reader->buffer, reader->bufsize);
            }
            reader->buffer[reader->buflen] = ch;

            /* newline? */
            if (ch == '\n') {
                reader->buffer[reader->buflen] = '\0';
                /* delete CR if present */
                if (reader->buflen > 0
                        && reader->buffer[reader->buflen - 1] == '\r') {
                    reader->buffer[--(reader->buflen)] = '\0';
                }
                /* update line number and return buffer */
                reader->entries->linenum++;
                return reader->buffer;
            }
            reader->buflen++;
        }
    }
}


/** \brief  Get line number of current file
 *
 * Get the line number of the currently opened file in \a reader.
 *
 * \param[in]   reader  textfile reader
 *
 * \return  line number or -1 when no file is open
 */
static long textfile_reader_linenum(const textfile_reader_t *reader)
{
    if (reader->entries != NULL) {
        return reader->entries->linenum;
    } else {
        return -1;
    }
}


/** \brief  Get name of current file
 *
 * Get the path of the currently opened file of \a reader.
 *
 * \param[in]   reader  textfile reader
 *
 * \return  path or "<null>" when no file is open
 */
static const char *textfile_reader_filename(const textfile_reader_t *reader)
{
    if (reader->entries != NULL) {
        return reader->entries->path;
    } else {
        return "<null>";
    }
}


/* }}} */



/* {{{ Parser methods and helpers */

/** \brief  Strip leading and trailing whitespace
 *
 * Remove leading and trailing whitespace from string \a s.
 *
 * \param[in]   s   string
 *
 * \return  heap-allocated string
 *
 * \note    free with lib_free()
 * \todo    move into src/lib.c?
 */
static char *parser_strtrim(const char *s)
{
    char *t;

    s = util_skip_whitespace(s);

    if (*s == '\0') {
        /* empty string */
        t = lib_calloc(1, 1);
    } else {
        /* trim trailing whitespace */
        const char *p = util_skip_whitespace_trailing(s);
        p++;
        /* add 1 for the terminating nul char */
        t = lib_malloc(p - s + 1);
        memcpy(t, s, p - s);
        t[p - s] = '\0';
    }
    return t;
}


/** \brief  Substitute a substring with another string
 *
 * Replace first occurences of \a search in \a original with \a replace.
 *
 * \param[in]   original    original string
 * \param[in]   search      (sub)string of \a original to replace
 * \param[in]   replace     replacement for \a search
 *
 * \return  heap-allocated string
 *
 * \note    free result with lib_free()
 */
static char *parser_strsubst(const char *original,
                             const char *search,
                             const char *replace)
{
    char *result = NULL;

    if (original == NULL || *original == '\0') {
        /* empty string, return empty string */
        result = lib_calloc(1, 1);
    } else if (search == NULL || *search == '\0') {
        /* no search string, return copy of original */
        result = lib_strdup(original);
    } else {
        /* start scanning */
        size_t slen = strlen(search);
        size_t olen = strlen(original);

        if (slen > olen) {
            /* search string longer than original, will never find a match */
            result = lib_strdup(original);
        } else {
            /* search and destroy! */
            const char *pos = strstr(original, search);
            if (pos == NULL) {
                /* no match, return copy of original */
                result = lib_strdup(original);
            } else {
                const char *opos;    /* position in original */
                char *rpos;          /* position in result */
                size_t reslen;
                size_t rlen = strlen(replace);

                /* avoid juggling with conversion/promotion */
                if (slen >= rlen) {
                    /* result string will be shorter or the same size */
                    reslen = olen - (slen - rlen);
                } else {
                    /* result string will be larger */
                    reslen = olen + (rlen - slen);
                }

                result = rpos = lib_malloc(reslen + 1);
                opos = original;

                /* copy slice before the substitution, if any */
                if (pos > original) {
                    memcpy(rpos, original, pos - original);
                    rpos += pos - original;
                    opos += pos - original;
                }
                /* copy substitution */
                memcpy(rpos, replace, rlen);
                rpos += rlen;
                opos += slen;
                /* copy slice after the substitution, if any */
                if (*opos != '\0') {
                    strcpy(rpos, opos);
                }
                /* terminate string */
                result[reslen] = '\0';
            }

        }
    }
    return result;
}


/** \brief  Scan string for keyword
 *
 * Scan string \a name for a keyword and return keyword ID on match.
 *
 * The input is allowed to have trailing characters starting with whitespace,
 * which could be the keyword's argument list. The input is expected to not
 * contain the keyword indicator token. Matching is done case-insensitive.
 *
 * \param[in]   name    string with possible keyword
 *
 * \return  keyword ID when found, -1 otherwise
 */
static hotkeys_keyword_id_t parser_get_keyword_id(const char *name)
{
    size_t i = 0;

#if 0
    debug_gtk3("Looking up '%s':", name);
#endif
    for (i = 0; i < ARRAY_LEN(hotkeys_keyword_list); i++) {
        int k = 0;
        const hotkeys_keyword_t *kw = &(hotkeys_keyword_list[i]);
        const char *kwname = kw->name;
#if 0
        debug_gtk3("Checking against '%s'.", kw->name);
#endif
        /* do lower case compare */
        while (kwname[k] != '\0'
                && name[k] != '\0'
                && tolower((int)name[k]) == tolower((int)kwname[k])) {
            k++;
        }
        if (kwname[k] == '\0' && name[k] == '\0') {
            /* full match, return id */
            return kw->id;
        } else if (kwname[k] == '\0') {
            /* input matched keyword so far, but input contains more
             * characters */
            if (isspace((unsigned char)name[k])) {
                /* remaining input cannot be part of a keyword: match */
                return kw->id;
            }
        } else if (name[k] == '\0') {
            /* input matched keyword so far, but keyword contains more
             * characters: keyword is higher in alphabetical sort
             * order, so we cannot get a match */
            return HOTKEYS_KW_ID_ILLEGAL;
        }
        /* keep looking, next keyword is higher in alphabetical sort
         * order than the current keyword and could match */
    }

    /* list exhausted, no match */
    return HOTKEYS_KW_ID_ILLEGAL;
}


/** \brief  Look up GDK modifier mask by \a name
 *
 * Scans the modifiers table for a match with \a name, where name is expected
 * to end with '>' and name should not contain '<'.
 *
 * \param[in]   name    modifier name, without the leading '<'
 * \param[out]  endptr  object to store pointer to closing '>'
 *
 * \return  modifier ID
 */
static hotkeys_modifier_id_t parser_get_modifier_id(const char *name,
                                                    const char **endptr)
{
    int i = 0;

    while (hotkeys_modifier_list[i].name != NULL) {
        int k = 0;
        const hotkeys_modifier_t *mod = &(hotkeys_modifier_list[i]);
        const char *modname = mod->name;
#if 0
        debug_gtk3("Testing '%s' against '%s'.", modname, name);
#endif
        while (modname[k] != '\0'
                && name[k] != '\0'
                && name[k] != HOTKEYS_MODIFIER_CLOSE
                && tolower((int)name[k]) == tolower((int)modname[k])) {
            k++;
        }

        if (modname[k] == '\0' && name[k] == HOTKEYS_MODIFIER_CLOSE) {
            /* match */
            if (endptr != NULL) {
                *endptr = name + k;
            }
            return mod->id;
        } else if (modname[k] == '\0' && !isalpha((unsigned char)name[k])) {
            /* input is longer, but contains characters that cannot match any
             * modifier name */
            return HOTKEYS_MOD_ID_ILLEGAL;
        } else if (name[k] == '\0') {
            /* missing closing '>' */
            return HOTKEYS_MOD_ID_ILLEGAL;
        }
        i++;
    }

    return HOTKEYS_MOD_ID_ILLEGAL;
}


/** \brief  Get GDK modifier mask for modifier \a id
 *
 * \param[in]   id  modifier ID
 *
 * \return  modifier mask or 0 on error
 */
static GdkModifierType parser_get_modifier_mask(hotkeys_modifier_id_t id)
{
    int i = 0;

    while (hotkeys_modifier_list[i].name != NULL) {
        if (hotkeys_modifier_list[i].id == id) {
            return hotkeys_modifier_list[i].mask;
        }
        i++;
    }
    return 0;
}

#if 0
static hotkeys_modifier_id_t parser_get_modifier_id_for_mask(GdkModifierType mask)
{
    int i = 0;

    if (mask == 0) {
        return -1;
    }

    while (hotkeys_modifier_list[i].name != NULL) {
        if (hotkeys_modifier_list[i].mask == mask) {
            return hotkeys_modifier_list[i].id;
        }
        i++;
    }
    return -1;
}
#endif

#if 0
/** \brief  Get modifier name for modifier \a id
 *
 * \param[in]   id  modifier ID
 *
 * \return  name or `NULL` when not found
 */
static const char *parser_get_modifier_name(hotkeys_modifier_id_t id)
{
    int i = 0;

    while (hotkeys_modifier_list[i].name != NULL) {
        if (hotkeys_modifier_list[i].id == id) {
            return hotkeys_modifier_list[i].name;
        }
        i++;
    }
    return NULL;
}
#endif

/** \brief  Parse string for key name and modifiers
 *
 * Parses string \a line for modifier names and a key name and stores the
 * resulting GDK modifier mask in \a mask and the GDK key value in \a keyval.
 *
 * The \a endptr will be set to the first character after the key name in
 * string \a line.
 *
 * \param[in]   line    string to parse
 * \param[out]  endptr  object to store pointer to character after key name
 * \param[in]   reader  textfile reader (for error logging)
 * \param[out]  mask    object to store resulting modifier mask
 * \param[out]  keyval  object to store resulting GDK key value
 *
 * \return  bool
 *
 * \note    On error \a mask will be set to 0 and \a keyval to GDK_KEY_VoidSymbol.
 */
static bool parser_get_gdk_mask_and_keyval(const char *line,
                                           const char **endptr,
                                           textfile_reader_t *reader,
                                           GdkModifierType *mask,
                                           guint *keyval)
{
    GdkModifierType m = 0;
    const char *s = line;
    const char *oldpos;
    char keyname[256];

    /* set return values */
    *keyval = GDK_KEY_VoidSymbol;
    *mask = 0;

    /* collect modifiers */
    while (*s != '\0') {
        const char *end = NULL;
        hotkeys_modifier_id_t id;

#if 0
        debug_gtk3("Scanning '%s'", s);
#endif
        if (*s != HOTKEYS_MODIFIER_OPEN) {
            /* no opening '<' found, assume key name */
            break;
        }

        /* opening '<': parse single modifier */
        s++;
        id = parser_get_modifier_id(s, &end);
        if (id != HOTKEYS_MOD_ID_ILLEGAL) {
#if 0
            log_message(hotkeys_log,
                        "Hotkeys: Found modifier '%s'.",
                        parser_get_modifier_name(id));
#endif
            m |= parser_get_modifier_mask(id);
            s = end + 1;
        } else {
            log_message(hotkeys_log,
                        "Hotkeys: %s:%ld: parse error: unknown modifier.",
                        textfile_reader_filename(reader),
                        textfile_reader_linenum(reader));
            return false;
        }
    }

    /* End of modifiers, now we expect a GDK keyname without the leading
     * 'GDK_KEY_':
     */
    oldpos = s;
    while (*s != '\0' && (isalpha((unsigned char)*s) ||
                          isdigit((unsigned char)*s) ||
                          *s == '_')) {
        s++;
    }
    if (oldpos == s) {
        /* error, no keyname candidate found */
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: no keyname found.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }

    /* copy keyname */
    if (s - oldpos >= sizeof(keyname)) {
        /* key name is way too long, will never match */
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: key name is too long.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }
    memcpy(keyname, oldpos, s - oldpos);
    keyname[s - oldpos] = '\0';

    /* get Gdk keysym */
    *keyval = gdk_keyval_from_name(keyname);
    /* store mask */
    *mask = m;
    /* store endpointer */
    *endptr = s;

    return true;
}

/*
 * Directive handlers
 */

/** \brief  Handler for the '!CLEAR' keyword
 *
 * Clears all hotkeys registered.
 *
 * \param[in]   line    text to parse (unused)
 * \param[in]   reader  textfile reader (unused)
 *
 * \return  bool
 */
static bool parser_do_clear(const char *line, textfile_reader_t *reader)
{
    if (hotkeys_debug) {
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: !CLEAR -> clearing all hotkeys.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
    }
    ui_clear_hotkeys();
    return true;
}


/** \brief  Handler for the '!DEBUG' keyword
 *
 * Enable/disable debugging messages.
 *
 * \param[in]   line    text to parse (unused)
 * \param[in]   reader  textfile reader (unused)
 *
 * \return  bool
 */
static bool parser_do_debug(const char *line, textfile_reader_t *reader)
{
    const char *arg;
    size_t i;
#if 0
    debug_gtk3("Found !DEBUG, check arg.");
#endif
    arg = util_skip_whitespace(line);
    for (i = 0; i < ARRAY_LEN(debug_arglist); i++) {
        if (util_strncasecmp(debug_arglist[i].symbol,
                             arg,
                             strlen(debug_arglist[i].symbol)) == 0) {
#if 0
            debug_gtk3("Found '%s' -> '%s'.",
                       debug_arglist[i].symbol,
                       debug_arglist[i].value ? "True" : "False");
#endif
            /* TODO: report debug on/off as part of debugging, but only
             *       mention 'off' if it was previously 'on': this way using
             *       '!debug off' at the start of a vhk file won't trigger a
             *       debugging message.
             */
            hotkeys_debug = debug_arglist[i].value;
            return true;
        }
    }
    /* no match */
    log_message(hotkeys_log,
                "Hotkeys: %s:%ld: syntax error: unknown argument to !DEBUG, assuming False",
                textfile_reader_filename(reader),
                textfile_reader_linenum(reader));
    hotkeys_debug = false;
    return true;
}


/** \brief  Handler for the '!INCLUDE' a keyword
 *
 * \param[in]   line    text to parse, must start *after* "!INCLUDE"
 * \param[in]   reader  textfile reader
 *
 * \return  bool
 *
 * \note    errors are reported with log_message()
 */
static bool parser_do_include(const char *line, textfile_reader_t *reader)
{
    const char *s;
    char *arg;
    char *a;
    bool result;

    s = util_skip_whitespace(line);
    if (*s == '\0') {
        /* missing argument */
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: missing argument for !INCLUDE",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }

    /* quotes? */
    if (*s == HOTKEYS_QUOTE) {
        /* allocate memory for resulting arg:
         * -2 to skip enclosing quotes,
         * +1 for terminating nul */
        arg = a = lib_malloc(strlen(line) - 2 + 1);

        /* copy string, turning any '\"' sequence into '"' */
        s++;    /* move after opening quote */
        while (*s != '\0') {
            if (*s == HOTKEYS_ESCAPE) {
                s++;
                if (*s == '\0') {
                    /* end of string, but escape token active */
                    log_message(hotkeys_log,
                                "Hotkeys: %s:%ld: "
                                "parse error: unexpected end of line.",
                                textfile_reader_filename(reader),
                                textfile_reader_linenum(reader));
                    lib_free(arg);
                    return false;
                }
                *a++ = *s++;
            } else if (*s == HOTKEYS_QUOTE) {
                /* found closing quote */
                break;
            } else {
                *a++ = *s++;
            }
        }
        if (*s != HOTKEYS_QUOTE) {
            /* error, no closing quote */
            log_message(hotkeys_log,
                        "Hotkeys: %s:%ld: "
                        "parse error: missing closing quote.",
                        textfile_reader_filename(reader),
                        textfile_reader_linenum(reader));
            lib_free(arg);
            return false;
        }
        /* terminate result */
        *a = '\0';
    } else {
        /* no quotes, copy until first whitespace character */
        arg = a = lib_malloc(strlen(line) + 1);
        while (*s != '\0' && !isspace((unsigned char)*s)) {
            *a++ = *s++;
        }
        *a = '\0';
    }

    /* Fuck with the path separators to work around crappy findpath()
     *
     * With the $VICEDIR stuff in place we would pass in an absolute path to
     * sysfile_open()/findpath(), but with that removed findpath() fucks up
     * on directory separators in its argument and considers anything with a
     * separator in it to be relative to the user's directory, not the vice dir.
     *
     * So in order to fool findpath() we change the OS-native directory separators
     * to the non-native separators */
    for (int i = 0; arg[i] != '\0'; i++) {
#ifdef WINDOWS_COMPILE
        if (arg[i] == '\\') {
            arg[i] = '/';
        }
#else
        if (arg[i] == '/') {
            arg[i] = '\\';
        }
#endif
    }

    result = textfile_reader_open(reader, arg);
    lib_free(arg);
    return result;
}


/** \brief  Handler for the '!UNDEF' a keyword
 *
 * \param[in]   line    text to parse
 * \param[in]   reader  textfile reader
 *
 * \return  bool
 *
 * \note    errors are reported with log_message()
 *
 * \todo    Support two windows for x128
 */
static bool parser_do_undef(const char *line, textfile_reader_t *reader)
{
    const char *s;
    const char *oldpos;
    GdkModifierType mask;
    guint keyval;
    hotkey_map_t *map;

    s = util_skip_whitespace(line);
    if (*s == '\0') {
        /* error: missing argument */
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: missing argument for !UNDEF",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return true;
    }

    /* get combined modifier masks and keyval */
    oldpos = s;
    if (!parser_get_gdk_mask_and_keyval(s, &s, reader, &mask, &keyval)) {
        /* error already logged */
        return false;
    }
    if (hotkeys_debug) {
        log_message(hotkeys_log,
                    "Hotkeys %s:%ld: mask: %04x, keyval: %08x, keyname: %s.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader),
                    (unsigned int)mask,
                    keyval,
                    gdk_keyval_name(keyval));
    }

    /* lookup map for hotkey */
    map = hotkey_map_get_by_hotkey(keyval, mask);
    if (map != NULL) {
        if (hotkeys_debug) {
            log_message(hotkeys_log,
                        "Hotkeys: %s:%ld: found hotkey defined for action %d (%s),"
                        " clearing.",
                        textfile_reader_filename(reader),
                        textfile_reader_linenum(reader),
                        map->action, ui_action_get_name(map->action));
        }
        hotkey_map_clear_hotkey(map);
    } else {
        /* cannot use gtk_accelerator_name(): Gtk throws a fit about not having
         * a display and thus no GdkKeymap. :( */
#if 0

        char *accel_name = gtk_accelerator_name(keyval, mask);
#endif
        /* do it the hard way: copy argument without trailing crap */
        char accel_name[256];
        size_t accel_len;

        accel_len = (size_t)(s - oldpos);
        if (accel_len >= sizeof(accel_name)) {
            accel_len = sizeof(accel_name) - 1;
        }
        memcpy(accel_name, oldpos, accel_len);
        accel_name[accel_len] = '\0';

        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: warning: hotkey '%s' not found!",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader),
                    accel_name);
    }
    return true;
}


/** \brief  Handle a keyword
 *
 * \param[in]   line    text to parse, must start *after* the '!' indicator
 * \param[in]   reader  textfile reader
 *
 * \return  bool
 *
 * \note    errors are reported with log_message()
 */
static bool parser_handle_keyword(const char *line, textfile_reader_t *reader)
{
    bool result = true;

    if (*line == '\0') {
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: syntax error, missing keyword after '!'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        result =  false;
    } else if (!isalpha((unsigned char)*line)) {
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: syntax error, illegal character after '!'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        result = false;
    } else {
        /* get keyword ID */
        hotkeys_keyword_id_t id = parser_get_keyword_id(line);

        switch (id) {
            case HOTKEYS_KW_ID_CLEAR:
                /* handle CLEAR */
                result = parser_do_clear(line, reader);
                break;

            case HOTKEYS_KW_ID_DEBUG:
                /* handle DEBUG */
                result = parser_do_debug(line + 6, reader);
                break;

            case HOTKEYS_KW_ID_INCLUDE:
                /* handle INCLUDE */
                result = parser_do_include(line + 8, reader);
                break;

            case HOTKEYS_KW_ID_UNDEF:
                /* handle UNDEF */
                result = parser_do_undef(line + 6, reader);
                break;

            case HOTKEYS_KW_ID_IFDEF:
                /* handle IFDEF */
                log_message(hotkeys_log, "Hotkeys: TODO: handle !IFDEF.");
                break;

            case HOTKEYS_KW_ID_IFNDEF:
                /* handle IFNDEF */
                log_message(hotkeys_log, "Hotkeys: TODO: handle !IFNDEF.");
                break;

            case HOTKEYS_KW_ID_ENDIF:
                /* handle ENDIF */
                log_message(hotkeys_log, "Hotkeys: TODO: handle !ENDIF.");
                break;

            default:
                /* unknown keyword */
                log_message(hotkeys_log,
                            "Hotkeys: %s:%ld: syntax error, unknown keyword.",
                            textfile_reader_filename(reader),
                            textfile_reader_linenum(reader));
                result = false;
        }
    }

    return result;
}



/** \brief  Handle a hotkey mapping
 *
 * Parse \a line for a hotkey mapping, set hotkey on success.
 *
 * \param[in]   line    string to parse, expected to be trimmed
 * \param[in]   reader  textfile reader
 *
 * \return  bool
 *
 * \note    errors are reported with log_message()
 */
static bool parser_handle_mapping(const char *line, textfile_reader_t* reader)
{
    const char *s;
    const char *oldpos;
    char action_name[256];
    int action_id = ACTION_INVALID;
    guint keysym = 0;
    GdkModifierType mask = 0;
    hotkey_map_t *map;

    s = line;

    /* get action name */
    oldpos = s;
    /* TODO: support quotes? */
    while (*s != '\0' && IS_ACTION_NAME_CHAR((int)*s)) {
        s++;
    }
    /* check for errors */
    if (s == oldpos) {
        /* no valid action name tokens found */
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: missing action name.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }
    if (s - oldpos >= sizeof(action_name)) {
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: action name is too long.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }

    /* make properly nul-terminated string of action name for lookups*/
    memcpy(action_name, oldpos, s - oldpos);
    action_name[s - oldpos] = '\0';

    s = util_skip_whitespace(s);

    /* get combined modifier masks and keyval */
    if (!parser_get_gdk_mask_and_keyval(s, &s, reader, &mask, &keysym)) {
        /* error already logged */
        return false;
    }

    if (hotkeys_debug) {
        log_message(hotkeys_log,
                    "Hotkeys: mask: %04x, keysym: %08x, keyname: %s, action: %s",
                    (unsigned int)mask, keysym, gdk_keyval_name(keysym), action_name);
    }

    /* finally try to register the hotkey */
    action_id = ui_action_get_id(action_name);
    if (action_id <= ACTION_NONE) {
        log_message(hotkeys_log,
                    "Hotkeys: %s:%ld: error: unknown action '%s'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader),
                    action_name);
        /* allow parsing to continue */
        return true;
    }

    /* register mapping, first try looking up the item */
    map = hotkey_map_get_by_action(action_id);
    if (map != NULL) {
#if 0
        debug_gtk3("Got mapping for action %d, updating with hotkey data.",
                    action_id);
#endif
        map->keysym = keysym;
        map->modifier = mask;
    } else {
#if 0
        debug_gtk3("Couldn't find mapping for action %d, allocating new mapping.",
                    action_id);
#endif
        map = hotkey_map_new();
        map->action = action_id;
        map->keysym = keysym;
        map->modifier = mask;
        hotkey_map_append(map);
    }

    /* set hotkey for menu item, if it exists */
    hotkey_map_setup_hotkey(map);
   return true;
}


/** \brief  Parse hotkeys file
 *
 * \param[in]   path    path to hotkeys file
 *
 * \return  bool
 */
bool ui_hotkeys_parse(const char *path)
{
    textfile_reader_t reader;
    bool status = true;

    /* disable debugging */
    hotkeys_debug = false;
#if 0
    hotkeys_log_timestamp();
#endif
    /* initialize file stack and open the file */
    textfile_reader_init(&reader);
    if (textfile_reader_open(&reader, path)) {
        /* main hotkeys file */

        while (true) {
            const char *line = textfile_reader_read(&reader);

            if (line != NULL) {
                bool parse_ok = true;
                char *trimmed = parser_strtrim(line);

                if (hotkeys_debug) {
                    log_debug("LINE %3ld: '%s'",
                              textfile_reader_linenum(&reader),
                              line);
                }

                if (*trimmed == '\0'
                        || *trimmed == HOTKEYS_COMMENT
                        || *trimmed == HOTKEYS_COMMENT_ALT) {
                    /* empty line or comment, skip */
                    parse_ok = true;
                } else if (*trimmed == HOTKEYS_KEYWORD) {
                    /* found keyword */
                    parse_ok = parser_handle_keyword(trimmed + 1, &reader);
                } else {
                    /* assume hotkey definition */
                    parse_ok = parser_handle_mapping(trimmed, &reader);
                }

                /* free trimmed line and check for parse result of line */
                lib_free(trimmed);
                if (!parse_ok) {
                    /* assume error already logged */
                    textfile_reader_free(&reader);
                    return false;
                }
            } else {
                /* TODO: check if EOF or error */
                break;
            }
        }

        textfile_reader_close(&reader);

    } else {
        /* failed to open main file */
        status = false;
    }
    textfile_reader_free(&reader);
#if 0
    hotkeys_log_timestamp();
#endif
    return status;
}

/* }}} */


/** \brief  Return a string describing the key+modifiers for \a action
 *
 * \param[in]   action_id   action ID
 *
 * \return  string with key name and modifiers
 * \note    free the string after use with lib_free()
 */
char *ui_hotkeys_get_hotkey_string_for_action(gint action_id)
{
    char *str = NULL;
    hotkey_map_t *map = hotkey_map_get_by_action(action_id);

    if (map != NULL) {
        str = gtk_accelerator_get_label(map->keysym, map->modifier);
        if (str != NULL) {
            /* make VICE take ownership */
            char *tmp = lib_strdup(str);
            g_free(str);
            str = tmp;
        }
    }
    return str;
}


/** \brief  Helper: log I/O error
 *
 * Logs libc I/O errors to the hotkeys log, including errno and description.
 */
static void export_log_io_error(void)
{
    log_error(hotkeys_log,
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
    int result;
    char buffer[1024];
    time_t t;
    const struct tm *tinfo;

    result = fprintf(fp,
"# Gtk3 hotkeys file for %s\n"
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

    /* add ViCE version */
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
    hotkey_map_t *map;

    log_message(hotkeys_log,
                "Hotkeys: exporting current hotkeys to '%s'.", path);

    fp = fopen(path, "wb");
    if (fp == NULL) {
        log_error(hotkeys_log,
                  "Hotkeys: failed to open '%s' for writing (%d: %s).",
                  path, errno, strerror(errno));
        return false;
    }

    export_header(fp);

    for (map = hotkey_map_get_head(); map != NULL; map = map->next) {
        if (map->keysym > 0) {
            gchar *accel;
            int result;

            accel = gtk_accelerator_name(map->keysym, map->modifier);
            if (accel != NULL) {
                char *hotkey;
                const char *name;

                hotkey = parser_strsubst(accel, "Primary", PRIMARY_REPLACEMENT);
                g_free(accel);
                name = ui_action_get_name(map->action);

                result = fprintf(fp, "%-30s  %s\n", name, hotkey);
                if (result < 0) {
                    export_log_io_error();
                    lib_free(hotkey);
                    fclose(fp);
                    return false;
                }
                lib_free(hotkey);
            }
        }
    }

    fclose(fp);
    return true;
}


const hotkeys_modifier_t *ui_hotkeys_get_modifier_list(void)
{
    return hotkeys_modifier_list;
}

