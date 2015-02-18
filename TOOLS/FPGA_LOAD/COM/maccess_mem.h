#ifndef _MACCESS_MEM_H
#define _MACCESS_MEM_H

#define MAC_MEM_MAPPED

#include <MEN/men_typs.h>
#include <MEN/maccess.h>

__inline u_int8 MREAD_MEM_D8(MACCESS ma, u_int32 offs) {
	return MREAD_D8(ma, offs);
}
__inline u_int16 MREAD_MEM_D16(MACCESS ma, u_int32 offs) {
	return MREAD_D16(ma, offs);
}
__inline u_int32 MREAD_MEM_D32(MACCESS ma, u_int32 offs) {
	return MREAD_D32(ma, offs);
}

__inline void MWRITE_MEM_D8(MACCESS ma, u_int32 offs, u_int32 val) {
	MWRITE_D8(ma, offs, val);
	return;
}
__inline void MWRITE_MEM_D16(MACCESS ma, u_int32 offs, u_int32 val) {
	MWRITE_D16(ma, offs, val);
	return;
}
__inline void MWRITE_MEM_D32(MACCESS ma, u_int32 offs, u_int32 val) {
	MWRITE_D32(ma, offs, val);
	return;
}

#undef MREAD_D8
#undef MREAD_D16
#undef MREAD_D32
#undef MWRITE_D8
#undef MWRITE_D16
#undef MWRITE_D32
#undef MBLOCK_READ_D8
#undef MBLOCK_READ_D16
#undef MBLOCK_READ_D32
#undef MBLOCK_WRITE_D8
#undef MBLOCK_WRITE_D16
#undef MBLOCK_WRITE_D32
#undef MBLOCK_SET_D8
#undef MBLOCK_SET_D16
#undef MBLOCK_SET_D32
#undef MSETMASK_D8
#undef MSETMASK_D16
#undef MSETMASK_D32
#undef MCLRMASK_D8
#undef MCLRMASK_D16
#undef MCLRMASK_D32
#undef MFIFO_READ_D8
#undef MFIFO_READ_D16
#undef MFIFO_READ_D32
#undef MFIFO_WRITE_D8
#undef MFIFO_WRITE_D16
#undef MFIFO_WRITE_D32

#undef _MACCESS_H

#ifdef WINNT
#	undef _NT_FUNC
#endif /* WINNT */

#ifdef LINUX
#	undef _MACCESS_LINUX_H

#	ifdef _MAC_LINUX_GENERIC_H
#		undef _MAC_LINUX_GENERIC_H
#	endif

#	ifdef _MAC_LINUX_PPC_H
#		undef _MAC_LINUX_PPC_H
#	endif

#	ifdef _MAC_LINUX_USERSPACE_H
#		undef _MAC_LINUX_USERSPACE_H
#	endif
#endif /* LINUX */

#undef MAC_MEM_MAPPED

#endif /* _MACCESS_MEM_H */
