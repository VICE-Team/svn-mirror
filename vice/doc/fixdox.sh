#!/bin/sh

#
# fixdox.sh - fix up the texi file for proper generation of the final document.
#
# Written by
#  Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This file is part of VICE, the Versatile Commodore Emulator.
# See README for copyright notice.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
#  02111-1307  USA.
#
# Usage: fixdox.sh <format-to-fix-for>
#                   $1
#

FORMAT=$1

fixtxt()
{
  while read data
  do
    if test x"$data" != "xSTART-INFO-DIR-ENTRY" -a x"$data" != "xEND-INFO-DIR-ENTRY"; then
      echo "$data"
    else
      if test x"$data" = "xSTART-INFO-DIR-ENTRY"; then
        read data
        header=""
        for i in $data
        do
          if test x"$header" != "x"; then
             header="$header $i"
          fi
          if test x"$header" = "x" -a x"$i" = "x(vice)."; then
             header=" "
          fi
        done
        echo $header
      fi
    fi
  done
}

fixpdf()
{
  echo not implemented yet
}

fixhtml()
{
  echo not implemented yet
}

if test x"$FORMAT" = "xtxt"; then
  fixtxt
fi

if test x"$FORMAT" = "xpdf"; then
  fixpdf
fi

if test x"$FORMAT" = "xhtml"; then
  fixhtml
fi
