/*
 * ctest_dnx_rx_trap_protocol.c
 *
 *  Created on: Dec 20, 2017
 *      Author: dp889757
 */

/*
  * Include files.
  * {
  */
#include <sal/appl/sal.h>
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <bcm_int/dnx/rx/rx.h>
#include <shared/bsl.h>
#include "ctest_dnx_rx_trap_protocol.h"
#include "ctest_dnx_rx_trap.h"
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#undef BSL_LOG_MODULE
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * }
 */

/** Protocol trap test details */
sh_sand_man_t sh_dnx_rx_trap_protocol_man = {
    "Protocol traps configuration testing",
    "Set protocol trap, then get trap configuration and compare values."
};

/**
 * \brief
 *   This function compares value_set and value_get
 *
 * \param [in] unit - The unit number.
 * \param [in] value_set   -  value which was set
 * \param [in] value_get   -  value which was retrieved using get function
 * \param [in] value_name   -  value name
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_values_compare(
    int unit,
    int value_set,
    int value_get,
    char *value_name)
{
    if (value_set != value_get)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit,
                              "Unexpected data returned.\n"
                              "%s expected %d received %d.\n"), value_name, value_set, value_get));
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/**
 * \brief
 *   This function compares the configuration set for protocol trap profiles
 *   and the profiles configuration retrieved using get function.
 *
 * \param [in] unit - The unit number.
 * \param [in] profiles_set_p   -  protocol trap profiles configuration which was set
 * \param [in] profiles_get_p   -  protocol trap profiles configuration which was retrieved using get function
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_profiles_compare(
    int unit,
    bcm_rx_trap_protocol_profiles_t * profiles_set_p,
    bcm_rx_trap_protocol_profiles_t * profiles_get_p)
{
    return (ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->icmpv6_trap_profile,
                                                      (int) profiles_get_p->icmpv6_trap_profile, "ICMPV6 profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->icmpv4_trap_profile,
                                                      (int) profiles_get_p->icmpv4_trap_profile, "ICMPV4 profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->l2cp_trap_profile,
                                                      (int) profiles_get_p->l2cp_trap_profile, "L2CP profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->arp_trap_profile,
                                                      (int) profiles_get_p->arp_trap_profile, "ARP profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->igmp_trap_profile,
                                                      (int) profiles_get_p->igmp_trap_profile, "IGMP profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->dhcp_trap_profile,
                                                      (int) profiles_get_p->dhcp_trap_profile, "DHCP profile") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, (int) profiles_set_p->non_auth_8021x_trap_profile,
                                                      (int) profiles_get_p->non_auth_8021x_trap_profile,
                                                      "Non authorized 802.1x profile"));
}

/**
 * \brief
 *   This function compares the configuration set for protocol trap
 *   and the configuration retrieved using get function.
 *
 * \param [in] unit - The unit number.
 * \param [in] trap_gport_set   -  protocol trap action profile (encoded as gport) configuration which was set
 * \param [in] trap_gport_get   -  protocol trap action profile (encoded as gport) configuration which was retrieved using get function
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_gport_compare(
    int unit,
    bcm_gport_t trap_gport_set,
    bcm_gport_t trap_gport_get)
{
    return (ctest_dnx_rx_trap_protocol_values_compare(unit, BCM_GPORT_TRAP_GET_STRENGTH(trap_gport_set),
                                                      BCM_GPORT_TRAP_GET_STRENGTH(trap_gport_get),
                                                      "Forward strength") ||
            ctest_dnx_rx_trap_protocol_values_compare(unit, BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_set),
                                                      BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport_get),
                                                      "Snoop strength"));
}

/**
 * \brief
 *   This function checks protocol profiles configuration (set/get)
 *
 * \param [in] unit - The unit number.
 * \param [in] port   -  protocol trap action profile (encoded as gport) configuration which was set
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_profiles_config_check(
    int unit,
    bcm_gport_t port)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 1, 2, 3, 1, 2, 3, 1 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_get = { 0 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_reset = { 3, 1, 2, 3, 1, 2, 0 };

    result = bcm_rx_trap_protocol_profiles_set(unit, port, &protocol_profiles);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_protocol_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_profiles_get(unit, port, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_protocol_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = ctest_dnx_rx_trap_protocol_profiles_compare(unit, &protocol_profiles, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    result = bcm_rx_trap_protocol_profiles_set(unit, port, &protocol_profiles_reset);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_protocol_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_profiles_get(unit, port, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "dnx_rx_trap_protocol_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = ctest_dnx_rx_trap_protocol_profiles_compare(unit, &protocol_profiles_reset, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    /**
     * Clean-up, set all profiles to 0(default).
     */
    sal_memset(&protocol_profiles_reset, 0, sizeof(bcm_rx_trap_protocol_profiles_t));

    result = bcm_rx_trap_protocol_profiles_set(unit, port, &protocol_profiles_reset);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    return result;

}

