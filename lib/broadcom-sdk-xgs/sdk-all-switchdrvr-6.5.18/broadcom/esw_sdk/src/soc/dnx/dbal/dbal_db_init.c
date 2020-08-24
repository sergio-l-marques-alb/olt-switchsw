/**
 * \file dbal_db_init.c
 * $Id$
 *
 * APIs for init the dbal logical tables Database
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/
#include <stdlib.h>
#include <sal/core/libc.h>
#include <soc/drv.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>
#include <shared/dbx/dbx_xml.h>
#include <shared/dbx/dbx_file.h>
#include <shared/utilex/utilex_str.h>
#include <soc/sand/sand_aux_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data.h>
#include <soc/dnx/swstate/auto_generated/access/dbal_access.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/mcm/memregs.h>
#include <soc/dnx/mdb.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_tables_hard_logic.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_tables_sw_state.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_tables_tcam_context_selection.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_tables_mdb.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_tables_pemla.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_hw_entity_groups.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <shared/utilex/utilex_bitstream.h>
#include <soc/dnx/utils/dnx_pp_programmability_utils.h>

#include "dbal_internal.h"
#include "dbal_formula_cb.h"
/*************
 *  DEFINES  *
 *************/
#define DBAL_NOF_XML_FILES              (1000)
#define DBAL_DB_INIT_DEFAULT_VAL_TEMP   (0x12345678)

#define DBAL_IS_VALID_FUNCTION_ATTRIBUTES_IN_FORMULA(function_attributes_in_formula) ((function_attributes_in_formula)->end_index != 0)


/*************
 * TYPEDEFS  *
 *************/

/** 
 * used to indicate the beginining and the end of a dbal 
 * function
 * A*B+log2(C+D)+E 
 *     ^       ^
 *     |       |
 */
typedef struct
{
    /** start of the dbal function in formula string to parse  */
    int start_index;
    /** end of the dbal function in formulat string to parse  */
    int end_index;
    /**  function enum  */
    dbal_formula_operations_e formula_operation_function;
} function_attributes_in_formula_t;

/*************
 *  GLOBALS  *
 *************/

/**
 * \brief
 * Delimiters for dnx data indication
 */
static char *dnx_data_delimiters = ".(),";

/**
 * \brief
 * Delimiters for physical db id indication
 */
static char *physical_db_id_delimiters = ".";

/**
 * \brief
 * DBAL XMLs files list
 */
char *xml_files_list[DBAL_NOF_XML_FILES];

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];

#define INVALID_ARRm   {INVALIDm,INVALIDm,INVALIDm,INVALIDm,INVALIDm, INVALIDm,INVALIDm,INVALIDm,INVALIDm,INVALIDm, \
                        INVALIDm,INVALIDm,INVALIDm,INVALIDm,INVALIDm, INVALIDm,INVALIDm,INVALIDm,INVALIDm,INVALIDm}

#define INVALID_ARRr   {INVALIDr,INVALIDr,INVALIDr,INVALIDr,INVALIDr, INVALIDr,INVALIDr,INVALIDr,INVALIDr,INVALIDr, \
                        INVALIDr,INVALIDr,INVALIDr,INVALIDr,INVALIDr, INVALIDr,INVALIDr,INVALIDr,INVALIDr,INVALIDr}

#define DBAL_INIT_COPY_N_STR(dest, src, length) \
do                                                      \
{                                                       \
    sal_strncpy_s(dest, src, length);                 \
}while (0);

/*************
 * FUNCTIONS *
 *************/
shr_error_e
dbal_db_init_dynamic_dbal_tables_sw_state(
    int unit)
{
    int table_index;
    int nof_dynamic_tables;
    int nof_dynamic_xml_tables = 0;
    uint8 res_type_index;
    int nof_multi_res_types;

    SHR_FUNC_INIT_VARS(unit);

    nof_dynamic_tables = dnx_data_dbal.table.nof_dynamic_tables_get(unit);

    /**dynamic tables init*/
    for (table_index = 0; table_index < nof_dynamic_tables; table_index++)
    {
        /** allocate table labels info memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.labels.alloc(unit, table_index));

        /** allocate table key info memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.key_info_data.alloc(unit, table_index));

        /** allocate table multi res type  memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.multi_res_info_data.alloc(unit, table_index));

        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.res_info_data.alloc(unit, table_index));

#if defined(INCLUDE_KBP)
        SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.access_info.alloc(unit, table_index, sizeof(kbp_db_handles_t)));
#endif
    }

    if (DBAL_DYNAMIC_XML_TABLES_ENABLED)
    {
        nof_dynamic_xml_tables = dnx_data_dbal.table.nof_dynamic_xml_tables_get(unit);

        /**dynamic xml tables init*/
        for (table_index = 0; table_index < nof_dynamic_xml_tables; table_index++)
        {
            /** allocate table labels info memory including swstate */
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.labels.alloc(unit, table_index));

            /** allocate table key info memory including swstate */
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.key_info_data.alloc(unit, table_index));

            /** allocate table multi res type  memory including swstate */
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.multi_res_info_data.alloc(unit, table_index));

            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.res_info_data.alloc(unit, table_index));
        }
    }

    /** allocate ppmc table swstate DB */
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_status.alloc(unit));
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.alloc(unit));
    nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    for (res_type_index = 0; res_type_index < nof_multi_res_types; res_type_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.
                        multi_res_info_status.set(unit, res_type_index, DBAL_MULTI_RES_INFO_AVAIL));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_PPMC_RES_TYPE.multi_res_info_data_sep.res_info_data.alloc(unit, res_type_index));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_dynamic_dbal_tables_memory(
    int unit)
{
    dbal_logical_table_t *table;
    int table_index;
    int nof_tables;
    int labels_size_bytes;
    int key_info_data_size_bytes;
    int multi_res_info_data_size_bytes;
    int multi_res_info_results_info_data_size_bytes;
    int nof_multi_res_types;
    uint8 multi_res_type_index;
    int dynamic_entries_start_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_tables = dnx_data_dbal.table.nof_dynamic_tables_get(unit);
    dynamic_entries_start_index = DBAL_NOF_TABLES;
    if (DBAL_DYNAMIC_XML_TABLES_ENABLED)
    {
        dynamic_entries_start_index = DBAL_NOF_STATIC_TABLES;
        nof_tables += (DBAL_NOF_TABLES - DBAL_NOF_STATIC_TABLES);
    }

    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        /** get table pointer */
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit, table_index + dynamic_entries_start_index, &table));
        /** Initialize table to default values */
        SHR_IF_ERR_EXIT(dbal_logical_table_t_init(unit, table));

        /** allocate table labels info memory */
        labels_size_bytes = dnx_data_dbal.table.nof_dynamic_tables_labels_get(unit) * sizeof(dbal_labels_e);
        if (labels_size_bytes)
        {
            SHR_ALLOC_SET_ZERO(table->table_labels, labels_size_bytes,
                               "table labels allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
        }

        /** allocate table key info memory */
        key_info_data_size_bytes =
            dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t);
        if (key_info_data_size_bytes)
        {
            SHR_ALLOC_SET_ZERO(table->keys_info, key_info_data_size_bytes,
                               "key fields info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
        }

        /** allocate table multi res type  memory */
        nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
        multi_res_info_data_size_bytes = nof_multi_res_types * sizeof(multi_res_info_t);
        multi_res_info_results_info_data_size_bytes =
            dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t);
        if (multi_res_info_data_size_bytes)
        {
            SHR_ALLOC_SET_ZERO(table->multi_res_info, multi_res_info_data_size_bytes,
                               "multiple results info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);

            /** allocate table result fields memory */
            for (multi_res_type_index = 0; multi_res_type_index < nof_multi_res_types; multi_res_type_index++)
            {
                table->multi_res_info[multi_res_type_index].results_info = NULL;
                SHR_ALLOC_SET_ZERO(table->multi_res_info[multi_res_type_index].results_info,
                                   multi_res_info_results_info_data_size_bytes, "results fields info allocation",
                                   "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
            }
        }

#if defined(INCLUDE_KBP)
        SHR_ALLOC_SET_ZERO(table->kbp_handles, sizeof(kbp_db_handles_t),
                           "multiple result direct mapping allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
#endif
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function parse the read DNX DATA reference from xml to
 * dnx data components
 */
static shr_error_e
dbal_db_parse_dnx_data(
    int unit,
    char *full_map,
    dbal_parsed_db_dnx_data_struct_t * int_or_dnx_data)
{
    char *dnx_data_label;
    char *saveptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** first word is DNX_DATA - skip it */
    dnx_data_label = sal_strtok_r(full_map, dnx_data_delimiters, &saveptr);

    /** second word is module */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        sal_strncpy_s(int_or_dnx_data->module, dnx_data_label, sizeof(int_or_dnx_data->module));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx data do not include module:%s\n", full_map);
    }

    /** third word is submodule */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        sal_strncpy_s(int_or_dnx_data->submodule, dnx_data_label, sizeof(int_or_dnx_data->submodule));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx data do not include submodule:%s\n", full_map);
    }

    /** fourth word is data */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        sal_strncpy_s(int_or_dnx_data->data, dnx_data_label, sizeof(int_or_dnx_data->data));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "dnx data do not include data:%s\n", full_map);
    }

    /** fifth word (optional) member */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        sal_strncpy_s(int_or_dnx_data->member, dnx_data_label, sizeof(int_or_dnx_data->member));
    }
    else
    {
        sal_strncpy_s(int_or_dnx_data->member, EMPTY, sizeof(int_or_dnx_data->member));
    }

    /** Read key 1 (first integer) (optional) */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        int_or_dnx_data->key1 = sal_atoi(dnx_data_label);
    }
    else
    {
        int_or_dnx_data->key1 = 0;
    }

    /** Read key 2 (second integer) (optional) */
    dnx_data_label = sal_strtok_r(NULL, dnx_data_delimiters, &saveptr);
    if (dnx_data_label != NULL)
    {
        int_or_dnx_data->key2 = sal_atoi(dnx_data_label);
    }
    else
    {
        int_or_dnx_data->key2 = 0;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function parse the read TABLE reference from xml to
 * dbal physical table id
 */
static shr_error_e
dbal_db_parse_phyiscal_db_id(
    int unit,
    char *full_map,
    dbal_physical_tables_e * physical_db_id)
{
    char *phyiscal_db_label;
    char *saveptr = NULL;

    SHR_FUNC_INIT_VARS(unit);

    /** first word is PHYDB - skip it */
    phyiscal_db_label = sal_strtok_r(full_map, physical_db_id_delimiters, &saveptr);

    /** second word is physical db name */
    phyiscal_db_label = sal_strtok_r(NULL, physical_db_id_delimiters, &saveptr);
    if (phyiscal_db_label != NULL)
    {
        SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, phyiscal_db_label, physical_db_id));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Physical DB is not valid:%s\n", full_map);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function calls the dnx_data general API
 */
static shr_error_e
dbal_db_dnx_data_get_value(
    int unit,
    dbal_parsed_db_dnx_data_struct_t * parsed_dnx_data,
    uint32 *return_value)
{
    const uint32 *data_from_dnx;

    SHR_FUNC_INIT_VARS(unit);

    data_from_dnx = dnxc_data_utils_generic_2d_data_get(unit, parsed_dnx_data->module, parsed_dnx_data->submodule,
                                                        parsed_dnx_data->data, parsed_dnx_data->member,
                                                        parsed_dnx_data->key1, parsed_dnx_data->key2);
    if (data_from_dnx == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "DNX_DATA data cannot be found\n"
                     "module: %s, submodule: %s, data: %s, member: %s, key1=%d, key2=%d\n",
                     parsed_dnx_data->module, parsed_dnx_data->submodule, parsed_dnx_data->data,
                     (parsed_dnx_data->member == NULL) ? EMPTY : parsed_dnx_data->member,
                     parsed_dnx_data->key1, parsed_dnx_data->key2);
    }
    *return_value = *data_from_dnx;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function check if 2 have common field in its definition.
 * (fieldID and child fields)
 */
static shr_error_e
dbal_db_check_field_duplication(
    int unit,
    dbal_fields_e field_1,
    dbal_fields_e field_2,
    uint8 *is_duplicated)
{
    int ii, jj;
    uint8 is_same_instance = FALSE;
    dbal_field_types_basic_info_t *field_1_info;
    dbal_field_types_basic_info_t *field_2_info;

    SHR_FUNC_INIT_VARS(unit);

    *is_duplicated = FALSE;

    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_1, &field_1_info));
    SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, field_2, &field_2_info));

    /** check that the fields are different */
    if (field_1 == field_2)
    {
        *is_duplicated = TRUE;
        SHR_EXIT();
    }

    /** If fields are of the same instance - do not check */
    SHR_IF_ERR_EXIT(dbal_fields_of_same_instance(unit, field_1, field_2, &is_same_instance));
    if (is_same_instance)
    {
        SHR_EXIT();
    }

    /** check that the field2 is not child field of field1 */
    for (ii = 0; ii < field_1_info->nof_child_fields; ii++)
    {
        dbal_fields_e sub_field_id = field_1_info->sub_field_info[ii].sub_field_id;
        if (sub_field_id == field_2)
        {
            *is_duplicated = TRUE;
            SHR_EXIT();
        }
    }

    /** check that the field1 is not child field of field2 */
    for (ii = 0; ii < field_2_info->nof_child_fields; ii++)
    {
        dbal_fields_e sub_field_id = field_2_info->sub_field_info[ii].sub_field_id;
        if (sub_field_id == field_1)
        {
            *is_duplicated = TRUE;
            SHR_EXIT();
        }
    }

    /*
     * check that all child fields of field1 and field2 are separated
     */
    for (ii = 0; ii < field_1_info->nof_child_fields; ii++)
    {
        for (jj = 0; jj < field_2_info->nof_child_fields; jj++)
        {
            if (field_1_info->sub_field_info[ii].sub_field_id == field_2_info->sub_field_info[jj].sub_field_id)
            {
                *is_duplicated = TRUE;
                SHR_EXIT();
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function parse and adds to table the condition parameters
 * read from XML
 */
static shr_error_e
dbal_db_init_condition_parsing(
    int unit,
    dbal_tables_e table_id,
    dbal_access_condition_info_t * entry_condition,
    table_db_access_condition_struct_t * condition_params)
{
    SHR_FUNC_INIT_VARS(unit);

    if (condition_params->condition_type != DBAL_CONDITION_NONE)
    {
        entry_condition->type = condition_params->condition_type;
        if (condition_params->field_id != DBAL_FIELD_EMPTY)
        {
            int field_index, rv;
            entry_condition->field_id = condition_params->field_id;
            /** validate that the field exists in table's key info */
            rv = dbal_tables_field_index_in_table_get_no_err(unit, table_id, entry_condition->field_id, TRUE, 0, 0,
                                                             &field_index);
            entry_condition->field_pos_in_table = field_index;
            if (rv)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "mapping condition uses an invalid field:%s for table:%s\n",
                             dbal_field_to_string(unit, entry_condition->field_id),
                             dbal_logical_table_to_string(unit, table_id));
            }
        }
        if (condition_params->enum_val != DBAL_DB_INVALID)
        {
            entry_condition->value[0] = condition_params->enum_val;
        }
        else
        {
            entry_condition->value[0] = condition_params->value_dnx_data.int_val;
        }
        sal_memcpy(entry_condition->mask, G_dbal_field_full_mask, DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 * Search for dbal function starting index in 
 * formula string str_to_parse starting at offset. 
 */
static shr_error_e
dbal_db_parse_formula_start_function_index(
    int unit,
    dbal_tables_e table_id,
    char *str_to_parse,
    int offset,
    int *dbal_start_function_index)
{
    int str_len = sal_strlen(str_to_parse);

    SHR_FUNC_INIT_VARS(unit);

    *dbal_start_function_index = offset;

    while (*dbal_start_function_index < str_len)
    {
        /** found */
        if (sal_strncasecmp
            (&(str_to_parse[*dbal_start_function_index]), DBAL_LOG2_FUNCTION, sal_strlen(DBAL_LOG2_FUNCTION)) == 0)
        {
            SHR_EXIT();
        }
        (*dbal_start_function_index)++;
    }
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't find start of dbal function in formula %s in table %s\n",
                 str_to_parse, dbal_logical_table_to_string(unit, table_id));
exit:
    SHR_FUNC_EXIT;
}
/** 
 * \brief 
 * update the index of the dbal function end index 
 * Expect that 1st char is "(" 
 * if not found, return error
 */
static shr_error_e
dbal_db_parse_formula_end_function_index(
    int unit,
    dbal_tables_e table_id,
    char *str_to_parse,
    int offset,
    int *function_end_index)
{

    int str_len = sal_strlen(str_to_parse);
    int index;
    int open_parenthesis = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Expect that 1st char is "(" 
     */
    if (str_to_parse[offset] != '(')
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "It was expected to have a ( at this point in %s in table %s",
                     &(str_to_parse[offset]), dbal_logical_table_to_string(unit, table_id));
    }

    for (index = offset; index < str_len; index++)
    {
        if (str_to_parse[index] == '(')
        {
            open_parenthesis++;
        }
        else if (str_to_parse[index] == ')')
        {
            open_parenthesis--;
            if (open_parenthesis == 0)
            {
                *function_end_index = index;
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can't find end of dbal function in formula in %s in table %s",
                 str_to_parse, dbal_logical_table_to_string(unit, table_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_parse_formula_function_index_ranges(
    int unit,
    dbal_tables_e table_id,
    char *curr_str,
    function_attributes_in_formula_t * dbal_function_ranges)
{

    int index = 0;
    int dbal_function_ranges_index = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** search for dbal log function in formula string */
    while (sal_strstr(&(curr_str[index]), DBAL_LOG2_FUNCTION) != NULL)
    {

        if (dbal_function_ranges_index >= DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of dbal function in formula %s is too long in table %s\n",
                         curr_str, dbal_logical_table_to_string(unit, table_id));
        }

        /** now we know there is log2 function in the formula.
         *  we search for log2 function start index and end index in the
         *  formula */
        SHR_IF_ERR_EXIT(dbal_db_parse_formula_start_function_index(unit, table_id,
                                                                   curr_str,
                                                                   index,
                                                                   &(dbal_function_ranges
                                                                     [dbal_function_ranges_index].start_index)));

        SHR_IF_ERR_EXIT(dbal_db_parse_formula_end_function_index(unit, table_id,
                                                                 curr_str,
                                                                 dbal_function_ranges
                                                                 [dbal_function_ranges_index].start_index +
                                                                 sal_strlen(DBAL_LOG2_FUNCTION),
                                                                 &(dbal_function_ranges
                                                                   [dbal_function_ranges_index].end_index)));

        dbal_function_ranges[dbal_function_ranges_index].formula_operation_function = DBAL_FORMULA_OPERATION_F_LOG2;

        /** update string index */
        index = dbal_function_ranges[dbal_function_ranges_index].end_index + 1;
        /** update dbal function range index */
        dbal_function_ranges_index++;
    }

exit:
    SHR_FUNC_EXIT;
}

static uint8
dbal_db_parse_formula_index_in_function_range(
    int unit,
    int index,
    function_attributes_in_formula_t * dbal_function_ranges)
{
    int dbal_function_ranges_index = 0;
    for (dbal_function_ranges_index = 0; dbal_function_ranges_index < DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1;
         dbal_function_ranges_index++)
    {
        /** no more ranges to check */
        if (!DBAL_IS_VALID_FUNCTION_ATTRIBUTES_IN_FORMULA(&dbal_function_ranges[dbal_function_ranges_index]))
        {
            return FALSE;
        }
        /** index is within range */
        else if ((index >= dbal_function_ranges[dbal_function_ranges_index].start_index)
                 && (index <= dbal_function_ranges[dbal_function_ranges_index].end_index))
        {
            return TRUE;
        }
    }
    return FALSE;
}

/** 
 * Return true 
 * when the whole string is a function call. 
 * Only function range is expected
 */
static uint8
dbal_db_parse_formula_is_dbal_function(
    int unit,
    char *curr_str,
    function_attributes_in_formula_t * dbal_function_ranges)
{
    if (DBAL_IS_VALID_FUNCTION_ATTRIBUTES_IN_FORMULA(dbal_function_ranges))
    {
        int str_len = sal_strlen(curr_str);
        return (dbal_function_ranges->start_index == 0) && (dbal_function_ranges->end_index == str_len - 1);
    }
    return FALSE;
}

/**
 * \brief 
 *  calls utilex_str_stoul, one change, DBAL is not allowing HEX number to start with laters, in this case DBAL consider
 *  the number to be field name.. for example the string "DA" will mean the field DA and not the HEX value (218)
 */
static uint32
dbal_str_stoul(
    char *str,
    uint32 *val)
{
    char ch;
    ch = str[0];
    if (!((ch >= '0') && (ch <= '9')))
    {
        return _SHR_E_PARAM;
    }

    return utilex_str_stoul(str, val);
}
/**
 * \brief
 */
static shr_error_e
dbal_db_parse_offset_formula(
    int unit,
    dbal_tables_e table_id,
    uint8 is_res_type_mapping,
    char *str_formula,
    int int_formula,
    dnx_dbal_formula_offset_cb ag_cb_formula,
    dbal_offset_formula_t * formula,
    uint8 allow_dynamic_fields)
{
    int str_len;
    char curr_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    int nof_plus_minus_operands = 0;
    int plus_minus_operands[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1] = { 0 };
    int nof_mult_div_mod_operands = 0;
    int mult_div_mod_operands[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1] = { 0 };
    int index = 0;
    int nof_operands;
    uint8 plus_or_mult;
    char *cb_token;
    uint32 nof_values = 0;
    char **decoded_values_list = NULL;
    function_attributes_in_formula_t dbal_function_ranges[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1];
    uint8 formula_is_dbal_function;

    SHR_FUNC_INIT_VARS(unit);

    /** the formula was parsed by Autocoder (simple int), we can skip the costly parsing */
    if (int_formula != DBAL_DB_INVALID)
    {
        formula->nof_vars = 1;
        formula->val_as_formula = NULL;
        formula->key_val = FALSE;
        /** This section is just to make sure we fill
         *  the formula struct when we go through the full parsing
         *  Exception: we don't set formula->as_string.  */
        formula->val = int_formula;
        SHR_EXIT();
    }

    DBAL_INIT_COPY_N_STR(formula->as_string, str_formula, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    DBAL_INIT_COPY_N_STR(curr_str, str_formula, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);

    /** check if it's an autogenerated cb formula   */
    if (ag_cb_formula != NULL)
    {
        formula->formula_offset_cb = ag_cb_formula;
        SHR_EXIT();
    }

    sal_memset(dbal_function_ranges, 0, sizeof(dbal_function_ranges));

    str_len = sal_strlen(str_formula);
    {
        /** Check if it's a cb formula */
        if (sal_strstr(str_formula, DBAL_INIT_FORMULA_CB_INDICATOR) == str_formula)
        {
            /** Decode the formula string */
            if ((decoded_values_list = utilex_str_split(str_formula, "=", 2, &nof_values)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to parse offset formula: %s\n", str_formula);
            }

            cb_token = decoded_values_list[1];

            while (sal_strcmp(formula_offset_cb_map[index].cb_str, DBAL_FORMULA_CB_LAST_CB))
            {
                if (!strcmp(formula_offset_cb_map[index].cb_str, cb_token))
                {
                    if (formula_offset_cb_map[index].cb == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Formula CB %s is pointing to NULL", cb_token);
                    }
                    formula->formula_offset_cb = formula_offset_cb_map[index].cb;
                    SHR_EXIT();
                }
                index++;
            }
            if (sal_strcmp(formula_offset_cb_map[index].cb_str, "last_cb"))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unrecognized formula cb string");
            }
        }

        /** before searching for binary operators: +,-,*,/, we'll
         *  search for function, such as log2.
         *  If found we'll protect those function so that the function
         *  name and parameters are not searched for binary operators
         *  ex: B * A + log2(B+C) should result:
         *  formula
         *    formula_val: B*A
         *    operator: +
         *    formulat_val: log2(B+C)
         */
        dbal_db_parse_formula_function_index_ranges(unit, table_id, curr_str, dbal_function_ranges);

        /** search for minus and plus separators */

        index = 0;

        while (index < str_len)
        {
            /** skip index within dbal function range  */
            if (dbal_db_parse_formula_index_in_function_range(unit, index, dbal_function_ranges))
            {
                index++;
                continue;
            }

            if ((curr_str[index] == '+') || (curr_str[index] == '-'))
            {
                if (nof_plus_minus_operands >= DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR number of elements in formula %s is too long table %s\n",
                                 formula->as_string, dbal_logical_table_to_string(unit, table_id));
                }
                plus_minus_operands[nof_plus_minus_operands] = index;
                nof_plus_minus_operands++;
            }
            index++;
        }

        if (nof_plus_minus_operands == 0)
        {
            /**
             *  if no plus/minus separators, find the last * % /
             *  and split into two variables
             */
            index = 0;
            while (index < str_len)
            {
                /** skip index within dbal function range  */
                if (dbal_db_parse_formula_index_in_function_range(unit, index, dbal_function_ranges))
                {
                    index++;
                    continue;
                }

                if (nof_mult_div_mod_operands >= DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR number of elements in formula %s is too long table %s\n",
                                 formula->as_string, dbal_logical_table_to_string(unit, table_id));
                }
                if ((curr_str[index] == '*') || (curr_str[index] == '/') || (curr_str[index] == '%'))
                {
                    mult_div_mod_operands[nof_mult_div_mod_operands] = index;
                    nof_mult_div_mod_operands++;
                }
                index++;
            }
        }

        nof_operands = nof_plus_minus_operands == 0 ? nof_mult_div_mod_operands : nof_plus_minus_operands;
        plus_or_mult = nof_plus_minus_operands == 0 ? 0 : 1;

        formula_is_dbal_function = dbal_db_parse_formula_is_dbal_function(unit, curr_str, dbal_function_ranges);

        /** the formula is a dbal function  */
        if (formula_is_dbal_function)
        {
            char val_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH] = { '\0' };
            int parameter_start_index = dbal_function_ranges[0].start_index + sal_strlen(DBAL_LOG2_FUNCTION) + 1;
            int parameter_string_length = dbal_function_ranges[0].end_index - parameter_start_index;

            /** only one value as formula is expected:
             *  the function parameter.
             */
            int nof_elements = 1;
            formula->nof_vars = nof_elements;
            SHR_ALLOC_SET_ZERO(formula->val_as_formula, nof_elements * sizeof(dbal_offset_formula_t),
                               "val_as_formula", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
            formula->val = 0xFFFFFFFF;
            formula->field_id = DBAL_FIELD_EMPTY;

            formula->action[0] = dbal_function_ranges[0].formula_operation_function;

            /** get the parameter of the function  */

            sal_strncpy(val_str, &(curr_str[parameter_start_index]), parameter_string_length);
            SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                            (unit, table_id, is_res_type_mapping, val_str, DBAL_DB_INVALID, NULL,
                             &formula->val_as_formula[0], allow_dynamic_fields));
        }
        /** the formula is a value   */
        else if (nof_operands == 0)
        {
            int res;
            formula->nof_vars = 1;
            formula->val_as_formula = NULL;
            formula->key_val = FALSE;
            formula->val = 0xFFFFFFFF;

            if (sal_strcmp(curr_str, "KEY") == 0)
            {
                formula->key_val = TRUE;
                if (!allow_dynamic_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "KEY in formula is a dynamic information that cannot be used in that place.\n");
                }
            }
            else if (sal_strcmp(curr_str, "RESULT") == 0)
            {
                if (is_res_type_mapping)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Mapping field RESULT_TYPE cannot be done using RESULT size\n");
                }
                formula->result_size = TRUE;
                if (!allow_dynamic_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "RESULT in formula is a dynamic information that cannot be used in that place.\n");
                }
            }
            else if (sal_strcmp(curr_str, "INSTANCE") == 0)
            {
                formula->instance_index = TRUE;
                if (!allow_dynamic_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "INSTANCE in formula is a dynamic information that cannot be used in that place.\n");
                }
            }
            else if (sal_strstr(curr_str, DB_INIT_PHYDB_INDICATION) != NULL)
            {
                dbal_physical_tables_e physical_db_id;
                int physical_db_size_val = 0;

                SHR_IF_ERR_EXIT(dbal_db_parse_phyiscal_db_id(unit, curr_str, &physical_db_id));
                SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, physical_db_id, &physical_db_size_val));

                formula->val = (uint32) physical_db_size_val;
            }
            else
            {
                if (sal_strstr(curr_str, DB_INIT_DNX_DATA_INDICATION) != NULL)
                {
                    uint8 negative_check = FALSE;
                    uint32 dnx_data_val = 0;
                    dbal_parsed_db_dnx_data_struct_t parsed_dnx_data;
                    if (curr_str[0] == '!')
                    {
                        negative_check = TRUE;
                    }
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data(unit, curr_str, &parsed_dnx_data));
                    SHR_IF_ERR_EXIT(dbal_db_dnx_data_get_value(unit, &parsed_dnx_data, &dnx_data_val));
                    formula->val = (dnx_data_val > UTILEX_I32_MAX) ? UTILEX_I32_MAX : (int) dnx_data_val;
                    if (negative_check == TRUE)
                    {
                        formula->val = !formula->val;
                    }
                }
                else
                {
                    if (dbal_str_stoul(curr_str, (uint32 *) &formula->val) != _SHR_E_NONE)
                    {
                        res = dbal_field_string_to_id_no_error(unit, curr_str, &formula->field_id);
                        if (res == _SHR_E_NOT_FOUND)
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "Formula contain a string (%s) that not related to table %s with ID %d \n",
                                         curr_str, dbal_logical_table_to_string(unit, table_id), table_id);
                        }
                        else
                        {
                            if (!allow_dynamic_fields)
                            {
                                SHR_ERR_EXIT(_SHR_E_PARAM,
                                             "Field names in formula is a dynamic information that cannot be used in that place,"
                                             "table %s formula %s\n", dbal_logical_table_to_string(unit, table_id),
                                             str_formula);
                            }
                            if (is_res_type_mapping)
                            {
                                int rv;
                                int field_index;
                                rv = dbal_tables_field_index_in_table_get_no_err(unit, table_id, formula->field_id,
                                                                                 TRUE, 0, 0, &field_index);
                                if (rv == _SHR_E_NOT_FOUND)
                                {
                                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                                 "RESULT_TYPE mapping with field %s is illegal (it is result field or not in table %s)\n",
                                                 curr_str, dbal_logical_table_to_string(unit, table_id));
                                }

                                formula->is_result = 0;
                                formula->field_pos_in_table = field_index;
                            }
                            else
                            {
                                int rv;
                                int field_index;
                                /** 
                                 *  handling field, look for the field in the table, to identify if it is a key field or result field. incase it is not
                                 *  known (since the field was not found in the table definitions yet set info to unknown..
                                 */

                                /** checking if it is a key field */
                                rv = dbal_tables_field_index_in_table_get_no_err(unit, table_id, formula->field_id,
                                                                                 TRUE, 0, 0, &field_index);
                                if (rv == _SHR_E_NOT_FOUND)
                                {
                                    CONST dbal_logical_table_t *table;
                                    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

                                    /** for multiple result type tables, the field position can change per result type, so we cant know what is the
                                     *  correct position since we don't know the result type.. */
                                    if (table->nof_result_types < 1)
                                    {
                                        /** checking if it is a result field */
                                        rv = dbal_tables_field_index_in_table_get_no_err(unit, table_id,
                                                                                         formula->field_id, FALSE, 0, 0,
                                                                                         &field_index);
                                        if (rv != _SHR_E_NONE)
                                        {
                                            /** we don't know yet if it is a key or result.. */
                                            formula->is_result = DBAL_TABLE_FIELD_KEY_OR_RESULT;
                                            formula->field_pos_in_table = DBAL_TABLE_FIELD_POSITION_UNKNOWN;
                                        }
                                        else
                                        {
                                            /** result field */
                                            formula->is_result = 1;
                                            formula->field_pos_in_table = field_index;
                                        }
                                    }
                                    else
                                    {
                                        formula->is_result = DBAL_TABLE_FIELD_KEY_OR_RESULT;
                                        formula->field_pos_in_table = DBAL_TABLE_FIELD_POSITION_UNKNOWN;
                                    }
                                }
                                else
                                {
                                    /**it is a key field */
                                    formula->is_result = 0;
                                    formula->field_pos_in_table = field_index;
                                }
                            }
                        }
                    }
                }
            }
        }
        /** the formula has operands   */
        else
        {
            uint8 found_operand = FALSE;
            char val_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
            int str_index = 0;
            int sub_str_index = 0;
            int operand_index = 0;
            int num_of_elements = nof_operands + 1;
            formula->nof_vars = num_of_elements;

            sal_memset(val_str, 0, sizeof(val_str));
            SHR_ALLOC_SET_ZERO(formula->val_as_formula, num_of_elements * sizeof(dbal_offset_formula_t),
                               "val_as_formula", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

            formula->val = 0xFFFFFFFF;
            formula->field_id = DBAL_FIELD_EMPTY;

            while (str_index < str_len)
            {
                found_operand = FALSE;
                if (plus_or_mult)
                {
                    if (nof_plus_minus_operands >= DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR number of elements in formula %s is too long table %s\n",
                                     formula->as_string, dbal_logical_table_to_string(unit, table_id));
                    }
                    if (str_index == plus_minus_operands[operand_index])
                    {
                        found_operand = TRUE;
                        if (curr_str[plus_minus_operands[operand_index]] == '+')
                        {
                            formula->action[operand_index] = DBAL_FORMULA_OPERATION_ADD;
                        }
                        else if (curr_str[plus_minus_operands[operand_index]] == '-')
                        {
                            formula->action[operand_index] = DBAL_FORMULA_OPERATION_SUBTRACT;
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal path in algorithm\n");
                        }
                    }
                }
                else
                {
                    if (nof_plus_minus_operands >= DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR number of elements in formula %s is too long table %s\n",
                                     formula->as_string, dbal_logical_table_to_string(unit, table_id));
                    }
                    if (str_index == mult_div_mod_operands[operand_index])
                    {
                        found_operand = TRUE;
                        if (curr_str[mult_div_mod_operands[operand_index]] == '%')
                        {
                            formula->action[operand_index] = DBAL_FORMULA_OPERATION_MODULO;
                        }
                        else if (curr_str[mult_div_mod_operands[operand_index]] == '/')
                        {
                            formula->action[operand_index] = DBAL_FORMULA_OPERATION_DIVIDE;
                        }
                        else if (curr_str[mult_div_mod_operands[operand_index]] == '*')
                        {
                            formula->action[operand_index] = DBAL_FORMULA_OPERATION_MULTIPLY;
                        }
                        else
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal path in algorithm\n");
                        }
                    }
                }

                if (found_operand)
                {
                    val_str[sub_str_index] = '\0';
                    SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                                    (unit, table_id, is_res_type_mapping, val_str, DBAL_DB_INVALID, NULL,
                                     &formula->val_as_formula[operand_index], allow_dynamic_fields));
                    sub_str_index = 0;
                    operand_index++;
                    str_index++;
                }
                else
                {
                    val_str[sub_str_index++] = curr_str[str_index++];
                }
            }
            val_str[sub_str_index] = '\0';
            SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                            (unit, table_id, is_res_type_mapping, val_str, DBAL_DB_INVALID, NULL,
                             &formula->val_as_formula[operand_index], allow_dynamic_fields));
        }
    }
