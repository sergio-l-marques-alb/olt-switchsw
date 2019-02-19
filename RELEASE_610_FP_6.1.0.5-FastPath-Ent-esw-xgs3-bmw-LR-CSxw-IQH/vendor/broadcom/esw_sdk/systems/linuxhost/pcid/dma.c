/*
 * $Id: dma.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:
 *	dma.c
 * Requires:
 *	soc_internal_memory_fetch/store
 *	soc_internal_send_int
 * Provides:
 *	pcid_dcb_fetch
 *	pcid_dcb_store
 *	pcid_dma_tx_start
 *	pcid_dma_rx_start
 *	pcid_dma_rx_check
 *	pcid_dma_start_chan
 *	pcid_dma_stat_write
 *	pcid_dma_ctrl_write
 */

#include <sys/types.h>
#include <soc/dcb.h>
#include <soc/dma.h>
#include <soc/higig.h>
#include <soc/pbsmh.h>

#include "pcid.h"
#include "cmicsim.h"

uint32
pcid_dcb_fetch(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb)
{
    int	size;

    debugk(DK_DMA, "Reading descriptor at 0x%08x\n", addr);

    size = SOC_DCB_SIZE(pcid_info->unit);
    soc_internal_memory_fetch(pcid_info, addr,
			      (void *)dcb,
			      size,
			      MF_ES_DMA_OTHER);

#ifdef BROADCOM_DEBUG
    if (debugk_check(DK_DMA)) {
	SOC_DCB_DUMP(pcid_info->unit, dcb, "PCID Fetch", 0);
    }
#endif /* BROADCOM_DEBUG */

    return addr + size;
}

uint32
pcid_dcb_store(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb)
{
    int	size;

    size = SOC_DCB_SIZE(pcid_info->unit);
    soc_internal_memory_store(pcid_info, addr,
			      (void *)dcb,
			      size,
			      MF_ES_DMA_OTHER);

#ifdef BROADCOM_DEBUG
    if (debugk_check(DK_DMA)) {
	SOC_DCB_DUMP(pcid_info->unit, dcb, "PCID Store", 1);
    }
#endif /* BROADCOM_DEBUG */

    return addr + size;
}

/*
 * Function:
 *	_pcid_loop_tx_cb
 * Purpose:
 *	Loop back packets transmitted from CPU
 * Parameters:
 *      pcid_info - Pointer to pcid control structure
 *      dcb - Pointer to DCB describing packet data
 *      chan - Channel on which packet is being transmitted
 */

