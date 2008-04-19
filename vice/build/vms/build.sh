#!/bin/sh

# see if we are in the top of the tree
if [ ! -f configure.in ]; then
  cd ../..
  if [ ! -f configure.in ]; then
    echo "please run this script from the base of the VICE directory"
    echo "or from the appropriate build directory"
    exit 1
  fi
fi

compile_file()
{
  echo "compiling $1.c"
  if test x"$PLATFORM" = "xVAX"; then
    if [ -f $1.obj ]; then
      echo "$1.c already compiled, skipping"
    else
      cc $1.c $STDINCFLAGS
      if [ ! -f $1.obj ]; then
        echo "compile of $1.c failed"
        exit 1
      fi
    fi
  else
    if [ -f $1.o ]; then
      echo "$1.c already compiled, skipping"
    else
      gcc -c $1.c $STDINCFLAGS
      if [ ! -f $1.o ]; then
        echo "compile of $1.c failed"
        exit 1
      fi
    fi
  fi
}

delete_dir()
{
  if test x"$PLATFORM" = "xVAX"; then
    del \[.$1\]*.*\;*
    del $1.dir\;
  else
    rm -f -r $1
  fi
}

delete_objects()
{
  if test x"$PLATFORM" = "xVAX"; then
    touch dummy.a
    del *.a\;*
  else
    rm -f *.a
  fi
}

move_lib()
{
  if test x"$PLATFORM" = "xIA64"; then
    mv lib$1.a $2/$1.olb
  else
    mv lib$1.a $2
  fi
}

# Alpha, VAX or IA64
PLATFORM=`uname -p`

cd src
delete_dir vmslibs
mkdir vmslibs
cp arch/unix/vms/config.h ./
cp arch/unix/x11/xaw/videoarch.h arch/unix/x11/xaw/x11xawvideoarch.h
cp arch/unix/x11/xaw/uiarch.h arch/unix/x11/xaw/xawuiarch.h
cp arch/unix/x11/xaw/uimenu.h arch/unix/x11/xaw/xawuimenu.h

cd c128
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.c128],[-],[-.arch.unix],[-.drive],[-.c64],[-.sid],[-.tape],[-.vicii],[-.vdc])"
  OBJECTS="*.obj"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive -I../c64 -I../sid -I../tape -I../vicii -I../vdc"
  OBJECTS="*.o"
fi
for i in c128-cmdline-options c128-resources c128-snapshot c128 c128cia1 \
         c128cpu c128drive c128fastiec c128mem c128meminit c128memlimit \
         c128memrom c128memsnapshot c128mmu c128rom c128romset c128video daa \
         functionrom z80mem
do
  compile_file $i
done

# Compile z80.c seperately, and replace with dummy if memory runs out.
echo "compiling z80.c"
if test x"$PLATFORM" = "xVAX"; then
cc z80.c $STDINCFLAGS
if [ ! -f $1.obj ]; then
  cc z80vms.c $STDINCFLAGS
  if [ ! z80vms.obj ]; then
     echo "compile of z80.c failed"
     exit 1
   fi
  fi
else
  gcc -c z80.c $STDINCFLAGS
  if [ ! -f z80.o ]; then
    echo "compile of z80.c failed"
    exit 1
  fi
fi
ar cru libc128.a $OBJECTS
move_lib c128 ../vmslibs

cd ../pet
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.pet],[-],[-.crtc],[-.drive],[-.sid],[-.tape],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../crtc -I../drive -I../sid -I../tape -I../arch/unix"
fi
for i in pet-cmdline-options pet-resources pet-snapshot pet petacia1 petbus \
         petdatasette petdrive petiec petmem petmemsnapshot petpia1 petpia2 \
         petprinter petreu petrom petromset petsound petvia petvideo
do
  compile_file $i
done
ar cru libpet.a $OBJECTS
move_lib pet ../vmslibs

cd ../core
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.core],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in ciacore ciatimer riotcore tpicore viacore
do
  compile_file $i
done
ar cru libcore.a $OBJECTS
move_lib core ../vmslibs

cd ../crtc
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.crtc],[-],[-.arch.unix],[-.raster])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../raster"
fi
for i in crtc-cmdline-options crtc-draw crtc-mem crtc-resources crtc-snapshot \
         crtc
do
  compile_file $i
done
ar cru libcrtc.a $OBJECTS
move_lib crtc ../vmslibs

