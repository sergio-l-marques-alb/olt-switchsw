/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/** \file lif_table_mngr_lib.c
 *
 *  Lif table allocations and HW writes.
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
#include <shared/utilex/utilex_os_interface.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_table_mngr.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lif.h>
#include <bcm_int/dnx/lif/lif_table_mngr_lib.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include "lif_lib_internal.h"

/*
 * }
 */
/*************
 * DEFINES   *
 *************/
/*
 * {
 */

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
 * List of all fields whose value is set by api outside lif table manager
 */
#define LIF_TABLE_MNGR_DO_NOT_SET_FIELD(_field, _dbal_table_id) \
       ((_field==DBAL_FIELD_RESULT_TYPE) || (_field==DBAL_FIELD_GLOB_OUT_LIF) || \
        ((_dbal_table_id != DBAL_TABLE_EEDB_RIF_BASIC) && (_field==DBAL_FIELD_STAT_OBJECT_CMD)) || \
        ((_dbal_table_id != DBAL_TABLE_EEDB_RIF_BASIC) && (_field==DBAL_FIELD_STAT_OBJECT_ID)) || \
        (_field==DBAL_FIELD_OAM_LIF_SET) || (_field==DBAL_FIELD_DESTINATION))
/*
 * List of all fields whose value is set by api outside lif table manager,
 * BUT if unset they need to be replaced by default values
 */
#define LIF_TABLE_MNGR_IF_UNSET_RESET_FIELD(_field) \
       ((_field==DBAL_FIELD_STAT_OBJECT_ID) || (_field==DBAL_FIELD_STAT_OBJECT_CMD))
/*
 * List of all fields whose value CAN BE set by api outside lif table manager, but not always.
 * If set inside lif api it will be specified on the handle id.
 */