/**
 * \brief
 *   This function checks protocol profiles configuration (set/get)
 *
 * \param [in] unit - The unit number.
 * \param [in] intf   -  LIF/RIF interface
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_interface_profiles_config_check(
    int unit,
    bcm_if_t intf)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 1, 2, 3, 1, 2, 3, 1 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_get = { 0 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_reset = { 3, 1, 2, 3, 1, 2, 0 };

    result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &protocol_profiles);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_interface_profiles_get(unit, intf, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = ctest_dnx_rx_trap_protocol_profiles_compare(unit, &protocol_profiles, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &protocol_profiles_reset);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_interface_profiles_get(unit, intf, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = ctest_dnx_rx_trap_protocol_profiles_compare(unit, &protocol_profiles_reset, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    /**
     * Clean-up, set all profiles to 0(default).
     */
    sal_memset(&protocol_profiles_reset, 0, sizeof(bcm_rx_trap_protocol_profiles_t));

    result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &protocol_profiles_reset);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    return result;

}

/**
 * \brief
 *   This function compares protocol profiles configuration (set/get) of APIs
 *   bcm_rx_trap_protocol_profiles_set/get vs. bcm_rx_trap_protocol_interface_profiles_set/get
 *   For the same LIF (given to each API in correct encoding gport/intf)
 *
 * \param [in] unit - The unit number.
 * \param [in] intf   -  LIF interface
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
ctest_dnx_rx_trap_protocol_interface_profiles_to_protocol_profiles_compare(
    int unit,
    bcm_if_t intf)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 1, 2, 3, 1, 2, 3, 1 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_get = { 0 };
    bcm_rx_trap_protocol_profiles_t protocol_interface_profiles_get = { 0 };
    bcm_rx_trap_protocol_profiles_t protocol_profiles_clear = { 0 };
    bcm_gport_t gport;

    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, intf);

    result = bcm_rx_trap_protocol_profiles_set(unit, gport, &protocol_profiles);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_profiles_get(unit, gport, &protocol_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_profiles_set(unit, gport, &protocol_profiles_clear);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &protocol_profiles);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_set failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_protocol_interface_profiles_get(unit, intf, &protocol_interface_profiles_get);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_get failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    result =
        ctest_dnx_rx_trap_protocol_profiles_compare(unit, &protocol_profiles_get, &protocol_interface_profiles_get);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    /**
     * Clean-up, set all profiles to 0(default).
     */

    result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &protocol_profiles_clear);
    if (BCM_FAILURE(result))
    {
        return result;
    }

    return result;
}

static bcm_error_t
ctest_dnx_rx_trap_ndp_my_ip_config_check(
    int unit)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info[4], info_get;
    int index, type_idx;
    bcm_switch_control_t types[2] = { bcmSwitchNdpMyIp1, bcmSwitchNdpMyIp2 };

    for (type_idx = 0; type_idx < 2; type_idx++)
    {
        for (index = 0; index < 4; index++)
        {
            key.type = types[type_idx];
            key.index = index;

            info[index].value = (0xFFFFFFF0 + index);
            result = bcm_switch_control_indexed_set(unit, key, info[index]);
            if (BCM_FAILURE(result))
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "bcm_switch_control_indexed_set failed. Error:%d (%s)\n"), result,
                           bcm_errmsg(result)));
                return result;
            }
        }
    }

    for (type_idx = 0; type_idx < 2; type_idx++)
    {
        for (index = 0; index < 4; index++)
        {
            key.type = types[type_idx];
            key.index = index;
            result = bcm_switch_control_indexed_get(unit, key, &info_get);
            if (BCM_FAILURE(result))
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit, "bcm_switch_control_indexed_get failed. Error:%d (%s)\n"), result,
                           bcm_errmsg(result)));
                return result;
            }

            if (info_get.value != info[index].value)
            {
                LOG_ERROR(BSL_LS_APPL_COMMON,
                          (BSL_META_U(unit,
                                      "Unexpected data returned.\n"
                                      "NDP MyIP(%d) index:%d expected %d received %d.\n"), (type_idx + 1), index,
                           info[index].value, info_get.value));
                return BCM_E_FAIL;
            }
        }
    }
    return result;

}