exit:
    utilex_str_split_free(decoded_values_list, nof_values);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function parse and adds to table offset parameters read
 * from XML
 */
static shr_error_e
dbal_db_init_offset_parsing(
    int unit,
    dbal_tables_e table_id,
    uint8 is_res_type_mapping,
    dbal_offset_encode_info_t * entry_offset,
    table_db_offset_in_hw_struct_t * offset_params)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((sal_strcmp(offset_params->formula, EMPTY) != 0) || (offset_params->formula_int != DBAL_DB_INVALID)
        || (offset_params->formula_cb != NULL))
    {
        SHR_ALLOC_SET_ZERO(entry_offset->formula, 1 * sizeof(dbal_offset_formula_t),
                           "formula", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_PARSE);
        SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                        (unit, table_id, is_res_type_mapping, offset_params->formula, offset_params->formula_int,
                         offset_params->formula_cb, entry_offset->formula, TRUE));
        DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_PARSE);
    }

exit:
    SHR_FUNC_EXIT;
}

/********************************************************************************/
/**
* \brief
* The function gets a string and check if it has
* dnx data indication.
* If it has - the indication will be parsed.
* If the string is null, a default value can be returned.
* If the field wasn't mandatory
*/
shr_error_e
dbal_db_parse_dnx_data_indication(
    int unit,
    dbal_db_int_or_dnx_data_info_struct_t * int_or_dnx_data_as_formula,
    char *value_to_parse,
    uint8 is_mandatory,
    int default_val,
    char *str_child_name,
    char *str_to_read)
{
    uint32 formula_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_DNX_DATA_PARSING);

        /** when no value was parsed, return default val if not mandatory  */
    if (value_to_parse == NULL)
    {
        if (is_mandatory)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error when try reading an XML tag:%s,%s\n", str_child_name, str_to_read);
        }
        else
        {
            int_or_dnx_data_as_formula->int_val = default_val;
            SHR_EXIT();
        }
    }

    /*
     * if the value is not valid, this can be when enum is mapped differently per device an in a specific device the
     * value is not mapped
     */
    if (sal_strcmp(value_to_parse, "INVALID") == 0)
    {
        int_or_dnx_data_as_formula->is_invalid = 1;
        SHR_EXIT();
    }

    sal_strncpy_s(int_or_dnx_data_as_formula->formula_as_string, value_to_parse,
                  sizeof(int_or_dnx_data_as_formula->formula_as_string));

    if (utilex_str_stoul(int_or_dnx_data_as_formula->formula_as_string, (uint32 *) &int_or_dnx_data_as_formula->int_val)
        == _SHR_E_NONE)
    {
        SHR_EXIT();
    }

    int_or_dnx_data_as_formula->formula = NULL;

    SHR_ALLOC_SET_ZERO(int_or_dnx_data_as_formula->formula, 1 * sizeof(dbal_offset_formula_t),
                       "formula", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                    (unit, DBAL_NOF_TABLES, FALSE, int_or_dnx_data_as_formula->formula_as_string,
                     DBAL_DB_INVALID, NULL, int_or_dnx_data_as_formula->formula, FALSE));

    SHR_IF_ERR_EXIT(dbal_calculate_offset_from_formula
                    (unit, NULL, DBAL_NOF_TABLES, DBAL_FIELD_EMPTY, int_or_dnx_data_as_formula->formula, &formula_val));

    int_or_dnx_data_as_formula->int_val = (int) formula_val;

    dbal_tables_formulas_free(int_or_dnx_data_as_formula->formula, 0);

exit:
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_DNX_DATA_PARSING);
    SHR_FUNC_EXIT;
}

/**
* \brief
* adding defines to the field type.
*/
static shr_error_e
dbal_db_init_field_type_define_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * fld_type_ent)
{
    int max_value_for_define = 0;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    fld_type_ent->nof_defines_values = fld_type_prm->nof_defined_vals;
    if (fld_type_ent->nof_defines_values > DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more define values than max allowed.\n", fld_type_prm->name);
    }

    SHR_ALLOC_SET_ZERO(fld_type_ent->defines_info,
                       fld_type_ent->nof_defines_values * sizeof(dbal_defiens_info_t),
                       "defines info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);
    {
        if (fld_type_prm->max_size_dnx_data.int_val > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Defined values cannot be bigger that 32 bits long, %s\n",
                         fld_type_prm->name);
        }
        else if (fld_type_prm->max_size_dnx_data.int_val == 32)
        {
            max_value_for_define = UTILEX_U32_MAX;
        }
        else
        {
            max_value_for_define = ((1 << fld_type_prm->max_size_dnx_data.int_val) - 1);
        }
    }
    for (ii = 0; ii < fld_type_prm->nof_defined_vals; ii++)
    {
        sal_strncpy_s(fld_type_ent->defines_info[ii].name, fld_type_prm->name,
                      sizeof(fld_type_ent->defines_info[ii].name));
        sal_strncat_s(fld_type_ent->defines_info[ii].name, "_", sizeof(fld_type_ent->defines_info[ii].name));
        sal_strncat_s(fld_type_ent->defines_info[ii].name, fld_type_prm->define[ii].define_name,
                      sizeof(fld_type_ent->defines_info[ii].name));
        fld_type_ent->defines_info[ii].value = fld_type_prm->define[ii].define_value.int_val;
        if (fld_type_ent->defines_info[ii].value > max_value_for_define)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Illegal value for Define. field size is %d, defined value is %d. field %s\n",
                         fld_type_prm->max_size_dnx_data.int_val, fld_type_ent->defines_info[ii].value,
                         fld_type_ent->name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
* adding enum values to the field type.
*/
static shr_error_e
dbal_db_init_field_type_enum_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * fld_type_ent)
{
    int max_value_for_enum_mapping = 0;
    int ii;
    uint8 is_zero_mapping_exists = FALSE;

    SHR_FUNC_INIT_VARS(unit);

    if (fld_type_ent->nof_enum_values > DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more enums values than max allowed.\n", fld_type_prm->name);
    }

    if (fld_type_prm->max_size_dnx_data.int_val > 31)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Enum fields cannot be bigger that 31 bits long, %s\n", fld_type_prm->name);
    }
    else
    {
        max_value_for_enum_mapping = ((1 << fld_type_prm->max_size_dnx_data.int_val) - 1);
    }

    if (fld_type_ent->print_type != DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Type %s has enum values print type must be ENUM\n", fld_type_prm->name);
    }

    SHR_ALLOC_SET_ZERO(fld_type_ent->enum_val_info,
                       fld_type_ent->nof_enum_values * sizeof(dbal_enum_decoding_info_t),
                       "enum info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);

    fld_type_ent->encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_ENUM;
    /** In case of Enum type - the max value the max enum value */
    fld_type_ent->min_value = 0;
    fld_type_ent->max_value = fld_type_prm->nof_enum_vals - 1;

    for (ii = 0; ii < fld_type_prm->nof_enum_vals; ii++)
    {
        sal_strncpy_s(fld_type_ent->enum_val_info[ii].name, fld_type_prm->name,
                      sizeof(fld_type_ent->enum_val_info[ii].name));
        sal_strncat_s(fld_type_ent->enum_val_info[ii].name, "_", DBAL_MAX_LONG_STRING_LENGTH);
        sal_strncat_s(fld_type_ent->enum_val_info[ii].name, fld_type_prm->enums[ii].name_from_interface,
                      DBAL_MAX_LONG_STRING_LENGTH);
        if (fld_type_prm->enums[ii].value_from_mapping_dnx_data.is_invalid)
        {
                            /** incase that the field is invalid */
            fld_type_ent->enum_val_info[ii].is_invalid = 1;
            fld_type_ent->enum_val_info[ii].value = 0;
            fld_type_ent->nof_invalid_enum_values++;
        }
        else
        {
            fld_type_ent->enum_val_info[ii].value = fld_type_prm->enums[ii].value_from_mapping_dnx_data.int_val;
        }

        if (fld_type_prm->enums[ii].value_from_mapping_dnx_data.int_val > max_value_for_enum_mapping)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "Illegal value for HW mapping in enum. field size is %d, hw value is %d. field %s\n",
                         fld_type_prm->max_size_dnx_data.int_val,
                         fld_type_prm->enums[ii].value_from_mapping_dnx_data.int_val, fld_type_ent->name);
        }

        if (fld_type_ent->enum_val_info[ii].value == 0)
        {
            is_zero_mapping_exists = TRUE;
        }
    }
    if (!is_zero_mapping_exists)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal value for HW mapping in enum. zero value must be in use. field %s\n",
                     fld_type_ent->name);
    }

    /** Default Value as Enum String */
    if (fld_type_prm->default_val_valid)
    {
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID);

        for (ii = 0; ii < fld_type_prm->nof_enum_vals; ii++)
        {
            if (sal_strcmp(fld_type_prm->enums[ii].name_from_interface, fld_type_prm->default_val_for_enum) == 0)
            {
                fld_type_ent->default_value = ii;
                break;
            }
        }
        if (ii == fld_type_prm->nof_enum_vals)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Default value %s of field %s (ENUM) is not found in values definition\n",
                         fld_type_prm->default_val_for_enum, fld_type_ent->name);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief adding struct fields to the field type
