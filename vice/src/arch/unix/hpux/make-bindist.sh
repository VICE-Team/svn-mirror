#!/bin/sh
# make-bindist.sh for the HPUX ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# make-bindist.sh <strip> <perl> <vice-version> <prefix> <hostcpu> <system> <--enable-arch> <zip|nozip> <x64sc-included> <topsrcdir>
#                 $1      $2     $3             $4       $5        $6       $7              $8          $9               $10

STRIP=$1
PERL=$2
VICEVERSION=$3
PREFIX=$4
CPU=$5
SYSTEM=$6
ENABLEARCH=$7
ZIPKIND=$8
X64SC=$9

shift
TOPSRCDIR=$9

UUID=`uuidgen`
DATESTRING=`$PERL src/arch/unix/hpux/getnow.pl`
UNAMES=`uname -s`
UNAMER=`uname -r`
UNAMEM=`uname -m`
UNAMECONCAT="$UNAMES_$UNAMER_$UNAMEM"

getsize()
{
  gotsize=$2
}

getsum()
{
  gotsum=$1
}

installfile()
{
# installfile <source-file> <dest-path> <info-file> <installed-path> <total size var> <mode>
#             $1            $2          $3          $4               $5               $6

  check=`cksum $1`
  moddate=`$PERL src/arch/unix/hpux/getdate.pl $1`
  getsize $check
  getsum $check
  cp $1 $2$4
  chmod $6 $2$4
  echo >>$3 file
  echo >>$3 path $4
  echo >>$3 type f
  echo >>$3 size $gotsize
  echo >>$3 cksum $gotsum
  echo >>$3 mode $6
  echo >>$3 uid 0
  echo >>$3 gid 3
  echo >>$3 mtime $moddate
  if test x"$5" = "xman"; then
    mantotal=`expr $mantotal + $gotsize`
  else
    runtotal=`expr $runtotal + $gotsize`
  fi
}

if test x"$PREFIX" != "x/usr/local"; then
  echo Error: installation path is not /usr/local
  exit 1
fi

if test x"$X64SC" = "xyes"; then
  SCFILE="x64sc"
else
  SCFILE=""
fi

EMULATORS="x64 xscpu64 x64dtv $SCFILE x128 xcbm2 xcbm5x0 xpet xplus4 xvic vsid"
CONSOLE_TOOLS="c1541 cartconv petcat"
EXECUTABLES="$EMULATORS $CONSOLE_TOOLS"

for i in $EXECUTABLES
do
  if [ ! -e src/$i ]
  then
    echo Error: executable file\(s\) not found, do a \"make all\" first
    exit 1
  fi
done

echo Generating HPUX port binary distribution.
rm -f -r catalog vice runtmp mantmp

# Prepare directory structure
mkdir -p catalog/dfiles
mkdir -p catalog/vice/pfiles
mkdir -p catalog/vice/vice-MAN
mkdir -p catalog/vice/vice-RUN
mkdir -p vice/vice-MAN/usr/local/man/man1
mkdir -p vice/vice-RUN/usr/local/bin
mkdir -p vice/vice-RUN/usr/local/lib/vice/VIC20
mkdir -p vice/vice-RUN/usr/local/lib/vice/PRINTER
mkdir -p vice/vice-RUN/usr/local/lib/vice/PLUS4
mkdir -p vice/vice-RUN/usr/local/lib/vice/PET
mkdir -p vice/vice-RUN/usr/local/lib/vice/DRIVES
mkdir -p vice/vice-RUN/usr/local/lib/vice/doc/images
mkdir -p vice/vice-RUN/usr/local/lib/vice/fonts
mkdir -p vice/vice-RUN/usr/local/lib/vice/CBM-II
mkdir -p vice/vice-RUN/usr/local/lib/vice/C64DTV
mkdir -p vice/vice-RUN/usr/local/lib/vice/C64
mkdir -p vice/vice-RUN/usr/local/lib/vice/C128
mkdir -p vice/vice-RUN/usr/local/info

