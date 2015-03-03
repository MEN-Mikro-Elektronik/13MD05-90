/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  dbg.h
 *
 *      \author  uf
 *        $Date: 2013/07/02 11:17:11 $
 *    $Revision: 1.25 $
 *
 *     \project  DBG Library
 *  	 \brief  Debug interface definitions and macros
 *    \switches  DBG                      - enable debug code\n
 *				 LINUX, __QNXNTO__, WINNT - operating system switch\n
 *               _NO_DBG_HANDLE           - don't declare DBG_HANDLE\n
 *               __INC_DBG_OS__           - force include dbg_os.h\n
 *               DBG_INITEXIT_DEFINED_BY_DBG_OS_H - DBG_Init/Exit() defined in dbg_os.h\n
 *               DBG_WRITE_DEFINED_BY_DBG_OS_H    - DBG_Write() defined in dbg_os.h\n
 *               DBG_MEMDUMP_DEFINED_BY_DBG_OS_H  - DBG_Memdump() defined in dbg_os.h\n
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: dbg.h,v $
 * Revision 1.25  2013/07/02 11:17:11  MRoth
 * R: external Dbg_Init,Dbg_Exit functions replaced with blank defines
 * M: added external prototypes again to avoid compiler errors, removed defines
 *---------------------------------------------------------------------------
 * Copyright (c) 2015 MEN Mikro Elektronik GmbH. All rights reserved.
 ****************************************************************************/

#ifndef _DBG_H_
#define _DBG_H_

