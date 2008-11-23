/*
  GP2X minimal library v0.A by rlyeh, 2005. emulnation.info@rlyeh (swap it!)

  + GP2X 920t/940t CPUs library with a FIFO message system.
  + GP2X video library with double buffering.
  + GP2X sound library with double buffering.
  + GP2X blitter library.
  + GP2X timer library.
  + GP2X joystick library.

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)


  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.


  Known projects using the library or parts from it
  =================================================

  REminiscence-0.1.8 (rlyeh)
  Atari800 GP2X pre-release 3 (foft)
  XUMP (kedo)
  MAME2X (Franxis)
  DrMD for GP2X (Reesy)


  What's new
  ==========

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

#include "minimal.h"

         unsigned char  *gp2x_screen8                 ,*gp2x_upperRAM;
         unsigned long   gp2x_dev[8]={0,0,0,0,0,0,0,0}, gp2x_physvram[8], *gp2x_memregl,       gp2x_volume,                      gp2x_ticks_per_second;
volatile unsigned long   gp2x_sound_pausei=1,           gp2x_ticks=0,      gp2x_sound=0,      *gp2x_dualcore_ram;
         unsigned short *gp2x_memregs,                 *gp2x_screen15,    *gp2x_logvram15[4],  gp2x_sound_buffer[4+((44100/25)*2)*8]; /* 25 Hz gives our biggest supported sampling buffer */
volatile unsigned short  gp2x_palette[512];
         pthread_t       gp2x_sound_thread=0;
	 struct termios  initial_settings, new_settings;
	 unsigned char   keybuffer[64];
static   int             gp2x_usbjoy[4];
         unsigned long   gp2x_usbjoys;

/* Credits: GnoStiC */

unsigned char gp2x_keyboard_read(void) {
	if (gp2x_dev[5] == -1) { return 0; }
	unsigned char key;

	read(gp2x_dev[5],&key,1);

	if ((key&0x80)) { key = 0; } else { key = key & 0x7F; }
	return key;
}

unsigned int gp2x_keyboard_readext(void) {
	if (gp2x_dev[5] == -1) { return 0; }
	return read(gp2x_dev[5], keybuffer, 64);
}

unsigned int gp2x_keyboard_ledget(void) {
	unsigned long int leds;
	ioctl(gp2x_dev[5], KDGETLED, &leds);
	return leds;
/*
if ( == LED_SCROLLLOCK) printf ("Scroll Lock led is on\n");
else if (arg == LED_NUMLOCK) printf ("Num Lock led is on\n");
else if (arg == LED_CAPSLOCK) printf ("Caps Lock led is on\n");
else if (arg == LED_NUMLOCK + LED_CAPLOCK) printf("Num Lock & Caps Lock leds are on\n");
else if (arg == LED_NUMLOCK + LED_SCROLLLOCK) printf("Num Lock & Scroll Lock leds are on\n");
else if (arg == LED_CAPLOCK + LED_SCROLLLOCK) printf("Caps Lock & Scroll Lock LEDs are on\n");
else if (arg == LED_NUMLOCK + LED_SCROLLLOCK + LED_CAPLOCK) printf("Num Lock, Scroll Lock & Caps Lock leds are on\n");
*/
}

void gp2x_keyboard_ledset(unsigned int led) {
	ioctl(gp2x_dev[5], KDSETLED, led);
}

void gp2x_video_flip(void)
{
  unsigned long address=gp2x_physvram[gp2x_physvram[7]];

  if(gp2x_physvram[7]==0) gp2x_physvram[7]=1;
  else gp2x_physvram[7]=0;
  
  gp2x_screen15=gp2x_logvram15[gp2x_physvram[7]]; 
  gp2x_screen8=(unsigned char *)gp2x_screen15; 

  gp2x_memregs[0x290E>>1]=(unsigned short)(address & 0xFFFF);
  gp2x_memregs[0x2910>>1]=(unsigned short)(address >> 16);
  gp2x_memregs[0x2912>>1]=(unsigned short)(address & 0xFFFF);
  gp2x_memregs[0x2914>>1]=(unsigned short)(address >> 16);
}

void gp2x_video_setgamma(unsigned short gamma) /*0..255*/
{
  int i=256*3; 
  gp2x_memregs[0x295C>>1]=0;                                                     
  while(i--) gp2x_memregs[0x295E>>1]=gamma; 
}

void gp2x_video_setpalette(void)
{
  unsigned short *g=(unsigned short *)gp2x_palette; int i=512;
  gp2x_memregs[0x2958>>1]=0;                                                     
  while(i--) gp2x_memregs[0x295A>>1]=*g++; 
}

