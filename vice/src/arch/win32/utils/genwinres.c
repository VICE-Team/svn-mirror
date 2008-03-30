#include <stdio.h>
#include <stdlib.h>

int idr_counter;
int idd_counter;
int idc_counter;
int ids_counter;
int idm_counter;
int idi_counter;
int general_counter;

int idr_start;
int idd_start;
int idc_start;
int ids_start;
int idm_start;
int idi_start;
int general_start;

unsigned char buffer[4096];
int buffer_size;
int buffer_readpointer;
unsigned char header[3];
char header_index;

void read_buffer(FILE *file)
{
    buffer_size = fread(buffer, 1, 4096, file);
    buffer_readpointer = 0;
}

#define GET_HEADER_FIRST_BYTE    0
#define GET_HEADER_SECOND_BYTE   1
#define GET_HEADER_THIRD_BYTE    2
#define FIND_LINE_END            3
#define SKIP_LINE                4
#define OUTPUT_LINE              5

void process_file(char *filename, FILE *output)
{
FILE *infile;
int state;
int res_value;

    infile = fopen(filename, "rb");
    if (infile) {
        state = GET_HEADER_FIRST_BYTE;
        read_buffer(infile);
        while (buffer_size) {
            buffer_readpointer = 0;
            while (buffer_readpointer < buffer_size) {
                switch (state) {
                    case GET_HEADER_FIRST_BYTE:
                        header[0] = buffer[buffer_readpointer];
                        if ((header[0] == 0x0a) || (header[1] == 0x0d)) {
                            // Empty line, find start of next line.
                            state = SKIP_LINE;
                        } else if ((header[0] == ' ') || (header[0] == 0x09)) {
                            // skip leading whitespace chars
                        } else {
                            state = GET_HEADER_SECOND_BYTE;
                        }
                        buffer_readpointer++;
                        break;
                    case GET_HEADER_SECOND_BYTE:
                        header[1] = buffer[buffer_readpointer];
                        if ((header[1] == 0x0a) || (header[1] == 0x0d)) {
                            //  Broken line skip it.
                            state = SKIP_LINE;
                        } else if ((header[0] == '/') && (header[1] == '/')) {
                            //  Comment, skip the line
                            state = FIND_LINE_END;
                        } else {
                            state = GET_HEADER_THIRD_BYTE;
                        }
                        buffer_readpointer++;
                        break;
                    case GET_HEADER_THIRD_BYTE:
                        header[2] = buffer[buffer_readpointer];
                        if ((header[2] == 0x0a) || (header[2] == 0x0d)) {
                            //  Broken line skip it.
                            state = SKIP_LINE;
                        } else {
                            if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'R')) {
                                res_value = idr_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'D')) {
                                res_value = idd_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'C')) {
                                res_value = idc_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'S')) {
                                res_value = ids_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'M')) {
                                res_value = idm_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else if ((header[0] == 'I') && (header[1] == 'D') && (header[2] == 'I')) {
                                res_value = idi_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            } else {
                                res_value = general_counter++;
                                fprintf(output, "#define ");
                                fwrite(header, 1, 3, output);
                                state = OUTPUT_LINE;
                            }
                        }
                        buffer_readpointer++;
                        break;
                    case FIND_LINE_END:
                        if ((buffer[buffer_readpointer] == 0x0a) || (buffer[buffer_readpointer] == 0x0d)) {
                            state = SKIP_LINE;
                        }
                        buffer_readpointer++;
                        break;
                    case SKIP_LINE:
                        if ((buffer[buffer_readpointer] != 0x0a) && (buffer[buffer_readpointer] != 0x0d)) {
                            state = GET_HEADER_FIRST_BYTE;
                        } else {
                            buffer_readpointer++;
                        }
                        break;
                    case OUTPUT_LINE:
                        if ((buffer[buffer_readpointer] == 0x0a) || (buffer[buffer_readpointer] == 0x0d)) {
                            fprintf(output, " %d\n", res_value);
                            state = GET_HEADER_FIRST_BYTE;
                        } else {
                            fwrite(&buffer[buffer_readpointer], 1, 1, output);
                        }
                        buffer_readpointer++;
                        break;
                }
            }
            read_buffer(infile);
        }
    }
}


