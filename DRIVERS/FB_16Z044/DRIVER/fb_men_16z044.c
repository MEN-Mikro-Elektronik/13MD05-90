/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  fb_men_16z044.c
 *
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2010/01/27 20:10:01 $
 *    $Revision: 1.8 $
 *
 *  	 \brief  Framebuffer driver for FPGAs containg a 16z044 unit.
 * 		 Supported modes:
 * 		 640x400 800x600 1024x768 1280x1024 at fixed 16bpp.
 *		 The driver is supposed to be used together with the
 *		 men_chameleon subsystem.
 *
 *     Switches:
 */
/*-------------------------------[ History ]---------------------------------
 *
 * $Log: fb_men_16z044.c,v $
 * Revision 1.8  2010/01/27 20:10:01  rt
 * R: 1) Linux may freeze at start-up if more than one Z043 SDRAM
 *       controller is in the system.
 * M: 1) Use only associated SDRAM controller (check chameleon group
 *       and PCI device).
 *
 * Revision 1.7  2009/02/24 16:54:21  ts
 * R: 1) driver did unnecessary seek in probe routine for DISP unit
 *    2) Resolution information should be available for new MEN BSP
 * M: 1) removed chameleon find call for 16Z044, probe routine is already
 *    called with this units informations
 *    2) added printk with resolution info for customers
 *
 * Revision 1.6  2008/12/01 17:34:07  rt
 * R:1. Wrong colours at display.
 *   2. Not compilable with Kernel 2.6.24
 * M:1.a) Comment of ByteSwap function changed according to 16z044 manual.
 *     b) Parameter at ByteSwap function calls changed.
 *   2. Added compiler switches.
 *
 * Revision 1.5  2008/09/05 15:02:58  ts
 * R: 1.) driver in ElinOS 4.2 was not up to date
 *    2.) changes from Sysgo differed to locat repository
 * M: 1.) officially fixed base offset problem and sent to Sysgo
 *    2.) merged in changes done by Sysgo
 *
 * Revision 1.4  2008/02/21 12:04:10  ts
 * fixed: - add offset for DISP unit (was previously 0x0, in EM1N its 0x700)
 *        - merged in changes from Sysgo
 *
 * Revision 1.3  2007/11/22 11:14:51  jal
 * pr#1852-elinos
 *
 * Now serial driver uses SA_SHIRQ
 * Fixed the device node name to be /dev/ttFPGAs/x
 *
 * There was also a problem with the frame buffer support which does not
 * compile. This was because a macro (KERNEL_IS_26) was not defined
 * anymore. I hjave not this defined.
 *
 * I also have added 2 new options in elk :
 * - Enable GPIO
 * - Enable FPGA serial
 *
 * Revision 1.2  2006/05/03 13:01:09  alm
 * Fix missing include.
 * Fixed compiler warning to do with ANSI C90
 *
 * Revision 1.1.1.1  2006/04/19 12:06:50  acs
 * First version of MEN EM1/PP01 BSP merge.
 * PR#935-elinos
 *
 * Revision 1.1.1.1  2006/02/24 11:33:18  acs
 * Added support for 2.6.15
 *
 * Revision 1.1.1.1  2005/09/13 12:25:37  pon
 * Initial import of MEN EM1
 *
 * Revision 1.1  2005/07/26 15:55:08  ts
 * Initial Revision
 *
 *---------------------------------------------------------------------------
 * (c) Copyright 2008 by MEN Mikro Elektronik GmbH, Nuremberg, Germany
 ****************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/pci.h>
#include <linux/fb.h>
#include <linux/console.h>
#include <linux/selection.h>  		/* default_colors 	*/
#include <linux/init.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <asm/uaccess.h> 		/* copy_to/from_user */
#include <MEN/fb_men_16z044.h>		/* public ioctls 	*/
#include <MEN/men_chameleon.h>
#include <MEN/16z044_disp.h>

# include <linux/device.h>

/*-----------------------------+
 |  DEFINES                    |
 +-----------------------------*/

#define DBG

/* debug helpers */
#if 1 /*def 	DBG*/
#define DPRINTK(x...)           printk(x)
#define DBG_FCTNNAME   		printk("%s()\n", __FUNCTION__ )
#else
#define DPRINTK(x...)
#define DBG_FCTNNAME
#endif

#define FB_16Z044_COLS			16
#define MEN_16Z044_REFRESH_75HZ		75
#define MEN_16Z044_REFRESH_60HZ		60

/* not in 16z044_disp.h yet? */
#define MEN_16Z044_FP_CTRL			( 0x0C )
#define FB_IDENTIFIER	  			"MEN MIKROELEKTRONIK"
#define MEN_FB_NAME 				"fb16z044"
#define FBDRV_NAMELEN				32

#define IP_CORE_16Z043     43 /* IP modcode V0, not part of chameleon.h */
#define IP_CORE_16Z024     24 /* IP modcode V0, not part of chameleon.h */
#define IP_CORE_16Z044     44 /* IP modcode V0, not part of chameleon.h */

/*--------------------------------+
 |  TYPEDEFS                      |
 +--------------------------------*/
/* storing the sets of available resolutions */
struct RES_SET
{
    u16 xres;
    u16 yres;
    u16 bits_per_pixel;
};

struct PALETTE
{
    u16 blue, green, red, pad;
};

/* set refresh rate (module parameter) */
static unsigned int refresh;

/* fb_ops */