void
_pcid_loop_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan)
{
    int			cnt;		/* Bytes in this piece of packet */
    int			eop;		/* End of packet indicator */
    uint8		*buf;		/* alloc'd for this piece of packet */
    int			unit;
    int                 pkt_pos;
    static uint8        pkt_data[4][PKT_SIZE_MAX];
    static uint32       dcbd[4][11];
#ifdef BCM_XGS3_SWITCH_SUPPORT
    uint32              hgf;
    uint32              opcode = 0xff;
    uint32              hgh_store[4];
    soc_higig_hdr_t     *hgh;
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    /* Accumulate length for each channel */
    static int len[N_DMA_CHAN] = {0, 0, 0, 0};
    static int tx_pkt_no = 1;
    static int tx_dcb_no = 0;

    unit = pcid_info->unit;
    eop = !SOC_DCB_SG_GET(unit, dcb);
    cnt = SOC_DCB_REQCOUNT_GET(unit, dcb);

    if ((SOC_DCB_TYPE(pcid_info->unit) >= 9) &&
        (SOC_DCB_TYPE(pcid_info->unit) <= 13)) {
        /* Copy module Header */
        if (len[chan] == 0) {
            uint32      *up;
            int         i;
            up = (uint32 *)dcb;
            up +=2; /* skip 2 words */
            for(i = 0; i < 8; i++) {
                dcbd[chan][i] = up[i];
            }
#ifdef BCM_XGS3_SWITCH_SUPPORT
            hgf = CMIC_PORT(unit);
            if (SOC_DCB_HG_GET(unit, dcb)) {
                hgh_store[0] = soc_htonl(dcbd[chan][0]);
                hgh_store[1] = soc_htonl(dcbd[chan][1]);
                hgh_store[2] = soc_htonl(dcbd[chan][2]);
                hgh_store[3] = soc_htonl(dcbd[chan][3]);
                hgh =  (soc_higig_hdr_t *)&hgh_store[0];

                switch (soc_higig_field_get(unit, hgh, HG_start)) {
                case 0xff:
                    hgf = soc_pbsmh_field_get(unit,
                                              (soc_pbsmh_hdr_t *)hgh,
                                              PBSMH_dst_port);
                    break;
                case 0xfb:
                case 0xfc:
                    hgf = soc_higig_field_get(unit, hgh, HG_dst_port);
                    soc_higig_field_set(unit, hgh, HG_src_port, hgf);
                    for(i = 0; i < 4; i++) {
                        dcbd[chan][i] = soc_ntohl(hgh_store[i]);
                    }
                    break;
                default:
                    break;
                }
            }
            SOC_DCB_RX_INGPORT_SET(unit, &dcbd[chan][-2], hgf);
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        }
    }
    pkt_pos = len[chan];
    len[chan] += cnt;

    buf = &pkt_data[chan][pkt_pos];
    if ((len[chan]) >= PKT_SIZE_MAX) {
	return;
    }

    soc_internal_bytes_fetch(pcid_info,
                             SOC_DCB_PADDR_GET(unit, dcb),
                             buf, cnt);
    tx_dcb_no++;

    if (eop) {
	tx_pkt_no++;
	tx_dcb_no = 0;
#ifdef BCM_XGS3_SWITCH_SUPPORT
        /*
         * Not a generic implementation. Intended only for LB test
         */
        if (SOC_IS_XGS3_SWITCH(unit)) {
            buf = &pkt_data[chan][0];
            hgh_store[0] = soc_htonl(dcbd[chan][0]);
            hgh_store[1] = soc_htonl(dcbd[chan][1]);
            hgh_store[2] = soc_htonl(dcbd[chan][2]);
            hgh_store[3] = soc_htonl(dcbd[chan][3]);
            hgh =  (soc_higig_hdr_t *)&hgh_store[0];

            if ((soc_higig_field_get(unit, hgh, HG_start) == 0xfb) ||
                (soc_higig_field_get(unit, hgh, HG_start) == 0xfc)) {
                opcode = soc_higig_field_get(unit, hgh, HG_opcode);
            } else {
                opcode = 0xff;
            }
            if ((opcode == 0) ||
                ((buf[12] != 0x81) || (buf[13] != 0x00))) {
                uint16 vtag;
                int i;

                vtag = soc_higig_field_get(pcid_info->unit,
                                           hgh,
                                           HG_vlan_tag);
                for(i = len[chan] - 1; i >= 12; i--) {
                    buf[i + 4] = buf[i];
                }
                buf[12] = 0x81;
                buf[13] = 0x00;
                buf[14] = vtag >> 8;
                buf[15] = vtag & 0xff;
                len[chan] += 4;
            }
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
        if (!(SOC_IS_XGS3_SWITCH(unit) &&
             SOC_DCB_PURGE_GET(unit, dcb))) {
            pcid_add_pkt(pcid_info,
                         &pkt_data[chan][0],
                         len[chan],
                         &dcbd[chan][0]);
        }
        len[chan] = 0;
    }
}

/*
 * Function:
 *	_pcid_default_tx_cb
 * Purpose:
 *	Default function for transmit callback.
 * Parameters:
 *      pcid_info - Pointer to pcid control structure
 *      dcb - Pointer to DCB describing packet data
 *      chan - Channel on which packet is being transmitted
 */

static void
_pcid_default_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan)
{
    int			cnt;		/* Bytes in this piece of packet */
    int			i, j;
    int			eop;		/* End of packet indicator */
    uint8		*buf;		/* alloc'd for this piece of packet */
    int			unit;
    char		prefix[64];

    /* Accumulate length for each channel */
    static int len[N_DMA_CHAN] = {0, 0, 0, 0};
    static int tx_pkt_no = 1;
    static int tx_dcb_no = 0;

    unit = pcid_info->unit;
    eop = !SOC_DCB_SG_GET(unit, dcb);
    cnt = SOC_DCB_REQCOUNT_GET(unit, dcb);

    len[chan] += cnt;

    if ((buf = sal_alloc(cnt, "txcbb")) == NULL) {
	return;		/* D'oh! */
    }

    soc_internal_bytes_fetch(pcid_info,
                             SOC_DCB_PADDR_GET(unit, dcb),
                             buf, cnt);

    sprintf(prefix, "TX %d\tDCB[%d]", tx_pkt_no, tx_dcb_no);
#ifdef BROADCOM_DEBUG
    SOC_DCB_DUMP(pcid_info->unit, dcb, prefix, 1);
#endif /* BROADCOM_DEBUG */

    tx_dcb_no++;

    for (i = 0; i < cnt; i += 16) {
	printk("\tdata[%04x]=", i);

	for (j = 0; j < 16 && i + j < cnt; j++) {
	    printk("%02x ", buf[i + j]);
	}

	printk("\n");
    }

    if (eop) {
        printk("TX %d\tEnd %d-byte packet\n"
	       "---------------------------------"
	       "---------------------------------\n",
	       tx_pkt_no, len[chan]);
	tx_pkt_no++;
	tx_dcb_no = 0;
        len[chan] = 0;
    }

    sal_free(buf);
}

