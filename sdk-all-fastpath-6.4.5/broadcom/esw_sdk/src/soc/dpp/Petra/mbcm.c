/*
 * $Id: mbcm.c,v 1.65 Broadcom SDK $
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

#include <soc/dpp/mbcm.h>
#include <soc/dpp/port_map.h>
#include <soc/dpp/Petra/petra_drv.h>
#include <soc/dpp/Petra/PB_TM/pb_api_action_cmd.h>
#include <soc/dpp/Petra/PB_TM/pb_api_cnm.h>
#include <soc/dpp/Petra/PB_TM/pb_api_cnt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_diagnostics.h>
#include <soc/dpp/Petra/PB_TM/pb_api_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_fabric.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ssr.h>
#include <soc/dpp/Petra/PB_TM/pb_api_stack.h>
#include <soc/dpp/Petra/PB_TM/pb_api_tdm.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/petra_api_cell.h>
#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_api_fabric.h>
#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_ingress_scheduler.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_multicast_egress.h>
#include <soc/dpp/Petra/petra_api_multicast_fabric.h>
#include <soc/dpp/Petra/petra_api_multicast_ingress.h>
#include <soc/dpp/Petra/petra_api_ofp_rates.h>
#include <soc/dpp/Petra/petra_api_packet.h>
#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/petra_api_reg_access.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_header_parsing_utils.h>
#include <soc/dpp/Petra/petra_nif.h>

#include <soc/dpp/Petra/petra_fabric.h>

CONST mbcm_dpp_functions_t mbcm_petrab_driver = {
    soc_pb_action_cmd_snoop_set, 
    soc_pb_action_cmd_snoop_get, 
    soc_pb_action_cmd_mirror_set, 
    soc_pb_action_cmd_mirror_get,
    NULL,/*mbcm_dpp_action_cmd_cosq_flush*/   
    soc_pb_cnm_cp_set, 
    soc_pb_cnm_cp_get, 
    soc_pb_cnm_q_mapping_set, 
    soc_pb_cnm_q_mapping_get, 
    soc_pb_cnm_congestion_test_set, 
    soc_pb_cnm_congestion_test_get, 
    soc_pb_cnm_cp_profile_set, 
    soc_pb_cnm_cp_profile_get, 
    soc_pb_cnm_sampling_profile_set, 
    soc_pb_cnm_sampling_profile_get, 
    soc_pb_cnm_cpq_pp_set, 
    soc_pb_cnm_cpq_pp_get, 
    soc_pb_cnm_cpq_sampling_set, 
    soc_pb_cnm_cpq_sampling_get, 
    soc_pb_cnm_intercept_timer_set, 
    soc_pb_cnm_intercept_timer_get, 
    soc_pb_cnt_counters_set, 
    NULL, /* dma_unset */
    NULL, /* dma_set */
    soc_pb_cnt_counters_get, 
    soc_pb_cnt_status_get, 
    NULL, /* cnt_engine_to_fifo_dma_index */
    NULL, /* cnt_fifo_dma_offset_in_engine */
    soc_pb_cnt_algorithmic_read, 
    soc_pb_cnt_direct_read, 
    soc_pb_cnt_q2cnt_id, 
    soc_pb_cnt_cnt2q_id, 
    NULL, /*epni_regs_set*/
    NULL /* soc_pb_cnt_meter_hdr_compensation_set */, 
    NULL /* soc_pb_cnt_meter_hdr_compensation_get */, 
    soc_pb_diag_last_packet_info_get, 
    soc_pb_diag_sample_enable_set, 
    soc_pb_diag_sample_enable_get, 
    soc_pb_diag_signals_dump, 
    soc_pb_egr_q_prio_set_dispatch, 
    soc_pb_egr_q_prio_get_dispatch, 
    soc_pb_egr_q_profile_map_set_dispatch, 
    soc_pb_egr_q_profile_map_get_dispatch,
    NULL, /* arad_egr_q_fqp_scheduler_config */
    NULL, /* arad_egr_q_cgm_interface_set */
    soc_pb_fabric_line_coding_set, 
    soc_pb_fabric_line_coding_get, 
    NULL, /* soc_pb_fabric_pcp_dest_mode_config_set */
    NULL, /* soc_pb_fabric_pcp_dest_mode_config_get */
    NULL, /*mbcm_dpp_port_fabric_clk_freq_init*/
    soc_pb_interface_id_verify, 
    soc_pb_if_type_from_id, 
    soc_pb_mal_equivalent_id_verify, 
    NULL, /* mbcm_dpp_stat_fabric_init */
    NULL, /* mbcm_dpp_stat_nif_init */ 
    NULL, /* mbcm_dpp_info_config_custom_reg_access */  
    NULL, /* mbcm_dpp_mapping_stat_get */  
    NULL, /* soc_pb_itm_committed_q_size_set */
    NULL, /* soc_pb_itm_committed_q_size_get */
    NULL, /* soc_pb_cosq_gport_sched_set */
    NULL, /* soc_pb_cosq_gport_sched_get */
    soc_pb_port_pp_port_set_dispatch, 
    soc_pb_port_pp_port_get, 
    soc_pb_port_to_pp_port_map_set_dispatch, 
    soc_pb_port_to_pp_port_map_get,
    NULL, /* mbcm_dpp_port_control_low_latency_set */
    NULL, /* mbcm_dpp_port_control_fec_error_detect_set */
    NULL, /* mbcm_dpp_port_control_low_latency_get */
    NULL, /* mbcm_dpp_port_control_fec_error_detect_get */
    NULL, /*mbcm_dpp_port_extract_cig_from_llfc_enable_set*/
    NULL, /*mbcm_dpp_port_extract_cig_from_llfc_enable_get*/
    soc_pb_port_forwarding_header_set, 
    soc_pb_port_forwarding_header_get,
    NULL, /* mbcm_dpp_port_nrdy_th_profile_set */
    NULL, /* mbcm_dpp_port_nrdy_th_profile_get */
    NULL, /* mbcm_dpp_port_nrdy_th_optimial_value_get */  
    soc_petra_ports_logical_sys_id_build_with_device, 
    soc_petra_sys_virtual_port_to_local_port_map_set_unsafe, 
    soc_pb_ssr_buff_size_get, 
    soc_pb_ssr_to_buff, 
    soc_pb_ssr_from_buff, 
    soc_pb_ssr_is_device_init_done, 
    soc_pb_stack_global_info_set, 
    soc_pb_stack_global_info_get, 
    soc_pb_stack_port_distribution_info_set, 
    soc_pb_stack_port_distribution_info_get,
    soc_pb_tdm_ftmh_set, 
    soc_pb_tdm_ftmh_get, 
    soc_pb_tdm_opt_size_set, 
    soc_pb_tdm_opt_size_get, 
    soc_pb_tdm_stand_size_range_set, 
    soc_pb_tdm_stand_size_range_get,   
    soc_pb_tdm_mc_static_route_set, 
    soc_pb_tdm_mc_static_route_get, 
    NULL, /* mbcm_dpp_tdm_port_packet_crc_set INVALID for PB */
    NULL, /* mbcm_dpp_tdm_port_packet_crc_get INVALID for PB */
    NULL, /* mbcm_dpp_tdm_direct_routing_set INVALID for PB */
    NULL, /* mbcm_dpp_tdm_direct_routing_get INVALID for PB */
    NULL, /* mbcm_dpp_tdm_direct_routing_profile_map_set INVALID for PB */
    NULL, /* mbcm_dpp_tdm_direct_routing_profile_map_get INVALID for PB */
    NULL, /* mbcm_dpp_tdm_ifp_get INVALID for PB */
    NULL, /* mbcm_dpp_tdm_ifp_set INVALID for PB */
    soc_petra_read_from_fe600, 
    soc_petra_write_to_fe600, 
    soc_petra_indirect_read_from_fe600, 
    soc_petra_indirect_write_to_fe600, 
    soc_petra_cpu2cpu_write, 
    soc_petra_cpu2cpu_read, 
    soc_petra_cell_mc_tbl_write, 
    soc_petra_cell_mc_tbl_read, 
    soc_petra_diag_ipt_rate_get, 
    soc_petra_diag_iddr_set, 
    soc_petra_diag_iddr_get, 
    soc_petra_diag_regs_dump, 
    soc_petra_diag_tbls_dump, 
    soc_petra_diag_tbls_dump_all, 
    soc_petra_nif_diag_last_packet_get, 
    soc_petra_diag_soft_error_test_start, 
    soc_petra_egq_resources_print, 
    NULL,  /*mbcm_dpp_egr_threshold_types_verify*/
    soc_petra_egr_ofp_thresh_type_set_dispatch, 
    soc_petra_egr_ofp_thresh_type_get, 
    soc_petra_egr_sched_drop_set, 
    soc_petra_egr_sched_drop_get, 
    soc_petra_egr_unsched_drop_set, 
    soc_petra_egr_unsched_drop_get, 
    soc_petra_egr_dev_fc_set, 
    soc_petra_egr_dev_fc_get, 
    soc_petra_egr_xaui_spaui_fc_set, 
    soc_petra_egr_xaui_spaui_fc_get, 
    soc_petra_egr_ofp_fc_set, 
    NULL, /*mbcm_dpp_egr_sched_port_fc_thresh_set*/
    NULL, /*mbcm_dpp_egr_sched_q_fc_thresh_set*/
    soc_petra_egr_ofp_fc_get, 
    soc_petra_egr_mci_fc_set, 
    soc_petra_egr_mci_fc_get, 
    soc_petra_egr_mci_fc_enable_set, 
    soc_petra_egr_mci_fc_enable_get, 
    soc_petra_egr_ofp_sch_mode_set, 
    soc_petra_egr_ofp_sch_mode_get, 
    soc_petra_egr_ofp_scheduling_set, 
    soc_petra_egr_ofp_scheduling_get, 
    soc_petra_egr_unsched_drop_prio_set, 
    soc_petra_egr_unsched_drop_prio_get, 
    soc_petra_sch_device_rate_entry_set,
    soc_petra_sch_device_rate_entry_get, 
    NULL, /*mbcm_dpp_sch_device_rate_entry_core_set */
    NULL, /*mbcm_dpp_sch_device_rate_entry_core_get */
    soc_petra_sch_if_shaper_rate_set_dispatch, 
    soc_petra_sch_if_shaper_rate_get_dispatch, 
    soc_petra_sch_device_if_weight_idx_set_dispatch, 
    soc_petra_sch_device_if_weight_idx_get_dispatch, 
    soc_petra_sch_if_weight_conf_set, 
    soc_petra_sch_if_weight_conf_get, 
    soc_petra_sch_class_type_params_set, 
    soc_petra_sch_class_type_params_get, 
    soc_petra_sch_class_type_params_table_set, 
    soc_petra_sch_class_type_params_table_get, 
    soc_petra_sch_port_sched_set,
    soc_petra_sch_port_sched_get, 
    soc_petra_sch_port_hp_class_conf_set, 
    soc_petra_sch_port_hp_class_conf_get, 
    NULL, /*soc_petra_sch_slow_max_rates_set*/
    NULL, /*soc_petra_sch_slow_max_rates_get*/
    NULL, /*mbcm_dpp_sch_slow_max_rates_per_level_set*/
    NULL, /*mbcm_dpp_sch_slow_max_rates_per_level_get*/
    soc_petra_sch_aggregate_set, 
    soc_petra_sch_aggregate_group_set,
    soc_petra_sch_aggregate_get, 
    soc_petra_sch_flow_delete, 
    soc_petra_sch_flow_set, 
    soc_petra_sch_flow_get, 
    soc_petra_sch_flow_status_set, 
    soc_petra_sch_flow_ipf_config_mode_set,
    soc_petra_sch_flow_ipf_config_mode_get,
    soc_petra_sch_per1k_info_set, 
    soc_petra_sch_per1k_info_get, 
    soc_petra_sch_flow_to_queue_mapping_set, 
    soc_petra_sch_flow_to_queue_mapping_get, 
    soc_petra_sch_flow_id_verify_unsafe, 
    soc_petra_sch_se_id_verify_unsafe, 
    soc_petra_sch_port_id_verify_unsafe, 
    soc_petra_sch_k_flow_id_verify_unsafe, 
    soc_petra_sch_quartet_id_verify_unsafe, 
    soc_petra_sch_se2port_id, 
    NULL, /* mbcm_dpp_sch_se2port_tc_id */
    soc_petra_sch_flow2se_id, 
    soc_petra_sch_port2se_id, 
    NULL, /* mbcm_dpp_sch_port_tc2se_id */
    soc_petra_sch_se2flow_id, 
    soc_petra_sch_se_get_type_by_id,
    NULL, /* mbcm_dpp_sch_e2e_interface_allocate */
    NULL, /* mbcm_dpp_sch_e2e_interface_deallocate */
    NULL, /* mbcm_dpp_sch_prio_propagation_enable_set */
    NULL, /* mbcm_dpp_sch_prio_propagation_enable_get */
    NULL, /*mbcm_dpp_sch_prio_propagation_port_set*/
    NULL, /*mbcm_dpp_sch_prio_propagation_port_get*/
    NULL, /* mbcm_dpp_sch_shds_tbl_get_unsafe */
    NULL, /* mbcm_dpp_sch_shds_tbl_set_unsafe */
    soc_petra_fabric_fc_enable_set, 
    soc_petra_fabric_fc_enable_get, 
    NULL, /* mbcm_dpp_fabric_fc_shaper_get - invalid for Soc_petra-B */
    NULL, /* mbcm_dpp_fabric_fc_shaper_set - invalid for Soc_petra-B */
    soc_petra_fabric_cell_format_get, 
    soc_petra_fabric_coexist_set, 
    soc_petra_fabric_coexist_get, 
    soc_petra_fabric_stand_alone_fap_mode_get, 
    soc_petra_fabric_connect_mode_set, 
    soc_petra_fabric_connect_mode_get, 
    soc_petra_fabric_fap20_map_set, 
    soc_petra_fabric_fap20_map_get,
    NULL, /*mbcm_dpp_fabric_priority_set*/
    NULL, /*mbcm_dpp_fabric_priority_get*/
    soc_petra_fabric_topology_status_connectivity_get, 
    soc_petra_fabric_links_status_get,
    NULL, /*mbcm_dpp_fabric_aldwp_config*/ 
    soc_petra_fabric_topology_status_connectivity_print, 
    soc_petra_fabric_nof_links_get,
    NULL, /*mbcm_dpp_fabric_gci_enable_set*/
    NULL, /*mbcm_dpp_fabric_gci_enable_get*/
    NULL, /*mbcm_dpp_fabric_gci_config_set*/
    NULL, /*mbcm_dpp_fabric_gci_config_get*/
    NULL, /*mbcm_dpp_fabric_gci_backoff_masks_init*/
    NULL, /*mbcm_dpp_fabric_llfc_threshold_set*/
    NULL, /*mbcm_dpp_fabric_llfc_threshold_get*/
    NULL, /*mbcm_dpp_fabric_rci_enable_set*/
    NULL, /*mbcm_dpp_fabric_rci_enable_get*/
    NULL, /*mbcm_dpp_fabric_rci_config_set*/
    NULL, /*mbcm_dpp_fabric_rci_config_get*/
    NULL, /*mbcm_dpp_fabric_minimal_links_to_dest_set*/
    NULL, /*mbcm_dpp_fabric_minimal_links_to_dest_get*/
    NULL, /*mbcm_dpp_fabric_minimal_links_all_reachable_set*/
    NULL, /*mbcm_dpp_fabric_minimal_links_all_reachable_get*/
    NULL, /*mbcm_dpp_fabric_link_tx_traffic_disable_set*/
    NULL, /*mbcm_dpp_fabric_link_tx_traffic_disable_get*/
    NULL, /*mbcm_dpp_fabric_link_thresholds_pipe_set */
    NULL, /*mbcm_dpp_fabric_link_thresholds_pipe_get */
    NULL, /*mbcm_dpp_fabric_cosq_control_backward_flow_control_set */
    NULL, /*mbcm_dpp_fabric_cosq_control_backward_flow_control_get */
    NULL, /*mbcm_dpp_fabric_egress_core_cosq_gport_sched_set */
    NULL, /*mbcm_dpp_fabric_egress_core_cosq_gport_sched_get */
    NULL, /*mbcm_dpp_fabric_cosq_gport_rci_threshold_set */
    NULL, /*mbcm_dpp_fabric_cosq_gport_rci_threshold_get */
    NULL, /*mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_set */
    NULL, /*mbcm_dpp_fabric_cosq_gport_priority_drop_threshold_get */
    NULL, /*mbcm_dpp_fabric_link_topology_set*/
    NULL, /*mbcm_dpp_fabric_link_topology_get */
    NULL, /*mbcm_dpp_fabric_link_topology_unset */
    NULL, /*mbcm_dpp_fabric_multicast_set */
    NULL, /*mbcm_dpp_fabric_multicast_get */
    NULL, /*mbcm_dpp_fabric_modid_group_set */
    NULL, /*mbcm_dpp_fabric_modid_group_get */
    NULL, /*mbcm_dpp_fabric_local_dest_id_verify */
    NULL, /*mbcm_dpp_fabric_rci_thresholds_config_set*/
    NULL, /*mbcm_dpp_fabric_rci_thresholds_config_get*/
    NULL, /*mbcm_dpp_fabric_link_repeater_enable_set*/
    NULL, /*mbcm_dpp_fabric_link_repeater_enable_get*/
    NULL, /*mbcm_dpp_fabric_queues_info_get*/
    NULL, /*mbcm_dpp_fabric_cpu2cpu_write*/ 
    NULL, /*mbcm_dpp_fabric_mesh_topology_get*/
    NULL, /*mbcm_dpp_fabric_rx_fifo_status_get*/
    NULL, /*mbcm_dpp_fabric_port_sync_e_link_set*/
    NULL, /*mbcm_dpp_fabric_port_sync_e_link_get*/
    NULL, /*mbcm_dpp_port_sync_e_divider_set*/
    NULL, /*mbcm_dpp_port_sync_e_divider_get*/
    NULL, /*mbcm_dpp_fabric_sync_e_enable_set*/
    NULL, /*mbcm_dpp_fabric_sync_e_enable_get*/
    NULL, /*mbcm_dpp_fabric_force_set*/   
    NULL, /*mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_set */
    NULL, /*mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_get */
    NULL, /*mbcm_dpp_fabric_stack_module_max_all_reachable_set */
    NULL, /*mbcm_dpp_fabric_stack_module_max_all_reachable_get*/ 
    NULL, /*mbcm_dpp_fabric_stack_module_max_set */
    NULL, /*mbcm_dpp_fabric_stack_module_max_get */ 
    NULL, /*mbcm_dpp_fabric_stack_module_devide_by_32_verify */
    NULL, /*mbcm_dpp_fabric_cell_cpu_data_get*/
    soc_petra_ipq_explicit_mapping_mode_info_set, 
    soc_petra_ipq_explicit_mapping_mode_info_get, 
    soc_petra_ipq_traffic_class_map_set, 
    soc_petra_ipq_traffic_class_map_get, 
    NULL, /* mbcm_dpp_ipq_traffic_class_multicast_priority_map_set_f */
    NULL, /* mbcm_dpp_ipq_traffic_class_multicast_priority_map_get_f */
    soc_petra_ipq_destination_id_packets_base_queue_id_set, 
    soc_petra_ipq_destination_id_packets_base_queue_id_get, 
    soc_petra_ipq_queue_interdigitated_mode_set, 
    soc_petra_ipq_queue_interdigitated_mode_get, 
    soc_petra_ipq_queue_to_flow_mapping_set, 
    soc_petra_ipq_queue_to_flow_mapping_get, 
    soc_petra_ipq_queue_qrtt_unmap, 
    soc_petra_ipq_quartet_reset, 
    soc_petra_ipq_attached_flow_port_get,
    NULL, /* mbcm_dpp_ipq_tc_profile_set_f */
    NULL, /* mbcm_dpp_ipq_tc_profile_get_f */
    NULL, /* mbcm_dpp_ipq_tc_profile_map_set_f */
    NULL, /* mbcm_dpp_ipq_tc_profile_map_get_f */
    NULL, /* mbcm_dpp_ipq_stack_lag_packets_base_queue_id_set_f */
    NULL, /* mbcm_dpp_ipq_stack_lag_packets_base_queue_id_get_f */
    NULL, /* mbcm_dpp_ipq_stack_fec_map_stack_lag_set_f */
    NULL, /* mbcm_dpp_ipq_stack_fec_map_stack_lag_get_f */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_bandwidth_set */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_bandwidth_get */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_bandwidth_set */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_bandwidth_get */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_sched_set */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_sched_get */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_sched_set */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_sched_get */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_burst_set */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_burst_get */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_burst_set */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_burst_get */ 
    NULL, /* mbcm_dpp_ingress_scheduler_clos_slow_start_set */
    NULL, /* mbcm_dpp_ingress_scheduler_clos_slow_start_get */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_slow_start_set */
    NULL, /* mbcm_dpp_ingress_scheduler_mesh_slow_start_get */  
    NULL, /* mbcm_dpp_ingress_scheduler_init */                     
    NULL, /* mbcm_dpp_cosq_control_range_dram_mix_dbuff_threshold_get */
    NULL, /* mbcm_dpp_cosq_control_range_dram_mix_dbuff_threshold_set */
    NULL, /* mbcm_dpp_cosq_control_range_ocb_committed_multicast_get */
    NULL, /* mbcm_dpp_cosq_control_range_ocb_committed_multicast_set */
    NULL, /* mbcm_dpp_cosq_control_range_ocb_eligible_multicast_get */
    NULL, /* mbcm_dpp_cosq_control_range_ocb_eligible_multicast_set */
    soc_petra_itm_dram_buffs_get,
    NULL, /* mbcm_dpp_dram_info_verify, */
    NULL, /* mbcm_dpp_user_buffer_dram_write, */
    NULL, /* mbcm_dpp_user_buffer_dram_read,  */
    NULL, /* mbcm_dpp_cache_table_update_all */
    soc_petra_itm_glob_rcs_fc_set, 
    soc_petra_itm_glob_rcs_fc_get, 
    soc_petra_itm_glob_rcs_drop_set, 
    soc_petra_itm_glob_rcs_drop_get, 
    soc_petra_itm_category_rngs_set, 
    soc_petra_itm_category_rngs_get, 
    soc_petra_itm_admit_test_tmplt_set, 
    soc_petra_itm_admit_test_tmplt_get, 
    soc_petra_itm_init, 
    soc_petra_itm_cr_request_set, 
    soc_petra_itm_cr_request_get, 
    soc_petra_itm_cr_discount_set, 
    soc_petra_itm_cr_discount_get, 
    soc_petra_itm_queue_test_tmplt_set, 
    soc_petra_itm_queue_test_tmplt_get, 
    soc_petra_itm_wred_exp_wq_set, 
    soc_petra_itm_wred_exp_wq_get, 
    soc_petra_itm_wred_set, 
    soc_petra_itm_wred_get, 
    soc_petra_itm_tail_drop_set, 
    soc_petra_itm_tail_drop_get, 
    soc_petra_itm_cr_wd_set, 
    soc_petra_itm_cr_wd_get, 
    NULL, /* mbcm_dpp_itm_enable_ecn */
    NULL, /* mbcm_dpp_itm_get_ecn_enabled */
    soc_petra_itm_vsq_qt_rt_cls_set, 
    soc_petra_itm_vsq_qt_rt_cls_get, 
    soc_petra_itm_vsq_fc_set, 
    soc_petra_itm_vsq_fc_get, 
    soc_petra_itm_vsq_tail_drop_set, 
    soc_petra_itm_vsq_tail_drop_get, 
    soc_petra_itm_vsq_tail_drop_default_get, 
    soc_petra_itm_vsq_wred_gen_set, 
    soc_petra_itm_vsq_wred_gen_get, 
    soc_petra_itm_vsq_wred_set, 
    soc_petra_itm_vsq_wred_get, 
    soc_petra_itm_vsq_counter_set, 
    soc_petra_itm_vsq_counter_get, 
    soc_petra_itm_vsq_counter_read, 
    soc_petra_itm_queue_info_set, 
    soc_petra_itm_queue_info_get, 
    soc_petra_itm_ingress_shape_set, 
    soc_petra_itm_ingress_shape_get, 
    soc_petra_itm_priority_map_tmplt_set, 
    soc_petra_itm_priority_map_tmplt_get, 
    soc_petra_itm_priority_map_tmplt_select_set, 
    soc_petra_itm_priority_map_tmplt_select_get, 
    soc_petra_itm_sys_red_drop_prob_set, 
    soc_petra_itm_sys_red_drop_prob_get, 
    soc_petra_itm_sys_red_queue_size_boundaries_set, 
    soc_petra_itm_sys_red_queue_size_boundaries_get, 
    soc_petra_itm_sys_red_q_based_set, 
    soc_petra_itm_sys_red_q_based_get, 
    soc_petra_itm_sys_red_eg_set, 
    soc_petra_itm_sys_red_eg_get, 
    soc_petra_itm_sys_red_glob_rcs_set, 
    soc_petra_itm_sys_red_glob_rcs_get, 
    soc_petra_itm_vsq_index_global2group, 
    soc_petra_itm_vsq_index_group2global, 
    NULL, /* mbcm_dpp_itm_dyn_total_thresh_set */
    soc_petra_mult_eg_vlan_membership_group_range_set, 
    soc_petra_mult_eg_vlan_membership_group_range_get, 
    soc_petra_mult_eg_group_open, 
    soc_petra_mult_eg_group_update, 
    NULL, /* mbcm_dpp_mult_eg_group_set */
    soc_petra_mult_eg_group_close, 
    soc_petra_mult_eg_port_add, 
    NULL, /*dpp_mult_eg_reps_add*/
    soc_petra_mult_eg_port_remove, 
    NULL, /*dpp_mult_eg_reps_remove*/
    soc_petra_mult_eg_group_size_get, 
    soc_petra_mult_eg_group_get, 
    NULL, /* mbcm_dpp_mult_eg_get_group */ 
    soc_petra_mult_eg_vlan_membership_group_open, 
    NULL, /* mbcm_dpp_mult_eg_bitmap_group_create */ 
    soc_petra_mult_eg_vlan_membership_group_update, 
    soc_petra_mult_eg_vlan_membership_group_close, 
    soc_petra_mult_eg_vlan_membership_port_add, 
    soc_petra_mult_eg_vlan_membership_port_remove, 
    NULL, /* dpp_mult_eg_bitmap_group_bm_add */
    NULL, /* dpp_mult_eg_bitmap_group_bm_remove */
    soc_petra_mult_eg_vlan_membership_group_get, 
    soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_set, 
    soc_petra_mult_fabric_traffic_class_to_multicast_cls_map_get, 
    soc_petra_mult_fabric_base_queue_set, 
    soc_petra_mult_fabric_base_queue_get, 
    NULL, /* soc_petra_mult_fabric_credit_source_set, */
    NULL, /* soc_petra_mult_fabric_credit_source_get, */
    soc_petra_mult_fabric_enhanced_set, 
    soc_petra_mult_fabric_enhanced_get, 
    NULL, /*mbcm_dpp_mult_fabric_flow_control_set*/
    NULL, /*mbcm_dpp_mult_fabric_flow_control_get*/
    soc_petra_mult_fabric_active_links_set, 
    soc_petra_mult_fabric_active_links_get, 
    NULL, /* mbcm_dpp_arad_mult_does_group_exist */ 
    soc_petra_mult_ing_traffic_class_map_set, 
    soc_petra_mult_ing_traffic_class_map_get, 
    soc_petra_mult_ing_group_open, 
    soc_petra_mult_ing_group_update, 
    soc_petra_mult_ing_group_close, 
    soc_petra_mult_ing_destination_add, 
    NULL, /* mbcm_dpp_mult_ing_encode_entry */
    soc_petra_mult_ing_destination_remove, 
    soc_petra_mult_ing_group_size_get, 
    soc_petra_mult_ing_group_get, 
    NULL, /* soc_petra_mult_ing_get_group */ 
    /* soc_petra_ofp_rates_set,
    soc_petra_ofp_rates_get, 
    soc_petra_ofp_all_ofp_rates_get, 
    soc_petra_ofp_rates_single_port_set, 
    soc_petra_ofp_rates_single_port_get, 
    soc_petra_ofp_rates_mal_shaper_set, 
    soc_petra_ofp_rates_mal_shaper_get, 
    soc_petra_ofp_rates_egq_calendar_validate, 
    soc_petra_ofp_rates_update_device_set, 
    soc_petra_ofp_rates_update_device_get,  */
    soc_petra_pkt_packet_callback_set, 
    soc_petra_pkt_packet_callback_get, 
    soc_petra_pkt_packet_send, 
    soc_petra_pkt_packet_recv, 
    soc_petra_pkt_packet_receive_mode_set, 
    soc_petra_pkt_packet_receive_mode_get, 
    soc_petra_sys_phys_to_local_port_map_set, 
    soc_petra_sys_phys_to_local_port_map_get, 
    soc_petra_local_to_sys_phys_port_map_get, 
    NULL, /* soc_petra_modport_to_sys_phys_port_map_get */
    soc_petra_port_to_interface_map_set_dispatch, 
    soc_petra_port_to_interface_map_get_dispatch,
    soc_petra_ports_is_port_lag_member,
    soc_petra_ports_lag_set, 
    soc_petra_ports_lag_get, 
    soc_petra_ports_lag_sys_port_add, 
    soc_petra_ports_lag_member_add, 
    soc_petra_ports_lag_sys_port_remove, 
    soc_petra_ports_lag_sys_port_info_get, 
    soc_petra_ports_lag_order_preserve_set, 
    soc_petra_ports_lag_order_preserve_get, 
    soc_petra_port_header_type_set, 
    soc_petra_port_header_type_get, 
    soc_petra_ports_mirror_inbound_set, 
    soc_petra_ports_mirror_inbound_get, 
    soc_petra_ports_mirror_outbound_set, 
    soc_petra_ports_mirror_outbound_get, 
    soc_petra_ports_snoop_set, 
    soc_petra_ports_snoop_get, 
    soc_petra_ports_itmh_extension_set, 
    soc_petra_ports_itmh_extension_get, 
    soc_petra_ports_shaping_header_set, 
    soc_petra_ports_shaping_header_get, 
    soc_petra_ports_forwarding_header_set, 
    soc_petra_ports_forwarding_header_get, 
    soc_petra_ports_stag_set, 
    soc_petra_ports_stag_get, 
    soc_petra_ports_ftmh_extension_set, 
    soc_petra_ports_ftmh_extension_get,
    NULL, /* mbcm_dpp_ports_reference_clock_set */
    NULL, /* mbcm_dpp_ports_port_to_nif_id_get */     
    soc_petra_port_egr_hdr_credit_discount_type_set, 
    soc_petra_port_egr_hdr_credit_discount_type_get, 
    soc_petra_port_egr_hdr_credit_discount_select_set, 
    soc_petra_port_egr_hdr_credit_discount_select_get, 
    NULL, /* mbcm_dpp_port_stacking_info_set_f */ 
    NULL, /* mbcm_dpp_port_stacking_info_get_f */
    NULL, /* mbcm_dpp_port_stacking_route_history_bitmap_set_f */
    NULL, /* mbcm_dpp_port_direct_lb_key_set */
    NULL, /* mbcm_dpp_port_direct_lb_key_get */
    NULL, /* mbcm_port_direct_lb_key_min_set */
    NULL, /* mbcm_port_direct_lb_key_max_set */
    NULL, /* mbcm_port_direct_lb_key_min_get */
    NULL, /* mbcm_port_direct_lb_key_max_get */
    NULL, /* mbcm_dpp_port_synchronize_lb_key_tables_at_egress_f */
    NULL, /* mbcm_dpp_port_switch_lb_key_tables_f */
    NULL, /* arad_port_rx_enable_get,*/
    NULL, /* arad_port_rx_enable_set,*/
    NULL, /* mbcm_dpp_port_ingr_reassembly_context_get,*/
    NULL, /* mbcm_dpp_port_rate_egress_pps_set*/
    NULL, /* mbcm_dpp_port_rate_egress_pps_get*/
    NULL, /* mbcm_dpp_port_protocol_offset_verify */
    NULL, /* mbcm_dpp_port_cable_diag */
    NULL, /* arad_ports_swap_set */
    NULL, /* arad_ports_swap_get */
    NULL, /* mbcm_dpp_ports_pon_tunnel_info_set_f */
    NULL, /* mbcm_dpp_ports_pon_tunnel_info_get_f */
    NULL, /* mbcm_dpp_ports_extender_mapping_enable_set_f */
    NULL, /* mbcm_dpp_ports_extender_mapping_enable_get_f */
    soc_petra_read_fld, 
    soc_petra_write_fld, 
    soc_petra_read_reg, 
    soc_petra_write_reg, 
    soc_petra_status_fld_poll, 
    soc_petra_mgmt_credit_worth_set, 
    soc_petra_mgmt_credit_worth_get_dispatch, 
    NULL, /* mbcm_dpp_mgmt_module_to_credit_worth_map_set_f */
    NULL, /* mbcm_dpp_mgmt_module_to_credit_worth_map_get_f */
  NULL, /*mbcm_dpp_mgmt_credit_worth_remote_set*/
  NULL, /*mbcm_dpp_mgmt_credit_worth_remote_get*/
  NULL, /*mbcm_dpp_mgmt_change_all_faps_credit_worth_unsafe*/
    soc_petra_mgmt_all_ctrl_cells_enable_get, 
    soc_petra_mgmt_all_ctrl_cells_enable_set, 
    NULL, /* mbcm_dpp_force_tdm_bypass_traffic_to_fabric_set */
    NULL, /* mbcm_dpp_force_tdm_bypass_traffic_to_fabric_get */
    soc_petra_mgmt_enable_traffic_set, 
    soc_petra_mgmt_enable_traffic_get, 
    soc_pb_register_device, 
    soc_pb_unregister_device, 
    soc_petra_mgmt_system_fap_id_set, 
    soc_petra_mgmt_system_fap_id_get,
    NULL, /*soc_petra_mgmt_tm_domain_set*/
    NULL, /*soc_petra_mgmt_tm_domain_get*/
    soc_petra_hpu_itmh_build_verify, 
    soc_petra_hpu_ftmh_build_verify, 
    soc_petra_hpu_otmh_build_verify, 
    soc_petra_hpu_itmh_build, 
    soc_petra_hpu_itmh_parse, 
    soc_petra_hpu_ftmh_build, 
    soc_petra_hpu_ftmh_parse, 
    soc_petra_hpu_otmh_build, 
    soc_petra_hpu_otmh_parse, 
    NULL, /* arad_loopback_set*/
    NULL, /* arad_loopback_get*/
    NULL, /* mbcm_dpp_nif_synce_clk_sel_port_set */
    NULL, /* mbcm_dpp_nif_synce_clk_sel_port_get */
    NULL, /* mbcm_dpp_nif_synce_clk_div_set */
    NULL, /* mbcm_dpp_nif_synce_clk_div_get */
    soc_petra_flow_and_up_info_get,
    soc_petra_ips_non_empty_queues_info_get,
    NULL, /*mbcm_dpp_itm_pfc_tc_map_set*/
    NULL, /*mbcm_dpp_itm_pfc_tc_map_get*/ 
    NULL, /*soc_pb_fc_gen_cal_set, */
    NULL, /*soc_pb_fc_gen_cal_get, */
    NULL, /*soc_pb_fc_gen_inbnd_set, */
    NULL, /*soc_pb_fc_gen_inbnd_get, */
    NULL, /*soc_pb_fc_gen_inbnd_glb_hp_set, */
    NULL, /*soc_pb_fc_gen_inbnd_glb_hp_get, */
    NULL, /*soc_pb_fc_rec_cal_set, */
    NULL, /*soc_pb_fc_rec_cal_get, */
    NULL, /* mbcm_dpp_fc_pfc_generic_bitmap_set */
    NULL, /* mbcm_dpp_fc_pfc_generic_bitmap_get */
    NULL, /* mbcm_dpp_fc_port_fifo_threshold_set */
    NULL, /* mbcm_dpp_fc_port_fifo_threshold_get */
    NULL, /* mbcm_dpp_egr_dsp_pp_to_base_q_pair_get */
    NULL, /* mbcm_dpp_egr_dsp_pp_to_base_q_pair_set */
    NULL, /* mbcm_dpp_egr_dsp_pp_priorities_mode_get */
    NULL, /* mbcm_dpp_egr_dsp_pp_priorities_mode_set */
    NULL, /* mbcm_dpp_egr_dsp_pp_shaper_mode_set */
    NULL, /*mbcm_dpp_egr_queuing_dev_set*/
    NULL, /*mbcm_dpp_egr_queuing_dev_get*/
    NULL, /*mbcm_dpp_egr_queuing_global_drop_set*/
    NULL, /*mbcm_dpp_egr_queuing_global_drop_get*/
    NULL, /*mbcm_dpp_egr_queuing_sp_tc_drop_set*/
    NULL, /*mbcm_dpp_egr_queuing_sp_tc_drop_get*/
    NULL, /*mbcm_dpp_egr_queuing_sch_unsch_drop_set*/
    NULL, /*mbcm_dpp_egr_queuing_sp_reserved_set*/
    NULL, /*mbcm_dpp_egr_queuing_global_fc_set*/
    NULL, /*mbcm_dpp_egr_queuing_global_fc_get*/
    NULL, /*mbcm_dpp_egr_queuing_mc_tc_fc_set*/
    NULL, /*mbcm_dpp_egr_queuing_mc_tc_fc_get*/
    NULL, /*mbcm_dpp_egr_queuing_mc_cos_map_set*/
    NULL, /*mbcm_dpp_egr_queuing_mc_cos_map_get*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_set*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_get*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_uc_set*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_uc_get*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_mc_set*/
    NULL, /*mbcm_dpp_egr_queuing_if_fc_mc_get*/
    NULL, /*mbcm_dpp_egr_queuing_if_uc_map_set*/
    NULL, /*mbcm_dpp_egr_queuing_if_mc_map_set*/
    NULL, /*mbcm_dpp_egr_queuing_is_high_priority_port_get*/
    /*mbcm_dpp_ofp_rates_port_priority_shaper_set*/
    /*mbcm_dpp_ofp_rates_port_priority_shaper_get*/
    /*mbcm_dpp_ofp_rates_tcg_shaper_set*/
    /*mbcm_dpp_ofp_rates_tcg_shaper_get*/
    NULL, /*mbcm_dpp_egr_queuing_ofp_tcg_set*/
    NULL, /*mbcm_dpp_egr_queuing_ofp_tcg_get*/
    NULL, /*mbcm_dpp_egr_queuing_egr_interface_alloc*/
    NULL, /*mbcm_dpp_egr_queuing_egr_interface_free*/
    NULL, /*mbcm_dpp_egr_queuing_tcg_weight_set*/
    NULL, /*mbcm_dpp_egr_queuing_tcg_weight_get*/
    NULL, /* mbcm_dpp_egr_queuing_nrdy_th_profile_data_set */
    NULL, /* mbcm_dpp_egr_queuing_nrdy_th_profile_data_get */
    NULL, /*mbcm_dpp_sch_port_tcg_weight_set*/
    NULL, /*mbcm_dpp_sch_port_tcg_weight_get*/
    NULL, /*soc_pb_mgmt_max_pckt_size_set,*/
    NULL, /* soc_pb_mgmt_max_pckt_size_get, */
    NULL, /* mbcm_dpp_mgmt_ocb_voq_eligible_dynamic_set */ 
    NULL, /*mbcm_dpp_mult_cud_to_port_map_set;*/
    NULL, /*mbcm_dpp_mult_cud_to_port_map_get;*/
    NULL, /*mbcm_dpp_nof_interrupts*/
    NULL, /*mbcm_dpp_mgmt_nof_block_instances*/
    NULL, /*mbcm_dpp_mgmt_temp_pvt_get*/
    NULL, /*mbcm_dpp_mgmt_avs_value_get*/
    soc_pb_itm_dp_discard_set,
    soc_pb_itm_dp_discard_get,
    NULL, /* mbcm_dpp_itm_alpha_set */
    NULL, /* mbcm_dpp_itm_alpha_get */
    NULL, /* mbcm_dpp_itm_fair_adaptive_tail_drop_enable_set */
    NULL, /* mbcm_dpp_itm_fair_adaptive_tail_drop_enable_get */
    NULL, /*mbcm_dpp_ports_application_mapping_info_set */
    NULL, /*mbcm_dpp_ports_application_mapping_info_get */
    NULL, /*mbcm_dpp_ofp_rates_max_credit_empty_port_set;*/
    NULL, /*mbcm_dpp_ofp_rates_max_credit_empty_port_get;*/
    NULL, /*mbcm_dpp_itm_vsq_index_fc_group2global*/
    soc_petra_allocate_tm_port_in_range_and_recycle_channel,
    soc_petra_free_tm_port_and_recycle_channel,
    soc_petra_info_config_device_ports,
    soc_petra_is_olp,
    soc_petra_is_oamp,
    soc_petra_validate_fabric_mode,
    soc_petra_prop_fap_device_mode_get,
    soc_petra_deinit,
    soc_petra_attach,
    soc_petra_fc_oob_mode_validate,
    NULL, /*soc_petra_ofp_rates_port_priority_max_burst_for_fc_queues_set,*/
    NULL, /*soc_petra_ofp_rates_port_priority_max_burst_for_empty_queues_set,*/
    NULL,/*soc_petra_ofp_rates_port_priority_max_burst_for_fc_queues_get,*/
    NULL,/*soc_petra_ofp_rates_port_priority_max_burst_for_empty_queues_get,*/ 
    soc_petra_ofp_rates_sch_single_port_rate_sw_set,                     
    soc_petra_ofp_rates_sch_single_port_rate_hw_set,                     
    soc_petra_ofp_rates_egq_single_port_rate_sw_set_dispatch,
    NULL, /*mbcm_dpp_ofp_rates_egq_single_port_rate_sw_get*/                     
    soc_petra_ofp_rates_egq_single_port_rate_hw_set_dispatch,                     
    soc_petra_ofp_rates_sch_single_port_rate_hw_get,                           
    soc_petra_ofp_rates_egq_single_port_rate_hw_get_dispatch,                           
    soc_petra_ofp_rates_single_port_max_burst_set,                          
    soc_petra_ofp_rates_single_port_max_burst_get,                                               
    soc_petra_ofp_rates_egq_interface_shaper_set_dispatch,                                               
    soc_petra_ofp_rates_egq_interface_shaper_get_dispatch,           
    NULL, /*soc_petra_ofp_rates_egq_tcg_rate_sw_set,*/                             
    NULL, /*soc_petra_ofp_rates_egq_tcg_rate_hw_set,*/                             
    NULL, /*soc_petra_ofp_rates_sch_tcg_rate_set,*/                                   
    NULL, /*soc_petra_ofp_rates_egq_tcg_rate_hw_get,*/                                   
    NULL, /*soc_petra_ofp_rates_sch_tcg_rate_get,*/                                   
    NULL, /*soc_petra_ofp_rates_egq_tcg_max_burst_set,*/                              
    NULL, /*soc_petra_ofp_rates_sch_tcg_max_burst_set,*/                              
    NULL, /*soc_petra_ofp_rates_egq_tcg_max_burst_get,*/                              
    NULL, /*soc_petra_ofp_rates_sch_tcg_max_burst_get,*/                              
    NULL, /*soc_petra_ofp_rates_egq_port_priority_rate_sw_set,*/                   
    NULL, /*soc_petra_ofp_rates_egq_port_priority_rate_hw_set,  */                 
    NULL, /*soc_petra_ofp_rates_sch_port_priority_rate_set,*/                         
    NULL, /*mbcm_dpp_ofp_rates_sch_port_priority_rate_sw_set*/
    NULL, /*mbcm_dpp_ofp_rates_sch_port_priority_hw_set*/
    NULL, /*soc_petra_ofp_rates_egq_port_priority_rate_hw_get,*/                         
    NULL, /*soc_petra_ofp_rates_sch_port_priority_rate_get,*/                         
    NULL, /*soc_petra_ofp_rates_egq_port_priority_max_burst_set,*/                    
    NULL, /*soc_petra_ofp_rates_sch_port_priority_max_burst_set,*/                    
    NULL, /*soc_petra_ofp_rates_egq_port_priority_max_burst_get,*/                    
    NULL, /*soc_petra_ofp_rates_sch_port_priority_max_burst_get*/
    NULL, /*mbcm_dpp_ofp_rates_port2chan_cal_get*/
    NULL, /*mbcm_dpp_ofp_rates_retrieve_egress_shaper_reg_field_names*/
    NULL, /*mbcm_dpp_ofp_rates_egress_shaper_reg_field_read*/
    NULL, /*mbcm_dpp_ofp_rates_egress_shaper_reg_field_write*/
    NULL, /*mbcm_dpp_ofp_rates_egq_scm_chan_arb_id2scm_id*/
    NULL, /*mbcm_dpp_ofp_rates_interface_internal_rate_get*/
    NULL, /*mbcm_dpp_ofp_rates_packet_mode_packet_size_get*/
    NULL, /*mbcm_dpp_ofp_rates_egress_shaper_cal_write*/
    NULL, /*mbcm_dpp_ofp_rates_egress_shaper_cal_read*/
    NULL, /*soc_petra_port2egress_offset*/  
    NULL, /*soc_petra_egr_is_channelized*/
    NULL, /*soc_petra_sch_cal_tbl_set_unsafe*/
    NULL, /*soc_petra_sch_cal_tbl_get_unsafe*/
    NULL, /*soc_petra_sch_cal_max_size*/          
    petra_soc_dpp_local_to_tm_port_get,
    petra_soc_dpp_local_to_pp_port_get,
    petra_soc_dpp_tm_to_local_port_get,
    petra_soc_dpp_pp_to_local_port_get,
    soc_petra_port_init,
    soc_petra_port_post_init,
    soc_petra_port_deinit,
    soc_petra_port_enable_set,
    soc_petra_port_enable_get,
    soc_petra_port_speed_set,
    soc_petra_port_speed_get,
    NULL, /* mbcm_dpp_port_interface_set */
    soc_petra_port_interface_get,
    NULL, /* mbcm_dpp_port_link_state_get */
    NULL, /* mbcm_dpp_is_supported_encap_get */
    NULL, /* mbcm_dpp_egr_q_nif_cal_set */
    NULL, /* mbcm_dpp_egr_q_fast_port_set */
    NULL, /* arad_parser_nof_bytes_to_remove_set */
    NULL, /* arad_ps_db_find_free_binding_ps */
    NULL, /* arad_ps_db_release_binding_ps */
    NULL, /* arad_ps_db_alloc_binding_ps_with_id */    
    NULL, /* arad_egr_prog_editor_profile_set */
    NULL, /* mbcm_dpp_fc_pfc_mapping_set */
    NULL, /* mbcm_dpp_fc_pfc_mapping_get */
    soc_petra_port_probe,
    NULL, /* mbcm_dpp_port_detach */
    NULL, /* soc_dcmn_port_control_pcs_set */
    NULL, /* soc_dcmn_port_control_pcs_get */
    NULL, /* soc_dcmn_port_control_power_set */
    NULL, /* soc_dcmn_port_control_power_get */
    NULL, /* mbcm_dpp_port_control_rx_enable_set */
    NULL, /* mbcm_dpp_port_control_tx_enable_set */
    NULL, /* mbcm_dpp_port_control_rx_enable_get */
    NULL, /* mbcm_dpp_port_control_tx_enable_get */
    NULL, /* mbcm_dpp_port_control_strip_crc_set */
    NULL, /* mbcm_dpp_port_control_strip_crc_get */
    NULL, /* mbcm_dpp_port_prbs_tx_enable_set */    
    NULL, /* mbcm_dpp_port_prbs_tx_enable_get */       
    NULL, /* mbcm_dpp_port_prbs_rx_enable_set */       
    NULL, /* mbcm_dpp_port_prbs_rx_enable_get */      
    NULL, /* mbcm_dpp_port_prbs_rx_status_get */      
    NULL, /* mbcm_dpp_port_prbs_polynomial_set */      
    NULL, /* mbcm_dpp_port_prbs_polynomial_get */     
    NULL, /* mbcm_dpp_port_prbs_tx_invert_data_set */  
    NULL, /* mbcm_dpp_port_prbs_tx_invert_data_get */  
    NULL, /* mbcm_dpp_port_pfc_refresh_set */
    NULL, /* mbcm_dpp_port_pfc_refresh_get */
    NULL, /* mbcm_dpp_local_fault_clear */
    NULL, /* mbcm_dpp_remote_fault_clear */
    NULL, /* mbcm_dpp_pad_size_set */
    NULL, /* mbcm_dpp_pad_size_get */
    NULL, /* mbcm_dpp_port_reset */
    NULL, /* mbcm_dpp_port_phy_control_set */
    NULL, /* mbcm_dpp_port_phy_control_get */
    NULL, /* mbcm_dpp_port_phy_get */
    NULL, /* mbcm_dpp_port_phy_set */
    NULL, /* mbcm_dpp_port_phy_modify */
    NULL, /* mbcm_dpp_port_mac_sa_set */
    NULL, /* mbcm_dpp_port_mac_sa_get */
    NULL, /* mbcm_dpp_port_eee_enable_get */
    NULL, /* mbcm_dpp_port_eee_enable_set */
    NULL, /* mbcm_dpp_port_eee_tx_idle_time_get */
    NULL, /* mbcm_dpp_port_eee_tx_idle_time_set */
    NULL, /* mbcm_dpp_port_eee_tx_wake_time_get */
    NULL, /* mbcm_dpp_port_eee_tx_wake_time_set */
    NULL, /* mbcm_dpp_port_eee_link_active_duration_get */
    NULL, /* mbcm_dpp_port_eee_link_active_duration_set */
    NULL, /* mbcm_dpp_port_eee_statistics_clear */
    NULL, /* mbcm_dpp_port_eee_event_count_symmetric_set */
    NULL, /* mbcm_dpp_port_eee_tx_event_count_get */
    NULL, /* mbcm_dpp_port_eee_tx_duration_get */
    NULL, /* mbcm_dpp_port_eee_rx_event_count_get */
    NULL, /* mbcm_dpp_port_eee_rx_duration_get */
    NULL, /* mbcm_dpp_port_eee_event_count_symmetric_get */
    NULL, /* mbcm_fc_enables_set_f */
    NULL, /* mbcm_fc_enables_get_f */
    NULL, /* mbcm_fc_ilkn_mub_channel_set_f */
    NULL, /* mbcm_fc_ilkn_mub_channel_get_f */
    NULL, /* mbcm_fc_ilkn_mub_gen_cal_set_f */
    NULL, /* mbcm_fc_ilkn_mub_gen_cal_get_f */
    NULL, /* mbcm_fc_cat_2_tc_hcfc_bitmap_set_f */
    NULL, /* mbcm_fc_cat_2_tc_hcfc_bitmap_get_f */
    NULL, /* mbcm_fc_glb_hcfc_bitmap_set_f */
    NULL, /* mbcm_fc_glb_hcfc_bitmap_get_f */
    NULL, /* mbcm_fc_inbnd_mode_set_f */
    NULL, /* mbcm_fc_inbnd_mode_get_f */
    NULL, /* mbcm_fc_glb_rcs_mask_set_f */
    NULL, /* mbcm_fc_glb_rcs_mask_get_f */
    NULL, /* mbcm_fc_init_pfc_mapping_f */
    NULL, /* soc_pb_fc_ilkn_llfc_set, */
    NULL, /* soc_pb_fc_ilkn_llfc_get  */
    NULL, /* tx_average_ipg_set, */
    NULL, /* tx_average_ipg_get  */
    NULL, /* mbcm_dpp_port_link_get */
    NULL, /* soc_jer_port_autoneg_get */
    NULL, /* mbcm_dpp_port_mdix_set */
    NULL, /* mbcm_dpp_port_mdix_get */
    NULL, /* mbcm_dpp_port_mdix_status_get */
    NULL, /* soc_jer_port_duplex_set */
    NULL, /* portmod_port_duplex_get */
    NULL, /* mbcm_dpp_fc_hcfc_watchdog_set_f */
    NULL, /* mbcm_dpp_fc_hcfc_watchdog_get_f */
	NULL, /* mbcm_dpp_port_fault_get */
    NULL /* mbcm_dpp_fabric_link_fault_get */
};

