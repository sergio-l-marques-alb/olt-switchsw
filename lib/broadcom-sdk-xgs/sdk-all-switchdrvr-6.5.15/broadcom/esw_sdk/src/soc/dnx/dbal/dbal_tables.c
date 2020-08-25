/*
 * $Id: dpp_dbal.c,v 1.13 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX
#include <shared/bsl.h>
#include "dbal_internal.h"
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <sal/appl/sal.h>
#include <soc/dnxc/dnxc_generic_state_journal.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_hw_entity_porting.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>



static dbal_logical_tables_info_t logical_tables_info[BCM_MAX_NUM_UNITS];

#define DBAL_TABLE_ID_VALIDATE( table_id)                       \
        if (table_id >= dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))     \
        {                                                       \
            SHR_ERR_EXIT(_SHR_E_PARAM,"Illegal table_id %d, max=%d\n", table_id, dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit));\
        }

static shr_error_e dbal_tables_sw_state_restore(
    int unit);

/**************************************************Internal usage APIs *************************************************/

/*
 * Initialize non xml tables db
 */
static shr_error_e
dbal_tables_generic_table_xmls_info_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!DBAL_IS_J2A0)
    {
        SHR_IF_ERR_EXIT(dbal_init_hw_entity_porting_init(unit, &(logical_tables_info[unit].hl_porting_info)));

    }
    SHR_IF_ERR_EXIT(dbal_db_init_hw_element_group_info_init(unit, &(logical_tables_info[unit].groups_info)));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dbal_db_init_dynamic_dbal_tables_sw_state(unit));
    }
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT);

    logical_tables_info[unit].nof_tables_with_error = 0;
    logical_tables_info[unit].hl_porting_info.nof_memories = 0;
    logical_tables_info[unit].hl_porting_info.nof_registers = 0;
    SHR_IF_ERR_EXIT(dbal_db_init_table_alloc(unit, logical_tables_info[unit].logical_tables));
    SHR_IF_ERR_EXIT(dbal_db_init_dynamic_dbal_tables_memory(unit));

    SHR_IF_ERR_EXIT(dbal_tables_generic_table_xmls_info_init(unit));

    SHR_IF_ERR_EXIT(dbal_db_init_logical_tables(unit, DBAL_INIT_FLAGS_NONE, logical_tables_info[unit].logical_tables));
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT);
    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_VALIDATION);

    SHR_IF_ERR_EXIT(dbal_db_init_tables_logical_validation(unit, logical_tables_info[unit].logical_tables));
    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_VALIDATION);

    if (logical_tables_info[unit].nof_tables_with_error)
    {
        LOG_CLI((BSL_META("WARNING, %d DBAL tables had HW error, list of tables: dbal table info status=hwerr\n"),
                 logical_tables_info[unit].nof_tables_with_error));
    }

    if (SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(dbal_tables_sw_state_restore(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_porting_info_get(
    int unit,
    hl_porting_info_t ** hl_porting_info)
{
    SHR_FUNC_INIT_VARS(unit);

    (*hl_porting_info) = &(logical_tables_info[unit].hl_porting_info);

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_groups_info_get(
    int unit,
    hl_groups_info_t ** hl_groups_info)
{
    SHR_FUNC_INIT_VARS(unit);

    (*hl_groups_info) = &(logical_tables_info[unit].groups_info);

    SHR_FUNC_EXIT;
}

/**
 * \brief Free memory of a formula.
 * Formula is a recursive element, that need to be frees from bottom top.
 * Each element that has that more than one variable, should free all its sub formula types.
 * sub formulas should be freed all from the head of the array -> formula_index_in_array=0
 */
void
dbal_tables_formulas_free(
    dbal_offset_formula_t * formula,
    int formula_index_in_array)
{
    if (formula != NULL)
    {
        if (formula->nof_vars != 1)
        {
            int ii;
            for (ii = formula->nof_vars - 1; ii >= 0; ii--)
            {
                dbal_tables_formulas_free(&formula->val_as_formula[ii], ii);
            }
        }
        if (formula_index_in_array == 0)
        {
            SHR_FREE(formula);
        }
    }
}

/**
 * \brief
 * zeros the fields inside a dbal_logical_table_t structure.
 * Used to clear dynamically allocated tables that may have been deleted through error recovery,
 * but the structure representing them was not cleared.
 *   \param [in] unit   - Relevant unit
 *   \param [out] table - Pointer to a dbal_logical_table_t structure to be cleared.
 *
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
static shr_error_e
dbal_tables_dbal_logical_table_t_clear(
    int unit,
    dbal_logical_table_t * table)
{

    dbal_table_field_info_t *results_info;
    int multi_res_index;
    int key_info_data_size_bytes;
    int results_info_data_size_bytes;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table, _SHR_E_PARAM, "table");

    key_info_data_size_bytes = dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t);
    results_info_data_size_bytes = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t);
    sal_memset(table->keys_info, 0, key_info_data_size_bytes);
    if (table->multi_res_info != NULL)
    {
        for (multi_res_index = 0; multi_res_index < table->nof_result_types; multi_res_index++)
        {
            results_info = table->multi_res_info[multi_res_index].results_info;
            sal_memset(&table->multi_res_info[multi_res_index], 0, sizeof(multi_res_info_t));
            table->multi_res_info[multi_res_index].results_info = results_info;
            sal_memset(table->multi_res_info[multi_res_index].results_info, 0, results_info_data_size_bytes);
        }
    }
#if defined(INCLUDE_KBP)
    sal_memset(table->kbp_handles, 0, sizeof(kbp_db_handles_t));
#endif

    SHR_IF_ERR_EXIT(dbal_logical_dynamic_table_t_init(unit, table));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_deinit(
    int unit)
{
    int ii, jj, kk, qq;
    dbal_logical_table_t *table_entry;
    int nof_dynamic_tables_multi_result_types = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLES; ii++)
    {
        table_entry = &logical_tables_info[unit].logical_tables[ii];

        if (table_entry->table_labels)
        {
            SHR_FREE(table_entry->table_labels);
        }

        if (table_entry->keys_info)
        {
            SHR_FREE(table_entry->keys_info);
        }
        if (table_entry->multi_res_info)
        {
            for (jj = 0; jj < table_entry->nof_result_types; jj++)
            {
                SHR_FREE(table_entry->multi_res_info[jj].results_info);
            }
            /** free the superset result type set, for ETM_PP_DESCRIPTOR_EXPANSION it is not supported */
            if (table_entry->has_result_type
                && (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0))
            {
                SHR_FREE(table_entry->multi_res_info[table_entry->nof_result_types].results_info);
            }
            else if(sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") == 0)
            {
                int nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
                for (jj = table_entry->nof_result_types; jj < nof_multi_res_types; jj++)
                {
                    SHR_FREE(table_entry->multi_res_info[jj].results_info);
                }
            }
            SHR_FREE(table_entry->multi_res_info);
        }

        if ((table_entry->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
            || (table_entry->access_method == DBAL_ACCESS_METHOD_TCAM_CS))
        {
            if (table_entry->hl_mapping_multi_res)
            {
                for (jj = 0; jj < table_entry->nof_result_types; jj++)
                {
                    for (kk = 0; kk < DBAL_NOF_HL_ACCESS_TYPES; kk++)
                    {
                        if (table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].num_of_access_fields > 0)
                        {
                            for (qq = 0; qq < table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].num_of_access_fields;
                                 qq++)
                            {
                                dbal_hl_l2p_field_info_t *l2p_fields_info =
                                    &table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].l2p_fields_info[qq];
                                if (l2p_fields_info->nof_conditions > 0)
                                {
                                    SHR_FREE(l2p_fields_info->mapping_condition);
                                }
                                dbal_tables_formulas_free(l2p_fields_info->array_offset_info.formula, 0);
                                dbal_tables_formulas_free(l2p_fields_info->entry_offset_info.formula, 0);
                                dbal_tables_formulas_free(l2p_fields_info->data_offset_info.formula, 0);
                                dbal_tables_formulas_free(l2p_fields_info->block_index_info.formula, 0);
                                dbal_tables_formulas_free(l2p_fields_info->group_offset_info.formula, 0);
                                dbal_tables_formulas_free(l2p_fields_info->alias_data_offset_info.formula, 0);
                            }
                            SHR_FREE(table_entry->hl_mapping_multi_res[jj].l2p_hl_info[kk].l2p_fields_info);
                        }
                    }
                }
                SHR_FREE(table_entry->hl_mapping_multi_res);
            }
        }
        if (table_entry->pemla_mapping.key_fields_mapping)
        {
            SHR_FREE(table_entry->pemla_mapping.key_fields_mapping);
        }
        if (table_entry->pemla_mapping.result_fields_mapping)
        {
            SHR_FREE(table_entry->pemla_mapping.result_fields_mapping);
        }
#if defined(INCLUDE_KBP)
        if (table_entry->kbp_handles)
        {
            SHR_FREE(table_entry->kbp_handles);
        }
#endif
        if (table_entry->table_size_str)
        {
            SHR_FREE(table_entry->table_size_str);
        }
        table_entry->maturity_level = DBAL_MATURITY_LOW;
    }

    /** Deinit dynamic tables */
    for (ii = DBAL_NOF_TABLES; ii < nof_tables; ii++)
    {
        table_entry = &logical_tables_info[unit].logical_tables[ii];

        if (table_entry->table_labels)
        {
            SHR_FREE(table_entry->table_labels);
        }

        if (table_entry->keys_info)
        {
            SHR_FREE(table_entry->keys_info);
        }
        if (table_entry->multi_res_info)
        {
            for (jj = 0; jj < nof_dynamic_tables_multi_result_types; jj++)
            {
                SHR_FREE(table_entry->multi_res_info[jj].results_info);
            }
            SHR_FREE(table_entry->multi_res_info);
        }
#if defined(INCLUDE_KBP)
        if (table_entry->kbp_handles)
        {
            SHR_FREE(table_entry->kbp_handles);
        }
#endif
        table_entry->maturity_level = DBAL_MATURITY_LOW;
    }

    if (logical_tables_info[unit].groups_info.nof_groups > 0)
    {
        SHR_FREE(logical_tables_info[unit].groups_info.group_info);
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_tables_available_table_id_get(
    int unit,
    dbal_tables_e * table_id)
{
    dbal_table_status_e table_status;
    int table_index;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (table_index = DBAL_NOF_TABLES; table_index < nof_tables; table_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_index, &table_status));
        if (table_status == DBAL_TABLE_NOT_INITIALIZED)
        {
            (*table_id) = table_index;
            break;
        }
    }

    if (table_index == dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No available table IDs\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_access_info_set(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info)
{
    dbal_logical_table_t *table;
    int sw_state_table_index;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];

    if (table->access_method != access_method)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incompatible access methods table %s input %s\n",
                     dbal_access_method_to_string(unit, table->access_method),
                     dbal_access_method_to_string(unit, access_method));
    }

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            table->app_id = ((dbal_table_mdb_access_info_t *) (access_info))->app_id;
            table->nof_physical_tables = ((dbal_table_mdb_access_info_t *) (access_info))->nof_physical_tables;
            sal_memcpy(table->physical_db_id, ((dbal_table_mdb_access_info_t *) (access_info))->physical_db_id,
                       sizeof(dbal_physical_tables_e) * DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE);
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.set(unit, table_id,
                                                                       ((dbal_table_mdb_access_info_t
                                                                         *) (access_info))->tcam_handler_id));
            table->app_id_size = ((dbal_table_mdb_access_info_t *) (access_info))->app_id_size;

            
            if (table->table_type != DBAL_TABLE_TYPE_TCAM && table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT)
            {
                SHR_IF_ERR_EXIT(mdb_init_logical_table(unit, table_id, table));
            }
            else
            {
                /** in table type TCAM the only available phy_db_id is */
                if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_TCAM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM table must have DBAL_PHYSICAL_TABLE_TCAM in physical_db_id\n");
                }
            }
            /** for dynamic tables store in sw state */
            if (table_id >= DBAL_NOF_TABLES)
            {
                sw_state_table_index = table_id - DBAL_NOF_TABLES;

                SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.data.set(unit, sw_state_table_index, (CONST dbal_logical_table_t *)table));
            }
            break;

        case DBAL_ACCESS_METHOD_KBP:
