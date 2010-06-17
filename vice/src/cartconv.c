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

#if defined(WATCOM_COMPILE) && defined(HAVE_IO_H)
#include <io.h>
#endif

#include "cartridge.h"

static FILE *infile, *outfile;
static int load_address = 0;
static int loadfile_offset = 0;
static unsigned int loadfile_size = 0;
static char *output_filename = NULL;
static char *input_filename[33];
static char *cart_name = NULL;
static signed char cart_type = -1;
static char convert_to_bin = 0;
static char convert_to_prg = 0;
static char convert_to_ultimax = 0;
static unsigned char input_filenames = 0;
static char loadfile_is_crt = 0;
static char loadfile_is_ultimax = 0;
static unsigned char loadfile_cart_type = 0;
static unsigned char filebuffer[(1024 * 1024) + 2];
static unsigned char headerbuffer[0x40];
static unsigned char extra_buffer_32kb[0x8000];
static unsigned char chipbuffer[16];

static int load_input_file(char *filename);

typedef struct cart_s {
    unsigned char game;
    unsigned char exrom;
    unsigned int sizes;
    unsigned int bank_size;
    unsigned int load_address;
    unsigned char banks;   /* 0 means the amount of banks need to be taken from the load-size and bank-size */
    unsigned int data_type;
    char *name;
    char *opt;
    void (*save)(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char gameline, unsigned char exromline);
} cart_t;

/* some prototypes to save routines */
static void save_regular_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_2_blocks_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_generic_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6);
static void save_easyflash_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_ocean_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_funplay_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_zaxxon_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_stardos_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_delaep64_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_delaep256_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_delaep7x8_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);
static void save_rexep256_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char game, unsigned char exrom);

