/* search.c - searching subroutines using dfa, kwset and regex for grep.
   Copyright 1992, 1998, 2000 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
   02111-1307, USA.  */

/* Written August 1992 by Mike Haertel. */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <sys/types.h>
#include "system.h"
#include "grep.h"
#include "regex.h"
#include "dfa.h"
#include "kwset.h"
#ifdef HAVE_LIBPCRE
# include <pcre.h>
#endif

#define NCHAR (UCHAR_MAX + 1)

/* For -w, we also consider _ to be word constituent.  */
#define WCHAR(C) (ISALNUM(C) || (C) == '_')

/* DFA compiled regexp. */
static struct dfa dfa;

/* Regex compiled regexp. */
static struct re_pattern_buffer regexbuf;

/* KWset compiled pattern.  For Ecompile and Gcompile, we compile
   a list of strings, at least one of which is known to occur in
   any string matching the regexp. */
static kwset_t kwset;

/* Number of compiled fixed strings known to exactly match the regexp.
   If kwsexec returns < kwset_exact_matches, then we don't need to
   call the regexp matcher at all. */
static int kwset_exact_matches;

void
dfaerror (char const *mesg)
{
  fatal(mesg, 0);
}

static void
kwsinit (void)
{
  static char trans[NCHAR];
  int i;

  if (match_icase)
    for (i = 0; i < NCHAR; ++i)
      trans[i] = TOLOWER(i);

  if (!(kwset = kwsalloc(match_icase ? trans : (char *) 0)))
    fatal("memory exhausted", 0);
}

/* If the DFA turns out to have some set of fixed strings one of
   which must occur in the match, then we build a kwset matcher
   to find those strings, and thus quickly filter out impossible
   matches. */
static void
kwsmusts (void)
{
  struct dfamust const *dm;
  char const *err;

  if (dfa.musts)
    {
      kwsinit();
      /* First, we compile in the substrings known to be exact
	 matches.  The kwset matcher will return the index
	 of the matching string that it chooses. */
      for (dm = dfa.musts; dm; dm = dm->next)
	{
	  if (!dm->exact)
	    continue;
	  ++kwset_exact_matches;
	  if ((err = kwsincr(kwset, dm->must, strlen(dm->must))) != 0)
	    fatal(err, 0);
	}
      /* Now, we compile the substrings that will require
	 the use of the regexp matcher.  */
      for (dm = dfa.musts; dm; dm = dm->next)
	{
	  if (dm->exact)
	    continue;
	  if ((err = kwsincr(kwset, dm->must, strlen(dm->must))) != 0)
	    fatal(err, 0);
	}
      if ((err = kwsprep(kwset)) != 0)
	fatal(err, 0);
    }
}

static void
Gcompile (char const *pattern, size_t size)
{
  const char *err;

  re_set_syntax(RE_SYNTAX_GREP | RE_HAT_LISTS_NOT_NEWLINE);
  dfasyntax(RE_SYNTAX_GREP | RE_HAT_LISTS_NOT_NEWLINE, match_icase, eolbyte);

  if ((err = re_compile_pattern(pattern, size, &regexbuf)) != 0)
    fatal(err, 0);

  /* In the match_words and match_lines cases, we use a different pattern
     for the DFA matcher that will quickly throw out cases that won't work.
     Then if DFA succeeds we do some hairy stuff using the regex matcher
     to decide whether the match should really count. */
  if (match_words || match_lines)
    {
      /* In the whole-word case, we use the pattern:
	 \(^\|[^[:alnum:]_]\)\(userpattern\)\([^[:alnum:]_]|$\).
	 In the whole-line case, we use the pattern:
	 ^\(userpattern\)$.  */

      static char const line_beg[] = "^\\(";
      static char const line_end[] = "\\)$";
      static char const word_beg[] = "\\(^\\|[^[:alnum:]_]\\)\\(";
      static char const word_end[] = "\\)\\([^[:alnum:]_]\\|$\\)";
      char *n = malloc (sizeof word_beg - 1 + size + sizeof word_end);
      size_t i;
      strcpy (n, match_lines ? line_beg : word_beg);
      i = strlen(n);
      memcpy(n + i, pattern, size);
      i += size;
      strcpy (n + i, match_lines ? line_end : word_end);
      i += strlen(n + i);
      pattern = n;
      size = i;
    }

  dfacomp (pattern, size, &dfa, 1);
  kwsmusts();
}

