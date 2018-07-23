/*********************  P r o g r a m  -  M o d u l e ***********************
 *
 *         Name: mk_calls.c
 *      Project: MDIS4LINUX
 *
 *       Author: kp
 *        $Date: 2009/08/04 16:49:20 $
 *    $Revision: 1.4 $
 *
 *  Description: Handle the MK Get/Setstats
 *
 *     Required: -
 *     Switches: DBG
 *
 *-------------------------------[ History ]---------------------------------
 *
 * $Log: mk_calls.c,v $
 * Revision 1.4  2009/08/04 16:49:20  CRuff
 * R: make 64bit compatible
 * M: 1. change function signature for MDIS_MkGetStat
 *       (use INT32_OR_64* for valueP)
 *    2. change type casts from int32 to INT32_OR_64 where necessary
 *
 * Revision 1.3  2008/08/22 12:23:31  dpfeuffer
 * R: M_ADDRSPACE struct in mdis_api.h: native_int replaced by U_INT32_OR_64
 * M: native_int casts replaced by U_INT32_OR_64
 *
 * Revision 1.2  2003/02/21 13:34:37  kp
 * added some comments
 *
 * Revision 1.1  2001/01/19 14:58:33  kp
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2000 by MEN mikro elektronik GmbH, Nuremberg, Germany
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

#include "mk_intern.h"

/*---------------+
|  Defines       |
+---------------*/
/* add idStr+NL to idBuf if doExec */
#define ADD_STRING(idStr, idBuf, doExec)    { \
				if (doExec) strcpy(idBuf,idStr); \
				idBuf += strlen(idStr); \
				if (doExec) strcpy(idBuf,"\n"); \
				idBuf += strlen("\n"); \
				}


/*---------------+
|  Prototypes    |
+---------------*/
static int32 GetRevId(MK_DEV *dev, char *buf, u_int32 *size, u_int32 doExec);


/******************************** MDIS_MkSetStat *****************************
 *
 *  Description: Handle MK setstat code
 *			
 *  Used by both Linux and RTAI implementation
 *			
 *---------------------------------------------------------------------------
 *  Input......: mkPath	  MK path structure
 *				 code	  status code
 *               arg	  for std codes: value
 *						  for blk codes: M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_MkSetStat( MK_PATH *mkPath, u_int32 code, void *arg )
{
	int32 error = 0;
	// value is never interpreted as a pointer in this function
	int32 value = (int32)(INT32_OR_64)arg;
	MK_DEV *dev = mkPath->dev;

	DBGWRT_2((DBH," M_MK_xxx code\n"));
	
	/*------------------------------+
	| handle code                   |
	+------------------------------*/
    switch (code) {
        case M_MK_CH_CURRENT:
		    if (!IN_RANGE(value, 0, dev->devNrChan-1))
				return(ERR_MK_ILL_PARAM);

            mkPath->chan = value;
            break;

        case M_MK_IO_MODE:
		    if (!(value==M_IO_EXEC || value==M_IO_EXEC_INC))
				return(ERR_MK_ILL_PARAM);

            mkPath->ioMode = value;
            break;

        case M_MK_IRQ_ENABLE:
		    if (!IN_RANGE(value, 0, 1))
				return(ERR_MK_ILL_PARAM);

		    error = MDIS_EnableIrq(mkPath->dev, value);
            break;

        case M_MK_IRQ_COUNT:
		    dev->irqCnt = value;
            break;

        case M_MK_DEBUG_LEVEL:
		    DBG_MYLEVEL = value;
            break;

        case M_MK_API_DEBUG_LEVEL:
		    /* nothing to do */
            break;

        case M_MK_OSS_DEBUG_LEVEL:
		    OSS_DbgLevelSet(dev->osh, value);
            break;

        /*---------------------------+
        |  other codes               |
        +---------------------------*/
        default:
			error = ERR_MK_UNK_CODE;
            break;
    }

	return(error);
}

