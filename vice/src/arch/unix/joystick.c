/*
 * joystick.c - Joystick support for Linux.
 *
 * Written by
 *  Bernhard Kuhn    (kuhn@eikon.e-technik.tu-muenchen.de)
 *  Ulmer Lionel     (ulmer@poly.polytechnique.fr)
 *
 * Patches by
 *  Daniel Sladic    (sladic@eecg.toronto.edu)
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

#ifdef HAS_JOYSTICK

#include <linux/joystick.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "types.h"
#include "kbd.h"
#include "resources.h"
#include "joystick.h"

int ajoyfd[2] = { -1, -1 };
int djoyfd[2] = { -1, -1 };

int hjoyport[2];

#define JOYCALLOOPS 100
#define JOYSENSITIVITY 5
int joyxcal[2];
int joyycal[2];
int joyxmin[2];
int joyxmax[2];
int joyymin[2];
int joyymax[2];

void joyport1select(int port) {
  hjoyport[0]=port;
};

void joyport2select(int port) {
  hjoyport[1]=port;
};

void joyset(void) {
  joyport1select(app_resources.joyDevice1);
  joyport2select(app_resources.joyDevice2);
}

void joyini(void)
{
  int i;

  joyset();
  
  /* close all device files */
  for(i=0;i<2;i++) {
    if(ajoyfd[i] != -1)
      close(ajoyfd[i]);
    if(djoyfd[i] != -1)
      close(djoyfd[i]);
  }
	 
  /* open analog device files */
  for(i=0;i<2;i++) {
    
    char* dev;
    dev=(i==0)?"/dev/js0":"/dev/js1";
    
    ajoyfd[i]=open(dev,O_RDONLY);
    if (ajoyfd[i] < 0) {
      fprintf(stderr, "Warning: couldn't open the joystick device %s!\n",dev);
    }
    else {
      int j;
      
      /* calibration loop */
      for(j=0;j<JOYCALLOOPS;j++) {
	struct JS_DATA_TYPE js;
	int status=read(ajoyfd[i],&js,JS_RETURN);
	
	if (status != JS_RETURN) {
	  fprintf(stderr, "Warning: error reading the joystick device%s!\n",
		  dev);
	}
	else {
	  /* determine average */
	  joyxcal[i]+=js.x;
	  joyycal[i]+=js.y;
	}
      }

      /* correct average */
      joyxcal[i]/=JOYCALLOOPS;
      joyycal[i]/=JOYCALLOOPS;
      
      /* determine treshoulds */
      joyxmin[i]=joyxcal[i]-joyxcal[i]/JOYSENSITIVITY;
      joyxmax[i]=joyxcal[i]+joyxcal[i]/JOYSENSITIVITY;
      joyymin[i]=joyycal[i]-joyycal[i]/JOYSENSITIVITY;
      joyymax[i]=joyycal[i]+joyycal[i]/JOYSENSITIVITY;

      printf("hardware joystick calibration %s:\n",dev);
      printf("X: min: %i , mid: %i , max: %i\n",joyxmin[i],joyxcal[i],
	     joyxmax[i]);
      printf("Y: min: %i , mid: %i , max: %i\n",joyymin[i],joyycal[i],
	     joyymax[i]);
    }
  }
    
  /* open device files for digital joystick */
  for(i=0;i<2;i++) {
    char* dev;
    dev=(i==0)?"/dev/djs0":"/dev/djs1";

    djoyfd[i]=open(dev,O_RDONLY);
    if (djoyfd[i] < 0) {
      fprintf(stderr, "Warning: couldn't open the joystick device %s!\n",dev);
    }
  }
}

void joyclose(void)
{
  if(ajoyfd[0]>0) close (ajoyfd[0]);
  if(ajoyfd[1]>0) close (ajoyfd[1]);
  if(djoyfd[0]>0) close (djoyfd[0]);
  if(djoyfd[1]>0) close (djoyfd[1]);
}

void joystick(void)
{
  int i;
  for(i=1;i<=2;i++) {
    int joyport=hjoyport[i-1];

#ifdef HAS_DIGITAL_JOYSTICK
    if(joyport==3 || joyport==4) {
      int status;
      struct DJS_DATA_TYPE djs;
      int djoyport=joyport-3;
      
      if(djoyfd[djoyport]>0) {
	status=read(djoyfd[djoyport],&djs,DJS_RETURN);
	if (status != DJS_RETURN) {
	  fprintf(stderr,
		  "Warning: error reading the digital joystick device!\n");
	}
	else {
	  joy[i]=(joy[i]&0xe0)|((~(djs.switches>>3))&0x1f);
	}
      }
    } else
#endif
    if (joyport==1 || joyport==2) {
      int status;
      struct JS_DATA_TYPE js;
      int ajoyport=joyport-1;
      
      if(ajoyfd[ajoyport]>0) {
	status=read(ajoyfd[ajoyport],&js,JS_RETURN);
	if (status != JS_RETURN) {
	  fprintf(stderr, "Warning: error reading the joystick device!\n");
	}
	else {
	  if (js.y<joyymin[ajoyport]) joy[i] |= 1;
	  else joy[i] &= ~1;
	  if (js.y>joyymax[ajoyport]) joy[i] |= 2;
	  else joy[i] &= ~2;
	  if (js.x<joyxmin[ajoyport]) joy[i] |= 4;
	  else joy[i] &= ~4;
	  if (js.x>joyxmax[ajoyport]) joy[i] |= 8;
	  else joy[i] &= ~8;
	  if(js.buttons) joy[i] |= 16;
	  else joy[i] &= ~16;
	}
      }
    }
  }      
}                      

#endif /* HAS_JOYSTICK */

