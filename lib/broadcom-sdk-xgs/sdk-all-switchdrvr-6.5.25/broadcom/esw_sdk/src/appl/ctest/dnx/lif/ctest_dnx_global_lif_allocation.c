/** \file diag_dnx_in_lif_profile.c
 *
 * in_lif_profile unit test.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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

#include <appl/diag/bslenable.h>
/**
 * }
 */

sh_sand_man_t dnx_global_lif_semantic_test_man = {
    "Global lif semantic test.",
    "Perform a semantic test of global lif. allocate/get/free, testing different scenarios."
};

sh_sand_man_t dnx_global_lif_tag_semantic_test_man = {
    "Global lif semantic test.",
    "Perform a semantic test of global lif tag allocation. allocating global lifs with different tag params"
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
                        (unit, 0, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, NULL, &lif_id));

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
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, 0, DNX_ALGO_LIF_EGRESS, NULL, &lif_id));

        /*
         *    2.2. Allocate an ingress only lif with the same ID.
         */
        cli_out("  2.2. Allocate an ingress only lif with the same ID.\n");
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate
                        (unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, DNX_ALGO_LIF_INGRESS, NULL, &lif_id));

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
                        (unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF, DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS, NULL,
                         &lif_id));

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

static shr_error_e
ctest_dnx_global_lif_tag_single_param_test(
    int unit,
    global_lif_alloc_info_t alloc_info[4],
    char test_parm_name[20])
{
    shr_error_e rv;
    bsl_severity_t orig_lif_severity, orig_resmngr_severity;
    int lif_ids[8];
    uint8 alloc_idx, prev_alloc;
    uint8 direction;
    uint32 selected_direction[2] = { DNX_ALGO_LIF_EGRESS, (DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS) };

    SHR_FUNC_INIT_VARS(unit);

    cli_out("Starting %s test\n", test_parm_name);

    for (direction = 0; direction < 2; direction++)
    {
        if (direction == 0)
        {
            cli_out("Egress direction\n");
        }
        else
        {
            cli_out("Symmetric direction\n");
        }
        for (alloc_idx = 0; alloc_idx < 4; alloc_idx++)
        {
            uint8 allocated_lif_idx = ((direction * 4) + alloc_idx);
            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF,
                                                                    selected_direction[direction],
                                                                    &alloc_info[alloc_idx],
                                                                    &lif_ids[allocated_lif_idx]));
            cli_out("LIF ID 0x%x was allocated with BTA %d, INTF %d, GROUP %d\n", lif_ids[allocated_lif_idx],
                    alloc_info[alloc_idx].bta, alloc_info[alloc_idx].outlif_intf, alloc_info[alloc_idx].outlif_group);
        }
    }

    cli_out("Checking allocations integrity\n");
    for (alloc_idx = 0; alloc_idx < 8; alloc_idx++)
    {
        for (prev_alloc = 0; prev_alloc < alloc_idx; prev_alloc++)
        {
            /** special case where equally is expected */
            if (((prev_alloc == 0) && (alloc_idx == 3)) || ((prev_alloc == 4) && (alloc_idx == 7)))
            {
                if ((lif_ids[prev_alloc] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)) !=
                    (lif_ids[alloc_idx] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "lifs 0x%x and 0x%x were NOT allocated in the same bank\n",
                                 lif_ids[prev_alloc], lif_ids[alloc_idx]);
                }
            }
            else
            {
                if ((lif_ids[prev_alloc] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)) ==
                    (lif_ids[alloc_idx] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "lifs 0x%x and 0x%x were allocated in the same bank\n",
                                 lif_ids[prev_alloc], lif_ids[alloc_idx]);
                }
            }
        }
    }
    cli_out("Allocations integrity verified\n");

    cli_out("Freeing resources\n");
    for (direction = 0; direction < 2; direction++)
    {
        for (alloc_idx = 0; alloc_idx < 4; alloc_idx++)
        {
            uint8 allocated_lif_idx = ((direction * 4) + alloc_idx);
            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                            (unit, selected_direction[direction], lif_ids[allocated_lif_idx]));
        }
    }

    cli_out("Starting negative test\n");
    lif_ids[0] = 0x4000;
    lif_ids[1] = 0x4001;
    lif_ids[2] = 0x4002;

    /** expected to succeed */
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info[0], &lif_ids[0]);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x failed unexpectedly\n", lif_ids[0]);
    }
    /** expected to succeed */
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info[0], &lif_ids[1]);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x failed unexpectedly\n", lif_ids[1]);
    }
    /** expected to fail */
    /** closing the prints severity */
    orig_lif_severity = bslenable_get(bslLayerBcmdnx, bslSourceLif);
    orig_resmngr_severity = bslenable_get(bslLayerBcmdnx, bslSourceResmngr);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, bslSeverityOff);
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info[2], &lif_ids[2]);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x did not failed as expected, returned with status %d\n",
                     lif_ids[2], rv);
    }
    /** restoring the prints severity */
    bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_lif_severity);
    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, orig_resmngr_severity);

    cli_out("Freeing negative test resources\n");
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, lif_ids[0]));
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, lif_ids[1]));

    cli_out("%s test has ended successfully\n\n", test_parm_name);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_global_lif_tag_multiple_params_test(
    int unit,
    uint8 nof_supported_params)
{
    shr_error_e rv;
    global_lif_alloc_info_t alloc_info = { 0 };
    global_lif_alloc_info_t initial_alloc_info = { 0 };
    bsl_severity_t orig_lif_severity, orig_resmngr_severity;
    int lif_ids[15] = { 0 };
    uint8 alloc_idx, prev_alloc;
    uint8 last_loop_idx = (2 + nof_supported_params);
    uint8 before_last_loop_idx = (last_loop_idx - 1);
    uint8 nof_lif_allocations = 2 * (last_loop_idx + 1);
    uint8 direction;
    uint32 selected_direction[2] = { DNX_ALGO_LIF_EGRESS, (DNX_ALGO_LIF_INGRESS | DNX_ALGO_LIF_EGRESS) };

    SHR_FUNC_INIT_VARS(unit);

    cli_out("Starting multiple parameters test with %d parameters\n", nof_supported_params);

    /*
     * the number of loops is the number of supported parameters + 3 (loops 0 to (2 + nof_supported_params))
     * the first uses pre-selected values
     * after that, there will be a loop for changing every param (while the others remain the same)
     * the one before last loop change all params
     * the last loop use the same pre-selected values as the first loop
     */

    /** updating the initial values */
    if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
    {
        initial_alloc_info.bta = 5;
    }
    if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
    {
        initial_alloc_info.outlif_intf = 1;
    }
    if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
    {
        initial_alloc_info.outlif_group = 368;
    }

    /** running the test loops */
    for (direction = 0; direction < 2; direction++)
    {
        if (direction == 0)
        {
            cli_out("Egress direction\n");
        }
        else
        {
            cli_out("Symmetric direction\n");
        }
        for (alloc_idx = 0; alloc_idx <= last_loop_idx; alloc_idx++)
        {
            uint8 allocated_lif_idx = ((direction * (last_loop_idx + 1)) + alloc_idx);
            if ((alloc_idx == 0) || (alloc_idx == last_loop_idx))
            {
                alloc_info = initial_alloc_info;
            }
            else if (alloc_idx == before_last_loop_idx)
            {
                if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
                {
                    alloc_info.bta++;
                }
                if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
                {
                    alloc_info.outlif_intf++;
                }
                if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
                {
                    alloc_info.outlif_group++;
                }
            }
            else
            {
                /** check which param should be updated */
                uint8 param_to_update = nof_supported_params - alloc_idx;
                uint8 param_idx = 0;
                if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
                {
                    if (param_idx == param_to_update)
                    {
                        alloc_info.bta++;
                    }
                    param_idx++;
                }
                if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
                {
                    if (param_idx == param_to_update)
                    {
                        alloc_info.outlif_intf++;
                    }
                    param_idx++;
                }
                if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
                {
                    if (param_idx == param_to_update)
                    {
                        alloc_info.outlif_group++;
                    }
                    param_idx++;
                }
            }

            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_allocate(unit, LIF_MNGR_L2_GPORT_GLOBAL_LIF,
                                                                    selected_direction[direction],
                                                                    &alloc_info, &lif_ids[allocated_lif_idx]));
            cli_out("LIF ID 0x%x was allocated with BTA value %d, INTF value %d, GROUP value %d,\n",
                    lif_ids[allocated_lif_idx], alloc_info.bta, alloc_info.outlif_intf, alloc_info.outlif_group);
        }
    }

    cli_out("Checking allocations integrity\n");
    for (alloc_idx = 0; alloc_idx < nof_lif_allocations; alloc_idx++)
    {
        for (prev_alloc = 0; prev_alloc < alloc_idx; prev_alloc++)
        {
            /** special case where equally is expected */
            if (((prev_alloc == 0) && (alloc_idx == last_loop_idx)) ||
                ((prev_alloc == (nof_lif_allocations / 2))
                 && (alloc_idx == ((nof_lif_allocations / 2) + last_loop_idx))))
            {
                if ((lif_ids[prev_alloc] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)) !=
                    (lif_ids[alloc_idx] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "lifs 0x%x and 0x%x were NOT allocated in the same bank\n",
                                 lif_ids[prev_alloc], lif_ids[alloc_idx]);
                }
            }
            else
            {
                if ((lif_ids[prev_alloc] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)) ==
                    (lif_ids[alloc_idx] / dnx_data_lif.global_lif.global_lif_grain_size_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "lifs 0x%x and 0x%x were allocated in the same bank\n",
                                 lif_ids[prev_alloc], lif_ids[alloc_idx]);
                }
            }
        }
    }
    cli_out("Allocations integrity verified\n");

    cli_out("Freeing resources\n");
    for (direction = 0; direction < 2; direction++)
    {
        for (alloc_idx = 0; alloc_idx <= last_loop_idx; alloc_idx++)
        {
            uint8 allocated_lif_idx = ((direction * (last_loop_idx + 1)) + alloc_idx);
            SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free
                            (unit, selected_direction[direction], lif_ids[allocated_lif_idx]));
        }
    }

    cli_out("Starting negative test\n");
    lif_ids[0] = 0x4000;
    lif_ids[1] = 0x4001;
    lif_ids[2] = 0x4002;

    /** expected to succeed */
    alloc_info = initial_alloc_info;
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info, &lif_ids[0]);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x failed unexpectedly\n", lif_ids[0]);
    }
    /** expected to succeed */
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info, &lif_ids[1]);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x failed unexpectedly\n", lif_ids[1]);
    }
    /** expected to fail */
    /** closing the prints severity */
    orig_lif_severity = bslenable_get(bslLayerBcmdnx, bslSourceLif);
    orig_resmngr_severity = bslenable_get(bslLayerBcmdnx, bslSourceResmngr);
    bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);
    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, bslSeverityOff);
    /** updating alloc_info data to create different tag */
    if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
    {
        alloc_info.bta++;
    }
    else if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
    {
        alloc_info.outlif_intf++;
    }
    else if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
    {
        alloc_info.outlif_group++;
    }
    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 DNX_ALGO_LIF_EGRESS, &alloc_info, &lif_ids[2]);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Allocating lif with id 0x%x did not failed as expected, returned with status %d\n",
                     lif_ids[2], rv);
    }
    /** restoring the prints severity */
    bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_lif_severity);
    bslenable_set(bslLayerBcmdnx, bslSourceResmngr, orig_resmngr_severity);

    cli_out("Freeing negative test resources\n");
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, lif_ids[0]));
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, lif_ids[1]));

    cli_out("Multiple parameters test has ended successfully\n\n");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
