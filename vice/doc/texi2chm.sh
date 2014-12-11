#!/bin/sh
# texi2chm.sh - This script uses t2h.pl and the windows chm compiler to generate a vice.chm
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# texi2chm.sh <hhc> <chm-file> <texi-file> <fixdox.sh> <t2h.pl>
#             $1    $2         $3          $4          $5

HHC=$1
CHMFILE=$2
TEXIFILE=$3
FIXDOXSH=$4
T2HPL=$5

rm -f -r chmtmp
mkdir chmtmp
cp $FIXDOXSH chmtmp
cp $T2HPL chmtmp
cp $TEXIFILE chmtmp
cd chmtmp
./fixdox.sh chm <vice.texi >vicefix.texi
rm -f vice.texi
mv vicefix.texi vice.texi
./t2h.pl vice.texi
$HHC vice.hhp
cd ..
mv chmtmp/vice.chm $CHMFILE
rm -f -r chmtmp