#if defined(INCLUDE_KBP)
            ((kbp_db_handles_t *) table->kbp_handles)->db_p = ((kbp_db_handles_t *) access_info)->db_p;
            ((kbp_db_handles_t *) table->kbp_handles)->ad_db_p = ((kbp_db_handles_t *) access_info)->ad_db_p;

            /** for dynamic tables store in sw state */
            if (table_id >= DBAL_NOF_TABLES)
            {
                sw_state_table_index = table_id - DBAL_NOF_TABLES;
                SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.access_info.memwrite(unit, sw_state_table_index,
                        (uint8 *) (table->kbp_handles), 0, sizeof(kbp_db_handles_t)));

            }
#else
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP not compiled\n");
#endif
            break;
        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:

            SHR_ERR_EXIT(_SHR_E_INTERNAL, "setting info not supported for %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal access method %d \n", access_method);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_access_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_access_method_e access_method,
    void *access_info)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];

    if (table->access_method != access_method)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incompatible access methods table %s input %s\n",
                     dbal_access_method_to_string(unit, table->access_method),
                     dbal_access_method_to_string(unit, access_method));
    }

    switch (access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            ((dbal_table_mdb_access_info_t *) (access_info))->app_id = table->app_id;
            ((dbal_table_mdb_access_info_t *) (access_info))->nof_physical_tables = table->nof_physical_tables;
            sal_memcpy(((dbal_table_mdb_access_info_t *) (access_info))->physical_db_id, table->physical_db_id,
                       sizeof(dbal_physical_tables_e) * DBAL_MAX_NUM_OF_PHY_DB_IN_TABLE);
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, table_id,
                                                                       &(((dbal_table_mdb_access_info_t
                                                                           *) (access_info))->tcam_handler_id)));
            ((dbal_table_mdb_access_info_t *) (access_info))->app_id_size = table->app_id_size;
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_KBP:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        case DBAL_ACCESS_METHOD_SW_STATE:

            SHR_ERR_EXIT(_SHR_E_INTERNAL, "getting info not supported for %s \n",
                         dbal_access_method_to_string(unit, access_method));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "illegal access method %d \n", access_method);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Add dynamic result types to sw state, only applicable for PPMC table.
 *   \param [in] unit
 *   \param [in] table_id       - the table add result type to. Only PPMC table is supported.
 *   \param [in] res_type_index - The index of the result type that should be copied to SW state.
 * \return
 * Non-zero(shr_error_e) in case of an error.
 */
shr_error_e
dbal_tables_table_res_type_sw_state_add(
    int unit,
    dbal_tables_e table_id,
    int res_type_index)
{
    dbal_logical_table_t *table;
    uint8 res_field_index;
    int res_type_wr_index;
    multi_res_info_t multi_res_info;
    dbal_multi_res_info_status_e res_info_status;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    table = &logical_tables_info[unit].logical_tables[table_id];

    /** Find the first available place in SW state to allocate for the result type.*/
    for (res_type_wr_index = 0; res_type_wr_index < DBAL_MAX_NUMBER_OF_RESULT_TYPES; res_type_wr_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                        multi_res_info_status.get(unit, res_type_wr_index, &res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_AVAIL)
        {
            break;
        }
    }
    if (res_type_wr_index == DBAL_MAX_NUMBER_OF_RESULT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_RESOURCE, "Cannot write any more dynamic result types to SW state. Table %s.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    /** Write the result fields.*/
    for (res_field_index = 0; res_field_index < table->multi_res_info[res_type_index].nof_result_fields;
         res_field_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.set
                        (unit, res_type_wr_index, res_field_index,
                         table->multi_res_info[res_type_index].results_info[res_field_index]));
    }
    /*
     * Write the multi_res_info_data, while changing the result field pointer to NULL.
     */
    multi_res_info = table->multi_res_info[res_type_index];

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.set
                    (unit, res_type_wr_index, (CONST multi_res_info_t *) & multi_res_info));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                    multi_res_info_status.set(unit, res_type_wr_index, DBAL_MULTI_RES_INFO_USED));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * read dynamic result types form sw state, only applicable for PPMC table.
 *   \param [in] unit
 *   \param [in] table_id       - the table to get info for. Only PPMC table is supported.
 *   \param [in] index          - the index within the SW state of the dynamic result types for the table.
 *   \param [in] multi_res_info - Pointer be filled with the result type info.
 *                                Includes allocating space for results_info. Expects results_info to be NULL.
 *                                if multi_res_info is NULL, then it is not filled, and the procedure is only used
 *                                to check if the index is allocated.
 * \return
 * Non-zero(shr_error_e) in case of an error. _SHR_E_NOT_FOUND if the index is not being used.
 * \par INDIRECT OUTPUT:
 *   If multi_res_info isn't NULL it is filled with the result type info.
 */
shr_error_e
dbal_tables_table_res_type_sw_state_get(
    int unit,
    dbal_tables_e table_id,
    int index,
    multi_res_info_t * multi_res_info)
{
    uint8 res_field_index;
    dbal_multi_res_info_status_e res_info_status;

    CONST multi_res_info_t *multi_res_swstate_info;

    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (index >= DBAL_MAX_NUMBER_OF_RESULT_TYPES)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get(unit, index, &res_info_status));
    if (res_info_status == DBAL_MULTI_RES_INFO_AVAIL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }
    else if (res_info_status != DBAL_MULTI_RES_INFO_USED)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "unexpected value in res_info_status of index %d: %d. Table %s.\n",
                     index, res_info_status, dbal_logical_table_to_string(unit, table_id));
    }

    if (multi_res_info != NULL)
    {
        dbal_table_field_info_t *results_info;

        results_info = multi_res_info->results_info;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                        (unit, index, &multi_res_swstate_info));
        sal_memcpy(multi_res_info, multi_res_swstate_info, sizeof(multi_res_info_t));

        multi_res_info->results_info = results_info;

        for (res_field_index = 0; res_field_index < multi_res_info->nof_result_fields; res_field_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.get
                            (unit, index, res_field_index, &(multi_res_info->results_info[res_field_index])));
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete a dynamic reult type from SW state by HW value, only applicable for PPMC table.
 *   \param [in] unit
 *   \param [in] table_id          - the table in which we delete the result type. Only PPMC table is supported.
 *   \param [in] res_type_hw_value - The HW Value of the result type that we want to delete.
 * \return
 * Non-zero(shr_error_e) in case of an error.
 */
