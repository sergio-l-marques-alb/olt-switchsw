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
 * This files contains functions implementing various features in FA+ device.
 * Also, it implements functions for interfacing with other modules
 *
 */

#include <linux/module.h>
#include <linux/if_ether.h>
#include <linux/if_pppox.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/skbuff.h>
#include <net/ip.h>
#include <net/ipv6.h>
#ifndef CONFIG_IPROC_FA2_NAPT_BYPASS
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_helper.h>
#endif /* CONFIG_IPROC_FA2_NAPT_BYPASS */
#include "typedefs.h"
#include "bcmutils.h"
#include "mach/iproc_regs.h"
#include "fa2_regs.h"
#include "fa2_defs.h"
#include "fa2_if.h"

#define FA2_MAKE_BCMHDR_3_W1_PARTIAL(v, pop, fop, l4o) v = (pop); \
                                v |= ((fop) << FA2_BCMHDR_OP_3_FWD_OP_SHIFT); \
                                v |= ((l4o) << FA2_BCMHDR_OP_3_L4_OFFSET_SHIFT);

extern fa2_flentry_t *fa2_flcache;
extern spinlock_t fa2_lock;
extern uint8_t fa2_enable_aging;
static spinlock_t fa2_spu_lock = __SPIN_LOCK_UNLOCKED(fa2_spu_lock);

/* Any connection over this port is considered 'wan' side by FA+ driver */
static int fa2_wan_port;
static int fa2_pppoe; /* Used in flow entries to enable/disable pppoe */
static struct timer_list fa2_age_timer;

#define MAX_SPU_HOST_PKTS (64)  /* must be power of two */
static u32 spu_trns_head = 0, spu_trns_tail = 0;  /* protected by fa2_spu_lock */
static void * spu_trns_cb_data_ptr[MAX_SPU_HOST_PKTS];
static unsigned spu_trns_cb_data_idx[MAX_SPU_HOST_PKTS];
static void (*spu_trns_cb[MAX_SPU_HOST_PKTS])(u8 *data, size_t len, void *cb_data_ptr, unsigned cb_data_idx);

extern int et_fa2_spu_tx(struct sk_buff *skb);
static int fa2_process_spu_result(struct sk_buff *skb);

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
/* For better performance, use this function only if both words of the header
 * have non-zero values
 */
#endif

/* Debug function to display cache entries
 * If index range is between 0 - configured table size (reg 0x4) 
 * (up to FA2_FLOWTBL_SIZE), the corresponding entry will be displayed.
 * If index is -1, all cached entries are displayed.
 * If index is -1 and in_hw is 1, only the entries programmed in h/w flow table 
 * are displayed.
 * If index is not -1, entry will be displayed irrespective of whether its in
 * h/w 
 */
void fa2_dump_flow_cache(int index, int in_hw)
{
    int i, n, num_entries, max_size, cnt;
    fa2_flentry_t *f;

    max_size = FA2_FLOWTBL_SIZE; /* chk get max val from reg 0x4 */

    if (index >= max_size) {
        printk(KERN_DEBUG "%s: index %d exceeds max value %d, returning\n",
               __func__, index, FA2_FLOWTBL_SIZE - 1);
        return;
    }

    printk(KERN_INFO "\nDriver flow cache:\n");

    num_entries = (index == -1) ? max_size : 1;
    i = (index == -1) ? 0 : index;
    cnt = 0;

    for (n = 0; n < num_entries; n++, i++) {

        f = fa2_flcache + i;

        if (f->location == FA2_FL_INVALID) {
            /* Flow entry not present, or not initialized */
            continue;
        }

        if (in_hw) { /* h/w entries only */
            if (f->location != FA2_FL_IN_HW) {
                continue;
            }
        }

        printk(KERN_INFO "__________ %s entry %d __________\n\n",
               (f->word1 & FA2_DRV_IPV4_ENTRY) ? "v4" : "v6", n);

        cnt++;

        if (f->word1 & FA2_DRV_IPV4_ENTRY) {
            /* ipv4 */
            printk(KERN_INFO "PROTO=%u, SIP = 0x%08X, SPORT= %u, DIP = 0x%08X,"
                   " DPORT = %u\n\n", f->protocol, f->ip.v4.sip,
                   f->sport, f->ip.v4.dip, f->dport);

            printk(KERN_INFO "flid = %u, location = %u, state = %u,"
                   " entry_state = %u\n\n",
                   f->flid, f->location, f->state, f->entry_state);

            printk(KERN_INFO "word0 = 0x%08X,  word1 = 0x%08X\n\n",
                   f->word0, f->word1);

            printk(KERN_INFO "ct=0x%08X, nfct=0x%08X\n\n",
                   (unsigned int)(f->ct), (unsigned int)(f->nfct));

            printk(KERN_INFO "______________________________\n\n");

        }
    }
}

static int fa2_get_frame_info(unsigned char *p, unsigned int *et, 
                              unsigned int *vid1, unsigned int *vid2,
                              unsigned int *frm_data, uint8_t *eth_snapllc,
                              uint8_t offset)
{
    uint32_t val1, val2, val3, val4;

    val1 = ntohl(*((uint32_t *)(&p[16 + offset])));
    val2 = ntohl(*((uint32_t *)(&p[20 + offset])));
    val3 = ntohl(*((uint32_t *)(&p[24 + offset])));
    val4 = ntohl(*((uint32_t *)(&p[28 + offset])));

    if (FA2_IS_VLAN_TAGID(val1) && FA2_IS_VLAN_TAGID(val2) &&
        ((val3 & FA2_SNAP_ID1_MASK) == FA2_SNAP_ID1) && 
        (val4 == FA2_SNAP_ID2)) {

        val4 = ntohl(*((uint16_t *)(&p[32 + offset]))); /* reusing val4 */

        *et = (val4 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
        *vid1 = val1 & FA2_VLAN_ID_MASK; /* OTPID */ 
        *vid2 = val2 & FA2_VLAN_ID_MASK; /* ITPID */
        *frm_data = 34 + offset;
        *eth_snapllc = 1;
        if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\nFrame with eth2+2vlantags+snap\n");
        }
        goto frminfo_exit;
    }

    if (FA2_IS_VLAN_TAGID(val1) && FA2_IS_VLAN_TAGID(val2)) {
        *et = (val3 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
        *vid1 = val1 & FA2_VLAN_ID_MASK; /* OVLANID */ 
        *vid2 = val2 & FA2_VLAN_ID_MASK; /* IVLANID */
        *frm_data = 26 + offset;
        *eth_snapllc = 0;

        if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\nFrame with eth2+2vlantags \n");
        }
        goto frminfo_exit;
    }

    if (FA2_IS_VLAN_TAGID(val1) &&
        ((val2 & FA2_SNAP_ID1_MASK) == FA2_SNAP_ID1) &&
        (val3 == FA2_SNAP_ID2)) {

        *et = (val4 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
        *vid1 = val1 & FA2_VLAN_ID_MASK; /* VLANID */ 
        *vid2 = 0;
        *frm_data = 30 + offset;
        *eth_snapllc = 1;

        if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\nFrame with eth2+1vlantag+snap\n");
        }
        goto frminfo_exit;
    }

    if (FA2_IS_VLAN_TAGID(val1)) {
        *et = (val2 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
        *vid1 = val1 & FA2_VLAN_ID_MASK; /* VLANID */ 
        *vid2 = 0;
        *frm_data = 22 + offset;
        *eth_snapllc = 0;

        if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\nFrame with eth2+1vlantag\n");
        }
        goto frminfo_exit;
    }

    if (((val1 & FA2_SNAP_ID1_MASK) == FA2_SNAP_ID1) && 
        (val2 == FA2_SNAP_ID2)) {

        *et = (val3 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
        *vid1 = 0;
        *vid2 = 0;
        *frm_data = 26 + offset;
        *eth_snapllc = 1;

        if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\nUntagged frame with eth2+snap\n");
        }
        goto frminfo_exit;
    }

    *et = (val1 & FA2_TYPE_MASK) >> FA2_TYPE_SHIFT;
    *vid1 = 0;
    *vid2 = 0;
    *frm_data = 18 + offset;
    *eth_snapllc = 0;

    if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
        printk(KERN_DEBUG "\nUntagged eth2 frame\n");
    }

frminfo_exit:
    if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
        printk(KERN_DEBUG "\net=0x%04X(%u), outervlanid=0x%04X(%u), "
               "innervlanid=0x%04X(%u), frm_data_locn=%u\n\n",
               *et, *et, *vid1, *vid1, *vid2, *vid2, *frm_data);
    }

    return(0);
}

/*
 * Function     : fa2_dump_skb
 * Description  : Dump skb data (for debugging only)
 */
void fa2_dump_skb(struct sk_buff *skb, uint32_t offset)
{
	unsigned char *pdata;
	int ix = 0;
    uint8_t buf[80], *p = buf;

	if (offset >= 0 && offset < skb->len) {
		pdata = (unsigned char *)&skb->data[offset];
        // testpdata = (unsigned char *)&skb->head[offset];
	} else {
		printk(KERN_INFO "Invalid Offset\n");
		return;
	}

	printk(KERN_INFO "\nAll data bytes in hex\n\n");
	for (ix = 0; ix < (skb->len - offset); ix++) {

		//printk(KERN_INFO "%02X ", pdata[ix]);
        snprintf(p, 80, "%02X ", pdata[ix]);
        p += 3;

		if ((ix % 16) == 15) {
			printk(KERN_INFO "%s\n", buf);
            buf[0] = '\0';
            p = buf;
		}
	}

    if (buf[0] != '\0') {
        /* Print remaining bytes */
        printk(KERN_INFO "%s\n", buf);
    }

	printk(KERN_INFO "\n");
}

