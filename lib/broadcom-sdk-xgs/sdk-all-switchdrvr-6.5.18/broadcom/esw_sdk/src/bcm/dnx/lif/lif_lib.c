/*
 * $Copyright: (c) 2019 Broadcom.
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
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mdb.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/swstate/auto_generated/access/algo_lif_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/swstate/auto_generated/access/mdb_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_l3_access.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>

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
#define LIF_LIB_GLEM_RIF_KEY_GET(_global_lif) \
    (dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit) ? (_global_lif - (_global_lif % 4)) : _global_lif)
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

    /*
     * Update mdb phase map only for clusters (DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS) For add - update for
     * first outlif which allocates this bank For delete - update once all the relevant outlifs are removed (last
     * outlif) 
     */
    if (_SHR_IS_FLAG_SET(outlif_hw_info->flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_POINTERS))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_7, &entry_handle_id));

        /**Align the phase map bank, depends on the mapping ratio. For allocation size=8k, physical size=4k --> the ratio is 2, first 4k are added to first bank and the next 4k to the next. once all those 8k outlifs are removed - need to clear both banks. We normalize it according to the ratio*/
        current_outlif_phase_map_bank =
            (LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO) * (LIF_LIB_LOCAL_OUTLIF_TO_PHASE_MAP_BANK(local_outlif) /
                                                              (LIF_LIB_LOCAL_OUTLIF_BANK_TO_PHASE_MAP_RATIO));
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
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_MDB_10, &entry_handle_id));

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

