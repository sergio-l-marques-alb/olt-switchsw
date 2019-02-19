/*
 * $Id: knet.c,v 1.0 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * File:        knet.c
 * Purpose:     Kernel Networking Management 
 *
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/dcbformats.h>
#include <soc/knet.h>

#include <bcm/error.h>
#include <bcm/knet.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/common/debug.h>

#include <shared/bsl.h>

#ifdef INCLUDE_KNET

/* Dune RX DMA Descriptor */
typedef struct {
        uint32  addr;                   /* T9.0: physical address */
#if  LE_HOST
        uint32  c_count:16,             /* Requested byte count */
        c_chain:1,                      /* Chaining */
        c_sg:1,                         /* Scatter Gather */
        c_reload:1,                     /* Reload */
        :13;                            /* Don't care */
#else
        uint32  :13,                    /* Don't care */
        c_reload:1,                     /* Reload */
        c_sg:1,                         /* Scatter Gather */
        c_chain:1,                      /* Chaining */
        c_count:16;                     /* Requested byte count */
#endif
        uint32  ep_to_cpu1[2];          /* EP_TO_CPU Header 0-2, DW 2,3 */

#if LE_HOST
        uint32  cpu_cos:6,              /* CPU COS, Egress Queue Number % 64 */
        :26;                            /* Reserved */
#else
        uint32  :26,                    /* Reserved */
        cpu_cos:6;                      /* CPU COS, Egress Queue Number % 64 */
#endif

#if LE_HOST
        uint32  source_port:8,          /* Source Port */
        :24;                            /* Reserved */
#else
        uint32  :24,                    /* Reserved */
        source_port:8;                  /* Source Port */
#endif

        uint32  ep_to_cpu2[9];          /* DW 6-14 */

#if  LE_HOST
        uint32  count:16,               /* Transferred byte count */
        end:1,                          /* End bit (RX) */
        start:1,                        /* Start bit (RX) */
        error:1,                        /* Cell Error (RX) */
        :12,                            /* Don't Care */
        done:1;                         /* Descriptor Done */
#else
        uint32  done:1,                 /* Descriptor Done */
        :12,                            /* Don't Care */
        error:1,                        /* Cell Error (RX) */
        start:1,                        /* Start bit (RX) */
        end:1,                          /* End bit (RX) */
        count:16;                       /* Transferred byte count */
#endif
}dcb_rx_t;

STATIC int
_petra_rx_reason_get(int unit, soc_rx_reasons_t *reasons)
{ 
    return BCM_E_NONE;;
}

STATIC int
_petra_trav_filter_clean(int unit, bcm_knet_filter_t *filter, void *user_data)
{
    return bcm_petra_knet_filter_destroy(unit, filter->id);
}

#endif /* INCLUDE_KNET */

/*
 * Function:
 *     bcm_petra_knet_init
 * Purpose:
 *     Initialize the kernel networking subsystem.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     BCM_E_XXX
 * Notes:
 */
int 
bcm_petra_knet_init(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    bcm_knet_filter_t filter;

    rv = bcm_petra_knet_cleanup(unit);

    if (BCM_SUCCESS(rv)) {
        bcm_knet_filter_t_init(&filter);
        filter.type = BCM_KNET_FILTER_T_RX_PKT;
        filter.dest_type = BCM_KNET_DEST_T_BCM_RX_API;
        filter.priority = 255;
        sal_strcpy(filter.desc, "DefaultRxAPI");
        rv = bcm_petra_knet_filter_create(unit, &filter);
    }
    return rv; 
#endif
}

