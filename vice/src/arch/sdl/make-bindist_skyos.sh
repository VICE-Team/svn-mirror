#!/bin/sh
# make-bindist.sh for the SkyOS SDL port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <zip|nozip> <topsrcdir>
#                 $1      $2             $3       $4          $5

STRIP=$1
VERSION=$2
PREFIX=$3
ZIPKIND=$4
TOPSRCDIR=$5

if test x"$PREFIX" != "x/boot/programs/SDLVICE"; then
  echo Error: installation path is not /boot/programs/SDLVICE
  exit 1
fi

if [ ! -e /boot/programs/SDLVICE/bin/x64 -o ! -e /boot/programs/SDLVICE/bin/x64dtv -o ! -e /boot/programs/SDLVICE/bin/x64sc -o ! -e /boot/programs/SDLVICE/bin/x128 -o ! -e /boot/programs/SDLVICE/bin/xvic -o ! -e /boot/programs/SDLVICE/bin/xpet -o ! -e /boot/programs/SDLVICE/bin/xplus4 -o ! -e /boot/programs/SDLVICE/bin/xcbm2 -o ! -e /boot/programs/SDLVICE/bin/c1541 -o ! -e /boot/programs/SDLVICE/bin/petcat -o ! -e /boot/programs/SDLVICE/bin/cartconv ]
then
  echo Error: \"make install\" needs to be done first
  exit 1
fi

