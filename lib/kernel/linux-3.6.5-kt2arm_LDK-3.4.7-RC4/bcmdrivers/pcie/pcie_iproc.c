/*
 * Copyright (C) 2013, Broadcom Corporation. All Rights Reserved.
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/*
* Northstar PCI-Express driver
* Only supports Root-Complex (RC) mode
*
* Notes:
* PCI Domains are being used to identify the PCIe port 1:1.
*
* Only MEM access is supported, PAX does not support IO.
*
* TODO:
*	MSI interrupts,
*	DRAM > 128 MBytes (e.g. DMA zones)
*/

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/bug.h>
#include <linux/delay.h>
#include <linux/pci.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/msi.h>

#include <mach/memory.h>
#include <mach/io_map.h>

#include <asm/mach/pci.h>
#include <asm/sizes.h>

#include <linux/version.h>
#include <asm/mach/irq.h>
#include "pcie_aer_iproc.h"

#define pci_std_swizzle pci_common_swizzle
#define SZ_32M 0x02000000
#define SZ_48M 0x03000000


#define PCI_AER_RC 0



/*
 * Register offset definitions
 */

#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
#define MAX_EXT_IRQS		32
#else
#define MAX_EXT_IRQS		80
#endif
#define NS_PCI_DEBUG		0
#define	SOC_PCIE_CONTROL	0x000	/* a.k.a. CLK_CONTROL reg */
#define	SOC_PCIE_PM_STATUS	0x008
#define	SOC_PCIE_PM_CONTROL	0x00c	/* in EP mode only ! */

#define	SOC_PCIE_EXT_CFG_ADDR	0x120
#define	SOC_PCIE_EXT_CFG_DATA	0x124
#define	SOC_PCIE_CFG_ADDR	0x1f8
#define	SOC_PCIE_CFG_DATA	0x1fc

#define	SOC_PCIE_SYS_RC_INTX_EN			0x330
#define	SOC_PCIE_SYS_RC_INTX_CSR		0x334
#define	SOC_PCIE_SYS_HOST_INTR_EN		0x344
#define	SOC_PCIE_SYS_HOST_INTR_CSR		0x348

#define SOC_PCIE_MSI_EQ_ADDRESS			0x200
#define SOC_PCIE_MSI_MSI_ADDRESS		0x204
#define SOC_PCIE_MSI_INTS_ENABLE		0x208
#define SOC_PCIE_MSI_EQS_ENABLE			0x210
#define SOC_PCIE_MSI_HEAD_PTR			0x250
#define SOC_PCIE_MSI_TAIL_PTR			0x254
#define SOC_PCIE_MSI_IMAP_0_ADDRESS		0xC10
#define SOC_PCIE_MSI_IARR_0_ADDRESS		0xD00
#define SOC_PCIE_MSI_NUM_EVENT_QUEUES	0x6
#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
#define SOC_PCI_MSI_LAST_ASSIGNED_MSI	127
#else
#define SOC_PCI_MSI_LAST_ASSIGNED_MSI	134
#endif
#define	SOC_PCIE_HDR_OFF	0x400	/* 256 bytes per function */

/* 32-bit 4KB in-bound mapping windows for Function 0..3, n=0..7 */
#define	SOC_PCIE_SYS_IMAP0(f,n)		(0xc00+((f)<<9)((n)<<2)) 
/* 64-bit in-bound mapping windows for func 0..3 */
#define	SOC_PCIE_SYS_IMAP1(f)		(0xc80+((f)<<3))
#define	SOC_PCIE_SYS_IMAP2(f)		(0xcc0+((f)<<3))
/* 64-bit in-bound address range n=0..2 */
#define	SOC_PCIE_SYS_IARR(n)		(0xd00+((n)<<3))
/* 64-bit out-bound address filter n=0..2 */
#define	SOC_PCIE_SYS_OARR(n)		(0xd20+((n)<<3))
/* 64-bit out-bound mapping windows n=0..2 */
#define	SOC_PCIE_SYS_OMAP(n)		(0xd40+((n)<<3))

#ifdef	__nonexistent_regs_
#define	SOC_PCIE_MDIO_CONTROL	0x128
#define	SOC_PCIE_MDIO_RD_DATA	0x12c
#define	SOC_PCIE_MDIO_WR_DATA	0x130
#define	SOC_PCIE_CLK_STAT	0x1e0 
#endif

#define MSI_IARRO_PHYS_ADDRESS 0x20000000

#define PCI_MAX_BUS		4
#define pcieHostPrimSecBusNum		(0x00000100 | (PCI_MAX_BUS<<16))
#define pcieSwitchPrimSecBusNum		(0x00000201 | (PCI_MAX_BUS<<16))


#ifndef CONFIG_PCI_MSI
void write_msi_msg(unsigned int irq, struct msi_msg *msg) {}
#endif

#ifdef	CONFIG_PCI

/*
 * Forward declarations
 */
static int soc_pci_setup(int nr, struct pci_sys_data *sys);
static struct pci_bus * soc_pci_scan_bus(int nr, struct pci_sys_data *sys);
static int soc_pcie_map_irq(struct pci_dev *dev, u8 slot, u8 pin);
static int soc_pci_read_config(struct pci_bus *bus, unsigned int devfn,
                                   int where, int size, u32 *val);
static int soc_pci_write_config(struct pci_bus *bus, unsigned int devfn,
                                    int where, int size, u32 val);

#ifndef	CONFIG_PCI_DOMAINS
#error	CONFIG_PCI_DOMAINS is required
#endif


/*
 * PCIe host controller registers
 * one entry per port
 */
#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
#define MAX_PCI_INTFS 3
#else
#define MAX_PCI_INTFS 2
#endif
/* this is for northstar, co-star and northstar+ */
static struct resource soc_pcie_regs[MAX_PCI_INTFS] = {
	{
	.name = "pcie0",
	.start = 0x18012000,
	.end   = 0x18012fff,
	.flags = IORESOURCE_MEM,
	},
	{
	.name = "pcie1",
	.start = 0x18013000,
	.end   = 0x18013fff,
	.flags = IORESOURCE_MEM,
	},
#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
	{
	.name = "pcie2",
	.start = 0x18014000,
	.end   = 0x18014fff,
	.flags = IORESOURCE_MEM,
	},
#endif
};

static struct resource soc_pcie_owin[MAX_PCI_INTFS] = {
	{
	.name = "PCIe Outbound Window, Port 0",
	.start = 0x08000000,
	.end =   0x08000000 + SZ_128M - 1,
	.flags = IORESOURCE_MEM,
	},
	{
	.name = "PCIe Outbound Window, Port 1",
	.start = 0x40000000,
	.end =   0x40000000 + SZ_128M - 1,
	.flags = IORESOURCE_MEM,
	},
#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
	{
	.name = "PCIe Outbound Window, Port 2",
	.start = 0x48000000,
	.end =   0x48000000 + SZ_128M - 1,
	.flags = IORESOURCE_MEM,
	},
#endif
};

/*
 * Per port control structure
 */
static struct soc_pcie_port {
	struct resource * regs_res ;
	struct resource * owin_res ;
	void * __iomem reg_base;
	unsigned short irqs[6];
	struct hw_pci hw_pci ;

