#!/bin/sh
# make-bindist.sh for the SkyOS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <zip|nozip> <topsrcdir> <make-command>
#                 $1      $2             $3       $4          $5          $6

STRIP=$1
VERSION=$2
PREFIX=$3
ZIPKIND=$4
TOPSRCDIR=$5
MAKECOMMAND=$6

if test x"$PREFIX" != "x/boot/programs/VICE"; then
  echo Error: installation path is not /boot/programs/VICE
  exit 1
fi

if [ ! -e src/x64 -o ! -e src/x128 -o ! -e src/xvic -o ! -e src/xpet -o ! -e src/xplus4 -o ! -e src/xcbm2 -o ! -e src/c1541 -o ! -e src/petcat -o ! -e src/cartconv ]
then
  echo Error: \"make\" needs to be done first
  exit 1
fi

echo Generating SkyOS port binary distribution.
rm -f -r VICE-$VERSION
curdir=`pwd`
$MAKECOMMAND prefix=$curdir/VICE-$VERSION/programs/VICE VICEDIR=$curdir/VICE-$VERSION/programs/VICE/lib/vice install
mv VICE-$VERSION/programs/VICE/bin/x64 VICE-$VERSION/programs/VICE/bin/x64.app
$STRIP VICE-$VERSION/programs/VICE/bin/x64.app
mv VICE-$VERSION/programs/VICE/bin/x128 VICE-$VERSION/programs/VICE/bin/x128.app
$STRIP VICE-$VERSION/programs/VICE/bin/x128.app
mv VICE-$VERSION/programs/VICE/bin/xvic VICE-$VERSION/programs/VICE/bin/xvic.app
$STRIP VICE-$VERSION/programs/VICE/bin/xvic.app
mv VICE-$VERSION/programs/VICE/bin/xpet VICE-$VERSION/programs/VICE/bin/xpet.app
$STRIP VICE-$VERSION/programs/VICE/bin/xpet.app
mv VICE-$VERSION/programs/VICE/bin/xplus4 VICE-$VERSION/programs/VICE/bin/xplus4.app
$STRIP VICE-$VERSION/programs/VICE/bin/xplus4.app
mv VICE-$VERSION/programs/VICE/bin/xcbm2 VICE-$VERSION/programs/VICE/bin/xcbm2.app
$STRIP VICE-$VERSION/programs/VICE/bin/xcbm2.app
mv VICE-$VERSION/programs/VICE/bin/c1541 VICE-$VERSION/programs/VICE/bin/c1541.app
$STRIP VICE-$VERSION/programs/VICE/bin/c1541.app
mv VICE-$VERSION/programs/VICE/bin/petcat VICE-$VERSION/programs/VICE/bin/petcat.app
$STRIP VICE-$VERSION/programs/VICE/bin/petcat.app
mv VICE-$VERSION/programs/VICE/bin/cartconv VICE-$VERSION/programs/VICE/bin/cartconv.app
$STRIP VICE-$VERSION/programs/VICE/bin/cartconv.app
rm `find VICE-$VERSION -name "amiga_*.vkm"`
rm `find VICE-$VERSION -name "beos_*.vkm"`
rm `find VICE-$VERSION -name "dos_*.vkm"`
rm `find VICE-$VERSION -name "os2*.vkm"`
rm `find VICE-$VERSION -name "osx*.vkm"`
rm `find VICE-$VERSION -name "win_*.vkm"`
rm `find VICE-$VERSION -name "RO*.vkm"`
cp -r $TOPSRCDIR/src/arch/unix/skyos/icons VICE-$VERSION/programs/VICE/
if test x"$ZIPKIND" = "xzip"; then
  THEDATE=`date +%d-%b-%Y`
  cat >VICE-$VERSION/install.sif <<_END
[GENERAL]
Magic=SkyOS Installation file

[DESCRIPTION]
Product=VICE $VERSION
Detail=VICE, the Versatile Commodore Emulator 
Copyright=The VICE Team
Version=$VERSION
Date=$THEDATE
License=GPL
Category=Emulators

[PACKAGE]
DefaultPath=

[PANELMENU]
/menu="Emulators/VICE"	/name="x64"	/link="/boot/programs/VICE/bin/x64.app"		/icon="/boot/programs/VICE/icons/x64.ico"   
/menu="Emulators/VICE"	/name="x128"	/link="/boot/programs/VICE/bin/x128.app"	/icon="/boot/programs/VICE/icons/x128.ico"   
/menu="Emulators/VICE"	/name="xcbm2"	/link="/boot/programs/VICE/bin/xcbm2.app"	/icon="/boot/programs/VICE/icons/xcbm2.ico"   
/menu="Emulators/VICE"	/name="xpet"	/link="/boot/programs/VICE/bin/xpet.app"	/icon="/boot/programs/VICE/icons/xpet.ico"   
/menu="Emulators/VICE"	/name="xplus4"	/link="/boot/programs/VICE/bin/xplus4.app"	/icon="/boot/programs/VICE/icons/xplus4.ico"   
/menu="Emulators/VICE"	/name="xvic"	/link="/boot/programs/VICE/bin/xvic.app"	/icon="/boot/programs/VICE/icons/xvic.ico"   

[FILEICONS]
/file="\$INSTALL_ROOT/programs/VICE/bin/x64.app"	/icon="/boot/programs/VICE/icons/x64.ico"
/file="\$INSTALL_ROOT/programs/VICE/bin/x128.app"	/icon="/boot/programs/VICE/icons/x128.ico"
/file="\$INSTALL_ROOT/programs/VICE/bin/xcbm2.app"	/icon="/boot/programs/VICE/icons/xcbm2.ico"
/file="\$INSTALL_ROOT/programs/VICE/bin/xpet.app"	/icon="/boot/programs/VICE/icons/xpet.ico"
/file="\$INSTALL_ROOT/programs/VICE/bin/xplus4.app"	/icon="/boot/programs/VICE/icons/xplus4.ico"
/file="\$INSTALL_ROOT/programs/VICE/bin/xvic.app"	/icon="/boot/programs/VICE/icons/xvic.ico"

[SS_DESCRIPTION]
VICE is a program that runs on a Unix, MS-DOS, Win32, OS/2, Acorn RISC OS, BeOS, QNX, SkyOS, Amiga, GP2X or Mac OS X machine and executes programs intended for the old 8-bit computers. The current version emulates the C64, the C128, the VIC20, almost all PET models, the PLUS4 and the CBM-II (aka C610).
[/SS_DESCRIPTION]

[END]
_END

  cd VICE-$VERSION
  tar cf ../VICE-$VERSION.tar *
  cd ..
  gzip VICE-$VERSION.tar
  mv VICE-$VERSION.tar.gz VICE-$VERSION.pkg
  rm -f -r VICE-$VERSION
  echo SkyOS port binary package generated as VICE-$VERSION.pkg
else
  echo SkyOS port binary distribution directory generated as VICE-$VERSION
fi
