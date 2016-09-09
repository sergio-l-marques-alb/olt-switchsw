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
/*                                                                            */
/* File Name  : ffe.c							      */
/*									      */
/* Description: This file implements the Broadcom Flow Forwarding Engine.     */
/******************************************************************************/
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
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <linux/proc_fs.h>
#include "ctf_common.h"
#include "ctf_regs.h"
#include "fcache.h"

//#define DEBUG	1
flow_cache_t fcache;    /* Global flow cache context */

spinlock_t ctf_lock;

/* Used for flow aging */
static volatile unsigned int flow_timestamp = 0;

extern int ctf_activate_flow(volatile napt_flow_t *flow_p);
extern int ctf_deactivate_flow(volatile napt_flow_t *flow_p);
extern int ctf_clear_napt_flow_table(int index);
extern u16 crc16(u16 crc, u8 const *buffer, size_t len);
extern int ctf_next_hop_table_add_entry(uint32_t vlan_tag,
					uint8_t *sa, uint8_t *sa_ix, uint8_t frm_type, uint8_t op);
extern int ctf_add_napt_pool_entry(unsigned char *smac, uint8_t *pt_ix,
				   int flag);
extern int ctf_update_timestamp(unsigned int ts);
extern int ctf_get_flow_entry_timestamp(uint32_t hashix, uint32_t bktix, 
                                        unsigned int *ts);
extern int is_ctf_initialized(void);
static int fc_find_pair_entry(volatile napt_flow_t *flow_p, uint32_t *tp);

static unsigned int get_curr_tstamp(void)
{
    return(flow_timestamp);
}

/*
 * Function     : fc_index
 * Description  : Given a Flow_t pointer, fetch its hash table index.
 */
static inline uint32_t fc_index(volatile napt_flow_t * flow_p)
{
	uint32_t ix;

	BUG_ON(flow_p == FLOW_NULL);

	ix = (uint32_t)flow_p->key.hash_index;
	BUG_ON(ix > FCACHE_HTABLE_SIZE);
	return ix;
}

/*
 * Function     : fc_entry
 * Description  : Given a flow cache key (hash, bucket), check and return
 *		  pointer to entry.
 */
static inline napt_flow_t *fc_entry(uint32_t hashix, uint32_t bktix)
{
	napt_flow_t *flow_p;
	napt_flow_t *chain_p;

	BUG_ON(hashix > FCACHE_HTABLE_SIZE);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
        printk("%s: h_idx=%u, b_ix=%u\n", __func__, hashix, bktix);

	flow_p = fcache.htable[hashix];
	chain_p = flow_p;

	if (bktix == 0) {
		return chain_p;
	} else {
		while (flow_p != NULL) {
			bktix--;
			chain_p = flow_p->chain_p;
			flow_p = chain_p;
			if (bktix == 0) {
				return chain_p;
			}
		}
	}

	printk(KERN_ERR "Failed to locate entry hashix <%d> bktix <%d>\n",
		hashix, bktix);

	return FLOW_NULL;
}

/*
 * Function   : fc_clear
 *              Packet CTF to flow cache downcall function.
 * Description: Clear flow cache entry(s) association with HW. 
 *              The flow is not removed from the hash table.
 * Parameters :
 *      key   : key refers to the NAPT 5 tuple
 *
 * Returns    : Number of associations cleared.
 */
int fc_clear(uint32_t key)
{
	int count = 0;

	printk(KERN_DEBUG "Clearing NAPT Flows...\n");

	return count;
}

/*
 * Function     : fc_dump_skb
 * Description  : Dump a skb data (dump only first 64 bytes
 */
static void fc_dump_skb(struct sk_buff *skb_p, uint32_t offset)
{
	unsigned char *pdata;
	int ix = 0;

	if (offset >= 0 && offset < skb_p->len) {
		pdata = (unsigned char *)&skb_p->data[offset];
	} else {
		printk("Invalid Offset\n");
		return;
	}
	for (ix = 0; ix < (skb_p->len - offset); ix++) {
		printk("0x%02x ", pdata[ix]);
		if ((ix % 16) == 15) {
			printk("\n");
		}
	}
	printk("\n");
}

/*
 *  Flow cache management
 */
void fc_evict(volatile napt_flow_t *flow_p); /* Evict a flow from the hash table */
int fc_collect(int items);      /* Garbage collect flows that have been idle */

/*
 * Function     : fc_collect
 * Description  : Garbage collect an idle flow if the free list is depleted.
 */
int fc_collect(int items)
{
	int collected = 0;

	return collected;
}

/*
 * Function     : fc_match
 * Description  : Checks whether the flow entry tuple matches.
 */
static inline uint32_t fc_match(volatile napt_flow_t *flow_p, uint32_t sip,
				uint32_t dip, uint16_t sport,
				uint16_t dport, uint16_t proto, uint16_t is_ipv4)
{
	BUG_ON(flow_p == FLOW_NULL);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
        printk("In %s\n", __func__);

	if (is_ipv4) { /* ipv4 */
		return ((flow_p->fe.ipv4.sport == sport)
			&& (flow_p->fe.ipv4.dport == dport)
			&& (flow_p->fe.ipv4.sip == sip)
			&& (flow_p->fe.ipv4.dip == dip)
			&& (flow_p->fe.ipv4.proto == proto));
	} else {
		return ((flow_p->fe.ipv6.sport == sport)
			&& (flow_p->fe.ipv6.dport == dport)
			&& (memcmp(((napt_flow_t *)flow_p)->fe.ipv6.sip, (uint32_t *)sip,
				sizeof(char) * 16) == 0)
			&& (memcmp(((napt_flow_t *)flow_p)->fe.ipv6.dip, (uint32_t *)dip,
				sizeof(char) * 16) == 0)
			&& (flow_p->fe.ipv6.proto == proto));
	}
}

/*
 * Function     : fc_lookup_nf_conn
 * Description  : Given a skb lookup flow cache.
 */
static volatile napt_flow_t *fc_lookup_nf_conn(struct sk_buff *skb_p)
{
	volatile napt_flow_t	*flow_p;
//	napt_flow_t	*chain_p;
	uint32_t	hash_ix;
//	uint32_t	bkt_ix = 3;

	BUG_ON(skb_p == NULL);

	for (hash_ix = 0; hash_ix < FCACHE_MAX_ENTRIES; hash_ix++) {
		flow_p = &fcache.etable[hash_ix];
		if(flow_p->nfct) {
			if (flow_p->nfct == skb_p->nfct) {
				printk("Flow Lookup nfct return "
					"flow <0x%08x>\n",
					(unsigned int)flow_p);
				return flow_p;
			}
		}
	}
	printk("Flow Lookup nfct NOT FOUND\n");
	return FLOW_NULL;
}

/*
 * Function     : fc_lookup
 * Description  : Given an 5-tuple and hash index, lookup flow cache.
 */
static volatile napt_flow_t *fc_lookup(uint32_t hashix, uint32_t bktix)
{
	volatile napt_flow_t *flow_p;
//	napt_flow_t *chain_p;

	BUG_ON(hashix > FCACHE_HTABLE_SIZE);
	BUG_ON(bktix >= FCACHE_MAX_BKT_ENTRIES);

	flow_p = &fcache.etable[(hashix * 4) + bktix];
	return flow_p;
}

/*
 * Function     : fc_hash_insert
 * Description  : Insert a new flow into the flow cache at a given hash index.
 *                Cause the activation of the flow in hardware.
 */
static int fc_hash_insert(volatile napt_flow_t *flow_p)
{
    int ret = CTF_SUCCESS;

	BUG_ON(flow_p == FLOW_NULL);


    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
        printk("\n\nlocn=%u, hash=%u, bkt=%u\n", flow_p->location, flow_p->key.hash_index, flow_p->key.bkt_index);

	/* Activate flow in hardware */
	ret = ctf_activate_flow(flow_p);

    if (ret == CTF_SUCCESS) {
	    printk(KERN_DEBUG "flow<0x%08x> activated\n", (unsigned int)flow_p);
    }

    return ret;
}

/*
 * Function     : fc_alloc
 * Description  : Given an 5-tuple and hash index, allocate flow cache.
 */
static volatile napt_flow_t *fc_alloc(uint32_t hashix, uint32_t bktix)
{
	volatile napt_flow_t *flow_p;
//	napt_flow_t *chain_p;

	BUG_ON(hashix > FCACHE_HTABLE_SIZE);
	BUG_ON(bktix >= FCACHE_MAX_BKT_ENTRIES);

    spin_lock(&ctf_lock);
	flow_p = &fcache.etable[(hashix * 4) + bktix];
    if (flow_p->entry_state == true) {
        spin_unlock(&ctf_lock);
	    return flow_p;
    }
	memset((napt_flow_t *)flow_p, 0, sizeof(napt_flow_t));
	flow_p->key.hash_index = hashix;
	flow_p->key.bkt_index = bktix;
	flow_p->valid = 1;
    flow_p->location = IN_FCACHE;
    flow_p->entry_state = true;
    spin_unlock(&ctf_lock);

	return flow_p;
}

/*
 * Function     : fc_hash_delete
 * Description  : Remove a flow from the flow cache at a given hash index.
 *                Cause the deactivation of the flow in hardware.
 */
