/** \file ctest_dnx_dbal_tests.c
 *
 * Main unitests for dbal applications.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDBALDNX

/*************
 * INCLUDES  *
 *************/
#include <sal/appl/sal.h>
#include <shared/bslnames.h>
#include <appl/diag/diag.h>
#include <bcm/l2.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/utilex/utilex_framework.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/field/field_entry.h>
#include "src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h"
#include <bcm_int/dnx/algo/res_mngr/res_mngr_dbal_api.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/lif/lif_lib.h>
#include <bcm_int/dnx/cosq/egress/ecgm.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dbal/dbal_journal.h>
#include <soc/dnx/pll/pll.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx_dispatch.h>
#include <sal/core/boot.h>
#include <soc/counter.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>

/*************
 *  DEFINES  *
 *************/
/**
 * minimum number of entries that checked in logical table test
 */
#define DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE       (32)

/**
 * total max num of fields (result+key)
 */
#define DBAL_TABLE_MAX_NUM_OF_FIELDS              (DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS + DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)

/*************
 * TYPEDEFS  *
 *************/
/**
 * all combination per table structure
 */
typedef struct
{
    /**
     * Number of fields in combination
     */
    int nof_fields;

    /**
     * Number of valid combinations
     */
    int nof_combinations;

    /**
     * The fields combinations
     */
    dbal_fields_e fields_id[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * The fields combinations
     */
    int instance_idx[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * key indication per field per combination
     */
    uint8 fields_is_key[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * field length [bits] per field per combination
     */
    int fields_bit_length[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * field min value per field per combination
     */
    int fields_min_val[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * field max value per field per combination
     */
    int fields_max_val[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];

    /**
     * field offset [bits] per field per combination
     */
    int fields_bit_offset[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE][DBAL_TABLE_MAX_NUM_OF_FIELDS];
} diag_dbal_ltt_fields_to_check;

/*************
 *  GLOBALS  *
 *************/

/**
 * fields combinations
 */
diag_dbal_ltt_fields_to_check fields_comb;

/**
 * lacal outlif base value
 */
int outlif_base_val;
/**
 * fields values for random values test
 */
uint8 valid_key_value[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE];

/**
 * fields values for random values test
 */
uint32 key_field_val[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE]
    [DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * fields get values for random values test
 */
uint32 key_field_val_get[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE]
    [DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * fields get values for random values test
 */
uint32 result_field_val[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE]
    [DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * fields get values for random values test
 */
uint32 result_field_val_get[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE]
    [DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * fields get values as uint8
 */
uint8 result_field_val_arr8_get[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES];

/**
 * iterated fields (all entries)
 */
uint32 iterator_parsed_fields[DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

/**
 * found by iterator indication (for random entries values)
 */
uint8 iterator_found_indications[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE];

/**
 * Access id for tcam access
 */
uint32 access_id[DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE];

const uint32 dbal_ltt_timer_limits[DBAL_NOF_ACCESS_METHODS][LTT_TIMER_ENTIRE_LTT_TEST] = {
    /*
     * , SET , GET , ITERATOR , EN_CLEAR , TA_CLEAR , ITERATOR_E ,
     */
    /** MDB */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX},
    /** TCAM CS */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX},
    /** HL */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX},
    /** SW ONLY */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX},
    /** PEMLA */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX},
    /** KBP   */
    {UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX, UTILEX_U32_MAX}

};

/*************
 *  MACROES  *
 *************/
#define TABLE_CLEAR_WITH_INFO_AND_TIMER(info_string)                                                        \
do                                                                                                          \
{                                                                                                           \
    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%s"), info_string));                                        \
    if (flags & LTT_FLAGS_MEASUREMERNTS)                                                                    \
    {                                                                                                       \
        LTT_TIMERS_TIMER_SET("Table Clear Timer", LTT_TIMER_TABLE_CLEAR);                                   \
    }                                                                                                       \
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));                                                      \
    if (DBAL_TABLE_IS_TCAM(table) && (table->access_method == DBAL_ACCESS_METHOD_MDB))                      \
    {                                                                                                       \
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy_all(unit, table_id));                             \
    }                                                                                                       \
    if (flags & LTT_FLAGS_MEASUREMERNTS)                                                                    \
    {                                                                                                       \
        LTT_TIMERS_TIMER_STOP(LTT_TIMER_TABLE_CLEAR);                                                       \
    }                                                                                                       \
}while(0)

#define ITERATOR_EMPTY_TABLE_WITH_TIMER()                                                                   \
do                                                                                                          \
{                                                                                                           \
    int counter = -1;                                                                                       \
    if (flags & LTT_FLAGS_MEASUREMERNTS)                                                                    \
    {                                                                                                       \
        LTT_TIMERS_TIMER_SET("Empty Table Iterator Timer\n", LTT_TIMER_ITERATOR_EMPTY_TABLE);               \
    }                                                                                                       \
    SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, &counter));                            \
    if (flags & LTT_FLAGS_MEASUREMERNTS)                                                                    \
    {                                                                                                       \
        LTT_TIMERS_TIMER_STOP(LTT_TIMER_ITERATOR_EMPTY_TABLE);                                              \
    }                                                                                                       \
    if (counter != 0)                                                                                       \
    {                                                                                                       \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found %d entries in table after table clean. table %s\n",            \
                     counter, table->table_name);                                                           \
    }                                                                                                       \
}while(0)

/*************
 * FUNCTIONS *
 *************/

/** this function return 1 if the field type is a special field type that uses arr8 with reverse byte oredering to
 *  encode the field */
int
diag_dbal_is_arr8_special_field(
    int unit,
    dbal_fields_e field_id)
{
    dbal_field_types_defs_e field_type;
    int rv;

    rv = dbal_fields_field_type_get(unit, field_id, &field_type);

    if (rv)
    {
        LOG_CLI((BSL_META("Illegal field %s\n"), dbal_field_to_string(unit, field_id)));
    }
    switch (field_type)
    {
        case DBAL_FIELD_TYPE_DEF_IPV6_GROUP:
        case DBAL_FIELD_TYPE_DEF_IPV6:
        case DBAL_FIELD_TYPE_DEF_L2_MAC:
            return 1;
        default:
            return 0;
    }
}

shr_error_e
diag_dnx_dbal_timers_check_exceptions(
    int unit,
    dbal_tables_e table_id)
{
    int ii;

    dbal_access_method_e access_method;

    CONST dbal_logical_table_t *table;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    access_method = table->access_method;

    for (ii = 0; ii < LTT_TIMER_ENTIRE_LTT_TEST; ii++)
    {
        uint32 basic_time_limit = dbal_ltt_timer_limits[access_method][ii];
        uint32 weighted_time_limit = 0;

        switch (ii)
        {
            case LTT_TIMER_SET:
                weighted_time_limit = basic_time_limit;
                break;
            case LTT_TIMER_GET:
                weighted_time_limit = basic_time_limit;
                break;
            case LTT_TIMER_ITERATOR:
                weighted_time_limit = basic_time_limit;
                break;
            case LTT_TIMER_ENTRY_CLEAR:
                weighted_time_limit = basic_time_limit;
                break;
            case LTT_TIMER_TABLE_CLEAR:
                weighted_time_limit = basic_time_limit;
                break;
            case LTT_TIMER_ITERATOR_EMPTY_TABLE:
                weighted_time_limit = basic_time_limit;
                break;
        }
        if (LTT_TIMERS_TIMER_GET_TIME_US(ii) > weighted_time_limit)
        {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT,
                         "Timer exceed the limit.Measured %d,Limit %d,Basic time %d,Table %s,Timer %d\n",
                         LTT_TIMERS_TIMER_GET_TIME_US(ii), weighted_time_limit, basic_time_limit,
                         table->table_name, ii);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#ifndef ADAPTER_SERVER_MODE
/**
* \brief
* check if table is part of the
*/
STATIC uint8
table_is_exclude_from_pcid_mode(
    dbal_tables_e table_id)
{
    int ii, nof_tables;

    dbal_tables_e table_list[] = {

        /** Specific tables */
        DBAL_NOF_TABLES,
        DBAL_TABLE_KLEAP_IPMF1_KBR_INFO,
        DBAL_TABLE_KLEAP_IPMF1_INITIAL_KBR_INFO,
        DBAL_TABLE_KLEAP_IPMF2_KBR_INFO,
        DBAL_TABLE_KLEAP_IPMF3_KBR_INFO,
        DBAL_TABLE_KLEAP_E_PMF_KBR_INFO,
        DBAL_TABLE_KLEAP_IPMF1_FFC_INSTRUCTION,
        DBAL_TABLE_KLEAP_IPMF2_FFC_INSTRUCTION,
        DBAL_TABLE_KLEAP_IPMF3_FFC_INSTRUCTION,
        DBAL_TABLE_KLEAP_E_PMF_FFC_INSTRUCTION,
    };

    nof_tables = sizeof(table_list) / sizeof(dbal_tables_e);

    for (ii = 0; ii < nof_tables; ii++)
    {
        if (table_id == table_list[ii])
        {
            return TRUE;
        }
    }
    return FALSE;
}
#endif

int
compare_fields_values(
    int unit,
    CONST dbal_logical_table_t * table,
    uint32 field_val[][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    uint32 field_val_get[][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS],
    int res_type_idx,
    uint8 is_key)
{
    int ii, jj;
    uint32 bit, bit_get;
    int nof_fields = is_key ? table->nof_key_fields : table->multi_res_info[res_type_idx].nof_result_fields;
    CONST dbal_table_field_info_t *field_info =
        is_key ? table->keys_info : table->multi_res_info[res_type_idx].results_info;

    for (ii = 0; ii < nof_fields; ii++)
    {

        /**
         * Skip readonly, writeonly and trigger fields
         */
        if ((field_info[ii].permission == DBAL_PERMISSION_READONLY)
            || (field_info[ii].permission == DBAL_PERMISSION_WRITEONLY)
            || (field_info[ii].permission == DBAL_PERMISSION_TRIGGER))
        {
            continue;
        }

        for (jj = field_info[ii].offset_in_logical_field;
             jj < (field_info[ii].field_nof_bits + field_info[ii].offset_in_logical_field); jj++)
        {
            bit = (field_val[ii][jj / 32] >> (jj % 32)) & 0x1;
            bit_get = (field_val_get[ii][jj / 32] >> (jj % 32)) & 0x1;
            if (bit != bit_get)
            {
                LOG_VERBOSE(BSL_LOG_MODULE,
                            (BSL_META_U(unit, "Comparison error: field=%s, bit=%d, expectWord=0x%x, getWord=0x%x\n"),
                             dbal_field_to_string(unit, field_info[ii].field_id), jj,
                             field_val[ii][jj / 32], field_val_get[ii][jj / 32]));
                return -1;
            }
        }
    }
    return 0;
}

/**
* \brief
* print out all table's field possibole combinations
*/
static void
print_fields_combinations(
    int unit,
    diag_dbal_ltt_fields_to_check * field_combo)
{
    int ii, jj;
    for (ii = 0; ii < field_combo->nof_combinations; ii++)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "%d)\t"), ii));
        for (jj = 0; jj < field_combo->nof_fields; jj++)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "field[%d]=%s(instance=%d) "), jj,
                                      dbal_field_to_string(unit, field_combo->fields_id[ii][jj]),
                                      field_combo->instance_idx[ii][jj]));
        }
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "\n")));
    }
}