*/
static shr_error_e
dbal_db_init_field_type_struct_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * fld_type_ent)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if (fld_type_ent->nof_struct_fields > DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more struct fields than max allowed.\n", fld_type_prm->name);
    }

    SHR_ALLOC_SET_ZERO(fld_type_ent->struct_field_info,
                       fld_type_ent->nof_struct_fields * sizeof(dbal_sub_struct_field_info_t),
                       "struct field info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);

    fld_type_ent->encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_STRUCT;

    for (ii = 0; ii < fld_type_prm->nof_structs; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, fld_type_prm->structs[ii].name,
                                                &fld_type_ent->struct_field_info[ii].struct_field_id));

        fld_type_ent->struct_field_info[ii].length = fld_type_prm->structs[ii].length_dnx_data.int_val;
        fld_type_ent->struct_field_info[ii].offset = fld_type_prm->structs[ii].start_offest_dnx_data.int_val;
        fld_type_ent->struct_field_info[ii].arr_prefix_table_entry_value =
            fld_type_prm->structs[ii].arr_prefix_table_entry_value_dnx_data.int_val;
        fld_type_ent->struct_field_info[ii].arr_prefix_table_entry_width =
            fld_type_prm->structs[ii].arr_prefix_table_entry_width_dnx_data.int_val;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief adding child fields to the field type
*/
static shr_error_e
dbal_db_init_field_type_child_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * fld_type_ent)
{
    int has_reference_field = FALSE;
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    if (fld_type_ent->nof_child_fields > DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more child fields than max allowed.\n", fld_type_prm->name);
    }

    SHR_ALLOC_SET_ZERO(fld_type_ent->sub_field_info, fld_type_ent->nof_child_fields * sizeof(dbal_sub_field_info_t),
                       "sub field info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);

    if (sal_strcmp(fld_type_prm->refernce_field_name, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, fld_type_prm->refernce_field_name,
                                                          &(fld_type_ent->refernce_field_id)));
        has_reference_field = TRUE;
    }

    for (ii = 0; ii < fld_type_prm->nof_childs; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, fld_type_prm->childs[ii].name,
                                                &fld_type_ent->sub_field_info[ii].sub_field_id));

        if (has_reference_field)
        {
            fld_type_ent->sub_field_info[ii].encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_PREFIX;
            SHR_IF_ERR_EXIT(dbal_field_types_enum_name_to_hw_value_get(unit,
                                                                       fld_type_ent->refernce_field_id,
                                                                       fld_type_prm->childs[ii].name,
                                                                       &fld_type_ent->sub_field_info[ii].
                                                                       encode_info.input_param));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_field_encode_type_string_to_id(unit, fld_type_prm->childs[ii].encode_type,
                                                                &fld_type_ent->sub_field_info[ii].
                                                                encode_info.encode_mode));
            fld_type_ent->sub_field_info[ii].encode_info.input_param =
                fld_type_prm->childs[ii].encode_param1_dnx_data.int_val;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds a field read from XML to the dbal field DB. Main functionality is translating the string from XML
 * to corresponding values
 */
shr_error_e
dbal_db_init_field_type_add(
    int unit,
    field_type_db_struct_t * fld_type_prm,
    dbal_field_types_basic_info_t * field_types_info)
{
    int ii;
    dbal_field_types_basic_info_t *fld_type_ent;
    dbal_field_types_defs_e fld_type;

    SHR_FUNC_INIT_VARS(unit);

    /** find the field id according to its name */
    SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, fld_type_prm->name, &fld_type));
    fld_type_ent = field_types_info + fld_type;

    /** Name - Printing Name */
    sal_strncpy_s(fld_type_ent->name, fld_type_prm->name, sizeof(fld_type_ent->name));

    /** Size */
    fld_type_ent->max_size = fld_type_prm->max_size_dnx_data.int_val;

    /** Printing Type */
    SHR_IF_ERR_EXIT(dbal_field_print_type_string_to_id(unit, fld_type_prm->print_type, &fld_type_ent->print_type));

    /** bm */
    if (fld_type_prm->is_common & fld_type_prm->is_default)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "A field type is either common, default or with a specific image, please update the GeneralInfo \n");
    }

    if (fld_type_prm->is_common)
    {
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_COMMON);
    }
    if (fld_type_prm->is_default)
    {
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT);
    }
    if (fld_type_prm->is_standard_1)
    {
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_STANDARD_1);
    }

    /** is allocator */
    if (sal_strcmp(fld_type_prm->resource_name, EMPTY) != 0)
    {
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR);
    }

    /** Max Value */
    if (fld_type_prm->max_value_dnx_data.int_val != DBAL_DB_INVALID)
    {
        fld_type_ent->max_value = fld_type_prm->max_value_dnx_data.int_val;
    }
    else
    {
        if (fld_type_ent->max_size > 32)
        {
            fld_type_ent->max_value = 0;
        }
        else
        {
            fld_type_ent->max_value = utilex_max_value_by_size(fld_type_ent->max_size);
        }
    }

    /** Min Value */
    if (fld_type_prm->min_value_dnx_data.int_val != DBAL_DB_INVALID)
    {
        fld_type_ent->min_value = fld_type_prm->min_value_dnx_data.int_val;
    }
    else
    {
        fld_type_ent->min_value = 0;
    }

    /** Const Value */
    if (fld_type_prm->const_value_dnx_data.int_val != DBAL_DB_INVALID)
    {
        fld_type_ent->const_value = fld_type_prm->const_value_dnx_data.int_val;
        fld_type_ent->min_value = fld_type_ent->const_value;
        fld_type_ent->max_value = fld_type_ent->const_value;
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);

    }
    else
    {
        SHR_BITCLR(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);
    }

    /** Illegal Values */
    for (ii = 0; ii < fld_type_prm->nof_illegal_value; ii++)
    {
        fld_type_ent->illegal_values[ii] = fld_type_prm->illegal_values_dnx_data[ii].int_val;
    }
    fld_type_ent->nof_illegal_values = fld_type_prm->nof_illegal_value;

    /** Encoding */
    if (sal_strcmp(fld_type_prm->encode_type, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_encode_type_string_to_id(unit, fld_type_prm->encode_type,
                                                            &fld_type_ent->encode_info.encode_mode));
        if ((fld_type_ent->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_MULTIPLE) ||
            (fld_type_ent->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_PREFIX) ||
            (fld_type_ent->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_SUFFIX))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field %s encoding, not supported yet\n", fld_type_ent->name);
            /** currently we are not supporting this type of encoding because it makes the field bigger than the original size,
             *  if we want to support need to limit the max size of the field according to the size of the logical value second
             *  we need to add some bits to the size of the field. */
        }
    }

    fld_type_ent->encode_info.input_param = fld_type_prm->encode_param1_dnx_data.int_val;

    if (fld_type_ent->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_VALID_IND)
    {
        if ((fld_type_ent->max_size - 1) >= 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal max size for field %s with valid indication\n", fld_type_ent->name);
        }
        fld_type_ent->max_value = (1 << (fld_type_ent->max_size - 1)) - 1;      /* 2^(fld_type_ent->max_size-1)-1 */
    }

    /** Set default value when not enums */
    if (fld_type_prm->default_val_valid == TRUE && (fld_type_ent->nof_enum_values == 0))
    {
        /** Default Value */
        fld_type_ent->default_value = fld_type_prm->default_val_dnx_data.int_val;
        SHR_BITSET(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_DEFAULT_VALUE_VALID);
    }

    /** handle child fields */
    fld_type_ent->nof_child_fields = fld_type_prm->nof_childs;
    if (fld_type_ent->nof_child_fields > 0)
    {
        SHR_IF_ERR_EXIT(dbal_db_init_field_type_child_add(unit, fld_type_prm, fld_type_ent));
    }

    /** handle struct fields */
    fld_type_ent->nof_struct_fields = fld_type_prm->nof_structs;
    if (fld_type_ent->nof_struct_fields > 0)
    {
        SHR_IF_ERR_EXIT(dbal_db_init_field_type_struct_add(unit, fld_type_prm, fld_type_ent));
    }

    /** handle enums */
    fld_type_ent->nof_enum_values = fld_type_prm->nof_enum_vals;
    if (fld_type_ent->nof_enum_values > 0)
    {
        SHR_IF_ERR_EXIT(dbal_db_init_field_type_enum_add(unit, fld_type_prm, fld_type_ent));
    }

    /** handle Defines */
    fld_type_ent->nof_defines_values = fld_type_prm->nof_defined_vals;
    if (fld_type_ent->nof_defines_values > 0)
    {
        SHR_IF_ERR_EXIT(dbal_db_init_field_type_define_add(unit, fld_type_prm, fld_type_ent));
    }

    /** encoding validation, at the moment encoding only supported for fields less than or equal 32bit */
    if ((fld_type_ent->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE) &&
        (fld_type_ent->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_STRUCT))
    {
        if (fld_type_ent->max_size > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field %s encoding, not supported for field bigger than 32 bit\n",
                         fld_type_ent->name);
        }
    }

    /**Dynamic loading will be checked later*/
    SHR_BITCLR(fld_type_ent->field_indication_bm, DBAL_FIELD_IND_IS_DYNAMIC);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_init_direct_capacity_calculate(
    int unit,
    dbal_logical_table_t * table,
    int *max_capacity)
{
    int ii;
    uint32 capacity = 1;
    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_max = 0, field_min = 0;
        int field_range = 0;
        field_max = table->keys_info[ii].max_value;
        field_min = table->keys_info[ii].min_value;
        field_range = field_max - field_min + 1;

        if (field_range <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Range is 0, cannot calculate capacity. table %s \n",
                         table->table_name);
        }
        capacity *= field_range;
    }
    (*max_capacity) = capacity;

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * The function calculate the max_capacity of the table.
 */
static shr_error_e
dbal_db_init_table_capacity_calculate(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table,
    table_db_struct_t * table_params)
{
    soc_mem_t mem;
    SHR_FUNC_INIT_VARS(unit);

    switch (table->access_method)
    {
        case DBAL_ACCESS_METHOD_KBP:
            
            table->max_capacity = 0;
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_db_init_direct_capacity_calculate(unit, table, &table->max_capacity));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            mem = table->tcam_cs_mapping->memory;
            table->max_capacity = SOC_MEM_INFO(unit, mem).index_max + 1;
            break;

        case DBAL_ACCESS_METHOD_MDB:
            
            table->max_capacity = 0;
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
        {
            {
                if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_direct_capacity_calculate(unit, table, &table->max_capacity));

                }
                else if (table_params->physical_db_id != DBAL_NOF_PHYSICAL_TABLES)
                {
                    table->max_capacity = 0;
                }
                else
                {
                    table->max_capacity = table_params->indirect_table_size_dnx_data.int_val;
                }
            }
            break;
        }
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
        {
            {
                if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_direct_capacity_calculate(unit, table, &table->max_capacity));
                }
                else if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                {
                     /** We assume that Key, KeyMask and Result are mapped to same memory. Get max capacity by the Key mapping */
                    mem =
                        table->hl_mapping_multi_res[0].
                        l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY].memory[0];
                    table->max_capacity = 1 + SOC_MEM_INFO(unit, mem).index_max;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                                 dbal_table_type_to_string(unit, table->table_type), table->table_name);
                }
            }
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal access method\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the key/result fields read from XML to the
 * dbal table DB. also update the max capacity and key size when updating the
 * Read by : dbal_db_init_table_add_interface_to_table
 */
