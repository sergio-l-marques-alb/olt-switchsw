/*
 * m8xx_wdt.c - MPC8xx watchdog driver
 *
 * Author: Florian Schirmer <jolt@tuxbox.org>
 *
 * 2002 (c) Florian Schirmer <jolt@tuxbox.org> This file is licensed under
 * the terms of the GNU General Public License version 2. This program
 * is licensed "as is" without any warranty of any kind, whether express
 * or implied.
 */

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/mpc8260.h>
#include <asm/immap_cpm2.h>
#include <syslib/m82xx_wdt.h>

#define debug(fmt) {}

static int wdt_timeout;

static irqreturn_t m82xx_wdt_interrupt(int, void *, struct pt_regs *);
static struct irqaction m82xx_wdt_irqaction = {
	.handler = m82xx_wdt_interrupt,
	.name = "watchdog",
};

static __inline__ void reset_8260_watchdog(void)
{
    volatile cpm2_map_t *immr= (void*)CPM_MAP_ADDR;
    immr->im_siu_conf.siu_82xx.sc_swsr = 0x556c;
    immr->im_siu_conf.siu_82xx.sc_swsr = 0xaa39;
}

void m82xx_wdt_reset(void)
{
	reset_8260_watchdog();
}

static irqreturn_t m82xx_wdt_interrupt(int irq, void *dev, struct pt_regs *regs)
{
        volatile cpm2_map_t *immr= (void*)CPM_MAP_ADDR;
	m82xx_wdt_reset();

	immr->im_sit.sit_piscr |= PISCR_PS;	/* clear irq */

	debug("m82xx_wdt_interrupt\n");

	return IRQ_HANDLED;
}

void m82xx_wdt_handler_install(bd_t * binfo)
{
        volatile cpm2_map_t *immr= (void*)CPM_MAP_ADDR;
	u32 pitc;
	u32 sypcr;
	u32 pitrtclk;

	immr->im_siu_conf.siu_82xx.sc_sypcr= 0xFFFFFFFF;

	sypcr = immr->im_siu_conf.siu_82xx.sc_sypcr;

	if (!(sypcr & 0x04)) {
		printk(KERN_NOTICE "m82xx_wdt: wdt disabled (SYPCR: 0x%08X)\n",
		       sypcr);
		return;
	}

	m82xx_wdt_reset();

	printk(KERN_NOTICE
	       "m82xx_wdt: active wdt found (SWTC: 0x%04X, SWP: 0x%01X)\n",
	       (sypcr >> 16), sypcr & 0x01);

	wdt_timeout = (sypcr >> 16) & 0xFFFF;

	if (!wdt_timeout)
		wdt_timeout = 0xFFFF;

	if (sypcr & 0x01)
		wdt_timeout *= 2048;

	/*
	 * Fire trigger if half of the wdt ticked down 
	 */

//	if (immr->im_sit.sit_rtcsc & RTCSC_38K)
//		pitrtclk = 9600;
//	else
		pitrtclk = 8192;

	if ((wdt_timeout) > (UINT_MAX / pitrtclk))
		pitc = wdt_timeout / binfo->bi_intfreq * pitrtclk / 2;
	else
		pitc = pitrtclk * wdt_timeout / binfo->bi_intfreq / 2;

	immr->im_sit.sit_pitc = 0x00100000;//pitc << 16;
	immr->im_sit.sit_piscr =
	    /*(mk_int_mask(SIU_INT_PIT) << 8) | */PISCR_PIE | PISCR_PTE;

	if (setup_irq(SIU_INT_PIT, &m82xx_wdt_irqaction))
		panic("m82xx_wdt: error setting up the watchdog irq!");

	printk(KERN_NOTICE
	       "m82xx_wdt: keep-alive trigger installed (PITC: 0x%04X)\n", pitc);

	wdt_timeout /= binfo->bi_intfreq;
}

int m82xx_wdt_get_timeout(void)
{
	return wdt_timeout;
}
