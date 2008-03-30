REM ----- Make dist -----

@Echo Off

xdel Vice2\*.*  /s /l /n /d

md "Vice2"
copy install.cmd              Vice2
copy ..\..\..\data\x64.exe    Vice2
copy ..\..\..\data\x128.exe   Vice2
copy ..\..\..\data\xvic.exe   Vice2
copy ..\..\..\data\xpet.exe   Vice2
copy ..\..\..\data\xcbm2.exe  Vice2
copy ..\..\..\data\c1541.exe  Vice2
copy ..\..\..\data\petcat.exe Vice2

md "Vice2\Icons"
copy icons\folder1.ico Vice2\Icons
copy icons\folder2.ico Vice2\Icons

md Vice2\C64
copy ..\..\..\data\C64\basic        Vice2\C64
copy ..\..\..\data\C64\c64s.vpl     Vice2\C64
copy ..\..\..\data\C64\ccs64.vpl    Vice2\C64
copy ..\..\..\data\C64\chargen      Vice2\C64
copy ..\..\..\data\C64\default.vkm  Vice2\C64
copy ..\..\..\data\C64\default.vpl  Vice2\C64
copy ..\..\..\data\C64\default.vrs  Vice2\C64
copy ..\..\..\data\C64\frodo.vpl    Vice2\C64
copy ..\..\..\data\C64\godot.vpl    Vice2\C64
copy ..\..\..\data\C64\kernal       Vice2\C64
copy ..\..\..\data\C64\pc64.vpl     Vice2\C64
copy ..\..\..\data\C64\position.vkm Vice2\C64

md Vice2\C128
copy ..\..\..\data\C128\basic        Vice2\C128
copy ..\..\..\data\C128\c64s.vpl     Vice2\C128
copy ..\..\..\data\C128\chargen      Vice2\C128
copy ..\..\..\data\C128\default.vkm  Vice2\C128
copy ..\..\..\data\C128\default.vpl  Vice2\C128
copy ..\..\..\data\C128\default.vrs  Vice2\C128
copy ..\..\..\data\C128\frodo.vpl    Vice2\C128
copy ..\..\..\data\C128\godot.vpl    Vice2\C128
copy ..\..\..\data\C128\kernal       Vice2\C128
copy ..\..\..\data\C128\pc64.vpl     Vice2\C128
copy ..\..\..\data\C128\position.vkm Vice2\C128
copy ..\..\..\data\C128\vdc_deft.vpl Vice2\C128
copy ..\..\..\data\C128\z80bios      Vice2\C128

md Vice2\PET
copy ..\..\..\data\PET\amber.vpl     Vice2\PET
copy ..\..\..\data\PET\basic1        Vice2\PET
copy ..\..\..\data\PET\basic2        Vice2\PET
copy ..\..\..\data\PET\basic4        Vice2\PET
copy ..\..\..\data\PET\busi_de.vkm   Vice2\PET
copy ..\..\..\data\PET\busi_uk.vkm   Vice2\PET
copy ..\..\..\data\PET\chargen       Vice2\PET
copy ..\..\..\data\PET\chargen.de    Vice2\PET
copy ..\..\..\data\PET\default.vpl   Vice2\PET
copy ..\..\..\data\PET\edit1g        Vice2\PET
copy ..\..\..\data\PET\edit2b        Vice2\PET
copy ..\..\..\data\PET\edit2g        Vice2\PET
copy ..\..\..\data\PET\edit4b40      Vice2\PET
copy ..\..\..\data\PET\edit4b80      Vice2\PET
copy ..\..\..\data\PET\edit4g40      Vice2\PET
copy ..\..\..\data\PET\graphics.vkm  Vice2\PET
copy ..\..\..\data\PET\kernal1       Vice2\PET
copy ..\..\..\data\PET\kernal2       Vice2\PET
copy ..\..\..\data\PET\kernal4       Vice2\PET
copy ..\..\..\data\PET\posg_de.vkm   Vice2\PET
copy ..\..\..\data\PET\rom1g.vrs     Vice2\PET
copy ..\..\..\data\PET\rom2b.vrs     Vice2\PET
copy ..\..\..\data\PET\rom2g.vrs     Vice2\PET
copy ..\..\..\data\PET\rom4b40.vrs   Vice2\PET
copy ..\..\..\data\PET\rom4b80.vrs   Vice2\PET
copy ..\..\..\data\PET\rom4g40.vrs   Vice2\PET
copy ..\..\..\data\PET\white.vpl     Vice2\PET

md Vice2\VIC20
copy ..\..\..\data\VIC20\basic         Vice2\VIC20
copy ..\..\..\data\VIC20\chargen       Vice2\VIC20
copy ..\..\..\data\VIC20\default.vkm   Vice2\VIC20
copy ..\..\..\data\VIC20\default.vpl   Vice2\VIC20
copy ..\..\..\data\VIC20\default.vrs   Vice2\VIC20
copy ..\..\..\data\VIC20\kernal        Vice2\VIC20
copy ..\..\..\data\VIC20\position.vkm  Vice2\VIC20

md Vice2\CBM-II
copy ..\..\..\data\CBM-II\amber.vpl     Vice2\CBM-II
copy ..\..\..\data\CBM-II\basic.128     Vice2\CBM-II
copy ..\..\..\data\CBM-II\basic.256     Vice2\CBM-II
copy ..\..\..\data\CBM-II\busi_uk.vkm   Vice2\CBM-II
copy ..\..\..\data\CBM-II\chargen.600   Vice2\CBM-II
copy ..\..\..\data\CBM-II\chargen.700   Vice2\CBM-II
copy ..\..\..\data\CBM-II\default.vpl   Vice2\CBM-II
copy ..\..\..\data\CBM-II\kernal        Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom128h.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom128l.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom256h.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\rom256l.vrs   Vice2\CBM-II
copy ..\..\..\data\CBM-II\white.vpl     Vice2\CBM-II

md Vice2\DRIVES
copy ..\..\..\data\DRIVES\d1541II       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1001       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1541       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1571       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos1581       Vice2\DRIVES
copy ..\..\..\data\DRIVES\dos2031       Vice2\DRIVES

md Vice2\doc
copy "vac++\vice2.inf"         Vice2\doc

md Vice2\doc\readme
copy  ..\..\..\AUTHORS         Vice2\doc\readme
copy  ..\..\..\BUGS            Vice2\doc\readme
copy  ..\..\..\COPYING         Vice2\doc\readme
copy  ..\..\..\FEEDBACK        Vice2\doc\readme
copy  ..\..\..\NEWS            Vice2\doc\readme
copy  ..\..\..\README          Vice2\doc\readme
copy  ..\..\..\TODO            Vice2\doc\readme

md vice2\doc\txt
copy  ..\..\..\doc\Evaluation  Vice2\doc\txt\Evaluation
xcopy ..\..\..\doc\*.txt       Vice2\doc\txt

md Vice2\doc\html
xcopy ..\..\..\doc\html\*.html Vice2\doc\html

zip -r -9 vice2 vice2\*.*

