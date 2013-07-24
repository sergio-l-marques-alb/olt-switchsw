
/******************************************************************************
 *
 *  $Id: zl30310.c 8869 2012-08-22 13:05:35Z RB $
 *
 ******************************************************************************/

/*
 * drivers/net/zl30310.c
 *
 * Gianfar Ethernet Driver
 * This driver is designed for the non-CPM ethernet controllers
 * on the 85xx and 83xx family of integrated processors
 * Based on 8260_io/fcc_enet.c
 *
 * Author: Andy Fleming
 * Maintainer: Kumar Gala
 *
 * Copyright (c) 2002-2008 Freescale Semiconductor, Inc.
 * Copyright (c) 2007 MontaVista Software, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 *  Gianfar:  AKA Lambda Draconis, "Dragon"
 *  RA 11 31 24.2
 *  Dec +69 19 52
 *  V 3.84
 *  B-V +1.62
 *
 *  Theory of operation
 *
 *  The driver is initialized through platform_device.  Structures which
 *  define the configuration needed by the board are defined in a
 *  board structure in arch/ppc/platforms (though I do not
 *  discount the possibility that other architectures could one
 *  day be supported.
 *
 *  The Gianfar Ethernet Controller uses a ring of buffer
 *  descriptors.  The beginning is indicated by a register
 *  pointing to the physical address of the start of the ring.
 *  The end is determined by a "wrap" bit being set in the
 *  last descriptor of the ring.
 *
 *  When a packet is received, the RXF bit in the
 *  IEVENT register is set, triggering an interrupt when the
 *  corresponding bit in the IMASK register is also set (if
 *  interrupt coalescing is active, then the interrupt may not
 *  happen immediately, but will wait until either a set number
 *  of frames or amount of time have passed).  In NAPI, the
 *  interrupt handler will signal there is work to be done, and
 *  exit.  Without NAPI, the packet(s) will be handled
 *  immediately.  Both methods will start at the last known empty
 *  descriptor, and process every subsequent descriptor until there
 *  are none left with data (NAPI will stop after a set number of
 *  packets to give time to other tasks, but will eventually
 *  process all the packets).  The data arrives inside a
 *  pre-allocated skb, and so after the skb is passed up to the
 *  stack, a new skb must be allocated, and the address field in
 *  the buffer descriptor must be updated to indicate this new
 *  skb.
 *
 *  When the kernel requests that a packet be transmitted, the
 *  driver starts where it left off last time, and points the
 *  descriptor at the buffer which was passed in.  The driver
 *  then informs the DMA engine that there are packets ready to
 *  be transmitted.  Once the controller is finished transmitting
 *  the packet, an interrupt may be triggered (under the same
 *  conditions as for reception, but depending on the TXF bit).
 *  The driver then cleans up the buffer.
 *
 *  Nov 2011: .5 Go to first busyBuffer within driver
 *  Oct 2011: .4 Additional pattern length check
 *  Sep 2010: .3 Modified for additional Rx buffers
 *  Jul 2010: .2 Modified for user-specified packet filtering
 *  Feb 2010: .1 Modified for Microsemi ZL30310 Network Port
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/unistd.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/platform_device.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/in.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/reg.h>
#include <linux/module.h>
#include <linux/dma-mapping.h>
#include <linux/crc32.h>
#include <linux/mii.h>
#include <linux/phy.h>

#include "zl30310.h"

#define TX_TIMEOUT      (1*HZ)
#define SKB_ALLOC_TIMEOUT 1000000
#undef BRIEF_GFAR_ERRORS
#undef VERBOSE_GFAR_ERRORS

#ifdef CONFIG_GFAR_NAPI
#ERROR_CONFIG_GFAR_NAPI
#define RECEIVE(x) netif_receive_skb(x)
#else
#define RECEIVE(x) netif_rx(x)
#endif

const char zl30310_driver_name[] = "CXP Ethernet";
const char zl30310_driver_version[] = "1.5";


/* The Microsemi TimeStamp (TS) Pkt to user-space driver defines. */
//JVM ---
#define ZL_CHAR_DEV

static int zl30310_enet_open(struct net_device *dev);
static int zl30310_start_xmit(struct sk_buff *skb, struct net_device *dev);
static void zl30310_timeout(struct net_device *dev);
static int zl30310_close(struct net_device *dev);
struct sk_buff *zl30310_new_skb(struct net_device *dev, struct rxbd8 *bdp);
static struct net_device_stats *zl30310_get_stats(struct net_device *dev);
static int zl30310_set_mac_address(struct net_device *dev);
static int zl30310_change_mtu(struct net_device *dev, int new_mtu);
static irqreturn_t zl30310_error(int irq, void *dev_id);
static irqreturn_t zl30310_transmit(int irq, void *dev_id);
static irqreturn_t zl30310_interrupt(int irq, void *dev_id);
static void adjust_link(struct net_device *dev);
static void init_registers(struct net_device *dev);
static int init_phy(struct net_device *dev);
static int zl30310_probe(struct platform_device *pdev);
static int zl30310_remove(struct platform_device *pdev);
static void free_skb_resources(struct zl30310_private *priv);
static void zl30310_set_multi(struct net_device *dev);
static void zl30310_set_hash_for_addr(struct net_device *dev, u8 *addr);
static void zl30310_configure_serdes(struct net_device *dev);
extern int gfar_local_mdio_write(struct gfar_mii *regs, int mii_id, int regnum, u16 value);
extern int gfar_local_mdio_read(struct gfar_mii *regs, int mii_id, int regnum);
#ifdef CONFIG_GFAR_NAPI
static int zl30310_poll(struct net_device *dev, int *budget);
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
static void zl30310_netpoll(struct net_device *dev);
#endif
int zl30310_clean_rx_ring(struct net_device *dev, int rx_work_limit);
static int zl30310_process_frame(struct net_device *dev, struct sk_buff *skb, int length);
static void zl30310_vlan_rx_register(struct net_device *netdev,
		                struct vlan_group *grp);
//static void gfar_halt_nodisable(struct net_device *dev);
void zl30310_halt(struct net_device *dev);
void zl30310_start(struct net_device *dev);
static void zl30310_clear_exact_match(struct net_device *dev);
static void zl30310_set_mac_for_addr(struct net_device *dev, int num, u8 *addr);

extern const struct ethtool_ops gfar_ethtool_ops;

#ifdef ZL_CHAR_DEV
    /* File operations supported by the ZL TS to UserSpace driver. */
    static int zlPktToUser_open (struct inode *inode, struct file *filp);
    static int zlPktToUser_release(struct inode *inode, struct file *filp);
    static ssize_t zlPktToUser_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
    static ssize_t zlCmdFromUser_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);

    struct file_operations zlPktToUser_fops = {
        .owner   = THIS_MODULE,
        .open    = zlPktToUser_open,
        .release = zlPktToUser_release,
        .read    = zlPktToUser_read,
        .write   = zlCmdFromUser_write,
    };

    #define ZL_USES_FPE     /* SDK versions earlier than 3.0.0 would not define this! */
    #undef  ZL_NTP_SERVER   /* Special - Non-FPE accurate timestamp NTP server mode */

    /* Must match with _FpeFilter.c! */
    #define FPE_C 0     /* Command 0=Delete, 1=Add */
    #define FPE_F 1     /* Filter# */
    #define FPE_O 2     /* Offset */
    #define FPE_L 3     /* Length */
    #define FPE_P 4     /* Pattern */
    #define FPE_CMD 70  /* holds above cmds + 64 pattern bytes  */
    #define PKT_MATCH_CONFIG_CMD_SIZE FPE_CMD

    #define RX_PKT_Q_SIZE     16    /* Max 63 (<sizeof a u_int64_t)*/

    #undef DEBUG
    #undef MORE_DEBUG

    #define ZL_TS_MAJOR 66              /* "Borrow" this major number */
    #define DRIVER_NAME "zlTsPkts"
    #define MAX_ZL_PKT_MINOR 1

    #ifndef USE_PKT_PREAMBLE
    	#define PKT_PREAMBLE_LEN 0
    #else
    	#define PKT_PREAMBLE_LEN 1     /* byte 0 - Indicates rule that matched */
    #endif
    #define START_OF_PAYLOAD PKT_PREAMBLE_LEN

    #define MAX_PKT_LEN_TO_USER 1522 + 43            /* Max payload + header */
    /* End of the Microsemi defines */


    char filterCmd[FPE_CMD]        = {0};        /* Inbound command */
    char filterTable[64][FPE_CMD]  = {{0},{0}};  /* Current filter table */
    long long int filterActiveBits = 0;          /* Bitfields of active filters - 64 max */
    long long int busyBufferBits = 0;            /* Bitfields of active buffers - 64 max */

    u_char rx_kbuf[RX_PKT_Q_SIZE][PKT_PREAMBLE_LEN + MAX_PKT_LEN_TO_USER + START_OF_PAYLOAD] = {{0},{0}};     /* Rx buffer pool */
    u_int32_t pktLengthToUser[RX_PKT_Q_SIZE] = {0};
    static u_int32_t currRxIndx = 0;             /* The index the eth rx intr handler is currently writing */

    static u_char devInUse = 0;                  /* Boolean - One user at a time */
    static DECLARE_WAIT_QUEUE_HEAD(qTsPktWait);  /* TS pkt client waiting queue */

#endif


MODULE_AUTHOR("Zarlink Semiconductor, Inc");
MODULE_DESCRIPTION("Zarlink Ethernet Driver");
MODULE_LICENSE("GPL");

/* Returns 1 if incoming frames use an FCB */
static inline int zl30310_uses_fcb(struct zl30310_private *priv)
{
	return (priv->vlan_enable || priv->rx_csum_enable);
}

/***************************************************************
 *  zl30310_probe
 *     Set up the ethernet device structure
 *
 ***************************************************************/
