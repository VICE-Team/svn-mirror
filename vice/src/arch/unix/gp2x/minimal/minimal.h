/*
  Title: GP2X minimal library v0.C by rlyeh, 2005-2008.

  GP2X minimal library by rlyeh, 2005-2008. emulnation.info@rlyeh (swap it!):

  GP2X minimal library is a general purpose C multimedia library done for GP2X handheld, 
  featuring efficent code and a very exhaustive API covering almost all known hardware.
 
  I wrote the library originally in 2005 as a fun exercise trying to support video (8,15 & 16), 
  joystick, sound and timers in less than 128 lines of code (hence the original 'minimal' name :-).
  
  I certainly had a fun at coding whe whole library in less than 128 lines, but I knew 
  I had to switch to a bigger library at some point in order to provide a full GP2X support 
  to the recent community of upcoming GP2X developers.

  Nowadays the library is around 7,000 lines of code, including snippets from other developers. 
  I'm lucky that I have not to change the library name to 'maximal' at this point though, because 
  it stills being 'minimal' in some aspects (setup is only minimal.c and minimal.h files! :-).
  
  Enjoy the coding!
  
  
  Features:
  =========
  
  + GP2X 920t/940t CPUs library with a FIFO message system.
  + GP2X 32 bits YUV video library using double buffering.
  + GP2X 15/16 bits RGB video library using triple buffering.
  + GP2X 8 bits RGB video library using sextuple buffering.
  + GP2X sound library using octuple buffering.
  + GP2X TV out library using CX24785 video encoder chipset directly.
  + GP2X MMU library by providing own /dev/minimal kernel module.
  + GP2X joystick library with USB support.
  + GP2X profiler library.
  + GP2X blitter library.
  + GP2X image library.
  + GP2X timer library.
  + GP2X font library.


  Setting up:
  ===========
  
  Just add 'minimal.c' and 'minimal.h' files to your project directory. A piece of cake.

  
  License:
  ========

    Copyright (c) 2005-2008, rlyeh.

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.

  
  Credits:
  ========
  
  I would like to thank Squidge, RobBrown, snaff, Reesy, NK, foft, K-teto, sasq, whizzbang,
  paeryn, god_at_hell, craigix, Emu&Co, Puck2099 and GnoStiC for the help & previous work! :-)
  
  I would like to thank Anarchy, Craigix and EvilDragon for providing me free hardware
  to develop nicely this library, my Reminiscence port and all the emulators I am coding. Thanks guys! :-)
  
  I would like to "thank" GamePark Holdings for constructing such a "quality" device and 
  for ignoring developers and their requests actively.
  
  
  Contact:
  ========
  
  You can contact me at emulnation.info @ rlyeh (swap it!) or rlyeh at #gp2xdev @ EFnet (IRC :-)
  
  
  Known projects using the library or parts from it:
  ==================================================

  + AlexKidd2X (Puck2099)
  + Atari800 GP2X (foft)
  + Criticalbeep 0.1.0 (Critical)
  + D3DGP2XCross (FredrikH)
  + DrMDx (Reesy)
  + DrPocketSnes (Reesy)
  + Exult GP2X (Puck2099)
  + Fenix GP2X (Puck2099)
  + File Frontend (kounch)
  + Fuse GP2X (ben_ost)
  + GNUBoy2x (K-teto)
  + GP2Xengine (Hermes/PS2Reality, Metalbrain)
  + GPFCE (zzhu8192, notaz)
  + GP LadyKiller (Puck2099)
  + Handy2X (Tharos)
  + Lotty's Lines (Baggers)
  + MAME GP2X (Franxis)
  + NeoPOP-GP2x (madelman)
  + Noiz2sa (notaz)
  + Openjazz GP2X (Puck2099)
  + Payback (jrd)
  + Phamicom-GP2X (madelman)
  + Picodrive GP2X (Notaz)
  + REminiscence (rlyeh)
  + Rise of the Triad (Puck2099)
  + SquidgeSNES (Squidge, Reesy, Notaz)
  + Samurai (Puck2099)
  + UAE4ALL GP2X (Critical)
  + VICE2X (Snaff)
  + WS2X (^MiSaTo^, Puck2099)
  + XUMP (kedo)


  What's delayed for final release:
  =================================

  - OSD layer support
  - fine scaling (post scaling) support (holy grail? YUV only?)
  - support for DaveC's pad configurations (model 0,I,II,III,IV)
  - support for scaling and rotating rects
  - touchpad support
  - usb mouse support
  - usb keyboard support
  - custom tv modes
  - custom framebuffer size
  - single buffering support (una-i's request)
  - eeprom reading support


  What's missing for this milestone:
  ==================================

  - open2x and minimal.o bug.
  - fix a bug in RGB windows (joanvr's bug).
  - test gp2x_file handling (& file selector) & write proper api.
  - do zip/lzw loader.
  - test PSD,PNG image handling.
  - test hardware blitting & write proper api.
  - test yuv scaling fix.
  - test tv video output (ntsc & pal) & test all layers.
  - test I2C support.
  - test uninstallhack.
  - do WAV/RAW loader.
  - do sw mixer.
  
  - fix touchscreen input values.
  
  - tested tv rgb scaling fix.
  - added gp2x_timer_reset.
  - gp2x_counter_* write proper doc.


  What's new:
  ===========

  (start code)

  0.C: added full 64mb support finally! :)
       added optimized RAM timings (done with UPLL prescaler; thanks god_at_hell, Emu&Co and craigix)
       added built-in MMU hack (thanks Squidge & NK)
       added built-in high resolution profiler (thanks GnoStiC for suggestions :-)
       added 16 high resolution software timers
       added overclocking support (thanks RobBrown, sasq & whizzbang)
       added preliminary gp2x hardware blitter support (thanks Paeryn, Reesy & Squidge)
       added /dev/minimal module driver (thanks oyzzo, GnoStiC and Gary Miller for compiling my sources)
       added hardcore homebrew (HH) programs launching support (thanks RobBrown!)
       added TV out support by using CX24785 encoder directly (thanks THB & vimacs for testing)
       added USB joysticks support (thanks GnoStiC for testing)
       added I2C (CX24785) support
       added BMP file image handling
       added TGA file image handling
       added flushing cache support
       added rebooting support
       added GP2X-F200 touch screen support (thanks ********************************)
       added GP2X-F100 leds support (thanks K-teto & RobBrown)
       added GP2X-F100 battery check support (thanks Squidge)
       added LCD backlight support (thanks RobBrown)
       added stereo/mono sound support
       added 3D DAC boost support
       added 6db pre-DAC attenuation support
       added bass/treble intensity support
       added headphones detection support
       added memory card detection support
       added levels based debug macro (produces compilable code only when levels match)
       added API exceptions in touch screen functions for GP2X-F100 units
       added API exceptions in battery checkout, battery LED, LCD backlight, custom sound rates, and volume level for GP2X-F200 units
       added UP+DOWN+LEFT+RIGHT as PUSH in GP2X-F200 models.
       fixed white LCD bug when using TV out (thanks THB for the report)
       fixed hsync/vsync polarity for firmware 2.0.0 (thanks K-teto)
       fixed YUV aspect ratio (thanks Puck2099)
       fixed dualcore support again (broken in 0.B)
       fixed two stupid typos in sound.c tutorial (thanks Nawa for the report)
       fixed intensive cpu usage bug when not playing sound
       fixed gp2x_video_setdithering function
       changed video buffering for 16bits mode (triple now)
       changed video buffering for 8bits mode (sextuple now!)
       changed YUV/RGB/dualcore/cursor memory mappings
       changed whole memory mapping to a better layout featuring a free, big and contiguous memory block sized up to 31 Mb.
       changed dualcore initialization method
       changed dualcore memory mapping method
       changed dualcore shared area usage through gp2x_dualcore_data() macro (easier to use now)
       changed dualcore stack area to a bigger and safer one (using 64kb now)
       changed dualcore 940t icache/dcache settings (should be faster now)
       changed dualcore 940t binary limit from 10Mb to 31Mb
       changed RGB region initialization method
       changed joystick read handling. You can check also for absolute values now.
       changed license for commercial projects
       changed volume level when using GP2X-F200 model
       restored clock setting when initializing your program (200 MHz default)
       restored leds when initializing your program
       restored low battery led when initializing your program

       ... 
       and I'm pretty sure I've forgotten a few things. I'm sorry for that.

       added functions:
       - gp2x_blitter_enable()
       - gp2x_blitter_send()
       - gp2x_blitter_wait()
       - gp2x_dualcore_launch_HH()
       - gp2x_dualcore_clock()
       - gp2x_i2c_read()
       - gp2x_i2c_write()
       - gp2x_image_bmp() 
       - gp2x_image_tga() 
       - gp2x_image_free()
       - gp2x_joystick_name()
       - gp2x_joystick_scan()
       - gp2x_joystick_wait()
       - gp2x_misc_flushcache()
       - gp2x_misc_led()
       - gp2x_misc_lcd()
       - gp2x_misc_battery()
       - gp2x_misc_headphones()
       - gp2x_misc_memcard()
       - gp2x_profile_analyze()
       - gp2x_reboot()
       - gp2x_sound_stereo()
       - gp2x_sound_3Dboost()
       - gp2x_sound_attenuation()
       - gp2x_sound_setintensity()
       - gp2x_timer_read_raw()
       - gp2x_timer_raw_to_ticks()
       - gp2x_timer_raw_one_second()
       - gp2x_timer_init()
       - gp2x_timer_start()
       - gp2x_timer_read()
       - gp2x_timer_stop()
       - gp2x_tv_setmode()
       - gp2x_tv_adjust()

       added macros:
       - gp2x_debug
       - gp2x_profile

       modified variables:
       - *gp2x_dualcore_ram

       changed functions:
       - gp2x_timer_read() is now gp2x_timer_read_ticks()
       
       modified functions:
       - gp2x_joystick_read()
       - gp2x_video_setdithering()

       modified structs:
       - deleted { *screen8, *screen16, *screen32 } pointers in gp2x_video_YUV[0,1,2,3].
       - deleted { *screen8, *screen16, *screen32 } pointers in gp2x_video_RGB[0].
       - added { void *screen } pointer into gp2x_video_YUV[0,1,2,3]
       - added { void *screen } pointer into gp2x_video_RGB[0]
       - added gp2x_rect.type enum  { RECT_RGB8=1, RECT_RGB16=2, RECT_YUV=4 };

       deprecated headers:
       - removed minimal_940t.h. Use '#define MINIMAL_940T' before '#include minimal.h' and you're ready now.

       documentation:
       - added dualcore introduction
       - added new documentation
       - improved old documentation

       examples:
       - added yuv example
       - added reboot example

  0.B: preliminary documentation. Created with NaturalDocs (thanks synkro for the tip!).
       preliminary Cursor layer support.
       preliminary RGB windows support.
       preliminary RGB alpha blending support.
       preliminary RGB key colouring support.
       preliminary RGB coarse scaling support.
       preliminary YUV layer support.
       preliminary YUV regions support.
       preliminary YUV splitting screen support.
       preliminary YUV mirroring support.
       preliminary YUV coarse scaling support.
       preliminary Gamma correction support.
       preliminary dithering support (untested).
       preliminary font support. Using public domain 6x10 console font.
       using octuple buffering for sound now.
       new 16 bits RGB video mode.

       improved functions:
        - gp2x_dualcore_exec()  ; it queues up to 1000 commands (dualcore should work better now)
  
       fixed functions:
        - gp2x_blitter_rect()   ; fixed stupid typo (thanks slygamer for the bug report)

       added structs:
        - gp2x_video_YUV[0,1,2,3] which contain {*screen8,*screen16,*screen32} pointers
        - gp2x_video_RGB[0] which contains {*screen8,*screen16,*screen32} pointers

       added functions:
        - gp2x_video_YUV_color()
        - gp2x_video_YUV_flip()
        - gp2x_video_YUV_setregions()
        - gp2x_video_YUV_setscaling()
        - gp2x_video_RGB_color16()
        - gp2x_video_RGB_setwindows()
        - gp2x_video_RGB_setscaling()
        - gp2x_video_RGB_setcolorkey()
        - gp2x_video_setluminance()
        - gp2x_video_setgammacorrection()
        - gp2x_video_waitvsync()
        - gp2x_video_waithsync()
        - gp2x_printf_init()
        - gp2x_printf()

       deprecated variables:
        - *gp2x_screen15
        - *gp2x_screen8

       changed functions:
        - gp2x_video_flip()           to  gp2x_video_RGB_flip()
        - gp2x_video_setpalette()     to  gp2x_video_RGB_setpalette()
        - gp2x_video_color15(R,G,B,A) to  gp2x_video_RGB_color15(R,G,B,A)
        - gp2x_video_color8(C,R,G,B)  to  gp2x_video_RGB_color8(R,G,B,C)  ; notice new parameters order 
        - gp2x_init()                 to  gp2x_init()                     ; got an extra parameter now
  
  0.A: 940t disabled all time unless required
       sound is paused by default now, use gp2x_sound_pause() to unlock it
  
       fixed functions:
        - gp2x_sound_play()                        ; increased the number of sampling buffers
        - gp2x_timer_read()                        ; it should work again. broken at some point before
        - gp2x_dualcore_launch_program_from_disk() ; it missed the code uploading call
  
       added functions:
        - gp2x_sound_pause()
        - gp2x_timer_delay()
        - gp2x_dualcore_pause()
  
  0.9: initial FIFO message system for dual cpu cores.
       initial 48 Mb support.
       initial quadruple buffering in 8bbp mode.              
  
       added functions:
        - gp2x_dualcore_exec() ; initial FIFO message system for dual cpu cores. 
        - gp2x_dualcore_sync() ; initial FIFO message system for dual cpu cores.
  
       improved functions:
        - gp2x_video_flip()    ; initial quadruple buffering in 8bbp mode.              
  
  0.8: initial dual cores cpu support.
       very basic blit functions by popular demand ;-)
       vsync & hsync waiting code (thanks Reesy)
  
       added functions:
        - gp2x_launch_program()            ; initial dual cores cpu support.
        - gp2x_launch_program_from_disk()  ; initial dual cores cpu support.
        - gp2x_launch_subprogram()         ; initial dual cores cpu support.
        - gp2x_blitter_rect15()            ; very basic blit function by popular demand ;-)
        - gp2x_blitter_rect8()             ; very basic blit function by popular demand ;-)
        - gp2x_video_hsync()               ; hsync waiting code (thanks Reesy)
        - gp2x_video_vsync()               ; vsync waiting code (thanks Reesy)
  
       fixed functions:
        - gp2x_video_color8()              ; bugfixed a stupid typo (thanks Franxis for the bug report)
  
  0.7: added functions:
        - gp2x_sound_volume()
  
       fixed functions:
        - gp2x_deinit()           ; fixed menu launch code when exiting.
  
       improved functions:
        - gp2x_timer_read()       ; rewritten timer. it should be more accurate now.
        - gp2x_init()             ; faster init code.
  
  0.6: added functions:
        - gp2x_timer_read()
        - gp2x_sound_pause()
  
       fixed functions:
        - gp2x_video_setpalette() ; palette handling was incorrect. fixed.
  
  0.5: improved functions:
        - gp2x_init()             ; better and cleaner initalizing code.
        - gp2x_init()             ; sound patched for real stereo output (by using NK's solution)
  
  0.4: lots of cleanups.
       sound is threaded and double buffered now.
       8 bpp video support.
  
       fixed functions:
        - gp2x_deinit()           ; better and cleaner exiting code.
  
  0.3: shorter library.
  
       fixed functions:
        - gp2x_joystick_read()    ; improved joystick diagonal detection.
  
  0.2: better code layout.
       public release.
  
  0.1: beta release.
*/

