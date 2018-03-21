#!/usr/bin/env python3
"""
Parse all Gtk3 sources and look for resource declarations

A resource declaration should be in the form:

    "$VICERES <resource-name> [<supported-emus>]"

Where <supported-emus> can be:

* Empty or 'all' to indicate all emus
* A whitespace separated list of emu names (ie 'x64 x64sc x128')
* A whitespace separated list of emu names prefixed with '-', meaning all emus
  except the ones prefixed with '-' (ie '-vsid -scpu64)
"""


import os
import os.path
import pprint
import re


# Directory with gtk3 sources to parse
GTK3_SOURCES = "src/arch/gtk3"

# Extensions of gtk3 sources to parse
GTK3_EXTENSIONS = ('.c')

# List of emus
ALL_EMUS = [ "x64", "x64sc", "xscpu64", "x64dtv", "x128", "xvic", "xpet",
        "xplus4", "xcbm5x0", "xcbm2", "vsid" ]


EMU_HEADERS = """
    x           x
    s x       x c
  x c d       p b x
  6 p t x x x l m c v
x 4 u v 1 v p u 5 b s
6 s 6 6 2 i e s x m i
4 c 4 4 8 c t 4 0 2 d
"""

# Precompile regex to make stuff run a bit faster
regex = re.compile(r"\$VICERES\s+(\w+)\s+(.*)")



def iterate_sources():
    """
    A generator that looks up gtk3 source files

    :yields:    path to gtk3 source file
    """
    for root, dirs, files in os.walk(GTK3_SOURCES):
        for name in files:
            if name.endswith(GTK3_EXTENSIONS):
                yield os.path.join(root, name)


def get_emulators(line):
    """
    Parse the emulator list, returning only the supported emulators

    :param line: string with a whitespace separated list of emulators

    :returns: list of supported emulators
    """

    emus = line.split()
    if 'all' in emus:
        # all, return full list of emulators
        return ALL_EMUS

    # handle -emu
    unsupported = []
    for emu in emus:
        if emu.startswith('-'):
            unsupported.append(emu[1:])

    if unsupported:
        return list(set(ALL_EMUS) - set(unsupported))
    else:
        return emus


def parse_source(path):
    """
    Parse a gtk3 source file for resource declarations

    :param path: path to gtk3 source file
    :returns: tuple of (resource-name, tuple of emus))
    """

    resources = []

    with open(path, "r") as infile:
        for line in infile:
            result = regex.search(line)
            if result:
                if result.group(2):
                    # TODO: handle the -emu things
                    resources.append((result.group(1),
                        get_emulators(result.group(2))))
                else:
                    resources.append((result.group(1), ['all']))
    return resources


def print_emu_header():
    """
    Print emulator types on stdout
    """
    for line in EMU_HEADERS.split('\n'):
        print(" " * 40, end="")
        print(line)


def list_resources(resources):
    """
    List resources alphabetically, with emu-support and source file

    :param resources: dictionary of resources parsed from the sources

    :returns: number of resources
    """

    num = 0

    print_emu_header()
    for res in sorted(resources.keys()):
        # print("len of resources[{}] = {}".format(res, len(resources[res])))
        for entry in resources[res]:
            print("{:40}".format(res), end="")
            filename = entry[0]
            emus = entry[1];
            for e in ALL_EMUS:
                if e in emus:
                    print("* ", end="")
                else:
                    print("- ", end="")
            print(" {}".format(filename[len(GTK3_SOURCES) + 1:]))

    return len(resources)


def list_emu_resources(resources, emu):
    """
    List resources for emu

    :param resources: dict with reource info
    :param emu: emulator name
    """

    num = 0

    for res in sorted(resources.keys()):
        for entry in resources[res]:
            if emu in entry[1]:
                print("{:40}  {}".format(res, entry[0]))
                # num++ (still not?)
                num += 1

    return num


def main():
    """
    Main driver, just for testing the code, for now

    :todo:   add command line parser to allow selecting different reports
    """

    # dictionary with the resource name as key, and a list of (filename, emus)
    # as each value
    reslist = dict()

    for source in iterate_sources():
        resources = parse_source(source)
        if resources:
            for name,emus in resources:
                if name not in reslist:
                    reslist[name] = []
                reslist[name].append((source, emus))

    # pprint.pprint(reslist)

    num = list_resources(reslist)
    # num = list_emu_resources(reslist, "vsid")

    print("\n{} documented resources found in the Gtk3 UI".format(num))


# include guard: only run the code when called as a program, allowing including
# this file in another file as a module
if __name__ == "__main__":
    main()
