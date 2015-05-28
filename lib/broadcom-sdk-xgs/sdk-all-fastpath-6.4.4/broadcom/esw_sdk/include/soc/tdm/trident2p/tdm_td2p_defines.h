/*
 * $Id: tdm_td2p_defines.h$
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * All Rights Reserved.$
 *
 * TDM macro values for BCM56860
 */

#ifndef TDM_TD2P_PREPROCESSOR_MACROS_H
#define TDM_TD2P_PREPROCESSOR_MACROS_H

/* Default number of PM in chip */
#define TD2P_NUM_PM_MOD 33
#define TD2P_NUM_PHY_PM 32
/* Default number of lanes per PM in chip */
#define TD2P_NUM_PM_LNS 4
/* Allocation length of port lookup tables */
#define TD2P_NUM_EXT_PORTS 130
/* Number of the hardware designated management PM */
#define TD2P_MGMT_TSC_ID 3

/* Number of possible physical ports */
#define TD2P_NUM_PHY_PORTS 128
/* Number of scheduling blocks */
#define TD2P_NUM_QUAD 4

/* Default number of ancillary ports in calendar */
#define TD2P_ACC_PORT_NUM 10

/* Allocation length of LLS line rate calendar */
#define TD2P_LR_LLS_LEN 64
/* Allocation length of VBS line rate calendar */
#define TD2P_LR_VBS_LEN 256
/* Allocation length of IARB static calendar */
#define TD2P_LR_IARB_STATIC_LEN 512
/* Allocation area of LLS port groups */
#define TD2P_OS_LLS_GRP_NUM 2
#define TD2P_OS_LLS_GRP_LEN 32
/* Allocation area of VBS port groups */
#define TD2P_OS_VBS_GRP_NUM 8
#define TD2P_OS_VBS_GRP_LEN 16

/* Architecturally specific allocation area of VBS vector map */
#define TD2P_VMAP_MAX_LEN TD2P_LR_VBS_LEN
#define TD2P_VMAP_MAX_WID 49

/* Tokenization values */
#define TD2P_MGMT_TOKEN (TD2P_NUM_EXT_PORTS+3)
#define TD2P_US1G_TOKEN (TD2P_NUM_EXT_PORTS+4) 
#define TD2P_ANCL_TOKEN (TD2P_NUM_EXT_PORTS+5)
#ifdef _TDM_STANDALONE
    /* Token for oversub round robin */
    #define TD2P_OVSB_TOKEN (TD2P_NUM_EXT_PORTS+7)
    /* Memreset, L2 management, other */
    #define TD2P_IDL1_TOKEN (TD2P_NUM_EXT_PORTS+8)
    /* EP refresh, idle cycle */
    #define TD2P_IDL2_TOKEN (TD2P_NUM_EXT_PORTS)
#else
    /* Token for oversub round robin */
    #define TD2P_OVSB_TOKEN 250
    /* Memreset, L2 management, other */
    #define TD2P_IDL1_TOKEN 251
    /* EP refresh, idle cycle */
    #define TD2P_IDL2_TOKEN 252
#endif

/* Static port identities */
#define TD2P_MGMT_PORT_0 13
#define TD2P_MGMT_PORT_1 14
#define TD2P_MGMT_PORT_2 15
#define TD2P_MGMT_PORT_3 16
#define TD2P_LOOPBACK_PORT 129
#define TD2P_CPU_PORT 0

/* Macro definitions */
#define TD2P_PIPE_X_ID 0
#define TD2P_PIPE_Y_ID 1

#define TD2P_XPIPE_CAL_ID 4
#define TD2P_YPIPE_CAL_ID 5


#endif /* TDM_TD2P_PREPROCESSOR_MACROS_H */
