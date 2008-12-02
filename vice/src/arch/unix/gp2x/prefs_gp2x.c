/*
 * prefs_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
 *  Mustafa 'GnoStiC' Tufan <mtufan@gmail.com>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "attach.h"
#include "autostart.h"
#include "cartridge.h"
#include "imagecontents.h"
#include "input_gp2x.h"
#include "interrupt.h"
#include "machine.h"
#include "minimal.h"
#include "prefs_gp2x.h"
#include "resources.h"
#include "screenshot.h"
#include "ui_gp2x.h"
#include "uiarch.h"
#include "uitext_gp2x.h"
#include "videoarch.h"

extern volatile unsigned short *gp2x_memregs;

int vkeyb_open=0;
int prefs_open=0;
int stats_open=0;

int menu_bg, menu_fg, menu_hl;

int display_width;
int hwscaling;
int centred=1;
int tvout=0;
int tvout_pal;

void (*ui_handle_sidengine_resource)(int);
int (*ui_handle_X)(int);
void (*ui_draw_resid_string)(unsigned char *, int, int);
void (*ui_draw_memory_string)(unsigned char *, int, int, int);
int (*ui_set_ramblocks)(int);
void (*ui_attach_cart)(char *,int);

int vic20_mem=5;

int cpu_speed=200;

/* clock setting code from:
  cpuctrl for GP2X
    Copyright (C) 2005  Hermes/PS2Reality 
	Modified by Vimacs using Robster's Code and god_at_hell(LCD)
*/
#define SYS_CLK_FREQ 7372800
void set_FCLK(unsigned MHZ)
{
	unsigned v;
	unsigned mdiv,pdiv=3,scale=0;
	MHZ*=1000000;
	mdiv=(MHZ*pdiv)/SYS_CLK_FREQ;
	mdiv=((mdiv-8)<<8) & 0xff00;
	pdiv=((pdiv-2)<<2) & 0xfc;
	scale&=3;
	v=mdiv | pdiv | scale;
	gp2x_memregs[0x910>>1]=v;

	//GnoStiC: (hopefully) prevent fast speed change crashes
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
	asm volatile ("nop"::);
}

#define MENU_HEIGHT 25
#define MENU_X 32
#define MENU_Y 16
#define MENU_LS MENU_Y+8

char *blank_line="                                 ";

unsigned int mapped_up    = 0;
unsigned int mapped_down  = 0;
unsigned int mapped_left  = 0;
unsigned int mapped_right = 0;
char *mapped_up_txt    = "none";
char *mapped_down_txt  = "none";
char *mapped_left_txt  = "none";
char *mapped_right_txt = "none";


void display_set() {
	if(hwscaling) {
		display_width = 384; // 384*272
		gp2x_memregs[0x290c>>1] = display_width;		/* screen width */
		if(tvout) {
			gp2x_memregs[0x2906>>1] = 614;			/* scale horizontal */
			if(tvout_pal) gp2x_memregs[0x2908>>1] = 384;	/* scale vertical PAL */
			else gp2x_memregs[0x2908>>1] = 460;		/* scale vertical NTSC */
		} else {
			gp2x_memregs[0x2906>>1] = 1228;			/* scale horizontal */
			gp2x_memregs[0x2908>>1] = 430;			/* vertical */
		}
	} else {
		display_width = 320; //320*240
		gp2x_memregs[0x290c>>1] = display_width;		/* screen width */
		if(tvout) {
			gp2x_memregs[0x2906>>1] = 420;			/* scale horizontal */
		} else {
			gp2x_memregs[0x2906>>1] = 1024;			/* scale horizontal */
		}
			gp2x_memregs[0x2908>>1] = 320;			/* scale vertical */
	}
}

void write_snapshot(WORD unused_addr, void *filename) {
	machine_write_snapshot((char *)filename, 1, 1, 0);
}

void draw_stats(unsigned char *screen) {
	char tmp_string[41];
	int drive8_colour;
	float track=(float)drive8_half_track;
	track/=2;
	snprintf(tmp_string, 41, " %3.0f%%, %2.0ffps        drive8  %2.1f ", emu_speed, emu_fps, track);
	draw_ascii_string(screen, display_width, 0, 232, tmp_string, menu_fg, menu_bg);
	if (drive8_status) drive8_colour=C64_RED;
	else drive8_colour=C64_WHITE;
	draw_ascii_string(screen, display_width, 26*8, 232, "*", drive8_colour, menu_bg);
}

struct dir_item {
	char *name;
	unsigned int type; /* 0=dir, 1=file, 2=zip archive */
};