shr_error_e
dbal_tables_table_res_type_sw_state_del(
    int unit,
    dbal_tables_e table_id,
    int res_type_hw_value)
{
    int res_type_del_index;
    int res_type_shift_index;
    int hw_value_index;
    multi_res_info_t multi_res_info;
    CONST multi_res_info_t *multi_res_swstate_info;

    dbal_multi_res_info_status_e res_info_status;
    uint8 res_field_index;
    dbal_table_field_info_t result_info;
    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_del_index = 0; res_type_del_index < DBAL_MAX_NUMBER_OF_RESULT_TYPES; res_type_del_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get
                        (unit, res_type_del_index, &res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_USED)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                            (unit, res_type_del_index, &multi_res_swstate_info));
            sal_memcpy(&multi_res_info, multi_res_swstate_info, sizeof(multi_res_info_t));

            if (multi_res_info.result_type_nof_hw_values <= 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "result_type_nof_hw_values of index %d in sw state is %d, "
                             "must be at least one. Table %s.\n",
                             res_type_del_index, multi_res_info.result_type_nof_hw_values,
                             dbal_logical_table_to_string(unit, table_id));
            }
            for (hw_value_index = 0; hw_value_index < multi_res_info.result_type_nof_hw_values; hw_value_index++)
            {
                if (res_type_hw_value == multi_res_info.result_type_hw_value[hw_value_index])
                {
                    break;
                }
            }
            if (hw_value_index < multi_res_info.result_type_nof_hw_values)
            {
                break;
            }
        }
    }
    if (res_type_del_index == DBAL_MAX_NUMBER_OF_RESULT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "trying to delete unallocated result type hw value %d\n", res_type_hw_value);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                    (unit, res_type_del_index, DBAL_MULTI_RES_INFO_AVAIL));
    /*
     * Shift all of the following result types to help avoid restoring them out of order later
     * (which is not necessarily a problamatic scenarion).
     */
    for (res_type_shift_index = res_type_del_index; res_type_shift_index < (DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1);
         res_type_shift_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.get
                        (unit, res_type_shift_index + 1, &res_info_status));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                        (unit, res_type_shift_index, res_info_status));
        if (res_info_status == DBAL_MULTI_RES_INFO_USED)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.get
                            (unit, res_type_shift_index + 1, &multi_res_swstate_info));
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.multi_res_info_data.set
                            (unit, res_type_shift_index, multi_res_swstate_info));
            for (res_field_index = 0; res_field_index < multi_res_info.nof_result_fields; res_field_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.get
                                (unit, res_type_shift_index + 1, res_field_index, &result_info));
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.set
                                (unit, res_type_shift_index, res_field_index, result_info));
            }
        }
        else if (res_info_status != DBAL_MULTI_RES_INFO_AVAIL)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type %d in SW state has unrecognized status %d Table %s.\n",
                         res_info_status, res_info_status, dbal_logical_table_to_string(unit, table_id));
        }
    }
    /*
     * Mark the last result type as available to finish the shift.
     */
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.set
                    (unit, res_type_shift_index, DBAL_MULTI_RES_INFO_AVAIL));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Delete all dynamic reult type from SW state, only applicable for PPMC table.
 *   \param [in] unit
 *   \param [in] table_id          - the table to delete dunamic result type info for. Only PPMC table is supported.
 * \return
 * Non-zero(shr_error_e) in case of an error.
 */
shr_error_e
dbal_tables_table_res_type_sw_state_delete_all(
    int unit,
    dbal_tables_e table_id)
{
    int res_type_index;
    SHR_FUNC_INIT_VARS(unit);

    if (table_id != DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Only table %s is supported. Table given %s.\n",
                     dbal_logical_table_to_string(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION),
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_index = 0; res_type_index < DBAL_MAX_NUMBER_OF_RESULT_TYPES; res_type_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                        multi_res_info_status.set(unit, res_type_index, DBAL_MULTI_RES_INFO_AVAIL));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Store dynamic table in sw state.
 * the sw_state DB structure is:
 * table_id - store the table id.
 * labels_size_bytes - holds the size of sw_state memory
 *                     allocated for table labels.
 * labels - holds the table labels information.
 * data_size_bytes -  holds the size of sw_state memory
 *                    allocated for table struct information.
 *                    this holds the static fields information
 *                    of the table.
 * data - holds the static fields information of the table.
 * key_info_data_size_bytes - holds the size of sw_state memory
 *                            allocated for key fields
 *                            information
 * key_info_data - holds the key field information of the table.
 * multi_res_info_data_size_bytes - holds the size of sw_state
 *                                  memory allocated for table
 *                                  result types information
 * multi_res_info_data - holds the result types information of
 *                       the table.
 * res_info_data_size_bytes - holds the size of sw_state memory
 *                            allocated for table result fields.
 * res_info_data - holds the result fields information of the
 *                 table.
 * access_info_size_bytes - holds the size of sw_state memory
 *                          allocated for access information
 * access_info - holds the dynamic access information of the
 *               table
 */
shr_error_e
dbal_tables_table_interface_to_sw_state_add(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    int sw_state_table_index;
    int label_index;
    int key_index;
    int data_size_bytes;
    uint8 res_field_index;
    uint8 multi_res_type_index;
    int char_index;
    char eos = '\0';

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];
    sw_state_table_index = table_id - DBAL_NOF_TABLES;

    SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.table_id.set(unit, sw_state_table_index, table_id));

    /*
     * Set dynamic table name
     */
    for (char_index = 0; char_index < sal_strlen(table->table_name); char_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.
                        table_name.set(unit, sw_state_table_index, char_index, &table->table_name[char_index]));
    }

    SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.table_name.set(unit, sw_state_table_index, char_index, &eos));

    for (label_index=0; label_index<table->nof_labels; label_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.labels.set(unit, sw_state_table_index, label_index, table->table_labels[label_index]));
    }

    data_size_bytes = sizeof(dbal_logical_table_t);
    SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.data.set(unit, sw_state_table_index, (CONST dbal_logical_table_t*) table));

    for (key_index=0; key_index<table->nof_key_fields; key_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.key_info_data.set(unit, sw_state_table_index, key_index,
                                                               (CONST dbal_table_field_info_t *) &table->keys_info[key_index]));
    }

    for (multi_res_type_index=0; multi_res_type_index<table->nof_result_types; multi_res_type_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.multi_res_info_data.set(unit, sw_state_table_index, multi_res_type_index,
                (CONST multi_res_info_t *) &table->multi_res_info[multi_res_type_index]));
    }

    for (multi_res_type_index = 0; multi_res_type_index < table->nof_result_types; multi_res_type_index++)
    {
        for (res_field_index = 0; res_field_index < table->multi_res_info[multi_res_type_index].nof_result_fields;
             res_field_index++)
        {
            SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.res_info_data.set(unit, sw_state_table_index, multi_res_type_index, res_field_index,
                    (CONST dbal_table_field_info_t *)(&table->multi_res_info[multi_res_type_index].results_info[res_field_index])));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_restore(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    dbal_logical_table_t *table_data;
    int sw_state_table_index;
    uint8 res_field_index;
    uint8 key_index;
    uint8 lable_index;
    uint8 multi_res_type_index;
    CONST char *table_name;
    dbal_labels_e *table_labels;
    dbal_table_field_info_t *keys_info;
    multi_res_info_t *multi_res_info;
    dbal_hl_access_info_t *hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    kbp_db_handles_t *kbp_handles;
#endif

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    sw_state_table_index = table_id - DBAL_NOF_TABLES;

    DBAL_TBL_IN_SW_STATE.table_name.get(unit, sw_state_table_index, 0, &table_name);
    sal_strncpy_s(table->table_name, table_name, DBAL_MAX_STRING_LENGTH);

    SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.data.get(unit, sw_state_table_index, (CONST dbal_logical_table_t **)&table_data));

    /*
     * Keep pointers before restoring structure from swstate as in sw state we have the previous freed pointers ,
     * the pointers kept in swstate are not relevant
     */
    table_labels = table->table_labels;;
    keys_info = table->keys_info;
    multi_res_info = table->multi_res_info;
    hl_mapping_multi_res = table->hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    kbp_handles = table->kbp_handles;
#endif

    sal_memcpy(table, table_data, sizeof(dbal_logical_table_t));

    /** restore new pointers after structure copy from swstate */
    table->table_labels = table_labels;
    table->keys_info = keys_info;
    table->multi_res_info = multi_res_info;
    table->hl_mapping_multi_res = hl_mapping_multi_res;
#if defined(INCLUDE_KBP)
    table->kbp_handles = kbp_handles;
#endif

    for (lable_index=0; lable_index<table->nof_labels; lable_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.labels.get(unit, sw_state_table_index, lable_index, &table->table_labels[lable_index]));
    }

    for (key_index=0; key_index<table->nof_key_fields; key_index++)
    {
        dbal_table_field_info_t *keys_info;
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.key_info_data.get(unit, sw_state_table_index, key_index ,
                (CONST dbal_table_field_info_t **) &keys_info));
        sal_memcpy(&table->keys_info[key_index], keys_info, sizeof(dbal_table_field_info_t));
    }

    if (table->nof_result_types)
    {
        for (multi_res_type_index = 0; multi_res_type_index < table->nof_result_types; multi_res_type_index++)
        {
            multi_res_info_t *multi_res_info;
            dbal_table_field_info_t *results_info;

            /*
             * Keep results_info pointer before restoring structure from swstate as in sw state we have the previous freed pointer,
             * the pointers kept in swstate are not relevant
             */
            results_info = table->multi_res_info[multi_res_type_index].results_info;

            SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.multi_res_info_data.get(unit, sw_state_table_index, multi_res_type_index,
                    (CONST multi_res_info_t **)&multi_res_info));
            sal_memcpy(&table->multi_res_info[multi_res_type_index], multi_res_info, sizeof(multi_res_info_t));

            /** restore results_info pointer after structure copy from swstate */
            table->multi_res_info[multi_res_type_index].results_info = results_info;

            for (res_field_index = 0; res_field_index < table->multi_res_info[multi_res_type_index].nof_result_fields;
                 res_field_index++)
            {
                SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.res_info_data.get(unit, sw_state_table_index,
                        multi_res_type_index, res_field_index, (CONST dbal_table_field_info_t **)&results_info));
                sal_memcpy(&table->multi_res_info[multi_res_type_index].results_info[res_field_index], results_info,
                        sizeof(dbal_table_field_info_t));
            }
        }
    }

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
        case DBAL_ACCESS_METHOD_SW_STATE:
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_PEMLA:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            break;

        case DBAL_ACCESS_METHOD_KBP:
#if defined(INCLUDE_KBP)
            SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.access_info.memread(unit, sw_state_table_index, table->kbp_handles, 0,
                                                                     sizeof(kbp_db_handles_t)));
#endif
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal access method %d \n", table->access_method);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * restore all dynamic tables from sw state memory
 *   \param [in] unit - relevant unit
 *
 * \return
 * Non-zero(shr_error_e) in case of an error
 */
static shr_error_e
dbal_tables_sw_state_restore(
    int unit)
{
    dbal_table_status_e table_status;
    dbal_tables_e table_id;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = DBAL_NOF_TABLES; table_id < nof_tables; table_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        if (table_status != DBAL_TABLE_NOT_INITIALIZED)
        {
            SHR_IF_ERR_EXIT(dbal_tables_table_restore(unit, table_id));
        }
    }

    /*
     * Re-init the group info.
     */
    SHR_IF_ERR_EXIT(dbal_tables_generic_table_xmls_info_init(unit));

    /*
     * Restore dynamic result types for tables. currently only PPMC table
     */
    SHR_IF_ERR_EXIT(dbal_tables_result_type_restore(unit, DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_create(
    int unit,
    dbal_access_method_e access_method,
    dbal_table_type_e table_type,
    dbal_core_mode_e core_mode,
    int nof_key_fields,
    dbal_table_field_input_info_t * keys_info,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *table_name,
    dbal_tables_e * table_id)
{
    dbal_logical_table_t *table = NULL;
    int iter;

    SHR_FUNC_INIT_VARS(unit);

    if ((nof_key_fields > dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit)) ||
        (nof_result_fields > dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of fields: key: req[%d]avail[%d], result: req[%d]avail[%d]\n",
                nof_key_fields, dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit),
                nof_result_fields, dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit));
    }

    SHR_IF_ERR_EXIT(dbal_tables_available_table_id_get(unit, table_id));

    table = &logical_tables_info[unit].logical_tables[(*table_id)];

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    table->maturity_level = DBAL_MATURITY_HIGH;

    if (table_name != NULL && table_name[0] != '\0')
    {
        dbal_tables_e log_table_id;
        _shr_error_t rv;
        rv = dbal_logical_table_string_to_id_no_error(unit, table_name, &log_table_id);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_EXISTS, "A table with the name \"%.*s\" already exists: %d.\n",
                         DBAL_MAX_STRING_LENGTH, table_name, log_table_id);
        }
        sal_strncpy_s(table->table_name, table_name, DBAL_MAX_STRING_LENGTH);
    }
    else
    {
        sal_snprintf(table->table_name, DBAL_MAX_STRING_LENGTH, "DYNAMIC_%d", (*table_id));
    }

    if ((access_method != DBAL_ACCESS_METHOD_MDB) && (access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Access method, Supported only Access methods MDB / KBP \n");
    }
    table->access_method = access_method;

    
    if ((table_type != DBAL_TABLE_TYPE_TCAM) && (table_type != DBAL_TABLE_TYPE_TCAM_DIRECT) &&
        (table_type != DBAL_TABLE_TYPE_EM))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal table_type, Supported only DBAL_TABLE_TYPE_TCAM, "
                     "DBAL_TABLE_TYPE_TCAM or DBAL_TABLE_TYPE_EM\n");
    }
    table->table_type = table_type;

    
    if ((table_type == DBAL_TABLE_TYPE_TCAM || table_type == DBAL_TABLE_TYPE_TCAM_DIRECT) &&
        (access_method == DBAL_ACCESS_METHOD_MDB))
    {
        /**when table type is TCAM and access method is MDB there is only one known physical_db_id that fits */
        table->physical_db_id[0] = DBAL_PHYSICAL_TABLE_TCAM;
        table->nof_physical_tables = 1;
    }

    table->core_mode = core_mode;

    if (nof_key_fields > DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of key fields %d max value %d\n", nof_key_fields,
                     DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS);
    }

    if (nof_result_fields > DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal number of result fields %d max value %d\n", nof_result_fields,
                     DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
    }
    table->nof_key_fields = nof_key_fields;

    for (iter = 0; iter < nof_key_fields; iter++)
    {
        dbal_field_types_basic_info_t *field_type_info;
        uint32 field_max_value;
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, keys_info[iter].field_id, &field_type_info));

        if (keys_info[iter].is_valid_indication_needed)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field - is_valid_indication_needed - cannot be set for a key (%s)\n",
                         dbal_field_to_string(unit, keys_info[iter].field_id));
        }

        table->keys_info[iter].field_id = keys_info[iter].field_id;
        if (keys_info[iter].field_nof_bits == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s size was set to zero. Key field with size zero is not supported.\n",
                         dbal_field_to_string(unit, keys_info[iter].field_id));
        }
        else if (keys_info[iter].field_nof_bits != DBAL_USE_DEFAULT_SIZE)
        {
            table->keys_info[iter].field_nof_bits = keys_info[iter].field_nof_bits;
        }
        else
        {
            if (field_type_info->max_size == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s size must be set or use a field type that defines a size.\n",
                             dbal_field_to_string(unit, keys_info[iter].field_id));
            }
            table->keys_info[iter].field_nof_bits = field_type_info->max_size;
        }

        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, keys_info[iter].field_id, &field_max_value));
        table->keys_info[iter].max_value =
            UTILEX_MIN(field_max_value, utilex_power_of_2(table->keys_info[iter].field_nof_bits) - 1);
        table->keys_info[iter].nof_instances = 1;
        table->key_size += table->keys_info[iter].field_nof_bits;
    }

    table->nof_result_types = 1;

    table->multi_res_info->nof_result_fields = nof_result_fields;

    if (nof_result_fields > DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "num of result fields for dynamic atble is limited to %d , requested %d\n",
                     DBAL_NOF_DYNAMIC_TABLE_RESULT_FIELDS, nof_result_fields);
    }
    for (iter = 0; iter < nof_result_fields; iter++)
    {
        uint8 is_valid_indication_needed;
        uint32 field_max_value;
        dbal_field_types_basic_info_t *field_type_info;
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, results_info[iter].field_id, &field_type_info));

        table->multi_res_info->results_info[iter].field_id = results_info[iter].field_id;

        if (results_info[iter].field_nof_bits != DBAL_USE_DEFAULT_SIZE)
        {
            table->multi_res_info->results_info[iter].field_nof_bits = results_info[iter].field_nof_bits;
        }
        else
        {
            table->multi_res_info->results_info[iter].field_nof_bits = field_type_info->max_size;
        }

        is_valid_indication_needed = results_info[iter].is_valid_indication_needed;
        table->multi_res_info->results_info[iter].is_valid_indication_needed = is_valid_indication_needed;

        /** Set max value */
        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, results_info[iter].field_id, &field_max_value));
        table->multi_res_info->results_info[iter].max_value =
            UTILEX_MIN(field_max_value,
                       utilex_power_of_2(table->multi_res_info->results_info[iter].field_nof_bits) - 1);

        if (is_valid_indication_needed)
        {
            if (table->multi_res_info->results_info[iter].field_nof_bits > 32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s : Valid indication not supported for field_nof_bits > 32\n",
                             dbal_field_to_string(unit, results_info[iter].field_id));
            }
            else
            {
                /** increasing the max field value by one to allow the valid indication bit */
                table->multi_res_info->results_info[iter].field_nof_bits += 1;
            }
        }

        table->multi_res_info->results_info[iter].nof_instances = 1;
        table->multi_res_info->entry_payload_size += table->multi_res_info->results_info[iter].field_nof_bits;
    }

    SHR_IF_ERR_EXIT(dbal_db_init_table_mutual_interface_validation(unit, *table_id, table));
    /*
     * For an MDB table, the key/result fields are ordered from MSB to LSB.
     */
    if ((access_method == DBAL_ACCESS_METHOD_MDB) || (access_method == DBAL_ACCESS_METHOD_KBP))
    {
        SHR_IF_ERR_EXIT(dbal_db_init_table_mdb_validation(unit, *table_id, table));
    }
    SHR_IF_ERR_EXIT(dbal_tables_table_interface_to_sw_state_add(unit, *table_id));
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, *table_id, DBAL_TABLE_INITIALIZED));

    /*
     * use state rollback journal to attempt to destroy created table
     */
    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_dbal_table_create(unit, *table_id));

    LOG_DEBUG_EX(BSL_LOG_MODULE,
                 "dbal_tables_table_create table_id=%d, table_type %d access_method %d %s\n ",
                 *table_id, table_type, access_method, EMPTY);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_add(
    int unit,
    dbal_tables_e table_id,
    int nof_result_fields,
    dbal_table_field_input_info_t * results_info,
    char *res_type_name,
    int result_type_size,
    int result_type_hw_value,
    int *result_type_index)
{
    dbal_logical_table_t *table = NULL;
    int new_res_type_idx = 0, result_total_size = 0, result_offset = 0;
    mdb_em_entry_encoding_e entry_encoding;
    int res_type_iter;
    int res_hw_val_iter;
    int res_field_iter;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    /** Currently, this API is to support PPMC (MC-ID) table only */
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Adding result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!table->has_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot add result type to tables without already having result type, table %s\n",
                     table->table_name);
    }

    if (table->nof_result_types == dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot add result type to tables, maximum number of result types [%d] used, table %s\n",
                             dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit),table->table_name);
    }

    if (table->multi_res_info[0].results_info[0].field_nof_bits != result_type_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Cannot add a result type with different size of RESULT_TYPE Field. New %d, Exist %d, table %s\n",
                     result_type_size, table->multi_res_info[0].results_info[0].field_nof_bits, table->table_name);
    }

    if (nof_result_fields <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type to table %s, without having RESULT_TYPE field in leading field "
                     "(nof_result_fields is %d)\n", table->table_name, nof_result_fields);
    }

    if (results_info[0].field_id != DBAL_FIELD_RESULT_TYPE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type to table %s, without having RESULT_TYPE field in leading field\n",
                     table->table_name);
    }

    if (results_info[0].is_valid_indication_needed)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type with valid bit is not supported. Table %s.\n", table->table_name);
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        for (res_hw_val_iter = 0; res_hw_val_iter < table->multi_res_info[res_type_iter].result_type_nof_hw_values;
             res_hw_val_iter++)
        {
            if (result_type_hw_value == table->multi_res_info[res_type_iter].result_type_hw_value[res_hw_val_iter])
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "New result type HW value (%d) already exists in table, in result type index %d "
                             " HW value index %d. table %s\n",
                             result_type_hw_value, res_type_iter, res_hw_val_iter, table->table_name);
            }
        }

    }
    new_res_type_idx = table->nof_result_types;
    table->nof_result_types += 1;

    table->multi_res_info[new_res_type_idx].result_type_nof_hw_values = 1;
    table->multi_res_info[new_res_type_idx].result_type_hw_value[0] = result_type_hw_value;
    if (res_type_name == NULL)
    {
        sal_snprintf(table->multi_res_info[new_res_type_idx].result_type_name, DBAL_MAX_STRING_LENGTH,
                     "DYN_RES_%d", new_res_type_idx);
    }
    else
    {
        sal_snprintf(table->multi_res_info[new_res_type_idx].result_type_name, DBAL_MAX_STRING_LENGTH,
                     "DYN_RES_%s", res_type_name);
    }
    table->multi_res_info[new_res_type_idx].nof_result_fields = nof_result_fields;

    /*
     * First find the total size of the results, to add zero LSB padding.
     * Find the size of each result field.
     */
    for (res_field_iter = 0; res_field_iter < nof_result_fields; res_field_iter++)
    {
        uint8 is_valid_indication_needed;
        uint32 field_max_value;
        dbal_field_types_basic_info_t *field_type_info;
        SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                        (unit, results_info[res_field_iter].field_id, &field_type_info));

        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_id =
            results_info[res_field_iter].field_id;

        if (results_info[res_field_iter].field_nof_bits != DBAL_USE_DEFAULT_SIZE)
        {
            table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits =
                results_info[res_field_iter].field_nof_bits;
        }
        else
        {
            table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits =
                field_type_info->max_size;
        }
        /** Set max value */
        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, results_info[res_field_iter].field_id, &field_max_value));
        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].max_value =
            UTILEX_MIN(field_max_value,
                       utilex_power_of_2(table->multi_res_info[new_res_type_idx].
                                         results_info[res_field_iter].field_nof_bits) - 1);

        is_valid_indication_needed = results_info[res_field_iter].is_valid_indication_needed;
        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].is_valid_indication_needed =
            is_valid_indication_needed;
        if (is_valid_indication_needed)
        {
            if (table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits > 32)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Field %s : Valid indication not supported for field_nof_bits > 32\n",
                             dbal_field_to_string(unit, results_info[res_field_iter].field_id));
            }
            else
            {
                /** increasing the max field value by one to allow the valid indication bit */
                table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits += 1;
            }
        }
        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].nof_instances = 1;
        result_total_size += table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits;
    }

    if (result_total_size > table->max_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Adding result type which is longer (%d bits) than all existing types (%d bits) is not supported. table %s\n",
                     result_total_size, table->max_payload_size, table->table_name);
    }
    /** Find the buffer offset of each result field other than the result type.*/
    table->multi_res_info[new_res_type_idx].zero_padding = table->max_payload_size - result_total_size;
    result_offset = table->multi_res_info[new_res_type_idx].zero_padding;
    table->multi_res_info[new_res_type_idx].entry_payload_size = result_total_size;
    for (res_field_iter = nof_result_fields - 1; res_field_iter >= 0; res_field_iter--)
    {
        table->multi_res_info[new_res_type_idx].results_info[res_field_iter].bits_offset_in_buffer = result_offset;
        result_offset += table->multi_res_info[new_res_type_idx].results_info[res_field_iter].field_nof_bits;
    }
    /** Note: This is currently served only the PPMC (MC-ID) allocations */
    SHR_IF_ERR_EXIT(mdb_em_get_min_entry_encoding(unit, DBAL_PHYSICAL_TABLE_PPMC, table->key_size,
                                                  table->multi_res_info[new_res_type_idx].entry_payload_size, 0, 0, 0,
                                                  &entry_encoding));
    SHR_IF_ERR_EXIT(mdb_set_vmv_size_value
                    (unit, DBAL_PHYSICAL_TABLE_PPMC, entry_encoding, 6,
                     table->multi_res_info[new_res_type_idx].result_type_hw_value[0]));

    *result_type_index = new_res_type_idx;

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_add(unit, table_id, new_res_type_idx));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * read dynamic result types form sw state, only applicable for
 * PPMC table
 */
