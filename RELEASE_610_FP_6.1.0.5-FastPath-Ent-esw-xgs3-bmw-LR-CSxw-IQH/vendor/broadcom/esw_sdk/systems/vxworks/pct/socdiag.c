/*
 * $Id: socdiag.c,v 1.1 2011/04/18 17:11:11 mruas Exp $
 * $Copyright: Copyright 2006, Broadcom Corporation All Rights Reserved.
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

#include "config.h"
/*
 * NOTE: The LED blink process in usrConfig.c blinks the LED at a
 * rate dependent on system load. This task simply shows BCM with
 * a swirl on the LCD display.
 */

/*ARGSUSED*/
static void
led_beat(void *p)
{
    uint32 period = (1000000 / 3);

    for (;;) {
        sal_led_string("BCM\\");
        sal_usleep(period / 4);
        sal_led_string("BCM|");
        sal_usleep(period / 4);
        sal_led_string("BCM/");
        sal_usleep(period / 4);
        sal_led_string("BCM-");
        sal_usleep(period / 4);
    }
}

/*
 * Create the BMW/MPC8245 bde
 */

#include "bde/vxworks/vxbde.h"

ibde_t *bde;

int
bde_create(void)
{
    vxbde_bus_t bus;
    bus.base_addr_start = BCM_BASE_ADDR;
    bus.int_line = BCM_SWITCH_IRQ;
    bus.be_pio = 1;
    bus.be_packet = 0;
    bus.be_other = 1;
    return vxbde_create(&bus, &bde);
}

/*
 * Main: start the diagnostics and CLI shell under vxWorks.
 */
void
vxSpawn(void)
{
    extern void diag_shell(void *);

#ifdef WIND_DEBUG
    IMPORT taskDelay(int ticks);
    printf("Waiting for CrossWind attach ...\n");
    taskDelay(sysClkRateGet()*60);
#endif
    sal_core_init();
    sal_appl_init();
#ifdef DEBUG_STARTUP
    debugk_select(DEBUG_STARTUP);
#endif
    printk ("SOC BIOS (VxWorks) %s v%s.\n", sysModel(), vxWorksVersion);
    printk ("Kernel: %s.\n", kernelVersion());
    printk ("Made on %s.\n", creationDate);
    sal_thread_create("bcmCLI", 128*1024,100, diag_shell, 0);
    sal_thread_create("bcmLED", 8192, 99, led_beat, 0);
}

