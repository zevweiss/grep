/* pcresearch.c - searching subroutines using PCRE for grep.
   Copyright 2000, 2007, 2009-2014 Free Software Foundation, Inc.

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

/* Written August 1992 by Mike Haertel. */

#include <config.h>
#include "search.h"
#if HAVE_PCRE_H
# include <pcre.h>
#elif HAVE_PCRE_PCRE_H
# include <pcre/pcre.h>
#endif

#if HAVE_LIBPCRE
/* Compiled internal form of a Perl regular expression.  */
static pcre *cre;

/* Additional information about the pattern.  */
static pcre_extra *extra;

# ifdef PCRE_STUDY_JIT_COMPILE
static pcre_jit_stack *jit_stack;
# else
#  define PCRE_STUDY_JIT_COMPILE 0
# endif
#endif

void
Pcompile (char const *pattern, size_t size)
{
#if !HAVE_LIBPCRE
  error (EXIT_TROUBLE, 0, "%s",
         _("support for the -P option is not compiled into "
           "this --disable-perl-regexp binary"));
#else
  int e;
  char const *ep;
  char *re = xnmalloc (4, size + 7);
  int flags = (PCRE_MULTILINE
               | (match_icase ? PCRE_CASELESS : 0));
  char const *patlim = pattern + size;
  char *n = re;
  char const *p;
  char const *pnul;

  if (using_utf8 ())
    flags |= PCRE_UTF8;
  else if (MB_CUR_MAX != 1)
    error (EXIT_TROUBLE, 0, _("-P supports only unibyte and UTF-8 locales"));

  /* FIXME: Remove these restrictions.  */
  if (memchr (pattern, '\n', size))
    error (EXIT_TROUBLE, 0, _("the -P option only supports a single pattern"));

  *n = '\0';
  if (match_lines)
    strcpy (n, "^(?:");
  if (match_words)
    strcpy (n, "(?<!\\w)(?:");
  n += strlen (n);

  /* The PCRE interface doesn't allow NUL bytes in the pattern, so
     replace each NUL byte in the pattern with the four characters
     "\000", removing a preceding backslash if there are an odd
     number of backslashes before the NUL.

     FIXME: This method does not work with some multibyte character
     encodings, notably Shift-JIS, where a multibyte character can end
     in a backslash byte.  */
  for (p = pattern; (pnul = memchr (p, '\0', patlim - p)); p = pnul + 1)
    {
      memcpy (n, p, pnul - p);
      n += pnul - p;
      for (p = pnul; pattern < p && p[-1] == '\\'; p--)
        continue;
      n -= (pnul - p) & 1;
      strcpy (n, "\\000");
      n += 4;
    }

  memcpy (n, p, patlim - p);
  n += patlim - p;
  *n = '\0';
  if (match_words)
    strcpy (n, ")(?!\\w)");
  if (match_lines)
    strcpy (n, ")$");

  cre = pcre_compile (re, flags, &ep, &e, pcre_maketables ());
  if (!cre)
    error (EXIT_TROUBLE, 0, "%s", ep);

  extra = pcre_study (cre, PCRE_STUDY_JIT_COMPILE, &ep);
  if (ep)
    error (EXIT_TROUBLE, 0, "%s", ep);

# if PCRE_STUDY_JIT_COMPILE
  if (pcre_fullinfo (cre, extra, PCRE_INFO_JIT, &e))
    error (EXIT_TROUBLE, 0, _("internal error (should never happen)"));

  if (e)
    {
      /* A 32K stack is allocated for the machine code by default, which
         can grow to 512K if necessary. Since JIT uses far less memory
         than the interpreter, this should be enough in practice.  */
      jit_stack = pcre_jit_stack_alloc (32 * 1024, 512 * 1024);
      if (!jit_stack)
        error (EXIT_TROUBLE, 0,
               _("failed to allocate memory for the PCRE JIT stack"));
      pcre_assign_jit_stack (extra, NULL, jit_stack);
    }
# endif
  free (re);
#endif /* HAVE_LIBPCRE */
}

size_t
Pexecute (char const *buf, size_t size, size_t *match_size,
          char const *start_ptr)
{
#if !HAVE_LIBPCRE
  /* We can't get here, because Pcompile would have been called earlier.  */
  error (EXIT_TROUBLE, 0, _("internal error"));
  return -1;
#else
  /* This array must have at least two elements; everything after that
     is just for performance improvement in pcre_exec.  */
  enum { nsub = 300 };
  int sub[nsub];

  char const *p = start_ptr ? start_ptr : buf;
  int options = p == buf || p[-1] == eolbyte ? 0 : PCRE_NOTBOL;
  char const *line_start = buf;
  int e = PCRE_ERROR_NOMATCH;
  char const *line_end;

  /* pcre_exec mishandles matches that cross line boundaries.
     PCRE_MULTILINE isn't a win, partly because it's incompatible with
     -z, and partly because it checks the entire input buffer and is
     therefore slow on a large buffer containing many matches.
     Avoid these problems by matching line-by-line.  */
  for (; p < buf + size; p = line_start = line_end + 1)
    {
      line_end = memchr (p, eolbyte, buf + size - p);

      if (INT_MAX < line_end - p)
        error (EXIT_TROUBLE, 0, _("exceeded PCRE's line length limit"));

      /* Treat encoding-error bytes as data that cannot match.  */
      for (;;)
        {
          int valid_bytes;
          e = pcre_exec (cre, extra, p, line_end - p, 0, options, sub, nsub);
          if (e != PCRE_ERROR_BADUTF8)
            break;
          valid_bytes = sub[0];
          e = pcre_exec (cre, extra, p, valid_bytes, 0,
                         options | PCRE_NO_UTF8_CHECK | PCRE_NOTEOL,
                         sub, nsub);
          if (e != PCRE_ERROR_NOMATCH)
            break;
          p += valid_bytes + 1;
          options = PCRE_NOTBOL;
        }

      if (e != PCRE_ERROR_NOMATCH)
        break;
      options = 0;
    }

  if (e <= 0)
    {
      switch (e)
        {
        case PCRE_ERROR_NOMATCH:
          return -1;

        case PCRE_ERROR_NOMEMORY:
          error (EXIT_TROUBLE, 0, _("memory exhausted"));

        case PCRE_ERROR_MATCHLIMIT:
          error (EXIT_TROUBLE, 0,
                 _("exceeded PCRE's backtracking limit"));

        default:
          /* For now, we lump all remaining PCRE failures into this basket.
             If anyone cares to provide sample grep usage that can trigger
             particular PCRE errors, we can add to the list (above) of more
             detailed diagnostics.  */
          error (EXIT_TROUBLE, 0, _("internal PCRE error: %d"), e);
        }

      /* NOTREACHED */
      return -1;
    }
  else
    {
      char const *beg = start_ptr ? p + sub[0] : line_start;
      char const *end = start_ptr ? p + sub[1] : line_end + 1;
      *match_size = end - beg;
      return beg - buf;
    }
#endif
}
