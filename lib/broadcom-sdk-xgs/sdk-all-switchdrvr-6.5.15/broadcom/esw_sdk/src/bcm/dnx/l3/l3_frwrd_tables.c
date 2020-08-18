/** \file l3_frwrd_tables.c
 *
 * L3 forwarding tables procedures for DNX
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_L3

/*
 * Include files currently used for DNX.
 * {
 */
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/stat/stat_pp.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>

/*
 * }
 */
/*
 * Include files.
 * {
 */
#include <bcm/types.h>
#include <bcm/l3.h>
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * Maximum number of route tables - this number includes private and public table.
 */
#define MAX_NOF_L3_ROUTE_TABLES 4

/**stat_id is 17 bits long */
#define L3_FRWRD_TABLES_MAX_STAT_ID_VALUE 0x1FFFF

/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * Indicates whether the input address (addr) is an IPV4 unicast address (does not have a 4 bit prefix of 0xe)
 */
#define L3_IPV4_ADDRESS_IS_UNICAST(addr) (((addr) & 0xf0000000) != 0xe0000000)
/*
 * Indicates whether the input address (addr) is an IPV6 unicast address (first byte is not 0xff)
 */
#define L3_IPV6_ADDRESS_IS_UNICAST(addr) ((addr[0]) != 0xff)
/*
 * Whether an IPv4 entry is default or not, relevant only for route entries.
 */
#define L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(ip_mask) (ip_mask == 0)

/*
 * Whether an IPv6 entry is default or not, relevant only for route entries.
 * Assuming that the mask is valid if the first byte of the mask is 0 then all the other bytes should
 * be as well hence this is a default route.
 */
#define L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(ip_mask) (ip_mask[0] == 0)
/*
 * Define the list of supported flags for L3 APIs
 */
#define L3_SUPPORTED_BCM_L3_FLAGS (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_HOST_LOCAL | BCM_L3_ENCAP_SPACE_OPTIMIZED)
/** Define the list of supported BCM_L3_FLAGS2_* flags for L3 APIs. */
#define L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2 (BCM_L3_FLAGS2_RAW_ENTRY)
/*
 * Indicates whether the table is KBP
 */
#define L3_TABLE_IS_KBP(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD)  || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD))
/*
 * Indicates whether the table is public
 */
#define L3_TABLE_IS_PUBLIC(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD) || \
        (table == DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD)     || \
        (table == DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD))

 /*
  * Return the appropriate result type according to the KBP table
  */
#define L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD_FWD_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD_FWD_DEST_W_DEFAULT : -1)

/*
 * }
 */

/**
 * \brief
 *  Verify the host entry add/find/delete input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_host_add_verify \n
 *   * dnx_l3_host_get \n
 *   * dnx_l3_host_delete \n
 */
static shr_error_e
dnx_l3_host_verify(
    int unit,
    bcm_l3_host_t * info)
{
    int kaps_public_capacity;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the the l3a_nexthop_mac is zero.
     */
    if (!BCM_MAC_IS_ZERO(info->l3a_nexthop_mac))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "l3a_nexthop_mac is not supported for JR2.");
    }

    /*
     * Verify the the VRF is in range.
     */
    if (info->l3a_vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower than %d.", info->l3a_vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_host_* API.");
    }

    /** Verify supported flags2 */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, ~L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to the bcm_l3_host_* API - %u", info->l3a_flags2);
    }

    /*
     * Verify that the IP address is a unicast address.
     * If BCM_L3_IP6 flag is set, the field holding information about the IPv6 address (l3a_ip6_addr) will be checked.
     * If the flag is not set, the field holding the IPv4 address (l3a_ip_addr) will be checked.
     */
    if (!L3_IPV4_ADDRESS_IS_UNICAST(info->l3a_ip_addr) && !_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv4 address %d provided in l3a_ip_addr field is not a unicast address.",
                     info->l3a_ip_addr);
    }
    else if (!L3_IPV6_ADDRESS_IS_UNICAST(info->l3a_ip6_addr) && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 address, provided in l3a_ip6_addr field, is not a unicast address.");
    }

    /*
     * If capacity is more than 0, then a private and a public lookups are made.
     * However, for host entries there is only a Private table.
     * This means that if the property is not set, a VRF value of 0 is not allowed.
     * When the capacity of the MDB table is 0, then only a private lookup will be carried out.
     * Otherwise, both public and private are active.
     */

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
    if (kaps_public_capacity > 0 && info->l3a_vrf == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Illegal VRF value 0 for UC host entry when table capacity indicates both public and private lookup.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the route entry add/get/delete input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * dnx_l3_route_add_verify \n
 *   * dnx_l3_route_get \n
 *   * dnx_l3_route_delete \n
 */
static shr_error_e
dnx_l3_route_verify(
    int unit,
    bcm_l3_route_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the the VRF is in range.
     */
    if (info->l3a_vrf >= dnx_data_l3.vrf.nof_vrf_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "VRF %d is out of range, should be lower then %d.", info->l3a_vrf,
                     dnx_data_l3.vrf.nof_vrf_get(unit));
    }
    /** Verify supported flags */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags are passed to bcm_l3_route_* API.");
    }

    /** Verify supported flags2 */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, ~L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal L3 flags2 are passed to the bcm_l3_route_* API - %08lX",
                     (unsigned long) (info->l3a_flags2));
    }

    /*
     * Verify that the IP address is a unicast address.
     * If BCM_L3_IP6 flag is set, the field holding information about the IPv6 address (l3a_ip6_net) will be checked.
     * If the flag is not set, the field holding the IPv4 address (l3a_subnet) will be checked.
     */
    if (!L3_IPV4_ADDRESS_IS_UNICAST(info->l3a_subnet) && !_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv4 address %d provided in l3a_ip_addr field is not a unicast address.",
                     info->l3a_subnet);
    }
    else if (!L3_IPV6_ADDRESS_IS_UNICAST(info->l3a_ip6_net) && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "IPv6 address, provided in l3a_ip6_addr field, is not a unicast address.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the host entry add input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 host structure to be verified.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
static shr_error_e
dnx_l3_host_add_verify(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 fec_index;
    uint32 max_fec_id;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_host");

    /*
     * Jericho mode beginning
     * {
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Jericho mode end
     * }
     */

    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
    /*
     * Verify common fields of the host entry
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_verify(unit, info));

    /*
     * Check that the FEC value is in range.
     * The NOF of FECs is not configurable.
     */
    if (fec_index > max_fec_id)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, should be no higher than %d.", fec_index, max_fec_id);
    }

    /*
     * Verify that the out-LIF value is in range.
     */
    if (((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
         && !BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id))
        || (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
    {
        if (BCM_L3_ITF_VAL_GET(info->encap_id) > dnx_data_lif.global_lif.nof_global_out_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OUT_LIF %d is out of range, should be lower than %d.", info->encap_id,
                         dnx_data_lif.global_lif.nof_global_out_lifs_get(unit));
        }
    }

    /*
     * Statistics check
     */
    if ((info->stat_pp_profile != STAT_PP_PROFILE_INVALID))
    {
        if (info->stat_id > L3_FRWRD_TABLES_MAX_STAT_ID_VALUE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Stat id %d, max value 0x%x", info->stat_id, L3_FRWRD_TABLES_MAX_STAT_ID_VALUE);
        }
        SHR_IF_ERR_EXIT(dnx_stat_pp_crps_verify
                        (unit, BCM_CORE_ALL, info->stat_id, info->stat_pp_profile,
                         bcmStatCounterInterfaceIngressReceivePp));
    }

    if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE) &&
        !BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id) && (info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id must be encoded as EEI when using flag BCM_L3_ENCAP_SPACE_OPTIMIZED");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the route entry add input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
static shr_error_e
dnx_l3_route_add_verify(
    int unit,
    bcm_l3_route_t * info)
{
    
    uint32 fec_index;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_route");

    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /*
         * For 'raw' entries, the only limitation is the size of the result. In our case,
         * this is the size of the 'KAPS_RESULT' field.
         */
        uint32 field_size;

        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, DBAL_FIELD_KAPS_RESULT, &field_size));
        if (fec_index >= SAL_BIT(field_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Raw value for 'FEC' (%d) is out of range, should be lower than %d.", fec_index,
                         SAL_BIT(field_size));
        }
    }
    else
    {
        uint32 max_fec_id;
        /*
         * Enter for Non-raw entries.
         */
        SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));

        if (fec_index > max_fec_id)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, should be no higher than %d.", fec_index,
                         max_fec_id);
        }
        /*
         * FECs that are used for default entry has a different range of values.
         */
        if (fec_index > dnx_data_l3.fec.max_default_fec_get(unit)
            &&
            ((!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
              && L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(info->l3a_ip_mask))
             || (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                 && L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(info->l3a_ip6_mask))))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Default FEC index %d is out of range, should be lower then %d.", fec_index,
                         dnx_data_l3.fec.max_default_fec_get(unit));
        }
    }
    /*
     * Verify common fields in the input data.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_verify(unit, info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the input flags to host traverse
 * \param [in] unit - The unit number.
 * \param [in] flags - the control flags passed to the function
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_dnx_l3_host_traverse \n
 */