/** returns if the global lif is from type lif or rif */
static shr_error_e
dnx_lif_lib_global_lif_type_get(
    int unit,
    uint32 global_lif,
    int *is_rif)
{
    int iop_mode_outlif_selection, system_headers_mode;

    SHR_FUNC_INIT_VARS(unit);

    iop_mode_outlif_selection = dnx_data_lif.hw_bug.feature_get(unit, dnx_data_lif_hw_bug_iop_mode_outlif_selection);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    if (((global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
         && (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE))
        || ((global_lif < dnx_data_l3.rif.nof_rifs_get(unit)) && (iop_mode_outlif_selection == FALSE)))
    {
        (*is_rif) = 1;
    }
    else
    {
        (*is_rif) = 0;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_lif_lib_outlif_traffic_lock_bug_workaround(
    int unit)
{
    int outlif_physical_phase_by_size[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] = { 7, 0, 6, 1, 5, 2, 4, 3 };
    int outlif_phase_allocation_order[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];
    lif_mngr_outlif_phase_e outlif_phase_per_logical_phase[DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT] =
        { LIF_MNGR_OUTLIF_PHASE_RIF,
        LIF_MNGR_OUTLIF_PHASE_NATIVE_ARP,
        LIF_MNGR_OUTLIF_PHASE_VPLS_1,
        LIF_MNGR_OUTLIF_PHASE_MPLS_TUNNEL_2,
        LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_3,
        LIF_MNGR_OUTLIF_PHASE_IP_TUNNEL_4,
        LIF_MNGR_OUTLIF_PHASE_ARP,
        LIF_MNGR_OUTLIF_PHASE_AC
    };
    int allocated_outlifs[2][DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT];
    uint8 current_logical_phase, current_logical_phase_index;
    uint8 current_physical_phase, current_stack;
    lif_mngr_local_outlif_info_t local_outlif_info;

    int global_lifs[2], *global_lif_p;
    uint32 flags, entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Allocate the sw state for the workaround.
     */
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock_workaround.
                        local_lifs.alloc(unit, current_stack, DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT));
    }

    /*
     * Determine the phase allocation order.
     * We should go from smallest to largest so we have enough eedb banks for all phases.
     * outlif_physical_phase_by_size maps from physical phase to the order it should be allocated in.
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
        outlif_phase_allocation_order[outlif_physical_phase_by_size[current_physical_phase]] = current_logical_phase;
    }

    /*
     * Now, allocate for each logical phase two eedb entry with linked list in a new eedb banks, and save the allocated
     * outlifs in sw state.
     */
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        for (current_logical_phase_index = 0;
             current_logical_phase_index < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase_index++)
        {

            /*
             * Set up the arguments.
             * Just pick IPv6 data entry because it has linked list and it doesn't matter.
             */
            current_logical_phase = outlif_phase_allocation_order[current_logical_phase_index];

            sal_memset(&local_outlif_info, 0, sizeof(local_outlif_info));
            local_outlif_info.dbal_table_id = DBAL_TABLE_EEDB_DATA_ENTRY;
            local_outlif_info.dbal_result_type = DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6;
            local_outlif_info.outlif_phase = outlif_phase_per_logical_phase[current_logical_phase];

            local_outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_NEW_BANK_ALLOCATION
                | DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_FORCE_DATA_IN_EEDB_BANKS;

            if (current_logical_phase == DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST)
            {
                /*
                 * Only the first logical_phase needs a linked list.
                 */
                global_lif_p = &global_lifs[current_stack];
                flags = 0;
            }
            else
            {
                global_lif_p = NULL;
                flags = LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF;
            }

            /*
             * Allocate the outlif.
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, flags, global_lif_p, NULL, &local_outlif_info));

            allocated_outlifs[current_stack][current_logical_phase] = local_outlif_info.local_outlif;
        }
    }

    /*
     * Now link the outlifs in HW.
     */
    DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EEDB_DATA_ENTRY, &entry_handle_id);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_EEDB_DATA_ENTRY_DIP6);

    /*
     * Iterate per logical phase, set all the relevant tables.
     */
    for (current_stack = 0; current_stack < 2; current_stack++)
    {

        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
             current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT - 1; current_logical_phase++)
        {

            /*
             * Set key.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF,
                                       allocated_outlifs[current_stack][current_logical_phase]);

            /*
             * Set linked list pointer.
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                                         INST_SINGLE, allocated_outlifs[current_stack][current_logical_phase + 1]);

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

    /*
     * Fill the GLEM with the global lifs.
     */
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem
                        (unit, _SHR_CORE_ALL, global_lifs[current_stack], allocated_outlifs[current_stack][0]));
    }

    /*
     * Now save the local and global outlifs to sw state.
     */
    for (current_stack = 0; current_stack < 2; current_stack++)
    {
        SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock_workaround.
                        global_lif_destination.set(unit, current_stack, global_lifs[current_stack]));

        for (current_logical_phase = DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_FIRST;
             current_logical_phase < DNX_ALGO_LOCAL_OUTLIF_LOGICAL_PHASE_COUNT; current_logical_phase++)
        {
            SHR_IF_ERR_EXIT(mdb_db_infos.eedb_bank_traffic_lock_workaround.
                            local_lifs.set(unit, current_stack, current_logical_phase,
                                           allocated_outlifs[current_stack][current_logical_phase]));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_init(
    int unit)
{
    uint8 is_std_1 = TRUE;
    int is_rif = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_init_phase_mapping(unit));
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_mdb_clusters_init(unit));
        SHR_IF_ERR_EXIT(dnx_lif_lib_init_rif(unit));

        SHR_IF_ERR_EXIT(dnx_pp_prgm_default_image_check(unit, &is_std_1));
        if (is_std_1)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_init(unit));
        }

        if (dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_eedb_bank_traffic_lock))
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_traffic_lock_bug_workaround(unit));
        }

        /**If the outlif pointer size is 20 - use eedb banks only for ll*/
        if ((dnx_data_lif.out_lif.outlif_profile_width_get(unit)) == 0)
        {
            dnx_algo_local_outlif_eedb_data_banks_disable_set(unit, TRUE);
            SHR_IF_ERR_EXIT(dnx_algo_local_outlif_eedb_ovf_set(unit, 0, 1));
        }
    }

    /** checking if global lif 0 is RIF, in most of the cases it should be RIF, there are some cases due to HW bugs 
      * that the RIF table is not valid, in those cases the RIF table should not be valid and all entries should be added  
      * to the inlif table. */
    dnx_lif_lib_global_lif_type_get(unit, 0, &is_rif);
    if (!is_rif)
    {
        /** updating the min value of the LIF table to be 0, all global lif should be added to this table */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, DBAL_TABLE_GLOBAL_LIF_EM, DBAL_FIELD_GLOB_OUT_LIF, TRUE, 0, 0,
                         DBAL_PREDEF_VAL_MIN_VALUE, 0));

        /*
         * updating the Max value of the rif table to be 0, no global lif should be fit to this table 
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_update
                        (unit, DBAL_TABLE_GLOBAL_RIF_EM, DBAL_FIELD_GLOB_OUT_RIF, TRUE, 0, 0,
                         DBAL_PREDEF_VAL_MAX_VALUE, 0));

    }

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

    if ((outlif_info && _SHR_IS_FLAG_SET(outlif_hw_info.flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_UPDATE_HW))
        && !_SHR_IS_FLAG_SET(outlif_info->local_lif_flags, DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_DONT_UPDATE_HW))
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
    uint32 local_lif,
    uint8 is_replace)
{
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

    if (!is_replace || local_lif != LIF_LIB_GLEM_KEEP_OLD_VALUE)
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, local_lif);
    }
    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                        egress_global_lif.is_allocated(unit, global_lif, &is_allocated));
    }
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "global_lif is is not allocated, cannot write to GLEM.");
    }

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
    int is_rif = 0;
    uint8 entry_changed = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_add_to_glem_verify(unit, core_id, global_lif, local_lif, is_replace));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    /*
     * Iterate per core, or just on one core.
     */
    DNXCMN_CORES_ITER(unit, core_id, core_index)
    {
        if (is_rif)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));
            /*
             * Set dbal key : Global Rif + core.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF,
                                       LIF_LIB_GLEM_RIF_KEY_GET(global_lif));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);
            /*
             * Set dbal result: The lif profile.
             */
            if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                entry_changed = TRUE;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                             LIF_LIB_GLEM_RIF_PROFILE_INSTANCE_GET(global_lif), outlif_profile);

            }
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
            /*
             * Set dbal key : Global lif + core.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);
            /*
             * Set dbal result: The local lif + lif profile.
             */
            if ((!is_replace) || (local_lif != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                entry_changed = TRUE;
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif);
            }
            if ((!is_replace) || (outlif_profile != LIF_LIB_GLEM_KEEP_OLD_VALUE))
            {
                if (dnx_data_lif.out_lif.outlif_profile_width_get(unit))
                {
                    entry_changed = TRUE;
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                                 outlif_profile);
                }
            }
        }

        if (entry_changed == FALSE)
        {
            SHR_EXIT();
        }
        if ((dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit)) && (is_rif))
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
    uint8 is_allocated;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(outlif_profile, _SHR_E_PARAM, "outlif_profile");
    DNXCMN_CORE_VALIDATE(unit, core_id, FALSE);
    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);
    if (global_lif >= dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_NULL_CHECK(local_lif, _SHR_E_PARAM, "local_lif");
    }
    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
    }
    else
    {
        SHR_IF_ERR_EXIT(lif_mngr_db.global_lif_allocation.
                        egress_global_lif.is_allocated(unit, global_lif, &is_allocated));
    }
    if (!is_allocated)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "global_lif is is not allocated, cannot read from GLEM.");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_lif_lib_read_from_glem(
    int unit,
    int core_id,
    uint32 global_lif,
    uint32 *local_lif_p,
    uint32 *outlif_profile_p)
{
    uint32 entry_handle_id;
    int is_rif;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_read_from_glem_verify(unit, core_id, global_lif, local_lif_p, outlif_profile_p));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    /*
     * Set dbal result type according to lif range.
     */
    if (is_rif)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));
        /*
         * Set dbal key : Global lif + core.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF,
                                   LIF_LIB_GLEM_RIF_KEY_GET(global_lif));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE,
                                   LIF_LIB_GLEM_RIF_PROFILE_INSTANCE_GET(global_lif), outlif_profile_p);

    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
        /*
         * Set dbal key : Global lif + core.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, INST_SINGLE, local_lif_p);
        if (dnx_data_lif.out_lif.outlif_profile_width_get(unit))
        {
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ERPP_OUT_LIF_PROFILE, INST_SINGLE,
                                       outlif_profile_p);
        }
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
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);

    LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, global_lif);

    if (global_lif < dnx_data_l3.rif.nof_rifs_get(unit))
    {
        SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, global_lif, &is_allocated));
        if (is_allocated)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "global_rif is allocated, cannot delete. First free rif allocation");
        }
    }

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
    uint8 rif_exists = FALSE;
    int is_rif = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_lib_remove_from_glem_verify(unit, core_id, global_lif));

    SHR_IF_ERR_EXIT(dnx_lif_lib_global_lif_type_get(unit, global_lif, &is_rif));

    /*
     * In case of RIF and glem_rif_optimization_enabled, delete only if no other RIF exists on this entry
     */
    if (is_rif && dnx_data_lif.global_lif.glem_rif_optimization_enabled_get(unit))
    {
        uint32 base_id = LIF_LIB_GLEM_RIF_KEY_GET(global_lif);
        /** If base id is 0, we start looking from offset 1 because rif 0 id is invalid */
        int offset = base_id ? 0 : 1;
        is_rif = 1;
        while ((!rif_exists) && (offset < 4))
        {
            SHR_IF_ERR_EXIT(algo_l3_db.rif_alloc.rif_res_manager.is_allocated(unit, base_id + offset, &rif_exists));
            offset++;
        }
    }

    if (!is_rif)
    {
        DNXCMN_CORES_ITER(unit, core_id, core_index)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_LIF_EM, &entry_handle_id));
            /*
             * Set dbal key : Global lif + core.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, global_lif);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_index);

            rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
    if (is_rif && !rif_exists)
    {
        /*
         * Iterate per core, or just on one core.
         */
        DNXCMN_CORES_ITER(unit, core_id, core_index)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_RIF_EM, &entry_handle_id));

            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_RIF,
                                       LIF_LIB_GLEM_RIF_KEY_GET(global_lif));
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
