#!/bin/sh
# make-bindist.sh for the OPENSERVER, UNIXWARE & SOLARIS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-version> <prefix> <cross> <zip|nozip> <system> <infodir> <mandir> <topsrcdir>
#                 $1      $2             $3       $4      $5          $6       $7        $8       $9

STRIP=$1
VICEVERSION=$2
PREFIX=$3
CROSS=$4
ZIPKIND=$5
SYSTEM=$6
INFODIR=$7
MANDIR=$8
TOPSRCDIR=$9

if test x"$PREFIX" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$SYSTEM" = "xsco7"; then
  PLATFORM="UNIXWARE 7.x"
fi

if test x"$SYSTEM" = "xsco6"; then
  PLATFORM="OPENSERVER 6.x"
fi

if test x"$SYSTEM" = "xsco5"; then
  PLATFORM="OPENSERVER 5.x"
fi

if test x"$SYSTEM" = "xsol"; then
  PLATFORM="SOLARIS"
fi

if test x"$CROSS" = "xtrue"; then
  echo Error: make bindist for $PLATFORM can only be done on $PLATFORM
  exit 1
fi

if [ ! -e /usr/local/bin/x64 -o ! -e /usr/local/bin/x128 -o ! -e /usr/local/bin/xvic -o ! -e /usr/local/bin/xpet -o ! -e /usr/local/bin/xplus4 -o ! -e /usr/local/bin/xcbm2 -o ! -e /usr/local/bin/c1541 -o ! -e /usr/local/bin/petcat -o ! -e /usr/local/bin/cartconv ]
then
  echo Error: \"make install\" needs to be done first
  exit 1
fi

echo Generating $PLATFORM port binary distribution.
rm -f -r VICE-$VICEVERSION
mkdir VICE-$VICEVERSION
mkdir -p VICE-$VICEVERSION/usr/local
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/de/LC_MESSAGES
mv /usr/local/lib/locale/de/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/de/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/fr/LC_MESSAGES
mv /usr/local/lib/locale/fr/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/fr/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/it/LC_MESSAGES
mv /usr/local/lib/locale/it/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/it/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/sv/LC_MESSAGES
mv /usr/local/lib/locale/sv/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/sv/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/pl/LC_MESSAGES
mv /usr/local/lib/locale/pl/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/pl/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/nl/LC_MESSAGES
mv /usr/local/lib/locale/nl/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/nl/LC_MESSAGES
mkdir -p VICE-$VICEVERSION/usr/local/lib/locale/hu/LC_MESSAGES
mv /usr/local/lib/locale/hu/LC_MESSAGES/vice.* VICE-$VICEVERSION/usr/local/lib/locale/hu/LC_MESSAGES
mkdir VICE-$VICEVERSION/usr/local/bin
mv /usr/local/bin/vsid VICE-$VICEVERSION/usr/local/bin
mv /usr/local/bin/x64 VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/x64
mv /usr/local/bin/x128 VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/x128
mv /usr/local/bin/xvic VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/xvic
mv /usr/local/bin/xpet VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/xpet
mv /usr/local/bin/xplus4 VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/xplus4
mv /usr/local/bin/xcbm2 VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/xcbm2
mv /usr/local/bin/c1541 VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/c1541
mv /usr/local/bin/petcat VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/petcat
mv /usr/local/bin/cartconv VICE-$VICEVERSION/usr/local/bin
$STRIP VICE-$VICEVERSION/usr/local/bin/cartconv
mv /usr/local/lib/vice VICE-$VICEVERSION/usr/local/lib
rm `find VICE-$VICEVERSION -name "amiga_*.vkm"`
rm `find VICE-$VICEVERSION -name "beos_*.vkm"`
rm `find VICE-$VICEVERSION -name "dos_*.vkm"`
rm `find VICE-$VICEVERSION -name "os2*.vkm"`
rm `find VICE-$VICEVERSION -name "osx*.vkm"`
rm `find VICE-$VICEVERSION -name "win_*.vkm"`
rm `find VICE-$VICEVERSION -name "RO*.vkm"`
mkdir -p VICE-$VICEVERSION$MANDIR/man1
mv $MANDIR/man1/c1541.1 VICE-$VICEVERSION$MANDIR/man1
mv $MANDIR/man1/petcat.1 VICE-$VICEVERSION$MANDIR/man1
mv $MANDIR/man1/vice.1 VICE-$VICEVERSION$MANDIR/man1
mkdir VICE-$VICEVERSION$INFODIR
mv $INFODIR/vice.info* VICE-$VICEVERSION$INFODIR
if test x"$ZIPKIND" = "xzip"; then
  rm -f -r /var/spool/pkg/UMCVICE
  gcc $TOPSRCDIR/src/arch/unix/sco_sol/convertprototype.c -o ./convertprototype

  currentdir=`pwd`

  cd VICE-$VICEVERSION/usr/local
  find . -print | pkgproto >prototype.tmp
  echo >prototype "i pkginfo=./pkginfo"
  $currentdir/convertprototype prototype.tmp >>prototype

  if test x"$SYSTEM" = "xsol"; then
    arch_cpu=`uname -m`

    if test x"$arch_cpu" = "xi86pc"; then
      arch_cpu=x86
    else
      arch_cpu=sparc
    fi

    arch_version=`uname -r`

    if test x"$arch_version" = "x5.6"; then
      arch_version=sol26
    fi

    if test x"$arch_version" = "x5.7"; then
      arch_version=sol7
    fi

    if test x"$arch_version" = "x5.8"; then
      arch_version=sol8
    fi

    if test x"$arch_version" = "x5.9"; then
      arch_version=sol9
    fi

    if test x"$arch_version" = "x5.10"; then
      arch_version=sol10
    fi
  else
    arch_cpu=x86

    if test x"$SYSTEM" = "xsco5"; then
      arch_version=osr5
    fi

    if test x"$SYSTEM" = "xsco6"; then
      arch_version=osr6
    fi

    if test x"$SYSTEM" = "xsco7"; then
      arch_version=uw7
    fi
  fi

  cat >pkginfo <<_END
PKG="UMCVICE"
NAME="VICE"
ARCH="$arch_cpu"
VERSION="$VICEVERSION"
CATEGORY="emulator"
VENDOR="The VICE Team"
EMAIL="vice-devel@firenze.linux.it"
PSTAMP="Marco van den Heuvel"
BASEDIR="/usr/local"
CLASSES="none"
_END

  packagename=vice-$VICEVERSION-$arch_version-$arch_cpu-local

  echo >$currentdir/input.txt all

  pkgmk -r `pwd`
  rm -f -r prototype.tmp
  cd /var/spool/pkg
  pkgtrans -s `pwd` /tmp/$packagename <$currentdir/input.txt
  gzip /tmp/$packagename
  cd $currentdir
  mv /tmp/$packagename.gz ./
  rm -f -r VICE-$VICEVERSION convertprototype $currentdir/input.txt

  echo $PLATFORM port binary package generated as $packagename.gz
else
  echo $PLATFORM port binary distribution directory generated as VICE-$VICEVERSION
fi
