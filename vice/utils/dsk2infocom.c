/****************************************************************/
/*    "dsk2infocom" converts disk image of Infocom game         */
/*      for the Commodore 64 to a data file which can be        */
/*      used on every computer                                  */
/*                                                              */
/*    v1.0:   April 26, 1993                                    */
/*    v1.0a:  May 24, 1993                                      */
/*                                                              */
/*    Author: Paul David Doherty (h0142kdd@rz.hu-berlin.de)     */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0

#define MAXLENGTH 131072        /* longest possible V3 datafile */
/*
   C-64, using tracks 5-16 and 18-35 ->  30 * 17 * 256 = 130560
   Apple II, using tracks 4-35       ->  32 * 16 * 256 = 131072
*/

struct ids
{
  char *version;
  char type;
  unsigned short id[6];
} vers_ids[] =

{
  "A1", 1, 0xf5, 0x60, 0x20, 0xf7, 0x1f, 0x20,
  "A2", 1, 0x00, 0xd0, 0xf5, 0x60, 0x20, 0xf9,
/*
   "A1" and "A2" are version names I just made up...
*/
  "B", 2, 0x0f, 0xa8, 0xa6, 0x78, 0x20, 0xba,
  "C", 3, 0xff, 0xa2, 0x0f, 0x20, 0xc9, 0xff,
  "D", 3, 0xdd, 0x27, 0x09, 0x30, 0x99, 0xf0,
  "E", 3, 0xff, 0xa2, 0x00, 0xc9, 0x0a, 0x90,
  "F", 3, 0xa2, 0xef, 0xa0, 0x27, 0xa9, 0x01,
  "G", 3, 0x69, 0x6e, 0x75, 0x65, 0x2e, 0x0d,
  "H", 3, 0x45, 0x54, 0x55, 0x52, 0x4e, 0x5d,
  "Unknown", 0, 0, 0, 0, 0, 0, 0
};

struct specs
{
  char t4;                      /* track 4 used? */
/*
   tracks 5..16 are always used
*/
  char t17;                     /* track 17 used? */
  char t18;                     /* track 18 used? [starts at sec 2] */
/*
   tracks 19..35 are always used
*/
  char tr_used;                 /* 16 = 0..15, 17 = 0..16 */
} vers_specs[] =

{
  0, 0, 0, 0,                   /* 0 - don't leave it out! */
  TRUE, TRUE, TRUE, 16,         /* 1 */
  FALSE, FALSE, FALSE, 16,      /* 2 */
  FALSE, FALSE, TRUE, 17        /* 3 */
};

FILE *infile;
FILE *outfile;

char version;
unsigned short checksum;
unsigned short checkedsum = 0;
unsigned long bytecount = 0;
unsigned long filelength;

void gotrack (int);
void transtrack (void);
void usage (void);

/*******************************************************************/
/*  MAIN function                                                  */
/*******************************************************************/

void
main (int argc, char **argv)
{
  int i, j;
  char gv;
  char dummy[30];
  unsigned short act_id[6];

  if (--argc != 1)
    usage ();

  if ((infile = fopen (argv[argc], "rb")) == NULL)
    {
      printf ("Error: Can't read \"%s\"\n", argv[argc]);
      exit (5);
    }

  strcpy (dummy, argv[argc]);
  strcat (dummy, ".dat");

  if ((outfile = fopen (dummy, "wb")) == NULL)
    {
      printf ("Error: Can't create \"%s\"\n", dummy);
      exit (5);
    }

  fseek (infile, 0x2000, SEEK_SET);
  for (i = 0; i < 6; i++)
    act_id[i] = getc (infile);

  i = 0;

  do
    {
      gv = version = vers_ids[i].type;
      for (j = 0; j < 6; j++)
        {
          if (act_id[j] != vers_ids[i].id[j])
            version = 0;
        }
      i++;
    }
  while ((gv != 0) && (version == 0));

  if (version == 0)
    {
      printf ("%s interpreter\n", vers_ids[--i].version);
      exit (5);
    }

  printf ("C-64 Interpreter %s\n", vers_ids[--i].version);

  for (i = 4; i <= 35; i++)
    {
      if ((i == 4) && (!(vers_specs[version].t4)))
        continue;
      if ((i == 17) && (!(vers_specs[version].t17)))
        continue;
      if ((i == 18) && (!(vers_specs[version].t18)))
        continue;

      gotrack (i);
      if (i == 18)
        fseek (infile, 512, SEEK_CUR);

      transtrack ();
    }
}

/*******************************************************************/
/*  Function: gotrack                                              */
/*******************************************************************/

void
gotrack (int track)
{
  unsigned long pos;

/*
   track  1..17:  21 sectors
         18..24:  19
         25..30:  18
         31..35:  17
*/

  pos = (track - 1) * 21;
  pos = pos - ((track > 18) ? (track - 18) : 0) * 2;
  pos = pos - ((track > 25) ? (track - 25) : 0) * 1;
  pos = pos - ((track > 31) ? (track - 31) : 0) * 1;

  fseek (infile, pos * 256, SEEK_SET);
}

/*******************************************************************/
/*  Function: transtrack                                           */
/*******************************************************************/

void
transtrack (void)
{
  int gotten;
  int j, k;

  for (j = 0; j < (vers_specs[version].tr_used); j++)
    {
      for (k = 0; k < 256; k++)
        {
          gotten = getc (infile);
          putc (gotten, outfile);

          if (bytecount == 0x1a)
            filelength = gotten * 256;
          if (bytecount == 0x1b)
            {
              filelength = (filelength + gotten) * 2;
              if (filelength == 0)
                filelength = MAXLENGTH;
            }

          if (bytecount == 0x1c)
            checksum = gotten * 256;
          if (bytecount == 0x1d)
            checksum = checksum + gotten;

          bytecount++;

          if (bytecount > 0x40)
            {
              checkedsum = checkedsum + gotten;
              if (bytecount == filelength)
                {
                  printf ("Checksum %s\n", (checksum == checkedsum) ? "okay" : "incorrect!");
                  exit (0);
                }
            }
        }
    }
}

/*******************************************************************/
/*  Function: usage                                                */
/*******************************************************************/

void
usage (void)
{
  printf ("\nDsk To Infocom V1.0\n");
  printf ("\n");
  printf ("Usage: Dsk2Infocom Filename.\n\n");
  exit (0);
}