#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#include <limits.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>
#include <linux/keyboard.h>
#include <linux/kd.h>
#include <linux/joystick.h>
#include <termios.h>
#include <keycodes.h>

#define MINILIB_VERSION  "GP2X minimal library v0.C by rlyeh, (c) 2005-2007."


#ifndef GP2X_DEBUG_LEVEL
#define GP2X_DEBUG_LEVEL 0
#endif

#ifdef  GP2X_PROFILE_DISABLE
#define GP2X_PROFILE 0
#else
#define GP2X_PROFILE 1
#endif

#define gp2x_debug(level, ...)   if(GP2X_DEBUG_LEVEL & level) { __VA_ARGS__ ;} 

#define gp2x_profile( ... ) \
 if(GP2X_PROFILE) {                                                                          \
 static gp2x_profiles *prf = NULL;                                                           \
                                                                                             \
 if(prf == NULL) { prf = gp2x_profile_register();                                            \
                   sprintf(prf->fname,  __FILE__ ":%d:%s", __LINE__, __func__ );             \
                   prf->time = prf->calls = 0; }                                          \
                                                                                             \
 prf->calls++;                                                                               \
 prf->time -= gp2x_memregl[0x0A00>>2];                                                       \
 { __VA_ARGS__ ;}                                                                                   \
 prf->time += gp2x_memregl[0x0A00>>2]; } else { __VA_ARGS__ ;}
 
