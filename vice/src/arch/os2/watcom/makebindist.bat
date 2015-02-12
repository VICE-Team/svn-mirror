@echo off
call ..\..\win32\vice-version.bat
set VICEDIR=Vice2-%VICEVERSION%
if not exist x64.exe goto missingwatcom
if not exist x64dtv.exe goto missingwatcom
if not exist x64sc.exe goto missingwatcom
if not exist xscpu64.exe goto missingwatcom
if not exist x128.exe goto missingwatcom
if not exist xcbm2.exe goto missingwatcom
if not exist xcbm5x0.exe goto missingwatcom
if not exist xpet.exe goto missingwatcom
if not exist xplus4.exe goto missingwatcom
if not exist xvic.exe goto missingwatcom
if not exist vsid.exe goto missingwatcom
if not exist x1541.exe goto missingwatcom
if not exist cartconv.exe goto missingwatcom
if not exist petcat.exe goto missingwatcom
echo generating OPENWATCOM OS/2 binary port distribution
if exist %VICEDIR% goto removedir

:makevicedir
mkdir %VICEDIR%
type ..\install.cmd > %VICEDIR%\install.cmd
copy x64.exe      %VICEDIR%
copy x64dtv.exe   %VICEDIR%
copy x64sc.exe    %VICEDIR%
copy xscpu64.exe  %VICEDIR%
copy x128.exe     %VICEDIR%
copy xvic.exe     %VICEDIR%
copy xpet.exe     %VICEDIR%
copy xplus4.exe   %VICEDIR%
copy xcbm2.exe    %VICEDIR%
copy xcbm5x0.exe  %VICEDIR%
copy vsid.exe     %VICEDIR%
copy x1541.exe    %VICEDIR%
copy petcat.exe   %VICEDIR%
copy cartconv.exe %VICEDIR%
copy vsid.exe     %VICEDIR%

mkdir %VICEDIR%\Icons
copy ..\icons\folder1.ico %VICEDIR%\Icons
copy ..\icons\folder2.ico %VICEDIR%\Icons
copy ..\icons\c1541.ico   %VICEDIR%\Icons
copy ..\icons\vsid.ico    %VICEDIR%\Icons
copy ..\icons\x64.ico     %VICEDIR%\Icons
copy ..\icons\x64dtv.ico  %VICEDIR%\Icons
copy ..\icons\x64sc.ico   %VICEDIR%\Icons
copy ..\icons\x128.ico    %VICEDIR%\Icons
copy ..\icons\xcbm2.ico   %VICEDIR%\Icons
copy ..\icons\xcbm5x0.ico %VICEDIR%\Icons
copy ..\icons\xpet.ico    %VICEDIR%\Icons
copy ..\icons\xplus4.ico  %VICEDIR%\Icons
copy ..\icons\xscpu64.ico %VICEDIR%\Icons
copy ..\icons\xvic.ico    %VICEDIR%\Icons

mkdir %VICEDIR%\C64
copy ..\..\..\..\data\C64\basic       %VICEDIR%\C64
copy ..\..\..\..\data\C64\c64hq.vpl   %VICEDIR%\C64
copy ..\..\..\..\data\C64\c64mem.sym  %VICEDIR%\C64
copy ..\..\..\..\data\C64\c64s.vpl    %VICEDIR%\C64
copy ..\..\..\..\data\C64\ccs64.vpl   %VICEDIR%\C64
copy ..\..\..\..\data\C64\chargen     %VICEDIR%\C64
copy ..\..\..\..\data\C64\default.vpl %VICEDIR%\C64
copy ..\..\..\..\data\C64\default.vrs %VICEDIR%\C64
copy ..\..\..\..\data\C64\edkernal    %VICEDIR%\C64
copy ..\..\..\..\data\C64\frodo.vpl   %VICEDIR%\C64
copy ..\..\..\..\data\C64\godot.vpl   %VICEDIR%\C64
copy ..\..\..\..\data\C64\gskernal    %VICEDIR%\C64
copy ..\..\..\..\data\C64\jpchrgen    %VICEDIR%\C64
copy ..\..\..\..\data\C64\jpkernal    %VICEDIR%\C64
copy ..\..\..\..\data\C64\kernal      %VICEDIR%\C64
copy ..\..\..\..\data\C64\os2.vkm     %VICEDIR%\C64
copy ..\..\..\..\data\C64\os2_sym.vkm %VICEDIR%\C64
copy ..\..\..\..\data\C64\pc64.vpl    %VICEDIR%\C64
copy ..\..\..\..\data\C64\sxkernal    %VICEDIR%\C64
copy ..\..\..\..\data\C64\vice.vpl    %VICEDIR%\C64

