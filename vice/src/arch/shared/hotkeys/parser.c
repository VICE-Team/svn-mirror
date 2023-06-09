/** \file   parser.c
 * \brief   Parsing functions for hotkeys
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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "lib.h"
#include "log.h"
#include "textfilereader.h"
#include "uiactions.h"
#include "uiapi.h"
#include "uihotkeys.h"
#include "util.h"
#include "vhkkeysyms.h"

#include "parser.h"


/* #define DEBUG_VHK */
#include "vhkdebug.h"


/** \brief  Array length helper
 *
 * \param[in]   arr array name
 */
#define ARRAY_LEN(arr)  (sizeof (arr) / sizeof (arr[0]) )



/** \brief  Object for mapping of !DEBUG arguments to boolean
 */
typedef struct debug_args_s {
    const char *symbol; /**< string literal */
    bool        value;  /**< boolean value */
} debug_args_t;


/** \brief  Hotkeys parser keyword list
 *
 * List of parser keywords with ID and argument count.
 *
 * \note    The array needs to stay in alphabetical order.
 */
static const vhk_keyword_t vhk_keyword_list[] = {
    { "clear",      VHK_KW_ID_CLEAR,    0, 0,
      "!CLEAR",
      "Clear all hotkeys" },

    { "debug",      VHK_KW_ID_DEBUG,    1, 1,
      "!DEBUG <enable|disable|on|off>",
      "Turn debugging output on or off" },

    { "endif",      VHK_KW_ID_ENDIF,    0, 0,
      "!ENDIF",
      "End of !IF[N]DEF" },

    { "ifdef",      VHK_KW_ID_IFDEF,    1, 1,
      "!IFDEF <condition>",
      "Execute following statement(s) if <condition> is met" },

    { "ifndef",      VHK_KW_ID_IFNDEF,  1, 1,
      "!IFNDEF <condition>",
      "Execute following statement(s) if <condition> is not met" },

    { "include",    VHK_KW_ID_INCLUDE,  1, 1,
      "!INCLUDE <path>",
      "Start parsing hotkeys file <path>" },

    { "undef",      VHK_KW_ID_UNDEF,    1, 1,
      "!UNDEF <modifiers+key>",
      "Undefine a hotkey by <modifiers+key>" }
};


/** \brief  Mapping of !DEBUG arguments to boolean
 */
static const debug_args_t debug_arglist[] = {
    { "enable",     true },
    { "disable",    false },
    { "on",         true },
    { "off",        false }
};


/** \brief  Debug messages enable flag
 */
static bool vhk_debug = false;


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
static char *vhk_parser_strtrim(const char *s)
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

/* currently unused, but might be required again later, so do not delete */
#if 0
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
static char *vhk_parser_strsubst(const char *original,
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
                char       *rpos;    /* position in result */
                size_t      reslen;
                size_t      rlen = strlen(replace);

                /* avoid juggling with conversion/promotion */
                if (slen >= rlen) {
                    /* result string will be shorter or the same size */
                    reslen = olen - (slen - rlen);
                } else {
                    /* result string will be larger */
                    reslen = olen + (rlen - slen);
                }

                rpos   = lib_malloc(reslen + 1);
                opos   = original;
                result = rpos;

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
#endif

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
static vhk_keyword_id_t vhk_parser_get_keyword_id(const char *name)
{
    size_t i = 0;

#if 0
    debug_vhk("Looking up '%s':", name);
#endif
    for (i = 0; i < ARRAY_LEN(vhk_keyword_list); i++) {
        int k = 0;
        const vhk_keyword_t *kw = &(vhk_keyword_list[i]);
        const char *kwname = kw->name;
#if 0
        debug_vhk("Checking against '%s'.", kw->name);
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
            return VHK_KW_ID_ILLEGAL;
        }
        /* keep looking, next keyword is higher in alphabetical sort
         * order than the current keyword and could match */
    }

    /* list exhausted, no match */
    return VHK_KW_ID_ILLEGAL;
}

/** \brief  Parse string for modifier
 *
 * Parse string \a s for a known modifier name closed by '\>', store pointer
 * to closing angled bracket in \a endptr (if not `NULL`).
 * Matching is done case-insensitive.
 *
 * \param[in]   s       string to parse
 * \param[out]  endptr  pointer to closing bracket (can be `NULL`)
 *
 * \return  modifier mask or `VHK_MOD_NONE` (0) when no valid modifier found
 */