cd ../diskimage
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.diskimage],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in diskimage fsimage-check fsimage-create fsimage-gcr fsimage-probe \
         fsimage
do
  compile_file $i
done
ar cru libdiskimage.a $OBJECTS
move_lib diskimage ../vmslibs

cd ../drive
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.drive],[-],[-.arch.unix],[-.vdrive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../vdrive"
fi
for i in drive-check drive-cmdline-options drive-overflow drive-resources \
         drive-snapshot drive-writeprotect drive drivecpu drivemem driveimage \
         driverom drivesync rotation
do
  compile_file $i
done
ar cru libdrive.a $OBJECTS
move_lib drive ../vmslibs

cd iec
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.iec],[-],[--],[--.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix"
fi
for i in cia1571d cia1581d glue1571 iec-cmdline-options iec-resources iec \
         iecrom memiec via1d1541 wd1770
do
  compile_file $i
done
ar cru libdriveiec.a $OBJECTS
move_lib driveiec ../../vmslibs

cd c64exp
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.c64exp],[-],[--],[---],[---.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../.. -I../../../arch/unix"
fi
for i in c64exp-cmdline-options c64exp-resources iec-c64exp mc6821 profdos
do
  compile_file $i
done
ar cru libdriveiecc64exp.a $OBJECTS
move_lib driveiecc64exp ../../../vmslibs

cd ../plus4exp
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.plus4exp],[-],[--],[---],[---.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../.. -I../../../arch/unix"
fi
for i in iec-plus4exp plus4exp-cmdline-options plus4exp-resources
do
  compile_file $i
done
ar cru libdriveiecplus4exp.a $OBJECTS
move_lib driveiecplus4exp ../../../vmslibs

cd ../../iec128dcr
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.iec128dcr],[-],[--],[--.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix"
fi
for i in iec128dcr-cmdline-options iec128dcr-resources iec128dcr iec128dcrrom
do
  compile_file $i
done
ar cru libiec128dcr.a $OBJECTS
move_lib iec128dcr ../../vmslibs

cd ../iecieee
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.ieciee],[-],[--],[--.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix"
fi
for i in iecieee via2d
do
  compile_file $i
done
ar cru libiecieee.a $OBJECTS
move_lib iecieee ../../vmslibs

cd ../ieee
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.ieee],[-],[--],[--.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix"
fi
for i in fdc ieee-cmdline-options ieee-resources ieee ieeerom memieee riot1d \
         riot2d via1d2031
do
  compile_file $i
done
ar cru libieee.a $OBJECTS
move_lib ieee ../../vmslibs

cd ../tcbm
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.tcbm],[-],[--],[--.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix"
fi
for i in glue1551 mem1551 tcbm-cmdline-options tcbm-resources tcbm tcbmrom tpid
do
  compile_file $i
done
ar cru libtcbm.a $OBJECTS
move_lib tcbm ../../vmslibs

cd ../../fileio
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.fileio],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in cbmfile fileio p00
do
  compile_file $i
done
ar cru libfileio.a $OBJECTS
move_lib fileio ../vmslibs

cd ../fsdevice
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.fsdevice],[-],[-.arch.unix],[-.vdrive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../vdrive"
fi
for i in fsdevice-close fsdevice-cmdline-options fsdevice-flush fsdevice-open \
         fsdevice-read fsdevice-resources fsdevice-write fsdevice
do
  compile_file $i
done
ar cru libfsdevice.a $OBJECTS
move_lib fsdevice ../vmslibs

cd ../gfxoutputdrv
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.gfxoutputdrv],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in bmpdrv gfxoutput iffdrv pcxdrv ppmdrv
do
  compile_file $i
done
ar cru libgfxoutputdrv.a $OBJECTS
move_lib gfxoutputdrv ../vmslibs

