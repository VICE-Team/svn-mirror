/*
 * mon_command.c - The VICE built-in monitor command table.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <viceteam@t-online.de>
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
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#include "mon_command.h"
#include "montypes.h"
#include "mon_parse.h" /* FIXME ! */
#include "mon_util.h"
#include "translate.h"
#include "uimon.h"
#include "util.h"

#define GET_PARAM(c) ((c->use_param_names_id == USE_PARAM_ID) ? translate_text(c->param_names_id) : _(c->param_names))
#define GET_DESCRIPTION(c) ((c->use_description_id == USE_DESCRIPTION_ID) ? translate_text(c->description_id) : _(c->description))

typedef struct mon_cmds_s {
   const char *str;
   const char *abbrev;
   int use_param_names_id;
   int use_description_id;
   int param_names_id;
   int description_id;
   const char *param_names;
   const char *description;
} mon_cmds_t;

static const mon_cmds_t mon_cmd_array[] = {
   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_AVAILABLE_COMMANDS_ARE,
     "", NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MACHINE_STATE_COMMANDS,
     "", NULL },

   { "bank", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE_R_BANKNAME, IDGS_MON_BANK_DESCRIPTION,
     NULL, NULL },

   { "backtrace", "bt",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_BACKTRACE_DESCRIPTION,
     NULL, NULL },

   { "cpu", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDCLS_P_TYPE, IDGS_MON_CPU_DESCRIPTION,
     NULL, NULL },

   { "cpuhistory", "chis",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_COUNT, IDGS_MON_CPUHISTORY_DESCRIPTION,
     NULL, NULL },

   { "dump",            "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME, IDGS_MON_DUMP_DESCRIPTION,
     NULL, NULL },

   { "goto", "g",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS, IDGS_MON_GOTO_DESCRIPTION,
     NULL, NULL },

   { "io", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_IO_DESCRIPTION,
     NULL, NULL },

   { "next", "n",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_NEXT_DESCRIPTION,
     NULL, NULL },

   { "registers", "r",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_REG_NAME_P_NUMBER, IDGS_MON_REGISTERS_DESCRIPTION,
     NULL, NULL },

   { "reset", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_TYPE, IDGS_MON_RESET_DESCRIPTION,
     NULL, NULL },

   { "return", "ret",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_RETURN_DESCRIPTION,
     NULL, NULL },

   { "screen", "sc",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_SCREEN_DESCRIPTION,
     NULL, NULL },

   { "step", "z",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_COUNT, IDGS_MON_STEP_DESCRIPTION,
     NULL, NULL },

   { "undump", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME, IDGS_MON_UNDUMP_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_SYMBOL_TABLE_COMMANDS,
     "", NULL },

   { "add_label", "al",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE_P_ADDRESS_P_LABEL, IDGS_MON_ADD_LABEL_DESCRIPTION,
     NULL, NULL },

   { "delete_label", "dl",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE_P_LABEL, IDGS_MON_DELETE_LABEL_DESCRIPTION,
     NULL, NULL },

   { "load_labels", "ll",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE_QP_FILENAME, IDGS_MON_LOAD_LABELS_DESCRIPTION,
     NULL, NULL },

   { "save_labels", "sl",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE_QP_FILENAME, IDGS_MON_SAVE_LABELS_DESCRIPTION,
     NULL, NULL },

   { "show_labels", "shl",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MEMSPACE, IDGS_MON_SHOW_LABELS_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_ASSEMBLER_AND_MEMORY_COMMANDS,
     "", NULL },

   { ">", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_ADDRESS_P_DATA_LIST, IDGS_MON_WRITE_DESCRIPTION,
     NULL, NULL },

   { "a", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_RP_INSTRUCTION, IDGS_MON_ASSEMBLE_DESCRIPTION,
     NULL, NULL },

   { "compare", "c",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_RANGE_P_ADDRESS, IDGS_MON_COMPARE_DESCRIPTION,
     NULL, NULL },

   { "delete", "del",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM, IDGS_MON_DELETE_DESCRIPTION,
     NULL, NULL },

   { "disass", "d",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_ADDRESS_RP_ADDRESS, IDGS_MON_DISASS_DESCRIPTION,
     NULL, NULL },

   { "fill", "f",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_RANGE_P_DATA_LIST, IDGS_MON_FILL_DESCRIPTION,
     NULL, NULL },

   { "hunt", "h",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_RANGE_P_DATA_LIST, IDGS_MON_HUNT_DESCRIPTION,
     NULL, NULL },

   { "i", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_OPT_RANGE, IDGS_MON_I_DESCRIPTION,
     NULL, NULL },

   { "ii", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_OPT_RANGE, IDGS_MON_II_DESCRIPTION,
     NULL, NULL },

   { "mem", "m",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE, IDGS_MON_MEM_DESCRIPTION,
     NULL, NULL },

   { "memchar", "mc",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE, IDGS_MON_MEMCHAR_DESCRIPTION,
     NULL, NULL },

   { "memmapsave", "mmsave",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_P_FORMAT, IDGS_MON_MEMMAPSAVE_DESCRIPTION,
     NULL, NULL },

   { "memmapshow", "mmsh",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_MASK_RP_ADDRESS_OPT_RANGE, IDGS_MON_MEMMAPSHOW_DESCRIPTION,
     NULL, NULL },

   { "memmapzap", "mmzap",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_MEMMAPZAP_DESCRIPTION,
     NULL, NULL },

   { "memsprite",       "ms",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_DATA_TYPE_RP_ADDRESS_OPT_RANGE, IDGS_MON_MEMSPRITE_DESCRIPTION,
     NULL, NULL },

   { "move", "t",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_ADDRESS_RANGE_P_ADDRESS, IDGS_MON_MOVE_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_CHECKPOINT_COMMANDS,
     "", NULL },

   { "break", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_ADDRESS_RP_COND_EXPR, IDGS_MON_BREAK_DESCRIPTION,
     NULL, NULL },

   { "command", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM_QP_COMMAND, IDGS_MON_COMMAND_DESCRIPTION,
     NULL, NULL },

   { "condition", "cond",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM_IF_P_COND_EXPR, IDGS_MON_CONDITION_DESCRIPTION,
     NULL, NULL },

   { "disable", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM, IDGS_MON_DISABLE_DESCRIPTION,
     NULL, NULL },

   { "enable", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM, IDGS_MON_ENABLE_DESCRIPTION,
     NULL, NULL },

   { "ignore", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_CHECKNUM_RP_COUNT, IDGS_MON_IGNORE_DESCRIPTION,
     NULL, NULL },

   { "until", "un",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_ADDRESS, IDGS_MON_UNTIL_DESCRIPTION,
     NULL, NULL },

   { "watch", "w",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_R_LOADSTORE_R_ADDRESS_R_ADDRESS, IDGS_MON_WATCH_DESCRIPTION,
     NULL, NULL },

   { "trace", "tr",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_R_ADDRESS_R_ADDRESS, IDGS_MON_TRACE_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MONITOR_STATE_COMMANDS,
     "", NULL },

   { "device", "dev",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_DEVICE_DESCRIPTION,
     "[c:|8:|9:|10:|11:]", NULL },

   { "exit", "x",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_EXIT_DESCRIPTION,
     NULL, NULL },

   { "quit", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
#ifdef OS2
     IDGS_UNUSED, IDGS_MON_EXIT_DESCRIPTION,
#else
     IDGS_UNUSED, IDGS_MON_QUIT_DESCRIPTION,
#endif
     NULL, NULL },

   { "radix", "rad",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_RADIX_DESCRIPTION,
     "[H|D|O|B]", NULL },

   { "sidefx", "sfx",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_R_ON_R_OFF_R_TOGGLE, IDGS_MON_SIDEFX_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_DISK_COMMANDS,
     "", NULL },

   { "@", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_DISK_COMMAND, IDGS_MON_AT_DESCRIPTION,
     NULL, NULL },

   { "attach", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_FILENAME_P_DEVICE, IDGS_MON_ATTACH_DESCRIPTION,
     NULL, NULL },

   { "bload", "bl",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS, IDGS_MON_BLOAD_DESCRIPTION,
     NULL, NULL },

   { "block_read", "br",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_TRACK_P_SECTOR_RP_ADDRESS, IDGS_MON_BLOCK_READ_DESCRIPTION,
     NULL, NULL },

   { "bsave", "bs",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2, IDGS_MON_BSAVE_DESCRIPTION,
     NULL, NULL },

   { "block_write", "bw",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_TRACK_P_SECTOR_P_ADDRESS, IDGS_MON_BLOCK_WRITE_DESCRIPTION,
     NULL, NULL },

   { "cd", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_DIRECTORY, IDGS_MON_CD_DESCRIPTION,
     NULL, NULL },

   { "detach", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDCLS_P_DEVICE, IDGS_MON_DETACH_DESCRIPTION,
     NULL, NULL },

   { "dir", "ls",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_DIRECTORY, IDGS_MON_DIR_DESCRIPTION,
     NULL, NULL },

   { "load", "l",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_P_DEVICE_RP_ADDRESS, IDGS_MON_LOAD_DESCRIPTION,
     NULL, NULL },

   { "pwd", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_PWD_DESCRIPTION,
     NULL, NULL },

   { "save", "s",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_P_DEVICE_P_ADDRESS1_P_ADDRESS2, IDGS_MON_SAVE_DESCRIPTION,
     NULL, NULL },

   { "", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_OTHER_COMMANDS,
     "", NULL },

   { "~", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDCLS_P_NUMBER, IDGS_MON_DISPLAY_NUMBER_DESCRIPTION,
     NULL, NULL },

   { "cartfreeze", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_CARTFREEZE_DESCRIPTION,
     NULL, NULL },

   { "help", "?",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_RP_COMMAND, IDGS_MON_HELP_DESCRIPTION,
     NULL, NULL },

   { "keybuf", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_STRING, IDGS_MON_KEYBUF_DESCRIPTION,
     NULL, NULL },

   { "playback", "pb",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME, IDGS_MON_PLAYBACK_DESCRIPTION,
     NULL, NULL },

   { "print", "p",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_EXPRESSION, IDGS_MON_PRINT_DESCRIPTION,
     NULL, NULL },

   { "record", "rec",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME, IDGS_MON_RECORD_DESCRIPTION,
     NULL, NULL },

   { "resourceget", "resget",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_RESOURCE, IDGS_MON_RESOURCEGET_DESCRIPTION,
     NULL, NULL },

   { "resourceset", "resset",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_RESOURCE_QP_VALUE, IDGS_MON_RESOURCESET_DESCRIPTION,
     NULL, NULL },

   { "stop", "",
     USE_PARAM_STRING, USE_DESCRIPTION_ID,
     IDGS_UNUSED, IDGS_MON_STOP_DESCRIPTION,
     NULL, NULL },

   { "screenshot", "scrsh",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_QP_FILENAME_RP_FORMAT, IDGS_MON_SCREENSHOT_DESCRIPTION,
     NULL, NULL },

   { "tapectrl", "",
     USE_PARAM_ID, USE_DESCRIPTION_ID,
     IDGS_P_COMMAND, IDGS_MON_TAPECTRL_DESCRIPTION,
     NULL, NULL },

   { NULL }

};

