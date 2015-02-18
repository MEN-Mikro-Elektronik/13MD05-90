/************************  F P G A _ L O A D  *********************************/
/*!
 *
 *        \file  fpga_load.c
 *
 *      \author  Christian.Schuster@men.de
 *        $Date: 2014/02/04 13:10:05 $
 *    $Revision: 1.28 $
 *
 *       \brief  Tool to load FPGA configurations into flash over PCI bus
 *
 *
 *     Required: uti.l (under OS-9), libuti.a (under Linux)
 *
 *
 */
/*---------------------------[ Public Functions ]-------------------------------
 * none
 *
 *---------------------------------[ History ]----------------------------------
 *
 * $Log: fpga_load.c,v $
 * Revision 1.28  2014/02/04 13:10:05  awerner
 * R: Minor Revision still displayed wrong
 * M: Added seperate filed in CHAMELEONV2_TABLE for minor Revision
 *    and add it to fpga_load.
 *
 * Revision 1.27  2014/01/20 10:20:24  awerner
 * R: 1. Wrong flash type in combination with -d (direct specify flash address)
 *       caused fpga flash corruption
 *    2. Default Write Offset mentioned in usage is wrong.
 * M: 1. Added parameter for seriell and parallel (-z -j) flash. The parameter
 *       MUST be specified.
 *    2. Change default offset in WriteBlock from 0x40000 to 0x00
 *
 * Revision 1.26  2014/01/09 15:37:26  awerner
 * R: busId interpreted as Minor FPGA Revision
 * M: Changed printf to print Minor Revision instead of busID
 *
 * Revision 1.25  2013/06/21 16:43:22  MRoth
 * R: no support for Windows7 64bit
 * M: added WIN64 specific address mapping
 *
 * Revision 1.24 2013/04/18 10:09:06  ts
 * R: printed revision string looked chopped off
 * M: corrected printf in usage text
 *
 * Revision 1.23  2012/09/10 16:41:13  sy
 * Add OSS_VXBUS_SUPPORT to support multiple PCI domains
 *
 * Revision 1.22  2012/03/14 13:26:18  dpfeuffer
 * R:1. FPGA@LPCbus introduction (SC24)
 *   2. usage information dubious
 *   3. console output inconsistent
 *   4. option -u doesn't work with serial flash
 * M:1. option -k <tbl-addr> added to support access over ISA/LPC bus
 *   2. usage information completely revised
 *   3. printfs revised
 *   4. option -u copies FPGA header from file for serial flashes
 *
 * Revision 1.21  2012/03/09 15:47:23  ts
 * R: 1. verify failed on EM10A with file EM10AIC002A2.bin
 *    2. unresolved symbol CHAM_InitMemSw
 * M: 1. bugfix in flash verify: loop must end < len instead <= len
 *    2. make call of function dependent if
 *
 * Revision 1.20  2010/08/26 09:57:00  CKauntz
 * R: PCI Config Space Command Register not reset after changes
 * M: Added Reset command register to the old value
 *
 * Revision 1.19  2009/03/05 09:52:43  CKauntz
 * R: Execution order incorrect at BIG Endian SMB flash type
 * M: Added switches for BIG and LITTLE Endian at SMB flas types
 *
 * Revision 1.18  2009/01/22 17:55:16  CKauntz
 * Added Dummy_Routine to solve compiler warnings and add
 *   OSS_AlarmCreate and OSS_SmbHdl to MDIS object for VxWorks BSPs
 *
 * Revision 1.17  2008/07/30 15:54:11  gvarlet
 * R: No support for QNX
 * M: Support for QNX added
 *
 * Revision 1.16  2008/06/19 17:09:28  CKauntz
 * R: Verify_FpgaConfig just compared first index
 * M: Fixed buffer incremets to compare the whole buffer
 *
 * Revision 1.15  2008/05/09 19:26:45  CKauntz
 * Added:   Parameter -z to switch between parallel and spi flash types
 * Changed: Parameter -z determines the IP Core 16Z045_FLASH or 16Z126_SPI_FLACH
 * Fixed:   Go to end when no Chameleon device found
 *
 * Revision 1.14  2008/03/19 16:10:54  CKauntz
 * added:
 *  + STM25P32_Trys
 *  + flash interface 16Z126_SPI_FLASH
 *
 * Revision 1.13  2007/07/13 17:26:34  cs
 * removed option to access SMBus over OSS_GetSmbHdl when used in user space
 * cosmetics
 *
 * Revision 1.12  2007/07/09 20:10:45  CKauntz
 *  + changed: renamed goto end commands
 *             Chameleon table outlook
 *             Decision 8Bit, 16Bit moved to AD29LVxxx_TRY function
 *  + added :  Z100_ISTRATAPC28FXXXP30_TRY versions,
 * 	    Option -t for a list of all Chameleon tables
 * 	    Option -d for direct addressing
 *
 * Revision 1.11  2006/09/18 11:34:39  cschuster
 * added error check for WriteBlock
 *
 * Revision 1.10  2006/03/24 14:59:57  cschuster
 * print return value of CHAM.InitPci on error
 *
 * Revision 1.9  2006/02/08 12:24:22  cschuster
 * added option to set bus switches of A500 (-x)
 *
 * Revision 1.8  2005/12/12 14:29:49  cschuster
 * changed FLASH_READ/WRITE macros to Z100_FLASH_READ/WRITE functions
 * moved VME and PCI init out of main function
 * added support for am29lvxxx_smb (flash access over SMBus and PLD)
 * added casts to avoid warnings with VxWorks compiler
 * added Chameleon Lib exit call (Term)
 * docu: added references between related functions
 * Copyright string changed
 *
 * Revision 1.7  2005/07/14 12:27:47  cs
 * minor bugfixes
 * cosmetics
 * OSS_DbgLevel set considering the verbose level (0/1)
 *
 * Revision 1.6  2005/07/08 17:49:42  cs
 * use MACCESS macros and chameleon (+oss_usr, +dbg_usr) library
 * add VME bus support
 * cosmetics
 *
 * Revision 1.5  2005/01/31 13:58:11  cs
 * changed parameters -ec to -c and -es to -e
 * added casts (printf + sscanf) to avoid warnings when compiling VxWorks tool
 * added CHAM_ENT structure to DEV_HDL structure
 * cosmetics
 *
 * Revision 1.4  2005/01/21 13:36:44  cs
 * added support for new chameleon structure (magic 0xCDEF)
 * bugfixes:
 *   vendor and device ID of PCI devices where printed in decimal form
 *   parameters are now checked before beeing parsed
 * additional error handling
 * cosmetics
 * changed parameter -bar to -b
 * added VxWorks parameters -m (pci to memory offset) and -i (pci to io offset)
 *
 * Revision 1.3  2004/12/23 15:10:38  cs
 * minor bugfixes
 * cosmetics
 * removed duplicate code
 *
 * Revision 1.2  2004/12/13 18:03:30  cs
 * bugfix: if config Nr. is omitted with -u option, 0 is assumed correctly
 * cosmetics in documentation and debug messages
 *
 * Revision 1.1  2004/11/30 18:04:53  cs
 * Initial Revision
 *
 *
 *------------------------------------------------------------------------------
 * (c) Copyright 2004-2005 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ******************************************************************************/

static char RCSid[]="$Header: /dd2/CVSR/COM/TOOLS/FPGA_LOAD/COM/fpga_load.c,v 1.28 2014/02/04 13:10:05 awerner Exp $";

#include "fpga_load.h"
#include "fpga_load_flash.h"
#include "MEN/smb2.h"

/*-----------------------------------------+
 |  GLOBALS                                |
 +-----------------------------------------*/

static FLASH_TRYP fpga_flash_trys[] = {
	/* for some reason the P18 FPGA (PCI) hangs completely, after
	   the first access to the flash, when the AM29LVXXX_TRY functions
	   are called in the wrong order.
	   we still leave the other version in there just in case the problem
	   is limited to the P18 and works on other boards already, e.g. when
	   the FPGA is on another bus than PCI.
	  */

#ifdef _BIG_ENDIAN_
	&Z100_AM29LVXXX_TRY_sw,
	&Z100_AM29LVXXX_TRY,
#else
	&Z100_AM29LVXXX_TRY,
	&Z100_AM29LVXXX_TRY_sw,
#endif /* _BIG_ENDIAN_ */
#ifdef Z100_CONFIG_SMB
    #ifdef _BIG_ENDIAN_
    	&Z100_AM29LVXXX_SMB_TRY_sw,
    	&Z100_AM29LVXXX_SMB_TRY,
    #else
        &Z100_AM29LVXXX_SMB_TRY,
    	&Z100_AM29LVXXX_SMB_TRY_sw,
    #endif /* _BIG_ENDIAN */
#endif /* Z100_CONFIG_SMB */
	&Z100_ISTRATAPC28FXXXP30_TRY_sw,
	&Z100_ISTRATAPC28FXXXP30_TRY_mmod,
	&Z100_ISTRATAPC28FXXXP30_TRY_mmod_sw,
	&Z100_ISTRATAPC28FXXXP30_TRY,
	&Z100_STM25P32_TRY,
	&Z100_STM25P32_TRY_sw,
	0
};

#ifdef WINNT
	HANDLE	*G_pHga;
#endif

/*-----------------------------------------+
 |  DEFINES                                |
 +-----------------------------------------*/
#define FPGA_P_HELP "           type fpga_load -h for help\n"

#ifdef DBG
#	define Z100_OSS_DBG_LEVEL	DBG_LEVERR | DBG_NORM | 0x01
#else
#	define Z100_OSS_DBG_LEVEL	DBG_LEVERR
#endif

#define MAP_REG_SIZE 0x10 /* amount of memory space to map */
#define SHOW_ALL_CHAMTABLES	0xff	/*  shows all Chameleon Tables */

#ifdef OSS_VXBUS_SUPPORT	
IMPORT VXB_DEVICE_ID sysGetMdisBusCtrlID(void);
#endif
/*-----------------------------------------+
 |  TYPEDEFS                               |
 +-----------------------------------------*/
/*-----------------------------------------+
 |  PROTOTYPES                             |
 +-----------------------------------------*/

static int Erase_chip( int argc, char* argv[], DEV_HDL *h, u_int8 *retCnt);
static int Erase_sectors( int argc, char* argv[], DEV_HDL *h, u_int8 *retCnt);
static int Read_block( int argc,
					   char* argv[],
					   DEV_HDL *h,
					   OSS_HANDLE *osHdl,
					   u_int8 *retCnt);
static int Write_block( int argc,
						char* argv[],
						DEV_HDL *h,
						OSS_HANDLE *osHdl,
						u_int8 *retCnt);
static int Set_BusSwitch( int largc,
						  char* largv[],
						  DEV_HDL *h,
						  OSS_HANDLE *osHdl,
						  u_int8 *retCnt );
static void Trigger_fpga_load(DEV_HDL *h);
static int Update_fpga( int argc,
						char* argv[],
						DEV_HDL *h,
						OSS_HANDLE *osHdl,
						u_int8 *retCnt);
static int Init_Flash( DEV_HDL *h,
					   OSS_HANDLE *osHdl,
					   u_int32 flashInterf );
static int32 Get_file( OSS_HANDLE *osHdl,
					   char* fName,
					   char **buf,
					   u_int32* memGot);
static int32 Put_file(char* fName, char *buf, u_int32 size);
static int32 Get_Chameleon( OSS_HANDLE *osHdl,
					 PCI_DEVS *pciDev,
					 void* tblAddr,
					 u_int16 mod_id,
					 u_int16 mod_group,
					 CHAMELEONV2_INFO *chamInfo,
					 CHAMELEONV2_UNIT *chamUnit,
					 u_int8 verbose );
static int32 FindFlashSect( FLASH_DEVS *fDev,
					  		u_int32 phyAddr,
					  		u_int32 *sectAddr,
					  		u_int32 *sectSize );
static void Get_FpgaHeader( FLASH_DEVS *fDev,
					 		u_int32 offset,
					 		FPGA_LONGHEADER *header );
static int32 Verify_FpgaConfig( DEV_HDL *h,
								OSS_HANDLE *osHdl,
					 			u_int32 offset,
					 			u_int32 len,
					 			u_int8 *buf );
static void Print_PciDeviceInfo( PCI_DEVS **pciDevs,
								 u_int32 numDevs,
								 u_int8 id  );
static int32 FindPciDevice( OSS_HANDLE *osHdl,
							PCI_DEVS *dev,
							PCI_DEVS* allPciDevs[],
							u_int32 *numDevs,
							int show_all );
static int32 Z100_VmeInit( OSS_HANDLE *osHdl,
						   DEV_HDL *h,
						   int argc,
						   char **argv,
						   void** addrWinHdl);
