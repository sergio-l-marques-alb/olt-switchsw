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
 *
 *
 */
/******************************************************************************/
/*									      */
/* File Name  : ctf_drv.c						      */
/*									      */
/* Description: This file contains Linux character device driver entry points */
/*              for the BCM95301x  CTF Driver.				      */
/******************************************************************************/
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include "mach/iproc_regs.h"
#include "mach/io_map.h"
#include "mach/irqs.h"
#include "ctf_common.h"
#include "ctf_regs.h"
#include "fcache.h"


#undef  CTF_DECL
#define CTF_DECL(x)		#x,

bool ctf_drv_reg_flag = 0;

extern bool napt_flow_init_done;
extern bool napt_next_hop_init_done;

extern int ctf_status_show(void);
extern void ctf_ctl_init(void);
extern int ctf_disable(void);
extern int ctf_enable(void);
extern int ctf_reset_tables(int table);
extern int ctf_regdump(int table, int index);
extern int ctf_dump_napt_pool_table(void);
extern int ctf_dump_next_hop_table(void);

static irqreturn_t ctf_interrupt_handler(int irq, void *dev_id);

// void *ctf_dev_id = 0xabcdabcd;

const char *ctf_ioctl_name[] =
{
	CTF_DECL(FAIOCTL_STATUS)
	CTF_DECL(FAIOCTL_RESET)
	CTF_DECL(FAIOCTL_INIT)
	CTF_DECL(FAIOCTL_ENABLE)
	CTF_DECL(FAIOCTL_DISABLE)
	CTF_DECL(FAIOCTL_REGDUMP)
	CTF_DECL(FAIOCTL_DUMPPT)
	CTF_DECL(FAIOCTL_DUMPNHT)
	CTF_DECL(FAIOCTL_CFGLOG)
	CTF_DECL(FAIOCTL_ADDFE)
	CTF_DECL(FAIOCTL_INVALID)
};

/*
 * Invoke CTF kernel calls from user plane for debugging.
 */
unsigned int ctf_args = 0;

void ctf_usage(unsigned int arg)
{
	printk(" 0:status\n"
		" 1:reset\n"
		" 2:init\n"
		" 3:enable\n"
		" 4:disable\n"
		" 5:regdump\n"
		" 6:dump pooltable\n"
		" 7:dump nexthoptable\n"
		" 8:debug\n");
}

int process_ctf_cmd(unsigned int arg)
{
	ctf_args = arg;

	switch (arg) {
	case 0:
		ctf_status_show();
		break;

	case 1:
		ctf_reset_tables(-1);
		break;

	case 2:
		ctf_ctl_init();
		break;

	case 3:
		ctf_enable();
		break;

	case 4:
		ctf_disable();
		break;

	case 5:
		ctf_regdump(-1, -1);
		break;

	case 7:
		ctf_dump_napt_pool_table();
		break;

	case 8:
		ctf_dump_next_hop_table();
		break;

	default:
		ctf_usage( arg );
		break;
	}
	
	return CTF_SUCCESS;
}

/*
 * Function Name: ctf_handle_ioctl
 * Description  : Main entry point to handle user applications IOCTL requests
 *                CTF Utility, may pass more than parameter in the
 *                CTF_TUPLE16 format.
 * Returns      : 0 - success or error
 */