cd ../iecbus
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.iecbus],[-],[-.arch.unix],[-.drive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive"
fi
for i in iecbus
do
  compile_file $i
done
ar cru libiecbus.a $OBJECTS
move_lib iecbus ../vmslibs

cd ../imagecontents
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.imagecontents],[-],[-.arch.unix],[-.vdrive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../vdrive"
fi
for i in diskcontents-block diskcontents-iec diskcontents imagecontents \
         tapecontents
do
  compile_file $i
done
ar cru libimagecontents.a $OBJECTS
move_lib imagecontents ../vmslibs

cd ../monitor
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.monitor],[-],[-.arch.unix],[-.drive],[-.vdrive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive -I../vdrive"
fi
for i in asm6502 asmz80 mon_assemble6502 mon_assemblez80 mon_breakpoint \
         mon_command mon_disassemble mon_drive mon_file mon_memory \
         mon_register6502 mon_registerz80 mon_ui mon_util mon_lex mon_parse \
         monitor
do
  compile_file $i
done
ar cru libmonitor.a $OBJECTS
move_lib monitor ../vmslibs

cd ../parallel
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.parallel],[-],[-.arch.unix],[-.drive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive"
fi
for i in parallel-trap parallel
do
  compile_file $i
done
ar cru libparallel.a $OBJECTS
move_lib parallel ../vmslibs

cd ../printerdrv
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.printerdrv],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in driver-select drv-ascii drv-mps803 drv-nl10 interface-serial \
         interface-userport output-graphics output-select output-text \
         printer-serial printer-userport printer
do
  compile_file $i
done
ar cru libprinterdrv.a $OBJECTS
move_lib printerdrv ../vmslibs

cd ../raster
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.raster],[-],[-.arch.unix],[-.arch.unix.x11.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../arch/unix/x11/xaw"
fi
for i in raster-cache raster-canvas raster-changes raster-cmdline-options \
         raster-line-changes-sprite raster-line-changes raster-line \
         raster-modes raster-resources raster-sprite raster-sprite-status \
         raster-sprite-cache raster
do
  compile_file $i
done
ar cru libraster.a $OBJECTS
move_lib raster ../vmslibs

cd ../rs232drv
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.rs232drv],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in rs232drv rsuser
do
  compile_file $i
done
ar cru librs232drv.a $OBJECTS
move_lib rs232drv ../vmslibs

cd ../serial
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.serial],[-],[-.arch.unix],[-.drive])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive"
fi
for i in fsdrive serial-device serial-iec-bus serial-iec-device \
         serial-iec-lib serial-iec serial-realdevice serial-trap serial
do
  compile_file $i
done
ar cru libserial.a $OBJECTS
move_lib serial ../vmslibs

cd ../sid
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.sid],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in fastsid sid-cmdline-options sid-resources sid-snapshot sid
do
  compile_file $i
done
ar cru libsid.a $OBJECTS
move_lib sid ../vmslibs

cd ../sounddrv
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.sounddrv],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in soundaiff sounddummy sounddump soundfs soundiff soundspeed soundvoc \
         soundwav
do
  compile_file $i
done
ar cru libsounddrv.a $OBJECTS
move_lib sounddrv ../vmslibs

cd ../tape
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.tape],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in t64 tap tape-internal tape-snapshot tape tapeimage
do
  compile_file $i
done
ar cru libtape.a $OBJECTS
move_lib tape ../vmslibs

cd ../vdc
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.vdc],[-],[-.arch.unix],[-.raster])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../raster"
fi
for i in vdc-cmdline-options vdc-draw vdc-mem vdc-resources vdc-snapshot vdc
do
  compile_file $i
done
ar cru libvdc.a $OBJECTS
move_lib vdc ../vmslibs

cd ../vdrive
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.vdrive],[-],[-.arch.unix])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix"
fi
for i in vdrive-bam vdrive-command vdrive-dir vdrive-iec vdrive-internal \
         vdrive-rel vdrive-snapshot vdrive
do
  compile_file $i
done
ar cru libvdrive.a $OBJECTS
move_lib vdrive ../vmslibs