#define isUp(x)    
#define isDown(x)  
#define isPress(x) 

enum  { RECT_RGB8=1, RECT_RGB16=2, RECT_YUV=4 };

enum  { GP2X_UP=(0x1L<<0),    GP2X_LEFT=(0x1L<<2),    GP2X_DOWN=(0x1L<<4), GP2X_RIGHT=(0x1L<<6),
        GP2X_START=(1L<<8),   GP2X_SELECT=(1L<<9),    GP2X_L=(1L<<10),     GP2X_R=(1L<<11),
        GP2X_A=(1L<<12),      GP2X_B=(1L<<13),        GP2X_X=(1L<<14),     GP2X_Y=(1L<<15),
        GP2X_VOL_UP=(1L<<23), GP2X_VOL_DOWN=(1L<<22), GP2X_PUSH=(1L<<27),  
        GP2X_HOME=(1L<<8),    GP2X_TOUCH=(1L<<28) };
        
enum  { LCD = 0, PAL = 4, NTSC = 3 }; 

typedef struct gp2x_font        { int x,y,w,wmask,h,fg,bg,solid; unsigned char *data; } gp2x_font;
typedef struct gp2x_queue       { volatile unsigned long head, tail, items, max_items; unsigned long *place920t, *place940t; } gp2x_queue;
typedef struct gp2x_rect        { int x,y,w,h,solid,type; void *data; } gp2x_rect;