/**
* \brief
* adding new list of fields to combinations structure.
* this function will be called when a parent field is in the table, and will add its child fields
*/
static shr_error_e
diag_dbal_test_add_child_fields_to_combinations(
    int unit,
    diag_dbal_ltt_fields_to_check * field_combo,
    int parent_field_size,
    dbal_sub_field_info_t * child_fields_info,
    int nof_chiled_fields,
    int field_offset_in_buffer,
    uint8 is_key,
    int nof_instances)
{
    int ii, jj, kk;
    int nof_combos;
    int nof_valid_childs = 0;
    int child_index = 0;
    uint8 is_valid_child[DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < nof_chiled_fields; ii++)
    {
        uint32 sub_field_type_size;
        uint32 encoding_size, encoding_val;

        encoding_val = child_fields_info[ii].encode_info.input_param;
        if (encoding_val == 0)
        {
            encoding_size = 0;
        }
        else
        {
            encoding_size = (utilex_msb_bit_on(encoding_val) + 1);
        }
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, child_fields_info[ii].sub_field_id, &sub_field_type_size));
        
        if (sub_field_type_size + encoding_size <= parent_field_size)
        {
            is_valid_child[ii] = TRUE;
            nof_valid_childs++;
        }
    }
    child_index = 0;
    nof_combos = field_combo->nof_combinations;
    for (ii = 0; ii < nof_chiled_fields; ii++)
    {
        if (is_valid_child[ii] == FALSE)
        {
            continue;
        }
        for (jj = 0; jj < nof_combos; jj++)
        {
            sal_memcpy(&field_combo->fields_id[child_index * nof_combos + jj][0], &field_combo->fields_id[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->instance_idx[child_index * nof_combos + jj][0], &field_combo->instance_idx[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->fields_is_key[child_index * nof_combos + jj][0],
                       &field_combo->fields_is_key[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->fields_bit_length[child_index * nof_combos + jj][0],
                       &field_combo->fields_bit_length[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->fields_max_val[child_index * nof_combos + jj][0],
                       &field_combo->fields_max_val[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->fields_min_val[child_index * nof_combos + jj][0],
                       &field_combo->fields_min_val[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
            sal_memcpy(&field_combo->fields_bit_offset[child_index * nof_combos + jj][0],
                       &field_combo->fields_bit_offset[jj][0],
                       sizeof(int) * DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE);
        }
        child_index++;
    }

    for (kk = 0; kk < nof_instances; kk++)
    {
        child_index = 0;
        for (ii = 0; ii < nof_chiled_fields; ii++)
        {
            dbal_fields_e sub_field_id = child_fields_info[ii].sub_field_id;
            if (is_valid_child[ii] == FALSE)
            {
                continue;
            }
            for (jj = 0; jj < nof_combos; jj++)
            {
                uint32 sub_field_length, sub_field_max_val, sub_field_min_val;
                SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, sub_field_id, &sub_field_length));
                SHR_IF_ERR_EXIT(dbal_fields_min_value_get(unit, sub_field_id, &sub_field_min_val));
                SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, sub_field_id, &sub_field_max_val));
                field_combo->fields_id[child_index * nof_combos + jj][field_combo->nof_fields] = sub_field_id;
                field_combo->instance_idx[child_index * nof_combos + jj][field_combo->nof_fields] = kk;
                field_combo->fields_is_key[child_index * nof_combos + jj][field_combo->nof_fields] = is_key;
                field_combo->fields_bit_length[child_index * nof_combos + jj][field_combo->nof_fields] =
                    sub_field_length;
                field_combo->fields_min_val[child_index * nof_combos + jj][field_combo->nof_fields] = sub_field_min_val;
                field_combo->fields_max_val[child_index * nof_combos + jj][field_combo->nof_fields] = sub_field_max_val;
                field_combo->fields_bit_offset[child_index * nof_combos + jj][field_combo->nof_fields] =
                    field_offset_in_buffer;
            }
            child_index++;
        }
        field_combo->nof_fields++;
    }
    field_combo->nof_combinations *= nof_valid_childs;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* Add field to the fields combination array.
***********************************************/
static shr_error_e
diag_dbal_test_add_field_to_combinations(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_table_field_info_t * field_info,
    uint8 is_key,
    int res_type_idx,
    int *nof_instance_added)
{
    int ii, jj;
    dbal_field_types_basic_info_t *basic_field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_info->field_id, &basic_field_info));

    SHR_IF_ERR_EXIT(dbal_tables_field_nof_instance_get(unit, table_id, field_info->field_id, is_key,
                                                       res_type_idx, nof_instance_added));

    if ((basic_field_info->nof_child_fields != 0) && (!is_key))
    {
        SHR_IF_ERR_EXIT(diag_dbal_test_add_child_fields_to_combinations(unit, &fields_comb,
                                                                        field_info->field_nof_bits,
                                                                        basic_field_info->sub_field_info,
                                                                        basic_field_info->nof_child_fields,
                                                                        field_info->offset_in_logical_field, is_key,
                                                                        *nof_instance_added));
    }
    else
    {
        for (jj = 0; jj < *nof_instance_added; jj++)
        {
            for (ii = 0; ii < fields_comb.nof_combinations; ii++)
            {
                fields_comb.fields_id[ii][fields_comb.nof_fields] = field_info->field_id;
                fields_comb.instance_idx[ii][fields_comb.nof_fields] = jj;
                fields_comb.fields_is_key[ii][fields_comb.nof_fields] = is_key;
                fields_comb.fields_bit_length[ii][fields_comb.nof_fields] = field_info->field_nof_bits;
                fields_comb.fields_max_val[ii][fields_comb.nof_fields] = field_info->max_value;
                fields_comb.fields_min_val[ii][fields_comb.nof_fields] = field_info->min_value;
                fields_comb.fields_bit_offset[ii][fields_comb.nof_fields] = field_info->offset_in_logical_field;
            }
            fields_comb.nof_fields++;
        }
    }

    if (fields_comb.nof_combinations <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Below the min num of fields combinations (%d). table %s\n",
                     0, dbal_logical_table_to_string(unit, table_id));
    }

    if (fields_comb.nof_combinations > DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Exceeds the max num of fields combinations (%d). table %s\n",
                     DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE, dbal_logical_table_to_string(unit, table_id));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_fields_get_value_for_field(
    int unit,
    dbal_fields_e field_id,
    fields_values_types_e type_of_value,
    uint32 field_min_val,
    uint32 field_max_val,
    uint32 field_len,
    uint8 is_key,
    uint32 arr_prefix,
    uint32 arr_prefix_size,
    uint32 *field_value)
{
    dbal_field_types_basic_info_t *field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_id, &field_info));

    switch (field_info->print_type)
    {
        case DBAL_FIELD_PRINT_TYPE_BOOL:
        case DBAL_FIELD_PRINT_TYPE_IPV4:
        case DBAL_FIELD_PRINT_TYPE_IPV6:
        case DBAL_FIELD_PRINT_TYPE_MAC:
        case DBAL_FIELD_PRINT_TYPE_UINT32:
        case DBAL_FIELD_PRINT_TYPE_STRUCT:
        case DBAL_FIELD_PRINT_TYPE_ARRAY8:
        case DBAL_FIELD_PRINT_TYPE_BITMAP:
        case DBAL_FIELD_PRINT_TYPE_ARRAY32:
        case DBAL_FIELD_PRINT_TYPE_HEX:
        case DBAL_FIELD_PRINT_TYPE_DBAL_TABLE:
        case DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE:
        case DBAL_FIELD_PRINT_TYPE_STRING:
        {
            if (field_len > 32)
            {
                /** Const fields bigger than 32 bits should only have 0 value */
                if (SHR_BITGET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID))
                {
                    DIAG_DBAL_MIN_FIELD_VAL(field_value, field_len, field_info->print_type);
                }
                else
                {
                    if (type_of_value == MIN_FIELD_VAL)
                    {
                        DIAG_DBAL_MIN_FIELD_VAL(field_value, field_len, field_info->print_type);
                    }
                    else if (type_of_value == MAX_FIELD_VAL)
                    {
                        DIAG_DBAL_MAX_FIELD_VAL(field_value, field_len);
                    }
                    else
                    {
                        
                        DIAG_DBAL_RANDOM_FIELD_VAL(field_value, field_len, is_key);
                    }
                }
            }
            else
            {
                if (type_of_value == MIN_FIELD_VAL)
                {
                    uint8 is_illegal = TRUE;

                    field_value[0] = field_min_val;

                    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, field_id, field_value[0], &is_illegal));
                    while (is_illegal)
                    {
                        field_value[0]++;
                        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, field_id, field_value[0], &is_illegal));

                        if (field_value[0] == field_min_val + 1000)
                        {
                            /** tring to find min value,, do it for 1000 iterations, usually the first iteration is fine */
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "didnot find min value for field %s min val %d\n",
                                         dbal_field_to_string(unit, field_id), field_min_val);
                        }
                    }
                }
                else if (type_of_value == MAX_FIELD_VAL)
                {
                    uint8 is_illegal = TRUE;

                    field_value[0] = field_max_val;

                    SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, field_id, field_value[0], &is_illegal));
                    while (is_illegal)
                    {
                        field_value[0]--;
                        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, field_id, field_value[0], &is_illegal));

                        if (field_value[0] == 0)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "didnot find max value for field %s max val %d\n",
                                         dbal_field_to_string(unit, field_id), field_max_val);
                        }
                    }
                }
                else
                {
                    uint8 is_illegal = TRUE;

                    while (is_illegal)
                    {
                        
                        DIAG_DBAL_RANDOM_FIELD_VAL(field_value, field_len, is_key);
                        if (field_max_val < UTILEX_U32_MAX)
                        {
                            field_value[0] %= (field_max_val + 1);
                        }

                        if ((field_value[0] == 0) && (!is_key))
                        {
                            field_value[0] = 1;
                        }

                        if ((field_value[0] < field_min_val) || (field_value[0] > field_max_val))
                        {
                            if (field_min_val == field_max_val)
                            {
                                field_value[0] = field_min_val;
                                is_illegal = FALSE;
                            }
                            else
                            {
                                is_illegal = TRUE;
                            }
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, field_id, field_value[0], &is_illegal));
                        }
                    }
                }
                if (arr_prefix_size)
                {
                    uint32 tmp_field_value = field_value[0];
                    SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, arr_prefix,
                                                              field_len, field_len + arr_prefix_size, field_value));
                }
            }

            
            if ((field_id == DBAL_FIELD_FEC) && ((field_value[0] & 0xc0000) == 0xc0000))
            {
                /** if fec field, and both bits 19th,20th are on - disable them */
                field_value[0] &= 0x3ffff;
            }
            break;
        }
        case DBAL_FIELD_PRINT_TYPE_ENUM:
        {
            uint8 found_valid = FALSE;
            uint32 arr_prefix_max = 0;
            uint32 arr_prefix_min = 0;

            if ((field_max_val == 0) && (field_min_val == 0))
            {
                /** incase that the min == max == 0 not need to run the algorithm, just return 0 */
                found_valid = TRUE;
                field_value[0] = 0;
            }

            if (arr_prefix)
            {
                /** find the min/max values for arr prefix */
                uint32 tmp_field_value = 0;

                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, arr_prefix,
                                                          field_len, field_len + arr_prefix_size, &arr_prefix_min));

                tmp_field_value = ((1 << field_len) - 1);
                SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, arr_prefix,
                                                          field_len, field_len + arr_prefix_size, &arr_prefix_max));

            }

            while (!found_valid)
            {
                if (field_max_val > 0)
                {
                    field_value[0] = sal_rand() % UTILEX_MIN(field_max_val + 1, field_info->nof_enum_values);
                }
                else
                {
                    field_value[0] = sal_rand() % field_info->nof_enum_values;
                }

                if ((field_info->enum_val_info[field_value[0]].is_invalid))
                {
                    continue;
                }

                if ((field_info->enum_val_info[field_value[0]].value == 0) && (!is_key))
                {
                    continue;
                }

                if (arr_prefix)
                {
                    if ((field_info->enum_val_info[field_value[0]].value < arr_prefix_min) ||
                        (field_info->enum_val_info[field_value[0]].value > arr_prefix_max) ||
                        (field_value[0] < field_min_val))
                    {
                        continue;
                    }
                    else
                    {
                        found_valid = TRUE;
                    }
                }

                if ((field_info->enum_val_info[field_value[0]].value < utilex_power_of_2(field_len)) &&
                    (field_value[0] >= field_min_val))
                {
                    found_valid = TRUE;
                }
            }
            break;
        }

        case DBAL_FIELD_PRINT_TYPE_NONE:
        case DBAL_NOF_FIELD_PRINT_TYPES:
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field type\n");
            break;
    }

    if (field_info->print_type == DBAL_FIELD_PRINT_TYPE_STRING)
    {
        uint8 *ptr = (uint8 *) field_value;
        ptr[BITS2BYTES(field_len)] = '\0';
    }

    /*
     * WA for MACT dynamic entries, in order to run on device correctly.
     * It is illegal to insert entry with payload length >  64 and indication of dynamic entry.
     * Should be handled correctly using XMLs (limiting values per result type
     */
    if (field_id == DBAL_FIELD_MAC_STRENGTH)
    {
        field_value[0] = DBAL_ENUM_FVAL_MAC_STRENGTH_2;
    }