static shr_error_e
dbal_db_init_table_add_fields(
    int unit,
    dbal_logical_table_t * table_entry,
    table_db_struct_t * table_params,
    uint8 is_key_fields,
    int result_set_idx)
{
    int ii;
    int num_of_fields;
    dbal_fields_e field_id;
    dbal_field_types_defs_e field_type;
    table_db_field_params_struct_t *fields_read_data;
    int field_index = 0;
    uint32 field_def_size = 0;
    uint32 field_max_value = 0, field_min_value = 0, field_const_value = 0;
    uint8 field_const_value_valid = 0;
    dbal_table_field_info_t *fields_info;
    int key_length_in_bits = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (is_key_fields)
    {
        fields_info = table_entry->keys_info;
        num_of_fields = table_params->nof_key_fields;
        fields_read_data = table_params->key_fields;
    }
    else
    {
        fields_info = table_entry->multi_res_info[result_set_idx].results_info;
        num_of_fields = table_params->results_set[result_set_idx].nof_res_fields;
        fields_read_data = table_params->results_set[result_set_idx].result_fields;
    }

    for (ii = 0; ii < num_of_fields; ii++)
    {
        dbal_field_types_defs_e curr_field_type;

        if (fields_read_data[ii].valid == 0)
        {
            continue;
        }
        field_id = fields_read_data[ii].field_id;
        fields_info[field_index].field_id = field_id;

        field_type = fields_read_data[ii].field_type;

        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &curr_field_type));
        if (curr_field_type != field_type)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s has multiple types definition: type1=%s type2=%s\n",
                         dbal_field_to_string(unit, fields_info[field_index].field_id),
                         dbal_field_types_def_to_string(unit, curr_field_type),
                         dbal_field_types_def_to_string(unit, field_type));
        }

        if ((field_type == DBAL_FIELD_TYPE_DEF_CORE_ID) && (field_id != DBAL_FIELD_CORE_ID))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field type CORE_ID only valid for field CORE_ID\n");

        }
        fields_info[field_index].field_type = field_type;

        /** get field max size (default size) */
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_def_size));

        /**  get field max size (default size) */
        SHR_IF_ERR_EXIT(dbal_fields_max_value_get(unit, field_id, &field_max_value));

        /**  get field min size (default size) */
        SHR_IF_ERR_EXIT(dbal_fields_min_value_get(unit, field_id, &field_min_value));

        SHR_IF_ERR_EXIT(dbal_fields_const_value_get(unit, field_id, &field_const_value, &field_const_value_valid));

        if (is_key_fields)
        {
            uint8 is_allocator;
            /**  in case that the field is a key field we need to check if the field is allocator */
            SHR_IF_ERR_EXIT(dbal_fields_is_allocator_get(unit, field_id, &is_allocator));
            if (is_allocator)
            {
                SHR_BITSET(fields_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_ALLOCATOR);
                if ((table_entry->allocator_field_id == DBAL_FIELD_EMPTY))
                {
                    table_entry->allocator_field_id = fields_info[field_index].field_id;
                    if (table_entry->access_method == DBAL_ACCESS_METHOD_MDB && (num_of_fields > 1))
                    {
                        
                        table_entry->allocator_field_id = DBAL_FIELD_EMPTY;
                    }
                }
            }
        }

        /** Read field offset */
        fields_info[field_index].offset_in_logical_field = fields_read_data[ii].offset;

        /** permission  */
        fields_info[field_index].permission = fields_read_data[ii].permission;

        if (is_key_fields && (fields_info[field_index].permission != DBAL_PERMISSION_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key field cannot be set to read-only. field %s table %s\n",
                         dbal_field_to_string(unit, fields_info[field_index].field_id), table_entry->table_name);
        }

        if (fields_read_data[ii].size != 0)
        {
            fields_info[field_index].field_nof_bits = fields_read_data[ii].size;
        }
        else
        {
            fields_info[field_index].field_nof_bits = field_def_size;
        }

        /*
         * validate that field size and offset are no longer than field
         * default size
         */
        if ((fields_info[field_index].offset_in_logical_field + fields_info[field_index].field_nof_bits) >
            field_def_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "field %s in table %s is longer than its default max size. offset=%d, size=%d\n",
                         dbal_field_to_string(unit, fields_info[field_index].field_id), table_entry->table_name,
                         fields_info[field_index].offset_in_logical_field, fields_info[field_index].field_nof_bits);
        }

        if ((fields_info[field_index].field_nof_bits + fields_info[field_index].offset_in_logical_field) <= 32)
        {
            int field_effective_size = fields_info[field_index].field_nof_bits +
                fields_info[field_index].offset_in_logical_field;
            dbal_field_print_type_e field_print_type;

            /** Set Max Value of field  */
            SHR_IF_ERR_EXIT(dbal_fields_print_type_get(unit, field_id, &field_print_type));
            if (field_print_type != DBAL_FIELD_PRINT_TYPE_ENUM)
            {
                uint32 max_value_effective_size = utilex_max_value_by_size(field_effective_size);

                /** in case field type size > 32, but field in table <32,
                 *  then we can't access field type size.
                 *  In this case only, we'll set the effective size max value
                 */
                if (field_def_size > 32)
                {
                    fields_info[field_index].max_value = max_value_effective_size;
                }
                else
                {
                    fields_info[field_index].max_value = UTILEX_MIN(field_max_value, max_value_effective_size);
                }
            }
            else
            {
                /** for enum we are using the field type max value */
                fields_info[field_index].max_value = field_max_value;

                /** if the enum size in this table is smaller than the field general type change the max value according to it */
                if ((field_def_size > fields_info[field_index].field_nof_bits)
                    && (fields_read_data[ii].arr_prefix_width == 0))
                {
                    fields_info[field_index].max_value =
                        utilex_max_value_by_size(fields_info[field_index].field_nof_bits);

                }
            }

            /** update with specified in table field max value.
             *  In case no max value defined at table field.
             *  table field default value is UINT_32_MAX */
            if (fields_read_data[ii].max_value != DBAL_DB_INVALID)
            {
                fields_info[field_index].max_value =
                    UTILEX_MIN(fields_info[field_index].max_value, fields_read_data[ii].max_value);
            }

            /** Set Min Value of field  */
            fields_info[field_index].min_value = field_min_value;

            if (fields_read_data[ii].min_value > 0)
            {
                fields_info[field_index].min_value =
                    UTILEX_MAX(fields_info[field_index].min_value, fields_read_data[ii].min_value);
            }
            {
                /** set const value for field, read from field property or from field type */
                if (fields_read_data[ii].const_value_dnx_data.int_val != DBAL_DB_INVALID)
                {
                    fields_info[field_index].const_value = fields_read_data[ii].const_value_dnx_data.int_val;
                    field_const_value_valid = 1;
                }
                else if (field_const_value_valid)
                {
                    fields_info[field_index].const_value = field_const_value;
                }
            }
            if (field_const_value_valid)
            {
                fields_info[field_index].min_value = fields_info[field_index].const_value;
                fields_info[field_index].max_value = fields_info[field_index].const_value;
                SHR_BITSET(fields_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);
            }

            if (!is_key_fields && fields_read_data[ii].arr_prefix_width)
            {
                CONST dbal_field_types_basic_info_t *field_type_info;
                SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, curr_field_type, &field_type_info));

                SHR_IF_ERR_EXIT(dbal_field_arr_prefix_calculate(unit,
                                                                &fields_info[field_index].arr_prefix_size,
                                                                &fields_info[field_index].arr_prefix,
                                                                fields_read_data[ii].arr_prefix_width,
                                                                fields_read_data[ii].arr_prefix_value,
                                                                field_type_info->max_size,
                                                                fields_info[field_index].field_nof_bits));

                if (fields_info[field_index].field_type == DBAL_FIELD_TYPE_DEF_UINT)
                {
                    if (fields_info[field_index].field_id != DBAL_FIELD_RESERVED)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s with type uint cannot use arr prefix\n",
                                     dbal_field_to_string(unit, fields_info[field_index].field_id));
                    }
                }
            }
        }
        else
        {
            /** set min/max values */
            fields_info[field_index].min_value = 0;
            fields_info[field_index].max_value = 0;
            if ((fields_read_data[ii].max_value != DBAL_DB_INVALID) || (fields_read_data[ii].min_value > 0))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Max/Min values cannot be set with fields larger than 32bits. field %s\n",
                             dbal_field_to_string(unit, fields_info[field_index].field_id));
            }

            /** set const value for field, read from field property or from field type */
            if ((fields_read_data[ii].const_value_dnx_data.int_val != DBAL_DB_INVALID) || field_const_value_valid)
            {
                if (field_const_value != 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Const value can only be set to zero with fields larger than 32bits. field %s\n",
                                 dbal_field_to_string(unit, fields_info[field_index].field_id));
                }
                fields_info[field_index].const_value = 0;
                field_const_value_valid = 1;
                /** No need to set min and max here, since they're already validated to be 0 */
                SHR_BITSET(fields_info[field_index].field_indication_bm, DBAL_FIELD_IND_IS_CONST_VALID);
            }
        }
        key_length_in_bits += fields_info[field_index].field_nof_bits;

        /** multiple instances support */
        fields_info[field_index].nof_instances = fields_read_data[ii].nof_instances;
        if (fields_read_data[ii].nof_instances > 1)
        {
            int jj;
            uint8 reverse_order = fields_read_data[ii].is_reverse_order;

            if (is_key_fields)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "not supported multiple instances for key fields. field %s\n",
                             dbal_field_to_string(unit, fields_info[field_index].field_id));
            }
            for (jj = 1; jj < fields_info[field_index].nof_instances; jj++)
            {
                dbal_field_types_defs_e field_type_ins;

                sal_memcpy(&(fields_info[field_index + jj]), &(fields_info[field_index]),
                           sizeof(dbal_table_field_info_t));
                fields_info[field_index + jj].field_id = fields_info[field_index].field_id + jj;

                SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id + jj, &field_type_ins));
                if (field_type_ins != field_type)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s_%03d has multiple types definition: type1=%s type2=%s\n",
                                 dbal_field_to_string(unit, fields_read_data[ii].field_id),
                                 jj,
                                 dbal_field_types_def_to_string(unit, field_type),
                                 dbal_field_types_def_to_string(unit, field_type_ins));
                }
                fields_info[field_index].field_type = field_type;
            }

            /*
             * If table is MDB, or reverse order flag is specify
             */
            if ((table_entry->access_method == DBAL_ACCESS_METHOD_MDB) && !reverse_order)
            {
                for (jj = 0; jj < fields_info[field_index].nof_instances; jj++)
                {
                    SHR_BITSET(fields_info[field_index + jj].field_indication_bm, DBAL_FIELD_IND_IS_INSTANCE_REVERSE);
                }
            }

            if ((table_entry->access_method != DBAL_ACCESS_METHOD_MDB) && reverse_order)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Reverse order of instances is supported only for MDB tables\n");
            }

            field_index += fields_info[field_index].nof_instances;
        }
        else
        {
            field_index++;
        }
    }

    /** calculating the max capacity and the key size */
    if (is_key_fields)
    {
        table_entry->key_size = key_length_in_bits;
        table_entry->nof_key_fields = field_index;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table interface part read from XML to
 * the dbal table DB.
 * The interface is common to all logical tables type
 * Read by : dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_table_add_interface_to_table(
    int unit,
    dbal_logical_table_t * table_entry,
    table_db_struct_t * table_params)
{
    int ii, jj;
    int nof_multi_res_types;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_IF);

    /** Key Fields */
    if (table_params->nof_key_fields > DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more key fields than max allowed.\n", table_entry->table_name);
    }

    SHR_ALLOC_SET_ZERO(table_entry->keys_info, table_params->nof_key_fields * sizeof(dbal_table_field_info_t),
                       "key fields info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dbal_db_init_table_add_fields(unit, table_entry, table_params, TRUE, 0));

    /** for multiple results fields */
    table_entry->nof_result_types = table_params->num_of_results_sets;
    if (table_params->result_type_exists)
    {
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE);
    }

    if (table_entry->nof_result_types > DBAL_MAX_NUMBER_OF_RESULT_TYPES - 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more result types than max allowed.\n", table_entry->table_name);
    }

    /*
     *  Only for MDB tables, RESULT_TYPE can be mapped to SW
     *  state using the below flag
     */
    if (table_params->result_type_in_sw_dnx_data.int_val == 1)
    {
        if (SHR_IS_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
            && (table_entry->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type mapping for table %s."
                         "result type in HL should be mapped in Access\n", table_entry->table_name);
        }
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW);
    }

    /** superset result type is not supported for table that has dynamic result types */
    if (SHR_IS_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
        && sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
    {
        /** allocating result info also for superset result type */
        nof_multi_res_types = table_entry->nof_result_types + 1;
    }
    else if (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") == 0)
    {
        nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);
    }
    else
    {
        nof_multi_res_types = table_entry->nof_result_types;
    }

    SHR_ALLOC_SET_ZERO(table_entry->multi_res_info, (nof_multi_res_types * sizeof(multi_res_info_t)),
                       "multiple results info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    for (ii = 0; ii < table_params->num_of_results_sets; ii++)
    {
        /** Result Fields */
        int nof_results_field = 0;

        for (jj = 0; jj < table_params->results_set[ii].nof_res_fields; jj++)
        {
            if (table_params->results_set[ii].result_fields[jj].valid == 0)
            {
                continue;
            }
            if (table_params->results_set[ii].result_fields[jj].nof_instances > 1)
            {
                nof_results_field += table_params->results_set[ii].result_fields[jj].nof_instances;
            }
            else
            {
                nof_results_field++;
            }
        }

        /** Total Num of Fields */
        table_entry->multi_res_info[ii].nof_result_fields = nof_results_field;

        /** indication if result set if disabled   */
        table_entry->multi_res_info[ii].is_disabled = table_params->results_set[ii].is_disabled;

        table_entry->multi_res_info[ii].results_info = NULL;
        if (table_entry->multi_res_info[ii].nof_result_fields > DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, result type %s has more result fields than max allowed.\n",
                         table_entry->table_name, table_entry->multi_res_info[ii].result_type_name);
        }

        SHR_ALLOC_SET_ZERO(table_entry->multi_res_info[ii].results_info,
                           nof_results_field * sizeof(dbal_table_field_info_t), "results fields info allocation",
                           "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dbal_db_init_table_add_fields(unit, table_entry, table_params, FALSE, ii));
        {
            /** Result type HW value */
            if (table_params->reference_field != DBAL_FIELD_TYPE_DEF_EMPTY)
            {
                table_entry->multi_res_info[ii].refernce_field_id = table_params->reference_field;

                table_entry->multi_res_info[ii].result_type_nof_hw_values = 1;
                SHR_IF_ERR_EXIT(dbal_field_types_enum_name_to_hw_value_get
                                (unit, table_entry->multi_res_info[ii].refernce_field_id,
                                 table_params->results_set[ii].result_type_name,
                                 &(table_entry->multi_res_info[ii].result_type_hw_value[0])));
            }
            else if (table_params->results_set[ii].result_is_mapped)
            {
                for (jj = 0; jj < table_params->results_set[ii].nof_physical_values_for_result_type; jj++)
                {
                    table_entry->multi_res_info[ii].result_type_hw_value[jj] =
                        table_params->results_set[ii].result_type_physical_value[jj];
                }
                table_entry->multi_res_info[ii].result_type_nof_hw_values =
                    table_params->results_set[ii].nof_physical_values_for_result_type;
            }
            else
            {
                table_entry->multi_res_info[ii].result_type_hw_value[0] = ii;
                table_entry->multi_res_info[ii].result_type_nof_hw_values = 1;
            }
        }
        /** Result type HW value */
        sal_strncpy_s(table_entry->multi_res_info[ii].result_type_name, table_params->results_set[ii].result_type_name,
                      sizeof(table_entry->multi_res_info[ii].result_type_name));
    }

    /** allocating the last result type for the superset result type support */
    if (SHR_IS_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
        && sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
    {
        SHR_ALLOC_SET_ZERO(table_entry->multi_res_info[ii].results_info,
                           DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE * sizeof(dbal_table_field_info_t),
                           "dynamic rest type allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    }
    else if (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") == 0)
    {
        int nof_result_fields = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit);

        nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);

        /*
         * allocate result_info for empty result types for future dynamic result types result fields.
         */
        for (ii = table_entry->nof_result_types; ii < nof_multi_res_types; ii++)
        {
            SHR_ALLOC_SET_ZERO(table_entry->multi_res_info[ii].results_info,
                               (nof_result_fields * sizeof(dbal_table_field_info_t)), "results fields info allocation",
                               "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
        }
    }
#if defined(INCLUDE_KBP)
    SHR_ALLOC_SET_ZERO(table_entry->kbp_handles, sizeof(kbp_db_handles_t),
                       "KBP handles allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
#endif

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_IF);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_init_reg_mem_hw_entity_validate(
    int unit,
    int table_id,
    int reg_mem_hw_entity_id,
    uint8 is_reg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_reg && (reg_mem_hw_entity_id == INVALIDr))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s INVALIDr wasn't expected\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if ((!is_reg) && (reg_mem_hw_entity_id == INVALIDm))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s INVALIDm wasn't expected\n",
                     dbal_logical_table_to_string(unit, table_id));
    }

    if (is_reg && (!SOC_REG_IS_VALID(unit, reg_mem_hw_entity_id)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, register %s is invalid for device \n",
                     dbal_logical_table_to_string(unit, table_id), SOC_REG_NAME(unit, reg_mem_hw_entity_id));
    }

    if ((!is_reg) && (!SOC_MEM_IS_VALID(unit, reg_mem_hw_entity_id)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, memory %s is invalid for device \n",
                     dbal_logical_table_to_string(unit, table_id), SOC_MEM_NAME(unit, reg_mem_hw_entity_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Precondition: the reg/mem is valid.
 *(see dbal_db_init_reg_mem_hw_entity_validate)
 */
shr_error_e
dbal_db_init_field_hw_entity_validate(
    int unit,
    int table_id,
    dbal_hl_l2p_field_info_t * access,
    uint8 is_reg,
    uint8 print_error)
{
    int nof_fields;
    soc_field_info_t *fieldinfo;
    soc_field_info_t *fields_array;

    SHR_FUNC_INIT_VARS(unit);

    /** invalid field, nothing to validate   */
    if (access->hw_field == INVALIDf)
    {
        SHR_EXIT();
    }

    if (is_reg)
    {
        nof_fields = SOC_REG_INFO(unit, access->reg[0]).nFields;
        fields_array = SOC_REG_INFO(unit, access->reg[0]).fields;
    }
    else
    {
        nof_fields = SOC_MEM_INFO(unit, access->memory[0]).nFields;
        fields_array = SOC_MEM_INFO(unit, access->memory[0]).fields;
    }
    SOC_FIND_FIELD(access->hw_field, fields_array, nof_fields, fieldinfo);
    if (fieldinfo == NULL)
    {
        if (print_error)
        {
            SHR_ERR_EXIT(_SHR_E_ACCESS, "Table %s, invalid field: %s \n",
                         dbal_logical_table_to_string(unit, table_id), SOC_FIELD_NAME(unit, access->hw_field));
        }
        else
        {
            SHR_SET_CURRENT_ERR(_SHR_E_ACCESS);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** port hw entity (reg or mem).
 *  porting is based on hw entities from J2A0.
 *  Update the ported hw entity only if porting was found */
static shr_error_e
dbal_hw_entity_porting(
    int unit,
    int hw_entity_id_to_port,
    int *ported_hw_entity,
    int is_reg)
{
    int num_of_elements = 0;
    hl_porting_info_t *hl_porting_info;
    int iter;
    int porting_info_hw_entity_id;

    SHR_FUNC_INIT_VARS(unit);

    if (DBAL_IS_JR2_A0)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dbal_tables_porting_info_get(unit, &hl_porting_info));

    num_of_elements = is_reg ? hl_porting_info->nof_registers : hl_porting_info->nof_memories;

    for (iter = 0; iter < num_of_elements; iter++)
    {
        porting_info_hw_entity_id = is_reg ? hl_porting_info->register_ids[iter] : hl_porting_info->memory_ids[iter];

        if (hw_entity_id_to_port == porting_info_hw_entity_id)
        {
            *ported_hw_entity =
                is_reg ? (int) hl_porting_info->
                register_mapped_ids[iter] : (int) hl_porting_info->memory_mapped_ids[iter];
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/** check that a member of the group is valid.
 *  Return not found error when not valid.
 */
static shr_error_e
dbal_db_init_hw_element_group_hw_entity_verify(
    int unit,
    int hw_entity,
    uint8 is_reg)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_reg && !SOC_REG_IS_VALID(unit, (soc_reg_t) hw_entity))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else if (!is_reg && !SOC_MEM_IS_VALID(unit, (soc_mem_t) hw_entity))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

    if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
    {
        char *hw_entity_name;
        if (is_reg)
        {
            hw_entity_name = SOC_REG_NAME(unit, (soc_reg_t) hw_entity);
        }
        else
        {
            hw_entity_name = SOC_MEM_NAME(unit, (soc_mem_t) hw_entity);
        }

        if (DBAL_IS_J2C_A0)
        {
            LOG_VERBOSE(BSL_LOG_MODULE,
                        (BSL_META_U(unit, "dbal_db_init_hw_element_group_handle : " "hw entity not found (%s)\n"),
                         hw_entity_name));
        }
        else
        {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "dbal_db_init_hw_element_group_handle: " "hw entity not found (%s)\n"),
                       hw_entity_name));
        }
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get the hw group info including the hw entities ids
 * this function is called for each hw entity group.
 */
shr_error_e
dbal_db_init_hw_element_group_handle(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id,
    dbal_hl_l2p_field_info_t * access_entry,
    int *is_reg)
{
    hl_group_info_t *group_info;
    int hw_entity_index;
    int hw_entity;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_group_info_get(unit, hw_entity_group_id, &group_info));

    (*is_reg) = group_info->is_reg;

    if (*is_reg)
    {
        SHR_ALLOC_SET_ZERO(access_entry->reg, ((group_info->group_hw_entity_num + 1) * sizeof(soc_reg_t)),
                           "HL access register", "%s%s%s\r\n", dbal_hw_entity_group_to_string(unit, hw_entity_group_id),
                           EMPTY, EMPTY);
    }
    else
    {
        SHR_ALLOC_SET_ZERO(access_entry->memory, ((group_info->group_hw_entity_num + 1) * sizeof(soc_mem_t)),
                           "HL access memory", "%s%s%s\r\n", dbal_hw_entity_group_to_string(unit, hw_entity_group_id),
                           EMPTY, EMPTY);
    }

    /** port each member of the group */
    for (hw_entity_index = 0; hw_entity_index < group_info->group_hw_entity_num; hw_entity_index++)
    {
        hw_entity = group_info->hw_entity_id[hw_entity_index];

        /** check if porting is required*/
        SHR_IF_ERR_EXIT(dbal_hw_entity_porting(unit, hw_entity, &hw_entity, *is_reg));

        SHR_IF_ERR_EXIT(dbal_db_init_hw_element_group_hw_entity_verify(unit, hw_entity, *is_reg));

        if (*is_reg)
        {
            access_entry->reg[hw_entity_index] = hw_entity;
        }
        else
        {
            access_entry->memory[hw_entity_index] = hw_entity;
        }
    }
    if (*is_reg)
    {
        access_entry->reg[hw_entity_index] = INVALIDr;
    }
    else
    {
        access_entry->memory[hw_entity_index] = INVALIDm;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 *
 *
 */
shr_error_e
dbal_db_init_hw_element_group_info_init(
    int unit,
    hl_groups_info_t * groups_info)
{
    SHR_FUNC_INIT_VARS(unit);

    groups_info->nof_groups = DBAL_NOF_HW_ENTITY_GROUPS;
    groups_info->group_info = NULL;

    SHR_ALLOC_SET_ZERO(groups_info->group_info, (DBAL_NOF_HW_ENTITY_GROUPS * sizeof(hl_group_info_t)),
                       "group_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(dbal_init_hw_entity_groups_init(unit, groups_info->group_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table physical mapping part Memory and
 * register access types of hard logic tables
 */
static shr_error_e
dbal_db_init_table_add_hl_memory_register(
    int unit,
    dbal_tables_e table_id,
    dbal_hl_l2p_field_info_t * access_entry,
    table_db_access_params_struct_t * access_params)
{
    int ii, is_reg = 0;
    uint8 is_result_type_mapping;

    SHR_FUNC_INIT_VARS(unit);

    /** handling group */
    if (access_params->hw_entity_group_id != DBAL_HW_ENTITY_GROUP_EMPTY)
    {
        access_entry->hw_entity_group_id = access_params->hw_entity_group_id;
        SHR_IF_ERR_EXIT(dbal_db_init_hw_element_group_handle
                        (unit, access_params->hw_entity_group_id, access_entry, &is_reg));
    }
    else
    {
        is_reg = (access_params->access_type == DBAL_HL_ACCESS_REGISTER);
        SHR_IF_ERR_EXIT(dbal_db_init_reg_mem_hw_entity_validate
                        (unit, table_id, access_params->reg_mem_hw_entity_id, is_reg));

        /** add memory */
        if (!is_reg)
        {
            SHR_ALLOC_SET_ZERO(access_entry->memory, 2 * sizeof(soc_mem_t), "HL access memory",
                               "%s%s%s\r\n", dbal_hw_entity_group_to_string(unit, access_params->reg_mem_hw_entity_id),
                               EMPTY, EMPTY);

            access_entry->memory[0] = access_params->reg_mem_hw_entity_id;
            access_entry->memory[1] = INVALIDm;
            /** Alias Memory */
            if (access_params->alias_reg_mem_hw_entity_id != INVALIDm)
            {
                access_entry->alias_memory = access_params->alias_reg_mem_hw_entity_id;
            }
        }
        else
        {
            SHR_ALLOC_SET_ZERO(access_entry->reg, 2 * sizeof(soc_reg_t), "HL access register",
                               "%s%s%s\r\n", dbal_hw_entity_group_to_string(unit, access_params->reg_mem_hw_entity_id),
                               EMPTY, EMPTY);
            access_entry->reg[0] = access_params->reg_mem_hw_entity_id;
            access_entry->reg[1] = INVALIDr;
            /** Alias Register */
            if (access_params->alias_reg_mem_hw_entity_id != INVALIDr)
            {
                access_entry->alias_reg = access_params->alias_reg_mem_hw_entity_id;
            }
        }
    }

    access_entry->hw_field = access_params->hw_field;


    /** Encoding */
    if (sal_strcmp(access_params->encode_type, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_encode_type_string_to_id(unit, access_params->encode_type,
                                                            &access_entry->encode_info.encode_mode));
        access_entry->encode_info.input_param = access_params->encode_param1_dnx_data.int_val;
    }
    else
    {
        access_entry->encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_NONE;
    }

    access_entry->nof_conditions = access_params->nof_conditions;
    if (access_entry->nof_conditions > 0)
    {
        if (access_entry->nof_conditions > DBAL_FIELD_MAX_NUM_OF_CONDITIONS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more conditions than max allowed.\n",
                         dbal_logical_table_to_string(unit, table_id));
        }

        SHR_ALLOC_SET_ZERO(access_entry->mapping_condition,
                           access_entry->nof_conditions * sizeof(dbal_access_condition_info_t), "HL mapping condition",
                           "%s%s%s\r\n", dbal_logical_table_to_string(unit, table_id), EMPTY, EMPTY);
    }
    /** condition */
    for (ii = 0; ii < access_params->nof_conditions; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_db_init_condition_parsing(unit, table_id, &access_entry->mapping_condition[ii],
                                                       &access_params->access_condition[ii]));
    }
    /** offsets (Array, Entry, Data, block) */

    is_result_type_mapping = access_entry->field_id == DBAL_FIELD_RESULT_TYPE ? TRUE : FALSE;
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->array_offset_info,
                     &access_params->array_offset));
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->block_index_info,
                     &access_params->block_index));
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->entry_offset_info,
                     &access_params->entry_offset));
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->data_offset_info,
                     &access_params->data_offset));
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->alias_data_offset_info,
                     &access_params->alias_data_offset));
    SHR_IF_ERR_EXIT(dbal_db_init_offset_parsing
                    (unit, table_id, is_result_type_mapping, &access_entry->group_offset_info,
                     &access_params->group_offset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * types of hard logic tables
 * The function calculate the total size to allocate to SW part
 * of access payload
 */
static int
dbal_db_init_table_hl_sw_add_length(
    int unit,
    table_db_struct_t * table_params,
    dbal_logical_table_t * table_entry,
    table_db_access_params_struct_t * access_params,
    int res_type_idx)
{
    int jj;
    int current_result_length = 0;

    dbal_table_field_info_t *res_info = table_entry->multi_res_info[res_type_idx].results_info;
    /** calculate max result length */
    current_result_length = 0;
    for (jj = 0; jj < table_entry->multi_res_info[res_type_idx].nof_result_fields; jj++)
    {
        if (access_params->access_field_id == res_info[jj].field_id)
        {
            current_result_length = res_info[jj].field_nof_bits;
            break;
        }
    }
    return current_result_length;
}

/**
 * \brief
 * The function adds the table physical mapping part of hard
 * logic type tables read from XML to the dbal table DB.
 * Read by : dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_add_result_rype_hl_mapping(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table_entry,
    table_db_struct_t * table_params,
    int result_idx,
    int acc_map_idx,
    int *sw_length_bits)
{
    int ii, jj;

    dbal_hl_l2p_field_info_t *access_entry;
    dbal_hard_logic_access_types_e access_type;
    dbal_hl_access_info_t *hl_mapping;

    int has_field_access_error = FALSE;
    int rv_field_access_error = 0;

    CONST static dbal_hl_l2p_field_info_t hl_default_mapping = {
        DBAL_FIELD_EMPTY,   /** field_id */
        0,                  /**hl_tcam_access_type*/
        0,                  /** access_nof_bits*/
        0,                  /** access_offset*/
        {0},                /** encode_info */
        0,                  /** field_pos_in_interface*/
        0,                  /** nof_bits_in_interface*/
        0,                  /** offset_in_interface*/
        NULL,               /** memory[DBAL_MAX_NUMBER_OF_HW_ELEMENTS]*/
        NULL,               /** reg[DBAL_MAX_NUMBER_OF_HW_ELEMENTS]*/
        0,                  /**is_default_non_standard*/
        INVALIDf,           /** hw_field*/
        DBAL_HW_ENTITY_GROUP_EMPTY,  /** hw_entity_group_id*/
        {0},                /** array_offset_info*/
        {0},                /** entry_offset_info*/
        {0},                /** data_offset_info*/
        {0},                /** block_index_info*/
        {0},                /** group_offset_info*/
        INVALIDm,           /** alias_memory*/
        INVALIDr,           /** alias_reg*/
        {0},                /** alias_data_offset_info*/
        0,                  /** nof_conditions*/
        NULL               /** mapping_condition*/
    };

    SHR_FUNC_INIT_VARS(unit);

    hl_mapping = &table_entry->hl_mapping_multi_res[result_idx];
    *sw_length_bits = 0;

    /** access layer */
    for (ii = 0; ii < table_params->hl_access[acc_map_idx].nof_access; ii++)
    {
        /** count the num of access for each access type, for dynamic
         *  memory allocation */
        access_type = table_params->hl_access[acc_map_idx].access[ii].access_type;

        
        /** To Validate with Niv/Ronen
        if (sal_strcmp(table_params->hl_access[acc_map_idx].access[ii].access_name, "NONE") == 0)
        {
            continue;
        }
        */

        hl_mapping->l2p_hl_info[access_type].num_of_access_fields++;

        /** now we've found out the dbal access type,
         *  set the packed fields indication for the hard logic mapping
         *  of
         *  the dbal table. */
        hl_mapping->l2p_hl_info[access_type].is_packed_fields = table_params->hl_access[acc_map_idx].is_packed_fields;
    }

    for (ii = 0; ii < DBAL_NOF_HL_ACCESS_TYPES; ii++)
    {
        /** Allocate the dynamic memory according to nof access */
        hl_mapping->l2p_hl_info[ii].l2p_fields_info = NULL;
        if (hl_mapping->l2p_hl_info[ii].num_of_access_fields > 0)
        {

            SHR_ALLOC(hl_mapping->l2p_hl_info[ii].l2p_fields_info,
                      hl_mapping->l2p_hl_info[ii].num_of_access_fields * sizeof(dbal_hl_l2p_field_info_t),
                      "HL Access allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

            for (jj = 0; jj < hl_mapping->l2p_hl_info[ii].num_of_access_fields; jj++)
            {
                hl_mapping->l2p_hl_info[ii].l2p_fields_info[jj] = hl_default_mapping;
            }
        }
        /** Set the num of access back to zero, will be updated below */
        hl_mapping->l2p_hl_info[ii].num_of_access_fields = 0;
    }

    for (ii = 0; ii < table_params->hl_access[acc_map_idx].nof_access; ii++)
    {
        dbal_fields_e access_field_id;
        /** access type */
        access_type = table_params->hl_access[acc_map_idx].access[ii].access_type;

        access_entry = hl_mapping->l2p_hl_info[access_type].l2p_fields_info +
            hl_mapping->l2p_hl_info[access_type].num_of_access_fields;

        access_field_id = table_params->hl_access[acc_map_idx].access[ii].access_field_id;

        /** check if the field is valid, in case it is not, no need to verify the mapping. */
        if (access_field_id)
        {
            dbal_table_field_info_t *field_info;
            int field_index_in_table;
            dbal_fields_e parent_field_id;
            int rv;

            rv = dbal_tables_field_info_get_internal(unit, table_id, access_field_id,
                                                     0, result_idx, 0, &field_info, &field_index_in_table,
                                                     &parent_field_id);

            if (rv == _SHR_E_NOT_FOUND)
            {
                /*
                 * in this case no need to do the L2P since the field is not valid
                 */
                /*
                 * LOG_CLI((BSL_META("field %s skipped since it is not valid table %s\n"), dbal_field_to_string(unit,
                 * access_field_id), table_entry->table_name));
                 */
                continue;
            }
        }

        hl_mapping->l2p_hl_info[access_type].num_of_access_fields++;
        {
            if (table_entry->table_type == DBAL_TABLE_TYPE_DIRECT)
            {
                access_entry->field_id = access_field_id;
            }
            else if (DBAL_TABLE_IS_TCAM(table_entry))
            {
                /** DBAL_TABLE_TYPE_TCAM the access field names can be key mask or result */
                access_entry->hl_tcam_access_type = table_params->hl_access[acc_map_idx].access[ii].tcam_access_type;
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                             dbal_table_type_to_string(unit, table_entry->table_type), table_entry->table_name);
            }
        }
        access_entry->access_nof_bits = table_params->hl_access[acc_map_idx].access[ii].access_size;
        access_entry->access_offset = table_params->hl_access[acc_map_idx].access[ii].access_offset;

        switch (access_type)
        {
            case DBAL_HL_ACCESS_MEMORY:
            case DBAL_HL_ACCESS_REGISTER:
            {

                int rv = 0;
                rv = dbal_db_init_table_add_hl_memory_register(unit, table_id, access_entry,
                                                               &table_params->hl_access[acc_map_idx].access[ii]);
                if (rv)
                {
                    has_field_access_error = TRUE;
                    rv_field_access_error = rv;
                }
            }
                break;

            case DBAL_HL_ACCESS_SW:
                /*
                 * We can assign all field bits to software or just part of it, for fields that are fully assigned
                 * access_entry->access_nof_bits is 0 here (it is updated in another function later) so we take the
                 * full size of the field from the interface definition for it. otherwise access_entry->access_nof_bits
                 * holds the exact number of bits mapped to sw so we use it to define the sw bits length
                 */
                if (access_entry->access_nof_bits == 0)
                {
                    *sw_length_bits += dbal_db_init_table_hl_sw_add_length(unit, table_params, table_entry,
                                                                           &table_params->hl_access[acc_map_idx].
                                                                           access[ii], result_idx);
                }
                else
                {
                    *sw_length_bits += access_entry->access_nof_bits;
                }

                break;

            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "access type is not supported yet.table:%s\n", table_entry->table_name);
                break;
        }
    }

    if (has_field_access_error)
    {
        SHR_SET_CURRENT_ERR(rv_field_access_error);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * indicate if need to update the status of the table to
 * incompatible image
 */
static uint8
is_dbal_db_init_status_incompatible_image(
    int unit,
    table_db_struct_t * table_params)
{
    /*
     * update status to incompatible image
     */
    return ((!table_params->is_compatible_with_all_images)
            && (table_params->is_standard_1_table) && (!dbal_image_name_is_std_1(unit)));
}

/**
 * \brief
 * The function adds the table physical mapping part of MDB type
 * tables read from XML to the dbal table DB. Read by :
 * dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_table_add_mapping_mdb(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    table_db_struct_t * table_params)
{
    dbal_physical_table_def_t *physical_table;
    SHR_FUNC_INIT_VARS(unit);

    if (table_params->mdb_access->app_db_id_size_dnx_data.int_val != 0)
    {
        table_entry->app_id_size = table_params->mdb_access->app_db_id_size_dnx_data.int_val;
        table_entry->app_id = table_params->mdb_access->app_db_id_dnx_data.int_val;
    }

    SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id
                    (unit, table_params->mdb_access->phy_db, &table_entry->physical_db_id[0]));
    table_entry->nof_physical_tables = 1;

    SHR_IF_ERR_EXIT(dbal_physical_table_get(unit, table_entry->physical_db_id[0], &physical_table));

    
    if (DBAL_MAX_NUM_OF_CORES > 1)
    {
        if ((table_entry->core_mode == DBAL_CORE_MODE_DPC)
            || (physical_table->physical_core_mode == DBAL_CORE_MODE_DPC))
        {
            int core_idx;
            for (core_idx = 1; core_idx < DBAL_MAX_NUM_OF_CORES; core_idx++)
            {
                table_entry->physical_db_id[core_idx] = table_entry->physical_db_id[0] + core_idx;
            }
            table_entry->nof_physical_tables = DBAL_MAX_NUM_OF_CORES;
        }
    }
    else
    {
        /** In case of single core device, the "DPC" DBs should be duplicated */
        int num_of_dbs_to_duplicate = 2;
        if ((table_entry->core_mode == DBAL_CORE_MODE_DPC)
            || (physical_table->physical_core_mode == DBAL_CORE_MODE_DPC))
        {
            int core_idx;
            for (core_idx = 1; core_idx < num_of_dbs_to_duplicate; core_idx++)
            {
                table_entry->physical_db_id[core_idx] = table_entry->physical_db_id[0] + core_idx;
            }
            table_entry->nof_physical_tables = num_of_dbs_to_duplicate;
        }
    }

    if (table_entry->physical_db_id[0] == DBAL_PHYSICAL_TABLE_TCAM)
    {
        if (table_params->pipe_stage == DBAL_STAGE_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM tables have to be mapped to a stage. table:%s\n",
                         table_entry->table_name);
        }
        table_entry->dbal_stage = table_params->pipe_stage;
    }

    /** for LPM tables hitbit is not enabled by default since it affect performance */
    if (table_entry->table_type != DBAL_TABLE_TYPE_LPM)
    {
        SHR_IF_ERR_EXIT(dbal_tables_hitbit_enable(unit, table_id, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table physical mapping part of TCAM CS
 * type tables read from XML to the dbal table DB. Read by :
 * dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_table_add_mapping_tcam_cs(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    table_db_struct_t * table_params)
{
    dbal_tcam_cs_stage_info_t cs_tcam_db_stage_info;
    SHR_FUNC_INIT_VARS(unit);
    table_entry->dbal_stage = table_params->pipe_stage;

    /*
     * Verify that we have a proper DNX data for the table.
     */
    if (table_entry->dbal_stage >= DBAL_NOF_STAGES ||
        dnx_data_ingress_cs.properties.per_stage_get(unit, table_entry->dbal_stage)->valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) does not have DNX data.\n",
                     dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage);
    }

    SHR_IF_ERR_EXIT(dbal_tcam_cs_stage_info_get(unit, table_entry->dbal_stage, &cs_tcam_db_stage_info));

    if (table_entry->key_size > cs_tcam_db_stage_info.line_length_key_only)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DBAL table %s (%d), that uses context selection dbal_stage %s (%d) "
                     "has more bits on key (%d) than the CS TCAM it uses (%d).\n",
                     dbal_logical_table_to_string(unit, table_id), table_id,
                     dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                     table_entry->key_size, cs_tcam_db_stage_info.line_length_key_only);
    }

    /*
     * Verify that dnx_data_ingress_cs.properties.cs_buffer_nof_bits_max_get(unit) is updated and larger or equal to
     * all CS TCAM key sizes.
     */
    if (cs_tcam_db_stage_info.line_length_with_key_size_prefix >
        dnx_data_ingress_cs.properties.cs_buffer_nof_bits_max_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) has more bits on key (%d) "
                     "than the value provided in DNX DATA as the maximum for all CS TCAMs (%d).\n",
                     dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                     cs_tcam_db_stage_info.line_length_with_key_size_prefix,
                     dnx_data_ingress_cs.properties.cs_buffer_nof_bits_max_get(unit));
    }

    /*
     * Verify that the CS TCAM has a non zero key size.
     */
    if (cs_tcam_db_stage_info.line_length_with_key_size_prefix <= 0 || cs_tcam_db_stage_info.line_length_key_only <= 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) has zero key.\n",
                     dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage);
    }

    if (cs_tcam_db_stage_info.supports_half_entries)
    {
        /*
         * Verify that a table that supports half entries has an even key.
         */
        if (((cs_tcam_db_stage_info.line_length_with_key_size_prefix % 2) != 0) ||
            ((cs_tcam_db_stage_info.line_length_key_only % 2) != 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) supports half entries, but line "
                         "length is not even (%d).\n",
                         dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                         cs_tcam_db_stage_info.line_length_with_key_size_prefix);
        }
        /*
         * Verify that the key is big enough to contain the entry size prefix
         */
        if ((cs_tcam_db_stage_info.line_length_with_key_size_prefix <
             (2 * dnx_data_ingress_cs.properties.nof_bits_entry_size_prefix_get(unit))) ||
            (cs_tcam_db_stage_info.line_length_with_key_size_prefix < cs_tcam_db_stage_info.line_length_key_only) ||
            (cs_tcam_db_stage_info.line_length_key_only < 0))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) supports half entries, but key "
                         "size (%d) is not sufficient for entry size prefix.\n",
                         dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                         cs_tcam_db_stage_info.line_length_with_key_size_prefix);
        }
        /*
         * Verify that a table that supports half entries has a key contained within the full key size.
         */
        if (table_entry->key_size > cs_tcam_db_stage_info.line_length_key_only)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Context Selection dbal_stage %s (%d) supports half entries, but key "
                         "size (%d) of DBAL table excceeds full key size (%d) of the CS TCAM.\n",
                         dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                         table_entry->key_size, cs_tcam_db_stage_info.line_length_key_only);
        }
        
        if (table_entry->key_size > (cs_tcam_db_stage_info.line_length_key_only / 2))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Full entries in a CS TCAM that supports half entries not yet supported. "
                         "table %s (%d), CS TCAM %s (%d), key size %d out of %d.\n",
                         dbal_logical_table_to_string(unit, table_id), table_id,
                         dbal_stage_to_string(unit, table_entry->dbal_stage), table_entry->dbal_stage,
                         table_entry->key_size, cs_tcam_db_stage_info.line_length_key_only);
        }
    }

    SHR_ALLOC_SET_ZERO(table_entry->tcam_cs_mapping, sizeof(dbal_cs_access_info_t),
                       "CS mapping allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    table_entry->tcam_cs_mapping->memory = table_params->cs_access->reg_mem_hw_entity_id;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table's physical mapping part for tables
 * of type PEMLA.
 * read from XML to the dbal table DB.
 * called by : dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_table_add_mapping_pemla(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    table_db_struct_t * table_params)
{
    int ii;
    int nof_key_fields, nof_result_fields;

    SHR_FUNC_INIT_VARS(unit);

    table_entry->app_id = 0;

    nof_key_fields = table_entry->nof_key_fields;
    nof_result_fields = table_entry->multi_res_info[0].nof_result_fields;

    SHR_ALLOC_SET_ZERO(table_entry->pemla_mapping.key_fields_mapping, nof_key_fields * sizeof(uint32),
                       "pemla key mapping allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    SHR_ALLOC_SET_ZERO(table_entry->pemla_mapping.result_fields_mapping, nof_result_fields * sizeof(uint32),
                       "pemla result mapping allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    for (ii = 0; ii < nof_key_fields; ii++)
    {
        table_entry->pemla_mapping.key_fields_mapping[ii] = table_params->pemla_access->pemla_key_mapping[ii];
    }

    for (ii = 0; ii < nof_result_fields; ii++)
    {
        table_entry->pemla_mapping.result_fields_mapping[ii] = table_params->pemla_access->pemla_result_mapping[ii];
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds the table's physical mapping part for tables
 * of type HL.
 * read from XML to the dbal table DB.
 * called by : dbal_db_init_logical_table_add
 */
static shr_error_e
dbal_db_init_table_add_mapping_hard_logic(
    int unit,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    table_db_struct_t * table_params)
{
    int max_hl_sw_part_size_bytes = 0, hl_sw_sizse_per_result_type = 0;
    int ii, jj;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL);

    if (table_params->is_hook_active_dnx_data.int_val)
    {
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_IS_HOOK_ACTIVE);
    }

    SHR_ALLOC_SET_ZERO(table_entry->hl_mapping_multi_res,
                       table_entry->nof_result_types * sizeof(dbal_hl_access_info_t),
                       "multiple result direct mapping allocation", "%s%s%s\r\n",
                       table_entry->table_name, EMPTY, EMPTY);

    for (ii = 0; ii < table_entry->nof_result_types; ii++)
    {

        if (table_entry->nof_result_types > 1)
        {
            for (jj = 0; jj < table_entry->nof_result_types; jj++)
            {
                if (sal_strcmp(table_params->results_set[ii].result_type_name,
                               table_params->hl_access[jj].mapping_result_name) == 0)
                {
                    break;
                }
            }
            if (jj == table_entry->nof_result_types)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Cannot find result type %s in result mapping names. table:%s\n",
                             table_params->results_set[ii].result_type_name, table_entry->table_name);
            }
        }
        else
        {
            jj = 0;
        }

        /** for disabled result types, the mapping is set to 0 */
        if (table_params->results_set[ii].is_disabled)
        {
            continue;
        }

        DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_RES_TYPE);
        rv = dbal_db_init_add_result_rype_hl_mapping(unit, table_id, table_entry, table_params,
                                                     ii, jj, &hl_sw_sizse_per_result_type);
        DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL_RES_TYPE);

        if (SHR_FAILURE(rv))
        {
            SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
            SHR_EXIT();
        }
        max_hl_sw_part_size_bytes = UTILEX_MAX(max_hl_sw_part_size_bytes, BITS2BYTES(hl_sw_sizse_per_result_type));
    }

    if (max_hl_sw_part_size_bytes > 0)
    {
        /** Indicate that an Hard logic table has fields mapped to SW */
        table_entry->sw_access_info.sw_payload_length_bytes = max_hl_sw_part_size_bytes;
    }

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_HL);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 * The function adds a logical table read from XML to the dbal
 * tables DB.
 * Main functionality is translating the string from XML to
 * corresponding values
 */