shr_error_e
dbal_tables_result_type_restore(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    uint8 multi_res_type_index;
    uint8 multi_res_type_index_sw;
    int nof_dynamic_res_types = 0;
    int nof_ppmc_multi_result_types;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    /** Currently, this API is to support PPMC (MC-ID) table only */
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    /*
     * Count the number of dynamic result types.
     */
    nof_dynamic_res_types = 0;
    for (multi_res_type_index_sw = 0; multi_res_type_index_sw < DBAL_MAX_NUMBER_OF_RESULT_TYPES;
         multi_res_type_index_sw++)
    {
        rv = dbal_tables_table_res_type_sw_state_get(unit, table_id, nof_dynamic_res_types, NULL);
        if (rv == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }
        nof_dynamic_res_types++;
    }

    /*
     * restore dynamic result types
     */

    if (nof_dynamic_res_types > 0)
    {
        nof_ppmc_multi_result_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
        table = &logical_tables_info[unit].logical_tables[table_id];

        multi_res_type_index_sw = 0;
        for (multi_res_type_index = table->nof_result_types;
             multi_res_type_index < (table->nof_result_types + nof_dynamic_res_types); multi_res_type_index++)
        {
            for (; multi_res_type_index_sw < nof_ppmc_multi_result_types; multi_res_type_index_sw++)
            {
                rv = dbal_tables_table_res_type_sw_state_get(unit, table_id, multi_res_type_index_sw,
                                                             &table->multi_res_info[multi_res_type_index]);
                if (rv == _SHR_E_NOT_FOUND)
                {
                    continue;
                }
                else
                {
                    SHR_IF_ERR_EXIT(rv);
                    multi_res_type_index_sw++;
                    break;
                }
            }
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reached result type index %d, number of static result types %d, "
                             "number of dynamic result types %d, but could not find corresponding result type in "
                             "SW state.\n", multi_res_type_index, table->nof_result_types, nof_dynamic_res_types);
            }
        }
    }

    table->nof_result_types += nof_dynamic_res_types;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_destroy(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if (table_status == DBAL_TABLE_NOT_INITIALIZED)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table not exists\n");
    }

    /*
     * use state journal to rollback the dbal table destroy in case of an error
     */
    SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_dbal_table_destroy(unit, table_id));

    SHR_IF_ERR_EXIT(dbal_tables_dbal_logical_table_t_clear(unit, table));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_NOT_INITIALIZED));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_dynamic_result_type_delete(
    int unit,
    dbal_tables_e table_id,
    int result_type_index)
{
    dbal_logical_table_t *table = NULL;
    dbal_table_field_info_t *results_info = NULL;
    uint32 del_res_type_hw_value;

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    /** Currently, this API is to support PPMC (MC-ID) table only */
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!table->has_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy result type in table without RESULT_TYPE, table %s\n",
                     table->table_name);
    }

    /** Verify that the index to delete is the really dynamic one */
    if (sal_strstr(table->multi_res_info[result_type_index].result_type_name, "DYN_RES_") == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Result type index, %d, is not a dynamic result type. result name=%s. table %s\n",
                     result_type_index, table->multi_res_info[result_type_index].result_type_name, table->table_name);
    }


    del_res_type_hw_value = table->multi_res_info[result_type_index].result_type_hw_value[0];

    /** Keep deleted result type result info ptr*/
    results_info = table->multi_res_info[result_type_index].results_info;
    /** clear deleted result type result info */
    sal_memset(results_info, 0,
               dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t));

    if (result_type_index == table->nof_result_types - 1)
    {
        /** it is the last result type, just clear it */
        sal_memset(&table->multi_res_info[result_type_index], 0,  sizeof(multi_res_info_t));
        table->multi_res_info[result_type_index].results_info = results_info;
    }
    else
    {
        int nof_multi_res_types = table->nof_result_types;
        /* Move last result type to this location */
        sal_memcpy(&table->multi_res_info[result_type_index], &table->multi_res_info[nof_multi_res_types - 1],
                   sizeof(multi_res_info_t));
        sal_memset(&table->multi_res_info[nof_multi_res_types - 1], 0,  sizeof(multi_res_info_t));
                table->multi_res_info[nof_multi_res_types - 1].results_info = results_info;

    }
    table->nof_result_types -= 1;
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                            multi_res_info_status.set(unit, table->nof_result_types, DBAL_MULTI_RES_INFO_AVAIL));

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_del(unit, table_id, del_res_type_hw_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_dynamic_result_type_destroy_all(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = NULL;
    int iter, res_type_index;
    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    /** Currently, this API is to support PPMC (MC-ID) table only */
    if (table->physical_db_id[0] != DBAL_PHYSICAL_TABLE_PPMC)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Currently, this API is to support PPMC (MC-ID) table only, table %s \n",
                     table->table_name);
    }

    if (table->access_method != DBAL_ACCESS_METHOD_MDB)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic result type is available for MDB only, table %s\n", table->table_name);
    }

    if (!table->has_result_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot destroy result type in table without RESULT_TYPE, table %s\n",
                     table->table_name);
    }

    /** Find the first result which is dynamic */
    for (iter = 0; iter < table->nof_result_types; iter++)
    {
        if (sal_strstr(table->multi_res_info[iter].result_type_name, "DYN_RES_") != NULL)
        {
            break;
        }
    }
    if (iter == table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_EMPTY, "table %s does not contain any dynamic result type, Illegal destroy\n",
                     table->table_name);
    }
    if (iter == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s must have  at least on static result type.\n", table->table_name);
    }

    for (res_type_index = iter; res_type_index < table->nof_result_types; res_type_index++)
    {
        dbal_table_field_info_t *results_info;

        results_info = table->multi_res_info[table->nof_result_types].results_info;

        sal_memset(&table->multi_res_info[table->nof_result_types], 0,  sizeof(multi_res_info_t));
        table->multi_res_info[table->nof_result_types].results_info = results_info;

        sal_memset(table->multi_res_info[table->nof_result_types].results_info, 0,
                dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t));
    }

    table->nof_result_types = iter;

    SHR_IF_ERR_EXIT(dbal_tables_table_res_type_sw_state_delete_all(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t ** table)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    (*table) = &logical_tables_info[unit].logical_tables[table_id];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_nof_res_type_get(
    int unit,
    dbal_tables_e table_id,
    int *nof_res_type)
{
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];

    (*nof_res_type) = table->nof_result_types;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_get(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t ** table)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    (*table) = &logical_tables_info[unit].logical_tables[table_id];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_is_key_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 *is_key)
{
    int field_index;
    dbal_logical_table_t *table;
    uint8 is_sub_field_found = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];
    *is_key = FALSE;

    for (field_index = 0; field_index < table->nof_key_fields; field_index++)
    {
        if (table->keys_info[field_index].field_id == field_id)
        {
            *is_key = TRUE;
            break;
        }
        SHR_IF_ERR_EXIT(dbal_fields_sub_field_match(unit, table->keys_info[field_index].field_id,
                                                    field_id, &is_sub_field_found));
        if (is_sub_field_found)
        {
            *is_key = TRUE;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get_internal(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t ** field_info,
    int *field_index_in_table,
    dbal_fields_e * parent_field_id)
{
    int iter;
    int nof_fields;
    dbal_logical_table_t *table;
    dbal_table_field_info_t *fields_db;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &logical_tables_info[unit].logical_tables[table_id];

    (*field_info) = NULL;
    (*parent_field_id) = DBAL_FIELD_EMPTY;
    (*field_index_in_table) = -1;

    if (is_key)
    {
        nof_fields = table->nof_key_fields;
        fields_db = table->keys_info;
    }
    else
    {
        nof_fields = table->multi_res_info[result_type_idx].nof_result_fields;
        fields_db = table->multi_res_info[result_type_idx].results_info;
    }

    if (inst_idx == INST_SINGLE)
    {
        inst_idx = 0;
    }

    for (iter = 0; iter < nof_fields; iter++)
    {
        uint8 is_sub_field_found = FALSE;

        if (fields_db[iter].field_id == field_id)
        {
            if (inst_idx >= fields_db[iter].nof_instances)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Instance idx is out of range. inst=%d, field %s, table %s\n", inst_idx,
                             dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id));
            }
            *field_info = &(fields_db[iter + inst_idx]);
            (*field_index_in_table) = iter + inst_idx;
            field_id += inst_idx;
            break;
        }

        SHR_IF_ERR_EXIT(dbal_fields_sub_field_match(unit, fields_db[iter].field_id, field_id, &is_sub_field_found));
        if (is_sub_field_found)
        {
            if (inst_idx >= fields_db[iter].nof_instances)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Instance id is out of range for child field. inst=%d, field %s, table %s\n",
                             inst_idx, dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit,
                                                                                                          table_id));
            }
            (*parent_field_id) = fields_db[iter + inst_idx].field_id;
            (*field_index_in_table) = iter + inst_idx;
            *field_info = &(fields_db[iter + inst_idx]);
            break;
        }
    }

    LOG_DEBUG(BSL_LOG_MODULE,
              (BSL_META_U(unit, "field_id found %s, parent field %s"), dbal_field_to_string(unit, field_id),
               dbal_field_to_string(unit, (*parent_field_id))));

    if (!(*field_info))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "field %s not found in table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_info) = (*field_info_p);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_size_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error in field %s table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_size) = field_info_p->field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_max_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_size)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv != _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error in field %s table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        if (field_info_p->field_nof_bits > 32)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "cannot receive max value for field bigger than 32 bit field %s table %s \n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id));
        }
        (*field_size) = field_info_p->max_value;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_hw_value_result_type_get(
    int unit,
    dbal_tables_e table_id,
    uint32 hw_value,
    int *result_type_idx)
{
    int res_type_iter;
    int hw_value_iter;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit].logical_tables[table_id]);
    if (table->has_result_type == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s has doesn't have result types.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        /** skip disabled result type    */
        if (table->multi_res_info[res_type_iter].is_disabled)
        {
            continue;
        }
        for (hw_value_iter = 0; hw_value_iter < table->multi_res_info[res_type_iter].result_type_nof_hw_values;
             hw_value_iter++)
        {
            if (table->multi_res_info[res_type_iter].result_type_hw_value[hw_value_iter] == hw_value)
            {
                /** result type found  */
                (*result_type_idx) = res_type_iter;
                SHR_EXIT();
            }
        }
    }

    /** No result type found with the HW value.*/
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_by_name_get(
    int unit,
    dbal_tables_e table_id,
    char *result_type_name,
    int *result_type_idx)
{
    int res_type_iter;
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit].logical_tables[table_id]);
    if (table->has_result_type == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "table %s has doesn't have result types.\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    for (res_type_iter = 0; res_type_iter < table->nof_result_types; res_type_iter++)
    {
        if (sal_strncmp(table->multi_res_info[res_type_iter].result_type_name, result_type_name, DBAL_MAX_STRING_LENGTH)
            == 0)
        {
            /** result type found  */
            (*result_type_idx) = res_type_iter;
            SHR_EXIT();
        }
    }

    /** No result type found with the HW value.*/
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_offset_in_hw_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    int *field_offset)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    SHR_SET_CURRENT_ERR(rv);
    if (rv)
    {
        if (rv == _SHR_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "field %s not found in table %s res type %d as %s field\n",
                         dbal_field_to_string(unit, field_id), dbal_logical_table_to_string(unit, table_id),
                         result_type_idx, is_key ? "key" : "result");
        }
    }
    else
    {
        (*field_offset) = field_info_p->bits_offset_in_buffer -
            logical_tables_info[unit].logical_tables[table_id].multi_res_info[result_type_idx].zero_padding;
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_tables_next_table_get(
    int unit,
    dbal_tables_e table_id,
    dbal_labels_e label,
    dbal_access_method_e access_method,
    dbal_physical_tables_e mdb_physical_db,
    dbal_table_type_e table_type,
    dbal_tables_e * next_table_id)
{
    dbal_tables_e table_counter;
    int jj;
    dbal_logical_table_t *table = NULL;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if ((mdb_physical_db != DBAL_PHYSICAL_TABLE_NONE) && (access_method != DBAL_ACCESS_METHOD_MDB))
    {
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "if access_method is not MDB, mdb_physical_db must be NONE")}

    (*next_table_id) = DBAL_TABLE_EMPTY;

    for (table_counter = table_id + 1; table_counter < nof_tables; table_counter++)
    {
        dbal_table_status_e table_status;

        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_counter, &table_status));
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_counter, &table));

        if ((table_status == DBAL_TABLE_NOT_INITIALIZED))
        {
            continue;
        }

        if (table->maturity_level == DBAL_MATURITY_LOW)
        {
            continue;
        }

        if (table->mdb_image_type == DBAL_MDB_IMG_STD_1_NOT_ACTIVE)
        {
            continue;
        }

        if (label != DBAL_LABEL_NONE)
        {
            for (jj = 0; jj < table->nof_labels; jj++)
            {
                if ((table->table_labels[jj] == label))
                {
                    break;
                }
            }
            if (jj == table->nof_labels)
            {
                continue;
            }
        }

        if ((access_method != DBAL_NOF_ACCESS_METHODS) && (table->access_method != access_method))
        {
            continue;
        }

        if ((table_type != DBAL_TABLE_TYPE_NONE) && (table->table_type != table_type))
        {
            continue;
        }

        if ((mdb_physical_db != DBAL_PHYSICAL_TABLE_NONE) && (table->physical_db_id[0] != mdb_physical_db))
        {
            continue;
        }

        (*next_table_id) = table_counter;
        break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_info_get_no_err(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_table_field_info_t * field_info)
{
    int field_index_in_table;
    dbal_fields_e parent_field_id;
    dbal_table_field_info_t *field_info_p;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_tables_field_info_get_internal(unit, table_id, field_id, is_key, result_type_idx, inst_idx,
                                             &field_info_p, &field_index_in_table, &parent_field_id);
    if (rv)
    {
        SHR_SET_CURRENT_ERR(rv);
    }
    else
    {
        (*field_info) = (*field_info_p);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_table_type_get(
    int unit,
    dbal_tables_e table_id,
    dbal_table_type_e * table_type)
{

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    *table_type = logical_tables_info[unit].logical_tables[table_id].table_type;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_app_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 app_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (logical_tables_info[unit].logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_KBP)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "app DB get is not applicable for table %s\n",
                     logical_tables_info[unit].logical_tables[table_id].table_name);
    }
    logical_tables_info[unit].logical_tables[table_id].app_id = app_db_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_app_db_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *app_db_id,
    int *app_db_size)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if ((logical_tables_info[unit].logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_MDB) &&
        (logical_tables_info[unit].logical_tables[table_id].access_method != DBAL_ACCESS_METHOD_KBP))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "app DB get is not applicable for table %s\n",
                     logical_tables_info[unit].logical_tables[table_id].table_name);
    }
    *app_db_id = logical_tables_info[unit].logical_tables[table_id].app_id;
    *app_db_size = logical_tables_info[unit].logical_tables[table_id].app_id_size;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_core_mode_get(
    int unit,
    dbal_tables_e table_id,
    dbal_core_mode_e * core_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    *core_mode = logical_tables_info[unit].logical_tables[table_id].core_mode;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_result_type_hw_value_get(
    int unit,
    dbal_tables_e table_id,
    int result_type_idx,
    uint32 *hw_value)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);
    if (logical_tables_info[unit].logical_tables[table_id].nof_result_types <= result_type_idx)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Result type index=%d is not existed in table %s. Nof result types is %d\n",
                     result_type_idx, logical_tables_info[unit].logical_tables[table_id].table_name,
                     logical_tables_info[unit].logical_tables[table_id].nof_result_types);
    }
    *hw_value =
        logical_tables_info[unit].logical_tables[table_id].multi_res_info[result_type_idx].result_type_hw_value[0];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_capacity_get(
    int unit,
    dbal_tables_e table_id,
    int *max_capacity)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    table = &(logical_tables_info[unit].logical_tables[table_id]);

    *max_capacity = table->max_capacity;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_tcam_handler_id_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *tcam_handler_id)
{
    uint32 tcam_han_id;
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if ((logical_tables_info[unit].logical_tables[table_id].table_type != DBAL_TABLE_TYPE_TCAM) &&
        (logical_tables_info[unit].logical_tables[table_id].table_type != DBAL_TABLE_TYPE_TCAM_DIRECT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s is not a TCAM table and the tcam_handler_id is invalid for it\n",
                     logical_tables_info[unit].logical_tables[table_id].table_name);
    }
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.get(unit, table_id, &tcam_han_id));
    *tcam_handler_id = tcam_han_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_tcam_handler_id_set(
    int unit,
    dbal_tables_e table_id,
    uint32 tcam_handler_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if ((logical_tables_info[unit].logical_tables[table_id].table_type != DBAL_TABLE_TYPE_TCAM) &&
        (logical_tables_info[unit].logical_tables[table_id].table_type != DBAL_TABLE_TYPE_TCAM_DIRECT))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s is not a TCAM table and the tcam_handler_id is invalid for it\n",
                     logical_tables_info[unit].logical_tables[table_id].table_name);
    }

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.tcam_handler_id.set(unit, table_id, tcam_handler_id));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_max_key_value_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer)
{
    int ii;
    uint32 key_buffer_local[DBAL_PHYSICAL_KEY_SIZE_IN_WORDS] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (BITS2BYTES(logical_tables_info[unit].logical_tables[table_id].key_size) > 4)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key size for table %s too long for usage %d\n",
                     dbal_logical_table_to_string(unit, table_id),
                     BITS2BYTES(logical_tables_info[unit].logical_tables[table_id].key_size));
    }
    sal_memset(key_buffer, 0, BITS2BYTES(logical_tables_info[unit].logical_tables[table_id].key_size));

    for (ii = 0; ii < logical_tables_info[unit].logical_tables[table_id].nof_key_fields; ii++)
    {
        dbal_field_print_type_e print_type;

        uint32 key_field_max_val = logical_tables_info[unit].logical_tables[table_id].keys_info[ii].max_value;
        int key_field_offset = logical_tables_info[unit].logical_tables[table_id].keys_info[ii].bits_offset_in_buffer;
        int key_field_length = logical_tables_info[unit].logical_tables[table_id].keys_info[ii].field_nof_bits;

        if (logical_tables_info[unit].logical_tables[table_id].keys_info[ii].field_id == DBAL_FIELD_CORE_ID)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit,
                                                   logical_tables_info[unit].logical_tables[table_id].
                                                   keys_info[ii].field_id, &print_type));

        if (print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
        {
            key_field_max_val = UTILEX_U32_MAX;
        }

        if (key_field_offset + key_field_length > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key size for table %s too long for usage \n",
                         dbal_logical_table_to_string(unit, table_id));
        }
        SHR_IF_ERR_EXIT(utilex_bitstream_set_field
                        (key_buffer_local, key_field_offset, key_field_length, key_field_max_val));
    }

    (*key_buffer) = key_buffer_local[0];

