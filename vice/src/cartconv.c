/*
 * cartconv - Cartridge Conversion utility.
 *
 * Written by
 *  Marco van den heuvel <blackystardust68@yahoo.com>
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

#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#define GENERIC_CRT             0
#define ACTION_REPLAY_CRT       1
#define KCS_CRT                 2
#define FINAL_CARTRIDGE_3_CRT   3
#define SIMONS_BASIC_CRT        4
#define OCEAN_CRT               5
#define EXPERT_CRT              6
#define FUN_PLAY_CRT            7
#define SUPER_GAMES_CRT         8
#define ATOMIC_POWER_CRT        9
#define EPYX_CRT               10
#define WESTERMANN_CRT         11
#define REX_UTILITY_CRT        12
#define FINAL_CARTRIDGE_1_CRT  13
#define MAGIC_FORMEL_CRT       14
#define C64GS_CRT              15
#define WARPSPEED_CRT          16
#define DINAMIC_CRT            17
#define ZAXXON_CRT             18
#define MAGIC_DESK_CRT         19
#define SUPER_SNAPSHOT_5_CRT   20
#define COMAL80_CRT            21
#define STRUCTURED_BASIC_CRT   22
#define ROSS_CRT               23
#define DELA_EP64_CRT          24
#define DELA_EP7x8_CRT         25
#define DELA_EP256_CRT         26
#define REX_EP256_CRT          27
#define MIKRO_ASSEMBLER_CRT    28

#define SIZE_4KB     0x1000
#define SIZE_8KB     0x2000
#define SIZE_16KB    0x4000
#define SIZE_20KB    0x5000
#define SIZE_32KB    0x8000
#define SIZE_64KB    0x10000
#define SIZE_128KB   0x20000
#define SIZE_256KB   0x40000
#define SIZE_512KB   0x80000

FILE *infile, *outfile;
int load_address=0;
int loadfile_offset=0;
unsigned int loadfile_size=0;
char *output_filename=NULL;
char *input_filename[33];
char *cart_name=NULL;
signed char cart_type=-1;
char convert_to_bin=0;
char convert_to_prg=0;
char convert_to_ultimax=0;
unsigned char input_filenames=0;
char loadfile_is_crt=0;
char loadfile_is_ultimax=0;
unsigned char loadfile_cart_type=0;
unsigned char filebuffer[(512*1024)+2];
unsigned char extra_buffer_32kb[0x8000];
unsigned char chipbuffer[16];

typedef struct cart_s {
    unsigned char game;
    unsigned char exrom;
    unsigned int sizes;
    unsigned int bank_size;
    unsigned int load_address;
    unsigned char banks;
    char *name;
} cart_t;


static const cart_t cart_info[] = {
  
/*  {1, 0, SIZE_8KB, 0x2000, 0x8000, 1, "Generic 8kb"}, */
/*  {0, 0, SIZE_16KB, 0x4000, 0x8000, 1, "Generic 16kb"}, */
/*  {0, 1, SIZE_4KB | SIZE_16KB, 0, 0, 1, "Ultimax"}, */

  {1, 0, SIZE_4KB | SIZE_8KB | SIZE_16KB, 0, 0, 1, "Generic Cartridge"},
  {0, 0, SIZE_32KB, 0x2000, 0x8000, 4, "Action Replay"},
  {0, 0, SIZE_16KB, 0x2000, 0, 2, "KCS Power Cartridge"},
  {1, 1, SIZE_64KB, 0x4000, 0x8000, 4, "Final Cartridge III"},
  {1, 0, SIZE_16KB, 0x2000, 0, 2, "Simons Basic"},
  {0, 0, SIZE_128KB | SIZE_256KB | SIZE_512KB, 0x2000, 0, 0, "Ocean"},
  {1, 1, SIZE_8KB, 0x2000, 0x8000, 1, "Expert Cartridge"},
  {0, 0, SIZE_128KB, 0x2000, 0x8000, 16, "Fun Play, Power Play"},
  {0, 0, SIZE_64KB, 0x4000, 0x8000, 4, "Super Games"},
  {0, 0, SIZE_32KB, 0x2000, 0x8000, 4, "Atomic Power"},
  {1, 1, SIZE_8KB, 0x2000, 0x8000, 1, "Epyx Fastload"},
  {0, 0, SIZE_16KB, 0x4000, 0x8000, 1, "Westermann Learning"},
  {1, 0, SIZE_8KB, 0x2000, 0x8000, 1, "Rex Utility"},
  {1, 1, SIZE_16KB, 0x4000, 0x8000, 1, "Final Cartridge I"},
  {0, 0, SIZE_64KB, 0x2000, 0xe000, 8, "Magic Formel"},
  {0, 1, SIZE_512KB, 0x2000, 0x8000, 64, "C64GS, System 3"},
  {0, 1, SIZE_16KB, 0x4000, 0x8000, 1, "WarpSpeed"},
  {0, 1, SIZE_128KB, 0x2000, 0x8000, 16, "Dinamic"},
  {1, 1, SIZE_20KB, 0, 0, 3, "Zaxxon"},
  {0, 1, SIZE_32KB | SIZE_64KB | SIZE_128KB, 0x2000, 0x8000, 0, "Magic Desk, Domark, Hes Australia"},
  {1, 1, SIZE_64KB, 0x4000, 0x8000, 4, "Super Snapshot 5"},
  {1, 1, SIZE_64KB, 0x4000, 0x8000, 4, "Comal-80"},
  {0, 1, SIZE_16KB, 0x2000, 0x8000, 2, "Structured Basic"},
  {1, 1, SIZE_16KB | SIZE_32KB, 0x4000, 0x8000, 0, "Ross"},
  {1, 0, SIZE_8KB, 0, 0x8000, 0, "Dela EP64"},
  {1, 0, SIZE_8KB, 0x2000, 0x8000, 0, "Dela EP7x8"},
  {1, 0, SIZE_8KB, 0x2000, 0x8000, 0, "Dela EP256"},
  {1, 0, SIZE_8KB, 0, 0x8000, 0, "Rex EP256"},
  {1, 0, SIZE_8KB, 0x2000, 0x8000, 1, "Mikro Assembler"}
};

