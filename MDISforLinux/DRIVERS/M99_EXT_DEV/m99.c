/****************************************************************************
 * Copyright 2019, MEN Mikro Elektronik GmbH
 ****************************************************************************/
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

#include <asm/system.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>

#define MAC_MEM_MAPPED
#define LINUX
#include <MEN/maccess.h>
#include <MEN/mk_nonmdisif.h>
#include <MEN/mdis_com.h>
#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/ll_defs.h>


/* MC68230 registers */
#define PGC_REG    0x80    /* port general control */
#define PADD_REG   0x84    /* port A data dir */
#define PBDD_REG   0x86    /* port B data dir */
#define PAC_REG    0x8c    /* port A control */
#define PBC_REG    0x8e    /* port B control */
#define PAD_REG    0x90    /* port A data */
#define PBD_REG    0x92    /* port B data */
#define TC_REG     0xa0    /* timer control */
#define TIV_REG    0xa2    /* timer irq vector, not used - no vector from modul supported */
#define CPH_REG    0xa6    /* counter preload high */
#define CPM_REG    0xa8    /* counter preload middle */
#define CPL_REG    0xaa    /* counter preload low */
#define TS_REG     0xb4    /* timer status */
#define M99_CLOCK_FREQ  250000


static void *G_dev;
static MACCESS G_ma;

	
static int IrqHandler(void *data)
{
	printk( "M99 irqHandler\n" );

	MWRITE_D16(G_ma, TS_REG, 0xff);  /* clear interrupt */

	return LL_IRQ_DEVICE;
}

static void setTime
(
    unsigned int timerval
)
{
    MWRITE_D16( G_ma, CPL_REG, timerval       & 0xff);
    MWRITE_D16( G_ma, CPM_REG, (timerval >> 8)  & 0xff);
    MWRITE_D16( G_ma, CPH_REG, (timerval >> 16) & 0xff);
}/*setTime*/

/* called when module is inserted */
static int __init init_mod(void)
{
	int ret, i;
	OSS_HANDLE *osh;

	printk( "init_mod\n" );

	if( (ret = mdis_open_external_dev( 0, "m99", "c203_1", 0,
									   MDIS_MA08, MDIS_MD16,
									   256, (void *)&G_ma,
									   (void *)&osh, &G_dev )) < 0)
		return ret;


	printk( "init_mod: open done ma=%p\n", (void *)G_ma );

	/* init M99, start IRQ */
	MREAD_D16( G_ma, 0x00 );
	MWRITE_D16( G_ma, PGC_REG,  0x00 );
	MWRITE_D16( G_ma, PAD_REG,  0xaa );
	MWRITE_D16( G_ma, PADD_REG, 0xff );
	MWRITE_D16( G_ma, PBDD_REG, 0x00 );
	MWRITE_D16( G_ma, PAC_REG,  0x00 );
	MWRITE_D16( G_ma, PBC_REG,  0x00 );
	MWRITE_D16( G_ma, TIV_REG,  0x0f ); /* as default after reset */
	MWRITE_D16(G_ma, TS_REG, 0xff);  /* clear interrupt */

	if( (ret = mdis_install_external_irq( G_dev, IrqHandler, (void *)1234 ))<0)
		return ret;

	setTime( 250000 );

#if 1
	MWRITE_D16( G_ma, TC_REG, 0xa1 ); /* enable timer irq */
#endif
	for(i=0; i<256; i+=2 )
		printk( "%02x: %02x\n", i,  MREAD_D16( G_ma, i ));

	printk( "TC_REG=%02x\n", MREAD_D16( G_ma, TC_REG ));
	printk( "PADD_REG=%02x\n", MREAD_D16( G_ma, PADD_REG ));
	printk( "TS_REG(1)=%02x\n", MREAD_D16( G_ma, TS_REG ));
	OSS_Delay( osh, 2000 );

	printk( "TS_REG=%02x\n", MREAD_D16( G_ma, TS_REG ));


    return 0;
}

/* called when module is removed */
static void __exit cleanup_mod(void)
{
	mdis_close_external_dev( G_dev );

    return;
}

module_init(init_mod);
module_exit(cleanup_mod);

