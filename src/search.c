/* search.c - searching subroutines using dfa, kwset and regex for grep.
   Copyright 1992, 1998, 2000, 2007, 2009-2010 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

/* Written August 1992 by Mike Haertel. */

#include <config.h>

#include <sys/types.h>

#include "mbsupport.h"
#ifdef MBS_SUPPORT
/* We can handle multibyte strings. */
# include <wchar.h>
# include <wctype.h>
#endif

#include "system.h"
#include "grep.h"
#ifndef FGREP_PROGRAM
# include <regex.h>
# include "dfa.h"
#endif
#include "kwset.h"
#include "error.h"
#include "xalloc.h"
#ifdef HAVE_LIBPCRE
# include <pcre.h>
#endif

#define NCHAR (UCHAR_MAX + 1)

/* For -w, we also consider _ to be word constituent.  */
#define WCHAR(C) (ISALNUM(C) || (C) == '_')

/* KWset compiled pattern.  For Ecompile and Gcompile, we compile
   a list of strings, at least one of which is known to occur in
   any string matching the regexp. */
static kwset_t kwset;

static void
kwsinit (void)
{
  static char trans[NCHAR];
  int i;

  if (match_icase && MB_CUR_MAX == 1)
    {
      for (i = 0; i < NCHAR; ++i)
        trans[i] = TOLOWER (i);

      kwset = kwsalloc (trans);
    }
  else
    kwset = kwsalloc (NULL);

  if (!kwset)
    xalloc_die ();
}

#ifdef MBS_SUPPORT
/* Convert the *N-byte string, BEG, to lowercase, and write the
   NUL-terminated result into malloc'd storage.  Upon success, set *N
   to the length (in bytes) of the resulting string (not including the
   trailing NUL byte), and return a pointer to the lowercase string.
   Upon memory allocation failure, this function exits.

   Note that while this function returns a pointer to malloc'd storage,
   the caller must not free it, since this function retains a pointer
   to the buffer and reuses it on any subsequent call.  As a consequence,
   this function is not thread-safe.  */
static char *
mbtolower (const char *beg, size_t *n)
{
  static char *out;
  static size_t outalloc;
  size_t outlen, mb_cur_max;
  mbstate_t is, os;
  const char *end;
  char *p;

  if (*n > outalloc)
    {
      out = xrealloc (out, *n);
      outalloc = *n;
    }

  memset (&is, 0, sizeof (is));
  memset (&os, 0, sizeof (os));
  end = beg + *n;

  mb_cur_max = MB_CUR_MAX;
  p = out;
  outlen = 0;
  while (beg < end)
    {
      wchar_t wc;
      size_t mbclen = mbrtowc(&wc, beg, end - beg, &is);
      if (outlen + mb_cur_max >= outalloc)
        {
          out = x2nrealloc (out, &outalloc, 1);
          p = out + outlen;
        }

      if (mbclen == (size_t) -1 || mbclen == (size_t) -2 || mbclen == 0)
        {
          /* An invalid sequence, or a truncated multi-octet character.
             We treat it as a single-octet character.  */
          *p++ = *beg++;
          outlen++;
          memset (&is, 0, sizeof (is));
          memset (&os, 0, sizeof (os));
        }
      else
        {
          beg += mbclen;
          mbclen = wcrtomb (p, towlower ((wint_t) wc), &os);
          p += mbclen;
          outlen += mbclen;
        }
    }

  *n = p - out;
  *p++ = 0;
  return out;
}
#endif


#ifndef FGREP_PROGRAM
/* DFA compiled regexp. */
static struct dfa dfa;

/* The Regex compiled patterns.  */
static struct patterns
{
  /* Regex compiled regexp. */
  struct re_pattern_buffer regexbuf;
  struct re_registers regs; /* This is here on account of a BRAIN-DEAD
			       Q@#%!# library interface in regex.c.  */
} patterns0;

struct patterns *patterns;
size_t pcount;

void
dfaerror (char const *mesg)
{
  error (EXIT_TROUBLE, 0, "%s", mesg);
}

/* Number of compiled fixed strings known to exactly match the regexp.
   If kwsexec returns < kwset_exact_matches, then we don't need to
   call the regexp matcher at all. */
static int kwset_exact_matches;

