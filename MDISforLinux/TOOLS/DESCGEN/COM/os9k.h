/***********************  I n c l u d e  -  F i l e  ************************
 *
 *         Name: os9k.h
 *
 *       Author: kp
 *        $Date: 1999/06/09 16:02:56 $
 *    $Revision: 1.2 $
 *
 *  Description: module definitions for OS-9000 descriptors
 *
 *     Switches: -
 *
 *---------------------------------------------------------------------------
 * Copyright (c) 1999-2019, MEN Mikro Elektronik GmbH
 ******************************************************************************/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _OS9K_H
#define _OS9K_H

#ifdef __cplusplus
	extern "C" {
#endif

/*--------------------------------------+
|   DEFINES                             |
+--------------------------------------*/
#define MODSYNC_PPC 0xf00d /* module header sync code for PowerPC processors */
#define MODSYNC_X86 0x4afc /* module header sync code for 80386 processors */
#define MODNAME(mod) ((u_int8*)((u_int8*)mod + ((Mh_com)mod)->m_name))
#define MODSIZE(mod) ((u_int32)((Mh_com)mod)->m_size)

/*--------------------------------------+
|   TYPDEFS                             |
+--------------------------------------*/

typedef struct scf_path_desc	*Scf_path_desc;
typedef struct scf_path_opts	*Scf_path_opts;
typedef struct scf_lu_stat		*Scf_lu_stat;
typedef struct scf_lu_opts		*Scf_lu_opts;

typedef struct mh_com {
	u_int16
		m_sync,			/* sync bytes       								*/
		m_sysrev;		/* system revision check value						*/
	u_int32
		m_size;			/* module size										*/
	u_int32
		m_owner;		/* group/user id									*/
	u_int32
		m_name;			/* offset to module name							*/
	u_int16
		m_access,		/* access permissions								*/
		m_tylan,		/* type/lang										*/
		m_attrev,		/* attributes/revision   							*/
		m_edit;			/* edition											*/
	u_int32
		m_needs,		/* module hardware requirements flags  (reserved)	*/
		m_share,		/* shared data offset								*/
		m_symbol,		/* symbol table offset								*/
		m_exec,			/* offset to execution entry point					*/
		m_excpt,		/* offset to exception entry point					*/
		m_data,			/* data storage requirement							*/
		m_stack,		/* stack size										*/
		m_idata,		/* offset to initialized data						*/
		m_idref,		/* offset to data reference lists					*/
		m_init,			/* initialization routine offset					*/
		m_term;			/* termination routine offset						*/
	u_int32
		m_dbias,		/* data area bias									*/
		m_cbias;		/* code area bias									*/
	u_int16
		m_ident;		/* ident code for ident program						*/
	char
		m_spare[8];		/* reserved bytes									*/
	u_int16
		m_parity; 		/* header parity									*/
} mh_com, *Mh_com;

/* Device descriptor data definitions */
typedef struct {
	u_int32
		dd_port;		/* device port address 			*/
	u_int16
		dd_lu_num,		/* logical unit number 			*/
		dd_pd_size,		/* path descriptor size 		*/
		dd_type,		/* device type 					*/
		dd_mode;		/* device mode capabilities 	*/
	u_int32
		dd_fmgr,		/* file manager name offset 	*/
		dd_drvr;		/* device driver name offset	*/
	u_int16
		dd_class,		/* sequential or random 		*/
		dd_dscres;		/* (reserved) 					*/
} *Dd_com, dd_com;


/* MDISFM file manager static storage.	*/
typedef struct mdisfm_fm_stat{
	u_int32
		func_count;
	int32
		(*v_attach)(),	/* address of Mdisfm's "attach" function */
		(*v_chgdir)(),	/* address of Mdisfm's "chgdir" function */
		(*v_close)(),	/* address of Mdisfm's "close" function */
		(*v_create)(),	/* address of Mdisfm's "create" function */
		(*v_delete)(),	/* address of Mdisfm's "delete" function */
		(*v_detach)(),	/* address of Mdisfm's "detach" function */
		(*v_dup)(),		/* address of Mdisfm's "dup" function */
		(*v_getstat)(),	/* address of Mdisfm's "getstat" function */
		(*v_makdir)(),	/* address of Mdisfm's "makdir" function */
		(*v_open)(),	/* address of Mdisfm's "open" function */
		(*v_read)(),	/* address of Mdisfm's "read" function */
		(*v_readln)(),	/* address of Mdisfm's "readln" function */
		(*v_seek)(),	/* address of Mdisfm's "seek" function */
		(*v_setstat)(),	/* address of Mdisfm's "setstat" function */
		(*v_write)(),	/* address of Mdisfm's "write" function */
		(*v_writeln)();	/* address of Mdisfm's "writeln" function */
} mdisfm_fm_stat;

/* MDISFM driver static storage. */
typedef struct mdisfm_drvr_stat{
	int32
		(*v_getentry)();	/* address of driver's "getentry" function */

} mdisfm_drvr_stat;


/* MDISFM unit static storage */
typedef struct mdisfm_lu_stat{
	u_int32 mdisDescOffs;	/* offset to mdis descriptor data in devDesc */
	/* u_int8	data[1024]; */
} mdisfm_lu_stat;

/* Path descriptor common header definition */
typedef struct pathcom {
	u_int32
		pd_id;			/* path number 							*/
	void *
		pd_dev;			/* device list element pointer 			*/
	u_int32
		pd_own;			/* group/user number of path's creator 	*/
	struct pathcom
		*pd_paths,		/* list of open paths on device 		*/
		*pd_dpd;		/* default directory path 				*/
	u_int16
		pd_mode,		/* mode (read/write/update) 			*/
		pd_count,		/* actual number of open images 		*/
		pd_type,		/* device type 							*/
		pd_class;		/* device class (sequential or random) 	*/
	u_int32
		pd_cproc;		/* current process id 					*/
	u_int8
		*pd_plbuf,		/* pointer to partial pathlist 			*/
		*pd_plist;		/* pointer to complete pathlist 		*/
	u_int32
		pd_plbsz;		/* size of pathlist buffer 				*/
	void *
		pd_lock[4];		/* Queue ID for path descriptor 		*/
	void
		*pd_async;		/* asynchronous I/O resource pointer	*/
	u_int32
		pd_state,		/* status bits (e.g. PD_SYSTATE)		*/
		pd_rsrv[7];		/* reserved space 						*/
} pd_com,*Pd_com;


/* MDISFM complete path descriptor structure */
typedef struct mdisfm_path_desc {
	struct pathcom
		pd_common;		/* common path descriptor structure */

	u_int32 chan;		/* current MDIS channel */
	u_int32	ioMode;		/* current MDIS i/o mode */
} mdisfm_path_desc;

/* MDISFM Device descriptor module data area */
typedef struct mdisfm_desc {
	dd_com
		dd_descom;		/* common device descriptor variables */
} mdisfm_desc;

/*--------------------------------------+
|   EXTERNALS                           |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   GLOBALS                             |
+--------------------------------------*/
/* none */

/*--------------------------------------+
|   PROTOTYPES                          |
+--------------------------------------*/
/* none */


#ifdef __cplusplus
	}
#endif

#endif	/* _OS9K_H */
