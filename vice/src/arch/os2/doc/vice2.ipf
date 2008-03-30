.*--------------------------------------------------------------*
.*  Help for Product information                                *
.*      res = 1000 name = PANEL_PRODUCTINFO                     *
.*--------------------------------------------------------------*
:userdoc.

:h1 name=258 IDM_ATTACH.Attach File
:p.Attach an image file to the emulator as
:p.- Tape (*.tap; *.t64)
:p.- Disk

:h1 res=291 name=IDM_READCONFIG.Read Configuration from file
:p.This reads the configuration file. If you didn't specify
a different one the standard file 'vice2.cfg' which must be 
located in the same directory as the executable vice2.exe
is read.

:h1 res=292 name=IDM_WRITECONFIG.Write Configuration to file
:p.This writes the configuration file. If you didn't specify
a different one the standard file 'vice2.cfg' which must be 
located in the same directory as the executable vice2.exe
is written.

:h1 res=293 name=IDM_SOFTRESET.Soft Reset
:p.A soft reset of the emulated machine will be done. It is 
like pressing a reset button, which you soldered to your 
original machine.

:h1 res=294 name=IDM_HARDRESET.Hard Reset
:p.A hard reset of the emulated machine will be done. It is
like switching off and on your original machine.

:h1 res=295 name=IDM_EXIT.Exit Emulator
:p.Exit the running emulation.

:h1 res=296 name=IDM_ATTACH8.Attach File to Drive #8
:p.This attaches a disk image to drive #8

:h1 res=297 name=IDM_ATTACH9.Attach File to Drive #9
:p.This attaches a disk image to drive #9

:h1 res=298 name=IDM_ATTACH10.Attach File to Drive #10
:p.This attaches a disk image to drive #10

:h1 res=299 name=IDM_ATTACH11.Attach File to Drive #11
:p.This attaches a disk image to drive #11

:h1 res=304 name=IDM_DETACHTAPE.Detach File from Tape
:p.This detaches the attached tape

:h1 res=312 name=IDM_DETACH8.Detach File from Drive #8
:p.This detaches the attached disk image from drive #8

:h1 res=313 name=IDM_DETACH9.Detach File from Drive #9
:p.This detaches the attached disk image from drive #9

:h1 res=314 name=IDM_DETACH10.Detach File from Drive #10
:p.This detaches the attached disk image from drive #10

:h1 res=315 name=IDM_DETACH11.Detach File from Drive #11
:p.This detaches the attached disk image from drive #11

:h1 res=319 name=IDM_DETACHALL.Detach Files from all Drives
:p.This detaches all attached disk images from all drives

:euserdoc.