void
pcid_dma_tx_start(pcid_info_t *pcid_info, int ch)
{
    uint32 dcb_addr, dcb_addr_next;
    dcb_t *dcb;

    assert(ch >= 0 && ch < N_DMA_CHAN);

    dcb_addr = PCIM(pcid_info, CMIC_DMA_DESC(ch));

    debugk(DK_DMA, "pcid_dma_tx_start: dcb_addr=0x%08x\n", dcb_addr);

    dcb = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "txs_dcb");
    if (dcb == NULL) {
	return;		/* Yow! */
    }
    do {
	dcb_addr_next = pcid_dcb_fetch(pcid_info, dcb_addr, dcb);

	if (SOC_DCB_RELOAD_GET(pcid_info->unit, dcb) &&
	    (PCIM(pcid_info, CMIC_CONFIG) & CC_RLD_OPN_EN)) {
	    dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit, dcb);
	} else { /* Check for call back function */
            if (pcid_info->tx_cb) {
                (pcid_info->tx_cb)(pcid_info, dcb, ch);
            } else { /* Call back not defined; use default */
                _pcid_default_tx_cb(pcid_info, dcb, ch);
            }
        }

	/* Update descriptor in memory */

	SOC_DCB_DONE_SET(pcid_info->unit, dcb, 1);
        SOC_DCB_XFERCOUNT_SET(pcid_info->unit, dcb,
                              SOC_DCB_REQCOUNT_GET(pcid_info->unit, dcb));
	pcid_dcb_store(pcid_info, dcb_addr, dcb);

	dcb_addr = dcb_addr_next;

	PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DESC_DONE_TST(ch);

	PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_DESC_DONE(ch);
	/* send_int(); */
    } while (SOC_DCB_CHAIN_GET(pcid_info->unit, dcb));

    /* Notify completion */

    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_CHAIN_DONE_TST(ch);
    PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(ch);

    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_CHAIN_DONE(ch);

    soc_internal_send_int(pcid_info);
    sal_free(dcb);
}

static uint32 		rx_dcb_addr[N_DMA_CHAN];
static dcb_t		*rx_dcb[N_DMA_CHAN];


