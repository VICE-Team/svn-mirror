/*
 * convertprototype.c - Helper program for the sco openserver/unixware package creation.
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
#include <sys/stat.h>

static void convertprototype(unsigned char *readbuffer, int filesize)
{
  int counter=0;

  while (counter<filesize)
  {
    while (!isspace(readbuffer[counter]) || !isdigit(readbuffer[counter+1]))
    {
      printf("%c",readbuffer[counter]);
      counter++;
    }
    printf("%c",readbuffer[counter]);
    printf("%c",readbuffer[counter+1]);
    printf("%c",readbuffer[counter+2]);
    printf("%c",readbuffer[counter+3]);
    counter+=4;
    printf(" bin bin\n");
    while (readbuffer[counter] != '\n')
      counter++;
    counter++;
  }
}

int main(int argc, char **argv)
{
  struct stat statbuf;
  FILE *infile;
  unsigned char *buffer=NULL;

  if (argc==2)
  {
    if (stat(argv[1], &statbuf)>=0)
    {
      if (statbuf.st_size>0)
      {
        buffer=(unsigned char*)malloc(statbuf.st_size);
        if (buffer!=NULL)
        {
          infile=fopen(argv[1],"rb");
          if (infile)
          {
            if (fread(buffer,1,statbuf.st_size,infile)==statbuf.st_size)
            {
              convertprototype(buffer,statbuf.st_size);
            }
            else
              fclose(infile);
          }
        }
      }
    }
  }
  return 0;
}
