/*********************  P r o g r a m  -  M o d u l e ***********************/
/*!
 *        \file  men_16z077_eth.c
 *      \author  thomas.schnuerer@men.de
 *        $Date: 2017/05/08 $
 *
 *        \brief driver for IP core 16Z087 (Ethernet cores).
 *               supports kernel 3.0 to 4.8
 *
 *     Switches:
 */
/*-------------------------------[ History ]---------------------------------
 *
 * git controlled file. Latest CVS logmessage was:
 * Revision 1.45  2014/07/16 19:30:45  ts
 * R: 1. Compilerwarning: incompatible pointer type of .ndo_vlan_rx_add_vid
 *    2. several compilerwarnings about typecasts with gcc 4.8, kernel 3.14
 * M: 1. corrected declaration of z77_vlan_rx_add_vid
 *    2. changed variable types to appropriate ones, use unsigned long etc.
 *
 * For all later versions see git commit message from MEN website!  
 *
 ****************************************************************************/
/* 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/dma-mapping.h>
#include <linux/etherdevice.h>
#include <linux/netdevice.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/if_ether.h>
#include <linux/if_vlan.h>
#include <linux/version.h>
#include <linux/ethtool.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/i2c.h>
#include <linux/pci.h>
#include <linux/mii.h>
#include <asm/page.h>

#include <MEN/men_typs.h>
#include <MEN/oss.h>
#include <MEN/mdis_err.h>
#include <MEN/dbg.h>
#include <MEN/men_chameleon.h>
#include <MEN/smb2.h>
#include "men_16z077_eth.h"

/* defines */

/* max. # of supported Z87 Instances in the system */
#define NR_ETH_CORES_MAX		8
/* timeout to wait for reply from MII */
#define MII_ACCESS_TIMEOUT 		10000
/*  max. loops to wait for Idle condition */
#define IP_CORE_TIMEOUT 		200
#define PHY_MAX_ADR				31
#define MAX_MCAST_LST			64			/* we store up to 64 addresses */
#define MAC_ADDR_LEN			6			/* MAC len in byte */
#define MCAST_HASH_POLYNOM  	0x04C11DB7	/* CRC32 polynom for hash calculation */
#define MCAST_HASH_CRC_SEED 	0xFFFFFFFF	/* initial polynom seed */
#define MCAST_MULT_SHFT 		26			/* bit shift to calculate hash bin */
#define MCAST_HASH_MASK			0x3f    	/* pass up only lower 6 bit */
#define LEN_CRC					4			/* additional CRC bytes present in Frames */
#define I2C_MAX_ADAP_CNT		16			/* max. # of I2C adapters to query ID EEPROM */
#define MEN_BRDID_EE_ADR   		0x57		/* ID EEPROM addres on F1x cards = 0x57 */
#define ID_EE_NAME_OFF 			9			/* board name offset in ID EEPROM */
#define MEN_BRDID_EE_MAC_OF 	0x90		/* begin of MAC(s) in Board ID EEPROM */
#define ID_EE_NAME_LEN			6			/* length of name in ID EEPROM */
#define RX_BD_ALL_FULL 			(0xffffffff)

/* all used IRQs on the Z87 */
#define Z077_IRQ_ALL 			(OETH_INT_TXE | OETH_INT_RXF | OETH_INT_RXE | OETH_INT_BUSY | OETH_INT_TXB)

/* from 3.1 on (acc. to free electrons) DMA bit mask changed */
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,1,0)
# define Z87_BIT_MASK_32BIT		DMA_BIT_MASK(32)
#else
# define Z87_BIT_MASK_32BIT		DMA_32BIT_MASK
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,10,0)
# define Z87_VLAN_FEATURES    	(NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_CTAG_RX)
#else
# define Z87_VLAN_FEATURES    	(NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,0,0)
# define vlan_tag_present_func(x) 		vlan_tx_tag_present(x)
# define vlan_tag_get_func(x) 			vlan_tx_tag_get(x)
#else
# define vlan_tag_present_func(x) 		skb_vlan_tag_present(x)
# define vlan_tag_get_func(x) 			skb_vlan_tag_get(x)
#endif

#ifdef NIOS_II
# error NIOS_II (non-MMU driver) not longer supported. Use older driver if needed!
#endif

#define VLAN_TAG_SIZE			4		/* how much longer VLAN frames are */
#define ETH_SRCDST_MAC_SIZE		12		/* combined length or source and dest MAC addr */

#if defined(CONFIG_MENEP05)
#error EP05 no longer supported
#endif

#ifdef DBG
#define Z77DBG(lvl,msg...) 	  if (np->msg_enable >= lvl)	   \
										 printk( msg );
#else
#define Z77DBG(lvl,msg...) 	  do {} while (0)
#endif

/* F218 uses other Micrel PHY */
#define PHY_ID2_KSZ8041_1	0x1512
#define PHY_ID2_KSZ8041_2	0x1513

/* Typedefs */

/* Add more possible PHY IDs here */
static PHY_DEVICE_TBL z077PhyAttachTbl[] = {
	{ 0x0022, "Micrel "			},		/* 	Micrel PHY on EM01				*/
	{ 0x0141, "Marvell 88E6095"	},		/* 	Switch F301, F302				*/
	{ 0x0143, "Broadcom BCM5481"},		/* 	F11S 							*/
	{ 0x0013, "LXT971"			},		/*  P511+US03	 					*/
	{ 0x000d, "MEN PHY"			},		/*  dummy PHY in F218 rear Ethernet	*/
	{ 0xffff, ""}	/* end marker */
};

/**
 * z077_private: main data struct for the driver \n
 * this struct keeps all data for a per-IP-core instance of 16Z077/87
 */
/*@{*/
struct z77_private {
	long 				open_time;			/*!< how long is device open already */
    long	  			flags;				/*!< Our local flags 				 */
	u32  				instance;			/*!< chameleon instance if more Z87  */
	u32					phymode;			/*!< chameleon instance if more Z87  */
	u32  				instCount;			/*!< global probe cnt. for phyadr[]  */
	u32					nCurrTbd;			/*!< currently used Tx BD 			*/
	u32					txIrq;				/*!< last serviced TX IRQ			*/
	u32					modCode;			/*!< chameleon modCode				*/
	unsigned long		bdBase;				/*!< start address of BDs in RAM 	*/
	dma_addr_t 			bdPhys;				/*!< DMA address of BDs in RAM 		*/
	u32 				pollInProgress;
	u32 				bdOff;				/*!< BDoffset from BAR start 		*/
	u32 				tbdOff;				/*!< TX Buffer offset to phys base  */
	u32 				rbdOff;				/*!< TX Buffer offset to phys base  */
	u32					serialnr;			/*!< serial nr. of P51x or MM1 		*/
	u32					board;				/*!< board identifier of this eth 	*/
	SMB_DESC_PORTCB 	smb2desc;       	/*!< SMB2 descriptor for EEPROM     */
	void            	*smbHdlP;			/*!< SMB2 Handle 					*/
	struct work_struct 	reset_task; 		/*!< process context reseting (ndo_tx_timeout) */
	struct timer_list 	timer;				/*!< period timer for linkchange poll */
	u32					prev_linkstate;		/*!< previous link state */
#if defined(Z77_USE_VLAN_TAGGING)
	struct vlan_group	*vlgrp; 			/*!< VLAN tagging group */
	u32					err_vlan;			/*!< VLAN error flags/count */
# define RXD_VLAN_MASK		0x0000ffff
# define VLAN_ETHER_TYPE 	0x8100
#endif
	u8					mcast_lst[MAX_MCAST_LST][MAC_ADDR_LEN]; /*!< store Mcast addrs	*/
	Z077_BD				txBd[Z077_TBD_NUM];	/*!< Tx Buffer Descriptor address 			*/
	Z077_BD				rxBd[Z077_RBD_NUM]; /*!< Rx Buffer Descriptor address 			*/
	struct net_device_stats stats;			/*!< status flags to report to IP 			*/
	spinlock_t 			lock;				/*!< prevent concurrent accesses 			*/
	struct pci_dev		*pdev;				/*!< the pci device we belong to 			*/
	struct mii_if_info 	mii_if;				/*!< MII API hooks, info 					*/
	u32 				msg_enable;			/*!< debug message level 					*/
	struct napi_struct 	napi;       		/*!< NAPI struct 							*/
	struct net_device  	*dev;       		/*!< net device 							*/
};
/*@}*/

static int	z77_open(struct net_device *dev);
static int	z77_send_packet(struct sk_buff *skb, struct net_device *dev);
static irqreturn_t z77_irq(int irq, void *dev_id);
static int	z77_close(struct net_device *dev);
static struct	net_device_stats *z77_get_stats(struct net_device *dev);
static void z77_tx_timeout(struct net_device *dev);
static void z77_rx_err(struct net_device *dev );
static void z77_tx_err(struct net_device *dev );
static int z77_poll(struct napi_struct *napi,int budget);
static int chipset_init(struct net_device *dev, unsigned int first_init);
static int z77_phy_reset(struct net_device *dev, u8 phyAddr);
static int z77_phy_identify(struct net_device *dev, u8 phyAddr);
static int z77_phy_init(struct net_device *dev);
static int z77_init_phymode(struct net_device *dev, u8 phyAddr);
static void z77_pass_packet( struct net_device *dev, unsigned int idx );
static int z77_mdio_read(struct net_device *dev, int phy_id, int location);
static void z77_mdio_write(struct net_device *dev, int phy_id, int location, int val);
static void z77_reset( struct net_device *dev );
static int z77_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd);
static void z77_hash_table_setup(struct net_device *dev);
static int ether_gen_crc(struct net_device *dev, u8 *data);
#if defined(Z77_USE_VLAN_TAGGING)
# if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
static void z77_vlan_rx_register(struct net_device *dev, struct vlan_group *grp);
static void z77_vlan_rx_kill_vid(struct net_device *dev, unsigned short vid);
# endif
static int z77_vlan_rx_add_vid(struct net_device *dev, unsigned short proto, unsigned short vid);
#endif
static void z77_timerfunc(unsigned long);

/*  Globals  */

/* filled depending on found IP core, either "16Z077" or "16Z087" */
static char cardname[16];
static const char *version = "$Id: men_16z077_eth.c,v 1.45 2014/07/16 19:30:45 ts Exp $";

/* helper for description of phy advertised/supported capabilities*/
static const char *phycaps[]={	"10baseT_Half",	"10baseT_Full", "100baseT_Half", "100baseT_Full", "1000baseT_Half", "1000baseT_Full",	"Auto",	"TP", "AUI", "MII",	"FIBRE", "BNC", "10000baseT_Full", "Pause", "Asym_Pause" };

enum phymodes {
 	phymode_auto,		/* 0 */
 	phymode_10hd,		/* 1 */
 	phymode_10fd,		/* 2 */
 	phymode_100hd,		/* 3 */
 	phymode_100fd		/* 4 */
};

/* global to count detected instances */
static int G_globalInstanceCount = 0;

/* array to tokenize / store multiple phy modes */
#define PHY_MODE_NAME_LEN	(5+1)
#define MAC_ADDR_NAME_LEN   (13+1)

static int nrcores  = NR_ETH_CORES_MAX;
static int dbglvl   = 0;
static int phyadr[NR_ETH_CORES_MAX]    = {0,0,0,0,0,0,0,0};
static int mode[NR_ETH_CORES_MAX]      = {0,0,0,0,0,0,0,0};

module_param_array( mode, int, (void*)&nrcores, 0664 );
MODULE_PARM_DESC( mode, " 0=autoneg 1=10MbitHD 2=10MbitFD 3=100MbitHD 4=100MbitFD   example: mode=4,0,0");
module_param_array( phyadr, int, (void*)&nrcores, 0664 );
MODULE_PARM_DESC( phyadr, " address of PHY#n connected to each Z87 unit. example: phyadr=1,2,0");
module_param( dbglvl, int, 0664 );
MODULE_PARM_DESC( dbglvl, " 0=none 1=basic 2=verbose 3=very verbose (dumps every packet, use with care!). ");

