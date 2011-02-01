#!/bin/sh

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