cd ../vicii
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.vicii],[-],[-.arch.unix],[-.raster],[-.c64],[-.arch.unix.x11.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../raster -I../c64 -I../arch/unix/x11/xaw"
fi
for i in vicii-badline vicii-clock-stretch vicii-cmdline-options vicii-color \
         vicii-draw vicii-fetch vicii-irq vicii-mem vicii-phi1 \
         vicii-resources vicii-snapshot vicii-sprites vicii-timing vicii
do
  compile_file $i
done
ar cru libvicii.a $OBJECTS
move_lib vicii ../vmslibs

cd ../video
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.video],[-],[-.arch.unix],[-.raster],[-.arch.unix.x11.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../raster -I../arch/unix/x11/xaw"
fi
for i in render1x1 render1x1pal render1x2 render2x2 render2x2pal \
         renderscale2x renderyuv video-canvas video-cmdline-options \
         video-color video-render-1x2 video-render-2x2 video-render-pal \
         video-render video-resources-pal video-resources video-viewport
do
  compile_file $i
done
ar cru libvideo.a $OBJECTS
move_lib video ../vmslibs

cd ../c64
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.c64],[-],[-.arch.unix],[-.drive],[-.sid],[-.tape],[-.vicii])"
  OBJECTS128="c64acia1.obj c64bus.obj c64cia2.obj c64datasette.obj c64export.obj c64iec.obj c64io.obj c64keyboard.obj c64meminit.obj c64memrom.obj c64printer.obj c64pla.obj c64parallel.obj c64rsuser.obj c64sound.obj c64tpi.obj digimax.obj georam.obj mmc64.obj patchrom.obj ramcart.obj reu.obj"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive -I../sid -I../tape -I../vicii"
  OBJECTS128="c64acia1.o c64bus.o c64cia2.o c64datasette.o c64export.o c64iec.o c64io.o c64keyboard.o c64meminit.o c64memrom.o c64printer.o c64pla.o c64parallel.o c64rsuser.o c64sound.o c64tpi.o digimax.o georam.o mmc64.o patchrom.o ramcart.o reu.o"
fi
for i in c64-cmdline-options c64-resources c64-snapshot c64 c64_256k c64acia1 \
         c64bus c64cia1 c64cia2 c64datasette c64drive c64export c64fastiec \
         c64iec c64io c64keyboard c64mem c64meminit c64memlimit c64memrom \
         c64memsnapshot c64parallel c64pla c64printer c64rom c64romset \
         c64rsuser c64sound c64tpi c64video digimax georam mmc64 patchrom \
         plus256k plus60k psid ramcart reloc65 reu
do
  compile_file $i
done
ar cru libc64.a $OBJECTS
move_lib c64 ../vmslibs
ar cru libc128c64.a $OBJECTS128
move_lib c128c64 ../vmslibs

cd cart
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.cart],[-],[--],[--.arch.unix],[--.vicii])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../arch/unix -I../../vicii"
fi
for i in actionreplay3 actionreplay atomicpower c64cart c64cartmem comal80 \
         crt delaep256 delaep64 delaep7x8 epyxfastload expert final generic \
         ide64 kcs magicformel mikroass retroreplay rexep256 ross stb \
         supergames supersnapshot zaxxon
do
  compile_file $i
done
ar cru libc64cart.a $OBJECTS
move_lib c64cart ../../vmslibs

cd ../../cbm2
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.cbm2],[-],[-.arch.unix],[-.sid],[-.crtc],[-.vicii],[-.drive],[-.tape])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../sid -I../crtc -I../vicii -I../drive -I../tape"
fi
for i in cbm2-cmdline-options cbm2-resources cbm2-snapshot cbm2 cbm2acia1 \
         cbm2bus cbm2cia1 cbm2cpu cbm2datasette cbm2drive cbm2iec cbm2mem \
         cbm2memsnapshot cbm2printer cbm2rom cbm2romset cbm2sound cbm2tpi1 \
         cbm2tpi2 cbm2video
do
  compile_file $i
done
ar cru libcbm2.a $OBJECTS
move_lib cbm2 ../vmslibs

