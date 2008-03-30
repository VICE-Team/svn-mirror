#!/bin/sh
echo Generating AMIGA port binary distribution.

if test x"$4" = "xaros"; then
  AMIGAFLAVOR=AROS-$2
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

rm -f -r ../VICE-$AMIGAFLAVOR ../VICE-$AMIGAFLAVOR.info
mkdir ../VICE-$AMIGAFLAVOR
if test x"$4" != "xaros"; then
  $1 src/x64$6
  $1 src/x128$6
  $1 src/xvic$6
  $1 src/xpet$6
  $1 src/xplus4$6
  $1 src/xcbm2$6
  $1 src/c1541$6
  $1 src/petcat$6
  $1 src/cartconv$6
else
  $1 --strip-unneeded --remove-section .comment src/x64$6
  $1 --strip-unneeded --remove-section .comment src/x128$6
  $1 --strip-unneeded --remove-section .comment src/xvic$6
  $1 --strip-unneeded --remove-section .comment src/xpet$6
  $1 --strip-unneeded --remove-section .comment src/xplus4$6
  $1 --strip-unneeded --remove-section .comment src/xcbm2$6
  $1 --strip-unneeded --remove-section .comment src/c1541$6
  $1 --strip-unneeded --remove-section .comment src/petcat$6
  $1 --strip-unneeded --remove-section .comment src/cartconv$6
fi
cp src/x64$6 ../VICE-$AMIGAFLAVOR/x64.exe
cp src/x128$6 ../VICE-$AMIGAFLAVOR/x128.exe
cp src/xvic$6 ../VICE-$AMIGAFLAVOR/xvic.exe
cp src/xpet$6 ../VICE-$AMIGAFLAVOR/xpet.exe
cp src/xplus4$6 ../VICE-$AMIGAFLAVOR/xplus4.exe
cp src/xcbm2$6 ../VICE-$AMIGAFLAVOR/xcbm2.exe
cp src/c1541$6 ../VICE-$AMIGAFLAVOR/c1541.exe
cp src/petcat$6 ../VICE-$AMIGAFLAVOR/petcat.exe
cp src/cartconv$6 ../VICE-$AMIGAFLAVOR/cartconv.exe
cp -a data/C128 data/C64 data/CBM-II data/DRIVES ../VICE-$AMIGAFLAVOR
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 ../VICE-$AMIGAFLAVOR
cp -a data/fonts ../VICE-$AMIGAFLAVOR
cp -a doc/html ../VICE-$AMIGAFLAVOR
cp FEEDBACK README ../VICE-$AMIGAFLAVOR
cp src/arch/amigaos/README.AMIGA ../VICE-$AMIGAFLAVOR
cp src/arch/amigaos/info-files/VICE.info ../VICE-$AMIGAFLAVOR.info
cp src/arch/amigaos/info-files/*.exe.info ../VICE-$AMIGAFLAVOR
rm `find ../VICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find ../VICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find ../VICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find ../VICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find ../VICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find ../VICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm ../VICE-$AMIGAFLAVOR/html/texi2html
if test x"$5" = "xzip"; then
  cd ..
  tar cf VICE-$AMIGAFLAVOR.tar VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  gzip VICE-$AMIGAFLAVOR.tar
  echo AMIGA port binary distribution archive generated as ../VICE-$AMIGAFLAVOR.tar.gz
else
  echo AMIGA port binary destribution directory generated as ../VICE-$AMIGAFLAVOR
fi
