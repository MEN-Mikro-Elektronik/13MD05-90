#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,33)
 #include <generated/utsrelease.h>
#else
 #if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,18)
  #include <linux/utsrelease.h>
 #endif
#endif

RELEASE UTS_RELEASE
VERSION LINUX_VERSION_CODE