	bool	enable;
	bool	link;
	bool	isSwitch;
	bool	port1Active;
	bool	port2Active;
	int		lastAssignedMSI;
	unsigned int msi_EQ_Address;
	unsigned int msi_MSI_Address;
	struct resource msi_IARR0_Address;
	unsigned int oldMSIIntVal;
} soc_pcie_ports[MAX_PCI_INTFS] = {
	{
	.regs_res = & soc_pcie_regs[0],
	.owin_res = & soc_pcie_owin[0],
#if defined(CONFIG_MACH_HX4)
	.irqs = {214, 215, 216, 217, 218, 219},
#elif defined(CONFIG_MACH_HR2)
	.irqs = {214, 215, 216, 217, 218, 219},
#elif defined(CONFIG_MACH_KT2)
	.irqs = {214, 215, 216, 217, 218, 219},
#elif defined(CONFIG_MACH_NS)
	.irqs = {159, 160, 161, 162, 163, 164},
#elif defined(CONFIG_MACH_NSP)
	.irqs = {159, 160, 161, 162, 163, 164},
#endif
	.hw_pci = {
		.domain 	= 0,
		.swizzle 	= pci_std_swizzle,
		.nr_controllers = 1,
		.setup 		= soc_pci_setup,
		.scan 		= soc_pci_scan_bus,
		.map_irq 	= (void *) soc_pcie_map_irq,
		},
	.enable = 1,
	.isSwitch = 0,
	.port1Active = 0,
	.port2Active = 0,
	.lastAssignedMSI = SOC_PCI_MSI_LAST_ASSIGNED_MSI,
	.msi_IARR0_Address.start = MSI_IARRO_PHYS_ADDRESS,
	.msi_IARR0_Address.end = MSI_IARRO_PHYS_ADDRESS + 4096 - 1,
	.msi_IARR0_Address.flags = IORESOURCE_MEM,

	},
	{
	.regs_res = & soc_pcie_regs[1],
	.owin_res = & soc_pcie_owin[1],
#if defined(CONFIG_MACH_HX4)
	.irqs = {220, 221, 222, 222, 224, 225},
#elif defined(CONFIG_MACH_HR2)
	.irqs = {220, 221, 222, 222, 224, 225},
#elif defined(CONFIG_MACH_KT2)
	.irqs = {220, 221, 222, 222, 224, 225},
#elif defined(CONFIG_MACH_NS)
	.irqs = {165, 166, 167, 168, 169, 170},
#elif defined(CONFIG_MACH_NSP)
	.irqs = {165, 166, 167, 168, 169, 170},
#endif
	.hw_pci = {
		.domain 	= 1,
		.swizzle 	= pci_std_swizzle,
		.nr_controllers = 1,
		.setup 		= soc_pci_setup,
		.scan 		= soc_pci_scan_bus,
		.map_irq 	= (void *) soc_pcie_map_irq,
		},
	.enable = 1,
	.isSwitch = 0,
	.port1Active = 0,
	.port2Active = 0,
	.lastAssignedMSI = SOC_PCI_MSI_LAST_ASSIGNED_MSI + SOC_PCIE_MSI_NUM_EVENT_QUEUES,
	.msi_IARR0_Address.start = MSI_IARRO_PHYS_ADDRESS + 0x01000000,
	.msi_IARR0_Address.end = MSI_IARRO_PHYS_ADDRESS + 0x01000000 + 4096 - 1,
	.msi_IARR0_Address.flags = IORESOURCE_MEM,
	},
#if defined(CONFIG_MACH_NS) || defined(CONFIG_MACH_NSP)
	{
	.regs_res = & soc_pcie_regs[2],
	.owin_res = & soc_pcie_owin[2],
	.irqs = {171, 172, 173, 174, 175, 176},
	.hw_pci = {
		.domain 	= 2,
		.swizzle 	= pci_std_swizzle,
		.nr_controllers = 1,
		.setup 		= soc_pci_setup,
		.scan 		= soc_pci_scan_bus,
		.map_irq 	= (void *) soc_pcie_map_irq,
		},
	.enable = 1,
	.isSwitch = 0,
	.port1Active = 0,
	.port2Active = 0,
	.lastAssignedMSI = 139,
	.msi_IARR0_Address.start = MSI_IARRO_PHYS_ADDRESS + 0x02000000,
	.msi_IARR0_Address.end = MSI_IARRO_PHYS_ADDRESS + 0x02000000 + 4096 - 1,
	.msi_IARR0_Address.flags = IORESOURCE_MEM,
	}
#endif
	};



/*
 * Methods for accessing configuration registers
 */
static struct pci_ops soc_pcie_ops = {
        .read = soc_pci_read_config,
        .write = soc_pci_write_config,
};

static struct soc_pcie_port * 
	soc_pcie_sysdata2port( struct pci_sys_data * sysdata )
{
	unsigned port;

	port = sysdata->domain;
	BUG_ON( port >= ARRAY_SIZE( soc_pcie_ports ));
	return & soc_pcie_ports[ port ];
}

static struct soc_pcie_port * soc_pcie_pdev2port( struct pci_dev *pdev )
{
	return soc_pcie_sysdata2port( pdev->sysdata );
}

static struct soc_pcie_port * soc_pcie_bus2port( struct pci_bus * bus )
{
	return soc_pcie_sysdata2port( bus->sysdata );
}

static struct pci_bus *soc_pci_scan_bus(int nr, struct pci_sys_data *sys)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0)
        return pci_scan_bus(sys->busnr, &soc_pcie_ops, sys);
#else
		return pci_scan_root_bus(NULL, sys->busnr, &soc_pcie_ops, sys,
					&sys->resources);
#endif
}

static int soc_pcie_map_irq(struct pci_dev *pdev, u8 slot, u8 pin)
{
        struct soc_pcie_port *port = soc_pcie_pdev2port(pdev);
        int irq;

        irq = port->irqs[4];	/* All INTx share int src 5, last per port */

        printk("PCIe map irq: %04d:%02x:%02x.%02x slot %d, pin %d, irq: %d\n",
                pci_domain_nr(pdev->bus), 
		pdev->bus->number, 
		PCI_SLOT(pdev->devfn),
                PCI_FUNC(pdev->devfn), 
		slot, pin, irq);

        return irq;
}

static void __iomem * soc_pci_cfg_base(struct pci_bus *bus,
                                  unsigned int devfn, int where)
{
    struct soc_pcie_port *port = soc_pcie_bus2port(bus);
    int busno = bus->number;
    int slot = PCI_SLOT(devfn);
    int fn  = PCI_FUNC(devfn);
    void __iomem *base;
    int offset;
    int type;
	u32 addr_reg ;

	base = port->reg_base ;

    /* If there is no link, just show the PCI bridge. */
    if (!port->link && (busno > 0 || slot > 0))
            return NULL;
    /*
     */
	if (busno == 0) {
                if (slot >= 1)
                        return NULL;
                type = slot;
		__raw_writel( where & 0xffc, base + SOC_PCIE_EXT_CFG_ADDR );
		offset = SOC_PCIE_EXT_CFG_DATA;
	} else {
        type = 1;
		if (fn > 1)
			return NULL;
		addr_reg = 	(busno & 0xff) << 20 |
				(slot << 15) |
				(fn << 12)   |
				(where & 0xffc) |
				(type & 0x3);
 
		__raw_writel( addr_reg, base + SOC_PCIE_CFG_ADDR );
		offset =  SOC_PCIE_CFG_DATA ;
    }

    return base + offset;
}