ctest_dnx_global_lif_tag_one_sided_symmetric_test(
    int unit,
    uint8 is_egress_first)
{
    shr_error_e rv;
    global_lif_alloc_info_t alloc_info_base = { 0 };
    global_lif_alloc_info_t alloc_info_egress = { 0 };
    global_lif_alloc_info_t alloc_info_ingress = { 0 };
    global_lif_alloc_info_t *first_alloc_info = (is_egress_first == TRUE) ? &alloc_info_egress : &alloc_info_ingress;
    global_lif_alloc_info_t *second_alloc_info = (is_egress_first == TRUE) ? &alloc_info_ingress : &alloc_info_egress;
    bsl_severity_t orig_lif_severity, orig_resmngr_severity;
    int base_global_lif, one_side_symmetric_global_lif;
    uint32 first_direction_flag = (is_egress_first == TRUE) ? DNX_ALGO_LIF_EGRESS : DNX_ALGO_LIF_INGRESS;
    uint32 second_direction_flag = (is_egress_first == TRUE) ? DNX_ALGO_LIF_INGRESS : DNX_ALGO_LIF_EGRESS;

    SHR_FUNC_INIT_VARS(unit);

    cli_out("Starting one-sided symmetric test with is_egress_first = %d\n", is_egress_first);

    if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
    {
        alloc_info_base.bta = 1;
        alloc_info_egress.bta = 2;
    }
    else if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
    {
        alloc_info_base.outlif_intf = 1;
        alloc_info_egress.outlif_intf = 2;
    }
    else if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
    {
        alloc_info_base.outlif_group = 1;
        alloc_info_egress.outlif_group = 2;
    }

    /*
     * Step 1 - allocating base egress global lif
     */
    rv = dnx_algo_global_lif_allocation_allocate(unit,
                                                 (LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF |
                                                  LIF_MNGR_L2_GPORT_GLOBAL_LIF), DNX_ALGO_LIF_EGRESS, &alloc_info_base,
                                                 &base_global_lif);
    cli_out("Allocated base global lif 0x%x, egress direction, with BTA value %d, INTF value %d, GROUP value %d,\n",
            base_global_lif, alloc_info_base.bta, alloc_info_base.outlif_intf, alloc_info_base.outlif_group);

    /*
     * Step 2 - allocating the one-sided symmetric global lif first side
     */

    rv = dnx_algo_global_lif_allocation_allocate(unit,
                                                 (LIF_MNGR_ONE_SIDED_SYMMETRIC_GLOBAL_LIF |
                                                  LIF_MNGR_L2_GPORT_GLOBAL_LIF), first_direction_flag,
                                                 first_alloc_info, &one_side_symmetric_global_lif);
    if (rv != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Allocating one-sided symmetric lif with id 0x%x failed unexpectedly, is_egress_first = %d\n",
                     one_side_symmetric_global_lif, is_egress_first);
    }
    cli_out
        ("Allocated one side symmetric global lif 0x%x, is_egress_first = %d, with BTA value %d, INTF value %d, GROUP value %d,\n",
         one_side_symmetric_global_lif, is_egress_first, first_alloc_info->bta, first_alloc_info->outlif_intf,
         first_alloc_info->outlif_group);

    /*
     * Step 3 - allocating the one-sided symmetric global lif second side
     */
    /** if ingress was made first, expct faliure, thus closing the prints severity */
    if (is_egress_first == FALSE)
    {
        orig_lif_severity = bslenable_get(bslLayerBcmdnx, bslSourceLif);
        orig_resmngr_severity = bslenable_get(bslLayerBcmdnx, bslSourceResmngr);
        bslenable_set(bslLayerBcmdnx, bslSourceLif, bslSeverityOff);
        bslenable_set(bslLayerBcmdnx, bslSourceResmngr, bslSeverityOff);
    }

    rv = dnx_algo_global_lif_allocation_allocate(unit, (LIF_MNGR_GLOBAL_LIF_WITH_ID | LIF_MNGR_L2_GPORT_GLOBAL_LIF),
                                                 second_direction_flag, second_alloc_info,
                                                 &one_side_symmetric_global_lif);

    /** check test resolution */
    if (is_egress_first == TRUE)
    {
        if (rv != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Allocating one-sided symmetric second ingress lif with id 0x%x failed unexpectedly\n",
                         one_side_symmetric_global_lif);
        }
        else
        {
            cli_out("Allocated one side symmetric global lif 0x%x, ingress direction\n", one_side_symmetric_global_lif);
        }
    }
    else        /* (is_egress_first == FALSE) */
    {
        if (rv != _SHR_E_PARAM)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Allocating one-sided symmetric second egress lif with id 0x%x did not failed as expected, returned with status %d\n",
                         one_side_symmetric_global_lif, rv);
        }
        else
        {
            cli_out
                ("Failed to allocated egress global lif 0x%x, egress direction (as expected), with BTA value %d, INTF value %d, GROUP value %d,\n",
                 one_side_symmetric_global_lif, second_alloc_info->bta, second_alloc_info->outlif_intf,
                 second_alloc_info->outlif_group);
        }
    }

    /** if required, restoring the prints severity */
    if (is_egress_first == FALSE)
    {
        bslenable_set(bslLayerBcmdnx, bslSourceLif, orig_lif_severity);
        bslenable_set(bslLayerBcmdnx, bslSourceResmngr, orig_resmngr_severity);
    }

    /*
     * Step 4 - Freeing resources
     */
    if (is_egress_first == TRUE)
    {
        cli_out("Freeing the one side symmetric global lif 0x%x, from both sides\n", one_side_symmetric_global_lif);
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, one_side_symmetric_global_lif));
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, one_side_symmetric_global_lif));
    }
    else
    {
        cli_out("Freeing the one side symmetric global lif 0x%x, required only from ingress side\n",
                one_side_symmetric_global_lif);
        SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_INGRESS, one_side_symmetric_global_lif));
    }
    cli_out("Freeing the base global lif 0x%x\n", base_global_lif);
    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_free(unit, DNX_ALGO_LIF_EGRESS, base_global_lif));

    cli_out("One-sided symmetric test has ended successfully\n\n");

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_global_lif_tag_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 supported_params = 0;
    char test_parm_name[20];
    global_lif_alloc_info_t alloc_info[4];

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Step 1: Single parameters testing
     */
    if (dnx_data_lif.global_lif.global_lif_bta_max_val_get(unit))
    {
        sal_memset(alloc_info, 0, sizeof(alloc_info));
        alloc_info[0].bta = 26;
        alloc_info[1].bta = 1;
        alloc_info[2].bta = 7;
        alloc_info[3].bta = 26;
        sal_strcpy(test_parm_name, "BTA");
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_single_param_test(unit, alloc_info, test_parm_name));
        supported_params++;
    }

    if (dnx_data_lif.global_lif.global_lif_intf_max_val_get(unit))
    {
        sal_memset(alloc_info, 0, sizeof(alloc_info));
        alloc_info[0].outlif_intf = 1;
        alloc_info[1].outlif_intf = 3;
        alloc_info[2].outlif_intf = 2;
        alloc_info[3].outlif_intf = 1;
        sal_strcpy(test_parm_name, "INTF");
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_single_param_test(unit, alloc_info, test_parm_name));
        supported_params++;
    }

    if (dnx_data_lif.global_lif.global_lif_group_max_val_get(unit))
    {
        sal_memset(alloc_info, 0, sizeof(alloc_info));
        alloc_info[0].outlif_group = 1023;
        alloc_info[1].outlif_group = 434;
        alloc_info[2].outlif_group = 435;
        alloc_info[3].outlif_group = 1023;
        sal_strcpy(test_parm_name, "GROUP");
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_single_param_test(unit, alloc_info, test_parm_name));
        supported_params++;
    }

    /*
     * Step 2: Multiple parameters testing
     */
    if (supported_params > 1)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_multiple_params_test(unit, supported_params));
    }

    /*
     * Step 3: One-sided symmetric testing
     */
    if (supported_params > 0)
    {
        /** egress followed by ingress */
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_one_sided_symmetric_test(unit, TRUE));
        /** ingress followed by egress */
        SHR_IF_ERR_EXIT(ctest_dnx_global_lif_tag_one_sided_symmetric_test(unit, FALSE));
    }

    cli_out("PASS!\n");

exit:
    SHR_FUNC_EXIT;
}