static shr_error_e
dnx_l3_host_traverse_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (flags != 0 && _SHR_IS_FLAG_SET(flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The flag provided to bcm_l3_host_traverse is not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Verify the input flags to route traverse
 * \param [in] unit - The unit number.
 * \param [in] flags - the control flags passed to the function
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_dnx_l3_route_traverse \n
 */
static shr_error_e
dnx_l3_route_traverse_verify(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    if (flags != 0 && _SHR_IS_FLAG_SET(flags, ~L3_SUPPORTED_BCM_L3_FLAGS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The flag provided to bcm_l3_route_traverse is not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for a UC LMP route table entry.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for EM Private host
 *        table
 * \param [in] entry_handle_id - a handle to the LPM table
 *        (public or private based on the info->l3a_vrf value)
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_host_add
 *  * bcm_dnx_l3_host_delete
 *  * bcm_dnx_l3_host_find
 */
static shr_error_e
dnx_l3_host_to_key(
    int unit,
    bcm_l3_host_t * info,
    uint32 *entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If flag BCM_L3_IP6 is set then the entry belongs to the IPv6 Host table.
     * Valid fields for the key of the entry are IPv6 address(l3a_ip6_addr) and VRF(l3a_vrf).
     *
     * If flag BCM_L3_IP6 is NOT set (else clause) then the entry belongs to the IPv4 Host table.
     * Valid fields for the key of the IPv4 entry key are IPv4 address(l3a_ip_addr) and VRF(l3a_vrf).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);
        dbal_entry_key_field_arr8_set(unit, *entry_handle_id, DBAL_FIELD_IPV6, info->l3a_ip6_addr);
    }
    else
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_IPV4, info->l3a_ip_addr);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set the fields in the key for a UC LMP route table entry.
 * \param [in] unit -
 *   The unit number.
 * \param [in] info -
 *   contains valid fields for LPM Public and Private tables.
 * \param [in] dbal_table -
 *   dbal table (required by KBP)
 * \param [in] entry_handle_id -
 *   a handle to the LPM table (public or private based on
 *   the info->l3a_vrf value)
 * \param [out] destination_type -
 *   This procedure loads pointed memory by the table's result value.
 *   For 'raw' result type, get the full value as it stored in HW,
 *   with no decoding. Otherwise, this indicates FEC or default FEC.
 * \param [out] is_kbp_default -
 *   Default route indication; valid only for KBP tables.
 * \return
 *  * Zero for success
 * \remark
 *   * This procedure assumes a previous procedure (dnx_l3_route_dbal_table_select())
 *     has checked validity:
 *     Currently, it is not allowed to specify BCM_L3_FLAGS2_RAW_ENTRY for KBP.
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_to_key(
    int unit,
    bcm_l3_route_t * info,
    uint32 dbal_table,
    uint32 *entry_handle_id,
    uint32 *destination_type,
    uint8 *is_kbp_default)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Setting key fields:
     * If flag BCM_L3_IP6 is set, then valid fields for the entry are l3a_ip6_net, its mask (l3a_ip6_mask).
     * They are valid for IPv6 entries.
     * If the flag is not set, then an IPv4 entry fiels are valid - l3a_subnet, l3a_ip_mask.
     * VRF value (l3a_vrf) is valid for both IPv4 and IPv6 entries as long as they are private table entries.
     */
    /*
     * In case this is a default entry, the destination type is a default FEC which is limited to a smaller range than
     * the regular FEC and have different encoding in the LPM table.
     * KBP tables have a separate field for default route indication that needs to be set.
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        dbal_entry_key_field_arr8_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV6, info->l3a_ip6_net,
                                             info->l3a_ip6_mask);
        if (L3_TABLE_IS_KBP(dbal_table))
        {
            *destination_type = DBAL_FIELD_FEC;
            *is_kbp_default = (L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(info->l3a_ip6_mask)) ? TRUE : FALSE;
        }
        else
        {
            *destination_type =
                L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(info->l3a_ip6_mask) ? DBAL_FIELD_FEC_DEFAULT : DBAL_FIELD_FEC;
        }
    }
    else
    {
        dbal_entry_key_field32_masked_set(unit, *entry_handle_id, DBAL_FIELD_IPV4, info->l3a_subnet, info->l3a_ip_mask);
        if (L3_TABLE_IS_KBP(dbal_table))
        {
            *destination_type = DBAL_FIELD_FEC;
            *is_kbp_default = (L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(info->l3a_ip_mask)) ? TRUE : FALSE;
        }
        else
        {
            *destination_type =
                L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(info->l3a_ip_mask) ? DBAL_FIELD_FEC_DEFAULT : DBAL_FIELD_FEC;
        }
    }
    /*
     * If 'raw' entry is required then override the 'destination' field (to be used in the calling proceure).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /*
         * If 'raw' entry is required then specify the whole field (which is usually encoded
         * into FEC_DEFAULT or FEC). This indicates 'no encoding'.
         */
        *destination_type = (L3_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT);
    }
    /*
     * Public tables do not have VRF field
     */
    if (!L3_TABLE_IS_PUBLIC(dbal_table))
    {
        dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Determines in which table the L3 route entry belongs.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for determining correct table.
 *      - info->l3a_vrf -
 *          Virtual Router Instance - valid for Private table
 *      - info->l3a_flags -
 *          control flags (eg. BCM_L3_IP6)
 *      - info->l3a_flags2 -
 *          control flags (Currently, using BCM_L3_FLAGS2_RAW_ENTRY)
 * \param [out] dbal_table - returns the name of the dbal table in which the entry will be added
 * \return
 *  * Zero for success
 * \remark
 *    * Currently, it is not allowed to specify BCM_L3_FLAGS2_RAW_ENTRY for KBP. An
 *      error is returned in this case.
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_route_dbal_table_select(
    int unit,
    bcm_l3_route_t * info,
    uint32 *dbal_table)
{
    uint32 kbp_mngr_status;
    int kaps_public_capacity;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));

    /*
     * If flag BCM_L3_IP6 is set, then the entry will be looked for in the IPV6_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv6 address(l3a_ip6_net) and its mask(l3a_ip6_mask).
     *
     * If flag BCM_L3_IP6 is NOT set, then the entry will be looked for in the IPV4_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv4 address(l3a_subnet) and its mask(l3a_ip_mask).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        /*
         * Get a handle to the LPM or KBP table for IPv6 route entries.
         * In case the VRF > 0 or a single DB check is defined a private table is used and
         * the VRF is part of the key which is formed by <VRF, DIP>.
         * In the case of VRF = 0 the public route table is used where the key is composed only using the DIP.
         */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            uint8 kbp_public_enabled = FALSE;
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            SHR_IF_ERR_EXIT(kbp_mngr_ipv6_public_enabled_get(unit, &kbp_public_enabled));

            if ((info->l3a_vrf > 0) || (kbp_public_enabled == FALSE))
            {
                *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD;
            }
        }
        else
        {
            if (info->l3a_vrf > 0 || kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                *dbal_table = DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD;
            }
        }
    }
    else
    {
        /*
         * Get an handle to the LPM table for IPv4 route entries.
         * When the KBP IPv4 forwarding application is enabled, use the KBP table handle.
         * If the KBP forwarding application is disabled or the entry is default, use the KAPS table handle.
         *
         * In case the VRF > 0 or only a private table is used and the entry is private.
         * Here the VRF is part of the key which is formed by VRF, DIP.
         * When the VRF = 0, the public table is being used.
         * The VRF is not part of the key which is composed only from the DIP.
         */

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            uint8 kbp_public_enabled = FALSE;
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            SHR_IF_ERR_EXIT(kbp_mngr_ipv4_public_enabled_get(unit, &kbp_public_enabled));

            if ((info->l3a_vrf > 0) || (kbp_public_enabled == FALSE))
            {
                *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD;
            }
        }
        else
        {
            if (info->l3a_vrf > 0 || kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                *dbal_table = DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD;
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Determines to which table the L3 route entry belongs.
 * \param [in] unit - The unit number.
 * \param [in] info - contains valid fields for determining correct table.
 *      - info->l3a_flags - control flags (eg. BCM_L3_IP6)
 * \param [out] dbal_table - returns the name of the dbal table in which the entry will be added
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_route_add
 *  * bcm_dnx_l3_route_delete
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_host_dbal_table_select(
    int unit,
    bcm_l3_host_t * info,
    uint32 *dbal_table)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * If flag BCM_L3_IP6 is set, then the entry will be added to the IPV6_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv6 address(l3a_ip6_addr).
     *
     * If flag BCM_L3_IP6 is NOT set, then the entry will be added to the IPV4_UNICAST_*_LPM_FORWARD table.
     * Valid fields for it are VRF and IPv4 address(l3a_ip_addr).
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
    {
        /*
         * Get an handle to the IPv6 private host table and set the fields that form the key of the entry
         * When the KBP IPv6 forwarding application is enabled, use the KBP table handle
         */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            *dbal_table = (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HOST_LOCAL)) ?
                DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST : DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST;
        }
    }
    else
    {
        /*
         * Get an handle to the IPv4 private host table and set the fields that form the key of the entry
         * When the KBP IPv4 forwarding application is enabled, use the KBP table handle
         */
        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            *dbal_table = (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HOST_LOCAL)) ?
                DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST : DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        }
        else
        {
            *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds/updates an IP host entry.
 *  Result types of the host that are currently supported:
 *          * FEC
 *          * FEC + EEP(outLIF)
 *          * Port + ETH-RIF + ARP
 *          * FEC + ETH-RIF + ARP
 * \param [in] unit - The unit number.
 * \param [in] info - L3 host information (key and payload) to
 *        be added/updated.
 *      * info->l3a_ip_addr - IPv4 UC address of host;
 *      * info->l3a_ip6_addr - IPv6 UC address of host;
 *      * info->l3a_vrf - Virtual Router Interface;
 *      * info->l3a_flags - control flags
 *              *BCM_L3_IP6 - If set, determines that an IPv6
 *               entry is being defined;
 *              *BCM_L3_REPLACE - If set, an existing entry will
 *               be updated;
 *      * info->l3a_intf - L3 interface used as destination of
 *        the entry (FEC);
 *      * info->encap_id - encapsulation ID (out-LIF);
 *      * info->l3a_port_tgid - when using host format NO-FEC this field should include
 *        gport information, when this field is assigned info->l3a_intf should hold outrif and not fec.
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative in case of error.
 * \see
 *  EM Tables in which the entry may be added:
 *   * IPV6_UNICAST_PRIVATE_HOST
 *   * IPV4_UNICAST_PRIVATE_HOST
 *   * KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *   * 
 */
int
bcm_dnx_l3_host_add(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 entry_handle_id;
    uint32 l3_intf;
    uint32 encap_id, eei_val, dest_val;
    uint32 result_type;
    uint32 dbal_table, destination;
    int system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Jericho mode beginning
     * {
     */
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    /*
     * Jericho mode end
     * }
     */
    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_add_verify(unit, info));

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    encap_id = BCM_L3_ITF_VAL_GET(info->encap_id);

    /*
     * Determine the correct table in which the host entry needs to be added to.
     * Allocate handle to that table that will be used for adding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, info, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry that will be created.
     * Valid fields for it are control flags, VRF value, IP address and its mask.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, info, &entry_handle_id));

    /*
     * Build destination result based on the defined fields in the bcm_l3_host_t.
     * Payload types that are currently supported:
     *          * FEC
     *          * FEC + EEP(outLIF)
     *          * {System-Port, OutRIF, ARP (No-FEC)}
     *          * FEC + ETH-RIF + ARP
     */
    /*
     * if !BCM_L3_ITF_TYPE_IS_FEC and encap_id != 0 then the host format is no-fec.
     * l3a_port_tgid must be assigned and not equal to zero.
     * in this case we assign system-port,outrif,arp to the host
     */
    if (!BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) && info->encap_id != 0)
    {
        if (info->l3a_port_tgid == BCM_GPORT_TYPE_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "host points directly to outRIF, l3a_port_tgid must be valid\n");
        }

        if (L3_TABLE_IS_KBP(dbal_table))
        {
            if ((BCM_L3_ITF_TYPE_IS_LIF(info->l3a_intf) && BCM_L3_ITF_TYPE_IS_LIF(info->encap_id))
                || (BCM_L3_ITF_TYPE_IS_RIF(info->l3a_intf)
                    && (info->l3a_intf != 0) && BCM_L3_ITF_TYPE_IS_LIF(info->encap_id)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP does not support more than one outLIF in the result\n");
            }
            else
            {
                if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "KBP does not support statistics profile in the result\n");
                }
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT;
                }
                else
                {
                    result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT;
                }
            }
        }
        else
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
            {
                result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                    DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                    DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
            }
            else
            {
                result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                    DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                    DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        if (L3_TABLE_IS_KBP(dbal_table))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, encap_id);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, l3_intf);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, encap_id);
        }
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, info->l3a_port_tgid, &destination));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, destination);
    }
    else
    {
        if (info->encap_id == 0)
        {

            if (L3_TABLE_IS_KBP(dbal_table))
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT;
                }
            }
            else
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST;
                }
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, l3_intf);
            if (L3_TABLE_IS_KBP(dbal_table))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, 0);
            }
        }
        else
        {
            /*
             * If encap_id != 0 and the FEC is valid, then the destination of the entry will resolve to
             * a FEC and an out-LIF.
             */
            if (L3_TABLE_IS_KBP(dbal_table))
            {
                if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "KBP does not support outLIF with statistics profile in the result\n");
                }
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT;
                }
                else
                {
                    result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT;
                }
            }
            else
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    /*
                     * Jericho mode beginning
                     * {
                     */
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                    /*
                     * Jericho mode end
                     * }
                     */
                }
                else
                {
                    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    /*
                     * Jericho mode beginning
                     * {
                     */
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                    /*
                     * Jericho mode end
                     * }
                     */
                }
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            /*
             * Jericho mode beginning
             * {
             */
            if ((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI) ||
                (result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI))
            {
                if (BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)
                    && _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED)
                    && BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
                {
                    uint32 field_in_struct_encoded_val;

                    field_in_struct_encoded_val = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(info->encap_id);;
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL,
                                     &field_in_struct_encoded_val, &eei_val));

                    field_in_struct_encoded_val =
                        BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(info->encap_id);
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID,
                                     &field_in_struct_encoded_val, &eei_val));

                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, &encap_id, &eei_val));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, &encap_id, &eei_val));

                }
                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, &l3_intf, &dest_val));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_val);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, dest_val);
            }
            /*
             * Jericho mode end
             * }
             */
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, encap_id);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, l3_intf);
                if (L3_TABLE_IS_KBP(dbal_table))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, 0);
                }
            }
        }
    }

    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, info->stat_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                     STAT_PP_PROFILE_ID_GET(info->stat_pp_profile));
    }

    /*
     * Create the entry if it doesn't exist, update it if it does.
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Adds/updates an IP route entry.
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      L3 route information (key and payload) to be
 *      added/updated. \n
 *      info.l3a_vrf - Vritual Router Instance \n
 *      info.l3a_flags - Route entry control flags defined as
 *      BCM_L3_*** \n
 *      info.l3a_intf - L3 interface associated with the route\n
 *      info.l3a_subnet - IPv4 subnet address\n
 *      info.l3a_ip_mask - IPv4 subnet mask \n
 *      info.l3a_ip6_net - IPv6 subnet address\n
 *      info.l3a_ip6_mask - IPv6 subnet mask\n
 * \return
 *   \retval Zero in case of NO ERROR.
 *   \retval Negative is error was detected. See \ref
 *           shr_error_e
 * \see
 *   * bcm_l3_route_t
 *
 *   Tables to which the entry may be added are:
 *      * IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      * IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      * IPV6_UNICAST_PUBLIC_LPM_FORWARD
 *      * IPV4_UNICAST_PUBLIC_LPM_FORWARD
 */