void main(int argc, char **argv)
{
FILE *output;
int i;

    if (argc < 3) {
        printf("Usage: genwinres output-filename source-filename [source-filenames]\n");
        exit(0);
    }
    output = fopen(argv[1], "wt");
    if (output) {
        idr_start = idr_counter = 100;
        idd_start = idd_counter = 100;
        /* Command ID codes 0-11 and 32000 are used by the OS as well, to be safe we start at 100 */
        idc_start = idc_counter = 100;
        ids_start = ids_counter = 1;
        idm_start = idm_counter = 100;
        idi_start = idi_counter = 1;
        general_start = general_counter = 1;

        fprintf(output, "/*\n");
        fprintf(output, " * %s\n", argv[1]);
        fprintf(output, " *\n");
        fprintf(output, " * Autogenerated genwinres file, DO NOT EDIT !!!\n");
        fprintf(output, " *\n");
        fprintf(output, " * Written by\n");
        fprintf(output, " *  Andreas Boose <viceteam@t-online.de>\n");
        fprintf(output, " *  Ettore Perazzoli <ettore@comm2000.it>\n");
        fprintf(output, " *  Tibor Biczo <crown@t-online.hu>\n");
        fprintf(output, " *\n");
        fprintf(output, " * This file is part of VICE, the Versatile Commodore Emulator.\n");
        fprintf(output, " * See README for copyright notice.\n");
        fprintf(output, " *\n");
        fprintf(output, " *  This program is free software; you can redistribute it and/or modify\n");
        fprintf(output, " *  it under the terms of the GNU General Public License as published by\n");
        fprintf(output, " *  the Free Software Foundation; either version 2 of the License, or\n");
        fprintf(output, " *  (at your option) any later version.\n");
        fprintf(output, " *\n");
        fprintf(output, " *  This program is distributed in the hope that it will be useful,\n");
        fprintf(output, " *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
        fprintf(output, " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
        fprintf(output, " *  GNU General Public License for more details.\n");
        fprintf(output, " *\n");
        fprintf(output, " *  You should have received a copy of the GNU General Public License\n");
        fprintf(output, " *  along with this program; if not, write to the Free Software\n");
        fprintf(output, " *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA\n");
        fprintf(output, " *  02111-1307  USA.\n");
        fprintf(output, " *\n");
        fprintf(output, " */\n");
        fprintf(output, "\n");
        fprintf(output, "#ifndef _RES_H\n");
        fprintf(output, "#define _RES_H\n");
        fprintf(output, "\n");

        for (i = 2; i < argc; i++) {
            fprintf(output, "/*  Definitions from %s  */\n\n", argv[i]);
            process_file(argv[i], output);
            fprintf(output, "\n");
        }
        fprintf(output, "#define FIRST_IDR %d\n", idr_start);
        fprintf(output, "#define LAST_IDR %d\n", idr_counter);
        fprintf(output, "#define FIRST_IDD %d\n", idd_start);
        fprintf(output, "#define LAST_IDD %d\n", idd_counter);
        fprintf(output, "#define FIRST_IDC %d\n", idc_start);
        fprintf(output, "#define LAST_IDC %d\n", idc_counter);
        fprintf(output, "#define FIRST_IDS %d\n", ids_start);
        fprintf(output, "#define LAST_IDS %d\n", ids_counter);
        fprintf(output, "#define FIRST_IDM %d\n", idm_start);
        fprintf(output, "#define LAST_IDM %d\n", idm_counter);
        fprintf(output, "#define FIRST_IDI %d\n", idi_start);
        fprintf(output, "#define LAST_IDI %d\n", idi_counter);
        fprintf(output, "#define FIRST_GENERAL %d\n", general_start);
        fprintf(output, "#define LAST_GENERAL %d\n", general_counter);
        fprintf(output, "\n");
        fprintf(output, "#endif\n");

        fclose(output);
    }
}