void gp2x_blitter_rect15(gp2x_rect *r)
{
 int x, y; unsigned short *data=r->data15, *offset=&gp2x_screen15[r->x+r->y*320];

 y=r->h; if(r->solid)
         while(y--) { x=r->w; while(x--) *offset++=*data++; offset+=320-x; }
         else
         while(y--) { x=r->w; while(x--) { if(*data) *offset=*data; offset++, data++; }
                      offset+=320-x; }
}

void gp2x_blitter_rect8(gp2x_rect *r)
{
 int x, y; unsigned char *data=r->data8,   *offset=&gp2x_screen8[r->x+r->y*320]; 

 y=r->h; if(r->solid)
         while(y--) { x=r->w; while(x--) *offset++=*data++; offset+=320-x; }
         else
         while(y--) { x=r->w; while(x--) { if(*data) *offset=*data; offset++, data++; }
                      offset+=320-x; }
}
 

/*
   ,--------------------.
   |                    |X
   |   GP2X - JOYSTICK  |X
   |                    |X
   `--------------------'X
    XXXXXXXXXXXXXXXXXXXXXX 
*/

/* Internal function which inits usb joystick variables.

   Credits:
   rlyeh */

static void gp2x_joystick_init (void)
{
 char device[32]; int i=-1; gp2x_usbjoys = 0;

 while(++i<4) { sprintf (device, "/dev/input/js%d", i); 
                if((gp2x_usbjoy[i] = open(device, O_RDONLY|O_NONBLOCK, 0)) >0) gp2x_usbjoys++; }
#if 0
	printf("Joystick(s) found: %d\n",gp2x_usbjoys+1);
 for (i=0;i<gp2x_usbjoys+1;i++) {
	printf("Joystick %d: %s\n",i,gp2x_joystick_name(i));
 }
#endif
}


/* Internal function which deinits usb joystick variables.

   Credits:
   rlyeh */

static void gp2x_joystick_deinit(void)
{
 int i; for(gp2x_usbjoys=i=0;i<4;i++) if(gp2x_usbjoy[i] > 0) close(gp2x_usbjoy[i]);
}


/* Function: gp2x_joystick_scan
   This function forces a new scan looking for recently plugged joysticks.

   Note:
   - Notice that Minimal Library scans already for plugged USB joysticks at start of program.
   - It would be wise to check, after calling this function, new and old joysticks both together with <gp2x_joystick_name>
   
   See also:
   <gp2x_joystick_name>, <gp2x_usbjoys>

   Credits:
   rlyeh */

void gp2x_joystick_scan(void)
{
 gp2x_joystick_deinit();
 gp2x_joystick_init();
}

/* Function: gp2x_joystick_read
   This function returns the active <GP2X joystick values>.

   Usage:
   Call this function once per frame to keep your joystick values updated.

   Parameters:
   joystick - 0 for GP2X pad, 1 for first USB joystick, 2 for second USB joystick, etc...

   Note:
   In order to detect simultaneous buttons you will have to mask the value.

   See also:
   <gp2x_joystick_wait>, <GP2X joystick values>, <gp2x_usbjoys>

   Example:
   > unsigned long pad=gp2x_joystick_read(0);
   >
   > if(pad==GP2X_A) ...               //check that only A is pressed.
   > if(pad&GP2X_A)  ...               //check that A is pressed, despite the other buttons.
   > if(pad&GP2X_R) if(pad&GP2X_L) ... //check that both L and R are pressed, despite the other buttons.

   Credits:
   Puck2099 and GnoStiC (original code)
   rlyeh */

