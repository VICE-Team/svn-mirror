#!/bin/sh
# texi2guide.sh - This script generates an amigaguide from vice.texi
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# texi2guide.sh <makeguide> <guide-file> <texi-file> <host-cpu> <host-os>
#               $1          $2           $3          $4         $5

MAKEGUIDE=$1
GUIDEFILE=$2
TEXIFILE=$3
HOSTCPU=$4
HOSTOS=$5

AMIGAFLAVOR=""

if test x"$HOSTOS" = "xaros"; then
  AMIGAFLAVOR=-Dplatformaros
fi

if test x"$HOSTOS" = "xmorphos"; then
  AMIGAFLAVOR=-Dplatformmorphos
fi

if test x"$HOSTOS" = "xwarpos"; then
  AMIGAFLAVOR=-Dplatformamigaos3
fi

if test x"$HOSTOS" = "xpowerup"; then
  AMIGAFLAVOR=-Dplatformamigaos3
fi

if test x"$HOSTOS" = "xamigaos" -a x"$HOSTCPU" = "xm68k"; then
  AMIGAFLAVOR=-Dplatformamigaos3
fi

if test x"$HOSTOS" = "xamigaos" -a x"$AMIGAFLAVOR" != "x-Dplatformamigaos3"; then
  AMIGAFLAVOR=-Dplatformamigaos4
fi

if test x"$AMIGAFLAVOR" = "x"; then
  AMIGAFLAVOR="-Dplatformamigaall"
fi

echo generating amigaguide
$MAKEGUIDE $AMIGAFLAVOR --error-limit 10000 --amiga -o $GUIDEFILE $TEXIFILE
echo done
