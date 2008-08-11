#!/bin/sh
# make-bindist.sh for the AmigaOS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <host-cpu> <host-system> <zip|nozip> <top-srcdir> <exe-ext>
#                 $1      $2             $3         $4            $5          $6           $7

STRIP=$1
VICEVERSION=$2
HOSTCPU=$3
HOSTSYSTEM=$4
ZIPKIND=$5
TOPSCRDIR=$6
EXEEXT=$7

if [ ! -e src/x64$EXEEXT -o ! -e src/x64dtv$EXEEXT -o ! -e src/x128$EXEEXT -o ! -e src/xvic$EXEEXT -o ! -e src/xpet$EXEEXT -o ! -e src/xplus4$EXEEXT -o ! -e src/xcbm2$EXEEXT -o ! -e src/c1541$EXEEXT -o ! -e src/petcat$EXEEXT -o ! -e src/cartconv$EXEEXT ]
then
  echo Error: executable file\(s\) not found, do a \"make all\" first
  exit 1
fi

if test x"$HOSTSYSTEM" = "xaros"; then
  AMIGAFLAVOR=$HOSTCPU-AROS-$VICEVERSION
  echo Generating AROS port binary distribution.
else
  if test x"$HOSTSYSTEM" = "xmorphos"; then
    AMIGAFLAVOR=MorphOS-$VICEVERSION
    echo Generating MorphOS port binary distribution.
  else
    if test x"$HOSTSYSTEM" = "xwarpos"; then
      AMIGAFLAVOR=WarpOS-$VICEVERSION
      echo Generating WarpOS port binary distribution.
    else
      if test x"$HOSTSYSTEM" = "xpowerup"; then
        AMIGAFLAVOR=PowerUP-$VICEVERSION
        echo Generating PowerUP port binary distribution.
      else
        if test x"$HOSTCPU" = "xm68k"; then
          AMIGAFLAVOR=AmigaOS3-$VICEVERSION
          echo Generating AmigaOS3 port binary distribution.
        else
          AMIGAFLAVOR=AmigaOS4-$VICEVERSION
          echo Generating AmigaOS4 port binary distribution.
        fi
      fi
    fi
  fi
fi

rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
mkdir VICE-$AMIGAFLAVOR
if test x"$HOSTSYSTEM" != "xaros"; then
  $STRIP src/x64$EXEEXT
  $STRIP src/x64dtv$EXEEXT
  $STRIP src/x128$EXEEXT
  $STRIP src/xvic$EXEEXT
  $STRIP src/xpet$EXEEXT
  $STRIP src/xplus4$EXEEXT
  $STRIP src/xcbm2$EXEEXT
  $STRIP src/c1541$EXEEXT
  $STRIP src/petcat$EXEEXT
  $STRIP src/cartconv$EXEEXT
  $STRIP src/arch/amigaos/x64ns$EXEEXT
  $STRIP src/arch/amigaos/x64dtvns$EXEEXT
  $STRIP src/arch/amigaos/x128ns$EXEEXT
  $STRIP src/arch/amigaos/xvicns$EXEEXT
  $STRIP src/arch/amigaos/xpetns$EXEEXT
  $STRIP src/arch/amigaos/xplus4ns$EXEEXT
  $STRIP src/arch/amigaos/xcbm2ns$EXEEXT
else
  $STRIP --strip-unneeded --remove-section .comment src/x64$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/x64dtv$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/x128$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xvic$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xpet$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xplus4$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xcbm2$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/c1541$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/petcat$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/cartconv$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/x64ns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/x64dtvns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/x128ns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/xvicns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/xpetns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/xplus4ns$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/arch/amigaos/xcbm2ns$EXEEXT
fi
if test x"$HOSTSYSTEM" = "xmorphos"; then
  cp src/x64$EXEEXT VICE-$AMIGAFLAVOR/x64
  cp src/x64dtv$EXEEXT VICE-$AMIGAFLAVOR/x64dtv
  cp src/x128$EXEEXT VICE-$AMIGAFLAVOR/x128
  cp src/xvic$EXEEXT VICE-$AMIGAFLAVOR/xvic
  cp src/xpet$EXEEXT VICE-$AMIGAFLAVOR/xpet
  cp src/xplus4$EXEEXT VICE-$AMIGAFLAVOR/xplus4
  cp src/xcbm2$EXEEXT VICE-$AMIGAFLAVOR/xcbm2
  cp src/c1541$EXEEXT VICE-$AMIGAFLAVOR/c1541
  cp src/petcat$EXEEXT VICE-$AMIGAFLAVOR/petcat
  cp src/cartconv$EXEEXT VICE-$AMIGAFLAVOR/cartconv
  cp src/arch/amigaos/x64ns$EXEEXT VICE-$AMIGAFLAVOR/x64\ \(no\ sound\)
  cp src/arch/amigaos/x64dtvns$EXEEXT VICE-$AMIGAFLAVOR/x64dtv\ \(no\ sound\)
  cp src/arch/amigaos/x128ns$EXEEXT VICE-$AMIGAFLAVOR/x128\ \(no\ sound\)
  cp src/arch/amigaos/xvicns$EXEEXT VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\)
  cp src/arch/amigaos/xpetns$EXEEXT VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\)
  cp src/arch/amigaos/xplus4ns$EXEEXT VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\)
  cp src/arch/amigaos/xcbm2ns$EXEEXT VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\)