static char const *
kwsincr_case (const char *must)
{
  const char *buf;
  size_t n;

  n = strlen (must);
#ifdef MBS_SUPPORT
  if (match_icase && MB_CUR_MAX > 1)
    buf = mbtolower (must, &n);
  else
#endif
    buf = must;
  return kwsincr (kwset, buf, n);
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
      kwsinit ();
      /* First, we compile in the substrings known to be exact
	 matches.  The kwset matcher will return the index
	 of the matching string that it chooses. */
      for (dm = dfa.musts; dm; dm = dm->next)
	{
	  if (!dm->exact)
	    continue;
	  ++kwset_exact_matches;
	  if ((err = kwsincr_case (dm->must)) != NULL)
	    error (EXIT_TROUBLE, 0, "%s", err);
	}
      /* Now, we compile the substrings that will require
	 the use of the regexp matcher.  */
      for (dm = dfa.musts; dm; dm = dm->next)
	{
	  if (dm->exact)
	    continue;
	  if ((err = kwsincr_case (dm->must)) != NULL)
	    error (EXIT_TROUBLE, 0, "%s", err);
	}
      if ((err = kwsprep (kwset)) != NULL)
	error (EXIT_TROUBLE, 0, "%s", err);
    }
}
#endif /* !FGREP_PROGRAM */

#ifdef MBS_SUPPORT

static bool
is_mb_middle(const char **good, const char *buf, const char *end)
{
  const char *p = *good;
  const char *prev = p;
  mbstate_t cur_state;

  /* TODO: can be optimized for UTF-8.  */
  memset(&cur_state, 0, sizeof(mbstate_t));
  while (p < buf)
    {
      size_t mbclen = mbrlen(p, end - p, &cur_state);

      /* Store the beginning of the previous complete multibyte character.  */
      if (mbclen != (size_t) -2)
        prev = p;

      if (mbclen == (size_t) -1 || mbclen == (size_t) -2 || mbclen == 0)
	{
	  /* An invalid sequence, or a truncated multibyte character.
	     We treat it as a single byte character.  */
	  mbclen = 1;
	}
      p += mbclen;
    }

  *good = prev;
  return p > buf;
}
#endif /* MBS_SUPPORT */

#if defined(GREP_PROGRAM) || defined(EGREP_PROGRAM)
/* No __VA_ARGS__ in C89.  So we have to do it this way.  */
static void
GEAcompile (char const *pattern, size_t size, reg_syntax_t syntax_bits)
{
  const char *err;
  const char *p, *sep;
  size_t total = size;
  char *motif;

  if (match_icase)
    syntax_bits |= RE_ICASE;
  re_set_syntax (syntax_bits);
  dfasyntax (syntax_bits, match_icase, eolbyte);

  /* For GNU regex compiler we have to pass the patterns separately to detect
     errors like "[\nallo\n]\n".  The patterns here are "[", "allo" and "]"
     GNU regex should have raise a syntax error.  The same for backref, where
     the backref should have been local to each pattern.  */
  p = pattern;
  do
    {
      size_t len;
      sep = memchr (p, '\n', total);
      if (sep)
	{
	  len = sep - p;
	  sep++;
	  total -= (len + 1);
	}
      else
	{
	  len = total;
	  total = 0;
	}

      patterns = realloc (patterns, (pcount + 1) * sizeof (*patterns));
      if (patterns == NULL)
	error (EXIT_TROUBLE, errno, _("memory exhausted"));
      patterns[pcount] = patterns0;

      if ((err = re_compile_pattern (p, len,
				    &(patterns[pcount].regexbuf))) != NULL)
	error (EXIT_TROUBLE, 0, "%s", err);
      pcount++;

      p = sep;
    } while (sep && total != 0);

  /* In the match_words and match_lines cases, we use a different pattern
     for the DFA matcher that will quickly throw out cases that won't work.
     Then if DFA succeeds we do some hairy stuff using the regex matcher
     to decide whether the match should really count. */
  if (match_words || match_lines)
    {
      static char const line_beg_no_bk[] = "^(";
      static char const line_end_no_bk[] = ")$";
      static char const word_beg_no_bk[] = "(^|[^[:alnum:]_])(";
      static char const word_end_no_bk[] = ")([^[:alnum:]_]|$)";
      static char const line_beg_bk[] = "^\\(";
      static char const line_end_bk[] = "\\)$";
      static char const word_beg_bk[] = "\\(^\\|[^[:alnum:]_]\\)\\(";
      static char const word_end_bk[] = "\\)\\([^[:alnum:]_]\\|$\\)";
      int bk = !(syntax_bits & RE_NO_BK_PARENS);
      char *n = xmalloc (sizeof word_beg_bk - 1 + size + sizeof word_end_bk);

      strcpy (n, match_lines ? (bk ? line_beg_bk : line_beg_no_bk)
			     : (bk ? word_beg_bk : word_beg_no_bk));
      total = strlen(n);
      memcpy (n + total, pattern, size);
      total += size;
      strcpy (n + total, match_lines ? (bk ? line_end_bk : line_end_no_bk)
				     : (bk ? word_end_bk : word_end_no_bk));
      total += strlen (n + total);
      pattern = motif = n;
      size = total;
    }
  else
    motif = NULL;

  dfacomp (pattern, size, &dfa, 1);
  kwsmusts ();

  free(motif);
}