static int ctf_handle_ioctl( struct file *filep,
				unsigned int command, unsigned long arg)
{
	ctf_ioctl_t cmd;
//    unsigned long sip, dip, sport, dport, proto;
    unsigned long args[5];

	int ret = CTF_SUCCESS;

	if (command > FAIOCTL_INVALID)
		cmd = FAIOCTL_INVALID;
	else
		cmd = (ctf_ioctl_t)command;


	switch (cmd) {
	case FAIOCTL_STATUS:
		ret = ctf_callback(FAIOCTL_STATUS, arg);
		break;

	case FAIOCTL_RESET:
		ret += ctf_callback(FAIOCTL_RESET, 0);
		break;

	case FAIOCTL_INIT:
		ret = ctf_callback(FAIOCTL_INIT, 0);
		break;

	case FAIOCTL_DISABLE:
		ret += ctf_callback(FAIOCTL_DISABLE, 0);
		break;

	case FAIOCTL_ENABLE:
		ret = ctf_callback(FAIOCTL_ENABLE, 0);
		break;

	case FAIOCTL_REGDUMP:
		ret += ctf_callback(FAIOCTL_REGDUMP, 0);
		break;

	case FAIOCTL_DUMPPT:
		ret = ctf_callback(FAIOCTL_DUMPPT, 0);
		break;

	case FAIOCTL_DUMPNHT:
		ret = ctf_callback(FAIOCTL_DUMPNHT, 0);
		break;

	case FAIOCTL_CFGLOG:
		ret = ctf_callback(FAIOCTL_CFGLOG, arg);
		break;

	case FAIOCTL_ADDFE:
        //printk("\n\n%s: arg0=%s, arg1=%s\n\n", __func__, ((char *)arg), ((char *)arg) + 20);
        //printk("\n\n%s: arg2=%s, arg3=%s\n\n", __func__, ((char *)arg) + 40, ((char *)arg) + 60);
        //printk("\n\n%s: arg4=%s\n\n", __func__, ((char *)arg) + 80);

        args[0] /* sip */ = simple_strtoul((char *)arg, NULL, 16);
        args[1] /* dip */ = simple_strtoul(((char *)arg) + 20, NULL, 16);
        args[2] /* sport */ = simple_strtoul(((char *)arg) + 40, NULL, 16);
        args[3] /* dport */ = simple_strtoul(((char *)arg) + 60, NULL, 16);
        args[4] /* proto */ = simple_strtoul(((char *)arg) + 80, NULL, 16);

		ret = ctf_callback(FAIOCTL_ADDFE, (int)args);
        
        //printk("\n\n%s: sip=0x%08X, dip=0x%08X, sport=0x%08X, dport=0x%08X, proto=0x%08X\n\n", __func__, sip, dip, sport, dport, proto);
		break;

	case FAIOCTL_ADDPT:
		ret = ctf_callback(FAIOCTL_ADDPT, arg);
		break;

	case FAIOCTL_ADDNHT:
		ret = ctf_callback(FAIOCTL_ADDNHT, arg);
		break;

	default:
		printk(KERN_DEBUG "Invalid cmd[%u]", command);
		return CTF_ERROR;
	}

	return ret;

}

/*
 * Function Name: ctf_open
 * Description  : Called when an user application opens this device.
 * Returns      : 0 - success
 */
static int ctf_open(struct inode *inode, struct file *fp)
{
//	printk(KERN_INFO "Access CTF Char Device");
	return CTF_SUCCESS;
}

/* Global file ops */
static struct file_operations ctf_fops =
{
	.unlocked_ioctl  = (void *) ctf_handle_ioctl,
	.open   = ctf_open,
};

/*
 * Function Name: ctf_drv_init
 * Description  : Initial function that is called at system startup that
 *                registers this device.
 * Returns      : CTFDRV_MAJOR (success).
 *		  err (error)
 */

int __init ctf_drv_init(void)
{
	int	err;
	char 	*dev_name = "CTF";

	printk(KERN_DEBUG "CTF Driver Init");

	if (register_chrdev(CTFDRV_MAJOR, CTFDRV_NAME, &ctf_fops)) {
		printk(KERN_ERR "%s Unable to get major number <%d>\n",
			__FUNCTION__, CTFDRV_MAJOR);
		return CTF_ERROR;
	}

	printk(KERN_INFO "Char Driver Registered<%d>\n", CTFDRV_MAJOR);

	/*
	 * Register an interrupt handler and disable interrupts.
	 */
	err = request_irq(BCM_INT_ID_FA, ctf_interrupt_handler,
			  IRQF_DISABLED | IRQF_PERCPU,
			  dev_name, NULL);
	if (err) {
		printk(KERN_ERR "%s: Unable to request %s IRQ <%d>\n",
			__func__, dev_name, BCM_INT_ID_FA);
		return err;
	}
	ctf_drv_reg_flag = 1;
	printk(KERN_INFO "Interrupt Registered\n");

	return CTFDRV_MAJOR;
}

/*
 * Function Name: ctf_drv_exit
 * Description  : Final function that is called when the module is unloaded.
 * Returns      : IRQ_HANDLED.
 */
void __exit ctf_drv_exit(void)
{

	unregister_chrdev( CTFDRV_MAJOR, CTFDRV_NAME );

	printk(KERN_INFO "Char Driver Unregistered<%d>\n", CTFDRV_MAJOR);
}

/*
 * Function Name: ctf_interrupt_handler
 * Description  : Flow Acceleration Interrupt handler
 * Returns      : None.
 */
static irqreturn_t ctf_interrupt_handler(int irq, void *dev_id)
{
	void __iomem		*ctf_reg;
	ctf_intstat_reg_t	intstat;

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_INTSTAT_REG_OFFSET);
	/* Read the int status and clear it */
	intstat.word = readl(ctf_reg);

	switch(intstat.word) {
	case CTF_INTSTAT_NXT_HOP_INIT_DONE:
		napt_next_hop_init_done = CTF_FALSE;
		intstat.word |= CTF_INTSTAT_NXT_HOP_INIT_DONE;
		break;

	case CTF_INTSTAT_NAPT_FLOW_INIT_DONE:
		napt_flow_init_done = CTF_FALSE;
		intstat.word |= CTF_INTSTAT_NAPT_FLOW_INIT_DONE;
		break;

	default:
		intstat.word |= (intstat.word | 0x3FF);
	}

	return IRQ_HANDLED;
}