exit:
    SHR_FUNC_EXIT;
}

cmd_result_t
diag_dbal_alloc_dealloc_value(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    int result_type,
    int core_id,
    int element,
    int is_alloc)
{
    uint8 is_allocated;
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    /**check if it is allocated at all   */
    SHR_IF_ERR_EXIT(dnx_algo_res_dbal_is_allocated(unit, core_id,
                                                   0, element, table_id, result_type, field_id, 1, &is_allocated));

        /** for fec we only need to check if the first fec is allocated */
    if ((field_type == DBAL_FIELD_TYPE_DEF_SUPER_FEC_ID) && (is_allocated == 2))
    {
                /**in-case that the fec that is allocated is only the second FEC dbal can use this fec in the test */
        is_allocated = 0;
    }

    /**LOG_CLI((BSL_META("%sAlloc value %d field %s\n"),
             is_alloc ? "" : "De-", element, dbal_field_to_string(unit, field_id)));*/

    if (is_alloc)
    {
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_free(unit, core_id, 0, element, table_id, result_type, field_id));
        }
        SHR_IF_ERR_EXIT(dnx_algo_res_dbal_alloc(unit, core_id, 0, element, table_id, result_type, field_id));
    }
    else
    {
        /** can be that the resource was released before (in_lif alloc is index/3) */
        if (is_allocated)
        {
            SHR_IF_ERR_EXIT(dnx_algo_res_dbal_free(unit, core_id, 0, element, table_id, result_type, field_id));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
/**
 * /brief - checks if error recovery should be used for cleanup
 */
static uint8
diag_dbal_test_logical_table_cleanup_is_err_recovery(
    int unit,
    uint32 flags)
{
    return (flags & LTT_FLAGS_ERR_RECOVERY_CLEAN_UP) && (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST);
}

/**
 * /brief - checkls if snapshot negative flags are on
 */
static uint8
diag_dbal_test_logical_table_cleanup_is_snapshot_negative(
    int unit,
    uint32 flags)
{
    return ((flags & LTT_FLAGS_SNAPSHOT_NEGATIVE) != 0);
}
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

static void
diag_dbal_ltt_test_params_init(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_tables_e table_id,
    int max_capacity,
    uint32 *flags,
    int *nof_entries,
    uint32 *seed)
{
    uint32 flags_local;
    int nof_entries_local = 0;
    uint32 seed_local = 0;

    flags_local = *flags;

    /** Set flags according to dependencies flags*/
    if (flags_local & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST)
    {
        flags_local |= LTT_FLAGS_RUN_ITERATOR;
        flags_local |= LTT_FLAGS_TABLE_CLEAR_END_OF_TEST;
    }

    if (flags_local & LTT_FLAGS_RUN_TEST_WITH_3_ENTRIES)
    {
        nof_entries_local = 3;
    }
    else
    {
        if (max_capacity == 0)
        {
            nof_entries_local = DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE;
        }
        else
        {
            nof_entries_local = UTILEX_MIN(max_capacity, DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE);
        }
    }

    /** Initialize random seed value */
    if (flags_local & LTT_FLAGS_RANDOM_VALUES)
    {
        seed_local = sal_time_usecs();
    }
    else
    {
        seed_local = 1000 + table_id;
    }
    sal_srand(seed_local);

    *flags = flags_local;
    *nof_entries = nof_entries_local;
    *seed = seed_local;
}

/**
 * \brief - Allocate key fields for EGRESS_PP_PORT
 *      The key to EGRESS_PP_PORT should be valid pp_port.
 *      The function gets 32 valid pp_ports from valid logical ports and use them as keys to the table.
 *
 * \param [in] unit - Relevant unit
 * \param [in] table - Logical table structure definition
 * \param [in] table_id - Table_id
 * \param [in] max_entries_in_test - Maximum number of entries in the test
 * \param [in] final_nof_entries - The number of entries without invalid entries
 * \param [in] final_invalid_entries - Number of invalid entries
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
diag_dbal_egress_pp_port_test_key_fields_allocation(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_tables_e table_id,
    int max_entries_in_test,
    int *final_nof_entries,
    int *final_invalid_entries)
{
    bcm_pbmp_t valid_ports;
    uint32 *egress_pp_port_arr_p = NULL;
    uint32 *egress_pp_port_core_id_arr_p = NULL;

    uint32 pp_port;
    bcm_core_t core_id;
    bcm_port_t valid_port;
    int index = 0;
    int nof_entries = 0;
    uint8 is_illegal_value = FALSE;
    int ii;
    int jj = 0;
    int invalid_entries_counter = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC_SET_ZERO(egress_pp_port_arr_p, sizeof(*egress_pp_port_arr_p) * max_entries_in_test,
                       "egress_pp_port_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
    SHR_ALLOC_SET_ZERO(egress_pp_port_core_id_arr_p, sizeof(*egress_pp_port_core_id_arr_p) * max_entries_in_test,
                       "egress_pp_port_core_id_arr_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    nof_entries = max_entries_in_test;
    /** Get all valid logical ports */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_PP, 0, &valid_ports));
    BCM_PBMP_ITER(valid_ports, valid_port)
    {
        /** Get the pp port and the core id and keep their values in arrays*/
        if (index < nof_entries)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get(unit, valid_port, &core_id, &pp_port));
            egress_pp_port_arr_p[index] = pp_port;
            egress_pp_port_core_id_arr_p[index] = core_id;
            index++;
        }
        else
        {
            break;
        }
    }

    for (ii = 0; ii < nof_entries; ii++)
    {
        /** Set pp_port */
        key_field_val[ii][jj][0] = egress_pp_port_arr_p[ii];
        /** Set core_id */
        key_field_val[ii][jj + 1][0] = egress_pp_port_core_id_arr_p[ii];

        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, table->keys_info[jj].field_id,
                                                     key_field_val[ii][jj][0], &is_illegal_value));
        if (is_illegal_value)
        {
            valid_key_value[ii] = FALSE;
            continue;
        }
    }
    for (ii = 0; ii < nof_entries; ii++)
    {
        /** Validate that there are no entries duplication */
        if (valid_key_value[ii] == FALSE)
        {
            continue;
        }
        for (jj = ii + 1; jj < nof_entries; jj++)
        {
            if (sal_memcmp(key_field_val[ii], key_field_val[jj],
                           (sizeof(key_field_val) / (DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE * sizeof(uint32)))) == 0)
            {
                valid_key_value[jj] = FALSE;
                invalid_entries_counter++;
            }
        }

    }
    *final_invalid_entries = invalid_entries_counter;
    *final_nof_entries = nof_entries;
