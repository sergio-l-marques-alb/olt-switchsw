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
 * This file contains functions to access various tables in fa+ device. This 
 * functions must be used internally within the fa+ driver
 *
 */
#include <linux/module.h>
#include <linux/if_ether.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <asm/irq.h>
#include "mach/iproc_regs.h"
#include "mach/io_map.h"
#include "mach/irqs.h"
#include "fa2_regs.h"
#include "fa2_defs.h"

#define FA2_SEL_TBL(t) ((FA2_FLTBL_V6_W1 <= (t)) && ((t) <= FA2_FLTBL_V4)) ? \
                       FA2_SEL_FLTBL : ((t) == FA2_MTUTBL) ? \
                       FA2_SEL_MTUTBL : ((t) == FA2_PORTTYPTBL) ? \
                       FA2_SEL_PORTTYPTBL : ((t) == FA2_NXTHOPTBL) ? \
                       FA2_SEL_NXTHOPTBL : ((t) == FA2_RTMACTBL) ? \
                       FA2_SEL_RTMACTBL : ((t) == FA2_TUNLTBL) ? \
                       FA2_SEL_TUNLTBL : FA2_SEL_INVALID;

#define FA2_TBL_ENT_SZ(t) ((FA2_FLTBL_V6_W1 <= (t)) && \
                           ((t) <= FA2_FLTBL_V4)) ? \
                       FA2_FLOWTBL_ENTRY_SIZE : ((t) == FA2_MTUTBL) ? \
                       FA2_MTUTBL_ENTRY_SIZE : ((t) == FA2_PORTTYPTBL) ? \
                       FA2_PORTYPTBL_ENTRY_SIZE : ((t) == FA2_NXTHOPTBL) ? \
                       FA2_NEXTHOPTBL_ENTRY_SIZE : ((t) == FA2_RTMACTBL) ? \
                       FA2_RTMACTBL_ENTRY_SIZE : ((t) == FA2_TUNLTBL) ? \
                       FA2_TUNLTBL_ENTRY_SIZE : FA2_INVALID_ENTRY_SIZE;

/* Used for write and read accesses of all FA+ tables. For FA+'s internal use
 * only. Do not modify.
 */
struct fa2_fld_info {
    int hi_bit; /* Upper bit # of the field (in FA+ Uarch doc) */
    int lo_bit; /* Lower bit # of the field (in FA+ Uarch doc) */
    uint32_t loword_mask; /* lo/hiword_mask used with 'to' buffer */
    uint32_t hiword_mask;
    uint32_t pri_mask; /* pri/next mask and shift are used with 'from' buffer */
    uint32_t nxt_mask;
    int nxtmask_shift;
};

/* Values of enum match TABLE_SELECT field in register 0x4 (mem acc ctl reg).
 * Do not change unless TABLE_SELECT values change (due to hw design changes)
 */
enum fa2_tbl_sel {
    FA2_SEL_FLTBL = 0, /* Flow table */
    FA2_SEL_RTMACTBL = 1, /* Tunnel table */
    FA2_SEL_NXTHOPTBL = 2, /* Router MAC table */
    FA2_SEL_MTUTBL = 3, /* MTU table */
    FA2_SEL_TUNLTBL = 4, /* Tunnel table */
    FA2_SEL_PORTTYPTBL = 5, /* Port type table */
    FA2_SEL_INVALID = 0xFF
};

void fa2_fill_rtmac_entry(struct fa2_rtmac_tbl_entry *rtmp)
{
    rtmp->host = 0x0;
    rtmp->l4_checksum_check = 0x1;
    rtmp->external = 0x1;
    rtmp->mac_addr[0] = 0x0F;
    rtmp->mac_addr[1] = 0x06;
    rtmp->mac_addr[2] = 0xD5;
    rtmp->mac_addr[3] = 0x02;
    rtmp->mac_addr[4] = 0x01;
    rtmp->mac_addr[5] = 0xFF;

    return;
}

void fa2_fill_tunltbl_entry(struct fa2_tunnel_tbl_entry *ttp)
{
    ttp->action = 0x1;
    ttp->dmac_idx = 0x3;
    ttp->smac[0] = 0xFF;
    ttp->smac[1] = 0x01;
    ttp->smac[2] = 0xDD;
    ttp->smac[3] = 0xCC;
    ttp->smac[4] = 0x02;
    ttp->smac[5] = 0xBE;
    ttp->session_id = 0x88A9;

    return;
}

void fa2_fill_nht_entry(struct fa2_nh_tbl_entry *nhp)
{

    nhp->vlan = 0xFFEE;
    nhp->op = 0x7;
    nhp->l2_frmtype = 0x1;
    nhp->da[0] = 0x00;
    nhp->da[1] = 0x00;
    nhp->da[2] = 0xFF;
    nhp->da[3] = 0xA8;
    nhp->da[4] = 0x0E;
    nhp->da[5] = 0x01;

    return;
}

void fa2_fill_ipv4_entry(struct fa2_flow_entry_ipv4 *v4p)
{

    v4p->ipv4_key_type = 0x1;
    v4p->valid = 0x1;
    v4p->sip = 0xDAA5A55F;
    v4p->dip= 0xC001F011;
    v4p->protocol = 0x88;
    v4p->sport = 0xF10E;
    v4p->dport = 0x8ABC;
    v4p->rev_flow_ptr = 0xEAB; /* 12 bits */
    v4p->brcm_tag_opcode = 0x7;
    v4p->brcm_tag_tc = 0x1;
    v4p->brcm_tag_te = 0x0;
    v4p->brcm_tag_ts = 0x0;
    v4p->brcm_tag_destmap = 0x30F; /* 10 bits */
    v4p->direction = 1;
    v4p->l4_chksum_chk = 0;
    v4p->ppp_tunnel_en = 1;
    v4p->ppp_tunnel_idx = 0xF; /* 4 bits */
    v4p->mtu_idx = 0x7; /* 3 bits */
    v4p->next_hop_idx = 0x7F; /* 7 bits */
    v4p->remap_sa_idx = 0x7; /* 3 bits */
    v4p->dest_dma_chan = 0x3; /* 2 bits */
    v4p->action = 0x4; /* 3 bits */
    v4p->hits = 0xF234FEBE;
    v4p->tcp_fin = 0x0; /* 1 bit */
    v4p->tcp_rst = 0x1; /* 1 bit */
    v4p->tcp_ack_after_close = 0x1; /* 1 bit */
    v4p->hit_after_close = 0x1; /* 1 bit */
    v4p->flow_state = 0x5; /* 3 bits */
    v4p->flow_timer = 0x1A;

    return;
}

void fa2_fill_ipv6_entry_w1(struct fa2_flow_entry_ipv6_w1 *w1p)
{
    w1p->ipv4_key_type = 0x1;
    w1p->valid = 1;
    w1p->dip_upper[0] = 0xE1;
    w1p->dip_upper[1] = 0xE2;
    w1p->dip_upper[2] = 0xE3;
    w1p->dip_upper[3] = 0xE4;
    w1p->dip_upper[4] = 0xF1;
    w1p->dip_upper[5] = 0x02;
    w1p->dip_upper[6] = 0x08;
    w1p->dip_upper[7] = 0xFF;

    w1p->sip[0] = 0x68;
    w1p->sip[1] = 0x34;
    w1p->sip[2] = 0x56;
    w1p->sip[3] = 0x78;
    w1p->sip[4] = 0x59;
    w1p->sip[5] = 0xBB;
    w1p->sip[6] = 0xCE;
    w1p->sip[7] = 0x23;

    w1p->sip[8] = 0x55;
    w1p->sip[9] = 0x56;
    w1p->sip[10] = 0x57;
    w1p->sip[11] = 0x58;
    w1p->sip[12] = 0xFF;
    w1p->sip[13] = 0xFF;
    w1p->sip[14] = 0x7B;
    w1p->sip[15] = 0x80;

    return;
}

void fa2_fill_ipv6_entry_w0(struct fa2_flow_entry_ipv6_w0 *w0p)
{
    w0p->ipv4_key_type = 0x0;
    w0p->valid = 0;
    w0p->dip_lower[0] = 0xAA;
    w0p->dip_lower[1] = 0xBB;
    w0p->dip_lower[2] = 0xCC;
    w0p->dip_lower[3] = 0xDD;
    w0p->dip_lower[4] = 0xEE;
    w0p->dip_lower[5] = 0xFF;
    w0p->dip_lower[6] = 0x11;
    w0p->dip_lower[7] = 0x22;

    w0p->protocol = 0x06;
    w0p->sport = 0xBB9C;
    w0p->dport = 0x23AC;
    w0p->rev_flow_ptr = 0x100; /* 12 bits */
    w0p->brcm_tag_opcode = 0x6;
    w0p->brcm_tag_tc = 0x5;
    w0p->brcm_tag_te = 0x1;
    w0p->brcm_tag_ts = 0x1;
    w0p->brcm_tag_destmap = 0x57; /* 10 bits */
    w0p->direction = 1;
    w0p->l4_chksum_chk = 0;
    w0p->ppp_tunnel_en = 1;
    w0p->ppp_tunnel_idx = 0xF; /* 4 bits */
    w0p->mtu_idx = 0x1; /* 3 bits */
    w0p->next_hop_idx = 0x2A; /* 7 bits */
    w0p->remap_sa_idx = 0x2; /* 3 bits */
    w0p->dest_dma_chan = 0x1; /* 2 bits */
    w0p->action = 0x4; /* 3 bits */
    w0p->hits = 0x00000101;
    w0p->tcp_fin = 0x1; /* 1 bit */
    w0p->tcp_rst = 0x1; /* 1 bit */
    w0p->tcp_ack_after_close = 0x0; /* 1 bit */
    w0p->hit_after_close = 0x1; /* 1 bit */
    w0p->flow_state = 0x7; /* 3 bits */
    w0p->flow_timer = 0x80;

    return;
}

void fa2_display_rtmac_entry(struct fa2_rtmac_tbl_entry *rtmp)
{
    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_DEBUG "\nRouter MAC table entry:");
    printk(KERN_INFO "\nhost = %u", rtmp->host);
    printk(KERN_INFO "\nl4_checksum_check = %u", rtmp->l4_checksum_check);
    printk(KERN_INFO "\nexternal = %u", rtmp->external);
    printk(KERN_INFO "\nmac_addr[5-0] = 0x {%02X %02X %02X %02X %02X %02X}\n", 
           rtmp->mac_addr[5], rtmp->mac_addr[4], rtmp->mac_addr[3],
           rtmp->mac_addr[2], rtmp->mac_addr[1], rtmp->mac_addr[0]);

    printk(KERN_INFO "\n------------------------------------------------\n");
}