static void fc_hash_delete(volatile napt_flow_t *flow_p, uint32_t hashix)
{
	register napt_flow_t *h_flow_p = fcache.htable[hashix];

	BUG_ON(hashix > FCACHE_HTABLE_SIZE);
	BUG_ON(flow_p == FLOW_NULL);

	if (unlikely(h_flow_p == FLOW_NULL)) {
		printk(KERN_DEBUG "fcache.htable[%u] is NULL\n", hashix);
		goto fc_notfound;
	}

	if (likely(h_flow_p == flow_p)) {
		/* Delete the head */
		fcache.htable[ hashix ] = flow_p->chain_p;
	} else {
		uint32_t found = 0;
        
		/* Traverse the single linked hash collision chain */
		for (h_flow_p = fcache.htable[hashix];
			likely(h_flow_p->chain_p != FLOW_NULL);
			h_flow_p = h_flow_p->chain_p) {
			if (h_flow_p->chain_p == flow_p) {
				h_flow_p->chain_p = flow_p->chain_p;
				found = 1;
				break;
			}
		}

		if (unlikely(found == 0)) {
			printk(KERN_DEBUG "fcache.htable[%u] not found\n",
				hashix);
			goto fc_notfound;
		}
	}

	printk(KERN_DEBUG "flow<0x%08x> @ hash<%u>\n", (int)flow_p, hashix);

	ctf_deactivate_flow(flow_p);

fc_notfound:
	return;
}

/* This function searches an entry in the opposite direction, associated with
 * this connection. If the other entry is not found, it means this is the
 * first connection entry to be added to flow table; so we will wait for other 
 * connection entry to be added
 */
static int fc_find_pair_entry(volatile napt_flow_t *flow_p, uint32_t *tp)
{
    int indx;
    volatile napt_flow_t *fptr;
    uint16_t ipv4_entry;

    for (indx = 0; indx < FCACHE_MAX_ENTRIES; indx++) {

		fptr = &fcache.etable[indx];

        /* Avoid comparing entry with itself */
        if (fptr == flow_p) {
            continue;
        }

        if ((fptr->location == IN_FLOW_TABLE) && (fptr->l3_proto == flow_p->l3_proto)) {

            ipv4_entry = (fptr->l3_proto == ETH_P_IP) ? 1 : 0;

            if (ipv4_entry) {

                if (fc_match(fptr, tp[0], tp[1], tp[2], tp[3], tp[4],
                             ipv4_entry)) {
                    break; /* ipv4 entry found */
                }

            }
            else {

                if (fc_match(fptr, (uint32_t)(&tp[0]), (uint32_t)(&tp[4]), 
                             tp[8], tp[9], tp[10], ipv4_entry)) {
                    break; /* ipv6 entry found */
                 }

            }
        }
    }

    /* If entry was not found return -1 */
    if (indx >= FCACHE_MAX_ENTRIES) {

        indx = -1;

    }

	//printk(KERN_INFO "\n%s: tp0=0x%08X, tp1=0x%08X, tp4=%u\n\n", __func__, tp[0], tp[1], tp[4]);

    /* Return -1 if other conn entry is not present, else 0-based table index */
    return(indx);
}

/* This function sets up same timestamp for connection entries in both
 * directions
 */
static int fc_set_conn_timestamp(volatile napt_flow_t *flow_p)
{
    int ret = CTF_SUCCESS;
    unsigned int tstamp;
    int index;
    uint32_t dirn;
    volatile napt_flow_t *fptr;

    /* Note: Called from fc_transmit; spin lock has been acquired */

    dirn = (flow_p->l3_proto == ETH_P_IP) ? flow_p->fe.ipv4.direction :
                                            flow_p->fe.ipv6.direction;

    if (flow_p->l3_proto == ETH_P_IP) {
        uint32_t tuple[5];

        if (dirn == 0) {
            /* This is outbound entry, create inbound tuple */
            tuple[0] /* sip */   = flow_p->fe.ipv4.dip;
            tuple[1] /* dip */   = flow_p->fe.ipv4.remap_ip;
            tuple[2] /* sport */ = flow_p->fe.ipv4.dport;
            tuple[3] /* dport */ = flow_p->fe.ipv4.remap_port;
        }
        else {
            /* This is inbound entry, create outbound tuple */
            tuple[0] /* sip */   = flow_p->fe.ipv4.remap_ip;
            tuple[1] /* dip */   = flow_p->fe.ipv4.sip;
            tuple[2] /* sport */ = flow_p->fe.ipv4.remap_port;
            tuple[3] /* dport */ = flow_p->fe.ipv4.sport;
        }

        tuple[4] /* proto */ = flow_p->fe.ipv4.proto;

        tstamp = flow_timestamp % CTFCTL_TIMESTAMP_NUM_STATES;
                                 /* This is reqd since the variable's
                                    value changes in the timer isr. So
                                    we maintain the same value for both
                                    flows */

        index = fc_find_pair_entry(flow_p, tuple);

	    //printk(KERN_INFO "\n%s: this idx = %u, pair index=%d\n\n",
        //       __func__, (flow_p->key.hash_index << 2) +  flow_p->key.bkt_index,
        //      index);

        /* Search tuple */
        if (index != -1) {
            /* An entry for other direction is present for this conection */

            flow_p->fe.ipv4.timestamp = tstamp;
            flow_p->pair_entry_index = (uint16_t)index;
            flow_p->state = FC_NEW_FLOW;

            /* Update other entry with the latest time stamp */
		    fptr = &fcache.etable[index];

            /* The 1st flow added can have a timestamp differing by 1. We can
             * either set up both timestamps in the flow table to have the same
             * value; or leave the timestamp for the 1st entry uchanged.
             * The 1st option appears to simplify the implementation a bit, for
             * now. Also refer comments in fc_check_and_evict_flows()
             */
             fptr->fe.ipv4.timestamp = tstamp;

             fptr->pair_entry_index = (flow_p->key.hash_index << 2) +
                                       flow_p->key.bkt_index;

	         //printk(KERN_INFO "\ndirn=%u, index=%d, tstamp=%u, pair=%u\n\n",
             //       dirn, index, tstamp, fptr->pair_entry_index);
        }
        else {
            /* This is the first entry for this conection */
            flow_p->fe.ipv4.timestamp = tstamp;
            flow_p->state = FC_NEW_FLOW;
        }
    }

    if (flow_p->l3_proto == ETH_P_IPV6) {
        uint32_t tuple[11];

        if (dirn == 0) {
            /* This is outbound entry, create inbound tuple */
            tuple[0] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.dip + 0));
            tuple[1] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.dip + 4));
            tuple[2] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.dip + 8));
            tuple[3] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.dip + 12));

            tuple[4] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 0));
            tuple[5] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 4));
            tuple[6] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 8));
            tuple[7] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 12));

            tuple[8] /* sport */ = flow_p->fe.ipv6.dport;
            tuple[9] /* dport */ = flow_p->fe.ipv6.remap_port;
        }
        else {
            tuple[0] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 0));
            tuple[1] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 4));
            tuple[2] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 8));
            tuple[3] /* sip */ = *((uint32_t *)(flow_p->fe.ipv6.remap_ip + 12));

            tuple[4] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.sip + 0));
            tuple[5] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.sip + 4));
            tuple[6] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.sip + 8));
            tuple[7] /* dip */ = *((uint32_t *)(flow_p->fe.ipv6.sip + 12));

            tuple[8] /* sport */ = flow_p->fe.ipv6.remap_port;
            tuple[9] /* dport */ = flow_p->fe.ipv6.sport;
        }

        tuple[10] /* proto */ = flow_p->fe.ipv6.proto;

        tstamp = flow_timestamp % CTFCTL_TIMESTAMP_NUM_STATES;
        index = fc_find_pair_entry(flow_p, tuple);

        if (index != -1) {
            /* An entry for other direction is present for this conection */

            flow_p->fe.ipv6.timestamp = tstamp;
            flow_p->pair_entry_index = (uint16_t)index;
            flow_p->state = FC_NEW_FLOW;

            /* Update other entry with the latest time stamp */
		    fptr = &fcache.etable[index];

            /* The 1st flow added can have a timestamp differing by 1. We can
             * either set up both timestamps in the flow table to have the same
             * value; or leave the timestamp for the 1st entry uchanged.
             * The 1st option appears to simplify the implementation a bit, for
             * now. Also refer comments in fc_check_and_evict_flows()
             */
             fptr->fe.ipv6.timestamp = tstamp;

             fptr->pair_entry_index = (flow_p->key.hash_index << 2) +
                                       flow_p->key.bkt_index;

	         //printk(KERN_INFO "\ndirn=%u, index=%d, tstamp=%u, pair=%u\n\n",
             //       dirn, index, tstamp, fptr->pair_entry_index);
        }
        else {

            /* This is the first entry for this conection */
            flow_p->fe.ipv6.timestamp = tstamp;
            flow_p->state = FC_NEW_FLOW;
        }
    }

    return ret;
}

/*
 * Function     : fc_learn
 * Description  : Learn a flow, invoked on TX.
 *                Allocates a flow_t, populates it and inserts into hash table.
 */
static int fc_learn(volatile napt_flow_t *flow_p, struct sk_buff *skb_p)
{
    int ret = CTF_SUCCESS;

	BUG_ON(flow_p == FLOW_NULL);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("%s: flow<0x%08x> skb <0x%08x>\n", __func__, 
               (unsigned int)flow_p, (unsigned int)skb_p);

    fc_set_conn_timestamp(flow_p);

	ret = fc_hash_insert(flow_p);   /* Insert into hash table */

    if (ret == CTF_SUCCESS) {
        flow_p->location = IN_FLOW_TABLE;
    }

	return ret;
}

