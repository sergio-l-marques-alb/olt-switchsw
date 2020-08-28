/** \file ctest_dnx_access.c
 * 
 * ACCESS TEST - Testing access
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_TESTS

/*
 * INCLUDE FILES:
 * {
 */
/*soc*/
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for access */
/*sal*/
#include <sal/appl/sal.h>
#include <shared/shrextend/shrextend_debug.h>

#include <appl/diag/sand/diag_sand_utils.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/**
 * \brief - Test memory defaults when resetting memories by dma
 */
static shr_error_e
ctest_dnx_access_memory_defaults_dma(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t ctest_soc_set_h = NULL;

    ctest_soc_property_t ctest_soc_property[] = {
        {"custom_feature_mem_defaults_verify", "1"},
        {"bist_enable", "0"},
        {NULL}
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Test memory defaults when resetting memories by mbist
 */
static shr_error_e
ctest_dnx_access_memory_defaults_mbist(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    rhhandle_t ctest_soc_set_h = NULL;

    ctest_soc_property_t ctest_soc_property[] = {
        {"custom_feature_mem_defaults_verify", "1"},
        {"bist_enable", "1"},
        {NULL}
    };
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_FUNC_EXIT;
}

static sh_sand_man_t ctest_dnx_access_memory_defaults_mbist_man = {
    "Test memory defaults when resetting memories by mbist"
};

static sh_sand_man_t ctest_dnx_access_memory_defaults_dma_man = {
    "Test memory defaults when resetting memories by dma"
};

sh_sand_cmd_t ctest_dnx_access_memory_defaults_cmds[] = {
    {"mbist", ctest_dnx_access_memory_defaults_mbist, NULL, NULL, &ctest_dnx_access_memory_defaults_mbist_man, NULL,
     NULL, CTEST_POSTCOMMIT},
    {"dma", ctest_dnx_access_memory_defaults_dma, NULL, NULL, &ctest_dnx_access_memory_defaults_dma_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {NULL}
};

/**
 * \brief List of memory access tests
 */
sh_sand_cmd_t ctest_dnx_access_memory_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer
     */
    {"defaults", NULL, ctest_dnx_access_memory_defaults_cmds},
    {NULL}      /* This line should always stay as last one */
};

/**
 * \brief List of access tests
 */
sh_sand_cmd_t ctest_dnx_access_cmds[] = {
    /*
     * NAME | Leaf action | Junction Array Pointer
     */
    {"memory", NULL, ctest_dnx_access_memory_cmds},
    {NULL}      /* This line should always stay as last one */
};
