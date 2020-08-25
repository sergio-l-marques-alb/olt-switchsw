/**
 * \file dbal_db_init.c
 * $Id$
 *
 * Main functions for init the dbal fields and logical tables DB
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DBALDNX

/*************
 * INCLUDES  *
 *************/

#include <sal/core/libc.h>
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
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_field_types.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_max_allocations_hw_entity_groups.h>
#include <src/soc/dnx/dbal/dbal_init/auto_generated/dbal_init_hw_entity_groups.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pp.h>
#include <bcm_int/dnx/init/init_time_analyzer.h>

#include "dbal_internal.h"

/*************
 *  DEFINES  *
 *************/
#define DBAL_NOF_XML_FILES              (1000)
#define DBAL_DB_INIT_DEFAULT_VAL_TEMP   (0x12345678)
#define DBAL_SW_STATE_ACCESS dbal_db

/*************
 *  GLOBALS  *
 *************/

/**
 * \brief
 * Delimiters for labels list
 */
static char *label_delimiters = "\n\t ";

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

uint8 check_hw_ent_in_init = 0;

extern uint32 G_dbal_field_full_mask[DBAL_PHYSICAL_RES_SIZE_IN_WORDS];
extern int sal_atoi(
    const char *nptr);

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
    int nof_tables;
    uint8 res_type_index;
    int nof_multi_res_types;

    SHR_FUNC_INIT_VARS(unit);

    nof_tables = dnx_data_dbal.table.nof_dynamic_tables_get(unit);

    SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.alloc(unit));
    SHR_IF_ERR_EXIT(dbal_db.tables_properties.alloc(unit));


    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        /** allocate table labels info memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.labels.alloc(unit,table_index));

        /** allocate table key info memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.key_info_data.alloc(unit, table_index));

        /** allocate table multi res type  memory including swstate */
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.multi_res_info_data.alloc(unit,table_index));

        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.res_info_data.alloc(unit,table_index));

#if defined(INCLUDE_KBP)
        SHR_IF_ERR_EXIT(DBAL_TBL_IN_SW_STATE.access_info.alloc(unit, table_index, sizeof(kbp_db_handles_t)));
#endif
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

    SHR_FUNC_INIT_VARS(unit);

    nof_tables = dnx_data_dbal.table.nof_dynamic_tables_get(unit);

    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        /** get table pointer */
        SHR_IF_ERR_EXIT(dbal_tables_table_get_internal(unit,table_index + DBAL_NOF_TABLES, &table));
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
        key_info_data_size_bytes = dnx_data_dbal.table.nof_dynamic_tables_key_fields_get(unit) * sizeof(dbal_table_field_info_t);
        if (key_info_data_size_bytes)
        {
            SHR_ALLOC_SET_ZERO(table->keys_info, key_info_data_size_bytes,
                               "key fields info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);
        }

        /** allocate table multi res type  memory */
        nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_multi_result_types_get(unit);
        multi_res_info_data_size_bytes = nof_multi_res_types * sizeof(multi_res_info_t);
        multi_res_info_results_info_data_size_bytes = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit) * sizeof(dbal_table_field_info_t);
        if (multi_res_info_data_size_bytes)
        {
            SHR_ALLOC_SET_ZERO(table->multi_res_info, multi_res_info_data_size_bytes,
                               "multiple results info allocation", "%s%s%s\r\n", table->table_name, EMPTY, EMPTY);

            /** allocate table result fields memory */
            for (multi_res_type_index = 0; multi_res_type_index < nof_multi_res_types; multi_res_type_index++)
            {
                table->multi_res_info[multi_res_type_index].results_info = NULL;
                SHR_ALLOC_SET_ZERO(table->multi_res_info[multi_res_type_index].results_info,
                                   multi_res_info_results_info_data_size_bytes, "results fields info allocation", "%s%s%s\r\n",
                                   table->table_name, EMPTY, EMPTY);
            }
        }

#if defined(INCLUDE_KBP)
        SHR_ALLOC_SET_ZERO(table->kbp_handles, sizeof(kbp_db_handles_t),
                           "multiple result direct mapping allocation", "%s%s%s\r\n",
                           table->table_name, EMPTY, EMPTY);
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
            uint8 is_key;
            entry_condition->field_id = condition_params->field_id;
            /** validate that the field exists in table's key info */
            SHR_IF_ERR_EXIT(dbal_tables_field_is_key_get(unit, table_id, entry_condition->field_id, &is_key));
            if (!is_key)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "mapping condition uses an invalid field:%s for table:%s\n",
                             dbal_field_to_string(unit, entry_condition->field_id),
                             dbal_logical_table_to_string(unit, table_id));
            }
        }
        if (sal_strcmp(condition_params->enum_val, EMPTY) != 0)
        {
            int ii;
            char enum_name[DBAL_MAX_LONG_STRING_LENGTH];
            dbal_field_types_basic_info_t *field_info = NULL;
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get(unit, entry_condition->field_id, &field_info));

            sal_strncpy_s(enum_name, field_info->name, sizeof(enum_name));
            sal_strncat(enum_name, "_", sizeof("_"));
            sal_strncat(enum_name, condition_params->enum_val, sizeof(condition_params->enum_val));

            for (ii = 0; ii < field_info->nof_enum_values; ii++)
            {
                if (sal_strcmp(field_info->enum_val_info[ii].name, enum_name) == 0)
                {
                    entry_condition->value[0] = ii;
                    break;
                }
            }
            if (ii == field_info->nof_enum_values)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Mapping condition set with enum, but enum doesn't exist in field db. field:%s, enum:%s\n",
                             dbal_field_to_string(unit, entry_condition->field_id), condition_params->enum_val);
            }
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
 */
