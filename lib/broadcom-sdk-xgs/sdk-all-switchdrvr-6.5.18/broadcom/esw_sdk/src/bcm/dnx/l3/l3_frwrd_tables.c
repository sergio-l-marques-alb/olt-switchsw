/** \file l3_frwrd_tables.c
 *
 * L3 forwarding tables procedures for DNX
 */
/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnx/mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_internal_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
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
#define MAX_NOF_L3_ROUTE_TABLES 6

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
#define L3_SUPPORTED_BCM_L3_FLAGS (BCM_L3_IP6 | BCM_L3_REPLACE | BCM_L3_HOST_LOCAL | BCM_L3_ENCAP_SPACE_OPTIMIZED | BCM_L3_HIT_CLEAR | BCM_L3_HIT)
/** Define the list of supported BCM_L3_FLAGS2_* flags for L3 APIs. */
#define L3_FORWARD_SUPPORTED_BCM_L3_FLAGS2 (BCM_L3_FLAGS2_RAW_ENTRY | BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY | BCM_L3_FLAGS2_SCALE_ROUTE | BCM_L3_FLAGS2_NO_PAYLOAD)
/*
 * Indicates whether the table is KBP
 */

#define L3_TABLE_IS_KBP(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is RPF
 */

#define L3_TABLE_IS_RPF(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is KBP IPV4
 */

#define L3_TABLE_IS_KBP_IPV4(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is KBP IPV6
 */

#define L3_TABLE_IS_KBP_IPV6(table) ( \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) || \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF))
/*
 * Indicates whether the table is public
 */

#define L3_TABLE_IS_PUBLIC(table) ( \
        (table == DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2)     || \
        (table == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2))

 /*
  * Return the appropriate result type according to the KBP table
  */

#define L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(table) ( \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) ? DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT : \
        (table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF) ? DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF_KBP_DEST_W_DEFAULT : -1)