# Prepare font
mkdir -p fonttmp
cp data/fonts/vice-cbm.bdf fonttmp
cd fonttmp
bdftopcf -o vice-cbm.pcf vice-cbm.bdf
rm vice-cbm.bdf
mkfontdir .
cd ..

# Install and prepare vice-RUN
runtotal=0
for i in $EXECUTABLES
do
  $STRIP src/$i
  installfile src/$i vice/vice-RUN runtmp /usr/local/bin/$i run 0755
done
installfile doc/vice.info vice/vice-RUN runtmp /usr/local/info/vice.info run 0644
installfile data/C128/basic64 vice/vice-RUN runtmp /usr/local/lib/vice/C128/basic64 run 0644
installfile data/C128/basichi vice/vice-RUN runtmp /usr/local/lib/vice/C128/basichi run 0644
installfile data/C128/basiclo vice/vice-RUN runtmp /usr/local/lib/vice/C128/basiclo run 0644
installfile data/C128/c64hq.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/c64hq.vpl run 0644
installfile data/C128/c64s.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/c64s.vpl run 0644
installfile data/C128/ccs64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/ccs64.vpl run 0644
installfile data/C128/chargde vice/vice-RUN runtmp /usr/local/lib/vice/C128/chargde run 0644
installfile data/C128/chargen vice/vice-RUN runtmp /usr/local/lib/vice/C128/chargen run 0644
installfile data/C128/chargfr vice/vice-RUN runtmp /usr/local/lib/vice/C128/chargfr run 0644
installfile data/C128/chargse vice/vice-RUN runtmp /usr/local/lib/vice/C128/chargse run 0644
installfile data/C128/chargch vice/vice-RUN runtmp /usr/local/lib/vice/C128/chargch run 0644
installfile data/C128/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/default.vpl run 0644
installfile data/C128/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/C128/default.vrs run 0644
installfile data/C128/frodo.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/frodo.vpl run 0644
installfile data/C128/godot.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/godot.vpl run 0644
installfile data/C128/kernal vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernal run 0644
installfile data/C128/kernal64 vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernal64 run 0644
installfile data/C128/kernalde vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalde run 0644
installfile data/C128/kernalfi vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalfi run 0644
installfile data/C128/kernalfr vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalfr run 0644
installfile data/C128/kernalit vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalit run 0644
installfile data/C128/kernalno vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalno run 0644
installfile data/C128/kernalse vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalse run 0644
installfile data/C128/kernalch vice/vice-RUN runtmp /usr/local/lib/vice/C128/kernalch run 0644
installfile data/C128/pc64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/pc64.vpl run 0644
installfile data/C128/vdc_deft.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/vdc_deft.vpl run 0644
installfile data/C128/vdc_comp.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/vdc_comp.vpl run 0644
installfile data/C128/vice.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C128/vice.vpl run 0644
installfile data/C128/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C128/x11_pos.vkm run 0644
installfile data/C128/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C128/x11_sym.vkm run 0644
installfile data/C64/basic vice/vice-RUN runtmp /usr/local/lib/vice/C64/basic run 0644
installfile data/C64/c64hq.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/c64hq.vpl run 0644
installfile data/C64/c64mem.sym vice/vice-RUN runtmp /usr/local/lib/vice/C64/c64mem.sym run 0644
installfile data/C64/c64s.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/c64s.vpl run 0644
installfile data/C64/ccs64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/ccs64.vpl run 0644
installfile data/C64/chargen vice/vice-RUN runtmp /usr/local/lib/vice/C64/chargen run 0644
installfile data/C64/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/default.vpl run 0644
installfile data/C64/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/C64/default.vrs run 0644
installfile data/C64/edkernal vice/vice-RUN runtmp /usr/local/lib/vice/C64/edkernal run 0644
installfile data/C64/frodo.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/frodo.vpl run 0644
installfile data/C64/godot.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/godot.vpl run 0644
installfile data/C64/gskernal vice/vice-RUN runtmp /usr/local/lib/vice/C64/gskernal run 0644
installfile data/C64/jpchrgen vice/vice-RUN runtmp /usr/local/lib/vice/C64/jpchrgen run 0644
installfile data/C64/jpkernal vice/vice-RUN runtmp /usr/local/lib/vice/C64/jpkernal run 0644
installfile data/C64/kernal vice/vice-RUN runtmp /usr/local/lib/vice/C64/kernal run 0644
installfile data/C64/pc64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/pc64.vpl run 0644
installfile data/C64/sxkernal vice/vice-RUN runtmp /usr/local/lib/vice/C64/sxkernal run 0644
installfile data/C64/vice.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64/vice.vpl run 0644
installfile data/C64/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C64/x11_pos.vkm run 0644
installfile data/C64/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C64/x11_sym.vkm run 0644
installfile data/SCPU64/c64hq.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/c64hq.vpl run 0644
installfile data/SCPU64/c64s.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/c64s.vpl run 0644
installfile data/SCPU64/ccs64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/ccs64.vpl run 0644
installfile data/SCPU64/chargen vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/chargen run 0644
installfile data/SCPU64/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/default.vpl run 0644
installfile data/SCPU64/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/default.vrs run 0644
installfile data/SCPU64/frodo.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/frodo.vpl run 0644
installfile data/SCPU64/godot.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/godot.vpl run 0644
installfile data/SCPU64/pc64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/pc64.vpl run 0644
installfile data/SCPU64/scpu64 vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/scpu64 run 0644
installfile data/SCPU64/jpchrgen vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/jpchrgen run 0644
installfile data/SCPU64/scpu64mem.sym vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/scpu64mem.sym run 0644
installfile data/SCPU64/vice.vpl vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/vice.vpl run 0644
installfile data/SCPU64/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/x11_pos.vkm run 0644
installfile data/SCPU64/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/SCPU64/x11_sym.vkm run 0644
installfile data/C64DTV/basic vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/basic run 0644
installfile data/C64DTV/c64hq.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/c64hq.vpl run 0644
installfile data/C64DTV/c64mem.sym vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/c64mem.sym run 0644
installfile data/C64DTV/c64s.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/c64s.vpl run 0644
installfile data/C64DTV/ccs64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/ccs64.vpl run 0644
installfile data/C64DTV/chargen vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/chargen run 0644
installfile data/C64DTV/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/default.vpl run 0644
installfile data/C64DTV/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/default.vrs run 0644
installfile data/C64DTV/dtvrom.bin vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/dtvrom.bin run 0644
installfile data/C64DTV/frodo.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/frodo.vpl run 0644
installfile data/C64DTV/godot.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/godot.vpl run 0644
installfile data/C64DTV/kernal vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/kernal run 0644
installfile data/C64DTV/pc64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/pc64.vpl run 0644
installfile data/C64DTV/vice.vpl vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/vice.vpl run 0644
installfile data/C64DTV/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/x11_pos.vkm run 0644
installfile data/C64DTV/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/C64DTV/x11_sym.vkm run 0644
installfile data/CBM-II/amber.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/amber.vpl run 0644
installfile data/CBM-II/basic.128 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/basic.128 run 0644
installfile data/CBM-II/basic.256 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/basic.256 run 0644
installfile data/CBM-II/basic.500 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/basic.500 run 0644
installfile data/CBM-II/c64hq.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/c64hq.vpl run 0644
installfile data/CBM-II/c64s.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/c64s.vpl run 0644
installfile data/CBM-II/ccs64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/ccs64.vpl run 0644
installfile data/CBM-II/chargen.500 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/chargen.500 run 0644
installfile data/CBM-II/chargen.600 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/chargen.600 run 0644
installfile data/CBM-II/chargen.700 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/chargen.700 run 0644
installfile data/CBM-II/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/default.vpl run 0644
installfile data/CBM-II/frodo.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/frodo.vpl run 0644
installfile data/CBM-II/godot.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/godot.vpl run 0644
installfile data/CBM-II/green.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/green.vpl run 0644
installfile data/CBM-II/kernal vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/kernal run 0644
installfile data/CBM-II/kernal.500 vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/kernal.500 run 0644
installfile data/CBM-II/pc64.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/pc64.vpl run 0644
installfile data/CBM-II/rom128h.vrs vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/rom128h.vrs run 0644
installfile data/CBM-II/rom128l.vrs vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/rom128l.vrs run 0644
installfile data/CBM-II/rom256h.vrs vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/rom256h.vrs run 0644
installfile data/CBM-II/rom256l.vrs vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/rom256l.vrs run 0644
installfile data/CBM-II/rom500.vrs vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/rom500.vrs run 0644
installfile data/CBM-II/white.vpl vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/white.vpl run 0644
installfile data/CBM-II/x11_buks.vkm vice/vice-RUN runtmp /usr/local/lib/vice/CBM-II/x11_buks.vkm run 0644
installfile COPYING vice/vice-RUN runtmp /usr/local/lib/vice/doc/COPYING run 0644
installfile NEWS vice/vice-RUN runtmp /usr/local/lib/vice/doc/NEWS run 0644
installfile doc/NLS-Howto.txt vice/vice-RUN runtmp /usr/local/lib/vice/doc/NLS-Howto.txt run 0644
installfile doc/html/amigaos.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/amigaos.html run 0644
installfile doc/html/beos.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/beos.html run 0644
installfile doc/iec-bus.txt vice/vice-RUN runtmp /usr/local/lib/vice/doc/iec-bus.txt run 0644
installfile doc/html/index.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/index.html run 0644
installfile doc/html/macosx.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/macosx.html run 0644
installfile doc/html/images/new.gif vice/vice-RUN runtmp /usr/local/lib/vice/doc/images/new.gif run 0644
installfile doc/openGLsync-howto.txt vice/vice-RUN runtmp /usr/local/lib/vice/doc/openGLsync-howto.txt run 0644
installfile doc/html/solaris.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/solaris.html run 0644
installfile doc/html/images/vice-logo.jpg vice/vice-RUN runtmp /usr/local/lib/vice/doc/images/vice-logo.jpg run 0644
installfile doc/html/vice_1.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_1.html run 0644
installfile doc/html/vice_2.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_2.html run 0644
installfile doc/html/vice_3.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_3.html run 0644
installfile doc/html/vice_4.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_4.html run 0644
installfile doc/html/vice_5.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_5.html run 0644
installfile doc/html/vice_6.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_6.html run 0644
installfile doc/html/vice_7.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_7.html run 0644
installfile doc/html/vice_8.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_8.html run 0644
installfile doc/html/vice_9.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_9.html run 0644
installfile doc/html/vice_10.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_10.html run 0644
installfile doc/html/vice_11.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_11.html run 0644
installfile doc/html/vice_12.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_12.html run 0644
installfile doc/html/vice_13.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_13.html run 0644
installfile doc/html/vice_14.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_14.html run 0644
installfile doc/html/vice_15.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_15.html run 0644
installfile doc/html/vice_16.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_16.html run 0644
installfile doc/html/vice_toc.html vice/vice-RUN runtmp /usr/local/lib/vice/doc/vice_toc.html run 0644
installfile data/DRIVES/dos1001 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1001 run 0644
installfile data/DRIVES/dos1540 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1540 run 0644
installfile data/DRIVES/dos1541 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1541 run 0644
installfile data/DRIVES/d1541II vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/d1541II run 0644
installfile data/DRIVES/dos1551 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1551 run 0644
installfile data/DRIVES/dos1570 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1570 run 0644
installfile data/DRIVES/dos1571 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1571 run 0644
installfile data/DRIVES/d1571cr vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/d1571cr run 0644
installfile data/DRIVES/dos1581 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos1581 run 0644
installfile data/DRIVES/dos2000 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos2000 run 0644
installfile data/DRIVES/dos4000 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos4000 run 0644
installfile data/DRIVES/dos2031 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos2031 run 0644
installfile data/DRIVES/dos2040 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos2040 run 0644
installfile data/DRIVES/dos3040 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos3040 run 0644
installfile data/DRIVES/dos4040 vice/vice-RUN runtmp /usr/local/lib/vice/DRIVES/dos4040 run 0644
installfile fonttmp/fonts.dir vice/vice-RUN runtmp /usr/local/lib/vice/fonts/fonts.dir run 0644
installfile fonttmp/vice-cbm.pcf vice/vice-RUN runtmp /usr/local/lib/vice/fonts/vice-cbm.pcf run 0644
installfile data/PET/amber.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PET/amber.vpl run 0644
installfile data/PET/basic1 vice/vice-RUN runtmp /usr/local/lib/vice/PET/basic1 run 0644
installfile data/PET/basic2 vice/vice-RUN runtmp /usr/local/lib/vice/PET/basic2 run 0644
installfile data/PET/basic4 vice/vice-RUN runtmp /usr/local/lib/vice/PET/basic4 run 0644
installfile data/PET/chargen vice/vice-RUN runtmp /usr/local/lib/vice/PET/chargen run 0644
installfile data/PET/chargen.de vice/vice-RUN runtmp /usr/local/lib/vice/PET/chargen.de run 0644
installfile data/PET/edit1g vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit1g run 0644
installfile data/PET/edit2b vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit2b run 0644
installfile data/PET/edit2g vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit2g run 0644
installfile data/PET/edit4b40 vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit4b40 run 0644
installfile data/PET/edit4b80 vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit4b80 run 0644
installfile data/PET/edit4g40 vice/vice-RUN runtmp /usr/local/lib/vice/PET/edit4g40 run 0644
installfile data/PET/green.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PET/green.vpl run 0644
installfile data/PET/kernal1 vice/vice-RUN runtmp /usr/local/lib/vice/PET/kernal1 run 0644
installfile data/PET/kernal2 vice/vice-RUN runtmp /usr/local/lib/vice/PET/kernal2 run 0644
installfile data/PET/kernal4 vice/vice-RUN runtmp /usr/local/lib/vice/PET/kernal4 run 0644
installfile data/PET/rom1g.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom1g.vrs run 0644
installfile data/PET/rom2b.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom2b.vrs run 0644
installfile data/PET/rom2g.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom2g.vrs run 0644
installfile data/PET/rom4b40.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom4b40.vrs run 0644
installfile data/PET/rom4b80.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom4b80.vrs run 0644
installfile data/PET/rom4g40.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PET/rom4g40.vrs run 0644
installfile data/PET/white.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PET/white.vpl run 0644
installfile data/PET/x11_bdep.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PET/x11_bdep.vkm run 0644
installfile data/PET/x11_bdes.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PET/x11_bdes.vkm run 0644
installfile data/PET/x11_bgrs.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PET/x11_bgrs.vkm run 0644
installfile data/PET/x11_buks.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PET/x11_buks.vkm run 0644
installfile data/PLUS4/3plus1hi vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/3plus1hi run 0644
installfile data/PLUS4/3plus1lo vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/3plus1lo run 0644
installfile data/PLUS4/basic vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/basic run 0644
installfile data/PLUS4/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/default.vpl run 0644
installfile data/PLUS4/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/default.vrs run 0644
installfile data/PLUS4/kernal vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/kernal run 0644
installfile data/PLUS4/kernal.005 vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/kernal.005 run 0644
installfile data/PLUS4/kernal.232 vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/kernal.232 run 0644
installfile data/PLUS4/kernal.364 vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/kernal.364 run 0644
installfile data/PLUS4/vice.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/vice.vpl run 0644
installfile data/PLUS4/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/x11_pos.vkm run 0644
installfile data/PLUS4/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/PLUS4/x11_sym.vkm run 0644
installfile data/PRINTER/cbm1526 vice/vice-RUN runtmp /usr/local/lib/vice/PRINTER/cbm1526 run 0644
installfile data/PRINTER/mps801 vice/vice-RUN runtmp /usr/local/lib/vice/PRINTER/mps801 run 0644
installfile data/PRINTER/mps803 vice/vice-RUN runtmp /usr/local/lib/vice/PRINTER/mps803 run 0644
installfile data/PRINTER/mps803.vpl vice/vice-RUN runtmp /usr/local/lib/vice/PRINTER/mps803.vpl run 0644
installfile data/PRINTER/nl10-cbm vice/vice-RUN runtmp /usr/local/lib/vice/PRINTER/nl10-cbm run 0644
installfile data/VIC20/basic vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/basic run 0644
installfile data/VIC20/chargen vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/chargen run 0644
installfile data/VIC20/default.vpl vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/default.vpl run 0644
installfile data/VIC20/default.vrs vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/default.vrs run 0644
installfile data/VIC20/kernal vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/kernal run 0644
installfile data/VIC20/x11_pos.vkm vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/x11_pos.vkm run 0644
installfile data/VIC20/x11_sym.vkm vice/vice-RUN runtmp /usr/local/lib/vice/VIC20/x11_sym.vkm run 0644