unsigned long gp2x_joystick_read(int joystick)
{
 unsigned long value;
 
 if(!joystick)
 {
  value=(gp2x_memregs[0x1198>>1] & 0x00FF);

  if(value==0xFD) value=0xFA;
  if(value==0xF7) value=0xEB;
  if(value==0xDF) value=0xAF;
  if(value==0x7F) value=0xBE;
  
  value = (~((gp2x_memregs[0x1184>>1] & 0xFF00) | value | (gp2x_memregs[0x1186>>1] << 16))) & (~0xc0040000);
  
//  if(gp2x_f200)
//    value |= ((value & (GP2X_LEFT|GP2X_RIGHT|GP2X_UP|GP2X_DOWN)) == (GP2X_LEFT|GP2X_RIGHT|GP2X_UP|GP2X_DOWN) ? GP2X_PUSH : 0);
  
  return value;
 }
 else
 { 
  struct JS_DATA_TYPE js; int i;

  if(read(gp2x_usbjoy[joystick-1], &js, JS_RETURN) != JS_RETURN) return 0;

  i = js.buttons;
  value  = ((i &    1)>> 0) * (GP2X_Y); /* these values should be optimized at compilation time */
  value |= ((i &    2)>> 1) * (GP2X_B);
  value |= ((i &    4)>> 2) * (GP2X_X);
  value |= ((i &    8)>> 3) * (GP2X_A);
  value |= ((i &   16)>> 4) * (GP2X_L);
  value |= ((i &   32)>> 5) * (GP2X_R);
  value |= ((i &   64)>> 6) * (GP2X_L);
  value |= ((i &  128)>> 7) * (GP2X_R);
  value |= ((i &  256)>> 8) * (GP2X_SELECT);
  value |= ((i &  512)>> 9) * (GP2X_START);
  value |= ((i & 1024)>>10) * (GP2X_PUSH);
  value |= ((i & 2048)>>11) * (GP2X_PUSH);

  if(js.x & 0x7F) value |= (js.x & 0x80 ? GP2X_RIGHT : GP2X_LEFT);
  if(js.y & 0x7F) value |= (js.y & 0x80 ? GP2X_DOWN  : GP2X_UP);
 }
 return value;
} 

/* Function: gp2x_joystick_name
   This function returns name for joystick.

   Note:
   - Function returns an empty string if joystick not found.

   Parameters:
   joystick - 0 for GP2X pad, 1 for first USB joystick, 2 for second USB joystick, etc...

   Example:
   > printf(gp2x_joystick_name(0)); // printfs "GP2X gamepad"
   > printf(gp2x_joystick_name(1)); // printfs 1st usb joystick name if plugged

   See also:
   <gp2x_joystick_scan>, <gp2x_usbjoys>

   Credits:
   rlyeh */

char *gp2x_joystick_name(int joystick)
{
 static char name[128];
 
 sprintf(name, !joystick ? "GP2X gamepad" : "");
 if(joystick>0) if(gp2x_usbjoy[joystick-1] > 0) ioctl(gp2x_usbjoy[joystick-1], JSIOCGNAME(sizeof(name)), name); 
 
 return name;
}

/* Function: gp2x_joystick_wait
   This function waits for joystick to press *and* release a combination.

   Parameters:
   joystick - 0 for GP2X pad, 1 for first USB joystick, 2 for second USB joystick, etc...
   combination - a combination of or'ed <GP2X joystick values> values

   Example:
   > gp2x_joystick_wait(1, GP2X_A | GP2X_START); // waits until A & START are simultaneously pressed and released on usb joystick #1

   See also:
   <gp2x_joystick_read>, <GP2X joystick values>, <gp2x_usbjoys>

   Credits:
   rlyeh */

void gp2x_joystick_wait(int joystick, unsigned long combination)
{
 while(gp2x_joystick_read(joystick) != combination);
 while(gp2x_joystick_read(joystick) == combination);
}

void gp2x_sound_volume(int l, int r)
{
 l=(((l*0x50)/100)<<8)|((r*0x50)/100);          /* 0x5A, 0x60 */
 ioctl(gp2x_dev[4], SOUND_MIXER_WRITE_PCM, &l); /* SOUND_MIXER_WRITE_VOLUME */
}

void gp2x_timer_delay(unsigned long ticks)
{
 unsigned long target=gp2x_memregl[0x0A00>>2]+ticks*gp2x_ticks_per_second;
 while(gp2x_memregl[0x0A00>>2]<target); 
}

unsigned long gp2x_timer_read(void)
{
 return gp2x_memregl[0x0A00>>2]/gp2x_ticks_per_second;
}

void gp2x_sound_pause(int yes) { gp2x_sound_pausei=yes; }

static void gp2x_initqueue(gp2x_queue *q, unsigned long queue_items, unsigned long *position920t, unsigned long *position940t)
{
  q->head  = q->tail  = q->items = 0;
  q->max_items = queue_items;
  if(position920t) q->place920t=position920t; else q->place920t=(unsigned long *)malloc(sizeof(unsigned long) * queue_items);
  if(position940t) q->place940t=position940t;
  memset(q->place920t, 0, sizeof(unsigned long) * queue_items);
}

static void gp2x_enqueue(gp2x_queue *q, unsigned long data)
{	
  while(q->items==q->max_items); /*waiting for tail to decrease...*/
  q->place920t[q->head = (q->head < q->max_items ? q->head+1 : 0)] = data;
  q->items++;
}

       void gp2x_dualcore_pause(int yes) { if(yes) gp2x_memregs[0x0904>>1] &= 0xFFFE; else gp2x_memregs[0x0904>>1] |= 1; }
