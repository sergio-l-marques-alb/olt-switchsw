/*
 * $Id: mbcm.h,v 1.73 Broadcom SDK $
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
 * File:        mbcm_pp.h
 * Purpose:     Multiplexing of the bcm layer for PP
 *
 * Different chip families require such different implementations
 * of some basic BCM layer functionality that the functions are
 * multiplexed to allow a fast runtime decision as to which function
 * to call.  This file contains the basic declarations for this
 * process.
 * 
 *
 * See internal/design/soft_arch/xgs_plan.txt for more info.
 *
 * Conventions:
 *    MBCM_PP is the multiplexed pp bcm prefix
 *    _f is the function type declaration postfix
 */

#ifndef _SOC_DPP_MBCM_PP_H
#define _SOC_DPP_MBCM_PP_H

#include <bcm/types.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mymac.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_oam.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_mact.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_frwrd_ipv4.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_parse.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_dbal.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_vlan_edit.h>
#include <soc/dpp/PPC/ppc_api_extender.h>
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_port.h>
#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_oam.h>
#include <soc/dpp/PPC/ppc_api_l3_src_bind.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>
#include <soc/dpp/PPC/ppc_api_metering.h>
#include <soc/dpp/PPC/ppc_api_diag.h>
#include <soc/dpp/PPC/ppc_api_lif.h>

/****************************************************************
 *
 * SOC Family type:
 *
 ****************************************************************/
typedef enum soc_pp_chip_family_e {
    BCM_PP_FAMILY_PETRAB,   /* Not supported for mbcm_pp, but should have a family */
    BCM_PP_FAMILY_ARAD,
    BCM_PP_FAMILY_JERICHO
} soc_pp_chip_family_t;


/****************************************************************
 *
 * Type definitions for multiplexed BCM functions.
 *
 ****************************************************************/
 
typedef soc_error_t 
    (*mbcm_pp_frwrd_mact_opport_mode_get_f)(
       SOC_SAND_IN  int                   unit,
       SOC_SAND_OUT  uint32               *opport
       );
 
typedef soc_error_t
    (*mbcm_pp_frwrd_mact_clear_access_bit_f)(
       SOC_SAND_IN  int                                 unit,
       SOC_SAND_IN  int                                 fid,
       SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS             *dst_mac_address
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_event_handle_info_set_dma_f)(
       SOC_SAND_IN int enable_dma,
       SOC_SAND_IN  int unit
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_learning_dma_set_f)(
       SOC_SAND_IN  int unit
       );

typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_learning_dma_unset_f)(
       SOC_SAND_IN  int unit
       );
 
typedef soc_error_t 
   (*mbcm_pp_frwrd_mact_event_handler_callback_register_f)(
       SOC_SAND_IN  int                                      unit,
       SOC_SAND_IN  l2_event_fifo_interrupt_handler_cb_t     event_handler_cb
       );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   disable
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get_f) (
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint8*  is_enabled
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_transplant_static_set_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint8   enable
  );

typedef soc_error_t
  (*mbcm_pp_frwrd_mact_transplant_static_get_f) (
    SOC_SAND_IN  int     unit,
    SOC_SAND_OUT uint8*  is_enabled
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_spacial_labels_init_f)(
    int unit
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_action_set_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_action_get_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_ACTION_INFO *range_action_info
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_profile_set_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info
  );

typedef soc_error_t 
  (*mbcm_pp_mpls_termination_range_profile_get_f)(
    int unit, 
    uint32 entry_index, 
    SOC_PPC_MPLS_TERM_RANGE_PROFILE_INFO *range_profile_info
  );

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_vsan_mode_set_f)(int unit, int enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_vsan_mode_get_f)(int unit, int* enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_npv_switch_set_f)(int unit, int enable);

typedef soc_error_t
    (*mbcm_pp_frwrd_fcf_npv_switch_get_f)(int unit, int* enable);

typedef soc_error_t
    (*mbcm_pp_mymac_protocol_group_set_f)(int unit, uint32 protocols, uint32 group);

typedef soc_error_t 
    (*mbcm_pp_mymac_protocol_group_get_protocol_by_group_f)(int unit, uint8 group, uint32 *protocols);

typedef soc_error_t 
    (*mbcm_pp_mymac_protocol_group_get_group_by_protocols_f)(int unit, uint32 protocols, uint8 *group);

typedef soc_error_t
(*mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi_f)(int unit, int cam_index, int enable);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_set_f)(int unit, SOC_PPC_VRRP_CAM_INFO *info);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_get_f)(int unit, SOC_PPC_VRRP_CAM_INFO *info);

typedef soc_error_t
(*mbcm_pp_mymac_vrrp_cam_info_delete_f)(int unit, uint8 cam_index);

typedef soc_error_t 
(*mbcm_pp_port_property_set_f)(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value);

typedef soc_error_t 
(*mbcm_pp_port_property_get_f)(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value);

typedef soc_error_t
(*mbcm_pp_oam_my_cfm_mac_delete_f)(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_set_f)(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_get_f)(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_app_set_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, uint32 val, SHR_BITDCL *full_profile);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_app_get_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, SHR_BITDCL *full_profile, uint32 *val);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_get_app_mask_f)(int unit, SOC_OCC_MGMT_TYPE interface_type, int application_type, SHR_BITDCL *mask);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_init_f)(int unit);

