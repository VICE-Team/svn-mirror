#!/bin/sh
# make-bindist.sh for the AmigaOS SDL ports
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

if [ ! -e src/x64$EXEEXT -o ! -e src/x64dtv$EXEEXT -o ! -e src/x64sc$EXEEXT -o ! -e src/x128$EXEEXT -o ! -e src/xvic$EXEEXT -o ! -e src/xpet$EXEEXT -o ! -e src/xplus4$EXEEXT -o ! -e src/xcbm2$EXEEXT -o ! -e src/c1541$EXEEXT -o ! -e src/petcat$EXEEXT -o ! -e src/cartconv$EXEEXT ]
then
  echo Error: executable file\(s\) not found, do a \"make all\" first
  exit 1
fi

if test x"$HOSTSYSTEM" = "xaros"; then
  AMIGAFLAVOR=$HOSTCPU-AROS-$VICEVERSION
  echo Generating AROS SDL port binary distribution.
else
  if test x"$HOSTSYSTEM" = "xmorphos"; then
    AMIGAFLAVOR=MorphOS-$VICEVERSION
    echo Generating MorphOS SDL port binary distribution.
  else
    if test x"$HOSTSYSTEM" = "xwarpos"; then
      AMIGAFLAVOR=WarpOS-$VICEVERSION
      echo Generating WarpOS SDL port binary distribution.
    else
      if test x"$HOSTSYSTEM" = "xpowerup"; then
        AMIGAFLAVOR=PowerUP-$VICEVERSION
        echo Generating PowerUP SDL port binary distribution.
      else
        if test x"$HOSTCPU" = "xm68k"; then
          AMIGAFLAVOR=AmigaOS3-$VICEVERSION
          echo Generating AmigaOS3 SDL port binary distribution.
        else
          AMIGAFLAVOR=AmigaOS4-$VICEVERSION
          echo Generating AmigaOS4 SDL port binary distribution.
        fi
      fi
    fi
  fi
fi

rm -f -r SDLVICE-$AMIGAFLAVOR SDLVICE-$AMIGAFLAVOR.info
mkdir SDLVICE-$AMIGAFLAVOR
if test x"$HOSTSYSTEM" != "xaros"; then
  $STRIP src/x64$EXEEXT
  $STRIP src/x64dtv$EXEEXT
  $STRIP src/x64sc$EXEEXT
  $STRIP src/x128$EXEEXT
  $STRIP src/xvic$EXEEXT
  $STRIP src/xpet$EXEEXT
  $STRIP src/xplus4$EXEEXT
  $STRIP src/xcbm2$EXEEXT
  $STRIP src/c1541$EXEEXT
  $STRIP src/petcat$EXEEXT
  $STRIP src/cartconv$EXEEXT
else
  $STRIP --strip-unneeded --remove-section .comment src/x64$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/x64dtv$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/x64sc$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/x128$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xvic$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xpet$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xplus4$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/xcbm2$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/c1541$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/petcat$EXEEXT
  $STRIP --strip-unneeded --remove-section .comment src/cartconv$EXEEXT
fi
if test x"$HOSTSYSTEM" = "xmorphos"; then
  cp src/x64$EXEEXT SDLVICE-$AMIGAFLAVOR/x64
  cp src/x64dtv$EXEEXT SDLVICE-$AMIGAFLAVOR/x64dtv
  cp src/x64sc$EXEEXT SDLVICE-$AMIGAFLAVOR/x64sc
  cp src/x128$EXEEXT SDLVICE-$AMIGAFLAVOR/x128
  cp src/xvic$EXEEXT SDLVICE-$AMIGAFLAVOR/xvic
  cp src/xpet$EXEEXT SDLVICE-$AMIGAFLAVOR/xpet
  cp src/xplus4$EXEEXT SDLVICE-$AMIGAFLAVOR/xplus4
  cp src/xcbm2$EXEEXT SDLVICE-$AMIGAFLAVOR/xcbm2
  cp src/c1541$EXEEXT SDLVICE-$AMIGAFLAVOR/c1541
  cp src/petcat$EXEEXT SDLVICE-$AMIGAFLAVOR/petcat
  cp src/cartconv$EXEEXT SDLVICE-$AMIGAFLAVOR/cartconv