static int men_16z044_pan_display(struct fb_var_screeninfo *var, struct fb_info *info);

struct MEN_16Z044_FB
{
    u16 xres;
    u16 yres;
    u16 xres_virtual;
    u16 yres_virtual;

    u16 byteswap;
    u16 refresh_rate;

    u16 line_length;
    u16 bits_per_pixel;
    u16 bytes_per_pixel;	/* for colors */

    u32 sdram_phys;  		/* phys mem base address (FB memory) from FPGA */
    u32 sdram_size;  		/* total size (BAR1) */
    u32 mmio_start; 		/* phys. start  of mmapped registers */
    u32 mmio_len;   		/* length*/
    void *sdram_virt;

    u32 dispctr_phys;
    u32 dispctr_size;

    void *dispctr_virt;
    u32 disp_offs;
    struct PALETTE palette[FB_16Z044_COLS];
    char *identifier;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    struct fb_info info;

    char name[FBDRV_NAMELEN+1];

    struct platform_device fb_device;

    struct pci_dev	*pdev;

    unsigned int barSdram;
    unsigned int barDisp;

};

/* currently possible resolutions (fixed into FPGA unit)*/
static const struct RES_SET G_resol[]={
    {	640 ,  480,  16 },
    {	800 ,  600,  16 },
    {  1024 ,  768,  16 },
    {  1280 , 1024,  16 },
    {   320 ,  240,  16 },
    {  1280 ,  800,  16 },
    {   256 ,   64,  8 /* MP70S */ },
};


/*--------------------------------+
 |  GLOBALS                       |
 +--------------------------------*/

#ifdef MODULE
/* module parameter for video refresh rate */
static unsigned int refresh=MEN_16Z044_REFRESH_60HZ;
#endif



int z44rd[16];
int z44gn[16];
int z44bl[16];


/* ------------------------------------------------------------------- */
/**********************************************************************/
/** HW panning Function, not implemented yet
 *
 * \brief  HW 'panning' might be implemented by manipulating the
 *         selected PAR_SETS timing registers. not tested yet.
 *
 * \returns -
 */
/* TODO: currently not supported */
static int men_16z044_pan_display(struct fb_var_screeninfo *var,
								  struct fb_info *info)
{
    return 0;
}



/**********************************************************************/
/** provide offset address of the controll registers
 *
 * \brief  The offset of the Display controller registers is retrieved from
 *         the chameleon subsystem.
 *
 * \param \IN   fbP  address of struct MEN_16Z044_FB whose to access
 *
 * \returns void * offset address of Display controller regs
 */
static void *fb_men_16z044_FrmOffsetReg(struct MEN_16Z044_FB *fbP)
{
    return (void*)(fbP->dispctr_virt + Z044_DISP_FOFFS );
}


/**********************************************************************/
/** provide virtual base address of the display controller
 *
 * \brief  The address of the Display controller unit is retrieved from
 *         the chameleon subsystem.
 *
 * \param \IN   fbP  address of struct MEN_16Z044_FB to access
 *
 * \returns void * address of Display controller unit
 */
static void *fb_men_16z044_DispCtrlBase(struct MEN_16Z044_FB *fbP)
{
    void *p=(void*)( fbP->dispctr_virt + Z044_DISP_CTRL + fbP->disp_offs );
    DPRINTK("fb_men_16z044_DispCtrlBase = %p offs 0x%04x\n",
			p, fbP->disp_offs);
    return p;
}



/*
     reminder from fh.h
    int (*fb_check_var)(struct fb_var_screeninfo *var, struct fb_info *info);
    int (*fb_set_par)(struct fb_info *info);
*/

static int men_16z044_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{

  return 0; /* immer ok.. */
}

static int men_16z044_set_par( struct fb_info *info )
{
  return 0; /* immer ok.. */
}


/**********************************************************************/
/** return the MEN_16Z044_FB struct associated with this info struct
 *
 * \param \IN   info
 *
 * \returns NULL if no 16z044 entry for this fb_info found or pointer
 *			to it on success
 */
static struct MEN_16Z044_FB *men_16z044_from_info( struct fb_info *infoP )
{
    if ( !infoP || !infoP->par ){
		DPRINTK("invalid NULL pointer\n");
		return NULL;
    }

    return (struct MEN_16Z044_FB *)(infoP->par);

}


/**********************************************************************/
/** provide variable screeninfo structure of framebuffer
 *
 * \param \IN	con		console switch
 * \param \IN   info	fb_info struct currently used
 *
 * \returns -EIO if error  or 0 on success
 */
#if 0 /* not supported yet */
static int men_16z044_update_var(int con, struct fb_info *info)
{
    /* does panning , therefore not supported */
    return 0;

}
#endif

/***************************************************************************/
/** get the framebuffers current color map.
 *
 * \param \IN 	regno	fb_fix_screeninfo struct
 * \param \IN	red		red color part of color <regno> to set
 * \param \IN	green	green color part of color <regno> to set
 * \param \IN	blue	blue color part of color <regno> to set
 * \param \IN	transp  transparency Value (not used)
 * \param \IN   info	fb_info of the display
 *
 * \returns 	0 on success or errorcode
 */
static int men_16z044_setcolreg(unsigned regno,
								unsigned red,
								unsigned green,
								unsigned blue,
								unsigned transp,
								struct fb_info *fb_info)
{

    struct MEN_16Z044_FB *fbP=NULL;

