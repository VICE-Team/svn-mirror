/*
 * mmc64.c - Cartridge handling, MMC64 cart.
 *
 * Written by
 *  Markus Stehr <bastetfurry@ircnet.de>
 *  Marco van den Heuvel <blackystardust68@yahoo.com>
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
#include "c64_256k.h"
#include "c64cart.h"
#include "c64export.h"
#include "c64io.h"
#include "c64mem.h"
#include "cmdline.h"
#include "lib.h"
#include "log.h"
#include "machine.h"
#include "mem.h"
#include "mmc64.h"
#include "plus256k.h"
#include "plus60k.h"
#include "resources.h"
#include "translate.h"
#include "types.h"
#include "util.h"
#include "vicii-phi1.h"

static const c64export_resource_t export_res = {
    "MMC64", 1, 0
};

/* MMC64 enable */
int mmc64_enabled;

/* MMC64 clockport enable */
int mmc64_clockport_enabled=1;

/* MMC64 clockport base address */
int mmc64_hw_clockport=0xde02;

/* MMC64 bios writable */
static int mmc64_bios_write;

/* Bios file name */
static char *mmc64_bios_filename = NULL;

/* Image file name */
static char *mmc64_image_filename = NULL;

/* Image file */
static FILE *mmc64_image_file;

/* Pointer inside image */
static unsigned int mmc64_image_pointer;

/* write sequence counter */
static int mmc64_write_sequence;

/* Command buffer */
static unsigned char mmc64_cmd_puffer[9];
static unsigned int mmc64_cmd_puffer_pointer;

/* $DF11 bit 7 unlock flag */
static int mmc64_bit7_unlocked=0;

/* Unlocking sequences buffer */
static BYTE mmc64_unlocking[2]={0,0};

/* BIOS changed flag */
static int mmc64_bios_changed=0;

/* flash jumper flag */
static int mmc64_hw_flashjumper;

/* write protect flag */
static int mmc64_hw_writeprotect;

/* Control Bits */
/* $DF11 (R/W): MMC64 control register */
#define MMC_ACTIVE  0x80	/* bit 7: 0 = MMC64 is active, 1 = MMC64 is completely disabled                   */
#define MMC_SPIMODE 0x40	/* bit 6: 0 = SPI write trigger mode, 1 = SPI read trigger mode                   */
#define MMC_EXTROM  0x20	/* bit 5: 0 = allow external rom when BIOS is disabled , 1 = disable external ROM */
#define MMC_FLASH   0x10	/* bit 4: 0 = normal Operation, 1 = flash mode                                    */
#define MMC_CPORT   0x08	/* bit 3: 0 = clock port @ $DE00, 1 = clock port @ $DF20                          */
#define MMC_SPEED   0x04	/* bit 2: 0 = 250khz transfer, 1 = 8mhz transfer                                  */
#define MMC_CARDSEL 0x02	/* bit 1: 0 = card selected, 1 = card not selected                                */
#define MMC_BIOSSEL 0x01	/* bit 0: 0 = MMC64 BIOS active, 1 = external ROM active                          */

/* Variables of the various control bits */
static BYTE mmc64_active;
static BYTE mmc64_spi_mode;
static BYTE mmc64_extrom;
static BYTE mmc64_flashmode;
static BYTE mmc64_cport;
static BYTE mmc64_speed;
static BYTE mmc64_cardsel;
static BYTE mmc64_biossel;

/* Status Bits */
/* $DF12 (R): MMC64 status register */
#define MMC_FLASHJMP	0x20 /* bit 5: 0 = flash jumper not set, 1 = flash jumper set  */
#define MMC_WRITEPROT	0x10 /* bit 4: 0 = card write enabled, 1 = card write disabled */
#define MMC_CARDPRS	0x08 /* bit 3: 0 = card inserted, 1 = no card inserted         */
#define MMC_EXTEXROM	0x04 /* bit 2: external EXROM line                             */
#define MMC_EXTGAME	0x02 /* bit 1: external GAME line                              */
#define MMC_SPISTAT	0x01 /* bit 0: 0 = SPI ready, 1 = SPI busy                     
*/

