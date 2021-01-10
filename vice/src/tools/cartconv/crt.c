
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "cartridge.h"

extern unsigned char cart_subtype;
extern signed char cart_type;
extern char *cart_name;
extern FILE *outfile;
extern char *output_filename;
extern unsigned char filebuffer[CARTRIDGE_SIZE_MAX + 2];
extern int loadfile_offset;

int write_crt_header(unsigned char gameline, unsigned char exromline)
{
    unsigned char crt_header[0x40] = "C64 CARTRIDGE   ";
    int endofname = 0;
    int i;

    /* header length */
    crt_header[0x10] = 0;
    crt_header[0x11] = 0;
    crt_header[0x12] = 0;
    crt_header[0x13] = 0x40;

    crt_header[0x14] = 1;   /* crt version high */
    /* crt version low */
    if (cart_subtype > 0) {
        crt_header[0x15] = 1;   
    } else {
        crt_header[0x15] = 0;   
    }

    crt_header[0x16] = 0;   /* cart type high */
    crt_header[0x17] = (unsigned char)cart_type;

    crt_header[0x18] = exromline;
    crt_header[0x19] = gameline;

    crt_header[0x1a] = cart_subtype;
    
    /* unused/reserved */
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
                crt_header[0x20 + i] = (unsigned char)toupper((int)cart_name[i]);
            }
        }
    }

    outfile = fopen(output_filename, "wb");
    if (outfile == NULL) {
        fprintf(stderr, "Error: Can't open output file %s\n", output_filename);
        return -1;
    }
    if (fwrite(crt_header, 1, 0x40, outfile) != 0x40) {
        fprintf(stderr, "Error: Can't write crt header to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    return 0;
}

int write_chip_package(unsigned int length, unsigned int bank, unsigned int address, unsigned char type)
{
    unsigned char chip_header[0x10] = "CHIP";

    chip_header[4] = 0;
    chip_header[5] = 0;
    chip_header[6] = (unsigned char)((length + 0x10) >> 8);
    chip_header[7] = (unsigned char)((length + 0x10) & 0xff);

    chip_header[8] = 0;
    chip_header[9] = type;

    chip_header[0xa] = (unsigned char)(bank >> 8);
    chip_header[0xb] = (unsigned char)(bank & 0xff);

    chip_header[0xc] = (unsigned char)(address >> 8);
    chip_header[0xd] = (unsigned char)(address & 0xff);

    chip_header[0xe] = (unsigned char)(length >> 8);
    chip_header[0xf] = (unsigned char)(length & 0xff);
    if (fwrite(chip_header, 1, 0x10, outfile) != 0x10) {
        fprintf(stderr, "Error: Can't write chip header to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    if (fwrite(filebuffer + loadfile_offset, 1, length, outfile) != length) {
        fprintf(stderr, "Error: Can't write data to file %s\n", output_filename);
        fclose(outfile);
        unlink(output_filename);
        return -1;
    }
    loadfile_offset += (int)length;
    return 0;
}
 
