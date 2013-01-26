Unix Port of VICE
=================
Date: 2013-01-26
Started by: pottendo@gmx.net

This document contains some relevant information for the Unix ports of
VICE.

The port is maintained by a few people who contributed in the
respective parts.

This document is work-in-progress and is filled on demand with some
specific information:

1. Installation
---------------

1.1. Choose the GUI

At configure time use:
--enable-gnomeui for a GTK+ based UI, Sane fullscreen support is
given, when libgtkgl-ext is available on your system (at compile
time). 

The default (without specific option) the Xaw widget set is used. Sane
fullsscreen support is given when libxv is available on your system
(at compile time).

2. Specfics on Video Output
---------------------------

2.1 Feature: OpenGL Sync

This feature is deprecated and may show no effect on your system. It
shall disappear from VICE as the required API `glXWaitVideoSyncSGI()'
in the OpenGL library has vanished in recent libraries.

2.2 Feature: Multithreaded GUI

This feature is enabled at configure time: option --with-uithreads
Limitations: limited to gtk+ ui, option --enable-gnomeui, 
	     libgtkgl-ext is mandatory (HWSCALE support).
	     x128 is know to be broken

CAVEAT: this function is in alpha stage - use at your own risk.

Enabling this feature makes vice run multithreaded decoupling the
emulation (machinethread) from display (displaythread) and GUI event
handling (eventthread).
The result is accurate emulation cycles at machine frequency (emulated
PAL machines ~50Hz) and potential synchronized display output
following the display frequency of you monitor/display (on modern TFTs
normally 60Hz).
To overcome the non-aligned frequencies (50Hz vs. 60Hz) HW accelerated
alpha blending (OpenGL) is used giving the best possible results and
smoother animations, especially when smooth scrollers are shown.

Best results are achieved when a lightweigtht desktop e.g. LUBUNTU
(Linux) is used. Gnome Shell or Unity are known to be interacting
badly on the gfx output side (OpenGL) imposing visible jitters.
You may find the article:
  http://paradoxuncreated.com/Blog/wordpress/?p=2268
helpful to tune your system for best results.
To get best results the video thread shall synchronize to the vertical
retrace. For nvidia gfx boards this can often be controlled either by
the control program nvidia-settings or the environment variable:
  __GL_SYNC_TO_VBLANK=1
Refer to you gfx driver documentation for details.

VICEs video chip emulation offer `Video Cache'. This has to be
disabled: 
e.g. via commandline: xplus4 +TEDvcache) or via menu
  Settings -> VICII Settings -> Video Cache)

