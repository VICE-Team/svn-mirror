  Special OS/2 Commandline options
------------------------------------

-stretch <number>  Stretch PM Window by a factor of <number>
-border <number>   Display border (2=big, 1=small, 
                   else=no border<default>)
-joydev1 <number>  Set CBM-Joystick #1 to 0=none, 1=Joystick 1, 
                   2=Joystick 2
-joydev2 <number>  Set CBM-Joystick #2 to 0=none, 1=Joystick 1, 
                   2=Joystick 2
-joy1cal           Start Auto Calibration for Joystick#1 at 
                   startup
-joy2cal           Start Auto Calibration for Joystick#1 at 
                   startup
-mouse             Support mouse
+mouse             Don't support mouse
-hidemouseptr      Hide OS/2 mouse pointer in Vice-Window
+hidemouseptr      Don't hide mouse pointer
-menu              Show menu bar <DEFAULT>
+menu              Hide menu bar


  Keys
--------

Scrllck:  Warp mode on/off
F10:      Switch CAPS-Key on/off (x128 only)
F11:      Switch VDC-Key on/off  (x128 only)
F12:      Restore
Esc:      Run/Stop
Alt-Home: Toggle Fullscreen/Window mode
Alt-PgUp: Higher Fulscreen Refresh Rate (FS mode only)
Alt-PgDn: Lower Fullscreen Refresh Rate (FS mode only)
Alt-F2:   Save File
Alt-F3:   Open File
Alt-+:    Increase running speed
Alt--:    Decrease running speed
Alt-0:    Attach tape image to Datasette
Alt-2:    Toggle double size on/off
Alt-3:    Toggle real PAL emulation: Off, Fake, Sharp, Blur.
Alt-4:    Enable/Disable Emulation of Printer Device #4
Alt-5:    Enable/Disable Emulation of Printer Device #4
Alt-8:    Attach disk image to Drive 8
Alt-9:    Attach disk image to Drive 9
Alt-A:    Open about-dialog
Alt-B:    Enable/Disable menubar
Alt-C:    Open datasette-control-dialog
Alt-D:    Open drive-dialog
Alt-H:    Open hardware-dialog
Alt-I:    VIC-II Color Setup
Alt-J:    Open joystick-dialog
Alt-L:    Display logging window
Alt-M:    Open monitor
Alt-N:    Flip Drive #8 to next image of fliplist
Alt-P:    Pause Emulation
Alt-Q:    Do hard-reset (switch machine off and on)
Alt-R:    Do soft-reset (press reset)
Alt-S:    Open sound-dialog
Alt-T:    Switch true drive emulation on/off
Alt-W:    Write actual configuration to file


  Additional info
-------------------

 - The video cache for CRTC doesn't work yet.
 - You can autostart images by drag'n'drop
 - Use drag'n'drop to play another SID-File in vsid mode (-vsid)


  Reported bugs/problems
--------------------------
 
 - some people reported problems with caps-lock after exiting 
   vice
 - somebody had problems with cs4235 based soundcard 
   (using sound-dialog hangs OS/2)
 - somebody had problems with the attach-dialog and a 
   Matrox g400
 - x128 hangs when closing on some systems. This seems to be a
   problem of the graphic card driver. The use of SciTech
   Display Doctor instead of the drivers solves the problem. It 
   is available from the IBM Device Driver Pack

  To Do
---------

