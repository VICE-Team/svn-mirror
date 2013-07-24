@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC9 project as vs9
if exist vs9 goto removedir
:makemsvcdir
mkdir vs9
cd vs_tmpl
mkmsvc.exe -native -9 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs9\make-msvc-bindist.bat
copy msvc\winid*.bat vs9
echo MSVC9 project files generated in vs9
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs9\*.* /S
rd vs9 /s /q
if exist vs9 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs9 directory, please remove vs9 manually and run this batch file again.
:end
pause