/*
 * }
 */

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
    int kaps_public_capacity = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (((_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
          && !(dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))))
        || (!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
            && !(dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))))
    {
        /*
         * Only get the KAPS2 capacity if KBP is disabled
         * and vrf == 0 or SCALE flag set, otherwise KAPS1 DB is used
         */
        if ((info->l3a_vrf == 0) || (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE))
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, DBAL_PHYSICAL_TABLE_KAPS_2, &kaps_public_capacity));
        }
    }

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
            {
                if (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE)
                {
                    *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else if (kbp_mngr_split_rpf_in_use(unit))
                {
                    if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
                    {
                        *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
                    }
                    else
                    {
                        *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
                    }
                }
                else
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
                }
            }
        }
        else
        {
            if (kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                if ((info->l3a_vrf == 0) || (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE))
                {
                    *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
                }
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
            {
                if (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE)
                {
                    *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else if (kbp_mngr_split_rpf_in_use(unit))
                {
                    if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
                    {
                        *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
                    }
                    else
                    {
                        *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
                    }
                }
                else
                {
                    *dbal_table = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
                }
            }
        }
        else
        {
            if (kaps_public_capacity == 0)
            {
                *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            }
            else
            {
                if (info->l3a_vrf == 0 || (info->l3a_flags2 & BCM_L3_FLAGS2_SCALE_ROUTE))
                {
                    *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;
                }
                else
                {
                    *dbal_table = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
                }
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
 * \param [in] dbal_table - The table that is selected for host entry.
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
static shr_error_e
dnx_l3_host_add_verify(
    int unit,
    bcm_l3_host_t * info,
    uint32 dbal_table)
{
    uint32 fec_index;
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(info, _SHR_E_PARAM, "l3_host");
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    fec_index = BCM_L3_ITF_VAL_GET(info->l3a_intf);

    /*
     * Verify common fields of the host entry
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_verify(unit, info));

    /*
     * Check that the FEC value is in range.
     * The NOF of FECs is not configurable.
     */
    if ((BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf) || info->l3a_port_tgid == BCM_GPORT_TYPE_NONE)
        && fec_index >= dnx_data_l3.fec.nof_fecs_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, should be less than %d.", fec_index,
                     dnx_data_l3.fec.nof_fecs_get(unit));
    }

    /*
     * Check encap_id is set correctly.
     */
    if ((info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED))
    {
        /** In this case, encap_id must be EEI.*/
        if (!BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "encap_id(0x%08X) must be encoded as EEI when BCM_L3_ENCAP_SPACE_OPTIMIZED is set.",
                         info->encap_id);
        }
        /** In JR2, EEI must be for push-command and the push-profile must be 0.*/
        if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
        {
            if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) != BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
            {
                /** If it's an outLIF, do not encode it as EEI in JR2*/
                SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id(0x%08X) encoded as an EEI must be used for mpls port.",
                             info->encap_id);
            }
            else if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(info->encap_id) != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id(0x%08X) encoded as an EEI can not include non-zero push profile.",
                             info->encap_id);
            }
        }
    }
    else
    {
        /** In this case, encap_id must include a outlif. Verify that the out-LIF value is in range*/
        if (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) > dnx_data_lif.global_lif.nof_global_out_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "OUT_LIF %d is out of range, should be lower than %d.", info->encap_id,
                         dnx_data_lif.global_lif.nof_global_out_lifs_get(unit));
        }
    }

    /*
     * Check cases when l3a_port_tgid is defined.
     */
    if ((info->l3a_port_tgid != BCM_GPORT_TYPE_NONE)
        && (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) != 0 || BCM_L3_ITF_VAL_GET(info->l3a_intf) != 0))
    {
        /** Verify that l3a_intf is not zero while encap_id is set. */
        if (BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id) != 0 && BCM_L3_ITF_VAL_GET(info->l3a_intf) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot have encap_id defined while l3a_intf is not set.");
        }
        /** Verify that l3a_intf is not with type FEC while l3a_port_tgid is set. */
        if (BCM_L3_ITF_TYPE_IS_FEC(info->l3a_intf))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Multiple destination types indicated - l3a_intf is of type FEC but l3a_port_tgid is defined");
        }

        /** In case of KBP, only one outlif is possible and stat is not supported.*/
        if (L3_TABLE_IS_KBP(dbal_table))
        {
            if (info->encap_id != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP support one outlif by l3a_intf only, encap_id (%d) must be 0.\n",
                             info->encap_id);
            }
            if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "KBP does not support statistics profile in the result\n");
            }
        }
        else
        {
            if (info->l3a_flags & BCM_L3_ENCAP_SPACE_OPTIMIZED)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "HOST does not support outLIF and EEI in the result\n");
            }
        }
    }
    else if (info->l3a_port_tgid == BCM_GPORT_TYPE_NONE)
    {
        /*
         * In case l3a_port_tgid is not set:
         * l3a_intf must be FEC, encap_id can be outlif, EEI or NULL.
         * For KBP, stat is possible only when encap_id is not set.
         */
        if (info->encap_id != 0)
        {
            if (L3_TABLE_IS_KBP(dbal_table))
            {
                if (info->stat_pp_profile != STAT_PP_PROFILE_INVALID)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "If destination is FEC, and encap_id is set, KBP can not support stat in the result.\n");
                }
            }
            else
            {
                if ((system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) &&
                    (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) &&
                    _SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "If destination is FEC, EEI can not be used together with stat!\n");
                }
            }
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
    if (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RAW_ENTRY))
    {
        /*
         * Enter for Non-raw entries.
         * Raw entries verification will be done after selecting the dbal table, because KAPS and KBP have different
         * available payload sizes - KAPS_RESULT (20 bits) DESTINATION (21 bits).
         */
        if (fec_index >= dnx_data_l3.fec.nof_fecs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "FEC index %d is out of range, should be less than %d.", fec_index,
                         dnx_data_l3.fec.nof_fecs_get(unit));
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
 *  Verify the KBP route entry add, get and delete input.
 * \param [in] unit - The unit number.
 * \param [in] info - The l3 route structure to be verified.
 * \return
 *   \retval Zero if no error was encountered
 *   \retval Non-zero in case of an error.
 * \see
 *   * info above \n
 */
static shr_error_e
dnx_l3_route_kbp_verify(
    int unit,
    bcm_l3_route_t * info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** KBP specific routes check */
    if (((_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
          && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
         || (!_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6)
             && dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4)))
        && (!_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_SCALE_ROUTE)))
    {
        uint32 kbp_mngr_status;

        /** Get the KBP device status */
        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
        }

        if (kbp_mngr_split_rpf_in_use(unit))
        {
            if (!_SHR_IS_FLAG_SET(info->l3a_flags2, (BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP route APIs in split RPF mode should be used with one of the flags BCM_L3_FLAGS2_FWD_ONLY or BCM_L3_FLAGS2_RPF_ONLY\n");
            }

            if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY)
                && _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP route APIs in split RPF mode should use only one of the flags BCM_L3_FLAGS2_FWD_ONLY and BCM_L3_FLAGS2_RPF_ONLY, but not both\n");
            }
        }
        else
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags2, (BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "KBP route APIs in non-split RPF mode do not allow the use of the flags (BCM_L3_FLAGS2_FWD_ONLY and/or BCM_L3_FLAGS2_RPF_ONLY)\n");
            }
        }
    }
    else
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags2, (BCM_L3_FLAGS2_FWD_ONLY | BCM_L3_FLAGS2_RPF_ONLY)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "KBP only flags (BCM_L3_FLAGS2_FWD_ONLY and/or BCM_L3_FLAGS2_RPF_ONLY) are used without KBP\n");
        }
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
        uint32 field_size;
        int table_field_size;
        /*
         * If 'raw' entry is required then specify the whole field - KAPS_RESULT for KAPS and DESTINATION for KBP.
         * This indicates 'no encoding'.
         */
        *destination_type = (L3_TABLE_IS_KBP(dbal_table) ? DBAL_FIELD_DESTINATION : DBAL_FIELD_KAPS_RESULT);

        /*
         * Validate the result size using the field size (KAPS_RESULT 20 bits; DESTINATION 21 bits).
         * Also check the table field size in case it's truncated.
         */
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, *destination_type, &field_size));
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get
                        (unit, dbal_table, *destination_type, FALSE, 0, INST_SINGLE, &table_field_size));
        /** Take the smaller of the two sizes */
        field_size = (field_size > table_field_size ? table_field_size : field_size);
        if (BCM_L3_ITF_VAL_GET(info->l3a_intf) >= SAL_BIT(field_size))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Raw value for 'FEC' (%d) is out of range, should be lower than %d.",
                         BCM_L3_ITF_VAL_GET(info->l3a_intf), SAL_BIT(field_size));
        }
    }
    dbal_entry_key_field32_set(unit, *entry_handle_id, DBAL_FIELD_VRF, info->l3a_vrf);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Resolves the destination of the L3 host entry and fills in the appropriate fields in info structure.
 * \param [in] unit - The unit number.
 * \param [in] result_type - The DBAL result type of the entry.
 * \param [in] dbal_table_id - The DBAL table to which the entry belongs
 * \param [in] entry_handle_id - The DBAL handle to the entry.
 * \param [out] info - structure describing the entry. Valid out fields which can be filled in:
 *      * info->l3a_intf - L3 interface
 *      * info->encap_id - Encapsulation index.
 *      * info->l3a_port_tgid - Port or trunk ID.
 *      * info->stat_id - Statistics index
 *      * info->stat_pp_profile - statistics profile
 * \return
 *  * Zero for success
 * \see
 *  * bcm_dnx_l3_route_traverse
 *  * bcm_dnx_l3_route_get
 */