exit:
    SHR_FREE(egress_pp_port_arr_p);
    SHR_FREE(egress_pp_port_core_id_arr_p);
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_test_key_fields_allocation(
    int unit,
    CONST dbal_logical_table_t * table,
    dbal_tables_e table_id,
    int max_entries_in_test,
    int *final_nof_entries,
    int *final_invalid_entries)
{
    int ii, jj;
    int key_size = 0;
    int nof_entries = 0;
    int invalid_entries_counter = 0;

    SHR_FUNC_INIT_VARS(unit);

    key_size = table->key_size;

    if ((table_id != DBAL_TABLE_EGRESS_PP_PORT)
        || ((table_id == DBAL_TABLE_EGRESS_PP_PORT)
            && !(SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE))))
    {
        if (key_size < 8)
        {
            /*
             * if key is less than 8 bits, fill all table, with all valid
             * keys values (or up to 32 different values)
             */
            int total_bits_before_field = 0;
            nof_entries = UTILEX_MIN(max_entries_in_test, (1 << key_size));
            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                uint32 cur_field_val = 0;
                int field_nof_values = utilex_power_of_2(table->keys_info[jj].field_nof_bits);
                dbal_field_types_basic_info_t *field_info;

                SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, table->keys_info[jj].field_id, &field_info));

                if (field_nof_values <= 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Something went wrong. field_nof_values cannot be zero. table %s\n",
                                 table->table_name);
                }

                for (ii = 0; ii < nof_entries; ii++)
                {
                    uint8 is_illegal = FALSE;
                    uint8 need_to_allocate = TRUE;
                    int value_to_allocate = ii;

                    while (need_to_allocate)
                    {
                        cur_field_val = ((value_to_allocate >> total_bits_before_field) % field_nof_values);
                        key_field_val[ii][jj][0] = cur_field_val;

                        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, table->keys_info[jj].field_id, cur_field_val,
                                                                     &is_illegal));

                        if (field_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                        {
                            if (field_info->enum_val_info[cur_field_val].is_invalid)
                            {
                                is_illegal = 1;
                            }
                        }

                        if ((cur_field_val > table->keys_info[jj].max_value) ||
                            (cur_field_val < table->keys_info[jj].min_value) || is_illegal ||
                            ((field_info->print_type == DBAL_FIELD_PRINT_TYPE_ENUM) &&
                             ((cur_field_val >= field_info->nof_enum_values) ||
                              (field_info->enum_val_info[cur_field_val].value >= field_nof_values))))
                        {
                            /*
                             * When field is enum, the entry is invalid if (at least one):
                             * 1. the field value is equal or greater then num of enum vals
                             * 2. the hw value is equal or greater then the field max value by range
                             * If field has max_value indication, its value cannot be larger than it
                             */
                            value_to_allocate++;
                        }
                        else
                        {
                            need_to_allocate = FALSE;
                        }
                    }
                }
                total_bits_before_field += table->keys_info[jj].field_nof_bits;
            }
        }
        else
        {
            /*
             * for 9bit and larger key size, check 32 entries.
             * min entry, max entry, 30 random entries
             */
            nof_entries = max_entries_in_test;
            for (ii = 0; ii < nof_entries; ii++)
            {
                for (jj = 0; jj < table->nof_key_fields; jj++)
                {
                    fields_values_types_e field_value_type;
                    if (ii == 0)
                    {
                        field_value_type = MIN_FIELD_VAL;
                    }
                    else if (ii == (nof_entries - 1))
                    {
                        field_value_type = MAX_FIELD_VAL;
                    }
                    else
                    {
                        field_value_type = RANDOM_FIELD_VAL;
                    }
                    SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field(unit, table->keys_info[jj].field_id,
                                                                    field_value_type,
                                                                    table->keys_info[jj].min_value,
                                                                    table->keys_info[jj].max_value,
                                                                    table->keys_info[jj].field_nof_bits +
                                                                    table->keys_info[jj].offset_in_logical_field,
                                                                    TRUE, 0, 0, key_field_val[ii][jj]));

                    /** Check if we got an illegal value for the field  */
                    {
                        uint8 is_illegal_value = FALSE;
                        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, table->keys_info[jj].field_id,
                                                                     key_field_val[ii][jj][0], &is_illegal_value));
                        if (is_illegal_value)
                        {
                            valid_key_value[ii] = FALSE;
                            continue;
                        }
                    }

                    if (table->access_method == DBAL_ACCESS_METHOD_MDB)
                    {
                    /** In case of direct MDB table, the entries are limited to max capacity */
                        dbal_physical_table_def_t *physical_table;
                        SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX],
                                                                &physical_table));

                        if (table->table_type == DBAL_TABLE_TYPE_DIRECT && !(dbal_logical_table_is_fec(unit, table)))
                        {
                            /*
                             * There is an assumption here:
                             * Key field size is smaller than 32 bits
                             */
                            int basic_payload_len = 0;
                            int payload_size = 0;
                            int index_factor = 0;

                            /** for MDB direct, the max value of the field can be bigger than the actual legal value since it is depended on the
                             *  MDB profile. the solution for that is to check the capacity and the assumption is that the max values is is the
                             *  size of the capacity. (for fec tables we don't need this assumption because the min/max value is updated) */
                            if (!(dbal_logical_table_is_out_rif_allocator(table) ||
                                  dbal_logical_table_is_out_lif_allocator_eedb(table)))
                            {
                                int capacity = 0;
                                SHR_IF_ERR_EXIT(mdb_db_infos.
                                                capacity.get(unit, table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX],
                                                             &capacity));
                                key_field_val[ii][jj][0] = key_field_val[ii][jj][0] % capacity;

                                /*
                                 * This API should set in basic_payload_len the length in bits
                                 */
                                SHR_IF_ERR_EXIT(mdb_direct_table_get_basic_size_using_logical_mdb_table
                                                (unit, table->physical_db_id[DBAL_PHY_DB_DEFAULT_INDEX],
                                                 &basic_payload_len));
                                /*
                                 * Compute the max payload length of the table
                                 */
                                payload_size = table->max_payload_size;

                                /*
                                 * translate key to valid value, according to the payload size
                                 * and basic payload size ratio
                                 */
                                if (dbal_physical_table_is_in_lif(table->physical_db_id[0]))
                                {
                                /**
                                 *  Due to in lif allocation allocate 2 entries of 3 indexes or 3 entries of 2 indexes per allocation,
                                 *  Use 6 as index factor, to make the lifs compatible wit all result types of the tables
                                 */
                                    index_factor = 6;
                                }
                                else
                                {
                                    index_factor = (payload_size + basic_payload_len - 1) / (basic_payload_len);
                                }
                                key_field_val[ii][jj][0] /= index_factor;
                                key_field_val[ii][jj][0] *= index_factor;
                                if (capacity - key_field_val[ii][jj][0] < index_factor)
                                {
                                    key_field_val[ii][jj][0] -= index_factor;
                                }
                            }
                            else
                            {
                                if (key_field_val[ii][jj][0] < 4)
                                {
                                    key_field_val[ii][jj][0] += 4;
                                }
                                key_field_val[ii][jj][0] /= 4;
                                key_field_val[ii][jj][0] *= 4;
                            }
                        }
                        else
                        {
                            if (table_id == DBAL_TABLE_OAMP_RMEP_STATIC_DATA_DB)
                            {
                                key_field_val[ii][jj][0] /= 2;
                                key_field_val[ii][jj][0] *= 2;
                            }
                        }
                    }
                    else if (table_id == DBAL_TABLE_OAMP_MEP_DB)
                    {
                        key_field_val[ii][jj][0] /= 4;
                        key_field_val[ii][jj][0] *= 4;
                    }
                    else if (table_id == DBAL_TABLE_TUNNEL_SOURCE_ADDRESS_IPV6)
                    {
                        key_field_val[ii][jj][0] /= 4;
                        key_field_val[ii][jj][0] *= 4;
                    }
                    else if ((table_id == DBAL_TABLE_OAMP_RMEP_DB)
                             || (table_id == DBAL_TABLE_OAM_REMOTE_ENDPOINT_INFO_SW))
                    {
                        key_field_val[ii][jj][0] /= 2;
                        key_field_val[ii][jj][0] *= 2;
                    }
                }
            }
        }

        if (table->core_mode == DBAL_CORE_MODE_SBC)
        {
            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                if (table->keys_info[jj].field_id == DBAL_FIELD_CORE_ID)
                {
                    for (ii = 0; ii < nof_entries; ii++)
                    {
                        key_field_val[ii][jj][0] = DBAL_CORE_ALL;
                    }
                }

            }

        }

        for (ii = 0; ii < nof_entries; ii++)
        {
        /** validate that there are no entries duplication */
            if (valid_key_value[ii] == FALSE)
            {
                continue;
            }
            for (jj = ii + 1; jj < nof_entries; jj++)
            {
                if (sal_memcmp(key_field_val[ii], key_field_val[jj],
                               (sizeof(key_field_val) / (DIAG_DBAL_MIN_NOF_ENTRIES_PER_TABLE * sizeof(uint32)))) == 0)
                {
                    valid_key_value[jj] = FALSE;
                    invalid_entries_counter++;
                }
            }

            if (dbal_physical_table_is_in_lif(table->physical_db_id[0]))
            {
                if (key_field_val[ii][0][0] < DIAG_DBAL_MIN_IN_LIF_TO_ALLOCATE)
                {
                    valid_key_value[ii] = FALSE;
                    invalid_entries_counter++;
                }
            }
        }
        *final_invalid_entries = invalid_entries_counter;
        *final_nof_entries = nof_entries;
    }
    else
    {
        /** The key for EGRESS_PP_PORT should be valid pp_port and that`s why it is used additional function for it */
        SHR_IF_ERR_EXIT(diag_dbal_egress_pp_port_test_key_fields_allocation
                        (unit, table, table_id, max_entries_in_test, final_nof_entries, final_invalid_entries));
    }

exit:
    SHR_FUNC_EXIT;
}

static int
dbal_diag_test_find_core_id_val(
    int unit,
    int entry)
{
    int ii;
    for (ii = 0; ii < fields_comb.nof_fields; ii++)
    {
        if ((fields_comb.fields_is_key[entry][ii]) && (fields_comb.fields_id[entry][ii] == DBAL_FIELD_CORE_ID))
        {
            return (int) key_field_val[entry][ii][0];
        }
    }
    return _SHR_CORE_ALL;
}

static shr_error_e
dbal_diag_test_combination_build(
    int unit,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int res_type_idx,
    uint8 *has_result_type)
{
    int jj;

    uint8 has_result_field_to_write = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    fields_comb.nof_fields = 0;
    fields_comb.nof_combinations = 1;

    for (jj = 0; jj < table->nof_key_fields;)
    {
        int nof_instances = 0;
        SHR_IF_ERR_EXIT(diag_dbal_test_add_field_to_combinations(unit, table_id, &table->keys_info[jj], TRUE,
                                                                 res_type_idx, &nof_instances));
        jj += nof_instances;
    }
    for (jj = 0; jj < table->multi_res_info[res_type_idx].nof_result_fields;)
    {
        int nof_instances = 0;

        SHR_IF_ERR_EXIT(diag_dbal_test_add_field_to_combinations
                        (unit, table_id, &table->multi_res_info[res_type_idx].results_info[jj], FALSE, res_type_idx,
                         &nof_instances));
        if ((table->multi_res_info[res_type_idx].results_info[jj].field_id != DBAL_FIELD_RESULT_TYPE)
            && (table->multi_res_info[res_type_idx].results_info[jj].permission != DBAL_PERMISSION_READONLY))
        {
            has_result_field_to_write = TRUE;
        }
        jj += nof_instances;
    }
    *has_result_type = has_result_field_to_write;

    /** If there is more than one combination - print out all combinations */
    if (fields_comb.nof_combinations > 1)
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Checking %d fields combinations:\n"),
                                  fields_comb.nof_combinations));
        print_fields_combinations(unit, &fields_comb);
    }

