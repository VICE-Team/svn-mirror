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

void PrintChars(char *c, int max)
{
    for (int i = 0; i < max; i++) {
        for (int y = 0; y < 7; y++) {
            for (int x = 0; x < 6; x++) {
                int bitnr = i * 7 * 8 + y * 8 + x;
                int byte = bitnr / 8;
                int bit = 7 - bitnr % 8;

                cout << (c[byte] & (1 << bit) ? "*" : " ");
            }
            cout << endl;
        }
        cout << i << ": -------" << endl;
    }
}

int main(int argc, char **argv)
{
    char name[80];

    cout << "Creating chargen for MPFROMSHORT-803 font." << endl;

    ifstream fin("mps803.fnt", ios::binary);
    ofstream fout("chargen", ios::binary);

    const int hlen = 27 * 64 + 30;
    const int elen = 33 * 7;
    const int clen = 95 * 7;

    char empty[elen];
    char chars[clen];
    memset(empty, 0, elen);

    fout.write(empty, elen);

    fin.seekg(hlen+elen);
    fin.read(chars, clen);
    fout.write(chars, clen);

    PrintChars(chars, 95);

    fout.write(empty, elen);

    fin.seekg(hlen+elen + elen + clen - 7);
    fin.read(chars, 63 * 7);
    fout.write(chars, 63 * 7);

    PrintChars(chars, 63);

    cout << "###############" << endl;

    fout.write(empty, 7);

    fin.seekg(hlen + elen + elen + clen + 62 * 7);
    fin.read(chars, 31 * 7);
    fout.write(chars, 31 * 7);

    PrintChars(chars, 31);
}
