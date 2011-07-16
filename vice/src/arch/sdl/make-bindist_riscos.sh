#!/bin/sh
# make-bindist.sh for the RISC OS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-ver-major> <vice-ver-minor> <zip|nozip> <top-srcdir>
#                 $1      $2               $3               $4          $5

STRIP=$1
VICEVERMAJOR=$2
VICEVERMINOR=$3
ZIPKIND=$4
TOPSRCDIR=$5

fix_data_dir()
{
  datafiles=`ls $1`
  for i in $datafiles
  do
    mv $1/$i $1/$i,ffd
  done
  datafiles=`find $1 -name "Makefile*" -or -name "amiga_*.vkm,ffd" -or \
             -name "dos_*.vkm,ffd" -or -name "os2*.vkm,ffd" -or \
             -name "beos_*.vkm,ffd" -or -name "x11_*.vkm,ffd" -or \
             -name "win*.v*,ffd" -or -name "osx*.vkm,ffd"`
  for i in $datafiles
  do
    if [ -e $i ]
    then
      rm $i
    fi
  done
  datafiles=`find $1 -name "*.vpl,ffd"`
  for i in $datafiles
  do
    actualfile=${i%%.vpl,ffd}
    if [ -e $i ]
    then
      mv $i $actualfile.vpl,fff
    fi
  done
  datafiles=`find $1 -name "*.vrs,ffd"`
  for i in $datafiles
  do
    actualfile=${i%%.vrs,ffd}
    if [ -e $i ]
    then
      mv $i $actualfile.vrs,fff
    fi
  done
  datafiles=`find $1 -name "*.vkm,ffd"`
  for i in $datafiles
  do
    actualfile=${i%%.vkm,ffd}
    if [ -e $i ]
    then
      mv $i $actualfile.vkm,fff
    fi
  done
}

