/*
 * prefs_gp2x.c
 *
 * Written by
 *  Mike Dawson <mike@gp2x.org>
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
#include "minimal.h"
#include "machine.h"
#include "ui_gp2x.h"
#include "input_gp2x.h"
#include "prefs_gp2x.h"
#include "videoarch.h"
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "attach.h"
#include "autostart.h"
#include "resources.h"
#include "imagecontents.h"
#include "cartridge.h"
#include "screenshot.h"
#include "uiarch.h"

extern unsigned short *gp2x_memregs;

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
#if 0
	printf ("Old value = %04X\r",MEM_REG[0x924>>1]," ");
	printf ("APLL = %04X\r",MEM_REG[0x91A>>1]," ");
#endif
	mdiv=((mdiv-8)<<8) & 0xff00;
	pdiv=((pdiv-2)<<2) & 0xfc;
	scale&=3;
	v=mdiv | pdiv | scale;
	gp2x_memregs[0x910>>1]=v;
}

#define MENU_HEIGHT 25
#define MENU_X 32
#define MENU_Y 16
#define MENU_LS MENU_Y+8

char *blank_line="                                 ";

void display_set() {
	if(hwscaling) {
		display_width=384;
		gp2x_memregs[0x290c>>1]=display_width; /* screen width */
		if(tvout) {
			gp2x_memregs[0x2906>>1]=614; /* scale horizontal */
			if(tvout_pal) gp2x_memregs[0x2908>>1]=384; /* scale vertical PAL */
			else gp2x_memregs[0x2908>>1]=460; /* scale vertical NTSC */
#if 0
			gp2x_memregs[0x2908>>1]=430; /* scale vertical */
#endif
		} else {
			gp2x_memregs[0x2906>>1]=1228; /* scale horizontal */
			gp2x_memregs[0x2908>>1]=430; /* vertical */
		}
	} else {
		display_width=320;
		gp2x_memregs[0x290c>>1]=display_width; /* screen width */
		if(tvout) {
			gp2x_memregs[0x2906>>1]=420; /* scale horizontal */
			gp2x_memregs[0x2908>>1]=320; /* scale vertical */
		} else {
			gp2x_memregs[0x2906>>1]=1024; /* scale horizontal */
			gp2x_memregs[0x2908>>1]=320; /* vertical */
		}
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
	snprintf(tmp_string, 41, " %3.0f%%, %2.0ffps        drive8  %2.1f ", 
		emu_speed, emu_fps, track);
	draw_ascii_string(screen, display_width, 0, 232, tmp_string, menu_fg, menu_bg);
	if(drive8_status) drive8_colour=C64_RED;
	else drive8_colour=C64_WHITE;
	draw_ascii_string(screen, display_width, 26*8, 232, "*", drive8_colour, menu_bg);
}

struct dir_item {
	char *name;
	int type; /* 0=dir, 1=file, 2=zip archive */
};

void sort_dir(struct dir_item *list, int num_items, int sepdir) {
	int i;
	struct dir_item temp;

	for(i=0; i<(num_items-1); i++) {
		if(strcmp(list[i].name, list[i+1].name)>0) {
			temp=list[i];
			list[i]=list[i+1];
			list[i+1]=temp;
			i=0;
		}
	}
	if(sepdir) {
		for(i=0; i<(num_items-1); i++) {
			if((list[i].type!=0)&&(list[i+1].type==0)) {
				temp=list[i];
				list[i]=list[i+1];
				list[i+1]=temp;
				i=0;
			}
		}
	}
}