typedef struct gp2x_video_layer { void *screen;

                                  unsigned long i[8];  //physical addresses of each buffer. they might have other usages too
                                  void *p[8];          //virtual address of each buffer
                                } gp2x_video_layer;

typedef struct gp2x_file        { int size, compressed; void *data; } gp2x_file;

typedef struct gp2x_profiles    { char fname[56]; unsigned long calls, time; } gp2x_profiles;

extern gp2x_video_layer         gp2x_video_RGB[1], gp2x_video_YUV[4];
extern volatile unsigned long  *gp2x_dualcore_ram, *gp2x_memregl, *gp2x_blitter;
extern volatile unsigned short *gp2x_memregs;
extern unsigned long            gp2x_usbjoys, gp2x_f200;
extern gp2x_font                gp2x_default_font;
//extern gp2x_profiles           *gp2x_profiles;


extern void           gp2x_video_setgammacorrection(float);
extern void           gp2x_video_setdithering(int);
extern void           gp2x_video_setluminance(int, int);
extern void           gp2x_video_waitvsync(void);
extern void           gp2x_video_waithsync(void);
extern void           gp2x_video_cursor_show(int);
extern void           gp2x_video_cursor_move(int, int);
extern void           gp2x_video_cursor_setalpha(int, int);
extern void           gp2x_video_cursor_setup(unsigned char *, int, unsigned char, int, int, int, int, unsigned char, int, int, int, int);
extern void           gp2x_video_logo_enable(int);

extern void           gp2x_video_RGB_color8 (int, int, int, int);
extern unsigned short gp2x_video_RGB_color15(int, int, int, int);
extern unsigned short gp2x_video_RGB_color16(int, int, int);
extern void           gp2x_video_RGB_setpalette(void);
extern void           gp2x_video_RGB_setcolorkey(int, int, int);
extern void           gp2x_video_RGB_setscaling(int, int);
extern void           gp2x_video_RGB_flip(int);
extern void           gp2x_video_RGB_setwindows(int, int, int, int, int, int);

extern unsigned long  gp2x_video_YUV_color(int, int, int);
extern void           gp2x_video_YUV_setscaling(int, int, int);
extern void           gp2x_video_YUV_flip(int);
extern void           gp2x_video_YUV_setparts(int, int, int, int, int, int);

extern void           gp2x_blitter_rect(gp2x_rect *);
extern void           gp2x_blitter_wait(void);
extern void           gp2x_blitter_send(gp2x_rect *);

extern unsigned long  gp2x_joystick_read(int);
extern char          *gp2x_joystick_name(int);
extern void           gp2x_joystick_wait(int, unsigned long);
extern void           gp2x_joystick_scan(void);

extern void           gp2x_touchscreen_update(void);

/*writeme : raw -> timer, timer -> counter ? */
extern void           gp2x_timer_reset(void);
extern void           gp2x_timer_delay(unsigned long);
extern unsigned long  gp2x_timer_read_raw(void);
extern unsigned long  gp2x_timer_read(void);
extern unsigned long  gp2x_timer_raw_to_ticks(unsigned long);
extern unsigned long  gp2x_timer_raw_one_second(void);

