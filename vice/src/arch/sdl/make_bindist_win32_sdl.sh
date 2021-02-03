#!/bin/sh
# vim: set et ts=2 sw=2 sts=2 syntax=bash:

# Per UI/port declarations of bindist subdirectories
BINDIST_EXE_DIR="."
BINDIST_DLL_DIR="."
BINDIST_DOC_DIR="doc"
BINDIST_DATA_DIR="."


# Find required DLLs (for x64sc.exe)
#
find_dlls()
{
  echo "[debug] SDL2: finding DLLs"
  if [[ "$CROSS" != "yes" && "$CROSS" != "true" ]]; then
    # Native Windows compile, assuming msys2 here
    #
    # Grab DLL's with 'mingw32/bin' or 'mingw64/bin' in their path, then
    # translate the Windows-style paths to Unix-style paths
    BINDIST_DLLS=`ntldd -R $BINDIST_DIR/x64sc.exe | gawk '/\\\\mingw(32|64)\\\\/{ print $3; }' | cygpath -u -f -`
  fi
}



# Clean out unwanted files from $1
#
# Careful: here be dragons, perhaps!
#
clean_emu_data_dir()
{
  # Guard against calling rm on root directory:
  if [[ -z "$1" || "$1" = "/" ]]; then
    echo "$(basename $0): error: invalid directory given."
    exit 1
  fi

  if [ "$VERBOSE" = "yes" ]; then
    echo ".. cleaning '$1'"
    rm $1/Makefile* 2>/dev/null
    rm $1/gtk3*.vkm 2>/dev/null
  fi
}
