@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC12 project as vs12
if exist vs12 goto removedir
:makemsvcdir
mkdir vs12
cd vs_tmpl
mkmsvc.exe -native -12 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs12\make-msvc-bindist.bat
copy msvc\winid*.bat vs12
copy msvc\debug.h vs12\debug.h
echo MSVC12 project files generated in vs12
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs12\*.* /S
rd vs12 /s /q
if exist vs12 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs12 directory, please remove vs12 manually and run this batch file again.
:end
pause