cd ../plus4
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.plus4],[-],[-.arch.unix],[-.drive],[-.sid],[-.tape],[-.raster],[-.arch.unix.x11.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive -I../sid -I../tape -I../raster -I../arch/unix/x11/xaw"
fi
for i in plus4-cmdline-options plus4-resources plus4-snapshot plus4 plus4acia \
         plus4bus plus4cart plus4cpu plus4datasette plus4drive plus4iec \
         plus4mem plus4memcsory256k plus4memhannes256k plus4memlimit \
         plus4memrom plus4memsnapshot plus4parallel plus4pio1 plus4pio2 \
         plus4printer plus4rom plus4romset plus4tcbm plus4video ted-badline \
         ted-cmdline-options ted-color ted-draw ted-fetch ted-irq ted-mem \
         ted-resources ted-snapshot ted-sound ted-timer ted-timing ted
do
  compile_file $i
done
ar cru libplus4.a $OBJECTS
move_lib plus4 ../vmslibs

cd ../vic20
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.vic20],[-],[-.arch.unix],[-.drive],[-.sid],[-.tape],[-.raster],[-.arch.unix.x11.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../arch/unix -I../drive -I../sid -I../tape -I../raster -I../arch/unix/x11/xaw"
fi
for i in vic-cmdline-options vic-color vic-draw vic-mem vic-resources \
         vic-snapshot vic vic20-cmdline-options vic20-resources \
         vic20-snapshot vic20 vic20bus vic20cartridge vic20datasette \
         vic20drive vic20iec vic20ieeevia1 vic20ieeevia2 vic20mem vic20memrom \
         vic20memsnapshot vic20printer vic20rom vic20romset vic20rsuser \
         vic20sound vic20via1 vic20via2 vic20video
do
  compile_file $i
done
ar cru libvic20.a $OBJECTS
move_lib vic20 ../vmslibs

cd ../arch/unix
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.unix],[-],[--],[--.drive],[--.c64],[--.c128],[--.vic20],[--.pet],[--.cbm2],[--.sid])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../drive -I../../c64 -I../../c128 -I../../vic20 -I../../pet -I../../cbm2 -I../../sid"
fi
for i in archdep blockdev console coproc joy joy_usb mousedrv rs232 signals \
         unixmain vsyncarch
do
  compile_file $i
done
ar cru libarch.a $OBJECTS
move_lib arch ../../vmslibs

cd gui
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.gui],[-],[---],[---.drive],[---.c64],[---.c128],[---.vic20],[---.pet],[---.cbm2],[---.sid],[-.x11],[-.x11.xaw])"
  OBJEXT=".obj"
else
  STDINCFLAGS="-I. -I.. -I../../.. -I../../../drive -I../../../c64 -I../../../c128 -I../../../vic20 -I../../../pet -I../../../cbm2 -I../../../sid -I../x11 -I../x11/xaw"
  OBJEXT=".o"
fi
for i in c128ui c64ui cbm2ui petui plus4ui ui uiacia1 uiattach uic64_256k \
         uic64cart uicmdline uicommands uicrtc uidatasette uidigimax uidrive \
         uidrivec128 uidrivec64 uidrivec64c128 uidrivec64vic20 uidriveiec \
         uidrivepetcbm2 uidriveplus4 uidrivevic20 uifliplist uigeoram uiide64 \
         uijoystick2 uikeyboard uilib uimmc64 uimon uimouse uinetplay \
         uipalemu uipalette uiperipheral uiperipheraliec uiperipheralieee \
         uipetreu uiplus256k uiplus60k uiprinter uiprinteriec \
         uiprinteriecplus4 uiprinterieee uiramcart uireu uiromset uirs232 \
         uirs232c64c128 uirs232petplus4cbm2 uirs232user uisettings uisid \
         uisidcart uisound uited uivdc uivic uivicii vic20ui vsidui
do
  compile_file $i
done
mv c128ui$OBJEXT ../../../vmslibs
mv c64ui$OBJEXT ../../../vmslibs
mv cbm2ui$OBJEXT ../../../vmslibs
ar cru libgui.a $OBJECTS
move_lib gui ../../../vmslibs

cd ../x11
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.x11],[-],[-.gui],[---],[.xaw])"
else
  STDINCFLAGS="-I. -I.. -I../gui -I../../.. -Ixaw"
fi
for i in c128icon c64icon cbm2icon fullscreen peticon plus4icon \
         ui-cmdline-options ui-resources vic20icon
do
  compile_file $i
done
ar cru libx11ui.a $OBJECTS
move_lib x11ui ../../../vmslibs

cd xaw
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.xaw],[-],[--],[----],[----.drive],[----.vdrive],[----.c64],[----.c128],[----.vic20],[----.pet],[----.cbm2],[--.gui])"
else
  STDINCFLAGS="-I. -I.. -I../.. -I../../../.. -I../../../../drive -I../../../../vdrive -I../../../../c64 -I../../../../c128 -I../../../../vic20 -I../../../../pet -I../../../../cbm2 -I../../gui"
fi
for i in about uicartridge uicolor uiedisk uimenu uiscreenshot uisnapshot \
         uivsidcontrol x11video x11kbd x11ui
do
  compile_file $i
done
ar cru libxawui.a $OBJECTS
move_lib xawui ../../../../vmslibs

cd widgets
delete_objects
if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.widgets],[--],[---],[-----])"
else
  STDINCFLAGS="-I. -I../.. -I../../.. -I../../../../.."
