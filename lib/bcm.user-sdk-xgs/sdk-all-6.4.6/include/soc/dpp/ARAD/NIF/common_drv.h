/*
 * $Id: common_drv.h,v 1.4 Broadcom SDK $
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
 * SOC NIF COMMON DRV H
 */
 
#ifndef _SOC_NIF_COMMON_DRV_H_
#define _SOC_NIF_COMMON_DRV_H_

#include <soc/error.h>

/*Selects Aggregate mode (CMAC) or independent mode (XMAC)*/
#define MAC_MODE_INDEP                  0
#define MAC_MODE_AGGR                   1

#define NIF_AVERAGE_IPG_DEFAULT         12
#define NIF_AVERAGE_IPG_HIGIG           8
#define NIF_TX_PREAMBLE_LENGTH_DEFAULT  8
#define NIF_PACKET_MAX_SIZE             (16*1024-1)

#define NIF_XMAC_SPEED_10     0x0
#define NIF_XMAC_SPEED_100    0x1
#define NIF_XMAC_SPEED_1000   0x2
#define NIF_XMAC_SPEED_2500   0x3
#define NIF_XMAC_SPEED_10000  0x4


soc_error_t soc_common_drv_bypass_mode_set(int unit, soc_port_t port, uint32 bypass_mode);
soc_error_t soc_common_drv_mac_mode_reg_hdr_set(int unit, soc_port_t port, soc_reg_t mac_mode_reg, uint32 *reg_val);
soc_error_t soc_common_drv_fast_port_set(int unit, soc_port_t port, int is_ilkn, int turn_on);
soc_error_t soc_common_link_status_clear(int unit, soc_port_t port);
soc_error_t soc_common_link_status_get(int unit, soc_port_t port, int *is_latch_down);
soc_error_t soc_common_drv_core_port_mode_get(int unit, soc_port_t port, uint32 *core_port_mode);
soc_error_t soc_common_drv_phy_port_mode_get(int unit, soc_port_t port, uint32 *phy_port_mode);

#endif /*_SOC_NIF_COMMON_DRV_H_*/