static shr_error_e
dbal_db_parse_offset_formula(
    int unit,
    dbal_tables_e table_id,
    uint8 is_res_type_mapping,
    char *str_formula,
    int  int_formula, 
    dbal_offset_formula_t * formula,
    uint8 allow_dynamic_fields)
{
    int str_len;
    char curr_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    int nof_plus_minus_operands = 0;
    int plus_minus_operands[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1] = { 0 };
    int nof_mult_div_mod_operands = 0;
    int mult_div_mod_operands[DBAL_OFFSET_FORMULA_MAX_NUM_OF_VARS - 1] = { 0 };
    int index;
    int nof_operands;
    uint8 plus_or_mult;

    SHR_FUNC_INIT_VARS(unit);

    /* the formula was parsed by Autocoder (simple int), we can skip the costly parsing */
    if (int_formula != DBAL_DB_INVALID) {
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
    str_len = sal_strlen(str_formula);

    /** search for minus and plus separators */

    index = 0;

    while (index < str_len)
    {
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

    if (nof_operands == 0)
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
        else if(sal_strstr(curr_str, DB_INIT_PHYDB_INDICATION) != NULL)
        {
            dbal_physical_tables_e physical_db_id;
            int physical_db_size_val = 0;

            SHR_IF_ERR_EXIT(dbal_db_parse_phyiscal_db_id(unit, curr_str, &physical_db_id));
            SHR_IF_ERR_EXIT(mdb_db_infos.capacity.get(unit, physical_db_id, &physical_db_size_val));

            formula->val = (uint32)physical_db_size_val;
        }
        else
        {
            res = dbal_field_string_to_id_no_error(unit, curr_str, &formula->field_id);
            if (res == _SHR_E_NOT_FOUND)
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
                    if (utilex_str_stoul(curr_str, (uint32 *) &formula->val) != _SHR_E_NONE)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Formula contain a string (%s) that not related to table\n", curr_str);
                    }
                }
            }
            else
            {
                if (!allow_dynamic_fields)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "Field names in formula is a dynamic information that cannot be used in that place.\n");
                }
                if (is_res_type_mapping)
                {
                    int rv;
                    dbal_table_field_info_t field_info;
                    rv = dbal_tables_field_info_get_no_err(unit, table_id, formula->field_id, TRUE, 0, 0, &field_info);
                    if (rv == _SHR_E_NOT_FOUND)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "RESULT_TYPE mapping with field %s is illegal (it is result field or not in table)\n",
                                     curr_str);
                    }
                }
            }
        }
    }
    else
    {
        uint8 found_operand = FALSE;
        char val_str[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
        int str_index = 0;
        int sub_str_index = 0;
        int operand_index = 0;
        int num_of_elements = nof_operands + 1;
        formula->nof_vars = num_of_elements;

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
                                (unit, table_id, is_res_type_mapping, val_str, DBAL_DB_INVALID,
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
                        (unit, table_id, is_res_type_mapping, val_str, DBAL_DB_INVALID, 
                         &formula->val_as_formula[operand_index],
                         allow_dynamic_fields));
    }

exit:
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

    if (sal_strcmp(offset_params->formula, EMPTY) != 0)
    {
        SHR_ALLOC_SET_ZERO(entry_offset->formula, 1 * sizeof(dbal_offset_formula_t),
                           "formula", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

        DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_PARSING_OFFSETS_PARSE);
        SHR_IF_ERR_EXIT(dbal_db_parse_offset_formula
                        (unit, table_id, is_res_type_mapping, offset_params->formula, offset_params->formula_int , entry_offset->formula, TRUE));
        DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_PARSING_OFFSETS_PARSE);
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
                     DBAL_DB_INVALID,
                     int_or_dnx_data_as_formula->formula, FALSE));

    SHR_IF_ERR_EXIT(dbal_calculate_offset_from_formula
                    (unit, NULL, DBAL_NOF_TABLES, DBAL_FIELD_EMPTY, int_or_dnx_data_as_formula->formula, &formula_val));

    int_or_dnx_data_as_formula->int_val = (int) formula_val;

    dbal_tables_formulas_free(int_or_dnx_data_as_formula->formula, 0);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function adds a field read from XML to the dbal field DB.
 * Main functionality is translating the string from XML to
 * corresponding values
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
    uint8 is_zero_mapping_exists = FALSE;
    int max_value_for_enum_mapping = 0;
    int max_value_for_define = 0;
    int has_reference_field = FALSE;

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

    /** is allocator */

    if (sal_strcmp(fld_type_prm->resource_name, EMPTY) != 0)
    {
        fld_type_ent->is_allocator = TRUE;
    }
    else
    {
        fld_type_ent->is_allocator = FALSE;
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
        else if (fld_type_ent->max_size == 32)
        {
            fld_type_ent->max_value = UTILEX_U32_MAX;
        }
        else
        {
            fld_type_ent->max_value = utilex_power_of_2(fld_type_ent->max_size) - 1;
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
        fld_type_ent->const_value_valid = TRUE;
    }
    else
    {
        fld_type_ent->const_value_valid = FALSE;
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
            /** currently we are not supporting this type of encoding because it makes the field biffer than the original size,
             *  if we want to support need to limit the max size of the field according to the size of the logical value second
             *  we need to add some bits to the size of the field. */
        }
    }
    fld_type_ent->encode_info.input_param = fld_type_prm->encode_param1_dnx_data.int_val;

    /** In case of Enum type - set also enum encoding */
    if (fld_type_ent->print_type == DBAL_FIELD_PRINT_TYPE_ENUM)
    {
        fld_type_ent->encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_ENUM;
        /** In case of Enum type - the max value the max enum value */
        fld_type_ent->min_value = 0;
        fld_type_ent->max_value = fld_type_prm->nof_enum_vals - 1;
    }

    if (fld_type_ent->print_type == DBAL_FIELD_PRINT_TYPE_STRUCT)
    {
        fld_type_ent->encode_info.encode_mode = DBAL_VALUE_FIELD_ENCODE_STRUCT;
    }

    /** encoding validation, at the moment encoding only
     *  supported for fields less than or equal 32bit */
    if ((fld_type_ent->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_NONE) &&
        (fld_type_ent->encode_info.encode_mode != DBAL_VALUE_FIELD_ENCODE_STRUCT))
    {
        if (fld_type_ent->max_size > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal field %s encoding, not supported for field bigger than 32 bit\n",
                         fld_type_ent->name);
        }

        if (fld_type_ent->encode_info.encode_mode == DBAL_VALUE_FIELD_ENCODE_VALID_IND)
        {
            fld_type_ent->max_value = (1 << (fld_type_ent->max_size - 1)) - 1;  /* 2^(fld_type_ent->max_size-1)-1 */
        }
    }

    /** child fields */
    fld_type_ent->nof_child_fields = fld_type_prm->nof_childs;
    if (fld_type_ent->nof_child_fields > 0)
    {
        if (fld_type_ent->nof_child_fields > DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more child fields than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", fld_type_prm->name);
        }

        SHR_ALLOC_SET_ZERO(fld_type_ent->sub_field_info, fld_type_ent->nof_child_fields * sizeof(dbal_sub_field_info_t),
                           "sub field info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);

        if (sal_strcmp(fld_type_prm->refernce_field_name, EMPTY) != 0)
        {
            SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, fld_type_prm->refernce_field_name,
                                                              &(fld_type_ent->refernce_field_id)));
            has_reference_field = TRUE;
        }
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

    /** struct fields */
    fld_type_ent->nof_struct_fields = fld_type_prm->nof_structs;
    if (fld_type_ent->nof_struct_fields > 0)
    {
        if (fld_type_ent->nof_struct_fields > DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more struct fields than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", fld_type_prm->name);
        }

        SHR_ALLOC_SET_ZERO(fld_type_ent->struct_field_info,
                           fld_type_ent->nof_struct_fields * sizeof(dbal_sub_struct_field_info_t),
                           "struct field info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);
    }

    for (ii = 0; ii < fld_type_prm->nof_structs; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, fld_type_prm->structs[ii].name,
                                                &fld_type_ent->struct_field_info[ii].struct_field_id));

        fld_type_ent->struct_field_info[ii].length = fld_type_prm->structs[ii].length_dnx_data.int_val;
        fld_type_ent->struct_field_info[ii].offset = fld_type_prm->structs[ii].start_offest_dnx_data.int_val;
    }

    /** enums */
    fld_type_ent->nof_enum_values = fld_type_prm->nof_enum_vals;
    if (fld_type_ent->nof_enum_values > 0)
    {
        if (fld_type_ent->nof_enum_values > DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more enums values than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", fld_type_prm->name);
        }
        SHR_ALLOC_SET_ZERO(fld_type_ent->enum_val_info,
                           fld_type_ent->nof_enum_values * sizeof(dbal_enum_decoding_info_t),
                           "enum info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);
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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field Type %s has enum values so print type has to be ENUM\n",
                         fld_type_prm->name);
        }

        for (ii = 0; ii < fld_type_prm->nof_enum_vals; ii++)
        {
            sal_strncpy_s(fld_type_ent->enum_val_info[ii].name, fld_type_prm->name,
                          sizeof(fld_type_ent->enum_val_info[ii].name));
            sal_strncat(fld_type_ent->enum_val_info[ii].name, "_", sizeof("_"));
            sal_strncat(fld_type_ent->enum_val_info[ii].name, fld_type_prm->enums[ii].name_from_interface,
                        sizeof(fld_type_prm->enums[ii].name_from_interface));
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
            fld_type_ent->is_default_value_valid = fld_type_prm->default_val_valid;

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
    }

    /** set default value when not enum (see above for enum) */
    if (fld_type_prm->default_val_valid == TRUE && (fld_type_ent->nof_enum_values == 0))
    {
        /** Default Value */
        fld_type_ent->default_value = fld_type_prm->default_val_dnx_data.int_val;
        fld_type_ent->is_default_value_valid = fld_type_prm->default_val_valid;
    }

    /** Defines */
    fld_type_ent->nof_defines_values = fld_type_prm->nof_defined_vals;
    if (fld_type_ent->nof_defines_values > 0)
    {
        if (fld_type_ent->nof_defines_values > DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field %s has more define values than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", fld_type_prm->name);
        }

        SHR_ALLOC_SET_ZERO(fld_type_ent->defines_info,
                           fld_type_ent->nof_defines_values * sizeof(dbal_defiens_info_t),
                           "defines info allocation", "%s%s%s\r\n", fld_type_ent->name, EMPTY, EMPTY);
        if (fld_type_prm->max_size_dnx_data.int_val > 32)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Defined values fields cannot be bigger that 32 bits long, %s\n",
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

        for (ii = 0; ii < fld_type_prm->nof_defined_vals; ii++)
        {
            sal_strncpy_s(fld_type_ent->defines_info[ii].name, fld_type_prm->name,
                          sizeof(fld_type_ent->defines_info[ii].name));
            sal_strncat(fld_type_ent->defines_info[ii].name, "_", sizeof(fld_type_ent->defines_info[ii].name) - 1);
            sal_strncat(fld_type_ent->defines_info[ii].name, fld_type_prm->define[ii].define_name,
                        sizeof(fld_type_prm->define[ii].define_name));
            fld_type_ent->defines_info[ii].value = fld_type_prm->define[ii].define_value.int_val;

            if (fld_type_ent->defines_info[ii].value > max_value_for_define)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Illegal value for Define. field size is %d, defined value is %d. field %s\n",
                             fld_type_prm->max_size_dnx_data.int_val, fld_type_ent->defines_info[ii].value,
                             fld_type_ent->name);
            }
        }
    }

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
            mem =
                table->hl_mapping_multi_res[0].
                l2p_hl_info[DBAL_HL_ACCESS_MEMORY].l2p_fields_info[DBAL_HL_TCAM_ACCESS_TYPE_KEY].memory[0];
            table->max_capacity = SOC_MEM_INFO(unit, mem).index_max + 1;
            break;

        case DBAL_ACCESS_METHOD_MDB:
            
            table->max_capacity = 0;
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
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
            break;

        case DBAL_ACCESS_METHOD_HARD_LOGIC:
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
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Ilegal access method\n");
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

        if (fields_read_data[ii].valid_dnx_data.int_val == 0)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, fields_read_data[ii].name, &field_id));
        fields_info[field_index].field_id = field_id;

        field_type = fields_read_data[ii].field_type;

        SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &curr_field_type));
        if (curr_field_type != field_type)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s has multiple types definition: type1=%s type2=%s\n",
                         fields_read_data[ii].name, dbal_field_types_def_to_string(unit, curr_field_type),
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

        SHR_IF_ERR_EXIT(dbal_fields_illegal_values_get(unit, field_id, &fields_info[field_index].nof_illegal_values,
                                                       fields_info[field_index].illegal_values));

        if (is_key_fields)
        {
            /**  in case that the field is a key field we need to check if the field is allocator */
            SHR_IF_ERR_EXIT(dbal_fields_is_allocator_get(unit, field_id, &(fields_info[field_index].is_allocator)));

            if ((table_entry->allocator_field_id == DBAL_FIELD_EMPTY) && (fields_info[field_index].is_allocator))
            {
                table_entry->allocator_field_id = fields_info[field_index].field_id;
                if (table_entry->access_method == DBAL_ACCESS_METHOD_MDB && num_of_fields > 1)
                {
                    /** currently not supported allocator field in MDB for tables with more than one Key field */
                    table_entry->allocator_field_id = DBAL_FIELD_EMPTY;
                }
            }
        }

        /** Read field offset */
        fields_info[field_index].offset_in_logical_field = fields_read_data[ii].offset_dnx_data.int_val;

        /** permission  */
        fields_info[field_index].permission = fields_read_data[ii].permission;

        if (is_key_fields && (fields_info[field_index].permission != DBAL_PERMISSION_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Key field cannot be set to read-only. field %s table %s\n",
                         dbal_field_to_string(unit, fields_info[field_index].field_id), table_params->name);
        }

        if (fields_read_data[ii].size_dnx_data.int_val != 0)
        {
            fields_info[field_index].field_nof_bits = fields_read_data[ii].size_dnx_data.int_val;
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
                         dbal_field_to_string(unit, fields_info[field_index].field_id), table_params->name,
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
                if (field_max_value > 0)
                {
                    if (field_effective_size == 32)
                    {
                        fields_info[field_index].max_value = field_max_value;
                    }
                    else
                    {
                        fields_info[field_index].max_value =
                            UTILEX_MIN(field_max_value, utilex_power_of_2(field_effective_size) - 1);
                    }
                }
                else
                {
                    if (field_effective_size == 32)
                    {
                        fields_info[field_index].max_value = UTILEX_U32_MAX;
                    }
                    else
                    {
                        fields_info[field_index].max_value = utilex_power_of_2(field_effective_size) - 1;
                    }
                }
            }
            else
            {
                /** for enum we are using the field type max value */
                fields_info[field_index].max_value = field_max_value;
            }

            if ((fields_info[field_index].field_id == DBAL_FIELD_CORE_ID) && (field_max_value == 0))
            {
                fields_info[field_index].max_value = 0;
            }

            /** update for specific table */
            if (fields_read_data[ii].max_value_dnx_data.int_val > 0)
            {
                fields_info[field_index].max_value =
                    UTILEX_MIN(fields_info[field_index].max_value, fields_read_data[ii].max_value_dnx_data.int_val);
            }

            /** Set Min Value of field  */
            fields_info[field_index].min_value = field_min_value;

            if (fields_read_data[ii].min_value_dnx_data.int_val > 0)
            {
                fields_info[field_index].min_value =
                    UTILEX_MAX(fields_info[field_index].min_value, fields_read_data[ii].min_value_dnx_data.int_val);
            }

            /** set const value for field, read from field property or from field type */
            if (fields_read_data[ii].const_value_dnx_data.int_val != DBAL_DB_INVALID)
            {
                fields_info[field_index].const_value = fields_read_data[ii].const_value_dnx_data.int_val;
                fields_info[field_index].const_value_valid = TRUE;
            }
            else if (field_const_value_valid)
            {
                fields_info[field_index].const_value = field_const_value;
                fields_info[field_index].const_value_valid = TRUE;
            }

            if (fields_info[field_index].const_value_valid)
            {
                fields_info[field_index].min_value = fields_info[field_index].const_value;
                fields_info[field_index].max_value = fields_info[field_index].const_value;
            }

            if (!is_key_fields && fields_read_data[ii].arr_prefix_width_dnx_data.int_val)
            {
                CONST dbal_field_types_basic_info_t *field_type_info;

                SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, curr_field_type, &field_type_info));

                /** arr prefix size the difference between the field type size and the field in table */
                fields_info[field_index].arr_prefix_size =
                    (field_type_info->max_size - fields_info[field_index].field_nof_bits);

                fields_info[field_index].arr_prefix =
                    (fields_read_data[ii].
                     arr_prefix_value_dnx_data.int_val >> (fields_read_data[ii].arr_prefix_width_dnx_data.int_val -
                                                           fields_info[field_index].arr_prefix_size));

                if (fields_info[field_index].field_type == DBAL_FIELD_TYPE_DEF_UINT)
                {
                    if (fields_info[field_index].field_id != DBAL_FIELD_RESERVED)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "field %s with type uint cannot use arr prefix\n",
                                     dbal_field_to_string(unit, fields_info[field_index].field_id));
                    }
                }

                
                if (fields_info[field_index].field_id == DBAL_FIELD_VLAN_EDIT_PROFILE)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }
                
                if (fields_info[field_index].field_id == DBAL_FIELD_VSI)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }
                
                if (fields_info[field_index].field_id == DBAL_FIELD_VRF)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }
                
                if (fields_info[field_index].field_id == DBAL_FIELD_DA_NOT_FOUND_DEST)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }
                
                if (fields_info[field_index].field_id == DBAL_FIELD_MC_ID)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }
                
                if (fields_info[field_index].field_id == DBAL_FIELD_FEC)
                {
                    fields_info[field_index].arr_prefix_size = 0;
                    fields_info[field_index].arr_prefix = 0;
                }

            }
        }
        else
        {
            fields_info[field_index].min_value = 0;
            fields_info[field_index].max_value = 0;
            if ((fields_read_data[ii].max_value_dnx_data.int_val > 0) ||
                (fields_read_data[ii].min_value_dnx_data.int_val > 0))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Max/Min values cannot be set with fields larger than 32bits. field %s\n",
                             dbal_field_to_string(unit, fields_info[field_index].field_id));
            }
        }
        key_length_in_bits += fields_info[field_index].field_nof_bits;

        /** multiple instances support */
        fields_info[field_index].nof_instances = fields_read_data[ii].nof_instances_dnx_data.int_val;
        if (fields_read_data[ii].nof_instances_dnx_data.int_val > 1)
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
                                 fields_read_data[ii].name, jj, dbal_field_types_def_to_string(unit, field_type),
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
                    fields_info[field_index + jj].is_instances_reverse_order = TRUE;
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
    int label_index;
    int ii, jj;
    char *label_token;
    char *saveptr = NULL;
    char label_copy[DBAL_MAX_NOF_ENTITY_LABEL_TYPES * DBAL_MAX_SHORT_STRING_LENGTH];
    int nof_multi_res_types;

    SHR_FUNC_INIT_VARS(unit);

    /** Name - Printing Name */
    sal_strncpy_s(table_entry->table_name, table_params->name, sizeof(table_entry->table_name));

    /** Maturity Level */
    table_entry->maturity_level = table_params->maturity_level;

    /** Type */
    SHR_IF_ERR_EXIT(dbal_logical_table_type_string_to_id(unit, table_params->type, &table_entry->table_type));

    /** Labels */
    label_index = 0;
    sal_strncpy_s(label_copy, table_params->labels, sizeof(label_copy));
    label_token = sal_strtok_r(table_params->labels, label_delimiters, &saveptr);
    while (label_token != NULL)
    {
        label_token = sal_strtok_r(NULL, label_delimiters, &saveptr);
        label_index++;
    }
    if (label_index > 0)
    {
        table_entry->nof_labels = label_index;
        if (table_entry->nof_labels > DBAL_MAX_NOF_ENTITY_LABEL_TYPES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more labels than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", table_params->name);
        }

        SHR_ALLOC_SET_ZERO(table_entry->table_labels, label_index * sizeof(dbal_labels_e),
                           "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        label_index = 0;
        saveptr = NULL;
        label_token = sal_strtok_r(label_copy, label_delimiters, &saveptr);
        while (label_token != NULL)
        {
            SHR_IF_ERR_EXIT(dbal_label_string_to_id(unit, label_token, &table_entry->table_labels[label_index]));
            label_token = sal_strtok_r(NULL, label_delimiters, &saveptr);
            label_index++;
        }
    }

    /** Key Fields */
    if (table_params->nof_key_fields > DBAL_TABLE_MAX_NUM_OF_KEY_FIELDS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more key fields than max allowed."
                     "update the max allowed in dbal_defines_max_allocations.h\n", table_params->name);
    }

    SHR_ALLOC_SET_ZERO(table_entry->keys_info, table_params->nof_key_fields * sizeof(dbal_table_field_info_t),
                       "key fields info allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dbal_db_init_table_add_fields(unit, table_entry, table_params, TRUE, 0));

    /** for multiple results fields */
    table_entry->nof_result_types = table_params->num_of_results_sets;
    table_entry->has_result_type = table_params->result_type_exists;
    if (table_entry->nof_result_types > DBAL_MAX_NUMBER_OF_RESULT_TYPES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more result types than max allowed."
                     "update the max allowed in dbal_defines_max_allocations.h\n", table_params->name);
    }

    /*
     *  Only for MDB tables, RESULT_TYPE can be mapped to SW
     *  state using the below flag
     */
    if (table_params->result_type_in_sw_dnx_data.int_val == 1)
    {
        if (table_entry->has_result_type && (table_entry->access_method == DBAL_ACCESS_METHOD_HARD_LOGIC))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal result type mapping for table %s."
                         "result type in HL should be mapped in Access\n", table_entry->table_name);
        }
        table_entry->result_type_mapped_to_sw = TRUE;
    }

    /** superset result type is not supported for table that has dynamic result types */
    if (table_entry->has_result_type && sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
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
            if (table_params->results_set[ii].result_fields[jj].valid_dnx_data.int_val == 0)
            {
                continue;
            }
            if (table_params->results_set[ii].result_fields[jj].nof_instances_dnx_data.int_val > 1)
            {
                nof_results_field += table_params->results_set[ii].result_fields[jj].nof_instances_dnx_data.int_val;
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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, result type %s has more result fields than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n", table_params->name,
                         table_entry->multi_res_info[ii].result_type_name);
        }

        SHR_ALLOC_SET_ZERO(table_entry->multi_res_info[ii].results_info,
                           nof_results_field * sizeof(dbal_table_field_info_t), "results fields info allocation",
                           "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);

        SHR_IF_ERR_EXIT(dbal_db_init_table_add_fields(unit, table_entry, table_params, FALSE, ii));

        /** Result type HW value */
        if (sal_strcmp(table_params->reference_field_name, EMPTY) != 0)
        {
            SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, table_params->reference_field_name,
                                                              &(table_entry->multi_res_info[ii].refernce_field_id)));

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

        /** Result type HW value */
        sal_strncpy_s(table_entry->multi_res_info[ii].result_type_name, table_params->results_set[ii].result_type_name,
                      sizeof(table_entry->multi_res_info[ii].result_type_name));
    }

    /** allocating the last result type for the superset result type support */
    if (table_entry->has_result_type && sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
    {
        SHR_ALLOC_SET_ZERO(table_entry->multi_res_info[ii].results_info,
                           DBAL_TABLE_MAX_NUM_OF_RESULT_FIELDS_PER_RES_TYPE * sizeof(dbal_table_field_info_t),
                           "dynamic rest type allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
    }
    else if (sal_strcasecmp(table_entry->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") == 0)
    {
        int nof_result_fields = dnx_data_dbal.table.nof_dynamic_tables_result_fields_get(unit);

        nof_multi_res_types = dnx_data_dbal.table.nof_dynamic_tables_ppmc_multi_result_types_get(unit);

        /* allocate result_info for empty result types for future dynamic result types result fields. */
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

exit:
    SHR_FUNC_EXIT;
}

static 
shr_error_e 
dbal_db_init_reg_mem_hw_entity_validate(
    int unit, 
    int table_id, 
    int reg_mem_hw_entity_id,
    uint8 is_reg) {
    SHR_FUNC_INIT_VARS(unit);

    if (is_reg && (reg_mem_hw_entity_id == INVALIDr))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "INVALIDr wasn't expected\n");
    }

    if ((!is_reg) && (reg_mem_hw_entity_id == INVALIDm)) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "INVALIDm wasn't expected\n");
    }

    if ((is_reg && (!SOC_REG_IS_VALID(unit, reg_mem_hw_entity_id))) 
        || ((!is_reg) && (!SOC_MEM_IS_VALID(unit, reg_mem_hw_entity_id)))) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, invalif reg/mem: %d \n", 
                     dbal_logical_table_to_string(unit, table_id),
                     reg_mem_hw_entity_id);
    } 
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/** 
 * \brief 
 * Precondition: the reg/mem is valid. 
 *(see dbal_db_init_reg_mem_hw_entity_validate)
 */ 
