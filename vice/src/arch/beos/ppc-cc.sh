#!/bin/sh
# ppc-cc.sh for the BEOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This script makes sure a 'valid' output file is generated
#   for when the configure script checks the compiler(-wrapper)

foundo=no

for i in $*
do
  if test x"$i" = "x-o" -o x"$i" = "x-c"; then
    foundo=yes
  fi
done

if test x"$foundo" = "xno"; then
  mwccppc $* -o a.out
else
  mwccppc $*
fi
