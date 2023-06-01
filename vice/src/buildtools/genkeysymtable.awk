#!/bin/awk -f
#
# Process `vhkkeysyms.h` to generate sorted table of key names with symbolic
# values. This script is used by `genkeysymtable.sh`.

/^[#]define VHK_KEY_[a-zA-Z0-9_]+\s+0x[0-9a-fA-F]+/ {
    define = $2
    value  = $3
    keysym = "\"" substr(define, 9) "\","
    printf "    { %-25s %-28s }, /* %s */\n", keysym, define, value
}
