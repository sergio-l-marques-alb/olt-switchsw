/*
 * $Id: mbcm.c $
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
 * File:        mbcm.c
 */

#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mpls_term.h>
#include <soc/dpp/JER/JER_PP/jer_pp_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_fcf.h>
#include <soc/dpp/JER/JER_PP/jer_pp_ing_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_protection.h>
#include <soc/dpp/JER/JER_PP/jer_pp_lif.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_encap.h>
#include <soc/dpp/JER/JER_PP/jer_pp_eg_filter.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_lif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_rif.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact_mgmt.h>
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
#include <soc/dpp/JER/JER_PP/jer_pp_kaps_diag.h>
#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_extender.h>


CONST mbcm_pp_functions_t mbcm_pp_jericho_driver = {
    arad_pp_frwrd_mact_cpu_counter_learn_limit_set,
    arad_pp_frwrd_mact_cpu_counter_learn_limit_get,
    soc_jer_pp_mpls_termination_spacial_labels_init,
    soc_jer_pp_mpls_termination_range_action_set,
    soc_jer_pp_mpls_termination_range_action_get,
    soc_jer_pp_mpls_termination_range_profile_set,
    soc_jer_pp_mpls_termination_range_profile_get,
    soc_jer_mymac_protocol_group_set,
    soc_jer_mymac_protocol_group_get_protocol_by_group,
    soc_jer_mymac_protocol_group_get_group_by_protocols,
    arad_pp_mymac_vrid_mymac_map_set_to_all_vsi,
    soc_jer_mymac_vrrp_tcam_info_set,
    NULL,  /*mbcm_pp_mymac_vrrp_tcam_info_get*/
    soc_jer_mymac_vrrp_tcam_info_delete,
    arad_pp_port_property_set,
    arad_pp_port_property_get,
    arad_pp_oam_my_cfm_mac_delete,
    arad_pp_oam_my_cfm_mac_set,
    arad_pp_oam_my_cfm_mac_get,
    arad_pp_frwrd_fcf_npv_switch_set,
    arad_pp_frwrd_fcf_npv_switch_get,
    arad_pp_occ_mgmt_app_set,
    arad_pp_occ_mgmt_app_get,
    arad_pp_occ_mgmt_init,
    arad_pp_occ_mgmt_deinit,
	arad_pp_frwrd_fcf_vsan_mode_set,
	arad_pp_frwrd_fcf_vsan_mode_get,
    NULL,
    soc_jer_ingress_protection_state_set,
    soc_jer_ingress_protection_state_get,
    soc_jer_lif_glem_access_entry_add,
    soc_jer_lif_glem_access_entry_remove,
    soc_jer_lif_glem_access_entry_by_key_get,
    arad_pp_port_additional_tpids_set,
    arad_pp_port_additional_tpids_get,
    soc_jer_pp_oam_sa_addr_msbs_set,
    soc_jer_pp_oam_sa_addr_msbs_get,
    soc_jer_egress_protection_state_set,
    soc_jer_egress_protection_state_get,
    soc_jer_pp_oam_dm_trigger_set,
    arad_pp_oam_oamp_1dm_get,
    arad_pp_oam_oamp_1dm_set,
    arad_pp_oamp_pe_use_1dm_check,
    soc_jer_eg_encap_ether_type_index_clear, 
    soc_jer_eg_encap_ether_type_index_set, 
    soc_jer_eg_encap_ether_type_index_get,
    soc_jer_pp_network_group_config_set,
    soc_jer_pp_network_group_config_get,
    soc_jer_pp_oam_inlif_profile_map_set,
    soc_jer_pp_oam_inlif_profile_map_get,
    soc_jer_pp_oam_classifier_default_profile_add,
    soc_jer_pp_oam_classifier_default_profile_remove,
    arad_pp_bfd_one_hop_lem_entry_add,
    arad_pp_bfd_one_hop_lem_entry_remove,
    soc_jer_pp_oam_egress_pcp_set_by_profile_and_tc,
    NULL, /* mbcm_pp_oam_oamp_gen_mem_set. May be set in the future.*/
    arad_pp_aging_num_of_cycles_get,
    NULL,  /* mbcm_pp_eg_trill_entry_set */
    arad_pp_lif_additional_data_set,
    arad_pp_lif_additional_data_get,
    arad_pp_lif_is_wide_entry,
    arad_pp_oam_bfd_discriminator_rx_range_set,
    arad_pp_oam_bfd_discriminator_rx_range_get,
    arad_pp_oam_bfd_acc_endpoint_tx_disable,
    NULL, /*arad_pp_ip6_compression_add*/
    NULL, /*arad_pp_ip6_compression_delete*/
    NULL, /*arad_pp_ip6_compression_get*/
#if defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030)
    soc_jer_pp_diag_kaps_lkup_info_get,
#else
    NULL, /* soc_jer_pp_diag_kaps_lkup_info_get */
#endif /* defined(BCM_88675_A0) && defined(INCLUDE_KBP) && !defined(BCM_88030) */
    soc_jer_eg_encap_direct_bank_set,
    soc_jer_eg_encap_extension_mapping_set,
    soc_jer_eg_encap_extension_mapping_get,
    arad_pp_oamp_report_mode_set,
    arad_pp_oamp_report_mode_get,
    arad_pp_rif_global_urpf_mode_set,
    NULL,             /* mbcm_pp_ipmc_ssm_tcam_entry_add */
    NULL,             /* mbcm_pp_ipmc_ssm_tcam_entry_delete */
    NULL,              /* mbcm_pp_ipmc_ssm_tcam_entry_get */
    arad_pp_extender_port_info_set,
    arad_pp_extender_port_info_get,
    arad_pp_extender_init,
    arad_pp_extender_deinit
};

