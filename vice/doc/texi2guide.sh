#!/bin/sh
# texi2guide.sh - This script generates an amigaguide from vice.texi
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# texi2guide.sh <makeguide> <guide-file>
#               $1          $2

MAKEGUIDE=$1
GUIDEFILE=$2

$MAKEGUIDE --error-limit 10000 --amiga -o $GUIDEFILE vice.texi