static 
shr_error_e
dbal_db_init_field_hw_entity_validate(
    int unit, 
    int table_id, 
    dbal_hl_l2p_field_info_t * access,
    uint8 is_reg)
{
    int nof_fields;
    soc_field_info_t *fieldinfo;
    soc_field_info_t *fields_array;

    SHR_FUNC_INIT_VARS(unit);


    /** invalid field, nothing to validate   */
    if (access->hw_field == INVALIDf) {
        SHR_EXIT();
    }

    if (is_reg)
    {
        nof_fields = SOC_REG_INFO(unit, access->reg[0]).nFields;
        fields_array = SOC_REG_INFO(unit,  access->reg[0]).fields;
    }
    else 
    {
        nof_fields = SOC_MEM_INFO(unit,  access->memory[0]).nFields;
        fields_array = SOC_MEM_INFO(unit, access->memory[0]).fields;
    }
    SOC_FIND_FIELD(access->hw_field, fields_array, nof_fields, fieldinfo);
    if (fieldinfo == NULL) {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s, invalif field: %d \n", 
                     dbal_logical_table_to_string(unit, table_id),
                     access->hw_field);
    }
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}


/**
 * \brief
 * get the hw group info includinf the hw entities ids
 * this function is called for each hw entity group.
 */
shr_error_e
dbal_db_init_hw_element_group_handle(
    int unit,
    char *group_name,
    dbal_hl_l2p_field_info_t * access_entry,
    int *is_reg)
{
    hl_groups_info_t *groups_info;
    hl_group_info_t *group_info;
    int group_index;
    int hw_entity_index;
    char hw_entity_name[DBAL_MAX_EXTRA_LONG_STRING_LENGTH];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_groups_info_get(unit, &groups_info));

    (*is_reg) = 0;

    if (groups_info->nof_groups == 0)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "HL groups # is %d\n", groups_info->nof_groups);
    }

    for (group_index = 0; group_index < groups_info->nof_groups; group_index++)
    {
        group_info = &groups_info->group_info[group_index];

        if (sal_strcmp(group_info->group_name, group_name) == 0)
        {
            if (group_info->is_reg == FALSE)
            {
                (*is_reg) = FALSE;

                for (hw_entity_index = 0; hw_entity_index < group_info->group_hw_entity_num; hw_entity_index++)
                {
                    sal_strncpy_s(&hw_entity_name[0], group_info->hw_entity_name[hw_entity_index],
                                  sizeof(hw_entity_name));
                    SHR_IF_ERR_EXIT(dbal_hw_entity_string_to_id
                                    (unit, hw_entity_name, DBAL_HL_ENTITY_MEMORY,
                                     &group_info->hw_entity_id[hw_entity_index]));
                    access_entry->memory[hw_entity_index] = group_info->hw_entity_id[hw_entity_index];
                }
                SHR_EXIT();
            }
            else
            {
                (*is_reg) = TRUE;
                for (hw_entity_index = 0; hw_entity_index < group_info->group_hw_entity_num; hw_entity_index++)
                {
                    sal_strncpy_s(&hw_entity_name[0], group_info->hw_entity_name[hw_entity_index],
                                  sizeof(hw_entity_name));
                    SHR_IF_ERR_EXIT(dbal_hw_entity_string_to_id
                                    (unit, hw_entity_name, DBAL_HL_ENTITY_REGISTER,
                                     &group_info->hw_entity_id[hw_entity_index]));
                    access_entry->reg[hw_entity_index] = group_info->hw_entity_id[hw_entity_index];
                }
                SHR_EXIT();
            }
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "group name %s not found\n", group_name);

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
    hl_group_info_t *group_p;
    hl_group_info_t **group_p_p;

    SHR_FUNC_INIT_VARS(unit);

    groups_info->nof_groups = DBAL_HL_GROUP_NOF_GROUPS;
    groups_info->group_info = NULL;

    SHR_ALLOC_SET_ZERO(groups_info->group_info, (DBAL_HL_GROUP_NOF_GROUPS * sizeof(hl_group_info_t)),
                       "group_info", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    group_p = groups_info->group_info;
    group_p_p = &(group_p);
    SHR_IF_ERR_EXIT(dbal_init_hw_entity_groups_init(unit, group_p_p));

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
    uint32 data_index = 0;
    uint8 success = 0;
    hw_ent_groups_map_hash_key hash_key;
    uint8 is_result_type_mapping;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&hash_key, 0, sizeof(hash_key));

    /** handling group */
    if (sal_strcmp(access_params->access_group_name, EMPTY) != 0)
    {

        int rv;
        sal_strncpy_s(access_entry->group_name, access_params->access_group_name, sizeof(access_entry->group_name));
        rv = dbal_db_init_hw_element_group_handle(unit, access_params->access_group_name, access_entry, &is_reg);
        if (rv)
        {
            SHR_SET_CURRENT_ERR(rv);
            SHR_EXIT();
        }
        /*
         * Memory/Register usage validation
         * Add all memories registers read from groups to the group
         * hash table
         */
        if (check_hw_ent_in_init)
        {
            for (ii = 0; ii < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; ii++)
            {
                if (access_entry->memory[ii] == 0)
                {
                    if (access_entry->alias_memory != 0)
                    {
                        utilex_U32_to_U8((uint32 *) (&access_entry->alias_memory), 4, (uint8 *) &hash_key);
                        ((uint8 *) &hash_key)[4] = 1;
                        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.insert(unit, &hash_key,
                                                                                               &data_index, &success));
                        if (!success)
                        {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding alias memory to validation hash\n")}
                    }
                    break;
                }
                utilex_U32_to_U8((uint32 *) (access_entry->memory + ii), 4, (uint8 *) &hash_key);
                ((uint8 *) &hash_key)[4] = 1;

                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.
                                hw_ent_groups_map_hash_tbl.insert(unit, &hash_key, &data_index, &success));
                if (!success)
                {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding memory to validation hash\n")}
            }

            for (ii = 0; ii < DBAL_MAX_NUMBER_OF_HW_ELEMENTS; ii++)
            {
                if (access_entry->reg[ii] == 0)
                {
                    if (access_entry->alias_reg != 0)
                    {
                        utilex_U32_to_U8((uint32 *) (&access_entry->alias_reg), 4, (uint8 *) &hash_key);
                        ((uint8 *) &hash_key)[4] = 2;
                        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.insert(unit, &hash_key,
                                                                                               &data_index, &success));
                        if (!success)
                        {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding alias register to validation hash\n")}
                    }
                    break;
                }
                utilex_U32_to_U8((uint32 *) (access_entry->reg + ii), 4, (uint8 *) &hash_key);
                ((uint8 *) &hash_key)[4] = 2;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.insert(unit,
                                                                                       &hash_key, &data_index,
                                                                                       &success));
                if (!success)
                {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding register to validation hash\n")}
            }
        }
    }
    else
    {

        if (access_params->access_type == DBAL_HL_ACCESS_MEMORY)
        {
              DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_MEM_ACCESS);
            access_entry->memory[0] = access_params->reg_mem_hw_entity_id;
            SHR_IF_ERR_EXIT(dbal_db_init_reg_mem_hw_entity_validate(unit, table_id, access_entry->memory[0], FALSE /* is reg */));

            if (check_hw_ent_in_init)
            {
                utilex_U32_to_U8((uint32 *) (access_entry->memory), 4, (uint8 *) &hash_key);
                ((uint8 *) &hash_key)[4] = 1;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.insert(unit,
                                                                                       (hw_ent_direct_map_hash_key *) &
                                                                                       hash_key, &data_index,
                                                                                       &success));
                if (!success)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding memory to validation hash\n");
                }
            }

            /** Alias Memory */
            if (sal_strcmp(access_params->alias_name, EMPTY) != 0)
            {
                int rv = 0;
                rv = dbal_hw_entity_string_to_id(unit, access_params->alias_name, DBAL_HL_ENTITY_MEMORY,
                                                 &access_entry->alias_memory);
                if (rv)
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
                }

                if (check_hw_ent_in_init)
                {
                    utilex_U32_to_U8((uint32 *) (&access_entry->alias_memory), 4, (uint8 *) &hash_key);
                    ((uint8 *) &hash_key)[4] = 1;
                    SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.insert
                                    (unit, (hw_ent_direct_map_hash_key *) & hash_key, &data_index, &success));
                    if (!success)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding alias memory to validation hash\n");
                    }
                }
            }
                    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_MEM_ACCESS);
        }
        else
        {
            DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_REG_ACCESS);
            access_entry->reg[0] = access_params->reg_mem_hw_entity_id;
            SHR_IF_ERR_EXIT(dbal_db_init_reg_mem_hw_entity_validate(unit, table_id, access_entry->reg[0], TRUE /* is reg */));
            if (access_entry->reg[0] == FDA_GTIMER_CYCLEr) {
            }

            if (check_hw_ent_in_init)
            {
                utilex_U32_to_U8((uint32 *) (access_entry->reg), 4, (uint8 *) &hash_key);
                ((uint8 *) &hash_key)[4] = 2;
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.insert
                                (unit, (hw_ent_direct_map_hash_key *) & hash_key, &data_index, &success));
                if (!success)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding register to validation hash\n");
                }
            }

            /** Alias Register */
            if (sal_strcmp(access_params->alias_name, EMPTY) != 0)
            {
                int rv = 0;
                rv = dbal_hw_entity_string_to_id(unit, access_params->alias_name, DBAL_HL_ENTITY_REGISTER,
                                                 &access_entry->alias_reg);
                if (rv)
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
                }

                if (check_hw_ent_in_init)
                {
                    utilex_U32_to_U8((uint32 *) (&access_entry->alias_reg), 4, (uint8 *) &hash_key);
                    ((uint8 *) &hash_key)[4] = 2;
                    SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.insert
                                    (unit, (hw_ent_direct_map_hash_key *) & hash_key, &data_index, &success));
                    if (!success)
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error while adding alias register to validation hash\n");
                    }
                }
            }
            is_reg = 1;
            DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG_REG_ACCESS);
        }
    }

    access_entry->hw_field = access_params->hw_field;

    /** add a validation that field exist for the mem/reg.
     *  otherwise, return an error to comply with the previsous
     *  implementation */
    SHR_IF_ERR_EXIT(dbal_db_init_field_hw_entity_validate(unit, table_id, access_entry, is_reg));
    /*
    if (access_entry->hw_field != INVALIDf)
    {
        if (is_reg)
        {
            SHR_IF_ERR_EXIT(dbal_hw_field_form_reg_string_to_id
                            (unit, access_params->hw_field, access_entry->reg[0], &access_entry->hw_field));
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_hw_field_form_mem_string_to_id
                            (unit, access_params->hw_field, access_entry->memory[0], &access_entry->hw_field));
        }
        if (access_entry->hw_field == -1)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            SHR_EXIT();
        }
    }   
    */
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
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Table %s has more conditions than max allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n",
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
dbal_db_init_table_add_mapping_hl(
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

    CONST static dbal_hl_l2p_field_info_t hl_default_mapping = {
        DBAL_FIELD_EMPTY,   /** field_id */
        0,                  /**hl_tcam_access_type*/
        0,                  /** access_nof_bits*/
        0,                  /** access_offset*/
        {0},                /** encode_info */
        0,                  /** field_pos_in_interface*/
        0,                  /** nof_bits_in_interface*/
        0,                  /** offset_in_interface*/
        INVALID_ARRm,       /** memory[DBAL_MAX_NUMBER_OF_HW_ELEMENTS]*/
        INVALID_ARRr,       /** reg[DBAL_MAX_NUMBER_OF_HW_ELEMENTS]*/
        INVALIDf,           /** hw_field*/
        EMPTY,              /** group_name*/
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

        /** In case of None memory, it means that the access block is missing */
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
        /** access type */
        access_type = table_params->hl_access[acc_map_idx].access[ii].access_type;

        /** In case of None memory, it means that the access block is missing */
        /** To Validate with Niv/Ronen
        if (sal_strcmp(table_params->hl_access[acc_map_idx].access[ii].access_name, "NONE") == 0)
        {
            continue;
        }
        */
        access_entry = hl_mapping->l2p_hl_info[access_type].l2p_fields_info +
            hl_mapping->l2p_hl_info[access_type].num_of_access_fields;

        hl_mapping->l2p_hl_info[access_type].num_of_access_fields++;

        if (table_entry->table_type == DBAL_TABLE_TYPE_DIRECT)
        {
            access_entry->field_id = table_params->hl_access[acc_map_idx].access[ii].access_field_id;
        }
        else if (table_entry->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT
                 || table_entry->table_type == DBAL_TABLE_TYPE_TCAM)
        {
            /** DBAL_TABLE_TYPE_TCAM the access field names can be key mask or result */
            access_entry->hl_tcam_access_type = table_params->hl_access[acc_map_idx].access[ii].tcam_access_type;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                         dbal_table_type_to_string(unit, table_entry->table_type), table_entry->table_name);
        }

        access_entry->access_nof_bits = table_params->hl_access[acc_map_idx].access[ii].access_size;
        access_entry->access_offset = table_params->hl_access[acc_map_idx].access[ii].access_offset;

        switch (access_type)
        {
            case DBAL_HL_ACCESS_MEMORY:
            case DBAL_HL_ACCESS_REGISTER:
            {

                int rv = 0;
                DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG);
                rv = dbal_db_init_table_add_hl_memory_register(unit, table_id, access_entry,
                                                               &table_params->hl_access[acc_map_idx].access[ii]);
                DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL_MEM_REG);
                if (rv)
                {
                    SHR_SET_CURRENT_ERR(rv);
                    SHR_EXIT();
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

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * decided mdb_image_type
 */
static void
dbal_db_init_mdb_image_type_set(
    int unit,
    dbal_logical_table_t * table_entry,
    table_db_struct_t * table_params)
{
    if (table_params->mdb_access->is_standard_1_table)
    {
        if (table_params->mdb_access->is_standard_1_image)
        {
            table_entry->mdb_image_type = DBAL_MDB_IMG_STD_1_ACTIVE;
        }
        else
        {
            table_entry->mdb_image_type = DBAL_MDB_IMG_STD_1_NOT_ACTIVE;
        }
    }
    else
    {
        table_entry->mdb_image_type = DBAL_MDB_IMG_NOT_STD_1;
    }
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
    if ((table_entry->core_mode == DBAL_CORE_MODE_DPC) || (physical_table->physical_core_mode == DBAL_CORE_MODE_DPC))
    {
        int core_idx;
        for (core_idx = 1; core_idx < DBAL_MAX_NUM_OF_CORES; core_idx++)
        {
            table_entry->physical_db_id[core_idx] = table_entry->physical_db_id[0] + core_idx;
        }
        table_entry->nof_physical_tables = DBAL_MAX_NUM_OF_CORES;
    }

    if (table_entry->physical_db_id[0] == DBAL_PHYSICAL_TABLE_TCAM)
    {
        if (sal_strcmp(table_params->pipe_stage, EMPTY) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "TCAM tables have to be mapped to a stage. table:%s\n",
                         table_entry->table_name);
        }
        SHR_IF_ERR_EXIT(dbal_stage_string_to_id(unit, table_params->pipe_stage, &table_entry->dbal_stage));
    }

    /** Set image type of table */
    dbal_db_init_mdb_image_type_set(unit, table_entry, table_params);

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
    int hl_sw_size;
    dbal_tcam_cs_stage_info_t cs_tcam_db_stage_info;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dbal_stage_string_to_id(unit, table_params->pipe_stage, &table_entry->dbal_stage));

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

    SHR_ALLOC_SET_ZERO(table_entry->hl_mapping_multi_res, sizeof(dbal_hl_access_info_t),
                       "multiple result direct mapping allocation", "%s%s%s\r\n",
                       table_entry->table_name, EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_hl(unit, table_id, table_entry, table_params, 0, 0, &hl_sw_size));

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

    table_entry->app_id = table_params->pemla_db_id_dnx_data.int_val;

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
    int ii, jj;
    int rv;
    int max_hl_sw_part_size_bytes = 0, hl_sw_sizse_per_result_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_INTERFACE);


    /** find the field id according to its name */
    SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table_params->name, &table_id));
    table_entry = table_info + table_id;

    /** Physical mapping part */
    table_entry->access_method = table_params->access_method;

    /** Validity */
    if (table_params->valid_dnx_data.int_val == 0)
    {
        if (!SOC_WARM_BOOT(unit))
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_NOF_TABLE_STATUS));
        }
        table_entry->maturity_level = DBAL_MATURITY_LOW;
        SHR_EXIT();
    }

    if (table_entry->access_method == DBAL_ACCESS_METHOD_KBP)
    {
        /** Disable table on real device if KBP device is not enabled */
        if (!dnx_kbp_device_enabled(unit))
        {
            if (!SOC_WARM_BOOT(unit))
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_NOF_TABLE_STATUS));
            }
            SHR_EXIT();
        }
    }
    /** set interface data */
    SHR_IF_ERR_EXIT(dbal_db_init_table_add_interface_to_table(unit, table_entry, table_params));

    /** Core Mode, if core mode not set the table is set to HW error and exit (can be because there is no mapping to this
     *  table for this device) */
    if ((sal_strcasecmp("", table_params->core_mode) == 0))
    {
        SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
        SHR_EXIT();
    }
    rv = dbal_core_mode_string_to_id(unit, table_params->core_mode, &table_entry->core_mode);
    if (SHR_FAILURE(rv))
    {
        SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
        SHR_EXIT();
    }

    DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_INTERFACE);


    switch (table_entry->access_method)
    {
        case DBAL_ACCESS_METHOD_HARD_LOGIC:

            table_entry->is_hook_active = table_params->is_hook_active_dnx_data.int_val;

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
                DNX_INIT_TIME_ANALYZER_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL);

                rv = dbal_db_init_table_add_mapping_hl(unit, table_id, table_entry, table_params,
                                                       ii, jj, &hl_sw_sizse_per_result_type);
                DNX_INIT_TIME_ANALYZER_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_LOGICAL_INIT_ADD_DBAL_TABLE_MAPPING_HL);

                if (SHR_FAILURE(rv))
                {
                    SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
                    SHR_EXIT();
                }
                max_hl_sw_part_size_bytes =
                    UTILEX_MAX(max_hl_sw_part_size_bytes, BITS2BYTES(hl_sw_sizse_per_result_type));
            }

            if (max_hl_sw_part_size_bytes > 0)
            {
                /** Indicate that an Hard logic table has fields mapped to SW */
                table_entry->sw_payload_length_bytes = max_hl_sw_part_size_bytes;
            }

            break;

        case DBAL_ACCESS_METHOD_MDB:
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_mdb(unit, table_entry, table_params));
            break;

        case DBAL_ACCESS_METHOD_TCAM_CS:
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_tcam_cs(unit, table_entry, table_id, table_params));
            break;

        case DBAL_ACCESS_METHOD_SW_STATE:
            SHR_ALLOC_SET_ZERO(table_entry->table_size_str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH,
                                           "table labels allocation", "%s%s%s\r\n", table_entry->table_name, EMPTY, EMPTY);
            sal_memcpy(table_entry->table_size_str, table_params->table_size_str, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
            break;

        case DBAL_ACCESS_METHOD_PEMLA:
            SHR_IF_ERR_EXIT(dbal_db_init_table_add_mapping_pemla(unit, table_entry, table_id, table_params));
            break;

        case DBAL_ACCESS_METHOD_KBP:
            /** TBD kbp related xml information parsing */
            dbal_db_init_mdb_image_type_set(unit, table_entry, table_params);
            break;
        default:
            break;
    }

    SHR_IF_ERR_EXIT(dbal_db_init_table_capacity_calculate(unit, table_entry, table_params));

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
    char *print_type)
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

