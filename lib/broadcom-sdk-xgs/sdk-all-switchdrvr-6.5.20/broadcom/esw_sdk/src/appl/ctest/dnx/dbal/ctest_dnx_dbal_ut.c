/** \file diag_dnx_dbal_ut.c
 *
 * Main diagnostics for dbal applications All CLI commands, that are related to DBAL, are gathered in this file.
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 *************/

/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif

/** allow memregs.h include excplictly */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include <soc/drv.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/mcm/memregs.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_qos_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/lif_mngr_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/ecmp_access.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l2.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_l2.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include <include/bcm_int/dnx/field/field_entry.h>
#include <sal/core/boot.h>

#include "src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h"

#include "ctest_dnx_dbal.h"
/*************
 * TYPEDEFS  *
 *************/

extern shr_error_e dbal_db_init_dynamic_result_type_clear(
    int unit,
    multi_res_info_t * multi_res_info);

extern cmd_result_t diag_dbal_pre_action_logger_close(
    int unit);

extern cmd_result_t diag_dbal_post_action_logger_restore(
    int unit);

extern cmd_result_t diag_dbal_pre_action_mdb_logger_close(
    int unit);

extern cmd_result_t diag_dbal_post_action_mdb_logger_restore(
    int unit);

extern shr_error_e dbal_db_init_field_hw_entity_validate(
    int unit,
    int table_id,
    dbal_hl_l2p_field_info_t * access,
    uint8 is_reg,
    uint8 print_error);

/*************
 * FUNCTIONS *
 *************/
#define DBAL_SW_STATE_ACCESS dbal_db

/*************
* FUNCTIONS *
*************/

/**
 * \brief Define the number of field updates made before commit
 * in UPDATE_FIELD test case
 */
#define MAX_NUM_UPDATE_BEFORE_COMMIT 5

#define CTEST_DBAL_GROUP_HASH_MEMORY    1
#define CTEST_DBAL_GROUP_HASH_REGISTER  2

/**
 * fields values for random values test
 */
uint32 key_field_val[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 key_field_mask[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 field_val[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 field_mask[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * fields get values for random values test
 */
uint32 get_field_val[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
uint32 get_field_mask[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/************* DBAL TESTS FUNCTIONS *************/

/**
 *  \brief
 *  adding element to the hash table according properties for group mapping validations
 */
static shr_error_e
ctest_dbal_element_hash_add(
    int unit,
    int is_group,
    int is_reg,
    uint32 hw_entity_id)
{
    hw_ent_groups_map_hash_key hash_key;
    uint32 data_index = 0;
    uint8 rt = 0;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hash_key, 0, sizeof(hash_key));

    utilex_U32_to_U8(&hw_entity_id, 4, (uint8 *) &hash_key);
    if (is_reg)
    {
        ((uint8 *) &hash_key)[4] = CTEST_DBAL_GROUP_HASH_REGISTER;
    }
    else
    {
        ((uint8 *) &hash_key)[4] = CTEST_DBAL_GROUP_HASH_MEMORY;
    }

    if (is_group)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.insert(unit, &hash_key, &data_index, &rt));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.insert(unit,
                                                                               (hw_ent_direct_map_hash_key *) &
                                                                               hash_key, &data_index, &rt));
    }

    if (!rt)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Adding element to validation fail is_group = %d, is_reg %d\n", is_group, is_reg);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  compare the group hash table and the direct hash table - checks that there is no element in the group hash table
 *  that is in the direct hash table.
 */
static shr_error_e
ctest_dbal_hw_elements_compare(
    int unit)
{
    uint32 hash_entry_idx = 0;
    hw_ent_direct_map_hash_key direct_hash_key;
    uint32 data_indx;
    uint8 found_in_groups = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&direct_hash_key, 0, sizeof(direct_hash_key));

    while (!SW_STATE_HASH_TABLE_ITER_IS_END(&hash_entry_idx))
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.get_next
                        (unit, &hash_entry_idx, &direct_hash_key, &data_indx));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.find
                        (unit, (hw_ent_groups_map_hash_key *) & direct_hash_key, &data_indx, &found_in_groups));
        if (found_in_groups)
        {
            /** use array instead of singleton because of coverity ARRAY_VS_SINGLETON */
            uint32 hw_ent_id[1] = { 0 };
            SHR_IF_ERR_EXIT(utilex_U8_to_U32((uint8 *) &direct_hash_key, 4, hw_ent_id));

            LOG_CLI((BSL_META("\n ERROR group mapping!\n")));
            if (((uint8 *) &direct_hash_key)[4] == CTEST_DBAL_GROUP_HASH_MEMORY)
            {
                LOG_CLI((BSL_META("Memory (%s) mapped directly and via group, "), SOC_MEM_NAME(unit, hw_ent_id[0])));
            }
            else
            {
                LOG_CLI((BSL_META("register (%s) mapped directly and via group, "), SOC_REG_NAME(unit, hw_ent_id[0])));
            }

            LOG_CLI((BSL_META("to find the related table use dbal access hl HWElement=<mem/reg>\n")));

            SHR_ERR_EXIT(_SHR_E_PARAM, "Mapping Error\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief
 *  perofrm init to the hash table that keeps all the memories and registers
 */
static shr_error_e
dnx_dbal_group_definition_hash_init(
    int unit)
{
    sw_state_htbl_init_info_t hash_tbl_init_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /** hash table init */
    sal_memset(&hash_tbl_init_info, 0, sizeof(hash_tbl_init_info));

    hash_tbl_init_info.max_nof_elements = dnx_data_dbal.hw_ent.nof_direct_maps_get(unit);
    hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;
    SHR_SET_CURRENT_ERR(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.create(unit, &hash_tbl_init_info));
    if (SHR_FUNC_VAL_IS(_SHR_E_EXISTS))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.clear(unit));
    }
    else if (SHR_FUNC_ERR())
    {
        SHR_EXIT();
    }

    hash_tbl_init_info.max_nof_elements = dnx_data_dbal.hw_ent.nof_groups_maps_get(unit);
    hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;
    SHR_SET_CURRENT_ERR(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.create(unit, &hash_tbl_init_info));
    if (SHR_FUNC_VAL_IS(_SHR_E_EXISTS))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NONE);
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.clear(unit));
    }
    else if (SHR_FUNC_ERR())
    {
        SHR_EXIT();
    }

    dnxc_sw_state_alloc_during_test_set(unit, 0);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_group_definition_validate(
    int unit,
    dbal_tables_e table_id)
{
    CONST dbal_logical_table_t *table;
    dbal_tables_e table_iter = DBAL_TABLE_EMPTY;
    int curr_res_type;
    dbal_hard_logic_access_types_e access_type;
    dbal_hl_access_info_t *hl_mapping = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_group_definition_hash_init(unit));

    /** traversing over all dbal tables */
    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_NONE, &table_iter));

    while (table_iter != DBAL_TABLE_EMPTY)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_iter, &table));

        /** Fill group/direct hash tables per logical table */
        for (curr_res_type = 0; curr_res_type < table->nof_result_types; curr_res_type++)
        {
            int hw_entity = 0, is_reg, hl_iter;
            dbal_hl_l2p_info_t *l2p_hl_info = NULL;
            hl_mapping = &table->hl_mapping_multi_res[curr_res_type];

            for (access_type = DBAL_HL_ACCESS_MEMORY; access_type <= DBAL_HL_ACCESS_REGISTER; access_type++)
            {
                if (hl_mapping->l2p_hl_info[access_type].num_of_access_fields > 0)
                {
                    l2p_hl_info = &(table->hl_mapping_multi_res[curr_res_type].l2p_hl_info[access_type]);
                    is_reg = (access_type == DBAL_HL_ACCESS_REGISTER) ? 1 : 0;

                    for (hl_iter = 0; hl_iter < l2p_hl_info->num_of_access_fields; hl_iter++)
                    {
                        int index, is_group = 0;
                        dbal_hl_l2p_field_info_t *l2p_fields_info = &l2p_hl_info->l2p_fields_info[hl_iter];

                        /** handling aliasing of memory/register */
                        hw_entity = is_reg ? l2p_fields_info->alias_reg : l2p_fields_info->alias_memory;
                        if (hw_entity != INVALIDm)
                        {
                            SHR_IF_ERR_EXIT(ctest_dbal_element_hash_add(unit, 0, is_reg, hw_entity));
                        }

                        for (index = 0; index < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; index++)
                        {
                            hw_entity = is_reg ? l2p_fields_info->reg[index] : l2p_fields_info->memory[index];
                            if (hw_entity == INVALIDm)
                            {
                                break;
                            }
                            is_group = (l2p_fields_info->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY) ? 1 : 0;
                            SHR_IF_ERR_EXIT(ctest_dbal_element_hash_add(unit, is_group, is_reg, hw_entity));
                        }
                    }
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_iter, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_HARD_LOGIC, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_NONE, &table_iter));
    }

    SHR_IF_ERR_EXIT(ctest_dbal_hw_elements_compare(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_partial_access(
    int unit,
    dbal_tables_e table_id)
{
    dbal_ut_modes_t mode = DBAL_UT_RESTORE_TABLE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(test_dnx_dbal_partial_access(unit, table_id, mode, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test sets the table key & payload fields &commit the
 * changes. Then it update only one payload field & commit. read
 * the all table and verify all payload fields have the correct
 * value.
 */
cmd_result_t
test_dnx_dbal_partial_access(
    int unit,
    dbal_tables_e table_id,
    dbal_ut_modes_t mode,
    uint8 is_rand)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 seed = 0;
    CONST dbal_logical_table_t *table;
    int field_index;
    uint32 nof_result_fields_to_use = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_val[0][0], 0, sizeof(field_val));
    sal_memset(&get_field_val[0][0], 0, sizeof(get_field_val));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Initialize random seed value */
    if (is_rand)
    {
        seed = sal_time_usecs();
        LOG_CLI((BSL_META("****** Running DBAL UT update field before commit.  table %s, seed=%d, ******\n"),
                 table->table_name, seed));
    }
    else
    {
        seed = 1000 + table_id;
    }
    sal_srand(seed);

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        key_field_val[field_index][0] = table->keys_info[field_index].min_value;
        if (dbal_table_is_in_lif(table))
        {
            key_field_val[field_index][0] = DIAG_DBAL_MIN_IN_LIF_TO_ALLOCATE;
        }
        if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR) &&
            (!dbal_table_is_allcator_not_needed(unit, table_id)))
        {
            int core_val = table->core_mode == DBAL_CORE_MODE_DPC ? 0 : _SHR_CORE_ALL;
            SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, table->keys_info[field_index].field_id,
                                                          table_id, 0, core_val, key_field_val[field_index][0], 1));
        }

        if (table->core_mode == DBAL_CORE_MODE_SBC && table->keys_info[field_index].field_id == DBAL_FIELD_CORE_ID)
        {
            uint32 core_val = table->core_mode == DBAL_CORE_MODE_DPC ? 0 : _SHR_CORE_ALL;
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, DBAL_FIELD_CORE_ID, &core_val);
        }
        else
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                           key_field_val[field_index]);
        }
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        /**
         * skip readonly, writeonly, trigger fields
         */
        if ((table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_READONLY)
            || (table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_WRITEONLY)
            || (table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_TRIGGER))
        {
            continue;
        }

        nof_result_fields_to_use++;

        if (table->multi_res_info[0].results_info[field_index].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            field_val[field_index][0] = 0;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field
                            (unit, table->multi_res_info[0].results_info[field_index].field_id, RANDOM_FIELD_VAL,
                             table->multi_res_info[0].results_info[field_index].min_value,
                             table->multi_res_info[0].results_info[field_index].max_value,
                             table->multi_res_info[0].results_info[field_index].field_nof_bits +
                             table->multi_res_info[0].results_info[field_index].offset_in_logical_field, FALSE,
                             table->multi_res_info[0].results_info[field_index].arr_prefix,
                             table->multi_res_info[0].results_info[field_index].arr_prefix_size,
                             field_val[field_index]));
        }

        dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                         table->multi_res_info[0].results_info[field_index].field_id,
                                         INST_SINGLE, field_val[field_index]);
    }

    if (nof_result_fields_to_use < 2)
    {
        /** in this case the table is not applicable to perform partial access test - dealloc the allocated values*/
        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR) &&
                (!dbal_table_is_allcator_not_needed(unit, table_id)))
            {
                int core_val = table->core_mode == DBAL_CORE_MODE_DPC ? 0 : _SHR_CORE_ALL;
                SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, table->keys_info[field_index].field_id,
                                                              table_id, 0, core_val, key_field_val[field_index][0], 0));
            }
        }
        SHR_EXIT();
    }
    /*
     * Commit table values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * In case of MDB, commit the descriptor DMA chain to HW.
     * If descriptor DMA is disabled, the call immediately returns.
     */
    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchDescCommit, 1));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_wr));

    /**
     * Set key field for new handle
     */
    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (table->core_mode == DBAL_CORE_MODE_SBC && table->keys_info[field_index].field_id == DBAL_FIELD_CORE_ID)
        {
            uint32 core_val = table->core_mode == DBAL_CORE_MODE_DPC ? 0 : _SHR_CORE_ALL;
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, DBAL_FIELD_CORE_ID, &core_val);
        }
        else
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                           key_field_val[field_index]);
        }
    }

    /*
     * Update N-1 fields, we use do-while here to support case of  table with 1 result field
     */
    field_index = 0;
    do
    {
        if ((table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_READONLY)
            || (table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_WRITEONLY)
            || (table->multi_res_info[0].results_info[field_index].permission == DBAL_PERMISSION_TRIGGER))
        {
            field_index++;
            continue;
        }

        if (table->multi_res_info[0].results_info[field_index].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            field_val[field_index][0] = 0;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field(unit,
                                                            table->multi_res_info[0].results_info[field_index].field_id,
                                                            RANDOM_FIELD_VAL,
                                                            table->multi_res_info[0].
                                                            results_info[field_index].min_value,
                                                            table->multi_res_info[0].
                                                            results_info[field_index].max_value,
                                                            table->multi_res_info[0].
                                                            results_info[field_index].field_nof_bits +
                                                            table->multi_res_info[0].
                                                            results_info[field_index].offset_in_logical_field, FALSE,
                                                            table->multi_res_info[0].
                                                            results_info[field_index].arr_prefix,
                                                            table->multi_res_info[0].
                                                            results_info[field_index].arr_prefix_size,
                                                            field_val[field_index]));
        }

        dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                         table->multi_res_info[0].results_info[field_index].field_id,
                                         INST_SINGLE, field_val[field_index]);
        field_index++;
    }
    while (field_index < (table->multi_res_info[0].nof_result_fields - 1));

    /*
     * Commit table values
     */
    if (dbal_tables_is_non_direct(unit, table_id))
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));
    }

    /*
     * In case of MDB, commit the descriptor DMA chain to HW.
     * If descriptor DMA is disabled, the call immediately returns.
     */
    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchDescCommit, 1));
    }

    /*
     * Read table fields
     */
    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        /*
         * restore key fields
         */
        dbal_entry_key_field_arr32_set(unit, entry_handle_rd, table->keys_info[field_index].field_id,
                                       key_field_val[field_index]);
    }

    if ((dbal_table_is_in_lif(table)) || (dbal_table_is_out_lif(table)))
    {
        dbal_entry_value_field32_set(unit, entry_handle_rd, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_GET_ALL_FIELDS));

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_rd,
                                                                table->multi_res_info[0].
                                                                results_info[field_index].field_id, INST_SINGLE,
                                                                get_field_val[field_index]));
    }

    /*
     * compare results
     */
    if (compare_fields_values(unit, table, field_val, get_field_val, 0, 0) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s wrong value\n", dbal_logical_table_to_string(unit, table_id));
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

    /*
     * In case mode is restore we set the entry to 0
     */
    if (mode == DBAL_UT_RESTORE_TABLE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_wr));

        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                           key_field_val[field_index]);
        }
        if (dbal_table_is_in_lif(table) || dbal_table_is_out_lif(table))
        {
            dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        }
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_wr, DBAL_COMMIT));
    }

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (SHR_BITGET(table->keys_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR) &&
            (!dbal_table_is_allcator_not_needed(unit, table_id)))
        {
            int core_val = table->core_mode == DBAL_CORE_MODE_DPC ? 0 : _SHR_CORE_ALL;
            SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, table->keys_info[field_index].field_id,
                                                          table_id, 0, core_val, key_field_val[field_index][0], 0));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_update_field_before_commit(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_update_field_before_commit(unit, table_id, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test sets the table key & payload fields number of times
 * before commit.  read the all table and verify all payload
 * fields have the correct value.
 */
shr_error_e
dnx_dbal_update_field_before_commit(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand)
{
    dbal_ut_modes_t mode = DBAL_UT_RESTORE_TABLE;
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 seed = 0;
    CONST dbal_logical_table_t *table;
    int field_index, update_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_val[0][0], 0, sizeof(field_val));
    sal_memset(&get_field_val[0][0], 0, sizeof(get_field_val));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Initialize random seed value */
    if (is_rand)
    {
        seed = sal_time_usecs();
        LOG_CLI((BSL_META("****** Running DBAL UT update field before commit.  table %s, seed=%d, ******\n"),
                 table->table_name, seed));
    }
    else
    {
        seed = 1000 + table_id;
    }
    sal_srand(seed);

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        for (update_index = 0; update_index < MAX_NUM_UPDATE_BEFORE_COMMIT; update_index++)
        {
            SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field(unit, table->keys_info[field_index].field_id,
                                                            RANDOM_FIELD_VAL,
                                                            table->keys_info[field_index].min_value,
                                                            table->keys_info[field_index].max_value,
                                                            table->keys_info[field_index].field_nof_bits +
                                                            table->keys_info[field_index].offset_in_logical_field,
                                                            TRUE, 0, 0, key_field_val[field_index]));

            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                           key_field_val[field_index]);
        }
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        for (update_index = 0; update_index < MAX_NUM_UPDATE_BEFORE_COMMIT; update_index++)
        {
            SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field
                            (unit, table->multi_res_info[0].results_info[field_index].field_id, RANDOM_FIELD_VAL,
                             table->multi_res_info[0].results_info[field_index].min_value,
                             table->multi_res_info[0].results_info[field_index].max_value,
                             table->multi_res_info[0].results_info[field_index].field_nof_bits +
                             table->multi_res_info[0].results_info[field_index].offset_in_logical_field, FALSE,
                             table->multi_res_info[0].results_info[field_index].arr_prefix,
                             table->multi_res_info[0].results_info[field_index].arr_prefix_size,
                             field_val[field_index]));

            dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                             table->multi_res_info[0].results_info[field_index].field_id, INST_SINGLE,
                                             field_val[field_index]);
        }
    }

    /*
     * Commit table values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * Read table fields
     */
    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        /*
         * restore key fields
         */
        dbal_entry_key_field_arr32_set(unit, entry_handle_rd, table->keys_info[field_index].field_id,
                                       key_field_val[field_index]);
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        /*
         * restore value fields
         */
        dbal_value_field_arr32_request(unit, entry_handle_rd,
                                       table->multi_res_info[0].results_info[field_index].field_id,
                                       INST_SINGLE, get_field_val[field_index]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * compare results
     */
    if (compare_fields_values(unit, table, field_val, get_field_val, 0, 0) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s wrong value\n", dbal_logical_table_to_string(unit, table_id));
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

    /*
     * In case mode is restore we set the entry to 0
     */
    if (mode == DBAL_UT_RESTORE_TABLE)
    {
        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                           key_field_val[field_index]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_wr, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test validate field_unset API and DBAL_HANDLE_COPY macro.
 * it sets the table key & payload fields then unset one field.  read the all entry
 * and verify all payload fields have the correct value.
 */
shr_error_e
dnx_dbal_ut_handle_operations(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 copied_entry_handle = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_SCH_SE_CL_CONFIG)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    sal_memset(&field_val[0][0], 0, sizeof(field_val));
    sal_memset(&get_field_val[0][0], 0, sizeof(get_field_val));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CL_PROFILE, 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    field_index = 0;
    field_val[field_index][0] = 1;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CL_MODE, INST_SINGLE, field_val[field_index]);
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 0, field_val[field_index]);
    field_val[field_index][0] = 0; /** set to 0 for final compare as we are going to unset the array*/
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 1, field_val[field_index]);
    field_val[field_index][0] = 0; /** set to 0 for final compare as we are going to unset the array*/
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 2, field_val[field_index]);
    field_val[field_index][0] = 0; /** set to 0 for final compare as we are going to unset the array*/
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 3, field_val[field_index]);
    field_val[field_index][0] = 0; /** set to 0 for final compare as we are going to unset the array*/
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_WFQ_WEIGHT_MODE, INST_SINGLE,
                                     field_val[field_index]);
    field_index++;
    field_val[field_index][0] = 2;
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_CL_ENHANCED_MODE, INST_SINGLE,
                                     field_val[field_index]);

    /*
     * unset multi instance field
     */
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, INST_ALL);
    /*
     * unset single field
     */
    dbal_entry_value_field_unset(unit, entry_handle_id, DBAL_FIELD_CL_ENHANCED_MODE, INST_SINGLE);
    field_val[field_index][0] = 0;

    /*
     * check handle copy API
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_COPY(unit, entry_handle_id, &copied_entry_handle));

    /*
     * Commit table values using the copied handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, copied_entry_handle, DBAL_COMMIT));

    /*
     * restore value fields
     */
    field_index = 0;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CL_MODE, INST_SINGLE, get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 0,
                                   get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 1,
                                   get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 2,
                                   get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY_INV_WEIGHT, 3,
                                   get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_WFQ_WEIGHT_MODE, INST_SINGLE,
                                   get_field_val[field_index]);
    field_index++;
    dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_CL_ENHANCED_MODE, INST_SINGLE,
                                   get_field_val[field_index]);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * compare results
     */
    if (compare_fields_values(unit, table, field_val, get_field_val, 0, 0) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s wrong value\n", dbal_logical_table_to_string(unit, table_id));
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test verify the sub fields functionality.
 * it access table with DESTINATION field and access it in
 * different ways using different sub fields.
 */
shr_error_e
dnx_dbal_ut_parent_field_mapping(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    uint32 get_destination_val;
    uint32 fec_val = 0x33;
    uint32 get_fec_val, get_fec_val_error;
    uint32 mc_id_val = 0x44;
    uint32 get_mc_id_val;
    uint32 port_id_val = 0x66;
    uint32 get_port_id_val;
    uint32 sub_field_val;
    dbal_fields_e sub_field_id;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with EXAMPLE_SW_SUB_FIELD table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_SW_SUB_FIELD)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_key_fields < 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with table %s - less than 2 key fields\n",
                     dbal_logical_table_to_string(unit, table_id));
    }
    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_wr, table->keys_info[0].field_id, 2);
    dbal_entry_key_field32_set(unit, entry_handle_wr, table->keys_info[1].field_id, 15);

    dbal_entry_key_field32_set(unit, entry_handle_rd, table->keys_info[0].field_id, 2);
    dbal_entry_key_field32_set(unit, entry_handle_rd, table->keys_info[1].field_id, 15);

    /*
     * Set fec Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_FEC, INST_SINGLE, fec_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * Get fec Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_FEC, INST_SINGLE, &get_fec_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Set MC_ID Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_MC_ID, INST_SINGLE, mc_id_val);

    diag_dbal_pre_action_logger_close(unit);

    /** re-adding the entry without commit update... should fail  */
    rv = dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT_UPDATE));

    /*
     * Get fec Payload, This should fail as last set field id is DBAL_FIELD_MC_ID
     */
    diag_dbal_pre_action_logger_close(unit);
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_FEC, INST_SINGLE, &get_fec_val_error);
    rv = dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);

    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

    /*
     * Get MC_ID Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_MC_ID, INST_SINGLE, &get_mc_id_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Check the MC_ID val is as expected
     */
    if (get_mc_id_val != mc_id_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_mc_id_val[%d] != mc_id_val[%d],\n", get_mc_id_val,
                     mc_id_val);
    }

    /*
     * Check multiple set to field and correct behavior
     * Set fec Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_FEC, INST_SINGLE, fec_val);

    /*
     * Set MC_ID Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_MC_ID, INST_SINGLE, mc_id_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT_UPDATE));

    /*
     * Set PORT_ID Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_PORT_ID, INST_SINGLE, port_id_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT_UPDATE));

    /*
     * Get PORT_ID Payload - we set the field multiple times and check that we get the latest value when we read
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_PORT_ID, INST_SINGLE, &get_port_id_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Get DESTINATION Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_DESTINATION, INST_SINGLE, &get_destination_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Check the FEC val is as expected
     */
    if (get_fec_val != fec_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_fec_val[%d] != fec_val[%d]\n", get_fec_val, fec_val);
    }

    /*
     * Check the PORT_ID val is as expected
     */
    if (get_port_id_val != port_id_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_port_id_val[%d] != port_id_val[%d]\n",
                     get_port_id_val, port_id_val);
    }

    /*
     * Check the DESTINATION field val not equal to PORT_ID val
     */
    if (get_destination_val == port_id_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_destination_val[%d] == port_id_val[%d],\n",
                     get_destination_val, port_id_val);
    }

    /*
     * Encode Destination field
     */
    sub_field_val = 0;
    SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit, DBAL_FIELD_DESTINATION, &get_destination_val,
                                                    &sub_field_id, &sub_field_val));

    /*
     * Check the ENCODED DESTINATION field val is as expected
     */
    if (sub_field_val != port_id_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: encoded port_from_destination[%d] != port_id_val[%d],\n",
                     sub_field_val, port_id_val);
    }

    /*
     * Check the ENCODED DESTINATION field ID is as expected
     */
    if (sub_field_id != DBAL_FIELD_PORT_ID)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: encoded destination field id[%d] != DBAL_FIELD_PORT_ID,\n",
                     sub_field_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_wr, DBAL_COMMIT));
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test write a multiple instances field and read & compare
 * the results. then write & update a multiple instances field
 * and read & compare the results.
 */