#define LIF_TABLE_MNGR_DO_NOT_SET_FIELD_UNLESS_EXPLICITLY_SPECIFIED(_field) \
       (_field==DBAL_FIELD_ESEM_NAME_SPACE)
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
 * Declaration of extern functions
 * {
 */
extern shr_error_e dbal_entry_handle_update_field_ids(
    int unit,
    uint32 entry_handle_id);
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
 * \brief - Utility to set lif table keys according to ingress/egress and dbal table id indications
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] result_type - result type, if -1 ignore
 *   \param [in] local_lif_id - local lif id key
 *   \param [in] lif_table_entry_handle_id - handle id to set key on
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_lif_table_key_fields(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    int local_lif_id,
    uint32 lif_table_entry_handle_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
        {
            dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_OUT_RIF, local_lif_id);
        }
        else
        {
            dbal_entry_key_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_OUT_LIF, local_lif_id);
        }
    }

    if (result_type != -1)
    {
        dbal_entry_value_field32_set(unit, lif_table_entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new entry into lif table, copy contents from existing lif
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] src_result_type - result type of the original entry
 *   \param [in] src_local_lif_id - local lif to copy from
 *   \param [in] dst_local_lif_id - new local lif to create, copy contents of src_local_lif_id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_copy_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 src_result_type,
    int src_local_lif_id,
    int dst_local_lif_id)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    /*
     * Get source lif entry
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, src_result_type, src_local_lif_id,
                     lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));
    /*
     * Change to new key
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, -1, dst_local_lif_id, lif_table_entry_handle_id));
    /*
     * Update fields and write back to lif table
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, lif_table_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new entry into lif table, according to given handle id fields
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] local_lif_id - new entry local lif
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    int local_lif_id)
{
    uint32 lif_table_entry_handle_id;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all the fields and fill the table */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        /** Check if field exists on entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** Field exists: Copy the field value from the superset handle unless not required */
            if (!LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id))
            {
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
        else
        {
            /** Field does not exist: set to default unless no update required */
            if ((!LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id)
                 && (!LIF_TABLE_MNGR_DO_NOT_SET_FIELD_UNLESS_EXPLICITLY_SPECIFIED(field_id)))
                || (LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id)
                    && LIF_TABLE_MNGR_IF_UNSET_RESET_FIELD(field_id)))
            {
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);

            }
        }

    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Replace existing entry in lif table with new result type
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] existing_result_type - result type of the current entry
 *   \param [in] new_result_type - new entry result type
 *   \param [in] local_lif_id - new entry local lif
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_set_hw_replace_result_type(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 existing_result_type,
    uint32 new_result_type,
    int local_lif_id)
{
    uint32 lif_table_entry_handle_id_existing, lif_table_entry_handle_id_new;
    int field_idx;
    CONST dbal_logical_table_t *table;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** We need two handles - one to read existing values and one to fill new entry */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_existing));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id_new));
    /** Set keys for the two handles */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, existing_result_type, local_lif_id,
                     lif_table_entry_handle_id_existing));
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, new_result_type, local_lif_id,
                     lif_table_entry_handle_id_new));

    /*
     * In case we replace result type of an entry we must fill all the fields.
     * We will copy all fields that are mutual for the two result types and fill with defaults all the other values.
     */
    /** Get all fields from existing entry */
    SHR_IF_ERR_EXIT(dbal_entry_get
                    (unit, lif_table_entry_handle_id_existing, DBAL_GET_ALL_FIELDS | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

    /*
     * For each field in the new result type:
     *    1. If input entry_handle_id should set it: write new value according to user input
     *    2. else if should be rewritten by default: write default value
     *    3. If neither done:
     *       3.1 If present in the old result type: fill it from there
     *       3.2 else fill default
     */
    for (field_idx = 0; field_idx < table->multi_res_info[new_result_type].nof_result_fields; field_idx++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        uint8 is_field_set = FALSE;

        dbal_fields_e field_id = table->multi_res_info[new_result_type].results_info[field_idx].field_id;
        if (field_id == DBAL_FIELD_RESULT_TYPE)
        {
            /** Only field that should be skipped is result type */
            continue;
        }
        /** Check if field exists on user entry */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** Field exists: Copy the field value from the superset handle unless not required */
            if (!LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id))
            {
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE, field_val);
                is_field_set = TRUE;
            }
        }
        else
        {
            /** Field does not exist: set to default unless no update required */
            if ((!LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id)
                 && (!LIF_TABLE_MNGR_DO_NOT_SET_FIELD_UNLESS_EXPLICITLY_SPECIFIED(field_id)))
                || (LIF_TABLE_MNGR_DO_NOT_SET_FIELD(field_id, dbal_table_id)
                    && LIF_TABLE_MNGR_IF_UNSET_RESET_FIELD(field_id)))
            {
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
                is_field_set = TRUE;

            }
        }
        /** Field that were not set must get value - either from old table or default */
        if (!is_field_set)
        {
            /** Check if field exists on old hw entry */
            rv = dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id_existing, field_id,
                                                         INST_SINGLE, field_val);
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
            if (rv == _SHR_E_NONE)
            {
                /** Copy old HW value to new entry */
                dbal_entry_value_field_arr32_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE, field_val);
            }
            else
            {
                /** Set value from default */
                dbal_entry_value_field_predefine_value_set(unit, lif_table_entry_handle_id_new, field_id, INST_SINGLE,
                                                           DBAL_PREDEF_VAL_DEFAULT_VALUE);
            }
        }
    }
    /** Now we have all the values in the new entry set and we can write new entry to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit
                    (unit, lif_table_entry_handle_id_new, DBAL_COMMIT | DBAL_COMMIT_IGNORE_ALLOC_ERROR));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get dbal handle id fields and table specific info from HW lif table entry
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] result_type - new entry result type
 *   \param [in] lif_table_entry_handle_id - DBAL entry handle of the hw lif table
 *   \param [in] field_ids_bitmap - bitmap containing sw state result for valid fields in this entry
 *   \param [out] entry_handle_id - DBAL entry handle. The entry will be set to type DBAL_SUPERSET_RESULT_TYPE
 *                  and contain all the fields that are set in HW
 *   \param [out] table_specific_flags - additional information per table
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 */
static shr_error_e
dnx_lif_table_mngr_get_table_fields_and_specific_flags(
    int unit,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type,
    uint32 lif_table_entry_handle_id,
    uint32 *field_ids_bitmap,
    uint32 entry_handle_id,
    uint32 *table_specific_flags)
{
    int field_idx;
    CONST dbal_logical_table_t *table;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(field_ids_bitmap, _SHR_E_INTERNAL, "field_ids_bitmap");
    SHR_NULL_CHECK(table_specific_flags, _SHR_E_INTERNAL, "table_specific_flags");

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, DBAL_SUPERSET_RESULT_TYPE);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    /** Loop on all the fields and fill lif info */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id != DBAL_FIELD_RESULT_TYPE)
        {
            /** Check in SW state if this field is enabled */
            if (SHR_BITGET(field_ids_bitmap, field_idx))
            {
                /*
                 * Field enabled - copy value from HW to the superset handle
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, lif_table_entry_handle_id, field_id,
                                                                        INST_SINGLE, field_val));
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
            }
        }
    }

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_table_specific_flags_fill
                    (unit, dbal_table_id, result_type, table_specific_flags));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_get_outlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    uint32 lif_table_entry_handle_id, sw_field_indicator_entry_handle_id;
    uint32 result_type;
    dbal_tables_e dbal_table_id;
    uint32 field_ids_bitmap[_SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit))];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");

    sal_memset(lif_info, 0, sizeof(lif_table_mngr_outlif_info_t));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get(unit, local_lif_id,
                                                                             &dbal_table_id,
                                                                             &result_type, &lif_info->outlif_phase));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** This SW table stores the valid fields for each lif */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_table_fields_and_specific_flags
                    (unit, local_lif_id, dbal_table_id, result_type, lif_table_entry_handle_id,
                     field_ids_bitmap, entry_handle_id, &(lif_info->table_specific_flags)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_get_inlif_info(
    int unit,
    int local_lif_id,
    uint32 entry_handle_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    uint32 lif_table_entry_handle_id, sw_field_indicator_entry_handle_id;
    uint32 result_type;
    dbal_tables_e dbal_table_id;
    uint32 field_ids_bitmap[_SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit))];
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");

    /*
     * Take DBAL table ID from handle and get the physical table id from table id
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                    (unit, local_lif_id, lif_info->core_id, physical_table_id, &dbal_table_id, &result_type));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, lif_info->core_id, TRUE, dbal_table_id, result_type, local_lif_id,
                     lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_table_entry_handle_id, DBAL_GET_ALL_FIELDS));

    /** This SW table stores the valid fields for each lif */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_IN_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, lif_info->core_id);
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_get_table_fields_and_specific_flags
                    (unit, local_lif_id, dbal_table_id, result_type, lif_table_entry_handle_id,
                     field_ids_bitmap, entry_handle_id, &(lif_info->table_specific_flags)));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief - Clear HW Inlif table.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - relevant for inlif tables
 *   \param [in] is_ingress - set if ingress dbal table is to be cleared
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] local_lif_id - Allocated local lif id
 *   \param [in] result_type - Chosen result type
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_lif_clear_hw(
    int unit,
    int core_id,
    uint8 is_ingress,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 result_type)
{
    uint32 lif_table_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &lif_table_entry_handle_id));

    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_lif_table_key_fields
                    (unit, core_id, is_ingress, dbal_table_id, result_type, local_lif_id, lif_table_entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, lif_table_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate local lif according to table id and result type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] result_type - Chosen result type
 *   \param [out] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_allocate_outlif(
    int unit,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_outlif_info_t * lif_info,
    uint32 result_type,
    int *local_lif_id)
{
    lif_mngr_local_outlif_info_t outlif_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * RIF has special allocation procedure
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        SHR_IF_ERR_EXIT(dnx_algo_l3_rif_allocate(unit, *local_lif_id, result_type));
    }
    else
    {
        sal_memset(&outlif_info, 0, sizeof(lif_mngr_local_outlif_info_t));

        outlif_info.dbal_table_id = dbal_table_id;
        outlif_info.outlif_phase = lif_info->outlif_phase;
        outlif_info.dbal_result_type = result_type;

        /** Allocate only local LIF */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_allocate(unit, &outlif_info));

        *local_lif_id = outlif_info.local_outlif;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate local lif according to table id and result type.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] dbal_table_id - dbal table of relevant lif
 *   \param [in] lif_info - Input info about the lif to allocate
 *   \param [in] result_type - Chosen result type
 *   \param [out] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_allocate_inlif(
    int unit,
    dbal_tables_e dbal_table_id,
    lif_table_mngr_inlif_info_t * lif_info,
    uint32 result_type,
    int *local_lif_id)
{
    lif_mngr_local_inlif_info_t inlif_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&inlif_info, 0, sizeof(lif_mngr_local_inlif_info_t));

    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.core_id = lif_info->core_id;
    inlif_info.dbal_result_type = result_type;

    /** Allocate only local LIF */
    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_allocate(unit, &inlif_info));

    *local_lif_id = inlif_info.local_inlif;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SW state table that contains information
 *   on the valid fields for each lif. 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_lif_table_mngr_valid_fields_clear_by_out_lif(
    int unit,
    int local_lif_id)
{
    uint32 sw_field_indicator_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    /*
     * Clear entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SW state table that contains information
 *   on the valid fields for each lif.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] local_lif_id - Allocated local lif id
 * \par DIRECT OUTPUT:
 *   shr_error_e
 */
static shr_error_e
dnx_lif_table_mngr_valid_fields_clear_by_in_lif(
    int unit,
    int core_id,
    int local_lif_id)
{
    uint32 sw_field_indicator_entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_IN_LIF_TABLE,
                                      &sw_field_indicator_entry_handle_id));
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /*
     * Clear entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Fill SW state table that contains information
 *   on the valid fields for each lif. 
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id (relevant only for ingress
 *   \param [in] is_ingress - indication is the table configured is inlif or outlif
 *   \param [in] dbal_table_id - relevant table id
 *   \param [in] entry_handle_id - DBAL entry handle. The entry must be of type DBAL_SUPERSET_RESULT_TYPE
 *                  Contains all the fields that should be set in the new entry
 *   \param [in] result_type - new entry result type
 *   \param [in] local_lif_id - new entry local outlif
 *   \param [in] is_replace - indication to replace existing entry
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   lif_table_mngr_outlif_info_t
 */