/*
 * Function     : fc_evict
 * Description  : Evict a specified cached flow
 *                Unbinds flow with netfilter.
 *                Flushes the entry from hardware CTF, removes from hash table
 *                and fres the napt_flow_t object.
 */
void fc_evict(volatile napt_flow_t *flow_p)
{
	BUG_ON(flow_p == FLOW_NULL);

	/* Remove from hash table */
	fc_hash_delete(flow_p, flow_p->key.hash_index);

}

void fc_display_ipv6_addr(uint8_t *addr, char *str)
{
    printk(KERN_INFO "%s: %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X "
           "%02X%02X %02X%02X", str, addr[0], addr[1], addr[2], addr[3], addr[4],
           addr[5], addr[6], addr[7], addr[8], addr[9], addr[10], addr[11], 
           addr[12],addr[13],addr[14],addr[15]);
}

/* If entry is -1, dump the whole table */
void fc_dump_flow_cache(int entry, uint32_t hw_entries_only)
{
    int cache_index, found, num_entries, index;
	volatile napt_flow_t 	*flow_p;

	printk(KERN_INFO "\n%s: Dumping flow cache contents\n\n", __func__);

    if (entry >= FCACHE_MAX_ENTRIES) {

        printk (KERN_DEBUG "\n%s: index %d exceeds max allowed %d\n\n",
                __func__, entry, FCACHE_MAX_ENTRIES - 1);

        return;

    }
   
    found = 0;
    num_entries = (entry == -1) ? FCACHE_MAX_ENTRIES : 1;
    index = (entry == -1) ? 0 : entry;

	for (cache_index = 0; cache_index < num_entries;
         cache_index++, index++) {

		flow_p = &fcache.etable[index];

        if (flow_p == NULL) {
            continue;
        }

		if (flow_p->valid) {

            /* If we want to dump only entries present in the flow table,
             * check location field of each flow cache entry
             */
            if (hw_entries_only) {
                if (flow_p->location != IN_FLOW_TABLE) {
                    continue;
                }
            }

            found++;

			printk(KERN_INFO "__________ Entry %d __________\n\n",
                   cache_index);

            if (flow_p->l3_proto == ETH_P_IP) {

			printk(KERN_INFO "PROTO=%u, SIP = 0x%08X, SPORT= %u, DIP = 0x%08X,"
                   " DPORT = %u\n\n", flow_p->fe.ipv4.proto, flow_p->fe.ipv4.sip,
                   flow_p->fe.ipv4.sport, flow_p->fe.ipv4.dip, 
                   flow_p->fe.ipv4.dport);

			printk(KERN_INFO "ct=0x%08X, nfct=0x%08X\n\n",
                   (unsigned int)(flow_p->ct),
                   (unsigned int)(flow_p->nfct));

			printk(KERN_INFO "ipv4_entry = %u,  valid = %u, dirn = %u\n\n",
                   flow_p->fe.ipv4.ipv4_entry, flow_p->fe.ipv4.valid,
                   flow_p->fe.ipv4.direction);

			printk(KERN_INFO "remap_ip = 0x%08X, remap_port = %u\n\n",
                   flow_p->fe.ipv4.remap_ip, flow_p->fe.ipv4.remap_port);
                   
			printk(KERN_INFO "next_hop_idx = %u, remap_sa_idx = %u\n\n",
                   flow_p->fe.ipv4.next_hop_idx, flow_p->fe.ipv4.remap_sa_idx);

			printk(KERN_INFO "dest_dma = %u, action = %u, hits = %u,"
                   " timestamp = %u\n\n", flow_p->fe.ipv4.dest_dma,
                   flow_p->fe.ipv4.action, flow_p->fe.ipv4.hits,
                   flow_p->fe.ipv4.timestamp);

			printk(KERN_INFO "locn = %u, state = %u, pair_entry_index = %u\n\n",
                   flow_p->location, flow_p->state, flow_p->pair_entry_index);
                   
			printk(KERN_INFO "tag_oc = %u, tag_tc = %u\n\n",
                   flow_p->fe.ipv4.tag_oc, flow_p->fe.ipv4.tag_tc);
                   
			printk(KERN_INFO "l3_proto = %u (0x%X), l4_proto = %u\n\n",
                   flow_p->l3_proto, flow_p->l3_proto, flow_p->l4_proto);

			printk(KERN_INFO "KEY contents:::::\nhash_index = %u,"
                   " bkt_index = %u\n\n", flow_p->key.hash_index, 
                   flow_p->key.bkt_index);

			printk(KERN_INFO "l3_sip = 0x%08X, l3_dip = 0x%08X,"
                   " l3_proto=%u(0x%X)\n\n", flow_p->key.f.ipv4.l3_sip,
                   flow_p->key.f.ipv4.l3_dip, flow_p->key.f.ipv4.l3_proto,
                   flow_p->key.f.ipv4.l3_proto);

			printk(KERN_INFO "l4_sport = %u, l4_dport = %u\n\n",
                   flow_p->key.f.ipv4.l4_sport, flow_p->key.f.ipv4.l4_dport);

			printk(KERN_INFO "---\n\n");
            }

            if (flow_p->l3_proto == ETH_P_IPV6) {

            printk(KERN_INFO "\nPROTO=%u, ", flow_p->fe.ipv6.proto);
            fc_display_ipv6_addr(((napt_flow_t *)flow_p)->fe.ipv6.sip, "SIP");
            printk(KERN_INFO "SPORT=%u, ", flow_p->fe.ipv6.sport);
            fc_display_ipv6_addr(((napt_flow_t *)flow_p)->fe.ipv6.dip, "DIP");
            printk(KERN_INFO "DPORT=%u\n\n", flow_p->fe.ipv6.dport);

			printk(KERN_INFO "ct=0x%08X, nfct=0x%08X\n\n",
                   (unsigned int)(flow_p->ct),
                   (unsigned int)(flow_p->nfct));

			printk(KERN_INFO "ipv4_entry = %u,  valid = %u, dirn = %u\n\n",
                   flow_p->fe.ipv6.ipv4_entry, flow_p->fe.ipv6.valid,
                   flow_p->fe.ipv6.direction);

            fc_display_ipv6_addr(((napt_flow_t *)flow_p)->fe.ipv6.remap_ip, "remap_ip");
			printk(KERN_INFO "\nremap_port = %u\n\n", flow_p->fe.ipv6.remap_port);
                   
			printk(KERN_INFO "next_hop_idx = %u, remap_sa_idx = %u\n\n",
                   flow_p->fe.ipv6.next_hop_idx, flow_p->fe.ipv6.remap_sa_idx);

			printk(KERN_INFO "dest_dma = %u, action = %u, hits = %u,"
                   " timestamp = %u\n\n", flow_p->fe.ipv6.dest_dma,
                   flow_p->fe.ipv6.action, flow_p->fe.ipv6.hits,
                   flow_p->fe.ipv6.timestamp);

			printk(KERN_INFO "locn = %u, state = %u, pair_entry_index = %u\n\n",
                   flow_p->location, flow_p->state, flow_p->pair_entry_index);
                   
			printk(KERN_INFO "tag_oc = %u, tag_tc = %u\n\n",
                   flow_p->fe.ipv6.tag_oc, flow_p->fe.ipv6.tag_tc);
                   
			printk(KERN_INFO "l3_proto = %u (0x%X), l4_proto = %u\n\n",
                   flow_p->l3_proto, flow_p->l3_proto, flow_p->l4_proto);

			printk(KERN_INFO "KEY contents:::::\nhash_index = %u,"
                   " bkt_index = %u\n\n", flow_p->key.hash_index, 
                   flow_p->key.bkt_index);

            fc_display_ipv6_addr(((napt_flow_t *)flow_p)->key.f.ipv6.l3_sip, "l3_sip");

            fc_display_ipv6_addr(((napt_flow_t *)flow_p)->key.f.ipv6.l3_dip, "l3_dip");

            printk(KERN_INFO "l3_proto=%u(0x%X)\n\n",
                   flow_p->key.f.ipv6.l3_proto, flow_p->key.f.ipv6.l3_proto);

			printk(KERN_INFO "l4_sport = %u, l4_dport = %u\n\n",
                   flow_p->key.f.ipv6.l4_sport, flow_p->key.f.ipv6.l4_dport);

			printk(KERN_INFO "---\n\n");
            }

		}
    }

    printk (KERN_INFO "\n%s: Found %d entr%s \n",
            __func__, found, (found != 1) ? "ies" : "y");

    return;
}

/*
 * Function     : fc_destroy
 * Description  : Destroy the Flow Cache elements when conntrack is destroyed.
 */
void fc_destroy(struct nf_conn *ct_p)
{
//	int 		hash_index = 0;
	int 		cache_index = 0;
	volatile napt_flow_t 	*flow_p;
//	volatile napt_flow_t 	*next_p;
    int found;

    printk (KERN_DEBUG "\nEntered %s ct_p=0x%X\n", __func__, (uint32_t)ct_p);

    if (ct_p == NULL) {
        printk (KERN_DEBUG "\nct_p is NULL, exiting\n");
//        return;
    }

    found = 0;

	for (; cache_index < FCACHE_MAX_ENTRIES; cache_index++) {

		flow_p = &fcache.etable[cache_index];

        if (flow_p == NULL) {
            /* flow_p is NULL, check next entry */
            continue;
        }

		/* Check the head */
		if (flow_p->ct == ct_p) {

			printk(KERN_DEBUG "%s: ct match found. evicting entry %d \n", 
                   __func__, cache_index);

			//fc_evict(flow_p); 
            if (ctf_deactivate_flow(flow_p) == CTF_SUCCESS) {

		        memset((void *)flow_p, 0, sizeof(napt_flow_t));
                found++;

            }
            else {

                printk (KERN_DEBUG "\n%s: ctf_deactivate_flow failed\n",
                        __func__);

            }
		}
    }

    if (!found) {

        printk (KERN_DEBUG "\n%s: No flow cache entry matched\n", __func__);

    } 
    else { 

        printk (KERN_DEBUG "\n%s: %d entr%s matched\n",
                __func__, found, (found > 1) ? "ies" : "y");

    } 

}

