/* searchutils.c - helper subroutines for grep's matchers.
   Copyright 1992, 1998, 2000, 2007, 2009-2016 Free Software Foundation, Inc.

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

#include <config.h>

#define SEARCH_INLINE _GL_EXTERN_INLINE
#define SYSTEM_INLINE _GL_EXTERN_INLINE
#include "search.h"

#define NCHAR (UCHAR_MAX + 1)

kwset_t
kwsinit (bool mb_trans)
{
  static char trans[NCHAR];
  char *transptr = NULL;

  if (match_icase && (MB_CUR_MAX == 1 || mb_trans))
    {
      if (MB_CUR_MAX == 1)
        for (int i = 0; i < NCHAR; i++)
          trans[i] = toupper (i);
      else
        for (int i = 0; i < NCHAR; i++)
          {
            wint_t wc = localeinfo.sbctowc[i];
            wint_t uwc = towupper (wc);
            if (uwc != wc)
              {
                mbstate_t mbs = { 0 };
                size_t len = wcrtomb (&trans[i], uwc, &mbs);
                if (len != 1)
                  abort ();
              }
            else
              trans[i] = i;
          }
      transptr = trans;
    }

  return kwsalloc (transptr, false);
}

/* In the buffer *MB_START, return the number of bytes needed to go
   back from CUR to the previous boundary, where a "boundary" is the
   start of a multibyte character or is an error-encoding byte.  The
   buffer ends at END (i.e., one past the address of the buffer's last
   byte).  If CUR is already at a boundary, return 0.  If *MB_START is
   greater than or equal to CUR, return the negative value CUR - *MB_START.

   When returning zero, set *MB_START to CUR.  When returning a
   positive value, set *MB_START to the next boundary after CUR, or to
   END if there is no such boundary.  When returning a negative value,
   leave *MB_START alone.  */
ptrdiff_t
mb_goback (char const **mb_start, char const *cur, char const *end)
{
  const char *p = *mb_start;
  const char *p0 = p;
  mbstate_t cur_state;

  memset (&cur_state, 0, sizeof cur_state);

  while (p < cur)
    {
      size_t clen = mb_clen (p, end - p, &cur_state);

      if ((size_t) -2 <= clen)
        {
          /* An invalid sequence, or a truncated multibyte character.
             Treat it as a single byte character.  */
          clen = 1;
          memset (&cur_state, 0, sizeof cur_state);
        }
      p0 = p;
      p += clen;
    }

  *mb_start = p;
  return p == cur ? 0 : cur - p0;
}

/* In the buffer BUF, return the wide character that is encoded just
   before CUR.  The buffer ends at END.  Return WEOF if there is no
   wide character just before CUR.  */
wint_t
mb_prev_wc (char const *buf, char const *cur, char const *end)
{
  if (cur == buf)
    return WEOF;
  char const *p = buf;
  cur--;
  cur -= mb_goback (&p, cur, end);
  return mb_next_wc (cur, end);
}

/* Return the wide character that is encoded at CUR.  The buffer ends
   at END.  Return WEOF if there is no wide character encoded at CUR.  */
wint_t
mb_next_wc (char const *cur, char const *end)
{
  wchar_t wc;
  mbstate_t mbs = { 0 };
  return (end - cur != 0 && mbrtowc (&wc, cur, end - cur, &mbs) < (size_t) -2
          ? wc : WEOF);
}
