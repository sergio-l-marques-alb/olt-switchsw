/*
 * arch/powerpc/platforms/83xx/mpc8313_rdb.c
 *
 * Description: MPC8313x RDB board specific routines. This file is based on mpc834x_sys.c
 * Author: Lo Wlison <r43300@freescale.com>
 *
 * Copyright (C) 2006 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/reboot.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/console.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/root_dev.h>
#include <linux/fsl_devices.h>
#include <linux/rtc.h>

#include <asm/system.h>
#include <asm/atomic.h>
#include <asm/time.h>
#include <asm/io.h>
#include <asm/machdep.h>
#include <asm/ipic.h>
//#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#include "mpc83xx.h"
#include <linux/of_platform.h>

#ifdef CONFIG_GFAR_PTP_VCO
#define MPC8313_SICRH_1588_EXT_CLK	0x00000040
#endif

#ifndef CONFIG_PCI
unsigned long isa_io_base = 0;
unsigned long isa_mem_base = 0;
#endif

extern enum fsl_usb2_phy_modes determine_usb_phy(const char *phy_type);

static int __init olt7_8ch_usb_cfg(void)
{
//	u32 temp;
//	void __iomem *immap, *usb_regs;
//	struct device_node *np = NULL;
//	enum fsl_usb2_phy_modes phy_mode;
//	const void *prop;
//	struct resource res;
//	int ret = 0;
//
//	if ((np = of_find_compatible_node(NULL, "usb", "fsl-usb2-dr")) == NULL)
//		return -ENODEV;
//	prop = of_get_property(np, "phy_type", NULL);
//	phy_mode = determine_usb_phy(prop);
//
//	/* Map IMMR space for pin and clock settings */
//	immap = ioremap(get_immrbase(), 0x1000);
//	if (!immap) {
//		of_node_put(np);
//		return -ENOMEM;
//	}
//
//	/* Configure clock */
//	temp = in_be32(immap + MPC83XX_SCCR_OFFS);
//	temp &= ~MPC83XX_SCCR_USB_MASK;
//	temp |= MPC83XX_SCCR_USB_DRCM_11;  /* 1:3 */
//	out_be32(immap + MPC83XX_SCCR_OFFS, temp);
//
//	/* Configure pin mux for ULPI.  There is no pin mux for UTMI */
//	if (phy_mode == FSL_USB2_PHY_ULPI) {
//		temp = in_be32(immap + MPC83XX_SICRL_OFFS);
//		temp &= ~MPC8313_SICRL_MASK;
//		temp |= MPC8313_SICRL_ULPI;
//		out_be32(immap + MPC83XX_SICRL_OFFS, temp);
//		temp = in_be32(immap + MPC83XX_SICRH_OFFS);
//		temp &= ~MPC8313_SICRH_MASK;
//		temp |= MPC8313_SICRH_ULPI;
//		out_be32(immap + MPC83XX_SICRH_OFFS, temp);
//	}
//
//	iounmap(immap);
//
//	/* Map USB SOC space */
//	ret = of_address_to_resource(np, 0, &res);
//	if (ret) {
//		of_node_put(np);
//		return ret;
//	}
//	usb_regs = ioremap(res.start, res.end - res.start + 1);
//
//	/* Using on-chip PHY */
//	if ((phy_mode == FSL_USB2_PHY_UTMI_WIDE) ||
//			(phy_mode == FSL_USB2_PHY_UTMI)) {
//		/* Set UTMI_PHY_EN, REFSEL to 48MHZ */
//		out_be32(usb_regs + FSL_USB2_CONTROL_OFFS, 0x280);
//	/* Using external UPLI PHY */
//	} else if (phy_mode == FSL_USB2_PHY_ULPI) {
//		/* Set PHY_CLK_SEL to ULPI */
//		temp = 0x400;
//#ifdef CONFIG_USB_OTG
//		/* Set OTG_PORT */
//		temp |= 0x20;
//#endif /* CONFIG_USB_OTG */
//		out_be32(usb_regs + FSL_USB2_CONTROL_OFFS, temp);
//	} else {
//		printk(KERN_ERR "USB PHY type not supported\n");
//		ret = -EINVAL;
//	}
//
//	iounmap(usb_regs);
//	of_node_put(np);
//	return ret;
	return 0;
}

/* ************************************************************************
 *
 * Setup the architecture
 *
 */
