/*
 * $Id: ccm.h, v 1.0 Broadcom SDK $
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
 * File:    ccm.h
 * Purpose: CCM Application definitions common to SDK and uKernel.
 *
 */
#ifndef   _SOC_SHARED_CCM_H_
#define   _SOC_SHARED_CCM_H_

#define _FP_OAM_CCM_EVENT_THREAD_PRIO   200 /* CCM Event handler thread Prio */
#define _FP_OAM_MAX_MDL     7   /* Max num of MDLs (0-7) */
#define _FP_OAM_MAX_CTRS    3   /* Max num of ctrs that can be modified */
#define _FP_OAM_MAID_LEN    48  /* MAID Length */

#define _FP_OAM_MEP_TYPE_PORT       1
#define _FP_OAM_MEP_TYPE_CVLAN      2
#define _FP_OAM_MEP_TYPE_SVLAN      3
#define _FP_OAM_MEP_TYPE_S_C_VLAN   4

/* DA(12) + SA(12) + TAG1(4) + TAG2(4) + ETH_TYPE(2) + Rsvd(4) = 38 (~40) */
#define _FP_OAM_CCM_L2_ENCAP_LEN_MAX    40 /* Max L2 Encap Size */

#define _FP_OAM_OLP_L2_HDR_LEN          18 /* Size of OLP L2 Hdr */
#define _FP_OAM_OLP_OAM_TX_HDR_LEN      16 /* Size of OLP Tx OAM Hdr */
/* Size of Complete OLP Tx Header (L2 + OAM) */
#define _FP_OAM_OLP_TX_LEN              (_FP_OAM_OLP_L2_HDR_LEN + \
                                         _FP_OAM_OLP_OAM_TX_HDR_LEN)
#define _FP_OAM_OLP_OAM_RX_HDR_LEN      20 /* Size of OLP Rx OAM Hdr */
/* Size of Complete OLP Rx Header (L2 + OAM) */
#define _FP_OAM_OLP_RX_LEN              (_FP_OAM_OLP_L2_HDR_LEN + \
                                         _FP_OAM_OLP_OAM_RX_HDR_LEN)

#define _FP_OAM_OLP_CTR_WIDTH       11 /* Num bits for CNTR Idx */
#define _FP_OAM_OLP_CTR_POOL_WIDTH  3  /* Num bits for CNTR POOL */

#define _FP_OAM_OLP_CTR_LOC_IP      0  /* CTR Location IP */
#define _FP_OAM_OLP_CTR_LOC_EP      1  /* CTR Location EP */

#define _FP_OAM_OLP_CTR_ACTN_INC    1  /* CTR Action Incr */
#define _FP_OAM_OLP_CTR_ACYN_SAMPLE 2  /* CTR Action Sample */

/* Group Data Flags */
#define _FP_OAM_GROUP_SW_RDI_FLAG   (1 << 0)
#define _FP_OAM_INVALID_LMEP_ID     (-1)

/* MEP Data Flags */
#define _FP_OAM_REMOTE_MEP          (1 << 0)
#define _FP_OAM_INTERFACE_STATUS_TX (1 << 1) /* Tx Intf Status TLV */
#define _FP_OAM_PORT_STATUS_TX      (1 << 2) /* Tx Port Status TLV */
#define _FP_OAM_REMOTE_DEFECT_TX    (1 << 3) /* Tx RDI */

#endif