/* Variables of the various status bits */
static BYTE mmc64_flashjumper;
static BYTE mmc64_writeprotect;
static BYTE mmc64_cardpresent;
static BYTE mmc64_extexrom;
static BYTE mmc64_extgame;
static BYTE mmc64_spistatus;

#define MMC_CARD_IDLE          0
#define MMC_CARD_RESET         1
#define MMC_CARD_INIT          2
#define MMC_CARD_READ          3
#define MMC_CARD_DUMMY_READ    4
#define MMC_CARD_WRITE         5
#define MMC_CARD_DUMMY_WRITE   6
#define MMC_CARD_RETURN_WRITE  7

static BYTE mmc64_card_state;
static BYTE mmc64_card_reset_count;
static int mmc64_revision;
static BYTE mmc64_image_file_readonly=0;

/* Gets set when dummy byte is read */
static unsigned int mmc64_read_firstbyte;

static unsigned int mmc64_block_size;

static log_t mmc64_log = LOG_ERR;

static BYTE mmc64_bios[0x2002];

static int mmc64_bios_offset=0;

static int mmc64_activate(void);
static int mmc64_deactivate(void);

static void mmc64_clear_cmd_puffer(void)
{
  int i;

  for (i = 0; i < 9; i++)
  {
    mmc64_cmd_puffer[i] = 0;
  }
  mmc64_cmd_puffer_pointer = 0;
}

/* Resets the card */
static void mmc64_reset_card(void)
{
  mmc64_active=0;
  mmc64_spi_mode=0;
  mmc64_extrom=0;
  mmc64_flashmode=0;
  mmc64_cport=0;
  mmc64_speed=0;
  mmc64_cardsel=0;
  mmc64_biossel=0;

  mmc64_extexrom=0x04;
  mmc64_extgame=0x02;
  mmc64_spistatus=0;
  mmc64_clockport_enabled=1;

  mmc64_card_reset_count=0;
  mmc64_image_pointer = 0;
  mmc64_block_size=512;
  mmc64_clear_cmd_puffer();
  if (mmc64_enabled)
  {
    export.exrom = 1;
    mem_pla_config_changed();
  }
}

void mmc64_reset(void)
{
  mmc64_active=0;
  mmc64_spi_mode=0;
  mmc64_extrom=0;
  mmc64_flashmode=0;
  mmc64_cport=0;
  mmc64_speed=0;
  mmc64_cardsel=0;
  mmc64_biossel=0;

  mmc64_extexrom=0x04;
  mmc64_extgame=0x02;
  mmc64_spistatus=0;
  mmc64_clockport_enabled=1;

  mmc64_card_reset_count=0;
  mmc64_image_pointer = 0;
  mmc64_block_size=512;
  mmc64_clear_cmd_puffer();
  if (mmc64_enabled)
  {
    export.exrom = 1;
    mem_pla_config_changed();
  }
}

static int set_mmc64_enabled(int val, void *param)
{
  if (!val)
  {
    if (mmc64_enabled)
    {
      if (mmc64_deactivate() < 0)
      {
        return -1;
      }
      machine_trigger_reset(MACHINE_RESET_MODE_HARD);
    }
    c64export_remove(&export_res);
    mmc64_enabled = 0;
    export.exrom = 0;
    mem_pla_config_changed();
    return 0;
  }
  else
  {
    if (c64export_query(&export_res) >= 0)
    {
      if (!mmc64_enabled)
      {
        if (mmc64_activate() < 0)
        {
          return -1;
        }
        machine_trigger_reset(MACHINE_RESET_MODE_HARD);
      }

      if (c64export_add(&export_res) < 0)
        return -1;

      mmc64_enabled = 1;
      export.exrom = 1;
      mem_pla_config_changed();
      return 0;
    }
    else
    {
      return -1;
    }
  }
}

