#!/bin/sh
# texi2guide.sh - This script generates an amigaguide from vice.texi
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# texi2guide.sh <makeguide> <guide-file> <texi-file>
#               $1          $2           $3

MAKEGUIDE=$1
GUIDEFILE=$2
TEXIFILE=$3

$MAKEGUIDE --error-limit 10000 --amiga -o $GUIDEFILE $TEXIFILE
