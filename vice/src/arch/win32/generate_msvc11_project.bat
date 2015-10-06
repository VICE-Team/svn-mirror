@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC11 project as vs11
if exist vs11 goto removedir
:makemsvcdir
mkdir vs11
cd vs_tmpl
mkmsvc.exe -native -11 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs11\make-msvc-bindist.bat
copy msvc\winid*.bat vs11
copy msvc\debug.h vs11\debug.h
echo MSVC11 project files generated in vs11
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs11\*.* /S
rd vs11 /s /q
if exist vs11 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs11 directory, please remove vs11 manually and run this batch file again.
:end
pause
