#ifndef _CONFIG_H_
#define _CONFIG_H_

#define UNSTABLE

#define HAVE_ALLOCA             1
#define RETSIGTYPE              void
#define HAVE_RESID              1
#define HAS_JOYSTICK            1
#define HAVE_MOUSE              1
#define HAVE_CATWEASELMKIII     1
#define HAVE_HARDSID            1

#define HAS_LONGLONG_INTEGER    1
#define HAS_UNLOCKRESOURCE      1
#define SIZEOF_UNSIGNED_INT     4
#define SIZEOF_UNSIGNED_LONG    4
#define SIZEOF_UNSIGNED_SHORT   2
#define HAVE_ATEXIT             1
#define HAVE_MEMMOVE            1
#define HAVE_STRERROR           1
#define HAVE_FCNTL_H            1
#define HAVE_LIMITS_H           1
#define HAVE_COMMCTRL_H         1
#define HAVE_SHLOBJ_H           1
#define HAVE_DIRECT_H           1
#define HAVE_DIRENT_H           1
#define HAVE_ERRNO_H            1
#define HAVE_IO_H               1
#define HAVE_PROCESS_H          1
#define HAVE_SYS_TYPES_H        1
#define HAVE_SYS_STAT_H         1
#define HAVE_SIGNAL_H           1
#define HAVE_WINIOCTL_H         1
#define HAVE_GUIDLIB            1
#define DWORD_IS_LONG           1
#define HAVE_TFE                1
#define HAVE_FFMPEG             1

#define __i386__                1

#define inline                  _inline

#define _ANONYMOUS_UNION

#define S_ISDIR(m)              ((m) & _S_IFDIR)

#define MSVC_RC                 1

#define strcasecmp(s1, s2)      _stricmp(s1, s2)

#endif
