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

/*****************
 * INCLUDE FILES *
 * ***************
 */
/*
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>

#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <bcm_int/dnx/lif/lif_lib.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/**
 * }
 */

/*************
 * DEFINES   *
 *************/
/*
 * {
 */

#define NOF_INLIFS_FOR_TEST         400

#define NOF_RESOURCES_TO_COMPARE    2

/*
 * }
 */
/*************
 * MACROS    *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 *************/
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 *************/
/*
 * {
 */
extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);
/**
 * \brief
 * Man for inlif test.
 */
sh_sand_man_t dnx_inlif_semantic_test_man = {
    "Local inlif semantic test.",
    "Perform a semantic test of local inlif. allocate/get/free, testing different scenarios."
};

sh_sand_man_t dnx_inlif_allocation_replace_semantic_test_man = {
    "Semantic test for local inlif allocation replace.",
    "Create an inlif, then replace it in several different ways."
};

/*
 * }
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * {
 */

static shr_error_e
ctest_dnx_local_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * lif_info,
    int *current_index,
    int *global_lif_array,
    int *local_lif_array)
{
    SHR_FUNC_INIT_VARS(unit);

    lif_info->local_inlif = local_lif_array[*current_index - 1];

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif_array[*current_index - 1], lif_info, LIF_MNGR_INVALID));

exit:
    (*current_index)--;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given an inlif resource, frees all of its instances. ignore_erros can be set in case this is called after
 * the exit label, in which case we don't care if any single free fails, we just want to clean as much as possible.
 */
static shr_error_e
ctest_dnx_local_inlif_free_all(
    int unit,
    int ignore_errors,
    lif_mngr_local_inlif_info_t * lif_info,
    int *current_index,
    int *global_lif_array,
    int *local_lif_array)
{
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    while (*current_index > 0)
    {
        rv = (ctest_dnx_local_inlif_free(unit, lif_info, current_index, global_lif_array, local_lif_array));

        if (!ignore_errors)
        {
            SHR_IF_ERR_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 * Allocate a single inlif element.
 */
static shr_error_e
ctest_dnx_local_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * lif_info,
    int *current_index,
    int *global_lif_array,
    int *local_lif_array)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &(global_lif_array[*current_index]), lif_info, NULL));

    local_lif_array[*current_index] = lif_info->local_inlif;
    (*current_index)++;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given a local inlif resource, allocates entries until a certain element is reached.
 * success indicates whether this element was reached (TRUE), or passed (FALSE).
 */
static shr_error_e
ctest_dnx_local_inlif_allocate_up_to_local_lif_id(
    int unit,
    int required_inlif,
    lif_mngr_local_inlif_info_t * lif_info,
    int *current_index,
    int *global_lif_array,
    int *local_lif_array,
    int *success)
{
    SHR_FUNC_INIT_VARS(unit);

    while (TRUE)
    {
        if (lif_info->local_inlif == required_inlif)
        {
            *success = TRUE;
            break;
        }
        else if (lif_info->local_inlif > required_inlif)
        {
            *success = FALSE;
            break;
        }
        else if (*current_index == NOF_INLIFS_FOR_TEST)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Too many inlifs allocated.");
        }

        SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocate
                        (unit, lif_info, current_index, global_lif_array, local_lif_array));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Given two inlif resources, make it so the last element allocated for both is the same element.
 * expect_success should be set to indicate whether both of them should be able to get the same element,
 * or not. If the success is not as expected, an error will be returned.
 */
