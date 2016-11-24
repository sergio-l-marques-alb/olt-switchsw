/*
 * $Id: packet.c,v 1.1 2011/04/18 17:11:10 mruas Exp $
 * $Copyright: Copyright 2007, Broadcom Corporation All Rights Reserved.
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES
 * OF ANY KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE.
 * BROADCOM SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.$
 *
 * File:        packet.c
 * Purpose:
 * Requires:    
 */

#include "cmicsim.h"
#include <sal/core/alloc.h>

#if !defined(min)
#define min(a,b)  ((a) < (b) ? (a) : (b))
#endif


int
pcid_add_pkt(pcid_info_t *pcid_info, uint8 *pkt_data, int pkt_len, uint32 *dcbd)
{
    packet_t *newpkt, *pp;
    int      i;

    if (pcid_info->pkt_count < CPU_MAX_PACKET_QUEUE) {
        ++(pcid_info->pkt_count);
        newpkt = sal_alloc(sizeof(packet_t), "pcid_add_pkt");
        if (newpkt == NULL) {
            return 1;
        }
        newpkt->length = min(pkt_len, PKT_SIZE_MAX);
        newpkt->consum = 0;
        newpkt->next = 0;
        for(i = 0; i < 8; i++) {
            newpkt->dcbd[i] = dcbd[i]; /* FB/ER 3 + 5 words of HG + PBI */
        }
        memset(newpkt->data, PKT_SIZE_MAX, 0);
        memcpy(newpkt->data, pkt_data, min(pkt_len, PKT_SIZE_MAX));

        sal_mutex_take(pcid_info->pkt_mutex, sal_mutex_FOREVER);
        /* Queue current packet on end of list */
        if (!pcid_info->pkt_list) {
            pcid_info->pkt_list = newpkt;
        } else {
            for (pp = pcid_info->pkt_list; pp->next; pp = pp->next)
                ;
            pp->next = newpkt;
        }
        sal_mutex_give(pcid_info->pkt_mutex);

        return 0;
    }

    return 1;
}

