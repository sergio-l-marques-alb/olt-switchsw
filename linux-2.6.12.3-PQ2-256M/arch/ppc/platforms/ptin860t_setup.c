/*
 * arch/ppc/platforms/ptin860t_setup.c
 *
 * PTIN860T platform support
 *
 * Author: Jose Pedro Matos <jose-v-matos@ptinovacao.pt>
 * Derived from: unicom_v1_setup.c by José Pedro Matos
 *
 * Copyright 2005 PTInovação
 *
 * This program is free software; you can redistribute  it and/or modify it
 * under  the terms of  the GNU General  Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */

#include <linux/config.h>

#include <linux/seq_file.h>

#include <asm/mpc8xx.h>
#include <asm/machdep.h>
#include <asm/8xx_immap.h>
#include <asm/pgtable.h>
#include <asm/io.h>
#include <asm/cputable.h>

static void (*callback_setup_arch)(void);

extern unsigned char __res[sizeof(bd_t)];


static int
ptin860t_show_cpuinfo(struct seq_file *m)
{
	bd_t	*binfo = (bd_t *)__res;

	seq_printf(m, "vendor\t\t: PT Inovação \n"
		      "machine\t\t: PTIN860T\n"
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

static int xpto=0;
static void __init
ptin860t_setup_arch(void)
{
	printk("PTIN860T Port:%d\n",xpto);
asm(BEGIN_FTR_SECTION);
        printk("ISTO nao deve aparecer !!!!! \n");                             /* for 601, do nothing */
asm(END_FTR_SECTION_IFSET(PPC_FEATURE_UNIFIED_CACHE));
		
	callback_setup_arch();
}

/* Map the IMMR, plus anything else we can cover
 * in that upper space according to the memory controller
 * chip select mapping.  Grab another bunch of space
 * below that for stuff we can't cover in the upper.
 */
static void __init
ptin860t_m8xx_map_io(void)
{
	bd_t    *binfo = (bd_t *)__res;
	/* Map IMMR region to a 64kk BAT */
	io_block_mapping(IMAP_ADDR, IMAP_ADDR, IMAP_SIZE, _PAGE_IO);
	/* Map I/O region to a (mem) MB BAT */
	io_block_mapping(0, 0, (unsigned int)(binfo->bi_memsize), _PAGE_RAM);
	xpto++;
}

static void ptin860t_progress(char *s, unsigned short u)
{
	printk("%s : %hd\n",s,u);
	return;
}

void __init
ptin860t_init(void)
{
	ppc_md.setup_io_mappings= ptin860t_m8xx_map_io;

	/* Anything special for this platform */
	ppc_md.show_cpuinfo	= ptin860t_show_cpuinfo;

	callback_setup_arch	= ppc_md.setup_arch;
	ppc_md.setup_arch	= ptin860t_setup_arch;
	ppc_md.progress         = &ptin860t_progress;
	xpto++;
}
