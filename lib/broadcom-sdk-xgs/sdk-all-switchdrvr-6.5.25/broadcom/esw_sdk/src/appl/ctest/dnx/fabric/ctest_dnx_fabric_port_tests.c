/** \file ctest_dnx_fabric_tests.c
 *
 * Tests for fabric
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/fabric/fabric_if.h>

/*
 * \brief
 *   FSRDs power state test.
 *   Make actual sure power state is according to the supported FSRDs.
 */
static shr_error_e
ctest_dnx_fabric_fsrd_power_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nof_test_cases = 0;
    uint32 fail_cnt = 0;
    bcm_pbmp_t supported_fsrd;
    int nof_fsrd;
    int fsrd_block = 0;
    int is_enable, is_supported, is_fail;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * prepare expected power state of all FSRDs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_supported_fsrd_get(unit, &supported_fsrd));

    nof_fsrd = dnx_data_fabric.blocks.nof_instances_fsrd_get(unit);

    /*
     * Iterate over all FSRDs and validate their power state matches
     * expected state.
     */
    for (fsrd_block = 0; fsrd_block < nof_fsrd; ++fsrd_block)
    {
        nof_test_cases += 1;

        SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_block_enable_get(unit, fsrd_block, &is_enable));
        is_supported = _SHR_PBMP_MEMBER(supported_fsrd, fsrd_block) ? 1 : 0;

        is_fail = (is_enable ^ is_supported);
        if (is_fail)
        {
            fail_cnt += 1;
        }

        LOG_CLI_EX("fsrd_id:%3u, supported:%-3.3s enabled:%-3.3s result:%-4.4s\n",
                   fsrd_block, (is_supported ? "yes" : "no"), (is_enable ? "yes" : "no"), (is_fail ? "fail" : "pass"));
    }

    if (fail_cnt != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Test FAILED. Number of cases tested = %d\n", nof_test_cases);
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS. Number of cases tested = %d\n", nof_test_cases);

exit:

    SHR_FUNC_EXIT;
}

static sh_sand_invoke_t ctest_dnx_fabric_fsrd_power_tests[] = {
    {"FSRDs power state", "", CTEST_POSTCOMMIT},
    {NULL}
};

static sh_sand_man_t ctest_dnx_fabric_fsrd_power_test_man = {
    .brief = "Test FSRD power state",
    .full = "Test FSRD power state",
};

/* *INDENT-OFF* */

sh_sand_cmd_t ctest_dnx_fabric_port_test_cmds[] = {
    { .keyword = "port_power",
      .action = ctest_dnx_fabric_fsrd_power_test_cmd,
      .man = &ctest_dnx_fabric_fsrd_power_test_man,
      .invokes = ctest_dnx_fabric_fsrd_power_tests },
    { NULL }
};

/* *INDENT-ON* */
