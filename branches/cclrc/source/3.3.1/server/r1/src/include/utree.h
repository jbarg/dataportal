
/*
 *	@(#) utree.h	(SDSC/NCSA) 1.5 95/05/26
 */

/*
 *
 * Tom Sherwin
 * The San Diego Supercomputer Center
 *
 * (C) Copyright 1994, 1995 The San Diego Supercomputer Center,
 *       a division of General Atomics, San Diego, California 92138.
 *
 * All rights reserved.
 *
 * This software is not subject to any license of the American Telephone
 * and Telegraph Company or the Regents of the University of California.
 *
 * Permission is granted to anyone to use this software for any purpose
 * on any computer system, and to alter it and redistribute it freely,
 * subject to the following restrictions:
 *
 * 1. Neither the authors nor The San Diego Supercomputer Center nor
 *    General Atomics are responsible for any consequences of the use
 *    of this software.
 *
 * 2. The origin of this software must not be misrepresented, either
 *    by explicit claim or by omission.  Credit to the authors, The San
 *    Diego Supercomputer Center and General Atomics must appear in
 *    documentation and sources.
 *
 * 3. Altered versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 4. This notice may not be removed or altered.
 */

/**********************************************************************
***********************************************************************
**                                                                   **
**  NOTICE:                                                          **
**                                                                   **
**  Significant portions of UniTree Release 1 software were          **
**  developed using private DISCOS funding, however, part of this    **
**  program resulted from work developed under a United States       **
**  Government contract and the following legend applies to the      **
**  government developed portions of the code.                       **
**                                                                   **
**  The Government retains for itself and others acting on its       **
**  behalf a paid-up, non-exclusive, irrevocable worldwide license   **
**  in such copyrighted data to reproduce, prepare derivative works, **
**  distribute copies to the public, and perform publicly and        **
**  display publicly, by or on behalf of the Government.             **
**                                                                   **
**  Neither the Government, nor any of its employees, makes any      **
**  warranty, expressed or implied, or assumes any legal liability   **
**  or responsibilty for the accuracy, completeness, or usefulness   **
**  of any information, apparatus, product, or process disclosed,    **
**  or represents that its use would not infringe privately owned    **
**  rights.                                                          **
**                                                                   **
***********************************************************************
**********************************************************************/

/*****************************************************************************
*
*                             Libtree
*                           Dec 4, 1991
*
* Libtree source code and documentation are in the public domain.
* Specifically, we give to the public domain all rights for future
* licensing of the source code, all resale rights, and all publishing
* rights.
*
* We ask, but do not require, that the following message be included in all
* derived works:
*
* Portions developed at the National Center for Supercomputing Applications at
* the University of Illinois at Urbana-Champaign.
*
* THE UNIVERSITY OF ILLINOIS GIVES NO WARRANTY, EXPRESSED OR IMPLIED, FOR THE
* SOFTWARE AND/OR DOCUMENTATION PROVIDED, INCLUDING, WITHOUT LIMITATION,
* WARRANTY OF MERCHANTABILITY AND WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE
*
*****************************************************************************
*
* Von Welch		vwelch@ncsa.uiuc.edu
* Undergraduate Research Assistant
* Networking Development Group, NCSA
*
******************************************************************************
*
* Extensions and integration with libnsl done by
*
* Tom Sherwin			Mass Storage Group, SDSC
* sherwint@sdsc.edu
*
*****************************************************************************/

/*
	utree.h


	This file contains macros changing standard unix file system calls
	to those directly accessing UniTree. Calls accessing UniTree are
	named the same as their unix equivalent but are preceded with a 'L'.

	Note that many of these calls return an equivalent type, but the
	structure may not be equivalent. For example fopen() returns a
	pointer, but not to a FILE structure. I guarantee that programs
	that do really clever or weird or kludgy things will crash and die.

	To change a file over to UniTree this file must be included and the
	main() function of the file must be renamed to prime().

	Not all functions are currently implemented. Look below to see
	if a specific function is. A couple are included in the library
	but do nothing. Those functions currently are:
		flock()
		fcntl()

	Also included are two routines UNIX_fopen(), and UNIX_open(). These
	always implement standard unix calls and should be used for
	opening devices and the like.


*/

#ifndef _UTREE_H
#define _UTREE_H

#include <stdio.h>
#include <sys/types.h>

/* dirent.h must also be included but it causes trouble if included twice */


/*
	Function Definitions and Declarations
*/

int Laccess();
#define access(path, amode)		Laccess(path, amode)