static int set_mmc64_readonly(int val, void *param)
{
  if (!mmc64_image_file_readonly)
  {
    mmc64_hw_writeprotect = val;
    mmc64_writeprotect = val*MMC_WRITEPROT;
    return 0;
  }
  else
  {
    mmc64_hw_writeprotect=1;
    mmc64_writeprotect=1;
  }
  return -1;
}

static int set_mmc64_flashjumper(int val, void *param)
{
  mmc64_hw_flashjumper = val;
  mmc64_flashjumper = val*MMC_FLASHJMP;
  return 0;
}

static int set_mmc64_revision(int val, void *param)
{
  mmc64_revision = val;
  return 0;
}

static int set_mmc64_bios_write(int val, void *param)
{
  mmc64_bios_write = val;
  return 0;
}

static int set_mmc64_bios_filename(const char *name, void *param)
{
  if (mmc64_bios_filename != NULL && name != NULL && strcmp(name, mmc64_bios_filename) == 0)
    return 0;

  if (name != NULL && *name != '\0') {
      if (util_check_filename_access(name) < 0)
          return -1;
  }

  if (mmc64_enabled)
  {
    mmc64_deactivate();
    util_string_set(&mmc64_bios_filename, name);
    mmc64_activate();
    machine_trigger_reset(MACHINE_RESET_MODE_HARD);
  }
  else
  {
    util_string_set(&mmc64_bios_filename, name);
  }

  return 0;
}

static int set_mmc64_image_filename(const char *name, void *param)
{
  if (mmc64_image_filename != NULL && name != NULL && strcmp(name, mmc64_image_filename) == 0)
    return 0;

  if (name != NULL && *name != '\0') {
      if (util_check_filename_access(name) < 0)
          return -1;
  }

  if (mmc64_enabled)
  {
    mmc64_deactivate();
    util_string_set(&mmc64_image_filename, name);
    mmc64_activate();
  }
  else
  {
    util_string_set(&mmc64_image_filename, name);
  }

  return 0;
}

void mmc64_init_card_config(void)
{
  mmc64_active=0;
  mmc64_spi_mode=0;
  mmc64_extrom=0;
  mmc64_flashmode=0;
  mmc64_cport=0;
  mmc64_speed=0;
  mmc64_cardsel=0;
  mmc64_biossel=0;

  mmc64_extexrom=0x04;  /* for now external exrom and game are constantly   *
  mmc64_extgame=0x02;    * high until the pass-through port support is made */
  mmc64_spistatus=0;

  if (mmc64_enabled)
  {
    export.exrom = 1;
    mem_pla_config_changed();
  }
}