void pcieSwitchInit( struct pci_bus *bus, unsigned int devfn)
{
    struct soc_pcie_port *port = soc_pcie_bus2port(bus);
	u32 	dRead = 0;
	u32		bm = 0;

    soc_pci_read_config(bus, devfn, 0x100, 4, &dRead);

	printk("PCIE: Doing PLX switch Init...Test Read = %08x\n",(unsigned int)dRead);

	//Debug control register.
    soc_pci_read_config(bus, devfn, 0x1dc, 4, &dRead);

	dRead &= ~(1<<22);

    soc_pci_write_config(bus, devfn, 0x1dc, 4, dRead);

	//Set GPIO enables.
    soc_pci_read_config(bus, devfn, 0x62c, 4, &dRead);

	printk("PCIE: Doing PLX switch Init...GPIO Read = %08x\n",(unsigned int)dRead);

	dRead &= ~((1<<0)|(1<<1)|(1<<3));
	dRead |= ((1<<4)|(1<<5)|(1<<7));

    soc_pci_write_config(bus, devfn, 0x62c, 4, dRead);

	mdelay(50);
	dRead |= ((1<<0)|(1<<1));

    soc_pci_write_config(bus, devfn, 0x62c, 4, dRead);

    soc_pci_read_config(bus, devfn, 0x4, 2, &bm);
#if NS_PCI_DEBUG
	printk("bus master: %08x\n", bm);
#endif
	bm |= 0x06;
	soc_pci_write_config(bus, devfn, 0x4,2, bm);
	bm = 0;
    soc_pci_read_config(bus, devfn, 0x4, 2, &bm);
	printk("bus master after: %08x\n", bm);
	bm =0;
	//Bus 1 if the upstream port of the switch. Bus 2 has the two downstream ports, one on each device number.
	if(bus->number == 1)
	{
		soc_pci_write_config(bus, devfn, 0x18, 4, pcieSwitchPrimSecBusNum);

		//TODO: We need to scan all outgoing windows, to look for a base limit pair for this register.
		//npciConfigOutLong(instance, busNo, deviceNo, 0, 0x20,0xcff0c000);
		/* MEM_BASE, MEM_LIM require 1MB alignment */
		BUG_ON( (port->owin_res->start   >> 16) & 0xf );
		soc_pci_write_config(bus, devfn, PCI_MEMORY_BASE, 2,  
		port->owin_res->start   >> 16 );
		BUG_ON(((port->owin_res->start + SZ_32M) >> 16 ) & 0xf );
		soc_pci_write_config(bus, devfn, PCI_MEMORY_LIMIT, 2, 
		(port->owin_res->start + SZ_32M) >> 16 );

	}
	else if(bus->number == 2)
	{
		//TODO: I need to fix these hard coded addresses.
		if(devfn == 0x8)
		{
			soc_pci_write_config(bus, devfn, 0x18, 4, (0x00000000 | ((bus->number+1)<<16) | ((bus->number+1)<<8) | bus->number));
			BUG_ON( ((port->owin_res->start + SZ_48M)   >> 16) & 0xf );
			soc_pci_write_config(bus, devfn, PCI_MEMORY_BASE, 2, 
			(port->owin_res->start + SZ_48M)   >> 16 );
			BUG_ON(((port->owin_res->start + SZ_48M + SZ_32M) >> 16 ) & 0xf );
			soc_pci_write_config(bus, devfn, PCI_MEMORY_LIMIT, 2, 
			(port->owin_res->start + SZ_48M + SZ_32M) >> 16 );
			soc_pci_read_config(bus, devfn, 0x7A, 2, &bm);
			if (bm & PCI_EXP_LNKSTA_DLLLA)
				port->port1Active = 1;
			printk("bm = %04x\n devfn = = %08x, bus = %08x\n", bm, devfn, bus->number); 
		}
		else if(devfn == 0x10)
		{
			soc_pci_write_config(bus, devfn, 0x18, 4, (0x00000000 | ((bus->number+2)<<16) | ((bus->number+2)<<8) | bus->number));
			BUG_ON( ((port->owin_res->start + (SZ_48M * 2 ))  >> 16) & 0xf );
			soc_pci_write_config(bus, devfn, PCI_MEMORY_BASE, 2,  
			(port->owin_res->start  + (SZ_48M * 2 ))   >> 16 );
			BUG_ON(((port->owin_res->start + (SZ_48M * 2 ) + SZ_32M) >> 16 ) & 0xf );
			soc_pci_write_config(bus, devfn, PCI_MEMORY_LIMIT, 2,  
			(port->owin_res->start + (SZ_48M * 2 ) + SZ_32M) >> 16 );
			soc_pci_read_config(bus, devfn, 0x7A, 2, &bm);
			if (bm & PCI_EXP_LNKSTA_DLLLA)
				port->port2Active = 1;
			printk("bm = %04x\n devfn = = %08x, bus = %08x\n", bm, devfn, bus->number); 
		}
	}
}

static int soc_pci_read_config(struct pci_bus *bus, unsigned int devfn,
                                   int where, int size, u32 *val)
{
    void __iomem *base;
	u32 data_reg;
    struct soc_pcie_port *port = soc_pcie_bus2port(bus);

	if ((bus->number > 4))
	{
		*val = ~0UL;
		return PCIBIOS_SUCCESSFUL;
	}
	if (port->isSwitch == 1)
	{
		if (bus->number == 2)
		{
			if (!((devfn == 0x8) || (devfn == 0x10)))
			{
				*val = ~0UL;
				return PCIBIOS_SUCCESSFUL;
			}
		}
		else if ((bus->number == 3) || (bus->number == 4))
		{
			if (devfn != 0)
			{
				*val = ~0UL;
				return PCIBIOS_SUCCESSFUL;
			}
			else if ((bus->number == 3) && (port->port1Active == 0))
			{
				*val = ~0UL;
				return PCIBIOS_SUCCESSFUL;
			}
			else if ((bus->number == 4) && (port->port2Active == 0))
			{
				*val = ~0UL;
				return PCIBIOS_SUCCESSFUL;
			}
		}
	}
	base = soc_pci_cfg_base(bus, devfn, where);

    if (base == NULL )
	{
            *val = ~0UL;
            return PCIBIOS_SUCCESSFUL;
	}

#if NS_PCI_DEBUG
	printk("PCI-E: R: bus %08x, where %08x, devfn %08x\n", bus->number, where, devfn);
#endif
	data_reg = __raw_readl( base );
#if NS_PCI_DEBUG
	printk("PCI-E: R: data_reg %08x\n", data_reg);
#endif


	if ((bus->number == 1) && (port->isSwitch == 0) && (where == 0) && (((data_reg >> 16) & 0x0000FFFF) == 0x00008603))
	{
		pcieSwitchInit( bus, devfn);
		port->isSwitch = 1;
	}
	else if ((bus->number == 1) && (port->isSwitch == 0) && (where == 0) && (((data_reg >> 16) & 0x0000FFFF) == 0x00008617))
	{
		pcieSwitchInit( bus, devfn);
		port->isSwitch = 1;
	}

	if ((bus->number == 2) && (port->isSwitch == 1) && (where == 0) &&(((data_reg >> 16) & 0x0000FFFF) == 0x00008603))
		pcieSwitchInit(bus, devfn);
	else if ((bus->number == 2) && (port->isSwitch == 1) && (where == 0) &&(((data_reg >> 16) & 0x0000FFFF) == 0x00008617))
		pcieSwitchInit(bus, devfn);

	/* HEADER_TYPE=00 indicates the port in EP mode */

	data_reg = (data_reg) >> ((where & 3) * 8);
	*val = data_reg;
    return PCIBIOS_SUCCESSFUL;
}

