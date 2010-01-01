#! /bin/sh
# Regression test for GNU grep.
#
# Copyright (C) 2001, 2006, 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

: ${srcdir=.}
: ${GREP=../src/grep}

failures=0

# The Khadafy test is brought to you by Scott Anderson . . .

${GREP} -E -f $srcdir/khadafy.regexp $srcdir/khadafy.lines > khadafy.out
if cmp $srcdir/khadafy.lines khadafy.out
then
	:
else
	echo Khadafy test failed -- output left on khadafy.out
	failures=1
fi

exit $failures
