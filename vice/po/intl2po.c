/*
 * intl2po - win32 intl resource and amiga intl.c to .po compatible
 *           format conversion helper program.
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
#include <stdlib.h>
#include <string.h>

static char *win32_rc_files[] = {
  "/src/arch/win32/res.rc",
  "/src/arch/win32/resacia.rc",
  "/src/arch/win32/resc128.rc",
  "/src/arch/win32/resc64.rc",
  "/src/arch/win32/resc64_256k.rc",
  "/src/arch/win32/rescbm2.rc",
  "/src/arch/win32/resdrivec128.rc",
  "/src/arch/win32/resdrivec64vic20.rc",
  "/src/arch/win32/resdrivepetcbm2.rc",
  "/src/arch/win32/resdriveplus4.rc",
  "/src/arch/win32/resgeoram.rc",
  "/src/arch/win32/reside64.rc",
  "/src/arch/win32/respet.rc",
  "/src/arch/win32/respetreu.rc",
  "/src/arch/win32/resplus256k.rc",
  "/src/arch/win32/resplus4.rc",
  "/src/arch/win32/resplus60k.rc",
  "/src/arch/win32/resramcart.rc",
  "/src/arch/win32/resreu.rc",
  "/src/arch/win32/resrs232user.rc",
  "/src/arch/win32/ressid.rc",
  "/src/arch/win32/restfe.rc",
  "/src/arch/win32/resvic20.rc",
  "/src/arch/win32/resvicii.rc",
  NULL};

static char *intl_files[] = {
  "/src/arch/amigaos/intl.c",
  NULL};

/* status definitions */
#define SCANNING		0
#define STRINGTABLE_BEGIN_SCAN	1
#define MENU_BEGIN_SCAN		2
#define DIALOG_BEGIN_SCAN	3
#define TEXT_CONVERSION	4

/* found definitions */
#define UNKNOWN			0
#define FOUND_STRINGTABLE	1
#define FOUND_MENU		2
#define FOUND_DIALOG		3
#define FOUND_CAPTION		4
#define FOUND_BEGIN		5
#define FOUND_END		6

static char line_buffer[512];

int getline(FILE *file)
{
  char c=0;
  int counter=0;

  while (c!='\n' && !feof(file) && counter<511)
  {
    c=fgetc(file);
    line_buffer[counter]=c;
    counter++;
  }
  line_buffer[counter]=0;

  if (!strncasecmp(line_buffer,"STRINGTABLE",11))
    return FOUND_STRINGTABLE;

  if (!strncasecmp(line_buffer,"IDR_",4))
    return FOUND_MENU;

  if (!strncasecmp(line_buffer,"IDD_",4))
    return FOUND_DIALOG;

  if (!strncasecmp(line_buffer,"CAPTION",7))
    return FOUND_CAPTION;

  if (!strncasecmp(line_buffer,"BEGIN",5))
    return FOUND_BEGIN;

  if (!strncasecmp(line_buffer,"END",3))
    return FOUND_END;

  return UNKNOWN;
}

void getline_simple(FILE *file)
{
  char c=0;
  int counter=0;

  while (c!='\n' && !feof(file) && counter<511)
  {
    c=fgetc(file);
    line_buffer[counter]=c;
    counter++;
  }
  line_buffer[counter]=0;
}

int check_quote(char *text)
{
  int i;

  if (text[0]=='/' && text[1]=='/')
    return 0;

  for (i=0;text[i]!=0;i++)
  {
    if (text[i]=='"')
    {
      if (text[i+1]!='"')
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
  }
  return 0;
}

int language_id(char *text)
{
  int i;

  for (i=0;(text[i]!=' ' && text[i]!='\n');i++)
  {
  }

  if (text[i]=='\n')
    return 1;

  if (!strncmp(text,"IDR",3))
  {
    if (strncmp(text+i+1,"MENU",4))
      return 1;
  }

  if (!strncmp(text,"IDD",3))
  {
    if (strncmp(text+i+1,"DIALOG",6))
      return 1;
  }

  if (text[i-3]=='_' && text[i-2]=='D' && text[i-1]=='E')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='F' && text[i-1]=='R')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='H' && text[i-1]=='U')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='I' && text[i-1]=='T')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='N' && text[i-1]=='L')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='P' && text[i-1]=='L')
    return 1;

  if (text[i-3]=='_' && text[i-2]=='S' && text[i-1]=='V')
    return 1;

  return 0;
}