/* Executes a command */
static void mmc64_execute_cmd(void)
{
  unsigned int mmc_current_address_pointer;
  log_message(mmc64_log,"Executing CMD %02x %02x %02x %02x %02x %02x %02x %02x %02x",mmc64_cmd_puffer[0]
                         ,mmc64_cmd_puffer[1],mmc64_cmd_puffer[2],mmc64_cmd_puffer[3],mmc64_cmd_puffer[4]
                         ,mmc64_cmd_puffer[5],mmc64_cmd_puffer[6],mmc64_cmd_puffer[7],mmc64_cmd_puffer[8]);
  switch(mmc64_cmd_puffer[1])
  {
    case 0xff:
      log_message(mmc64_log,"Hard reset received");
	mmc64_card_state = MMC_CARD_IDLE;
      break;
    case 0x40:    /* CMD00 Reset */
      log_message(mmc64_log,"CMD Reset received");
      mmc64_reset_card();
      mmc64_card_state = MMC_CARD_RESET;
      break;
    case 0x41:    /* CMD01 Init */
      log_message(mmc64_log,"CMD Init received");
      mmc64_card_state = MMC_CARD_INIT;
      break;
    case 0x4c:    /* CMD12 Stop */
      log_message(mmc64_log,"CMD Stop received");
      mmc64_card_state = MMC_CARD_IDLE;
      break;
    case 0x50:
      log_message(mmc64_log,"CMD Set Block Size received");
      mmc64_card_state = MMC_CARD_IDLE;
      mmc64_block_size = mmc64_cmd_puffer[5] + (mmc64_cmd_puffer[4]*0x100) + (mmc64_cmd_puffer[3]*0x10000) + (mmc64_cmd_puffer[2]*0x1000000);
      break;
    case 0x51:
      log_message(mmc64_log,"CMD Block Read received");
      if (!mmc64_cardpresent)
      {
        mmc64_card_state = MMC_CARD_READ;
        mmc64_read_firstbyte = 0;
        mmc_current_address_pointer = mmc64_cmd_puffer[5] + (mmc64_cmd_puffer[4]*0x100) + (mmc64_cmd_puffer[3]*0x10000) +
                                     (mmc64_cmd_puffer[2]*0x1000000);
        log_message(mmc64_log,"Address: %08x",mmc_current_address_pointer);
        if (fseek(mmc64_image_file, mmc_current_address_pointer, SEEK_SET)!=0)
        {
          mmc64_card_state = MMC_CARD_DUMMY_READ;
        }
      }
      else
      {
        mmc64_card_state = MMC_CARD_DUMMY_READ;
        mmc64_read_firstbyte = 0;
      }
      break;
    case 0x58:
      log_message(mmc64_log,"CMD Block Write received");
      if (!mmc64_cardpresent && mmc64_block_size>0)
      {
        mmc64_write_sequence=0;
        mmc64_card_state = MMC_CARD_WRITE;
        mmc_current_address_pointer = mmc64_cmd_puffer[5] + (mmc64_cmd_puffer[4]*0x100) + (mmc64_cmd_puffer[3]*0x10000) +
                                     (mmc64_cmd_puffer[2]*0x1000000);
        log_message(mmc64_log,"Address: %08x",mmc_current_address_pointer);
        if (fseek(mmc64_image_file, mmc_current_address_pointer, SEEK_SET)!=0 || mmc64_writeprotect)
        {
          mmc64_card_state = MMC_CARD_DUMMY_WRITE;
        }
      }
      else
      {
        mmc64_write_sequence=0;
        mmc64_card_state = MMC_CARD_DUMMY_WRITE;
      }
      break;
  }
}

static void mmc64_write_to_cmd_puffer(unsigned char mmc64_cmd_char)
{
  /* Check for 0xff sync byte */
  if (mmc64_cmd_puffer_pointer == 0)
  {
    if (mmc64_cmd_char < 0xff)
    {
      return;
    }
  }

  /* Check for 0xff sync byte too much */
  if (mmc64_cmd_puffer_pointer == 1)
  {
    if (mmc64_cmd_char == 0xff)
    {
      mmc64_cmd_puffer_pointer = 0;
      return;
    }
  }

  /* Write byte to buffer */
  mmc64_cmd_puffer[mmc64_cmd_puffer_pointer] = mmc64_cmd_char;
  mmc64_cmd_puffer_pointer++;

  /* If the buffer is full, execute the buffer and clear it */
  if (mmc64_cmd_puffer_pointer > 9 || (mmc64_cmd_puffer_pointer > 8 && mmc64_cmd_puffer[1]==0x50))
  {
    mmc64_execute_cmd();
    mmc64_clear_cmd_puffer();
  }
}

static void mmc64_write_to_mmc(BYTE value)
{
  switch (mmc64_write_sequence)
  {
    case 0:
      if (value==0xfe)
      {
        mmc64_write_sequence++;
        mmc64_image_pointer=0;
      }
      break;
    case 1:
      if (mmc64_card_state==MMC_CARD_WRITE)
        fwrite(&value,1,1,mmc64_image_file);
      mmc64_image_pointer++;
      if (mmc64_image_pointer==mmc64_block_size)
        mmc64_write_sequence++;
      break;
    case 2:
      mmc64_write_sequence++;
      break;
    case 3:
      mmc64_card_state = MMC_CARD_RETURN_WRITE;
      break;
  }
}