    if (regno >= FB_16Z044_COLS)
		return 1;

    fbP = men_16z044_from_info(fb_info);

    if (!fbP)
		return -ENODEV;

    fbP->palette[regno].red   = red;
    fbP->palette[regno].green = green;
    fbP->palette[regno].blue  = blue;

    /* the 16z044 has just RGB565 built in for now*/
    ((u32*) (fb_info->pseudo_palette))[regno] =
		((red   & 0xf800)      ) |
		((green & 0xfc00) >>  5) |
		((blue  & 0xf800) >> 11);

    return 0;
}

/***************************************************************************/
/** select the number of the Screen to display in intern FB memory
 *
 * \param \IN	nr		Number of screen in memory, default is 0
 * \param \IN   fbP		fb struct of the display whose screen # to select
 *
 * \brief The number of 'virtual' Screens depend on FB memsize and Resolution.
 *        amount of virtual screens: memsize / ((x_res*y_res)/bytes_per_pixel)
 *
 * \returns 0 on sucess or Errorcode
 */
static int men_16z044_SetScreen(struct MEN_16Z044_FB *fbP, unsigned int nr)
{
    unsigned int nrScreens = 0;

    if (!fbP)
		return -EINVAL;

    nrScreens=(fbP->sdram_size/((fbP->xres * fbP->yres)/ fbP->bytes_per_pixel));

    DBG_FCTNNAME;
    DPRINTK("Nr. of Screens: %d\n", nrScreens);

    if ( nr > nrScreens ){
		printk(KERN_ERR "maximum number of virtual Screens = %d\n", nrScreens);
		return -EINVAL;
    }

    writel( nr * fbP->xres * fbP->yres * fbP->bytes_per_pixel,
			fb_men_16z044_FrmOffsetReg(fbP) );

    return 0;
}





/**********************************************************************/
/** blanking / unblanking the screen
 *
 * \brief
 *  Blank the screen if blank_mode != 0, else unblank. If blank == NULL
 *  then the caller blanks by setting the CLUT (Color Look Up Table) to all
 *  black. Return 0 if blanking succeeded, != 0 if un-/blanking failed due
 *  to e.g. a video mode which doesn't support it. Implements VESA suspend
 *  and powerdown modes on hardware that supports disabling hsync/vsync:
 *    blank_mode == 2: suspend vsync
 *    blank_mode == 3: suspend hsync
 *    blank_mode == 4: powerdown
 *
 * In the 16z044 blanking is supported via the DISPLAY_CONTROL register bit30.
 * When set, the graphics output becomes completely idle, so most modern
 * Monitors will shut down to save energy.
 *
 * \param \IN 	blank 	blanking method used
 * \param \IN   info	pointer to fb_info struct
 *
 */
static void men_16z044_blank(int blank, struct fb_info *info)
{

    unsigned int ctrl=0;

    struct MEN_16Z044_FB *fbP=men_16z044_from_info(info);

    if (!fbP )
		return;
    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP));

    if (!!blank)
		ctrl |= Z044_DISP_CTRL_ONOFF;
    else
		ctrl &= ~Z044_DISP_CTRL_ONOFF;

    /* bit31 must be set to '1' too to let changes take effect. */
    ctrl |= Z044_DISP_CTRL_CHANGE;
    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP) );

}

/**********************************************************************/
/** enable/disable the P018 Testpattern (a colored frame at the	edges
 *    of the Screen, determined by the current Resolution)
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 * \param \IN 	en  	1: test pattern on  0: disable it
 *
 * \returns  if success / negative errorcode on error
 */
static int men_16z044_EnableTestMode(struct MEN_16Z044_FB *fbP,
									 unsigned int en)
{
    unsigned int ctrl=0;
    if (!fbP)
		return -EINVAL;

    if (!fbP->dispctr_virt)
		return -EINVAL;

    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP) );
    if (!!en)
		ctrl |= Z044_DISP_CTRL_DEBUG;
    else
		ctrl &= ~Z044_DISP_CTRL_DEBUG;

    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP));
    return 0;
}


/**********************************************************************/
/** Switch the refresh rate between 60 Hz and 75Hz. (currently) only
 *  these Values are supported in the 16z044.
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 * \param \IN 	rate 	Value in Hz to set. currently just 60 or 75.
 *
 *
 * \returns 0 if success / negative errorcode on error
 */
static int men_16z044_SetRefreshRate( struct MEN_16Z044_FB *fbP,
									  unsigned int rate)
{

    unsigned int ctrl=0;

    if (!fbP )
		return -EINVAL;

    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP) );

    switch (rate){
    case MEN_16Z044_REFRESH_75HZ:
		DPRINTK("setting 75 Hz\n");
		ctrl |= Z044_DISP_CTRL_REFRESH;
		break;
    case MEN_16Z044_REFRESH_60HZ:
		DPRINTK("setting 60 Hz\n");
		ctrl &= ~Z044_DISP_CTRL_REFRESH;
		break;
    default:
		return -EINVAL;
    }

    ctrl |= Z044_DISP_CTRL_CHANGE;

    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP));

    return 0;

}


/**********************************************************************/
/**  Readout the current Resolution thats set in HW
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 *
 * \returns (0,1,2,3) as resolution if success or negative errorcode
 *
 */
