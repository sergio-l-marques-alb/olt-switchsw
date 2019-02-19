/*
 * $Id: robo_sf3.h,v 1.3 Broadcom SDK $
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
 
#ifndef _ROBO_SF3_H
#define _ROBO_SF3_H


#define DRV_SF3_MCAST_GROUP_NUM         128
#define DRV_SF3_AGE_TIMER_MAX           1048575
#define DRV_SF3_TRUNK_GROUP_NUM         2
#define DRV_SF3_TRUNK_MAX_PORT_NUM      4
#define DRV_SF3_COS_QUEUE_NUM           8
#define DRV_SF3_MSTP_GROUP_NUM          8
#define DRV_SF3_SEC_MAC_NUM_PER_PORT    1
#define DRV_SF3_COS_QUEUE_MAX_WEIGHT_VALUE  255
#define DRV_SF3_AUTH_SUPPORT_PBMP       0x0000002f
#define DRV_SF3_RATE_CONTROL_SUPPORT_PBMP 0x00000002f
#define DRV_SF3_VLAN_ENTRY_NUM  4095
#define DRV_SF3_BPDU_NUM    1
#define DRV_SF3_CFP_TCAM_SIZE 128
#define DRV_SF3_CFP_UDFS_NUM 93
#define DRV_SF3_CFP_UDFS_OFFSET_MAX (2 * (32 - 1))
#define DRV_SF3_AUTH_SEC_MODE (DRV_SECURITY_VIOLATION_NONE | \
                                     DRV_SECURITY_EAP_MODE_EXTEND | \
                                     DRV_SECURITY_EAP_MODE_SIMPLIFIED)
                                        
#define DRV_SF3_MAC_LOW_POWER_SUPPORT_PBMP 0x0000000f
#define DRV_SF3_WRED_NUM                 (16)
#define DRV_SF3_WRED_MAX_AQD_PERIOD      (150) /* 150 us */
#define DRV_SF3_WRED_AQD_PERIOD_UNIT     (10) /* unit : 10 us */
#define DRV_SF3_CELL_UNIT                (256) /* unit : 256 bytes */
#define DRV_SF3_TC_MAX                   (7)
/* port4 and port5 within MACSEC attached */
#define DRV_SF3_MACSEC_PBMP           (0x30)  


/* SA learn limit */
#define DRV_SF3_ARL_ENTRY_NUM           (4096)
#define DRV_SF3_SYS_MAX_LEARN_LIMIT     DRV_SF3_ARL_ENTRY_NUM
#define DRV_SF3_PORT_MAX_LEARN_LIMIT    DRV_SF3_ARL_ENTRY_NUM

/* DEI and PCP format */
#define DRV_SF3_TAG_PRIORITY_MASK   (0x0F)
#define _DRV_SF3_TAGPRI_DEI_SHIFT   (3)
#define _DRV_SF3_TAGPRI_PCP_SHIFT   (0)
#define _DRV_SF3_TAGPRI_DEI_MASK    (0x1)
#define _DRV_SF3_TAGPRI_PCP_MASK    (0x7)
#define DRV_SF3_TAG_PRIORITY_DEI_SET(pri, dei)   \
        (pri) |= ((((dei) & _DRV_SF3_TAGPRI_DEI_MASK) << \
        _DRV_SF3_TAGPRI_DEI_SHIFT) & DRV_SF3_TAG_PRIORITY_MASK)  
#define DRV_SF3_TAG_PRIORITY_PCP_SET(pri, pcp)   \
        (pri) |= ((((pcp) & _DRV_SF3_TAGPRI_PCP_MASK) << \
        _DRV_SF3_TAGPRI_PCP_SHIFT) & DRV_SF3_TAG_PRIORITY_MASK)  
#define DRV_SF3_TAG_PRIORITY_DEI_GET(pri, dei)   \
        (dei) = ((((pri) & DRV_SF3_TAG_PRIORITY_MASK) >>  \
        _DRV_SF3_TAGPRI_DEI_SHIFT) & _DRV_SF3_TAGPRI_DEI_MASK)
#define DRV_SF3_TAG_PRIORITY_PCP_GET(pri, pcp)   \
        (pcp) = ((((pri) & DRV_SF3_TAG_PRIORITY_MASK) >>  \
        _DRV_SF3_TAGPRI_PCP_SHIFT) & _DRV_SF3_TAGPRI_PCP_MASK)

/* Egress rate */
/* kbps */
#define SF3_RATE_REFRESH_GRANULARITY  64

/* kbps */
#define SF3_RATE_MAX_REFRESH_RATE  (1000 * 1000)

/* byte */
#define SF3_RATE_BUCKET_UNIT_SIZE  64

/* bucket unit */
#define SF3_RATE_MAX_BUCKET_UNIT  0x3FFFF

/* max burst size (bytes)  */
#define SF3_RATE_MAX_BUCKET_SIZE \
    (SF3_RATE_MAX_BUCKET_UNIT * SF3_RATE_BUCKET_UNIT_SIZE)

/* unit of packet-based */
#define SF3_RATE_PACKET_BASED_REFRESH_UNIT           (125)  /* 125 pps */
#define SF3_RATE_PACKET_BASED_MAX_REFRESH_VALUE      (0x1FFFF)
#define SF3_RATE_PACKET_BASED_BUCKET_UNIT            (1)  /* 1 packet */
#define SF3_RATE_PACKET_BASED_MAX_BUCKET_VALUE       (0x1FFFF)


/* The Pause off threshold of Ingress rate control */
#define SF3_INGRESS_RATE_PAUSE_ON_THRESHOLD_KBYTES     (12) /* 12K bytes */

/* Starfighter3 Storm Control related definition*/
#define SF3_STORM_CONTROL_PKT_MASK (DRV_STORM_CONTROL_BCAST | \
                                    DRV_STORM_CONTROL_MCAST | \
                                    DRV_STORM_CONTROL_DLF | \
                                    DRV_STORM_CONTROL_RSV_MCAST | \
                                    DRV_STORM_CONTROL_UCAST)

#endif