void mmc64_clockport_enable_store(BYTE value)
{
  if (value&1)
  {
    mmc64_clockport_enabled=1;
  }
  else
  {
    mmc64_clockport_enabled=0;
  }
}

void REGPARM2 mmc64_io2_store(WORD addr, BYTE value)
{
  switch(addr)
  {
    case 0xdf10:    /* MMC64 SPI transfer register */
      if (mmc64_active==0)    /* if the MMC64 has been disabled in software this input is ignored */
      {
        if (mmc64_card_state==MMC_CARD_WRITE || mmc64_card_state==MMC_CARD_DUMMY_WRITE)
        {
          mmc64_write_to_mmc(value);
        }
        else
        {
          mmc64_write_to_cmd_puffer(value);
        }
      }
      break;
    case 0xdf11:    /* MMC64 control register */
      if (mmc64_active==0)    /* if the MMC64 has been disabled in software this input is ignored */
      {
        if (mmc64_bit7_unlocked==1 && mmc64_active==0)    /* this bit can only be changed after unlocking it */
        {
          if (value & MMC_ACTIVE)
          {
            log_message(mmc64_log,"disabling MMC64");
            mmc64_active = MMC_ACTIVE;
            export.exrom = 0;
            mem_pla_config_changed();
          }
        }
        mmc64_bit7_unlocked=0;
        if (value & MMC_SPIMODE)
        {
          mmc64_spi_mode = MMC_SPIMODE;
        }
        else
        {
          mmc64_spi_mode = 0;
        }
        if (value & MMC_EXTROM)
        {
          mmc64_extrom = MMC_EXTROM;
        }
        else
        {
          mmc64_extrom = 0;
        }
        if (mmc64_flashjumper)    /* this bit can only be changed if the flashjumper is on */
        {
          if (value & MMC_FLASH)
          {
            mmc64_flashmode = MMC_FLASH;
          }
          else
          {
            mmc64_flashmode = 0;
          }
        }
        if (value & MMC_CPORT)
        {
          mmc64_cport = MMC_CPORT;
          mmc64_hw_clockport = 0xdf22;
        }
        else
        {
          mmc64_cport = 0;
          mmc64_hw_clockport = 0xde02;
        }
        if (value & MMC_SPEED)
        {
          mmc64_speed = MMC_SPEED;
        }
        else
        {
          mmc64_speed = 0;
        }
        if (value & MMC_CARDSEL)
        {
          mmc64_cardsel = MMC_CARDSEL;
        }
        else
        {
          mmc64_cardsel = 0;
        }
        if (value & MMC_BIOSSEL)   /* this controls the mapping of the MMC64 bios */
        {
          mmc64_biossel = MMC_BIOSSEL;
          export.exrom = 0;
          mem_pla_config_changed();
        }
        else
        {
          mmc64_biossel = 0;
          export.exrom = 1;
          mem_pla_config_changed();
        }
      }
      break;
    case 0xdf12:  /* MMC64 status register, read only */
      break;
    case 0xdf13:  /* MMC64 identification register, also used for unlocking sequences */
      mmc64_unlocking[0] = mmc64_unlocking[1];
      mmc64_unlocking[1] = value;
      if (mmc64_unlocking[0]==0x55 && mmc64_unlocking[1]==0xaa)
      {
        log_message(mmc64_log,"bit 7 unlocked");
        mmc64_bit7_unlocked=1;    /* unlock bit 7 of $DF11 */
      }
      if (mmc64_unlocking[0]==0x0a && mmc64_unlocking[1]==0x1c)
      {
        mmc64_active = 0;
        export.exrom = 1;
        mem_pla_config_changed();   /* re-enable the MMC64 */
      }
      break;
    default:      /* Not for us */
      return;
  }
}