else
  cp src/x64$EXEEXT SDLVICE-$AMIGAFLAVOR/x64.exe
  cp src/x64dtv$EXEEXT SDLVICE-$AMIGAFLAVOR/x64dtv.exe
  cp src/x64sc$EXEEXT SDLVICE-$AMIGAFLAVOR/x64sc.exe
  cp src/x128$EXEEXT SDLVICE-$AMIGAFLAVOR/x128.exe
  cp src/xvic$EXEEXT SDLVICE-$AMIGAFLAVOR/xvic.exe
  cp src/xpet$EXEEXT SDLVICE-$AMIGAFLAVOR/xpet.exe
  cp src/xplus4$EXEEXT SDLVICE-$AMIGAFLAVOR/xplus4.exe
  cp src/xcbm2$EXEEXT SDLVICE-$AMIGAFLAVOR/xcbm2.exe
  cp src/c1541$EXEEXT SDLVICE-$AMIGAFLAVOR/c1541.exe
  cp src/petcat$EXEEXT SDLVICE-$AMIGAFLAVOR/petcat.exe
  cp src/cartconv$EXEEXT SDLVICE-$AMIGAFLAVOR/cartconv.exe
fi
cp -a $TOPSCRDIR/data/C128 $TOPSCRDIR/data/C64 SDLVICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/C64DTV $TOPSCRDIR/data/CBM-II SDLVICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/DRIVES $TOPSCRDIR/data/PET SDLVICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/PLUS4 $TOPSCRDIR/data/PRINTER SDLVICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/data/VIC20 $TOPSCRDIR/data/fonts SDLVICE-$AMIGAFLAVOR
cp -a $TOPSCRDIR/doc/html SDLVICE-$AMIGAFLAVOR
cp $TOPSCRDIR/FEEDBACK $TOPSCRDIR/README SDLVICE-$AMIGAFLAVOR
cp $TOPSCRDIR/doc/cartconv.txt SDLVICE-$AMIGAFLAVOR
cp $TOPSCRDIR/doc/ReadmeSDL.txt SDLVICE-$AMIGAFLAVOR
if test x"$HOSTSYSTEM" = "xwarpos"; then
  for i in cartconv.exe c1541.exe petcat.exe x64.exe x64dtv.exe x64sc.exe x128.exe \
           xcbm2.exe xpet.exe xplus4.exe xvic.exe
  do
    elf2exe SDLVICE-$AMIGAFLAVOR/$i SDLVICE-$AMIGAFLAVOR/$i.new
    mv -f SDLVICE-$AMIGAFLAVOR/$i.new SDLVICE-$AMIGAFLAVOR/$i
  done
fi
if test x"$HOSTSYSTEM" = "xmorphos"; then
  cp $TOPSCRDIR/src/arch/amigaos/info-files/morphos/VICE.info SDLVICE-$AMIGAFLAVOR.info
  cp $TOPSCRDIR/src/arch/amigaos/info-files/morphos/x*.info SDLVICE-$AMIGAFLAVOR
  cp SDLVICE-$AMIGAFLAVOR/x64.info SDLVICE-$AMIGAFLAVOR/x64dtv.info
  cp SDLVICE-$AMIGAFLAVOR/x64.info SDLVICE-$AMIGAFLAVOR/x64sc.info
else
  cp $TOPSCRDIR/src/arch/amigaos/info-files/VICE.info SDLVICE-$AMIGAFLAVOR.info
  cp $TOPSCRDIR/src/arch/amigaos/info-files/*.exe.info SDLVICE-$AMIGAFLAVOR
  cp SDLVICE-$AMIGAFLAVOR/x64.exe.info SDLVICE-$AMIGAFLAVOR/x64dtv.exe.info
  cp SDLVICE-$AMIGAFLAVOR/x64.exe.info SDLVICE-$AMIGAFLAVOR/x64sc.exe.info
fi
rm `find SDLVICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find SDLVICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "osx*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "RO*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "amiga*.vkm"`
rm `find SDLVICE-$AMIGAFLAVOR -name "*.vsc"`
rm SDLVICE-$AMIGAFLAVOR/html/texi2html
if test x"$ZIPKIND" = "xzip"; then
  tar cf SDLVICE-$AMIGAFLAVOR.tar SDLVICE-$AMIGAFLAVOR SDLVICE-$AMIGAFLAVOR.info
  gzip SDLVICE-$AMIGAFLAVOR.tar
  rm -f -r SDLVICE-$AMIGAFLAVOR SDLVICE-$AMIGAFLAVOR.info
  echo AMIGA SDL port binary distribution archive generated as SDLVICE-$AMIGAFLAVOR.tar.gz
else
  echo AMIGA SDL port binary destribution directory generated as SDLVICE-$AMIGAFLAVOR
fi