/* helper to keep Register descriptions in a comfortable struct */
const Z077_REG_INFO z77_reginfo[] = {
	{"MODER     ", Z077_REG_MODER		},
	{"INT_SRC   ", Z077_REG_INT_SRC 	},
	{"INT_MASK  ", Z077_REG_INT_MASK 	},
	{"IPGT      ", Z077_REG_IPGT 		},
	{"IPGR1     ", Z077_REG_IPGR1 		},
	{"IPGR2     ", Z077_REG_IPGR2 		},
	{"PACKLEN   ", Z077_REG_PACKLEN 	},
	{"COLLCONF  ", Z077_REG_COLLCONF 	},
	{"TX_BDNUM  ", Z077_REG_TX_BDNUM 	},
	{"CTRLMODER ", Z077_REG_CTRLMODER 	},
	{"MIIMODER  ", Z077_REG_MIIMODER 	},
	{"MIICMD    ", Z077_REG_MIICMD 		},
	{"MIIADR    ", Z077_REG_MIIADR 		},
	{"MIITX_DATA", Z077_REG_MIITX_DATA 	},
	{"MIIRX_DATA", Z077_REG_MIIRX_DATA 	},
	{"MIISTATUS ", Z077_REG_MIISTATUS 	},
	{"MAC_ADDR0 ", Z077_REG_MAC_ADDR0 	},
	{"MAC_ADDR1 ", Z077_REG_MAC_ADDR1	},
	{"HASH_ADDR0", Z077_REG_HASH_ADDR0 	},
	{"HASH_ADDR1", Z077_REG_HASH_ADDR1 	},
	{"TXCTRL    ", Z077_REG_TXCTRL		},
	{"GLOBAL RST", Z077_REG_GLOBALRST	},
	{"BD_START  ", Z077_REG_BDSTART		},
	{"RX_EMPTY0 ", Z077_REG_RXEMPTY0	},
	{"RX_EMPTY1 ", Z077_REG_RXEMPTY1	},
	{"TX_EMPTY0 ", Z077_REG_TXEMPTY0	},
	{"TX_EMPTY1 ", Z077_REG_TXEMPTY1	},
	{"RX_BDSTAT ", Z077_REG_RXBDSTAT	},
	{"SMBCTRL   ", Z077_REG_SMBCTRL  	},
	{"COREREV   ", Z077_REG_COREREV  	},
	{"RXERRCNT1 ", Z077_REG_RXERRCNT1	},
	{"RXERRCNT2 ", Z077_REG_RXERRCNT2	},
	{"TXERRCNT1 ", Z077_REG_TXERRCNT1	},
	{"TXERRCNT2 ", Z077_REG_TXERRCNT2	},
	{NULL, 0xffff} /* end mark */
};

/*****************************************************************************/
/** Write MAC address from struct net to Registers MAC_ADDR0/1
 *
 *
 * \param dev		\IN net_device struct for this NIC
 */
static void z77_store_mac(struct net_device *dev)
{
	Z77WRITE_D32( Z077_BASE, Z077_REG_MAC_ADDR0, dev->dev_addr[2] << 24 | dev->dev_addr[3] << 16 | dev->dev_addr[4] << 8  | dev->dev_addr[5]);
	Z77WRITE_D32( Z077_BASE, Z077_REG_MAC_ADDR1, dev->dev_addr[0] << 8 | dev->dev_addr[1] );
}

/*****************************************************************************/
/** Handle multicast and promiscuous mode set.
 *
 *  The set_multi entry point is called whenever the multicast address
 *  list or the network interface flags are updated.  This routine is
 *  responsible for configuring the hardware for proper multicast,
 *  promiscuous mode, and all-multi behavior.
 *
 * \param dev		\IN net_device struct for this NIC
 */
static void z77_set_rx_mode(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&np->lock, flags);

	Z77DBG( ETHT_MESSAGE_LVL1, MEN_Z77_DRV_NAME "z77_set_rx_mode: dev %s flags:0x%x\n",
			dev->name, dev->flags);
	
	/* check changed flags separately. Otherwise _PRO is set every time _IAM changes and vice versa. */
	if ( dev->flags & IFF_PROMISC) {
		if ( !(Z77READ_D32(Z077_BASE, Z077_REG_MODER) & OETH_MODER_PRO)) {
			/* promisc was cleared, set it */
			Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME "dev %s: set IFF_PROMISC\n", dev->name);
			Z077_SET_MODE_FLAG(OETH_MODER_PRO);
		}
	} else {
		if ( (Z77READ_D32(Z077_BASE, Z077_REG_MODER) & OETH_MODER_PRO)) {
			/* promisc was set, clear it */
			Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME " dev %s clear IFF_PROMISC\n", dev->name);
			Z077_CLR_MODE_FLAG(OETH_MODER_PRO);
		}
	}

	if ( dev->flags & IFF_MULTICAST) {
		if ( !(Z77READ_D32(Z077_BASE, Z077_REG_MODER) & OETH_MODER_IAM)) {
			/* mcast hash usage was cleared, set it */
			Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME " dev %s: set IFF_MULTICAST\n", dev->name);
			Z077_SET_MODE_FLAG(OETH_MODER_IAM);
		}
		/* update the HASH0/1 bits in any case, could also be a removed mcast MAC */
		z77_hash_table_setup( dev );
	} else {
		if ( (Z77READ_D32(Z077_BASE, Z077_REG_MODER) & OETH_MODER_IAM)) {
			/* mcast hash usage was cleared, set it and add passed MAC to mc_list */
			Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME " dev %s clear IFF_MULTICAST\n", dev->name);
			Z077_CLR_MODE_FLAG(OETH_MODER_IAM);
		}
	}

	spin_unlock_irqrestore (&np->lock, flags);
	return;
}

/******************************************************************************
 ** calculate multicast hash bin value
 *
 * \return	hash bit position (0-63) on success or -1 on Error
 */
static int ether_gen_crc(struct net_device *dev, u8 *data)
{
	u32 crc 	= MCAST_HASH_CRC_SEED;
	int length 	= MAC_ADDR_LEN;
	int hashbin = 0;
	u8 curr_oct = 0;
	u8 *p 		= data;
	int bit		= 0;
	struct z77_private *np = netdev_priv(dev);

	if (data == NULL)
		return -1;

	while ( --length >= 0 ) {
		curr_oct = *data++;
		for (bit = 0; bit < 8; bit++ ) {
			if ( ((crc & 0x80000000) ? 1 : 0) != (curr_oct & 0x1) ) {
				crc = (crc << 1) ^ MCAST_HASH_POLYNOM;
			} else {
				crc <<=1;
			}
			crc = crc & 0xffffffff;
			curr_oct >>=1;
		}
	}

	/* return bin position */
	hashbin = (int)((crc >> MCAST_MULT_SHFT) & MCAST_HASH_MASK);

	return(hashbin);
}  /* ether_gen_crc */

/******************************************************************************
 ** setup hash table and bits in HASH0/1 when a multicast MAC is set up
 *  or removed
 *
 * \return	0 on success or -EINVAL
 */
static void z77_hash_table_setup(struct net_device *dev)
{
	int i = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	struct netdev_hw_addr *ha = NULL;
#else
	struct dev_mc_list *ptr=NULL;
#endif

	u8 *p=NULL;
	struct z77_private *np = netdev_priv(dev);
	u32 hash0 = 0;
	u32 hash1 = 0;
	u32 bin_pos = 0;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,35)
	Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME " z77_hash_table_setup: mc_count = %d\n",
			netdev_mc_count(dev));
	if ((dev->flags & IFF_MULTICAST) && netdev_mc_count(dev)) {
		if (netdev_mc_count(dev) <= MAX_MCAST_LST) {
			netdev_for_each_mc_addr(ha, dev) {
				/* check if its a valid MC addr: bit1 in mac[0] set ? */
				if ((ha->addr[0] & 0x1) == 0x0)
					continue;
				memcpy(np->mcast_lst[i], ha->addr, MAC_ADDR_LEN);
				p = (u8*)(np->mcast_lst[i]);

				/* collect every hash bit, OR it together and update HASH0 and HASH1 registers */
				bin_pos = ether_gen_crc(dev, p );

				if (bin_pos > 31)
					hash1 |=( 1<<(bin_pos-32));
				else
					hash0 |=( 1<<bin_pos);
				i++;
			}
		}
	}
#else
	Z77DBG( ETHT_MESSAGE_LVL2, MEN_Z77_DRV_NAME " z77_hash_table_setup: mc_count = %d\n",
			dev->mc_count);
	if ((dev->flags & IFF_MULTICAST) && dev->mc_count) {
		if (dev->mc_count <= MAX_MCAST_LST) {
			for ( ptr = dev->mc_list; ptr ; ptr = ptr->next ) {
				/* check if its a valid MC addr: bit1 in mac[0] set ? */
				if (!(*ptr->dmi_addr & 1))
					continue;

				memcpy(np->mcast_lst[i], ptr->dmi_addr, MAC_ADDR_LEN);
				p = (u8*)(np->mcast_lst[i]);

				/* collect every hash bit, OR it together and update HASH0 and HASH1 registers */
				bin_pos = ether_gen_crc(dev, p );

				if (bin_pos > 31)
					hash1 |=( 1<<(bin_pos-32));
				else
					hash0 |=( 1<<bin_pos);
				i++;
			}
		}
	}
#endif

	Z77DBG( ETHT_MESSAGE_LVL3, MEN_Z77_DRV_NAME " z77_hash_table_setup: HASH0=0x%08x HASH1=0x%08x\n",
			hash0, hash1);
	Z77WRITE_D32( Z077_BASE, Z077_REG_HASH_ADDR0, hash0 );
	Z77WRITE_D32( Z077_BASE, Z077_REG_HASH_ADDR1, hash1 );
	return;
}

/* new API: net_device_ops moved out of struct net_device in own ops struct */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
/******************************************************************************
 ** set new MAC address: unused here
 *
 * \return	0 always
 */
static int z77_set_mac_address(struct net_device *dev, void *p)
{
	struct z77_private *np = netdev_priv(dev);
	struct sockaddr *addr = p;
	unsigned long flags;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, addr->sa_data, dev->addr_len);

	/* shut-down */
	z77_reset( dev );

	/* store MAC Address to in MAC_ADDR0, MAC_ADDR1 */
	spin_lock_irqsave(&np->lock, flags);

	z77_store_mac( dev );

	/* force link-up */
	np->prev_linkstate = 0;

	spin_unlock_irqrestore (&np->lock, flags);
	return 0;
}

/******************************************************************************
 ** set new MTU for the interface
 *
 * \return	0 on success or -EINVAL
 */
static int z77_change_mtu(struct net_device *netdev, int new_mtu)
{
	if (new_mtu < ETH_ZLEN || new_mtu > ETH_DATA_LEN)
		return -EINVAL;
	netdev->mtu = new_mtu;
	return 0;
}

static const struct net_device_ops z77_netdev_ops = {
	.ndo_open				= z77_open,
	.ndo_stop				= z77_close,
	.ndo_start_xmit			= z77_send_packet,
	.ndo_do_ioctl			= z77_ioctl,
	.ndo_get_stats			= z77_get_stats,
	.ndo_tx_timeout			= z77_tx_timeout,
	.ndo_set_rx_mode 		= z77_set_rx_mode,
	.ndo_change_mtu			= z77_change_mtu,
	.ndo_validate_addr		= eth_validate_addr,
	.ndo_set_mac_address	= z77_set_mac_address,
#if defined(Z77_USE_VLAN_TAGGING)
# if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
	.ndo_vlan_rx_register	= z77_vlan_rx_register,
	.ndo_vlan_rx_kill_vid   = z77_vlan_rx_kill_vid,
# endif
	.ndo_vlan_rx_add_vid	= z77_vlan_rx_add_vid,
#endif

};
#endif

/******************************************************************************
 ** z77_sda_in - read SDA Pin on SMB Register
 *
 * \param dat	\IN general purpose data, the Z87 instances base address
 *
 * \return		pin state: 0 or 1
 */
static int z77_sda_in(void *dat)
{
	int pin=0;
	volatile unsigned int tmp = 0, i = 0;

	for (i=0; i < 2; i++) {
		tmp = Z77READ_D32( dat, Z077_REG_SMBCTRL);
	}

	pin = (tmp & SMB_REG_SDA) ? 1 : 0;
	return pin;
}

/******************************************************************************
 ** z77_sda_out - set SDA Pin on SMB Register
 *
 * \param dat	 \IN general purpose data, the Z87 instances base address
 * \param pinval \IN pin state to set, 0 or 1
 *
 * \return		 0 if no difference or 1 if pin state not equal to pinval
 */
static int z77_sda_out(void *dat, int pinval)
{
	volatile unsigned int tmp 	= 	Z77READ_D32( dat, Z077_REG_SMBCTRL);

	if (pinval)
		tmp |=SMB_REG_SDA;
	else
		tmp &=~SMB_REG_SDA;

	Z77WRITE_D32( dat, Z077_REG_SMBCTRL, tmp);
	return(0);
}

/******************************************************************************
 ** z77_scl_out - set SCL Pin on SMB Register
 *
 * \param dat	 \IN general purpose data, the Z87 instances base address
 * \param pinval \IN pin state, 0 or 1
 *
 * \return 0 if no difference or 1 if pin state not equal to pinval
 */
static int z77_scl_out(void *dat, int pinval)
{

	volatile unsigned int tmp = Z77READ_D32( dat, Z077_REG_SMBCTRL);
	if (pinval)
		tmp |=SMB_REG_SCL;
	else
		tmp &=~SMB_REG_SCL;
	Z77WRITE_D32( dat, Z077_REG_SMBCTRL, tmp );
	return(0);
}

