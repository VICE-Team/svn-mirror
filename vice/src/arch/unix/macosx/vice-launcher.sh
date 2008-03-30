#!/bin/bash
# vice-launcher.sh
# written by Christian Vogelgsang <chris@vogelgsang.org>
# runs vice from the nearby application bundle

NAME="`basename \"$0\"`"
DIR="`dirname \"$0\"`"

# find bundle
BUNDLE="$DIR/../VICE.app"
if [ ! -d "$BUNDLE" ]; then
  echo "Error: associated bundle '$BUNDLE' not found!"
  exit 1
fi

# find launcher
if [ -e "$BUNDLE/Contents/Resources/script" ]; then
  LAUNCHER="$BUNDLE/Contents/Resources/script"
elif [ -e "$BUNDLE/Contents/MacOS/VICE" ]; then
  LAUNCHER="$BUNDLE/Contents/MacOS/VICE"
else
  echo "Error: no launcher script found in '$BUNDLE'!"
  exit 1
fi

# run launcher
export PROGRAM="$NAME"
exec "$LAUNCHER" "$@"