typedef soc_error_t 
(*mbcm_pp_occ_mgmt_deinit_f)(int unit);

typedef soc_error_t 
(*mbcm_pp_presel_max_id_get_f)(int unit, SOC_PPC_FP_DATABASE_STAGE  stage, int *max_presel_id);

/* Protection functions */
typedef soc_error_t (*mbcm_pp_ingress_protection_state_set_f)(
    int unit,
    uint32 protection_ndx,
    uint8 path_state);

typedef soc_error_t (*mbcm_pp_ingress_protection_state_get_f)(
    int unit,
    uint32 protection_ndx,
    uint8 *path_state);

typedef soc_error_t (*mbcm_pp_egress_protection_state_set_f)(
    int unit,
    uint32 protection_ndx,
    uint8 path_state);

typedef soc_error_t (*mbcm_pp_egress_protection_state_get_f)(
    int unit,
    uint32 protection_ndx,
    uint8 *path_state);

typedef soc_error_t (* mbcm_pp_aging_num_of_cycles_get_f) (
    SOC_SAND_IN int     unit,
    SOC_SAND_IN uint32  aging_profile,
    SOC_SAND_OUT int     *fid_aging_cycles
);
/* GLEM functions */
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_add_f         )(int unit, int global_lif_index, int egress_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_remove_f      )(int unit, int global_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_by_key_get_f  )(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_set_f) (int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_get_f) (int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);



/*------------------------------OAM functions */

/* used by soc_jer_pp_oam_sa_addr_msbs_s/get*/
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8_star) (int, int, uint8*);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8) (int, int, uint8);
typedef soc_error_t (*mbcm_pp_oam_three_ints) (int, int, int);
typedef soc_error_t (*mbcm_pp_oam_one_int_two_int_stars) (int, int*, int*);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_const_uint8_star) (int, int, const uint8*);
typedef soc_error_t (*mbcm_pp_oam_one_int_three_uint8s) (int, uint8, uint8, uint8);

typedef soc_error_t (*mbcm_pp_oam_two_ints) (int,int);
typedef soc_error_t (*mbcm_pp_oam_counter_increment_bitmap) (int,int);


typedef soc_error_t (*mbcm_pp_network_group_config_set_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id, 
    SOC_SAND_IN uint32 is_filter);

typedef soc_error_t (*mbcm_pp_network_group_config_get_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 source_network_group_id,
    SOC_SAND_IN uint32 dest_network_group_id,
    SOC_SAND_OUT uint32 *is_filter);

/* Used by default profile setting */
typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_set_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_IN uint32);
typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_get_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_OUT uint32*);

typedef uint32 (*mbcm_pp_oam_oamp_punt_event_hendling_profile_set_f)
(SOC_SAND_IN int unit, SOC_SAND_IN uint32 profile_ndx, SOC_SAND_IN SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA *punt_profile_data);

typedef uint32 (*mbcm_pp_oam_oamp_punt_event_hendling_profile_get_f)
(SOC_SAND_IN int unit, SOC_SAND_IN uint32 profile_ndx, SOC_SAND_OUT SOC_PPD_OAM_OAMP_PUNT_PROFILE_DATA *punt_profile_data);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_add_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID,SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY*,SOC_SAND_IN uint8);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_remove_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID);

typedef soc_error_t (*mbcm_pp_oamp_pe_use_1dm_check_f)
(SOC_SAND_IN int, SOC_SAND_IN uint32);

typedef uint32 (*mbcm_pp_oam_tod_set_f) (int, uint8, uint64);

typedef soc_error_t (*mbcm_pp_oamp_report_mode_set_f) (int, SOC_PPC_OAM_REPORT_MODE);

typedef soc_error_t (*mbcm_pp_oamp_report_mode_get_f) (int, SOC_PPC_OAM_REPORT_MODE*);

typedef soc_error_t (*mbcm_pp_rif_global_urpf_mode_set_f) (int, int);

typedef int (*mbcm_pp_bfd_one_hop_lem_entry_add_f)
(int,const ARAD_PP_LEM_BFD_ONE_HOP_ENTRY_INFO *);

typedef int (*mbcm_pp_bfd_one_hop_lem_entry_remove_f)
(int,  uint32);

typedef int (*mbcm_pp_oam_bfd_acc_endpoint_tx_disable_f) (int, uint32, const SOC_PPC_OAM_OAMP_MEP_DB_ENTRY*);

typedef uint32 (*mbcm_pp_oam_oamp_loopback_set_f)(const int,ARAD_PP_OAM_LOOPBACK_INFO*);
typedef uint32 (*mbcm_pp_oam_oamp_loopback_get_f)(const int,ARAD_PP_OAM_LOOPBACK_INFO*);

/*------------------------------eg encap functions */
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_clear_f) (int, int);
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_set_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_IN SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_get_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_OUT SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 
typedef soc_error_t (*mbcm_pp_eg_trill_entry_set_f) (SOC_SAND_IN int, SOC_SAND_IN uint32, SOC_SAND_IN SOC_PPC_EG_ENCAP_TRILL_INFO*, SOC_SAND_IN uint32, SOC_SAND_IN uint8); 


