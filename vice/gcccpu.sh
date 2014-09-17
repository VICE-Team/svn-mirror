#!/bin/sh
# gcccpu.sh helper
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# This script compiles the *cpu.c files with -O0

cpufile=no

for i in $*
do
  case "$i" in
    *cpu.c)
      cpufile=yes
      ;;
  esac
done

newcmdline="gcc"

for i in $*
do
  isopt=no
  case "$i" in
    -O2|-O3)
      isopt=yes
      ;;
  esac

  if test x"$cpufile" = "xyes" -a x"$isopt" = "xyes"; then
    newcmdline="$newcmdline -O0"
  else
    newcmdline="$newcmdline $i"
  fi
done

$newcmdline
