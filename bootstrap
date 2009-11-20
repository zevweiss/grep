#! /bin/sh
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

# Generate configure.ac with ALL_LINGUAS matching the languages
# we have...
ALL_LINGUAS=""
for i in po/*.po; do
	ALL_LINGUAS="$ALL_LINGUAS `basename $i .po`"
done
# Eliminate leading whitespace
ALL_LINGUAS="`echo $ALL_LINGUAS |cut -b1-`"
echo "$ALL_LINGUAS" > po/LINGUAS

# Let auto* do its work
if \
aclocal -I m4 &&
autoheader &&
automake -a &&
autoconf
then
	echo "Next, run ./configure && make && make check"
else
	echo
	echo "An error occured."
	exit 1
fi