static int32 Z100_VmeExit( OSS_HANDLE *osHdl,
						   void* addrWinHdl);
static int32 Z100_PciInit( OSS_HANDLE *osHdl,
						   DEV_HDL *h,
						   u_int32 flashInterf,
						   int argc,
						   char **argv);
static int32 Z100_PciExit( OSS_HANDLE *osHdl,
						   DEV_HDL *h );
static int32 Z100_IsaInit(	OSS_HANDLE *osHdl,
							DEV_HDL *h,
							u_int32 flashInterf,
							int argc,
							char **argv);
static int32 Z100_SmbInit( OSS_HANDLE *osHdl,
						   DEV_HDL *h );
static int32 Z100_SmbExit( OSS_HANDLE *osHdl,
						   DEV_HDL *h );


/********************************* Usage **************************************/
/** Print program usage
 *
 ******************************************************************************/
static void Usage(void)
{	
	printf("\n\n"
            "Usage   : fpga_load [options]\n"
			"Function: Manage FPGA configurations\n"
			"Options :\n"
			" \n"
			" _____ flash Location _____\n"
			" The flash interface address can be specified directly with\n"
			" options -b/-o (PCI), -a (VME), -d (ISA/LPC).\n"
			" Without option -b/-o (PCI) or with option -k (ISA/LPC), the\n"
			" flash interface address will be taken from the Chameleon table\n"
			" of the specified FPGA device (PCI) or the specified Chameleonn\n"
			" table address (ISA/LPC).\n"
			" \n"
			" ----- PCIbus -----\n"
			" <vendor-id>         PCI vendor ID (e.g. 0x1172)\n"
			" <device-id>         PCI device ID (e.g. 0x4D45 for chameleon_2 dev)\n"
			" <sub-vendor-id>     PCI subsystem vendor ID (e.g. 0x0007 for 15P018)\n"
			" <instance-nbr>      instance number of PCI Chameleon FPGA (see -s)\n"
			" [-b <bar>           PCI BAR number 0..5\n"
			"  -o <offset>]       offset from specified BAR to flash interface\n"
			" -s                  only show all PCI device instances that match to\n"
			"                      <vendor-id>, <device-id>, <sub-vendor-id>\n"
#ifdef VXWORKS
			" -i <offset>         CPU to PCI io-mapped offset                          [0x0]\n"
			" -m <offset>         CPU to PCI memory-mapped offset                      [0x0]\n"
#endif
			"\n"

			"----- VMEbus -----\n"
			" -a <VME-addr>       directly specify VME address of flash interface\n"
			" \n"
			"----- ISA/LPC -----\n"
			" -d <addr>           directly specify address of flash interface,\n"
			"		      !!!ATTENTION: No flash type validation, please specify -z or -j.\n"
			"                      set LSB to 1 for i/o access, e.g. 0xe281\n"
			" -k <tbl-addr>       use Chameleon table at specified address,\n"
			"                      set LSB to 1 for i/o access, e.g. 0xe001\n"
			"\n"
			" _____ flash Type _____\n"
			" -z                  use only SPI Flashes with 16Z126 flash Interface\n"
			" -j		      use only Parallel Flashes\n"
#ifdef Z100_CONFIG_SMB
			" -p <SMB-ctrlr>      use SMB flash interface instead of 16Z045_FLASH,\n"
			"                      list of supported SMB-ctrlr:\n"
			"                      menz001\n"
#endif
			"\n"
			" _____ Safe Actions _____\n"
			" -u <file> <nr>      update FPGA config Nr. <0-3> in flash\n"
			"                      Offset in flash will be read from flash fpga header\n"
			"                      and header info will be validated before update.\n"
			"                      If this option is selected, all others are ignored.\n"
			" -r <start> <len>    read <len> bytes from the flash at offset <start>\n"
			"    [<file>]          If <file> is specified, the data is written\n"
			"                      to the file, otherwise data is printed to stdout\n"
			" -l                  load new FPGA config at end                          [FALSE]\n"
			"\n"
			" _____ Force Mode Actions _____\n"
			" -f <force-cmd>       switch to force moden, (use one of the cmds below)\n"
			"                       !!! please pay attention what you are doing !!!!\n"
			"                       - no header validation will be performed\n"
			"                       - all options must be passed by command line\n"
			"                       - full access to flash commands is granted\n"
			"                       - more than one command can be passed\n"
			"  	                      (they will be executed from first to last)\n"
			"   -c                  erase chip\n"
			"   -e <start> <len>    erase complete blocks, starting with the block\n"
			"                        where offset <start> resides, ending with \n"
			"                        the block that is reached by <len> bytes          [0]\n"
			"   -w <file> [<start>] write <file> to the flash, starting at offset\n"
			"                        <start> (<start> default is 0x00)\n"
			"\n"
			" _____ HW Specifics _____\n"
			" -x <z>              set bus switches of A500 ESM carrier\n"
			"                      z = 1: PCI64 enable;  IO disable;\n"
			"                      z = 2: PCI64 disable; IO enable;\n"
			"                      z = 3: PCI64 disable; IO disable;\n"
			"\n"
			" _____ Misc _____\n"
			"    -t                shows the chameleon table                           [no]\n"
			"    -v                verbose mode                                        [no]\n"
			"    -h / -?           print this help                                     [no]\n"
			"\n"
			"Examples:\n"
			"	LPC FPGA (SC24): \n"
			"		Dump Chameleon Table: fpga_load -k 0xe000e000 -t\n"
			"		Program FPGA File:    fpga_load -k 0xe000e000 -z -f -w <filename> 80000\n"
			"	PCI FPGA (F75P): \n"
			"		Dump Chameleon Table: fpga_load 1a88 4d45 b3 0 -t\n"
			"		Program FPGA File:	  fpga_load 1a88 4d45 b3 0 -z -f -w <filename.rbf> 0\n"
			"\n"
			" WARNING: Please be aware that you do FPGA configuration updates at your own risk.\n"
			"          After an incorrect update your hardware may no longer be accessible.\n"
			"\n"
		   "(c) 2004-2013 MEN Mikro Elektronik GmbH. File revision:\n%s",RCSid);
	printf("\nBuilt: %s %s\n",__DATE__, __TIME__);
}


/********************************* main ***************************************/
/** Program main function
 *
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *
 *  \return	          0
 ******************************************************************************/
int
#ifdef WINNT
_cdecl
#endif
main(int argc, char *argv[ ])
{
	int32 error = 0;
	u_int8 rc, *tempBuf;
	u_int8 do_load_new = 0;
	DEV_HDL *h = NULL;
/*	int be_flag = 0; */
	u_int8 forcemode_en = 0;
	u_int32 devHdlGotSize = 0, i;
	u_int8 *errstr, errbuf[40];
	OSS_HANDLE *osHdl = NULL;
	void* vmeAddrWinHdl;
	u_int32	addrOnVmeBus = 0;
	u_int32 flashInterf = 0; /* 0 = 16Z045_FLASH interace
	                            1 = PLD/SMB flash interface */	
#if defined(WINNT) || defined(LINUX)
	if( OSS_Init( "FPGA_LOAD", &osHdl ) )
#elif defined(VXWORKS)
	if( OSS_Init() )
#elif defined(__QNX__)
	if( 0 != OSS_Init( "FPGA_LOAD", &osHdl, NULL ) )
#endif /* WINNT / LINUX / VXWORKS / QNX */
	{
		printf("\n*** ERROR: initializing OSS\n");
		goto MAINEND;
	}

#ifdef WINNT
	G_pHga = &osHdl->pciGenDevice;
#endif

	if( !(h = (DEV_HDL *)OSS_MemGet( osHdl, sizeof(DEV_HDL), &devHdlGotSize )) )
	{
		printf("\n*** ERROR: allocating memory for device handle\n");
		error = ERR_OSS_MEM_ALLOC;
		goto MAINEND;
	}

	tempBuf = (u_int8*)h;
	for( i=0; i<sizeof(DEV_HDL); i++)
		*tempBuf++ = (u_int8)0;

	h->flashDev.devHdl = h;

	/* parse command line */
	/* find general settings */
#ifdef VXWORKS
	if ((errstr = UTL_ILLIOPT("obadksurltfwv?himecpxzj", errbuf)))
#else
	if ((errstr = UTL_ILLIOPT("obadksurltfwv?hecpxzj", errbuf)))
#endif
	{
		printf("*** ERROR: %s\n" FPGA_P_HELP, errstr);
		error = 1;
		goto MAINEND;
	}

	if( UTL_TSTOPT("?") || UTL_TSTOPT("h") || argc == 1) {
		Usage();
		goto MAINEND;
	}

	h->dbgLevel	  = (UTL_TSTOPT("v") ? 1 : 0);

	if( h->dbgLevel )
		OSS_DbgLevelSet(osHdl, Z100_OSS_DBG_LEVEL );
	else
		OSS_DbgLevelSet(osHdl, DBG_LEVERR );

	/* Z045_FLASH or PLD/SMB interface? */
	if( (flashInterf = (UTL_TSTOPT("p") ? 1 : 0)) )
	{
		int largc = argc;
		char **largv = &argv[0];

		h->smbLocHdl.smbCtlName[0] = 0;

		while( largc ){
			if( !strcmp( "-p", *largv ) ) {
				if (largc < 2 ||
					(!sscanf(largv[1],"%u",(unsigned int*)&h->smbLocHdl.smbCtlNum) &&
					 !sscanf(largv[1],"%s",h->smbLocHdl.smbCtlName)) )
				{
					printf("*** ERROR: option -p requires type/number of SMB "
							"controller\n" FPGA_P_HELP);
					error = ERR_UOS_ILL_PARAM;
					goto MAINEND;
				}
				break;
			}
			largc--;
			largv++;
		}
	}

	/*
	* parallel or seriell flash must be specified
	*/
	if ( !UTL_TSTOPT("z") && !UTL_TSTOPT("j") && !UTL_TSTOPT("t") && !UTL_TSTOPT("s")) {
		printf("*** ERROR: please specify the flash type (-j or -z)\n");
		error = ERR_UOS_ILL_PARAM;
		goto MAINEND;
	/* seriell flash */
	} else if (UTL_TSTOPT("z")) {
		h->interfacespi = 1;
	/* parallel flash */
	} else {
		h->interfacespi = 0;
	}

	/* only init PCI and VME addresses if not SMBus or SMBus with directly
	 * accessing the SMB controller is used */
	if( !flashInterf || *h->smbLocHdl.smbCtlName ) {

		/* VME bus ? */
		if((addrOnVmeBus = (UTL_TSTOPT("a") ? 1 : 0) ))
		{
			error = Z100_VmeInit( osHdl, h, argc, argv, &vmeAddrWinHdl );
		}
		else if ( UTL_TSTOPT("d") )
		{
			int largc = argc;
			u_int32 BusAddr = 0 ;
			char **largv = &argv[0];

			while( largc ){
				if ( !strcmp( "-d", *largv )) {
					if (largc < 2 ||
						(!sscanf(largv[1],"%x",(unsigned int*)&BusAddr)))
					{
						printf("*** ERROR: option -d requires address as hex value\n"
								FPGA_P_HELP);
						error = ERR_UOS_ILL_PARAM;
						goto MAINEND;
					}
					break;
    			}
    			largc--;
    			largv++;
			}

			h->physAddr = (void *)(BusAddr & ~0x1);
			h->busType  = OSS_BUSTYPE_NONE;
			if( BusAddr & 0x1 ){
				#ifndef Z100_IO_ACCESS_ENABLE
					printf("IO mapped access not supported\n");
					goto MAINEND;
				#endif
				h->mapType = OSS_ADDRSPACE_IO;
			}
			else
				h->mapType  = OSS_ADDRSPACE_MEM;
		}
		else if ( UTL_TSTOPT("k") )
		{
			error = Z100_IsaInit( osHdl, h, flashInterf, argc, argv );
		}
		else
		{
			error = Z100_PciInit( osHdl, h, flashInterf, argc, argv );
		}
	}

	if( error ){
		if( error == 100 ){ /* only return error == 100 if show only enabled */
			error = 0;
		}
		goto MAINEND;
	}

	if( (flashInterf == 1) && (error = Z100_SmbInit( osHdl, h )) )
	{
		goto MAINEND;
	}

	if( (error = Init_Flash( h, osHdl, flashInterf )) ) {
		/* flash not initialized and/or init failed */
		printf("*** ERROR: initializing FLASH interface: (0x%x)\n",
			(unsigned int)error);
		goto MAINEND;
	}

	/* simple update? */
	if( UTL_TSTOPT("u") )
	{
		int largc = argc;
		char **largv = &argv[0];
		while( largc ){
			if( !strcmp( "-u", *largv ) ) {
				error = Update_fpga(largc, largv, h, osHdl, &rc);
				goto MAINEND;
			}
			largc--;
			largv++;
		}
		goto MAINEND; /* should never get here */
	}

	/* read only? */
	if( UTL_TSTOPT("r") )
	{
		int largc = argc;
		char **largv = &argv[0];
		while( largc )
		{
			if( !strcmp( "-r", *largv ) )
			{
				if((error = Read_block(largc, largv, h, osHdl, &rc)))
					printf("*** ERROR: ReadBlock failed (0x%x)\n",
							(unsigned int)(error));
				goto MAINEND;
			}
			largc--;
			largv++;
		}
		goto MAINEND;
	}

	if( (flashInterf == 1) && /* SMB/PLD */
		UTL_TSTOPT("x") )      /* want to set bus switches (A500) */
	{
		int largc = argc;
		char **largv = &argv[0];
		while( largc ){
			if( !strcmp( "-x", *largv ) ) {
				if((error = Set_BusSwitch(largc, largv, h, osHdl, &rc)))
					printf("*** ERROR: Set_BusSwitch failed (0x%x)\n",
							(unsigned int)(error));
				goto MAINEND;
			}
			largc--;
			largv++;
		}
		goto MAINEND;
	}

	while( argc > 0 ){
		rc = 0;
		error = 0;

		/* enter force mode or read block */
		if( !forcemode_en ) {
			if( !strcmp( "-f", *argv ) ) {
				forcemode_en = 1;
			}
			argc--;
			argv++;
			continue;
		}

		/* parse functions to perform */
		if( !strcmp( "-l", *argv ) ) {
			do_load_new = 1;
			argc--;
			argv++;
			continue;
		}
		/* erase chip */
		if( !strcmp( "-c", *argv ) ) {
			error = Erase_chip(argc, argv, h, &rc);
			if( !error ) {
				argc -= rc;
				argv += rc;
				continue;
			}
			printf("*** ERROR: EraseChip failed (0x%x)\n",
					(unsigned int)(error));
			goto MAINEND;
		}
		/* erase block */
		if( !strcmp( "-e", *argv ) ) {
			error = Erase_sectors(argc, argv, h, &rc);
			if( !error ) {
				argc -= rc;
				argv += rc;
				continue;
			}
			printf("*** ERROR: EraseSectors failed (0x%x)\n",
					(unsigned int)(error));
			goto MAINEND;
		}
		/* read block */
		if( !strcmp( "-r", *argv ) ) {
			error = Read_block(argc, argv, h, osHdl, &rc);
			if( !error ) {
				argc -= rc;
				argv += rc;
				continue;
			}
			printf("*** ERROR: ReadBlock failed (0x%x)\n",
					(unsigned int)(error));
			goto MAINEND;
		}
		/* write block */
		if( !strcmp( "-w", *argv ) ) {
			error = Write_block(argc, argv, h, osHdl, &rc);
			if( !error ) {
				argc -= rc;
				argv += rc;
				continue;
			}
			printf("*** ERROR: WriteBlock failed (0x%x)\n",
					(unsigned int)(error));
			goto MAINEND;
		}

#ifdef VXWORKS
		if( !strcmp( "-i", *argv ) ||
			!strcmp( "-m", *argv ) ) { /* ignore -i and -m option here */
			argc -= 2;
			argv += 2;
			continue;
		}
#endif

		if( !strcmp( "-v", *argv ) ) { /* ignore -v option here */
			argc--;
			argv++;
			continue;
		}

		/* nothing matched !!!!!!! */
		printf("*** ERROR: unknown parameter: %s\n", *argv);

		argc--; argv++;
	}

	if( do_load_new ) {
		Trigger_fpga_load(h);
	}
MAINEND:
	if( h ) {
		if( h->flashDev.isInit  )
			h->flash_entry.Exit(&h->flashDev);

		if( h->smbLocHdl.smbHdl )
			Z100_SmbExit( osHdl, h );

		if( h->mappedAddr ) {
			if( addrOnVmeBus )
				Z100_VmeExit( osHdl, vmeAddrWinHdl );
			else if ( !UTL_TSTOPT("k") )
				Z100_PciExit( osHdl, h );

#ifdef LINUX
			/* for LINUX, VME addresses are unmapped by OSS_UnMapVmeAddr() */
			if( !addrOnVmeBus )
#endif /* LINUX */
				OSS_UnMapVirtAddr(osHdl,
								  (void**)&h->mappedAddr,
								  h->mappedSize,
								  h->mapType);
		}

		OSS_MemFree( osHdl, h, devHdlGotSize );
	}

#if defined(WINNT) || defined(LINUX)
	OSS_Exit(&osHdl);
#elif defined(VXWORKS)
	OSS_Exit();
#endif /* WINNT || LINUX */
	return( error );
}

