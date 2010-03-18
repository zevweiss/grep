#define FGREP_PROGRAM
#include "search.c"

struct matcher const matchers[] = {
  { "fgrep", Fcompile, Fexecute },
  { NULL, NULL, NULL },
};