# Install and prepare vice-MAN
mantotal=0
installfile man/c1541.1 vice/vice-MAN mantmp /usr/local/man/man1/c1541.1 man 0644
installfile man/petcat.1 vice/vice-MAN mantmp /usr/local/man/man1/petcat.1 man 0644
installfile man/vice.1 vice/vice-MAN mantmp /usr/local/man/man1/vice.1 man 0644

# Make catalog/dfiles/INDEX
cat >catalog/dfiles/INDEX <<_END
distribution
layout_version 1.0
data_model_revision 2.40
uuid $UUID
mod_time $DATESTRING
create_time $DATESTRING
path_max 255
name_max 100
media 
sequence_number 1
_END

# Make catalog/dfiles/INFO
check=`cksum catalog/dfiles/INDEX`
getsize $check

cat >catalog/dfiles/INFO.1 <<_END
control_file
path INDEX
size $gotsize
mode 0444
mtime $DATESTRING
tag INDEX
control_file
path INFO
_END

echo >catalog/dfiles/INFO.2 size xxx

cat >catalog/dfiles/INFO.3 <<_END
mode 0444
mtime $DATESTRING
tag INFO
_END

check1=`cksum catalog/dfiles/INFO.1`
check2=`cksum catalog/dfiles/INFO.2`
check3=`cksum catalog/dfiles/INFO.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
totalsize=`expr $size1 + $size2 + $size3 + 60`
echo >catalog/dfiles/INFO.2 size $totalsize
cat >catalog/dfiles/INFO catalog/dfiles/INFO.1 catalog/dfiles/INFO.2 catalog/dfiles/INFO.3
rm -f catalog/dfiles/INFO.*

# Make catalog/vice/pfiles/INDEX
cat >catalog/vice/pfiles/INDEX <<_END
product
tag vice
data_model_revision 2.40
instance_id 1
control_directory vice
revision $VICEVERSION
title "vice"
mod_time $DATESTRING
create_time $DATESTRING
architecture $UNAMECONCAT
machine_type *
os_name $UNAMES
os_release $UNAMER
os_version *
directory /usr/local
all_filesets vice-MAN vice-RUN 
is_locatable false
subproduct
tag Manuals
contents vice-MAN 
subproduct
tag Runtime
contents vice-RUN 
_END

# Make catalog/vice/pfiles/INFO
check=`cksum catalog/vice/pfiles/INDEX`
getsize $check

cat >catalog/vice/pfiles/INFO.1 <<_END
control_file
path INDEX
size $gotsize
mode 0444
mtime $DATESTRING
tag INDEX
control_file
path INFO
_END

echo >catalog/vice/pfiles/INFO.2 size xxx

cat >catalog/vice/pfiles/INFO.3 <<_END
mode 0444
mtime $DATESTRING
tag INFO
_END

check1=`cksum catalog/vice/pfiles/INFO.1`
check2=`cksum catalog/vice/pfiles/INFO.2`
check3=`cksum catalog/vice/pfiles/INFO.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
totalsize=`expr $size1 + $size2 + $size3 + 60`
echo >catalog/vice/pfiles/INFO.2 size $totalsize
cat >catalog/vice/pfiles/INFO catalog/vice/pfiles/INFO.1 catalog/vice/pfiles/INFO.2 catalog/vice/pfiles/INFO.3
rm -f -r catalog/vice/pfiles/INFO.*

