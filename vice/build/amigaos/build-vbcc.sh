#!/bin/sh
# build-vbcc.sh for the AmigaOS ports
#
# written by Marco van den Heuvel <blackystardust68@yahoo.com>
#
# build-vbcc.sh <vbcc-target>

compile_file()
{
  if [ ! -f $1.o ]; then
    echo "compiling $1"
    vc $STDCFLAGS $STDINCFLAGS -c $1 -o $1.o
    if [ ! -f $1.o ]; then
      echo "compile of $1 failed"
      exit 1
    fi
  else
    echo "$1 already compiled, skipping"
  fi
}

compile_file_no_optimize()
{
  if [ ! -f $1.o ]; then
    echo "compiling $1"
    vc $STDCFLAGSNO $STDINCFLAGS -c $1 -o $1.o
    if [ ! -f $1.o ]; then
      echo "compile of $1 failed"
      exit 1
    fi
  else
    echo "$1 already compiled, skipping"
  fi
}

ar_file()
{
  if test x"$TARGET_AR" = "xar"; then
    rm -f $srcdir/lib$1.a
    echo "generating lib$1.a"
    ar q lib$1.a *.o
    if [ ! -f lib$1.a ]; then
      echo "generation of lib$1.a failed"
      exit 1
    fi
    if text x"$1" != "xbase"; then
      mv lib$1.a $srcdir
    fi
  else
    rm -f $srcdir/$1.lib
    echo "generating $1.lib"
    cat >$srcdir/$1.lib *.o
  fi
}

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

if test x"$1" = "xaos68k"; then
  TARGET="+aos68k"
  TARGET_DEF="-DAOS68K_VBCC"
  TARGET_LIBM="-lmieee"
  TARGET_AR="cat"
  AMIGAFLAVOR="AmigaOS3-1.21"
fi

if test x"$1" = "xaosppc"; then
  TARGET="+aosppc"
  TARGET_DEF="-DAOSPPC_VBCC"
  TARGET_LIBM="-lm"
  TARGET_AR="ar"
  AMIGAFLAVOR="AmigaOS4-1.21"
fi

if test x"$1" = "xmorphos"; then
  TARGET="+morphos"
  TARGET_DEF="-DMORPHOS_VBCC"
  TARGET_LIBM="-lm"
  TARGET_AR="ar"
  AMIGAFLAVOR="MorphOS-1.21"
fi

if test x"$1" = "xpowerup"; then
  TARGET="+powerup"
  TARGET_DEF="-DPOWERUP_VBCC"
  TARGET_LIBM="-lm"
  TARGET_AR="ar"
  AMIGAFLAVOR="PowerUP-1.21"
fi

if test x"$1" = "xwarpos"; then
  TARGET="+warpos"
  TARGET_DEF="-DWARPOS_VBCC"
  TARGET_LIBM="-lm"
  TARGET_AR="cat"
  AMIGAFLAVOR="WarpOS-1.21"
fi

if test x"$1" = "xclean"; then
  for k in *.c.o lib*.a *.lib *.exe
  do
    for i in `find src -name $k`
    do
      rm -f -r $i
    done
  done
  exit 1
fi

if test x"$TARGET" = "x"; then
  echo "usage: build-vbcc.sh <vbcc-target>"
  echo ""
  echo "targets:"
  echo "aos68k  = AmigaOS3"
  echo "aosppc  = AmigaOS4"
  echo "morphos = MorphOS"
  echo "powerup = PowerUP"
  echo "warpos  = WarpOS"
  exit 1
fi

STDCFLAGS="$TARGET -c99 -g -O2 -DHAVE_CONFIG_H $TARGET_DEF -I."
STDCFLAGSNO="$TARGET -c99 -g -O0 -DHAVE_CONFIG_H $TARGET_DEF -I."
STDLDFLAGS="$TARGET -c99"

cd src
srcdir=`pwd`

