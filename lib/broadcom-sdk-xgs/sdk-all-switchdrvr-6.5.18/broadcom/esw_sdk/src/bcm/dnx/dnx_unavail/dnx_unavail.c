/** \file dnx_unavail.c
 * $Id$
 *
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/vlan_access.h>
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
bcm_dnx_qos_port_vlan_map_get(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int *ing_map,
    int *egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API unavailable, instead use bcm_vlan_port_find() followed by bcm_qos_port_map_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_qos_port_vlan_map_set(
    int unit,
    bcm_port_t port,
    bcm_vlan_t vid,
    int ing_map,
    int egr_map)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                 "API unavailable, instead use bcm_vlan_port_create() followed by bcm_qos_port_map_set()");

exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_sha1_set(
    int unit,
    int index,
    bcm_bfd_auth_sha1_t * sha1)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_get(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_auth_simple_password_set(
    int unit,
    int index,
    bcm_bfd_auth_simple_password_t * sp)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, authentication no longer supported");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_endpoint_poll(
    int unit,
    bcm_bfd_endpoint_t endpoint)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_BFD)
int
bcm_dnx_bfd_tx_start(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, start/stop no longer supported");

exit:
    SHR_FUNC_EXIT;
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
bcm_dnx_l2_age_timer_set(
    int unit,
    int age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_l2_age_timer_meta_cycle_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_age_timer_get(
    int unit,
    int *age_seconds)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_l2_age_timer_meta_cycle_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_delete(
    int unit,
    int index)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_get(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_profile_get(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_profile_set(
    int unit,
    int trap_type,
    uint32 profile_idx,
    uint32 flags,
    bcm_l2cp_profile_info_t * l2cp_profile_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_set(
    int unit,
    int index,
    bcm_l2_cache_addr_t * addr,
    int *index_used)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_size_get(
    int unit,
    int *size)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_get(
    int unit,
    uint32 vsi,
    uint32 *profile_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l2_cache_vpn_to_profile_map_set(
    int unit,
    uint32 vsi,
    uint32 profile_idx)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_protocol_set()");

exit:
    SHR_FUNC_EXIT;
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
bcm_dnx_port_cfi_color_get(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");
exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_cfi_color_set(
    int unit,
    bcm_port_t port,
    int cfi,
    bcm_color_t color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_discard_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_lif_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_discard_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_rx_trap_lif_set()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_get(
    int unit,
    bcm_port_t port,
    int srccp,
    int *mapcp,
    int *prio)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_mode_get(
    int unit,
    bcm_port_t port,
    int *mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_dscp_map_set(
    int unit,
    bcm_port_t port,
    int srccp,
    int mapcp,
    int prio)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
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
bcm_dnx_port_inner_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_get_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_inner_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_priority_color_get(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t * color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_priority_color_set(
    int unit,
    bcm_port_t port,
    int prio,
    bcm_color_t color)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for port mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_add(
    int unit,
    bcm_port_t port,
    uint16 tpid,
    int color_select)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_delete(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_delete() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_delete_all(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_delete_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_get(
    int unit,
    bcm_port_t port,
    uint16 *tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_get_all() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_tpid_set(
    int unit,
    bcm_port_t port,
    uint16 tpid)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_switch_tpid_add() per device");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_untagged_priority_get(
    int unit,
    bcm_port_t port,
    int *priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for Untagged mapping");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_port_untagged_priority_set(
    int unit,
    bcm_port_t port,
    int priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs for Untagged mapping");

exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use QoS mapping APIs");

exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_vlan_port_find()");

exit:
    SHR_FUNC_EXIT;
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

int
bcm_dnx_extender_forward_add(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_add()");

exit:
    SHR_FUNC_EXIT;
}

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_delete()");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_delete_all(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_delete_all()");

exit:
    SHR_FUNC_EXIT;
}
#endif

#if defined(INCLUDE_L3)
int
bcm_dnx_extender_forward_get(
    int unit,
    bcm_extender_forward_t * extender_forward_entry)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_extender_port_get()");

exit:
    SHR_FUNC_EXIT;
}
#endif

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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_create(
    int unit,
    uint32 flags,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_delete(
    int unit,
    bcm_if_t mpintf,
    bcm_if_t intf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_destroy(
    int unit,
    bcm_if_t mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_find(
    int unit,
    int intf_count,
    bcm_if_t * intf_array,
    bcm_if_t * mpintf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_get(
    int unit,
    bcm_if_t mpintf,
    int intf_size,
    bcm_if_t * intf_array,
    int *intf_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_l3_egress_multipath_traverse(
    int unit,
    bcm_l3_egress_multipath_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use APIs with the prefix bcm_l3_egress_ecmp");

exit:
    SHR_FUNC_EXIT;
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
bcm_dnx_rx_snoop_create(
    int unit,
    int flags,
    int *snoop_cmnd)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_create()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_destroy(
    int unit,
    int flags,
    int snoop_cmnd)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_destroy()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_get(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_rx_snoop_set(
    int unit,
    int snoop_cmnd,
    bcm_rx_snoop_config_t * config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_mirror_destination_set()");

exit:
    SHR_FUNC_EXIT;
}

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
bcm_dnx_vswitch_init(
    int unit)
{
    return BCM_E_UNAVAIL;
}

int
bcm_dnx_field_data_qualifier_get(
    int unit,
    int qual_id,
    bcm_field_data_qualifier_t * qual_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_info_get()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_destroy(
    int unit,
    int qual_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_destroy()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_destroy_all(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_destroy() for each entry");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_field_data_qualifier_create(
    int unit,
    bcm_field_data_qualifier_t * qual_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, instead use bcm_field_qualifier_create()");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_bfd_discard_stat_get(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_bfd_discard_stat_set(
    int unit,
    bcm_bfd_discard_stat_t * discarded_info)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API unavailable, statistics can be accessed per endpoint");

exit:
    SHR_FUNC_EXIT;
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
bcm_dnx_switch_obm_classifier_mapping_get(
    int unit,
    bcm_gport_t gport,
    bcm_switch_obm_classifier_type_t switch_obm_classifier_type,
    bcm_switch_obm_classifier_t * switch_obm_classifier)
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
