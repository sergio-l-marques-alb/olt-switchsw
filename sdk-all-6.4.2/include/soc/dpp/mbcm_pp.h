/*
 * $Id: mbcm.h,v 1.73 Broadcom SDK $
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
#include <soc/dpp/PPC/ppc_api_mymac.h>
#include <soc/dpp/PPC/ppc_api_port.h>
#include <soc/dpp/PPC/ppc_api_profile_mgmt.h>
#include <soc/dpp/PPC/ppc_api_fp.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>
#include <soc/dpp/PPC/ppc_api_oam.h>


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
  (*mbcm_pp_mpls_termination_spacial_labels_init_f)(
    int unit
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
(*mbcm_pp_port_property_set_f)(int unit, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value);

typedef soc_error_t 
(*mbcm_pp_port_property_get_f)(int unit, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value);

typedef soc_error_t
(*mbcm_pp_oam_my_cfm_mac_delete_f)(int unit, SOC_SAND_PP_MAC_ADDRESS *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_set_f)(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_oam_my_cfm_mac_get_f)(int unit, SOC_SAND_PP_MAC_ADDRESS  *dst_mac_address, uint32 table_index);

typedef soc_error_t 
(*mbcm_pp_profile_mgmt_if_profile_advanced_val_set_f)(int unit, SOC_PPC_PROFILE_MGMT_TYPE interface_type, uint32 val, uint32 *full_profile);

typedef soc_error_t 
(*mbcm_pp_profile_mgmt_if_profile_advanced_get_val_from_map_f)(int unit, SOC_PPC_PROFILE_MGMT_TYPE interface_type, uint32 full_profile, uint32 *val);

typedef soc_error_t 
(*mbcm_pp_profile_mgmt_if_profile_val_set_f)(int unit, SOC_PPC_PROFILE_MGMT_TYPE interface_type, uint32 applicaton_type, uint32 val, uint32 *full_profile);

typedef soc_error_t 
(*mbcm_pp_profile_mgmt_if_profile_get_val_from_map_f)(int unit, SOC_PPC_PROFILE_MGMT_TYPE interface_type, uint32 applicaton_type, uint32 full_profile, uint32 *val);

typedef soc_error_t 
(*mbcm_pp_profile_mgmt_if_profile_init_f)(int unit);

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

/* GLEM functions */
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_add_f         )(int unit, int global_lif_index, int egress_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_remove_f      )(int unit, int global_lif_index);
typedef soc_error_t (*mbcm_pp_lif_glem_access_entry_by_key_get_f  )(int unit, int global_lif_id, int *egress_lif_id, uint8 *accessed, uint8 *is_found);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_set_f) (int unit, uint32 *additional_tpids);

typedef soc_error_t 
(*mbcm_pp_port_additional_tpids_get_f) (int unit, uint32 *additional_tpids);



/*------------------------------OAM functions */

/* used by soc_jer_pp_oam_sa_addr_msbs_s/get*/
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8_star) (int, int, uint8*);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_uint8) (int, int, uint8);
typedef soc_error_t (*mbcm_pp_oam_two_ints_one_const_uint8_star) (int, int, const uint8*);

typedef soc_error_t (*mbcm_pp_oam_two_ints) (int,int);

/* Used by default profile setting */
typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_set_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_IN uint32);
typedef soc_error_t (*mbcm_pp_oam_inlif_profile_map_get_f)
(SOC_SAND_IN int,SOC_SAND_IN uint32,SOC_SAND_OUT uint32*);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_add_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID,SOC_SAND_IN SOC_PPC_OAM_CLASSIFIER_MEP_ENTRY*,SOC_SAND_IN uint8);

typedef soc_error_t (*mbcm_pp_oam_classifier_default_profile_remove_f)
(SOC_SAND_IN int,SOC_SAND_IN ARAD_PP_OAM_DEFAULT_EP_ID);

/*------------------------------eg encap functions */
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_clear_f) (int, int);
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_set_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_IN SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 
typedef soc_error_t (*mbcm_pp_eg_encap_ether_type_index_get_f) (SOC_SAND_IN int, SOC_SAND_IN int, SOC_SAND_OUT SOC_PPC_EG_ENCAP_ETHER_TYPE_INDEX_INFO*); 


/****************************************************************
 *
 * Multiplexed BCM SOC Driver Structure
 *
 ****************************************************************/
typedef struct mbcm_pp_functions_s {
    /* insert functions here */
    mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set_f mbcm_pp_frwrd_mact_cpu_counter_learn_limit_set;
    mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get_f mbcm_pp_frwrd_mact_cpu_counter_learn_limit_get;
    mbcm_pp_mpls_termination_spacial_labels_init_f mbcm_pp_mpls_termination_spacial_labels_init;
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
    mbcm_pp_profile_mgmt_if_profile_advanced_val_set_f mbcm_pp_profile_mgmt_if_profile_advanced_val_set;
    mbcm_pp_profile_mgmt_if_profile_advanced_get_val_from_map_f mbcm_pp_profile_mgmt_if_profile_advanced_get_val_from_map;
    mbcm_pp_profile_mgmt_if_profile_val_set_f      mbcm_pp_profile_mgmt_if_profile_val_set;
    mbcm_pp_profile_mgmt_if_profile_get_val_from_map_f mbcm_pp_profile_mgmt_if_profile_get_val_from_map;
    mbcm_pp_profile_mgmt_if_profile_init_f         mbcm_pp_profile_mgmt_if_profile_init;
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
    mbcm_pp_eg_encap_ether_type_index_clear_f      mbcm_pp_eg_encap_ether_type_index_clear; 
    mbcm_pp_eg_encap_ether_type_index_set_f        mbcm_pp_eg_encap_ether_type_index_set; 
    mbcm_pp_eg_encap_ether_type_index_get_f        mbcm_pp_eg_encap_ether_type_index_get;
    mbcm_pp_oam_inlif_profile_map_set_f            mbcm_pp_oam_inlif_profile_map_set;
    mbcm_pp_oam_inlif_profile_map_get_f            mbcm_pp_oam_inlif_profile_map_get;
    mbcm_pp_oam_classifier_default_profile_add_f   mbcm_pp_oam_classifier_default_profile_add;
    mbcm_pp_oam_classifier_default_profile_remove_f mbcm_pp_oam_classifier_default_profile_remove;
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