void
pcid_dma_rx_start(pcid_info_t *pcid_info, int ch)
{
    /*
     * This notifies the pcid_dma_rx_check polling routine that a DCB is
     * executing.
     */

    rx_dcb_addr[ch] = PCIM(pcid_info, CMIC_DMA_DESC(ch));

    debugk(DK_DMA,
	   "pcid_dma_rx_start: ch=%d dcb=0x%08x\n",
	   ch, rx_dcb_addr[ch]);
}

void
pcid_dma_rx_check(pcid_info_t *pcid_info, int chan)
{
    uint32 		rx_dcb_addr_next;
    int 		cnt, len, xfer;
    int 		eop, drop_pkt = 0;
    uint32		dma_ctrl;

    assert(chan >= 0 && chan < N_DMA_CHAN);

    dma_ctrl = PCIM(pcid_info, CMIC_DMA_CTRL);

    if ((dma_ctrl & DC_DIRECTION_MASK(chan)) != DC_SOC_TO_MEM(chan)) {
	return;		/* Channel not configured for RX */
    }

    if ((PCIM(pcid_info, CMIC_DMA_STAT) & DS_DMA_EN_TST(chan)) == 0) {
	return;		/* Channel not enabled */
    }

    if (rx_dcb_addr[chan] == 0) {
	return;		/* Should not happen */
    }

    

    debugk(DK_DMA, "pcid_dma_rx_check: dcb=0x%08x\n", rx_dcb_addr[chan]);

    if (rx_dcb[chan] == NULL) {
	rx_dcb[chan] = sal_alloc(SOC_DCB_SIZE(pcid_info->unit), "rxc_dcb");
	if (rx_dcb[chan] == NULL) {
	    return;	/* Yow! */
	}
    }
    rx_dcb_addr_next = pcid_dcb_fetch(pcid_info, rx_dcb_addr[chan],
                                      rx_dcb[chan]);

    if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan]) &&
        (PCIM(pcid_info, CMIC_CONFIG) & CC_RLD_OPN_EN)) {
	rx_dcb_addr_next = SOC_DCB_PADDR_GET(pcid_info->unit,
					     rx_dcb[chan]);

	debugk(DK_DMA,
	       "pcid_dma_rx_check: reload to 0x%08x\n",
	       rx_dcb_addr_next);
    } else {
        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        if (pcid_info->pkt_list == 0) {
            sal_mutex_give(pcid_info->pkt_mutex);
            return;		/* No pending packets anyway */
        }
	eop = !SOC_DCB_SG_GET(pcid_info->unit, rx_dcb[chan]);
	cnt = SOC_DCB_REQCOUNT_GET(pcid_info->unit, rx_dcb[chan]);

	len = pcid_info->pkt_list->length - pcid_info->pkt_list->consum;

	debugk(DK_DMA,
	       "pcid_dma_rx_check: eop=%d cnt=%d length=%d consum=%d len=%d\n",
	       eop, cnt, pcid_info->pkt_list->length,
	       pcid_info->pkt_list->consum, len);

	/* Clear status */
	SOC_DCB_STATUS_INIT(pcid_info->unit, rx_dcb[chan]);

	if (len <= cnt) { /* Room to receive whole packet in current DCB */
	    xfer = len;
	    SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], 1);
	    drop_pkt = 1;
            eop = 1;
	} else {	/* Part of packet is received */
	    xfer = cnt;
	    drop_pkt = eop;
	}
	SOC_DCB_XFERCOUNT_SET(pcid_info->unit, rx_dcb[chan], xfer);
	SOC_DCB_RX_START_SET(pcid_info->unit, rx_dcb[chan],
				    (pcid_info->pkt_list->consum == 0));
	SOC_DCB_RX_ERROR_SET(pcid_info->unit, rx_dcb[chan], 0);
	SOC_DCB_RX_CRC_SET(pcid_info->unit, rx_dcb[chan], 1);
        SOC_DCB_RX_END_SET(pcid_info->unit, rx_dcb[chan], eop);
        if ((SOC_DCB_TYPE(pcid_info->unit) == 9) ||
            (SOC_DCB_TYPE(pcid_info->unit) == 10) ||
            (SOC_DCB_TYPE(pcid_info->unit) == 11) ||
            (SOC_DCB_TYPE(pcid_info->unit) == 12) ||
            (SOC_DCB_TYPE(pcid_info->unit) == 13)) {
            uint32      *up;
            int         i;
            up = rx_dcb[chan];
            up += 2; /* skip 2 words */
            for (i = 0; i < 8; i++) {
                up[i] = pcid_info->pkt_list->dcbd[i];
            }
        }

	soc_internal_memory_store(pcid_info,
                      SOC_DCB_PADDR_GET(pcid_info->unit, rx_dcb[chan]),
		      &pcid_info->pkt_list->data[pcid_info->pkt_list->consum],
		      xfer,
		      MF_ES_DMA_PACKET);
        /* Now indicate this part of packet is consumed */
        pcid_info->pkt_list->consum += xfer;
        sal_mutex_give(pcid_info->pkt_mutex);
    }

    SOC_DCB_DONE_SET(pcid_info->unit, rx_dcb[chan], 1);

    pcid_dcb_store(pcid_info, rx_dcb_addr[chan], rx_dcb[chan]);

    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DESC_DONE_TST(chan);

    PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_DESC_DONE(chan);

    if (SOC_DCB_CHAIN_GET(pcid_info->unit, rx_dcb[chan])) {
	rx_dcb_addr[chan] = rx_dcb_addr_next;
    } else {
        if (SOC_DCB_RELOAD_GET(pcid_info->unit, rx_dcb[chan])) {
            rx_dcb_addr[chan] = rx_dcb_addr_next;
        } else {
            rx_dcb_addr[chan] = 0;
        }

	/* Notify completion */

	PCIM(pcid_info, CMIC_DMA_STAT) |= DS_CHAIN_DONE_TST(chan);
	PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(chan);

	PCIM(pcid_info, CMIC_IRQ_STAT) |= IRQ_CHAIN_DONE(chan);
	PCIM(pcid_info, CMIC_IRQ_STAT) &= ~(IRQ_DESC_DONE(chan));
    }

    soc_internal_send_int(pcid_info);

    if (drop_pkt) {
	packet_t		*tmp;

        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
	tmp = pcid_info->pkt_list;
	pcid_info->pkt_list = pcid_info->pkt_list->next;
        pcid_info->pkt_count--;
        sal_mutex_give(pcid_info->pkt_mutex);
	sal_free(tmp);
    }
}