static void fa2_age_timer_func(unsigned long data)
{
	volatile void __iomem		*reg_addr;
	volatile void __iomem		*reg_addr1;
	volatile uint32_t	        reg_val;
    uint8_t num_entries; 
    uint16_t num_entries_processed;
    uint16_t fl_idx;
    fa2_flentry_t *this_flow, *pair_flow;

    /* If aging is disabled as per comment in 'while' below, we simply return */
    if (!fa2_enable_aging) {
        return;
    }

    num_entries_processed = 0;

    reg_addr1 = fa2_virt_addr + CTF_STATUS_REG_OFF;
    reg_val = ioread32(reg_addr1);

    /* Check if h/w has closed any entries. If so, process them, else simply
     * return after re-starting the timer
     */
    printk(KERN_DEBUG "\n%s: reg 0x68 is 0x%08X\n", __func__, reg_val);
    if (reg_val & FA2_CTF_STATUS_FLOW_TIMEOUT_FIFO_NONEMPTY) {

        /* Clear only the non-empty bit; other status should remain intact */
        iowrite32(FA2_CTF_STATUS_FLOW_TIMEOUT_FIFO_NONEMPTY, reg_addr1);

        /* Read the #entries closed and list of flow addresses closed during 
         * previous polling cycle
         */
        reg_addr = fa2_virt_addr + CTF_FLOW_TIMEOUT_CONTROL_REG_OFF;
        reg_val = ioread32(reg_addr);
        num_entries = reg_val & FA2_CTF_FLOW_TIMEOUT_CONTROL_FIFO_DEPTH;

        while (num_entries) {

            printk(KERN_DEBUG "\nnum_entries=%u\n", num_entries);

            fl_idx = FA2_GETBITS(reg_val, 
                                FA2_CTF_FLOW_TIMEOUT_CONTROL_FLOW_ENTRY_POINTER,
                                CTF_FLOW_TIMEOUT_CONTROL__FLOW_ENTRY_POINTER_R);

            printk(KERN_DEBUG "\nfl_idx from fifo=%u\n", fl_idx);

            this_flow = &fa2_flcache[fl_idx];

            /* Mark this entry as aged; when both entries of a pair age, we
             * will delete them
             */
            this_flow->state = FA2_AGED_FLOW;

            if (this_flow->word1 & FA2_DRV_REV_FL_VALID) {

                pair_flow = &fa2_flcache[this_flow->word0 &
                                         FA2_DRV_REV_FLOW_PTR];

                /* Both flows have aged, delete both flows. If a flow is not
                 * in the present list it will be looked at in the subsequent
                 * poll
                 */
                if (pair_flow->state == FA2_AGED_FLOW) {
                    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];
                    int ret;

                    memset(val, 0x0, sizeof(val));

                    /* Delete both flows and clear their cache entries */
                    if (this_flow->word1 & FA2_DRV_IPV4_ENTRY) {
                        /* Clear ipv4 h/w entries */
                        ret = fa2_indirect_write(FA2_FLTBL_V4, fl_idx, val);
                        ret |= fa2_indirect_write(FA2_FLTBL_V4,
                                                  this_flow->word0 &
                                                  FA2_DRV_REV_FLOW_PTR, val);
                    } else {
                        /* Clear ipv6 h/w entries */
                  
                        /* This flow W1, W0 */ 
                        /* Chk if the order is correct */
                        ret = fa2_indirect_write(FA2_FLTBL_V6_W1, fl_idx, val);

                        ret |= fa2_indirect_write(FA2_FLTBL_V6_W0, fl_idx + 1,
                                                  val);

                        /* Pair flow W1, W0 */
                        ret |= fa2_indirect_write(FA2_FLTBL_V6_W1, 
                                                 this_flow->word0 & 
                                                 FA2_DRV_REV_FLOW_PTR, val);

                        ret |= fa2_indirect_write(FA2_FLTBL_V6_W0,
                                                  (this_flow->word0 &
                                                  FA2_DRV_REV_FLOW_PTR) + 1,
                                                  val);
                    }

                    if (ret == FA2_SUCCESS) {
                        printk(KERN_DEBUG "\nDeleted flows %u and %u\n", 
                               fl_idx, this_flow->word0 & FA2_DRV_REV_FLOW_PTR);

                        memset((void *)this_flow, 0x0,
                               sizeof(fa2_flentry_t));
                        memset((void *)pair_flow, 0x0,
                               sizeof(fa2_flentry_t));

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
                       /* Set source port to invalid value */
                       FA2_SETBITS(this_flow->word1, FA2_DRV_INVALID_SRC_PID,
                                   FA2_DRV_RCVD_SRC_PID,
                                   FA2_DRV_RCVD_SRC_PID_SHIFT);

                       FA2_SETBITS(pair_flow->word1,
                                   FA2_DRV_INVALID_SRC_PID,
                                   FA2_DRV_RCVD_SRC_PID,
                                   FA2_DRV_RCVD_SRC_PID_SHIFT);
#endif
                    } else {
                        /* If one/more h/w deletes failed we keep cache 
                         * entries intact; may be useful to retry deletion
                         */
                        printk(KERN_DEBUG "\n%s: One or both flow entries"
                               "could not be deleted, this entry: %d, "
                               "pair_entry: %d\n", __func__, fl_idx, 
                               this_flow->word0 & FA2_DRV_REV_FLOW_PTR);
                    }
                }
            } else {
                pair_flow = NULL;
                /* This condition should not occur since we always program
                 * entries in pairs. If it does happen we need to
                 * delete 'this_flow' in this polling cycle
                 */
            }

            /* Read next flow index from FIFO */
            reg_val = ioread32(reg_addr);

            num_entries--;
            num_entries_processed++;
            
            /* Should not happen, but we keep track of #entries to avoid this
             * loop turning in to an infinite loop
             */
            if (num_entries_processed >= FA2_FLOWTBL_SIZE) {

                printk(KERN_DEBUG "\nWarning: %u entries aged! Possible problem"
                       " in h/w aging logic, or timeout values (reg 0x60)"
                       " too low\n", num_entries_processed);

                /* We can disable aging here so that further damage is not 
                 * done (set fa2_enable_aging to 0)
                 */

                fa2_enable_aging = 0;

                return;
            }

            /* Read fifo depth again, since we must keep reading all entries
             * until the fifo is empty. Otherwise, the non-empty bit in reg 0x68
             * will not be set again, as per h/w engg.
             */
            if (num_entries == 0) {
                num_entries = reg_val & FA2_CTF_FLOW_TIMEOUT_CONTROL_FIFO_DEPTH;
            }
        }
    }

    fa2_age_timer.expires = jiffies + FA2_FL_AGE_CHK_INT * HZ;
    add_timer(&fa2_age_timer);

    return;
}


/* For timer set up, clearing flow entries etc. check if it is reqd */
int fa2_init_flow_cache(void)
{
    int ret;
    int index;

    if (fa2_flcache == NULL) {
        printk(KERN_ERR "\nError: No space allocated for flow cache earlier,"
               " check boot up log\n");
        return FA2_ERROR;
    }

    /* Clear memory */
    memset(fa2_flcache, 0x0, sizeof(fa2_flentry_t) * FA2_FLOWTBL_SIZE);

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    for (index = 0; index < FA2_FLOWTBL_SIZE; index++) { 
        fa2_flentry_t *f;

        f = &fa2_flcache[index];

        /* Init src port id to invalid value */
        FA2_SETBITS(f->word1, FA2_DRV_INVALID_SRC_PID, FA2_DRV_RCVD_SRC_PID,
                    FA2_DRV_RCVD_SRC_PID_SHIFT);
    }
#endif

    fa2_wan_port = getwanport();

    fa2_pppoe = false;

#if 0 /* The check below is correct, but the above api returns 0 even for a
       * valid wan port id of 0, contacted the developer to make change. 
       * After the change is made, then re-enable this code.
       * Example, nsp-hr board
       */

    if (fa2_wan_port == 0) {
        printk(KERN_ERR "\nError: getwanport returned 0. Verify uboot params"
               " for wan port cfg\n\n");
        return FA2_ERROR;
    }
#endif /* 0 */

    printk(KERN_DEBUG "\nWan port is %u\n", fa2_wan_port);

    /* Look for FA2_DRV_MTU_IDX in fa2_transmit. For now we are using mtu table
     * location 0 for inbound connections, 1 for outbound connections
     */
    index = 0;
    ret = fa2_set_mtu_entry(1526, index);

    if (ret == FA2_SUCCESS) {
        index = 1;
        ret = fa2_set_mtu_entry(1526, index);
    }

    if (ret != FA2_SUCCESS) {
        printk(KERN_ERR "\n%s: fa2_set_mtu_entry failed for index %d\n",
               __func__, index);
        return FA2_ERROR;
    }

    /* Initialize port type table */ /* TODO Change 5 with macro from gmac
                                        driver */
    for (index = 0; index < 5; index++) { 
        uint8_t v;

        v = (index != fa2_wan_port) ? 0 : 1; /* LAN : WAN */

        ret = fa2_set_port_type_entry(v, index, 1);

        if (ret != FA2_SUCCESS) {

            printk(KERN_ERR "\n%s: fa2_set_port_type_entry failed for "
                   "index %d\n", __func__, index);

            return FA2_ERROR;
        }
    }

    if (fa2_enable_aging) {
        /* Initialize aging timer */
    
        init_timer(&fa2_age_timer);

        /* 10 sec default */
        fa2_age_timer.expires = jiffies + FA2_FL_AGE_CHK_INT * HZ;
        fa2_age_timer.function = fa2_age_timer_func;
        fa2_age_timer.data = 0;
        add_timer(&fa2_age_timer);
    }

    return FA2_SUCCESS;
}

static fa2_flentry_t *fa2_init_cache_entry(uint16_t idx)
{
    fa2_flentry_t *f;

    //spin_lock_bh(&fa2_lock);

    f = &fa2_flcache[idx];

    /* Entry previously seen and initialized, return the same */
    if (f->entry_state == true) {
        //spin_unlock_bh(&fa2_lock);
	    return f;
    }

    memset((void *)f, 0x0, sizeof(fa2_flentry_t));
    f->flid = idx;
    f->location = FA2_FL_IN_CACHE;
    f->entry_state = true;
#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    FA2_SETBITS(f->word1, FA2_DRV_INVALID_SRC_PID, FA2_DRV_RCVD_SRC_PID,
                FA2_DRV_RCVD_SRC_PID_SHIFT);
#endif
    //spin_unlock_bh(&fa2_lock);

    return f;
}

/*
 * This function is used to check and, if required, create a new driver cache
 * entry for a incoming flow.
 * Note packet contents are not modified
 *
 */