int Lchroot();
#define chroot(path)			Lchroot(path)

int Lchdir();
#define chdir(path)			Lchdir(path)

int Lchmod();
#define chmod(path, mode)		Lchmod(path, mode)

int Lchown();
#define chown(path, user, grp)		Lchown(path, user, grp)
#define lchown(path, user, grp)		Lchown(path, user, grp)

int Lclose();
#define close(fd)			Lclose(fd)

void Lclosedir();
#define closedir(dirp)			Lclosedir(dirp)

int Lcreat();
#define creat(path, mode)		Lcreat(path, mode)

int Ldup();
#define dup(oldd)			Ldup(oldd)

int Ldup2();
#define dup2(oldd, newd)		Ldup2(oldd, newd)

int Lfchmod();
#define fchmod(fd, mode)		Lfchmod(fd, mode)

int Lfchown();
#define fchown(fd, owner, group)	Lfchown(fd, owner, group)

int Lfclose();
#define fclose(file)			Lfclose(file)

/* fcntl() currently not implemented. Call simply returns 0 */
int Lfcntl();
#define fcntl(fd, cmd, arg)		Lfcntl(fd, cmd, arg)

/* flock() not currently implemented. Call does nothing. (returns 0) */
int Lflock();

FILE *Lfopen();
#define fopen(filename, type)		Lfopen(filename, type)

int Lfread();
#define fread(ptr, size, nitems, Cap)	Lfread(ptr, size, nitems, Cap)

int Lfstat();
#define fstat(fd, buf)			Lfstat(fd, buf)

/* Lftruncate() currently does nothing, just returns 0 */
int Lftruncate();
#define ftruncate(fd, len)		Lftruncate(fd, len)

int Lfwrite();
#define fwrite(ptr, size, nitems, Cap)	Lfwrite(ptr, size, nitems, Cap)

char *Lgetcwd();
#define getcwd(buf, size)		Lgetcwd(buf, size)

char *Lgetwd();
#define getwd(pathname)			Lgetwd(pathname)

/* lockf() not currently implemented. Call does nothing. (returns 0) */
int Llockf();
#define lockf(fd, cmd, size)		Llockf(fd, cmd, size)

off_t Llseek();
#define lseek(fd, offset, whence)	Llseek(fd, offset, whence)

int Lmkdir();
#define mkdir(dir, mode)		Lmkdir(dir, mode)

/* open() may legitimately have two or three arguments */
int Lopen();
#define	open				Lopen

/* An error here means you need to include <dirent.h> in your file */
DIR *Lopendir();
#define opendir(filename)		Lopendir(filename)

int Lread();
#define read(fd, buf, nbyte)		Lread(fd, buf, nbyte)

struct dirent *Lreaddir();
#define readdir(dirp)			Lreaddir(dirp)

int Lrename();
#define rename(from, to)		Lrename(from, to)

/*	Since rewindir usually seems to be a macro	*/
#ifdef rewinddir
#undef rewinddir
#endif

void Lrewinddir();
#define rewinddir(dirp)			Lseekdir(dirp, (int) 0)

int Lrmdir();
#define rmdir(path)			Lrmdir(path)

int Lscandir();
#define scandir(dir, list, sel, comp)	Lscandir(dir, list, sel, comp)

void Lseekdir();
#define seekdir(dirp, loc)		Lseekdir(dirp, loc)

int Lstat();
#define lstat(path, buf)		Lstat(path, buf)

/*
	We can't change stat because then it dies on every occurance
	of the word stat (as in 'struct stat').

#define stat(path, buf)			Lstat(path, buf)
*/

int Lsymlink();
#define symlink(name, new_name)		Lsymlink(name, new_name)

long Ltelldir();
#define telldir(dirp)			Ltelldir(dirp)

/* Ltruncate() currently does nothing, just returns 0 */
int Ltruncate();
#define truncate(path, len)		Ltruncate(path, len)

int Lunlink();
#define unlink(path)			Lunlink(path)

int Lwrite();
#define write(fd, buf, nbyte)		Lwrite(fd, buf, nbyte)

int Lfprintf();
#define fprintf				Lfprintf

char *Lfgets();
#define fgets( buffer, count, stream )	Lfgets( buffer, count, stream )

int Lfsync();
#define fsync( fd )			Lfsync( fd )


/* Calls directly to unix */
int UNIX_lstat();
int UNIX_open();

FILE *UNIX_fopen();

#endif	/* _UTREE_H */