static void
Ecompile (char const *pattern, size_t size)
{
  const char *err;

  if (strcmp(matcher, "awk") == 0)
    {
      re_set_syntax(RE_SYNTAX_AWK);
      dfasyntax(RE_SYNTAX_AWK, match_icase, eolbyte);
    }
  else
    {
      re_set_syntax (RE_SYNTAX_POSIX_EGREP);
      dfasyntax (RE_SYNTAX_POSIX_EGREP, match_icase, eolbyte);
    }

  if ((err = re_compile_pattern(pattern, size, &regexbuf)) != 0)
    fatal(err, 0);

  /* In the match_words and match_lines cases, we use a different pattern
     for the DFA matcher that will quickly throw out cases that won't work.
     Then if DFA succeeds we do some hairy stuff using the regex matcher
     to decide whether the match should really count. */
  if (match_words || match_lines)
    {
      /* In the whole-word case, we use the pattern:
	 (^|[^[:alnum:]_])(userpattern)([^[:alnum:]_]|$).
	 In the whole-line case, we use the pattern:
	 ^(userpattern)$.  */

      static char const line_beg[] = "^(";
      static char const line_end[] = ")$";
      static char const word_beg[] = "(^|[^[:alnum:]_])(";
      static char const word_end[] = ")([^[:alnum:]_]|$)";
      char *n = malloc (sizeof word_beg - 1 + size + sizeof word_end);
      size_t i;
      strcpy (n, match_lines ? line_beg : word_beg);
      i = strlen(n);
      memcpy(n + i, pattern, size);
      i += size;
      strcpy (n + i, match_lines ? line_end : word_end);
      i += strlen(n + i);
      pattern = n;
      size = i;
    }

  dfacomp (pattern, size, &dfa, 1);
  kwsmusts();
}

static size_t
EGexecute (char const *buf, size_t size, size_t *match_size)
{
  register char const *buflim, *beg, *end;
  char eol = eolbyte;
  int backref, start, len;
  struct kwsmatch kwsm;
  static struct re_registers regs; /* This is static on account of a BRAIN-DEAD
				    Q@#%!# library interface in regex.c.  */

  buflim = buf + size;

  for (beg = end = buf; end < buflim; beg = end)
    {
      if (kwset)
	{
	  /* Find a possible match using the KWset matcher. */
	  size_t offset = kwsexec (kwset, beg, buflim - beg, &kwsm);
	  if (offset == (size_t) -1)
	    return (size_t) -1;
	  beg += offset;
	  /* Narrow down to the line containing the candidate, and
	     run it through DFA. */
	  end = memchr(beg, eol, buflim - beg);
	  end++;
	  while (beg > buf && beg[-1] != eol)
	    --beg;
	  if (kwsm.index < kwset_exact_matches)
	    goto success;
	  if (dfaexec (&dfa, beg, end - beg, &backref) == (size_t) -1)
	    continue;
	}
      else
	{
	  /* No good fixed strings; start with DFA. */
	  size_t offset = dfaexec (&dfa, beg, buflim - beg, &backref);
	  if (offset == (size_t) -1)
	    return (size_t) -1;
	  /* Narrow down to the line we've found. */
	  beg += offset;
	  end = memchr(beg, eol, buflim - beg);
	  end++;
	  while (beg > buf && beg[-1] != eol)
	    --beg;
	}

      /* Successful, no backreferences encountered! */
      if (!backref)
	goto success;

      /* If we've made it to this point, this means DFA has seen
	 a probable match, and we need to run it through Regex. */
      regexbuf.not_eol = 0;
      if (0 <= (start = re_search (&regexbuf, beg,
				   end - beg - 1, 0,
				   end - beg - 1, &regs)))
	{
	  len = regs.end[0] - start;
	  if ((!match_lines && !match_words)
	      || (match_lines && len == end - beg - 1))
	    goto success;
	  /* If -w, check if the match aligns with word boundaries.
	     We do this iteratively because:
	     (a) the line may contain more than one occurence of the pattern, and
	     (b) Several alternatives in the pattern might be valid at a given
	     point, and we may need to consider a shorter one to find a word
	     boundary. */
	  if (match_words)
	    while (start >= 0)
	      {
		if ((start == 0 || !WCHAR ((unsigned char) beg[start - 1]))
		    && (len == end - beg - 1
			|| !WCHAR ((unsigned char) beg[start + len])))
		  goto success;
		if (len > 0)
		  {
		    /* Try a shorter length anchored at the same place. */
		    --len;
		    regexbuf.not_eol = 1;
		    len = re_match(&regexbuf, beg, start + len, start, &regs);
		  }
		if (len <= 0)
		  {
		    /* Try looking further on. */
		    if (start == end - beg - 1)
		      break;
		    ++start;
		    regexbuf.not_eol = 0;
		    start = re_search (&regexbuf, beg, end - beg - 1,
				       start, end - beg - 1 - start, &regs);
		    len = regs.end[0] - start;
		  }
	      }
	}
    }

  return -1;

 success:
  *match_size = end - beg;
  return beg - buf;
}

static void
Fcompile (char const *pattern, size_t size)
{
  char const *beg, *lim, *err;

  kwsinit();
  beg = pattern;
  do
    {
      for (lim = beg; lim < pattern + size && *lim != '\n'; ++lim)
	;
      if ((err = kwsincr(kwset, beg, lim - beg)) != 0)
	fatal(err, 0);
      if (lim < pattern + size)
	++lim;
      beg = lim;
    }
  while (beg < pattern + size);

  if ((err = kwsprep(kwset)) != 0)
    fatal(err, 0);
}

