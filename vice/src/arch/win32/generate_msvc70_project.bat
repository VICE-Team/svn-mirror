@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.0 project as vs70
if exist vs70 goto removedir
:makemsvcdir
mkdir vs70
cd vs_tmpl
mkmsvc.exe -native -70 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs70\make-msvc-bindist.bat
copy msvc\debug.h vs70\debug.h
echo MSVC7.0 project files generated in vs70
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs70\*.* /S
rd vs70 /s /q
if exist vs70 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs70 directory, please remove vs70 manually and run this batch file again.
:end
pause