typedef soc_error_t (*mbcm_pp_lif_additional_data_set_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_IN uint64 data);

typedef soc_error_t (*mbcm_pp_lif_additional_data_get_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_OUT uint64 *data);

typedef soc_error_t (*mbcm_pp_lif_is_wide_entry_f)
    (SOC_SAND_IN int unit, 
    SOC_SAND_IN int lif_id,
    SOC_SAND_IN uint8 is_ingress,
    SOC_SAND_OUT uint8 *is_wide_entry,
    SOC_SAND_OUT uint8 *ext_type);

/* IP6 compression functions */
typedef soc_error_t (*mbcm_pp_ip6_compression_add_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*); 
typedef soc_error_t (*mbcm_pp_ip6_compression_delete_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE*);
typedef soc_error_t (*mbcm_pp_ip6_compression_get_f) (SOC_SAND_IN int, SOC_SAND_INOUT SOC_PPC_IPV6_COMPRESSED_ENTRY*, SOC_SAND_OUT uint8*);

/*KAPS functions*/
typedef soc_error_t (*mbcm_pp_diag_kaps_lkup_info_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int core_id);

typedef soc_error_t (*mbcm_pp_lag_print_ecmp_lb_data_f)(SOC_SAND_OUT int unit);

typedef soc_error_t (*mbcm_pp_eg_encap_direct_bank_set_f)(int unit, int bank, uint8 is_mapped);

/* TCAM IPMC SSM functions */
typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_add_f)(SOC_SAND_IN int unit,
                                                         SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                         SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                         SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_delete_f)(SOC_SAND_IN int unit,
                                                            SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_tcam_entry_get_f)(SOC_SAND_IN  int unit,
                                                         SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                         SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                         SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS*    route_status,
                                                         SOC_SAND_OUT uint8* found);
														 
typedef soc_error_t (*mbcm_pp_eg_encap_extension_mapping_set_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_IN uint32 is_extended, SOC_SAND_IN uint32 extesnion_bank);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_mapping_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_OUT uint32* is_extended, SOC_SAND_OUT uint32* extesnion_bank);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_type_set_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_IN uint8 is_ext_data);

typedef soc_error_t (*mbcm_pp_eg_encap_extension_type_get_f)(SOC_SAND_IN int unit, SOC_SAND_IN int bank, SOC_SAND_OUT uint8* is_ext_data);

typedef soc_error_t (*mbcm_pp_extender_port_info_set_f)(int unit, ARAD_PP_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);
typedef soc_error_t (*mbcm_pp_extender_port_info_get_f)(int unit, ARAD_PP_PORT port, SOC_PPC_EXTENDER_PORT_INFO *port_info);

typedef soc_error_t (*mbcm_pp_extender_init_f)(int unit);
typedef soc_error_t (*mbcm_pp_extender_deinit_f)(int unit);

typedef soc_error_t (*mbcm_pp_flp_dbal_bfd_echo_program_tables_init_f)(int unit);

typedef uint32 (*mbcm_pp_oam_classifier_oem1_entry_set_unsafe_f)(SOC_SAND_IN int unit, SOC_SAND_IN SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_KEY *oem1_key, SOC_SAND_IN  SOC_PPD_OAM_CLASSIFIER_OEM1_ENTRY_PAYLOAD  *oem1_payload);

typedef uint32 (*mbcm_pp_oam_classifier_oem1_init_f)(SOC_SAND_IN int unit);

/* Metering functions*/
typedef soc_error_t (*mbcm_pp_mtr_meter_ins_bucket_get_f)(int unit,
							  int core_id,
							  SOC_PPC_MTR_METER_ID *meter_ins_ndx,
							  int *cbl,
							  int *ebl);
typedef soc_error_t(*mbcm_pp_mtr_policer_global_sharing_set_f)
  (
	int                         unit,
	int                         core_id,
	int        					meter_id,
	int							meter_group,
	uint32* 					global_sharing_ptr
  );
							  
typedef soc_error_t(*mbcm_pp_mtr_policer_global_sharing_get_f)
  (
   int                         	unit,
   int                         	core_id,
   int        					meter_id,
   int							meter_group,
   uint32* 						global_sharing_ptr
  );

typedef uint32(*mbcm_pp_mtr_policer_ipg_compensation_set_f)
  (
	int                         unit,
	uint8 ipg_compensation_enabled
  );

typedef uint32(*mbcm_pp_mtr_policer_ipg_compensation_get_f)
  (
	int                         unit,
	uint8 *ipg_compensation_enabled
  );

typedef uint32(*mbcm_pp_mtr_policer_hdr_compensation_set_f)
  (
	int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         compensation
  );

typedef uint32(*mbcm_pp_mtr_policer_hdr_compensation_get_f)
  (
	int                         unit,
	int                         core_id,
	uint32                      pp_port,
	int                         *compensation
  );


typedef uint32(*mbcm_pp_metering_pcd_entry_get_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_DECISION_INFO *pcd_entry
  );

typedef uint32(*mbcm_pp_metering_pcd_entry_set_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_IN SOC_PPC_MTR_COLOR_DECISION_INFO *pcd_entry
  );

typedef uint32(*mbcm_pp_metering_dp_map_entry_get_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_OUT SOC_PPC_MTR_COLOR_RESOLUTION_INFO *dp_map_entry
  );

