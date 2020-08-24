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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trap.h>
#include <sal/appl/sal.h>
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
    {"mode", SAL_FIELD_TYPE_ENUM, "Type of lif, only relevant for positive test.", "all",
     (void *) sh_dnx_rx_trap_lif_types_enum}
    ,
    {"type", SAL_FIELD_TYPE_STR, "Type of test", "pos"}
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
    {"InLIF", "type=pos mode=inlif", CTEST_POSTCOMMIT}
    ,
    {"InRIF", "type=pos mode=inrif", CTEST_POSTCOMMIT}
    ,
    {"OutLIF", "type=pos mode=outlif", CTEST_POSTCOMMIT}
    ,
    {"OutRIF", "type=pos mode=outrif", CTEST_POSTCOMMIT}
    ,
    {"ALL", "type=pos mode=all", CTEST_POSTCOMMIT}
    ,
    {"Negative", "type=neg", CTEST_POSTCOMMIT}
    ,
    {"Exhaustive", "type=exh ", CTEST_POSTCOMMIT}
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
 * \param [in] test_mode - lif type.
 * \param [in] gport_sub_type - used to indicate esem_default and esem_match config tests.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_lif_pos(
    int unit,
    bcm_rx_trap_lif_type_t test_mode,
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

    switch (test_mode)
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
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d\n", test_mode);
            break;
    }

    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                    %s %s TEST                \n"),
              sh_dnx_rx_trap_lif_types_enum[test_mode].string, subtype_name));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------------------------------------------------------- \n")));

    lif_config.lif_type = test_mode;
    lif_config_get.lif_type = test_mode;

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
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_rif_destroy_example(unit, &rif, test_mode));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "LIF TRAP CONFIGURATION TEST END.\n")));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks all error messages for invalid data supplied to the API
 *   or illegal configurations.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_lif_neg(
    int unit)
{
    bcm_rx_trap_lif_config_t lif_config;
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t lif = 0, out_lif = 0;
    int rv = BCM_E_NONE;
    int ingress_trap_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&trap_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, &ingress_trap_id));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inlif_create_example(unit, &lif));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_lif_set TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid LIF-type to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    lif_config.lif_gport = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = 5;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid RIF to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.rif_intf = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInRif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING non-encoded LIF value to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = 300;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING non-existing(not created) LIF to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = 0x1890270A;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING Ingress Trap ID > 512 to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, 520, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING ETPP Trap ID > 8 to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &out_lif));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = out_lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, 16, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeOutLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_INTERNAL)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING non-allocated Trap ID to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, 260, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING non-trap gport encoded value to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = lif;
    lif_config.action_gport = 120;
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_set should fail with BCM_E_INTERNAL! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_lif_set TESTING ENDS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_lif_get TESTING STARTS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid flags to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    lif_config.lif_gport = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = 5;

    rv = bcm_rx_trap_lif_get(unit, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING invalid RIF to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.rif_intf = lif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInRif;

    rv = bcm_rx_trap_lif_get(unit, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "SUPPLYING non-encoded LIF value to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = 300;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_get(unit, &lif_config);
    if (rv != BCM_E_PARAM)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_get should fail with BCM_E_PARAM! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "SUPPLYING non-existing(not created) LIF to bcm_rx_trap_lif_set \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    lif_config.lif_gport = 0x1890270A;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
    lif_config.lif_type = bcmRxTrapLifTypeInLif;

    rv = bcm_rx_trap_lif_get(unit, &lif_config);
    if (rv != BCM_E_NOT_FOUND)
    {
        LOG_ERROR(BSL_LS_APPL_COMMON,
                  (BSL_META_U(unit, "bcm_rx_trap_lif_get should fail with BCM_E_NOT_FOUND! . Error:%d (%s)\n"), rv,
                   bcm_errmsg(rv)));
        return rv;
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "--------------- bcm_rx_trap_lif_get TESTING ENDS HERE ---------- \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &out_lif));

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                SUCCESS: Test PASS                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    rv = BCM_E_NONE;
    return rv;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function checks all boudried of the API and exhausts all resources
 *   multiple times to validate allocation/deallocation dynamic.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static bcm_error_t
appl_dnx_rx_trap_lif_exh(
    int unit)
{
    bcm_rx_trap_lif_config_t lif_config, lif_config_get;
    bcm_rx_trap_config_t trap_config;

    bcm_gport_t *lif_p = NULL;
    bcm_gport_t *out_lif_p = NULL;
    bcm_gport_t *set_gport_ingress_p = NULL;
    bcm_gport_t *set_gport_etpp_p = NULL;

    int etpp_trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;
    int ingress_trap_id;
    int index, rv = BCM_E_NONE;
    int reference_index = 4095;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.device_id_get(unit) == 0x8690)
    {
        reference_index = 2045;
    }

    /*
     * Dummy glem entry(global_null_lif) is added to DBAL_TABLE_GLOBAL_LIF_EM by SDK init.
     */
    if (dnx_data_lif.global_lif.wa_null_lif_get(unit))
    {
        reference_index--;
    }

    SHR_ALLOC_SET_ZERO(lif_p, sizeof(*lif_p) * reference_index, "lif_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(set_gport_ingress_p,
                       sizeof(*set_gport_ingress_p) * reference_index,
                       "set_gport_ingress_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(out_lif_p, sizeof(*out_lif_p) * reference_index, "out_lif_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(set_gport_etpp_p, sizeof(*set_gport_etpp_p) * reference_index,
                       "set_gport_etpp_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    bcm_rx_trap_config_t_init(&trap_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDfltDroppedPacket, &ingress_trap_id));

    bcm_rx_trap_lif_config_t_init(&lif_config);

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &etpp_trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                ETPP Reference Check                       \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < reference_index; index++)
    {
        out_lif_p[index] = index;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &out_lif_p[index]));

        lif_config.lif_gport = out_lif_p[index];

        BCM_GPORT_TRAP_SET(lif_config.action_gport, etpp_trap_id, 7, 0);

        lif_config.lif_type = bcmRxTrapLifTypeOutLif;
        sal_memcpy(&set_gport_etpp_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        LOG_INFO(BSL_LS_APPL_COMMON,
                 (BSL_META_U(unit, "-----------------------------%d-------------------------\n"), index));

    }

    for (index = 0; index < reference_index; index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = out_lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeOutLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &out_lif_p[index]));
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, etpp_trap_id));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                Ingress Reference Check                     \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    bcm_rx_trap_lif_config_t_init(&lif_config);
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ingress_trap_id));

    for (index = 0; index < reference_index; index++)
    {
        lif_p[index] = index + 1;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inlif_create_example(unit, &lif_p[index]));

        lif_config.lif_gport = lif_p[index];
        BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
        sal_memcpy(&set_gport_ingress_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));
        lif_config.lif_type = bcmRxTrapLifTypeInLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));
    }

    for (index = 0; index < reference_index; index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeInLif;
        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_p[index]));
    }

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, ingress_trap_id));

    SHR_FREE(lif_p);
    SHR_FREE(out_lif_p);
    SHR_FREE(set_gport_ingress_p);
    SHR_FREE(set_gport_etpp_p);

    SHR_ALLOC_SET_ZERO(lif_p, sizeof(*lif_p) * dnx_data_trap.ingress.nof_lif_traps_get(unit), "lif_p", "%s%s%s", EMPTY,
                       EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(out_lif_p, sizeof(*out_lif_p) * dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit), "out_lif_p",
                       "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(set_gport_ingress_p,
                       sizeof(*set_gport_ingress_p) * dnx_data_trap.ingress.nof_lif_traps_get(unit),
                       "set_gport_ingress_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(set_gport_etpp_p, sizeof(*set_gport_etpp_p) * dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit),
                       "set_gport_etpp_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                Ingress set + Overflow                       \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.ingress.nof_lif_traps_get(unit)); index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ingress_trap_id));

        lif_p[index] = index + 1;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inlif_create_example(unit, &lif_p[index]));

        lif_config.lif_gport = lif_p[index];
        BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
        sal_memcpy(&set_gport_ingress_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));
        lif_config.lif_type = bcmRxTrapLifTypeInLif;

        rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
        if ((rv == BCM_E_FULL) && (index == 7))
        {
            LOG_INFO(BSL_LS_APPL_COMMON,
                     (BSL_META_U(unit, "Ingress - bcm_rx_trap_lif_set should fail with BCM_E_FULL! . Error:%d\n"), rv));

        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                Ingress get + compare + cleanup                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.ingress.nof_lif_traps_get(unit) - 1); index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeInLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config));

        if (lif_config.action_gport != set_gport_ingress_p[index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Set action gport(0x%08x) is not equal to get action gport(0x%08x). \n",
                         set_gport_ingress_p[index], lif_config.action_gport);
        }

        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeInLif;
        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_p[index]));

        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, BCM_GPORT_TRAP_GET_ID(set_gport_ingress_p[index])));
    }

    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_p[index]));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                ETPP set + Overflow                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit)); index++)
    {
        out_lif_p[index] = index;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &out_lif_p[index]));
        if (index < 2)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &etpp_trap_id));
        }

        lif_config.lif_gport = out_lif_p[index];
        if (index < 2)
        {
            BCM_GPORT_TRAP_SET(lif_config.action_gport, etpp_trap_id, 7, 0);
        }
        else
        {
            BCM_GPORT_MIRROR_MIRROR_SET(lif_config.action_gport, index);
        }

        lif_config.lif_type = bcmRxTrapLifTypeOutLif;
        sal_memcpy(&set_gport_etpp_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));

        rv = bcm_rx_trap_lif_set(unit, 0, &lif_config);
        if (rv == BCM_E_FULL && index == 3)
        {
            LOG_ERROR(BSL_LS_APPL_COMMON,
                      (BSL_META_U(unit, "ETPP - bcm_rx_trap_lif_set should fail with BCM_E_FULL! . Error:%d (%s)\n"),
                       rv, bcm_errmsg(rv)));

        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                ETPP get + compare + clean                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit) - 1); index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = out_lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeOutLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config));

        if (lif_config.action_gport != set_gport_etpp_p[index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Set action gport(0x%08x) is not equal to get actiongport(0x%08x). \n",
                         set_gport_etpp_p[index], lif_config.action_gport);
        }

        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = out_lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeOutLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &out_lif_p[index]));
        if (index < 2)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, BCM_GPORT_TRAP_GET_ID(set_gport_etpp_p[index])));
        }
    }
    SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &out_lif_p[index]));

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                Ingress set/get/compare/clean                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.ingress.nof_lif_traps_get(unit) - 1); index++)
    {
        bcm_rx_trap_lif_config_t_init(&lif_config);
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &ingress_trap_id));

        lif_p[index] = index + 1;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_inlif_create_example(unit, &lif_p[index]));

        lif_config.lif_gport = lif_p[index];
        BCM_GPORT_TRAP_SET(lif_config.action_gport, ingress_trap_id, 7, 0);
        sal_memcpy(&set_gport_ingress_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));
        lif_config.lif_type = bcmRxTrapLifTypeInLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        bcm_rx_trap_lif_config_t_init(&lif_config_get);
        lif_config_get.lif_gport = lif_p[index];
        lif_config_get.lif_type = bcmRxTrapLifTypeInLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config_get));

        if (lif_config.action_gport != set_gport_ingress_p[index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Set action gport(0x%08x) is not equal to get action gport(0x%08x). \n",
                         set_gport_ingress_p[index], lif_config.action_gport);
        }

        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeInLif;
        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

    }

    for (index = 0; index < (dnx_data_trap.ingress.nof_lif_traps_get(unit) - 1); index++)
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, BCM_GPORT_TRAP_GET_ID(set_gport_ingress_p[index])));
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &lif_p[index]));
    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON,
             (BSL_META_U(unit, "                ETPP set/get/compare/clean                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));

    for (index = 0; index < (dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit) - 1); index++)
    {
        out_lif_p[index] = index;
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_outlif_create_example(unit, &out_lif_p[index]));
        if (index < 2)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &etpp_trap_id));
        }

        lif_config.lif_gport = out_lif_p[index];
        if (index < 2)
        {
            BCM_GPORT_TRAP_SET(lif_config.action_gport, etpp_trap_id, 7, 0);
        }
        else
        {
            BCM_GPORT_MIRROR_MIRROR_SET(lif_config.action_gport, 2);
        }

        lif_config.lif_type = bcmRxTrapLifTypeOutLif;
        sal_memcpy(&set_gport_etpp_p[index], &lif_config.action_gport, sizeof(bcm_gport_t));

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = out_lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeOutLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config));

        if (lif_config.action_gport != set_gport_etpp_p[index])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Set action gport(0x%08x) is not equal to get actiongport(0x%08x). \n",
                         set_gport_etpp_p[index], lif_config.action_gport);
        }

        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_gport = out_lif_p[index];
        lif_config.lif_type = bcmRxTrapLifTypeOutLif;

        SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

    }

    for (index = 0; index < (dnx_data_trap.etpp.nof_etpp_lif_traps_get(unit) - 1); index++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_rx_trap_lif_destroy_example(unit, &out_lif_p[index]));
        if (index < 2)
        {
            SHR_IF_ERR_EXIT(bcm_rx_trap_type_destroy(unit, BCM_GPORT_TRAP_GET_ID(set_gport_etpp_p[index])));
        }

    }

    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "                SUCCESS: Test PASS                        \n")));
    LOG_INFO(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "-------------------------------------------------------------\n")));
    rv = BCM_E_NONE;
    return rv;