#if 0 /* currently not used */
/********************************* Make_hex ***********************************/
/** converts strings to hex numbers
 *
 *  \param argp		\IN		string to convert
 *  \param hexval	\OUT	converted hex value
 *
 *  \return	          success (0) or error (-1)
 ******************************************************************************/
static int Make_hex(char *argp, u_int32 *hexval )
{
	if( sscanf( argp, "%lx", hexval ) != 1 )
		return -1;
	return 0;
}
#endif
/********************************* Erase_chip *********************************/
/** Reads neccessary command line parameters and initiates EraseChip command
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 ******************************************************************************/
static int Erase_chip( int argc, char* argv[], DEV_HDL *h, u_int8 *retCnt )
{
	int32 error = 0;

	*retCnt = 1;
	if( h->dbgLevel )
		printf("Erase_chip\n");

	h->flash_entry.Reset(&h->flashDev);
	error = h->flash_entry.EraseChip(&h->flashDev);

	if( h->dbgLevel && !error )
		printf("----> OK: Chip Erased\n\n");

	return(error);
}

/****************************** Erase_sectors *********************************/
/** reads neccessary command line parameters and initiates EraseSectors command
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 ******************************************************************************/
static int Erase_sectors( int argc, char* argv[], DEV_HDL *h, u_int8 *retCnt )
{
	static u_int32 startOffs = 0;
	u_int32 	len = 0;
	int32 error = 0;

	if( h->dbgLevel )
		printf("Erase_sectors\n");

	if( argc < 3 ||
		!sscanf(argv[1], "%x", (unsigned int*)&startOffs) ||
		!sscanf(argv[2], "%x", (unsigned int*)&len) ) {
		printf("\n*** ERROR: -e requires address in the sector to erase first\n"
				" and length of data to erase (hex values)\n");
		error = ERR_UOS_ILL_PARAM;
		goto ERASEEND;
	}
	*retCnt = 3;

	h->flash_entry.Reset(&h->flashDev);

	DBGOUT(( "Erase_Sectors: startOffs = 0x%08x, length = 0x%06x\n",
			 (int)startOffs, (int)len ));

	if((error = h->flash_entry.EraseSectors(&h->flashDev, startOffs, len)))
		printf("*** ERROR: erase Block failed\n");

ERASEEND:
	if(error){
		h->flash_entry.Reset(&h->flashDev);
	}
	return( error );
}

/******************************* Read_block ***********************************/
/** reads neccessary command line parameters and initiates ReadBlock command
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 ******************************************************************************/
static int Read_block( int argc,
						char* argv[],
						DEV_HDL *h,
						OSS_HANDLE *osHdl,
						u_int8 *retCnt )
{
	u_int32 startOffset = 0, len = 0;
	u_int8 fileName[256], *buf = NULL, putToFile=0;
	int32 error = 0;
	u_int32 bufGotSize = 0;

	*retCnt = 3;

	if( h->dbgLevel )
		printf("Read_block\n");
	if( argc < 3 ||
		!sscanf(argv[1],"%x",(unsigned int*)&startOffset) ||
		!sscanf(argv[2],"%x",(unsigned int*)&len) )
	{
		printf("\n*** ERROR: Option -r requires at least "
				"the offset to start and the length to read\n");
		error = ERR_UOS_ILL_PARAM;
		goto READEND;
	} else if(  argc > 3 &&
				*argv[3] != '-' &&
				sscanf(argv[3],"%s", fileName))
	{
		(*retCnt)++;
		putToFile=1;
	}

 	/* allocate buffer */
	if( !(buf = OSS_MemGet( osHdl, Z100_MAX_FILE_SIZE, &bufGotSize)) ) {
		printf("\n*** ERROR: allocating buffer for verifying flash content\n");
		error = ERR_OSS_MEM_ALLOC;
		goto READEND;
	}

	h->flash_entry.ReadBlock(&h->flashDev, startOffset, len, buf);

	if( putToFile ) {
		if( Put_file( fileName, buf, len ) < 0 )
			printf("*** ERROR: writing read data to file\n");
		else if( h->dbgLevel )
			printf("==> Data put to file %s\n", fileName);
	} else {
		/* print stuff */
		u_int32 i=0;
		char addrstr[20];
		printf("==> Data read(hex):\n ");
		for(i=0; i < len; i++) {
			sprintf(addrstr, "\n 0x%08X: ", (int)(i+startOffset));
			printf("%s%02X ",
					!(i%16) ? addrstr : (!(i%8)&&(i!=0)) ? " " : "",
					(int)*(buf+i));
		}
		printf("\n");
	}

READEND:
	if( error ) {
		printf("*** ERROR: Read_block failed (0x%x)\n", (unsigned int)error);
		h->flash_entry.Reset(&h->flashDev);
	}

	if(buf)
		OSS_MemFree( osHdl, (void*)buf, bufGotSize );

	return( error );
}

/***************************** Write_block ************************************/
/** reads neccessary command line parameters and initiates WriteBlock command
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 *
 *  \sa Update_fpga()
 ******************************************************************************/
static int Write_block
(
	int argc,
	char* argv[],
	DEV_HDL *h,
	OSS_HANDLE *osHdl,
	u_int8 *retCnt
)
{
	u_int32 startOffset = 0;
	u_int8 fileName[256], *fBuf;
	u_int32 fBufGotSize = 0;
	int32 fileSize = 0;
	int32 error = 0;

	*retCnt = 1;

	if( h->dbgLevel )
		printf("Write_block\n");

	if (argc<2 || !sscanf(argv[1],"%s", fileName)) {
		printf("\n*** ERROR: Option -w requires at least the file to be loaded\n");
		error = ERR_UOS_ILL_PARAM;
		goto WRITEEND;
	} else if( (fileSize = Get_file( osHdl,
									 fileName,
									 (char**)&fBuf,
									 &fBufGotSize )) < 0 )
	{
		printf("*** ERROR: illegal path/filename\n");
		error = ERR_UOS_ILL_PARAM;
		goto WRITEEND;
	}
	(*retCnt)++;

	if ( argc > 2 && sscanf(argv[2],"%x",(unsigned int*)&startOffset) ) {
		(*retCnt)++;
	} else {
		/* Default Offset */
		startOffset = 0x00;
	}

	/* erase sectors occupied by requested config */
	if( h->dbgLevel )
		printf("ERASE Sectors: Please wait\n");

	if( (error = h->flash_entry.EraseSectors( &h->flashDev, startOffset,
											  fileSize )) )
		goto WRITEEND;
	else if( h->dbgLevel )
		printf("  ----> OK\n\n");

	h->flash_entry.Reset(&h->flashDev);
	if( h->dbgLevel )
		printf("PROGRAM offset 0x%08x, size 0x%08x: Please wait ...\n", (int)startOffset, (int)fileSize);

    if( (error = h->flash_entry.WriteBlock( &h->flashDev,
											startOffset,
											fileSize,
											fBuf )) )
	{
		goto WRITEEND;
	}

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

	/* verify fpga configurations in flash and in buffer read from file */
	if( (error = Verify_FpgaConfig( h, osHdl, startOffset, fileSize, fBuf )) )
		goto WRITEEND;

	printf("Write and Verify OK\n");

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

WRITEEND:
	if(error) {
		printf("\n*** ERROR: updating flash failed (0x%x),\n"
				"     FPGA will not be reloaded please try again\n"
				"     if resetting now, FPGA may not be able to "
				" initialize correctly\n", (int)error);
		h->flash_entry.Reset(&h->flashDev);
	}

	if(fBuf)
		OSS_MemFree( osHdl, (void*)fBuf, fBufGotSize);
	return( error );
}

/******************************* Trigger_fpga_load ****************************/
/** initiates the reload of the FPGA after all other functions finished
 *
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *	\return -
 ******************************************************************************/