shr_error_e
dnx_dbal_ut_multiple_instance_field(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    uint32 counter_val = 0x0;
    uint8 instance_index;
    uint32 field_val[DBAL_NUM_OF_FIELD_INSTANCES][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 get_field_val[DBAL_NUM_OF_FIELD_INSTANCES][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    int nof_instances;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&field_val[0][0], 0, sizeof(field_val));
    sal_memset(&get_field_val[0][0], 0, sizeof(get_field_val));

    /*
     * Verify table ID, This test is a special test that can run only
     * run with DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    nof_instances = table->multi_res_info[0].results_info[0].nof_instances;

    /*
     * Set counter instance Field
     */
    for (instance_index = 0; instance_index < nof_instances; instance_index++, counter_val++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, instance_index, counter_val);
        field_val[instance_index][0] = counter_val;
    }

    /*
     * Commit table values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Read table fields
     */
    for (instance_index = 0; instance_index < nof_instances; instance_index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, instance_index,
                                   get_field_val[instance_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * compare results
     */
    if (compare_fields_values(unit, table, field_val, get_field_val, 0, 0) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s wrong value\n", dbal_logical_table_to_string(unit, table_id));
    }

    /*
     * Set counter instance Field
     */
    counter_val = 0;
    for (instance_index = 0; instance_index < nof_instances; instance_index++, counter_val++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, instance_index, counter_val);
        field_val[instance_index][0] = counter_val;
    }

    counter_val = 1;
    for (instance_index = 0; instance_index < nof_instances; instance_index++, counter_val++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, instance_index, counter_val);
        field_val[instance_index][0] = counter_val;
    }

    /*
     * Commit table values
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Read table fields
     */
    for (instance_index = 0; instance_index < nof_instances; instance_index++)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, instance_index,
                                   get_field_val[instance_index]);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * compare results
     */
    if (compare_fields_values(unit, table, field_val, get_field_val, 0, 0) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s wrong value\n", dbal_logical_table_to_string(unit, table_id));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test runs ltt and ltt_full tests, with
 * no validation on commit/field set
 */
shr_error_e
dnx_dbal_ut_table_no_validations(
    int unit,
    dbal_tables_e table_id)
{
    dnx_dbal_ltt_input_t ltt_input;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ltt_input.dbal_table = table_id;
    ltt_input.access_method = DBAL_NOF_ACCESS_METHODS;
    ltt_input.table_type = DBAL_NOF_TABLE_TYPES;
    ltt_input.flags = 0;
    ltt_input.nof_entries = DIAG_DBAL_DEFAULT_NOF_ENTRIES;

    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_NO_VALIDATIONS, 1));

    /*
     * Run ltt test
     */
    SHR_IF_ERR_EXIT(ctest_dbal_table_test_run(unit, table_id, &ltt_input));

    /*
     * Set ltt full test parameters
     */
    ltt_input.flags = LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST | LTT_FLAGS_MEASUREMENTS;
    /*
     * Run ltt full test
     */
    SHR_IF_ERR_EXIT(ctest_dbal_table_test_run(unit, table_id, &ltt_input));

    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_NO_VALIDATIONS, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test verify the result type table functionality.
 * it access table with multiple result types defined and access
 * it in different ways using different result type values.
 */
shr_error_e
dnx_dbal_ut_rt_access(
    int unit,
    dbal_tables_e table_id)
{
    uint32 field_val_int[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;

    uint32 ret_gport_val, ret_fec_val, ret_peer_gport_val;
    uint32 result_type;
    uint32 gport_val = 0x11;
    uint32 fec_val = 0x22;
    uint32 peer_gport_val = 0x33;
    uint32 vlan_domain_val = 0x44;
    uint32 esem_access_val = 0x5;
    uint32 tx_outer_tag_valid_val = 0x1;
    uint32 tx_outer_tag_vid_val = 0x77;
    int rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_SW_RESULT_TYPE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    /*
     * Get table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    /*
     * Set RT1 result type fields
     */
    result_type = DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT1;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, gport_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, fec_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PEER_GPORT, INST_SINGLE, peer_gport_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            field_val_int));
    if (field_val_int[0] != DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_RESULT_TYPE), field_val_int[0],
                     DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, field_val_int));
    if (field_val_int[0] != gport_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_GPORT_FOR_LEARNING), field_val_int[0], gport_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, field_val_int));
    if (field_val_int[0] != fec_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_FEC), field_val_int[0], fec_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEER_GPORT, INST_SINGLE,
                                                            field_val_int));
    if (field_val_int[0] != peer_gport_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_PEER_GPORT), field_val_int[0], peer_gport_val);
    }

     /** negative test, checking field that is not exists */
    diag_dbal_pre_action_logger_close(unit);

    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE,
                                                 field_val_int);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "negative test passed. field %s should not be found in entry,\n",
                     dbal_field_to_string(unit, DBAL_FIELD_ESEM_ACCESS_CMD));
    }

    /** Known result type validations - getting the entry when the result type is known */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, &ret_gport_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FEC, INST_SINGLE, &ret_fec_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PEER_GPORT, INST_SINGLE, &ret_peer_gport_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (ret_peer_gport_val != peer_gport_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Known res type, Comparison error: field %s, ret_val %d != expected_val %d,\n",
                     dbal_field_to_string(unit, DBAL_FIELD_PEER_GPORT), ret_peer_gport_val, peer_gport_val);
    }

    if (ret_fec_val != fec_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Known res type, Comparison error: field %s, ret_val %d != expected_val %d,\n",
                     dbal_field_to_string(unit, DBAL_FIELD_FEC), ret_fec_val, fec_val);
    }

    if (ret_peer_gport_val != peer_gport_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Known res type, Comparison error: field %s, ret_val %d != expected_val %d",
                     dbal_field_to_string(unit, DBAL_FIELD_PEER_GPORT), ret_peer_gport_val, peer_gport_val);
    }

    /*
     * table handle result type cannot be changed
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    result_type = DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT1;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set RT2 result type fields
     */

    /*
     * Try to change table handle result type to RT2 - this should fail
     */
    diag_dbal_pre_action_logger_close(unit);
    result_type = DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT2;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);

    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Negative test passed with no ERROR - change result type in same handle succedded\n");
    }

    /*
     * get a new handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, vlan_domain_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, esem_access_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VALID, INST_SINGLE,
                                 tx_outer_tag_valid_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VID, INST_SINGLE, tx_outer_tag_vid_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            field_val_int));
    if (field_val_int[0] != DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_RESULT_TYPE), field_val_int[0],
                     DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT2);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE,
                                                            field_val_int));
    if (field_val_int[0] != vlan_domain_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_VLAN_DOMAIN), field_val_int[0], vlan_domain_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_ESEM_ACCESS_CMD, INST_SINGLE, field_val_int));
    if (field_val_int[0] != esem_access_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_ESEM_ACCESS_CMD), field_val_int[0], esem_access_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VALID, INST_SINGLE, field_val_int));
    if (field_val_int[0] != tx_outer_tag_valid_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_TX_OUTER_TAG_VALID), field_val_int[0],
                     tx_outer_tag_valid_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_TX_OUTER_TAG_VID, INST_SINGLE, field_val_int));
    if (field_val_int[0] != tx_outer_tag_vid_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: field_id[%s], get_field_val[%d] != expected_val[%d],\n",
                     dbal_field_to_string(unit, DBAL_FIELD_TX_OUTER_TAG_VID), field_val_int[0], tx_outer_tag_vid_val);
    }

    /** negative test, checking field that is not exists  */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PEER_GPORT, INST_SINGLE,
                                                 field_val_int);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "negative test passed. field %s should not be found in entry,\n",
                     dbal_field_to_string(unit, DBAL_FIELD_ESEM_ACCESS_CMD));
    }

    /*
     * Negative test - try to write to other result type field
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 15);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);

    /*
     * dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
     * DBAL_RESULT_TYPE_EXAMPLE_SW_RESULT_TYPE_RT2);
     */
    diag_dbal_pre_action_logger_close(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, gport_val);
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Negative test passed with no ERROR - access wrong field for current result type\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This is a negative test. This test set the key fields of the
 * table correctly. * For payload fields it sets the fields with
 * MAX values in length of field_nof_bits+2. then commit the
 * changes, this test should FAIL.
 */

shr_error_e
dnx_dbal_ut_wrong_field_size(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_wrong_field_size(unit, table_id, FALSE));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_wrong_field_size(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index, rv;
    uint32 seed = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    /** Initialize random seed value */
    if (is_rand)
    {
        seed = sal_time_usecs();
        LOG_CLI((BSL_META("****** Running DBAL UT update field before commit.  table %s, seed=%d, ******\n"),
                 table->table_name, seed));
    }
    else
    {
        seed = 1000 + table_id;
    }
    sal_srand(seed);

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        DIAG_DBAL_RANDOM_FIELD_VAL(key_field_val[field_index], table->keys_info[field_index].field_nof_bits, TRUE);
        dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                       key_field_val[field_index]);
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        DIAG_DBAL_MAX_FIELD_VAL(field_val[field_index],
                                (table->multi_res_info[0].results_info[field_index].field_nof_bits + 2));

        diag_dbal_pre_action_logger_close(unit);
        dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                         table->multi_res_info[0].results_info[field_index].field_id,
                                         INST_SINGLE, field_val[field_index]);

        diag_dbal_post_action_logger_restore(unit);
    }

    /*
     * Commit table values, This operation should fail as this is a negative test
     */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_wrong_field_access(
    int unit,
    dbal_tables_e table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_dbal_wrong_field_access(unit, table_id, FALSE));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * This is a negative test. This test set the key fields of the
 * table correctly. For payload fields it sets the fields with
 * MAX values in length of  field_nof_bits+2. then commit the
 * changes, this test should FAIL.
 * *******************************************************
 */
shr_error_e
dnx_dbal_wrong_field_access(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index, rv;
    uint32 seed = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    /** Initialize random seed value */
    if (is_rand)
    {
        seed = sal_time_usecs();
        LOG_CLI((BSL_META("****** Running DBAL UT wrong field access.  table %s, seed=%d, ******\n"),
                 table->table_name, seed));
    }
    else
    {
        seed = 1000 + table_id;
    }
    sal_srand(seed);

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        DIAG_DBAL_RANDOM_FIELD_VAL(key_field_val[field_index], table->keys_info[field_index].field_nof_bits, TRUE);
        dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                       key_field_val[field_index]);
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        field_val[field_index][0] = 0;
        dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                         table->multi_res_info[0].results_info[field_index].field_id, INST_SINGLE,
                                         field_val[field_index]);
    }

    diag_dbal_pre_action_logger_close(unit);
    field_val[field_index][0] = DBAL_CORE_ALL;
    dbal_entry_value_field_arr32_set(unit, entry_handle_wr, DBAL_FIELD_CORE_ID, INST_SINGLE, field_val[field_index]);

    /*
     * Commit table values, This operation should fail as this is a negative test
     */
    rv = dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_superset_res_type(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    int ii, cur_res_type, rv;
    uint32 ret_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    dbal_fields_e next_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    field_val[0] = 1;

    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "test only supported for mul res type tables table %s has only one \n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, table_id, DBAL_FIELD_EMPTY, 1, 0, &next_field_id));
    while (next_field_id != DBAL_FIELD_EMPTY)
    {
        if ((next_field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, next_field_id, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, next_field_id, 1);
        }

        SHR_IF_ERR_EXIT(dbal_tables_next_field_get(unit, table_id, next_field_id, 1, 0, &next_field_id));
    }

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                 table->multi_res_info[table->nof_result_types].
                                                 results_info[1].field_id, INST_SINGLE, ret_field_val);

    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get for unexisting field returns pass\n");
    }

    /** set all fields from all result types (without the result type itself  */
    for (cur_res_type = 0; cur_res_type < table->nof_result_types; cur_res_type++)
    {
        for (ii = 1; ii < table->multi_res_info[cur_res_type].nof_result_fields; ii++)
        {
            dbal_entry_value_field_arr32_set(unit, entry_handle_id,
                                             table->multi_res_info[cur_res_type].results_info[ii].field_id, INST_SINGLE,
                                             field_val);
        }
    }

    /** Negative test, cannot perform action on entry that the result type is superset   */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

    /** read all result fields from handle */
    for (cur_res_type = 0; cur_res_type < table->nof_result_types; cur_res_type++)
    {
        for (ii = 1; ii < table->multi_res_info[cur_res_type].nof_result_fields; ii++)
        {
            dbal_fields_e field_id = table->multi_res_info[cur_res_type].results_info[ii].field_id;
            rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, ret_field_val);
            if (rv)
            {
                SHR_ERR_EXIT(rv, "issue with field %s \n", dbal_field_to_string(unit, field_id));
            }
            if (ret_field_val[0] != 1)
            {
                dbal_fields_e sub_field_id;
                uint32 sub_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

                SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode_no_err(unit,
                                                                       field_id, ret_field_val, &sub_field_id,
                                                                       sub_field_val));
                if (sub_field_id == DBAL_FIELD_EMPTY)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected value for field %s value = %d\n",
                                 dbal_field_to_string(unit, field_id), ret_field_val[0]);
                }
                else if (sub_field_val[0] != 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected value for field %s[%s] value = %d\n",
                                 dbal_field_to_string(unit, sub_field_id), dbal_field_to_string(unit, field_id),
                                 sub_field_val[0]);
                }
            }
        }
    }

    /** read all Key fields from handle */
    for (ii = 1; ii < table->nof_key_fields; ii++)
    {
        uint32 expected_val;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, table->keys_info[ii].field_id, ret_field_val));
        if ((table->keys_info[ii].field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            expected_val = 0;
        }
        else
        {
            expected_val = 1;
        }
        if (ret_field_val[0] != expected_val)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "value unexpected field = %s value = %d\n",
                         dbal_field_to_string(unit, table->multi_res_info[cur_res_type].results_info[ii].field_id),
                         ret_field_val[0]);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test verify the enum mapping fields functionality.
 * it access table with ENUM_TEST field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e
dnx_dbal_ut_enum_mapping(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int rv;

    uint32 destination_val = 0x22;
    uint32 get_destination_val;
    uint32 gport_val = 0x33;
    uint32 get_gport_val;
    uint32 enum_test_val = DBAL_ENUM_FVAL_ENUM_TEST_DROP;
    uint32 get_enum_test_val;
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with DBAL_TABLE_EXAMPLE_SW_DIRECT table
     */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));

    /*
     * Set Key Fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_wr, table->keys_info[0].field_id, 15);
    dbal_entry_key_field32_set(unit, entry_handle_rd, table->keys_info[0].field_id, 15);

    dbal_entry_key_field32_set(unit, entry_handle_wr, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);
    dbal_entry_key_field32_set(unit, entry_handle_rd, DBAL_FIELD_CORE_ID, DBAL_CORE_DEFAULT);

    /*
     * Set gport Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, gport_val);

    /*
     * Set destination Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_DESTINATION, INST_SINGLE, destination_val);

    /*
     * Set enum_test Payload
     */
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_ENUM_TEST, INST_SINGLE, enum_test_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * Get gport Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_GPORT_FOR_LEARNING, INST_SINGLE, &get_gport_val);
    /*
     * Get destination Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_DESTINATION, INST_SINGLE, &get_destination_val);
    /*
     * Get enum_test Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_ENUM_TEST, INST_SINGLE, &get_enum_test_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Check the GPORT_FOR_LEARNING val is as expected
     */
    if (get_gport_val != gport_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_gport_val[%d] != gport_val[%d],\n",
                     get_gport_val, gport_val);
    }

    /*
     * Check the DESTINATION val is as expected
     */
    if (get_destination_val != destination_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_destination_val[%d] != destination_val[%d],\n",
                     get_destination_val, destination_val);
    }

    /*
     * Check the ENUM_TEST val is as expected
     */
    if (get_enum_test_val != enum_test_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_enum_test_val[%d] != enum_test_val[%d],\n",
                     get_enum_test_val, enum_test_val);
    }

    /*
     * Set enum_test Payload
     */
    enum_test_val = DBAL_ENUM_FVAL_ENUM_TEST_SNOOP;
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_ENUM_TEST, INST_SINGLE, enum_test_val);

    enum_test_val = DBAL_ENUM_FVAL_ENUM_TEST_REDIRECT;
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_ENUM_TEST, INST_SINGLE, enum_test_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * Get enum_test Payload
     */
    dbal_value_field32_request(unit, entry_handle_rd, DBAL_FIELD_ENUM_TEST, INST_SINGLE, &get_enum_test_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Check the ENUM_TEST val is as expected
     */
    if (get_enum_test_val != enum_test_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error: get_enum_test_val[%d] != enum_test_val[%d]\n",
                     get_enum_test_val, enum_test_val);
    }

    /*
     * Set enum_test Payload with wrong value
     */
    diag_dbal_pre_action_logger_close(unit);
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_ENUM_TEST, &field_info));
    dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_ENUM_TEST, INST_SINGLE, field_info->nof_enum_values);
    rv = dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test verify the enum mapping fields functionality in different devices.
 * for different devices the enum mappinc could be changed, some of the values might be invalid
 * this test access table with such ENUM field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e
dnx_dbal_ut_enum_mapping_multiple_device(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int rv;
    dbal_enum_value_field_olp_command_e olp_cmd_val;
    int hw_value_from_xml_insert_instead_of_learn[DBAL_NOF_ENUM_OLP_COMMAND_VALUES] = { 0, 1, 2, 3, 4, 5, 0 };
    int hw_value_from_xml_other[DBAL_NOF_ENUM_OLP_COMMAND_VALUES] = { 4, 1, 2, 3, 0, 5, 0 };
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with DBAL_TABLE_L2_DMA_DSP_COMMAND table
     */
    if (table_id != DBAL_TABLE_L2_DMA_DSP_COMMAND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, DBAL_FIELD_OLP_COMMAND, &field_info));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (olp_cmd_val = 0; olp_cmd_val < DBAL_NOF_ENUM_OLP_COMMAND_VALUES; olp_cmd_val++)
    {
        /** clear the handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        diag_dbal_pre_action_logger_close(unit);
        if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_learn_events_wrong_command) ||
            dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_learning_use_insert_cmd))
        {
            if (field_info->enum_val_info[olp_cmd_val].value != hw_value_from_xml_insert_instead_of_learn[olp_cmd_val])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong hw value for enum: %s got: %d, expected %d\n",
                             field_info->enum_val_info[olp_cmd_val].name, field_info->enum_val_info[olp_cmd_val].value,
                             hw_value_from_xml_insert_instead_of_learn[olp_cmd_val]);
            }
            if (olp_cmd_val == DBAL_ENUM_FVAL_OLP_COMMAND_INSERT)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND, INST_SINGLE, olp_cmd_val);
                rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
                diag_dbal_post_action_logger_restore(unit);
                if (rv == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
                }
                continue;
            }
        }
        else
        {
            if (field_info->enum_val_info[olp_cmd_val].value != hw_value_from_xml_other[olp_cmd_val])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong hw value for enum: %s got: %d, expected %d\n",
                             field_info->enum_val_info[olp_cmd_val].name, field_info->enum_val_info[olp_cmd_val].value,
                             hw_value_from_xml_other[olp_cmd_val]);
            }

            if (olp_cmd_val == DBAL_ENUM_FVAL_OLP_COMMAND_LEARN_UNUSED)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND, INST_SINGLE, olp_cmd_val);
                rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
                diag_dbal_post_action_logger_restore(unit);
                if (rv == 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
                }
                continue;
            }
        }
        diag_dbal_post_action_logger_restore(unit);
        /*
         * Normal operation
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND, INST_SINGLE, olp_cmd_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
range_of_entries_single_key(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    int jj;
    uint32 res_field_val = 0, res_field2_val = 0;
    uint32 max_val = 0;
    dbal_fields_e key_field_1, res_field, res_field2;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    key_field_1 = table->keys_info[0].field_id;
    res_field = table->multi_res_info[0].results_info[0].field_id;
    if (table_id == DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE)
    {
        res_field = DBAL_FIELD_ENABLE_STACKING_UC;
        res_field2 = DBAL_FIELD_ENABLE_STACKING_MC;
    }

    max_val = table->keys_info[0].max_value;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** setting all entries to 0x1 */
    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 1);
    if (table_id == DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, res_field2, INST_SINGLE, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &res_field_val);
    if (table_id == DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE)
    {
        dbal_value_field32_request(unit, entry_handle_id, res_field2, INST_SINGLE, &res_field2_val);
    }
    for (jj = 0; jj < max_val; jj++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (res_field_val != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field1 Setting all, wrong value for field %s, expecting 1 received %d index %d\n",
                         dbal_field_to_string(unit, res_field), res_field_val, jj);
        }

        if ((table_id == DBAL_TABLE_FIELD_INGRESS_HEADER_PROFILE) && (res_field2_val != 1))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field2 Setting all, wrong value for field %s, expecting 1 received %d index %d\n",
                         dbal_field_to_string(unit, res_field2), res_field2_val, jj);
        }
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /** setting all entries to 0x1 */
    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, max_val / 2);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &res_field_val);
    for (jj = 0; jj < max_val; jj++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (jj <= max_val / 2)
        {
            if (res_field_val != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Setting half %d, wrong value for field %s, expecting 1 received %d index %d\n",
                             max_val / 2, dbal_field_to_string(unit, res_field), res_field_val, jj);
            }
        }
        else
        {
            if (res_field_val != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Setting half %d, wrong value for field %s, expecting 0 received %d index %d\n",
                             max_val / 2, dbal_field_to_string(unit, res_field), res_field_val, jj);
            }
        }
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief Test the feature setting range of entries in a table and validate that all range is set.
 *   API tested dbal_entry_key_field32_range_set(), dbal_entry_key_field16_range_set, dbal_entry_key_field8_range_set
 * flow:
 * (1)setting all range with value 0x1, validating
 * (2)setting partial range with  value 0xf, validating all range
 * (3)clearing table
 * (4)setting range for two fields with value 0xd, validating
 * (5)clearing table
 *
 */