/**
 * \brief
 *   This function checks the configuration of protocol trap.
 *   Sets a protocol trap
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_protocol_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_rx_trap_config_t config = { 0 };
    bcm_rx_trap_protocol_key_t protocol_key;
    bcm_gport_t trap_gport, trap_gport_get, lif = 200;
    bcm_rx_trap_t protocol_types[] = { bcmRxTrapL2cpPeer, bcmRxTrapIcmpRedirect, bcmRxTrap8021xFail,
        bcmRxTrapIgmpMembershipQuery, bcmRxTrapIgmpReportLeaveMsg, bcmRxTrapIgmpUndefined,
        bcmRxTrapDhcpv4Client, bcmRxTrapDhcpv4Server, bcmRxTrapDhcpv6Client,
        bcmRxTrapDhcpv6Server, bcmRxTrapArp, bcmRxTrapArpMyIp, bcmRxTrapCount
    };
    uint8 nof_trap_args[] = { 64, 32, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    uint8 trap_arg, protocol_trap_profile, ii, kk;
    int trap_id = 0, fwd_strength = 15, snp_strength = 7;
    char *test_mode;
    bcm_if_t intf = 20;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE, test_mode);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "POSITIVE PROTOCOL TRAP CONFIGURATION TEST START...\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ACTION CONFIGURATION - USER DEFINED TRAP\n")));

    result = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_create failed. Error:%d (%s)\n"), result, bcm_errmsg(result)));
        return result;
    }

    result = bcm_rx_trap_set(unit, trap_id, &config);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set failed. Error:%d (%s)\n"), result, bcm_errmsg(result)));
        return result;
    }

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_set/get CONFIG CHECK\n")));

        result = (bcm_error_t) ctest_dnx_rx_trap_inlif_create_example(unit, &lif);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ctest_dnx_rx_trap_inlif_create_example failed. Error:%d (%s)\n"), result,
                       bcm_errmsg(result)));
            return result;
        }

        result = ctest_dnx_rx_trap_protocol_profiles_config_check(unit, lif);
        if (BCM_FAILURE(result))
        {
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "bcm_rx_trap_protocol_interface_profiles_set/get CONFIG CHECK\n")));

        result = (bcm_error_t) ctest_dnx_rx_trap_eth_inrif_create_example(unit, &intf);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ctest_dnx_rx_trap_eth_inrif_create_example failed. Error:%d (%s)\n"), result,
                       bcm_errmsg(result)));
            return result;
        }

        result = ctest_dnx_rx_trap_protocol_interface_profiles_config_check(unit, intf);
        if (BCM_FAILURE(result))
        {
            return result;
        }

        result = (bcm_error_t) ctest_dnx_rx_trap_inrif_create_example(unit, &intf);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ctest_dnx_rx_trap_inrif_create_example failed. Error:%d (%s)\n"), result,
                       bcm_errmsg(result)));
            return result;
        }

        result = ctest_dnx_rx_trap_protocol_interface_profiles_config_check(unit, intf);
        if (BCM_FAILURE(result))
        {
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U
                  (unit,
                   "bcm_rx_trap_protocol_profiles_set/get vs. bcm_rx_trap_protocol_interface_profiles_set/get CONFIG\n")));

        result = ctest_dnx_rx_trap_protocol_interface_profiles_to_protocol_profiles_compare(unit, intf);
        if (BCM_FAILURE(result))
        {
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "TRAP ACTION PROFILE CONFIGURATION\n")));

    for (kk = 0; kk < 2; kk++)
    {
        for (ii = 0; protocol_types[ii] != bcmRxTrapCount; ii++)
        {
            for (trap_arg = 0; trap_arg < nof_trap_args[ii]; trap_arg++)
            {
                for (protocol_trap_profile = 0; protocol_trap_profile < 4; protocol_trap_profile++)
                {
                    protocol_key.protocol_type = protocol_types[ii];
                    protocol_key.trap_args = trap_arg;
                    protocol_key.protocol_trap_profile = protocol_trap_profile;

                    result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_set failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }

                    trap_gport_get = 0;
                    result = bcm_rx_trap_protocol_get(unit, &protocol_key, &trap_gport_get);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_get failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }

                    result = ctest_dnx_rx_trap_protocol_gport_compare(unit, trap_gport, trap_gport_get);
                    if (BCM_FAILURE(result))
                    {
                        return result;
                    }

                    result = bcm_rx_trap_protocol_clear(unit, &protocol_key);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_destroy failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }
                }
            }
        }
    }

    for (kk = 0; kk < 2; kk++)
    {
        for (ii = 0; protocol_types[ii] != bcmRxTrapCount; ii++)
        {
            for (trap_arg = 0; trap_arg < nof_trap_args[ii]; trap_arg++)
            {
                for (protocol_trap_profile = 0; protocol_trap_profile < 4; protocol_trap_profile++)
                {
                    protocol_key.protocol_type = protocol_types[ii];
                    protocol_key.trap_args = trap_arg;
                    protocol_key.protocol_trap_profile = protocol_trap_profile;

                    result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_set failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }
                }
            }
        }
    }

    for (kk = 0; kk < 2; kk++)
    {
        for (ii = 0; protocol_types[ii] != bcmRxTrapCount; ii++)
        {
            for (trap_arg = 0; trap_arg < nof_trap_args[ii]; trap_arg++)
            {
                for (protocol_trap_profile = 0; protocol_trap_profile < 4; protocol_trap_profile++)
                {
                    protocol_key.protocol_type = protocol_types[ii];
                    protocol_key.trap_args = trap_arg;
                    protocol_key.protocol_trap_profile = protocol_trap_profile;

                    trap_gport_get = 0;
                    result = bcm_rx_trap_protocol_get(unit, &protocol_key, &trap_gport_get);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_get failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }

                    result = ctest_dnx_rx_trap_protocol_gport_compare(unit, trap_gport, trap_gport_get);
                    if (BCM_FAILURE(result))
                    {
                        return result;
                    }
                }
            }
        }
    }

    for (kk = 0; kk < 2; kk++)
    {
        for (ii = 0; protocol_types[ii] != bcmRxTrapCount; ii++)
        {
            for (trap_arg = 0; trap_arg < nof_trap_args[ii]; trap_arg++)
            {
                for (protocol_trap_profile = 0; protocol_trap_profile < 4; protocol_trap_profile++)
                {
                    protocol_key.protocol_type = protocol_types[ii];
                    protocol_key.trap_args = trap_arg;
                    protocol_key.protocol_trap_profile = protocol_trap_profile;

                    result = bcm_rx_trap_protocol_clear(unit, &protocol_key);
                    if (BCM_FAILURE(result))
                    {
                        LOG_ERROR(BSL_LS_APPL_COMMON,
                                  (BSL_META_U(unit, "bcm_rx_trap_protocol_destroy failed. Error:%d (%s)\n"), result,
                                   bcm_errmsg(result)));
                        return result;
                    }
                }
            }
        }
    }

    if (dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_ndp_trap_en) == TRUE)
    {
        result = ctest_dnx_rx_trap_ndp_my_ip_config_check(unit);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ctest_dnx_rx_trap_ndp_my_ip_config_check failed. Error:%d (%s)\n"), result,
                       bcm_errmsg(result)));
            return result;
        }
    }

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        result = (bcm_error_t) ctest_dnx_rx_trap_lif_destroy_example(unit, &lif);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ctest_dnx_rx_trap_lif_destroy_example failed. Error:%d (%s)\n"), result,
                       bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "POSITIVE PROTOCOL TRAP CONFIGURATION TEST END.\n")));
    return result;
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   This function checks the configuration of protocol trap.
 *   Sets a protocol trap
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_protocol_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_error_t result = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_rx_trap_protocol_profiles_t profiles;
    bcm_rx_trap_protocol_key_t protocol_key;
    bcm_gport_t trap_gport, port, lif = 0, gport;
    bcm_if_t intf = 20;
    bcm_rx_trap_t protocol_types[] = { bcmRxTrapL2cpPeer, bcmRxTrapIcmpRedirect, bcmRxTrap8021xFail,
        bcmRxTrapIgmpMembershipQuery, bcmRxTrapIgmpReportLeaveMsg, bcmRxTrapIgmpUndefined,
        bcmRxTrapDhcpv4Client, bcmRxTrapDhcpv4Server, bcmRxTrapDhcpv6Client,
        bcmRxTrapDhcpv6Server, bcmRxTrapArp, bcmRxTrapArpMyIp, bcmRxTrapCount
    };
    int nof_trap_args[] = { 64, 256, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
    uint8 protocol_trap_index;
    int trap_id = 0, fwd_strength = 15, snp_strength = 7;
    char *test_mode;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE, test_mode);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "NEGATIVE PROTOCOL TRAP CONFIGURATION TEST START...\n")));

    bcm_rx_trap_protocol_key_t_init(&protocol_key);
    bcm_rx_trap_protocol_profiles_t_init(&profiles);

    result = (bcm_error_t) ctest_dnx_rx_trap_inlif_create_example(unit, &lif);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "ctest_dnx_rx_trap_inlif_create_example failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_set NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING PORT IN IN-LIF MODE \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        port = 200;
        result = bcm_rx_trap_protocol_profiles_set(unit, port, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
        gport = lif;
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING IN-LIF IN PORT MODE \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_profiles_set(unit, lif, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
        gport = 200;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING PROTOCOL PROFILE OUT OF BOUNDS(>3) \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    profiles.l2cp_trap_profile = 4;
    profiles.icmpv4_trap_profile = 20;
    result = bcm_rx_trap_protocol_profiles_set(unit, gport, &profiles);
    if (result != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           SUPPLYING Interface instead of gport/port         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    result = (bcm_error_t) ctest_dnx_rx_trap_eth_inrif_create_example(unit, &intf);
    if (BCM_FAILURE(result))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "ctest_dnx_rx_trap_inrif_create_example failed. Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    bcm_rx_trap_protocol_profiles_t_init(&profiles);
    profiles.l2cp_trap_profile = 4;
    result = bcm_rx_trap_protocol_profiles_set(unit, intf, &profiles);
    if (result != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), result,
                   bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_set NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "bcm_rx_trap_protocol_interface_profiles_set NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "             SUPPLYING gport instead of interface            \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        bcm_rx_trap_protocol_profiles_t_init(&profiles);
        result = bcm_rx_trap_protocol_interface_profiles_set(unit, gport, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "bcm_rx_trap_protocol_interface_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "                  SUPPLYING FEC type interface               \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        bcm_rx_trap_protocol_profiles_t_init(&profiles);
        result = bcm_rx_trap_protocol_interface_profiles_set(unit, 0x2000CCCE, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "bcm_rx_trap_protocol_interface_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "        SUPPLYING PROTOCOL PROFILE OUT OF BOUNDS(>3)         \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        profiles.icmpv4_trap_profile = 20;
        result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "        Using API in unsupported port mode         \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        profiles.icmpv4_trap_profile = 2;
        result = bcm_rx_trap_protocol_interface_profiles_set(unit, intf, &profiles);
        if (result != BCM_E_UNAVAIL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_set should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "bcm_rx_trap_protocol_interface_profiles_set NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_get NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    bcm_rx_trap_protocol_profiles_t_init(&profiles);

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING PORT IN IN-LIF MODE \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_profiles_get(unit, port, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING NON-CREATED LIF \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_profiles_get(unit, lif + 1, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING IN-LIF IN PORT MODE \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_profiles_get(unit, lif, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "bcm_rx_trap_protocol_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_profiles_get NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "bcm_rx_trap_protocol_interface_profiles_get NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    if (sal_strncasecmp
        (test_mode, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE_LIF,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "             SUPPLYING gport instead of interface            \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_interface_profiles_get(unit, gport, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "bcm_rx_trap_protocol_interface_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "                  SUPPLYING FEC type interface               \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_interface_profiles_get(unit, 0x2000CCCE, &profiles);
        if (result != BCM_E_PARAM)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "bcm_rx_trap_protocol_interface_profiles_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "                  Using API in unsupported port mode               \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        result = bcm_rx_trap_protocol_interface_profiles_get(unit, 0x2000CCCE, &profiles);
        if (result != BCM_E_UNAVAIL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit,
                        "bcm_rx_trap_protocol_interface_profiles_get should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "bcm_rx_trap_protocol_interface_profiles_get NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_set NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY INVALID PROTOCOL TYPE \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = bcmRxTrapSameInterface;
    protocol_key.protocol_trap_profile = 2;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL OUT OF BOUNDS(>3)  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = protocol_types[0];
    protocol_key.protocol_trap_profile = 5;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (protocol_trap_index = 0; protocol_types[protocol_trap_index] != bcmRxTrapCount; protocol_trap_index++)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS %d  \n"), protocol_trap_index));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        protocol_key.protocol_type = protocol_types[protocol_trap_index];
        protocol_key.protocol_trap_profile = 3;
        protocol_key.trap_args = nof_trap_args[protocol_trap_index];

        result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
        if (result != BCM_E_CONFIG)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_protocol_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "        SUPPLY NDP PROTOCOL TYPE        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = bcmRxTrapNdp;
    protocol_key.protocol_trap_profile = 2;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_set(unit, &protocol_key, trap_gport);
    if (dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_ndp_trap_en) == FALSE)
    {
        if (result != BCM_E_UNAVAIL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_protocol_set should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }
    else
    {
        if (result != BCM_E_CONFIG)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_protocol_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_set NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_get NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY INVALID PROTOCOL TYPE \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = bcmRxTrapSameInterface;
    protocol_key.protocol_trap_profile = 2;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_get(unit, &protocol_key, &trap_gport);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL OUT OF BOUNDS(>3)  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = protocol_types[0];
    protocol_key.protocol_trap_profile = 5;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_get(unit, &protocol_key, &trap_gport);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (protocol_trap_index = 0; protocol_types[protocol_trap_index] != bcmRxTrapCount; protocol_trap_index++)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS %d  \n"), protocol_trap_index));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        protocol_key.protocol_type = protocol_types[protocol_trap_index];
        protocol_key.protocol_trap_profile = 3;
        protocol_key.trap_args = (nof_trap_args[protocol_trap_index] + 1);

        result = bcm_rx_trap_protocol_get(unit, &protocol_key, &trap_gport);
        if (result != BCM_E_CONFIG)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_protocol_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "dnx_rx_trap_protocol_set NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_clear NEGATIVE PROTOCOL TRAP CONFIGURATION START...\n")));

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY INVALID PROTOCOL TYPE \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = bcmRxTrapSameInterface;
    protocol_key.protocol_trap_profile = 2;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_clear(unit, &protocol_key);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_clear should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL OUT OF BOUNDS(>3)  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    protocol_key.protocol_type = protocol_types[0];
    protocol_key.protocol_trap_profile = 5;
    protocol_key.trap_args = 2;

    result = bcm_rx_trap_protocol_clear(unit, &protocol_key);
    if (result != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_protocol_clear should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (protocol_trap_index = 0; protocol_types[protocol_trap_index] != bcmRxTrapCount; protocol_trap_index++)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY PROTOCOL ARGS OF BOUNDS %d  \n"), protocol_trap_index));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-------------------------------------------------------------\n")));

        protocol_key.protocol_type = protocol_types[protocol_trap_index];
        protocol_key.protocol_trap_profile = 3;
        protocol_key.trap_args = (nof_trap_args[protocol_trap_index] + 1);

        result = bcm_rx_trap_protocol_clear(unit, &protocol_key);
        if (result != BCM_E_CONFIG)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_protocol_clear should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                       result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLY INVALID INDEX TO NDP ADDITIONAL CONFIG  \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchNdpMyIp1;
    key.index = 4;

    info.value = 0xFFFFFFFF;
    result = bcm_switch_control_indexed_set(unit, key, info);
    if (result != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    key.type = bcmSwitchNdpMyIp2;
    key.index = 20;

    result = bcm_switch_control_indexed_get(unit, key, &info);
    if (result != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   result, bcm_errmsg(result)));
        return result;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "bcm_rx_trap_protocol_clear NEGATIVE PROTOCOL TRAP CONFIGURATION END...\n")));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "NEGATIVE PROTOCOL TRAP CONFIGURATION TEST END.\n")));
    result = BCM_E_NONE;

    return result;
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Options list for 'protocol' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_protocol_options[] = {
    /*
     * Name
     *//*
     * Type
     *//*
     * Description
     *//*
     * Default
     */
    {CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test positive, negative or exhaustive",
     "positive"}
    ,
    {CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_MODE, SAL_FIELD_TYPE_STR,
     "Mode of Protocol traps, per In-Port or per In-LIF", "lif"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for protocol shell command (to be run on regression, pre-commit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_protocol_tests[] = {
    {"protocol_pos", "type=pos mode=lif", CTEST_POSTCOMMIT}
    ,
    {"protocol_neg", "type=neg mode=lif", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief - run Protocol trap Ctest application.
 */
shr_error_e
sh_dnx_rx_trap_protocol_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_TYPE, test_type);
    if (sal_strncasecmp
        (test_type, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_TYPE_POSITIVE,
         SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_protocol_pos(unit, args, sand_control));
    }
    else if (sal_strncasecmp
             (test_type, CTEST_DNX_RX_TRAPS_PROTOCOL_OPTION_TEST_TYPE_NEGATIVE,
              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_protocol_neg(unit, args, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_protocol_pos(unit, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}
