/*
 * mon_command.c - The VICE built-in monitor command table.
 *
 * Written by
 *  Daniel Sladic <sladic@eecg.toronto.edu>
 *  Ettore Perazzoli <ettore@comm2000.it>
 *  Andreas Boose <boose@linux.rz.fh-hannover.de>
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

#include "mon.h"
#include "mon_command.h"
#include "mon_parse.h"
#include "uimon.h"
#include "utils.h"

typedef struct mon_cmds_s {
   const char *str;
   const char *abbrev;
   int token;
   int next_state;
   const char *param_names;
   const char *description;
} mon_cmds_t;

static mon_cmds_t mon_cmd_array[] = {
   { "",                "",     BAD_CMD,                STATE_INITIAL },

   { "~",               "~",    CONVERT_OP,             STATE_INITIAL,
     "<number>",
     "Display the specified number in decimal, hex, octal and binary."},

   { ">",               ">",    CMD_ENTER_DATA,         STATE_INITIAL,
     "[<address>] <data_list>",
     "Write the specified data at `address'."},

   { "@",               "@",    CMD_DISK,               STATE_ROL,
   "<disk command>",
     "Perform a disk command on the currently attached disk image on drive 8.  The\n"
     "specified disk command is sent to the drive's channel #15." },

   { "]",               "]",    CMD_ENTER_BIN_DATA,     STATE_INITIAL },

   { "a",               "a",    CMD_ASSEMBLE,           STATE_INITIAL,
     "<address> [ <instruction> [: <instruction>]* ]",
     "Assemble instructions to the specified address.  If only one instruction\n"
     "is specified, enter assembly mode (enter an empty line to exit assembly\n"
     "mode)." },

   { "add_label",       "al",   CMD_ADD_LABEL,          STATE_INITIAL,
    "<address> <label>",
     "Map a given address to a label.  This label can be used when entering\n"
     "assembly code and is shown during disassembly." },

   { "bank",            "",     CMD_BANK,               STATE_BNAME,
     "[<memspace>] [bankname]",
     "If bankname is not given, print the possible banks for the memspace.\n"
     "If bankname is given set the current bank in the memspace to the given\n"
     "bank." },

   { "bload",           "bl",   CMD_BLOAD,              STATE_FNAME,
     "\"<filename>\" <address>",
     "Load the specified file into memory at the specified address."},

   { "br",              "",     CMD_BLOCK_READ,         STATE_INITIAL,
     "<track> <sector> [<address>]",
     "Read the block at the specified track and sector.  If an address is\n"
     "specified, the data is loaded into memory.  If no address is given, the\n"
     "data is displayed using the default datatype." },

   { "break",           "",     CMD_BREAK,              STATE_INITIAL,
     "[<address> [if <cond_expr>] ]",
     "If no address is given, the currently valid watchpoints are printed.\n"
     "If an address is given, a breakpoint is set for that address and the\n"
     "breakpoint number is printed.  A conditional expression can also be\n"
     "specified for the breakpoint.  For more information on conditions, see\n"
     "the CONDITION command."   },

   { "brmon",           "",     CMD_BRMON,              STATE_INITIAL },

   { "bsave",           "bs",   CMD_BSAVE,              STATE_FNAME,
     "\"<filename>\" <address1> <address2>",
     "Save the memory from address1 to address2 to the specified file." },

   { "bw",              "",     CMD_BLOCK_WRITE,        STATE_INITIAL,
     "<track> <sector> <address>",
     "Write a block of data at `address' on the specified track and sector\n"
     "of disk in drive 8." },

   { "cd",              "",     CMD_CHDIR,              STATE_ROL,
     "<directory>",
     "Change the working directory."},

   { "command",         "",     CMD_COMMAND,            STATE_INITIAL,
     "<checknum> \"<command>\"",
     "Specify `command' as the command to execute when checkpoint `checknum'\n"
     "is hit.  Note that the `x' command is not yet supported as a\n"
     "command argument." },

   { "compare",         "c",    CMD_COMPARE,            STATE_INITIAL,
     "<address_range> <address>",
     "Compare memory from the source specified by the address range to the\n"
     "destination specified by the address.  The regions may overlap.  Any\n"
     "values that miscompare are displayed using the default displaytype.\n" },

   { "condition",       "cond", CMD_CONDITION,          STATE_INITIAL,
     "<checknum> if <cond_expr>",
     "Each time the specified checkpoint is examined, the condition is\n"
     "evaluated.  If it evalutes to true, the checkpoint is activated.\n"
     "Otherwise, it is ignores.  If registers are specified in the expression,\n"
     "the values used are those at the time the checkpoint is examined, not\n"
     "when the condition is set.\n" },

   { "cpu",             "",     CMD_CPU,                STATE_CTYPE,
     "<type>",
     "Specify the type of CPU currently used (6502/z80)." },

   { "d",               "d",    CMD_DISASSEMBLE,        STATE_INITIAL,
     "[<address> [<address>]]",
     "Disassemble instructions.  If two addresses are specified, they are used\n"
     "as a start and end address.  If only one is specified, it is treated as\n"     "the start address and a default number of instructions are\n"
     "disassembled.  If no addresses are specified, a default number of\n"
     "instructions are disassembled from the dot address." },

   { "delete",          "del",  CMD_DELETE,             STATE_INITIAL,
     "<checknum>",
     "Delete checkpoint `checknum'." },

   { "delete_label",    "dl",   CMD_DEL_LABEL,          STATE_INITIAL },

   { "device",          "dev",  CMD_DEVICE,             STATE_INITIAL,
     "[c:|8:|9:]",
     "Set the default memory device to either the computer `c:' or the\n"
     "specified disk drive (`8:', `9:')." },

   { "disable",         "",     CMD_CHECKPT_OFF,        STATE_INITIAL,
     "<checknum>",
     "Disable checkpoint `checknum'." },

   { "down",            "",     CMD_DOWN,               STATE_INITIAL },

   { "dump",            "",     CMD_DUMP,               STATE_FNAME },

   { "enable",          "",     CMD_CHECKPT_ON,         STATE_INITIAL,
     "<checknum>",
     "Enable checkpoint `checknum'." },

   { "exit",            "x",    CMD_EXIT,               STATE_INITIAL,
     NULL,
     "Leave the monitor and return to execution." },

   { "fill",            "f",    CMD_FILL,               STATE_INITIAL,
     "<address_range> <data_list>",
     "Fill memory in the specified address range with the data in\n"
     "<data_list>.  If the size of the address range is greater than the size\n"
     "of the data_list, the data_list is repeated." },

   { "goto",            "g",    CMD_GOTO,               STATE_INITIAL,
     "<address>",
     "Change the PC to ADDRESS and continue execution" },

   { "help",            "?",    CMD_HELP,               STATE_ROL },

   { "hunt",            "h",    CMD_HUNT,               STATE_INITIAL,
     "<address_range> <data_list>",
     "Hunt memory in the specified address range for the data in\n"
     "<data_list>.  If the data is found, the starting address of the match is\n"
     "displayed.  The entire range is searched for all possible matches." },

   { "i",               "i",    CMD_TEXT_DISPLAY,       STATE_INITIAL,
     "<address_opt_range>",
     "Display memory contents as PETSCII text." },

   { "ignore",          "",     CMD_IGNORE,             STATE_INITIAL,
     "<checknum> [<count>]",
     "Ignore a checkpoint a given number of crossings.  If no count is given,\n"
     "the default value is 1.\n" },

   { "io",              "",     CMD_IO,                 STATE_INITIAL },

   { "load",            "l",    CMD_LOAD,               STATE_FNAME,
     "\"<filename>\" <address>",
     "Load the specified file into memory at the specified address. Set BASIC\n"
     "pointers appropriately (not all emulators). Use (otherwise ignored)\n"
     "two-byte load address from file if no address specified."},

   { "load_labels",     "ll",   CMD_LOAD_LABELS,        STATE_FNAME,
     "[<memspace>] \"<filename>\"",
     "Load a file containing a mapping of labels to addresses.  If no memory\n"
     "space is specified, the default readspace is used." },

   { "m",               "m",    CMD_MEM_DISPLAY,        STATE_INITIAL,
     "[<data_type>] [<address_opt_range>]"
     "Display the contents of memory.  If no datatype is given, the default is\n"
     "used.  If only one address is specified, the length of data displayed is\n"
     "based on the datatype.  If no addresses are given, the 'dot' address is\n"
     "used." },

   { "mc",              "",     CMD_CHAR_DISPLAY,       STATE_INITIAL,
     "[<data_type>] [<address_opt_range>]",
     "Display the contents of memory as character data.  If only one address\n"
     "is specified, only one character is displayed.  If no addresses are\n"
     "given, the ``dot'' address is used." },

   { "move",            "t",    CMD_MOVE,               STATE_INITIAL,
     "<address_range> <address>",
     "Move memory from the source specified by the address range to\n"
     "the destination specified by the address.  The regions may overlap." },

   { "ms",              "",     CMD_SPRITE_DISPLAY,     STATE_INITIAL,
     "[<data_type>] [<address_opt_range>]",
     "Display the contents of memory as sprite data.  If only one address is\n"
     "specified, only one sprite is displayed.  If no addresses are given, the\n"
     "``dot'' address is used." },

   { "next",            "n",    CMD_NEXT,               STATE_INITIAL,
     NULL,
     "Advance to the next instruction.  Subroutines are treated as\n"
     "a single instruction." },

   { "playback",        "pb",   CMD_PLAYBACK,           STATE_FNAME,
     "\"<filename>\"",
     "Monitor commands from the specified file are read and executed.  This\n"
     "command stops at the end of file or when a STOP command is read." },

   { "print",           "p",    CMD_PRINT,              STATE_INITIAL,
     "<expression>",
     "Evaluate the specified expression and output the result." },

/* If we want 'quit' for OS/2 I couldn't leave the emulator by calling exit(0)
   So I decided to skip this (I think it's unnecessary for OS/2 */