shr_error_e
dbal_db_init_table_set_access_register_memory(
    int unit,
    table_db_access_params_struct_t * access_params,
/*    int * hw_entity_ids_in_group, 
    int nof_hw_entity_ids_in_group,
    */
    char *group_name,
    int reg_mem_hw_entity_id, 
    char *group_offset_formula,
    int  group_offset, 
    char *array_offset_formula,
    int  array_offset, 
    char *entry_offset_formula,
    int  entry_offset, 
    char *block_index_formula,
    int block_index, 
    char *data_offset_formula,
    int data_offset, 
    int hw_field,
    char *alias_name,
    char *alias_data_offset_formula, 
    int alias_data_offset)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
    access_params->nof_hw_entity_ids_in_group = nof_hw_entity_ids_in_group;
    if (access_params->nof_hw_entity_ids_in_group > 0) {
        sal_memcpy(access_params->hw_entity_ids_in_group, hw_entity_ids_in_group, 
                   access_params->nof_hw_entity_ids_in_group * sizeof(int));
    }
     * */ 

    sal_strncpy_s(access_params->access_group_name, group_name, sizeof(access_params->access_group_name));

    /** invalidr/invalidm if has group */ 
    access_params->reg_mem_hw_entity_id = reg_mem_hw_entity_id; 

    sal_strncpy_s(access_params->group_offset.formula, group_offset_formula,
                  sizeof(access_params->group_offset.formula));
    access_params->group_offset.formula_int = group_offset;
    sal_strncpy_s(access_params->array_offset.formula, array_offset_formula,
                  sizeof(access_params->array_offset.formula));
    access_params->array_offset.formula_int  = array_offset;
    sal_strncpy_s(access_params->entry_offset.formula, entry_offset_formula,
                  sizeof(access_params->entry_offset.formula));
    access_params->entry_offset.formula_int = entry_offset;
    sal_strncpy_s(access_params->block_index.formula, block_index_formula, sizeof(access_params->block_index.formula));
    access_params->block_index.formula_int = block_index;
    sal_strncpy_s(access_params->data_offset.formula, data_offset_formula, sizeof(access_params->data_offset.formula));
    access_params->data_offset.formula_int = data_offset;
    /** invalidf is no field specified */
    access_params->hw_field = hw_field;

    sal_strncpy_s(access_params->alias_name, alias_name, sizeof(access_params->alias_name));
    sal_strncpy_s(access_params->alias_data_offset.formula, alias_data_offset_formula,
                  sizeof(access_params->alias_data_offset.formula));
    access_params->alias_data_offset.formula_int = alias_data_offset;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_access_condition(
    int unit,
    table_db_access_condition_struct_t * access_condition,
    char *type,
    char *field,
    char *value,
    char *enum_value)
{
    SHR_FUNC_INIT_VARS(unit);
    if (sal_strcmp(type, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_condition_string_to_id(unit, type, &access_condition->condition_type));
    }
    access_condition->field_id = DBAL_FIELD_EMPTY;

    if (sal_strcmp(field, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field, &access_condition->field_id));
    }

    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &access_condition->value_dnx_data, value, FALSE, 0, "", ""));
    sal_strncpy_s(access_condition->enum_val, enum_value, sizeof(access_condition->enum_val));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_access_packed_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    char *field_size,
    char *encode_type,
    char *encode_value)
{
    dbal_db_int_or_dnx_data_info_struct_t field_size_dnx_data;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_size_dnx_data, field_size, FALSE, 0, "", ""));
    access_params->access_size = field_size_dnx_data.int_val;

    sal_strncpy_s(access_params->encode_type, encode_type, sizeof(access_params->encode_type));
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &access_params->encode_param1_dnx_data, encode_value, FALSE, 0, "", ""));
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_db_init_table_set_access_field(
    int unit,
    table_db_access_params_struct_t * access_params,
    char orig_field_name[DBAL_MAX_STRING_LENGTH],
    int instance_index,
    char *field_size,
    char *field_offset,
    uint8 is_tcam,
    char *encode_type,
    char *encode_value)
{
    dbal_db_int_or_dnx_data_info_struct_t field_size_dnx_data, field_offset_dnx_data;
    char field_name[DBAL_MAX_STRING_LENGTH];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * field name
     */
    sal_strncpy_s(field_name, orig_field_name, sizeof(field_name));
    /*
     * instance index 0, is for the field name without suffix
     */
    if (instance_index != 0)
    {
        sal_snprintf(field_name, sizeof(field_name), "%s_%03d", orig_field_name, instance_index);
    }

    /**
     * access field id/tcam access type.
     */
    if (is_tcam)
    {
        SHR_IF_ERR_EXIT(dbal_hl_tcam_access_type_string_to_id(unit, field_name, &access_params->tcam_access_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_field_string_to_id(unit, field_name, &access_params->access_field_id));
    }
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_size_dnx_data, field_size, FALSE, 0, "", ""));
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_offset_dnx_data, field_offset, FALSE, 0, "", ""));

    access_params->access_size = field_size_dnx_data.int_val;
    access_params->access_offset = field_offset_dnx_data.int_val;

    sal_strncpy_s(access_params->encode_type, encode_type, sizeof(access_params->encode_type));
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &access_params->encode_param1_dnx_data, encode_value, FALSE, 0, "", ""));
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

