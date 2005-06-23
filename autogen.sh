#! /bin/sh

chmod +x tests/*.sh   # fix a bug in CVS

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