typedef uint32(*mbcm_pp_metering_dp_map_entry_set_f)
  (
	SOC_SAND_IN int                         unit,
	SOC_SAND_IN SOC_PPC_MTR_COLOR_RESOLUTION_INFO *dp_map_entry
  );


typedef soc_error_t (*mbcm_pp_ptp_p2p_delay_set_f) (SOC_SAND_IN int, SOC_SAND_IN ARAD_PP_PORT, SOC_SAND_IN int);
typedef soc_error_t (*mbcm_pp_ptp_p2p_delay_get_f) (SOC_SAND_IN int, SOC_SAND_IN ARAD_PP_PORT, SOC_SAND_OUT int*);
 
typedef soc_error_t (*mbcm_pp_eg_encap_map_encap_intpri_color_set_f)(
    int                            unit,
    int                            index,
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *  entry_info
  );

typedef soc_error_t (*mbcm_pp_eg_encap_map_encap_intpri_color_get_f)(
    int                            unit,
    int                            index,
    SOC_PPC_EG_MAP_ENCAP_INTPRI_COLOR_INFO *  entry_info
  );


typedef soc_error_t (*mbcm_pp_eg_etpp_trap_set_f )(
    int                            unit,
    SOC_PPC_TRAP_ETPP_TYPE trap,
    SOC_PPC_TRAP_ETPP_INFO *entry_info
  );

typedef soc_error_t (*mbcm_pp_eg_etpp_trap_get_f )(
    int                            unit,
    SOC_PPC_TRAP_ETPP_TYPE trap,
    SOC_PPC_TRAP_ETPP_INFO *entry_info
  );


typedef soc_error_t (*mbcm_pp_eg_encap_access_clear_bank_f)(int unit, int bank_id);


typedef soc_error_t (*mbcm_pp_extender_global_etag_ethertype_set_f)(int unit, uint16 etag_tpid);
typedef soc_error_t (*mbcm_pp_extender_global_etag_ethertype_get_f)(int unit, uint16 *etag_tpid);

typedef soc_error_t (*mbcm_pp_llp_parse_packet_format_eg_info_set_f)
(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT ARAD_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );

typedef soc_error_t(*mbcm_pp_llp_parse_packet_format_eg_info_get_f)
  (
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_PARSE_INFO                      *parse_key,
    SOC_SAND_OUT ARAD_PP_LLP_PARSE_PACKET_FORMAT_INFO        *format_info
  );
typedef soc_error_t (*mbcm_pp_oam_oamp_lb_tst_header_set_f)(SOC_SAND_IN int unit, 
                                                            SOC_SAND_INOUT uint8* header_buffer, 
                                                            SOC_SAND_IN uint32 endpoint_id, 
                                                            SOC_SAND_IN bcm_mac_t dest_mac,
                                                            SOC_SAND_IN int flag,
                                                            SOC_SAND_OUT int *header_offset);

typedef soc_error_t (*mbcm_pp_diag_glem_signals_get_f)(int unit, int core_id, soc_ppc_diag_glem_signals_t *result);

typedef soc_error_t (*mbcm_pp_l2_lif_extender_remove_f)(int unit, SOC_PPC_L2_LIF_EXTENDER_KEY *extender_key, int *lif_index);

typedef uint32 (*mbcm_pp_l2_lif_extender_get_f)(
    int                                unit,
    ARAD_PP_L2_LIF_EXTENDER_KEY        *extender_key,
    ARAD_PP_LIF_ID                     *lif_index,
    ARAD_PP_L2_LIF_EXTENDER_INFO       *extender_info,
    uint8                              *found);

typedef soc_error_t (*mbcm_pp_extender_eve_etag_format_set_f)(int unit, uint32 edit_profile, uint8 is_extender);
typedef soc_error_t (*mbcm_pp_extender_eve_etag_format_get_f)(int unit, uint32 edit_profile, uint8 *is_extender);

typedef soc_error_t (*mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set_f) (
   int                                                    unit, 
   int                                                    encapsulation_mode, 
   SOC_PPC_EG_ENCAP_IP_TUNNEL_SIZE_PROTOCOL_TEMPLATE_INFO *ip_tunnel_size_protocol_template); 

typedef uint32(*mbcm_pp_frwrd_fec_is_protected_get_f)
  (
    int                         unit,
	uint32                      fec_ndx,
	uint8						*is_protected
  );
typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_add_f)(SOC_SAND_IN  int unit,
                                                    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                    SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE* success);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_delete_f)(SOC_SAND_IN int unit,
                                                       SOC_SAND_IN ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key);

typedef soc_error_t (*mbcm_pp_ipmc_ssm_entry_get_f )( SOC_SAND_IN  int unit,
                                                      SOC_SAND_IN  ARAD_PP_FRWRD_IPV4_MC_ROUTE_KEY* route_key,
                                                      SOC_SAND_OUT ARAD_PP_FRWRD_IPV4_MC_ROUTE_INFO* route_info,
                                                      SOC_SAND_OUT ARAD_PP_FRWRD_IP_ROUTE_STATUS*    route_status,
                                                      SOC_SAND_OUT uint8* found);

