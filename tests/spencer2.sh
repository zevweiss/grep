#! /bin/sh
# Regression test for GNU grep.

: ${srcdir=.}

failures=0

# . . . and the following by Henry Spencer.

${AWK-awk} -f $srcdir/spencer1.awk $srcdir/spencer2.tests > spencer2.script

sh spencer2.script && exit $failures
exit 1
