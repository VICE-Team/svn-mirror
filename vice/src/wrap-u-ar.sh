#!/bin/sh
# wrap-u-ar.sh for 'complaining' ar commands
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This script converts an 'ar cru *' call into a 'ar cr *' call

REAL_AR="$1"
shift

if [ "$1" = "cru" ]; then
  shift
  $REAL_AR cr $*
else
  $REAL_AR $*
fi