char *image_file_req(unsigned char *screen, const char *image) {
	static int num_items=0;
	static int cursor_pos;
	static int first_visible;
	int bg;
	char *selected;
	char *contents_str;
	static char *contents_list[255];
	int row;
	static image_contents_t *contents;
	image_contents_file_list_t *prg_list;
#if 0
	char tmp_string[255];
#endif
	int i;

#if 0
	contents_list[0]="test1";
	contents_list[1]="test2";
	contents_list[2]="test3";
	contents_list[3]=NULL;
	num_items=3;
	cursor_pos=0;
	first_visible=0;
#endif

	if(num_items==0) {
		contents_str=image_contents_read_string(IMAGE_CONTENTS_AUTO,
				image, 8, IMAGE_CONTENTS_STRING_PETSCII);
		if(contents_str==NULL) return (char*)-1;
		contents_list[0]=contents_str;
		for(i=0; contents_str[i]; i++) {
			if(contents_str[i]=='\n') {
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

#if 0
	if(num_items==0) {
		contents=image_contents_read(IMAGE_CONTENTS_AUTO, image, 8);
		if(contents==NULL) return (char *)-1;
		prg_list=contents->file_list;
		for(num_items=0; prg_list!=NULL; num_items++) {
			prg_list=prg_list->next;
		}
		cursor_pos=0;
		first_visible=0;
	}

        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, blank_line, menu_fg, menu_bg);
        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, 
		(char *)contents->name, menu_fg, menu_bg);

	prg_list=contents->file_list;
	for(i=0; i<first_visible; i++) prg_list=prg_list->next;
#endif

	row=0;
	while(row<(num_items-1) && row<MENU_HEIGHT) {
		if(row==(cursor_pos-first_visible)) {
			bg=menu_hl;
#if 0
			selected=(char *)prg_list->name;
#endif
			selected=contents_list[row+first_visible+1];
		} else bg=menu_bg;
#if 0
		if(dir_items[row+first_visible].type==0) {
        		draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), "<DIR> ", menu_fg, bg);
		}
		snprintf(tmp_string, 16, "%s", dir_items[row+first_visible].name);
        	draw_ascii_string(screen, display_width, MENU_X+(8*6), MENU_LS+(8*row), tmp_string, menu_fg, bg);
		strncpy(tmp_string, prg_list->name, 255);
#endif
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), 
			blank_line, menu_fg, bg);
        	draw_petscii_string(screen, display_width, MENU_X, MENU_LS+(8*row), 
			contents_list[row+first_visible+1], menu_fg, bg);
#if 0
			(char *)prg_list->name, menu_fg, bg);
		prg_list=prg_list->next;
