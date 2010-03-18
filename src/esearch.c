#define EGREP_PROGRAM
#include "search.c"

struct matcher const matchers[] = {
  { "egrep", Ecompile, EGexecute },
  { NULL, NULL, NULL },
};