/*
 * Function     : fc_refresh_nfconn
 * Description  : Refresh the associated nf_conn
 */
static void fc_refresh_nfconn(volatile napt_flow_t *flow_p)
{
	uint32_t refresh;

	BUG_ON(flow_p == FLOW_NULL);

	if (flow_p->l4_proto == IPPROTO_TCP)
		refresh = FCACHE_REFRESH_TCP_INTERVAL;
	else
		refresh = FCACHE_REFRESH_UDP_INTERVAL;

	printk(KERN_DEBUG "Refresh nf_conn<0x%08x> time<%u>",
		(int)flow_p, refresh);

}

static int fc_check_and_evict_flows(unsigned int ts)
{
    int ret = CTF_SUCCESS;
    int index;
    volatile napt_flow_t *fptr;
    volatile napt_flow_t *pair_entry = FLOW_NULL; /* entry associated with fptr */
    uint16_t num_entries_evicted;
    unsigned int ts_this_entry = 0;
    unsigned int ts_pair_entry = 0;
    unsigned int aged = 0;
    unsigned int x = 0;
    unsigned int scanned[FCACHE_MAX_ENTRIES / FC_FLOWS_PER_LINE];
    
    memset((void *)&scanned, 0, sizeof(scanned));
    num_entries_evicted = 0;
    index = 0;
    x = 7 + (ts % CTFCTL_TIMESTAMP_NUM_STATES);

    spin_lock(&ctf_lock);
    for (index = 0; index < FCACHE_MAX_ENTRIES; index++) {

		fptr = &fcache.etable[index];
        aged = 0;

        if (fptr->valid) {

            /* If one or both flows were added in this polling cycle,
             * do not check for aging in this cycle. Wait for subsequent 
             * iteration for fptr to take on pair_entry's value, and mark it
             * 'not new'
             */
            if (fptr->state == FC_NEW_FLOW) {
            
                fptr->state = FC_NOT_NEW_FLOW;

                continue;
            }

            /* Flow occupying only the flow cache, or unidirectional.
             * Timestamp recorded in fc_receive. 
             * For hw valid entries, set_conn_timestamp will use the latest
             * timestamp value
             */
            if (fptr->l3_proto == ETH_P_IP) {
                ts_this_entry = fptr->fe.ipv4.timestamp;
            }
            else {
                ts_this_entry = fptr->fe.ipv6.timestamp;
            }

            if (fptr->location == IN_FLOW_TABLE) {

                /* If the flow hasn't aged, read the timestamp from hw */
                if (fptr->state != FC_AGED_FLOW) {
                    /* Get timestamp for this flow of a connection */
                    ctf_get_flow_entry_timestamp(fptr->key.hash_index, 
                               fptr->key.bkt_index, &ts_this_entry);
                }

            }

            /* Don't return error if the pair entry is invalid, so that other
             * entries are processed. The pair entry can get added latter on,
             * after connection establishment is complete
             */
            if (fptr->pair_entry_index >= FCACHE_MAX_ENTRIES) {

                //printk(KERN_DEBUG "%s: INFO: for idx %u:%u, peer not (yet)"
                 //      " added",
                  //     __func__, index, fptr->pair_entry_index);

                /* Info: in fc_receive we set flow tstamp to be 1 less, so a 
                 * flow gets polled for up to 8 intervals, giving sufficient
                 * time for a bidirectional connection establishment. Also read
                 * the comment below
                 */
                //printk("\n:::::: %u: x(now)=%u, ts_this_entry=%u, locn=%u\n",
                //       index, x, ts_this_entry, fptr->location);
                if ((x - ts_this_entry) == 7)  {

                   /* Note this case is hit by uidirectional flows as well as
                    * the packets which were seen only once (so a hw flow entry
                    * was not created), or for packets hose flows were not added
                    * to the hw flow table (e.g., unsupported protocols)
                    */

                    //printk(KERN_DEBUG "\nPair flow entry not present,"
                    //       " deleting flow %u\n", index);

                    /* If flow was added to the flow table, remove it first */
                    if (fptr->location == IN_FLOW_TABLE) {

                        if ((ret = ctf_deactivate_flow(fptr)) != CTF_SUCCESS) {

                            printk (KERN_DEBUG "\n%s: ctf_deactivate_flow"
                                    " failed, for entry %u, return val=%d",
                                    __func__, index, ret);

                            /* We do not clear the flow cache in this case, 
                             * so that in the next attempt the deletion can be
                             * tried again, and the driver's copy stays intact
                             * for making other decisions
                             */
                            continue;
                        }
                    }

		            memset((void *)fptr, 0, sizeof(napt_flow_t));
                    num_entries_evicted++;

                }

                continue;

            }
 
            pair_entry = &fcache.etable[fptr->pair_entry_index];

            /* If the pair entry is not valid (unlikely), it can happen
             * that the 2nd entry has not yet been added. We can delete this
             * entry in this interval, or wait until the next interval; for now
             * we will continue.
             * We could also check hash/bucket indx, but since these are 
             * defined as bit fields, they will always be 'in range'.
             */
            if (!(pair_entry->valid)) {

                printk(KERN_DEBUG "%s: for idx %u, pair idx %u not valid",
                       __func__, index, fptr->pair_entry_index);
#ifndef CONSISTENCY_CHK
                if ((x - ts_this_entry) == 7)  {
                    if (fptr->location == IN_FLOW_TABLE) {
                        if ((ret = ctf_deactivate_flow(fptr)) != CTF_SUCCESS) {

                            printk (KERN_DEBUG "\n%s: ctf_deactivate_flow"
                                    " failed, for entry %u, return val=%d",
                                    __func__, index, ret);

                            /* We do not clear the flow cache in this case, 
                             * so that in the next attempt the deletion can be
                             * tried again, and the driver's copy stays intact
                             * for making other decisions
                             */
                            continue;
                        }
                    }

		            memset((void *)fptr, 0, sizeof(napt_flow_t));
                    num_entries_evicted++;
                }
#endif /* CONSISTENCY_CHK */

                continue;
            }



            if (pair_entry->state == FC_NEW_FLOW) {
            
                pair_entry->state = FC_NOT_NEW_FLOW;

                continue;
            }

            /* If this flow was scanned by its peer, don't check it again */
            if (scanned[index >> FC_NUM_LINE_BITS] & 
                       (1 << (index & FC_LINE_MASK))) {

                continue;
            }

#ifndef CONSISTENCY_CHK
            /* If there is protocol mismatch in the entry and its pair, we 
             * will not track those flows for aging, so that it will be useful
             * for debugging. User of FA driver should correct the inconsistency
             */
            if (fptr->l3_proto != pair_entry->l3_proto) {
                printk (KERN_DEBUG "\n%s: Warning: protocol field mismatch in"
                        " entry %u and its pair %u.\n"
                        " Entry proto: %u, pair entry proto: %u\n",
                        __func__, index, fptr->pair_entry_index,
                        fptr->l3_proto, pair_entry->l3_proto);

                continue;
            }
#endif /* CONSISTENCY_CHK */

            if (pair_entry->l3_proto == ETH_P_IP) {
                ts_pair_entry = pair_entry->fe.ipv4.timestamp;
            }
            else {
                ts_pair_entry = pair_entry->fe.ipv6.timestamp;
            }

            if (pair_entry->location == IN_FLOW_TABLE) {
            
                /* If the flow hasn't aged, read timestamp from hw */
                if (pair_entry->state != FC_AGED_FLOW) {
                    /* Get timestamp for the other flow of a connection */
                    ctf_get_flow_entry_timestamp(pair_entry->key.hash_index, 
                           pair_entry->key.bkt_index, &ts_pair_entry);
                }
            }
            
            /* We compare both timestamps on a connection, so that we do not
             * end up evicting a connection sending traffic only in one
             * direction. If both entries have aged, only then we will remove
             * the entries
             */

            /* If the entry timestamps were not updated by hardware, the
             * difference between the present tick and the timestamp value
             * within flow entry will become 7. In that case, we will delete
             * both entries. This covers the case where both flows aged at the
             * same time. The second condition of 'if' below is for the case
             * where one of the flows aged first, and then we waited for the 
             * other flow to age
             */

             printk(KERN_DEBUG "\n=== %u: x(now)=%u, ts_this_entry=%u,"
                    " ts_pair_entry=%u\n", index, x, ts_this_entry, 
                    ts_pair_entry);

            if ((((x - ts_this_entry) == 7) && ((x - ts_pair_entry) == 7)) ||
                ((fptr->state == FC_AGED_FLOW) && 
                 (pair_entry->state == FC_AGED_FLOW))) {

                if ((ret = ctf_deactivate_flow(fptr)) == CTF_SUCCESS) {

                    if ((ret = ctf_deactivate_flow(pair_entry)) ==
                         CTF_SUCCESS) {
		                memset((void *)fptr, 0, sizeof(napt_flow_t));
		                memset((void *)pair_entry, 0, sizeof(napt_flow_t));
                        num_entries_evicted += 2;
                    }
                    else {

                        printk (KERN_DEBUG "\n%s: ctf_deactivate_flow failed"
                                " for entry %u (hash=%u, bkt=%u)\n", __func__,
                                (pair_entry->key.hash_index << 2) + 
                                pair_entry->key.bkt_index, 
                                pair_entry->key.hash_index, 
                                pair_entry->key.bkt_index); 

                        /* In case of failure(unlikely), we do not clear flow
                         * cache; flow may remain longer in the table
                         */
                        continue;
                    }
                }
                else {

                    printk (KERN_DEBUG "\n%s: ctf_deactivate_flow failed for"
                            "entry %u (hash=%u, bkt=%u)\n", __func__,
                            (fptr->key.hash_index << 2) + fptr->key.bkt_index,
                            fptr->key.hash_index, fptr->key.bkt_index); 

                        /* In case of failure(unlikely), we do not clear flow
                         * cache; flow may remain longer in the table
                         */
                        continue;
                }
            }
            else {
                /* Check if this entry has aged. If yes, set its timestamp same
                 * as that of its peer, so that they will age at the same time.
                 * Don't change the tstamp later, since this flow is marked 
                 * for deletion
                 */
                if ((fptr->state != FC_AGED_FLOW) && 
                    ((x - ts_this_entry) == 7) && 
                    ((x - ts_pair_entry) != 7)) {

                    fptr->state = FC_AGED_FLOW;

                    if (fptr->l3_proto == ETH_P_IP) {
                        /* ipv4 */
                        fptr->fe.ipv4.timestamp = ts_pair_entry;
                    }
                    else {
                        /* ipv6 */
                        fptr->fe.ipv6.timestamp = ts_pair_entry;
                    }

                }
                else {
                    /* Check if peer entry has aged. Also read above comment */
                    if ((pair_entry->state != FC_AGED_FLOW) &&
                        ((x - ts_this_entry) != 7) && 
                        ((x - ts_pair_entry) == 7)) {

                        pair_entry->state = FC_AGED_FLOW;

                        if (pair_entry->l3_proto == ETH_P_IP) {
                            /* ipv4 */
                            pair_entry->fe.ipv4.timestamp = ts_this_entry;
                        }
                        else {
                            /* ipv6 */
                            pair_entry->fe.ipv6.timestamp = ts_this_entry;
                        }
                    }
                }

                printk(KERN_DEBUG "%s: idx %u state=%u, pair idx %u state = %u",
                       __func__, index, fptr->state, fptr->pair_entry_index,
                       pair_entry->state);
            }

            /* Update entries of a connection as scanned, so that scan of
             * the next entry is not required
             */
            scanned[index >> FC_NUM_LINE_BITS] |= 
                             (1 << (index & FC_LINE_MASK));

            scanned[fptr->pair_entry_index >> FC_NUM_LINE_BITS] |=  
                          (1 << (fptr->pair_entry_index & FC_LINE_MASK));
        }
    }
    spin_unlock(&ctf_lock);

	printk(KERN_DEBUG "%s: %u(%u): num_entries_evicted=%u\n",
           __func__, ts, x,  num_entries_evicted);
    
    return ret;
}