void replace_string(char *text, FILE *file)
{
  int i,j;

  if (check_quote(text)==0)
  {
    fprintf(file,"%s",text);
  }
  else
  {
    for (i=0;text[i]!='"';i++)
    {
      fputc(text[i],file);
    }
    fputc('N',file);
    fputc('_',file);
    fputc('(',file);
    fputc(text[i],file);

    for (j=i+1;text[j]!='"';j++)
    {
      fputc(text[j],file);
    }
    fputc('"',file);
    fputc(')',file);

    for (i=j+1;text[i]!=0;i++)
    {
      fputc(text[i],file);
    }
  }
}

void wrong_location(char *text, FILE *infile, FILE *outfile, char *filename)
{
  printf("%s found at wrong location in %s\n",text,filename);
  fclose(infile);
  fclose(outfile);
}

int convert_rc(char *in_filename, char *out_filename)
{
  struct stat statbuf;
  FILE *infile, *outfile;
  int status=SCANNING;
  int found=UNKNOWN;
  int stringtable_found=0;

  if (stat(in_filename, &statbuf) < 0)
  {
    printf("cannot stat %s\n",in_filename);
    return 0;
  }

  if (statbuf.st_size==0)
  {
    printf("file %s is 0 bytes\n",in_filename);
    return 0;
  }

  infile=fopen(in_filename,"rb");
  if (infile==NULL)
  {
    printf("cannot open %s for reading\n",in_filename);
    return 0;
  }

  outfile=fopen(out_filename,"wb");
  if (outfile==NULL)
  {
    printf("cannot open %s for writing\n",out_filename);
    fclose(infile);
    return 0;
  }

  while (!feof(infile))
  {
    found=getline(infile);
    switch(found)
    {
      case FOUND_STRINGTABLE:
        if (stringtable_found==0)
        {
          if (status!=SCANNING)
          {
            wrong_location("STRINGTABLE",infile,outfile,in_filename);
            return 0;
          }
          status=STRINGTABLE_BEGIN_SCAN;
          fprintf(outfile,"%s",line_buffer);
        }
        else
        {
          fprintf(outfile,"%s",line_buffer);
          getline(infile);
          if (!strncmp(line_buffer,"LANGUAGE LANG_ENGLISH",21))
          {
            status=STRINGTABLE_BEGIN_SCAN;
          }
          fprintf(outfile,"%s",line_buffer);
        }
        break;
      case FOUND_MENU:
        if (status!=SCANNING)
        {
          wrong_location("MENU",infile,outfile,in_filename);
          return 0;
        }
        else
        {
          if (language_id(line_buffer)==0)
          {
            status=MENU_BEGIN_SCAN;
          }
          fprintf(outfile,"%s",line_buffer);
        }
        break;
      case FOUND_DIALOG:
        if (status!=SCANNING)
        {
          printf(line_buffer);
          wrong_location("DIALOG",infile,outfile,in_filename);
          return 0;
        }
        else
        {
          if (language_id(line_buffer)==0)
          {
            status=DIALOG_BEGIN_SCAN;
          }
          fprintf(outfile,"%s",line_buffer);
        }
        break;
      case FOUND_CAPTION:
        if (status==DIALOG_BEGIN_SCAN)
        {
          replace_string(line_buffer,outfile);
        }
        else
        {
          fprintf(outfile,"%s",line_buffer);
        }
        break;
      case FOUND_BEGIN:
        switch(status)
        {
          case STRINGTABLE_BEGIN_SCAN:
            stringtable_found=1;
            status=TEXT_CONVERSION;
            break;
          case MENU_BEGIN_SCAN:
          case DIALOG_BEGIN_SCAN:
            status=TEXT_CONVERSION;
            break;
        }
        fprintf(outfile,"%s",line_buffer);
        break;
      case FOUND_END:
        status=SCANNING;
        fprintf(outfile,"%s",line_buffer);
        break;
      default:
        switch(status)
        {
          case TEXT_CONVERSION:
            replace_string(line_buffer,outfile);
            break;
          case SCANNING:
            if (!feof(infile))
              fprintf(outfile,"%s",line_buffer);
            break;
        }
    }
  }
  fclose(infile);
  fclose(outfile);
  return 1;
}

