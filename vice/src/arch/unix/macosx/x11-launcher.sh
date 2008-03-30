#!/bin/bash
#
# x11-launcher.sh
#
# X11 Application Launcher for the VICE Project (www.viceteam.org)
# written by Christian Vogelgsang <chris@vogelgsang.org>
# inspired by Gimp.app of Aaron Voisine <aaron@voisine.org>

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

# --- determine launch environment ---
LAUNCH=cmdline
# finder always appends a -psn_ switch
echo "$1" | grep -e -psn_ > /dev/null
if [ "$?" == "0" ]; then
  LAUNCH=finder
fi
# platypus passes the bundle as in first arg
if [ "$1" = "$BUNDLE_DIR" ]; then
  LAUNCH=platypus
  shift
fi
dbgecho "LAUNCH=$LAUNCH"

# --- find VICE binary ---
BIN_DIR="$RESOURCES_DIR/bin"
if [ ! -d "$BIN_DIR" ]; then
  dbgecho "Directory $BIN_DIR not found!"
  exit 1
fi
PROGRAM=`cd "$BIN_DIR" && ls x* | head -1`
if [ "$PROGRAM" = "" ]; then
  dbgecho "No binary found!"
  exit 1
fi
dbgecho "PROGRAM=$PROGRAM"
PROGRAM_PATH="$BIN_DIR/$PROGRAM"
dbgecho "PROGRAM_PATH=$PROGRAM_PATH"

# --- create a temporary .xinitc if X11 is not running and user has none ---
CREATED_XINITRC=0
# check for X11
ps -wx -ocommand | grep X11.app > /dev/null | grep -v grep > /dev/null
if [ "$?" != "0" ]; then
  # if user has no config then create one
  if [ ! -f "$HOME/.xinitrc" ]; then
    DEFAULT_XINITRC=/usr/X11R6/lib/X11/xinit/xinitrc
    if [ -f "$DEFAULT_XINITRC" ]; then
      # create a one time xinitrc for gimp without an xterm
      sed 's/xterm/# xterm/' $DEFAULT_XINITRC > $HOME/.xinitrc
      CREATED_XINITRC=1
      dbgecho "created user's .xinitrc"
    fi
  fi
fi

# --- launch x11 if not already here and find out our DISPLAY ---
# create temp 
TMP_DIR=/var/tmp/$UID-$$
mkdir -p $TMP_DIR || exit 1
DISPLAY_RUN=$TMP_DIR/display.run
DISPLAY_RESULT=$TMP_DIR/display.result
rm -f $DISPLAY_RESULT
# make display emitter command
echo "#!/bin/sh" > $DISPLAY_RUN
echo "echo \"\$DISPLAY\" > $DISPLAY_RESULT" >> $DISPLAY_RUN
chmod 755 $DISPLAY_RUN
# launch x11 and run display emitter
/usr/bin/open-x11 $DISPLAY_RUN
# wait for command
while [ "$?" == "0" -a ! -f $DISPLAY_RESULT ]; do sleep 1; done
# fetch display
DISPLAY=`cat $DISPLAY_RESULT`
if [ "$DISPLAY" = "" ]; then
  DISPLAY=":0"
fi
dbgecho "DISPLAY=$DISPLAY"
export DISPLAY
# clean up
rm -f $DISPLAY_RUN $DISPLAY_RESULT
rmdir $TMP_DIR

# --- prepare platypus dropped file args for VICE ---
if [ "$LAUNCH" = "platypus" ]; then
  if [ "$1" != "" ]; then
    LAUNCH_FILE="$1"
  fi
fi

# setup dylib path
LIB_DIR="$RESOURCES_DIR/lib"
if [ -d "$LIB_DIR" ]; then
  export DYLD_LIBRARY_PATH="$LIB_DIR"
fi

# setup path
export PATH="$BIN_DIR:/usr/X11R6/bin:$PATH"

# --- now launch the VICE emulator ---
if [ "$LAUNCH" = "cmdline" ]; then
  # launch in cmd line without xterm
  dbgecho "CMDLINE ARGS=" "$@"
  $PROGRAM_PATH "$@"
else
  # use xterm as console
  dbgecho "XTERM LAUNCH_FILE=" "$LAUNCH_FILE"
  if [ "$LAUNCH_FILE" != "" ]; then
    /usr/X11R6/bin/xterm \
      -sb -title "VICE $PROGRAM Console" \
      -e "$PROGRAM_PATH" -autostart "$LAUNCH_FILE"
  else
    /usr/X11R6/bin/xterm \
      -sb -title "VICE $PROGRAM Console" \
      -e "$PROGRAM_PATH"
  fi
fi

# --- clean up ---
# remove temporary .xinitc
if [ $CREATED_XINITRC = 1 ]; then
  rm $HOME/.xinitrc
  dbgecho "removed user's .xinitrc"
fi

exit 0