static void gp2x_940t_reset(int yes)     { gp2x_memregs[0x3B48>>1] = ((yes&1) << 7) | (0x03); }
static void gp2x_940t_pause(int yes)     { gp2x_dualcore_pause(yes); }

static void gp2x_dualcore_registers(int save)
{
 static unsigned short regs[8];

 if(save)
 {
  regs[0]=gp2x_memregs[0x0904>>1];  regs[1]=gp2x_memregs[0x0912>>1];
  regs[2]=gp2x_memregs[0x091c>>1];  regs[3]=gp2x_memregs[0x3b40>>1];
  regs[4]=gp2x_memregs[0x3b42>>1];  regs[5]=gp2x_memregs[0x3b48>>1];
  regs[6]=gp2x_memregs[0x3b44>>1];  regs[7]=gp2x_memregs[0x3b46>>1];

  gp2x_940t_reset(1);
  gp2x_940t_pause(1);
 }
 else
 {
  gp2x_memregs[0x0904>>1]=regs[0];  gp2x_memregs[0x0912>>1]=regs[1];
  gp2x_memregs[0x091c>>1]=regs[2];  gp2x_memregs[0x3b40>>1]=regs[3];
  gp2x_memregs[0x3b42>>1]=regs[4];  gp2x_memregs[0x3b48>>1]=regs[5];
  gp2x_memregs[0x3b44>>1]=regs[6];  gp2x_memregs[0x3b46>>1]=regs[7];
 }
}

void gp2x_dualcore_sync(void)
{
  gp2x_queue *q=(gp2x_queue *)gp2x_1stcore_data_ptr(GP2X_QUEUE_ARRAY_PTR);
  while(q->items);
}

void gp2x_dualcore_exec(unsigned long command) { gp2x_enqueue((gp2x_queue *)gp2x_1stcore_data_ptr(GP2X_QUEUE_ARRAY_PTR),command); }

void gp2x_dualcore_launch_program(unsigned long *area, unsigned long size)
{
  unsigned long i=0, *arm940t_ram=(unsigned long *)gp2x_dualcore_ram;
  
  gp2x_940t_reset(1);

  gp2x_memregs[0x3B40>>1] = 0;                               /* disable interrupts */
  gp2x_memregs[0x3B42>>1] = 0;
  gp2x_memregs[0x3B44>>1] = 0xffff;
  gp2x_memregs[0x3B46>>1] = 0xffff;

  gp2x_940t_pause(0);            
                             
  while(i < size) *arm940t_ram++=area[i++];

  gp2x_initqueue((gp2x_queue *)gp2x_1stcore_data_ptr(GP2X_QUEUE_ARRAY_PTR), GP2X_QUEUE_MAX_ITEMS, (unsigned long *)gp2x_1stcore_data_ptr(GP2X_QUEUE_DATA_PTR), (unsigned long *)gp2x_2ndcore_data_ptr(GP2X_QUEUE_DATA_PTR));

  gp2x_940t_reset(0);
}

void gp2x_dualcore_launch_program_from_disk(const char *file, unsigned long offset, unsigned long size)
{
 FILE *in; void *data;

 if((in=fopen(file, "rb"))==NULL) return;
 if((data=malloc(size))==NULL) { fclose(in); return; }
 fseek(in, 0L, offset);
 fread(data, 1, size, in);
 gp2x_dualcore_launch_program((unsigned long *)data, size);
 free(data);
 fclose(in);
}