fi
for i in Canvas DirMgr Directory DrawImageString DrawString FileSel MultiList \
         RegExp ScrList Tablist2Tabs TextField TextWidth strnchr
do
  compile_file $i
done
ar cru libwidgets.a $OBJECTS
move_lib widgets ../../../../../vmslibs

cd ../../../../..
if test x"$PLATFORM" = "xVAX"; then
  touch libbdummy.a
  touch dummy.obj
  del *.obj\;*
  del libb*.a\;*
  STDINCFLAGS="/inc=([-.src],[.arch.unix],[.drive],[.vdrive],[.arch.unix.x11.xaw])"
else
  rm -f libb*.a
  rm -f *.o
  STDINCFLAGS="-I. -Iarch/unix -Idrive -Ivdrive -Iarch/unix/x11/xaw"
fi
for i in alarm attach autostart charset clkguard cmdline cbmdos cbmimage \
         color crc32 datasette debug dma emuid event findpath fliplist gcr \
         info init initcmdline interrupt ioutil joystick kbdbuf keyboard lib \
         log machine-bus machine mouse network palette ram rawfile resources \
         romset screenshot snapshot sound sysfile traps util vsync zfile \
         zipcode
do
  compile_file $i
done
ar cru libbase.a $OBJECTS
move_lib base vmslibs

compile_file main
compile_file maincpu

mv vmslibs/c128ui$OBJEXT ./
mv vmslibs/c64ui$OBJEXT ./
mv vmslibs/cbm2ui$OBJEXT ./

# Copy needed Xaw and Xvmsutils libs optionally put into the src dir in case
# the platform doesn't have them.
if [ -f libXaw.a ]; then
  cp libXaw.a vmslibs
fi

if [ -f Xaw.olb ]; then
  cp Xaw.olb vmslibs
fi

if [ -f libXvmsutils.a ]; then
  cp libXvmsutils.a vmslibs
fi

if [ -f Xvmsutils.olb ]; then
  cp Xvmsutils.olb vmslibs
fi

echo "linking x64.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o x64.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT c64ui$OBJEXT -lc64 \
                 -lgui -lrs232drv -ldriveiec -ldriveiecc64exp -lc64cart \
                 -lvicii -lraster -lbase -lfsdevice -lfileio -lsounddrv \
                 -larch -lx11ui -lxawui -lwidgets -lvideo -lbase -lgui \
                 -lx11ui -limagecontents -lxawui -lieee -liecieee -lc64 \
                 -lparallel -lsid -lserial -lgfxoutputdrv -lprinterdrv \
                 -liecbus -lcore -ldrive -lmonitor -lgui -lvdrive -ldiskimage \
                 -lbase -ltape -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
else
  gcc -o x64.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT c64ui$OBJEXT -lc64 -lgui -lrs232drv -ldriveiec -ldriveiecc64exp -lc64cart -lvicii -lraster -lbase -lfsdevice -lfileio -lsounddrv -larch -lx11ui -lxawui -lwidgets -lvideo -lbase -lgui -lx11ui -limagecontents -lxawui -lieee -liecieee -lc64 -lparallel -lsid -lserial -lgfxoutputdrv -lprinterdrv -liecbus -lcore -ldrive -lmonitor -lgui -lvdrive -ldiskimage -lbase -ltape -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f x64.exe ]; then
  echo "linking of x64.exe failed"
  exit 1
fi

echo "linking x128.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o x128.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT c128ui$OBJEXT -lgui \
                  -lx11ui -lxawui -lc128 -limagecontents -lmonitor -lvdc \
                  -liec128dcr -ldriveiec -lvicii -lc128 -lraster -lparallel \
                  -lieee -liecieee -ldrive -lvdrive -ldiskimage -lbase -lgui \
                  -lrs232drv -liecbus -lsid -lc128c64 -lbase -lx11ui \
                  -lfsdevice -ltape -lfileio -lbase -lsounddrv -lserial \
                  -liecbus -lgfxoutputdrv -lprinterdrv -ldriveiecc64exp \
                  -lc64cart -larch -lcore -lvideo -lwidgets -lXaw -lXt -lXmu \
                  -lXvmsutils -lXext -lX11