static void Trigger_fpga_load(DEV_HDL *h)
{
	/* set bit 31 of address register to 1 */
	if( h->mapType )
#ifdef Z100_IO_ACCESS_ENABLE
		Z100_Mwrite_Io_D32( h->mappedAddr,
							Z045_FLASH_ADDR_REG_OFFSET,
							0x80000000);
#else /* Z100_IO_ACCESS_ENABLE */
		printf("*** ERROR: Trigger_fpga_load: IO mapped access not supported\n");
#endif /* Z100_IO_ACCESS_ENABLE */
	else
		Z100_MWRITE_D32( h->mappedAddr,
							Z045_FLASH_ADDR_REG_OFFSET,
							0x80000000);
}

/********************************* Update_fpga ********************************/
/** reads neccessary command line parameters and performs the FPGA update
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 *
 *  \sa Write_block()
 ******************************************************************************/
static int Update_fpga( int argc, char* argv[], DEV_HDL *h, OSS_HANDLE *osHdl, u_int8 *retCnt )
{
	u_int32 startOffset = 0;
	u_int8 fileName[256], *fBuf = NULL;
	u_int32 fBufGotSize = 0;
	int32 fileSize = 0;
/*	int32 be_flag = 0; */
	int32 error = 0, i = 0;
	u_int32 fpga_num = 0;
	FPGA_LONGHEADER header, *headerP1;

	*retCnt = 2;

	if( h->dbgLevel )
		printf("Update_fpga\n");
	if (argc<2 ||
		!sscanf(argv[1],"%s", fileName) ){
		printf("\n*** ERROR: option -u requires filename (see \"fpga_load -h\")\n");
		return (ERR_UOS_ILL_PARAM);
	}

	if ( argc==2  ||
		(argc>2 && !sscanf(argv[2],"%u", (unsigned int*)&fpga_num)) ) {
		fpga_num = 0;
	} else
		(*retCnt)++;

	DBGOUT(( "Update_fpga: update configuration %d\n", (int)fpga_num ));

	if( (fileSize = Get_file( osHdl,
							  fileName,
							  (char**)&fBuf,
							  &fBufGotSize)) < 0 )
	{
		printf("*** ERROR: illegal path/filename\n");
		error = ERR_UOS_ILL_PARAM;
		goto UPDATEEND;
	}

	/* seriell flash? */
	if( h->interfacespi ){

		if( fBufGotSize < sizeof(header) ){
			printf("*** ERROR: %s size smaller than header size\n", fileName);
			error = ERR_UOS_ILL_PARAM;
			goto UPDATEEND;
		}

		/* copy FPGA header from file */
		OSS_MemCopy( osHdl, sizeof(header), (char*)fBuf, (char*)&header );
	}
	/* parallel */
	else {
		/* read FPGA header from flash fallback config */
		Get_FpgaHeader( &h->flashDev, 0x00, &header );
	}

	/* don't allow overwriting of the fallback header */
	if( Z100_SWAP_BE32(header.offset[fpga_num]) < 0x20000 ){
		printf("*** ERROR: tried to update fallback configuration\n");
		goto UPDATEEND;
	}

	/* verify header information */
	headerP1 = (FPGA_LONGHEADER*)fBuf;
	if( Z100_SWAP_BE32(header.magic) != FPGA_LONGHEADER_MAGIC ) {
		printf("\n*** ERROR: invalid FPGA header in FLASH\n");
		error = ERR_Z100_ILL_CONFIG_FILE;
		goto UPDATEEND;
	}
	if( Z100_SWAP_BE32(headerP1->magic) != FPGA_LONGHEADER_MAGIC ) {
		printf("\n*** ERROR: invalid FPGA configuration file Header\n");
		error = ERR_Z100_ILL_CONFIG_FILE;
		goto UPDATEEND;
	}
	if( strcmp(header.boardType, headerP1->boardType) ) {
		printf("\n*** ERROR: wrong board type in configuration file\n"
				"          Board Type specified in flash: \"%s\"\n",
				header.boardType);
		error = ERR_Z100_ILL_CONFIG_FILE;
		goto UPDATEEND;
	}

	/* build XOR checksum over FPGA data (and padding bytes) and verify */
	{
		u_int32 *p = (u_int32 *)fBuf + FPGA_SIZE_HEADER_LONG/4, xor;

		for( xor=0,i=0; i<(fileSize-FPGA_SIZE_HEADER_LONG)/4; i++ )
			xor ^= *p++;

		DBGOUT(( "Update_fpga: Checksum: 0x%08x\n",
				 (unsigned int)Z100_SWAP_BE32(xor) ));
		if( headerP1->chksum != xor ) {
			printf("\n*** ERROR: corrupted Fpga configuration file\n");
			error = ERR_Z100_CORRUPT_CONFIG_FILE;
			goto UPDATEEND;
		}
	}

	/* get start Offset of configuration to program */
	startOffset = Z100_SWAP_BE32(header.offset[fpga_num]);

	/* erase sectors occupied by requested config */
	if( h->dbgLevel )
		printf("ERASE Sectors: Please wait\n");
		DBGOUT(( "Update_fpga: start Addr: 0x%08x, size: 0x%08x\n",
			 	(int)startOffset, (int)fileSize ));
	if( (error = h->flash_entry.EraseSectors( &h->flashDev, startOffset,
											  fileSize )) )
		return( -error );
	else if( h->dbgLevel )
		printf("  ----> OK\n\n");

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

	if( h->dbgLevel )
		printf("PROGRAM: Please wait ...\n");
	h->flash_entry.WriteBlock(&h->flashDev, startOffset, fileSize, fBuf );

	/* verify fpga configurations in flash and in buffer read from file */
	if( (error = Verify_FpgaConfig( h, osHdl, startOffset, fileSize, fBuf )) )
		goto UPDATEEND;

	printf("Update OK\n");

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

UPDATEEND:
	if(error) {
		printf("\n*** ERROR: updating flash  failed (0x%x),\n"
				"     FPGA will not be reloaded please try again\n"
				"     if resetting now, FPGA may not be able to "
				" initialize correctly\n", (unsigned int)error);
		h->flash_entry.Exit(&h->flashDev);
	}

	if(fBuf)
		OSS_MemFree( osHdl, (void*)fBuf, fBufGotSize );
	return( error );
}

/****************************** Set_BusSwitch *********************************/
/** Set bus switches (A500) in flash
 *
 *  The A500 has two bus switches which are initialized from the first two bytes
 *  in the flash memory. To set this bus switches this function reads the first
 *  sector, modifies the first two bytes to match the new setting, erases the
 *  first sector and writes the modified data.
 *
 *
 *  \param argc		\IN		argument counter
 *  \param argv		\IN		argument vector
 *  \param h		\IN		DEV_HDL handle
 *  \param osHdl    \IN     valid OSS_HANDLE structure
 *  \param retCnt	\OUT	number of command line parameters used
 *
 *  \return			success (number of parameters used from command line)
 * 					or error (-error)
 ******************************************************************************/
static int Set_BusSwitch( int argc,
						  char* argv[],
						  DEV_HDL *h,
						  OSS_HANDLE *osHdl,
						  u_int8 *retCnt )
{
	u_int32 sectAddr = 0;
	u_int32 sectSize = 0;
/*	int32 be_flag = 0; */
	int32 error = 0;
	int32 retVal = 0;
	u_int8 busSwitch = 0;
	u_int32 bufGotSize = 0;
	u_int8 *buf = NULL;

	*retCnt = 2;

	if( h->dbgLevel )
		printf("Set_BusSwitch\n");

	if ( argc < 2 ||									/* no bus switch?     */
		!sscanf(argv[1],"%x",(unsigned int*)&retVal) || /* switch not number  */
		retVal < 1 || retVal > 3 ){						/* set both switches? */
		printf("\n*** ERROR: option -x requires bus switch [1/2/3] (see \"fpga_load -h\")\n");
		return (ERR_UOS_ILL_PARAM);
	}

	switch(retVal) {
	case 0x1:
		if( h->dbgLevel ) {
			printf("\n");
			printf("\tenable PCI64; disable IO\n");
		}
		retVal = 0xFE;
		break;

	case 0x2:
		if( h->dbgLevel ) {
			printf("\n");
			printf("\tdisable PCI64; enable IO\n");
		}
		retVal = 0xFD;
		break;

	case 0x3:
		if( h->dbgLevel ) {
			printf("\n");
			printf("\tdisable both\n");
		}
		retVal = 0xFF;
		break;
	}

	busSwitch = (u_int8)retVal;

	if( (error == FindFlashSect( &h->flashDev,
						  		 A500_BUS_SWITCH_OFFS,
						  		 &sectAddr,
					  			 &sectSize )) < 0 ) {
		return( 1 );
	}

	/* !!! tbd remove next 2 lines when PLD supports erase of single sectors !!!
	  currently PLD does not support the erase o single sectors. Therefore
	  always read and restore the complete relevant flash content */
	sectAddr = 0x00000000;
	sectSize = A500_RELEVANT_FLASH_CONTENT_SIZE;

	/* allocate buffer, read sector */
	if( !(buf = OSS_MemGet( osHdl, Z100_MAX_FILE_SIZE, &bufGotSize)) ) {
		printf("\n*** ERROR: allocating buffer for saving sector\n");
		error = ERR_OSS_MEM_ALLOC;
		goto SETBUSSWEND;
	}

	h->flash_entry.ReadBlock(&h->flashDev, sectAddr, sectSize, buf);

	/* modify bytes */
	buf[A500_BUS_SWITCH_OFFS] = busSwitch;

	/* delete sector */
	if( h->dbgLevel )
		printf("ERASE Sector: Please wait\n");

	DBGOUT(( "Set_BusSwitch: sectAddr: 0x%08x, sectSize: 0x%08x, BSwitch: 0x%02x\n",
			(unsigned int)sectAddr, (unsigned int)sectSize, busSwitch ));
	if( (error = h->flash_entry.EraseSectors( &h->flashDev, sectAddr,
											  sectSize )) )
		goto SETBUSSWEND;
	else if( h->dbgLevel )
		printf("  ----> OK\n\n");

	/* rewrite sector */
	h->flash_entry.Reset(&h->flashDev);
	if( h->dbgLevel )
		printf("PROGRAM sector: Please wait ...\n");
	h->flash_entry.WriteBlock(&h->flashDev, sectAddr, sectSize, buf );

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

	/* verify fpga configurations in flash and in buffer read from file */
	if( (error = Verify_FpgaConfig( h, osHdl, sectAddr, sectSize, buf )) )
		goto SETBUSSWEND;

	printf("Write and Verify OK\n");

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

SETBUSSWEND:
	if( error ) {
		printf("*** ERROR: Set_BusSwitch failed (0x%x)\n", (unsigned int)error);
		h->flash_entry.Reset(&h->flashDev);
	}

	if(buf)
		OSS_MemFree( osHdl, (void*)buf, bufGotSize );

	return( error );
}


/********************************* Init_Flash *********************************/
/** find flash type used and initiate Flash_init function
 *
 *  \param h		\IN		DEV_HDL handle
 *  \param osHdl		\IN		valid OSS_HANDLE
 *  \param flashInterf	\IN		0 - Z045_FLASH, 1 - PLD/SMB
 *
 *  \return			success (0)	or error (code)
 ******************************************************************************/
static int Init_Flash(
	DEV_HDL *h,
	OSS_HANDLE *osHdl,
	u_int32 flashInterf
)
{
	FLASH_TRYP *Flash_try = &fpga_flash_trys[0];
	FLASH_INITP Flash_init;
	int32 error;

	if(h->dbgLevel)
		printf("Init_Flash\n");

	/* map address of module to user space */
	if( !flashInterf ) /* 16Z045_FLASH  or 16Z126_SPI_FLASH */
	{

#if defined(WINNT) && defined(_WIN64)
		if( h->mapType == OSS_ADDRSPACE_IO ){
			h->mappedAddr = (u_int8*)h->physAddr;
		}
		else
		{
#endif

			if( (error = OSS_MapPhysToVirtAddr(osHdl,
								 h->physAddr,
								 MAP_REG_SIZE,	/* size */
								 h->mapType,
								 h->busType,
								 h->pciDev.bus,
								 (void**)&h->mappedAddr)) )
			{
				printf("*** ERROR: Init_Flash: can't map address 0x%x (0x%x)\n",
						h->physAddr, (int)error);
				return( ERR_NO_SUPPORTED_FLASH_DEVICE_FOUND );
			}

#if defined(WINNT) && defined(_WIN64)
		}
#endif

		h->mappedSize = MAP_REG_SIZE;
		h->smbLocHdl.smbHdl = NULL;


	} else if ( flashInterf == 1 )
	{
		if( !h->smbLocHdl.smbHdl ) {
			printf("*** ERROR: Init_Flash: SMBus Handle not initialized\n");
			return( ERR_NO_SUPPORTED_FLASH_DEVICE_FOUND );
		}
		h->flash_acc_size = Z100_FLASH_ACCESS_16BIT;
	} else {
		printf("*** ERROR: no supported flash interface\n");
	}

	while(*Flash_try){
		/* list of SPI flash types */
	     if(h->interfacespi){
	        if(((*Flash_try)== Z100_STM25P32_TRY) ||
	           ((*Flash_try)== Z100_STM25P32_TRY_sw))
		    {
                if( ((*Flash_try)(h, &Flash_init, h->dbgLevel)) == 0){
        		    break;
                }
        	}
		}else{
            if(((*Flash_try)!= Z100_STM25P32_TRY) &&
	           ((*Flash_try)!= Z100_STM25P32_TRY_sw))
		    {
        		if( ((*Flash_try)(h, &Flash_init, h->dbgLevel)) == 0){
        			break;
        		}
        	}
		}

		Flash_try++;
	}
	if( !(*Flash_try) )  /* reach end of try function array without success */
		return( ERR_NO_SUPPORTED_FLASH_DEVICE_FOUND );

	/* Initialize flash specific functions for found device */
	Flash_init(h);

	return 0;
}