void
pcid_dma_start_chan(pcid_info_t *pcid_info, int ch)
{
    uint32 dma_ctrl;
    int tx, modbmp, abort, intrdesc, drop_tx;

    PCIM(pcid_info, CMIC_DMA_STAT) |= DS_DMA_ACTIVE(ch);

    dma_ctrl = PCIM(pcid_info, CMIC_DMA_CTRL);

    tx = ((dma_ctrl & DC_DIRECTION_MASK(ch)) == DC_MEM_TO_SOC(ch));
    modbmp = ((dma_ctrl & DC_MOD_BITMAP_MASK(ch)) == DC_MOD_BITMAP(ch));
    abort = ((dma_ctrl & DC_ABORT_DMA_MASK(ch)) == DC_ABORT_DMA(ch));
    intrdesc = ((dma_ctrl & DC_INTR_ON_MASK(ch)) == DC_INTR_ON_DESC(ch));
    drop_tx = ((dma_ctrl & DC_DROP_TX_MASK(ch)) == DC_DROP_TX(ch));

    debugk(DK_DMA,
	   "Starting DMA on channel %d\n", ch);

    debugk(DK_DMA, "Mode: %s %s %s %s %s\n",
	   tx ? "TX" : "RX",
	   modbmp ? "MOD_BMP" : "NO_MOD_BMP",
	   abort ? "ABORT" : "NO_ABORT",
	   intrdesc ? "INTR_ON_DESC" : "INTR_ON_PKT",
	   drop_tx ? "DROP_TX" : "NO_DROP_TX");

    if (tx) {
	pcid_dma_tx_start(pcid_info, ch);
    } else {
	pcid_dma_rx_start(pcid_info, ch);
    }
}