static void __init olt7_8ch_setup_arch(void)
{
	struct device_node *np;
#ifdef CONFIG_GFAR_PTP_VCO
	void __iomem *immap;
	unsigned long sicrh;
#endif

	if (ppc_md.progress)
		ppc_md.progress("olt7_8ch_setup_arch()", 0);

	np = of_find_node_by_type(NULL, "cpu");
	if (np != 0) {
		const unsigned int *fp =
			of_get_property(np, "clock-frequency", NULL);
		if (fp != 0)
			loops_per_jiffy = *fp / HZ;
		else
			loops_per_jiffy = 50000000 / HZ;
		of_node_put(np);
	}
#ifdef CONFIG_FSL_PCI
        for_each_compatible_node(np, "pci", "fsl,mpc8349-pci")
                mpc83xx_add_bridge(np);
#endif

	olt7_8ch_usb_cfg();

#ifdef  CONFIG_ROOT_NFS
	ROOT_DEV = Root_NFS;
#else
	ROOT_DEV = Root_HDA1;
#endif

#ifdef CONFIG_GFAR_PTP_VCO
	immap = ioremap(get_immrbase(), 0x1000);
	sicrh = in_be32(immap + MPC83XX_SICRH_OFFS);
	sicrh |= MPC8313_SICRH_1588_EXT_CLK;
	out_be32(immap + MPC83XX_SICRH_OFFS, sicrh);
#endif
	
}

void __init olt7_8ch_init_IRQ(void)
{
	struct device_node *np;

	np = of_find_node_by_type(NULL, "ipic");
	if (!np)
		return;

	ipic_init(np, 0);

	/* Initialize the default interrupt mapping priorities,
	 * in case the boot rom changed something on us.
	 */
	ipic_set_default_priority();
}

/*
 * Called very early, MMU is off, device-tree isn't unflattened
 */
static int __init olt7_8ch_probe(void)
{
	/* We always match for now, eventually we should look at the flat
	   dev tree to ensure this is the board we are suppose to run on
	*/
	return 1;
}

static struct of_device_id olt7_8ch_soc_ids[] = {
	{ .type = "soc", },
	{},
};

static int __init olt7_8ch_declare_of_platform_devices(void)
{
	if (!machine_is(olt7_8ch))
		return 0;

	of_platform_bus_probe(NULL, olt7_8ch_soc_ids, NULL);
		return 0;
}
device_initcall(olt7_8ch_declare_of_platform_devices);

#if defined(CONFIG_I2C_MPC) && defined(CONFIG_SENSORS_DS1337)
extern int ds1337_do_command(int bus, int cmd, void *arg);
extern spinlock_t rtc_lock;
#define DS1337_GET_DATE 0
#define DS1337_SET_DATE 1
static void ds1337_get_rtc_time(struct rtc_time *tm)
{
	int result;
	unsigned long flags;

	spin_lock_irqsave(&rtc_lock, flags);
	result = ds1337_do_command(0, DS1337_GET_DATE, tm);
	spin_unlock_irqrestore(&rtc_lock, flags);
	return;
}

static int ds1337_set_rtc_time(struct rtc_time *tm)
{
	int result;
	unsigned long flags;

	spin_lock_irqsave(&rtc_lock, flags);
	result = ds1337_do_command(0, DS1337_SET_DATE, tm);
	spin_unlock_irqrestore(&rtc_lock, flags);

	return result;
}
static int __init mpc8313_rtc_hookup(void)
{
	struct rtc_time tm;
	struct timespec tv;

	if (!machine_is(olt7_8ch))
		return 0;
	ppc_md.get_rtc_time = ds1337_get_rtc_time;
	ppc_md.set_rtc_time = ds1337_set_rtc_time;

	ppc_md.get_rtc_time(&tm);
	tv.tv_nsec = 0;
	tv.tv_sec = mktime(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
			tm.tm_hour, tm.tm_min, tm.tm_sec);
	do_settimeofday(&tv);
	return 0;
}
late_initcall(mpc8313_rtc_hookup);
#endif

define_machine(olt7_8ch) {
	.name			= "OLT7_8CH",
	.probe			= olt7_8ch_probe,
	.setup_arch		= olt7_8ch_setup_arch,
	.init_IRQ		= olt7_8ch_init_IRQ,
	.get_irq		= ipic_get_irq,
	.restart		= mpc83xx_restart,
	.time_init		= mpc83xx_time_init,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