#ifndef EGREP_PROGRAM
static void
Gcompile (char const *pattern, size_t size)
{
  return GEAcompile (pattern, size,
		     (RE_SYNTAX_GREP
		      | RE_HAT_LISTS_NOT_NEWLINE
		      | RE_NO_EMPTY_RANGES));
}

static void
Acompile (char const *pattern, size_t size)
{
  return GEAcompile (pattern, size, RE_SYNTAX_AWK);
}
#endif /* !EGREP_PROGRAM */

static void
Ecompile (char const *pattern, size_t size)
{
  return GEAcompile (pattern, size, RE_SYNTAX_POSIX_EGREP | RE_NO_EMPTY_RANGES);
}

static size_t
EGexecute (char const *buf, size_t size, size_t *match_size,
	   char const *start_ptr)
{
  char const *buflim, *beg, *end, *match, *best_match, *mb_start;
  char eol = eolbyte;
  int backref, start, len, best_len;
  struct kwsmatch kwsm;
  size_t i, ret_val;
#ifdef MBS_SUPPORT
  if (MB_CUR_MAX > 1)
    {
      if (match_icase)
        {
          /* mbtolower adds a NUL byte at the end.  That will provide
	     space for the sentinel byte dfaexec may add.  */
          char *case_buf = mbtolower (buf, &size);
	  if (start_ptr)
	    start_ptr = case_buf + (start_ptr - buf);
          buf = case_buf;
        }
    }
#endif /* MBS_SUPPORT */

  mb_start = buf;
  buflim = buf + size;

  for (beg = end = buf; end < buflim; beg = end)
    {
      if (!start_ptr)
	{
	  /* We don't care about an exact match.  */
	  if (kwset)
	    {
	      /* Find a possible match using the KWset matcher. */
	      size_t offset = kwsexec (kwset, beg, buflim - beg, &kwsm);
	      if (offset == (size_t) -1)
		goto failure;
	      beg += offset;
	      /* Narrow down to the line containing the candidate, and
		 run it through DFA. */
	      if ((end = memchr(beg, eol, buflim - beg)) != NULL)
	        end++;
              else
                end = buflim;
	      match = beg;
	      while (beg > buf && beg[-1] != eol)
		--beg;
	      if (kwsm.index < kwset_exact_matches)
                {
#ifdef MBS_SUPPORT
                  if (mb_start < beg)
                    mb_start = beg;
                  if (MB_CUR_MAX == 1 || !is_mb_middle (&mb_start, match, buflim))
#endif
                    goto success;
                }
	      if (dfaexec (&dfa, beg, (char *) end, 0, NULL, &backref) == NULL)
		continue;
	    }
	  else
	    {
	      /* No good fixed strings; start with DFA. */
	      char const *next_beg = dfaexec (&dfa, beg, (char *) buflim,
					      0, NULL, &backref);
	      if (next_beg == NULL)
		break;
	      /* Narrow down to the line we've found. */
	      beg = next_beg;
	      if ((end = memchr(beg, eol, buflim - beg)) != NULL)
	        end++;
              else
                end = buflim;
	      while (beg > buf && beg[-1] != eol)
		--beg;
	    }
	  /* Successful, no backreferences encountered! */
	  if (!backref)
	    goto success;
	}
      else
	{
	  /* We are looking for the leftmost (then longest) exact match.
	     We will go through the outer loop only once.  */
	  beg = start_ptr;
	  end = buflim;
	}

      /* If we've made it to this point, this means DFA has seen
	 a probable match, and we need to run it through Regex. */
      best_match = end;
      best_len = 0;
      for (i = 0; i < pcount; i++)
	{
	  patterns[i].regexbuf.not_eol = 0;
	  if (0 <= (start = re_search (&(patterns[i].regexbuf),
				       buf, end - buf - 1,
				       beg - buf, end - beg - 1,
				       &(patterns[i].regs))))
	    {
	      len = patterns[i].regs.end[0] - start;
	      match = buf + start;
	      if (match > best_match)
		continue;
	      if (start_ptr && !match_words)
		goto assess_pattern_match;
	      if ((!match_lines && !match_words)
		  || (match_lines && len == end - beg - 1))
		{
		  match = beg;
		  len = end - beg;
		  goto assess_pattern_match;
		}
	      /* If -w, check if the match aligns with word boundaries.
		 We do this iteratively because:
		 (a) the line may contain more than one occurence of the
		 pattern, and
		 (b) Several alternatives in the pattern might be valid at a
		 given point, and we may need to consider a shorter one to
		 find a word boundary.  */
	      if (match_words)
		while (match <= best_match)
		  {
		    if ((match == buf || !WCHAR ((unsigned char) match[-1]))
			&& (len == end - beg - 1
			    || !WCHAR ((unsigned char) match[len])))
		      goto assess_pattern_match;
		    if (len > 0)
		      {
			/* Try a shorter length anchored at the same place. */
			--len;
			patterns[i].regexbuf.not_eol = 1;
			len = re_match (&(patterns[i].regexbuf),
					buf, match + len - beg, match - buf,
					&(patterns[i].regs));
		      }
		    if (len <= 0)
		      {
			/* Try looking further on. */
			if (match == end - 1)
			  break;
			match++;
			patterns[i].regexbuf.not_eol = 0;
			start = re_search (&(patterns[i].regexbuf),
					   buf, end - buf - 1,
					   match - buf, end - match - 1,
					   &(patterns[i].regs));
			if (start < 0)
			  break;
			len = patterns[i].regs.end[0] - start;
			match = buf + start;
		      }
		  } /* while (match <= best_match) */
	      continue;
	    assess_pattern_match:
	      if (!start_ptr)
		{
		  /* Good enough for a non-exact match.
		     No need to look at further patterns, if any.  */
		  beg = match;
		  goto success_in_len;
		}
	      if (match < best_match || (match == best_match && len > best_len))
		{
		  /* Best exact match:  leftmost, then longest.  */
		  best_match = match;
		  best_len = len;
		}
	    } /* if re_search >= 0 */
	} /* for Regex patterns.  */
	if (best_match < end)
	  {
	    /* We have found an exact match.  We were just
	       waiting for the best one (leftmost then longest).  */
	    beg = best_match;
	    len = best_len;
	    goto success_in_len;
	  }
    } /* for (beg = end ..) */

 failure:
  ret_val = -1;
  goto out;

 success:
  len = end - beg;
 success_in_len:
  *match_size = len;
  ret_val = beg - buf;
 out:
  return ret_val;
}
#endif /* defined(GREP_PROGRAM) || defined(EGREP_PROGRAM) */