void fa2_display_tunltbl_entry(struct fa2_tunnel_tbl_entry *ttp)
{
    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_INFO "\nTunnel table entry:\n");
    printk(KERN_INFO "\naction = %u", ttp->action);
    printk(KERN_INFO "\ndmac_idx = %u", ttp->dmac_idx);
    printk(KERN_INFO "\nsmac[5-0] = 0x {%02X %02X %02X %02X %02X %02X}\n", 
           ttp->smac[5], ttp->smac[4], ttp->smac[3],
           ttp->smac[2], ttp->smac[1], ttp->smac[0]);
    printk(KERN_INFO "\n session_id = %u (0x%X)\n", ttp->session_id, ttp->session_id);

    printk(KERN_INFO "\n------------------------------------------------\n");

    return;
}

void fa2_display_nh_entry(struct fa2_nh_tbl_entry *nhp)
{

    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_INFO "\nNext hop table entry:\n");
    printk(KERN_INFO "\nvlan = %u (0x%X)", nhp->vlan, nhp->vlan);
    printk(KERN_INFO "\nop = %u", nhp->op);
    printk(KERN_INFO "\nl2_frmtype = %u", nhp->l2_frmtype);

    printk(KERN_INFO "\nda[5-0] = 0x {%02X %02X %02X %02X %02X %02X}\n", 
           nhp->da[5], nhp->da[4], nhp->da[3],
           nhp->da[2], nhp->da[1], nhp->da[0]);

    printk(KERN_INFO "\n------------------------------------------------\n");

    return;
}

void fa2_display_ipv4_info(struct fa2_flow_entry_ipv4 *v4p)
{

    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_INFO "\nIPv4:\n");
    printk(KERN_INFO "\nipv4_key_type = %u", v4p->ipv4_key_type);
    printk(KERN_INFO "\nvalid = %u", v4p->valid);
    printk(KERN_INFO "\nSIP = 0x%08X", v4p->sip);
    printk(KERN_INFO "\nDIP = 0x%08X", v4p->dip);

    printk(KERN_INFO "\nprotocol = %u(0x%X)", v4p->protocol, v4p->protocol);
    printk(KERN_INFO "\nsport = %u(0x%X)", v4p->sport, v4p->sport);
    printk(KERN_INFO "\ndport = %u(0x%X)", v4p->dport, v4p->dport);
    printk(KERN_INFO "\nrev_flow_ptr = %u(0x%X)", v4p->rev_flow_ptr, v4p->rev_flow_ptr);
    printk(KERN_INFO "\nbrcm_tag_opcode = %u(0x%X)", v4p->brcm_tag_opcode, 
           v4p->brcm_tag_opcode);
    printk(KERN_INFO "\nbrcm_tag_tc = %u(0x%X)", v4p->brcm_tag_tc, v4p->brcm_tag_tc);
    printk(KERN_INFO "\nbrcm_tag_te = %u(0x%X)", v4p->brcm_tag_te, v4p->brcm_tag_te);
    printk(KERN_INFO "\nbrcm_tag_ts = %u(0x%X)", v4p->brcm_tag_ts, v4p->brcm_tag_ts);
    printk(KERN_INFO "\nbrcm_tag_destmap = %u(0x%X)", v4p->brcm_tag_destmap, 
           v4p->brcm_tag_destmap);
    printk(KERN_INFO "\ndirection = %u(0x%X)", v4p->direction, v4p->direction);
    printk(KERN_INFO "\nl4_chksum_chk = %u(0x%X)", v4p->l4_chksum_chk,
           v4p->l4_chksum_chk);
    printk(KERN_INFO "\nppp_tunnel_en = %u(0x%X)", v4p->ppp_tunnel_en, 
           v4p->ppp_tunnel_en);
    printk(KERN_INFO "\nppp_tunnel_idx = %u(0x%X)", v4p->ppp_tunnel_idx,
           v4p->ppp_tunnel_idx);
    printk(KERN_INFO "\nmtu_idx = %u(0x%X)", v4p->mtu_idx, v4p->mtu_idx);
    printk(KERN_INFO "\nnext_hop_idx = %u(0x%X)", v4p->next_hop_idx, v4p->next_hop_idx);
    printk(KERN_INFO "\nremap_sa_idx = %u(0x%X)", v4p->remap_sa_idx, v4p->remap_sa_idx);
    printk(KERN_INFO "\ndest_dma_chan = %u(0x%X)", v4p->dest_dma_chan,
           v4p->dest_dma_chan);
    printk(KERN_INFO "\naction = %u(0x%X)", v4p->action, v4p->action);
    printk(KERN_INFO "\nhits = %u(0x%X)", v4p->hits, v4p->hits);
    printk(KERN_INFO "\ntcp_fin = %u(0x%X)", v4p->tcp_fin, v4p->tcp_fin);
    printk(KERN_INFO "\ntcp_rst = %u(0x%X)", v4p->tcp_rst, v4p->tcp_rst);
    printk(KERN_INFO "\ntcp_ack_after_close = %u(0x%X)",
           v4p->tcp_ack_after_close, v4p->tcp_ack_after_close);
    printk(KERN_INFO "\nhit_after_close = %u(0x%X)", v4p->hit_after_close,
           v4p->hit_after_close);
    printk(KERN_INFO "\nflow_state = %u(0x%X)", v4p->flow_state, v4p->flow_state);
    printk(KERN_INFO "\nflow_timer = %u(0x%X)", v4p->flow_timer, v4p->flow_timer);
    printk(KERN_INFO "\n------------------------------------------------\n");

    return;
}

void fa2_display_ipv6_w1_info(struct fa2_flow_entry_ipv6_w1 *w1p)
{

    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_INFO "\nIPv6-word1:\n\n");
    printk(KERN_INFO "\nipv4_key_type = %u", w1p->ipv4_key_type);
    printk(KERN_INFO "\nvalid = %u", w1p->valid);

    printk(KERN_INFO "\nw1p->sip[0-7] = 0x { %02X%02X %02X%02X %02X%02X %02X%02X }", w1p->sip[0], w1p->sip[1], w1p->sip[2], w1p->sip[3], w1p->sip[4], w1p->sip[5], w1p->sip[6], w1p->sip[7]);
    printk(KERN_INFO "\nw1p->sip[8-15] = 0x { %02X%02X %02X%02X %02X%02X %02X%02X }", w1p->sip[8], w1p->sip[9], w1p->sip[10], w1p->sip[11], w1p->sip[12], w1p->sip[13], w1p->sip[14], w1p->sip[15]);
    printk(KERN_INFO "\nw1p->dip_upper[0-7] = 0x { %02X%02X %02X%02X %02X%02X %02X%02X }", w1p->dip_upper[0], w1p->dip_upper[1], w1p->dip_upper[2], w1p->dip_upper[3], w1p->dip_upper[4], w1p->dip_upper[5], w1p->dip_upper[6], w1p->dip_upper[7]);
    printk(KERN_INFO "\n------------------------------------------------\n");

    return;
}


void fa2_display_ipv6_w0_info(struct fa2_flow_entry_ipv6_w0 *w0p)
{

    printk(KERN_INFO "\n------------------------------------------------\n");
    printk(KERN_INFO "\nIPv6-word0:\n\n");
    printk(KERN_INFO "\nipv4_key_type = %u", w0p->ipv4_key_type);
    printk(KERN_INFO "\nvalid = %u", w0p->valid);

    printk(KERN_INFO "\nw0p->dip_lower[0-7] = 0x { %02X%02X %02X%02X %02X%02X %02X%02X }", w0p->dip_lower[0], w0p->dip_lower[1], w0p->dip_lower[2], w0p->dip_lower[3], w0p->dip_lower[4], w0p->dip_lower[5], w0p->dip_lower[6], w0p->dip_lower[7]);

    printk(KERN_INFO "\nprotocol = %u(0x%X)", w0p->protocol, w0p->protocol);
    printk(KERN_INFO "\nsport = %u(0x%X)", w0p->sport, w0p->sport);
    printk(KERN_INFO "\ndport = %u(0x%X)", w0p->dport, w0p->dport);
    printk(KERN_INFO "\nrev_flow_ptr = %u(0x%X)", w0p->rev_flow_ptr, w0p->rev_flow_ptr);
    printk(KERN_INFO "\nbrcm_tag_opcode = %u(0x%X)", w0p->brcm_tag_opcode, w0p->brcm_tag_opcode);
    printk(KERN_INFO "\nbrcm_tag_tc = %u(0x%X)", w0p->brcm_tag_tc, w0p->brcm_tag_tc);
    printk(KERN_INFO "\nbrcm_tag_te = %u(0x%X)", w0p->brcm_tag_te, w0p->brcm_tag_te);
    printk(KERN_INFO "\nbrcm_tag_ts = %u(0x%X)", w0p->brcm_tag_ts, w0p->brcm_tag_ts);
    printk(KERN_INFO "\nbrcm_tag_destmap = %u(0x%X)", w0p->brcm_tag_destmap, w0p->brcm_tag_destmap);
    printk(KERN_INFO "\ndirection = %u(0x%X)", w0p->direction, w0p->direction);
    printk(KERN_INFO "\nl4_chksum_chk = %u(0x%X)", w0p->l4_chksum_chk, w0p->l4_chksum_chk);
    printk(KERN_INFO "\nppp_tunnel_en = %u(0x%X)", w0p->ppp_tunnel_en, w0p->ppp_tunnel_en);
    printk(KERN_INFO "\nppp_tunnel_idx = %u(0x%X)", w0p->ppp_tunnel_idx, w0p->ppp_tunnel_idx);
    printk(KERN_INFO "\nmtu_idx = %u(0x%X)", w0p->mtu_idx, w0p->mtu_idx);
    printk(KERN_INFO "\nnext_hop_idx = %u(0x%X)", w0p->next_hop_idx, w0p->next_hop_idx);
    printk(KERN_INFO "\nremap_sa_idx = %u(0x%X)", w0p->remap_sa_idx, w0p->remap_sa_idx);
    printk(KERN_INFO "\ndest_dma_chan = %u(0x%X)", w0p->dest_dma_chan, w0p->dest_dma_chan);
    printk(KERN_INFO "\naction = %u(0x%X)", w0p->action, w0p->action);
    printk(KERN_INFO "\nhits = %u(0x%X)", w0p->hits, w0p->hits);
    printk(KERN_INFO "\ntcp_fin = %u(0x%X)", w0p->tcp_fin, w0p->tcp_fin);
    printk(KERN_INFO "\ntcp_rst = %u(0x%X)", w0p->tcp_rst, w0p->tcp_rst);
    printk(KERN_INFO "\ntcp_ack_after_close = %u(0x%X)", w0p->tcp_ack_after_close, w0p->tcp_ack_after_close);
    printk(KERN_INFO "\nhit_after_close = %u(0x%X)", w0p->hit_after_close, w0p->hit_after_close);
    printk(KERN_INFO "\nflow_state = %u(0x%X)", w0p->flow_state, w0p->flow_state);
    printk(KERN_INFO "\nflow_timer = %u(0x%X)", w0p->flow_timer, w0p->flow_timer);
    printk(KERN_INFO "\n------------------------------------------------\n");

    return;
}