static size_t
Fexecute (char const *buf, size_t size, size_t *match_size)
{
  register char const *beg, *try, *end;
  register size_t len;
  char eol = eolbyte;
  struct kwsmatch kwsmatch;

  for (beg = buf; beg <= buf + size; ++beg)
    {
      size_t offset = kwsexec (kwset, beg, buf + size - beg, &kwsmatch);
      if (offset == (size_t) -1)
	return offset;
      beg += offset;
      len = kwsmatch.size[0];
      if (match_lines)
	{
	  if (beg > buf && beg[-1] != eol)
	    continue;
	  if (beg + len < buf + size && beg[len] != eol)
	    continue;
	  goto success;
	}
      else if (match_words)
	for (try = beg; len; )
	  {
	    if (try > buf && WCHAR((unsigned char) try[-1]))
	      break;
	    if (try + len < buf + size && WCHAR((unsigned char) try[len]))
	      {
		size_t offset = kwsexec (kwset, beg, --len, &kwsmatch);
		if (offset == (size_t) -1)
		  return offset;
		try = beg + offset;
		len = kwsmatch.size[0];
	      }
	    else
	      goto success;
	  }
      else
	goto success;
    }

  return -1;

 success:
  end = memchr (beg + len, eol, (buf + size) - (beg + len));
  end++;
  while (buf < beg && beg[-1] != eol)
    --beg;
  *match_size = end - beg;
  return beg - buf;
}

#if HAVE_LIBPCRE
/* Compiled internal form of a Perl regular expression.  */
static pcre *cre;

/* Additional information about the pattern.  */
static pcre_extra *extra;
#endif

static void
Pcompile (char const *pattern, size_t size)
{
#if !HAVE_LIBPCRE
  fatal (_("The -P option is not supported"), 0);
#else
  int e;
  char const *ep;
  char *re = xmalloc (4 * size + 7);
  int flags = PCRE_MULTILINE | (match_icase ? PCRE_CASELESS : 0);
  char const *patlim = pattern + size;
  char *n = re;
  char const *p;
  char const *pnul;

  /* FIXME: Remove this restriction.  */
  if (eolbyte != '\n')
    fatal (_("The -P and -z options cannot be combined"), 0);

  *n = '\0';
  if (match_lines)
    strcpy (n, "^(");
  if (match_words)
    strcpy (n, "\\b(");
  n += strlen (n);
  
  /* The PCRE interface doesn't allow NUL bytes in the pattern, so
     replace each NUL byte in the pattern with the four characters
     "\000", removing a preceding backslash if there are an odd
     number of backslashes before the NUL.

     FIXME: This method does not work with some multibyte character
     encodings, notably Shift-JIS, where a multibyte character can end
     in a backslash byte.  */
  for (p = pattern; (pnul = memchr (p, '\0', patlim - p)); p = pnul + 1)
    {
      memcpy (n, p, pnul - p);
      n += pnul - p;
      for (p = pnul; pattern < p && p[-1] == '\\'; p--)
	continue;
      n -= (pnul - p) & 1;
      strcpy (n, "\\000");
      n += 4;
    }
  
  memcpy (n, p, patlim - p);
  n += patlim - p;
  *n = '\0';
  if (match_words)
    strcpy (n, ")\\b");
  if (match_lines)
    strcpy (n, ")$");
      
  cre = pcre_compile (re, flags, &ep, &e, pcre_maketables ());
  if (!cre)
    fatal (ep, 0);

  extra = pcre_study (cre, 0, &ep);
  if (ep)
    fatal (ep, 0);

  free (re);
#endif
}

static size_t
Pexecute (char const *buf, size_t size, size_t *match_size)
{
#if !HAVE_LIBPCRE
  abort ();
  return -1;
#else
  /* This array must have at least two elements; everything after that
     is just for performance improvement in pcre_exec.  */
  int sub[300];

  int e = pcre_exec (cre, extra, buf, size, 0, 0,
		     sub, sizeof sub / sizeof *sub);

  if (e <= 0)
    {
      switch (e)
	{
	case PCRE_ERROR_NOMATCH:
	  return -1;
	  
	case PCRE_ERROR_NOMEMORY:
	  fatal (_("Memory exhausted"), 0);
	  
	default:
	  abort ();
	}
    }
  else
    {
      /* Narrow down to the line we've found.  */
      char const *beg = buf + sub[0];
      char const *end = buf + sub[1];
      char const *buflim = buf + size;
      char eol = eolbyte;
      end = memchr (end, eol, buflim - end);
      end++;
      while (buf < beg && beg[-1] != eol)
	--beg;

      *match_size = end - beg;
      return beg - buf;
    }
#endif
}

struct matcher const matchers[] = {
  { "default", Gcompile, EGexecute },
  { "grep", Gcompile, EGexecute },
  { "egrep", Ecompile, EGexecute },
  { "awk", Ecompile, EGexecute },
  { "fgrep", Fcompile, Fexecute },
  { "perl", Pcompile, Pexecute },
  { "", 0, 0 },
};