if test x"$TARGET_AR" = "xar"; then
  rm -f $srcdir/libc1541.a $srcdir/libpetcat.a
else
  rm -f $srcdir/c1541.lib $srcdir/petcat.lib
fi

cp $srcdir/arch/amigaos/config.h.vbcc $srcdir/config.h

cd $srcdir

if [ ! -f translate.h ]; then
  echo "generating translate.h..."
  /bin/sh ./gentranslate_h.sh <./translate.txt >translate.h
fi

if [ ! -f translate_table.h ]; then
  echo "generating translate_table.h..."
  /bin/sh ./gentranslatetable.sh <./translate.txt >translate_table.h
fi

#sounddrv lib
cd $srcdir/sounddrv
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in sounddummy.c sounddump.c soundfs.c soundspeed.c soundwav.c soundahi.c
do
  compile_file $i
done
ar_file sounddrv

#driveiecc64exp lib
cd $srcdir/drive/iec/c64exp
STDINCFLAGS="-I../../../ -I../../../arch/amigaos -I../../../drive"
for i in c64exp-cmdline-options.c c64exp-resources.c iec-c64exp.c profdos.c
do
  compile_file $i
done
ar_file driveiecc64exp

#driveiecplus4exp lib
cd $srcdir/drive/iec/plus4exp
STDINCFLAGS="-I../../../ -I../../../arch/amigaos -I../../../drive"
for i in iec-plus4exp.c plus4exp-cmdline-options.c plus4exp-resources.c
do
  compile_file $i
done
ar_file driveiecplus4exp

#driveiec lib
cd $srcdir/drive/iec
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive"
for i in cia1571d.c cia1581d.c glue1571.c iec-cmdline-options.c \
         iec-resources.c iec.c iecrom.c mc6821.c memiec.c via1d1541.c wd1770.c
do
  compile_file $i
done
ar_file driveiec

#driveiec128dcr lib
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive"
cd $srcdir/drive/iec128dcr
for i in iec128dcr-cmdline-options.c iec128dcr-resources.c iec128dcr.c \
         iec128dcrrom.c
do
  compile_file $i
done
ar_file driveiec128dcr

#driveiecieee lib
cd $srcdir/drive/iecieee
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive"
for i in iecieee.c via2d.c
do
  compile_file $i
done
ar_file driveiecieee

#driveieee lib
cd $srcdir/drive/ieee
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive"
for i in fdc.c ieee-cmdline-options.c ieee-resources.c ieee.c ieeerom.c \
         memieee.c riot1d.c riot2d.c via1d2031.c
do
  compile_file $i
done
ar_file driveieee

#drivetcbm lib
cd $srcdir/drive/tcbm
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive"
for i in glue1551.c mem1551.c tcbm-cmdline-options.c tcbm-resources.c tcbm.c \
         tcbmrom.c tpid.c
do
  compile_file $i
done
ar_file drivetcbm

#drive lib
cd $srcdir/drive
STDINCFLAGS="-I../ -I../arch/amigaos -I../vdrive"
for i in drive-check.c drive-cmdline-options.c drive-overflow.c \
         drive-resources.c drive-snapshot.c drive-writeprotect.c drive.c \
         drivemem.c driveimage.c driverom.c drivesync.c rotation.c
do
  compile_file $i
done
compile_file_no_optimize drivecpu.c
ar_file drive

#vdrive lib
cd $srcdir/vdrive
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in vdrive-bam.c vdrive-command.c vdrive-dir.c vdrive-iec.c \
         vdrive-internal.c vdrive-rel.c vdrive-snapshot.c vdrive.c
do
  compile_file $i
done
ar_file vdrive

#fsdevice lib
cd $srcdir/fsdevice
STDINCFLAGS="-I../ -I../arch/amigaos -I../vdrive"
for i in fsdevice-close.c fsdevice-cmdline-options.c fsdevice-flush.c \
         fsdevice-open.c fsdevice-read.c fsdevice-resources.c \
         fsdevice-write.c fsdevice.c