/*
 * This function copies a register field of any length (not restricted to
 * 32 bits) from 'from' buffer in to 'to' buffer. Caller provides starting
 * bit location in 'to' buffer (lo_bit) where the function will start copying
 * (hi_bit - lo_bit + 1) bits of data. 
 * Value for hi_bit and to_lo_bit for each field are obtained from
 * corresponding table definitions in FA+ Uarch doc
 */
void fa2_copy_table_field(uint32_t *from, uint32_t *to, struct fa2_fld_info *f)
{
    int hi, lo;
    uint32_t val_l, val_u;
    int hiw, low, n;
    uint32_t mask;

    lo = f->lo_bit & 0x1F; /* % 32 */
    hi = f->hi_bit & 0x1F;
    low = f->lo_bit >> 5;
    hiw = f->hi_bit >> 5;

    n = 0;

    /* Find the location where bits should be copied */
    to += low;

    /* If field is contained in the same word, we apply simple steps to
     * copy the bits
     */
    if (hiw == low) {

        FA2_SETBITS(*to, *from, f->loword_mask, lo);

        return;
    }

    do {

        val_l = *from & f->pri_mask;
        n+=(32 - lo);

        /* If we read 32 bits, we increment 'from' ptr to read next item */
        if (f->pri_mask == 0xFFFFFFFF) {
            from++;

            val_u = (*from & f->nxt_mask) >> f->nxtmask_shift;
        }
        else {

                val_u = (*from & f->nxt_mask) >> f->nxtmask_shift;

            from++;


        }
        n+= (32 - f->nxtmask_shift);

        mask = f->loword_mask;

        FA2_SETBITS(*to, val_l, mask, lo);

        low++;

        /* Set up mask */
        if (low < hiw) {

            mask = ~mask; /* E.g., in present iter we write 17 bits, then 15 and
                           * so on
                           */
        }
        else {

            mask = f->hiword_mask;

        }

        to++;

        FA2_SETBITS(*to, val_u, mask, 0);

    if (low == hiw) {
        if (n < (f->hi_bit - f->lo_bit + 1)) {
            val_l = *from & f->pri_mask;
            FA2_SETBITS(*to, val_l, 0x0, f->nxtmask_shift);
        }
    }

    } while (low < hiw);


    return;
}


void fa2_get_table_field(uint32_t *from, uint32_t *to, struct fa2_fld_info *f)
{
    int hi, lo;
    uint32_t val;
    int hiw, low;
    uint32_t mask;
    int n;

    lo = f->lo_bit & 0x1F; /* % 32 */
    hi = f->hi_bit & 0x1F;
    low = f->lo_bit >> 5;
    hiw = f->hi_bit >> 5;

    /* Find the location from where bits should be copied */
    from += low;

    /* If field is contained in the same word, we apply simple steps to
     * copy the bits
     */
    if (hiw == low) {

        *to = FA2_GETBITS(*from, f->loword_mask, lo);

        return;
    }

    do {

        mask = f->loword_mask;
 
        *to = FA2_GETBITS(*from, mask, lo);

        n+=(32 - lo);
        from++;

        low++;

        /* Set up maskt */
        if (low < hiw) {

            mask = ~mask; /* E.g., in present iter we write 17 bits, then 15 and
                           * so on
                           */
        }
        else {

            mask = f->hiword_mask;

        }

        val = FA2_GETBITS(*from, mask, 0);
        n+= (32 - f->nxtmask_shift);


        /* If above get fetched 32 bits, we copy next value to the next
         * location
         */
        if (f->loword_mask == 0xFFFFFFFF) {
            to++;

            *to |= (val << f->nxtmask_shift);
        }
        else {

            *to |= (val << f->nxtmask_shift);

            to++;
        }
        if ((low == hiw) && (val >> f->nxtmask_shift)) {
            *to = val >> f->nxtmask_shift;
        }

    } while (low < hiw);


    return;
}

void fa2_get_nht_entry(struct fa2_nh_tbl_entry *nhp, uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    f.hi_bit = FA2_NHT_VLAN_HI_BIT;
    f.lo_bit = FA2_NHT_VLAN_LO_BIT;
    f.loword_mask = FA2_NHT_VLAN_LO_WORD_MASK;
    f.hiword_mask = FA2_NHT_VLAN_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_NHT_VLAN_PRI_MASK;
    f.nxt_mask = FA2_DRV_NHT_VLAN_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_NHT_VLAN_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    nhp->vlan = val;

    val = FA2_GETBITS(buffer[FA2_NHT_OP_LO_BIT >> 5], FA2_NHT_OP_MASK, FA2_NHT_OP_LO_BIT & 0x1F);
    nhp->op = val;

    val = FA2_GETBITS(buffer[FA2_NHT_L2_FRAME_TYPE_LO_BIT >> 5], FA2_NHT_L2_FRAME_TYPE_MASK, FA2_NHT_L2_FRAME_TYPE_LO_BIT & 0x1F);
    nhp->l2_frmtype = val;

    f.hi_bit = FA2_NHT_DA_HI_BIT;
    f.lo_bit = FA2_NHT_DA_LO_BIT;
    f.loword_mask = FA2_NHT_DA_LO_WORD_MASK;
    f.hiword_mask = FA2_NHT_DA_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_NHT_DA_PRI_MASK;
    f.nxt_mask = FA2_DRV_NHT_DA_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_NHT_DA_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(nhp->da), &f);

    return;
}

void fa2_get_tunltbl_entry(struct fa2_tunnel_tbl_entry *ttp, uint32_t *buffer)
{

    struct fa2_fld_info f;
    uint32_t val;

    val = FA2_GETBITS(buffer[FA2_TUNTBL_ACTION_LO_BIT >> 5], FA2_TUNTBL_ACTION_MASK, FA2_TUNTBL_ACTION_LO_BIT & 0x1F);
    ttp->action = val;

    val = FA2_GETBITS(buffer[FA2_TUNTBL_DMAC_IDX_LO_BIT >> 5], FA2_TUNTBL_DMAC_IDX_MASK, FA2_TUNTBL_DMAC_IDX_LO_BIT & 0x1F);
    ttp->dmac_idx = val;

    f.hi_bit = FA2_TUNTBL_SMAC_HI_BIT;
    f.lo_bit = FA2_TUNTBL_SMAC_LO_BIT;
    f.loword_mask = FA2_TUNTBL_SMAC_LO_WORD_MASK;
    f.hiword_mask = FA2_TUNTBL_SMAC_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_TUNTBL_SMAC_PRI_MASK;
    f.nxt_mask = FA2_DRV_TUNTBL_SMAC_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_TUNTBL_SMAC_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(ttp->smac), &f);

    val = FA2_GETBITS(buffer[FA2_TUNTBL_SESSION_ID_LO_BIT >> 5], FA2_TUNTBL_SESSION_ID_MASK, FA2_TUNTBL_SESSION_ID_LO_BIT & 0x1F);
    ttp->session_id = val;

    return;
}

void fa2_get_rtmac_entry(struct fa2_rtmac_tbl_entry *rtmp, uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = FA2_GETBITS(buffer[FA2_RTMAC_HOST_LO_BIT >> 5], FA2_RTMAC_HOST_MASK, FA2_RTMAC_HOST_LO_BIT & 0x1F);
    rtmp->host = val;

    val = FA2_GETBITS(buffer[FA2_RTMAC_L4_CHECKSUM_CHECK_LO_BIT >> 5], FA2_RTMAC_L4_CHECKSUM_CHECK_MASK, FA2_RTMAC_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);
    rtmp->l4_checksum_check = val;

    val = FA2_GETBITS(buffer[FA2_RTMAC_EXTERNAL_LO_BIT >> 5], FA2_RTMAC_EXTERNAL_MASK, FA2_RTMAC_EXTERNAL_LO_BIT & 0x1F);
    rtmp->external = val;

    f.hi_bit = FA2_RTMAC_ROUTER_MAC_ADDR_HI_BIT;
    f.lo_bit = FA2_RTMAC_ROUTER_MAC_ADDR_LO_BIT;
    f.loword_mask = FA2_RTMAC_ROUTER_MAC_ADDR_LO_WORD_MASK;
    f.hiword_mask = FA2_RTMAC_ROUTER_MAC_ADDR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_PRI_MASK;
    f.nxt_mask = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(rtmp->mac_addr), &f);

    return;
}