static int soc_pci_write_config(struct pci_bus *bus, unsigned int devfn,
                                    int where, int size, u32 val)
{
    void __iomem *base;
	u32  data_reg ;
	int saveWhere;
    struct soc_pcie_port *port = soc_pcie_bus2port(bus);

	saveWhere = where;
	if ((bus->number > 4))
	{
		return PCIBIOS_SUCCESSFUL;
	}
	if ((bus->number == 2) && (port->isSwitch == 1 ))
	{
		if (!((devfn == 0x8) || (devfn == 0x10)))
		{
			return PCIBIOS_SUCCESSFUL;
		}
	}
	else if ((bus->number == 3) && (port->isSwitch == 1))
	{
		if (devfn != 0)
			return PCIBIOS_SUCCESSFUL;
	}
	else if ((bus->number == 4) && (port->isSwitch == 1))
	{
		if (devfn != 0)
		{
			return PCIBIOS_SUCCESSFUL;
		}
	}
	base = soc_pci_cfg_base(bus, devfn, where);

    if (base == NULL)
	{
            return PCIBIOS_SUCCESSFUL;
	}

	if( size < 4 ){
		where = (where & 3) * 8;

#if NS_PCI_DEBUG
		printk("PCI-E: WR: bus %08x, where %08x, devfn %08x, size: %08x\n", bus->number, saveWhere, devfn, size);
#endif
		data_reg = __raw_readl( base );
#if NS_PCI_DEBUG
		printk("PCI-E: WR: after data_reg %08x\n", data_reg);
#endif

		if (size == 1)
			data_reg &= ~(0xff << where);
		else
			data_reg &= ~(0xffff << where);
		data_reg |= (val << where);
	}
	else{
		data_reg = val;
	}

#if NS_PCI_DEBUG
	printk("PCI-E: W: bus %08x, where %08x, devfn %08x, data_reg %08x, size: %08x\n", bus->number, saveWhere, devfn, data_reg, size);
#endif
	__raw_writel( data_reg, base );
#if NS_PCI_DEBUG
	printk("PCI-E: W: after\n");
#endif

    return PCIBIOS_SUCCESSFUL;
}

static int soc_pci_setup(int nr, struct pci_sys_data *sys)
{
        struct soc_pcie_port *port = soc_pcie_sysdata2port(sys);

	BUG_ON( request_resource( &iomem_resource, port->owin_res ));

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0)
	sys->resource[0] = port->owin_res ;
	//sys->private_data = port;
#else
	pci_add_resource(&sys->resources, port->owin_res);
#endif

        return 1;
}

/*
 * Check link status, return 0 if link is up in RC mode,
 * otherwise return non-zero
 */
