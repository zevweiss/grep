#! /bin/sh

chmod +x tests/*.sh   # fix a bug in CVS

# Generate configure.ac with ALL_LINGUAS matching the languages
# we have...
ALL_LINGUAS=""
for i in po/*.po; do
	ALL_LINGUAS="$ALL_LINGUAS `basename $i .po`"
done
# Eliminate leading whitespace
ALL_LINGUAS="`echo $ALL_LINGUAS |cut -b1-`"
sed -e "s,@ALL_LINGUAS@,$ALL_LINGUAS," configure.ac.in >configure.ac

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
