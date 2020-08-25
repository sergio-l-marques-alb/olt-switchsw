
/*
 * $Id: mbcm.c $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        mbcm.c
 */

#include <soc/dnx/legacy/mbcm.h>

/* Arad includes */
#include <soc/dnx/legacy/ARAD/arad_ingress_traffic_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_ingress_packet_queuing.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_multicast_fabric.h>
#if (0)
/* { */
#include <soc/dnx/legacy/ARAD/arad_ofp_rates.h>
#include <soc/dnx/legacy/ARAD/arad_ofp_rates.h>
/* } */
#endif
#include <soc/dnx/legacy/ARAD/arad_flow_control.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_fabric.h>
#if (0)
/* { */
#include <soc/dnx/legacy/ARAD/arad_egr_queuing.h>
/* } */
#endif
#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>

/* Jericho includes */
#include <soc/dnx/legacy/JER/jer_ingress_scheduler.h>
#if (0)
/* { */
#include <soc/dnx/legacy/JER/jer_egr_queuing.h>
#include <soc/dnx/legacy/JER/jer_ofp_rates.h>
#include <soc/dnx/legacy/JER/jer_egr_queuing.h>
/* } */
#endif
#include <soc/portmod/portmod.h>
#include <soc/dnx/legacy/JER/jer_flow_control.h>

/* JER2 includes */
#include <soc/dnx/legacy/JER2/jer2_ingress_packet_queuing.h>
#include <soc/dnx/legacy/JER2/jer2_multicast_fabric.h>
#include <soc/dnx/legacy/JER2/jer2_regs.h>
#include <soc/dnx/legacy/JER2/jer2_fabric.h>
#include <bcm_int/dnx/fabric/fabric_transmit.h>

