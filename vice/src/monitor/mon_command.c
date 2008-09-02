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
#include "uimon.h"
#include "util.h"


typedef struct mon_cmds_s {
   const char *str;
   const char *abbrev;
   const char *param_names;
   const char *description;
} mon_cmds_t;

static const mon_cmds_t mon_cmd_array[] = {
   { "", "", "", N_("Available commands are:") },

   { "", "", "", N_("Machine state commands:") },

   { "bank",            "",
     N_("[<memspace>] [bankname]"),
     N_("If bankname is not given, print the possible banks for the memspace.\n"
     "If bankname is given set the current bank in the memspace to the given\n"
     "bank.") },

   { "backtrace",       "bt",
     NULL,
     N_("Print JSR call chain (most recent call first). Stack offset\n"
     "relative to SP+1 is printed in parentheses. This is a best guess\n"
     "only.") },

   { "cpu",             "",
     N_("<type>"),
     N_("Specify the type of CPU currently used (6502/z80).") },

   { "cpuhistory",       "chis",
     N_("[<count>]"),
     N_("Show <count> last executed commands.") },

   { "dump",            "",
     N_("\"<filename>\""),
     N_("Write a snapshot of the machine into the file specified.\n"
     "This snapshot is compatible with a snapshot written out by the UI.\n"
     "Note: No ROM images are included into the dump.") },

/*
   { "down",            "",
     NULL,
     "*** unimplemented ***" },
*/

   { "goto",            "g",
     N_("<address>"),
     N_("Change the PC to ADDRESS and continue execution") },

   { "io",              "",
     NULL,
     N_("Print out the I/O area of the emulated machine.") },

   { "next",            "n",
     NULL,
     N_("Advance to the next instruction.  Subroutines are treated as\n"
     "a single instruction.") },

   { "registers",       "r",
     N_("[<reg_name> = <number> [, <reg_name> = <number>]*]"),
     N_("Assign respective registers.  With no parameters, display register\n"
     "values.") },

   { "reset",           "",
     N_("[<type>]"),
     N_("Reset the machine or drive. Type: 0 = soft, 1 = hard, 8-11 = drive.") },

   { "return",          "ret",
     NULL,
     N_("Continues execution and returns to the monitor just before the next\n"
     "RTS or RTI is executed.") },

   { "screen",         "sc",
     NULL,
     N_("Displays the contents of the screen.") },

   { "step",            "z",
     N_("[<count>]"),
     N_("Single-step through instructions.  COUNT allows stepping\n"
     "more than a single instruction at a time.") },

   { "undump",          "",
     N_("\"<filename>\""),
     N_("Read a snapshot of the machine from the file specified.") },

/*
   { "up",              "",
     NULL,
     "*** unimplemented ***" },
*/
   { "", "", "", N_("Symbol table commands:") },

   { "add_label",       "al",
     N_("[<memspace>] <address> <label>"),
     N_("<memspace> is one of: C: 8: 9: 10: 11:\n"
     "<address>  is the address which should get the label.\n"
     "<label>    is the name of the label; it must start with a dot (\".\").\n\n"
     "Map a given address to a label.  This label can be used when entering\n"
     "assembly code and is shown during disassembly.  Additionally, it can\n"
     "be used whenever an address must be specified.") },

   { "delete_label",    "dl",
     N_("[<memspace>] <label>"),
     N_("<memspace> is one of: C: 8: 9: 10: 11:\n"
     "<label>    is the name of the label; it must start with a dot (\".\").\n\n"
     "Delete a previously defined label.") },

   { "load_labels",     "ll",
     N_("[<memspace>] \"<filename>\""),
     N_("Load a file containing a mapping of labels to addresses.  If no memory\n"
     "space is specified, the default readspace is used.\n\n" 
     "The format of the file is the one written out by the `save_labels' command;\n" 
     "it consists of some `add_label' commands, written one after the other.") },

   { "save_labels",     "sl",
     N_("[<memspace>] \"<filename>\""),
     N_("Save labels to a file.  If no memory space is specified, all of the\n"
     "labels are saved.") },

   { "show_labels",     "shl",
     N_("[<memspace>]"),
     N_("Display current label mappings.  If no memory space is specified, show\n"
     "all labels.") },

   { "", "", "", N_("Assembler and memory commands:") },

   { ">",               "",
     N_("[<address>] <data_list>"),
     N_("Write the specified data at `address'.") },

   { "a",               "",
     N_("<address> [ <instruction> [: <instruction>]* ]"),
     N_("Assemble instructions to the specified address.  If only one\n"
     "instruction is specified, enter assembly mode (enter an empty line to\n"
     "exit assembly mode).") },

   { "compare",         "c",
     N_("<address_range> <address>"),
     N_("Compare memory from the source specified by the address range to the\n"
     "destination specified by the address.  The regions may overlap.  Any\n"
     "values that miscompare are displayed using the default displaytype.") },

   { "delete",          "del",
     N_("<checknum>"),
     N_("Delete checkpoint `checknum'.") },

   { "disass",          "d",
     N_("[<address> [<address>]]"),
     N_("Disassemble instructions.  If two addresses are specified, they are\n"
     "used as a start and end address.  If only one is specified, it is\n"
     "treated as the start address and a default number of instructions are\n"
     "disassembled.  If no addresses are specified, a default number of\n"
     "instructions are disassembled from the dot address.") },

   { "fill",            "f",
     N_("<address_range> <data_list>"),
     N_("Fill memory in the specified address range with the data in\n"
     "<data_list>.  If the size of the address range is greater than the size\n"
     "of the data_list, the data_list is repeated.") },

   { "hunt",            "h",
     N_("<address_range> <data_list>"),
     N_("Hunt memory in the specified address range for the data in\n"
     "<data_list>.  If the data is found, the starting address of the match\n"
     "is displayed.  The entire range is searched for all possible matches.") },

   { "i",               "",
     N_("<address_opt_range>"),
     N_("Display memory contents as PETSCII text.") },

   { "ii",              "",
     N_("<address_opt_range>"),
     N_("Display memory contents as screen code text.") },

   { "mem",             "m",
     N_("[<data_type>] [<address_opt_range>]"),
     N_("Display the contents of memory.  If no datatype is given, the default\n"
     "is used.  If only one address is specified, the length of data\n"
     "displayed is based on the datatype.  If no addresses are given, the\n"
     "'dot' address is used.") },

   { "memchar",         "mc",
     N_("[<data_type>] [<address_opt_range>]"),
     N_("Display the contents of memory as character data.  If only one address\n"
     "is specified, only one character is displayed.  If no addresses are\n"
     "given, the ``dot'' address is used.") },

   { "memmapsave",       "mmsave",
     N_("\"<filename>\" <format>"),
     N_("Save the memmap as a picture. Format is:\n"
     "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF.") },

   { "memmapshow",       "mmsh",
     N_("[<mask>] [<address_opt_range>]"),
     N_("Show the memmap. The mask can be specified to show only those\n"
     "locations with accesses of certain type(s). The mask is a number\n"
     "with the bits \"ioRWXrwx\", where RWX are for ROM and rwx for RAM.\n"
     "Optionally, an address range can be specified.") },

   { "memmapzap",        "mmzap",
     NULL,
     N_("Clear the memmap.") },

   { "memsprite",       "ms",
     N_("[<data_type>] [<address_opt_range>]"),
     N_("Display the contents of memory as sprite data.  If only one address is\n"
     "specified, only one sprite is displayed.  If no addresses are given,\n"
     "the ``dot'' address is used.") },

   { "move",            "t",
     N_("<address_range> <address>"),
     N_("Move memory from the source specified by the address range to\n"
     "the destination specified by the address.  The regions may overlap.") },

   { "", "", "", N_("Checkpoint commands:") },

   { "break",           "",
     N_("[<address> [if <cond_expr>] ]"),
     N_("If no address is given, the currently valid watchpoints are printed.\n"
     "If an address is given, a breakpoint is set for that address and the\n"
     "breakpoint number is printed.  A conditional expression can also be\n"
     "specified for the breakpoint.  For more information on conditions, see\n"
     "the CONDITION command.") },

   { "command",         "",
     N_("<checknum> \"<command>\""),
     N_("Specify `command' as the command to execute when checkpoint `checknum'\n"
     "is hit.  Note that the `x' command is not yet supported as a\n"
     "command argument.") },

   { "condition",       "cond",
     N_("<checknum> if <cond_expr>"),
     N_("Each time the specified checkpoint is examined, the condition is\n"
     "evaluated.  If it evalutes to true, the checkpoint is activated.\n"
     "Otherwise, it is ignored.  If registers are specified in the expression,\n"
     "the values used are those at the time the checkpoint is examined, not\n"
     "when the condition is set.\n"
     "The condition can make use of registers (.A, .X, .Y, .PC, .SP) and\n"
     "compare them (==, !=, <, >, <=, >=) again other registers or constants.\n"
     "Registers can be the registers of other devices; this is denoted by\n"
     "a memspace prefix (i.e., c:, 8:, 9:, 10:, 11:\n"
     "Examples: .A == 0, .X == .Y, 8:.X == .X\n") },

   { "disable",         "",
     N_("<checknum>"),
     N_("Disable checkpoint `checknum'.") },

   { "enable",          "",
     N_("<checknum>"),
     N_("Enable checkpoint `checknum'.") },

   { "ignore",          "",
     N_("<checknum> [<count>]"),
     N_("Ignore a checkpoint a given number of crossings.  If no count is given,\n"
     "the default value is 1.") },

   { "until",           "un",
     N_("[<address>]"),
     N_("If no address is given, the currently valid breakpoints are printed.\n"
     "If an address is given, a temporary breakpoint is set for that address\n"
     "and the breakpoint number is printed.  Control is returned to the\n"
     "emulator by this command.  The breakpoint is deleted once it is hit.") },

   { "watch",           "w",
     N_("[loadstore] [address [address]]"),
     N_("Set a watchpoint.  If a single address is specified, set a watchpoint\n"
     "for that address.  If two addresses are specified, set a watchpoint\n"
     "for the memory locations between the two addresses.\n"
     "`loadstore' is either `load' or `store' to specify on which operation\n"
     "the monitor breaks. If not specified, the monitor breaks on both\n"
     "operations.") },

   { "trace",           "tr",
     N_("[address [address]]"),
     N_("Set a tracepoint.  If a single address is specified, set a tracepoint\n"
     "for that address.  If two addresses are specified, set a tracepoint\n"
     "for the memory locations between the two addresses.") },

   { "", "", "", N_("Monitor state commands:") },

   { "device",          "dev",
     N_("[c:|8:|9:|10:|11:]"),
     N_("Set the default memory device to either the computer `c:' or the\n"
     "specified disk drive (`8:', `9:').") },

   { "exit",            "x",
     NULL,
     N_("Leave the monitor and return to execution.") },

   { "quit",            "",
     NULL,
#ifdef OS2
     N_("Leave the monitor and return to execution.") },
#else
     N_("Exit the emulator immediately.")},