static int zl30310_probe(struct platform_device *pdev)
{
	u32 tempval;
	struct net_device *dev = NULL;
	struct zl30310_private *priv = NULL;
	struct gianfar_platform_data *einfo;
	struct resource *r;
	int idx;
	int err = 0;
	//JVM u32 svr;

	einfo = (struct gianfar_platform_data *) pdev->dev.platform_data;

	if (NULL == einfo) {
		printk(KERN_ERR "zl30310 %d: Missing additional data!\n",
		       pdev->id);

		return -ENODEV;
	}

	/* Create an ethernet device instance */
	dev = alloc_etherdev(sizeof (*priv));

	if (NULL == dev)
		return -ENOMEM;

	priv = netdev_priv(dev);

	/* Set the info in the priv to the current info */
	priv->einfo = einfo;

	/* fill out IRQ fields */
	if (einfo->device_flags & FSL_GIANFAR_DEV_HAS_MULTI_INTR) {
                priv->interruptTransmit = platform_get_irq_byname(pdev, "eth1_tx");
                priv->interruptReceive = platform_get_irq_byname(pdev, "eth1_rx");
                priv->interruptError = platform_get_irq_byname(pdev, "eth1_error");
		if (priv->interruptTransmit < 0 || priv->interruptReceive < 0 || priv->interruptError < 0)
			goto regs_fail;
	} else {
		priv->interruptTransmit = platform_get_irq(pdev, 0);
		if (priv->interruptTransmit < 0)
			goto regs_fail;
	}

	/* get a pointer to the register memory */
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	priv->regs = ioremap(r->start, sizeof (struct zl30310));

	if (NULL == priv->regs) {
		err = -ENOMEM;
		goto regs_fail;
	}

	spin_lock_init(&priv->txlock);
	spin_lock_init(&priv->rxlock);
	spin_lock_init(&priv->bflock);

	platform_set_drvdata(pdev, dev);

	/* Stop the DMA engine now, in case it was running before */
	/* (The firmware could have used it, and left it running). */
	/* To do this, we write Graceful Receive Stop and Graceful */
	/* Transmit Stop, and then wait until the corresponding bits */
	/* in IEVENT indicate the stops have completed. */
	tempval = zl30310_read(&priv->regs->dmactrl);
	tempval &= ~(DMACTRL_GRS | DMACTRL_GTS);
	zl30310_write(&priv->regs->dmactrl, tempval);

	tempval = zl30310_read(&priv->regs->dmactrl);
	tempval |= (DMACTRL_GRS | DMACTRL_GTS);
	zl30310_write(&priv->regs->dmactrl, tempval);

	while (!(zl30310_read(&priv->regs->ievent) & (IEVENT_GRSC | IEVENT_GTSC)))
		cpu_relax();

	/* Reset MAC layer */
	zl30310_write(&priv->regs->maccfg1, MACCFG1_SOFT_RESET);

/* JVM
	svr = mfspr(SPRN_SVR);
	// MPC8315E Rev1.2 or MPC8313E Rev2.1
	if (svr == 0x80B40012 || svr == 0x80B00021) {
		dev_info(&pdev->dev, "enabling Lossless Flow Control\n");
		priv->lfc_enable = 1;
		// FIXME: threshold configurable?
		priv->free_bd_thres = 3;
	}

	if (priv->lfc_enable) {
		tempval = (MACCFG1_TX_FLOW | MACCFG1_RX_FLOW);
	} else {
		// do not enable flow control due to errata eTSEC27 for MPC8313
		// bring the MAC out of reset by clearing the soft reset bit
		tempval = zl30310_read(&priv->regs->maccfg1);
		tempval &= ~MACCFG1_SOFT_RESET;
	}
*/

	tempval = (MACCFG1_TX_FLOW | MACCFG1_RX_FLOW);
	zl30310_write(&priv->regs->maccfg1, tempval);

	/* Initialize MACCFG2. */
	zl30310_write(&priv->regs->maccfg2, MACCFG2_INIT_SETTINGS);

	/* Initialize ECNTRL */
	zl30310_write(&priv->regs->ecntrl, ECNTRL_INIT_SETTINGS);

	/* Copy the station address into the dev structure, */
	memcpy(dev->dev_addr, einfo->mac_addr, MAC_ADDR_LEN);

	/* Set the dev->base_addr to the zl30310 reg region */
	dev->base_addr = (unsigned long) (priv->regs);

	//SET_MODULE_OWNER(dev);
	SET_NETDEV_DEV(dev, &pdev->dev);

	/* Fill in the dev structure */
	dev->open = zl30310_enet_open;
	dev->hard_start_xmit = zl30310_start_xmit;
	dev->tx_timeout = zl30310_timeout;
	dev->watchdog_timeo = TX_TIMEOUT;
#if defined(CONFIG_GFAR_PTP)
        /* Flags for enabling ptp Timestamping and ptp ioctl handler */
        priv->ptp_timestamping = 1;
        dev->do_ioctl = gfar_ioctl;
#endif
#ifdef CONFIG_GFAR_NAPI
	dev->poll = zl30310_poll;
	dev->weight = GFAR_DEV_WEIGHT;
#endif
#ifdef CONFIG_NET_POLL_CONTROLLER
	dev->poll_controller = zl30310_netpoll;
#endif
	dev->stop = zl30310_close;
	dev->get_stats = zl30310_get_stats;
	dev->change_mtu = zl30310_change_mtu;
	dev->mtu = 1500;
	dev->set_multicast_list = zl30310_set_multi;

	dev->ethtool_ops = &gfar_ethtool_ops;

	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_CSUM) {
		priv->rx_csum_enable = 1;
		dev->features |= NETIF_F_IP_CSUM;
	} else
		priv->rx_csum_enable = 0;

	priv->vlgrp = NULL;

	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_VLAN) {
		dev->vlan_rx_register = zl30310_vlan_rx_register;

		dev->features |= NETIF_F_HW_VLAN_TX | NETIF_F_HW_VLAN_RX;

		priv->vlan_enable = 1;
	}

	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_EXTENDED_HASH) {
		priv->extended_hash = 1;
		priv->hash_width = 9;

		priv->hash_regs[0] = &priv->regs->igaddr0;
		priv->hash_regs[1] = &priv->regs->igaddr1;
		priv->hash_regs[2] = &priv->regs->igaddr2;
		priv->hash_regs[3] = &priv->regs->igaddr3;
		priv->hash_regs[4] = &priv->regs->igaddr4;
		priv->hash_regs[5] = &priv->regs->igaddr5;
		priv->hash_regs[6] = &priv->regs->igaddr6;
		priv->hash_regs[7] = &priv->regs->igaddr7;
		priv->hash_regs[8] = &priv->regs->gaddr0;
		priv->hash_regs[9] = &priv->regs->gaddr1;
		priv->hash_regs[10] = &priv->regs->gaddr2;
		priv->hash_regs[11] = &priv->regs->gaddr3;
		priv->hash_regs[12] = &priv->regs->gaddr4;
		priv->hash_regs[13] = &priv->regs->gaddr5;
		priv->hash_regs[14] = &priv->regs->gaddr6;
		priv->hash_regs[15] = &priv->regs->gaddr7;

	} else {
		priv->extended_hash = 0;
		priv->hash_width = 8;

		priv->hash_regs[0] = &priv->regs->gaddr0;
                priv->hash_regs[1] = &priv->regs->gaddr1;
		priv->hash_regs[2] = &priv->regs->gaddr2;
		priv->hash_regs[3] = &priv->regs->gaddr3;
		priv->hash_regs[4] = &priv->regs->gaddr4;
		priv->hash_regs[5] = &priv->regs->gaddr5;
		priv->hash_regs[6] = &priv->regs->gaddr6;
		priv->hash_regs[7] = &priv->regs->gaddr7;
	}

	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_PADDING)
		priv->padding = DEFAULT_PADDING;
	else
		priv->padding = 0;
#if defined(CONFIG_GFAR_PTP)
#ERROR______CONFIG_GFAR_PTP
        /*
         * Check for PTP Timestamping enable flag as well for allocating
         * space for FCB
         */
        if (dev->features & NETIF_F_IP_CSUM  || priv->ptp_timestamping)
#else
	if (dev->features & NETIF_F_IP_CSUM)
#endif
		dev->hard_header_len += GMAC_FCB_LEN;

	priv->tx_ring_size = DEFAULT_TX_RING_SIZE;
	priv->rx_ring_size = DEFAULT_RX_RING_SIZE;

	priv->txcoalescing = DEFAULT_TX_COALESCE;
	priv->txcount = DEFAULT_TXCOUNT;
	priv->txtime = DEFAULT_TXTIME;
	priv->rxcoalescing = DEFAULT_RX_COALESCE;
	priv->rxcount = DEFAULT_RXCOUNT;
	priv->rxtime = DEFAULT_RXTIME;

	/* Enable most messages by default */
	priv->msg_enable = (NETIF_MSG_IFUP << 1 ) - 1;

//#warning ZL - Modify the Timing device MAC address by changing a byte!
//    dev->dev_addr[4] = (&dev & 0xFF);   /* Looking for unique */

	err = register_netdev(dev);

	if (err) {
		printk(KERN_ERR "%s: Cannot register net device, aborting.\n",
				dev->name);
		goto register_fail;
	}

	/* Create all the sysfs files */
	//zl30310_init_sysfs(dev);

	/* Print out the device info */
	printk(KERN_INFO DEVICE_NAME, dev->name);
	for (idx = 0; idx < 6; idx++)
		printk("%2.2x%c", dev->dev_addr[idx], idx == 5 ? ' ' : ':');
	printk("\n");

#ifdef CONFIG_GFAR_SKBUFF_RECYCLING
	priv->rx_skbuff_truesize = GFAR_DEFAULT_RECYCLE_TRUESIZE;
	gfar_reset_skb_handler(&priv->skb_handler);
#endif
	/* Setup MTU, receive buffer size */
	// JVM zl30310_change_mtu(dev, 1522);
	zl30310_change_mtu(dev, 1500);

	/* Even more device info helps when determining which kernel */
	/* provided which set of benchmarks. */
#ifdef CONFIG_GFAR_NAPI
	printk(KERN_INFO "%s: Running with NAPI enabled\n", dev->name);
#else
	printk(KERN_INFO "%s: Running with NAPI disabled\n", dev->name);
#endif
	printk(KERN_INFO "%s: %d/%d RX/TX BD ring size\n",
	       dev->name, priv->rx_ring_size, priv->tx_ring_size);

	return 0;

register_fail:
	iounmap(priv->regs);
regs_fail:
#if defined(CONFIG_GFAR_PTP)
        gfar_ptp_cleanup(priv);
#endif
	free_netdev(dev);
	return err;
}

/***************************************************************
 *  zl30310_remove
 *     remove the ethernet device
 *
 ***************************************************************/
static int zl30310_remove(struct platform_device *pdev)
{
	struct net_device *dev = platform_get_drvdata(pdev);
	struct zl30310_private *priv = netdev_priv(dev);

	platform_set_drvdata(pdev, NULL);

	iounmap(priv->regs);
	free_netdev(dev);

	return 0;
}

#define zl30310_suspend NULL
#define zl30310_resume NULL

//JVM+++

/* Reads the controller's registers to determine what interface
 * connects it to the PHY.
 */
static phy_interface_t zl30310_get_interface(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	u32 ecntrl = zl30310_read(&priv->regs->ecntrl);

	if (ecntrl & ECNTRL_SGMII_MODE)
		return PHY_INTERFACE_MODE_SGMII;

	if (ecntrl & ECNTRL_TBI_MODE) {
		if (ecntrl & ECNTRL_REDUCED_MODE)
			return PHY_INTERFACE_MODE_RTBI;
		else
			return PHY_INTERFACE_MODE_TBI;
	}

	if (ecntrl & ECNTRL_REDUCED_MODE) {
		if (ecntrl & ECNTRL_REDUCED_MII_MODE)
			return PHY_INTERFACE_MODE_RMII;
		else {
			phy_interface_t interface = priv->einfo->interface;

			/*
			 * This isn't autodetected right now, so it must
			 * be set by the device tree or platform code.
			 */
			if (interface == PHY_INTERFACE_MODE_RGMII_ID)
				return PHY_INTERFACE_MODE_RGMII_ID;

			return PHY_INTERFACE_MODE_RGMII;
		}
	}

	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_GIGABIT)
		return PHY_INTERFACE_MODE_GMII;

	return PHY_INTERFACE_MODE_MII;
}


/* Initializes driver's PHY state, and attaches to the PHY.
 * Returns 0 on success.
 */
static int init_phy(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	uint gigabit_support =
		priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_GIGABIT ?
		SUPPORTED_1000baseT_Full : 0;
	struct phy_device *phydev;
	char phy_id[BUS_ID_SIZE];
	phy_interface_t interface;

	priv->oldlink = 0;
	priv->oldspeed = 0;
	priv->oldduplex = -1;

	snprintf(phy_id, BUS_ID_SIZE, PHY_ID_FMT, priv->einfo->bus_id, priv->einfo->phy_id);

	interface = zl30310_get_interface(dev);
	printk("JVM: zl30310 interface type is %d\n", interface);

	phydev = phy_connect(dev, phy_id, &adjust_link, 0, interface);

	if (interface == PHY_INTERFACE_MODE_SGMII) {
#ifdef CONFIG_SGMII_RISING_CARD
#ERROR_CONFIG_SGMII_RISING_CARD
		if (phydev->addr < 0x10)
			phydev->addr += PHY_SGMII_ADDR_OFFSET;
#endif
		zl30310_configure_serdes(dev);
	}

	if (IS_ERR(phydev)) {
		printk(KERN_ERR "%s: Could not attach to PHY\n", dev->name);
		return PTR_ERR(phydev);
	}

	/* Remove any features not supported by the controller */
	phydev->supported &= (GFAR_SUPPORTED | gigabit_support);
	phydev->advertising = phydev->supported;

	priv->phydev = phydev;

	return 0;
}