void
pcid_dma_stat_write(pcid_info_t *pcid_info, uint32 value)
{
    uint32	old_stat = PCIM(pcid_info, CMIC_DMA_STAT); /* Initial value */
    uint32	new_stat;
    int		ch;

    /*
     * In the hardware, the DMA is triggered by a couple conditions:
     *	1) DMA Enable == 1
     *	2) Chain_done == 0
     *
     * DMA must be triggered as soon as all of these conditions are true.
     */
    if (value & 0x80) {			/* Set a bit */
	new_stat = PCIM(pcid_info, CMIC_DMA_STAT) |= 1 << (value & 0x1f);
    } else {
	new_stat = PCIM(pcid_info, CMIC_DMA_STAT) &= ~(1 << (value & 0x1f));
    }

    /*
     * Now look at differences and descide what action to take. This
     * could be optimized since only one channel changed, but for now take
     * the easy approach.
     */

    for (ch = 0; ch < N_DMA_CHAN; ch++) {
	int	old_enable, old_chain_done, old_desc_done;
	int	new_enable, new_chain_done, new_desc_done;

	old_enable = DS_DMA_EN_TST(ch) & old_stat;
	old_chain_done = DS_CHAIN_DONE_TST(ch) & old_stat;
	old_desc_done = DS_DESC_DONE_TST(ch) & old_stat;

	new_enable = DS_DMA_EN_TST(ch) & new_stat;
	new_chain_done = DS_CHAIN_DONE_TST(ch) & new_stat;
	new_desc_done = DS_DESC_DONE_TST(ch) & new_stat;

	/*
	 * Check for interrupt changes - this MUST be before the DMA
	 * check below to avoid clearing status from a potentially
	 * started DMA.
	 */

	if (old_desc_done && !new_desc_done) { /* Clear interrupt */
	    PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_DESC_DONE(ch);
	}

	if (old_chain_done && !new_chain_done) { /* Clear interrupt */
	    PCIM(pcid_info, CMIC_IRQ_STAT) &= ~IRQ_CHAIN_DONE(ch);
	}

	/* Trigger a DMA ? */

	if ((!old_enable || old_chain_done) && /* Not running before */
	    (new_enable && !new_chain_done)) { /* Should start */
	    pcid_dma_start_chan(pcid_info, ch);
	}
    }
}

void
pcid_dma_ctrl_write(pcid_info_t *pcid_info, uint32 value)
{
    int		ch;

    PCIM(pcid_info, CMIC_DMA_CTRL) = value;

    for (ch = 0; ch < N_DMA_CHAN; ch++) {
	if (value & DC_ABORT_DMA(ch)) {
	    PCIM(pcid_info, CMIC_DMA_STAT) &= ~DS_DMA_ACTIVE(ch);
	}
    }
}

void
pcid_dma_stop_all(pcid_info_t *pcid_info) 
{
    int ch;

    printk("Abort and Disable  DMA\n");

    /* 
     * Abort and disable all DMA channels when the client exits so that
     * the PCID does not poll for the old DMA when the client connects again.
     */ 
    for (ch = 0; ch < N_DMA_CHAN; ch++) {
        pcid_dma_ctrl_write(pcid_info, DC_ABORT_DMA(ch)); /* Abort current DMA */
        pcid_dma_stat_write(pcid_info, (0x00 | ch));      /* Disable DMA */
        rx_dcb_addr[ch] = 0x0;                           
    }
}