#endif

   { "radix",           "rad",
     N_("[H|D|O|B]"),
     N_("Set the default radix to hex, decimal, octal, or binary.  With no\n"
     "argument, the current radix is printed.") },

   { "sidefx",          "sfx",
     N_("[on|off|toggle]"),
     N_("Control how monitor generated reads affect memory locations that have\n"
     "read side-effects.  If the argument is 'on' then reads may cause\n"
     "side-effects.  If the argument is 'off' then reads don't cause\n"
     "side-effects.  If the argument is 'toggle' then the current mode is\n"
     "switched.  No argument displays the current state.") },

   { "", "", "", N_("Disk commands:") },

   { "@",               "",
     N_("<disk command>"),
     N_("Perform a disk command on the currently attached disk image on drive 8.\n"
     "The specified disk command is sent to the drive's channel #15.") },

   { "attach",          "",
     N_("<filename> <device>"),
     N_("Attach file to device. (device 32 = cart)") },

   { "bload",           "bl",
     N_("\"<filename>\" <device> <address>"),
     N_("Load the specified file into memory at the specified address.\n"
     "If device is 0, the file is read from the file system.") },

   { "block_read",      "br",
     N_("<track> <sector> [<address>]"),
     N_("Read the block at the specified track and sector.  If an address is\n"
     "specified, the data is loaded into memory.  If no address is given, the\n"
     "data is displayed using the default datatype.") },

   { "bsave",           "bs",
     N_("\"<filename>\" <device> <address1> <address2>"),
     N_("Save the memory from address1 to address2 to the specified file.\n"
     "If device is 0, the file is written to the file system.") },

   { "block_write",     "bw",
     N_("<track> <sector> <address>"),
     N_("Write a block of data at `address' on the specified track and sector\n"
     "of disk in drive 8.") },

   { "cd",              "",
     N_("<directory>"),
     N_("Change the working directory.") },

   { "detach",          "",
     N_("<device>"),
     N_("Detach file from device. (device 32 = cart)") },

   { "dir",              "ls",
     N_("[<directory>]"),
     N_("Display the directory contents.") },

   { "load",            "l",
     N_("\"<filename>\" <device> [<address>]"),
     N_("Load the specified file into memory at the specified address. Set BASIC\n"
     "pointers appropriately if loaded into computer memory (not all emulators).\n"
     "Use (otherwise ignored) two-byte load address from file if no address\n"
     "specified.\n"
     "If device is 0, the file is read from the file system.") },

   { "pwd",           "",
     NULL,
     N_("Show current working directory.") },

   { "save",            "s",
     N_("\"<filename>\" <device> <address1> <address2>"),
     N_("Save the memory from address1 to address2 to the specified file.\n"
     "Write two-byte load address.\n"
     "If device is 0, the file is written to the file system.") },

   { "", "", "", N_("Other commands:") },

   { "~",               "",
     N_("<number>"),
     N_("Display the specified number in decimal, hex, octal and binary.") },

   { "cartfreeze",      "",
     NULL,
     N_("Use cartridge freeze.") },

   { "help",            "?",
     N_("[<command>]"),
     N_("If no argument is given, prints out a list of all available commands\n" 
     "If an argument is given, prints out specific help for that command.") },

   { "keybuf",          "",
     N_("\"<string>\""),
     N_("Put the specified string into the keyboard buffer.") },

   { "playback",        "pb",
     N_("\"<filename>\""),
     N_("Monitor commands from the specified file are read and executed.  This\n"
     "command stops at the end of file or when a STOP command is read.") },

   { "print",           "p",
     N_("<expression>"),
     N_("Evaluate the specified expression and output the result.") },

   { "record",          "rec",
     N_("\"<filename>\""),
     N_("After this command, all commands entered are written to the specified\n"
     "file until the STOP command is entered.") },

   { "resourceget",     "resget",
     N_("\"<resource>\""),
     N_("Displays the value of the resource.") },

   { "resourceset",     "resset",
     N_("\"<resource>\" \"<value>\""),
     N_("Sets the value of the resource.") },

   { "stop",            "",
     NULL,
     N_("Stop recording commands.  See `record'.") },

   { "screenshot",       "scrsh",
     N_("\"<filename>\" [<format>]"),
     N_("Take a screenshot. Format is:\n"
     "default = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF.") },

   { "tapectrl",       "",
     N_("<command>"),
     N_("Control the datasette. Valid commands:\n"
     "0 = stop, 1 = start, 2 = forward, 3 = rewind, 4 = record,\n"
     "5 = reset, 6 = reset counter.") },

/*
   { "]",               "",
     NULL,
     "*** unimplemented ***" },
*/

/*
   { "brmon",           "",
     NULL,
     "*** unimplemented ***" },
*/

/*
   { "system",          "sys",
     NULL,
     "*** unimplemented ***" },
*/

/*
   { "verify",          "v",
     NULL,
     "*** unimplemented ***" },
*/

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
                mon_out("\n%s\n", _(c->description));
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
            mon_out(_("Command `%s' unknown.\n"), cmd);
        else if (mon_cmd_array[cmd_num].description == NULL)
            mon_out(_("No help available for `%s'\n"), cmd);
        else {
            const mon_cmds_t *c;

            c = &mon_cmd_array[cmd_num];

            mon_out(_("\nSyntax: %s %s\n"),
                      c->str,
                      c->param_names != NULL ? _(c->param_names) : "");
            if (!util_check_null_string(c->abbrev))
                mon_out(_("Abbreviation: %s\n"), c->abbrev);
            mon_out("\n%s\n\n", _(c->description));
        }
    }
}
