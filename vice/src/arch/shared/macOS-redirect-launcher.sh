#!/bin/bash

#
# macOS-redirect-launcher.sh - macOS Application Launcher for the VICE Project
#
# Written by
#  David Hogan <david.q.hogan@gmail.com>
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

SCRIPT_DIR="`dirname \"$0\"`"
RESOURCES_DIR="`cd \"$SCRIPT_DIR/../Resources\" && pwd`"
BUNDLE_DIR="`cd \"$SCRIPT_DIR/../..\" && pwd`"
BUNDLE_NAME="`basename \"$BUNDLE_DIR\" .app`"

MASTER_VICE_APP="$BUNDLE_DIR/../VICE.app"
if [ ! -e $MASTER_VICE_APP ]; then
  osascript -e "display notification \"${BUNDLE_NAME}.app requires that VICE.app be installed in the same location.\" with title \"VICE Installation Error\""
  exit 1
fi

# open the emulator via VICE.app
export PROGRAM=$BUNDLE_NAME
open "$MASTER_VICE_APP" --args "$@"

exit 0