shr_error_e
dnx_dbal_ut_range_of_entries_set(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    int ii, jj;
    uint32 edit_command_index;
    uint32 max_val = 0;
    uint32 expected_value = 1;
    dbal_fields_e key_field_1, key_field_2, res_field;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_key_fields != 2)
    {
        if (table->nof_key_fields == 1)
        {
            SHR_IF_ERR_EXIT(range_of_entries_single_key(unit, table_id));
            SHR_EXIT();
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "not enough key fields to run the test %s table\n",
                         dbal_logical_table_to_string(unit, table_id));
        }
    }

    if (table->multi_res_info[0].results_info[0].field_nof_bits < 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result field to small to run the test %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    key_field_1 = table->keys_info[0].field_id;
    key_field_2 = table->keys_info[1].field_id;
    res_field = table->multi_res_info[0].results_info[0].field_id;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /** setting all entries to 0x1 */
    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /** validating values   */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        if (table->keys_info[ii].field_id == key_field_1)
        {
            max_val = table->keys_info[ii].max_value;
            break;
        }
    }

    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &edit_command_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);
    for (jj = 0; jj < max_val; jj++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (edit_command_index != expected_value)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Setting all, wrong value for field %s, expecting %d received %d index %d\n",
                         dbal_field_to_string(unit, res_field), expected_value, edit_command_index, jj);
        }
    }

    /** partial range test with value 0xd */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field8_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xf);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &edit_command_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);
    for (jj = 0; jj < max_val; jj++)
    {
        expected_value = 1;
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((jj > 3) && (jj < 11))
        {
            expected_value = 0xf;
        }
        if (edit_command_index != expected_value)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "partial, index %d wrong value for field %s, expecting %d received %d\n", jj,
                         dbal_field_to_string(unit, res_field), expected_value, edit_command_index);
        }
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /** two fields range test */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_2, 3, 17);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xd);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** validating value */
    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &edit_command_index);

    for (jj = 4; jj < 11; jj++)
    {
        expected_value = 0xd;
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        for (ii = 3; ii < 18; ii++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, ii);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            if (edit_command_index != expected_value)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "index %d,%d wrong value for field %s, expecting %d received %d\n", jj,
                             ii, dbal_field_to_string(unit, res_field), expected_value, edit_command_index);
            }
        }
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /** without cleaning the handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xd);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 2);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0x7);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 3);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0x7);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 4);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0x7);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** validating value */
    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &edit_command_index);

    for (jj = 0; jj < max_val; jj++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, jj);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if ((jj == 2) || (jj == 3) || (jj == 4))
        {
            if (edit_command_index != 0x7)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "intermidate, wrong value for field %s, expecting %d received %d\n",
                             dbal_field_to_string(unit, res_field), expected_value, edit_command_index);
            }
        }
        else
        {
            if (edit_command_index != 0xd)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "all, wrong value for field %s, expecting %d received %d\n",
                             dbal_field_to_string(unit, res_field), expected_value, edit_command_index);
            }
        }

    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief Test the feature test the clearing of range of entries
 * flow:
 * (1)setting all range with value 0x1, validating clearing and validating
 * (2)setting partial range with  value 0xf, clearing and validating all range
 * (3)clearing table
 * (4)setting range for two fields with value 0xd, clearing part of the range and validating
 * (5)clearing the rest of the range and validating
 *
 */

shr_error_e
dnx_dbal_ut_range_of_entries_clear(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    int entries_count, entries_counter2;
    uint32 expected_nof_entries = 0;
    dbal_fields_e key_field_1, key_field_2, res_field;
    int is_end;
    int entries_counter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_key_fields < 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "not enough key fields to run the test %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (table->multi_res_info[0].results_info[0].field_nof_bits < 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result field to small to run the test %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    key_field_1 = table->keys_info[0].field_id;
    key_field_2 = table->keys_info[1].field_id;
    res_field = table->multi_res_info[0].results_info[0].field_id;

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** setting all entries to 0x1 */
    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, &entries_counter));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, &entries_counter2));

    if (entries_counter2 != entries_counter * 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "num of entries added in the second %d range set is not equal to the first %d\n",
                     entries_counter2 - entries_counter, entries_counter);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /** clear all entries in range */
    dbal_entry_key_field16_range_set(unit, entry_handle_id, key_field_1, 0, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 1);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** validate that no entries exists  */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot clear all entries 1\n");
    }

    /** partial range test with value 0xd */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xf);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** clear the range */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 0);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** validate that no entries exists  */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot clear all entries 2\n");
    }

    /** two fields range test - adding 105 entries   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_2, 3, 17);

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xd);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    entries_count = 0;
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    expected_nof_entries = 105; /** nof entries added in the range, (7*15)*/
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != expected_nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot correct amout of entries entries left=%d expected =%d\n",
                     entries_count, expected_nof_entries);
    }

    /** clearing part of the entries clearing 7 entries */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 3);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    expected_nof_entries = 98; /** nof entries left in the range, (7*14)*/

    /** validate that only the first reange of entries exists  */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != expected_nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot correct amout of entries entries left=%d expected =%d\n",
                     entries_count, expected_nof_entries);
    }

    /** clearing the rest of the entries  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_2, 4, 17);

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** validate that no entries exists  */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot clear all entries\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_range_of_entries_double(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id, iter_handle_id;
    CONST dbal_logical_table_t *table;
    int entries_count, entries_counter2;
    dbal_fields_e key_field_1, key_field_2, res_field;
    int is_end;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_key_fields < 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "not enough key fields to run the test %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (table->multi_res_info[0].results_info[0].field_nof_bits < 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "result field to small to run the test %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    key_field_1 = table->keys_info[0].field_id;
    key_field_2 = table->keys_info[1].field_id;
    res_field = table->multi_res_info[0].results_info[0].field_id;

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /** multiple range set */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &iter_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, 0xd);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_2, 3);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 4, 10);

    /*
     * setting 7 entries*
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** validate that only the first reange of entries exists  */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, iter_handle_id, &is_end));

    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, iter_handle_id, &is_end));
    }

    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_2, 0, 2);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, key_field_1, 0, 3);

    /*
     * setting 3*4 = 12 entries*
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, iter_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, iter_handle_id, &is_end));

    entries_counter2 = 0;
    while (!is_end)
    {
        entries_counter2++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, iter_handle_id, &is_end));
    }

    if (entries_counter2 != 12 + 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "range set error expected 19 entries counted %d entries\n", entries_counter2);
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * This test set field with multiple instances using the
 * INST_ALL flag. than check all instances values
 */
shr_error_e
dnx_dbal_ut_all_instances_set(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    int nof_counter_inst = 0;
    uint32 pp_port_val = 0x15;
    uint32 core_id_val = 0x1;
    uint32 counters_init_value = 0x10;
    uint32 counters_mutual_value = 0xAB;
    uint32 counters_get_value[DBAL_NUM_OF_FIELD_INSTANCES] = { 0 };
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with EXAMPLE_TABLE_FOR_MUL_INSTANCES table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_TABLE_FOR_MUL_INSTANCES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** set each counter in an entry with specific value  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port_val);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id_val);
    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get
                    (unit, table_id, DBAL_FIELD_COUNTER, FALSE, 0, &nof_counter_inst));
    for (ii = 0; ii < nof_counter_inst; ii++)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, ii, counters_init_value + ii);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get all fields of the same entry and compare its values */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port_val);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    for (ii = 0; ii < nof_counter_inst; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_COUNTER,
                                                            ii, &counters_get_value[ii]));
        if (counters_get_value[ii] != (counters_init_value + ii))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for counter %d, singe inst set step\n", ii);
        }
    }

    /** set all counters in an entry with metual value value  */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port_val);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_ALL, counters_mutual_value);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get all fields of the same entry and compare its values */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port_val);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id_val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    for (ii = 0; ii < nof_counter_inst; ii++)
    {
        counters_get_value[ii] = 0;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_COUNTER,
                                                            ii, &counters_get_value[ii]));
        if (counters_get_value[ii] != counters_mutual_value)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for counter %d, all inst set step %d %d\n", ii,
                         counters_get_value[ii], counters_mutual_value);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_update_handle_with_buffers(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entry_handle_id;
    uint32 buffers_entry_handle_id;
    CONST dbal_logical_table_t *table;
    int key_start_offset = 100;
    int payload_start_offset = 200;
    uint32 payload_buffer[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
    uint32 key_buffer[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS];
    dbal_entry_handle_t *entry_handle, *buffers_entry_handle;
    uint32 *buffer_to_read;
    int result_type = DBAL_RESULT_TYPE_FWD_MACT_FWD_MACT_RESULT_DOUBLE_OUTLIF;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     */
    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** set key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_value;
        uint32 field_value_arr[2] = { 0 };
        field_value = key_start_offset + ii;
        if (table->keys_info[ii].max_value != 0)
        {
            field_value = UTILEX_MIN(table->keys_info[ii].max_value, key_start_offset + ii);
        }
        field_value_arr[0] = field_value;
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, table->keys_info[ii].field_id, field_value_arr);
    }

    /** set value fields */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_ALL, result_type);
    for (ii = 1; ii < table->multi_res_info[result_type].nof_result_fields; ii++)
    {
        uint32 field_value;
        uint32 field_value_arr[2] = { 0 };
        field_value = payload_start_offset + ii;
        if (table->multi_res_info[result_type].results_info[ii].max_value != 0)
        {
            field_value =
                UTILEX_MIN(table->multi_res_info[result_type].results_info[ii].max_value, payload_start_offset + ii);
        }
        field_value_arr[0] = field_value;
        dbal_entry_value_field_arr32_set(unit, entry_handle_id,
                                         table->multi_res_info[result_type].results_info[ii].field_id, INST_ALL,
                                         field_value_arr);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    buffer_to_read = entry_handle->phy_entry.key;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(buffer_to_read, 0, table->key_size, key_buffer));

    buffer_to_read = entry_handle->phy_entry.payload;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (buffer_to_read, 0, table->multi_res_info[result_type].entry_payload_size, payload_buffer));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &buffers_entry_handle_id));
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_payload_update
                        (unit, buffers_entry_handle_id, key_buffer, NULL, DBAL_CORE_ALL,
                         table->multi_res_info[result_type].entry_payload_size, payload_buffer));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_value;
        uint32 field_value_arr[2] = { 0 };
        field_value = key_start_offset + ii;
        if (table->keys_info[ii].max_value != 0)
        {
            field_value = UTILEX_MIN(table->keys_info[ii].max_value, key_start_offset + ii);
        }
        field_value_arr[0] = field_value;
        dbal_entry_key_field_arr32_set(unit, entry_handle_id, table->keys_info[ii].field_id, field_value_arr);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, buffers_entry_handle_id, &buffers_entry_handle));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    entry_handle->handle_id = buffers_entry_handle->handle_id;

    /** Copy the hit bit value */
    buffers_entry_handle->phy_entry.hitbit = entry_handle->phy_entry.hitbit;
    if (sal_memcmp(entry_handle, buffers_entry_handle, sizeof(dbal_entry_handle_t)) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "get and created by buffers handles are not identical\n");
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test validated the behavior of fields with const values
 */
shr_error_e
dnx_dbal_ut_const_values(
    int unit,
    dbal_tables_e table_id)
{
    uint32 res_val = 14;
    uint32 res_val_get = 0;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Allocate handle, init and const values should be set */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    /** negative test, Try to set a different value than the const value to KEY_B */
    diag_dbal_pre_action_logger_close(unit);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_KEY_B, 7);
    diag_dbal_post_action_logger_restore(unit);

    if (!entry_handle->error_info.error_exists)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Expected error on handle due to setting illegal value and it passed successfully");
    }

    /** Add an entry without setting key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, res_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Get the entry without setting key fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNTER, INST_SINGLE, &res_val_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (res_val_get != res_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for result field. set %d, get %d\n", res_val, res_val_get);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test Set  entries from DPC table using CORE_ALL values validate that the HW set correctly
 *
 * core 0, V1=0, v2=0
 * core 1, V1=1, v2=1
 * core all, V1=2
 *
 * validate that:
 * core 0, V1=2, v2=0
 * core 1, V1=2, v2=1
 *
 */
shr_error_e
dnx_dbal_ut_core_all_dpc(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    uint32 result_fields_val[4];
    CONST dbal_logical_table_t *table;
    dbal_fields_e key_field_id = DBAL_FIELD_EMPTY; /** the key field ID other than core_id to be set */
    dbal_fields_e value_field_id_1 = DBAL_FIELD_EMPTY, value_field_id_2 = DBAL_FIELD_EMPTY; /** the value field IDs to validate */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table_id == DBAL_TABLE_INGRESS_PTC_PORT)
    {
        /** example for register IPPF_HIGH_PTC_MAPPINGr   */
        value_field_id_1 = DBAL_FIELD_PTC_CS_PROFILE_VTT2;
        value_field_id_2 = DBAL_FIELD_PTC_CS_PROFILE_VTT3;
        key_field_id = DBAL_FIELD_PTC;
    }
    else if (DBAL_TABLE_INGRESS_PP_RECYCLE_COMMAND)
    {
        /** Both are mapped to IPPE_PRT_RECYCLE_INFOm */
        value_field_id_1 = DBAL_FIELD_IN_PP_PORT;
        value_field_id_2 = DBAL_FIELD_SOURCE_SYSTEM_PORT;
        key_field_id = DBAL_FIELD_RECYCLE_CMD;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "tests supported only for dedicated tables.\n");
    }

    /** adding entry to core 0 both fields 0 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, value_field_id_1, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, value_field_id_2, INST_SINGLE, 0);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** adding entry to core 1 both fields 1 */

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, value_field_id_1, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, value_field_id_2, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /** setting value 1 for field_1 for both cores */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, 0);

    dbal_entry_value_field32_set(unit, entry_handle_id, value_field_id_1, INST_SINGLE, 2);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** doing validations */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, 0);

    dbal_value_field32_request(unit, entry_handle_id, value_field_id_1, INST_SINGLE, &result_fields_val[0]);
    dbal_value_field32_request(unit, entry_handle_id, value_field_id_2, INST_SINGLE, &result_fields_val[1]);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_id, 0);

    dbal_value_field32_request(unit, entry_handle_id, value_field_id_1, INST_SINGLE, &result_fields_val[2]);
    dbal_value_field32_request(unit, entry_handle_id, value_field_id_2, INST_SINGLE, &result_fields_val[3]);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (result_fields_val[0] != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error validation 1 val=%d\n", result_fields_val[0]);
    }

    if (result_fields_val[1] != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error validation 2 val=%d\n", result_fields_val[1]);
    }

    if (result_fields_val[2] != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error validation 3 val=%d\n", result_fields_val[2]);
    }

    if (result_fields_val[3] != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "error validation 4 val=%d\n", result_fields_val[3]);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test Set ang get entries from DPC table using
 * CORE_ANY/CORE_ALL values
 */
shr_error_e
dnx_dbal_ut_core_any_dpc(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entry_handle_id;
    uint32 result_fields_val[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Verify table id */
    if ((table->core_mode != DBAL_CORE_MODE_DPC) || (table->nof_key_fields < 2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table core mode must be DPC\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    /** All key fields are 0, core 0, all result fields is 1 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** All key fields are 0, core 1, all result fields is 1 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        if (field_id == DBAL_FIELD_CORE_ID)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
    }
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get both entries using get core ALL, and verify all
     *  results are 1 */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        if (field_id == DBAL_FIELD_CORE_ID)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_CORE_DEFAULT);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                &result_fields_val[ii][0]));
        if (result_fields_val[ii][0] != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for result field %d, first get stage\n", ii);
        }
    }

    /** All key fields are 0, core ANY, all result fields is 0   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        if (field_id == DBAL_FIELD_CORE_ID)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, DBAL_CORE_ALL);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
    }
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** get fields on core 0 and verify all results are 0   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                &result_fields_val[ii][0]));
        if (result_fields_val[ii][0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for result field %d, second get stage\n", ii);
        }
    }

    /** get fields on core 0 and verify all results are 0   */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                &result_fields_val[ii][0]));
        if (result_fields_val[ii][0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value for result field %d, third get stage\n", ii);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test verify the enum mapping fields functionality.
 * it access table with ENUM_TEST field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e
dnx_dbal_ut_uint64_fields(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd2 = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;

    uint32 key_val = 0x12;
    uint64 key_val_64 = COMPILER_64_INIT(0, key_val);
    uint32 res_hi = 0x11223344;
    uint32 res_lo = 0x55667788;
    uint64 res_val;
    /*
     * uint64 key_val_64;
     */
    uint64 res_val_get;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with DBAL_TABLE_EXAMPLE_FOR_UINT64_USAGE_TEST table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_FOR_UINT64_USAGE_TEST)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd2));

    /*
     * Set Key Fields
     */
    /*
     * COMPILER_64_ZERO(key_val_64);
     */
    /*
     * COMPILER_64_SET(key_val_64, key_val, 0);
     */
    dbal_entry_key_field64_set(unit, entry_handle_wr, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, key_val_64);
    dbal_entry_key_field32_set(unit, entry_handle_rd, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, key_val);
    dbal_entry_key_field32_set(unit, entry_handle_rd2, DBAL_FIELD_ROUTING_ENABLERS_PROFILE, key_val);

    /*
     * Set Payload
     */
    COMPILER_64_ZERO(res_val);
    COMPILER_64_SET(res_val, res_hi, res_lo);
    dbal_entry_value_field64_set(unit, entry_handle_wr, DBAL_FIELD_ENABLERS_VECTOR, INST_SINGLE, res_val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    /*
     * Get Payload
     */
    COMPILER_64_ZERO(res_val_get);
    dbal_value_field64_request(unit, entry_handle_rd, DBAL_FIELD_ENABLERS_VECTOR, INST_SINGLE, &res_val_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT));

    /*
     * Check that val is as expected
     */
    if (COMPILER_64_LO(res_val_get) != COMPILER_64_LO(res_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error, Low part: res_val=0x%x, res_val_get=0x%x\n",
                     COMPILER_64_LO(res_val), COMPILER_64_LO(res_val_get));
    }
    if (COMPILER_64_HI(res_val_get) != COMPILER_64_HI(res_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error, High part: res_val=0x%x, res_val_get=0x%x\n",
                     COMPILER_64_HI(res_val), COMPILER_64_HI(res_val_get));
    }

    /*
     * Get Payload Using All fields
     */
    COMPILER_64_ZERO(res_val_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_rd2, DBAL_GET_ALL_FIELDS));
    dbal_entry_handle_value_field64_get(unit, entry_handle_rd2, DBAL_FIELD_ENABLERS_VECTOR, INST_SINGLE, &res_val_get);

    /*
     * Check that val is as expected
     */
    if (COMPILER_64_LO(res_val_get) != COMPILER_64_LO(res_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error, Low part: res_val=0x%x, res_val_get=0x%x\n",
                     COMPILER_64_LO(res_val), COMPILER_64_LO(res_val_get));
    }
    if (COMPILER_64_HI(res_val_get) != COMPILER_64_HI(res_val))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Comparison error, High part: res_val=0x%x, res_val_get=0x%x\n",
                     COMPILER_64_HI(res_val), COMPILER_64_HI(res_val_get));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief for SBC tables validate that both cores are wrriten by using flag DBAL_COMMIT_VALIDATE_OTHER_CORE
 * when getting an entry.
 */
shr_error_e
dnx_dbal_ut_both_cores(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    uint32 field_value_received, field_value_set = 0x7;
    dbal_fields_e key_field_1, res_field;
    dbal_tables_e extra_table_id = DBAL_NOF_TABLES;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    switch (table_id)
    {
        case DBAL_TABLE_QOS_INGRESS_PHB_PROFILE_INFO:
            key_field_1 = DBAL_FIELD_QOS_PROFILE;
            res_field = DBAL_FIELD_QOS_CS_PROFILE;
            extra_table_id = DBAL_TABLE_INGRESS_PP_PORT;
            break;

        case DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP:
            key_field_1 = DBAL_FIELD_OAM_OPCODE;
            res_field = DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET;
            break;

        case DBAL_TABLE_IN_LIF_PROFILE_TABLE:
            key_field_1 = DBAL_FIELD_IN_LIF_PROFILE;
            res_field = DBAL_FIELD_CS_IN_LIF_VTT_PROFILE;
            field_value_set = 0x2;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                         dbal_logical_table_to_string(unit, table_id));
    }

    /** setting entries  */
    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 3);
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field, INST_SINGLE, field_value_set);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 3);
    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &field_value_received);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT_VALIDATE_OTHER_CORE));

    if (field_value_received != field_value_set)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "other core - received value 0x%x not equal to set value 0x%x\n",
                     field_value_received, field_value_set);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, key_field_1, 3);
    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &field_value_received);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (field_value_received != field_value_set)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "regular core - received value 0x%x not equal to set value 0x%x\n",
                     field_value_received, field_value_set);
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    if (extra_table_id == DBAL_TABLE_INGRESS_PP_PORT)
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, extra_table_id));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, extra_table_id, &entry_handle_id));
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_INJECT_ENABLE, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, extra_table_id, entry_handle_id));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 3);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PP_INJECT_ENABLE, INST_SINGLE,
                                   &field_value_received);

        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if (field_value_received != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Validation for range with core failed expected %d received %d\n",
                         1, field_value_received);
        }

        SHR_IF_ERR_EXIT(dbal_table_clear(unit, extra_table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief for SBC tables validate that both cores are wrriten by using flag DBAL_COMMIT_VALIDATE_OTHER_CORE
 * when getting an entry.
 */
shr_error_e
dnx_dbal_ut_mac_encoding(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    /**
     * Keys built as:
     * 1 - offset 0 size 10
     * 2- offset 21 size 9
     * All other bits are don't care, use two different MAC to set
     * and get
     */
    bcm_mac_t key1_mac = { 0xCB, 0xA9, 0x87, 0x60, 0x43, 0x21 };
    bcm_mac_t key2_mac = { 0x34, 0x56, 0x47, 0x7F, 0xBF, 0x21 };
    bcm_mac_t res_mac = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };

    bcm_mac_t res_mac_1_get = { 0 };
    bcm_mac_t res_mac_2_get = { 0 };
    bcm_mac_t res_mac_3_get = { 0 };

    bcm_mac_t res_mac_1_expected = { 0, 0, 0, 0, 0x0a, 0xbc };
    bcm_mac_t res_mac_2_expected = { 0x12, 0x20, 0, 0, 0, 0 };
    bcm_mac_t res_mac_3_expected = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_EXAMPLE_SW_EM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** setting entries  */
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, key2_mac);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, key1_mac);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_RES_1, INST_SINGLE, res_mac);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_RES_2, INST_SINGLE, res_mac);
    dbal_entry_value_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_RES_3, INST_SINGLE, res_mac);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, key1_mac);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, key2_mac);
    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_L2_RES_1, INST_SINGLE, res_mac_1_get);
    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_L2_RES_2, INST_SINGLE, res_mac_2_get);
    dbal_value_field_arr8_request(unit, entry_handle_id, DBAL_FIELD_L2_RES_3, INST_SINGLE, res_mac_3_get);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    for (ii = 0; ii < 6; ii++)
    {
        if (res_mac_1_get[ii] != res_mac_1_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac1 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
        if (res_mac_2_get[ii] != res_mac_2_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac2 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
        if (res_mac_3_get[ii] != res_mac_3_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac3 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
    }

    sal_memset(res_mac_1_get, 0, sizeof(bcm_mac_t));
    sal_memset(res_mac_2_get, 0, sizeof(bcm_mac_t));
    sal_memset(res_mac_3_get, 0, sizeof(bcm_mac_t));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, key1_mac);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, key1_mac);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_L2_RES_1, INST_SINGLE, res_mac_1_get));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_L2_RES_2, INST_SINGLE, res_mac_2_get));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                    (unit, entry_handle_id, DBAL_FIELD_L2_RES_3, INST_SINGLE, res_mac_3_get));

    for (ii = 0; ii < 6; ii++)
    {
        if (res_mac_1_get[ii] != res_mac_1_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac1 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
        if (res_mac_2_get[ii] != res_mac_2_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac2 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
        if (res_mac_3_get[ii] != res_mac_3_expected[ii])
        {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Mac3 Comparison error Byte %d. Get=0x%02x, Expected=0x%02x\n",
                         ii, res_mac_1_get[ii], res_mac_1_expected[ii])}
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, key2_mac);
    dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, key2_mac);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief use to validate the max value / reset value setting for field by using the
 *  dbal_entry_key_field_predefine_value_set(), dbal_entry_value_field_predefine_value_set
 */