static shr_error_e
dnx_lif_table_mngr_store_valid_fields(
    int unit,
    int core_id,
    uint8 is_ingress,
    dbal_tables_e dbal_table_id,
    uint32 entry_handle_id,
    uint32 result_type,
    int local_lif_id,
    uint8 is_replace)
{
    int field_idx;
    dbal_tables_e sw_stat_table_id;
    CONST dbal_logical_table_t *table;
    uint32 sw_field_indicator_entry_handle_id;
    uint32 field_ids_bitmap[_SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit))];
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    sal_memset(field_ids_bitmap, 0, sizeof(field_ids_bitmap));
    sw_stat_table_id = is_ingress ? DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_IN_LIF_TABLE :
        DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE;

    /*
     * Add only relevant entries
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, sw_stat_table_id, &sw_field_indicator_entry_handle_id));
    if (is_ingress)
    {
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
    }
    else
    {
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    }

    /** Get table info */
    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

        /** Get potential required field */
        field_id = table->multi_res_info[result_type].results_info[field_idx].field_id;

        /** Check if the field is required */
        rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
        SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        if (rv == _SHR_E_NONE)
        {
            /** This field is required add to SW state */
            SHR_BITSET(field_ids_bitmap, field_idx);
        }
        else
        {
            /** This field is not required */
            SHR_BITCLR(field_ids_bitmap, field_idx);
        }
    }

    dbal_entry_value_field_arr32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                     field_ids_bitmap);
    /** Valid for both new entry and update, so existing entry should be overwritten */
    if (is_replace)
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT_UPDATE));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse ISEM and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] physical_table_id - inlif table id (1/2)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_isem_pointers(
    int unit,
    dbal_physical_tables_e physical_table_id,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_physical_tables_e match_physical_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    

    
    match_physical_table_id =
        (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_1) ? DBAL_PHYSICAL_TABLE_ISEM_1 : DBAL_PHYSICAL_TABLE_ISEM_2;
    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, match_physical_table_id,
                     DBAL_TABLE_TYPE_EM, &table_id));
    while (table_id != DBAL_TABLE_EMPTY)
    {
        /*
         * Traverse by rule over the current table
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, INST_SINGLE,
                         DBAL_CONDITION_EQUAL_TO, &old_local_lif_id, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, entry_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_IN_LIF,
                         INST_SINGLE, &new_local_lif_id, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

        DBAL_HANDLE_FREE(unit, entry_handle_id);

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_NONE, DBAL_ACCESS_METHOD_MDB, match_physical_table_id,
                         DBAL_TABLE_TYPE_EM, &table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse ISEM and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] core_id - core id
 *   \param [in] physical_table_id - physical table id (inlif1/2/3)
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_termination_match_information(
    int unit,
    int core_id,
    dbal_physical_tables_e physical_table_id,
    int old_local_lif_id,
    int new_local_lif_id)
{
    uint32 entry_handle_id;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Check according to physical table which SW state to access DPC or SBC
     */
    if (physical_table_id == DBAL_PHYSICAL_TABLE_INLIF_2)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_DPC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LOCAL_SBC_IN_LIF_MATCH_INFO_SW, &entry_handle_id));
    }

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, old_local_lif_id);
    /*
     * Not all lif store match information, replace only if exists
     */
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);

    if (rv == _SHR_E_NONE)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, new_local_lif_id);
        SHR_IF_ERR_EXIT(dbal_entry_handle_update_field_ids(unit, entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LOCAL_LIF, old_local_lif_id);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Traverse EEDB and change all pointer to given lif to the new value.
 *  Used by dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - unit id
 *   \param [in] old_local_lif_id - local lif id that should be replaced
 *   \param [in] new_local_lif_id - new local lif value
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \see
 *   dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_update_eedb_next_pointer(
    int unit,
    uint32 old_local_lif_id,
    uint32 new_local_lif_id)
{
    uint32 entry_handle_id;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                    (unit, DBAL_TABLE_EMPTY, DBAL_LABEL_EEDB, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                     DBAL_TABLE_TYPE_DIRECT, &table_id));
    while (table_id != DBAL_TABLE_EMPTY)
    {
        /*
         * Traverse by rule over the current table
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));

        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_NEXT_OUTLIF_POINTER, INST_SINGLE,
                         DBAL_CONDITION_EQUAL_TO, &old_local_lif_id, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_field_arr32_action_add
                        (unit, entry_handle_id, DBAL_ITER_ACTION_UPDATE, DBAL_FIELD_NEXT_OUTLIF_POINTER,
                         INST_SINGLE, &new_local_lif_id, NULL));

        SHR_IF_ERR_EXIT(dbal_iterator_action_commit(unit, entry_handle_id));

        DBAL_HANDLE_FREE(unit, entry_handle_id);

        SHR_IF_ERR_EXIT(dbal_tables_next_table_get
                        (unit, table_id, DBAL_LABEL_EEDB, DBAL_ACCESS_METHOD_MDB, DBAL_PHYSICAL_TABLE_NONE,
                         DBAL_TABLE_TYPE_DIRECT, &table_id));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_lif_table_mngr_allocate_local_outlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_allocate_local_outlif_and_set_hw_verify(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && (lif_info->global_lif == 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lig must be allocated when calling dnx_lif_table_mngr_allocate_local_outlif_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_allocate_local_outlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_outlif_info_t * lif_info)
{
    uint32 result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif, is_replace_with_new_local_lif;
    int old_local_lif_id = 0;
    uint8 transparent_move = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_table_mngr_allocate_local_outlif_and_set_hw_verify
                          (unit, entry_handle_id, local_lif_id, lif_info));

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));

    /*
     * Decide on smallest result type 
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type
                    (unit, dbal_table_id, lif_info->table_specific_flags, entry_handle_id, &result_type));
    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        int outlif_phase_dummy;
        dbal_tables_e dbal_table_id_dummy;

        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get(unit, *local_lif_id,
                                                                                 &dbal_table_id_dummy,
                                                                                 &existing_result_type,
                                                                                 &outlif_phase_dummy));
        /*
         * Currently replacing result type is not allowed
         */
        result_type = existing_result_type;
    }

    
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE) ||
                        (existing_result_type != result_type));
    is_replace_with_new_local_lif = _SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
        && is_new_local_lif;

    /*
     * Lif allocation 
     */
    if (is_new_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_outlif(unit, dbal_table_id, lif_info, result_type, local_lif_id));
    }

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_store_valid_fields
                    (unit, 0, FALSE, dbal_table_id, entry_handle_id, result_type, *local_lif_id, !is_new_local_lif));

    /*
     * set HW 
     */
    
    {
        CONST dbal_logical_table_t *table;
        int new_size, old_size;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        new_size = table->multi_res_info[result_type].entry_payload_size;
        old_size = table->multi_res_info[existing_result_type].entry_payload_size;
        transparent_move = (new_size >= old_size);
    }
    if (is_replace_with_new_local_lif && transparent_move)
    {
        /*
         * In case of new lif, first copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, existing_result_type, old_local_lif_id,
                         *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw
                        (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, entry_handle_id, result_type, *local_lif_id));
    }

    /*
     * In case of replacing with new local lif: Change all EEDB pointers to point on the new lif
     */
    if (is_replace_with_new_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_eedb_next_pointer
                        (unit, (uint32) old_local_lif_id, (uint32) *local_lif_id));
    }

    /*
     * Write to GLEM if required
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;

        if (is_replace_with_new_local_lif)
        {
            /*
             * In case of replacing with new local lif: Update GLM and remove all mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_replace_glem
                            (unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id, LIF_LIB_GLEM_KEEP_OLD_VALUE));
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif));
        }
        else
        {
            /*
             * New lif: add new GLEM entry
             */
            SHR_IF_ERR_EXIT(dnx_lif_lib_add_to_glem(unit, _SHR_CORE_ALL, lif_info->global_lif, *local_lif_id));
        }

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_EGRESS, lif_info->global_lif, &local_lif_mapping_info));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (is_replace_with_new_local_lif)
    {
        if (transparent_move)
        {
            /*
             * Write the new content to the new lif (up to now contained the old lif entry)
             */
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw_replace_result_type
                            (unit, _SHR_CORE_ALL, FALSE, dbal_table_id, entry_handle_id, existing_result_type,
                             result_type, *local_lif_id));
        }
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, _SHR_CORE_ALL, FALSE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local Lif clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, old_local_lif_id));
        /*
         * LIF table valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, old_local_lif_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * Verify function for dnx_lif_table_mngr_allocate_local_inlif_and_set_hw
 */