BYTE REGPARM1 mmc64_io2_read(WORD addr)
{
  BYTE mmc_readbyte;

  switch(addr)
  {
    case 0xdf10:    /* MMC64 SPI transfer register */
      io_source=IO_SOURCE_MMC64;
      switch(mmc64_card_state)
      {
        case MMC_CARD_RETURN_WRITE:
          mmc64_card_state=MMC_CARD_IDLE;
          return 0xff;
          break;
        case MMC_CARD_RESET:
          log_message(mmc64_log,"Card Reset Response!");
          switch(mmc64_card_reset_count)
          {
            case 0:
              log_message(mmc64_log,"Reset 0");
              mmc64_card_reset_count++;
              return 0x00;
              break;
            case 1:
              log_message(mmc64_log,"Reset 1");
              mmc64_card_reset_count++;
              return 0x01;
              break;
            case 2:
              log_message(mmc64_log,"Reset 2");
              mmc64_card_reset_count++;
              return 0x01;
              break;
            case 3:
              log_message(mmc64_log,"Reset 3");
              mmc64_card_reset_count++;
              return 0x00;
              break;
            case 4:
              log_message(mmc64_log,"Reset 4");
              mmc64_card_reset_count++;
              return 0x01;
              break;
            case 5:
              log_message(mmc64_log,"Reset 5");
              mmc64_card_reset_count=0;
              return 0x01;
              break;
          }
          break;
        case MMC_CARD_INIT:
          io_source=IO_SOURCE_MMC64;
          log_message(mmc64_log,"SPI Card Init Response!");
          return 0x00;
          break;
        case MMC_CARD_READ:
        case MMC_CARD_DUMMY_READ:
          io_source=IO_SOURCE_MMC64;
          if (mmc64_spi_mode==MMC_SPIMODE)
          {
            if (mmc64_read_firstbyte != mmc64_block_size+5)
              mmc64_read_firstbyte++;

            if (mmc64_read_firstbyte == mmc64_block_size+3)
              return 0x00;

            if (mmc64_read_firstbyte == mmc64_block_size+4)
              return 0x01;

            if (mmc64_read_firstbyte == mmc64_block_size+5)
              return 0x00;
          }
          else
          {
            if (mmc64_read_firstbyte != mmc64_block_size+2)
              mmc64_read_firstbyte++;

            if (mmc64_read_firstbyte == mmc64_block_size+1)
              return 0x00;

            if (mmc64_read_firstbyte == mmc64_block_size+2)
              return 0x01;
          }

          if (mmc64_read_firstbyte == 1)
            return 0xFE;

          if (mmc64_read_firstbyte == 2 && mmc64_spi_mode==MMC_SPIMODE)
            return 0xFE;
          
          if (!mmc64_cardpresent && mmc64_card_state!=MMC_CARD_DUMMY_READ)
          {
            fread(&mmc_readbyte, 1,1, mmc64_image_file);
            return mmc_readbyte;
          }
          else
          {
            return 0x00;
          }
          break;
      }
      return 0x00;
      break;
    case 0xdf11:    /* MMC64 control register */
      io_source=IO_SOURCE_MMC64;
      return (mmc64_biossel + mmc64_cardsel + mmc64_speed + mmc64_cport + mmc64_flashmode + mmc64_extrom + mmc64_spi_mode + mmc64_active);
      break;
    case 0xdf12:    /* MMC64 status register */
      io_source=IO_SOURCE_MMC64;
      return (mmc64_flashjumper + mmc64_writeprotect + mmc64_cardpresent + mmc64_extexrom + mmc64_extgame + mmc64_spistatus);
      break;
    case 0xdf13:    /* MMC64 identification register */
      io_source=IO_SOURCE_MMC64;
      if (!mmc64_cardsel)
      {
        return 0x64;
      }
      else
      {
        if (mmc64_revision)
        {
          return 2;
        }
        else
        {
          return 1;
        }
      }
      break;
  }
  return vicii_read_phi1();
}