static uint32_t parser_get_modifier(const char *s, const char **endptr)
{
    return vhk_modifier_from_name(s, endptr);
#if 0
    size_t i;

    if (s == NULL || *s == '\0') {
        return VHK_MOD_NONE;
    }

    for (i = 0; ARRAY_LEN(vhk_modifier_list); i++) {
        const vhk_modifier_t *mod;
        const char           *name;
        int                   k = 0;

        mod  = &(vhk_modifier_list[i]);
        name = mod->name;

        while (name[k] != '\0'
                && s[k] != '\0' && s[k] != VHK_MODIFIER_CLOSE
                && tolower((unsigned char)s[k]) == tolower((unsigned char)name[k])) {
            k++;
        }

        if (name[k] == '\0' && s[k] == VHK_MODIFIER_CLOSE) {
            /* end of name in table and closing bracket: match */
            if (endptr != NULL) {
                *endptr = s + k;
            }
            return mod->mask;
        }
    }
    return VHK_MOD_NONE;
#endif
}

/** \brief  Parse string for keysym and optional modifiers
 *
 * \param[in]   line            string to parse
 * \param[out]  endptr          first character in \a line past the key name
 * \param[out]  vice_keysym     VICE modifier mask
 * \param[out]  vice_modmask    VICE keysym
 * \param[out]  arch_keysym     arch keysym
 * \param[out]  arch_modmask    arch modifier mask
 *
 * \return  `true` on success
 */
static bool vhk_parser_get_keysym_and_modmask(const char         *line,
                                              const char        **endptr,
                                              textfile_reader_t  *reader,
                                              uint32_t           *vice_keysym,
                                              uint32_t           *vice_modmask,
                                              uint32_t           *arch_keysym,
                                              uint32_t           *arch_modmask)
{
    const char *curpos;
    const char *oldpos;
    char        keyname[256];
    ptrdiff_t   keylen;
    uint32_t    v_key;
    uint32_t    a_key;
    uint32_t    v_mask = 0;
    uint32_t    a_mask = 0;


    curpos = line;

    if (arch_keysym != NULL) {
        *arch_keysym = 0;
    }
    if (arch_modmask != NULL) {
        *arch_modmask = 0;
    }
    if (vice_keysym != NULL) {
        *vice_keysym = 0;
    }
    if (vice_modmask != NULL) {
        *vice_modmask = 0;
    }

    /* collect modifiers */
    while (*curpos != '\0') {
        uint32_t    v_mod;
        const char *end = NULL;

        if (*curpos != VHK_MODIFIER_OPEN) {
            /* no opening '<' found, must be key name */
            break;
        }

        curpos++;   /* skip '<' */
        v_mod = parser_get_modifier(curpos, &end);
        if (v_mod == VHK_MOD_NONE) {
            log_message(vhk_log,
                        "Hotkeys: %s:%ld: parse error unknown modifier.",
                        textfile_reader_filename(reader),
                        textfile_reader_linenum(reader));
            return false;
        }
        /* add found modifier to final mask */
        v_mask |= v_mod;
        a_mask |= ui_hotkeys_arch_modifier_to_arch(v_mod);
        curpos = end + 1;   /* move past closing '>' */
    }

    /* end of modifiers, get key name */
    oldpos = curpos;
    while (*curpos != '\0' && (isalpha((unsigned char)*curpos) ||
                               isdigit((unsigned char)*curpos) ||
                               *curpos == '_')) {
        curpos++;
    }
    keylen = curpos - oldpos;
    if (keylen == 0) {
        /* error, no key name found */
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: parser error: no keyname found.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }
    if (keylen >= (ptrdiff_t)sizeof keyname) {
        /* key name is way too long, will never match */
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: parse error: key name exceeds allowed size.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }
    memcpy(keyname, oldpos, keylen);
    keyname[keylen] = '\0';

    /* set results */
    v_key = vhk_keysym_from_name(keyname);
    a_key = ui_hotkeys_arch_keysym_to_arch(v_key);

    debug_vhk("VICE vhk key name  = \"%s\"",keyname);
    debug_vhk("VICE keysym + mods = %04x, %08x", v_key, v_mask);
    debug_vhk("arch keysym + mods = %04x, %08x", a_key, a_mask);

    if (vice_keysym != NULL) {
        *vice_keysym = v_key;
    }
    if (vice_modmask != NULL) {
        *vice_modmask = v_mask;
    }
    if (arch_keysym != NULL) {
        *arch_keysym = a_key;
    }
    if (arch_modmask != NULL) {
        *arch_modmask = a_mask;
    }
    if (endptr != NULL) {
        *endptr = curpos;
    }

    return true;
}


