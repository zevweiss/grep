#include <config.h>
#include "search.h"

static void
Ecompile (char const *pattern, size_t size)
{
  return GEAcompile (pattern, size, RE_SYNTAX_POSIX_EGREP | RE_NO_EMPTY_RANGES);
}

struct matcher const matchers[] = {
  { "egrep", Ecompile, EGexecute },
  { NULL, NULL, NULL },
};

