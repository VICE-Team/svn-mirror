/*
 * geninfocontrib - win32 infocontrib.h generation helper program.
 *
 * Written by
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char line_buffer[512];
static char text[65536];

static char *name[256];
static char *emailname[256];

int get_line(FILE *file)
{
    char c = 0;
    int counter = 0;

    while (c != '\n' && !feof(file) && counter < 511) {
        c = fgetc(file);
        if (c != 0xd) {
            line_buffer[counter++] = c;
        }
    }
    if (counter > 1) {
        line_buffer[counter - 1] = 0;
    }

    return counter - 1;
}

static void read_sed_file(FILE *sedfile)
{
    int amount = 0;
    int counter = 0;
    int foundend = 0;
    int buffersize = 0;

    buffersize = fread(text, 1, 65536, sedfile);
    text[buffersize] = 0;

    while (foundend == 0) {
        if (text[counter] == 0) {
            foundend = 1;
        } else {
            if (text[counter] == 's' && text[counter + 1] == '/' && text[counter + 2] == '@' && text[counter + 3] == 'b') {
                while (text[counter++] != '{') {}
                name[amount] = text + counter;
                while (text[counter++] != '}') {}
                text[counter - 1] = 0;
                counter++;
                emailname[amount++] = text + counter;
                while (text[counter++] != '/') {}
                text[counter - 1] = 0;
            }
            while (text[counter++] != '\n') {}
        }
    }
    name[amount] = NULL;
    emailname[amount] = NULL;
}

static int checklineignore(void)
{
    if (!strncmp(line_buffer, "@c", 2)) {
        return 1;
    }
    if (!strncmp(line_buffer, "@itemize @bullet", 16)) {
        return 1;
    }
    if (!strncmp(line_buffer, "@item", 5)) {
        return 1;
    }
    if (!strncmp(line_buffer, "@end itemize", 12)) {
        return 1;
    }
    return 0;
}

static void replacetags(void)
{
#ifdef _MSC_VER
    char *temp = _strdup(line_buffer);
#else
    char *temp = strdup(line_buffer);
#endif
    int countersrc = 0;
    int counterdst = 0;
    int i, j, len;

    while (temp[countersrc] != 0) {
        if (temp[countersrc] == '@') {
            countersrc++;
            if (!strncmp(temp + countersrc, "b{", 2)) {
                countersrc += 2;
                for (i = 0; name[i] != NULL; i++) {
                    if (!strncmp(temp + countersrc, name[i], strlen(name[i]))) {
                        len = strlen(emailname[i]);
                        for (j = 0; j < len; j++) {
                            line_buffer[counterdst++] = emailname[i][j];
                        }
                    }
                }
                while (temp[countersrc++] != '}') {}
            } else if (!strncmp(temp + countersrc, "t{", 2)) {
                countersrc += 2;
                while (temp[countersrc] != '}') {
                    line_buffer[counterdst++] = temp[countersrc++];
                }
                countersrc++;
            } else if (!strncmp(temp + countersrc, "code{", 5)) {
                countersrc += 5;
                line_buffer[counterdst++] = '`';
                while (temp[countersrc] != '}') {
                    line_buffer[counterdst++] = temp[countersrc++];
                }
                countersrc++;
                line_buffer[counterdst++] = '\'';
            } else if (!strncmp(temp + countersrc, "dots{", 5)) {
                countersrc += 6;
                line_buffer[counterdst++] = '.';
                line_buffer[counterdst++] = '.';
                line_buffer[counterdst++] = '.';
                line_buffer[counterdst++] = '.';
            } else {
                countersrc += 11;
                line_buffer[counterdst++] = '(';
                line_buffer[counterdst++] = 'C';
                line_buffer[counterdst++] = ')';
            }
        } else {
            line_buffer[counterdst++] = temp[countersrc++];
        }
    }
    line_buffer[counterdst] = 0;
    free(temp);
}

static void strip_name_slashes(char *text)
{
    int i = 0;
    int j = 0;

    while (text[i] != 0) {
        if (text[i] == '\\') {
            i++;
        } else {
            text[j++] = text[i++];
        }
    }
    text[j] = 0;
}

static void strip_emailname_slashes(char *text)
{
    int i = 0;
    int j = 0;

    while (text[i] != 0) {
        if (text[i] == '\\' && text[i + 1] != '"') {
            i++;
        } else {
            text[j++] = text[i++];
        }
    }
    text[j] = 0;
}

static void replace_tokens(void)
{
    int i = 0;
    char *found = NULL;

    while (name[i] != NULL) {
        found = strstr(name[i], "\\");
        if (found != NULL) {
            strip_name_slashes(name[i]);
            strip_emailname_slashes(emailname[i]);
        }
        i++;
    }
}

static void generate_infocontrib(char *in_filename, char *out_filename, char *sed_filename)
{
    int found_start = 0;
    int found_end = 0;
    size_t line_size;
    FILE *infile, *outfile, *sedfile;

    infile = fopen(in_filename, "rb");
    if (infile == NULL) {
        printf("cannot open %s for reading\n", in_filename);
        return;
    }

    sedfile = fopen(sed_filename, "rb");
    if (sedfile == NULL) {
        printf("cannot open %s for reading\n", sed_filename);
        fclose(infile);
    }

    outfile = fopen(out_filename, "wb");
    if (outfile == NULL) {
        printf("cannot open %s for writing\n", out_filename);
        fclose(infile);
        fclose(sedfile);
        return;
    }

    read_sed_file(sedfile);
    replace_tokens();

    fprintf(outfile, "/*\n");
    fprintf(outfile, " * infocontrib.h - Text of contributors to VICE, as used in info.c\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " * Autogenerated by geninfocontrib_h.sh, DO NOT EDIT !!!\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " * Written by");
    fprintf(outfile, " *  Marco van den Heuvel <blackystardust68@yahoo.com>\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " * This file is part of VICE, the Versatile Commodore Emulator.\n");
    fprintf(outfile, " * See README for copyright notice.\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " *  This program is free software; you can redistribute it and/or modify\n");
    fprintf(outfile, " *  it under the terms of the GNU General Public License as published by\n");
    fprintf(outfile, " *  the Free Software Foundation; either version 2 of the License, or\n");
    fprintf(outfile, " *  (at your option) any later version.\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " *  This program is distributed in the hope that it will be useful,\n");
    fprintf(outfile, " *  but WITHOUT ANY WARRANTY; without even the implied warranty of\n");
    fprintf(outfile, " *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n");
    fprintf(outfile, " *  GNU General Public License for more details.\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " *  You should have received a copy of the GNU General Public License\n");
    fprintf(outfile, " *  along with this program; if not, write to the Free Software\n");
    fprintf(outfile, " *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA\n");
    fprintf(outfile, " *  02111-1307  USA.\n");
    fprintf(outfile, " *\n");
    fprintf(outfile, " */\n\n");
    fprintf(outfile, "#ifndef VICE_INFOCONTRIB_H\n");
    fprintf(outfile, "#define VICE_INFOCONTRIB_H\n\n");