static int men_16z044_GetResolution( struct MEN_16Z044_FB *fbP)
{
	unsigned int res_hi;
    unsigned int res = 0;
    if ( !fbP )
		return -EINVAL;

    res = readl( fb_men_16z044_DispCtrlBase(fbP) ) & 0x3;
	res_hi = readl( fb_men_16z044_DispCtrlBase(fbP) ) & 0x30;
	res |= res_hi >> 2;
	
	printk( KERN_INFO "16Z044 found. Resolution: %d x %d\n",
			G_resol[res].xres, G_resol[res].yres );
    return res;

}



/**********************************************************************/
/**  Set the Resolution to one of the 4 possible values.
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 * \param \IN 	res 	0: 640x480 1:800x600 2:1024x768 3:1280x1024
 *
 * \returns 0 if success / negative errorcode on error
 */
#if 0  /* not yet supported */
static int men_16z044_SetResolution( struct MEN_16Z044_FB *fbP,
									 unsigned int res)
{

    unsigned int ctrl=0,check=0;

    if ( !fbP || res > MEN_16Z044_RES_1280 )
		return -EINVAL;

    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP) );
    ctrl = (ctrl & 0xfffffffc)| res;

    /* set bit31 to let changes take effect */
    ctrl |= Z044_DISP_CTRL_CHANGE;

    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP));

    /* thats wrong (yet) anyway because a different
       FPGA content must be loaded which is a more complicated process */

    /* let new Resolution 'settle' */
    return 0;

}
#endif

/**********************************************************************/
/**  Set byte swapping of the 16bpp value according to architecture
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 * \param \IN 	en		1 to swap bytes (PPC), 0 to not swap bytes
 *
 * \returns 0 if success / negative errorcode on error
 */
static int men_16z044_ByteSwap( struct MEN_16Z044_FB *fbP, unsigned int en)
{

    unsigned int ctrl=0;
    DPRINTK("men_16z044_ByteSwap: en = %d\n", en);

    if ( !fbP )
		return -EINVAL;

    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP));

    /* set to 0 first */
    ctrl &= ~Z044_DISP_CTRL_BYTESWAP;

    if (!!en)
		ctrl |= Z044_DISP_CTRL_BYTESWAP;

    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP) );

    return 0;

}


/**********************************************************************/
/**  Switch flat panel on / off
 *
 * \param \IN 	fbP 	pointer to struct of 16z044 data
 * \param \IN 	en		0 to swap bytes , 1 to not swap bytes (PPC)
 *
 * \returns 0 if success / negative errorcode on error
 */
static int men_16z044_FlatPanel( struct MEN_16Z044_FB *fbP, unsigned int en)
{

    unsigned int ctrl=0;

    if ( !fbP )
		return -EINVAL;

    ctrl = readl( fb_men_16z044_DispCtrlBase(fbP) + MEN_16Z044_FP_CTRL);

    /* set to 0 first */
    ctrl &= ~( 0x7 );

    if (!!en)
		ctrl |= (0x7);

    writel( ctrl, fb_men_16z044_DispCtrlBase(fbP) + MEN_16Z044_FP_CTRL);

    return 0;

}


/**********************************************************************/
/** Support P018/P518 specific Hardware Functions via ioctls
 *
 * \param \IN 	inode
 * \param \IN 	file
 * \param \IN 	cmd
 * \param \IN 	arg
 * \param \IN 	con
 *
 * \returns Errorcode if error  or 0 on success
 */
/* perform fb specific ioctl (optional) */
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2,6,15))
static int men_16z044_ioctl(struct fb_info *info,
							unsigned int cmd,
							unsigned long arg )
#elif (LINUX_VERSION_CODE > KERNEL_VERSION(2,5,0))
	static int men_16z044_ioctl( struct inode *inode, struct file *file,
								 unsigned int cmd, unsigned long arg,
								 struct fb_info *info)
#endif
{

    struct MEN_16Z044_FB *fbP;
    unsigned int scrnr;

    fbP = men_16z044_from_info(info);
    scrnr=0;

    if (!fbP)
		return -EINVAL;

    DPRINTK("men_16z044_ioctl called: cmd = 0x%08x, arg = 0x%08x\n");


    switch (cmd){
    case FBIO_ENABLE_MEN_16Z044_TEST:
		DPRINTK("ioctl FBIO_ENABLE_MEN_16Z044_TEST\n");
		return men_16z044_EnableTestMode( fbP, 1);

    case FBIO_DISABLE_MEN_16Z044_TEST:
		DPRINTK("ioctl FBIO_DISABLE_MEN_16Z044_TEST\n");
		return men_16z044_EnableTestMode( fbP, 0);

    case FBIO_ENABLE_75HZ:
		DPRINTK("ioctl FBIO_ENABLE_75HZ\n");
		return men_16z044_SetRefreshRate( fbP, 75);

    case FBIO_ENABLE_60HZ:
		DPRINTK("ioctl FBIO_ENABLE_60HZ\n");
		return men_16z044_SetRefreshRate( fbP, 60);

    case FBIO_MEN_16Z044_SWAP_ON:
		DPRINTK("ioctl FBIO_MEN_16Z044_SWAP_ON\n");
		return men_16z044_ByteSwap( fbP, 1);

    case FBIO_MEN_16Z044_SWAP_OFF:
		DPRINTK("ioctl FBIO_MEN_16Z044_SWAP_OFF\n");
		return men_16z044_ByteSwap( fbP, 0);

    case FBIO_MEN_16Z044_BLANK:
		DPRINTK("ioctl FBIO_MEN_16Z044_BLANK\n");
		men_16z044_blank(1, info);
		return 0;

    case FBIO_MEN_16Z044_UNBLANK:
		DPRINTK("ioctl FBIO_MEN_16Z044_UNBLANK\n");
		men_16z044_blank(0, info);
		return 0;

    case FBIO_MEN_16Z044_SET_SCREEN:
		copy_from_user( (void*)&scrnr, (void*)arg, sizeof(scrnr));
		DPRINTK("ioctl FBIO_MEN_16Z044_SET_SCREEN. nr: %d\n", scrnr);
		return men_16z044_SetScreen(fbP, scrnr);

    default:
		return -EINVAL;
    }
}

