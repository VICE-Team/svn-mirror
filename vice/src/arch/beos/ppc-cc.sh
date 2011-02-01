#!/bin/sh

foundo=no

for i in $*
do
  if test x"$i" = "x-o"; then
    foundo=yes
  fi
done

if test x"$foundo" = "xno"; then
  mwccppc $* -o aout
else
  mwccppc $*
fi