shr_error_e
dnx_dbal_ut_predefine_value(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    uint32 field_value_received, expected_val;
    dbal_fields_e key_field_1, res_field;
    dbal_table_field_info_t table_field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    switch (table_id)
    {
        case DBAL_TABLE_EXAMPLE_FOR_HL_WITH_MEMORY_ALIASING:
            key_field_1 = DBAL_FIELD_MIN_DSP_SIZE;
            res_field = DBAL_FIELD_MPLS_OUT;
            break;

        case DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP:
            key_field_1 = DBAL_FIELD_OAM_OPCODE;
            res_field = DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET;
            break;

        case DBAL_TABLE_IN_LIF_PROFILE_TABLE:
            key_field_1 = DBAL_FIELD_IN_LIF_PROFILE;
            res_field = DBAL_FIELD_CS_IN_LIF_VTT_PROFILE;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                         dbal_logical_table_to_string(unit, table_id));
    }

       /** setting entries  */
    dbal_entry_key_field_predefine_value_set(unit, entry_handle_id, key_field_1, DBAL_PREDEF_VAL_MAX_VALUE);
    dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, res_field, INST_SINGLE,
                                               DBAL_PREDEF_VAL_MAX_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_value_field32_request(unit, entry_handle_id, res_field, INST_SINGLE, &field_value_received);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, res_field, 0, 0, 0, &table_field_info));
    expected_val = table_field_info.max_value;

    if (field_value_received != expected_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong max value, received = %d expected = %d\n", field_value_received,
                     expected_val);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run Struct field encode and decode validation test
 * 1. encode a struct field
 * 2. compare to the expected value
 */
shr_error_e
dnx_dbal_ut_struct_field_encode_decode(
    int unit,
    dbal_tables_e table_id)
{
    dbal_fields_e field_id, field_in_struct_id;
    uint32 field_in_struct_ffs_encoded_val, field_in_struct_ffs_encoded_mask, field_in_struct_fko_encoded_val;
    uint32 field_in_struct_decoded_val, field_in_struct_ffs_decoded_mask;
    uint32 full_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 full_field_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    shr_error_e rv;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set FFC_FIELD_SIZE field inside FWD12_FFC_LAYER_RECORD
     */
    field_id = DBAL_FIELD_FWD12_FFC_LAYER_RECORD;
    field_in_struct_id = DBAL_FIELD_FFC_FIELD_SIZE;     /* 2nd field (offset 8 size 5) */
    field_in_struct_ffs_encoded_val = 4;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, field_id, field_in_struct_id, &field_in_struct_ffs_encoded_val, full_field_val));
    /*
     * Get the decoded value
     */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val, full_field_val));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_ffs_encoded_val != field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " encoded FFC_FIELD_SIZE in FWD12_FFC_LAYER_RECORD to %d, but decoded value: %d\n",
                     field_in_struct_ffs_encoded_val, field_in_struct_decoded_val);
    }

    /*
     * Set FFC_KEY_OFFSET field inside FWD12_FFC_LAYER_RECORD
     */
    field_in_struct_id = DBAL_FIELD_FFC_KEY_OFFSET;     /* 1st field (offset 0 size 8) */
    field_in_struct_fko_encoded_val = 13;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, field_id, field_in_struct_id, &field_in_struct_fko_encoded_val, full_field_val));
    /*
     * Get the decoded value
     */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val, full_field_val));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_fko_encoded_val != field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " encoded FFC_KEY_OFFSET in FWD12_FFC_LAYER_RECORD to %d, but decoded value: %d\n",
                     field_in_struct_fko_encoded_val, field_in_struct_decoded_val);
    }

    /*
     * Get the decoded value for FFC_FIELD_SIZE again
     */
    field_in_struct_id = DBAL_FIELD_FFC_FIELD_SIZE;     /* 2nd field (offset 8 size 5) */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val, full_field_val));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_ffs_encoded_val != field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " encoded FFC_FIELD_SIZE in FWD12_FFC_LAYER_RECORD to %d, but decoded value: %d\n",
                     field_in_struct_ffs_encoded_val, field_in_struct_decoded_val);
    }

    /*
     * Set FFC_KEY_OFFSET field inside FWD12_FFC_LAYER_RECORD - to a too big value
     */
    field_in_struct_id = DBAL_FIELD_FFC_KEY_OFFSET;     /* 1st field (offset 0 size 8) */
    field_in_struct_fko_encoded_val = 0x101;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, field_id, field_in_struct_id, &field_in_struct_fko_encoded_val, full_field_val));
    /*
     * Get the decoded value
     */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val, full_field_val));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_fko_encoded_val == field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " encoded FFC_KEY_OFFSET in FWD12_FFC_LAYER_RECORD to %d, which requires more than 8 bits, but decoding was successful\n",
                     field_in_struct_fko_encoded_val);
    }

    /** Check encode with mask */

    field_id = DBAL_FIELD_FWD12_FFC_LAYER_RECORD;
    field_in_struct_id = DBAL_FIELD_FFC_KEY_OFFSET;     /* 1st field (offset 0 size 8) */
    field_in_struct_ffs_encoded_val = 64;
    field_in_struct_ffs_encoded_mask = 0x3F;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, &field_in_struct_ffs_encoded_val,
                     &field_in_struct_ffs_encoded_mask, full_field_val, full_field_mask));

    field_in_struct_id = DBAL_FIELD_FFC_FIELD_OFFSET;   /* 3rd field (offset 13 size 5) */
    field_in_struct_ffs_encoded_val = 15;
    field_in_struct_ffs_encoded_mask = 0xF;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, &field_in_struct_ffs_encoded_val,
                     &field_in_struct_ffs_encoded_mask, full_field_val, full_field_mask));

    field_in_struct_id = DBAL_FIELD_FFC_FIELD_OFFSET;   /* 3rd field (offset 13 size 5) */
    field_in_struct_ffs_encoded_val = 3;
    field_in_struct_ffs_encoded_mask = 0x3;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, &field_in_struct_ffs_encoded_val,
                     &field_in_struct_ffs_encoded_mask, full_field_val, full_field_mask));
    /*
     * Set FFC_FIELD_SIZE field inside FWD12_FFC_LAYER_RECORD
     */
    field_in_struct_id = DBAL_FIELD_FFC_FIELD_SIZE;     /* 2nd field (offset 8 size 5) */
    field_in_struct_ffs_encoded_val = 4;
    field_in_struct_ffs_encoded_mask = 0x1F;
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode_masked
                    (unit, field_id, field_in_struct_id, &field_in_struct_ffs_encoded_val,
                     &field_in_struct_ffs_encoded_mask, full_field_val, full_field_mask));
    /*
     * Get the decoded value
     */
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode_masked
                    (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val,
                     &field_in_struct_ffs_decoded_mask, full_field_val, full_field_mask, 0));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_ffs_encoded_val != field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " encoded FFC_FIELD_SIZE in FWD12_FFC_LAYER_RECORD to %d, but decoded value: %d\n",
                     field_in_struct_ffs_encoded_val, field_in_struct_decoded_val);
    }
    /*
     * Compare encoded and decoded masks
     */
    if ((field_in_struct_ffs_encoded_mask != field_in_struct_ffs_decoded_mask))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " encoded mask FFC_FIELD_SIZE in FWD12_FFC_LAYER_RECORD to %d, but decoded value: %d\n",
                     field_in_struct_ffs_encoded_mask, field_in_struct_ffs_decoded_mask);
    }

    /*
     * Try setting an invalid field in struct
     */
    field_in_struct_id = DBAL_FIELD_FWD12_FFC_LAYER_RECORD;
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_fields_struct_field_encode(unit, field_id, field_in_struct_id, &field_in_struct_decoded_val,
                                         full_field_val);
    diag_dbal_post_action_logger_restore(unit);
    if (!rv)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " negative test passed, should return error - encoded an invalid field in FWD12_FFC_LAYER_RECORD\n");
    }

    /*
     * Try setting an invalid struct
     */
    field_id = DBAL_FIELD_FFC_FIELD_SIZE;
    field_in_struct_id = DBAL_FIELD_FFC_FIELD_SIZE;
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_fields_struct_field_encode(unit, field_id, field_in_struct_id, &field_in_struct_decoded_val,
                                         full_field_val);
    diag_dbal_post_action_logger_restore(unit);
    if (!rv)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " negative test passed, should return error - encoded an invalid struct\n");
    }

    /** sub field validations (one of the struct fields is parent field and adding subfield) */

    field_in_struct_fko_encoded_val = 0x11;
    sal_memset(full_field_val, 0, sizeof(full_field_val));
    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                    (unit, DBAL_FIELD_ITMH_BASE, DBAL_FIELD_PORT_ID, &field_in_struct_fko_encoded_val, full_field_val));

    SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                    (unit, DBAL_FIELD_ITMH_BASE, DBAL_FIELD_PORT_ID, &field_in_struct_decoded_val, full_field_val));
    /*
     * Compare encoded and decoded values
     */
    if ((field_in_struct_fko_encoded_val != field_in_struct_decoded_val))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     " encoded DBAL_FIELD_PORT_ID in DBAL_FIELD_ITMH_BASE to %d, which is subfield not decoded seccussfuly\n",
                     field_in_struct_fko_encoded_val);
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_OLP_HEADER_CONFIGURATION));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OLP_HEADER_CONFIGURATION, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OLP_COMMAND_DESTINATION, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                 DBAL_RESULT_TYPE_OLP_HEADER_CONFIGURATION_UDH_O_ITMH_PTCH2);
    dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_BASE, INST_SINGLE, full_field_val);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief this test validate dynamic table operations, it creates and destroy tables, adding entries to created tables.
 *
 */
shr_error_e
dnx_dbal_ut_dynamic_table_validations(
    int unit,
    dbal_tables_e in_table_id)
{
    int rv, iter;
    dbal_tables_e first_table_id, table_id, wrong_table_id;
    int nof_result_fields = 3, nof_key_fields = 2;
    dbal_table_field_input_info_t keys_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { {0} };
    dbal_table_field_input_info_t results_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE] = { {0} };
    uint32 entry_handle_id;
    dbal_table_mdb_access_info_t access_info = { 0 };
    dbal_table_mdb_access_info_t access_info_rec = { 0 };
    dbal_fields_e dynamic_key_field, dynamic_result_field, temp_dynamic_id;
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_create(unit, DBAL_FIELD_TYPE_DEF_UINT, "dynamic field 1", &dynamic_key_field));
    SHR_IF_ERR_EXIT(dbal_fields_field_create
                    (unit, DBAL_FIELD_TYPE_DEF_VLAN_ID, "dynamic field 2", &dynamic_result_field));
    temp_dynamic_id = dynamic_result_field;
    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, dynamic_result_field));
    SHR_IF_ERR_EXIT(dbal_fields_field_create
                    (unit, DBAL_FIELD_TYPE_DEF_VLAN_ID, "dynamic field 2", &dynamic_result_field));

    if (temp_dynamic_id != dynamic_result_field)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " recreated ID %d not equal to the original ID %d\n", dynamic_result_field,
                     temp_dynamic_id);
    }

    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_fields_field_destroy(unit, DBAL_FIELD_PP_PORT);
    diag_dbal_post_action_logger_restore(unit);
    if (!rv)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " negative test failed, should return error - destroying  field\n");
    }

    keys_info[0].field_id = DBAL_FIELD_PP_PORT;
    keys_info[0].field_nof_bits = DBAL_USE_DEFAULT_SIZE;
    keys_info[1].field_id = dynamic_key_field;
    keys_info[1].field_nof_bits = DBAL_USE_DEFAULT_SIZE;

    results_info[0].field_id = DBAL_FIELD_FEC;
    results_info[0].field_nof_bits = DBAL_USE_DEFAULT_SIZE;
    results_info[1].field_id = DBAL_FIELD_IN_LIF;
    results_info[1].field_nof_bits = DBAL_USE_DEFAULT_SIZE;
    results_info[2].field_id = dynamic_result_field;
    results_info[2].field_nof_bits = DBAL_USE_DEFAULT_SIZE;

    /**table creation validations */
    SHR_IF_ERR_EXIT(dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_TCAM_BY_ID,
                                             DBAL_CORE_MODE_SBC, nof_key_fields, keys_info, nof_result_fields,
                                             results_info, "ctest_dyn_tbl", &table_id));

    access_info.tcam_handler_id = 20;
    access_info.physical_db_id[0] = DBAL_PHYSICAL_TABLE_TCAM;
    access_info.nof_physical_tables = 1;
    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_set(unit, table_id, DBAL_ACCESS_METHOD_MDB, (void *) &access_info));

    SHR_IF_ERR_EXIT(dbal_tables_table_access_info_get
                    (unit, table_id, DBAL_ACCESS_METHOD_MDB, (void *) &access_info_rec));
    if (access_info_rec.tcam_handler_id != access_info.tcam_handler_id
        || access_info_rec.physical_db_id[0] != access_info.physical_db_id[0]
        || access_info_rec.nof_physical_tables != access_info.nof_physical_tables)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " handler ID received not equal to set received %d expected 20\n",
                     access_info_rec.tcam_handler_id);
    }
    /**SHR_IF_ERR_EXIT(diag_dbal_logical_table_dump(unit, table_id, 1, &sand_control));*/

    /** validating that it is possible to set dynamic fields */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, dynamic_result_field, INST_SINGLE, 1);
    dbal_entry_key_field32_set(unit, entry_handle_id, dynamic_key_field, 15);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    if (entry_handle->error_info.error_exists)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " error setting field %s\n",
                     dbal_field_to_string(unit, entry_handle->error_info.field_id));
    }

    first_table_id = table_id;

    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, table_id));

    diag_dbal_pre_action_logger_close(unit);

    rv = dbal_tables_table_destroy(unit, table_id);

    diag_dbal_post_action_logger_restore(unit);

    if (!rv)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " negative test failed, should return error - destroying nonexistent table\n");
    }

    for (iter = (first_table_id - DBAL_NOF_TABLES); iter < dnx_data_dbal.table.nof_dynamic_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_TCAM_BY_ID,
                                                 DBAL_CORE_MODE_SBC, nof_key_fields, keys_info, nof_result_fields,
                                                 results_info, NULL, &table_id));
    }

    diag_dbal_pre_action_logger_close(unit);

    rv = dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_TCAM_BY_ID,
                                  DBAL_CORE_MODE_SBC, nof_key_fields, keys_info, nof_result_fields, results_info,
                                  NULL, &wrong_table_id);

    diag_dbal_post_action_logger_restore(unit);

    if (!rv)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " negative test failed, should return error - creating table over the limit\n");
    }

    /**Adding entries validations - TBD  */

    /**destroying all tables */

    for (iter = (first_table_id - DBAL_NOF_TABLES); iter < dnx_data_dbal.table.nof_dynamic_tables_get(unit); iter++)
    {
        SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, iter + DBAL_NOF_TABLES));
    }

    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, dynamic_key_field));
    SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, dynamic_result_field));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run iterator over multiple result types test
 * 1. Iterate over a multiple results table's results
 * 2. compare to the expected value
 */