/********************************* Get_file ***********************************/
/** read file
 *
 *  \param fName	\IN		name of file to read
 *  \param buf		\OUT		pointer to buffer to be filled with file data
 *
 *  \return			file size or error (-1)
 ******************************************************************************/
static int32 Get_file
(
	OSS_HANDLE *osHdl,
	char* fName,
	char **buf,
	u_int32 *gotMem
)
{
	size_t fSize=0;
	char* temp_buf;
	FILE *inFp;
	int32 error = 0;

	/*----------------------------------+
	|  Open input file                  |
	+----------------------------------*/
	inFp = fopen( fName, "rb" );
	if( inFp == NULL ){
		printf("*** ERROR: can't open input file\n");
		return( -1 );
	}

	/* Determine size of input file */
	fseek( inFp, 0, SEEK_END );
	fSize = ftell( inFp );
	fseek( inFp, 0, SEEK_SET );

	/* read in file */
	if( (temp_buf = OSS_MemGet( osHdl, fSize, gotMem )) == NULL ){
		printf("*** ERROR: can't allocate buffer\n");
		error = -1;
		goto GETFILEEND;
	}

	if( fread( temp_buf, 1, fSize, inFp ) != (size_t)fSize ){
		printf("*** ERROR: reading file\n");
		error = -1;
		goto GETFILEEND;
	}

GETFILEEND:
	*buf = temp_buf;

	if ( inFp )
		fclose(inFp);

	if ( error == -1 )
		return( error );

	return(fSize);
}

/********************************* Put_file ***********************************/
/** read file
 *
 *  \param fName	\IN		name of file to read
 *  \param buf		\IN		pointer to buffer with file data
 *  \param size		\IN		amount of data to write
 *
 *  \return			success (0) or error (-1)
 ******************************************************************************/
static int32 Put_file(char* fName, char *buf, u_int32 size)
{
	size_t fSize=size;
	FILE *outFp;

	/*----------------------------------+
	|  Open output file                  |
	+----------------------------------*/
	outFp = fopen( fName, "wb" );
	if( outFp == NULL ){
		printf("*** ERROR: can't open output file");
		return(-1);
	}

	if( fwrite( buf, 1, fSize, outFp ) != fSize){
		printf("*** ERROR: error writing data to file");
		return(-1);
	}

	fclose(outFp);
	return(0);
}

/******************************* Get_Chameleon ********************************/
/** Get entry from chameleon table
 *
 * \param osHdl		\IN OSS handle
 * \param pciDev	\IN PCI_DEVS handle or NULL
 * \param tblAddr	\IN table address or NULL
 * \param mod_id	\IN Module Id of module to get
 * \param mod_group	\IN Module group of module to get
 * \param chamInfo	\IN chameleon info to be filled
 * \param chamUnit	\IN chameleon unit to be filled
 * \param verbose	\IN flag, print detailed debug messages
 *
 * \return 0 on success or -1 if failed
 ******************************************************************************/
static int32 Get_Chameleon( OSS_HANDLE *osHdl,
					 PCI_DEVS *pciDev,
					 void* tblAddr,
					 u_int16 mod_id,
					 u_int16 mod_group,
					 CHAMELEONV2_INFO *chamInfo,
					 CHAMELEONV2_UNIT *chamUnit,
					 u_int8 verbose )
{
	u_int32 i;
	int32 ret;

	CHAMELEONV2_HANDLE	*chaHdl;
	CHAM_FUNCTBL		*chaFktTbl = NULL;
	CHAMELEONV2_TABLE	chamTable;
	u_int32 chaUnitNbr = 0;
#ifdef OSS_VXBUS_SUPPORT				
	VXB_DEVICE_ID busCtrlID = sysGetMdisBusCtrlID();
#endif
		
	/* use specified chameleon table address (ISA/LPC) */
	if( tblAddr ){
		CHAM_FUNCTBL chaNoswFktTbl, chaSwFktTbl;
		u_int8 ioAccess = 0;

		/*------------------------------+
		|  CHAM_InitXxx                 |
		+------------------------------*/
		/* io access? */
		if( (u_int32)tblAddr & 0x1 ){
			#ifndef Z100_IO_ACCESS_ENABLE
				printf("*** ERROR: IO mapped access not supported\n");
				goto ERROR_END;
			#endif
			ioAccess = 1;
			tblAddr = (void*)((u_int32)tblAddr & ~0x1);

			if( CHAM_InitIo( &chaNoswFktTbl ) || CHAM_InitIoSw( &chaSwFktTbl ) ){
				printf("*** ERROR: CHAM_InitIo failed\n");
				goto ERROR_END;			
			}
		}
		/* mem access */
		else {
			if( CHAM_InitMem( &chaNoswFktTbl )  || CHAM_InitMemSw( &chaSwFktTbl )	){
				printf("*** ERROR: CHAM_InitMem failed\n");
				goto ERROR_END;
			}			
		}

		/* try non-swapped and swapped */
		if( (ret = chaNoswFktTbl.InitInside( osHdl, tblAddr, &chaHdl)) == 0 ){
			chaFktTbl = &chaNoswFktTbl;
		}
		else if( (ret = chaSwFktTbl.InitInside( osHdl, tblAddr, &chaHdl)) == 0 ){
			chaFktTbl = &chaSwFktTbl;
		}
		else {
			printf("*** ERROR: no chameleon table found at address 0x%x (0x%x)\n",
				tblAddr, ret);
			goto ERROR_END;
		}
	}
	/* use PCI device info */	
	else{
		CHAM_FUNCTBL		chaMemFktTbl, chaMemSwFktTbl;
#ifdef Z100_IO_ACCESS_ENABLE
		CHAM_FUNCTBL 		chaIoFktTbl, chaIoSwFktTbl;
#endif
		/*------------------------------+
		|  CHAM_InitXxx                 |
		+------------------------------*/
		if( CHAM_InitMem( &chaMemFktTbl )  || CHAM_InitMemSw( &chaMemSwFktTbl )
#ifdef Z100_IO_ACCESS_ENABLE
			|| CHAM_InitIo( &chaIoFktTbl ) || CHAM_InitIoSw( &chaIoSwFktTbl )
#endif
		){
			printf("*** ERROR: CHAM_InitXxx failed\n");
			goto ERROR_END;
		}		
		if( (ret = chaMemFktTbl.InitPci( osHdl,				/* osh */
#ifdef OSS_VXBUS_SUPPORT				
						busCtrlID,
#endif
						 pciDev->bus,		/* pciBus */
						 pciDev->dev,		/* pciDev */
						 pciDev->fun,		/* pciFunc*/
						 &chaHdl)) == 0 )
		{
			chaFktTbl = &chaMemFktTbl;
		}
		else if( (ret = chaMemSwFktTbl.InitPci( osHdl,		/* osh */
#ifdef OSS_VXBUS_SUPPORT				
										 	   busCtrlID,
#endif				
									 		   pciDev->bus,	/* pciBus */
											   pciDev->dev,	/* pciDev */
									 		   pciDev->fun,	/* pciFunc*/
									 		   &chaHdl)) == 0 )
		{
			chaFktTbl = &chaMemSwFktTbl;
		}
#ifdef Z100_IO_ACCESS_ENABLE
		else if( (ret = chaIoFktTbl.InitPci( osHdl,			/* osh */
#ifdef OSS_VXBUS_SUPPORT				
										 	 busCtrlID,
#endif				
											 pciDev->bus,	/* pciBus */
											 pciDev->dev,	/* pciDev */
											 pciDev->fun,	/* pciFunc*/
											 &chaHdl))     == 0 )
		{
			chaFktTbl = &chaIoFktTbl;
		}
		else if( (ret = chaIoSwFktTbl.InitPci( osHdl,		/* osh */
#ifdef OSS_VXBUS_SUPPORT				
										 	   busCtrlID,
#endif				
											   pciDev->bus,	/* pciBus */
											   pciDev->dev,	/* pciDev */
											   pciDev->fun,	/* pciFunc*/
											   &chaHdl))     == 0 )
		{
			chaFktTbl = &chaIoSwFktTbl;
		}
#endif
		else
		{
			printf("*** ERROR: No chameleon table found for PCI device %d/%d/%d "
					"(bus/dev/fun) (0x%x)\n",
					pciDev->bus, pciDev->dev, pciDev->fun, ret);
			goto ERROR_END;
		}
	}

	/*------------------------------+
	|  CHAM - Info                  |
	+------------------------------*/
	if( chaFktTbl->Info( chaHdl, chamInfo ) != 0 )	{
		/* ISA/LPC */
		if( tblAddr ){
			printf("*** ERROR: Cham_Info failed for device 0x%x\n",
					tblAddr);
		}
		/* PCI */
		else
		{
			printf("*** ERROR: Cham_Info failed for device %d/%d/%d (bus/dev/fun)\n",
					pciDev->bus, pciDev->dev, pciDev->fun);
		}
		goto ERROR_END;
	}
	if (verbose) {
		/* ISA/LPC */
		if( tblAddr ){
			printf("Cham_Info for device 0x%x:\n",
				tblAddr);
		}
		/* PCI */
		else
		{
			printf("Cham_Info for device %d/%d/%d (bus/dev/fun):\n",
				pciDev->bus, pciDev->dev, pciDev->fun);
		}
		printf(" chaRev: %d;	busId: %d;	tableNbr %d;\n"
				" unitNbr: %d;	bridgeNbr: %d;	cpuNbr %d;\n",
				chamInfo->chaRev,  chamInfo->busId,     chamInfo->tableNbr,
				chamInfo->unitNbr, chamInfo->bridgeNbr, chamInfo->cpuNbr);
		for(i=0; i<6; i++)
			printf("BAR%d: 0x%08x; size: 0x%08x, mapType: %s;\n",
					(int)i, (int)chamInfo->ba[i].addr,
					(int)chamInfo->ba[i].size,
					chamInfo->ba[i].type == 0 ? "MEM" :
					chamInfo->ba[i].type == 1 ? "IO" : "unused");
	}

	/*------------------------------+
	|  CHAM - TableIdent            |
	+------------------------------*/
	for( i=0; i<1000; i++ ){
		if( (ret = chaFktTbl->TableIdent( chaHdl, i, &chamTable )) ==
			CHAMELEONV2_NO_MORE_ENTRIES )
			break;

		if(ret != 0) {
			/* ISA/LPC */
			if( tblAddr ){
				printf("*** ERROR: Cham_TableIdent failed for device 0x%x\n",
						tblAddr);
			}
			/* PCI */
			else
			{
				printf("*** ERROR: Cham_TableIdent failed for device %d/%d/%d (bus/dev/fun)\n",
						pciDev->bus, pciDev->dev, pciDev->fun);
			}
			goto ERROR_END;
		}

		if(verbose)
			printf("CHAMELEONV2_HEADER #%2d:\n"
					"  busType=0x%02x, busId=%d, model=%c, MajorRevision=0x%02x,"
					"  MinorRevision=0x%02x,\n file=%s, magicWord=0x%04x\n",
					(int)i,
					chamTable.busType, chamTable.busId, chamTable.model,
					chamTable.revision,chamTable.minRevision,
					chamInfo->chaRev < 2 ? "not supported" : chamTable.file,
					chamTable.magicWord);
	}

	/*------------------------------+
	|  CHAM - UnitIdent             |
	+------------------------------*/
	if (verbose){
		printf("CHAMELEONV2_UNIT:\n");
		printf("Idx DevId  Module                   Grp Inst Var Rev IRQ BAR Offset     Address\n"
				"--- ------ ------------------------ --- ---- --- --- --- --- ---------- ----------\n");
	}

	for( i=0; i<1000; i++ ){

		if( (ret = chaFktTbl->UnitIdent( chaHdl, i, chamUnit )) ==
			CHAMELEONV2_NO_MORE_ENTRIES )
			break;

		if(ret != 0) {
			/* ISA/LPC */
			if( tblAddr ){
				printf("*** ERROR: Cham_UnitIdent failed for device 0x%x\n",
						tblAddr);
			}
			/* PCI */
			else
			{
				printf("*** ERROR: Cham_UnitIdent failed for device %d/%d/%d (bus/dev/fun)\n",
						pciDev->bus, pciDev->dev, pciDev->fun);
			}
			goto ERROR_END;
		}

		if(verbose)	{
			printf("%3d 0x%04x %-24s %3d %4d %3d %3d %3d %3d 0x%08lx %p\n",
				   (int)i, chamUnit->devId,CHAM_DevIdToName(chamUnit->devId),chamUnit->group,
				   chamUnit->instance, chamUnit->variant, chamUnit->revision,
				   (chamUnit->interrupt /*- SYS_NUM_CHAM_INTO*/), chamUnit->bar,
				   chamUnit->offset, chamUnit->addr);
		}

		/* check if the specified Flash interface is found */
		if( ((chamInfo->chaRev <  2) && (chamUnit->devId == mod_id))    ||
			((chamInfo->chaRev >= 2) && (chamUnit->devId == mod_id) &&
			 (chamUnit->group == mod_group)) )
		{
			if (verbose != SHOW_ALL_CHAMTABLES){
				break;
			}else {
				chaUnitNbr = i;
			}
		}
	}
	if ( ( verbose != SHOW_ALL_CHAMTABLES ) && ( chamUnit->devId != mod_id ) ){
		goto ERROR_END;
	}

	if ( verbose == SHOW_ALL_CHAMTABLES ){
		chaFktTbl->UnitIdent( chaHdl, chaUnitNbr, chamUnit );
	}

	chaFktTbl->Term( &chaHdl );
	return 0;

ERROR_END:
	printf("*** ERROR: can't find device 0x%03x\n", (unsigned int)mod_id);
	OSS_MemFill(osHdl, sizeof(CHAMELEONV2_INFO), (char*)chamInfo, 0);
	OSS_MemFill(osHdl, sizeof(CHAMELEONV2_UNIT), (char*)chamUnit, 0);

	if( chaHdl )
		chaFktTbl->Term( &chaHdl );

	return(ERR_Z100_CHAMELEON_DEVICE_NOT_FOUND);

}

