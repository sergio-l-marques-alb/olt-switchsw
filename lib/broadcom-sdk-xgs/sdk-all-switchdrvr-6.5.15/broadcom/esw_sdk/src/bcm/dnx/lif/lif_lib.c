/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_lib.c
 * $Id$ 
 * 
 * This file contains the APIs required to perform general lif HW writes.
 *  
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*************
 * INCLUDES  *
 *************/
/*
 * { 
 */

#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <soc/dnx/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>

/*
 * } 
 */
/*************
 * DEFINES   *
 *************/
/*
 * { 
 */
/**
 * \brief
 * This is how many entries are represented by each entry of outlif phase map.
 */
#define LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE      (dnx_data_lif.out_lif.physical_bank_size_get(unit))

/**
 * \brief
 * The local outlif banks are allocated per linked list bank. However, the data banks are capable of lower resolution.
 * to compensate for that, when setting the phase map, we need to set this many banks for each outlif bank.
 */
#define LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO  (dnx_data_lif.out_lif.allocation_bank_size_get(unit) \
                                                        / LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE)

/*
 * } 
 */
/*************
 * MACROS    *
 *************/
/*
 * { 
 */

/**
 * \brief
 * Translate outlif to its phase map index.
 */
#define LIF_LIB_LOCAL_OUTLIF_TO_PHASE_MAP_BANK(_local_outlif) (_local_outlif / LIF_LIB_LOCAL_OUTLIF_PHASE_MAP_BANK_SIZE)

/**
 * \brief
 * Get the key to GLEM.
 * In case of RIF there is only one GLEM entry for every 4 RIFs
 */
#define LIF_LIB_GLEM_KEY_GET(_global_lif) \
    ((dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit) && (_global_lif < dnx_data_l3.rif.nof_rifs_get(unit))) ? (_global_lif - (_global_lif % 4)) : _global_lif)
/**
 * \brief
 * Get one of 4 available outrif profile instances according to global lif index.
 */
#define LIF_LIB_GLEM_RIF_PROFILE_INSTANCE_GET(_global_lif)          (_global_lif % 4)

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

/*
 * } 
 */
/*************
 * FUNCTIONS *
 *************/
/*
 * { 
 */

/**
 * \brief Given a local outlif, and add or delete indication, writes the new configuration to
 * the MDB and pipe registers.
 */
static shr_error_e
dnx_lif_lib_outlif_bank_map_change(
    int unit,
    int local_outlif,
    dnx_algo_local_outlif_hw_info_t * outlif_hw_info,
    int delete_0_add_1)
{
    int update_ll, update_data;
    uint32 entry_handle_id;
    int current_outlif_phase_map_bank, last_outlif_phase_map_bank;

    shr_error_e(*mdb_init_func_cb) (int, int, mdb_cluster_alloc_info_t *);

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    update_ll = _SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_LL_BANK);
    update_data = _SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_DATA_BANK);
    mdb_init_func_cb = (delete_0_add_1) ? mdb_init_add_table_resources : mdb_init_delete_table_resources;
    /*
     * Start by setting the MDB.
     * Only changes in eedb banks need to be configured. MDB clusters are set on device init.
     */
    if (update_data)
    {
        SHR_IF_ERR_EXIT(mdb_init_func_cb(unit, 1, &outlif_hw_info->data_bank_info));
    }

    if (update_ll)
    {
        SHR_IF_ERR_EXIT(mdb_init_func_cb(unit, 1, &outlif_hw_info->ll_bank_info));
    }

    /*
     * Now set the phase map.
     */

    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_7, &entry_handle_id));

        current_outlif_phase_map_bank = LIF_LIB_LOCAL_OUTLIF_TO_PHASE_MAP_BANK(local_outlif);
        last_outlif_phase_map_bank = current_outlif_phase_map_bank + LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO;

        for (; current_outlif_phase_map_bank < last_outlif_phase_map_bank; current_outlif_phase_map_bank++)
        {
            /*
             * Set dbal key : The MSB of the local outlif.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_KEY_0, current_outlif_phase_map_bank);

            /*
             * Set dbal result: The logical phase it's mapped to.
             * If it's delete - just set to phase 0.
             */
            if (delete_0_add_1)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             outlif_hw_info->logical_phase);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, INST_SINGLE,
                                             DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Initiate the logical to physical phase mapping of the EEDB.
 */
