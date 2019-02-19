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
#include <linux/err.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/cpu.h>
#include <linux/platform_device.h>
#include <asm/io.h>
#include <linux/kobject.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <linux/proc_fs.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>

#include <net/xfrm.h>
#include <linux/pfkeyv2.h>

#include <mach/iproc_regs.h>
#include <mach/socregs_nsp_open.h>
#include "pae_fw.h"
#include "pae_regs.h"
#include "pae_log.h"
#include "pae_cmds.h"
#include "pae_stats.h"
#include "paemsg_int.h"
#include "pae_arp.h"

/* #include <iprocspu.h> */
/* #include "paemsg.h" */
/* #include <paemgmt.h> */
/* #include <paehost.h> */

#define NO_FA2_DRIVER (0)
#define PAE_POLL_INTERVAL_SECS (1)

int pae_status = PAE_STOPPED;

static struct platform_device *pae_device = 0;

/* Background polling for cumulative statistics */

static void pae_bgr_poll(struct work_struct *work);

DECLARE_DELAYED_WORK(pae_bgr_poll_work, pae_bgr_poll);

static void pae_bgr_poll(struct work_struct *work)
{
    pae_update_cumulative_statistics();

    if (pae_arp_watch_timer) {
        pae_watch_arp_tables(PAE_POLL_INTERVAL_SECS);
    }

    schedule_delayed_work(&pae_bgr_poll_work, PAE_POLL_INTERVAL_SECS * HZ);
}

/* Setup and shutdown */

