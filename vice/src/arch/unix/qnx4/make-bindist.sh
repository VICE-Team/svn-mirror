#!/bin/sh
# make-bindist.sh for the QNX 4.x port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <vice-version> <prefix> <zip|nozip> <topsrcdir>
#                 $1             $2       $4          $5

VICEVERSION=$1
PREFIX=$2
ZIPKIND=$3
TOPSRCDIR=$4

if test x"$PREFIX" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if [ ! -e /usr/local/bin/x64 -o ! -e /usr/local/bin/x128 -o ! -e /usr/local/bin/xvic -o ! -e /usr/local/bin/xpet -o ! -e /usr/local/bin/xplus4 -o ! -e /usr/local/bin/xcbm2 -o ! -e /usr/local/bin/c1541 -o ! -e /usr/local/bin/petcat -o ! -e /usr/local/bin/cartconv ]
then
  echo Error: \"make install\" needs to be done first
  exit 1
fi

if test x"$ZIPKIND" = "xnozip"; then
  echo Error: This port only supports \"make bindistzip\"
  exit 1
fi

echo Generating QNX 4 port binary distribution.
wstrip /usr/local/bin/vsid
wstrip /usr/local/bin/x64
wstrip /usr/local/bin/x128
wstrip /usr/local/bin/xvic
wstrip /usr/local/bin/xpet
wstrip /usr/local/bin/xplus4
wstrip /usr/local/bin/xcbm2
wstrip /usr/local/bin/c1541
wstrip /usr/local/bin/petcat
wstrip /usr/local/bin/cartconv
cp $TOPSRCDIR/src/arch/unix/qnx4/install_msg ./
tar cf vice-$VICEVERSION-qnx4.tar install_msg /usr/local/bin/vsid /usr/local/bin/x64 /usr/local/bin/x128 /usr/local/bin/xvic /usr/local/bin/xpet /usr/local/bin/xplus4 /usr/local/bin/xcbm2 /usr/local/bin/c1541 /usr/local/bin/petcat /usr/local/bin/cartconv /usr/local/lib/vice /usr/local/man/man1/c1541.1 /usr/local/man/man1/petcat.1 /usr/local/man/man1/vice.1 /usr/local/info/vice.info*
freeze vice-$VICEVERSION-qnx4.tar
rm -f install_msg
echo QNX 4 port binary distribution directory generated as vice-$VICEVERSION-qnx4.tar.F
