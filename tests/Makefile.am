## Process this file with automake to create Makefile.in
# Copyright 1997-1998, 2005-2010 Free Software Foundation, Inc.
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

TESTS =		\
  backref.sh	\
  bre.sh	\
  empty.sh	\
  ere.sh	\
  file.sh	\
  fmbtest.sh	\
  foad1.sh	\
  khadafy.sh	\
  options.sh	\
  pcre.sh	\
  spencer1.sh	\
  status.sh	\
  warning.sh	\
  yesno.sh

EXTRA_DIST =	\
  $(TESTS)	\
  bre.awk	\
  bre.tests	\
  ere.awk	\
  ere.tests	\
  khadafy.lines	\
  khadafy.regexp \
  spencer1.awk	\
  spencer1.tests

CLEANFILES = \
  spencer1.script bre.script ere.script khadafy.out patfile \
  cspatfile csinput yesno.txt

TESTS_ENVIRONMENT = \
  AWK=$(AWK) \
  GREP=$(top_builddir)/src/grep \
  GREP_OPTIONS='' \
  LC_ALL=C

# the *.sh when checkout does not have the right permission
# make sure everyting is ok.
dist-hook:
	cd $(distdir); for i in $(TESTS); do chmod +x $$i; done
