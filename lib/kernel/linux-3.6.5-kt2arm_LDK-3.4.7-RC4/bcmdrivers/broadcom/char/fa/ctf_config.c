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
/* File Name   : ctf_config.c						      */
/*									      */
/* Description :							      */
/*    This file implements interface for configuring the Flow Acceleration    */
/*    H/W registers, CTF Control register, CTF memory access control register,*/
/*    CTF Broadcom header control register, and default CTF tables.	      */
/*									      */
/*    The initialization during module loading and cleanup are also	      */
/*    contained in this file.						      */
/*									      */
/*    A CTF character device driver is registered when this module is         */
/*    loaded. The intialization of CTF hardware and the configuration can be  */
/*    done via IOCTL calls into device driver.				      */
/*									      */
/*    A CTF Control Utility implements the IOCTL invocations.	              */
/*									      */
/******************************************************************************/

#include <linux/module.h>
#include <linux/if_ether.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include "mach/irqs.h"
#include "mach/iproc_regs.h"
#include "ctf_common.h"
#include "ctf_regs.h"
#include "fcache.h"

//#define CTF_ADD_STATIC_FLOWS 1

/* Based on 5-tuple in each direction. These values are obtained from hw as
 * a result of a miss. They are in bcm hdr (hash and bkt indx). To get these,
 * issue ctfctl config_loglvl 4; then send 1 packet from tester (which will 
 * result in a miss). Check the log lines for fc_receive, the decimal value of
 * index is printed on the console
 */
#define CTF_VAL_ENTRY_INDX 176 /* dirn 0 */
#define CTF_VAL_ENTRY_PAIR_INDX 512 /* dirn 1 *//* Fill in the correct val */

#undef  CTF_DECL
#define CTF_DECL(x)     #x,

#define MAC_ADDR_LEN	6
#define NAPT_POOL_EXT	0
#define NAPT_POOL_INT	1

#define CTF_FLOW_CTL_ON()
#define CTF_FLOW_CTL_OFF()

/* In smp mode, locks up flow table display command, so we keep these macros
 * empty
 */
#define CTF_LOCK_SECTION           /* spin_lock(&ctf_lock); */
#define CTF_UNLOCK_SECTION         /* spin_unlock(&ctf_lock); */

extern void fc_status(void);
extern void fc_flush(void);
extern int getbrcmtag(void);
extern int __init ctf_drv_init(void);
extern void __exit ctf_drv_exit(void);
void *et_get_hndl(uint unit);
extern int robo_write_reg(void *rih, unsigned char page, unsigned char reg, void *val, int len);
extern int robo_read_reg(void *rih, unsigned char page, unsigned char reg, void *val, int len);

void fc_dump_flow_cache(int entry, uint32_t hw_entries_only);

static int ctf_debug_add_ipv4_flow_entry(unsigned long *args);
static int ctf_debug_add_ipv6_flow_entry(unsigned long *args, uint8_t *sip,
                                         uint8_t *dip);
int ctf_add_napt_pool_entry(unsigned char *smac, uint8_t *pt_ix, int flag);
int ctf_next_hop_table_add_entry(uint32_t vlan_tag, uint8_t *da, uint8_t *sa_ix,                                 uint8_t frm_type, uint8_t op);

extern spinlock_t ctf_lock;

bool napt_flow_init_done	= CTF_TRUE;
bool napt_next_hop_init_done	= CTF_TRUE;
static int ctf_initialized	= CTF_FALSE;

/* Forward declaration */
int is_ctf_initialized(void);

/*
 * Switch CTF will directly hardware forward flows:
 */
uint32_t ctf_tx_chan = 0;


uint32_t ctf_dbg_log_level = CTF_DBG_DEFAULT_LVL; /* No debug log */

/*
 * The following two hooks are initialized by the GMAC Driver.
 *
 * Hooks to GMAC Driver function that will configure the WAN port:
 *
 * (*ctf_wan_port_enable)()
 * - Enable Rx, Enable Tx on WAN port
 *
 * (*ctf_wan_port_disable)()
 * - Disable Rx, Disable Tx on WAN port
 */
int (*ctf_wan_port_enable)  = (int *) NULL;
int (*ctf_wan_port_disable) = (int *) NULL;

/* Callback names */
const char * ctf_cb_name[] =
{
    CTF_DECL(CTF_STATUS)
    CTF_DECL(CTF_RESET)
    CTF_DECL(CTF_INIT)
    CTF_DECL(CTF_ENABLE)
    CTF_DECL(CTF_DISABLE)
    CTF_DECL(CTF_REGDUMP)
    CTF_DECL(CTF_DUMPPT)
    CTF_DECL(CTF_DUMPNHT)
    CTF_DECL(CTF_CFG_LOG)
    CTF_DECL(CTF_ADDFE)
    CTF_DECL(CTF_ADDPT)
    CTF_DECL(CTF_ADDNHT)
};

struct ctf_pooltable_entry {
    unsigned char mac[6];
    uint8_t ext;
    uint8_t valid;
    uint8_t dummy;
};

static struct ctf_pooltable_entry ctf_drv_pooltbl[4];

/* Searches pool entries maintained by the driver. Entries should be added by
 * ctf_add_napt_pool_entry() prior to calling this function.
 * Return Value: index of the location where MAC address was found. The index
 *                matches actual hw entry pool entry location
 *               -1 if not found 
 */
int ctf_search_pooltable(unsigned char *mac)
{
    int i;
    int size = sizeof(char) * 6;

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
        printk("\n%s: Mac input: 0x%02X%02X%02X%02X%02X%02X\n",
               __func__, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    for (i = 0; i < 4; i++) {

		if ((ctf_drv_pooltbl[i].valid) &&
            (memcmp(mac, ctf_drv_pooltbl[i].mac, size) == 0)) {

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
			    printk(KERN_DEBUG "%s: Found mac in internal pool table at:"
                       " %d\n", __func__, i);
            }

            break;
		}
	}

    if (i >= 4) {
        i = -1; /* Not found */

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {

            printk(KERN_DEBUG "%s: Mac not found in internal pool table\n",
                   __func__);
        }
    }

    return i;
}

int ctf_set_ctf_bypass_mode(unsigned int enable)
{
	volatile ctfctl_reg_t		ctf_ctl;
	volatile void __iomem		*ctf_reg;

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word = readl(ctf_reg);
    //printk("\n\nBEFORE: ctf control reg = 0x%08X\n\n", ctf_ctl.word);
	ctf_ctl.f.bypass_ctf = enable & 0x1;
	writel(ctf_ctl.word, ctf_reg);

	// ctf_ctl.word = readl(ctf_reg); /* For debug purpose */
    // printk("\n\nAFTER: ctf control reg = 0x%08X\n\n", ctf_ctl.word);

	return CTF_SUCCESS;
}

static int ctf_set_hwq_thr(unsigned int thr)
{
	volatile ctfctl_reg_t		ctf_ctl;
	volatile void __iomem		*ctf_reg;

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word = readl(ctf_reg);
    //printk("\n\nBEFORE: ctf control reg = 0x%08X\n\n", ctf_ctl.word);
    ctf_ctl.f.hwq_thresh = thr & 0x1FF; /* 9 bit field */
	writel(ctf_ctl.word, ctf_reg);

	//ctf_ctl.word = readl(ctf_reg);
    //printk("\n\nAFTER: ctf control reg = 0x%08X\n\n", ctf_ctl.word);

    return (CTF_SUCCESS);
}

/*
 * Function   : ctf_ctl_init
 * Description: Configure the Control Registers for Switch CTF.
 */
void ctf_ctl_init(void)
{
	ctfctl_reg_t		ctf_ctl;
	ctfctl_brcmhdr_reg_t	brcmhdr_ctl;
	ctfctl_l2skip_reg_t	l2skip_ctl;
	ctfctl_l3napt_reg_t	l3napt_ctl;
	ctf_intmask_reg_t	intmask;
	ctf_rxstatmask_reg_t	rxstat_en;
	ctf_errstatmask_reg_t	errmask;
    int retries;
//    int bcmtag;
#ifdef CTFDBG
	ctf_dbgctl_reg_t	dbg_ctl;
#endif
	void __iomem		*ctf_reg;
    uint32_t init_done_bits;

    /* Check ctf_initialized flag to see if this function was called earlier */
    if (ctf_initialized	== CTF_TRUE) {

        printk(KERN_DEBUG "%s: FA is already initialized\n", __func__);

        return;
    }


	/*
	 * Initialize control register
	 * By default set:
	 * 	Normal Mode
	 * 	Enable CTF
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word 		= readl(ctf_reg);
	ctf_ctl.word		|= CTFCTL_CFG_DSBL_MAC_DA_CHECK;
	ctf_ctl.word		|= CTFCTL_CFG_NAPT_FLOW_INIT;
	ctf_ctl.word		|= CTFCTL_CFG_NEXT_HOP_INIT;
	ctf_ctl.word		|= CTFCTL_CFG_HWQ_INIT;
	ctf_ctl.word		|= CTFCTL_CFG_LAB_INIT;
	ctf_ctl.word		|= CTFCTL_CFG_HB_INIT;
	ctf_ctl.word		|= CTFCTL_CFG_CRC_OWRT;
	writel(ctf_ctl.word, ctf_reg);
	ctf_ctl.word = readl(ctf_reg);

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk("FA Control register 0x%08x\n", ctf_ctl.word);

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_INTSTAT_REG_OFFSET);

    init_done_bits = CTF_INTSTAT_HB_INIT_DONE | CTF_INTSTAT_LAB_INIT_DONE |
                     CTF_INTSTAT_HWQ_INIT_DONE | CTF_INTSTAT_NXT_HOP_INIT_DONE |
                     CTF_INTSTAT_NAPT_FLOW_INIT_DONE;

    retries = 0;
    do {
        msleep(1);
	    ctf_ctl.word = readl(ctf_reg);
        retries++;
    } while (!(ctf_ctl.word & init_done_bits) && (retries <= 50));

    if (retries > 50)  {
    
	    printk("%s: ERROR: Hardware initialization did not complete after %d"
               " attempts. Reg 0x30 = 0x%08X. Exiting. \n",
               __func__, retries, ctf_ctl.word);

        return;
    }

    printk("FA: #%d: 0x%X:0x%X",
           retries, CTFCTL_INTSTAT_REG_OFFSET, ctf_ctl.word);

    printk("\nFA internal buffers and memories initialized");

	/*
	 * BRCM Header Control Register
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_BRCMHDR_REG_OFFSET);

	brcmhdr_ctl.word	= 0xF;

	writel(brcmhdr_ctl.word, ctf_reg);
	brcmhdr_ctl.word = readl(ctf_reg);

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk(KERN_ERR "CTF Header Control Register 0x%08x\n",
               brcmhdr_ctl.word);

	/*
	 * L2 Skip Control Register
	 * By default set:
	 * 	SNAP conversion (Enabled)
	 * 	ET Skip (Disabled)
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_L2SKIP_REG_OFFSET);
	l2skip_ctl.word = 0;
	l2skip_ctl.f.eth2_to_snap_conv	= 1;
	writel(l2skip_ctl.word, ctf_reg);

	/*
	 * L3 NAPT Control Register
	 * By default set:
	 * 	Hash seed
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_L3NAPT_REG_OFFSET);
	l3napt_ctl.word = readl(ctf_reg);
	l3napt_ctl.f.hash_sel = 1;
	l3napt_ctl.f.hits_clr_on_rd_en = 0; /* Refer Jira NSTAR-740. This bug
                                           is valid for both ipv4 and ipv6 */
	l3napt_ctl.f.napt_timestamp = 0;
	l3napt_ctl.f.napt_hash_seed = htons(0x4321);
	writel(l3napt_ctl.word, ctf_reg);

	/*
	 * Interrupt Mask Register
	 * By default enable all
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_INTMASK_REG_OFFSET);
	intmask.word = 0;
	writel(intmask.word, ctf_reg);

	/*
	 * Receive Status Mask Register. Enable all bits to count any failure
     * conditions
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_RXMASK_REG_OFFSET);
	rxstat_en.word = 0xFF;
	writel(rxstat_en.word, ctf_reg);

    /* Enable check of ipv4 checksum */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_L3IPV4_REG_OFFSET);
	ctf_ctl.word 		= readl(ctf_reg);
	ctf_ctl.word 		|= CTFCTL_L3_IPV4_CKSUM_EN;
	writel(ctf_ctl.word, ctf_reg);

	/*
	 * Error Status Mask Register
	 * By default enable all
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_ERR_STATUS_MASK_REG_OFFSET);
	errmask.word = 0;
	writel(errmask.word, ctf_reg);

    memset(ctf_drv_pooltbl, 0, sizeof(ctf_drv_pooltbl));


    ctf_initialized	= CTF_TRUE;

	/*
	 * Debug Control Register
	 */