int fa2_receive(struct sk_buff *skb)
{
#ifndef CONFIG_IPROC_FA2_NAPT_BYPASS
    int ret;
	uint32_t		vlan_tag, vlan_tag_next, data_loc;
	uint32_t		et_type;
    uint32_t        bcmhdr0, bcmhdr1;
    unsigned       proc_op;
    uint8_t        op, eth_snapllc;
    uint8_t        hdr_chk_res;
    uint8_t        src_pid;
	struct iphdr		*iph = NULL;
	struct ipv6hdr		*ipv6h = NULL;
    uint16_t        sport, dport;
    uint16_t        flid;
    fa2_flentry_t *f;
    uint8_t offset;

    BUG_ON(skb == (struct sk_buff*)NULL);

    if (fa2_dbg_log_lvl == FA2_DBG_PKT_DUMP) {
        printk(KERN_INFO "Received Packet... <----------\n");

        printk(KERN_INFO "\n%s: Recv skb len: %d\n", __func__, skb->len);
        fa2_dump_skb(skb, 0);
        printk(KERN_INFO "----------------------------------------\n");
    }

    /* Read Broadcom header word(s) */
    bcmhdr0 = ntohl(*((uint32_t *)skb->data));
    bcmhdr1 = 0;

    op = FA2_GETBITS(bcmhdr0, FA2_BCMHDR_OPCODE, FA2_BCMHDR_OPCODE_SHIFT);

    hdr_chk_res = FA2_GETBITS(bcmhdr0, FA2_BCMHDR_OP_2_HDR_CHK_RESULT, 
                              FA2_BCMHDR_OP_2_HDR_CHK_RESULT_SHIFT);

    /* If the result is non-zero, then some of the values simply provide status
     * of pipeline operation; others denote error. For now we simply log the
     * status. For certain errors, may be the right action may be to dscard the
     * packet
     */
    if (hdr_chk_res) {

        if (fa2_dbg_log_lvl > FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\n%s: hdr_chk_res for this pkt 0x%X\n", 
                   __func__, hdr_chk_res);
        }
    }

    /* opcodes 1 and 3 have one more word */
    if (op == FA2_BCMHDR_OPCODE_1) {
        bcmhdr1 = ntohl(*((uint32_t *)(skb->data + 4)));

        if (fa2_dbg_log_lvl > FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\n%s: opcode %u: words 0, 1:  0x%08X, 0x%08X\n",
                   __func__, (op == FA2_BCMHDR_OPCODE_1) ? 
                   FA2_BCMHDR_OPCODE_1 : FA2_BCMHDR_OPCODE_3, 
                   bcmhdr0, bcmhdr1);
        }

        /* TODO Find out if any processing is reqd for this opcode type */
        /* If further processing is reqd, the skb ptrs used in the code
         * below will change
         */

        return FA2_PKT_NORM;
    }

    /* opcode 3 indicates a packet sent from this host, processed via the FA+/SPU
       pipeline, then returned.
    */
    if (op == FA2_BCMHDR_OPCODE_3) {
        bcmhdr1 = ntohl(*((uint32_t *)(skb->data + 4)));
        proc_op = FA2_GETBITS(bcmhdr1, FA2_BCMHDR_OP_3_PROC_OP,
                                       FA2_BCMHDR_OP_3_PROC_OP_SHIFT);

        switch (proc_op) {
        case 8:
            {
                bool is_crypto = *(skb->data + 12) & 0x80;

                if (fa2_dbg_log_lvl > FA2_DBG_LOW_LVL) {
                    printk(KERN_DEBUG "\n%s: opcode %u: words 0, 1:  0x%08X, 0x%08X\n",
                           __func__, (op == FA2_BCMHDR_OPCODE_1) ? 
                           FA2_BCMHDR_OPCODE_1 : FA2_BCMHDR_OPCODE_3, 
                           bcmhdr0, bcmhdr1);
                }

                if (is_crypto) {
                    if (fa2_process_spu_result(skb) == 0) {
                        /* it was processed here, so tell the caller that we have dealt with it */
                        return FA2_PKT_DONE;
                    } else {
                        return FA2_PKT_NORM;
                    }
                }
                /* This is a SPU packet, but was not generated via this driver, so return it to be
                   processed as a regular packet
                */
                return FA2_PKT_NORM;
            }
            break;

            /* Std pipeline processing, continue in to the function to process
             * 'miss' and/or other events
             */
            case 0:
            case 1:
            case 2:
            case 5:
                offset = 4;
                break;

            default:
                printk(KERN_DEBUG "\n%s: Illegal value of proc_op: %u, return\n", __func__, proc_op);
                return FA2_PKT_NORM;

        }
    } else {
        /* opcode 2 */
        offset = 0;
    }

    /* All buckets are occupied, no space to add new flows, let the stack do
     * the processing
     */
    if (bcmhdr0 & FA2_BCMHDR_OP_2_ALL_BKTS_FULL) {
        printk(KERN_DEBUG "\n%s: H/w indicates all buckets full 0x%08X\n",
               __func__, bcmhdr0);
        return FA2_PKT_NORM;
    }

    /* If we are dealing with broadcom header 0x3, we have to adjust the 
     * offsets in to the skb by an addtional 4 (hdr 0x3 is 8-byte wide)
     */
    /* Get protcol info */
    ret = fa2_get_frame_info(skb->data, &et_type, &vlan_tag, &vlan_tag_next,
                             &data_loc, &eth_snapllc, offset);

    if (fa2_pppoe) {

        if (et_type == 0x8864) { /* Check if pppoe session frame */
            uint8_t proto_val;

            data_loc += 8; /* Skip 8 bytes (PPPoE header +
                              PPP protocol bytes) */

            /* Check L3 protocol */
			proto_val = (skb->data[data_loc] & 0xF0) >> 4;
			et_type = (proto_val == 0x4) ? ETH_P_IP : 
                      (proto_val == 0x6) ? ETH_P_IPV6 :
                      et_type;

        }
    }

    if (fa2_dbg_log_lvl >= FA2_DBG_LOW_LVL) {
        printk(KERN_INFO "\nbcm hdr words 0, 1:  0x%08X, 0x%08X\n",
               bcmhdr0, bcmhdr1);
        printk(KERN_DEBUG "\nRcvd ET 0x%04X, eth_snapllc = %d, vlan1=%u, "
               "vlan2=%u, data_loc=%u\n",
               et_type, eth_snapllc, vlan_tag, vlan_tag_next, data_loc);
    }

    if (et_type == ETH_P_IP) {

        iph = (struct iphdr *)&(skb->data[data_loc]);

		if ((iph->protocol != IPPROTO_TCP) &&
		    (iph->protocol != IPPROTO_UDP)) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
			        printk(KERN_DEBUG "%s:IPv4: Unsupported L4 proto rcvd :"
                           " %u(0x%X)\n", __func__, iph->protocol, 
                           iph->protocol);
            }

			return FA2_PKT_NORM;
		}

        if (iph->protocol == IPPROTO_TCP) {
            /* TCP */
    	    struct tcphdr *th;

            th = (struct tcphdr *)((unsigned int)iph + (iph->ihl * 4));

            sport = ntohs(th->source);
            dport = ntohs(th->dest);
        } else {
            /* UDP */
            struct udphdr *uh;

            uh = (struct udphdr *)((unsigned int)iph + (iph->ihl * 4));
            sport = ntohs(uh->source);
            dport = ntohs(uh->dest);
        }
    } else if (et_type == ETH_P_IPV6) {
        /* Extract L4 info from IPv6 header */

		ipv6h = (struct ipv6hdr *)&(skb->data[data_loc]);

        /* chk if we support other protocols, this might change */
		if ((ipv6h->nexthdr != NEXTHDR_TCP) &&
		    (ipv6h->nexthdr != NEXTHDR_UDP)) {

			  printk(KERN_DEBUG "%s: IPv6: unsupported v6 next hdr rcvd"
                     " %u (0x%X)\n", __func__, ipv6h->nexthdr,
                     ipv6h->nexthdr);

			return FA2_PKT_NORM;

		}

        if (ipv6h->nexthdr == NEXTHDR_TCP) {
            /* TCP */
    	    struct tcphdr *th;

            /* IPv6 hdr size is fixed */
            th = (struct tcphdr *)&(skb->data[data_loc + 40]);
            sport = ntohs(th->source);
            dport = ntohs(th->dest);
        } else {
            /* UDP */
            struct udphdr *uh;

            uh = (struct udphdr *)&(skb->data[data_loc + 40]);
            sport = ntohs(uh->source);
            dport = ntohs(uh->dest);
        }
    } else {

        if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
            printk(KERN_DEBUG "et type rcvd %u(0x%X)\n", et_type, et_type);
        }

        return FA2_PKT_NORM;
    }

    if ((hdr_chk_res == 0x1C) || (hdr_chk_res == 0x1D)) {
        flid = FA2_GETBITS(bcmhdr0, FA2_BCMHDR_OP_2_NAPT_FLOW_ADDR, 
                           FA2_BCMHDR_OP_2_NAPT_FLOW_ADDR_SHIFT);
    } else {
        /* Reason code does not indicate a miss, so we do not process this
         * packet
         */
       return FA2_PKT_NORM;
    }

    BUG_ON(flid >= FA2_FLOWTBL_SIZE);

    /* If fa2_transmit is modifying flow cache, wait for the modification to
     * complete
     */
    spin_lock(&fa2_lock);

    /* This entry was initialized earlier, do not overwrite it. Chk 
     * In case of FA, cache entrie's fields are overwritten by the latest
     * values. Chk if FA+ is a better approach. If this line is changed, check
     * if its ok to overwrite all fields of the current cache entry 
     */
    if (fa2_flcache[flid].entry_state == true) {
        /* Populate the flow address for tx function to use */
	    skb->napt_hash_ix	= flid;
	    skb->napt_bkt_ix	= 0;
        spin_unlock(&fa2_lock);
		return FA2_PKT_NORM;
    }

    /* In case of a blast of traffic (e.g., udp pkts), the 1st packet will 
     * result in populating of flow entry. So we should avoid other
     * packets of the same flow modify flow cache entry and/or hw table entry
     */
    if (fa2_flcache[flid].location == FA2_FL_IN_HW) {
		// printk(KERN_DEBUG "Flow <hash:%u, bkt:%u> already in hw flow table...\n", hash_index, bkt_index);
        spin_unlock(&fa2_lock);
		return FA2_PKT_NORM;
    }

    /* chk if a flow is still in the cache before a connection is assured, 
     * should we return
     */
    f = fa2_init_cache_entry(flid);

    /* Common field vals for both v4 and v6 */

    /* Check if the packet is coming from wan side */
    src_pid = bcmhdr0 & FA2_BCMHDR_OP_2_SRC_PID;

    if (src_pid == fa2_wan_port) {
        f->word0 |= FA2_DRV_DIRN; /* Inbound dirn */
    } else {
        f->word0 &= ~FA2_DRV_DIRN; /* Outbound dirn */
    }

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    /* Save src port id. Tx side may use it for offload functions */
    FA2_SETBITS(f->word1, src_pid, FA2_DRV_RCVD_SRC_PID,
                FA2_DRV_RCVD_SRC_PID_SHIFT);
#endif

    /* Reverse flow not known at this pt, so we mark it invalid. In the
     * transmit, we can figure out the pair index, populate rev flow 
     * (in w0) and set this bit
     */
    f->word1 &= ~FA2_DRV_REV_FL_VALID;

    /* If this entry is added to h/w, action == 0x3 will forward all
     * future packets to unimac 
     */
    FA2_SETBITS(f->word1, 0x3, FA2_DRV_DEST_DMA_CH, 
                FA2_DRV_DEST_DMA_CH_SHIFT);

    /* Note: action (replace ip or port or both) should be decided based
     * on configuration outside of FA+ driver. For now we set it to 
     * default value
     */
    FA2_SETBITS(f->word1, 0x1, FA2_DRV_ACTION, FA2_DRV_ACTION_SHIFT);

    f->sport = sport;
    f->dport = dport;

    /* Store the index of this entry. chk: done in fa2_init_cache_entry as well
     * find out which place is correct
     */
    //f->flid = flid; if entry_state is false, this fld is already init'ed. If
    // true we are exiting above (we do not reach here)

	switch (et_type) {
        case ETH_P_IP:
            f->ip.v4.sip = ntohl(iph->saddr);
            f->ip.v4.dip = ntohl(iph->daddr);
            f->protocol = iph->protocol; /* L4 proto */
            f->word1 |= FA2_DRV_IPV4_ENTRY; /* L3 proto is IPv4 */
            break;

        case ETH_P_IPV6:
            /* Byte order is big endian (nw order) here */
		    memcpy(f->ip.v6.sip, (void *)&(ipv6h->saddr),
			       sizeof(struct in6_addr));
		    memcpy(f->ip.v6.dip, (void *)&(ipv6h->daddr),
			       sizeof(struct in6_addr));
            /* chk if next protocol will always be l4 hdr in our system */
            f->protocol = ipv6h->nexthdr; /* L4 proto */
            f->word1 &= ~FA2_DRV_IPV4_ENTRY; /* L3 proto is IPv6 */
            break;

	    default:
		    printk(KERN_DEBUG "%s: Unknown protocol %u(0x%X)\n", 
                  __func__, et_type, et_type);
    }

    spin_unlock(&fa2_lock);

	skb->napt_hash_ix	= flid;
	skb->napt_bkt_ix	= 0;