/********************************* FindFlashSect ******************************/
/** Get start Address and size of sector from physical address
 *
 *---------------------------------------------------------------------------
 *  \param	fDev		\IN		FLASH_DEVS handle
 *  \param	phyAddr 	\IN		phys address
 *  \param	sectAddr	\OUT	ptr to var where start addr of sector is stored
 *  \param	sectSize	\OUT	ptr to var where size of sector is stored
 *
 *  \return	sector number or error (-1) if not found
 ******************************************************************************/
static int32 FindFlashSect( FLASH_DEVS *fDev,
					  		u_int32 phyAddr,
					  		u_int32 *sectAddr,
					  		u_int32 *sectSize )
{
	u_int32 addr = phyAddr;
	u_int32 sect;

	for( sect=0; sect < fDev->nSectors; sect++ ){
		if( addr >= fDev->sectAddr[sect]*2 &&
			addr < fDev->sectAddr[sect+1]*2){
			*sectAddr = fDev->sectAddr[sect]*2;
			*sectSize = (fDev->sectAddr[sect+1] - fDev->sectAddr[sect])*2;
			return sect;
		}
	}
	return -1;
}

/******************************* Get_FpgaHeader *******************************/
/** Get FPGA header
 *
 * \param fDev		\IN FLASH_DEVS handle
 * \param offset	\IN offset of FPGA configuration in flash
 * \param header	\IN header structure to be filled to be filled
 *
 * \return void
 ******************************************************************************/
static void Get_FpgaHeader( FLASH_DEVS *fDev,
					 u_int32 offset,
					 FPGA_LONGHEADER *header )
{

	fDev->devHdl->flash_entry.ReadBlock( fDev,
										 offset,
										 FPGA_SIZE_HEADER_LONG,
										 (u_int8*)header);

	/* print stuff */
	if( fDev->devHdl->dbgLevel )
		printf("FPGA configuration header:\n"
				"    Magic: 0x%08x\n"
				"    boardType: %s\n"
				"    offset[0]: 0x%08x; offset[1]: 0x%08x\n"
				"	 offset[2]: 0x%08x; offset[3]: 0x%08x\n",
				(unsigned int)Z100_SWAP_BE32(header->magic),
				header->boardType,
				(unsigned int)Z100_SWAP_BE32(header->offset[0]),
				(unsigned int)Z100_SWAP_BE32(header->offset[1]),
				(unsigned int)Z100_SWAP_BE32(header->offset[2]),
				(unsigned int)Z100_SWAP_BE32(header->offset[3]) );
	return;
}

/******************************* Verify_FpgaConfig ****************************/
/** Verify FPGA configuration in flash with configuration in buffer
 *
 * \param h			\IN DEV_HDL handle
 * \param offset	\IN offset of FPGA configuration to verify in flash
 * \param len		\IN length of FPGA configuration to verify in flash
 * \param buf		\IN buffer containing FPGA configuration to verify against
 *
 * \return (0) on success or error code
 ******************************************************************************/
static int32 Verify_FpgaConfig
(
	DEV_HDL *h,
	OSS_HANDLE *osHdl,
	u_int32 offset,
	u_int32 len,
	u_int8 *buf
)
{
	u_int8 *verBuf = NULL, *bufp1, *bufp2;
	int32 error = 0;
	u_int32 numBytes, verBufGot;

	/* reset flash */
	h->flash_entry.Reset(&h->flashDev);

	/* verify written stuff */
	if( h->dbgLevel )
		printf("VERIFY: Please Wait ...");

	if( !(verBuf = OSS_MemGet( osHdl, len, &verBufGot )) ) {
		printf("\n*** ERROR: allocating buffer for verifying flash content\n");
		error = ERR_OSS_MEM_ALLOC;
		goto VERIFYEXIT;
	}

	/* fill the buffer with 0 */
	OSS_MemFill(osHdl,len,verBuf,0);

	h->flash_entry.ReadBlock(&h->flashDev, offset, len, verBuf);
	{
		bufp1 = buf;
		bufp2 = verBuf;
		for(numBytes = 0; numBytes < len; numBytes++) {
			if((*bufp1) != (*bufp2)) {
				printf("*** ERROR: verifying flash content with file content\n"
						"       at byte Nr. 0x%x, File: 0x%0x, Read: 0x%0x",
						(unsigned int)numBytes,*bufp1,*bufp2);
				error = ERR_FLASH_VERIFICATION;
				goto VERIFYEXIT;
			}
			bufp1++;
			bufp2++;
		}
	}

VERIFYEXIT:
	if(verBuf)
		OSS_MemFree( osHdl, (void*)verBuf, verBufGot);

	return(error);


}

/***************************** Print_PciDeviceInfo ****************************/
/** Print information of PCI devices
 *
 * \param pciDevs	\IN pointer to PCI_DEVS handle array
 * \param numDevs	\IN number of devices to print info for
 * \param id		\IN flag print detailed device info
 *
 * \return void
 ******************************************************************************/
static void Print_PciDeviceInfo( PCI_DEVS **pciDevs,
								 u_int32 numDevs,
								 u_int8 id )
{
	u_int32 n;
	if( id ) {
		printf("Nr.|bus|dev|fun| Ven ID | Dev ID | SubVen ID |\n");
		for(n = 0; n < numDevs; n++) {
			printf("%3d %3d %3d %3d  0x%04x   0x%04x    0x%04x\n",
					(int)n, pciDevs[n]->bus, pciDevs[n]->dev, pciDevs[n]->fun,
					(unsigned int)pciDevs[n]->venId,
					(unsigned int)pciDevs[n]->devId,
					(unsigned int)pciDevs[n]->subSysVenId);
		}
	} else {
		printf("Nr.|bus|dev|fun|"
				"   BAR0  |   BAR1  |   BAR2  |"
				"   BAR3  |   BAR4  |   BAR5\n");
		for(n = 0; n < numDevs; n++)
			printf("%3d %3d %3d %3d  %08x  %08x  %08x "
					" %08x  %08x  %08x\n",
					(int)n, pciDevs[n]->bus, pciDevs[n]->dev, pciDevs[n]->fun,
					(unsigned int)pciDevs[n]->bar[0],
					(unsigned int)pciDevs[n]->bar[1],
					(unsigned int)pciDevs[n]->bar[2],
					(unsigned int)pciDevs[n]->bar[3],
					(unsigned int)pciDevs[n]->bar[4],
					(unsigned int)pciDevs[n]->bar[5]);
	}
	return;
}

/**************************** FindPciDevice ***************************/
/** find PCI device specified by vendor and device ID and get parameters
 *
 *  \param osHdl        \IN     OSS_HANDLE
 *  \param dev			\IN		PCI_DEVS handle
 *  \param allPciDevs	\OUT	structure to fill with found devices
 *  \param numdevs		\OUT	number of PCI devices found
 *  \param show_all		\IN		return all PCI devices in system
 *
 *  \return success (0) or error (-1)
 *          or read value
 ******************************************************************************/
static int32 FindPciDevice( OSS_HANDLE *osHdl,
							PCI_DEVS *dev,
							PCI_DEVS* allPciDevs[],
							u_int32 *numDevs,
							int show_all )
{
	u_int32 vendorId=dev->venId,
			deviceId=dev->devId,
			subSysVenId=dev->subSysVenId,
			headerType = 0;
#ifdef OSS_VXBUS_SUPPORT	
	VXB_DEVICE_ID busCtrlID = sysGetMdisBusCtrlID();
#endif
	int32 error = 0, val1, val2;
	PCI_DEVS *pCurdev;
	u_int32 bus, slot, function, barn;
	u_int32 slotIndex = 0; /* current slot */
	*numDevs = 0;

	pCurdev = allPciDevs[0];

    /* for each bus */
    for(bus = 0; bus < 0x100; bus++) {
        /* for slot */
        for(slot = 0; slot < 32; slot++) {
            /* for each function */
            for(function = 0; function < 8; function++) {
                /* get the initial bus data */            	
				OSS_PciGetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT						
						busCtrlID,
#endif						
						bus, slot, function,
						OSS_PCI_VENDOR_ID, &val1 );

				
				/* check if a card is installed */
				/* if PCI bus is not implemented, 0 is returned instead of -1 */
                if ( (val1 == 0xFFFF) || (val1 == 0x0000) ) {
                    if (function == 0) {
                        /* nothing in slot, go to next */
                        break;
                    } else {
                        /* function 0 is required on all cards,
                         * if this is a multifunction card, other
                         * functions do not need to be contiguous,
                         * search for additional functions */
                        continue;
                    }
                } else {
					u_int32   comReg = 0;
					/* get parameters */
					pCurdev = allPciDevs[slotIndex];
					pCurdev->bus = (u_int8)bus;
					pCurdev->dev = (u_int8)slot;
					pCurdev->fun = (u_int8)function;

					pCurdev->venId = val1;
#ifdef OSS_VXBUS_SUPPORT					
					OSS_PciGetConfig( osHdl,busCtrlID,
									  bus, slot, function,
									  OSS_PCI_DEVICE_ID,
									  &pCurdev->devId);
					OSS_PciGetConfig( osHdl,busCtrlID,
									  bus, slot, function,
									  OSS_PCI_SUBSYS_VENDOR_ID,
									  &pCurdev->subSysVenId);
					OSS_PciGetConfig( osHdl,busCtrlID,
									  bus, slot, function,
									  OSS_PCI_COMMAND,
									  &comReg);
#else
					OSS_PciGetConfig( osHdl,
									  bus, slot, function,
									  OSS_PCI_DEVICE_ID,
									  &pCurdev->devId);
					OSS_PciGetConfig( osHdl,
									  bus, slot, function,
									  OSS_PCI_SUBSYS_VENDOR_ID,
									  &pCurdev->subSysVenId);
					OSS_PciGetConfig( osHdl,
									  bus, slot, function,
									  OSS_PCI_COMMAND,
									  &comReg);
#endif
					pCurdev->origComReg = (u_int16)(Z100_SWAP_BE16(comReg));

					OSS_PciGetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT							
							          busCtrlID,
#endif
									  bus, slot, function,
									  OSS_PCI_HEADER_TYPE,
									  &headerType);	
					
					if( !(headerType & 0x7f) ) /* some bridge? */
						for (barn = 0; barn < 6; barn++)							
							OSS_PciGetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT									
											  busCtrlID,
#endif											
											  bus, slot, function,
											  OSS_PCI_ADDR_0+barn,
											  &pCurdev->bar[barn]);
						pCurdev->comRegChanged = 0;


					/* check for match and continue to next */
					if( !show_all &&
						!(headerType & 0x7f) &&
						(pCurdev->venId == vendorId) &&
						(pCurdev->devId == deviceId) &&
						(pCurdev->subSysVenId == subSysVenId) )
					{
						(*numDevs)++;
						/* enable Memory/IO access of device if neccessary */
						if( !(pCurdev->origComReg &
							  OSS_PCI_COMMAND_ENABLE_IO_SPACE) ||
							!(pCurdev->origComReg &
							  OSS_PCI_COMMAND_ENABLE_MEM_SPACE) )
						{
							pCurdev->comRegChanged = 1;
							// Save original comand register for reset
                            dev->comRegChanged = 1;
                            dev->origComReg = pCurdev->origComReg;

							if( (error = OSS_PciSetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT									
											busCtrlID,
#endif
											bus, slot, function,
											OSS_PCI_COMMAND,
											Z100_SWAP_BE16(pCurdev->origComReg    |
											 OSS_PCI_COMMAND_ENABLE_IO_SPACE |
											 OSS_PCI_COMMAND_ENABLE_MEM_SPACE))))
							{
								printf("*** ERROR: couldn't enable PCI device "
										"access (0x%x)\n",
										(int)error);
								goto ERROR_END;
							}
						}
						if( *numDevs == Z100_MAX_DEVICES )
							goto FINDPCIEND;

						slotIndex++;

						/* function 0 is required on all cards,
						 * if this is a multifunction card, other
						 * functions do not need to be contiguous,
						 * search for additional functions */
						if (function == 0) {
							OSS_PciGetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT									
											busCtrlID,
#endif									
											  bus, slot, 0,
											  OSS_PCI_HEADER_TYPE,
											  &val2);
							/* look in Header Type if we have a multifunction
							 * card */
							if(!(val2 & 0x80)) {
								/* not a multi-function card */
								break;
							}
						}
					}  else if ( show_all ) {
						(*numDevs)++;

						if( *numDevs == Z100_MAX_DEVICES )
							goto FINDPCIEND;

						slotIndex++;
					}

				}
            }
        }
    }

