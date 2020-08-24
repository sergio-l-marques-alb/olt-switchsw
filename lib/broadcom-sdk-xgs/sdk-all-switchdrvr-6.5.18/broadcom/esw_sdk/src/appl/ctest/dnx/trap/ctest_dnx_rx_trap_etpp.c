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
#include <sal/appl/sal.h>
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
    {"etpp_appl_positive", "type=pos mode=appl", CTEST_POSTCOMMIT}
    ,
    {"etpp_appl_negative", "type=neg mode=appl", CTEST_POSTCOMMIT}
    ,
    {"etpp_actions_positive", "type=pos mode=act", CTEST_POSTCOMMIT}
    ,
    {"etpp_actions_negative", "type=neg mode=act", CTEST_POSTCOMMIT}
    ,
    {"fwd_recycle_cmd_positive", "type=pos mode=rcy_cmd", CTEST_POSTCOMMIT}
    ,
    {"fwd_recycle_cmd_negative", "type=neg mode=rcy_cmd", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief
 *   Options list for 'etpp' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_etpp_options[] = {
    {"type", SAL_FIELD_TYPE_STR, "Type of test, options: pos,neg", "pos"}
    ,
    {"mode", SAL_FIELD_TYPE_STR, "Mode of test, options: appl,act,rcy_cmd", "appl"}
    ,
    {NULL}      /* End of options list - must be last. */
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
    trap_config_p->pp_drop_reason = (sal_rand() % 3);
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
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

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
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                    (unit, &trap_config_p->pp_drop_reason, &trap_config_get.pp_drop_reason,
                     sizeof(trap_config_get.pp_drop_reason), "pp_drop_reason"));

    SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, app_trap_type, &appl_trap_info));
    LOG_INFO_EX(BSL_LOG_MODULE, "Action profile Get and compare was done on bcmRxTrap%s \n %s%s%s",
                appl_trap_info.trap_name, EMPTY, EMPTY, EMPTY);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks Fwd Recycle command configuration.
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
appl_dnx_rx_trap_fwd_rcy_cmd_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int new_rcy_cmd, trap_id, rcy_cmd, default_rcy_cmd = 0;
    dnx_rx_trap_recycle_cmd_config_t rcy_cmd_config = { 0 }, rcy_cmd_config_get =
    {
    0};
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE FORWARD RECYCLE COMMAND CONFIGURATION TEST START.\n%s%s%s%s", EMPTY,
                EMPTY, EMPTY, EMPTY);

    for (rcy_cmd = 11; rcy_cmd < 15; rcy_cmd++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

        rcy_cmd_config.recycle_strength = 5;
        rcy_cmd_config.ingress_trap_id = trap_id;
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set(unit, 0, &rcy_cmd_config, &new_rcy_cmd));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &rcy_cmd, &new_rcy_cmd, sizeof(int), "fwd_recycle_cmd"));

        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_hw_get(unit, BCM_CORE_ALL, rcy_cmd, &rcy_cmd_config_get));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &rcy_cmd_config, &rcy_cmd_config_get,
                                                         sizeof(dnx_rx_trap_recycle_cmd_config_t),
                                                         "recycle_cmd_config"));
    }

    for (rcy_cmd = 11; rcy_cmd < 15; rcy_cmd++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_hw_get(unit, BCM_CORE_ALL, rcy_cmd, &rcy_cmd_config_get));

        rcy_cmd_config.recycle_strength = 0;
        rcy_cmd_config.ingress_trap_id = 0;
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set(unit, rcy_cmd, &rcy_cmd_config, &new_rcy_cmd));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &default_rcy_cmd, &new_rcy_cmd, sizeof(int), "fwd_recycle_cmd"));

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, rcy_cmd_config_get.ingress_trap_id));
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE FORWARD RECYCLE COMMAND CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of ETPP traps.
 *   Sets a UserDefined ETPP trap
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
appl_dnx_rx_trap_etpp_actions_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id[8];
    int fwd_action_profile = 0;
    bcm_rx_trap_config_t trap_config[8], trap_config_get;
    uint8 nof_etpp_trap_profiles = dnx_data_trap.etpp.nof_etpp_user_configurable_profiles_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE ETPP TRAP ACTIONS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

    for (fwd_action_profile = 0; fwd_action_profile < (nof_etpp_trap_profiles - 1); fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id[fwd_action_profile]));
    }

    for (fwd_action_profile = 0; fwd_action_profile < (nof_etpp_trap_profiles - 1); fwd_action_profile++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id[fwd_action_profile]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_set twice on the same trap_id             \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id[0]));
    bcm_rx_trap_config_t_init(&trap_config[0]);
    ctest_dnx_rx_trap_etpp_fill(unit, &trap_config[0]);

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
        ctest_dnx_rx_trap_etpp_fill(unit, &trap_config[fwd_action_profile]);
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

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "          bcm_rx_trap_get without init struct                 \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, trap_id[0], &trap_config[7]));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id[0]));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          bcm_rx_trap_get DEFAULT profile                 \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT, &trap_config[7]));

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "          bcm_rx_trap_get DROP profile                 \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    bcm_rx_trap_config_t_init(&trap_config[0]);
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, &trap_config[0]));

    if (trap_config[0].dest_port != BCM_GPORT_BLACK_HOLE)
    {
        SHR_ERR_EXIT(BCM_E_PARAM,
                     "The default drop config is not retreived correctly. Dest_port = %d, should be BCM_GPORT_BLACK_HOLE",
                     trap_config[0].dest_port);
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "POSITIVE ETPP TRAP ACTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                EMPTY);

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
appl_dnx_rx_trap_etpp_appl_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, trap_strength = 7, snoop_strength = 0;
    int trap_iter = 0, config_iter;
    bcm_gport_t trap_gport, get_trap_gport;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t bcm_trap_type;
    dnx_rx_trap_map_type_t appl_trap_info;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&appl_trap_info, 0, sizeof(dnx_rx_trap_map_type_t));

    /*
     * ---------------------------------------- ETPP Application Traps ----------------------------------
     */
    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_etpp_fill(unit, &trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &trap_id, &trap_config));
    for (config_iter = 0; config_iter < 2; config_iter++)
    {
        for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
        {
            SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
            if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ETPP)
            {
                /** Set the action profile configuration for an ETPP trap */
                BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
                SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));

                SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_etpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));

                /** Clear the action profile configuration for an ETPP trap */
                SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
            }
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ETPP)
        {
            /** Set the action profile configuration for an ETPP trap */
            BCM_GPORT_TRAP_SET(trap_gport, trap_id, trap_strength, snoop_strength);
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_set(unit, trap_iter, trap_gport));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ETPP)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_etpp_get_and_compare(unit, trap_iter, &trap_gport, &trap_config));
        }
    }

    for (trap_iter = 0; trap_iter < bcmRxTrapCount; trap_iter++)
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_appl_trap_map_info_get(unit, trap_iter, &appl_trap_info));
        if (appl_trap_info.trap_block == DNX_RX_TRAP_BLOCK_ETPP)
        {
             /** Clear the action profile configuration for an ETPP trap */
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_profile_clear(unit, trap_iter));
        }
    }

    /** Covering ETPP Trap type from ID get case */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, trap_id, &bcm_trap_type));

    /** Destroy the ETPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    /** Set the action profile configuration for an ETPP trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &trap_id, &trap_config));
    BCM_GPORT_TRAP_SET(trap_gport, trap_id, 0, 7);
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, trap_gport));

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_get(unit, bcmRxTrapEgTxLatency, &get_trap_gport));

    if (get_trap_gport != trap_gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Get trap gport(0x%08X) is not equal to set trap gport(0x%08X)!", get_trap_gport,
                     trap_gport);
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_clear(unit, bcmRxTrapEgTxLatency));

        /** Destroy the ETPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "ETPP TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the error messages
 *   and codes of Fwd Recycle command configuration.
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
appl_dnx_rx_trap_fwd_rcy_cmd_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int new_rcy_cmd, rv, trap_id, rcy_cmd, default_rcy_cmd = 0;
    dnx_rx_trap_recycle_cmd_config_t rcy_cmd_config = { 0 }, rcy_cmd_config_get =
    {
    0};
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE FORWARD RECYCLE COMMAND CONFIGURATION TEST START.\n%s%s%s%s", EMPTY,
                EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "               Check Recycle command resource full           \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    for (rcy_cmd = 11; rcy_cmd < 15; rcy_cmd++)
    {
        /** Set strength as recycle cmd value since they are int the same range of values */
        rcy_cmd_config.recycle_strength = rcy_cmd;
        rcy_cmd_config.ingress_trap_id = trap_id;
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set(unit, 0, &rcy_cmd_config, &new_rcy_cmd));
    }

    rcy_cmd_config.recycle_strength = 3;
    rcy_cmd_config.ingress_trap_id = trap_id;
    rv = dnx_rx_trap_etpp_recycle_cmd_profile_set(unit, 0, &rcy_cmd_config, &new_rcy_cmd);
    if (rv != BCM_E_FULL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_FULL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    rv = dnx_rx_trap_etpp_recycle_cmd_hw_get(unit, BCM_CORE_ALL, 16, &rcy_cmd_config_get);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    for (rcy_cmd = 11; rcy_cmd < 15; rcy_cmd++)
    {
        rcy_cmd_config.recycle_strength = 0;
        rcy_cmd_config.ingress_trap_id = 0;
        SHR_IF_ERR_EXIT(dnx_rx_trap_etpp_recycle_cmd_profile_set(unit, rcy_cmd, &rcy_cmd_config, &new_rcy_cmd));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values
                        (unit, &default_rcy_cmd, &new_rcy_cmd, sizeof(int), "fwd_recycle_cmd"));
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE FORWARD RECYCLE COMMAND CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the error messages
 *   and codes of all ETPP trap actions.
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
appl_dnx_rx_trap_etpp_actions_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int trap_id, rv, oam_trap_id;
    bcm_rx_trap_config_t trap_config, trap_config_get;
    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ETPP TRAP ACTIONS CONFIGURATION TEST START.\n%s%s%s%s", EMPTY, EMPTY,
                EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "      Supplying cpu_trap_gport not encoded as trap gport     \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.cpu_trap_gport = 0;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "       Supplying cpu_trap_gport with non ingress trap id     \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, trap_id, 7, 0);
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "      Supplying cpu_trap_gport with not allocated trap id    \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    BCM_GPORT_TRAP_SET(trap_config.cpu_trap_gport, 0, 7, 0);
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "        Supplying is_recycle_high_priority out of range      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_high_priority = 2;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           Supplying is_recycle_crop_pkt out of range        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_crop_pkt = 3;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "         Supplying is_recycle_append_ftmh out of range       \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_append_ftmh = 4;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "              Supplying pp_drop_reason out of range          \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.pp_drop_reason = 4;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "         Supplying stamped_trap_code for non oam trap        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stamped_trap_code = 4;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxOamLevel, &oam_trap_id));
    SHR_IF_ERR_EXIT(bcm_rx_trap_get(unit, oam_trap_id, &trap_config_get));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "     Supplying stamped_trap_code with non ingress trap id    \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.stamped_trap_code = trap_id;
    rv = bcm_rx_trap_set(unit, oam_trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, oam_trap_id, &trap_config_get));

    /** Destroy the ETPP UserDefined trap */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying default trap ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT;
    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "  Supplying default drop trap ID \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;
    bcm_rx_trap_config_t_init(&trap_config);

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO_EX(BSL_LS_APPL_COMMON, "NEGATIVE ETPP TRAP ACTIONS CONFIGURATION TEST END.\n%s%s%s%s", EMPTY, EMPTY, EMPTY,
                EMPTY);

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
appl_dnx_rx_trap_etpp_appl_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int etpp_trap_id, erpp_trap_id, trap_strength = 7, snoop_strength = 0;
    int rv = BCM_E_NONE, trap_id;
    bcm_gport_t trap_gport, erpp_gport, etpp_gport, zero_str_gport;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t bcm_trap_type;
    SHR_FUNC_INIT_VARS(unit);

    
    /*
     * ---------------------------------------- ETPP Application Traps ----------------------------------
     */
    bcm_rx_trap_config_t_init(&trap_config);
    ctest_dnx_rx_trap_etpp_fill(unit, &trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &etpp_trap_id, &trap_config));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(trap_gport, etpp_trap_id, trap_strength, snoop_strength);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Ingress trap ID to bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapLinkLayerHeaderSizeErr, trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap ID out of range to bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = bcm_rx_trap_action_profile_set(unit, 0, (bcmRxTrapCount), trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap ID out of range to bcm_rx_trap_action_profile_get \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = bcm_rx_trap_action_profile_get(unit, (bcmRxTrapCount - 1), &trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap ID out of range to bcm_rx_trap_action_profile_clear \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = bcm_rx_trap_action_profile_clear(unit, (bcmRxTrapCount - 1));
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_clear should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "SUPPLYING ERPP Appl trap ID and ETPP FWD_ACTION_PROFILE bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgIpv4Ttl0, trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U
              (unit, "SUPPLYING ETPP Appl trap ID and ERPP FWD_ACTION_PROFILE bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgUserDefine, &erpp_trap_id, &trap_config));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(erpp_gport, erpp_trap_id, trap_strength, snoop_strength);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, erpp_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap and Snoop Strengths 0 bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    /** Set the action profile configuration for an ETPP trap with 0 strengths */
    BCM_GPORT_TRAP_SET(zero_str_gport, etpp_trap_id, 0, 0);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, zero_str_gport);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_CONFIG! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Non-allocated FWD_ACTION_INDEX bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(trap_gport, etpp_trap_id + 2, trap_strength, snoop_strength);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap and Snoop Strengths > 0 bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(trap_gport, etpp_trap_id, 5, 5);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Snoop Profile not allocated bcm_rx_trap_action_profile_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(trap_gport, etpp_trap_id + 2, 0, 5);

    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, trap_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_action_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                   ETPP Application trap type get \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id);
    if (rv != BCM_E_CONFIG)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_get should fail with BCM_E_CONFIG! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "           ETPP Trap type from id get non-existant value    \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "------------------------------------------------------------\n")));

    /** Covering ETPP Trap type from ID get case */
    rv = bcm_rx_trap_type_from_id_get(unit, 0, 0xE09, &bcm_trap_type);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_from_id_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    /** Destroy traps */
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, etpp_trap_id));
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_destroy(unit, erpp_trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "Destroying action profile which is attached to 2 different Appl traps. \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_config_t_init(&trap_config);
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_create_and_set(unit, bcmRxTrapEgTxUserDefine, &etpp_trap_id, &trap_config));

    /** Set the action profile configuration for an ETPP trap */
    BCM_GPORT_TRAP_SET(etpp_gport, etpp_trap_id, trap_strength, snoop_strength);
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxLatency, etpp_gport));
    SHR_IF_ERR_EXIT(bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxDiscardFrameTypeFilter, etpp_gport));

    rv = bcm_rx_trap_type_destroy(unit, etpp_trap_id);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_type_destroy should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUCCESS! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

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
    char *test_type;
    char *test_mode;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("type", test_type);
    SH_SAND_GET_STR("mode", test_mode);

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAP_ETPP_TEST_TYPE_POSITIVE) == 0)
    {
        if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAP_ETPP_TEST_MODE_RCY_CMD) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_fwd_rcy_cmd_pos(unit, args, sand_control));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAP_ETPP_TEST_MODE_ACTIONS) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp_actions_pos(unit, args, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp_appl_pos(unit, args, sand_control));
        }
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAP_ETPP_TEST_TYPE_NEGATIVE) == 0)
    {
        if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAP_ETPP_TEST_MODE_RCY_CMD) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_fwd_rcy_cmd_neg(unit, args, sand_control));
        }
        else if (sal_strcasecmp(test_mode, CTEST_DNX_RX_TRAP_ETPP_TEST_MODE_ACTIONS) == 0)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp_actions_neg(unit, args, sand_control));
        }
        else
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_etpp_appl_neg(unit, args, sand_control));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
