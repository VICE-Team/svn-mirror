@echo off
if not exist vs_tmpl\mkmsvc.exe goto missingmkmsvc
echo generating MSVC7.1 project as vs71
if exist vs71 goto removedir
:makemsvcdir
mkdir vs71
cd vs_tmpl
mkmsvc.exe -native -71 vice
cd ..
copy msvc\make-msvc-bindist-native.bat.proto vs71\make-msvc-bindist.bat
copy msvc\debug.h vs71\debug.h
echo MSVC7.1 project files generated in vs71
goto end
:missingmkmsvc
echo vs_tmpl\mkmsvc.exe is missing
goto end
:removedir
echo y | del vs71\*.* /S
rd vs71 /s /q
if exist vs71 goto cannotremovedir
goto makemsvcdir
:cannotremovedir
echo can't delete the vs71 directory, please remove vs71 manually and run this batch file again.
:end
pause
