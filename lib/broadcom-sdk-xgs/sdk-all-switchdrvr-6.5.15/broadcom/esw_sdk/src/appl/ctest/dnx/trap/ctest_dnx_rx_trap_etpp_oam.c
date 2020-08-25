/*
 * ctest_dnx_rx_trap_etpp_oam.c
 *
 * Semantic testing of the ETPP OAM traps.
 * Currently only supports Create/Delete testing.
 *
 * NOTE: Test fails with Segmentation Fault caused by PEMLA initialization
 *
 *  Created on: Dec 20, 2017
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
 /*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/rx/rx.h>
#include <bcm_int/dnx/rx/rx_trap_map.h>
#include <shared/bsl.h>
#include "ctest_dnx_rx_trap.h"
#include "ctest_dnx_rx_trap_etpp_oam.h"
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

/** ETPP OAM trap test details */
sh_sand_man_t sh_dnx_rx_trap_etpp_oam_man = {
    "ETPP OAM traps configuration testing",
    "Set ETPP OAM trap, then get trap configuration and compare values."
};

/**
 * \brief
 *   List of tests for ETPP OAM shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_etpp_oam_tests[] = {
    {"etpp_oam", "", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
* \brief
*   fill OAM trap action configuration.
* \param [in] unit - unit ID
* \param [in] trap_config_p - trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
void
ctest_dnx_rx_trap_etpp_oam_fill(
    int unit,
    bcm_rx_trap_config_t * trap_config_p)
{
    int trap_strength = sal_rand() % 14 + 1;
    BCM_GPORT_TRAP_SET(trap_config_p->cpu_trap_gport, DBAL_ENUM_FVAL_INGRESS_TRAP_ID_DEFAULT_DROP_TRAP, trap_strength,
                       0);
    trap_config_p->is_recycle_high_priority = 1;
    trap_config_p->stamped_trap_code = DBAL_ENUM_FVAL_INGRESS_TRAP_ID_OAM_REFLECTOR;
}

/**
* \brief
*   Gets the configuration of trap and compares with set configuration.
* \param [in] unit - unit ID
* \param [in] trap_type - trap type
* \param [in] trap_id - trap id
* \param [in] trap_config_p - set trap configuration
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
shr_error_e
ctest_dnx_rx_trap_etpp_oam_get_and_compare(
    int unit,
    bcm_rx_trap_t trap_type,
    int trap_id,
    bcm_rx_trap_config_t * trap_config_p)
{
    bcm_rx_trap_config_t trap_config_get;
    SHR_FUNC_INIT_VARS(unit);

    /** Get the action profile configuration for an ETPP trap */
    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get on Trap: %s \n %s%s%s",
                dnx_rx_trap_type_map[trap_type].trap_name, EMPTY, EMPTY, EMPTY);

    /** Get trap action */
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id, &trap_config_get));

    /** Compare configuration */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->cpu_trap_gport, &trap_config_get.cpu_trap_gport,
                     sizeof(trap_config_get.cpu_trap_gport), "cpu_trap_gport"));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->is_recycle_high_priority, &trap_config_get.is_recycle_high_priority,
                     sizeof(trap_config_get.is_recycle_high_priority), "is_recycle_high_priority"));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of ETPP OAM traps.
 *   Sets a ETPP OAM trap
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
appl_dnx_rx_trap_etpp_oam(
    int unit)
{
    int trap_type_index;
    int oam_trap_id;
    bcm_rx_trap_config_t trap_config;
    bcm_error_t result = BCM_E_NONE;
    bcm_rx_trap_t oam_trap_type[CTEST_DNX_RX_TRAP_ETPP_OAM_NOF] = { bcmRxTrapEgTxOamUpMEPDest2 };
    /** OAM traps: bcmRxTrapEgTxOamUpMEPOamp, bcmRxTrapEgTxOamUpMEPDest1, bcmRxTrapEgTxOamReflector, 
             bcmRxTrapEgTxOamLevel, bcmRxTrapEgTxOamPassive are configured as part of src/appl/reference/dnx/appl_ref_oam_init.c */

    for (trap_type_index = 0; trap_type_index < CTEST_DNX_RX_TRAP_ETPP_OAM_NOF; trap_type_index++)
    {
        bcm_rx_trap_config_t_init(&trap_config);
        ctest_dnx_rx_trap_etpp_oam_fill(unit, &trap_config);
        result = ctest_dnx_rx_trap_create_and_set(unit, oam_trap_type[trap_type_index], &oam_trap_id, &trap_config);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "Creating and setting the trap failed for trap_type(%d). Error:%d (%s)\n"),
                       oam_trap_type[trap_type_index], result, bcm_errmsg(result)));
            return result;
        }

        result =
            ctest_dnx_rx_trap_etpp_oam_get_and_compare(unit, oam_trap_type[trap_type_index], oam_trap_id, &trap_config);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "Get and compare failed for trap_type(%d). Error:%d (%s)\n"),
                       oam_trap_type[trap_type_index], result, bcm_errmsg(result)));
            return result;
        }
    }

    for (trap_type_index = 0; trap_type_index < CTEST_DNX_RX_TRAP_ETPP_OAM_NOF; trap_type_index++)
    {
        result = bcm_rx_trap_type_get(unit, 0, oam_trap_type[trap_type_index], &oam_trap_id);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_type_get failed for trap_type(%d). Error:%d (%s)\n"),
                       oam_trap_type[trap_type_index], result, bcm_errmsg(result)));
            return result;
        }

        result = bcm_rx_trap_type_destroy(unit, oam_trap_id);
        if (BCM_FAILURE(result))
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "bcm_rx_trap_type_destroy failed for trap_type(%d). Error:%d (%s)\n"),
                       oam_trap_type[trap_type_index], result, bcm_errmsg(result)));
            return result;
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ETPP OAM TRAP CONFIGURATION TEST END.\n")));
    return result;

}

shr_error_e
sh_dnx_rx_trap_etpp_oam_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp_oam(unit));

exit:
    SHR_FUNC_EXIT;
}