#ifndef HAVE_STRNCASECMP
static const unsigned char charmap[] = {
	'\000', '\001', '\002', '\003', '\004', '\005', '\006', '\007',
	'\010', '\011', '\012', '\013', '\014', '\015', '\016', '\017',
	'\020', '\021', '\022', '\023', '\024', '\025', '\026', '\027',
	'\030', '\031', '\032', '\033', '\034', '\035', '\036', '\037',
	'\040', '\041', '\042', '\043', '\044', '\045', '\046', '\047',
	'\050', '\051', '\052', '\053', '\054', '\055', '\056', '\057',
	'\060', '\061', '\062', '\063', '\064', '\065', '\066', '\067',
	'\070', '\071', '\072', '\073', '\074', '\075', '\076', '\077',
	'\100', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\133', '\134', '\135', '\136', '\137',
	'\140', '\141', '\142', '\143', '\144', '\145', '\146', '\147',
	'\150', '\151', '\152', '\153', '\154', '\155', '\156', '\157',
	'\160', '\161', '\162', '\163', '\164', '\165', '\166', '\167',
	'\170', '\171', '\172', '\173', '\174', '\175', '\176', '\177',
	'\200', '\201', '\202', '\203', '\204', '\205', '\206', '\207',
	'\210', '\211', '\212', '\213', '\214', '\215', '\216', '\217',
	'\220', '\221', '\222', '\223', '\224', '\225', '\226', '\227',
	'\230', '\231', '\232', '\233', '\234', '\235', '\236', '\237',
	'\240', '\241', '\242', '\243', '\244', '\245', '\246', '\247',
	'\250', '\251', '\252', '\253', '\254', '\255', '\256', '\257',
	'\260', '\261', '\262', '\263', '\264', '\265', '\266', '\267',
	'\270', '\271', '\272', '\273', '\274', '\275', '\276', '\277',
	'\300', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\333', '\334', '\335', '\336', '\337',
	'\340', '\341', '\342', '\343', '\344', '\345', '\346', '\347',
	'\350', '\351', '\352', '\353', '\354', '\355', '\356', '\357',
	'\360', '\361', '\362', '\363', '\364', '\365', '\366', '\367',
	'\370', '\371', '\372', '\373', '\374', '\375', '\376', '\377',
};

int strncasecmp(const char *s1, const char *s2, int n)
{
  unsigned char u1, u2;

  for (; n != 0; --n)
  {
    u1=(unsigned char)*s1++;
    u2=(unsigned char)*s2++;
    if (charmap[u1]!=charmap[u2])
      return charmap[u1]-charmap[u2];

    if (u1 == '\0')
      return 0;
  }
  return 0;
}
#endif

static void cleanup(void)
{
  int i;

  if (output_filename!=NULL)
    free(output_filename);
  if (cart_name!=NULL)
    free(cart_name);
  for (i=0; i<33; i++)
  {
    if (input_filename[i]!=NULL)
      free(input_filename[i]);
  }
}

static void usage(void)
{
  cleanup();
  printf("cartconv [-t carttype] -i \"input name\" -o \"output name\" [-n \"cart name\"] [-l loadaddress]\n");
  printf("carttypes:\n");
  printf("bin      Binary .bin file (Default crt->bin\n");
  printf("prg      Binary C64 .prg file with load-address\n");
  printf("normal   Generic 8kb/16kb .crt file (Default bin->crt)\n");
  printf("ulti     Ultimax mode 4kb/16kb .crt file\n");
  printf("ar       Action Replay .crt file\n");
  printf("kcs      KCS .crt file\n");
  printf("fc3      Final Cartridge 3 .crt file\n");
  printf("simon    Simons Basic .crt file\n");
  printf("ocean    Ocean type 1/2 .crt file\n");
  printf("expert   Expert Cartridge .crt file\n");
  printf("fp       Fun Play, Power Play .crt file\n");
  printf("sg       Super Games .crt file\n");
  printf("ap       Atomic Power .crt file\n");
  printf("epyx     Epyx Fastload .crt file\n");
  printf("wl       Westermann Learning .crt file\n");
  printf("ru       Rex Utility .crt file\n");
  printf("fc1      Final Cartridge 1 .crt file\n");
  printf("mf       Magic Formel .crt file\n");
  printf("gs       C64GS, System 3 .crt file\n");
  printf("ws       WarpSpeed .crt file\n");
  printf("din      Dinamic .crt file\n");
  printf("zax      Zaxxon, Super Zaxxon .crt file\n");
  printf("md       Magic Desk, Domark, Hes Australia .crt file\n");
  printf("ss5      Super Snapshot 5 .crt file\n");
  printf("comal    Comal-80 .crt file\n");
  printf("sb       Structured Basic .crt file\n");
  printf("ross     Ross .crt file\n");
  printf("mikro    Mikro Assembler .crt file\n");
  printf("dep64    Dela EP64 .crt file, extra files can be inserted\n");
  printf("dep7x8   Dela EP7x8 .crt file, extra files can be inserted\n");
  printf("dep256   Dela EP256 .crt file, extra files can be inserted\n");
  printf("rep256   Rex EP256 .crt file, extra files can be inserted\n");
  exit(1);
}

