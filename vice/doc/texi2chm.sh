#!/bin/sh
# texi2chm.sh - This script uses t2h.sh and the windows chm compiler to generate a vice.chm
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# texi2chm.sh <hhc> <chm-file>
#             $1    $2

HHC=$1
CHMFILE=$2

rm -f -r chmtmp
mkdir chmtmp
cd chmtmp
cp ../vice.texi ./
../t2h.pl vice.texi
$HHC vice.hhp
mv vice.chm ../$CHMFILE
cd ..
rm -f -r chmtmp