/******************************************************************************
 ** z77_timerfunc - periodically check the link state (when started with AUTO)
 *
 * \param dat	\IN general purpose data, used as z77_private struct
 *
 * \return			-
 */
static void z77_timerfunc(unsigned long dat)
{
	u32 linkstate;
	struct net_device *dev = (struct net_device *)dat;
	struct z77_private *np = netdev_priv(dev);
	linkstate = mii_link_ok(&np->mii_if);
	/* printk( KERN_INFO " z77_timerfunc: linkstate = %d np->prev_linkstate = %d\n", linkstate, np->prev_linkstate);  */
	if ( np->prev_linkstate != linkstate ) {
		if ( linkstate == 1 ) { /* link came up: restart IP core */
			z77_reset( dev );
			Z077_SET_MODE_FLAG( OETH_MODER_RXEN | OETH_MODER_TXEN );
			np->nCurrTbd = 0;
			np->flags |= IFF_UP;
			printk( KERN_INFO MEN_Z77_DRV_NAME " (%s): link is up\n", dev->name);
		} else { /* link went down: close device */
			printk( KERN_INFO MEN_Z77_DRV_NAME " (%s): link is down\n", dev->name);
			np->flags &= ~IFF_UP;
		}
		np->prev_linkstate = linkstate;
	}

	/* restart timer */
	np->timer.expires = jiffies + CONFIG_HZ / 2;
	add_timer(&np->timer);
}

/******************************************************************************
 ** z77_regdump - Dump the contents of the 16Z077/087 Registers and BDs
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return			-
 */
static int z77_regdump( struct net_device *dev )
{
	u32 adr	= 0;
	u32 tmp = 0;
 	u16 dat = 0;

	unsigned long i = 0;
	struct z77_private *np = netdev_priv(dev);

	printk( KERN_INFO MEN_Z77_DRV_NAME " (netdevice '%s') base Addr: 0x%08lx\n", dev->name, dev->base_addr );
	printk( KERN_INFO "np->bdOff    0x%04x\n", np->bdOff);
	printk( KERN_INFO "np->tbdOff   0x%04x\n", np->tbdOff);
	printk( KERN_INFO "np->rbdOff   0x%04x\n", np->rbdOff);

	while (z77_reginfo[i].addr != 0xffff){
		tmp = Z77READ_D32( dev->base_addr, z77_reginfo[i].addr);
		printk( KERN_INFO "%s   0x%08x\n", z77_reginfo[i].name, tmp);
		i++;
	}

	printk( KERN_INFO "current TXBd 0x%02x\n", np->nCurrTbd);
	printk( KERN_INFO "----- MII Registers -----\n");
	printk( KERN_INFO "instance\t\t\t0x%02x\n", np->instance );
	printk( KERN_INFO "PHY ADR\t\t\t0x%02x\n", 	np->mii_if.phy_id );

	dat = z77_mdio_read(dev, np->mii_if.phy_id, MII_BMCR );
	printk( KERN_INFO "MII_BMCR\t\t\t0x%04x\n", dat );

	/* ts: initially the link status isnt up to date, read it twice.. */
	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_BMSR );
	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_BMSR );

	printk( KERN_INFO "MII_BMSR\t\t0x%04x\tLink: %s\n",	dat, (dat & BMSR_LSTATUS) ? "up" : "down" );

	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_PHYSID1 );
	printk( KERN_INFO "MII_PHYSID1\t\t0x%04x\n", dat );

	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_PHYSID2 );
	printk( KERN_INFO "MII_PHYSID2\t\t0x%04x\n", dat );

	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_ADVERTISE );
	printk( KERN_INFO "MII_ADVERTISE\t\t0x%04x\n", dat );

	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_LPA );
	printk( KERN_INFO "MII_LPA\t\t\t0x%04x\n", dat );

	dat = z77_mdio_read(dev, np->mii_if.phy_id , MII_100_BASE_TX_PHY);

	printk( KERN_INFO "MII_100_BASE_TX_PHY\t0x%04x (", dat );
	/* dump PHY Op Mode Indication Reg. Bits [4..2] */

	switch((dat & 0x1c) >> 2) {
	case 0x0:
		printk( KERN_INFO "in Autoneg)\n");
		break;
	case 0x1:
		printk( KERN_INFO "10MB HD)\n");
		break;
	case 0x2:
		printk( KERN_INFO "100MB HD)\n");
		break;
	case 0x3:
		printk( KERN_INFO "Default)\n");
		break;
	case 0x5:
		printk( KERN_INFO "10MB FD)\n");
		break;
	case 0x6:
		printk( KERN_INFO "100MB FD)\n");
		break;
	case 0x7:
		printk( KERN_INFO "PHY Isolated)\n");
		break;
	default:
		printk( KERN_INFO "unknown)\n");
		break;
	}

	if (np->msg_enable >= ETHT_MESSAGE_LVL2) {
		printk(KERN_INFO "------------------ TX BDs: -------------------\n");
		for (i = 0; i < Z077_TBD_NUM; i++ ) {
			adr = Z077_BD_OFFS + (i * Z077_BDSIZE);
			printk(KERN_INFO "%02x STAT: 0x%04x LEN: 0x%04x  ADR 0x%08x\n", i,
				   Z077_GET_TBD_FLAG(i, 0xffff), Z077_GET_TBD_LEN(i), Z077_GET_TBD_ADDR(i));
		}

		printk(KERN_INFO "------------------ RX BDs: -------------------\n");
		for (i = 0; i < Z077_RBD_NUM ; i++ ) {
			adr = Z077_BD_OFFS + (i + Z077_TBD_NUM ) * Z077_BDSIZE;
			printk(KERN_INFO "%02x STAT: 0x%04x LEN: 0x%04x  ADR 0x%08x\n", i,
				   Z077_GET_RBD_FLAG(i, 0xffff), Z077_GET_RBD_LEN(i), Z077_GET_RBD_ADDR(i) );
		}
	}
	return 0;
}

/******************************************************************************
 ** z77_mdio_read  Wrapper for MII read access
 *
 * \param dev		\IN net_device struct for this NIC
 * \param phy_id	\IN address of the PHY
 * \param location	\IN net_device struct for this NIC
 *
 * \return			Value read from PHY <phy_id> register <location>
 */
static int z77_mdio_read(struct net_device *dev, int phy_id, int location)
{
	int retVal 			= 0xffff;
	volatile u32 miival = 0;
	volatile u32 tout 	= MII_ACCESS_TIMEOUT;

	/* wait until a previous BUSY disappears */
	do {
		miival = Z77READ_D32(Z077_BASE, Z077_REG_MIISTATUS );
		tout--;
	} while( (miival & OETH_MIISTATUS_BUSY) && tout);

	if (!tout) {
		printk(KERN_ERR "*** MII Read timeout!\n");
		return -1;
	}

	/* set up combined PHY and Register within Phy, then kick off read cmd */
	Z77WRITE_D32( Z077_BASE, Z077_REG_MIIADR, (location & 0xff) << 8 | phy_id );
	Z77WRITE_D32( Z077_BASE, Z077_REG_MIICMD, OETH_MIICMD_RSTAT);

	/* wait until the PHY finished */
	do {
		miival = Z77READ_D32(Z077_BASE, Z077_REG_MIISTATUS );
		tout--;
	} while( (miival & OETH_MIISTATUS_BUSY) && tout);

	if (!tout) {
		printk(KERN_ERR "*** MII Read timeout!\n");
		return -1;
	}

    /* fetch read Value from MIIRX_DATA*/
    retVal = Z77READ_D32( Z077_BASE, Z077_REG_MIIRX_DATA );
	return retVal;
}

/******************************************************************************
 ** z77_mdio_write  	Wrapper for MII write access
 *
 * \param dev		\IN net_device struct for this NIC
 * \param phy_id	\IN address of the PHY (= 1 on EM01)
 * \param location	\IN net_device struct for this NIC
 * \param val 		\IN value to write to MII Register
 *
 * \return			-
 */
static void z77_mdio_write(struct net_device *dev, int phy_id,
						   int location, int val)
{
	volatile u32 miival = 0;
	volatile u32 tout 	= MII_ACCESS_TIMEOUT;

	do { 	/* wait until a previous BUSY disappears */
		miival = Z77READ_D32(Z077_BASE, Z077_REG_MIISTATUS );
		tout--;
	} while( (miival & OETH_MIISTATUS_BUSY) && tout);

	if (!tout) {
		printk(KERN_ERR "*** MII Write timeout!\n");
		return;
	}

	Z77WRITE_D32( Z077_BASE, Z077_REG_MIIADR, (location & 0xff) << 8 | phy_id );
	Z77WRITE_D32( Z077_BASE, Z077_REG_MIITX_DATA, val );
	Z77WRITE_D32( Z077_BASE, Z077_REG_MIICMD,  OETH_MIICMD_WCTRLDATA );

	/* wait until a previous BUSY disappears */
	do {
		miival = Z77READ_D32(Z077_BASE, Z077_REG_MIISTATUS );
		tout--;
	} while( (miival & OETH_MIISTATUS_BUSY) && tout);

	if (!tout) {
		printk(KERN_ERR "*** MII Write timeout!\n");
		return;
	}
}

/**
 * \defgroup _NETOPS_FUNC ethtool support functions
 */
/*@{*/
/******************************************************************************
 ** z77_ethtool_get_drvinfo retrieve drivers info with ethtool
 *
 * \param dev		\IN net_device struct for this NIC
 * \param info		\IN ethtool supporting info struct
 *
 * \return			-
 */
static void z77_ethtool_get_drvinfo(struct net_device *dev,
									struct ethtool_drvinfo *info)
{
	struct z77_private  *np = netdev_priv(dev);
	struct pci_dev		*pcd = np->pdev;
	unsigned long flags;

	spin_lock_irqsave(&np->lock, flags);

	strncpy(info->driver, cardname, sizeof(info->driver)-1);
	strncpy(info->version, version, sizeof(info->version)-1);

	if (pcd)
		strcpy(info->bus_info, pci_name(pcd));

	/* ts: added Register Dumps */
	if (np->msg_enable)
		z77_regdump(dev);

	spin_unlock_irq(&np->lock);
}

/******************************************************************************
 ** z77_dump_ecmd helper function to dump contents of ethtool command struct
 *
 * \param ecmd		\IN command type passed to ethtool, see linux/ethtool.h
 *
 * \return			0;
 */
static void z77_dump_ecmd(struct ethtool_cmd *ecmd)
{
	unsigned int i = 0;
	printk( KERN_INFO "main contents of ethtool_cmd struct:\n");
	printk( KERN_INFO "cmd          0x%08x\n", ecmd->cmd );

	printk( KERN_INFO "supported    0x%08x\n", ecmd->supported );
	for (i = 0; i < 15; i++)
		if ( (1<<i) & ecmd->supported)
			printk("    + %s\n", phycaps[i]);
	printk( KERN_INFO "advertising  0x%08x\n", ecmd->advertising );
	for (i = 0; i < 15; i++)
		if ( (1<<i) & ecmd->advertising)
			printk("    + %s\n", phycaps[i]);

	printk( KERN_INFO "speed        %d\n", 		ecmd->speed );
	printk( KERN_INFO "duplex       %d (%s)\n",	ecmd->duplex, ecmd->duplex ? "full" : "half" );
	printk( KERN_INFO "autoneg      %d (%s)\n",	ecmd->autoneg, ecmd->autoneg ? "on" : "off" );
}

/******************************************************************************
 ** z77_ethtool_get_settings retrieve NIC settings with ethtool
 *
 * \param dev		\IN net_device struct for this NIC
 * \param ecmd		\IN command type passed to ethtool, see linux/ethtool.h
 *
 * \return			0;
 */
static int z77_ethtool_get_settings(struct net_device *dev,	struct ethtool_cmd *ecmd)
{
	struct z77_private *np = netdev_priv(dev);
	unsigned long flags;

	if( !(np->flags & IFF_UP) ) {
		return -ENETDOWN;
	}

	spin_lock_irqsave(&np->lock, flags);
	mii_ethtool_gset(&np->mii_if, ecmd);

	if (np->msg_enable == ETHT_MESSAGE_LVL3)
		z77_dump_ecmd(ecmd);

	spin_unlock_irqrestore (&np->lock, flags);
	return 0;
}

/******************************************************************************
 ** z77_ethtool_set_settings set NIC settings with ethtool
 *
 * \param dev		\IN net_device struct for this NIC
 * \param ecmd		\IN command type passed to ethtool, see linux/ethtool.h
 *
 * \return			0 or negative error number;
 */