int pae_init(void)
{
    u32 regval;
    int rv;

    RETURN_ON_FAIL(pae_register_set(PAE_M0_IDM_IDM_RESET_CONTROL, 0), NO_MSG);
    RETURN_ON_FAIL(pae_register_set(PAE_S0_IDM_IDM_RESET_CONTROL, 0), NO_MSG);

    if (NO_FA2_DRIVER) {
        RETURN_ON_FAIL(pae_register_get(CTF_CONTROL_REG, &regval), NO_MSG);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, MEM_INIT, 1);
        RETURN_ON_FAIL(pae_register_set(CTF_CONTROL_REG, regval), NO_MSG);
    }

    RETURN_ON_FAIL(pae_register_get(AMAC_IDM0_IO_CONTROL_DIRECT, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, AMAC_IDM0_IO_CONTROL_DIRECT, CLK_250_SEL, 1);
    RETURN_ON_FAIL(pae_register_set(AMAC_IDM0_IO_CONTROL_DIRECT, regval), NO_MSG);

    RETURN_ON_FAIL(pae_register_get(AMAC_IDM1_IO_CONTROL_DIRECT, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, AMAC_IDM1_IO_CONTROL_DIRECT, CLK_250_SEL, 1);
    RETURN_ON_FAIL(pae_register_set(AMAC_IDM1_IO_CONTROL_DIRECT, regval), NO_MSG);

    if (NO_FA2_DRIVER) {
        RETURN_ON_FAIL(pae_register_get(CTF_CONTROL_REG, &regval), NO_MSG);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, CTF_MODE, 1);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, FRAGMENTATION_ENABLE, 0);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, DISABLE_MAC_DA_CHECK, 0);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, PAE_ENABLED, 1);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, SPU_ENABLE, 1);
        RETURN_ON_FAIL(pae_register_set(CTF_CONTROL_REG, regval), NO_MSG);
    } else {
        RETURN_ON_FAIL(pae_register_get(CTF_CONTROL_REG, &regval), NO_MSG);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, PAE_ENABLED, 1);
        REG_FIELD_SET_BIT(&regval, CTF_CONTROL, SPU_ENABLE, 1);
        RETURN_ON_FAIL(pae_register_set(CTF_CONTROL_REG, regval), NO_MSG);
    }

    if (NO_FA2_DRIVER) {
        RETURN_ON_FAIL(pae_register_get(CTF_BRCM_HDR_CONTROL, &regval), NO_MSG);
        REG_FIELD_SET(&regval, CTF_BRCM_HDR_CONTROL, BRCM_HDR_REASON_CODE_MASK, 0);
        RETURN_ON_FAIL(pae_register_set(CTF_BRCM_HDR_CONTROL, regval), NO_MSG);
    }

    RETURN_ON_FAIL(pae_register_get(SPU_CONTROL, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, SPU_CONTROL, SOFT_RST, 1);
    RETURN_ON_FAIL(pae_register_set(SPU_CONTROL, regval), NO_MSG);
    
    RETURN_ON_FAIL(pae_register_get(SPU_CONTROL, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, SPU_CONTROL, OUT_ENDIAN, 0);
    REG_FIELD_SET_BIT(&regval, SPU_CONTROL, IN_ENDIAN, 0);
    RETURN_ON_FAIL(pae_register_set(SPU_CONTROL, regval), NO_MSG);
    
    RETURN_ON_FAIL(pae_register_get(R5_CONFIG0, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, TE_INIT,                 0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, SYS_PORESET,             1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, RESET_N,                 1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, PARITY_ODD,              0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, PADDR_DEBUG31,           0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, LOC_ZERO_RAMA,           1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INTERRUPT_ASYNC,         0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INITRAMB,                1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INITRAMA,                1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESTART,           0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET_N,           1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET,             0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_NO_CLK_STOP,       1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_NIDEN,             1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_ENTCM1IF,          1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_EN,                0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_EDBGRQ,            0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DAP_DAP_TO_DEBUG_APB_EN, 0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CPU_HALT,                0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CFG_ENDIAN,              0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CFG_EE,                  0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, BTCM_SPLIT,              1);
    REG_FIELD_SET(&regval,     R5_CONFIG0, BTCM_SIZE,               9);
    REG_FIELD_SET(&regval,     R5_CONFIG0, ATCM_SIZE,               8);
    REG_FIELD_SET(&regval,     R5_CONFIG0, RMW_RAM,                 3);
    RETURN_ON_FAIL(pae_register_set(R5_CONFIG0, regval), NO_MSG);

    RETURN_ON_FAIL(pae_register_get(PAE_ECC_DEBUG, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, PAE_ECC_DEBUG, ECC_DISABLE, 1);
    RETURN_ON_FAIL(pae_register_set(PAE_ECC_DEBUG, regval), NO_MSG);

    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_CONFIG, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, PAE_BUFFER_CONFIG, PAE_MEM_INIT, 1);
    REG_FIELD_SET_BIT(&regval, PAE_BUFFER_CONFIG, PAE_SYS_INIT, 1);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_CONFIG, regval), NO_MSG);

    /* Poll PAE_BUFFER_CONFIG PAE_MEM_INIT_DONE */
    do {
        RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_CONFIG, &regval), NO_MSG);
    } while (REG_FIELD_GET_BIT(regval, PAE_BUFFER_CONFIG, PAE_MEM_INIT_DONE) == 0);

    /* Bcm_cmd "modreg PAE_BUFFER_ALLOCATION.0 INTERCEPT_PT_START_ADDR=0x0 INTERCEPT_PT_END_ADDR=0x5ff" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_ALLOCATION0, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_ALLOCATION, INTERCEPT_PT_START_ADDR, 0);
    REG_FIELD_SET(&regval, PAE_BUFFER_ALLOCATION, INTERCEPT_PT_END_ADDR, 0x5ff);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_ALLOCATION0, regval), NO_MSG);

     /* Bcm_cmd "modreg PAE_BUFFER_ALLOCATION.1 INTERCEPT_PT_START_ADDR=0x600 INTERCEPT_PT_END_ADDR=0xbff" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_ALLOCATION1, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_ALLOCATION, INTERCEPT_PT_START_ADDR, 0x600);
    REG_FIELD_SET(&regval, PAE_BUFFER_ALLOCATION, INTERCEPT_PT_END_ADDR, 0xbff);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_ALLOCATION1, regval), NO_MSG);

     /* Bcm_cmd "modreg PAE_BUFFER_BACKPRESSURE_CONFIG.0 WATERMARK_DEPTH_XON=0x13b WATERMARK_DEPTH_XOFF=0x200" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_BACKPRESSURE_CONFIG0, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_CONFIG, WATERMARK_DEPTH_XON, 0x13b);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_CONFIG, WATERMARK_DEPTH_XOFF, 0x200);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_BACKPRESSURE_CONFIG0, regval), NO_MSG);

     /* Bcm_cmd "modreg PAE_BUFFER_BACKPRESSURE_CONFIG.1 WATERMARK_DEPTH_XON=0x2cb WATERMARK_DEPTH_XOFF=0x400" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_BACKPRESSURE_CONFIG1, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_CONFIG, WATERMARK_DEPTH_XON, 0x2cb);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_CONFIG, WATERMARK_DEPTH_XOFF, 0x400);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_BACKPRESSURE_CONFIG1, regval), NO_MSG);

    /* Bcm_cmd "modreg PAE_BUFFER_CONGESTION_CONFIG ENQ0_STOP_LEVEL=0x337" */
    /* ENQ0_STOP_LEVEL = (end_addr - start_addr) - depth_xon */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_CONGESTION_CONFIG, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_CONGESTION_CONFIG, ENQ0_STOP_LEVEL, 0x337);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_CONGESTION_CONFIG, regval), NO_MSG);

     /* Bcm_cmd "modreg PAE_BUFFER_BACKPRESSURE_MAP.0 INTERCEPT_PT_BACKPRESSURE_CONTRIBUTOR_MASK=1" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_BACKPRESSURE_MAP0, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_MAP, INTERCEPT_PT_BACKPRESSURE_CONTRIBUTOR_MASK, 1);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_BACKPRESSURE_MAP0, regval), NO_MSG);

     /* Bcm_cmd "modreg PAE_BUFFER_BACKPRESSURE_MAP.1 INTERCEPT_PT_BACKPRESSURE_CONTRIBUTOR_MASK=2" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_BACKPRESSURE_MAP1, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_BUFFER_BACKPRESSURE_MAP, INTERCEPT_PT_BACKPRESSURE_CONTRIBUTOR_MASK, 2);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_BACKPRESSURE_MAP1, regval), NO_MSG);

    /* Bcm_cmd "modreg PAE_SCRATCHPAD_ALLOCATION SCRATCHPAD_START_ADDR=0xc00 SCRATCHPAD_END_ADDR=0xfff" */
    RETURN_ON_FAIL(pae_register_get(PAE_SCRATCHPAD_ALLOCATION, &regval), NO_MSG);
    REG_FIELD_SET(&regval, PAE_SCRATCHPAD_ALLOCATION, SCRATCHPAD_END_ADDR, 0xfff);
    REG_FIELD_SET(&regval, PAE_SCRATCHPAD_ALLOCATION, SCRATCHPAD_START_ADDR, 0xc00);
    RETURN_ON_FAIL(pae_register_set(PAE_SCRATCHPAD_ALLOCATION, regval), NO_MSG);

     /* Bcm_cmd "modreg CTF_DEBUG_CONTROL DM_FIFO_BP_LEVEL=0xa2" */
    if (NO_FA2_DRIVER) {
        RETURN_ON_FAIL(pae_register_get(CTF_DEBUG_CONTROL, &regval), NO_MSG);
        REG_FIELD_SET(&regval, CTF_DEBUG_CONTROL, DM_FIFO_BP_LEVEL, 0xa2);
        RETURN_ON_FAIL(pae_register_set(CTF_DEBUG_CONTROL, regval), NO_MSG);
    }

     /* Bcm_cmd "modreg PAE_BUFFER_CONFIG PAE_SYS_INIT=0x1" */
    RETURN_ON_FAIL(pae_register_get(PAE_BUFFER_CONFIG, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, PAE_BUFFER_CONFIG, PAE_SYS_INIT, 0x1);
    RETURN_ON_FAIL(pae_register_set(PAE_BUFFER_CONFIG, regval), NO_MSG);

    /* Set SPU endianness.  It appears that if input and output endianness to SPU
       are set to the same value, that there is some swapping on the packets after
       exiting the SPU.  So, arbitrarily, it is currently set to input as little-endian
       output as big-endian, which results in no swapping */
    RETURN_ON_FAIL(pae_register_get(SPU_CONTROL, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, SPU_CONTROL, IN_ENDIAN, 0);
    REG_FIELD_SET_BIT(&regval, SPU_CONTROL, OUT_ENDIAN, 0);
    RETURN_ON_FAIL(pae_register_set(SPU_CONTROL, regval), NO_MSG);

    rv = pae_tcm_init();

    schedule_delayed_work(&pae_bgr_poll_work, PAE_POLL_INTERVAL_SECS * HZ);

    pae_status = PAE_NO_FW;

    return rv;
}


static int pae_stop(void)
{
    u32 regval;

    cancel_delayed_work_sync(&pae_bgr_poll_work);

    pae_tcm_exit();
    RETURN_ON_FAIL(pae_register_get(CTF_CONTROL_REG, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, CTF_CONTROL, PAE_ENABLED, 0);
    RETURN_ON_FAIL(pae_register_set(CTF_CONTROL_REG, regval), NO_MSG);

    RETURN_ON_FAIL(pae_register_get(R5_CONFIG0, &regval), NO_MSG);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, TE_INIT,                 0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, SYS_PORESET,             1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, RESET_N,                 1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, PARITY_ODD,              0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, PADDR_DEBUG31,           0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, LOC_ZERO_RAMA,           1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INTERRUPT_ASYNC,         0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INITRAMB,                1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, INITRAMA,                1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESTART,           0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET_N,           1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_RESET,             0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_NO_CLK_STOP,       1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_NIDEN,             0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_ENTCM1IF,          1);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_EN,                0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DEBUG_EDBGRQ,            0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, DAP_DAP_TO_DEBUG_APB_EN, 0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CPU_HALT,                0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CFG_ENDIAN,              0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, CFG_EE,                  0);
    REG_FIELD_SET_BIT(&regval, R5_CONFIG0, BTCM_SPLIT,              1);
    REG_FIELD_SET(&regval,     R5_CONFIG0, BTCM_SIZE,               9);
    REG_FIELD_SET(&regval,     R5_CONFIG0, ATCM_SIZE,               8);
    REG_FIELD_SET(&regval,     R5_CONFIG0, RMW_RAM,                 3);
    RETURN_ON_FAIL(pae_register_set(R5_CONFIG0, regval), NO_MSG);

    pae_status = PAE_STOPPED;
    return 0;
}


