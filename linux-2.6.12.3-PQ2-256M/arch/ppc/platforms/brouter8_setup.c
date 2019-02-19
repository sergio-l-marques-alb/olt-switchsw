/*
 * arch/ppc/platforms/brouter8_setup.c
 *
 * BROUTER8 platform support
 *
 * Author: Jose Pedro Matos <jose-v-matos@ptinovacao.pt>
 * Derived from: m8260_setup.c by Dan Malek, MVista
 *
 * Copyright 2005 PTInovação
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>
#ifndef CONFIG_BROUTER8
#error "XPTO!!"
#endif

#include <linux/seq_file.h>

#include <asm/mpc8260.h>
#include <asm/machdep.h>
#include <asm/immap_cpm2.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <linux/ide.h>
#include <linux/delay.h>
#include <linux/mtd/physmap.h>


static void (*callback_setup_arch)(void);

extern unsigned char __res[sizeof(bd_t)];

unsigned int brouter8_bootmem_buffer = (unsigned int)-1;

static int
brouter8_show_cpuinfo(struct seq_file *m)
{
    /*
	bd_t	*binfo = (bd_t *)__res;

	seq_printf(m, "vendor\t\t: PT Inovação \n"
		      "machine\t\t: BROUTER8\n"
		      "bi_brgfreq\t\t: %d\n",
		      binfo->bi_brgfreq);
	return 0;
    */

	uint pvid, svid, phid1;
	bd_t *binfo = (bd_t *) __res;
	unsigned int freq;

	/* get the core frequency */
	freq = binfo->bi_intfreq;

	pvid = mfspr(SPRN_PVR);
	svid = mfspr(SPRN_SVR);

	seq_printf(m, "Vendor\t\t: Freescale Inc.\n");
	seq_printf(m, "core clock\t: %d MHz\n"
			"bus  clock\t: %d MHz\n",
			(int)(binfo->bi_intfreq / 1000000),
			(int)(binfo->bi_busfreq / 1000000));
	seq_printf(m, "PVR\t\t: 0x%x\n", pvid);
	seq_printf(m, "SVR\t\t: 0x%x\n", svid);

	/* Display cpu Pll setting */
	phid1 = mfspr(SPRN_HID1);
	seq_printf(m, "PLL setting\t: 0x%x\n", ((phid1 >> 24) & 0x3f));

	/* Display the amount of memory */
	seq_printf(m, "Memory\t\t: %d MB\n", (int)(binfo->bi_memsize / (1024 * 1024)));

    return(0);
}

static void __init
brouter8_setup_arch(void)
{
	printk("BROUTER8 Port\n");
	callback_setup_arch();
}
/* Map the IMMR, plus anything else we can cover
 * in that upper space according to the memory controller
 * chip select mapping.  Grab another bunch of space
 * below that for stuff we can't cover in the upper.
 */
static void __init
brouter8_m8260_map_io(void)
{
	bd_t    *binfo = (bd_t *)__res;

	/* Map IMMR region to a 256k BAT */
	io_block_mapping(CPM_MAP_ADDR/*virt*/, IMAP_ADDR/*phys*/, 0x40000, _PAGE_IO);
	io_block_mapping(IMAP_ADDR/*virt*/, IMAP_ADDR/*phys*/, 0x40000, _PAGE_IO);
	/* Map I/O region to a 64MB BAT */
//	io_block_mapping(0, 0, (unsigned int)binfo->bi_memsize, _PAGE_RAM|_PAGE_USER);
/*XXX*/	//printk("%s @ %s : %d\n",__FILE__,__FUNCTION__,__LINE__);
}

static u8 br_ide_inb (unsigned long port)
{
        return (u8) in_8(port);
}

static u16 br_ide_inw (unsigned long port)
{
        return (u16) in_be16(port);
}

//static unsigned int br_ide_base= 0;
static void br_ide_insw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
                *(u16 *)addr = in_be16((void*)port);
                addr += 2;
        }
}

static void br_ide_outb (u8 val, unsigned long port)
{
	out_8(port, val);	
}

static void br_ide_outbsync (ide_drive_t *drive, u8 addr, unsigned long port)
{
        out_8(port, addr);
}

static void br_ide_outw (u16 val, unsigned long port)
{
        out_be16(port, val);
}

static void br_ide_outsw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
		out_be16(port,*(u16 *)addr);
                addr += 2;
        }
}

void brouter8_hwif_iops (ide_hwif_t *hwif/*, unsigned long ide_base*/)
{
        hwif->OUTB      = br_ide_outb;
        hwif->OUTBSYNC  = br_ide_outbsync;
        hwif->OUTW      = br_ide_outw;
        hwif->OUTL      = NULL;
        hwif->OUTSW     = br_ide_outsw;
        hwif->OUTSL     = NULL;
        hwif->INB       = br_ide_inb;
        hwif->INW       = br_ide_inw;
        hwif->INL       = NULL;
        hwif->INSW      = br_ide_insw;
        hwif->INSL      = NULL;

//	br_ide_base= ide_base;

#if 0
{
        
        u16 cmd= ATA_CMD_PIO_READ;
        u32 lba= 0x00000001;
        u16 sector= 1;
        unsigned short buffer[256];

        if(0x00 == (hwif->INB(ide_base+7) & 0x08))
        {
        //***Set sector count*** //LBA[7:0]
        hwif->OUTB(ide_base+2,sector);
        hwif->OUTB(ide_base+3,lba & 0xFF);
        //***Set the LBA address of memory block to be written*** 
        hwif->OUTB(ide_base+4,(lba >> 8) & 0xFF);
        hwif->OUTB(ide_base+5,(lba >> 12) & 0xFF);
        hwif->OUTB(ide_base+6,0xE0);
        hwif->OUTB(ide_base+7,cmd);
        while ( (hwif->INB(ide_base+7) & 0x80) ) {//poll for busy bit (bit 7 of register), quit loop when busy bit =0
        }

        printk("Start read to buffer\n");
        //Read data to data buffer until DRQ is clear
	hwif->INSW(ide_base+8,buffer,256);
        printk("Finish Read: |ide_base:0x%X\n",ide_base);
#define printbuffer(x,c) \
{ \
  int j; \
  for (j= 0; j<c; j++) { \
    if(0 == (j % 10)) printk("\n"); \
    printk("0x%4.4X ", x[j]); \
  } \
  printk("\n"); \
} 

        printbuffer(buffer, 256);
        }
        else
        {
        printk("%s @ %d: Está busy antes de executar um cmd!\n",__FUNCTION__,__LINE__);
        }
}
#endif //debug
}


EXPORT_SYMBOL(brouter8_hwif_iops);
EXPORT_SYMBOL(brouter8_bootmem_buffer);

#ifdef CONFIG_MTD_PHYSMAP
static struct mtd_partition brouter8_partitions[1] = {
        {
         .name = "U-BOOT",
         .size = 0x00040000,
         .offset = 0,
         .mask_flags = 0,
         },
};

static int __init brouter8_setup_mtd(void)
{
	physmap_configure(0xfff00000, 0x40000, 1, NULL);
	physmap_map.size= 0x40000;
        physmap_set_partitions(brouter8_partitions, 1);
        return 0;
}

arch_initcall(brouter8_setup_mtd);
#endif



void brouter8_init(void)
{
	ppc_md.setup_io_mappings= brouter8_m8260_map_io;

	/* Anything special for this platform */
	ppc_md.show_cpuinfo	= brouter8_show_cpuinfo;

	callback_setup_arch	= ppc_md.setup_arch;
	ppc_md.setup_arch	= brouter8_setup_arch;
}