#if defined(GREP_PROGRAM) || defined(FGREP_PROGRAM)
static void
Fcompile (char const *pattern, size_t size)
{
  char const *beg, *end, *lim, *err, *pat;
  size_t psize;

  kwsinit ();
  psize = size;
  if (match_icase && MB_CUR_MAX > 1)
    pat = mbtolower (pattern, &psize);
  else
    pat = pattern;

  beg = pat;
  do
    {
      for (lim = beg;; ++lim)
	{
	  end = lim;
	  if (lim >= pat + psize)
	    break;
	 if (*lim == '\n')
	   {
	     lim++;
	     break;
	   }
#if HAVE_DOS_FILE_CONTENTS
	 if (*lim == '\r' && lim + 1 < pat + psize && lim[1] == '\n')
	   {
	     lim += 2;
	     break;
	   }
#endif
	}

      if ((err = kwsincr (kwset, beg, end - beg)) != NULL)
	error (EXIT_TROUBLE, 0, "%s", err);
      beg = lim;
    }
  while (beg < pat + psize);

  if ((err = kwsprep (kwset)) != NULL)
    error (EXIT_TROUBLE, 0, "%s", err);
}

static size_t
Fexecute (char const *buf, size_t size, size_t *match_size,
	  char const *start_ptr)
{
  char const *beg, *try, *end, *mb_start;
  size_t len;
  char eol = eolbyte;
  struct kwsmatch kwsmatch;
  size_t ret_val;
#ifdef MBS_SUPPORT
  if (MB_CUR_MAX > 1)
    {
      if (match_icase)
        {
          char *case_buf = mbtolower (buf, &size);
	  if (start_ptr)
	    start_ptr = case_buf + (start_ptr - buf);
          buf = case_buf;
        }
    }
#endif /* MBS_SUPPORT */

  for (mb_start = beg = start_ptr ? start_ptr : buf; beg <= buf + size; beg++)
    {
      size_t offset = kwsexec (kwset, beg, buf + size - beg, &kwsmatch);
      if (offset == (size_t) -1)
	goto failure;
#ifdef MBS_SUPPORT
      if (MB_CUR_MAX > 1 && is_mb_middle (&mb_start, beg + offset, buf + size))
        {
          beg = mb_start - 1;
          continue; /* It is a part of multibyte character.  */
        }
#endif /* MBS_SUPPORT */
      beg += offset;
      len = kwsmatch.size[0];
      if (start_ptr && !match_words)
	goto success_in_beg_and_len;
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
		offset = kwsexec (kwset, beg, --len, &kwsmatch);
		if (offset == (size_t) -1)
		  break;
		try = beg + offset;
		len = kwsmatch.size[0];
	      }
	    else if (!start_ptr)
	      goto success;
	    else
	      goto success_in_beg_and_len;
	  } /* for (try) */
      else
	goto success;
    } /* for (beg in buf) */

 failure:
  ret_val = -1;
  goto out;

 success:
  if ((end = memchr (beg + len, eol, (buf + size) - (beg + len))) != NULL)
    end++;
  else
    end = buf + size;
  while (buf < beg && beg[-1] != eol)
    --beg;
  len = end - beg;
 success_in_beg_and_len:
  *match_size = len;
  ret_val = beg - buf;
 out:
  return ret_val;
}
#endif /* defined(GREP_PROGRAM) || defined(FGREP_PROGRAM) */

