#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
# include <linux/utsrelease.h>
#else
# include <generated/utsrelease.h>
#endif

RELEASE UTS_RELEASE
VERSION LINUX_VERSION_CODE

