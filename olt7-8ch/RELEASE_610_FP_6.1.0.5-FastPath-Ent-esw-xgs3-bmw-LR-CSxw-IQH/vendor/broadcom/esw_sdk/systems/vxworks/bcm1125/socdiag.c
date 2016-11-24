/*
 * $Id: socdiag.c,v 1.1 2011/04/18 17:11:11 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 */

#include <vxWorks.h>
#include <version.h>
#include <kernelLib.h>
#include <sysLib.h>

#include <sal/appl/sal.h>
#include <soc/debug.h>
#include <sal/appl/io.h>
#include <sal/core/boot.h>
#include <sal/core/thread.h>

#if defined(SIBYTE_INTR_HACK)

int total_soc_device = 0;
volatile int IN_SOC_INTR_INVOKE = 0;

void SOC_INTR_INVOKE(void)
{
    volatile int    s;
    int u;

    if (IN_SOC_INTR_INVOKE) {
        return;
    }

    s = sal_splhi();
    IN_SOC_INTR_INVOKE = 1;
    sal_spl(s);
    for (u = 0; u < total_soc_device; u++) {
        soc_intr(u);
    }
    s = sal_splhi();
    IN_SOC_INTR_INVOKE = 0;
    sal_spl(s);
}
#else
#define SOC_INTR_INVOKE
#endif

/*ARGSUSED*/
static void
led_string(void *p)
{
    uint32 period = (1000000 / 3);
    int blue_led = 0;

    for (;;) {
	sal_led_string("BCM\\");
	sal_usleep(period / 4);
	sal_led_string("BCM|");
	sal_usleep(period / 4);
	sal_led_string("BCM/");
	sal_usleep(period / 4);
	sal_led_string("BCM-");
	sal_usleep(period / 4);
        sal_led(blue_led);
        blue_led = !blue_led;
    }
}

/* 
 * Create the BCM SiByte1125/1250 MIPS bde
 */

#include "bde/vxworks/vxbde.h"

ibde_t *bde;

/*
 * Temp - display raw PCI header
 */
void
read_pci_conf(int bus, int dev, int func, int begin, int end)
{
    int i, pdata;

    printf("bus %d dev %d func %d\n", bus, dev, func);
    for(i = begin; i <= end; i += 4) {
        pciConfigInLong(bus, dev, func, i, &pdata);
        printf("REG%02x = 0x%08x\n", i, pdata);
    }
}

int
bde_create(void)
{	
    vxbde_bus_t bus; 
    int rv;

#if 0
    bus.base_addr_start = 0;
    bus.int_line = 0;
#endif
    bus.base_addr_start = 0x41000000;
    bus.int_line = 0x38;

#ifdef MIPSEL
    bus.be_pio = 1;
    bus.be_packet = 0;
    bus.be_other = 0;
#else
    bus.be_pio = 1;
    bus.be_packet = 0;
    bus.be_other = 1;
#endif

    rv = vxbde_create(&bus, &bde);

#if defined(SIBYTE_INTR_HACK)
    total_soc_device = bde->num_devices();
#endif

    return rv;
}

#if 0
/*
 * Main: start the diagnostics and CLI shell under vxWorks.
 */
void vxSpawn(void)
{
    extern void diag_shell(void *);
    sal_core_init();
    sal_appl_init();
#ifdef DEBUG_STARTUP
    debugk_select(DEBUG_STARTUP);
#endif
    printk ("SOC BIOS (VxWorks) %s v%s.\n", sysModel(), vxWorksVersion);
    printk ("Kernel: %s.\n", kernelVersion());
    printk ("Made on %s.\n", creationDate);
    sal_thread_create("bcmCLI", 128*1024, 100, diag_shell, 0);
    sal_thread_create("bcmLED", 8192, 99, led_string, 0);
}
#endif