void fa2_get_ipv4_entry (struct fa2_flow_entry_ipv4 *v4p, uint32_t *buffer)
{

    struct fa2_fld_info f;
    uint32_t val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_IPV4_KEY_TYPE_LO_BIT >> 5], FA2_FE_V4_IPV4_KEY_TYPE_MASK, FA2_FE_V4_IPV4_KEY_TYPE_LO_BIT & 0x1F);
    v4p->ipv4_key_type = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_VALID_LO_BIT >> 5], FA2_FE_V4_VALID_MASK, FA2_FE_V4_VALID_LO_BIT & 0x1F);
    v4p->valid = val;

    f.hi_bit = FA2_FE_V4_SIP_HI_BIT;
    f.lo_bit = FA2_FE_V4_SIP_LO_BIT;
    f.loword_mask = FA2_FE_V4_SIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_SIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_SIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_SIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_SIP_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->sip = val;

    f.hi_bit = FA2_FE_V4_DIP_HI_BIT;
    f.lo_bit = FA2_FE_V4_DIP_LO_BIT;
    f.loword_mask = FA2_FE_V4_DIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_DIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_DIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_DIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_DIP_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->dip = val;


    val = FA2_GETBITS(buffer[FA2_FE_V4_PROTO_LO_BIT >> 5], FA2_FE_V4_PROTO_MASK, FA2_FE_V4_PROTO_LO_BIT & 0x1F);
    v4p->protocol = val;

    f.hi_bit = FA2_FE_V4_SPORT_HI_BIT;
    f.lo_bit = FA2_FE_V4_SPORT_LO_BIT;
    f.loword_mask = FA2_FE_V4_SPORT_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_SPORT_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_SPORT_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_SPORT_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_SPORT_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->sport = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_DPORT_LO_BIT >> 5], FA2_FE_V4_DPORT_MASK, FA2_FE_V4_DPORT_LO_BIT & 0x1F);
    v4p->dport = val;

    f.hi_bit = FA2_FE_V4_REVERSE_FLOW_PTR_HI_BIT;
    f.lo_bit = FA2_FE_V4_REVERSE_FLOW_PTR_LO_BIT;
    f.loword_mask = FA2_FE_V4_REVERSE_FLOW_PTR_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_REVERSE_FLOW_PTR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_REVERSE_FLOW_PTR_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_REVERSE_FLOW_PTR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_REVERSE_FLOW_PTR_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->rev_flow_ptr = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_BCMTAG_OPCODE_LO_BIT >> 5], FA2_FE_V4_BCMTAG_OPCODE_MASK, FA2_FE_V4_BCMTAG_OPCODE_LO_BIT & 0x1F);
    v4p->brcm_tag_opcode = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_BCMTAG_TC_LO_BIT >> 5], FA2_FE_V4_BCMTAG_TC_MASK, FA2_FE_V4_BCMTAG_TC_LO_BIT & 0x1F);
    v4p->brcm_tag_tc = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_BCMTAG_TE_LO_BIT >> 5], FA2_FE_V4_BCMTAG_TE_MASK, FA2_FE_V4_BCMTAG_TE_LO_BIT & 0x1F);
    v4p->brcm_tag_te = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_BCMTAG_TS_LO_BIT >> 5], FA2_FE_V4_BCMTAG_TS_MASK, FA2_FE_V4_BCMTAG_TS_LO_BIT & 0x1F);
    v4p->brcm_tag_ts = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_BCMTAG_DEST_MAP_LO_BIT >> 5], FA2_FE_V4_BCMTAG_DEST_MAP_MASK, FA2_FE_V4_BCMTAG_DEST_MAP_LO_BIT & 0x1F);
    v4p->brcm_tag_destmap = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_DIRECTION_LO_BIT >> 5], FA2_FE_V4_DIRECTION_MASK, FA2_FE_V4_DIRECTION_LO_BIT & 0x1F);
    v4p->direction = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_L4_CHECKSUM_CHECK_LO_BIT >> 5], FA2_FE_V4_L4_CHECKSUM_CHECK_MASK, FA2_FE_V4_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);
    v4p->l4_chksum_chk = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_PPP_TUNNEL_EN_LO_BIT >> 5], FA2_FE_V4_PPP_TUNNEL_EN_MASK, FA2_FE_V4_PPP_TUNNEL_EN_LO_BIT & 0x1F);
    v4p->ppp_tunnel_en = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_PPP_TUNNEL_IDX_LO_BIT >> 5], FA2_FE_V4_PPP_TUNNEL_IDX_MASK, FA2_FE_V4_PPP_TUNNEL_IDX_LO_BIT & 0x1F);
    v4p->ppp_tunnel_idx = val;

    f.hi_bit = FA2_FE_V4_MTU_IDX_HI_BIT;
    f.lo_bit = FA2_FE_V4_MTU_IDX_LO_BIT;
    f.loword_mask = FA2_FE_V4_MTU_IDX_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_MTU_IDX_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_MTU_IDX_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_MTU_IDX_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_MTU_IDX_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->mtu_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_NEXT_HOP_IDX_LO_BIT >> 5], FA2_FE_V4_NEXT_HOP_IDX_MASK, FA2_FE_V4_NEXT_HOP_IDX_LO_BIT & 0x1F);
    v4p->next_hop_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_REMAP_SA_IDX_LO_BIT >> 5], FA2_FE_V4_REMAP_SA_IDX_MASK, FA2_FE_V4_REMAP_SA_IDX_LO_BIT & 0x1F);
    v4p->remap_sa_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_DEST_DMA_CHANNEL_LO_BIT >> 5], FA2_FE_V4_DEST_DMA_CHANNEL_MASK, FA2_FE_V4_DEST_DMA_CHANNEL_LO_BIT & 0x1F);
    v4p->dest_dma_chan = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_ACTION_LO_BIT >> 5], FA2_FE_V4_ACTION_MASK, FA2_FE_V4_ACTION_LO_BIT & 0x1F);
    v4p->action = val;

    f.hi_bit = FA2_FE_V4_HITS_HI_BIT;
    f.lo_bit = FA2_FE_V4_HITS_LO_BIT;
    f.loword_mask = FA2_FE_V4_HITS_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_HITS_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_HITS_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_HITS_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_HITS_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    v4p->hits = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_TCP_FIN_LO_BIT >> 5], FA2_FE_V4_TCP_FIN_MASK, FA2_FE_V4_TCP_FIN_LO_BIT & 0x1F);
    v4p->tcp_fin = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_TCP_RST_LO_BIT >> 5], FA2_FE_V4_TCP_RST_MASK, FA2_FE_V4_TCP_RST_LO_BIT & 0x1F);
    v4p->tcp_rst = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_TCP_ACK_AFTER_CLOSE_LO_BIT >> 5], FA2_FE_V4_TCP_ACK_AFTER_CLOSE_MASK, FA2_FE_V4_TCP_ACK_AFTER_CLOSE_LO_BIT & 0x1F);
    v4p->tcp_ack_after_close = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_HIT_AFTER_CLOSE_LO_BIT >> 5], FA2_FE_V4_HIT_AFTER_CLOSE_MASK, FA2_FE_V4_HIT_AFTER_CLOSE_LO_BIT & 0x1F);
    v4p->hit_after_close = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_FLOW_STATE_LO_BIT >> 5], FA2_FE_V4_FLOW_STATE_MASK, FA2_FE_V4_FLOW_STATE_LO_BIT & 0x1F);
    v4p->flow_state = val;

    val = FA2_GETBITS(buffer[FA2_FE_V4_FLOW_TIMER_LO_BIT >> 5], FA2_FE_V4_FLOW_TIMER_MASK, FA2_FE_V4_FLOW_TIMER_LO_BIT & 0x1F);
    v4p->flow_timer = val;

    return;
}

void fa2_get_ipv6_w1_flow_entry (struct fa2_flow_entry_ipv6_w1 *w1p,
                                 uint32_t *buffer)
{

    struct fa2_fld_info f;
    uint32_t val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_LO_BIT >> 5], FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_MASK, FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_LO_BIT & 0x1F);
    w1p->ipv4_key_type = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W1_VALID_LO_BIT >> 5], FA2_FE_V6_W1_VALID_MASK, FA2_FE_V6_W1_VALID_LO_BIT & 0x1F);
    w1p->valid = val;

    f.hi_bit = FA2_FE_V6_W1_SIP_HI_BIT;
    f.lo_bit = FA2_FE_V6_W1_SIP_LO_BIT;
    f.loword_mask = FA2_FE_V6_W1_SIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W1_SIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W1_SIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W1_SIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W1_SIP_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(w1p->sip), &f);

    f.hi_bit = FA2_FE_V6_W1_DIP_UPPER_HI_BIT;
    f.lo_bit = FA2_FE_V6_W1_DIP_UPPER_LO_BIT;
    f.loword_mask = FA2_FE_V6_W1_DIP_UPPER_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W1_DIP_UPPER_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W1_DIP_UPPER_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W1_DIP_UPPER_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W1_DIP_UPPER_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(w1p->dip_upper), &f);

    return;
}

void fa2_get_ipv6_w0_flow_entry (struct fa2_flow_entry_ipv6_w0 *w0p,
                                    uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_LO_BIT >> 5], FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_MASK, FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_LO_BIT & 0x1F);
    w0p->ipv4_key_type = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_VALID_LO_BIT >> 5], FA2_FE_V6_W0_VALID_MASK, FA2_FE_V6_W0_VALID_LO_BIT & 0x1F);
    w0p->valid = val;

    f.hi_bit = FA2_FE_V6_W0_DIP_LOWER_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_DIP_LOWER_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_DIP_LOWER_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_DIP_LOWER_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_DIP_LOWER_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_DIP_LOWER_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_DIP_LOWER_SHIFT;
    fa2_get_table_field(buffer, (uint32_t *)(w0p->dip_lower), &f);

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_PROTO_LO_BIT >> 5], FA2_FE_V6_W0_PROTO_MASK, FA2_FE_V6_W0_PROTO_LO_BIT & 0x1F);
    w0p->protocol = val;

    f.hi_bit = FA2_FE_V6_W0_SPORT_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_SPORT_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_SPORT_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_SPORT_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_SPORT_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_SPORT_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_SPORT_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    w0p->sport = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_DPORT_LO_BIT >> 5], FA2_FE_V6_W0_DPORT_MASK, FA2_FE_V6_W0_DPORT_LO_BIT & 0x1F);
    w0p->dport = val;

    f.hi_bit = FA2_FE_V6_W0_REVERSE_FLOW_PTR_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_REVERSE_FLOW_PTR_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_REVERSE_FLOW_PTR_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_REVERSE_FLOW_PTR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    w0p->rev_flow_ptr = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_BCMTAG_OPCODE_LO_BIT >> 5], FA2_FE_V6_W0_BCMTAG_OPCODE_MASK, FA2_FE_V6_W0_BCMTAG_OPCODE_LO_BIT & 0x1F);
    w0p->brcm_tag_opcode = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TC_LO_BIT >> 5], FA2_FE_V6_W0_BCMTAG_TC_MASK, FA2_FE_V6_W0_BCMTAG_TC_LO_BIT & 0x1F);
    w0p->brcm_tag_tc = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TE_LO_BIT >> 5], FA2_FE_V6_W0_BCMTAG_TE_MASK, FA2_FE_V6_W0_BCMTAG_TE_LO_BIT & 0x1F);
    w0p->brcm_tag_te = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TS_LO_BIT >> 5], FA2_FE_V6_W0_BCMTAG_TS_MASK, FA2_FE_V6_W0_BCMTAG_TS_LO_BIT & 0x1F);
    w0p->brcm_tag_ts = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_BCMTAG_DEST_MAP_LO_BIT >> 5], FA2_FE_V6_W0_BCMTAG_DEST_MAP_MASK, FA2_FE_V6_W0_BCMTAG_DEST_MAP_LO_BIT & 0x1F);
    w0p->brcm_tag_destmap = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_DIRECTION_LO_BIT >> 5], FA2_FE_V6_W0_DIRECTION_MASK, FA2_FE_V6_W0_DIRECTION_LO_BIT & 0x1F);
    w0p->direction = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_L4_CHECKSUM_CHECK_LO_BIT >> 5], FA2_FE_V6_W0_L4_CHECKSUM_CHECK_MASK, FA2_FE_V6_W0_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);
    w0p->l4_chksum_chk = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_PPP_TUNNEL_EN_LO_BIT >> 5], FA2_FE_V6_W0_PPP_TUNNEL_EN_MASK, FA2_FE_V6_W0_PPP_TUNNEL_EN_LO_BIT & 0x1F);
    w0p->ppp_tunnel_en = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_PPP_TUNNEL_IDX_LO_BIT >> 5], FA2_FE_V6_W0_PPP_TUNNEL_IDX_MASK, FA2_FE_V6_W0_PPP_TUNNEL_IDX_LO_BIT & 0x1F);
    w0p->ppp_tunnel_idx = val;

    f.hi_bit = FA2_FE_V6_W0_MTU_IDX_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_MTU_IDX_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_MTU_IDX_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_MTU_IDX_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_MTU_IDX_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_MTU_IDX_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_MTU_IDX_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    w0p->mtu_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_NEXT_HOP_IDX_LO_BIT >> 5], FA2_FE_V6_W0_NEXT_HOP_IDX_MASK, FA2_FE_V6_W0_NEXT_HOP_IDX_LO_BIT & 0x1F);
    w0p->next_hop_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_REMAP_SA_IDX_LO_BIT >> 5], FA2_FE_V6_W0_REMAP_SA_IDX_MASK, FA2_FE_V6_W0_REMAP_SA_IDX_LO_BIT & 0x1F);
    w0p->remap_sa_idx = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_DEST_DMA_CHANNEL_LO_BIT >> 5], FA2_FE_V6_W0_DEST_DMA_CHANNEL_MASK, FA2_FE_V6_W0_DEST_DMA_CHANNEL_LO_BIT & 0x1F);
    w0p->dest_dma_chan = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_ACTION_LO_BIT >> 5], FA2_FE_V6_W0_ACTION_MASK, FA2_FE_V6_W0_ACTION_LO_BIT & 0x1F);
    w0p->action = val;

    f.hi_bit = FA2_FE_V6_W0_HITS_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_HITS_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_HITS_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_HITS_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_HITS_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_HITS_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_HITS_SHIFT;
    fa2_get_table_field(buffer, &val, &f);
    w0p->hits = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_TCP_FIN_LO_BIT >> 5], FA2_FE_V6_W0_TCP_FIN_MASK, FA2_FE_V6_W0_TCP_FIN_LO_BIT & 0x1F);
    w0p->tcp_fin = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_TCP_RST_LO_BIT >> 5], FA2_FE_V6_W0_TCP_RST_MASK, FA2_FE_V6_W0_TCP_RST_LO_BIT & 0x1F);
    w0p->tcp_rst = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_LO_BIT >> 5], FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_MASK, FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_LO_BIT & 0x1F);
    w0p->tcp_ack_after_close = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_HIT_AFTER_CLOSE_LO_BIT >> 5], FA2_FE_V6_W0_HIT_AFTER_CLOSE_MASK, FA2_FE_V6_W0_HIT_AFTER_CLOSE_LO_BIT & 0x1F);
    w0p->hit_after_close = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_FLOW_STATE_LO_BIT >> 5], FA2_FE_V6_W0_FLOW_STATE_MASK, FA2_FE_V6_W0_FLOW_STATE_LO_BIT & 0x1F);
    w0p->flow_state = val;

    val = FA2_GETBITS(buffer[FA2_FE_V6_W0_FLOW_TIMER_LO_BIT >> 5], FA2_FE_V6_W0_FLOW_TIMER_MASK, FA2_FE_V6_W0_FLOW_TIMER_LO_BIT & 0x1F);
    w0p->flow_timer = val;

    return;
}

