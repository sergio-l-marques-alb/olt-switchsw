/*
 * $Id:  $
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
 * This file contains structure and routine declarations for the
 * Switch-on-a-Chip Driver.
 *
 * This file also includes the more common include files so the
 * individual driver files don't have to include as much.
 */
#ifndef SOC_DPP_ARAD_DRV_H
#define SOC_DPP_ARAD_DRV_H

#include <shared/cyclic_buffer.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mgmt.h>
#include <soc/dpp/TMC/tmc_api_end2end_scheduler.h>

extern soc_driver_t soc_driver_acp;

typedef cyclic_buffer_t dpp_captured_buffer_t;

typedef struct soc_dpp_config_arad_s {
    ARAD_MGMT_INIT  init;
    int8 voq_mapping_mode; /* supported values are: VOQ_MAPPING_DIRECT, VOQ_MAPPING_INDIRECT */
    int8 port_egress_recycling_scheduler;
    int8 action_type_source_mode; /* supported values are: ACTION_TYPE_FROM_FORWARDING_ACTION, ACTION_TYPE_FROM_QUEUE_SIGNATURE */
    dpp_captured_buffer_t captured_cells_buffer;/*Used for cell dump*/
    dpp_captured_buffer_t sr_cells_buffer;
    ARAD_PP_MGMT_OPERATION_MODE pp_op_mode;
    uint16 tdm_source_fap_id_offset; /* In case sending VCS256 to a Petra device, the TDM source FAP ID will be the FAP ID + This value */
    int8 xgs_compatability_tm_system_port_encoding; /* supported value 0-1 */
    int8 enable_lpm_custom_lookup; /* lookup {InLIF, TC} in LPM to get a general property of InLIF */
    ARAD_MGMT_RESERVED_PORT_INFO reserved_ports[SOC_MAX_NUM_PORTS]; /* for dynamic nif */
    soc_pbmp_t reserved_isq_base_q_pair[SOC_DPP_DEFS_MAX(NOF_CORES)]; /* for dynamic nif */
    soc_pbmp_t reserved_fmq_base_q_pair[SOC_DPP_DEFS_MAX(NOF_CORES)]; /* for dynamic nif */
    uint32 region_nof_remote_cores[SOC_DPP_DEFS_MAX(NOF_CORES)][SOC_TMC_COSQ_TOTAL_FLOW_REGIONS];
    int caui_fast_recycle;
    int8 hqos_mapping_enable;
} soc_dpp_config_arad_t;

#define ARAD_LPM_CUSTOM_LOOKUP_ENABLED(unit) (SOC_DPP_CONFIG(unit)->arad->enable_lpm_custom_lookup)
#define SOC_DPP_NOF_IN_RIF_PROFILE_BITS       6
#define SOC_ARAD_CPU_PHY_CORE                 0 /* The only valid core id in Arad. */
/* 
 * RCY port specified by the user for IPv6/4 UC with RPF check in two passes.
 * First Pass: SIP + RPF filtering
 * Second Pass RCY: (RCY) used for L3 DIP forwarding 
 * Custom Feature SOC 
 * -1: Disabled
 * 0 >= The port number, Feature enabled
 * if Property is set with specific port value in the following format: 
 * custom_feature_l3uc_with_rpf_2pass_recycle_port.<Device> 
 */
#define CF_IPV6_UC_WITH_RPF_2PASS_ENABLED  "l3uc_with_rpf_2pass_recycle_port"

int soc_arad_allocate_rcy_port(int unit, int core, soc_port_t *port);
int soc_arad_free_tm_port_and_recycle_channel(int unit, int port);
int soc_arad_isq_hr_get(int unit, bcm_core_t core, int *hr_isq);
int soc_arad_fmq_base_hr_get(int unit, bcm_core_t core, int** base_hr_fmq);
int soc_arad_info_config_device_ports(int unit);
int soc_arad_is_olp(int unit, soc_port_t port, uint32* is_olp);
int soc_arad_is_oamp(int unit, soc_port_t port, uint32* is_oamp);
int soc_arad_validate_fabric_mode(int unit, SOC_TMC_FABRIC_CONNECT_MODE *fabric_connect_mode);
int soc_arad_prop_fap_device_mode_get(int unit, uint8 *pp_enable, SOC_TMC_MGMT_TDM_MODE *tdm_mode);
int soc_arad_fc_oob_mode_validate(int unit, int port);
int soc_arad_dram_param_set(int unit, soc_dpp_config_arad_t *dpp_arad);
int soc_arad_default_ofp_rates_set(int unit);
int soc_arad_is_device_ilkn_disabled(int unit, int inlk);
int soc_arad_str_prop_parse_ucode_port_erp_lane(int unit, int *lane);
int soc_arad_ps_reserved_mapping_init(int unit);
int soc_arad_info_elk_config(int unit);
int soc_arad_default_config_get(int unit, soc_dpp_config_arad_t *cfg);
int soc_arad_core_frequency_config_get(int unit, int dflt_freq_khz, uint32 *freq_khz);
int soc_arad_schan_timeout_config_get(int unit, int *schan_timeout);
int soc_arad_init(int unit, int reset_action);
int soc_arad_deinit(int unit);
int soc_arad_attach(int unit);
int soc_arad_device_reset(int unit, int mode, int action);
int soc_arad_reinit(int unit, int reset);
int soc_arad_info_config(int unit);
int soc_arad_dma_mutex_init(int unit);
void soc_arad_dma_mutex_destroy(int unit);
int soc_arad_ilkn_phy_ports_btmp_set(int unit, uint32 ilkn_id, soc_pbmp_t *phy_ports);
void soc_arad_free_cache_memory(int unit);

int soc_arad_str_prop_parse_init_vid_enabled_port_get(int unit, soc_port_t port, uint32 *init_vid_enabled);
int soc_arad_str_prop_parse_pon_port(int unit, soc_port_t port, uint32* is_pon);
int soc_arad_collect_nif_config(int unit);
int soc_arad_ilkn_config_get(int unit, uint32 protocol_offset);
int soc_arad_caui_config_get(int unit, uint32 protocol_offset);

#ifdef BCM_WARM_BOOT_SUPPORT
void soc_arad_init_empty_scache(int unit);
#endif /* BCM_WARM_BOOT_SUPPORT */

int soc_arad_pp_init(int unit);
int soc_arad_str_prop_voq_mapping_mode_get(int unit, int8 *voq_mapping_mode);
int soc_arad_str_prop_hqos_mapping_get(int unit, int8 *hqos_mapping_enable);
int soc_arad_specific_info_config_direct(int unit);
int soc_arad_specific_info_config_derived(int unit);
int arad_info_config_custom_reg_access(int unit);
int soc_arad_validate_hr_is_free(int unit, int core, uint32 base_q_pair, uint8 *is_free);

#endif  /* SOC_DPP_ARAD_DRV_H */
