/*
 * arch/ppc/platforms/unicom_v1_setup.c
 *
 * UNICOM_V1 platform support
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
#ifndef CONFIG_UNICOM_V1
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

static void (*callback_setup_arch)(void);

extern unsigned char __res[sizeof(bd_t)];


static int
unicom_v1_show_cpuinfo(struct seq_file *m)
{
	bd_t	*binfo = (bd_t *)__res;

	seq_printf(m, "vendor\t\t: PT Inovação \n"
		      "machine\t\t: UNICOM_V1\n"
		      "\n"
		      "mem size\t\t: 0x%08x\n"
		      "console baud\t\t: %d\n"
		      "ethaddr\t\t: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n"
		      "\n",
		      (unsigned int)(binfo->bi_memsize),
		      (unsigned int)(binfo->bi_baudrate),
		      binfo->bi_enetaddr[0],
		      binfo->bi_enetaddr[1],
		      binfo->bi_enetaddr[2],
		      binfo->bi_enetaddr[3],
		      binfo->bi_enetaddr[4],
		      binfo->bi_enetaddr[5]
		      );
	return 0;
}

static void __init
unicom_v1_setup_arch(void)
{
	printk("UNICOM_V1 Port\n");
	callback_setup_arch();
}

/* Map the IMMR, plus anything else we can cover
 * in that upper space according to the memory controller
 * chip select mapping.  Grab another bunch of space
 * below that for stuff we can't cover in the upper.
 */
static void __init
unicom_v1_m8260_map_io(void)
{
	uint addr;
	bd_t    *binfo = (bd_t *)__res;

	/* Map IMMR region to a 128k BAT */
	addr = (cpm2_immr != NULL) ? (uint)cpm2_immr : IMAP_ADDR;
	io_block_mapping(addr, addr, 0x20000, _PAGE_IO);
	/* Map I/O region to a 64MB BAT */
	io_block_mapping(0, 0, (unsigned int)(binfo->bi_memsize), _PAGE_RAM);
}

//static void
//pq2fadszu_m8260_restart(char *cmd)
//{
//	 uint    startaddr;

    /* Most boot roms have a warmstart as the second instruction
	 * of the reset vector.  If that doesn't work for you, change this
	 * or the reboot program to send a proper address.
	 */
//	 startaddr = BOOTROM_RESTART_ADDR;
//	 if (cmd != NULL) {
//	   if (!strncmp(cmd, "startaddr=", 10))
//	      startaddr = simple_strtoul(&cmd[10], NULL, 0);
//	 }

//     m8260_gorom((void*)__pa(__res), startaddr);

//	 local_irq_disable();
	 
//	 ((void (*)(void))startaddr)();

//}
static u8 unicom_ide_inb (unsigned long port)
{
        return (u8) in_8(port);
}

static u16 unicom_ide_inw (unsigned long port)
{
        return (u16) in_be16(port);
}

static unsigned int unicom_ide_base= 0;
static void unicom_ide_insw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
                *(u16 *)addr = in_be16((void*)port);
                addr += 2;
        }
}

static void unicom_ide_outb (u8 val, unsigned long port)
{
	out_8(port, val);	
}

static void unicom_ide_outbsync (ide_drive_t *drive, u8 addr, unsigned long port)
{
        out_8(port, addr);
}

static void unicom_ide_outw (u16 val, unsigned long port)
{
        out_be16(port, val);
}

static void unicom_ide_outsw (unsigned long port, void *addr, u32 count)
{
	while (count--) {
		out_be16(port,*(u16 *)addr);
                addr += 2;
        }
}

void unicom_v1_hwif_iops (ide_hwif_t *hwif, unsigned long ide_base)
{
        hwif->OUTB      = unicom_ide_outb;
        hwif->OUTBSYNC  = unicom_ide_outbsync;
        hwif->OUTW      = unicom_ide_outw;
        hwif->OUTL      = NULL;
        hwif->OUTSW     = unicom_ide_outsw;
        hwif->OUTSL     = NULL;
        hwif->INB       = unicom_ide_inb;
        hwif->INW       = unicom_ide_inw;
        hwif->INL       = NULL;
        hwif->INSW      = unicom_ide_insw;
        hwif->INSL      = NULL;

	unicom_ide_base= ide_base;

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


EXPORT_SYMBOL(unicom_v1_hwif_iops);


void unicom_v1_init(void)
{
	ppc_md.setup_io_mappings= unicom_v1_m8260_map_io;

	/* Anything special for this platform */
	ppc_md.show_cpuinfo	= unicom_v1_show_cpuinfo;

	callback_setup_arch	= ppc_md.setup_arch;
	ppc_md.setup_arch	= unicom_v1_setup_arch;
}
