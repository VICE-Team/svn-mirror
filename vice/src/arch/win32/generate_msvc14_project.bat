@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC14 project as vs14
if exist vs14 goto removedir
:makemsvcdir
mkdir vs14
cd vs_tmpl
mkmsvc.exe -native -14 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs14\make-msvc-bindist.bat
copy msvc\winid*.bat vs14
copy msvc\debug.h vs14\debug.h
echo MSVC14 project files generated in vs14
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs14\*.* /S
rd vs14 /s /q
if exist vs14 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs14 directory, please remove vs14 manually and run this batch file again.
:end
pause