extern void               gp2x_counter_init(int);
extern void               gp2x_counter_start(int);
extern unsigned long long gp2x_counter_read(int);
extern void               gp2x_counter_pause(int);

extern gp2x_profiles *gp2x_profile_register(void);
extern char          *gp2x_profile_analyze(void);

extern void           gp2x_tv_setmode(unsigned char);
extern void           gp2x_tv_adjust(signed char, signed char);

extern void           gp2x_sound_frame (void *, void *, int);
extern void           gp2x_sound_volume(int, int);
extern void           gp2x_sound_pause(int);
extern void           gp2x_sound_stereo(int);
extern void           gp2x_sound_3Dboost(int);
extern void           gp2x_sound_attenuation(int);
extern void           gp2x_sound_setintensity(int, int, int);

extern void           gp2x_i2c_write(unsigned char, unsigned char, unsigned char);
extern unsigned char  gp2x_i2c_read(unsigned char, unsigned char);

extern void           gp2x_dualcore_clock(int);
extern void           gp2x_dualcore_pause(int);
extern void           gp2x_dualcore_sync(void);
extern void           gp2x_dualcore_exec(unsigned long);
extern void           gp2x_dualcore_launch_program(unsigned long *, unsigned long);
extern void           gp2x_dualcore_launch_program_from_disk(char *, unsigned long, unsigned long);
extern void           gp2x_dualcore_launch_HH(char *);

extern void           gp2x_printf(gp2x_font *, int, int, const char *, ...);
extern void           gp2x_printf_init(gp2x_font *, int, int, void *, int, int, int);

extern void           gp2x_misc_led(int);
extern void           gp2x_misc_lcd(int);
extern  int           gp2x_misc_battery(void);
extern void           gp2x_misc_flushcache(void);
extern  int           gp2x_misc_headphones(void);
extern void           gp2x_misc_memcard(void);

extern  int           gp2x_image_tga(FILE *fp, gp2x_rect *r);
extern  int           gp2x_image_bmp(FILE *fp, gp2x_rect *r, unsigned char paletteIndex);
extern void           gp2x_unimage(gp2x_rect *r);

extern void           gp2x_init(int, int, int, int, int, int, int);
extern void           gp2x_deinit(void);
extern void           gp2x_reboot(void);


/* for our minimal kernel module */
#define MINIMAL_I2C_WRITE            0x0
#define MINIMAL_I2C_READ             0x1
#define MINIMAL_MMU_FLUSHCACHE       0x2
#define MINIMAL_MMU_INSTALLHACK      0x3
#define MINIMAL_MMU_INSTALLHACK_32   0x4
#define MINIMAL_MMU_INSTALLHACK_48   0x5
#define MINIMAL_MMU_UNINSTALLHACK    0x6
#define MINIMAL_MMU_R001             0x100
#define MINIMAL_MMU_R100             0x101
#define MINIMAL_MMU_W100             0x102
#define MINIMAL_MMU_R200             0x103
#define MINIMAL_MMU_W200             0x104
#define MINIMAL_MMU_R300             0x105
#define MINIMAL_MMU_W300             0x106
#define MINIMAL_MMU_R500             0x107
#define MINIMAL_MMU_W500             0x108
#define MINIMAL_MMU_R501             0x109
#define MINIMAL_MMU_W501             0x10a
#define MINIMAL_MMU_R600             0x10b
#define MINIMAL_MMU_W600             0x10c
#define MINIMAL_MMU_W750             0x10d
#define MINIMAL_MMU_W760             0x10e
#define MINIMAL_MMU_W770             0x10f
#define MINIMAL_MMU_W751             0x110
#define MINIMAL_MMU_W7131            0x111
#define MINIMAL_MMU_W761             0x112
#define MINIMAL_MMU_W7101            0x113
#define MINIMAL_MMU_W7141            0x114
#define MINIMAL_MMU_W7102            0x115
#define MINIMAL_MMU_W7104            0x116
#define MINIMAL_MMU_W7142            0x117
#define MINIMAL_MMU_W704             0x118
#define MINIMAL_MMU_W870             0x119
#define MINIMAL_MMU_W850             0x11a
#define MINIMAL_MMU_W851             0x11b
#define MINIMAL_MMU_W860             0x11c
#define MINIMAL_MMU_W861             0x11d
#define MINIMAL_MMU_R900             0x11e
#define MINIMAL_MMU_W900             0x11f
#define MINIMAL_MMU_R901             0x120
#define MINIMAL_MMU_W901             0x121
#define MINIMAL_MMU_R1000            0x122
#define MINIMAL_MMU_W1000            0x123
#define MINIMAL_MMU_R1001            0x124
#define MINIMAL_MMU_W1001            0x125
#define MINIMAL_MMU_R1300            0x126
#define MINIMAL_MMU_W1300            0x127
#define MINIMAL_MMU_DRAINWRITEBUFFER MINIMAL_MMU_W7104
#define MINIMAL_MMU_INVALIDATEICACHE MINIMAL_MMU_W750
#define MINIMAL_MMU_INVALIDATEDCACHE MINIMAL_MMU_W760



//for blitter usage

#define BLITTER_DST_BPP(r,a) gp2x_blitter[0x00 >> 2] = ((r)<<6)|((a)<<5)       //r=1 reads dest then ROP // a=0,1 (8,16 bpp)
#define BLITTER_DST_PTR(a)   gp2x_blitter[0x04 >> 2] = (unsigned int)(a)       //manejar puntero de la forma &kk[offset]
#define BLITTER_DST_BPL(a)   gp2x_blitter[0x08 >> 2] = (a)                     //bytes per line (320, 640, 1280...)