# Make catalog/vice/vice-MAN/INDEX.1|2|3 parts

cat >catalog/vice/vice-MAN/INDEX.1 <<_END
fileset
tag vice-MAN
data_model_revision 2.40
instance_id 1
control_directory vice-MAN
_END

echo >catalog/vice/vice-MAN/INDEX.2 size xxxxx

cat >catalog/vice/vice-MAN/INDEX.3 <<_END
mod_time $DATESTRING
create_time $DATESTRING
state available
_END

check1=`cksum catalog/vice/vice-MAN/INDEX.1`
check2=`cksum catalog/vice/vice-MAN/INDEX.2`
check3=`cksum catalog/vice/vice-MAN/INDEX.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
indexsize=`expr $size1 + $size2 + $size3`

# Make catalog/vice/vice-MAN/INFO

cat >catalog/vice/vice-MAN/INFO.1 <<_END
control_file
path INDEX
size $indexsize
mode 0444
mtime $DATESTRING
tag INDEX
control_file
path INFO
_END

echo >catalog/vice/vice-MAN/INFO.2 size xxxx

cat >catalog/vice/vice-MAN/INFO.0 <<_END
mode 0444
mtime $DATESTRING
tag INFO
_END

cat >catalog/vice/vice-MAN/INFO.3 catalog/vice/vice-MAN/INFO.0 mantmp
rm -f mantmp

check1=`cksum catalog/vice/vice-MAN/INFO.1`
check2=`cksum catalog/vice/vice-MAN/INFO.2`
check3=`cksum catalog/vice/vice-MAN/INFO.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
infosize=`expr $size1 + $size2 + $size3 + 59`