static int men_16z044_mmap(struct fb_info *info, struct vm_area_struct *vma)
{
	unsigned long off = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long phys = info->fix.smem_start + off;
	unsigned long vsize = vma->vm_end - vma->vm_start;
	unsigned long psize = info->fix.smem_len - off;

	if (vsize > psize)
		return -EINVAL;

	vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);

	return vm_iomap_memory(vma, phys, vsize);
}

extern int soft_cursor(struct fb_info *info, struct fb_cursor *cursor);
static struct fb_ops men_16z044_ops = {
    .fb_setcolreg	= men_16z044_setcolreg,
    .fb_pan_display	= men_16z044_pan_display,
    .fb_fillrect	= cfb_fillrect,
    .fb_copyarea	= cfb_copyarea,
    /*       
     reminder from fh.h
    int (*fb_check_var)(struct fb_var_screeninfo *var, struct fb_info *info);
    int (*fb_set_par)(struct fb_info *info);
    */
    .fb_blank           = men_16z044_blank,
    .fb_check_var       = men_16z044_check_var,
    .fb_set_par         = men_16z044_set_par,
    .fb_imageblit	= cfb_imageblit,
#ifdef CONFIG_FRAMEBUFFER_CONSOLE
    .fb_cursor		= soft_cursor,
#endif /*CONFIG_FRAMEBUFFER_CONSOLE*/
    /* perform fb specific ioctl (optional) */
    .fb_ioctl		= men_16z044_ioctl,
    /* perform mmap on fb (optional) */
    .fb_mmap = men_16z044_mmap,
};

/**********************************************************************/
/** Init the Addresses and remapped Spaces of the FB
 *
 * \param \IN 	pointer to struct of 16z044 data to init
 *
 * \returns 0 if success / errorcode on error
 */
static unsigned int men_16z044_MapAdresses(struct MEN_16Z044_FB *fbP)
{

    /*
     * map memory unit
     */
    fbP->sdram_phys  = pci_resource_start( fbP->pdev, fbP->barSdram );
    fbP->sdram_size  = pci_resource_len(  fbP->pdev, fbP->barSdram );
    fbP->sdram_virt  = ioremap(fbP->sdram_phys, fbP->sdram_size);

    DPRINTK("fbP->sdram_phys=0x%08x ->sdram_size=0x%08x ->sdram_virt=%p\n", fbP->sdram_phys, fbP->sdram_size,fbP->sdram_virt );
    fbP->mmio_start  = fbP->sdram_phys; /* needed in fb subsystem */
    fbP->mmio_len    = fbP->sdram_size;

    /*
     * map 16Z044_DISP unit
     */
    fbP->dispctr_phys  = pci_resource_start( fbP->pdev, fbP->barDisp );
    fbP->dispctr_size  = pci_resource_len( fbP->pdev, fbP->barDisp );
    fbP->dispctr_virt  = ioremap(fbP->dispctr_phys, fbP->dispctr_size);
    DPRINTK("fbP->dispctr_phys=0x%08x fbP->dispctr_size=0x%08x\n", fbP->dispctr_phys, fbP->dispctr_size );

    if (!fbP->dispctr_phys || !fbP->dispctr_size)
    {
      printk(KERN_ERR "*** %s: invalid BAR content (disp ctrl)\n",
	     __FUNCTION__);
      return -ENOMEM;
    }

    return 0;
}


static void men_16z044_InitVarFb( struct MEN_16Z044_FB *fbP )
{

    fbP->var.xres 			= fbP->var.xres_virtual = fbP->xres;
    fbP->var.yres 			= fbP->var.yres_virtual = fbP->yres ;
    fbP->var.bits_per_pixel 		= fbP->bits_per_pixel; /* JT 16; */
    fbP->var.grayscale 			= 0;	/* != 0 Graylevels instead of colors */
    fbP->bytes_per_pixel 		= 2;

    switch (fbP->var.bits_per_pixel) {
    case 15:/* 32k */
    case 16:
		fbP->var.red.offset		= 11;
		fbP->var.red.length		= 5;
		fbP->var.green.offset  	= 5;
		fbP->var.green.length  	= 6;
		fbP->var.blue.offset	= 0;
		fbP->var.blue.length	= 5;
		break;
	case 4:
	case 8: /*ts 23.05.2016 for MP70S*/
		break;
    default:/* not supported (yet) */
		printk(KERN_ERR "no support for %dbpp\n",fbP->var.bits_per_pixel);
		/* fbP->disp.dispsw = &fbcon_dummy;   /\* ??? *\/ */
		break;
    }

    fbP->var.nonstd 	  	= 0;  /* != 0 Non standard pixel format 	*/
    fbP->var.activate   	= 0;  /* see FB_ACTIVATE_*				*/
    fbP->var.height 	  	= -1;	/* height of picture in mm    		*/
    fbP->var.width 	  		= -1;	/* width of picture in mm     		*/
    fbP->var.accel_flags 	= 0; 	/* FB_ACCELF_TEXT; accel flags (hints)	*/

    /* Timing: All values in pixclocks, except pixclock (of course) */
    fbP->var.pixclock 		= 25000;/* pixel clock in pico seconds 	*/
    fbP->var.left_margin 	= 0;	/* time from sync to picture	*/
    fbP->var.right_margin 	= 0;	/* time from picture to sync	*/
    fbP->var.upper_margin 	= 0;	/* time from sync to picture	*/
    fbP->var.lower_margin 	= 0;
    fbP->var.hsync_len 		= 0;	/* length of horizontal sync	*/
    fbP->var.vsync_len 		= 0;	/* length of vertical sync		*/
    fbP->var.sync 			= 0;	/* see FB_SYNC_*				*/
    fbP->var.vmode 			= FB_VMODE_NONINTERLACED; /* see FB_VMODE_* */

}