#ifdef CTFDBG
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_CTL_REG_OFFSET);
	dbg_ctl.f.reg_dbg = CTF_DBG_REG;
	dbg_ctl.f.force_all_miss = CTF_DBG_FORCE_ALL_MISS;
	dbg_ctl.f.force_all_hit = CTF_DBG_FORCE_ALL_HIT;
	dbg_ctl.f.ok_to_send = 0x7;
#endif
}

int is_ctf_initialized(void)
{
    return(ctf_initialized);
}


/*
 * Function   : ctf_bcmhdr_parse_cfg 
 * Description: Configure default rule in CTF to remove BRCM Tag.
 *              This rule will also be used by all packets that fail NAT
 *              and use the feature.
 */
static int ctf_bcmhdr_parse_cfg(void)
{
	int 			ret = CTF_SUCCESS;
	ctfctl_brcmhdr_reg_t	bcmhdr_ctl;
	void __iomem		*ctf_reg;

	printk(KERN_DEBUG "Disabling BRCM Header Parsing...\n");

	/*
	 * Enable BRCM_HDR_PARSE_IGNORE_EXCEPTIONS
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_BRCMHDR_REG_OFFSET);
	bcmhdr_ctl.word = readl(ctf_reg);
	bcmhdr_ctl.f.hdr_parse_ign_excep |= CTFCTL_BRCM_HDR_PARSE_IGN_EN;
	writel(bcmhdr_ctl.word, ctf_reg);

	return ret;
}

/* This is a debug function to add a pair of entries at 2 locations. First entry
 * is added at location specified by index. Second entry is added at either
 * location index + 1, or at index1 if index1 is a positive number
 */
static int ctf_add_static_flows(int proto, int index, int index1)
{
    unsigned long args[] = { /* For ipv6, sip, dip are not used */
                      0, /* sip */
                      0, /* dip */
                      0,  /* sport */
                      0,  /* dport */
                      0,  /* proto */
                      0, /* pt_ix */
                      0, /* nht_ix */
                      0, /* table index 0 <= i < 1024 */
                  };
    uint8_t ix = 0;
    uint8_t ix1 = 0;
    unsigned char mac[6];


    if ((proto != 4) && (proto != 6)) {
        printk(KERN_INFO "\nERROR: %s: Invalid proto val passed (%d)\n",
               __func__, proto);
        return CTF_ERROR;
    }

/* --------------------------------------------------------------- */

    /* dirn = 0, has vlantag 2*/
    mac[0] = 0xA1;
    mac[1] = 0x88;
    mac[2] = 0x00;
    mac[3] = 0x19;
    mac[4] = 0x11;
    mac[5] = 0xD0;
    ctf_next_hop_table_add_entry(2, &mac[0], &ix1, 0, 1);

    /* dirn = 0, has external = 1 in pool table */
    mac[0] = 0x00;
    mac[1] = 0x90;
    mac[2] = 0x4C;
    mac[3] = 0x76;
    mac[4] = 0xA5;
    mac[5] = 0x89;

    /* Refer fc_transmit for args */
    ctf_add_napt_pool_entry(&mac[0], &ix, 0x1);
    printk("\npt_ix=%u\n", ix);

    /* dirn = 0 */
    args[2] = 63;  /* sport */
    args[3] = 63;  /* dport */
    args[4] = 0; /* proto */
    args[5] = ix; /* pool table */
    args[6] = ix1; /* next hop table */
    args[7] = index & 0x3FF; /* location in the table, < 1024 */

    if (proto == 4) {
        args[0] = 0xC0A80002; /* sip */
        args[1] = 0x0A0A0102; /* dip */

        ctf_debug_add_ipv4_flow_entry(&args[0]);
    } else {
        uint8_t sip[16], dip[16];

        sip[0] = 0xFE; /* msb */
        sip[1] = 0x80;
        sip[2] = 0x00;
        sip[3] = 0x00;
        sip[4] = 0x00;
        sip[5] = 0x00;
        sip[6] = 0x00;
        sip[7] = 0x00;
        sip[8] = 0xA3;
        sip[9] = 0x88;
        sip[10] = 0x00;
        sip[11] = 0xFF;
        sip[12] = 0xFE;
        sip[13] = 0x1D;
        sip[14] = 0x11;
        sip[15] = 0x58; /* lsb */

        dip[0] = 0x35; /* msb */
        dip[1] = 0x55;
        dip[2] = 0x55;
        dip[3] = 0x55;
        dip[4] = 0x66;
        dip[5] = 0x66;
        dip[6] = 0x66;
        dip[7] = 0x66;
        dip[8] = 0x77;
        dip[9] = 0x77;
        dip[10] = 0x77;
        dip[11] = 0x77;
        dip[12] = 0x88;
        dip[13] = 0x88;
        dip[14] = 0x88;
        dip[15] = 0x88; /* lsb */

        ctf_debug_add_ipv6_flow_entry(&args[0], sip, dip);
    }

/* --------------------------------------------------------------- */

    /* dirn = 1, has vlantag 1 */
    mac[0] = 0xA1;
    mac[1] = 0x88;
    mac[2] = 0x00;
    mac[3] = 0x1D;
    mac[4] = 0x11;
    mac[5] = 0x58;
    ctf_next_hop_table_add_entry(1, &mac[0], &ix1, 0, 1);

    /* dirn = 1, has external = 0 in pool table */
    mac[0] = 0x00;
    mac[1] = 0x90;
    mac[2] = 0x4C;
    mac[3] = 0x76;
    mac[4] = 0xA5;
    mac[5] = 0x87;

    /* Refer fc_transmit for args */
    ctf_add_napt_pool_entry(&mac[0], &ix, 0x0);
    printk("\npt_ix=%u\n", ix);

    /* dirn = 1 */
    args[2] = 63; 
    args[3] = 63;
    args[4] = 0;
    args[5] = ix;
    args[6] = ix1;
    if (index1 == -1) {
        args[7] = (index & 0x3FF) + 1; /* next location in the table, < 1024 */
    }
    else {
        args[7] = index1 & 0x3FF; /* pair location in the table, < 1024 */
    }

    if (proto == 4) {
        args[0] = 0x0A0A0102;
        args[1] = 0x0A0A0101;

        ctf_debug_add_ipv4_flow_entry(&args[0]);
    } else {
        uint8_t sip[16], dip[16];

        sip[0] = 0x35;
        sip[1] = 0x55;
        sip[2] = 0x55;
        sip[3] = 0x55;
        sip[4] = 0x66;
        sip[5] = 0x66;
        sip[6] = 0x66;
        sip[7] = 0x66;
        sip[8] = 0x77;
        sip[9] = 0x77;
        sip[10] = 0x77;
        sip[11] = 0x77;
        sip[12] = 0x88;
        sip[13] = 0x88;
        sip[14] = 0x88;
        sip[15] = 0x88;

        dip[0] = 0x35;
        dip[1] = 0x55;
        dip[2] = 0x55;
        dip[3] = 0x55;
        dip[4] = 0x66;
        dip[5] = 0x66;
        dip[6] = 0x66;
        dip[7] = 0x66;
        dip[8] = 0x77;
        dip[9] = 0x77;
        dip[10] = 0x77;
        dip[11] = 0x77;
        dip[12] = 0x88;
        dip[13] = 0x88;
        dip[14] = 0x88;
        dip[15] = 0x87;

        ctf_debug_add_ipv6_flow_entry(&args[0], sip, dip);
    }


    return CTF_SUCCESS;
}

/*
 * Function   : ctf_init
 * Description: Default Initial Configuration of Switch CTF.
 *
 *              ctf_ctl_init()
 *              - Control Registers intialized.
 *
 *              ctf_bcmhdr_parse_cfg()
 *              - BRCM Tag removal rule is enabled 
 *
 */
static int ctf_init(void)
{
	int	ret = CTF_SUCCESS;

    if (ctf_initialized	== CTF_TRUE) {
        printk(KERN_DEBUG "FA already initialized\n");
	    return ret;
    }

	printk(KERN_DEBUG "CTF Driver Initialization...\n");

	/*
	 * Initialize control register
	 */
	ctf_ctl_init();

	/*
	 * Bypass BRCM Header parsing
	 */
	ret = ctf_bcmhdr_parse_cfg();


	return ret;
}

/*
 * Function   : ctf_status_show
 * Description: Display the status of both CTF including NAPT flow table.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_status_show(void)
{
	int				ret = CTF_SUCCESS;
	void __iomem			*ctf_reg;

	printk(KERN_DEBUG "Forwarding Accelerator Status\n");

	/*
	 * Flow cache learning status
	 */
	fc_status();

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA);
	printk(KERN_INFO "Reg base address: 0x%X\n\n", (unsigned int)ctf_reg);

	printk(KERN_INFO "Error Status(0x%X):               %u\n",
           CTF_ERR_STATUS_REG_OFFSET, 
           readl(ctf_reg + CTF_ERR_STATUS_REG_OFFSET));

	printk(KERN_INFO "Flow ECC error addr (0x%X):       %u\n",
           0x84, 
           readl(ctf_reg + 0x84));

	printk(KERN_INFO "Next hop error addr (0x%X):       %u\n",
           0x88, 
           readl(ctf_reg + 0x88));

	printk(KERN_INFO "HWQ error addr (0x%X):            %u\n",
           0x8C, 
           readl(ctf_reg + 0x8C));

	printk(KERN_INFO "lab error addr (0x%X):            %u\n",
           0x90, 
           readl(ctf_reg + 0x90));

	printk(KERN_INFO "hb error addr (0x%X):             %u\n",
           0x94, 
           readl(ctf_reg + 0x94));

	printk(KERN_INFO "hwq max depth seen (0x%X):        %u\n",
           0x98, 
           readl(ctf_reg + 0x98));

	printk(KERN_INFO "lat. buff. max depth seen (0x%X): %u\n",
           0x9C, 
           readl(ctf_reg + 0x9C));

	/*
	 * Dump the counts
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_HITCOUNT_REG_OFFSET);
    
	printk(KERN_INFO "Hits: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "Miss:	\t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "SNAP Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "Etype Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "Vers Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "Frag Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "Proto Ext Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "IPv4 CKSum Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "IPv4 Opts Fail: \t\t\t %u\n", readl(ctf_reg));
	ctf_reg += (1 << 2);
	printk(KERN_INFO "IPv4 Hdr Len Fail: \t\t\t %u\n", readl(ctf_reg));



    return ret;
}

/*
 * Function   : ctf_clear_napt_flow_table
 * Description: Reset NAPT Flow Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_clear_napt_flow_table(int index)
{
	int			ret = CTF_SUCCESS;
	ctfctl_reg_t		ctf_ctl;
	ctfctl_memacc_reg_t	memacc_ctl;
	ctf_data_reg_t		table_data;
	void __iomem		*ctf_reg;
    int retries;

	printk(KERN_DEBUG "Clearing NAPT Flow Table...\n");

    CTF_FLOW_CTL_ON();
	/*
	 * Just setting the control register NAPT flow table bit
	 * does the job.
	 */
	if (index == -1) {
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
		ctf_ctl.word = CTFCTL_CFG_NAPT_FLOW_INIT;
		writel(ctf_ctl.word, ctf_reg);
		napt_flow_init_done = CTF_FALSE;

	    ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_INTSTAT_REG_OFFSET);

        retries = 0;
        do {
            udelay(1000);
	        ctf_ctl.word = readl(ctf_reg);
            retries++;
        } while (!(ctf_ctl.word & CTF_INTSTAT_NAPT_FLOW_INIT_DONE) &&
                  (retries <= 50));

        if (retries > 50)  {
    
	        printk("%s: ERROR: Hardware initialization did not complete after"
                   " %d attempts. Reg 0x30 = 0x%08X. Exiting. \n",
                   __func__, retries, ctf_ctl.word);

		    ret = CTF_ERROR;
        }

		goto done;
	}

	if (index > 0x3FF) {
		printk(KERN_ERR "%s: Illegal Table Index<%d>\n",
			__func__, index);
		ret = CTF_ERROR;
		goto done;
	}

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);

	/*
	 * To clear the NAPT Flow Table set the CTF_MEM_ACC_CONTROL reg bits:
	 *	- Set rd_wr_n = 1
	 *	- Set select = NAPT_FLOW_TABLE
	 *	- Set index = 0
	 */
	memset(&table_data, 0, sizeof(ctf_data_reg_t));
	memacc_ctl.word = (CTFCTL_MEMACC_RD_WR_N |
			   CTFCTL_MEMACC_NAPT_FLOW_TAB |
			   (((unsigned int)index & 0x3FF) << 0));