FINDPCIEND:
	if( !(*numDevs) ){
		printf("*** ERROR: No PCI devices matching vendor ID    0x%04x,\n"
			   "                                   device ID    0x%04x and\n"
			   "                                   subVendor ID 0x%04x found!\n",
				(int)vendorId, (int)deviceId, (int)subSysVenId);
		dev=NULL;
		goto ERROR_END;
	}

	return 0;
ERROR_END:
	return( -1 );
}

/******************************* Z100_VmeInit *********************************/
/** Initialize access over VME bus
 *
 *  \param osHdl      \IN  valid OSS_HANDLE
 *  \param h          \IN  valid DEV_HDL
 *  \param argc       \IN  argument counter
 *  \param argv       \IN  argument vector
 *  \param addrWinHdl \IN  VME addr. window handle to be filled
 *
 *  \return	          0
 *
 *  \sa Z100_VmeExit()
 ******************************************************************************/
static int32 Z100_VmeInit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h,
	int argc,
	char **argv,
	void** addrWinHdl
)
{
	int32 error = 0;
#ifndef Z100_CONFIG_VME
	printf("*** ERROR: VME Bus not supported!!\n");
	error = ERR_UOS_ILL_PARAM;
#else /* Z100_CONFIG_VME */
	u_int32 vmeBusAddr = 0;
	void* vmeLocAddr = 0;

	if( h->dbgLevel )
		printf("Z100_VmeInit\n");

	while( argc ){
		if( !strcmp( "-a", *argv ) ) {
			if (argc < 2 ||
				!sscanf(argv[1],"%x",(unsigned int*)&vmeBusAddr))
			{
				printf("*** ERROR: option -a requires VME address as hex value\n"
						FPGA_P_HELP);
				error = ERR_UOS_ILL_PARAM;
				goto VMEINITEND;
			}
			break;
		}
		argc--;
		argv++;
	}

#ifdef OSS_HAS_MAP_VME_ADDR
	if( (error = OSS_MapVmeAddr( osHdl,
								(u_int64)vmeBusAddr,
								OSS_VME_AM_24UD,
								OSS_VME_DM_16,
								MAP_REG_SIZE,
								0x00,
								&vmeLocAddr,
								addrWinHdl )) )
	{
		printf("*** ERROR: OSS_MapVmeAddr failed (0x%x)\n", error);
		goto VMEINITEND;
	}
#else /* OSS_HAS_MAP_VME_ADDR */
	if( (error = OSS_BusToPhysAddr( osHdl,
									OSS_BUSTYPE_VME,
									&vmeLocAddr,
									(void*)vmeBusAddr,
									OSS_VME_A24 | OSS_VME_D16,
									MAP_REG_SIZE )) )
	{
		printf("*** ERROR: OSS_BusToPhysAddr failed (0x%x)\n", error);
		goto VMEINITEND;
	}
#endif  /* OSS_HAS_MAP_VME_ADDR */
	h->physAddr = vmeLocAddr;
	h->busType  = OSS_BUSTYPE_VME;
	h->mapType  = OSS_ADDRSPACE_MEM;
#endif /* Z100_CONFIG_VME */

VMEINITEND:
	return error;
}

/******************************* Z100_VmeExit *********************************/
/** Deinitialize access over VME bus
 *
 *  \param osHdl       \IN  valid OSS_HANDLE
 *  \param addrWinHdl  \IN  valid VME addr. window handle
 *
 *  \return	          0 or error returned from OSS_UnMapVmeAddr
 *
 *  \sa Z100_VmeInit()
 ******************************************************************************/
static int32 Z100_VmeExit(
	OSS_HANDLE *osHdl,
	void* addrWinHdl
)
{
#ifdef OSS_HAS_MAP_VME_ADDR
	return OSS_UnMapVmeAddr(osHdl, addrWinHdl);
#else
	return 0;
#endif
}

/******************************* Z100_PciInit *********************************/
/** Initialize access over PCI bus
 *
 *  \param osHdl       \IN  valid OSS_HANDLE
 *  \param h           \IN  valid DEV_HDL
 *  \param flashInterf \IN  type of flash interface (0=Z045_FLASH, 1=PLD/SMB)
 *  \param argc        \IN  argument counter
 *  \param argv        \IN  argument vector
 *
 *  \return	           0
 *
 *  \sa Z100_PciExit()
 ******************************************************************************/
static int32 Z100_PciInit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h,
	u_int32 flashInterf,
	int argc,
	char **argv
)
{
	int32 error = 0;
	u_int32	AddrSetManually = 0;
	PCI_DEVS *pciDev, *allPciDevs[Z100_MAX_DEVICES], *tmpPciDev = NULL;
	u_int16 barN = 0;
	u_int32 units_reg_offset = 0;
	u_int32 numDevs = 0, n, instance = 0, tmpPciDevGotSize;
	u_int8 showOnly = 0;
	u_int8 showChamTable = 0;
#ifdef VXWORKS
	int32 cpu_to_pci_mem_offset = 0;	/* specifies an offset to be added for
										 * accesses to PCI memory mapped devs*/
	int32 cpu_to_pci_io_offset = 0;		/* specifies an offset to be added for
										 * accesses to PCI IO-mapped devs */
#endif

	if( h->dbgLevel )
		printf("Z100_PCIInit\n");

	for(n = 0; n < Z100_MAX_DEVICES; n++)
		allPciDevs[n] = NULL;

	if(UTL_TSTOPT("b") && !UTL_TSTOPT("o") ){
		printf("*** ERROR: option -b requires option -o as well\n"
				FPGA_P_HELP);
		error = 1;
		goto PCIINITEND;
	}

	showOnly = (UTL_TSTOPT("s") ? 1 : 0);
	showChamTable = (UTL_TSTOPT("t")? 1 : 0 );
	AddrSetManually = UTL_TSTOPT("b") ? 1 : 0;
	pciDev = &h->pciDev;


	/* read parameters from command line */
	if( ((!AddrSetManually && /* PCI, use chameleom table? */
		  ( argc<5 ||				/* need venId,devId,subVenId,instance */
		    !sscanf(argv[1], "%x", (unsigned int*)&pciDev->venId) ||
		    !sscanf(argv[2], "%x", (unsigned int*)&pciDev->devId) ||
		    !sscanf(argv[3], "%x", (unsigned int*)&pciDev->subSysVenId) ||
		    !sscanf(argv[4], "%u", (unsigned int*)&instance) )) ||
		 (AddrSetManually && /* PCI, set bar+offset manually */
		  ( argc<9 ||				/* need as above plus bar,offset */
		    !sscanf(argv[1], "%x", (unsigned int*)&pciDev->venId) ||
		    !sscanf(argv[2], "%x", (unsigned int*)&pciDev->devId) ||
		    !sscanf(argv[3], "%x", (unsigned int*)&pciDev->subSysVenId) ||
		    !sscanf(argv[4], "%u", (unsigned int*)&instance) ||
		    !sscanf(argv[6], "%u", (unsigned int*)&barN) ||
		    (barN > 5)									||
		    !sscanf(argv[8], "%x", (unsigned int*)&units_reg_offset) )) ) &&
		!showOnly ) {
		Usage();
		error = 1;
		goto PCIINITEND;
	}
#if 0
	pciDev->venId = 0x1a88;
	pciDev->devId = 0x4d45;
	pciDev->subSysVenId = 0xab;
	instance = 0;
#endif	
	/* allocate and init memory to detect all PCI devices */
	if( !(tmpPciDev = (PCI_DEVS*)OSS_MemGet(osHdl,
											Z100_MAX_DEVICES*sizeof(PCI_DEVS),
											&tmpPciDevGotSize)) ) {
		printf("*** ERROR: Insufficient ressources for PCI device information\n");
		error = ERR_OSS_MEM_ALLOC;
		goto PCIINITEND;
	}

	OSS_MemFill(osHdl, tmpPciDevGotSize, (char*)tmpPciDev, 0);

	for(n = 0; n < Z100_MAX_DEVICES; n++)
		allPciDevs[n] = &tmpPciDev[n];

	if( showOnly && !(pciDev->venId && pciDev->devId && pciDev->subSysVenId) ) {
		printf("\n"); /* needed, otherwise program will terminate violently */
		/* print all pci devices in sytem */
		if( !(error = FindPciDevice( osHdl, pciDev, allPciDevs, &numDevs, 1)) ) {
			Print_PciDeviceInfo(allPciDevs, numDevs, 1);
		}
		error = 100;
		goto PCIINITEND;
	}
	
	if( (error = FindPciDevice( osHdl, pciDev, allPciDevs, &numDevs, 0)) )
		goto PCIINITEND;
	
	if( showOnly ) {
		/* print devices matching venId and devId */
		Print_PciDeviceInfo(allPciDevs, numDevs, 0);
		error = 100;
		goto PCIINITEND;
	}

	if( instance > (numDevs - 1) ) {
		printf("*** ERROR: instance Nr. is too large, found only %d devices\n",
				(int)numDevs);
		error = 1;
		goto PCIINITEND;
	}

	OSS_MemCopy(osHdl, sizeof(PCI_DEVS), (char*)allPciDevs[instance], (char*)pciDev);

	if( h->dbgLevel ) {
		printf("\nUpdate device:\n");
		Print_PciDeviceInfo(&pciDev, 1, 0);
	}

	h->busType = OSS_BUSTYPE_PCI;
	
#ifdef VXWORKS
	/* get CPU to PCI offsets for io and memory mapped accesses */
	{
		int i;
		int largc = argc;
		char **largv = &argv[0];

		while( largc > 0 ){
			if( !strcmp( "-m", *largv ) ) {
				if (largc<2 ||
					!sscanf(largv[1],"%x",(unsigned int*)&cpu_to_pci_mem_offset))
				{
					printf("*** ERROR: option -m requires offset as hex value\n"
							FPGA_P_HELP);
					error = ERR_UOS_ILL_PARAM;
					goto PCIINITEND;
				}
				largc -= 2;
				largv += 2;
				continue;
			}
			if( !strcmp( "-i", *largv ) ) {
				if (largc<2 ||
					!sscanf(largv[1],"%x",(unsigned int*)&cpu_to_pci_io_offset))
				{
					printf("*** ERROR: option -i requires offset as hex value\n"
							FPGA_P_HELP);
					error = ERR_UOS_ILL_PARAM;
					goto PCIINITEND;
				}
				largc -= 2;
				largv += 2;
				continue;
			}
			largc--;
			largv++;
		}
		/* apply cpu_to_pci_xx_offset to all BARs */
		for(i = 0; i < 6; i++) {
			if( pciDev->bar[i] & 0x00000001 )
				/* bar is IO-mapped */
				pciDev->bar[i] += cpu_to_pci_io_offset;
			else
				/* bar is memory mapped */
				pciDev->bar[i] += cpu_to_pci_mem_offset;
		}
	}
#endif /* VXWORKS */
	
	/* set address, if configured manually, for PCI bus */
	if( AddrSetManually )
	{
		if( h->pciDev.bar[barN] & 0x1 ) {
			/* also set this information where evaluated by other macros */
			h->chamInfo.ba[barN].type = OSS_ADDRSPACE_IO;
			if( h->dbgLevel )
				printf("!!! Bar %d is IO mapped !!!!\n", barN);
		}
		h->chamUnit.bar = barN;
		h->chamUnit.addr = (void*)((h->pciDev.bar[barN] & ~0x0f) +
								   units_reg_offset );
		h->physAddr = (void*)h->chamUnit.addr;
		h->mapType = h->pciDev.bar[barN] & 0x01;
	}
	else
	{
		u_int16 modId = WB2FLASH_INTERFACE_ID;
		u_int16 grpId = WB2FLASH_INTERFACE_GROUP;

        if (h->interfacespi == 1){
            modId = WB2SPI_FLASH_INTERFACE_ID;
        }

		if( flashInterf == 1 )
		{
			if( !strcmp(h->smbLocHdl.smbCtlName, "menz001") )
			{
				modId = CHAMELEON_16Z001_SMB;
				grpId = 0;
			} else
			{
				printf("*** ERROR: No known chameleon device, "
						"please specify address (bar/offs) manually\n"
						FPGA_P_HELP);
				error = ERR_UOS_ILL_PARAM;
				goto PCIINITEND;

			}
		}
		
		if( showChamTable){
			if( (error = Get_Chameleon( osHdl,
										&h->pciDev,
										NULL,
										modId,
										grpId,
										&h->chamInfo,
										&h->chamUnit,
										SHOW_ALL_CHAMTABLES )) )
			{
					printf("*** ERROR: Chameleon table or flash interface not found "
							"(0x%x)\n",
							(unsigned int)error);
					goto PCIINITEND;
			}
			error = 100;
			goto PCIINITEND;
		}else if( (error = Get_Chameleon( osHdl,
									&h->pciDev,
									NULL,
									modId,
									grpId,
									&h->chamInfo,
									&h->chamUnit,
									h->dbgLevel )) )
		{
			/* error occured while searching for 16Z045_FLASH or 16Z126_SPI_FLASH */
			printf("*** ERROR: Chameleon table or flash interfaces not found "
						"(0x%x)\n",
						(unsigned int)error);
			goto PCIINITEND;
		}

#ifdef VXWORKS

		/* put address together ourself to consider evtl offsets */
		if(h->chamInfo.ba[h->chamUnit.bar].type)
			/* IO mapped */
			h->physAddr = (void*)h->chamUnit.addr + cpu_to_pci_io_offset;
		else
			/* MEM mapped */
			h->physAddr = (void*)h->chamUnit.addr + cpu_to_pci_mem_offset;

#else	/* VXWORKS */
		h->physAddr = (void*)h->chamUnit.addr;
#endif /* VXWORKS */
		h->mapType = h->chamInfo.ba[h->chamUnit.bar].type;
	}

PCIINITEND:
	if( tmpPciDev )
		OSS_MemFree( osHdl, (void*)tmpPciDev, tmpPciDevGotSize );
	return error;
}