/******************************** MDIS_MkGetStat *****************************
 *
 *  Description: Handle MK getstat code
 *			
 *  Used by both Linux and RTAI implementation
 *			
 *---------------------------------------------------------------------------
 *  Input......: lu		  unit storage
 *				 code	  status code
 *               pd       path descriptor
 *  Input......: mkPath	  MK path structure
 *				 code	  status code
 *               arg	  for std codes: ptr to value
 *						  for blk codes: M_SG_BLOCK struct
 *  Output.....: return   success (0) or error code
 *  Globals....:
 ****************************************************************************/
int32 MDIS_MkGetStat(MK_PATH *mkPath, int32 code, INT32_OR_64 *valueP)
{
	int32 error = 0;
	M_SG_BLOCK *blk = (M_SG_BLOCK*)valueP;
	MK_DEV *dev = mkPath->dev;

	DBGWRT_2((DBH," M_MK_xxx code\n"));
	
	/*------------------------------+
	| handle code                   |
	+------------------------------*/
    switch (code) {
		/*---------------------+
        |  STD codes           |
        +---------------------*/
        case M_MK_NBR_ADDR_SPACE:
		    *valueP = dev->spaceCnt;
            break;

        case M_MK_CH_CURRENT:
            *valueP = mkPath->chan;
            break;

        case M_MK_IO_MODE:
            *valueP = mkPath->ioMode;
            break;

        case M_MK_IRQ_ENABLE:
		    *valueP = dev->irqEnable;
            break;

        case M_MK_IRQ_COUNT:
		    *valueP = dev->irqCnt;
            break;

        case M_MK_DEBUG_LEVEL:
		    *valueP = DBG_MYLEVEL;
            break;

        case M_MK_API_DEBUG_LEVEL:
		    *valueP = DBG_OFF;
            break;

        case M_MK_OSS_DEBUG_LEVEL:
		    *valueP = OSS_DbgLevelGet(dev->osh);
            break;

        case M_MK_LOCKMODE:
		    *valueP = dev->lockMode;
            break;

        case M_MK_PATHCNT:
		    *valueP = dev->useCount;
            break;

        case M_MK_DEV_SLOT:
		    *valueP = dev->devSlot;
            break;

        case M_MK_DEV_ADDRMODE:
		    *valueP = dev->devAddrMode;
            break;

        case M_MK_DEV_DATAMODE:
		    *valueP = dev->devDataMode;
            break;

        case M_MK_BUSTYPE:
		    *valueP = dev->busType;
            break;

        case M_MK_IRQ_INFO:
		    *valueP = dev->irqInfo;
            break;

        case M_MK_IRQ_MODE:
		    *valueP = dev->irqMode;
            break;

        case M_MK_IRQ_INSTALLED:
		    *valueP = dev->irqInstalled;
            break;

        case M_MK_TICKRATE:
		    *valueP = OSS_TickRateGet(dev->osh);
            break;

	    case M_MK_REV_SIZE:
			/* get string size */
			if ((error = GetRevId(dev, NULL, (u_int32*)valueP, FALSE)))
				return(error);
            break;

		/*---------------------+
        |  BLK codes           |
        +---------------------*/
        case M_MK_BLK_PHYSADDR:
		{
			M_ADDRSPACE *sp = (M_ADDRSPACE*)blk->data;

			if (blk->size < sizeof(M_ADDRSPACE))	/* check user buf */
				return(ERR_MK_USERBUF);

		    if (sp->space >= dev->spaceCnt)			/* check space index */
				return(ERR_MK_ILL_PARAM);

			sp->addr = (U_INT32_OR_64)dev->space[sp->space].physAddr;
			sp->size = (U_INT32_OR_64)dev->space[sp->space].reqSize;
            break;
		}

        case M_MK_BLK_VIRTADDR:
		{
			M_ADDRSPACE *sp = (M_ADDRSPACE*)blk->data;

			if (blk->size < sizeof(M_ADDRSPACE))	/* check user buf */
				return(ERR_MK_USERBUF);

		    if (sp->space >= dev->spaceCnt)			/* check space index */
				return(ERR_MK_ILL_PARAM);

			sp->addr = (U_INT32_OR_64)dev->space[sp->space].virtAddr;
			sp->size = (U_INT32_OR_64)dev->space[sp->space].availSize;
            break;
		}


        case M_MK_BLK_BB_HANDLER:
		{
			u_int32 size = OSS_StrLen(dev->osh, dev->brdName) + 1;

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);			

			OSS_MemCopy(dev->osh, size, dev->brdName, (char*)blk->data);
			blk->size = size;
            break;
		}

        case M_MK_BLK_DEV_NAME:
		{
			u_int32 size = OSS_StrLen(dev->osh, dev->devName) + 1;

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);			

			OSS_MemCopy(dev->osh, size, dev->devName, (char*)blk->data);
			blk->size = size;
            break;
		}

        case M_MK_BLK_REV_ID:
		{
			u_int32 size;

			/* get string size */
			if ((error = GetRevId(dev, NULL, &size, FALSE)))
				return(error);

			if (blk->size < size)		/* check user buf */
				return(ERR_MK_USERBUF);			

			/* create strings */
			if ((error = GetRevId(dev, (char*)blk->data, &size, TRUE)))
				return(error);

			blk->size = size;
            break;
		}

        /*---------------------------+
        |  other codes               |
        +---------------------------*/
        default:
			error = ERR_MK_UNK_CODE;
            break;
    }

	return(error);
}