#endif /* CONFIG_IPROC_FA2_NAPT_BYPASS */
    return FA2_PKT_NORM;
}

void fa2_display_ipv6_addr(uint8_t *addr, char *str, uint8_t log)
{

    if (log) {
        /* Use dmesg to view */
        printk(KERN_DEBUG "%s: %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X "
               "%02X%02X %02X%02X %02X%02X", str, addr[0], addr[1], addr[2], 
               addr[3], addr[4], addr[5], addr[6], addr[7], addr[8], addr[9], 
               addr[10], addr[11], addr[12],addr[13],addr[14],addr[15]);
    } else {
        /* Display on console (for debugging cmds) */
        printk(KERN_INFO "%s: %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X "
               "%02X%02X %02X%02X %02X%02X\n", str, addr[0], addr[1], addr[2], 
               addr[3], addr[4], addr[5], addr[6], addr[7], addr[8], addr[9], 
               addr[10], addr[11], addr[12],addr[13],addr[14],addr[15]);
    }

    return;
}

int fa2_dbg_get_fltable(void)
{
    int r = FA2_SUCCESS;
    int idx;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];
    fa2_flentry_t *f;

    printk(KERN_INFO "\nH/w flow table:\n");

    for (idx = 0; idx < FA2_FLOWTBL_SIZE; idx++) {

        f = &fa2_flcache[idx];

        if (f->word0 & FA2_DRV_VALID) {
            memset(val, 0x0, sizeof(val));

            r = fa2_indirect_read(FA2_FLTBL_V4, idx, val);

            if (r == FA2_SUCCESS) {

                printk(KERN_INFO "\n@%d: val[6-0 (bits 208-0)] = 0x {%08X %08X %08X"
                       " %08X %08X %08X %08X}\n", idx, val[6], val[5], val[4], 
                       val[3], val[2], val[1], val[0]);

            } else {
                printk(KERN_INFO "\n%s: Error: Call to fa2_indirect_read failed"
                       " for index %d. Error is %d\n", __func__, idx, r);

                break;
            }
        }
    }

    return(r);
}

static int fa2_write_to_hw(fa2_flentry_t *f)
{
    int ret = FA2_SUCCESS;

    BUG_ON(f == NULL);

    /* Check if reverse flow is valid */
    if (!(f->word1 & FA2_DRV_REV_FL_VALID)) {
	    printk(KERN_INFO "%s: Warning: reverse flow not valid for this flow"
               " (idx=%u)\n", __func__, f->flid);

        return FA2_ABORT;
    }

    if (f->word1 & FA2_DRV_IPV4_ENTRY) {
        struct fa2_flow_entry_ipv4 v4;

        memset((void *)(&v4), 0x0, sizeof(struct fa2_flow_entry_ipv4));

        v4.ipv4_key_type = 1;
        v4.valid = 1;
        v4.sip = f->ip.v4.sip;
        v4.dip = f->ip.v4.dip;
        v4.protocol = f->protocol;
        v4.sport = f->sport;
        v4.dport = f->dport;

        if (f->word1 & FA2_DRV_REV_FL_VALID) {
            v4.rev_flow_ptr = FA2_GETBITS(f->word0, FA2_DRV_REV_FLOW_PTR,
                                          FA2_DRV_REV_FLOW_PTR_SHIFT);
        } else {
            v4.rev_flow_ptr = 4095; /* dummy flow chk if this is OK */
        }

        v4.brcm_tag_opcode = 0; /* chk */
        v4.brcm_tag_tc = 0;
        v4.brcm_tag_te = 0;
        v4.brcm_tag_ts = 0;
        v4.brcm_tag_destmap = 0; /* chk */
        v4.direction = (f->word0 & FA2_DRV_DIRN) ? 1 : 0;
        v4.l4_chksum_chk = 0;
        v4.ppp_tunnel_en = (f->word0 & FA2_DRV_PPPOE_TUNL_EN) ? 1 : 0;
        if (v4.ppp_tunnel_en) {
            v4.ppp_tunnel_idx = FA2_GETBITS(f->word0, FA2_DRV_PPPOE_TUNL_IDX,
                                            FA2_DRV_PPPOE_TUNL_IDX_SHIFT);
        }
        v4.mtu_idx = FA2_GETBITS(f->word0, FA2_DRV_MTU_IDX,
                                 FA2_DRV_MTU_IDX_SHIFT);
        v4.next_hop_idx = FA2_GETBITS(f->word0, FA2_DRV_NXT_HOP_IDX,
                                 FA2_DRV_NXT_HOP_IDX_SHIFT);
        v4.remap_sa_idx = FA2_GETBITS(f->word0, FA2_DRV_REMAP_SA_IDX,
                                 FA2_DRV_REMAP_SA_IDX_SHIFT);
        v4.dest_dma_chan = 3; /* Send to switch. For aged flow, this might
                                 be changed to 'dropped' */

        v4.action = 1; /* should be determined by external config. For now, we
                        * keep value to 'translate ip only'
                        */
        v4.hits = 0;
        v4.tcp_fin = 0;
        v4.tcp_rst = 0;
        v4.tcp_ack_after_close = 0;
        v4.hit_after_close = 0;

        if (fa2_enable_aging) {
            if (f->protocol == IPPROTO_TCP) {
                v4.flow_state = 1; /* tcp established */
                v4.flow_timer = FA2_TCP_EST_FL_DEF_TOUT;
            } else {
                v4.flow_state = 0; /* udp established */
                v4.flow_timer = FA2_UDP_EST_FL_DEF_TOUT;
            }
        } else {
            v4.flow_state = 2; /* If aging is disabled, we define 'static' 
                                * flows. When aging is enabled, we need to
                                * change this state to 'tcp or udp established'
                                */
            v4.flow_timer = 0;
        }

        ret = fa2_set_tbl_entry(FA2_FLTBL_V4, f->flid, (void *)(&v4));
    } else {
        struct fa2_flow_entry_ipv6_w0 v6w0;
        struct fa2_flow_entry_ipv6_w1 v6w1;

        memset((void *)(&v6w0), 0x0, sizeof(struct fa2_flow_entry_ipv6_w0));
        memset((void *)(&v6w1), 0x0, sizeof(struct fa2_flow_entry_ipv6_w1));

        v6w0.ipv4_key_type = 0;
        v6w0.valid = 1;
        memcpy(v6w0.dip_lower, f->ip.v6.dip + 8, 8); /* Lower 8 bytes have lower
                                                        bytes of ipv6 addr */
        v6w0.protocol = f->protocol;
        v6w0.sport = f->sport;
        v6w0.dport = f->dport;

        if (f->word1 & FA2_DRV_REV_FL_VALID) {
            v6w0.rev_flow_ptr = FA2_GETBITS(f->word0, FA2_DRV_REV_FLOW_PTR,
                                          FA2_DRV_REV_FLOW_PTR_SHIFT);
        } else {
            v6w0.rev_flow_ptr = 2047; /* dummy flow chk if this is OK */
        }

        v6w0.brcm_tag_opcode = 0; /* chk */
        v6w0.brcm_tag_tc = 0;
        v6w0.brcm_tag_te = 0;
        v6w0.brcm_tag_ts = 0;
        v6w0.brcm_tag_destmap = 0; /* chk */
        v6w0.direction = (f->word0 & FA2_DRV_DIRN) ? 1 : 0;
        v6w0.l4_chksum_chk = 0;
        v6w0.ppp_tunnel_en = (f->word0 & FA2_DRV_PPPOE_TUNL_EN) ? 1 : 0;
        if (v6w0.ppp_tunnel_en) {
            v6w0.ppp_tunnel_idx = FA2_GETBITS(f->word0, FA2_DRV_PPPOE_TUNL_IDX,
                                              FA2_DRV_PPPOE_TUNL_IDX_SHIFT);
        }

        v6w0.mtu_idx = FA2_GETBITS(f->word0, FA2_DRV_MTU_IDX,
                                   FA2_DRV_MTU_IDX_SHIFT);
        v6w0.next_hop_idx = FA2_GETBITS(f->word0, FA2_DRV_NXT_HOP_IDX,
                                        FA2_DRV_NXT_HOP_IDX_SHIFT);
        v6w0.remap_sa_idx = FA2_GETBITS(f->word0, FA2_DRV_REMAP_SA_IDX,
                                        FA2_DRV_REMAP_SA_IDX_SHIFT);
        v6w0.dest_dma_chan = 3; /* Send to switch. For aged flow, this might
                                   be changed to 'dropped' */
        v6w0.action = 1; /* should be determined by external config. For now, we
                          * keep value to 'translate ip only'
                          */
        v6w0.hits = 0;
        v6w0.tcp_fin = 0;
        v6w0.tcp_rst = 0;
        v6w0.tcp_ack_after_close = 0;
        v6w0.hit_after_close = 0;

        if (fa2_enable_aging) {
            if (f->protocol == NEXTHDR_TCP) {
                v6w0.flow_state = 1; /* tcp established */
                v6w0.flow_timer = FA2_TCP_EST_FL_DEF_TOUT;
            } else {
                v6w0.flow_state = 0; /* udp established */
                v6w0.flow_timer = FA2_UDP_EST_FL_DEF_TOUT;
            }
        } else {
            v6w0.flow_state = 2; /* 'static' flows if aging is disabled */
            v6w0.flow_timer = 0;
        }

        v6w1.ipv4_key_type = 0;
        v6w1.valid = 1;
        memcpy(v6w1.sip, f->ip.v6.sip, 16);
        memcpy(v6w1.dip_upper, f->ip.v6.dip, 8); /* Upper bytes consist of
                                                  * upper bits of ipv6 
                                                  * addresses
                                                  */

        /* chk with h/w engg if order of writing w0, w1 matters */
        ret = fa2_set_tbl_entry(FA2_FLTBL_V6_W1, f->flid, (void *)(&v6w1));
        ret |= fa2_set_tbl_entry(FA2_FLTBL_V6_W0, f->flid, (void *)(&v6w0));
    }

    return(ret);
}

/*
 * Function     : fa2_match
 * Description  : Compares tuple with that of flow entry
 */
