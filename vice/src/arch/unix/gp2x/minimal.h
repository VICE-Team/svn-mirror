/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/

#include <fcntl.h>
#include <linux/fb.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#define MINILIB_VERSION  "GP2X minimal library v0.A by rlyeh, (c) 2005."

typedef struct gp2x_queue { volatile unsigned long head, tail, items, max_items; unsigned long *place920t, *place940t; } gp2x_queue;
typedef struct gp2x_rect  { int x,y,w,h,solid; unsigned short *data15; unsigned char *data8; } gp2x_rect;

extern void gp2x_timer_delay(unsigned long ticks);
extern void gp2x_sound_pause(int yes);
extern void gp2x_dualcore_pause(int yes);
extern void             gp2x_blitter_rect15(gp2x_rect *r);
extern void             gp2x_blitter_rect8(gp2x_rect *r);



extern void             gp2x_sound_frame (void *blah, void *bufferg, int samples);

enum  { GP2X_UP=0x1,       GP2X_LEFT=0x4,       GP2X_DOWN=0x10,  GP2X_RIGHT=0x40,
        GP2X_START=1<<8,   GP2X_SELECT=1<<9,    GP2X_L=1<<10,    GP2X_R=1<<11,
        GP2X_A=1<<12,      GP2X_B=1<<13,        GP2X_X=1<<14,    GP2X_Y=1<<15,
        GP2X_VOL_UP=1<<23, GP2X_VOL_DOWN=1<<22, GP2X_PUSH=1<<27                   };


#define GP2X_QUEUE_MAX_ITEMS           16
#define GP2X_QUEUE_ARRAY_PTR           ((0x1000-(sizeof(gp2x_queue)<<2)))
#define GP2X_QUEUE_DATA_PTR            (GP2X_QUEUE_ARRAY_PTR-(GP2X_QUEUE_MAX_ITEMS<<2))

#define gp2x_2ndcore_code(v)           (*(volatile unsigned long *)(v))
#define gp2x_1stcore_code(v)           gp2x_dualcore_ram[(v)>>2]
#define gp2x_2ndcore_data(v)           gp2x_2ndcore_code((v)+0x100000)
#define gp2x_1stcore_data(v)           gp2x_1stcore_code((v)+0x100000)
#define gp2x_dualcore_data(v)          gp2x_1stcore_data(v)

#define gp2x_2ndcore_code_ptr(v)       ((volatile unsigned long *)(v))
#define gp2x_1stcore_code_ptr(v)       (&gp2x_dualcore_ram[(v)>>2])
#define gp2x_2ndcore_data_ptr(v)       gp2x_2ndcore_code_ptr((v)+0x100000)
#define gp2x_1stcore_data_ptr(v)       gp2x_1stcore_code_ptr((v)+0x100000)
                                            
#define gp2x_video_wait_vsync()        while(gp2x_memregs[0x1182>>1]&(1<<4));
#define gp2x_video_wait_hsync()        while(gp2x_memregs[0x1182>>1]&(1<<5));

#define gp2x_video_color8(C,R,G,B)     do gp2x_palette[((C)<<1)+0]=((G)<<8)|(B),gp2x_palette[((C)<<1)+1]=(R); while(0)
#define gp2x_video_color15(R,G,B,A)    ((((R)&0xF8)<<8)|(((G)&0xF8)<<3)|(((B)&0xF8)>>3)|((A)<<5))


extern volatile unsigned short  gp2x_palette[512];
extern          unsigned short *gp2x_screen15;
extern          unsigned char  *gp2x_screen8;
extern volatile unsigned long  *gp2x_dualcore_ram;

extern void             gp2x_init(int tickspersecond, int bpp, int rate, int bits, int stereo, int hz);
extern void             gp2x_deinit(void);

extern void             gp2x_video_flip(void);
extern void             gp2x_video_setpalette(void);

extern unsigned long    gp2x_joystick_read(void);

extern void             gp2x_sound_volume(int left, int right);

extern unsigned long    gp2x_timer_read(void);

extern void             gp2x_dualcore_enable(int on);
extern void             gp2x_dualcore_sync(void);
extern void             gp2x_dualcore_exec(unsigned long command);
extern void             gp2x_dualcore_launch_program(unsigned long *area, unsigned long size);
extern void             gp2x_dualcore_launch_program_from_disk(const char *file, unsigned long offset, unsigned long size);

#define gp2x_dualcore_declare_subprogram(name) extern void gp2x_dualcore_launch_## name ##_subprogram(void);
#define gp2x_dualcore_launch_subprogram(name)  gp2x_dualcore_launch_## name ##_subprogram()

#endif