void fa2_create_nht_entry(struct fa2_nh_tbl_entry *nhp, uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    f.hi_bit = FA2_NHT_VLAN_HI_BIT;
    f.lo_bit = FA2_NHT_VLAN_LO_BIT;
    f.loword_mask = FA2_NHT_VLAN_LO_WORD_MASK;
    f.hiword_mask = FA2_NHT_VLAN_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_NHT_VLAN_PRI_MASK;
    f.nxt_mask = FA2_DRV_NHT_VLAN_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_NHT_VLAN_SHIFT;
    val = nhp->vlan;
    fa2_copy_table_field(&val, buffer, &f);

    val = nhp->op;
    FA2_SETBITS(buffer[FA2_NHT_OP_LO_BIT >> 5], val, FA2_NHT_OP_MASK, FA2_NHT_OP_LO_BIT & 0x1F);

    val = nhp->l2_frmtype;
    FA2_SETBITS(buffer[FA2_NHT_L2_FRAME_TYPE_LO_BIT >> 5], val, FA2_NHT_L2_FRAME_TYPE_MASK, FA2_NHT_L2_FRAME_TYPE_LO_BIT & 0x1F);

    f.hi_bit = FA2_NHT_DA_HI_BIT;
    f.lo_bit = FA2_NHT_DA_LO_BIT;
    f.loword_mask = FA2_NHT_DA_LO_WORD_MASK;
    f.hiword_mask = FA2_NHT_DA_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_NHT_DA_PRI_MASK;
    f.nxt_mask = FA2_DRV_NHT_DA_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_NHT_DA_SHIFT;
    fa2_copy_table_field((uint32_t *)(nhp->da), buffer, &f);

    return;
}

void fa2_create_tunltbl_entry(struct fa2_tunnel_tbl_entry *ttp, uint32_t *buffer)
{

    struct fa2_fld_info f;
    uint32_t val;

    val = ttp->action;
    FA2_SETBITS(buffer[FA2_TUNTBL_ACTION_LO_BIT >> 5], val, FA2_TUNTBL_ACTION_MASK, FA2_TUNTBL_ACTION_LO_BIT & 0x1F);

    val = ttp->dmac_idx;
    FA2_SETBITS(buffer[FA2_TUNTBL_DMAC_IDX_LO_BIT >> 5], val, FA2_TUNTBL_DMAC_IDX_MASK, FA2_TUNTBL_DMAC_IDX_LO_BIT & 0x1F);

    f.hi_bit = FA2_TUNTBL_SMAC_HI_BIT;
    f.lo_bit = FA2_TUNTBL_SMAC_LO_BIT;
    f.loword_mask = FA2_TUNTBL_SMAC_LO_WORD_MASK;
    f.hiword_mask = FA2_TUNTBL_SMAC_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_TUNTBL_SMAC_PRI_MASK;
    f.nxt_mask = FA2_DRV_TUNTBL_SMAC_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_TUNTBL_SMAC_SHIFT;
    fa2_copy_table_field((uint32_t *)(ttp->smac), buffer, &f);

    val = ttp->session_id;
    FA2_SETBITS(buffer[FA2_TUNTBL_SESSION_ID_LO_BIT >> 5], val, FA2_TUNTBL_SESSION_ID_MASK, FA2_TUNTBL_SESSION_ID_LO_BIT & 0x1F);

    return;
}

/* Make sure mac_addr present in rtmp is swapped correctly, because ls bits of
 * mac addr (31:0) go in to data0 reg, ms bits go in to data1 reg
 */
void fa2_create_rtmac_entry(struct fa2_rtmac_tbl_entry *rtmp, uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = rtmp->host;
    FA2_SETBITS(buffer[FA2_RTMAC_HOST_LO_BIT >> 5], val, FA2_RTMAC_HOST_MASK, FA2_RTMAC_HOST_LO_BIT & 0x1F);

    val = rtmp->l4_checksum_check;
    FA2_SETBITS(buffer[FA2_RTMAC_L4_CHECKSUM_CHECK_LO_BIT >> 5], val, FA2_RTMAC_L4_CHECKSUM_CHECK_MASK, FA2_RTMAC_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);

    val = rtmp->external;
    FA2_SETBITS(buffer[FA2_RTMAC_EXTERNAL_LO_BIT >> 5], val, FA2_RTMAC_EXTERNAL_MASK, FA2_RTMAC_EXTERNAL_LO_BIT & 0x1F);

    f.hi_bit = FA2_RTMAC_ROUTER_MAC_ADDR_HI_BIT;
    f.lo_bit = FA2_RTMAC_ROUTER_MAC_ADDR_LO_BIT;
    f.loword_mask = FA2_RTMAC_ROUTER_MAC_ADDR_LO_WORD_MASK;
    f.hiword_mask = FA2_RTMAC_ROUTER_MAC_ADDR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_PRI_MASK;
    f.nxt_mask = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_RTMAC_ROUTER_MAC_ADDR_SHIFT;
    fa2_copy_table_field((uint32_t *)(rtmp->mac_addr), buffer, &f);

    return;
}