/* this table must be in correct order so it can be indexed by CRT ID */
/*
    game, exrom, sizes, bank size, load addr, num banks, data type, name, option, saver

    num banks == 0 - take number of banks from input file size
*/
static const cart_t cart_info[] = {

/*  {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Generic 8kb", NULL, NULL}, */
/*  {0, 0, CARTRIDGE_SIZE_16KB, 0x4000, 0x8000, 1, 0, "Generic 16kb", NULL, NULL}, */
/*  {0, 1, CARTRIDGE_SIZE_4KB | CARTRIDGE_SIZE_16KB, 0, 0, 1, 0, "Ultimax", NULL, NULL}, */

    {1, 0, CARTRIDGE_SIZE_4KB | CARTRIDGE_SIZE_8KB | CARTRIDGE_SIZE_16KB, 0, 0, 0, 0, "Generic Cartridge", NULL, save_generic_crt},
    {0, 0, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 0, "Action Replay", "ar1", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_16KB, 0x2000, 0, 2, 0, "KCS Power Cartridge", "kcs", save_2_blocks_crt},
    {1, 1, CARTRIDGE_SIZE_64KB, 0x4000, 0x8000, 4, 0, "Final Cartridge III", "fc3", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_16KB, 0x2000, 0, 2, 0, "Simons Basic", "simon", save_2_blocks_crt},
    {0, 0, CARTRIDGE_SIZE_128KB | CARTRIDGE_SIZE_256KB | CARTRIDGE_SIZE_512KB, 0x2000, 0, 0, 0, "Ocean", "ocean", save_ocean_crt},
    {1, 1, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 2, "Expert Cartridge", "expert", NULL},
    {0, 0, CARTRIDGE_SIZE_128KB, 0x2000, 0x8000, 16, 0, "Fun Play, Power Play", "fp", save_funplay_crt},
    {0, 0, CARTRIDGE_SIZE_64KB, 0x4000, 0x8000, 4, 0, "Super Games", "sg", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 0, "Atomic Power", "ap", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Epyx Fastload", "epyx", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_16KB, 0x4000, 0x8000, 1, 0, "Westermann Learning", "wl", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Rex Utility", "ru", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_16KB, 0x4000, 0x8000, 1, 0, "Final Cartridge I", "fc1", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_64KB | CARTRIDGE_SIZE_96KB | CARTRIDGE_SIZE_128KB, 0x2000, 0xe000, 0, 0, "Magic Formel", "mf", save_regular_crt}, /* FIXME: 64k (v1), 96k (v2) and 128k (full) bins exist */
    {0, 1, CARTRIDGE_SIZE_512KB, 0x2000, 0x8000, 64, 0, "C64GS, System 3", "gs", save_regular_crt},
    {0, 1, CARTRIDGE_SIZE_16KB, 0x4000, 0x8000, 1, 0, "WarpSpeed", "ws", save_regular_crt},
    {0, 1, CARTRIDGE_SIZE_128KB, 0x2000, 0x8000, 16, 0, "Dinamic", "din", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_20KB, 0, 0, 3, 0, "Zaxxon", "zax", save_zaxxon_crt},
    {0, 1, CARTRIDGE_SIZE_32KB | CARTRIDGE_SIZE_64KB | CARTRIDGE_SIZE_128KB, 0x2000, 0x8000, 0, 0, "Magic Desk, Domark, Hes Australia", "md", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_64KB, 0x4000, 0x8000, 4, 0, "Super Snapshot 5", "ss5", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_64KB, 0x4000, 0x8000, 4, 0, "Comal-80", "comal", save_regular_crt},
    {0, 1, CARTRIDGE_SIZE_16KB, 0x2000, 0x8000, 2, 0, "Structured Basic", "sb", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_16KB | CARTRIDGE_SIZE_32KB, 0x4000, 0x8000, 0, 0, "Ross", "ross", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0, 0x8000, 0, 0, "Dela EP64", "dep64", save_delaep64_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 0, 0, "Dela EP7x8", "dep7x8", save_delaep7x8_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 0, 0, "Dela EP256", "dep256", save_delaep256_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0, 0x8000, 0, 0, "Rex EP256", "rep256", save_rexep256_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Mikro Assembler", "mikro", save_regular_crt},
    {1, 1, CARTRIDGE_SIZE_32KB, 0x8000, 0x0000, 1, 0, "Final Cartridge Plus", "fcp", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 0, "Action Replay 4", "ar4", save_regular_crt},
    {0, 1, CARTRIDGE_SIZE_16KB, 0x2000, 0, 4, 0, "StarDOS", "star", save_stardos_crt},
    {0, 1, CARTRIDGE_SIZE_1024KB, 0x2000, 0, 128, 0, "EasyFlash", "easy", save_easyflash_crt},
    {0, 0, 0, 0, 0, 0, 0, "EasyFlash xbank", NULL, NULL}, /* TODO ?? */
    {0, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Capture", "cap", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_16KB, 0x2000, 0x8000, 2, 0, "Action Replay 3", "ar3", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_64KB | CARTRIDGE_SIZE_128KB, 0x2000, 0x8000, 0, 0, "Retro Replay", "retro", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "MMC64", "mmc64", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_64KB | CARTRIDGE_SIZE_512KB, 0x2000, 0x8000, 0, 0, "MMC Replay", "mmcr", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_64KB, 0x2000, 0x8000, 8, 0, "IDE64", "ide64", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 0, "Super Snapshot 4", "ss4", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_4KB, 0x1000, 0x8000, 1, 0, "IEEE488", "ieee", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0xe000, 1, 0, "Game Killer", "gk", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_256KB, 0x2000, 0x8000, 32, 0, "Prophet 64", "p64", save_regular_crt},
    {0, 1, CARTRIDGE_SIZE_8KB, 0x2000, 0xe000, 1, 0, "Exos", "exos", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_8KB, 0x2000, 0x8000, 1, 0, "Freeze Frame", "ff", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_32KB, 0x2000, 0x8000, 4, 0, "Freeze Machine", "fm", save_regular_crt},
    {0, 0, CARTRIDGE_SIZE_4KB, 0x1000, 0xe000, 1, 0, "Snapshot 64", "s64", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_16KB, 0x2000, 0x8000, 2, 0, "Super Explode 5", "se5", save_regular_crt},
    {1, 0, CARTRIDGE_SIZE_16KB, 0x2000, 0x8000, 2, 0, "Magic Voice", "mv", save_regular_crt},
    {0, 0, 0, 0, 0, 0, 0, NULL, NULL, NULL}
};

#ifndef HAVE_STRDUP
char *strdup(const char *string)
{
    char *new;

    new = malloc(strlen(string) + 1);
    if (new != NULL) {
        strcpy(new, string);
    }
    return new;
}
#endif

#if !defined(HAVE_STRNCASECMP) && !defined(GP2X_SDL)
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

    for (; n != 0; --n) {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (charmap[u1] != charmap[u2]) {
            return charmap[u1]-charmap[u2];
        }

        if (u1 == '\0') {
            return 0;
        }
    }
    return 0;
}
#endif

static void cleanup(void)
{
    int i;

    if (output_filename != NULL) {
        free(output_filename);
    }
    if (cart_name != NULL) {
        free(cart_name);
    }
    for (i = 0; i < 33; i++) {
        if (input_filename[i] != NULL) {
            free(input_filename[i]);
        }
    }
}

/* FIXME: sort the list of options alphabetically before printing */
static void usage(void)
{
    int i = 1, n;
    cleanup();
    printf("convert:    cartconv [-t carttype] -i \"input name\" -o \"output name\" [-n \"cart name\"] [-l loadaddress]\n");
    printf("print info: cartconv -f \"input name\"\n");
    printf("\ncarttypes:\n");

    printf("bin      Binary .bin file (Default crt->bin)\n");
    printf("normal   Generic 8kb/16kb .crt file (Default bin->crt)\n");
    printf("prg      Binary C64 .prg file with load-address\n");
    printf("ulti     Ultimax mode 4kb/16kb .crt file\n\n");

    while (cart_info[i].name) {
        if (cart_info[i].opt) {
            n = (((i == CARTRIDGE_DELA_EP7x8) || (i == CARTRIDGE_DELA_EP64) || (i == CARTRIDGE_REX_EP256) || (i == CARTRIDGE_DELA_EP256)));
            printf("%-8s %s .crt file%s\n", cart_info[i].opt, cart_info[i].name, n ? ", extra files can be inserted" : "");
        }
        i++;
    }
    exit(1);
}

static void printinfo(char *name)
{
    int crtid;
    char *idname;
    load_input_file(name);
    crtid = headerbuffer[0x17] + (headerbuffer[0x16] << 8);
    if (headerbuffer[0x17] & 0x80) {
        /* handle our negative test IDs */
        crtid -= 0x10000;
    }
    if ((crtid >= 0) && (crtid <= CARTRIDGE_LAST)) {
        idname = cart_info[crtid].name;
    } else {
        idname = "unknown";
    }
    printf("Hardware ID:%5d %20s %s\n", crtid, idname, name);
    exit (0);
}

static void checkflag(char *flg, char *arg)
{
    int i;

    switch (tolower(flg[1])) {
        case 'f':
            printinfo(arg);
            break;
        case 'o':
            if (output_filename == NULL) {
                output_filename = strdup(arg);
            } else {
                usage();
            }
            break;
        case 'n':
            if (cart_name == NULL) {
                cart_name = strdup(arg);
            } else {
                usage();
            }
            break;
        case 'l':
            if (load_address == 0) {
                load_address = atoi(arg);
            } else {
                usage();
            }
            break;
        case 't':
            if (cart_type != -1 || convert_to_bin != 0 || convert_to_prg != 0 || convert_to_ultimax != 0) {
                usage();
            } else {
                for (i = 0; cart_info[i].name != NULL; i++) {
                    if (cart_info[i].opt != NULL) {
                        if (!strncasecmp(cart_info[i].opt, arg, strlen(cart_info[i].opt))) {
                            cart_type = i;
                            break;
                        }
                    }
                }
                if (cart_type == -1) {
                    if (!strcmp(arg, "bin")) {
                        convert_to_bin = 1;
                    } else if (!strcmp(arg, "normal")) {
                        cart_type = CARTRIDGE_CRT;
                    } else if (!strcmp(arg, "prg")) {
                        convert_to_prg = 1;
                    } else if (!strcmp(arg, "ulti")) {
                        cart_type = CARTRIDGE_CRT;
                        convert_to_ultimax = 1;
                    } else {
                        usage();
                    }
                }
            }
            break;
        case 'i':
            if (input_filenames == 33) {
                usage();
            }
            input_filename[input_filenames] = strdup(arg);
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


/* this loads the easyflash cart and puts it as the interleaved way into
   the buffer for easy binary saving */
static int load_easyflash_crt(void)
{
    unsigned int load_position;

    memset(filebuffer, 0xff, 0x100000);
    while (1) {
        if (fread(chipbuffer, 1, 16, infile) != 16) {
            if (loadfile_size == 0) {
                return -1;
            } else {
                return 0;
            }
        }
        loadfile_size = 0x100000;
        if (chipbuffer[0] != 'C' || chipbuffer[1] != 'H' || chipbuffer[2] != 'I' || chipbuffer[3] != 'P') {
            return -1;
        }
        if (load_address == 0) {
            load_address = (chipbuffer[0xc] << 8) + chipbuffer[0xd];
        }
        load_position = (chipbuffer[0xb] * 0x4000) + ((chipbuffer[0xc] == 0x80) ? 0 : 0x2000);
        if (fread(filebuffer + load_position, 1, 0x2000, infile) != 0x2000) {
            return -1;
        }
    }
}

static int load_all_banks(void)
{
    unsigned int length;

    if (loadfile_cart_type == CARTRIDGE_EASYFLASH) {
        return load_easyflash_crt();
    }

    while (1) {
        if (fread(chipbuffer, 1, 16, infile) != 16) {
            if (loadfile_size == 0) {
                return -1;
            } else {
                return 0;
            }
        }
        if (chipbuffer[0] != 'C' || chipbuffer[1] != 'H' || chipbuffer[2] != 'I' || chipbuffer[3] != 'P') {
            return -1;
        }
        if (load_address == 0) {
            load_address = (chipbuffer[0xc] << 8) + chipbuffer[0xd];
        }
        length = (chipbuffer[4] << 24) + (chipbuffer[5] << 16) + (chipbuffer[6] << 8) + chipbuffer[7] - 16;
        if (fread(filebuffer + loadfile_size, 1, length, infile) != length) {
            return -1;
        }
        loadfile_size += length;
    }
}

static int save_binary_output_file(void)
{
    char address_buffer[2];

    outfile = fopen(output_filename, "wb");
    if (outfile == NULL) {
        printf("Error: Can't open output file %s\n", output_filename);
        return -1;
    }
    if (convert_to_prg == 1) {
        address_buffer[0] = load_address & 0xff;
        address_buffer[1] = load_address >> 8;
        if (fwrite(address_buffer, 1, 2, outfile) != 2) {
            printf("Error: Can't write to file %s\n", output_filename);
            fclose(outfile);
            return -1;
        }
    }
    if (fwrite(filebuffer, 1, loadfile_size, outfile) != loadfile_size) {
        printf("Error: Can't write to file %s\n", output_filename);
        fclose(outfile);
        return -1;
    }
    fclose(outfile);
    printf("Input file : %s\n", input_filename[0]);
    printf("Output file : %s\n", output_filename);
    printf("Conversion from %s .crt to binary format successful.\n", cart_info[loadfile_cart_type].name);
    return 0;
}

static int write_crt_header(unsigned char gameline, unsigned char exromline)
{
    unsigned char crt_header[0x40] = "C64 CARTRIDGE   ";
    int endofname = 0;
    int i;

    crt_header[0x10] = 0;
    crt_header[0x11] = 0;
    crt_header[0x12] = 0;
    crt_header[0x13] = 0x40;

    crt_header[0x14] = 1;
    crt_header[0x15] = 0;

    crt_header[0x16] = 0;
    crt_header[0x17] = cart_type;

    crt_header[0x18] = exromline;
    crt_header[0x19] = gameline;

    crt_header[0x1a] = 0;
    crt_header[0x1b] = 0;
    crt_header[0x1c] = 0;
    crt_header[0x1d] = 0;
    crt_header[0x1e] = 0;
    crt_header[0x1f] = 0;

    if (cart_name == NULL) {
        cart_name = strdup("VICE CART");
    }

    for (i = 0; i < 32; i++) {
        if (endofname == 1) {
            crt_header[0x20 + i] = 0;
        } else {
            if (cart_name[i] == 0) {
                endofname = 1;
            } else {
                crt_header[0x20 + i] = toupper(cart_name[i]);
            }
        }
    }

    outfile = fopen(output_filename, "wb");
    if (outfile == NULL) {
        printf("Error: Can't open output file %s\n", output_filename);
        return -1;
    }
    if (fwrite(crt_header, 1, 0x40, outfile) != 0x40) {
        printf("Error: Can't write crt header to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    return 0;
}

static int write_chip_package(unsigned int length, unsigned int bankint, unsigned int address, unsigned char type)
{
    unsigned char chip_header[0x10] = "CHIP";
    unsigned char bank = (unsigned char)bankint;

    /* make sure the above conversion did not remove significant bits */
    assert(bankint == bank);

    chip_header[4] = 0;
    chip_header[5] = 0;
    chip_header[6] = (unsigned char)((length + 0x10) >> 8);
    chip_header[7] = (unsigned char)((length + 0x10) & 0xff);

    chip_header[8] = 0;
    chip_header[9] = type;

    chip_header[0xa] = 0;
    chip_header[0xb] = bank;

    chip_header[0xc] = (unsigned char)(address >> 8);
    chip_header[0xd] = (unsigned char)(address & 0xff);

    chip_header[0xe] = (unsigned char)(length >> 8);
    chip_header[0xf] = (unsigned char)(length & 0xff);
    if (fwrite(chip_header, 1, 0x10, outfile) != 0x10) {
        printf("Error: Can't write chip header to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    if (fwrite(filebuffer + loadfile_offset, 1, length, outfile) != length) {
        printf("Error: Can't write data to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    loadfile_offset += length;
    return 0;
}

static void bin2crt_ok(void)
{
    printf("Input file : %s\n", input_filename[0]);
    printf("Output file : %s\n", output_filename);
    printf("Conversion from binary format to %s .crt successful.\n", cart_info[(unsigned char)cart_type].name);
}

static void save_regular_crt(unsigned int length, unsigned int banks, unsigned int address, unsigned int type, unsigned char game, unsigned char exrom)
{
    unsigned int i;
    unsigned int real_banks = banks;

    if (write_crt_header(game, exrom) < 0) {
        cleanup();
        exit(1);
    }

    if (real_banks == 0) {
        real_banks = loadfile_size / length;
    }

    for (i = 0; i < real_banks; i++) {
        if (write_chip_package(length, i, address, (unsigned char)type) < 0) {
            cleanup();
            exit(1);
        }
    }
    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_2_blocks_crt(unsigned int l1, unsigned int l2, unsigned int a1, unsigned int a2, unsigned char game, unsigned char exrom)
{

    if (write_crt_header(game, exrom) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, (a2 == 0xe000) ? 0xe000 : 0xa000, 0) < 0) {
        cleanup();
        exit(1);
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static int check_empty_easyflash(void)
{
    int i;

    for (i = 0; i < 0x2000; i++) {
        if (filebuffer[loadfile_offset + i] != 0xff) {
            return 0;
        }
    }
    return 1;
}

static void save_easyflash_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int i, j;

    if (write_crt_header(0, 0) < 0) {
        cleanup();
        exit(1);
    }

    for (i = 0; i < 64; i++) {
        for (j = 0; j < 2; j++) {
            if (check_empty_easyflash() == 1) {
                loadfile_offset += 0x2000;
            } else {
                if (write_chip_package(0x2000, i, (j == 0) ? 0x8000 : 0xa000, 0) < 0) {
                    cleanup();
                    exit(1);
                }
            }
        }
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_ocean_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int i;

    if (loadfile_size != CARTRIDGE_SIZE_256KB) {
        save_regular_crt(0x2000, 0, 0x8000, 0, 0, 0);
    } else {
        if (write_crt_header(0, 0) < 0) {
            cleanup();
            exit(1);
        }

        for (i = 0; i < 16; i++) {
            if (write_chip_package(0x2000, i, 0x8000, 0) < 0) {
                cleanup();
                exit(1);
            }
        }

        for (i = 0; i < 16; i++) {
            if (write_chip_package(0x2000, i + 16, 0xa000, 0) < 0) {
                cleanup();
                exit(1);
            }
        }

        fclose(outfile);
        bin2crt_ok();
        cleanup();
        exit(0);
    }
}

static void save_funplay_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int i=0;

    if (write_crt_header(0, 0) < 0) {
        cleanup();
        exit(1);
    }

    while (i != 0x41) {
        if (write_chip_package(0x2000, i, 0x8000, 0) < 0) {
            cleanup();
            exit(1);
        }
        i += 8;
        if (i == 0x40) {
            i = 1;
        }
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_zaxxon_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    if (write_crt_header(1, 1) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x1000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0xa000, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 1, 0xa000, 0) < 0) {
        cleanup();
        exit(1);
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_stardos_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    if (write_crt_header(1, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0xe000, 0) < 0) {
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
    loadfile_offset = 0;
    infile = fopen(filename, "rb");
    if (infile == NULL) {
        printf("Error: Can't open %s\n", filename);
        return -1;
    }
    if (fread(filebuffer, 1, 16, infile) != 16) {
        printf("Error: Can't read %s\n",filename);
        fclose(infile);
        return -1;
    }
    if (!strncmp("C64 CARTRIDGE   ", (char *)filebuffer, 16)) {
        loadfile_is_crt = 1;
        if (fread(headerbuffer + 0x10, 1, 0x30, infile) != 0x30) {
            printf("Error: Can't read the full header of %s\n", filename);
            fclose(infile);
            return -1;
        }
        if (headerbuffer[0x10] != 0 || headerbuffer[0x11] != 0 || headerbuffer[0x12] != 0 || headerbuffer[0x13] != 0x40) {
            printf("Error: Illegal header size in %s\n", filename);
            fclose(infile);
            return -1;
        }
        if (headerbuffer[0x18] == 1 && headerbuffer[0x19] == 0) {
            loadfile_is_ultimax = 1;
        } else {
            loadfile_is_ultimax = 0;
        }
        loadfile_cart_type = headerbuffer[0x17];
        loadfile_size = 0;
        if (load_all_banks() < 0) {
            printf("Error: Can't load all banks of %s\n", filename);
            fclose(infile);
            return -1;
        } else {
            fclose(infile);
            return 0;
        }
    } else {
        loadfile_is_crt = 0;
        loadfile_size = (unsigned int)fread(filebuffer + 0x10, 1, 0x100000 - 14, infile) + 0x10;
        switch (loadfile_size) {
            case 0x1000:
            case 0x2000:
            case 0x4000:
            case 0x5000:
            case 0x8000:
            case 0x10000:
            case 0x20000:
            case 0x40000:
            case 0x80000:
            case 0x100000:
                loadfile_offset = 0;
                fclose(infile);
                return 0;
                break;
            case 0x1002:
            case 0x2002:
            case 0x4002:
            case 0x5002:
            case 0x8002:
            case 0x10002:
            case 0x20002:
            case 0x40002:
            case 0x80002:
            case 0x100002:
                loadfile_size -= 2;
                loadfile_offset = 2;
                fclose(infile);
                return 0;
                break;
            case 0x8004:
                loadfile_size -= 4;
                loadfile_offset = 4;
                fclose(infile);
                return 0;
                break;
            default:
                printf("Error: Illegal file size of %s\n", filename);
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

static void save_delaep64_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int i;

    if (loadfile_size != CARTRIDGE_SIZE_8KB) {
        printf("Error: wrong size of Dela EP64 base file %s (%d)\n", input_filename[0], loadfile_size);
        cleanup();
        exit(1);
    }

    if (write_crt_header(1, 0) < 0) {
        cleanup();
        exit(1);
    }

    /* write base file */
    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (input_filenames > 1) {
        /* write user eproms */
        for (i = 0; i < input_filenames; i++) {
            if (load_input_file(input_filename[i]) < 0) {
                close_output_cleanup();
            }
            if (loadfile_is_crt == 1) {
                printf("Error: to be inserted file can only be a binary for Dela EP64\n");
                close_output_cleanup();
            }
            if (loadfile_size != CARTRIDGE_SIZE_32KB) {
                printf("Error: to be insterted file can only be 32KB in size for Dela EP64\n");
                close_output_cleanup();
            }
            if (write_chip_package(0x8000, i + 1, 0x8000, 0) < 0) {
                close_output_cleanup();
            }
        }
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_delaep256_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int i,j;
    unsigned int insert_size = 0;

    if (loadfile_size != CARTRIDGE_SIZE_8KB) {
        printf("Error: wrong size of Dela EP256 base file %s (%d)\n", input_filename[0], loadfile_size);
        cleanup();
        exit(1);
    }

    if (input_filenames == 1) {
        printf("Error: no files to insert into Dela EP256 .crt\n");
        cleanup();
        exit(1);
    }

    if (write_crt_header(1, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    for (i = 0; i < input_filenames - 1; i++) {
        if (load_input_file(input_filename[i + 1]) < 0) {
            close_output_cleanup();
        }

        if (loadfile_size != CARTRIDGE_SIZE_32KB && loadfile_size != CARTRIDGE_SIZE_8KB) {
            printf("Error: only 32KB binary files or 8KB bin/crt files can be inserted in Dela EP256\n");
            close_output_cleanup();
        }

        if (insert_size == 0) {
            insert_size = loadfile_size;
        }

        if (insert_size == CARTRIDGE_SIZE_32KB && input_filenames > 8) {
            printf("Error: a maximum of 8 32KB images can be inserted\n");
            close_output_cleanup();
        }

        if (insert_size != loadfile_size) {
            printf("Error: only one type of insertion is allowed at this time for Dela EP256\n");
            close_output_cleanup();
        }

        if (loadfile_is_crt == 1 && (loadfile_size != CARTRIDGE_SIZE_8KB || load_address != 0x8000 || loadfile_is_ultimax == 1)) {
            printf("Error: you can only insert generic 8KB .crt files for Dela EP256\n");
            close_output_cleanup();
        }

        if (insert_size == CARTRIDGE_SIZE_32KB) {
            for (j = 0; j < 4; j++) {
                if (write_chip_package(0x2000, (i * 4) + j + 1, 0x8000, 0) < 0) {
                    close_output_cleanup();
                }
            }
            printf("inserted %s in banks %d-%d of the Dela EP256 .crt\n", input_filename[i + 1], (i * 4) + 1, (i * 4) + 4);
        } else {
            if (write_chip_package(0x2000, i + 1, 0x8000, 0) < 0) {
                close_output_cleanup();
            }
            printf("inserted %s in bank %d of the Dela EP256 .crt\n", input_filename[i + 1], i + 1);
        }
    }

    fclose(outfile);
    bin2crt_ok();
    cleanup();
    exit(0);
}

static void save_delaep7x8_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int inserted_size = 0;
    int name_counter = 1;
    int chip_counter = 1;

    if (loadfile_size != CARTRIDGE_SIZE_8KB) {
        printf("Error: wrong size of Dela EP7x8 base file %s (%d)\n", input_filename[0], loadfile_size);
        cleanup();
        exit(1);
    }

    if (input_filenames == 1) {
        printf("Error: no files to insert into Dela EP7x8 .crt\n");
        cleanup();
        exit(1);
    }

    if (write_crt_header(1, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    while (name_counter != input_filenames) {
        if (load_input_file(input_filename[name_counter]) < 0) {
            close_output_cleanup();
        }

        if (loadfile_size == CARTRIDGE_SIZE_32KB) {
            if (loadfile_is_crt == 1) {
                printf("Error: (%s) only binary 32KB images can be inserted into a Dela EP7x8 .crt\n",
                       input_filename[name_counter]);
                close_output_cleanup();
            } else {
                if (inserted_size != 0) {
                    printf("Error: (%s) only the first inserted image can be a 32KB image for Dela EP7x8\n",
                           input_filename[name_counter]);
                    close_output_cleanup();
                } else {
                    if (write_chip_package(0x2000, chip_counter, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    if (write_chip_package(0x2000, chip_counter + 1, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    if (write_chip_package(0x2000, chip_counter + 2, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    if (write_chip_package(0x2000, chip_counter + 3, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    printf("inserted %s in banks %d-%d of the Dela EP7x8 .crt\n",
                           input_filename[name_counter], chip_counter, chip_counter + 3);
                    chip_counter += 4;
                    inserted_size += 0x8000;
                }
            }
        }

        if (loadfile_size == CARTRIDGE_SIZE_16KB) {
            if (loadfile_is_crt == 1 && (loadfile_cart_type != 0 || loadfile_is_ultimax == 1)) {
                printf("Error: (%s) only generic 16KB .crt images can be inserted into a Dela EP7x8 .crt\n",
                       input_filename[name_counter]);
                close_output_cleanup();
            } else {
                if (inserted_size >= 0xc000) {
                    printf("Error: (%s) no room to insert a 16KB binary file into the Dela EP7x8 .crt\n",
                           input_filename[name_counter]);
                    close_output_cleanup();
                } else {
                    if (write_chip_package(0x2000, chip_counter, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    if (write_chip_package(0x2000, chip_counter + 1, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    printf("inserted %s in banks %d and %d of the Dela EP7x8 .crt\n",
                           input_filename[name_counter], chip_counter, chip_counter + 1);
                    chip_counter += 2;
                    inserted_size += 0x4000;
                }
            }
        }

        if (loadfile_size == CARTRIDGE_SIZE_8KB) {
            if (loadfile_is_crt == 1 && (loadfile_cart_type != 0 || loadfile_is_ultimax == 1)) {
                printf("Error: (%s) only generic 8KB .crt images can be inserted into a Dela EP7x8 .crt\n",
                       input_filename[name_counter]);
                close_output_cleanup();
            } else {
                if (inserted_size >= 0xe000) {
                    printf("Error: (%s) no room to insert a 8KB binary file into the Dela EP7x8 .crt\n",
                           input_filename[name_counter]);
                    close_output_cleanup();
                } else {
                    if (write_chip_package(0x2000, chip_counter, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    printf("inserted %s in bank %d of the Dela EP7x8 .crt\n", input_filename[name_counter], chip_counter);
                    chip_counter++;
                    inserted_size += 0x2000;
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

static void save_rexep256_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    int eprom_size_for_8kb = 0;
    int images_of_8kb_started = 0;
    int name_counter = 1;
    int chip_counter = 1;
    int subchip_counter = 1;

    if (loadfile_size != CARTRIDGE_SIZE_8KB) {
        printf("Error: wrong size of Rex EP256 base file %s (%d)\n", input_filename[0], loadfile_size);
        cleanup();
        exit(1);
    }

    if (input_filenames == 1) {
        printf("Error: no files to insert into Rex EP256 .crt\n");
        cleanup();
        exit(1);
    }

    if (write_crt_header(1, 0) < 0) {
        cleanup();
        exit(1);
    }

    if (write_chip_package(0x2000, 0, 0x8000, 0) < 0) {
        cleanup();
        exit(1);
    }

    while (name_counter != input_filenames) {
        if (load_input_file(input_filename[name_counter]) < 0) {
            close_output_cleanup();
        }

        if (chip_counter > 8) {
            printf("Error: no more room for %s in the Rex EP256 .crt\n", input_filename[name_counter]);
        }

        if (loadfile_size == CARTRIDGE_SIZE_32KB) {
            if (loadfile_is_crt == 1) {
                printf("Error: (%s) only binary 32KB images can be inserted into a Rex EP256 .crt\n",
                       input_filename[name_counter]);
                close_output_cleanup();
            } else {
                if (images_of_8kb_started != 0) {
                    printf("Error: (%s) only the first inserted images can be a 32KB image for Rex EP256\n",
                           input_filename[name_counter]);
                    close_output_cleanup();
                } else {
                    if (write_chip_package(0x8000, chip_counter, 0x8000, 0) < 0) {
                        close_output_cleanup();
                    }
                    printf("inserted %s in bank %d as a 32KB eprom of the Rex EP256 .crt\n",
                           input_filename[name_counter], chip_counter);
                    chip_counter++;
                }
            }
        }

        if (loadfile_size == CARTRIDGE_SIZE_8KB) {
            if (loadfile_is_crt == 1 && (loadfile_cart_type != 0 || loadfile_is_ultimax == 1)) {
                printf("Error: (%s) only generic 8KB .crt images can be inserted into a Rex EP256 .crt\n",
                       input_filename[name_counter]);
                close_output_cleanup();
            } else {
                if (images_of_8kb_started == 0) {
                    images_of_8kb_started = 1;
                    if ((9 - chip_counter) * 4 < input_filenames - name_counter) {
                        printf("Error: no room for the amount of input files given\n");
                        close_output_cleanup();
                    }
                    eprom_size_for_8kb = 1;
                    if ((9 - chip_counter) * 2 < input_filenames - name_counter) {
                        eprom_size_for_8kb = 4;
                    }
                    if (9 - chip_counter < input_filenames - name_counter) {
                        eprom_size_for_8kb = 2;
                    }
                }

                if (eprom_size_for_8kb == 1) {
                    if (write_chip_package(0x2000, chip_counter, 0x8000, 0) < 0) {
                        close_output_cleanup();
                        printf("inserted %s as an 8KB eprom in bank %d of the Rex EP256 .crt\n",
                               input_filename[name_counter], chip_counter);
                        chip_counter++;
                    }

                    if (eprom_size_for_8kb == 4 && (subchip_counter == 4 || name_counter == input_filenames - 1)) {
                        memcpy(extra_buffer_32kb + ((subchip_counter - 1) * 0x2000), filebuffer + loadfile_offset, 0x2000);
                        memcpy(filebuffer, extra_buffer_32kb, 0x8000);
                        loadfile_offset = 0;
                        if (write_chip_package(0x8000, chip_counter, 0x8000, 0) < 0) {
                            close_output_cleanup();
                        }
                        if (subchip_counter == 1) {
                            printf("inserted %s as a 32KB eprom in bank %d of the Rex EP256 .crt\n",
                                   input_filename[name_counter], chip_counter);
                        } else {
                            printf(" and %s as a 32KB eprom in bank %d of the Rex EP256 .crt\n",
                                   input_filename[name_counter], chip_counter);
                        }
                        chip_counter++;
                        subchip_counter = 1;
                    }

                    if (eprom_size_for_8kb == 4 && (subchip_counter == 3 || subchip_counter == 2) && 
                        name_counter != input_filenames) {
                        memcpy(extra_buffer_32kb + ((subchip_counter - 1) * 0x2000), filebuffer + loadfile_offset, 0x2000);
                        printf(", %s",input_filename[name_counter]);
                        subchip_counter++;
                    }

                    if (eprom_size_for_8kb == 2) {
                        if (subchip_counter == 2 || name_counter == input_filenames - 1) {
                            memcpy(extra_buffer_32kb + ((subchip_counter - 1) * 0x2000),
                                   filebuffer + loadfile_offset, 0x2000);
                            memcpy(filebuffer, extra_buffer_32kb, 0x4000);
                            loadfile_offset = 0;
                            if (write_chip_package(0x4000, chip_counter, 0x8000, 0) < 0) {
                                close_output_cleanup();
                            }
                            if (subchip_counter == 1) {
                                printf("inserted %s as a 16KB eprom in bank %d of the Rex EP256 .crt\n",
                                       input_filename[name_counter], chip_counter);
                            } else {
                                printf(" and %s as a 16KB eprom in bank %d of the Rex EP256 .crt\n",
                                       input_filename[name_counter],chip_counter);
                            }
                            chip_counter++;
                            subchip_counter = 1;
                        } else {
                            memcpy(extra_buffer_32kb, filebuffer + loadfile_offset, 0x2000);
                            printf("inserted %s", input_filename[name_counter]);
                            subchip_counter++;
                        }
                    }

                    if (eprom_size_for_8kb == 4 && subchip_counter == 1 && name_counter != input_filenames) {
                        memcpy(extra_buffer_32kb, filebuffer + loadfile_offset, 0x2000);
                        printf("inserted %s", input_filename[name_counter]);
                        subchip_counter++;
                    }
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

static void save_generic_crt(unsigned int p1, unsigned int p2, unsigned int p3, unsigned int p4, unsigned char p5, unsigned char p6)
{
    if (convert_to_ultimax == 1) {
        switch (loadfile_size) {
            case CARTRIDGE_SIZE_4KB:
                save_regular_crt(0x1000, 1, 0xf000, 0, 0, 1);
                break;
            case CARTRIDGE_SIZE_8KB:
                save_regular_crt(0x2000, 1, 0xe000, 0, 0, 1);
                break;
            case CARTRIDGE_SIZE_16KB:
                save_2_blocks_crt(0x2000, 0x2000, 0x8000, 0xe000, 0, 1);
                break;
        }
    } else {
        switch (loadfile_size) {
            case CARTRIDGE_SIZE_4KB:
            case CARTRIDGE_SIZE_8KB:
                save_regular_crt(0x1000, 0, 0x8000, 0, 1, 0);
                break;
            case CARTRIDGE_SIZE_16KB:
                save_regular_crt(0x4000, 1, 0x8000, 0, 0, 0);
                break;
        }
    }
}

int main(int argc, char *argv[])
{
    int i;
    int arg_counter = 1;
    char *flag, *argument;

    if (argc == 1) {
        usage();
    }
    if (((argc >> 1) << 1) == argc) {
        usage();
    }

    for (i = 0; i < 33; i++) {
        input_filename[i] = NULL;
    }

    while (arg_counter != argc) {
        flag = argv[arg_counter];
        argument = argv[arg_counter + 1];
        if (flag[0] != '-') {
            usage();
        } else {
            checkflag(flag, argument);
        }
        arg_counter += 2;
    }
    if (output_filename == NULL) {
        printf("Error: no output filename\n");
        cleanup();
        exit(1);
    }
    if (input_filenames == 0) {
        printf("Error: no input filename\n");
        cleanup();
        exit(1);
    }
    if (load_input_file(input_filename[0]) < 0) {
        cleanup();
        exit(1);
    }
    if (input_filenames > 1 && cart_type != CARTRIDGE_DELA_EP64 && cart_type != CARTRIDGE_DELA_EP256 &&
        cart_type != CARTRIDGE_DELA_EP7x8 && cart_type != CARTRIDGE_REX_EP256 && loadfile_cart_type != CARTRIDGE_DELA_EP64 &&
        loadfile_cart_type != CARTRIDGE_DELA_EP256 && loadfile_cart_type != CARTRIDGE_DELA_EP7x8 &&
        loadfile_cart_type != CARTRIDGE_REX_EP256) {
        too_many_inputs();
    }
    if ((cart_type == CARTRIDGE_DELA_EP64 || loadfile_cart_type == CARTRIDGE_DELA_EP64) && input_filenames > 3) {
        too_many_inputs();
    }
    if ((cart_type == CARTRIDGE_DELA_EP7x8 || loadfile_cart_type == CARTRIDGE_DELA_EP7x8) && input_filenames > 8) {
        too_many_inputs();
    }
    if (loadfile_is_crt == 1) {
        if (cart_type == CARTRIDGE_DELA_EP64 || cart_type == CARTRIDGE_DELA_EP256 || cart_type == CARTRIDGE_DELA_EP7x8 ||
            cart_type == CARTRIDGE_REX_EP256) {
            cart_info[(unsigned char)cart_type].save(0, 0, 0, 0, 0, 0);
        } else {
            if (cart_type == -1) {
                if (save_binary_output_file() < 0) {
                    cleanup();
                    exit(1);
                }
            } else {
                printf("Error: File is already .crt format\n");
                cleanup();
                exit(1);
            }
        }
    } else {
        if (cart_type == -1) {
            printf("Error: File is already in binary format\n");
            cleanup();
            exit(1);
        }
        /* FIXME: the sizes are used in a bitfield, and also by their absolute values. this
                  check is doomed to fail because of that :)
        */
        if ((loadfile_size & cart_info[(unsigned char)cart_type].sizes) != loadfile_size) {
            printf("Error: Input file size (%d) doesn't match %s requirements\n",
                   loadfile_size, cart_info[(unsigned char)cart_type].name);
            cleanup();
            exit(1);
        }
        if (cart_info[(unsigned char)cart_type].save != NULL) {
            cart_info[(unsigned char)cart_type].save(cart_info[(unsigned char)cart_type].bank_size,
            cart_info[(unsigned char)cart_type].banks,
            cart_info[(unsigned char)cart_type].load_address,
            cart_info[(unsigned char)cart_type].data_type,
            cart_info[(unsigned char)cart_type].game,
            cart_info[(unsigned char)cart_type].exrom);
        }
    }
    return 0;
}