shr_error_e
dnx_dbal_ut_iterator_mul_result(
    int unit,
    dbal_tables_e table_id)
{
    int ii, jj, tt, is_end, entries_count, nof_res_types_to_test, max_keys;
    int MAX_NOF_RES_TYPES_TO_TEST = 3;
    int ENTRIES_PER_RES_TYPE = 3;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id;
    uint32 res_type[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    uint32 first_key_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    int num_of_disabled_res_types = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    max_keys = (MAX_NOF_RES_TYPES_TO_TEST * ENTRIES_PER_RES_TYPE);

    /**Check if the table is valid for the test */
    if ((table->nof_key_fields < 1) ||
        ((table->keys_info[0].field_nof_bits < 32) && ((1 << table->keys_info[0].field_nof_bits) < max_keys)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Table must have at least one key, and the first key should be big enough for %d values. This test can not run with %s table\n",
                     max_keys, dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Commit 3 entries for each result type. Do it for maximum
     *  MAX_NOF_RES_TYPES_TO_TEST result types. Total
     *  MAX_NOF_RES_TYPES_TO_TEST*ENTRIES_PER_RES_TYPE entries. Each
     *  of these tables has at least 2 result types, And the first
     *  key has at least 4 bits (so MAX_NOF_RES_TYPES_TO_TEST is set
     *  to 5). This first key will be set to be a modulo of the
     *  result type index */
    nof_res_types_to_test =
        (MAX_NOF_RES_TYPES_TO_TEST > table->nof_result_types) ? table->nof_result_types : MAX_NOF_RES_TYPES_TO_TEST;
    for (jj = 0; jj < nof_res_types_to_test; jj++)
    {
        if (table->multi_res_info[jj].is_disabled)
        {
            num_of_disabled_res_types++;
            continue;
        }

        for (tt = 0; tt < ENTRIES_PER_RES_TYPE; tt++)
        {
            /** Set All keys to 0 , only the first key set to be set to be a
             *  modulo of the result type index */
            for (ii = 0; ii < table->nof_key_fields; ii++)
            {
                uint32 val = 0;
                if ((ii == 0))
                {
                    val = jj + MAX_NOF_RES_TYPES_TO_TEST * tt;
                }
                field_id = table->keys_info[ii].field_id;
                dbal_entry_key_field32_set(unit, entry_handle_id, field_id, val);
            }

            /** Set the result type */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, jj);

            /**  All result fields set to 1   */
            for (ii = 1; ii < table->multi_res_info[jj].nof_result_fields; ii++)
            {
                uint32 val = 1;

                field_id = table->multi_res_info[jj].results_info[ii].field_id;
                if (table->multi_res_info[jj].results_info[ii].arr_prefix_size)
                {
                    dbal_table_field_info_t *table_field = &(table->multi_res_info[jj].results_info[ii]);

                    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &val, table_field->arr_prefix,
                                                              table_field->field_nof_bits,
                                                              table_field->field_nof_bits +
                                                              table_field->arr_prefix_size, &val));
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, val);
            }

            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        }
    }

    /** Iterate to find all entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    entries_count = 0;
    while (!is_end)
    {
        entries_count++;

        /** Get result type */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, res_type));
        /**Get first key value */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                        (unit, entry_handle_id, table->keys_info[0].field_id, first_key_val));

        /**Check equality */
        if (res_type[0] != (first_key_val[0] % MAX_NOF_RES_TYPES_TO_TEST))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "First key was set to to be a modulo of the result type value (%d), but contains the value %d\n",
                         res_type[0], first_key_val[0]);
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    }

    if (entries_count != (nof_res_types_to_test - num_of_disabled_res_types) * (ENTRIES_PER_RES_TYPE))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Committed total of %d entries, but iterator has %d entries\n",
                     (nof_res_types_to_test - num_of_disabled_res_types) * ENTRIES_PER_RES_TYPE, entries_count);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_iterator_hit_bit_rule(
    int unit,
    dbal_tables_e table_id)
{
    int ii, is_end, entries_count;
    int nof_entries_to_add = 10;
    uint32 return_key_val = 0xFFFFFFFF;
    uint32 first_key_val = 60;
    uint32 entry_handle_id, entry_handle_id2;
    uint32 default_lif = 0;
    CONST dbal_logical_table_t *table;
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    if (table_id != DBAL_TABLE_ING_VSI_INFO_DB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The hit bit rule iterator test is written to run over ING_VSI_INFO_DB only\n");
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Add nof_entries entries, with different result types */
    for (ii = 0; ii < nof_entries_to_add; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, first_key_val + ii);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TOPOLOGY_ID, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }

    /*
     * Read default LIF of port 201, core0, and change that LIF to point to the odd VSIs
     * per VSI, send packet to default LIF to hit that VSI.
     */
    SHR_IF_ERR_EXIT(bcm_port_get(unit, 201, &flags, &interface_info, &mapping_info));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id2));
    dbal_entry_key_field32_set(unit, entry_handle_id2, DBAL_FIELD_PP_PORT, mapping_info.pp_port);
    dbal_entry_key_field32_set(unit, entry_handle_id2, DBAL_FIELD_CORE_ID, mapping_info.core);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id2, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id2, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, &default_lif));

    for (ii = 0; ii < nof_entries_to_add; ii += 2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_IN_AC_INFO_DB, entry_handle_id2));
        dbal_entry_key_field32_set(unit, entry_handle_id2, DBAL_FIELD_IN_LIF, default_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id2, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_IN_AC_INFO_DB_IN_LIF_AC_MP_OPTIMIZED_LEARN_PAYLOAD);
        dbal_entry_value_field32_set(unit, entry_handle_id2, DBAL_FIELD_FODO_ASSIGNMENT_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_ENUM_FORWARD_DOMAIN_FROM_LIF);
        dbal_entry_value_field32_set(unit, entry_handle_id2, DBAL_FIELD_VSI, INST_SINGLE, first_key_val + ii);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id2, DBAL_COMMIT));
        sh_process_command(unit,
                           "tx 1 PSRC=201 DATA=0x0011223344AA000000000001810000010800c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c");
    }

    /** Iterate to find all entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, entry_handle_id,
                                                     DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY |
                                                     DBAL_ENTRY_ATTR_HIT_SECONDARY, DBAL_HITBIT_SET_RULE));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &return_key_val));

        if (return_key_val & 0x1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found unexpected entry. expected only even keys, but found VSI=%d\n",
                         return_key_val);
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != (nof_entries_to_add / 2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n", nof_entries_to_add / 2,
                     entries_count);
    }
    dbal_iterator_destroy_macro(unit, entry_handle_id);

    /** Iterate to find all entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, entry_handle_id,
                                                     DBAL_ENTRY_ATTR_HIT_GET | DBAL_ENTRY_ATTR_HIT_PRIMARY |
                                                     DBAL_ENTRY_ATTR_HIT_SECONDARY, DBAL_HITBIT_NOT_SET_RULE));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &return_key_val));

        if ((return_key_val & 0x1) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found unexpected entry. expected only odd keys, but found VSI=%d\n",
                         return_key_val);
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != (nof_entries_to_add / 2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n", nof_entries_to_add / 2,
                     entries_count);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_table_hitbit(
    int unit,
    dbal_tables_e table_id)
{
    uint32 hitbit;
    uint32 hitbit_get;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "The table hitbit test is written to run over DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD only\n");
    }

    /** Set hitbit for table */
    hitbit = FALSE;
    SHR_IF_ERR_EXIT(dbal_tables_hitbit_enable(unit, table_id, hitbit));

    /** Get hitbit for table - expected result hitbit = 0 */
    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_HITBIT_EN, &hitbit_get));
    if (hitbit != hitbit_get)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "HitBit Value is [%d] expected HitBit Value is [0]\n", hitbit_get);
    }

    /** Set hitbit for table */
    hitbit = TRUE;
    SHR_IF_ERR_EXIT(dbal_tables_hitbit_enable(unit, table_id, hitbit));

    /** Get hitbit for table  - expected result hitbit = 1 */
    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_HITBIT_EN, &hitbit_get));
    if (hitbit != hitbit_get)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "HitBit Value is [%d] expected HitBit Value is [1]\n", hitbit_get);

    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_iterator_age_rule(
    int unit,
    dbal_tables_e table_id)
{
    int ii, is_end, entries_count;
    int nof_entries_age1 = 3, nof_entries_age2 = 2;
    int age1_val = 1, age2_val = 6;
    uint32 first_key_val = 60;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    bcm_mac_t mac = { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "The age rule iterator test is written to run over FWD_MACT table only\n");
    }

    if (SAL_BOOT_PLISIM)
    {
        SHR_EXIT();
    }

    if (dnx_data_l2.feature.feature_get(unit, dnx_data_l2_feature_age_machine_pause_after_flush))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "age feature is not supported");
    }

    /** Set learning time to 0, to have manual age control */
    SHR_IF_ERR_EXIT(bcm_l2_age_timer_meta_cycle_set(unit, 0));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Add age1_val entries */
    for (ii = 0; ii < nof_entries_age1; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, first_key_val + ii);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, ii);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, 3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }

    /** Call the switch which decrease the aging X5 */
    for (ii = 0; ii < 5; ii++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchL2AgeScan, 0));
    }

    /** Add age1_val entries */
    for (ii = 0; ii < nof_entries_age2; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FODO, 2 * first_key_val + ii);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 100 + ii);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, 3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }

    /** Call the switch which decrease the aging X1 */
    for (ii = 0; ii < 1; ii++)
    {
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_set(unit, bcmSwitchL2AgeScan, 0));
    }

    /** Iterate to find all entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != (nof_entries_age1 + nof_entries_age2))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n",
                     nof_entries_age1 + nof_entries_age2, entries_count);
    }
    dbal_iterator_destroy_macro(unit, entry_handle_id);

    /** Iterate to find all age1 entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_AGE, age1_val));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != nof_entries_age1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n", nof_entries_age1, entries_count);
    }
    dbal_iterator_destroy_macro(unit, entry_handle_id);

    /** Iterate to find all age2 entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_AGE, age2_val));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != nof_entries_age2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n", nof_entries_age2, entries_count);
    }
    dbal_iterator_destroy_macro(unit, entry_handle_id);

    /** Iterate to find no entries */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_attribute_rule_add(unit, entry_handle_id, DBAL_ENTRY_ATTR_AGE, 4));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    entries_count = 0;
    while (!is_end)
    {
        entries_count++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    if (entries_count != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected to find %d entries, but found %d\n", 0, entries_count);
    }
    dbal_iterator_destroy_macro(unit, entry_handle_id);

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform simple set get with masked APIs in TCAM CS table
 */
shr_error_e
dnx_dbal_ut_tcam_cs_validations(
    int unit,
    dbal_tables_e table_id)
{
    int ii, is_end;
    uint32 entry_handle_id;
    dbal_entry_handle_t *entry_handle;
    CONST dbal_logical_table_t *table;
    int extra_entries[] = { 5, 6, 18, 19 };
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    if (table->access_method != DBAL_ACCESS_METHOD_TCAM_CS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has access method %s instead of %s. \n",
                     dbal_logical_table_to_string(unit, table_id),
                     dbal_access_method_to_string(unit, table->access_method),
                     dbal_access_method_to_string(unit, DBAL_ACCESS_METHOD_TCAM_CS));
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        if (DBAL_FIELD_CORE_ID != field_id)
        {
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, 1, 3);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_MAX_NUM_OF_CORES - 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));
        if (DBAL_FIELD_CORE_ID != field_id && field_val[0] != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected 1 received %d \n",
                         dbal_field_to_string(unit, field_id), field_val[0]);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
        if (field_val[0] != 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected 1 received %d \n",
                         dbal_field_to_string(unit, field_id), field_val[0]);
        }
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        if (DBAL_FIELD_CORE_ID != field_id)
        {
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, 1, 2);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }

    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_EXISTS)
    {
        SHR_IF_ERR_EXIT(rv);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry rewritten without update flag.\n");
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_MAX_NUM_OF_CORES - 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));

    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry found after clear.\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    if (table->core_mode == DBAL_CORE_MODE_DPC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(rv);
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cleared non existent entry.\n");
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range did not clear all entries 1\n");
    }

    /*
     * Write entries of even and odd numbers and see that the iterator follows them.
     */
    for (ii = 0; ii < (sizeof(extra_entries) / sizeof(extra_entries[0])); ii++)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, extra_entries[ii]));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    for (ii = 0; ii < (sizeof(extra_entries) / sizeof(extra_entries[0])); ii++)
    {
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
        if (is_end || entry_handle->access_id_set == FALSE || entry_handle->phy_entry.entry_hw_id != extra_entries[ii])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator didn't find entry %d.\n", extra_entries[ii]);
        }
    }

    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "iterator didn't reach the end after reading all entries.\n");
    }

    /*
     * Delete the table.
     */

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range did not clear all entries 1\n");
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Perform simple set get with masked APIs in an MDB TCAM table
 */
shr_error_e
dnx_dbal_ut_mdb_tcam_validations(
    int unit,
    dbal_tables_e table_id)
{
    int ii, is_end;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    uint32 access_id;
    uint32 field_size;
    uint32 full_mask;
    shr_error_e rv;

    uint32 result_before_update = 1;
    uint32 result_after_update = 0;
    uint32 key_before_update = 1;
    uint32 key_after_update = 2;

    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->access_method != DBAL_ACCESS_METHOD_MDB && table->table_type != DBAL_TABLE_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "this test supported only for MDB indirect TCAM tests\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        /** First time we write the key with a mask and read it with a mask.*/
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_size));
        full_mask = (field_size == SAL_UINT32_NOF_BITS) ? (-1) : (SAL_UPTO_BIT(field_size));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, key_before_update, full_mask);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, 7, &access_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    /** negative test that validate not exists */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Negative test failed get access ID expected not found, received %s\n",
                     shrextend_errmsg_get(rv));
    }
    rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Negative test failed clear access ID expected not found, received %s\n",
                     shrextend_errmsg_get(rv));
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** validating get for the entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 mask_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));
        if (field_val[0] != key_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_before_update, field_val[0]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit, entry_handle_id, field_id,
                                                                     field_val, mask_val));
        if (field_val[0] != key_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_before_update, field_val[0]);
        }
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_size));
        full_mask = (field_size == SAL_UINT32_NOF_BITS) ? (-1) : (SAL_UPTO_BIT(field_size));
        if (mask_val[0] != full_mask)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for mask of field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), full_mask, mask_val[0]);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
        if (field_val[0] != result_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), result_before_update, field_val[0]);
        }
    }

    /*
     * Validate updating entry payload. (without removing fields).
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        /** Second time we write the key without a mask (expecting the mask to be all ones and read it with a mask.*/
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_before_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_after_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

    /** validating get for the entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint32 mask_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));
        if (field_val[0] != key_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_before_update, field_val[0]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get(unit, entry_handle_id, field_id,
                                                                     field_val, mask_val));
        if (field_val[0] != key_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_before_update, field_val[0]);
        }
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_size));
        full_mask = (field_size == SAL_UINT32_NOF_BITS) ? (-1) : (SAL_UPTO_BIT(field_size));
        if (mask_val[0] != full_mask)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for mask of field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), full_mask, mask_val[0]);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
        if (field_val[0] != result_after_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), result_after_update, field_val[0]);
        }
    }

    /*
     * Validate updating entry key. (without removing fields).
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        /** Second time we write the key without a mask (expecting the mask to be all ones and read it without a mask.*/
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_after_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_after_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

    /** validating get for the entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));
        if (field_val[0] != key_after_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_after_update, field_val[0]);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
        if (field_val[0] != result_after_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), result_after_update, field_val[0]);
        }
    }

    /** validating that cannot use DBAL_COMMIT flag to update existing entry.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_before_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_EXISTS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     " negative test for updating wihthout appropriate flag passed, expected "
                     "parameter error received %s\n", shrextend_errmsg_get(rv));

    }

    /** validating that entry not found */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, table_id, access_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " negative test passed expected not found received %s\n",
                     shrextend_errmsg_get(rv));

    }

    /** validating that cannot use DBAL_COMMIT_UPDATE flag to create new entry.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_before_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     " negative test for creating entry without appropriate flag passed, expected "
                     "not found received %s\n", shrextend_errmsg_get(rv));

    }

    /** validating that cannot commit an entry with an invalid access ID.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_before_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id + 1));

    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     " negative test for updating without valid access ID, expected not found received %s\n",
                     shrextend_errmsg_get(rv));

    }

    /** validating that the table is empty */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "clear range didnot clear all entries 1\n");
    }

    rv = dnx_field_entry_access_id_destroy(unit, table_id, access_id);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, " Negative test failed destroy access ID expected not found, received %s\n",
                     shrextend_errmsg_get(rv));
    }

    /** validating that cannot commit an entry with a destroyed access ID.*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_before_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id));

    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     " negative test for updating with destroyed valid access ID, expected not found received %s\n",
                     shrextend_errmsg_get(rv));

    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Check functionallity of commit forced feature
 * Run this test on a table that has more than one result field,
 * to get better verification (for MDB-LPM currently no such
 * table). First result field expected to be at least 4 bits.
 */
shr_error_e
dnx_dbal_ut_commit_forced(
    int unit,
    dbal_tables_e table_id)
{
    uint32 ii, nof_results, setValue;
    uint32 updatedValue = 15;
    uint32 set_entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 upd_entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 get_entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id;
#define MAX_NUM_RESULTS 5
    uint32 get_values[MAX_NUM_RESULTS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&key_field_val[0][0], 0, sizeof(key_field_val));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    nof_results =
        MAX_NUM_RESULTS >
        table->multi_res_info[0].nof_result_fields ? table->multi_res_info[0].nof_result_fields : MAX_NUM_RESULTS;

    /*
     * Create an entry handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &set_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &upd_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &get_entry_handle_id));

    /*
     * All key fields are set to 0
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field_arr32_set(unit, set_entry_handle_id, field_id, key_field_val[ii]);
        dbal_entry_key_field_arr32_set(unit, upd_entry_handle_id, field_id, key_field_val[ii]);
        dbal_entry_key_field_arr32_set(unit, get_entry_handle_id, field_id, key_field_val[ii]);
    }

    /*
     * All result fields are set by index
     */
    for (ii = 0; ii < nof_results; ii++)
    {
        field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, set_entry_handle_id, field_id, INST_SINGLE, ii);
        dbal_value_field_arr32_request(unit, get_entry_handle_id, field_id, INST_SINGLE, get_values[ii]);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, set_entry_handle_id, DBAL_COMMIT_FORCE));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));

    /*
     * Check that all get and set values are equal
     */
    for (ii = 0; ii < nof_results; ii++)
    {
        if (get_values[ii][0] != ii)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For result index %d - set value 0x%x but get value is 0x%x\n",
                         ii, ii, get_values[ii][0]);
        }

        else if (get_values[ii][1] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For result index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                         ii, get_values[ii][1]);
        }
    }

    /*
     * Update the first result only
     */
    field_id = table->multi_res_info[0].results_info[0].field_id;
    dbal_entry_value_field32_set(unit, upd_entry_handle_id, field_id, INST_SINGLE, updatedValue);
    dbal_value_field_arr32_request(unit, get_entry_handle_id, field_id, INST_SINGLE, get_values[0]);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, upd_entry_handle_id, DBAL_COMMIT_FORCE));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));

    /*
     * Check that all get and set values are still equal - including the updated value
     */
    for (ii = 0; ii < nof_results; ii++)
    {
        setValue = (ii == 0) ? updatedValue : ii;

        if (get_values[ii][0] != setValue)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For result index %d - set value 0x%x but get value is 0x%x\n",
                         ii, setValue, get_values[ii][0]);
        }

        else if (get_values[ii][1] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For result index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                         ii, get_values[ii][1]);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** \brief this test checks basic scenario for tcam entries using XXX_id DBAL APIs */
shr_error_e
dnx_dbal_ut_tcam_table_test(
    int unit,
    dbal_tables_e table_id)
{
    CONST dbal_logical_table_t *table;
    uint32 entry_access_id, entry_handle_id;
    uint32 received_entry_access_id = 0;
    int ii;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Create an entry handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, 3, &entry_access_id));

    /*
     * All key fields are set to 0
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        int max_value = table->multi_res_info[0].results_info[ii].max_value;

        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            max_value = 0;
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, max_value);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &received_entry_access_id));

    if (received_entry_access_id != entry_access_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong access ID received = %d, expected = %d\n", received_entry_access_id,
                     entry_access_id);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }

    SHR_IF_ERR_EXIT(dbal_entry_access_id_by_key_get(unit, entry_handle_id, &received_entry_access_id, DBAL_COMMIT));

    if (received_entry_access_id != entry_access_id)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong access ID received = %d, expected = %d\n", received_entry_access_id,
                     entry_access_id);
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, table_id, entry_access_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Verify the error code of entry_get, entry_clear,
 * commit_update functions
 */
shr_error_e
dnx_dbal_ut_non_direct_error_codes(
    int unit,
    dbal_tables_e table_id)
{
    int ii, round;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /** Verify table id */
    if ((table_id != DBAL_TABLE_BIER_BFER_TERMINATION) && (table_id != DBAL_TABLE_BFD_ENDPOINT_SW_INFO))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Received an unexpected table: %s\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    /*
     * Create an entry handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * All key fields are set to 0
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    }

    /*
     * All result fields are set to 1
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }

    /** 2 rounds of tests */
    for (round = 0; round < 2; ++round)
    {

        /** Call entry_get without any prior commits */
        diag_dbal_pre_action_logger_close(unit);
        diag_dbal_pre_action_mdb_logger_close(unit);
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        diag_dbal_post_action_mdb_logger_restore(unit);
        diag_dbal_post_action_logger_restore(unit);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " negative test - called entry_get without any prior commits - expected '_SHR_E_NOT_FOUND'(-7) error, but received: %d\n",
                         (int) rv);
        }

        /** Call entry_clear without any prior commits */
        diag_dbal_pre_action_logger_close(unit);
        diag_dbal_pre_action_mdb_logger_close(unit);
        rv = dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT);
        diag_dbal_post_action_mdb_logger_restore(unit);
        diag_dbal_post_action_logger_restore(unit);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " negative test - called entry_clear without any prior commits - expected '_SHR_E_NOT_FOUND'(-7) error, but received: %d\n",
                         (int) rv);
        }

        /** Call commit_update without any prior commits */
        diag_dbal_pre_action_logger_close(unit);
        diag_dbal_pre_action_mdb_logger_close(unit);
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE);
        diag_dbal_post_action_mdb_logger_restore(unit);
        diag_dbal_post_action_logger_restore(unit);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         " negative test - called commit_update without any prior commits - expected '_SHR_E_NOT_FOUND'(-7) error, but received: %d\n",
                         (int) rv);
        }

        if (round == 0)
        {
            /*
             * Commit
             */
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            /**Try to commit again */
            diag_dbal_pre_action_logger_close(unit);
            diag_dbal_pre_action_mdb_logger_close(unit);
            rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
            diag_dbal_post_action_mdb_logger_restore(unit);
            diag_dbal_post_action_logger_restore(unit);
            if (rv != _SHR_E_EXISTS)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             " negative test - called entry_commit twice - expected '_SHR_E_EXISTS'(-8) error, but received: %d\n",
                             (int) rv);
            }

            /*
             * Clear
             */
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test is currently not used!!
 * Verify DBAL functionallity after dynamically setting valid
 * indication
 */
shr_error_e
dnx_dbal_ut_field_valid_indication(
    int unit,
    dbal_tables_e in_table_id)
{
#define VALID_IND_MAX_NUM_RESULTS 4
    dbal_tables_e table_id;
    int ii, nof_key_fields = 1;
    CONST dbal_logical_table_t *table;
    uint32 set_entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 get_entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 get_values[VALID_IND_MAX_NUM_RESULTS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { {0} };
    dbal_table_field_input_info_t keys_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { {0} };
    dbal_table_field_input_info_t results_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE] = { {0} };
    /*
     * uint32 entry_access_id;
     */
    dbal_entry_handle_t *entry_handle;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    keys_info[0].field_id = DBAL_FIELD_PP_PORT;
    keys_info[0].field_nof_bits = DBAL_USE_DEFAULT_SIZE;
    results_info[0].field_id = DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS; /* 32b */
    results_info[0].field_nof_bits = 32;
    results_info[0].is_valid_indication_needed = 1;
    results_info[1].field_id = DBAL_FIELD_CRPS_ENGINE_DMA_SELECT;       /* 1b */
    results_info[1].field_nof_bits = 1;
    results_info[1].is_valid_indication_needed = 1;
    results_info[2].field_id = DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START;      /* 15b */
    results_info[2].field_nof_bits = 15;
    results_info[2].is_valid_indication_needed = 1;
    results_info[3].field_id = DBAL_FIELD_FODO; /* 18b */
    results_info[3].field_nof_bits = 18;
    results_info[3].is_valid_indication_needed = 1;

    /**table creation validations */
    SHR_IF_ERR_EXIT(dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_TCAM_BY_ID,
                                             DBAL_CORE_MODE_SBC, nof_key_fields, keys_info, VALID_IND_MAX_NUM_RESULTS,
                                             results_info, NULL, &table_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Create an entry handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &set_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &get_entry_handle_id));

    /*
     * All key fields are set to 0
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, set_entry_handle_id, field_id, 0);
        dbal_entry_key_field32_set(unit, get_entry_handle_id, field_id, 0);
    }

    /*
     * Set at most VALID_IND_MAX_NUM_RESULTS result fields to their max decoded value
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        int max_value = table->multi_res_info[0].results_info[ii].max_value;

        dbal_entry_value_field32_set(unit, set_entry_handle_id, field_id, INST_SINGLE, max_value);
        dbal_value_field_arr32_request(unit, get_entry_handle_id, field_id, INST_SINGLE, get_values[ii]);

    }
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, set_entry_handle_id, &entry_handle));

    if (entry_handle->error_info.error_exists)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error setting fields with dynamic indication in dynamic table\n");
    }

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, get_entry_handle_id, &entry_handle));

    if (entry_handle->error_info.error_exists)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error getting fields with dynamic indication in dynamic table\n");
    }

#if 0
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, table_id, 3, &entry_access_id));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, set_entry_handle_id, entry_access_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, set_entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));

    /*
     * Check that all get and set values are equal
     */
    for (ii = 0; ii < VALID_IND_MAX_NUM_RESULTS; ii++)
    {
        if (table->multi_res_info[0].results_info[ii].max_value != get_values[ii][0])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For result index %d - set value 0x%x but get value is 0x%x\n",
                         ii, table->multi_res_info[0].results_info[ii].max_value, get_values[ii][0]);
        }

        else if (get_values[ii][1] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For result index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                         ii, get_values[ii][1]);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, set_entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, table_id, entry_access_id));
#endif

    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, table_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This test is currently not used!!
 * Verify DBAL functionallity after dynamically setting valid
 * indication
 */