static void checkflag(char *flg, char *arg)
{
  switch (tolower(flg[1]))
  {
    case 'o':
      if (output_filename==NULL)
        output_filename=strdup(arg);
      else
        usage();
      break;
    case 'n':
      if (cart_name==NULL)
        cart_name=strdup(arg);
      else
        usage();
      break;
    case 'l':
      if (load_address==0)
        load_address=atoi(arg);
      else
        usage();
      break;
    case 't':
      if (cart_type!=-1 || convert_to_bin!=0 || convert_to_prg!=0 || convert_to_ultimax!=0)
        usage();
      else
      {
        switch (tolower(arg[0]))
        {
          case 'a':
            if (tolower(arg[1])=='p' || tolower(arg[1])=='t')
              cart_type=ATOMIC_POWER_CRT;
            if (tolower(arg[1])=='r' || tolower(arg[1])=='c')
              cart_type=ACTION_REPLAY_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'b':
            convert_to_bin=1;
            break;
          case 'c':
            if (tolower(arg[1])=='6')
              cart_type=C64GS_CRT;
            if (tolower(arg[1])=='o')
              cart_type=COMAL80_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'd':
            if (tolower(arg[1])=='i')
              cart_type=DINAMIC_CRT;
            if (tolower(arg[1])=='o')
              cart_type=MAGIC_DESK_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='p' && tolower(arg[3])=='2')
              cart_type=DELA_EP256_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='p' && tolower(arg[3])=='6')
              cart_type=DELA_EP64_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='p' && tolower(arg[3])=='7')
              cart_type=DELA_EP7x8_CRT;
            if (!strncasecmp(arg,"delaep2",7))
              cart_type=DELA_EP256_CRT;
            if (!strncasecmp(arg,"delaep6",7))
              cart_type=DELA_EP64_CRT;
            if (!strncasecmp(arg,"delaep7",7))
              cart_type=DELA_EP7x8_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'e':
            if (tolower(arg[1])=='f' || tolower(arg[1])=='p')
              cart_type=EPYX_CRT;
            if (tolower(arg[1])=='x')
              cart_type=EXPERT_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'f':
            if (tolower(arg[1])=='p' || tolower(arg[1])=='u')
              cart_type=FUN_PLAY_CRT;
            if (tolower(arg[1])=='c' && tolower(arg[2])=='1')
              cart_type=FINAL_CARTRIDGE_1_CRT;
            if (tolower(arg[1])=='c' && tolower(arg[2])=='3')
              cart_type=FINAL_CARTRIDGE_3_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'g':
            cart_type=C64GS_CRT;
            break;
          case 'h':
            cart_type=MAGIC_DESK_CRT;
            break;
          case 'k':
            cart_type=KCS_CRT;
            break;
          case 'm':
            if (tolower(arg[1])=='d')
              cart_type=MAGIC_DESK_CRT;
            if (tolower(arg[1])=='f')
              cart_type=MAGIC_FORMEL_CRT;
            if (!strncasecmp(arg,"magicd",6))
              cart_type=MAGIC_DESK_CRT;
            if (!strncasecmp(arg,"magicf",6))
              cart_type=MAGIC_FORMEL_CRT;
            if (tolower(arg[1])=='i')
              cart_type=MIKRO_ASSEMBLER_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'n':
            cart_type=GENERIC_CRT;
            break;
          case 'o':
            cart_type=OCEAN_CRT;
            break;
          case 'p':
            if (tolower(arg[1])=='r' && tolower(arg[2])=='g')
              convert_to_prg=1;
            if (tolower(arg[1])=='r' && tolower(arg[2])=='o')
              convert_to_prg=1;
            if (tolower(arg[1])=='o')
              cart_type=FUN_PLAY_CRT;
            if (cart_type==-1 && convert_to_prg==0)
              usage();
            break;
          case 'r':
            if (tolower(arg[1])=='e' && tolower(arg[2])=='p' && tolower(arg[3])=='2')
              cart_type=REX_EP256_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='x' && tolower(arg[3])=='e')
              cart_type=REX_EP256_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='x' && tolower(arg[3])=='u')
              cart_type=REX_UTILITY_CRT;
            if (tolower(arg[1])=='e' && tolower(arg[2])=='p' && tolower(arg[3])=='l')
              cart_type=ACTION_REPLAY_CRT;
            if (tolower(arg[1])=='o')
              cart_type=ROSS_CRT;
            if (tolower(arg[1])=='u')
              cart_type=REX_UTILITY_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 's':
            if (tolower(arg[1])=='3' || tolower(arg[1])=='y')
              cart_type=C64GS_CRT;
            if (tolower(arg[1])=='b' || tolower(arg[1])=='t')
              cart_type=STRUCTURED_BASIC_CRT;
            if (tolower(arg[1])=='g')
              cart_type=SUPER_GAMES_CRT;
            if (tolower(arg[1])=='i')
              cart_type=SIMONS_BASIC_CRT;
            if (tolower(arg[1])=='s')
              cart_type=SUPER_SNAPSHOT_5_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'u':
            cart_type=GENERIC_CRT;
            convert_to_ultimax=1;
            break;
          case 'w':
            if (tolower(arg[1])=='l' || tolower(arg[1])=='l')
              cart_type=WESTERMANN_CRT;
            if (tolower(arg[1])=='s' || tolower(arg[1])=='a')
              cart_type=WARPSPEED_CRT;
            if (cart_type==-1)
              usage();
            break;
          case 'z':
            cart_type=ZAXXON_CRT;
            break;
          default:
            usage();
        }
      }
      break;
    case 'i':
      if (input_filenames==33)
        usage();
      input_filename[input_filenames]=strdup(arg);
      input_filenames++;
      break;
    default:
      usage();
  }
}

