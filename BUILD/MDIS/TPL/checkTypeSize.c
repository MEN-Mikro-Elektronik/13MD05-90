#include <limits.h>

// the LP64 data model is the only one where long and int have different widths

#if (LONG_MAX > INT_MAX)
#  error _64
#else
#  error no_64
#endif