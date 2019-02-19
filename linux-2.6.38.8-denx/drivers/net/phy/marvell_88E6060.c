/*
 * drivers/net/phy/marvel_88E6060.c
 *
 * drivers/net/phy/vitesse_7385.c
 *
 * Dummy Ethernet switch driver for MPC8313ERDB platform
 *
 * Author:  Tony Li <Tony.Li@freescale.com>
 *      Based on the MPC8349-mITX switch code. All the substantial init
 *      operations has been done in u-boot. This code just creats a dummy
 *      phy device to deal with kernel stock and do not touch hardware.
 *
 * Copyright (C) 2006 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
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
#include <linux/spinlock.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mii.h>
#include <linux/ethtool.h>
#include <linux/phy.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

MODULE_DESCRIPTION("Dummy Ethernet switch driver for OLT7_8CH platform");
MODULE_AUTHOR("Jose Matos");
MODULE_LICENSE("GPL");

static int marvel_88e6060_config_init(struct phy_device *phydev)
{
	return 0;
}

static int marvel_88e6060_ack_interrupt(struct phy_device *phydev)
{
	return 0;
}

static int marvel_88e6060_config_intr(struct phy_device *phydev)
{
	return 0;
}

static int marvel_88e6060_config_aneg(struct phy_device *phydev)
{
	return 0;
}

static int marvel_88e6060_read_status(struct phy_device *phydev)
{
	/* Switch is always assumed to be up */
	phydev->speed = SPEED_100;
	phydev->duplex = DUPLEX_FULL;
	phydev->pause = phydev->asym_pause = 0;
//	phydev->link = 1;
//	phydev->state = PHY_RUNNING;
//	netif_carrier_on(phydev->attached_dev);

	return 0;
}

//#define OLT7_8CH_SWITCHADDR	0x1	/* PHY Address 1*/
#define OLT7_8CH_SWITCHID	0x01410C87	/* PHY ID */

/* Dummy Ethernet switch driver for MPC8313ERDB platform */
static struct phy_driver marvel_88e6060_driver = {
	.phy_id		= OLT7_8CH_SWITCHID,
	.name		= "OLT7_8CH Ethernet Switch",
	.phy_id_mask	= 0xFFFFFFFF,
	.features	= PHY_BASIC_FEATURES,
	.flags		= 0,
	.config_init	= &marvel_88e6060_config_init,
	.config_aneg	= &marvel_88e6060_config_aneg,
	.read_status	= &marvel_88e6060_read_status,
	.ack_interrupt	= &marvel_88e6060_ack_interrupt,
	.config_intr	= &marvel_88e6060_config_intr,
	.driver 	= { .owner = THIS_MODULE,},
};

static int __init marvel_88e6060_init(void)
{
	return phy_driver_register(&marvel_88e6060_driver);
}

static void __exit marvel_88e6060_exit(void)
{
	phy_driver_unregister(&marvel_88e6060_driver);
}

module_init(marvel_88e6060_init);
module_exit(marvel_88e6060_exit);
