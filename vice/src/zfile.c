/*
 * zfile.c - Transparent handling of compressed files.
 *
 * Written by
 *  Ettore Perazzoli (ettore@comm2000.it)
 *
 * BZIP v2 support added by
 *  Andreas Boose (boose@rzgw.rz.fh-hannover.de)
 *
 * ARCHIVE, ZIPCODE and LYNX supports added by
 *  Teemu Rantanen (tvr@cs.hut.fi)
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

/* This code might be improved a lot...  */

#include "vice.h"

#ifdef STDC_HEADERS
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <errno.h>
#endif

#include "utils.h"
#include "zfile.h"
#include "zipcode.h"

/* ------------------------------------------------------------------------- */

/* #define DEBUG_ZFILE */

#ifdef DEBUG_ZFILE
#define ZDEBUG(a) printf a
#else
#define ZDEBUG(a)
#endif

/* We could add more here...  */
enum compression_type {
    COMPR_NONE,
    COMPR_GZIP,
    COMPR_BZIP,
    COMPR_ARCHIVE,
    COMPR_ZIPCODE,
    COMPR_LYNX
};

/* This defines a linked list of all the compressed files that have been
   opened.  */
struct zfile {
    char *tmp_name;		/* Name of the temporary file.  */
    char *orig_name;		/* Name of the original file.  */
    int write_mode;		/* Non-zero if the file is open for writing.  */
    FILE *stream;		/* Associated stdio-style stream.  */
    int fd;			/* Associated file descriptor.  */
    enum compression_type type;	/* Compression algorithm.  */
    struct zfile *prev, *next;  /* Link to the previous and next nodes.  */
};
struct zfile *zfile_list = NULL;

/* ------------------------------------------------------------------------- */

static int zinit_done = 0;

static int zinit(void)
{
    struct zfile *p;

    /* Free the `zfile_list' if not empty.  */
    for (p = zfile_list; p != NULL; ) {
	struct zfile *next;

	free(p->orig_name);
	free(p->tmp_name);
	next = p->next;
	free(p);
	p = next;
    }

    zfile_list = NULL;
    zinit_done = 1;

    return 0;
}

/* Add one zfile to the list.  `orig_name' is automatically expanded to the
   complete path.  */
static void zfile_list_add(const char *tmp_name,
			   const char *orig_name,
			   enum compression_type type,
			   int write_mode,
			   FILE *stream, int fd)
{
    struct zfile *new_zfile = (struct zfile *)xmalloc(sizeof(struct zfile));

    /* Make sure we have the complete path of the file.  */

#ifdef __MSDOS__
    /* MS-DOS version.  */
    new_zfile->orig_name = _truename(orig_name, NULL);
    if (new_zfile->orig_name == NULL) {
	fprintf(stderr, "zfile_list_add: warning, illegal file name `%s'.\n",
		orig_name);
	new_zfile->orig_name = stralloc(orig_name);
    }
#else
    /* Unix version.  */
    if (*orig_name == '/') {
	new_zfile->orig_name = stralloc(orig_name);
    } else {
	static char *cwd;

	cwd = get_current_dir();
	new_zfile->orig_name = concat(cwd, "/", orig_name, NULL);
	free(cwd);
    }
#endif

    /* The new zfile becomes first on the list.  */
    new_zfile->tmp_name = stralloc(tmp_name);
    new_zfile->write_mode = write_mode;
    new_zfile->stream = stream;
    new_zfile->fd = fd;
    new_zfile->type = type;
    new_zfile->next = zfile_list;
    new_zfile->prev = NULL;
    if (zfile_list != NULL)
	zfile_list->prev = new_zfile;
    zfile_list = new_zfile;
}

/* ------------------------------------------------------------------------ */

/* Uncompression.  */

/* If `name' has a gzip-like extension, try to uncompress it into a temporary
   file using gzip.  If this succeeds, return the name of the temporary file;
   return NULL otherwise.  */