#endif
		row++;
	}
	while(row<MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	if(input_down) {
		input_down=0;
		if(cursor_pos<(num_items-1)) cursor_pos++;
		if((cursor_pos-first_visible)>=MENU_HEIGHT) first_visible++;
	} else if(input_up) {
		input_up=0;
		if(cursor_pos>0) cursor_pos--;
		if(cursor_pos<first_visible) first_visible--;
	} else if(input_left) {
		input_left=0;
		if(cursor_pos>=10) cursor_pos-=10;
		else cursor_pos=0;
		if(cursor_pos<first_visible) first_visible=cursor_pos;
	} else if(input_right) {
		input_right=0;
		if(cursor_pos<(num_items-11)) cursor_pos+=10;
		else cursor_pos=num_items-1;
		if((cursor_pos-first_visible)>=MENU_HEIGHT) 
			first_visible=cursor_pos-(MENU_HEIGHT-1);
	} else if(input_b) {
		input_b=0;
		num_items=0;
		selected+=7;
		for(i=0; selected[i]!='"' && i<32; i++);
		selected[i]='\0';
		return selected;
	} else if(input_x) {
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

	if(dir!=NULL) cwd=dir;
	if(cwd==NULL) cwd=".";

	if(num_items==0) {
#ifdef __zipfile__
		if(!strcmp(cwd+(strlen(cwd)-4), ".zip")
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
			if(dirstream==NULL) {
				printf("error opening directory %s\n", cwd);
				return (char *)-1;
			}
			/* read directory entries */
			while(direntry=readdir(dirstream)) {
				dir_items[num_items].name=(char *)malloc(strlen(direntry->d_name)+1);
				strcpy(dir_items[num_items].name, direntry->d_name);
				num_items++;
				if(num_items>1024) break;
			}
			closedir(dirstream);
			/* get entry types */
			for(i=0; i<num_items; i++) {
				path=(char *)malloc(strlen(cwd)+strlen(dir_items[i].name)+2);
				sprintf(path, "%s/%s", cwd, dir_items[i].name);
				if(!stat(path, &item)) {
					if(S_ISDIR(item.st_mode)) {
						dir_items[i].type=0;
#ifdef __zipfile__
					} else if(!strcmp(path+(strlen(path)-4), ".zip")
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
		if(strcmp(cwd, last_dir)) {
			cursor_pos=0;
			first_visible=0;
			last_dir=(char *)malloc(strlen(cwd)+1);
			strcpy(last_dir, cwd);
		}
	}

        draw_ascii_string(screen, display_width, MENU_X, MENU_Y-8, 
		"B=load/browse, Y=unzip", menu_fg, menu_bg);
	/* display current directory */
        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, blank_line, menu_fg, menu_bg);
        draw_ascii_string(screen, display_width, MENU_X, MENU_Y, cwd, menu_fg, menu_bg);

	/* display directory contents */
	row=0;
	while(row<num_items && row<MENU_HEIGHT) {
		if(row==(cursor_pos-first_visible)) {
			bg=menu_hl; /* C64_YELLOW; */
			selected=dir_items[row+first_visible].name;
		} else bg=menu_bg; /* C64_WHITE; */
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, bg);
		if(dir_items[row+first_visible].type==0) {
        		draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), "<DIR> ", menu_fg, bg);
#ifdef __zipfile__
		} else if(dir_items[row+first_visible].type==2) {
        		draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), "<ZIP> ", menu_fg, bg);
#endif
		}
		snprintf(tmp_string, 25, "%s", dir_items[row+first_visible].name);
        	draw_ascii_string(screen, display_width, MENU_X+(8*6), MENU_LS+(8*row), tmp_string, menu_fg, bg);
		row++;
	}
	while(row<MENU_HEIGHT) {
        	draw_ascii_string(screen, display_width, MENU_X, MENU_LS+(8*row), blank_line, menu_fg, menu_bg);
		row++;
	}

	if(input_down) {
		input_down=0;
		if(cursor_pos<(num_items-1)) cursor_pos++;
		if((cursor_pos-first_visible)>=MENU_HEIGHT) first_visible++;
	} else if(input_up) {
		input_up=0;
		if(cursor_pos>0) cursor_pos--;
		if(cursor_pos<first_visible) first_visible--;
	} else if(input_left) {
		input_left=0;
		if(cursor_pos>=10) cursor_pos-=10;
		else cursor_pos=0;
		if(cursor_pos<first_visible) first_visible=cursor_pos;
	} else if(input_right) {
		input_right=0;
		if(cursor_pos<(num_items-11)) cursor_pos+=10;
		else cursor_pos=num_items-1;
		if((cursor_pos-first_visible)>=MENU_HEIGHT) 
			first_visible=cursor_pos-(MENU_HEIGHT-1);
	} else if(input_b) {
		input_b=0;
		num_items=0;
		int i;
		path=(char *)malloc(strlen(cwd)
			+strlen(dir_items[cursor_pos].name)
			+2);
		sprintf(path, "%s/%s", cwd, dir_items[cursor_pos].name);
#if 0
		/* FIXME */
		for(i=0; i<num_items; i++) free(dir_items[i].name);
#endif
		if(dir_items[cursor_pos].type==0) {
			/* directory selected */
			pathlength=strlen(path);
			if(path[pathlength-1]=='.'
				/* check for . selected */
					&& path[pathlength-2]=='/') {
				path[pathlength-2]='\0';
#if 0
				return path;
#endif
			} else if(path[pathlength-1]=='.'
				/* check for .. selected */
					&& path[pathlength-2]=='.'
					&& path[pathlength-3]=='/') {
#if 0
					&&pathlength>4) {
#endif
				for(i=4; i<pathlength && path[pathlength-i]!='/'; i++);
				if(i<pathlength
					&& path[(pathlength-i)+1]!='.'
					&& path[(pathlength-i)+2]!='.') 
					path[pathlength-i]='\0';
				if(strlen(path)==0) {
					path[0]='.';
					path[1]='\0';
				}
#if 0
				if(strlen(path)<=5) {
					path[5]='/';
					path[6]='\0';
				}
#endif
				cwd=path;
			} else {
				cwd=path;
			}
#ifdef _zipfile__
		} else if(dir_items[cursor_pos].type==2) {
			cwd=path;
#endif
		} else {
			/* file selected */
			return path;
		}
#if 0
	} else if(input_select) {
#endif
	} else if(input_x) {
		input_x=0;
		return (char *)-1;
	} else if(input_y) {
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

void draw_prefs(unsigned char *screen) {
	static int cursor_pos=0;
	unsigned char bg, fg;
	char tmp_string[1024];
	char tmp_string2[1024];
	char tmp_string3[1024];
	static int getfilename=0;
	static int gotfilename=0;
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
#if 0
	static int xfiletype;
#endif
	int update_prefs=0;
	int i,j;
	char zip_error_str[255];

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
        option_txt[JOYSTICK]=           "Joystick in port                 ";
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
	option_txt[TVOUT]=		"TV out mode                      ";
	option_txt[CENTRED]=		"Display centred                  ";
	option_txt[CPUSPEED]=		"CPU speed                        ";
        option_txt[EXITVICE]=           "Exit VICE                        ";
	option_txt[BLANK1]=blank_line;
        option_txt[BLANK2]=blank_line;
        option_txt[BLANK3]=blank_line;
        option_txt[BLANK4]=blank_line;
        option_txt[NUM_OPTIONS]=blank_line;
        option_txt[NUM_OPTIONS+1]=blank_line;
        option_txt[NUM_OPTIONS+2]=blank_line;
        option_txt[NUM_OPTIONS+3]=blank_line;
        option_txt[NUM_OPTIONS+4]=      NULL;

	if(getfilename) {
		if(!gotfilename) imagefile=file_req(screen, NULL);
		if(imagefile==NULL) return;
		else if(imagefile==(char *)-1) getfilename=0;
		else {
			gotfilename=1;
#ifdef __zipfile__
			fprintf(stderr, "opening zip file\n");
			if(strstr(imagefile, ".zip/") || strstr(imagefile, ".ZIP/")) {
				strcpy(tmp_string, imagefile);
				for(i=strlen(tmp_string); tmp_string[i]!='/'; i--);
				tmp_string[i]='\0'; /* tmp_string points now to zip archive */
				char *zip_member=tmp_string+i+1;
				int zip_error;
				struct zip *zip_archive=zip_open(tmp_string, 0, &zip_error);
				if(zip_archive) {
					struct zip_file *zip_content=zip_fopen(zip_archive, zip_member, 0);
					if(zip_content) {
						char *zipfile_buffer=(char *)malloc(174848);
						int filesize=zip_fread(zip_content, zipfile_buffer, 174848);

						snprintf(tmp_string2, 255, "%s/c64/tmp/vice.%s", 
							AppDirPath, (zip_member+(strlen(zip_member)-3)));
						if(!strcmp(prefs->DrivePath[0], tmp_string2)) {
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
#if 0
			if(!strcmp(imagefile+(strlen(imagefile)-4), ".sna")
				|| !strcmp(imagefile+(strlen(imagefile)-4), ".SNA")) {
				TheC64->LoadSnapshot(imagefile);
				TheC64->Resume();
				sdl_prefs_open=false;
			} else if(!strcmp(imagefile+(strlen(imagefile)-4), ".prg")
				|| !strcmp(imagefile+(strlen(imagefile)-4), ".PRG")) {
				char loadprg[255];
				for(i=strlen(imagefile); i>0; i--) {
					if(imagefile[i]=='/') break;
				}
				strcpy(loadprg, "\rLOAD\"");
				for(j=0; imagefile[i+1+j]; j++) {
					unsigned char c;
					c=imagefile[i+1+j];
					if ((c >= 'A') && (c <= 'Z') || (c >= 'a') && (c <= 'z')) c ^= 0x20;
					tmp_string[j]=c;
				}
				tmp_string[j]='\0';
				strcat(loadprg, tmp_string);
				strcat(loadprg, "\",8,1\rRUN\r");
				imagefile[i]='\0';
				strcpy(prefs->DrivePath[0], imagefile);
				kbd_buf_feed(loadprg);
				update_prefs=1;
				TheC64->Resume();
				sdl_prefs_open=false;
			} else {
				strcpy(prefs->DrivePath[0], imagefile);
				update_prefs=1;
				cursor_pos=LOADSTAR;
			}
#endif
			if(auto_start) {
				autostart_autodetect(imagefile, NULL, 0, 0);
				auto_start=0;
				getfilename=0;
				gotfilename=0;
				prefs_open=0;
			} else if(manual_start) {
				char *image_prg=image_file_req(screen, imagefile);
				if(image_prg==NULL) return;
				else if(image_prg==(char *)-1) {
					imagefile=NULL;
					gotfilename=0;
				} else {
#if 0
					fprintf(stderr, "calling autostart %s, %s\n",
						imagefile, image_prg);
#endif
					autostart_autodetect(imagefile, image_prg, 0, 0);
					manual_start=0;
					getfilename=0;
					gotfilename=0;
					prefs_open=0;
				}
			} else if(attach_unit) {
				/* FIXME check imagefile allocation */
				file_system_attach_disk(attach_unit, imagefile);
				attach_unit=0;
				getfilename=0;
				gotfilename=0;
				prefs_open=0;
			} else if(attach_cart) {
				ui_attach_cart(imagefile,0);
				attach_cart=0;
				getfilename=0;
				gotfilename=0;
			} else if(attach_cart_vic20_2000) {
				ui_attach_cart(imagefile,2);
				attach_cart_vic20_2000=0;
				getfilename=0;
				gotfilename=0;
			} else if(attach_cart_vic20_4000) {
				ui_attach_cart(imagefile,4);
				attach_cart_vic20_4000=0;
				getfilename=0;
				gotfilename=0;
			} else if(attach_cart_vic20_6000) {
				ui_attach_cart(imagefile,6);
				attach_cart_vic20_6000=0;
				getfilename=0;
				gotfilename=0;
			} else if(attach_cart_vic20_a000) {
				ui_attach_cart(imagefile,0xa);
				attach_cart=0;
				getfilename=0;
				gotfilename=0;
			} else if(attach_cart_vic20_b000) {
				ui_attach_cart(imagefile,0xb);
				attach_cart_vic20_b000=0;
				getfilename=0;
				gotfilename=0;
			}
		}
	}

	if(input_down) {
		input_down=0;
		cursor_pos++;
		if(cursor_pos>=NUM_OPTIONS) cursor_pos=NUM_OPTIONS-1;
	} else if(input_up) {
		input_up=0;
		cursor_pos--;
		if(cursor_pos<0) cursor_pos=0;
	} else if(input_left) {
		input_left=0;
		if(cursor_pos==JOYSTICK) {
			cur_port=1;
		} else if(cursor_pos==FRAMESKIP) {
			int rrate;
			resources_get_int("RefreshRate", &rrate);
			if(rrate>0) {
				rrate--;
				resources_set_int("RefreshRate", rrate);
			}
		} else if(cursor_pos==WARP) {
			resources_set_int("WarpMode", 0);
		} else if(cursor_pos==TRUEDRIVE) {
			resources_set_int("DriveTrueEmulation", 0);
		} else if(cursor_pos==VDEVICES) {
			resources_set_int("VirtualDevices", 0);
		} else if(cursor_pos==SIDENGINE) {
			ui_handle_sidengine_resource(0);
		} else if(cursor_pos==SCALED) {
#if 0
			gp2x_memregs[0x2906>>1]=1024; /* scale horizontal */
			gp2x_memregs[0x2908>>1]=320; /* vertical */
			display_width=320;
			gp2x_memregs[0x290c>>1]=display_width; /* screen width */
#endif
			hwscaling=0;
			display_set();
#if 0
		} else if(cursor_pos==TVOUT) {
			gp2x_memregs[0x2800>>1]&=~0x300;
			tvout=0;
			tvout_pal=0;
			display_set();
#endif
		} else if(cursor_pos==CENTRED) {
			xoffset=xoffset_uncentred; 
			yoffset=yoffset_uncentred;
			centred=0;
		} else if(cursor_pos==CPUSPEED) {
			if(cpu_speed==250) {
				cpu_speed=200;
				set_FCLK(cpu_speed);
			} else if(cpu_speed==200) {
				cpu_speed=166;
				set_FCLK(cpu_speed);
			}
		} else if(cursor_pos==X8) {
			if (ui_set_ramblocks(0)==1) vic20_mem=0;
		}
	} else if(input_right) {
		input_right=0;
		if(cursor_pos==JOYSTICK) {
			cur_port=2;
		} else if(cursor_pos==FRAMESKIP) {
			int rrate;
			resources_get_int("RefreshRate", &rrate);
			if(rrate<50) {
				rrate++;
				resources_set_int("RefreshRate", rrate);
			}
		} else if(cursor_pos==WARP) {
			resources_set_int("WarpMode", 1);
		} else if(cursor_pos==TRUEDRIVE) {
			resources_set_int("DriveTrueEmulation", 1);
		} else if(cursor_pos==VDEVICES) {
			resources_set_int("VirtualDevices", 1);
		} else if(cursor_pos==SIDENGINE) {
			ui_handle_sidengine_resource(1);
		} else if(cursor_pos==SCALED) {
#if 0
			gp2x_memregs[0x2906>>1]=1228; /* scale horizontal */
			gp2x_memregs[0x2908>>1]=430; /* scale vertical */
			gp2x_memregs[0x2908>>1]=384; /* scale vertical */
			display_width=384;
			gp2x_memregs[0x290c>>1]=display_width; /* screen width */
#endif
			hwscaling=1;
			display_set();
#if 0
		} else if(cursor_pos==TVOUT) {
			gp2x_memregs[0x2800>>1]|=0x300;
			hwscaling=1;
			tvout=1;
			tvout_pal=1;
			display_set();
#endif
		} else if(cursor_pos==CENTRED) {
			xoffset=xoffset_centred; 
			yoffset=yoffset_centred;
			centred=1;
		} else if(cursor_pos==CPUSPEED) {
			if(cpu_speed==166) {
				cpu_speed=200;
				set_FCLK(cpu_speed);
			} else if(cpu_speed==200) {
				cpu_speed=250;
				set_FCLK(cpu_speed);
			}
		} else if(cursor_pos==X8) {
			if (ui_set_ramblocks(1)==1) vic20_mem=5;
		}
#if 0
		if(cursor_pos==LIMITSPEED) {
			prefs->LimitSpeed=true;
			update_prefs=1;
		} else if(cursor_pos==RESET) {
			prefs->FastReset=true;
			update_prefs=1;
		} else if(cursor_pos==SOUND) {
			prefs->SIDType=SIDTYPE_DIGITAL;
			update_prefs=1;
		} else if(cursor_pos==RCONTROL) {
			control_reversed=1;
		} else if(cursor_pos==BRIGHTNESS) {
			set_luma(brightness+1, contrast);
		} else if(cursor_pos==CONTRAST) {
			set_luma(brightness, contrast+1);
		}
#endif
	}

	if(input_b) {
		input_b=0;
		if(cursor_pos==X1) {
			if (ui_handle_X(1)==1) {
				getfilename=1;
				attach_cart=1;
			}
		} else if(cursor_pos==X2) {
			if (ui_handle_X(2)==1) {
				getfilename=1;
				attach_cart_vic20_2000=1;
			}
		} else if(cursor_pos==X3) {
			if (ui_handle_X(3)==1) {
				getfilename=1;
				attach_cart_vic20_4000=1;
			}
		} else if(cursor_pos==X4) {
			if (ui_handle_X(4)==1) {
				getfilename=1;
				attach_cart_vic20_6000=1;
			}
		} else if(cursor_pos==X5) {
			if (ui_handle_X(5)==1) {
				getfilename=1;
				attach_cart_vic20_a000=1;
			}
		} else if(cursor_pos==X6) {
			if (ui_handle_X(6)==1) {
				getfilename=1;
				attach_cart_vic20_b000=1;
			}
		} else if(cursor_pos==X7) {
			ui_handle_X(7);
		} else if(cursor_pos==AUTOSTART) {
			getfilename=1;
			auto_start=1;
		} else if(cursor_pos==START) {
			getfilename=1;
			manual_start=1;
		} else if(cursor_pos==ATTACH8) {
			getfilename=1;
			attach_unit=8;
		} else if(cursor_pos==ATTACH9) {
			getfilename=1;
			attach_unit=9;
		} else if(cursor_pos==RESET) {
			prefs_open=0;
			machine_trigger_reset(MACHINE_RESET_MODE_HARD);
		} else if(cursor_pos==SAVE_SNAP) {
			int freename=0;
			int snapnum;
			for(snapnum=0; !freename; snapnum++) {
				DIR *snaps_dir=opendir("./snapshots");
				struct dirent *direntry;
				freename=1;
				while(direntry=readdir(snaps_dir)) {
					sprintf(tmp_string, "%04d.sna", snapnum);
					if(!strcmp(tmp_string, direntry->d_name)) freename=0;
				}
				closedir(snaps_dir);
			}
#if 0
			char *fname="test.sna";
			interrupt_maincpu_trigger_trap(write_snapshot, (void *)fname);
#endif
			sprintf(tmp_string2, "%s/%s", "./snapshots", tmp_string);
			interrupt_maincpu_trigger_trap(write_snapshot, (void *)tmp_string2);
			sprintf(tmp_string3, "%s/screen%04d.bmp", "./snapshots", snapnum-1);
			screenshot_save("BMP", tmp_string3, current_canvas);
			prefs_open=0;
		} else if(cursor_pos==EXITVICE) {
			resources_save("vicerc");
			exit(0);
		}
#if 0
		if(cursor_pos==D64) {
			xfiletype=0;
			getfilename=true;
		} else if(cursor_pos==LOADSTAR) {
			kbd_buf_feed("\rLOAD\":*\",8,1\rRUN\r");
			TheC64->Resume();
			sdl_prefs_open=false;
		} else if(cursor_pos==LOADER) {
			load_prg(TheC64, c64loader, sizeof(c64loader));
			kbd_buf_feed("\rLOAD\"X\",8,1\rRUN\r");
			TheC64->Resume();
			sdl_prefs_open=false;
		} else if(cursor_pos==PRG) {
			xfiletype=1;
			getfilename=true;
		} else if(cursor_pos==LOAD_SNAP) {
			TheC64->LoadSnapshot("test.sna");
			TheC64->Resume();
			sdl_prefs_open=false;
		} else if(cursor_pos==SAVE_SNAP) {
#ifdef __gp2x__
			chdir(AppDirPath);
			chdir("c64/snapshots/");
#else
			chdir("/tmp/c64/snapshots/");
#endif
			int freename=0;
			for(int snapnum=0; !freename; snapnum++) {
				DIR *snaps_dir=opendir(".");
				struct dirent *direntry;
				freename=1;
				while(direntry=readdir(snaps_dir)) {
					sprintf(tmp_string, "%04d.sna", snapnum);
					if(!strcmp(tmp_string, direntry->d_name)) freename=0;
				}
				closedir(snaps_dir);
			}
			TheC64->SaveSnapshot(tmp_string);


			/* save thumbnail */
			if(thumb_current) {
				sprintf(tmp_string2, "thumbs/%s.thm", tmp_string);
				FILE *thumb_file=fopen(tmp_string2, "w");
				fwrite(thumb_current, (DISPLAY_X/4)*(DISPLAY_Y/4), 1, thumb_file);
				fclose(thumb_file);
			}

			TheC64->Resume();
			sdl_prefs_open=false;
		}
#endif
	}

#if 0
	for(i=0; option_txt[i]; i++) {
#endif
	for(i=0; i<NUM_OPTIONS; i++) {
		bg=menu_bg;
		if(i==cursor_pos) bg=menu_hl;
		draw_ascii_string(screen, display_width, MENU_X, MENU_Y+(i*8), option_txt[i], menu_fg, bg);
	}
	while(i<=MENU_HEIGHT) {
		draw_ascii_string(screen, display_width, MENU_X, MENU_Y+(i*8), option_txt[BLANK1], menu_bg, bg);
		i++;
	}

        /* joystick port */
        if(cur_port==1) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*JOYSTICK), "1", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*JOYSTICK), "2", menu_fg, menu_bg);
	}
	/* frameskip */
	int cur_rrate;
	resources_get_int("RefreshRate", &cur_rrate);
	if(cur_rrate==0)  {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*FRAMESKIP), 
			"auto", menu_fg, menu_bg);
	} else {
		snprintf(tmp_string, 255, "%d", cur_rrate-1);
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*FRAMESKIP), 
			tmp_string, menu_fg, menu_bg);
	}

	/* warp mode */
	int warpmode;
	resources_get_int("WarpMode", &warpmode);
	if(warpmode) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*WARP), 
			"On", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*WARP), 
			"Off", menu_fg, menu_bg);
	}

	/* truedrive emulation */
	int truedrive;
	resources_get_int("DriveTrueEmulation", &truedrive);
	if(truedrive) {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*TRUEDRIVE), 
			"On", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), MENU_Y+(8*TRUEDRIVE), 
			"Off", menu_fg, menu_bg);
	}

	/* virtual devices */
	int vdevices;
	resources_get_int("VirtualDevices", &vdevices);
	if(vdevices) 
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), 
			MENU_Y+(8*VDEVICES), "On", menu_fg, menu_bg);
	else
        	draw_ascii_string(screen, display_width, MENU_X+(8*24), 
			MENU_Y+(8*VDEVICES), "Off", menu_fg, menu_bg);

	/* sid engine */
	ui_draw_resid_string(screen, MENU_X+(8*21), MENU_Y+(8*SIDENGINE));

	/* screen scaling */
	if(hwscaling) {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*SCALED), 
			"scaled", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*SCALED), 
			"1:1", menu_fg, menu_bg);
	}

	/* centre display */
	if(centred) {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*CENTRED), 
			"on", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*CENTRED), 
			"off", menu_fg, menu_bg);
	}