#ifdef OS2
   { "quit",            "",     CMD_EXIT,               STATE_INITIAL,
     NULL,
     "Leave the monitor and return to execution." },
#else
   { "quit",            "",     CMD_QUIT,               STATE_INITIAL,
     NULL,
     "Exit the emulator immediately."},
#endif

   { "radix",           "rad",  CMD_RADIX,              STATE_INITIAL,
     "[H|D|O|B]",
     "Set the default radix to hex, decimal, octal, or binary.  With no\n"
     "argument, the current radix is printed." },

   { "record",          "rec",  CMD_RECORD,             STATE_FNAME,
     "\"<filename>\"",
     "After this command, all commands entered are written to the specified\n"
     "file until the STOP command is entered." },

   { "registers",       "r",    CMD_REGISTERS,          STATE_REG_ASGN,
     "[<reg_name> = <number> [, <reg_name> = <number>]*]",
     "Assign respective registers.  With no parameters, display register\n"
     "values." },

   { "return",          "ret",  CMD_RETURN,             STATE_INITIAL,
     NULL,
     "Continues execution and returns to the monitor just before the next\n"
     "RTS or RTI is executed." },

   { "save",            "s",    CMD_SAVE,               STATE_FNAME,
     "\"<filename>\" <address1> <address2>",
     "Save the memory from address1 to address2 to the specified file. Write\n"
     "two-byte load address." },

   { "save_labels",     "sl",   CMD_SAVE_LABELS,        STATE_FNAME,
     "[<memspace>] \"<filename>\"",
     "Save labels to a file.  If no memory space is specified, all of the\n"
     "labels are saved." },

   { "screen",         "sc",    CMD_SCREEN,             STATE_INITIAL,
     NULL,
     "Displays the contents of the screen." },

   { "show_labels",     "shl",  CMD_SHOW_LABELS,        STATE_INITIAL,
     "[<memspace>]",
     "Display current label mappings.  If no memory space is specified, show\n"
     "all labels." },

   { "sidefx",          "sfx",  CMD_SIDEFX,             STATE_INITIAL,
     "[on|off|toggle]",
     "Control how monitor generated reads affect memory locations that have\n"
     "read side-effects.  If the argument is 'on' then reads may cause\n"
     "side-effects.  If the argument is 'off' then reads don't cause\n"
     "side-effects.  If the argument is 'toggle' then the current mode is\n"
     "switched.  No argument displays the current state." },

   { "step",            "z",    CMD_STEP,               STATE_INITIAL,
     "[<count>]",
     "Single-step through instructions.  COUNT allows stepping\n"
     "more than a single instruction at a time." },

   { "stop",            "",     CMD_STOP,               STATE_INITIAL,
     NULL,
     "Stop recording commands.  See `record'." },

   { "system",          "sys",  CMD_SYSTEM,             STATE_ROL },

   { "trace",           "tr",   CMD_TRACE,              STATE_INITIAL,
     "[address [address]]",
     "Set a tracepoint.  If a single address is specified, set a tracepoint\n"
     "for that address.  If two addresses are specified, set a tracepoint\n"
     "for the memory locations between the two addresses." },

   { "until",           "un",   CMD_UNTIL,              STATE_INITIAL,
     "[<address>]",
     "If no address is given, the currently valid breakpoints are printed.\n"
     "If an address is given, a temporary breakpoint is set for that address\n"
     "and the breakpoint number is printed.  Control is returned to the\n"
    "emulator by this command.  The breakpoint is deleted once it is hit.\n" },

   { "undump",          "",     CMD_UNDUMP,             STATE_FNAME },

   { "up",              "",     CMD_UP,                 STATE_INITIAL },

   { "verify",          "v",    CMD_VERIFY,             STATE_FNAME },

   { "watch",           "w",    CMD_WATCH,              STATE_INITIAL,
     "[loadstore] [address [address]]",
     "Set a watchpoint.  If a single address is specified, set a watchpoint\n"
     "for that address.  If two addresses are specified, set a watchpoint\n"
     "for the memory locations between the two addresses.\n"
     "`loadstore' is either `load' or `store' to specify on which operation\n"
     "the monitor breaks. If not specified, the monitor breaks on both\n"
     "operations." },

   { "",                "",     -1,                     STATE_INITIAL }

};

