#!/bin/bash
# Generate VIM syntax rules for VICE .vhk or .vkm files
#
# Author:   Bas Wassink
# Updated:  2021-10-07
#
# TODO: Perhaps filter out stuff like Braille and asian languages? We currently
#       get over 1200 matches.
#

# Location of the GDK keysyms header on Debian
GDKKEYSYMS=/usr/include/gtk-3.0/gdk/gdkkeysyms.h
# Pattern to match, capturing the text after 'GDK_KEY_'
SEARCH='^#define GDK_KEY_\([0-9a-zA-Z_]\+\).*'
# Replacement pattern for VICE hotkeys
REPLACE_VHK='syn match vhkGdkKeyname "\\<\1\\>"'
# Replacement pattern for VICE keymaps
REPLACE_VKM='syn match vkmGdkKeyname "\\<\1\\>" nextgroup=vkmRowNumber skipwhite contains=vkmNumber'


# Print help on stdout
function usage()
{
    echo "$(basename $0) - Generate Vim syntax rules to match Gdk key names"
    echo
    echo "Usage: $(basename $0) <filetype>"
    echo
    echo "Where <filetype> is one of:"
    echo "    vhk   VICE hotkeys"
    echo "    vkm   VICE keymaps"
}


# Script entry point
if [ -z "$1" ]; then
    usage
    exit 1
fi

case "$1" in
    -h|--help)
        usage
        exit 0
        ;;
    vhk)
        cat ${GDKKEYSYMS} | sed -n "s/${SEARCH}/${REPLACE_VHK}/p"
        ;;
    vkm)
        cat ${GDKKEYSYMS} | sed -n "s/${SEARCH}/${REPLACE_VKM}/p"
        ;;
    *)
        echo "$(basename $0): error: unrecognized file type '$1'" >&2
        exit 1
        ;;
esac



