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

#include "archdep.h"
#include "c64mem.h"
#include "cmdline.h"
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


static log_t vlog = LOG_ERR;

typedef struct psid_s {
  /* PSID data */
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
  BYTE start_page;
  BYTE max_pages;
  WORD reserved;
  WORD data_size;
  BYTE data[65536];

  /* Non-PSID data */
  DWORD frames_played;
} psid_t;


#define PSID_V1_DATA_OFFSET 0x76
#define PSID_V2_DATA_OFFSET 0x7c

int psid_ui_set_tune(resource_value_t tune, void *param);

static psid_t* psid = NULL;
static int psid_tune = 0;
static int keepenv = 0;

int set_keepenv(resource_value_t val, void *param)
{
  keepenv = (int)val;

  return 0;
}

static const resource_t resources[] = {
    { "PSIDKeepEnv", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&keepenv, set_keepenv, NULL },
    { "PSIDTune", RES_INTEGER, (resource_value_t)0,
      (resource_value_t *)&psid_tune, psid_ui_set_tune, NULL },
    { NULL }
};

int psid_init_resources(void)
{
    return resources_register(resources);
}

static int cmdline_vsid_mode(const char *param, void *extra_param)
{
    vsid_mode = 1;
    return 0;
}

static int cmdline_keepenv(const char *param, void *extra_param)
{
    keepenv = 1;
    return 0;
}

static int cmdline_psid_tune(const char *param, void *extra_param)
{
  psid_tune = atoi(param);
  return 0;
}