static char *try_uncompress_with_gzip(const char *name)
{
    static char tmp_name[L_tmpnam];
    int l = strlen(name);
    int exit_status;
    char *argv[4];

    /* Check whether the name sounds like a gzipped file by checking the
       extension.  The last case (3-character extensions whose last character
       is a `z' (or 'Z'), is the standard convention for the MS-DOS version
       of gzip.  */
    if ((l < 4 || strcasecmp(name + l - 3, ".gz") != 0)
	&& (l < 3 || strcasecmp(name + l - 2, ".z") != 0)
	&& (l < 4 || toupper(name[l - 1]) != 'Z' || name[l - 4] != '.'))
	return NULL;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("gzip");
    argv[1] = stralloc("-cd");
    argv[2] = stralloc(name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_with_gzip: spawning gzip -cd %s\n", name));
    tmpnam(tmp_name);
    exit_status = spawn("gzip", argv, tmp_name, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("try_uncompress_with_gzip: OK\n"));
	return tmp_name;
    } else {
	ZDEBUG(("try_uncompress_with_gzip: failed\n"));
	unlink(tmp_name);
	return NULL;
    }
}

/* If `name' has a bzip-like extension, try to uncompress it into a temporary
   file using bzip.  If this succeeds, return the name of the temporary file;
   return NULL otherwise.  */
static char *try_uncompress_with_bzip(const char *name)
{
    static char tmp_name[L_tmpnam];
    int l = strlen(name);
    int exit_status;
    char *argv[4];

    /* Check whether the name sounds like a bzipped file by checking the
       extension.  MSDOS and UNIX variants of bzip v2 use the extension
       '.bz2'.  bzip v1 is obsolete.  */
    if (l < 5 || strcasecmp(name + l - 4, ".bz2") != 0)
	return NULL;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("bzip2");
    argv[1] = stralloc("-cd");
    argv[2] = stralloc(name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_with_bzip: spawning bzip -cd %s\n", name));
    tmpnam(tmp_name);
    exit_status = spawn("bzip2", argv, tmp_name, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("try_uncompress_with_bzip: OK\n"));
	return tmp_name;
    } else {
	ZDEBUG(("try_uncompress_with_bzip: failed\n"));
	unlink(tmp_name);
	return NULL;
    }
}

/* is the name zipcode -name? */
static int is_zipcode_name(char *name)
{
    if (name[0] >= '1' && name[0] <= '4' && name[1] == '!')
	return 1;
    return 0;
}

/* Extensions we know about */
static char *extensions[] = {
    ".d64", ".x64", ".dsk", ".t64", ".p00", ".prg", ".lnx", NULL
};

static int is_valid_extension(char *end, int l, int nameoffset)
{
    int				i, len;
    /* zipcode testing is a special case */
    if (l > nameoffset + 2 && is_zipcode_name(end + nameoffset))
	return 1;
    /* others */
    for (i = 0; extensions[i]; i++)
    {
	len = strlen(extensions[i]);
	if (l < nameoffset + len)
	    continue;
	if (!strcasecmp(extensions[i], end + l - len))
	    return 1;
    }
    return 0;
}


/* If `name' has a correct extension, try to list its contents and search for
   the first file with a proper extension; if found, extract it.  If this
   succeeds, return the name of the temporary file; if the archive file is
   valid but `write_mode' is non-zero, return a zero-length string; in all
   the other cases, return NULL.  */