shr_error_e
dbal_db_init_table_add(
    int unit,
    table_db_struct_t * table_params,
    dbal_logical_table_t * table_info)
{
    dbal_logical_table_t *table_entry;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    table_id = table_params->table_id;
    table_entry = table_info + table_id;

    /** set interface data */
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_interface_to_table(unit, table_entry, table_params));

    /** Core Mode, if core mode not set the table is set to HW error and exit (can be because there is no mapping to this
     *  table for this device) */
    if (table_params->core_mode == DBAL_CORE_NONE)
    {
        SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
        SHR_EXIT();
    }

    table_entry->core_mode = table_params->core_mode;

    if ((table_params->is_standard_1_table) || (table_params->is_compatible_with_all_images == FALSE))
    {
        /** indicate that the table is per image */
        SHR_BITSET(table_entry->indications_bm, DBAL_TABLE_IND_IS_IMAGE_SPECIFIC_TABLE);
    }

    /** update status to wrong image
     *  when image is not standard 1 and file is standard 1 */
    if (is_dbal_db_init_status_incompatible_image(unit, table_params))
    {
        /*
         * The table status will be set to DBAL_TABLE_INCOMPATIBLE_IMAGE during init.
         * We do not need to set it again during WB.
         */
        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_INCOMPATIBLE_IMAGE));
        }
        if (table_entry->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC)
        {
            SHR_EXIT();
        }
    }

    switch (table_entry->access_method)
    {
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_hard_logic(unit, table_entry, table_id, table_params));
            break;

        case DBAL_ACCESS_METHOD_MDB:
            DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_MDB);
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_mdb(unit, table_entry, table_id, table_params));
            DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_MDB);
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_TCAM_CS);
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_tcam_cs(unit, table_entry, table_id, table_params));
            DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_TABLE_MAP_TCAM_CS);
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            if (sal_strcmp(table_params->table_size_str, "INVALID") != 0)
            {
                SHR_ALLOC_SET_ZERO(table_entry->sw_access_info.table_size_str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH,
                                   "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
                sal_memcpy(table_entry->sw_access_info.table_size_str, table_params->table_size_str,
                           DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
            }
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_pemla(unit, table_entry, table_id, table_params));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            /** TBD kbp related xml information parsing */
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT(dbal_db_init_table_capacity_calculate(unit, table_id, table_entry, table_params));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_field_type_general_info_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    char *name,
    char *size,
    char *resource,
    char *print_type,
    uint8 is_common,
    uint8 is_default,
    uint8 is_standard_1)
{
    SHR_FUNC_INIT_VARS(unit);
    /** name */
    sal_strncpy_s(cur_field_type_param->name, name, sizeof(cur_field_type_param->name));
    /** size   */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_field_type_param->max_size_dnx_data, size, TRUE, 0, "", ""));
    /**print */
    sal_strncpy_s(cur_field_type_param->print_type, print_type, sizeof(cur_field_type_param->print_type));
    /** resource */
    sal_strncpy_s(cur_field_type_param->resource_name, resource, sizeof(cur_field_type_param->resource_name));

    /** common / default / specific */
    cur_field_type_param->is_common = is_common;
    cur_field_type_param->is_default = is_default;
    cur_field_type_param->is_standard_1 = is_standard_1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_field_type_mapping_restriction_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    char *max_value,
    char *min_value,
    char *const_value)
{

    SHR_FUNC_INIT_VARS(unit);
    /** max value   */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_field_type_param->max_value_dnx_data, max_value, FALSE, 0, "", ""));
    /** min value */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_field_type_param->min_value_dnx_data, min_value, FALSE, 0, "", ""));
    /** const value  */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_field_type_param->const_value_dnx_data, const_value, FALSE, 0, "", ""));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * for field types, for phy mapping, the function set the
 * defaultValue
 */
shr_error_e
dbal_db_init_field_types_phy_mapping_default_value_set(
    int unit,
    field_type_db_struct_t * cur_field_type_param,
    uint8 is_enum,
    char *default_val_string)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_enum)
    {
        sal_strncpy_s(cur_field_type_param->default_val_for_enum,
                      default_val_string, sizeof(cur_field_type_param->default_val_for_enum));
        cur_field_type_param->default_val_valid = TRUE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                        (unit, &cur_field_type_param->default_val_dnx_data, default_val_string, FALSE,
                         DBAL_DB_INIT_DEFAULT_VAL_TEMP, "", ""));

        if (cur_field_type_param->default_val_dnx_data.int_val != DBAL_DB_INIT_DEFAULT_VAL_TEMP)
        {
            cur_field_type_param->default_val_valid = TRUE;
        }
        else
        {
            cur_field_type_param->default_val_valid = FALSE;
            cur_field_type_param->default_val_dnx_data.int_val = 0;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static void
dbal_db_init_offset_init(
    table_db_offset_in_hw_struct_t * offset)
{
    sal_strncpy_s(offset->formula, EMPTY, sizeof(offset->formula));
    offset->formula_int = DBAL_DB_INVALID;
    offset->formula_cb = NULL;
}

shr_error_e
dbal_db_init_table_set_access_register_memory(
    int unit,
    table_db_access_params_struct_t * access_params,
    dbal_hw_entity_group_e hw_entity_group_id,
    int reg_mem_hw_entity_id,
    int hw_field,
    int alias_reg_mem_hw_entity_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ACCESS_REGISTER_MEMORY);

    

    access_params->hw_entity_group_id = hw_entity_group_id;

    /** invalidr/invalidm if has group */
    access_params->reg_mem_hw_entity_id = reg_mem_hw_entity_id;
    dbal_db_init_offset_init(&(access_params->group_offset));
    dbal_db_init_offset_init(&(access_params->array_offset));
    dbal_db_init_offset_init(&(access_params->entry_offset));
    dbal_db_init_offset_init(&(access_params->block_index));
    dbal_db_init_offset_init(&(access_params->data_offset));
    dbal_db_init_offset_init(&(access_params->alias_data_offset));

    /** invalidf is no field specified */
    access_params->hw_field = hw_field;

    /** invalidr/invalidm if alias is invalid */
    access_params->alias_reg_mem_hw_entity_id = alias_reg_mem_hw_entity_id;

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ACCESS_REGISTER_MEMORY);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_access_condition(
    int unit,
    table_db_access_condition_struct_t * access_condition,
    dbal_condition_types_e type,
    dbal_fields_e field,
    char *value,
    uint32 enum_value)
{
    SHR_FUNC_INIT_VARS(unit);
    access_condition->condition_type = type;

    access_condition->field_id = field;

    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &access_condition->value_dnx_data, value, FALSE, 0, "", ""));
    access_condition->enum_val = enum_value;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_access_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    char *field_size,
    char *field_offset,
    char *encode_type,
    char *encode_value)
{
    dbal_db_int_or_dnx_data_info_struct_t field_size_dnx_data, field_offset_dnx_data;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_ACCESS_FIELD);

    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_size_dnx_data, field_size, FALSE, 0, "", ""));
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_offset_dnx_data, field_offset, FALSE, 0, "", ""));

    access_params->access_size = field_size_dnx_data.int_val;
    access_params->access_offset = field_offset_dnx_data.int_val;

    sal_strncpy_s(access_params->encode_type, encode_type, sizeof(access_params->encode_type));
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &access_params->encode_param1_dnx_data, encode_value, FALSE, 0, "", ""));

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_ACCESS_FIELD);
exit:
    SHR_FUNC_EXIT;
}