echo Generating SkyOS SDL port binary distribution.
rm -f -r SDLVICE-$VERSION
mkdir -p SDLVICE-$VERSION/programs/SDLVICE
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/da/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/da/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/da/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/de/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/de/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/de/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/fr/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/fr/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/fr/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/it/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/it/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/it/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/sv/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/sv/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/sv/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/pl/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/pl/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/pl/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/nl/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/nl/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/nl/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/hu/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/hu/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/hu/LC_MESSAGES
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/lib/locale/tr/LC_MESSAGES
mv /boot/programs/SDLVICE/lib/locale/tr/LC_MESSAGES/vice.* SDLVICE-$VERSION/programs/SDLVICE/lib/locale/tr/LC_MESSAGES
mkdir SDLVICE-$VERSION/programs/SDLVICE/bin
mv /boot/programs/SDLVICE/bin/vsid SDLVICE-$VERSION/programs/SDLVICE/bin
mv /boot/programs/SDLVICE/bin/x64 SDLVICE-$VERSION/programs/SDLVICE/bin/x64.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/x64.app
mv /boot/programs/SDLVICE/bin/x64dtv SDLVICE-$VERSION/programs/SDLVICE/bin/x64dtv.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/x64dtv.app
mv /boot/programs/SDLVICE/bin/x64dtv SDLVICE-$VERSION/programs/SDLVICE/bin/x64sc.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/x64sc.app
mv /boot/programs/SDLVICE/bin/x128 SDLVICE-$VERSION/programs/SDLVICE/bin/x128.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/x128.app
mv /boot/programs/SDLVICE/bin/xvic SDLVICE-$VERSION/programs/SDLVICE/bin/xvic.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/xvic.app
mv /boot/programs/SDLVICE/bin/xpet SDLVICE-$VERSION/programs/SDLVICE/bin/xpet.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/xpet.app
mv /boot/programs/SDLVICE/bin/xplus4 SDLVICE-$VERSION/programs/SDLVICE/bin/xplus4.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/xplus4.app
mv /boot/programs/SDLVICE/bin/xcbm2 SDLVICE-$VERSION/programs/SDLVICE/bin/xcbm2.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/xcbm2.app
mv /boot/programs/SDLVICE/bin/c1541 SDLVICE-$VERSION/programs/SDLVICE/bin/c1541.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/c1541.app
mv /boot/programs/SDLVICE/bin/petcat SDLVICE-$VERSION/programs/SDLVICE/bin/petcat.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/petcat.app
mv /boot/programs/SDLVICE/bin/cartconv SDLVICE-$VERSION/programs/SDLVICE/bin/cartconv.app
$STRIP SDLVICE-$VERSION/programs/SDLVICE/bin/cartconv.app
mv /boot/programs/SDLVICE/lib/vice SDLVICE-$VERSION/programs/SDLVICE/lib
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/share/man/man1
mv /boot/programs/SDLVICE/share/man/man1/* SDLVICE-$VERSION/programs/SDLVICE/share/man/man1/
mkdir -p SDLVICE-$VERSION/programs/SDLVICE/info
mv /boot/programs/SDLVICE/share/info/vice.info* SDLVICE-$VERSION/programs/SDLVICE/share/info
cp -r $TOPSRCDIR/src/arch/unix/skyos/icons SDLVICE-$VERSION/programs/SDLVICE/
rm -f -r /boot/programs/SDLVICE
if test x"$ZIPKIND" = "xzip"; then
  THEDATE=`date +%d-%b-%Y`
  cat >SDLVICE-$VERSION/install.sif <<_END
[GENERAL]
Magic=SkyOS Installation file

[DESCRIPTION]
Product=SDLVICE $VERSION
Detail=VICE, the Versatile Commodore Emulator 
Copyright=The VICE Team
Version=$VERSION
Date=$THEDATE
License=GPL
Category=Emulators

[PACKAGE]
DefaultPath=

[PANELMENU]
/menu="Emulators/SDLVICE"	/name="x64"	/link="/boot/programs/SDLVICE/bin/x64.app"		/icon="/boot/programs/SDLVICE/icons/x64.ico"   
/menu="Emulators/SDLVICE"	/name="x64dtv"	/link="/boot/programs/SDLVICE/bin/x64dtv.app"		/icon="/boot/programs/SDLVICE/icons/x64dtv.ico"   
/menu="Emulators/SDLVICE"	/name="x64sc"	/link="/boot/programs/SDLVICE/bin/x64sc.app"		/icon="/boot/programs/SDLVICE/icons/x64sc.ico"   
/menu="Emulators/SDLVICE"	/name="x128"	/link="/boot/programs/SDLVICE/bin/x128.app"	/icon="/boot/programs/SDLVICE/icons/x128.ico"   
/menu="Emulators/SDLVICE"	/name="xcbm2"	/link="/boot/programs/SDLVICE/bin/xcbm2.app"	/icon="/boot/programs/SDLVICE/icons/xcbm2.ico"   
/menu="Emulators/SDLVICE"	/name="xpet"	/link="/boot/programs/SDLVICE/bin/xpet.app"	/icon="/boot/programs/SDLVICE/icons/xpet.ico"   
/menu="Emulators/SDLVICE"	/name="xplus4"	/link="/boot/programs/SDLVICE/bin/xplus4.app"	/icon="/boot/programs/SDLVICE/icons/xplus4.ico"   
/menu="Emulators/SDLVICE"	/name="xvic"	/link="/boot/programs/SDLVICE/bin/xvic.app"	/icon="/boot/programs/SDLVICE/icons/xvic.ico"   

[FILEICONS]
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/x64.app"	/icon="/boot/programs/SDLVICE/icons/x64.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/x64dtv.app"	/icon="/boot/programs/SDLVICE/icons/x64dtv.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/x64sc.app"	/icon="/boot/programs/SDLVICE/icons/x64sc.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/x128.app"	/icon="/boot/programs/SDLVICE/icons/x128.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/xcbm2.app"	/icon="/boot/programs/SDLVICE/icons/xcbm2.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/xpet.app"	/icon="/boot/programs/SDLVICE/icons/xpet.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/xplus4.app"	/icon="/boot/programs/SDLVICE/icons/xplus4.ico"
/file="\$INSTALL_ROOT/programs/SDLVICE/bin/xvic.app"	/icon="/boot/programs/SDLVICE/icons/xvic.ico"

[SS_DESCRIPTION]
VICE is a program that runs on a Unix, MS-DOS, Win32, OS/2, Acorn RISC OS, BeOS, QNX, SkyOS, Amiga, GP2X or Mac OS X machine and executes programs intended for the old 8-bit computers. The current version emulates the C64, the C128, the VIC20, almost all PET models, the PLUS4 and the CBM-II (aka C610).
[/SS_DESCRIPTION]

[END]
_END

  cd SDLVICE-$VERSION
  tar cf ../SDLVICE-$VERSION.tar *
  cd ..
  gzip SDLVICE-$VERSION.tar
  mv SDLVICE-$VERSION.tar.gz SDLVICE-$VERSION.pkg
  rm -f -r SDLVICE-$VERSION
  echo SkyOS SDL port binary package generated as SDLVICE-$VERSION.pkg
else
  echo SkyOS SDL port binary distribution directory generated as SDLVICE-$VERSION
fi