else
  gcc -o x128.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT c128ui$OBJEXT -lgui -lx11ui -lxawui -lc128 -limagecontents -lmonitor -lvdc -liec128dcr -ldriveiec -lvicii -lc128 -lraster -lieee -liecieee -lparallel -ldrive -lvdrive -ldiskimage -lbase -lgui -lrs232drv -liecbus -lsid -lc128c64 -lbase -lx11ui -lfsdevice -ltape -lfileio -lbase -lsounddrv -lserial -liecbus -lgfxoutputdrv -lprinterdrv -ldriveiecc64exp -lc64cart -larch -lcore -lvideo -lwidgets -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f x128.exe ]; then
  echo "linking of x128.exe failed"
  exit 1
fi

echo "linking xcbm2.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o xcbm2.exe -Lvmslibs main$OBJEXT cbm2ui$OBJEXT -lgui -limagecontents \
                   -lxawui -lwidgets -lbase -lfsdevice -lfileio -lsounddrv \
                   -larch -lcbm2 -lmonitor -lgui -lsid -lprinterdrv \
                   -lgfxoutputdrv -lvicii -lx11ui -lcrtc -lraster -lvideo \
                   -lparallel -lserial -lieee -liecieee -ltape -ldrive \
                   -lvdrive -ldiskimage -lbase -lcore -lrs232drv -lXaw -lXt \
                   -lXmu -lXvmsutils -lXext -lX11
else
  gcc -o xcbm2.exe -Lvmslibs main$OBJEXT cbm2ui$OBJEXT -lgui -limagecontents -lxawui -lwidgets -lbase -lfsdevice -lfileio -lsounddrv -larch -lcbm2 -lmonitor -lgui -lsid -lprinterdrv -lgfxoutputdrv -lvicii -lx11ui -lcrtc -lraster -lvideo -lserial -lieee -liecieee -lparallel -ltape -ldrive -lvdrive -ldiskimage -lbase -lcore -lrs232drv -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f xcbm2.exe ]; then
  echo "linking of xcbm.exe failed"
  exit 1
fi

echo "linking xpet.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o xpet.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT -lbase -lsounddrv \
                  -lfsdevice -lfileio -larch -lxawui -lgui -limagecontents \
                  -lpet -lsid -ltape -lcrtc -lraster -lvideo -lparallel \
                  -lserial -lieee -liecieee -lcore -lrs232drv -lxawui -ldrive \
                  -lmonitor -lgui -lx11ui -lvdrive -ldiskimage -lbase -larch \
                  -lprinterdrv -lgfxoutputdrv -lwidgets -lXaw -lXt -lXmu \
                  -lXvmsutils -lXext -lX11
else
  gcc -o xpet.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT -lbase -lsounddrv -lfsdevice -lfileio -larch -lxawui -lgui -limagecontents -lpet -lsid -ltape -lcrtc -lraster -lvideo -lserial -lieee -lparallel -liecieee -lcore -lrs232drv -lxawui -ldrive -lmonitor -lgui -lx11ui -lvdrive -ldiskimage -lbase -larch -lprinterdrv -lgfxoutputdrv -lwidgets -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f xpet.exe ]; then
  echo "linking of xpet.exe failed"
  exit 1
fi

echo "linking xplus4.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o xplus4.exe -Lvmslibs main$OBJEXT -larch -lgui -lx11ui -lbase \
                    -lsounddrv -lplus4 -lsid -ldriveiec -liecieee \
                    -ldriveiecplus4exp -ltcbm -ltape -lraster -lrs232drv \
                    -lmonitor -lgui -lx11ui -limagecontents -lserial -liecbus \
                    -lcore -lxawui -ldrive -lprinterdrv -lgfxoutputdrv \
                    -lvdrive -ldiskimage -lbase -lfsdevice -lfileio -lbase \
                    -larch -lvideo -lwidgets -lXaw -lXt -lXmu -lXvmsutils \
                    -lXext -lX11
else
  gcc -o xplus4.exe -Lvmslibs main$OBJEXT -larch -lgui -lx11ui -lbase -lsounddrv -lplus4 -lsid -ldriveiec -liecieee -ldriveiecplus4exp -ltcbm -ltape -lraster -lrs232drv -lmonitor -lgui -lx11ui -limagecontents -lserial -liecbus -lcore -lxawui -ldrive -lprinterdrv -lgfxoutputdrv -lvdrive -ldiskimage -lbase -lfsdevice -lfileio -lbase -larch -lvideo -lwidgets -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f xplus4.exe ]; then
  echo "linking of xplus4.exe failed"
  exit 1