shr_error_e
dnx_dbal_ut_table_restore(
    int unit,
    dbal_tables_e in_table_id)
{
#define TABLE_RESTORE_MAX_NUM_RESULTS 4
    dbal_tables_e table_id;
    int ii, nof_key_fields = 1;
    dbal_logical_table_t *table;
    dbal_table_field_input_info_t keys_info[DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS] = { {0} };
    dbal_table_field_input_info_t results_info[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE] = { {0} };

    SHR_FUNC_INIT_VARS(unit);

    keys_info[0].field_id = DBAL_FIELD_PP_PORT;
    keys_info[0].field_nof_bits = DBAL_USE_DEFAULT_SIZE;
    results_info[0].field_id = DBAL_FIELD_CRPS_ENGINE_SCAN_WAIT_TIME_BETWEEN_ALG_SCANS; /* 32b */
    results_info[0].field_nof_bits = 32;
    results_info[0].is_valid_indication_needed = 1;
    results_info[1].field_id = DBAL_FIELD_CRPS_ENGINE_DMA_SELECT;       /* 1b */
    results_info[1].field_nof_bits = 1;
    results_info[1].is_valid_indication_needed = 1;
    results_info[2].field_id = DBAL_FIELD_CRPS_ENGINE_SCAN_SEQ_BASE_ADDRESS_START;      /* 15b */
    results_info[2].field_nof_bits = 15;
    results_info[2].is_valid_indication_needed = 1;
    results_info[3].field_id = DBAL_FIELD_FODO; /* 18b */
    results_info[3].field_nof_bits = 18;
    results_info[3].is_valid_indication_needed = 1;

    /**table creation validations */
    SHR_IF_ERR_EXIT(dbal_tables_table_create(unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_TCAM_BY_ID,
                                             DBAL_CORE_MODE_SBC, nof_key_fields, keys_info,
                                             TABLE_RESTORE_MAX_NUM_RESULTS, results_info, NULL, &table_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    /** Check key fields */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;

        if (keys_info[ii].field_id != field_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error restoring key field\n");
        }
    }

    /** Check result fields */

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        if ((results_info[ii].field_id != table->multi_res_info[0].results_info[ii].field_id) ||
            ((results_info[ii].field_nof_bits + 1) != table->multi_res_info[0].results_info[ii].field_nof_bits))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error restoring result field\n");
        }
        if (results_info[ii].is_valid_indication_needed
            &&
            !(SHR_BITGET
              (table->multi_res_info[0].results_info[ii].field_indication_bm, DBAL_FIELD_IND_IS_VALID_INDICATION)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error restoring valid indication for field\n");
        }
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 *  \brief Check functionallity of access for HL TCAM
 */
shr_error_e
dnx_dbal_ut_hl_tcam(
    int unit,
    dbal_tables_e table_id)
{
#define MAX_NUM_RESULT_FIELDS 10
    int ii, is_end = 0;
    uint32 partial_set_entry_handle_id, full_set_entry_handle_id, get_entry_handle_id;
    uint32 updated_val = 2;
    CONST dbal_logical_table_t *table;
    uint32 get_key[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 get_k_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 get_values[MAX_NUM_RESULT_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get handles
     *
     * partial_set_entry_handle_id - we do not set result at index 0 - default value should be set by DBAL mechanism - using DBAL_COMMIT_OVERRIDE_D FAULT_ENRTY
     * full_set_entry_handle_id - we commit a full entry using DBAL_COMMIT
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &partial_set_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &get_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &full_set_entry_handle_id));

    /*
     * All key fields are set to 1
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_masked_set(unit, partial_set_entry_handle_id, field_id, 1, 3);
    }
    dbal_entry_key_field32_set(unit, full_set_entry_handle_id, table->keys_info[0].field_id, updated_val);

    /*
     * Set at most MAX_NUM_RESULT_FIELDS result fields
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        uint32 set_val = 1;

        if (ii == 0)
        {
            /*
             * Special value to be set only in full_set_entry_handle_id
             */
            set_val = updated_val;
        }
        else
        {
            dbal_entry_value_field32_set(unit, partial_set_entry_handle_id, field_id, INST_SINGLE, set_val);
        }

        dbal_entry_value_field32_set(unit, full_set_entry_handle_id, field_id, INST_SINGLE, set_val);
        dbal_value_field_arr32_request(unit, get_entry_handle_id, field_id, INST_SINGLE, get_values[ii]);

    }

    /*
     * Commit a partial entry
     * result at index 0 was not set and should have the default value on get
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, partial_set_entry_handle_id, 0));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, partial_set_entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_OVERRIDE_DEFAULT));

    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, get_entry_handle_id, 0));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));

    /*
     * Check that all get and set result values are equal
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 expected_val = 1;
        if (ii == 0)
        {
            expected_val = 0; /** the default value is 0*/
        }

        if (get_values[ii][0] != expected_val)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For result index %d - set value 0x%x but get value is 0x%x\n",
                         ii, expected_val, get_values[ii][0]);
        }

        else if (get_values[ii][1] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For result index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                         ii, get_values[ii][1]);
        }
    }

    /*
     * Check that all get and set key values are equal
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_entry_handle_key_field_arr32_masked_get(unit, get_entry_handle_id, table->keys_info[ii].field_id, get_key,
                                                     get_k_mask);
        {
            if (get_key[0] != 1)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "For key index %d - set value 0x%x but get value is 0x%x\n",
                             ii, 1, get_key[0]);
            }
            else if (get_key[1] != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "For key index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                             ii, get_key[1]);
            }
        }
        if (ii == 0 && get_k_mask[0] != 3)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For key mask index %d - set value 0x%x but get value is 0x%x\n",
                         ii, 3, get_k_mask[0]);
        }
    }

    /*
     * Commit a full entry and verify
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, full_set_entry_handle_id, 0));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, full_set_entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 expected_val = 1;
        if (ii == 0)
        {
            expected_val = updated_val;
        }

        if (get_values[ii][0] != expected_val)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After update: For result index %d - set value 0x%x but get value is 0x%x\n",
                         ii, expected_val, get_values[ii][0]);
        }

        else if (get_values[ii][1] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "For result index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                         ii, get_values[ii][1]);
        }
    }
    /*
     * Check the updated key field
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_masked_get
                    (unit, get_entry_handle_id, table->keys_info[0].field_id, get_key, get_k_mask));
    if (get_key[0] != updated_val)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "For key index %d - set value 0x%x but get value is 0x%x\n",
                     ii, updated_val, get_key[0]);
    }
    else if (get_key[1] != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "For key index %d - get value has a set bit in the range of 32-63. full value for these bits: 0x%x\n",
                     ii, get_key[1]);
    }

    /*
     * Clear one entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, partial_set_entry_handle_id, DBAL_COMMIT));

    /*
     * Check that the entry is empty
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_COMMIT));
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        if (get_values[ii][0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For result index %d - should be empty but get value is 0x%x\n",
                         ii, get_values[ii][0]);
        }
    }
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {

        dbal_entry_handle_key_field_arr32_masked_get(unit, get_entry_handle_id, table->keys_info[ii].field_id, get_key,
                                                     get_k_mask);

        if (get_key[0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For key index %d - should be empty but get value is 0x%x\n", ii, get_key[0]);
        }

        if (get_k_mask[0] != 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "For k_mask index %d - should be empty but get value is 0x%x\n",
                         ii, get_k_mask[0]);
        }
    }

    /*
     * Commit to all id's
     */
    for (ii = 0; ii < table->max_capacity; ++ii)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, full_set_entry_handle_id, ii));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, full_set_entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Clear the table
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /*
     * Check that the table is empty
     */
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, get_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, get_entry_handle_id, &is_end));
    if (!is_end)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Used dbal_table_clear but table %s has entries\n", table->table_name);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief Check functionallity of mask validation for LPM
 *  Table must have at least 2 key fields
 */
shr_error_e
dnx_dbal_ut_lpm_mask(
    int unit,
    dbal_tables_e table_id)
{
    int rv, ii, field_size;
    uint32 entry_handle_id, get_entry_handle_id, max_val, mask = 0;
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id;
    int key_index_0, key_index_1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &get_entry_handle_id));

    /*
     * Set the valid fields according to the tale used
     */
    if (table_id == DBAL_TABLE_IPV4_MULTICAST_PRIVATE_LPM_FORWARD)
    {
        key_index_0 = 1;
        key_index_1 = 3;
    }
    else
    {
        key_index_0 = 0;
        key_index_1 = 1;
    }

    /*
     * First 2 key fields are set with mask
     * Using a valid mask
     */
    field_id = table->keys_info[key_index_0].field_id;
    dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 14);
    dbal_entry_key_field32_set(unit, get_entry_handle_id, field_id, 14);

    field_id = table->keys_info[key_index_1].field_id;
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get
                    (unit, table_id, field_id, TRUE, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE, &max_val));
    SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, table_id, field_id, TRUE, 0, 0, &field_size));
    /*
     * Set mask to one bit less than the full mask
     * Set the two handles to have a different value for this field - the lsb will be different
     * This bit should be ignored because of the mask, making the field value similar
     *
     * Assumptions:
     * 1) field_size>1
     * 2) max_val is odd
     */
    SHR_IF_ERR_EXIT(utilex_bitstream_set_bit_range(&mask, 1, field_size - 1));
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, max_val & mask, mask);
    dbal_entry_key_field32_masked_set(unit, get_entry_handle_id, field_id, max_val | 1, mask);

    /*
     * Set at most MAX_NUM_RESULT_FIELDS result fields - in the set entry only
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }

    /*
     * Verify validation PASS
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Get entry (verify key==6 has entry)
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, get_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Clear entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * All key fields are set to 1
     * Using a valid mask
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        if (field_id == DBAL_FIELD_ZERO_PADDING || field_id == DBAL_FIELD_ZERO_PADDING_1)
        {
            /** ZERO_PADDING fields are not valid and do not need to be set */
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
    }

    /*
     * Verify validation PASS
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set invalid mask to first field
     */
    field_id = table->keys_info[key_index_0].field_id;
    dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, 1, 4);

    /*
     * Try commiting the bad mask
     */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "negative test passed. expected _SHR_E_PARAM when commiting an invalid k_mask\n");
    }

    /*
     * Clear entry
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief Check Entries counter (for any table type)
 *  For Direct and TCAM_DIRECT - (nof_entries==0) at any step
 *  For LPM and EM - nof_entries expected to change according to
 *  action taken
 */
shr_error_e
dnx_dbal_ut_entries_counter(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entries_counter;     /* The expected entries counter at each step */
    uint32 entry_handle_id;
    uint32 result_value[2] = { 1, 0 };
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id;
    int nof_entries;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    if (DBAL_TABLE_IS_TCAM(table))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    }

    /*
     * All key fields are set to 1
     */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        if ((field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
        }
    }

    /*
     * All result fields are set to 1
     */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_value);
    }

    /*
     * Clear table
     * Verify it's entries count according to table type
     * In order for the table->nof_entries to be more than zero, commit an entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    entries_counter = 0;
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
    if (entries_counter != nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "After Table clear action, expected Entries count: %u, but actual is: %d. Table: %s\n",
                     entries_counter, nof_entries, dbal_logical_table_to_string(unit, table_id));
    }

    /*
     * Commit action
     * Verify entries count according to table type
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    if (dbal_tables_is_non_direct(unit, table_id))
    {
        entries_counter++;
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
    if (entries_counter != nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After Commit action, expected Entries count: %u, but actual is: %d. Table: %s\n",
                     entries_counter, nof_entries, dbal_logical_table_to_string(unit, table_id));
    }

    /*
     * Commit Update action
     * Set first result to zero
     * Verify counter does not change
     * Relevant only for LPM and EM - otherwise error will be returned from commit function
     */
    if (dbal_tables_is_non_direct(unit, table_id))
    {

        field_id = table->multi_res_info[0].results_info[0].field_id;

        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
        if (entries_counter != nof_entries)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "After Commit Update action, expected Entries count: %u, but actual is: %d. Table: %s\n",
                         entries_counter, nof_entries, dbal_logical_table_to_string(unit, table_id));
        }
    }

    /*
     * Clear entry
     * Verify entries count according to table type
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    if (dbal_tables_is_non_direct(unit, table_id))
    {
        entries_counter--;
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.get(unit, table_id, &nof_entries));
    if (entries_counter != nof_entries)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "After Clear action, expected Entries count: %u, but actual is: %d. Table: %s\n",
                     entries_counter, nof_entries, dbal_logical_table_to_string(unit, table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 *  \brief TBD
 */
shr_error_e
dnx_dbal_ut_merge_entries(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->multi_res_info[0].nof_result_fields < 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s not supported for this test, has to have more than 1 result field",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (DBAL_TABLE_IS_TCAM(table) && (table->access_method == DBAL_ACCESS_METHOD_MDB))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB TCAM not supported for this test ");
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    if (DBAL_TABLE_IS_TCAM(table))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM currently not supported for this test ");
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, 0));
    }

    /** All key fields are set to 1 */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        if ((field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
        }
    }

    /** All result fields are set to 1  */
    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id,
                                      dbal_tables_is_non_direct(unit, table_id) ? DBAL_COMMIT_FORCE : DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /** All key fields are set to 1 */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        if ((field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
        }
    }

    /** only first result fields set to 1  */
    field_id = table->multi_res_info[0].results_info[0].field_id;
    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id,
                                      dbal_tables_is_non_direct(unit, table_id) ? DBAL_COMMIT_UPDATE : DBAL_COMMIT));

    /** changing to new key, and commiting expecting that only the first result will be set to 1 */
    field_id = table->keys_info[0].field_id;
    dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** validations */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    /** All key fields are set to 1 */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        field_id = table->keys_info[ii].field_id;
        if ((field_id == DBAL_FIELD_CORE_ID) && (dnx_data_device.general.nof_cores_get(unit) == 1))
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 1);
        }
    }

    /** only first result fields set to 1  */
    field_id = table->keys_info[0].field_id;
    dbal_entry_key_field32_set(unit, entry_handle_id, field_id, 0);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val_int[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE,
                                                                field_val_int));
        if (ii == 0)
        {
            if (field_val_int[0] == 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected error! set didnot work field Id %s val %d index %d",
                             dbal_field_to_string(unit, field_id), field_val_int[0], ii);
            }
        }
        else
        {
            if (field_val_int[0] != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "marge failed field %s, val %d index %d",
                             dbal_field_to_string(unit, field_id), field_val_int[0], ii);
            }

        }
    }

    /** Clear entry Verify entries count according to table type*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Run Dynamic result type validation test
 * 1. Create a dynamic result type in PPMC table.
 * 2. Validate that new result properties in table
 * 3. MAking some semantic commands with new result
 */
shr_error_e
dnx_dbal_ut_dynamic_res_type(
    int unit,
    dbal_tables_e table_id)
{
    int ii, rv;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    int nof_result_fields;
    dbal_table_field_input_info_t results_info[3] = { {0} };
    int result_type_hw_value;
    int result_type_index;
    int expected_result_type_index;
    int total_fields_size = 0;
    int result_type_size;
    uint32 read_value;
    int nof_result_type = 4;
    int first_dynamic_result_type = 1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "This test run over MC_ID table only. table %s\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    nof_result_fields = 3;
    results_info[0].field_id = DBAL_FIELD_RESULT_TYPE;
    results_info[0].field_nof_bits = 6;
    results_info[1].field_id = DBAL_FIELD_DESTINATION;
    results_info[1].field_nof_bits = 21;
    results_info[2].field_id = DBAL_FIELD_GLOB_OUT_LIF;
    results_info[2].field_nof_bits = 22;
    result_type_hw_value = 5;
    result_type_size = 6;
    total_fields_size = 49;
    expected_result_type_index = 4;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->nof_result_types != nof_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected nof result types before test. Expected %d, Actual %d\n",
                     nof_result_type, table->nof_result_types);
    }

    /** Here are few negative tests, all should return _SHR_E_PARAM */
    /** Illegal result type size */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                     NULL, 4, result_type_hw_value, &result_type_index);
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));
    diag_dbal_post_action_logger_restore(unit);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with unexpected error (%d), Illegal result field size\n",
                     rv);
    }

    /** Illegal result type hw value */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                     NULL, result_type_size, 0, &result_type_index);
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));
    diag_dbal_post_action_logger_restore(unit);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with unexpected error (%d), Illegal result type hw value\n",
                     rv);
    }

    /** Illegal table_id */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_tables_result_type_add(unit, DBAL_TABLE_EEDB_ARP, nof_result_fields, results_info,
                                     NULL, result_type_size, result_type_hw_value, &result_type_index);
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));
    diag_dbal_post_action_logger_restore(unit);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with unexpected error (%d), Illegal table ID\n", rv);
    }

    /** Illegal First field in structure */
    diag_dbal_pre_action_logger_close(unit);
    results_info[0].field_id = DBAL_FIELD_CDU;
    rv = dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                     NULL, result_type_size, result_type_hw_value, &result_type_index);
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));
    diag_dbal_post_action_logger_restore(unit);
    if (rv != _SHR_E_PARAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Negative test passed with unexpected error (%d), Illegal First field in structure\n", rv);
    }
    results_info[0].field_id = DBAL_FIELD_RESULT_TYPE;

    /** legal call - should pass */
    SHR_IF_ERR_EXIT(dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                                "TESTING_TYPE", result_type_size, result_type_hw_value,
                                                &result_type_index));
    SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));

    if (result_type_index != expected_result_type_index)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. result_type_index = %d, expected %d\n", result_type_index,
                     expected_result_type_index);
    }
    if (table->multi_res_info[result_type_index].entry_payload_size != total_fields_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. entry_payload_size = %d, expected %d\n",
                     table->multi_res_info[result_type_index].entry_payload_size, total_fields_size);
    }
    total_fields_size = 0;
    for (ii = 0; ii < nof_result_fields; ii++)
    {
        if (table->multi_res_info[result_type_index].results_info[ii].field_id != results_info[ii].field_id)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. field_id[%d] %d\n", ii,
                         table->multi_res_info[result_type_index].results_info[ii].field_id);
        }
        if (table->multi_res_info[result_type_index].results_info[ii].field_nof_bits != results_info[ii].field_nof_bits)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. field_nof_bits[%d] %d\n", ii,
                         table->multi_res_info[result_type_index].results_info[ii].field_nof_bits);
        }
        total_fields_size += table->multi_res_info[result_type_index].results_info[ii].field_nof_bits;
    }

    /** Get handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** All key fields are set to 1 */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[ii].field_id, 1);
    }

    /** Set values to results */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0x1234);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0x5678);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Get handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    /** All key fields are set to 1 */
    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[ii].field_id, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Validate results fields */
    read_value = 0;
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &read_value);
    if (read_value != result_type_index)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. RESULT_TYPE = %d, expected %d\n", read_value,
                     result_type_index);
    }
    read_value = 0;
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &read_value);
    if (read_value != 0x1234)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. DESTINATION = %d, expected %d\n", read_value, 0x1234);
    }
    read_value = 0;
    dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &read_value);
    if (read_value != 0x5678)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected results. GLOB_OUT_LIF = %d, expected %d\n", read_value, 0x5678);
    }

    /** Clear entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** destroy dynamic types */
    SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete(unit, table_id, expected_result_type_index));

    if (table->nof_result_types != nof_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected nof result types after destroy. Expected %d, Actual %d\n",
                     nof_result_type, table->nof_result_types);
    }

    /*
     * Now we attempt to perform restore (from SW state) on the result types.
     * Since restore assumes that the existing result types are static, we expect it to double the dynamic result types
     * (which is an illegal scenario, as we can't have different result types with different HW values).
     */

    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore(unit, table_id));
    if (table->nof_result_types != ((nof_result_type - first_dynamic_result_type) * 2 + first_dynamic_result_type))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected nof result types after restore. Expected %d, Actual %d\n",
                     ((nof_result_type - first_dynamic_result_type) * 2 + first_dynamic_result_type),
                     table->nof_result_types);
    }
    for (result_type_index = first_dynamic_result_type; result_type_index < nof_result_type; result_type_index++)
    {
        int result_type_restored_index = result_type_index + nof_result_type - first_dynamic_result_type;
        if (table->multi_res_info[result_type_index].result_type_nof_hw_values !=
            table->multi_res_info[result_type_restored_index].result_type_nof_hw_values ||
            table->multi_res_info[result_type_index].entry_payload_size !=
            table->multi_res_info[result_type_restored_index].entry_payload_size ||
            table->multi_res_info[result_type_index].nof_result_fields !=
            table->multi_res_info[result_type_restored_index].nof_result_fields ||
            table->multi_res_info[result_type_index].reference_field_id !=
            table->multi_res_info[result_type_restored_index].reference_field_id ||
            sal_strncmp(table->multi_res_info[result_type_index].result_type_name,
                        table->multi_res_info[result_type_restored_index].result_type_name,
                        sizeof(table->multi_res_info[result_type_index].result_type_name)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type info %d was not restored properly.\n", result_type_index);
        }
        for (ii = 0; ii < table->multi_res_info[result_type_index].result_type_nof_hw_values; ii++)
        {
            if (table->multi_res_info[result_type_index].result_type_hw_value[ii] !=
                table->multi_res_info[result_type_restored_index].result_type_hw_value[ii])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type info %d was not restored properly. "
                             "Hw value %d (index %d) was restored as %d.\n",
                             result_type_index,
                             table->multi_res_info[result_type_index].result_type_hw_value[ii],
                             ii, table->multi_res_info[result_type_restored_index].result_type_hw_value[ii]);
            }
        }
        for (ii = 0; ii < table->multi_res_info[result_type_index].nof_result_fields; ii++)
        {
            if (sal_memcmp(&(table->multi_res_info[result_type_index].results_info[ii]),
                           &(table->multi_res_info[result_type_restored_index].results_info[ii]),
                           sizeof(table->multi_res_info[result_type_index].results_info[ii])))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type info %d, result field %d was not restored properly.\n",
                             result_type_index, ii);
            }
        }
    }
    /*
     * Delete the restored result type. Note that the SW state wasn't be changed by the restore, so we do not use
     * dbal_tables_dynamic_result_type_delete().
     */

    for (result_type_index = first_dynamic_result_type; result_type_index < nof_result_type; result_type_index++)
    {
        int result_type_restored_index = result_type_index + nof_result_type - first_dynamic_result_type;
        SHR_IF_ERR_EXIT(dbal_db_init_dynamic_result_type_clear
                        (unit, &(((dbal_logical_table_t *) table)->multi_res_info[result_type_restored_index])));
    }
    ((dbal_logical_table_t *) table)->nof_result_types = nof_result_type;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * For all kind of access methods, run iterator with key rules and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e