echo >catalog/vice/vice-MAN/INFO.2 size $infosize

cat >catalog/vice/vice-MAN/INFO catalog/vice/vice-MAN/INFO.1 catalog/vice/vice-MAN/INFO.2 catalog/vice/vice-MAN/INFO.3
rm -f catalog/vice/vice-MAN/INFO.*

# Make catalog/vice/vice-MAN/INDEX

mansize=`expr $mantotal + $indexsize + $infosize`
echo >catalog/vice/vice-MAN/INDEX.2 size $mansize

cat >catalog/vice/vice-MAN/INDEX catalog/vice/vice-MAN/INDEX.1 catalog/vice/vice-MAN/INDEX.2 catalog/vice/vice-MAN/INDEX.3
rm -f catalog/vice/vice-MAN/INDEX.*

# Make catalog/vice/vice-RUN/INDEX.1|2|3 parts

cat >catalog/vice/vice-RUN/INDEX.1 <<_END
fileset
tag vice-RUN
data_model_revision 2.40
instance_id 1
control_directory vice-RUN
_END

echo >catalog/vice/vice-RUN/INDEX.2 size xxxxxxxx

cat >catalog/vice/vice-RUN/INDEX.3 <<_END
mod_time $DATESTRING
create_time $DATESTRING
state available
_END