static shr_error_e
dnx_lif_table_mngr_allocate_local_inlif_and_set_hw_verify(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(lif_info, _SHR_E_INTERNAL, "lif_info");
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && (lif_info->global_lif == 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "global_lig must be allocated when calling dnx_lif_table_mngr_allocate_local_inlif_and_set_hw unless flag LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING is set");
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_allocate_local_inlif_and_set_hw(
    int unit,
    uint32 entry_handle_id,
    int *local_lif_id,
    lif_table_mngr_inlif_info_t * lif_info)
{
    uint32 result_type;
    uint32 existing_result_type = 0;
    dbal_tables_e dbal_table_id;
    uint8 is_new_local_lif, is_replace_with_new_local_lif;
    int old_local_lif_id = 0;
    dbal_physical_tables_e physical_table_id;
    
    uint8 transparent_move;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_lif_table_mngr_allocate_local_inlif_and_set_hw_verify
                          (unit, entry_handle_id, local_lif_id, lif_info));

    /*
     * Take DBAL table ID from handle
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_related_table_get(unit, entry_handle_id, &dbal_table_id));
    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));
    /*
     * Decide on smallest result type
     */
    SHR_IF_ERR_EXIT(dnx_algo_lif_table_mngr_decide_result_type
                    (unit, dbal_table_id, lif_info->table_specific_flags, entry_handle_id, &result_type));
    /*
     * In case of replace find existing result type
     */
    if (_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE))
    {
        dbal_tables_e dbal_table_id_dummy;

        old_local_lif_id = *local_lif_id;
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, *local_lif_id, lif_info->core_id, physical_table_id, &dbal_table_id_dummy,
                         &existing_result_type));
        /*
         * Currently replacing result type is not allowed
         */
        result_type = existing_result_type;
    }

    
    is_new_local_lif = (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE) ||
                        (existing_result_type != result_type));
    is_replace_with_new_local_lif = _SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_REPLACE)
        && is_new_local_lif;

    /*
     * Lif allocation
     */
    if (is_new_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_allocate_inlif(unit, dbal_table_id, lif_info, result_type, local_lif_id));
    }

    /*
     * Save in SW state all the valid fields requested by this action, clear undesired
     * In case of using existing lif, replace flag is on
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_store_valid_fields
                    (unit, lif_info->core_id, TRUE, dbal_table_id, entry_handle_id, result_type, *local_lif_id,
                     !is_new_local_lif));

    /*
     * set HW
     */
    
    {
        CONST dbal_logical_table_t *table;
        int new_size, old_size;

        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));
        new_size = table->multi_res_info[result_type].entry_payload_size;
        old_size = table->multi_res_info[existing_result_type].entry_payload_size;
        transparent_move = (new_size >= old_size);
    }

    if (is_replace_with_new_local_lif && transparent_move)
    {
        /*
         * First copy old lif contents to new lif,
         * redirect the pointers to the new lif and only then rewrite with new data
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_copy_hw
                        (unit, lif_info->core_id, TRUE, dbal_table_id, existing_result_type, old_local_lif_id,
                         *local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw
                        (unit, lif_info->core_id, TRUE, dbal_table_id, entry_handle_id, result_type, *local_lif_id));
    }

    /*
     * In case of replacing with new local lif: Change all ISEM entries to point on the new lif
     */
    if (is_replace_with_new_local_lif)
    {
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_isem_pointers
                        (unit, physical_table_id, (uint32) old_local_lif_id, (uint32) *local_lif_id));
    }

    /*
     * Create SW global->local mapping
     */
    if (!_SHR_IS_FLAG_SET(lif_info->flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING)
        && is_new_local_lif)
    {
        lif_mapping_local_lif_info_t local_lif_mapping_info;
        sal_memset(&local_lif_mapping_info, 0, sizeof(lif_mapping_local_lif_info_t));
        local_lif_mapping_info.local_lif = *local_lif_id;
        local_lif_mapping_info.core_id = lif_info->core_id;
        local_lif_mapping_info.phy_table = physical_table_id;

        if (is_replace_with_new_local_lif)
        {
            /*
             * In case of replacing with new local lif: Remove mapping from SW
             */
            SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif));
        }

        /** Add new global to local lif mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_create
                        (unit, DNX_ALGO_LIF_INGRESS, lif_info->global_lif, &local_lif_mapping_info));
    }

    /*
     * In case of replacing with new local lif: Clear old allocated id and HW old local lif entry
     */
    if (is_replace_with_new_local_lif)
    {
        lif_mngr_local_inlif_info_t old_local_lif_info;

        /*
         * Write the new content to the new lif (up to now contained the old lif entry)
         */
        if (transparent_move)
        {
            SHR_IF_ERR_EXIT(dnx_lif_table_mngr_set_hw_replace_result_type
                            (unit, lif_info->core_id, TRUE, dbal_table_id, entry_handle_id, existing_result_type,
                             result_type, *local_lif_id));
        }
        /*
         * HW clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                        (unit, lif_info->core_id, TRUE, old_local_lif_id, dbal_table_id, existing_result_type));
        /*
         * Local Lif clear
         */
        old_local_lif_info.dbal_table_id = dbal_table_id;
        old_local_lif_info.dbal_result_type = existing_result_type;
        old_local_lif_info.core_id = lif_info->core_id;
        old_local_lif_info.local_inlif = old_local_lif_id;

        SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &old_local_lif_info));

        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_update_termination_match_information
                        (unit, lif_info->core_id, physical_table_id, old_local_lif_id, *local_lif_id));
        /*
         * old LIF valid fields clear
         */
        SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif(unit, lif_info->core_id, old_local_lif_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_outlif_info_clear(
    int unit,
    int local_lif_id,
    uint32 flags)
{
    dbal_tables_e dbal_table_id;
    uint32 result_type;
    int outlif_phase_dummy;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get(unit, local_lif_id,
                                                                             &dbal_table_id,
                                                                             &result_type, &outlif_phase_dummy));

    /*
     * clear HW 
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw
                    (unit, _SHR_CORE_ALL, FALSE, local_lif_id, dbal_table_id, result_type));

    /*
     * clear SW
     */

    /*
     * Clear VALID FIELDS SW state
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_out_lif(unit, local_lif_id));

    /*
     * Local Lif clear 
     */
    if (dbal_table_id == DBAL_TABLE_EEDB_RIF_BASIC)
    {
        /*
         * RIF has special handling
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_rif_free(unit, local_lif_id));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_lib_outlif_free(unit, local_lif_id));
    }

    /*
     * Clear GLEM if required
     */
    if (!_SHR_IS_FLAG_SET(flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING))
    {
        int global_lif;
        lif_mapping_local_lif_info_t local_lif_info;
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_lif_id;
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_EGRESS, &local_lif_info, &global_lif));

        /** Update the  GLEM. **/
        SHR_IF_ERR_EXIT(dnx_lif_lib_remove_from_glem(unit, _SHR_CORE_ALL, global_lif));

        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_EGRESS, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file for details */
shr_error_e
dnx_lif_table_mngr_inlif_info_clear(
    int unit,
    int core_id,
    int local_lif_id,
    dbal_tables_e dbal_table_id,
    uint32 flags)
{
    uint32 result_type;
    lif_mngr_local_inlif_info_t inlif_info;
    dbal_physical_tables_e physical_table_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get physical table id
     */
    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get(unit, dbal_table_id, DBAL_PHY_DB_DEFAULT_INDEX, &physical_table_id));

    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                    (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id, &result_type));

    /*
     * clear HW
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_lif_clear_hw(unit, core_id, TRUE, local_lif_id, dbal_table_id, result_type));

    /*
     * clear SW
     */

    /*
     * Clear VALID FIELDS SW state
     */
    SHR_IF_ERR_EXIT(dnx_lif_table_mngr_valid_fields_clear_by_in_lif(unit, core_id, local_lif_id));

    /*
     * Local Lif clear
     */
    inlif_info.dbal_table_id = dbal_table_id;
    inlif_info.dbal_result_type = result_type;
    inlif_info.core_id = core_id;
    inlif_info.local_inlif = local_lif_id;

    SHR_IF_ERR_EXIT(dnx_lif_lib_inlif_free(unit, &inlif_info));

    /*
     * Clear global to local mapping if required
     */
    if (!_SHR_IS_FLAG_SET(flags, LIF_TABLE_MNGR_LIF_INFO_DONT_UPDATE_GLOBAL_TO_LOCAL_MAPPING))
    {
        int global_lif;
        lif_mapping_local_lif_info_t local_lif_info;
        sal_memset(&local_lif_info, 0, sizeof(local_lif_info));
        local_lif_info.local_lif = local_lif_id;
        local_lif_info.core_id = core_id;
        local_lif_info.phy_table = physical_table_id;

        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_local_to_global_get
                        (unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif));

        /** delete SW state mapping */
        SHR_IF_ERR_EXIT(dnx_algo_lif_mapping_remove(unit, DNX_ALGO_LIF_INGRESS, global_lif));
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_is_field_exist_on_dbal_handle(
    int unit,
    uint32 entry_handle_id,
    dbal_fields_e field_id,
    uint8 *is_field_on_handle)
{
    shr_error_e rv;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Try to read value, if succeeded return found */
    rv = dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, field_id, INST_SINGLE, field_val);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    *is_field_on_handle = (rv == _SHR_E_NONE);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_lif_table_mngr_is_valid_field(
    int unit,
    int core_id,
    int local_lif_id,
    uint8 is_ingress,
    dbal_physical_tables_e physical_table_id,
    dbal_fields_e field_id,
    uint8 *is_valid)
{
    uint32 sw_field_indicator_entry_handle_id;
    uint32 result_type;
    lif_mngr_outlif_phase_e outlif_phase_dummy;
    dbal_tables_e dbal_table_id;
    uint32 field_ids_bitmap[_SHR_BITDCLSIZE(dnx_data_lif.lif_table_manager.max_fields_per_result_type_get(unit))];
    int field_idx;
    CONST dbal_logical_table_t *table;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                        (unit, local_lif_id, core_id, physical_table_id, &dbal_table_id, &result_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_table_id_and_result_type_get(unit, local_lif_id, &dbal_table_id,
                                                                                 &result_type, &outlif_phase_dummy));
    }

    /** This SW table stores the valid fields for each lif */
    if (is_ingress == FALSE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_OUT_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, local_lif_id);
    }
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_LIF_TABLE_MANAGER_VALID_FIELDS_PER_IN_LIF_TABLE,
                                          &sw_field_indicator_entry_handle_id));
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, local_lif_id);
        dbal_entry_key_field32_set(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    }
    dbal_value_field_arr32_request(unit, sw_field_indicator_entry_handle_id, DBAL_FIELD_FIELDS_BMP, INST_SINGLE,
                                   field_ids_bitmap);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_field_indicator_entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, dbal_table_id, &table));

    *is_valid = FALSE;
    /** Loop over the fields and find the relevant field to check */
    for (field_idx = 0; field_idx < table->multi_res_info[result_type].nof_result_fields; field_idx++)
    {
        dbal_fields_e field_id_curr = table->multi_res_info[result_type].results_info[field_idx].field_id;

        if (field_id == field_id_curr)
        {
            /** Check in SW state if this field is enabled and exit */
            *is_valid = SHR_BITGET(field_ids_bitmap, field_idx) ? TRUE : FALSE;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * }
 */
