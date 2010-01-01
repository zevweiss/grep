/* Configuration file for OpenVMS */

/* Copyright (C) 1992, 1997-2002, 2004-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#define HAVE_STRING_H 1

#define HAVE_MEMCHAR

#define HAVE_STRERROR

#define HAVE_STDLIB_H 1

#define HAVE_UNISTD_H 1

#define STDC_HEADERS

#define HAVE_DIRENT_H 1

#define VERSION "2.4.1"
/* Avoid namespace collision with operating system supplied C library */

/* Make sure we have the C-RTL definitions */
#include <unistd.h>
#include <stdio.h>

/* Now override everything with the GNU version */
#ifdef VMS
# define getopt gnu_getopt
# define optarg gnu_optarg
# define optopt gnu_optopt
# define optind gnu_optind
# define opterr gnu_opterr
#endif

#if defined(VMS) && defined(__DECC) /* need function prototype */
#if (__DECC_VER<50790004)           /* have an own one         */
char *alloca(unsigned int);
#else
#define alloca __ALLOCA
#endif
#endif