static inline uint32_t fa2_match(fa2_flentry_t *f, uint32_t sip,
				                 uint32_t dip, uint16_t sport,
				                 uint16_t dport, uint8_t proto,
                                 uint8_t is_ipv4)
{
	BUG_ON(f == (fa2_flentry_t *)NULL);

	if (is_ipv4) { /* ipv4 */
		return ((f->sport == sport)
			&& (f->dport == dport)
			&& (f->ip.v4.sip == sip)
			&& (f->ip.v4.dip == dip)
			&& (f->protocol == proto));
	} else { /* ipv6 */
		return ((f->sport == sport)
			&& (f->dport == dport)
			&& (memcmp(f->ip.v6.sip, (uint32_t *)sip, 16) == 0)
			&& (memcmp(f->ip.v6.dip, (uint32_t *)dip, 16) == 0)
			&& (f->protocol == proto));
	}
}

/* This function searches an entry in the opposite direction, associated with
 * this connection. If the other entry is not found, it means this is the
 * first connection entry to be added to flow table; so we will wait for other 
 * connection entry to be added
 * tp is array consisting of 5-tuple values. The values will be different based
 * on whether we need only ip addr translation, or, both ip and port translation
 */
static int fa2_find_pair_entry(fa2_flentry_t *flow, uint32_t *tp)
{
    int indx;
    fa2_flentry_t *f;
    uint8_t ipv4_entry;

    for (indx = 0; indx < FA2_FLOWTBL_SIZE; indx++) {

		f = &fa2_flcache[indx]; // chk

        /* Avoid empty entries */
        if (f->location == FA2_FL_INVALID) {
            continue;
        }

        /* Avoid comparing entry with itself */
        if (f == flow) {
            continue;
        }

        //if ((f->location == FA2_FL_IN_HW) && /* chk if entry is in hw */
        // Entry will never been in h/w when this fn is called, because
        // we write both entries to h/w only if pair entry is found
        if ((f->word1 & FA2_DRV_IPV4_ENTRY) ==  /* chk if L3 protos match */
             (flow->word1 & FA2_DRV_IPV4_ENTRY)) {

            ipv4_entry = (f->word1 & FA2_DRV_IPV4_ENTRY) ? 1 : 0;

            if (ipv4_entry) {

                if (fa2_match(f, tp[0], tp[1], tp[2], tp[3], tp[4],
                             ipv4_entry)) {
                    break; /* ipv4 entry found */
                }

            }
            else {

                if (fa2_match(f, (uint32_t)(&tp[0]), (uint32_t)(&tp[4]), 
                             tp[8], tp[9], tp[10], ipv4_entry)) {
                    break; /* ipv6 entry found */
                 }

            }
        }
    }

    /* If entry was not found return -1 */
    if (indx >= FA2_FLOWTBL_SIZE) {

        indx = -1;

    }

    /* Return -1 if other conn entry is not present, else 0-based table index */
    return(indx);
}

void print_opcode3_flds(uint32_t *v)
{
    uint32_t val;

    printk("\nbcm hdr word0 = 0x%08X, word1 = 0x%08X\n", *v, *(v + 1));

    val = FA2_GETBITS(*v, FA2_BCMHDR_OPCODE, FA2_BCMHDR_OPCODE_SHIFT);
    printk("\nBCM HDR OPCODE = %d\n", val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_RSVD, FA2_BCMHDR_OP_3_RSVD_SHIFT);
    printk("\nRSVD = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_ING_BH_OP, FA2_BCMHDR_OP_3_ING_BH_OP_SHIFT);
    printk("\nING_BCMHDR_OP = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_CLASS_ID, FA2_BCMHDR_OP_3_CLASS_ID_SHIFT);
    printk("\nCLASSIFICATION_ID = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_REASON_CODE, FA2_BCMHDR_OP_3_REASON_CODE_SHIFT);
    printk("\nREASON_CODE = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_TC, FA2_BCMHDR_OP_3_TC_SHIFT);
    printk("\nTC = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_SRC_PID, FA2_BCMHDR_OP_3_SRC_PID_SHIFT);
    printk("\nSRC_PID = 0x%X (%d)", val, val);

    printk("\n\nWord #1 fields:\n");

    v++; /* Note, we are accessing the next word now */

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_W1_RSVD, FA2_BCMHDR_OP_3_W1_RSVD_SHIFT);
    printk("\nRSVD = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_W1_TE, FA2_BCMHDR_OP_3_W1_TE_SHIFT);
    printk("\nTE = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_TS, FA2_BCMHDR_OP_3_TS_SHIFT);
    printk("\nTS = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_DST_MAP, FA2_BCMHDR_OP_3_DST_MAP_SHIFT);
    printk("\nDST_MAP = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_L4_OFFSET, FA2_BCMHDR_OP_3_L4_OFFSET_SHIFT);
    printk("\nL4_OFFSET = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_FWD_OP, FA2_BCMHDR_OP_3_FWD_OP_SHIFT);
    printk("\nFWD_OP = 0x%X (%d)", val, val);

    val = FA2_GETBITS(*v, FA2_BCMHDR_OP_3_PROC_OP, FA2_BCMHDR_OP_3_PROC_OP_SHIFT);
    printk("\nPROC_OP = 0x%X (%d)\n\n", val, val);
}

/* Make sure that lock is obtained before calling this function */
int fa2_get_packet_info(struct sk_buff *skb, struct fa2_pkt_info *info)
{
    struct ethhdr *mac;
	struct iphdr		*iph = NULL;
	struct ipv6hdr		*ipv6h = NULL;
    uint32_t data_loc;
    int ret;
#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
#endif

    if ((skb == NULL) || (info == NULL)) {
        return -1;
    }

    info->mac = mac = (struct ethhdr *)&skb->data[4];

    ret = fa2_get_frame_info(skb->data, &info->et_type, &info->vlan_tag,
                             &info->vlan_tag_next, &data_loc,
                             &info->eth_snapllc, 0);

    if (fa2_pppoe) {

        if (info->et_type == 0x8864) { /* Check if pppoe session frame */
            uint8_t proto_val;

            data_loc += 8; /* Skip 8 bytes (PPPoE header +
                              PPP protocol bytes) */

            /* Check L3 protocol */
			proto_val = (skb->data[data_loc] & 0xF0) >> 4;
			info->et_type = (proto_val == 0x4) ? ETH_P_IP : 
                      (proto_val == 0x6) ? ETH_P_IPV6 :
                      info->et_type;

        }
    }

	/* Determine L4 Header if TCP/UDP */
	if (info->et_type == ETH_P_IP) {
        iph = (struct iphdr *)&(skb->data[data_loc]);

        info->ipv4_or_ipv6 = (void *)iph;

		if ((iph->protocol != IPPROTO_TCP) &&
		    (iph->protocol != IPPROTO_UDP)) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
			    printk(KERN_DEBUG "%s:IPv4: Skip L4 processing for proto"
                " %u(0x%X) in skb: \n", __func__, iph->protocol, iph->protocol);
            }

            info->proto = FA2_PROTO_NOT_SUPPORTED;
            info->tcp_or_udp = (void *)NULL;
            info->need_hdr_bytes = 0;
            info->hdr_words[0] = info->hdr_words[1] = 0;

			goto done;
		}

        if (iph->protocol == IPPROTO_TCP) {
            info->proto = 1; /* TCP */
        }
        else {
            info->proto = 0; /* UDP */
        }
#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
        info->need_hdr_bytes = 4;
#endif

    } else if (info->et_type == ETH_P_IPV6) {
        ipv6h = (struct ipv6hdr *)&(skb->data[data_loc]);

        info->ipv4_or_ipv6 = (void *)ipv6h;

		if ((ipv6h->nexthdr != NEXTHDR_TCP) &&
		    (ipv6h->nexthdr != NEXTHDR_UDP)) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
		        printk(KERN_DEBUG "%s: IPv6: Unsupported L4 proto in skb"
                       " %u(0x%X)\n", __func__, ipv6h->nexthdr, ipv6h->nexthdr);
		    }

            info->proto = FA2_PROTO_NOT_SUPPORTED;
            info->tcp_or_udp = (void *)NULL;
            info->need_hdr_bytes = 0;
            info->hdr_words[0] = info->hdr_words[1] = 0;
            goto done;
		}

        if (ipv6h->nexthdr == NEXTHDR_TCP) {
            info->proto = 1; /* TCP */
        }
        else {
            info->proto = 0; /* UDP */
        }

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
        info->need_hdr_bytes = 4;
#else
        info->need_hdr_bytes = 0;
        info->hdr_words[0] = info->hdr_words[1] = 0;
#endif
    } else {
        if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
            printk(KERN_DEBUG "\n%s: No processing for et_type 0x%X\n",
                   __func__, info->et_type);
        }

        info->ipv4_or_ipv6 = (void *)NULL;
        info->proto = FA2_PROTO_NOT_SUPPORTED;
        info->tcp_or_udp = (void *)NULL;
        info->need_hdr_bytes = 0;
        info->hdr_words[0] = info->hdr_words[1] = 0;
        goto done;
    }

    if (info->proto) {
        /* TCP */
        if (info->et_type == ETH_P_IPV6) { /* v6 */
            info->tcp_or_udp = (void *)((struct tcphdr *)(ipv6h + 1));
        } else { /* v4 */
            info->tcp_or_udp = (void *)((struct tcphdr *)((unsigned int)iph + (iph->ihl * 4)));
        }
    } else {
        /* UDP */
        if (info->et_type == ETH_P_IPV6) { /* v6 */
            info->tcp_or_udp = (void *)((struct udphdr *)(ipv6h + 1));
        } else { /* v4 */
            info->tcp_or_udp = (void *)((struct udphdr *)((unsigned int)iph + (iph->ihl * 4)));
        }
    }

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    /* Notes:
     * 1. For an 'offload' packet, 'pkt_type' is _not_ used to determine
     * pipeline processing; 'proc_op' is used for that.
     * 2. Also, for nfct == null, no pipeline proc will occur.
     */
    info->pkt_type = FA2_LOCAL_TX_PKT;

#ifndef CONFIG_IPROC_FA2_NAPT_BYPASS
	if (skb->nfct != NULL) {
        /* If forwarded packet, we do not want h/w to insert checksum. So, do 
         * not append bcm hdr 0x3. Send it on AXI0
         */
        if ((skb->napt_hash_ix != BCM_CTF_INVALID_IDX_VAL) &&
            (skb->napt_bkt_ix != BCM_CTF_INVALID_IDX_VAL)) {

            info->pkt_type = FA2_FWD_PKT;
            info->hdr_words[0] = 0;
            info->hdr_words[1] = 0;
            goto done;
        }
    }
#endif /* CONFIG_IPROC_FA2_NAPT_BYPASS */

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD_SMALL_PKT_WA)
    /* Note we are assuming 4 bytes appended in et_start */
    if (skb->len <= 68) {
        info->pkt_type = FA2_LOCAL_SMALL_TX_PKT;
    }
#endif



    /* For performance, we only set the non zero fields */
    info->hdr_words[0] = FA2_BCMHDR_OPCODE_3 <<  FA2_BCMHDR_OPCODE_SHIFT;

    {
        uint8_t p = info->proto ? 0x3 : 0x4;
        uint8_t f = 0x3;

        FA2_MAKE_BCMHDR_3_W1_PARTIAL(info->hdr_words[1], p, f, 
                                     (data_loc + (iph->ihl * 4) - 4));
    }

    /* Based on proc_op, we will either proceed further in to the function (for populating flow entries), or simply return after creating bcm hdr 3 (and then wait for rx intr to deliver packet with updated chksum, if the action is to send it back to the CPU (value of fwd_op) , as in chksum offload feature...).
     */


#else
    /* If ingress opcode 1 is required, value of word0 below will change */
    info->hdr_words[0] = 0;
    info->hdr_words[1] = 0;
#endif

done:

    return(0);
}

int fa2_transmit(struct sk_buff *skb, struct fa2_pkt_info *pkt_info)
{
#ifndef CONFIG_IPROC_FA2_NAPT_BYPASS
    int ret;
    uint32_t bcmhdr0, bcmhdr1;
    uint8_t op;
    int nhidx, rtidx;
    uint16_t sport, dport;
    fa2_flentry_t *f;
    struct fa2_rtmac_tbl_entry rt;
    struct fa2_nh_tbl_entry nh;
    int rev_flow_idx = 0xFFFF;
    struct fa2_pkt_info p;
    struct fa2_pkt_info *pi;
    unsigned long flags;
#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    uint8_t p_op;
#endif

    BUG_ON(skb == (struct sk_buff*)NULL);
    if (!spin_trylock_irqsave(&fa2_lock, flags)) {
        /* Lock is in use by rcv thread, return. Lock will be available for
         * future packet processing
         */
        return FA2_PKT_NORM;
    }


    /* */
    bcmhdr0 = ntohl(*((uint32_t *)skb->data));
    bcmhdr1 = 0;

    op = FA2_GETBITS(bcmhdr0, FA2_BCMHDR_OPCODE, FA2_BCMHDR_OPCODE_SHIFT);

#if !defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    if ((op == FA2_BCMHDR_OPCODE_1) || (op == FA2_BCMHDR_OPCODE_3)) {

        bcmhdr1 = ntohl(*((uint32_t *)(skb->data + 4)));

        if (fa2_dbg_log_lvl > FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "\n%s: opcode %u: words 0, 1:  0x%08X, 0x%08X\n",
                   __func__, (op == FA2_BCMHDR_OPCODE_1) ? FA2_BCMHDR_OPCODE_1 : 
                   FA2_BCMHDR_OPCODE_3, bcmhdr0, bcmhdr1);
        }

	    goto done;
    }
#endif 

    /* If packet info is available, do not parse the packet again */
    if (pkt_info == NULL) {
        memset((void *)&p, 0x0, sizeof(p));
        fa2_get_packet_info(skb, &p);

        pi = &p;
    } else {
        pi = pkt_info;
    }

    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
       printk(KERN_DEBUG "%s: vltag=%u, et_type=0x%X, vltag_next=%u, "
              "protocol=%s\n", __func__, pi->vlan_tag, pi->et_type,
              pi->vlan_tag_next, (pi->proto == 1) ? 
                                 "tcp" : (pi->proto == 0) ? "udp" : "n/a");

       printk(KERN_DEBUG "\nskb hash, bkt %u %u\n",
              skb->napt_hash_ix, skb->napt_bkt_ix);
    }

    if (pi->proto == FA2_PROTO_NOT_SUPPORTED) {
        printk(KERN_DEBUG "\n No processing reqd\n");
        goto done;
    }

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD)
    /* We need to pass the forwarded packets without appending bcm hdr 0x3.
     * Also, the 'miss' packets need to be processed in the forward path. So we
     * make a check here
     */
    switch (pi->pkt_type) {
        case FA2_FWD_PKT:
            /* This is a forwarded packet, proceed in to the function */

            if (skb->ip_summed == CHECKSUM_PARTIAL) {
                int ret;

                ret = skb_checksum_help(skb);

                if (ret) {
                    printk(KERN_DEBUG "\nskb_checksum_help returned error %d\n",
                           ret);
                    goto done;
                }
            }
            break;

        case FA2_LOCAL_TX_PKT:
            p_op = FA2_GETBITS(pi->hdr_words[1], FA2_BCMHDR_OP_3_PROC_OP,
                               FA2_BCMHDR_OP_3_PROC_OP_SHIFT);

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                printk(KERN_DEBUG "\n%s: word0=0x%08X, word1=0x%08X, p_op=%u\n",
                       __func__, pi->hdr_words[0], pi->hdr_words[1], p_op);
            }

            switch (p_op) {
                case 0:
                case 1:
                case 2:
                case 5:
                    /* pipeline processing op code, go further in to this
                     * function
                     */

                    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                        printk(KERN_DEBUG "\n%s: proc_op = %u. pipeline proc"
                               " reqd\n", __func__, p_op);
                    }
                    break;

                default:
                    /* other op code. Does not require pipeline processing */

                    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
                        printk(KERN_DEBUG "\n%s: proc_op = %u. No pipeline proc"
                               " reqd\n", __func__, p_op);
                    }

                    goto done;
            }
            break;

