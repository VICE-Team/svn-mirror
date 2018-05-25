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

#include "lib.h"
#include "mon_command.h"
#include "asm.h"
#include "montypes.h"

#ifdef AMIGA_MORPHOS
#undef REG_PC
#endif

#include "mon_parse.h" /* FIXME ! */
#include "mon_util.h"
#include "translate.h"
#include "uimon.h"
#include "util.h"

#define GET_DESCRIPTION(c) ((c->use_description_id == USE_DESCRIPTION_ID) ? translate_text(c->description_id) : _(c->description))

typedef struct mon_cmds_s {
    const char *str;
    const char *abbrev;
    int use_param_names_id;
    int use_description_id;
    char *braces;
    int param_amount;
    int param_ids[4];
    int description_id;
    const char *param_names;
    const char *description;
    const int filename_as_arg;
} mon_cmds_t;

#define NO_FILENAME_ARG    0
#define FILENAME_ARG       1

static const mon_cmds_t mon_cmd_array[] = {
    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Available commands are:",
      NO_FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Machine state commands:",
      NO_FILENAME_ARG
    },

    { "bank", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] [%s]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>] [bankname]",
      "If bankname is not given, print the possible banks for the memspace.\n"
      "If bankname is given set the current bank in the memspace to the given\n"
      "bank.",
      NO_FILENAME_ARG
    },

    { "backtrace", "bt",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Print JSR call chain (most recent call first). Stack offset\n"
      "relative to SP+1 is printed in parentheses. This is a best guess\n"
      "only.",
      NO_FILENAME_ARG
    },

    { "cpu", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<Type>",
      "Specify the type of CPU currently used (6502/z80).",
      NO_FILENAME_ARG
    },

    { "cpuhistory", "chis",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<count>]",
      "Show <count> last executed commands.",
      NO_FILENAME_ARG
    },

    { "dump", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "\"<filename>\"",
      "Write a snapshot of the machine into the file specified.\n"
      "This snapshot is compatible with a snapshot written out by the UI.\n"
      "Note: No ROM images are included into the dump.",
      FILENAME_ARG
    },

    { "export", "exp",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Print out list of attached expansion port devices.",
      NO_FILENAME_ARG
    },

    { "goto", "g",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address>",
      "Change the PC to ADDRESS and continue execution",
      NO_FILENAME_ARG
    },

    { "io", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address>",
      "Print out the I/O area of the emulated machine.",
      NO_FILENAME_ARG
    },

    { "next", "n",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Advance to the next instruction.  Subroutines are treated as\n"
      "a single instruction.",
      NO_FILENAME_ARG
    },

    { "registers", "r",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s> = <%s> [, <%s> = <%s>]*]", 4,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<reg_name> = <number> [, <reg_name> = <number>]*]",
      "Assign respective registers.  With no parameters, display register\n"
      "values.",
      NO_FILENAME_ARG
    },

    { "reset", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<Type>]",
      "Reset the machine or drive. Type: 0 = soft, 1 = hard, 8-11 = drive.",
      NO_FILENAME_ARG
    },

    { "return", "ret",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Continues execution and returns to the monitor just after the next\n"
      "RTS or RTI is executed.",
      NO_FILENAME_ARG
    },

    { "screen", "sc",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Displays the contents of the screen.",
      NO_FILENAME_ARG
    },

    { "step", "z",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<count>]",
      "Single-step through instructions.  COUNT allows stepping\n"
      "more than a single instruction at a time.",
      NO_FILENAME_ARG
    },

    { "stopwatch", "sw",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[reset]", 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Print the CPU cycle counter of the current device. 'reset' sets the counter to 0.",
      NO_FILENAME_ARG
    },

    { "undump", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "\"<filename>\"",
      "Read a snapshot of the machine from the file specified.",
      FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Symbol table commands:",
      NO_FILENAME_ARG
    },

    { "add_label", "al",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] <%s> <%s>", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>] <address> <label>",
      "<memspace> is one of: C: 8: 9: 10: 11:\n"
      "<address>  is the address which should get the label.\n"
      "<label>    is the name of the label; it must start with a dot (\".\").\n"
      "\n"
      "Map a given address to a label.  This label can be used when entering\n"
      "assembly code and is shown during disassembly.  Additionally, it can\n"
      "be used whenever an address must be specified.",
      NO_FILENAME_ARG
    },

    { "delete_label", "dl",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>] <label>",
      "<memspace> is one of: C: 8: 9: 10: 11:\n"
      "<label>    is the name of the label; it must start with a dot (\".\").\n"
      "\n"
      "Delete a previously defined label.",
      NO_FILENAME_ARG
    },

    { "load_labels", "ll",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] \"<%s>\"", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>] \"<filename>\"",
      "Load a file containing a mapping of labels to addresses.  If no memory\n"
      "space is specified, the default readspace is used.\n"
      "\n"
      "The format of the file is the one written out by the `save_labels' command;\n"
      "it consists of some `add_label' commands, written one after the other.",
      FILENAME_ARG
    },

    { "save_labels", "sl",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] \"<%s>\"", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>] \"<filename>\"",
      "Save labels to a file.  If no memory space is specified, all of the\n"
      "labels are saved.",
      FILENAME_ARG
    },

    { "show_labels", "shl",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>]",
      "Display current label mappings.  If no memory space is specified, show\n"
      "all labels from default address space.",
      NO_FILENAME_ARG
    },

    { "clear_labels", "cl",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<memspace>]",
      "Clear current label mappings.  If no memory space is specified, clear\n"
      "all labels from default address space.",
      NO_FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Assembler and memory commands:",
      NO_FILENAME_ARG
    },

    { ">", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<address>] <data_list>",
      "Write the specified data at `address'.",
      NO_FILENAME_ARG
    },

    { "a", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> [ <%s> [: <%s>]* ]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address> [ <instruction> [: <instruction>]* ]",
      "Assemble instructions to the specified address.  If only one\n"
      "instruction is specified, enter assembly mode (enter an empty line to\n"
      "exit assembly mode).",
      NO_FILENAME_ARG
    },

    { "compare", "c",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_range> <address>",
      "Compare memory from the source specified by the address range to the\n"
      "destination specified by the address.  The regions may overlap.  Any\n"
      "values that miscompare are displayed using the default displaytype.",
      NO_FILENAME_ARG
    },

    { "disass", "d",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s> [<%s>]]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<address> [<address>]]",
      "Disassemble instructions.  If two addresses are specified, they are\n"
      "used as a start and end address.  If only one is specified, it is\n"
      "treated as the start address and a default number of instructions are\n"
      "disassembled.  If no addresses are specified, a default number of\n"
      "instructions are disassembled from the dot address.",
      NO_FILENAME_ARG
    },

    { "fill", "f",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_range> <data_list>",
      "Fill memory in the specified address range with the data in\n"
      "<data_list>.  If the size of the address range is greater than the size\n"
      "of the data_list, the data_list is repeated.",
      NO_FILENAME_ARG
    },

    { "hunt", "h",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_range> <data_list>",
      "Hunt memory in the specified address range for the data in\n"
      "<data_list>.  If the data is found, the starting address of the match\n"
      "is displayed.  The entire range is searched for all possible matches.",
      NO_FILENAME_ARG
    },

    { "i", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_opt_range>",
      "Display memory contents as PETSCII text.",
      NO_FILENAME_ARG
    },

    { "ii", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_opt_range>",
      "Display memory contents as screen code text.",
      NO_FILENAME_ARG
    },

    { "mem", "m",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<data_type>] [<address_opt_range>]",
      "Display the contents of memory.  If no datatype is given, the default\n"
      "is used.  If only one address is specified, the length of data\n"
      "displayed is based on the datatype.  If no addresses are given, the\n"
      "'dot' address is used.\n"
      "Please note: due to the ambiguous meaning of 'b' and 'd' these data-\n"
      "types must be given in uppercase!",
      NO_FILENAME_ARG
    },

    { "memchar", "mc",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<data_type>] [<address_opt_range>]",
      "Display the contents of memory as character data.  If only one address\n"
      "is specified, only one character is displayed.  If no addresses are\n"
      "given, the ``dot'' address is used.",
      NO_FILENAME_ARG
    },

    { "memmapsave", "mmsave",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "\"<%s>\" <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "\"<filename>\" <Format>",
      "Save the memmap as a picture. Format is:\n"
      "0 = BMP, 1 = PCX, 2 = PNG, 3 = GIF, 4 = IFF.",
      FILENAME_ARG
    },

    { "memmapshow", "mmsh",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<mask>] [<address_opt_range>]",
      "Show the memmap. The mask can be specified to show only those\n"
      "locations with accesses of certain type(s). The mask is a number\n"
      "with the bits \"ioRWXrwx\", where RWX are for ROM and rwx for RAM.\n"
      "Optionally, an address range can be specified.",
      NO_FILENAME_ARG
    },

    { "memmapzap", "mmzap",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Clear the memmap.",
      NO_FILENAME_ARG
    },

    { "memsprite", "ms",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>] [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<data_type>] [<address_opt_range>]",
      "Display the contents of memory as sprite data.  If only one address is\n"
      "specified, only one sprite is displayed.  If no addresses are given,\n"
      "the ``dot'' address is used.",
      NO_FILENAME_ARG
    },

    { "move", "t",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<address_range> <address>",
      "Move memory from the source specified by the address range to\n"
      "the destination specified by the address.  The regions may overlap.",
      NO_FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Checkpoint commands:",
      NO_FILENAME_ARG
    },

    { "break", "bk",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[load|store|exec] [%s [%s] [if <%s>]]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[load|store|exec] [address [address] [if <cond_expr>]]",
      "Set a breakpoint, If no address is given, the currently valid break-\n"
      "points are printed.\n"
      "If an address is given, a breakpoint is set for that address and the\n"
      "breakpoint number is printed.\n"
      "`load|store|exec' is either `load', `store' or `exec' (or any combina-\n"
      "tion of these) to specify on which operation the monitor breaks. If\n"
      "not specified, the monitor breaks on `exec'.\n"
      "A conditional expression can also be specified for the breakpoint.\n"
      "For more information on conditions, see the CONDITION command.",
      NO_FILENAME_ARG
    },

    { "command", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> \"<%s>\"", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum> \"<Command>\"",
      "Specify `command' as the command to execute when checkpoint `checknum'\n"
      "is hit.  Note that the `x' command is not yet supported as a\n"
      "command argument.",
      NO_FILENAME_ARG
    },

    { "condition", "cond",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> if <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum> if <cond_expr>",
      "Each time the specified checkpoint is examined, the condition is\n"
      "evaluated. If it evalutes to true, the checkpoint is activated.\n"
      "Otherwise, it is ignored. If registers are specified in the expression,\n"
      "the values used are those at the time the checkpoint is examined, not\n"
      "when the condition is set.\n"
      "The condition can use registers (A, X, Y, PC, SP, FL and other cpu\n"
      "specific registers (see manual)) and compare them (==, !=, <, >, <=, >=)\n"
      "against other registers or constants.\n"
      "Registers can be the registers of other devices; this is denoted by\n"
      "a memspace prefix (i.e., c:, 8:, 9:, 10:, 11:\n"
      "Examples: A == $0, X == Y, 8:X == X)\n"
      "You can also compare against the value of a memory location in a specific\n"
      "bank, i.e you can break only if the vic register $d020 is $f0.\n"
      "use the form @[bankname]:[$<address>] | [.label].\n"
      "Note this is for the C : memspace only.\n"
      "Examples : if @io:$d020 == $f0, if @io:.vicBorder == $f0",
      NO_FILENAME_ARG
    },

    { "delete", "del",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum>",
      "Delete checkpoint `checknum'. If no checkpoint is specified delete all checkpoints.",
      NO_FILENAME_ARG
    },

    { "disable", "dis",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum>",
      "Disable checkpoint `checknum'. If no checkpoint is given all checkpoints will be disabled.",
      NO_FILENAME_ARG
    },

    { "enable", "en",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum>",
      "Enable checkpoint `checknum'. If no checkpoint is given all checkpoints will be enabled.",
      NO_FILENAME_ARG
    },

    { "ignore", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<checknum> [<count>]",
      "Ignore a checkpoint a given number of crossings.  If no count is given,\n"
      "the default value is 1.",
      NO_FILENAME_ARG
    },

    { "until", "un",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[<address>]",
      "If no address is given, the currently valid breakpoints are printed.\n"
      "If an address is given, a temporary breakpoint is set for that address\n"
      "and the breakpoint number is printed.  Control is returned to the\n"
      "emulator by this command.  The breakpoint is deleted once it is hit.",
      NO_FILENAME_ARG
    },

    { "watch", "w",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[load|store|exec] [%s [%s] [if <%s>]]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[load|store|exec] [address [address] [if <cond_expr>]]",
      "Set a watchpoint. If no address is given, the currently valid watch-\n"
      "points are printed. If a single address is specified, set a watchpoint\n"
      "for that address.  If two addresses are specified, set a watchpoint\n"
      "for the memory locations between the two addresses.\n"
      "`load|store|exec' is either `load', `store' or `exec' (or any combina-\n"
      "tion of these) to specify on which operation the monitor breaks. If\n"
      "not specified, the monitor breaks on `load' and `store'.",
      NO_FILENAME_ARG
    },

    { "trace", "tr",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "[load|store|exec] [%s [%s] [if <%s>]]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[load|store|exec] [address [address] [if <cond_expr>]]",
      "Set a tracepoint. If no address is given, the currently valid trace-\n"
      "points are printed. If a single address is specified, set a tracepoint\n"
      "for that address.  If two addresses are specified, set a tracepoint\n"
      "for the memory locations between the two addresses.\n"
      "`load|store|exec' is either `load', `store' or `exec' (or any combina-\n"
      "tion of these) to specify on which operation the monitor breaks. If\n"
      "not specified, the monitor traces all three operations.",
      NO_FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Monitor state commands:",
      NO_FILENAME_ARG
    },

    { "device", "dev",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[c:|8:|9:|10:|11:]",
      "Set the default memory device to either the computer `c:' or the\n"
      "specified disk drive (`8:', `9:').",
      NO_FILENAME_ARG
    },

    { "exit", "x",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Leave the monitor and return to execution.",
      NO_FILENAME_ARG
    },

    { "quit", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      NULL,
      "Exit the emulator immediately.",
      NO_FILENAME_ARG
    },

    { "radix", "rad",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "[H|D|O|B]",
      "Set the default radix to hex, decimal, octal, or binary.  With no\n"
      "argument, the current radix is printed.",
      NO_FILENAME_ARG
    },

    { "sidefx", "sfx",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_SIDEFX_DESCRIPTION,
      "[on|off|toggle]",
      "Control how monitor generated reads affect memory locations that have\n"
      "read side-effects.  If the argument is 'on' then reads may cause\n"
      "side-effects.  If the argument is 'off' then reads don't cause\n"
      "side-effects.  If the argument is 'toggle' then the current mode is\n"
      "switched.  No argument displays the current state.",
      NO_FILENAME_ARG
    },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "",
      "Disk commands:",
      NO_FILENAME_ARG
    },

    { "@", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<disk command>",
      "Perform a disk command on the currently attached disk image on drive 8.\n"
      "The specified disk command is sent to the drive's channel #15.",
      NO_FILENAME_ARG
    },

    { "attach", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s>", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<filename> <device>",
      "Attach file to device. (device 32 = cart)",
      FILENAME_ARG
    },

    { "autostart", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> [%s]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<filename> [file_index]",
      "Autostart a given disk/tape image or program.",
      FILENAME_ARG
    },

    { "autoload", "",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> [%s]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<filename> [file_index]",
      "Autoload given disk/tape image or program.",
      FILENAME_ARG
    },

    { "bload", "bl",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "\"<%s>\" <%s> <%s>", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "\"<filename>\" <device> <address>",
      "Load the specified file into memory at the specified address.\n"
      "If device is 0, the file is read from the file system.",
      FILENAME_ARG
    },

    { "block_read", "br",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s> [<%s>]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<track> <sector> [<address>]",
      "Read the block at the specified track and sector.  If an address is\n"
      "specified, the data is loaded into memory.  If no address is given, the\n"
      "data is displayed using the default datatype.",
      NO_FILENAME_ARG
    },

    { "bsave", "bs",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "\"<%s>\" <%s> <%s1> <%s2>", 4,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "\"<filename>\" <device> <address1> <address2>",
      "Save the memory from address1 to address2 to the specified file.\n"
      "If device is 0, the file is written to the file system.",
      FILENAME_ARG
    },

    { "block_write", "bw",
      USE_PARAM_STRING, USE_DESCRIPTION_STRING,
      "<%s> <%s> <%s>", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_UNUSED,
      "<track> <sector> <address>",
      "Write a block of data at `address' on the specified track and sector\n"
      "of disk in drive 8.",
      NO_FILENAME_ARG
    },

    { "cd", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_CD_DESCRIPTION,
      "<Directory>", NULL, NO_FILENAME_ARG },

    { "detach", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_DETACH_DESCRIPTION,
      "<device>", NULL, NO_FILENAME_ARG },

    { "dir", "ls",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_DIR_DESCRIPTION,
      "[<Directory>]", NULL, NO_FILENAME_ARG },

    { "list", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_LIST_DESCRIPTION,
      "[<Directory>]", NULL, NO_FILENAME_ARG },

    { "load", "l",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\" <%s> [<%s>]", 3,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_LOAD_DESCRIPTION,
      "\"<filename>\" <device> [<address>]", NULL, FILENAME_ARG },

    { "pwd", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_PWD_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "save", "s",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\" <%s> <%s1> <%s2>", 4,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_SAVE_DESCRIPTION,
      "\"<filename>\" <device> <address1> <address2>", NULL, FILENAME_ARG },

    { "", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_OTHER_COMMANDS,
      "", NULL, NO_FILENAME_ARG },

    { "~", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_DISPLAY_NUMBER_DESCRIPTION,
      "<number>", NULL, NO_FILENAME_ARG },

    { "cartfreeze", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_CARTFREEZE_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "help", "?",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "[<%s>]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_HELP_DESCRIPTION,
      "[<Command>]", NULL, NO_FILENAME_ARG },

    { "keybuf", "",
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_STRING, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_KEYBUF_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "playback", "pb",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_PLAYBACK_DESCRIPTION,
      "\"<filename>\"", NULL, FILENAME_ARG },

    { "print", "p",
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      "<%s>", 1,
      { IDGS_EXPRESSION, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_PRINT_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "record", "rec",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_RECORD_DESCRIPTION,
      "\"<filename>\"", NULL, FILENAME_ARG },

    { "resourceget", "resget",
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_RESOURCE, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_RESOURCEGET_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "resourceset", "resset",
      USE_PARAM_ID, USE_DESCRIPTION_ID,
      "\"<%s>\" \"<%s>\"", 2,
      { IDGS_RESOURCE, IDGS_VALUE, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_RESOURCESET_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "load_resources", "resload",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_LOAD_RESOURCES_DESCRIPTION,
      "\"<filename>\"", NULL, FILENAME_ARG },

    { "save_resources", "ressave",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\"", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_SAVE_RESOURCES_DESCRIPTION,
      "\"<filename>\"", NULL, FILENAME_ARG },

    { "stop", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      NULL, 0,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_STOP_DESCRIPTION,
      NULL, NULL, NO_FILENAME_ARG },

    { "screenshot", "scrsh",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "\"<%s>\" [<%s>]", 2,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_SCREENSHOT_DESCRIPTION,
      "\"<filename>\" [<Format>]", NULL, FILENAME_ARG },

    { "tapectrl", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "<%s>", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_TAPECTRL_DESCRIPTION,
      "<Command>", NULL, NO_FILENAME_ARG },

    { "maincpu_trace", "",
      USE_PARAM_STRING, USE_DESCRIPTION_ID,
      "[on|off|toggle]", 1,
      { IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED, IDGS_UNUSED },
      IDGS_MON_MAINCPU_TRACE_DESCRIPTION,
      "[on|off|toggle]", NULL, NO_FILENAME_ARG },

    { NULL, NULL, 0, 0, NULL, 0, { 0, 0, 0, 0 }, 0, NULL, NULL, 0 }
};

int mon_get_nth_command(int index, const char** full_name, const char **short_name, int *takes_filename_as_arg)
{
    if (index < 0 || index >= sizeof(mon_cmd_array) / sizeof(*mon_cmd_array) - 1) {
        return 0;
    }
    *full_name = mon_cmd_array[index].str;
    *short_name = mon_cmd_array[index].abbrev;
    *takes_filename_as_arg = mon_cmd_array[index].filename_as_arg;
    return 1;
}

static int mon_command_lookup_index(const char *str)
{
    int num = 0;

    if (str == NULL) {
        return -1;
    }

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
    char *parameters;
    char *braces;
    int param_amount;

    if (cmd == NULL) {
        longest = 0;
        for (c = mon_cmd_array; c->str != NULL; c++) {
            len = (int)strlen(c->str);
            if (!util_check_null_string(c->abbrev)) {
                len += 3 + (int)strlen(c->abbrev); /* 3 => " ()" */
            }
            if (len > longest) {
                longest = len;
            }
        }
        longest += 2; /* some space */
        max_col = 80 / longest - 1;

        column = 0;
        for (c = mon_cmd_array; c->str != NULL; c++) {
            int tot = (int)strlen(c->str);

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
                tot += 3 + (int)strlen(c->abbrev);
            }

            if (column >= max_col) {
                mon_out("\n");
                column = 0;
            } else {
                for (; tot < longest; tot++) {
                    mon_out(" ");
                }
                column++;
            }
            if (mon_stop_output != 0) {
                break;
            }
        }
        mon_out("\n\n");
    } else {
        int cmd_num;

        cmd_num = mon_command_lookup_index(cmd);

        if (cmd_num == -1) {
            mon_out(translate_text(IDGS_COMMAND_S_UNKNOWN), cmd);
        } else if (mon_cmd_array[cmd_num].description == NULL && mon_cmd_array[cmd_num].description_id == IDGS_UNUSED) {
            mon_out(translate_text(IDGS_NO_HELP_AVAILABLE_FOR_S), cmd);
        } else {
            const mon_cmds_t *c;

            c = &mon_cmd_array[cmd_num];

            if (c->use_param_names_id == USE_PARAM_ID) {
                braces = c->braces;
                param_amount = c->param_amount;
                switch (param_amount) {
                    default:
                    case 1:
                        parameters = lib_msprintf(braces, translate_text(c->param_ids[0]));
                        break;
                    case 2:
                        parameters = lib_msprintf(braces, translate_text(c->param_ids[0]),
                                                  translate_text(c->param_ids[1]));
                        break;
                    case 3:
                        parameters = lib_msprintf(braces, translate_text(c->param_ids[0]),
                                                  translate_text(c->param_ids[1]),
                                                  translate_text(c->param_ids[2]));
                        break;
                    case 4:
                        parameters = lib_msprintf(braces, translate_text(c->param_ids[0]),
                                                  translate_text(c->param_ids[1]),
                                                  translate_text(c->param_ids[2]),
                                                  translate_text(c->param_ids[3]));
                        break;
                }
            } else {
                if (c->param_names == NULL) {
                    parameters = NULL;
                } else {
                    parameters = lib_stralloc(_(c->param_names));
                }
            }

            mon_out(translate_text(IDGS_SYNTAX_S_S),
                    c->str,
                    parameters != NULL ? parameters : "");
            lib_free(parameters);
            if (!util_check_null_string(c->abbrev)) {
                mon_out(translate_text(IDGS_ABBREVIATION_S), c->abbrev);
            }
            mon_out("\n%s\n\n", GET_DESCRIPTION(c));
        }
    }
}