else
  cp src/x64$EXEEXT VICE-$AMIGAFLAVOR/x64.exe
  cp src/x64dtv$EXEEXT VICE-$AMIGAFLAVOR/x64dtv.exe
  cp src/x128$EXEEXT VICE-$AMIGAFLAVOR/x128.exe
  cp src/xvic$EXEEXT VICE-$AMIGAFLAVOR/xvic.exe
  cp src/xpet$EXEEXT VICE-$AMIGAFLAVOR/xpet.exe
  cp src/xplus4$EXEEXT VICE-$AMIGAFLAVOR/xplus4.exe
  cp src/xcbm2$EXEEXT VICE-$AMIGAFLAVOR/xcbm2.exe
  cp src/c1541$EXEEXT VICE-$AMIGAFLAVOR/c1541.exe
  cp src/petcat$EXEEXT VICE-$AMIGAFLAVOR/petcat.exe
  cp src/cartconv$EXEEXT VICE-$AMIGAFLAVOR/cartconv.exe
  cp src/arch/amigaos/x64ns$EXEEXT VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).exe
  cp src/arch/amigaos/x64dtvns$EXEEXT VICE-$AMIGAFLAVOR/x64dtv\ \(no\ sound\).exe
  cp src/arch/amigaos/x128ns$EXEEXT VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).exe
  cp src/arch/amigaos/xvicns$EXEEXT VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).exe
  cp src/arch/amigaos/xpetns$EXEEXT VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).exe
  cp src/arch/amigaos/xplus4ns$EXEEXT VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).exe
  cp src/arch/amigaos/xcbm2ns$EXEEXT VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).exe
fi
cp -a $TOPSCRDIR/data/C128 $TOPSCRDIR/data/C64 $TOPSCRDIR/data/C64DTV $TOPSCRDIR/data/CBM-II $TOPSCRDIR/data/DRIVES VICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/PET $TOPSCRDIR/data/PLUS4 $TOPSCRDIR/data/PRINTER $TOPSCRDIR/data/VIC20 VICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/fonts VICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/doc/html VICE-$AMIGAFLAVOR
cp $TOPSCRDIR/FEEDBACK $TOPSCRDIR/README $TOPSCRDIR/doc/cartconv.txt VICE-$AMIGAFLAVOR
cp $TOPSCRDIR/src/arch/amigaos/README.AMIGA VICE-$AMIGAFLAVOR
if test x"$HOSTSYSTEM" = "xwarpos"; then
  for i in cartconv.exe c1541.exe petcat.exe x64.exe x64dtv.exe x128.exe xcbm2.exe \
           xpet.exe xplus4.exe xvic.exe
  do
    elf2exe VICE-$AMIGAFLAVOR/$i VICE-$AMIGAFLAVOR/$i.new
    mv -f VICE-$AMIGAFLAVOR/$i.new VICE-$AMIGAFLAVOR/$i
  done
fi
if test x"$HOSTSYSTEM" = "xmorphos"; then
  cp $TOPSCRDIR/src/arch/amigaos/info-files/morphos/VICE.info VICE-$AMIGAFLAVOR.info
  cp $TOPSCRDIR/src/arch/amigaos/info-files/morphos/x*.info VICE-$AMIGAFLAVOR
  cp VICE-$AMIGAFLAVOR/x64.info VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/x64.info VICE-$AMIGAFLAVOR/x64dtv.info
  cp VICE-$AMIGAFLAVOR/x64.info VICE-$AMIGAFLAVOR/x64dtv\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/x128.info VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xvic.info VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xpet.info VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xplus4.info VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).info
  cp VICE-$AMIGAFLAVOR/xcbm2.info VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).info
else
  cp $TOPSCRDIR/src/arch/amigaos/info-files/VICE.info VICE-$AMIGAFLAVOR.info
  cp $TOPSCRDIR/src/arch/amigaos/info-files/*.exe.info VICE-$AMIGAFLAVOR
  cp VICE-$AMIGAFLAVOR/x64.exe.info VICE-$AMIGAFLAVOR/x64\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/x64.exe.info VICE-$AMIGAFLAVOR/x64dtv.exe.info
  cp VICE-$AMIGAFLAVOR/x64.exe.info VICE-$AMIGAFLAVOR/x64dtv\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/x128.exe.info VICE-$AMIGAFLAVOR/x128\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xvic.exe.info VICE-$AMIGAFLAVOR/xvic\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xpet.exe.info VICE-$AMIGAFLAVOR/xpet\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xplus4.exe.info VICE-$AMIGAFLAVOR/xplus4\ \(no\ sound\).exe.info
  cp VICE-$AMIGAFLAVOR/xcbm2.exe.info VICE-$AMIGAFLAVOR/xcbm2\ \(no\ sound\).exe.info
fi
rm `find VICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find VICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "osx*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "RO*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "*.vsc"`
rm VICE-$AMIGAFLAVOR/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  tar cf VICE-$AMIGAFLAVOR.tar VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  gzip VICE-$AMIGAFLAVOR.tar
  rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
  echo AMIGA port binary distribution archive generated as VICE-$AMIGAFLAVOR.tar.gz
else
  echo AMIGA port binary destribution directory generated as VICE-$AMIGAFLAVOR
fi
