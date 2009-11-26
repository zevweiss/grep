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

include gnulib.mk

INCLUDES = -I.. -I$(srcdir)
DEFS = @DEFS@

libgreputils_a_SOURCES += \
  closeout.c exclude.c hard-locale.c isdir.c quotearg.c savedir.c

libgreputils_a_LIBADD += @LIBOBJS@ @ALLOCA@
noinst_HEADERS += \
  closeout.h exclude.h hard-locale.h quotearg.h savedir.h