do
  compile_file $i
done
ar_file fsdevice

#diskimage lib
cd $srcdir/diskimage
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in diskimage.c fsimage-check.c fsimage-create.c fsimage-gcr.c \
         fsimage-probe.c fsimage.c rawimage.c
do
  compile_file $i
done
ar_file diskimage

#iecbus lib
cd $srcdir/iecbus
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive"
compile_file iecbus.c
ar_file iecbus

#serial lib
cd $srcdir/serial
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive"
for i in fsdrive.c serial-device.c serial-iec-bus.c serial-iec-device.c \
         serial-iec-lib.c serial-iec.c serial-realdevice.c serial-trap.c \
         serial.c
do
  compile_file $i
done
ar_file serial

#part of c1541.lib
if test x"$TARGET_AR" = "xar"; then
  ar q libc1541.a fsdrive.c.o serial-device.c.o serial-iec.c.o serial-iec-bus.c.o serial-iec-lib.c.o serial-realdevice.c.o
  mv libc1541.a $srcdir
else
  cat >>$srcdir/c1541.lib fsdrive.c.o serial-device.c.o serial-iec.c.o serial-iec-bus.c.o serial-iec-lib.c.o serial-realdevice.c.o
fi

#parallel lib
cd $srcdir/parallel
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive"
for i in parallel-trap.c parallel.c
do
  compile_file $i
done
ar_file parallel

#tape lib
cd $srcdir/tape
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in t64.c tap.c tape-internal.c tape-snapshot.c tape.c tapeimage.c
do
  compile_file $i
done
ar_file tape

#part of c1541.lib
if test x"$TARGET_AR" = "xar"; then
  mv $srcdir/libc1541.a ./
  ar q libc1541.a t64.c.o tap.c.o tape-internal.c.o tapeimage.c.o
  mv libc1541.a $srcdir
else
  cat >>$srcdir/c1541.lib t64.c.o tap.c.o tape-internal.c.o tapeimage.c.o
fi

#imagecontents lib
cd $srcdir/imagecontents
STDINCFLAGS="-I../ -I../arch/amigaos -I../vdrive"
for i in diskcontents-block.c diskcontents-iec.c diskcontents.c \
         imagecontents.c tapecontents.c
do
  compile_file $i
done
ar_file imagecontents

#fileio lib
cd $srcdir/fileio
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in cbmfile.c fileio.c p00.c
do
  compile_file $i
done
ar_file fileio

#video lib
cd $srcdir/video
STDINCFLAGS="-I../ -I../arch/amigaos -I../raster"
for i in render1x1.c render1x1pal.c render1x2.c render2x2.c render2x2pal.c \
         renderscale2x.c renderyuv.c video-canvas.c video-cmdline-options.c \
         video-color.c video-render-1x2.c video-render-2x2.c \
         video-render-pal.c video-render.c video-resources-pal.c \
         video-resources.c video-viewport.c
do
  compile_file $i
done
ar_file video

#raster lib
STDINCFLAGS="-I../ -I../arch/amigaos"
cd $srcdir/raster
for i in raster-cache.c raster-canvas.c raster-changes.c \
         raster-cmdline-options.c raster-line-changes-sprite.c \
         raster-line-changes.c raster-line.c raster-modes.c \
         raster-resources.c raster-sprite.c raster-sprite-status.c \
         raster-sprite-cache.c raster.c
do
  compile_file $i
done
ar_file raster

#vicii lib
cd $srcdir/vicii
STDINCFLAGS="-I../ -I../arch/amigaos -I../raster -I../c64"
for i in vicii-badline.c vicii-cmdline-options.c vicii-color.c vicii-draw.c \
         vicii-fetch.c vicii-irq.c vicii-mem.c vicii-phi1.c vicii-resources.c \
         vicii-snapshot.c vicii-sprites.c vicii-timing.c vicii.c
do
  compile_file $i
done
ar_file vicii

