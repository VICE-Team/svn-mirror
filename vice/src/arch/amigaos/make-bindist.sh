#!/bin/sh
# make-bindist.sh for the AmigaOS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <host-cpu> <host-system> <zip|nozip> <top-srcdir> <exe-ext>
#                 $1      $2             $3         $4            $5          $6           $7

echo Generating AMIGA port binary distribution.

if test x"$4" = "xaros"; then
  AMIGAFLAVOR=$3-AROS-$2
else
  if test x"$4" = "xmorphos"; then
    AMIGAFLAVOR=MorphOS-$2
  else
    if test x"$3" = "xm68k"; then
      AMIGAFLAVOR=AmigaOS3-$2
    else
      AMIGAFLAVOR=AmigaOS4-$2
    fi
  fi
fi

rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
mkdir VICE-$AMIGAFLAVOR
if test x"$4" != "xaros"; then
  $1 src/x64$7
  $1 src/x128$7
  $1 src/xvic$7
  $1 src/xpet$7
  $1 src/xplus4$7
  $1 src/xcbm2$7
  $1 src/c1541$7
  $1 src/petcat$7
  $1 src/cartconv$7
else
  $1 --strip-unneeded --remove-section .comment src/x64$7
  $1 --strip-unneeded --remove-section .comment src/x128$7
  $1 --strip-unneeded --remove-section .comment src/xvic$7
  $1 --strip-unneeded --remove-section .comment src/xpet$7
  $1 --strip-unneeded --remove-section .comment src/xplus4$7
  $1 --strip-unneeded --remove-section .comment src/xcbm2$7
  $1 --strip-unneeded --remove-section .comment src/c1541$7
  $1 --strip-unneeded --remove-section .comment src/petcat$7
  $1 --strip-unneeded --remove-section .comment src/cartconv$7
fi
cp src/x64$7 VICE-$AMIGAFLAVOR/x64.exe
cp src/x128$7 VICE-$AMIGAFLAVOR/x128.exe
cp src/xvic$7 VICE-$AMIGAFLAVOR/xvic.exe
cp src/xpet$7 VICE-$AMIGAFLAVOR/xpet.exe
cp src/xplus4$7 VICE-$AMIGAFLAVOR/xplus4.exe
cp src/xcbm2$7 VICE-$AMIGAFLAVOR/xcbm2.exe
cp src/c1541$7 VICE-$AMIGAFLAVOR/c1541.exe
cp src/petcat$7 VICE-$AMIGAFLAVOR/petcat.exe
cp src/cartconv$7 VICE-$AMIGAFLAVOR/cartconv.exe
cp -a $6/data/C128 $6/data/C64 $6/data/CBM-II $6/data/DRIVES VICE-$AMIGAFLAVOR
cp -a $6/data/PET $6/data/PLUS4 $6/data/PRINTER $6/data/VIC20 VICE-$AMIGAFLAVOR
cp -a $6/data/fonts VICE-$AMIGAFLAVOR
cp -a $6/doc/html VICE-$AMIGAFLAVOR
cp $6/FEEDBACK $6/README VICE-$AMIGAFLAVOR
cp $6/src/arch/amigaos/README.AMIGA VICE-$AMIGAFLAVOR
cp $6/src/arch/amigaos/info-files/VICE.info VICE-$AMIGAFLAVOR.info
cp $6/src/arch/amigaos/info-files/*.exe.info VICE-$AMIGAFLAVOR
rm `find VICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find VICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm VICE-$AMIGAFLAVOR/html/texi2html
if test x"$5" = "xzip"; then
  tar cf VICE-$AMIGAFLAVOR.tar VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  gzip VICE-$AMIGAFLAVOR.tar
  rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  echo AMIGA port binary distribution archive generated as VICE-$AMIGAFLAVOR.tar.gz
else
  echo AMIGA port binary destribution directory generated as VICE-$AMIGAFLAVOR
fi
