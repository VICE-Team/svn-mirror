#!/usr/bin/env python3
#
# Scan src/arch/shared/uiactions.c and produce text to be used in the Vim syntax
# file and the hotkeys documentation.
#
# @author:  Bas Wassink <b.wassink@ziggo.nl>

import sys
import os.path
import re


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
    print('    texinfo     output texinfo table of all UI actions')
    print('    vim         output Vim syntax rules for all UI actions')


def markdown(actions, namewidth, descwidth):
    """
    Output markdown table with UI action names and descriptions on stdout.

    @param actions: dict with UI actions
    @param namewidth: maximum width of an action name
    @param descwidth: maximum width of an action description
    """

    # output rows
    for key in sorted(actions):
        action = actions[key]
        print('| {0:{namewidth}} | {1:{descwidth}} |'.format(
            '`' + action['name'] + '`', action['desc'],
            namewidth=namewidth + 2, descwidth=descwidth))


def texinfo(actions):
    """
    Output texinfo table with UI action names and descriptions on stdout.

    @param actions: dict with UI actions
    """

    for key in sorted(actions):
        action = actions[key]
        print('@item @code{{{0}}}'.format(action['name']))
        print('@tab {0}'.format(action['desc']))


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
    if command not in ['markdown', 'texinfo', 'vim']:
        print('error: unknown command \'{0}\''.format(command))
        sys.exit(1)

    define = re.compile(r'    { (ACTION_[a-zA-Z0-9_]+),\s*"(.*)",\s*"(.*)"')
    actions = {}
    maxlen_name = 0
    maxlen_desc = 0

    with open('src/arch/shared/uiactions.c', 'r') as infile:
        for line in infile:
            result = define.search(line)
            if result:
                symbol = result.group(1)
                value  = result.group(2)
                desc   = result.group(3)

                actions[symbol] = { 'name': value,
                                    'desc': desc }
                if len(value) > maxlen_name:
                    maxlen_name = len(value)
                if len(desc) > maxlen_desc:
                    maxlen_desc = len(desc)

    # print("maxlen_name = {0}, maxlen_desc = {1}".format(maxlen_name, maxlen_desc))
    if command == 'markdown':
        markdown(actions, maxlen_name, maxlen_desc)
    elif command == 'texinfo':
        texinfo(actions)
    elif command == 'vim':
        vim(actions)


# Make script importable
if __name__ == '__main__':
    main()