mkdir %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\basic       %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\c64mem.sym  %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\c64s.vpl    %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\ccs64.vpl   %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\chargen     %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\default.vpl %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\default.vrs %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\dtvrom.bin  %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\frodo.vpl   %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\godot.vpl   %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\kernal      %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\os2.vkm     %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\os2_sym.vkm %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\pc64.vpl    %VICEDIR%\C64DTV
copy ..\..\..\..\data\C64DTV\vice.vpl    %VICEDIR%\C64DTV

mkdir %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\c64hq.vpl     %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\c64s.vpl      %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\ccs64.vpl     %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\chargen       %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\default.vpl   %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\default.vrs   %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\frodo.vpl     %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\godot.vpl     %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\os2.vkm       %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\os2_sym.vkm   %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\pc64.vpl      %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\scpu64        %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\jpchrgen      %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\scpu64mem.sym %VICEDIR%\SCPU64
copy ..\..\..\..\data\SCPU64\vice.vpl      %VICEDIR%\SCPU64

mkdir %VICEDIR%\C128
copy ..\..\..\..\data\C128\basic64      %VICEDIR%\C128
copy ..\..\..\..\data\C128\basiclo      %VICEDIR%\C128
copy ..\..\..\..\data\C128\basichi      %VICEDIR%\C128
copy ..\..\..\..\data\C128\c64s.vpl     %VICEDIR%\C128
copy ..\..\..\..\data\C128\ccs64.vpl    %VICEDIR%\C128
copy ..\..\..\..\data\C128\chargde      %VICEDIR%\C128
copy ..\..\..\..\data\C128\chargen      %VICEDIR%\C128
copy ..\..\..\..\data\C128\chargfr      %VICEDIR%\C128
copy ..\..\..\..\data\C128\chargse      %VICEDIR%\C128
copy ..\..\..\..\data\C128\chargch      %VICEDIR%\C128
copy ..\..\..\..\data\C128\default.vpl  %VICEDIR%\C128
copy ..\..\..\..\data\C128\default.vrs  %VICEDIR%\C128
copy ..\..\..\..\data\C128\frodo.vpl    %VICEDIR%\C128
copy ..\..\..\..\data\C128\godot.vpl    %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernal       %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernal64     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalde     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalfi     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalfr     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalit     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalno     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalse     %VICEDIR%\C128
copy ..\..\..\..\data\C128\kernalch     %VICEDIR%\C128
copy ..\..\..\..\data\C128\os2.vkm      %VICEDIR%\C128
copy ..\..\..\..\data\C128\pc64.vpl     %VICEDIR%\C128
copy ..\..\..\..\data\C128\vdc_deft.vpl %VICEDIR%\C128
copy ..\..\..\..\data\C128\vdc_comp.vpl %VICEDIR%\C128
copy ..\..\..\..\data\C128\vice.vpl     %VICEDIR%\C128

mkdir %VICEDIR%\PET
copy ..\..\..\..\data\PET\amber.vpl   %VICEDIR%\PET
copy ..\..\..\..\data\PET\basic1      %VICEDIR%\PET
copy ..\..\..\..\data\PET\basic2      %VICEDIR%\PET
copy ..\..\..\..\data\PET\basic4      %VICEDIR%\PET
copy ..\..\..\..\data\PET\chargen     %VICEDIR%\PET
copy ..\..\..\..\data\PET\chargen.de  %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit1g      %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit2b      %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit2g      %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit4b40    %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit4b80    %VICEDIR%\PET
copy ..\..\..\..\data\PET\edit4g40    %VICEDIR%\PET
copy ..\..\..\..\data\PET\green.vpl   %VICEDIR%\PET
copy ..\..\..\..\data\PET\kernal1     %VICEDIR%\PET
copy ..\..\..\..\data\PET\kernal2     %VICEDIR%\PET
copy ..\..\..\..\data\PET\kernal4     %VICEDIR%\PET
copy ..\..\..\..\data\PET\os2.vkm     %VICEDIR%\PET
copy ..\..\..\..\data\PET\os2_40.vkm  %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom1g.vrs   %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom2b.vrs   %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom2g.vrs   %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom4b40.vrs %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom4b80.vrs %VICEDIR%\PET
copy ..\..\..\..\data\PET\rom4g40.vrs %VICEDIR%\PET
copy ..\..\..\..\data\PET\white.vpl   %VICEDIR%\PET
copy ..\..\..\..\data\PET\*.bin       %VICEDIR%\PET

