/*
 * ctest_dnx_rx_trap_lif.c
 *
 *  Created on: April 11, 2018
 *      Author: sm888907
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
#include "ctest_dnx_rx_trap_lif.h"
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
sh_sand_man_t sh_dnx_rx_trap_lif_man = {
    "LIF traps configuration testing",
    "Configure LIF trap, then get the LIF config and compare values."
};

const sh_sand_enum_t sh_dnx_rx_trap_lif_types_enum[] = {
    /**String      Value*/
    {"InLIF", bcmRxTrapLifTypeInLif}
    ,
    {"InRIF", bcmRxTrapLifTypeInRif}
    ,
    {"OutLIF", bcmRxTrapLifTypeOutLif}
    ,
    {"OutRIF", bcmRxTrapLifTypeOutRif}
    ,
    {"ALL", bcmRxTrapLifTypeCount}
    ,
    {NULL}
};

/**
 * \brief
 *   Options list for 'lif' shell command
 * \remark
 */
sh_sand_option_t sh_dnx_rx_trap_lif_options[] = {
    {"Type", SAL_FIELD_TYPE_ENUM, "Type of test", "all", (void *) sh_dnx_rx_trap_lif_types_enum}
    ,
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief
 *   List of tests for LIF Traps shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_rx_trap_lif_tests[] = {
    {"InLIF", "type=inlif", CTEST_POSTCOMMIT}
    ,
    {"InRIF", "type=inrif", CTEST_POSTCOMMIT}
    ,
    {"OutLIF", "type=outlif", CTEST_POSTCOMMIT}
    ,
    {"OutRIF", "type=outrif", CTEST_POSTCOMMIT}
    ,
    {"ALL", "type=all", CTEST_POSTCOMMIT}
    ,
    {NULL}
};

/**
 * \brief
 *   This function checks the configuration of LIF traps.
 *   Create ETPP UserDefined Trap
 *   Set an LIF configuration
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 * \param [in] test_type - lif test type.
 * \param [in] gport_sub_type - used to indicate esem_default and esem_match config tests.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_lif(
    int unit,
    bcm_rx_trap_lif_type_t test_type,
    bcm_gport_t gport_sub_type)
{
    bcm_rx_trap_lif_config_t lif_config, lif_config_get;
    bcm_rx_trap_config_t trap_config;
    bcm_if_t rif = 0;
    bcm_gport_t lif = 0;
    char subtype_name[DBAL_MAX_SHORT_STRING_LENGTH] = "basic";
    int ingress_trap_id, etpp_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;

    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, &ingress_trap_id));

    bcm_rx_trap_lif_config_t_init(&lif_config);
    bcm_rx_trap_lif_config_t_init(&lif_config_get);

    switch (test_type)
    {
        case bcmRxTrapLifTypeInLif:
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inlif_create_example(unit, &lif));
            lif_config.lif_gport = lif;
            lif_config_get.lif_gport = lif;
            BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
            break;
        case bcmRxTrapLifTypeInRif:
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inrif_create_example(unit, &rif));
            lif_config.rif_intf = rif;
            lif_config_get.rif_intf = rif;
            BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 10, 0);
            break;
        case bcmRxTrapLifTypeOutLif:
            switch (gport_sub_type)
            {
                case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT:
                    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_esem_default_example(unit, &lif));
                    sal_strncpy(subtype_name, "ESEM default", DBAL_MAX_SHORT_STRING_LENGTH);
                    break;
                case BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH:
                    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_esem_match_example(unit, &lif));
                    sal_strncpy(subtype_name, "ESEM match", DBAL_MAX_SHORT_STRING_LENGTH);
                    break;
                default:
                    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &lif));
            }
            lif_config.lif_gport = lif;
            lif_config_get.lif_gport = lif;
            BCM_GPORT_TRAP_SET(lif_config.action_gport, etpp_trap_id, 3, 0);
            break;
        case bcmRxTrapLifTypeOutRif:
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outrif_create_example(unit, &rif));
            lif_config.rif_intf = rif;
            lif_config_get.rif_intf = rif;
            BCM_GPORT_MIRROR_MIRROR_SET(lif_config.action_gport, 2);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", test_type);
            break;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                    %s %s TEST                \n"),
              sh_dnx_rx_trap_lif_types_enum[test_type].string, subtype_name));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    lif_config.lif_type = test_type;
    lif_config_get.lif_type = test_type;

    SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

    SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config_get));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_compare_values(unit, &lif_config.action_gport,
                                                     &lif_config_get.action_gport, sizeof(bcm_gport_t),
                                                     "action_gport"));

    /** Clean LIF profile */
    lif_config.action_gport = BCM_GPORT_INVALID;
    SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));
    if (lif != 0)
    {
        if (gport_sub_type == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT ||
            gport_sub_type == BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_esem_destroy_example(unit, &lif));
        }
        else
        {
            SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif));
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_rif_destroy_example(unit, &rif, test_type));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "LIF TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_lif_type_t test_type;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("type", test_type);

    if (test_type == bcmRxTrapLifTypeCount)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeInLif, BCM_GPORT_SUB_TYPE_LIF));
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeInRif, BCM_GPORT_SUB_TYPE_LIF));
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_LIF));
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT));
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH));
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, bcmRxTrapLifTypeOutRif, BCM_GPORT_SUB_TYPE_LIF));
    }
    else
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif(unit, test_type, BCM_GPORT_SUB_TYPE_LIF));
    }

exit:
    SHR_FUNC_EXIT;
}