void fa2_create_ipv4_entry (struct fa2_flow_entry_ipv4 *v4p, uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = v4p->ipv4_key_type;
    FA2_SETBITS(buffer[FA2_FE_V4_IPV4_KEY_TYPE_LO_BIT >> 5], val, FA2_FE_V4_IPV4_KEY_TYPE_MASK, FA2_FE_V4_IPV4_KEY_TYPE_LO_BIT & 0x1F);

    val = v4p->valid;
    FA2_SETBITS(buffer[FA2_FE_V4_VALID_LO_BIT >> 5], val, FA2_FE_V4_VALID_MASK, FA2_FE_V4_VALID_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V4_SIP_HI_BIT;
    f.lo_bit = FA2_FE_V4_SIP_LO_BIT;
    f.loword_mask = FA2_FE_V4_SIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_SIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_SIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_SIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_SIP_SHIFT;
    val = v4p->sip;
    fa2_copy_table_field(&val, buffer, &f);

    f.hi_bit = FA2_FE_V4_DIP_HI_BIT;
    f.lo_bit = FA2_FE_V4_DIP_LO_BIT;
    f.loword_mask = FA2_FE_V4_DIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_DIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_DIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_DIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_DIP_SHIFT;
    val = v4p->dip;
    fa2_copy_table_field(&val, buffer, &f);

    val = v4p->protocol;
    FA2_SETBITS(buffer[FA2_FE_V4_PROTO_LO_BIT >> 5], val, FA2_FE_V4_PROTO_MASK, FA2_FE_V4_PROTO_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V4_SPORT_HI_BIT;
    f.lo_bit = FA2_FE_V4_SPORT_LO_BIT;
    f.loword_mask = FA2_FE_V4_SPORT_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_SPORT_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_SPORT_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_SPORT_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_SPORT_SHIFT;
    val = v4p->sport;
    fa2_copy_table_field(&val, buffer, &f);

    val = v4p->dport;
    FA2_SETBITS(buffer[FA2_FE_V4_DPORT_LO_BIT >> 5], val, FA2_FE_V4_DPORT_MASK, FA2_FE_V4_DPORT_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V4_REVERSE_FLOW_PTR_HI_BIT;
    f.lo_bit = FA2_FE_V4_REVERSE_FLOW_PTR_LO_BIT;
    f.loword_mask = FA2_FE_V4_REVERSE_FLOW_PTR_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_REVERSE_FLOW_PTR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_REVERSE_FLOW_PTR_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_REVERSE_FLOW_PTR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_REVERSE_FLOW_PTR_SHIFT;
    val = v4p->rev_flow_ptr;
    fa2_copy_table_field(&val, buffer, &f);

    val = v4p->brcm_tag_opcode;
    FA2_SETBITS(buffer[FA2_FE_V4_BCMTAG_OPCODE_LO_BIT >> 5], val, FA2_FE_V4_BCMTAG_OPCODE_MASK, FA2_FE_V4_BCMTAG_OPCODE_LO_BIT & 0x1F);

    val = v4p->brcm_tag_tc;
    FA2_SETBITS(buffer[FA2_FE_V4_BCMTAG_TC_LO_BIT >> 5], val, FA2_FE_V4_BCMTAG_TC_MASK, FA2_FE_V4_BCMTAG_TC_LO_BIT & 0x1F);

    val = v4p->brcm_tag_te;
    FA2_SETBITS(buffer[FA2_FE_V4_BCMTAG_TE_LO_BIT >> 5], val, FA2_FE_V4_BCMTAG_TE_MASK, FA2_FE_V4_BCMTAG_TE_LO_BIT & 0x1F);

    val = v4p->brcm_tag_ts;
    FA2_SETBITS(buffer[FA2_FE_V4_BCMTAG_TS_LO_BIT >> 5], val, FA2_FE_V4_BCMTAG_TS_MASK, FA2_FE_V4_BCMTAG_TS_LO_BIT & 0x1F);

    val = v4p->brcm_tag_destmap;
    FA2_SETBITS(buffer[FA2_FE_V4_BCMTAG_DEST_MAP_LO_BIT >> 5], val, FA2_FE_V4_BCMTAG_DEST_MAP_MASK, FA2_FE_V4_BCMTAG_DEST_MAP_LO_BIT & 0x1F);

    val = v4p->direction;
    FA2_SETBITS(buffer[FA2_FE_V4_DIRECTION_LO_BIT >> 5], val, FA2_FE_V4_DIRECTION_MASK, FA2_FE_V4_DIRECTION_LO_BIT & 0x1F);

    val = v4p->l4_chksum_chk;
    FA2_SETBITS(buffer[FA2_FE_V4_L4_CHECKSUM_CHECK_LO_BIT >> 5], val, FA2_FE_V4_L4_CHECKSUM_CHECK_MASK, FA2_FE_V4_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);

    val = v4p->ppp_tunnel_en;
    FA2_SETBITS(buffer[FA2_FE_V4_PPP_TUNNEL_EN_LO_BIT >> 5], val, FA2_FE_V4_PPP_TUNNEL_EN_MASK, FA2_FE_V4_PPP_TUNNEL_EN_LO_BIT & 0x1F);


    val = v4p->ppp_tunnel_idx;
    FA2_SETBITS(buffer[FA2_FE_V4_PPP_TUNNEL_IDX_LO_BIT >> 5], val, FA2_FE_V4_PPP_TUNNEL_IDX_MASK, FA2_FE_V4_PPP_TUNNEL_IDX_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V4_MTU_IDX_HI_BIT;
    f.lo_bit = FA2_FE_V4_MTU_IDX_LO_BIT;
    f.loword_mask = FA2_FE_V4_MTU_IDX_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_MTU_IDX_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_MTU_IDX_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_MTU_IDX_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_MTU_IDX_SHIFT;
    val = v4p->mtu_idx;
    fa2_copy_table_field(&val, buffer, &f);

    val = v4p->next_hop_idx;
    FA2_SETBITS(buffer[FA2_FE_V4_NEXT_HOP_IDX_LO_BIT >> 5], val, FA2_FE_V4_NEXT_HOP_IDX_MASK, FA2_FE_V4_NEXT_HOP_IDX_LO_BIT & 0x1F);

    val = v4p->remap_sa_idx;
    FA2_SETBITS(buffer[FA2_FE_V4_REMAP_SA_IDX_LO_BIT >> 5], val, FA2_FE_V4_REMAP_SA_IDX_MASK, FA2_FE_V4_REMAP_SA_IDX_LO_BIT & 0x1F);

    val = v4p->dest_dma_chan;
    FA2_SETBITS(buffer[FA2_FE_V4_DEST_DMA_CHANNEL_LO_BIT >> 5], val, FA2_FE_V4_DEST_DMA_CHANNEL_MASK, FA2_FE_V4_DEST_DMA_CHANNEL_LO_BIT & 0x1F);

    val = v4p->action;
    FA2_SETBITS(buffer[FA2_FE_V4_ACTION_LO_BIT >> 5], val, FA2_FE_V4_ACTION_MASK, FA2_FE_V4_ACTION_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V4_HITS_HI_BIT;
    f.lo_bit = FA2_FE_V4_HITS_LO_BIT;
    f.loword_mask = FA2_FE_V4_HITS_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V4_HITS_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V4_HITS_PRI_MASK;
    f.nxt_mask = FA2_DRV_V4_HITS_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V4_HITS_SHIFT;
    val = v4p->hits;
    fa2_copy_table_field(&val, buffer, &f);

    val = v4p->tcp_fin;
    FA2_SETBITS(buffer[FA2_FE_V4_TCP_FIN_LO_BIT >> 5], val, FA2_FE_V4_TCP_FIN_MASK, FA2_FE_V4_TCP_FIN_LO_BIT & 0x1F);

    val = v4p->tcp_rst;
    FA2_SETBITS(buffer[FA2_FE_V4_TCP_RST_LO_BIT >> 5], val, FA2_FE_V4_TCP_RST_MASK, FA2_FE_V4_TCP_RST_LO_BIT & 0x1F);

    val = v4p->tcp_ack_after_close;
    FA2_SETBITS(buffer[FA2_FE_V4_TCP_ACK_AFTER_CLOSE_LO_BIT >> 5], val, FA2_FE_V4_TCP_ACK_AFTER_CLOSE_MASK, FA2_FE_V4_TCP_ACK_AFTER_CLOSE_LO_BIT & 0x1F);

    val = v4p->hit_after_close;
    FA2_SETBITS(buffer[FA2_FE_V4_HIT_AFTER_CLOSE_LO_BIT >> 5], val, FA2_FE_V4_HIT_AFTER_CLOSE_MASK, FA2_FE_V4_HIT_AFTER_CLOSE_LO_BIT & 0x1F);

    val = v4p->flow_state;
    FA2_SETBITS(buffer[FA2_FE_V4_FLOW_STATE_LO_BIT >> 5], val, FA2_FE_V4_FLOW_STATE_MASK, FA2_FE_V4_FLOW_STATE_LO_BIT & 0x1F);

    val = v4p->flow_timer;
    FA2_SETBITS(buffer[FA2_FE_V4_FLOW_TIMER_LO_BIT >> 5], val, FA2_FE_V4_FLOW_TIMER_MASK, FA2_FE_V4_FLOW_TIMER_LO_BIT & 0x1F);

    return;
}

void fa2_create_ipv6_w1_flow_entry (struct fa2_flow_entry_ipv6_w1 *w1p,
                                    uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = w1p->ipv4_key_type;
    FA2_SETBITS(buffer[FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_LO_BIT >> 5], val, FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_MASK, FA2_FE_V6_W1_IPV4_KEY_TYPE_W1_LO_BIT & 0x1F);

    val = w1p->valid;
    FA2_SETBITS(buffer[FA2_FE_V6_W1_VALID_LO_BIT >> 5], val, FA2_FE_V6_W1_VALID_MASK, FA2_FE_V6_W1_VALID_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W1_SIP_HI_BIT;
    f.lo_bit = FA2_FE_V6_W1_SIP_LO_BIT;
    f.loword_mask = FA2_FE_V6_W1_SIP_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W1_SIP_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W1_SIP_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W1_SIP_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W1_SIP_SHIFT;
    fa2_copy_table_field((uint32_t *)(w1p->sip), buffer, &f);

    f.hi_bit = FA2_FE_V6_W1_DIP_UPPER_HI_BIT;
    f.lo_bit = FA2_FE_V6_W1_DIP_UPPER_LO_BIT;
    f.loword_mask = FA2_FE_V6_W1_DIP_UPPER_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W1_DIP_UPPER_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W1_DIP_UPPER_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W1_DIP_UPPER_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W1_DIP_UPPER_SHIFT;
    fa2_copy_table_field((uint32_t *)(w1p->dip_upper), buffer, &f);

    return;
}
/* 
 * This function creates ipv6 word0 entry as per table #20.
 * buffer[0] will have least significant bits of the flow entry (starting with
 * flow timer) buffer[6] will have most significant bits of the flow entry
 */
void fa2_create_ipv6_w0_flow_entry (struct fa2_flow_entry_ipv6_w0 *w0p,
                                    uint32_t *buffer)
{
    struct fa2_fld_info f;
    uint32_t val;

    val = w0p->ipv4_key_type;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_LO_BIT >> 5], val, FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_MASK, FA2_FE_V6_W0_IPV4_KEY_TYPE_W0_LO_BIT & 0x1F);

    val = w0p->valid;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_VALID_LO_BIT >> 5], val, FA2_FE_V6_W0_VALID_MASK, FA2_FE_V6_W0_VALID_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W0_DIP_LOWER_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_DIP_LOWER_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_DIP_LOWER_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_DIP_LOWER_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_DIP_LOWER_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_DIP_LOWER_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_DIP_LOWER_SHIFT;
    fa2_copy_table_field((uint32_t *)(w0p->dip_lower), buffer, &f);

    val = w0p->protocol;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_PROTO_LO_BIT >> 5], val, FA2_FE_V6_W0_PROTO_MASK, FA2_FE_V6_W0_PROTO_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W0_SPORT_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_SPORT_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_SPORT_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_SPORT_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_SPORT_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_SPORT_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_SPORT_SHIFT;
    val = w0p->sport;
    fa2_copy_table_field(&val, buffer, &f);

    val = w0p->dport;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_DPORT_LO_BIT >> 5], val, FA2_FE_V6_W0_DPORT_MASK, FA2_FE_V6_W0_DPORT_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W0_REVERSE_FLOW_PTR_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_REVERSE_FLOW_PTR_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_REVERSE_FLOW_PTR_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_REVERSE_FLOW_PTR_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_REVERSE_FLOW_PTR_SHIFT;
    val = w0p->rev_flow_ptr;
    fa2_copy_table_field(&val, buffer, &f);

    val = w0p->brcm_tag_opcode;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_BCMTAG_OPCODE_LO_BIT >> 5], val, FA2_FE_V6_W0_BCMTAG_OPCODE_MASK, FA2_FE_V6_W0_BCMTAG_OPCODE_LO_BIT & 0x1F);

    val = w0p->brcm_tag_tc;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TC_LO_BIT >> 5], val, FA2_FE_V6_W0_BCMTAG_TC_MASK, FA2_FE_V6_W0_BCMTAG_TC_LO_BIT & 0x1F);

    val = w0p->brcm_tag_te;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TE_LO_BIT >> 5], val, FA2_FE_V6_W0_BCMTAG_TE_MASK, FA2_FE_V6_W0_BCMTAG_TE_LO_BIT & 0x1F);

    val = w0p->brcm_tag_ts;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_BCMTAG_TS_LO_BIT >> 5], val, FA2_FE_V6_W0_BCMTAG_TS_MASK, FA2_FE_V6_W0_BCMTAG_TS_LO_BIT & 0x1F);

    val = w0p->brcm_tag_destmap;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_BCMTAG_DEST_MAP_LO_BIT >> 5], val, FA2_FE_V6_W0_BCMTAG_DEST_MAP_MASK, FA2_FE_V6_W0_BCMTAG_DEST_MAP_LO_BIT & 0x1F);

    val = w0p->direction;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_DIRECTION_LO_BIT >> 5], val, FA2_FE_V6_W0_DIRECTION_MASK, FA2_FE_V6_W0_DIRECTION_LO_BIT & 0x1F);

    val = w0p->l4_chksum_chk;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_L4_CHECKSUM_CHECK_LO_BIT >> 5], val, FA2_FE_V6_W0_L4_CHECKSUM_CHECK_MASK, FA2_FE_V6_W0_L4_CHECKSUM_CHECK_LO_BIT & 0x1F);

    val = w0p->ppp_tunnel_en;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_PPP_TUNNEL_EN_LO_BIT >> 5], val, FA2_FE_V6_W0_PPP_TUNNEL_EN_MASK, FA2_FE_V6_W0_PPP_TUNNEL_EN_LO_BIT & 0x1F);

    val = w0p->ppp_tunnel_idx;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_PPP_TUNNEL_IDX_LO_BIT >> 5], val, FA2_FE_V6_W0_PPP_TUNNEL_IDX_MASK, FA2_FE_V6_W0_PPP_TUNNEL_IDX_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W0_MTU_IDX_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_MTU_IDX_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_MTU_IDX_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_MTU_IDX_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_MTU_IDX_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_MTU_IDX_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_MTU_IDX_SHIFT;
    val = w0p->mtu_idx;
    fa2_copy_table_field(&val, buffer, &f);

    val = w0p->next_hop_idx;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_NEXT_HOP_IDX_LO_BIT >> 5], val, FA2_FE_V6_W0_NEXT_HOP_IDX_MASK, FA2_FE_V6_W0_NEXT_HOP_IDX_LO_BIT & 0x1F);

    val = w0p->remap_sa_idx;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_REMAP_SA_IDX_LO_BIT >> 5], val, FA2_FE_V6_W0_REMAP_SA_IDX_MASK, FA2_FE_V6_W0_REMAP_SA_IDX_LO_BIT & 0x1F);

    val = w0p->dest_dma_chan;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_DEST_DMA_CHANNEL_LO_BIT >> 5], val, FA2_FE_V6_W0_DEST_DMA_CHANNEL_MASK, FA2_FE_V6_W0_DEST_DMA_CHANNEL_LO_BIT & 0x1F);

    val = w0p->action;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_ACTION_LO_BIT >> 5], val, FA2_FE_V6_W0_ACTION_MASK, FA2_FE_V6_W0_ACTION_LO_BIT & 0x1F);

    f.hi_bit = FA2_FE_V6_W0_HITS_HI_BIT;
    f.lo_bit = FA2_FE_V6_W0_HITS_LO_BIT;
    f.loword_mask = FA2_FE_V6_W0_HITS_LO_WORD_MASK;
    f.hiword_mask = FA2_FE_V6_W0_HITS_HI_WORD_MASK;
    f.pri_mask = FA2_DRV_V6_W0_HITS_PRI_MASK;
    f.nxt_mask = FA2_DRV_V6_W0_HITS_NXT_MASK;
    f.nxtmask_shift = FA2_DRV_V6_W0_HITS_SHIFT;
    val = w0p->hits;
    fa2_copy_table_field(&val, buffer, &f);

    val = w0p->tcp_fin;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_TCP_FIN_LO_BIT >> 5], val, FA2_FE_V6_W0_TCP_FIN_MASK, FA2_FE_V6_W0_TCP_FIN_LO_BIT & 0x1F);

    val = w0p->tcp_rst;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_TCP_RST_LO_BIT >> 5], val, FA2_FE_V6_W0_TCP_RST_MASK, FA2_FE_V6_W0_TCP_RST_LO_BIT & 0x1F);

    val = w0p->tcp_ack_after_close;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_LO_BIT >> 5], val, FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_MASK, FA2_FE_V6_W0_TCP_ACK_AFTER_CLOSE_LO_BIT & 0x1F);

    val = w0p->hit_after_close;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_HIT_AFTER_CLOSE_LO_BIT >> 5], val, FA2_FE_V6_W0_HIT_AFTER_CLOSE_MASK, FA2_FE_V6_W0_HIT_AFTER_CLOSE_LO_BIT & 0x1F);

    val = w0p->flow_state;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_FLOW_STATE_LO_BIT >> 5], val, FA2_FE_V6_W0_FLOW_STATE_MASK, FA2_FE_V6_W0_FLOW_STATE_LO_BIT & 0x1F);

    val = w0p->flow_timer;
    FA2_SETBITS(buffer[FA2_FE_V6_W0_FLOW_TIMER_LO_BIT >> 5], val, FA2_FE_V6_W0_FLOW_TIMER_MASK, FA2_FE_V6_W0_FLOW_TIMER_LO_BIT & 0x1F);

    return;
}