dnx_dbal_ut_iterator_with_key_rules(
    int unit,
    dbal_tables_e table_id)
{
#define NOF_ITERATIONS 4
    int ii;
    uint32 entry_handle_id;
    int test_section = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    {
        if (table_id == DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT)
        {
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 0);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x2);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            if (dnx_data_device.general.nof_cores_get(unit) == 1)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 2);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, 0x3);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            for (ii = 0; ii < NOF_ITERATIONS; ii++)
            {
                int is_end = 0;
                int entries_counter = 0;
                int jj;
            /** Rules parameters per iteration */
                int expected_counter[NOF_ITERATIONS] = { 3, 2, 1, 0 };

                dbal_condition_types_e rule_key_condition[NOF_ITERATIONS] =
                    { DBAL_CONDITION_LOWER_THAN, DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_IS_ODD,
                    DBAL_CONDITION_IS_EVEN
                };
                dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                    { DBAL_FIELD_PP_PORT, DBAL_FIELD_PP_PORT, DBAL_FIELD_CORE_ID, DBAL_FIELD_CORE_ID };
                uint32 rule_field_val[NOF_ITERATIONS] = { 4, 0, 0, 0 };

            /** For single core devices core ID is always 0 change expected resulte accordingly */
                if (dnx_data_device.general.nof_cores_get(unit) == 1)
                {
                    rule_key_condition[2] = DBAL_CONDITION_IS_EVEN;
                    rule_key_condition[3] = DBAL_CONDITION_IS_ODD;
                    expected_counter[2] = 2;
                }
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

                for (jj = 0; jj <= ii; jj++)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                                    (unit, entry_handle_id, rule_field_id[jj], rule_key_condition[jj],
                                     &rule_field_val[jj], NULL));
                }
                while (!is_end)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                }

                SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

                if (entries_counter != expected_counter[ii])
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                                 test_section, ii, expected_counter[ii], entries_counter);
                }
            }
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
            test_section++;
        }
        else if (table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_LPM_FORWARD)
        {
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 1111);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, 0x1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 2);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 2222);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, 0x2);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 3);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 3333);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_KAPS_RESULT, INST_SINGLE, 0x3);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            for (ii = 0; ii < NOF_ITERATIONS; ii++)
            {
                int is_end = 0;
                int entries_counter = 0;
                int jj;
            /** Rules parameters per iteration */
                int expected_counter[NOF_ITERATIONS] = { 3, 2, 1, 1 };
                dbal_condition_types_e rule_key_condition[NOF_ITERATIONS] =
                    { DBAL_CONDITION_LOWER_THAN, DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_IS_ODD,
                    DBAL_CONDITION_EQUAL_TO
                };
                dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                    { DBAL_FIELD_VRF, DBAL_FIELD_VRF, DBAL_FIELD_IPV4, DBAL_FIELD_IPV4 };
                uint32 rule_field_val[NOF_ITERATIONS] = { 4, 1, 0, 3333 };

                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

                for (jj = 0; jj <= ii; jj++)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                                    (unit, entry_handle_id, rule_field_id[jj], rule_key_condition[jj],
                                     &rule_field_val[jj], NULL));
                }
                while (!is_end)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                }

                SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

                if (entries_counter != expected_counter[ii])
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                                 test_section, ii, expected_counter[ii], entries_counter);
                }
            }
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
            test_section++;
        }
        else if (table_id == DBAL_TABLE_EXAMPLE_SW_EM_LONG_KEY)
        {
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, 1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, 1);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ASSOCIATED_ENDPOINTS_LIST_HEAD, INST_SINGLE,
                                         1);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, 2);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, 2);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ASSOCIATED_ENDPOINTS_LIST_HEAD, INST_SINGLE,
                                         2);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_1, 3);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_KEY_2, 3);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ASSOCIATED_ENDPOINTS_LIST_HEAD, INST_SINGLE,
                                         3);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

            for (ii = 0; ii < NOF_ITERATIONS; ii++)
            {
                int is_end = 0;
                int entries_counter = 0;
                int jj;
            /** Rules parameters per iteration */
                int expected_counter[NOF_ITERATIONS] = { 3, 2, 1, 0 };
                dbal_condition_types_e rule_key_condition[NOF_ITERATIONS] =
                    { DBAL_CONDITION_LOWER_THAN, DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_IS_ODD,
                    DBAL_CONDITION_IS_EVEN
                };
                dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                    { DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY, DBAL_FIELD_EMPTY };
                uint32 key_val_1[2] = { 0, 1 };
                uint32 key_val_2[2] = { 0x100001, 0 };
                uint32 *rule_field_val[NOF_ITERATIONS] = { key_val_1, key_val_2, key_val_1, key_val_1 };

                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
                SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

                for (jj = 0; jj <= ii; jj++)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                                    (unit, entry_handle_id, rule_field_id[jj], rule_key_condition[jj],
                                     rule_field_val[jj], NULL));
                }
                while (!is_end)
                {
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                }

                SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

                if (entries_counter != expected_counter[ii])
                {
                    SHR_ERR_EXIT(_SHR_E_FAIL,
                                 "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                                 test_section, ii, expected_counter[ii], entries_counter);
                }
            }
        /** Clear Table */
            SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
            test_section++;
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * For all kind of access methods, run iterator with value rules and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e
dnx_dbal_ut_iterator_with_value_rules(
    int unit,
    dbal_tables_e table_id)
{
#define NOF_ITERATIONS 4
    int ii;
    uint32 entry_handle_id;
    int test_section = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT)
    {
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        if (dnx_data_device.general.nof_cores_get(unit) == 1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 2);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, 0x3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        for (ii = 0; ii < NOF_ITERATIONS; ii++)
        {
            int is_end = 0;
            int entries_counter = 0;
            int jj;
            /** Rules parameters per iteration */
            int expected_counter[NOF_ITERATIONS] = { 3, 2, 1, 0 };

            dbal_condition_types_e rule_val_condition[NOF_ITERATIONS] =
                { DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_BIGGER_THAN,
                DBAL_CONDITION_BIGGER_THAN
            };
            dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                { DBAL_FIELD_VLAN_DOMAIN, DBAL_FIELD_PORT_VID, DBAL_FIELD_DEFAULT_LIF, DBAL_FIELD_DESTINATION };
            uint32 rule_field_val[NOF_ITERATIONS] = { 0, 0, 0, 0 };

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

            for (jj = 0; jj <= ii; jj++)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, rule_field_id[jj], 0, rule_val_condition[jj],
                                 &rule_field_val[jj], NULL));
            }
            while (!is_end)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }

            SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

            if (entries_counter != expected_counter[ii])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                             test_section, ii, expected_counter[ii], entries_counter);
            }
        }
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        test_section++;
    }

    else if (table_id == DBAL_TABLE_IPV4_UNICAST_PRIVATE_HOST)
    {
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 1111);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_OUTLIF);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0x1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0x1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 2);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 2222);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST_DOUBLE_OUTLIF);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0x2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, 0x2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF_2ND, INST_SINGLE, 0x2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VRF, 3);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IPV4, 3333);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     DBAL_RESULT_TYPE_IPV4_UNICAST_PRIVATE_HOST_FWD_DEST);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, 0x3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        for (ii = 0; ii < NOF_ITERATIONS; ii++)
        {
            int is_end = 0;
            int entries_counter = 0;
            int jj;
            /** Rules parameters per iteration */
            int expected_counter[NOF_ITERATIONS] = { 3, 2, 1, 0 };
            dbal_condition_types_e rule_val_condition[NOF_ITERATIONS] =
                { DBAL_CONDITION_LOWER_THAN, DBAL_CONDITION_BIGGER_THAN, DBAL_CONDITION_IS_EVEN, DBAL_CONDITION_IS_ODD
            };
            dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                { DBAL_FIELD_RESULT_TYPE, DBAL_FIELD_DESTINATION, DBAL_FIELD_GLOB_OUT_LIF_2ND,
                DBAL_FIELD_GLOB_OUT_LIF_2ND
            };
            uint32 rule_field_val[NOF_ITERATIONS] = { 6, 1, 0, 0 };

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

            for (jj = 0; jj <= ii; jj++)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, rule_field_id[jj], 0, rule_val_condition[jj],
                                 &rule_field_val[jj], NULL));
            }
            while (!is_end)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }

            SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

            if (entries_counter != expected_counter[ii])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                             test_section, ii, expected_counter[ii], entries_counter);
            }
        }
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        test_section++;
    }

    else if (table_id == DBAL_TABLE_EXAMPLE_SW_LONG_RES)
    {
        uint32 res_value[12] = { 0 };
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
        res_value[0] = 1;
        res_value[1] = 3;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, 1);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, res_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
        res_value[0] = 1;
        res_value[1] = 2;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, 2);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, res_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
        res_value[0] = 2;
        res_value[1] = 2;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_GROUP_ID, 3);
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_RX_NAME, INST_SINGLE, res_value);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        for (ii = 0; ii < NOF_ITERATIONS; ii++)
        {
            int is_end = 0;
            int entries_counter = 0;
            /** Rules parameters per iteration */
            int expected_counter[NOF_ITERATIONS] = { 2, 1, 1, 2 };
            dbal_condition_types_e rule_val_condition[NOF_ITERATIONS] =
                { DBAL_CONDITION_NOT_EQUAL_TO, DBAL_CONDITION_EQUAL_TO, DBAL_CONDITION_EQUAL_TO,
                DBAL_CONDITION_BIGGER_THAN
            };
            dbal_fields_e rule_field_id[NOF_ITERATIONS] =
                { DBAL_FIELD_RX_NAME, DBAL_FIELD_RX_NAME, DBAL_FIELD_RX_NAME, DBAL_FIELD_RX_NAME };
            uint32 res_rule_value[NOF_ITERATIONS][12] = { {0} };
            uint32 res_rule_mask[NOF_ITERATIONS][12] = { {0} };
            res_rule_value[0][0] = 1;
            res_rule_value[0][1] = 1;
            res_rule_mask[0][0] = 1;
            res_rule_mask[0][1] = 1;

            res_rule_value[1][0] = 1;
            res_rule_value[1][1] = 1;
            res_rule_mask[1][0] = 1;
            res_rule_mask[1][1] = 1;

            res_rule_value[2][0] = 2;
            res_rule_value[2][1] = 1;
            res_rule_mask[2][0] = 2;
            res_rule_mask[2][1] = 0;

            res_rule_value[3][0] = 1;
            res_rule_value[3][1] = 2;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

            if (ii == 3)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, rule_field_id[ii], 0, rule_val_condition[ii],
                                 res_rule_value[ii], NULL));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                                (unit, entry_handle_id, rule_field_id[ii], 0, rule_val_condition[ii],
                                 res_rule_value[ii], res_rule_mask[ii]));
            }
            while (!is_end)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }

            SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));

            if (entries_counter != expected_counter[ii])
            {
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "Wrong number of counted entries (section %d iteration %d). Expected %d, Actual %d\n",
                             test_section, ii, expected_counter[ii], entries_counter);
            }
        }
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        test_section++;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * For all kind of access methods, run iterator with rules and actions and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e
dnx_dbal_ut_iterator_with_rules_actions(
    int unit,
    dbal_tables_e table_id)
{
    int ii;
    uint32 entry_handle_id;
    int test_section = 0;
    int is_end = 0;
    int entries_counter = 0;
    uint32 update_res_field = 0;
    uint32 rule_value = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (table_id == DBAL_TABLE_EXAMPLE_FOR_HL_MULTIPLE_RESULT)
    {
        update_res_field = 2;
        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        /** Entry 1 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 0);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 2 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /** Entry 3 */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, 1);
        if (dnx_data_device.general.nof_cores_get(unit) == 1)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 0);
        }
        else
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, 1);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, 0x3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_VID, INST_SINGLE, 0x3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, 0x3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /** Delete a single entry */
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        rule_value = 1;
        SHR_IF_ERR_EXIT(dbal_iterator_key_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_CONDITION_EQUAL_TO, &rule_value, NULL));
        rule_value = 1;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, 0, DBAL_CONDITION_EQUAL_TO, &rule_value, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

        /** count only 2 entries */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        is_end = 0;
        entries_counter = 0;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != 2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected 2 entries but counter is %d. test section %d\n",
                         entries_counter, test_section);
        }

        /** Update a result field */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        rule_value = 0;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, 0, DBAL_CONDITION_EQUAL_TO, &rule_value, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, entry_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_VLAN_DOMAIN, 0, &update_res_field,
                         NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        rule_value = 2;
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, 0, DBAL_CONDITION_EQUAL_TO, &rule_value, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, entry_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_VLAN_DOMAIN, 0, &update_res_field,
                         NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

        /** count only 2 entries and validate the vlan domain field*/
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        ii = 0;
        is_end = 0;
        entries_counter = 0;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            uint32 get_res_field = 0;
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, 0, &get_res_field));
            if (get_res_field != update_res_field)
            {
                ii++;
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Expected Vlan_domain %d, get vlan domain %d. entry counter %d, test section %d\n",
                             update_res_field, get_res_field, ii, test_section);
            }
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != 2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Expected 2 entries but counter is %d. test section %d\n",
                         entries_counter, test_section);
        }

        /** Clear Table */
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
        test_section++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief this test preform negative tests for field
 * allocator.try to commit / get entries when the resource was not allocated. also validating iterator.
 */

shr_error_e
dnx_dbal_ut_field_allocator(
    int unit,
    dbal_tables_e table_id)
{
    int ii, iter, field_id, rv;
    uint32 entry_handle_id, entry_handle_id_count;
    CONST dbal_logical_table_t *table;
    dbal_fields_e alloc_field_id = 0;
    uint8 is_allocator = DBAL_FIELD_EMPTY;
    dnx_algo_res_dump_data_t info;
    int is_end = 0, counter = 0, start_cnt = 0, expect_cnt = 3;
    dbal_field_types_defs_e field_type;
    int alloc_resource[3];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    /*
     * Count number of table entries at the begining of the test
     */
    while (!is_end)
    {
        start_cnt++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    /*
     * check the table has an allocator field
     */
    for (field_id = 0; field_id < table->nof_key_fields; field_id++)
    {
        if (SHR_BITGET(table->keys_info[field_id].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
        {
            alloc_field_id = table->keys_info[field_id].field_id;
        }
    }

    if (alloc_field_id == DBAL_FIELD_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "no allocator field in table %s\n", dbal_logical_table_to_string(unit, table_id));
    }
    SHR_IF_ERR_EXIT(dbal_fields_is_allocator_get(unit, alloc_field_id, &is_allocator));
    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, alloc_field_id, &field_type));

    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.dump_info_get(unit, &info));
            break;
        case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
            SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.dump_info_get(unit, &info));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the relevant resource.\n");
            break;
    }

    /*
     * set 3 entries
     */
    for (iter = 0; iter < expect_cnt; iter++)
    {
        /*
         * looking for index that was not allocated
         * Start of index 1 as value 0 of teh allocator field is used by BCM init
         */
        for (ii = 1; ii < info.create_data.nof_elements; ii++)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_is_allocated
                            (unit, _SHR_CORE_ALL, 0, ii, table_id, 0, alloc_field_id, 0, &is_allocator));
            if (!is_allocator)
            {
                break;
            }
        }

        if (ii == info.create_data.nof_elements)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "no available elements that was not allocated %s\n",
                         dbal_logical_table_to_string(unit, table_id));
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        for (field_id = 0; field_id < table->nof_key_fields; field_id++)
        {
            if (SHR_BITGET(table->keys_info[field_id].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, alloc_field_id, ii);
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_id].field_id, 0);
            }
        }

        dbal_entry_value_field32_set(unit, entry_handle_id, table->multi_res_info[0].results_info[0].field_id,
                                     INST_SINGLE, 1);

        diag_dbal_pre_action_logger_close(unit);
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
        diag_dbal_post_action_logger_restore(unit);
        diag_dbal_pre_action_logger_close(unit);
        rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
        diag_dbal_post_action_logger_restore(unit);
        rv = dbal_fields_field_type_get(unit, alloc_field_id, &field_type);
        SHR_IF_ERR_EXIT(rv);

        switch (field_type)
        {
            case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
                SHR_IF_ERR_EXIT(algo_qos_db.
                                ingress_qos.allocate_single(unit, DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &ii));
                alloc_resource[iter] = ii;
                break;
            case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
                SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.allocate_single(unit,
                                                                            DNX_ALGO_RES_ALLOCATE_WITH_ID, NULL, &ii));
                alloc_resource[iter] = ii;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the relevant resource.\n");
                break;
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id_count));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id_count, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_count, &is_end));

    /*
     * check we have 3 new entries set
     */
    while (!is_end)
    {
        /** uint32 field_val;*/
        counter++;
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id_count, &is_end));
        /*
         * SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit,entry_handle_id_count, alloc_field_id, &field_val));
         * LOG_CLI((BSL_META("qos profile found is %d\n"), field_val));
         */
    }

    if ((counter - start_cnt) != expect_cnt)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong num of entries found (%d),start (%d) expected (%d)\n", counter, start_cnt,
                     expect_cnt);
    }
    else
    {
        /*
         * LOG_CLI((BSL_META("num of entries found (%d),start (%d) expected (%d)\n"), counter, start_cnt,expect_cnt));
         */
    }

    /*
     * clear the entries from the table
     */
    for (iter = 0; iter < expect_cnt; iter++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        for (field_id = 0; field_id < table->nof_key_fields; field_id++)
        {
            if (SHR_BITGET(table->keys_info[field_id].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR))
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, alloc_field_id, alloc_resource[iter]);
            }
            else
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_id].field_id, 0);
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

        switch (field_type)
        {
            case DBAL_FIELD_TYPE_DEF_QOS_PROFILE:
                SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos.free_single(unit, alloc_resource[iter]));
                break;
            case DBAL_FIELD_TYPE_DEF_ECN_QOS_PROFILE:
                SHR_IF_ERR_EXIT(algo_qos_db.ingress_qos_ecn.free_single(unit, alloc_resource[iter]));
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Add relevant case for the relevant resource.\n");
                break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief this test preform negative tests for field
 * try to write on field which are readonly
 */
shr_error_e
dnx_dbal_ut_readonly_field(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index, rv;
    uint32 seed = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
    seed = 1000 + table_id;
    sal_srand(seed);

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        DIAG_DBAL_RANDOM_FIELD_VAL(key_field_val[field_index], table->keys_info[field_index].field_nof_bits, TRUE);
        dbal_entry_key_field_arr32_set(unit, entry_handle_wr, table->keys_info[field_index].field_id,
                                       key_field_val[field_index]);
    }

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        DIAG_DBAL_MAX_FIELD_VAL(field_val[field_index],
                                (table->multi_res_info[0].results_info[field_index].field_nof_bits + 2));

        diag_dbal_pre_action_logger_close(unit);
        dbal_entry_value_field_arr32_set(unit, entry_handle_wr,
                                         table->multi_res_info[0].results_info[field_index].field_id,
                                         INST_SINGLE, field_val[field_index]);

        diag_dbal_post_action_logger_restore(unit);
    }

    /*
     * Commit table values, This operation should fail as this is a negative test
     */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_read_not_readable_field(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_wr = DBAL_SW_NOF_ENTRY_HANDLES;
    uint32 entry_handle_rd = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index, rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only
     * run with L2_AGE_SCAN table for writeonly
     * and with HCC_BIST_RUN table for trigger
     */
    if ((table_id != DBAL_TABLE_L2_AGE_SCAN) && (table_id != DBAL_TABLE_HCC_BIST_RUN))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get wr & rd table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_wr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_rd));

    /*
     * Set age scan trigger
     */
    if (table_id == DBAL_TABLE_L2_AGE_SCAN)
    {
        SHR_EXIT();
        dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_AGE_SCAN_TRIGGER, INST_SINGLE, 1);
    }
    else if (table_id == DBAL_TABLE_HCC_BIST_RUN)
    {
        dbal_entry_key_field32_set(unit, entry_handle_wr, DBAL_FIELD_CHANNEL, 1);
        dbal_entry_key_field_predefine_value_set(unit, entry_handle_wr, DBAL_FIELD_HBMC_INDEX,
                                                 DBAL_PREDEF_VAL_MIN_VALUE);
        dbal_entry_value_field32_set(unit, entry_handle_wr, DBAL_FIELD_BIST_GO, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_wr, DBAL_COMMIT));

    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        diag_dbal_pre_action_logger_close(unit);
        dbal_value_field32_request(unit, entry_handle_rd, table->multi_res_info[0].results_info[field_index].field_id,
                                   INST_SINGLE, get_field_val[field_index]);
        diag_dbal_post_action_logger_restore(unit);
    }
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_get(unit, entry_handle_rd, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);

    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR\n");
    }
    /*
     * Note: can't clear because one of the field is readonly
     */
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_default_values(
    int unit,
    dbal_tables_e table_id)
{
    CONST dbal_logical_table_t *table;
    dbal_fields_e field_id = DBAL_FIELD_EMPTY;
    uint32 entry_handle_id;
    int field_indx;

    uint32 def_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 def_value_returned[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /** setting the Key */
    for (field_indx = 0; field_indx < table->nof_key_fields; field_indx++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_indx].field_id, 1);
    }

    for (field_indx = 0; field_indx < table->multi_res_info[0].nof_result_fields; field_indx++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_predefine_value_get(unit, entry_handle_id,
                                                        table->multi_res_info[0].results_info[field_indx].field_id,
                                                        0, DBAL_PREDEF_VAL_DEFAULT_VALUE, def_value));

        if (sal_memcmp(def_value, zero_buffer_to_compare, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES) != 0)
        {
            field_id = table->multi_res_info[0].results_info[field_indx].field_id;
            break;
        }
    }

    if (field_id == DBAL_FIELD_EMPTY)
    {
                /** no field with default value was found use the first field.. */
        field_id = table->multi_res_info[0].results_info[0].field_id;
    }

    for (field_indx = 0; field_indx < table->multi_res_info[0].nof_result_fields; field_indx++)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id,
                                                   table->multi_res_info[0].results_info[field_indx].field_id,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (field_indx = 0; field_indx < table->nof_key_fields; field_indx++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_indx].field_id, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, field_id, INST_SINGLE, def_value_returned));

    if (sal_memcmp(def_value, def_value_returned, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "default value was not set to HW table = %s def_value = %d def_value_returned = %d\n",
                     dbal_logical_table_to_string(unit, table_id), def_value[0], def_value_returned[0]);
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (field_indx = 0; field_indx < table->nof_key_fields; field_indx++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_indx].field_id, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (field_indx = 0; field_indx < table->nof_key_fields; field_indx++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_indx].field_id, 1);
    }

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, field_id, INST_SINGLE, def_value_returned));

    if (sal_memcmp(def_value, def_value_returned, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "default value was not set to HW in entry clear table = %s def_value = %d def_value_returned = %d\n",
                     dbal_logical_table_to_string(unit, table_id), def_value[0], def_value_returned[0]);
    }

        /** validating that the API for default entry get works. */
    dbal_entry_default_get(unit, entry_handle_id, 0);

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, field_id, INST_SINGLE, def_value_returned));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_struct_arr_prefix_field(
    int unit,
    dbal_fields_e field_id,
    dbal_fields_e field_in_struct_id)
{
    CONST dbal_sub_struct_field_info_t *struct_field_info = NULL;
    CONST dbal_field_types_basic_info_t *field_info, *field_in_struct_info;
    dbal_field_types_defs_e field_type, field_in_struct_type;
    uint32 field_in_struct_encoded_val[2];
    uint32 field_in_struct_decoded_val;
    uint32 full_field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
    uint32 field_index;
    uint8 val_index;
    uint32 length = 0;
    uint32 arr_prefix_size = 0;
    uint32 arr_prefix_value = 0;
    uint32 nof_struct_fields;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));
    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_info));

    nof_struct_fields = field_info->nof_struct_fields;
    for (field_index = 0; field_index < nof_struct_fields; field_index++)
    {
        struct_field_info = &field_info->struct_field_info[field_index];
        if (struct_field_info->struct_field_id == field_in_struct_id)
        {
            break;
        }
    }
    if (field_index == nof_struct_fields)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field_id %d is not a struct field of field_id %d\n", field_id,
                     field_in_struct_id);
    }

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_in_struct_id, &field_in_struct_type));
    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_in_struct_type, &field_in_struct_info));

    /** encode the field val with the arr prefix use both field min val and max val */
    field_in_struct_encoded_val[0] = field_in_struct_info->min_value;
    field_in_struct_encoded_val[1] = field_in_struct_info->max_value;
    length = struct_field_info->length;
    arr_prefix_size = struct_field_info->arr_prefix_size;
    arr_prefix_value = struct_field_info->arr_prefix;

    for (val_index = 0; val_index < 2; val_index++)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                        (&arr_prefix_value, length, arr_prefix_size, &field_in_struct_encoded_val[val_index]));

        /*
         *  get the encoded value
         */
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, field_id, field_in_struct_id, &field_in_struct_encoded_val[val_index], full_field_val));
        /*
         * Get the decoded value
         */
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, field_id, field_in_struct_id, &field_in_struct_decoded_val, full_field_val));

        /** Compare results */
        if ((field_in_struct_encoded_val[val_index] != field_in_struct_decoded_val))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, " encoded EEI in FEC_JR1_DESTINATION_ETH_RIF_ARP to %x, but decoded value: %x\n",
                         field_in_struct_encoded_val[val_index], field_in_struct_decoded_val);
        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief this test perform encode / decode of arr prefix in struct field.
 * test scenario:
 * 1. encode struct field value P,S2 the result should be S2 without the prefix,
 * 2. decode struct field value S2 the result should be P,S2 with the prefix,
 * 3. compare the full encoded value with the full decoded value.
 */