done:
    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_get_flow_entry
 * Description: Get NAPT Flow Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_get_flow_entry(uint32_t hashix, uint32_t bktix,
			      ctf_data_reg_t *entry)
{
	int			ret = CTF_SUCCESS;
	ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem		*ctf_reg;
	volatile unsigned		reg_val;
    unsigned int    retries;

    if (ctf_dbg_log_level >= CTF_DBG_MEDIUM_LVL)
	    printk("Retrieving NAPT Flow Table Entry"
	    		" hash<%08x> bucket<%08x>...\n", hashix, bktix);

    CTF_FLOW_CTL_ON();

    /* Check if previous memory access completed */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	reg_val = readl(ctf_reg);

    if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

        retries = 0;

        do {
	        reg_val = readl(ctf_reg);
            retries++;
        } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

        if (retries >= 32)  {
    
            CTF_FLOW_CTL_OFF();

	        printk("%s: ERROR: Memory access did not complete after %d"
                   " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                   __func__, retries, reg_val);

            return CTF_ERROR;
        }
    }


	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);

	memset(&memacc_ctl.word, 0, sizeof(int));
	memacc_ctl.word = ((1 << 12) | (((hashix & 0x000000FF) << 2) |
			(bktix & 0x00000003)));

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Mem Acc Ctl <0x%08x>\n", memacc_ctl.word);

	writel(memacc_ctl.word, ctf_reg);

	/* Read and discard data 0 */
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	entry->data[0] = readl(ctf_reg);

	/* Now read the table data d7..d0 */
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA7_REG_OFFSET);
	entry->data[7] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA6_REG_OFFSET);
	entry->data[6] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA5_REG_OFFSET);
	entry->data[5] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA4_REG_OFFSET);
	entry->data[4] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA3_REG_OFFSET);
	entry->data[3] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA2_REG_OFFSET);
	entry->data[2] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
	entry->data[1] = readl(ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	entry->data[0] = readl(ctf_reg);

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
	writel(0x0, ctf_reg); /* Set rd_wr_n=0 */

    /* Wait for the read access to complete */
    retries = 0;

    ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

    do {
	    reg_val = readl(ctf_reg);
        retries++;
    } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

    if (retries >= 32)  {
    
        CTF_FLOW_CTL_OFF();

	    printk("%s: ERROR: Memory access did not complete after %d"
               " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        return CTF_ERROR;
    }

    CTF_FLOW_CTL_OFF();

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Flow Table data[0x%08x %08x %08x %08x %08x %08x %08x %08x]\n",
    		   entry->data[7], entry->data[6], entry->data[5], entry->data[4],
    		   entry->data[3], entry->data[2], entry->data[1], entry->data[0]);

	return ret;
}

