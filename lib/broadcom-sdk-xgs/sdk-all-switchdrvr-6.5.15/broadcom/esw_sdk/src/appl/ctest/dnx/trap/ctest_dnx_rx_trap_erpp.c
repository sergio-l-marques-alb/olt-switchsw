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
#include <bcm_int/dnx/rx/rx.h>
#include <include/bcm_int/dnx/rx/rx_trap_map.h>
#include <shared/bsl.h>
#include "ctest_dnx_rx_trap.h"
#include "ctest_dnx_rx_trap_erpp.h"
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
 *   List of tests for ERPP shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_erpp_tests[] = {
    {"erpp", "", CTEST_POSTCOMMIT}
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
    trap_config_p->dest_port = 200;
    trap_config_p->prio = (sal_rand() % 2);
    trap_config_p->color = (sal_rand() % 2);
    trap_config_p->qos_map_id = (sal_rand() % 5);
    trap_config_p->encap_id = 0x4444;
    trap_config_p->snoop_cmnd = 3;
    trap_config_p->mirror_profile = 1;
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
    SHR_FUNC_INIT_VARS(unit);

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

    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get and compare was done on bcmRxTrap%s \n %s%s%s",
                dnx_rx_app_trap_type_map[app_trap_type].trap_name, EMPTY, EMPTY, EMPTY);
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
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_erpp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, trap_strength = 7, snoop_strength = 0;
    int trap_iter = 0;
    bcm_gport_t trap_gport;
    bcm_rx_trap_config_t trap_config;
    uint8 ipv6_traps_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_ipv6_traps_en);
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ---------------------------------------- ERPP Application Traps ----------------------------------
     */

    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_erpp_fill(unit, &trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgUserDefine, &trap_id, &trap_config));

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        if (dnx_rx_app_trap_type_map[trap_iter].trap_block == DNX_RX_TRAP_BLOCK_ERPP &&
            trap_iter != bcmRxTrapEgUnknownDa)
        {
            if (trap_iter == bcmRxTrapEgIpv6Error && ipv6_traps_enabled == FALSE)
            {
                continue;
            }

            /** Set the action profile configuration for an ETPP trap */
            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_erpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));

            /** Clear the action profile configuration for an ETPP trap */
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    /** Destroy the ERPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "ERPP TRAP CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_erpp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_erpp(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