/**
 * hard logic direct access types
 */
shr_error_e
dbal_db_init_hard_logic_direct_set_access_type(
    int unit,
    table_db_access_params_struct_t * access_params,
    uint8 is_register,
    uint8 is_memory)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_register)
    {
        access_params->access_type = DBAL_HL_ACCESS_REGISTER;
    }
    else if (is_memory)
    {
        access_params->access_type = DBAL_HL_ACCESS_MEMORY;
    }
    else
    {
        access_params->access_type = DBAL_HL_ACCESS_SW;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dbal_db_init_table_set_pemla_mapping_by_index(
    table_db_struct_t * table_param)
{
    int ii;
    /** If field mapping does not exists, map according to index */
    for (ii = 0; ii < table_param->nof_key_fields; ii++)
    {
        table_param->pemla_access->pemla_key_mapping[ii] = ii;
    }
    for (ii = 0; ii < table_param->results_set[0].nof_res_fields; ii++)
    {
        table_param->pemla_access->pemla_result_mapping[ii] = ii;
    }
}

/**
 * \brief
 * function that handles the CS contact selection info from XML to struct. this is used for the autocoder.
 */
shr_error_e
dbal_db_init_table_tcam_cs_mapping_add(
    int unit,
    table_db_struct_t * cur_table_param,
    soc_mem_t mem)
{
    SHR_FUNC_INIT_VARS(unit);

    cur_table_param->cs_access->reg_mem_hw_entity_id = mem;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dbal_db_init_table_db_struct_hl_access_params_clear(
    table_db_access_params_struct_t * access)
{
    sal_memset(access, 0x0, sizeof(table_db_access_params_struct_t));
}

/** 
 * clear hl_access. Don't clear access array itself. 
 * Done before init adding a new access.
 *  
 */
void
dbal_db_init_table_db_struct_hl_access_clear(
    table_db_struct_t * table_db)
{
    int map_idx = 0;
    for (map_idx = 0; map_idx < table_db->num_of_results_sets; map_idx++)
    {
        table_db->hl_access[map_idx].is_packed_fields = 0;
        table_db->hl_access[map_idx].nof_access = 0;
        sal_memset(table_db->hl_access[map_idx].mapping_result_name, 0x0,
                   sizeof(table_db->hl_access[map_idx].mapping_result_name));
    }
}

void
dbal_db_init_table_db_struct_access_clear(
    table_db_struct_t * table_db)
{
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(0, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_ACCESS_CLEAR);
    switch (table_db->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            sal_memset(table_db->mdb_access, 0x0, sizeof(table_db_mdb_access_struct_t));
            break;
        case DBAL_ACCESS_METHOD_TCAM_CS:
            sal_memset(table_db->cs_access, 0x0, sizeof(table_db_cs_access_struct_t));
            break;
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            dbal_db_init_table_db_struct_hl_access_clear(table_db);
            /*
             * sal_memset(table_db->hl_access, 0x0, table_db->num_of_results_sets *
             * sizeof(table_db_hl_access_struct_t)); 
             */
            break;
        case DBAL_ACCESS_METHOD_SW_STATE:
            /** unlike other access method, the don't init a sw_access
             *  because indirect_table_size_dnx_data is shared with other
             *  access method and therefore init at "interface clear"
             *  function */
            break;
        case DBAL_ACCESS_METHOD_PEMLA:
            sal_memset(table_db->pemla_access, 0x0, sizeof(dbal_db_int_or_dnx_data_info_struct_t));
            break;
        case DBAL_ACCESS_METHOD_KBP:
            /**TBD KBP table related information */
            break;
        default:
            break;
    }

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(0, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_ACCESS_CLEAR);
}

void
dbal_init_field_type_db_struct_clear(
    int unit,
    field_type_db_struct_t * cur_field_type_param)
{
    /*
     * save pointers to restore them after clear
     */
    dbal_db_child_field_info_struct_t *childs = cur_field_type_param->childs;
    dbal_db_enum_info_struct_t *enums = cur_field_type_param->enums;
    dbal_db_struct_field_info_struct_t *structs = cur_field_type_param->structs;
    dbal_db_defines_info_struct_t *define = cur_field_type_param->define;
    dbal_db_int_or_dnx_data_info_struct_t *illegal_values_dnx_data = cur_field_type_param->illegal_values_dnx_data;

    sal_memset(cur_field_type_param, 0x0, sizeof(field_type_db_struct_t));
    /** restore pointer after clear */
    cur_field_type_param->childs = childs;
    cur_field_type_param->enums = enums;
    cur_field_type_param->structs = structs;
    cur_field_type_param->define = define;
    cur_field_type_param->illegal_values_dnx_data = illegal_values_dnx_data;
}

/*************************************************************************************************************************************************/


shr_error_e
dbal_db_init_general_info_set(
    int unit,
    table_db_struct_t * cur_table_param,
    dbal_logical_table_t * table_entry,
    dbal_tables_e table_id,
    int valid_dnx_data,
    int *is_valid,
    char *table_name,
    dbal_access_method_e access_method)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_GENERAL_INFO);

    /** set dbal table id */
    cur_table_param->table_id = table_id;

    /*
     * set valid 
     */
    *is_valid = valid_dnx_data;

    if (*is_valid)
    {
        if (!sw_state_is_warm_boot(unit))
        {
            if (table_entry->maturity_level != DBAL_MATURITY_LOW)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "table already initialized %s there are more than one definition of the table \n",
                             table_name);
            }
        }
    }

    /** set access method */
    cur_table_param->access_method = access_method;
    table_entry->access_method = access_method;

    /** table is invalid if:
     *  1) valid table attribute is false
     *  2) Disable table on real device if KBP device is not enabled
     */
    if ((!*is_valid) || ((table_entry->access_method == DBAL_ACCESS_METHOD_KBP) && (!dnx_kbp_device_enabled(unit))))
    {
        table_entry->maturity_level = DBAL_MATURITY_LOW;

        if (!sw_state_is_warm_boot(unit))
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_NOF_TABLE_STATUS));
        }
    }

    /** Name - Printing Name */
    sal_strncpy_s(table_entry->table_name, table_name, sizeof(table_entry->table_name));

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_GENERAL_INFO);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_maturity_level_set(
    int unit,
    dbal_logical_table_t * dbal_entry,
    dbal_maturity_level_e maturity_level)
{
    SHR_FUNC_INIT_VARS(unit);

    dbal_entry->maturity_level = maturity_level;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/*
 * const value
 * 1. Try to parse it as enum value of the field
 * 2. parse with regular parsing function
 */
static shr_error_e
dbal_db_init_field_in_table_const_value_parsing(
    int unit,
    table_db_field_params_struct_t * field_db,
    char *const_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (const_value != NULL)
    {
        uint8 enum_found = FALSE;
        int enum_idx = 0;
        CONST dbal_field_types_basic_info_t *field_type_info;

        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_db->field_type, &field_type_info));
        if (field_type_info->nof_enum_values > 0)
        {
            char enum_name[DBAL_MAX_LONG_STRING_LENGTH];
            sal_strncpy_s(enum_name, field_type_info->name, sizeof(enum_name));
            sal_strncat_s(enum_name, "_", sizeof(enum_name));
            sal_strncat_s(enum_name, const_value, sizeof(enum_name));

            for (enum_idx = 0; enum_idx < field_type_info->nof_enum_values; enum_idx++)
            {
                if ((sal_strcmp(enum_name, field_type_info->enum_val_info[enum_idx].name) == 0) ||
                   
                    (sal_strstr(field_type_info->enum_val_info[enum_idx].name, const_value) != NULL))
                {
                    enum_found = TRUE;
                    field_db->const_value_dnx_data.int_val = enum_idx;
                    break;
                }
            }

            if (!enum_found)
            {

                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Const value for ENUM field in not in enum names. field_name:%s, const name:%s\n",
                             dbal_field_to_string(unit, field_db->field_id), const_value);
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                            (unit, &field_db->const_value_dnx_data, const_value, FALSE, DBAL_DB_INVALID, "", ""));
        }
    }
    else
    {
        field_db->const_value_dnx_data.int_val = DBAL_DB_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/** 
 * Note: the logic on default value has been moved to autocoder. 
*/
shr_error_e
dbal_db_init_table_set_field(
    int unit,
    table_db_field_params_struct_t * field_db,
    dbal_fields_e field_id,
    int valid,
    int size,
    int offset,
    int max_value,
    int min_value,
    char *const_value,
    int nof_instances,
    dbal_field_permission_e permission,
    uint8 is_reverse_order,
    dbal_field_types_defs_e field_type)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_FIELD);
    /** field id   */
    field_db->field_id = field_id;

    field_db->valid = valid;
    field_db->size = size;
    field_db->offset = offset;
    field_db->max_value = max_value;
    field_db->min_value = min_value;
    field_db->nof_instances = nof_instances;

    /** permission: read only, right only, trigger */
    field_db->permission = permission;

    /** reverse order */
    field_db->is_reverse_order = is_reverse_order;

    /** type.
     *  Note: logic is in the autocoder: field type from type or
     *  from name if doesn't exist */
    field_db->field_type = field_type;

    /** Const Value resolution */
    SHR_IF_ERR_EXIT(dbal_db_init_field_in_table_const_value_parsing(unit, field_db, const_value));

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_ADD_FIELD);

exit:
    SHR_FUNC_EXIT;
}

void
dbal_db_init_table_set_field_arr_prefix(
    int unit,
    table_db_field_params_struct_t * field_db,
    int arr_prefix_value,
    int arr_prefix_width)
{
    field_db->arr_prefix_value = arr_prefix_value;
    field_db->arr_prefix_width = arr_prefix_width;
}

