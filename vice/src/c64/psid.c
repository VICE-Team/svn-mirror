/*
 * psid.c - PSID file handling.
 *
 * Written by
 *  Dag Lem <resid@nimrod.no>
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "c64mem.h"
#include "cmdline.h"
#include "drive.h"
#include "interrupt.h"
#include "log.h"
#include "machine.h"
#include "psid.h"
#include "resources.h"
#include "types.h"
#include "ui.h"
#include "utils.h"
#include "vsidui.h"
#include "vsync.h"
#include "zfile.h"

typedef struct psid {
  WORD version;
  WORD data_offset;
  WORD load_addr;
  WORD init_addr;
  WORD play_addr;
  WORD songs;
  WORD start_song;
  DWORD speed;
  BYTE name[32];
  BYTE author[32];
  BYTE copyright[32];
  WORD flags;
  DWORD reserved;
  WORD data_size;
  BYTE data[65536];
} psid_t;


#define PSID_V1_DATA_OFFSET 0x76
#define PSID_V2_DATA_OFFSET 0x7c

int psid_ui_set_tune(resource_value_t tune);

static psid_t* psid = NULL;
static int psid_tune = 0;    /* app_resources.PSIDTune */

static resource_t resources[] = {
    { "PSIDTune", RES_INTEGER, (resource_value_t) 0,
      (resource_value_t *) &psid_tune, psid_ui_set_tune },
    { NULL }
};

int psid_init_resources(void)
{
    return resources_register(resources);
}

static int cmdline_psid_mode(const char *param, void *extra_param)
{
    psid_mode = 1;
    return 0;
}

static int cmdline_psid_tune(const char *param, void *extra_param)
{
  psid_tune = atoi(param);
  return 0;
}

static cmdline_option_t cmdline_options[] =
{
    { "-vsid", CALL_FUNCTION, 0, cmdline_psid_mode, NULL, NULL, NULL,
      NULL, "SID player mode" },
    { "-tune", CALL_FUNCTION, 1, cmdline_psid_tune, NULL, NULL, NULL,
      "<number>", "Specify PSID tune <number>" },
    { NULL }
};

int psid_init_cmdline_options(void)
{
    return cmdline_register_options(cmdline_options);
}


static WORD psid_extract_word(BYTE** buf)
{
  WORD word = (*buf)[0] << 8 | (*buf)[1];
  *buf += 2;
  return word;
}

int psid_load_file(const char* filename)
{
  FILE* f;
  BYTE buf[PSID_V2_DATA_OFFSET + 2];
  BYTE* ptr = buf;
  unsigned int length;

  if (!(f = zfopen(filename, "rb"))) {
    return -1;
  }

  free(psid);
  psid = xmalloc(sizeof(psid_t));

  if (fread(ptr, 1, 6, f) != 6 || memcmp(ptr, "PSID", 4) != 0) {
    goto fail;
  }

  ptr += 4;
  psid->version = psid_extract_word(&ptr);

  if (psid->version < 1 || psid->version > 2) {
    log_error(LOG_DEFAULT, "PSID: Unknown version number: %d.",
	      (int)psid->version);
    goto fail;
  }

  length = (unsigned int)((psid->version == 1 
           ? PSID_V1_DATA_OFFSET : PSID_V2_DATA_OFFSET) - 6);

  if (fread(ptr, 1, length, f) != length) {
    log_message(LOG_DEFAULT, "PSID: Error reading header.");
    goto fail;
  }

  psid->data_offset = psid_extract_word(&ptr);
  psid->load_addr = psid_extract_word(&ptr);
  psid->init_addr = psid_extract_word(&ptr);
  psid->play_addr = psid_extract_word(&ptr);
  psid->songs = psid_extract_word(&ptr);
  psid->start_song = psid_extract_word(&ptr);
  psid->speed = psid_extract_word(&ptr) << 16;
  psid->speed |= psid_extract_word(&ptr);
  memcpy(psid->name, ptr, 32);
  psid->name[31] = '\0';
  ptr += 32;
  memcpy(psid->author, ptr, 32);
  psid->author[31] = '\0';
  ptr += 32;
  memcpy(psid->copyright, ptr, 32);
  psid->copyright[31] = '\0';
  ptr += 32;
  if (psid->version == 2) {
    psid->flags = psid_extract_word(&ptr);
    psid->reserved = psid_extract_word(&ptr) << 16;
    psid->reserved |= psid_extract_word(&ptr);
  }
  else {
    psid->flags = 0;
    psid->reserved = 0;
  }

  /* Zero load address => the load address is stored in the
     first two bytes of the binary C64 data. */
  if (psid->load_addr == 0) {
    if (fread(ptr, 1, 2, f) != 2) {
      log_message(LOG_DEFAULT, "PSID: Error reading load address.");
      goto fail;
    }
    psid->load_addr = ptr[0] | ptr[1] << 8;
  }

  /* Zero init address => use load address. */
  if (psid->init_addr == 0) {
    psid->init_addr = psid->load_addr;
  }

  /* Read binary C64 data. */
  psid->data_size = fread(psid->data, 1, sizeof(psid->data), f);

  if (ferror(f)) {
    log_message(LOG_DEFAULT, "PSID: Error reading data.");
    goto fail;
  }
    
  if (!feof(f)) {
    log_message(LOG_DEFAULT, "PSID: More than 64K data.");
    goto fail;
  }
    
  zfclose(f);
  return 0;

fail:
  zfclose(f);
  free(psid);
  psid = NULL;
  return -1;
}