#ifdef WINMIPS
    fprintf(outfile, "const char *info_contrib_text[] = {\n");
#else
    fprintf(outfile, "const char info_contrib_text[] =\n");
#endif

    while (found_start == 0) {
        line_size = get_line(infile);
        if (line_size >= strlen("@chapter Acknowledgments")) {
            if (!strncmp(line_buffer, "@chapter Acknowledgments", 24)) { 
                found_start = 1;
            }
        }
    }

    while (found_end == 0) {
        line_size = get_line(infile);
        if (line_size == 0) {
#ifdef WINMIPS
            fprintf(outfile, "\"\\n\",\n");
#else
            fprintf(outfile, "\"\\n\"\n");
#endif
        } else {
            if (!strncmp(line_buffer, "@node Copyright, Contacts, Acknowledgments, Top", 47)) {
                found_end = 1;
            } else {
                if (checklineignore() == 0) {
                    replacetags();
#ifdef WINMIPS
                    fprintf(outfile, "\"  %s\\n\",\n", line_buffer);
#else
                    fprintf(outfile, "\"  %s\\n\"\n", line_buffer);
#endif
                }
            }
        }
    }
#ifdef WINMIPS
    fprintf(outfile, "\"\\n\",\n};\n#endif\n");
#else
    fprintf(outfile, "\"\\n\";\n#endif\n");
#endif

    fclose(infile);
    fclose(sedfile);
    fclose(outfile);
}

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("too few arguments\n");
        exit(1);
    }

    generate_infocontrib(argv[1], argv[2], argv[3]);

    return 0;
}