#vdc lib
STDINCFLAGS="-I../ -I../arch/amigaos -I../raster"
cd $srcdir/vdc
for i in vdc-cmdline-options.c vdc-draw.c vdc-mem.c vdc-resources.c \
         vdc-snapshot.c vdc.c
do
  compile_file $i
done
ar_file vdc

#crtc lib
STDINCFLAGS="-I../ -I../arch/amigaos -I../raster"
cd $srcdir/crtc
for i in crtc-cmdline-options.c crtc-draw.c crtc-mem.c crtc-resources.c \
         crtc-snapshot.c crtc.c
do
  compile_file $i
done
ar_file crtc

#gfxoutputdrv lib
STDINCFLAGS="-I../ -I../arch/amigaos"
cd $srcdir/gfxoutputdrv
for i in bmpdrv.c gfxoutput.c iffdrv.c pcxdrv.c ppmdrv.c
do
  compile_file $i
done
ar_file gfxoutputdrv

#printerdrv
STDINCFLAGS="-I../ -I../arch/amigaos"
cd $srcdir/printerdrv
for i in driver-select.c drv-ascii.c drv-mps803.c drv-nl10.c \
         interface-serial.c interface-userport.c output-graphics.c \
         output-select.c output-text.c printer-serial.c printer-userport.c \
         printer.c
do
  compile_file $i
done
ar_file printerdrv

#rsdrv lib
cd $srcdir/rs232drv
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in rs232drv.c rsuser.c
do
  compile_file $i
done
ar_file rsdrv

#sid lib
cd $srcdir/sid
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in fastsid.c sid-cmdline-options.c sid-resources.c sid-snapshot.c sid.c
do
  compile_file $i
done
ar_file sid

#monitor lib
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive -I../vdrive"
cd $srcdir/monitor
for i in asm6502.c asmz80.c mon_assemble6502.c mon_assemblez80.c \
         mon_breakpoint.c mon_command.c mon_disassemble.c mon_drive.c \
         mon_file.c mon_memory.c mon_register6502.c mon_registerz80.c \
         mon_ui.c mon_util.c mon_lex.c mon_parse.c monitor.c
do
  compile_file $i
done
ar_file monitor

#core lib
cd $srcdir/core
STDINCFLAGS="-I../ -I../arch/amigaos"
for i in ciacore.c ciatimer.c riotcore.c tpicore.c viacore.c
do
  compile_file $i
done
ar_file core

#c64cart lib
cd $srcdir/c64/cart
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../c64 -I../../vicii"
for i in actionreplay3.c actionreplay.c atomicpower.c c64cart.c c64cartmem.c \
         comal80.c crt.c delaep256.c delaep64.c delaep7x8.c epyxfastload.c \
         expert.c final.c generic.c ide64.c kcs.c magicformel.c mikroass.c \
         retroreplay.c rexep256.c ross.c stb.c supergames.c supersnapshot.c \
         zaxxon.c
do
  compile_file $i
done
ar_file c64cart

#c64 lib
cd $srcdir/c64
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive -I../sid -I../tape -I../vicii"
for i in c64-cmdline-options.c c64-resources.c c64-snapshot.c c64.c \
         c64_256k.c c64acia1.c c64bus.c c64cia1.c c64cia2.c c64datasette.c \
         c64drive.c c64export.c c64fastiec.c c64iec.c c64io.c c64keyboard.c \
         c64mem.c c64meminit.c c64memlimit.c c64memrom.c c64memsnapshot.c \
         c64parallel.c c64pla.c c64printer.c c64rom.c c64romset.c c64rsuser.c \
         c64sound.c c64tpi.c c64video.c georam.c mmc64.c patchrom.c \
         plus256k.c plus60k.c psid.c ramcart.c reloc65.c reu.c tfe.c
do
  compile_file $i
done
ar_file c64