exit:
    SHR_FUNC_EXIT;
}

/**************************************************General usage APIs (outside of dbal *************************************************/
shr_error_e
dbal_tables_field_predefine_value_update(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    int is_key,
    int result_type,
    int instance_id,
    dbal_field_predefine_value_type_e value_type,
    uint32 predef_value)
{
    int nof_fields, iter;
    dbal_logical_table_t *table;
    dbal_table_field_info_t *fields_info;

    SHR_FUNC_INIT_VARS(unit);

    table = &logical_tables_info[unit].logical_tables[table_id];

    if (result_type > table->nof_result_types)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "illegal result_type %d, max value %d\n", result_type, table->nof_result_types);
    }
    if (instance_id == INST_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "instance all not supported\n");
    }

    if (instance_id == INST_SINGLE)
    {
        instance_id = 0;
    }

    field_id = field_id + instance_id;

    if (is_key)
    {
        nof_fields = table->nof_key_fields;
        fields_info = table->keys_info;
    }
    else
    {
        nof_fields = table->multi_res_info[result_type].nof_result_fields;
        fields_info = table->multi_res_info[result_type].results_info;
    }

    for (iter = 0; iter < nof_fields; iter++)
    {
        if (fields_info[iter].field_id == field_id)
        {
            break;
        }
    }

    if (iter == nof_fields)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "field not found %s\n", dbal_field_to_string(unit, field_id));
    }
    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            if (predef_value > fields_info[iter].max_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "min value bigger then max value %d\n", fields_info[iter].max_value);
            }
            fields_info[iter].min_value = predef_value;
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            if (predef_value < fields_info[iter].min_value)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "max value lower then min value %d\n", fields_info[iter].min_value);
            }

            fields_info[iter].max_value = predef_value;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "illegal predef type %d\n", value_type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_nof_instance_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int res_type,
    int *nof_inst)
{
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, is_key, res_type, 0, &field_info));
    *nof_inst = field_info.nof_instances;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (physical_tbl_index >= logical_tables_info[unit].logical_tables[table_id].nof_physical_tables)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "physical_tbl_index (%d) is out of range. max for %s is %d\n",
                     physical_tbl_index, logical_tables_info[unit].logical_tables[table_id].table_name,
                     logical_tables_info[unit].logical_tables[table_id].nof_physical_tables);
    }
    *physical_table_id = logical_tables_info[unit].logical_tables[table_id].physical_db_id[physical_tbl_index];

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_entry_counter_update(
    int unit,
    dbal_entry_handle_t * entry_handle,
    int num_of_entries_to_update,
    uint8 is_add_operation)
{
    int jj;
    dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    table = entry_handle->table;

    if (dbal_tables_is_non_direct(unit, entry_handle->table_id) == FALSE)
    {
        SHR_EXIT();
    }

     /** update the number of entries for the physical layer for MDB only */
    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
    {
        dbal_physical_table_def_t *PhysicalTable;
        for (jj = 0; jj < table->nof_physical_tables; jj++)
        {
            if ((entry_handle->core_id != DBAL_CORE_ALL) && (jj != entry_handle->core_id))
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table->physical_db_id[jj], &PhysicalTable));

            if (num_of_entries_to_update < 0)
            {
                PhysicalTable->nof_entries = 0;
            }
            else if (is_add_operation)
            {
                PhysicalTable->nof_entries += num_of_entries_to_update;
            }
            else
            {
                PhysicalTable->nof_entries -= num_of_entries_to_update;
            }
        }
    }

    /**negativ number set to Zero (means that the operation is table clear)*/
    if (num_of_entries_to_update < 0)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.set(unit, entry_handle->table_id, 0));
    }
    else if (is_add_operation)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.inc(unit, entry_handle->table_id, num_of_entries_to_update));
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.nof_entries.dec(unit, entry_handle->table_id, num_of_entries_to_update));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_payload_size_get(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int *p_size)
{
    SHR_FUNC_INIT_VARS(unit);

    DBAL_TABLE_ID_VALIDATE(table_id);

    if (res_type_idx < logical_tables_info[unit].logical_tables[table_id].nof_result_types)
    {
        *p_size = logical_tables_info[unit].logical_tables[table_id].multi_res_info[res_type_idx].entry_payload_size;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal result type index %d for table %s. num of result types is %d\n",
                     res_type_idx, logical_tables_info[unit].logical_tables[table_id].table_name,
                     logical_tables_info[unit].logical_tables[table_id].nof_result_types);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_field_predefine_value_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint8 is_key,
    int result_type_idx,
    int inst_idx,
    dbal_field_predefine_value_type_e value_type,
    uint32 *predef_value)
{
    dbal_table_field_info_t table_field_info;
    dbal_field_types_basic_info_t *field_type_info;
    dbal_logical_table_t * table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_type_info));
    SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, field_id, is_key,
                                               result_type_idx, inst_idx, &table_field_info));
    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    switch (value_type)
    {
        case DBAL_PREDEF_VAL_MIN_VALUE:
            (*predef_value) = table_field_info.min_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Min value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }

                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));
            }
            break;

        case DBAL_PREDEF_VAL_MAX_VALUE:
            (*predef_value) = table_field_info.max_value;
            if (table_field_info.arr_prefix_size != 0)
            {
                if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Max value of Enum fields with ARR prefix cannot be resolved. field: %s",
                                 dbal_field_to_string(unit, table_field_info.field_id));
                }

                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                          table_field_info.field_nof_bits,
                                                          table_field_info.field_nof_bits +
                                                          table_field_info.arr_prefix_size, predef_value));
            }
            break;

        case DBAL_PREDEF_VAL_DEFAULT_VALUE:

            if (field_type_info->is_default_value_valid)
            {
                (*predef_value) = field_type_info->default_value;
            }
            else
            {
                if (field_type_info->max_size <= 32)
                {
                    if(table_field_info.const_value_valid)
                    {
                        *predef_value = table_field_info.const_value;
                    }
                    else
                    {
                        (*predef_value) = 0;
                        if((!is_key) && (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Default value for HL table result fields might be depended by the key value."
                                         " Need to use: dbal_fields_predefine_value_get, table %s field %s\n",
                                         table->table_name,
                                         dbal_field_to_string(unit, field_id));
                        }

                        if (table_field_info.arr_prefix_size != 0)
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, predef_value, table_field_info.arr_prefix,
                                                                      table_field_info.field_nof_bits,
                                                                      table_field_info.field_nof_bits +
                                                                      table_field_info.arr_prefix_size, predef_value));

                            if (field_type_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                            {
                                int ii;
                                for (ii = 0; ii < field_type_info->nof_enum_values; ii++)
                                {
                                    if ((*predef_value) == field_type_info->enum_val_info[ii].value)
                                    {
                                        (*predef_value) = ii;
                                        break;
                                    }
                                }

                                if(ii == field_type_info->nof_enum_values)
                                {
                                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Enum with ARR prefix, default value couldn't be resolved. field: %s",
                                            dbal_field_to_string(unit, table_field_info.field_id));
                                }
                            }
                        }
                    }
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "default value is not supported for fields bigger than 32bits: %s",
                            dbal_field_to_string(unit, table_field_info.field_id));

                }
            }
            break;

        case DBAL_PREDEF_VAL_RESET_VALUE:
            
            (*predef_value) = 0;
            break;

        default:
            /*
             * set error in handle and exit
             */
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

