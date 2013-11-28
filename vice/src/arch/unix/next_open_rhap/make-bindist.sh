#!/bin/sh
# make-bindist.sh for the OPENSTEP port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <--enable-arch> <zip|nozip> <x64sc-included> <platform> <topsrcdir> <make-command>
#                 $1      $2             $3       $4              $5          $6               $7         $8          $9

STRIP=$1
VICEVERSION=$2
PREFIX=$3
ENABLEARCH=$4
ZIPKIND=$5
X64SC=$6
OPERATINGSYSTEM=$7
TOPSRCDIR=$8
MAKECOMMAND=$9

if test x"$PREFIX" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 xscpu64 x64dtv $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i ]
  then
    echo Error: \"make\" needs to be done first
    exit 1
  fi
done

curdir=`pwd`
if [ ! -e VICE-$VICEVERSION ]
then
  echo "Error: no install directory is present, do the following:"
  echo "gnumake -e prefix=$curdir/VICE-$VICEVERSION/usr/local  VICEDIR=$curdir/VICE-$VICEVERSION/usr/local/lib/vice install"
  exit 1
fi

if test x"$OPERATINGSYSTEM" = "xopenstep"; then
  OSNAME="OpenStep"
  OSID="OS"
fi

if test x"$OPERATINGSYSTEM" = "xnextstep"; then
  OSNAME="NextStep"
  OSID="NS"
fi

if test x"$OPERATINGSYSTEM" = "xrhapsody"; then
  OSNAME="Rhapsody"
  OSID="RH"
fi

echo Generating $OSNAME port binary distribution.
for i in $EXECUTABLES
do
  i586=`file VICE-$VICEVERSION/usr/local/bin/$i | grep i586`
  if test x"$i586" != "x"; then
    $TOPSRCDIR/src/arch/unix/next_open_rhap/nextfix.sh VICE-$VICEVERSION/usr/local/bin/$1
  fi
done
if test x"$ZIPKIND" = "xzip"; then
  if test x"$OSID" = "xRH"; then
    package $curdir/VICE-$VICEVERSION/usr/local $TOPSRCDIR/src/arch/unix/next_open_rhap/vice.info
  else
    /NextAdmin/Installer.app/package $curdir/VICE-$VICEVERSION/usr/local $TOPSRCDIR/src/arch/unix/next_open_rhap/vice.info
  fi
  file >/tmp/vice.tmp VICE-$VICEVERSION/usr/local/bin/x64
  i386_found=`fgrep 86 /tmp/vice.tmp`
  m68k_found=`fgrep m68k /tmp/vice.tmp`
  sparc_found=`fgrep sparc /tmp/vice.tmp`
  hppa_found=`fgrep hppa /tmp/vice.tmp`
  ppc_found=`fgrep ppc /tmp/vice.tmp`
  powerpc_found=`fgrep powerpc /tmp/vice.tmp`

  PLATFORMS=""

  if test x"$m68k_found" != "x"; then
    PLATFORMS="N"
  fi

  if test x"$ppc_found" != "x" -o x"$powerpc_found" != "x"; then
    PLATFORMS="$PLATFORMS""P"
  fi

  if test x"$i386_found" != "x"; then
    PLATFORMS="$PLATFORMS""I"
  fi

  if test x"$hppa_found" != "x"; then
    PLATFORMS="$PLATFORMS""H"
  fi

  if test x"$sparc_found" != "x"; then
    PLATFORMS="$PLATFORMS""S"
  fi

  tar -cvf - vice.pkg | gzip -9c > VICE-$VICEVERSION-$OSID-$PLATFORMS.tar.gz

  rm -f -r VICE-$VICEVERSION vice.pkg

  echo $OSNAME port binary package generated as VICE-$VICEVERSION-$OSID-$PLATFORMS.tar.gz
else
  echo $OSNAME port binary distribution directory generated as VICE-$VICEVERSION
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