static void zl30310_configure_serdes(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct gfar_mii __iomem *regs =
			(void __iomem *)&priv->regs->zl30310_mii_regs;

	printk("JVM: gfar_configure_serdes!\n");

	/* Initialise TBI i/f to communicate with serdes (lynx phy) */

	/* Single clk mode, mii mode off(for aerdes communication) */
	gfar_local_mdio_write(regs, TBIPA_VALUE, MII_TBICON, TBICON_CLK_SELECT);

	/* Supported pause and full-duplex, no half-duplex */
	gfar_local_mdio_write(regs, TBIPA_VALUE, MII_ADVERTISE,
			ADVERTISE_1000XFULL | ADVERTISE_1000XPAUSE |
			ADVERTISE_1000XPSE_ASYM);

	/* ANEG enable, restart ANEG, full duplex mode, speed[1] set */
	gfar_local_mdio_write(regs, TBIPA_VALUE, MII_BMCR, BMCR_ANENABLE |
			BMCR_ANRESTART | BMCR_FULLDPLX | BMCR_SPEED1000);
}
//


/***************************************************************
 *  init_registers
 *     initialize the TSEC1 registers
 *
 ***************************************************************/
static void init_registers(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);

	/* Clear IEVENT */
	zl30310_write(&priv->regs->ievent, IEVENT_INIT_CLEAR);

	/* Initialize IMASK */
	zl30310_write(&priv->regs->imask, IMASK_INIT_CLEAR);

	/* Init hash registers to zero */
	zl30310_write(&priv->regs->igaddr0, 0);
	zl30310_write(&priv->regs->igaddr1, 0);
	zl30310_write(&priv->regs->igaddr2, 0);
	zl30310_write(&priv->regs->igaddr3, 0);
	zl30310_write(&priv->regs->igaddr4, 0);
	zl30310_write(&priv->regs->igaddr5, 0);
	zl30310_write(&priv->regs->igaddr6, 0);
	zl30310_write(&priv->regs->igaddr7, 0);

	zl30310_write(&priv->regs->gaddr0, 0);
	zl30310_write(&priv->regs->gaddr1, 0);
	zl30310_write(&priv->regs->gaddr2, 0);
	zl30310_write(&priv->regs->gaddr3, 0);
	zl30310_write(&priv->regs->gaddr4, 0);
	zl30310_write(&priv->regs->gaddr5, 0);
	zl30310_write(&priv->regs->gaddr6, 0);
	zl30310_write(&priv->regs->gaddr7, 0);

	/* Zero out the rmon mib registers if it has them */
	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_RMON) {
		memset_io(&(priv->regs->rmon), 0, sizeof (struct rmon_mib));

		/* Mask off the CAM interrupts */
		zl30310_write(&priv->regs->rmon.cam1, 0xffffffff);
		zl30310_write(&priv->regs->rmon.cam2, 0xffffffff);
	}

	/* Initialize the max receive buffer length */
	zl30310_write(&priv->regs->mrblr, priv->rx_buffer_size);

	/* Initialize the Minimum Frame Length Register */
	zl30310_write(&priv->regs->minflr, MINFLR_INIT_SETTINGS);

	/* Assign the TBI an address which won't conflict with the PHYs */
	zl30310_write(&priv->regs->tbipa, TBIPA_VALUE);
}


/***************************************************************
 *  zl30310_halt
 *     Disable TSEC1 transmit & receive
 *
 ***************************************************************/
void zl30310_halt(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	u32 tempval;

	/* Disable Rx and Tx */
	tempval = zl30310_read(&regs->maccfg1);
	tempval &= ~(MACCFG1_RX_EN | MACCFG1_TX_EN);
	zl30310_write(&regs->maccfg1, tempval);
}

/***************************************************************
 *  stop_zl30310
 *     Stop the ethernet device
 *
 ***************************************************************/
void stop_zl30310(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	unsigned long flags;

	//JVM +
	phy_stop(priv->phydev);

	/* Lock it down */
	spin_lock_irqsave(&priv->txlock, flags);
	spin_lock(&priv->rxlock);

	zl30310_halt(dev);

	spin_unlock(&priv->rxlock);
	spin_unlock_irqrestore(&priv->txlock, flags);

#if defined(CONFIG_GFAR_PTP)
        /* Stop 1588 Timer Module */
        gfar_1588_stop(dev);
#endif

	/* Free the IRQs */
	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_MULTI_INTR) {
		free_irq(priv->interruptError, dev);
		free_irq(priv->interruptTransmit, dev);
		free_irq(priv->interruptReceive, dev);
	} else {
 		free_irq(priv->interruptTransmit, dev);
	}

	free_skb_resources(priv);

	dma_free_coherent(NULL,
			sizeof(struct txbd8)*priv->tx_ring_size
			+ sizeof(struct rxbd8)*priv->rx_ring_size,
			priv->tx_bd_base,
			zl30310_read(&regs->tbase0));
}


/***************************************************************
 *  free_skb_resources
 *     If there are any tx skbs or rx skbs still around, free them.
 *     Then free tx_skbuff and rx_skbuff
 *
 ***************************************************************/
static void free_skb_resources(struct zl30310_private *priv)
{
	struct rxbd8 *rxbdp;
	struct txbd8 *txbdp;
	int i;

	/* Go through all the buffer descriptors and free their data buffers */
	txbdp = priv->tx_bd_base;

	for (i = 0; i < priv->tx_ring_size; i++) {

		if (priv->tx_skbuff[i]) {
			dma_unmap_single(NULL, txbdp->bufPtr,
					txbdp->length,
					DMA_TO_DEVICE);
			dev_kfree_skb_any(priv->tx_skbuff[i]);
			priv->tx_skbuff[i] = NULL;
		}
	}

	kfree(priv->tx_skbuff);

	rxbdp = priv->rx_bd_base;

	/* rx_skbuff is not guaranteed to be allocated, so only
	 * free it and its contents if it is allocated */
	if(priv->rx_skbuff != NULL) {
		for (i = 0; i < priv->rx_ring_size; i++) {
			if (priv->rx_skbuff[i]) {
				dma_unmap_single(NULL, rxbdp->bufPtr,
						priv->rx_buffer_size,
						DMA_FROM_DEVICE);

				dev_kfree_skb_any(priv->rx_skbuff[i]);
				priv->rx_skbuff[i] = NULL;
			}

			rxbdp->status = 0;
			rxbdp->length = 0;
			rxbdp->bufPtr = 0;

			rxbdp++;
		}

		kfree(priv->rx_skbuff);
	}
}

/***************************************************************
 *   zl30310_start
 *     Start the ethernet device
 *
 ***************************************************************/
void zl30310_start(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	u32 tempval;

	/* Enable Rx and Tx in MACCFG1 */
	tempval = zl30310_read(&regs->maccfg1);
	tempval |= (MACCFG1_RX_EN | MACCFG1_TX_EN);
	zl30310_write(&regs->maccfg1, tempval);

	/* Initialize DMACTRL to have WWR and WOP */
	tempval = zl30310_read(&priv->regs->dmactrl);
	tempval |= DMACTRL_INIT_SETTINGS;
	zl30310_write(&priv->regs->dmactrl, tempval);

	/* Make sure we aren't stopped */
	tempval = zl30310_read(&priv->regs->dmactrl);
	tempval &= ~(DMACTRL_GRS | DMACTRL_GTS);
	zl30310_write(&priv->regs->dmactrl, tempval);

	/* Clear THLT/RHLT, so that the DMA starts polling now */
	zl30310_write(&regs->tstat, TSTAT_CLEAR_THALT);
	zl30310_write(&regs->rstat, RSTAT_CLEAR_RHALT);

	/* Unmask the interrupts we look for */
	zl30310_write(&regs->imask, IMASK_DEFAULT);
}

/***************************************************************
 *   startup_zl30310
 *     Bring the controller up and running
 *
 ***************************************************************/
int startup_zl30310(struct net_device *dev)
{
	struct txbd8 *txbdp;
	struct rxbd8 *rxbdp;
	dma_addr_t addr;
	unsigned long vaddr;
	int i;
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	int err = 0, rc=0;
	u32 rctrl = 0;
	u32 attrs = 0;

	zl30310_write(&regs->imask, IMASK_INIT_CLEAR);

	/* Allocate memory for the buffer descriptors */
	vaddr = (unsigned long) dma_alloc_coherent(NULL,
			sizeof (struct txbd8) * priv->tx_ring_size +
			sizeof (struct rxbd8) * priv->rx_ring_size,
			&addr, GFP_KERNEL);

	if (vaddr == 0) {
		if (netif_msg_ifup(priv))
			printk(KERN_ERR "%s: Could not allocate buffer descriptors!\n",
					dev->name);
		return -ENOMEM;
	}

	priv->tx_bd_base = (struct txbd8 *) vaddr;

	/* enet DMA only understands physical addresses */
	zl30310_write(&regs->tbase0, addr);

	/* Start the rx descriptor ring where the tx ring leaves off */
	addr = addr + sizeof (struct txbd8) * priv->tx_ring_size;
	vaddr = vaddr + sizeof (struct txbd8) * priv->tx_ring_size;
	priv->rx_bd_base = (struct rxbd8 *) vaddr;
	zl30310_write(&regs->rbase0, addr);

	/* Setup the skbuff rings */
	priv->tx_skbuff =
	    (struct sk_buff **) kmalloc(sizeof (struct sk_buff *) *
					priv->tx_ring_size, GFP_KERNEL);

	if (NULL == priv->tx_skbuff) {
		if (netif_msg_ifup(priv))
			printk(KERN_ERR "%s: Could not allocate tx_skbuff\n",
					dev->name);
		err = -ENOMEM;
		goto tx_skb_fail;
	}

	for (i = 0; i < priv->tx_ring_size; i++)
		priv->tx_skbuff[i] = NULL;

	priv->rx_skbuff =
	    (struct sk_buff **) kmalloc(sizeof (struct sk_buff *) *
					priv->rx_ring_size, GFP_KERNEL);

	if (NULL == priv->rx_skbuff) {
		if (netif_msg_ifup(priv))
			printk(KERN_ERR "%s: Could not allocate rx_skbuff\n",
					dev->name);
		err = -ENOMEM;
		goto rx_skb_fail;
	}

	for (i = 0; i < priv->rx_ring_size; i++)
		priv->rx_skbuff[i] = NULL;

	/* Initialize some variables in our dev structure */
	priv->dirty_tx = priv->cur_tx = priv->tx_bd_base;
	priv->cur_rx = priv->rx_bd_base;
	priv->skb_curtx = priv->skb_dirtytx = 0;
	priv->skb_currx = 0;

	/* Initialize Transmit Descriptor Ring */
	txbdp = priv->tx_bd_base;
	for (i = 0; i < priv->tx_ring_size; i++) {
		txbdp->status = 0;
		txbdp->length = 0;
		txbdp->bufPtr = 0;
		txbdp++;
	}

	/* Set the last descriptor in the ring to indicate wrap */
	txbdp--;
	txbdp->status |= TXBD_WRAP;

	rxbdp = priv->rx_bd_base;
	for (i = 0; i < priv->rx_ring_size; i++) {
		struct sk_buff *skb = NULL;

		rxbdp->status = 0;

		skb = zl30310_new_skb(dev, rxbdp);

		priv->rx_skbuff[i] = skb;

		rxbdp++;
	}

	/* Set the last descriptor in the ring to wrap */
	rxbdp--;
	rxbdp->status |= RXBD_WRAP;

	/* If the device has multiple interrupts, register for
	 * them.  Otherwise, only register for the one */
	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_MULTI_INTR) {
		/* Install our interrupt handlers for Error,
		 * Transmit, and Receive */

		if ((rc=request_irq(priv->interruptError, zl30310_error, 0, "eth1_error", dev)) < 0) {
			printk(KERN_ERR "%s: Can't get ERR IRQ %d, rc=%x\n", dev->name, priv->interruptError, rc);

			err = -1;
			goto err_irq_fail;
		}

		if (request_irq(priv->interruptTransmit, zl30310_transmit, 0, "eth1_tx", dev) < 0) {
			printk(KERN_ERR "%s: Can't get TX IRQ %d\n", dev->name, priv->interruptTransmit);
			err = -1;
			goto tx_irq_fail;
		}

		if (request_irq(priv->interruptReceive, zl30310_receive, 0, "eth1_rx", dev) < 0) {
			printk(KERN_ERR "%s: Can't get RX IRQ %d\n", dev->name, priv->interruptReceive);
			err = -1;
			goto rx_irq_fail;
		}
	} else {
		if (request_irq(priv->interruptTransmit, zl30310_interrupt,
				0, "zl30310_interrupt", dev) < 0) {
			if (netif_msg_intr(priv))
				printk(KERN_ERR "%s: Can't get IRQ %d\n",
					dev->name, priv->interruptError);

			err = -1;
			goto err_irq_fail;
		}
	}

	//JVM+
	phy_start(priv->phydev);

	/* Configure the coalescing support */
	if (priv->txcoalescing)
		zl30310_write(&regs->txic,
			   mk_ic_value(priv->txcount, priv->txtime));
	else
		zl30310_write(&regs->txic, 0);

	if (priv->rxcoalescing)
		zl30310_write(&regs->rxic,
			   mk_ic_value(priv->rxcount, priv->rxtime));
	else
		zl30310_write(&regs->rxic, 0);