/*
 * Function:
 *      bcm_petra_knet_cleanup
 * Purpose:
 *      Clean up the kernel networking subsystem.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_cleanup(int unit)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;

    rv = bcm_petra_knet_filter_traverse(unit, _petra_trav_filter_clean, NULL);

    return rv; 
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_create
 * Purpose:
 *      Create a kernel network interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif - (IN/OUT) Network interface configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_netif_create(int unit, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_netif_create_t netif_create;

    sal_memset(&netif_create, 0, sizeof(netif_create));
    netif_create.hdr.opcode = KCOM_M_NETIF_CREATE;
    netif_create.hdr.unit = unit;

    switch (netif->type) {
    case BCM_KNET_NETIF_T_TX_CPU_INGRESS:
        netif_create.netif.type = KCOM_NETIF_T_VLAN;
        break;
    case BCM_KNET_NETIF_T_TX_LOCAL_PORT:
        netif_create.netif.type = KCOM_NETIF_T_PORT;
        break;
    case BCM_KNET_NETIF_T_TX_META_DATA:
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported interface type\n")));
        return BCM_E_PARAM;
    }

    if (netif->flags & BCM_KNET_NETIF_F_ADD_TAG) {
        netif_create.netif.flags |= KCOM_NETIF_F_ADD_TAG;
    }

    netif_create.netif.vlan = netif->vlan;
    netif_create.netif.port = netif->port;

    sal_memcpy(netif_create.netif.macaddr, netif->mac_addr, 6);
    sal_memcpy(netif_create.netif.name, netif->name,
               sizeof(netif_create.netif.name) - 1);

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_create,
                          sizeof(netif_create), sizeof(netif_create));
    if (BCM_SUCCESS(rv)) {
        /* ID and interface name are assigned by kernel */
        netif->id = netif_create.netif.id;
        sal_memcpy(netif->name, netif_create.netif.name,
                   sizeof(netif->name) - 1);
    }
    return rv;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_destroy
 * Purpose:
 *      Destroy a kernel network interface.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif_id - (IN) Network interface ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_netif_destroy(int unit, int netif_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_netif_destroy_t netif_destroy;

    sal_memset(&netif_destroy, 0, sizeof(netif_destroy));
    netif_destroy.hdr.opcode = KCOM_M_NETIF_DESTROY;
    netif_destroy.hdr.unit = unit;

    netif_destroy.hdr.id = netif_id;
    
    return soc_knet_cmd_req((kcom_msg_t *)&netif_destroy,
                            sizeof(netif_destroy), sizeof(netif_destroy));
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_get
 * Purpose:
 *      Get a kernel network interface configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      netif_id - (IN) Network interface ID
 *      netif - (OUT) Network interface configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_netif_get(int unit, int netif_id, bcm_knet_netif_t *netif)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_netif_get_t netif_get;

    sal_memset(&netif_get, 0, sizeof(netif_get));
    netif_get.hdr.opcode = KCOM_M_NETIF_GET;
    netif_get.hdr.unit = unit;

    netif_get.hdr.id = netif_id;

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_get,
                          sizeof(netif_get.hdr), sizeof(netif_get));

    if (BCM_SUCCESS(rv)) {
        bcm_knet_netif_t_init(netif);

        switch (netif_get.netif.type) {
        case KCOM_NETIF_T_VLAN:
            netif->type = BCM_KNET_NETIF_T_TX_CPU_INGRESS;
            break;
        case KCOM_NETIF_T_PORT:
            netif->type = BCM_KNET_NETIF_T_TX_LOCAL_PORT;
            break;
        case KCOM_NETIF_T_META:
        default:
            /* Unknown type - defaults to VLAN */
            break;
        }

        if (netif_get.netif.flags & KCOM_NETIF_F_ADD_TAG) {
            netif->flags |= BCM_KNET_NETIF_F_ADD_TAG;
        }
        if (netif_get.netif.flags & KCOM_NETIF_F_RCPU_ENCAP) {
            netif->flags |= BCM_KNET_NETIF_F_RCPU_ENCAP;
        }

        netif->id = netif_get.netif.id;
        netif->vlan = netif_get.netif.vlan;
        netif->port = netif_get.netif.port;
        sal_memcpy(netif->mac_addr, netif_get.netif.macaddr, 6);
        sal_memcpy(netif->name, netif_get.netif.name,
                   sizeof(netif->name) - 1);
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_netif_traverse
 * Purpose:
 *      Traverse kernel network interface objects
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) User provided call back function
 *      user_data - (IN) User provided data used as input param for callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_netif_traverse(int unit, bcm_knet_netif_traverse_cb trav_fn,
                            void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv, idx;
    bcm_knet_netif_t netif;
    kcom_msg_netif_list_t netif_list;

    if (trav_fn == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&netif_list, 0, sizeof(netif_list));
    netif_list.hdr.opcode = KCOM_M_NETIF_LIST;
    netif_list.hdr.unit = unit;

    rv = soc_knet_cmd_req((kcom_msg_t *)&netif_list,
                          sizeof(netif_list.hdr), sizeof(netif_list));

    if (BCM_SUCCESS(rv)) {
        for (idx = 0; idx < netif_list.ifcnt; idx++) {
            rv = bcm_petra_knet_netif_get(unit, netif_list.id[idx], &netif);
            if (BCM_SUCCESS(rv)) {
                rv = trav_fn(unit, &netif, user_data);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_create
 * Purpose:
 *      Create a kernel packet filter.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter - (IN/OUT) Rx packet filter configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_filter_create(int unit, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    int idx, pdx;
    int oob_size;
    int data_offset;
    uint32 reason, bpdu;
    kcom_msg_filter_create_t filter_create;

    sal_memset(&filter_create, 0, sizeof(filter_create));
    filter_create.hdr.opcode = KCOM_M_FILTER_CREATE;
    filter_create.hdr.unit = unit;

    filter_create.filter.type = KCOM_FILTER_T_RX_PKT;

    switch (filter->dest_type) {
    case BCM_KNET_DEST_T_NULL:
        filter_create.filter.dest_type = KCOM_DEST_T_NULL;
        break;
    case BCM_KNET_DEST_T_NETIF:
        filter_create.filter.dest_type = KCOM_DEST_T_NETIF;
        break;
    case BCM_KNET_DEST_T_BCM_RX_API:
        filter_create.filter.dest_type = KCOM_DEST_T_API;
        break;
    case BCM_KNET_DEST_T_CALLBACK:
        filter_create.filter.dest_type = KCOM_DEST_T_CB;
        break;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported destination type\n")));
        return BCM_E_PARAM;
    }

    switch (filter->mirror_type) {
    case BCM_KNET_DEST_T_NULL:
        filter_create.filter.mirror_type = KCOM_DEST_T_NULL;
        break;
    case BCM_KNET_DEST_T_NETIF:
        filter_create.filter.mirror_type = KCOM_DEST_T_NETIF;
        break;
    case BCM_KNET_DEST_T_BCM_RX_API:
        filter_create.filter.mirror_type = KCOM_DEST_T_API;
        break;
    case BCM_KNET_DEST_T_CALLBACK:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Cannot mirror to callback\n")));
        return BCM_E_PARAM;
    default:
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "KNET: Unsupported mirror type\n")));
        return BCM_E_PARAM;
    }

    if (filter->flags & BCM_KNET_FILTER_F_STRIP_TAG) {
        filter_create.filter.flags |= KCOM_FILTER_F_STRIP_TAG;
    }

    filter_create.filter.dest_id = filter->dest_id;
    filter_create.filter.dest_proto = filter->dest_proto;
    filter_create.filter.mirror_id = filter->mirror_id;
    filter_create.filter.mirror_proto = filter->mirror_proto;

    filter_create.filter.priority = filter->priority;
    sal_strncpy(filter_create.filter.desc, filter->desc,
                sizeof(filter_create.filter.desc) - 1);

    oob_size = 0;
    if (filter->match_flags & ~BCM_KNET_FILTER_M_RAW) {
        oob_size = SOC_DCB_SIZE(unit);
    }

    /* Create inverted mask */
    for (idx = 0; idx < oob_size; idx++) {
        filter_create.filter.mask.b[idx] = 0xff;
    }

    bpdu = 0;
    if (BCM_RX_REASON_GET(filter->m_reason, bcmRxReasonBpdu)){
        bpdu = 1;
    }
    reason = _petra_rx_reason_get(unit, &filter->m_reason);

    /* Check if specified reason is supported */
    if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
        if ((reason + bpdu) == 0) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "KNET: Unsupported Rx reason\n")));
            return BCM_E_PARAM;
        }
    }

    if (SOC_DCB_TYPE(unit) == 28) {
        dcb_rx_t *dcb_data = (dcb_rx_t *)&filter_create.filter.data;
        dcb_rx_t *dcb_mask = (dcb_rx_t *)&filter_create.filter.mask;

        if (filter->match_flags & BCM_KNET_FILTER_M_VLAN) {
            /*
            dcb_data->outer_vid = filter->m_vlan;
            dcb_mask->outer_vid = 0;
            */
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_INGPORT) {
            dcb_data->source_port = filter->m_ingport;
            dcb_mask->source_port = 0;
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_REASON) {
            /*
            dcb_data->reason = reason;
            dcb_mask->reason = 0;
            */
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_FP_RULE) {
            /*
            dcb_data->match_rule = filter->m_fp_rule;
            dcb_mask->match_rule = 0;
            */
        }
        if (filter->match_flags & BCM_KNET_FILTER_M_ERROR) {
            /*
            dcb_data->error = 1;
            dcb_mask->error = 0;
            */
        }
    } else {

    }
 

    /* Invert inverted mask */
    for (idx = 0; idx < oob_size; idx++) {
        filter_create.filter.mask.b[idx] ^= 0xff;
    }

    filter_create.filter.oob_data_size = oob_size;

    if (filter->match_flags & BCM_KNET_FILTER_M_RAW) {
        data_offset = 0;
        for (pdx = 0; pdx < filter->raw_size; pdx++) {
            if (filter->m_raw_mask[pdx] != 0) {
                data_offset = pdx;
                break;
            }
        }
        idx = oob_size;
        for (; pdx < filter->raw_size; pdx++) {
            /* Check for array overflow */
            if (idx >= KCOM_FILTER_BYTES_MAX) {
                return BCM_E_PARAM;
            }
            filter_create.filter.data.b[idx] = filter->m_raw_data[pdx];
            filter_create.filter.mask.b[idx] = filter->m_raw_mask[pdx];
            idx++;
        }
        filter_create.filter.pkt_data_offset = data_offset;
        filter_create.filter.pkt_data_size = filter->raw_size - data_offset;
    }

    /*
     * If no match flags are set we treat raw filter data as OOB data.
     * Note that this functionality is intended for debugging only.
     */
    if (filter->match_flags == 0) {
        for (idx = 0; idx < filter->raw_size; idx++) {
            /* Check for array overflow */
            if (idx >= KCOM_FILTER_BYTES_MAX) {
                return BCM_E_PARAM;
            }
            filter_create.filter.data.b[idx] = filter->m_raw_data[idx];
            filter_create.filter.mask.b[idx] = filter->m_raw_mask[idx];
        }
        filter_create.filter.oob_data_size = SOC_DCB_SIZE(unit);
    }

    /* Dump raw data for debugging purposes */
    for (idx = 0; idx < BYTES2WORDS(oob_size); idx++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "OOB[%d]: 0x%08x [0x%08x]\n"), idx,
                     filter_create.filter.data.w[idx],
                     filter_create.filter.mask.w[idx]));
    }
    for (idx = 0; idx < filter_create.filter.pkt_data_size; idx++) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "PKT[%d]: 0x%02x [0x%02x]\n"),
                     idx + filter_create.filter.pkt_data_offset,
                     filter_create.filter.data.b[idx + oob_size],
                     filter_create.filter.mask.b[idx + oob_size]));
    }

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_create,
                          sizeof(filter_create), sizeof(filter_create));

    if (BCM_SUCCESS(rv)) {
        /* ID is assigned by kernel */
        filter->id = filter_create.filter.id;
    }
    return rv;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_destroy
 * Purpose:
 *      Destroy a kernel packet filter.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter_id - (IN) Rx packet filter ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_filter_destroy(int unit, int filter_id)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    kcom_msg_filter_destroy_t filter_destroy;

    sal_memset(&filter_destroy, 0, sizeof(filter_destroy));
    filter_destroy.hdr.opcode = KCOM_M_FILTER_DESTROY;
    filter_destroy.hdr.unit = unit;

    filter_destroy.hdr.id = filter_id;

    return soc_knet_cmd_req((kcom_msg_t *)&filter_destroy,
                            sizeof(filter_destroy), sizeof(filter_destroy));
