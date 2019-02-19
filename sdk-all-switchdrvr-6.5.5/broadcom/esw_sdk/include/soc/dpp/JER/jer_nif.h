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
 * File: jer_nif.h
 */

#ifndef __JER_NIF_INCLUDED__

#define __JER_NIF_INCLUDED__

#include <bcm/port.h>
#include <soc/portmod/portmod.h>
#include <soc/dpp/JER/jer_stat.h>
#include <soc/dcmn/dcmn_defs.h>

#define NUM_OF_LANES_IN_PM                4
#define MAX_NUM_OF_PMS_IN_ILKN            6
#define JER_NIF_ILKN_MAX_NOF_LANES        24

#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO        24
#define SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX            16

#define SOC_JER_NIF_PLL_TYPE_FABRIC0_FIRST_PHY_LANE 190
#define SOC_JER_NIF_PLL_TYPE_FABRIC0_LAST_PHY_LANE  226

#define SOC_JER_NIF_FIRST_FABRIC_PHY_PORT(unit)   \
        SOC_DPP_FIRST_FABRIC_PHY_PORT(unit)

#define SOC_JER_NIF_IS_ILKN_IF_OVER_FABRIC(unit, ilkn_if_id) \
        SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, ilkn_if_id)

#define SOC_JER_NIF_IS_ILKN_OVER_FABRIC_ENABLED(unit) \
        ((SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[0]) ||  \
        (SOC_DPP_CONFIG(unit)->jer->nif.ilkn_over_fabric[1]))

#define SOC_JER_NIF_ILKN_OVER_FABRIC_MAX_LANE(unit) (SOC_IS_QMX(unit) ? SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_QMX : SOC_JER_NIF_NOF_ILKN_OVER_FABRIC_LINKS_JERICHO)

/* PLL types enum*/
typedef enum
{
  SOC_JER_NIF_PLL_TYPE_PMH,
  SOC_JER_NIF_PLL_TYPE_PML0,
  SOC_JER_NIF_PLL_TYPE_PML1,
  SOC_JER_NIF_PLL_TYPE_FABRIC0,

  /* Total number of PLL types. */
  SOC_JER_NIF_NOF_PLL_TYPE
} SOC_JER_NIF_PLL_TYPE;

int soc_jer_portmod_calc_os(int unit, phymod_phy_access_t* phy_access, uint32* os_int, uint32* os_remainder);
int soc_jer_portmod_init(int unit);
int soc_jer_portmod_post_init(int unit, soc_pbmp_t* ports);
int soc_jer_portmod_deinit(int unit);
int soc_jer_portmod_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
int soc_jer_portmod_port_detach(int unit, int port);
int soc_jer_portmod_port_enable_set(int unit, soc_port_t port, uint32 mac_only, int enable);
int soc_jer_portmod_port_enable_get(int unit, soc_port_t port, uint32 mac_only, int* enable);
int soc_jer_portmod_port_speed_set(int unit, soc_port_t port, int speed);
int soc_jer_portmod_port_speed_get(int unit, soc_port_t port, int* speed);
int soc_jer_portmod_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
int soc_jer_portmod_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_jer_portmod_fabric_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
int soc_jer_portmod_fabric_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
int soc_jer_portmod_port_link_state_get(int unit, soc_port_t port, int clear_status, int *is_link_up, int *is_latch_down);
int soc_jer_portmod_is_supported_encap_get(int unit, int mode, int* is_supported);
int soc_jer_portmod_autoneg_set(int unit, soc_port_t port, int enable);
int soc_jer_portmod_autoneg_get(int unit, soc_port_t port, int* enable);
int soc_jer_port_ability_remote_get(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_ability_advert_set(int unit, soc_port_t port, soc_port_ability_t *ability_mask);
int soc_jer_port_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mode);
int soc_jer_port_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mode);
int soc_jer_port_mdix_status_get(int unit, soc_port_t port, soc_port_mdix_status_t *status);
int soc_jer_portmod_pfc_refresh_set(int unit, soc_port_t port, int value);
int soc_jer_portmod_pfc_refresh_get(int unit, soc_port_t port, int* value);
int soc_jer_port_phy_reset(int unit, soc_port_t port);
int soc_jer_port_mac_sa_set(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_mac_sa_get(int unit, int port, sal_mac_addr_t mac_sa);
int soc_jer_port_pll_type_get(int unit, soc_port_t port, SOC_JER_NIF_PLL_TYPE *pll_type);
int soc_jer_port_duplex_set(int unit, soc_port_t port, int duplex);
int soc_jer_port_fault_get(int unit, soc_port_t port, uint32 *flags);
int soc_jer_port_eee_enable_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_enable_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_idle_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_idle_time_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_eee_tx_wake_time_set(int unit, soc_port_t port, uint32 value);
int soc_jer_port_eee_tx_wake_time_get(int unit, soc_port_t port, uint32 *value);
int soc_jer_port_nif_nof_lanes_get(int unit, soc_port_if_t interface, uint32 first_phy_port, uint32 nof_lanes_to_set, uint32 *nof_lanes);
uint32 jer_port_link_up_mac_update(int unit, soc_port_t port, int link);
int soc_jer_port_remote_fault_enable_set(int unit, bcm_port_t port, int enable);
int soc_jer_nif_ilkn_pbmp_get(int unit, soc_port_t port, uint32 ilkn_id, soc_pbmp_t* phys, soc_pbmp_t* src_pbmp);
int soc_jer_nif_ilkn_over_fabric_pbmp_get(int unit, soc_pbmp_t* phys);
int soc_jer_nif_qsgmii_pbmp_get(int unit, soc_port_t port, uint32 id, soc_pbmp_t *phy_pbmp);

int soc_jer_pm_instances_get(int unit, portmod_pm_instances_t **pm_instances, int *pms_instances_arr_len);
int soc_jer_pml_table_get(int unit, soc_dpp_pm_entry_t **soc_pml_table);

int soc_jer_portmod_port_quad_get(int unit, soc_port_t port, soc_pbmp_t* quad_bmp);
int soc_jer_port_sch_config(int unit, soc_port_t port);
int soc_jer_port_close_path(int unit, soc_port_t port);
int soc_jer_port_ports_to_same_quad_get(int unit, soc_port_t port, soc_pbmp_t* ports);
int soc_jer_port_quad_ports_get(int unit, uint32 quad, soc_pbmp_t* ports_bm);
int soc_jer_nif_port_rx_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_nif_port_rx_enable_get(int unit, soc_port_t port, int *enable);
int soc_jer_nif_port_tx_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_nif_port_tx_enable_get(int unit, soc_port_t port, int *enable);
int soc_jer_port_protocol_offset_verify(int unit, soc_port_t port, uint32 protocol_offset);
int soc_jer_port_ilkn_over_fabric_set(int unit, soc_port_t port, uint32 ilkn_id);

int soc_jer_nif_priority_set(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_IN     int     priority_level);

int soc_jer_nif_priority_get(   SOC_SAND_IN     int     unit,
                                SOC_SAND_IN     int     core,
                                SOC_SAND_IN     uint32  quad_ilkn,
                                SOC_SAND_IN     uint32  is_ilkn,
                                SOC_SAND_IN     uint32  flags,
                                SOC_SAND_IN     uint32  allow_tdm,
                                SOC_SAND_OUT    int*    priority_level);

int jer_nif_ilkn_counter_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  soc_port_t               port,
    SOC_SAND_IN  soc_jer_counters_t       counter_type,
    SOC_SAND_OUT uint64                   *counter_val
  );