#c64c128 (cc) lib
if test x"$TARGET_AR" = "xar"; then
  echo "generating libcc.a"
  ar q libcc.a c64acia1.c.o c64bus.c.o c64cia2.c.o c64datasette.c.o \
               c64export.c.o c64iec.c.o c64io.c.o c64keyboard.c.o \
               c64meminit.c.o c64memrom.c.o c64printer.c.o c64pla.c.o \
               c64parallel.c.o c64rsuser.c.o c64sound.c.o c64tpi.c.o \
               georam.c.o mmc64.c.o patchrom.c.o ramcart.c.o reu.c.o tfe.c.o
  if [ ! -f libcc.a ]; then
    echo "generation of libcc.a failed"
    exit 1
  fi
  mv libcc.a $srcdir
else
  echo "generating cc.lib"
  cat >$srcdir/cc.lib c64acia1.c.o c64bus.c.o c64cia2.c.o c64datasette.c.o \
                      c64export.c.o c64iec.c.o c64io.c.o c64keyboard.c.o \
                      c64meminit.c.o c64memrom.c.o c64printer.c.o c64pla.c.o \
                      c64parallel.c.o c64rsuser.c.o c64sound.c.o c64tpi.c.o \
                      georam.c.o mmc64.c.o patchrom.c.o ramcart.c.o reu.c.o \
                      tfe.c.o 
fi

#c128 lib
cd $srcdir/c128
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive -I../c64 -I../sid -I../tape -I../vicii -I../vdc"
for i in c128-cmdline-options.c c128-resources.c c128-snapshot.c c128.c \
         c128cia1.c c128drive.c c128fastiec.c c128mem.c c128memlimit.c \
         c128memrom.c c128memsnapshot.c c128mmu.c c128rom.c c128romset.c \
         c128video.c daa.c functionrom.c z80mem.c
do
  compile_file $i
done
for i in c128cpu.c z80.c c128meminit.c
do
  compile_file_no_optimize $i
done
ar_file c128

#vic20 lib
cd $srcdir/vic20
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive -I../sid -I../tape -I../raster"
for i in vic-cmdline-options.c vic-color.c vic-draw.c vic-mem.c \
         vic-resources.c vic-snapshot.c vic.c vic20-cmdline-options.c \
         vic20-resources.c vic20-snapshot.c vic20.c vic20bus.c \
         vic20cartridge.c vic20datasette.c vic20drive.c vic20iec.c \
         vic20ieeevia1.c vic20ieeevia2.c vic20mem.c vic20memrom.c \
         vic20memsnapshot.c vic20printer.c vic20rom.c vic20romset.c \
         vic20rsuser.c vic20sound.c vic20via1.c vic20via2.c vic20video.c
do
  compile_file $i
done
ar_file vic20

#pet lib
cd $srcdir/pet
STDINCFLAGS="-I../ -I../arch/amigaos -I../crtc -I../drive -I../sid -I../tape"
for i in pet-cmdline-options.c pet-resources.c pet-snapshot.c pet.c \
         petacia1.c petbus.c petdatasette.c petdrive.c petiec.c petmem.c \
         petmemsnapshot.c petpia1.c petpia2.c petprinter.c petreu.c \
         petrom.c petromset.c petsound.c petvia.c petvideo.c
do
  compile_file $i
done
ar_file pet

#plus4 lib
cd $srcdir/plus4
STDINCFLAGS="-I../ -I../arch/amigaos -I../drive -I../sid -I../tape -I../raster"
for i in plus4-cmdline-options.c plus4-resources.c plus4-snapshot.c plus4.c \
         plus4acia.c plus4bus.c plus4cart.c plus4datasette.c plus4drive.c \
         plus4iec.c plus4mem.c plus4memcsory256k.c plus4memhannes256k.c \
         plus4memlimit.c plus4memrom.c plus4memsnapshot.c plus4parallel.c \
         plus4pio1.c plus4pio2.c plus4printer.c plus4rom.c plus4romset.c \
         plus4tcbm.c plus4video.c ted-badline.c ted-cmdline-options.c \
         ted-color.c ted-draw.c ted-fetch.c ted-irq.c ted-mem.c \
         ted-resources.c ted-snapshot.c ted-sound.c ted-timer.c ted-timing.c \
         ted.c
