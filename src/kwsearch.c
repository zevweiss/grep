/* kwsearch.c - searching subroutines using kwset for grep.
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

/* Written August 1992 by Mike Haertel. */

#include <config.h>
#include "search.h"

void *
Fcompile (char const *pattern, size_t size, reg_syntax_t ignored)
{
  kwset_t kwset;
  size_t total = size;
  char *buf = NULL;
  size_t bufalloc = 0;

  kwset = kwsinit (true);

  char const *p = pattern;
  do
    {
      size_t len;
      char const *sep = memchr (p, '\n', total);
      if (sep)
        {
          len = sep - p;
          sep++;
          total -= (len + 1);
        }
      else
        {
          len = total;
          total = 0;
        }

      if (match_lines)
        {
          if (eolbyte == '\n' && pattern < p && sep)
            p--;
          else
            {
              if (bufalloc < len + 2)
                {
                  free (buf);
                  bufalloc = len + 2;
                  buf = x2realloc (NULL, &bufalloc);
                  buf[0] = eolbyte;
                }
              memcpy (buf + 1, p, len);
              buf[len + 1] = eolbyte;
              p = buf;
            }
          len += 2;
        }
      kwsincr (kwset, p, len);

      p = sep;
    }
  while (p);

  free (buf);
  kwsprep (kwset);

  return kwset;
}

size_t
Fexecute (void *vcp, char const *buf, size_t size, size_t *match_size,
          char const *start_ptr)
{
  char const *beg, *end, *mb_start;
  size_t len;
  char eol = eolbyte;
  struct kwsmatch kwsmatch;
  size_t ret_val;
  bool mb_check;
  bool longest;
  kwset_t kwset = vcp;

  if (match_lines)
    mb_check = longest = false;
  else
    {
      mb_check = localeinfo.multibyte & !localeinfo.using_utf8;
      longest = mb_check | !!start_ptr | match_words;
    }

  for (mb_start = beg = start_ptr ? start_ptr : buf; beg <= buf + size; beg++)
    {
      size_t offset = kwsexec (kwset, beg - match_lines,
                               buf + size - beg + match_lines, &kwsmatch,
                               longest);
      if (offset == (size_t) -1)
        break;
      len = kwsmatch.size[0] - 2 * match_lines;
      if (mb_check && mb_goback (&mb_start, beg + offset, buf + size) != 0)
        {
          /* We have matched a single byte that is not at the beginning of a
             multibyte character.  mb_goback has advanced MB_START past that
             multibyte character.  Now, we want to position BEG so that the
             next kwsexec search starts there.  Thus, to compensate for the
             for-loop's BEG++, above, subtract one here.  This code is
             unusually hard to reach, and exceptionally, let's show how to
             trigger it here:

               printf '\203AA\n'|LC_ALL=ja_JP.SHIFT_JIS src/grep -F A

             That assumes the named locale is installed.
             Note that your system's shift-JIS locale may have a different
             name, possibly including "sjis".  */
          beg = mb_start - 1;
          continue;
        }
      beg += offset;
      if (!!start_ptr & !match_words)
        goto success_in_beg_and_len;
      if (match_lines)
        {
          len += start_ptr == NULL;
          goto success_in_beg_and_len;
        }
      if (! match_words)
        goto success;

      /* Succeed if the preceding and following characters are word
         constituents.  If the following character is not a word
         constituent, keep trying with shorter matches.  */
      char const *bol = memrchr (mb_start, eol, beg - mb_start);
      if (bol)
        mb_start = bol + 1;
      if (! wordchar_prev (mb_start, beg, buf + size))
        for (;;)
          {
            if (! wordchar_next (beg + len, buf + size))
              {
                if (start_ptr)
                  goto success_in_beg_and_len;
                else
                  goto success;
              }
            if (!len)
              break;
            offset = kwsexec (kwset, beg, --len, &kwsmatch, true);
            if (offset != 0)
              break;
            len = kwsmatch.size[0];
          }

      /* No word match was found at BEG.  Skip past word constituents,
         since they cannot precede the next match and not skipping
         them could make things much slower.  */
      beg += wordchars_size (beg, buf + size);
      mb_start = beg;
    } /* for (beg in buf) */

  return -1;

 success:
  end = memchr (beg + len, eol, (buf + size) - (beg + len));
  end = end ? end + 1 : buf + size;
  beg = memrchr (buf, eol, beg - buf);
  beg = beg ? beg + 1 : buf;
  len = end - beg;
 success_in_beg_and_len:;
  size_t off = beg - buf;

  *match_size = len;
  ret_val = off;
  return ret_val;
}
