#!/usr/bin/env python3
#
# Scan src/arch/gtk/uiactions.h and produce text to be used in the Vim syntax
# file and the hotkeys documentation.
#
# @author:  Bas Wassink <b.wassink@ziggo.nl>


import sys
import os.path
import re
import pprint


# 'VICE_MACHINE_$EMU' symbols without the 'VICE_MACHINE_' prefix mapped to
# emulator names
EMU_MASKS = [('C64',    'x64'),
             ('C64SC',  'x64sc'),
             ('C64DTV', 'x64dtv'),
             ('SCPU64', 'xscpu64'),
             ('C128',   'x128'),
             ('VIC20',  'xvic'),
             ('PLUS4',  'xplus4'),
             ('PET',    'xpet'),
             ('CBM5x0', 'xcbm5x0'),
             ('CBM6x0', 'xcbm2')]


def mask_to_bools(mask):
    # split mask into list of emu names with prefix stripped
    machines = [m[len('VICE_MACHINE_'):] for m in mask.split('|')]

    if machines[0] == 'ALL':
        return ['yes'] * len(EMU_MASKS)

    machine_names = [m for m, _ in EMU_MASKS]
    return ['yes' if name in machines else 'no' for name in machine_names]


def usage():
    """
    Output usage message on stdout.
    """

    print("Usage: {0} <command>".format(os.path.basename(sys.argv[0])))
    print()
    print('Where <command> is one of the following:')
    print()
    print('    help        show this text')
    print('    markdown    output markdown table of all UI actions')
    print('    vim         output Vim syntax rules for all UI actions')


def markdown(actions, namewidth, descwidth):
    """
    Output markdown table with UI action names, descriptions and emulator
    support on stdout.

    @param actions: dict with UI actions
    @param namewidth: maximum width of an action name
    @param descwidth: maximum width of an action description
    """

    # output table header
    emu_names = [emu for _, emu in EMU_MASKS]
    emu_header = ' | '.join(emu_names)
    emu_separator = ' | '.join(['-' * len(emu) for emu in emu_names])
    print('| {0:{nwidth}} | {1:{dwidth}} | {2} |'.format(
        'name', 'description', emu_header, nwidth=namewidth + 2, dwidth=descwidth))
    print('| {0:-<{nwidth}} | {1:-<{dwidth}} | {2} |'.format(
        ':-', ':-', emu_separator, nwidth=namewidth + 2, dwidth=descwidth))

    # output rows
    for key in sorted(actions):
        action = actions[key]

        # transform mask into 'yes'/'no' entries
        emus = mask_to_bools(action['mask'])
        emus_map = zip([name for name in emu_names], emus)
        yesno = ' | '.join(['{0:{width}}'.format(state, width=len(name))
                            for name, state in emus_map])

        print('| {0:{nwidth}} | {1:{dwidth}} | {2} |'.format(
            '`' + action['name'] + '`', action['desc'], yesno,
            nwidth=namewidth + 2, dwidth=descwidth))


def vim(actions):
    """
    Output Vim syntax highlighting pattern matches for all UI actions

    @param actions: dict with UI actions
    """

    for key in sorted(actions):
        print('syn match vhkActionName "\\<{0}\\>"'.format(actions[key]['name']))


def main():
    """
    Program driver.

    Expects a single argument: the command to invoke.
    """

    if len(sys.argv) < 2 or sys.argv[1] in ['help', '-h', '--help']:
        usage()
        sys.exit(0)

    command = sys.argv[1]
    if command not in ['markdown', 'vim']:
        print('error: unknown command \'{0}\''.format(command))
        sys.exit(1)

    define = re.compile(r'#define\s+(ACTION_[a-zA-Z0-9_]+)\s+(.*)')
    actions = {}
    maxlen_name = 0
    maxlen_desc = 0

    with open('src/arch/gtk3/uiactions.h', 'r') as infile:
        for line in infile:
            result = define.search(line)
            if result:
                symbol = result.group(1)
                value = result.group(2)

                # strip quotes and parentheses
                if value[0] in ('"', '('):
                    value = value[1:-1]

                # handle each #define ... line
                if symbol.endswith('_MASK'):
                    actions[symbol[:-5]]['mask'] = value
                elif symbol.endswith('_DESC'):
                    actions[symbol[:-5]]['desc'] = value
                    if len(value) > maxlen_desc:
                        maxlen_desc = len(value)
                else:
                    # unadorned symbols come first in the input, so this should
                    # be safe:
                    actions[symbol] = { 'name': value,
                                        'desc': None,
                                        'mask': None }
                    if len(value) > maxlen_name:
                        maxlen_name = len(value)

    # print("maxlen_name = {0}, maxlen_desc = {1}".format(maxlen_name, maxlen_desc))
    if command == 'markdown':
        markdown(actions, maxlen_name, maxlen_desc)
    elif command == 'vim':
        vim(actions)


# Make script importable
if __name__ == '__main__':
    main()