int
bcm_dnx_l3_route_add(
    int unit,
    bcm_l3_route_t * info)
{
    dbal_fields_e dest_type;
    uint32 entry_handle_id;
    uint32 l3_intf;
    uint32 dbal_table;
    uint8 is_kbp_default;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_add_verify(unit, info));

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    /*
     * Determine which table the entry belongs to. It will be returned in dbal_table variable.
     * A handle will be allocated to that table using the value of dbal_table variable.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key for the new entry addition.
     * Set the value of the entry to be l3_intf.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_to_key(unit, info, dbal_table, &entry_handle_id, &dest_type, &is_kbp_default));
    /*
     * KBP requires multiple result types to be used
     */
    if (L3_TABLE_IS_KBP(dbal_table))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(dbal_table));
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, is_kbp_default);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, dest_type, INST_SINGLE, l3_intf);

    /*
     * Create the entry if it doesn't exist, update it if it does.
     */
    if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_REPLACE))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find an IP route entry
 * \param [in] unit - The unit number.
 * \param [in,out] info -
 *      The l3 route structure describing the entry.\n
 *      As input -
 *          info.l3a_vrf   -          \n
 *            Virtual Router Instance \n
 *          info.l3a_flags -                                   \n
 *            Control flags for route entries -                \n
 *              BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_MULTIPATH)  \n
 *          info.l3a_flags2 -                          \n
 *            Indicative Control flags -               \n
 *              BCM_L3_FLAGS2_RAW_ENTRY -              \n
 *                If set then required output format is 'raw'. Output (on 'info->l3a_intf')
 *                will NOT be encoded (as per BCM_L3_ITF_SET, BCM_L3_ITF_TYPE_FEC, ...)
 *          info.l3a_subnet -         \n
 *            IPv4 subnet address     \n
 *          info.l3a_ip_mask -        \n
 *            IPv4 subnet mask        \n
 *          info.l3a_ip6_net -        \n
 *            IPv6 subnet address     \n
 *          info.l3a_ip6_mask -       \n
 *            IPv6 subnet mask        \n
 *      As output -
 *          info.l3a_intf -                            \n
 *            Destination interface is returned as FEC \n
 *          info.l3a_flags2 -                          \n
 *            Indicative Control flags -               \n
 *              BCM_L3_FLAGS2_RAW_ENTRY -              \n
 *                If set at input then output value on 'info->l3a_intf' as 'raw'
 *                (without encoding) and flag remains as is.
 *                Otherwise, flag is set if procedure managed to identify entry
 *                as 'raw'. Otherwise, output value on 'info->l3a_intf'is encoded
 *                (as per BCM_L3_ITF_SET, BCM_L3_ITF_TYPE_FEC, ...) and flag is
 *                not set.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error
 * \remark
 *   * receive information from LPM tables:
 *      IPV4_UNICAST_PUBLIC_LPM_FORWARD
 *      IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      IPV6_UNICAST_PUBLIC_LPM_FORWARD
 *      IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_route_get(
    int unit,
    bcm_l3_route_t * info)
{
    dbal_fields_e dest_type;
    uint32 entry_handle_id;
    uint32 fec_index, result_value, destination_val;
    uint32 dbal_table;
    dbal_fields_e dbal_sub_field;
    uint8 is_kbp_default;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_verify(unit, info));

    /*
     * Make sure that the output parameter is 0 before receiving data.
     * In case of an error receiving the result value, output value will be 0.
     */
    BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, 0);
    /*
     * Select the table to which the entry belongs and allocate handle to it.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_l3_route_to_key(unit, info, dbal_table, &entry_handle_id, &dest_type, &is_kbp_default));

    if (L3_TABLE_IS_KBP(dbal_table))
    {
        /** KBP requires multiple result types to be used */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(dbal_table));

        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination_val);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination_val, &dbal_sub_field, &result_value));

    }
    else
    {
        /** Handling of non-KBP tables */
        /*
         *  Setting pointer to 'result_value' variable to receive the field value from the table.
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &destination_val);
        /*
         * Preforming the action after this call the pointers that we set in field32_request()
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Get the used sub-field and its value.
         * The sub-field ID value is assigned to dbal_sub_field variable and its decoded
         * value is assigned to result_value.
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_KAPS_RESULT, destination_val, &dbal_sub_field, &result_value));
    }

    if ((info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) != 0)
    {
        /*
         * If 'raw' flag was set at input then return extracted value as is, no encoding.
         */
        info->l3a_intf = destination_val;
    }
    else
    {
        /*
         * If 'raw' flag was NOT set at input then check:
         *   If setting is illegal then consider this as a 'raw' entry -
         *     return extracted value without encoding and set the 'raw' flag.
         *   Otherwise, encode extracted entry and return encoded value.
         */
        if (dest_type != dbal_sub_field)
        {
            info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
            fec_index = destination_val;
        }
        else
        {
            switch (dbal_sub_field)
            {
                case DBAL_FIELD_FEC:
                {
                    uint32 max_fec_id;

                    SHR_IF_ERR_EXIT(mdb_db_infos.max_fec_id_value.get(unit, &max_fec_id));
                    /*
                     * If encoding is 'FEC' (full 20 bits value) and value is higher
                     * than maximal for FEC then assume it is 'raw'.
                     * So, set 'raw' flag to one.
                     * Otherwise, assume it is standard FEC and, so, set 'raw' flag to zero.
                     */
                    if (result_value > max_fec_id)
                    {
                        info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
                        fec_index = destination_val;
                    }
                    else
                    {
                        fec_index = result_value;
                    }
                    break;
                }
                case DBAL_FIELD_FEC_DEFAULT:
                {
                    if (!L3_TABLE_IS_KBP(dbal_table))
                    {
                        /*
                         * If encoding is 'FEC_DEFAULT' (17 bits value) and value is higher
                         * than maximal for FEC_DEFAULT then assume it is 'raw'.
                         * So, set 'raw' flag to one.
                         * Otherwise, assume it is standard FEC_DEFAULT and, so, set 'raw' flag to zero.
                         */
                        if (result_value > dnx_data_l3.fec.max_default_fec_get(unit))
                        {
                            info->l3a_flags2 |= BCM_L3_FLAGS2_RAW_ENTRY;
                            fec_index = destination_val;
                        }
                        else
                        {
                            fec_index = result_value;
                        }
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of L3 route get.\n",
                                     dbal_sub_field);
                    }
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal sub-field with ID %d found as result of L3 route get.\n",
                                 dbal_sub_field);
                    break;
                }
            }
        }
        if ((info->l3a_flags2 & BCM_L3_FLAGS2_RAW_ENTRY) != 0)
        {
            /*
             * If 'raw' flag was set at input then return extracted value as is, no encoding.
             */
            info->l3a_intf = destination_val;
        }
        else
        {
            /*
             * Set the value of the l3a_intf field using fec_index.
             */
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Find an IP host entry
 * \param [in] unit - The unit number.
 * \param [in,out] info -
 *      The l3 host structure describing the entry.\n
 *      As input:
 *          info.l3a_vrf - Virtual Router Instance \n
 *          info.l3a_flags - Control flags for route entries -
 *          BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_HIT_CLEAR)\n
 *          info.l3a_ip_addr - IPv4 subnet address \n
 *          info.l3a_ip6_addr - IPv6 subnet address \n
 *      As output:
 *          info.l3a_intf - Destination interface is
 *          returned as FEC \n
 *          info.encap_id - Destination LIF returned as out-LIF if
 *          present \n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *   * receive information from LPM table:
 *      IPV4_UNICAST_PRIVATE_HOST
 *      IPV6_UNICAST_PRIVATE_HOST
 *      KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      
 * \see
 *   * bcm_l3_host_t fields \n
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_find(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 entry_handle_id;
    uint32 fec_index;
    uint32 arp_index;
    uint32 eth_rif;
    uint32 result_type;
    uint32 dbal_table;
    uint32 stat_pp_profile;
    uint32 destination;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_verify(unit, info));

    /*
     * Nullify the stucture members before beginning.
     * In case of an error receiving the result value, output value will be 0.
     */
    fec_index = 0;
    arp_index = 0;
    eth_rif = 0;
    BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
    BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, arp_index);

    /*
     * Determine the correct table in which the host entry needs to be looked for.
     * Allocate handle to that table that will be used for finding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, info, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build host entry key based on flags, VRF value and IP address.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, info, &entry_handle_id));

    /*
     * Getting all destination fields of the entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type);

    /** Copy the specific field value to a given pointer */
    if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST
        || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT
        || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST
        || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT
        || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT)
    {
        /*
         * If the result type of the entry is FWD_DEST then we can receive only the FEC from the value field.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, &fec_index));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
        info->encap_id = 0;
    }
    /*
     * Jericho mode beginning
     * {
     */
    else if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI)
    {
        uint32 dbal_sub_field, result_value, eei_val;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination, &dbal_sub_field, &result_value));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, result_value);

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &eei_val));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, eei_val, &dbal_sub_field, &result_value));

        if (dbal_sub_field == DBAL_FIELD_EEI_ENCAPSULATION_POINTER)
        {
            if (result_value)
            {
                BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, result_value);
            }
        }
        else
        {
            uint32 mpls_label, push_cmd, encap_id;
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &mpls_label, &eei_val));

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &push_cmd, &eei_val));
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(encap_id, mpls_label, push_cmd);
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, encap_id);
        }
    }
    /*
     * Jericho mode end
     * }
     */
    else if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
             || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT)
    {
        /*
         * If the result type of the entry is FWD_DEST_OUTLIF then the fields that we can receive are FEC and OUT_LIF.
         */
        uint32 dbal_sub_field, result_value;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));
        printf("destination 0x%08X\n", destination);
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination, &dbal_sub_field, &result_value));
        printf("dbal_sub_field %d\n", dbal_sub_field);
        if (dbal_sub_field == DBAL_FIELD_FEC)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, &fec_index));
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
        }
        else if (dbal_sub_field == DBAL_FIELD_PORT_ID)
        {
            info->l3a_intf = 0;
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, destination, &info->l3a_port_tgid));
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &arp_index));
        BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, arp_index);
    }
    else if (result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT
             || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF
             || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT)
    {
        /*
         * If the result type of the entry is FWD_DEST_DOUBLE_OUTLIF
         * then the fields that we can receive are FEC, ETH-RIF and OUT_LIF.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination));
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, destination, &info->l3a_port_tgid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &eth_rif));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, &arp_index));
        BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, arp_index);
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_RIF, eth_rif);
    }

    if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                            entry_handle_id,
                                                            DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));

        /*
         * Get statistic info only if stat profile is valid
         */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                &info->stat_id));

            STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile, bcmStatCounterInterfaceIngressReceivePp);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete an IP host entry
 * \param [in] unit - The unit number.
 * \param [in] ip_addr -
 *      The l3 host structure describing the entry.\n
 *      ip_addr.l3a_vrf - Virtual Router Instance \n
 *      ip_addr.l3a_flags - Control flags for route entries
 *      defined as BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_IPMC,
 *      BCM_L3_MULTIPATH, etc.)\n
 *      ip_addr.l3a_ip_addr - IPv4 subnet address\n
 *      ip_addr.l3a_ip6_addr - IPv6 subnet address\n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
