/*
 * $Id: dma.h,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        dma.h
 * Purpose:     
 */

#ifndef   _PCID_DMA_H_
#define   _PCID_DMA_H_

#include <soc/defs.h>
#include <soc/dcb.h>
#include <sys/types.h>

#include "pcid.h"

extern uint32 pcid_dcb_fetch(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb);
extern uint32 pcid_dcb_store(pcid_info_t *pcid_info, uint32 addr, dcb_t *dcb);
extern void pcid_dma_tx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_rx_start(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_rx_check(pcid_info_t *pcid_info, int chan);
extern void pcid_dma_start_chan(pcid_info_t *pcid_info, int ch);
extern void pcid_dma_stat_write(pcid_info_t *pcid_info, uint32 value);
extern void pcid_dma_ctrl_write(pcid_info_t *pcid_info, uint32 value);
extern void pcid_dma_stop_all(pcid_info_t *pcid_info);

/* Call back function type for tx */
typedef int (*pcid_tx_cb_f)(pcid_info_t *pcid_info, uint8 *pkt_data, int eop);
extern void pcid_dma_tx_cb_set(pcid_info_t *pcid_info, pcid_tx_cb_f fun);
void _pcid_loop_tx_cb(pcid_info_t *pcid_info, dcb_t *dcb, int chan);


#endif /* _PCID_DMA_H_ */
