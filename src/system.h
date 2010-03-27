/* Portability cruft.  Include after config.h and sys/types.h.
   Copyright 1996, 1998-2000, 2007, 2009-2010 Free Software Foundation, Inc.

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

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "binary-io.h"
#include "configmake.h"
#include "dirname.h"

#if O_BINARY
# define HAVE_DOS_FILE_CONTENTS 1
#endif

#ifdef EISDIR
# define is_EISDIR(e, f) ((e) == EISDIR)
#else
# define is_EISDIR(e, f) 0
#endif

#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

enum { EXIT_TROUBLE = 2 };

#ifndef isgraph
# define isgraph(C) (isprint(C) && !isspace(C))
#endif

#if defined (STDC_HEADERS) || (!defined (isascii) && !defined (HAVE_ISASCII))
# define IN_CTYPE_DOMAIN(c) 1
#else
# define IN_CTYPE_DOMAIN(c) isascii(c)
#endif

#define ISALPHA(C)	(IN_CTYPE_DOMAIN (C) && isalpha (C))
#define ISUPPER(C)	(IN_CTYPE_DOMAIN (C) && isupper (C))
#define ISLOWER(C)	(IN_CTYPE_DOMAIN (C) && islower (C))
#define ISDIGIT(C)	(IN_CTYPE_DOMAIN (C) && isdigit (C))
#define ISXDIGIT(C)	(IN_CTYPE_DOMAIN (C) && isxdigit (C))
#define ISSPACE(C)	(IN_CTYPE_DOMAIN (C) && isspace (C))
#define ISPUNCT(C)	(IN_CTYPE_DOMAIN (C) && ispunct (C))
#define ISALNUM(C)	(IN_CTYPE_DOMAIN (C) && isalnum (C))
#define ISPRINT(C)	(IN_CTYPE_DOMAIN (C) && isprint (C))
#define ISGRAPH(C)	(IN_CTYPE_DOMAIN (C) && isgraph (C))
#define ISCNTRL(C)	(IN_CTYPE_DOMAIN (C) && iscntrl (C))

#define TOLOWER(C) (ISUPPER(C) ? tolower(C) : (C))

#include <gettext.h>
#define N_(String) gettext_noop(String)
#define _(String) gettext(String)

#include <locale.h>

#ifndef initialize_main
#define initialize_main(argcp, argvp)
#endif

#include "unlocked-io.h"