/*
	if (priv->lfc_enable) {
		// Clear the LFC bit
		zl30310_write(&regs->rctrl, rctrl);

		/ Set the Receiv Queue Parameter Register0
		zl30310_write(&regs->rqprm[0], priv->free_bd_thres << 24 |
				priv->rx_ring_size);
		rctrl = RCTRL_LFC;
	}
*/

	if (priv->rx_csum_enable)
		rctrl |= RCTRL_CHECKSUMMING;

	if (priv->extended_hash) {
		rctrl |= RCTRL_EXTHASH;

		zl30310_clear_exact_match(dev);
		rctrl |= RCTRL_EMEN;
	}

	if (priv->vlan_enable)
		rctrl |= RCTRL_VLAN;

	if (priv->padding) {
		rctrl &= ~RCTRL_PAL_MASK;
		rctrl |= RCTRL_PADDING(priv->padding);
	}

#if defined(CONFIG_GFAR_PTP)
#ERROR______CONFIG_GFAR_PTP
        /* Enable Filer and Rx Packet Parsing capability of eTSEC */
        /* Set Filer Table */
        gfar_1588_start(dev);

        /* Enable Filer for Rx Queue */
        rctrl |= RCTRL_PRSDEP_INIT |
                RCTRL_TS_ENABLE | RCTRL_PADB_SIZE | RCTRL_FSQEN;
#endif

	/* Init rctrl based on our settings */
	zl30310_write(&priv->regs->rctrl, rctrl);

	if (dev->features & NETIF_F_IP_CSUM)
		zl30310_write(&priv->regs->tctrl, TCTRL_INIT_CSUM);

	/* Set the extraction length and index */
	attrs = ATTRELI_EL(priv->rx_stash_size) |
		ATTRELI_EI(priv->rx_stash_index);

	zl30310_write(&priv->regs->attreli, attrs);

	/* Start with defaults, and add stashing or locking
	 * depending on the approprate variables */
	attrs = ATTR_INIT_SETTINGS;

	if (priv->bd_stash_en)
		attrs |= ATTR_BDSTASH;

	if (priv->rx_stash_size != 0)
		attrs |= ATTR_BUFSTASH;

	zl30310_write(&priv->regs->attr, attrs);

	zl30310_write(&priv->regs->fifo_tx_thr, priv->fifo_threshold);
	zl30310_write(&priv->regs->fifo_tx_starve, priv->fifo_starve);
	zl30310_write(&priv->regs->fifo_tx_starve_shutoff, priv->fifo_starve_off);

	/* Start the controller */
	zl30310_start(dev);

	return 0;

rx_irq_fail:
	free_irq(priv->interruptTransmit, dev);
tx_irq_fail:
	free_irq(priv->interruptError, dev);
err_irq_fail:
rx_skb_fail:
	free_skb_resources(priv);
tx_skb_fail:
	dma_free_coherent(NULL,
			sizeof(struct txbd8)*priv->tx_ring_size
			+ sizeof(struct rxbd8)*priv->rx_ring_size,
			priv->tx_bd_base,
			zl30310_read(&regs->tbase0));

	return err;
}

/***************************************************************
 *  zl30310_enet_open
 *    open the device. Returns 0 for success
 *
 ***************************************************************/
static int zl30310_enet_open(struct net_device *dev)
{
	int err;

	/* Initialize a bunch of registers */
	init_registers(dev);

	zl30310_set_mac_address(dev);

	err = init_phy(dev);

	if(err)
		return err;

	err = startup_zl30310(dev);

	netif_start_queue(dev);

	return err;
}

/***************************************************************
 *  zl30310_add_fcb
 *    adjust the header for the GMAC
 *
 ***************************************************************/
static inline struct txfcb *zl30310_add_fcb(struct sk_buff *skb, struct txbd8 *bdp)
{
	struct txfcb *fcb = (struct txfcb *)skb_push (skb, GMAC_FCB_LEN);

	memset(fcb, 0, GMAC_FCB_LEN);

	return fcb;
}

/***************************************************************
 *  zl30310_tx_checksum
 *
 *
 ***************************************************************/
static inline void zl30310_tx_checksum(struct sk_buff *skb, struct txfcb *fcb)
{
	u8 flags = 0;

	/* If we're here, it's a IP packet with a TCP or UDP
	 * payload.  We set it to checksum, using a pseudo-header
	 * we provide
	 */
	flags = TXFCB_DEFAULT;

	/* Tell the controller what the protocol is */
	/* And provide the already calculated phcs */
	if (ip_hdr(skb)->protocol == IPPROTO_UDP) {
		flags |= TXFCB_UDP;
		fcb->phcs = udp_hdr(skb)->check;
	} else
		fcb->phcs = tcp_hdr(skb)->check;

	/* l3os is the distance between the start of the
	 * frame (skb->data) and the start of the IP hdr.
	 * l4os is the distance between the start of the
	 * l3 hdr and the l4 hdr */
	fcb->l3os = (u16)(skb_network_offset(skb) - GMAC_FCB_LEN);
	fcb->l4os = skb_network_header_len(skb);

	fcb->flags = flags;
}

/***************************************************************
 *  zl30310_tx_vlan
 *
 *
 ***************************************************************/
void inline zl30310_tx_vlan(struct sk_buff *skb, struct txfcb *fcb)
{
	fcb->flags |= TXFCB_VLN;
	fcb->vlctl = vlan_tx_tag_get(skb);
}

/***************************************************************
 *  zl30310_start_xmit
 *    This is called by the kernel when a frame is ready for transmission.
 *    It is pointed to by the dev->hard_start_xmit function pointer
 *
 ***************************************************************/
static int zl30310_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct txfcb *fcb = NULL;
	struct txbd8 *txbdp;
	u16 status;
	unsigned long flags;

	/* Update transmit stats */
	priv->stats.tx_bytes += skb->len;

	/* Lock priv now */
	spin_lock_irqsave(&priv->txlock, flags);

	/* Point at the first free tx descriptor */
	txbdp = priv->cur_tx;

	/* Clear all but the WRAP status flags */
	status = txbdp->status & TXBD_WRAP;

	/* Set up checksumming */
	if (likely((dev->features & NETIF_F_IP_CSUM)
			&& (CHECKSUM_PARTIAL == skb->ip_summed))) {
		fcb = zl30310_add_fcb(skb, txbdp);
		status |= TXBD_TOE;
		zl30310_tx_checksum(skb, fcb);
	}

	if (priv->vlan_enable &&
			unlikely(priv->vlgrp && vlan_tx_tag_present(skb))) {
		if (unlikely(NULL == fcb)) {
			fcb = zl30310_add_fcb(skb, txbdp);
			status |= TXBD_TOE;
		}

		zl30310_tx_vlan(skb, fcb);
	}

#if defined(CONFIG_GFAR_PTP)
        /* Enable ptp flag so that Tx time stamping happens */
        if (gfar_ptp_do_txstamp(skb)) {
                if (fcb == NULL)
                        fcb = gfar_add_fcb(skb, txbdp);
                fcb->ptp = 0x01;
                status |= TXBD_TOE;
        }
#endif

	/* Set buffer length and pointer */
	txbdp->length = skb->len;
	txbdp->bufPtr = dma_map_single(NULL, skb->data,
			skb->len, DMA_TO_DEVICE);

	/* Save the skb pointer so we can free it later */
	priv->tx_skbuff[priv->skb_curtx] = skb;

	/* Update the current skb pointer (wrapping if this was the last) */
	priv->skb_curtx =
	    (priv->skb_curtx + 1) & TX_RING_MOD_MASK(priv->tx_ring_size);

	/* Flag the BD as interrupt-causing */
	status |= TXBD_INTERRUPT;

	/* Flag the BD as ready to go, last in frame, and  */
	/* in need of CRC */
	status |= (TXBD_READY | TXBD_LAST | TXBD_CRC);

	dev->trans_start = jiffies;

	/* The powerpc-specific eieio() is used, as wmb() has too strong
	 * semantics (it requires synchronization between cacheable and
	 * uncacheable mappings, which eieio doesn't provide and which we
	 * don't need), thus requiring a more expensive sync instruction.  At
	 * some point, the set of architecture-independent barrier functions
	 * should be expanded to include weaker barriers.
	 */

	eieio();
	txbdp->status = status;

	/* If this was the last BD in the ring, the next one */
	/* is at the beginning of the ring */
	if (txbdp->status & TXBD_WRAP)
		txbdp = priv->tx_bd_base;
	else
		txbdp++;

	/* If the next BD still needs to be cleaned up, then the bds
	   are full.  We need to tell the kernel to stop sending us stuff. */
	if (txbdp == priv->dirty_tx) {
		netif_stop_queue(dev);

		priv->stats.tx_fifo_errors++;
	}

	/* Update the current txbd to the next one */
	priv->cur_tx = txbdp;

	/* Tell the DMA to go go go */
	zl30310_write(&priv->regs->tstat, TSTAT_CLEAR_THALT);

	/* Unlock priv */
	spin_unlock_irqrestore(&priv->txlock, flags);

	return 0;
}

/***************************************************************
 *  zl30310_close
 *    Stops the kernel queue, and halts the controller
 *
 ***************************************************************/
static int zl30310_close(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	stop_zl30310(dev);

	//JVM+++
	/* Disconnect from the PHY */
	phy_disconnect(priv->phydev);
	priv->phydev = NULL;

	netif_stop_queue(dev);

	return 0;
}

/***************************************************************
 *  zl30310_get_stats
 *     returns net_device_stats structure pointer
 *
 ***************************************************************/
static struct net_device_stats * zl30310_get_stats(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);

	return &(priv->stats);
}

/***************************************************************
 *  zl30310_set_mac_address
 *     Changes the mac address if the controller is not running
 *
 ***************************************************************/
int zl30310_set_mac_address(struct net_device *dev)
{
	zl30310_set_mac_for_addr(dev, 0, dev->dev_addr);

	return 0;
}


/***************************************************************
 *  zl30310_vlan_rx_register
 *     Enables and disables VLAN insertion/extraction
 *
 ***************************************************************/
static void zl30310_vlan_rx_register(struct net_device *dev,
		struct vlan_group *grp)
{
	struct zl30310_private *priv = netdev_priv(dev);
	unsigned long flags;
	u32 tempval;

	spin_lock_irqsave(&priv->rxlock, flags);

