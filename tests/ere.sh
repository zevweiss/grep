#! /bin/sh
# Regression test for GNU grep.
#
# Copyright (C) 2001, 2006, 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

: ${srcdir=.}

failures=0

# . . . and the following by Henry Spencer.

${AWK-awk} -f $srcdir/ere.awk $srcdir/ere.tests > ere.script

${SHELL-sh} ere.script && exit $failures
exit 1