/* sysfs platform driver pieces */


int pae_add_action_stats_attribute(struct attribute *attr)
{
    return sysfs_add_file_to_group(&pae_device->dev.kobj, attr, "stats");    
}


void pae_remove_action_stats_attribute(struct attribute *attr)
{
    sysfs_remove_file_from_group(&pae_device->dev.kobj, attr, "stats");
}


static struct bin_attribute fw_bin_attribute = {
    .attr = {
        .name = "fw",
        .mode = S_IWUSR
    },
    .size = 128*1024,
    .read = NULL,
    .write = &fw_write
};


static ssize_t read_status(struct device *dev,
                           struct device_attribute *devattr, char *buf)
{
    /* Corresponding to the #defines of PAE_NO_FIRMWARE, etc. */
    static const char * pae_status_text[] = {
        "No Firmware",
        "Not Started",
        "Running",
        "Stopped"
    };

	return sprintf(buf, "%s\n", pae_status_text[pae_status]);
}


static ssize_t write_status(struct device *dev, struct device_attribute *attr,
                            const char *buf, size_t count)
{
    int rv = -EINVAL;

    /* If string ends in newline, remove it from the comparison */
    int comparecount = count;
    if (count > 0 && buf[count-1] == '\n') {
        --comparecount;
    }

    if (strnicmp(buf, "stop", comparecount) == 0) {
        /* "stop" or "STOP" will remove the PAE from the packet pipeline */
        if (pae_status == PAE_STOPPED) {
            printk(KERN_INFO "PAE Already stopped");
            rv = 0;
        } else {
            rv = pae_stop();
        }
    } else if (strnicmp(buf, "init", comparecount) == 0) {
        /* "init" or "INIT" will set up the PAE without loading firmware    */
        /* This is not typically needed, as a firmware load will do an init */
        /* As no firmware will be running, it will block packets.           */
        if (pae_status != PAE_STOPPED) {
            printk(KERN_INFO "PAE Already running");
            rv = 0;
        } else {
            rv = pae_init();
        }
    }

    return (rv == 0) ? count : rv;
}