void
dbal_db_init_table_db_struct_access_clear(
    table_db_struct_t * table_db)
{
    switch (table_db->access_method)
    {
        case DBAL_ACCESS_METHOD_MDB:
            sal_memset(table_db->mdb_access, 0x0, sizeof(table_db_mdb_access_struct_t));
            break;
        case DBAL_ACCESS_METHOD_TCAM_CS:
        case DBAL_ACCESS_METHOD_HARD_LOGIC:
            sal_memset(table_db->hl_access, 0x0, table_db->num_of_results_sets * sizeof(table_db_hl_access_struct_t));
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

shr_error_e
dbal_db_init_field_types(
    int unit,
    int flags,
    dbal_field_types_basic_info_t * field_types_info)
{
    field_type_db_struct_t *cur_field_type_param = NULL;
    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(cur_field_type_param, sizeof(field_type_db_struct_t),
              "field_types struct", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->childs = NULL;
    SHR_ALLOC(cur_field_type_param->childs,
              DBAL_FIELD_MAX_NUM_OF_CHILD_FIELDS * sizeof(dbal_db_child_field_info_struct_t), "field_types,childs",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->enums = NULL;
    SHR_ALLOC(cur_field_type_param->enums, DBAL_FIELD_MAX_NUM_OF_ENUM_VALUES * sizeof(dbal_db_enum_info_struct_t),
              "field_types,enums", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->structs = NULL;
    SHR_ALLOC(cur_field_type_param->structs,
              DBAL_FIELD_MAX_NUM_OF_STRUCT_FIELDS * sizeof(dbal_db_struct_field_info_struct_t), "field_types,structs",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->define = NULL;
    SHR_ALLOC(cur_field_type_param->define, DBAL_FIELD_MAX_NUM_OF_DEFINE_VALUES * sizeof(dbal_db_defines_info_struct_t),
              "field_types,define", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_field_type_param->illegal_values_dnx_data = NULL;
    SHR_ALLOC(cur_field_type_param->illegal_values_dnx_data,
              DBAL_DB_MAX_NUM_OF_ILLEGAL_VALUES * sizeof(dbal_db_int_or_dnx_data_info_struct_t),
              "field_types,illegal_values_dnx_data", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    /** Read xml DB */
    SHR_IF_ERR_EXIT(dbal_init_field_types_init(unit, cur_field_type_param, field_types_info));
exit:
    SHR_FREE(cur_field_type_param->childs);
    SHR_FREE(cur_field_type_param->enums);
    SHR_FREE(cur_field_type_param->structs);
    SHR_FREE(cur_field_type_param->define);
    SHR_FREE(cur_field_type_param->illegal_values_dnx_data);
    SHR_FREE(cur_field_type_param);
    SHR_FUNC_EXIT;
}

/*************************************************************************************************************************************************/


shr_error_e
dbal_db_init_general_info_set(
    int unit,
    table_db_struct_t * cur_table_param,
    char *dbal_table_name,
    char *valid,
    dbal_access_method_e access_method)
{
    SHR_FUNC_INIT_VARS(unit);
    /** set dbal table name  */
    sal_strncpy_s(cur_table_param->name, dbal_table_name, sizeof(cur_table_param->name));
    /** set valid */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &cur_table_param->valid_dnx_data, valid, FALSE, 1, "", ""));
    /** set access method */
    cur_table_param->access_method = access_method;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_maturity_level_set(
    int unit,
    table_db_struct_t * cur_table_param,
    dbal_maturity_level_e maturity_level)
{
    SHR_FUNC_INIT_VARS(unit);

    cur_table_param->maturity_level = maturity_level;

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

    if(const_value != NULL)
    {
        uint8 enum_found = FALSE;
        int enum_idx = 0;
        CONST dbal_field_types_basic_info_t * field_type_info;

        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_db->field_type, &field_type_info));
        if(field_type_info->nof_enum_values > 0)
        {
            char enum_name[DBAL_MAX_LONG_STRING_LENGTH];
            sal_strncpy_s(enum_name, field_type_info->name, DBAL_MAX_LONG_STRING_LENGTH);
            sal_strncat(enum_name, "_", sizeof("_"));
            sal_strncat(enum_name, const_value, DBAL_MAX_LONG_STRING_LENGTH - sizeof(field_type_info->name) - 1);

            for(enum_idx = 0; enum_idx < field_type_info->nof_enum_values; enum_idx++)
            {
                if((sal_strcmp(enum_name, field_type_info->enum_val_info[enum_idx].name) == 0) ||
                   
                   (sal_strstr(field_type_info->enum_val_info[enum_idx].name, const_value) != NULL))
                {
                    enum_found = TRUE;
                    field_db->const_value_dnx_data.int_val = enum_idx;
                    break;
                }
            }

            if(!enum_found)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Const value for ENUM field in not in enum names. field_name:%s, const name:%s\n",
                             field_db->name, const_value);
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

shr_error_e
dbal_db_init_table_set_field(
    int unit,
    table_db_field_params_struct_t * field_db,
    char *field_name,
    char *valid,
    char *size,
    char *offset,
    char *max_value,
    char *min_value,
    char *const_value,
    char *nof_instances,
    dbal_field_permission_e permission,
    uint8 is_reverse_order,
    char *field_type)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * field name
     */
    sal_strncpy_s(field_db->name, field_name, sizeof(field_db->name));
    /*
     * valid
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_db->valid_dnx_data, valid, FALSE, 1, "", ""));
    /*
     * size
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_db->size_dnx_data, size, FALSE, 0, "", ""));
    /*
     * offset
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication(unit, &field_db->offset_dnx_data, offset, FALSE, 0, "", ""));
    /*
     * max value
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &field_db->max_value_dnx_data, max_value, FALSE, 0, "", ""));
    /*
     * min value
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &field_db->min_value_dnx_data, min_value, FALSE, 0, "", ""));
    /*
     * nof instances
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &field_db->nof_instances_dnx_data, nof_instances, FALSE, 1, "", ""));
    /*
     * read only
     */
    field_db->permission = permission;
    /*
     * reverse order
     */
    field_db->is_reverse_order = is_reverse_order;
    /*
     * type
     */
    /*
     * Ag code return NULL if didn't find field_type. Check what is empty
     */
    if ((field_type != NULL) && sal_strcmp(field_type, EMPTY) != 0)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, field_type, &field_db->field_type));
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_field_types_def_string_to_id(unit, field_db->name, &field_db->field_type));
    }

    /*
     * Const Value resolution
     */
    SHR_IF_ERR_EXIT(dbal_db_init_field_in_table_const_value_parsing(unit, field_db, const_value));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_table_set_field_arr_prefix(
    int unit,
    table_db_field_params_struct_t * field_db,
    char *arr_prefix_value,
    char *arr_prefix_width)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * arr prefix value
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &field_db->arr_prefix_value_dnx_data, arr_prefix_value, FALSE, 0, "", ""));
    /*
     * arr prefix width
     */
    SHR_IF_ERR_EXIT(dbal_db_parse_dnx_data_indication
                    (unit, &field_db->arr_prefix_width_dnx_data, arr_prefix_width, FALSE, 0, "", ""));

exit:
    SHR_FUNC_EXIT;
}