int soc_jer_nif_ilkn_phys_aligned_pbmp_get(int unit, soc_port_t port, soc_pbmp_t *phys_aligned, int is_revert);
int jer_nif_ilkn_counter_clear(int unit, soc_port_t port);

/* Add QSGMIIs offset to phy port
   Input:  Physical lane in the range of 1-72
   Output: Physical port in the range of 1-144*/
int soc_jer_qsgmii_offsets_add(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_add_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);

/* Remove QSGMIIs offset from phy port
   Input:  Physical port in the range of 1-144
   Output: Physical lane in the range of 1-72*/
int soc_jer_qsgmii_offsets_remove(int unit, uint32 phy, uint32 *new_phy);
int soc_jer_qsgmii_offsets_remove_pbmp(int unit, soc_pbmp_t* pbmp, soc_pbmp_t* new_pbmp);

int soc_jer_port_open_ilkn_path(int unit, int port); 
int soc_jer_port_close_ilkn_path(int unit, int port); 

int soc_jer_port_ilkn_init(int unit);

int soc_jer_port_open_path(int unit, soc_port_t port);

/*Fabric*/
soc_error_t soc_jer_port_fabric_clk_freq_init(int unit, soc_pbmp_t pbmp);

int soc_jer_port_nif_quad_to_core_validate(int unit);
int soc_jer_nif_ilkn_wrapper_port_enable_set(int unit, soc_port_t port, int enable);
int soc_jer_port_is_pcs_loopback(int unit, soc_port_t port, int *result);

int soc_jer_wait_gtimer_trigger(int unit);
int soc_jer_phy_nif_measure(int unit, soc_port_t port, uint32 *type_of_bit_clk, int *one_clk_time_measured_int, int *one_clk_time_measured_remainder, int *serdes_freq_int, int *serdes_freq_remainder, uint32 *lane);
int soc_jer_phy_nif_pll_div_get(int unit, soc_port_t port, soc_dcmn_init_serdes_ref_clock_t *ref_clk, int *p_div, int *n_div, int *m0_div);
int soc_jer_port_ilkn_bypass_interface_enable(int unit, int port, int enable);

/* PRD */
int soc_jer_port_prd_enable_set(int unit, soc_port_t port, uint32 flags, int enable);
int soc_jer_port_prd_enable_get(int unit, soc_port_t port, uint32 flags, int *enable);
int soc_jer_port_prd_config_set(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_jer_port_prd_config_get(int unit, soc_port_t port, uint32 flags, soc_dpp_port_prd_config_t *config);
int soc_jer_port_prd_threshold_set(int unit, soc_port_t port, uint32 flags, int priority, uint32 value);
int soc_jer_port_prd_threshold_get(int unit, soc_port_t port, uint32 flags, int priority, uint32 *value);
int soc_jer_port_prd_map_set(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int priority);
int soc_jer_port_prd_map_get(int unit, soc_port_t port, uint32 flags, soc_dpp_prd_map_t map, uint32 key, int* priority);
int soc_jer_port_prd_drop_count_get(int unit, soc_port_t port, uint32 *count);
int soc_jer_port_prd_tpid_set(int unit, uint32 flags, int index, uint16 tpid);
int soc_jer_port_prd_tpid_get(int unit, uint32 flags, int index, uint16 *tpid);
int soc_jer_nif_sif_set(int unit, uint32 first_phy);
int soc_jer_nif_sku_restrictions(int unit, bcm_pbmp_t phy_pbmp, bcm_port_if_t interface, uint32 protocol_offset, uint32 is_kbp);
#endif /*__JER_NIF_INCLUDED__*/