static void too_many_inputs(void)
{
  printf("Error: too many input files\n");
  cleanup();
  exit(1);
}

static int load_all_banks(void)
{
  unsigned int length;

  while (1)
  {
    if (fread(chipbuffer,1,16,infile)!=16)
    {
      if (loadfile_size==0)
        return -1;
      else
        return 0;
    }
    if (chipbuffer[0]!='C' || chipbuffer[1]!='H' || chipbuffer[2]!='I' || chipbuffer[3]!='P')
      return -1;
    if (load_address==0)
      load_address=(chipbuffer[0xc]<<8)+chipbuffer[0xd];
    length=(chipbuffer[4]<<24)+(chipbuffer[5]<<16)+(chipbuffer[6]<<8)+chipbuffer[7]-16;
    if (fread(filebuffer+loadfile_size,1,length,infile)!=length)
      return -1;
    loadfile_size+=length;
  }
}

static int save_binary_output_file(void)
{
  char address_buffer[2];

  outfile=fopen(output_filename,"wb");
  if (outfile==NULL)
  {
    printf("Error: Can't open output file %s\n",output_filename);
    return -1;
  }
  if (convert_to_prg==1)
  {
    address_buffer[0]=load_address&0xff;
    address_buffer[1]=load_address>>8;
    if(fwrite(address_buffer,1,2,outfile)!=2)
    {
      printf("Error: Can't write to file %s\n",output_filename);
      fclose(outfile);
      return -1;
    }
  }
  if (fwrite(filebuffer,1,loadfile_size,outfile)!=loadfile_size)
  {
    printf("Error: Can't write to file %s\n",output_filename);
    fclose(outfile);
    return -1;
  }
  fclose(outfile);
  printf("Input file : %s\n",input_filename[0]);
  printf("Output file : %s\n",output_filename);
  printf("Conversion from %s .crt to binary format successful.\n",cart_info[loadfile_cart_type].name);
  return 0;
}

static int write_crt_header(unsigned char gameline, unsigned char exromline)
{
  unsigned char crt_header[0x40]="C64 CARTRIDGE   ";
  int endofname=0;
  int i;

  crt_header[0x10]=0;
  crt_header[0x11]=0;
  crt_header[0x12]=0;
  crt_header[0x13]=0x40;

  crt_header[0x14]=1;
  crt_header[0x15]=0;

  crt_header[0x16]=0;
  crt_header[0x17]=cart_type;

  crt_header[0x18]=exromline;
  crt_header[0x19]=gameline;

  crt_header[0x1a]=0;
  crt_header[0x1b]=0;
  crt_header[0x1c]=0;
  crt_header[0x1d]=0;
  crt_header[0x1e]=0;
  crt_header[0x1f]=0;

  if (cart_name==NULL)
    cart_name=strdup("VICE CART");
  
  for (i=0; i<32; i++)
  {
    if (endofname==1)
      crt_header[0x20+i]=0;
    else
    {
      if (cart_name[i]==0)
        endofname=1;
      else
        crt_header[0x20+i]=toupper(cart_name[i]);
    }
  }

  outfile=fopen(output_filename,"wb");
  if (outfile==NULL)
  {
    printf("Error: Can't open output file %s\n",output_filename);
    return -1;
  }
  if (fwrite(crt_header,1,0x40,outfile)!=0x40)
  {
    printf("Error: Can't write crt header to file %s\n",output_filename);
    fclose(outfile);
    unlink(output_filename);
    return -1;
  }
  return 0;
}

static int write_chip_package(unsigned int length, unsigned int bankint, unsigned int address, unsigned char type)
{
  unsigned char chip_header[0x10]="CHIP";
  unsigned char bank = (unsigned char) bankint;

  /* make sure the above conversion did not remove significant bits */
  assert(bankint == bank);

  chip_header[4]=0;
  chip_header[5]=0;
  chip_header[6]=(unsigned char)((length+0x10)>>8);
  chip_header[7]=(unsigned char)((length+0x10)&0xff);

  chip_header[8]=0;
  chip_header[9]=type;

  chip_header[0xa]=0;
  chip_header[0xb]=bank;

  chip_header[0xc]=(unsigned char)(address>>8);
  chip_header[0xd]=(unsigned char)(address&0xff);

  chip_header[0xe]=(unsigned char)(length>>8);
  chip_header[0xf]=(unsigned char)(length&0xff);
  if (fwrite(chip_header,1,0x10,outfile)!=0x10)
  {
    printf("Error: Can't write chip header to file %s\n",output_filename);
    fclose(outfile);
    unlink(output_filename);
    return -1;
  }
  if (fwrite(filebuffer+loadfile_offset,1,length,outfile)!=length)
  {
    printf("Error: Can't write data to file %s\n",output_filename);
    fclose(outfile);
    unlink(output_filename);
    return -1;
  }
  loadfile_offset+=length;
  return 0;
}

static void bin2crt_ok(void)
{
  printf("Input file : %s\n",input_filename[0]);
  printf("Output file : %s\n",output_filename);
  printf("Conversion from binary format to %s .crt successful.\n",cart_info[(unsigned char)cart_type].name);
}