#if defined(CONFIG_IPROC_FA2_CS_OFFLOAD_SMALL_PKT_WA)
        /* For small packets we do not append bcm hdr 0x3, and let it go on
         * AXI0
         */
        case FA2_LOCAL_SMALL_TX_PKT:
            if (skb->ip_summed == CHECKSUM_PARTIAL) {
                //int ret;

                /* Note: In et_start 4 bytes are pushed. For any push/pull
                 * changes there, we may need to make changes here
                 */
/*
                ret = skb_checksum_help(skb);


                if (ret) {
                    printk(KERN_DEBUG "\nskb_checksum_help returned error %d\n",
                           ret);
                    goto done;
                }
            printk("\nCalled skb_chsum_help\n");
*/
            }

            break;
#endif

        default:
            printk(KERN_DEBUG "\nUnknown value of pkt_type %u\n", pi->pkt_type);
            goto done;
    }
#endif /* CONFIG_IPROC_FA2_CS_OFFLOAD */

	if (skb->nfct != NULL) {
        if ((skb->napt_hash_ix == BCM_CTF_INVALID_IDX_VAL) ||
            (skb->napt_bkt_ix == BCM_CTF_INVALID_IDX_VAL)) {

            goto done;

        }

        BUG_ON(skb->napt_hash_ix >= FA2_FLOWTBL_SIZE);

        /* Get the flow cache entry */
		f = &fa2_flcache[skb->napt_hash_ix];

        if (fa2_dbg_log_lvl > FA2_DBG_LOW_LVL) {
            printk(KERN_DEBUG "%s: Using flow entry %d now\n",
                   __func__, skb->napt_hash_ix);
        }

/* ----------------- */
	    BUG_ON(f == (fa2_flentry_t *)NULL);

        /* If this flow is already programmed, no need to check it again.
         * This check may have been at the beginning of the fn, but since 
         * it also receives arp, icmp and other L3/L4 packets for which 
         * flow index is not defined, we keep it here
        */
        if (f->location == FA2_FL_IN_HW) {
            //&& (fa2_flcache[rev_flow_idx].location == FA2_FL_IN_HW)

            goto done;
        }
/* ----------------- */
	} else {
        goto done;
	}

	/* Validate nf_conn status if packet is tracked by netfilter */
	if ((f->ct != (struct nf_conn *)NULL)) {
		struct nf_conn_help *help;

		/* Netfilter conntrack is confirmed */
		if ((pi->proto) && (!test_bit(IPS_CONFIRMED_BIT, &f->ct->status))) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {

			    printk(KERN_DEBUG "Connection not yet confirmed for"
                       " ct 0x%08X\n", (unsigned int)(f->ct));
            }

            goto done;
		}

        /* TODO Check if this is reqd */
        /* Add delay to throttle the connection establishment and control
         * traffic, so that the stack gets time to update connection state,
         * and flow entries are populated. After connection is fully 
         * established, we do not delay future packets
         */
        /* TCP protocol */
        if (pi->proto) {
            //udelay(1000);
        }

		/*
		 * Delay activation of IP flows until flow is "assured"
		 * An assured connection is typically never deleted
		 * prematurely.
		 */
		if ((pi->proto) && (!test_bit(IPS_ASSURED_BIT, &f->ct->status))) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
			    printk("Connection not yet assured for ct<0x%08x>"
			           " proto<%u>\n", (unsigned int)f->ct, f->protocol);
            }

            goto done;
		}

        /* Applies for TCP */
        if (pi->proto) {
            f->word1 |= FA2_DRV_CONN_ASSURED;
        }

/* ------------------- */
        /* Note */
        /* We need to have ls bits of mac in ms positions of s/w structs 
         * (nh, rt) because they are copied in to h/w registers starting with 
         * nh/rt's mac addr 0, so that ls bits of mac are copied in to data0 
         * reg, ms bits of mac are copied in to data1 reg
         */
        /* Next hop table */
        memset((void *)(&nh), 0x0, sizeof(struct fa2_nh_tbl_entry));
        nh.vlan = pi->vlan_tag;
        nh.op = 1; /* Keeping same as fa, chk if change needed */ 
        nh.l2_frmtype = 0; /* --""-- */
        //memcpy(nh.da, mac->h_dest, 6);
        FA2_SWAP_MAC_FOR_HW(nh.da, pi->mac->h_dest);

        /* RTMAC table */
        memset((void *)(&rt), 0x0, sizeof(struct fa2_rtmac_tbl_entry));
        rt.host = 1;
        rt.l4_checksum_check = 0;
        //memcpy(rt.mac_addr, mac->h_source, 6);
        FA2_SWAP_MAC_FOR_HW(rt.mac_addr, pi->mac->h_source);

        if (f->word0 & FA2_DRV_DIRN) {
			/* WAN to LAN Inbound */
            rt.external = 0;
		} else {
		    /* LAN to WAN Outbound */
            rt.external = 1;
	    }

        if (fa2_add_nh_entry(&nh, &nhidx, 0) != FA2_SUCCESS) {
            printk(KERN_DEBUG "\nFailed to search/add to nh table,"
                   " returning\n");
            goto done;
        }

        if (fa2_add_rtmac_entry(&rt, &rtidx, 0) != FA2_SUCCESS) {
            printk(KERN_DEBUG "\nFailed to search/add to rtmac table,"
                   " returning\n");
            goto done;
        }

        printk(KERN_DEBUG "\nflid=%u, nhidx = %u, rtidx = %u\n", 
               f->flid, nhidx, rtidx );

        FA2_SETBITS(f->word0, nhidx, FA2_DRV_NXT_HOP_IDX, 
                    FA2_DRV_NXT_HOP_IDX_SHIFT);
        FA2_SETBITS(f->word0, rtidx, FA2_DRV_REMAP_SA_IDX, 
                    FA2_DRV_REMAP_SA_IDX_SHIFT);

