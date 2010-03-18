#include "kwsearch.c"

struct matcher const matchers[] = {
  { "fgrep", Fcompile, Fexecute },
  { NULL, NULL, NULL },
};

