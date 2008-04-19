#!/bin/bash
#
# ----- build-allext.sh -----
# build the allext on ppc+i386 macs for VICE
#
# Usage:     build-allext.sh <build-dir> <arch:ppc|i386>
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"

# readline only for ppc
$SHELL "$SCRIPT_DIR/build-readline.sh" "$1" ppc
# hidutils
$SHELL "$SCRIPT_DIR/build-hidutils.sh" "$1" ppc
$SHELL "$SCRIPT_DIR/build-hidutils.sh" "$1" i386
# pcaplibnet
$SHELL "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" ppc
$SHELL "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" i386
# ffmpeg
$SHELL "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" ppc
$SHELL "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" i386
# gtk
$SHELL "$SCRIPT_DIR/build-gtk.sh" "$1" ppc
$SHELL "$SCRIPT_DIR/build-gtk.sh" "$1" i386
echo "========== build-allext: ready =========="