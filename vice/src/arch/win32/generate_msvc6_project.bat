@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC6 project as vs6
if exist vs6 goto removedir
:makemsvcdir
mkdir vs6
cd vs_tmpl
mkmsvc.exe -native -6 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs6\make-msvc-bindist.bat
copy msvc\debug.h vs6\debug.h
echo MSVC6 project files generated in vs6
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs6\*.* /S
rd vs6 /s /q
if exist vs6 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs6 directory, please remove vs6 manually and run this batch file again.
:end
pause