shr_error_e
dnx_dbal_ut_struct_arr_prefix(
    int unit,
    dbal_tables_e table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * This field has arr prefix as defined in standard_1_fec_field_types_definitions.xml
     */
    SHR_IF_ERR_EXIT(dnx_dbal_ut_struct_arr_prefix_field
                    (unit, DBAL_FIELD_FEC_JR1_DESTINATION_ETH_RIF_ARP, DBAL_FIELD_EEI));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_arr_prefix(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id, res_field_value_received = 0;
    CONST dbal_logical_table_t *table;
    int res_field_index, field_index, rv;
    dbal_fields_e res_field_id = DBAL_FIELD_EMPTY;
    uint32 field_min_val = 0;
    uint32 field_max_val = 0;
    uint32 tmp_field_value = 0;
    dbal_table_field_info_t *table_field = NULL;
    uint32 key_field_val = 0;
    dbal_field_types_basic_info_t *field_type_info;
    uint32 value_to_set = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    for (res_field_index = 0; res_field_index < table->multi_res_info[0].nof_result_fields; res_field_index++)
    {
        if (table->multi_res_info[0].results_info[res_field_index].arr_prefix_size)
        {
            res_field_id = table->multi_res_info[0].results_info[res_field_index].field_id;
            table_field = &(table->multi_res_info[0].results_info[res_field_index]);
            break;
        }
    }

    if (res_field_index == table->multi_res_info[0].nof_result_fields)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table no field with arr prefix \n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (table_id == DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES)
    {
        key_field_val = 6;
    }

    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_min_val));

    tmp_field_value = ((1 << table_field->field_nof_bits) - 1);
    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, table_field->arr_prefix,
                                              table_field->field_nof_bits,
                                              table_field->field_nof_bits + table_field->arr_prefix_size,
                                              &field_max_val));

    value_to_set = field_min_val;
    field_min_val--;

    dbal_fields_field_types_info_get(unit, res_field_id, &field_type_info);

    /**for enum need to complare the hw values */
    if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        int ii;
        uint32 enum_field_min_val = 0;

        value_to_set = -1;

        for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
        {
            if ((field_type_info->enum_val_info[ii].value < field_max_val) &&
                field_type_info->enum_val_info[ii].value > field_min_val)
            {
                value_to_set = ii;
            }

            if (field_type_info->enum_val_info[ii].value < field_min_val)
            {
                enum_field_min_val = ii;
            }
        }

        if (value_to_set == -1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "didnot find enum HW value in the range of the arr prefix \n");
        }
        field_min_val = enum_field_min_val;
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /**Setting the min field value, for enum setting value in range..   */

    dbal_entry_value_field32_set(unit, entry_handle_id, res_field_id, INST_SINGLE, value_to_set);

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
    }

    /**getting value of the field, the value should include the arr_prefix_value. */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, res_field_id, INST_SINGLE, &res_field_value_received));

    if (res_field_value_received != value_to_set)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Min: arr value not equal to expected %d received %d \n",
                     value_to_set, res_field_value_received);
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /**setting the max field value */

    if (field_type_info->print_type != DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, res_field_id, INST_SINGLE, field_max_val);

        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
        }

        /**getting value of the field, the value should include the arr_prefix_value. */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, res_field_id, INST_SINGLE, &res_field_value_received));

        if (res_field_value_received != field_max_val)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "MAX: arr value not equal to expected %d received %d \n",
                         field_max_val, res_field_value_received);
        }

        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** Negative tests bigger then max for enum no such case..   */
    if (field_type_info->print_type != DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        for (field_index = 0; field_index < table->nof_key_fields; field_index++)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
        }

        diag_dbal_pre_action_logger_close(unit);
        dbal_entry_value_field32_set(unit, entry_handle_id, res_field_id, INST_SINGLE, field_max_val + 1);
        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
        diag_dbal_post_action_logger_restore(unit);
        if (rv == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR for bigger than max\n");
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }

    /** Negative tests smaller then min   */

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, table->keys_info[field_index].field_id, key_field_val);
    }

    diag_dbal_pre_action_logger_close(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, res_field_id, INST_SINGLE, field_min_val);
    rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);
    diag_dbal_post_action_logger_restore(unit);
    if (rv == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Negative test passed with no ERROR for bigger than min\n");
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#define NOF_ENTRIES_PER_RT (4)
shr_error_e
dnx_dbal_ut_mact_iterator(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int ii, long_rt_entries = 0;

    /**
     * test parameters
     * 1. destination=0xAAAA, strength=2 (static)
     * 2. destination=0xBBBB, strength=2 (static)
     * 3. destination=0xAAAA, strength=1 (dynamic)
     * 4. destination=0xBBBB, strength=1 (dynamic)
     */
    bcm_mac_t mac_address = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6 };
    uint32 destination_values[NOF_ENTRIES_PER_RT] = { 0xAAAA, 0xBBBB, 0xAAAA, 0xBBBB };
    uint32 strength_values[NOF_ENTRIES_PER_RT] = {
        DBAL_ENUM_FVAL_MAC_STRENGTH_2,
        DBAL_ENUM_FVAL_MAC_STRENGTH_2,
        DBAL_ENUM_FVAL_MAC_STRENGTH_1,
        DBAL_ENUM_FVAL_MAC_STRENGTH_1
    };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only on MACT
     */
    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * First clear the table
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD_MACT));

    /*
     * Add entries
     * per result type add NOF_ENTRIES_PER_RT entries:
     */
    cli_out("Step1. Adding %d entries per result type\n", NOF_ENTRIES_PER_RT);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    for (ii = 0; ii < (NOF_ENTRIES_PER_RT * table->nof_result_types); ii++)
    {
        int payload_size = table->multi_res_info[ii / NOF_ENTRIES_PER_RT].entry_payload_size;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, ii);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_address);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     ii / NOF_ENTRIES_PER_RT);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     destination_values[ii % NOF_ENTRIES_PER_RT]);
        if (payload_size > 64)
        {
            long_rt_entries++;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE,
                                         DBAL_ENUM_FVAL_MAC_STRENGTH_2);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE,
                                         strength_values[ii % NOF_ENTRIES_PER_RT]);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_FORCE));
    }

    /*
     * Iterate table and count entries - should be NOF_ENTRIES_PER_RT*table->nof_result_types
     */
    cli_out("Step2. Iterate table and count entries\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = (NOF_ENTRIES_PER_RT * table->nof_result_types);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "basic iteration failed. found %d entries, expected %d\n", entries_counter,
                         expected_entries_count);
        }
    }

    /*
     * Iterate table with DESTINATION rule and count entries -
     * should be NOF_ENTRIES_PER_RT*table->nof_result_types/2
     */
    cli_out("Step3. Iterate table with destination rule and count entries\n");
    {
        uint32 dest_value_rule[1] = { 0 };
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = (NOF_ENTRIES_PER_RT * table->nof_result_types / 2);
        dest_value_rule[0] = destination_values[0];
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         dest_value_rule, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "destination rule iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }
    }

    /*
     * Iterate table with STRENGTH& DESTINATION rules and count entries -
     * should be NOF_ENTRIES_PER_RT*table->nof_result_types/4
     */
    cli_out("Step4. Iterate table with destination &  mac_strength rules and count entries\n");
    {
        uint32 dest_value_rule = 0;
        uint32 strength_value_rule[1] = { 0 };
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count =
            long_rt_entries / 2 + ((NOF_ENTRIES_PER_RT * table->nof_result_types - long_rt_entries) / 4);
        dest_value_rule = destination_values[1];
        strength_value_rule[0] = DBAL_ENUM_FVAL_MAC_STRENGTH_2;

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         dest_value_rule, 0));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, DBAL_CONDITION_EQUAL_TO,
                         strength_value_rule, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "destination & strength rules iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }
    }

    cli_out("Finish \n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_mact_iterator_key_rule_only(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int ii, nof_entries;

    bcm_mac_t mac_address = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6 };
    int vsi = 1000;
    uint32 destination = 0x1234;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only on MACT
     */
    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * First clear the table
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD_MACT));

    /*
     * Add entries
     * per result type add NOF_ENTRIES_PER_RT entries:
     */
    cli_out("Step1. Adding DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES(%d) + 1 entries of all result types\n",
            DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    ii = 0;
    nof_entries = 0;
    while (nof_entries < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES + 1)
    {
        int payload_size = table->multi_res_info[ii % table->nof_result_types].entry_payload_size;

        if (payload_size > 64)
        {
            ii++;
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi + nof_entries);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_address);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     ii % table->nof_result_types);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     destination + nof_entries);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        ii++;
        nof_entries++;
    }

    /*
     * Iterate table and count entries - should be DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES+1
     */
    cli_out("Step2. Iterate table and count entries\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES + 1;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "basic iteration failed. found %d entries, expected %d\n", entries_counter,
                         expected_entries_count);
        }
    }

    /*
     * Delete DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES entries with key rule on vsi only
     */
    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
    {
        bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleAdd);
    }

    cli_out("Step3. Delete DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES entries with key rule on vsi only\n");
    {
        for (ii = 0; ii < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; ii++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, vsi + ii, 0));
            SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
            SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
        }

        if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
        {
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleCommit);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleClear);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateNormal);
        }
    }

    /*
     * Iterate table and validate a single entry is found,
     * with key VSI=vsi+DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES
     */
    cli_out
        ("Step4. Iterate table and validate a single entry is found, with key VSI=vsi+DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = 1;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "destination & strength rules iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI,
                                   vsi + DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_address);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    }

    cli_out("Finish \n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_mact_iterator_key_and_dest(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int ii, nof_entries;

    bcm_mac_t mac_address = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6 };
    int vsi = 0x1000;
    uint32 destination = 0xffe0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is a special test that can run only on MACT
     */
    if (table_id != DBAL_TABLE_FWD_MACT)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * First clear the table
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, DBAL_TABLE_FWD_MACT));

    /*
     * Add entries
     * per result type add NOF_ENTRIES_PER_RT entries:
     */
    cli_out("Step1. Adding DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES(%d) + 1 entries of all result types\n",
            DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    ii = 0;
    nof_entries = 0;
    while (nof_entries < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES)
    {
        int payload_size = table->multi_res_info[ii % table->nof_result_types].entry_payload_size;

        if (payload_size > 64)
        {
            ii++;
            continue;
        }

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi + nof_entries);
        dbal_entry_key_field_arr8_set(unit, entry_handle_id, DBAL_FIELD_L2_MAC, mac_address);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     ii % table->nof_result_types);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     destination + nof_entries);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, 3);

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        ii++;
        nof_entries++;
    }

    /*
     * Iterate table and count entries - should be DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES
     */
    cli_out("Step2. Iterate table and count entries\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "basic iteration failed. found %d entries, expected %d\n", entries_counter,
                         expected_entries_count);
        }
    }

    /*
     * Delete DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES entries with key rule on vsi only
     */
    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
    {
        bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleAdd);
    }

    cli_out("Step3. Delete entry with destination rule on ends with 0x5\n");
    {
        for (ii = 0; ii < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; ii++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, 0x1000 + ii, 0));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 0x5,
                             0xf));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 2,
                             0x3));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 3,
                             0xf));
            SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
            SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
        }

        if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
        {
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleCommit);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleClear);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateNormal);
        }
    }

    /*
     * Iterate table and validate a single entry is found,
     * with key VSI=vsi+DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES
     */
    cli_out
        ("Step4. Iterate table and validate a DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES -1 entries are found, only one deleted\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES - 1;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "destination & strength rules iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }
    }

    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
    {
        bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleAdd);
    }

    cli_out("Step5. Delete entries with destination rule ends with last bit 1\n");
    {
        for (ii = 0; ii < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; ii++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, 0x1000 + ii, 0));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 0x1,
                             0x1));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 2,
                             0x3));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 3,
                             0xf));
            SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
            SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
        }

        if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
        {
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleCommit);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleClear);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateNormal);
        }
    }

    /*
     * Iterate table and validate a single entry is found,
     * with key VSI=vsi+DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES
     */
    cli_out("Step4. Iterate table and validate half of original entries are removed\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES / 2;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "destination & strength rules iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }
    }

    if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
    {
        bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleAdd);
    }

    cli_out("Step5. Delete entries with destination rule ends with last bit 0\n");
    {
        for (ii = 0; ii < DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES; ii++)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add(unit, entry_handle_id, DBAL_FIELD_VSI,
                                                               DBAL_CONDITION_EQUAL_TO, 0x1000 + ii, 0));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 0x0,
                             0x1));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_MAC_STRENGTH, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 2,
                             0x3));
            SHR_IF_ERR_EXIT(dbal_iterator_value_field32_rule_add
                            (unit, entry_handle_id, DBAL_FIELD_ENTRY_GROUPING, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, 3,
                             0xf));
            SHR_IF_ERR_EXIT(dbal_iterator_entry_action_add(unit, entry_handle_id, DBAL_ITER_ACTION_DELETE));
            SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));
        }

        if (dnx_data_l2.general.feature_get(unit, dnx_data_l2_general_flush_machine_support))
        {
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleCommit);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateRuleClear);
            bcm_switch_control_set(0, bcmSwitchTraverseMode, bcmSwitchTableUpdateNormal);
        }
    }

    /*
     * Iterate table and validate a single entry is found,
     * with key VSI=vsi+DNX_DATA_MAX_L2_DMA_FLUSH_DB_NOF_DMA_RULES
     */
    cli_out("Step4. Iterate table and validate all of original entries are removed\n");
    {
        int is_end = FALSE;
        int entries_counter = 0, expected_entries_count = 0;

        expected_entries_count = 0;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_entries_count_get(unit, entry_handle_id, &entries_counter));
        if (entries_counter != expected_entries_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "destination & strength rules iteration failed. found %d entries, expected %d\n",
                         entries_counter, expected_entries_count);
        }
    }

    cli_out("Finish \n");

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Negative test. Check that user can't set a value for a disabled result type. In table
 *  EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD, result type RT1 is disabled for device with one core */
shr_error_e
dnx_dbal_ut_disabled_result_type(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int cur_result_type, field_index;
    int rv;

    /*
     * key values
     */
    uint32 pp_port = 10;
    uint32 core_id = 0;
    /*
     * result values
     */
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dbal_is_j2c_a0(unit))
    {
        SHR_EXIT();
    }

    /*
     * Verify table ID, This test is valid onlu for specific table
     */
    if (table_id != DBAL_TABLE_EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Set values per result type
     */
    for (cur_result_type = 0; cur_result_type < table->nof_result_types; cur_result_type++)
    {
        /*
         * Get table handle
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /*
         * Set Key Fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, pp_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        pp_port++;

        /*
         * when RT1, expect to fail at dbal_entry_value_field32_set and dbal_entry_commit
         */
        if (table->multi_res_info[cur_result_type].is_disabled)
        {
            diag_dbal_pre_action_logger_close(unit);
            /** setting the result type to disabled result type */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, cur_result_type);
        }
        else
        {
            for (field_index = 0; field_index < table->multi_res_info[cur_result_type].nof_result_fields; field_index++)
            {
                /*
                 * set result type value
                 */
                if (table->multi_res_info[cur_result_type].results_info[field_index].field_id == DBAL_FIELD_RESULT_TYPE)
                {
                    value = cur_result_type;
                }
                /*
                 * set other values (not result type)
                 */
                else
                {
                    value = 1;
                }

                dbal_entry_value_field32_set(unit, entry_handle_id,
                                             table->multi_res_info[cur_result_type].results_info[field_index].field_id,
                                             INST_SINGLE, value);
            }
        }

        rv = dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT);

        if (table->multi_res_info[cur_result_type].is_disabled)
        {
            /** if rt disabled we expect that entry_commit will fail. */
            if (rv == 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Negative test failed! dbal_entry_commit passed for a disabled result type\n");
            }
            diag_dbal_post_action_logger_restore(unit);
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
            SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_dbal_ut_none_direct_mode(
    int unit,
    dbal_tables_e table_id)
{
    CONST dbal_logical_table_t *table;
    uint32 orig_mode;
    uint32 mode = 0;
    uint32 entry_handle;
    int field_index;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (!DBAL_TABLE_IS_NONE_DIRECT(table))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "only none direct tables is supported for this test \n");
    }

    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, &orig_mode));

    /** changing commit mode and checking that it is set */
    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, 1));
    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, &mode));

    if (mode != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "updating mode to optimized failed \n");
    }

    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));

    /** setting all key fields */
    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        dbal_entry_key_field_predefine_value_set(unit, entry_handle, table->keys_info[field_index].field_id,
                                                 DBAL_PREDEF_VAL_MIN_VALUE);
    }

    /** setting all result fields */
    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle,
                                                   table->multi_res_info[0].results_info[field_index].field_id,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MIN_VALUE);
    }

    /** Committing entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT));

    /** checking that entry found (if not found returns error..) */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle, DBAL_GET_ALL_FIELDS));

    /** updating the entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT_UPDATE));

    /** checking that entry found (if not found returns error..)  */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, orig_mode));

    /** Clearing entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

typedef struct
{
    int unit;
    dbal_tables_e table_id;
} table_protection_validate_ut_inputs;

/** Global parameter that helps to validate the table protection feature */
/** 0 - main thread at work 1 - main thread completed 2 - checker thread completed*/
int protection_sync_value = 0;

/** checker thread to validate that the table protection is working.  protection_sync_value is changed to 1 after
 *  table operation is done. */
void
dbal_checker_table_protection(
    void *input)
{
    uint32 entry_handle;
    table_protection_validate_ut_inputs *ut_inputs = (table_protection_validate_ut_inputs *) input;
    int rv;

    /** taking the handle, lock was enabled, thread should wait until main thread release.. */
    rv = dbal_entry_handle_take_macro(ut_inputs->unit, ut_inputs->table_id, &entry_handle);

    if (rv)
    {
        LOG_CLI((BSL_META("\n dbal_entry_handle_take_macro failed!\n")));
        assert(0);
    }

    if (protection_sync_value == 0)
    {
        /** if raeched here means that lock didn't work since main thread didn't complete is work */
        assert(0);
    }

    sal_usleep(50000);

    protection_sync_value = 2;

    /** releasing the lock */
    rv = dbal_entry_handle_release_internal(ut_inputs->unit, entry_handle);

    if (rv)
    {
        LOG_CLI((BSL_META("\n dbal_entry_handle_release_internal failed!\n")));
        assert(0);
    }

}

/** test table protection. Semantic test that validate that can enable protection for table. after enabling protection
 *  checking that handle take and handle release working as expected. TBD check that protection is working as expected.
 *  */
shr_error_e
dnx_dbal_ut_table_protection_validate(
    int unit,
    dbal_tables_e table_id)
{
    CONST dbal_logical_table_t *table;
    uint32 mode;
    uint32 entry_handle;
    int field_index;
    table_protection_validate_ut_inputs inputs = { unit, table_id };
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    protection_sync_value = 0;

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED, 1));
    SHR_IF_ERR_EXIT(dbal_tables_indication_get(unit, table_id, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED, &mode));

    /** handle alloc mutex should be taken for the table. */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));

    /** creating new thread for validate the lock */
    sal_thread_create("checker thread", SAL_THREAD_STKSZ, 1, dbal_checker_table_protection, &inputs);

    /** waiting that the new thread will get to work */
    sal_usleep(90000);

    /** setting all key fields */
    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        dbal_entry_key_field_predefine_value_set(unit, entry_handle, table->keys_info[field_index].field_id,
                                                 DBAL_PREDEF_VAL_MIN_VALUE);
    }

    /** setting all result fields */
    for (field_index = 0; field_index < table->multi_res_info[0].nof_result_fields; field_index++)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle,
                                                   table->multi_res_info[0].results_info[field_index].field_id,
                                                   INST_SINGLE, DBAL_PREDEF_VAL_MIN_VALUE);
    }

    /** Committing entry */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle, DBAL_COMMIT_FORCE));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle));

    protection_sync_value = 1;

    /** freeing the handle -> after releasing the protection, checker thread should start working */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle));

    /** waiting that the new thread will continue to work */
    sal_usleep(50000);

    /** wait till checker thread will complete the his work */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle));

    /** checker thread should change the protection_sync_value to 2. this should be done  */
    if (protection_sync_value != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "checker thread didn't perofrom action %d \n", protection_sync_value);
    }

    /** Clearing table */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle));
    SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_THREAD_PROTECTION_ENABLED, 0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Negative test. Check that we can't set a disabled result
 *  type.
 *  In table EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD,
 *  result type RT0 is enabled for j2c only */
shr_error_e
dnx_dbal_ut_update_result_type(
    int unit,
    dbal_tables_e table_id)
{
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;
    CONST dbal_logical_table_t *table;
    int field_index;
    int rv;

    /*
     * key value
     */
    uint32 vsi = 1;

    /*
     * result types
     */
    uint32 orig_res_type = DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_BASIC;
    uint32 new_res_type = DBAL_RESULT_TYPE_ING_VSI_INFO_DB_VSI_ENTRY_WITH_STATS;

    /*
     * result values
     */
    uint32 value;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Verify table ID, This test is designated for ING_VSI_INFO_DB
     */
    if (table_id != DBAL_TABLE_ING_VSI_INFO_DB)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "This test can not run with %s table\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    /*
     * Get table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * First entry commit
     */

    /*
     * Set Key Field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);

    for (field_index = 0; field_index < table->multi_res_info[orig_res_type].nof_result_fields; field_index++)
    {
        /*
         * skip read only fields
         */
        if ((table->multi_res_info[orig_res_type].results_info[field_index].permission == DBAL_PERMISSION_READONLY) ||
            (table->multi_res_info[orig_res_type].results_info[field_index].permission == DBAL_PERMISSION_TRIGGER))
        {
            continue;
        }
        /*
         * set result type value
         */
        if (table->multi_res_info[orig_res_type].results_info[field_index].field_id == DBAL_FIELD_RESULT_TYPE)
        {
            value = orig_res_type;
        }
        /*
         * set other values (not result type)
         */
        else
        {
            value = 1;
        }

        if (table->multi_res_info[orig_res_type].results_info[field_index].arr_prefix_size)
        {
            dbal_table_field_info_t *table_field = &(table->multi_res_info[orig_res_type].results_info[field_index]);

            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &value, table_field->arr_prefix,
                                                      table_field->field_nof_bits,
                                                      table_field->field_nof_bits +
                                                      table_field->arr_prefix_size, &value));
        }

        dbal_entry_value_field32_set(unit, entry_handle_id,
                                     table->multi_res_info[orig_res_type].results_info[field_index].field_id,
                                     INST_SINGLE, value);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Update result type - loose mode. Should pass */
    SHR_IF_ERR_EXIT(dbal_entry_result_type_update
                    (unit, entry_handle_id, new_res_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Try to update result type to the existing one. Should fail */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    /** Turn of logger in order to avoid error messages */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_result_type_update(unit, entry_handle_id, new_res_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE);
    /** Re-open logger */
    diag_dbal_post_action_logger_restore(unit);
    if ((rv != _SHR_E_PARAM) && (rv != _SHR_E_NOT_FOUND))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error, update should've failed");
    }

    /** Update result type - strict mode. Should fail */
    /** Turn of logger in order to avoid error messages */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_result_type_update(unit, entry_handle_id, orig_res_type, DBAL_RESULT_TYPE_UPDATE_MODE_STRICT);
    /** Re-open logger */
    diag_dbal_post_action_logger_restore(unit);
    if ((rv != _SHR_E_PARAM) && (rv != _SHR_E_NOT_FOUND))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error, update should've failed");
    }

    /** Clear entry */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    /** Re-add entry, leave all result values at default */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, orig_res_type);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Update result type - loose default mode. Should pass */
    SHR_IF_ERR_EXIT(dbal_entry_result_type_update
                    (unit, entry_handle_id, new_res_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Set STAT_OBJECT_ID field, which only exists in the new result type */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, 2);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** Update result type - loose default mode. Should fail */
    /** Turn of logger in order to avoid error messages */
    diag_dbal_pre_action_logger_close(unit);
    SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_entry_result_type_update
                              (unit, entry_handle_id, orig_res_type, DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT),
                              _SHR_E_PARAM);
    /** Re-open logger */
    diag_dbal_post_action_logger_restore(unit);

    /** Update result type to a non-existing result type. Should fail */
    /** Turn of logger in order to avoid error messages */
    diag_dbal_pre_action_logger_close(unit);
    rv = dbal_entry_result_type_update(unit, entry_handle_id, DBAL_NOF_RESULT_TYPE_ING_VSI_INFO_DB + 1,
                                       DBAL_RESULT_TYPE_UPDATE_MODE_LOOSE_DEFAULT);
    /** Re-open logger */
    diag_dbal_post_action_logger_restore(unit);
    if ((rv != _SHR_E_PARAM) && (rv != _SHR_E_NOT_FOUND))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Internal error, update should've failed");
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish \n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* *INDENT-ON* */
