REM ----- Make dist -----

@Echo Off

xdel Vice2\*.*  /s /l /n /d

md "Vice2"
type install.cmd > Vice2\install.cmd
copy ..\..\..\data\x64.exe      Vice2
copy ..\..\..\data\x64dtv.exe   Vice2
copy ..\..\..\data\x64sc.exe    Vice2
copy ..\..\..\data\xscpu64.exe  Vice2
copy ..\..\..\data\x128.exe     Vice2
copy ..\..\..\data\xvic.exe     Vice2
copy ..\..\..\data\xpet.exe     Vice2
copy ..\..\..\data\xplus4.exe   Vice2
copy ..\..\..\data\xcbm2.exe    Vice2
copy ..\..\..\data\xcbm5x0.exe  Vice2
copy ..\..\..\data\vsid.exe     Vice2
copy ..\..\..\data\x1541.exe    Vice2
copy ..\..\..\data\petcat.exe   Vice2
copy ..\..\..\data\cartconv.exe Vice2
copy ..\..\..\data\vice2.dll    Vice2
copy ..\..\..\data\vice2.fon    Vice2

md "Vice2\Icons"
copy icons\folder1.ico Vice2\Icons
copy icons\folder2.ico Vice2\Icons
copy icons\c1541.ico   Vice2\Icons
copy icons\vsid.ico    Vice2\Icons
copy icons\x64.ico     Vice2\Icons
copy icons\x64dtv.ico  Vice2\Icons
copy icons\x64sc.ico   Vice2\Icons
copy icons\x128.ico    Vice2\Icons
copy icons\xcbm2.ico   Vice2\Icons
copy icons\xcbm5x0.ico Vice2\Icons
copy icons\xpet.ico    Vice2\Icons
copy icons\xplus4.ico  Vice2\Icons
copy icons\xscpu64.ico Vice2\Icons
copy icons\xvic.ico    Vice2\Icons

md Vice2\C64
copy ..\..\..\data\C64\basic        Vice2\C64
copy ..\..\..\data\C64\c64hq.vpl    Vice2\C64
copy ..\..\..\data\C64\c64mem.sym   Vice2\C64
copy ..\..\..\data\C64\c64s.vpl     Vice2\C64
copy ..\..\..\data\C64\ccs64.vpl    Vice2\C64
copy ..\..\..\data\C64\chargen      Vice2\C64
copy ..\..\..\data\C64\default.vpl  Vice2\C64
copy ..\..\..\data\C64\default.vrs  Vice2\C64
copy ..\..\..\data\C64\frodo.vpl    Vice2\C64
copy ..\..\..\data\C64\godot.vpl    Vice2\C64
copy ..\..\..\data\C64\kernal       Vice2\C64
copy ..\..\..\data\C64\os2.vkm      Vice2\C64
copy ..\..\..\data\C64\os2_sym.vkm  Vice2\C64
copy ..\..\..\data\C64\pc64.vpl     Vice2\C64
copy ..\..\..\data\C64\vice.vpl     Vice2\C64

md Vice2\C64DTV
copy ..\..\..\data\C64DTV\basic        Vice2\C64DTV
copy ..\..\..\data\C64DTV\c64mem.sym   Vice2\C64DTV
copy ..\..\..\data\C64DTV\c64s.vpl     Vice2\C64DTV
copy ..\..\..\data\C64DTV\ccs64.vpl    Vice2\C64DTV
copy ..\..\..\data\C64DTV\chargen      Vice2\C64DTV
copy ..\..\..\data\C64DTV\default.vpl  Vice2\C64DTV
copy ..\..\..\data\C64DTV\default.vrs  Vice2\C64DTV
copy ..\..\..\data\C64DTV\dtvrom.bin Vice2\C64DTV
copy ..\..\..\data\C64DTV\frodo.vpl    Vice2\C64DTV
copy ..\..\..\data\C64DTV\godot.vpl    Vice2\C64DTV
copy ..\..\..\data\C64DTV\kernal       Vice2\C64DTV
copy ..\..\..\data\C64DTV\os2.vkm      Vice2\C64DTV
copy ..\..\..\data\C64DTV\os2_sym.vkm  Vice2\C64DTV
copy ..\..\..\data\C64DTV\pc64.vpl     Vice2\C64DTV
copy ..\..\..\data\C64DTV\vice.vpl     Vice2\C64DTV

