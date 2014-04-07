/* dfasearch.c - searching subroutines using dfa and regex for grep.
   Copyright 1992, 1998, 2000, 2007, 2009-2014 Free Software Foundation, Inc.

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
#include "intprops.h"
#include "search.h"
#include "dfa.h"

/* For -w, we also consider _ to be word constituent.  */
#define WCHAR(C) (isalnum (C) || (C) == '_')

/* KWset compiled pattern.  For Ecompile and Gcompile, we compile
   a list of strings, at least one of which is known to occur in
   any string matching the regexp. */
static kwset_t kwset;

/* DFA compiled regexp. */
static struct dfa *dfa;

/* The Regex compiled patterns.  */
static struct patterns
{
  /* Regex compiled regexp. */
  struct re_pattern_buffer regexbuf;
  struct re_registers regs; /* This is here on account of a BRAIN-DEAD
                               Q@#%!# library interface in regex.c.  */
} patterns0;

static struct patterns *patterns;
static size_t pcount;

/* Number of compiled fixed strings known to exactly match the regexp.
   If kwsexec returns < kwset_exact_matches, then we don't need to
   call the regexp matcher at all. */
static size_t kwset_exact_matches;

void
dfaerror (char const *mesg)
{
  error (EXIT_TROUBLE, 0, "%s", mesg);

  /* notreached */
  /* Tell static analyzers that this function does not return.  */
  abort ();
}

/* For now, the sole dfawarn-eliciting condition (use of a regexp
   like '[:lower:]') is unequivocally an error, so treat it as such,
   when possible.  */
void
dfawarn (char const *mesg)
{
  static enum { DW_NONE = 0, DW_POSIX, DW_GNU } mode;
  if (mode == DW_NONE)
    mode = (getenv ("POSIXLY_CORRECT") ? DW_POSIX : DW_GNU);
  if (mode == DW_GNU)
    dfaerror (mesg);
}

/* If the DFA turns out to have some set of fixed strings one of
   which must occur in the match, then we build a kwset matcher
   to find those strings, and thus quickly filter out impossible
   matches. */
static void
kwsmusts (void)
{
  /* With case-insensitive matching in a multi-byte locale, do not
     use kwsearch, because in that case, it would be too expensive,
     requiring that we case-convert all searched input.  */
  if (MB_CUR_MAX > 1 && match_icase)
    return;

  struct dfamust const *dm = dfamusts (dfa);
  if (dm)
    {
      char const *err;
      kwsinit (&kwset);
      /* First, we compile in the substrings known to be exact
         matches.  The kwset matcher will return the index
         of the matching string that it chooses. */
      for (; dm; dm = dm->next)
        {
          if (!dm->exact)
            continue;
          ++kwset_exact_matches;
          if ((err = kwsincr (kwset, dm->must, strlen (dm->must))) != NULL)
            error (EXIT_TROUBLE, 0, "%s", err);
        }
      /* Now, we compile the substrings that will require
         the use of the regexp matcher.  */
      for (dm = dfamusts (dfa); dm; dm = dm->next)
        {
          if (dm->exact)
            continue;
          if ((err = kwsincr (kwset, dm->must, strlen (dm->must))) != NULL)
            error (EXIT_TROUBLE, 0, "%s", err);
        }
      if ((err = kwsprep (kwset)) != NULL)
        error (EXIT_TROUBLE, 0, "%s", err);
    }
}

void
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

      patterns = xnrealloc (patterns, pcount + 1, sizeof *patterns);
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

  dfa = dfaalloc ();
  dfacomp (pattern, size, dfa, 1);
  kwsmusts ();

  free(motif);
}

