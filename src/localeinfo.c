/* locale information

   Copyright 2016 Free Software Foundation, Inc.

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

/* Written by Paul Eggert.  */

#include <config.h>

#include <localeinfo.h>

#include <verify.h>

#include <limits.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

/* The sbclen implementation relies on this.  */
verify (MB_LEN_MAX <= SCHAR_MAX);

/* Return true if the locale uses UTF-8.  */

static bool
is_using_utf8 (void)
{
  wchar_t wc;
  mbstate_t mbs = {0};
  return mbrtowc (&wc, "\xc4\x80", 2, &mbs) == 2 && wc == 0x100;
}

/* Initialize *LOCALEINFO from the current locale.  */

void
init_localeinfo (struct localeinfo *localeinfo)
{
  int i;

  localeinfo->multibyte = MB_CUR_MAX > 1;
  localeinfo->using_utf8 = is_using_utf8 ();

  for (i = CHAR_MIN; i <= CHAR_MAX; i++)
    {
      char c = i;
      unsigned char uc = i;
      mbstate_t s = {0};
      wchar_t wc;
      size_t len = mbrtowc (&wc, &c, 1, &s);
      localeinfo->sbclen[uc] = len <= 1 ? 1 : - (int) - len;
      localeinfo->sbctowc[uc] = len <= 1 ? wc : WEOF;
    }
}
