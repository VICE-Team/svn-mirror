#!/bin/bash
#
# ----- build-allext.sh -----
# build the allext on ppc+i386 macs for VICE
#
# Usage:     build-allext.sh <build-dir> [force build]
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"

run () {
  echo "--- $@ ---"
  $SHELL "$@"
  if [ $? != 0 ]; then
    echo "*** FAILED: $@"
    exit 1
  fi
}

# hidutils
run "$SCRIPT_DIR/build-hidutils.sh" "$1" ppc 10.4 gcc40 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" i386 10.4 gcc40 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" ppc 10.5 gcc40 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" i386 10.5 gcc40 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" i386 10.6 gcc42 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" x86_64 10.6 gcc42 $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" i386 10.6 clang $2
run "$SCRIPT_DIR/build-hidutils.sh" "$1" x86_64 10.6 clang $2

# pcaplibnet
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" ppc 10.4 gcc40 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" i386 10.4 gcc40 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" ppc 10.5 gcc40 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" i386 10.5 gcc40 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" i386 10.6 gcc42 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" x86_64 10.6 gcc42 $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" i386 10.6 clang $2
run "$SCRIPT_DIR/build-pcaplibnet.sh" "$1" x86_64 10.6 clang $2

# ffmpeg
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" ppc 10.4 gcc40 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" i386 10.4 gcc40 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" ppc 10.5 gcc40 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" i386 10.5 gcc40 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" i386 10.6 gcc42 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" x86_64 10.6 gcc42 $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" i386 10.6 clang $2
run "$SCRIPT_DIR/build-ffmpeglame.sh" "$1" x86_64 10.6 clang $2

# gtk
run "$SCRIPT_DIR/build-gtk.sh" "$1" ppc 10.4 gcc40 $2
run "$SCRIPT_DIR/build-gtk.sh" "$1" i386 10.4 gcc40 $2

# SDLmain
run "$SCRIPT_DIR/build-sdlmain.sh" "$1" ppc 10.4 gcc40 $2
run "$SCRIPT_DIR/build-sdlmain.sh" "$1" i386 10.4 gcc40 $2

echo "========== build-allext: ready =========="