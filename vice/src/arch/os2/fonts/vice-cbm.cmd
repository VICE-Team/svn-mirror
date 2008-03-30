@Echo Off

REM /*                        */
REM /* Set Variables          */
REM /*                        */

call e:\toolkit\tksetenv

REM /*                        */
REM /* Convert Unix Font File */
REM /*                        */
chargen2fnt ..\..\..\..\data\C64\chargen   C64
chargen2fnt ..\..\..\..\data\C128\chargen  C128
chargen2fnt ..\..\..\..\data\PET\chargen   PET
chargen2fnt ..\..\..\..\data\VIC20\chargen VIC20

REM /*                        */
REM /* Create font-dll        */
REM /*                        */
alp vice2.asm
link386 vice2,,,,vice2.def
rc vice2.rc vice2.dll

REM /*                        */
REM /* 'Rename' font-dll      */
REM /*                        */
move vice2.dll vice2.fon

del chargen*.fnt vice-*.fnt *.res *.obj *.map
