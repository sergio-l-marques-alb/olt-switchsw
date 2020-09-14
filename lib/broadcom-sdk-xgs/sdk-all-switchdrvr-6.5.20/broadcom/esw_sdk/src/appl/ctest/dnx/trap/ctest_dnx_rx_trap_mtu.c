/*
 * ctest_dnx_rx_trap_mtu.c
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
#include "ctest_dnx_rx_trap_mtu.h"
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
sh_sand_man_t sh_dnx_rx_trap_mtu_man = {
    "MTU traps configuration testing",
    "Create an ETPP UserDefined trap, create MTU config, then get the MTU config and compare values."
};

/**
 * \brief
 *   Options list for 'mtu' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_mtu_options[] = {
    /*
     * Name 
     *//*
     * Type 
     *//*
     * Description 
     *//*
     * Default 
     */
    {CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_MODE, SAL_FIELD_TYPE_STR, "Mode of test (port,lif or rif)", "port"}
    ,
    {CTEST_DNX_RX_TRAPS_MTU_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (pos, neg, exh)", "pos"}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for MTU Traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_mtu_tests[] = {
    {"mtu_pos_port", "type=pos mode=port", CTEST_POSTCOMMIT}
    ,
    {"mtu_pos_lif", "type=pos mode=lif", CTEST_POSTCOMMIT}
    ,
    {"mtu_pos_rif", "type=pos mode=rif", CTEST_POSTCOMMIT}
    ,
    {"mtu_neg", "type=neg ", CTEST_POSTCOMMIT}
    ,
    {"mtu_exh", "type=exh ", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief
 *   This function fills in the MTU struct and sets it.
 *
 * \param [in] unit             - The unit number.
 * \param [out] set_mtu_config   - MTU Struct to be set.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_rx_trap_mtu_mtu_config(
    int unit,
    int trap_id,
    bcm_rx_mtu_profile_value_t * mtu_value,
    bcm_rx_mtu_profile_key_t * mtu_key,
    bcm_rx_mtu_config_t * mtu_config)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int flags = mtu_config->flags;

    SHR_FUNC_INIT_VARS(unit);

    if (trap_id != 0)
    {
        if (!(mtu_config->flags & BCM_RX_MTU_PORT))
        {

            key.type = bcmSwitchLinkLayerMtuFilter;
            if (mtu_config->flags & BCM_RX_MTU_LIF)
            {
                key.index = bcmFieldLayerTypeMpls;

                info.value = 3;
            }
            else
            {
                key.index = bcmFieldLayerTypeIp4;

                info.value = 1;
            }

            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
        }
        mtu_key->cmp_layer_type = info.value;
    }
    else
    {
        if (!(mtu_config->flags & BCM_RX_MTU_PORT))
        {
            key.type = bcmSwitchLinkLayerMtuFilter;
            if (mtu_config->flags & BCM_RX_MTU_LIF)
            {
                key.index = bcmFieldLayerTypeMpls;
            }
            else
            {
                key.index = bcmFieldLayerTypeIp4;
            }

            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &info));
        }
    }

    SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_set(unit, flags, mtu_key, mtu_value));

    SHR_IF_ERR_EXIT(bcm_rx_mtu_set(unit, mtu_config));