exit:
    SHR_FREE(lif_p);
    SHR_FREE(out_lif_p);
    SHR_FREE(set_gport_ingress_p);
    SHR_FREE(set_gport_etpp_p);
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_rx_trap_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_lif_type_t test_mode;
    char *test_type;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("mode", test_mode);
    SH_SAND_GET_STR("type", test_type);

    if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_POSITIVE) == 0)
    {
        if (test_mode == bcmRxTrapLifTypeCount)
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos(unit, bcmRxTrapLifTypeInLif, BCM_GPORT_SUB_TYPE_LIF));
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos(unit, bcmRxTrapLifTypeInRif, BCM_GPORT_SUB_TYPE_LIF));
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos(unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_LIF));
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos
                            (unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT));
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos
                            (unit, bcmRxTrapLifTypeOutLif, BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH));
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos(unit, bcmRxTrapLifTypeOutRif, BCM_GPORT_SUB_TYPE_LIF));
        }
        else
        {
            SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_pos(unit, test_mode, BCM_GPORT_SUB_TYPE_LIF));
        }
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_NEGATIVE) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_neg(unit));
    }
    else if (sal_strcasecmp(test_type, CTEST_DNX_RX_TRAPS_LIF_TEST_TYPE_EXHAUSTIVE) == 0)
    {
        SHR_IF_ERR_EXIT(appl_dnx_rx_trap_lif_exh(unit));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type '%s' supplied, should be 'pos' , 'neg' or 'exh'\n", test_type);
    }

exit:
    SHR_FUNC_EXIT;
}
