#include <config.h>
#include "search.h"

struct matcher const matchers[] = {
  { "fgrep", Fcompile, Fexecute },
  { NULL, NULL, NULL },
};