/*
 * Function   : ctf_match_flow_entry
 * Description: Get NAPT Flow Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_match_flow_entry(napt_flow_t *s_flow, napt_flow_t *d_flow)
{
	int	ret = CTF_SUCCESS;

	return ret;
}

/*
 * Function   : ctf_set_flow_entry
 * Description: Set NAPT Flow Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_set_flow_entry(uint32_t hashix, uint32_t bktix,
			      ctf_data_reg_t *entry)
{
	int			ret = CTF_SUCCESS;
	ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem		*ctf_reg;
	volatile unsigned		acc_ctl;
	volatile unsigned		reg_val;
    unsigned int    retries;

    if (ctf_dbg_log_level >= CTF_DBG_MEDIUM_LVL)
	    printk("Updating NAPT Flow Table Entry"
	    	" hash<%d> bucket<%d>...\n", hashix, bktix);

    /* Check if previous memory access completed */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

    retries = 0;

    do {
	    reg_val = readl(ctf_reg);
        retries++;
    } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

    if (retries >= 32)  {
    
	    printk("%s: ERROR: Memory access did not complete after %d"
               " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        return CTF_ERROR;
    }

    CTF_FLOW_CTL_ON();
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);

	memset(&memacc_ctl.word, 0, sizeof(int));
	memacc_ctl.f.table_sel		= 0;
	memacc_ctl.f.rd_wr_n		= 0;
	memacc_ctl.f.table_index	= ((hashix * 4) + bktix);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Mem Acc Ctl <0x%08x>\n", memacc_ctl.word);

	writel(memacc_ctl.word, ctf_reg);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	    acc_ctl = readl(ctf_reg);
	    printk("Mem Acc Control <0x%08x>\n", acc_ctl);
    }

	/* Now write the table data d7..d0 */
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA7_REG_OFFSET);
	writel(entry->data[7], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA6_REG_OFFSET);
	writel(entry->data[6], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA5_REG_OFFSET);
	writel(entry->data[5], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA4_REG_OFFSET);
	writel(entry->data[4], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA3_REG_OFFSET);
	writel(entry->data[3], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA2_REG_OFFSET);
	writel(entry->data[2], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
	writel(entry->data[1], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	writel(entry->data[0], ctf_reg);

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

    retries = 0;

    do {
	    reg_val = readl(ctf_reg);
        retries++;
    } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

    if (retries >= 32)  {
    
        CTF_FLOW_CTL_OFF();

	    printk("%s: ERROR: Memory access did not complete after %d"
               " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        return CTF_ERROR;
    }

    CTF_FLOW_CTL_OFF();

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Added Entry [0x%08x %08x %08x %08x %08x %08x %08x %08x]\n",
		       entry->data[7], entry->data[6], entry->data[5], entry->data[4],
		       entry->data[3], entry->data[2], entry->data[1], entry->data[0]);

	return ret;
}

/*
 * Function   : ctf_activate_flow
 * Description: Activate NAPT Flow Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_activate_flow(volatile napt_flow_t *flow_p)
{
	int			ret = CTF_SUCCESS;
	ctf_data_reg_t		table_data;
	uint32_t		hashix;
	uint32_t		bktix;
	uint32_t		data;
    int i, j;
    uint32_t val;

	BUG_ON(flow_p == FLOW_NULL);

	hashix	= flow_p->key.hash_index;
	bktix	= flow_p->key.bkt_index;

    if (ctf_dbg_log_level >= CTF_DBG_MEDIUM_LVL)
	    printk("FA Activating flow %u (hash<%d> bucket<%d>)...\n",
               (hashix << 2) + bktix, hashix, bktix);

	/* Match flow */

	/* Set the valid bit */
	if (flow_p->l3_proto == ETH_P_IP) {
		flow_p->fe.ipv4.valid = 1;
		data = 0;
		data = (flow_p->fe.ipv4.timestamp |
			((flow_p->fe.ipv4.hits & 0x1FFFFFFF) << 3));
		table_data.data[0] = data;
		data = (((flow_p->fe.ipv4.hits & 0xFFFFFFF8) >> 29)	|
			(flow_p->fe.ipv4.action << 3)			|
			(flow_p->fe.ipv4.dest_dma << 5)			|
			(flow_p->fe.ipv4.remap_sa_idx << 6)		|
			(flow_p->fe.ipv4.next_hop_idx << 8)		|
			((flow_p->fe.ipv4.remap_ip & 0x1FFFF) << 15));
		table_data.data[1] = data;
		data = (((flow_p->fe.ipv4.remap_ip & 0xFFFE0000) >> 17) |
			(flow_p->fe.ipv4.remap_port << 15)		|
			((flow_p->fe.ipv4.dport & 0x1) << 31));
		table_data.data[2] = data;
		data = (((flow_p->fe.ipv4.dport & 0xFFFE) >> 1)		|
			(flow_p->fe.ipv4.sport << 15)			|
			(flow_p->fe.ipv4.proto << 31));
		table_data.data[3] = data;
		data = (flow_p->fe.ipv4.dip);
		table_data.data[4] = data;
		data = (flow_p->fe.ipv4.sip);
		table_data.data[5] = data;
		data = (flow_p->fe.ipv4.direction			|
			((flow_p->fe.ipv4.tag_dest_map & 0x3FF) << 1)	|
			((flow_p->fe.ipv4.tag_ts & 0x1) << 11)		|
			((flow_p->fe.ipv4.tag_te & 0x3) << 12)		|
			((flow_p->fe.ipv4.tag_tc & 0x7) << 14)		|
			((flow_p->fe.ipv4.tag_oc & 0x7) << 17)		|
			(flow_p->fe.ipv4.valid << 20));
		table_data.data[6] = data;
		data = (flow_p->fe.ipv4.ipv4_entry << 31);
		table_data.data[7] = data;

        if (ctf_dbg_log_level > CTF_DBG_LOW_LVL) {

	        printk("Timestamp: %x\n", flow_p->fe.ipv4.timestamp);
	        printk("Hits: %x\n", flow_p->fe.ipv4.hits);
	        printk("Action: %x\n", flow_p->fe.ipv4.action);
	        printk("Dest DMA: %x\n", flow_p->fe.ipv4.dest_dma);
	        printk("Remap SA Index: %x\n", flow_p->fe.ipv4.remap_sa_idx);
	        printk("Next Hop Index: %x\n", flow_p->fe.ipv4.next_hop_idx);
	        printk("Remap IP: %x\n", flow_p->fe.ipv4.remap_ip);
	        printk("Remap Port: %x\n", flow_p->fe.ipv4.remap_port);
	        printk("Dest Port: %x\n", flow_p->fe.ipv4.dport);
	        printk("Src Port: %x\n", flow_p->fe.ipv4.sport);
	        printk("Protocol: %x\n", flow_p->fe.ipv4.proto);
	        printk("Dest IP: %x\n", flow_p->fe.ipv4.dip);
	        printk("Src IP: %x\n", flow_p->fe.ipv4.sip);
	        printk("Direction: %x\n", flow_p->fe.ipv4.direction);
	        printk("Tag Dest Map: %x\n", flow_p->fe.ipv4.tag_dest_map);
	        printk("Tag TS: %x\n", flow_p->fe.ipv4.tag_ts);
	        printk("Tag TE: %x\n", flow_p->fe.ipv4.tag_te);
	        printk("Tag TC: %x\n", flow_p->fe.ipv4.tag_tc);
	        printk("Tag OC: %x\n", flow_p->fe.ipv4.tag_oc);
	        printk("Valid: %x\n", flow_p->fe.ipv4.valid);
	        printk("IPv4 Entry: %x\n", flow_p->fe.ipv4.ipv4_entry);
        }

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	        printk("\nIpv4 Flow Entry Data\n");
		    printk("0x %08X %08X %08X %08X %08X %08X %08X %08X\n",
		            table_data.data[7], table_data.data[6],
		            table_data.data[5], table_data.data[4],
	            	table_data.data[3], table_data.data[2],
	            	table_data.data[1], table_data.data[0]);
        }

	} else {

        /* Set Word 1 first */
        /* Copy dip. Array dip[] is in big endian format, so msb's of ipv6 addr
         * are in lowest dip[] locations
         */
        for (i = 0, j = 3; i < 16; i += 4, j--) {

            val = (flow_p->fe.ipv6.dip[i + 0]) << 24;
            val |= (flow_p->fe.ipv6.dip[i + 1]) << 16;
            val |= (flow_p->fe.ipv6.dip[i + 2]) << 8;
            val |= (flow_p->fe.ipv6.dip[i + 3]);

            table_data.data[j] = val;
        }

        /* Copy sip. Read comment above  */
        for (i = 0, j = 7; i < 16; i += 4, j--) {

            val = (flow_p->fe.ipv6.sip[i + 0]) << 24;
            val |= (flow_p->fe.ipv6.sip[i + 1]) << 16;
            val |= (flow_p->fe.ipv6.sip[i + 2]) << 8;
            val |= (flow_p->fe.ipv6.sip[i + 3]);

            table_data.data[j] = val;
        }

        /* Note: bktix+1 for word 1 */
	    ret |= ctf_set_flow_entry(hashix, bktix + 1, &table_data);

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {

	        printk("SIP : 0x %08X %08X %08X %08X\n", 
                   *((uint32_t *)(flow_p->fe.ipv6.sip + 12)), 
                   *((uint32_t *)(flow_p->fe.ipv6.sip + 8)),
                   *((uint32_t *)(flow_p->fe.ipv6.sip + 4)), 
                   *((uint32_t *)(flow_p->fe.ipv6.sip + 0)));
	        printk("DIP: 0x %08X %08X %08X %08X\n", 
                   *((uint32_t *)(flow_p->fe.ipv6.dip + 12)), 
                   *((uint32_t *)(flow_p->fe.ipv6.dip + 8)),
                   *((uint32_t *)(flow_p->fe.ipv6.dip + 4)), 
                   *((uint32_t *)(flow_p->fe.ipv6.dip + 0)));

	        printk("\nIpv6 Flow Entry Data (word1) ret%d\n", ret);
		    printk("0x %08X %08X %08X %08X %08X %08X %08X %08X\n",
		            table_data.data[7], table_data.data[6],
	            	table_data.data[5], table_data.data[4],
            		table_data.data[3], table_data.data[2],
	            	table_data.data[1], table_data.data[0]);
        }

        /* Set Word 0 next; this word has the 'valid' bit */

        /* data 0 */
		data = flow_p->fe.ipv6.timestamp;
		table_data.data[0] = data;

        /* data 1 */
        data = 0; /* Sets hit count bits to 0 */
        data |= ((flow_p->fe.ipv6.action << 3)		|
			(flow_p->fe.ipv6.dest_dma << 5)			|
			(flow_p->fe.ipv6.remap_sa_idx << 6)		|
			(flow_p->fe.ipv6.next_hop_idx << 8)		|
			(flow_p->fe.ipv6.remap_ip[15] << 15)     | /* 17 bits of r_ip */
			(flow_p->fe.ipv6.remap_ip[14] << 23)     |
			((flow_p->fe.ipv6.remap_ip[13] & 0x1) << 31));
		table_data.data[1] = data;

        /* data 2 */
        data  = flow_p->fe.ipv6.remap_ip[13] >> 1; /* 7 bits */
        data |= flow_p->fe.ipv6.remap_ip[12] << 7;
        data |= flow_p->fe.ipv6.remap_ip[11] << 15;
        data |= flow_p->fe.ipv6.remap_ip[10] << 23;
	    data |= ((flow_p->fe.ipv6.remap_ip[9] & 0x1) << 31);
		table_data.data[2] = data;

        /* data 3 */
        data  = flow_p->fe.ipv6.remap_ip[9] >> 1; /* 7 bits */
        data |= flow_p->fe.ipv6.remap_ip[8] << 7;
        data |= flow_p->fe.ipv6.remap_ip[7] << 15;
        data |= flow_p->fe.ipv6.remap_ip[6] << 23;
	    data |= ((flow_p->fe.ipv6.remap_ip[5] & 0x1) << 31);
		table_data.data[3] = data;

        /* data 4 */
        data  = flow_p->fe.ipv6.remap_ip[5] >> 1; /* 7 bits */
        data |= flow_p->fe.ipv6.remap_ip[4] << 7;
        data |= flow_p->fe.ipv6.remap_ip[3] << 15;
        data |= flow_p->fe.ipv6.remap_ip[2] << 23;
	    data |= ((flow_p->fe.ipv6.remap_ip[1] & 0x1) << 31);
		table_data.data[4] = data;

        /* data 5 */
        data  = flow_p->fe.ipv6.remap_ip[1] >> 1; /* 7 bits */
        data |= flow_p->fe.ipv6.remap_ip[0] << 7; /* r_ip filled */
		data |= flow_p->fe.ipv6.remap_port << 15; /* r_port filled */
		data |= (flow_p->fe.ipv6.dport & 0x1)<< 31; /* lsb of dport */
		table_data.data[5] = data;

        /* data 6 */
        data = flow_p->fe.ipv6.dport >> 1; /* msb's of dport, 15 bits */
        data |= flow_p->fe.ipv6.sport << 15;
        data |= flow_p->fe.ipv6.proto << 31;
		table_data.data[6] = data;

        /* data 7 */
        data = flow_p->fe.ipv6.direction;
        data |= flow_p->fe.ipv6.tag_dest_map << 1;
        data |= flow_p->fe.ipv6.tag_ts << 11;
        data |= flow_p->fe.ipv6.tag_te << 12;
        data |= flow_p->fe.ipv6.tag_tc << 14;
        data |= flow_p->fe.ipv6.tag_oc << 17;
        data |= flow_p->fe.ipv6.valid  << 20;
        data |= flow_p->fe.ipv6.ipv4_entry  << 31;
		table_data.data[7] = data;

        /* Write word0 of the entry */
	    ret |= ctf_set_flow_entry(hashix, bktix, &table_data);

        if (ctf_dbg_log_level > CTF_DBG_LOW_LVL) {

	        printk("Ipv4 Entry: %u\n", flow_p->fe.ipv6.ipv4_entry);
	        printk("Valid: %u\n", flow_p->fe.ipv6.valid);
	        printk("Tag opcode: %u\n", flow_p->fe.ipv6.tag_oc);
	        printk("Tag TC: %u\n", flow_p->fe.ipv6.tag_tc);
	        printk("Tag TE: %u\n", flow_p->fe.ipv6.tag_te);
	        printk("Tag TS: %u\n", flow_p->fe.ipv6.tag_ts);
	        printk("Tag Dest Map: %u\n", flow_p->fe.ipv6.tag_dest_map);
	        printk("Direction: %u\n", flow_p->fe.ipv6.direction);
	        printk("Protocol: %u\n", flow_p->fe.ipv6.proto);
	        printk("Src Port: %u\n", flow_p->fe.ipv6.sport);
	        printk("Dest Port: %u\n", flow_p->fe.ipv6.dport);
	        printk("Remap Port: %u\n", flow_p->fe.ipv6.remap_port);
	        printk("Remap IP: 0x %08X %08X %08X %08X\n", 
                   *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 0)), 
                   *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 4)),
                   *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 8)), 
                   *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 12)));
	        printk("Next Hop Index: %u\n", flow_p->fe.ipv6.next_hop_idx);
	        printk("Remap SA Index: %u\n", flow_p->fe.ipv6.remap_sa_idx);
	        printk("Dest DMA: %u\n", flow_p->fe.ipv6.dest_dma);
	        printk("Action: %u\n", flow_p->fe.ipv6.action);
	        printk("Hits: %u\n", flow_p->fe.ipv6.hits);
	        printk("Timestamp: %u\n", flow_p->fe.ipv6.timestamp);
        }

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	        printk("\nIpv6 Flow Entry Data (word0) ret%d\n", ret);
		    printk("0x %08X %08X %08X %08X %08X %08X %08X %08X\n",
		            table_data.data[7], table_data.data[6],
	            	table_data.data[5], table_data.data[4],
            		table_data.data[3], table_data.data[2],
	            	table_data.data[1], table_data.data[0]);
        }
	}

	/* Write the data back to the specified index */
	ret |= ctf_set_flow_entry(hashix, bktix, &table_data);

	return ret;
}

/*
 * Function   : ctf_deactivate_flow
 * Description: Deactivate NAPT Flow Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_deactivate_flow(volatile napt_flow_t *flow_p)
{
	int			ret = CTF_SUCCESS;
	ctf_data_reg_t		table_data;
	uint32_t		hashix;
	uint32_t		bktix;
	uint32_t		valid;

    //if (ctf_dbg_log_level >= CTF_DBG_MEDIUM_LVL)
	    printk(KERN_DEBUG "%s: De-activating NAPT Flow Table Entry hash<%d> bucket<%d>...\n",  __func__, flow_p->key.hash_index, flow_p->key.bkt_index);

	BUG_ON(flow_p == FLOW_NULL);

	hashix	= flow_p->key.hash_index;
	bktix	= flow_p->key.bkt_index;

	/* Read the specified index of flow table */
	ret = ctf_get_flow_entry(hashix, bktix, &table_data);

	/* Match flow */

	/* Reset the valid bit */
	if (flow_p->l3_proto == ETH_P_IP) {
		uint32_t mask = (1 << 20);
		valid = table_data.data[6];
		valid &= ~mask;
		table_data.data[6] = valid;
		
	} else {
		uint32_t mask = (1 << 20);
		valid = table_data.data[7];
		valid &= ~mask;
		table_data.data[7] = valid;
	}


	/* Write the data back to the specified index */
	ret |= ctf_set_flow_entry(hashix, bktix, &table_data);
	printk(KERN_DEBUG "%s: Ret val = %u\n",  __func__, ret);

	return ret;
}