static int z77_ethtool_set_settings(struct net_device *dev, struct ethtool_cmd *ecmd)
{
	struct ethtool_cmd ncmd;
	struct z77_private *np = netdev_priv(dev);
	int res=0;
	unsigned long flags;

	if ( !(np->flags & IFF_UP) ) {
		return -ENETDOWN;
	}

	spin_lock_irqsave(&np->lock, flags);

	if (np->msg_enable == ETHT_MESSAGE_LVL3)
		z77_dump_ecmd(ecmd);

	if (ecmd->cmd != ETHTOOL_TEST) {
		res = mii_ethtool_sset(&np->mii_if, ecmd);
		/* wait to let settings take effect */
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
		/* set_current_state(TASK_INTERRUPTIBLE); */
		schedule_timeout_interruptible(CONFIG_HZ/4);
#else
		current->state = TASK_INTERRUPTIBLE;
		schedule_timeout(CONFIG_HZ/4);
#endif
		/* check PHY again, set MODER[10] to match duplexity setting in it */
		mii_ethtool_gset(&np->mii_if, &ncmd);

		/* hand over duplexity from phy */
		if (ncmd.duplex == DUPLEX_FULL) {
			Z077_SET_MODE_FLAG(OETH_MODER_FULLD);
		} else {
			Z077_CLR_MODE_FLAG(OETH_MODER_FULLD);
		}
	}

	spin_unlock_irqrestore (&np->lock, flags);
	/* force link-up */
	np->prev_linkstate = 0;
	return res;
}

/******************************************************************************
 ** z77_ethtool_nway_reset restart MII
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return			0 or error value;
 */
static int z77_ethtool_nway_reset(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	return mii_nway_restart(&np->mii_if);
}

/******************************************************************************
 ** z77_ethtool_get_link
 * check link status
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return			1 if link us up or 0
 */
static u32 z77_ethtool_get_link(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	return mii_link_ok(&np->mii_if);
}

/******************************************************************************
 ** z77_ethtool_get_msglevel - get message level
 *
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return			message verbosity level
 */
static u32 z77_ethtool_get_msglevel(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	/* printk("z77_ethtool_get_msglevel: v = %d\n", np->msg_enable ); */
	return np->msg_enable;
}

/******************************************************************************
 ** z77_ethtool_set_msglevel - Set message verbosity level
 *
 * \param dev	\IN net_device struct for this NIC
 * \param v		\IN message verbosity. 0 disable 1 normal 2 verbose 3 + IRQs
 * \brief		this is called when user calls ethtool -s eth0 msglvl <v>
 *
 * \return	-
 */
static void z77_ethtool_set_msglevel(struct net_device *dev, u32 v)
{
	struct z77_private *np = netdev_priv(dev);
	if (v > 3)
		v = 3;
	np->msg_enable = v;
}

/******************************************************************************
 ** z77_ethtool_testmode - Set PHY test mode
 *
 * \param dev	\IN net_device struct for this NIC
 * \param etest	\IN test function(s), unused
 * \param data	\IN result data
 * \brief		this is called when user calls ethtool -t eth0 msglvl
 *
 * \return	-
 */
static void z77_ethtool_testmode(struct net_device *dev, struct ethtool_test *etest, u64 *data)
{
	printk("TODO: setting %s into test pattern mode\n", dev->name);
	/* TODO: yet to come */
	return;
}
/*@}*/   /* end defgroup ethtool funcs */

/**
 * This structure provides the interface functions to the standard ethtool
 */
static struct ethtool_ops z77_ethtool_ops = {
	.get_drvinfo 	= z77_ethtool_get_drvinfo,
	.get_settings 	= z77_ethtool_get_settings,
	.set_settings 	= z77_ethtool_set_settings,
	.nway_reset 	= z77_ethtool_nway_reset,
	.get_link 		= z77_ethtool_get_link,
	.get_msglevel 	= z77_ethtool_get_msglevel,
	.set_msglevel 	= z77_ethtool_set_msglevel,
	.self_test		= z77_ethtool_testmode,
};

/* return non zero if the Tx BD is full already, a stall condition occured */
u32 tx_full(struct net_device *dev)
{
	int txbEmpty;
	struct z77_private *np = netdev_priv(dev);

	/* Z87 Core with extra TXBd empty Flags */
	if ( np->nCurrTbd < 32 )
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY0) & (1 << np->nCurrTbd);
	else
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY1) & (1 << (np->nCurrTbd-32));

	return !txbEmpty;
}

/* ts@men: we need the true linkstate for F218R01-01 */
static ssize_t z77_show_linkstate(struct device *dev, struct device_attribute *attr, char *buf)
{
	unsigned int bsmr = 0;
	struct z77_private *np  = netdev_priv(to_net_dev(dev));
#define  MII_BMSR_LINK_VALID	0x0004
	/* ts: initially the link status isnt up to date, read it twice. This is often
	   a latched Register inside PHYs */
	bsmr = z77_mdio_read(np->dev, np->mii_if.phy_id , MII_BMSR );
	bsmr = z77_mdio_read(np->dev, np->mii_if.phy_id , MII_BMSR );
	return sprintf(buf, "%c\n", bsmr & MII_BMSR_LINK_VALID ? '1' : '0' );
}

static ssize_t z77_set_linkstate(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	return 0;	/* no op */
}

static DEVICE_ATTR(linkstate, 0644, z77_show_linkstate, z77_set_linkstate );
/****************************************************************************/
/** z77_bd_setup - perform initialization of buffer descriptors
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \brief this initializes the buffer descriptors such that PCI DMA - ready
 *        memory chunks are allocated
 *
 */
static int z77_bd_setup(struct net_device *dev)
{
	u32 i=0;
	struct z77_private *np = netdev_priv(dev);
	dma_addr_t memPhysDma;
	struct pci_dev *pcd = np->pdev;
	dma_addr_t dma_handle = 0;
	void *     memVirtDma = NULL;

	memset((char*)(Z077_BDBASE + Z077_BD_OFFS), 0x00, 0x400);

	/* Setup Tx BDs */
	for ( i = 0; i < Z077_TBD_NUM; i++ ) {
		memVirtDma = dma_alloc_coherent(&pcd->dev, Z77_ETHBUF_SIZE, &memPhysDma, GFP_KERNEL );
		np->txBd[i].BdAddr = memVirtDma;
		memset((char*)(memVirtDma), 0, Z77_ETHBUF_SIZE);
		Z077_SET_TBD_FLAG( i, Z077_TBD_IRQ );
		smp_wmb();
	}

	/* Setup Receive BDs */
	for (i = 0; i < Z077_RBD_NUM; i++ ) {
		memVirtDma = dma_alloc_coherent( &pcd->dev, Z77_ETHBUF_SIZE, &memPhysDma, GFP_KERNEL );
		dma_handle = dma_map_single( &pcd->dev, memVirtDma, (size_t)Z77_ETHBUF_SIZE, DMA_FROM_DEVICE);
		np->rxBd[i].BdAddr = memVirtDma;
		np->rxBd[i].hdlDma = dma_handle;
		memset((char*)(memVirtDma), 0, Z77_ETHBUF_SIZE);
		Z077_SET_RBD_ADDR( i, dma_handle );
		smp_wmb();
		Z077_SET_RBD_FLAG( i, Z077_RBD_IRQ | Z077_RBD_EMP );
		smp_wmb();
	}

	/* close the Rx/Tx Rings with Wrap bit in each last BD */
	Z077_SET_TBD_FLAG( Z077_TBD_NUM - 1 , Z077_TBD_WRAP );
	Z077_SET_RBD_FLAG( Z077_RBD_NUM - 1 , Z077_RBD_WRAP );
	smp_wmb();
	return(0);
}

#if defined(Z77_USE_VLAN_TAGGING)

/****************************************************************************/
/** z77_vlan_rx_add_vid - add a VLAN group ID to this net device
 *
 * \param dev		\IN net_device struct for this NIC
 * \param vid		\IN VLAN group ID
 *
 */
static int z77_vlan_rx_add_vid(struct net_device *dev, unsigned short proto, unsigned short vid)
{
	struct z77_private *np = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&np->lock, flags);

	Z77DBG(ETHT_MESSAGE_LVL1, "--> %s\n", __FUNCTION__);

	if (!np->vlgrp) {
		Z77DBG(ETHT_MESSAGE_LVL1, "%s: vlgrp = NULL!\n", __FUNCTION__);
	} else {
		Z77DBG(ETHT_MESSAGE_LVL1, "%s: adding VLAN:%d\n", __FUNCTION__, vid );
	}
	spin_unlock_irqrestore (&np->lock, flags);
	return 0;
}

# if LINUX_VERSION_CODE < KERNEL_VERSION(3,0,0)
/****************************************************************************/
/** z77_vlan_rx_kill_vid - delete a VLAN group ID
 *
 * \param dev		\IN net_device struct for this NIC
 * \param vid		\IN VLAN group ID
 *
 */
static void z77_vlan_rx_kill_vid(struct net_device *dev, unsigned short vid)
{
	struct z77_private *np = netdev_priv(dev);
	unsigned long flags;

	spin_lock_irqsave(&np->lock, flags);

	if (!np->vlgrp) {
		Z77DBG(ETHT_MESSAGE_LVL1, "%s: vlgrp = NULL!\n", __FUNCTION__);
	} else {
		Z77DBG(ETHT_MESSAGE_LVL1, "%s: killing VLAN:%d (vlgrp entry:%p)\n",
			   __FUNCTION__, vid, vlan_group_get_device(np->vlgrp, vid));

		vlan_group_set_device(np->vlgrp, vid, NULL);

	}
	spin_unlock_irqrestore (&np->lock, flags);
	return;
}
# endif
#endif

/****************************************************************************/
/** net_rx_err - irq context handler to report Errors
 *
 * \param dev		\IN net_device struct for this NIC
 *
 */
static void z77_rx_err( struct net_device *dev )
{
	int i;
	struct z77_private *np = netdev_priv(dev);

	/* just skip Rx BD Ring backwards so we miss none */
	for (i = Z077_RBD_NUM - 1; i >= 0; i--) {
		if (Z077_GET_RBD_FLAG( i , 0x1ff)) {
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_OVERRUN)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: overrun[%d]\n", i);
				np->stats.rx_over_errors++;
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_INVSYMB)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: inv symbol[%d]\n", i);
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_DRIBBLE)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: dribble[%d]\n", i);
				np->stats.rx_frame_errors++;
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_TOOLONG)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: too long[%d]\n", i);
				np->stats.rx_length_errors++;
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_SHORT)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: too short[%d]\n", i);
				np->stats.rx_length_errors++;
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_CRCERR)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: CRC err[%d]\n", i);
				np->stats.rx_crc_errors++;
			}
			if (Z077_GET_RBD_FLAG( i ,OETH_RX_BD_LATECOL)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** RX: late coll[%d]\n", i);
			}
			np->stats.rx_errors++;
		    /* Flags are reported, clear them */
			Z077_CLR_RBD_FLAG( i , OETH_RX_BD_OVERRUN | OETH_RX_BD_INVSYMB | \
							   OETH_RX_BD_DRIBBLE | OETH_RX_BD_TOOLONG | \
							   OETH_RX_BD_SHORT | OETH_RX_BD_CRCERR | \
							   OETH_RX_BD_LATECOL );
		}
	}
}

/****************************************************************************/
/** net_tx_err - irq context handler to report Errors
 *
 * \param dev		\IN net_device struct for this NIC
 *
 */
static void z77_tx_err( struct net_device *dev)
{
	int i;
	struct z77_private *np = netdev_priv(dev);

	/* simply skip Tx BD Ring backwards */
	for (i = Z077_TBD_NUM-1; i >= 0; i--) {
		if (Z077_GET_TBD_FLAG( i , OETH_TX_BD_DEFER | OETH_TX_BD_CARRIER
							   | OETH_TX_BD_UNDERRUN | OETH_TX_BD_RETLIM
							   | OETH_TX_BD_LATECOL)) {
			if (Z077_GET_TBD_FLAG( i, OETH_TX_BD_DEFER)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** TX: defered frame[%d]\n", i);
			}

			if (Z077_GET_TBD_FLAG( i, OETH_TX_BD_CARRIER)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** TX: Carrier lost[%d]\n", i);
				np->stats.tx_carrier_errors++;
			}

			if (Z077_GET_TBD_FLAG( i, OETH_TX_BD_UNDERRUN)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** TX: underrun[%d]\n", i);
				np->stats.tx_fifo_errors++;
			}

			if (Z077_GET_TBD_FLAG( i, OETH_TX_BD_RETLIM)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** TX: retrans limit[%d]\n", i);
				np->stats.tx_aborted_errors++;
			}

			if (Z077_GET_TBD_FLAG( i, OETH_TX_BD_LATECOL)) {
				Z77DBG(ETHT_MESSAGE_LVL1, "*** TX: late coll[%d]\n", i);
				np->stats.tx_window_errors++;
			}

			np->stats.tx_errors++;
			np->stats.collisions += Z077_GET_TBD_FLAG(i, OETH_TX_BD_RETRY) >> 4;

			Z077_CLR_TBD_FLAG( i, OETH_TX_BD_DEFER | OETH_TX_BD_CARRIER | \
							   OETH_TX_BD_UNDERRUN | OETH_TX_BD_RETLIM | \
							   OETH_TX_BD_LATECOL );
		}
	}
}