void sort_dir(struct dir_item *list, int num_items, int sepdir) {
	unsigned int i;
	struct dir_item temp;

	for(i=0; i<(unsigned int)(num_items-1); i++) {
		if (strcmp(list[i].name, list[i+1].name)>0) {
			temp=list[i];
			list[i]=list[i+1];
			list[i+1]=temp;
			i=0;
		}
	}
	if (sepdir) {
		for(i=0; i<(unsigned int)(num_items-1); i++) {
			if ((list[i].type!=0)&&(list[i+1].type == 0)) {
				temp=list[i];
				list[i]=list[i+1];
				list[i+1]=temp;
				i=0;
			}
		}
	}
}

/* CODE ME */
int sidemu_menu (unsigned char *screen) {
	static char *contents_list[06];

	contents_list[00] = "SID EMULATION MENU -- (START=exit)";
	contents_list[01] = "SID Engine     :";
	contents_list[02] = "SID Model      :";
	contents_list[03] = "SID Filters    :";
	contents_list[04] = "reSID sampling :";
	contents_list[05] = NULL;
	static unsigned int num_items = 4;
	unsigned int row;
	static unsigned int cursor_pos;
	int bg;
	int sidengine, sidmodel, sidfilters, sidsampling;

        draw_ascii_string (screen, display_width, MENU_X, MENU_Y, contents_list[0], menu_fg, menu_bg);

	row = 0;
	while (row < num_items) {
		if (row == cursor_pos) { bg = menu_hl; } else bg = menu_bg;
        	draw_ascii_string (screen, display_width, MENU_X, MENU_LS+(8*row), blank_line,           menu_fg, menu_bg);
        	draw_ascii_string (screen, display_width, MENU_X, MENU_LS+(8*row), contents_list[row+1], menu_fg, bg);

		switch (row) {
	       		case 0: 
				resources_get_int("SidEngine", &sidengine);
				if (sidengine) {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*1), "reSID  ", menu_fg, menu_bg);
				} else {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*1), "FastSID", menu_fg, menu_bg);
				}
				break;
       			case 1:
				resources_get_int("SidModel", &sidmodel);
				if (sidmodel) {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*2), "8580 (new)", menu_fg, menu_bg);
				} else {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*2), "6581 (old)", menu_fg, menu_bg);
				}
				break;
       			case 2:
				resources_get_int("SidFilters", &sidfilters);
				if (sidfilters) {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*3), "enabled ", menu_fg, menu_bg);
				} else {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*3), "disabled", menu_fg, menu_bg);
				}
	       		case 3: 
				resources_get_int("SidResidSampling", &sidsampling);
				if (sidsampling == 0) {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*4), "Fast         ", menu_fg, menu_bg);
				} else if (sidsampling == 1) {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*4), "Interpolation", menu_fg, menu_bg);
				} else {
					draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*4), "Resampling   ", menu_fg, menu_bg);
				}
				break;
		}
		row++;
	}
	while (row < MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	return 0;
}

int keymapping_menu (unsigned char *screen) {
	static char *contents_list[06];

	contents_list[00] = "KEYMAPPING MENU -- (START=reset)";
	contents_list[01] = "Joystick UP     :";
	contents_list[02] = "Joystick DOWN   :";
	contents_list[03] = "Joystick LEFT   :";
	contents_list[04] = "Joystick RIGHT  :";
	contents_list[05] = NULL;
	static unsigned int num_items = 4;
	unsigned int row;
	static unsigned int cursor_pos;
	int bg;

        draw_ascii_string (screen, display_width, MENU_X, MENU_Y, contents_list[0], menu_fg, menu_bg);

	row = 0;
	while (row < num_items) {
		if (row == cursor_pos) { bg = menu_hl; } else bg = menu_bg;
        	draw_ascii_string (screen, display_width, MENU_X, MENU_LS+(8*row), blank_line,           menu_fg, menu_bg);
        	draw_ascii_string (screen, display_width, MENU_X, MENU_LS+(8*row), contents_list[row+1], menu_fg, bg);

		switch (row) {
	       		case 0: draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*1), mapped_up_txt,    menu_fg, menu_bg); break;
       			case 1: draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*2), mapped_down_txt,  menu_fg, menu_bg); break;
       			case 2: draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*3), mapped_left_txt,  menu_fg, menu_bg); break;
	       		case 3: draw_ascii_string(screen, display_width, MENU_X+(8*18), MENU_Y+(8*4), mapped_right_txt, menu_fg, menu_bg); break;
		}
		row++;
	}
	while (row < MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	if (input_down) {
		input_down = 0;
		if (cursor_pos < num_items-1) cursor_pos++;
	} else if (input_up) {
		input_up = 0;
		if (cursor_pos > 0) cursor_pos--;
	} else if (input_left) {
		input_left = 0;
		return 1;
	} else if (input_a) {
		input_a = 0;
		switch (cursor_pos) {
			case 0: mapped_up_txt    = "A   "; mapped_up    = GP2X_A; break;
			case 1: mapped_down_txt  = "A   "; mapped_down  = GP2X_A; break;
			case 2: mapped_left_txt  = "A   "; mapped_left  = GP2X_A; break;
			case 3: mapped_right_txt = "A   "; mapped_right = GP2X_A; break;
		}
	} else if (input_x) {
		input_x = 0;
		switch (cursor_pos) {
			case 0: mapped_up_txt    = "X   "; mapped_up    = GP2X_X; break;
			case 1: mapped_down_txt  = "X   "; mapped_down  = GP2X_X; break;
			case 2: mapped_left_txt  = "X   "; mapped_left  = GP2X_X; break;
			case 3: mapped_right_txt = "X   "; mapped_right = GP2X_X; break;
		}
	} else if (input_y) {
		input_y = 0;
		switch (cursor_pos) {
			case 0: mapped_up_txt    = "Y   "; mapped_up    = GP2X_Y; break;
			case 1: mapped_down_txt  = "Y   "; mapped_down  = GP2X_Y; break;
			case 2: mapped_left_txt  = "Y   "; mapped_left  = GP2X_Y; break;
			case 3: mapped_right_txt = "Y   "; mapped_right = GP2X_Y; break;
		}
	} else if (input_start) {
		input_start = 0;
		switch (cursor_pos) {
			case 0: mapped_up_txt    = "none"; mapped_up    = 0; break;
			case 1: mapped_down_txt  = "none"; mapped_down  = 0; break;
			case 2: mapped_left_txt  = "none"; mapped_left  = 0; break;
			case 3: mapped_right_txt = "none"; mapped_right = 0; break;
		}
	}

	return 0;
}