typedef soc_error_t (*mbcm_pp_eg_vlan_edit_packet_is_tagged_set_f)(
    SOC_SAND_IN int                                unit,
    SOC_SAND_IN ARAD_PP_EG_VLAN_EDIT_COMMAND_KEY   *eg_command_key,
    SOC_SAND_IN uint32                             is_tagged
    );

typedef soc_error_t (*mbcm_pp_eg_vlan_edit_packet_is_tagged_get_f)(
   SOC_SAND_IN int                                 unit,
   SOC_SAND_IN ARAD_PP_EG_VLAN_EDIT_COMMAND_KEY    *eg_command_key,
   SOC_SAND_OUT uint32                             *is_tagged
   );


typedef soc_error_t (*mbcm_dpp_pp_eg_encap_null_value_set_f) (
      int                           unit,
      uint32                       *value
  );

typedef soc_error_t (*mbcm_dpp_pp_eg_encap_null_value_get_f) (
      int                           unit,
      uint32                       *value
  );

typedef soc_error_t (*mbcm_pp_eg_encap_push_2_swap_init_f)(
    SOC_SAND_IN  int                               unit
   );

typedef uint32(*mbcm_pp_ecmp_hash_slb_combine_get_f)
  (
	int unit,
	int *combine_slb
  );

typedef uint32(*mbcm_pp_ecmp_hash_slb_combine_set_f)
  (
	int unit,
	int combine_slb);
	
typedef uint32 (*mbcm_dpp_oam_oamp_protection_packet_header_raw_set_f)(
      SOC_SAND_IN  int                                    unit,
	  SOC_SAND_IN  SOC_PPD_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );
  
typedef uint32 (*mbcm_dpp_oam_oamp_protection_packet_header_raw_get_f)(
      int                                    unit,
	  SOC_PPD_OAM_OAMP_PROTECTION_HEADER     *packet_protection_header
  );

typedef uint32 (*mbcm_pp_frwrd_fec_entry_urpf_mode_set_f)(
      int                                    unit,
	  uint32                                 fec_ndx,
	  uint32                                 uc_rpf_mode
  );
  

/****************************************************************
 *
 * Multiplexed BCM SOC Driver Structure
 *
 ****************************************************************/