/****************************************************************************/
/** z77_reset - reset device with asynchronous global Reset Register 0x54
 *
 * \param dev		\IN net_device struct for this NIC
 *
 */
static void z77_reset( struct net_device *dev )
{
    /* stop receiving/transmitting */
    Z077_CLR_MODE_FLAG(OETH_MODER_RXEN | OETH_MODER_TXEN );
}

/****************************************************************************/
/** z77_phy_init - initialize and configure the PHY devices
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \brief
 * This routine scans, initializes and configures PHY devices.
 */
static int z77_phy_init(struct net_device *dev)
{
    u16	dat = 0;
	u8	phyAddr;		/* address of a PHY */
    u32	found = FALSE;	/* no PHY has been found */

	struct z77_private *np = netdev_priv(dev);
	phyAddr = np->mii_if.phy_id;

	/* reset the PHY */
	z77_phy_reset( dev, phyAddr	);

	/* check which PHY is there */
	if (z77_phy_identify(dev, phyAddr)) {
		return (-ENODEV);
	}

	/* on F218 revert the Link and activity LED */
	dat = z77_mdio_read(dev, phyAddr, MII_PHYSID2 );
	if ((dat == PHY_ID2_KSZ8041_1) || (dat == PHY_ID2_KSZ8041_2) ) {
		printk( KERN_INFO "found PHY KSZ8041. reverting Link/Act LED" );
		dat = z77_mdio_read(dev, np->mii_if.phy_id , 0x1e );
		dat |= 1 << 14; /* just set bit14, bit 15 is reserved (datasheet p.29) */
		z77_mdio_write( dev, phyAddr, 0x1e, dat );
	}

	found = TRUE;

	/* disable powerdown mode */
	dat = z77_mdio_read(dev, phyAddr, MII_100_BASE_TX_PHY );
	dat &=~(MII_100BASE_PWR);
	z77_mdio_write(dev, phyAddr, MII_100_BASE_TX_PHY, dat);

	/* set desired Phy mode: auto-negotiation or fixed. */
	if (z77_init_phymode (dev, phyAddr) == 0)
		return (0);

    if (!found)
		return (-ENODEV);

    /* if we're here, none of the PHYs could be initialized */
	printk( KERN_ERR "*** z77_phy_init: no link found, check cable connection \n");
    return (-ENODEV);
}

/****************************************************************************/
/** z77_phy_identify - probe the PHY device
 *
 * \param dev		\IN net_device struct for this NIC
 * \param phyAddr	\IN Address of used PHY, currently 1
 *
 * \return			zero if Phy found, otherwise nonzero
 * \brief
 * This routine probes the PHY device by reading its PHY Identifier Register
 */
static int z77_phy_identify(struct net_device *dev,u8 phyAddr )
{
	u32 i=0;
    u16	data;		/* data to be written to the control reg */
	u16	id2;
    data = z77_mdio_read(dev, phyAddr, MII_PHYSID1 );

	while (z077PhyAttachTbl[i].ident !=0xffff) {
		if (data == z077PhyAttachTbl[i].ident ) {
			id2 = z77_mdio_read(dev, phyAddr, MII_PHYSID2 );
			printk( KERN_INFO "PHY %s found. (MII_PHYSID1: 0x%04x MII_PHYSID2: 0x%04x)\n",
					z077PhyAttachTbl[i].name, data, id2 );
			return (0);
		}
		i++;
	}
	/* found no known PHY in table, error */
	printk( KERN_ERR "*** z77_phy_identify: unknown Phy ID 0x%04x!\n", data );
	return (-ENODEV);
}

/****************************************************************************/
/** z77_phy_reset
*
* \param dev		\IN net_device struct for this NIC
* \param phyAddr	\IN Address of used PHY, currently 1
*
* \brief
* This routine send a Reset command to the PHY specified in
* the parameter phyaddr.
*
* \return 0 always
*/
static int z77_phy_reset( struct net_device *dev, u8 phyAddr )
{
	u16 dat = 0;
	u32 tmp = 0;

	dat = z77_mdio_read(dev, phyAddr, MII_BMCR );
	tmp = Z77READ_D32( Z077_BASE , Z077_REG_MIISTATUS );
	z77_mdio_write( dev, phyAddr, MII_BMCR, BMCR_RESET );
	dat = z77_mdio_read( dev, phyAddr, MII_BMCR );

	return(0);
}

/****************************************************************************/
/** z77_init_phymode - Set Phy Mode and Flags according to given mode
 *
 * \param dev		\IN net_device struct for this NIC
 * \param phyAddr	\IN PHY Chip address, one of 0 to 31. Is 1 at EM01
 *
 * \return 0 or error code
 */
static int z77_init_phymode (struct net_device *dev, u8 phyAddr)
{
	unsigned bDoAutoneg = 0;
	unsigned int dat=0;
	struct ethtool_cmd cmd;
	int res = 0;
	struct z77_private *np = netdev_priv(dev);
	Z77DBG(ETHT_MESSAGE_LVL1, "--> %s(phyAddr=%d)\n", __FUNCTION__, phyAddr);

	/* some default settings */
	cmd.port 		 	= PORT_MII;
	cmd.transceiver  	= XCVR_INTERNAL;
	cmd.phy_address  	= phyAddr;
	cmd.autoneg      	= AUTONEG_DISABLE;

	switch ( mode[np->instCount] ) {
	case phymode_10hd:
		np->mii_if.full_duplex	=	0;
		np->mii_if.force_media	=	1;
		cmd.speed 				= 	SPEED_10;
		cmd.duplex 				=	DUPLEX_HALF;
		break;
	case phymode_10fd:
		np->mii_if.full_duplex	=	1;
		np->mii_if.force_media	=	1;
		cmd.speed 				= 	SPEED_10;
		cmd.duplex 				= 	DUPLEX_FULL;
		break;
	case phymode_100hd:
		np->mii_if.full_duplex	=	0;
		np->mii_if.force_media	=	1;
		cmd.speed 				= 	SPEED_100;
		cmd.duplex 				= 	DUPLEX_HALF;
		break;
	case phymode_100fd:
		np->mii_if.full_duplex	=	1;
		np->mii_if.force_media	=	1;
		cmd.speed 				= 	SPEED_100;
		cmd.duplex 				= 	DUPLEX_FULL;
		break;
	case phymode_auto:		
		np->mii_if.full_duplex	=	1;
		np->mii_if.force_media	=	0;
		cmd.speed 				= 	SPEED_100;
		cmd.duplex 				= 	DUPLEX_FULL;
		cmd.autoneg 			= 	AUTONEG_ENABLE;
		bDoAutoneg 				= 	1;
		break;
	default:
		printk(KERN_ERR "*** invalid mode parameter '%d'\n", mode[np->instCount] );
		return -EINVAL;
	}

	/* apply desired mode or autonegotiate */
	if ( bDoAutoneg ) {
		/* set NWAYEN bit 0.12 accordingly */
		dat = z77_mdio_read(dev, np->mii_if.phy_id, MII_BMCR );
		dat |= ( 1<<12 ); /* bit 0.12 = autonegotiation enable */
		z77_mdio_write( dev, np->mii_if.phy_id, MII_BMCR, dat );
		if ( (res = mii_nway_restart(&np->mii_if )) ) {
			printk(KERN_ERR "*** setting autoneg. PHY mode failed\n");
			return -EINVAL;
		}
	} else {
		/* set MODER[10] (HD/FD) according to passed duplex setting */
		if (np->mii_if.full_duplex) {
			Z077_SET_MODE_FLAG(OETH_MODER_FULLD);
		}
		else {
			Z077_CLR_MODE_FLAG(OETH_MODER_FULLD);
		}

		if ((res = mii_ethtool_sset(&np->mii_if, &cmd))) {
			printk(KERN_INFO "PHY setting fixed mode failed - fixed MEN Phy\n" );
		}
	}
	Z77DBG(ETHT_MESSAGE_LVL1, "<-- %s()\n", __FUNCTION__);
	return 0;
}

/****************************************************************************/
/** Do the complete Autonegotiation for the used Ethernet PHY
 *
 * \param dev			\IN net_device struct for this NIC
 *
 * \return 0 or error code
 */
static int z77_do_autonegotiation(struct net_device *dev)
{
	int rv = 0;

	if (z77_phy_init(dev)) {
		printk("*** PHY Initialization failed!\n");
		rv = -ENODEV;
	}

	return rv;
}

/****************************************************************************/
/** remove the NIC from Kernel
 *
 * \param dev			\IN net_device struct for this NIC
 *
 * \return 0 or error code
 */
static void cleanup_card(struct net_device *dev)
{
	free_irq(dev->irq, dev);
	release_region(dev->base_addr, Z77_CFGREG_SIZE);
}

/****************************************************************************/
/** Timeout handler when no scheduled ETH irq arrived
 *
 * \param work		\IN handle of work_struct
 *
 * \return -
 *
 */
static void z77_reset_task(struct work_struct *work)
{
	struct z77_private *np = container_of(work, struct z77_private, reset_task);
    struct net_device *dev = np->dev;
	struct ethtool_cmd ecmd = {0};
	int settings_saved=0;

	Z077_DISABLE_IRQ( Z077_IRQ_ALL );

	netif_tx_disable(dev);

	printk(KERN_WARNING "%s: NETDEV WATCHDOG timeout! (%s)\n", dev->name, __FUNCTION__ );

	if ( np->msg_enable ) {
		printk(KERN_WARNING "Current register settings before restart:\n");
		z77_regdump(dev);
	}

	settings_saved = !z77_ethtool_get_settings(dev, &ecmd);
	z77_close(dev);
	z77_open(dev);

	/* restore settings */
	if (settings_saved) {
		z77_ethtool_set_settings(dev, &ecmd);
	}

	np->stats.tx_errors++;

	/* If we have space available to accept new transmits, do so */
	if (!tx_full(dev))
		netif_wake_queue(dev);
}

/****************************************************************************/
/** Timeout handler when no scheduled ETH irq arrived
 *
 * \param dev			\IN net_device struct for this NIC
 *
 * \return -
 */
static void z77_tx_timeout(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	Z77DBG( ETHT_MESSAGE_LVL1, "z77_tx_timeout called!\n");
	/* place reset outside of interrupt context, timer calls are soft IRQs */
	schedule_work(&np->reset_task);
}

/****************************************************************************/
/** When RX0 or RX1 nonempty, return oldest entry. This is tailored for the
 *  2 32bit Registers RX0_EMPTY, RX1_EMPTY.
 *
 * \return oldest (rightmost) bit in Rx0/1_EMPTY
 */
static u32 z77_get_oldest_frame(u32 rx0, u32 rx1, u32 *nrframes)
{
	volatile u32 frameNum=0, emp_n=0, emp_n1=0;
	int i;
	u32 cnt=0;

	for ( i = 63; i >=0; i-- ) 
	{	/* the 64 Rx BDs are split in 2 x 32bit registers, check boundaries */
		if (i > 32) { /* 63..33  RX1 only */
			emp_n  = ( rx1 & ( 1 << (i-32)   ));
			emp_n1 = ( rx1 & ( 1 << (i-32-1) ));
		}
		if (i == 32) { /* 32:  check border RX1/RX0 */
			emp_n  = ( rx1 & 0x00000001 );
			emp_n1 = ( rx0 & 0x80000000 );
		}
		if ( (i < 32) && ( i > 0) ) { /* 31..1 : RX0 only */
			emp_n  = ( rx0 & ( 1 <<    i ));
			emp_n1 = ( rx0 & ( 1 << (i-1)));
		}
		if ( i == 0) { /* 0->63 */
			emp_n  = ( rx0 & 0x00000001 );
			emp_n1 = ( rx1 & 0x80000000 );
		}

		/* if at this position an full-to-empty occurs it is our startposition to pass packets upwards from. 
		   Otherwise go on and count # of frames. If another frame arrives right at this moment it will be handled
		   after next IRQ enable and be the new start position.
		*/
		if ( (emp_n != 0) && (emp_n1 == 0) ) {			
			frameNum = i;
			break;
		}
	}

	/* count all 1-bits to check how many packets are there to process in this NAPI poll */
	for ( i = 31; i >=0; i-- ) {
		if ( rx1 & (1 << i ))
			cnt++;
		if ( rx0 & (1 << i ))
			cnt++;
	}

	*nrframes = cnt;
	return frameNum;
}