do
  compile_file $i
done
compile_file_no_optimize plus4cpu.c
ar_file plus4

#cbm2 lib
cd $srcdir/cbm2
STDINCFLAGS="-I../ -I../arch/amigaos -I../sid -I../crtc -I../vicii -I../drive -I../tape"
for i in cbm2-cmdline-options.c cbm2-resources.c cbm2-snapshot.c cbm2.c \
         cbm2acia1.c cbm2bus.c cbm2cia1.c cbm2datasette.c cbm2drive.c \
         cbm2iec.c cbm2mem.c cbm2memsnapshot.c cbm2printer.c cbm2rom.c \
         cbm2romset.c cbm2sound.c cbm2tpi1.c cbm2tpi2.c cbm2video.c
do
  compile_file $i
done
compile_file_no_optimize cbm2cpu.c
ar_file cbm2

cd $srcdir/arch/amigaos

if [ ! -f intl.h ]; then
  echo "generating intl.h..."
  /bin/sh ./genintl_h.sh <./intl.txt >intl.h
fi

if [ ! -f intl_table.h ]; then
  echo "generating intl_table.h..."
  /bin/sh ./genintltable.sh <./intl.txt >intl_table.h
fi

#arch lib
STDINCFLAGS="-I../../ -I../../arch/amigaos -I../../drive -I../../c64 -I../../c128 -I../../vic20 -I../../pet -I../../plus4 -I../../cbm2 -I../../sid -I../../vdrive"
for i in ahi.c amigamain.c archdep.c blockdev.c c128ui.c c64ui.c \
         catweaselmkiii.c cbm2ui.c console.c fullscreen.c hardsid.c intl.c \
         joy.c joyai.c joyll.c kbd.c mousedrv.c noinlineargs.c petui.c \
         plus4ui.c pointer.c rs232.c screen-shot.c signals.c statusbar.c \
         tfearch.c timer.c ui.c uiapi.c uiattach.c uic64cart.c uicart.c \
         uicmdline.c uicolor.c uilib.c uimon.c vic20ui.c video.c vsidui.c \
         vsyncapi.c mui/filereq.c mui/mui.c mui/uiacia.c mui/uic64_256k.c \
         mui/uic128settings.c mui/uidatasette.c mui/uicbm2settings.c \
         mui/uidrivec128.c mui/uidrivec64vic20.c mui/uidrivepetcbm2.c \
         mui/uidriveplus4.c mui/uifliplist.c mui/uigeoram.c mui/uiide64.c \
         mui/uijoystick.c mui/uijoystickll.c mui/uimmc64.c mui/uinetwork.c \
         mui/uipetreu.c mui/uipetsettings.c mui/uiplus256k.c \
         mui/uiplus4settings.c mui/uiplus60k.c mui/uiram.c mui/uiramcart.c \
         mui/uireu.c mui/uiromc128settings.c mui/uiromc64vic20settings.c \
         mui/uiromcbm2settings.c mui/uirompetsettings.c \
         mui/uiromplus4settings.c mui/uirs232user.c mui/uisid.c \
         mui/uisidcart.c mui/uisnapshot.c mui/uisound.c mui/uivic20mem.c \
         mui/uivicii.c mui/uivideoc128.c mui/uivideoc64plus4vic20.c \
         mui/uivideocbm2pet.c
do
  compile_file $i