void strip_comments(char *text)
{
  int lang_comment=0;
  int i;

  if (!strncmp(text,"/* en */",8))
    lang_comment=1;

  if (!strncmp(text,"/* de */",8))
    lang_comment=1;

  if (!strncmp(text,"/* fr */",8))
    lang_comment=1;

  if (!strncmp(text,"/* hu */",8))
    lang_comment=1;

  if (!strncmp(text,"/* it */",8))
    lang_comment=1;

  if (!strncmp(text,"/* nl */",8))
    lang_comment=1;

  if (!strncmp(text,"/* pl */",8))
    lang_comment=1;

  if (!strncmp(text,"/* sv */",8))
    lang_comment=1;

  if (lang_comment==1)
  {
    for (i=0;!(text[i]=='*' && text[i+1]=='/');i++)
    {
      text[i]=32;
    }
    text[i]=32;
    text[i+1]=32;
  }
}

int convert_intl(char *in_filename, char *out_filename)
{
  struct stat statbuf;
  FILE *infile, *outfile;

  if (stat(in_filename, &statbuf) < 0)
  {
    printf("cannot stat %s\n",in_filename);
    return 0;
  }

  if (statbuf.st_size==0)
  {
    printf("file %s is 0 bytes\n",in_filename);
    return 0;
  }

  infile=fopen(in_filename,"rb");
  if (infile==NULL)
  {
    printf("cannot open %s for reading\n",in_filename);
    return 0;
  }

  outfile=fopen(out_filename,"wb");
  if (outfile==NULL)
  {
    printf("cannot open %s for writing\n",out_filename);
    fclose(infile);
    return 0;
  }

  while (!feof(infile))
  {
    getline_simple(infile);
    if (!feof(infile))
    {
      if (!strncmp(line_buffer,"/* en */",8))
      {
        strip_comments(line_buffer);
        replace_string(line_buffer,outfile);
      }
      else
      {
        strip_comments(line_buffer);
        fprintf(outfile,"%s",line_buffer);
      }
    }
  }
  fclose(infile);
  fclose(outfile);
  return 1;
}

void win32_rc_to_po(char *path)
{
  int i;
  int result;
  char *in_filename=NULL;
  char *out_filename=NULL;

  for (i=0; win32_rc_files[i]!=NULL; i++)
  {
    in_filename=malloc(strlen(path)+strlen(win32_rc_files[i])+1);
    if (in_filename==NULL)
    {
      printf("memory allocation error\n");
      exit(1);
    }
    strcpy(in_filename,path);
    strcat(in_filename,win32_rc_files[i]);

    out_filename=malloc(strlen(path)+strlen(win32_rc_files[i])+6);
    if (out_filename==NULL)
    {
      printf("memory allocation error\n");
      free(in_filename);
      exit(1);
    }
    strcpy(out_filename,path);
    strcat(out_filename,win32_rc_files[i]);
    strcat(out_filename,".po.c");

    result=convert_rc(in_filename, out_filename);

    free(in_filename);
    in_filename=NULL;

    free(out_filename);
    out_filename=NULL;

    if (result==0)
      exit(1);
  }
}

void intl_to_po(char *path)
{
  int i;
  int result;
  char *in_filename=NULL;
  char *out_filename=NULL;

  for (i=0; intl_files[i]!=NULL; i++)
  {
    in_filename=malloc(strlen(path)+strlen(intl_files[i])+1);
    if (in_filename==NULL)
    {
      printf("memory allocation error\n");
      exit(1);
    }
    strcpy(in_filename,path);
    strcat(in_filename,intl_files[i]);

    out_filename=malloc(strlen(path)+strlen(intl_files[i])+6);
    if (out_filename==NULL)
    {
      printf("memory allocation error\n");
      free(in_filename);
      exit(1);
    }
    strcpy(out_filename,path);
    strcat(out_filename,intl_files[i]);
    strcat(out_filename,".po.c");

    result=convert_intl(in_filename, out_filename);

    free(in_filename);
    in_filename=NULL;

    free(out_filename);
    out_filename=NULL;

    if (result==0)
      exit(1);
  }
}

int main(int argc, char *argv[])
{
  win32_rc_to_po(argv[1]);
  intl_to_po(argv[1]);
  return 0;
}