static shr_error_e
ctest_dnx_local_inlif_bring_resources_to_same_element(
    int unit,
    int expect_success,
    lif_mngr_local_inlif_info_t * lif_info[NOF_RESOURCES_TO_COMPARE],
    int *current_index[NOF_RESOURCES_TO_COMPARE],
    int *global_lif_array[NOF_RESOURCES_TO_COMPARE],
    int *local_lif_array[NOF_RESOURCES_TO_COMPARE])
{
    int current_resource, target_inlif;
    int first_inlif, second_inlif;
    int tmp_success, success;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Start by allocating a single element on both resources.
     */
    for (current_resource = 0; current_resource < NOF_RESOURCES_TO_COMPARE; current_resource++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocate(unit,
                                                       lif_info[current_resource],
                                                       current_index[current_resource],
                                                       global_lif_array[current_resource],
                                                       local_lif_array[current_resource]));
    }

    /*
     * If the allocated elements are not the same, then allocate on both resources until
     *      both are on the same element.
     */
    first_inlif = local_lif_array[0][*(current_index[0]) - 1];
    second_inlif = local_lif_array[1][*(current_index[1]) - 1];
    if (first_inlif == second_inlif)
    {
        /*
         * target_inlif -1 indicates that no further allocation is needed.
         */
        target_inlif = -1;
    }
    else if (first_inlif < second_inlif)
    {
        target_inlif = second_inlif;
    }
    else
    {
        target_inlif = first_inlif;
    }

    success = TRUE;
    tmp_success = FALSE;

    if (target_inlif != -1)
    {
        /*
         * The most likely next common inlif is the next inlif that is divisible by the size of the n-pack.
         */
        target_inlif = UTILEX_ALIGN_UP(target_inlif, dnx_data_lif.in_lif.inlif_resource_tag_size_get(unit));

        /*
         * Advance both resources up to this inlif.
         */
        for (current_resource = 0; current_resource < NOF_RESOURCES_TO_COMPARE; current_resource++)
        {
            SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocate_up_to_local_lif_id(unit,
                                                                              target_inlif,
                                                                              lif_info[current_resource],
                                                                              current_index[current_resource],
                                                                              global_lif_array[current_resource],
                                                                              local_lif_array[current_resource],
                                                                              &tmp_success));

            if (!tmp_success && expect_success)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Can't get same element on both resources.");
            }
            else if (!expect_success)
            {
                success = success && tmp_success;
            }
        }
    }
    else
    {
        /*
         * If they're equal, then just use the current id of one of them.
         */
        target_inlif = first_inlif;
    }

    if (success && !expect_success)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Got the same element on both resources even though wasn't supposed to.");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Compare two inlif resources.
 * a. Bring both resources to the same starting index.
 * b. Get both of them, and make sure that they're equal to the info used to create them.
 * c. Verify that the info we got is not identical between the two.
 * d. Delete the newest lifs. They shouldn't interfere with each other.
 */
