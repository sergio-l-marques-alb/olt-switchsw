/*
 * $Id$
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
 * File:    bhh_feature.h
 */

#ifndef BHH_FEATURE_H_
#define BHH_FEATURE_H_

#define BHH_SDK_BASE_VERSION    0x01000001

#define BHH_SDK_VERSION    BHH_SDK_BASE_VERSION          | \
                           (1 << BHH_WB_SESS_GET)        | \
                           (1 << BHH_LOSS_GET)           | \
                           (1 << BHH_PM_MODE)            | \
                           (1 << BHH_REMOTE_EP_NAME)     | \
                           (1 << BHH_FAULTS_GET)         | \
                           (1 << BHH_PM_STAT_EXTRA_ELEM) | \
                           (1 << BHH_PASSIVE_MEP)        | \
                           (1 << BHH_OLP_ENCAP)          | \
                           (1 << BHH_OLP_ENCAP_VALID)    | \
                           (1 << BHH_PRIORITY_EVENT)     | \
                           (1 << BHH_DM_DATA_TLV)        | \
                           (1 << BHH_TRUNK_SUPPORT)

#define BHH_UC_MIN_VERSION 0x01000000

/* Indicates the feature supported : Same bit has to be used in SDK and UKERNEL
 * 0 and 24th bit cannot be used as they are set in base version string
 * BHH_SDK_VERSION : 0x01000001 BHH_APPL_VERSION : 0x01000000*/

#define BHH_FEATURE_RESERVED1  0
#define BHH_FEATURE_RESERVED2  24

#define BHH_WB_SESS_GET        1
#define BHH_LOSS_GET           2
#define BHH_PM_MODE            3
#define BHH_REMOTE_EP_NAME     4
#define BHH_FAULTS_GET         5
/* Extra elements added in pm_stat structure */
#define BHH_PM_STAT_EXTRA_ELEM 6
#define BHH_PASSIVE_MEP        7 /* Add support to create following endpoints
                                  *  - Only Rx CCM
                                  *  - Only Tx CCM
                                  *  - No Tx/Rx CCM
                                  */
#define BHH_OLP_ENCAP          8 /* Enhanced messages to support OLP encap,
                                  * loss_add, delay_add and loopback_add
                                  * enhnaced to take OLP encap. Corresponding get
                                  * messages are enahanced to return OLP encap.
                                  * SESS_SET takes inner & outer vlan for hash
                                  * calculation for Section MEP. MSG init takes
                                  * an additional param to indicate max encap
                                  * length. SESS_SET message takes num_counters
                                  * to indicate number of counters associated
                                  * this endpoint
                                  */
#define BHH_OLP_ENCAP_VALID    9 /* Indicates SDK's capability to send OLP
                                  * encapsulated BHH packets.
                                  * Note that though this is set for all platforms,
                                  * On the UKernel side this will be handled only 
                                  * for platforms for which BHH_OLP compile flag is
                                  * defined, which are Saber2 and Apache.
                                  */
#define BHH_PRIORITY_EVENT    10 /* Add support to raise priority mismatch event
                                  * for BHH CCM packets received from Peer MEP
                                  * BHH: int_pri and pkt_pri handling for
                                  * various BHH message types
                                  */
#define BHH_DM_DATA_TLV       11 /* Support data tlv in DM */
#define BHH_TRUNK_SUPPORT     12 /* Specify different Rx, Tx port for a session,
                                  * Add trunk support for TR3, Hx4 and KT
                                  */

/* bhh firmware version running in ukernel */
extern uint32 bhh_firmware_version;

/*Macro for BHH feature check*/
#define BHH_UC_FEATURE_CHECK(feature)  (bhh_firmware_version & (1 << feature))

#endif /* BHH_FEAT_H_ */
