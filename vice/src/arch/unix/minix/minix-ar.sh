#!/bin/sh
# minix-ar.sh for the BEOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This script converts an 'ar cru *' call into a 'ar cr *' call

if [ "$1" = "cru" ]; then
  shift
  ar cr $*
else
  ar $*
fi