static shr_error_e
ctest_dnx_local_inlif_compare_two_resources(
    int unit,
    lif_mngr_local_inlif_info_t * lif_info[NOF_RESOURCES_TO_COMPARE],
    int *current_index[NOF_RESOURCES_TO_COMPARE],
    int *global_lif_array[NOF_RESOURCES_TO_COMPARE],
    int *local_lif_array[NOF_RESOURCES_TO_COMPARE])
{
    int current_resource;
    int gports[NOF_RESOURCES_TO_COMPARE];
    dnx_algo_gpm_gport_hw_resources_t gports_hw_res[NOF_RESOURCES_TO_COMPARE];
    lif_mapping_local_lif_info_t lif_mapping_info;
    int global_lifs[NOF_RESOURCES_TO_COMPARE], current_global_lif;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * a. Bring both resources to the same starting index.
     */
    cli_out("  a. Bring both resources to the same starting index.\n");
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_bring_resources_to_same_element
                    (unit, TRUE, lif_info, current_index, global_lif_array, local_lif_array));

    /*
     * b. The two different lif resources now have the same local inilf id.
     *      Get both of them, and make sure that they're equal to the info used to create them.
     *      There are two ways to do it:
     *    1. Turn the global lif into tunnel gport, and get its hw resources.
     *    2. Call the lif mapping to get the local lif from the global lif.
     */
    cli_out("  b. Get both of them, and make sure that they're equal to the info used to create them.\n");
    sal_memset(&gports_hw_res, 0, sizeof(gports_hw_res));
    sal_memset(&global_lifs, 0, sizeof(global_lifs));
    for (current_resource = 0; current_resource < NOF_RESOURCES_TO_COMPARE; current_resource++)
    {
        /*
         * 1. Turn the global lif to tunnel, and get its hw resources.
         */
        cli_out("    %d. Turn global lif #%d to tunnel, and get its hw resources.\n", current_resource + 1,
                current_resource * 2 + 1);
        current_global_lif = global_lif_array[current_resource][(*current_index[current_resource]) - 1];
        BCM_L3_ITF_SET(current_global_lif, BCM_L3_ITF_TYPE_LIF, current_global_lif);
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gports[current_resource], current_global_lif);

        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                        (unit, gports[current_resource],
                         DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gports_hw_res[current_resource]));

        /*
         * Verify that the hw resources match the lif info.
         */
        SHR_VAL_VERIFY(gports_hw_res[current_resource].local_in_lif, lif_info[current_resource]->local_inlif,
                       _SHR_E_FAIL, "Mismatching local inlif");
        SHR_VAL_VERIFY(gports_hw_res[current_resource].local_in_lif_core, lif_info[current_resource]->core_id,
                       _SHR_E_FAIL, "Mismatching core");
        SHR_VAL_VERIFY(gports_hw_res[current_resource].inlif_dbal_table_id, lif_info[current_resource]->dbal_table_id,
                       _SHR_E_FAIL, "Mismatching dbal table id");
        SHR_VAL_VERIFY(gports_hw_res[current_resource].inlif_dbal_result_type,
                       lif_info[current_resource]->dbal_result_type, _SHR_E_FAIL, "Mismatching dbal result type");

        /*
         * 2. Get the global lif from the local lif, and compare it to the one allocated.
         */
        cli_out("    %d. Get global lif #%d from the local lif, and compare it to the one allocated.\n",
                current_resource + 1, current_resource * 2 + 2);
        sal_memset(&lif_mapping_info, 0, sizeof(lif_mapping_info));
        lif_mapping_info.local_lif = lif_info[current_resource]->local_inlif;
        lif_mapping_info.core_id = lif_info[current_resource]->core_id;
        SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                        (unit, lif_info[current_resource]->dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX,
                         &lif_mapping_info.phy_table));

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_INGRESS, &lif_mapping_info, &(global_lifs[current_resource])));
        SHR_VAL_VERIFY(global_lifs[current_resource], BCM_L3_ITF_VAL_GET(current_global_lif), _SHR_E_FAIL,
                       "Mismatching global lif");
    }

    /*
     * c. Verify that the gport hw res and the global lifs we got for each resource are different.
     */
    cli_out("  c. Verify that the gport hw res and the global lifs we got for each resource are different.\n");
    if (global_lifs[0] == global_lifs[1])
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Global lifs for different inlif resources are different.");
    }

    if (sal_memcmp(&gports_hw_res[0], &gports_hw_res[1], sizeof(dnx_algo_gpm_gport_hw_resources_t)) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "HW resources for different inlif resources are different.");
    }

    /*
     * d. Delete the newest lifs. They shouldn't interfere with each other.
     */
    cli_out("  d. Delete the newest lifs. They shouldn't interfere with each other.\n");
    for (current_resource = 0; current_resource < NOF_RESOURCES_TO_COMPARE; current_resource++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_free(unit, lif_info[current_resource], current_index[current_resource],
                                                   global_lif_array[current_resource],
                                                   local_lif_array[current_resource]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**Clear inlif resources*/
shr_error_e
ctest_dnx_local_inlif_clear_current_resources(
    int unit)
{
    uint32 entry_handle_id;
    int is_end;
    lif_mngr_local_inlif_info_t lif_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    lif_info.core_id = _SHR_CORE_ALL;
    /**Iterate over all lifs and free them*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_INLIF_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF,
                                              (uint32 *) &lif_info.local_inlif);
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE,
                                            &lif_info.dbal_table_id);
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE,
                                            (uint32 *) &lif_info.dbal_result_type);
        SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &lif_info, LIF_MNGR_INVALID));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_INLIF_INFO_SW, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    while (!is_end)
    {
        dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF,
                                              (uint32 *) &lif_info.local_inlif);
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DBAL_APP_DB_NAME, INST_SINGLE,
                                            &lif_info.dbal_table_id);
        dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, INST_SINGLE,
                                            (uint32 *) &lif_info.dbal_result_type);
        SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, &lif_info, LIF_MNGR_INVALID));

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The local inlif semantic test runs a semantic test mainly verifying that all lif table resouces (sbc inlif and each
 * instance of dpc inlif) can use the same addresses without conflicts to the create / add / destroy functionality.
 *
 * The resources we compare are:
 * 1. dpc inlif for all covers vs. sbc inlif.
 * 2. Each single core instance of dpc inlif.
 * 3. A single instance of dpc inlif vs. the all cores dpc inlif - these should actually not have the same addresses, since
 *      they represent the same entity, and the same entity can't be used twice.
 */
shr_error_e
ctest_dnx_local_inlif_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int global_sbc_inlifs[NOF_INLIFS_FOR_TEST], global_dpc_all_cores_inlifs[NOF_INLIFS_FOR_TEST],
        global_dpc_inlifs[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][NOF_INLIFS_FOR_TEST];
    int local_sbc_inlifs[NOF_INLIFS_FOR_TEST], local_dpc_all_cores_inlifs[NOF_INLIFS_FOR_TEST],
        local_dpc_inlifs[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][NOF_INLIFS_FOR_TEST];
    int sbc_index, dpc_all_cores_index, dpc_index[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];
    lif_mngr_local_inlif_info_t sbc_inlif_info, dpc_all_cores_inlif_info,
        dpc_inlif_info[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES];

    lif_mngr_local_inlif_info_t *lif_info[NOF_RESOURCES_TO_COMPARE];
    int *current_index[NOF_RESOURCES_TO_COMPARE],
        *global_lif_array[NOF_RESOURCES_TO_COMPARE], *local_lif_array[NOF_RESOURCES_TO_COMPARE];

    int current_core;

    SHR_FUNC_INIT_VARS(unit);

    /**Save current status in journal*/
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    cli_out("0. Local inlif semantic test - start.\n");

    /**Clean all inlifs, later restore them through journal*/
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_clear_current_resources(unit));

    /*
     * First, set everything to 0.
     */
    sal_memset(global_sbc_inlifs, 0, sizeof(global_sbc_inlifs));
    sal_memset(global_dpc_all_cores_inlifs, 0, sizeof(global_dpc_all_cores_inlifs));
    sal_memset(global_dpc_inlifs, 0, sizeof(global_dpc_inlifs));
    sal_memset(local_sbc_inlifs, 0, sizeof(local_sbc_inlifs));
    sal_memset(local_dpc_all_cores_inlifs, 0, sizeof(local_dpc_all_cores_inlifs));
    sal_memset(local_dpc_inlifs, 0, sizeof(local_dpc_inlifs));
    sal_memset(&sbc_inlif_info, 0, sizeof(sbc_inlif_info));
    sal_memset(&dpc_all_cores_inlif_info, 0, sizeof(dpc_all_cores_inlif_info));
    sal_memset(dpc_inlif_info, 0, sizeof(dpc_inlif_info));

    sbc_index = 0;
    dpc_all_cores_index = 0;
    sal_memset(dpc_index, 0, sizeof(dpc_index));

    /*
     * Set the lif info for every type of lif.
     * These were chosen randomally from the valid dbal tables, but the result type is supposed to be
     */

    sbc_inlif_info.dbal_table_id = DBAL_TABLE_IN_AC_INFO_DB;
    sbc_inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_P2P_W_1_VLAN;
    sbc_inlif_info.core_id = _SHR_CORE_ALL;

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
    {
        dpc_inlif_info[current_core].core_id = current_core;
        if (current_core % 2 == 0)
        {
            dpc_inlif_info[current_core].dbal_table_id = DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT;
            dpc_inlif_info[current_core].dbal_result_type =
                DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT_IN_ETH_VLAN_EDIT_VSI_MP;
        }
        else
        {
            dpc_inlif_info[current_core].dbal_table_id = DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION;
            dpc_inlif_info[current_core].dbal_result_type =
                DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_IN_ETH_AC_MP;
        }
    }

    dpc_all_cores_inlif_info.dbal_table_id = DBAL_TABLE_IN_LIF_FORMAT_PWE;
    dpc_all_cores_inlif_info.dbal_result_type = DBAL_RESULT_TYPE_IN_LIF_FORMAT_PWE_IN_LIF_PWE_MP;
    dpc_all_cores_inlif_info.core_id = _SHR_CORE_ALL;

    /*
     * Compare the sbc local inlif resouce with the all cores local inlif.
     */
    lif_info[0] = &dpc_all_cores_inlif_info;
    current_index[0] = &dpc_all_cores_index;
    global_lif_array[0] = global_dpc_all_cores_inlifs;
    local_lif_array[0] = local_dpc_all_cores_inlifs;

    lif_info[1] = &sbc_inlif_info;
    current_index[1] = &sbc_index;
    global_lif_array[1] = global_sbc_inlifs;
    local_lif_array[1] = local_sbc_inlifs;

    cli_out("1. Compare dpc_all_cores inlif with sbc inlif\n");
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_compare_two_resources
                    (unit, lif_info, current_index, global_lif_array, local_lif_array));

    if (DNXCMN_CHIP_IS_MULTI_CORE(unit))
    {
        /*
         * If chip is multi core, then run the comparison test for cores 0 and 1, and then verify that all_cores and either core can't have
         * the same lif ID allocated.
         */
        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
        {
            if (current_core == NOF_RESOURCES_TO_COMPARE)
            {
                /*
                 * In case this device has more cores than the number of resouces we want to compare,
                 * break the loop.
                 */
                break;
            }
            lif_info[current_core] = &(dpc_inlif_info[current_core]);
            current_index[current_core] = &(dpc_index[current_core]);
            global_lif_array[current_core] = global_dpc_inlifs[current_core];
            local_lif_array[current_core] = local_dpc_inlifs[current_core];
        }

        cli_out("2. Compare dpc inlif for cores 0 and 1.\n");
        SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_compare_two_resources
                        (unit, lif_info, current_index, global_lif_array, local_lif_array));

        /*
         * Now verify that dpc_all_cors inlif can't have the same id as either core.
         */
        lif_info[0] = &dpc_all_cores_inlif_info;
        current_index[0] = &dpc_all_cores_index;
        global_lif_array[0] = global_dpc_all_cores_inlifs;
        local_lif_array[0] = local_dpc_all_cores_inlifs;

        DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
        {
            if (current_core == NOF_RESOURCES_TO_COMPARE)
            {
                /*
                 * In case this device has more cores than the number of resouces we want to compare,
                 * break the loop.
                 */
                break;
            }

            lif_info[1] = &(dpc_inlif_info[current_core]);
            current_index[1] = &(dpc_index[current_core]);
            global_lif_array[1] = global_dpc_inlifs[current_core];
            local_lif_array[1] = local_dpc_inlifs[current_core];
            cli_out
                ("3.%d. Compare dpc_all_cores inlif with dpc core %d inlif. Should not be able to use the same index.\n",
                 current_core, current_core);
            SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_bring_resources_to_same_element
                            (unit, FALSE, lif_info, current_index, global_lif_array, local_lif_array));
        }
    }

    /*
     * Now free all. Expect the free operation to pass without errors.
     */
    cli_out("4. Free all allocated inlifs.\n");
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_free_all
                    (unit, TRUE, &sbc_inlif_info, &sbc_index, global_sbc_inlifs, local_sbc_inlifs));
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_free_all
                    (unit, TRUE, &dpc_all_cores_inlif_info, &dpc_all_cores_index, global_dpc_all_cores_inlifs,
                     local_dpc_all_cores_inlifs));

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_free_all(unit, TRUE,
                                                       &dpc_inlif_info[current_core],
                                                       &dpc_index[current_core],
                                                       global_dpc_inlifs[current_core],
                                                       local_dpc_inlifs[current_core]));
    }