done
if test x"$TARGET_AR" = "xar"; then
  echo "generating libarch.a"
  ar q libarch.a *.o mui/*.o
  if [ ! -f libarch.a ]; then
    echo "generation of libarch.a failed"
    exit 1
  fi
  mv libarch.a $srcdir
else
  echo "generating arch.lib"
  cat >$srcdir/arch.lib *.o mui/*.o
fi

#part of c1541.lib
if test x"$TARGET_AR" = "xar"; then
  mv $srcdir/libc1541.a ./
  ar q libc1541.a archdep.c.o blockdev.c.o
  mv libc1541.a $srcdir
else
  cat >>$srcdir/c1541.lib archdep.c.o blockdev.c.o
fi

#part of petcat.lib
if test x"$TARGET_AR" = "xar"; then
  ar q libpetcat.a archdep.c.o
  mv libpetcat.a $srcdir
else
  cat >>$srcdir/petcat.lib archdep.c.o
fi

#base lib
cd $srcdir
STDINCFLAGS="-Iarch/amigaos -Idrive -Ivdrive"
for i in alarm.c attach.c autostart.c charset.c clkguard.c cmdline.c \
         cbmdos.c cbmimage.c color.c crc32.c datasette.c debug.c dma.c \
         emuid.c event.c findpath.c fliplist.c gcr.c info.c init.c \
         initcmdline.c interrupt.c ioutil.c joystick.c kbdbuf.c keyboard.c \
         lib.c log.c machine-bus.c machine.c mouse.c network.c palette.c \
         ram.c rawfile.c resources.c romset.c screenshot.c snapshot.c \
         sound.c sysfile.c translate.c traps.c util.c vsync.c zfile.c \
         zipcode.c memcmp.c
do
  compile_file $i
done
rm -f -r maincpu.c.o main.c.o c1541.c.o petcat.c.o cartconv.c.o
ar_file base

#part of c1541.lib
if test x"$TARGET_AR" = "xar"; then
  ar q libc1541.a cbmdos.c.o cbmimage.c.o charset.c.o gcr.c.o info.c.o ioutil.c.o lib.c.o log.c.o rawfile.c.o resources.c.o util.c.o zfile.c.o zipcode.c.o
else
  cat >>c1541.lib cbmdos.c.o cbmimage.c.o charset.c.o gcr.c.o info.c.o ioutil.c.o lib.c.o log.c.o rawfile.c.o resources.c.o util.c.o zfile.c.o zipcode.c.o
fi

#part of petcat.lib
if test x"$TARGET_AR" = "xar"; then
  ar q libpetcat.a ioutil.c.o lib.c.o log.c.o resources.c.o util.c.o
else
  cat >>petcat.lib ioutil.c.o lib.c.o log.c.o resources.c.o util.c.o
fi

compile_file main.c

compile_file_no_optimize maincpu.c

echo "generating x64.exe"
rm -f x64.exe
vc $STDLDFLAGS -o x64.exe main.c.o maincpu.c.o -larch -lbase -lc64 -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -lparallel -liecbus -lcore -lc64cart -lvicii -lraster -lrsdrv -lsid -ldriveieee -ldriveiec -ldriveiecieee -ldriveiecc64exp -lsounddrv -lfileio -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f x64.exe ]; then
  echo "linking of x64.exe failed"
  exit 1
fi

echo "generating x128.exe"
rm -f x128.exe
vc $STDLDFLAGS -o x128.exe main.c.o -larch -lbase -lc128 -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -lcc -lparallel -liecbus -lcore -lc64cart -lvicii -lsid -lrsdrv -lvdc -lraster -ldriveieee -ldriveiec -ldriveiecieee -ldriveiec128dcr -ldriveiecc64exp -lsounddrv -lfileio -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f x128.exe ]; then
  echo "linking of x128.exe failed"
  exit 1
fi

echo "generating xvic.exe"
rm -f xvic.exe
vc $STDLDFLAGS -o xvic.exe main.c.o maincpu.c.o -larch -lbase -lvic20 -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -lparallel -liecbus -lcore -ldriveieee -ldriveiec -ldriveiecieee -lrsdrv -lraster -lsounddrv -lsid -lfileio -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f xvic.exe ]; then
  echo "linking of xvic.exe failed"
  exit 1
fi

echo "generating xpet.exe"
rm -f xpet.exe
vc $STDLDFLAGS -o xpet.exe main.c.o maincpu.c.o -larch -lbase -lpet -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -lparallel -ldriveieee -lcore -ldriveiec -ldriveiecieee -lcrtc -lraster -lsounddrv -lsid -lfileio -lrsdrv -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f xpet.exe ]; then
  echo "linking of xpet.exe failed"
  exit 1
fi

echo "generating xplus4.exe"
rm -f xplus4.exe
vc $STDLDFLAGS -o xplus4.exe main.c.o -larch -lbase -lplus4 -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -liecbus -lcore -ldriveiec -ldriveiecplus4exp -ldrivetcbm -ldriveiecieee -lraster -lsid -lsounddrv -lfileio -lrsdrv -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f xplus4.exe ]; then
  echo "linking of xplus4.exe failed"
  exit 1
fi

echo "generating xcbm2.exe"
rm -f xcbm2.exe
vc $STDLDFLAGS -o xcbm2.exe main.c.o -larch -lbase -lcbm2 -lfsdevice -lprinterdrv -lgfxoutputdrv -ltape -lmonitor -ldrive -ldiskimage -lvideo -limagecontents -lvdrive -lserial -lparallel -ldriveieee -ldriveiec -ldriveiecieee -lcore -lsounddrv -lsid -lfileio -lvicii -lcrtc -lraster -lrsdrv -lposix -lamiga $TARGET_LIBM -lauto
if [ ! -f xcbm2.exe ]; then
  echo "linking of xcbm2.exe failed"
  exit 1
fi

compile_file c1541.c

echo "generating c1541.exe"
rm -f c1541.exe
vc $STDLDFLAGS -o c1541.exe c1541.c.o -lc1541 -lfileio -limagecontents -lvdrive -ldiskimage -lposix -lamiga -lauto
if [ ! -f c1541.exe ]; then
  echo "linking of c1541.exe failed"
  exit 1
fi

compile_file petcat.c

echo "generating petcat.exe"
vc $STDLDFLAGS -o petcat.exe petcat.c.o -lpetcat -lposix -lamiga -lauto
if [ ! -f petcat.exe ]; then
  echo "linking of petcat.exe failed"
  exit 1
fi

compile_file cartconv.c

echo "generating cartconv.exe"
vc $STDLDFLAGS -o cartconv.exe cartconv.c.o -lposix -lamiga -lauto
if [ ! -f cartconv.exe ]; then
  echo "linking of cartconv.exe failed"
  exit 1
fi

cd ..

rm -f -r VICE-$AMIGAFLAVOR VICE-$AMIGAFLAVOR.info
mkdir VICE-$AMIGAFLAVOR
cp src/*.exe VICE-$AMIGAFLAVOR
cp -a data/C128 data/C64 data/CBM-II data/DRIVES VICE-$AMIGAFLAVOR
cp -a data/PET data/PLUS4 data/PRINTER data/VIC20 VICE-$AMIGAFLAVOR
cp -a data/fonts VICE-$AMIGAFLAVOR
cp -a doc/html VICE-$AMIGAFLAVOR
cp FEEDBACK README doc/cartconv.txt VICE-$AMIGAFLAVOR
cp src/arch/amigaos/README.AMIGA VICE-$AMIGAFLAVOR
cp src/arch/amigaos/info-files/VICE.info VICE-$AMIGAFLAVOR.info
cp src/arch/amigaos/info-files/*.exe.info VICE-$AMIGAFLAVOR
rm `find VICE-$AMIGAFLAVOR -name "Makefile*"`
rm `find VICE-$AMIGAFLAVOR -name "dos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "os2*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "beos_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "win_*.vkm"`
rm `find VICE-$AMIGAFLAVOR -name "x11_*.vkm"`
rm VICE-$AMIGAFLAVOR/html/texi2html
echo AMIGA port binary destribution directory generated as VICE-$AMIGAFLAVOR