/*
 * .vhk file directive handlers
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
static bool vhk_parser_do_clear(const char *line, textfile_reader_t *reader)
{
    if (vhk_debug) {
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: !CLEAR -> clearing all hotkeys.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
    }
    ui_hotkeys_remove_all();
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
static bool vhk_parser_do_debug(const char *line, textfile_reader_t *reader)
{
    const char *arg;
    size_t i;

    arg = util_skip_whitespace(line);
    for (i = 0; i < ARRAY_LEN(debug_arglist); i++) {
        if (util_strncasecmp(debug_arglist[i].symbol,
                             arg,
                             strlen(debug_arglist[i].symbol)) == 0) {
            /* TODO: report debug on/off as part of debugging, but only
             *       mention 'off' if it was previously 'on': this way using
             *       '!debug off' at the start of a vhk file won't trigger a
             *       debugging message.
             */
            vhk_debug = debug_arglist[i].value;
            return true;
        }
    }
    /* no match */
    log_message(vhk_log,
                "Hotkeys: %s:%ld: syntax error: unknown argument to !DEBUG, assuming False",
                textfile_reader_filename(reader),
                textfile_reader_linenum(reader));
    vhk_debug = false;
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
static bool vhk_parser_do_include(const char *line, textfile_reader_t *reader)
{
    const char *s;
    char       *arg;
    char       *a;
    bool        result;

    s = util_skip_whitespace(line);
    if (*s == '\0') {
        /* missing argument */
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: error: missing argument for !INCLUDE",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }

    /* quotes? */
    if (*s == VHK_QUOTE) {
        /* allocate memory for resulting arg:
         * -2 to skip enclosing quotes,
         * +1 for terminating nul */
        arg = a = lib_malloc(strlen(line) - 2 + 1);

        /* copy string, turning any '\"' sequence into '"' */
        s++;    /* move after opening quote */
        while (*s != '\0') {
            if (*s == VHK_ESCAPE) {
                s++;
                if (*s == '\0') {
                    /* end of string, but escape token active */
                    log_message(vhk_log,
                                "Hotkeys: %s:%ld: "
                                "parse error: unexpected end of line.",
                                textfile_reader_filename(reader),
                                textfile_reader_linenum(reader));
                    lib_free(arg);
                    return false;
                }
                *a++ = *s++;
            } else if (*s == VHK_QUOTE) {
                /* found closing quote */
                break;
            } else {
                *a++ = *s++;
            }
        }
        if (*s != VHK_QUOTE) {
            /* error, no closing quote */
            log_message(vhk_log,
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
static bool vhk_parser_do_undef(const char *line, textfile_reader_t *reader)
{
    const char       *curpos;
    const char       *oldpos;
    uint32_t          vice_mask;
    uint32_t          vice_key;
    uint32_t          arch_mask;
    uint32_t          arch_key;
    ui_action_map_t  *map;

    curpos = util_skip_whitespace(line);
    if (*curpos == '\0') {
        /* error: missing argument */
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: error: missing argument for !UNDEF",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return true;
    }

    /* get combined modifier masks and keyval */
    oldpos = curpos;
    if (!vhk_parser_get_keysym_and_modmask(curpos, &curpos, reader,
                                           &vice_mask, &vice_key,
                                           &arch_mask, &arch_key)) {
        /* error already logged */
        return false;
    }
    if (vhk_debug) {
        log_message(vhk_log,
                    "Hotkeys %s:%ld: VICE key: %04x, VICE mask: %08x, keyname: %s.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader),
                    vice_key,
                    vice_mask,
                    vhk_keysym_name(vice_key));
    }

    /* lookup map for hotkey */
    map = ui_action_map_get_by_hotkey(vice_key, vice_mask);
    if (map != NULL) {
        if (vhk_debug) {
            log_message(vhk_log,
                        "Hotkeys: %s:%ld: found hotkey defined for action %d (%s),"
                        " clearing.",
                        textfile_reader_filename(reader),
                        textfile_reader_linenum(reader),
                        map->action, ui_action_get_name(map->action));
        }
    ui_action_map_clear_hotkey(map);
    } else {
        /* cannot use gtk_accelerator_name(): Gtk throws a fit about not having
         * a display and thus no GdkKeymap. :( */
#if 0

        char *accel_name = gtk_accelerator_name(keyval, mask);
#endif
        /* do it the hard way: copy argument without trailing crap */
        char   accel_name[256];
        size_t accel_len;

        accel_len = (size_t)(curpos - oldpos);
        if (accel_len >= sizeof(accel_name)) {
            accel_len = sizeof(accel_name) - 1;
        }
        memcpy(accel_name, oldpos, accel_len);
        accel_name[accel_len] = '\0';

        log_message(vhk_log,
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
static bool vhk_parser_handle_keyword(const char *line, textfile_reader_t *reader)
{
    bool result = true;

    if (*line == '\0') {
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: syntax error, missing keyword after '!'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        result =  false;
    } else if (!isalpha((unsigned char)*line)) {
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: syntax error, illegal character after '!'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        result = false;
    } else {
        /* get keyword ID */
        vhk_keyword_id_t id = vhk_parser_get_keyword_id(line);

        switch (id) {
            case VHK_KW_ID_CLEAR:
                /* handle CLEAR */
                result = vhk_parser_do_clear(line, reader);
                break;

            case VHK_KW_ID_DEBUG:
                /* handle DEBUG */
                result = vhk_parser_do_debug(line + 6, reader);
                break;

            case VHK_KW_ID_INCLUDE:
                /* handle INCLUDE */
                result = vhk_parser_do_include(line + 8, reader);
                break;

            case VHK_KW_ID_UNDEF:
                /* handle UNDEF */
                result = vhk_parser_do_undef(line + 6, reader);
                break;

            case VHK_KW_ID_IFDEF:
                /* handle IFDEF */
                log_message(vhk_log, "Hotkeys: TODO: handle !IFDEF.");
                break;

            case VHK_KW_ID_IFNDEF:
                /* handle IFNDEF */
                log_message(vhk_log, "Hotkeys: TODO: handle !IFNDEF.");
                break;

            case VHK_KW_ID_ENDIF:
                /* handle ENDIF */
                log_message(vhk_log, "Hotkeys: TODO: handle !ENDIF.");
                break;

            default:
                /* unknown keyword */
                log_message(vhk_log,
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
static bool vhk_parser_handle_mapping(const char *line, textfile_reader_t* reader)
{
    const char      *curpos;
    const char      *oldpos;
    char             action_name[256];
    ptrdiff_t        namelen;
    ui_action_map_t *map;
    int              action_id    = ACTION_INVALID;
    uint32_t         vice_keysym  = 0;
    uint32_t         vice_modmask = 0;
    uint32_t         arch_keysym  = 0;
    uint32_t         arch_modmask = 0;

    curpos = oldpos = line;

    /* get action name */
    /* TODO: support quotes? */
    while (*curpos != '\0' && IS_ACTION_NAME_CHAR((int)*curpos)) {
        curpos++;
    }
    /* check for errors */
    if (curpos == oldpos) {
        /* no valid action name tokens found */
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: error: missing action name.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }
    if (curpos - oldpos >= sizeof(action_name)) {
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: error: action name is too long.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader));
        return false;
    }

    /* make properly nul-terminated string of action name for lookups*/
    namelen = curpos - oldpos;
    memcpy(action_name, oldpos, (size_t)namelen);
    action_name[namelen] = '\0';

    curpos = util_skip_whitespace(curpos);

    /* get combined modifier masks and keyval */
    if (!vhk_parser_get_keysym_and_modmask(curpos, &curpos, reader,
                                           &vice_keysym, &vice_modmask,
                                           &arch_keysym, &arch_modmask)) {
        /* error already logged */
        return false;
    }

    if (vhk_debug) {
        log_message(vhk_log,
                    "Hotkeys: VICE keysym: %04x, VICE modmask: %08x, keyname: %s, action: %s",
                    vice_keysym, vice_modmask, vhk_keysym_name(vice_keysym), action_name);
    }

    /* finally try to register the hotkey */
    action_id = ui_action_get_id(action_name);
    if (action_id <= ACTION_NONE) {
        log_message(vhk_log,
                    "Hotkeys: %s:%ld: error: unknown action '%s'.",
                    textfile_reader_filename(reader),
                    textfile_reader_linenum(reader),
                    action_name);
        /* allow parsing to continue */
        return true;
    }

    /* register mapping, first try looking up the item */
    map = ui_action_map_set_hotkey(action_id,
                                   vice_keysym, vice_modmask,
                                   arch_keysym, arch_modmask);
    if (map != NULL) {
        /* set hotkey for menu item, if it exists */
        /* call arch-specific "virtual method" */
        debug_vhk("calling ui_hotkeys_install_by_map()");
        ui_hotkeys_install_by_map(map);
    }
    return true;
}


/** \brief  Parse hotkeys file
 *
 * \param[in]   path    path to hotkeys file
 *
 * \return  bool
 */
bool vhk_parser_parse(const char *path)
{
    textfile_reader_t reader;
    bool              status = true;

    /* disable debugging */
    vhk_debug = false;
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
                bool  parse_ok = true;
                char *trimmed  = vhk_parser_strtrim(line);

                if (vhk_debug) {
                    log_debug("LINE %3ld: '%s'",
                              textfile_reader_linenum(&reader),
                              line);
                }

                if (*trimmed == '\0'
                        || *trimmed == VHK_COMMENT
                        || *trimmed == VHK_COMMENT_ALT) {
                    /* empty line or comment, skip */
                    parse_ok = true;
                } else if (*trimmed == VHK_KEYWORD) {
                    /* found keyword */
                    parse_ok = vhk_parser_handle_keyword(trimmed + 1, &reader);
                } else {
                    /* assume hotkey definition */
                    parse_ok = vhk_parser_handle_mapping(trimmed, &reader);
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
    return status;
}
