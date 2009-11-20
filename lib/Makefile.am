# Copyright 1997, 1998, 2005, 2006, 2007, 2008, 2009 Free Software
# Foundation, Inc.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
SUBDIRS = posix

noinst_LIBRARIES = libgreputils.a

INCLUDES = -I.. -I$(srcdir) -I../intl
DEFS = -DLIBDIR=\"$(libdir)\" @DEFS@

libgreputils_a_SOURCES = \
  closeout.c error.c exclude.c hard-locale.c \
  isdir.c obstack.c quotearg.c savedir.c xmalloc.c xstrtol.c  xstrtoumax.c

libgreputils_a_LIBADD = @LIBOBJS@ @ALLOCA@
noinst_HEADERS = \
  closeout.h error.h exclude.h fnmatch.h getopt.h gettext.h \
  hard-locale.h obstack.h quotearg.h regex.h savedir.h xalloc.h xstrtol.h

EXTRA_DIST = strtol.c malloc.c realloc.c