	priv->vlgrp = grp;

	if (grp) {
		/* Enable VLAN tag insertion */
		tempval = zl30310_read(&priv->regs->tctrl);
		tempval |= TCTRL_VLINS;

		zl30310_write(&priv->regs->tctrl, tempval);

		/* Enable VLAN tag extraction */
		tempval = zl30310_read(&priv->regs->rctrl);
		tempval |= RCTRL_VLEX;
		zl30310_write(&priv->regs->rctrl, tempval);
	} else {
		/* Disable VLAN tag insertion */
		tempval = zl30310_read(&priv->regs->tctrl);
		tempval &= ~TCTRL_VLINS;
		zl30310_write(&priv->regs->tctrl, tempval);

		/* Disable VLAN tag extraction */
		tempval = zl30310_read(&priv->regs->rctrl);
		tempval &= ~RCTRL_VLEX;
		zl30310_write(&priv->regs->rctrl, tempval);
	}

	spin_unlock_irqrestore(&priv->rxlock, flags);
}

/***************************************************************
 *  zl30310_change_mtu
 *
 *
 ***************************************************************/
static int zl30310_change_mtu(struct net_device *dev, int new_mtu)
{
	int tempsize, tempval;
	struct zl30310_private *priv = netdev_priv(dev);
	int oldsize = priv->rx_buffer_size;
	int frame_size = new_mtu + ETH_HLEN;

	if (priv->vlan_enable)
		frame_size += VLAN_HLEN;

	if (zl30310_uses_fcb(priv))
		frame_size += GMAC_FCB_LEN;

	frame_size += priv->padding;

	if ((frame_size < 64) || (frame_size > JUMBO_FRAME_SIZE)) {
		if (netif_msg_drv(priv))
			printk(KERN_ERR "%s: Invalid MTU setting\n",
					dev->name);
		return -EINVAL;
	}

	tempsize =
	    (frame_size & ~(INCREMENTAL_BUFFER_SIZE - 1)) +
	    INCREMENTAL_BUFFER_SIZE;

	/* Only stop and start the controller if it isn't already
	 * stopped, and we changed something */
	if ((oldsize != tempsize) && (dev->flags & IFF_UP))
		stop_zl30310(dev);

	priv->rx_buffer_size = tempsize;

	dev->mtu = new_mtu;

	zl30310_write(&priv->regs->mrblr, priv->rx_buffer_size);
	zl30310_write(&priv->regs->maxfrm, priv->rx_buffer_size);

	/* If the mtu is larger than the max size for standard
	 * ethernet frames (ie, a jumbo frame), then set maccfg2
	 * to allow huge frames, and to check the length */
	tempval = zl30310_read(&priv->regs->maccfg2);

//JVM - somehow this is blocking the CPU!
	if (priv->rx_buffer_size > DEFAULT_RX_BUFFER_SIZE)
		tempval |= (MACCFG2_HUGEFRAME | MACCFG2_LENGTHCHECK);
	else
		tempval &= ~(MACCFG2_HUGEFRAME | MACCFG2_LENGTHCHECK);

	zl30310_write(&priv->regs->maccfg2, tempval);

	if ((oldsize != tempsize) && (dev->flags & IFF_UP))
		startup_zl30310(dev);

	return 0;
}

/***************************************************************
 *  zl30310_timeout
 *    zl30310_timeout gets called when a packet has not been
 *    transmitted after a set amount of time.
 *    For now, assume that clearing out all the structures, and
 *    starting over will fix the problem.
 *
 ***************************************************************/
static void zl30310_timeout(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);

	priv->stats.tx_errors++;

	if (dev->flags & IFF_UP) {
		stop_zl30310(dev);
		startup_zl30310(dev);
	}

	netif_tx_schedule_all(dev);
	//netif_schedule(dev);  //JVM
}

/***************************************************************
 *  zl30310_clean_tx_ring
 *     Transmit ring clean up routine
 *
 ***************************************************************/
int zl30310_clean_tx_ring(struct net_device *dev)
{
	struct txbd8 *bdp;
	struct zl30310_private *priv = netdev_priv(dev);
	int howmany = 0;

	bdp = priv->dirty_tx;
	while ((bdp->status & TXBD_READY) == 0) {
		/* If dirty_tx and cur_tx are the same, then either the */
		/* ring is empty or full now (it could only be full in the beginning, */
		/* obviously).  If it is empty, we are done. */
		if ((bdp == priv->cur_tx) && (netif_queue_stopped(dev) == 0))
			break;

		howmany++;

		/* Deferred means some collisions occurred during transmit, */
		/* but we eventually sent the packet. */
		if (bdp->status & TXBD_DEF)
			priv->stats.collisions++;

		/* Free the sk buffer associated with this TxBD */
		dev_kfree_skb_irq(priv->tx_skbuff[priv->skb_dirtytx]);
		priv->tx_skbuff[priv->skb_dirtytx] = NULL;
		priv->skb_dirtytx =
		    (priv->skb_dirtytx +
		     1) & TX_RING_MOD_MASK(priv->tx_ring_size);

		/* Clean BD length for empty detection */
		bdp->length = 0;

		/* update bdp to point at next bd in the ring (wrapping if necessary) */
		if (bdp->status & TXBD_WRAP)
			bdp = priv->tx_bd_base;
		else
			bdp++;

		/* Move dirty_tx to be the next bd */
		priv->dirty_tx = bdp;

		/* We freed a buffer, so now we can restart transmission */
		if (netif_queue_stopped(dev))
			netif_wake_queue(dev);
	} /* while ((bdp->status & TXBD_READY) == 0) */

	priv->stats.tx_packets += howmany;

	return howmany;
}

/***************************************************************
 *  zl30310_transmit
 *     Interrupt Handler for Transmit complete
 *
 ***************************************************************/
static irqreturn_t zl30310_transmit(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *) dev_id;
	struct zl30310_private *priv = netdev_priv(dev);

	/* Clear IEVENT */
	zl30310_write(&priv->regs->ievent, IEVENT_TX_MASK);

	/* Lock priv */
	spin_lock(&priv->txlock);

	zl30310_clean_tx_ring(dev);

	/* If we are coalescing the interrupts, reset the timer */
	/* Otherwise, clear it */
	if (likely(priv->txcoalescing)) {
		zl30310_write(&priv->regs->txic, 0);
		zl30310_write(&priv->regs->txic,
			   mk_ic_value(priv->txcount, priv->txtime));
	}

	spin_unlock(&priv->txlock);

	return IRQ_HANDLED;
}

/***************************************************************
 *  zl30310_new_skb
 *     normal new skb routine
 *
 ***************************************************************/
struct sk_buff * zl30310_new_skb(struct net_device *dev, struct rxbd8 *bdp)
{
	unsigned int alignamount;
	struct zl30310_private *priv = netdev_priv(dev);
	struct sk_buff *skb = NULL;
	unsigned int timeout = SKB_ALLOC_TIMEOUT;

	/* We have to allocate the skb, so keep trying till we succeed */
	while ((!skb) && timeout--)
		skb = dev_alloc_skb(priv->rx_buffer_size + RXBUF_ALIGNMENT);

	if (NULL == skb)
		return NULL;

	alignamount = RXBUF_ALIGNMENT -
		(((unsigned) skb->data) & (RXBUF_ALIGNMENT - 1));

	/* We need the data buffer to be aligned properly.  We will reserve
	 * as many bytes as needed to align the data properly
	 */
	skb_reserve(skb, alignamount);

	bdp->bufPtr = dma_map_single(NULL, skb->data,
			priv->rx_buffer_size, DMA_FROM_DEVICE);

	bdp->length = 0;

	/* Mark the buffer empty */
	eieio();
	bdp->status |= (RXBD_EMPTY | RXBD_INTERRUPT);

	return skb;
}

/***************************************************************
 *  count_errors
 *
 *
 ***************************************************************/
static inline void count_errors(unsigned short status, struct zl30310_private *priv)
{
	struct net_device_stats *stats = &priv->stats;
	struct zl30310_extra_stats *estats = &priv->extra_stats;

	/* If the packet was truncated, none of the other errors
	 * matter */
	if (status & RXBD_TRUNCATED) {
		stats->rx_length_errors++;

		estats->rx_trunc++;

		return;
	}
	/* Count the errors, if there were any */
	if (status & (RXBD_LARGE | RXBD_SHORT)) {
		stats->rx_length_errors++;

		if (status & RXBD_LARGE)
			estats->rx_large++;
		else
			estats->rx_short++;
	}
	if (status & RXBD_NONOCTET) {
		stats->rx_frame_errors++;
		estats->rx_nonoctet++;
	}
	if (status & RXBD_CRCERR) {
		estats->rx_crcerr++;
		stats->rx_crc_errors++;
	}
	if (status & RXBD_OVERRUN) {
		estats->rx_overrun++;
		stats->rx_crc_errors++;
	}
}

/***************************************************************
 *  zl30310_receive
 *
 *
 ***************************************************************/
irqreturn_t zl30310_receive(int irq, void *dev_id)
{
	struct net_device *dev = (struct net_device *) dev_id;
	struct zl30310_private *priv = netdev_priv(dev);
#ifdef CONFIG_GFAR_NAPI
	u32 tempval;
#else
	unsigned long flags;
#endif

	/* support NAPI */
#ifdef CONFIG_GFAR_NAPI
	/* Clear IEVENT, so rx interrupt isn't called again
	 * because of this interrupt */
	zl30310_write(&priv->regs->ievent, IEVENT_RTX_MASK);

	if (netif_rx_schedule_prep(dev)) {
		tempval = zl30310_read(&priv->regs->imask);
		tempval &= IMASK_RTX_DISABLED;
		zl30310_write(&priv->regs->imask, tempval);

		__netif_rx_schedule(dev);
	} else {
		if (netif_msg_rx_err(priv))
			printk(KERN_DEBUG "%s: receive called twice (%x)[%x]\n",
				dev->name, zl30310_read(&priv->regs->ievent),
				zl30310_read(&priv->regs->imask));
	}
#else
	/* Clear IEVENT, so rx interrupt isn't called again
	 * because of this interrupt */
	zl30310_write(&priv->regs->ievent, IEVENT_RX_MASK);

	spin_lock_irqsave(&priv->rxlock, flags);
	zl30310_clean_rx_ring(dev, priv->rx_ring_size);

	/* If we are coalescing interrupts, update the timer */
	/* Otherwise, clear it */
	if (likely(priv->rxcoalescing)) {
		zl30310_write(&priv->regs->rxic, 0);
		zl30310_write(&priv->regs->rxic,
			   mk_ic_value(priv->rxcount, priv->rxtime));
	}

	spin_unlock_irqrestore(&priv->rxlock, flags);
#endif

	return IRQ_HANDLED;
}

/***************************************************************
 *  zl30310_rx_vlan
 *
 *
 ***************************************************************/
static inline int zl30310_rx_vlan(struct sk_buff *skb,
		struct vlan_group *vlgrp, unsigned short vlctl)
{
#ifdef CONFIG_GFAR_NAPI
	return vlan_hwaccel_receive_skb(skb, vlgrp, vlctl);
#else
	return vlan_hwaccel_rx(skb, vlgrp, vlctl);
#endif
}

/***************************************************************
 *  zl30310_rx_checksum
 *
 *
 ***************************************************************/
static inline void zl30310_rx_checksum(struct sk_buff *skb, struct rxfcb *fcb)
{
	/* If valid headers were found, and valid sums
	 * were verified, then we tell the kernel that no
	 * checksumming is necessary.  Otherwise, it is */
	if ((fcb->flags & RXFCB_CSUM_MASK) == (RXFCB_CIP | RXFCB_CTU))
		skb->ip_summed = CHECKSUM_UNNECESSARY;
	else
		skb->ip_summed = CHECKSUM_NONE;
}

/***************************************************************
 *   zl30310_get_fcb
 *     remove the GMAC header
 *
 ***************************************************************/
