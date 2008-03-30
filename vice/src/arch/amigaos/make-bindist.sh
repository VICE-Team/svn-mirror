#!/bin/sh
# make-bindist.sh for the AmigaOS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <host-cpu> <host-system> <zip|nozip> <top-srcdir> <exe-ext>
#                 $1      $2             $3         $4            $5          $6           $7

if [ ! -e src/x64$7 -o ! -e src/x128$7 -o ! -e src/xvic$7 -o ! -e src/xpet$7 -o ! -e src/xplus4$7 -o ! -e src/xcbm2$7 -o ! -e src/c1541$7 -o ! -e src/petcat$7 -o ! -e src/cartconv$7 ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

if test x"$4" = "xaros"; then
  AMIGAFLAVOR=$3-AROS-$2
  echo Generating AROS port binary distribution.
else
  if test x"$4" = "xmorphos"; then
    AMIGAFLAVOR=MorphOS-$2
    echo Generating MorphOS port binary distribution.
  else
    if test x"$4" = "xwarpos"; then
      AMIGAFLAVOR=WarpOS-$2
      echo Generating WarpOS port binary distribution.
    else
      if test x"$4" = "xpowerup"; then
        AMIGAFLAVOR=PowerUP-$2
        echo Generating PowerUP port binary distribution.
      else
        if test x"$3" = "xm68k"; then
          AMIGAFLAVOR=AmigaOS3-$2
          echo Generating AmigaOS3 port binary distribution.
        else
          AMIGAFLAVOR=AmigaOS4-$2
          echo Generating AmigaOS4 port binary distribution.
        fi
      fi
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
  $1 src/arch/amigaos/x64ns$7
  $1 src/arch/amigaos/x128ns$7
  $1 src/arch/amigaos/xvicns$7
  $1 src/arch/amigaos/xpetns$7
  $1 src/arch/amigaos/xplus4ns$7
  $1 src/arch/amigaos/xcbm2ns$7
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
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/x64ns$7
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/x128ns$7
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/xvicns$7
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/xpetns$7
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/xplus4ns$7
  $1 --strip-unneeded --remove-section .comment src/arch/amigaos/xcbm2ns$7
fi
if test x"$4" = "xmorphos"; then
  cp src/x64$7 VICE-$AMIGAFLAVOR/x64
  cp src/x128$7 VICE-$AMIGAFLAVOR/x128
  cp src/xvic$7 VICE-$AMIGAFLAVOR/xvic
  cp src/xpet$7 VICE-$AMIGAFLAVOR/xpet
  cp src/xplus4$7 VICE-$AMIGAFLAVOR/xplus4
  cp src/xcbm2$7 VICE-$AMIGAFLAVOR/xcbm2
  cp src/c1541$7 VICE-$AMIGAFLAVOR/c1541
  cp src/petcat$7 VICE-$AMIGAFLAVOR/petcat
  cp src/cartconv$7 VICE-$AMIGAFLAVOR/cartconv
  cp src/arch/amigaos/x64ns$7 VICE-$AMIGAFLAVOR/x64\ \(no\ sound\)
  cp src/arch/amigaos/x128ns$7 VICE-$AMIGAFLAVOR/x128\ \(no\ sound\)
  cp src/arch/amigaos/xvicns$7 VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\)
  cp src/arch/amigaos/xpetns$7 VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\)
  cp src/arch/amigaos/xplus4ns$7 VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\)
  cp src/arch/amigaos/xcbm2ns$7 VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\)
else
  cp src/x64$7 VICE-$AMIGAFLAVOR/x64.exe
  cp src/x128$7 VICE-$AMIGAFLAVOR/x128.exe
  cp src/xvic$7 VICE-$AMIGAFLAVOR/xvic.exe
  cp src/xpet$7 VICE-$AMIGAFLAVOR/xpet.exe
  cp src/xplus4$7 VICE-$AMIGAFLAVOR/xplus4.exe
  cp src/xcbm2$7 VICE-$AMIGAFLAVOR/xcbm2.exe
  cp src/c1541$7 VICE-$AMIGAFLAVOR/c1541.exe
  cp src/petcat$7 VICE-$AMIGAFLAVOR/petcat.exe
  cp src/cartconv$7 VICE-$AMIGAFLAVOR/cartconv.exe
  cp src/arch/amigaos/x64ns$7 VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).exe
  cp src/arch/amigaos/x128ns$7 VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).exe
  cp src/arch/amigaos/xvicns$7 VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).exe
  cp src/arch/amigaos/xpetns$7 VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).exe
  cp src/arch/amigaos/xplus4ns$7 VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).exe
  cp src/arch/amigaos/xcbm2ns$7 VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).exe
fi
cp -a $6/data/C128 $6/data/C64 $6/data/CBM-II $6/data/DRIVES VICE-$AMIGAFLAVOR
cp -a $6/data/PET $6/data/PLUS4 $6/data/PRINTER $6/data/VIC20 VICE-$AMIGAFLAVOR
cp -a $6/data/fonts VICE-$AMIGAFLAVOR
cp -a $6/doc/html VICE-$AMIGAFLAVOR
cp $6/FEEDBACK $6/README $6/doc/cartconv.txt VICE-$AMIGAFLAVOR
cp $6/src/arch/amigaos/README.AMIGA VICE-$AMIGAFLAVOR
if test x"$4" = "xwarpos"; then
  for i in cartconv.exe c1541.exe petcat.exe x64.exe x128.exe xcbm2.exe \
           xpet.exe xplus4.exe xvic.exe
  do
    elf2exe VICE-$AMIGAFLAVOR/$i VICE-$AMIGAFLAVOR/$i.new
    mv -f VICE-$AMIGAFLAVOR/$i.new VICE-$AMIGAFLAVOR/$i
  done
fi
if test x"$4" = "xmorphos"; then
  cp $6/src/arch/amigaos/info-files/morphos/VICE.info VICE-$AMIGAFLAVOR.info
  cp $6/src/arch/amigaos/info-files/morphos/x*.info VICE-$AMIGAFLAVOR
  cp VICE-$AMIGAFLAVOR/x64.info VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/x128.info VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xvic.info VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xpet.info VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xplus4.info VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xcbm2.info VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).info
else
  cp $6/src/arch/amigaos/info-files/VICE.info VICE-$AMIGAFLAVOR.info
  cp $6/src/arch/amigaos/info-files/*.exe.info VICE-$AMIGAFLAVOR
  cp VICE-$AMIGAFLAVOR/x64.exe.info VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/x128.exe.info VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xvic.exe.info VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xpet.exe.info VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xplus4.exe.info VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xcbm2.exe.info VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).exe.info
fi
rm `find VICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find VICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "RO*.vkm"`
rm VICE-$AMIGAFLAVOR/html/texi2html
if test x"$5" = "xzip"; then
  tar cf VICE-$AMIGAFLAVOR.tar VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  gzip VICE-$AMIGAFLAVOR.tar
  rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  echo AMIGA port binary distribution archive generated as VICE-$AMIGAFLAVOR.tar.gz
else
  echo AMIGA port binary destribution directory generated as VICE-$AMIGAFLAVOR
fi