exit:
    SHR_FUNC_EXIT;

}
static shr_error_e
dbal_diag_test_entry_commit(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int index,
    uint8 last_entry,
    int res_type_idx,
    int *invalid_entries,
    uint8 is_error_recovery_overwritten_entry)
{
    int ii = index, jj;
    int comb_index;
    int invalid_entries_counter = *invalid_entries;
    dbal_entry_handle_t *entry_handle;
    int core = DBAL_CORE_ALL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry %d: "), ii));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    if (fields_comb.nof_combinations == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal nof field combinations: 0 \n");
    }

    comb_index = index % fields_comb.nof_combinations;

    if (DBAL_TABLE_IS_TCAM_BY_ID(table, 1))
    {
        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, ii, &access_id[ii]));
        }
        else
        {
            access_id[ii] = ii;
        }

        /** Committing for TCAM requires access id, for KBP ACL tables, working only without ID so the entry ID retrieved
         *  after insertion  */
        if (table->access_method != DBAL_ACCESS_METHOD_KBP)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id[ii]));
        }
    }

    for (jj = 0; jj < fields_comb.nof_fields; jj++)
    {
        if (fields_comb.fields_is_key[comb_index][jj])
        {
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "key%d=0x%x "), jj, key_field_val[ii][jj][0]));

            if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
            {
                uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                dbal_fields_transform_arr32_to_arr8(unit,
                                                    table_id, fields_comb.fields_id[comb_index][jj],
                                                    fields_comb.fields_is_key[comb_index][jj], res_type_idx,
                                                    fields_comb.instance_idx[comb_index][jj], key_field_val[ii][jj],
                                                    field_val_as_u8, NULL, NULL);

                dbal_entry_key_field_arr8_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                              field_val_as_u8);
            }
            else
            {
                /** handle allocator */
                if (SHR_BITGET(table->keys_info[jj].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR) &&
                    (!dbal_table_is_allcator_not_needed(unit, table_id)))
                {
                    int core_val = dbal_diag_test_find_core_id_val(unit, index);
                    SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, fields_comb.fields_id[comb_index][jj],
                                                                  table_id, res_type_idx, core_val,
                                                                  key_field_val[ii][jj][0], 1));
                }
                dbal_entry_key_field_arr32_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                               key_field_val[ii][jj]);
            }
        }
        else
        {
            fields_values_types_e type_of_value = RANDOM_FIELD_VAL;

            int res_idx = jj - table->nof_key_fields;
            if (last_entry)
            {
                type_of_value = MAX_FIELD_VAL;

                /*
                 * If only one entry in table is allowed, error recovery inserts min value for the first value field,
                 * and max value for the rest of the value fields, if they exist.
                 * This value is overwritten in the transaction by main algorithm.
                 * If there is only 1 result field to this entry max value is used.
                 */
                if (is_error_recovery_overwritten_entry && (index == 0) && (res_idx == 0)
                    && (table->multi_res_info[res_type_idx].nof_result_fields != 1))
                {
                    type_of_value = MIN_FIELD_VAL;
                }
            }

            if (table->multi_res_info[res_type_idx].results_info[res_idx].permission == DBAL_PERMISSION_READONLY)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dbal_fields_get_value_for_field(unit, fields_comb.fields_id[comb_index][jj],
                                                            type_of_value,
                                                            fields_comb.fields_min_val[comb_index][jj],
                                                            fields_comb.fields_max_val[comb_index][jj],
                                                            fields_comb.fields_bit_length[comb_index][jj] +
                                                            fields_comb.fields_bit_offset[comb_index][jj],
                                                            FALSE,
                                                            table->multi_res_info[res_type_idx].
                                                            results_info[res_idx].arr_prefix,
                                                            table->multi_res_info[res_type_idx].
                                                            results_info[res_idx].arr_prefix_size,
                                                            result_field_val[ii][res_idx]));
            /*
             * If it is a multiple results type table, set the RESULT_TYPE
             * field to the result type index
             */
            if (fields_comb.fields_id[comb_index][jj] == DBAL_FIELD_RESULT_TYPE)
            {
                result_field_val[ii][res_idx][0] = res_type_idx;
            }

            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "res%d=0x%x "), res_idx, result_field_val[ii][res_idx][0]));

            if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
            {
                uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                dbal_fields_transform_arr32_to_arr8(unit,
                                                    table_id, fields_comb.fields_id[comb_index][jj],
                                                    fields_comb.fields_is_key[comb_index][jj], res_type_idx,
                                                    fields_comb.instance_idx[comb_index][jj],
                                                    result_field_val[ii][res_idx], field_val_as_u8, NULL, NULL);

                dbal_entry_value_field_arr8_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                fields_comb.instance_idx[comb_index][jj], field_val_as_u8);
            }
            else
            {
                dbal_entry_value_field_arr32_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                 fields_comb.instance_idx[comb_index][jj],
                                                 result_field_val[ii][res_idx]);
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n\n")));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    /** in case of non standard default entry validating that the entry added is not default */
    if (table->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC &&
        table->hl_mapping_multi_res[entry_handle->cur_res_type].is_default_non_standard)
    {
        int is_default;
        SHR_IF_ERR_EXIT(dbal_hl_is_entry_default(unit, entry_handle, &is_default));

        if (is_default && (!is_error_recovery_overwritten_entry))
        {
            valid_key_value[ii] = FALSE;
            invalid_entries_counter++;
            *invalid_entries = invalid_entries_counter;
            SHR_EXIT();
        }
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id,
                                      dbal_tables_is_non_direct(unit, table_id) ? DBAL_COMMIT_FORCE : DBAL_COMMIT));

    if ((table->access_method == DBAL_ACCESS_METHOD_KBP) && (table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID))
    {
        /** for KBP ACL tables, working only without ID so the entry ID retrieved after insertion */
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_get(unit, entry_handle_id, &(access_id[ii])));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_diag_test_entry_get(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int res_type_idx,
    int index)
{
    int ii = index, jj, kk;
    int comb_index = index % fields_comb.nof_combinations;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "entry %d: "), ii));
    sal_memset(result_field_val_arr8_get, 0, sizeof(result_field_val_arr8_get));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    if (DBAL_TABLE_IS_TCAM_BY_ID(table, 0))
    {
        /** getting entry for TCAM is by access id  */
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id[ii]));
    }

    for (jj = 0; jj < fields_comb.nof_fields; jj++)
    {
        int res_idx = jj - table->nof_key_fields;

        if (fields_comb.fields_is_key[comb_index][jj])
        {
            if (!DBAL_TABLE_IS_TCAM_BY_ID(table, 0))
            {
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "key%d=0x%x "), jj, key_field_val[ii][jj][0]));
                if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
                {
                    uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                    dbal_fields_transform_arr32_to_arr8(unit,
                                                        table_id, fields_comb.fields_id[comb_index][jj],
                                                        fields_comb.fields_is_key[comb_index][jj], res_type_idx,
                                                        fields_comb.instance_idx[comb_index][jj], key_field_val[ii][jj],
                                                        field_val_as_u8, NULL, NULL);
                    dbal_entry_key_field_arr8_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                  field_val_as_u8);
                }
                else
                {
                    dbal_entry_key_field_arr32_set(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                   key_field_val[ii][jj]);
                }
            }
        }
        else
        {
            /*
             * don't request write only or trigger fields, skip them
             */
            if ((table->multi_res_info[res_type_idx].results_info[res_idx].permission == DBAL_PERMISSION_WRITEONLY)
                || (table->multi_res_info[res_type_idx].results_info[res_idx].permission == DBAL_PERMISSION_TRIGGER))
            {
                continue;
            }

            /*
             * If the table is a single result type table: use regular procedure
             * If table has multiple result types, use GET ALL FIELDS flags and parse fields
             */
            if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
            {
                if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
                {
                    dbal_value_field_arr8_request(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                  fields_comb.instance_idx[comb_index][jj],
                                                  result_field_val_arr8_get[res_idx]);
                }
                else
                {
                    dbal_value_field_arr32_request(unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                                   fields_comb.instance_idx[comb_index][jj],
                                                   result_field_val_get[ii][res_idx]);
                }
            }
            else
            {
                if ((dbal_physical_table_is_in_lif(table->physical_db_id[0])) ||
                    (dbal_logical_table_is_out_rif_allocator(table)) ||
                    (dbal_logical_table_is_out_lif_allocator_eedb(table)))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                 res_type_idx);
                }
                break;
            }
        }
    }
    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n\n")));
    SHR_IF_ERR_EXIT(dbal_entry_get
                    (unit, entry_handle_id,
                     SHR_IS_BITSET(table->indications_bm,
                                   DBAL_TABLE_IND_HAS_RESULT_TYPE) ? DBAL_GET_ALL_FIELDS : DBAL_COMMIT));

    /*
     * Get relevant field values
     */
    for (jj = 0; jj < fields_comb.nof_fields; jj++)
    {
        int res_idx = jj - table->nof_key_fields;

        if (fields_comb.fields_is_key[comb_index][jj])
        {
            if (DBAL_TABLE_IS_TCAM(table))
            {
                /** Get all key values */
                LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "key%d=0x%x "), jj, key_field_val[ii][jj][0]));
                if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
                {
                    uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get
                                    (unit, entry_handle_id, fields_comb.fields_id[comb_index][jj], field_val_as_u8));
                    SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                                    (unit,
                                     table_id, fields_comb.fields_id[comb_index][jj],
                                     fields_comb.fields_is_key[comb_index][jj], res_type_idx,
                                     fields_comb.instance_idx[comb_index][jj], field_val_as_u8,
                                     key_field_val_get[ii][jj], NULL, NULL));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, fields_comb.fields_id[comb_index][jj],
                                     key_field_val_get[ii][jj]));
                }
            }
        }
        else if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {

            /*
             * don't get write only or trigger fields, skip them
             */
            if ((table->multi_res_info[res_type_idx].results_info[res_idx].permission == DBAL_PERMISSION_WRITEONLY)
                || (table->multi_res_info[res_type_idx].results_info[res_idx].permission == DBAL_PERMISSION_TRIGGER))
            {
                continue;
            }

            if (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][jj]))
            {
                uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get(unit, entry_handle_id,
                                                                       fields_comb.fields_id[comb_index][jj],
                                                                       fields_comb.instance_idx[comb_index]
                                                                       [jj], field_val_as_u8));
                SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                                (unit, table_id, fields_comb.fields_id[comb_index][jj],
                                 fields_comb.fields_is_key[comb_index][jj], res_type_idx,
                                 fields_comb.instance_idx[comb_index][jj], field_val_as_u8,
                                 result_field_val_get[ii][res_idx], NULL, NULL));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                        fields_comb.fields_id[comb_index][jj],
                                                                        fields_comb.instance_idx[comb_index]
                                                                        [jj], result_field_val_get[ii][res_idx]));
            }
        }
    }
    /** transforming the special fields that was using arr8... */
    if (!SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
    {
        for (kk = 0; kk < fields_comb.nof_fields; kk++)
        {
            int res_idx = kk - table->nof_key_fields;
            if ((!fields_comb.fields_is_key[comb_index][kk]) &&
                (diag_dbal_is_arr8_special_field(unit, fields_comb.fields_id[comb_index][kk])))
            {
                SHR_IF_ERR_EXIT(dbal_fields_transform_arr8_to_arr32
                                (unit, table_id, fields_comb.fields_id[comb_index][kk],
                                 fields_comb.fields_is_key[comb_index][kk], res_type_idx,
                                 fields_comb.instance_idx[comb_index][kk], result_field_val_arr8_get[res_idx],
                                 result_field_val_get[ii][res_idx], NULL, NULL));
            }
        }
    }

    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "\n\n")));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** Iterate over all entries and compare to previously added entries*/