static shr_error_e
dnx_lif_lib_init_phase_mapping(
    int unit)
{
    uint32 entry_handle_id;
    uint8 current_logical_phase;
    uint8 current_physical_phase;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_10, &entry_handle_id);

    /*
     * Iterate per logical phase, set all the relevant tables.
     */
    for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
         current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
    {
        /*
         * Get the mapping from sw state.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_logical_phase_to_physical_phase(unit,
                                                                              current_logical_phase,
                                                                              &current_physical_phase));
        /*
         * Set mapping.
         */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_2, current_physical_phase,
                                    current_logical_phase);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_0, current_physical_phase,
                                    current_logical_phase);

        /*
         * Set inverse mapping.
         */
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_3,
                                    current_logical_phase, current_physical_phase);
        dbal_entry_value_field8_set(unit, entry_handle_id, DBAL_FIELD_MDB_ITEM_1, current_logical_phase,
                                    current_physical_phase);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_init_rif(
    int unit)
{
    dnx_algo_local_outlif_hw_info_t *outlif_hw_infos = NULL;
    int outrif_bank_starts[MDB_MAX_NOF_CLUSTERS];
    int nof_outlif_infos, current_outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(outlif_hw_infos, sizeof(dnx_algo_local_outlif_hw_info_t) * MDB_MAX_NOF_CLUSTERS,
                       "outlif_hw_infos", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (dnx_data_l3.rif.nof_rifs_get(unit) > 0)
    {
        /*
         * First, assign all required physical databases for the rif through the algorithm.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_rif_init(unit));

        /*
         * Now write these rifs to the MDB.
         */
        SHR_IF_ERR_EXIT(dnx_algo_local_outlif_outrif_physical_bank_hw_info_get(unit, outrif_bank_starts,
                                                                               outlif_hw_infos, &nof_outlif_infos));

        for (current_outlif_info = 0; current_outlif_info < nof_outlif_infos; current_outlif_info++)
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outrif_bank_starts[current_outlif_info],
                                                               &outlif_hw_infos[current_outlif_info], 1));
        }
    }

