/*
 * MPC85xx RDB Board Setup
 *
 * Copyright 2009-2011 Freescale Semiconductor Inc.
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/stddef.h>
#include <linux/kernel.h>
#include <linux/pci.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/seq_file.h>
#include <linux/interrupt.h>
#include <linux/of_platform.h>
#include <linux/memblock.h>

#include <asm/system.h>
#include <asm/time.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <mm/mmu_decl.h>
#include <asm/prom.h>
#include <asm/udbg.h>
#include <asm/mpic.h>
#include <asm/swiotlb.h>

#include <sysdev/fsl_soc.h>
#include <sysdev/fsl_pci.h>

#undef DEBUG

#ifdef DEBUG
#define DBG(fmt, args...) printk(KERN_ERR "%s: " fmt, __func__, ## args)
#else
#define DBG(fmt, args...)
#endif


void __init mpc85xx_rdb_pic_init(void)
{
	struct mpic *mpic;
	struct resource r;
	struct device_node *np;
	unsigned long root = of_get_flat_dt_root();

	np = of_find_node_by_type(NULL, "open-pic");
	if (np == NULL) {
		printk(KERN_ERR "Could not find open-pic node\n");
		return;
	}

	if (of_address_to_resource(np, 0, &r)) {
		printk(KERN_ERR "Failed to map mpic register space\n");
		of_node_put(np);
		return;
	}

	if (of_flat_dt_is_compatible(root, "fsl,MPC85XXRDB-CAMP")) {
		mpic = mpic_alloc(np, r.start,
			MPIC_PRIMARY |
			MPIC_BIG_ENDIAN | MPIC_BROKEN_FRR_NIRQS,
			0, 256, " OpenPIC  ");
	} else {
		mpic = mpic_alloc(np, r.start,
		  MPIC_PRIMARY | MPIC_WANTS_RESET |
		  MPIC_BIG_ENDIAN | MPIC_BROKEN_FRR_NIRQS |
		  MPIC_SINGLE_DEST_CPU,
		  0, 256, " OpenPIC  ");
	}

	BUG_ON(mpic == NULL);
	of_node_put(np);

	mpic_init(mpic);

}

#if defined(CONFIG_PCI)
/* Host/agent status can be read from porbmsr in the global utilities*/
static int get_pcie_host_agent(void)
{
	struct device_node *np;
	void __iomem *guts_regs;
	u32 host_agent;

	np = of_find_compatible_node(NULL, NULL, "fsl,p2020-guts");
	if (!np) {
		printk(KERN_ERR "Could not find global-utilities node\n");
		return 0;
	}

	guts_regs = of_iomap(np, 0);
	of_node_put(np);

	if (!guts_regs) {
		printk(KERN_ERR "Failed to map global-utilities register space\n");
		return 0;
	}
	host_agent = (in_be32(guts_regs + 4) & 0x00070000) >> 16 ;

	iounmap(guts_regs);
	return host_agent;
}

/*
 * To judge if the PCI(e) controller is host/agent mode through
 * the PORBMSR register.
 * 	0: agent mode
 * 	1: host mode
 */
static int is_pcie_host(u32 host_agent, resource_size_t res)
{
	if ((res & 0xfffff) == 0xa000) {
		switch (host_agent) {
		case 0:
		case 1:
		case 4:
		case 5:
			return 0;
		case 2:
		case 3:
		case 6:
		default:
			return 1;
		}
	} else
		return (host_agent % 2) ? 1 : 0;
}
#endif

/*
 * Setup the architecture
 */
