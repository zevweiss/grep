#! /bin/sh
# Test for POSIX.2 options for grep
#
# Copyright (C) 2001, 2006, 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.
#
# grep [ -E| -F][ -c| -l| -q ][-insvx] -e pattern_list
#      [-f pattern_file] ... [file. ..]
# grep [ -E| -F][ -c| -l| -q ][-insvx][-e pattern_list]
#      -f pattern_file ... [file ...]
# grep [ -E| -F][ -c| -l| -q ][-insvx] pattern_list [file...]

: ${srcdir=.}

failures=0

# checking for -E extended regex
echo "abababccccccd" | ${GREP} -E -e 'c{3}' > /dev/null 2>&1
if test $? -ne 0 ; then
        echo "Options: Wrong status code, test \#1 failed"
        failures=1
fi

# checking for basic regex
echo "abababccccccd" | ${GREP} -G -e 'c\{3\}' > /dev/null 2>&1
if test $? -ne 0 ; then
        echo "Options: Wrong status code, test \#2 failed"
        failures=1
fi

# checking for fixed string
echo "abababccccccd" | ${GREP} -F -e 'c\{3\}' > /dev/null 2>&1
if test $? -ne 1 ; then
	echo "Options: Wrong status code, test \#3 failed"
	failures=1
fi

exit $failures
