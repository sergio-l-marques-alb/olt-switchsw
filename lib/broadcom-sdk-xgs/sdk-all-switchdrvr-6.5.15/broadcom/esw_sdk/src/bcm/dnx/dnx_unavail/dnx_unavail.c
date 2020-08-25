/** \file dnx_unavail.c
 * $Id$
 *
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_VLAN
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/swstate/auto_generated/access/vlan_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm/init.h>
#include <bcm/qos.h>
#include <bcm/stg.h>
#include <bcm/ipsec.h>
#include <bcm/knet.h>
#include <bcm/lb.h>
#include <bcm/failover.h>
#include <bcm/mirror.h>
#include <bcm/vswitch.h>
#if defined(INCLUDE_I2C)
#include <bcm/bcmi2c.h>
#endif
#include <bcm/nat.h>
#if defined(INCLUDE_PTP)
#include <bcm/ptp.h>
#endif
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *   APIs for Jericho2
 */

int
bcm_dnx_clear(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_init_check(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_init_selective(
    int unit,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}

/**
 * \brief 
 * Init for qos module. 
 * tables 
 */
shr_error_e
bcm_dnx_qos_init(
    int unit)
{
    return _SHR_E_UNAVAIL;
}

int
bcm_dnx_policer_envelop_create(
    int unit,
    uint32 flag,
    bcm_policer_t macro_flow_policer_id,
    bcm_policer_t * policer_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_qos_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_qos_port_vlan_map_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int *ing_map,
    int *egr_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_qos_port_vlan_map_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int ing_map,
    int egr_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_modport_remote_map_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    int *remote_modid,
    bcm_port_t * remote_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_modport_remote_map_set(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    int remote_modid,
    bcm_port_t remote_port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_create(
    int unit,
    bcm_stk_system_gport_map_t * sys_gport_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_destroy(
    int unit,
    bcm_gport_t system_gport)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_stk_system_gport_map_get(
    int unit,
    bcm_stk_system_gport_map_t * sys_gport_map)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_stable_register(
    int unit,
    bcm_switch_read_func_t rf,
    bcm_switch_write_func_t wf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_dram_vendor_info_get(
    int unit,
    bcm_switch_dram_vendor_info_t * info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_create(
    int unit,
    bcm_switch_encap_info_t * encap_info,
    bcm_if_t * encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_destroy(
    int unit,
    bcm_if_t encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_switch_encap_info_t * encap_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_set(
    int unit,
    bcm_if_t encap_id,
    bcm_switch_encap_info_t * encap_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_encap_traverse(
    int unit,
    bcm_switch_encap_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_ipv6_reserved_multicast_addr_get(
    int unit,
    bcm_ip6_t * ip6_addr,
    bcm_ip6_t * ip6_mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_ipv6_reserved_multicast_addr_set(
    int unit,
    bcm_ip6_t ip6_addr,
    bcm_ip6_t ip6_mask)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_lif_property_get(
    int unit,
    bcm_switch_lif_property_t lif_property,
    bcm_switch_lif_property_config_t * lif_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_lif_property_set(
    int unit,
    bcm_switch_lif_property_t lif_property,
    bcm_switch_lif_property_config_t * lif_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_add(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_info_t * config_info,
    int *match_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_delete(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_delete_all(
    int unit,
    bcm_switch_match_service_t match_service)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_get(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t * config_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_set(
    int unit,
    bcm_switch_match_service_t match_service,
    int match_id,
    bcm_switch_match_config_info_t * config_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_config_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_config_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_get(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t * control_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_set(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_type_t control_type,
    bcm_gport_t gport,
    bcm_switch_match_control_info_t * control_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_match_control_traverse(
    int unit,
    bcm_switch_match_service_t match_service,
    bcm_switch_match_control_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_object_count_get(
    int unit,
    bcm_switch_object_t object,
    int *entries)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_object_count_multi_get(
    int unit,
    int object_size,
    bcm_switch_object_t * object_array,
    int *entries)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_add(
    int unit,
    uint32 options,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_delete(
    int unit,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_get(
    int unit,
    bcm_switch_olp_l2_addr_t * l2_addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_olp_l2_addr_traverse(
    int unit,
    bcm_switch_olp_l2_addr_traverse_cb cb_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_service_get(
    int unit,
    bcm_switch_service_t service,
    bcm_switch_service_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_service_set(
    int unit,
    bcm_switch_service_t service,
    bcm_switch_service_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_tpid_class_get(
    int unit,
    bcm_switch_tpid_class_t * tpid_class)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_tpid_get(
    int unit,
    bcm_switch_tpid_info_t * tpid_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_user_buffer_read(
    int unit,
    uint32 flags,
    bcm_switch_user_buffer_type_t buff_type,
    uint8 *buf,
    int offset,
    int nbytes)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_user_buffer_write(
    int unit,
    uint32 flags,
    bcm_switch_user_buffer_type_t buff_type,
    uint8 *buf,
    int offset,
    int nbytes)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_bitmap_expand(
    int unit,
    bcm_pbmp_t * pbmp_ptr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_failover_get(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t failport,
    int *psc,
    uint32 *flags,
    int array_size,
    bcm_gport_t * fail_to_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_failover_set(
    int unit,
    bcm_trunk_t tid,
    bcm_gport_t failport,
    int psc,
    uint32 flags,
    int count,
    bcm_gport_t * fail_to_array)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_mcast_join(
    int unit,
    bcm_trunk_t tid,
    bcm_vlan_t vid,
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_resilient_replace(
    int unit,
    uint32 flags,
    bcm_trunk_resilient_entry_t * match_entry,
    int *num_entries,
    bcm_trunk_resilient_entry_t * replace_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_trunk_resilient_traverse(
    int unit,
    uint32 flags,
    bcm_trunk_resilient_entry_t * match_entry,
    bcm_trunk_resilient_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_create(
    int unit,
    uint32 flags,
    int *exp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_destroy(
    int unit,
    int exp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_get(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_exp_map_set(
    int unit,
    int exp_map_id,
    bcm_mpls_exp_map_t * exp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_label_stat_attach(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port,
    uint32 stat_counter_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_label_stat_detach(
    int unit,
    bcm_mpls_label_t label,
    bcm_gport_t port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_tunnel_initiator_set(
    int unit,
    bcm_if_t intf,
    int num_labels,
    bcm_mpls_egress_label_t * label_array)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mpls_tunnel_switch_add(
    int unit,
    bcm_mpls_tunnel_switch_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_sha1_get(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t * sha1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_sha1_set(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t * sha1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_get(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_set(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_endpoint_poll(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_endpoint_stat_get(
    int unit,
    bcm_bfd_endpoint_t endpoint,
    bcm_bfd_endpoint_stat_t * ctr_info,
    uint32 options)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_tx_start(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_tx_stop(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_vlan_control_get(
    int unit,
    bcm_vlan_control_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_vlan_control_set(
    int unit,
    bcm_vlan_control_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t * vlan_ip,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_ip_action_delete(
    int unit,
    bcm_vlan_ip_t * vlan_ip)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_ip_action_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t * vlan_ip,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_ip_action_traverse(
    int unit,
    bcm_vlan_ip_action_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_list_by_pbmp(
    int unit,
    bcm_pbmp_t ports,
    bcm_vlan_data_t ** listp,
    int *countp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mac_action_delete(
    int unit,
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mac_action_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mac_action_traverse(
    int unit,
    bcm_vlan_mac_action_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mcast_flood_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_mcast_flood_t * mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_mcast_flood_set(
    int unit,
    bcm_vlan_t vlan,
    bcm_vlan_mcast_flood_t mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_default_action_delete(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_egress_default_action_delete(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_protocol_action_delete(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_protocol_action_delete_all(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_protocol_action_traverse(
    int unit,
    bcm_vlan_port_protocol_action_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_delete(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_range_traverse(
    int unit,
    bcm_vlan_translate_action_range_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_action_traverse(
    int unit,
    bcm_vlan_translate_action_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_egress_action_delete(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_vlan_action_set_t * action)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_translate_egress_action_traverse(
    int unit,
    bcm_vlan_translate_egress_action_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_vector_flags_set(
    int unit,
    bcm_vlan_vector_t vlan_vector,
    uint32 flags_mask,
    uint32 flags_value)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_enable(
    int unit,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_add(
    int unit,
    int *range_id,
    bcm_ipmc_range_t * range)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_delete(
    int unit,
    int range_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_get(
    int unit,
    int range_id,
    bcm_ipmc_range_t * range)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_range_size_get(
    int unit,
    int *size)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_add(
    int unit,
    int rp_id,
    bcm_if_t intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_create(
    int unit,
    uint32 flags,
    int *rp_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_delete(
    int unit,
    int rp_id,
    bcm_if_t intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_delete_all(
    int unit,
    int rp_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_destroy(
    int unit,
    int rp_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_get(
    int unit,
    int rp_id,
    int intf_max,
    bcm_if_t * intf_array,
    int *intf_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_rp_set(
    int unit,
    int rp_id,
    int intf_count,
    bcm_if_t * intf_array)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_attach(
    int unit,
    bcm_ipmc_addr_t * info,
    uint32 stat_counter_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_set(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_counter_sync_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 num_entries,
    uint32 *counter_indexes,
    bcm_stat_value_t * counter_values)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_detach(
    int unit,
    bcm_ipmc_addr_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_id_get(
    int unit,
    bcm_ipmc_addr_t * info,
    bcm_ipmc_stat_t stat,
    uint32 *stat_counter_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_get(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint64 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_get32(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint32 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_set(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint64 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_ipmc_stat_multi_set32(
    int unit,
    bcm_ipmc_addr_t * info,
    int nstat,
    bcm_ipmc_stat_t * stat_arr,
    uint32 *value_arr)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l2_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_addr_freeze(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_addr_multi_add(
    int unit,
    bcm_l2_addr_t * l2addr,
    int count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_addr_multi_delete(
    int unit,
    bcm_l2_addr_t * l2addr,
    int count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_age_timer_set(
    int unit,
    int age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL,
                 "bcm_l2_age_timer_set is no longer supported. Use bcm_l2_age_timer_meta_cycle_set instead");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_age_timer_get(
    int unit,
    int *age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(BCM_E_UNAVAIL,
                 "bcm_l2_age_timer_get is no longer supported. Use bcm_l2_age_timer_meta_cycle_get instead");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_addr_thaw(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_auth_add(
    int unit,
    bcm_l2_auth_addr_t * addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_auth_delete(
    int unit,
    bcm_l2_auth_addr_t * addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_auth_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_auth_get(
    int unit,
    bcm_l2_auth_addr_t * addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_auth_traverse(
    int unit,
    bcm_l2_auth_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_delete(
    int unit,
    int index)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_get(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_profile_get(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_profile_set(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_set(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr,
    int *index_used)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_size_get(
    int unit,
    int *size)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_get(
    int unit,
    uint32 vsi,
    uint32 *profile_idx)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_set(
    int unit,
    uint32 vsi,
    uint32 profile_idx)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_learn_limit_disable(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_learn_limit_enable(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_tunnel_add(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_tunnel_delete(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_tunnel_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_l2gre_port_t * l2gre_port)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_port_delete(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_gport_t l2gre_port_id)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_port_delete_all(
    int unit,
    bcm_vpn_t l2vpn)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_l2gre_port_t * l2gre_port)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_l2gre_port_t * port_array,
    int *port_count)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_l2gre_vpn_create(
    int unit,
    bcm_l2gre_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

int
bcm_dnx_l2gre_vpn_destroy(
    int unit,
    bcm_vpn_t l2vpn)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2gre_vpn_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2gre_vpn_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_l2gre_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2gre_vpn_traverse(
    int unit,
    bcm_l2gre_vpn_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rate_bandwidth_get(
    int unit,
    bcm_port_t port,
    int flags,
    uint32 *kbits_sec,
    uint32 *kbits_burst)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rate_bandwidth_set(
    int unit,
    bcm_port_t port,
    int flags,
    uint32 kbits_sec,
    uint32 kbits_burst)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vxlan_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_delete_all(
    int unit,
    bcm_vpn_t l2vpn)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_vxlan_port_t * port_array,
    int *port_count)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_port_traverse(
    int unit,
    bcm_vxlan_port_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_vpn_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_vxlan_vpn_traverse(
    int unit,
    bcm_vxlan_vpn_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

int
bcm_dnx_port_cable_diag(
    int unit,
    bcm_port_t port,
    bcm_port_cable_diag_t * status)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_cfi_color_get(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t * color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_cfi_color_set(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_congestion_config_get(
    int unit,
    bcm_gport_t port,
    bcm_port_congestion_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_congestion_config_set(
    int unit,
    bcm_gport_t port,
    bcm_port_congestion_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_discard_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_discard_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dscp_map_get(
    int unit,
    bcm_port_t port,
    int srccp,
    int *mapcp,
    int *prio)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dscp_map_mode_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dscp_map_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dscp_map_set(
    int unit,
    bcm_port_t port,
    int srccp,
    int mapcp,
    int prio)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dtag_mode_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_dtag_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_egress_get(
    int unit,
    bcm_port_t port,
    int modid,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_egress_set(
    int unit,
    bcm_port_t port,
    int modid,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_config_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_encap_config_t * encap_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_config_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_encap_config_t * encap_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_local_get(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    int *encap_local_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_map_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_gport_t * port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_map_set(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_gport_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_encap_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_force_dest_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_dest_info_t * dest_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_force_dest_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_dest_info_t * dest_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_ingress_tdm_failover_get(
    int unit,
    int flag,
    bcm_pbmp_t * tdm_enable_pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_ingress_tdm_failover_set(
    int unit,
    int flag,
    bcm_pbmp_t tdm_enable_pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_inner_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_inner_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_interface_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_interface_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_l3_encapsulated_mtu_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_l3_encapsulated_mtu_set(
    int unit,
    bcm_port_t port,
    int size)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_l3_mtu_get(
    int unit,
    bcm_port_t port,
    int *size)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_l3_mtu_set(
    int unit,
    bcm_port_t port,
    int size)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_modid_egress_get(
    int unit,
    bcm_port_t port,
    bcm_module_t modid,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_modid_egress_set(
    int unit,
    bcm_port_t port,
    bcm_module_t modid,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_modid_enable_get(
    int unit,
    bcm_port_t port,
    int modid,
    int *enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_modid_enable_set(
    int unit,
    bcm_port_t port,
    int modid,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pause_get(
    int unit,
    bcm_port_t port,
    int *pause_tx,
    int *pause_rx)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pause_set(
    int unit,
    bcm_port_t port,
    int pause_tx,
    int pause_rx)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pause_sym_get(
    int unit,
    bcm_port_t port,
    int *pause)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pause_sym_set(
    int unit,
    bcm_port_t port,
    int pause)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pon_tunnel_add(
    int unit,
    bcm_gport_t pon_port,
    uint32 flags,
    bcm_gport_t * tunnel_port_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pon_tunnel_map_get(
    int unit,
    bcm_gport_t pon_port,
    bcm_tunnel_id_t tunnel_id,
    bcm_gport_t * tunnel_port_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pon_tunnel_map_set(
    int unit,
    bcm_gport_t pon_port,
    bcm_tunnel_id_t tunnel_id,
    bcm_gport_t tunnel_port_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_pon_tunnel_remove(
    int unit,
    bcm_gport_t tunnel_port_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_priority_color_get(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t * color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_priority_color_set(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_stat_enable_set(
    int unit,
    bcm_gport_t port,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_stat_multi_set(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint64 *value_arr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_stat_multi_set32(
    int unit,
    bcm_gport_t port,
    int nstat,
    bcm_port_stat_t * stat_arr,
    uint32 *value_arr)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_stat_set(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint64 val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_stat_set32(
    int unit,
    bcm_gport_t port,
    bcm_port_stat_t stat,
    uint32 val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_tpid_add(
    int unit,
    bcm_port_t port,
    uint16 tpid,
    int color_select)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_tpid_delete(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_tpid_delete_all(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_untagged_priority_get(
    int unit,
    bcm_port_t port,
    int *priority)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_untagged_priority_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_vlan_priority_map_get(
    int unit,
    bcm_port_t port,
    int pkt_pri,
    int cfi,
    int *internal_pri,
    bcm_color_t * color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_vlan_priority_map_set(
    int unit,
    bcm_port_t port,
    int pkt_pri,
    int cfi,
    int internal_pri,
    bcm_color_t color)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_vlan_vector_get(
    int unit,
    bcm_gport_t port_id,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_port_vlan_vector_set(
    int unit,
    bcm_gport_t port_id,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t * ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_g781_option_get(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t * g781_option)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_QL_SSM_map(
    int unit,
    bcm_esmc_network_option_t opt,
    bcm_esmc_quality_level_t ql,
    uint8 *ssm_code)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_SSM_QL_map(
    int unit,
    bcm_esmc_network_option_t opt,
    uint8 ssm_code,
    bcm_esmc_quality_level_t * ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_g781_option_set(
    int unit,
    int stack_id,
    bcm_esmc_network_option_t g781_option)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_rx_callback_register(
    int unit,
    int stack_id,
    bcm_esmc_rx_cb rx_cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_rx_callback_unregister(
    int unit,
    int stack_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_tunnel_get(
    int unit,
    int stack_id,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_tunnel_set(
    int unit,
    int stack_id,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_esmc_tx(
    int unit,
    int stack_id,
    bcm_pbmp_t pbmp,
    bcm_esmc_pdu_data_t * esmc_pdu_data)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_extender_forward_add(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_get(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_I2C)
int
bcm_dnx_i2c_ioctl(
    int unit,
    int fd,
    int opcode,
    void *data,
    int len)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_I2C)
int
bcm_dnx_i2c_open(
    int unit,
    char *devname,
    uint32 flags,
    int speed)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_I2C)
int
bcm_dnx_i2c_read(
    int unit,
    int fd,
    uint32 addr,
    uint8 *data,
    uint32 *nbytes)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_I2C)
int
bcm_dnx_i2c_write(
    int unit,
    int fd,
    uint32 addr,
    uint8 *data,
    uint32 nbytes)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_ipsec_config_set(
    int unit,
    bcm_ipsec_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_sa_create(
    int unit,
    uint32 flags,
    uint32 *sa_id,
    bcm_ipsec_sa_info_t * sa)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_sa_delete(
    int unit,
    uint32 flags,
    uint32 sa_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_sa_key_update(
    int unit,
    uint32 flags,
    uint32 sa_id,
    bcm_ipsec_sa_keys_t * sa_keys)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_sa_to_tunnel_map_get(
    int unit,
    uint32 sa_id,
    uint32 *tunnel_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_sa_to_tunnel_map_set(
    int unit,
    uint32 tunnel_id,
    uint32 sa_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_tunnel_create(
    int unit,
    uint32 flags,
    uint32 *tunnel_id,
    bcm_ipsec_tunnel_info_t * tunnel_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ipsec_tunnel_delete(
    int unit,
    uint32 flags,
    uint32 tunnel_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    int *num_entries,
    bcm_l3_egress_ecmp_resilient_entry_t * replace_entry)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_ecmp_resilient_traverse(
    int unit,
    uint32 flags,
    bcm_l3_egress_ecmp_resilient_entry_t * match_entry,
    bcm_l3_egress_ecmp_resilient_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_egress_find(
    int unit,
    bcm_l3_egress_t * egr,
    bcm_if_t * intf)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l3_egress_multi_alloc(
    int unit,
    bcm_l3_egress_multi_info_t egress_multi_info,
    bcm_if_t * base_egress_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multi_free(
    int unit,
    bcm_if_t base_egress_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_add(
    int unit,
    bcm_if_t mpintf,
    bcm_if_t intf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_create(
    int unit,
    uint32 flags,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_delete(
    int unit,
    bcm_if_t mpintf,
    bcm_if_t intf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_destroy(
    int unit,
    bcm_if_t mpintf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_find(
    int unit,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_get(
    int unit,
    bcm_if_t mpintf,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_max_create(
    int unit,
    uint32 flags,
    int max_paths,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_egress_multipath_traverse(
    int unit,
    bcm_l3_egress_multipath_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_enable_set(
    int unit,
    int enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_host_age(
    int unit,
    uint32 flags,
    bcm_l3_host_traverse_cb age_cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_host_conflict_get(
    int unit,
    bcm_l3_key_t * ipkey,
    bcm_l3_key_t * cf_array,
    int cf_max,
    int *cf_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_sequence_number_width_set(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    int number_width)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_stat_get(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_stat_val_t type,
    uint64 *value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_status_get(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_status_type_t status_type,
    int *value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_tc_dp_to_priority_config_get(
    int unit,
    uint32 flags,
    bcm_lb_tc_dp_t * tc_dp,
    bcm_lb_pkt_pri_t * pkt_pri)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_tc_dp_to_priority_config_set(
    int unit,
    uint32 flags,
    bcm_lb_tc_dp_t * tc_dp,
    bcm_lb_pkt_pri_t * pkt_pri)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_tx_sched_get(
    int unit,
    uint32 flags,
    int max_lbg_count,
    bcm_lb_lbg_weight_t * lbg_weights,
    int *lbg_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_tx_sched_set(
    int unit,
    uint32 flags,
    int lbg_count,
    bcm_lb_lbg_weight_t * lbg_weights)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_flow_traverse(
    int unit,
    bcm_port_t port,
    bcm_macsec_flow_traverse_cb callbk,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_port_capability_get(
    int unit,
    bcm_port_t port,
    bcm_macsec_port_capability_t * cap)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_port_config_get(
    int unit,
    bcm_port_t port,
    bcm_macsec_port_config_t * cfg)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_port_config_set(
    int unit,
    bcm_port_t port,
    bcm_macsec_port_config_t * cfg)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_port_traverse(
    int unit,
    bcm_macsec_port_traverse_cb callbk,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_assoc_create(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int chanId,
    bcm_macsec_secure_assoc_t * assoc,
    int *assocId)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_assoc_destroy(
    int unit,
    bcm_port_t port,
    int assocId)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_assoc_get(
    int unit,
    bcm_port_t port,
    int assocId,
    bcm_macsec_secure_assoc_t * assoc,
    int *chanId)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_assoc_traverse(
    int unit,
    bcm_port_t port,
    int chanId,
    bcm_macsec_secure_assoc_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_chan_create(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_macsec_secure_chan_t * chan,
    int *chanId)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_chan_destroy(
    int unit,
    bcm_port_t port,
    int chanId)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_chan_get(
    int unit,
    bcm_port_t port,
    int chanId,
    bcm_macsec_secure_chan_t * chan)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_secure_chan_traverse(
    int unit,
    bcm_port_t port,
    bcm_macsec_chan_traverse_cb calback,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_stat_clear(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_stat_get(
    int unit,
    bcm_port_t port,
    bcm_macsec_stat_t stat,
    int chanId,
    int assocId,
    uint64 *val)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_stat_get32(
    int unit,
    bcm_port_t port,
    bcm_macsec_stat_t stat,
    int chanId,
    int assocId,
    uint32 *val)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_stat_set(
    int unit,
    bcm_port_t port,
    bcm_macsec_stat_t stat,
    int chanId,
    int assocId,
    uint64 val)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_MACSEC)
int
bcm_dnx_macsec_stat_set32(
    int unit,
    bcm_port_t port,
    bcm_macsec_stat_t stat,
    int chanId,
    int assocId,
    uint32 val)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_add(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_delete(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_gport_t mim_port_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_port_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_port_t * mim_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_create(
    int unit,
    bcm_mim_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_destroy(
    int unit,
    bcm_mim_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_mim_vpn_get(
    int unit,
    bcm_mim_vpn_t vpn,
    bcm_mim_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_pstats_data_sync(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_create(
    int unit,
    int options,
    int array_count,
    bcm_pstats_session_element_t * element_array,
    bcm_pstats_session_id_t * session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_clear(
    int unit,
    bcm_pstats_session_id_t session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    bcm_pstats_timestamp_t * timestamp,
    int array_max,
    bcm_pstats_data_t * data_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_data_sync_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    bcm_pstats_timestamp_t * timestamp,
    int array_max,
    bcm_pstats_data_t * data_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_destroy(
    int unit,
    bcm_pstats_session_id_t session_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_get(
    int unit,
    bcm_pstats_session_id_t session_id,
    int array_max,
    bcm_pstats_session_element_t * element_array,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PSTATS)
int
bcm_dnx_pstats_session_traverse(
    int unit,
    bcm_pstats_session_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int priority1_alt_value,
    bcm_ptp_clock_peer_address_t * master_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_enabled_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 *enabled)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_enabled_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint8 enabled)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t * master_addr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_acceptable_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_cb_register(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_cb_unregister(
    int unit,
    bcm_ptp_cb_types_t cb_types,
    bcm_ptp_cb cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_accuracy_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t * accuracy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_accuracy_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_accuracy_t * accuracy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_create(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_clock_info_t * clock_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_current_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_current_dataset_t * dataset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_default_dataset_t * dataset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *domain)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 domain)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_info_t * clock_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_local_priority_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *local_priority)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_local_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 local_priority)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_max_steps_removed_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *max_steps_removed)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_max_steps_removed_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 max_steps_removed)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_parent_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_parent_dataset_t * dataset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_peer_age_timer_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *aging_time)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_peer_age_timer_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 aging_time)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_pkt_flags_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 override_flag,
    uint32 override_value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_ptp_pkt_flags_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *override_flag,
    uint32 *override_value)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_announce_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *timeout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_announce_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 timeout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_configure(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_dataset_t * dataset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_delay_mechanism_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *delay_mechanism)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_delay_mechanism_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 delay_mechanism)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_disable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_enable(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_identity_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_identity_t * identity)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_info_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_latency_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 latency_in,
    uint32 latency_out)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_announce_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_announce_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_min_delay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_min_delay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_min_pdelay_req_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_min_pdelay_req_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_sync_interval_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int *interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_log_sync_interval_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_mac_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_protocol_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_protocol_t * protocol)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_type_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_port_type_t * type)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_version_number_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    uint32 *version)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_priority1_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_priority1_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_priority2_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *priority2)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_priority2_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 priority2)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_quality_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t * clock_quality,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_quality_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_quality_t clock_quality,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_slaveonly_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *slaveonly)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_slaveonly_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 slaveonly)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_time_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t * time)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_time_properties_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_time_properties_t * data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_time_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timestamp_t * time)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_timescale_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t * timescale)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_timescale_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timescale_t * timescale)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_traceability_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t * trace)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_traceability_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_trace_t * trace)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_user_description_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 *desc)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_utc_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t * utc)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_utc_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_utc_t * utc)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_alarm_callback_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_ctdev_alarm_cb alarm_cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_alarm_callback_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_alpha_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 *alpha_numerator,
    uint16 *alpha_denominator)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_alpha_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 alpha_numerator,
    uint16 alpha_denominator)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_ctdev_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_foreign_master_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_foreign_master_dataset_t * data_set)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_g8275p1_traceability_info_clock_class_map(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_g8275p1_clock_traceability_info_t traceability_info,
    uint8 *clock_class)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_input_channel_precedence_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_select_mode)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_input_channel_switching_mode_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int channel_switching_mode)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_input_channels_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_channels,
    bcm_ptp_channel_t * channels)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_input_channels_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_channels,
    bcm_ptp_channel_t * channels)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_enable_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *enable,
    uint32 *flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int enable,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_phyts_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *phyts,
    int *framesync_pin)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_phyts_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int phyts,
    int framesync_pin)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_portbitmap_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_portbitmap_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_verbose_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *verbose)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_modular_verbose_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int verbose)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_packet_counters_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 counter_bitmap)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_packet_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_packet_counters_t * counters)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_peer_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_peers,
    bcm_ptp_peer_entry_t * peers,
    int *num_peers)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_phase_offset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_phase_offset_t * offset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_phase_offset_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    const bcm_ptp_phase_offset_t * offset)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_primary_domain_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *primary_domain)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_primary_domain_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int primary_domain)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_servo_configuration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_servo_configuration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_servo_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_status_t * status)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_servo_threshold_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t * threshold)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_servo_threshold_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_servo_threshold_t * threshold)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signal_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_count,
    bcm_ptp_signal_output_t * signal_output_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signal_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int signal_output_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signal_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *signal_output_id,
    bcm_ptp_signal_output_t * output_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaled_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t * master_info,
    uint32 mask)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaled_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaled_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t * slave_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaled_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int callstack)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaling_arbiter_register(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_signaling_arbiter_t arb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_signaling_arbiter_unregister(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_stack_create(
    int unit,
    bcm_ptp_stack_info_t * ptp_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_stack_destroy(
    int unit,
    bcm_ptp_stack_id_t ptp_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_stack_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_stack_info_t * ptp_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_stack_get_all(
    int unit,
    int max_size,
    bcm_ptp_stack_info_t * ptp_info,
    int *no_of_stacks)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_unicast_master_t * master_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_masters,
    int *num_masters,
    bcm_ptp_clock_peer_address_t * master_addr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_address_t * master_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_master_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_master_table_size_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int *max_table_entries)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_slave_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t * slave_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_slave_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    int max_num_slaves,
    int *num_slaves,
    bcm_ptp_clock_peer_t * slave_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_slave_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_peer_t * slave_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_static_unicast_slave_table_clear(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_sync_phy(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_sync_phy_input_t sync_input)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_bs_time_info_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_bs_time_info_t * time)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_init(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_network_option_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t * network_option)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_network_option_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_network_option_t network_option)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_add(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_best_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_clock_port_address_t * address,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_list(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int max_master_count,
    int *num_masters,
    int *best_master,
    bcm_ptp_telecom_g8265_pktmaster_t * pktmaster)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_lockout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 lockout,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_non_reversion_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 nonres,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_priority_override(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint8 override,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_priority_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 priority,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_packet_master_wait_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint64 wait_sec,
    bcm_ptp_clock_port_address_t * address)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_pktstats_thresholds_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t * thresholds)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_pktstats_thresholds_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_pktstats_t thresholds)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_quality_level_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_telecom_g8265_quality_level_t ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_receipt_timeout_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 *receipt_timeout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_receipt_timeout_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_message_type_t message_type,
    uint32 receipt_timeout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_telecom_g8265_shutdown(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_time_format_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    bcm_ptp_time_type_t type)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_timesource_input_status_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_timesource_status_t * status)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_tod_input_sources_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *num_tod_sources,
    bcm_ptp_tod_input_t * tod_sources)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_tod_input_sources_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int num_tod_sources,
    bcm_ptp_tod_input_t * tod_sources)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_tod_output_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_count,
    bcm_ptp_tod_output_t * tod_output)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_tod_output_remove(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int tod_output_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_tod_output_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int *tod_output_id,
    bcm_ptp_tod_output_t * output_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_transparent_clock_default_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    bcm_ptp_transparent_clock_default_dataset_t * data_set)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_transparent_clock_port_dataset_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint16 clock_port,
    bcm_ptp_transparent_clock_port_dataset_t * data_set)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_max_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_max_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_min_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 *duration_min)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_min_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration_min)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_unicast_request_duration_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    int port_num,
    uint32 duration)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_drop_counters_enable_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_port_drop_counters_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 clock_port,
    bcm_ptp_clock_port_packet_drop_counters_t * per_port_packet_drop_counters)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_bmca_override_set(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 flags,
    uint32 enable_mask)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_ptp_clock_bmca_override_get(
    int unit,
    bcm_ptp_stack_id_t ptp_id,
    int clock_num,
    uint32 *flags,
    uint32 *enable_mask)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_config_get(
    int unit,
    bcm_regex_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_config_set(
    int unit,
    bcm_regex_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_create(
    int unit,
    bcm_regex_engine_config_t * config,
    bcm_regex_engine_t * engid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_destroy(
    int unit,
    bcm_regex_engine_t engid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_get(
    int unit,
    bcm_regex_engine_t engid,
    bcm_regex_engine_config_t * config)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_info_get(
    int unit,
    int engine_id,
    bcm_regex_engine_info_t * regex_engine_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_engine_traverse(
    int unit,
    bcm_regex_engine_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_add(
    int unit,
    uint8 protocol,
    uint16 l4_start,
    uint16 l4_end)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_delete(
    int unit,
    uint8 protocol,
    uint16 l4_start,
    uint16 l4_end)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_exclude_get(
    int unit,
    int array_size,
    uint8 *protocol,
    uint16 *l4low,
    uint16 *l4high,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_get_match_id(
    int unit,
    int signature_id,
    int *match_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_get_sig_id(
    int unit,
    int match_id,
    int *signature_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_info_get(
    int unit,
    bcm_regex_info_t * regex_info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_match_check(
    int unit,
    bcm_regex_match_t * matches,
    int count,
    int *metric)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_match_set(
    int unit,
    bcm_regex_engine_t engid,
    bcm_regex_match_t * matches,
    int count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 param0,
    uint32 param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_delete(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 param0,
    uint32 param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    uint32 *param0,
    uint32 *param1)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_mac_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_mac_t mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_mac_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_ports_add(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_ports_get(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_remove(
    int unit,
    bcm_regex_policy_t policy,
    bcm_field_action_t action)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_action_remove_all(
    int unit,
    bcm_regex_policy_t policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_create(
    int unit,
    int flags,
    bcm_regex_policy_t * policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_REGEX)
int
bcm_dnx_regex_policy_destroy(
    int unit,
    bcm_regex_policy_t policy)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_bandwidth_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t * bandwidth)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_bandwidth_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bandwidth_t * bandwidth)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_bindings_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t * bindings)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_bindings_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_bindings_t * bindings)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_phase_control_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t * phase_control)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_phase_control_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_tdpll_dpll_phase_control_t * phase_control)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_dpll_reference_get(
    int unit,
    int stack_id,
    int max_num_dpll,
    int *dpll_ref,
    int *num_dpll)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_holdover_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t * ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_holdover_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_mac_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_mac_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_ql_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t * ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_ql_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_esmc_quality_level_t ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_rx_enable_get(
    int unit,
    int stack_id,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_rx_enable_set(
    int unit,
    int stack_id,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_rx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_rx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_rx_state_machine(
    int unit,
    int stack_id,
    int ingress_port,
    bcm_esmc_pdu_data_t * esmc_pdu_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_tx_enable_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_tx_enable_set(
    int unit,
    int stack_id,
    int dpll_index,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_tx_portbitmap_get(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_esmc_tx_portbitmap_set(
    int unit,
    int stack_id,
    int dpll_index,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_best_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *best_clock)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_control(
    int unit,
    int stack_id,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_change_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_ql_change_cb ql_change_cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_change_callback_unregister(
    int unit,
    int stack_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_frequency_error_get(
    int unit,
    int stack_id,
    int clock_index,
    int *freq_error_ppb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *clock_frequency,
    uint32 *tsevent_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 clock_frequency,
    uint32 tsevent_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_l1mux_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t * l1mux)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_l1mux_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_l1mux_t * l1mux)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_lockout_get(
    int unit,
    int stack_id,
    int clock_index,
    int *lockout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_lockout_set(
    int unit,
    int stack_id,
    int clock_index,
    int lockout)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_mac_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_mac_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_mac_t * mac)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_cb monitor_cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_callback_unregister(
    int unit,
    int stack_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_interval_get(
    int unit,
    int stack_id,
    uint32 *monitor_interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_interval_set(
    int unit,
    int stack_id,
    uint32 monitor_interval)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_threshold_get(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 *threshold)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_monitor_threshold_set(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    uint32 threshold)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_priority_get(
    int unit,
    int stack_id,
    int clock_index,
    int *priority)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_priority_set(
    int unit,
    int stack_id,
    int clock_index,
    int priority)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_enabled_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *ql_enabled)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_enabled_set(
    int unit,
    int stack_id,
    int dpll_index,
    int ql_enabled)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_ql_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_esmc_quality_level_t ql)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_revertive_get(
    int unit,
    int stack_id,
    int dpll_index,
    int *revertive)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_revertive_set(
    int unit,
    int stack_id,
    int dpll_index,
    int revertive)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_selector_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_input_clock_selector_cb selector_cb)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_selector_callback_unregister(
    int unit,
    int stack_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_callback_register(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type,
    bcm_tdpll_input_clock_cb callback)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_callback_unregister(
    int unit,
    int stack_id,
    bcm_tdpll_callback_type_t type)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_threshold_state_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_input_clock_monitor_type_t threshold_type,
    int *threshold_state)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_valid_get(
    int unit,
    int stack_id,
    int clock_index,
    int *valid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_input_clock_valid_set(
    int unit,
    int stack_id,
    int clock_index,
    int valid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_deriv_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *deriv_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_deriv_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 deriv_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_enable_get(
    int unit,
    int stack_id,
    int clock_index,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_enable_set(
    int unit,
    int stack_id,
    int clock_index,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_holdover_data_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_data_t * hdata)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_holdover_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_frequency_correction_t hfreq)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_holdover_mode_get(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t * hmode)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_holdover_mode_set(
    int unit,
    int stack_id,
    int clock_index,
    bcm_tdpll_holdover_mode_t hmode)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_holdover_reset(
    int unit,
    int stack_id,
    int clock_index)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_synth_frequency_get(
    int unit,
    int stack_id,
    int clock_index,
    uint32 *synth_frequency,
    uint32 *tsevent_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_PTP)
int
bcm_dnx_tdpll_output_clock_synth_frequency_set(
    int unit,
    int stack_id,
    int clock_index,
    uint32 synth_frequency,
    uint32 tsevent_frequency)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_delete_by_interface(
    int unit,
    bcm_l3_host_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_delete_by_network(
    int unit,
    bcm_l3_route_t * ip_addr)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_invalidate_entry(
    int unit,
    bcm_ip_t info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_host_validate_entry(
    int unit,
    bcm_ip_t info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_info(
    int unit,
    bcm_l3_info_t * l3info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_destroy(
    int unit,
    bcm_if_t intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_find(
    int unit,
    bcm_l3_ingress_t * ing_intf,
    bcm_if_t * intf_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_ingress_traverse(
    int unit,
    bcm_l3_ingress_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_intf_find_vlan(
    int unit,
    bcm_l3_intf_t * intf)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_age(
    int unit,
    uint32 flags,
    bcm_l3_route_traverse_cb age_out,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_delete_by_interface(
    int unit,
    bcm_l3_route_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_max_ecmp_get(
    int unit,
    int *max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_max_ecmp_set(
    int unit,
    int max)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_route_multipath_get(
    int unit,
    bcm_l3_route_t * the_route,
    bcm_l3_route_t * path_array,
    int max_path,
    int *path_count)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_add(
    int unit,
    bcm_l3_source_bind_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_delete(
    int unit,
    bcm_l3_source_bind_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_get(
    int unit,
    bcm_l3_source_bind_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_l2_egress_find(
    int unit,
    bcm_l2_egress_t * egr,
    bcm_if_t * encap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_egress_traverse(
    int unit,
    bcm_l2_egress_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_source_bind_traverse(
    int unit,
    bcm_l3_source_bind_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_add(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_delete(
    int unit,
    bcm_vlan_t vlan,
    uint32 vrid)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_delete_all(
    int unit,
    bcm_vlan_t vlan)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_l3_vrrp_get(
    int unit,
    bcm_vlan_t vlan,
    int alloc_size,
    int *vrid_array,
    int *count)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_lb_control_get(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_control_t lb_control,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_control_set(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_control_t lb_control,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_enable_get(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    uint32 *enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_enable_set(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    uint32 enable)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_flush_configure_get(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    bcm_lb_flush_type_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_flush_configure_set(
    int unit,
    bcm_lbg_t lbg_id,
    uint32 flags,
    bcm_lb_flush_type_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_control_get(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_modem_control_t modem_control,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_control_set(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_modem_control_t modem_control,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_packet_config_get(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_lb_modem_packet_config_t * packet_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_packet_config_set(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_lb_modem_packet_config_t * packet_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_shaper_get(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_lb_modem_shaper_config_t * shaper)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_shaper_set(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_lb_modem_shaper_config_t * shaper)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_stat_get(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_modem_stat_val_t type,
    uint64 *value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_status_get(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lb_modem_status_type_t status_type,
    int *value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_to_lbg_map_get(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lbg_t * lbg_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_to_lbg_map_set(
    int unit,
    bcm_modem_t modem_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    bcm_lbg_t lbg_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_to_port_map_get(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_gport_t * port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_modem_to_port_map_set(
    int unit,
    bcm_modem_t modem_id,
    uint32 flags,
    bcm_gport_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_packet_config_get(
    int unit,
    uint32 flags,
    bcm_lb_packet_config_t * packet_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_packet_config_set(
    int unit,
    uint32 flags,
    bcm_lb_packet_config_t * packet_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_rx_modem_map_get(
    int unit,
    bcm_lb_rx_modem_map_index_t * map_index,
    uint32 flags,
    bcm_lb_rx_modem_map_config_t * map_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_rx_modem_map_set(
    int unit,
    bcm_lb_rx_modem_map_index_t * map_index,
    uint32 flags,
    bcm_lb_rx_modem_map_config_t * map_config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_lb_sequence_number_width_get(
    int unit,
    bcm_lbg_t lbg_id,
    bcm_lb_direction_type_t direction,
    uint32 flags,
    int *number_width)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_list(
    int unit,
    bcm_pkt_t * pkt,
    bcm_pkt_cb_f all_done_cb,
    void *cookie)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_pkt_l2_map(
    int unit,
    bcm_pkt_t * pkt,
    bcm_mac_t dest_mac,
    int vid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_pkt_setup(
    int unit,
    bcm_pkt_t * tx_pkt)
{
    return BCM_E_UNAVAIL;
}

#if defined(BROADCOM_DEBUG)
int
bcm_dnx_tx_show(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_txbeacon_destroy(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_init(
    int unit,
    int uC)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_pkt_get(
    int unit,
    bcm_txbeacon_t * txbeacon)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_pkt_setup(
    int unit,
    bcm_txbeacon_t * txbeacon)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_start(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_stop(
    int unit,
    bcm_txbeacon_pkt_id_t pkt_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_txbeacon_traverse(
    int unit,
    bcm_txbeacon_traverse_cb_t trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_tx_array(
    int unit,
    bcm_pkt_t ** pkt,
    int count,
    bcm_pkt_cb_f all_done_cb,
    void *cookie)
{
    return BCM_E_UNAVAIL;
}

#if defined(BROADCOM_DEBUG)
int
bcm_dnx_tx_dv_dump(
    int unit,
    void *dv_p)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_auth_mode_get(
    int unit,
    int port,
    uint32 *modep)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_auth_mode_set(
    int unit,
    int port,
    uint32 mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_control_vsan_get(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_control_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_control_vsan_set(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_control_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_enable_set(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_add(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_by_interface(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_delete_by_prefix(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_route_find(
    int unit,
    bcm_fcoe_route_t * route)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_create(
    int unit,
    uint32 options,
    bcm_fcoe_vsan_t * vsan,
    bcm_fcoe_vsan_id_t * vsan_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_destroy(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_get(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_t * vsan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_vsan_set(
    int unit,
    bcm_fcoe_vsan_id_t vsan_id,
    bcm_fcoe_vsan_t * vsan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_add(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_did(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_sid(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_delete_by_vsan(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_fcoe_zone_get(
    int unit,
    bcm_fcoe_zone_entry_t * zone)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_cfg_get(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_channels_running(
    int unit,
    uint32 *channels)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_clear(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_control_get(
    int unit,
    bcm_rx_control_t type,
    int *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_control_set(
    int unit,
    bcm_rx_control_t type,
    int arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_cosq_mapping_get(
    int unit,
    int index,
    bcm_rx_reasons_t * reasons,
    bcm_rx_reasons_t * reasons_mask,
    uint8 *int_prio,
    uint8 *int_prio_mask,
    uint32 *packet_type,
    uint32 *packet_type_mask,
    bcm_cos_queue_t * cosq)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_cosq_mapping_set(
    int unit,
    int index,
    bcm_rx_reasons_t reasons,
    bcm_rx_reasons_t reasons_mask,
    uint8 int_prio,
    uint8 int_prio_mask,
    uint32 packet_type,
    uint32 packet_type_mask,
    bcm_cos_queue_t cosq)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_cpu_rate_get(
    int unit,
    int *pps)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_free_enqueue(
    int unit,
    void *pkt_data)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_queue_max_get(
    int unit,
    bcm_cos_queue_t * cosq)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_queue_packet_count_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *packet_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_queue_rate_limit_status_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *packet_tokens)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_rate_get(
    int unit,
    int *pps)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_reasons_policer_set(
    int unit,
    bcm_rx_reasons_t rx_reasons,
    bcm_policer_t polid)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_sched_register(
    int unit,
    bcm_rx_sched_cb sched_cb)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_sched_unregister(
    int unit)
{
    return BCM_E_UNAVAIL;
}

#if defined(BROADCOM_DEBUG)
int
bcm_dnx_rx_show(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_rx_snoop_create(
    int unit,
    int flags,
    int *snoop_cmnd)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_snoop_destroy(
    int unit,
    int flags,
    int snoop_cmnd)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_snoop_get(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_snoop_set(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_trap_sw_to_hw_id_map_get(
    int unit,
    uint32 flags,
    int sw_trap_id,
    int *hw_trap_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_rx_unit_next_get(
    int unit,
    int *next_unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_egress_get(
    int unit,
    bcm_port_t port,
    int *val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_egress_set(
    int unit,
    bcm_port_t port,
    int val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_ingress_get(
    int unit,
    bcm_port_t port,
    int *val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_ingress_set(
    int unit,
    bcm_port_t port,
    int val)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_mode_get(
    int unit,
    int *mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_mode_set(
    int unit,
    int mode)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_get(
    int unit,
    bcm_port_t port,
    bcm_module_t * dest_mod,
    bcm_port_t * dest_port,
    uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_info_get(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_mirror_port_info_t * info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_info_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    bcm_mirror_port_info_t * info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_set(
    int unit,
    bcm_port_t port,
    bcm_module_t dest_mod,
    bcm_port_t dest_port,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_vlan_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vlan,
    bcm_gport_t * dest_port,
    uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_port_vlan_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vlan,
    bcm_gport_t destport,
    uint32 flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_to_get(
    int unit,
    bcm_port_t * port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_to_pbmp_get(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t * pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_to_pbmp_set(
    int unit,
    bcm_port_t port,
    bcm_pbmp_t pbmp)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_to_set(
    int unit,
    bcm_port_t port)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_vlan_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid,
    uint16 *vlan)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_mirror_vlan_set(
    int unit,
    bcm_port_t port,
    uint16 tpid,
    uint16 vlan)
{
    return BCM_E_UNAVAIL;
}

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_cleanup(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_adjacency_add(
    int unit,
    bcm_trill_multicast_adjacency_t * trill_multicast_adjacency)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_adjacency_delete(
    int unit,
    bcm_trill_multicast_adjacency_t * trill_multicast_adjacency)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_adjacency_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_adjacency_traverse(
    int unit,
    bcm_trill_multicast_adjacency_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_delete_all(
    int unit,
    bcm_trill_name_t root_name)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_entry_add(
    int unit,
    bcm_trill_multicast_entry_t * trill_mc_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_entry_delete(
    int unit,
    bcm_trill_multicast_entry_t * trill_mc_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_entry_get(
    int unit,
    bcm_trill_multicast_entry_t * trill_mc_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_entry_traverse(
    int unit,
    bcm_trill_multicast_entry_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_source_add(
    int unit,
    bcm_trill_name_t root_name,
    bcm_trill_name_t source_rbridge_name,
    bcm_gport_t port,
    bcm_if_t encap_intf)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_source_delete(
    int unit,
    bcm_trill_name_t root_name,
    bcm_trill_name_t source_rbridge_name,
    bcm_gport_t port,
    bcm_if_t encap_intf)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_source_get(
    int unit,
    bcm_trill_name_t root_name,
    bcm_trill_name_t source_rbridge_name,
    bcm_gport_t * port)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_multicast_source_traverse(
    int unit,
    bcm_trill_multicast_source_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_port_add(
    int unit,
    bcm_trill_port_t * trill_port)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_port_delete(
    int unit,
    bcm_gport_t trill_port_id)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_port_delete_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_port_get(
    int unit,
    bcm_trill_port_t * trill_port)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_port_get_all(
    int unit,
    int port_max,
    bcm_trill_port_t * port_array,
    int *port_count)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_rbridge_entry_add(
    int unit,
    bcm_trill_rbridge_t * trill_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_rbridge_entry_delete(
    int unit,
    bcm_trill_rbridge_t * trill_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_rbridge_entry_get(
    int unit,
    bcm_trill_rbridge_t * trill_entry)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_vpn_create(
    int unit,
    bcm_trill_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_vpn_destroy(
    int unit,
    bcm_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_vpn_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_vpn_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_trill_vpn_config_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
#if defined(INCLUDE_L3)
int
bcm_dnx_trill_vpn_traverse(
    int unit,
    bcm_trill_vpn_traverse_cb cb,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
#endif
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_config_get(
    int unit,
    bcm_tunnel_config_t * tconfig)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_config_set(
    int unit,
    bcm_tunnel_config_t * tconfig)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_create(
    int unit,
    uint32 flags,
    int *dscp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_destroy(
    int unit,
    int dscp_map_id)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_get(
    int unit,
    int dscp_map_id,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_port_get(
    int unit,
    bcm_port_t port,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_port_set(
    int unit,
    bcm_port_t port,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_dscp_map_set(
    int unit,
    int dscp_map_id,
    bcm_tunnel_dscp_map_t * dscp_map)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t * intf,
    bcm_tunnel_initiator_t * tunnel)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_add(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_update(
    int unit,
    bcm_tunnel_terminator_t * info)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_vlan_get(
    int unit,
    bcm_gport_t tunnel,
    bcm_vlan_vector_t * vlan_vec)
{
    return BCM_E_UNAVAIL;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_tunnel_terminator_vlan_set(
    int unit,
    bcm_gport_t tunnel,
    bcm_vlan_vector_t vlan_vec)
{
    return BCM_E_UNAVAIL;
}
#endif

int
bcm_dnx_vswitch_destroy_all(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vswitch_detach(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vswitch_flexible_connect_add(
    int unit,
    uint32 flags,
    bcm_vswitch_flexible_connect_match_t * port_match,
    bcm_vswitch_flexible_connect_fwd_t * port_fwd)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vswitch_flexible_connect_delete(
    int unit,
    bcm_vswitch_flexible_connect_match_t * port_match)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vswitch_flexible_connect_get(
    int unit,
    uint32 flags,
    bcm_vswitch_flexible_connect_match_t * port_match,
    bcm_vswitch_flexible_connect_fwd_t * port_fwd)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vswitch_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_multicast_group_get(
    int unit,
    bcm_multicast_t group,
    uint32 *flags)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_get(
    int unit,
    int qual_id,
    bcm_field_data_qualifier_t * qual_p)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_destroy(
    int unit,
    int qual_id)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_destroy_all(
    int unit)
{

    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_create(
    int unit,
    bcm_field_data_qualifier_t * qual_p)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_bfd_discard_stat_get(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_bfd_discard_stat_set(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_bfd_status_multi_get(
    int unit,
    int max_endpoints,
    bcm_bfd_status_t * status_arr,
    int *count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_intf_vpn_set(
    int unit,
    bcm_if_t l3_intf_id,
    uint32 flags,
    bcm_vpn_t vpn)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l3_intf_vpn_get(
    int unit,
    bcm_if_t l3_intf_id,
    uint32 *flags,
    bcm_vpn_t * vpn)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_qos_map_control_set(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    uint64 arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_qos_map_control_get(
    int unit,
    uint32 map_id,
    uint32 flags,
    bcm_qos_map_control_type_t type,
    uint64 *arg)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_policer_stat_get32(
    int unit,
    bcm_policer_t policer_id,
    bcm_cos_t int_pri,
    bcm_policer_stat_t stat,
    uint32 *value)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_multi_get(
    int unit,
    bcm_gport_t port,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_max,
    bcm_switch_obm_classifier_t * switch_obm_classifier,
    int *array_count)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_switch_obm_classifier_mapping_multi_set(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    int array_count,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_l2_addr_delete_by_vlan_gport_multi(
    int unit,
    uint32 flags,
    int num_pairs,
    bcm_vlan_t * vlan,
    bcm_gport_t * gport)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_vlan_port_traverse(
    int unit,
    bcm_vlan_port_traverse_info_t * additional_info,
    bcm_vlan_port_traverse_cb trav_fn,
    void *user_data)
{
    return BCM_E_UNAVAIL;
}
