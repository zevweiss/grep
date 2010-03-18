#include "dfasearch.c"

struct matcher const matchers[] = {
  { "egrep", Ecompile, EGexecute },
  { NULL, NULL, NULL },
};