static DEVICE_ATTR(status, S_IRUSR | S_IWUSR, read_status, write_status);

static struct attribute *pae_attributes[] = {
	&dev_attr_status.attr,
    &dev_attr_pae_action_add.attr,
    &dev_attr_pae_action_delete.attr,
    &dev_attr_pae_action_update.attr,
    &dev_attr_pae_memory.attr,
    &dev_attr_pae_tuple_rule_add.attr,
    &dev_attr_pae_tuple_rule_delete.attr,
    &dev_attr_pae_tuple_mask.attr,
    &dev_attr_pae_arp_watch_add.attr,
    &dev_attr_pae_arp_watch_remove.attr,
    &dev_attr_pae_arp_watch_timer.attr,
    NULL
};


static const struct attribute_group pae_group = {
	.attrs = pae_attributes,
};


static struct attribute *pae_stats_attributes_initial[] = {
    &dev_attr_pae_idle_time.attr,
    &dev_attr_pae_cpu_load.attr,
    &dev_attr_action_stats[0][0].dev_attr.attr,
    &dev_attr_action_stats[0][1].dev_attr.attr,
    &dev_attr_action_stats[0][2].dev_attr.attr,
    NULL
};


static const struct attribute_group pae_stats_group = {
	.attrs = pae_stats_attributes_initial,
    .name = "stats"
};