exit:
    /*
     * Free all allocated inlifs. The only reason for inlifs to be allocated is if something failed,
     * otherwise they should already be freed. Because of that, we ignore errors.
     */
    ctest_dnx_local_inlif_free_all(unit, FALSE, &sbc_inlif_info, &sbc_index, global_sbc_inlifs, local_sbc_inlifs);
    ctest_dnx_local_inlif_free_all(unit, FALSE, &dpc_all_cores_inlif_info, &dpc_all_cores_index,
                                   global_dpc_all_cores_inlifs, local_dpc_all_cores_inlifs);

    DNXCMN_CORES_ITER(unit, _SHR_CORE_ALL, current_core)
    {
        ctest_dnx_local_inlif_free_all(unit, FALSE,
                                       &dpc_inlif_info[current_core],
                                       &dpc_index[current_core],
                                       global_dpc_inlifs[current_core], local_dpc_inlifs[current_core]);
    }

    /*
     * rollback modifications done through journal
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

    cli_out("Local inlif semantic test - %s\n", SHR_FUNC_ERR()? "failed :(" : "success!");

    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_local_inlif_allocation_replace(
    int unit,
    lif_mngr_local_inlif_info_t * local_inlif_info,
    int global_lif,
    int new_result_type,
    int test_case,
    int should_be_reused)
{
    int gport;
    int can_be_reused;
    dnx_algo_gpm_gport_hw_resources_t gport_hw_res;
    lif_mapping_local_lif_info_t lif_mapping_info;
    lif_mngr_local_inlif_info_t tmp_lif;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify that we can replace the large entry with the small entry.
     */
    /**Set the old result type before the replace*/
    local_inlif_info->old_result_type = local_inlif_info->dbal_result_type;
    local_inlif_info->dbal_result_type = new_result_type;
    local_inlif_info->local_lif_flags = 0;

    SHR_IF_ERR_EXIT(dnx_algo_local_inlif_can_be_reused(unit, local_inlif_info, &can_be_reused, NULL));

    if ((should_be_reused && !can_be_reused) || (!should_be_reused && can_be_reused))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error: should%s be able to reuse entry in test case %d.",
                     (should_be_reused ? "" : " not"), test_case);
    }

    if (!can_be_reused)
    {
        /**Save the lif*/
        sal_memcpy(&tmp_lif, local_inlif_info, sizeof(lif_mngr_local_inlif_info_t));
        /*
         * If entry can't be reused - then reserve a new one and delete the old one.
         */
        local_inlif_info->local_lif_flags |= DNX_ALGO_LIF_MNGR_LOCAL_INLIF_RESERVE;
        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, local_inlif_info));

        /*
         * Delete the old entry's global lif mapping.
         *
         */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, global_lif));

        /*
         * Delete the old entry.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &tmp_lif));

        /*
         * Map the new entry to the global lif.
         */
        sal_memset(&lif_mapping_info, 0, sizeof(lif_mapping_info));
        lif_mapping_info.local_lif = local_inlif_info->local_inlif;
        lif_mapping_info.core_id = _SHR_CORE_ALL;
        lif_mapping_info.phy_table = DBAL_PHYSICAL_TABLE_INLIF_2;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create(unit, DNX_ALGO_LIF_INGRESS, global_lif, &lif_mapping_info));
    }

    /*
     * Now replace entry. Call lif mngr directly because it can't be done through the lif lib.
     */
    local_inlif_info->local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_INLIF_REPLACE;
    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, local_inlif_info));

    /*
     * Now turn the global lif into a gport, and verify new result type.
     */
    BCM_L3_ITF_SET(gport, BCM_L3_ITF_TYPE_LIF, global_lif);
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, gport);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources
                    (unit, gport, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS, &gport_hw_res));

    if (gport_hw_res.inlif_dbal_result_type != local_inlif_info->dbal_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error: didn't get correct result type in test case %d. "
                     "Expected %d but was %d", test_case, local_inlif_info->dbal_result_type,
                     gport_hw_res.inlif_dbal_result_type);
    }

    local_inlif_info->local_lif_flags = 0;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