shr_error_e
dbal_db_init_table_set_multiple_result_mapping_is_sw_state(
    int unit,
    table_db_struct_t * cur_table_param,
    char *sw_state)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_table_param->result_type_in_sw_dnx_data, sw_state, FALSE, 0, "", ""));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_result_type_physical_values(
    int unit,
    table_db_struct_t * cur_table_param,
    char *logical_value,
    char *result_type_hw_value,
    char *result_type_hw_value1,
    char *result_type_hw_value2,
    char *result_type_hw_value3,
    char *result_type_hw_value4,
    char *result_type_hw_value5,
    char *result_type_hw_value6,
    char *result_type_hw_value7)
{

    int ii;
    int nof_physical_values_for_result_type;

    SHR_FUNC_INIT_VARS(unit);

    nof_physical_values_for_result_type = 0;

    for (ii = 0; ii < cur_table_param->num_of_results_sets; ii++)
    {
        if (sal_strcmp(logical_value, cur_table_param->results_set[ii].result_type_name) == 0)
        {
            if (cur_table_param->results_set[ii].result_is_mapped)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "result type %s has multiple mapping. table:%s\n",
                             logical_value, dbal_logical_table_to_string(unit, cur_table_param->table_id));
            }
            else
            {
                dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data_value;
                cur_table_param->results_set[ii].result_is_mapped = TRUE;
                SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                (unit, &hw_value_dnx_data_value, result_type_hw_value, TRUE, 0, "", ""));
                cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                    hw_value_dnx_data_value.int_val;
                nof_physical_values_for_result_type++;
                if (result_type_hw_value1 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value1, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }

                if (result_type_hw_value2 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value2, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                if (result_type_hw_value3 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value3, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                if (result_type_hw_value4 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value4, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                if (result_type_hw_value5 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value5, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                if (result_type_hw_value6 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value6, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                if (result_type_hw_value7 != NULL)
                {
                    dbal_db_int_or_dnx_data_info_struct_t hw_value_dnx_data;
                    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                                    (unit, &hw_value_dnx_data, result_type_hw_value7, FALSE, -1, "", ""));
                    cur_table_param->results_set[ii].result_type_physical_value[nof_physical_values_for_result_type] =
                        hw_value_dnx_data.int_val;
                    nof_physical_values_for_result_type++;
                }
                cur_table_param->results_set[ii].nof_physical_values_for_result_type =
                    nof_physical_values_for_result_type;
                break;
            }
        }
    }

    if (ii == cur_table_param->num_of_results_sets)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "result type mapping %s was not found in interface. in table:%s\n", logical_value,
                     dbal_logical_table_to_string(unit, cur_table_param->table_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/** \brief
* clear section of the table_db_struct_t which is common between all types of tables.
*/
void
dbal_init_table_db_struct_clear(
    table_db_struct_t * cur_table_param)
{
    table_db_mdb_access_struct_t *mdb_access;
    table_db_hl_access_struct_t *hl_access;
    table_db_cs_access_struct_t *cs_access;
    table_db_pemla_access_struct_t *pemla_access;
    table_db_results_field_set_struct_t *results_set;

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(0, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_CLEAR);
    /*
     * save pointers to restore them aftet clear
     */
    mdb_access = cur_table_param->mdb_access;
    hl_access = cur_table_param->hl_access;
    cs_access = cur_table_param->cs_access;
    pemla_access = cur_table_param->pemla_access;
    results_set = cur_table_param->results_set;

    /** clear section of the table_db_struct_t which is common between all types of tables. */
    sal_memset(cur_table_param, 0x0, sizeof(table_db_struct_t));
    /** restore pointers after clear */
    cur_table_param->mdb_access = mdb_access;
    cur_table_param->hl_access = hl_access;
    cur_table_param->cs_access = cs_access;
    cur_table_param->pemla_access = pemla_access;
    cur_table_param->results_set = results_set;

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(0, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_STRUCT_CLEAR);
}

shr_error_e
dbal_db_init_logical_tables(
    int unit,
    int flags,
    dbal_logical_table_t * table_info)
{
    table_db_struct_t *cur_table_param = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(cur_table_param, sizeof(table_db_struct_t),
              "logical_tables,cur_table_param", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->mdb_access = NULL;
    SHR_ALLOC(cur_table_param->mdb_access, sizeof(table_db_mdb_access_struct_t),
              "logical_table,pemla_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->hl_access = NULL;
    SHR_ALLOC(cur_table_param->hl_access, DBAL_MAX_NUMBER_OF_RESULT_TYPES * sizeof(table_db_hl_access_struct_t),
              "logical_table,hl_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->cs_access = NULL;
    SHR_ALLOC(cur_table_param->cs_access, sizeof(table_db_cs_access_struct_t),
              "logical_table,cs_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->pemla_access = NULL;
    SHR_ALLOC(cur_table_param->pemla_access, sizeof(table_db_pemla_access_struct_t),
              "logical_table,pemla_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->results_set = NULL;
    SHR_ALLOC(cur_table_param->results_set,
              DBAL_MAX_NUMBER_OF_RESULT_TYPES * sizeof(table_db_results_field_set_struct_t),
              "logical_table,results_set", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** tables are initialized separately by AG code */
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_HL_INIT);
    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_init(unit, cur_table_param, table_info));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_HL_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_SW_STATE_INIT);
    SHR_IF_ERR_EXIT(dbal_init_tables_sw_state_init(unit, cur_table_param, table_info));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_SW_STATE_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_TCAMCON_INIT);
    SHR_IF_ERR_EXIT(dbal_init_tables_tcam_context_selection_init(unit, cur_table_param, table_info));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_TCAMCON_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_INIT);
    SHR_IF_ERR_EXIT(dbal_init_tables_pemla_init(unit, cur_table_param, table_info));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_PEMLA_INIT);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_MDB_INIT);
    SHR_IF_ERR_EXIT(dbal_init_tables_mdb_init(unit, cur_table_param, table_info));
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_MDB_INIT);

exit:
    SHR_FREE(cur_table_param->mdb_access);
    SHR_FREE(cur_table_param->hl_access);
    SHR_FREE(cur_table_param->cs_access);
    SHR_FREE(cur_table_param->pemla_access);
    SHR_FREE(cur_table_param->results_set);
    SHR_FREE(cur_table_param);
    SHR_FUNC_EXIT;
}

/***************************************************************************************************************************************/

shr_error_e
dbal_logical_table_t_init(
    int unit,
    dbal_logical_table_t * table_info)
{
    CONST static dbal_logical_table_t table_default = {
        "",                             /** table_name */
        DBAL_MATURITY_LOW,              /** maturity_level */
        0,                              /** nof _labels */
        NULL,                           /** table_labels */
        DBAL_TABLE_TYPE_NONE,           /** table_type */
        0,                              /** max_capacity */
        {0},                            /** indications_bm */
        0,                              /** nof_key_fields */
        NULL,                           /** keys_info */
        0,                              /** key_size */
        0,                              /** core_id_nof_bits */
        DBAL_FIELD_EMPTY,               /** allocator_field_id */
        NULL,                           /** iterator_optimized */
        0,                              /** nof_result_types */
        NULL,                           /** multi_res_info */
        0,                              /** max_payload_size */
        0,                              /** max_nof_result_fields */
        DBAL_CORE_NONE,                 /** core_mode */
        DBAL_NOF_ACCESS_METHODS,        /** access_method */
        {0},                            /** sw_access_info*/
        0,                              /** nof_physical_tables */
        {DBAL_PHYSICAL_TABLE_NONE},     /** physical_db_id */
        0,                              /** app_id */
        0,                              /** app_id_size */
        DBAL_NOF_STAGES,                /** dbal_stage */
        NULL,                           /** l2p_hl_info */
        NULL,                           /** tcam_cs_mapping */
        {NULL},                         /** pemla_mapping */
        NULL                            /** kbp_handles */
    };

    SHR_FUNC_INIT_VARS(unit);

    (*table_info) = table_default;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_dynamic_table_t_init(
    int unit,
    dbal_logical_table_t * table_info)
{
    SHR_FUNC_INIT_VARS(unit);

    table_info->maturity_level = 0;
    table_info->nof_labels = DBAL_TABLE_TYPE_NONE;
    table_info->max_capacity = 0;
    table_info->indications_bm[0] = 0;
    table_info->nof_key_fields = 0;
    table_info->key_size = 0;
    table_info->core_id_nof_bits = 0;
    table_info->allocator_field_id = DBAL_FIELD_EMPTY;
    table_info->nof_result_types = 0;
    table_info->max_payload_size = 0;
    table_info->max_nof_result_fields = 0;
    table_info->core_mode = DBAL_CORE_NONE;
    table_info->access_method = DBAL_NOF_ACCESS_METHODS;
    table_info->sw_access_info.sw_payload_length_bytes = 0;
    table_info->nof_physical_tables = 0;
    table_info->physical_db_id[0] = DBAL_PHYSICAL_TABLE_NONE;
    table_info->physical_db_id[1] = DBAL_PHYSICAL_TABLE_NONE;
    table_info->app_id = 0;
    table_info->app_id_size = 0;
    table_info->dbal_stage = DBAL_NOF_STAGES;

    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_alloc(
    int unit,
    dbal_logical_table_t * table_info)
{
    int table_index;

    SHR_FUNC_INIT_VARS(unit);

    /** Initialize all tables */
    for (table_index = 0; table_index < DBAL_NOF_TABLES; table_index++)
    {
        SHR_IF_ERR_EXIT(dbal_logical_table_t_init(unit, &(table_info[table_index])));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the Superset result type for a specific
 * field with the given field range that exists in one of the
 * possible result types.
 * The function is called as part of the DBAL Init process in
 * order to achieve a Superset result type that includes all the
 * possible fields in all of the available result types for the
 * table, with value max ranges for each field. The call is made 
 * as part of the traverse on all the fields of all the result 
 * types in each table - A call per encountered field. 
 *
 *   \param [in] unit - unit id
 *   \param [in] superset_res_type - A DBAL pointer to a
 *          structure that stores the Superset result type
 *          attributes for a specific DBAL table.
 *   \param [in] field_info - A pointer to a static field
 *          information in the context of a specific result
 *          type.
 * \return DIRECT OUTPUT
 *   shr_error_e
 * \see
 *   dbal_db_init_table_mutual_interface_validation
 */
shr_error_e
dbal_superset_result_type_field_update(
    int unit,
    multi_res_info_t * superset_res_type,
    dbal_table_field_info_t * field_info)
{
    int field_idx;
    int superset_buffer_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Go over the fields that are already allocated for the Superset result type to check 
     * whether the given field is included. If not found, the field will be added to the 
     * Superset result type 
     */
    for (field_idx = 0; field_idx < (superset_res_type->nof_result_fields); field_idx++)
    {
        if (superset_res_type->results_info[field_idx].field_id == field_info->field_id)
        {
            break;
        }
    }

    /*
     * If the field wasn't found in the Superset result type - Add it with the 
     * given field attributes as the last field in the Superset result type.
     */
    if (field_idx == (superset_res_type->nof_result_fields))
    {
        superset_res_type->results_info[field_idx].field_id = field_info->field_id;
        superset_res_type->results_info[field_idx].field_type = field_info->field_type;
        superset_res_type->results_info[field_idx].nof_instances = field_info->nof_instances;
        superset_res_type->results_info[field_idx].field_nof_bits = field_info->field_nof_bits;
        superset_res_type->results_info[field_idx].max_value = field_info->max_value;
        superset_res_type->results_info[field_idx].min_value = field_info->min_value;
        superset_res_type->results_info[field_idx].offset_in_logical_field = field_info->offset_in_logical_field;
        superset_res_type->nof_result_fields++;
    }
    /*
     * If the field was found in the Superset result type - Check the existing field attributes 
     * and update them if the given field's attributes exceeds them.
     */
    else
    {
        /*
         * The field max_value attribute has to be calculated with the field's prefix in order 
         * to have a correct field size allocation in the Superset result type
         */
        uint32 tmp_field_value = 0, field_max_value = 0;

        if ((field_info->arr_prefix_size) &&
            (!(SHR_BITGET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM))))
        {
            tmp_field_value = ((1 << field_info->field_nof_bits) - 1);
            SHR_IF_ERR_EXIT(dbal_fields_encode_prefix(unit, &tmp_field_value, field_info->arr_prefix,
                                                      field_info->field_nof_bits,
                                                      field_info->field_nof_bits + field_info->arr_prefix_size,
                                                      &field_max_value));

        }
        else
        {
            field_max_value = field_info->max_value;
        }

        if (superset_res_type->results_info[field_idx].field_nof_bits <
            (field_info->field_nof_bits + field_info->arr_prefix_size))
        {
            superset_res_type->results_info[field_idx].field_nof_bits =
                field_info->field_nof_bits + field_info->arr_prefix_size;
        }

        if (superset_res_type->results_info[field_idx].max_value < field_max_value)
        {
            superset_res_type->results_info[field_idx].max_value = field_max_value;
        }

        if (superset_res_type->results_info[field_idx].min_value > field_info->min_value)
        {
            superset_res_type->results_info[field_idx].min_value = field_info->min_value;
        }

        if (superset_res_type->results_info[field_idx].offset_in_logical_field > field_info->offset_in_logical_field)
        {
            superset_res_type->results_info[field_idx].offset_in_logical_field = field_info->offset_in_logical_field;
        }
    }

    /*
     * In case it's not the first field whose offset is always 0, rely on the previous field 
     * in the buffer to calculate the offset
     */
    if (field_idx)
    {
        superset_buffer_offset = superset_res_type->results_info[field_idx - 1].bits_offset_in_buffer +
            superset_res_type->results_info[field_idx - 1].field_nof_bits;
    }

    /*
     * Update the field offset for all the fields in the Superset result type from 
     * the affected field and onwards
     */
    for (; field_idx < (superset_res_type->nof_result_fields); field_idx++)
    {
        superset_res_type->results_info[field_idx].bits_offset_in_buffer = superset_buffer_offset;
        if (superset_res_type->results_info[field_idx].field_id == DBAL_FIELD_EMPTY)
        {
            break;
        }
        superset_buffer_offset += superset_res_type->results_info[field_idx].field_nof_bits;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * return TRUE if table support bit map allocation for iterator
 * Add here tables for using bitmap allocation
 */
static uint8
dbal_db_init_table_iterator_bitmap_allocation_supported(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table)
{
    /**For lif tables, we use bitmap optimization to reduce performance of the iterator (otherwise traverse takes too much time...)*/
    if ((dbal_logical_table_is_out_lif_or_rif_allocator_eedb(table))
        || (dbal_physical_table_is_in_lif(table->physical_db_id[0])))
    {
        return TRUE;
    }

    switch (table_id)
    {
        /** The Example table use this feature for verifying the bitmap implementation */
        case DBAL_TABLE_EXAMPLE_TABLE_CONST_VALUES:
        case DBAL_TABLE_EXAMPLE_TABLE_ILLEGAL_VALUES:
        case DBAL_TABLE_EXAMPLE_REG_MULTIPLE_BLOCKS:
            return TRUE;
            break;

        case DBAL_TABLE_INGRESS_PP_PORT:
        case DBAL_TABLE_EGRESS_PP_PORT:
            return TRUE;
            break;

        default:
            break;
    }

    return FALSE;
}

static shr_error_e
dbal_db_init_table_iterator_bitmap_allocation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table)
{
    uint32 alloc_bit_num = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((table->nof_key_fields == 1) && (table->max_capacity))
    {
        alloc_bit_num = table->max_capacity;
    }
    else if (table->key_size > 0)
    {
        alloc_bit_num = (0x1 << table->key_size);
    }

    if (alloc_bit_num > 0)
    {
        int core_index;

        SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED);

        SHR_ALLOC_SET_ZERO(table->iterator_optimized, (DBAL_MAX_NUM_OF_CORES * sizeof(uint32 *)),
                           "iterator optimized info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);

        if (!sw_state_is_warm_boot(unit))
        {
            DBAL_SW_STATE_TBL_PROP.iterator_optimized.alloc(unit, table_id);
        }

        for (core_index = 0; core_index < DBAL_MAX_NUM_OF_CORES; core_index++)
        {
            SHR_ALLOC_SET_ZERO(table->iterator_optimized[core_index], WORDS2BYTES(BITS2WORDS(alloc_bit_num) + 1),
                               "iterator optimized info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
            if (!sw_state_is_warm_boot(unit))
            {
                DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.alloc_bitmap(unit, table_id, core_index,
                                                                              alloc_bit_num);
            }
            else
            {
                /** restore bitmap values after WB */
                DBAL_SW_STATE_TBL_PROP.iterator_optimized.bitmap.bit_range_read(unit, table_id, core_index, 0, 0,
                                                                                alloc_bit_num,
                                                                                (SHR_BITDCL *)
                                                                                table->iterator_optimized[core_index]);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_mutual_interface_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table)
{
    int jj, kk, qq;
    uint8 is_core_in_key;
    int bits_offset_in_key, bits_offset_in_payload;
    int max_payload_size, max_nof_result_fields;
    dbal_table_field_info_t *field_info;
    dbal_table_status_e table_status;
    multi_res_info_t *superset_res_type = NULL;

    SHR_FUNC_INIT_VARS(unit);

    is_core_in_key = FALSE;
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    if (sw_state_is_warm_boot(unit))
    {
        if ((table_status == DBAL_TABLE_NOT_INITIALIZED) || (table_status == DBAL_TABLE_HW_ERROR))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "IN WB and Init Not preformed on table %s\n", table->table_name);
        }
    }
    else
    {
        /**  check that table is not already initiated */
        if ((table_status > DBAL_TABLE_NOT_INITIALIZED) && (table_status != DBAL_TABLE_INCOMPATIBLE_IMAGE))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Init alredy preformed on table %s\n", table->table_name);
        }
    }

    /** Validate that the core indication are valid */
    if ((table->core_mode != DBAL_CORE_MODE_SBC) && (table->core_mode != DBAL_CORE_MODE_DPC))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid core mode for table %s\n", table->table_name);
    }

    /*
     *  Key Fields:
     *  1. Calculate it offset in buffer
     *  2. For CORE_ID, validate its size
     *  3. Check for non repetition field in key fields
     *  4. Calculate the total size of the key
     */
    bits_offset_in_key = 0;
    for (jj = 0; jj < table->nof_key_fields; jj++)
    {
        field_info = &(table->keys_info[jj]);
        if (field_info->field_id == DBAL_FIELD_EMPTY)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid field ID in table %s \n", table->table_name);
        }

        if (field_info->field_id == DBAL_FIELD_CORE_ID)
        {
            /**  For CORE_ID, validate its size */
            is_core_in_key |= TRUE;
            if (field_info->field_nof_bits != DBAL_CORE_SIZE_IN_BITS)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field core ID has to be %d bit(s) field. table %s\n",
                             DBAL_CORE_SIZE_IN_BITS, table->table_name);
            }
            table->core_id_nof_bits = field_info->field_nof_bits;
        }
        else
        {
            /**  Calculate field offset in buffer */
            field_info->bits_offset_in_buffer = bits_offset_in_key;
            bits_offset_in_key += field_info->field_nof_bits;
        }

        /** Check for non repetition field in key fields  */
        for (kk = jj + 1; kk < table->nof_key_fields; kk++)
        {
            uint8 is_duplication = FALSE;
            SHR_IF_ERR_EXIT(dbal_db_check_field_duplication(unit, field_info->field_id,
                                                            table->keys_info[kk].field_id, &is_duplication));
            if (is_duplication)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "The field %s is appeared more than once in the table %s\n",
                             dbal_field_to_string(unit, field_info->field_id), table->table_name);
            }
        }
        /** update field indications */
        if (dbal_fields_is_parent_field(unit, field_info->field_id))
        {
            SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD);
        }
        if (dbal_fields_is_field_encoded(unit, field_info->field_id))
        {
            SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED);
        }

        {
            dbal_field_print_type_e field_print_type;
            dbal_fields_print_type_get(unit, field_info->field_id, &field_print_type);
            if ((field_print_type == DBAL_FIELD_PRINT_TYPE_MAC) || (field_print_type == DBAL_FIELD_PRINT_TYPE_IPV6))
            {
                SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS);
            }
            if (field_print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
            {
                SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM);
            }
        }
    }

    if (table->key_size > DBAL_PHYSICAL_KEY_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s - key size (%d) is larger than max allowed (%d).\n",
                     table->table_name, table->key_size, DBAL_PHYSICAL_KEY_SIZE_IN_BITS);
    }

    /** Check the correct usage of core mode and core id field */
    if (table->core_mode == DBAL_CORE_MODE_DPC && !is_core_in_key)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DPC tables must have core field in Key table %s\n", table->table_name);
    }

    /*
     *  Result Fields,  for each result type:
     *  1. CORE_ID cannot be a result field - validate it
     *  2. Calculate it offset in buffer
     *  3. Check for non repetition field in key fields
     *  4. Calculate the total size of the payload
     *  5. Keep the max payload size and max num of result fields for all result types
     *  6. updating superset result type
     */
    max_payload_size = 0;
    max_nof_result_fields = 0;
    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE)
        && sal_strcasecmp(table->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
    {
        superset_res_type = &(table->multi_res_info[table->nof_result_types]);
    }

    for (qq = 0; qq < table->nof_result_types; qq++)
    {
        multi_res_info_t *result_type_set = &(table->multi_res_info[qq]);
        bits_offset_in_payload = 0;
        result_type_set->entry_payload_size = 0;
        for (jj = 0; jj < result_type_set->nof_result_fields; jj++)
        {
            field_info = &(result_type_set->results_info[jj]);
            if (field_info->field_id == DBAL_FIELD_EMPTY)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid field ID in table %s, result type %s \n",
                             table->table_name, result_type_set->result_type_name);
            }

            /** CORE_ID cannot be a result field - validate it */
            if (field_info->field_id == DBAL_FIELD_CORE_ID)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "The field CORE_ID cannot be used as result fields. table %s \n",
                             table->table_name);
            }

            if (field_info->arr_prefix_size != 0)
            {
                
            }

            /**  Calculate field offset in buffer */
            result_type_set->results_info[jj].bits_offset_in_buffer = bits_offset_in_payload;
            bits_offset_in_payload += result_type_set->results_info[jj].field_nof_bits;
            /** Calculate the total size of the result */
            result_type_set->entry_payload_size += result_type_set->results_info[jj].field_nof_bits;
            /** Check for non repetition field in result fields  */
            for (kk = jj + 1; kk < result_type_set->nof_result_fields; kk++)
            {
                uint8 is_duplication = FALSE;
                SHR_IF_ERR_EXIT(dbal_db_check_field_duplication(unit, field_info->field_id,
                                                                table->multi_res_info[qq].results_info[kk].field_id,
                                                                &is_duplication));
                if (is_duplication)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "The field %s is appear more than once in the table %s, result type %s\n",
                                 dbal_field_to_string(unit, result_type_set->results_info[kk].field_id),
                                 table->table_name, result_type_set->result_type_name);
                }
            }

            /** update field indications */
            if (dbal_fields_is_parent_field(unit, field_info->field_id))
            {
                SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_PARENT_FIELD);
            }
            if (dbal_fields_is_field_encoded(unit, field_info->field_id))
            {
                SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENCODED);
            }
            {
                dbal_field_print_type_e field_print_type;
                dbal_fields_print_type_get(unit, field_info->field_id, &field_print_type);
                if ((field_print_type == DBAL_FIELD_PRINT_TYPE_MAC) || (field_print_type == DBAL_FIELD_PRINT_TYPE_IPV6))
                {
                    SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ADDRESS);
                }
                if (field_print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
                {
                    SHR_BITSET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_FIELD_ENUM);
                }
            }

            /** Update the superset result type for each encountered field.
             *  The update relies on the values of field_indication_bm and
             *  therefore the call needs to be after field_indication_bm is
             *  set */
            if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE) && superset_res_type)
            {
                SHR_IF_ERR_EXIT(dbal_superset_result_type_field_update(unit, superset_res_type, field_info));
            }
        }

        /** keep the max payload size and max number of result fields for all result types */
        max_payload_size = UTILEX_MAX(max_payload_size, result_type_set->entry_payload_size);
        max_nof_result_fields = UTILEX_MAX(max_nof_result_fields, result_type_set->nof_result_fields);
        if (superset_res_type)
        {
            sal_strncpy_s(superset_res_type->result_type_name, "superset", DBAL_MAX_STRING_LENGTH - 1);
        }
    }
    table->max_payload_size = max_payload_size;
    table->max_nof_result_fields = max_nof_result_fields;
    if (table->max_payload_size > DBAL_PHYSICAL_RES_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s - result size (%d) is larger than max allowed (%d).\n",
                     table->table_name, table->max_payload_size, DBAL_PHYSICAL_RES_SIZE_IN_BITS);
    }

    /*
     * currently only a single static field is supported for
     */
    if ((table_id == DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION)
        && (table->nof_result_types != ETM_PP_DESCRIPTOR_EXPANSION_NOF_STATIC_RES_TYPES))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table ETM_PP_DESCRIPTOR_EXPANSION has more SATIC result types than expected!\n");
    }

    if (dbal_db_init_table_iterator_bitmap_allocation_supported(unit, table_id, table))
    {
        SHR_IF_ERR_EXIT(dbal_db_init_table_iterator_bitmap_allocation(unit, table_id, table));
    }

    /*
     * Set non-direct_optimized indication for all MDB LPM (KAPS) tables.
     * This means that if all result fields are set, it is possible to just commit the entry.
     * The access layer knows whether the entry needs to be added or updated and will perform the action accordingly.
     * This is done for optimization of the entry insertion rate.
     */
    if ((table->table_type == DBAL_TABLE_TYPE_LPM) && (table->access_method == DBAL_ACCESS_METHOD_MDB))
    {
        SHR_IF_ERR_EXIT(dbal_tables_indication_set(unit, table_id, DBAL_TABLE_IND_IS_NONE_DIRECT_OPTIMIZED, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function add some validation for hl access with packed
 * fields:
 * checks that the register/memory entry is fully mapped.
 *
 */
static shr_error_e
dbal_db_init_table_hl_access_packed_fields_validation(
    int unit,
    dbal_logical_table_t * table,
    dbal_hard_logic_access_types_e access_type,
    multi_res_info_t * result_type_set,
    dbal_hl_l2p_field_info_t * l2p_field_info)
{

    uint32 size_of_register_or_memory_entry = 0;
    int nof_fields = 0;
    soc_field_info_t *fields_array;
    int ii = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** calculate the size for mem/register */
    switch (access_type)
    {
        case (DBAL_HL_ACCESS_MEMORY):
        {
            soc_mem_t mem = l2p_field_info->memory[0];
            nof_fields = SOC_MEM_INFO(unit, mem).nFields;
            fields_array = SOC_MEM_INFO(unit, mem).fields;

            for (ii = 0; ii < nof_fields; ii++)
            {
                /** We skip the last field (ECC) */
                char *field_name = SOC_FIELD_NAME(unit, fields_array[ii].field);
                if (sal_strcasecmp(field_name, "ecc") == 0)
                {
                    continue;
                }
                size_of_register_or_memory_entry += fields_array[ii].len;
            }
            break;
        }
        case (DBAL_HL_ACCESS_REGISTER):
        {
            soc_reg_t reg = l2p_field_info->reg[0];
            nof_fields = SOC_REG_INFO(unit, reg).nFields;
            fields_array = SOC_REG_INFO(unit, reg).fields;

            for (ii = 0; ii < nof_fields; ii++)
            {
                size_of_register_or_memory_entry += fields_array[ii].len;
            }
            break;
        }
        /** This case where we have packed_fields for access mode
         *  different than memory/register shouldn't happen because of
         *  dbal xml validation. */
        case (DBAL_HL_ACCESS_SW):
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "packed fields is not suported for sw access\n");
    }

    if (size_of_register_or_memory_entry != result_type_set->entry_payload_size)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "In table %s, packed fields tag can be used only when size of results (%d) equal size of register/memory entry size (%d).\n",
                     table->table_name, result_type_set->entry_payload_size, size_of_register_or_memory_entry);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function add some validations and general properties calculations for SW ONLY tables.
 */
static shr_error_e
dbal_db_init_table_sw_only_validation(
    int unit,
    dbal_logical_table_t * table)
{
    int qq;
    int max_sw_payload_size;
    multi_res_info_t *result_type_set;
    SHR_FUNC_INIT_VARS(unit);
    max_sw_payload_size = 0;
    for (qq = 0; qq < table->nof_result_types; qq++)
    {
        result_type_set = &(table->multi_res_info[qq]);
        max_sw_payload_size = UTILEX_MAX(max_sw_payload_size, BITS2BYTES(result_type_set->entry_payload_size));
    }
    table->sw_access_info.sw_payload_length_bytes = max_sw_payload_size;
    if (table->sw_access_info.sw_payload_length_bytes > DBAL_PHYSICAL_RES_SIZE_IN_BYTES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s, result length for SW table id %d bytes exceeding the max %u bytes.\n",
                     table->table_name, table->sw_access_info.sw_payload_length_bytes,
                     (unsigned) DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function validate the table interface structure
 */
static shr_error_e
dbal_db_init_table_reverse_instance_order(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table)
{
    int jj, kk, qq;
    dbal_table_field_info_t *field_info;
    dbal_table_status_e table_status;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
    /**  check that table is not already initiated */
    if ((table_status > DBAL_TABLE_NOT_INITIALIZED) && (table_status != DBAL_TABLE_INCOMPATIBLE_IMAGE)
        && !sw_state_is_warm_boot(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Init alredy preformed on table %s\n", table->table_name);
    }

    for (jj = 0; jj < table->nof_key_fields;)
    {
        int nof_instance;
        field_info = &(table->keys_info[jj]);
        nof_instance = field_info->nof_instances;
        if ((nof_instance > 1) && (SHR_BITGET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_INSTANCE_REVERSE)))
        {
            for (kk = 0; kk < nof_instance / 2; kk++)
            {
                int offset_temp = table->keys_info[jj + kk].bits_offset_in_buffer;
                table->keys_info[jj + kk].bits_offset_in_buffer =
                    table->keys_info[jj + nof_instance - kk - 1].bits_offset_in_buffer;
                table->keys_info[jj + nof_instance - kk - 1].bits_offset_in_buffer = offset_temp;
            }
        }
        jj += nof_instance;
    }

    for (qq = 0; qq < table->nof_result_types; qq++)
    {
        for (jj = 0; jj < table->multi_res_info[qq].nof_result_fields;)
        {
            int nof_instance;
            field_info = &(table->multi_res_info[qq].results_info[jj]);
            nof_instance = field_info->nof_instances;
            if ((nof_instance > 1) && (SHR_BITGET(field_info->field_indication_bm, DBAL_FIELD_IND_IS_INSTANCE_REVERSE)))
            {
                for (kk = 0; kk < nof_instance / 2; kk++)
                {
                    int offset_temp = table->multi_res_info[qq].results_info[jj + kk].bits_offset_in_buffer;
                    table->multi_res_info[qq].results_info[jj + kk].bits_offset_in_buffer =
                        table->multi_res_info[qq].results_info[jj + nof_instance - kk - 1].bits_offset_in_buffer;
                    table->multi_res_info[qq].results_info[jj + nof_instance - kk - 1].bits_offset_in_buffer =
                        offset_temp;
                }
            }
            jj += nof_instance;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_mdb_validation(
    int unit,
    dbal_tables_e table_id,
    dbal_logical_table_t * table)
{
    int qq, kk;
    int bits_offset_in_buffer;
    multi_res_info_t *result_type_set;
    uint32 key_width, key_prefix_size;
    SHR_FUNC_INIT_VARS(unit);
    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
#if defined(INCLUDE_KBP)
        /** in case of KBP no need the mdb_id + the prefix size is dynamic according to the table updating the table
         *  parameters according to the KBP configurations */
        SHR_IF_ERR_EXIT(dnx_kbp_valid_key_width(unit, table->key_size, &key_width));
        /*
         * For field group (identified as dynamic tables) we do not override max_payload_size with the DB size.
         * For other we do at the moment.
         */
        if (table_id < DBAL_NOF_TABLES)
        {
            uint32 payload_size;
            SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, table->max_payload_size, &payload_size));
            table->max_payload_size = payload_size;
        }
        key_prefix_size = 0;
#else
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "KBP lib is not compiled. Cannot create KBP tables\n");
#endif
    }
    else
    {
        key_width = dnx_data_mdb.kaps.key_width_in_bits_get(unit);
        key_prefix_size = dnx_data_mdb.kaps.key_prefix_length_get(unit);
    }

    /** Invert the order of result buffer adding the offset for KBP */
    for (qq = 0; qq < table->nof_result_types; qq++)
    {
        result_type_set = &(table->multi_res_info[qq]);
        bits_offset_in_buffer = 0;
        for (kk = result_type_set->nof_result_fields - 1; kk >= 0; kk--)
        {
            result_type_set->results_info[kk].bits_offset_in_buffer = bits_offset_in_buffer;
            bits_offset_in_buffer += result_type_set->results_info[kk].field_nof_bits;
            if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RESULT_TYPE_MAPPED_TO_SW)
                && (result_type_set->results_info[kk].field_id == DBAL_FIELD_RESULT_TYPE))
            {
                table->sw_access_info.sw_payload_length_bytes =
                    BITS2BYTES(result_type_set->results_info[kk].field_nof_bits);
            }
            if (result_type_set->results_info[kk].field_id == DBAL_FIELD_NEXT_OUTLIF_POINTER)
            {
                result_type_set->has_link_list = TRUE;
            }
        }
    }

    /*
     *  Invert the order of key buffer:
     *  1. LPM (KAPS) table's key is aligned to (MSB-KAPS_PREFIX)
     *  with fixed key size of dnx_data_mdb.kaps.key_width_in_bits_get(unit)
     *  2. Other types just revert order
     *  3. direct tables key is limited to 32bits
     */
    bits_offset_in_buffer = 0;
    if ((table->table_type == DBAL_TABLE_TYPE_LPM) || (table->access_method == DBAL_ACCESS_METHOD_KBP))
    {
        for (kk = 0; kk < table->nof_key_fields; kk++)
        {
            if (table->keys_info[kk].field_id != DBAL_FIELD_CORE_ID)
            {
                bits_offset_in_buffer += table->keys_info[kk].field_nof_bits;
                table->keys_info[kk].bits_offset_in_buffer = key_width - (key_prefix_size + bits_offset_in_buffer);
                if (table->keys_info[kk].bits_offset_in_buffer < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s, invalid key size for LPM table.\n", table->table_name);
                }
            }
        }
        table->key_size = key_width;
    }
    else
    {
        for (kk = table->nof_key_fields - 1; kk >= 0; kk--)
        {
            if (table->keys_info[kk].field_id != DBAL_FIELD_CORE_ID)
            {
                table->keys_info[kk].bits_offset_in_buffer = bits_offset_in_buffer;
                bits_offset_in_buffer += table->keys_info[kk].field_nof_bits;
            }
        }
    }

    if ((table->key_size > SAL_UINT32_NOF_BITS) && (table->table_type == DBAL_TABLE_TYPE_DIRECT))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "MDB tables's key of type direct is limited to 32 bit. table %s\n",
                     table->table_name);
    }

    if (table->key_size > DBAL_PHYSICAL_KEY_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s - key size (%d) is larger than max allowed (%d).\n",
                     table->table_name, table->key_size, DBAL_PHYSICAL_KEY_SIZE_IN_BITS);
    }
    if (table->max_payload_size > DBAL_PHYSICAL_RES_SIZE_IN_BITS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s - result size (%d) is larger than max allowed (%d).\n",
                     table->table_name, table->max_payload_size, DBAL_PHYSICAL_RES_SIZE_IN_BITS);
    }

    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
        if (DBAL_TABLE_IS_TCAM(table))
        {
            SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_PRIORITY_SUPPORTED);
        }
    }
    else
    {
        if (!sw_state_is_warm_boot(unit))
        {
            /** This code can be removed once PMF team set the learning indication bit when creating the table
             * currently the hw support learning but the API does not.*/
            if ((table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_LEXEM)
                || (table->physical_db_id[0] == DBAL_PHYSICAL_TABLE_SEXEM_3)
                || (table->physical_db_id[1] == DBAL_PHYSICAL_TABLE_LEXEM)
                || (table->physical_db_id[1] == DBAL_PHYSICAL_TABLE_SEXEM_3))
            {
                SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_LEARNING_EN);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * post processing for hard logic Tcam tables
 */
static shr_error_e
dbal_db_init_table_hl_validation_tcam(
    int unit,
    dbal_logical_table_t * table)
{
    int iter;
    dbal_hl_l2p_info_t *access_info = &(table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY]);
    soc_mem_info_t *meminfo;
    soc_field_info_t *fieldinfo;
    uint32 data_size;
    soc_mem_t memory;

    SHR_FUNC_INIT_VARS(unit);

    /** Verify that table is not mapped to a register */
    if (table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_REGISTER].num_of_access_fields != 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "HL TCAM does not support mapping to register. Table: %s\n", table->table_name);
    }
    /*
     * In HL TCAM currently we do not allow conditions Therefore all conditions must be NONE
     */
    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        if (access_info->l2p_fields_info[iter].nof_conditions != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s (HL TCAM) - conditions are not supported \n", table->table_name);
        }
    }
    /*
     * Assumption is that memory is the same for Key/KeyMask/Result
     */
    memory = access_info->l2p_fields_info[0].memory[0];
    SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get(unit, access_info->l2p_fields_info[0].memory[0],
                                                  &(table->hl_mapping_multi_res[0].is_default_non_standard)));
    for (iter = 1; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        if (memory != access_info->l2p_fields_info[iter].memory[0])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "HL TCAM does not support different memories for Key,KeyMask,Result. Table: %s\n",
                         table->table_name);
        }
    }
    /*
     * Verify key and Result sizes against the memory offsets if HW field exists
     * If user used data offset - cannot check in init
     */
    meminfo = &SOC_MEM_INFO(unit, memory);
    for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
    {
        int len_from_memory = 0;
        dbal_hl_tcam_access_type_e access_type_id = access_info->l2p_fields_info[iter].hl_tcam_access_type;
        if (access_info->l2p_fields_info[iter].hw_field != INVALIDf)
        {
            /*
             * Get field len by HW field
             */
            SOC_FIND_FIELD(access_info->l2p_fields_info[iter].hw_field, meminfo->fields, meminfo->nFields, fieldinfo);
            len_from_memory = fieldinfo->len;
            /*
             * Get Current type size
             */
            switch (access_type_id)
            {
                case (DBAL_HL_TCAM_ACCESS_TYPE_KEY):
                    data_size = table->key_size;
                    break;
                case (DBAL_HL_TCAM_ACCESS_TYPE_KEY_MASK):
                    data_size = table->key_size;
                    break;
                case (DBAL_HL_TCAM_ACCESS_TYPE_RESULT):
                    /** reducing by 1 to remove the valid bit size */
                    data_size = table->max_payload_size - 1;
                    break;
                default:
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "hl_tcam_access_type was not set to a valid value in table %s, l2p_fields_info[%d] \n",
                                 table->table_name, iter);
            }

            if (data_size > len_from_memory)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "Illegal size in table %s , hl_tcam_access_type %s, length from memory is %d, but size from table is %u \n",
                             table->table_name, dbal_hl_tcam_access_type_to_string(unit, access_type_id),
                             len_from_memory, data_size);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_init_table_hl_validation_direct_memory(
    int unit,
    dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_field,
    multi_res_info_t * result_type_set,
    int field_in_table,
    int res_type)
{
    int is_default_non_standard = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (!SOC_MEM_IS_VALID(unit, curr_l2p_field->memory[0]))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "The field %s, table %s mapped to invalid memory, result type %s\n",
                     dbal_field_to_string(unit, result_type_set->results_info[field_in_table].field_id),
                     table->table_name, result_type_set->result_type_name);
    }

    if (curr_l2p_field->alias_memory != INVALIDm)
    {
        SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get(unit, curr_l2p_field->alias_memory, &is_default_non_standard));
        curr_l2p_field->is_default_non_standard = is_default_non_standard;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get(unit, curr_l2p_field->memory[0], &is_default_non_standard));
        curr_l2p_field->is_default_non_standard = is_default_non_standard;
    }

    if (is_default_non_standard)
    {
        table->hl_mapping_multi_res[res_type].is_default_non_standard = 1;
    }
    if ((curr_l2p_field->array_offset_info.formula != NULL)
        || (curr_l2p_field->entry_offset_info.formula != NULL)
        || (curr_l2p_field->data_offset_info.formula != NULL)
        || (curr_l2p_field->block_index_info.formula != NULL)
        || (curr_l2p_field->alias_data_offset_info.formula != NULL))
    {
        SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_init_table_hl_validation_direct_register(
    int unit,
    dbal_logical_table_t * table,
    dbal_hl_l2p_field_info_t * curr_l2p_field,
    multi_res_info_t * result_type_set,
    int field_in_table,
    int res_type)
{
    int is_default_non_standard = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED);

    if (!curr_l2p_field->reg || !SOC_REG_IS_VALID(unit, curr_l2p_field->reg[0]))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "The field %s, table %s mapped to invalid register, result type %s\n",
                     dbal_field_to_string(unit, result_type_set->results_info[field_in_table].field_id),
                     table->table_name, result_type_set->result_type_name);
    }

    SHR_IF_ERR_EXIT(dbal_hl_reg_default_mode_get(unit, curr_l2p_field->reg[0], &(is_default_non_standard)));
    curr_l2p_field->is_default_non_standard = is_default_non_standard;
    if (is_default_non_standard)
    {
        table->hl_mapping_multi_res[res_type].is_default_non_standard = 1;
        if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_IS_ITERATOR_OPTIMIZED))
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Iterator optimized is not supported for non-standard default, table[%s]",
                         table->table_name);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function add some validations and general properties
 * calculations for Hard Logic tables
 */