/** from  dbal_db_init_table_read_interface */
shr_error_e
dbal_db_init_app_db_set_interface(
    int unit,
    table_db_struct_t * cur_table_param,
    char *type,
    char *labels)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * type
     */
    sal_strncpy_s(cur_table_param->type, type, sizeof(cur_table_param->type));
    /*
     * label
     */
    sal_strncpy_s(cur_table_param->labels, labels, sizeof(cur_table_param->labels));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;

}

shr_error_e
dbal_db_init_table_set_app_to_phy_db(
    int unit,
    table_db_struct_t * cur_table_param,
    char *core_mode,
    char *reference_field_name)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * core mode
     */
    sal_strncpy_s(cur_table_param->core_mode, core_mode, sizeof(cur_table_param->core_mode));
    /*
     * reference field name
     */
    if (reference_field_name == NULL)
    {
        sal_strncpy_s(cur_table_param->reference_field_name, EMPTY, sizeof(cur_table_param->reference_field_name));
    }
    else
    {
        sal_strncpy_s(cur_table_param->reference_field_name, reference_field_name,
                      sizeof(cur_table_param->reference_field_name));
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
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
                             logical_value, cur_table_param->name);
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
                     cur_table_param->name);
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
    table_db_pemla_access_struct_t *pemla_access;
    table_db_results_field_set_struct_t *results_set;

    /*
     * save pointers to restore them aftet clear
     */
    mdb_access = cur_table_param->mdb_access;
    hl_access = cur_table_param->hl_access;
    pemla_access = cur_table_param->pemla_access;
    results_set = cur_table_param->results_set;

    /** clear section of the table_db_struct_t which is common between all types of tables. */
    sal_memset(cur_table_param, 0x0, sizeof(table_db_struct_t));
    /** restore pointers after clear */
    cur_table_param->mdb_access = mdb_access;
    cur_table_param->hl_access = hl_access;
    cur_table_param->pemla_access = pemla_access;
    cur_table_param->results_set = results_set;
}

char *
dbal_db_init_image_name_get(
    int unit)
{
    return dnx_data_pp.application.device_image_get(unit)->name;
}

void
dbal_db_init_mdb_image_name_is_std_1(
    int unit,
    uint8 *mdb_image_is_std_1)
{
    char *mdb_image_name = NULL;

    /** Read MDB DBAL image SoC Property - if not exists, assume standard_1 */
    mdb_image_name = soc_property_suffix_num_str_get(unit, -1, spn_CUSTOM_FEATURE, "device_image");

    if (mdb_image_name == NULL)
    {
        *mdb_image_is_std_1 = TRUE;
    }
    else if (sal_strncasecmp(mdb_image_name, "standard_1", RHFILE_MAX_SIZE) == 0)
    {
        *mdb_image_is_std_1 = TRUE;
    }
    else
    {
        *mdb_image_is_std_1 = FALSE;
    }
}

shr_error_e
dbal_db_init_logical_tables(
    int unit,
    int flags,
    dbal_logical_table_t * table_info)
{
    sw_state_htbl_init_info_t hash_tbl_init_info;
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
    cur_table_param->pemla_access = NULL;
    SHR_ALLOC(cur_table_param->pemla_access, sizeof(table_db_pemla_access_struct_t),
              "logical_table,pemla_access", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    cur_table_param->results_set = NULL;
    SHR_ALLOC(cur_table_param->results_set,
              DBAL_MAX_NUMBER_OF_RESULT_TYPES * sizeof(table_db_results_field_set_struct_t),
              "logical_table,results_set", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    /** create the hash tables for HW verification */
    sal_memset(&hash_tbl_init_info, 0, sizeof(hash_tbl_init_info));
    hash_tbl_init_info.max_nof_elements = DBAL_NOF_TABLES * DBAL_MAX_NUMBER_OF_HW_ELEMENTS;
    hash_tbl_init_info.expected_nof_elements = hash_tbl_init_info.max_nof_elements;

    if (!SOC_WARM_BOOT(unit))
    {
        check_hw_ent_in_init = TRUE;
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.create(unit, &hash_tbl_init_info));
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.create(unit, &hash_tbl_init_info));
    }

    /** tables are initialiazed separately by AG code */

    SHR_IF_ERR_EXIT(dbal_init_tables_hard_logic_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_sw_state_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_tcam_context_selection_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_pemla_init(unit, cur_table_param, table_info));
    SHR_IF_ERR_EXIT(dbal_init_tables_mdb_init(unit, cur_table_param, table_info));

exit:
    SHR_FREE(cur_table_param->mdb_access);
    SHR_FREE(cur_table_param->hl_access);
    SHR_FREE(cur_table_param->pemla_access);
    SHR_FREE(cur_table_param->results_set);
    SHR_FREE(cur_table_param);
    SHR_FUNC_EXIT;
}

/***************************************************************************************************************************************/

