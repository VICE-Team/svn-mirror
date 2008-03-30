#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int CopyHeader(FILE *out)
{
    FILE *in  = fopen("header.fnt", "rb");
    if (!in)
    {
        printf("File 'header.fnt' not found.\n");
        return 1;
    }

    // header contains 1758 bytes
    while (!feof(in))
    {
        char c = fgetc(in);
        if (!feof(in)) fputc(c, out);
    }

    fclose(in);

    return 0;
}

int ReadBdf(char font[255][8])
{
    FILE *in = fopen("vice-cbm.bdf", "r");

    if (!in)
    {
        printf("File 'vice-cbm.bdf' not found.\n");
        return 1;
    }

    int enc=0;
    int charn=0;
    int line=0;
    char str[1024];
    int h, dx, dy;
    while (!feof(in) && fscanf(in, "%s", str))
    {
        if (!strcmp(str, "ENDFONT")) break;
        switch (enc)
        {
        case 0:
            if (!strcmp(str,"ENCODING")) enc=1;
            break;
        case 1:
            charn=atoi(str);
            printf("%i ", charn);
            enc = 2;
            break;
        case 2:
            if (!strcmp(str,"BBX")) enc=3;
            break;
        case 3:
            enc=4;
            break;
        case 4:
            h = atoi(str);
            enc=5;
            break;
        case 5:
            dx = atoi(str);
            enc=6;
            break;
        case 6:
            dy = atoi(str);
            enc=7;
            break;
        case 7:
            if (!strcmp(str,"BITMAP")) enc=8;
            break;
        case 8:
            if (!strcmp(str,"ENDCHAR"))
            {
                line=0;
                enc=0;
            }
            else
            {
                char *c;
                font[charn][8-h-dy+line++] = strtol(str, &c, 16)>>dx;
            }
            break;
        }
    }
    fclose(in);
    return 0;
}

int main()
{
    FILE *out = fopen("vice-cbm.fnt", "wb");

    if (CopyHeader(out))
        return 0;

    char font[255][8];

    if (ReadBdf(font))
        return 0;

    for (int i=0; i<256; i++)
        for (int j=0; j<8; j++)
            fputc(font[i][j], out);

    fputc('\xff', out);
    fputc('\xff', out);
    fputc('\xff', out);
    fputc('\xff', out);
    fputc('\x08', out);
    fputc('\0', out);
    fputc('\0', out);
    fputc('\0', out);

    fclose(out);
}