static shr_error_e
dbal_diag_test_match_entries_by_iter(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int nof_entries,
    int res_type_idx,
    int *entry_counter)
{
    int ii, jj, is_end = 0;
    int ent_counter = 0;
    uint8 found_entry = FALSE;
    dbal_entry_handle_t *entry_handle;
    CONST dbal_table_field_info_t *results_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    results_info = table->multi_res_info[res_type_idx].results_info;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_NON_DEFAULT_PERMISSION_WRITE));
    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

    /*
     * iterator main loop
     * get next entry and find the match entry in result buffer
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        found_entry = FALSE;
        for (ii = 0; ii < nof_entries; ii++)
        {
            int comb_index = ii % fields_comb.nof_combinations;

            for (jj = 0; jj < fields_comb.nof_fields; jj++)
            {
                if (fields_comb.fields_is_key[comb_index][jj])
                {
                    continue;
                }
                else
                {
                    int res_idx = jj - table->nof_key_fields;
                    dbal_field_types_basic_info_t *field_type_info;

                    /*
                     * don't request write only fields and trigger fields, skip them
                     */
                    if ((results_info[res_idx].permission == DBAL_PERMISSION_WRITEONLY)
                        || (results_info[res_idx].permission == DBAL_PERMISSION_TRIGGER))
                    {
                        continue;
                    }

                    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                                    (unit, results_info[res_idx].field_id, &field_type_info));

                    if (field_type_info->nof_child_fields > 0)
                    {
                        /** if there are sub-fields here, parse the result*/
                        dbal_fields_e sub_field_id = DBAL_FIELD_EMPTY;
                        uint32 sub_field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

                        sal_memset(sub_field_value, 0, sizeof(sub_field_value));

                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                                results_info[res_idx].field_id,
                                                                                INST_SINGLE,
                                                                                iterator_parsed_fields[res_idx]));

                        SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode(unit,
                                                                        results_info[res_idx].field_id,
                                                                        iterator_parsed_fields[res_idx],
                                                                        &sub_field_id, sub_field_value));
                        if (sub_field_id != DBAL_FIELD_EMPTY)
                        {
                            sal_memcpy(iterator_parsed_fields[res_idx], sub_field_value,
                                       DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS * sizeof(uint32));
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "error parsing field %s\n", field_type_info->name);
                        }
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id,
                                                                                results_info[res_idx].field_id,
                                                                                INST_SINGLE,
                                                                                iterator_parsed_fields[res_idx]));
                    }
                }
            }

            if (compare_fields_values(unit, table, result_field_val[ii], iterator_parsed_fields, res_type_idx, 0) == 0)
            {
                if (iterator_found_indications[ii] == TRUE)
                {
                    continue;
                }
                if (valid_key_value[ii])
                {
                    LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Iterate #%d match to entry #%d\n"),
                                                 ent_counter, ii));
                    iterator_found_indications[ii] = TRUE;
                    found_entry = TRUE;
                    break;
                }
            }
        }

        if (found_entry == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Iterated entry #%d was not found in entries DB of the test. table %s\n", ent_counter,
                         table->table_name);
        }
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        ent_counter++;
    }

    *entry_counter = ent_counter;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_diag_test_entry_clear(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int result_type,
    int index)
{
    int ii = index, jj;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

    if (DBAL_TABLE_IS_TCAM_BY_ID(table, 0))
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_id[ii]));
    }
    else
    {
        for (jj = 0; jj < table->nof_key_fields; jj++)
        {
            if (diag_dbal_is_arr8_special_field(unit, table->keys_info[jj].field_id))
            {
                uint8 field_val_as_u8[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES] = { 0 };
                dbal_fields_transform_arr32_to_arr8(unit, table_id, table->keys_info[jj].field_id, TRUE, 0, 0,
                                                    key_field_val[ii][jj], field_val_as_u8, NULL, NULL);
                dbal_entry_key_field_arr8_set(unit, entry_handle_id, table->keys_info[jj].field_id, field_val_as_u8);
            }
            else
            {
                dbal_entry_key_field_arr32_set(unit, entry_handle_id, table->keys_info[jj].field_id,
                                               key_field_val[ii][jj]);
            }
        }
        if ((dbal_physical_table_is_in_lif(table->physical_db_id[0])) ||
            (dbal_logical_table_is_out_rif_allocator(table)) || (dbal_logical_table_is_out_lif_allocator_eedb(table)))
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type);
        }
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

    if ((DBAL_TABLE_IS_TCAM(table)) && (table->access_method == DBAL_ACCESS_METHOD_MDB))
    {
        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy(unit, table_id, access_id[ii]));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
/**
 * /brief - prerequisites for the error recovery cleanup.
 */
