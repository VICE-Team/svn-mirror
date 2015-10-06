@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC8 project as vs8
if exist vs8 goto removedir
:makemsvcdir
mkdir vs8
cd vs_tmpl
mkmsvc.exe -native -8 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs8\make-msvc-bindist.bat
copy msvc\winid*.bat vs8
copy msvc\debug.h vs8\debug.h
echo MSVC8 project files generated in vs8
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs8\*.* /S
rd vs8 /s /q
if exist vs8 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs8 directory, please remove vs8 manually and run this batch file again.
:end
pause
