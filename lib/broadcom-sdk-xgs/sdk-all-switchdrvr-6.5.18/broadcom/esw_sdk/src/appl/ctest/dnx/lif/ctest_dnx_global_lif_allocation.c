/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RESMNGR

/**
* INCLUDE FILES:
* {
*/
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr_api.h>

#include <appl/diag/sand/diag_sand_framework.h>

/**
 * }
 */

sh_sand_man_t dnx_global_lif_semantic_test_man = {
    "Global lif semantic test.",
    "Perform a semantic test of global lif. allocate/get/free, testing different scenarios."
};

/**
 * \brief
 *    Basic test for the global lif allocation module.
 *    This test is not meant to be exhustive, but just to test basic functionality.
 *
 *    1. Symmetric lif test:
 *    1.1. Allocate a symmetric lif. Don't pass the l2 gport flag.
 *    1.1.1. Make sure that an l2 gport ID wasn't allocated.
 *    1.2. Delete this lif.
 *    2. Assyemtric lif test:
 *    2.1. Allocate an egress only lif with the ID used before.
 *    2.2. Allocate an ingress only lif with the same ID.
 *    2.3. Free both ingress and egress lifs.
 *    3. L2 gport test:
 *    3.1. Allocate a global lif with the l2 gport flag.
 *    3.2. Delete this lif.
 *    4. Repeat
 */
shr_error_e
ctest_dnx_global_lif_basic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lif_id;
    int iter;
    SHR_FUNC_INIT_VARS(unit);

    for (iter = 0; iter < 2; iter++)
    {
        cli_out("Run %d:\n", iter + 1);
        /*
         * 1.1 Allocate a symmetric lif. No l2 gport flag given.
         */
        cli_out("  1.1 Allocate a symmetric lif. No l2 gport flag given.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, 0, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, &lif_id));

        /*
         * 1.1.1 Make sure that the ID is not in the l2 gport range.
         */
        if (dnx_data_lif.global_lif.nof_global_in_lifs_get(unit) >=
            dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit)
            && lif_id < dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INIT,
                         "No l2 gport flag was given, but global lif was allocated in the l2 gport range. "
                         "Allocated id is 0x%8x", lif_id);
        }

        /*
         * 1.2. Delete this lif.
         */
        cli_out("  1.2. Delete this lif.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, lif_id));

        /*
         * 2.1. Allocate an egress only lif with the ID used before.
         */
        cli_out("  2.1. Allocate an egress only lif with the ID used before.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, 0, DNX_ALGO_LIF_EGRESS, &lif_id));

        /*
         *    2.2. Allocate an ingress only lif with the same ID.
         */
        cli_out("  2.2. Allocate an ingress only lif with the same ID.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, DNX_ALGO_LIF_INGRESS, &lif_id));

        /*
         *    2.3. Free both ingress and egress lifs.
         */
        cli_out("  2.3. Free both ingress and egress lifs.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, lif_id));

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, lif_id));

        /*
         * 3.1 Allocate an l2 gport global lif and verify that it's in the correct range.
         */
        cli_out("  3.1 Allocate an l2 gport global lif and verify that it's in the correct range.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, &lif_id));

        /*
         * Make sure that the ID is in the l2 gport range.
         */
        if (lif_id >= dnx_data_lif.global_lif.nof_global_l2_gport_lifs_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_INIT,
                         "L2 gport flag was given, but global lif was allocated outside the l2 gport range. "
                         "Allocated id is 0x%8x", lif_id);
        }

        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, lif_id));
    }

    cli_out("PASS!\n");
exit:
    SHR_FUNC_EXIT;
}