md Vice2\SCPU64
copy ..\..\..\data\SCPU64\c64hq.vpl     Vice2\SCPU64
copy ..\..\..\data\SCPU64\c64s.vpl      Vice2\SCPU64
copy ..\..\..\data\SCPU64\ccs64.vpl     Vice2\SCPU64
copy ..\..\..\data\SCPU64\chargen       Vice2\SCPU64
copy ..\..\..\data\SCPU64\default.vpl   Vice2\SCPU64
copy ..\..\..\data\SCPU64\default.vrs   Vice2\SCPU64
copy ..\..\..\data\SCPU64\frodo.vpl     Vice2\SCPU64
copy ..\..\..\data\SCPU64\godot.vpl     Vice2\SCPU64
copy ..\..\..\data\SCPU64\os2.vkm       Vice2\SCPU64
copy ..\..\..\data\SCPU64\os2_sym.vkm   Vice2\SCPU64
copy ..\..\..\data\SCPU64\pc64.vpl      Vice2\SCPU64
copy ..\..\..\data\SCPU64\scpu64        Vice2\SCPU64
copy ..\..\..\data\SCPU64\scpu64mem.sym Vice2\SCPU64
copy ..\..\..\data\SCPU64\vice.vpl      Vice2\SCPU64

md Vice2\C128
copy ..\..\..\data\C128\basic64      Vice2\C128
copy ..\..\..\data\C128\basiclo      Vice2\C128
copy ..\..\..\data\C128\basichi      Vice2\C128
copy ..\..\..\data\C128\c64s.vpl     Vice2\C128
copy ..\..\..\data\C128\ccs64.vpl    Vice2\C128
copy ..\..\..\data\C128\chargde      Vice2\C128
copy ..\..\..\data\C128\chargen      Vice2\C128
copy ..\..\..\data\C128\chargfr      Vice2\C128
copy ..\..\..\data\C128\chargse      Vice2\C128
copy ..\..\..\data\C128\chargch      Vice2\C128
copy ..\..\..\data\C128\default.vpl  Vice2\C128
copy ..\..\..\data\C128\default.vrs  Vice2\C128
copy ..\..\..\data\C128\frodo.vpl    Vice2\C128
copy ..\..\..\data\C128\godot.vpl    Vice2\C128
copy ..\..\..\data\C128\kernal       Vice2\C128
copy ..\..\..\data\C128\kernal64     Vice2\C128
copy ..\..\..\data\C128\kernalde     Vice2\C128
copy ..\..\..\data\C128\kernalfi     Vice2\C128
copy ..\..\..\data\C128\kernalfr     Vice2\C128
copy ..\..\..\data\C128\kernalit     Vice2\C128
copy ..\..\..\data\C128\kernalno     Vice2\C128
copy ..\..\..\data\C128\kernalse     Vice2\C128
copy ..\..\..\data\C128\kernalch     Vice2\C128
copy ..\..\..\data\C128\os2.vkm      Vice2\C128
copy ..\..\..\data\C128\pc64.vpl     Vice2\C128
copy ..\..\..\data\C128\vdc_deft.vpl Vice2\C128
copy ..\..\..\data\C128\vdc_comp.vpl Vice2\C128
copy ..\..\..\data\C128\vice.vpl     Vice2\C128

md Vice2\PET
copy ..\..\..\data\PET\amber.vpl     Vice2\PET
copy ..\..\..\data\PET\basic1        Vice2\PET
copy ..\..\..\data\PET\basic2        Vice2\PET
copy ..\..\..\data\PET\basic4        Vice2\PET
copy ..\..\..\data\PET\chargen       Vice2\PET
copy ..\..\..\data\PET\chargen.de    Vice2\PET
copy ..\..\..\data\PET\edit1g        Vice2\PET
copy ..\..\..\data\PET\edit2b        Vice2\PET
copy ..\..\..\data\PET\edit2g        Vice2\PET
copy ..\..\..\data\PET\edit4b40      Vice2\PET
copy ..\..\..\data\PET\edit4b80      Vice2\PET
copy ..\..\..\data\PET\edit4g40      Vice2\PET
copy ..\..\..\data\PET\green.vpl     Vice2\PET
copy ..\..\..\data\PET\kernal1       Vice2\PET
copy ..\..\..\data\PET\kernal2       Vice2\PET
copy ..\..\..\data\PET\kernal4       Vice2\PET
copy ..\..\..\data\PET\os2.vkm       Vice2\PET
copy ..\..\..\data\PET\os2_40.vkm    Vice2\PET
copy ..\..\..\data\PET\rom1g.vrs     Vice2\PET
copy ..\..\..\data\PET\rom2b.vrs     Vice2\PET
copy ..\..\..\data\PET\rom2g.vrs     Vice2\PET
copy ..\..\..\data\PET\rom4b40.vrs   Vice2\PET
copy ..\..\..\data\PET\rom4b80.vrs   Vice2\PET
copy ..\..\..\data\PET\rom4g40.vrs   Vice2\PET
copy ..\..\..\data\PET\white.vpl     Vice2\PET
copy ..\..\..\data\PET\*.bin         Vice2\PET

