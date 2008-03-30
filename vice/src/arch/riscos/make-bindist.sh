#!/bin/sh
# make-bindist.sh for the RISC OS port
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <vice-ver-major> <vice-ver-minor> <zip|nozip> <top-srcdir>
#                 $1      $2               $3               $4          $5

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
             -name "win*.vkm,ffd" -or -name "osx*.vkm,ffd"`
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

if [ ! -e src/x64,ff8 -o ! -e src/x128,ff8 -o ! -e src/xvic,ff8 -o ! -e src/xpet,ff8 -o ! -e src/xplus4,ff8 -o ! -e src/xcbm2,ff8 -o ! -e src/c1541,ff8 -o ! -e src/petcat,ff8 -o ! -e src/cartconv,ff8 ]
then
  echo Error: executable file\(s\) not found, do a \"make\" first
  exit 1
fi

if [ ! "$GCCSDKDIR" ]; then
  echo "Set environment variable GCCSDKDIR first"
  exit 1
fi

echo Generating RISC OS port binary distribution.
rm -f -r vice-riscos$2_$3
$1 src/x64,ff8
$1 src/x128,ff8
$1 src/xvic,ff8
$1 src/xpet,ff8
$1 src/xplus4,ff8
$1 src/xcbm2,ff8
$1 src/c1541,ff8
$1 src/petcat,ff8
$1 src/cartconv,ff8
mkdir vice-riscos$2_$3
cp $5/src/arch/riscos/binfiles/ReadMeRO vice-riscos$2_$3/ReadMeRO,fff
cp $5/src/arch/riscos/binfiles/ReadMeSnd vice-riscos$2_$3/ReadMeSnd,fff
mkdir vice-riscos$2_$3/!Vice128
cp $5/src/arch/riscos/binfiles/runothers vice-riscos$2_$3/!Vice128/!Run,feb
cp src/x128,ff8 vice-riscos$2_$3/!Vice128/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/x128sprites vice-riscos$2_$3/!Vice128/!Sprites,ff9
mkdir vice-riscos$2_$3/!Vice64
cp $5/src/arch/riscos/binfiles/c64boot vice-riscos$2_$3/!Vice64/!Boot,feb
cp $5/src/arch/riscos/binfiles/runc64 vice-riscos$2_$3/!Vice64/!Run,feb
cp src/x64,ff8 vice-riscos$2_$3/!Vice64/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/x64sprites vice-riscos$2_$3/!Vice64/!Sprites,ff9
mkdir vice-riscos$2_$3/!ViceCBM2
cp $5/src/arch/riscos/binfiles/runothers vice-riscos$2_$3/!ViceCBM2/!Run,feb
cp src/xcbm2,ff8 vice-riscos$2_$3/!ViceCBM2/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/xcbm2sprites vice-riscos$2_$3/!ViceCBM2/!Sprites,ff9
mkdir vice-riscos$2_$3/!VicePET
cp $5/src/arch/riscos/binfiles/runothers vice-riscos$2_$3/!VicePET/!Run,feb
cp src/xpet,ff8 vice-riscos$2_$3/!VicePET/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/xpetsprites vice-riscos$2_$3/!VicePET/!Sprites,ff9
mkdir vice-riscos$2_$3/!VicePLUS4
cp $5/src/arch/riscos/binfiles/runothers vice-riscos$2_$3/!VicePLUS4/!Run,feb
cp src/xplus4,ff8 vice-riscos$2_$3/!VicePLUS4/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/xplus4sprites vice-riscos$2_$3/!VicePLUS4/!Sprites,ff9
mkdir vice-riscos$2_$3/!ViceVIC
cp $5/src/arch/riscos/binfiles/runothers vice-riscos$2_$3/!ViceVIC/!Run,feb
cp src/xvic,ff8 vice-riscos$2_$3/!ViceVIC/!RunImage,ff8
cp $5/src/arch/riscos/binfiles/xvicsprites vice-riscos$2_$3/!ViceVIC/!Sprites,ff9
mkdir vice-riscos$2_$3/!ViceVSID
cp $5/src/arch/riscos/binfiles/runvsid vice-riscos$2_$3/!ViceVSID/!Run,feb
cp $5/src/arch/riscos/binfiles/vsidsprites vice-riscos$2_$3/!ViceVSID/!Sprites,ff9
mkdir vice-riscos$2_$3/doc
htmlfiles=`find $5/doc/html -name "*.html"`
for i in $htmlfiles
do
  actualfile=`basename ${i%%.html}`
  cp $i vice-riscos$2_$3/doc/$actualfile,faf
done
cp $5/AUTHORS vice-riscos$2_$3/doc/AUTHORS,fff
cp $5/doc/html/plain/BUGS vice-riscos$2_$3/doc/BUGS,fff
cp $5/doc/html/plain/COPYING vice-riscos$2_$3/doc/COPYING,fff
cp $5/FEEDBACK vice-riscos$2_$3/doc/FEEDBACK,fff
cp $5/INSTALL vice-riscos$2_$3/doc/INSTALL,fff
cp $5/doc/html/plain/NEWS vice-riscos$2_$3/doc/NEWS,fff
cp $5/README vice-riscos$2_$3/doc/README,fff
cp $5/doc/html/plain/TODO vice-riscos$2_$3/doc/TODO,fff
mkdir vice-riscos$2_$3/doc/images
cp $5/doc/html/images/new.gif vice-riscos$2_$3/doc/images/new.gif,695
cp $5/doc/html/images/vice-logo.jpg vice-riscos$2_$3/doc/images/vice-logo.jpg,c85
mkdir vice-riscos$2_$3/!ViceRsrc
cp $5/src/arch/riscos/binfiles/rsrcboot vice-riscos$2_$3/!ViceRsrc/!Boot,feb
cp $5/src/arch/riscos/binfiles/help vice-riscos$2_$3/!ViceRsrc/!Help,fff
cp $5/src/arch/riscos/binfiles/rsrcrun vice-riscos$2_$3/!ViceRsrc/!Run,feb
cp $5/src/arch/riscos/binfiles/rsrcsprites vice-riscos$2_$3/!ViceRsrc/!Sprites,ff9
cp $5/src/arch/riscos/binfiles/bplot vice-riscos$2_$3/!ViceRsrc/BPlot,ffa
cp src/c1541,ff8 src/petcat,ff8 src/cartconv,ff8 vice-riscos$2_$3/!ViceRsrc
cp $5/src/arch/riscos/binfiles/messages vice-riscos$2_$3/!ViceRsrc/Messages,fff
cp $5/src/arch/riscos/binfiles/vicesprites vice-riscos$2_$3/!ViceRsrc/Sprites,ff9
cp $5/src/arch/riscos/binfiles/templates vice-riscos$2_$3/!ViceRsrc/Templates,fec
cp -a $5/data/C128 vice-riscos$2_$3/!ViceRsrc
cp $5/src/arch/riscos/binfiles/x128.vrs vice-riscos$2_$3/!ViceRsrc/C128/default.vrs
cp $5/src/arch/riscos/binfiles/x128basic vice-riscos$2_$3/!ViceRsrc/C128/basic
cp $5/src/arch/riscos/binfiles/x128charg64 vice-riscos$2_$3/!ViceRsrc/C128/charg64
cp $5/src/arch/riscos/binfiles/z80bios vice-riscos$2_$3/!ViceRsrc/C128/z80bios
fix_data_dir vice-riscos$2_$3/!ViceRsrc/C128
cp $5/src/arch/riscos/binfiles/x128.vra vice-riscos$2_$3/!ViceRsrc/C128/romset.vra,fff
cp $5/src/arch/riscos/binfiles/x128vicerc vice-riscos$2_$3/!ViceRsrc/C128/vicerc,fff
cp -a $5/data/C64 vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/C64
rm -f vice-riscos$2_$3/!ViceRsrc/C64/c64mem.sym,ffd
cp $5/src/arch/riscos/binfiles/x64.vra vice-riscos$2_$3/!ViceRsrc/C64/romset.vra,fff
cp $5/src/arch/riscos/binfiles/x64vicerc vice-riscos$2_$3/!ViceRsrc/C64/vicerc,fff
cp -a $5/data/CBM-II vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/CBM-II
cp $5/src/arch/riscos/binfiles/xcbm2.vra vice-riscos$2_$3/!ViceRsrc/CBM-II/romset.vra,fff
cp $5/src/arch/riscos/binfiles/xcbm2vicerc vice-riscos$2_$3/!ViceRsrc/CBM-II/vicerc,fff
cp -a $5/data/DRIVES vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/DRIVES
mkdir vice-riscos$2_$3/!ViceRsrc/fonts
cp $5/data/fonts/vice-cbm.bdf vice-riscos$2_$3/!ViceRsrc/fonts/vice-cbm.bfd,fff
cp -a $5/data/PET vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/PET
cp $5/src/arch/riscos/binfiles/xpet.vra vice-riscos$2_$3/!ViceRsrc/PET/romset.vra,fff
cp $5/src/arch/riscos/binfiles/xpetvicerc vice-riscos$2_$3/!ViceRsrc/PET/vicerc,fff
cp -a $5/data/PLUS4 vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/PLUS4
cp $5/src/arch/riscos/binfiles/xplus4vicerc vice-riscos$2_$3/!ViceRsrc/PLUS4/vicerc,fff
cp -a $5/data/PRINTER vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/PRINTER
cp -a $5/data/VIC20 vice-riscos$2_$3/!ViceRsrc
fix_data_dir vice-riscos$2_$3/!ViceRsrc/VIC20
cp $5/src/arch/riscos/binfiles/xvic.vra vice-riscos$2_$3/!ViceRsrc/VIC20/romset.vra,fff
cp $5/src/arch/riscos/binfiles/xvicvicerc vice-riscos$2_$3/!ViceRsrc/VIC20/vicerc,fff
mkdir vice-riscos$2_$3/!ViceRsrc/VSID
cp $5/src/arch/riscos/binfiles/xvsidvicerc vice-riscos$2_$3/!ViceRsrc/VSID/vicerc,fff
if test x"$4" = "xzip"; then
  cd vice-riscos$2_$3
  ${GCCSDKDIR}/bin/zip -r -9 -q -, ../vice-riscos$2_$3.zip *
  cd ..
  rm -f -r vice-riscos$2_$3
  echo RISC OS port binary distribution archive generated as vice-riscos$2_$3.zip
else
  echo RISC OS port binary distribution directory generated as vice-riscos$2_$3
fi
