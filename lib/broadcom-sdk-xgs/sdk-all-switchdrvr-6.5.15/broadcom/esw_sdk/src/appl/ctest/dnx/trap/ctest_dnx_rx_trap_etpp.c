/*
 * ctest_dnx_rx_trap_etpp.c
 *
 *  Created on: Feb 1, 2018
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
#include "ctest_dnx_rx_trap_etpp.h"
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

/** ETPP trap test details */
sh_sand_man_t sh_dnx_rx_trap_etpp_man = {
    "ETPP traps configuration testing",
    "Create an ETPP Userdefined trap, then set its action profile according to"
        "an ETPP Application trap." "Get the trap action profile and compare it." "Clear the action profile"
};

/**
 * \brief
 *   List of tests for ETPP traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_etpp_tests[] = {
    {"etpp", "", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
* \brief
*   fill trap action configuration.
* \param [in] unit - unit ID
* \param [in] trap_config_p - trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
void
ctest_dnx_rx_trap_etpp_fill(
    int unit,
    bcm_rx_trap_config_t * trap_config_p)
{
    BCM_GPORT_TRAP_SET(trap_config_p->cpu_trap_gport, DBAL_ENUM_FVAL_INGRESS_TRAP_ID_DEFAULT_DROP_TRAP, 15, 0);
    trap_config_p->is_recycle_high_priority = (sal_rand() % 1);
    trap_config_p->is_recycle_crop_pkt = (sal_rand() % 1);
    trap_config_p->is_recycle_append_ftmh = (sal_rand() % 1);
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
ctest_dnx_rx_trap_etpp_get_and_compare(
    int unit,
    bcm_rx_trap_t app_trap_type,
    bcm_gport_t * trap_gport_p,
    bcm_rx_trap_config_t * trap_config_p)
{
    bcm_gport_t trap_gport_get;
    bcm_rx_trap_config_t trap_config_get;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the action profile configuration for an ETPP trap */
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
                    (unit, &trap_config_p->cpu_trap_gport, &trap_config_get.cpu_trap_gport,
                     sizeof(trap_config_get.cpu_trap_gport), "cpu_trap_gport"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->is_recycle_high_priority, &trap_config_get.is_recycle_high_priority,
                     sizeof(trap_config_get.is_recycle_high_priority), "is_recycle_high_priority"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->is_recycle_crop_pkt, &trap_config_get.is_recycle_crop_pkt,
                     sizeof(trap_config_get.is_recycle_crop_pkt), "is_recycle_crop_pkt"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->is_recycle_append_ftmh, &trap_config_get.is_recycle_append_ftmh,
                     sizeof(trap_config_get.is_recycle_append_ftmh), "is_recycle_append_ftmh"));

    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get and compare was done on bcmRxTrap%s \n %s%s%s",
                dnx_rx_app_trap_type_map[app_trap_type].trap_name, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

 /**
 * \brief
 *   This function checks the configuration of ETPP traps.
 *   Sets a UserDefined ETPP trap
 *   Does an action profile set
 *   Gets the values and compares.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the ctest mechanism
 * \param [in] sand_control - passed according to the ctest mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_etpp(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, trap_strength = 7, snoop_strength = 0;
    int trap_iter = 0;
    bcm_gport_t trap_gport;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * ---------------------------------------- ETPP Application Traps ----------------------------------
     */
    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_etpp_fill(unit, &trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &trap_id, &trap_config));

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        if (dnx_rx_app_trap_type_map[trap_iter].trap_block == DNX_RX_TRAP_BLOCK_ETPP)
        {
            /** Set the action profile configuration for an ETPP trap */
            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));

            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_etpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));

            /** Clear the action profile configuration for an ETPP trap */
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    /** Destroy the ETPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ETPP TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx_trap_etpp.h for info
 */
shr_error_e
sh_dnx_rx_trap_etpp_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