static shr_error_e
diag_dbal_test_err_recovery_cleanup_preq(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int res_type_idx,
    int nof_entries,
    int *counter,
    int *err_recovery_test_valid_entry_idx,
    uint32 flags)
{
    int invalid_entries_counter = 0;
    int ii = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * should be called only if error recovery cleanup
     */
    if (!diag_dbal_test_logical_table_cleanup_is_err_recovery(unit, flags))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot use error recovery cleanup\n");
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Adding one entry to to dbal table prior to transaction start")));

    /*
     * find the first valid key value
     */
    for (ii = 0; ii < nof_entries; ii++)
    {

        /*
         * don't add entry for tcam corner cases
         */
        /**  Currently SDK is not supporting entry update for TCAM by ID. So we don't need to check the update of an entry. */
        if ((table->table_type == DBAL_TABLE_TYPE_TCAM_BY_ID))
        {
            break;
        }

        /**  Currently SDK is not supporting entry update for MDB TCAM. So we don't need to check the update of an entry. */
        if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type == DBAL_TABLE_TYPE_TCAM))
        {
            break;
        }

        if (valid_key_value[ii])
        {
            /*
             * insert one entry in before in table before erorr recovery transaction, that is guaranteed to be overwritten during the transaction.
             * entry will not be deleted by the error recovery transaction rollback, it is deleted manually after the error_recovery transaction.
             */
            SHR_IF_ERR_EXIT(dbal_diag_test_entry_commit
                            (unit, entry_handle_id, table_id, table, ii, ii == (nof_entries - 1), res_type_idx,
                             &invalid_entries_counter, TRUE));

            /*
             * save index for later disposal
             */
            *err_recovery_test_valid_entry_idx = ii;
            break;
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Get number of entries prior to transaction start\n")));

    SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, counter));

    /*
     * start snapshot manager
     */
    DNX_STATE_COMPARISON_START(unit);

    /*
     * start error recovery transaction
     */
    DNX_ROLLBACK_JOURNAL_START(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
 * /brief - delete all entries using error recovery cleanup
 */
static shr_error_e
diag_dbal_test_err_recovery_cleanup(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int res_type_idx,
    int pre_transaction_count,
    int err_recovery_test_valid_entry_idx,
    uint32 flags)
{
    int counter = -1;
    int jj = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * should be called only if error recovery cleanup
     */
    if (!diag_dbal_test_logical_table_cleanup_is_err_recovery(unit, flags))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot use error recovery cleanup\n");
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Err recovery rollback\n")));

    /*
     * clear the entry that was overwritten.
     * used for validation entry clean can be successfully rolled back.
     * Don't clear entry for cornercases for which no entry was added.
     */
    if ((err_recovery_test_valid_entry_idx >= 0) && valid_key_value[err_recovery_test_valid_entry_idx])
    {
        SHR_IF_ERR_EXIT(dbal_diag_test_entry_clear(unit,
                                                   entry_handle_id,
                                                   table_id, table, res_type_idx, err_recovery_test_valid_entry_idx));
    }

    /*
     * cleanup using error recovery
     */
    DNX_ROLLBACK_JOURNAL_END_AND_CLEAR(unit);

    /*
     * check system state using snapshot manager
     */
    DNX_STATE_COMPARISON_END_AND_COMPARE(unit);

    if (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST)
    {

        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Verify all entries were deleted\n")));

        SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, &counter));

        if (counter != pre_transaction_count)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found %d entries in table, expected %d. table %s\n",
                         counter, pre_transaction_count, dbal_logical_table_to_string(unit, table_id));
        }

        /*
         * delete test entry, if it was added in prior to the transaction
         */
        if (err_recovery_test_valid_entry_idx >= 0)
        {
            SHR_IF_ERR_EXIT(dbal_diag_test_entry_clear
                            (unit, entry_handle_id, table_id, table, res_type_idx, err_recovery_test_valid_entry_idx));

            /*
             * deallocate resources associated with the inserted entry
             */
            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                if (SHR_BITGET(table->keys_info[jj].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR)
                    && (!dbal_table_is_allcator_not_needed(unit, table_id)))
                {
                    int core_val = dbal_diag_test_find_core_id_val(unit, err_recovery_test_valid_entry_idx);
                    SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, table->keys_info[jj].field_id,
                                                                  table_id, res_type_idx, core_val,
                                                                  key_field_val[err_recovery_test_valid_entry_idx][jj]
                                                                  [0], 0));
                }
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dbal_test_snapshot_manager_check(
    int unit,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (diag_dbal_test_logical_table_cleanup_is_snapshot_negative(unit, flags))
    {
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Expectng system state difference for negative test...\n")));

        DNX_STATE_COMPARISON_END_AND_COMPARE_NEGATIVE(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * /brief - prerequisites for the journaling tests.
 */
static shr_error_e
diag_dbal_test_journaling_preq(
    int unit,
    uint32 entry_handle_id,
    dbal_tables_e table_id,
    CONST dbal_logical_table_t * table,
    int res_type_idx,
    int nof_entries,
    int *counter,
    int *err_recovery_test_valid_entry_idx,
    uint8 *err_recovery_in_transaction,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * valid only for error recovery and snapshot manager testing
     */
    if (!diag_dbal_test_logical_table_cleanup_is_err_recovery(unit, flags)
        && !diag_dbal_test_logical_table_cleanup_is_snapshot_negative(unit, flags))
    {
        SHR_EXIT();
    }

    if (!dnx_dbal_journal_should_create_test(unit, table_id))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL journaling not supported for test\n");
    }

    if (diag_dbal_test_logical_table_cleanup_is_err_recovery(unit, flags))
    {
        SHR_IF_ERR_EXIT(diag_dbal_test_err_recovery_cleanup_preq(unit,
                                                                 entry_handle_id,
                                                                 table_id,
                                                                 table,
                                                                 res_type_idx,
                                                                 nof_entries,
                                                                 counter, err_recovery_test_valid_entry_idx, flags));
        *err_recovery_in_transaction = TRUE;
    }
    else if (diag_dbal_test_logical_table_cleanup_is_snapshot_negative(unit, flags))
    {
        /*
         * start snapshot manager
         */
        DNX_STATE_COMPARISON_START(unit);
    }

exit:
    SHR_FUNC_EXIT;
}
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

static void
diag_dbal_test_print_debug_info(
    int unit,
    CONST dbal_logical_table_t * table,
    int nof_entries,
    int res_type_idx)
{
    int ii;
    multi_res_info_t *res_info;

    LOG_CLI((BSL_META("Test fail for table %s, result type %d , %d entries. test information:\n"),
             table->table_name, res_type_idx, nof_entries));

    res_info = &table->multi_res_info[res_type_idx];
    for (ii = 0; ii < nof_entries; ii++)
    {
        LOG_CLI((BSL_META("Entry %2d - "), ii));
        if (valid_key_value[ii] == TRUE)
        {
            int jj;
            for (jj = 0; jj < table->nof_key_fields; jj++)
            {
                int kk;
                LOG_CLI((BSL_META("key%d=0x"), jj));
                for (kk = BITS2WORDS(table->keys_info[jj].field_nof_bits); kk > 0; kk--)
                {
                    if (kk == BITS2WORDS(table->keys_info[jj].field_nof_bits))
                    {
                        int print_width = (3 + (table->keys_info[jj].field_nof_bits % 32)) / 4;
                        LOG_CLI((BSL_META("%0*x"), print_width, key_field_val[ii][jj][kk - 1]));
                    }
                    else
                    {
                        LOG_CLI((BSL_META("%08x"), key_field_val[ii][jj][kk - 1]));
                    }
                }
                LOG_CLI((BSL_META(", ")));
            }
            for (jj = 0; jj < res_info->nof_result_fields; jj++)
            {
                int kk;
                LOG_CLI((BSL_META("res%d=0x"), jj));
                for (kk = BITS2WORDS(res_info->results_info[jj].field_nof_bits); kk > 0; kk--)
                {
                    if (kk == BITS2WORDS(res_info->results_info[jj].field_nof_bits))
                    {
                        int print_width = (3 + (res_info->results_info[jj].field_nof_bits % 32)) / 4;
                        LOG_CLI((BSL_META("%0*x"), print_width, result_field_val[ii][jj][kk - 1]));
                    }
                    else
                    {
                        LOG_CLI((BSL_META("%08x"), result_field_val[ii][jj][kk - 1]));
                    }
                }
                if (jj == (res_info->nof_result_fields - 1))
                {
                    LOG_CLI((BSL_META("\n")));
                }
                else
                {
                    LOG_CLI((BSL_META(", ")));
                }
            }
        }
        else
        {
            LOG_CLI((BSL_META("Invalid entry (skipped)\n")));
        }
    }
}

shr_error_e
diag_dbal_table_has_standard_entries(
    int unit,
    int entry_handle_id,
    int *has_standard_entries)
{
    dbal_iterator_info_t *iterator_info;
    dbal_entry_handle_t *entry_handle;
    int iter;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));
    SHR_IF_ERR_EXIT(dbal_iterator_info_get(unit, entry_handle_id, &iterator_info));

    *has_standard_entries = FALSE;

    for (iter = 0; iter < entry_handle->table->multi_res_info[entry_handle->cur_res_type].nof_result_fields; iter++)
    {
        if ((entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[iter].permission ==
             DBAL_PERMISSION_READONLY)
            || (entry_handle->table->multi_res_info[entry_handle->cur_res_type].results_info[iter].permission ==
                DBAL_PERMISSION_TRIGGER))
        {
            continue;
        }
        else
        {
            *has_standard_entries = TRUE;
            break;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_test_out_lif_allocate(
    int unit,
    dbal_tables_e table_id,
    int res_type_idx,
    int nof_entries,
    int *invalid_entries_counter)
{
    int ii, jj;
    lif_mngr_local_outlif_info_t outlif_info = { 0 };
    int max_outlif;
    int eedb_bank_size = dnx_data_lif.out_lif.allocation_bank_size_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_mdb.hw_bug.feature_get(unit, dnx_data_mdb_hw_bug_eedb_bank_traffic_lock))
    {
        /*
         * In case of traffic lock bug, only half of the bank can be used 
         */
        eedb_bank_size /= 2;
    }
    max_outlif = eedb_bank_size - 1;

    outlif_info.dbal_table_id = table_id;
    outlif_info.dbal_result_type = res_type_idx;
    outlif_info.outlif_phase = LIF_MNGR_OUTLIF_PHASE_FIRST;
    outlif_info.local_lif_flags = DNX_ALGO_LIF_MNGR_LOCAL_OUTLIF_IGNORE_PHASE_ALLOC_MISMATCH;

    SHR_IF_ERR_EXIT(dnx_lif_lib_allocate(unit, LIF_MNGR_DONT_ALLOCATE_GLOBAL_LIF, NULL, NULL, &outlif_info));
    outlif_base_val = outlif_info.local_outlif;

    for (ii = 0; ii < nof_entries; ii++)
    {
        if (valid_key_value[ii] == TRUE)
        {
            key_field_val[ii][0][0] = (outlif_base_val & ~(max_outlif)) + (key_field_val[ii][0][0] & max_outlif);
            if ((key_field_val[ii][0][0] & max_outlif) > max_outlif - 3)
            {
                key_field_val[ii][0][0] -= 4;
            }

            if (outlif_base_val == key_field_val[ii][0][0])
            {
                valid_key_value[ii] = FALSE;
                (*invalid_entries_counter)++;
                continue;
            }

            for (jj = 0; jj < ii; jj++)
            {
                if (key_field_val[ii][0][0] == key_field_val[jj][0][0])
                {
                    valid_key_value[ii] = FALSE;
                    (*invalid_entries_counter)++;
                    break;
                }
            }

            if (jj != ii)
            {
                continue;
            }

            /** check is outlif is already allocated (by SDK init procedure), in case is it, skip it */
            {
                uint8 is_allocated = FALSE;
                uint32 current_bank = DNX_ALGO_OUTLIF_LOCAL_LIF_TO_BANK(key_field_val[ii][0][0]);
                SHR_IF_ERR_EXIT(lif_mngr_db.egress_local_lif.is_allocated(unit, current_bank, key_field_val[ii][0][0],
                                                                          &is_allocated));
                if (is_allocated)
                {
                    valid_key_value[ii] = FALSE;
                    (*invalid_entries_counter)++;
                    continue;
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dbal_test_logical_table(
    int unit,
    dbal_tables_e table_id,
    uint32 flags)
{
    int ii, jj;
    int res_type_idx = 0;
    int nof_result_types_to_run;
    int nof_entries = 0, max_entries_in_test = 0, invalid_entries_counter = 0, max_capacity = 0;
    uint32 seed = 0;
    uint8 error_in_partial_access = FALSE;
    int table_has_standard_entries;
    dbal_table_status_e table_status;
    uint32 ecgm_dynamic_access;
    int meter_dynamic_access;

    /** params for DBAL_TABLE_FABRIC_STAT_CTRL restore */
    uint32 counter_flags = 0;
    int counter_interval = 0;
    bcm_pbmp_t counter_pbmp;

    CONST dbal_logical_table_t *table;
    uint32 entry_handle_id = DBAL_SW_NOF_ENTRY_HANDLES;

    int err_recovery_table_entries_count = 0;
    int err_recovery_test_valid_entry_idx = -1;
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    uint8 err_recovery_in_transaction = FALSE;
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    
    if (!kbp_mngr_split_rpf_in_use(unit) &&
        ((table_id == DBAL_TABLE_KBP_IPV4_UNICAST_PRIVATE_LPM_RPF) ||
         (table_id == DBAL_TABLE_KBP_IPV6_UNICAST_PRIVATE_LPM_RPF)))
    {
        LOG_CLI((BSL_META("table %s not supported\n"), dbal_logical_table_to_string(unit, table_id)));
        SHR_EXIT();
    }

    sal_memset(&key_field_val[0][0][0], 0, sizeof(key_field_val));
    sal_memset(&key_field_val_get[0][0][0], 0, sizeof(key_field_val_get));
    sal_memset(&valid_key_value[0], TRUE, sizeof(valid_key_value));

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
    if (table_status != DBAL_TABLE_INITIALIZED)
    {
        LOG_CLI((BSL_META("table %s not initialized or in HW error, cannot run the test \n"),
                 dbal_logical_table_to_string(unit, table_id)));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, table_id, &max_capacity));

#ifndef ADAPTER_SERVER_MODE
    if (SAL_BOOT_PLISIM)
    {
        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "MDB tables cannot run on pcid\n")));
            return _SHR_E_UNAVAIL;
        }
        if (table_is_exclude_from_pcid_mode(table_id))
        {
            return _SHR_E_UNAVAIL;
        }
    }
#endif

    /*
     * Check if the table is KBP and perform device lock.
     * If the device is already locked it should be OK.
     */
    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
        SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchExternalTcamSync, 0));
    }

    if (table_id == DBAL_TABLE_FABRIC_STAT_CTRL)
    {
        SHR_IF_ERR_EXIT(soc_counter_status(unit, &counter_flags, &counter_interval, &counter_pbmp));
        soc_counter_stop(unit);
    }
    if ((table_id == DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_MAX)
        || (table_id == DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_MAX)
        || (table_id == DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_MAX))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_dynamic_memory_access_get(unit, &ecgm_dynamic_access));
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_dynamic_memory_access_set(unit, TRUE));
    }
    if ((table_id == DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES)
        || (table_id == DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES)
        || (table_id == DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE)
        || (table_id == DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE))
    {
        SHR_IF_ERR_EXIT(dnx_meter_dynamic_access_enable_get(unit, &meter_dynamic_access));
        SHR_IF_ERR_EXIT(dnx_meter_dynamic_access_enable_set(unit, TRUE));
    }

    SHR_IF_ERR_EXIT(dbal_tables_capacity_get(unit, table_id, &max_capacity));

    /*
     * Initialize test parameters and print it out -
     * must be printed out for logging information
     */
    diag_dbal_ltt_test_params_init(unit, table, table_id, max_capacity, &flags, &max_entries_in_test, &seed);
    LOG_CLI((BSL_META("****** Running DBAL LTT. table=%s, flags=0x%x, seed=%d, nof_entries=%d ******\n"),
             table->table_name, flags, seed, max_entries_in_test));

    /** set and allocate the valid keys for the test */
    SHR_IF_ERR_EXIT(diag_dbal_test_key_fields_allocation
                    (unit, table, table_id, max_entries_in_test, &nof_entries, &invalid_entries_counter));

    /** In case of iterator test - clear the table first */
    if (flags & LTT_FLAGS_RUN_ITERATOR)
    {
        TABLE_CLEAR_WITH_INFO_AND_TIMER("Clear table before running the test\n");
    }

    /** In case of MDB EM test - clear the table first */
    if ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type == DBAL_TABLE_TYPE_EM))
    {
        SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /** Run the test per result type */
    if ((table_id == DBAL_TABLE_MCDB)
        && ((flags & LTT_FLAGS_RUN_ITERATOR) || (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST)))
    {
        /**
         * MCDB is a table with 6 result types, and 19 bits key.
         * Running LTT_FULL for all result types is taking more than 15 minutes.
         * it cannot be run in regression. The table is also tested in he DBAL UT list, under multiple RT test
         */
        nof_result_types_to_run = 1;
    }
    else
    {
        nof_result_types_to_run = table->nof_result_types;
    }
    for (res_type_idx = 0; res_type_idx < nof_result_types_to_run; res_type_idx++)
    {
        uint8 has_result_field_to_write = 0;
        err_recovery_table_entries_count = 0;
        err_recovery_test_valid_entry_idx = -1;

        /** in case the result type is disabled (when invalid for a
         *  device), we skip the result type  */
        if (table->multi_res_info[res_type_idx].is_disabled)
        {
            continue;
        }
        /*
         * For DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE table result type ETPS_NULL has HW value of 0, so for LTT_FULL test 
         * this entry is not counted at it is the default entry , we skip the result type
         */
        if ((flags & LTT_FLAGS_FULL_ITERATOR_TABLE_CLEAR_TEST) && (table_id == DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE) &&
            (res_type_idx == DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_NULL))
        {
            continue;
        }

        
        if ((table_id == DBAL_TABLE_IN_LIF_IPvX_TUNNELS) && (res_type_idx < 2))
        {
            continue;
        }

        sal_memset(&fields_comb, 0, sizeof(diag_dbal_ltt_fields_to_check));
        sal_memset(&result_field_val[0][0][0], 0, sizeof(result_field_val));
        sal_memset(&result_field_val_get[0][0][0], 0, sizeof(result_field_val_get));
        sal_memset(&iterator_parsed_fields[0][0], 0, sizeof(iterator_parsed_fields));
        sal_memset(&iterator_found_indications[0], 0, sizeof(iterator_found_indications));
        sal_memset(&access_id[0], 0, sizeof(access_id));

        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Start test for result type %d\n"), res_type_idx));

        /**
         *  Out-Lif Allocation
         *  Should be inside the result type loop as some of the result types may contain LL and some not.
         */
        if (dbal_logical_table_is_out_lif_allocator_eedb(table))
        {
            SHR_IF_ERR_EXIT(diag_dbal_test_out_lif_allocate
                            (unit, table_id, res_type_idx, nof_entries, &invalid_entries_counter));
        }

        /*
         * arrange all fields possible combinations
         * The combinations are derived from child fields
         */
        SHR_IF_ERR_EXIT(dbal_diag_test_combination_build
                        (unit, table_id, table, res_type_idx, &has_result_field_to_write));
        if (!has_result_field_to_write)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Result type %d has no writable fields\n"), res_type_idx));
            continue;
        }

        if (nof_entries == invalid_entries_counter)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error with key allocation, no valid entries to run\n");
        }

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
        SHR_IF_ERR_EXIT(diag_dbal_test_journaling_preq(unit,
                                                       entry_handle_id,
                                                       table_id,
                                                       table,
                                                       res_type_idx,
                                                       nof_entries,
                                                       &err_recovery_table_entries_count,
                                                       &err_recovery_test_valid_entry_idx,
                                                       &err_recovery_in_transaction, flags));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

        /*
         * add all entries to the table.
         * keys- according to prior selection
         * values - randomly, except maximum result value with maximum key value
         */
        LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Start commit\n")));
        for (ii = 0; ii < nof_entries; ii++)
        {
            if (!valid_key_value[ii])
            {
                continue;
            }

            if (flags & LTT_FLAGS_MEASUREMERNTS)
            {
                LTT_TIMERS_TIMER_SET("Entry Commit Timer", LTT_TIMER_SET);
            }
            SHR_IF_ERR_EXIT(dbal_diag_test_entry_commit
                            (unit, entry_handle_id, table_id, table, ii, ii == (nof_entries - 1), res_type_idx,
                             &invalid_entries_counter, FALSE));
            if (flags & LTT_FLAGS_MEASUREMERNTS)
            {
                LTT_TIMERS_TIMER_STOP(LTT_TIMER_SET);
            }
        }

        /*
         * In case of MDB, commit the descriptor DMA chain to HW.
         * If descriptor DMA is disabled, the call immediately returns.
         */
        if (table->access_method == DBAL_ACCESS_METHOD_MDB)
        {
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchDescCommit, 1));
        }

        SHR_IF_ERR_EXIT(diag_dbal_table_has_standard_entries(unit, entry_handle_id, &table_has_standard_entries));

        if (table_has_standard_entries)
        {
            /*
             * get all entries, using the inserted keys
             * compare to the corresponding result values
             */
            LOG_VERBOSE(BSL_LOG_MODULE, (BSL_META_U(unit, "Start get\n")));
            for (ii = 0; ii < nof_entries; ii++)
            {
                if (!valid_key_value[ii])
                {
                    continue;
                }
                if (flags & LTT_FLAGS_MEASUREMERNTS)
                {
                    LTT_TIMERS_TIMER_SET("Entry Get Timer", LTT_TIMER_GET);
                }
                SHR_IF_ERR_EXIT(dbal_diag_test_entry_get(unit, entry_handle_id, table_id, table, res_type_idx, ii));
                if (flags & LTT_FLAGS_MEASUREMERNTS)
                {
                    LTT_TIMERS_TIMER_STOP(LTT_TIMER_GET);
                }

                if (DBAL_TABLE_IS_TCAM(table))
                {
                    /*
                     * For TCAM Type: Compare also key fields
                     */
                    if (compare_fields_values(unit, table, key_field_val[ii], key_field_val_get[ii], 0, 1) != 0)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry %d - key set/get values are different. table %s\n", ii,
                                     table->table_name);
                    }
                }

                /*
                 * Compare result fields
                 */
                if (compare_fields_values(unit, table, result_field_val[ii], result_field_val_get[ii], res_type_idx, 0)
                    != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry %d was not found. table %s\n", ii, table->table_name);
                }
            }
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Finish with min/max/random values basic test\n")));
        }

        /*
         * In case of LTT_FLAGS_RUN_ITERATOR is set, continue to iterator test
         */
        if ((flags & LTT_FLAGS_RUN_ITERATOR) && (table_has_standard_entries))
        {
            int ent_counter = 0;
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Starting iterator test\n")));
            if (flags & LTT_FLAGS_MEASUREMERNTS)
            {
                LTT_TIMERS_TIMER_SET("Table Iterator Timer", LTT_TIMER_ITERATOR);
            }
            SHR_IF_ERR_EXIT(dbal_diag_test_match_entries_by_iter
                            (unit, entry_handle_id, table_id, table, nof_entries, res_type_idx, &ent_counter));
            if (flags & LTT_FLAGS_MEASUREMERNTS)
            {
                LTT_TIMERS_TIMER_STOP(LTT_TIMER_ITERATOR);
            }

            /*
             * Iterator validation - validate all valid entries were found
             */
            if (ent_counter != (nof_entries - invalid_entries_counter))
            {
                int valid_iter;
                for (valid_iter = 0; valid_iter < 32; valid_iter++)
                {
                    if (valid_key_value[ii] && (iterator_found_indications[valid_iter] == 0))
                    {
                        LOG_CLI((BSL_META("Entry %d not found\n"), valid_iter));
                    }
                }
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong counting of entries in table."
                             "iterator count=%d, num_of_entries=%d, invalid_entries_counter=%d. table %s\n",
                             ent_counter, nof_entries, invalid_entries_counter, table->table_name);
            }
            else
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Iterator found %d entries\n"), ent_counter));
            }
            for (ii = 0; ii < nof_entries; ii++)
            {
                if (!valid_key_value[ii])
                {
                    continue;
                }

                if (iterator_found_indications[ii] == FALSE)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Entry #%d, was not found by iterator. table %s\n", ii,
                                 table->table_name);
                }
            }
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Done with iterator test\n")));
        }