#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_get
 * Purpose:
 *      Get a kernel packet filter configuration.
 * Parameters:
 *      unit - (IN) Unit number.
 *      filter_id - (IN) Rx packet filter ID
 *      filter - (OUT) Rx packet filter configuration
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_filter_get(int unit, int filter_id, bcm_knet_filter_t *filter)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_filter_get_t filter_get;
    dcb_t *dcb_data;
    dcb_t *dcb_mask;
    soc_rx_reasons_t no_reasons;
    soc_rx_reasons_t mask_reasons;
    int idx, rdx, fdx;

    sal_memset(&filter_get, 0, sizeof(filter_get));
    filter_get.hdr.opcode = KCOM_M_FILTER_GET;
    filter_get.hdr.unit = unit;

    filter_get.hdr.id = filter_id;

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_get,
                          sizeof(filter_get.hdr), sizeof(filter_get));

    if (BCM_SUCCESS(rv)) {
        bcm_knet_filter_t_init(filter);

        switch (filter_get.filter.type) {
        case KCOM_FILTER_T_RX_PKT:
            filter->type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        default:
            /* Unknown type */
            break;
        }

        switch (filter_get.filter.dest_type) {
        case KCOM_DEST_T_NETIF:
            filter->dest_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->dest_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            filter->dest_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->dest_type = BCM_KNET_DEST_T_NULL;
            break;
        }

        switch (filter_get.filter.mirror_type) {
        case KCOM_DEST_T_NETIF:
            filter->mirror_type = BCM_KNET_DEST_T_NETIF;
            break;
        case KCOM_DEST_T_API:
            filter->mirror_type = BCM_KNET_DEST_T_BCM_RX_API;
            break;
        case KCOM_DEST_T_CB:
            /* Should never get here, but keep for completeness */
            filter->mirror_type = BCM_KNET_DEST_T_CALLBACK;
            break;
        default:
            filter->mirror_type = BCM_KNET_DEST_T_NULL;
            break;
        }

        if (filter_get.filter.flags & KCOM_FILTER_F_STRIP_TAG) {
            filter->flags |= BCM_KNET_FILTER_F_STRIP_TAG;
        }

        filter->dest_id = filter_get.filter.dest_id;
        filter->dest_proto = filter_get.filter.dest_proto;
        filter->mirror_id = filter_get.filter.mirror_id;
        filter->mirror_proto = filter_get.filter.mirror_proto;

        filter->id = filter_get.filter.id;
        filter->priority = filter_get.filter.priority;
        sal_memcpy(filter->desc, filter_get.filter.desc,
                   sizeof(filter->desc) - 1);

        dcb_data = (dcb_t *)&filter_get.filter.data;
        dcb_mask = (dcb_t *)&filter_get.filter.mask;

        sal_memset(&no_reasons, 0, sizeof(no_reasons));
        SOC_DCB_RX_REASONS_GET(unit, dcb_mask, &mask_reasons);
        if (sal_memcmp(&mask_reasons, &no_reasons, sizeof(mask_reasons))) {
            filter->match_flags |= BCM_KNET_FILTER_M_REASON;
            SOC_DCB_RX_REASONS_GET(unit, dcb_data, &filter->m_reason);
        }
        if (SOC_DCB_RX_OUTER_VID_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_VLAN;
            filter->m_vlan = SOC_DCB_RX_OUTER_VID_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_INGPORT_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_INGPORT;
            filter->m_ingport = SOC_DCB_RX_INGPORT_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_SRCPORT_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODPORT;
            filter->m_src_modport = SOC_DCB_RX_SRCPORT_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_SRCMOD_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_SRC_MODID;
            filter->m_src_modid = SOC_DCB_RX_SRCMOD_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_MATCHRULE_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_FP_RULE;
            filter->m_fp_rule = SOC_DCB_RX_MATCHRULE_GET(unit, dcb_data);
        }
        if (SOC_DCB_RX_ERROR_GET(unit, dcb_mask)) {
            filter->match_flags |= BCM_KNET_FILTER_M_ERROR;
        }
        if (filter_get.filter.pkt_data_size) {
            filter->match_flags |= BCM_KNET_FILTER_M_RAW;
            rdx = filter_get.filter.pkt_data_offset;
            fdx = filter_get.filter.oob_data_size;
            for (idx = 0; idx < filter_get.filter.pkt_data_size; idx++) {
                filter->m_raw_data[rdx] = filter_get.filter.data.b[fdx];
                filter->m_raw_mask[rdx] = filter_get.filter.mask.b[fdx];
                rdx++;
                fdx++;
            }
            filter->raw_size = rdx;
        } else {
            /*
             * If a filter contains no raw packet data then we copy the OOB
             * data into the raw data buffer while raw_size remains zero.
             * Note that this functionality is intended for debugging only.
             */
            for (idx = 0; idx < SOC_DCB_SIZE(unit); idx++) {
                filter->m_raw_data[idx] = filter_get.filter.data.b[idx];
                filter->m_raw_mask[idx] = filter_get.filter.mask.b[idx];
            }
        }
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_petra_knet_filter_traverse
 * Purpose:
 *      Traverse kernel packet filter objects
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) User provided call back function
 *      user_data - (IN) User provided data used as input param for callback function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_petra_knet_filter_traverse(int unit, bcm_knet_filter_traverse_cb trav_fn, 
                             void *user_data)
{
#ifndef INCLUDE_KNET
    return BCM_E_UNAVAIL;
#else
    int rv, idx;
    bcm_knet_filter_t filter;
    kcom_msg_filter_list_t filter_list;

    if (trav_fn == NULL) {
        return BCM_E_PARAM;
    }

    sal_memset(&filter_list, 0, sizeof(filter_list));
    filter_list.hdr.opcode = KCOM_M_FILTER_LIST;
    filter_list.hdr.unit = unit;

    rv = soc_knet_cmd_req((kcom_msg_t *)&filter_list,
                          sizeof(filter_list.hdr), sizeof(filter_list));

    if (BCM_SUCCESS(rv)) {
        for (idx = 0; idx < filter_list.fcnt; idx++) {
            rv = bcm_petra_knet_filter_get(unit, filter_list.id[idx], &filter);
            if (BCM_SUCCESS(rv)) {
                rv = trav_fn(unit, &filter, user_data);
            }
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    return rv;
#endif
}
