#!/bin/sh
# ppc-ar.sh for the BEOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This script converts an 'ar cru *' call into a 'mwldppc -xml -o *' call

if [ "$1" = "cru" ]; then
  shift
  mwldppc -xml -o $*
else
  ar $*
fi