#define BLITTER_SRC_BPP(r,a) gp2x_blitter[0x0C >> 2] = ((r)<<7)|((a)<<5)       //r=1 reads src after ROP // a=0,1 (8,16 bpp) 
#define BLITTER_SRC_PTR(a)   gp2x_blitter[0x10 >> 2] = (unsigned int)(a)       //manejar puntero de la forma &kk[offset]
#define BLITTER_SRC_BPL(a)   gp2x_blitter[0x14 >> 2] = (a)                     //bytes per line (320, 640, 1280...)

#define BLITTER_DIM(w,h)     gp2x_blitter[0x2c >> 2] = ((h)<<16) | (w)

#define NO_MIRROR 0
#define MIRROR_W  1
#define MIRROR_H  2

#define ROP_P     (0xF0) /*PATERN*/
#define ROP_S     (0xCC) /*SOURCE*/
#define ROP_D     (0xAA) /*DESTINATION*/
#define ROP_0     (0x00) /*FALSE*/
#define ROP_1     (0xFF) /*TRUE*/

#define BLITTER_ROP(transp_color,transp_on,fifoclear,ydirxdir,rop) gp2x_blitter[0x30 >> 2] = (((transp_color)<<16)|((transp_on)<<11)|((fifoclear)<<10)|((ydirxdir)<<8)|((rop)<<0))
#define BLITTER_GO()         gp2x_blitter[0x34 >> 2] = 1

#define BLITTER_FG(fg) gp2x_blitter[0x18 >> 2] = fg
#define BLITTER_BG(bg) gp2x_blitter[0x1c >> 2] = bg

#define BLITTER_PATTERN_FG_COLOR(fg565) gp2x_blitter[0x24 >> 2] = fg565
#define BLITTER_PATTERN_BG_COLOR(bg565) gp2x_blitter[0x28 >> 2] = bg565

#define BLITTER_PATTERN(mono, on, bpp, offset) gp2x_blitter[0x20 >> 2] = (((mono)<<6)|((on)<<5)|((bpp)<<3)|((offset)<<0)) 

#define BLITTER_PATTERN_SET(x) gp2x_blitter[0x80 >> 2] = x


#if 0
 examples
 for a custom ROP to do destination = (source & ~destination ) ^ ~pattern;
 I'd use this: #define MYROP         ((ROP_S & ~ROP_D ) ^ ~ROP_P)
 
 more examples using this:
 PATCOPY	 : D = P 
 PATINVERT	 : D = P ^ D 
 DSTINVERT	 : D = ~D 
 SRCCOPY	 : D = S 
 NOTSRCCOPY	 : D = ~S 
 SRCINVERT	 : D = S ^ D 
 SRCAND		 : D = S & D 
 SRCPAINT	 : D = S | D 
 SRCERASE	 : D = S & ~D 
 NOTSRCERASE : D = ~S & ~D 
 MERGEPAINT	 : D = ~S | D 
 MERGECOPY	 : D = S & P 
 PATPAINT	 : D = D | P | ~S 
#endif


//for our dualcore solution

#define GP2X_QUEUE_MAX_ITEMS           ((4096 - sizeof(gp2x_queue)) / 4) 
#define GP2X_QUEUE_STRUCT_PTR          (0                  + 0x1000)
#define GP2X_QUEUE_DATA_PTR            (sizeof(gp2x_queue) + 0x1000)

#define gp2x_2ndcore_code(v)           (*(volatile unsigned long *)(v))
#define gp2x_1stcore_code(v)           gp2x_dualcore_ram[(v)>>2]
#define gp2x_2ndcore_data(v)           gp2x_2ndcore_code((v)+0x1ff6000)
#define gp2x_1stcore_data(v)           gp2x_1stcore_code((v)+0x1ff6000)

#define gp2x_2ndcore_code_ptr(v)       ((volatile unsigned long *)(v))
#define gp2x_1stcore_code_ptr(v)       (&gp2x_dualcore_ram[(v)>>2])
#define gp2x_2ndcore_data_ptr(v)       gp2x_2ndcore_code_ptr((v)+0x1ff6000)
#define gp2x_1stcore_data_ptr(v)       gp2x_1stcore_code_ptr((v)+0x1ff6000)

#define gp2x_dualcore_data(v)          gp2x_1stcore_data((v)<<2)
#define gp2x_dualcore_data_ptr(v)      gp2x_1stcore_data_ptr((v)<<2)

#define gp2x_dualcore_declare_subprogram(name) extern void gp2x_dualcore_launch_## name ##_subprogram(void);
#define gp2x_dualcore_launch_subprogram(name)  gp2x_dualcore_launch_## name ##_subprogram()




#ifdef MINIMAL_940T

#undef    gp2x_dualcore_data
#define   gp2x_dualcore_data(v)         gp2x_2ndcore_data((v)<<2)

#undef    gp2x_dualcore_data_ptr
#define   gp2x_dualcore_data_ptr(v)     gp2x_2ndcore_data_ptr((v)<<2)

#define   main                          gp2x_2ndcore_run

static void gp2x_2ndcore_start(void) __attribute__((naked));

static void gp2x_2ndcore_start(void)
{
  unsigned long gp2x_dequeue(gp2x_queue *q)
  {
   unsigned long data;

   while(!q->items); //waiting for head to increase...

   data=q->place940t[q->tail = (q->tail < q->max_items ? q->tail+1 : 0)];
   q->items--;

   return data;
  }
}