/*
 * This function takes an argument for the table to read, or write. It is for
 * testing purposes only, do not call it from main code
 */
void fa2_debug_tbl_acc(enum fa2_tbl_num table_id)
{
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];
    struct fa2_flow_entry_ipv6_w0 ipv6_w0;
/*
    struct fa2_flow_entry_ipv6_w1 ipv6_w1;
    struct fa2_flow_entry_ipv4 ipv4;
    struct fa2_rtmac_tbl_entry rtm;
    struct fa2_tunnel_tbl_entry tt;
    struct fa2_nh_tbl_entry nh;
 */
    int i;

    fa2_fill_ipv6_entry_w0(&ipv6_w0);
    //fa2_fill_ipv6_entry_w1(&ipv6_w1);
    //fa2_fill_ipv4_entry(&ipv4);
    //fa2_fill_rtmac_entry(&rtm);
    //fa2_fill_tunltbl_entry(&tt);
    //fa2_fill_nht_entry(&nh);

    memset(val, 0, sizeof(val));

    fa2_create_ipv6_w0_flow_entry(&ipv6_w0, val);
    //fa2_create_ipv6_w1_flow_entry(&ipv6_w1, val);
    //fa2_create_ipv4_entry(&ipv4, val);
    //fa2_create_rtmac_entry(&rtm, val);
    //fa2_create_tunltbl_entry(&tt, val);
    //fa2_create_nht_entry(&nh, val);

    /* Zero out before reading */
    memset(&ipv6_w0, 0, sizeof(ipv6_w0));
    //memset(&ipv6_w1, 0, sizeof(ipv6_w1));
    //memset(&ipv4, 0, sizeof(ipv4));
    //memset(&rtm, 0, sizeof(rtm));
    //memset(&tt, 0, sizeof(tt));
    //memset(&nh, 0, sizeof(nh));

    fa2_get_ipv6_w0_flow_entry(&ipv6_w0, val);
    //fa2_get_ipv6_w1_flow_entry (&ipv6_w1, val);
    //fa2_get_ipv4_entry (&ipv4, val);
    //fa2_get_rtmac_entry(&rtm, val);
    //fa2_get_tunltbl_entry(&tt, val);
    //fa2_get_nht_entry(&nh, val);


    printk(KERN_INFO "\n              2  22  1 1      1 1      1 1      0 0      0 0      0 0      0");
    printk(KERN_INFO "\n              2  00  9 9      6 5      2 2      9 9      6 6      3 3      0");
    printk(KERN_INFO "\n              3  87  2 1      0 9      8 7      6 5      4 3      2 1      0\n\n");
    printk(KERN_INFO "val[6-0] = 0x");
    for (i = FA2_MAX_TBL_ENT_WIDTH - 1; i >= 0; i--)
    {
        printk(KERN_INFO " %08X", val[i]);
    }

    fa2_display_ipv6_w0_info(&ipv6_w0);
    //fa2_display_ipv6_w1_info(&ipv6_w1);
    //fa2_display_ipv4_info(&ipv4);
    //fa2_display_rtmac_entry(&rtm);
    //fa2_display_tunltbl_entry(&tt);
    //fa2_display_nh_entry(&nh);

}


/* val points to array of size FA2_MAX_TBL_ENT_WIDTH (7). Unused locations
 * may be set to 0.
 * index is the location to write data to. Caller must ensure a valid value
 * is passed, depending on the table being accessed 
 * t: table id being accessed
 */