/*
 * Function   : ctf_dump_napt_pool_table
 * Description: Dump NAPT Pool Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_dump_napt_pool_table(void)
{
	int			ret = CTF_SUCCESS;
	ctfctl_memacc_reg_t	memacc_ctl;
	void __iomem		*ctf_reg;
	uint32_t		data[8];
	uint8_t			ix = 0;

    if (ctf_dbg_log_level >= CTF_DBG_DEFAULT_LVL)
	    printk(KERN_DEBUG "Dump NAPT Pool Table...\n");

    CTF_FLOW_CTL_ON();

	for (ix = 0; ix < CTF_MAX_POOL_TABLE_INDEX; ix++) {
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		/* Read the entire table */
		memacc_ctl.word	= ((1 << 12) | (1 << 10) | ix);

		/* Write the table lookup control word */
		writel(memacc_ctl.word, ctf_reg);

		/* Read and discard data[0] */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

		/* Now read the table data d1..d0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
		data[1] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

        /* Set rd_wr_n to 0 after a read access */
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		writel(0x0, ctf_reg);

		printk("<%d> [0x%08x %08x]\n", ix, data[1], data[0]);
	}

    printk("\nInternal:");
    for (ix = 0; ix < 4; ix++) {
        printk("\n<%d>: mac: 0x%02X%02X%02X%02X%02X%02X, ",
               ix, ctf_drv_pooltbl[ix].mac[0],
               ctf_drv_pooltbl[ix].mac[1], ctf_drv_pooltbl[ix].mac[2], 
               ctf_drv_pooltbl[ix].mac[3], ctf_drv_pooltbl[ix].mac[4], 
               ctf_drv_pooltbl[ix].mac[5]);
        printk("ext: %u, valid: %u", ctf_drv_pooltbl[ix].ext,
               ctf_drv_pooltbl[ix].valid);
    }
    printk("\n\n");

    CTF_FLOW_CTL_OFF();

	return ret;
}

/*
 * Function   : ctf_lookup_pool_table
 * Description: Lookup Next Hop Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_lookup_pool_table(uint8_t *index, uint8_t *smac)
{
	int			ret = CTF_ERROR;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem		*ctf_reg;
	volatile uint32_t		data[8];
	uint8_t			ix = 0;
	uint8_t			mac_addr[6];
	extern bool		ctf_drv_reg_flag;
	volatile unsigned		reg_val;
    unsigned int    retries;

	if (ctf_drv_reg_flag != 1) {
		goto done;
	}

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Lookup Pool Table...\n");

	for (ix = 0; ix < CTF_MAX_POOL_TABLE_INDEX; ix++) {
		/* Read the entire table */
		memacc_ctl.word	= ((1 << 12) | (1 << 10) | ix);

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("Mem Acc Ctl 0x%08x\n", memacc_ctl.word);

		/* Write CTF_MEM_ACC_CONTROL */
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		writel(memacc_ctl.word, ctf_reg);

		/* Read and discard data 0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

		/* Now read the table data d7..d0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
		data[1] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

        ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
        writel(0x0, ctf_reg); /* Set rd_wr_n = 0 */

        /* Check if memory access completed */
	    ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	    reg_val = readl(ctf_reg);

        if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

            retries = 0;

            do {
	            reg_val = readl(ctf_reg);
                retries++;
            } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

            if (retries >= 32)  {
    
	            printk("%s: ERROR: Memory access did not complete after %d"
                       " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                       __func__, retries, reg_val);

                ret = CTF_ABORT;
                goto done;
            }
        }

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("ix<%d>:\t[0x%08x %08x]\n", ix, data[1], data[0]);

		mac_addr[5] = (uint8_t)((data[0] >> 1) & 0xFF);
		mac_addr[4] = (uint8_t)((data[0] >> 8) & 0xFF);
		mac_addr[3] = (uint8_t)((data[0] >> 16) & 0xFF);
		mac_addr[2] = (uint8_t)(((data[0] >> 24) & 0xFF) |
					((data[1] & 0x00000001) << 31));
		mac_addr[1] = (uint8_t)((data[1] >> 1) & 0xFF);
		mac_addr[0] = (uint8_t)((data[1] >> 8) & 0xFF);


        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("mac %02x:%02x:%02x:%02x:%02x:%02x "
			    "smac  %02x:%02x:%02x:%02x:%02x:%02x\n",
			    mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
			    mac_addr[4], mac_addr[5], smac[0], smac[1], smac[2], smac[3],
			    smac[4], smac[5]);

		if (memcmp(smac, mac_addr, sizeof(char) * 6) == 0) {
            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			    printk("FOUND MAC IN POOL TABLE ix %d\n", ix);
			ret = CTF_SUCCESS;
			*index = ix;
			goto done;
		}
	}

done:
        if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk("Pool Table entry %s: Index<%d>\n",
		       (ret == CTF_SUCCESS) ? "found" : "not found", *index);
	return ret;
}

/*
 * Function   : ctf_add_napt_pool_entry
 * Description: Reset NAPT Pool Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_add_napt_pool_entry(unsigned char *smac, uint8_t *pt_ix, int flag)
{
	int			ret = CTF_SUCCESS;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem		*ctf_reg;
	volatile uint32_t		data[8];
	int8_t			ix = 0;
	static uint8_t		index = 0;
	volatile unsigned		reg_val;
    unsigned int    retries;

    if (ctf_dbg_log_level >= CTF_DBG_MEDIUM_LVL)
	    printk("Add Pool Table Entry...\n");

    CTF_FLOW_CTL_ON();

	/* Lookup the entry using DA */
	ret = ctf_lookup_pool_table(&ix, smac);

	if (ret == CTF_ABORT) {
       ret = CTF_ERROR; /* Indirect operation failed, do not proceed */
	   goto done;
    }

	if (ret == CTF_SUCCESS) {
		/* Found it */

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
		printk("Found SA [%02x:%02x:%02x:%02x:%02x:%02x]\n",
			    smac[0], smac[1], smac[2], smac[3], smac[4], smac[5]);

		*pt_ix = (uint8_t)ix;
		goto done;
	}

	/* Add the entry to the next available location */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
	memacc_ctl.word = ((1 << 10) | index);
	writel(memacc_ctl.word, ctf_reg);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("MEMACC_CTL Index %d <0x%08x> readl <0x%08x>\n", index,
		    memacc_ctl.word, readl(ctf_reg));


	if (flag == 0) {
		/* Internal */
		data[0] = (0 | (smac[5] << 1) | (smac[4] << 9) |
			  (smac[3] << 17) | (smac[2] & 0x7F) << 25);
	} else {
		/* External */
		data[0] = (1 | (smac[5] << 1) | (smac[4] << 9) |
			  (smac[3] << 17) | (smac[2] & 0x7F) << 25);
	}
	data[1] = ((((smac[2] & 0x80) >> 7) << 0) | (smac[1] << 1) | (smac[0] << 9));

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk("Adding Pool Table Entry [0x%08x %08x]\n", data[1], data[0]);

	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
	writel(data[1], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	writel(data[0], ctf_reg);
	*pt_ix = (uint8_t)index;

    /* Check if memory access completed */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	reg_val = readl(ctf_reg);

    if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

        retries = 0;

        do {
	        reg_val = readl(ctf_reg);
            retries++;
        } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

        if (retries >= 32)  {
    
	        printk("%s: ERROR: Memory access did not complete after %d"
                   " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                   __func__, retries, reg_val);

            ret = CTF_ERROR;
            goto done;
        }
    }

    ctf_drv_pooltbl[index].ext = flag;
    memcpy(ctf_drv_pooltbl[index].mac, smac, sizeof(char) * 6);
    ctf_drv_pooltbl[index].valid = 1;

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL) {

	    printk("Added Pool Table Entry index <%d> [0x%08x %08x]\n", index,
		       data[1], data[0]);
    }

	if (index >= (CTF_MAX_POOL_TABLE_INDEX - 1)) {
		index = 0;
	} else {
		index++;
	}

done:
    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_clear_napt_pool_table
 * Description: Reset NAPT Pool Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_clear_napt_pool_table(int index)
{
	int			ret = CTF_SUCCESS;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	uint32_t data;
	volatile void __iomem		*ctf_reg;
    int retries;
    volatile unsigned reg_val;

	printk(KERN_DEBUG "Clearing NAPT Pool Table, index=%d...\n", index);

    data = 0;

    CTF_FLOW_CTL_ON();

	if (index == -1) {

		/*
	 	 * Read the next index and clear until the end of table
	 	 */
		index = 0;
		do {
	        ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
	        memacc_ctl.word = (CTFCTL_MEMACC_NAPT_POOL_TAB | index);
	        writel(memacc_ctl.word, ctf_reg);

	        ctf_reg =
	                IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
	        writel(data, ctf_reg);
	        ctf_reg =
	                IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	        writel(data, ctf_reg);

            /* Check if memory access completed */
	        ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	        reg_val = readl(ctf_reg);

            if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

                retries = 0;

                do {
	                reg_val = readl(ctf_reg);
                    retries++;
                } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

                if (retries >= 32)  {
    
	                printk("%s: ERROR: Memory access did not complete after %d"
                           " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                           __func__, retries, reg_val);

                    ret = CTF_ERROR;
                    goto done;
                }
            }

            index++;
		} while(index < CTF_MAX_POOL_TABLE_INDEX);

        /* Clear all internal pool table entries */
        memset(ctf_drv_pooltbl, 0, sizeof(ctf_drv_pooltbl));

		goto done;
	}

	if (index >= CTF_MAX_POOL_TABLE_INDEX) {
		printk(KERN_ERR "%s: Illegal Table Index<%d>\n",
			__func__, index);
		ret = CTF_ERROR;
		goto done;
	}

	/*
	 * To clear the NAPT Pool Table set the CTF_MEM_ACC_CONTROL reg bits:
	 *	- Set rd_wr_n = 1
	 *	- Set select = NAPT_POOL_TABLE
	 *	- Set index = 0
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
	memacc_ctl.word = (CTFCTL_MEMACC_RD_WR_N |
			   CTFCTL_MEMACC_NAPT_POOL_TAB |
			   (((unsigned int)index & 0x3FF) << 0));

    /* Clear driver's corresponding internal entry */
    memset(&ctf_drv_pooltbl[index], 0, sizeof(struct ctf_pooltable_entry));

done:
    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_clear_next_hop_table
 * Description: Reset Next Hop Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_clear_next_hop_table(int index)
{
	int			ret = CTF_SUCCESS;
	void __iomem		*ctf_reg;
	ctfctl_reg_t		ctf_ctl;
	ctf_data_reg_t		table_data;
	ctfctl_memacc_reg_t	memacc_ctl;
	int			ix;
    int retries;

	printk(KERN_DEBUG "Clearing Next Hop Table, index=%d...\n", index);

	/*
	 * Just setting the control register NAPT flow table bit
	 * does the job.
	 */
    CTF_FLOW_CTL_ON();
	memset(&ctf_ctl.word, 0, sizeof(int));
	if (index == -1) {
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
		ctf_ctl.word = CTFCTL_CFG_NEXT_HOP_INIT;
		writel(ctf_ctl.word, ctf_reg);
		napt_next_hop_init_done = CTF_FALSE;

	    ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_INTSTAT_REG_OFFSET);

        retries = 0;
        do {
            udelay(1000);
	        ctf_ctl.word = readl(ctf_reg);
            retries++;
        } while (!(ctf_ctl.word & CTF_INTSTAT_NXT_HOP_INIT_DONE) &&
                  (retries <= 50));

        if (retries > 50)  {
    
	        printk("%s: ERROR: Hardware initialization did not complete after"
                   " %d attempts. Reg 0x30 = 0x%08X. Exiting. \n",
                   __func__, retries, ctf_ctl.word);

            return CTF_ERROR;
        }

		goto done;
	}

	if (index > 0x3FF) {
		printk(KERN_ERR "%s: Illegal Table Index<%d>\n",
			__func__, index);
		ret = CTF_ERROR;
		goto done;
	}

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);

	/*
	 * To clear the NAPT Flow Table set the CTF_MEM_ACC_CONTROL reg bits:
	 *	- Set rd_wr_n = 0
	 *	- Set select = NAPT_FLOW_TABLE
	 *	- Set index = 0
	 */
	memset(&table_data, 0, sizeof(ctf_data_reg_t));
	memacc_ctl.word = (CTFCTL_MEMACC_RD_WR_N |
			   CTFCTL_MEMACC_NEXT_HOP_TAB |
			   (((unsigned int)index & 0x3FF) << 0));
	writel(memacc_ctl.word, ctf_reg);

	/* Now clear the entry with index */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA7_REG_OFFSET);
	for (ix = 7; ix >= 0; ix--) {
		writel(table_data.data[ix], ctf_reg);
		ctf_reg -= 4;
	}