static const cmdline_option_t cmdline_options[] =
{
    /* The Video Standard options are copied from the machine files. */
    { "-pal", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (resource_value_t)MACHINE_SYNC_PAL,
      NULL, "Use PAL sync factor" },
    { "-ntsc", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (resource_value_t)MACHINE_SYNC_NTSC,
      NULL, "Use NTSC sync factor" },
    { "-ntscold", SET_RESOURCE, 0, NULL, NULL, "MachineVideoStandard",
      (resource_value_t)MACHINE_SYNC_NTSCOLD,
      NULL, "Use old NTSC sync factor" },
    { "-vsid", CALL_FUNCTION, 0, cmdline_vsid_mode, NULL, NULL, NULL,
      NULL, "SID player mode" },
    { "-keepenv", CALL_FUNCTION, 0, cmdline_keepenv, NULL, NULL, NULL,
      NULL, "Override PSID settings for Video standard and SID model" },
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

  if (vlog==LOG_ERR)
      vlog = log_open("Vsid");

  if (!(f = zfopen(filename, MODE_READ))) {
    return -1;
  }

  free(psid);
  psid = xmalloc(sizeof(psid_t));

  if (fread(ptr, 1, 6, f) != 6
      || (memcmp(ptr, "PSID", 4) != 0 && memcmp(ptr, "RSID", 4) != 0))
  {
    goto fail;
  }

  ptr += 4;
  psid->version = psid_extract_word(&ptr);

  if (psid->version < 1 || psid->version > 2) {
      log_error(vlog, "Unknown PSID version number: %d.",
                (int)psid->version);
      goto fail;
  }

  length = (unsigned int)((psid->version == 1
           ? PSID_V1_DATA_OFFSET : PSID_V2_DATA_OFFSET) - 6);

  if (fread(ptr, 1, length, f) != length) {
    log_error(vlog, "Reading PSID header.");
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
  psid->frames_played = 0;
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
    psid->start_page = *ptr++;
    psid->max_pages = *ptr++;
    psid->reserved = psid_extract_word(&ptr);
  }
  else {
    psid->flags = 0;
    psid->start_page = 0;
    psid->max_pages = 0;
    psid->reserved = 0;
  }

  /* Check for SIDPLAYER MUS files. */
  if (psid->flags & 0x01) {
    log_error(vlog, "SIDPLAYER MUS files not supported.");
    goto fail;
  }

  /* Zero load address => the load address is stored in the
     first two bytes of the binary C64 data. */
  if (psid->load_addr == 0) {
    if (fread(ptr, 1, 2, f) != 2) {
      log_error(vlog, "Reading PSID load address.");
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
    log_error(vlog, "Reading PSID data.");
    goto fail;
  }

  if (!feof(f)) {
    log_error(vlog, "More than 64K PSID data.");
    goto fail;
  }

  /* Relocation setup. */
  if (psid->start_page == 0x00) {
    /* Start and end pages. */
    int startp = psid->load_addr >> 8;
    int endp = (psid->load_addr + psid->data_size - 1) >> 8;

    /* Used memory ranges. */
    int used[] = { 0x00, 0x03,
		   0xa0, 0xbf,
		   0xd0, 0xff,
		   0x00, 0x00 };	/* calculated below */
    int pages[256];
    int last_page = 0;
    int i, page, tmp;

    /* finish initialization */
    used[6] = startp; used[7] = endp;
    
    /* Mark used pages in table. */
    memset(pages, 0, sizeof(pages));
    for (i = 0; i < sizeof(used)/sizeof(*used); i += 2) {
      for (page = used[i]; page <= used[i + 1]; page++) {
	pages[page] = 1;
      }
    }

    /* Find largest free range. */
    psid->max_pages = 0x00;
    for (page = 0; page < sizeof(pages)/sizeof(*pages); page++) {
      if (!pages[page]) continue;
      tmp = page - last_page;
      if (tmp > psid->max_pages) {
	psid->start_page = last_page;
	psid->max_pages = tmp;
      }
      last_page = page + 1;
    }

    if (psid->max_pages == 0x00) {
      psid->start_page = 0xff;
    }
  }

  if (psid->start_page == 0xff || psid->max_pages < 2)
  {
    log_error(vlog, "No space for driver.");
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


/* Use CBM80 vector to start PSID driver. This is a simple method to
   transfer control to the PSID driver while running in a pure C64
   environment. */
int psid_set_cbm80(WORD vec, WORD addr)
{
  int i;
  BYTE cbm80[] = { 0x00, 0x00, 0x00, 0x00, 0xc3, 0xc2, 0xcd, 0x38, 0x30 };

  cbm80[0] = vec & 0xff;
  cbm80[1] = vec >> 8;

  for (i = 0; i < sizeof(cbm80); i++) {
    ram_store((WORD)(addr + i), ram_read((WORD)(0x8000 + i)));
    ram_store((WORD)(0x8000 + i), cbm80[i]);
  }

  return i;
}


void psid_init_tune(void)
{
  int start_song = psid_tune;
  resource_value_t sync, sid_model;
  int i;
  WORD reloc_addr;
  WORD addr;
  int speedbit;
  char* irq;
  char irq_str[20];

  if (!psid) {
    return;
  }

  psid->frames_played = 0;

  reloc_addr = psid->start_page << 8;

  log_message(vlog, "driver=$%04X, image=$%04X-$%04X, init=$%04X, play=$%04X",
	      reloc_addr,
	      psid->load_addr, psid->load_addr + psid->data_size - 1,
	      psid->init_addr, psid->play_addr);

  /* PAL/NTSC. */
  resources_get_value("MachineVideoStandard", &sync);

  /* MOS6581/MOS8580. */
  resources_get_value("SidModel", &sid_model);

  /* Check tune number. */
  if (start_song == 0) {
    start_song = psid->start_song;
  }
  else if (start_song < 1 || start_song > psid->songs) {
    log_warning(vlog, "Tune out of range.");
    start_song = psid->start_song;
  }

  /* Check for PlaySID specific file. */
  if (psid->flags & 0x02) {
    log_warning(vlog, "Image is PlaySID specific - trying anyway.");
  }

  /* Check tune speed. */
  speedbit = 1;
  for (i = 1; i < start_song && i < 32; i++) {
    speedbit <<= 1;
  }

  irq = psid->speed & speedbit ? "CIA 1" : "VICII";

  if (psid->play_addr) {
    strcpy(irq_str, irq);
  }
  else {
    sprintf(irq_str, "custom (%s ?)", irq);
  }

  if (console_mode) {
      log_message(vlog, "Name: %s",      (char *)(psid->name));
      log_message(vlog, "Author: %s",    (char *)(psid->author));
      log_message(vlog, "Copyright: %s", (char *)(psid->copyright));
      log_message(vlog, "Using %s sync",
                  (int)sync == MACHINE_SYNC_PAL ? "PAL" : "NTSC");
      log_message(vlog, "Using %s emulation",
                  sid_model ? "MOS8580" : "MOS6581");
      log_message(vlog, "Using %s interrupt", irq_str);
      log_message(vlog, "Playing tune %d out of %d (default=%d)",
                  start_song, psid->songs, psid->start_song);
  }
  else {
    vsid_ui_display_name((char *)(psid->name));
    vsid_ui_display_author((char *)(psid->author));
    vsid_ui_display_copyright((char *)(psid->copyright));

    vsid_ui_display_sync((int)sync);
    vsid_ui_display_sid_model((int)sid_model);
    vsid_ui_display_irqtype(irq_str);
    vsid_ui_display_tune_nr(start_song);
    vsid_ui_set_default_tune(psid->start_song);
    vsid_ui_display_nr_of_tunes(psid->songs);
    vsid_ui_display_time(0);
  } 

  /* Store parameters for PSID player. */

  /* Skip JMP. */
  addr = reloc_addr + 3;

  /* CBM80 reset vector. */
  addr += psid_set_cbm80(reloc_addr, addr);

  ram_store(addr, (BYTE)(start_song));
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

int psid_ui_set_tune(resource_value_t tune, void *param)
{
  psid_tune = (int)tune == -1 ? 0 : (int)tune;

  psid_set_tune(psid_tune);
  vsync_suspend_speed_eval();
  maincpu_trigger_reset();

  return 0;
}

int psid_tunes(int* default_tune)
{
  *default_tune = psid ? psid->start_song : 0;
  return psid ? psid->songs : 0;
}


void psid_init_driver(void)
{
  BYTE psid_driver[] = {
#include "psiddrv.h"
  };
  BYTE *psid_reloc = psid_driver;
  int psid_size;

  WORD reloc_addr;
  WORD addr;
  int i;
  resource_value_t sync;

  if (!psid) {
    return;
  }

  /* C64 PAL/NTSC flag. */
  resources_get_value("MachineVideoStandard", &sync);
  if (!keepenv) {
    switch ((psid->flags >> 2) & 0x03) {
    case 0x01:
      sync = (resource_value_t)MACHINE_SYNC_PAL;
      resources_set_value("MachineVideoStandard", sync);
      break;
    case 0x02:
      sync = (resource_value_t)MACHINE_SYNC_NTSC;
      resources_set_value("MachineVideoStandard", sync);
      break;
    default:
      /* Keep settings (00 = unknown, 11 = any) */
      break;
    }
  }

  /* MOS6581/MOS8580 flag. */
  if (!keepenv) {
    switch ((psid->flags >> 4) & 0x03) {
    case 0x01:
      resources_set_value("SidModel", (resource_value_t)0);
      break;
    case 0x02:
      resources_set_value("SidModel", (resource_value_t)1);
      break;
    default:
      /* Keep settings (00 = unknown, 11 = any) */
      break;
    }
  }

  /* Clear low memory to minimize the damage of PSIDs doing bad reads. */
  for (addr = 0; addr < 0x0800; addr++) {
    ram_store(addr, (BYTE)0x00);
  }

  /* Relocation of C64 PSID driver code. */
  reloc_addr = psid->start_page << 8;
  psid_size = sizeof(psid_driver);

  if (!reloc65((char **)&psid_reloc, &psid_size, reloc_addr)) {
    log_error(vlog, "Relocation.");
    psid_set_tune(-1);
    return;
  }

  for (i = 0; i < psid_size; i++) {
    ram_store((WORD)(reloc_addr + i), psid_reloc[i]);
  }

  /* Store binary C64 data. */
  for (i = 0; i < psid->data_size; i++) {
    ram_store((WORD)(psid->load_addr + i), psid->data[i]);
  }

  /* Skip JMP and CBM80 reset vector. */
  addr = reloc_addr + 3 + 9;

  /* Store parameters for PSID player. */
  ram_store(addr++, (BYTE)(0));
  ram_store(addr++, (BYTE)(psid->songs));
  ram_store(addr++, (BYTE)(psid->load_addr & 0xff));
  ram_store(addr++, (BYTE)(psid->load_addr >> 8));
  ram_store(addr++, (BYTE)(psid->init_addr & 0xff));
  ram_store(addr++, (BYTE)(psid->init_addr >> 8));
  ram_store(addr++, (BYTE)(psid->play_addr & 0xff));
  ram_store(addr++, (BYTE)(psid->play_addr >> 8));
  ram_store(addr++, (BYTE)(psid->speed & 0xff));
  ram_store(addr++, (BYTE)((psid->speed >> 8) & 0xff));
  ram_store(addr++, (BYTE)((psid->speed >> 16) & 0xff));
  ram_store(addr++, (BYTE)(psid->speed >> 24));
  ram_store(addr++, (BYTE)((int)sync == MACHINE_SYNC_PAL ? 1 : 0));
}


unsigned int psid_increment_frames(void)
{
  if (!psid)
    return 0;

  (psid->frames_played)++;

  return (unsigned int)(psid->frames_played);
}