md Vice2\PLUS4
copy ..\..\..\data\PLUS4\3plus1hi      Vice2\PLUS4
copy ..\..\..\data\PLUS4\3plus1lo      Vice2\PLUS4
copy ..\..\..\data\PLUS4\basic         Vice2\PLUS4
copy ..\..\..\data\PLUS4\default.vpl   Vice2\PLUS4
copy ..\..\..\data\PLUS4\default.vrs   Vice2\PLUS4
copy ..\..\..\data\PLUS4\kernal        Vice2\PLUS4
copy ..\..\..\data\PLUS4\kernal.005    Vice2\PLUS4
copy ..\..\..\data\PLUS4\kernal.232    Vice2\PLUS4
copy ..\..\..\data\PLUS4\kernal.364    Vice2\PLUS4
copy ..\..\..\data\PLUS4\os2.vkm       Vice2\PLUS4
copy ..\..\..\data\PLUS4\vice.vpl      Vice2\PLUS4

md Vice2\VIC20
copy ..\..\..\data\VIC20\basic         Vice2\VIC20
copy ..\..\..\data\VIC20\chargen       Vice2\VIC20
copy ..\..\..\data\VIC20\default.vpl   Vice2\VIC20
copy ..\..\..\data\VIC20\default.vrs   Vice2\VIC20
copy ..\..\..\data\VIC20\kernal        Vice2\VIC20
copy ..\..\..\data\VIC20\os2.vkm       Vice2\VIC20
REM copy ..\..\..\data\VIC20\position.vkm  Vice2\VIC20

md Vice2\CBM-II
copy ..\..\..\data\CBM-II\amber.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\basic.128     Vice2\CBM-II
copy ..\..\..\data\CBM-II\basic.256     Vice2\CBM-II
copy ..\..\..\data\CBM-II\basic.500     Vice2\CBM-II
copy ..\..\..\data\CBM-II\c64s.vpl      Vice2\CBM-II
copy ..\..\..\data\CBM-II\ccs64.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\chargen.500   Vice2\CBM-II
copy ..\..\..\data\CBM-II\chargen.600   Vice2\CBM-II
copy ..\..\..\data\CBM-II\chargen.700   Vice2\CBM-II
copy ..\..\..\data\CBM-II\default.vpl   Vice2\CBM-II
copy ..\..\..\data\CBM-II\frodo.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\godot.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\green.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\kernal        Vice2\CBM-II
copy ..\..\..\data\CBM-II\kernal.500    Vice2\CBM-II
copy ..\..\..\data\CBM-II\os2.vkm       Vice2\CBM-II
copy ..\..\..\data\CBM-II\pc64.vpl      Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom128h.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom128l.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom256h.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom256l.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom500.vrs    Vice2\CBM-II
copy ..\..\..\data\CBM-II\white.vpl     Vice2\CBM-II

md Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1001       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1540       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1541       Vice2\DRIVES
copy ..\..\..\data\DRIVES\d1541II       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1551       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1570       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1571       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1571cr     Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1581       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos2000       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos4000       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos2031       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos2040       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos3040       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos4040       Vice2\DRIVES

md Vice2\PRINTER
copy ..\..\..\data\PRINTER\cbm1526      Vice2\PRINTER
copy ..\..\..\data\PRINTER\mps801       Vice2\PRINTER
copy ..\..\..\data\PRINTER\mps803       Vice2\PRINTER
copy ..\..\..\data\PRINTER\mps803.vpl   Vice2\PRINTER
copy ..\..\..\data\PRINTER\nl10-cbm     Vice2\PRINTER

md Vice2\doc
copy doc\Vice_for_OS2.txt      Vice2\doc

md Vice2\doc\readme
copy  ..\..\..\AUTHORS                Vice2\doc\readme
copy  ..\..\..\COPYING                Vice2\doc\readme
copy  ..\..\..\FEEDBACK               Vice2\doc\readme
copy  ..\..\..\NEWS                   Vice2\doc\readme
copy  ..\..\..\README                 Vice2\doc\readme

copy  ..\..\..\doc\vice.inf           Vice2\doc

md Vice2\doc\html
xcopy ..\..\..\doc\html\*.html Vice2\doc\html

del vice2.zip > NUL
zip -r -9 vice2 Vice2\*.*