#ifdef __cplusplus
   extern "C" {
#endif

/*-----------------------------------------+
|  TYPEDEFS                                |
+------------------------------------------*/
#ifndef _NO_DBG_HANDLE
   typedef void* DBG_HANDLE;
#endif

/*-----------------------------------------+
|  DEFINES & CONST                         |
+------------------------------------------*/
/**
 * \defgroup _DBG_LEV DBG_LEVx debug levels
 *  Debug levels for debug macros.
 *  @{
 */
#define DBG_LEV1      0x00000001  /**< level 1: function names */
#define DBG_LEV2      0x00000002  /**< level 2: more verbose */
#define DBG_LEV3      0x00000004  /**< level 3: very verbose */
#define DBG_LEV4      0x00000008  /**< level 4: very verbose */
#define DBG_LEV5      0x00000010  /**< level 5: very verbose */
#define DBG_LEV6      0x00000020  /**< level 6: very verbose */
#define DBG_LEV7      0x00000040  /**< level 7: very verbose */
#define DBG_LEV8      0x00000080  /**< level 8: very verbose */

#define DBG_LEVERR    0x00008000  /**< error level: error messages */

#define DBG_NORM 	0x40000000  /**< called from non-interrupt routines */
#define DBG_INTR  	0x80000000  /**< called from interrupt routines */
#define DBG_NORM_INTR	0xc0000000  /**< called from all routines */

#define DBG_OFF       0x00000000  /**< no debug output */
#define DBG_ALL		  0xffffffff  /**< all debug output */
/*! @} */

/* check for matching level */
#define DBG_NORM1(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV1))
#define DBG_NORM2(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV2))
#define DBG_NORM3(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV3))
#define DBG_NORM4(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV4))
#define DBG_NORM5(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV5))
#define DBG_NORM6(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV6))
#define DBG_NORM7(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV7))
#define DBG_NORM8(lev)   ((lev & DBG_NORM) && (lev & DBG_LEV8))
#define DBG_NORMERR(lev) ((lev & DBG_NORM_INTR) && (lev & DBG_LEVERR))
#define DBG_INTR1(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV1))
#define DBG_INTR2(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV2))
#define DBG_INTR3(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV3))
#define DBG_INTR4(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV4))
#define DBG_INTR5(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV5))
#define DBG_INTR6(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV6))
#define DBG_INTR7(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV7))
#define DBG_INTR8(lev)   ((lev & DBG_INTR) && (lev & DBG_LEV8))
#define DBG_INTRERR(lev) DBG_NORMERR(lev)


/* patch windows DBG define */
#if defined(WINNT)

#if defined(MAC_USERSPACE) && defined(_DEBUG)
	#define DBG 1
#endif /* MAC_USERSPACE && _DEBUG */

# ifdef DBG
#  if DBG==0
#   undef DBG
#  endif /* DBG==0 */
# endif /* DBG */

/* user space */
#if defined(MAC_USERSPACE)
# include <MEN/udbg_win.h>
/* kernel space */
#else
/* include OS-specific macros */
#if defined(_MDIS5_)
# include <MEN/dbg_os.h>
#endif /* _MDIS5_ */
#endif /* kernel space */

#endif /* WINNT */

/* check for debug enabled */
#ifdef DBG
# define DBGCMD(_x_)	_x_
#else
# define DBGCMD(_x_)
#endif

/* include OS-specific macros */
#if defined(LINUX) || defined(__QNXNTO__) || defined(__INC_DBG_OS__)
# include <MEN/dbg_os.h>
#endif


/* general macros */
#define DBGINIT(_x_)    DBGCMD(DBG_Init _x_)
#define DBGEXIT(_x_)    DBGCMD(DBG_Exit _x_)

/* lowlevel macros: check for matching debug level "lev" */
# define DBGWRT_LEV1(lev,_x_)     DBGCMD(if (DBG_NORM1(lev)) DBG_Write _x_)
# define DBGWRT_LEV2(lev,_x_) 	  DBGCMD(if (DBG_NORM2(lev)) DBG_Write _x_)
# define DBGWRT_LEV3(lev,_x_) 	  DBGCMD(if (DBG_NORM3(lev)) DBG_Write _x_)
# define DBGWRT_LEV4(lev,_x_) 	  DBGCMD(if (DBG_NORM4(lev)) DBG_Write _x_)
# define DBGWRT_LEV5(lev,_x_)     DBGCMD(if (DBG_NORM5(lev)) DBG_Write _x_)
# define DBGWRT_LEV6(lev,_x_) 	  DBGCMD(if (DBG_NORM6(lev)) DBG_Write _x_)
# define DBGWRT_LEV7(lev,_x_) 	  DBGCMD(if (DBG_NORM7(lev)) DBG_Write _x_)
# define DBGWRT_LEV8(lev,_x_) 	  DBGCMD(if (DBG_NORM8(lev)) DBG_Write _x_)
# define DBGWRT_LEVERR(lev,_x_)	  DBGCMD(if (DBG_NORMERR(lev)) DBG_Write _x_)

# define DBGDMP_LEV1(lev,_x_) 	  DBGCMD(if (DBG_NORM1(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV2(lev,_x_) 	  DBGCMD(if (DBG_NORM2(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV3(lev,_x_) 	  DBGCMD(if (DBG_NORM3(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV4(lev,_x_) 	  DBGCMD(if (DBG_NORM4(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV5(lev,_x_) 	  DBGCMD(if (DBG_NORM5(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV6(lev,_x_) 	  DBGCMD(if (DBG_NORM6(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV7(lev,_x_) 	  DBGCMD(if (DBG_NORM7(lev)) DBG_Memdump _x_)
# define DBGDMP_LEV8(lev,_x_) 	  DBGCMD(if (DBG_NORM8(lev)) DBG_Memdump _x_)
# define DBGDMP_LEVERR(lev,_x_)   DBGCMD(if (DBG_NORMERR(lev)) DBG_Memdump _x_)

# define IDBGWRT_LEV1(lev,_x_) 	  DBGCMD(if (DBG_INTR1(lev)) DBG_Write _x_)
# define IDBGWRT_LEV2(lev,_x_) 	  DBGCMD(if (DBG_INTR2(lev)) DBG_Write _x_)
# define IDBGWRT_LEV3(lev,_x_) 	  DBGCMD(if (DBG_INTR3(lev)) DBG_Write _x_)
# define IDBGWRT_LEV4(lev,_x_) 	  DBGCMD(if (DBG_INTR4(lev)) DBG_Write _x_)
# define IDBGWRT_LEV5(lev,_x_) 	  DBGCMD(if (DBG_INTR5(lev)) DBG_Write _x_)
# define IDBGWRT_LEV6(lev,_x_) 	  DBGCMD(if (DBG_INTR6(lev)) DBG_Write _x_)
# define IDBGWRT_LEV7(lev,_x_) 	  DBGCMD(if (DBG_INTR7(lev)) DBG_Write _x_)
# define IDBGWRT_LEV8(lev,_x_) 	  DBGCMD(if (DBG_INTR8(lev)) DBG_Write _x_)
# define IDBGWRT_LEVERR(lev,_x_)  DBGCMD(if (DBG_INTRERR(lev)) DBG_Write _x_)

# define IDBGDMP_LEV1(lev,_x_)    DBGCMD(if (DBG_INTR1(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV2(lev,_x_)    DBGCMD(if (DBG_INTR2(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV3(lev,_x_)    DBGCMD(if (DBG_INTR3(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV4(lev,_x_)    DBGCMD(if (DBG_INTR4(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV5(lev,_x_)    DBGCMD(if (DBG_INTR5(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV6(lev,_x_)    DBGCMD(if (DBG_INTR6(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV7(lev,_x_)    DBGCMD(if (DBG_INTR7(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEV8(lev,_x_)    DBGCMD(if (DBG_INTR8(lev)) DBG_Memdump _x_)
# define IDBGDMP_LEVERR(lev,_x_)  DBGCMD(if (DBG_INTRERR(lev)) DBG_Memdump _x_)

/**
 * \defgroup _DBGWRT DBGWRT highlevel macros
 *  Check for matching debug level DBG_MYLEVEL.
 *  @{
 */
# define DBGWRT_1(_x_) 		DBGWRT_LEV1(DBG_MYLEVEL, _x_)
# define DBGWRT_2(_x_) 		DBGWRT_LEV2(DBG_MYLEVEL, _x_)
# define DBGWRT_3(_x_) 		DBGWRT_LEV3(DBG_MYLEVEL, _x_)
# define DBGWRT_4(_x_) 		DBGWRT_LEV4(DBG_MYLEVEL, _x_)
# define DBGWRT_5(_x_) 		DBGWRT_LEV5(DBG_MYLEVEL, _x_)
# define DBGWRT_6(_x_) 		DBGWRT_LEV6(DBG_MYLEVEL, _x_)
# define DBGWRT_7(_x_) 		DBGWRT_LEV7(DBG_MYLEVEL, _x_)
# define DBGWRT_8(_x_) 		DBGWRT_LEV8(DBG_MYLEVEL, _x_)
# define DBGWRT_ERR(_x_) 	DBGWRT_LEVERR(DBG_MYLEVEL, _x_)

# define DBGDMP_1(_x_) 		DBGDMP_LEV1(DBG_MYLEVEL, _x_)
# define DBGDMP_2(_x_) 		DBGDMP_LEV2(DBG_MYLEVEL, _x_)
# define DBGDMP_3(_x_) 		DBGDMP_LEV3(DBG_MYLEVEL, _x_)
# define DBGDMP_4(_x_) 		DBGDMP_LEV4(DBG_MYLEVEL, _x_)
# define DBGDMP_5(_x_) 		DBGDMP_LEV5(DBG_MYLEVEL, _x_)
# define DBGDMP_6(_x_) 		DBGDMP_LEV6(DBG_MYLEVEL, _x_)
# define DBGDMP_7(_x_) 		DBGDMP_LEV7(DBG_MYLEVEL, _x_)
# define DBGDMP_8(_x_) 		DBGDMP_LEV8(DBG_MYLEVEL, _x_)
# define DBGDMP_ERR(_x_) 	DBGDMP_LEVERR(DBG_MYLEVEL, _x_)

# define IDBGWRT_1(_x_) 	IDBGWRT_LEV1(DBG_MYLEVEL, _x_)
# define IDBGWRT_2(_x_) 	IDBGWRT_LEV2(DBG_MYLEVEL, _x_)
# define IDBGWRT_3(_x_) 	IDBGWRT_LEV3(DBG_MYLEVEL, _x_)
# define IDBGWRT_4(_x_) 	IDBGWRT_LEV4(DBG_MYLEVEL, _x_)
# define IDBGWRT_5(_x_) 	IDBGWRT_LEV5(DBG_MYLEVEL, _x_)
# define IDBGWRT_6(_x_) 	IDBGWRT_LEV6(DBG_MYLEVEL, _x_)
# define IDBGWRT_7(_x_) 	IDBGWRT_LEV7(DBG_MYLEVEL, _x_)
# define IDBGWRT_8(_x_) 	IDBGWRT_LEV8(DBG_MYLEVEL, _x_)
# define IDBGWRT_ERR(_x_) 	IDBGWRT_LEVERR(DBG_MYLEVEL, _x_)

# define IDBGDMP_1(_x_) 	IDBGDMP_LEV1(DBG_MYLEVEL, _x_)
# define IDBGDMP_2(_x_) 	IDBGDMP_LEV2(DBG_MYLEVEL, _x_)
# define IDBGDMP_3(_x_) 	IDBGDMP_LEV3(DBG_MYLEVEL, _x_)
# define IDBGDMP_4(_x_) 	IDBGDMP_LEV4(DBG_MYLEVEL, _x_)
# define IDBGDMP_5(_x_) 	IDBGDMP_LEV5(DBG_MYLEVEL, _x_)
# define IDBGDMP_6(_x_) 	IDBGDMP_LEV6(DBG_MYLEVEL, _x_)
# define IDBGDMP_7(_x_) 	IDBGDMP_LEV7(DBG_MYLEVEL, _x_)
# define IDBGDMP_8(_x_) 	IDBGDMP_LEV8(DBG_MYLEVEL, _x_)
# define IDBGDMP_ERR(_x_) 	IDBGDMP_LEVERR(DBG_MYLEVEL, _x_)
/*! @} */

/*-----------------------------------------+
|  GLOBALS                                 |
+------------------------------------------*/
/* none */

/*-----------------------------------------+
|  PROTOTYPES                              |
+------------------------------------------*/
#ifndef DBG_INITEXIT_DEFINED_BY_DBG_OS_H
extern int32 DBG_Init(char *name, DBG_HANDLE **dbgP);
extern int32 DBG_Exit(DBG_HANDLE **dbgP);
#endif /* DBG_INITEXIT_DEFINED_BY_DBG_OS_H */

#ifndef DBG_WRITE_DEFINED_BY_DBG_OS_H
extern int32 DBG_Write(DBG_HANDLE *dbg, char *frmt, ...);
#endif /* DBG_WRITE_DEFINED_BY_DBG_OS_H */

#ifndef DBG_MEMDUMP_DEFINED_BY_DBG_OS_H
extern int32 DBG_Memdump(DBG_HANDLE *dbg, char *txt, void *buf,
						 u_int32 len, u_int32 fmt);
#endif /* DBG_MEMDUMP_DEFINED_BY_DBG_OS_H */


#ifdef __cplusplus
   }
#endif

#endif /*_DBG_H_*/