BYTE REGPARM1 mmc64_roml_read(WORD addr)
{
  if (!mmc64_active && !mmc64_biossel)
  {
    return mmc64_bios[(addr&0x1fff)+mmc64_bios_offset];
  }

  if (plus60k_enabled)
    return plus60k_ram_read(addr);

  if (plus256k_enabled)
    return plus256k_ram_high_read(addr);

  if (c64_256k_enabled)
    return c64_256k_ram_segment2_read(addr);

  return mem_ram[addr];
}

void REGPARM2 mmc64_roml_store(WORD addr, BYTE byte)
{
  if (!mmc64_active && !mmc64_biossel && mmc64_flashjumper && mmc64_flashmode)
  {
    if (mmc64_bios[(addr&0x1fff)+mmc64_bios_offset]!=byte)
    {
      mmc64_bios[(addr&0x1fff)+mmc64_bios_offset]=byte;
      mmc64_bios_changed=1;
      return;
    }
  }

  if (plus60k_enabled)
  {
    plus60k_ram_store(addr, byte);
    return;
  }

  if (plus256k_enabled)
  {
    plus256k_ram_high_store(addr, byte);
    return;
  }

  if (c64_256k_enabled)
  {
    c64_256k_ram_segment2_store(addr, byte);
    return;
  }

  mem_ram[addr]=byte;
}

static const resource_string_t resources_string[] = {
  { "MMC64BIOSfilename", "", RES_EVENT_NO, NULL,
    &mmc64_bios_filename, set_mmc64_bios_filename, NULL },
  { "MMC64imagefilename", "", RES_EVENT_NO, NULL,
    &mmc64_image_filename, set_mmc64_image_filename, NULL },
  { NULL }
};

static const resource_int_t resources_int[] = {
  { "MMC64", 0, RES_EVENT_STRICT, (resource_value_t)0,
    &mmc64_enabled, set_mmc64_enabled, NULL },
  { "MMC64_RO", 0, RES_EVENT_NO, NULL,
    &mmc64_hw_writeprotect, set_mmc64_readonly, NULL },
  { "MMC64_flashjumper", 0, RES_EVENT_NO, NULL,
    &mmc64_hw_flashjumper, set_mmc64_flashjumper, NULL },
  { "MMC64_revision", 0, RES_EVENT_NO, NULL,
    &mmc64_revision, set_mmc64_revision, NULL },
  { "MMC64_bios_write", 0, RES_EVENT_NO, NULL,
    &mmc64_bios_write, set_mmc64_bios_write, NULL },
  { NULL }
};

int mmc64_resources_init(void)
{
  if (resources_register_string(resources_string) < 0)
    return -1;

  return resources_register_int(resources_int);
}

void mmc64_resources_shutdown(void)
{
  lib_free(mmc64_bios_filename);
  lib_free(mmc64_image_filename);
}

/* ------------------------------------------------------------------------- */