static void men_16z044_InitInfo( struct MEN_16Z044_FB *fbP )
{

    /* ---  4. init  fb_info -- */
    fbP->info.var			= fbP->var;
    fbP->info.fix			= fbP->fix;
    fbP->info.screen_base		= fbP->sdram_virt;
    fbP->info.screen_size		= fbP->sdram_size;
    fbP->info.pseudo_palette		= fbP->palette;
    fbP->info.flags 			= FBINFO_FLAG_DEFAULT;
    fbP->info.fbops 			= &men_16z044_ops;
    fbP->info.node 			= -1;
    /* store address of 'this' 16z044  */
    fbP->info.par 				= (void*)fbP;
    DPRINTK("&fbP->info.par = %p\n", fbP->info.par);

}




static void men_16z044_InitFixFb( struct MEN_16Z044_FB *fbP )
{

/* struct fb_fix_screeninfo didnt change in 2.6 */
    strcpy(fbP->fix.id, fbP->name);	 /* ident string ( char[16] ) */
    fbP->fix.type 		= FB_TYPE_PACKED_PIXELS;   /* see FB_TYPE_* */
    fbP->fix.type_aux  		= 0;  	/* Interleave for interleaved Planes */
    fbP->fix.visual		= FB_VISUAL_TRUECOLOR;
    fbP->fix.xpanstep  		= 0;
    fbP->fix.ypanstep  		= 0;
    fbP->fix.ywrapstep 		= 0;
    fbP->fix.line_length 	= fbP->line_length; /* len of a line in bytes 	*/
    fbP->fix.smem_start 	= fbP->sdram_phys;
    fbP->fix.smem_len 		= fbP->sdram_size;
    fbP->fix.mmio_start 	= fbP->mmio_start;
    fbP->fix.mmio_len 		= fbP->mmio_len;
    fbP->fix.accel 			= 0;

}



static void men_16z044_InitDisp( struct MEN_16Z044_FB *fbP )
{

}


/**********************************************************************/
/** Init all structs contained in the main 16z044 struct
 *
 * \param \IN 	fbP			pointer to struct of 16z044 data to init
 * \param \IN 	instCount 	index of the FB Instance number found
 *							( = number of e.g. P18 Modules in System)
 * \returns 0 if success / errorcode on error
 */
static unsigned int men_16z044_InitDevData(struct MEN_16Z044_FB *fbP,
										   unsigned int instCount)
{


    unsigned int res = 0, i = 0;

#ifdef CONFIG_PPC /* TODO: might need to be refined in future ?*/
    fbP->byteswap = 1;
#else
    fbP->byteswap = 0;
#endif

#ifdef MODULE
    if( (refresh == 75) || \
		(refresh == 60)){
		DPRINTK("refresh rate = %d\n", refresh);
		fbP->refresh_rate = refresh;
    } else
		printk(KERN_WARNING " *** %s: invalid refresh value\n", __FUNCTION__);
#endif

    if ( men_16z044_MapAdresses( fbP )){
		printk(KERN_ERR " *** %s: cant remap adresses\n", __FUNCTION__);
		return -ENODEV;
    }

    /* setup default color table (by fbcon) */
    for(i = 0; i < FB_16Z044_COLS; i++) 
      {
#if 0
	int j = color_table[i];
	fbP->palette[i].red    = default_red[j];
	fbP->palette[i].green  = default_grn[j];
	fbP->palette[i].blue   = default_blu[j];
#else
#warning special palette handling for Z44 !

	fbP->palette[i].red    = z44rd[i] & 0xff;
	fbP->palette[i].green  = z44gn[i] & 0xff;
	fbP->palette[i].blue   = z44bl[i] & 0xff;
#endif
      }

    /* set this 16z044s resolution to the one found in HW */
    if ((res = men_16z044_GetResolution( fbP )) < 0 )
		return -EINVAL;

    sprintf( fbP->name, "%s_%d", MEN_FB_NAME, instCount );

    fbP->bits_per_pixel  	= G_resol[res].bits_per_pixel;
    fbP->bytes_per_pixel 	= fbP->bits_per_pixel >> 3;
    fbP->xres 			= G_resol[res].xres;
    fbP->yres 			= G_resol[res].yres;
    fbP->line_length 		= fbP->xres * fbP->bytes_per_pixel;

    /* Initialize all needed Structs for the Framebuffer subsystem */
    men_16z044_InitFixFb( fbP );
    men_16z044_InitVarFb( fbP );
    men_16z044_InitDisp( fbP );
    men_16z044_InitInfo( fbP );
    DPRINTK("finally unblank screen, setup initial swap/refresh Values\n");

    /* finally unblank screen, setup initial swap/refresh Values */
    men_16z044_blank(0, &fbP->info);

    if (fbP->byteswap)
		men_16z044_ByteSwap( fbP, 1);

    if (fbP->refresh_rate == 75)
		men_16z044_SetRefreshRate( fbP, 75);
    else
		men_16z044_SetRefreshRate( fbP, 60);

    /* new: Flatpanel Register , switch it on*/
    men_16z044_FlatPanel( fbP, 1 );

    return 0;

}