/*
 * Function     : fc_timer
 * Description  : Periodic timer that flushes entries that have not experienced
 *                any software or hardware hits. Check the hit count and
 *		  refresh flows with non-zero hits in hardware or software.
 */
static void fc_timer(unsigned long data)
{
    unsigned int timestamp = 0;


    /* 3-bit timestamp */
    timestamp = flow_timestamp % CTFCTL_TIMESTAMP_NUM_STATES;

    if (is_ctf_initialized()) {
        /* Check if in _this_ interval any flow aged */
        fc_check_and_evict_flows(timestamp);

        flow_timestamp++;

        ctf_update_timestamp(timestamp);
    }

     //if (is_ctf_initialized())
     //   ctf_get_flow_entry_timestamp((flow_timestamp % 256) << 2, 0, &tstamp); /* For testing only */

	//printk(KERN_DEBUG "=====  %s, %u:%u, tstamp=%u\n", __func__, (flow_timestamp%256)<<2, 0, tstamp);

	// printk(KERN_DEBUG "===== In %s, ctr=%u\n", __func__, flow_timestamp % CTFCTL_TIMESTAMP_NUM_STATES);

	fcache.timer.expires = jiffies +  FCACHE_REFRESH_INTERVAL * HZ;
	add_timer( &fcache.timer );
}

/*
 * Function     : fc_nfct
 * Description  : This routine creates a reference to netfilter for this
 *                flow.
 */
void fc_nfct(struct sk_buff *skb_p, struct nf_conn *ct)
{
	volatile napt_flow_t     *flow_p;

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
        printk("\n -------- %s: ct=0x%X, hash ix=%u, bkt_ix=%u\n\n",
               __func__, (unsigned int)ct, skb_p->napt_hash_ix, 
               skb_p->napt_bkt_ix);

    if ((skb_p->napt_hash_ix == BCM_CTF_INVALID_IDX_VAL) ||
        (skb_p->napt_bkt_ix == BCM_CTF_INVALID_IDX_VAL)) {

        return; /* reference will be added in the receive direction */

    }

    if ((skb_p->napt_hash_ix > FCACHE_HTABLE_SIZE) ||
        (skb_p->napt_bkt_ix >= FCACHE_MAX_BKT_ENTRIES)) {

        return;

    }

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("fc_nfct: Adding nf_conn <0x%08x> for skb <0x%08x> to flow\n",
		       (unsigned int)ct, (unsigned int)skb_p);
	/* Lookup flow */
	flow_p = fc_lookup(skb_p->napt_hash_ix, skb_p->napt_bkt_ix);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("fc_nfct: Looked up flow <0x%08x> with hash <%d> bkt <%d>\n",
		       (unsigned int)flow_p, skb_p->napt_hash_ix, skb_p->napt_bkt_ix);

	if (flow_p != FLOW_NULL) {
        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("fc_nfct: Flow found <0x%08x> adding ct <0x%08x>\n",
			       (unsigned int)flow_p, (unsigned int)ct);

		flow_p->ct	= ct;
		flow_p->nfct	= skb_p->nfct;
		return;
	}

	printk("fc_nfct: Invalid Flow <0x%x>\n", (unsigned int)flow_p);
}