#if 0
	/* tv out */
	if(tvout) {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*25), MENU_Y+(8*TVOUT), 
			"(on)", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*25), MENU_Y+(8*TVOUT), 
			"(off)", menu_fg, menu_bg);
	}
	if(tvout_pal) {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*TVOUT), 
			"PAL", menu_fg, menu_bg);
	} else {
        	draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*TVOUT), 
			"NTSC", menu_fg, menu_bg);
	}
#endif

	ui_draw_memory_string(screen,MENU_X+(8*21), MENU_Y+(8*X8), vic20_mem);

	/* cpu speed */
	sprintf(tmp_string, "%dmhz", cpu_speed);
        draw_ascii_string(screen, display_width, 
			MENU_X+(8*21), MENU_Y+(8*CPUSPEED),
			tmp_string, menu_fg, menu_bg);

#if 0
	sprintf(tmp_string, "%d", brightness);
        draw_ascii_string(screen_base, 80+(8*23), 25+(8*BRIGHTNESS), tmp_string, black, fill_gray);
	sprintf(tmp_string, "%d", contrast);
        draw_ascii_string(screen_base, 80+(8*23), 25+(8*CONTRAST), tmp_string, black, fill_gray);
#endif

#if 0
        /* reset speed */
        if(prefs->FastReset) {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*RESET), "Fast", black, fill_gray);
	} else {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*RESET), "Slow", black, fill_gray);
	}
        /* limit speed */
        if(prefs->LimitSpeed) {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*LIMITSPEED), "On", black, fill_gray);
	} else {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*LIMITSPEED), "Off", black, fill_gray);
	}
	/* sound */
        if(prefs->SIDType==SIDTYPE_NONE) {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*SOUND), "Off", black, fill_gray);
	} else {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*SOUND), "On", black, fill_gray);
	}
	/* sound */
	/* control */
	if(control_reversed) {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*RCONTROL), "On", black, fill_gray);
	} else {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*RCONTROL), "Off", black, fill_gray);
	}
	if(scaled) {
        	draw_ascii_string(screen_base, 80+(8*20), 25+(8*SCALED), "Scaled", black, fill_gray);
	} else {
        	draw_ascii_string(screen_base, 80+(8*23), 25+(8*SCALED), "1:1", black, fill_gray);
	}
#endif

#if 0
	/* attached media */
	snprintf(tmp_string, 22, "%s", prefs->DrivePath[0]);
        draw_ascii_string(screen_base, 80, 25+(8*BLANK3), "drive 8 media: ", black, fill_gray);
        draw_ascii_string(screen_base, 80, 25+(8*BLANK4), tmp_string, black, fill_gray);
#endif
}