int
bcm_dnx_l3_host_delete(
    int unit,
    bcm_l3_host_t * ip_addr)
{
    uint32 entry_handle_id;
    uint32 dbal_table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_verify(unit, ip_addr));

    /*
     * Determine the correct table in which the host entry needs to be added to.
     * Allocate handle to that table that will be used for adding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, ip_addr, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry that will be created.
     * Valid fields for it are control flags, VRF value, IP address and its mask.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_to_key(unit, ip_addr, &entry_handle_id));

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete an IP route entry
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure describing the entry.\n
 *      info.l3a_vrf - Virtual Router Instance \n
 *      info.l3a_flags - Control flags for route entries -
 *      BCM_L3_*** \n
 *      info.l3a_subnet - IPv4 subnet address \n
 *      info.l3a_ip_mask - IPv4 subnet mask \n
 *      info.l3a_ip6_net - IPv6 subnet address \n
 *      info.l3a_ip6_mask - IPv6 subnet mask \n
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
int
bcm_dnx_l3_route_delete(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 dest_type;
    uint8 is_kbp_default;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_verify(unit, info));

    /*
     * Select the table to which the entry belongs and allocate handle to it.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_dbal_table_select(unit, info, &dbal_table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));

    /*
     * Build the key of the entry based on the values of the input fields.
     * Valid fields are l3a_vrf, l3a_subnet, l3a_ip_mask for the IPv4 entries and
     * for IPv6 entries l3a_vrf, l3a_ip6_net and l3a_ip6_mask.
     */
    SHR_IF_ERR_EXIT(dnx_l3_route_to_key(unit, info, dbal_table, &entry_handle_id, &dest_type, &is_kbp_default));

    /** clearing the entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Clear the L3 IP route table
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure.\n
 *      info.l3a_flags - Control flags for route entries -
 *          * BCM_L3_IP6 - If flag is provided, all IPv6 route
 *            entries will be deleted. If it is not given, IPv4
 *            route table will be cleared
 *      info.l3a_vrf - Vritual router instance
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_route_delete_all(
    int unit,
    bcm_l3_route_t * info)
{
    uint32 dbal_table[8] = {
        DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD
    };
    uint32 table;
    uint32 nof_tables = 8;
    uint32 kbp_mngr_status;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;
    uint8 kbp_ipv4_public_enabled = FALSE;
    uint8 kbp_ipv6_public_enabled = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    SHR_IF_ERR_EXIT(kbp_mngr_ipv4_public_enabled_get(unit, &kbp_ipv4_public_enabled));
    SHR_IF_ERR_EXIT(kbp_mngr_ipv6_public_enabled_get(unit, &kbp_ipv6_public_enabled));

    /*
     * If the pointer to bcm_l3_route_t structure is empty, all route entries are going to be cleared.
     * Otherwise, public and private table will be cleared based on the BCM_L3_IP6 flag -
     * if it is set, then public and private IPv6 tables will be cleared;
     * if it is not set, then public and private IPv4 tables will be cleared.
     */
    /** clear tables */
    for (table = 0; table < nof_tables; table++)
    {
        if (info != NULL)
        {
            if ((_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                 && ((dbal_table[table] == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD
                      || dbal_table[table] == DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD
                      || dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
                      || dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD)
                     || ((dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
                         && !use_kbp_ipv6)
                     || ((dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD)
                         && (!use_kbp_ipv6 || !kbp_ipv6_public_enabled))))
                || (!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                    && ((dbal_table[table] == DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD
                         || dbal_table[table] == DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD
                         || dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD
                         || dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD)
                        || ((dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
                            && !use_kbp_ipv4)
                        || ((dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD)
                            && (!use_kbp_ipv4 || !kbp_ipv4_public_enabled)))))
            {
                continue;
            }
        }
        else
        {
            /** Skip KBP tables accordingly */
            if (((dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
                 && !use_kbp_ipv6)
                || ((dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD)
                    && (!use_kbp_ipv6 || !kbp_ipv6_public_enabled))
                || ((dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
                    && !use_kbp_ipv4)
                || ((dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD)
                    && (!use_kbp_ipv4 || !kbp_ipv4_public_enabled)))
            {
                continue;
            }
        }
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table[table]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Delete all L3 IP host entries in DNX devices.
 * \param [in] unit - The unit number.
 * \param [in] info -
 *      The l3 route structure bearing the control flags
 *      info.l3a_flags - Control flags for route entries -
 *          * BCM_L3_IP6 - If present, all IPv6 host table
 *            will be cleared. If it is not present, all IPv4
 *            host entries will be deleted.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *   * clear all information from LPM tables:
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_delete_all(
    int unit,
    bcm_l3_host_t * info)
{
    uint32 dbal_table[4] = {
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST,
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD
    };
    uint32 table;
    uint32 nof_tables = 4;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * If the pointer to bcm_l3_host_t structure is empty, all host entries are going to be cleared.
     * Otherwise, tables will be cleared based on the BCM_L3_IP6 flag -
     * if it is set, then IPv6 tables will be cleared;
     * if it is not set, then IPv4 tables will be cleared.
     */
    /** clear tables */
    for (table = 0; table < nof_tables; table++)
    {
        if (info != NULL)
        {
            if ((_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                 &&
                 ((dbal_table[table] == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
                   || dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
                  || (dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv6)))
                || (!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
                    &&
                    ((dbal_table[table] == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
                      || dbal_table[table] == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
                     || (dbal_table[table] == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD && !use_kbp_ipv4))))
            {
                continue;
            }
        }
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_table[table]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 IP host entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_IP6 - if provided, it indicates that the
 *            IPv6 host table will be traversed, otherwise - the
 *            IPv4 host entries will be gone over.
 * \param [in] start - The index of the first valid entry that
 *        will be processed.
 * \param [in] end - The index of the last valid entry that will
 *        be processed
 * \param [in] cb - the name of the callback function that is
 *        going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST
 *      DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 *      DBAL_TABLE_
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_host_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_host_traverse_cb cb,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 dbal_table;
    uint32 index = 0;
    uint32 *field_value;
    uint8 *field_value_eight_bits;
    int is_end;
    bcm_l3_host_t dbal_entry;
    uint32 result_type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_value = NULL;
    field_value_eight_bits = NULL;
    /*
     * Allocate maximum size of array that may be used.
     */
    SHR_ALLOC(field_value, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint32),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(field_value_eight_bits, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint8),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_traverse_verify(unit, flags));
    /*
     * Determine the correct table from which all host entries will be traversed based on the BCM_L3_IP6 flag.
     * If the BCM_IPMC_IP6 flag is not set, then IPv4 host table will be traversed.
     */
    dbal_entry.l3a_flags = flags;
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, &dbal_entry, &dbal_table));

    /*
     * Allocate handle to the table of the iteration and initialise an iterator entity.
     * The iterator is in mode ALL, which means that it will consider all entries regardless
     * of them being default entries or not.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        bcm_l3_host_t_init(&dbal_entry);

        /*
         * Receive key and value fields of the entry.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
        dbal_entry.l3a_vrf = field_value[0];
        /*
         * If the BCM_L3_IP6 flag is set, then IPv6 address will be extracted from the key.
         * Otherwise the IPv4 address will be taken.
         */
        if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                            (unit, entry_handle_id, DBAL_FIELD_IPV6, field_value_eight_bits));
            sal_memcpy(dbal_entry.l3a_ip6_addr, field_value_eight_bits, sizeof(bcm_ip6_t));
            dbal_entry.l3a_flags = BCM_L3_IP6;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IPV4, field_value));
            dbal_entry.l3a_ip_addr = field_value[0];
        }

        /*
         * Receive the result type of the entry and based on it will assign result values.
         * If the result type is HOST_DEST_OUTLIF then both the FEC and the out-LIF are used as result.
         * If the result type is FWD_DEST then only FEC value will be taken.
         * If the result type is neither one of those, then an error will be returned.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &result_type));
        if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
            || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT
            || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
            || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT
            || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT
            || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT
            || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_OUTLIF_W_DEFAULT
            || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT)
        {
            if (result_type != DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT
                && result_type != DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_STAT_W_DEFAULT)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF,
                                                                        INST_SINGLE, field_value));
                BCM_L3_ITF_SET(dbal_entry.encap_id, BCM_L3_ITF_TYPE_LIF, field_value[0]);
            }
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION,
                                                                    INST_SINGLE, field_value));
            BCM_L3_ITF_SET(dbal_entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value[0]);
        }

        /*
         * Jericho mode beginning
         * {
         */
        else if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI
                 || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI)
        {
            uint32 sub_field_id = 0, sub_field_val = 0;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_EEI,
                                                                    INST_SINGLE, field_value));
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_EEI, field_value, &sub_field_id, &sub_field_val));

            BCM_L3_ITF_SET(dbal_entry.encap_id, BCM_L3_ITF_TYPE_LIF, sub_field_val);
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION,
                                                                    INST_SINGLE, field_value));
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_DESTINATION, field_value, &sub_field_id, &sub_field_val));

            BCM_L3_ITF_SET(dbal_entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, sub_field_val);

        }
        /*
         * Jericho mode end
         * }
         */
        else if (result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST
                 || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT
                 || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST
                 || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_STAT
                 || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT
                 || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_FWD_DEST_W_DEFAULT)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION,
                                                                    INST_SINGLE, field_value));
            BCM_L3_ITF_SET(dbal_entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value[0]);
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type of host entry with index %u is not known.", index);
        }

        /** In case of an entry with stats - retreive stats configuration values */
        if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
        {
            uint32 stat_pp_profile;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                entry_handle_id,
                                                                DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                                                &stat_pp_profile));

            /*
             * Get statistic info only if stat profile is valid
             */
            if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
            {

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit,
                                                                        entry_handle_id,
                                                                        DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                        &dbal_entry.stat_id));

                STAT_PP_ENGINE_PROFILE_SET(dbal_entry.stat_pp_profile, stat_pp_profile,
                                           bcmStatCounterInterfaceIngressReceivePp);
            }
        }

        /*
         * If user provided a name of the callback function, it will be invoked with sending the dbal_entry structure
         * of the entry that was found.
         */
        if (cb != NULL)
        {
            /*
             * Invoke callback function
             */
            SHR_IF_ERR_EXIT((*cb) (unit, index, &dbal_entry, user_data));
        }
        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        index++;
    }

