/*
 * ctest_dnx_rx_trap_erpp.c
 *
 *  Created on: Dec 21, 2017
 *      Author: dp889757
 */
/*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include "ctest_dnx_rx_trap.h"
#include "ctest_dnx_rx_trap_erpp.h"
#ifdef BCM_DNX2_SUPPORT
#include <src/bcm/dnx/rx/rx_trap_v1.h>
#endif
/*
 * }
 */

/*
 * DEFINEs
 * {
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX
/*
 * }
 */

/** ERPP trap test details */
sh_sand_man_t sh_dnx_rx_trap_erpp_man = {
    "ERPP traps configuration testing",
    "Create an ERPP Userdefined trap, then set its action profile according to"
        "an ERPP Application trap." "Get the trap action profile and compare it." "Clear the action profile"
};

/**
 * \brief
 *   Options list for 'lif' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_erpp_options[] = {
    {"type", SAL_FIELD_TYPE_STR, "Type of test, options: pos,neg", "pos"}
    ,
    {"mode", SAL_FIELD_TYPE_STR, "Mode of test, options: appl,act", "appl"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for ERPP shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_erpp_tests[] = {
    {"erpp_appl_positive", "type=pos mode=appl", CTEST_POSTCOMMIT}
    ,
    {"erpp_appl_negative", "type=neg mode=appl", CTEST_POSTCOMMIT}
    ,
    {"erpp_actions_positive", "type=pos mode=act", CTEST_POSTCOMMIT}
    ,
    {"erpp_actions_negative", "type=neg mode=act", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

void
ctest_dnx_rx_trap_erpp_fill(
    int unit,
    bcm_rx_trap_config_t * trap_config_p)
{
    trap_config_p->flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_PRIO | BCM_RX_TRAP_UPDATE_COLOR |
        BCM_RX_TRAP_UPDATE_QOS_MAP_ID | BCM_RX_TRAP_UPDATE_ENCAP_ID | BCM_RX_TRAP_UPDATE_COUNTER |
        BCM_RX_TRAP_UPDATE_COUNTER_2;
    trap_config_p->dest_port = 200 + (sal_rand() % 4);
    trap_config_p->prio = (sal_rand() % 2);
    trap_config_p->color = (sal_rand() % 2);
    trap_config_p->qos_map_id = (sal_rand() % 5);
    trap_config_p->encap_id = 0x4444;
    trap_config_p->pp_drop_reason = (sal_rand() % 4);
    trap_config_p->snoop_cmnd = (sal_rand() % 4);
    trap_config_p->mirror_profile = (sal_rand() % 4);
    trap_config_p->stat_obj_config_arr[0].stat_profile = 0;
    trap_config_p->stat_obj_config_arr[0].stat_id = 0x12345;
    trap_config_p->stat_obj_config_arr[1].stat_profile = 1;
    trap_config_p->stat_obj_config_arr[1].stat_id = 0x67890;
}

/**
* \brief
*   Gets the configuration of trap and compares with set configuration.
* \param [in] unit - unit ID
* \param [in] app_trap_type - application trap type
* \param [in] trap_gport_p - set trap gport
* \param [in] trap_config_p - set trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e
ctest_dnx_rx_trap_erpp_get_and_compare(
    int unit,
    bcm_rx_trap_t app_trap_type,
    bcm_gport_t * trap_gport_p,
    bcm_rx_trap_config_t * trap_config_p)
{
    bcm_gport_t trap_gport_get;
    bcm_rx_trap_config_t trap_config_get;
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    trap_config_get.core_config_arr = NULL;
    trap_config_get.core_config_arr_len = 0;
    /** Get the action profile configuration for an ERPP trap */
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, app_trap_type, &trap_gport_get));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, trap_gport_p, &trap_gport_get,
                                                     sizeof(trap_gport_get), "trap_gport"));
    /** Get trap action */
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, BCM_GPORT_TRAP_GET_ID(trap_gport_get), &trap_config_get));

    /** Compare configuration */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->flags, &trap_config_get.flags,
                                                     sizeof(trap_config_get.flags), "flags"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->dest_port, &trap_config_get.dest_port,
                                                     sizeof(trap_config_get.dest_port), "dest_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->prio, &trap_config_get.prio, sizeof(trap_config_get.prio), "priority (TC)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->color, &trap_config_get.color, sizeof(trap_config_get.color), "color (DP)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->qos_map_id, &trap_config_get.qos_map_id,
                     sizeof(trap_config_get.qos_map_id), "qos_map_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->encap_id, &trap_config_get.encap_id,
                     sizeof(trap_config_get.encap_id), "encap_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->pp_drop_reason, &trap_config_get.pp_drop_reason,
                     sizeof(trap_config_get.pp_drop_reason), "pp_drop_reason"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->snoop_cmnd, &trap_config_get.snoop_cmnd,
                     sizeof(trap_config_get.snoop_cmnd), "snoop_cmnd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->mirror_cmd, &trap_config_get.mirror_cmd,
                     sizeof(trap_config_get.mirror_cmd), "mirror_cmd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_profile,
                     &trap_config_get.stat_obj_config_arr[0].stat_profile,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_profile),
                     "stat_obj_config_arr[0].stat_profile"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_id,
                     &trap_config_get.stat_obj_config_arr[0].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_id), "stat_obj_config_arr[0].stat_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_profile,
                     &trap_config_get.stat_obj_config_arr[1].stat_profile,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_profile),
                     "stat_obj_config_arr[1].stat_profile"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_id,
                     &trap_config_get.stat_obj_config_arr[1].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_id), "stat_obj_config_arr[1].stat_id"));

    SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, app_trap_type, &appl_trap_info));
    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get and compare was done on bcmRxTrap%s \n %s%s%s",
                appl_trap_info.trap_name, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Gets the configuration of trap and compares with set configuration.