check1=`cksum catalog/vice/vice-RUN/INDEX.1`
check2=`cksum catalog/vice/vice-RUN/INDEX.2`
check3=`cksum catalog/vice/vice-RUN/INDEX.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
indexsize=`expr $size1 + $size2 + $size3`

# Make catalog/vice/vice-RUN/INFO

cat >catalog/vice/vice-RUN/INFO.1 <<_END
control_file
path INDEX
size $indexsize
mode 0444
mtime $DATESTRING
tag INDEX
control_file
path INFO
_END

echo >catalog/vice/vice-RUN/INFO.2 size xxxxx

cat >catalog/vice/vice-RUN/INFO.0 <<_END
mode 0444
mtime $DATESTRING
tag INFO
_END

cat >catalog/vice/vice-RUN/INFO.3 catalog/vice/vice-RUN/INFO.0 runtmp
rm -f runtmp

check1=`cksum catalog/vice/vice-RUN/INFO.1`
check2=`cksum catalog/vice/vice-RUN/INFO.2`
check3=`cksum catalog/vice/vice-RUN/INFO.3`
getsize $check1
size1=$gotsize
getsize $check2
size2=$gotsize
getsize $check3
size3=$gotsize
infosize=`expr $size1 + $size2 + $size3 + 59`

echo >catalog/vice/vice-RUN/INFO.2 size $infosize

cat >catalog/vice/vice-RUN/INFO catalog/vice/vice-RUN/INFO.1 catalog/vice/vice-RUN/INFO.2 catalog/vice/vice-RUN/INFO.3
rm -f catalog/vice/vice-RUN/INFO.*

# Make catalog/vice/vice-RUN/INDEX

runsize=`expr $runtotal + $indexsize + $infosize`
echo >catalog/vice/vice-RUN/INDEX.2 size $runsize

cat >catalog/vice/vice-RUN/INDEX catalog/vice/vice-RUN/INDEX.1 catalog/vice/vice-RUN/INDEX.2 catalog/vice/vice-RUN/INDEX.3
rm -f catalog/vice/vice-RUN/INDEX.*

cat >catalog/INDEX <<_END
distribution
layout_version 1.0
data_model_revision 2.40
uuid $UUID
mod_time $DATESTRING
create_time $DATESTRING
path_max 255
name_max 100
media 
sequence_number 1
product
tag vice
data_model_revision 2.40
instance_id 1
control_directory vice
revision $VICEVERSION
title "vice"
description "VICEplus is a program that runs on a Unix, MS-DOS, Win32, OS/2,
Acorn RISC OS, QNX 6.x, AmigaOS or BeOS machine and executes programs
intended for the old 8-bit computers. The current version emulates the C64,
the C128, the VIC20, all the PET models (except the SuperPET 9000, which is
out of line anyway), the PLUS4 and the CBM-II (aka C610).
mod_time $DATESTRING
create_time $DATESTRING
architecture $UNAMECONCAT 
machine_type $UNAMEM
os_name $UNAMES
os_release $UNAMER
os_version *
directory /usr/local
all_filesets vice-MAN vice-RUN 
is_locatable false
subproduct
tag Manuals
contents vice-MAN 
subproduct
tag Runtime
contents vice-RUN 
fileset
tag vice-MAN
data_model_revision 2.40
instance_id 1
control_directory vice-MAN
size $mansize
mod_time $DATESTRING
create_time $DTESTRING
state available
fileset
tag vice-RUN
data_model_revision 2.40
instance_id 1
control_directory vice-RUN
size $runsize
mod_time $DATESTRING
create_time $DATESTRING
state available
_END

if test x"$ZIPKIND" = "xzip"; then
  cputouse=`echo $CPU | cut -c1-4`
  ostouse=`echo $SYSTEM | cut -c5-`
  tar cf vice-$VICEVERSION-$cputouse-$ostouse.depot catalog vice
  gzip vice-$VICEVERSION-$cputouse-$ostouse.depot
  rm -f -r catalog vice
  echo HPUX port binary depot generated as vice-$VICEVERSION-$cputouse-$ostouse.depot.gz
else
  echo HPUX port binary destribution directory generated as catalog and and vice
fi
if test x"$ENABLEARCH" = "xyes"; then
  echo Warning: binaries are optimized for your system and might not run on a different system, use --enable-arch=no to avoid this
fi
