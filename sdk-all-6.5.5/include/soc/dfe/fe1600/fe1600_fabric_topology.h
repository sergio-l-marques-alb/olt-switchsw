/*
 * $Id: fe1600_fabric_topology.h,v 1.4 Broadcom SDK $
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
 *
 * FE1600 FABRIC TOPOLOGY H
 */
 
#ifndef _SOC_FE1600_FABRIC_TOPOLOGY_H_
#define _SOC_FE1600_FABRIC_TOPOLOGY_H_

#include <bcm/fabric.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/error.h>

soc_error_t soc_fe1600_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe1600_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val);
soc_error_t soc_fe1600_fabric_topology_isolate_get(int unit, soc_dcmn_isolation_status_t* val);
soc_error_t soc_fe1600_fabric_link_topology_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array);
soc_error_t soc_fe1600_fabric_link_topology_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array);
soc_error_t soc_fe1600_fabric_topology_repeater_destination_set(int unit, soc_port_t source, soc_port_t destination);
soc_error_t soc_fe1600_fabric_topology_repeater_destination_get(int unit, soc_port_t source, soc_port_t* destination);
soc_error_t soc_fe1600_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links);
soc_error_t soc_fe1600_fabric_topology_min_nof_links_set(int unit, int min_nof_links);
soc_error_t soc_fe1600_fabric_topology_min_nof_links_get(int unit, int *min_nof_links);
soc_error_t soc_fe1600_fabric_topology_rmgr_set(int unit, int enable);
soc_error_t soc_fe1600_fabric_topology_mesh_topology_reset(int unit);
int soc_fe1600_log2_round_up(uint32 x);
#endif /*_SOC_FE1600_FABRIC_TOPOLOGY_H_*/
