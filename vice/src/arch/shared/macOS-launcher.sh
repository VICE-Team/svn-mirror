#!/bin/bash

#
# macOS-launcher.sh - macOS Application Launcher for the VICE Project
#
# Written by
#  Christian Vogelgsang <chris@vogelgsang.org>
#  Michael C. Martin <mcmartin@gmail.com>
#  David Hogan <david.q.hogan@gmail.com>
#
# inspired by Gimp.app of Aaron Voisine <aaron@voisine.org>
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

# Strip macOS process serial number from args, if present
if [[ "$1" = -psn_* ]]; then
    shift
fi

cd "$(dirname "$0")"
source bin/common-runtime.sh
source bin/ui-runtime.sh

# --- find VICE binary ---
# if not provided via $PROGRAM, derive emu name from executing script name
# and fall back to an interactive user choice.
if [ -z "$PROGRAM" ]; then
  case "$(basename "$0")" in
  x128*)
    PROGRAM=x128
    ;;
  x64dtv*)
    PROGRAM=x64dtv
    ;;
  x64sc*)
    PROGRAM=x64sc
    ;;
  xcbm2*)
    PROGRAM=xcbm2
    ;;
  xcbm5x0*)
    PROGRAM=xcbm5x0
    ;;
  xpet*)
    PROGRAM=xpet
    ;;
  xplus4*)
    PROGRAM=xplus4
    ;;
  xvic*)
    PROGRAM=xvic
    ;;
  script*)
    # pick emu name in dialog
    PROGRAM=`osascript -e 'first item of (choose from list {"x128","x64dtv","x64sc","xcbm2","xcbm5x0","xpet","xplus4","xvic"} with title "VICE Emulator" with prompt "Please select an Emulator to run:" default items {"x64sc"})'`
    ;;
  *)
    # invalid bundle name
    osascript -e 'display dialog "Invalid Bundle Name / PROGRAM var! (use: x128,x64dtv,x64sc,xcbm2,xcbm5x0,xpet,xplus4,xvic)" buttons {"Abort"} with icon stop'
    PROGRAM=""
    ;;
  esac
fi

if [ "$PROGRAM" != "" ]; then
  "./bin/$PROGRAM" "$@"
fi

exit 0

