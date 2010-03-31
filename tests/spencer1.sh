#! /bin/sh
# Regression test for GNU grep.
# Copyright (C) 1988 Henry Spencer.
# Copyright (C) 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

: ${srcdir=.}

failures=0

# . . . and the following by Henry Spencer.

${AWK-awk} -f $srcdir/spencer1.awk $srcdir/spencer1.tests > spencer1.script

${SHELL-sh} spencer1.script && exit $failures
exit 1
