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

int oldmain(int argc, char **argv)
{
    char name[80];

    cout << "Creating font file for '" << argv[2] << "'." << endl;

    sprintf(name, "vice-%s-uc.fnt", argv[2]);
    ofstream fout1(name, ios::binary);

    sprintf(name, "vice-%s-lc.fnt", argv[2]);
    ofstream fout2(name, ios::binary);

    sprintf(name, "chargen-%s-1.fnt", argv[2]);
    ofstream foutc1(name,   ios::binary);

    sprintf(name, "chargen-%s-2.fnt", argv[2]);
    ofstream foutc2(name,   ios::binary);

    ifstream fin(argv[1], ios::binary);
    ifstream finh("header.fnt", ios::binary);
    if (!fin) {
        cerr << "File 'chargen' not found." << endl;
        return -1;
    }
    if (!finh) {
        cerr << "File 'header.fnt' not found." << endl;
        return -1;
    }

    const int hlen = 27 * 64 + 30;
    char header[hlen];
    finh.read(header, 28);

    fout1.write(header, 28);
    fout2.write(header, 28);

    foutc1.write(header, 28);
    foutc2.write(header, 28);

    sprintf(name, "%s Upper Case", argv[2]);
    int len = strlen(name);
    memset(header, 0, 64);

    fout1.write(name, len);
    foutc1.write(name, len);
    fout1.write(header, 32 - len);
    foutc1.write(header, 32 - len);

    fout1.write(name, len);
    foutc1.write(name, len);
    fout1.write(header, 32 - len);
    foutc1.write(header, 32 - len);

    sprintf(name, "%s Lower Case", argv[2]);
    len = strlen(name);
    memset(header, 0, 64);

    fout2.write(name, len);
    foutc2.write(name, len);
    fout2.write(header, 32 - len);
    foutc2.write(header, 32 - len);

    fout2.write(name, len);
    foutc2.write(name, len);
    fout2.write(header, 32 - len);
    foutc2.write(header, 32 - len);

    finh.seekg(28 + 64);
    finh.read(header, hlen - (28 + 64));

    fout1.write(header, hlen - (28 + 64));
    fout2.write(header, hlen - (28 + 64));

    foutc1.write(header, hlen - (28 + 64));
    foutc2.write(header, hlen - (28 + 64));

    const int flen = 256 * 8;
    char font[flen];

    fin.read(font, flen);
    foutc1.write(font, flen);
    fin.read(font, flen);
    foutc2.write(font, flen);

    memset(header, 0, 32 * 8);     // 32 empty
    fout1.write(header, 32 * 8);
    fout2.write(header, 32 * 8);

    fin.seekg(32 * 8);             // 32,64 !-?
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);
    fout2.write(font, 32 * 8);

    fin.seekg(0);                // 32,96 A-Z
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);

    fin.seekg(flen + 0);
    fin.read(font, 32 * 8);
    fout2.write(font, 32 * 8);

    fin.seekg(64 * 8);             // 32,128 a-z
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);

    fin.seekg(flen + 64 * 8);
    fin.read(font, 32 * 8);
    fout2.write(font, 32 * 8);

    memset(header, 0, 32 * 8);
    fout1.write(header, 32 * 8);   // 32,160 empty
    fout2.write(header, 32 * 8);

    fin.seekg(96 * 8);             // 32,192 gfx1
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);

    fin.seekg(flen + 96 * 8);
    fin.read(font, 32 * 8);
    fout2.write(font, 32 * 8);

    fin.seekg(64 * 8);             // 32,224 a-z
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);

    fin.seekg(flen + 64 * 8);
    fin.read(font, 32 * 8);
    fout2.write(font, 32 * 8);

    fin.seekg(flen + 96 * 8);        // 32,256 gfx2
    fin.read(font, 32 * 8);
    fout1.write(font, 32 * 8);

    fin.seekg(96 * 8);
    fin.read(font, 32 * 8);
    fout2.write(font, 32 * 8);

    fout1 << "\xff\xff\xff\xff\x08\0\0\0" << flush;
    fout2 << "\xff\xff\xff\xff\x08\0\0\0" << flush;
    foutc1 << "\xff\xff\xff\xff\x08\0\0\0" << flush;
    foutc2 << "\xff\xff\xff\xff\x08\0\0\0" << flush;

    return 0;
}

int main(int argc, char **argv)
{
    char name[80];

    const int offset = atoi(argv[3]);

    cout << "Creating font files for '" << argv[2] << "'." << endl;

    sprintf(name, "chargen-%s.fnt", argv[2]);
    ofstream fout(name, ios::binary);

    ifstream fin(argv[1], ios::binary);
    if (!fin) {
        cerr << "File '" << argv[1] << "' not found." << endl;
        return -1;
    }

    ifstream finh("header.fnt", ios::binary);
    if (!finh) {
        cerr << "File 'header.fnt' not found." << endl;
        return -1;
    }

    const int hlen = 27 * 64 + 30;
    char header[hlen];
    finh.read(header, 28);
    fout.write(header, 28);

    sprintf(name, argv[2]);
    int len = strlen(name);
    memset(header, 0, 64);

    fout.write(name, len);
    fout.write(header, 32 - len);
    fout.write(name, len);
    fout.write(header, 32 - len);

    finh.seekg(28 + 64);
    finh.read (header, hlen - (28 + 64));
    fout.write(header, hlen - (28 + 64));

    const int flen = 256 * 8;
    char font[flen];

    if (strchr(name, '(')) {
        cout << "Using 2nd half of file..." << endl;
        fin.seekg(offset * flen + 4096);
    } else {
        fin.seekg(offset * flen);
    }
    fin.read(font, flen);

    fout.write(font, flen);
    fout << "\xff\xff\xff\xff\x08\0\0\0" << flush;
}
