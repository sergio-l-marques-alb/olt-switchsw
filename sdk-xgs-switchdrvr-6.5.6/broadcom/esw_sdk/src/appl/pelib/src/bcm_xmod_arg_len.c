/*
 * $Id:$
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
 * File:       bcm_xmod_arg_len.c
 */

#include "bcm_xmod_api.h"

const xmod_api_length_t bcm_xmod_ethernet_cmds_arg_len_tbl[XMOD_ETHERNET_CMDS_COUNT] = {
/* XMOD_PHY_FIRMWARE_LANE_CONFIG_GET   0x00 */   { 0,                                          0 },
/* XMOD_PHY_FIRMWARE_LANE_CONFIG_SET   0x01 */   { 0,                                          0 },
/* NA                                  0x02 */   { 0,                                          0 },
/* XMOD_PHY_INIT                       0x03 */   { 0,                                          0 },
/* NA                                  0x04 */   { 0,                                          0 },
/* XMOD_PHY_RX_RESTART                 0x05 */   { 0,                                          0 },
/* XMOD_PHY_POLARITY_GET               0x06 */   { 0,                                          0 },
/* XMOD_PHY_POLARITY_SET               0x07 */   { 0,                                          0 },
/* XMOD_PHY_TX_GET                     0x08 */   { 0,                                          0 },
/* XMOD_PHY_TX_SET                     0x09 */   { 0,                                          0 },
/* XMOD_PHY_MEDIA_TYPE_TX_GET          0x0a */   { 0,                                          0 },
/* NA                                  0x0b */   { 0,                                          0 },
/* XMOD_PHY_TX_OVERRIDE_GET            0x0c */   { 0,                                          0 },
/* XMOD_PHY_TX_OVERRIDE_SET            0x0d */   { 0,                                          0 },
/* NA                                  0x0e */   { 0,                                          0 },
/* NA                                  0x0f */   { 0,                                          0 },
/* XMOD_PHY_RX_GET                     0x10 */   { 0,                                          0 },
/* XMOD_PHY_RX_SET                     0x11 */   { 0,                                          0 },
/* XMOD_PHY_RESET_GET                  0x12 */   { 0,                                          0 },
/* XMOD_PHY_RESET_SET                  0x13 */   { 0,                                          0 },
/* XMOD_PHY_POWER_GET                  0x14 */   { 0,                                          0 },
/* XMOD_PHY_POWER_SET                  0x15 */   { 0,                                          0 },
/* XMOD_PHY_TX_LANE_CONTROL_GET        0x16 */   { 0,                                          0 },
/* XMOD_PHY_TX_LANE_CONTROL_SET        0x17 */   { 0,                                          0 },
/* XMOD_PHY_RX_LANE_CONTROL_GET        0x18 */   { 0,                                          0 },
/* XMOD_PHY_RX_LANE_CONTROL_SET        0x19 */   { 0,                                          0 },
/* XMOD_PHY_FEC_ENABLE_GET             0x1a */   { 0,                                          0 },
/* XMOD_PHY_FEC_ENABLE_SET             0x1b */   { 0,                                          0 },
/* XMOD_PHY_INTERFACE_CONFIG_GET       0x1c */   { 0,                                          0 },
/* XMOD_PHY_INTERFACE_CONFIG_SET       0x1d */   { 0,                                          0 },
/* XMOD_PHY_CL72_GET                   0x1e */   { 0,                                          0 },
/* XMOD_PHY_CL72_SET                   0x1f */   { 0,                                          0 },
/* XMOD_PHY_CL72_STATUS_GET            0x20 */   { 0,                                          0 },
/* NA                                  0x21 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_ABILITY_GET        0x22 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_ABILITY_SET        0x23 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_REMOTE_ABILITY_GET 0x24 */   { 0,                                          0 },
/* NA                                  0x25 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_GET                0x26 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_SET                0x27 */   { 0,                                          0 },
/* XMOD_PHY_AUTONEG_STATUS_GET         0x28 */   { 0,                                          0 },
/* NA                                  0x29 */   { 0,                                          0 },
/* XMOD_PHY_LOOPBACK_GET               0x2a */   { 0,                                          0 },
/* XMOD_PHY_LOOPBACK_SET               0x2b */   { 0,                                          0 },
/* XMOD_PHY_RX_PMD_LOCKED_GET          0x2c */   { 0,                                          0 },
/* NA                                  0x2d */   { 0,                                          0 },
/* XMOD_PHY_LINK_STATUS_GET            0x2e */   { 0,                                          0 },
/* NA                                  0x2f */   { 0,                                          0 },
/* XMOD_PHY_STATUS_DUMP                0x30 */   { 0,                                          0 },
/* NA                                  0x31 */   { 0,                                          0 },
/* XMOD_PHY_PCS_USERSPEED_GET          0x32 */   { 0,                                          0 },
/* XMOD_PHY_PCS_USERSPEED_SET          0x33 */   { 0,                                          0 },
/* NA                                  0x34 */   { 0,                                          0 },
/* XMOD_PORT_INIT                      0x35 */   { 0,                                          0 },
/* NA                                  0x36 */   { 0,                                          0 },
/* XMOD_PHY_AN_TRY_ENABLE              0x37 */   { 0,                                          0 },
/* NA                                  0x38 */   { 0,                                          0 },
/* XMOD_PE_INIT                        0x39 */   { XMOD_PE_INIT_IN_LEN,                        XMOD_PE_INIT_OUT_LEN },
/* XMOD_PE_STATUS_GET                  0x3a */   { XMOD_PE_STATUS_GET_IN_LEN,                  XMOD_PE_STATUS_GET_OUT_LEN },
/* NA                                  0x3b */   { 0,                                          0 },
/* XMOD_PE_PORT_STATS_GET              0x3c */   { XMOD_PE_PORT_STATS_GET_IN_LEN,              XMOD_PE_PORT_STATS_GET_OUT_LEN },
/* NA                                  0x3d */   { 0,                                          0 },
/* NA                                  0x3e */   { 0,                                          0 },
/* XMOD_PE_TAGID_SET                   0x3f */   { XMOD_PE_TAGID_SET_IN_LEN,                   XMOD_PE_TAGID_SET_OUT_LEN },
/* XMOD_PE_FW_VER_GET                  0x40 */   { XMOD_PE_FW_VER_GET_IN_LEN,                  XMOD_PE_FW_VER_GET_OUT_LEN },
/* NA                                  0x41 */   { 0,                                          0 },
};

