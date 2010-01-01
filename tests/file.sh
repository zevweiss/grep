#! /bin/sh
# Test for POSIX.2 options for grep:
# grep -E -f pattern_file file
# grep -F -f pattern_file file
# grep -G -f pattern_file file
#
# Copyright (C) 2001, 2006, 2009-2010 Free Software Foundation, Inc.
#
# Copying and distribution of this file, with or without modification,
# are permitted in any medium without royalty provided the copyright
# notice and this notice are preserved.

: ${srcdir=.}

failures=0

cat <<EOF >patfile
radar
MILES
GNU
EOF

# match
echo "miles" |  ${GREP} -i -E -f patfile > /dev/null 2>&1
if test $? -ne 0 ; then
        echo "File_pattern: Wrong status code, test \#1 failed"
        failures=1
fi

# match
echo "GNU" | ${GREP} -G -f patfile  > /dev/null 2>&1
if test $? -ne 0 ; then
        echo "File_pattern: Wrong status code, test \#2 failed"
        failures=1
fi

# checking for no match
echo "ridar" | ${GREP} -F -f patfile > /dev/null 2>&1
if test $? -ne 1 ; then
	echo "File_pattern: Wrong status code, test \#3 failed"
	failures=1
fi

cat <<EOF >patfile

EOF
# empty pattern : every match
echo "abbcd" | ${GREP} -F -f patfile > /dev/null 2>&1
if test $? -ne 0 ; then
	echo "File_pattern: Wrong status code, test \#4 failed"
	failures=1
fi

cp /dev/null patfile

# null pattern : no match
echo "abbcd" | ${GREP} -F -f patfile > /dev/null 2>&1
if test $? -ne 1 ; then
	echo "File_pattern: Wrong status code, test \#5 failed"
	failures=1
fi

exit $failures