#define gp2x_dualcore_name_subprogram(name) \
/* 00000020 <_start>:                                                                                   */ \
/*    0:*/  asm volatile (".word 0xe59ff02c");  /*        ldr     pc, [pc, #44]  ; 34 <ioffset>         */ \
/*    4:*/  asm volatile (".word 0xe59ff028");  /*        ldr     pc, [pc, #40]  ; 34 <ioffset>         */ \
/*    8:*/  asm volatile (".word 0xe59ff024");  /*        ldr     pc, [pc, #36]  ; 34 <ioffset>         */ \
/*    c:*/  asm volatile (".word 0xe59ff020");  /*        ldr     pc, [pc, #32]  ; 34 <ioffset>         */ \
/*   10:*/  asm volatile (".word 0xe59ff01c");  /*        ldr     pc, [pc, #28]  ; 34 <ioffset>         */ \
/*   14:*/  asm volatile (".word 0xe59ff018");  /*        ldr     pc, [pc, #24]  ; 34 <ioffset>         */ \
/*   18:*/  asm volatile (".word 0xe59ff014");  /*        ldr     pc, [pc, #20]  ; 34 <ioffset>         */ \
/*   1c:*/  asm volatile (".word 0xe59ff010");  /*        ldr     pc, [pc, #16]  ; 34 <ioffset>         */ \
/* 00000020 <_init>:                                                                                    */ \
/*   20:*/  asm volatile (".word 0xe59fd010");  /*        ldr     sp, [pc, #16]   ; 38 <stack>          */ \
/*   24:*/  asm volatile (".word 0xe59fc010");  /*        ldr     ip, [pc, #16]   ; 3c <deadbeef>       */ \
/*   28:*/  asm volatile (".word 0xe59fb010");  /*        ldr     fp, [pc, #16]   ; 40 <leetface>       */ \
/*   2c:*/  asm volatile (".word 0xe92d1800");  /*        stmdb   sp!, {fp, ip}                         */ \
/*   30:*/  asm volatile (".word 0xea000004");  /*        b       48 <realinit>                         */ \
/* 00000034 <ioffset>:                                                                                  */ \
/*   34:*/  asm volatile (".word 0x00000020");  /*                                                      */ \
/* 00000038 <stack>:                                                                                    */ \
/*   38:*/  asm volatile (".word 0x01fffffc");  /*                                                      */ \
/* 0000003c <deadbeef>:                                                                                 */ \
/*   3c:*/  asm volatile (".word 0xdeadbeef");  /*                                                      */ \
/* 00000040 <leetface>:                                                                                 */ \
/*   40:*/  asm volatile (".word 0x1ee7face");  /*                                                      */ \
/* 00000044 <area1>:                                                                                    */ \
/*   44:*/  asm volatile (".word 0x01ff6019");  /*                                                      */ \
/* 00000048 <realinit>:                                                                                 */ \
/*  our main code starts here...                                                                        */ \
/*   48:*/  asm volatile (".word 0xe3a0003f");  /*        mov     r0, #63 ; 0x3f                        */ \
/*   4c:*/  asm volatile (".word 0xee060f10");  /*        mcr     15, 0, r0, cr6, cr0, {0}              */ \
/*   50:*/  asm volatile (".word 0xee060f30");  /*        mcr     15, 0, r0, cr6, cr0, {1}              */ \
/*   54:*/  asm volatile (".word 0xe3a0001f");  /*        mov     r0, #31 ; 0x1f                        */ \
/*   58:*/  asm volatile (".word 0xe38004be");  /*        orr     r0, r0, #-1107296256    ; 0xbe000000  */ \
/*   5c:*/  asm volatile (".word 0xee060f11");  /*        mcr     15, 0, r0, cr6, cr1, {0}              */ \
/*   60:*/  asm volatile (".word 0xe51f0024");  /*        ldr     r0, [pc, #-36]  ; 44 <area>           */ \
/*   64:*/  asm volatile (".word 0xee060f12");  /*        mcr     15, 0, r0, cr6, cr2, {0}              */ \
/*   68:*/  asm volatile (".word 0xe3a00001");  /*        mov     r0, #1  ; 0x1                         */ \
/*   6c:*/  asm volatile (".word 0xee020f10");  /*        mcr     15, 0, r0, cr2, cr0, {0}              */ \
/*   70:*/  asm volatile (".word 0xe3a00001");  /*        mov     r0, #1  ; 0x1                         */ \
/*   74:*/  asm volatile (".word 0xee020f30");  /*        mcr     15, 0, r0, cr2, cr0, {1}              */ \
/*   78:*/  asm volatile (".word 0xe3a00001");  /*        mov     r0, #1  ; 0x1                         */ \
/*   7c:*/  asm volatile (".word 0xee030f10");  /*        mcr     15, 0, r0, cr3, cr0, {0}              */ \
/*   80:*/  asm volatile (".word 0xe3a000ff");  /*        mov     r0, #255        ; 0xff                */ \
/*   84:*/  asm volatile (".word 0xee050f10");  /*        mcr     15, 0, r0, cr5, cr0, {0}              */ \
/*   88:*/  asm volatile (".word 0xe3a00003");  /*        mov     r0, #3  ; 0x3                         */ \
/*   8c:*/  asm volatile (".word 0xee050f30");  /*        mcr     15, 0, r0, cr5, cr0, {1}              */ \
/*   90:*/  asm volatile (".word 0xe3a00000");  /*        mov     r0, #0  ; 0x0                         */ \
/*   94:*/  asm volatile (".word 0xe3800001");  /*        orr     r0, r0, #1      ; 0x1                 */ \
/*   98:*/  asm volatile (".word 0xe380007c");  /*        orr     r0, r0, #124    ; 0x7c                */ \
/*   9c:*/  asm volatile (".word 0xe3800a01");  /*        orr     r0, r0, #4096   ; 0x1000              */ \
/*   a0:*/  asm volatile (".word 0xe3800103");  /*        orr     r0, r0, #-1073741824    ; 0xc0000000  */ \
/*   a4:*/  asm volatile (".word 0xee010f10");  /*        mcr     15, 0, r0, cr1, cr0, {0}              */ \
   asm volatile ("mcr p15, 0, r0, c7, c10, 4" ::: "r0"); \
   while(1) gp2x_2ndcore_run(gp2x_dequeue((gp2x_queue *)gp2x_2ndcore_data_ptr(GP2X_QUEUE_STRUCT_PTR))); \
} \
void gp2x_dualcore_launch_##name##_subprogram(void) { gp2x_dualcore_launch_program((unsigned long *)&gp2x_2ndcore_start, ((int)&gp2x_dualcore_launch_##name##_subprogram)-((int)&gp2x_2ndcore_start)); }