done:
    CTF_FLOW_CTL_OFF();
	return ret;
}


/*
 * Function   : ctf_dump_next_hop_table
 * Description: Dump Next Hop Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_dump_next_hop_table(void)
{
	int			ret = CTF_SUCCESS;
	ctfctl_memacc_reg_t	memacc_ctl;
	void __iomem		*ctf_reg;
	uint32_t		data[8];
	uint8_t			ix = 0;

    if (ctf_dbg_log_level >= CTF_DBG_DEFAULT_LVL)
	    printk(KERN_DEBUG "Dump Next Hop Table...\n");

    CTF_FLOW_CTL_ON();
	for (ix = 0; ix < CTF_MAX_NEXTHOP_TABLE_INDEX; ix++) {
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		/* Read the entire table */
		memacc_ctl.word	= ((1 << 12) | (2 << 10) | ix);

		/* Write the table lookup control word */
		writel(memacc_ctl.word, ctf_reg);

		/* Read and discard data[0] */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

		/* Now read the table data d2..d0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA2_REG_OFFSET);
		data[2] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
		data[1] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

        /* Set rd_wr_n to 0 after a read access */
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		writel(0x0, ctf_reg);

		printk("<%d> [0x%08x %08x %08x]\n", ix, data[2], data[1], data[0]);
	}
    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_lookup_next_hop_table
 * Description: Lookup Next Hop Table
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_lookup_next_hop_table(uint8_t *index, uint8_t *da)
{
	int			ret = CTF_ERROR;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem		*ctf_reg;
	volatile uint32_t		data[8];
	uint8_t			ix = 0;
	uint8_t			mac_addr[6];
	volatile unsigned		reg_val;
    unsigned int    retries;

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk(KERN_DEBUG "Lookup Next Hop Table...\n");

	for (ix = 0; ix < CTF_MAX_NEXTHOP_TABLE_INDEX; ix++) {
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
		/* Read the entire table */
		memacc_ctl.word	= ((1 << 12) | (2 << 10) | ix);
		/* Write CTF_MEM_ACC_CONTROL */
		writel(memacc_ctl.word, ctf_reg);

		/* Read and discard data 0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

		/* Now read the table data d7..d0 */
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA2_REG_OFFSET);
		data[2] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
		data[1] = readl(ctf_reg);
		ctf_reg =
		IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
		data[0] = readl(ctf_reg);

        ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
        writel(0x0, ctf_reg); /* Set rd_wr_n = 0 */
    
        /* Check if memory access completed */
	    ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	    reg_val = readl(ctf_reg);

        if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

            retries = 0;

            do {
	            reg_val = readl(ctf_reg);
                retries++;
            } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

            if (retries >= 32)  {
    
	            printk("%s: ERROR: Memory access did not complete after %d"
                       " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                       __func__, retries, reg_val);

                ret = CTF_ABORT;

                goto done;
            }
        }

        if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
		    printk("ix<%d>:\t[0x%08x %08x %08x]\n", ix, data[2], data[1],
			    data[0]);

		mac_addr[5] = (uint8_t)((data[0] >> 19) & 0xFF);
		mac_addr[4] = (uint8_t)((data[0] >> 27) & 0x1F) | \
				(uint8_t)((data[1] & 0x00000007) << 5);
		mac_addr[3] = (uint8_t)((data[1] >> 3) & 0xFF);
		mac_addr[2] = (uint8_t)((data[1] >> 11) & 0xFF);
		mac_addr[1] = (uint8_t)((data[1] >> 19) & 0xFF);
		mac_addr[0] = (uint8_t)((data[1] >> 27) & 0x1F) | (uint8_t)((data[2] & 0x7) << 5);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		printk("mac %02x:%02x:%02x:%02x:%02x:%02x "
			"da  %02x:%02x:%02x:%02x:%02x:%02x\n",
			mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3],
			mac_addr[4], mac_addr[5], da[0], da[1], da[2], da[3],
			da[4], da[5]);

		if (memcmp(da, mac_addr, sizeof(char) * 6) == 0) {
            if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
			    printk("FOUND MAC IN NEXT HOP TABLE ix %d\n", ix);
			ret = CTF_SUCCESS;
			*index = ix;
			goto done;
		}
	};

done:
    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk("Next Hop Table entry %s: Index<%d>\n",
	    	(ret == CTF_SUCCESS) ? "found" : "not found", *index);

	return ret;
}

/*
 * Function   : ctf_next_hop_table_add_entry
 * Description: Add Next Hop Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_next_hop_table_add_entry(uint32_t vlan_tag, uint8_t *da, uint8_t *sa_ix,                                 uint8_t frm_type, uint8_t op)
{
	int			ret = CTF_SUCCESS;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	volatile void __iomem	*ctf_reg;
	volatile uint32_t		data[8];
	int8_t			ix = 0;
	static uint8_t		index = 0;
	volatile unsigned		reg_val;
    unsigned int    retries;

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
	    printk(KERN_DEBUG "Add Next Hop Table Entry...\n");

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("DA [%02x:%02x:%02x:%02x:%02x:%02x]\n",
		       da[0], da[1], da[2], da[3], da[4], da[5]);

    CTF_FLOW_CTL_ON();
	/* Lookup the entry using DA */
	ret = ctf_lookup_next_hop_table(&ix, da);

	if (ret == CTF_ABORT) {
       ret = CTF_ERROR; /* Indirect operation failed, do not proceed */
	   goto done;
    }

	if (ret == CTF_SUCCESS) {
		/* Found it */

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		printk(KERN_DEBUG
			   "Found DA [%02x:%02x:%02x:%02x:%02x:%02x]\n",
			   da[0], da[1], da[2], da[3], da[4], da[5]);

		*sa_ix = (uint8_t)ix;
		goto done;
	}

	/* Add the entry to the next available location */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);
	memacc_ctl.word = ((2 << 10) | index);
	writel(memacc_ctl.word, ctf_reg);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("MEMACC_CTL Index %d <0x%08x> readl <0x%08x>\n",
		    index, memacc_ctl.word, readl(ctf_reg));


    /* Note: hard-coded values for bits 0, 1: outgoing mac hdr (ethernet II) and
     * 'use c_tag'
     */
	data[0] = ((frm_type & 0x1) | ((op & 0x3) << 1) | ((vlan_tag & 0x0000FFFF) << 3) | 
		 (da[5] << 19) | (da[4] & 0x1F) << 27);
	data[1] = (((da[4] & 0xE0) >> 5) | (da[3] << 3) | (da[2] << 11) |
		  (da[1] << 19) | (da[0] & 0x1F) << 27);
	data[2] = ((da[0] & 0xE0) >> 5);

	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA2_REG_OFFSET);
	writel(data[2], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA1_REG_OFFSET);
	writel(data[1], ctf_reg);
	ctf_reg =
	IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA0_REG_OFFSET);
	writel(data[0], ctf_reg);
	*sa_ix = (uint8_t)index;

    /* Check if memory access completed */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_DBG_MEMACC_REG_OFFSET);

	reg_val = readl(ctf_reg);

    if (reg_val & CTF_DBG_MEM_ACC_BUSY) {

        retries = 0;

        do {
	        reg_val = readl(ctf_reg);
            retries++;
        } while ((reg_val & CTF_DBG_MEM_ACC_BUSY) && (retries < 32));

        if (retries >= 32)  {
    
	        printk("%s: ERROR: Memory access did not complete after %d"
                   " attempts. Reg 0x70 = 0x%08X. Exiting. \n",
                   __func__, retries, reg_val);

            ret = CTF_ERROR;
            goto done;
        }
    }

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	   printk("Added Next Hop Entry index <%d> [0x%08x %08x %08x]\n", index,
		   data[2], data[1], data[0]);

	if (index > CTF_MAX_NEXTHOP_TABLE_INDEX) {
		index = 0;
	} else {
		index++;
	}

done:
    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_next_hop_table_del_entry
 * Description: Delete Next Hop Table Entry
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
static int ctf_next_hop_table_del_entry(uint8_t index, uint8_t *da)
{
	int			ret = CTF_SUCCESS;
	volatile void __iomem		*ctf_reg;
	int			ix = 0;
	volatile ctfctl_memacc_reg_t	memacc_ctl;
	volatile uint32_t		data[8];

	printk(KERN_DEBUG "Delete Next Hop Table Entry...\n");

    CTF_FLOW_CTL_ON();
	/* Lookup the entry using DA */
	ret = ctf_lookup_next_hop_table(&index, da);

	if (ret == CTF_ABORT) {

       CTF_FLOW_CTL_OFF();

       return (CTF_ERROR); /* Indirect operation failed, do not proceed */
    }

	if ((ret == CTF_SUCCESS) &&
	    (index < CTF_MAX_NEXTHOP_TABLE_INDEX)) {

		/* Found it */
		printk(KERN_DEBUG
			"Found Entry Index<0x%08x>\n", index);

		memacc_ctl.word		= 0; /* makes rd_wr_n = 0*/
		memacc_ctl.word		|= CTFCTL_MEMACC_NEXT_HOP_TAB;
		memacc_ctl.word		|= ((index & 0x3FF) << 0);
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_MEMACC_REG_OFFSET);

		writel(memacc_ctl.word, ctf_reg);
		memset((void *)data, 0, (sizeof(int) * 8));
		ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTF_MEM_ACC_DATA7_REG_OFFSET);
		/* Now write data 7..0 */
		for (ix = 7; ix >= 0; ix--) {
			writel(data[ix], ctf_reg);
			ctf_reg -= 4;
		}
	}

    CTF_FLOW_CTL_OFF();
	return ret;
}