/* ------------------- */
		help = nfct_help(f->ct);

		if ((help != (struct nf_conn_help *)NULL) &&
		   (help->helper != (struct nf_conntrack_helper *)NULL)) {

            if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
			    printk("Helper pkt ct 0x%08x helper %s proto %u\n",
			           (unsigned int)f->ct, help->helper->name, f->protocol);
            }

            goto done;
		}

        if (pi->proto) {
            sport = ntohs(((struct tcphdr *)(pi->tcp_or_udp))->source);
            dport = ntohs(((struct tcphdr *)(pi->tcp_or_udp))->dest);
        } else {
            sport = ntohs(((struct udphdr *)(pi->tcp_or_udp))->source);
            dport = ntohs(((struct udphdr *)(pi->tcp_or_udp))->dest);
        }

        /* Section for finding flow in reverse direction */
		switch(pi->et_type) {
		    case ETH_P_IP:
                {
                    uint32_t tuple[5];

                    if (!(f->word0 & FA2_DRV_DIRN)) {
                        /* This is outbound entry, create inbound tuple */
                        tuple[0] /* sip */   = f->ip.v4.dip; /* from fa2_rcv */
                        tuple[1] /* dip */   = ntohl(((struct iphdr *)(pi->ipv4_or_ipv6))->saddr); /* from skb */
                        tuple[2] /* sport */ = f->dport;
                        tuple[3] /* dport */ = sport;
                    }
                    else {
                        /* This is inbound entry, create outbound tuple */
                        tuple[0] /* sip */   = ntohl(((struct iphdr *)(pi->ipv4_or_ipv6))->daddr); /* from skb */ 
                        tuple[1] /* dip */   = f->ip.v4.sip;
                        tuple[2] /* sport */ = dport; /* from skb */
                        tuple[3] /* dport */ = f->sport;
                    }

                    tuple[4] /* proto */ = f->protocol;

                    printk(KERN_DEBUG "\n----- rev tuple: sip=0x%08X, dip=0x%08X, sport=%u, dport=%u, l4_proto=%u, l3_proto=%s\n", tuple[0], tuple[1], tuple[2], tuple[3], tuple[4], (f->word1 & FA2_DRV_IPV4_ENTRY) ? "ipv4" : "ipv6");
                    rev_flow_idx = fa2_find_pair_entry(f, tuple);
                }

                //printk(KERN_INFO "\nskb_hashidx=%d, revflow_idx=%d\n", skb->napt_hash_ix, rev_flow_idx);
                if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {

			        printk(KERN_DEBUG "IPv4 Flow idx %d: sip 0x%08X dip 0x%08X "
			               " sport %u dport %u dir=%s, revflow_idx=%d\n",
			               f->flid, ntohl(((struct iphdr *)(pi->ipv4_or_ipv6))->saddr), ntohl(((struct iphdr *)(pi->ipv4_or_ipv6))->daddr),
			               sport, dport,
			               (f->word0 & FA2_DRV_DIRN) ? "Inbound" : "Outbound",
                           rev_flow_idx);
                }

			break;

		case ETH_P_IPV6:
                /* ipv6 entry */
                {
                    uint32_t tuple[11];

                    if (!(f->word0 & FA2_DRV_DIRN)) {
                        /* This is outbound entry, create inbound tuple */
                        tuple[0] /* sip */ = *((uint32_t *)(f->ip.v6.dip + 0));
                        tuple[1] /* sip */ = *((uint32_t *)(f->ip.v6.dip + 4));
                        tuple[2] /* sip */ = *((uint32_t *)(f->ip.v6.dip + 8));
                        tuple[3] /* sip */ = *((uint32_t *)(f->ip.v6.dip + 12));

                        tuple[4] /* dip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->saddr.s6_addr + 0));
                        tuple[5] /* dip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->saddr.s6_addr + 4));
                        tuple[6] /* dip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->saddr.s6_addr + 8));
                        tuple[7] /* dip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->saddr.s6_addr + 12));

                        tuple[8] /* sport */ = f->dport;
                        tuple[9] /* dport */ = sport;
                    } else {
                        /* This is inbound entry, create outbound tuple */
                        tuple[0] /* sip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->daddr.s6_addr + 0));
                        tuple[1] /* sip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->daddr.s6_addr + 4));
                        tuple[2] /* sip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->daddr.s6_addr + 8));
                        tuple[3] /* sip */ = *((uint32_t *)(((struct ipv6hdr *)(pi->ipv4_or_ipv6))->daddr.s6_addr + 12));

                        tuple[4] /* dip */ = *((uint32_t *)(f->ip.v6.sip + 0));
                        tuple[5] /* dip */ = *((uint32_t *)(f->ip.v6.sip + 4));
                        tuple[6] /* dip */ = *((uint32_t *)(f->ip.v6.sip + 8));
                        tuple[7] /* dip */ = *((uint32_t *)(f->ip.v6.sip + 12));

                        tuple[8] /* sport */ = dport;
                        tuple[9] /* dport */ = f->sport;
                    }

                    tuple[10] /* proto */ = f->protocol;

                    rev_flow_idx = fa2_find_pair_entry(f, tuple);
                }

                if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
			        printk(KERN_DEBUG "IPv6 Flow idx = %d,"
			               " sport %u dport %u dir=%s, revflow_idx=%d\n",
			               f->flid,
			               sport, dport,
			               (f->word0 & FA2_DRV_DIRN) ? "Inbound" : "Outbound", 
                           rev_flow_idx);
                    fa2_display_ipv6_addr(f->ip.v6.sip, "sip", 1);
                    fa2_display_ipv6_addr(f->ip.v6.dip, "dip", 1);
                }
			break;

		default:
			printk(KERN_DEBUG "\nUnknown layer 3 protocol %u\n", pi->et_type);
			goto done;
		}

        if (rev_flow_idx != -1) {

			printk(KERN_DEBUG "\nThis flow is %u. Reverse flow is %u\n",
                   f->flid, rev_flow_idx);

            /* If reverse flow is already associated with another flow, it
             * means this flow is a duplicate of the 1st flow, ignore it
             */
            if (fa2_flcache[rev_flow_idx].word1 & FA2_DRV_REV_FL_VALID) {
                goto done;
            }

            /* For TCP only: If both flows are not yet assured, wait until the 
             * stack marks them in assured state
             */
            if (pi->proto) {
                if ((!(fa2_flcache[rev_flow_idx].word1 & FA2_DRV_CONN_ASSURED))
                    || (!(f->word1 & FA2_DRV_CONN_ASSURED))) {
                    goto done;
                }
            }

            /* Set the 'first' flow's pair entry to this entry */
            FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, f->flid,
                        FA2_DRV_REV_FLOW_PTR, FA2_DRV_REV_FLOW_PTR_SHIFT);

            FA2_SETBITS(f->word0, rev_flow_idx, FA2_DRV_REV_FLOW_PTR, 
                        FA2_DRV_REV_FLOW_PTR_SHIFT);

            /* Mark rev flow as valid */
            f->word1 |= FA2_DRV_REV_FL_VALID;
            fa2_flcache[rev_flow_idx].word1 |= FA2_DRV_REV_FL_VALID;

            /* Set MTU idx locations */
            /* This is simplified way to initialize mtu_idx. locn 0 for inbound 
             * flows, 1 for outbound flows. Should be done at init time, or as
             * part of external command's execution, or config profile. For now,
             * we hard code the value. TODO program MTU idx and port type in h/w
             * in init code
             */
            if (f->word0 & FA2_DRV_DIRN) {
			    /* Inbound */
                FA2_SETBITS(f->word0, 0x0, FA2_DRV_MTU_IDX, 
                            FA2_DRV_MTU_IDX_SHIFT);
                FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, 0x1,
                            FA2_DRV_MTU_IDX, FA2_DRV_MTU_IDX_SHIFT);
		    } else {
		        /* Outbound */
                FA2_SETBITS(f->word0, 0x1, FA2_DRV_MTU_IDX, 
                            FA2_DRV_MTU_IDX_SHIFT);
                FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, 0x0, 
                            FA2_DRV_MTU_IDX, FA2_DRV_MTU_IDX_SHIFT);
	        }

            printk(KERN_DEBUG "\nIn %s, fa2_pppoe=%d \n", __func__, fa2_pppoe);

            if (fa2_pppoe == true) {
            
                FA2_SETBITS(f->word0, 0x1, FA2_DRV_PPPOE_TUNL_EN, 
                            FA2_DRV_PPPOE_TUNL_EN_SHIFT);

                FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, 0x1, 
                            FA2_DRV_PPPOE_TUNL_EN, FA2_DRV_PPPOE_TUNL_EN_SHIFT);

                /* TODO Temp code for unit testing. In normal code, we will 
                 * get the smac, dmac, session id, find it in the tunnel 
                 * table and then use that index below
                 */
                FA2_SETBITS(f->word0, 0x0, FA2_DRV_PPPOE_TUNL_IDX, 
                            FA2_DRV_PPPOE_TUNL_IDX_SHIFT);
                FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, 0x0,                                           FA2_DRV_PPPOE_TUNL_IDX,
                            FA2_DRV_PPPOE_TUNL_IDX_SHIFT);
            } else {
                /* To make sure the tunnel en is 0 */
                FA2_SETBITS(f->word0, 0x0, FA2_DRV_PPPOE_TUNL_EN, 
                            FA2_DRV_PPPOE_TUNL_EN_SHIFT);
                FA2_SETBITS(fa2_flcache[rev_flow_idx].word0, 0x0,
                            FA2_DRV_PPPOE_TUNL_IDX, 
                            FA2_DRV_PPPOE_TUNL_IDX_SHIFT);
            }

            /* Add both entries (in non-debugging case) of a connection to h/w*/
            ret = fa2_write_to_hw(f);

            if (ret == FA2_SUCCESS) {
                ret = fa2_write_to_hw(&fa2_flcache[rev_flow_idx]);
            }

            if (ret == FA2_SUCCESS) {
                /* Note: drv updated after both entries get actually added 
                 * to h/w
                 */
                f->location = FA2_FL_IN_HW;
                fa2_flcache[rev_flow_idx].location = FA2_FL_IN_HW;

                f->word0 |= FA2_DRV_VALID;
                fa2_flcache[rev_flow_idx].word0 |= FA2_DRV_VALID;
            } else {
                printk(KERN_INFO "\n%s: Error: Failed to program flows to h/w"
                       "flow_idx=%u, rev flow idx=%d\n", __func__, f->flid,
                       rev_flow_idx);

                goto done;
            }

        } else {
            /* This is the first flow of a connection. Wait for the 
             * second entry for other direction to be populated, and 
             * then write both entries simultaneaously to h/w
             */
            /* Mark rev flow as invalid */
            f->word1 &= ~FA2_DRV_REV_FL_VALID;
            goto done;
        }
	} else {

		printk(KERN_DEBUG "\n%s: Conntrack does not exist...\n", __func__);

        goto done;
	}

done:
    spin_unlock_irqrestore(&fa2_lock, flags);
