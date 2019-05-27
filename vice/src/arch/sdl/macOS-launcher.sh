#!/bin/bash

#
# macOS-launcher.sh - macOS Application Launcher for the VICE Project
#
# Written by
#  Christian Vogelgsang <chris@vogelgsang.org>
#  Michael C. Martin <mcmartin@gmail.com>
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

# --- debug echo ---
# only available if DEBUG_VICE_LAUNCHER is defined
dbgecho () {
  [ "$DEBUG_VICE_LAUNCHER" != "" ] && echo "$@"
  [ "$LOG_VICE_LAUNCHER" != "" ] && echo "$@" >> $HOME/vice_launcher.log
}

# --- find bundle name and resource dir ---
SCRIPT_DIR="`dirname \"$0\"`"
RESOURCES_DIR="`cd \"$SCRIPT_DIR/../Resources\" && pwd`"
BUNDLE_DIR="`cd \"$SCRIPT_DIR/../..\" && pwd`"
BUNDLE_NAME="`basename \"$BUNDLE_DIR\" .app`"
dbgecho "BUNDLE_DIR=$BUNDLE_DIR"
dbgecho "BUNDLE=$BUNDLE_NAME"
dbgecho "ARGS=""$@"

# --- determine launch environment ---
LAUNCH=cmdline
# finder always appends a -psn_ switch
echo "$1" | grep -e -psn_ > /dev/null
if [ "$?" == "0" ]; then
  LAUNCH=finder
fi

# --- setup environment ---
# setup dylib path
LIB_DIR="$RESOURCES_DIR/lib"
if [ -d "$LIB_DIR" ]; then
  export DYLD_FALLBACK_LIBRARY_PATH="$LIB_DIR"
fi
dbgecho "export DYLD_FALLBACK_LIBRARY_PATH=\"$LIB_DIR\""
# setup path
BIN_DIR="$RESOURCES_DIR/bin"
if [ ! -d "$BIN_DIR" ]; then
  dbgecho "Directory $BIN_DIR not found!"
  exit 1
fi
export PATH="$BIN_DIR:/usr/X11R6/bin:$PATH"

# GTK: setup fontconfig
ETC_DIR="$RESOURCES_DIR/etc"
if [ -d "$ETC_DIR/fonts" ]; then
  export "FONTCONFIG_PATH=$ETC_DIR/fonts"
fi

# --- find VICE binary ---
# derive emu name from bundle name
if [ "x$PROGRAM" = "x" ]; then
  EMUS="x128,x64dtv,x64sc,xcbm2,xcbm5x0,xpet,xplus4,xvic"
  case "$BUNDLE_NAME" in
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
  VICE*)
    # pick emu name in dialog
    PROGRAM=`osascript -e 'first item of (choose from list {"x128","x64dtv","x64sc","xcbm2","xcbm5x0","xpet","xplus4","xvic"} with title "VICE Emulator" with prompt "Please select an Emulator to run:" default items {"x64sc"})'`
    ;;
  *)
    # invalid bundle name
    osascript -e 'display alert "Invalid Bundle Name! (use: VICE,x128,x64dtv,x64sc,xcbm2,xcbm5x0,xpet,xplus4,xvic" buttons {"Abort"} with icon stop'
    PROGRAM=""
    ;;
  esac
fi

if [ "$PROGRAM" != "" ]; then
  dbgecho "PROGRAM=$PROGRAM"
  PROGRAM_PATH="$BIN_DIR/$PROGRAM"
  dbgecho "PROGRAM_PATH=$PROGRAM_PATH"

  # launch in cmd line without xterm
  dbgecho "CMDLINE ARGS=""$@"
  nohup "$PROGRAM_PATH" "$@" </dev/null >/dev/null 2>&1 &
fi

exit 0