static shr_error_e
dnx_l3_host_resolve_destination(
    int unit,
    uint32 result_type,
    uint32 dbal_table_id,
    uint32 *entry_handle_id,
    bcm_l3_host_t * info)
{
    uint32 field_value;
    uint32 stat_pp_profile;
    uint32 sub_field_id;
    uint32 sub_field_val;
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Nullify the stucture members before beginning.
     * In case of an error receiving the result value, output value will be 0.
     */
    info->l3a_intf = 0;
    info->encap_id = 0;
    info->l3a_port_tgid = 0;
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /** Resolve the destination fields of the info structure based on the result type and the table ID. */
    if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST
          || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_STAT)
         && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
        || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST
             || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_STAT)
            && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST)
        || ((result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT)
            && dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
        || ((result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT
             || result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT)
            && dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD))
    {
        /** If result type is FWD_Dest, then only DESTINATION field is used. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, field_value);
    }
    else if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF ||
               result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT)
              && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
             || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF ||
                  result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT)
                 && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST))
    {
        /*
         * If the result type of the entry is FWD_DEST_DOUBLE_OUTLIF
         * then the fields that we can receive are gport, ETH-RIF and OUT_LIF.
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, field_value, &info->l3a_port_tgid));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_RIF, field_value);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, &field_value));
        BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, field_value);
    }
    else if ((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI &&
              dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST) ||
             (result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI &&
              dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST) ||
             (result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT &&
              dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD) ||
             (result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT &&
              dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD))
    {
        /** If result type is FWD_Dest_EEI, then DESTINATION + EEI fields are used. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, field_value, &sub_field_id, &sub_field_val));
        BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, sub_field_val);

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, field_value, &sub_field_id, &sub_field_val));
        /** For EEI, either EEI_ENCAPSULATION_POINTER is used or EEI_MPLS_*_COMMAND */

        /*
         * Jericho mode beginning
         * }
         */
        if (sub_field_id == DBAL_FIELD_EEI_ENCAPSULATION_POINTER)
        {
            if (sub_field_val)
            {
                
                /**BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                             BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER, sub_field_val); */
                BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, sub_field_val);
            }
        }
        else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            uint32 mpls_label, push_cmd, encap_id;
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &mpls_label, &field_value));

            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &push_cmd, &field_value));
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(encap_id, mpls_label, push_cmd);
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, encap_id);
        }
        /*
         * Jericho mode end
         * }
         */
        else
        {
            uint32 mpls_label, encap_id;
            info->l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
            SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                            (unit, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, DBAL_FIELD_MPLS_LABEL, &mpls_label, &field_value));

            /** In JR2 mode, the encap_id is encoded as EEI with push_profile 0 always(the input push_profile is not used and lost)*/
            BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(encap_id, mpls_label, 0);
            BCM_FORWARD_ENCAP_ID_VAL_SET(info->encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI,
                                         BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, encap_id);
        }
    }
    else if (((result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
               || result_type == DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT)
              && dbal_table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
             || ((result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF
                  || result_type == DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT)
                 && dbal_table_id == DBAL_TABLE_IPV6_UNICAST_PRIVATE_HOST)
             || (result_type == DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT
                 && dbal_table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
             || (result_type == DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT
                 && dbal_table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD))
    {
        uint32 out_lif_val;
        uint32 fec_index;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, *entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF,
                                                            INST_SINGLE, &out_lif_val));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, *entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &field_value));
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, field_value, &sub_field_id, &sub_field_val));
        if (sub_field_id == DBAL_FIELD_FEC)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, *entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, &fec_index));
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_FEC, fec_index);
            BCM_L3_ITF_SET(info->encap_id, BCM_L3_ITF_TYPE_LIF, out_lif_val);
        }
        else if (sub_field_id == DBAL_FIELD_PORT_ID || sub_field_id == DBAL_FIELD_LAG_ID)
        {
            info->l3a_intf = 0;
            SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field(unit, 0, field_value, &info->l3a_port_tgid));
            BCM_L3_ITF_SET(info->l3a_intf, BCM_L3_ITF_TYPE_LIF, out_lif_val);
            info->encap_id = 0;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type of host entry is not known.");
    }

    /** In case of an entry with stats - retrieve stats configuration values */
    if (dnx_stat_pp_result_type_verify(unit, dbal_table_id, result_type) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, *entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD,
                                                            INST_SINGLE, &stat_pp_profile));

        /*
         * Get statistic info only if stat profile is valid
         */
        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                *entry_handle_id,
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
    dbal_enum_value_field_system_headers_mode_e system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Determine the correct table in which the host entry needs to be added to.
     * Allocate handle to that table that will be used for adding the entry.
     */
    SHR_IF_ERR_EXIT(dnx_l3_host_dbal_table_select(unit, info, &dbal_table));

    /** Verify the inputs.*/
    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_add_verify(unit, info, dbal_table));

    l3_intf = BCM_L3_ITF_VAL_GET(info->l3a_intf);
    encap_id = BCM_FORWARD_ENCAP_ID_VAL_GET(info->encap_id);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

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
    if (info->l3a_port_tgid != BCM_GPORT_TYPE_NONE)
    {
        /**
         * Choose result type field for the dbal table.
         * l3a_intf must be LIF or RIF, encap_id can be NULL or LIF.
         * If the table is KBP, then only result types with one LIF are supported, stat is not possible.
         * Otherwise, either one or two LIFs can be supported.
         */
        if (L3_TABLE_IS_KBP(dbal_table))
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
            {
                result_type = DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;
            }
            else
            {
                result_type = DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT;;
            }
        }
        else
        {
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
            {
                if (encap_id != 0)
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                }
            }
            else
            {
                if (encap_id != 0)
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                        DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                }
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        if (L3_TABLE_IS_KBP(dbal_table) || encap_id == 0)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, l3_intf);
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
        /*
         * In case l3a_port_tgid is not set:
         * l3a_intf must be FEC, encap_id can be NULL, LIF or EEI.
         */
        if (encap_id == 0)
        {
            if (L3_TABLE_IS_KBP(dbal_table))
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
                }
                else
                {
                    result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_DEST_W_DEFAULT;
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
        }
        else
        {
            /*
             * If encap_id != 0 and the FEC is valid, then the destination of the entry will resolve to
             * a FEC and an out-LIF.
             * If BCM_L3_ENCAP_SPACE_OPTIMIZED is set, encap_id must be EEI.
             */
            uint8 frwd_with_eei = 0;
            if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) ||
                (BCM_FORWARD_ENCAP_ID_IS_EEI(info->encap_id)
                 && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)))
            {
                frwd_with_eei = 1;
            }

            if (L3_TABLE_IS_KBP(dbal_table))
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    result_type = !frwd_with_eei ?
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT;
                }
                else
                {
                    result_type = !frwd_with_eei ?
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_OUTLIF_W_DEFAULT :
                        DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_EEI_W_DEFAULT;
                }
            }
            else
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
                {
                    if (!frwd_with_eei)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV6_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                }
                else
                {
                    if (!frwd_with_eei)
                    {
                        result_type = (info->stat_pp_profile != STAT_PP_PROFILE_INVALID) ?
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF_STAT :
                            DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF;
                    }
                    else
                    {
                        result_type = DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_EEI;
                    }
                }
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
            if (frwd_with_eei)
            {
                if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_ENCAP_SPACE_OPTIMIZED) &&
                    BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(info->encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
                {
                    /** EEI is a MPLS command.*/
                    uint32 field_in_struct_encoded_val;

                    field_in_struct_encoded_val = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(info->encap_id);
                    if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
                    {
                        /** In JR2 mode, only swap and pop commands are available. SWAP is using for PUSH for L3 routing into VPN.*/
                        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                        (unit, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, DBAL_FIELD_MPLS_LABEL,
                                         &field_in_struct_encoded_val, &eei_val));
                        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                        (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_SWAP_COMMAND, &encap_id, &eei_val));
                    }
                    else
                    {
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
                }
                /** JR1 mode only begain */
                else if (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    /** EEI is a EEDB pointer.*/
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, &encap_id, &eei_val));

                }
                /** JR1 mode only end */

                SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, &l3_intf, &dest_val));

                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, eei_val);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, dest_val);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, encap_id);
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, l3_intf);
            }
        }
    }

    if (L3_TABLE_IS_KBP(dbal_table))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, INST_SINGLE, 0);
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
 *      * IPV6_UNICAST_PRIVATE_LPM_FORWARD_2
 *      * IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
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
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_add_verify(unit, info));
    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_kbp_verify(unit, info));

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
        /** No payload is supported only for RPF databases */
        if (kbp_mngr_split_rpf_in_use(unit)
            && !L3_TABLE_IS_RPF(dbal_table) && _SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "BCM_L3_FLAGS2_NO_PAYLOAD flag is expected to set only for RPF databases\n");
        }

        if (info->stat_pp_profile == STAT_PP_PROFILE_INVALID)
        {
            uint32 kbp_zeros[2] = { 0, 0 };
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         L3_KBP_TABLE_TO_RESULT_TYPE_FWD_DEST(dbal_table));
            /*
             * W.A. - setting the KBP padding in the result.
             * Future plan for it is to be ignored in BCM layer and handled in DBAL.
             */
            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_KBP_FORWARD_PMF_GENERIC_DATA,
                                             INST_SINGLE, kbp_zeros);
        }
        else
        {
            if (dbal_table == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT);
            }
            else if (dbal_table == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                             DBAL_RESULT_TYPE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD_KBP_FORWARD_DEST_STAT_W_DEFAULT);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The provided dbal table [%s] does not support STAT\n",
                             dbal_logical_table_to_string(unit, dbal_table));
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, info->stat_id);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                         info->stat_pp_profile);
        }

        /** payload data has to be zeroes, if the flag is set */
        if (_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_NO_PAYLOAD))
        {
            if (is_kbp_default || l3_intf)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "BCM_L3_FLAGS2_NO_PAYLOAD flag is set, we expect the payload to be all zeros, is_kbp_default = %d, l3_intf = %d\n",
                             is_kbp_default, l3_intf);
            }
        }

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
 *              BCM_L3_*** (Eg. BCM_L3_IP6, BCM_L3_MULTIPATH, BCM_L3_HIT_CLEAR)  \n
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
 *      IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
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
    uint32 result_type;
    uint32 fec_index, result_value, destination_val;
    uint32 dbal_table;
    uint32 table_hb_enabled;
    uint32 hitbit = 0;
    dbal_fields_e dbal_sub_field;
    uint8 is_kbp_default;
    uint32 stat_pp_profile;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_verify(unit, info));
    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_kbp_verify(unit, info));

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
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        /** Get entry result type */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &destination_val));

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination_val, &dbal_sub_field, &result_value));

        /** Set the flag for the RPF databases, when the payload data is all zeroes */
        if (kbp_mngr_split_rpf_in_use(unit) && L3_TABLE_IS_RPF(dbal_table))
        {
            if (!is_kbp_default && !destination_val)
            {
                info->l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
            }

        }
        /** In case of an entry with stats - retrieve stats configuration values */
        if (dnx_stat_pp_result_type_verify(unit, dbal_table, result_type) == _SHR_E_NONE)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD,
                                                                INST_SINGLE, &stat_pp_profile));
            /*
             * Get statistic info only if stat profile is valid
             */
            if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                    entry_handle_id,
                                                                    DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                    &info->stat_id));
                STAT_PP_ENGINE_PROFILE_SET(info->stat_pp_profile, stat_pp_profile,
                                           bcmStatCounterInterfaceIngressReceivePp);
            }
        }
    }
    else
    {
        /** Handling of non-KBP tables */
        /*
         *  Setting pointer to 'result_value' variable to receive the field value from the table.
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, &destination_val);

        /** Receive indication whether the hitbit is enabled for the selected table. */
        SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, dbal_table, DBAL_TABLE_IND_IS_HITBIT_EN, &table_hb_enabled));
        /** If the HB indication is enabled, retrieve the hitbit and clear it if BCM_L3_HIT_CLEAR flag was provided. */
        if (table_hb_enabled)
        {
            uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET;
            hitbit_flags |= (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HIT_CLEAR)) ? DBAL_ENTRY_ATTR_HIT_CLEAR : 0;
            SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, hitbit_flags, &hitbit));
        }
        /*
         * Preforming the action after this call the pointers that we set in field32_request()
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (hitbit != 0)
        {
            info->l3a_flags |= BCM_L3_HIT;
        }
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
                    /*
                     * If encoding is 'FEC' (full 20 bits value) and value is higher
                     * than maximal for FEC then assume it is 'raw'.
                     * So, set 'raw' flag to one.
                     * Otherwise, assume it is standard FEC and, so, set 'raw' flag to zero.
                     */
                    if (result_value >= dnx_data_l3.fec.nof_fecs_get(unit))
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
    uint32 result_type;
    uint32 dbal_table;
    uint32 hit_bit = 0;
    uint32 attr_type = DBAL_ENTRY_ATTR_HIT_GET;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_host_verify(unit, info));

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
     * KBP doesn't support hit bit
     */
    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_HIT_CLEAR))
        {
            attr_type |= DBAL_ENTRY_ATTR_HIT_CLEAR;
        }
        SHR_IF_ERR_EXIT(dbal_entry_attribute_request(unit, entry_handle_id, attr_type, &hit_bit));
    }
    /*
     * Getting all destination fields of the entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type);

    if (hit_bit)
    {
        info->l3a_flags |= BCM_L3_HIT;
    }

    /** Copy the specific field value to a given pointer based on the result type */
    SHR_IF_ERR_EXIT(dnx_l3_host_resolve_destination(unit, result_type, dbal_table, &entry_handle_id, info));

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
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
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
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_verify(unit, info));
    SHR_INVOKE_VERIFY_DNX(dnx_l3_route_kbp_verify(unit, info));

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
 *  Indicates if a KBP dbal table is valid according to the route info
 * \param [in]  unit - The unit number.
 * \param [in]  info - The l3 route structure.
 * \param [in]  dbal_table - The dbal table ID
 * \param [out] is_valid - The indication if the KBP table is enabled
 * \return
 *   \retval Zero in case of no error.
 *   \retval Non-zero in case of an error.
 * \see
 *   * bcm_l3_route_t fields \n
 *   * BCM L3 Flags \n
 */