/****************************************************************************/
/** z77_poll - Rx poll function to support the NAPI functionality
 *
 * \param napi			\IN NAPI struct for this NIC
 * \param budget		\IN allowed # of packets to process in a call
 *  
 *  this is a softirq so dont use potentially sleeping sys calls!
 * 
 * \return 0 if all packets were processed or 1 of not all was processed
 *
 */
static int z77_poll(struct napi_struct *napi, int budget)
{
	int	i=0;
	u32 start_pos=0, rx0=0, rx1=0, nrframes=0;
	struct z77_private *np = container_of(napi, struct z77_private, napi);
    struct net_device *dev = np->dev;		

	/* bits in register are 1 for empty, 0 for full. we invert the logic */
	rx0 = ~Z77READ_D32( Z077_BASE, Z077_REG_RXEMPTY0 );
	rx1 = ~Z77READ_D32( Z077_BASE, Z077_REG_RXEMPTY1 );

	if( rx0 || rx1 ) {
		start_pos=z77_get_oldest_frame(rx0, rx1, &nrframes);
		Z77DBG( ETHT_MESSAGE_LVL3, "z77_poll: %08x%08x sp %d #fr %d\n", rx1, rx0, start_pos, nrframes );
		for (i=0; i < nrframes; i++)	
		{	/* pass new arrived packets up the stack, from start_pos (oldest) nonempty packet to recent one */
			z77_pass_packet( dev, start_pos );
			start_pos++;
			start_pos %= Z077_RBD_NUM;
		}
	} 
		
	if ( nrframes < budget ) { 
		/* we are done, for now */
		napi_complete(napi);
		Z077_ENABLE_IRQ( OETH_INT_RXF );
	}
	return nrframes;
}

/****************************************************************************/
/** z77_open - open the ethernet device for first usage
 *
 * \param dev			\IN net_device struct for this NIC
 *
 * \return 0 or error code
 *
 * \brief
 * Open/initialize the Ethernet device.
 * This routine should set everything up anew at each open, even
 * registers that should only need to be set once at boot, so that
 * there is non-reboot way to recover if something goes wrong.
 */
static int z77_open(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	Z77DBG( ETHT_MESSAGE_LVL1, "-> %s %s\n", __FUNCTION__, dev->name );

	/* do PHY/MAC initialization with forced mode or autonegotiation */
	if (chipset_init(dev, 1)) {
		printk(KERN_ERR "*** z77_open: initializing Ethernet core failed!\n"); ;
		return(-ENODEV);
	}
	/* setup the Tx/Rx buffer descriptors */
	z77_bd_setup(dev);

	/* clear any pending spurious IRQs */
	Z77WRITE_D32( Z077_BASE, Z077_REG_INT_SRC, 0x7f );

	/* hook in the Interrupt handler */
	Z77DBG( ETHT_MESSAGE_LVL1, "%s: request IRQ %d\n", dev->name, dev->irq );
	if (request_irq( dev->irq, z77_irq, IRQF_SHARED, cardname, dev)) {
		printk(KERN_ERR "*** %s: unable to get IRQ %d.\n", dev->name, dev->irq);
		return -ENOMEM;
	}

	np->open_time = jiffies;
	/* (re-)kick off link change detection */
	np->timer.expires = jiffies + CONFIG_HZ / 2;
	add_timer(&np->timer);

	napi_enable(&np->napi);

	netif_start_queue(dev);

	/* and let the games begin... */
	Z077_ENABLE_IRQ( Z077_IRQ_ALL );
	Z077_SET_MODE_FLAG(OETH_MODER_RXEN | OETH_MODER_TXEN );
	np->flags |= IFF_UP;


	Z77DBG( ETHT_MESSAGE_LVL1, "<-- %s()\n", __FUNCTION__ );
	return 0;
}

/*******************************************************************/
/** send a ready made ethernet frame
 *
 * \param dev			\IN net_device struct for this NIC
 * \param skb			\IN struct skbuf with data to transmit
 *
 * \return 				0 or error code
 */
static int z77_send_packet(struct sk_buff *skb, struct net_device *dev)
{

	struct z77_private *np 	= 	netdev_priv(dev);
	struct pci_dev *pcd 	=   np->pdev;
	unsigned char 	*buf 	= 	skb->data;
	u32 txbEmpty 			= 	0;
#if defined(Z77_USE_VLAN_TAGGING)
	unsigned int vlan_id	= 	0;
	unsigned int vlan_tag	=   0;
#endif
	unsigned int frm_len	=   0;
	int i 					= 	0;
	unsigned char 	idxTx 	=   0;
	dma_addr_t dma_handle 	= 	0;
	u8* dst = NULL;
	u8* src = NULL;

	/* place Tx request in the recent Tx BD */
	idxTx 	= np->nCurrTbd;

	np->stats.collisions += Z077_GET_TBD_FLAG( idxTx, OETH_TX_BD_RETRY) >> 4;

	/* Check if this Tx BD we use now is empty. If not -> drop . */
	if ( idxTx < 32 )
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY0) & (1<<idxTx);
	else
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY1) & (1 << (idxTx-32));

	if (!txbEmpty) { /* congestion? */
		netif_stop_queue(dev);
		np->stats.tx_dropped++;

		/* free this skb */
		dev_kfree_skb(skb);
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
		return NETDEV_TX_BUSY;
#else
		return 1;
#endif
	}

	Z77DBG(ETHT_MESSAGE_LVL2, "%s: z77_send_packet[%d] len 0x%04x\n", dev->name, idxTx, skb->len );
	dma_handle = dma_map_single( &pcd->dev, (void*)(np->txBd[idxTx].BdAddr), Z77_ETHBUF_SIZE, DMA_TO_DEVICE );
	if (dma_mapping_error( &pcd->dev, dma_handle)) {
		printk( KERN_ERR "*** dma_mapping_error occured, can't dma_map_single()!\n");
		return -ENOMEM;
	}


	np->txBd[idxTx].hdlDma = dma_handle;
	Z077_SET_TBD_ADDR( idxTx, dma_handle);
	src 	= (u8*)buf;
	dst 	= (u8*)np->txBd[idxTx].BdAddr;
	frm_len = skb->len;

#if defined(Z77_USE_VLAN_TAGGING)
	/* VLAN or regular frame ? */
	if ( vlan_tag_present_func( skb )) {
		vlan_id  = vlan_tag_get_func( skb );
		vlan_tag = htonl((ETH_P_8021Q << 16) | vlan_id);

		Z77DBG(ETHT_MESSAGE_LVL2, "VLAN frame: ID 0x%04x\n", vlan_id);

		memcpy(dst, src, ETH_SRCDST_MAC_SIZE ); 						/* copy 12 byte dest/src MAC addresses...  */
		memcpy(dst+ETH_SRCDST_MAC_SIZE, &vlan_tag, VLAN_TAG_SIZE);		/* insert 4 byte VLAN info */
		memcpy(dst + ETH_SRCDST_MAC_SIZE + VLAN_TAG_SIZE, 
			   src + ETH_SRCDST_MAC_SIZE, 
			   frm_len - ETH_SRCDST_MAC_SIZE); 		/* insert rest of the frame */
		frm_len += VLAN_TAG_SIZE;
	} else {
		Z77DBG(ETHT_MESSAGE_LVL2, "\nstandard frame:");
		memcpy(dst, src, skb->len);
	}
#else
	Z77DBG(ETHT_MESSAGE_LVL2, "\nstandard frame:");
	memcpy(dst, src, skb->len);
#endif

	Z077_SET_TBD_LEN(  idxTx, frm_len );

	/* very verbose debugging on? then dump sent frame */
	if ( np->msg_enable >= ETHT_MESSAGE_LVL3 ) {
		dst	= (u8*)np->txBd[idxTx].BdAddr;
		for (i=0; i < frm_len; i++) {
			if (!(i%16))
				Z77DBG(ETHT_MESSAGE_LVL3, "\n0x%03x: ", i);
			Z77DBG(ETHT_MESSAGE_LVL3, "%02x ", (unsigned char)(*dst++));
		}
		Z77DBG(ETHT_MESSAGE_LVL3, "\n");
	}

	/* sync Tx buffer for write to device */
	dma_sync_single_for_device( &pcd->dev, dma_handle, Z77_ETHBUF_SIZE, DMA_TO_DEVICE);
	/* prefetchw( (void*)(np->txBd[idxTx].BdAddr) ); */

	/* sync BD buffer for write to device */
	dma_sync_single_for_device( &pcd->dev, np->bdPhys, PAGE_SIZE, DMA_TO_DEVICE);

	/* finally kick off transmission */
	if (idxTx < 32) {
		Z77WRITE_D32(Z077_BASE, Z077_REG_TXEMPTY0, 1 << idxTx );
	}
	else {
		Z77WRITE_D32(Z077_BASE, Z077_REG_TXEMPTY1, 1 << (idxTx - 32));
	}

	/* sync BD buffer for write to device */
	dma_sync_single_for_device( &pcd->dev, np->bdPhys, PAGE_SIZE, DMA_TO_DEVICE);

	/* dev->trans_start = jiffies; */
	np->stats.tx_bytes += skb->len;

	/* .. and free this skb */
	dev_kfree_skb(skb);

	/* go to next Tx BD */
	np->nCurrTbd++;
	np->nCurrTbd %= Z077_TBD_NUM;

	/* Check if the next Tx BD is empty. If not -> stall . */
	idxTx = np->nCurrTbd;
	/* Z87 Core with extra TXBd empty Flags */
	if ( idxTx < 32 )
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY0) & (1<<idxTx);
	else
		txbEmpty = Z77READ_D32(Z077_BASE, Z077_REG_TXEMPTY1) & (1 << (idxTx-32));

	if (!txbEmpty) { /* congestion? */
		netif_stop_queue(dev);
		Z77DBG(ETHT_MESSAGE_LVL2, "%s: stop_queue\n", __FUNCTION__ );
	}
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,32)
	return NETDEV_TX_OK;
#else
	return 0;
#endif
}

/*******************************************************************/
/** perform a SMBus cycle on EEprom
*
* \param dev	\IN net_device struct for this NIC
* \param c	    \IN command char to describe which cycle to perform
*
* \return 0 or 1 when cycle='G' or 0xffff
*/
static unsigned int smb_cycle(struct net_device *dev, unsigned char c)
{
	int i;
	unsigned int val=0;
	int symboltype=-1;
	unsigned char chip[4][2][3] = { /* signal every cycle as a triplet */
		{{1,1,1},  /* SCL */   /* START */
		 {1,0,0}}, /* SDA */
		{{1,1,1},  /* STOP */
		 {0,1,1}},
		{{0,1,0},  /* '1' or ACK */
		 {1,1,1}},
		{{0,1,0},  /* '0' or low */
		 {0,0,0}}
	};
	switch( c ) {
	case 'S': /* START is SDA = 1,0,0 while SCL high (1,1,1) */
		symboltype =  0;
		break;
	case 'O': /* STOP is SDA = 0,1,1 while SCL high (1,1,1) */
		symboltype =  1;
		break;
	case 'A':
	case '1':
	case 'R':
	case 'G': /* these are SDA = 1,1,1 (tristate) while a SCL pulse(0,1,0)*/
		symboltype =  2;
		break;
	case 'W':
	case '0': /* these are SDA = 0,0,0 (driven low) while a SCL pulse(0,1,0)*/
		symboltype =  3;
		break;
	case 'P':
		udelay(50); /* Pause: change to other value if desired */
		break;
	default:
		printk("*** intern error (SMB sequence wrong char '%c')\n",c);
	}
	if (symboltype >= 0) {
		for (i=0; i< 3; i++) {
			udelay(50);
			if ( (i==1) && (c == 'G')) /* for 'G' cycles, store SDA */
				val = z77_sda_in(Z077_BASE);
			z77_scl_out(Z077_BASE, chip[symboltype][0][i] );
			z77_sda_out(Z077_BASE, chip[symboltype][1][i] );
		}
	}
	return (c == 'G') ? val : 0xffff;
}

/*******************************************************************/
/** Read a byte from MAC EEPROM directly attached to this Z87
 *
 * \param dev		\IN net_device struct for this NIC
 * \param offset	\IN position of byte to read
 *
 * \return byte value at this offset in EEPROM
 * \brief
 *  Every cycle of the SMB access is coded in a descriptive string:
 *	S: Start    0:   0
 *	O: Stop     1:   1
 *	R: Read  	A:   Ack (SDA just set high, no check!)
 *	W: Write    P:   Pause 50 us
 *	G: Get bit (=reads SDA)
 *  Attention: this assumes that exactly one byte is read (8x'G' in a row)!
 */