/*
 * Function   : ctf_reset_tables
 * Description: Resets tables associated with CTF
 *
 * Parameters :
 *      table :	0 = NAPT Flow Table.
 *		1 = NAPT Pool Table.
 *		2 = Next Hop Table.
 *		3 = All Tables.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_reset_tables(int table)
{
	int	ret 	= CTF_SUCCESS;
	int	index 	= -1;

	printk(KERN_DEBUG "Reset CTF Tables %d", table);

	switch(table) {
	case NAPT_FLOW_TABLE:
		//ret = ctf_clear_napt_flow_table(index);
        fc_flush();
		break;

	case NAPT_POOL_TABLE:
		ret = ctf_clear_napt_pool_table(index);
		break;

	case NEXT_HOP_TABLE:
		ret = ctf_clear_next_hop_table(index);
		break;

	case CTF_ALL_TABLES:
        fc_flush();
		//ret = ctf_clear_napt_flow_table(index);
		ret |= ctf_clear_napt_pool_table(index);
		ret |= ctf_clear_next_hop_table(index);
		break;

	default:
		; /* bad choice */
	}

	return ret;
}

/*
 * Function   : ctf_enable
 * Description: Enables CTF and sets the mode to normal mode.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_enable(void)
{
	void __iomem            *ctf_reg;
	ctfctl_reg_t            ctf_ctl;

	printk(KERN_DEBUG "Enabling Flow Acceleration...\n");

	/*
	 * Set BYPASS_CTF bit 0 to configure normal mode.
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word = readl(ctf_reg);
	ctf_ctl.f.bypass_ctf = 0;
	writel(ctf_ctl.word, ctf_reg);

	/*
	 * Enable CTF interrupts
	 */
	enable_irq(BCM_INT_ID_FA);

	return CTF_SUCCESS;
}

/*
 * Function   : ctf_disable
 * Description: Disables CTF and configures it for bypass CTF mode.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_disable(void)
{
	void __iomem            *ctf_reg;
	ctfctl_reg_t            ctf_ctl;

	printk(KERN_DEBUG "Disabling Flow Acceleration...\n");

	/*
	 * Set BYPASS_CTF bit 0 to configure normal mode.
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word = readl(ctf_reg);
	ctf_ctl.f.bypass_ctf = 1;
	writel(ctf_ctl.word, ctf_reg);

	return CTF_SUCCESS;
}

/*
 * Function   : ctf_flush
 * Description: Aborts table entries in CTF tables.
 *
 * Parameters :
 *      table :	0 = NAPT Flow Table.
 *		1 = NAPT Pool Table.
 *		2 = Next Hop Table.
 *    index   : Table index
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_flush(int table, int index)
{
	int	ret = CTF_SUCCESS;

	printk(KERN_DEBUG "Reset CTF Table %d Index %d", table, index);

	switch(table) {
	case NAPT_FLOW_TABLE:
		ret = ctf_clear_napt_flow_table(index);
		break;

	case NAPT_POOL_TABLE:
		ret = ctf_clear_napt_pool_table(index);
		break;

	case NEXT_HOP_TABLE:
		ret = ctf_clear_next_hop_table(index);
		break;

	default:
		; /* bad choice */
	}

	return ret;
}

int ctf_update_timestamp(unsigned int ts)
{
	void __iomem            *ctf_reg;
	ctfctl_l3napt_reg_t	l3napt_ctl;

    ts &= CTFCTL_TIMESTAMP_MASK;  /* Time stamp is a 3-bit value */

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_L3NAPT_REG_OFFSET);

	l3napt_ctl.word = readl(ctf_reg);
//	printk("-- Present value of 0x18027c2c <0x%08x>\n", l3napt_ctl.word);

	l3napt_ctl.word &= ~(CTFCTL_L3NAPT_TIMESTAMP); /* clear field */
    l3napt_ctl.word |= (ts << 16);
    writel(l3napt_ctl.word, ctf_reg);

	l3napt_ctl.word = readl(ctf_reg);
//	printk("New value of 0x18027c2c <0x%08x>\n", l3napt_ctl.word);

    return CTF_SUCCESS;
}

int ctf_get_flow_entry_timestamp(uint32_t hashix, uint32_t bktix, 
                                 unsigned int *ts)
{
	ctf_data_reg_t		table_data;
	int			ret = CTF_SUCCESS;


	/* Read the specified index of flow table */
	ret = ctf_get_flow_entry(hashix, bktix, &table_data);

	*ts = table_data.data[0] & CTFCTL_TIMESTAMP_MASK;

    return ret;
}

/*
 * Function   : ctf_regdump
 * Description: Dumps registers for FA block
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_regdump(void)
{
	void __iomem            *ctf_reg;
	ctfctl_reg_t            ctf_ctl;
	ctfctl_brcmhdr_reg_t	brcmhdr_ctl;
	ctfctl_l3napt_reg_t	l3napt_ctl;

	printk(KERN_DEBUG "Dumping registers...\n");

	/*
	 * Set BYPASS_CTF bit 0 to configure normal mode.
	 */
	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_REG_OFFSET);
	ctf_ctl.word = readl(ctf_reg);
	printk("FA Control 0x18027C00 <0x%08x>\n", ctf_ctl.word);

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_BRCMHDR_REG_OFFSET);
	brcmhdr_ctl.word = readl(ctf_reg);
	printk("BRCM Header Control 0x18027C08 <0x%08x>\n", brcmhdr_ctl.word);

	ctf_reg = IOMEM(IPROC_CTF_BASE_VA + CTFCTL_L3NAPT_REG_OFFSET);
	l3napt_ctl.word = readl(ctf_reg);
	printk("L3 NAPT Control 0x18027c2c <0x%08x>\n", l3napt_ctl.word);

	return CTF_SUCCESS;
}

/* Dump all flow table entries. 'entry' is 0-based */
void ctf_dump_flow_table_entries(int entry)
{
	ctf_data_reg_t		table_data;
    int tbl_index, num_entries, index, found;
    unsigned int valid_bit, ipv4_entry;

	printk(KERN_INFO "\n%s: Flow table \n", __func__);
	printk(KERN_INFO "\nNote: Only valid entries are displayed\n\n");

    found = 0;
    num_entries = (entry == -1) ? FCACHE_MAX_ENTRIES : entry + 1;
    index = (entry == -1) ? 0 : entry;

    CTF_LOCK_SECTION;

    for (tbl_index = index; tbl_index < num_entries; ) {

	    ctf_get_flow_entry(tbl_index >> 2, tbl_index & 0x3, &table_data);

        ipv4_entry = table_data.data[7] & (1 << 31);

        valid_bit = ipv4_entry ? (table_data.data[6] & (1 << 20)) :
                                 (table_data.data[7] & (1 << 20));

        if (valid_bit) {

            found++;

            /* Entry is valid */
	        printk(KERN_INFO
                   "@ %d%s: 0x[%08X %08X %08X %08X %08X %08X %08X %08X]\n",
    	    	   tbl_index, ipv4_entry ? "(v4)   " : "(v6-W0)", table_data.data[7],
                   table_data.data[6], table_data.data[5], table_data.data[4],
                   table_data.data[3], table_data.data[2], table_data.data[1],
                   table_data.data[0]);

            /* For ipv6 entry, read the next word too */
            if (!ipv4_entry) {

                tbl_index++;

	            ctf_get_flow_entry(tbl_index >> 2, tbl_index & 0x3,
                                   &table_data);
	            printk(KERN_INFO
                       "@ %d%s: 0x[%08X %08X %08X %08X %08X %08X %08X %08X]\n",
    	    	       tbl_index, ipv4_entry ? "(v4)   " : "(v6-W1)", table_data.data[7], 
                       table_data.data[6], table_data.data[5], 
                       table_data.data[4], table_data.data[3], 
                       table_data.data[2], table_data.data[1], 
                       table_data.data[0]);
            }

        }

        tbl_index++;

    }

    CTF_UNLOCK_SECTION;

    if (entry == -1) {

        printk (KERN_INFO "\n%s: Found %d entr%s \n\n\n",
                __func__, found, (found != 1) ? "ies" : "y");
    }

    return;
}

static int ctf_debug_add_ipv6_flow_entry(unsigned long *args, uint8_t *sip,
                                         uint8_t *dip)
{
    napt_flow_t flow;
    volatile napt_flow_t *flow_p = &flow;
    int ret;
    static int dir = 0;
    static uint8_t prev_sip[16];

             //printk("\n\n%s: sip=0x%08X, dip=0x%08X, sport=0x%08X, dport=0x%08X, proto=0x%08X\n\n", __func__, args[0], args[1], args[2], args[3], args[4]);

    /* Note: 2 consecutive locations for a connection */
    flow_p->pair_entry_index = args[7] + 1;
                              //++entry_index % FCACHE_MAX_ENTRIES;

    flow_p->key.hash_index = (args[7] / 4) % FCACHE_MAX_ENTRIES;
                             //(entry_index / 4) % FCACHE_MAX_ENTRIES;

    flow_p->key.bkt_index = args[7] % 4;
                            //entry_index % 4;

    flow_p->fe.ipv6.proto = args[4];
    memcpy(((napt_flow_t *)flow_p)->fe.ipv6.sip, sip, 16);
    flow_p->fe.ipv6.sport = args[2];
    memcpy(((napt_flow_t *)flow_p)->fe.ipv6.dip, dip, 16);
    flow_p->fe.ipv6.dport =  args[3];

    flow_p->fe.ipv6.ipv4_entry = 0;
    flow_p->fe.ipv6.valid = 1;

    flow_p->fe.ipv6.direction = dir % 2; /* change */

    if ((dir % 2) == 0) { /* outbound */
        memcpy(((napt_flow_t *)flow_p)->fe.ipv6.remap_ip, dip, 16); /* dip - 1 */ /* change */
        flow_p->fe.ipv6.remap_ip[15] = dip[15] - 1;

        flow_p->fe.ipv6.remap_port = args[2]; /* use sport */ /* change */
        memcpy(prev_sip, sip, 16);

    }
    else { /* inbound */

        memcpy(((napt_flow_t *)flow_p)->fe.ipv6.remap_ip, prev_sip, 16); /* sip of outbound entry*/ /* change */

        flow_p->fe.ipv6.remap_port = args[3]; /* use dport */ /* change */
    }

    dir++;

    flow_p->fe.ipv6.next_hop_idx = args[6]; /*remap_nh_idx++ % 128;  change */
    flow_p->fe.ipv6.remap_sa_idx = args[5]; /*remap_sa_idx++ % 4;  change */

    flow_p->fe.ipv6.dest_dma = 0;
    flow_p->fe.ipv6.action = 1;
    flow_p->fe.ipv6.hits = 0;
    flow_p->fe.ipv6.timestamp = 0;

    flow_p->location = 1;

    flow_p->fe.ipv6.tag_oc = 0;
    flow_p->fe.ipv6.tag_tc = 0;
    flow_p->fe.ipv6.tag_dest_map = 0;

    flow_p->l3_proto = 0x86DD;
    flow_p->l4_proto = 17; /* change */


    flow_p->ct = NULL;
    flow_p->nfct = NULL;

    ret = ctf_activate_flow(flow_p);

    if (ret != CTF_SUCCESS)
        printk("%s: ERROR: ctf_activate_flow failed", __func__);

    return ret;
}

