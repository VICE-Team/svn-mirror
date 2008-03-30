@Echo Off
e:\toolkit\tksetenv
bdf2fnt
del vice-cbm.fon
alp vice-cbm.asm
link386 vice-cbm,,,,vice-cbm.def
rc vice-cbm.rc vice-cbm.dll
ren vice-cbm.dll vice-cbm.fon