int fa2_indirect_write(enum fa2_tbl_num t, int index, uint32_t *val)
{
    int ret = FA2_SUCCESS;
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;
    enum fa2_tbl_sel tbl;
    volatile int retries;
    volatile int entry_size;

    /* Check if a indirect operation is already in progress */
    reg_addr = fa2_virt_addr + CTF_MEM_ACC_CONTROL_REG_OFF;
    reg_val = ioread32(reg_addr);

    if (reg_val & FA2_MEM_ACC_BUSY) {
        printk(KERN_INFO "\n%s: Previous indirect operation not complete,"
               " aborting, reg 0x4 = 0x%08X\n",
               __func__, reg_val);
        return FA2_ABORT; /* Abort indirect operation */
    }

    /* Map table # to value used by h/w */
    tbl = FA2_SEL_TBL(t);

    if (tbl == FA2_SEL_INVALID) {
        printk(KERN_INFO "\n%s: Invalid table id passed %d, aborting\n",
               __func__, t);
        return FA2_ABORT; /* Abort indirect operation */
    }

    entry_size = FA2_TBL_ENT_SZ(t);

    if (entry_size == FA2_INVALID_ENTRY_SIZE) {
        printk(KERN_INFO "\n%s: Invalid table size for table id %d, aborting\n",
               __func__, t);
        return FA2_ABORT; /* Abort indirect operation */
    }

    printk(KERN_DEBUG "\n%s: val[0]=0x%08X, val[1] = 0x%08X, val[2] = 0x%08X, val[3] = 0x%08X, val[4] = 0x%08X, val[5] = 0x%08X, val[6] = 0x%08X\n", __func__, val[0], val[1], val[2], val[3], val[4], val[5], val[6]);

    printk(KERN_DEBUG "\ntbl=%d, entry_size=%d, index=%d\n", tbl, entry_size, index);

    /*
     * Note: For table entries less than 7 words wide, we write 0s in the 
     * unused words
     */ 
    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA0_REG_OFF;
    reg_val = val[0];
    iowrite32(reg_val, reg_addr);

    /* Note: Unused registers set to 0 */
    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA1_REG_OFF;
    reg_val = (--entry_size > 0) ? val[1] : 0x0;
    iowrite32(reg_val, reg_addr);

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA2_REG_OFF;
    reg_val = (--entry_size > 0) ? val[2] : 0x0;
    iowrite32(reg_val, reg_addr);

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA3_REG_OFF;
    reg_val = (--entry_size > 0) ? val[3] : 0x0;
    iowrite32(reg_val, reg_addr);

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA4_REG_OFF;
    reg_val = (--entry_size > 0) ? val[4] : 0x0;
    iowrite32(reg_val, reg_addr);

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA5_REG_OFF;
    reg_val = (--entry_size > 0) ? val[5] : 0x0;
    iowrite32(reg_val, reg_addr);

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA6_REG_OFF;
    reg_val = (--entry_size > 0) ? val[6] : 0x0;
    iowrite32(reg_val, reg_addr);

    /* Initiate write */
    reg_addr = fa2_virt_addr + CTF_MEM_ACC_CONTROL_REG_OFF;
    reg_val = ioread32(reg_addr);
    FA2_SETBITS(reg_val, index, FA2_TABLE_INDEX, CTF_MEM_ACC_CONTROL__ALL_R);
    FA2_SETBITS(reg_val, tbl, FA2_TABLE_SELECT, CTF_MEM_ACC_CONTROL__TABLE_SELECT_R);
    FA2_SETBITS(reg_val, FA2_TBL_WR, FA2_RD_WR_N, CTF_MEM_ACC_CONTROL__RD_WR_N);

    /* Initiate indirect write operation */
    reg_val |= FA2_DO_ACCESS;

    iowrite32(reg_val, reg_addr);

    /* Note: reg_addr same as above value */

    retries = 0;

    do {
        reg_val = ioread32(reg_addr);
        retries++;
    } while ((reg_val & FA2_MEM_ACC_BUSY) && (retries < 32));

    if (retries >= 32) {
	    printk(KERN_INFO "%s: ERROR: Write access did not complete after %d"
               " polls. Reg 0x4 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        ret = FA2_ERROR;
    }

    return ret;
}

int fa2_set_tbl_entry(enum fa2_tbl_num t, int index, void *tbl_data)
{
    int ret = FA2_SUCCESS;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];


    memset(val, 0x0, sizeof(val)); /* Must clear array */

    switch(t) {

        case FA2_FLTBL_V6_W1:
        
            fa2_create_ipv6_w1_flow_entry(
                (struct fa2_flow_entry_ipv6_w1 *)tbl_data , val);
            break;

        case FA2_FLTBL_V6_W0:

           fa2_create_ipv6_w0_flow_entry(
               (struct fa2_flow_entry_ipv6_w0 *)tbl_data, val);

            break;

        case FA2_FLTBL_V4:
            //printk(KERN_INFO "\nBefore calling create entry\n");
            //fa2_display_ipv4_info((struct fa2_flow_entry_ipv4 *)tbl_data);
            fa2_create_ipv4_entry((struct fa2_flow_entry_ipv4 *)tbl_data, val);
            break;

        case FA2_MTUTBL:
            /* Single field, no conversion reqd */
            val[0] = *((uint32_t *)tbl_data);
            break;

        case FA2_PORTTYPTBL:
            /* Single field, no conversion reqd */
            val[0] = *((uint32_t *)tbl_data);
            break;

        case FA2_NXTHOPTBL:
            fa2_create_nht_entry((struct fa2_nh_tbl_entry *)tbl_data, val);
            break;

        case FA2_RTMACTBL:
            fa2_create_rtmac_entry((struct fa2_rtmac_tbl_entry *)tbl_data, val);
            break;

        case FA2_TUNLTBL:
            fa2_create_tunltbl_entry(
                (struct fa2_tunnel_tbl_entry *)tbl_data, val);
            break;

        default:
            printk(KERN_DEBUG "%s: Write: invalid table id passed %d, aborting",
                   __func__, t);
            return(FA2_ABORT);
            //break;

    }

    if (fa2_indirect_write(t, index, val) != FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Indirect write failed, table=%d, index=%d\n",
               __func__, t, index);
        ret = FA2_ERROR;
    }

    return(ret);
}

/* val points to array of size FA2_MAX_TBL_ENT_WIDTH (7). Unused locations
 * may be set to 0.
 * index is the location to write data to. Caller must ensure a valid value
 * is passed, depending on the table being accessed 
 * t: table id being accessed
 */
int fa2_indirect_read(enum fa2_tbl_num t, int index, uint32_t *val)
{
	volatile void __iomem		*reg_addr;
	volatile uint32_t	        reg_val;
    volatile int retries;
    int ret = FA2_SUCCESS;
    enum fa2_tbl_sel tbl;
    volatile int entry_size;

    /* Check if a indirect operation is in progress */
    reg_addr = fa2_virt_addr + CTF_MEM_ACC_CONTROL_REG_OFF;
    reg_val = ioread32(reg_addr);

    if (reg_val & FA2_MEM_ACC_BUSY) {
        printk(KERN_INFO "\n%s: Previous indirect operation not complete,"
               " aborting, reg 0x4 = 0x%08X\n",
               __func__, reg_val);
        return FA2_ABORT; /* Abort indirect operation */
    }

    /* Map table # to value used by h/w */
    tbl = FA2_SEL_TBL(t);

    if (tbl == FA2_SEL_INVALID) {
        printk(KERN_INFO "\n%s: Invalid table id passed %d, aborting\n",
               __func__, t);

        return FA2_ABORT; /* Abort indirect operation */
    }

    entry_size = FA2_TBL_ENT_SZ(t);

    if (entry_size == FA2_INVALID_ENTRY_SIZE) {
        printk(KERN_INFO "\n%s: Invalid table size for table id %d, aborting\n",
               __func__, t);
        return FA2_ABORT; /* Abort indirect operation */
    }

    reg_addr = fa2_virt_addr + CTF_MEM_ACC_CONTROL_REG_OFF;
    reg_val = ioread32(reg_addr);

    FA2_SETBITS(reg_val, index, FA2_TABLE_INDEX, CTF_MEM_ACC_CONTROL__ALL_R);
    FA2_SETBITS(reg_val, tbl, FA2_TABLE_SELECT, CTF_MEM_ACC_CONTROL__TABLE_SELECT_R);
    FA2_SETBITS(reg_val, FA2_TBL_RD, FA2_RD_WR_N, CTF_MEM_ACC_CONTROL__RD_WR_N);

    /* Initiate indirect read operation */
    reg_val |= FA2_DO_ACCESS;

    iowrite32(reg_val, reg_addr);

    /* Note: reg_addr same as above value */

    retries = 0;

    do {
        reg_val = ioread32(reg_addr);
        retries++;
    } while ((reg_val & FA2_MEM_ACC_BUSY) && (retries < 32));

    if (retries >= 32) {
	    printk(KERN_INFO "%s: ERROR: Read access did not complete after %d"
               " polls. Reg 0x4 = 0x%08X. Exiting. \n",
               __func__, retries, reg_val);

        ret = FA2_ERROR;
    }
    else
    {

        /*
         * Note: For table entries less than 7 words wide, we still read the 
         * unused words(registers)
         */ 
        reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA0_REG_OFF;
        reg_val = ioread32(reg_addr);
        val[0] = reg_val;

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA1_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[1] = reg_val;
        }

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA2_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[2] = reg_val;
        }

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA3_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[3] = reg_val;
        }

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA4_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[4] = reg_val;
        }

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA5_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[5] = reg_val;
        }

        if (--entry_size > 0) {
            reg_addr = fa2_virt_addr + CTF_MEM_ACC_DATA6_REG_OFF;
            reg_val = ioread32(reg_addr);
            val[6] = reg_val;
        }
    }

    return ret;
}

int fa2_get_tbl_entry(enum fa2_tbl_num t, int index, void *tbl_data)
{
    int ret = FA2_SUCCESS;
    uint32_t val[FA2_MAX_TBL_ENT_WIDTH];

    memset(val, 0x0, sizeof(val)); /* Must clear array */

    if (fa2_indirect_read(t, index, val) != FA2_SUCCESS) {
        printk(KERN_DEBUG "\n%s: Indirect read failed, table=%d, index=%d\n",
               __func__, t, index);
        return(FA2_ERROR);
    }

    switch(t) {

        case FA2_FLTBL_V6_W1:
            {
                struct fa2_flow_entry_ipv6_w1 *ipv6_w1;

                ipv6_w1 = (struct fa2_flow_entry_ipv6_w1 *)tbl_data;

                memset((void *)ipv6_w1, 0, sizeof(struct fa2_flow_entry_ipv6_w1));
                fa2_get_ipv6_w1_flow_entry(ipv6_w1, val);

            }
        
            break;

        case FA2_FLTBL_V6_W0:
            {
                struct fa2_flow_entry_ipv6_w0 *ipv6_w0;

                ipv6_w0 = (struct fa2_flow_entry_ipv6_w0 *)tbl_data;

                memset((void *)ipv6_w0, 0, sizeof(struct fa2_flow_entry_ipv6_w0));
                fa2_get_ipv6_w0_flow_entry(ipv6_w0, val);
            }

            break;

        case FA2_FLTBL_V4:
            {
                struct fa2_flow_entry_ipv4 *ipv4;

                ipv4 = (struct fa2_flow_entry_ipv4 *)tbl_data;

                memset((void *)ipv4, 0, sizeof(struct fa2_flow_entry_ipv4));

                fa2_get_ipv4_entry (ipv4, val);
            }
            break;

        case FA2_MTUTBL:
            /* Single field, no conversion reqd */
            *((uint32_t *)tbl_data) = val[0];
            break;

        case FA2_PORTTYPTBL:
            /* Single field, no conversion reqd */
            *((uint32_t *)tbl_data) = val[0];
            break;

        case FA2_NXTHOPTBL:
            {
                struct fa2_nh_tbl_entry *nh;

                nh = (struct fa2_nh_tbl_entry *)tbl_data;

                memset((void *)nh, 0, sizeof(struct fa2_nh_tbl_entry));

                fa2_get_nht_entry(nh, val);
            }
            break;

        case FA2_RTMACTBL:
            {
                struct fa2_rtmac_tbl_entry *rtm;

                rtm = (struct fa2_rtmac_tbl_entry *)tbl_data;

                memset((void *)rtm, 0, sizeof(struct fa2_rtmac_tbl_entry));

                fa2_get_rtmac_entry(rtm, val);
            }
            break;

        case FA2_TUNLTBL:
            {
                struct fa2_tunnel_tbl_entry *tt;

                tt = (struct fa2_tunnel_tbl_entry *)tbl_data;

                memset((void *)tt, 0, sizeof(struct fa2_tunnel_tbl_entry));

                fa2_get_tunltbl_entry(tt, val);

            }
            break;

        default:
            printk(KERN_DEBUG "%s: Read: invalid table num passed %d",
                   __func__, t);
            ret = FA2_ERROR ;
            break;

    }

    return(ret);
}
