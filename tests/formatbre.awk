#
# Basic Regular Expression

# kip comments
$0 ~ /^#/ { next; }

# skip those option specific to regexec/regcomp
$2 ~ /[msnr$#p^]/ { next; }

# skip empty lines
$0 ~ /^$/ { next; }

# debug
#{ printf ("<%s> <%s> <%s> <%s>\n", $1, $2, $3, $4); }

# errors
NF == 3 {
	gsub (/@/, ",");
# error in regex
	if (index ($2, "C") != 0)
	{
		if (index ($2, "b") != 0)
			printf ("2@%s@%s\n", $1, $3);
	}
# erro no match
	else
	{
		if (index ($2, "b") != 0)
			printf ("1@%s@%s\n", $1, $3);
	}
}

# ok
NF == 4 {
	gsub (/@/, ",");
	if (index ($2, "b") != 0)
		printf ("0@%s@%s\n", $1, $3);
}
