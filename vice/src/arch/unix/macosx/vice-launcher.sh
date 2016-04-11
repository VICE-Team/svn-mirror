#!/bin/bash
# vice-launcher.sh
# written by Christian Vogelgsang <chris@vogelgsang.org>
# runs vice from the nearby application bundle

REALFILE="$0"

# find dir of real file (not the linked ones)
DIR="`dirname \"$REALFILE\"`"
while [ -h "$REALFILE" ]; do
  REALFILE="`readlink \"$REALFILE\"`"
  CURDIR="`dirname \"$REALFILE\"`"
  if [ "${CURDIR:0:1}" = "/" ]; then
    DIR="$CURDIR"
  else
    DIR="$DIR/$CURDIR"
  fi
done

NAME="`basename \"$0\"`"

# find bundle
BUNDLE="$DIR/../VICE.app"
if [ ! -d "$BUNDLE" ]; then
  BUNDLE="$DIR/../$NAME.app"
  if [ ! -d "$BUNDLE" ]; then
    echo "Error: associated bundle '$BUNDLE' not found!"
    exit 1
  fi
fi
BASENAME="`basename \"$BUNDLE\" .app`"

# find launcher
if [ -e "$BUNDLE/Contents/Resources/script" ]; then
  LAUNCHER="$BUNDLE/Contents/Resources/script"
elif [ -e "$BUNDLE/Contents/MacOS/$BASENAME" ]; then
  LAUNCHER="$BUNDLE/Contents/MacOS/$BASENAME"
else
  echo "Error: no launcher script found in '$BUNDLE'!"
  exit 1
fi

# run launcher
export PROGRAM="$NAME"
exec "$LAUNCHER" "$@"