char *image_file_req(unsigned char *screen, const char *image) {
	static int num_items=0;
	static int cursor_pos;
	static int first_visible;
	int bg;
	char *selected = NULL;
	char *contents_str;
	static char *contents_list[255];
	int row;
	int i;

	if (num_items == 0) {
		contents_str=image_contents_read_string(IMAGE_CONTENTS_AUTO,
				image, 8, IMAGE_CONTENTS_STRING_PETSCII);
		if (contents_str == NULL) return (char*)-1;
		contents_list[0]=contents_str;
		for(i=0; contents_str[i]; i++) {
			if (contents_str[i] == '\n') {
				num_items++;
				contents_list[num_items]=contents_str+i+1;
				contents_str[i]='\0';
			}
		}
		contents_list[num_items+1]=NULL;
		cursor_pos=0;
		first_visible=0;
	}

        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, blank_line, menu_fg, menu_bg);
        draw_petscii_string(screen, display_width, MENU_X, MENU_Y, 
		contents_list[0], menu_fg, menu_bg);

	row=0;
	while(row<(num_items-1) && row<MENU_HEIGHT) {
		if (row == (cursor_pos-first_visible)) {
			bg=menu_hl;
			selected=contents_list[row+first_visible+1];
		} else bg=menu_bg;
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), 
			blank_line, menu_fg, bg);
        	draw_petscii_string(screen, display_width, MENU_X, MENU_LS+(8*row), 
			contents_list[row+first_visible+1], menu_fg, bg);
		row++;
	}
	while(row<MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	if (input_down) {
		input_down=0;
		if (cursor_pos<(num_items-1)) cursor_pos++;
		if ((cursor_pos-first_visible)>=MENU_HEIGHT) first_visible++;
	} else if (input_up) {
		input_up=0;
		if (cursor_pos>0) cursor_pos--;
		if (cursor_pos<first_visible) first_visible--;
	} else if (input_left) {
		input_left=0;
		if (cursor_pos>=10) cursor_pos-=10;
		else cursor_pos=0;
		if (cursor_pos<first_visible) first_visible=cursor_pos;
	} else if (input_right) {
		input_right=0;
		if (cursor_pos<(num_items-11)) cursor_pos+=10;
		else cursor_pos=num_items-1;
		if ((cursor_pos-first_visible)>=MENU_HEIGHT) 
			first_visible=cursor_pos-(MENU_HEIGHT-1);
	} else if (input_b) {
		input_b=0;
		num_items=0;
		selected+=7;
		for(i=0; selected[i]!='"' && i<32; i++);
		selected[i]='\0';
		return selected;
	} else if (input_x) {
		input_x=0;
		num_items=0;
		return (char *)-1;
	}
	return NULL;
}