/******************************* Z100_PciExit *********************************/
/** Deinitialize access over PCI bus
 *
 *  \param osHdl      \IN  valid OSS_HANDLE
 *  \param h          \IN  valid DEV_HDL
 *
 *  \return	          0
 *
 *  \sa Z100_PciInit()
 ******************************************************************************/
static int32 Z100_PciExit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h
)
{
    int32 error = 0;

	/* Reset PCI Configuration Command Register to the old value */
	if( h->pciDev.comRegChanged == 1 ) {
		if( (error = OSS_PciSetConfig( osHdl,
#ifdef OSS_VXBUS_SUPPORT				
								sysGetMdisBusCtrlID(),
#endif
								h->pciDev.bus, h->pciDev.dev, h->pciDev.fun,
								OSS_PCI_COMMAND,
								Z100_SWAP_BE16(h->pciDev.origComReg ))))
			

		{
			printf("*** ERROR: couldn't reset PCI Command register"
					" (0x%x)\n",
					(int)error);
		}
	}
	return 0;
}

/******************************* Z100_IsaInit *********************************/
/** Initialize access over ISA/LPC bus
 *
 *  \param osHdl       \IN  valid OSS_HANDLE
 *  \param h           \IN  valid DEV_HDL
 *  \param flashInterf \IN  type of flash interface (0=Z045_FLASH, 1=PLD/SMB)
 *  \param argc        \IN  argument counter
 *  \param argv        \IN  argument vector
 *
 *  \return	           0
 *
 ******************************************************************************/
static int32 Z100_IsaInit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h,
	u_int32 flashInterf,
	int argc,
	char **argv
)
{
	int32 error = 0;
	u_int8 showChamTable = 0;
	u_int16 modId = WB2FLASH_INTERFACE_ID;
	u_int16 grpId = WB2FLASH_INTERFACE_GROUP;
	void* tableAddr;

	if( h->dbgLevel )
		printf("Z100_IsaInit\n");

	showChamTable = (UTL_TSTOPT("t")? 1 : 0 );

	/* read parameters from command line */
	if( (argc<3) ||				/* need table address */
		    !sscanf(argv[2], "%x", (unsigned int*)&tableAddr) ) {
		Usage();
		error = 1;
		goto ISAINITEND;
	}

    if (h->interfacespi == 1){
        modId = WB2SPI_FLASH_INTERFACE_ID;
    }

	if( flashInterf == 1 )
	{
		if( !strcmp(h->smbLocHdl.smbCtlName, "menz001") )
		{
			modId = CHAMELEON_16Z001_SMB;
			grpId = 0;
		} else
		{
			printf("*** ERROR: No known chameleon device, "
					"please specify address manually\n"
					FPGA_P_HELP);
			error = ERR_UOS_ILL_PARAM;
			goto ISAINITEND;

		}
	}

	if( showChamTable){
		if( (error = Get_Chameleon( osHdl,
									NULL,
									tableAddr,
									modId,
									grpId,
									&h->chamInfo,
									&h->chamUnit,
									SHOW_ALL_CHAMTABLES )) )
		{
				printf("*** ERROR: Chameleon table or flash interface not found "
						"(0x%x)\n",
						(unsigned int)error);
				goto ISAINITEND;
		}
		error = 100;
		goto ISAINITEND;
	}else if( (error = Get_Chameleon( osHdl,
								NULL,
								tableAddr,
								modId,
								grpId,
								&h->chamInfo,
								&h->chamUnit,
								h->dbgLevel )) )
	{
		/* error occured while searching for 16Z045_FLASH or 16Z126_SPI_FLASH */
		printf("*** ERROR: Chameleon table or flash interfaces not found "
					"(0x%x)\n",
					(unsigned int)error);
		goto ISAINITEND;
	}


	h->physAddr = (void*)h->chamUnit.addr;
	h->mapType = h->chamInfo.ba[h->chamUnit.bar].type;
	
ISAINITEND:

	return error;
}

/******************************* Z100_SmbInit *********************************/
/** Initialize access over SMB bus (init SMB library)
 *
 *  currently supported SMB host controllers are:
 *  menz001

 *  \param osHdl      \IN  valid OSS_HANDLE
 *  \param h          \IN  valid DEV_HDL
 *
 *  \return	          0
 *
 *  \sa Z100_SmbExit()
 ******************************************************************************/
static int32 Z100_SmbInit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h
)
{
	int32 error = 0;
#ifndef Z100_CONFIG_SMB
	printf("*** ERROR: SMB Bus not supported!!\n");
	error = ERR_UOS_ILL_PARAM;
	goto SMBINITEND;
#else /* Z100_CONFIG_SMB */
	SMB_DESC_MENZ001	z001_desc;

	if( h->dbgLevel )
		printf("Z100_SmbInit\n");

	/* is it an "menz001" SMBus controller? */
	if( !strcmp(h->smbLocHdl.smbCtlName, "menz001") )
	{
		if( (error = OSS_MapPhysToVirtAddr(osHdl,
						 h->physAddr,
						 MAP_REG_SIZE,	/* size */
						 h->mapType,
						 h->busType,
						 h->pciDev.bus,
						 (void**)&h->mappedAddr)) )
		{
			printf("*** ERROR: Z100_SmbInit: can't map address 0x%x (0x%x)\n",
					h->physAddr, (int)error);
			goto SMBINITEND;
		}
		h->mappedSize = MAP_REG_SIZE;

		z001_desc.baseAddr		= (void*) h->mappedAddr;
		z001_desc.sclFreq		= 223;
		z001_desc.dbgLevel 		= Z100_OSS_DBG_LEVEL;
		z001_desc.busyWait 		= 20;
		z001_desc.alertPollFreq = 0;	/* don't care */
		z001_desc.timeOut 		= 20;
		z001_desc.mikroDelay 	= 1;	/* use OSS_Delay */

		if( (error = SMB_MENZ001_Init( &z001_desc, osHdl, &h->smbLocHdl.smbHdl )) )
		{
			printf("*** ERROR: Z100_SmbInit: init Z001 ctrl. failed (0x%x)\n",
					(int)error);
		}

		goto SMBINITEND;
	}


#ifndef MAC_USERSPACE
	/* eventually try OSS_GetSmbHdl() */
	error = OSS_GetSmbHdl(  osHdl,
							h->smbLocHdl.smbCtlNum,
							&h->smbLocHdl.smbHdl);

	if( error ) {
		printf("*** ERROR: Z100_SmbInit: No supported SMB host controller found\n");
		h->smbLocHdl.smbHdl = NULL;
	}
#endif /* !LINUX */

#endif /* Z100_CONFIG_SMB */

SMBINITEND:
	return error;
}

/******************************* Z100_SmbExit *********************************/
/** Deinitialize access over SMB bus (deinitialize SMB library)
 *
 *  \param osHdl      \IN  valid OSS_HANDLE
 *  \param h          \IN  valid DEV_HDL
 *
 *  \return	          0
 *
 *  \sa Z100_SmbInit()
 ******************************************************************************/
static int32 Z100_SmbExit(
	OSS_HANDLE *osHdl,
	DEV_HDL *h
)
{
	int32 error = 0;
#ifndef Z100_CONFIG_SMB
	printf("*** ERROR: SMB Bus not supported!!\n");
	error = ERR_UOS_ILL_PARAM;
	goto SMBEXITEND;
#else /* Z100_CONFIG_SMB */
	SMB_HANDLE *smbHdl = (SMB_HANDLE*)h->smbLocHdl.smbHdl;

	if( h->dbgLevel )
		printf("Z100_SmbExit\n");

	/* only perform an smbExit if we initialized SMB */
	if( *h->smbLocHdl.smbCtlName && (error = smbHdl->Exit(&h->smbLocHdl.smbHdl)) )
	{
		printf("*** ERROR: Z100_SmbExit: exit SMB ctrl. failed (0x%x)\n",
				(int)error);
		goto SMBEXITEND;
	}

#endif /* Z100_CONFIG_SMB */
SMBEXITEND:
	return error;
}

#ifdef Z100_CONFIG_SMB
/* will never be called, just added so lib is linked completely
   ( support for all I2C controllers if linked to BSP (e.g. VXWORKS) */
static void DummySmbLibInit(void)
{
	DummySmbLibInit(); /* no warning about this function not beeing called */
	SMB_MGT5200_Init( NULL, NULL, NULL);
	SMB_MPC85XX_Init( NULL, NULL, NULL);
}

#endif /* Z100_CONFIG_SMB */


#ifdef VXWORKS
/* Dummy Routine to link OSS_AlarmCreate and OSS_SetSmbHdl with MDIS to resolve
 * compiling errors with VxWorks BSP */
static void Dummy_Routine (void)
{
    Dummy_Routine(); /* don't complain about unused functions */
    OSS_AlarmCreate( NULL,
                     NULL,
                     NULL,
                     NULL);      /* link oss_alarm.c file */
    OSS_SetSmbHdl( NULL,0,NULL); /* link oss_smb.c file */
}
#endif /* VXWORKS */


