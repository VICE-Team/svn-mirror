#!/bin/bash
#
# Script to generate .desktop files for the emu binaries on Linux
#
#

show_help()
{
    echo "Usage: `basename $0` <top_srcdir> <top_builddir> <template> <prefix> <version> <binary> [<desc> [<icon>]]"
}


# Entry point, check for -h/--help
if [ -z "$1" ]; then
    show_help
    exit 1
fi
if [ "$1" = "-h" -o "$1" = "--help" ]; then
    show_help
    exit 0
fi


# grab params
TOPSRCDIR="$1"
TOPBUILDDIR="$2"
TEMPLATE="$3"
PREFIX="$4"
VERSION="$5"
BINARY="$6"
COMMENT="$7"
ICON="$8"

if [ -z "$COMMENT" ]; then
    COMMENT="$BINARY"
fi
if [ -z "$ICON" ]; then
    ICON="icon.png"
fi


# check for SVN revision, and if present, add to version number
if [ -e "$TOPSRCDIR/src/svnversion.h" ]; then
    SVNREVISION=`grep -oP 'VICE_SVN_REV_NUMBER\s+\K\d+' "$TOPSRCDIR/src/svnversion.h"`
    VERSION="$VERSION-r$SVNREVISION"
fi

#echo "top_srcdir   : $TOPSRCDIR"
#echo "top_builddir : $TOPBUILDDIR"
#echo "template     : $TEMPLATE"
#echo "prefix       : $PREFIX"
#echo "version      : $VERSION"
#echo "binary       : $BINARY"
#echo "cmment       : $COMMENT"
#echo "icon         : $ICON"

# This is a template 'engine' Harry P!
cat "${TEMPLATE}" | \
    sed -e "s@__PREFIX__@${PREFIX}@g;s@__VICE_EMU_BIN__@${BINARY}@g;s@__ICON__@${ICON}@g;s@__VICE_EMU_COMMENT__@${COMMENT}@g;s@__ICON__@${ICON}@g;s@__VICE_VERSION__@${VERSION}@g;s@__VICE_EMU_NAME__@${BINARY}@g" > "${BINARY}.desktop"