exit:
    SHR_FREE(outlif_hw_infos);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_outlif_mdb_clusters_init(
    int unit)
{
    mdb_cluster_alloc_info_t init_clusters_info[MDB_MAX_NOF_CLUSTERS];
    int nof_init_clusters;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Initialize the outlif banks for the mdb clusters, then configure them in the
     * MDB.
     */
    sal_memset(init_clusters_info, 0, sizeof(init_clusters_info));

    SHR_IF_ERR_EXIT(dnx_algo_local_outlif_mdb_bank_database_init(unit, init_clusters_info, &nof_init_clusters));

    SHR_IF_ERR_EXIT(mdb_init_add_table_resources(unit, nof_init_clusters, init_clusters_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_init_phase_mapping(unit));
    SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_mdb_clusters_init(unit));
    SHR_IF_ERR_EXIT(dnx_lif_lib_init_rif(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/*
 * Note: no input verification is done here. It's all assumed to be done in dnx_lif_mngr_lif_allocate.
 */
shr_error_e
dnx_lif_lib_allocate(
    int unit,
    uint32 flags,
    int *global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    lif_mngr_local_outlif_info_t * outlif_info)
{
    dnx_algo_local_outlif_hw_info_t outlif_hw_info;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_lif_allocate(unit, flags, global_lif, inlif_info, outlif_info, &outlif_hw_info));

    if (outlif_info && _SHR_IS_FLAG_SET(outlif_hw_info.flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
    {
        /*
         * If this is the first outlif using this outlif bank, then update the HW accordingly.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outlif_info->local_outlif, &outlif_hw_info, 1));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_inlif_allocate(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, inlif_info, NULL));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_outlif_allocate(
    int unit,
    lif_mngr_local_outlif_info_t * outlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, outlif_info));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Note: no input verification is done here. It's all assumed to be done in dnx_lif_mngr_lif_allocate.
 */
shr_error_e
dnx_lif_lib_free(
    int unit,
    int global_lif,
    lif_mngr_local_inlif_info_t * inlif_info,
    int local_outlif)
{
    lif_mngr_local_outlif_info_t outlif_info, *outlif_info_ptr;
    dnx_algo_local_outlif_hw_info_t outlif_hw_info;
    SHR_FUNC_INIT_VARS(unit);

    if (local_outlif != LIF_MNGR_INVALID)
    {
        sal_memset(&outlif_info, 0, sizeof(outlif_info));
        outlif_info.local_outlif = local_outlif;
        outlif_info_ptr = &outlif_info;
    }
    else
    {
        outlif_info_ptr = NULL;
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_lif_free(unit, global_lif, inlif_info, outlif_info_ptr, &outlif_hw_info));

    if (outlif_info_ptr && _SHR_IS_FLAG_SET(outlif_hw_info.flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
    {
        /*
         * If this is the last outlif using this outlif bank, then update the HW accordingly.
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_bank_map_change(unit, outlif_info_ptr->local_outlif, &outlif_hw_info, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_inlif_free(
    int unit,
    lif_mngr_local_inlif_info_t * inlif_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, inlif_info, LIF_MNGR_INVALID));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_outlif_free(
    int unit,
    int local_outlif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, local_outlif));
exit:
    SHR_FUNC_EXIT;
}

/**
 * The arguments for the verify function are identical to the parent function's arguments.
 * Just verify that they are all in legal range. 
 *  
 * \see 
 *  dnx_lif_lib_add_to_glem
 */
static shr_error_e
dnx_lif_lib_add_to_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

    LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_add_to_glem_internal(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint32 outlif_profile,
    uint8 is_replace)
{
    uint32 entry_handle_id;
    int core_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_add_to_glem_verify(unit, core_id, global_lif, local_lif));

    /*
     * Iterate per core, or just on one core.
     */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
        /*
         * Set dbal key : Global lif + core.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, LIF_LIB_GLEM_KEY_GET(global_lif));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);
        /*
         * Set dbal result type according to lif range.
         */
        if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_GLOBAL_LIF_EM_RIF_TYPE);
            /*
             * Set dbal result: The lif profile.
             */
            if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                             LIF_LIB_GLEM_RIF_PROFILE_INSTANCE_GET(global_lif), outlif_profile);
            }
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                         DBAL_RESULT_TYPE_GLOBAL_LIF_EM_LIF_TYPE);
            /*
             * Set dbal result: The local lif + lif profile.
             */
            if ((!is_replace) || (local_lif != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif);
            }
            if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                             outlif_profile);
            }
        }

        if (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit)
            && (global_lif < dnx_data_l3.rif.nof_rifs_get(unit)))
        {
            /** The entry might already exist in the GLEM because used for several RIFs */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
        }
        else if (is_replace)
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_add_to_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem_internal(unit, core_id, global_lif, local_lif, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_replace_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 local_lif,
    uint32 outlif_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem_internal(unit, core_id, global_lif, local_lif, outlif_profile, 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * The arguments for the verify function are identical to the parent function's arguments.
 * Just verify that they are all not NULL and in legal range
 * Note that if specified 'global_lif' is marked as 'not written to GLEM' then it is an error
 * to try and read it from GLEM.
 *  
 * \see 
 *  dnx_lif_lib_read_from_glem
 */
static shr_error_e
dnx_lif_lib_read_from_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif,
    uint32 *outlif_profile)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_profile, _SHR_E_PARAM, "outlif_profile");
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
    if (global_lif >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_NULL_CHECK(local_lif, _SHR_E_PARAM, "local_lif");
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lib_read_from_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif_p,
    uint32 *outlif_profile_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_read_from_glem_verify(unit, core_id, global_lif, local_lif_p, outlif_profile_p));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
    /*
     * Set dbal key : Global lif + core.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, LIF_LIB_GLEM_KEY_GET(global_lif));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /*
     * Set dbal result type according to lif range.
     */
    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_GLOBAL_LIF_EM_RIF_TYPE);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                   LIF_LIB_GLEM_RIF_PROFILE_INSTANCE_GET(global_lif), outlif_profile_p);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_GLOBAL_LIF_EM_LIF_TYPE);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif_p);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                   outlif_profile_p);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * The arguments for the verify function are identical to the parent function's arguemnts. 
 * Just verify that they are all in legal range. 
 *  
 * \see 
 *  dnx_lif_lib_remove_from_glem
 */
static shr_error_e
dnx_lif_lib_remove_from_glem_verify(
    int unit,
    int core_id,
    uint32 global_lif)
{
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_remove_from_glem(
    int unit,
    int core_id,
    uint32 global_lif)
{
    uint32 entry_handle_id;
    int core_index;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_remove_from_glem_verify(unit, core_id, global_lif));
    {
        /*
         * Iterate per core, or just on one core.
         */
        DNXCMN_CORES_ITER(unit, core_id, core_index)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
            /*
             * Set dbal key : Global lif + core.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF,
                                       LIF_LIB_GLEM_KEY_GET(global_lif));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);

            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * } 
 */