* \param [in] unit - unit ID
* \param [in] app_trap_type - application trap type
* \param [in] trap_gport_p - set trap gport
* \param [in] trap_config_p - set trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
ctest_dnx_rx_trap_erpp_actions_get_and_compare(
    int unit,
    int fwd_action_profile,
    bcm_rx_trap_config_t * trap_config_p)
{
    bcm_rx_trap_config_t trap_config_get;
    SHR_FUNC_INIT_VARS(unit);

    trap_config_get.core_config_arr = NULL;
    trap_config_get.core_config_arr_len = 0;

    /** Get trap action */
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, fwd_action_profile, &trap_config_get));

    /** Compare configuration */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->flags, &trap_config_get.flags,
                                                     sizeof(trap_config_get.flags), "flags"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &trap_config_p->dest_port, &trap_config_get.dest_port,
                                                     sizeof(trap_config_get.dest_port), "dest_port"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->prio, &trap_config_get.prio, sizeof(trap_config_get.prio), "priority (TC)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->color, &trap_config_get.color, sizeof(trap_config_get.color), "color (DP)"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->qos_map_id, &trap_config_get.qos_map_id,
                     sizeof(trap_config_get.qos_map_id), "qos_map_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->encap_id, &trap_config_get.encap_id,
                     sizeof(trap_config_get.encap_id), "encap_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->pp_drop_reason, &trap_config_get.pp_drop_reason,
                     sizeof(trap_config_get.pp_drop_reason), "pp_drop_reason"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->snoop_cmnd, &trap_config_get.snoop_cmnd,
                     sizeof(trap_config_get.snoop_cmnd), "snoop_cmnd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->mirror_cmd, &trap_config_get.mirror_cmd,
                     sizeof(trap_config_get.mirror_cmd), "mirror_cmd"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_profile,
                     &trap_config_get.stat_obj_config_arr[0].stat_profile,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_profile),
                     "stat_obj_config_arr[0].stat_profile"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[0].stat_id,
                     &trap_config_get.stat_obj_config_arr[0].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[0].stat_id), "stat_obj_config_arr[0].stat_id"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_profile,
                     &trap_config_get.stat_obj_config_arr[1].stat_profile,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_profile),
                     "stat_obj_config_arr[1].stat_profile"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->stat_obj_config_arr[1].stat_id,
                     &trap_config_get.stat_obj_config_arr[1].stat_id,
                     sizeof(trap_config_get.stat_obj_config_arr[1].stat_id), "stat_obj_config_arr[1].stat_id"));

    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile: %d - Get and compare was done! \n %s%s%s",
                fwd_action_profile, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of ERPP traps.
 *   Sets a UserDefined ERPP trap
 *   Does an action profile set
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_erpp_appl_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, trap_strength = 7, snoop_strength = 0;
    int trap_iter = 0, snoop_cmd = 0;
    uint8 is_allocated;
    bcm_gport_t init_trap_gport, trap_gport, trap_gport_get;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t bcm_trap_type;
    bcm_mirror_destination_t mirror_dest;
    uint8 unknown_da_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_unknown_da_trap_en);
    uint8 glem_not_found_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_glem_not_found_trap);
    uint8 system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, bcmRxTrapEgIpv4Ttl0, &init_trap_gport));

    /*
     * ---------------------------------------- ERPP Application Traps ----------------------------------
     */

    /** Check snoop configuration */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.gport = 200;
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    SHR_IF_ERR_EXIT(bcm_mirror_destination_create(unit, &mirror_dest));
    snoop_cmd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(trap_gport, snoop_cmd, 0, 5);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, bcmRxTrapEgIpv4Ttl0, trap_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, bcmRxTrapEgIpv4Ttl0, &trap_gport_get));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_gport, &trap_gport_get, sizeof(trap_gport_get), "trap_gport"));

    /** Check trap configuration */
    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_erpp_fill(unit, &trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgUserDefine, &trap_id, &trap_config));

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ERPP)
        {
            if (appl_trap_info.valid == FALSE)
            {
                continue;
            }

            if (trap_iter == bcmRxTrapEgUnknownDa && (unknown_da_enabled == FALSE ||
                                                      system_headers_mode ==
                                                      DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
            {
                continue;
            }

            if ((glem_not_found_enabled == FALSE) &&
                ((trap_iter == bcmRxTrapEgGlemPpTrap) || (trap_iter == bcmRxTrapEgGlemNonePpTrap)))
            {
                continue;
            }

            if (appl_trap_info.trap_id > DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE
                && dnx_data_device.general.device_id_get(unit) != 0x8850)
            {
                continue;
            }

            /** Set the action profile configuration for an ERPP trap */
            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_erpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));

            /** Clear the action profile configuration for an ERPP trap */
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ERPP)
        {
            if (appl_trap_info.valid == FALSE)
            {
                continue;
            }

            if (trap_iter == bcmRxTrapEgUnknownDa && (unknown_da_enabled == FALSE ||
                                                      system_headers_mode ==
                                                      DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
            {
                continue;
            }

            if ((glem_not_found_enabled == FALSE) &&
                ((trap_iter == bcmRxTrapEgGlemPpTrap) || (trap_iter == bcmRxTrapEgGlemNonePpTrap)))
            {
                continue;
            }

            if (appl_trap_info.trap_id > DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE
                && dnx_data_device.general.device_id_get(unit) != 0x8850)
            {
                continue;
            }

            /** Set the action profile configuration for an ERPP trap */
            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        if (trap_iter == bcmRxTrapEgUnknownDa && (unknown_da_enabled == FALSE ||
                                                  system_headers_mode ==
                                                  DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
        {
            continue;
        }

        if ((glem_not_found_enabled == FALSE) &&
            ((trap_iter == bcmRxTrapEgGlemPpTrap) || (trap_iter == bcmRxTrapEgGlemNonePpTrap)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));

        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ERPP)
        {
            if (appl_trap_info.valid == FALSE)
            {
                continue;
            }

            if (appl_trap_info.trap_id > DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE
                && dnx_data_device.general.device_id_get(unit) != 0x8850)
            {
                continue;
            }

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_erpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        if (trap_iter == bcmRxTrapEgUnknownDa && (unknown_da_enabled == FALSE ||
                                                  system_headers_mode ==
                                                  DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
        {
            continue;
        }

        if ((glem_not_found_enabled == FALSE) &&
            ((trap_iter == bcmRxTrapEgGlemPpTrap) || (trap_iter == bcmRxTrapEgGlemNonePpTrap)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));

        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ERPP)
        {
            if (appl_trap_info.valid == FALSE)
            {
                continue;
            }

            if (appl_trap_info.trap_id > DBAL_ENUM_FVAL_ERPP_TRAP_ID_TTL_EQUALS_ONE
                && dnx_data_device.general.device_id_get(unit) != 0x8850)
            {
                continue;
            }

             /** Clear the action profile configuration for an ERPP trap */
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    /** Covering ERPP Trap type from ID get case */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &bcm_trap_type));

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    /** Test if is_allocated function works on the default profile */
    SHR_IF_ERR_EXIT(dnx_rx_trap_is_trap_id_allocated(unit, BCM_RX_TRAP_EG_TRAP_ID_DEFAULT, &is_allocated));

#ifdef BCM_DNX2_SUPPORT
    if (dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_v1_erpp_filters_init(unit, init_trap_gport));
    }
#endif

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "ERPP TRAP CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of ERPP traps.
 *   Sets a UserDefined ERPP trap
 *   Does an action profile set
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_erpp_actions_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id[8];
    int fwd_action_profile = 0;
    bcm_rx_trap_config_t trap_config[8], trap_config_get;
    bcm_rx_trap_core_config_t core_config[2], core_config_get[2];
    bcm_core_t core_id;
#ifdef ADAPTER_SERVER_MODE
    uint32 nof_cores = 1;
#else
    uint32 nof_cores = dnx_data_device.general.nof_cores_get(unit);
#endif
    uint8 nof_erpp_trap_profiles = dnx_data_trap.erpp.nof_erpp_user_configurable_profiles_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE ERPP TRAP ACTIONS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

    for (fwd_action_profile = 0; fwd_action_profile < (nof_erpp_trap_profiles - 1); fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &trap_id[fwd_action_profile]));
    }

    for (fwd_action_profile = 0; fwd_action_profile < (nof_erpp_trap_profiles - 1); fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id[fwd_action_profile]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_set twice on the same trap_id             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &trap_id[0]));
    bcm_rx_trap_config_t_init(&trap_config[0]);
    ctest_dnx_rx_trap_erpp_fill(unit, &trap_config[0]);

    for (fwd_action_profile = 0; fwd_action_profile < 2; fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id[0], &trap_config[0]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_get twice on the same trap_id             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config_get);

    for (fwd_action_profile = 0; fwd_action_profile < 2; fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id[0], &trap_config_get));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_set overwrite on the same trap_id             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    for (fwd_action_profile = 0; fwd_action_profile < 2; fwd_action_profile++)
    {
        bcm_rx_trap_config_t_init(&trap_config[fwd_action_profile]);
        ctest_dnx_rx_trap_erpp_fill(unit, &trap_config[fwd_action_profile]);
        SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id[0], &trap_config[fwd_action_profile]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_set with only init struct                 \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config[0]);
    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id[0], &trap_config[0]));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id[0]));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          Create/set/get/compare/delete on all profiles             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    for (fwd_action_profile = 0; fwd_action_profile < (nof_erpp_trap_profiles - 1); fwd_action_profile++)
    {
        LOG_INFO_EX(BSL_LS_APPL_COMMON, "ERPP TRAP ACTIONS TEST - FWD_ACTION_INDEX : %d.\n%s%s%s",
                    (fwd_action_profile + 1), EMPTY, EMPTY, EMPTY);
        bcm_rx_trap_config_t_init(&trap_config[fwd_action_profile]);
        ctest_dnx_rx_trap_erpp_fill(unit, &trap_config[fwd_action_profile]);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set
                        (unit, bcmRxTrapEgUserDefine, &trap_id[fwd_action_profile], &trap_config[fwd_action_profile]));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_erpp_actions_get_and_compare
                        (unit, trap_id[fwd_action_profile], &trap_config[fwd_action_profile]));
    }

    for (fwd_action_profile = 0; fwd_action_profile < (nof_erpp_trap_profiles - 1); fwd_action_profile++)
    {
        /** Destroy the ERPP UserDefined trap */
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id[fwd_action_profile]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          bcm_rx_trap_set dest per core config.             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config[0]);
    trap_config[0].flags = BCM_RX_TRAP_UPDATE_DEST;
    core_config[0].dest_port = 201;
    core_config[1].dest_port = 200;
    trap_config[0].core_config_arr = core_config;
    trap_config[0].core_config_arr_len = nof_cores;
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgUserDefine, &trap_id[0], &trap_config[0]));

    bcm_rx_trap_config_t_init(&trap_config_get);
    trap_config_get.core_config_arr = core_config_get;
    trap_config_get.core_config_arr_len = nof_cores;
    /** Get trap action */
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id[0], &trap_config_get));

    /** Compare configuration */
    for (core_id = 0; core_id < nof_cores; core_id++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &core_config[core_id].dest_port, &core_config_get[core_id].dest_port,
                         sizeof(bcm_gport_t), "core_config_arr.dest_port"));
    }

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id[0]));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          bcm_rx_trap_get get a drop config.             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &trap_id[0]));
    bcm_rx_trap_config_t_init(&trap_config[0]);
    bcm_rx_trap_config_t_init(&trap_config[1]);
    trap_config[0].flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config[0].dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id[0], &trap_config[0]));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_erpp_actions_get_and_compare(unit, trap_id[0], &trap_config[0]));

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id[0]));

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE ERPP TRAP ACTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the error messages
 *   and codes of all ERPP trap actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_erpp_actions_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, rv;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_core_config_t core_config[2];
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ERPP TRAP ACTIONS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'dest_port' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 3000;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "             Supplying Trunk gport as destination           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    BCM_GPORT_TRUNK_SET(trap_config.dest_port, 200);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "      Set 'core_config_arr' invalid configuration          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 200;
    trap_config.core_config_arr = core_config;
    trap_config.core_config_arr_len = 2;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.core_config_arr = core_config;
    trap_config.core_config_arr_len = 0;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = 200;
    trap_config.core_config_arr = NULL;
    trap_config.core_config_arr_len = 2;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'prio' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_PRIO;
    trap_config.prio = 16;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'color' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COLOR;
    trap_config.color = 10;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'qos_map_id' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_QOS_MAP_ID;
    trap_config.qos_map_id = 0xFF;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'encap_id' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_ENCAP_ID;
    trap_config.encap_id = 0x4c100001;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'Counter 1' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COUNTER;
    trap_config.stat_obj_config_arr[0].stat_id = 0xFFFFFF;
    trap_config.stat_obj_config_arr[0].stat_profile = 0;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COUNTER;
    trap_config.stat_obj_config_arr[0].stat_id = 0xFFA;
    trap_config.stat_obj_config_arr[0].stat_profile = 10;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'Counter 2' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COUNTER_2;
    trap_config.stat_obj_config_arr[1].stat_id = 0xFFFFFF;
    trap_config.stat_obj_config_arr[1].stat_profile = 0;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COUNTER_2;
    trap_config.stat_obj_config_arr[1].stat_id = 0xFFA;
    trap_config.stat_obj_config_arr[1].stat_profile = 10;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'pp_drop_reason' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.pp_drop_reason = 300;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'mirror_profile' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.mirror_profile = 20;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          Set 'snoop_cmnd' invalid value              \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.snoop_cmnd = 20;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying default trap ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    trap_id = BCM_RX_TRAP_EG_TRAP_ID_DEFAULT;
    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COLOR;
    trap_config.color = 2;

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ERPP TRAP ACTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the error messages
 *   and codes of all ERPP application traps.
 *
 * \param [in] unit - The unit number.
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_erpp_appl_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, rv;
    uint8 unknown_da_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_unknown_da_trap_en);
    uint8 system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    bcm_gport_t trap_gport;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ERPP APPLICATION TRAPS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgUserDefine, &trap_id));

    if (unknown_da_enabled == FALSE || system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "        bcmRxTrapEgUnknownDa configured when not supported      \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

        BCM_GPORT_TRAP_SET(trap_gport, trap_id, 7, 0);
        rv = ctest_dnx_rx_trap_profile_set(unit, bcmRxTrapEgUnknownDa, trap_gport);
        if (rv != BCM_E_UNAVAIL)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U
                       (unit, "ctest_dnx_rx_trap_profile_set should fail with BCM_E_UNAVAIL! . Error:%d (%s)\n"), rv,
                       bcm_errmsg(rv)));
            return rv;
        }
    }

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "        bcmRxTrapEgUserDefine configured WITH_ID bigger than supported      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    trap_id = 360;
    rv = bcm_rx_trap_type_create(unit, BCM_RX_TRAP_WITH_ID, bcmRxTrapEgUserDefine, &trap_id);
    if (rv != _SHR_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U
                   (unit, "bcm_rx_trap_type_create should fail with _SHR_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   ERPP Application trap type get \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgUserDefine, &trap_id);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ERPP APPLICATION TRAPS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_erpp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;
    char *test_mode;
    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_data_trap.egress.feature_get(unit, dnx_data_trap_egress_egress_divided_to_erpp_and_etpp))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Test is not supported on this device\n");
    }

    SH_SAND_GET_STR("type", test_type);
    SH_SAND_GET_STR("mode", test_mode);

    if (sal_strncasecmp
        (test_type, CTEST_DNX_RX_TRAP_ERPP_TEST_TYPE_POSITIVE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        if (sal_strncasecmp
            (test_mode, CTEST_DNX_RX_TRAP_ERPP_TEST_MODE_ACTIONS, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_erpp_actions_pos(unit, args, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_erpp_appl_pos(unit, args, sand_control));
        }
    }
    else if (sal_strncasecmp
             (test_type, CTEST_DNX_RX_TRAP_ERPP_TEST_TYPE_NEGATIVE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {
        if (sal_strncasecmp
            (test_mode, CTEST_DNX_RX_TRAP_ERPP_TEST_MODE_ACTIONS, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_erpp_actions_neg(unit, args, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_erpp_appl_neg(unit, args, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