ctest_dnx_local_inlif_allocation_replace_semantic_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    lif_mngr_local_inlif_info_t local_inlif_info;
    int global_lif;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&local_inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));
    local_inlif_info.dbal_table_id = DBAL_TABLE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES;
    local_inlif_info.dbal_result_type = DBAL_RESULT_TYPE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES_RT1;
    local_inlif_info.core_id = _SHR_CORE_ALL;
    local_inlif_info.local_lif_flags = 0;
    local_inlif_info.local_inlif = 0;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &global_lif, &local_inlif_info, NULL));

    /**Add one extra entry, to ensure non-empty 6th-pack*/
    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, 0, &global_lif, &local_inlif_info, NULL));

    /*
     * Test case 1: Replace entry of 120b to entry of 120b, expect re-use of the same entry.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocation_replace(unit, &local_inlif_info, global_lif,
                                                             DBAL_RESULT_TYPE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES_RT2,
                                                             1, TRUE));

    /*
     * Test case 2: Now try to replace from 120 to 180, expect reserving a new entry.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocation_replace(unit, &local_inlif_info, global_lif,
                                                             DBAL_RESULT_TYPE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES_RT3,
                                                             2, FALSE));

    /*
     * Test case 3: Now try to replace from 180 to 120, expect re-use of the same entry.
     */
    SHR_IF_ERR_EXIT(ctest_dnx_local_inlif_allocation_replace(unit, &local_inlif_info, global_lif,
                                                             DBAL_RESULT_TYPE_EXAMPLE_IN_LIF_FORMAT_DIFFERENT_SIZES_RT1,
                                                             3, FALSE));

exit:
    /*
     * Now free the outlif.
     */
    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, global_lif, &local_inlif_info, LIF_MNGR_INVALID));

    cli_out("Local inlif semantic test - %s\n", SHR_FUNC_ERR()? "failed :(" : "success!");

    SHR_FUNC_EXIT;
}

/*
 * }
 */