static unsigned char z77_read_byte_data(struct net_device *dev,
					unsigned int offset )
{
	unsigned int i=0, j=7;
	unsigned char byte=0;
	unsigned char sequence[]="S1010000WA00000000AS1010000RAGGGGGGGGAO\0";

	for (i=0; i < 8; i++) /* insert offset in offset write phase above */
		sequence[10 + i] = (offset & (0x80>>i)) ? '1' : '0';
	i=0;
	while(sequence[i]) {
		if (sequence[i] == 'G') /* G: Get a bit (sda_in) */
			byte |= smb_cycle(dev, sequence[i++]) << (j--); /* assumes 8x'G'! */
		else  					/* other cycles: clocked out straightforward */
			smb_cycle(dev, sequence[i++]);
	}
	return byte;
}

/*******************************************************************/
/** finding a board ident EEPROM to read MAC from
 *
 * \param mac		\IN pointer to 6 byte for retrieved MAC storage
 *
 * \return 1 if MAC retrieved or 0 if none found
 *
 */
static int z77_get_mac_from_board_id(u8 *mac)
{
	int i,j, boardFound=0;
	struct i2c_adapter *adap = NULL;
	struct i2c_board_info i2cinfo;
	struct i2c_client  *client = NULL;
	char brd;
	memset(&i2cinfo, 0, sizeof(struct i2c_board_info));
	i2cinfo.addr  = MEN_BRDID_EE_ADR;
	i2cinfo.flags = I2C_CLASS_HWMON;
	strncpy(i2cinfo.type, "EEP", 3);

	for ( i=0; i < I2C_MAX_ADAP_CNT; i++ ) {
		adap = i2c_get_adapter(i);
		if (adap != NULL) {
			if ((client = i2c_new_device( adap, &i2cinfo ))) {
				/* is it MEN board ident EEPROM ? yes if 'F' or 'G' for a card name found */
				brd = (char)i2c_smbus_read_byte_data(client, ID_EE_NAME_OFF );
				if ((brd == 'F') || (brd == 'G' )) {
					for (j=0; j<6; j++)
						*mac++ = i2c_smbus_read_byte_data(client, MEN_BRDID_EE_MAC_OF + j );
					boardFound = 1;
				} 
				if (boardFound) {
					i2c_unregister_device( client );
					i2c_put_adapter( adap );
					break;
				}
				i2c_unregister_device( client );
			}
			i2c_put_adapter( adap );
		}
	}
	if (boardFound)
		return 1;
	else
		return 0;
}

/*******************************************************************/
/** Initialize the IP core Registers
 *
 * \param dev			\IN net_device struct for this NIC
 * \param first_init	\IN nonzero if Autoneg. shall be performed
 *
 * \brief
 *  setup the necessary Registers like MODER, BDNUM (used on Z77 only)
 *  ans others such that the NIC can completely be restarted if needed.
 *
 * \return 0 or error code
 *
 */
static int chipset_init(struct net_device *dev, u32 first_init)
{
	u32 moder = 0, i=0;
	struct z77_private *np = netdev_priv(dev);
	u8 mac[6] = {0,0,0,0,0,0};
	u32 mac0reg=0, mac1reg=0;

	Z77DBG(ETHT_MESSAGE_LVL1, "--> %s(%d)\n", __FUNCTION__, first_init);

	z77_reset( dev );
	if( first_init==0 ) { /* 1. Check what's already in the MAC Registers */
		mac0reg = Z77READ_D32( Z077_BASE, Z077_REG_MAC_ADDR0 );
		mac1reg = Z77READ_D32( Z077_BASE, Z077_REG_MAC_ADDR1 );
		mac[0] = ( mac1reg >> 8  ) & 0xff;
		mac[1] = ( mac1reg >> 0  ) & 0xff;
		mac[2] = ( mac0reg >> 24 ) & 0xff;
		mac[3] = ( mac0reg >> 16 ) & 0xff;
		mac[4] = ( mac0reg >> 8  ) & 0xff;
		mac[5] = ( mac0reg >> 0  ) & 0xff;
		if ( is_valid_ether_addr( mac )) {
			printk(KERN_INFO "current MAC %02x:%02x:%02x:%02x:%02x:%02x is valid, keeping it.\n", 
				   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
			memcpy(dev->dev_addr, mac, 6);
			goto cont_init;
		}

		/* 2. initial MAC wasn't valid, check for attached MAC EEPROM */
		printk(KERN_INFO "current MAC %02x:%02x:%02x:%02x:%02x:%02x is invalid, try get one from an attached MAC EEPROM.\n", 
			   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
		for (i=0; i < 6; i++ )
			mac[i] = z77_read_byte_data( dev, i+1 );

		if ( is_valid_ether_addr(mac) ) {
			printk(KERN_INFO "got MAC %02x:%02x:%02x:%02x:%02x:%02x from MAC EEPROM, assigning it.\n", 
				   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
			memcpy(dev->dev_addr, mac, 6);
			z77_store_mac( dev );
			goto cont_init;
		}
		printk(KERN_INFO "MAC %02x:%02x:%02x:%02x:%02x:%02x from MAC EEPROM is invalid or no MAC EEPROM attached,"
			   " try get one from Board ID EEPROM.\n", 
			   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );

		/* 3. no MAC EEPROM found or content invalid, check for board Ident EEPROM */
		if ( z77_get_mac_from_board_id(mac)) {
			if ( is_valid_ether_addr(mac) ) {
				printk(KERN_INFO "got MAC %02x:%02x:%02x:%02x:%02x:%02x from Board ID EEPROM, assigning it.\n", 
					   mac[0], mac[1], mac[2], mac[3], mac[4], mac[5] );
				memcpy(dev->dev_addr, mac, 6);
				z77_store_mac( dev );
				goto cont_init;
			}
		}
#if LINUX_VERSION_CODE > KERNEL_VERSION(3,4,0)
		printk(KERN_INFO "MAC from BOARD EEPROM is invalid or no board EEPROM found. Resorting to random MAC.\n" );
		eth_hw_addr_random( dev );
		printk(KERN_INFO "assigning MAC %02x:%02x:%02x:%02x:%02x:%02x ", 
			   dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2], dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5] );	
		z77_store_mac( dev );
#endif
	}

cont_init:
	np->mii_if.phy_id = phyadr[np->instCount];
	if (first_init) {
		if (z77_do_autonegotiation(dev)) {
			printk(KERN_ERR "*** PHY mode setting / autonegotiate failed!\n");
			return(-ENODEV);
		}
	}

	moder = Z77READ_D32( Z077_BASE, Z077_REG_MODER );
	moder |= OETH_MODER_IFG | OETH_MODER_EXDFREN | OETH_MODER_CRCEN | OETH_MODER_BRO | OETH_MODER_PAD | OETH_MODER_RECSMALL;
	
	if ( ((np->mii_if.full_duplex) || ((mode[np->instCount] == phymode_10fd ) || \
									   (mode[np->instCount] == phymode_10fd))) &&	 (moder & OETH_MODER_HD_AVAL)) {
		moder |= OETH_MODER_FULLD;
	} 
	Z77WRITE_D32( Z077_BASE, Z077_REG_MODER, moder);
	Z77WRITE_D32( Z077_BASE, Z077_REG_PACKLEN, Z77_PACKLEN_DEFAULT );
	Z77WRITE_D32( Z077_BASE, Z077_REG_TX_BDNUM, Z077_TBD_NUM );

	/* set management indices */
	np->nCurrTbd  	= 0;
	np->txIrq		= 0;

	Z77DBG(ETHT_MESSAGE_LVL1, "<-- %s()\n", __FUNCTION__);
	return(0);
}

/*******************************************************************/
/** The interrupt context Tx packet handler
 *
 * \brief Called from within main ISR context.
 *		  unmaps allocated PCI transfer memory region and updates
 *        stats info. No spin_lock here because main irq routine does
 *        local Eth core IRQ disabling already.
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return -
 */
void z77_tx(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);

	pci_unmap_single(np->pdev, np->txBd[np->txIrq].hdlDma, Z77_ETHBUF_SIZE, DMA_TO_DEVICE);
	np->txIrq++;
	np->txIrq%= Z077_TBD_NUM;
	np->stats.tx_packets++;

	/* If we had stopped the queue due to a "tx full" condition, wake up the queue. */
	if (netif_queue_stopped(dev))
		netif_wake_queue(dev);
}

/*****************************************************************************/
/** z77_pass_packet - packet passing function for one ETH frame
 *
 * \param dev		\IN net_device struct for this NIC
 * \param idx		\IN Rx BD index, 0..Z077_RBD_NUM
 *
 * \return 			-
 */
static void z77_pass_packet( struct net_device *dev, unsigned int idx )
{

	struct z77_private *np = netdev_priv(dev);
	struct pci_dev *pcd = np->pdev;
	struct sk_buff *skb = NULL;
	u32 pkt_len = 0;
	int i=0;
	u8 *dst=NULL;

	prefetch(np->rxBd[idx].BdAddr);		

	/* sync in data from IP core */
	dma_sync_single_for_cpu( &pcd->dev, np->rxBd[idx].hdlDma, Z77_ETHBUF_SIZE, DMA_FROM_DEVICE );
	
	pkt_len	= Z077_GET_RBD_LEN( idx );

	if (( np->rxBd[idx].BdAddr == NULL ) || ( pkt_len == 0 )) {
		Z77DBG(ETHT_MESSAGE_LVL3, "invalid length of pkt %d (len=%d)!\n", idx, pkt_len );
		return;
	}

	pkt_len	-= LEN_CRC;

	skb = dev_alloc_skb( pkt_len + NET_IP_ALIGN );
	Z77DBG(ETHT_MESSAGE_LVL2, "z77_pass_packet[%d]: pktlen=%04x\n", idx, pkt_len);
	if (skb) {
		skb->dev = dev;
		skb_reserve(skb, NET_IP_ALIGN); /* 16 byte align the IP fields. */
		skb_copy_to_linear_data(skb, (void*)(np->rxBd[idx].BdAddr ), pkt_len);

		skb_put(skb, pkt_len);
		skb->protocol = eth_type_trans (skb, dev);

		/* sync in data from IP core */
		dma_sync_single_for_cpu( &pcd->dev, np->rxBd[idx].hdlDma, Z77_ETHBUF_SIZE, DMA_FROM_DEVICE );
		
		/* tell network stack... */
		netif_receive_skb(skb);
#if LINUX_VERSION_CODE  < KERNEL_VERSION(4,10,0)
		dev->last_rx 		= jiffies;
#endif
		np->stats.rx_bytes += pkt_len;
		np->stats.rx_packets++;

		/* clean processed Rx BD nonempty Flag */
		if ( idx < 32 ) {
			Z77WRITE_D32(Z077_BASE, Z077_REG_RXEMPTY0, 1 << idx );
		}
		else {
			Z77WRITE_D32(Z077_BASE, Z077_REG_RXEMPTY1, 1 << (idx-32));
		}
		smp_mb();

	} else {
		printk (KERN_WARNING "*** %s:Mem squeeze! drop packet\n",dev->name);
		np->stats.rx_dropped++;		
	}
}

/*******************************************************************/
/** The inverse routine to z77_open().
 *
 *  The network interfaces resources are deallocated here
 *
 * \param dev		\IN net_device struct for this NIC
 *
 * \return error code or 0 on sucess
 */
static int z77_close(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	struct pci_dev *pcd = np->pdev;
	int i = 0;
	
	Z77DBG( ETHT_MESSAGE_LVL1, "--> %s()\n", __FUNCTION__ );
	np->open_time = 0;
	np->flags &= ~(IFF_UP);
	netif_stop_queue(dev);
	napi_disable(&np->napi);

	/* stop receiving/transmitting */
	Z077_CLR_MODE_FLAG( OETH_MODER_RXEN | OETH_MODER_TXEN );

	/* disable all IRQs */
	Z77WRITE_D32( Z077_BASE, Z077_REG_INT_MASK, 0 );

	/* clean spurious left IRQs */
	Z77WRITE_D32( Z077_BASE, Z077_REG_INT_SRC, 0x7f );

	del_timer_sync(&np->timer);

	free_irq(dev->irq, dev);

	/* free DMA resources */
	for (i = 0; i < Z077_RBD_NUM; i++ ) 
		dma_free_coherent(&pcd->dev, Z77_ETHBUF_SIZE, np->txBd[i].BdAddr, np->txBd[i].hdlDma);

	/* Rx BDs, these don't get unmapped after each packet so do that here */
	for (i = 0; i < Z077_RBD_NUM; i++ ) {
		dma_unmap_single( &pcd->dev, np->txBd[i].hdlDma, Z77_ETHBUF_SIZE, DMA_TO_DEVICE);
		dma_free_coherent( &pcd->dev, Z77_ETHBUF_SIZE, np->rxBd[i].BdAddr, np->rxBd[i].hdlDma);
	}

	Z77DBG( ETHT_MESSAGE_LVL1, "<-- %s()\n", __FUNCTION__ );
	return 0;
}

/*******************************************************************/
/** central IRQ handler
 *
 * \param irq		\IN INTB interrupt
 * \param dev_id	\IN unique identifier
 *
 * \return if IRQ was handled or not
 */