static inline struct rxfcb *zl30310_get_fcb(struct sk_buff *skb)
{
	struct rxfcb *fcb = (struct rxfcb *)skb->data;

	/* Remove the FCB from the skb */
	skb_pull(skb, GMAC_FCB_LEN);

	return fcb;
}

/***************************************************************
 *   zl30310_process_frame
 *     handle one incoming packet if skb isn't NULL.
 *
 ***************************************************************/
static int zl30310_process_frame(struct net_device *dev, struct sk_buff *skb, int length)
{
	struct zl30310_private *priv = netdev_priv(dev);
       	struct rxfcb *fcb = NULL;

	if (NULL == skb) {
		if (netif_msg_rx_err(priv))
			printk(KERN_WARNING "%s: Missing skb!!.\n", dev->name);
		priv->stats.rx_dropped++;
		priv->extra_stats.rx_skbmissing++;
	} else {
		int ret;
#ifdef ZL_CHAR_DEV
		int pktHookRule = -1;
#endif

		/* Prep the skb for the packet */
		skb_put(skb, length);

		/* Grab the FCB if there is one */
		if (zl30310_uses_fcb(priv))
			fcb = zl30310_get_fcb(skb);

#if defined(CONFIG_GFAR_PTP)
                if (fcb->rq == 0)
                gfar_ptp_store_rxstamp(dev, skb);
                skb_pull(skb, 8);
#endif

		/* Remove the padded bytes, if there are any */
		if (priv->padding)
			skb_pull(skb, priv->padding);

#ifdef ZL_CHAR_DEV

		skb->dev = dev;
		skb_reset_mac_header(skb);

        if (filterActiveBits)  /* FPE filters enabled */
        {
            int fltIndx;
            u_int64_t moreActiveBits = filterActiveBits;    /* Start with all configured filters */

            for (fltIndx = 0; fltIndx < 63 && moreActiveBits; fltIndx++)    /* 64 active bits in a u_int64_t */
            {
                /* Active filter bit set && packet data at the offset matches the pattern's entire length */
                if ((moreActiveBits & ((u_int64_t)1 << fltIndx)) &&
                    (filterTable[fltIndx][FPE_O] + filterTable[fltIndx][FPE_L] <= length) &&
                    !memcmp(&skb->data[(int)filterTable[fltIndx][FPE_O]], &filterTable[fltIndx][FPE_P], (int)filterTable[fltIndx][FPE_L] ))
                {
                    pktHookRule = fltIndx;
                    #ifdef MORE_DEBUG
                        printk("moreActiveBits=0x %x:%x matched @fltIndx=%d \n",
                               (unsigned int)((moreActiveBits>>16)>>16), (unsigned int)(moreActiveBits&0xFFFFFFFF), fltIndx);
                    #endif
                    break;  /* done */
                }

                moreActiveBits &= ~((u_int64_t)1 << fltIndx); /* Clear each bit as we go to exit the loop fast! */
            }
        }

		if (pktHookRule >= 0)   /* Pkt match */
		{
			#ifdef DEBUG
			printk("pktHookRule=%d\n", pktHookRule);
			#endif

			// check for an active listener
            if (devInUse)
			{
				unsigned short currPktLen = skb->len + PKT_PREAMBLE_LEN;

                if (busyBufferBits & ((u_int64_t)1 << currRxIndx)) /* buffer busy? */
                {
                    static int unprocessed = 0;

                    if ( !(++unprocessed % (RX_PKT_Q_SIZE*3)))
                    {
                        printk("More unprocessed msgs=%d. Pool size:%d,\n", unprocessed, RX_PKT_Q_SIZE);
                        unprocessed = 0;
                    }
                    priv->extra_stats.kernel_dropped++;
                    dev_kfree_skb_any(skb);
                    return 0;
                }

				#ifdef USE_PKT_PREAMBLE
				/* Let user-space know which rule (0-64) matched */
				rx_kbuf[currRxIndx][0] = (unsigned char)pktHookRule;
				#endif

				/* Limit copy - no malformed/jumbos to user space */
				if (currPktLen > MAX_PKT_LEN_TO_USER + PKT_PREAMBLE_LEN)
				{
					/* TS is lost! Tell someone */
					currPktLen = MAX_PKT_LEN_TO_USER + PKT_PREAMBLE_LEN;
					printk("Rx Pkt Length=%d > PKT_PREAMBLE_LEN+MAX_PKT_LEN_TO_USER=%d", skb->len, currPktLen);
				}

				/* Length of current buffer */
				pktLengthToUser[currRxIndx] = currPktLen;
				/* Copy the pkt into buffer */
				memcpy(&rx_kbuf[currRxIndx][START_OF_PAYLOAD], skb->data, currPktLen);
                busyBufferBits |= ((u_int64_t)1 << currRxIndx);     /* set this busy bit */
//              printk(" +%x; %d  ", (long int)busyBufferBits&0xffffffff, currRxIndx);
				#ifdef MORE_DEBUG
				{
					int i;

					printk("Hook=%d Rx TS Pkt; Length=0x%x: \n",
						pktHookRule, currPktLen);
					for(i=0; i<currPktLen; i++)
					printk("%2x ", rx_kbuf[currRxIndx][i]);
					printk("\n");
				}
				#endif

                currRxIndx = (++currRxIndx % RX_PKT_Q_SIZE);    /* wrap if needed */

				if (waitqueue_active(&qTsPktWait))
                {
                    /* This will unblock any waiting read() on /dev/zlTsPkts1 */
                    wake_up_interruptible(&qTsPktWait);
                }
			}
			else
			{
				/* Match configured but no listener - Drop this pkt */
				#ifdef DEBUG
				    printk("No active listener for TS Pkt; Dropped Pkt! ");
				#endif
			}

			// release socket buffer
			dev_kfree_skb_any(skb);
			return 0;
		}

#endif
		if (priv->rx_csum_enable)
			zl30310_rx_checksum(skb, fcb);

		/* Tell the skb what kind of packet this is */
		skb->protocol = eth_type_trans(skb, dev);

		/* Send the packet up the stack */
		if (unlikely(priv->vlgrp && (fcb->flags & RXFCB_VLN)))
			ret = zl30310_rx_vlan(skb, priv->vlgrp, fcb->vlctl);
		else
			ret = RECEIVE(skb);

		if (NET_RX_DROP == ret)
			priv->extra_stats.kernel_dropped++;
	}

	return 0;
}

/***************************************************************
 *   zl30310_clean_rx_ring
 *     Processes each frame in the rx ring until the budget/quota
 *     has been reached. Returns the number of frames handled
 *
 ***************************************************************/
int zl30310_clean_rx_ring(struct net_device *dev, int rx_work_limit)
{
	struct rxbd8 *bdp;
	struct sk_buff *skb;
	u16 pkt_len;
	int howmany = 0;
	struct zl30310_private *priv = netdev_priv(dev);

	/* Get the first full descriptor */
	bdp = priv->cur_rx;

	while (!((bdp->status & RXBD_EMPTY) || (--rx_work_limit < 0))) {
		rmb();
		skb = priv->rx_skbuff[priv->skb_currx];

		if (!(bdp->status &
		      (RXBD_LARGE | RXBD_SHORT | RXBD_NONOCTET
		       | RXBD_CRCERR | RXBD_OVERRUN | RXBD_TRUNCATED))
		    && (bdp->status & RXBD_LAST)) {
			/* Increment the number of packets */
			priv->stats.rx_packets++;
			howmany++;

			/* Remove the FCS from the packet length */
			pkt_len = bdp->length - 4;

			zl30310_process_frame(dev, skb, pkt_len);

			priv->stats.rx_bytes += pkt_len;
		} else {
			count_errors(bdp->status, priv);

			if (skb)
				dev_kfree_skb_any(skb);

			priv->rx_skbuff[priv->skb_currx] = NULL;
		}

		dev->last_rx = jiffies;

		/* Clear the status flags for this buffer */
		bdp->status &= ~RXBD_STATS;

		/* Add another skb for the future */
		skb = zl30310_new_skb(dev, bdp);
		priv->rx_skbuff[priv->skb_currx] = skb;

		/* Update Last Free TxBD pointer for LFC */
		if (priv->lfc_enable) {
			printk("JVM 5");
			/* write to the high half for 32bit system */
			zl30310_write((u32 __iomem *)&priv->regs->rfbptr[0] + 1,
					(u32)bdp);
		}

		/* Update to the next pointer */
		if (bdp->status & RXBD_WRAP)
			bdp = priv->rx_bd_base;
		else
			bdp++;

		/* update to point at the next skb */
		priv->skb_currx =
		    (priv->skb_currx +
		     1) & RX_RING_MOD_MASK(priv->rx_ring_size);

	}

	/* Update the current rxbd pointer to be the next one */
	priv->cur_rx = bdp;

	return howmany;
}

#ifdef CONFIG_GFAR_NAPI
/***************************************************************
 *   zl30310_poll
 *
 *
 ***************************************************************/
static int zl30310_poll(struct net_device *dev, int *budget)
{
	int howmany;
	struct zl30310_private *priv = netdev_priv(dev);
	int rx_work_limit = *budget;
	unsigned long int flags;

	if (rx_work_limit > dev->quota)
		rx_work_limit = dev->quota;

	howmany = zl30310_clean_rx_ring(dev, rx_work_limit);

	dev->quota -= howmany;
	rx_work_limit -= howmany;
	*budget -= howmany;

	/* Lock priv */
	spin_lock_irqsave(&priv->txlock, flags);
	zl30310_clean_tx_ring(dev);
	spin_unlock_irqrestore(&priv->txlock, flags);

	if (rx_work_limit > 0) {
		netif_rx_complete(dev);

		/* Clear the halt bit in RSTAT */
		zl30310_write(&priv->regs->rstat, RSTAT_CLEAR_RHALT);

		zl30310_write(&priv->regs->imask, IMASK_DEFAULT);

		/* If we are coalescing interrupts, update the timer */
		/* Otherwise, clear it */
		if (likely(priv->rxcoalescing)) {
			zl30310_write(&priv->regs->rxic, 0);
			zl30310_write(&priv->regs->rxic,
				   mk_ic_value(priv->rxcount, priv->rxtime));
		}
	}

	/* Return 1 if there's more work to do */
	return (rx_work_limit > 0) ? 0 : 1;
}
#endif

#ifdef CONFIG_NET_POLL_CONTROLLER
/***************************************************************
 *   zl30310_netpoll
 *     Polling 'interrupt' - used by things like netconsole to send skbs
 *     without having to re-enable interrupts. It's not called while
 *     the interrupt routine is executing.
 *
 ***************************************************************/
static void zl30310_netpoll(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);

	/* If the device has multiple interrupts, run tx/rx */
	if (priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_MULTI_INTR) {
		disable_irq(priv->interruptTransmit);
		disable_irq(priv->interruptReceive);
		disable_irq(priv->interruptError);
		zl30310_interrupt(priv->interruptTransmit, dev);
		enable_irq(priv->interruptError);
		enable_irq(priv->interruptReceive);
		enable_irq(priv->interruptTransmit);
	} else {
		disable_irq(priv->interruptTransmit);
		zl30310_interrupt(priv->interruptTransmit, dev);
		enable_irq(priv->interruptTransmit);
	}
}
#endif

/***************************************************************
 *   zl30310_interrupt
 *     The interrupt handler for devices with one interrupt
 *
 ***************************************************************/
static irqreturn_t zl30310_interrupt(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct zl30310_private *priv = netdev_priv(dev);

	/* Save ievent for future reference */
	u32 events = zl30310_read(&priv->regs->ievent);

	/* Check for reception */
	if (events & IEVENT_RX_MASK)
		zl30310_receive(irq, dev_id);

	/* Check for transmit completion */
	if (events & IEVENT_TX_MASK)
		zl30310_transmit(irq, dev_id);

	/* Check for errors */
	if (events & IEVENT_ERR_MASK)
		zl30310_error(irq, dev_id);

	return IRQ_HANDLED;
}

