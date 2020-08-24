/*
 * ctest_dnx_rx_trap_egress_exhaustive.c
 *
 *  Created on: April 25, 2018
 *      Author: sm888907
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
#include "ctest_dnx_rx_trap_etpp.h"
#include "ctest_dnx_rx_trap_egress_exhaustive.h"
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

/** EGRESS_EXHAUSTIVE trap test details */
sh_sand_man_t sh_dnx_rx_trap_egress_exhaustive_man = {
    "EGRESS_EXHAUSTIVE traps configuration testing",
    "Create an EGRESS_EXHAUSTIVE Userdefined trap, then set its action profile according to"
        "an EGRESS_EXHAUSTIVE Application trap." "Get the trap action profile and compare it."
        "Clear the action profile"
};

/**
 * \brief
 *   List of tests for EGRESS_EXHAUSTIVE traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_egress_exhaustive_tests[] = {
    {"egress_exhaustive", "", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

 /**
 * \brief
 *   This function checks the configuration of ERPP traps.
 *   Sets a UserDefined EGRESS_EXHAUSTIVE trap
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
appl_dnx_rx_trap_egress_exhaustive(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_profiles = dnx_data_trap.etpp.nof_etpp_user_configurable_profiles_get(unit);
    uint8 glem_not_found_enabled = dnx_data_trap.erpp.feature_get(unit, dnx_data_trap_erpp_glem_not_found_trap);
    int trap_iter = 0, trap_strength = 7, snoop_strength = 0;
    int trap_id;
    int *erpp_trap_id_p = NULL;
    int *etpp_trap_id_p = NULL;
    char trap_name[DBAL_MAX_STRING_LENGTH] = "";
    dnx_rx_trap_map_type_t appl_trap_info;
    bcm_gport_t trap_gport;
    bcm_rx_trap_config_t trap_config;
    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(erpp_trap_id_p, sizeof(*erpp_trap_id_p) * nof_profiles, "erpp_trap_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC(etpp_trap_id_p, sizeof(*etpp_trap_id_p) * nof_profiles, "etpp_trap_id_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    /*
     * ---------------------------------------- Configure ERPP and ETPP action profiles ----------------------------------
     */
    for (trap_iter = 0; trap_iter < nof_profiles; trap_iter++)
    {
        bcm_rx_trap_config_t_init(&trap_config);
        ctest_dnx_rx_trap_etpp_fill(unit, &trap_config);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &etpp_trap_id_p[trap_iter],
                                                         &trap_config));

        bcm_rx_trap_config_t_init(&trap_config);
        ctest_dnx_rx_trap_erpp_fill(unit, &trap_config);
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgUserDefine, &erpp_trap_id_p[trap_iter],
                                                         &trap_config));
    }

    /*
     * ---------------------------------------- ERPP and ETPP Application Traps ----------------------------------
     */
    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        if ((glem_not_found_enabled == FALSE) &&
            ((trap_iter == bcmRxTrapEgGlemPpTrap) || (trap_iter == bcmRxTrapEgGlemNonePpTrap)))
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));

        if (appl_trap_info.valid && trap_iter != bcmRxTrapEgUnknownDa)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_type_to_name(unit, trap_iter, trap_name));
            LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "Setting trap bcmRxTrap%s.\n"), trap_name));
            if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ERPP)
            {
                trap_id = erpp_trap_id_p[sal_rand() % nof_profiles];
            }
            else
            {
                trap_id = etpp_trap_id_p[sal_rand() % nof_profiles];
            }

            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.valid)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    /*
     * ---------------------------------------- Destroy ERPP and ETPP action profiles ----------------------------------
     */
    for (trap_iter = 0; trap_iter < nof_profiles; trap_iter++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, etpp_trap_id_p[trap_iter]));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, erpp_trap_id_p[trap_iter]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "EGRESS_EXHAUSTIVE TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FREE(erpp_trap_id_p);
    SHR_FREE(etpp_trap_id_p);
    SHR_FUNC_EXIT;
}

/*
 *  See ctest_dnx_rx_trap_egress_exhaustive.h for info
 */
shr_error_e
sh_dnx_rx_trap_egress_exhaustive_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_egress_exhaustive(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