void psid_init_tune(void)
{
  BYTE volume = 0x0f;
  BYTE portval = 0x35;
  int start_song = psid_tune;
  int i;

  if (!psid) {
    return;
  }

  log_message(LOG_DEFAULT, "\n%s\n%s\n%s\n",
	      psid->name, psid->author, psid->copyright);

  /* PAL/NTSC. */
  resources_set_value("VideoStandard", (resource_value_t)
  		      (psid->speed == 0 ? DRIVE_SYNC_PAL : DRIVE_SYNC_NTSC));

  /* Check tune number. */
  if (start_song == 0) {
    start_song = psid->start_song;
  }
  else if (start_song < 1 || start_song > psid->songs) {
    log_message(LOG_DEFAULT,
		"Warning: Tune out of range.\n");
    start_song = psid->start_song;
  }

  log_message(LOG_DEFAULT, "Playing tune %i of %i (%s).\n",
	      start_song, (int)psid->songs, psid->speed == 0 ? "PAL" : "NTSC");

  /* Store parameters for psid player. */
  ram_store(0x0306, psid->init_addr & 0xff);
  ram_store(0x0307, psid->init_addr >> 8);
  ram_store(0x0308, psid->play_addr & 0xff);
  ram_store(0x0309, psid->play_addr >> 8);
  ram_store(0x030a, start_song - 1);
  ram_store(0x030b, volume);
  ram_store(0x030c, portval);

  /* Store binary C64 data. */
  for (i = 0; i < psid->data_size; i++) {
    ram_store(psid->load_addr + i, psid->data[i]);
  }
}

void psid_set_tune(int tune)
{
  if (tune == -1) {
    psid_tune = 0;
    free(psid);
    psid = NULL;
  }
  else {
    psid_tune = tune;
  }
}

int psid_ui_set_tune(resource_value_t tune)
{
  char buf[10];
  psid_tune = (int)tune == -1 ? 0 : (int)tune;
  sprintf(buf, "tune %d\n", (int)tune);
  vsid_set_tune(buf);
  return 0;
}

int psid_tunes(int* default_tune)
{
  *default_tune = psid ? psid->start_song : 0;
  return psid ? psid->songs : 0;
}

void psid_init_driver(void) {
  BYTE psid_driver[] = {
#include "psiddrv.h"
  };

  ADDRESS addr;
  int i;

  /* 6510 vectors stored in both ROM and RAM. */
  for (addr = 0xfffa, i = 0; i < 6; i++) {
    rom_store((ADDRESS)(addr + i), psid_driver[i]);
    ram_store((ADDRESS)(addr + i), psid_driver[i]);
  }

  /* Driver code. */
  for (addr = 0x0300, i = 0x14; i < sizeof(psid_driver); i++) {
    ram_store(addr + i, psid_driver[i]);
  }
}