uint8
dbal_tables_is_non_direct(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = &logical_tables_info[unit].logical_tables[table_id];

    return ((table->table_type != DBAL_TABLE_TYPE_DIRECT) && (table->table_type != DBAL_TABLE_TYPE_TCAM_DIRECT));
}

uint8
dbal_tables_is_merge_entries_supported(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table = &logical_tables_info[unit].logical_tables[table_id];

    if ((table->table_type == DBAL_TABLE_TYPE_DIRECT) &&
        ((table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC) || (table->access_method == DBAL_ACCESS_METHOD_PEMLA)))
    {
        if ((table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC) && table->sw_payload_length_bytes > 0)
        {
            return 1;
        }
        else if ((table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
                 && (table->hl_mapping_multi_res->l2p_hl_info[DBAL_HL_ACCESS_REGISTER].is_packed_fields
                     || table->hl_mapping_multi_res->l2p_hl_info[DBAL_HL_ACCESS_MEMORY].is_packed_fields))
        {
            return 1;
        }
        return 0;
    }

    return 1;
}

shr_error_e
dbal_tables_update_hw_error(
    int unit,
    dbal_tables_e table_id)
{
    dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_id, &table));

    logical_tables_info[unit].nof_tables_with_error++;

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit, "DBAL table %s has HW issue HW elements are invalid \n"), table->table_name));
    /*
     * The table status will be set to DBAL_TABLE_HW_ERROR during init.
     * We do not need to set it again during WB.
     */
    if (!SOC_WARM_BOOT(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_HW_ERROR));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_tables_nof_hw_error_tables_get(
    int unit,
    int *not_tables)
{
    SHR_FUNC_INIT_VARS(unit);

    (*not_tables) = logical_tables_info[unit].nof_tables_with_error;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_field_printable_string_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 buffer_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 buffer_mask[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int result_type_idx,
    uint8 is_key,
    char buffer_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE])
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_in_table_printable_string_get(unit, field_id, table_id, buffer_val, buffer_mask,
                                                             result_type_idx, is_key, 0, buffer_to_print));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_printable_entry_get(
    int unit,
    dbal_tables_e table_id,
    uint32 *key_buffer,
    int core_id,
    int payload_size,
    uint32 *payload_buffer,
    dbal_printable_entry_t * entry_print_info)
{
    int rv, ii;
    uint32 entry_handle_id;
    dbal_logical_table_t *table;
    dbal_entry_handle_t *entry_handle;
    char field_value_to_print[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    table = entry_handle->table;

    rv = dbal_entry_handle_key_payload_update(unit, entry_handle_id, key_buffer, core_id, payload_size, payload_buffer);

    if ((rv == _SHR_E_NOT_FOUND) || (rv == _SHR_E_NONE))
    {
        if (key_buffer)
        {
                        /** Not found that the payload buffer wasn't match to any of the result types of the table
			 * Key fields can still be parsed */
                        /** Key parsing */
            entry_print_info->nof_key_fields = 0;
            for (ii = 0; ii < table->nof_key_fields; ii++)
            {
                int parsed_key_idx = entry_print_info->nof_key_fields;
                dbal_fields_e cur_field_id = table->keys_info[ii].field_id;

                sal_memset(field_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);

                if (cur_field_id == DBAL_FIELD_CORE_ID)
                {
                    continue;
                }
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, cur_field_id, field_value));
                SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                                (unit, table_id, cur_field_id, field_value, NULL, 0, TRUE, field_value_to_print));

                entry_print_info->key_fields_info[parsed_key_idx].field_id = cur_field_id;
                sal_strncpy_s(entry_print_info->key_fields_info[parsed_key_idx].field_name,
                              dbal_field_to_string(unit, cur_field_id), DBAL_MAX_LONG_STRING_LENGTH);
                sal_memcpy(entry_print_info->key_fields_info[parsed_key_idx].field_value, field_value,
                           DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
                sal_strncpy_s(entry_print_info->key_fields_info[parsed_key_idx].field_print_value, field_value_to_print,
                              DBAL_MAX_PRINTABLE_BUFFER_SIZE);
                entry_print_info->nof_key_fields++;
            }
            if (rv == _SHR_E_NOT_FOUND)
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
                SHR_EXIT();
            }
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(rv);
    }

    /** result parsing */
    entry_print_info->nof_res_fields = 0;
    for (ii = 0; ii < DBAL_RES_INFO.nof_result_fields; ii++)
    {
        int parsed_res_idx = entry_print_info->nof_res_fields;
        dbal_fields_e cur_field_id = DBAL_RES_INFO.results_info[ii].field_id;

        sal_memset(field_value, 0, DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, cur_field_id, INST_SINGLE, field_value));
        SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get
                        (unit, table_id, cur_field_id, field_value, NULL, entry_handle->cur_res_type, FALSE,
                         field_value_to_print));

        entry_print_info->res_fields_info[parsed_res_idx].field_id = cur_field_id;
        sal_strncpy_s(entry_print_info->res_fields_info[parsed_res_idx].field_name,
                      dbal_field_to_string(unit, cur_field_id), DBAL_MAX_LONG_STRING_LENGTH);
        sal_memcpy(entry_print_info->res_fields_info[parsed_res_idx].field_value, field_value,
                   DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
        sal_strncpy_s(entry_print_info->res_fields_info[parsed_res_idx].field_print_value, field_value_to_print,
                      DBAL_MAX_PRINTABLE_BUFFER_SIZE);
        entry_print_info->nof_res_fields++;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
