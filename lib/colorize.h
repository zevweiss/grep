/* Output colorization.

   Copyright 2011 Free Software Foundation, Inc.
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

#include <stdio.h>

static inline void init_colorize (void) { }
extern int should_colorize (int);

/* Start a colorized text attribute on stdout using the SGR_START
   format; the attribute is specified by SGR_SEQ.  */
static inline void
print_start_colorize (char const *sgr_start, char const *sgr_seq)
{
  printf (sgr_start, sgr_seq);
}

/* Restore the normal text attribute using the SGR_END string.  */
static inline void
print_end_colorize (char const *sgr_end)
{
  printf ("%s", sgr_end);
}
