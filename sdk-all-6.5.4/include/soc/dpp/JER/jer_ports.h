/*
 * $Id: $
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
 * File: jer_sch.h
 */

#ifndef __JER_PORTS_INCLUDED__

#define __JER_PORTS_INCLUDED__

#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dcmn/error.h>

int
  soc_jer_port_to_interface_map_set(
    int                   unit,
    soc_port_t            port,
    int                   unmap
   );

uint32
  soc_jer_port_ingr_reassembly_context_get(
      SOC_SAND_IN int         unit,
      SOC_SAND_IN soc_port_t  port,
      SOC_SAND_OUT uint32     *port_termination_context,
      SOC_SAND_OUT uint32     *reassembly_context
  );

uint32 
  soc_jer_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
   );

uint32 
  soc_jer_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
    );
int
  soc_jer_port_to_interface_ingress_map_set(
      int                   unit,
      soc_port_t            port
   );

int
  soc_jer_port_to_interface_ingress_unmap_set(
      int                   unit,
      soc_port_t            port
   );

uint32
 soc_jer_port_synce_clk_sel_get(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  *port);

uint32
 soc_jer_port_synce_clk_sel_set(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  port);

int
  soc_jer_port_reference_clock_set(
      int                   unit,
      soc_port_t            port
   );

uint32
  soc_jer_port_header_type_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port,
    SOC_SAND_IN  ARAD_PORT_DIRECTION    direction_ndx,
    SOC_SAND_IN  ARAD_PORT_HEADER_TYPE  header_type
  );

int
soc_jer_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable);

int
soc_jer_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable);

int
soc_jer_port_nrdy_th_profile_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 profile);

int
soc_jer_port_nrdy_th_profile_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *profile);

int
soc_jer_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value);

int
soc_jer_port_mirrored_channel_and_context_map(int unit, 
                                              int core, 
                                              uint32 termination_context,
                                              uint32 reassembly_context, 
                                              uint32 channel);
											  

soc_error_t 
soc_jer_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

soc_error_t 
soc_jer_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info);

soc_error_t 
soc_jer_port_fabric_detach(int unit, soc_port_t port);

soc_error_t
soc_jer_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link, int enable);

soc_error_t
soc_jer_port_update_fsrd_block(int unit, soc_port_t port, int enable);

int
soc_jer_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port);

#endif /*__JER_PORTS_INCLUDED__*/

