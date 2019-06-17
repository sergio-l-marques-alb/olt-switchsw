/** \file srv6.c
 * $Id$
 *
 * General SRv6 functionality for DNX.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SRV6
/*
 * Include files.
 * {
 */
#include <bcm_int/dnx/srv6/srv6.h>

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

/*
 * DEFINES
 * {
 */
/**
 * \brief - SRv6 Min packet size.
 * Minimum packet size in bytes to perform the SID swap
 * mechanism
 * The size are calculated as:
 * ETH0+IPv6+SRH_BASE+SID = 14+36+8+16 = 74Bytes
 */
#define SRV6_MIN_PACKET_SIZE        (74)
/*
 * }
 */

/*
 * see .h file for description
 */
shr_error_e
dnx_srv6_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Global configuration init */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SRV6_GLOBAL_CONFIGURATION, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_EXTENSION_HEADER_TYPE, INST_SINGLE,
                                 DBAL_DEFINE_IPV6_EXTENSION_HEADER_TYPE_SRV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV6_ETHERTYPE, INST_SINGLE,
                                 DBAL_DEFINE_ETHERTYPE_IPV6);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_MIN_PACKET_SIZE, INST_SINGLE,
                                 SRV6_MIN_PACKET_SIZE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Per PTC configuration */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_INGRESS_PTC_PORT, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PTC, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRV6_SID_SWAP_ENABLE, INST_SINGLE, TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_srv6_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_sid_initiator_create(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_sid_initiator_delete(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_sid_initiator_get(
    int unit, 
    bcm_srv6_sid_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_sid_initiator_traverse(
    int unit, 
    bcm_srv6_sid_initiator_traverse_info_t additional_info, 
    bcm_srv6_sid_initiator_traverse_cb cb, 
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_srv6_srh_base_initiator_create(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_srv6_srh_base_initiator_delete(
    int unit, 
    bcm_srv6_srh_base_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_srv6_srh_base_initiator_get(
    int unit,
    bcm_srv6_srh_base_initiator_info_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_srv6_srh_base_initiator_traverse(
    int unit, 
    bcm_srv6_srh_base_initiator_traverse_info_t additional_info, 
    bcm_srv6_srh_base_initiator_traverse_cb cb, 
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int 
bcm_dnx_srv6_terminator_next_protocol_add(
    int unit, 
    bcm_srv6_terminator_next_protocol_mapping_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_terminator_next_protocol_delete(
    int unit, 
    bcm_srv6_terminator_next_protocol_mapping_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_terminator_next_protocol_get(
    int unit, 
    bcm_srv6_terminator_next_protocol_mapping_t *info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_srv6_terminator_next_protocol_traverse(
    int unit, 
    bcm_srv6_terminator_next_protocol_traverse_info_t additional_info, 
    bcm_srv6_terminator_next_protocol_traverse_cb cb, 
    void *user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "API bcm_cosq_pkt_size_adjust_delta_map_set() is not supported for this device type.");

exit:
    SHR_FUNC_EXIT;
}