char *file_req(unsigned char *screen, char *dir) {
	static char *cwd=NULL;
	static int cursor_pos=1;
	static int first_visible;
	int bg;
	static int num_items=0;
	DIR *dirstream;
	struct dirent *direntry;
	static struct dir_item dir_items[1024];
	char *path;
	struct stat item;
	static int row;
	int pathlength;
	char tmp_string[32];
	char *selected;
	int i;
	static char *last_dir="";

	if (dir!=NULL) cwd=dir;
	if (cwd == NULL) cwd=".";

	if (num_items == 0) {
#ifdef __zipfile__
		if (!strcmp(cwd+(strlen(cwd)-4), ".zip")
			|| !strcmp(cwd+(strlen(cwd)-4), ".ZIP")) {
			fprintf(stderr, "reading zip archive %s\n", cwd);
			int zip_error;
			struct zip *zip_archive=zip_open(cwd, 0, &zip_error);
			int num_files=zip_get_num_files(zip_archive);
			const char *zip_file_name;
			dir_items[0].name="..";
			dir_items[0].type=0;
			num_items++;
			for(int i=0; i<num_files && i<1024; i++) {
				zip_file_name=zip_get_name(zip_archive, i, 0);
				dir_items[num_items].name=(char *)malloc(strlen(zip_file_name)+1);
				strcpy(dir_items[num_items].name, zip_file_name);
				num_items++;
			}
			zip_close(zip_archive);
			/* entry types assume all zip content is files */
			for(int i=1; i<num_items; i++) dir_items[i].type=1;
		} else {
#endif
			dirstream=opendir(cwd);
			if (dirstream == NULL) {
				printf("error opening directory %s\n", cwd);
				return (char *)-1;
			}
			/* read directory entries */
			while((direntry=readdir(dirstream))) {
				dir_items[num_items].name=(char *)malloc(strlen(direntry->d_name)+1);
				strcpy(dir_items[num_items].name, direntry->d_name);
				num_items++;
				if (num_items>1024) break;
			}
			closedir(dirstream);
			/* get entry types */
			for(i=0; i<num_items; i++) {
				path=(char *)malloc(strlen(cwd)+strlen(dir_items[i].name)+2);
				sprintf(path, "%s/%s", cwd, dir_items[i].name);
				if (!stat(path, &item)) {
					if (S_ISDIR(item.st_mode)) {
						dir_items[i].type=0;
#ifdef __zipfile__
					} else if (!strcmp(path+(strlen(path)-4), ".zip")
						|| !strcmp(path+(strlen(path)-4), ".ZIP")) {
						dir_items[i].type=2;
#endif
					} else {
						dir_items[i].type=1;
					}
				} else {
					dir_items[i].type=0;
				}
				free(path);
			}
#ifdef __zipfile__
		}
#endif
		sort_dir(dir_items, num_items, 1);
		if (strcmp(cwd, last_dir)) {
			cursor_pos=0;
			first_visible=0;
			last_dir=(char *)malloc(strlen(cwd)+1);
			strcpy(last_dir, cwd);
		}
	}

        draw_ascii_string(screen, display_width, MENU_X, MENU_Y-8, "B=load/browse, Y=unzip", menu_fg, menu_bg);
	/* display current directory */
        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, blank_line, menu_fg, menu_bg);
        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, cwd, menu_fg, menu_bg);

	/* display directory contents */
	row=0;
	while(row<num_items && row<MENU_HEIGHT) {
		if (row == (cursor_pos-first_visible)) {
			bg=menu_hl; /* C64_YELLOW; */
			selected=dir_items[row+first_visible].name;
		} else bg=menu_bg; /* C64_WHITE; */
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, bg);
		if (dir_items[row+first_visible].type == 0) {
        		draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), "<DIR> ", menu_fg, bg);
#ifdef __zipfile__
		} else if (dir_items[row+first_visible].type == 2) {
        		draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), "<ZIP> ", menu_fg, bg);