/* /\* Interrupt Service Routine *\/ */
/* irqreturn_t iproc_pae_isr(int irq, void *devid) */
/* { */
/*     printk("BOOO\n"); */

/*     return IRQ_HANDLED; */
/* } */

static int __devinit pae_probe(struct platform_device *pdev)
{
    int rv;
    printk(KERN_INFO "PAE Probe: %p\n", pdev);

	rv = sysfs_create_group(&pdev->dev.kobj, &pae_group);
    if (rv) {
		goto failed_group_create;
    }

	rv = sysfs_create_group(&pdev->dev.kobj, &pae_stats_group);
    if (rv) {
		goto failed_stats_group_create;
    }

    rv = sysfs_create_bin_file(&pdev->dev.kobj, &fw_bin_attribute);
    if (rv) {
        goto failed_create_fw_bin;
    }

    rv = pae_debug_setup();
    if (rv) {
        goto failed_debug_setup;
    }

//    rv = request_irq(204, iproc_pae_isr, 0x0, "Yeah Baby", NULL);

#ifdef PAE_FAKE_SW_NAT64
    {
        extern void pae_netfilter_setup(void); /* in pae_sw_fake_nat64.c */
        pae_netfilter_setup();
    }
#endif /* PAE_FAKE_SW_NAT64 */
    return 0;

    /* pae_debug_exit(); */
    
 failed_debug_setup:
    sysfs_remove_bin_file(&pdev->dev.kobj, &fw_bin_attribute);

 failed_create_fw_bin:
	sysfs_remove_group(&pdev->dev.kobj, &pae_stats_group);

 failed_stats_group_create:
	sysfs_remove_group(&pdev->dev.kobj, &pae_group);

 failed_group_create:
    return rv;
}

static int __devexit pae_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "pae_remove: %p\n", pdev);

    pae_debug_exit();
    sysfs_remove_bin_file(&pdev->dev.kobj, &fw_bin_attribute);
	sysfs_remove_group(&pdev->dev.kobj, &pae_group);
	sysfs_remove_group(&pdev->dev.kobj, &pae_stats_group);
	/* device_remove_file(&pdev->dev, &sensor_dev_attr_temp1_max.dev_attr); */


	return 0;
}

static struct platform_driver pae_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "bcmiproc-pae",
	},
	.probe = pae_probe,
	.remove = __devexit_p(pae_remove),
};


static struct resource pae_resources[] = {
    {
        .start= 0x2a000000,
        .end= 0x2a018000,
        .flags= IORESOURCE_MEM,
        .name= "tcm-a"
    },
    {
        .start= 0x2a040000,
        .end= 0x2a058000,
        .flags= IORESOURCE_MEM,
        .name= "tcm-b"
    }
};


static int __init paemodule_init(void)
{
    int rv;

	rv = platform_driver_register(&pae_driver);
    if (rv) {
        goto driver_reg_failed;
    }

    pae_device = platform_device_register_simple("bcmiproc-pae", -1, pae_resources, ARRAY_SIZE(pae_resources));

    if (!pae_device) {
        goto device_reg_failed;
    }

    return 0;

    /* If more init is added: */
    /* platform_device_unregister(pae_device); */
    /* pae_device = NULL; */

 device_reg_failed:
	platform_driver_unregister(&pae_driver);

 driver_reg_failed:
    return rv;
}

static void __exit paemodule_exit(void)
{
    int rv = 0;

    if (pae_status != PAE_STOPPED) {
        rv = pae_stop();
    }

    platform_device_unregister(pae_device);
    platform_driver_unregister(&pae_driver);

    if (rv) {
        printk(KERN_INFO "PAE module shutdown failed: %d\n", rv);
    } else {
        printk(KERN_INFO "PAE module shutdown succeeded\n");
    }
        
    return;
}


module_init(paemodule_init);
module_exit(paemodule_exit);

MODULE_AUTHOR("Broadcom Corporation");
MODULE_DESCRIPTION("Driver for iProc PAE");
MODULE_LICENSE("GPL");