/* Refer Tables 21 to 26 for frame formats */
int get_frame_info(unsigned char *p, unsigned int *et, 
                   unsigned int *vid1, unsigned int *vid2,
                   unsigned int *frm_data)
{
    uint32_t val1, val2, val3, val4;

    val1 = ntohl(*((uint32_t *)(&p[16])));
    val2 = ntohl(*((uint32_t *)(&p[20])));
    val3 = ntohl(*((uint32_t *)(&p[24])));
    val4 = ntohl(*((uint32_t *)(&p[28])));

    if (BCM_FA_IS_VLAN_TAGID(val1) && BCM_FA_IS_VLAN_TAGID(val2) &&
        ((val3 & BCM_FA_SNAP_ID1_MASK) == BCM_FA_SNAP_ID1) && 
        (val4 == BCM_FA_SNAP_ID2)) {

        val4 = ntohl(*((uint16_t *)(&p[32]))); /* reusing val4 */

        *et = (val4 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
        *vid1 = val1 & BCM_FA_VLAN_ID_MASK; /* OTPID */ 
        *vid2 = val2 & BCM_FA_VLAN_ID_MASK; /* ITPID */
        *frm_data = 34;
   
        if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
            printk(KERN_DEBUG "\nFrame with eth2+2vlantags+snap\n");
        goto frminfo_exit;
    }

    if (BCM_FA_IS_VLAN_TAGID(val1) && BCM_FA_IS_VLAN_TAGID(val2)) {
        *et = (val3 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
        *vid1 = val1 & BCM_FA_VLAN_ID_MASK; /* OVLANID */ 
        *vid2 = val2 & BCM_FA_VLAN_ID_MASK; /* IVLANID */
        *frm_data = 26;

        if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
            printk(KERN_DEBUG "\nFrame with eth2+2vlantags \n");
        goto frminfo_exit;
    }

    if (BCM_FA_IS_VLAN_TAGID(val1) &&
        ((val2 & BCM_FA_SNAP_ID1_MASK) == BCM_FA_SNAP_ID1) &&
        (val3 == BCM_FA_SNAP_ID2)) {

        *et = (val4 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
        *vid1 = val1 & BCM_FA_VLAN_ID_MASK; /* VLANID */ 
        *vid2 = 0;
        *frm_data = 30;

        if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
            printk(KERN_DEBUG "\nFrame with eth2+1vlantag+snap\n");
        goto frminfo_exit;
    }

    if (BCM_FA_IS_VLAN_TAGID(val1)) {
        *et = (val2 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
        *vid1 = val1 & BCM_FA_VLAN_ID_MASK; /* VLANID */ 
        *vid2 = 0;
        *frm_data = 22;

        if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
            printk(KERN_DEBUG "\nFrame with eth2+1vlantag\n");
        goto frminfo_exit;
    }

    if (((val1 & BCM_FA_SNAP_ID1_MASK) == BCM_FA_SNAP_ID1) && 
        (val2 == BCM_FA_SNAP_ID2)) {

        *et = (val3 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
        *vid1 = 0;
        *vid2 = 0;
        *frm_data = 26;

        if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL) 
            printk(KERN_DEBUG "\nUntagged frame with eth2+snap\n");
        goto frminfo_exit;
    }

    *et = (val1 & BCM_FA_TYPE_MASK) >> BCM_FA_TYPE_SHIFT;
    *vid1 = 0;
    *vid2 = 0;
    *frm_data = 18;

    if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
        printk(KERN_DEBUG "\nUntagged eth2 frame\n");

frminfo_exit:
    if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
        printk(KERN_DEBUG "\net=0x%04X(%u), outervlanid=0x%04X(%u), "
               "innervlanid=0x%04X(%u), frm_data_locn=%u\n\n",
               *et, *et, *vid1, *vid1, *vid2, *vid2, *frm_data);

    return(0);
}

/*
 * Function     : fc_receive
 * Description  : This routine must create a flow in our hash table
 *                and return the packet back to the caller for normal
 *		  routing.
 */
int fc_receive(struct sk_buff *skb_p)
{
	volatile napt_flow_t		*flow_p;
	struct bcmhdr		brcm_hdr;
	uint32_t		hash_index;
	uint32_t		bkt_index;
	uint32_t		vlan_tag, vlan_tag_next, data_loc;
	uint32_t		et_type;
	struct iphdr		*iph;
	struct ipv6hdr		*ipv6h;
	struct tcphdr		*th;
    int ret;

	BUG_ON(skb_p == (struct sk_buff*)NULL);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	    printk("fc_receive: skb<0x%08x> len<%d>\n",
		       (unsigned int)skb_p, (unsigned int)skb_p->len);

	    printk("Received Packet... <===========\n");

	    fc_dump_skb(skb_p, 0);

	    printk("==============================\n");
    }

	brcm_hdr.word = ntohl(*((uint32_t *)skb_p->data));

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("BRCM TAG 0x%08x\n", brcm_hdr.word);


    ret = get_frame_info(skb_p->data, &et_type, &vlan_tag, &vlan_tag_next, &data_loc);

    if (ctf_dbg_log_level >= CTF_DBG_LOW_LVL)
        printk(KERN_DEBUG "\nRcvd ET <0x%04x>, vlan1=%u, vlan2=%u,"
               " data_loc=%u\n",
               et_type, vlan_tag, vlan_tag_next, data_loc);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	        printk("BRCM TAG\n"
		        "Pid : %x "
		        "TC  : %x "
		        "Hdr Chk : %x "
		        "Flow ID : %x "
		        "Bkt ID  : %x "
		        "Bkts Full : %x "
		        "Opcode : %x\n",
		        brcm_hdr.oc1.src_pid,
		        brcm_hdr.oc1.tc,
		        brcm_hdr.oc1.hdr_chk_result,
		        brcm_hdr.oc1.napt_flow_id,
		        brcm_hdr.oc1.bkt_id,
		        brcm_hdr.oc1.all_bkts_full,
		        brcm_hdr.oc1.op_code);

	        printk("DA %02x:%02x:%02x:%02x:%02x:%02x "
		        "SA  %02x:%02x:%02x:%02x:%02x:%02x "
		        " Ether Type: %04x\n",
		        skb_p->data[4],
		        skb_p->data[5],
		        skb_p->data[6],
		        skb_p->data[7],
		        skb_p->data[8],
		        skb_p->data[9],
		        skb_p->data[10],
		        skb_p->data[11],
		        skb_p->data[12],
		        skb_p->data[13],
		        skb_p->data[14],
        		skb_p->data[15], et_type);
    }

	/* If the opcode is 0, nothing to do return */

	/* Extract the hash-tuple */
	if (et_type == ETH_P_IP) {

	    iph = (struct iphdr *)&(skb_p->data[data_loc]);

		if ((iph->protocol != IPPROTO_TCP) &&
		    (iph->protocol != IPPROTO_UDP)) {
			//printk("IPv4: Not TCP/UDP Packet <0x%02x>\n",
			//	iph->protocol);
			return PKT_NORM;
		}

        /* Note this is tcp header. This code works because both tcp, udp
         * headers have src/dst ports in the first word. We do not use any
         * other header field. Based on proto bit above we can select
         * udp header
         */
		th = (struct tcphdr *)((unsigned int)iph + (iph->ihl * sizeof(int)));
        

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("fc_recv: IPv4 Header Len %d iph 0x%08x th 0x%08x\n",
			       iph->ihl, (unsigned int)iph, (unsigned int)th);

	} else if (et_type == ETH_P_IPV6) {

		/* Check transport header for IPv6 */
		ipv6h = (struct ipv6hdr *)&(skb_p->data[data_loc]);

		if ((ipv6h->nexthdr != NEXTHDR_TCP) &&
		    (ipv6h->nexthdr != NEXTHDR_UDP)) {
			  //printk("%s: IPv6: Not TCP/UDP Packet <%u (0x%X)>\n",
			 // 	     __func__, ipv6h->nexthdr, ipv6h->nexthdr);
			return PKT_NORM;

		}

		/* Check next header */

        /* Note this is tcp header. This code works because both tcp, udp
         * headers have src/dst ports in the first word. We do not use any
         * other header field. Based on proto bit above we can select
         * udp header
         */
		th = (struct tcphdr *)&(skb_p->data[data_loc + 40]); /* IPv6 hdr size is
                                                                fixed */
		//th = (struct tcphdr *)(ipv6h + ipv6h->nexthdr);
	} else {
		//printk("Not a IP Packet\n");
		return PKT_NORM;
	}

	/* Extract the hash index and the bucket index */
	hash_index = brcm_hdr.oc1.napt_flow_id;
	bkt_index  = brcm_hdr.oc1.bkt_id;

	if (((hash_index * 4) + bkt_index) > FCACHE_MAX_ENTRIES) {
		printk("Invalid hash entry <%d><%d>\n",
			hash_index, bkt_index);
		return PKT_NORM;
	}

	/* Allocate a hash table entry. */

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("%s: Allocating flow...\n", __func__);

	flow_p = fc_alloc(brcm_hdr.oc1.napt_flow_id,  brcm_hdr.oc1.bkt_id);
	if (!flow_p) {
		printk(KERN_DEBUG "Flow cache entry already allocated...\n");
		return PKT_NORM;
	}

    /* In case of a blast of traffic (e.g., udp pkts), the 1st packet will 
     * result in populating of flow entry. So we should avoid other
     * packets of the same flow modify flow cache entry and/or hw table entry
     */
    if (flow_p->location == IN_FLOW_TABLE) {
		// printk(KERN_DEBUG "Flow <hash:%u, bkt:%u> already in hw flow table...\n", hash_index, bkt_index);
		return PKT_NORM;
    }

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("%s: Allocated flow at %u...\n", __func__,
               (brcm_hdr.oc1.napt_flow_id * 4) + brcm_hdr.oc1.bkt_id);

	/* Fill L2 and L3 contents of flow table */
	switch (et_type) {
	case ETH_P_IP:
        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		    printk("IPv4 Flow <0x%08x><%d:%d>: Src <0x%08x> Dest <0x%08x> Proto <%d>"
			    " SPort <0x%04x> DPort <0x%04x> dir %s\n", (unsigned int)flow_p,
			    brcm_hdr.oc1.napt_flow_id, brcm_hdr.oc1.bkt_id,
			    ntohl(iph->saddr), ntohl(iph->daddr),
				iph->protocol, ntohs(th->source), ntohs(th->dest),
			    ((vlan_tag & 0x00000FFF) == 2) ? "Inbound" :
			    "Outbound");

		/* IPv4 Packet */
		flow_p->fe.ipv4.sip		= ntohl(iph->saddr);
		flow_p->fe.ipv4.dip		= ntohl(iph->daddr);
		if (iph->protocol == IPPROTO_TCP) {
			flow_p->fe.ipv4.proto		= 1;
		} else if (iph->protocol == IPPROTO_UDP){
			flow_p->fe.ipv4.proto		= 0;
		} else {
			/*
			 * Not sure what to do in case of other
			 * L4 protocols.
			 */
			flow_p->fe.ipv4.proto		= 0;
		}
		flow_p->fe.ipv4.sport		= ntohs(th->source);
		flow_p->fe.ipv4.dport		= ntohs(th->dest);
		flow_p->fe.ipv4.ipv4_entry	= 1;
		flow_p->fe.ipv4.tag_oc		= 0;
		flow_p->fe.ipv4.tag_tc		= 0;
		flow_p->fe.ipv4.hits		= 0;
		flow_p->fe.ipv4.dest_dma	= 0; /* To Switch ?? */
		flow_p->fe.ipv4.action		= 1;
        /* Note we set tstamp to a value 1 less, so that a flow will be polled
         * for aging during all the 8 intervals
         */
        if (flow_timestamp) {
            flow_p->fe.ipv4.timestamp = (flow_timestamp - 1) % 
                                        CTFCTL_TIMESTAMP_NUM_STATES;
        }
        else {
            /* Very first flow, immediately after system boot up */
            flow_p->fe.ipv4.timestamp = CTFCTL_TIMESTAMP_NUM_STATES - 1;
        }

		if ((vlan_tag & 0x00000FFF) == 2) {
			flow_p->fe.ipv4.direction = 1; /* Inbound */
		} else {
			flow_p->fe.ipv4.direction = 0; /* Outbound */
		}

		flow_p->key.f.ipv4.l3_sip	= ntohl(iph->saddr);
		flow_p->key.f.ipv4.l3_dip	= ntohl(iph->daddr);
		flow_p->key.f.ipv4.l3_proto	= iph->protocol;
		flow_p->key.f.ipv4.l4_sport	= ntohs(th->source);
		flow_p->key.f.ipv4.l4_dport	= ntohs(th->dest);
		flow_p->key.hash_index		= hash_index;
		flow_p->key.bkt_index		= bkt_index;
		flow_p->l3_proto		= ETH_P_IP;
		flow_p->l4_proto		= iph->protocol;
		flow_p->pair_entry_index    = FCACHE_INVAL_INDX;

        if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
    		printk("fc_receive: IPv4: Flow Created...\n");

		break;

	case ETH_P_IPV6:
        /* code chk : check if byte order is correct. Now its in 
         * nw byte order(big endian)
         */
		memcpy(((napt_flow_t *)flow_p)->fe.ipv6.sip, (void *)&(ipv6h->saddr),
			sizeof(struct  in6_addr));
		memcpy(((napt_flow_t *)flow_p)->fe.ipv6.dip, (void *)&(ipv6h->daddr),
			sizeof(struct  in6_addr));

		if (ipv6h->nexthdr == IPPROTO_TCP) {
    		flow_p->fe.ipv6.proto		= 1;
		} else {
    		flow_p->fe.ipv6.proto		= 0;
        }

		flow_p->fe.ipv6.sport		= ntohs(th->source);
		flow_p->fe.ipv6.dport		= ntohs(th->dest);
		flow_p->fe.ipv6.ipv4_entry	= 0;
		flow_p->fe.ipv6.tag_oc		= 0;
		flow_p->fe.ipv6.tag_tc		= 0;
		flow_p->fe.ipv6.hits		= 0;
		flow_p->fe.ipv6.dest_dma	= 0; /* To Switch ?? */
		flow_p->fe.ipv6.action		= 1;
        /* Note we set tstamp to a value 1 less, so that a flow will be polled
         * for aging during all the 8 intervals
         */
        if (flow_timestamp) {
            flow_p->fe.ipv6.timestamp = (flow_timestamp - 1) % 
                                        CTFCTL_TIMESTAMP_NUM_STATES;
        }
        else {
            /* Very first flow, immediately after system boot up */
            flow_p->fe.ipv6.timestamp = CTFCTL_TIMESTAMP_NUM_STATES - 1;
        }

		if ((vlan_tag & 0x00000FFF) == 2) {
			flow_p->fe.ipv6.direction = 1; /* Inbound */
		} else {
			flow_p->fe.ipv6.direction = 0; /* Outbound */
		}

        /* code chk : chk endianess of the addresses below */
		memcpy(((napt_flow_t *)flow_p)->key.f.ipv6.l3_sip, (void *)&(ipv6h->saddr),
			sizeof(struct  in6_addr));
		memcpy(((napt_flow_t *)flow_p)->key.f.ipv6.l3_dip, (void *)&(ipv6h->daddr),
			sizeof(struct  in6_addr));
		flow_p->key.f.ipv6.l3_proto	= ipv6h->nexthdr;
		flow_p->key.f.ipv6.l4_sport	= ntohs(th->source);
		flow_p->key.f.ipv6.l4_dport	= ntohs(th->dest);
		flow_p->key.hash_index		= hash_index;
		flow_p->key.bkt_index		= bkt_index;
		flow_p->l3_proto		= ETH_P_IPV6;
		flow_p->l4_proto		= ipv6h->nexthdr;
		flow_p->pair_entry_index    = FCACHE_INVAL_INDX;

		break;

	default:
		printk(KERN_ERR "Unknown protocol...\n");
	}

	skb_p->napt_hash_ix	= hash_index;
	skb_p->napt_bkt_ix	= bkt_index;

	return PKT_NORM;
}

/*
 * Function     : fc_transmit
 * Description  : Given a packet quickly detect the IP tuple, lookup hash table
 *                and activate the flow in the hardware.
 */
int fc_transmit(struct sk_buff * skb_p)
{
	volatile napt_flow_t		*flow_p;
	struct bcmhdr		brcm_hdr;
	uint32_t		vlan_tag, vlan_tag_next, data_loc;
	uint32_t		et_type, protocol;
	struct iphdr		*iph;
	struct ipv6hdr		*ipv6h;
	struct tcphdr		*th;
	uint8_t			pt_ix;
	uint8_t			sa_ix;
    static int related = 0; /* Not related, by default */
    int ret;
	struct ethhdr		*mac;

	BUG_ON(skb_p == (struct sk_buff*)NULL);

    if (!spin_trylock(&ctf_lock)) {
        /* Lock is in use by rcv thread, return. Lock will be available for
         * future packet processing
         */
        return PKT_NORM;
    }

    //spin_lock(&ctf_lock);
	brcm_hdr.word = ntohl(*((uint32_t *)skb_p->data));

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	    printk("fc_transmit: skb<0x%08x> len<%d> nfct<0x%08x>\n",
		       (int)skb_p, (int)skb_p->len,(int)skb_p->nfct);

	    printk("Transmitting Packet.... ===============>\n");
	    fc_dump_skb(skb_p, 0);
	    printk("===================================\n");
    }

	mac = (struct ethhdr *)&skb_p->data[4];

    ret = get_frame_info(skb_p->data, &et_type, &vlan_tag, &vlan_tag_next, &data_loc);

	/* Determine L4 Header if TCP/UDP */
	if (et_type == ETH_P_IP) {

	    iph = (struct iphdr *)&(skb_p->data[data_loc]);

        if ((iph->protocol != IPPROTO_TCP) &&
            (iph->protocol != IPPROTO_UDP)) {
           //printk("IPv4: Not TCP/UDP Packet <0x%02x>\n",
           //      iph->protocol);
           goto done;
       }

       protocol = (iph->protocol == IPPROTO_TCP) ? 1 : 0;

	   th = (struct tcphdr *)((unsigned int)iph + (iph->ihl * sizeof(int)));
	}
    else if (et_type == ETH_P_IPV6){

		/* Check transport header for IPv6 */
		ipv6h = (struct ipv6hdr *)&(skb_p->data[data_loc]);

		if ((ipv6h->nexthdr != NEXTHDR_TCP) &&
		    (ipv6h->nexthdr != NEXTHDR_UDP)) {

		    //printk("%s: IPv6: Not TCP/UDP Packet <%u (0x%X)>\n",
			//  	     __func__, ipv6h->nexthdr, ipv6h->nexthdr);

            goto done;

		}

        protocol = (ipv6h->nexthdr == NEXTHDR_TCP) ? 1 : 0;

		/* Check next header */
        /* Note this is tcp header. This code works because both tcp, udp
         * headers have src/dst ports in the first word. We do not use any
         * other header field. Based on proto bit above we can select
         * udp header
         */
		th = (struct tcphdr *)&(skb_p->data[data_loc + 40]); /* IPv6 hdr size is
                                                                fixed */
		//th = (struct tcphdr *)(ipv6h + ipv6h->nexthdr);
	} else {
		//printk("Unknown L3 protocol 0x%X (%u)\n", et_type, et_type);
        goto done;
	}

    if (ctf_dbg_log_level > CTF_DBG_LOW_LVL)
       printk(KERN_DEBUG "vltag=%u, et_type=0x%X, vltag_next=%u, "
              "protocol=%s\n", vlan_tag, et_type, vlan_tag_next,
              protocol ? "tcp" : "udp");

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Looking up flow hash<%d> bkt<%d>\n",
		       skb_p->napt_hash_ix, skb_p->napt_bkt_ix);

	if (skb_p->nfct != NULL) {
		//flow_p = fc_lookup_nf_conn(skb_p);
        if ((skb_p->napt_hash_ix == BCM_CTF_INVALID_IDX_VAL) ||
            (skb_p->napt_bkt_ix == BCM_CTF_INVALID_IDX_VAL)) {

            goto done;

        }

		flow_p = fc_lookup(skb_p->napt_hash_ix, skb_p->napt_bkt_ix);
	} else {
        goto done;
	}
	BUG_ON(flow_p == FLOW_NULL);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
	    printk("Tx Path: Flow Located 0x%08x\n", (unsigned int)flow_p);

	/* validate nf_conn status if packet is tracked by netfilter */
	if ((flow_p->ct != (struct nf_conn *)NULL)) {
		struct nf_conn_help *help;

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
		printk("Flow hash conntrack <0x%08x>\n", (unsigned int)flow_p->ct);

//		printk("====== fc_transmit: ct <0x%08X>: ct->status=0x%08X\n",
//			       (unsigned int)(flow_p->ct), flow_p->ct->status);

		/* Netfilter conntrack is confirmed */
		if (!test_bit(IPS_CONFIRMED_BIT, &flow_p->ct->status)) {

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			    printk("ABORT: NOT CONFIRMED flow<0x%08x> ct<0x%08x>\n",
			           (int)flow_p, (int)flow_p->ct);

            goto done;
		}

		//printk(KERN_DEBUG "%s: ctinfo=%u\n", __func__, skb_p->nfctinfo);

        if ((protocol) && 
            (skb_p->nfctinfo == IP_CT_ESTABLISHED + IP_CT_RELATED)) {
            related = 1; /* For protocols like FTP, we allow to create flow
                            entries  for data connection */
        }

        /* Add delay to throttle the connection establishment and control
         * traffic, so that the stack gets time to update connection state,
         * and flow entries are populated. After connection is fully 
         * established, we do not delay future packets
         */
        //if (related)
        /* TCP protocol */
        if (protocol) {

            udelay(1000);

        }


		/*
		 * Delay activation of IP flows until flow is "assured"
		 * An assured connection is typically never deleted
		 * prematurely.
		 */
		if ((protocol) &&
		    (!test_bit(IPS_ASSURED_BIT, &flow_p->ct->status))) {

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			    printk("ABORT: NOT ASSURED TCP flow<0x%08x> ct<0x%08x>"
			           " proto<%u>\n",
                       (unsigned int)flow_p, (unsigned int)flow_p->ct,
			           flow_p->l4_proto);

            goto done;
		}

        /* For other related connection(s), we begin anew */
        related = 0;

		help = nfct_help(flow_p->ct);

		if ((help != (struct nf_conn_help *)NULL) &&
		   (help->helper != (struct nf_conntrack_helper *)NULL)) {

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			        printk("ABORT HELPER flow<0x%08x> ct<0x%08x> helper<%s>"
			               " proto<%u>\n",
			               (unsigned int)flow_p, (unsigned int)flow_p->ct,
                           help->helper->name, flow_p->l4_proto);

            goto done;
		}

        //udelay(1000);

		switch(et_type) {
		case ETH_P_IP:
			flow_p->dot1q_tag = vlan_tag;
			if (flow_p->fe.ipv4.direction == 1) {
				/* WAN to LAN Inbound */
				flow_p->fe.ipv4.remap_ip = ntohl(iph->daddr);
				flow_p->fe.ipv4.remap_port = ntohs(th->dest);
				//ctf_next_hop_table_add_entry(0x81000001,
				ctf_next_hop_table_add_entry(flow_p->dot1q_tag,
						mac->h_dest, &sa_ix, 0, 1); /* frmtype:0, op:1 */
				ctf_add_napt_pool_entry(mac->h_source, &pt_ix, 0);
			} else {
				/* LAN to WAN Outbound */
				flow_p->fe.ipv4.remap_ip = ntohl(iph->saddr);
				flow_p->fe.ipv4.remap_port = ntohs(th->source);
				//ctf_next_hop_table_add_entry(0x81000002,
				ctf_next_hop_table_add_entry(flow_p->dot1q_tag,
						mac->h_dest, &sa_ix, 0, 1); /* frmtype:0, op:1 */
				ctf_add_napt_pool_entry(mac->h_source, &pt_ix, 1);
			}
			flow_p->fe.ipv4.next_hop_idx	= sa_ix; /* next hop */
			flow_p->fe.ipv4.remap_sa_idx	= pt_ix; /* pool table */
			flow_p->fe.ipv4.valid		= 1;

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			    printk("IPv4 Flow <0x%08x><%d:%d>: Src <0x%08x> Dest <0x%08x> "
			           " SPort <0x%04x> DPort <0x%04x> dir=%s\n",
                       (unsigned int)flow_p,
			           flow_p->key.hash_index, flow_p->key.bkt_index,
			           ntohl(iph->saddr), ntohl(iph->daddr),
			           ntohs(th->source), ntohs(th->dest),
			           (flow_p->fe.ipv4.direction == 0) ? "Inbound" :
			           "Outbound");

			break;

		case ETH_P_IPV6:
			flow_p->dot1q_tag = vlan_tag;
			if (flow_p->fe.ipv6.direction == 1) {
				/* WAN to LAN Inbound */
                /* code chk */
                memcpy((void *)flow_p->fe.ipv6.remap_ip, 
                       (void *)&(ipv6h->daddr), sizeof(struct  in6_addr));
				flow_p->fe.ipv6.remap_port = ntohs(th->dest);
				ctf_next_hop_table_add_entry(flow_p->dot1q_tag,
						mac->h_dest, &sa_ix, 0, 1); /* frmtype:0, op:1 */
				ctf_add_napt_pool_entry(mac->h_source, &pt_ix, 0);
			} else {
				/* LAN to WAN Outbound */
                /* code chk */
	            memcpy((void *)flow_p->fe.ipv6.remap_ip, 
                       (void *)&(ipv6h->saddr), sizeof(struct  in6_addr));
				flow_p->fe.ipv6.remap_port = ntohs(th->source);
				ctf_next_hop_table_add_entry(flow_p->dot1q_tag,
						mac->h_dest, &sa_ix, 0, 1); /* frmtype:0, op:1 */
				ctf_add_napt_pool_entry(mac->h_source, &pt_ix, 1);
			}
			flow_p->fe.ipv6.next_hop_idx	= sa_ix; /* next hop */
			flow_p->fe.ipv6.remap_sa_idx	= pt_ix; /* pool table */
			flow_p->fe.ipv6.valid		= 1;

            if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL)
			    printk("IPv6 Flow <0x%08x><%d:%d>: "
			           " SPort <%u> DPort <%u> dir=%s\n",
                       (unsigned int)flow_p,
			           flow_p->key.hash_index, flow_p->key.bkt_index,
			           ntohs(th->source), ntohs(th->dest),
			           (flow_p->fe.ipv6.direction == 0) ? "Inbound" :
			           "Outbound");
			break;

		default:
			printk("Unknown layer 3 protocol <%d>\n",
				mac->h_proto);
			goto done;
		}
	} else {
		printk("Tx Path: Conntrack does not exist...\n");
        goto done;
	}


    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {

	    printk("====== fc_transmit: Adding to flow table at %u...\n",
               (skb_p->napt_hash_ix  * 4) + skb_p->napt_bkt_ix);
    }

    //printk(KERN_DEBUG "\n%s: location=%u\n", __func__, flow_p->location);

    if (flow_p->location == IN_FLOW_TABLE) {

         //printk(KERN_DEBUG "%s: flow_p=0x%p: Flow with hash=%u, bucket=%u is already in the flow"
          //      " table", 
           //     __func__, flow_p, flow_p->key.hash_index, flow_p->key.bkt_index);

         goto done;

   }

	fc_learn(flow_p, skb_p);

    if (ctf_dbg_log_level > CTF_DBG_MEDIUM_LVL) {
	    printk("fc_transmit: Tx Packet dump...\n");
	    fc_dump_skb(skb_p, 0);
    }