/******************************* GetRevId *********************************
 *
 *  Description:  Create revision ident string and/or calculate size
 *
 *---------------------------------------------------------------------------
 *  Input......:  dev	  MK device struct
 *				  buf     destination string buffer or NULL
 *                doExec  0 = only size is returned
 *                        1 = string is created in buf
 *  Output.....:  size    size of ident string
 *                return  pointer to ident string
 *  Globals....:  -
 ****************************************************************************/
static int32 GetRevId(MK_DEV *dev, char *buf, u_int32 *size, u_int32 doExec)
{
	MDIS_IDENT_FUNCT_TBL *funcTbl;
	char *idBuf = buf;
	int32 error;
	u_int32 i;

	DBGWRT_1((DBH," GetRevId: exec=%d\n",doExec));

	/*-----------------------------+
    |  MK ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ MDIS Kernel ]-----", idBuf, doExec);
	ADD_STRING(MDIS_Ident(), idBuf, doExec);	/* Kernel */

	/*-----------------------------+
    |  LL ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ Low Level Driver ]-----", idBuf, doExec);

	/* get function table ptr */
    if ((error = dev->llJumpTbl.getStat(dev->ll, M_MK_BLK_REV_ID, 0,
									   (INT32_OR_64*)&funcTbl))) {
		DBGWRT_ERR((DBH," *** GetRevId: can't get LL ident table\n"));
		return(error);
	}

	DBGDMP_3((DBH," functTbl",funcTbl,sizeof(MDIS_IDENT_FUNCT_TBL),4));

	/* call ident functions */
    for (i=0; i<=MDIS_MAX_IDCALLS; i++) {
        if (funcTbl->idCall[i].identCall == NULL )		/* last entry ? */
           break;										/* all done */

		ADD_STRING(funcTbl->idCall[i].identCall(), idBuf, doExec);
	}

	/*-----------------------------+
    |  BK ident strings            |
    +-----------------------------*/
	ADD_STRING("-----[ BBIS Kernel ]-----", idBuf, doExec);
	ADD_STRING(bbis_ident(), idBuf, doExec);	/* Kernel */

	ADD_STRING("-----[ BBIS Driver ]-----", idBuf, doExec);

	if( (error = dev->brdJumpTbl.getStat( dev->brd, 0, M_MK_BLK_REV_ID,
										  (INT32_OR_64*)&funcTbl ))) {
		DBGWRT_ERR((DBH," *** GetRevId: can't get BB ident table\n"));
		return(error);
	}

    for (i=0; i<=MDIS_MAX_IDCALLS; i++) {
        if (funcTbl->idCall[i].identCall == NULL )		/* last entry ? */
           break;										/* all done */

		ADD_STRING(funcTbl->idCall[i].identCall(), idBuf, doExec);
	}

	/* NULL terminator */
	if (doExec) *idBuf = 0;
	idBuf++;

	/* calculate size */
	*size = idBuf-buf;
	DBGWRT_1((DBH,"MK - GetRevId end: size=%d exec=%d\n",*size, doExec));

	return(ERR_SUCCESS);
}