#ifdef GREP_PROGRAM
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
  error (EXIT_TROUBLE, 0, "%s",
	 _("support for the -P option is not compiled into "
	   "this --disable-perl-regexp binary"));
#else
  int e;
  char const *ep;
  char *re = xmalloc (4 * size + 7);
  int flags = PCRE_MULTILINE | (match_icase ? PCRE_CASELESS : 0);
  char const *patlim = pattern + size;
  char *n = re;
  char const *p;
  char const *pnul;

  /* FIXME: Remove these restrictions.  */
  if (memchr(pattern, '\n', size))
    error (EXIT_TROUBLE, 0, _("the -P option only supports a single pattern"));

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
    error (EXIT_TROUBLE, 0, "%s", ep);

  extra = pcre_study (cre, 0, &ep);
  if (ep)
    error (EXIT_TROUBLE, 0, "%s", ep);

  free (re);
#endif
}

static size_t
Pexecute (char const *buf, size_t size, size_t *match_size,
	  char const *start_ptr)
{
#if !HAVE_LIBPCRE
  abort ();
  return -1;
#else
  /* This array must have at least two elements; everything after that
     is just for performance improvement in pcre_exec.  */
  int sub[300];

  const char *line_buf, *line_end, *line_next;
  int e = PCRE_ERROR_NOMATCH;
  ptrdiff_t start_ofs = start_ptr ? start_ptr - buf : 0;

  /* PCRE can't limit the matching to single lines, therefore we have to
     match each line in the buffer separately.  */
  for (line_next = buf;
       e == PCRE_ERROR_NOMATCH && line_next < buf + size;
       start_ofs -= line_next - line_buf)
    {
      line_buf = line_next;
      line_end = memchr (line_buf, eolbyte, (buf + size) - line_buf);
      if (line_end == NULL)
        line_next = line_end = buf + size;
      else
        line_next = line_end + 1;

      if (start_ptr && start_ptr >= line_end)
        continue;

      e = pcre_exec (cre, extra, line_buf, line_end - line_buf,
                     start_ofs < 0 ? 0 : start_ofs, 0,
                     sub, sizeof sub / sizeof *sub);
    }

  if (e <= 0)
    {
      switch (e)
	{
	case PCRE_ERROR_NOMATCH:
	  return -1;

	case PCRE_ERROR_NOMEMORY:
	  error (EXIT_TROUBLE, 0, _("memory exhausted"));

	default:
	  abort ();
	}
    }
  else
    {
      /* Narrow down to the line we've found.  */
      char const *beg = line_buf + sub[0];
      char const *end = line_buf + sub[1];
      char const *buflim = buf + size;
      char eol = eolbyte;
      if (!start_ptr)
	{
	  /* FIXME: The case when '\n' is not found indicates a bug:
	     Since grep is line oriented, the match should never contain
	     a newline, so there _must_ be a newline following.
	   */
	  if (!(end = memchr (end, eol, buflim - end)))
	    end = buflim;
	  else
	    end++;
	  while (buf < beg && beg[-1] != eol)
	    --beg;
	}

      *match_size = end - beg;
      return beg - buf;
    }
#endif
}
#endif /* GREP_PROGRAM */
