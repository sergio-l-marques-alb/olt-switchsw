
/*
 * $Id: eth_lm_dm.h,v 1.0 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * File:    eth_lm_dm.h
 * Purpose: ETH_LM_DM definitions common to SDK and uKernel
 *
 *
 */
#ifndef __ETH_LM_DM_H__
#define __ETH_LM_DM_H__
#if defined(INCLUDE_ETH_LM_DM)

#include <bcm/types.h>
#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/eth_lm_dm.h>
#include <soc/shared/eth_lm_dm_pkt.h>
#include <soc/shared/eth_lm_dm_msg.h>
#include <soc/shared/eth_lm_dm_pack.h>

#include <bcm_int/common/rx.h>

/*
 * ETH_LM_DM Encapsulation Definitions
 *
 * Defines for building the ETH_LM_DM packet encapsulation
 */

#define BCM_SB2_ETH_LM_DM_RX_CHANNEL    1 /* this rx dma channel is shared with MPLS-LM/DM if it is there */
#define BCM_SB2_ETH_LM_DM_UC_PKT_DMA_RX_CHANNEL 11 /* For packets coming to core-2 from HW pipeline */
#define BCM_SB2_ETH_LM_DM_CPU_COS       45
#define BCM_ETH_ENDPOINT_MAX_MEP_ID_LENGTH 32

/**************************************************************************************/
#define _ETH_LM_DM_UC_MSG_TIMEOUT_USECS          20000000

#define BCM_ETH_ENDPOINT_PASSIVE            0x0004     /* Specifies endpoint
                                                          takes passive role */
#define BCM_ETH_ENDPOINT_DEMAND             0x0008     /* Specifies local*/
#define BCM_ETH_ENDPOINT_ENCAP_SET          0x0010     /* Update encapsulation
                                                          on existing BFD
                                                          endpoint */
/*
 * ETH_LM_DM Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a ETH_LM_DM packet.
 */
#define _ETH_LM_DM_ENCAP_PKT_ETH_LM_DM                     (1 << 11)
/*
 * Internal flags for ETH_LM_DM.
 */
#define _ETH_LM_DM_F_SESSION_IS_SET                        0x00000001
#define _ETH_LM_DM_F_DM_IS_SET                             0x00000002
#define _ETH_LM_DM_F_LM_IS_SET                             0x00000004 

#define ETH_LMDM_EVENT_THREAD_PRIORITY                     200

/*
 * Macros to pack uint8, uint16, uint32 in Network byte order
 */
#define _ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)   SHR_ETH_LM_DM_ENCAP_PACK_U8(_buf, _var)
#define _ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)  SHR_ETH_LM_DM_ENCAP_PACK_U16(_buf, _var)
#define _ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)  SHR_ETH_LM_DM_ENCAP_PACK_U32(_buf, _var)


/* VLAN Tag - 802.1Q */
typedef struct _eth_lm_dm_vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint16   prio:3,    /*  3: Priority Code Point */
           		 cfi:1,     /*  1: Canonical Format Indicator */
        		 vid:12;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} _eth_lm_dm_vlan_tag_t;

/* L2 Header */
typedef struct _eth_l2_header_s {
    bcm_mac_t    dst_mac;     /* 48: Destination MAC */
    bcm_mac_t    src_mac;     /* 48: Source MAC */
    _eth_lm_dm_vlan_tag_t  vlan_tag;    /* VLAN Tag */
    _eth_lm_dm_vlan_tag_t  inner_vlan_tag; /* VLAN Tag */
    uint16       etype;       /* 16: Ether Type */
} _eth_l2_header_t;

#endif 
#endif
