/*
 * $Id:$
 *
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
 *
 * DCMN FABRIC PORTH
 */

#ifndef _SOC_DCMN_PORT_H_
#define _SOC_DCMN_PORT_H_

#include <phymod/phymod.h>
#include <soc/portmod/portmod.h>
#include <soc/dcmn/dcmn_defs.h>

#define SOC_DCMN_PORT_NO_LANE_SWAP      (0x3210)

typedef struct dcmn_port_fabric_init_config_s {
    int pcs;
    int speed;
    int cl72;
} dcmn_port_fabric_init_config_t;

typedef int (*core_address_get_f)(int unit, int core_index, uint16 *address);
soc_error_t soc_dcmn_external_phy_chain_info_get(int unit, int phy, int addresses_array_size, int *addresses, int *phys_in_chain, int *is_clause45);
soc_error_t soc_dcmn_fabric_pms_add(int unit, int cores_num , int first_port, int use_mutex, int quads_in_fsrd, core_address_get_f address_get_func, void **user_data);
soc_error_t soc_dcmn_fabric_port_probe(int unit, int port, dcmn_port_fabric_init_config_t* default_port_config);
soc_error_t soc_dcmn_to_portmod_lb(int unit, soc_dcmn_loopback_mode_t dmnn_lb_mode, portmod_loopback_mode_t *portmod_lb_mode);
soc_error_t soc_dcmn_port_phy_control_set(int unit, soc_port_t port, soc_phy_control_t type, uint32 value);
soc_error_t soc_dcmn_port_phy_control_get(int unit, soc_port_t port, soc_phy_control_t type, uint32 *value);
soc_error_t soc_dcmn_fabric_pcs_parse(int unit, char *pcs_str, int pcs_def, int *pcs);
soc_error_t soc_dcmn_port_config_get(int unit, soc_port_t port, dcmn_port_fabric_init_config_t* port_config);
#endif /*_SOC_DCMN_PORT_H_*/


