#include <endian.h>

#if (__BYTE_ORDER == __LITTLE_ENDIAN)
# error little
#else
# if (__BYTE_ORDER == __BIG_ENDIAN)
#  error big
# else
#  error undefined
# endif
#endif