static char *try_uncompress_archive(const char *name, int write_mode,
				    char *program, char *listopts,
				    char *extractopts, char *extension,
				    char *search)
{
    static char tmp_name[L_tmpnam];
    int l = strlen(name), nameoffset, found = 0, len;
    int exit_status;
    char *argv[8];
    FILE *fd;
    char tmp[1024];

    /* Do we have correct extension?  */
    len = strlen(extension);
    if (l <= len || strcasecmp(name + l - len, extension) != 0)
	return NULL;

    /* First run listing and search for first recognizeable extension.  */
    argv[0] = stralloc(program);
    argv[1] = stralloc(listopts);
    argv[2] = stralloc(name);
    argv[3] = NULL;

    ZDEBUG(("try_uncompress_archive: spawning `%d %s %s'\n",
	    program, listopts, name));
    tmpnam(tmp_name);
    exit_status = spawn(program, argv, tmp_name, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    /* No luck?  */
    if (exit_status != 0) {
	ZDEBUG(("try_uncompress_archive: `%s %s' failed.\n", program,
		listopts));
	unlink(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: `%s %s' successful.\n", program,
	    listopts));

    fd = fopen(tmp_name, "r");
    if (!fd) {
	ZDEBUG(("try_uncompress_archive: cannot read `%s %s' output.\n",
		program, archive));
	unlink(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: searching for the first valid file.\n"));

    /* Search for `search' first (if any) to see the offset where
       filename begins, then search for first recognizeable file.  */
    nameoffset = search ? -1 : 0;
    len = search ? strlen(search) : 0;
    while (!feof(fd)) {
	fgets(tmp, 1024, fd);
	l = strlen(tmp);
	while (l > 0 && (tmp[l-1] == '\n' || tmp[l-1] == '\r'))
	    tmp[--l] = 0;
	if (nameoffset < 0 && l >= len &&
	    !strcasecmp(tmp + l - len, search) != 0) {
	    nameoffset = l - 4;
	}
	if (nameoffset >= 0 && is_valid_extension(tmp, l, nameoffset)) {
	    ZDEBUG(("try_uncompress_archive: found `%s'.\n",
		    tmp + nameoffset));
	    found = 1;
	    break;
	}
    }

    fclose(fd);
    unlink(tmp_name);
    if (!found) {
	ZDEBUG(("try_uncompress_archive: no valid file found.\n"));
	return NULL;
    }

    /* This would be a valid ZIP file, but we cannot handle ZIP files in
       write mode.  Return a null temporary file name to report this.  */
    if (write_mode) {
	ZDEBUG(("try_uncompress_archive: cannot open file in write mode.\n"));
	return "";
    }

    /* And then file inside zip.  If we have a zipcode extract all of them
       to the same file. */
    argv[0] = stralloc(program);
    argv[1] = stralloc(extractopts);
    argv[2] = stralloc(name);
    if (is_zipcode_name(tmp + nameoffset)) {
	argv[3] = stralloc(tmp + nameoffset);
	argv[4] = stralloc(tmp + nameoffset);
	argv[5] = stralloc(tmp + nameoffset);
	argv[6] = stralloc(tmp + nameoffset);
	argv[7] = NULL;
	argv[3][0] = '1';
	argv[4][0] = '2';
	argv[5][0] = '3';
	argv[6][0] = '4';
    } else {
	argv[3] = stralloc(tmp + nameoffset);
	argv[4] = NULL;
    }

    ZDEBUG(("try_uncompress_archive: spawning `%s %s %s %s'.\n",
	    program, extractopts, name, tmp + nameoffset));
    exit_status = spawn(program, argv, tmp_name, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);
    free(argv[4]);
    if (is_zipcode_name(tmp + nameoffset)) {
	free(argv[5]);
	free(argv[6]);
	free(argv[7]);
    }

    if (exit_status != 0) {
	ZDEBUG(("try_uncompress_archive: `%s %s' failed.",
		program, extractopts));
	unlink(tmp_name);
	return NULL;
    }

    ZDEBUG(("try_uncompress_archive: `%s %s' successful.", program,
	    archive));
    return tmp_name;
}

/* If this file looks like a zipcode, try to extract is using c1541. We have
   to figure this out by reading the contents of the file */
static char *try_uncompress_zipcode(const char *name, int write_mode)
{
    static char			 tmp_name[L_tmpnam];
    int				 fd, i, count, sector, sectors = 0;
    unsigned char		 tmp[256];
    char			*argv[5];
    int				 exit_status;

    /* can we read this file? */
    fd = open(name, O_RDONLY);
    if (fd < 0)
	return NULL;
    /* Read first track to see if this is zipcode */
    lseek(fd, 4, SEEK_SET);
    for (count = 1; count < 21; count++) {
	i = zipcode_read_sector(fd, 1, &sector, tmp);
	if (i || sector < 0 || sector > 20 || (sectors & (1 << sector))) {
	    close(fd);
	    return NULL;
	}
	sectors |= 1 << sector;
    }
    close(fd);

    /* it is a zipcode. We cannot support write_mode */
    if (write_mode)
	return "";

    /* format image first */
    tmpnam(tmp_name);
    argv[0] = stralloc("c1541");
    argv[1] = stralloc("-format");
    argv[2] = stralloc(tmp_name);
    argv[3] = stralloc("a,bc");
    argv[4] = NULL;

    exit_status = spawn("c1541", argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

    if (exit_status) {
	unlink(tmp_name);
	return NULL;
    }

    /* ok, now extract the zipcode */
    argv[0] = stralloc("c1541");
    argv[1] = stralloc("-zcreate");
    argv[2] = stralloc(tmp_name);
    argv[3] = stralloc(name);
    argv[4] = NULL;

    exit_status = spawn("c1541", argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

    if (exit_status) {
	unlink(tmp_name);
	return NULL;
    }
    /* everything ok */
    return tmp_name;
}

/* If the file looks like a lynx image, try to extract it using c1541. We have
   to figure this out by reading the contsnts of the file */
static char *try_uncompress_lynx(const char *name, int write_mode)
{
    static char			 tmp_name[L_tmpnam];
    int				 fd, i, count;
    unsigned char		 tmp[256];
    char			*argv[5];
    int				 exit_status;

    /* can we read this file? */
    fd = open(name, O_RDONLY);
    if (fd < 0)
	return NULL;
    /* is this lynx -image? */
    i = read(fd, tmp, 2);
    if (i != 2 || tmp[0] != 1 || tmp[1] != 8) {
	close(fd);
	return NULL;
    }
    count = 0;
    while (1) {
	i = read(fd, tmp, 1);
	if (i != 1) {
	    close(fd);
	    return NULL;
	}
	if (tmp[0])
	    count = 0;
	else
	    count++;
	if (count == 3)
	    break;
    }
    i = read(fd, tmp, 1);
    if (i != 1 || tmp[0] != 13) {
	close(fd);
	return NULL;
    }
    count = 0;
    while (1) {
	i = read(fd, &tmp[count], 1);
	if (i != 1 || count == 254) {
	    close(fd);
	    return NULL;
	}
	if (tmp[count++] == 13)
	    break;
    }
    tmp[count] = 0;
    if (!atoi(tmp)) {
	close(fd);
	return NULL;
    }
    /* XXX: this is not a full check, but perhaps enough? */

    close(fd);

    /* it is a lynx image. We cannot support write_mode */
    if (write_mode)
	return "";

    /* format image first */
    tmpnam(tmp_name);
    argv[0] = stralloc("c1541");
    argv[1] = stralloc("-format");
    argv[2] = stralloc(tmp_name);
    argv[3] = stralloc("a,bc");
    argv[4] = NULL;

    exit_status = spawn("c1541", argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

    if (exit_status) {
	unlink(tmp_name);
	return NULL;
    }

    /* ok, now create the image */
    argv[0] = stralloc("c1541");
    argv[1] = stralloc("-unlynx");
    argv[2] = stralloc(tmp_name);
    argv[3] = stralloc(name);
    argv[4] = NULL;

    exit_status = spawn("c1541", argv, NULL, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);
    free(argv[3]);

    if (exit_status) {
	unlink(tmp_name);
	return NULL;
    }
    /* everything ok */
    return tmp_name;
}

/* List of archives we understand.  */
static struct {
    char	*program;
    char	*listopts;
    char	*extractopts;
    char	*extension;
    char	*search;
} valid_archives[] = {
#ifndef __MSDOS__
    { "unzip",	"-l",	"-p",		".zip",		"Name" },
    { "lha",	"lv",	"pq",		".lzh",		NULL },
    { "lha",	"lv",	"pq",		".lha",		NULL },
    /* Hmmm.  Did non-gnu tar have a -O -option?  */
    { "gtar",	"-tf",	"-xOf",		".tar",		NULL },
    { "tar",	"-tf",	"-xOf",		".tar",		NULL },
    { "gtar",	"-ztf",	"-zxOf",	".tar.gz",	NULL },
    { "tar",	"-ztf",	"-zxOf",	".tar.gz",	NULL },
    { "gtar",	"-ztf",	"-zxOf",	".tgz",		NULL },
    { "tar",	"-ztf",	"-zxOf",	".tgz",		NULL },
    /* this might be overkill, but adding this was sooo easy...  */
    { "zoo",	"lf1q",	"xpq",		".zoo",		NULL },
#endif
    { NULL }
};


/* Try to uncompress file `name' using the algorithms we now of.  If this is
   not possible, return `COMPR_NONE'.  Otherwise, uncompress the file into a
   temporary file, return the type of algorithm used and the name of the
   temporary file in `tmp_name'.  If `write_mode' is non-zero and the
   returned `tmp_name' has zero length, then the file cannot be accessed in
   write mode.  */
static enum compression_type try_uncompress(const char *name, char **tmp_name,
					    int write_mode)
{
    int i;

    for (i = 0; valid_archives[i].program; i++) {
	if ((*tmp_name = try_uncompress_archive(name, write_mode,
						valid_archives[i].program,
						valid_archives[i].listopts,
						valid_archives[i].extractopts,
						valid_archives[i].extension,
						valid_archives[i].search))
	    != NULL) {
	    return COMPR_ARCHIVE;
	}
    }

    /* need this order or .tar.gz is misunderstood */
    if ((*tmp_name = try_uncompress_with_gzip(name)) != NULL)
	return COMPR_GZIP;

    if ((*tmp_name = try_uncompress_with_bzip(name)) != NULL)
	return COMPR_BZIP;

    if ((*tmp_name = try_uncompress_zipcode(name, write_mode)) != NULL)
	return COMPR_ZIPCODE;

    if ((*tmp_name = try_uncompress_lynx(name, write_mode)) != NULL)
	return COMPR_LYNX;

    return COMPR_NONE;
}

/* ------------------------------------------------------------------------- */

/* Compression.  */

/* Compress `src' into `dest' using gzip.  */
static int compress_with_gzip(const char *src, const char *dest)
{
    static char *argv[4];
    int exit_status;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("gzip");
    argv[1] = stralloc("-c");
    argv[2] = stralloc(src);
    argv[3] = NULL;

    ZDEBUG(("compress_with_gzip: spawning gzip -c %s\n", src));
    exit_status = spawn("gzip", argv, dest, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("compress_with_gzip: OK.\n"));
	return 0;
    } else {
	ZDEBUG(("compress_with_gzip: failed.\n"));
	return -1;
    }
}

/* Compress `src' into `dest' using bzip.  */
static int compress_with_bzip(const char *src, const char *dest)
{
    static char *argv[4];
    int exit_status;

    /* `exec*()' does not want these to be constant...  */
    argv[0] = stralloc("bzip2");
    argv[1] = stralloc("-c");
    argv[2] = stralloc(src);
    argv[3] = NULL;

    ZDEBUG(("compress_with_bzip: spawning bzip -c %s\n", src));
    exit_status = spawn("bzip2", argv, dest, NULL);

    free(argv[0]);
    free(argv[1]);
    free(argv[2]);

    if (exit_status == 0) {
	ZDEBUG(("compress_with_bzip: OK.\n"));
	return 0;
    } else {
	ZDEBUG(("compress_with_bzip: failed.\n"));
	return -1;
    }
}

/* Compress `src' into `dest' using algorithm `type'.  */
static int compress(const char *src, const char *dest,
		    enum compression_type type)
{
    char *dest_backup_name;
    int retval;

    /* This shouldn't happen */
    if (type == COMPR_ARCHIVE)
    {
	fprintf(stderr, "compress: trying to compress archive -file\n");
	return -1;
    }

    /* This shouldn't happen */
    if (type == COMPR_ZIPCODE)
    {
	fprintf(stderr, "compress: trying to compress zipcode -file\n");
	return -1;
    }

    /* This shouldn't happen */
    if (type == COMPR_LYNX)
    {
	fprintf(stderr, "compress: trying to compress lynx -file\n");
	return -1;
    }

    /* Check whether `compression_type' is a known one.  */
    if (type != COMPR_GZIP && type != COMPR_BZIP) {
        ZDEBUG(("compress: unknown compression type\n"));
	return -1;
    }

    /* If we have no write permissions for `dest', give up.  */
    if (access(dest, W_OK) < 0) {
        ZDEBUG(("compress: no write permissions for `%s'\n",
                dest));
	return -1;
    }

    if (access(dest, R_OK) < 0) {
        ZDEBUG(("compress: no read permissions for `%s'\n",
               dest));
	perror(dest);
	dest_backup_name = NULL;
    } else {
	/* If `dest' exists, make a backup first.  */
	dest_backup_name = make_backup_filename(dest);
	if (dest_backup_name != NULL)
	    ZDEBUG(("compress: making backup %s... ", dest_backup_name));
	if (dest_backup_name != NULL && rename(dest, dest_backup_name) < 0) {
	    ZDEBUG(("failed.\n"));
	    perror("rename");
	    fprintf(stderr, "Could not make pre-compression backup.\n");
	    return -1;
	} else {
	    ZDEBUG(("OK\n"));
	}
    }

    switch (type) {
      case COMPR_GZIP:
	retval = compress_with_gzip(src, dest);
	break;
      case COMPR_BZIP:
	retval = compress_with_bzip(src, dest);
	break;
      default:
	retval = -1;
    }

    if (retval == -1) {
	/* Compression failed: restore original file.  */
	if (dest_backup_name != NULL && rename(dest_backup_name, dest) < 0) {
	    perror("rename");
	    fprintf(stderr, "Could not restore backup file after failed compression.\n");
	}
    } else {
	/* Compression succeeded: remove backup file.  */
	if (dest_backup_name != NULL && unlink(dest_backup_name) < 0) {
	    perror("unlink");
	    fprintf(stderr, "Warning: could not remove backup file.\n");
	    /* Do not return an error anyway (no data is lost).  */
	}
    }

    if (dest_backup_name)
	free(dest_backup_name);
    return retval;
}

/* ------------------------------------------------------------------------ */

/* Here we have the actual open/fopen and close/fclose wrappers.

   These functions work exactly like the standard library versions, but
   handle compression and decompression automatically.  When a file is
   opened, we check whether it looks like a compressed file of some kind.
   If so, we uncompress it and then actually open the uncompressed version.
   When a file that was opened for writing is closed, we re-compress the
   uncompressed version and update the original file.  */

/* `open()' wrapper.  */
int zopen(const char *name, mode_t opt, int flags)
{
    char *tmp_name;
    int fd;
    enum compression_type type;
    int write_mode;

    if (!zinit_done)
	zinit();

    /* Do we want to write to this file?  */
    write_mode = opt & (O_RDWR | O_WRONLY);

    /* Check for write permissions.  */
    if (write_mode && access(name, W_OK) < 0)
	return -1;

    type = try_uncompress(name, &tmp_name, write_mode);
    if (type == COMPR_NONE)
	return open(name, opt, flags);
    else if (*tmp_name == '\0') {
	errno = EACCES;
	return -1;
    }

    /* (Arghl...  The following code is very nice, except that it cannot work
       backwards, and it also clobbers `type' causing even "plain"
       compression not to work correctly anymore.)  */

#if 0
    /* OK, we managed to decompress that. Let's see if we can do that again.
       If we can, we can delete the previous tmpfile */
    while (1) {
	type = try_uncompress(tmp_name, &tmp_name2, write_mode);
	if (type == COMPR_NONE)
	    break;
	if (*tmp_name == '\0') {
	    unlink(tmp_name);
	    errno = EACCES;
	    return -1;
	}
	unlink(tmp_name);
	tmp_name = tmp_name2;
    }
#endif

    /* Open the uncompressed version of the file.  */
    fd = open(tmp_name, opt, flags);
    if (fd < 0)
	return fd;

    zfile_list_add(tmp_name, name, type, write_mode, NULL, fd);
    return fd;
}

/* `fopen()' wrapper.  */
FILE *zfopen(const char *name, const char *mode)
{
    char *tmp_name;
    FILE *stream;
    enum compression_type type;
    int write_mode;

    if (!zinit_done)
	zinit();

    /* Do we want to write to this file?  */
    write_mode = (strchr(mode, 'w') != NULL);

    /* Check for write permissions.  */
    if (write_mode && access(name, W_OK) < 0)
	return NULL;

    type = try_uncompress(name, &tmp_name, write_mode);
    if (type == COMPR_NONE)
	return fopen(name, mode);
    else if (*tmp_name == '\0') {
	errno = EACCES;
	return NULL;
    }

    /* Open the uncompressed version of the file.  */
    stream = fopen(tmp_name, mode);
    if (stream == NULL)
	return NULL;

    zfile_list_add(tmp_name, name, type, write_mode, stream, -1);
    return stream;
}

/* Handle close of a compressed file.  `ptr' points to the zfile to close.  */
static int handle_close(struct zfile *ptr)
{
    ZDEBUG(("handle_close: closing `%s' (`%s'), write_mode = %d\n",
            ptr->tmp_name, ptr->orig_name, ptr->write_mode));

    /* Recompress into the original file.  */
    if (ptr->write_mode
	&& compress(ptr->tmp_name, ptr->orig_name, ptr->type))
	return -1;

    /* Remove temporary file.  */
    if (unlink(ptr->tmp_name) < 0)
	perror(ptr->tmp_name);

    /* Remove item from list.  */
    if (ptr->prev != NULL)
	ptr->prev->next = ptr->next;
    else
	zfile_list = ptr->next;
    free(ptr->orig_name);
    free(ptr->tmp_name);
    free(ptr);

    return 0;
}

/* `close()' wrapper.  */
int zclose(int fd)
{
    struct zfile *ptr;

    if (!zinit_done) {
        ZDEBUG(("zclose: closing without init!?\n"));
	errno = EBADF;
	return -1;
    }

    ZDEBUG(("zclose: searching for the matching file...\n"));

    /* Search for the matching file in the list.  */
    for (ptr = zfile_list; ptr != NULL; ptr = ptr->next) {
	if (ptr->fd == fd) {

	    ZDEBUG(("zclose: file found, closing.\n"));

	    /* Close temporary file.  */
	    if (close(fd) == -1) {
	        ZDEBUG(("zclose: cannot close temporary file: %s\n",
	                strerror(errno)));
		return -1;
	    }

	    if (handle_close(ptr) < 0) {
		errno = EBADF;
		return -1;
	    }

	    return 0;
	}
    }

    ZDEBUG(("zclose: file descriptor not in the list, closing normally.\n"));
    return close(fd);
}

/* `fclose()' wrapper.  */
int zfclose(FILE *stream)
{
    struct zfile *ptr;

    if (!zinit_done) {
	errno = EBADF;
	return -1;
    }

    /* Search for the matching file in the list.  */
    for (ptr = zfile_list; ptr != NULL; ptr = ptr->next) {
	if (ptr->stream == stream) {

	    /* Close temporary file.  */
	    if (fclose(stream) == -1)
		return -1;

	    if (handle_close(ptr) < 0) {
		errno = EBADF;
		return -1;
	    }

	    return 0;
	}
    }

    return fclose(stream);
}

/* Close all files.  */
int zclose_all(void)
{
    struct zfile *p = zfile_list, *pnext;
    int ret = 0;

    if (!zinit_done)
	return 0;

    while (p != NULL) {
	if (p->stream != NULL) {
	    if (fclose(p->stream) == -1)
		ret = -1;
	} else if (p->fd != -1) {
	    if (close(p->fd) == -1)
		ret = -1;
	} else {
	    fprintf(stderr, "Inconsistent zfile list!\n");
            if (p->orig_name != NULL)
                free(p->orig_name);
            if (p->tmp_name != NULL)
                free(p->tmp_name);
            pnext = p->next;
            free(p);
            p = pnext;
	    continue;
	}
	/* Recompress into the original file.  */
	if (p->write_mode && compress(p->tmp_name, p->orig_name, p->type))
	    return -1;
	if (unlink(p->tmp_name) < 0)
	    perror(p->tmp_name);
	free(p->orig_name);
	free(p->tmp_name);
	pnext = p->next;
	free(p);
	p = pnext;
    }
    return ret;
}