#endif
		}
		snprintf(tmp_string, 25, "%s", dir_items[row+first_visible].name);
        	draw_ascii_string(screen, display_width, MENU_X+(8*6), MENU_LS+(8*row), tmp_string, menu_fg, bg);
		row++;
	}
	while (row<MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	if (input_down) {
		input_down = 0;
		if (cursor_pos<(num_items-1)) cursor_pos++;
		if ((cursor_pos-first_visible)>=MENU_HEIGHT) first_visible++;
	} else if (input_up) {
		input_up = 0;
		if (cursor_pos>0) cursor_pos--;
		if (cursor_pos<first_visible) first_visible--;
	} else if (input_left) {
		input_left = 0;
		if (cursor_pos>=10) cursor_pos-=10;
		else cursor_pos=0;
		if (cursor_pos<first_visible) first_visible=cursor_pos;
	} else if (input_right) {
		input_right = 0;
		if (cursor_pos<(num_items-11)) cursor_pos+=10;
		else cursor_pos=num_items-1;
		if ((cursor_pos-first_visible)>=MENU_HEIGHT) 
			first_visible=cursor_pos-(MENU_HEIGHT-1);
	} else if (input_b) {
		input_b = 0;
		num_items = 0;
		int i;
		path = (char *)malloc(strlen(cwd)+strlen(dir_items[cursor_pos].name)+2);
		sprintf(path, "%s/%s", cwd, dir_items[cursor_pos].name);
		if (dir_items[cursor_pos].type == 0) {
			/* directory selected */
			pathlength=strlen(path);
			if (path[pathlength-1] == '.'
				/* check for . selected */
					&& path[pathlength-2] == '/') {
				path[pathlength-2]='\0';
			} else if (path[pathlength-1] == '.'
				/* check for .. selected */
					&& path[pathlength-2] == '.'
					&& path[pathlength-3] == '/') {
				for(i=4; i<pathlength && path[pathlength-i]!='/'; i++);
				if (i<pathlength
					&& path[(pathlength-i)+1]!='.'
					&& path[(pathlength-i)+2]!='.') 
					path[pathlength-i]='\0';
				if (strlen(path) == 0) {
					path[0]='.';
					path[1]='\0';
				}
				cwd=path;
			} else {
				cwd=path;
			}
#ifdef __zipfile__
		} else if (dir_items[cursor_pos].type == 2) {
			cwd=path;
#endif
		} else {
			/* file selected */
			return path;
		}
	} else if (input_x) {
		input_x = 0;
		return (char *)-1;
	} else if (input_y) {
		/* unzip file to tmp directory and cd to it */
		input_y=0;
		char *command=(char *)malloc(1024);
		sprintf(command, "./unzip -o -d ./tmp %s/%s", cwd, dir_items[cursor_pos].name);
		system(command);
		free(command);
		num_items=0;
		cwd="./tmp";
	}
	return NULL;
}

char *option_txt[255];