typedef struct mbcm_pp_functions_s {
    /* insert functions here */
    mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set_f mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set;
    mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get_f mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get;
    mbcm_pp_frwrd_mact_transplant_static_get_f mbcm_pp_frwrd_mact_transplant_static_get;
    mbcm_pp_frwrd_mact_transplant_static_set_f mbcm_pp_frwrd_mact_transplant_static_set;
    mbcm_pp_frwrd_mact_clear_access_bit_f mbcm_pp_frwrd_mact_clear_access_bit;
    mbcm_pp_frwrd_mact_opport_mode_get_f mbcm_pp_frwrd_mact_opport_mode_get;
	mbcm_pp_frwrd_mact_event_handle_info_set_dma_f mbcm_pp_frwrd_mact_event_handle_info_set_dma;
    mbcm_pp_frwrd_mact_learning_dma_set_f mbcm_pp_frwrd_mact_learning_dma_set;
    mbcm_pp_frwrd_mact_learning_dma_unset_f mbcm_pp_frwrd_mact_learning_dma_unset;
    mbcm_pp_frwrd_mact_event_handler_callback_register_f mbcm_pp_frwrd_mact_event_handler_callback_register;
    mbcm_pp_mpls_termination_spacial_labels_init_f mbcm_pp_mpls_termination_spacial_labels_init;
    mbcm_pp_mpls_termination_range_action_set_f    mbcm_pp_mpls_termination_range_action_set;
    mbcm_pp_mpls_termination_range_action_get_f    mbcm_pp_mpls_termination_range_action_get;
    mbcm_pp_mpls_termination_range_profile_set_f   mbcm_pp_mpls_termination_range_profile_set;
    mbcm_pp_mpls_termination_range_profile_get_f   mbcm_pp_mpls_termination_range_profile_get;
    mbcm_pp_mymac_protocol_group_set_f mbcm_pp_mymac_protocol_group_set;
    mbcm_pp_mymac_protocol_group_get_protocol_by_group_f mbcm_pp_mymac_protocol_group_get_protocol_by_group;
    mbcm_pp_mymac_protocol_group_get_group_by_protocols_f mbcm_pp_mymac_protocol_group_get_group_by_protocols;
    mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi_f  mbcm_pp_mymac_vrid_mymac_map_set_to_all_vsi;
    mbcm_pp_mymac_vrrp_cam_info_set_f              mbcm_pp_mymac_vrrp_cam_info_set;
    mbcm_pp_mymac_vrrp_cam_info_get_f              mbcm_pp_mymac_vrrp_cam_info_get;
    mbcm_pp_mymac_vrrp_cam_info_delete_f           mbcm_pp_mymac_vrrp_cam_info_delete;
    mbcm_pp_port_property_set_f                    mbcm_pp_port_property_set;
    mbcm_pp_port_property_get_f                    mbcm_pp_port_property_get;
    mbcm_pp_oam_my_cfm_mac_delete_f                mbcm_pp_oam_my_cfm_mac_delete;
    mbcm_pp_oam_my_cfm_mac_set_f                   mbcm_pp_oam_my_cfm_mac_set;
    mbcm_pp_oam_my_cfm_mac_get_f                   mbcm_pp_oam_my_cfm_mac_get;
    mbcm_pp_frwrd_fcf_npv_switch_set_f             mbcm_pp_frwrd_fcf_npv_switch_set;
    mbcm_pp_frwrd_fcf_npv_switch_get_f             mbcm_pp_frwrd_fcf_npv_switch_get;
    mbcm_pp_occ_mgmt_app_set_f                     mbcm_pp_occ_mgmt_app_set;
    mbcm_pp_occ_mgmt_app_get_f                     mbcm_pp_occ_mgmt_app_get;
    mbcm_pp_occ_mgmt_get_app_mask_f                mbcm_pp_occ_mgmt_get_app_mask;
    mbcm_pp_occ_mgmt_init_f                        mbcm_pp_occ_mgmt_init;
    mbcm_pp_occ_mgmt_deinit_f                      mbcm_pp_occ_mgmt_deinit;
    mbcm_pp_frwrd_fcf_vsan_mode_set_f              mbcm_pp_frwrd_fcf_vsan_mode_set;
    mbcm_pp_frwrd_fcf_vsan_mode_get_f              mbcm_pp_frwrd_fcf_vsan_mode_get;
    mbcm_pp_presel_max_id_get_f                    mbcm_pp_presel_max_id_get;
    mbcm_pp_ingress_protection_state_set_f         mbcm_pp_ingress_protection_state_set;
    mbcm_pp_ingress_protection_state_get_f         mbcm_pp_ingress_protection_state_get;
    mbcm_pp_lif_glem_access_entry_add_f            mbcm_pp_lif_glem_access_entry_add;
    mbcm_pp_lif_glem_access_entry_remove_f         mbcm_pp_lif_glem_access_entry_remove;
    mbcm_pp_lif_glem_access_entry_by_key_get_f     mbcm_pp_lif_glem_access_entry_by_key_get;
    mbcm_pp_port_additional_tpids_set_f            mbcm_pp_port_additional_tpids_set;
    mbcm_pp_port_additional_tpids_get_f            mbcm_pp_port_additional_tpids_get;
    mbcm_pp_oam_two_ints_one_const_uint8_star      mbcm_pp_oam_sa_addr_msbs_set;
    mbcm_pp_oam_two_ints_one_uint8_star            mbcm_pp_oam_sa_addr_msbs_get;
    mbcm_pp_egress_protection_state_set_f          mbcm_pp_egress_protection_state_set;
    mbcm_pp_egress_protection_state_get_f          mbcm_pp_egress_protection_state_get;
    mbcm_pp_oam_two_ints                              mbcm_pp_oam_dm_trigger_set;
    mbcm_pp_oam_two_ints_one_uint8_star      mbcm_pp_oam_1dm_get;
    mbcm_pp_oam_two_ints_one_uint8        mbcm_pp_oam_1dm_set;
    mbcm_pp_oamp_pe_use_1dm_check_f                mbcm_pp_oamp_pe_use_1dm_check;
    mbcm_pp_eg_encap_ether_type_index_clear_f      mbcm_pp_eg_encap_ether_type_index_clear; 
    mbcm_pp_eg_encap_ether_type_index_set_f        mbcm_pp_eg_encap_ether_type_index_set; 
    mbcm_pp_eg_encap_ether_type_index_get_f        mbcm_pp_eg_encap_ether_type_index_get;
    mbcm_pp_network_group_config_set_f             mbcm_pp_network_group_config_set;
    mbcm_pp_network_group_config_get_f             mbcm_pp_network_group_config_get;
    mbcm_pp_oam_inlif_profile_map_set_f            mbcm_pp_oam_inlif_profile_map_set;
    mbcm_pp_oam_inlif_profile_map_get_f            mbcm_pp_oam_inlif_profile_map_get;
    mbcm_pp_oam_classifier_default_profile_add_f   mbcm_pp_oam_classifier_default_profile_add;
    mbcm_pp_oam_classifier_default_profile_remove_f mbcm_pp_oam_classifier_default_profile_remove;
    mbcm_pp_bfd_one_hop_lem_entry_add_f            mbcm_pp_lem_access_bfd_one_hop_lem_entry_add;
    mbcm_pp_bfd_one_hop_lem_entry_remove_f 		   mbcm_pp_lem_access_bfd_one_hop_lem_entry_remove;
    mbcm_pp_oam_one_int_three_uint8s          mbcm_pp_oam_egress_pcp_set_by_profile_and_tc;
    mbcm_pp_oam_three_ints            mbcm_pp_oam_oamp_gen_mem_set;
    mbcm_pp_aging_num_of_cycles_get_f               mbcm_pp_aging_num_of_cycles_get;
    mbcm_pp_eg_trill_entry_set_f                   mbcm_pp_eg_trill_entry_set;
    mbcm_pp_lif_additional_data_set_f              mbcm_pp_lif_additional_data_set;
    mbcm_pp_lif_additional_data_get_f              mbcm_pp_lif_additional_data_get;
    mbcm_pp_lif_is_wide_entry_f                    mbcm_pp_lif_is_wide_entry;
    mbcm_pp_oam_three_ints               mbcm_pp_oam_oam_bfd_discriminator_rx_range_set; 
    mbcm_pp_oam_one_int_two_int_stars        mbcm_pp_oam_oam_bfd_discriminator_rx_range_get; 
    mbcm_pp_oam_bfd_acc_endpoint_tx_disable_f      mbcm_pp_oam_bfd_acc_endpoint_tx_disable;
    mbcm_pp_ip6_compression_add_f                  mbcm_pp_ip6_compression_add;
    mbcm_pp_ip6_compression_delete_f               mbcm_pp_ip6_compression_delete;
    mbcm_pp_ip6_compression_get_f                  mbcm_pp_ip6_compression_get;
    mbcm_pp_diag_kaps_lkup_info_get_f              mbcm_pp_diag_kaps_lkup_info_get;
    mbcm_pp_lag_print_ecmp_lb_data_f               mbcm_pp_lag_print_ecmp_lb_data;
    mbcm_pp_eg_encap_direct_bank_set_f             mbcm_pp_eg_encap_direct_bank_set;
    mbcm_pp_eg_encap_extension_mapping_set_f       mbcm_pp_eg_encap_extension_mapping_set;
    mbcm_pp_eg_encap_extension_mapping_get_f       mbcm_pp_eg_encap_extension_mapping_get;
    mbcm_pp_eg_encap_extension_type_set_f          mbcm_pp_eg_encap_extension_type_set;
    mbcm_pp_eg_encap_extension_type_get_f          mbcm_pp_eg_encap_extension_type_get;
    mbcm_pp_oamp_report_mode_set_f                 mbcm_pp_oamp_report_mode_set;
    mbcm_pp_oamp_report_mode_get_f                 mbcm_pp_oamp_report_mode_get;
    mbcm_pp_rif_global_urpf_mode_set_f             mbcm_pp_rif_global_urpf_mode_set;
    mbcm_pp_ipmc_ssm_tcam_entry_add_f              mbcm_pp_ipmc_ssm_tcam_entry_add; 
    mbcm_pp_ipmc_ssm_tcam_entry_delete_f           mbcm_pp_ipmc_ssm_tcam_entry_delete;
    mbcm_pp_ipmc_ssm_tcam_entry_get_f              mbcm_pp_ipmc_ssm_tcam_entry_get;
    mbcm_pp_extender_port_info_set_f               mbcm_pp_extender_port_info_set;
    mbcm_pp_extender_port_info_get_f               mbcm_pp_extender_port_info_get;
    mbcm_pp_extender_init_f                        mbcm_pp_extender_init;
    mbcm_pp_extender_deinit_f                      mbcm_pp_extender_deinit;
    mbcm_pp_mtr_meter_ins_bucket_get_f	           mbcm_pp_mtr_meter_ins_bucket_get;
    mbcm_pp_ptp_p2p_delay_set_f		                 mbcm_pp_ptp_p2p_delay_set;
    mbcm_pp_ptp_p2p_delay_get_f			               mbcm_pp_ptp_p2p_delay_get;
    mbcm_pp_eg_encap_map_encap_intpri_color_set_f        mbcm_pp_eg_encap_map_encap_intpri_color_set;
    mbcm_pp_eg_encap_map_encap_intpri_color_get_f        mbcm_pp_eg_encap_map_encap_intpri_color_get;
	mbcm_pp_oam_tod_set_f                          mbcm_pp_oam_tod_set;
	mbcm_pp_flp_dbal_bfd_echo_program_tables_init_f      mbcm_pp_flp_dbal_bfd_echo_program_tables_init;
    mbcm_pp_eg_encap_access_clear_bank_f                mbcm_pp_encap_access_clear_bank;
    mbcm_pp_oam_oamp_loopback_set_f                 mbcm_pp_oam_oamp_loopback_set;
    mbcm_pp_oam_oamp_loopback_get_f                 mbcm_pp_oam_oamp_loopback_get;
    mbcm_pp_extender_global_etag_ethertype_set_f              mbcm_pp_extender_global_etag_ethertype_set;
    mbcm_pp_extender_global_etag_ethertype_get_f              mbcm_pp_extender_global_etag_ethertype_get;
    mbcm_pp_llp_parse_packet_format_eg_info_set_f  mbcm_pp_llp_parse_packet_format_eg_info_set;
    mbcm_pp_llp_parse_packet_format_eg_info_get_f  mbcm_pp_llp_parse_packet_format_eg_info_get;
	mbcm_pp_oam_two_ints                                  mbcm_pp_oam_slm_set;
    mbcm_pp_oam_counter_increment_bitmap           mbcm_pp_oam_counter_increment_bitmap_set;	
	mbcm_pp_mtr_policer_global_sharing_set_f				mbcm_pp_mtr_policer_global_sharing_set;
	mbcm_pp_mtr_policer_global_sharing_get_f				mbcm_pp_mtr_policer_global_sharing_get;
    mbcm_pp_oam_classifier_oem1_entry_set_unsafe_f   mbcm_pp_oam_classifier_oem1_entry_set_unsafe;
    mbcm_pp_eg_etpp_trap_set_f                       mbcm_pp_eg_etpp_trap_set;
    mbcm_pp_eg_etpp_trap_get_f                       mbcm_pp_eg_etpp_trap_get;
    mbcm_pp_oam_oamp_lb_tst_header_set_f             mbcm_pp_oam_oamp_lb_tst_header_set;	
	mbcm_pp_metering_pcd_entry_get_f                 mbcm_pp_metering_pcd_entry_get;
	mbcm_pp_metering_pcd_entry_set_f                 mbcm_pp_metering_pcd_entry_set;
	mbcm_pp_metering_dp_map_entry_get_f              mbcm_pp_metering_dp_map_entry_get;
	mbcm_pp_metering_dp_map_entry_set_f              mbcm_pp_metering_dp_map_entry_set;
    mbcm_pp_diag_glem_signals_get_f                  mbcm_pp_diag_glem_signals_get;
    mbcm_pp_oam_classifier_oem1_init_f               mbcm_pp_oam_classifier_oem1_init;
    mbcm_pp_l2_lif_extender_get_f                    mbcm_pp_l2_lif_extender_get;
    mbcm_pp_l2_lif_extender_remove_f                 mbcm_pp_l2_lif_extender_remove;
    mbcm_pp_extender_eve_etag_format_set_f           mbcm_pp_extender_eve_etag_format_set;
    mbcm_pp_extender_eve_etag_format_get_f           mbcm_pp_extender_eve_etag_format_get;
    mbcm_pp_oam_oamp_punt_event_hendling_profile_set_f mbcm_pp_oam_oamp_punt_event_hendling_profile_set;
    mbcm_pp_oam_oamp_punt_event_hendling_profile_get_f mbcm_pp_oam_oamp_punt_event_hendling_profile_get;
    mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set_f mbcm_pp_eg_encap_ip_tunnel_size_protocol_template_set; 
	mbcm_pp_mtr_policer_ipg_compensation_set_f		 mbcm_pp_mtr_policer_ipg_compensation_set;
	mbcm_pp_mtr_policer_ipg_compensation_get_f		 mbcm_pp_mtr_policer_ipg_compensation_get;
	mbcm_pp_mtr_policer_hdr_compensation_set_f       mbcm_pp_mtr_policer_hdr_compensation_set;
	mbcm_pp_mtr_policer_hdr_compensation_get_f       mbcm_pp_mtr_policer_hdr_compensation_get;
	mbcm_pp_frwrd_fec_is_protected_get_f             mbcm_pp_frwrd_fec_is_protected_get;
	mbcm_pp_ipmc_ssm_entry_add_f                     mbcm_pp_ipmc_ssm_entry_add;
	mbcm_pp_ipmc_ssm_entry_delete_f                  mbcm_pp_ipmc_ssm_entry_delete;
	mbcm_pp_ipmc_ssm_entry_get_f                     mbcm_pp_ipmc_ssm_entry_get;
    mbcm_pp_eg_vlan_edit_packet_is_tagged_set_f      mbcm_pp_eg_vlan_edit_packet_is_tagged_set;
    mbcm_pp_eg_vlan_edit_packet_is_tagged_get_f      mbcm_pp_eg_vlan_edit_packet_is_tagged_get;
    mbcm_dpp_pp_eg_encap_null_value_set_f            mbcm_dpp_pp_eg_encap_null_value_set;
    mbcm_dpp_pp_eg_encap_null_value_get_f            mbcm_dpp_pp_eg_encap_null_value_get;
    mbcm_pp_eg_encap_push_2_swap_init_f              mbcm_pp_eg_encap_push_2_swap_init;
    mbcm_pp_ecmp_hash_slb_combine_set_f              mbcm_pp_ecmp_hash_slb_combine_set;
    mbcm_pp_ecmp_hash_slb_combine_get_f              mbcm_pp_ecmp_hash_slb_combine_get;
    mbcm_dpp_oam_oamp_protection_packet_header_raw_set_f   mbcm_dpp_oam_oamp_protection_packet_header_raw_set;
	mbcm_dpp_oam_oamp_protection_packet_header_raw_get_f   mbcm_dpp_oam_oamp_protection_packet_header_raw_get;
	mbcm_pp_frwrd_fec_entry_urpf_mode_set_f         mbcm_pp_frwrd_fec_entry_urpf_mode_set;
} mbcm_pp_functions_t;


extern CONST mbcm_pp_functions_t *mbcm_pp_driver[BCM_MAX_NUM_UNITS]; /* USE _CALL MACRO FOR DISPATCH */
extern soc_pp_chip_family_t mbcm_pp_family[BCM_MAX_NUM_UNITS];

extern CONST mbcm_pp_functions_t mbcm_pp_arad_driver;
extern CONST mbcm_pp_functions_t mbcm_pp_jericho_driver;

extern int mbcm_dpp_pp_init(int unit);

/* The argumnet should be in parentheses and to include the unit, i.e. (unit, arg1, arg2, ...) */
#define MBCM_PP_DRIVER_CALL(unit, function, args) (((unit < BCM_MAX_NUM_UNITS) && mbcm_pp_driver[unit] && mbcm_pp_driver[unit]->function)?mbcm_pp_driver[unit]->function args:SOC_E_INTERNAL)

/* The argumnet should be in parentheses, i.e. (arg1, arg2, ...) */


#endif /*_SOC_DPP_MBCM_PP_H*/