#ifdef CONFIG_SMP
extern void __init mpc85xx_smp_init(void);
#endif
static void __init mpc85xx_rdb_setup_arch(void)
{
#ifdef CONFIG_PCI
	struct device_node *np;
	struct pci_controller *hose;
#endif
	dma_addr_t max = 0xffffffff;

	if (ppc_md.progress)
		ppc_md.progress("mpc85xx_rdb_setup_arch()", 0);

#ifdef CONFIG_PCI
	for_each_node_by_type(np, "pci") {
		if (of_device_is_compatible(np, "fsl,mpc8548-pcie")) {
			struct resource rsrc;
			of_address_to_resource(np, 0, &rsrc);
			if (!is_pcie_host(get_pcie_host_agent(), rsrc.start))
				continue;
			fsl_add_bridge(np, 0);
			hose = pci_find_hose_for_OF_device(np);
			max = min(max, hose->dma_window_base_cur +
					hose->dma_window_size);
		}
	}

#endif

#ifdef CONFIG_SMP
	mpc85xx_smp_init();
#endif

#ifdef CONFIG_SWIOTLB
	if (memblock_end_of_DRAM() > max) {
		ppc_swiotlb_enable = 1;
		set_pci_dma_ops(&swiotlb_dma_ops);
		ppc_md.pci_dma_dev_setup = pci_dma_dev_setup_swiotlb;
	}
#endif

	printk(KERN_INFO "MPC85xx RDB board from Freescale Semiconductor\n");
}

static struct of_device_id __initdata mpc85xxrdb_ids[] = {
	{ .type = "soc", },
	{ .compatible = "soc", },
	{ .compatible = "simple-bus", },
	{ .compatible = "gianfar", },
	{},
};

static int __init mpc85xxrdb_publish_devices(void)
{
	return of_platform_bus_probe(NULL, mpc85xxrdb_ids, NULL);
}
machine_device_initcall(p2020_rdb, mpc85xxrdb_publish_devices);
machine_device_initcall(p1020_rdb, mpc85xxrdb_publish_devices);
machine_device_initcall(p1020_mbg, mpc85xxrdb_publish_devices);
machine_device_initcall(p1024_rdb, mpc85xxrdb_publish_devices);
machine_arch_initcall(p2020_rdb, swiotlb_setup_bus_notifier);
machine_arch_initcall(p1020_rdb, swiotlb_setup_bus_notifier);
machine_arch_initcall(p1020_mbg, swiotlb_setup_bus_notifier);
machine_arch_initcall(p1024_rdb, swiotlb_setup_bus_notifier);

/*
 * Called very early, device-tree isn't unflattened
 */
static int __init p2020_rdb_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "fsl,P2020RDB"))
		return 1;
	return 0;
}

static int __init p1020_rdb_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "fsl,P1020RDB"))
		return 1;
	return 0;
}

static int __init p1020_mbg_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "fsl,P1020MBG"))
		return 1;
	return 0;
}

static int __init p1024_rdb_probe(void)
{
	unsigned long root = of_get_flat_dt_root();

	if (of_flat_dt_is_compatible(root, "fsl,P1024RDB"))
		return 1;
	return 0;
}

define_machine(p2020_rdb) {
	.name			= "P2020 RDB",
	.probe			= p2020_rdb_probe,
	.setup_arch		= mpc85xx_rdb_setup_arch,
	.init_IRQ		= mpc85xx_rdb_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};

define_machine(p1020_rdb) {
	.name			= "P1020 RDB",
	.probe			= p1020_rdb_probe,
	.setup_arch		= mpc85xx_rdb_setup_arch,
	.init_IRQ		= mpc85xx_rdb_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};

define_machine(p1020_mbg) {
	.name			= "P1020 MBG",
	.probe			= p1020_mbg_probe,
	.setup_arch		= mpc85xx_rdb_setup_arch,
	.init_IRQ		= mpc85xx_rdb_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};

define_machine(p1024_rdb) {
	.name			= "P1024 RDB",
	.probe			= p1024_rdb_probe,
	.setup_arch		= mpc85xx_rdb_setup_arch,
	.init_IRQ		= mpc85xx_rdb_pic_init,
#ifdef CONFIG_PCI
	.pcibios_fixup_bus	= fsl_pcibios_fixup_bus,
#endif
	.get_irq		= mpic_get_irq,
	.restart		= fsl_rstcr_restart,
	.calibrate_decr		= generic_calibrate_decr,
	.progress		= udbg_progress,
};