static int __init noinline soc_pcie_check_link(struct soc_pcie_port * port)
{
        u32 devfn = 0;
	u16 pos, tmp16;
	u8 nlw, tmp8;
	u32 tmp32;

        struct pci_sys_data sd = {
                .domain = port->hw_pci.domain,
        };
        struct pci_bus bus = {
                .number = 0,
                .ops = &soc_pcie_ops,
                .sysdata = &sd,
        };

	if( ! port->enable )
		return -EINVAL;

	/* See if the port is in EP mode, indicated by header type 00 */
        pci_bus_read_config_byte(&bus, devfn, PCI_HEADER_TYPE, &tmp8);
	if( tmp8 != PCI_HEADER_TYPE_BRIDGE ) {
		pr_info("PCIe port %d in End-Point mode - ignored\n",
			port->hw_pci.domain );
		return -ENODEV;
	}

	/* 
	* Under RC mode, write to function specific register 0x43c, to change
	* the CLASS code in configuration space 
	*/
	pci_bus_read_config_dword(&bus, devfn, 0x43c, &tmp32);
	tmp32 = (tmp32 & 0xff0000ff) | (PCI_CLASS_BRIDGE_PCI << 8);
	pci_bus_write_config_dword(&bus, devfn, 0x43c, tmp32);
	/* 
	* After this modification, the CLASS code in configuration space would be
	* read as PCI_CLASS_BRIDGE_PCI(0x0604) instead of network interface(0x0200) 
	*/


	/* NS PAX only changes NLW field when card is present */
        pos = pci_bus_find_capability(&bus, devfn, PCI_CAP_ID_EXP);
        pci_bus_read_config_word(&bus, devfn, pos + PCI_EXP_LNKSTA, &tmp16);

	printk("PCIE%d: LINKSTA reg %#x val %#x\n", port->hw_pci.domain,
		pos+PCI_EXP_LNKSTA, tmp16 );

	nlw = (tmp16 & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT ;
	port->link = tmp16 & PCI_EXP_LNKSTA_DLLLA ;

	if( nlw != 0 ) port->link = 1;

	for( ; pos < 0x100; pos += 2 )
		{
        	pci_bus_read_config_word(&bus, devfn, pos , &tmp16);
			if( tmp16 ) printk("reg[%#x]=%#x, ", pos , tmp16 );
		}
	if (nlw == 0)
	{
		/* try gen 1 */
		pci_bus_read_config_dword(&bus, devfn, 0xdc, &tmp32);
#if NS_PCI_DEBUG
		printk("\nLink status control 2 register 0xdc: %08x\n", tmp32);
#endif
		if ((tmp32 & 0xf) == 2)
		{
			tmp32 &= 0xfffffff0;
			tmp32 |= 0x1;
			pci_bus_write_config_dword(&bus, devfn, 0xdc, tmp32);
			pci_bus_read_config_dword(&bus, devfn, 0xdc, &tmp32);
			mdelay(100);
		/* NS PAX only changes NLW field when card is present */
			pos = pci_bus_find_capability(&bus, devfn, PCI_CAP_ID_EXP);
			pci_bus_read_config_word(&bus, devfn, pos + PCI_EXP_LNKSTA, &tmp16);
			nlw = (tmp16 & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT ;
			port->link = tmp16 & PCI_EXP_LNKSTA_DLLLA ;
#if NS_PCI_DEBUG
			printk("Link status control 2 register 0xdc: %08x, nlw: %08x, port->link: %08x\n", tmp32, nlw, port->link);
#endif
			if( nlw != 0 ) port->link = 1;
		}
	}
	printk("PCIE%d link=%d\n", port->hw_pci.domain,  port->link );

	return( (port->link)? 0: -ENOSYS );
}

/*
 * Initializte the PCIe controller
 */
static void __init soc_pcie_hw_init(struct soc_pcie_port * port)
{
	/* Turn-on Root-Complex (RC) mode, from reset defailt of EP */

	/* The mode is set by straps, can be overwritten via DMU
	   register <cru_straps_control> bit 5, "1" means RC
	 */

	/* Send a downstream reset */
	__raw_writel( 0x3, port->reg_base + SOC_PCIE_CONTROL);
	udelay(250);
	__raw_writel( 0x1, port->reg_base + SOC_PCIE_CONTROL);
	mdelay(250);

	/* TBD: take care of PM, check we're on */
}

/*
 * Setup the address translation
 */
static void __init soc_pcie_map_init(struct soc_pcie_port * port)
{
/*
* Disabling the address translation won't disable PCI memory read/write function,
* it just won't do address translation before or after PCIE memory transaction.
*
* As a PCIE RC driver, the OMAP/OARR could be disabled since we set OMAP/OARR
* the same value of PCIE window range. This takes the same effect as no address 
* translation.
*
* As a PCIE RC drive, the IMAP/IARR could be disabled and let the device decide which
* address to write to. After all we have no idea about the device's intention.
*
* It seems only PCIE EP mode is required to set the OMAP/OARR/IMAP/IARR function.
*
* @ Setting of IMAP/IARR
* 1. The setting of IMAP/IARR below is reversed, so the IMAP function is disabled 
* actually(the valid bit is set to size and becomes 0). 
* 2. We should take notice that the address shall be set to size-aligned, or the address
* translation might not be what we expect. For example, if the size is set to 128M, 
* the address should be set to 128M aligned.
*/

}

/*
 * Setup PCIE Host bridge
 */
static void __maybe_unused __init noinline soc_pcie_bridge_init(struct soc_pcie_port * port)
{
        u32 devfn = 0;
        u8 tmp8;
	u16 tmp16;

	/* Fake <bus> object */
        struct pci_sys_data sd = {
                .domain = port->hw_pci.domain,
        };
        struct pci_bus bus = {
                .number = 0,
                .ops = &soc_pcie_ops,
                .sysdata = &sd,
        };


	/*
	* PCI_PRIMARY_BUS, PCI_SECONDARY_BUS, and PCI_SUBORDINATE_BUS would be
	* set in ARM PCI enumeration process(pci_common_init). If connected to one 
	* PCIe device only, the PCI_PRIMARY_BUS, PCI_SECONDARY_BUS, and
	* PCI_SUBORDINATE_BUS would be set to 0, 1, 1 respectively.
	*/
        pci_bus_write_config_byte(&bus, devfn, PCI_PRIMARY_BUS, 0);
        pci_bus_write_config_byte(&bus, devfn, PCI_SECONDARY_BUS, 1);
        pci_bus_write_config_byte(&bus, devfn, PCI_SUBORDINATE_BUS, 4);

        pci_bus_read_config_byte(&bus, devfn, PCI_PRIMARY_BUS, &tmp8);
        pci_bus_read_config_byte(&bus, devfn, PCI_SECONDARY_BUS, &tmp8);
        pci_bus_read_config_byte(&bus, devfn, PCI_SUBORDINATE_BUS, &tmp8);

	/* MEM_BASE, MEM_LIM require 1MB alignment */
	BUG_ON( (port->owin_res->start   >> 16) & 0xf );
	printk("%s: membase %#x memlimit %#x\n", __FUNCTION__,
		port->owin_res->start, port->owin_res->end+1);
        pci_bus_write_config_word(&bus, devfn, PCI_MEMORY_BASE, 
		port->owin_res->start   >> 16 );
	BUG_ON(((port->owin_res->end+1) >> 16 ) & 0xf );
        pci_bus_write_config_word(&bus, devfn, PCI_MEMORY_LIMIT, 
		(port->owin_res->end) >> 16 );
	/*
	* Set resource->end to MEMORY_LIMIT instead of resource->end+1, 
	* PCI bridge spec depicted that the upper 12 bits of both MEMORY_BASE and 
	* MEMORY_LIMIT would be take as 0x###00000 and 0x###FFFFF respectively.
	* ARM PCI enumeration process(pci_common_init) would set MEMORY_BASE and
	* MEMORY_LIMIT(or PREF_MEMORY_BASE and PREF_MEMORY_LIMIT) to the total
	* window size that's assigned to the devices.
	*/


	/* These registers are not supported on the NS */
        pci_bus_write_config_word(&bus, devfn, PCI_IO_BASE_UPPER16, 0);
        pci_bus_write_config_word(&bus, devfn, PCI_IO_LIMIT_UPPER16, 0);

	/* Force class to that of a Bridge */
        pci_bus_write_config_word(&bus, devfn, PCI_CLASS_DEVICE,
		PCI_CLASS_BRIDGE_PCI);

        pci_bus_read_config_word(&bus, devfn, PCI_CLASS_DEVICE, &tmp16);
        pci_bus_read_config_word(&bus, devfn, PCI_MEMORY_BASE, &tmp16);
        pci_bus_read_config_word(&bus, devfn, PCI_MEMORY_LIMIT, &tmp16);
	
}

void toggle_msi_int(int irq, int toggle)
{
    struct soc_pcie_port *port = NULL;
	int i;
#if NS_PCI_DEBUG 
	printk("%s: %d entry %d\n", __FUNCTION__, __LINE__, irq);
#endif
	for (i = 0; i <  ARRAY_SIZE(soc_pcie_ports); i++)
	{
		port = &soc_pcie_ports[i];
		/* Enable EQ interrupts until we finish this handling */
		if (port != NULL)
		{
			if ((irq >= port->irqs[0]) && (irq <= port->irqs[5]))
			{
				if (toggle)
				{
#if NS_PCI_DEBUG 
					printk("%s: %d %08x\n", __FUNCTION__, __LINE__, port->oldMSIIntVal);
#endif
					__raw_writel(port->oldMSIIntVal, port->reg_base+SOC_PCIE_MSI_INTS_ENABLE);
					break;
				}
				else
				{
#if NS_PCI_DEBUG 
					printk("%s: %d %08x\n", __FUNCTION__, __LINE__, port->oldMSIIntVal);
#endif
					__raw_writel(0, port->reg_base+SOC_PCIE_MSI_INTS_ENABLE);
					break;
				}
			}
			else
			{
#if NS_PCI_DEBUG 
				printk("%s: %d irq match not found %d\n", __FUNCTION__, __LINE__, irq);
#endif
				port= NULL;
			}
		}

	}
}

static void bcm_msi_unmask_msi_irq(struct irq_data *d)
{
#if NS_PCI_DEBUG 
	printk("%s: %d entry %d\n", __FUNCTION__, __LINE__, irq);
#endif
	if (d != NULL);
	{
#if NS_PCI_DEBUG 
		printk("%s: %d %d\n", __FUNCTION__, __LINE__, irq);
#endif
		unmask_msi_irq(d);
	}
#if NS_PCI_DEBUG 
	printk("%s: %d exit %d\n", __FUNCTION__, __LINE__, irq);
#endif
	return; 
}

static void bcm_msi_mask_msi_irq(struct irq_data *d)
{
#if NS_PCI_DEBUG 
	printk("%s: %d entry %d\n", __FUNCTION__, __LINE__, irq);
#endif
	if (d != NULL);
	{
#if NS_PCI_DEBUG 
		printk("%s: %d %d\n", __FUNCTION__, __LINE__, irq);
#endif
		mask_msi_irq(d);
	}
#if NS_PCI_DEBUG 
	printk("%s: %d exit %d\n", __FUNCTION__, __LINE__, irq);
#endif
	return;
}

static irqreturn_t soc_pcie_msi_handler(unsigned int irq, void *ptr)
{
	unsigned int i, j = SOC_PCIE_MSI_HEAD_PTR, k = SOC_PCIE_MSI_TAIL_PTR;
	struct soc_pcie_port *port = NULL;
	unsigned int headPtr, tailPtr, numEntries,localIrq = 0;
	
#if NS_PCI_DEBUG 
	printk("%s: %d irq : %d entry\n", __FUNCTION__, __LINE__, irq);
#endif

	/* turn off msi event queue interrupts */

	toggle_msi_int(irq, 0);

	for (i = 0; i <  ARRAY_SIZE(soc_pcie_ports); i++)
	{
		port = &soc_pcie_ports[i];
		if ((irq >= port->irqs[0]) && (irq <= port->irqs[5]))
			break;
		port = NULL;
	}

	if (port == NULL)
	{
		printk("%s: %d irq %d can not be seviced by any pci-e controller available\n", __FUNCTION__, __LINE__, irq);
		goto int_clup;
	}

	j += ((irq - port->irqs[0]) * 8);
	k += ((irq - port->irqs[0]) * 8);
	localIrq = irq - MAX_EXT_IRQS;

	/* handle the msi event message and invoke device interrupt handler that generated the MSI event */
	do
	{
		headPtr = __raw_readl(port->reg_base+j);
		headPtr &= 0x3f;
		tailPtr = __raw_readl(port->reg_base+k);
		tailPtr &= 0x3f;
		numEntries = (tailPtr < headPtr) ? (64+(tailPtr - headPtr)) : (tailPtr-headPtr);
		if (numEntries == 0)
		{
#if NS_PCI_DEBUG 
			printk("%s: %d queue empty\n", __FUNCTION__, __LINE__);
#endif
			break;
		}
#if NS_PCI_DEBUG 
		printk("%s: %d %08x %08x\n", __FUNCTION__, __LINE__, localData, localIrq);
#endif
		if (headPtr == 0x3f)
			headPtr = 0;
		else
			headPtr++;
#if NS_PCI_DEBUG 
		printk("%s : %d %08x %08x\n", __FUNCTION__, __LINE__, headPtr, tailPtr);
#endif
		__raw_writel(headPtr, port->reg_base+j);
		generic_handle_irq(localIrq);
		headPtr = __raw_readl(port->reg_base+j);
		tailPtr = __raw_readl(port->reg_base+k);
#if NS_PCI_DEBUG 
		printk("%s : %d %08x %08x\n", __FUNCTION__, __LINE__, headPtr, tailPtr);
#endif
		numEntries--;
	} while (numEntries > 0);

int_clup:

	/* reenable MSI event queue intrrupts */

	toggle_msi_int(irq, 1);
#if NS_PCI_DEBUG 
	printk("%s: %d exit\n", __FUNCTION__, __LINE__);
#endif
	return IRQ_HANDLED;
}

void destroy_irq(unsigned int irq)
{
	printk("%s: %d entry\n", __FUNCTION__, __LINE__);
	dynamic_irq_cleanup(irq);
	printk("%s: %d exit\n", __FUNCTION__, __LINE__);
}

void arch_teardown_msi_irq(unsigned int irq)
{
	printk("%s: %d entry\n", __FUNCTION__, __LINE__);
	destroy_irq(irq);
	printk("%s: %d exit\n", __FUNCTION__, __LINE__);
}

static void soc_msi_nop(struct irq_data *d)
{
	return;
}


static struct irq_chip soc_iproc_msi_chip = {
	.name = "PCI-MSI",
	.irq_ack = soc_msi_nop,
#ifdef CONFIG_PCI_MSI
	.irq_enable = bcm_msi_unmask_msi_irq,
	.irq_disable = bcm_msi_mask_msi_irq,
	.irq_mask = bcm_msi_mask_msi_irq,
	.irq_unmask = bcm_msi_unmask_msi_irq,
# endif
};



int arch_setup_msi_irq(struct pci_dev *pdev, struct msi_desc *desc)
{
	int irq, ret;
	struct msi_msg msg;
    struct soc_pcie_port *port = soc_pcie_pdev2port(pdev);


#if NS_PCI_DEBUG
	printk("%s: %d irq: %d entry\n", __FUNCTION__, __LINE__, irq);
#endif
	if (port == NULL)
	{
		printk("%s: %d Bad port\n", __FUNCTION__, __LINE__);
		printk("%s: %d exit\n", __FUNCTION__, __LINE__);
		return -ENODEV;
	}
	if (port->lastAssignedMSI > (port->irqs[5] - MAX_EXT_IRQS))
	{
		printk("%s: %d no irq to assign %d %d %d\n", __FUNCTION__, __LINE__, port->lastAssignedMSI, port->irqs[5], MAX_EXT_IRQS);
		printk("%s: %d exit\n", __FUNCTION__, __LINE__);
		return -ENODEV;
	}
	if (port->lastAssignedMSI == port->irqs[4])
	{
#if PCI_AER_RC
		printk("%s: %d Bad port\n", __FUNCTION__, __LINE__);
		printk("%s: %d exit\n", __FUNCTION__, __LINE__);
		return -ENODEV;
#else
		port->lastAssignedMSI++;
#endif
	}
	irq = port->lastAssignedMSI;
	dynamic_irq_init(irq);
	port->lastAssignedMSI++;

	ret = irq_set_msi_desc(irq, desc);
#if NS_PCI_DEBUG
	printk("%s: %d irq: %d, ret: %08x \n", __FUNCTION__, __LINE__, irq, ret);
#endif
	if (!ret)
	{
		irq_set_chip_and_handler(irq, &soc_iproc_msi_chip, handle_simple_irq);
		set_irq_flags(irq, IRQF_VALID);
		msg.address_hi = 0x0;
		msg.address_lo = ( ((unsigned int)port->msi_IARR0_Address.start) | 0x4000 | ((irq + MAX_EXT_IRQS - port->irqs[0]) * 4));
#if NS_PCI_DEBUG
		printk("%s: %d irq: %d msg.address_lo: %08x irq %d, addr_base: %08x\n",
			   __FUNCTION__, __LINE__, irq, msg.address_lo, irq, (unsigned int)port->msi_IARR0_Address.start);
#endif
		msg.data = irq;
		write_msi_msg(irq, &msg);
	}
#if NS_PCI_DEBUG
	printk("%s: %d irq: %d exit\n", __FUNCTION__, __LINE__, irq);
#endif
	return 0;
}


#if PCI_AER_RC
int pcie_aer_rc_init(struct soc_pcie_port *port);
void pcie_aer_rc_status_isr(int irq, struct soc_pcie_port *port);
#endif

static int __init soc_pcie_init(void)
{
        unsigned i, j, k=SOC_PCIE_MSI_EQS_ENABLE;
		unsigned int eqRegVal = 0, intVal, *lptr;
		int ret = 0;

        for (i = 0; i < ARRAY_SIZE(soc_pcie_ports); i++)
		{
		struct soc_pcie_port * port = &soc_pcie_ports[i];
		
		/* Check if this port needs to be enabled */
		if( ! port->enable )
			continue;
		/* Setup PCIe controller registers */
		BUG_ON( request_resource( &iomem_resource, port->regs_res ));
		port->reg_base =
			ioremap( port->regs_res->start, 
			resource_size(port->regs_res) );
		BUG_ON( IS_ERR_OR_NULL(port->reg_base ));

                soc_pcie_hw_init( port );
		
		/* 
		* move soc_pcie_map_init after soc_pci_check_link function, 
		* since soc_pci_check_link function would have a check on 
		* RC or EP mode. And the soc_pcie_map_init function is trying
		* to set the mapping address under RC mode.
		*/
		/* soc_pcie_map_init( port ); */

		/*
		* Skip inactive ports -
		* will need to change this for hot-plugging
		*/
        if( soc_pcie_check_link( port ) != 0 )
		{
			continue;
		}

		soc_pcie_map_init(port); //jaime e hugo

		/* write the SYS_EQ_PAGE with 4k page */
		lptr = kzalloc(4096, GFP_KERNEL);

		if (lptr == NULL)
		{
			printk("%s: %d Event queue space allocation for MSI failed. Not configuring MSI support\n", __FUNCTION__, __LINE__);
			continue;
		}

		port->msi_EQ_Address = (unsigned int)lptr;

#if NS_PCI_DEBUG
		printk("%s: %d %08x\n", __FUNCTION__, __LINE__, port->msi_EQ_Address);
#endif
		__raw_writel(port->msi_EQ_Address, port->reg_base+SOC_PCIE_MSI_EQ_ADDRESS);


		/* write the SYS_MSI_PAGE with msi address and enable it */
		lptr = kzalloc(4096, GFP_KERNEL);

		if (lptr == NULL)
		{
			printk("%s: %d Event queue space allocation for MSI MSI page failed. Not configuring MSI support\n", __FUNCTION__, __LINE__);
			continue;
		}

		port->msi_MSI_Address = (unsigned int)lptr;

#if NS_PCI_DEBUG
		printk("%s: %d %08x\n", __FUNCTION__, __LINE__, port->msi_MSI_Address);
#endif
		__raw_writel((port->msi_MSI_Address), port->reg_base+SOC_PCIE_MSI_MSI_ADDRESS);

		/* write the imap0_4 with msi address and enable it */

		__raw_writel((port->msi_MSI_Address) | 1, port->reg_base+SOC_PCIE_MSI_IMAP_0_ADDRESS);

		/* write IARR_0 with this msi address for address translation */
#if NS_PCI_DEBUG
		printk("%s: %d addr_base: %08x\n", __FUNCTION__, __LINE__, port->msi_IARR0_Address.start);
#endif
		__raw_writel( (unsigned int)(port->msi_IARR0_Address.start) | 1, port->reg_base+SOC_PCIE_MSI_IARR_0_ADDRESS);

		/* enable event queue for MSI */

		for (j = 0; j < SOC_PCIE_MSI_NUM_EVENT_QUEUES; j++)
		{
			if (j == (SOC_PCIE_MSI_NUM_EVENT_QUEUES - 2))
				continue;
#if PCI_AER_RC
			if (j == (SOC_PCIE_MSI_NUM_EVENT_QUEUES - 1))
				continue;
#endif
			set_irq_flags(port->irqs[j], IRQF_VALID);
			ret = request_irq(port->irqs[j], (irq_handler_t)soc_pcie_msi_handler, IRQF_DISABLED | IRQF_PERCPU, "MSI-PCI-IPROC", NULL);
			if (ret != 0)
				printk("%s: %d request_irq return = %d\n", __FUNCTION__, __LINE__, ret);
		}
		for (j = 0; j < SOC_PCIE_MSI_NUM_EVENT_QUEUES; j++)
		{
#if NS_PCI_DEBUG
			printk("%s: %d k= %08x, port->regbase+k = %08x\n", __FUNCTION__, __LINE__, k, port->reg_base+k);
#endif
			eqRegVal = __raw_readl(port->reg_base+k);
			eqRegVal |= 0x3C3;
			__raw_writel(eqRegVal, port->reg_base+k);
			eqRegVal = __raw_readl(port->reg_base+k);
#if NS_PCI_DEBUG
			printk("after %s: %d k= %08x, port->regbase+k = %08x eqRegVal %08x\n", __FUNCTION__, __LINE__, k, port->reg_base+k, eqRegVal);
#endif
			k += 4;
		}
		intVal = __raw_readl(port->reg_base+SOC_PCIE_MSI_INTS_ENABLE);
		intVal |= 0x3F;
		__raw_writel(intVal, port->reg_base+SOC_PCIE_MSI_INTS_ENABLE);
		intVal = __raw_readl(port->reg_base+SOC_PCIE_MSI_INTS_ENABLE);
		printk("%s: %d %08x\n", __FUNCTION__, __LINE__, intVal);
		port->oldMSIIntVal = intVal;

		/* set the offset back to beginning for the next pci-e controller port */

		k = SOC_PCIE_MSI_EQS_ENABLE;

		soc_pcie_map_init( port );

		/*
		* What's set in soc_pcie_bridge_init function are overwritten in pci 
		* enumeration, and the value might be different to what's done in 
		* this function. 
		*/
                /* soc_pcie_bridge_init( port ); */

		/* Announce this port to ARM/PCI common code */
                pci_common_init( & port->hw_pci );

		/* Setup virtual-wire interrupts */
		__raw_writel(0xf, port->reg_base + SOC_PCIE_SYS_RC_INTX_EN );

#if PCI_AER_RC
                printk("About to enable PCI-AER-RC\n");
		if(pcie_aer_rc_init(port) != 0)
                        { printk("%s: %d pci_aer_rc_init \n", __FUNCTION__, __LINE__); }
#endif

        	}

        pci_assign_unassigned_resources();

        return 0;
}

/* AER Routines */
#if PCI_AER_RC

int pcie_aer_rc_init(struct soc_pcie_port *port)
{

    int data_reg;
    int addr_reg;
    int ret; 
    void __iomem *base = port->reg_base; 
    /* base = port->reg_base;  */

    /* Initialize the command register */
    /* write the AER address offset in cfgIndAddr */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (RC_AER_ROOT_ERR_COMMAND_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* write the value in cfgIndData */
    data_reg =  AER_CORR_INT_ENABLE | AER_NONFATAL_INT_ENABLE |AER_FATAL_INT_ENABLE;
    __raw_writel(data_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* write to Bridge control registers */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (PCI_PAXB_RC_BRIDGE_CONTROL_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    data_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );
    data_reg =  data_reg | PCI_PAXB_RC_BRIDEG_CONTROL_SEC_SERR_EN;
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (PCI_PAXB_RC_BRIDGE_CONTROL_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* write new data to the register */
    __raw_writel(data_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* write to device status control register */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (PCI_PAXB_RC_DEVICE_STATUS_CONTROL_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    data_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );
    data_reg =  data_reg | PCI_PAXB_RC_DEVICE_STATUS_CONTROL_NFATAL_ERR_REPORT_EN |
                           PCI_PAXB_RC_DEVICE_STATUS_CONTROL_FATAL_ERR_REPORT_EN;
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (PCI_PAXB_RC_DEVICE_STATUS_CONTROL_REG_OFFSET & 0x7FF);
    /* write new data to the register */
    __raw_writel(data_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* Mask the ANFM - Advisory Non Fatal Mask */
    /* write the AER address offset in cfgIndAddr */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (RC_AER_CORR_ERR_MASK_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* write the value in cfgIndData */
    data_reg =  RC_AER_CORR_ERR_MASK_ANFM;
    __raw_writel(data_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* attach the irq with system */
    ret = request_irq(port->irqs[5], (irq_handler_t)pcie_aer_rc_status_isr, IRQF_DISABLED | IRQF_PERCPU,"PCIE_AER",port);
    if (ret != 0)
    {
        printk("%s: %d request_irq return = %d\n", __FUNCTION__, __LINE__, ret);
    }
    return ret;
}/* end pcie_aer_rc_init */ 

/* check the error status if the system receives an aer interrupt */
/* void pcie_rc_aer_status_isr(struct soc_pcie_port *port) */
void pcie_aer_rc_status_isr(int irq, struct soc_pcie_port  *port)
{
    void __iomem *base;
    int addr_reg;
    int data_reg;
    int intr_reg;
    int fatal_int, nonfatal_int,multifatal_int;
    base = port->reg_base ;
  
    /* write the AER address offset in cfgIndAddr */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (RC_AER_ROOT_ERR_STATUS_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* Read from ERR_STATUS Register */
    data_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );

    /* check for FATAL/NON_FATAL interrupt */
    fatal_int = 0;
    nonfatal_int = 0;
    if((data_reg & RC_AER_STATUS_FATAL_NONFATAL_INTERRUPT) == RC_AER_STATUS_FATAL_NONFATAL_INTERRUPT)
    {
 	/* check for fatal interrupt */
   	if((data_reg & RC_AER_STATUS_FATAL_ERR_INTERRUPT) == RC_AER_STATUS_FATAL_ERR_INTERRUPT)
	{
            fatal_int = 1; 
        }
        /* check for non-fatal interrupt */
   	if((data_reg & RC_AER_STATUS_NONFATAL_ERR_INTERRUPT) == RC_AER_STATUS_NONFATAL_ERR_INTERRUPT)
        {
            nonfatal_int = 1; 
        }
   	if((data_reg & RC_AER_STATUS_MULTI_FATAL_NONFATAL_INTERRUPT) == RC_AER_STATUS_MULTI_FATAL_NONFATAL_INTERRUPT)
        {
            multifatal_int = 1; 
        }
    }

    if(fatal_int == 1)
    {
        /* check for uncorrr-fatal interrupt */
   	if((data_reg & RC_AER_STATUS_FATAL_UNCORR_ERR_INTERRUPT) == RC_AER_STATUS_FATAL_UNCORR_ERR_INTERRUPT)
	{
	    /* read un correctable interrupt register */ 
    	    /* write the AER address offset in cfgIndAddr */
	    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
                       ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
	               (RC_AER_UCORR_ERR_STATUS_REG_OFFSET & 0x7FF);
	    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
	    /* Read from ERR_STATUS Register */
	    intr_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );

            if((intr_reg & RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT)
            {
		printk(" fatal interrupt == RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT)
            {
		printk("fatal interrupt == RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT\n");
            }
           
	}
    } /* completion of fatal errors */

    if(nonfatal_int == 1)
    {
        /* check for uncorrr-fatal interrupt */
    	if((data_reg & RC_AER_STATUS_FATAL_UNCORR_ERR_INTERRUPT) == RC_AER_STATUS_FATAL_UNCORR_ERR_INTERRUPT)
	{
	    /* read un correctable interrupt register */ 
    	    /* write the AER address offset in cfgIndAddr */
	    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
                       ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
	               (RC_AER_UCORR_ERR_STATUS_REG_OFFSET & 0x7FF);
	    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
	    /* Read from ERR_STATUS Register */
	    intr_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );

            if((intr_reg & RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_DLPES_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_PTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_FCPES_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_CTS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_CAS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_UCS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_ROS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_MTLPS_INTERRUPT\n");
            }
            if((intr_reg & RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT) == RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT)
            {
		printk("non fatal interrupt == RC_AER_STATUS_UCORR_ERR_URES_INTERRUPT\n");
            }
	}
    } /* completion of non fatal errors */

    /* clear the ucorrect err status */ 
    if((fatal_int == 1) || (nonfatal_int ==1 ))
    {
        /* write the AER address offset in cfgIndAddr */
        addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
                   ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
	           (RC_AER_UCORR_ERR_STATUS_REG_OFFSET & 0x7FF);
	__raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
	/* clear the status register */
        __raw_writel(0x0 ,base + SOC_PCIE_EXT_CFG_ADDR );

    }

    /* check for correctable error */
    if((data_reg & RC_AER_STATUS_CORR_ERR_INTERRUPT) == RC_AER_STATUS_CORR_ERR_INTERRUPT)
    {
        /* read un correctable interrupt register */ 
        /* write the AER address offset in cfgIndAddr */
        addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
                   ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
	           (RC_AER_CORR_ERR_STATUS_REG_OFFSET & 0x7FF);
	__raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
	/* Read from ERR_STATUS Register */
	intr_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );
        if((intr_reg & RC_AER_STATUS_CORR_ERR_RX_INTERRUPT) == RC_AER_STATUS_CORR_ERR_RX_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_RX_INTERRUPT\n");
        }
        if((intr_reg & RC_AER_STATUS_CORR_ERR_BAD_TLP_INTERRUPT) == RC_AER_STATUS_CORR_ERR_BAD_TLP_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_BAD_TLP_INTERRUPT\n");
        }
        if((intr_reg & RC_AER_STATUS_CORR_ERR_BAD_DLLP_INTERRUPT) == RC_AER_STATUS_CORR_ERR_BAD_DLLP_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_BAD_DLLP_INTERRUPT\n");
        }
        if((intr_reg & RC_AER_STATUS_CORR_ERR_RPLAY_NUM_ROL_INTERRUPT) == RC_AER_STATUS_CORR_ERR_RPLAY_NUM_ROL_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_RPLAY_NUM_ROL_INTERRUPT\n");
        }
        if((intr_reg & RC_AER_STATUS_CORR_ERR_RPLAY_TIMER_TO_INTERRUPT) == RC_AER_STATUS_CORR_ERR_RPLAY_TIMER_TO_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_RPLAY_TIMER_TO_INTERRUPT\n");
        }
        if((intr_reg & RC_AER_STATUS_CORR_ERR_ADVSRY_INTERRUPT) == RC_AER_STATUS_CORR_ERR_ADVSRY_INTERRUPT)
        {
            printk("corr interrupt == RC_AER_STATUS_CORR_ERR_ADVSRY_INTERRUPT\n");
        }
        /* write the AER address offset in cfgIndAddr */
        addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
                   ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
	           (RC_AER_CORR_ERR_STATUS_REG_OFFSET & 0x7FF);
	__raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
	/* clear the status register */
        __raw_writel(0x0 ,base + SOC_PCIE_EXT_CFG_ADDR );
    }/* completion of correctable errors */

    /* clear the status error bits */ 
    /* write the AER address offset in cfgIndAddr */
    addr_reg = ((PCI_RC_DEVICE_FUNCTION & 0x7) << 12) |
           ((PCI_PROTOCOL_LAYER_AER & 0x3) << 10)   |
           (RC_AER_ROOT_ERR_STATUS_REG_OFFSET & 0x7FF);
    __raw_writel(addr_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
    /* Read from ERR_STATUS Register */
    data_reg = __raw_readl( base+ SOC_PCIE_EXT_CFG_ADDR );
    /* write the value in cfgIndData */
    data_reg =  data_reg & 0xFFFFFF80;
    __raw_writel(data_reg ,base + SOC_PCIE_EXT_CFG_ADDR );
}/* pcie_aer_rc_status_isr */
#endif



device_initcall(soc_pcie_init);

#endif	/* CONFIG_PCI */