static int ctf_debug_add_ipv4_flow_entry(unsigned long *args)
{
    napt_flow_t flow;
    volatile napt_flow_t *flow_p = &flow;
    int ret;
    static int dir = 0;
    static unsigned int prev_sip = 0;

             //printk("\n\n%s: sip=0x%08X, dip=0x%08X, sport=0x%08X, dport=0x%08X, proto=0x%08X\n\n", __func__, args[0], args[1], args[2], args[3], args[4]);

    /* Note: 2 consecutive locations for a connection */
    flow_p->pair_entry_index = args[7] + 1;
                              //++entry_index % FCACHE_MAX_ENTRIES;

    flow_p->key.hash_index = (args[7] / 4) % FCACHE_MAX_ENTRIES;
                             //(entry_index / 4) % FCACHE_MAX_ENTRIES;

    flow_p->key.bkt_index = args[7] % 4;
                            //entry_index % 4;

    flow_p->fe.ipv4.proto = args[4];
    flow_p->fe.ipv4.sip   = args[0];
    flow_p->fe.ipv4.sport = args[2];
    flow_p->fe.ipv4.dip   =  args[1];
    flow_p->fe.ipv4.dport =  args[3];


    flow_p->fe.ipv4.ipv4_entry = 1;
    flow_p->fe.ipv4.valid = 1;

    flow_p->fe.ipv4.direction = dir % 2; /* change */

    if ((dir % 2) == 0) { /* outbound */
        flow_p->fe.ipv4.remap_ip = args[1] - 1; /* dip - 1 */ /* change */
        flow_p->fe.ipv4.remap_port = args[2]; /* use sport */ /* change */
        prev_sip = args[0];
    }
    else { /* inbound */
        flow_p->fe.ipv4.remap_ip = prev_sip; /* sip of outbound entry*/ /* change */
        flow_p->fe.ipv4.remap_port = args[3]; /* use dport */ /* change */
    }

    dir++;

    flow_p->fe.ipv4.next_hop_idx = args[6]; /*remap_nh_idx++ % 128;  change */
    flow_p->fe.ipv4.remap_sa_idx = args[5]; /*remap_sa_idx++ % 4;  change */

    flow_p->fe.ipv4.dest_dma = 0;
    flow_p->fe.ipv4.action = 1;
    flow_p->fe.ipv4.hits = 0;
    flow_p->fe.ipv4.timestamp = 0;

    flow_p->location = 1;

    flow_p->fe.ipv4.tag_oc = 0;
    flow_p->fe.ipv4.tag_tc = 0;
    flow_p->fe.ipv4.tag_dest_map = 0;

    flow_p->l3_proto = 2048;
    flow_p->l4_proto = 17; /* change */


    flow_p->ct = NULL;
    flow_p->nfct = NULL;

    ret = ctf_activate_flow(flow_p);

    if (ret != CTF_SUCCESS)
        printk("%s: ERROR: ctf_activate_flow failed", __func__);

    return ret;
}

/*
 * Function   : ctf_callback
 * Description: Callback entry point into CTF subsystems for runtime settings.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
int ctf_callback(ctf_ioctl_t callback, int arg)
{
	int ret = CTF_ERROR;

	//printk(KERN_DEBUG "%s %d", ctf_cb_name[callback - 1], arg);

	switch (callback) {
	case CTF_STATUS:
        /*
         * If arg == 0, show only status regs
         * If arg == 1, show status regs + flow cache
         * If arg == 2, show status regs + flow table
         * If arg == 3, show status regs + flow cache + flow table
         */
		ret = ctf_status_show();

        /* Display all entries in flow cache */
        if (arg == 1)
            fc_dump_flow_cache(-1, 0);

        /* Display only flow cache entries which are also present in 
         * hw flow table
         */
        if (arg == 3)
            fc_dump_flow_cache(-1, 1);

        if ((arg == 2) || (arg == 3))
            ctf_dump_flow_table_entries(-1);
		break;

	case CTF_RESET:
		ret = ctf_reset_tables(arg);
		break;

	case CTF_INIT:
		ret = ctf_init();
		break;

	case CTF_ENABLE:
		ret = ctf_enable();
		break;
	case CTF_DISABLE:
		ret = ctf_disable();
		break;

	case CTF_REGDUMP:
		ret = ctf_regdump();
		break;

	case CTF_DUMPPT:
		ret = ctf_dump_napt_pool_table();
		break;

	case CTF_DUMPNHT:
		ret = ctf_dump_next_hop_table();
		break;

    case CTF_CFG_LOG:
         if (arg <= CTF_DBG_MAX_LVL) {

             ctf_dbg_log_level = arg;

		     printk(KERN_DEBUG "\ndebug level changed to %d",
                    ctf_dbg_log_level);

             if (ctf_dbg_log_level >= CTF_DBG_DEFAULT_LVL) {
		         printk(KERN_DEBUG "\n output for dbg lvl default, or higher");
             }

             if (ctf_dbg_log_level > CTF_DBG_LOW_LVL) {
		         printk(KERN_DEBUG "\n output for dbg lvl medium");
             }

             if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
		         printk(KERN_DEBUG "\n output for dbg lvl high");
             }

         }
         else {

		     printk(KERN_DEBUG "\nNew debug level %d exceeds max value %d",
                    arg, CTF_DBG_MAX_LVL);

         }

      
         ret = CTF_SUCCESS;
         break;

    case CTF_ADDFE:
         {
             unsigned long *args = (unsigned long *)arg;

             ret = ctf_debug_add_ipv4_flow_entry(args);
             //printk("\n\n%s: sip=0x%08X, dip=0x%08X, sport=0x%08X, dport=0x%08X, proto=0x%08X\n\n", __func__, args[0], args[1], args[2], args[3], args[4]);

         }
         break;

    case CTF_ADDPT:
         {
             //int ret;
             uint8_t pt_ix = 0;
             unsigned char args[12];
             unsigned char flag;

             memcpy(args, (unsigned char *)arg, 12);

             flag = *((unsigned char *)arg + 13) & 0xFF;
             ctf_add_napt_pool_entry(args, &pt_ix, flag & 0x1);

             printk("\n%s: pool table add: pt_ix=%d\n", __func__, pt_ix);
             //printk(KERN_DEBUG "\n\n%s: a0=0x%X, a1=0x%X, a2=0x%X, a3=0x%X, a4=0x%X, a5=0x%X, a6=%u\n\n", __func__, parg[0], parg[1], parg[2], parg[3], parg[4], parg[5], parg[6]);
             //printk(KERN_DEBUG "\n\n%s: a7=0x%X, a8=0x%X, a9=0x%X, a10=0x%X, a11=0x%X\n\n", __func__, parg[7], parg[8], parg[9], parg[10], parg[11]);
             //printk("\n\n%s: a0=0x%X, a1=0x%X, a2=0x%X, a3=0x%X, a4=0x%X, a5=0x%X, flag=%u\n\n", __func__, args[0], args[1], args[2], args[3], args[4], args[5], flag);
             ret = CTF_SUCCESS;
         }
         break;

    case CTF_ADDNHT:
         {
             uint8_t frm_type, op, da_ix = 0;
             uint8_t da[6];
             uint32_t vlan_tag;
             //int ret;

             memcpy(da, (unsigned char *)arg, 6);
             vlan_tag = *((uint32_t *)((unsigned char *)arg + 13));
             frm_type = *((uint8_t *)((uint8_t *)arg + 26));
             op = *((uint8_t *)((uint8_t *)arg + 39));

		     //printk("\n%s: MAC=0x%02X%02X%02X%02X%02X%02X",
             //       __func__, da[0], da[1], da[2], da[3], da[4], da[5]);
             //printk("\n    vlan_tag=%u, frm_type=%u, op = %u\n",
             //       vlan_tag, frm_type, op);

             /* Note: If mac address matches with a previously added entry, a
              * new next hop entry is _not_ created
              */
             ctf_next_hop_table_add_entry(vlan_tag, da, &da_ix, frm_type, op);

             printk("\n%s: next hop table add: da_ix=%d\n", __func__, da_ix);
             ret = CTF_SUCCESS;
         }
         break;

	default:
		printk(KERN_DEBUG "ERROR: callback(%d)", callback);
		ret = CTF_ERROR;
	}

	return ret;
}

/*
 * Function   : ctf_reset_init
 * Description: Wrapper callback invoked by MAC driver to reset and intialize
 *              Switch CTF with default rules.
 *
 * Returns    :
 *    success : CTF_SUCCESS, 0
 *    failure : CTF_ERROR, -1
 */
extern int ctf_reset_init(void)
{
	printk(KERN_DEBUG "Resetting CTF Hardware Please wait...\n");

	if (ctf_callback(CTF_RESET, 0) == CTF_ERROR) {
		printk(KERN_DEBUG "%s ERROR: CTF_RESET\n", __FUNCTION__ );
		return CTF_ERROR;
	}
	if ( ctf_callback(CTF_INIT, 0) == CTF_ERROR) {
		printk(KERN_DEBUG "%s ERROR: CTF_INIT\n",__FUNCTION__ );
		return CTF_ERROR;
	}
	if ( ctf_callback(CTF_ENABLE, 0) == CTF_ERROR) {
		printk(KERN_DEBUG"%s ERROR: CTF_ENABLE", __FUNCTION__);
		return CTF_ERROR;
	}

	return CTF_SUCCESS;
}

/*
 * Function   : ctf_load_init
 * Description: Module loading constructor.
 *
 * On module loading the character driver is constructed.
 * The HW is explicitly initalized by the Switch drivers.
 */
int __init ctf_load_init(void)
{
	printk(KERN_DEBUG "Loading CTF Driver...\n");

	ctf_drv_init();

	return 0;
}

/*
 * Function   : ctf_unload_exit
 * Description: Module unloading destructor.
 *      On module unloding the character driver is destroyed
 *      after the CTF hardware is disabled.
 */
void __exit ctf_unload_exit(void)
{
	printk(KERN_DEBUG "Unloading CTF Driver...\n");

	/*
	 * Disable the CTF hardware
	 */
	ctf_disable();

	/*
	 * Delete the char driver
	 */
	ctf_drv_exit();

    ctf_initialized	= CTF_FALSE;
}

EXPORT_SYMBOL(ctf_callback);      /* Single exported function    */

EXPORT_SYMBOL(ctf_wan_port_enable); /* Callback used by GMAC driver */
EXPORT_SYMBOL(ctf_wan_port_disable);/* Callback used by GMAC driver */
EXPORT_SYMBOL(ctf_reset_init);      /* Callback used by GMAC driver */

module_init(ctf_load_init);
module_exit(ctf_unload_exit);

MODULE_DESCRIPTION(CTF_MODNAME);
MODULE_VERSION(CTF_VERSION);
MODULE_LICENSE("Proprietary");