/**********************************************************************/
/** allocate one Element of the linked list and return it.
 *
 * \brief  men_16z044_AllocateDevice allocates all memory
 *		   needed to hold one 16Z044 device struct. Clears the struct
 *		   to zero.
 *
 * \returns    pointer to the new struct or NULL if error
 */
static struct MEN_16Z044_FB *men_16z044_AllocateDevice(void)
{

    struct MEN_16Z044_FB *newP = NULL;

    newP = (struct MEN_16Z044_FB*)(kmalloc(sizeof(struct MEN_16Z044_FB),
										   GFP_KERNEL));
    if (!newP)
		return NULL;

    memset( newP, 0, sizeof(struct MEN_16Z044_FB));

    return newP;

}


/***************************************************************************/
/** setup and options processing function
 *
 * \param \IN	options    string of passed video kernel options
 *
 * \returns 0
 */
int __init men_16z044_setup(char *options)
{

    char *this_opt;

	if (!options)
		options = "ref60";

    DPRINTK(" *** %s options: '%s'\n", __FUNCTION__, options);

    while ((this_opt = strsep(&options, ",")) != NULL) {
		if (!*this_opt) continue;
		else if (! strcmp(this_opt, "ref75"))
			refresh = MEN_16Z044_REFRESH_75HZ;
		else if (! strcmp(this_opt, "ref60"))
			refresh = MEN_16Z044_REFRESH_60HZ;
    }

    return 0;
}

#define MAX_INST_NR  0x10

/*******************************************************************/
/** find the memory matching the framebuffer IP core
 *
 * \param chu	\IN data of found unit, passed by chameleon driver
 *
 * \return 0 on success or negative linux error number
 *
 */
static int fb16z044_find_memory(CHAMELEONV2_UNIT_T *chu, CHAMELEONV2_UNIT_T *u)
{

    int i = 0;
    int rc;

    DPRINTK(KERN_INFO "fb16z044_find_memory: chu->unitFpga.group = %d chu->unitFpga.devId = 0x%02x\n", chu->unitFpga.group, chu->unitFpga.devId );

    memset( u, 0, sizeof(CHAMELEONV2_UNIT_T));
    /* 1. search for a matching 16Z043 (same group and Instance nr. 0) */

    for (i=0; i < MAX_INST_NR; i++) {
      rc = men_chameleonV2_unit_find( IP_CORE_16Z043, i, u );
      if ((rc == 0 ) && (chu->unitFpga.group == u->unitFpga.group))
	break;
    }

    if (i == MAX_INST_NR) 
      goto next;  /* no Z43 found */


    DPRINTK(KERN_INFO "%s: Found Z043 SDRAM on bar %d\n", MEN_FB_NAME, u->unitFpga.bar);
    DPRINTK(KERN_INFO "devId: %x \n", u->unitFpga.devId);
    DPRINTK(KERN_INFO "bar: %d \n", u->unitFpga.bar);
    DPRINTK(KERN_INFO "addr: %p \n", u->unitFpga.addr);
    DPRINTK(KERN_INFO "group: %d \n", u->unitFpga.group);

    return 0;
	
 next:

    for (i=0; i < MAX_INST_NR; i++) {
      rc = men_chameleonV2_unit_find( IP_CORE_16Z024, i, u );
      if ((rc == 0 ) && (chu->unitFpga.group == u->unitFpga.group))
	break;
    }          

    if (i == MAX_INST_NR)
    {
	printk( KERN_ERR "*** %s: cant find Z024 or Z043 SDRAM !\n", MEN_FB_NAME );
	goto none;
    }

    DPRINTK(KERN_INFO "%s: Found Z024 SRAM:\n", MEN_FB_NAME);
    DPRINTK(KERN_INFO "devId:    %x \n", u->unitFpga.devId);
    DPRINTK(KERN_INFO "bar:      %d \n", u->unitFpga.bar);
    DPRINTK(KERN_INFO "addr:     %p \n", u->unitFpga.addr);
    DPRINTK(KERN_INFO "group:    %d \n", u->unitFpga.group);
      
    if (chu->unitFpga.group == u->unitFpga.group)
    {
      printk(KERN_INFO "%s: Found Z024 SRAM on bar %d\n", MEN_FB_NAME, u->unitFpga.bar);
    } 
    else 
    {
	printk(KERN_INFO "*** %s: Found Z024 SRAM but group %d doesnt match DISP controller!\n", MEN_FB_NAME, u->unitFpga.group);
	goto none;
    }
    return 0;

 none:
      return -ENODEV;
}

