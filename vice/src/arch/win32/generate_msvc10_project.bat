@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC10 project as vs10
if exist vs10 goto removedir
:makemsvcdir
mkdir vs10
cd vs_tmpl
mkmsvc.exe -native -10 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs10\make-msvc-bindist.bat
copy msvc\winid*.bat vs10
copy msvc\debug.h vs10\debug.h
echo MSVC10 project files generated in vs10
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs10\*.* /S
rd vs10 /s /q
if exist vs10 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs10 directory, please remove vs10 manually and run this batch file again.
:end
pause