shr_error_e
dbal_field_types_basic_info_t_init(
    int unit,
    dbal_field_types_basic_info_t * field_type_info)
{
    CONST static dbal_field_types_basic_info_t field_type_default = {
        EMPTY,                          /** name */
        0,                              /** max_size */
        DBAL_FIELD_PRINT_TYPE_NONE,     /** print_type */
        FALSE,                          /** is_allocator */
        0,                              /** default_value_valid */
        0,                              /** default_value */
        0,                              /** max_value */
        0,                              /** min_value */
        0,                              /** const_value */
        0,                              /** const_value_valid */
        0,                              /** nof_illegal_values */
        {0},                            /** illegal_values */
        0,                              /** nof_child_fields */
        NULL,                           /** sub_fields_info */
        0,                              /** nof_struct_fields */
        NULL,                           /** struct_fields_info */
        0,                              /** nof_enum_values */
        0,                              /** nof_invalid_enum_values */
        NULL,                           /** enum_val_info */
        0,                              /** nof_defines_values */
        NULL,                           /** defines_info */
        0,                              /** refernce_field_id */
        {0}                             /** encode_info */
    };
    SHR_FUNC_INIT_VARS(unit);

    (*field_type_info) = field_type_default;
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_field_types_set_default(
    int unit,
    dbal_field_types_basic_info_t * field_types_info)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);
    /** Initialize all fields */
    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_basic_info_t_init(unit, &(field_types_info[ii])));
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_table_t_init(
    int unit,
    dbal_logical_table_t * table_info)
{
    CONST static dbal_logical_table_t table_default = {
            "",                         /** table_name */
            DBAL_MATURITY_LOW,          /** maturity_level */
            0,                          /** nof _labels */
            NULL,                       /** table_labels */
            DBAL_TABLE_TYPE_NONE,       /** table_type */
            0,                          /** max_capacity */
            0,                          /** range_set_supported */
            0,                          /** nof_key_fields */
            NULL,                       /** keys_info */
            0,                          /** key_size */
            0,                          /** core_id_nof_bits */
            0,                          /** has_result_type */
            0,                          /** result_type_mapped_to_sw */
            DBAL_FIELD_EMPTY,           /** allocator_field_id */
            0,                          /** nof_result_types */
            NULL,                       /** multi_res_info */
            0,                          /** max_payload_size */
            0,                          /** max_nof_result_fields */
            DBAL_CORE_NONE,             /** core_mode */
            DBAL_NOF_ACCESS_METHODS,    /** access_method */
            0,                          /** is_hook_active */
            0,                          /** sw_state_nof_entries */
            0,                          /** sw_payload_length_bytes */
            NULL,                       /** table_size_str */
            0,                          /** nof_physical_tables */
            {DBAL_PHYSICAL_TABLE_NONE}, /** physical_db_id */
            0,                          /** app_id */
            0,                          /** app_id_size */
            DBAL_MDB_IMG_STD_1_ACTIVE,  /** mdb_image_type */
            DBAL_NOF_STAGES,            /** dbal_stage */
            NULL,                       /** l2p_hl_info */
            {NULL},                     /** pemla_mapping */
            NULL                        /** kbp_handles */
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
    table_info->range_set_supported = 0;
    table_info->nof_key_fields = 0;
    table_info->key_size = 0;
    table_info->core_id_nof_bits = 0;
    table_info->has_result_type = 0;
    table_info->result_type_mapped_to_sw = 0;
    table_info->allocator_field_id = DBAL_FIELD_EMPTY;
    table_info->nof_result_types = 0;
    table_info->max_payload_size = 0;
    table_info->max_nof_result_fields = 0;
    table_info->core_mode = DBAL_CORE_NONE;
    table_info->access_method = DBAL_NOF_ACCESS_METHODS;
    table_info->is_hook_active = 0;
    table_info->sw_state_nof_entries = 0;
    table_info->sw_payload_length_bytes = 0;
    table_info->nof_physical_tables = 0;
    table_info->physical_db_id[0] = DBAL_PHYSICAL_TABLE_NONE;
    table_info->physical_db_id[1] = DBAL_PHYSICAL_TABLE_NONE;
    table_info->app_id = 0;
    table_info->app_id_size = 0;
    table_info->mdb_image_type = DBAL_MDB_IMG_STD_1_ACTIVE;
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
    for (table_index = 0; table_index < DBAL_NOF_TABLES ; table_index++)
    {
        SHR_IF_ERR_EXIT(dbal_logical_table_t_init(unit, &(table_info[table_index])));
    }

exit:
    SHR_FUNC_EXIT;
}

/** this function updates the sizes of the struct fields that are
 *  not set in the XML from the field type and according to the sizes and order
 *  setting the offsets of all the struct fields
 */
shr_error_e
dbal_fields_update_structure_info(
    int unit,
    dbal_field_types_basic_info_t * field_types_info)
{
    int ii;
    dbal_field_types_basic_info_t *field_type_info;
    SHR_FUNC_INIT_VARS(unit);
    /** first updating all the sizes */
    for (ii = 0; ii < field_types_info->nof_struct_fields; ii++)
    {
        if ((field_types_info->struct_field_info[ii].length == 0))
        {
            SHR_IF_ERR_EXIT(dbal_fields_field_types_info_get
                            (unit, field_types_info->struct_field_info[ii].struct_field_id, &field_type_info));
            field_types_info->struct_field_info[ii].length = field_type_info->max_size;
        }
    }

    /** updating offsets */
    for (ii = 0; ii < field_types_info->nof_struct_fields; ii++)
    {
        if (ii != 0)
        {
            field_types_info->struct_field_info[ii].offset = field_types_info->struct_field_info[ii - 1].length +
                field_types_info->struct_field_info[ii - 1].offset + field_types_info->struct_field_info[ii].offset;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_field_types_logical_validation(
    int unit,
    dbal_field_types_basic_info_t * field_types_info)
{
    int ii;
    int max_field_size;
    SHR_FUNC_INIT_VARS(unit);
    for (ii = DBAL_FIELD_TYPE_DEF_EMPTY + 1; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        /** validate max size of field */
        switch (field_types_info[ii].print_type)
        {
            case DBAL_FIELD_PRINT_TYPE_ARRAY32:
            case DBAL_FIELD_PRINT_TYPE_ARRAY8:
            case DBAL_FIELD_PRINT_TYPE_BITMAP:
            case DBAL_FIELD_PRINT_TYPE_HEX:
            case DBAL_FIELD_PRINT_TYPE_DBAL_TABLE:
            case DBAL_FIELD_PRINT_TYPE_STRING:
                max_field_size = BYTES2BITS(DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES);
                break;
            case DBAL_FIELD_PRINT_TYPE_BOOL:
                max_field_size = 1;
                break;
            case DBAL_FIELD_PRINT_TYPE_UINT32:
            case DBAL_FIELD_PRINT_TYPE_IPV4:
                max_field_size = 32;
                break;
            case DBAL_FIELD_PRINT_TYPE_STRUCT:
                max_field_size = 256;
                SHR_IF_ERR_EXIT(dbal_fields_update_structure_info(unit, &field_types_info[ii]));
                break;
            case DBAL_FIELD_PRINT_TYPE_ENUM:
                max_field_size = 32;
                break;
            case DBAL_FIELD_PRINT_TYPE_SYSTEM_CORE:
                max_field_size = 1;
                break;
            case DBAL_FIELD_PRINT_TYPE_MAC:
                max_field_size = 48;
                break;
            case DBAL_FIELD_PRINT_TYPE_IPV6:
                max_field_size = 128;
                break;
            case DBAL_FIELD_PRINT_TYPE_NONE:
            case DBAL_NOF_FIELD_PRINT_TYPES:
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown field print type, field type %s\n",
                             dbal_field_types_def_to_string(unit, ii));
        }
        if (max_field_size > DBAL_FIELD_ARRAY_MAX_SIZE_IN_BITS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field print type %s max size is larger than allowed."
                         "update the max allowed in dbal_defines_max_allocations.h\n",
                         dbal_field_print_type_to_string(unit, field_types_info[ii].print_type));
        }
        if (field_types_info[ii].max_size > max_field_size)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Field size for field %s is too long %d, max size of is %d bits\n",
                         field_types_info[ii].name, field_types_info[ii].max_size, max_field_size);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_superset_result_type_field_update(
    int unit,
    multi_res_info_t * superset_res_type,
    dbal_table_field_info_t * field_info)
{
    int kk;
    int superset_buffer_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (kk = 0; kk < (superset_res_type->nof_result_fields); kk++)
    {
        if (superset_res_type->results_info[kk].field_id == field_info->field_id)
        {
            break;
        }
    }

    if (kk == (superset_res_type->nof_result_fields))
    {
        /** incase that the field is a new field in the superset update all the info. */
        superset_res_type->results_info[kk].field_id = field_info->field_id;
        superset_res_type->results_info[kk].field_type = field_info->field_type;
        superset_res_type->results_info[kk].nof_instances = field_info->nof_instances;
        superset_res_type->results_info[kk].field_nof_bits = field_info->field_nof_bits;
        superset_res_type->results_info[kk].max_value = field_info->max_value;
        superset_res_type->results_info[kk].min_value = field_info->min_value;
        superset_res_type->results_info[kk].offset_in_logical_field = field_info->offset_in_logical_field;
        superset_res_type->nof_result_fields++;
    }
    else
    {
        /** incase that the field already exists, just update the max params */
        if (superset_res_type->results_info[kk].field_nof_bits < field_info->field_nof_bits)
        {
            superset_res_type->results_info[kk].field_nof_bits = field_info->field_nof_bits;
        }

        if (superset_res_type->results_info[kk].max_value < field_info->max_value)
        {
            superset_res_type->results_info[kk].max_value = field_info->max_value;
        }

        if (superset_res_type->results_info[kk].min_value > field_info->min_value)
        {
            superset_res_type->results_info[kk].min_value = field_info->min_value;
        }

        if (superset_res_type->results_info[kk].offset_in_logical_field > field_info->offset_in_logical_field)
        {
            superset_res_type->results_info[kk].offset_in_logical_field = field_info->offset_in_logical_field;
        }
    }

        /** update the bits_offset_in_buffer it is related to the prev fields sizes*/
    for (kk = 0; kk < (superset_res_type->nof_result_fields); kk++)
    {
        superset_res_type->results_info[kk].bits_offset_in_buffer = superset_buffer_offset;
        if (superset_res_type->results_info[kk].field_id == DBAL_FIELD_EMPTY)
        {
            break;
        }
        superset_buffer_offset += superset_res_type->results_info[kk].field_nof_bits;
    }

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

    if (SOC_WARM_BOOT(unit))
    {
        if ((table_status == DBAL_TABLE_NOT_INITIALIZED) || (table_status == DBAL_TABLE_HW_ERROR))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "IN WB and Init Not preformed on table %s\n", table->table_name);
        }
    }
    else
    {
        /**  check that table is not already initiated */
        if (table_status > DBAL_TABLE_NOT_INITIALIZED)
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
    if (table->has_result_type && sal_strcasecmp(table->table_name, "ETM_PP_DESCRIPTOR_EXPANSION") != 0)
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
            /** superset tesult type update all the fields that exists in the table   */
            if (table->has_result_type && superset_res_type)
            {
                SHR_IF_ERR_EXIT(dbal_superset_result_type_field_update(unit, superset_res_type, field_info));
            }
        }

        /** keep the max payload size and max num of result fields for all result types */
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
     *  Add zero padding to the end of the buffer
     *  so when flipping the results order (RESULT_TYPE should be in MSB)
     *  RESULT_TYPE will be aligned in buffer.
     *  this is correct only for MDB and KBP
     */
    if ((table->access_method == DBAL_ACCESS_METHOD_MDB) || (table->access_method == DBAL_ACCESS_METHOD_KBP))
    {
        for (qq = 0; qq < table->nof_result_types; qq++)
        {
            multi_res_info_t *result_type_set = &(table->multi_res_info[qq]);
            result_type_set->zero_padding = max_payload_size - result_type_set->entry_payload_size;
        }
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
    int qq, jj;
    int max_sw_payload_size;
    multi_res_info_t *result_type_set;
    SHR_FUNC_INIT_VARS(unit);
    max_sw_payload_size = 0;
    for (qq = 0; qq < table->nof_result_types; qq++)
    {
        result_type_set = &(table->multi_res_info[qq]);
        for (jj = 0; jj < result_type_set->nof_result_fields; jj++)
        {
            result_type_set->results_info[jj].is_sw_field = TRUE;
        }
        max_sw_payload_size = UTILEX_MAX(max_sw_payload_size, BITS2BYTES(result_type_set->entry_payload_size));
    }
    table->sw_payload_length_bytes = max_sw_payload_size;
    if (table->sw_payload_length_bytes > DBAL_PHYSICAL_RES_SIZE_IN_BYTES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s, result length for SW table id %d bytes exceeding the max %u bytes.\n",
                     table->table_name, table->sw_payload_length_bytes, (unsigned) DBAL_PHYSICAL_RES_SIZE_IN_BYTES);
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
    if ((table_status > DBAL_TABLE_NOT_INITIALIZED) && !SOC_WARM_BOOT(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Init alredy preformed on table %s\n", table->table_name);
    }

    for (jj = 0; jj < table->nof_key_fields;)
    {
        int nof_instance;
        uint8 reverse_order;
        field_info = &(table->keys_info[jj]);
        nof_instance = field_info->nof_instances;
        reverse_order = field_info->is_instances_reverse_order;
        if ((nof_instance > 1) && (reverse_order))
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
            uint8 reverse_order;
            field_info = &(table->multi_res_info[qq].results_info[jj]);
            nof_instance = field_info->nof_instances;
            reverse_order = field_info->is_instances_reverse_order;
            if ((nof_instance > 1) && (reverse_order))
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
    int payload_offset = 0;
    SHR_FUNC_INIT_VARS(unit);
    if (table->access_method == DBAL_ACCESS_METHOD_KBP)
    {
#if defined(INCLUDE_KBP)
        uint32 payload_size;
        /** in case of KBP no need the mdb_id + the prefix size is dynamic according to the table updating the table
         *  parameters according to the KBP configurations */
        SHR_IF_ERR_EXIT(dnx_kbp_valid_key_width(unit, table->key_size, &key_width));
        SHR_IF_ERR_EXIT(dnx_kbp_valid_result_width(unit, table->max_payload_size, &payload_size));
        payload_offset = payload_size - table->max_payload_size;
        table->max_payload_size = payload_size;
        key_prefix_size = 0;
        
        if (table_id >= DBAL_NOF_TABLES)
        {
            payload_offset = 0;
        }
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
        bits_offset_in_buffer = result_type_set->zero_padding + payload_offset;
        for (kk = result_type_set->nof_result_fields - 1; kk >= 0; kk--)
        {
            result_type_set->results_info[kk].bits_offset_in_buffer = bits_offset_in_buffer;
            bits_offset_in_buffer += result_type_set->results_info[kk].field_nof_bits;
            if (table->result_type_mapped_to_sw
                && (result_type_set->results_info[kk].field_id == DBAL_FIELD_RESULT_TYPE))
            {
                result_type_set->results_info[kk].is_sw_field = TRUE;
                table->sw_payload_length_bytes = BITS2BYTES(result_type_set->results_info[kk].field_nof_bits);
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

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dbal_db_init_table_tcam_cs_validation(
    int unit,
    dbal_logical_table_t * table)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * The function add some validations and general properties
 * calculations for Hard Logic tables
 */
static shr_error_e
dbal_db_init_table_hl_validation(
    int unit,
    dbal_logical_table_t * table)
{
    int qq, kk, jj, iter;
    int bits_offset_in_sw_payload = 0;
    int field_in_table = 0;
    multi_res_info_t *result_type_set;
    dbal_hl_l2p_field_info_t *curr_l2p_field;
    dbal_tables_e table_id;
    int bits_offset_in_buffer = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (table->table_type == DBAL_TABLE_TYPE_TCAM_DIRECT)
    {
        dbal_hl_l2p_info_t access_info = table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_MEMORY];
        dbal_hl_l2p_info_t register_access_info = table->hl_mapping_multi_res[0].l2p_hl_info[DBAL_HL_ACCESS_REGISTER];
        soc_mem_info_t *meminfo;
        soc_field_info_t *fieldinfo;
        uint32 data_size;
        soc_mem_t memory;
        /** Verify that table is not mapped to a register */
        if (register_access_info.num_of_access_fields != 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "HL TCAM does not support mapping to register. Table: %s\n", table->table_name);
        }

        /*
         * In HL TCAM currently we do not allow conditions
         * Therefore all conditions must be NONE
         */
        for (iter = 0; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
        {
            if (access_info.l2p_fields_info[iter].nof_conditions != 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Table %s (HL TCAM) - conditions are not supported \n", table->table_name);
            }
        }

        /*
         * Assumption is that memory is the same for Key/KeyMask/Result
         */
        memory = access_info.l2p_fields_info[0].memory[0];
        SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get(unit, access_info.l2p_fields_info[0].memory[0],
                                                      &(table->hl_mapping_multi_res[0].is_default_non_standard)));
        for (iter = 1; iter < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ++iter)
        {
            if (memory != access_info.l2p_fields_info[iter].memory[0])
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
            dbal_hl_tcam_access_type_e access_type_id = access_info.l2p_fields_info[iter].hl_tcam_access_type;
            if (access_info.l2p_fields_info[iter].hw_field != INVALIDf)
            {
                /*
                 * Get field len by HW field
                 */
                SOC_FIND_FIELD(access_info.l2p_fields_info[iter].hw_field, meminfo->fields, meminfo->nFields,
                               fieldinfo);
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

    }
    else if (table->table_type == DBAL_TABLE_TYPE_DIRECT)
    {
        /*
         * DBAL_TABLE_TYPE_DIRECT
         */

        table->range_set_supported = 1;
        for (qq = 0; qq < table->nof_result_types; qq++)
        {
            int total_of_of_mapped_fields = 0;
            int is_default_non_standard = 0;
            uint8 is_packed_fields = FALSE;
            result_type_set = &(table->multi_res_info[qq]);
            bits_offset_in_sw_payload = 0;
            table->hl_mapping_multi_res[qq].is_default_non_standard = 0;

            for (kk = 0; kk < DBAL_NOF_HL_ACCESS_TYPES; kk++)
            {

                /** check if is packed fields.
                 *  Used to skip section of validation.  */
                if (!is_packed_fields)
                {
                    is_packed_fields = (table->hl_mapping_multi_res[qq].l2p_hl_info[kk].is_packed_fields);
                }

                /** we support only full packed fields. Full packed fields mean
                 *  that result fields cover the whole register or memory entry.
                 *   To validate it, we'll compare the size of
                 *  all result fields in interface with the size of the
                 *  register or memory entry.
                 *
                 *  we validate only when some fields are defined for this
                 *  access type
                 */
                if (is_packed_fields && table->hl_mapping_multi_res[qq].l2p_hl_info[kk].num_of_access_fields > 0)
                {
                    SHR_IF_ERR_EXIT(dbal_db_init_table_hl_access_packed_fields_validation
                                    (unit, table, kk, result_type_set,
                                     &(table->hl_mapping_multi_res[qq].l2p_hl_info[kk].l2p_fields_info[0])));
                }

                total_of_of_mapped_fields += table->hl_mapping_multi_res[qq].l2p_hl_info[kk].num_of_access_fields;
                for (jj = 0; jj < table->hl_mapping_multi_res[qq].l2p_hl_info[kk].num_of_access_fields; jj++)
                {
                    curr_l2p_field = &table->hl_mapping_multi_res[qq].l2p_hl_info[kk].l2p_fields_info[jj];

                    /** packed fields skip the validation between results and
                     *  mapping. */
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
                            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                         "field %s in L2P not found in table %s as result field, result type %s\n",
                                         dbal_field_to_string(unit, curr_l2p_field->field_id), table->table_name,
                                         result_type_set->result_type_name);
                        }
                        SHR_IF_ERR_EXIT(dbal_logical_table_string_to_id(unit, table->table_name, &table_id));
                        curr_l2p_field->field_pos_in_interface = field_in_table;
                        curr_l2p_field->nof_bits_in_interface = (curr_l2p_field->access_nof_bits != 0) ?
                            curr_l2p_field->access_nof_bits : result_type_set->results_info[field_in_table].
                            field_nof_bits;
                        curr_l2p_field->offset_in_interface =
                            curr_l2p_field->access_offset +
                            result_type_set->results_info[field_in_table].bits_offset_in_buffer;
                    }
                    else
                    {
                        /** we consider only full packed fields. Therefore, we'll
                         *  consider the whole interface size  */
                        curr_l2p_field->offset_in_interface = 0;
                        curr_l2p_field->nof_bits_in_interface = result_type_set->entry_payload_size;
                    }

                    /** check that the field is mapped to a valid memory/register */
                    switch (kk)
                    {
                        case DBAL_HL_ACCESS_MEMORY:
                            if (!SOC_MEM_IS_VALID(unit, curr_l2p_field->memory[0]))
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "The field %s, table %s mapped to invalid memory, result type %s\n",
                                             dbal_field_to_string(unit,
                                                                  result_type_set->
                                                                  results_info[field_in_table].field_id),
                                             table->table_name, result_type_set->result_type_name);
                            }
                            if (curr_l2p_field->alias_memory != INVALIDm)
                            {
                                SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get
                                                (unit, curr_l2p_field->alias_memory, &is_default_non_standard));
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(dnx_init_mem_default_mode_get
                                                (unit, curr_l2p_field->memory[0], &is_default_non_standard));
                            }

                            if (is_default_non_standard)
                            {
                                table->hl_mapping_multi_res[qq].is_default_non_standard = 1;
                            }
                            if ((curr_l2p_field->array_offset_info.formula != NULL)
                                || (curr_l2p_field->entry_offset_info.formula != NULL)
                                || (curr_l2p_field->data_offset_info.formula != NULL)
                                || (curr_l2p_field->block_index_info.formula != NULL)
                                || (curr_l2p_field->alias_data_offset_info.formula != NULL))
                            {
                                table->range_set_supported = 0;
                            }
                            break;
                        case DBAL_HL_ACCESS_REGISTER:
                        {
                            table->range_set_supported = 0;
                            if (!SOC_REG_IS_VALID(unit, curr_l2p_field->reg[0]))
                            {
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "The field %s, table %s mapped to invalid register, result type %s\n",
                                             dbal_field_to_string(unit, result_type_set->results_info[kk].field_id),
                                             table->table_name, result_type_set->result_type_name);
                            }
                            SHR_IF_ERR_EXIT(dbal_hl_reg_default_mode_get(unit, curr_l2p_field->reg[0],
                                                                         &(is_default_non_standard)));
                            if (is_default_non_standard)
                            {
                                table->hl_mapping_multi_res[qq].is_default_non_standard = 1;
                            }
                        }
                            break;
                        case DBAL_HL_ACCESS_SW:
                            table->range_set_supported = 0;
                            result_type_set->results_info[field_in_table].is_sw_field = TRUE;
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
                                         kk, table->table_name);
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
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "table %s not supported aliasing with offset to registers\n",
                                         table->table_name);
                        }

                    }
                }
            }

            /* reverse the fields offsets for packed_fields of multiple results table */
            if (is_packed_fields && table->has_result_type)
            {
                result_type_set = &(table->multi_res_info[qq]);
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
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported table type for hard logic table: %s. Table %s \n",
                     dbal_table_type_to_string(unit, table->table_type), table->table_name);
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

    for (table_id = 0; table_id < DBAL_NOF_TABLES; table_id++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));
        table = &tables_info[table_id];
        /**  check that table is not set as invalid */

        if (SOC_WARM_BOOT(unit))
        {
            
            if ((table->access_method == DBAL_ACCESS_METHOD_KBP) ||
                ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type == DBAL_TABLE_TYPE_LPM)) ||
                ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type == DBAL_TABLE_TYPE_EM)) ||
                ((table->access_method == DBAL_ACCESS_METHOD_MDB) && (table->table_type == DBAL_TABLE_TYPE_TCAM)))
            {
                table->maturity_level = DBAL_MATURITY_PARTIALLY_FUNCTIONAL;
            }

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

        /** per access method validation */
        switch (table->access_method)
        {
            case DBAL_ACCESS_METHOD_MDB:
                SHR_IF_ERR_EXIT(dbal_db_init_table_mdb_validation(unit, table_id, table));
                break;
            case DBAL_ACCESS_METHOD_TCAM_CS:
                SHR_IF_ERR_EXIT(dbal_db_init_table_tcam_cs_validation(unit, table));
                break;
            case DBAL_ACCESS_METHOD_HARD_LOGIC:
                /** for HL if the table has error continue */
                rv = dbal_db_init_table_hl_validation(unit, table);
                if (SHR_FAILURE(rv))
                {
                    SHR_IF_ERR_EXIT(dbal_tables_update_hw_error(unit, table_id));
                    continue;
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
        if (!SOC_WARM_BOOT(unit))
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.set(unit, table_id, DBAL_TABLE_INITIALIZED));
        }
    }

    if (!SOC_WARM_BOOT(unit))
    {
        /*
         * Memories and registers mapping validation Compare memories and registers from groups and directly mapping
         * register\memory that used in a group cannot be used also in direct mapping
         */
        if (check_hw_ent_in_init)
        {
            uint8 finish_iterate = FALSE;
            uint32 hash_entry_idx = 0;
            hw_ent_direct_map_hash_key direct_hash_key;
            uint32 data_indx;
            uint8 found_in_groups = FALSE;
            sal_memset(&direct_hash_key, 0, sizeof(direct_hash_key));
            while (!finish_iterate)
            {
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_direct_map_hash_tbl.get_next
                                (unit, &hash_entry_idx, &direct_hash_key, &data_indx));
                SHR_IF_ERR_EXIT(DBAL_SW_STATE_ACCESS.hw_ent_groups_map_hash_tbl.find
                                (unit, (hw_ent_groups_map_hash_key *) & direct_hash_key, &data_indx, &found_in_groups));
                if (found_in_groups)
                {
                    /*
                     * use array instead of singleton because of coverity ARRAY_VS_SINGLETON
                     */
                    uint32 hw_ent_id[1] = { 0 };
                    SHR_IF_ERR_EXIT(utilex_U8_to_U32((uint8 *) &direct_hash_key, 4, hw_ent_id));
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Found hw %s (%d) that mapped directly and via groups\n",
                                 ((uint8 *) &direct_hash_key)[4] == 1 ? "Memory" : "Register", hw_ent_id[0]);
                }

                if (SW_STATE_HASH_TABLE_ITER_IS_END(&hash_entry_idx))
                {
                    finish_iterate = TRUE;
                }
            }
            check_hw_ent_in_init = FALSE;
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
    char *stage)
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
    sal_strncpy_s(cur_table_param->pipe_stage, stage, sizeof(cur_table_param->pipe_stage));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_hw_entity_group_general_info_set(
    int unit,
    hl_group_info_t * group_info,
    char *name,
    uint32 element_counter,
    uint8 is_reg)
{
    SHR_FUNC_INIT_VARS(unit);

    /** name */
    sal_strncpy_s(group_info->group_name, name, sizeof(group_info->group_name));
    /** count number of element per group */
    group_info->group_hw_entity_num = element_counter;
    /** indicate if group of register or memories */
    group_info->is_reg = is_reg;

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_db_init_hw_entity_group_reg_mem_name_set(
    int unit,
    hl_group_info_t * group_info,
    int element_index,
    char *reg_or_mem_name)
{
    SHR_FUNC_INIT_VARS(unit);
    /** set name for reg/mem */
    sal_strncpy_s(group_info->hw_entity_name[element_index],
                  reg_or_mem_name, sizeof(group_info->hw_entity_name[element_index]));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_db_init_hw_entity_porting_reg_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    char * name,
    char * map_to_name) {

    SHR_FUNC_INIT_VARS(unit);
    /** set name for reg */
    sal_strncpy_s(hl_porting_info->register_name[hl_porting_info->nof_registers],
                  name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    sal_strncpy_s(hl_porting_info->register_mapped_name[hl_porting_info->nof_registers],
                  map_to_name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    hl_porting_info->nof_registers++;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dbal_db_init_hw_entity_porting_mem_mapping_set(
    int unit,
    hl_porting_info_t * hl_porting_info,
    char * name,
    char * map_to_name) {

    SHR_FUNC_INIT_VARS(unit);
    /** set name for mem */
    sal_strncpy_s(hl_porting_info->memory_name[hl_porting_info->nof_memories], name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    sal_strncpy_s(hl_porting_info->memory_mapped_name[hl_porting_info->nof_memories],
                  map_to_name, DBAL_MAX_EXTRA_LONG_STRING_LENGTH);
    hl_porting_info->nof_memories++;
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