#endif /* CONFIG_IPROC_FA2_NAPT_BYPASS */
	return FA2_PKT_NORM;
}


void fa2_nfct(struct sk_buff *skb, struct nf_conn *ct)
{
#ifndef CONFIG_IPROC_FA2_NAPT_BYPASS
    fa2_flentry_t *f;

    if (fa2_dbg_log_lvl > FA2_DBG_MEDIUM_LVL) {
        printk(KERN_DEBUG "%s: fl idx is %u for ct 0x%p",
               __func__, skb->napt_hash_ix, ct);
    }

    if ((skb->napt_hash_ix == BCM_CTF_INVALID_IDX_VAL) ||
        (skb->napt_bkt_ix == BCM_CTF_INVALID_IDX_VAL)) {

        return; /* reference will be added in the receive direction */

    }

    if (skb->napt_hash_ix >= FA2_FLOWTBL_SIZE) {
        return;
    }

    /* Get flow cache entry */
	f = &fa2_flcache[skb->napt_hash_ix];

    f->ct = ct;
    f->nfct = skb->nfct;
#endif /* CONFIG_IPROC_FA2_NAPT_BYPASS */
}

void fa2_enable_pppoe(void)
{
    fa2_pppoe = true;
}

void fa2_disable_pppoe(void)
{
    fa2_pppoe = false;
}

/* This function is used to add a tunnel to fa+ tunnel table. It return index
 * to the tunnel table entry on success. Or -1 on failure.
 * The tunnel index can be used by the caller to delete the tunnel
 */
int fa2_setup_pppoe_tunnel(int session_id, char *smac, char *dmac)
{
    struct fa2_rtmac_tbl_entry rt;
    struct fa2_tunnel_tbl_entry tunl_ent;
    int rtidx, tunl_idx;
    int ret;

    printk(KERN_DEBUG "\nIn %s \n", __func__);

    printk(KERN_DEBUG "\nSession id = %d, \n", session_id);
    printk(KERN_DEBUG "\nsmac[0-5] = 0x %02X%02X%02X%02X%02X%02X\n",
           smac[0], smac[1], smac[2], smac[3], smac[4], smac[5]);
    printk(KERN_DEBUG "\ndmac[0-5] = 0x %02X%02X%02X%02X%02X%02X\n",
           dmac[0], dmac[1], dmac[2], dmac[3], dmac[4], dmac[5]);

    /* Look for mac address in rtmac table first */
    memset((void *)(&rt), 0x0, sizeof(struct fa2_rtmac_tbl_entry));
    rt.host = 1;
    rt.l4_checksum_check = 0;
    FA2_SWAP_MAC_FOR_HW(rt.mac_addr, dmac);
    /* Note router is pppoe client, so LAN to WAN outbound */
    rt.external = 1; /* See table 17 for value */

    /* Check if mac is present in rtmac table, if not, add it*/
    if ((ret =fa2_add_rtmac_entry(&rt, &rtidx, 0)) != FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Failed to search/add to rtmac table,"
                   " returning\n", __func__);
        goto done;
    }

    printk(KERN_DEBUG "\nrtidx = %d\n", rtidx);

    tunl_ent.session_id = session_id;
    //tunl_ent.dmac_idx = rtidx; Use when mac check is enabled in reg 0x0,
    // comment in table #18
    tunl_ent.dmac_idx = 0;
    /* Strip header */
    tunl_ent.action = 0;
    FA2_SWAP_MAC_FOR_HW(tunl_ent.smac, smac);

    ret = fa2_add_tunl_entry(&tunl_ent, &tunl_idx, 0);

    if (ret != FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Failed to search/add to tunnel table,"
                   " returning\n", __func__);
        goto done;
    } else {
        printk(KERN_DEBUG "\nrtidx = %d\n", tunl_idx);
    }

    return tunl_idx;
done:

    return -1;
}

/*
 * This function is used to tear down a tunnel previously created. It accepts
 * a tunnel index which was sent to the caller during addition of a tunnel
 */
int fa2_remove_pppoe_tunnel(int tunl_idx)
{

    printk(KERN_DEBUG "\n%s: Tunnel %d\n", __func__, tunl_idx);

    if ((tunl_idx >= FA2_TUNLTBL_SIZE) || (tunl_idx < 0)) {
        return -1;
    }

    if (fa2_del_tunl_entry(NULL, 0, tunl_idx) != FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Failed to delete tunnel %d,"
                   " returning\n", __func__, tunl_idx);
        return -1;
    } else {
        printk(KERN_DEBUG "\n%s: Deleted tunnel %d\n", __func__, tunl_idx);
        return 0;
    }

}

EXPORT_SYMBOL(fa2_tx_to_spu);
/* Send a packet to the FA+ / SPU pipeline */
int fa2_tx_to_spu(struct sk_buff *skb,
                  void (*cb)(unsigned char* data, size_t len, void *cb_data_ptr, unsigned cb_data_idx),
                  void *cb_data_ptr, unsigned cb_data_idx)
{
    int rv = 0;
    unsigned long flags;

    spin_lock_irqsave(&fa2_spu_lock, flags);
    if (spu_trns_head - spu_trns_tail >= MAX_SPU_HOST_PKTS) {
        printk(KERN_WARNING "fa2: too many pkts in flight, dropping.\n");
        rv = -EAGAIN;
        goto err_too_many;
    }

    /* Add callback & data to circular buffer */
    spu_trns_cb[spu_trns_head % MAX_SPU_HOST_PKTS] = cb;
    spu_trns_cb_data_ptr[spu_trns_head % MAX_SPU_HOST_PKTS] = cb_data_ptr;
    spu_trns_cb_data_idx[spu_trns_head % MAX_SPU_HOST_PKTS] = cb_data_idx;

    /* store the transaction ID in the packet, with the "driver" bit set */
    *(skb->data+4) |= (0x80 | (spu_trns_head & 0x3F));
    ++spu_trns_head;
    spin_unlock_irqrestore(&fa2_spu_lock, flags);

    rv = et_fa2_spu_tx(skb);

    if (rv) {
        kfree(skb);
    }

    return rv;

 err_too_many:
    spin_unlock_irqrestore(&fa2_spu_lock, flags);
    kfree_skb(skb);

    return rv;
}


static int fa2_process_spu_result(struct sk_buff *skb)
{
    void *this_cb_data_ptr;
    unsigned this_cb_data_idx;
    unsigned this_msg_len;
    void (*this_cb)(u8 *data, size_t len, void *cb_data_ptr, unsigned cb_data_idx);
    u16 pkt_trns_id;  /* index from packet header */
    u16 pkt_offset_from_tail, head_offset_from_tail;
    int delta_resp_len;
    unsigned long flags;

    if (skb->len < 8 + 4 + 4) { /* 8 Byte BCM Hdr, 4 byte SPU op, 4 byte SPU trans ID */
        printk(KERN_WARNING "Short SPU result seen\n");
        return -EINVAL;
    }

    /* remove BCM header */
    skb_pull(skb, 8);
    
    /* get the index from the packet */
    pkt_trns_id = *(skb->data+4) & 0x3F;
    delta_resp_len = (int8_t)(skb->data[7]);

    spin_lock_irqsave(&fa2_spu_lock, flags);

    pkt_offset_from_tail = (pkt_trns_id - spu_trns_tail) & 0x3f;
    head_offset_from_tail = (spu_trns_head -  spu_trns_tail) & 0x3f;

    /* Note, pkt_offst_from_tail is unsigned, so if pkt_trns_id < spu_trns_tail, it will be very large */
    /*   so, just check to see if offset from tail is between 0 and head_offset_from_tail */
    if (pkt_offset_from_tail >= head_offset_from_tail) {
        u32 cur_head = spu_trns_head;
        u32 cur_tail = spu_trns_tail;
        spin_unlock_irqrestore(&fa2_spu_lock, flags);

        printk(KERN_WARNING "FA2 SPU result with index 0x%04x seen, not within range [0x%08x .. 0x%08x) (masked)",
               pkt_trns_id, cur_tail, cur_head);
        return -EINVAL;
    }

    /* save data needed to perform callback */
    this_cb = spu_trns_cb[pkt_trns_id % MAX_SPU_HOST_PKTS];
    this_cb_data_ptr = spu_trns_cb_data_ptr[pkt_trns_id % MAX_SPU_HOST_PKTS];
    this_cb_data_idx = spu_trns_cb_data_idx[pkt_trns_id % MAX_SPU_HOST_PKTS];
    this_msg_len = skb->len - (uint8_t)(skb->data[6] & 0x3f);  /* remove any padding added by PAE */

    /* flag this entry as unused */
    spu_trns_cb[pkt_trns_id % MAX_SPU_HOST_PKTS] = 0;

    /* move tail up past any unused entries */
    while ((spu_trns_cb[spu_trns_tail % MAX_SPU_HOST_PKTS] == 0) &&
           ((int32_t)(spu_trns_head - spu_trns_tail) > 0)) {
        ++spu_trns_tail;
    }
    spin_unlock_irqrestore(&fa2_spu_lock, flags);

    if (!this_cb) {
        printk(KERN_WARNING "FA2 null callback for SPU operation\n");
        return -EINVAL;
    }

    /* call the callback for this operation */
    (*this_cb)(skb->data, this_msg_len, this_cb_data_ptr, this_cb_data_idx);
    return 0;
}

int fa2_modify_header(struct sk_buff *skb)
{
	struct iphdr	*iph = NULL;
	uint32_t 		hdr_words_tmp=0;
	uint32_t 		data_loc;

	iph = (struct iphdr *)skb->network_header;
	if ((iph->protocol == IPPROTO_TCP) || (iph->protocol == IPPROTO_UDP)) {
		data_loc = (uint32_t)(skb->network_header - skb->data);
		__skb_push(skb, 4);
		*((uint32_t *)skb->data) = htonl(FA2_BCMHDR_OPCODE_3 <<  FA2_BCMHDR_OPCODE_SHIFT);
		if (iph->protocol == IPPROTO_TCP) {
			FA2_MAKE_BCMHDR_3_W1_PARTIAL(hdr_words_tmp, 0x3, 0x3, 
					(data_loc + (iph->ihl * 4) - 4));
		}
		else { 
			FA2_MAKE_BCMHDR_3_W1_PARTIAL(hdr_words_tmp, 0x4, 0x3, 
					(data_loc + (iph->ihl * 4) - 4));
		}
		*((uint32_t *)skb->data + 1) = htonl(hdr_words_tmp);
		__skb_trim(skb, skb->len);

    	return 1;
	} 

    return 0;
}



EXPORT_SYMBOL(fa2_receive);
EXPORT_SYMBOL(fa2_transmit);
EXPORT_SYMBOL(fa2_nfct);
EXPORT_SYMBOL(fa2_enable_pppoe);
EXPORT_SYMBOL(fa2_disable_pppoe);
EXPORT_SYMBOL(fa2_setup_pppoe_tunnel);
EXPORT_SYMBOL(fa2_remove_pppoe_tunnel);
EXPORT_SYMBOL(fa2_modify_header);
