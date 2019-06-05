/***********************  I n c l u d e  -  F i l e  ************************/
/*!
 *        \file  rtagent.h
 *
 *      \author  klaus.popp@men.de
 *
 *  	 \brief  definitions for the "MDIS example execution environment"
 *				 (mexe) for RTAI.
 *
 *     Switches: -
 */
/*
 *---------------------------------------------------------------------------
 * Copyright (c) 2003-2019, MEN Mikro Elektronik GmbH
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

#ifndef _RTAGENT_H
#define _RTAGENT_H

/* RTA_SDB.primitive codes */
#define RTA_REQ 1
#define RTA_RSP 2

/* RTA_SDB.service codes */
#define RTA_NOSERVICE	0
/* mdis_rtagent -> RT-app */
#define RTA_START		1			
#define RTA_STOP		2

/* RT-app -> mdis_rtagent */
#define RTA_FPRINTF 	10
#define RTA_WAITKEY		11
#define RTA_CHECKKEY	12
#define RTA_FGETS		13
#define RTA_GETC		14
#define RTA_FOPEN		15
#define RTA_FCLOSE		16
#define RTA_FREAD		17
#define RTA_FWRITE		18
#define RTA_FEOF		19
#define RTA_FERROR		20
#define RTA_CLEARERR	21
#define RTA_FSEEK		22
#define RTA_FTELL		23
#define RTA_DONE		99

/* RTA_SDB.u.rsp.result */
#define RTA_OK				0
#define RTA_BAD_SERVICE		1
#define RTA_BAD_PARAMETER 	2

#define RTA_X 0					/* flags don't care */

#define RTA_SDB_MAGIC 0x8723872A

/** Union that can hold different data types */
typedef union {
	u_int32 u32;
	int i;
	long l;
	void *vp;
} RTA_PARAM;

/** Service description block */
typedef struct {
	u_int32 magic;
	u_int16 primitive;
	u_int16 service;
	u_int32 dataLen;
	union {
		struct {
			int errNum;
			RTA_PARAM param;
			u_int8 result;
		} rsp;
		struct {
			RTA_PARAM param1;
			RTA_PARAM param2;
		} req;
	} u;

} RTA_SDB;

/** fill SDB request structure */
#define RTA_SDB_REQ( _sdb, _serv, _dlen ) \
 (_sdb)->magic = RTA_SDB_MAGIC; \
 (_sdb)->primitive = RTA_REQ; \
 (_sdb)->service = _serv; \
 (_sdb)->dataLen = _dlen; \
 (_sdb)->u.req.param1.u32 = 0; \
 (_sdb)->u.req.param2.u32 = 0;


/*--- service data blocks ---*/
#define RTA_MAX_DATALEN 512

#ifdef __KERNEL__
/* rtai_mexe.c */
int MexeService(
	const RTA_SDB *reqSdb,
	const void *reqData,
	RTA_SDB *rspSdb,
	void *rspData);


#endif /* __KERNEL__ */

#endif /* _RTAGENT_H */