fi

echo "linking xvic.exe"
if test x"$PLATFORM" = "xAlpha"; then
gcc -o xvic.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT -lgui -lx11ui -lsounddrv \
                -lvic20 -lraster -lvideo -lxawui -lbase -larch -lfsdevice \
                -lfileio -lmonitor -lgui -lx11ui -limagecontents -lxawui \
                -ltape -lvic20 -lrs232drv -lsid -ldriveiec -lparallel -lieee \
                -liecieee -lserial -liecbus -lcore -ldrive -lprinterdrv \
                -lgfxoutputdrv -lvdrive -ldiskimage -lbase -lsounddrv \
                -lwidgets -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
else
  gcc -o xvic.exe -Lvmslibs main$OBJEXT maincpu$OBJEXT -lgui -lx11ui -lsounddrv -lvic20 -lraster -lvideo -lxawui -lbase -larch -lfsdevice -lfileio -lmonitor -lgui -lx11ui -limagecontents -lxawui -ltape -lvic20 -lrs232drv -lsid -ldriveiec -lieee -liecieee -lparallel -lserial -liecbus -lcore -ldrive -lprinterdrv -lgfxoutputdrv -lvdrive -ldiskimage -lbase -lsounddrv -lwidgets -lXaw -lXt -lXmu -lXvmsutils -lXext -lX11
fi
if [ ! -f xvic.exe ]; then
  echo "linking of xvic.exe failed"
  exit 1
fi

if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.src])"
else
  STDINCFLAGS="-I."
fi
compile_file cartconv
echo "linking cartconv.exe"
gcc -o cartconv.exe cartconv$OBJEXT
if [ ! -f cartconv.exe ]; then
  echo "linking of cartconv.exe failed"
  exit 1
fi

if test x"$PLATFORM" = "xVAX"; then
  STDINCFLAGS="/inc=([-.src],[.arch.unix],[.vdrive])"
else
  STDINCFLAGS="-I. -Iarch/unix -Ivdrive"
fi
compile_file c1541
echo "linking c1541.exe"
if test x"$PLATFORM" = "xAlpha"; then
gcc -o c1541.exe c1541$OBJEXT cbmdos$OBJEXT charset$OBJEXT findpath$OBJEXT \
                 gcr$OBJEXT cbmimage$OBJEXT info$OBJEXT ioutil$OBJEXT \
                 lib$OBJEXT log$OBJEXT rawfile$OBJEXT resources$OBJEXT \
                 util$OBJEXT zfile$OBJEXT zipcode$OBJEXT \
                 arch/unix/archdep$OBJEXT -Lvmslibs -ltape -ldiskimage \
                 -limagecontents -lvdrive -lserial -lfileio
else
  gcc -o c1541.exe c1541$OBJEXT cbmdos$OBJEXT charset$OBJEXT findpath$OBJEXT gcr$OBJEXT cbmimage$OBJEXT info$OBJEXT ioutil$OBJEXT lib$OBJEXT log$OBJEXT rawfile$OBJEXT resources$OBJEXT util$OBJEXT zfile$OBJEXT zipcode$OBJEXT arch/unix/archdep$OBJEXT -Lvmslibs -ltape -ldiskimage -limagecontents -lvdrive -lserial -lfileio
fi
if [ ! -f c1541.exe ]; then
  echo "linking of c1541.exe failed"
  exit 1
fi

compile_file petcat
echo "linking petcat.exe"
if test x"$PLATFORM" = "xAlpha"; then
  gcc -o petcat.exe petcat$OBJEXT arch/unix/archdep$OBJEXT charset$OBJEXT \
                    findpath$OBJEXT ioutil$OBJEXT lib$OBJEXT log$OBJEXT \
                    rawfile$OBJEXT resources$OBJEXT util$OBJEXT zfile$OBJEXT \
                    zipcode$OBJEXT
else
  gcc -o petcat.exe petcat$OBJEXT arch/unix/archdep$OBJEXT charset$OBJEXT findpath$OBJEXT ioutil$OBJEXT lib$OBJEXT log$OBJEXT rawfile$OBJEXT resources$OBJEXT util$OBJEXT zfile$OBJEXT zipcode$OBJEXT
fi
if [ ! -f petcat.exe ]; then
  echo "linking of petcat.exe failed"
  exit 1
fi

echo $PLATFORM OPENVMS COMPILE COMPLETE
