/*
 * $Id: file.h,v 1.2 1995/11/07 16:51:00 jopi Exp $
 *
 * This file is part of Commodore 64 emulator.
 * See README for copyright notice
 *
 * This file contains parameters to make fopen() portable.
 *
 * Written by
 *   Jouko Valta (jopi@stekt.oulu.fi)
 *
 *
 * $Log: file.h,v $
 * Revision 1.2  1995/11/07 16:51:00  jopi
 * MSDOS fix
 *
 * Revision 1.1  1994/12/12  16:59:44  jopi
 * Initial revision
 *
 *
 *
 */

#ifndef X64_FILE_H
#define X64_FILE_H

/*
 * Binary read and write modes for fopen()
 */

#if (defined(GEMDOS) || defined(__MSDOS__))
#define  READ	"rb"
#define  WRITE	"wb"
#define  APPEND	"wb"
#else
#define  READ	"r"
#define  WRITE	"w"
#define  APPEND	"w+"
#endif


#endif  /* X64_FILE_H */