/*******************************************************************/
/** PNP function for Framebuffer
 *
 * \param chu	\IN data of found unit, passed by chameleon driver
 *
 * \return 0 on success or negative linux error number
 *
 */
static int __init fb16z044_probe( CHAMELEONV2_UNIT_T *chu )
{

    struct MEN_16Z044_FB *drvDataP = NULL;
    unsigned int cnt=0, barSdram, barDisp, d_offs = 0;
    struct pci_dev *chamDev=NULL;
    CHAMELEONV2_UNIT_T u;

    DPRINTK(KERN_INFO "fb16z044_probe: DISP info:\n");
    DPRINTK(KERN_INFO "devId: %x \n", chu->unitFpga.devId);
    DPRINTK(KERN_INFO "bar: %d \n",   chu->unitFpga.bar);
    DPRINTK(KERN_INFO "addr: %p \n",  chu->unitFpga.addr);
    DPRINTK(KERN_INFO "group: %d \n", chu->unitFpga.group);

    /* store the DISP data */
    barDisp = chu->unitFpga.bar;
    chamDev = chu->pdev;
    d_offs  = chu->unitFpga.offset;

    /* find our connected memory (SDRAM or SRAM) in FPGA. */
    memset( &u, 0, sizeof(CHAMELEONV2_UNIT_T) );

    if (fb16z044_find_memory(chu, &u) == -ENODEV) {
      printk( KERN_CRIT "*** fb16z044_probe: could not find neither 16Z024 SRAM or 16Z043 SDRAM unit matching 16Z044 chameleon group, exiting.\n");
      return -ENODEV;
    }

    barSdram = u.unitFpga.bar;


    /*------------------------------+
     | alloc space for one FB device|
     +------------------------------*/
    if (!(drvDataP = men_16z044_AllocateDevice()) ){
		printk(KERN_ERR "*** %s: cant allocate device.\n", MEN_FB_NAME);
		return -ENOMEM;
    }

    drvDataP->pdev 	= chamDev;
    drvDataP->barSdram 	= barSdram;
    drvDataP->barDisp	= barDisp;
    drvDataP->disp_offs = d_offs;

    DPRINTK("barSdram=%d barDisp=%d offset disp= %04x\n",
			barSdram ,barDisp, d_offs );

    if ( men_16z044_InitDevData( drvDataP , cnt))
		return -ENOMEM;

    if (register_framebuffer( &drvDataP->info ) < 0 )
		return -EINVAL;

    chu->driver_data = drvDataP; /* chu = DISP unit here for later remove() */

    return 0;
}



/**********************************************************************/
/** Framebuffer driver deregistration from men_chameleon subsystem
 *
 * \param  chu  IN Chameleon unit to deregister
 *
 * \returns Errorcode if error  or 0 on success
 */
static int fb16z044_remove( CHAMELEONV2_UNIT_T *chu )
{

    struct MEN_16Z044_FB *fbP = (struct MEN_16Z044_FB *)(chu->driver_data);
    struct fb_info *info = &fbP->info;

    if (!fbP){
		printk(KERN_ERR "*** error: internal driver data corrupt!\n");
		return -EBUSY;
    }

    if (info) {
		unregister_framebuffer(info);
		framebuffer_release(info);
		iounmap(fbP->sdram_virt );
		iounmap(fbP->dispctr_virt);
		kfree(fbP);
    } else {
		printk(KERN_ERR "*** error: internal driver data corrupt!\n");
		return -EBUSY;
    }
    return 0;
}

static const u16 G_devIdArr[] = { IP_CORE_16Z044,
				  CHAMELEONV2_DEVID_END };

static CHAMELEONV2_DRIVER_T G_driver = {
    .name	= "fb16z044",
    .devIdArr 	= G_devIdArr,
    .probe	= fb16z044_probe,
    .remove	= fb16z044_remove
};


/**********************************************************************/
/** Framebuffer driver registration / initialization at men_chameleon
 *  subsystem
 *
 * \returns Errorcode if error  or 0 on success
 */
int __init men_16z044_init(void)
{
  int i;
  printk("MEN 16Z044 IP core framebuffer driver \n");

  return men_chameleonV2_register_driver( &G_driver ) ? 0 : -ENODEV;
}


/**********************************************************************/
/** modularized cleanup function
 *
 *
 */
static void __exit men_16z044_cleanup(void)
{
    /* this calls .remove() automatically */
    men_chameleonV2_unregister_driver( &G_driver );
}

/* let fb driver be configured with kernel parameters */
__setup( "fb16z044_mode=", men_16z044_setup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("thomas.schnuerer@men.de");
MODULE_DESCRIPTION("MEN 16z044 Framebuffer driver");

module_param( refresh, uint, 0 );
MODULE_PARM_DESC(refresh, "refresh rate in Hz: refresh=[60 or 75]");

int z44rd[16];
module_param_array(z44rd, int, NULL, 0); 
MODULE_PARM_DESC(z44rd, "array of the 16 red-components in console palette, 8bit only!");

int z44gn[16];
module_param_array(z44gn, int, NULL, 0); 
MODULE_PARM_DESC(z44gn, "array of the 16 green components in console palette, 8bit only!");

int z44bl[16];
module_param_array(z44bl, int, NULL, 0); 
MODULE_PARM_DESC(z44bl, "array of the 16 blue components in console palette, 8bit only!");



module_init(men_16z044_init);
module_exit(men_16z044_cleanup);