CONST mbcm_dnx_functions_t mbcm_jer2_driver = {
    NULL, /* was jer2_arad_egr_q_prio_set */
    NULL, /* was jer2_arad_egr_q_prio_get */
    NULL, /* Was jer2_arad_egr_q_profile_map_set */
    NULL, /* Was jer2_arad_egr_q_profile_map_get */
    NULL, /* jer2_arad_egr_q_cgm_interface_set*/
    NULL, /* soc_jer2_jer_egr_q_fqp_scheduler_config*/
    NULL, /* soc_jer2_qax_egr_congestion_statistics_get*/
    NULL, /* jer2_arad_fabric_line_coding_set */ 
    NULL, /* jer2_arad_fabric_line_coding_get */ 
    soc_jer2_fmac_block_enable_get,
    soc_jer2_fabric_link_force_signal_set,
    soc_jer2_fabric_link_mac_init,
    soc_jer2_fabric_mac_init,
    soc_jer2_fabric_aluwp_init,
    soc_jer2_fabric_transmit_init,
    soc_jer2_fabric_multicast_init,
    soc_jer2_fabric_scheduler_adaptation_init,
    soc_jer2_fabric_interleaving_init,
    soc_jer2_dtqs_contexts_init,
    NULL, /* soc_jer2_jer_port_fabric_clk_freq_init*/
    NULL, /* jer2_arad_interface_id_verify */
    NULL, /* jer2_arad_if_type_from_id */ 
    NULL, /* jer2_arad_mal_equivalent_id_verify */ 
    NULL, /* soc_jer2_jer_fabric_stat_init*/
    NULL, /* mbcm_dnx_sku_fabric_quad_valid */
    NULL, /* soc_jer2_jer_stat_nif_init*/
    NULL, /* soc_jer2_arad_stat_path_info_get*/
    NULL, /* soc_jer2_jer_info_config_custom_reg_access*/ 
    NULL, /* soc_jer2_jer_mapping_stat_get*/
    NULL, /* soc_jer2_jer_stat_counter_length_get*/
    NULL, /* soc_jer2_jer_stat_controlled_counter_enable_get*/
    NULL, /* mbcm_dnx_max_latency_pkts_get */    
    NULL, /* jer2_jer_itm_rate_limit_mpps_set*/
    NULL, /* jer2_jer_itm_rate_limit_mpps_get*/
    NULL, /* jer2_arad_port_pp_port_set*/
    NULL, /* jer2_arad_port_pp_port_get*/
    NULL, /* jer2_arad_port_to_pp_port_map_set*/
    NULL, /* jer2_arad_port_to_pp_port_map_get */
    NULL, /* soc_dnxc_port_control_low_latency_llfc_set*/
    NULL, /* soc_dnxc_port_control_fec_error_detect_set*/
    NULL, /* soc_dnxc_port_control_low_latency_llfc_get*/
    NULL, /* soc_dnxc_port_control_fec_error_detect_get*/ 
    NULL, /* soc_dnxc_port_extract_cig_from_llfc_enable_set*/
    NULL, /* soc_dnxc_port_extract_cig_from_llfc_enable_get*/
    NULL, /* jer2_arad_port_forwarding_header_set */ 
    NULL, /* jer2_arad_port_forwarding_header_get */
    NULL, /* soc_jer2_jer_port_nrdy_th_profile_set*/
    NULL, /* soc_jer2_jer_port_nrdy_th_profile_get*/
    NULL, /* soc_jer2_qax_port_nrdy_th_optimal_value_get*/
    NULL, /* jer2_arad_ports_logical_sys_id_build*/
    NULL, /* jer2_arad_sys_virtual_port_to_local_port_map_set_unsafe*/
    NULL, /* jer2_arad_ssr_buff_size_get */ 
    NULL, /* jer2_arad_ssr_to_buff */ 
    NULL, /* jer2_arad_ssr_from_buff */ 
    NULL, /* jer2_arad_ssr_is_device_init_done */ 
    NULL, /* jer2_arad_stack_global_info_set */ 
    NULL, /* jer2_arad_stack_global_info_get */ 
    NULL, /* jer2_arad_stack_port_distribution_info_set */ 
    NULL, /* jer2_arad_stack_port_distribution_info_get */
    NULL, /*jer2_arad_tdm_ftmh_set*/
    NULL, /*jer2_arad_tdm_ftmh_get*/
    NULL, /*jer2_jer_tdm_opt_size_set*/
    NULL, /*jer2_jer_tdm_opt_size_get*/
    NULL, /*jer2_arad_tdm_stand_size_range_set*/
    NULL, /*jer2_arad_tdm_stand_size_range_get*/
    NULL, /* jer2_arad_tdm_mc_static_route_set INVALID for JER2_ARAD */ 
    NULL, /* jer2_arad_tdm_mc_static_route_get INVALID for JER2_ARAD */ 
    NULL, /*jer2_arad_tdm_port_packet_crc_set*/
    NULL, /*jer2_arad_tdm_port_packet_crc_get*/
    NULL, /*jer2_arad_tdm_direct_routing_set*/
    NULL, /*jer2_arad_tdm_direct_routing_get*/
    NULL, /*jer2_arad_tdm_direct_routing_profile_map_set*/
    NULL, /*jer2_arad_tdm_direct_routing_profile_map_get*/
    NULL, /* jer2_arad_tdm_ifp_get*/
    NULL, /* jer2_arad_tdm_ifp_set*/
    NULL, /* jer2_arad_read_from_fe600 */ 
    NULL, /* jer2_arad_write_to_fe600 */ 
    NULL, /* jer2_arad_indirect_read_from_fe600 */ 
    NULL, /* jer2_arad_indirect_write_to_fe600 */ 
    NULL, /* jer2_arad_cpu2cpu_write */ 
    NULL, /* jer2_arad_cpu2cpu_read */ 
    NULL, /* jer2_arad_cell_mc_tbl_write */ 
    NULL, /* jer2_arad_cell_mc_tbl_read */ 
    NULL, /* jer2_arad_diag_ipt_rate_get */ 
    NULL, /* jer2_arad_diag_iddr_set */ 
    NULL, /* jer2_arad_diag_iddr_get */ 
    NULL, /* jer2_arad_diag_regs_dump */ 
    NULL, /* jer2_arad_diag_tbls_dump */ 
    NULL, /* jer2_arad_diag_tbls_dump_all */ 
    NULL, /* jer2_arad_nif_diag_last_packet_get */ 
    NULL, /* jer2_arad_diag_soft_error_test_start */ 
    NULL, /* jer2_arad_egq_resources_print */ 
    NULL, /* jer2_jer_egr_threshold_types_verify*/
    NULL, /* jer2_arad_egr_ofp_thresh_type_set*/
    NULL, /* jer2_arad_egr_ofp_thresh_type_get*/
    NULL, /* jer2_arad_egr_sched_drop_set*/
    NULL, /* jer2_arad_egr_sched_drop_get*/
    NULL, /* jer2_jer_egr_unsched_drop_set*/
    NULL, /* jer2_arad_egr_unsched_drop_get*/
    NULL, /* jer2_qax_egr_dev_fc_set*/
    NULL, /* jer2_qax_egr_dev_fc_get*/
    NULL, /* jer2_arad_egr_xaui_spaui_fc_set */ 
    NULL, /* jer2_arad_egr_xaui_spaui_fc_get */ 
    NULL, /* jer2_qax_egr_ofp_fc_set*/
    NULL, /* jer2_jer_egr_sched_port_fc_thresh_set*/
    NULL, /* jer2_jer_egr_sched_q_fc_thresh_set*/
    NULL, /* jer2_qax_egr_ofp_fc_get*/
    NULL, /* mbcm_dnx_egr_mci_fc_set - Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_mci_fc_get - Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_mci_fc_enable_set -  Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_mci_fc_enable_get - Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_ofp_sch_mode_set - Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_ofp_sch_mode_get - Invalid for JER2_ARAD */ 
    NULL, /* Was jer2_arad_egr_ofp_scheduling_set */
    NULL, /* Was jer2_arad_egr_ofp_scheduling_get */
    NULL, /* mbcm_dnx_egr_unsched_drop_prio_set - Invalid for JER2_ARAD */ 
    NULL, /* mbcm_dnx_egr_unsched_drop_prio_get - Invalid for JER2_ARAD */ 

    NULL, /* soc_jer2_sch_prio_propagation_port_set*/
    NULL, /* soc_jer2_sch_prio_propagation_port_get*/

    NULL, /* jer2_arad_fabric_fc_enable_set*/
    NULL, /* jer2_arad_fabric_fc_enable_get*/    
    NULL, /* jer2_arad_fabric_fc_shaper_get*/
    NULL, /* jer2_arad_fabric_fc_shaper_set*/
    NULL, /* jer2_arad_fabric_cell_format_get */ 
    NULL, /* jer2_arad_fabric_coexist_set */ 
    NULL, /* jer2_arad_fabric_coexist_get */ 
    NULL, /* jer2_arad_fabric_stand_alone_fap_mode_get */ 
    NULL, /* jer2_arad_fabric_connect_mode_set */ 
    NULL, /* jer2_arad_fabric_connect_mode_get */ 
    NULL, /* jer2_arad_fabric_fap20_map_set */ 
    NULL, /* jer2_arad_fabric_fap20_map_get */  
    soc_jer2_fabric_topology_status_connectivity_get,
    soc_jer2_fabric_link_status_get,
    soc_jer2_fabric_aldwp_config,
    NULL, /* jer2_arad_fabric_topology_status_connectivity_print */
    NULL, /* jer2_arad_fabric_nof_links_get*/
    NULL, /* jer2_arad_fabric_gci_enable_set*/
    NULL, /* jer2_arad_fabric_gci_enable_get*/
    NULL, /* jer2_arad_fabric_gci_config_set*/
    NULL, /* jer2_arad_fabric_gci_config_get*/
    NULL, /* jer2_jer_fabric_gci_backoff_masks_init*/
    NULL, /* jer2_arad_fabric_llfc_threshold_set*/
    NULL, /* jer2_arad_fabric_llfc_threshold_get*/
    NULL, /* jer2_arad_fabric_rci_enable_set*/
    NULL, /* jer2_arad_fabric_rci_enable_get*/
    NULL, /* jer2_arad_fabric_rci_config_set*/
    NULL, /* jer2_arad_fabric_rci_config_get*/
    soc_jer2_fabric_minimal_links_to_dest_init,
    soc_jer2_fabric_minimal_links_to_dest_set,
    soc_jer2_fabric_minimal_links_to_dest_get,
    soc_jer2_fabric_minimal_links_all_reachable_set,
    soc_jer2_fabric_minimal_links_all_reachable_get,
    soc_jer2_fabric_port_enable_set,
    soc_jer2_fabric_delay_single_cell_in_fabric_rx_init,
    soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_set,
    soc_jer2_fabric_delay_single_cell_in_fabric_rx_enable_get,
    NULL, /* soc_jer2_qax_fabric_cosq_control_backward_flow_control_set*/
    NULL, /* soc_jer2_qax_fabric_cosq_control_backward_flow_control_get*/
    NULL, /* soc_jer2_qax_fabric_egress_core_cosq_gport_sched_set*/
    NULL, /* soc_jer2_qax_fabric_egress_core_cosq_gport_sched_get*/
    NULL, /* soc_jer2_qax_fabric_cosq_gport_rci_threshold_set*/
    NULL, /* soc_jer2_qax_fabric_cosq_gport_rci_threshold_get*/
    NULL, /* soc_jer2_qax_fabric_cosq_gport_priority_drop_threshold_set*/
    NULL, /* soc_jer2_qax_fabric_cosq_gport_priority_drop_threshold_get*/
    soc_jer2_fabric_load_balance_init,
    NULL, /* soc_jer2_jer_fabric_port_sync_e_link_set*/
    NULL, /* soc_jer2_jer_fabric_port_sync_e_link_get*/
    NULL, /* soc_jer2_jer_fabric_port_sync_e_divider_set*/
    NULL, /* soc_jer2_jer_fabric_port_sync_e_divider_get*/
    NULL, /* soc_jer2_jer_fabric_sync_e_enable_get*/
    soc_jer2_fabric_force_set,
    NULL, /* soc_jer2_jer_fabric_efms_enable_set*/
    NULL, /* soc_jer2_jer_fabric_efms_enable_get*/
    soc_jer2_fabric_reachability_status_get,
    NULL, /* jer2_arad_ipq_traffic_class_multicast_priority_map_set*/
    NULL, /* jer2_arad_ipq_traffic_class_multicast_priority_map_get*/
    NULL, /* jer2_arad_ipq_attached_flow_port_get */ 
    NULL, /* jer2_arad_ipq_stack_lag_packets_base_queue_id_set*/
    NULL, /* jer2_arad_ipq_stack_lag_packets_base_queue_id_get*/
    NULL, /* jer2_arad_ipq_stack_fec_map_stack_lag_set*/
    NULL, /* jer2_arad_ipq_stack_fec_map_stack_lag_get*/
    NULL, /* jer2_qax_ipq_default_invalid_queue_set*/
    NULL, /* jer2_qax_ipq_default_invalid_queue_get*/

    NULL, /* soc_jer2_jer_ocb_control_range_dram_mix_dbuff_threshold_get*/
    NULL, /* soc_jer2_jer_ocb_control_range_dram_mix_dbuff_threshold_set*/
    NULL, /* soc_jer2_jer_ocb_control_range_ocb_committed_multicast_get*/
    NULL, /* soc_jer2_jer_ocb_control_range_ocb_committed_multicast_set*/
    NULL, /* soc_jer2_jer_ocb_control_range_ocb_eligible_multicast_get*/
    NULL, /* soc_jer2_jer_ocb_control_range_ocb_eligible_multicast_set*/
    NULL, /* jer2_arad_itm_dram_buffs_get*/
    NULL, /* soc_jer2_jer_dram_info_verify*/
    NULL, /* soc_jer2_arad_user_buffer_dram_write*/
    NULL, /* soc_jer2_arad_user_buffer_dram_read*/
    NULL, /* soc_dnx_drc_combo28_validate_dram_address*/
    NULL, /* soc_jer2_qax_dram_recovery_init*/
    NULL, /* soc_jer2_arad_cache_table_update_all*/
    NULL, /* jer2_arad_itm_glob_rcs_fc_set*/ 
    NULL, /* jer2_arad_itm_glob_rcs_fc_get*/ 
    NULL, /* jer2_qax_itm_init*/
    NULL, /* jer2_jer_itm_ingress_shape_set*/ 
    NULL, /* jer2_jer_itm_ingress_shape_get*/ 
    NULL, /* jer2_arad_itm_priority_map_tmplt_set*/ 
    NULL, /* jer2_arad_itm_priority_map_tmplt_get*/ 
    NULL, /* jer2_arad_itm_priority_map_tmplt_select_set*/ 
    NULL, /* jer2_arad_itm_priority_map_tmplt_select_get*/ 
 
    NULL, /* dnx_mult_eg_bitmap_group_range_set */
    NULL, /* dnx_mult_eg_bitmap_group_range_get */
    NULL, /* jer2_arad_mult_eg_group_open */
    NULL, /* jer2_arad_mult_eg_group_update */
    NULL, /* jer2_qax_mult_eg_group_set*/
    NULL, /* jer2_qax_mult_eg_group_close*/
    NULL, /*dnx_mult_eg_port_add*/
    NULL, /* jer2_qax_mult_eg_reps_add*/
    NULL, /*dnx_mult_eg_port_remove*/
    NULL, /* jer2_qax_mult_eg_reps_remove*/
    NULL, /* jer2_qax_mult_eg_group_size_get*/
    NULL, /* a jer2_arad_mult_eg_group_get */
    NULL, /* jer2_qax_mult_eg_get_group*/
    NULL, /* dnx_mult_eg_vlan_membership_group_open*/
    NULL, /* dnx_mult_eg_bitmap_group_create */
    NULL, /* dnx_mult_eg_bitmap_group_update */
    NULL, /* dnx_mult_eg_bitmap_group_close */
    NULL, /* dnx_mult_eg_bitmap_group_port_add */
    NULL, /* dnx_mult_eg_bitmap_group_port_remove */
    NULL, /* dnx_mult_eg_bitmap_group_bm_add */
    NULL, /* dnx_mult_eg_bitmap_group_bm_remove */
    NULL, /* dnx_mult_eg_bitmap_group_get */
    NULL, /* jer2_qax_mcds_multicast_init2*/
    NULL, /*jer2_arad_mult_fabric_traffic_class_to_multicast_cls_map_set*/
    NULL, /*jer2_arad_mult_fabric_traffic_class_to_multicast_cls_map_get*/
    NULL, /* jer2_arad_mult_fabric_base_queue_set*/
    NULL, /* jer2_arad_mult_fabric_base_queue_get*/
    jer2_mult_fabric_credit_source_set,
    jer2_mult_fabric_credit_source_get,
    jer2_mult_fabric_enhanced_set,
    jer2_mult_fabric_enhanced_get,
    NULL, /* jer2_arad_mult_fabric_flow_control_set */
    NULL, /* jer2_arad_mult_fabric_flow_control_get */
    NULL, /* jer2_arad_mult_fabric_active_links_set */ 
    NULL, /* jer2_arad_mult_fabric_active_links_get */ 
    NULL, /* jer2_qax_mult_does_group_exist*/
    NULL, /* dnx_mult_ing_traffic_class_map_set*/
    NULL, /* dnx_mult_ing_traffic_class_map_get*/
    NULL, /* jer2_qax_mult_ing_group_open*/
    NULL, /* jer2_qax_mult_ing_group_update*/
    NULL, /* jer2_qax_mult_ing_group_close*/
    NULL, /* jer2_qax_mult_ing_destination_add */
    NULL, /* jer2_qax_mult_ing_encode_entry */
    NULL, /* jer2_qax_mult_ing_destination_remove */
    NULL, /* jer2_qax_mult_ing_group_size_get*/
    NULL, /* jer2_arad_mult_ing_group_get */
    NULL, /* jer2_qax_mult_ing_get_group*/
    NULL, /* jer2_arad_pkt_packet_callback_set */ 
    NULL, /* jer2_arad_pkt_packet_callback_get */ 
    NULL, /* jer2_arad_pkt_packet_send */ 
    NULL, /* jer2_arad_pkt_packet_recv */ 
    NULL, /* jer2_arad_pkt_packet_receive_mode_set */ 
    NULL, /* jer2_arad_pkt_packet_receive_mode_get */ 
    NULL, /*jer2_arad_sys_phys_to_local_port_map_set*/
    NULL, /*jer2_arad_sys_phys_to_local_port_map_get*/
    NULL, /* jer2_arad_local_to_sys_phys_port_map_get*/
    NULL, /* jer2_arad_modport_to_sys_phys_port_map_get*/
    NULL, /* soc_jer2_jer_port_to_interface_map_set*/
    NULL, /* jer2_arad_port_to_interface_map_get*/
    NULL, /* jer2_arad_ports_is_port_lag_member*/
    NULL, /* jer2_arad_ports_lag_set */ 
    NULL, /* jer2_arad_ports_lag_get */ 
    NULL, /* jer2_arad_ports_lag_sys_port_add */ 
    NULL, /* jer2_arad_ports_lag_member_add */ 
    NULL, /* jer2_arad_ports_lag_sys_port_remove */ 
    NULL, /* jer2_arad_ports_lag_sys_port_info_get */ 
    NULL, /* jer2_arad_ports_lag_order_preserve_set */ 
    NULL, /* jer2_arad_ports_lag_order_preserve_get */ 
    NULL, /* soc_jer2_jer_port_header_type_set*/
    NULL, /* jer2_arad_port_header_type_get*/
    NULL, /* jer2_arad_ports_mirror_inbound_set */ 
    NULL, /* jer2_arad_ports_mirror_inbound_get */ 
    NULL, /* jer2_arad_ports_mirror_outbound_set */ 
    NULL, /* jer2_arad_ports_mirror_outbound_get */ 
    NULL, /* jer2_arad_ports_snoop_set */ 
    NULL, /* jer2_arad_ports_snoop_get */ 
    NULL, /* jer2_arad_ports_itmh_extension_set*/
    NULL, /* jer2_arad_ports_itmh_extension_get*/
    NULL, /* jer2_arad_ports_shaping_header_set */ 
    NULL, /* jer2_arad_ports_shaping_header_get */ 
    NULL, /* jer2_arad_ports_forwarding_header_set */ 
    NULL, /* jer2_arad_ports_forwarding_header_get */ 
    NULL, /* jer2_arad_ports_stag_set */ 
    NULL, /* jer2_arad_ports_stag_get */ 
    NULL, /* jer2_arad_ports_ftmh_extension_set*/
    NULL, /* jer2_arad_ports_ftmh_extension_get*/
    NULL, /* soc_jer2_jer_port_reference_clock_set*/
    NULL, /* mbcm_dnx_ports_port_to_nif_id_get */
    NULL, /* soc_jer2_jer_port_mirrored_channel_and_context_map*/
    NULL, /* jer2_arad_port_egr_hdr_credit_discount_type_set*/
    NULL, /* jer2_arad_port_egr_hdr_credit_discount_type_get*/
    NULL, /* jer2_arad_port_egr_hdr_credit_discount_select_set*/
    NULL, /* jer2_arad_port_egr_hdr_credit_discount_select_get*/
    NULL, /* jer2_arad_port_stacking_info_set*/
    NULL, /* jer2_arad_port_stacking_info_get*/
    NULL, /* jer2_arad_port_stacking_route_history_bitmap_set*/
    NULL, /* soc_jer2_jer_trunk_direct_lb_key_set*/
    NULL, /* soc_jer2_jer_trunk_direct_lb_key_get*/
    NULL, /* jer2_arad_port_direct_lb_key_min_set*/
    NULL, /* jer2_arad_port_direct_lb_key_max_set*/
    NULL, /* jer2_arad_port_direct_lb_key_min_get*/
    NULL, /* jer2_arad_port_direct_lb_key_max_get*/
    NULL, /* jer2_arad_port_synchronize_lb_key_tables_at_egress*/
    NULL, /* jer2_arad_port_switch_lb_key_tables*/
    NULL, /* jer2_arad_port_rx_enable_get*/
    NULL, /* jer2_arad_port_rx_enable_set*/
    NULL, /* mbcm_dnx_port_tx_enable_get */ 
    NULL, /* mbcm_dnx_port_tx_enable_set */ 
    NULL, /* soc_jer2_jer_port_ingr_reassembly_context_get*/
    soc_dnxc_port_rate_egress_ppt_set,
    soc_dnxc_port_rate_egress_ppt_get,
    NULL, /* soc_jer2_qax_port_protocol_offset_verify*/
    NULL, /* jer2_arad_port_cable_diag */
    NULL, /* jer2_arad_ports_swap_set*/
    NULL, /* jer2_arad_ports_swap_get*/
    NULL, /* jer2_arad_ports_pon_tunnel_info_set*/
    NULL, /* jer2_arad_ports_pon_tunnel_info_get*/
    NULL, /* jer2_arad_ports_extender_mapping_enable_set*/
    NULL, /* jer2_arad_ports_extender_mapping_enable_get*/
    NULL, /* jer2_arad_ports_tm_port_var_set*/
    NULL, /* jer2_arad_ports_tm_port_var_get*/
    NULL, /* jer2_arad_read_fld */ 
    NULL, /* jer2_arad_write_fld */ 
    NULL, /* jer2_arad_read_reg */ 
    NULL, /* jer2_arad_write_reg */ 
    NULL, /* jer2_arad_status_fld_poll */
    soc_jer2_brdc_fsrd_blk_id_set,
    soc_jer2_fsrd_blocks_low_power_init,
    soc_jer2_port_fsrd_block_enable_set,
    soc_jer2_fsrd_block_enable_set,
    NULL, /* jer2_arad_force_tdm_bypass_traffic_to_fabric_set*/
    NULL, /* jer2_arad_force_tdm_bypass_traffic_to_fabric_get*/
    NULL, /* jer2_jer_mgmt_enable_traffic_set*/
    NULL, /* jer2_jer_mgmt_enable_traffic_get*/
    NULL, /* jer2_arad_register_device*/
    NULL, /* jer2_arad_unregister_device*/
    NULL, /* jer2_arad_mgmt_tm_domain_set*/
    NULL, /* jer2_arad_mgmt_tm_domain_get*/
    NULL, /* jer2_jer_mgmt_dma_fifo_channel_free_find*/
    NULL, /* jer2_jer_mgmt_dma_fifo_channel_set*/
    NULL, /* jer2_jer_mgmt_dma_fifo_channel_get*/    
    NULL, /* jer2_arad_hpu_itmh_build_verify */ 
    NULL, /* jer2_arad_hpu_ftmh_build_verify */ 
    NULL, /* jer2_arad_hpu_otmh_build_verify */ 
    NULL, /* jer2_arad_hpu_itmh_build */ 
    NULL, /* jer2_arad_hpu_itmh_parse */ 
    NULL, /* jer2_arad_hpu_ftmh_build */ 
    NULL, /* jer2_arad_hpu_ftmh_parse */ 
    NULL, /* jer2_arad_hpu_otmh_build */ 
    NULL, /* jer2_arad_hpu_otmh_parse */ 
    NULL, /* soc_dnxc_port_loopback_set*/ 
    NULL, /* soc_dnxc_port_loopback_get*/ 
    NULL, /* jer2_nif_synce_clk_port_sel_set, */
    NULL, /* jer2_nif_synce_clk_port_sel_get, */
    NULL, /* jer2_nif_synce_clk_div_set, */
    NULL, /* jer2_nif_synce_clk_div_get, */
    NULL, /* jer2_nif_synce_clk_squelch_set, */
    NULL, /* jer2_nif_synce_clk_squelch_get, */
    NULL, /* jer2_jer_port_link_up_mac_update*/

    NULL, /* jer2_arad_itm_pfc_tc_map_set*/
    NULL, /* jer2_arad_itm_pfc_tc_map_get*/
    NULL, /* jer2_jer_fc_gen_cal_set*/
    NULL, /* jer2_jer_fc_gen_cal_get*/
    NULL, /* jer2_jer_fc_gen_inbnd_set*/
    NULL, /* jer2_jer_fc_gen_inbnd_get*/
    NULL, /*jer2_arad_fc_gen_inbnd_glb_hp_set*/
    NULL, /*jer2_arad_fc_gen_inbnd_glb_hp_get*/
    NULL, /* jer2_jer_fc_rec_cal_set*/
    NULL, /* jer2_jer_fc_rec_cal_get*/
    NULL, /* jer2_qax_fc_pfc_generic_bitmap_set*/
    NULL, /* jer2_qax_fc_pfc_generic_bitmap_get*/
    NULL, /* jer2_jer_fc_port_fifo_threshold_set*/
    NULL, /* jer2_jer_fc_port_fifo_threshold_get*/
    NULL, /* jer2_arad_egr_dsp_pp_to_base_q_pair_get*/
    NULL, /* jer2_arad_egr_dsp_pp_to_base_q_pair_set*/
    NULL, /* jer2_arad_egr_dsp_pp_priorities_mode_get*/
    NULL, /* jer2_arad_egr_dsp_pp_priorities_mode_set*/
    NULL, /* jer2_arad_egr_dsp_pp_shaper_mode_set_unsafe*/
    NULL, /* jer2_qax_egr_queuing_dev_set*/
    NULL, /* jer2_qax_egr_queuing_dev_get*/
    NULL, /* jer2_qax_egr_queuing_global_drop_set*/
    NULL, /* jer2_qax_egr_queuing_global_drop_get*/
    NULL, /* jer2_qax_egr_queuing_sp_tc_drop_set*/
    NULL, /* jer2_qax_egr_queuing_sp_tc_drop_get*/
    NULL, /* jer2_jer_egr_queuing_sch_unsch_drop_set*/
    NULL, /* soc_jer2_jer_egr_queuing_sch_unsch_drop_get_unsafe*/
    NULL, /* jer2_qax_egr_queuing_sp_reserved_set*/
    NULL, /* jer2_qax_egr_queuing_sp_reserved_get*/
    NULL, /* jer2_qax_egr_queuing_global_fc_set*/
    NULL, /* jer2_qax_egr_queuing_global_fc_get*/
    NULL, /* jer2_qax_egr_queuing_mc_tc_fc_set*/
    NULL, /* jer2_qax_egr_queuing_mc_tc_fc_get*/
    NULL, /* jer2_arad_egr_queuing_mc_cos_map_set */
    NULL, /* jer2_arad_egr_queuing_mc_cos_map_get */
    NULL, /* jer2_qax_egr_queuing_if_fc_set*/
    NULL, /* jer2_arad_egr_queuing_if_fc_get*/
    NULL, /* jer2_qax_egr_queuing_if_fc_uc_max_set*/
    NULL, /* jer2_qax_egr_queuing_if_fc_uc_set*/
    NULL, /* jer2_qax_egr_queuing_if_fc_uc_get*/
    NULL, /* jer2_qax_egr_queuing_if_fc_mc_set*/
    NULL, /* jer2_qax_egr_queuing_if_fc_mc_get*/
    NULL, /* jer2_arad_egr_queuing_if_uc_map_set*/
    NULL, /* jer2_arad_egr_queuing_if_mc_map_set*/
    NULL, /* jer2_arad_egr_queuing_ofp_tcg_set */
    NULL, /* was jer2_arad_egr_queuing_ofp_tcg_get */
    NULL, /* was jer2_arad_egr_queuing_tcg_weight_set */
    NULL, /* was jer2_arad_egr_queuing_tcg_weight_get */
    NULL, /* was jer2_arad_egr_queuing_is_high_priority_queue_get */
    NULL, /* soc_jer2_jer_egr_queuing_init_thresholds*/
    NULL, /* soc_jer2_jer_egr_interface_alloc*/
    NULL, /* soc_jer2_jer_egr_interface_free*/
    NULL, /* soc_jer2_jer_egr_nrdy_th_profile_data_set*/
    NULL, /* soc_jer2_jer_egr_nrdy_th_profile_data_get*/
    NULL, /* portmod_port_max_packet_size_set*/
    NULL, /* portmod_port_max_packet_size_get*/
    NULL, /* dnx_mult_cud_to_port_map_set*/
    NULL, /* dnx_mult_cud_to_port_map_get*/
    NULL, /* soc_jer2_qax_nof_interrupts*/
    NULL, /* jer2_arad_mgmt_nof_block_instances*/
    NULL, /* jer2_jer_mgmt_temp_pvt_get*/
    NULL, /* jer2_arad_ports_application_mapping_info_set*/
    NULL, /* jer2_arad_ports_application_mapping_info_get*/
    NULL, /* was jer2_arad_ofp_rates_max_credit_empty_port_set */
    NULL, /* was jer2_arad_ofp_rates_max_credit_empty_port_get */
    NULL, /*soc_jer2_arad_allocate_rcy_port*/
    NULL, /* soc_jer2_arad_free_tm_port_and_recycle_channel*/
    NULL, /* soc_jer2_arad_info_config_device_ports*/
    NULL, /* soc_jer2_arad_is_olp*/
    NULL, /* soc_jer2_arad_is_oamp*/
    NULL, /*soc_jer2_jer_fabric_mode_validate*/
    NULL, /*soc_jer2_arad_prop_fap_device_mode_get*/
    NULL, /*soc_jer2_arad_deinit*/
    NULL, /*soc_jer2_arad_attach*/
    NULL, /*soc_jer2_arad_fc_oob_mode_validate*/
    NULL, /* soc_jer2_qax_port_to_interface_egress_map_set*/
    NULL, /* was jer2_arad_ofp_rates_port_priority_max_burst_for_fc_queues_set */
    NULL, /* was jer2_arad_ofp_rates_port_priority_max_burst_for_empty_queues_set */
    NULL, /* was jer2_arad_ofp_rates_port_priority_max_burst_for_fc_queues_get */
    NULL, /* was jer2_arad_ofp_rates_port_priority_max_burst_for_empty_queues_get */
    NULL, /* was soc_jer2_jer_ofp_rates_egq_single_port_rate_sw_set */
    NULL, /* Was jer2_arad_ofp_rates_egq_single_port_rate_sw_get */
    NULL,
    NULL, /* was jer2_arad_ofp_rates_egq_single_port_rate_hw_get */
    NULL, /* was jer2_arad_ofp_rates_single_port_max_burst_set */
    NULL, /* was jer2_arad_ofp_rates_single_port_max_burst_get */
    NULL,
    NULL, /* was soc_jer2_jer_ofp_rates_egq_interface_shaper_get */
    NULL, /* was jer2_arad_ofp_rates_egq_tcg_rate_sw_set */
    NULL, /* was jer2_arad_ofp_rates_egq_tcg_rate_hw_set */
    NULL, /* was jer2_arad_ofp_rates_egq_tcg_rate_hw_get */

    NULL, /* was jer2_arad_ofp_rates_egq_tcg_max_burst_set */
    NULL, /* was jer2_arad_ofp_rates_egq_tcg_max_burst_get */

    NULL, /* was jer2_arad_ofp_rates_egq_port_priority_rate_sw_set */
    NULL, /* was jer2_arad_ofp_rates_egq_port_priority_rate_hw_set */
    NULL, /* was jer2_arad_ofp_rates_egq_port_priority_rate_hw_get */

    NULL, /* was jer2_arad_ofp_rates_egq_port_priority_max_burst_set */
    NULL, /* was jer2_arad_ofp_rates_egq_port_priority_max_burst_get */
    NULL, /* soc_jer2_jer_ofp_rates_port2chan_cal_get, */
    NULL, /* soc_jer2_jer_ofp_rates_retrieve_egress_shaper_reg_field_names*/
    NULL, /* soc_jer2_jer_ofp_rates_egress_shaper_mem_field_read*/
    NULL, /* soc_jer2_jer_ofp_rates_egress_shaper_mem_field_write*/
    NULL, /* soc_jer2_jer_ofp_rates_egq_scm_chan_arb_id2scm_id*/
    NULL, /* soc_jer2_jer_ofp_rates_interface_internal_rate_get*/
    NULL, /* jer2_arad_ofp_rates_packet_mode_packet_size_get*/
    NULL, /* was soc_jer2_jer_ofp_rates_egress_shaper_cal_write */
    NULL, /* was soc_jer2_jer_ofp_rates_egress_shaper_cal_read */
    NULL, /* Was soc_jer2_jer_egr_port2egress_offset. Replaced by dnx_egr_port2egress_offset*/
    NULL, /* soc_jer2_jer_egr_is_channelized*/
    NULL, /* dnx_port_sw_db_local_to_tm_port_get*/
    NULL, /* dnx_port_sw_db_local_to_pp_port_get*/
    NULL, /* dnx_port_sw_db_tm_to_local_port_get*/
    NULL, /* dnx_port_sw_db_pp_to_local_port_get*/
    NULL, /*soc_jer2_jer_portmod_init*/
    NULL, /*soc_jer2_jer_portmod_post_init*/
    NULL, /*soc_jer2_jer_portmod_deinit*/
    NULL, /*soc_jer2_jer_portmod_port_speed_set*/
    NULL, /*soc_jer2_jer_portmod_port_speed_get*/
    NULL, /*soc_jer2_jer_portmod_port_interface_set*/
    NULL, /*soc_jer2_jer_portmod_port_interface_get*/
    NULL, /*soc_jer2_jer_portmod_port_link_state_get*/
    NULL, /*soc_jer2_jer_portmod_is_supported_encap_get*/
    NULL, /* Was soc_jer2_jer_egr_q_nif_cal_set. Replaced by dnx_egr_q_nif_cal_set */
    NULL, /*soc_jer2_jer_egr_q_fast_port_set*/
    NULL, /* jer2_arad_parser_nof_bytes_to_remove_set*/
    NULL, /*jer2_arad_ps_db_find_free_binding_ps*/
    NULL, /*jer2_arad_ps_db_release_binding_ps*/
    NULL, /*jer2_arad_ps_db_alloc_binding_ps_with_id*/
    NULL, /*jer2_arad_egr_prog_editor_profile_set*/
    NULL, /* jer2_jer_fc_pfc_mapping_set*/
    NULL, /* jer2_jer_fc_pfc_mapping_get*/
    NULL, /* soc_jer2_jer_port_is_pcs_loopback*/
    NULL, /* soc_jer2_qax_qsgmii_offsets_add*/
    NULL, /* soc_jer2_qax_qsgmii_offsets_remove*/
    NULL, /* soc_jer2_jer_qsgmii_offsets_add_pbmp*/
    NULL, /* soc_jer2_jer_qsgmii_offsets_remove_pbmp*/
    NULL, /* soc_jer2_qax_port_sch_config*/
    NULL, /* soc_jer2_qax_port_open_fab_o_nif_path*/
    NULL, /* soc_jer2_qax_port_open_ilkn_path*/
    NULL, /* soc_jer2_qax_nif_sif_set*/
    NULL, /* soc_jer2_qax_port_close_ilkn_path*/
    NULL, /* soc_jer2_qax_port_ilkn_init*/
    NULL, /* soc_jer2_qax_nif_ilkn_pbmp_get*/
    NULL, /* soc_jer2_qax_nif_ilkn_phys_aligned_pbmp_get*/
    NULL, /* soc_jer2_qax_nif_qsgmii_pbmp_get*/
    NULL, /* soc_jer2_qax_port_ilkn_nif_port_get*/
    NULL, /* soc_jer2_qax_port_ilkn_bypass_interface_enable*/
    NULL, /* soc_jer2_qax_port_fabric_o_nif_bypass_interface_enable*/
    NULL, /* soc_jer2_qax_port_prd_enable_set*/
    NULL, /* soc_jer2_qax_port_prd_enable_get*/
    NULL, /* soc_jer2_qax_port_prd_config_set*/
    NULL, /* soc_jer2_qax_port_prd_config_get*/
    NULL, /* soc_jer2_qax_port_prd_threshold_set*/
    NULL, /* soc_jer2_qax_port_prd_threshold_get*/
    NULL, /* soc_jer2_qax_port_prd_map_set*/
    NULL, /* soc_jer2_qax_port_prd_map_get*/
    NULL, /* soc_jer2_qax_port_prd_drop_count_get*/
    NULL, /* soc_jer2_jer_port_prd_tpid_set*/
    NULL, /* soc_jer2_jer_port_prd_tpid_get*/
    NULL, /* soc_jer2_qax_port_speed_sku_restrictions*/
    NULL, /* mbcm_dnx_port_prd_ignore_ip_dscp_set */
    NULL, /* mbcm_dnx_port_prd_ignore_ip_dscp_get */
    NULL, /* mbcm_dnx_port_prd_ignore_mpls_exp_set */
    NULL, /* mbcm_dnx_port_prd_ignore_mpls_exp_get */
    NULL, /* mbcm_dnx_port_prd_ignore_inner_tag_set */
    NULL, /* mbcm_dnx_port_prd_ignore_inner_tag_get */
    NULL, /* mbcm_dnx_port_prd_ignore_outer_tag_set */
    NULL, /* mbcm_dnx_port_prd_ignore_outer_tag_get */
    NULL, /* mbcm_dnx_port_prd_default_priority_set */
    NULL, /* mbcm_dnx_port_prd_default_priority_get */
    NULL, /* mbcm_dnx_port_prd_custom_ether_type_set */
    NULL, /* mbcm_dnx_port_prd_custom_ether_type_get */
    NULL, /* mbcm_dnx_port_prd_control_frame_set */
    NULL, /* mbcm_dnx_port_prd_control_frame_get */
    NULL, /* mbcm_dnx_port_prd_flex_key_construct_set */
    NULL, /* mbcm_dnx_port_prd_flex_key_construct_get */
    NULL, /* mbcm_dnx_port_prd_flex_key_entry_set */
    NULL, /* mbcm_dnx_port_prd_flex_key_entry_get */
    NULL, /* soc_jer2_jer_port_prd_restore_hw_defaults*/
    NULL, /* soc_dnxc_port_control_pcs_set*/
    NULL, /* soc_dnxc_port_control_pcs_get*/
    NULL, /* soc_dnxc_port_control_power_set*/
    NULL, /* soc_dnxc_port_control_power_get*/
    NULL, /* soc_dnxc_port_control_rx_enable_set*/
    NULL, /* soc_dnxc_port_control_tx_enable_set*/
    NULL, /* soc_jer2_jer_port_control_tx_nif_enable_set*/
    NULL, /* soc_dnxc_port_control_rx_enable_get*/
    NULL, /* soc_dnxc_port_control_tx_enable_get*/
    NULL, /* soc_jer2_jer_port_control_tx_nif_enable_get*/
    NULL, /* mbcm_dnx_port_control_strip_crc_set */
    NULL, /* mbcm_dnx_port_control_strip_crc_get */
    NULL, /* soc_dnxc_port_prbs_tx_enable_set*/    
    NULL, /* soc_dnxc_port_prbs_tx_enable_get*/       
    NULL, /* soc_dnxc_port_prbs_rx_enable_set*/     
    NULL, /* soc_dnxc_port_prbs_rx_enable_get*/     
    NULL, /* soc_dnxc_port_prbs_rx_status_get*/    
    NULL, /* soc_dnxc_port_prbs_polynomial_set*/     
    NULL, /* soc_dnxc_port_prbs_polynomial_get*/     
    NULL, /* soc_dnxc_port_prbs_tx_invert_data_set*/  
    NULL, /* soc_dnxc_port_prbs_tx_invert_data_get*/ 
    NULL, /*soc_jer2_jer_portmod_pfc_refresh_set*/
    NULL, /*soc_jer2_jer_portmod_pfc_refresh_get*/
    NULL, /* portmod_port_local_fault_status_clear*/
    NULL, /* portmod_port_remote_fault_status_clear*/
    NULL, /* portmod_port_pad_size_set*/
    NULL, /* portmod_port_pad_size_get*/
    NULL, /* soc_jer2_jer_port_phy_reset*/
    NULL, /* soc_dnxc_port_phy_control_set*/
    NULL, /* soc_dnxc_port_phy_control_get*/
    soc_dnxc_port_phy_reg_get,
    soc_dnxc_port_phy_reg_set,
    soc_dnxc_port_phy_reg_modify,
    NULL, /* soc_jer2_jer_port_mac_sa_set*/
    NULL, /* soc_jer2_jer_port_mac_sa_get*/
    NULL, /* soc_jer2_jer_port_eee_enable_get*/
    NULL, /* soc_jer2_jer_port_eee_enable_set*/
    NULL, /* soc_jer2_jer_port_eee_tx_idle_time_get*/
    NULL, /* soc_jer2_jer_port_eee_tx_idle_time_set*/
    NULL, /* soc_jer2_jer_port_eee_tx_wake_time_get*/
    NULL, /* soc_jer2_jer_port_eee_tx_wake_time_set*/
    NULL, /* mbcm_dnx_port_eee_link_active_duration_get */
    NULL, /* mbcm_dnx_port_eee_link_active_duration_set */
    NULL, /* mbcm_dnx_port_eee_statistics_clear */
    NULL, /* mbcm_dnx_port_eee_event_count_symmetric_set */
    NULL, /* mbcm_dnx_port_eee_tx_event_count_get */
    NULL, /* mbcm_dnx_port_eee_tx_duration_get */
    NULL, /* mbcm_dnx_port_eee_rx_event_count_get */
    NULL, /* mbcm_dnx_port_eee_rx_duration_get */
    NULL, /* mbcm_dnx_port_eee_event_count_symmetric_get */
    NULL, /* jer2_jer_fc_enables_set*/
    NULL, /* jer2_jer_fc_enables_get*/
    NULL, /* jer2_jer_fc_ilkn_mub_channel_set*/
    NULL, /* jer2_jer_fc_ilkn_mub_channel_get*/
    NULL, /* jer2_jer_fc_ilkn_mub_gen_cal_set*/
    NULL, /* jer2_jer_fc_ilkn_mub_gen_cal_get*/
    NULL, /* jer2_jer_fc_cat_2_tc_hcfc_bitmap_set*/
    NULL, /* jer2_jer_fc_cat_2_tc_hcfc_bitmap_get*/
    NULL, /* jer2_jer_fc_glb_hcfc_bitmap_set*/
    NULL, /* jer2_jer_fc_glb_hcfc_bitmap_get*/
    NULL, /* jer2_jer_fc_inbnd_mode_set*/
    NULL, /* jer2_jer_fc_inbnd_mode_get*/
    NULL, /* jer2_qax_fc_glb_rcs_mask_set*/
    NULL, /* jer2_qax_fc_glb_rcs_mask_get*/
    NULL, /* jer2_jer_fc_init_pfc_mapping*/
    NULL, /* jer2_jer_fc_ilkn_llfc_set*/
    NULL, /* jer2_jer_fc_ilkn_llfc_get*/
    NULL, /* portmod_port_tx_average_ipg_set*/
    NULL, /* portmod_port_tx_average_ipg_get*/
    NULL, /* portmod_port_link_get*/
    NULL, /* soc_jer2_jer_portmod_autoneg_set*/
    NULL, /* soc_jer2_jer_portmod_autoneg_get*/
    NULL, /* portmod_port_ability_local_get*/
    NULL, /* soc_jer2_jer_port_ability_remote_get*/
    NULL, /* soc_jer2_jer_port_ability_advert_set*/
    NULL, /* portmod_port_ability_advert_get*/
    NULL, /* mbcm_dnx_port_mdix_set */
    NULL, /* mbcm_dnx_port_mdix_get */
    NULL, /* mbcm_dnx_port_mdix_status_get */
    NULL, /* soc_jer2_jer_port_duplex_set*/
    NULL, /* portmod_port_duplex_get*/
    NULL, /* soc_jer2_jer_port_nif_nof_lanes_get*/
    NULL, /* soc_jer2_jer_port_nif_quad_to_core_validate*/
    NULL, /* soc_jer2_jer_port_close_path*/
    NULL, /* jer2_arad_fc_hcfc_watchdog_set*/
    NULL, /* jer2_arad_fc_hcfc_watchdog_get*/
    NULL, /* soc_jer2_jer_port_fault_get*/
    NULL, /* jer2_arad_link_port_fault_get*/
    NULL, /* soc_jer2_jer_stat_if_queue_range_set*/
    NULL, /* soc_jer2_jer_stat_if_queue_range_get*/
    NULL, /* jer2_arad_ports_logical_sys_id_parse*/
    NULL, /* jer2_jer_fc_vsq_index_group2global*/
    NULL, /* portmod_port_cntmaxsize_set*/
    NULL, /* portmod_port_cntmaxsize_get*/
    NULL, /* jer2_jer_fc_status_info_get*/
    NULL, /* soc_jer2_jer_stat_counter_filter_set*/
    NULL, /* soc_jer2_jer_stat_counter_filter_get*/
    NULL, /* mbcm_dnx_cnt_ingress_compensation_profile_delta_set */
    NULL, /* mbcm_dnx_cnt_ingress_compensation_port_profile_set */
    NULL, /* mbcm_dnx_cnt_ingress_compensation_outLif_delta_set */
    NULL, /* mbcm_dnx_cnt_ingress_compensation_outLif_delta_get */
    NULL, /* mbcm_dnx_cnt_ingress_compensation_port_delta_and_profile_get */
    NULL, /* jer2_qax_cnt_filter_config_ingress_set_get*/
    NULL, /* jer2_qax_filter_config_egress_receive_set_get*/
    NULL, /* jer2_qax_cnt_crps_cgm_cmd_get*/
    NULL, /* _jer2_qax_cnt_counter_bmap_mem_by_src_type_get*/
    NULL, /* jer2_qax_cnt_do_not_count_field_by_src_type_get*/
    NULL, /* jer2_arad_cnt_stif_ingress_pp_source_set*/
    NULL, /* jer2_arad_fc_pfc_generic_bitmap_valid_update*/
    NULL, /* jer2_arad_fc_pfc_generic_bitmap_used_update*/
    NULL, /* jer2_qax_interrupts_array_init*/
    NULL, /* jer2_qax_interrupts_array_deinit*/
    NULL, /* jer2_qax_interrupt_cb_init*/
#ifdef PORTMOD_SUPPORT
    NULL, /* soc_jer2_qax_pm_instances_get*/
#endif
    NULL, /* soc_jer2_qax_pml_table_get*/
    NULL, /* soc_jer2_jer_portmod_port_quad_get*/
    NULL, /* soc_jer2_jer_port_ports_to_same_quad_get*/
    NULL, /* soc_jer2_jer_nif_priority_set*/
    NULL, /* soc_jer2_jer_nif_priority_get*/
    NULL, /* soc_jer2_qax_nif_sku_restrictions*/
    NULL, /* soc_jer2_qax_fabric_link_config_ovrd*/
    NULL, /* soc_jer2_qax_wait_gtimer_trigger*/
    NULL, /* soc_jer2_jer_phy_nif_measure*/
    NULL, /* soc_bist_all_jer2_qax*/
    NULL, /* soc_jer2_jer_device_reset*/
    NULL, /* jer2_jer_fc_cmic_rx_set*/
    NULL, /* jer2_jer_fc_cmic_rx_get*/
    NULL, /* jer2_qax_mult_get_entry*/
    NULL, /* mbcm_dnx_port_ilkn_over_fabric_set */
    NULL /* jer2_arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_rate*/
};