static irqreturn_t z77_irq(int irq, void *dev_id)
{
	/* uses dev_id to store 'this' net_device */
	struct net_device *dev = (struct net_device *)dev_id;
	struct z77_private *np = netdev_priv(dev);
	u8 *dst=NULL;
	int pkt_len,i,handled = 0;

	u32 status = Z77READ_D32( Z077_BASE, Z077_REG_INT_SRC );
	if (!status) {
		goto out;	/* It wasnt me, ciao. */
	}

	if (status & OETH_INT_RXF) { 			/* Got a packet. */
		Z077_DISABLE_IRQ( OETH_INT_RXF ); 	/* reenabled in NAPI poll routine */
		Z77WRITE_D32(Z077_BASE, Z077_REG_INT_SRC, status  );
		napi_schedule(&np->napi);
	}

	if (status & OETH_INT_TXB) { 	/* Transmit complete. */
		Z77WRITE_D32(Z077_BASE, Z077_REG_INT_SRC, status  );
		z77_tx(dev);
	}

	if (status & OETH_INT_BUSY) { 	/* RX FIFO overrun ? */
		Z77WRITE_D32(Z077_BASE, Z077_REG_INT_SRC, status & ~OETH_INT_RXE );
	}

	if (status & OETH_INT_TXE) {  	/* handle Tx Error */
		Z77WRITE_D32(Z077_BASE, Z077_REG_INT_SRC, status  );
		z77_tx_err(dev);
	}

	if (status & OETH_INT_RXE) {	/* handle Rx Error */
		Z77WRITE_D32(Z077_BASE, Z077_REG_INT_SRC, status  );
		z77_rx_err(dev);
	}

	handled = 1;
out:
	return IRQ_RETVAL(handled);
}

/*****************************************************************************/
/** Get the current statistics.
 *
 * \param dev		\IN net_device struct for this NIC
 * \return pointer to device status struct
 */
static struct net_device_stats *z77_get_stats(struct net_device *dev)
{
	struct z77_private *np = netdev_priv(dev);
	/* Update the statistics from the device registers. */
	return &np->stats;
}

/*****************************************************************************/
/** men_16z077_probe - PNP function for ETH, per instance
 *
 * \param chu		\IN wdt unit found
 * \return 			0 on success or negative linux error number
 *
 */
int men_16z077_probe( CHAMELEON_UNIT_T *chu )
{

	u32 phys_addr 			= 0;
	struct net_device *dev 	= NULL;
	struct z77_private *np 	= NULL;
	dma_addr_t memPhysDma;
	void *     memVirtDma = NULL;

	dev = alloc_etherdev(sizeof(struct z77_private));
	if (!dev)
		return -ENOMEM;

	/* enable bus mastering and DMA mask for this driver */
	pci_set_master( chu->pdev );

	/* this driver support 32bit PCI core/registers yet so make sure we get
	   DMAable memory from that range. Should one day 64bit IP cores arrive the bit mask
	   can be set to 64. */
	if ( dma_set_mask_and_coherent(&chu->pdev->dev, DMA_BIT_MASK(32)) ) {
		printk(KERN_ERR MEN_Z77_DRV_NAME "can't set 32bit DMA mask, aborting\n");
		goto err_free_reg;
	}

	phys_addr = pci_resource_start(chu->pdev, chu->bar) + chu->offset;
	dev->base_addr = (unsigned long)ioremap_nocache(phys_addr, (u32)Z77_CFGREG_SIZE );
	dev->irq       = chu->irq;

	if( dev_alloc_name( dev, "eth%d") < 0)
		printk("*** warning: couldnt retrieve a name for Z77\n");

	/* setup the phy Info within the private z77_private struct */
	np = netdev_priv(dev);

	np->pdev = chu->pdev;
	spin_lock_init(&np->lock);
	pci_set_drvdata(chu->pdev, dev);

	netif_napi_add( dev, &np->napi, z77_poll, Z077_WEIGHT );
	np->dev = dev;

	/* store Z87 instance to set its PHY address later, chu->instance starts @ 0 for every FPGA */
	np->instance  = chu->instance;
	np->instCount = G_globalInstanceCount;
	G_globalInstanceCount++;

#if defined(Z77_USE_VLAN_TAGGING)
	dev->features |= Z87_VLAN_FEATURES;
#endif
	/* pass initial debug level */
	np->msg_enable = (dbglvl > Z77_MAX_MSGLVL) ? Z77_MAX_MSGLVL : dbglvl;

	printk(KERN_INFO MEN_Z77_DRV_NAME "initial debug level %d\n", np->msg_enable );

	np->modCode = chu->modCode;

	/* get a coherent DMAable memory region for the BDs to have the 64 Rx/Tx BD stati in sync with IP core */
	memVirtDma = dma_alloc_coherent( &chu->pdev->dev, PAGE_SIZE, &memPhysDma, GFP_KERNEL );
	printk( KERN_INFO MEN_Z77_DRV_NAME "dma_alloc_coherent BD table memory @ CPU addr 0x%p, DMA addr 0x%p\n", memVirtDma, memPhysDma);
	memset((char*)(memVirtDma), 0, PAGE_SIZE);

	/* store for dma_free_coherent at module remove */
	np->bdBase = memVirtDma;
	np->bdPhys = memPhysDma;
	Z77WRITE_D32( Z077_BASE, Z077_REG_BDSTART, memPhysDma );

	np->tbdOff	= Z077_TBD_NUM;
	np->rbdOff	= 0;
	np->bdOff	= 0;

	/* clean BD Area */
	memset((void*)np->bdBase, 0, Z077_BD_AREA_SIZE );
	strncpy(cardname, "16Z087", sizeof(cardname));

	printk(KERN_INFO MEN_Z77_DRV_NAME "register 16Z087 as %s mode %d phys.addr 0x%08x irq 0x%x \n",
		   dev->name, mode[np->instCount], (u32)phys_addr, chu->irq );

	/* ok so far, store dev in Chameleon units driver_data for removal */
	chu->driver_data = (void*)dev;

	/* force new interrupt behavior */
	Z77WRITE_D32( dev->base_addr, Z077_REG_COREREV, Z77READ_D32(dev->base_addr, Z077_REG_COREREV) | REG_COREREV_IRQNEWEN );
	if( !(Z77READ_D32(dev->base_addr, Z077_REG_COREREV)
		  & REG_COREREV_IRQNEWEN) ) {
		printk(KERN_WARNING "%s: Couldn't set to new IRQ behavior\n", __func__);
	}

	/* set up timer to poll for link state changes */
	init_timer(&np->timer);
	np->timer.expires 	= jiffies + CONFIG_HZ / 2;
	np->timer.data 		= (unsigned long)dev;
	np->timer.function 	= z77_timerfunc;

	/* init the process context work queue function to restart Z77 */
	INIT_WORK(&np->reset_task, z77_reset_task);
#if LINUX_VERSION_CODE  < KERNEL_VERSION(2,6,30)
	dev->open				= z77_open;
	dev->stop				= z77_close;
	dev->hard_start_xmit	= z77_send_packet;
	dev->get_stats			= z77_get_stats;
	dev->tx_timeout			= z77_tx_timeout;
	dev->do_ioctl			= z77_ioctl;
	dev->get_stats			= z77_get_stats;
	dev->set_multicast_list	= z77_set_rx_mode;
#else
	dev->netdev_ops 		= &z77_netdev_ops;
#endif
	dev->watchdog_timeo		= MY_TX_TIMEOUT;

	/* use PHY address from passed module parameter */
	np->mii_if.phy_id_mask 	= 0x1f;
	np->mii_if.reg_num_mask = 0x1f;
	np->mii_if.dev 			= dev;
	np->mii_if.mdio_read 	= z77_mdio_read;
	np->mii_if.mdio_write 	= z77_mdio_write;

	/* YES, we support the ethtool utility */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,16,0)
	SET_ETHTOOL_OPS(dev,    &z77_ethtool_ops);
#else
	dev->ethtool_ops = &z77_ethtool_ops;
#endif
	/* Data setup done, now setup Connection */
	if (chipset_init(dev, 0)) {
		printk(KERN_ERR "*** probe_z77: Ethernet core init failed!\n");
		goto err_free_reg;
	} else {
		if (register_netdev(dev) == 0) {
			if (device_create_file(&dev->dev, &dev_attr_linkstate))
				dev_err(&dev->dev, "Error creating sysfs file\n");
		}
		return 0;
	}

err_free_reg:
	/* arrived here, we failed */
	pci_set_drvdata(chu->pdev, NULL);
	free_netdev(dev);
	printk("*** men_16z077_probe failed\n");
	cleanup_card(dev);
	return -ENXIO;
}

/*******************************************************************/
/** PNP function to remove the unit driver
 *
 *  The netdev struct is kept in struct CHAMELEON_UNIT_Ts private driver
 *  data pointer so any number of found netdevs can be freed.
 *
 * \param chu		\IN IP core unit to remove
 *
 * \return 0 on success or negative linux error number
 */
static int men_16z077_remove( CHAMELEON_UNIT_T *chu )
{
	struct net_device *dev = (struct net_device *)chu->driver_data;
	struct z77_private *np = netdev_priv(dev);
	Z77DBG( ETHT_MESSAGE_LVL2, "--> men_16z077_remove\n" );
	netif_napi_del(&np->napi);
	cancel_work_sync(&np->reset_task);
	dma_free_coherent(&chu->pdev->dev, PAGE_SIZE, np->bdBase, np->bdPhys );
	unregister_netdev(dev);
	return 0;
}

static u16 G_modCodeArr[] = {
	CHAMELEON_16Z087_ETH,
	CHAMELEON_MODCODE_END
};

static CHAMELEON_DRIVER_T G_driver = {
	.name		=	"z087-eth",
	.modCodeArr = 	G_modCodeArr,
	.probe		=	men_16z077_probe,
	.remove		= 	men_16z077_remove
};

/*******************************************************************/
/** ioctl function - interface to applications
 *
 * \param dev		\IN net_device struct for this NIC
 * \param ifr       \IN interface request struct
 * \param cmd       \IN command
 *
 * \return 0 on success or negative linux error number
 */
static int z77_ioctl(struct net_device *dev, struct ifreq *ifr, int cmd)
{
	struct mii_ioctl_data *data = if_mii(ifr);
	int value;
	unsigned char phyAddr;
	struct z77_private *np = netdev_priv(dev);

	switch (cmd) {
	case SIOCGMIIPHY:
		phyAddr = np->mii_if.phy_id;
		data->phy_id = phyAddr;
		break;
	case SIOCGMIIREG:
		value = z77_mdio_read(dev, data->phy_id, data->reg_num);
		data->val_out = value;
		break;
	case SIOCSMIIREG:
		z77_mdio_write(dev, data->phy_id, data->reg_num, data->val_in);
		break;
	default:
		return -EOPNOTSUPP;
	}
	return 0;
}

/********************************************************************/
/** drivers cleanup routine
 *
 */
static void __exit men_16z077_cleanup(void)
{
	men_chameleon_unregister_driver( &G_driver );
}

/*******************************************************************/
/** module init function
 *
 *  We are a chameleon unit driver rather than a true standalone
 *  Ethernet driver, so all Z77 detection, negotiation and stuff is
 *  done at its probe() routine
 *
 * \return		0 or error code from men_chameleon_register_driver
 */
static int __init men_16z077_init(void)
{
	int i=0;
#if defined(Z77_USE_VLAN_TAGGING)
	printk(KERN_INFO MEN_Z77_DRV_NAME "VLAN support enabled.\n");
#endif
	printk(KERN_INFO MEN_Z77_DRV_NAME "version %s\n", version);
	
	/* check passed module parameters */
	for (i = 0; i < NR_ETH_CORES_MAX; i++ ) {
		if ( (phyadr[i] < 0) || ((phyadr[i] > PHY_MAX_ADR))) {
			printk(KERN_ERR "*** invalid phyadr[%d] = %d, must be 0..31 !\n", i, phyadr[i] );
			goto errout;
		}
		if ( (mode[i] != phymode_auto) && (mode[i] != phymode_10hd) && (mode[i] != phymode_10fd) && 
			 (mode[i] != phymode_100hd) && ((mode[i] != phymode_100fd))) {
			printk(KERN_ERR "*** invalid phyadr[%d] = %d, must be 0..31 !\n", i, phyadr[i] );
			goto errout;
		}
	}

	/* men_chameleon_register_driver returns count of found instances */
	if (!men_chameleon_register_driver( &G_driver ))
		return -ENODEV;
	else
		return 0;
errout:
	return -EINVAL;
}

module_init(men_16z077_init);
module_exit(men_16z077_cleanup);

MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "MEN Ethernet IP Core driver" );
MODULE_AUTHOR("thomas.schnuerer@men.de");