mkdir %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\3plus1hi    %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\3plus1lo    %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\basic       %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\c2lo.364    %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\default.vpl %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\default.vrs %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\kernal      %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\kernal.005  %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\kernal.232  %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\kernal.264  %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\os2.vkm     %VICEDIR%\PLUS4
copy ..\..\..\..\data\PLUS4\vice.vpl    %VICEDIR%\PLUS4

mkdir %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\basic       %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\chargen     %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\default.vpl %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\default.vrs %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\kernal      %VICEDIR%\VIC20
copy ..\..\..\..\data\VIC20\os2.vkm     %VICEDIR%\VIC20

mkdir %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\amber.vpl   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\basic.128   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\basic.256   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\basic.500   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\c64s.vpl    %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\ccs64.vpl   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\chargen.500 %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\chargen.600 %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\chargen.700 %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\default.vpl %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\frodo.vpl   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\godot.vpl   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\green.vpl   %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\kernal      %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\kernal.500  %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\os2.vkm     %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\pc64.vpl    %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\rom128h.vrs %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\rom128l.vrs %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\rom256h.vrs %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\rom256l.vrs %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\rom500.vrs  %VICEDIR%\CBM-II
copy ..\..\..\..\data\CBM-II\white.vpl   %VICEDIR%\CBM-II

mkdir %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1001 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1540 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1541 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\d1541II %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1551 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1570 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1571 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\d1571cr %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos1581 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos2000 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos4000 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos2031 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos2040 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos3040 %VICEDIR%\DRIVES
copy ..\..\..\..\data\DRIVES\dos4040 %VICEDIR%\DRIVES

mkdir %VICEDIR%\PRINTER
copy ..\..\..\..\data\PRINTER\cbm1526    %VICEDIR%\PRINTER
copy ..\..\..\..\data\PRINTER\mps801     %VICEDIR%\PRINTER
copy ..\..\..\..\data\PRINTER\mps803     %VICEDIR%\PRINTER
copy ..\..\..\..\data\PRINTER\mps803.vpl %VICEDIR%\PRINTER
copy ..\..\..\..\data\PRINTER\nl10-cbm   %VICEDIR%\PRINTER

mkdir %VICEDIR%\doc
copy  "..\..\..\..\doc\vice.inf"               %VICEDIR%\doc
copy  "..\..\..\..\doc\readmes\Readme-OS2/.txt %VICEDIR%\doc

mkdir %VICEDIR%\doc\readme
copy  ..\..\..\..\AUTHORS                %VICEDIR%\doc\readme
copy  ..\..\..\..\COPYING                %VICEDIR%\doc\readme
copy  ..\..\..\..\FEEDBACK               %VICEDIR%\doc\readme
copy  ..\..\..\..\NEWS                   %VICEDIR%\doc\readme
copy  ..\..\..\..\README                 %VICEDIR%\doc\readme

mkdir %VICEDIR%\doc\html
xcopy ..\..\..\..\doc\html\*.html %VICEDIR%\doc\html

echo OS/2 port binary distribution directory generated as %VICEDIR%
goto end

:missingwatcom
echo executables are missing, please build the project first.
goto end

:removedir
echo y | del %VICEDIR%\*.* /S
rd %VICEDIR% /s /q
if exist %VICEDIR% goto cannotremovedir
goto makevicedir

:cannotremovedir
echo can't delete the %VICEDIR% directory, please remove %VICEDIR% manually and run this batch file again.

:end
pause