static void save_regular_crt(unsigned int length,
                            unsigned char banks,
                            unsigned int address,
                            unsigned char type,
                            unsigned char gameline,
                            unsigned char exromline)
{
  int i;

  if (write_crt_header(gameline,exromline)<0)
  {
    cleanup();
    exit(1);
  }

  for (i=0; i<banks; i++)
  {
    if (write_chip_package(length, i, address, type)<0)
    {
      cleanup();
      exit(1);
    }
  }
  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_2_blocks_crt(unsigned int length1, unsigned int length2,
                              unsigned int address1, unsigned int address2,
                              unsigned char gameline, unsigned char exromline)
{
  if (write_crt_header(gameline,exromline)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(length1, 0, address1, 0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(length2, 0, address2, 0)<0)
  {
    cleanup();
    exit(1);
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_ocean_256kb_crt(void)
{
  int i;

  if (write_crt_header(0,0)<0)
  {
    cleanup();
    exit(1);
  }

  for (i=0; i<16; i++)
  {
    if (write_chip_package(0x2000, i, 0x8000, 0)<0)
    {
      cleanup();
      exit(1);
    }
  }

  for (i=0; i<16; i++)
  {
    if (write_chip_package(0x2000, i+16, 0xa000, 0)<0)
    {
      cleanup();
      exit(1);
    }
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_funplay_crt(void)
{
  int i=0;

  if (write_crt_header(0,0)<0)
  {
    cleanup();
    exit(1);
  }

  while (i!=0x41)
  {
    if (write_chip_package(0x2000, i, 0x8000, 0)<0)
    {
      cleanup();
      exit(1);
    }
    i+=8;
    if (i==0x40)
      i=1;
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_zaxxon_crt(void)
{
  if (write_crt_header(1,1)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x1000, 0, 0x8000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 0, 0xa000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 1, 0xa000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static int load_input_file(char *filename)
{
  loadfile_offset=0;
  infile=fopen(filename,"rb");
  if (infile==NULL)
  {
    printf("Error: Can't open %s\n",filename);
    return -1;
  }
  if (fread(filebuffer,1,16,infile)!=16)
  {
    printf("Error: Can't read %s\n",filename);
    fclose(infile);
    return -1;
  }
  if (!strncmp("C64 CARTRIDGE   ",filebuffer,16))
  {
    loadfile_is_crt=1;
    if (fread(filebuffer+0x10,1,0x30,infile)!=0x30)
    {
      printf("Error: Can't read the full header of %s\n",filename);
      fclose(infile);
      return -1;
    }
    if (filebuffer[0x10]!=0 || filebuffer[0x11]!=0 || filebuffer[0x12]!=0 || filebuffer[0x13]!=0x40)
    {
      printf("Error: Illegal header size in %s\n",filename);
      fclose(infile);
      return -1;
    }
    if (filebuffer[0x18]==1 && filebuffer[0x19]==0)
      loadfile_is_ultimax=1;
    else
      loadfile_is_ultimax=0;
    loadfile_cart_type=filebuffer[0x17];
    loadfile_size=0;
    if (load_all_banks()<0)
    {
      printf("Error: Can't load all banks of %s\n",filename);
      fclose(infile);
      return -1;
    }
    else
    {
      fclose(infile);
      return 0;
    }
  }
  else
  {
    loadfile_is_crt=0;
    loadfile_size=fread(filebuffer+0x10,1,512*1024,infile)+0x10;
    switch (loadfile_size)
    {
      case 4096:
      case 8192:
      case 16384:
      case 20480:
      case 32768:
      case 65536:
      case 131072:
      case 262144:
      case 524288:
        loadfile_offset=0;
        fclose(infile);
        return 0;
        break;
      case 4098:
      case 8194:
      case 16386:
      case 20482:
      case 32770:
      case 65538:
      case 131074:
      case 262146:
      case 524290:
        loadfile_size-=2;
        loadfile_offset=2;
        fclose(infile);
        return 0;
        break;
      case 32772:
        loadfile_size-=4;
        loadfile_offset=4;
        fclose(infile);
        return 0;
        break;
      default:
        printf("Error: Illegal file size of %s\n",filename);
        fclose(infile);
        return -1;
    }
  }
}

static void close_output_cleanup(void)
{
  fclose(outfile);
  unlink(output_filename);
  cleanup();
  exit(1);
}

static void save_delaep64_crt(void)
{
  int i;

  if (loadfile_size!=SIZE_8KB)
  {
    printf("Error: wrong size of Dela EP64 base file %s (%d)\n",input_filename[0],loadfile_size);
    cleanup();
    exit(1);
  }

  if (input_filenames==1)
  {
    printf("Error: no files to insert into Dela EP64 .crt\n");
    cleanup();
    exit(1);
  }

  if (write_crt_header(1,0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 0, 0x8000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  for (i=0; i<input_filenames; i++)
  {
    if (load_input_file(input_filename[i])<0)
      close_output_cleanup();
    if (loadfile_is_crt==1)
    {
      printf("Error: to be inserted file can only be a binary for Dela EP64\n");
      close_output_cleanup();
    }
    if (loadfile_size!=SIZE_32KB)
    {
      printf("Error: to be insterted file can only be 32KB in size for Dela EP64\n");
      close_output_cleanup();
    }
    if (write_chip_package(0x8000,i+1,0x8000,0)<0)
      close_output_cleanup();
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_delaep256_crt(void)
{
  int i,j;
  unsigned int insert_size=0;

  if (loadfile_size!=SIZE_8KB)
  {
    printf("Error: wrong size of Dela EP256 base file %s (%d)\n",input_filename[0],loadfile_size);
    cleanup();
    exit(1);
  }

  if (input_filenames==1)
  {
    printf("Error: no files to insert into Dela EP256 .crt\n");
    cleanup();
    exit(1);
  }

  if (write_crt_header(1,0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 0, 0x8000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  for (i=0; i<input_filenames-1; i++)
  {
    if (load_input_file(input_filename[i+1])<0)
      close_output_cleanup();

    if (loadfile_size!=SIZE_32KB && loadfile_size!=SIZE_8KB)
    {
      printf("Error: only 32KB binary files or 8KB bin/crt files can be inserted in Dela EP256\n");
      close_output_cleanup();
    }

    if (insert_size==0)
      insert_size=loadfile_size;

    if (insert_size==SIZE_32KB && input_filenames>8)
    {
      printf("Error: a maximum of 8 32KB images can be inserted\n");
      close_output_cleanup();
    }

    if (insert_size!=loadfile_size)
    {
      printf("Error: only one type of insertion is allowed at this time for Dela EP256\n");
      close_output_cleanup();
    }

    if (loadfile_is_crt==1 && (loadfile_size!=SIZE_8KB || load_address!=0x8000 || loadfile_is_ultimax==1))
    {
      printf("Error: you can only insert generic 8KB .crt files for Dela EP256\n");
      close_output_cleanup();
    }

    if (insert_size==SIZE_32KB)
    {
      for (j=0; j<4; j++)
      {
        if (write_chip_package(0x2000,(i*4)+j+1,0x8000,0)<0)
          close_output_cleanup();
      }
      printf("inserted %s in banks %d-%d of the Dela EP256 .crt\n",input_filename[i+1],(i*4)+1,(i*4)+4);
    }
    else
    {
      if (write_chip_package(0x2000,i+1,0x8000,0)<0)
        close_output_cleanup();
        printf("inserted %s in bank %d of the Dela EP256 .crt\n",input_filename[i+1],i+1);
    }
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_delaep7x8_crt(void)
{
  int inserted_size=0;
  int name_counter=1;
  int chip_counter=1;

  if (loadfile_size!=SIZE_8KB)
  {
    printf("Error: wrong size of Dela EP7x8 base file %s (%d)\n",input_filename[0],loadfile_size);
    cleanup();
    exit(1);
  }

  if (input_filenames==1)
  {
    printf("Error: no files to insert into Dela EP7x8 .crt\n");
    cleanup();
    exit(1);
  }

  if (write_crt_header(1,0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 0, 0x8000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  while (name_counter!=input_filenames)
  {
    if (load_input_file(input_filename[name_counter])<0)
      close_output_cleanup();

    if (loadfile_size==SIZE_32KB)
    {
      if (loadfile_is_crt==1)
      {
        printf("Error: (%s) only binary 32KB images can be inserted into a Dela EP7x8 .crt\n", input_filename[name_counter]);
        close_output_cleanup();
      }
      else
      {
        if (inserted_size!=0)
        {
          printf("Error: (%s) only the first inserted image can be a 32KB image for Dela EP7x8\n", input_filename[name_counter]);
          close_output_cleanup();
        }
        else
        {
          if (write_chip_package(0x2000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          if (write_chip_package(0x2000,chip_counter+1,0x8000,0)<0)
            close_output_cleanup();
          if (write_chip_package(0x2000,chip_counter+2,0x8000,0)<0)
            close_output_cleanup();
          if (write_chip_package(0x2000,chip_counter+3,0x8000,0)<0)
            close_output_cleanup();
          printf("inserted %s in banks %d-%d of the Dela EP7x8 .crt\n",input_filename[name_counter],chip_counter,chip_counter+3);
          chip_counter+=4;
          inserted_size+=0x8000;
        }
      }
    }

    if (loadfile_size==SIZE_16KB)
    {
      if (loadfile_is_crt==1 && (loadfile_cart_type!=0 || loadfile_is_ultimax==1))
      {
        printf("Error: (%s) only generic 16KB .crt images can be inserted into a Dela EP7x8 .crt\n",input_filename[name_counter]);
        close_output_cleanup();
      }
      else
      {
        if (inserted_size>=0xc000)
        {
          printf("Error: (%s) no room to insert a 16KB binary file into the Dela EP7x8 .crt\n",input_filename[name_counter]);
          close_output_cleanup();
        }
        else
        {
          if (write_chip_package(0x2000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          if (write_chip_package(0x2000,chip_counter+1,0x8000,0)<0)
            close_output_cleanup();
          printf("inserted %s in banks %d and %d of the Dela EP7x8 .crt\n",input_filename[name_counter],chip_counter,chip_counter+1);
          chip_counter+=2;
          inserted_size+=0x4000;
        }
      }
    }

    if (loadfile_size==SIZE_8KB)
    {
      if (loadfile_is_crt==1 && (loadfile_cart_type!=0 || loadfile_is_ultimax==1))
      {
        printf("Error: (%s) only generic 8KB .crt images can be inserted into a Dela EP7x8 .crt\n",input_filename[name_counter]);
        close_output_cleanup();
      }
      else
      {
        if (inserted_size>=0xe000)
        {
          printf("Error: (%s) no room to insert a 8KB binary file into the Dela EP7x8 .crt\n",input_filename[name_counter]);
          close_output_cleanup();
        }
        else
        {
          if (write_chip_package(0x2000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          printf("inserted %s in bank %d of the Dela EP7x8 .crt\n",input_filename[name_counter],chip_counter);
          chip_counter++;
          inserted_size+=0x2000;
        }
      }
    }

    name_counter++;
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

static void save_rexep256_crt(void)
{
  int eprom_size_for_8kb=0;
  int images_of_8kb_started=0;
  int name_counter=1;
  int chip_counter=1;
  int subchip_counter=1;

  if (loadfile_size!=SIZE_8KB)
  {
    printf("Error: wrong size of Rex EP256 base file %s (%d)\n",input_filename[0],loadfile_size);
    cleanup();
    exit(1);
  }

  if (input_filenames==1)
  {
    printf("Error: no files to insert into Rex EP256 .crt\n");
    cleanup();
    exit(1);
  }

  if (write_crt_header(1,0)<0)
  {
    cleanup();
    exit(1);
  }

  if (write_chip_package(0x2000, 0, 0x8000, 0)<0)
  {
    cleanup();
    exit(1);
  }

  while (name_counter!=input_filenames)
  {
    if (load_input_file(input_filename[name_counter])<0)
      close_output_cleanup();

    if (chip_counter>8)
    {
      printf("Error: no more room for %s in the Rex EP256 .crt\n",input_filename[name_counter]);
    }

    if (loadfile_size==SIZE_32KB)
    {
      if (loadfile_is_crt==1)
      {
        printf("Error: (%s) only binary 32KB images can be inserted into a Rex EP256 .crt\n", input_filename[name_counter]);
        close_output_cleanup();
      }
      else
      {
        if (images_of_8kb_started!=0)
        {
          printf("Error: (%s) only the first inserted images can be a 32KB image for Rex EP256\n", input_filename[name_counter]);
          close_output_cleanup();
        }
        else
        {
          if (write_chip_package(0x8000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          printf("inserted %s in bank %d as a 32KB eprom of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
          chip_counter++;
        }
      }
    }

    if (loadfile_size==SIZE_8KB)
    {
      if (loadfile_is_crt==1 && (loadfile_cart_type!=0 || loadfile_is_ultimax==1))
      {
        printf("Error: (%s) only generic 8KB .crt images can be inserted into a Rex EP256 .crt\n",input_filename[name_counter]);
        close_output_cleanup();
      }
      else
      {
        if (images_of_8kb_started==0)
        {
          images_of_8kb_started=1;
          if ((9-chip_counter)*4<input_filenames-name_counter)
          {
            printf("Error: no room for the amount of input files given\n");
            close_output_cleanup();
          }
          eprom_size_for_8kb=1;
          if ((9-chip_counter)*2<input_filenames-name_counter)
            eprom_size_for_8kb=4;
          if (9-chip_counter<input_filenames-name_counter)
            eprom_size_for_8kb=2;
        }

        if (eprom_size_for_8kb==1)
        {
          if (write_chip_package(0x2000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          printf("inserted %s as an 8KB eprom in bank %d of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
          chip_counter++;
        }

        if (eprom_size_for_8kb==4 && (subchip_counter==4 || name_counter==input_filenames-1))
        {
          memcpy(extra_buffer_32kb+((subchip_counter-1)*0x2000),filebuffer+loadfile_offset,0x2000);
          memcpy(filebuffer,extra_buffer_32kb,0x8000);
          loadfile_offset=0;
          if (write_chip_package(0x8000,chip_counter,0x8000,0)<0)
            close_output_cleanup();
          if (subchip_counter==1)
            printf("inserted %s as a 32KB eprom in bank %d of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
          else
            printf(" and %s as a 32KB eprom in bank %d of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
          chip_counter++;
          subchip_counter=1;
        }

        if (eprom_size_for_8kb==4 && (subchip_counter==3 || subchip_counter==2) && name_counter!=input_filenames)
        {
          memcpy(extra_buffer_32kb+((subchip_counter-1)*0x2000),filebuffer+loadfile_offset,0x2000);
          printf(", %s",input_filename[name_counter]);
          subchip_counter++;
        }

        if (eprom_size_for_8kb==2)
        {
          if (subchip_counter==2 || name_counter==input_filenames-1)
          {
            memcpy(extra_buffer_32kb+((subchip_counter-1)*0x2000),filebuffer+loadfile_offset,0x2000);
            memcpy(filebuffer,extra_buffer_32kb,0x4000);
            loadfile_offset=0;
            if (write_chip_package(0x4000,chip_counter,0x8000,0)<0)
              close_output_cleanup();
            if (subchip_counter==1)
              printf("inserted %s as a 16KB eprom in bank %d of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
            else
              printf(" and %s as a 16KB eprom in bank %d of the Rex EP256 .crt\n",input_filename[name_counter],chip_counter);
            chip_counter++;
            subchip_counter=1;
          }
          else
          {
            memcpy(extra_buffer_32kb,filebuffer+loadfile_offset,0x2000);
            printf("inserted %s",input_filename[name_counter]);
            subchip_counter++;
          }
        }

        if (eprom_size_for_8kb==4 && subchip_counter==1 && name_counter!=input_filenames)
        {
          memcpy(extra_buffer_32kb,filebuffer+loadfile_offset,0x2000);
          printf("inserted %s",input_filename[name_counter]);
          subchip_counter++;
        }
      }
    }
    name_counter++;
  }

  fclose(outfile);
  bin2crt_ok();
  cleanup();
  exit(0);
}

int main(int argc, char *argv[])
{
  int i;
  int arg_counter=1;
  char *flag, *argument;

  if (argc==1)
    usage();
  if (((argc>>1)<<1)==argc)
    usage();

  for (i=0; i<33; i++)
    input_filename[i]=NULL;

  while (arg_counter!=argc)
  {
    flag=argv[arg_counter];
    argument=argv[arg_counter+1];
    if (flag[0]!='-')
      usage();
    else
      checkflag(flag, argument);
    arg_counter+=2;
  }
  if (output_filename==NULL)
  {
    printf("Error: no output filename\n");
    cleanup();
    exit(1);
  }
  if (input_filenames==0)
  {
    printf("Error: no input filename\n");
    cleanup();
    exit(1);
  }
  if (load_input_file(input_filename[0])<0)
  {
    cleanup();
    exit(1);
  }
  if (input_filenames>1 && cart_type!=DELA_EP64_CRT && cart_type!=DELA_EP256_CRT
                        && cart_type!=DELA_EP7x8_CRT && cart_type!=REX_EP256_CRT
                        && loadfile_cart_type!=DELA_EP64_CRT && loadfile_cart_type!=DELA_EP256_CRT
                        && loadfile_cart_type!=DELA_EP7x8_CRT && loadfile_cart_type!=REX_EP256_CRT)
    too_many_inputs();
  if ((cart_type==DELA_EP64_CRT || loadfile_cart_type==DELA_EP64_CRT) && input_filenames>3)
    too_many_inputs();
  if ((cart_type==DELA_EP7x8_CRT || loadfile_cart_type==DELA_EP7x8_CRT) && input_filenames>8)
    too_many_inputs();
  if (loadfile_is_crt==1)
  {
    if (cart_type==DELA_EP64_CRT || cart_type==DELA_EP256_CRT || cart_type==DELA_EP7x8_CRT || cart_type==REX_EP256_CRT)
    {
      switch (cart_type)
      {
        case DELA_EP64_CRT:
          save_delaep64_crt();
          break;
        case DELA_EP256_CRT:
          save_delaep256_crt();
          break;
        case DELA_EP7x8_CRT:
          save_delaep7x8_crt();
          break;
        case REX_EP256_CRT:
          save_rexep256_crt();
          break;
      }
    }
    else
    {
      if (cart_type==-1)
      {
        if (save_binary_output_file()<0)
        {
          cleanup();
          exit(1);
        }
      }
      else
      {
        printf("Error: File is already .crt format\n");
        cleanup();
        exit(1);
      }
    }
  }
  else
  {
    if (cart_type==-1)
    {
      printf("Error: File is already in binary format\n");
      cleanup();
      exit(1);
    }
    if ((loadfile_size&cart_info[(unsigned char)cart_type].sizes)!=loadfile_size)
    {
      printf("Error: Input file size (%d) doesn't match %s requirements\n",loadfile_size,cart_info[(unsigned char)cart_type].name);
      cleanup();
      exit(1);
    }
    switch (cart_type)
    {
      case GENERIC_CRT:
        if (convert_to_ultimax==1)
        {
          switch (loadfile_size)
          {
            case SIZE_4KB:
              save_regular_crt(0x1000,1,0xf000,0,0,1);
              break;
            case SIZE_8KB:
              save_regular_crt(0x2000,1,0xe000,0,0,1);
              break;
            case SIZE_16KB:
              save_2_blocks_crt(0x2000, 0x2000, 0x8000, 0xe000, 0, 1);
              break;
          }
        }
        else
        {
          switch (loadfile_size)
          {
            case SIZE_4KB:
              save_regular_crt(0x1000,1,0x8000,0,1,0);
              break;
            case SIZE_8KB:
              save_regular_crt(0x2000,1,0x8000,0,1,0);
              break;
            case SIZE_16KB:
              save_regular_crt(0x4000,1,0x8000,0,0,0);
              break;
          }
        }
        break;
      case ACTION_REPLAY_CRT:
      case FINAL_CARTRIDGE_3_CRT:
      case SUPER_GAMES_CRT:
      case ATOMIC_POWER_CRT:
      case EPYX_CRT:
      case MIKRO_ASSEMBLER_CRT:
      case WESTERMANN_CRT:
      case REX_UTILITY_CRT:
      case FINAL_CARTRIDGE_1_CRT:
      case MAGIC_FORMEL_CRT:
      case C64GS_CRT:
      case WARPSPEED_CRT:
      case DINAMIC_CRT:
      case SUPER_SNAPSHOT_5_CRT:
      case COMAL80_CRT:
      case STRUCTURED_BASIC_CRT:
        save_regular_crt(cart_info[(unsigned char)cart_type].bank_size,
                         cart_info[(unsigned char)cart_type].banks,
                         cart_info[(unsigned char)cart_type].load_address,
                         0,
                         cart_info[(unsigned char)cart_type].game,
                         cart_info[(unsigned char)cart_type].exrom);
        break;
      case KCS_CRT:
        save_2_blocks_crt(0x2000, 0x2000, 0x8000, 0xa000, 0, 0);
        break;
      case SIMONS_BASIC_CRT:
        save_2_blocks_crt(0x2000, 0x2000, 0x8000, 0xa000, 1, 0);
        break;
      case OCEAN_CRT:
        switch (loadfile_size)
        {
          case SIZE_128KB:
            save_regular_crt(0x2000, 16, 0x8000, 0, 0, 0);
            break;
          case SIZE_256KB:
            save_ocean_256kb_crt();
            break;
          case SIZE_512KB:
            save_regular_crt(0x2000, 64, 0x8000, 0, 0, 0);
            break;
        }
        break;
      case EXPERT_CRT:
        save_regular_crt(0x2000, 4, 0x8000, 2, 0, 1);
        break;
      case FUN_PLAY_CRT:
        save_funplay_crt();
        break;
      case ZAXXON_CRT:
        save_zaxxon_crt();
        break;
      case MAGIC_DESK_CRT:
        switch (loadfile_size)
        {
          case SIZE_32KB:
            save_regular_crt(0x2000, 4, 0x8000, 0, 0, 1);
            break;
          case SIZE_64KB:
            save_regular_crt(0x2000, 8, 0x8000, 0, 0, 1);
            break;
          case SIZE_128KB:
            save_regular_crt(0x2000, 16, 0x8000, 0, 0, 1);
            break;
        }
        break;
      case ROSS_CRT:
        switch (loadfile_size)
        {
          case SIZE_16KB:
            save_regular_crt(0x4000, 1, 0x8000, 0, 1, 1);
            break;
          case SIZE_32KB:
            save_regular_crt(0x4000, 2, 0x8000, 0, 1, 1);
            break;
        }
        break;
      case DELA_EP64_CRT:
        save_delaep64_crt();
        break;
      case DELA_EP7x8_CRT:
        save_delaep7x8_crt();
        break;
      case DELA_EP256_CRT:
        save_delaep256_crt();
        break;
      case REX_EP256_CRT:
        save_rexep256_crt();
        break;
    }
  }
  return 0;
}