void draw_prefs (unsigned char *screen) {
	static int cursor_pos=0;
	unsigned char bg;
	char tmp_string[1024];
	char tmp_string2[1024];
	char tmp_string3[1024];
	static unsigned int keymapping = 0;
	static unsigned int getfilename = 0;
	static unsigned int gotfilename = 0;
	static int auto_start=0;
	static int manual_start=0;
	static int attach_unit=0;
	static int attach_cart=0;
	static int attach_cart_vic20_2000=0;
	static int attach_cart_vic20_4000=0;
	static int attach_cart_vic20_6000=0;
	static int attach_cart_vic20_a000=0;
	static int attach_cart_vic20_b000=0;
	static char *imagefile;
	int i;

	option_txt[AUTOSTART]=		"Autostart image...               ";
	option_txt[START]=		"Browse d64...                    ";
	option_txt[ATTACH8]=            "Attach unit8...                  ";
	option_txt[ATTACH9]=            "Attach unit9...                  ";
        option_txt[SOUND]=              "Sound                            ";
	option_txt[SIDENGINE]=		"SID engine                       ";
        option_txt[BRIGHTNESS]=         "Brightness                       ";
        option_txt[CONTRAST]=           "Contrast                         ";
        option_txt[FRAMESKIP]=          "Frameskip                        ";
        option_txt[WARP]=               "Warp mode                        ";
        option_txt[LIMITSPEED]=         "Limit speed                      ";
        option_txt[KEYMAP]=             "Keymapping                       ";
        option_txt[JOYSTICK]=           "GP2X Joystick in port            ";
        option_txt[USBJOYSTICK1]=       "USB Joystick 1 in port           ";
        option_txt[USBJOYSTICK2]=       "USB Joystick 2 in port           ";
        option_txt[TRUEDRIVE]=          "True drive emulation             ";
        option_txt[VDEVICES]=           "Virtual devices                  ";
        option_txt[D64]=                "Attach image/load prg/sna...     ";
        option_txt[LOADSTAR]=           "Load first program               ";
        option_txt[LOADER]=             "Load disk browser                ";
        option_txt[PRG]=                "Load .prg file...                ";
        option_txt[SAVE_SNAP]=          "Save snapshot                    ";
        option_txt[LOAD_SNAP]=          "Load snapshot...                 ";
        option_txt[RESET]=              "Reset machine                    ";
	option_txt[RCONTROL]=		"Reverse controls                 ";
	option_txt[SCALED]=		"Display scaling                  ";
	option_txt[CENTRED]=		"Display centred                  ";
	option_txt[CPUSPEED]=		"CPU speed                        ";
        option_txt[EXITVICE]=           "Exit VICE                        ";
	option_txt[BLANK1]=blank_line;
/*        option_txt[BLANK2]=blank_line;*/
        option_txt[BLANK3]=blank_line;
        option_txt[BLANK4]=blank_line;
        option_txt[NUM_OPTIONS]=blank_line;
        option_txt[NUM_OPTIONS+1]=blank_line;
        option_txt[NUM_OPTIONS+2]=blank_line;
        option_txt[NUM_OPTIONS+3]=blank_line;
        option_txt[NUM_OPTIONS+4]=      NULL;

	if (keymapping) {
		if (keymapping_menu (screen)) { keymapping = 0; } else {return;}
	}
	if (getfilename) {
		if (!gotfilename) imagefile=file_req(screen, NULL);
		if (imagefile == NULL) return;
		else if (imagefile == (char *)-1) getfilename=0;
		else {
			gotfilename = 1;
#ifdef __zipfile__
			fprintf(stderr, "opening zip file\n");
			if (strstr(imagefile, ".zip/") || strstr(imagefile, ".ZIP/")) {
				strcpy(tmp_string, imagefile);
				for(i=strlen(tmp_string); tmp_string[i]!='/'; i--);
				tmp_string[i]='\0'; /* tmp_string points now to zip archive */
				char *zip_member=tmp_string+i+1;
				int zip_error;
				struct zip *zip_archive=zip_open(tmp_string, 0, &zip_error);
				if (zip_archive) {
					struct zip_file *zip_content=zip_fopen(zip_archive, zip_member, 0);
					if (zip_content) {
						char *zipfile_buffer=(char *)malloc(174848);
						int filesize=zip_fread(zip_content, zipfile_buffer, 174848);

						snprintf(tmp_string2, 255, "%s/c64/tmp/vice.%s", 
							AppDirPath, (zip_member+(strlen(zip_member)-3)));
						if (!strcmp(prefs->DrivePath[0], tmp_string2)) {
							snprintf(tmp_string2, 255, "%s/c64/tmp/vice2.%s", 
								AppDirPath, (zip_member+(strlen(zip_member)-3)));
						}
						zip_fclose(zip_content);
						fprintf(stderr, "creating temporary file %s\n", tmp_string2);
						FILE *tmp_image=fopen(tmp_string2, "w");
						fwrite(zipfile_buffer, filesize, 1, tmp_image);
						fclose(tmp_image);
					} else {
						zip_error_to_str(zip_error_str, 255, errno, zip_error);
						fprintf(stderr, "error in zip_fopen: %s\n", zip_error_str);
					}
					zip_close(zip_archive);
				} else {
					zip_error_to_str(zip_error_str, 255, errno, zip_error);
					fprintf(stderr, "error in zip_open: %s\n", zip_error_str);
				}
				imagefile=tmp_string2;
			}
#endif
			if (auto_start) {
				autostart_autodetect(imagefile, NULL, 0, 0);
				auto_start=0;
				getfilename=0;
				gotfilename=0;
				prefs_open=0;
			} else if (manual_start) {
				char *image_prg=image_file_req(screen, imagefile);
				if (image_prg == NULL) return;
				else if (image_prg == (char *)-1) {
					imagefile=NULL;
					gotfilename=0;
				} else {
					autostart_autodetect(imagefile, image_prg, 0, 0);
					manual_start=0;
					getfilename=0;
					gotfilename=0;
					prefs_open=0;
				}
			} else if (attach_unit) {
				/* FIXME check imagefile allocation */
				file_system_attach_disk(attach_unit, imagefile);
				attach_unit=0;
				getfilename=0;
				gotfilename=0;
				prefs_open=0;
			} else if (attach_cart) {
				ui_attach_cart(imagefile,0);
				attach_cart=0;
				getfilename=0;
				gotfilename=0;
			} else if (attach_cart_vic20_2000) {
				ui_attach_cart(imagefile,2);
				attach_cart_vic20_2000=0;
				getfilename=0;
				gotfilename=0;
			} else if (attach_cart_vic20_4000) {
				ui_attach_cart(imagefile,4);
				attach_cart_vic20_4000=0;
				getfilename=0;
				gotfilename=0;
			} else if (attach_cart_vic20_6000) {
				ui_attach_cart(imagefile,6);
				attach_cart_vic20_6000=0;
				getfilename=0;
				gotfilename=0;
			} else if (attach_cart_vic20_a000) {
				ui_attach_cart(imagefile,0xa);
				attach_cart=0;
				getfilename=0;
				gotfilename=0;
			} else if (attach_cart_vic20_b000) {
				ui_attach_cart(imagefile,0xb);
				attach_cart_vic20_b000=0;
				getfilename=0;
				gotfilename=0;
			}
		}
	}

	if (input_down) {
		input_down=0;
		cursor_pos++;
		if (cursor_pos>=NUM_OPTIONS) cursor_pos=NUM_OPTIONS-1;
	} else if (input_up) {
		input_up=0;
		cursor_pos--;
		if (cursor_pos<0) cursor_pos=0;
	} else if (input_left) {
		input_left = 0;
		if (gp2x_usbjoys > 0) {
			if (cursor_pos == USBJOYSTICK1)	cur_portusb1 = 1; 
		} else if (gp2x_usbjoys > 1) {
			if (cursor_pos == USBJOYSTICK2) cur_portusb2 = 1; 
		}
		if (cursor_pos == JOYSTICK) {
			cur_port = 1;
		} else if (cursor_pos == FRAMESKIP) {
			int rrate;
			resources_get_int("RefreshRate", &rrate);
			if (rrate>0) {
				rrate--;
				resources_set_int("RefreshRate", rrate);
			}
		} else if (cursor_pos == WARP) {
			resources_set_int("WarpMode", 0);
		} else if (cursor_pos == TRUEDRIVE) {
			resources_set_int("DriveTrueEmulation", 0);
		} else if (cursor_pos == VDEVICES) {
			resources_set_int("VirtualDevices", 0);
		} else if (cursor_pos == SIDENGINE) {
			ui_handle_sidengine_resource(0);
		} else if (cursor_pos == SCALED) {
			hwscaling=0;
			display_set();
		} else if (cursor_pos == CENTRED) {
			xoffset=xoffset_uncentred; 
			yoffset=yoffset_uncentred;
			centred=0;
		} else if (cursor_pos == CPUSPEED) {
			if (cpu_speed > 150) {
				cpu_speed -= 5;
				set_FCLK(cpu_speed);
			}
		} else if (cursor_pos == X8) {
			if (ui_set_ramblocks(0) == 1) vic20_mem=0;
		}
	} else if (input_right) {
		input_right=0;
		if (gp2x_usbjoys > 0) {
			if (cursor_pos == USBJOYSTICK1) cur_portusb1 = 2; 
		} else if (gp2x_usbjoys > 1) {
			if (cursor_pos == USBJOYSTICK2) cur_portusb2 = 2; 
		}
		if (cursor_pos == JOYSTICK) {
			cur_port=2;
		} else if (cursor_pos == FRAMESKIP) {
			int rrate;
			resources_get_int("RefreshRate", &rrate);
			if (rrate<50) {
				rrate++;
				resources_set_int("RefreshRate", rrate);
			}
		} else if (cursor_pos == WARP) {
			resources_set_int("WarpMode", 1);
		} else if (cursor_pos == TRUEDRIVE) {
			resources_set_int("DriveTrueEmulation", 1);
		} else if (cursor_pos == VDEVICES) {
			resources_set_int("VirtualDevices", 1);
		} else if (cursor_pos == SIDENGINE) {
			ui_handle_sidengine_resource(1);
		} else if (cursor_pos == SCALED) {
			hwscaling=1;
			display_set();
		} else if (cursor_pos == CENTRED) {
			xoffset=xoffset_centred; 
			yoffset=yoffset_centred;
			centred=1;
		} else if (cursor_pos == CPUSPEED) {
			if (cpu_speed < 280) {
				cpu_speed += 5;
				set_FCLK(cpu_speed);
			}
		} else if (cursor_pos == X8) {
			if (ui_set_ramblocks(1) == 1) vic20_mem=5;
		}
	}

	if (input_b) {
		input_b = 0;
		if (cursor_pos == KEYMAP) {
			keymapping = 1;
		} else if (cursor_pos == X1) {
			if (ui_handle_X(1) == 1) {
				getfilename=1;
				attach_cart=1;
			}
		} else if (cursor_pos == X2) {
			if (ui_handle_X(2) == 1) {
				getfilename=1;
				attach_cart_vic20_2000=1;
			}
		} else if (cursor_pos == X3) {
			if (ui_handle_X(3) == 1) {
				getfilename=1;
				attach_cart_vic20_4000=1;
			}
		} else if (cursor_pos == X4) {
			if (ui_handle_X(4) == 1) {
				getfilename=1;
				attach_cart_vic20_6000=1;
			}
		} else if (cursor_pos == X5) {
			if (ui_handle_X(5) == 1) {
				getfilename=1;
				attach_cart_vic20_a000=1;
			}
		} else if (cursor_pos == X6) {
			if (ui_handle_X(6) == 1) {
				getfilename=1;
				attach_cart_vic20_b000=1;
			}
		} else if (cursor_pos == X7) {
			ui_handle_X(7);
		} else if (cursor_pos == AUTOSTART) {
			getfilename=1;
			auto_start=1;
		} else if (cursor_pos == START) {
			getfilename=1;
			manual_start=1;
		} else if (cursor_pos == ATTACH8) {
			getfilename=1;
			attach_unit=8;
		} else if (cursor_pos == ATTACH9) {
			getfilename=1;
			attach_unit=9;
		} else if (cursor_pos == RESET) {
			prefs_open=0;
			machine_trigger_reset(MACHINE_RESET_MODE_HARD);
		} else if (cursor_pos == SAVE_SNAP) {
			int freename=0;
			int snapnum;
			for(snapnum=0; !freename; snapnum++) {
				DIR *snaps_dir=opendir("./snapshots");
				struct dirent *direntry;
				freename=1;
				while((direntry=readdir(snaps_dir))) {
					sprintf(tmp_string, "%04d.sna", snapnum);
					if (!strcmp(tmp_string, direntry->d_name)) freename=0;
				}
				closedir(snaps_dir);
			}
			sprintf(tmp_string2, "%s/%s", "./snapshots", tmp_string);
			interrupt_maincpu_trigger_trap(write_snapshot, (void *)tmp_string2);
			sprintf(tmp_string3, "%s/screen%04d.bmp", "./snapshots", snapnum-1);
			screenshot_save("BMP", tmp_string3, current_canvas);
			prefs_open=0;
		} else if (cursor_pos == EXITVICE) {
			resources_save("vicerc");
			exit(0);
		}
	}

	for(i=0; i<NUM_OPTIONS; i++) {
		bg=menu_bg;
		if (i == cursor_pos) bg=menu_hl;
		draw_ascii_string(screen, display_width, MENU_X, MENU_Y+(i*8), option_txt[i], menu_fg, bg);
	}
	while(i<=MENU_HEIGHT) {
		draw_ascii_string(screen, display_width, MENU_X, MENU_Y+(i*8), option_txt[BLANK1], menu_bg, bg);
		i++;
	}

        /* joystick port */
        if (cur_port == 1) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*JOYSTICK), "1", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*JOYSTICK), "2", menu_fg, menu_bg);
	}

        /* usb joystick 1 port */
	if (gp2x_usbjoys > 0) {
        	if (cur_portusb1 == 1) {
	        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK1), "1", menu_fg, menu_bg);
		} else {
        		draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK1), "2", menu_fg, menu_bg);
		}

	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK1), "none", menu_fg, menu_bg);
	}

        /* usb joystick 2 port */
	if (gp2x_usbjoys > 1) {
	        if (cur_portusb2 == 1) {
        		draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK2), "1", menu_fg, menu_bg);
		} else {
        		draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK2), "2", menu_fg, menu_bg);
		}
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*USBJOYSTICK2), "none", menu_fg, menu_bg);
	}

	/* frameskip */
	int cur_rrate;
	resources_get_int("RefreshRate", &cur_rrate);
	if (cur_rrate == 0)  {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*FRAMESKIP), "auto", menu_fg, menu_bg);
	} else {
		snprintf(tmp_string, 255, "%d", cur_rrate-1);
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*FRAMESKIP), tmp_string, menu_fg, menu_bg);
	}

	/* warp mode */
	int warpmode;
	resources_get_int("WarpMode", &warpmode);
	if (warpmode) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*WARP), "On", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*WARP), "Off", menu_fg, menu_bg);
	}

	/* truedrive emulation */
	int truedrive;
	resources_get_int("DriveTrueEmulation", &truedrive);
	if (truedrive) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*TRUEDRIVE), "On", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*TRUEDRIVE), "Off", menu_fg, menu_bg);
	}

	/* virtual devices */
	int vdevices;
	resources_get_int("VirtualDevices", &vdevices);
	if (vdevices) 
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*VDEVICES), "On", menu_fg, menu_bg);
	else
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*VDEVICES), "Off", menu_fg, menu_bg);

	/* sid engine */
	ui_draw_resid_string(screen, MENU_X+(8*21), MENU_Y+(8*SIDENGINE));

	/* screen scaling */
	if (hwscaling) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*21), MENU_Y+(8*SCALED), "scaled", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*21), MENU_Y+(8*SCALED), "1:1", menu_fg, menu_bg);
	}

	/* centre display */
	if (centred) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*21), MENU_Y+(8*CENTRED), "on", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*21), MENU_Y+(8*CENTRED), "off", menu_fg, menu_bg);
	}

	ui_draw_memory_string(screen,MENU_X+(8*21), MENU_Y+(8*X8), vic20_mem);

	/* cpu speed */
	sprintf(tmp_string, "%dmhz", cpu_speed);
        draw_ascii_string(screen, display_width, MENU_X+(8*21), MENU_Y+(8*CPUSPEED), tmp_string, menu_fg, menu_bg);
}
