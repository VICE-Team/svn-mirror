#ifndef _DIRPORT_H
#define _DIRPORT_H

#define INCL_DOS
#include <os2.h>
#include <malloc.h>

#define _A_VOLID   0
#define _A_NORMAL  FILE_NORMAL
#define _A_RDONLY  FILE_READONLY
#define _A_HIDDEN  FILE_HIDDEN
#define _A_SYSTEM  FILE_SYSTEM
#define _A_SUBDIR  FILE_DIRECTORY
#define _A_ARCH    FILE_ARCHIVED
#define _A_ANY     FILE_NORMAL | FILE_READONLY  | FILE_HIDDEN | \
                   FILE_SYSTEM | FILE_DIRECTORY | FILE_ARCHIVED

#define EPERM EDOM // Operation not permitted = Domain Error

#define dirent _FILEFINDBUF3
#define d_name achName     /* For struct dirent portability    */
#define d_size cbFile

#define mkdir(name, mode) mkdir(name)
//#define S_IFMT   0170000  /* Mask for file type */
#define S_ISDIR(mode)  ((mode) & S_IFDIR)

typedef struct _DIR {
    struct dirent buffer;
    HDIR handle;
    APIRET ulrc;
} DIR;

DIR *opendir (char *path);
struct dirent *readdir (DIR *dirp);
int closedir (DIR *dirp);

#endif /* DIRPORT__H */