done:
    spin_unlock(&ctf_lock);
	return PKT_NORM;
}

/*
 * Function     : fc_status
 * Description  : Dump the flow cache status and cummulative statistics
 */
void fc_status(void)
{
	printk(KERN_DEBUG "Dump Stats...\n");
}

/*
 * Function     : fc_flush
 * Description  : Flushes the entire flow cache
 */
void fc_flush(void)
{
	volatile flow_cache_t	*flow_p = &fcache;
	int		ix;

    spin_lock(&ctf_lock);
	/*
	 * Clear the hardware entries. Check netfilter 
	 * for any tracked flows and clear them.
	 */
	ctf_clear_napt_flow_table(-1);
	for (ix = 0; ix < FCACHE_HTABLE_SIZE; ix++) {
		flow_p->htable[ix] = FLOW_NULL;
	}

	for (ix = 0; ix < FCACHE_MAX_ENTRIES; ix++) {
		//memset((void *)&(flow_p->etable[ix]), 0, sizeof(napt_flow_t));
		fcache.etable[ix].valid = 0;
	}
    spin_unlock(&ctf_lock);
}

/*
 * Function     : fc_construct
 * Description  : Static construction of flow cache subsystem.
 */
int fc_construct(void)
{
	register int	id;
	volatile napt_flow_t	*flow_p;

	printk("Flow Cache Initializing...\n");

	memset((void*)&fcache, 0, sizeof(flow_cache_t));

	/* Initialize each flow cache entry */
	for (id = 0; id < FCACHE_MAX_ENTRIES; id++) {
		flow_p = &fcache.etable[id];
		memset((void *)&flow_p->key, 0, sizeof(napt_flow_key_t));
	}

    flow_timestamp = 0;

	init_timer(&fcache.timer);
	fcache.timer.expires = jiffies + FCACHE_REFRESH_INTERVAL * HZ;
	fcache.timer.function = fc_timer;
	fcache.timer.data = (unsigned long)0;
	add_timer(&fcache.timer);

    spin_lock_init(&ctf_lock);

	printk(KERN_DEBUG "%s %s Initialized\n", CTF_MODNAME, CTF_VER_STR);

	return 0;
}

/*
 * Function     : fc_destruct
 * Description  : Destruction of flow cache subsystem.
 */
void fc_destruct(void)
{
    int i;

    printk(KERN_DEBUG "Destroying Flow Cache...\n");

    del_timer(&fcache.timer);

    fc_flush();

    for (i = 0; i < FCACHE_MAX_ENTRIES; i++) {
        memset((void *)&(fcache.etable[i].key), 0, sizeof(napt_flow_key_t));
    }
    memset((void*)&fcache, 0, sizeof(flow_cache_t));

    printk(KERN_DEBUG "%s %s Destroyed\n", CTF_MODNAME, CTF_VER_STR);
}


EXPORT_SYMBOL(fc_receive);
EXPORT_SYMBOL(fc_transmit);
EXPORT_SYMBOL(fc_status);
EXPORT_SYMBOL(fc_flush);
EXPORT_SYMBOL(fc_clear);
EXPORT_SYMBOL(fc_destroy);
EXPORT_SYMBOL(fc_nfct);

module_init(fc_construct);
module_exit(fc_destruct);
