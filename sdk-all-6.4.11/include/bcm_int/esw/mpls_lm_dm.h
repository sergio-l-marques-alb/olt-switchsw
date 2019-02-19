/*
 * $Id: mpls_lm_dm.h,v 1.4 Broadcom SDK $
 * 
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
 */
#ifndef __BCM_INT_ESW_MPLS_LM_DM_H__
#define __BCM_INT_ESW_MPLS_LM_DM_H__
#if defined(INCLUDE_MPLS_LM_DM)

#include <soc/uc_msg.h>
#include <soc/shared/mos_msg_common.h>
#include <soc/shared/mpls_lm_dm_pkt.h>
#include <soc/shared/mpls_lm_dm_msg.h>
#include <soc/shared/mpls_lm_dm_pack.h>

#include <bcm_int/common/rx.h>

#if defined(BCM_TRIUMPH_SUPPORT)
#include <bcm_int/esw/triumph.h>
#endif /* BCM_TRIUMPH_SUPPORT */

/*
 * MPLS_LM_DM Encapsulation Definitions
 *
 * Defines for building the MPLS_LM_DM packet encapsulation
 */

#define MPLS_LM_DM_THREAD_PRI_DFLT           200
#define MPLS_LMDM_EVENT_THREAD_PRIORITY      200
#define MPLS_LM_DM_RX_CHANNEL_SDK_VIEW       11
#define MPLS_LM_DM_RX_CHANNEL_UC_VIEW        1
#define MPLS_LM_DM_CPU_COS                   45

#define _MPLS_LM_DM_UC_MSG_TIMEOUT_USECS    5000000 /* 5 sec for FW response */

/*
 * MPLS_LM_DM Encapsulation Format Header flags
 *
 * Indicates the type of headers/labels present in a MPLS_LM_DM packet.
 * Bottom 4 bits are reserved to indicate the GACH Type (A-E)
 */
/* ACH type Mask */
#define _MPLS_LM_DM_ACH_TYPE_MASK                     0x000F

#define _MPLS_LM_DM_ENCAP_PKT_MPLS                    (1 << 4) 
#define _MPLS_LM_DM_ENCAP_PKT_MPLS_ROUTER_ALERT       (1 << 5) 
#define _MPLS_LM_DM_ENCAP_PKT_MPLS_BOTTOM             (1 << 6) 
#define _MPLS_LM_DM_ENCAP_PKT_PW                      (1 << 7) 
#define _MPLS_LM_DM_ENCAP_PKT_GAL                     (1 << 8) 
#define _MPLS_LM_DM_ENCAP_PKT_G_ACH                   (1 << 9)


#define _MPLS_LM_DM_MPLS_MAX_LABELS    4   /* Max MPLS labels in Saber2 */

#define _MPLS_LM_DM_MPLS_DFLT_TTL      16

#ifndef INCLUDE_BHH
/* ACH - Associated Channel Header */
typedef struct _ach_header_s {
    uint8   f_nibble;        /*  4: First nibble, must be 1 */
    uint8   version;         /*  4: Version */
    uint8   reserved;        /*  8: Reserved */
    uint16  channel_type;    /* 16: Channel Type */
} _ach_header_t;

/* MPLS - Multiprotocol Label Switching Label */
typedef struct _mpls_label_s {
    uint32  label;    /* 20: Label */
    uint8   exp;      /*  3: Experimental, Traffic Class, ECN */
    uint8   s;        /*  1: Bottom of Stack */
    uint8   ttl;      /*  8: Time to Live */
} _mpls_label_t;

/* VLAN Tag - 802.1Q */
typedef struct _vlan_tag_s {
    uint16      tpid;    /* 16: Tag Protocol Identifier */
    struct {
        uint8   prio;    /*  3: Priority Code Point */
        uint8   cfi;     /*  1: Canonical Format Indicator */
        uint16  vid;     /* 12: Vlan Identifier */
    } tci;               /* Tag Control Identifier */
} _vlan_tag_t;

/* L2 Header */
typedef struct _l2_header_t {
    bcm_mac_t    dst_mac;           /* 48: Destination MAC */
    bcm_mac_t    src_mac;           /* 48: Source MAC */
    _vlan_tag_t  vlan_tag;          /* VLAN Tag */
    _vlan_tag_t  vlan_tag_inner;    /* VLAN Tag */
    uint16       etype;             /* 16: Ether Type */
} _l2_header_t;
#endif

#endif 
#endif
