/* Portability cruft.  Include after config.h and sys/types.h.
   Copyright (C) 1996, 1998 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

#undef PARAMS
#if defined (__STDC__) && __STDC__
# ifndef _PTR_T
# define _PTR_T
  typedef void * ptr_t;
# endif
# define PARAMS(x) x
#else
# ifndef _PTR_T
# define _PTR_T
  typedef char * ptr_t;
# endif
# define PARAMS(x) ()
#endif

#define valloc system_valloc /* kludge */
#ifdef HAVE_UNISTD_H
# include <fcntl.h>
# include <unistd.h>
#else
# define O_RDONLY 0
int open(), read(), close();
#endif

/* Some operating systems treat text and binary files differently.  */
#if defined(O_BINARY) && O_BINARY
# include <io.h>
# ifdef __DJGPP__
#  define SET_BINARY(fd)  setmode(fd,O_BINARY)
# else
#  define SET_BINARY(fd)  _setmode(fd,O_BINARY)
# endif
static inline int undossify_input PARAMS((char *, size_t));
#else
# ifndef O_BINARY
#  define O_BINARY 0
#  define SET_BINARY(fd)   (void)0
# endif
#endif

#if STAT_MACROS_BROKEN
# undef S_ISDIR
#endif
#if !defined(S_ISDIR) && defined(S_IFDIR)
# define S_ISDIR(Mode) (((Mode) & S_IFMT) == S_IFDIR)
#endif

#ifdef STDC_HEADERS
# include <stdlib.h>
#else
ptr_t malloc(), realloc(), calloc();
void free();
#endif

/* Be careful declaring valloc.  OSF/1 3.2 declares it in stdlib.h,
   BSD/OS 2.1 in unistd.h.  Ultrix 4.4 doesn't declare it.  */
#undef valloc /* undo the kludge */
#if !defined(HAVE_VALLOC)
# define valloc malloc
#else
ptr_t valloc();
#endif

#if __STDC__
# include <stddef.h>
#endif
#ifdef STDC_HEADERS
# include <limits.h>
#endif
#ifndef INT_MAX
# define INT_MAX 2147483647
#endif
#ifndef UCHAR_MAX
# define UCHAR_MAX 255
#endif

#if !defined(STDC_HEADERS) && defined(HAVE_STRING_H) && defined(HAVE_MEMORY_H)
# include <memory.h>
#endif
#if defined(STDC_HEADERS) || defined(HAVE_STRING_H)
# include <string.h>
#else
# include <strings.h>
# undef strchr
# define strchr index
# undef strrchr
# define strrchr rindex
# undef memcpy
# define memcpy(d, s, n) bcopy((s), (d), (n))
#endif
#ifndef HAVE_MEMCHR
ptr_t memchr();
#endif

#include <errno.h>
#ifndef errno
extern int errno;
#endif

#ifndef HAVE_STRERROR
extern int sys_nerr;
extern char *sys_errlist[];
# define strerror(E) ((E) < sys_nerr ? sys_errlist[(E)] : "bogus error number")
#endif

#include <ctype.h>

#ifndef isgraph
# define isgraph(C) (isprint(C) && !isspace(C))
#endif

#ifdef isascii
# define ISALPHA(C) (isascii(C) && isalpha(C))
# define ISUPPER(C) (isascii(C) && isupper(C))
# define ISLOWER(C) (isascii(C) && islower(C))
# define ISDIGIT(C) (isascii(C) && isdigit(C))
# define ISXDIGIT(C) (isascii(C) && isxdigit(C))
# define ISSPACE(C) (isascii(C) && isspace(C))
# define ISPUNCT(C) (isascii(C) && ispunct(C))
# define ISALNUM(C) (isascii(C) && isalnum(C))
# define ISPRINT(C) (isascii(C) && isprint(C))
# define ISGRAPH(C) (isascii(C) && isgraph(C))
# define ISCNTRL(C) (isascii(C) && iscntrl(C))
#else
# define ISALPHA(C) isalpha(C)
# define ISUPPER(C) isupper(C)
# define ISLOWER(C) islower(C)
# define ISDIGIT(C) isdigit(C)
# define ISXDIGIT(C) isxdigit(C)
# define ISSPACE(C) isspace(C)
# define ISPUNCT(C) ispunct(C)
# define ISALNUM(C) isalnum(C)
# define ISPRINT(C) isprint(C)
# define ISGRAPH(C) isgraph(C)
# define ISCNTRL(C) iscntrl(C)
#endif

#define TOLOWER(C) (ISUPPER(C) ? tolower(C) : (C))

#if ENABLE_NLS
# include <libintl.h>
# define _(String) gettext (String)
#else
# define _(String) String
#endif
#define N_(String) String

#if HAVE_SETLOCALE
# include <locale.h>
#endif

/* Expand the filename wildcards in the argument vector ARGV,
   returning an argument vector that contains the expanded file names.
   ARGV and the result vector are terminated by null pointers.

   On systems where filename wildcards are expanded by the shell, this
   macro should just return its argument.  On systems like VMS where
   filename wildcards must be expanded by the application, this
   function may return newly allocated storage.  If the latter occurs,
   all the allocated storage (including argument vector and any newly
   allocated strings) can be freed by freeing the return value.

   An application can test whether storage was allocated, and can
   free the storage, with code that looks like this:

	char **expanded_argv = EXPAND_WILDCARDS (argv);
	process (expanded_argv);
	if (expanded_argv != argv)
	  free (expanded_argv);

   */

#ifndef EXPAND_WILDCARDS
#define EXPAND_WILDCARDS(argv) (argv)
#endif