static shr_error_e
bcm_dnx_l3_route_kbp_table_is_valid(
    int unit,
    bcm_l3_route_t * info,
    dbal_tables_e dbal_table,
    uint8 *is_valid)
{
    uint32 kbp_mngr_status;
    uint8 use_kbp_ipv4 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4) ? TRUE : FALSE;
    uint8 use_kbp_ipv6 = dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6) ? TRUE : FALSE;
    uint8 kbp_ipv4_public_enabled = FALSE;
    uint8 kbp_ipv6_public_enabled = FALSE;
    uint8 kbp_split_fwd_rpf = kbp_mngr_split_rpf_in_use(unit);

    SHR_FUNC_INIT_VARS(unit);

    (*is_valid) = FALSE;

    if (!L3_TABLE_IS_KBP(dbal_table))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The provided dbal table is not a KBP table\n");
    }

    /** Get the KBP device status */
    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));

    SHR_IF_ERR_EXIT(kbp_mngr_ipv4_public_enabled_get(unit, &kbp_ipv4_public_enabled));
    SHR_IF_ERR_EXIT(kbp_mngr_ipv6_public_enabled_get(unit, &kbp_ipv6_public_enabled));

    /** The table should pass all the checks to be considered enabled */

    /** Flags should be checked */
    if (info != NULL)
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
        {
            /** Table is IPv4 or KBP IPv6 application is not enabled */
            if (L3_TABLE_IS_KBP_IPV4(dbal_table) || !use_kbp_ipv6)
            {
                SHR_EXIT();
            }
            /** IPv6 tables remain for this check; Table is public, but IPv6 public is not enabled */
            if (L3_TABLE_IS_PUBLIC(dbal_table) && !kbp_ipv6_public_enabled)
            {
                SHR_EXIT();
            }
        }
        else
        {
            /** Table is IPv6 or KBP IPv4 application is not enabled */
            if (L3_TABLE_IS_KBP_IPV6(dbal_table) || !use_kbp_ipv4)
            {
                SHR_EXIT();
            }
            /** IPv4 tables remain for this check; Table is public, but IPv4 public is not enabled */
            if (L3_TABLE_IS_PUBLIC(dbal_table) && !kbp_ipv4_public_enabled)
            {
                SHR_EXIT();
            }
        }
        /** Table is RPF, but FWD and RPF are not split or RPF only flag is not set */
        if (L3_TABLE_IS_RPF(dbal_table))
        {
            if (!kbp_split_fwd_rpf || !_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_RPF_ONLY))
            {
                SHR_EXIT();
            }
        }
        /** Table is FWD, but FWD and RPF are split and the FWD only flag is not set */
        else if (kbp_split_fwd_rpf && !_SHR_IS_FLAG_SET(info->l3a_flags2, BCM_L3_FLAGS2_FWD_ONLY))
        {
            SHR_EXIT();
        }
    }
    /** No flags are checked */
    else
    {
        /** Table is public, but public for IPv4 or IPv6 is not enabled */
        if (L3_TABLE_IS_PUBLIC(dbal_table)
            && ((L3_TABLE_IS_KBP_IPV4(dbal_table) && !kbp_ipv4_public_enabled)
                || (L3_TABLE_IS_KBP_IPV6(dbal_table) && !kbp_ipv6_public_enabled)))
        {
            SHR_EXIT();
        }
        /** Table is RPF, but FWD and RPF are not split */
        if (L3_TABLE_IS_RPF(dbal_table) && !kbp_split_fwd_rpf)
        {
            SHR_EXIT();
        }
    }

    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
    }

    (*is_valid) = TRUE;