/* Called every time the controller might need to be made
 * aware of new link state.  The PHY code conveys this
 * information through variables in the phydev structure, and this
 * function converts those variables into the appropriate
 * register values, and can bring down the device if needed.
 */
static void adjust_link(struct net_device *dev)
{
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	unsigned long flags;
	struct phy_device *phydev = priv->phydev;
	int new_state = 0;

	spin_lock_irqsave(&priv->txlock, flags);
	if (phydev->link) {
		u32 tempval = zl30310_read(&regs->maccfg2);
		u32 ecntrl = zl30310_read(&regs->ecntrl);

		/* Now we make sure that we can be in full duplex mode.
		 * If not, we operate in half-duplex mode. */
		if (phydev->duplex != priv->oldduplex) {
			new_state = 1;
			if (!(phydev->duplex))
				tempval &= ~(MACCFG2_FULL_DUPLEX);
			else
				tempval |= MACCFG2_FULL_DUPLEX;

			priv->oldduplex = phydev->duplex;
		}

		if (phydev->speed != priv->oldspeed) {
			new_state = 1;
			switch (phydev->speed) {
			case 1000:
				tempval =
				    ((tempval & ~(MACCFG2_IF)) | MACCFG2_GMII);
				break;
			case 100:
			case 10:
				tempval =
				    ((tempval & ~(MACCFG2_IF)) | MACCFG2_MII);

				/* Reduced mode distinguishes
				 * between 10 and 100 */
				if (phydev->speed == SPEED_100)
					ecntrl |= ECNTRL_R100;
				else
					ecntrl &= ~(ECNTRL_R100);
				break;
			default:
				if (netif_msg_link(priv))
					printk(KERN_WARNING
						"%s: Ack!  Speed (%d) is not 10/100/1000!\n",
						dev->name, phydev->speed);
				break;
			}

			priv->oldspeed = phydev->speed;
		}

		zl30310_write(&regs->maccfg2, tempval);
		zl30310_write(&regs->ecntrl, ecntrl);

		if (!priv->oldlink) {
			new_state = 1;
			priv->oldlink = 1;
			//netif_schedule(dev);
            netif_tx_schedule_all(dev);
		}
	} else if (priv->oldlink) {
		new_state = 1;
		priv->oldlink = 0;
		priv->oldspeed = 0;
		priv->oldduplex = -1;
	}

	if (new_state && netif_msg_link(priv))
		phy_print_status(phydev);

	spin_unlock_irqrestore(&priv->txlock, flags);
}

/***************************************************************
 *   zl30310_set_multi
 *     Update the hash table based on the current list of multicast
 *     addresses we subscribe to.  Also, change the promiscuity of
 *     the device based on the flags (this function is called
 *     whenever dev->flags is changed
 *
 ***************************************************************/
static void zl30310_set_multi(struct net_device *dev)
{
	struct dev_mc_list *mc_ptr;
	struct zl30310_private *priv = netdev_priv(dev);
	struct zl30310 __iomem *regs = priv->regs;
	u32 tempval;

	if(dev->flags & IFF_PROMISC) {
		/* Set RCTRL to PROM */
		tempval = zl30310_read(&regs->rctrl);
		tempval |= RCTRL_PROM;
		zl30310_write(&regs->rctrl, tempval);
	} else {
		/* Set RCTRL to not PROM */
		tempval = zl30310_read(&regs->rctrl);
		tempval &= ~(RCTRL_PROM);
		zl30310_write(&regs->rctrl, tempval);
	}

	if(dev->flags & IFF_ALLMULTI) {
		/* Set the hash to rx all multicast frames */
		zl30310_write(&regs->igaddr0, 0xffffffff);
		zl30310_write(&regs->igaddr1, 0xffffffff);
		zl30310_write(&regs->igaddr2, 0xffffffff);
		zl30310_write(&regs->igaddr3, 0xffffffff);
		zl30310_write(&regs->igaddr4, 0xffffffff);
		zl30310_write(&regs->igaddr5, 0xffffffff);
		zl30310_write(&regs->igaddr6, 0xffffffff);
		zl30310_write(&regs->igaddr7, 0xffffffff);
		zl30310_write(&regs->gaddr0, 0xffffffff);
		zl30310_write(&regs->gaddr1, 0xffffffff);
		zl30310_write(&regs->gaddr2, 0xffffffff);
		zl30310_write(&regs->gaddr3, 0xffffffff);
		zl30310_write(&regs->gaddr4, 0xffffffff);
		zl30310_write(&regs->gaddr5, 0xffffffff);
		zl30310_write(&regs->gaddr6, 0xffffffff);
		zl30310_write(&regs->gaddr7, 0xffffffff);
	} else {
		int em_num;
		int idx;

		/* zero out the hash */
		zl30310_write(&regs->igaddr0, 0x0);
		zl30310_write(&regs->igaddr1, 0x0);
		zl30310_write(&regs->igaddr2, 0x0);
		zl30310_write(&regs->igaddr3, 0x0);
		zl30310_write(&regs->igaddr4, 0x0);
		zl30310_write(&regs->igaddr5, 0x0);
		zl30310_write(&regs->igaddr6, 0x0);
		zl30310_write(&regs->igaddr7, 0x0);
		zl30310_write(&regs->gaddr0, 0x0);
		zl30310_write(&regs->gaddr1, 0x0);
		zl30310_write(&regs->gaddr2, 0x0);
		zl30310_write(&regs->gaddr3, 0x0);
		zl30310_write(&regs->gaddr4, 0x0);
		zl30310_write(&regs->gaddr5, 0x0);
		zl30310_write(&regs->gaddr6, 0x0);
		zl30310_write(&regs->gaddr7, 0x0);

		/* If we have extended hash tables, we need to
		 * clear the exact match registers to prepare for
		 * setting them */
		if (priv->extended_hash) {
			em_num = GFAR_EM_NUM + 1;
			zl30310_clear_exact_match(dev);
			idx = 1;
		} else {
			idx = 0;
			em_num = 0;
		}

		if(dev->mc_count == 0)
			return;

		/* Parse the list, and set the appropriate bits */
		for(mc_ptr = dev->mc_list; mc_ptr; mc_ptr = mc_ptr->next) {
			if (idx < em_num) {
				zl30310_set_mac_for_addr(dev, idx,
						mc_ptr->dmi_addr);
				idx++;
			} else
				zl30310_set_hash_for_addr(dev, mc_ptr->dmi_addr);
		}
	}

	return;
}


/***************************************************************
 *   zl30310_clear_exact_match
 *     Clears each of the exact match registers to zero, so they
 *     don't interfere with normal reception
 *
 ***************************************************************/
static void zl30310_clear_exact_match(struct net_device *dev)
{
	int idx;
	u8 zero_arr[MAC_ADDR_LEN] = {0,0,0,0,0,0};

	for(idx = 1;idx < GFAR_EM_NUM + 1;idx++)
		zl30310_set_mac_for_addr(dev, idx, (u8 *)zero_arr);
}

/***************************************************************
 *   zl30310_set_hash_for_addr
 *     Set the appropriate hash bit for the given addr
 *
 ***************************************************************/
/* The algorithm works like so:
 * 1) Take the Destination Address (ie the multicast address), and
 * do a CRC on it (little endian), and reverse the bits of the
 * result.
 * 2) Use the 8 most significant bits as a hash into a 256-entry
 * table.  The table is controlled through 8 32-bit registers:
 * gaddr0-7.  gaddr0's MSB is entry 0, and gaddr7's LSB is
 * gaddr7.  This means that the 3 most significant bits in the
 * hash index which gaddr register to use, and the 5 other bits
 * indicate which bit (assuming an IBM numbering scheme, which
 * for PowerPC (tm) is usually the case) in the register holds
 * the entry. */
static void zl30310_set_hash_for_addr(struct net_device *dev, u8 *addr)
{
	u32 tempval;
	struct zl30310_private *priv = netdev_priv(dev);
	u32 result = ether_crc(MAC_ADDR_LEN, addr);
	int width = priv->hash_width;
	u8 whichbit = (result >> (32 - width)) & 0x1f;
	u8 whichreg = result >> (32 - width + 5);
	u32 value = (1 << (31-whichbit));

	tempval = zl30310_read(priv->hash_regs[whichreg]);
	tempval |= value;
	zl30310_write(priv->hash_regs[whichreg], tempval);

	return;
}

/***************************************************************
 *   zl30310_set_mac_for_addr
 *     There are multiple MAC Address register pairs on some controllers
 *     This function sets the numth pair to a given address
 *
 ***************************************************************/
static void zl30310_set_mac_for_addr(struct net_device *dev, int num, u8 *addr)
{
	struct zl30310_private *priv = netdev_priv(dev);
	int idx;
	char tmpbuf[MAC_ADDR_LEN];
	u32 tempval;
	u32 __iomem *macptr = &priv->regs->macstnaddr1;

	macptr += num*2;

	/* Now copy it into the mac registers backwards, cuz */
	/* little endian is silly */
	for (idx = 0; idx < MAC_ADDR_LEN; idx++)
		tmpbuf[MAC_ADDR_LEN - 1 - idx] = addr[idx];

	zl30310_write(macptr, *((u32 *) (tmpbuf)));

	tempval = *((u32 *) (tmpbuf + 4));

	zl30310_write(macptr+1, tempval);
}

/***************************************************************
 *   zl30310_error
 *     Ethernet error interrupt handler
 *
 ***************************************************************/
static irqreturn_t zl30310_error(int irq, void *dev_id)
{
	struct net_device *dev = dev_id;
	struct zl30310_private *priv = netdev_priv(dev);

	/* Save ievent for future reference */
	u32 events = zl30310_read(&priv->regs->ievent);
	u32 masks = zl30310_read(&priv->regs->imask);
	printk(KERN_DEBUG "%s:zl30310_error events=%x, mask=%x\n", dev->name, events, masks);

	/* Clear IEVENT */
	zl30310_write(&priv->regs->ievent, events & IEVENT_ERR_MASK);

	/* Magic Packet is not an error. */
	if ((priv->einfo->device_flags & FSL_GIANFAR_DEV_HAS_MAGIC_PACKET) &&
	    (events & IEVENT_MAG))
		events &= ~IEVENT_MAG;

	/* Hmm... */
	if (netif_msg_rx_err(priv) || netif_msg_tx_err(priv))
		printk(KERN_DEBUG "%s: error interrupt (ievent=0x%08x imask=0x%08x)\n",
		       dev->name, events, zl30310_read(&priv->regs->imask));

	/* Update the error counters */
	if (events & IEVENT_TXE) {
		priv->stats.tx_errors++;

		if (events & IEVENT_LC)
			priv->stats.tx_window_errors++;
		if (events & IEVENT_CRL)
			priv->stats.tx_aborted_errors++;
		if (events & IEVENT_XFUN) {
			if (netif_msg_tx_err(priv))
				printk(KERN_DEBUG "%s: TX FIFO underrun, "
				       "packet dropped.\n", dev->name);
			priv->stats.tx_dropped++;
			priv->extra_stats.tx_underrun++;

			/* Reactivate the Tx Queues */
			zl30310_write(&priv->regs->tstat, TSTAT_CLEAR_THALT);
		}
		if (netif_msg_tx_err(priv))
			printk(KERN_DEBUG "%s: Transmit Error\n", dev->name);
	}
	if (events & IEVENT_BSY) {
		priv->stats.rx_errors++;
		priv->extra_stats.rx_bsy++;

		zl30310_receive(irq, dev_id);

#ifndef CONFIG_GFAR_NAPI
		/* Clear the halt bit in RSTAT */
		zl30310_write(&priv->regs->rstat, RSTAT_CLEAR_RHALT);
#endif

		if (netif_msg_rx_err(priv))
			printk(KERN_DEBUG "%s: busy error (rstat: %x)\n",
			       dev->name, zl30310_read(&priv->regs->rstat));
	}
	if (events & IEVENT_BABR) {
		priv->stats.rx_errors++;
		priv->extra_stats.rx_babr++;

		if (netif_msg_rx_err(priv))
			printk(KERN_DEBUG "%s: babbling RX error\n", dev->name);
	}
	if (events & IEVENT_EBERR) {
		priv->extra_stats.eberr++;
		if (netif_msg_rx_err(priv))
			printk(KERN_DEBUG "%s: bus error\n", dev->name);
	}
	if ((events & IEVENT_RXC) && netif_msg_rx_status(priv))
		printk(KERN_DEBUG "%s: control frame\n", dev->name);

	if (events & IEVENT_BABT) {
		priv->extra_stats.tx_babt++;
		if (netif_msg_tx_err(priv))
			printk(KERN_DEBUG "%s: babbling TX error\n", dev->name);
	}
	return IRQ_HANDLED;
}

