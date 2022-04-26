/*
 * ctest_dnx_rx_trap_svtag.c
 *
 *  Created on: Jan 5, 2018
 *      Author: dp889757
 */
/*
  * Include files.
  * {
  */
#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/rx/rx.h>
#include <bcm/rx.h>
#include <bcm/mpls.h>
#include <bcm/tunnel.h>
#include <shared/bsl.h>
#include <sal/appl/sal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include "ctest_dnx_rx_trap.h"
#include "ctest_dnx_rx_trap_svtag.h"
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

/** Ingress trap test details */
sh_sand_man_t sh_dnx_rx_trap_svtag_man = {
    "svtag traps configuration testing",
    "Create an ETPP UserDefined trap, create svtag config, then get the svtag config and compare values."
};

/**
 * \brief
 *   Options list for 'svtag' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_svtag_options[] = {
    /*
     * Name 
     *//*
     * Type 
     *//*
     * Description 
     *//*
     * Default 
     */
    {CTEST_DNX_RX_TRAPS_SVTAG_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (pos, neg, exh)", "pos"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for svtag Traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_svtag_tests[] = {
    {"svtag_pos", "type=pos", CTEST_POSTCOMMIT}
    ,
    {"svtag_neg", "type=neg ", CTEST_POSTCOMMIT}
    ,
    {"svtag_exh", "type=exh ", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief
 *   This function recevies a SET MTU struct and
 *   does a GET on the same key properties,
 *   in order to compare the SET and GET values
 *
 * \param [in] unit             - The unit number.
 * \param [in] set_mtu_config   - MTU Struct to be compared.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_rx_trap_svtag_get_compare(
    int unit,
    bcm_rx_trap_svtag_key_t * set_svtag_value,
    int sci,
    int err_code,
    bcm_gport_t set_action_gport)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_gport_t get_action_gport;

    SHR_FUNC_INIT_VARS(unit);

    key.type = bcmSwitchSvtagSciProfile;
    key.index = sci;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &info));

    if (info.value != set_svtag_value->sci_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET SCI Profile value %d is not equal to SET SCI Profile Value %d",
                     info.value, set_svtag_value->sci_profile);
    }

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = err_code;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &info));

    if (info.value != set_svtag_value->error_code_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET Err_code Profile value %d is not equal to SET Err_code Profile Value %d",
                     info.value, set_svtag_value->error_code_profile);
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_get(unit, set_svtag_value, &get_action_gport));

    if (get_action_gport != set_action_gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET action_gport value %d is not equal to SET action_gport  Value %d",
                     get_action_gport, set_action_gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of svtag traps.
 *   Create Ingress UserDefined Trap
 *   Set a svtag configuration
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
bcm_error_t
appl_dnx_rx_trap_svtag_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    bcm_rx_trap_config_t trap_config;
    bcm_gport_t action_gport;
    int sci = 0xA, sci_profile = 1;
    bcm_switch_svtag_err_code_t err_code = bcmSwitchSvtagErrCodeUntagCtrlPortDisabledPkt;
    int err_code_profile = 3;
    int trap_id;
    bcm_rx_trap_svtag_key_t svtag_trap_key;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_COLOR;
    trap_config.color = 2;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 0);

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);

    key.type = bcmSwitchSvtagSciProfile;
    key.index = sci;

    info.value = sci_profile;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = err_code;

    info.value = err_code_profile;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    svtag_trap_key.sci_profile = sci_profile;
    svtag_trap_key.error_code_profile = err_code_profile;
    svtag_trap_key.pkt_type = bcmSvtagPktTypeNonKayMgmt;
    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_svtag_get_compare(unit, &svtag_trap_key, sci, err_code, action_gport));

    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_set(unit, &svtag_trap_key, BCM_GPORT_INVALID));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                SVTAG Positive scenario SUCCESS \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of MTU traps.
 *   Create ETPP UserDefined Trap
 *   Set an MTU configuration
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
bcm_error_t
appl_dnx_rx_trap_svtag_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_gport_t action_gport;
    int trap_id;
    bcm_rx_trap_svtag_key_t svtag_trap_key;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));
    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 0);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING 11b to bcm_switch_control_key_t for SCI \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagSciProfile;
    key.index = 0x7FF;

    info.value = 1;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING value > 2b to bcm_switch_control_info_t SCI \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagSciProfile;
    key.index = 0xFF;

    info.value = 4;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING 9b to bcm_switch_control_key_t for ERR_CODE \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = 0x1FF;

    info.value = 1;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING value > 2b to bcm_switch_control_info_t ERR_CODE\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = 0x7F;

    info.value = 4;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "      Testing for bcm_switch_control_indexed_get START!      \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING 11b to bcm_switch_control_key_t for SCI \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagSciProfile;
    key.index = 0x7FF;

    rv = bcm_switch_control_indexed_get(unit, key, &info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING 9b to bcm_switch_control_key_t for ERR_CODE \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchSvtagErrCodeProfile;
    key.index = 0x1FF;

    rv = bcm_switch_control_indexed_get(unit, key, &info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_svtag_key_t TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid flags to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.flags = 5;
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Invalid Err_code_profile to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.error_code_profile = 5;
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Invalid SCI_profile to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.sci_profile = 5;
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Invalid pkt_type to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = 5;
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Non-allocated UD trap to bcm_rx_trap_svtag_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = bcmSvtagPktTypeKayMgmt;
    svtag_trap_key.sci_profile = 2;
    svtag_trap_key.error_code_profile = 3;
    BCM_GPORT_TRAP_SET(action_gport, 212, 15, 0);
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Zero Strengths to bcm_rx_trap_svtag_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = bcmSvtagPktTypeKayMgmt;
    svtag_trap_key.sci_profile = 2;
    svtag_trap_key.error_code_profile = 3;
    BCM_GPORT_TRAP_SET(action_gport, trap_id, 0, 0);
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Both strengths set to bcm_rx_trap_svtag_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = bcmSvtagPktTypeKayMgmt;
    svtag_trap_key.sci_profile = 2;
    svtag_trap_key.error_code_profile = 3;
    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 14);
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Non-trap gport to bcm_rx_trap_svtag_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = bcmSvtagPktTypeKayMgmt;
    svtag_trap_key.sci_profile = 2;
    svtag_trap_key.error_code_profile = 3;
    rv = bcm_rx_trap_svtag_set(unit, &svtag_trap_key, 0x150);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_svtag_get TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid flags to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.flags = 5;
    rv = bcm_rx_trap_svtag_get(unit, &svtag_trap_key, &action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Invalid Err_code_profile to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.error_code_profile = 5;
    rv = bcm_rx_trap_svtag_get(unit, &svtag_trap_key, &action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Invalid SCI_profile to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.sci_profile = 5;
    rv = bcm_rx_trap_svtag_get(unit, &svtag_trap_key, &action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Invalid pkt_type to bcm_rx_trap_svtag_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
    svtag_trap_key.pkt_type = 5;
    rv = bcm_rx_trap_svtag_get(unit, &svtag_trap_key, &action_gport);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_svtag_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, " ----------------     SUCCESS     ----------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    rv = BCM_E_NONE;
    return rv;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks the configuration of MTU traps.
 *   Create ETPP UserDefined Trap
 *   Set an MTU configuration
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
bcm_error_t
appl_dnx_rx_trap_svtag_exh(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_gport_t action_gport;
    int trap_id;
    bcm_rx_trap_svtag_key_t svtag_trap_key;
    int ii;
    int pkt_type_idx, sci_idx, err_code_idx, flag_idx;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));
    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 0);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                    Exhaustive Start                         \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (ii = 0; ii < dnx_data_trap.ingress.nof_scis_get(unit); ii++)
    {
        key.type = bcmSwitchSvtagSciProfile;
        key.index = ii;

        info.value = 1;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
    }

    for (err_code_idx = bcmSwitchSvtagErrCodeNone; err_code_idx < bcmSwitchSvtagErrCodeCount; err_code_idx++)
    {
        key.type = bcmSwitchSvtagErrCodeProfile;
        key.index = err_code_idx;

        info.value = 2;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
    }

    for (flag_idx = 0; flag_idx < 2; flag_idx++)
    {
        bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
        svtag_trap_key.flags = flag_idx;

        for (pkt_type_idx = 0; pkt_type_idx <= bcmSvtagPktTypeKayMgmt; pkt_type_idx++)
        {
            svtag_trap_key.pkt_type = pkt_type_idx;

            for (err_code_idx = 0; err_code_idx < dnx_data_trap.ingress.nof_error_code_profiles_get(unit);
                 err_code_idx++)
            {
                svtag_trap_key.error_code_profile = err_code_idx;

                for (sci_idx = 0; sci_idx < dnx_data_trap.ingress.nof_sci_profiles_get(unit); sci_idx++)
                {
                    svtag_trap_key.sci_profile = sci_idx;
                    BCM_GPORT_TRAP_SET(action_gport, trap_id, 15, 0);
                    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_set(unit, &svtag_trap_key, action_gport));
                }
            }
        }
    }
    SHR_EXIT();
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "----------------    Clean-up     -------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (ii = 0; ii < dnx_data_trap.ingress.nof_scis_get(unit); ii++)
    {
        key.type = bcmSwitchSvtagSciProfile;
        key.index = ii;

        info.value = 0;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
    }

    for (err_code_idx = bcmSwitchSvtagErrCodeNone; err_code_idx < bcmSwitchSvtagErrCodeCount; err_code_idx++)
    {
        key.type = bcmSwitchSvtagErrCodeProfile;
        key.index = err_code_idx;

        info.value = 0;

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
    }

    for (flag_idx = 0; flag_idx < 2; flag_idx++)
    {
        bcm_rx_trap_svtag_key_t_init(&svtag_trap_key);
        svtag_trap_key.flags = flag_idx;

        for (pkt_type_idx = 0; pkt_type_idx <= bcmSvtagPktTypeKayMgmt; pkt_type_idx++)
        {
            svtag_trap_key.pkt_type = pkt_type_idx;

            for (err_code_idx = 0; err_code_idx < dnx_data_trap.ingress.nof_error_code_profiles_get(unit);
                 err_code_idx++)
            {
                svtag_trap_key.error_code_profile = err_code_idx;

                for (sci_idx = 0; sci_idx < dnx_data_trap.ingress.nof_sci_profiles_get(unit); sci_idx++)
                {
                    svtag_trap_key.sci_profile = sci_idx;
                    SHR_IF_ERR_EXIT(bcm_rx_trap_svtag_set(unit, &svtag_trap_key, BCM_GPORT_INVALID));
                }
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "----------------    SUCCESS     -------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_svtag_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;
    int svtag_trap_en;

    SHR_FUNC_INIT_VARS(unit);

    svtag_trap_en = dnx_data_trap.ingress.feature_get(unit, dnx_data_trap_ingress_svtag_trap_en);
    if (svtag_trap_en == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "SVTag feature is not supported on this device! \n");
    }

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_SVTAG_TEST_TYPE, test_type);

    if (sal_strncasecmp
        (test_type, CTEST_DNX_RX_TRAPS_SVTAG_TEST_TYPE_POSITIVE, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_svtag_pos(unit, args, sand_control));

        SHR_EXIT();
    }
    else if (sal_strncasecmp
             (test_type, CTEST_DNX_RX_TRAPS_SVTAG_TEST_TYPE_NEGATIVE,
              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_svtag_neg(unit, args, sand_control));

        SHR_EXIT();
    }
    else if (sal_strncasecmp
             (test_type, CTEST_DNX_RX_TRAPS_SVTAG_TEST_TYPE_EXHAUSTIVE,
              SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_svtag_exh(unit, args, sand_control));

        SHR_EXIT();
    }
    else
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U
                  (unit, "Wrong parameter supplied to 'type', valid values are 'pos' , 'neg' and 'exh'! \n")));
    }
exit:
    SHR_FUNC_EXIT;
}