void gp2x_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz)
{
  struct fb_fix_screeninfo fixed_info;
  static int first=1;

  system("/sbin/insmod input    &> /dev/null");
  system("/sbin/insmod joydev   &> /dev/null");
  system("/sbin/insmod keybdev  &> /dev/null");
  system("/sbin/insmod mousedev &> /dev/null");

  gp2x_ticks_per_second=7372800/ticks_per_second; 

  if(!gp2x_dev[0])   gp2x_dev[0] = open("/dev/fb0",   O_RDWR);
  if(!gp2x_dev[1])   gp2x_dev[1] = open("/dev/fb1",   O_RDWR);
  if(!gp2x_dev[2])   gp2x_dev[2] = open("/dev/mem",   O_RDWR); 
  if(!gp2x_dev[4])   gp2x_dev[4] = open("/dev/mixer", O_RDWR);
  if(!gp2x_dev[5])   {
		     gp2x_dev[5] = -1;
		     gp2x_dev[5] = open("/dev/tty0",  O_RDWR | O_NDELAY, 0);
	if (gp2x_dev[5] < 0) {
		     gp2x_dev[5] = open("/dev/vc/0",  O_RDWR | O_NDELAY, 0);
	}

	if (gp2x_dev[5] >= 0) {
		tcgetattr(gp2x_dev[5], &initial_settings);
		new_settings = initial_settings;
		new_settings.c_lflag &= ~(ICANON | ECHO | ISIG);
		new_settings.c_iflag &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON);
		new_settings.c_cc[VMIN] = 0;
		new_settings.c_cc[VTIME] = 0;
		tcsetattr(gp2x_dev[5], TCSAFLUSH, &new_settings);
		//tcsetattr(gp2x_dev[5], TCSANOW, &new_settings);

		ioctl(gp2x_dev[5], KDSKBMODE, K_MEDIUMRAW); /* 7bits, 8th bit press/release */
		ioctl(gp2x_dev[5], KDSETMODE, KD_GRAPHICS);
		ioctl(gp2x_dev[5], KDSETLED, 0);
		printf("keyboard enabled\n");

#if 0
              int data = 0;
              ioctl(gp2x_dev[5], KDGKBTYPE, &data);
              if(data==KB_84||data==KB_101) printf("!!\n");
              printf("kb %d\n",data);
#endif
	}
  }
              gp2x_joystick_init(); /* open joystick devices */

  gp2x_dualcore_ram=(unsigned long  *)mmap(0, 0x1000000, PROT_READ|PROT_WRITE, MAP_SHARED, gp2x_dev[2], 0x03000000);
       gp2x_memregl=(unsigned long  *)mmap(0, 0x10000,   PROT_READ|PROT_WRITE, MAP_SHARED, gp2x_dev[2], 0xc0000000);
        gp2x_memregs=(unsigned short *)gp2x_memregl;

  if(first) { printf(MINILIB_VERSION "\n");
              gp2x_dualcore_registers(1); 
              gp2x_sound_volume(100,100);
              gp2x_memregs[0x0F16>>1] = 0x830a; usleep(100000);
              gp2x_memregs[0x0F58>>1] = 0x100c; usleep(100000); }

  ioctl(gp2x_dev[gp2x_physvram[7]=0], FBIOGET_FSCREENINFO, &fixed_info); 
   gp2x_screen15=gp2x_logvram15[2]=gp2x_logvram15[0]=(unsigned short *)mmap(0, 320*240*2, PROT_WRITE, MAP_SHARED, gp2x_dev[0], 0);
                                        gp2x_screen8=(unsigned char *)gp2x_screen15;
                   gp2x_physvram[2]=gp2x_physvram[0]=fixed_info.smem_start;

  ioctl(gp2x_dev[1], FBIOGET_FSCREENINFO, &fixed_info);
                 gp2x_logvram15[3]=gp2x_logvram15[1]=(unsigned short *)mmap(0, 320*240*2, PROT_WRITE, MAP_SHARED, gp2x_dev[1], 0);
                   gp2x_physvram[3]=gp2x_physvram[1]=fixed_info.smem_start;

  gp2x_memregs[0x28DA>>1]=(((bpp+1)/8)<<9)|0xAB; /*8/15/16/24bpp...*/
  gp2x_memregs[0x290C>>1]=320*((bpp+1)/8);       /*line width in bytes*/

  memset(gp2x_screen15, 0, 320*240*2); gp2x_video_flip();
  memset(gp2x_screen15, 0, 320*240*2); gp2x_video_flip();

  if(bpp==8)  gp2x_physvram[2]+=320*240,    gp2x_physvram[3]+=320*240,
             gp2x_logvram15[2]+=320*240/2, gp2x_logvram15[3]+=320*240/2; 

}


extern int fcloseall(void);
void gp2x_deinit(void)
{
  while((gp2x_sound++)<1000000);                               /* wait arm920t threads to finish */

  gp2x_dualcore_registers(0);

  gp2x_memregs[0x28DA>>1]=0x4AB;                               /* set video mode */
  gp2x_memregs[0x290C>>1]=640;   
 
  gp2x_joystick_deinit();

  { unsigned int i; for(i=0;i<8;i++) if(gp2x_dev[i]) close(gp2x_dev[i]); }    /* close all devices */

  fcloseall();                                                   /* close all files */

  chdir("/usr/gp2x");                                            /* go to menu */
  execl("gp2xmenu","gp2xmenu",NULL);
}
