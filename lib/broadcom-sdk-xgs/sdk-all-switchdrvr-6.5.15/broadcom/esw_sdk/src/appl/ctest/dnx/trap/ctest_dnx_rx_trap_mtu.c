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
    {CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_TYPE, SAL_FIELD_TYPE_STR, "Type of test (port,lif or rif)", "port"}
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
    {"mtu_port", "type=port", CTEST_POSTCOMMIT}
    ,
    {"mtu_lif", "type=lif", CTEST_POSTCOMMIT}
    ,
    {"mtu_rif", "type=rif", CTEST_POSTCOMMIT}
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
    bcm_rx_mtu_config_t * mtu_config)
{
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    int trap_strength = 15;
    int snoop_strength = 0;
    int mtu_val = 3000;

    SHR_FUNC_INIT_VARS(unit);

    if (trap_id != 0)
    {
        BCM_GPORT_TRAP_SET(mtu_config->trap_gport, trap_id, trap_strength, snoop_strength);
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

            mtu_config->compressed_layer_type = info.value;
        }
        mtu_config->mtu = mtu_val;
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
            mtu_config->compressed_layer_type = info.value;
        }
    }

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
    bcm_rx_mtu_config_t * set_mtu_config)
{
    bcm_rx_mtu_config_t get_mtu_config;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    SHR_FUNC_INIT_VARS(unit);

    get_mtu_config.flags = set_mtu_config->flags;
    get_mtu_config.gport = set_mtu_config->gport;
    get_mtu_config.intf = set_mtu_config->intf;
    if (set_mtu_config->flags & BCM_RX_MTU_PORT)
    {
        get_mtu_config.compressed_layer_type = set_mtu_config->compressed_layer_type;
    }
    else
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

        get_mtu_config.compressed_layer_type = info.value;
    }

    SHR_IF_ERR_EXIT(bcm_rx_mtu_get(unit, &get_mtu_config));

    if (get_mtu_config.mtu != set_mtu_config->mtu)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET MTU value %d is not equal to SET MTU Value %d",
                     get_mtu_config.mtu, set_mtu_config->mtu);
    }

    if (get_mtu_config.trap_gport != set_mtu_config->trap_gport)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "GET MTU Trap Gport value %d is not equal to SET MTU Trap Gport %d",
                     get_mtu_config.trap_gport, set_mtu_config->trap_gport);
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
static bcm_error_t
appl_dnx_rx_trap_mtu(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_mtu_config_t mtu_config, cleanup_mtu_config;
    bcm_rx_trap_config_t trap_config;
    bcm_if_t rif = 0;
    bcm_gport_t lif = 0;
    int port = 150;
    int trap_id;
    int flags;
    char *test_type;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR(CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_TYPE, test_type);
    bcm_rx_mtu_config_t_init(&mtu_config);

    rif = 0;
    lif = 0;
    flags = 0;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id));

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.is_recycle_append_ftmh = TRUE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &trap_config));

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_TYPE_PORT) == 0)
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

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_config));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_TYPE_RIF) == 0)
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

        mtu_config.flags = flags;
        mtu_config.intf = rif;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_config));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_MTU_OPTION_TEST_TYPE_LIF) == 0)
    {
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
        LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                    OUTLIF(****) MTU TEST                \n")));
        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

        flags = BCM_RX_MTU_LIF;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif));

        mtu_config.flags = flags;
        mtu_config.gport = lif;

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &mtu_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_get_compare(unit, &mtu_config));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "MTU TRAP CONFIGURATION TEST END.\n")));

    /** Clean-up */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, trap_id));

    bcm_rx_mtu_config_t_init(&cleanup_mtu_config);

    trap_id = 0;
    cleanup_mtu_config.flags = flags;
    cleanup_mtu_config.gport = mtu_config.gport;
    cleanup_mtu_config.intf = mtu_config.intf;

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_mtu_mtu_config(unit, trap_id, &cleanup_mtu_config));

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

shr_error_e
sh_dnx_rx_trap_mtu_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(appl_dnx_rx_trap_mtu(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
