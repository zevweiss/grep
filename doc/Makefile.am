# Process this file with automake to create Makefile.in
# Makefile.am for grep/doc.
#
# Copyright 2008-2010 Free Software Foundation, Inc.
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

info_TEXINFOS = grep.texi
grep_TEXINFOS = fdl.texi

man_MANS = grep.1 fgrep.1 egrep.1

EXTRA_DIST = grep.1 egrep.man fgrep.man

CLEANFILES = egrep.1 fgrep.1

fgrep.1: fgrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/fgrep.man > $@

egrep.1: egrep.man
	inst=`echo "grep" | sed '$(transform)'`.1; \
sed -e "s%@grep@%$$inst%g" $(srcdir)/egrep.man > $@