exit:
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Traverse all L3 IP route entries in DNX devices and call a
 *  function with the data for each entry found.
 * \param [in] unit - The unit number.
 * \param [in] flags - control flags that indicate choice of
 *        table to be traversed:
 *          * BCM_L3_IP6 - if provided, it indicates that the
 *            IPv6 route tables will be traversed, otherwise -
 *            the IPv4 route entries will be gone over.
 * \param [in] start - The index of the first valid entry that
 *        will be processed.
 * \param [in] end - The index of the last valid entry that will
 *        be processed
 * \param [in] trav_fn - the name of the callback function that
 *        is going to be called for each valid entry
 * \param [in] user_data - data that is passed to the callback
 *        function
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \remark
 *      DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD
 * \see
 *   * BCM L3 Flags \n
 */
int
bcm_dnx_l3_route_traverse(
    int unit,
    uint32 flags,
    uint32 start,
    uint32 end,
    bcm_l3_route_traverse_cb trav_fn,
    void *user_data)
{
    uint32 entry_handle_id;
    uint32 kbp_mngr_status;
    uint32 dbal_tables[MAX_NOF_L3_ROUTE_TABLES];
    uint32 nof_tables;
    uint32 table;
    uint32 index;
    uint32 *field_value;
    uint32 result;
    int is_end;
    bcm_l3_route_t dbal_entry;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    field_value = NULL;
    nof_tables = 0;
    /*
     * Allocate maximum size of array that may be used.
     */
    SHR_ALLOC(field_value, (sizeof(bcm_ip6_t) / sizeof(uint8)) * sizeof(uint32),
              "Array for reading table entry key or value", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_traverse_verify(unit, flags));

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    /*
     * Determine the correct table from which all host entries will be traversed based on the BCM_L3_IP6 flag.
     * If the BCM_IPMC_IP6 flag is not set, then IPv4 host table will be traversed.
     */
    if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
    {
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PUBLIC_LPM_FORWARD;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            uint8 kbp_public_enabled = FALSE;
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            SHR_IF_ERR_EXIT(kbp_mngr_ipv6_public_enabled_get(unit, &kbp_public_enabled));

            if (kbp_public_enabled == TRUE)
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PUBLIC_LPM_FORWARD;
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
        }
    }
    else
    {
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PUBLIC_LPM_FORWARD;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            uint8 kbp_public_enabled = FALSE;
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            SHR_IF_ERR_EXIT(kbp_mngr_ipv4_public_enabled_get(unit, &kbp_public_enabled));

            if (kbp_public_enabled == TRUE)
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PUBLIC_LPM_FORWARD;
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        }
    }
    /*
     * Allocate DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[0], &entry_handle_id));
    /*
     * Iterate over all tables of the selected IP version.
     */
    for (table = 0; table < nof_tables; table++)
    {
        index = 0;
        /*
         * Clear handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         *
         * If working on the first table, need to use DBAL_HANDLE_ALLOC.
         * Otherwise we change to which table the handle belongs using DBAL_HANDLE_CLEAR.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /*
         * Receive first entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        /*
         * is_end variable will be set when the last entry is traversed and iterator_get_next is called again.
         */
        while (!is_end)
        {
            bcm_l3_route_t_init(&dbal_entry);

            result = L3_TABLE_IS_KBP(dbal_tables[table]) ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT;
            /*
             * Public tables do not have VRF field (VRF=0)
             */
            if (L3_TABLE_IS_PUBLIC(dbal_tables[table]))
            {
                dbal_entry.l3a_vrf = 0;
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_VRF, field_value));
                dbal_entry.l3a_vrf = field_value[0];
            }
            /*
             * If the BCM_L3_IP6 flag is set, then IPv6 address subnet value will be extracted.
             * Otherwise, the IPv4 subnet value will be extracted.
             */
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_IP6))
            {
                dbal_entry.l3a_flags = BCM_L3_IP6;
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_masked_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV6, dbal_entry.l3a_ip6_net,
                                 dbal_entry.l3a_ip6_mask));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV4, &dbal_entry.l3a_subnet,
                                 &dbal_entry.l3a_ip_mask));
            }

            /*
             * If the current table is a KAPS one, then the result needs to be received from either field FEC or field
             * FEC_DEFAULT
             */
            if (result == DBAL_FIELD_KAPS_RESULT)
            {
                /*
                 * Verify whether the entry is default or not based on the l3a_ip6_mask(IPv6) or l3a_ip_mask(IPv4).
                 * If the mask indicates a default entry, the result will be received from the FEC_DEFAULT field.
                 */
                if (_SHR_IS_FLAG_SET(dbal_entry.l3a_flags, BCM_L3_IP6))
                {
                    result =
                        L3_FRWRD_TABLES_IS_IPV6_DEFAULT_ENTRY(dbal_entry.l3a_ip6_mask) ? DBAL_FIELD_FEC_DEFAULT :
                        DBAL_FIELD_FEC;
                }
                else
                {
                    result =
                        L3_FRWRD_TABLES_IS_IPV4_DEFAULT_ENTRY(dbal_entry.l3a_ip_mask) ? DBAL_FIELD_FEC_DEFAULT :
                        DBAL_FIELD_FEC;
                }
            }
            /** Receive result of entry */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, result, INST_SINGLE, field_value));
            BCM_L3_ITF_SET(dbal_entry.l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value[0]);

            if (trav_fn != NULL)
            {
                /*
                 * Invoke callback function
                 */
                SHR_IF_ERR_EXIT((*trav_fn) (unit, index, &dbal_entry, user_data));
            }
            /*
             * Receive next entry in table.
             */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            index++;
        }
    }

exit:
    SHR_FREE(field_value);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