static const cmdline_option_t cmdline_options[] =
{
  { "-mmc64", SET_RESOURCE, 0,
    NULL, NULL, "MMC64", (resource_value_t)1,
    USE_PARAM_STRING, USE_DESCRIPTION_ID,
    IDCLS_UNUSED, IDCLS_ENABLE_MMC64,
    NULL, NULL },
  { "+mmc64", SET_RESOURCE, 0,
    NULL, NULL, "MMC64", (resource_value_t)0,
    USE_PARAM_STRING, USE_DESCRIPTION_ID,
    IDCLS_UNUSED, IDCLS_DISABLE_MMC64,
    NULL, NULL },
  { "-mmc64bios", SET_RESOURCE, 1,
    NULL, NULL, "MMC64BIOSfilename", NULL,
    USE_PARAM_ID, USE_DESCRIPTION_ID,
    IDCLS_P_NAME, IDCLS_SPECIFY_MMC64_BIOS_NAME,
    NULL, NULL },
  { "-mmc64image", SET_RESOURCE, 1,
    NULL, NULL, "MMC64imagefilename", NULL,
    USE_PARAM_ID, USE_DESCRIPTION_ID,
    IDCLS_P_NAME, IDCLS_SPECIFY_MMC64_IMAGE_NAME,
    NULL, NULL },
  { "-mmc64readonly", SET_RESOURCE, 0,
    NULL, NULL, "MMC64_RO", (resource_value_t)1,
    USE_PARAM_STRING, USE_DESCRIPTION_ID,
    IDCLS_UNUSED, IDCLS_MMC64_READONLY,
    NULL, NULL },
  { "-mmc64readwrite", SET_RESOURCE, 0,
    NULL, NULL, "MMC64_RO", (resource_value_t)0,
    USE_PARAM_STRING, USE_DESCRIPTION_ID,
    IDCLS_UNUSED, IDCLS_MMC64_READWRITE,
    NULL, NULL },
  { "-mmc64bioswrite", SET_RESOURCE, 0,
    NULL, NULL, "MMC64_bios_write", (resource_value_t)1,
    USE_PARAM_STRING, USE_DESCRIPTION_ID,
    IDCLS_UNUSED, IDCLS_MMC64_BIOS_WRITE,
    NULL, NULL },
  { NULL }
};

int mmc64_cmdline_options_init(void)
{
  return cmdline_register_options(cmdline_options);
}

/* ------------------------------------------------------------------------- */

void mmc64_init(void)
{
  mmc64_log = log_open("MMC64");
}

static int mmc64_activate(void)
{
  FILE *bios_file=NULL;
  int amount_read=0;

  if (mmc64_bios_filename==NULL)
    return -1;

  bios_file=fopen(mmc64_bios_filename,"rb");

  if (bios_file==NULL)
    return -1;

  amount_read=fread(&mmc64_bios,1,0x2002,bios_file);

  fclose(bios_file);

  if (amount_read!=0x2000 && amount_read!=0x2002)
    return -1;

  if (amount_read==0x2002)
    mmc64_bios_offset=2;
  else
    mmc64_bios_offset=0;

  mmc64_bios_changed=0;

  if (mmc64_image_filename==NULL)
  {
    mmc64_cardpresent=MMC_CARDPRS;
    mmc64_reset();
    return 0;
  }

  mmc64_image_file=fopen(mmc64_image_filename,"rb+");

  if (mmc64_image_file==NULL)
  {
    mmc64_image_file=fopen(mmc64_image_filename,"rb");

    if (mmc64_image_file==NULL)
    {
      mmc64_cardpresent=MMC_CARDPRS;
    }
    else
    {
      mmc64_cardpresent=0;
      mmc64_image_file_readonly=1;
      mmc64_hw_writeprotect=1;
      mmc64_writeprotect=MMC_WRITEPROT;
    }
  }
  else
  {
    mmc64_image_file_readonly=0;
    mmc64_cardpresent=0;
  }
  mmc64_reset();
  return 0;

}

static int mmc64_deactivate(void)
{
  FILE *bios_file=NULL;

  if (mmc64_image_file!=NULL)
  {
    fclose(mmc64_image_file);
    mmc64_image_file=NULL;
  }

  if (mmc64_bios_changed && mmc64_bios_write)
  {
    bios_file=fopen(mmc64_bios_filename,"wb");

    if (bios_file==NULL)
      return 0;

    fwrite(&mmc64_bios,1,0x2000+mmc64_bios_offset,bios_file);
    fclose(bios_file);
    mmc64_bios_changed=0;
  }
  return 0;
}

void mmc64_shutdown(void)
{
  mmc64_deactivate();
}