static int mon_command_lookup_index(const char *str)
{
    int num = 0;

    if (str == NULL)
        return -1;

    do {
        if ((strcasecmp(str, mon_cmd_array[num].str) == 0) ||
            (strcasecmp(str, mon_cmd_array[num].abbrev) == 0)) {
           return num;
        }
        num++;
    } while (mon_cmd_array[num].str != NULL);

    return -1;
}

void mon_command_print_help(const char *cmd)
{
    const mon_cmds_t *c;
    int column;
    int len;
    int longest;
    int max_col;

    if (cmd == NULL) {
        longest = 0;
        for (c = mon_cmd_array; c->str != NULL; c++) {
            len = strlen(c->str);
            if (!util_check_null_string(c->abbrev))
                len += 3 + strlen(c->abbrev); /* 3 => " ()" */

            if (len > longest)
                longest = len;
        }
        longest += 2; /* some space */
        max_col = 80 / longest - 1;

        column = 0;
        for (c = mon_cmd_array; c->str != NULL; c++) {
            int tot = strlen(c->str);

            /* "Empty" command, that's a head line  */
            if (tot == 0) {
                if (column != 0) {
                    mon_out("\n");
                    column = 0;
                }
                mon_out("\n%s\n", GET_DESCRIPTION(c));
                continue;
            }

            mon_out("%s", c->str);

            if (!util_check_null_string(c->abbrev)) {
                mon_out(" (%s)", c->abbrev);
                tot += 3 + strlen(c->abbrev);
            }

            if (column >= max_col) {
                mon_out("\n");
                column = 0;
            } else {
                for (; tot < longest; tot++)
                    mon_out(" ");
                column++;
            }
            if (mon_stop_output != 0) break;
        }
        mon_out("\n\n");
    } else {
        int cmd_num;

        cmd_num = mon_command_lookup_index(cmd);

        if (cmd_num == -1)
            mon_out(translate_text(IDGS_COMMAND_S_UNKNOWN), cmd);
        else if (mon_cmd_array[cmd_num].description == NULL && mon_cmd_array[cmd_num].description_id == IDGS_UNUSED)
            mon_out(translate_text(IDGS_NO_HELP_AVAILABLE_FOR_S), cmd);
        else {
            const mon_cmds_t *c;

            c = &mon_cmd_array[cmd_num];

            mon_out(translate_text(IDGS_SYNTAX_S_S),
                      c->str,
                      GET_PARAM(c) != NULL ? GET_PARAM(c) : "");
            if (!util_check_null_string(c->abbrev))
                mon_out(translate_text(IDGS_ABBREVIATION_S), c->abbrev);
            mon_out("\n%s\n\n", GET_DESCRIPTION(c));
        }
    }
}
