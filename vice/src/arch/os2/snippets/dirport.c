#include "dirport.h"

#include <stdio.h>
#include <string.h>

void flog(char *n, long i) {
    FILE *f=fopen("output","a");
    fprintf(f,"%s %li\n",n,i);
    fclose(f);
}


DIR *opendir (char *path)
{
    APIRET ulrc;
    ULONG  cnt = 1;
    DIR  *dir;
    char *name;
    if (!(dir  = (DIR*) calloc(1,sizeof(DIR))))
       return NULL;
    if (  name = (char*)calloc(1,strlen(path)+3)) {
       strcat(strcpy(name, path),path[strlen(path)-1]=='\\'?"*":"\\*");
       dir->handle = HDIR_CREATE;
       ulrc = DosFindFirst (name, &(dir->handle), _A_ANY, &(dir->buffer),
                            sizeof(struct _FILEFINDBUF3), &cnt, FIL_STANDARD);
       flog(name ,ulrc);
       free(name);
    }
    if (!name || ulrc) {
       DosFindClose (dir->handle);
       free(dir);
       dir = NULL;
    }
    return dir;
}

struct dirent *readdir (DIR *dir)
{
  ULONG cnt = 1;
  return DosFindNext (dir->handle, &(dir->buffer), sizeof(struct _FILEFINDBUF), &cnt) ? NULL : &(dir->buffer);
}

int closedir (DIR *dir)
{
    APIRET ulrc = DosFindClose (dir->handle);
    flog("DosFindClose",ulrc);
    free(dir);
    return (int)ulrc;
}