#endif


#if 0

@ 940Tcrt0.s for the fake-crt0s naked boombastic code in minimal_940t.h
@ Written by rlyeh, (c) 2005-2006.


      .section".init"
      .code 32
      .align
      .global _start
 
_start:
      LDR PC, [pc, #44]              @ Reset
      LDR PC, [pc, #40]              @ Undefined instruction
      LDR PC, [pc, #36]              @ Software Interrupt
      LDR PC, [pc, #32]              @ Prefetch abort
      LDR PC, [pc, #28]              @ Data abort
      LDR PC, [pc, #24]              @ Reserved
      LDR PC, [pc, #20]              @ IRQ
      LDR PC, [pc, #16]              @ FIQ
 
_init:
      LDR   SP, stack                @ Load our stack at top of 64mb (32mb from 940t)
      LDR   IP, deadbeef             @ Load deadbeef
      LDR   FP, leetface             @ Load face0000
      STMDB R13!, {R11,R12}
      .word 0xea000004               @ B realinit   
                                     @ putting B instead of .word may causes some troubles
                                     @ in *our* specific circumstances.
ioffset:
      .word  0x00000020
stack:          
      .word  0x01fffffc
deadbeef:
      .word  0xdeadbeef              @ Squidge's legacy :-)
leetface:
      .word  0x1ee7face

area:
      .word  0x01ff6019              @ at 920t's relative offset 0x4000000-0x8000(stack)-0x2000(shared area, where I map to)
                                     @ 00000011000b = 8kb | 00000000001b = enable
 
realinit:
      MOV R0, #0x3F                  @ 4Gb page starting at 0x000000
      MCR p15, 0, R0,c6,c0, 0        @ Set as data memory region 0
      MCR p15, 0, R0,c6,c0, 1        @ Set as instruction memory region 0

      MOV R0, #0x1F                  @ Region 1 for MP registers, 0xc0000000-0x2000000
      ORR R0, R0, #0xBE000000        @ Base at registers start
      MCR p15, 0, r0,c6,c1, 0        @ Set as data memory region 1

      LDR R0, area                   @ 8kb page. This is our dualcore FIFO queue
      MCR p15, 0, R0,c6,c2, 0        @ Set as data memory region 2
     @MCR p15, 0, R0,c6,c2, 1        @ Set as instruction memory region 2

     @Region 0 is cacheable/bufferable
     @Region 1 is uncacheable/unbufferable
     @Region 2 is uncacheable/bufferable

      MOV R0, #0x01                  @ Region 0 is cachable/bufferable
      MCR p15, 0, R0,c2,c0, 0        @ Write data cachable bits
      MOV R0, #0x01                   
      MCR p15, 0, R0,c2,c0, 1        @ Write instruction cachable bits
      MOV R0, #0x01                   
      MCR p15, 0, R0,c3,c0, 0        @ Write bufferable bits

      MOV R0, #0xFF                  @ Full access in all areas (read+write)
      MCR p15, 0, R0,c5,c0, 0        @ Write dataspace access permissions
      MOV R0, #0x03                  @ Only region 1 has inst permissions
      MCR p15, 0, R0,c5,c0, 1        @ Write inst space access permissions to region 1

      MOV R0, #0x00
     @MRC p15, 0, R0,c1,c0, 0        @ Read control register
      ORR R0, R0, #1                 @ Protection unit enabled
      ORR R0, R0, #0x7C              @ DCache enabled (should be #0x4?)
      ORR R0, R0, #0x1000            @ ICache enabled
      ORR R0, R0, #0xC0000000        @ Async clocking + FastBus
      MCR p15, 0, R0,c1,c0, 0        @ Set control register

.pool
.end


@Improvement:
@Maybe I should enable cache at shared area, and flush cache
@before checking/droping dualcore messages.

@some tools for this:
@MOV R0, #0
@MCR p15, 0, R0, c7, c5,  0 @Flush ICache
@MCR p15, 0, R0, c7, c6,  0 @Flush DCache
@MCR p15, 0, R0, c7, c10, 4 @Drain write buffer
@MOV R0, address to prefetch
@MCR p15, 0, R0, c7, c13, 1 @ICache prefetch


#endif

#endif