size_t
EGexecute (char const *buf, size_t size, size_t *match_size,
           char const *start_ptr)
{
  char const *buflim, *beg, *end, *ptr, *match, *best_match, *mb_start;
  char eol = eolbyte;
  int backref;
  regoff_t start;
  size_t len, best_len;
  struct kwsmatch kwsm;
  size_t i;

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
              end = memchr (beg, eol, buflim - beg);
              end = end ? end + 1 : buflim;
              match = beg;
              beg = memrchr (buf, eol, beg - buf);
              beg = beg ? beg + 1 : buf;
              if (kwsm.index < kwset_exact_matches)
                {
                  if (mb_start < beg)
                    mb_start = beg;
                  if (MB_CUR_MAX == 1
                      || !is_mb_middle (&mb_start, match, buflim,
                                        kwsm.size[0]))
                    goto success;
                  /* The matched line starts in the middle of a multibyte
                     character.  Perform the DFA search starting from the
                     beginning of the next character.  */
                  if (! dfaexec (dfa, mb_start, (char *) end, 0, NULL,
                                 &backref))
                    continue;
                }
              else
                {
                  if (dfahint (dfa, beg, (char *) end, NULL) == (size_t) -1)
                    continue;
                  if (! dfaexec (dfa, beg, (char *) end, 0, NULL, &backref))
                    continue;
                }
            }
          else
            {
              /* No good fixed strings; start with DFA. */
              size_t offset, count;
              char const *next_beg;
              count = 0;
              offset = dfahint (dfa, beg, (char *) buflim, &count);
              if (offset == (size_t) -1)
                goto failure;
              if (offset == (size_t) -2)
                {
                  /* No use hint. */
                  next_beg = dfaexec (dfa, beg, (char *) buflim, 0,
                                      NULL, &backref);
                  /* If there's no match, or if we've matched the sentinel,
                     we're done.  */
                  if (next_beg == NULL || next_beg == buflim)
                    goto failure;
                }
              else
                next_beg = beg + offset;
              /* Narrow down to the line we've found. */
              beg = memrchr (buf, eol, next_beg - buf);
              beg = beg ? beg + 1 : buf;
              if (count != 0)
                {
                  /* If dfahint may match in multiple lines, try to
                     match in one line.  */
                  end = beg;
                  continue;
                }
              end = memchr (next_beg, eol, buflim - next_beg);
              end = end ? end + 1 : buflim;
              if (offset != (size_t) -2)
                {
                  next_beg = dfaexec (dfa, beg, (char *) end, 0, NULL,
                                      &backref);
                  /* If there's no match, or if we've matched the sentinel,
                     we're done.  */
                  if (next_beg == NULL || next_beg == end)
                    continue;
                }
            }
          /* Successful, no backreferences encountered! */
          if (!backref)
            goto success;
          ptr = beg;
        }
      else
        {
          /* We are looking for the leftmost (then longest) exact match.
             We will go through the outer loop only once.  */
          beg = buf;
          end = buflim;
          ptr = start_ptr;
        }

      /* If the "line" is longer than the maximum regexp offset,
         die as if we've run out of memory.  */
      if (TYPE_MAXIMUM (regoff_t) < end - beg - 1)
        xalloc_die ();

      /* If we've made it to this point, this means DFA has seen
         a probable match, and we need to run it through Regex. */
      best_match = end;
      best_len = 0;
      for (i = 0; i < pcount; i++)
        {
          patterns[i].regexbuf.not_eol = 0;
          start = re_search (&(patterns[i].regexbuf),
                             beg, end - beg - 1,
                             ptr - beg, end - ptr - 1,
                             &(patterns[i].regs));
          if (start < -1)
            xalloc_die ();
          else if (0 <= start)
            {
              len = patterns[i].regs.end[0] - start;
              match = beg + start;
              if (match > best_match)
                continue;
              if (start_ptr && !match_words)
                goto assess_pattern_match;
              if ((!match_lines && !match_words)
                  || (match_lines && len == end - ptr - 1))
                {
                  match = ptr;
                  len = end - ptr;
                  goto assess_pattern_match;
                }
              /* If -w, check if the match aligns with word boundaries.
                 We do this iteratively because:
                 (a) the line may contain more than one occurrence of the
                 pattern, and
                 (b) Several alternatives in the pattern might be valid at a
                 given point, and we may need to consider a shorter one to
                 find a word boundary.  */
              if (match_words)
                while (match <= best_match)
                  {
                    regoff_t shorter_len = 0;
                    if ((match == beg || !WCHAR (to_uchar (match[-1])))
                        && (start + len == end - beg - 1
                            || !WCHAR (to_uchar (match[len]))))
                      goto assess_pattern_match;
                    if (len > 0)
                      {
                        /* Try a shorter length anchored at the same place. */
                        --len;
                        patterns[i].regexbuf.not_eol = 1;
                        shorter_len = re_match (&(patterns[i].regexbuf),
                                                beg, match + len - ptr,
                                                match - beg,
                                                &(patterns[i].regs));
                        if (shorter_len < -1)
                          xalloc_die ();
                      }
                    if (0 < shorter_len)
                      len = shorter_len;
                    else
                      {
                        /* Try looking further on. */
                        if (match == end - 1)
                          break;
                        match++;
                        patterns[i].regexbuf.not_eol = 0;
                        start = re_search (&(patterns[i].regexbuf),
                                           beg, end - beg - 1,
                                           match - beg, end - match - 1,
                                           &(patterns[i].regs));
                        if (start < 0)
                          {
                            if (start < -1)
                              xalloc_die ();
                            break;
                          }
                        len = patterns[i].regs.end[0] - start;
                        match = beg + start;
                      }
                  } /* while (match <= best_match) */
              continue;
            assess_pattern_match:
              if (!start_ptr)
                {
                  /* Good enough for a non-exact match.
                     No need to look at further patterns, if any.  */
                  goto success;
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
  return -1;

 success:
  len = end - beg;
 success_in_len:;
  size_t off = beg - buf;
  *match_size = len;
  return off;
}