if [ ! -e src/x64,ff8 -o ! -e src/x64dtv,ff8 -o ! -e src/x128,ff8 -o ! -e src/xvic,ff8 -o ! -e src/xpet,ff8 -o ! -e src/xplus4,ff8 -o ! -e src/xcbm2,ff8 -o ! -e src/c1541,ff8 -o ! -e src/petcat,ff8 -o ! -e src/cartconv,ff8 ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

if [ ! "$GCCSDKDIR" ]; then
  echo "Set environment variable GCCSDKDIR first"
  exit 1
fi

echo Generating RISC OS SDL port binary distribution.
rm -f -r SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR
$STRIP src/x64,ff8
$STRIP src/x128,ff8
$STRIP src/xvic,ff8
$STRIP src/xpet,ff8
$STRIP src/xplus4,ff8
$STRIP src/xcbm2,ff8
$STRIP src/c1541,ff8
$STRIP src/petcat,ff8
$STRIP src/cartconv,ff8
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR
cp $TOPSRCDIR/doc/readmes/Readme-SDL.txt SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/ReadmeSDL,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/ReadMeSnd SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/ReadMeSnd,fff
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice128
cp $TOPSRCDIR/src/arch/riscos/binfiles/runothers SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice128/!Run,feb
cp src/x128,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice128/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128sprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice128/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64
cp $TOPSRCDIR/src/arch/riscos/binfiles/c64boot SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64/!Boot,feb
cp $TOPSRCDIR/src/arch/riscos/binfiles/runc64 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64/!Run,feb
cp src/x64,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64sprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64dtv
cp $TOPSRCDIR/src/arch/riscos/binfiles/c64dtvboot SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64dtv/!Boot,feb
cp $TOPSRCDIR/src/arch/riscos/binfiles/runc64dtv SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64dtv/!Run,feb
cp src/x64dtv,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64dtv/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64sprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!Vice64dtv/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceCBM2
cp $TOPSRCDIR/src/arch/riscos/binfiles/runothers SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceCBM2/!Run,feb
cp src/xcbm2,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceCBM2/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/xcbm2sprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceCBM2/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePET
cp $TOPSRCDIR/src/arch/riscos/binfiles/runothers SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePET/!Run,feb
cp src/xpet,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePET/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/xpetsprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePET/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePLUS4
cp $TOPSRCDIR/src/arch/riscos/binfiles/runothers SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePLUS4/!Run,feb
cp src/xplus4,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePLUS4/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/xplus4sprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!VicePLUS4/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceVIC
cp $TOPSRCDIR/src/arch/riscos/binfiles/runothers SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceVIC/!Run,feb
cp src/xvic,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceVIC/!RunImage,ff8
cp $TOPSRCDIR/src/arch/riscos/binfiles/xvicsprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceVIC/!Sprites,ff9
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc
htmlfiles=`find $TOPSRCDIR/doc/html -name "*.html"`
for i in $htmlfiles
do
  actualfile=`basename ${i%%.html}`
  cp $i SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/$actualfile,faf
done
cp $TOPSRCDIR/AUTHORS SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/AUTHORS,fff
cp $TOPSRCDIR/doc/html/plain/BUGS SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/BUGS,fff
cp $TOPSRCDIR/doc/html/plain/COPYING SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/COPYING,fff
cp $TOPSRCDIR/FEEDBACK SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/FEEDBACK,fff
cp $TOPSRCDIR/INSTALL SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/INSTALL,fff
cp $TOPSRCDIR/doc/html/plain/NEWS SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/NEWS,fff
cp $TOPSRCDIR/README SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/README,fff
cp $TOPSRCDIR/doc/html/plain/TODO SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/TODO,fff
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/images
cp $TOPSRCDIR/doc/html/images/new.gif SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/images/new.gif,695
cp $TOPSRCDIR/doc/html/images/vice-logo.jpg SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/doc/images/vice-logo.jpg,c85
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
cp $TOPSRCDIR/src/arch/riscos/binfiles/rsrcboot SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/!Boot,feb
cp $TOPSRCDIR/src/arch/riscos/binfiles/help SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/!Help,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/rsrcrun SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/!Run,feb
cp $TOPSRCDIR/src/arch/riscos/binfiles/rsrcsprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/!Sprites,ff9
cp $TOPSRCDIR/src/arch/riscos/binfiles/bplot SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/BPlot,ffa
cp src/c1541,ff8 src/petcat,ff8 src/cartconv,ff8 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
cp $TOPSRCDIR/src/arch/riscos/binfiles/messages SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/Messages,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/vicesprites SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/Sprites,ff9
cp $TOPSRCDIR/src/arch/riscos/binfiles/templates SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/Templates,fec
cp -a $TOPSRCDIR/data/C128 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128.vrs SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/default.vrs
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128basic SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/basic
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128charg64 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/charg64
cp $TOPSRCDIR/src/arch/riscos/binfiles/z80bios SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/z80bios
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/x128vicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C128/vicerc,fff
cp -a $TOPSRCDIR/data/C64 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64
rm -f SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64/c64mem.sym,ffd
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64vicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64/vicerc,fff
cp -a $TOPSRCDIR/data/C64DTV SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64DTV
rm -f SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64DTV/c64mem.sym,ffd
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64DTV/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/x64vicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/C64DTV/vicerc,fff
cp -a $TOPSRCDIR/data/CBM-II SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/CBM-II
cp $TOPSRCDIR/src/arch/riscos/binfiles/xcbm2.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/CBM-II/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/xcbm2vicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/CBM-II/vicerc,fff
cp -a $TOPSRCDIR/data/DRIVES SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/DRIVES
mkdir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/fonts
cp $TOPSRCDIR/data/fonts/vice-cbm.bdf SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/fonts/vice-cbm.bfd,fff
cp -a $TOPSRCDIR/data/PET SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PET
cp $TOPSRCDIR/src/arch/riscos/binfiles/xpet.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PET/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/xpetvicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PET/vicerc,fff
cp -a $TOPSRCDIR/data/PLUS4 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PLUS4
cp $TOPSRCDIR/src/arch/riscos/binfiles/xplus4vicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PLUS4/vicerc,fff
cp -a $TOPSRCDIR/data/PRINTER SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/PRINTER
cp -a $TOPSRCDIR/data/VIC20 SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc
fix_data_dir SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/VIC20
cp $TOPSRCDIR/src/arch/riscos/binfiles/xvic.vra SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/VIC20/romset.vra,fff
cp $TOPSRCDIR/src/arch/riscos/binfiles/xvicvicerc SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR/!ViceRsrc/VIC20/vicerc,fff
if test x"$ZIPKIND" = "xzip"; then
  cd SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR
  ${GCCSDKDIR}/bin/zip -r -9 -q -, ../vice-riscos$VICEVERMAJOR\_$VICEVERMINOR.zip *
  cd ..
  rm -f -r SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR
  echo RISC OS SDL port binary distribution archive generated as SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR.zip
else
  echo RISC OS SDL port binary distribution directory generated as SDLVICE-riscos$VICEVERMAJOR\_$VICEVERMINOR
fi
