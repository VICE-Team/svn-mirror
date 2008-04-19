#!/bin/bash
#
# ----- build-readline.sh -----
# build the readline on ppc/macs for VICE
#
# Usage:     build-readline.sh <build-dir> <arch:ppc|i386>
#
# written by Christian Vogelgsang <chris@vogelgsang.org>
#
# call this function only for ppc!

# get build tools
SCRIPT_DIR="`dirname \"$0\"`"
. "$SCRIPT_DIR/build-inc.sh"

# parse args
parse_args "$1" "$2"

# setup compiler environment
set_compiler_env

echo "===== readline build for $ARCH ====="

configure_make_install readline-5.2.tar.gz readline-5.2 lib/libreadline.a install \
                       "http://tiswww.case.edu/~chet/readline/rltop.html" \
                       "--disable-shared"

echo "===== readline ready for $ARCH ====="