#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
        /*
         * compare state before cleanup, check state manager sanity
         */
        SHR_IF_ERR_EXIT(diag_dbal_test_snapshot_manager_check(unit, flags));
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */

        /*
         * Delete all entries
         * According to flags, skip delete, use table clear or delete entries one by one
         */
        if (flags & LTT_FLAGS_DO_NOT_REMOVE_ENTRIES)
        {
            LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Keep entries in table, skip the delete part\n")));
        }
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
        else if (diag_dbal_test_logical_table_cleanup_is_err_recovery(unit, flags))
        {
            err_recovery_in_transaction = FALSE;

            SHR_IF_ERR_EXIT(diag_dbal_test_err_recovery_cleanup(unit,
                                                                entry_handle_id,
                                                                table_id,
                                                                table,
                                                                res_type_idx,
                                                                err_recovery_table_entries_count,
                                                                err_recovery_test_valid_entry_idx, flags));
        }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */
        else
        {
            if (flags & LTT_FLAGS_TABLE_CLEAR_END_OF_TEST)
            {
                TABLE_CLEAR_WITH_INFO_AND_TIMER("Clear table and verify\n");
                ITERATOR_EMPTY_TABLE_WITH_TIMER();
            }
            else
            {
                LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Delete entries\n")));
                for (ii = 0; ii < nof_entries; ii++)
                {
                    if (!valid_key_value[ii])
                    {
                        continue;
                    }

                    if (flags & LTT_FLAGS_MEASUREMERNTS)
                    {
                        LTT_TIMERS_TIMER_SET("Entry Clear Timer", LTT_TIMER_ENTRY_CLEAR);
                    }
                    SHR_IF_ERR_EXIT(dbal_diag_test_entry_clear
                                    (unit, entry_handle_id, table_id, table, res_type_idx, ii));
                    if (flags & LTT_FLAGS_MEASUREMERNTS)
                    {
                        LTT_TIMERS_TIMER_STOP(LTT_TIMER_ENTRY_CLEAR);
                    }
                }
            }

            /*
             * Deallocate all the allocated fields
             */
            for (ii = 0; ii < nof_entries; ii++)
            {
                if (!valid_key_value[ii])
                {
                    continue;
                }
                for (jj = 0; jj < table->nof_key_fields; jj++)
                {
                    if (SHR_BITGET(table->keys_info[jj].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR)
                        && (!dbal_table_is_allcator_not_needed(unit, table_id)))
                    {
                        int core_val = dbal_diag_test_find_core_id_val(unit, ii);
                        SHR_IF_ERR_EXIT(diag_dbal_alloc_dealloc_value(unit, table->keys_info[jj].field_id,
                                                                      table_id, res_type_idx, core_val,
                                                                      key_field_val[ii][jj][0], 0));
                    }
                }
            }
        }
        if (dbal_logical_table_is_out_lif_allocator_eedb(table))
        {
            SHR_IF_ERR_EXIT(dnx_lif_lib_free(unit, LIF_MNGR_INVALID, NULL, outlif_base_val));
        }
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Logical test is done for result type %d\n"), res_type_idx));
        LOG_INFO(BSL_LOG_MODULE, (BSL_META("****** DBAL LTT Done. table=%s, nof_entries_checked=%d ******\n"),
                                  table->table_name, nof_entries - invalid_entries_counter));
    }
    /*
     * call partial access test - only in regular mode
     */

    if ((flags == 0) && (!DBAL_TABLE_IS_TCAM(table)) &&
        (table->access_method != DBAL_ACCESS_METHOD_PEMLA) && !(dbal_logical_table_is_out_lif_allocator_eedb(table) &&
                                                                !(dbal_logical_table_is_out_rif_allocator(table))))
    {
        int rc;
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Running partial access test\n")));
        rc = test_dnx_dbal_partial_access(unit, table_id, DBAL_UT_RESTORE_TABLE, FALSE);
        if (rc != _SHR_E_NONE)
        {
            error_in_partial_access = TRUE;
            SHR_ERR_EXIT(rc, "Error in partial access. table %s", table->table_name);
        }
    }

    LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Logical test is done for table\n")));

exit:
#ifdef DNXC_ROLLBACK_JOURNAL_IS_ENABLED
    if (err_recovery_in_transaction)
    {
        /** error recovery transaction was not closed properly */
        diag_dbal_test_err_recovery_cleanup(unit, 0, table_id, table, res_type_idx, err_recovery_table_entries_count,
                                            -1, flags);
    }
#endif /* DNXC_ROLLBACK_JOURNAL_IS_ENABLED */
    if (SHR_FUNC_ERR())
    {
        if (!error_in_partial_access)
        {
            diag_dbal_test_print_debug_info(unit, table, nof_entries, res_type_idx);
        }
        LOG_INFO(BSL_LOG_MODULE, (BSL_META_U(unit, "Error in LTT. table %s\n"), table->table_name));
    }

    if (table_id == DBAL_TABLE_FABRIC_STAT_CTRL)
    {
        SHR_IF_ERR_EXIT(soc_counter_start(unit, counter_flags, counter_interval, counter_pbmp));
    }
    if ((table_id == DBAL_TABLE_ECGM_INTERFACE_RESOURCES_STATISTICS_MAX)
        || (table_id == DBAL_TABLE_ECGM_PORT_RESOURCES_STATISTICS_MAX)
        || (table_id == DBAL_TABLE_ECGM_QUEUE_RESOURCES_STATISTICS_MAX))
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_dynamic_memory_access_set(unit, ecgm_dynamic_access));
    }
    if ((table_id == DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_BIG_ENGINES)
        || (table_id == DBAL_TABLE_METER_GLOBAL_SHARING_FLAG_SMALL_ENGINES)
        || (table_id == DBAL_TABLE_METER_PROFILE_SET_FOR_SMALL_ENGINE)
        || (table_id == DBAL_TABLE_METER_PROFILE_SET_FOR_BIG_ENGINE))
    {
        SHR_IF_ERR_EXIT(dnx_meter_dynamic_access_enable_set(unit, meter_dynamic_access));
    }
    if (table_id == DBAL_TABLE_PLL1_CFG)
    {
        SHR_IF_ERR_EXIT(soc_dnx_pll1_init(unit));
    }

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