int mon_command_lookup_index(const char *str, int *push_back)
{
    int num = 0, partial = -1;

    if (str == NULL)
        return -1;

    do {
        if ((strcasecmp(str, mon_cmd_array[num].str) == 0) ||
            (strcasecmp(str, mon_cmd_array[num].abbrev) == 0)) {
           *push_back = 0;
           return num;
        }
        else if (strlen(mon_cmd_array[num].abbrev) &&
                strncasecmp(str, mon_cmd_array[num].abbrev,
                strlen(mon_cmd_array[num].abbrev)) == 0) {
           *push_back = strlen(mon_cmd_array[num].abbrev);
           partial = num;
        }
        num++;
    } while (mon_cmd_array[num].token > 0);

    return partial;
}

int mon_command_get_token(int mon_index)
{
    return mon_cmd_array[mon_index].token;
}

int mon_command_get_next_state(int mon_index)
{
    return mon_cmd_array[mon_index].next_state;
}

void mon_command_print_help(const char *cmd)
{
    if (cmd == NULL) {
        mon_cmds_t *c;
        int column = 0;

        /* Print on two columns.  This could be a lot nicer, but I am lazy.  */
        uimon_out("\nAvailable commands are:\n\n");
        for (c = mon_cmd_array; c->token != -1; c++) {
            int tot = 0;

            tot += strlen(c->str);
            if (tot == 0)        /* "Empty" command?  */
                continue;
            uimon_out("%s", c->str);

            if (!util_check_null_string(c->abbrev)) {
                uimon_out(" (%s)", c->abbrev);
                tot += 3 + strlen(c->abbrev);
            }

            if (tot > 40 || column == 1) {
                uimon_out("\n");
                column = 0;
            } else {
                for (; tot < 40; tot++)
                    uimon_out(" ");
                column = 1;
            }
            if (mon_stop_output != 0) break;
        }
        uimon_out("\n\n");
    } else {
        int push_back, cmd_num;

        cmd_num = mon_command_lookup_index(cmd, &push_back);

        if (cmd_num == -1 || push_back)
            uimon_out("Command `%s' unknown.\n", cmd);
        else if (mon_cmd_array[cmd_num].description == NULL)
            uimon_out("No help available for `%s'\n", cmd);
        else {
            mon_cmds_t *c;

            c = &mon_cmd_array[cmd_num];

            uimon_out("\nSyntax: %s %s\n",
                      c->str,
                      c->param_names != NULL ? c->param_names : "");
            if (!util_check_null_string(c->abbrev))
                uimon_out("Abbreviation: %s\n", c->abbrev);
            uimon_out("\n%s\n\n", c->description);
        }
    }
}

