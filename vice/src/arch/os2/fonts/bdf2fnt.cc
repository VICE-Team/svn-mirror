#include <iostream.h>
#include <fstream.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**************************************************************
 *
 *  To create a header file:
 *  create a font, with:
 *    - fixed size
 *    - width & height = 8
 *    - no empty characters
 *
 **************************************************************/

int CopyHeader(ofstream &fout)
{
    ifstream fin("header.fnt", ios::binary);

    if (!fin) {
        cout << "File 'header.fnt' not found." << endl;
        return 1;
    }

    // header contains 1758 bytes
    const int len = 27 * 64 + 30;
    char c[len];

    fin.read(c, len);
    fout.write(c, len);

    fout << flush;

    return 0;
}

int ReadBdf(char font[256][8])
{
    FILE *in = fopen("vice-cbm.bdf", "r");

    if (!in) {
        cout << "File 'vice-cbm.bdf' not found." << endl;
        return 1;
    }

    int enc = 0;
    int charn = 0;
    int line = 0;
    char str[1024];
    int h, dx, dy;

    while (!feof(in) && fscanf(in, "%s", str)) {
        if (!strcmp(str, "ENDFONT")) break;
        switch (enc) {
            case 0:
                if (!strcmp(str,"ENCODING")) {
                    enc = 1;
                }
                break;
            case 1:
                charn = atoi(str);
                cout << charn << " ";
                enc = 2;
                break;
            case 2:
                if (!strcmp(str,"BBX")) {
                    enc = 3;
                }
                break;
            case 3:
                enc = 4;
                break;
            case 4:
                h = atoi(str);
                enc = 5;
                break;
            case 5:
                dx = atoi(str);
                enc = 6;
                break;
            case 6:
                dy = atoi(str);
                enc = 7;
                break;
            case 7:
                if (!strcmp(str, "BITMAP")) {
                    enc = 8;
                }
                break;
            case 8:
                if (!strcmp(str,"ENDCHAR")) {
                    line = 0;
                    enc = 0;
                } else {
                    char *c;
                    font[charn][8 - h - dy + line++] = strtol(str, &c, 16) >> dx;
                }
                break;
        }
    }
    fclose(in);
    return 0;
}

int main()
{
    ofstream fout("vice-cbm.fnt", ios::binary);

    if (CopyHeader(fout)) {
        return 0;
    }

    char font[256][8];

    if (ReadBdf(font)) {
        return 0;
    }

    for (int i = 0; i < 256; i++) {
        for (int j = 0; j < 8; j++) {
            fout << font[i][j];
        }
    }

    fout << '\xff';
    fout << '\xff';
    fout << '\xff';
    fout << '\xff';
    fout << '\x08';
    fout << '\0';
    fout << '\0';
    fout << '\0' << flush;
}