exit:
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
    uint32 *dbal_tables_to_delete;
    
    uint32 dbal_tables_ipv4[] = {
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2,
        /** KBP split tables must be last in the IPv4/IPv6 list for the rest of the logic to work. */
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF,
    };
    uint32 dbal_tables_ipv6[] = {
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD,
        DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF
    };
    uint32 table = 0;
    uint32 nof_tables;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * If info is empty, then both IPv4 and IPv6 tables will be cleared.
     * If info is not empty, then clearance will be made according to BCM_L3_IP6 flag
     */
    if (info != NULL)
    {
        if (_SHR_IS_FLAG_SET(info->l3a_flags, BCM_L3_IP6))
        {
            /** Only IPv6 routes will be cleared */
            dbal_tables_to_delete = (uint32 *) sal_alloc(sizeof(dbal_tables_ipv6), "IPv6 tables to delete");
            sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv6, sizeof(dbal_tables_ipv6));
            nof_tables = sizeof(dbal_tables_ipv6) / sizeof(dbal_tables_ipv6[0]);
        }
        else
        {
            /** Only IPv4 routes will be cleared */
            dbal_tables_to_delete = (uint32 *) sal_alloc(sizeof(dbal_tables_ipv4), "IPV4 tables to delete");
            sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv4, sizeof(dbal_tables_ipv4));
            nof_tables = sizeof(dbal_tables_ipv4) / sizeof(dbal_tables_ipv4[0]);
        }
    }
    else
    {
        dbal_tables_to_delete =
            (uint32 *) sal_alloc(sizeof(dbal_tables_ipv6) + sizeof(dbal_tables_ipv4), "All tables to delete");
        sal_memcpy(dbal_tables_to_delete, dbal_tables_ipv4, sizeof(dbal_tables_ipv4));
        nof_tables = sizeof(dbal_tables_ipv4) / sizeof(dbal_tables_ipv4[0]);
        sal_memcpy(&dbal_tables_to_delete[nof_tables], dbal_tables_ipv6, sizeof(dbal_tables_ipv6));
        nof_tables += sizeof(dbal_tables_ipv6) / sizeof(dbal_tables_ipv6[0]);
    }

    /*
     * If the pointer to bcm_l3_route_t structure is empty, all route entries are going to be cleared.
     * Otherwise, public and private table will be cleared based on the BCM_L3_IP6 flag -
     * if it is set, then public and private IPv6 tables will be cleared;
     * if it is not set, then public and private IPv4 tables will be cleared.
     */
    /** clear tables */
    for (; table < nof_tables; table++)
    {
        if (L3_TABLE_IS_KBP(dbal_tables_to_delete[table]))
        {
            uint8 is_valid;
            SHR_IF_ERR_EXIT(bcm_dnx_l3_route_kbp_table_is_valid(unit, info, dbal_tables_to_delete[table], &is_valid));
            if (!is_valid)
            {
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dbal_table_clear(unit, dbal_tables_to_delete[table]));
    }

exit:
    if (dbal_tables_to_delete != NULL)
    {
        sal_free(dbal_tables_to_delete);
    }
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
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
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
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
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
        SHR_IF_ERR_EXIT(dnx_l3_host_resolve_destination(unit, result_type, dbal_table, &entry_handle_id, &dbal_entry));

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
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2
 *      DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD
 *      DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2
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
    uint32 table_hb_enabled;
    uint32 hitbit = 0;
    uint32 hitbit_flags = DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY | DBAL_ENTRY_ATTR_HIT_SECONDARY;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
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
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD_2;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV6_UNICAST_PRIVATE_LPM_FORWARD;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv6))
        {
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_FORWARD;
            if (kbp_mngr_split_rpf_in_use(unit))
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF;
            }
        }
    }
    else
    {
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
        dbal_tables[nof_tables++] = DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD_2;

        if (dnx_data_elk.application.feature_get(unit, dnx_data_elk_application_ipv4))
        {
            if ((kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "KBP device in use, but not locked\n");
            }
            dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_FORWARD;
            if (kbp_mngr_split_rpf_in_use(unit))
            {
                dbal_tables[nof_tables++] = DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF;
            }
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
        SHR_IF_ERR_EXIT(dbal_tables_indication_get
                        (unit, dbal_tables[table], DBAL_TABLE_IND_IS_HITBIT_EN, &table_hb_enabled));
        if (table_hb_enabled)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, hitbit_flags));
            if (_SHR_IS_FLAG_SET(flags, BCM_L3_HIT_CLEAR))
            {
                SHR_IF_ERR_EXIT(dbal_iterator_attribute_action_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_HIT_CLEAR));
            }
        }
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
                 /* coverity[callee_ptr_arith:FALSE]  */
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                                (unit, entry_handle_id, DBAL_FIELD_IPV4, &dbal_entry.l3a_subnet,
                                 &dbal_entry.l3a_ip_mask));
            }

            /*
             * KBP routes are split to FWD and RPF.
             * Update the flags2 with BCM_L3_FLAGS2_FWD_ONLY and BCM_L3_FLAGS2_RPF_ONLY accordingly
             */
            if (L3_TABLE_IS_KBP(dbal_tables[table]))
            {
                if (L3_TABLE_IS_RPF(dbal_tables[table]))
                {
                    dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
                }
                else
                {
                    uint32 result_type;
                    uint32 stat_pp_profile;

                    dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;

                    /** check if RT include stat information */
                    /** Get entry result type */
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &result_type));

                    /** In case of an entry with stats - retrieve stats configuration values */
                    if (dnx_stat_pp_result_type_verify(unit, dbal_tables[table], result_type) == _SHR_E_NONE)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE,
                                         &stat_pp_profile));
                        /*
                         * Get statistic info only if stat profile is valid
                         */
                        if (stat_pp_profile != STAT_PP_PROFILE_INVALID)
                        {
                            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                                                entry_handle_id,
                                                                                DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE,
                                                                                &dbal_entry.stat_id));
                            STAT_PP_ENGINE_PROFILE_SET(dbal_entry.stat_pp_profile, stat_pp_profile,
                                                       bcmStatCounterInterfaceIngressReceivePp);
                        }
                    }
                }
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
            /** If hitbit indication is enabled for this table, receive the attribute and set the BCM_L3_HIT flag if the hitbit is raised. */
            if (table_hb_enabled)
            {
                SHR_IF_ERR_EXIT(dbal_entry_attribute_get(unit, entry_handle_id, hitbit_flags, &hitbit));
                if (hitbit != 0)
                {
                    dbal_entry.l3a_flags |= BCM_L3_HIT;
                }
            }

            /** Set the flag for the RPF databases, when the payload data is all zeroes */
            if (L3_TABLE_IS_RPF(dbal_tables[table]) && kbp_mngr_split_rpf_in_use(unit))
            {
                uint8 is_kbp_default;

                dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_IS_DEFAULT, &is_kbp_default);

                if (!is_kbp_default && !dbal_entry.l3a_intf)
                {
                    dbal_entry.l3a_flags2 |= BCM_L3_FLAGS2_NO_PAYLOAD;
                }
            }

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