static shr_error_e
dbal_db_init_table_hl_validation_direct(
    int unit,
    dbal_logical_table_t * table)
{
    int res_type, jj;
    dbal_hard_logic_access_types_e hl_access_type;
    int bits_offset_in_sw_payload = 0;
    multi_res_info_t *result_type_set;
    dbal_hl_l2p_field_info_t *curr_l2p_field;
    int bits_offset_in_buffer = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITSET(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED);
    for (res_type = 0; res_type < table->nof_result_types; res_type++)
    {
        int total_of_of_mapped_fields = 0;
        uint8 is_packed_fields = FALSE;
        result_type_set = &(table->multi_res_info[res_type]);
        bits_offset_in_sw_payload = 0;
        table->hl_mapping_multi_res[res_type].is_default_non_standard = 0;

        for (hl_access_type = 0; hl_access_type < DBAL_NOF_HL_ACCESS_TYPES; hl_access_type++)
        {
            /** check if is packed fields. Used to skip section of validation.  */
            if (!is_packed_fields)
            {
                is_packed_fields = (table->hl_mapping_multi_res[res_type].l2p_hl_info[hl_access_type].is_packed_fields);
            }
            if (is_packed_fields)
            {   /* consecutive range is not supported yet for packed mode */
                SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED);
            }

            /** 
             *  we support only full packed fields. Full packed fields mean that result fields cover the whole
             *  register or memory entry. To validate it, we'll compare the size of all result fields in interface
             *  with the size of the register or memory entry.
             *
             *  we validate only when some fields are defined for this access type
             */
            if (is_packed_fields
                && table->hl_mapping_multi_res[res_type].l2p_hl_info[hl_access_type].num_of_access_fields > 0)
            {
                SHR_IF_ERR_EXIT(dbal_db_init_table_hl_access_packed_fields_validation
                                (unit, table, hl_access_type, result_type_set,
                                 &(table->hl_mapping_multi_res[res_type].
                                   l2p_hl_info[hl_access_type].l2p_fields_info[0])));
            }

            total_of_of_mapped_fields +=
                table->hl_mapping_multi_res[res_type].l2p_hl_info[hl_access_type].num_of_access_fields;
            for (jj = 0; jj < table->hl_mapping_multi_res[res_type].l2p_hl_info[hl_access_type].num_of_access_fields;
                 jj++)
            {
                int field_in_table = 0;
                curr_l2p_field = &table->hl_mapping_multi_res[res_type].l2p_hl_info[hl_access_type].l2p_fields_info[jj];

                /** packed fields skip the validation between results and mapping. */
                if (!is_packed_fields)
                {
                    /** look for the corresponding field in the table and set position */
                    for (field_in_table = 0; field_in_table < result_type_set->nof_result_fields; field_in_table++)
                    {
                        if ((result_type_set->results_info[field_in_table].field_id == curr_l2p_field->field_id))
                        {
                            break;
                        }
                    }

                    if (field_in_table == result_type_set->nof_result_fields)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "L2P field %s not found in table %s as result field, "
                                     "result type %s\n",
                                     dbal_field_to_string(unit, curr_l2p_field->field_id), table->table_name,
                                     result_type_set->result_type_name);
                    }
                    curr_l2p_field->field_pos_in_interface = field_in_table;
                    curr_l2p_field->nof_bits_in_interface = (curr_l2p_field->access_nof_bits != 0) ?
                        curr_l2p_field->access_nof_bits : result_type_set->results_info[field_in_table].field_nof_bits;
                    curr_l2p_field->offset_in_interface = curr_l2p_field->access_offset +
                        result_type_set->results_info[field_in_table].bits_offset_in_buffer;
                }
                else
                {
                    /** we consider only full packed fields. Therefore, we'll consider the whole interface size  */
                    curr_l2p_field->offset_in_interface = 0;
                    curr_l2p_field->nof_bits_in_interface = result_type_set->entry_payload_size;
                }

                /** check that the field is mapped to a valid memory/register */
                switch (hl_access_type)
                {
                    case DBAL_HL_ACCESS_MEMORY:
                        SHR_IF_ERR_EXIT(dbal_db_init_table_hl_validation_direct_memory(unit,
                                                                                       table,
                                                                                       curr_l2p_field,
                                                                                       result_type_set,
                                                                                       field_in_table, res_type));
                        break;
                    case DBAL_HL_ACCESS_REGISTER:
                        SHR_IF_ERR_EXIT(dbal_db_init_table_hl_validation_direct_register(unit,
                                                                                         table,
                                                                                         curr_l2p_field,
                                                                                         result_type_set,
                                                                                         field_in_table, res_type));
                        break;
                    case DBAL_HL_ACCESS_SW:
                        SHR_BITCLR(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED);
                        curr_l2p_field->field_pos_in_interface = field_in_table;
                        /*
                         * We can assign all field bits to software or just part of it, for fields that are fully assigned
                         * access_entry->access_nof_bits is 0 here and we update it to  the  full size of the field from the
                         * interface definition for it. otherwise access_entry->access_nof_bits
                         * holds the exact number of bits mapped to sw so we use it to define the sw bits length
                         */
                        if (curr_l2p_field->access_nof_bits == 0)
                        {
                            curr_l2p_field->access_nof_bits =
                                result_type_set->results_info[field_in_table].field_nof_bits;
                        }
                        curr_l2p_field->nof_bits_in_interface = curr_l2p_field->access_nof_bits;
                        curr_l2p_field->offset_in_interface =
                            curr_l2p_field->access_offset +
                            result_type_set->results_info[field_in_table].bits_offset_in_buffer;
                        /*
                         * we reuse access_offset variable to specifiy the exact bit offset in sw buffer
                         */
                        curr_l2p_field->access_offset = bits_offset_in_sw_payload;
                        bits_offset_in_sw_payload += curr_l2p_field->access_nof_bits;
                        break;
                    default:
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Invalid access type, %d, for hard-logic table: %s\n",
                                     hl_access_type, table->table_name);
                        break;
                }

                if (curr_l2p_field->alias_data_offset_info.formula != NULL)
                {
                    if (curr_l2p_field->memory[0] != INVALIDm)
                    {
                        int mem_size = SOC_MEM_BYTES(unit, curr_l2p_field->memory[0]);
                        mem_size = BYTES2BITS(mem_size);
                        curr_l2p_field->alias_data_offset_info.internal_inparam = mem_size;
                    }
                    else
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "table %s not supported aliasing with offset to registers\n", table->table_name);
                    }

                }
            }
        }
        /*
         * reverse the fields offsets for packed_fields of multiple results table
         */
        if (is_packed_fields && SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_HAS_RESULT_TYPE))
        {
            int kk;
            bits_offset_in_buffer = 0;
            for (kk = result_type_set->nof_result_fields - 1; kk >= 0; kk--)
            {
                result_type_set->results_info[kk].bits_offset_in_buffer = bits_offset_in_buffer;
                bits_offset_in_buffer += result_type_set->results_info[kk].field_nof_bits;
            }

        }
        if (!is_packed_fields && (total_of_of_mapped_fields < result_type_set->nof_result_fields))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s not all fields are mapped %d of %d \n", table->table_name,
                         total_of_of_mapped_fields, result_type_set->nof_result_fields);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_tables_logical_validation(
    int unit,
    dbal_logical_table_t * tables_info)
{
    int table_id;
    dbal_logical_table_t *table = NULL;
    dbal_table_status_e table_status;
    shr_error_e rv;

    SHR_FUNC_INIT_VARS(unit);

    for (table_id = 0; table_id < DBAL_NOF_STATIC_TABLES; table_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        table = &tables_info[table_id];
        /**  check that table is not set as invalid */

        if (sw_state_is_warm_boot(unit))
        {
            if ((table_status == DBAL_TABLE_NOT_INITIALIZED) || (table_status == DBAL_TABLE_HW_ERROR))
            {
                continue;
            }
        }
        else
        {
            if ((table_status == DBAL_NOF_TABLE_STATUS) || (table_status == DBAL_TABLE_HW_ERROR)
                || (table->maturity_level == DBAL_MATURITY_LOW))
            {
                if (table_status != DBAL_TABLE_HW_ERROR)
                {
                    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.
                                    table_status.set(unit, table_id, DBAL_TABLE_NOT_INITIALIZED));
                }
                continue;
            }
        }

        SHR_IF_ERR_EXIT(dbal_db_init_table_mutual_interface_validation(unit, table_id, table));

        /** skip validation for incompatible image tables. We'll skip actions on this tables */
        if (table_status == DBAL_TABLE_INCOMPATIBLE_IMAGE)
        {
            continue;
        }

        /** per access method validation */
        switch (table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_IF_ERR_EXIT(dbal_db_init_table_mdb_validation(unit, table_id, table));
                break;
            case DBAL_ACCESS_METHOD_TCAM_CS:
                break;
            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                /** for HL if the table has error continue */
                if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
                {
                    rv = dbal_db_init_table_hl_validation_tcam(unit, table);
                }
                else
                {
                    rv = dbal_db_init_table_hl_validation_direct(unit, table);
                }

                if (SHR_FAILURE(rv))
                {
                    /**This specific error refers to a non-standard default with bitmap table --> not supported*/
                    if (rv == _SHR_E_UNAVAIL)
                    {
                        SHR_SET_CURRENT_ERR(rv);
                        SHR_EXIT();
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
                        continue;
                    }
                }
                break;
            case DBAL_ACCESS_METHOD_SW_STATE:
                SHR_IF_ERR_EXIT(dbal_db_init_table_sw_only_validation(unit, table));
                break;
            case DBAL_ACCESS_METHOD_KBP:
                SHR_IF_ERR_EXIT(dbal_db_init_table_mdb_validation(unit, table_id, table));
                break;
            case DBAL_ACCESS_METHOD_PEMLA:
            default:
                break;
        }

        SHR_IF_ERR_EXIT(dbal_db_init_table_reverse_instance_order(unit, table_id, table));
        if (!sw_state_is_warm_boot(unit))
        {
            if (table_status != DBAL_TABLE_INCOMPATIBLE_IMAGE)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_INITIALIZED));
            }
            /** save table indications in sw_state */
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.set(unit, table_id, table->indications_bm[0]));
        }
        /** restore table indications in sw_state */
        else
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.indications_bm.get(unit, table_id, table->indications_bm));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_sw_state_set_indirect_table_size(
    int unit,
    table_db_struct_t * cur_table_param,
    char *indirect_table_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (indirect_table_size)
    {
        DBAL_INIT_COPY_N_STR(cur_table_param->table_size_str, indirect_table_size, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);

        cur_table_param->physical_db_id = DBAL_NOF_PHYSICAL_TABLES;

        if (sal_strstr(cur_table_param->table_size_str, DB_INIT_PHYDB_INDICATION) != NULL)
        {
            SHR_EXIT();
        }
    }
    else
    {
        DBAL_INIT_COPY_N_STR(cur_table_param->table_size_str, "INVALID", DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    }

    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_table_param->indirect_table_size_dnx_data, indirect_table_size, FALSE, 100, "", ""));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_mdb_info(
    int unit,
    table_db_struct_t * cur_table_param,
    char *phy_db,
    char *app_db_id_size,
    char *app_db_id,
    dbal_stage_e stage)
{
    SHR_FUNC_INIT_VARS(unit);
    /** phy db   */
    sal_strncpy_s(cur_table_param->mdb_access->phy_db, phy_db, sizeof(cur_table_param->mdb_access->phy_db));
    /** app db id size */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_table_param->mdb_access->app_db_id_size_dnx_data, app_db_id_size, FALSE, 0, "", ""));
    /** app db id */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &cur_table_param->mdb_access->app_db_id_dnx_data, app_db_id, FALSE, 0, "", ""));
    /** stage */
    cur_table_param->pipe_stage = stage;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_hw_entity_group_general_info_set(
    int unit,
    hl_group_info_t * group_info,
    uint32 element_counter,
    uint8 is_reg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** count number of element per group */
    group_info->group_hw_entity_num = element_counter;
    /** indicate if group of register or memories */
    group_info->is_reg = is_reg;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

void
dbal_db_init_hw_entity_group_reg_mem_name_set(
    int unit,
    hl_group_info_t * group_info,
    int element_index,
    int reg_or_mem_id)
{
    group_info->hw_entity_id[element_index] = reg_or_mem_id;
}

void
dbal_db_init_hw_entity_porting_reg_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    int reg_id,
    int map_to_reg_id)
{
    hl_porting_info->register_ids[hl_porting_info->nof_registers] = reg_id;
    hl_porting_info->register_mapped_ids[hl_porting_info->nof_registers] = (soc_reg_t) map_to_reg_id;
    hl_porting_info->nof_registers++;
}

void
dbal_db_init_hw_entity_porting_mem_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    int mem_id,
    int map_to_mem_id)
{
    hl_porting_info->memory_ids[hl_porting_info->nof_memories] = mem_id;
    hl_porting_info->memory_mapped_ids[hl_porting_info->nof_memories] = (soc_mem_t) map_to_mem_id;
    hl_porting_info->nof_memories++;
}

char *
dbal_db_init_image_name_get(
    int unit)
{
    char *image_name;

    dnx_pp_prgm_device_image_name_get(unit, &image_name);

    return image_name;
}