/***************************************************************
 *   zl30310_driver
 *     Structure for the ethernet device driver
 *
 ***************************************************************/
static struct platform_driver zl30310_driver = {
	.probe = zl30310_probe,
	.remove = zl30310_remove,
	.suspend = zl30310_suspend,
	.resume = zl30310_resume,
	.driver	= {
		.name = "zl30310-eth",
	},
};




#ifdef ZL_CHAR_DEV

/***************************************************************
 *   zlPktToUser_open
 *     Ethernet char device open operation
 *
 ***************************************************************/
static int
zlPktToUser_open (struct inode *inode, struct file *filp)
{
	int minor = MINOR(inode->i_rdev);
	printk(KERN_DEBUG "zlPktToUser_open: minor=%d\n", minor);

	if (minor > MAX_ZL_PKT_MINOR)  /* Support for a single ZL303XX device! */
	{
		printk(KERN_DEBUG "zlPktToUser_open: Only supports minor device=%d for now!\n", MAX_ZL_PKT_MINOR);
		return -ENODEV;
	}
	if (devInUse)
	{
		printk(KERN_DEBUG "zlPktToUser_open: Only a single client allowed - it's already in use!\n");
		return (-EMFILE);
	}

	devInUse++;
	filp->private_data = (void *)minor;

	return 0;
}

/***************************************************************
 *   zlPktToUser_release
 *
 *
 ***************************************************************/
static int
zlPktToUser_release(struct inode *inode, struct file *filp)
{
	devInUse--;
	return 0;
}

/***************************************************************
 *   parsePktMatchCmds
 *      Parsing the packet matching rule
 *
 ***************************************************************/
static int
parsePktMatchCmds(u_char *tx_kbuf, ssize_t bytes)
{
	#ifdef MORE_DEBUG
	    {
	    	int i;

	    	printk("zlCmdFromUser_write: Received Pkt from user space; Length=0x%x (command (1-Add),filter#,offset,length,pattern)=0X \n", bytes);
	    	for(i=0; i<bytes; i++)
	    		printk("%2x ", tx_kbuf[i]);
	    	printk("\n");
	    }
	#endif

    memset(&filterCmd, 0, sizeof(filterCmd));   /* Re-use existing Cmd */

    filterCmd[FPE_C] = tx_kbuf[FPE_C];          /* Command 0=Delete, 1=Add */
    filterCmd[FPE_F] = tx_kbuf[FPE_F];          /* Filter# */
    filterCmd[FPE_O] = tx_kbuf[FPE_O];          /* Offset */
    filterCmd[FPE_L] = tx_kbuf[FPE_L];          /* Length */
    memcpy(&filterCmd[FPE_P], &tx_kbuf[FPE_P], filterCmd[FPE_L]);  /* Length bytes of pattern data */

    if (bytes != (FPE_P + filterCmd[FPE_L]))    /* Check for bad command encoding */
    {
		printk("parsePktMatchCmds: Received bytes=%d from user space; but received %d-%d != patternLength=%d in filterCmd\n", bytes, bytes, FPE_P, filterCmd[FPE_L]);
        bytes = -bytes; /* Failure! */
    }

    if ((int)filterCmd[FPE_C] == 0)                                                 /* Remove an active filter */
    {
        memset(&filterTable[(int)filterCmd[FPE_F]][FPE_C], 0, sizeof(filterCmd));       /* Clear table entry @indx */
        filterActiveBits &= ~((u_int64_t)1 << filterCmd[FPE_F]);                                   /* Clear active bit */
	    #ifdef DEBUG
            printk("parsePktMatchCmds: Remove filterActiveBits = 0x %x:%x  \n",
                   (unsigned int)((filterActiveBits>>16)>>16), (unsigned int)(filterActiveBits&0xFFFFFFFF));
        #endif
    }
    else                                                                            /* Add an active filter */
    {
        /* Activate a new pattern filter match in the table */
        memcpy(&filterTable[(int)filterCmd[FPE_F]][0], &filterCmd[0], sizeof(filterCmd));
        filterActiveBits |= ((u_int64_t)1 << filterCmd[FPE_F]);                                    /* Set active bit */
	    #ifdef DEBUG
        printk("parsePktMatchCmds: Add filterActiveBits = 0x %x:%x  \n",
               (unsigned int)((filterActiveBits>>16)>>16(, (unsigned int)(filterActiveBits&0xFFFFFFFF));
        #endif
    }

	return bytes;
}

/***************************************************************
 *   zlCmdFromUser_write
 *     Ethernet char device write operation
 *
 ***************************************************************/
static ssize_t
zlCmdFromUser_write(struct file *filp, const char *buf, size_t bytes, loff_t *f_pos)
{
	u_char tx_kbuf[PKT_MATCH_CONFIG_CMD_SIZE];  /* From user */
/*	int if_id = (int)filp->private_data;    // file desc. num */

    if (!devInUse)
        return -ENODEV;

	/* Max command size */
	if (bytes > PKT_MATCH_CONFIG_CMD_SIZE)
		return -EFBIG;

	if (copy_from_user(tx_kbuf, buf, (ssize_t)bytes))
		return -EIO;


	return (parsePktMatchCmds(tx_kbuf, bytes));
}

/***************************************************************
 *   zlPktToUser_read
 *     Ethernet char device read operation
 *
 ***************************************************************/
static ssize_t
zlPktToUser_read(struct file *filp, char __user *buf, size_t bytes, loff_t *f_pos)
{
    static u_int32_t userIndex;
    u_int32_t retVal, activeIndx = userIndex;

    #ifdef MORE_DEBUG
        int if_id = (int)filp->private_data;
        printk("%s: read - bytes=%d, if_id=%d\n", DRIVER_NAME, bytes, if_id);
    #endif


    if (!busyBufferBits) /* Any buffers busy? then don't wait */
    {
        wait_queue_t tsPktWait;

        /* Setup user-space client read to block waiting for a pkt */
        init_waitqueue_entry(&tsPktWait, current);   /* Setup the wait */
        add_wait_queue(&qTsPktWait, &tsPktWait);    /* Add self to wait Q */
        set_current_state(TASK_INTERRUPTIBLE);
        if (!signal_pending(current))
        {
            #ifdef MORE_DEBUG
            printk("Schedule Pkt listener, waiting - \n");
            #endif
            schedule();
        }
        set_current_state(TASK_RUNNING);
        remove_wait_queue(&qTsPktWait, &tsPktWait);
    }

    while (busyBufferBits)
    {
        if (busyBufferBits & ((u_int64_t)1 << userIndex))       /* Active at index? */
        {
            activeIndx = userIndex;
            break;                                              /* Found pkt for user-space */
        }
        else
            userIndex = (++userIndex % RX_PKT_Q_SIZE);          /* Adjust and wrap if needed */
        if (activeIndx == userIndex)
            return 0;                                           /* complete cycle */
    }

    retVal = pktLengthToUser[userIndex];                        /* Normal return val */

    if (bytes < pktLengthToUser[userIndex])
    {
        printk("Received Pkt from kernel, length=%d > user provided bytes=%d \n", pktLengthToUser[userIndex], bytes);
        retVal = 0;
    }
    if (copy_to_user(buf, &(rx_kbuf[userIndex][0]), (ssize_t)pktLengthToUser[userIndex]))
    {
        retVal = -EIO;
    }

    memset(&(rx_kbuf[userIndex][0]), 0, MAX_PKT_LEN_TO_USER + PKT_PREAMBLE_LEN);
    pktLengthToUser[userIndex] = 0;
    busyBufferBits &= ~((u_int64_t)1 << userIndex);         /* clear this busy buffer bit */
    userIndex = (++userIndex % RX_PKT_Q_SIZE);              /* wrap userIndex if needed */

    return (ssize_t)retVal;
}

#endif


/***************************************************************
 *   zl30310_init
 *     Register the ethernet device driver and the char device driver
 *
 ***************************************************************/
static int __init zl30310_init(void)
{
	int err;// = gfar_mdio_init();

    //if (err)
//        return err;

	err = platform_driver_register(&zl30310_driver);

	if (err)
		return err;



#ifdef ZL_CHAR_DEV
	// Register the ZL TS Pkt to user-space driver.
	err = register_chrdev(ZL_TS_MAJOR, DRIVER_NAME, &zlPktToUser_fops);
	if (err < 0)
	{
		printk(KERN_DEBUG "Couldn't register device (major=%d); Err=%d\n", ZL_TS_MAJOR, err);
		return -EACCES;
	}

    #if !defined ZL_USES_FPE    /* No FPE! - Setup default filters for non-VLan PTP and RTP */
        {
            memset(&filterCmd, 0, sizeof(filterCmd));   /* Re-use existing Cmd */

            /* Activate a non-vlan PTP UDP dest. port match */
            filterCmd[FPE_C] = 1;          /* Command 0=Delete, 1=Add */
            filterCmd[FPE_F] = 0;          /* Filter# */
            filterCmd[FPE_O] = 36;         /* Offset */
            filterCmd[FPE_L] = 2;          /* Length */
            filterCmd[FPE_P] = 0x01;    /* Ptp event port 319 */
            filterCmd[FPE_P+1] = 0x3F;  /* Ptp event port 319 */
            if (parsePktMatchCmds(filterCmd, 6) != 6)
                printf("Write of PTP kernel ethernet filter failed!\n");

            /* Activate a non-vlan RTP UDP dest. port match */
            filterCmd[FPE_F] = 1;          /* Filter# */
            filterCmd[FPE_P] = 0x0C;    /* Rtp event port 50008 */
            filterCmd[FPE_P+1] = 0x58;  /* Rtp event port 50008 */
            if (parsePktMatchCmds(filterCmd, 6) != 6)
                printf("Write of RTP kernel ethernet filter failed!\n");

            #ifdef ZL_NTP_SERVER
                /* Activate a non-vlan NTP UDP dest. port match */
                filterCmd[FPE_F] = 2;          /* Filter# */
                filterCmd[FPE_O] = 37;         /* Offset */
                filterCmd[FPE_L] = 1;          /* Length */
                filterCmd[FPE_P] = 0x7B;  /* NTP event port 123 */
                if (parsePktMatchCmds(filterCmd, 5) != 5)
                    printf("Write of NTP kernel ethernet filter failed!\n");
            #endif

        }
    #endif
#endif

	return err;
}

/***************************************************************
 *   zl30310_exit
 *     Unregister the char device
 *
 ***************************************************************/
static void __exit zl30310_exit(void)
{
	platform_driver_unregister(&zl30310_driver);

#ifdef ZL_CHAR_DEV
	unregister_chrdev(ZL_TS_MAJOR, DRIVER_NAME);
    memset(&filterTable, 0, sizeof(filterTable));   /* Remove filters */
    filterActiveBits = 0;
    busyBufferBits = 0;
	devInUse = 0;
#endif
}

module_init(zl30310_init);
module_exit(zl30310_exit);