exit:
    SHR_FUNC_EXIT;
}

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
ctest_dnx_rx_trap_mtu_get_compare(
    int unit,
    bcm_rx_mtu_profile_value_t * set_mtu_value,
    bcm_rx_mtu_profile_key_t * set_mtu_key,
    bcm_rx_mtu_config_t * set_mtu_config)
{
    bcm_rx_mtu_config_t get_mtu_config;
    bcm_rx_mtu_profile_value_t get_mtu_value;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    /*
     * int flags = set_mtu_config->flags; for future use 
     */
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_mtu_profile_value_t_init(&get_mtu_value);

    get_mtu_config.flags = set_mtu_config->flags;
    get_mtu_config.gport = set_mtu_config->gport;
    get_mtu_config.intf = set_mtu_config->intf;

    if ((set_mtu_config->flags & BCM_RX_MTU_LIF) || (set_mtu_config->flags & BCM_RX_MTU_RIF))
    {
        key.type = bcmSwitchLinkLayerMtuFilter;
        if (set_mtu_config->flags & BCM_RX_MTU_LIF)
        {
            key.index = bcmFieldLayerTypeMpls;
        }
        else
        {
            key.index = bcmFieldLayerTypeIp4;
        }

        SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &info));
    }

    SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_get(unit, set_mtu_config->flags, set_mtu_key, &get_mtu_value));

    if (get_mtu_value.mtu_val != set_mtu_value->mtu_val)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET MTU value %d is not equal to SET MTU Value %d",
                     get_mtu_value.mtu_val, set_mtu_value->mtu_val);
    }

    if (get_mtu_value.trap_gport != set_mtu_value->trap_gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET MTU Trap GPORT 0x%08X is not equal to SET MTU Trap GPORT 0x%08X",
                     get_mtu_value.trap_gport, set_mtu_value->trap_gport);
    }

    SHR_IF_ERR_EXIT(bcm_rx_mtu_get(unit, &get_mtu_config));

    if (get_mtu_config.mtu_profile != set_mtu_config->mtu_profile)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET MTU Profile value %d is not equal to SET MTU Profile Value %d",
                     get_mtu_config.mtu_profile, set_mtu_config->mtu_profile);
    }

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
appl_dnx_rx_trap_mtu_pos(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_mtu_config_t mtu_config, cleanup_mtu_config;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;
    bcm_rx_trap_config_t trap_config;
    bcm_if_t rif = 0;
    bcm_gport_t lif = 0;
    int port = 201;
    int trap_id;
    int flags;
    int mtu_profile = 2, trap_strength = 15, snoop_strength = 0;
    int mtu_val = 3000;
    char *test_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_MODE, test_type);
    bcm_rx_mtu_config_t_init(&mtu_config);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    rif = 0;
    lif = 0;
    flags = 0;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_append_ftmh = TRUE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    BCM_GPORT_TRAP_SET(mtu_value.trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DEFAULT, 0, 0);

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_MODE_PORT) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "                       PORT MTU TEST                            \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

        flags = BCM_RX_MTU_PORT;

        mtu_config.flags = flags;
        mtu_config.gport = port;
        mtu_key.mtu_profile = mtu_profile;
        mtu_key.cmp_layer_type = 7;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_value, &mtu_key, &mtu_config));

        mtu_config.mtu_profile = mtu_profile;
        mtu_value.mtu_val = mtu_val;
        mtu_key.mtu_profile = mtu_profile;
        mtu_key.cmp_layer_type = 0;
        BCM_GPORT_TRAP_SET(mtu_value.trap_gport, trap_id, trap_strength, snoop_strength);

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_value, &mtu_key, &mtu_config));

        mtu_key.cmp_layer_type = 7;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_value, &mtu_key, &mtu_config));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_MODE_RIF) == 0)
    {

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "                    OUTRIF(IPv4 Tunnel) MTU TEST                \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

        flags = BCM_RX_MTU_RIF;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outrif_create_example(unit, &rif));

        sal_memset(&mtu_config, 0, sizeof(bcm_rx_mtu_config_t));

        BCM_GPORT_TRAP_SET(mtu_value.trap_gport, trap_id, trap_strength, snoop_strength);
        mtu_value.mtu_val = mtu_val;
        mtu_key.mtu_profile = mtu_profile;
        mtu_config.flags = flags;
        mtu_config.intf = rif;
        mtu_config.mtu_profile = mtu_profile;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_value, &mtu_key, &mtu_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_value, &mtu_key, &mtu_config));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_MODE_LIF) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                    OUTLIF(****) MTU TEST                \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

        flags = BCM_RX_MTU_LIF;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif));

        BCM_GPORT_TRAP_SET(mtu_value.trap_gport, trap_id, trap_strength, snoop_strength);
        mtu_key.mtu_profile = mtu_profile;
        mtu_value.mtu_val = mtu_val;
        mtu_config.flags = flags;
        mtu_config.gport = lif;
        mtu_config.mtu_profile = mtu_profile;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_value, &mtu_key, &mtu_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_value, &mtu_key, &mtu_config));
    }
    else
    {
        /** Clean-up */
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U
                  (unit, "Wrong parameter supplied to 'mode', valid values are 'port' , 'lif' and 'rif'! \n")));
        SHR_EXIT();

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "MTU TRAP CONFIGURATION TEST END.\n")));

    /** Clean-up */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));

    bcm_rx_mtu_config_t_init(&cleanup_mtu_config);

    mtu_value.trap_gport = BCM_GPORT_INVALID;
    cleanup_mtu_config.mtu_profile = 0;
    cleanup_mtu_config.flags = flags;
    cleanup_mtu_config.gport = mtu_config.gport;
    cleanup_mtu_config.intf = mtu_config.intf;

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, 0, &mtu_value, &mtu_key, &cleanup_mtu_config));

    if (lif != 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif));
    }
    else if (rif != 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_rif_destroy_example(unit, &rif, 0));
    }

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
appl_dnx_rx_trap_mtu_neg(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    bcm_rx_mtu_config_t mtu_config;
    int mtu_val = 17000;
    bcm_gport_t lif = 0;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;
    int trap_strength = 15, snoop_strength = 0;
    bcm_gport_t trap_gport;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING bcmFieldLayerTypeCount to bcm_switch_control_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = bcmFieldLayerTypeCount;

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
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING value > 3b to bcm_switch_control_info_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = bcmFieldLayerTypeMpls;

    info.value = 8;

    rv = bcm_switch_control_indexed_set(unit, key, info);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_switch_control_indexed_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"),
                   rv, bcm_errmsg(rv)));
        return rv;
    }

    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = bcmFieldLayerTypeCount;

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
             (BSL_META_U(unit, "--------------- bcm_rx_mtu_set TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid flags to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = 5;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Invalid Port to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_PORT;
    mtu_config.gport = 350;
    mtu_config.mtu = 200;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Non-Gport encoded LIF to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_LIF;
    mtu_config.gport = 350;
    mtu_config.mtu = 200;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Non-allocated LIF to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_LIF;
    mtu_config.gport = 0x18A022BB;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Non-allocated RIF to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = 241;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING MTU Profile out of range to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.mtu_profile = 15;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING FEC to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = 0x2000CCCE;
    rv = bcm_rx_mtu_set(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING MTU Profile out of range to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.mtu_profile = 15;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING FEC to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = 0x2000CCCE;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "--------------- bcm_rx_mtu_set TESTING ENDS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Compressed Layer Type out of range to bcm_rx_mtu_profile_key_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, trap_strength, snoop_strength);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 10;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Trap_ID out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, 0xC20, trap_strength, snoop_strength);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 6;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Non-allocated Trap_ID to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, 0xC04, trap_strength, snoop_strength);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Non-allocated Snoop ID to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, 0xC04, 0, 15);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Zero strength values to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 0, 0);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Trap and Snoop strength values to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 10, 7);

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Non-trap_gport encoding to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = 0x1234;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING MTU Value too big to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 3;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = mtu_val;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Flags out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 3;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, 6, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING MTU Profile out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 10;
    mtu_key.cmp_layer_type = 5;

    mtu_value.mtu_val = 1000;
    mtu_value.trap_gport = trap_gport;

    rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_mtu_profile_get TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Flags out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 2;
    mtu_key.cmp_layer_type = 5;

    rv = bcm_rx_mtu_profile_get(unit, 6, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING MTU Profile out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 10;
    mtu_key.cmp_layer_type = 5;

    rv = bcm_rx_mtu_profile_get(unit, 6, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING Compressed Layer Type out of range to bcm_rx_mtu_profile_value_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    mtu_key.mtu_profile = 3;
    mtu_key.cmp_layer_type = 12;

    rv = bcm_rx_mtu_profile_get(unit, 6, &mtu_key, &mtu_value);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_mtu_get TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid flags to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = 5;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid port to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_PORT;
    mtu_config.gport = 300;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_PORT)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_get should fail with BCM_E_PORT! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING non-allocated LIF to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_LIF;
    mtu_config.gport = 0x18A022BA;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING non-allocated RIF to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_mtu_config_t_init(&mtu_config);
    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = 93;
    rv = bcm_rx_mtu_get(unit, &mtu_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_mtu_get should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING invalid compressed layer type to bcm_rx_mtu_config_t \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif));

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
appl_dnx_rx_trap_mtu_exh(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv = BCM_E_NONE;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int mtu_val = 150, flags;
    int field_type, val, mtu_iter, compressed_layer_type = 0;
    bcm_gport_t trap_gport;
    bcm_rx_mtu_profile_value_t mtu_value;
    bcm_rx_mtu_profile_key_t mtu_key;

    SHR_FUNC_INIT_VARS(unit);
    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_value);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_switch_control_indexed_set Testing STARTS HERE!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (field_type = bcmFieldLayerTypeFirst; field_type < bcmFieldLayerTypeCount; field_type++)
    {
        for (val = 0; val < 7; val++)
        {
            key.type = bcmSwitchLinkLayerMtuFilter;
            key.index = bcmFieldLayerTypeMpls;
            info.value = val;

            SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_switch_control_indexed_set Testing ENDS HERE!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_rx_mtu_profile_set Testing STARTS HERE!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "MTU Profile + Compressed Layer Type Exhaustion Testing START!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    BCM_GPORT_TRAP_SET(trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 15, 0);

    for (mtu_iter = 0; mtu_iter < (dnx_data_trap.etpp.nof_mtu_profiles_get(unit) - 1); mtu_iter++)
    {
        for (compressed_layer_type = 0; compressed_layer_type < (dnx_data_trap.etpp.nof_mtu_profiles_get(unit) - 1);
             compressed_layer_type++)
        {

            mtu_key.cmp_layer_type = compressed_layer_type;
            mtu_key.mtu_profile = mtu_iter;

            mtu_value.mtu_val = mtu_val + mtu_iter;
            mtu_value.trap_gport = trap_gport;

            rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_value);
            if (rv != BCM_E_NONE && compressed_layer_type != 7)
            {

                return rv;
            }
            else if (rv == BCM_E_PARAM && compressed_layer_type == 7)
            {
                LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "LIF bcm_rx_mtu_set should fail with BCM_E_PARAM! \n")));
            }
        }

    }

    for (mtu_iter = 0; mtu_iter < 7; mtu_iter++)
    {
        mtu_key.mtu_profile = mtu_iter;

        mtu_value.mtu_val = mtu_val + mtu_iter;
        mtu_value.trap_gport = trap_gport;

        rv = bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_PORT, &mtu_key, &mtu_value);
        if (rv != BCM_E_NONE && compressed_layer_type != 7)
        {

            return rv;
        }
        else if (rv == BCM_E_PARAM && compressed_layer_type == 7)
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "LIF bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! \n")));
        }
    }
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "MTU Profile + Compressed Layer Type Exhaustion Testing END!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "MTU Profile Clean-up Exhaustion Testing Start!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    /** Clean-up */
    for (mtu_iter = 0; mtu_iter < dnx_data_trap.etpp.nof_mtu_profiles_get(unit); mtu_iter++)
    {
        for (compressed_layer_type = 0; compressed_layer_type < dnx_data_trap.etpp.nof_mtu_profiles_get(unit);
             compressed_layer_type++)
        {
            flags = BCM_RX_MTU_LIF;
            bcm_rx_mtu_profile_key_t_init(&mtu_key);
            bcm_rx_mtu_profile_value_t_init(&mtu_value);

            mtu_key.mtu_profile = mtu_iter;
            mtu_key.cmp_layer_type = compressed_layer_type;

            if (compressed_layer_type == 7)
            {
                flags = BCM_RX_MTU_PORT;
            }

            rv = bcm_rx_mtu_profile_set(unit, flags, &mtu_key, &mtu_value);
            if (rv != BCM_E_NONE && compressed_layer_type != 7)
            {

                return rv;
            }
            else if (rv == BCM_E_PARAM && compressed_layer_type == 7)
            {
                LOG_INFO(BSL_LS_APPL_COMMON,
                         (BSL_META_U(unit, "LIF bcm_rx_mtu_profile_set should fail with BCM_E_PARAM! \n")));
            }
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "MTU Profile Clean-up Exhaustion Testing END!!! \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "----------------    SUCCESS     -------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_mtu_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *test_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_MTU_TEST_TYPE, test_type);

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_TEST_TYPE_POSITIVE) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_mtu_pos(unit, args, sand_control));

        SHR_EXIT();
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_TEST_TYPE_NEGATIVE) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_mtu_neg(unit, args, sand_control));

        SHR_EXIT();
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_TEST_TYPE_EXHAUSTIVE) == 0)
    {

        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_mtu_exh(unit, args, sand_control));

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
