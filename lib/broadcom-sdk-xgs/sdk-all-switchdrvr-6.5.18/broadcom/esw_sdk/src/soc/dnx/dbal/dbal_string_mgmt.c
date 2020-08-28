/**
 * \file dbal_string_mgmt.c
 * $Id$
 *
 * Main functions for dbal strings (names) conversion
 * basically string_to_id and to_string functions
 *
 * Strings mapping array - will be removed in two stages:
 *  1. when auto-coder will be integrated
 *  2. effiecient hashing function
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
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include "dbal_internal.h"

#define DBAL_NOF_ALPHABET_LETTERS                               26
#define DBAL_FIRST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE        97
#define DBAL_FIRST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE        65
#define DBAL_LAST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE         122
#define DBAL_LAST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE         90

/*************
 * STATICS   *
 *************/
/**
 * \brief
 * Default unknown string
 */
static char *unknown_string = "unknown_id";
extern char *dbal_field_strings[DBAL_NOF_FIELDS];
extern char *dbal_label_strings[DBAL_NOF_LABEL_TYPES];
extern char *dbal_field_types_defs_strings[DBAL_NOF_FIELD_TYPES_DEF];
extern char *dbal_table_strings[DBAL_NOF_TABLES];
extern char *dbal_hw_entity_group_strings[DBAL_NOF_HW_ENTITY_GROUPS];

char *dbal_hl_tcam_access_type_strings[DBAL_NOF_HL_TCAM_ACCESS_TYPES] = {
    "KEY",
    "KEY_MASK",
    "RESULT"
};

/** 
 *  \brief
 *  after initiailized holds the start indexes in dbal_field_strings[] of each alphabet letter
 *  dbal_string_field_start_letter_to_index[0] holds the start index of a,A dbal_string_field_start_letter_to_index[1]
 *  holds the start index of b,B and so on.
 *  the last place in the array holds the last array index dbal_field_strings
 *  
 *   */
int dbal_string_field_start_letter_to_index[DBAL_NOF_ALPHABET_LETTERS + 1] = { 0 };

/**
 * \brief
 * Strings mapping array for dbal pyisical tables:
 */
static char *dbal_physical_table_strings[DBAL_NOF_PHYSICAL_TABLES] = {
    "NONE",
    "TCAM",
    "KAPS1",
    "KAPS2",
    "ISEM1",
    "INLIF1",
    "IVSI",
    "ISEM2",
    "ISEM3",
    "INLIF2",
    "INLIF3",
    "LEM",
    "IOEM1",
    "IOEM2",
    "MAP",
    "FEC1",
    "FEC2",
    "FEC3",
    "PPMC",
    "GLEM1",
    "GLEM2",
    "EEDB1",
    "EEDB2",
    "EEDB3",
    "EEDB4",
    "EEDB5",
    "EEDB6",
    "EEDB7",
    "EEDB8",
    "EOEM1",
    "EOEM2",
    "ESEM",
    "EVSI",
    "SEXEM1",
    "SEXEM2",
    "SEXEM3",
    "LEXEM",
    "RMEP_EM",
    "KBP",
};

/**
 * \brief
 * Strings mapping array for dbal core modes:
 */
static char *dbal_core_mode_strings[DBAL_NOF_CORE_MODE_TYPES] = {
    "NONE",
    "DPC",
    "SBC"
};

/**
 * \brief
 * Strings mapping array for dbal condition types:
 */
static char *dbal_condition_strings[DBAL_NOF_CONDITION_TYPES] = {
    "NONE",
    "BIGGER_THAN",
    "LOWER_THAN",
    "EQUAL_TO",
    "NOT_EQUAL_TO",
    "IS_EVEN",
    "IS_ODD"
};

/**
 * \brief
 * Strings mapping array for dbal field types:
 */
static char *dbal_field_print_type_strings[DBAL_NOF_FIELD_PRINT_TYPES] = {
    "NONE",
    "BOOL",
    "UINT32",
    "IPV4",
    "IPV6",
    "MAC",
    "ARRAY8",
    "ARRAY32",
    "BITMAP",
    "ENUM",
    "SYSTEM_CORE",
    "STRUCTURE",
    "HEX",
    "DBAL_TABLE",
    "STR"
};

/**
 * \brief
 * Strings mapping array for dbal encode types:
 * also need the update coreesponding parm1
 * dbal_encode_type_param1_strings
 */
static char *dbal_field_encode_type_strings[DBAL_NOF_VALUE_FIELD_ENCODE_TYPES] = {
    "NONE",
    "PREFIX",
    "SUFFIX",
    "SUBTRACT",
    "ADD",
    "MULTIPLY",
    "DIVIDE",
    "MODULO",
    "HARD_VALUE",
    "BITWISE_NOT",
    "VALID_IND",
    "ENUM",
    "STRUCTURE",
};

/**
 * \brief
 * Strings mapping array for dbal action flags:
 */
static char *dbal_action_flag_strings[DBAL_COMMIT_NOF_OPCODES] = {
    "NORMAL",
    "KEEP_HANDLE",
    "OVERRUN_ENTRY",
    "GET_ALL_FIELDS"
};

/**
 * \brief
 * Strings mapping array for dbal logger type:
 */
static char *dbal_logger_types_strings[DNX_DBAL_LOGGER_TYPE_LAST] = {
    "API",
    "ACCESS",
    "DIAG"
};

/**
 * \brief
 * Strings mapping array for dbal access types:
 */
static char *dbal_access_method_strings[DBAL_NOF_ACCESS_METHODS] = {
    "MDB",
    "TCAM_CS",
    "HARD_LOGIC",
    "SW_STATE",
    "PEMLA",
    "KBP"
};

/**
 * \brief
 * Strings mapping array for dbal table type:
 */
static char *dbal_logical_table_type_strings[DBAL_NOF_TABLE_TYPES] = {
    "NONE",
    "EM",
    "TCAM",
    "TCAM_DIRECT",
    "TCAM_BY_ID",
    "LPM",
    "DIRECT"
};

/**
 * \brief
 * Strings mapping array for dbal table status:
 */
static char *dbal_logical_table_status_strings[DBAL_NOF_TABLE_STATUS] = {
    "Not initialized",
    "Initialized",
    "HW error",
    "Incompatible Image",
};

/**
 * \brief
 * Strings mapping array for dbal maturity levels:
 */
static char *dbal_maturity_level_strings[DBAL_NOF_MATURITY_LEVELS] = {
    "LOW",
    "MEDIUM",
    "HIGH",
    "HIGH_SKIP_CMODEL",
    "HIGH_SKIP_DEVICE"
};

/**
 * \brief
 * Strings mapping array for dbal stages:
 */
static char *dbal_stage_strings[DBAL_NOF_STAGES] = {
    "NONE",
    "PRT",
    "LLR",
    "VT1",
    "VT2",
    "VT3",
    "VT4",
    "VT5",
    "FWD1",
    "FWD2",
    "IPMF1",
    "IPMF2",
    "IPMF3",
    "EPMF"
};

/** 
 * \brief 
 */
static char *dbal_field_permission_strings[DBAL_NOF_PERMISSIONS] = {
    "ALL",
    "READONLY",
    "WRITEONLY",
    "TRIGGER"
};

static char *dbal_hitbit_strings[DBAL_NOF_HITBIT_COMBINATIONS] = {
    "NONE",     /* 0 */
    "PRI_1ST",  /* 1 */
    "PRI_2ND",  /* 2 */
    "PRI_BOTH", /* 3 */
    "SEC_1ST",  /* 4 */
    "PRI_1ST_SEC_1ST",  /* 5 */
    "PRI_2ND_SEC_1ST",  /* 6 */
    "PRI_BOTH_SEC_1ST", /* 7 */
    "SEC_2ND",  /* 8 */
    "PRI_1ST_SEC_2ND",  /* 9 */
    "PRI_2ND_SEC_2ND",  /* 10 */
    "PRI_BOTH_SEC_2ND", /* 11 */
    "SEC_BOTH", /* 12 */
    "PRI_1ST_SEC_BOTH", /* 13 */
    "PRI_2ND_SEC_BOTH", /* 14 */
    "PRI_BOTH_SEC_BOTH",        /* 15 */
};

static char *dbal_fec_hitbit_strings[DBAL_NOF_FEC_HITBIT_COMBINATIONS] = {
    "NONE",     /* 0 */
    "PRI_FWD",  /* 1 */
    "PRI_RPF",  /* 2 */
    "PRI_FWD_RPF",      /* 3 */
    "SEC_RPF",  /* 4 */
    "PRI_FWD_SEC_FWD",  /* 5 */
    "PRI_RPF_SEC_FWD",  /* 6 */
    "PRI_FWD_RPF_SEC_FWD",      /* 7 */
    "SEC_RPF",  /* 8 */
    "PRI_FWD_SEC_RPF",  /* 9 */
    "PRI_RPF_SEC_RPF",  /* 10 */
    "PRI_FWD_RPF_SEC_RPF",      /* 11 */
    "SEC_FWD_RPF",      /* 12 */
    "PRI_FWD_SEC_FWD_RPF",      /* 13 */
    "PRI_RPF_SEC_FWD_RPF",      /* 14 */
    "PRI_FWD_RPF_SEC_FWD_RPF"   /* 15 */
};

static char *dbal_kaps_hitbit_strings[DBAL_NOF_KAPS_HITBIT_COMBINATIONS] = {
    "NONE",     /* 0 */
    "RPF",      /* 1 */
    "FWD",      /* 2 */
    "RPF_FWD"   /* 3 */
};

static char *dbal_lem_hitbit_strings[DBAL_NOF_LEM_HITBIT_COMBINATIONS] = {
    "NONE",     /* 0 */
    "RPForLEARN",       /* 1 */
    "FWD",      /* 2 */
    "RPForLEARN_FWD"    /* 3 */
};

static char *dbal_entry_attr_strings[DBAL_NOF_ENTRY_ATTR_TYPE] = {
    "PRIORITY",
    "AGE",
    "HIT_GET",
    "HIT_CLEAR",
    "PRIMARY",
    "SECONDARY"
};

static char *dbal_result_type_update_mode_strings[DBAL_NOF_RESULT_TYPE_UPDATE_MODES] = {
    "STRICT",
    "LOOSE",
    "LOOSE_DEFAULT"
};


static char *dbal_tcam_hitbit_strings[DBAL_NOF_TCAM_HITBIT_COMBINATIONS] = {
    "NONE",
    "CORE_0",
    "CORE_1",
    "CORE_0_1"
};

/*************
 * FUNCTIONS *
 *************/
shr_error_e
dbal_field_string_to_id(
    int unit,
    char *str,
    dbal_fields_e * field_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dbal_field_string_to_id_no_error(unit, str, field_id))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_field_string_to_id: " "field not found (%s)\n", str);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_flag_string_to_id(
    int unit,
    char *flag_name,
    dbal_entry_action_flags_e * action_flag)
{
    SHR_FUNC_INIT_VARS(unit);

    if (sal_strcasecmp(flag_name, "DBAL_COMMIT") == 0)
    {
        *action_flag = DBAL_COMMIT;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_UPDATE") == 0)
    {
        *action_flag = DBAL_COMMIT_UPDATE;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_FORCE") == 0)
    {
        *action_flag = DBAL_COMMIT_FORCE;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_GET_ALL_FIELDS") == 0)
    {
        *action_flag = DBAL_GET_ALL_FIELDS;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_OVERRUN") == 0)
    {
        *action_flag = DBAL_COMMIT_OVERRUN;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_OVERRIDE_DEFAULT") == 0)
    {
        *action_flag = DBAL_COMMIT_OVERRIDE_DEFAULT;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_DISABLE_ACTION_PRINTS") == 0)
    {
        *action_flag = DBAL_COMMIT_DISABLE_ACTION_PRINTS;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_VALIDATE_OTHER_CORE") == 0)
    {
        *action_flag = DBAL_COMMIT_VALIDATE_OTHER_CORE;
    }
    else if (sal_strcasecmp(flag_name, "DBAL_COMMIT_IGNORE_ALLOC_ERROR") == 0)
    {
        *action_flag = DBAL_COMMIT;
        *action_flag |= DBAL_COMMIT_IGNORE_ALLOC_ERROR;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_flag_string_to_id: " "flag not found (%s)\n", flag_name);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hl_tcam_access_type_string_to_id(
    int unit,
    char *str,
    dbal_hl_tcam_access_type_e * access_type)
{
    dbal_hl_tcam_access_type_e ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_HL_TCAM_ACCESS_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_hl_tcam_access_type_strings[ii], str) == 0)
        {
            *access_type = ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_hl_tcam_access_type_string_to_id: " "access type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

/** \brief
 *  initialize the accelerator for dbal_field_string_to_id, update for string the reletaed offsets in the
 *  dbal_field_strings array.
 */
shr_error_e
dbal_field_string_to_indexes_init(
    int unit)
{
    int field_index, char_index = 0, ascii_val;
    int upper_case_val, lower_case_val;

    SHR_FUNC_INIT_VARS(unit);

    upper_case_val = DBAL_FIRST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE;
    lower_case_val = DBAL_FIRST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE;

    /** running over the string array, starting for the second field since the first one is always EMPTY */
    for (field_index = 1; field_index < DBAL_NOF_FIELDS; field_index++)
    {
        ascii_val = (int) dbal_field_strings[field_index][0];

        if ((ascii_val == upper_case_val) || (ascii_val == lower_case_val))
        {
            dbal_string_field_start_letter_to_index[char_index] = field_index;
            upper_case_val++;
            lower_case_val++;
            char_index++;
            continue;
        }
        /** reached to the last letter 'z' no updating the next position to be DBAL_NOF_FIELDS */
        if (upper_case_val > DBAL_LAST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE)
        {
            dbal_string_field_start_letter_to_index[char_index] = DBAL_NOF_FIELDS;
            break;
        }

        /** this is the case that there is no fields that starts with certain letter, in this case we just need to update
         *  that the next  */
        if (((ascii_val > upper_case_val) && (ascii_val < DBAL_LAST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE)) ||
            ((ascii_val > lower_case_val) && (ascii_val < DBAL_LAST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE)))
        {
            field_index--;
            dbal_string_field_start_letter_to_index[char_index] = field_index;
            upper_case_val++;
            lower_case_val++;
            char_index++;
        }
    }
    dbal_string_field_start_letter_to_index[char_index] = DBAL_NOF_FIELDS;
    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * returns the start and index in the array dbal_field_strings according to the first letter and pre-define 
 * calculations. 
 * 
 * returns _SHR_E_NOT_FOUND if no match
 */
static shr_error_e
dbal_field_string_to_indexs_get(
    int unit,
    char *str,
    int *start_index,
    int *end_index)
{
    int ascii_val;

    SHR_FUNC_INIT_VARS(unit);

    /** taking the ascii value of the first char */
    ascii_val = (int) str[0];

    if ((ascii_val >= DBAL_FIRST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE)
        && (ascii_val <= DBAL_LAST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE))
    {
        /** lower case letters */
        ascii_val = ascii_val - DBAL_FIRST_ENGLISH_LOWER_CASE_LETTER_ASCII_VALUE;
    }
    else if ((ascii_val >= DBAL_FIRST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE)
             && (ascii_val <= DBAL_LAST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE))
    {
        /** UPPER case letters */
        ascii_val = ascii_val - DBAL_FIRST_ENGLISH_UPPER_CASE_LETTER_ASCII_VALUE;
    }
    else
    {
        SHR_IF_ERR_EXIT_NO_MSG(_SHR_E_NOT_FOUND);
    }

    (*start_index) = dbal_string_field_start_letter_to_index[ascii_val];
    (*end_index) = dbal_string_field_start_letter_to_index[ascii_val + 1];

exit:
    SHR_FUNC_EXIT;
}
shr_error_e
dbal_field_string_to_id_no_error(
    int unit,
    char *str,
    dbal_fields_e * field_id)
{
    dbal_fields_e field_index;
    dbal_field_types_defs_e field_type;
    CONST char *field_name;
    int start_index, end_index;

    SHR_FUNC_INIT_VARS(unit);

    DNX_INIT_TIME_ANALYZER_DBAL_INIT_START(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_FIELD_STRING_TO_ID);

    /** handling EMPTY string not aphabetic (in pos 0 in the array)   */
    if (sal_strcasecmp(dbal_field_strings[DBAL_FIELD_EMPTY], str) == 0)
    {
        *field_id = DBAL_FIELD_EMPTY;
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT_NO_MSG(dbal_field_string_to_indexs_get(unit, str, &start_index, &end_index));
    for (field_index = start_index; field_index < end_index; field_index++)
    {
        if (sal_strcasecmp(dbal_field_strings[field_index], str) == 0)
        {
            *field_id = field_index;
            SHR_EXIT();
        }
    }

    for (field_index = 0; field_index < DBAL_NOF_DYNAMIC_FIELDS; field_index++)
    {
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_type.get(unit, field_index, &field_type));
        if (field_type != DBAL_FIELD_TYPE_DEF_EMPTY)
        {
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_FIELD_PROP.field_name.get(unit, field_index, 0, &field_name));
            if (sal_strcasecmp(field_name, str) == 0)
            {
                *field_id = field_index + DBAL_NOF_FIELDS;
                SHR_EXIT();
            }
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
exit:
    DNX_INIT_TIME_ANALYZER_DBAL_INIT_STOP(unit, DNX_INIT_TIME_ANALYSER_DBAL_L_INIT_FIELD_STRING_TO_ID);
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_types_def_string_to_id(
    int unit,
    char *str,
    dbal_field_types_defs_e * field_types_def_id)
{
    dbal_field_types_defs_e ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_TYPES_DEF; ii++)
    {
        if (sal_strcasecmp(dbal_field_types_defs_strings[ii], str) == 0)
        {
            *field_types_def_id = ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_field_types_def_string_to_id: " "field type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_result_type_string_to_id_no_err(
    int unit,
    dbal_tables_e table_id,
    char *res_type_str,
    uint32 *res_type)
{
    CONST dbal_logical_table_t *table;
    int res_type_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    for (res_type_index = 0; res_type_index < table->nof_result_types; res_type_index++)
    {
        if (sal_strcasecmp(table->multi_res_info[res_type_index].result_type_name, res_type_str) == 0)
        {
            *res_type = res_type_index;
            SHR_EXIT();
        }
    }
    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_result_type_string_to_id(
    int unit,
    dbal_tables_e table_id,
    char *res_type_str,
    uint32 *res_type)
{
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_result_type_string_to_id_no_err(unit, table_id, res_type_str, res_type);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "result type not found (%s)\n", res_type_str);
    }
    else if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_enum_type_string_to_id_no_err(
    int unit,
    dbal_field_types_defs_e field_type,
    char *enum_value_string,
    uint32 *enum_value)
{

    int nof_enum_vals;
    int enum_val_index;
    int type_name_size = 0;
    CONST dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));

    nof_enum_vals = field_type_info->nof_enum_values;

    if (!nof_enum_vals)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, " field type is not enum %s\n", field_type_info->name);
    }

    type_name_size = sal_strlen(field_type_info->name) + 1;
    for (enum_val_index = 0; enum_val_index < nof_enum_vals; enum_val_index++)
    {

        if (!sal_strcmp(enum_value_string, &(field_type_info->enum_val_info[enum_val_index].name[type_name_size])))
        {
            (*enum_value) = enum_val_index;
            break;
        }
    }
    if (enum_val_index == nof_enum_vals)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_enum_type_string_to_id(
    int unit,
    dbal_field_types_defs_e field_type,
    char *enum_value_string,
    uint32 *enum_value)
{
    int rv;
    CONST dbal_field_types_basic_info_t *field_type_info;

    SHR_FUNC_INIT_VARS(unit);

    rv = dbal_enum_type_string_to_id_no_err(unit, field_type, enum_value_string, enum_value);
    if (rv == _SHR_E_NOT_FOUND)
    {
        SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, field_type, &field_type_info));
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, " Decoding failed for type %s HW name %s has no logical value\n",
                     field_type_info->name, enum_value_string);
    }
    else if (rv != _SHR_E_NONE)
    {
        SHR_SET_CURRENT_ERR(rv);
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_enum_string_to_id(
    int unit,
    dbal_fields_e field_id,
    char *enum_value_string,
    uint32 *enum_value)
{
    dbal_field_types_defs_e field_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_fields_field_type_get(unit, field_id, &field_type));

    SHR_IF_ERR_EXIT(dbal_enum_type_string_to_id(unit, field_type, enum_value_string, enum_value));

exit:
    SHR_FUNC_EXIT;
}

CONST char *
dbal_enum_id_type_to_string(
    int unit,
    dbal_field_types_defs_e field_type,
    uint32 enum_value)
{
    CONST dbal_field_types_basic_info_t *field_type_info = NULL;
    int type_name_size = 0;
    int nof_enum_vals = 0;
    shr_error_e rv;

    rv = dbal_field_types_info_get(unit, field_type, &field_type_info);
    if (rv)
    {
        return unknown_string;
    }

    if (field_type_info)
    {
        nof_enum_vals = field_type_info->nof_enum_values;
    }

    if (!nof_enum_vals)
    {
        return unknown_string;
    }

    type_name_size = sal_strlen(field_type_info->name) + 1;

    return (&field_type_info->enum_val_info[enum_value].name[type_name_size]);
}

CONST char *
dbal_enum_id_to_string(
    int unit,
    dbal_fields_e field_id,
    uint32 enum_value)
{
    dbal_field_types_defs_e field_type;
    shr_error_e rv;

    rv = dbal_fields_field_type_get(unit, field_id, &field_type);
    if (rv)
    {
        return unknown_string;
    }

    return dbal_enum_id_type_to_string(unit, field_type, enum_value);
}

shr_error_e
dbal_logical_table_string_to_id_no_error(
    int unit,
    char *str,
    dbal_tables_e * log_table_id)
{
    int table_index;
    int nof_tables = dnx_data_dbal.table.nof_dynamic_tables_get(unit);
    int nof_xml_dynamic_tables = dnx_data_dbal.table.nof_dynamic_xml_tables_get(unit);
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    for (table_index = 0; table_index < DBAL_NOF_STATIC_TABLES; table_index++)
    {
        if ((dbal_table_strings[table_index]) && (sal_strcasecmp(dbal_table_strings[table_index], str) == 0))
        {
            *log_table_id = (dbal_tables_e) table_index;
            SHR_EXIT();
        }
    }

    if (DBAL_DYNAMIC_XML_TABLES_ENABLED)
    {
        /**Iterate over dynamic xml tables*/
        for (table_index = 0; table_index < nof_xml_dynamic_tables; table_index++)
        {
            CONST char *table_name;
            SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.
                            table_status.get(unit, (DBAL_NOF_STATIC_TABLES + table_index), &table_status));
            if (table_status != DBAL_TABLE_NOT_INITIALIZED)
            {
                SHR_IF_ERR_EXIT(DBAL_DYNAMIC_XML_TBL_IN_SW_STATE.table_name.get(unit, table_index, 0, &table_name));
                if (sal_strcasecmp(table_name, str) == 0)
                {
                    *log_table_id = (dbal_tables_e) (table_index + DBAL_NOF_STATIC_TABLES);
                    SHR_EXIT();
                }
            }
        }
    }

    /**Iterate over dynamic tables added by APIs (field tables)*/
    for (table_index = 0; table_index < nof_tables; table_index++)
    {
        CONST char *table_name;
        SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, (DBAL_NOF_TABLES + table_index), &table_status));
        if (table_status != DBAL_TABLE_NOT_INITIALIZED)
        {
            SHR_IF_ERR_EXIT(DBAL_DYNAMIC_TBL_IN_SW_STATE.table_name.get(unit, table_index, 0, &table_name));
            if (sal_strcasecmp(table_name, str) == 0)
            {
                *log_table_id = (dbal_tables_e) (table_index + DBAL_NOF_TABLES);
                SHR_EXIT();
            }
        }
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_table_string_to_id(
    int unit,
    char *str,
    dbal_tables_e * log_table_id)
{

    SHR_FUNC_INIT_VARS(unit);

    if (dbal_logical_table_string_to_id_no_error(unit, str, log_table_id) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "table not found related to string %s\n", str);
    }
exit:
    SHR_FUNC_EXIT;
}

#if 0
/** INIT_PERFORMANCE_IMPROVEMENTS we can use this struct to improve the init by finding the renges of the first char */
shr_error_e
dbal_hw_entity_string_to_id(
    int unit,
    char *str,
    dbal_hl_entity_type_e entity_type,
    int *hw_entity_id)
{
    int ii;
    char *str_to_check = str;

    SHR_FUNC_INIT_VARS(unit);

    if (entity_type == DBAL_HL_ENTITY_REGISTER)
    {
        /** strip the last charcter "r" or "m"  */
        str_to_check[sal_strlen(str_to_check) - 1] = 0;
        for (ii = 0; ii < NUM_SOC_REG; ii++)
        {
            if (SOC_REG_IS_VALID(unit, ii))
            {
                if (sal_strcasecmp(SOC_REG_NAME(unit, ii), str_to_check) == 0)
                {
                    *hw_entity_id = ii;
                    SHR_EXIT();
                }
            }
        }
    }

    if (entity_type == DBAL_HL_ENTITY_MEMORY)
    {
        /** strip the last charcter "r" or "m"  */
        str_to_check[sal_strlen(str_to_check) - 1] = 0;
        for (ii = 0; ii < NUM_SOC_MEM; ii++)
        {
            if (SOC_MEM_IS_VALID(unit, ii))
            {
                if (sal_strcasecmp(SOC_MEM_NAME(unit, ii), str_to_check) == 0)
                {
                    *hw_entity_id = ii;
                    SHR_EXIT();
                }
            }
        }
    }

    if (entity_type == DBAL_HL_ENTITY_FIELD)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "not supported for fields, use dbal_hw_field_form_reg_string_to_id \n");
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);

    if (DBAL_IS_J2C_A0)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "dbal_hw_entities_string_to_id: " "hw entity not found (%s) (mapped to %s)\n"),
                     str, str_to_check));
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "dbal_hw_entities_string_to_id: " "hw entity not found (%s) (mapped to %s)\n"), str,
                   str_to_check));
    }
exit:
    SHR_FUNC_EXIT;
}

/** INIT_PERFORMANCE_IMPROVEMENTS we can improve performance of the init by finding the ranges of the first char */
shr_error_e
dbal_hw_field_form_reg_string_to_id(
    int unit,
    char *str,
    soc_reg_t reg,
    int *hw_entity_id)
{
    int ii;
    char *str_to_check = str;

    SHR_FUNC_INIT_VARS(unit);

    str_to_check[sal_strlen(str_to_check) - 1] = 0;

    for (ii = 0; ii < SOC_REG_INFO(unit, reg).nFields; ii++)
    {
        soc_field_info_t *hw_field = &(SOC_REG_INFO(unit, reg).fields[ii]);
        if (sal_strcasecmp(SOC_FIELD_NAME(unit, hw_field->field), str_to_check) == 0)
        {
            *hw_entity_id = hw_field->field;
            SHR_EXIT();
        }
    }

    if (DBAL_IS_J2C_A0)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "field %s not found in register  %s.\n\n"), str, SOC_REG_NAME(unit, reg)));
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "field %s not found in register  %s.\n\n"), str, SOC_REG_NAME(unit, reg)));

    }
    (*hw_entity_id) = -1;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hw_field_form_mem_string_to_id(
    int unit,
    char *str,
    soc_mem_t mem,
    int *hw_entity_id)
{
    int ii;
    char *str_to_check = str;

    SHR_FUNC_INIT_VARS(unit);

    str_to_check[sal_strlen(str_to_check) - 1] = 0;

    for (ii = 0; ii < SOC_MEM_INFO(unit, mem).nFields; ii++)
    {
        soc_field_info_t *hw_field = &(SOC_MEM_INFO(unit, mem).fields[ii]);
        if (sal_strcasecmp(SOC_FIELD_NAME(unit, hw_field->field), str) == 0)
        {
            *hw_entity_id = hw_field->field;
            SHR_EXIT();
        }
    }

    /** should return an error, currently we have maturity level one tables that fails here.. */
    if (DBAL_IS_J2C_A0)
    {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit, "field %s not found in memory %s.\n\n"), str, SOC_MEM_NAME(unit, mem)));
    }
    else
    {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "field %s not found in memory %s.\n\n"), str, SOC_MEM_NAME(unit, mem)));

    }
    (*hw_entity_id) = -1;

exit:
    SHR_FUNC_EXIT;
}

#endif

shr_error_e
dbal_physical_table_string_to_id(
    int unit,
    char *str,
    dbal_physical_tables_e * phy_table_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_PHYSICAL_TABLES; ii++)
    {
        if (sal_strcasecmp(dbal_physical_table_strings[ii], str) == 0)
        {
            *phy_table_id = (dbal_physical_tables_e) ii;
            SHR_EXIT();
        }
    }
    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_physical_table_string_to_id: " "phy table not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_label_string_to_id(
    int unit,
    char *str,
    dbal_labels_e * label_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_LABEL_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_label_strings[ii], str) == 0)
        {
            *label_id = (dbal_labels_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_label_string_to_id: " "label not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_core_mode_string_to_id(
    int unit,
    char *str,
    dbal_core_mode_e * core_mode_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_CORE_MODE_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_core_mode_strings[ii], str) == 0)
        {
            *core_mode_id = (dbal_core_mode_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_core_mode_string_to_id: " "core mode not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_condition_string_to_id(
    int unit,
    char *str,
    dbal_condition_types_e * condition_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_CONDITION_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_condition_strings[ii], str) == 0)
        {
            *condition_id = (dbal_condition_types_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_condition_string_to_id: " "condition type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_print_type_string_to_id(
    int unit,
    char *str,
    dbal_field_print_type_e * field_print_type_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_FIELD_PRINT_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_field_print_type_strings[ii], str) == 0)
        {
            *field_print_type_id = (dbal_field_print_type_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_field_print_type_string_to_id: " "field print type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_field_encode_type_string_to_id(
    int unit,
    char *str,
    dbal_value_field_encode_types_e * encode_type_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_VALUE_FIELD_ENCODE_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_field_encode_type_strings[ii], str) == 0)
        {
            *encode_type_id = (dbal_value_field_encode_types_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_field_encode_type_string_to_id: " "encode type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_logical_table_type_string_to_id(
    int unit,
    char *str,
    dbal_table_type_e * table_type_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLE_TYPES; ii++)
    {
        if (sal_strcasecmp(dbal_logical_table_type_strings[ii], str) == 0)
        {
            *table_type_id = (dbal_table_type_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_logical_table_type_string_to_id: " "table type not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_access_method_string_to_id(
    int unit,
    char *str,
    dbal_access_method_e * access_method_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_ACCESS_METHODS; ii++)
    {
        if (sal_strcasecmp(dbal_access_method_strings[ii], str) == 0)
        {
            *access_method_id = (dbal_access_method_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_access_method_string_to_id: " "access_method not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_maturity_level_string_to_id(
    int unit,
    char *str,
    dbal_maturity_level_e * maturity_level_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_MATURITY_LEVELS; ii++)
    {
        if (sal_strcasecmp(dbal_maturity_level_strings[ii], str) == 0)
        {
            *maturity_level_id = (dbal_maturity_level_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_maturity_level_string_to_id: " "maturity level not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_stage_string_to_id(
    int unit,
    char *str,
    dbal_stage_e * dbal_stage)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_STAGES; ii++)
    {
        if (sal_strcasecmp(dbal_stage_strings[ii], str) == 0)
        {
            *dbal_stage = (dbal_stage_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "dbal_stage_string_to_id: " "dbal stage not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_table_status_string_to_id(
    int unit,
    char *str,
    dbal_table_status_e * table_status_id)
{
    int ii;

    SHR_FUNC_INIT_VARS(unit);

    for (ii = 0; ii < DBAL_NOF_TABLE_STATUS; ii++)
    {
        if (sal_strcasecmp(dbal_logical_table_status_strings[ii], str) == 0)
        {
            *table_status_id = (dbal_table_status_e) ii;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, " table status not found (%s)\n", str);
exit:
    SHR_FUNC_EXIT;
}

CONST char *
dbal_field_to_string(
    int unit,
    dbal_fields_e field_id)
{
    CONST char *field_name;

    if (field_id < DBAL_NOF_FIELDS)
    {
        return dbal_field_strings[field_id];
    }
    else if (field_id < DBAL_NOF_FIELDS + DBAL_NOF_DYNAMIC_FIELDS)
    {
        DBAL_SW_STATE_FIELD_PROP.field_name.get(unit, (field_id - DBAL_NOF_FIELDS), 0, &field_name);
        return field_name;
    }

    return unknown_string;
}

CONST char *
dbal_result_type_to_string(
    int unit,
    dbal_tables_e table_id,
    int result_type)
{
    int rv;
    CONST dbal_logical_table_t *table;
    rv = dbal_tables_table_get(unit, table_id, &table);
    if (rv)
    {
        return unknown_string;
    }

    if (result_type < 0 || result_type >= table->nof_result_types)
    {
        return unknown_string;
    }
    return table->multi_res_info[result_type].result_type_name;
}

char *
dbal_hl_tcam_access_type_to_string(
    int unit,
    dbal_hl_tcam_access_type_e hl_tcam_access_type)
{
    if (hl_tcam_access_type < DBAL_NOF_HL_TCAM_ACCESS_TYPES)
    {
        return dbal_hl_tcam_access_type_strings[hl_tcam_access_type];
    }
    return unknown_string;
}

char *
dbal_field_types_def_to_string(
    int unit,
    dbal_field_types_defs_e field_types_def_id)
{
    if (field_types_def_id < DBAL_NOF_FIELD_TYPES_DEF)
    {
        return dbal_field_types_defs_strings[field_types_def_id];
    }
    return unknown_string;
}

char *
dbal_logical_table_to_string(
    int unit,
    dbal_tables_e table_id)
{
    if (table_id < DBAL_NOF_STATIC_TABLES)
    {
        return dbal_table_strings[table_id];
    }
    else if (table_id < dnx_data_dbal.table.nof_dynamic_and_static_tables_get(unit))
    {
        CONST dbal_logical_table_t *table;
        int rv;

        rv = dbal_tables_table_get(unit, table_id, &table);
        if (rv)
        {
            return unknown_string;
        }
        return (char *) table->table_name;
    }
    return unknown_string;
}

char *
dbal_logger_type_to_string(
    int unit,
    dnx_dbal_logger_type_e dbal_logger_type)
{
    if (dbal_logger_type < DNX_DBAL_LOGGER_TYPE_LAST)
    {
        return dbal_logger_types_strings[dbal_logger_type];
    }
    return unknown_string;
}

char *
dbal_physical_table_to_string(
    int unit,
    dbal_physical_tables_e phy_table_id)
{
    if (phy_table_id < DBAL_NOF_PHYSICAL_TABLES)
    {
        return dbal_physical_table_strings[phy_table_id];
    }
    return unknown_string;
}

char *
dbal_label_to_string(
    int unit,
    dbal_labels_e label_id)
{
    if (label_id < DBAL_NOF_LABEL_TYPES)
    {
        return dbal_label_strings[label_id];
    }

    return unknown_string;
}

char *
dbal_mdb_hitbit_to_string(
    int unit,
    dbal_physical_tables_e physical_db_id,
    uint8 hitbit)
{
    switch (physical_db_id)
    {
        case DBAL_PHYSICAL_TABLE_KAPS_1:
        case DBAL_PHYSICAL_TABLE_KAPS_2:
        {
            if (hitbit < DBAL_NOF_KAPS_HITBIT_COMBINATIONS)
            {
                return dbal_kaps_hitbit_strings[(hitbit & 0x3)];
            }
            break;
        }
        case DBAL_PHYSICAL_TABLE_LEM:
        {
            if (hitbit < DBAL_NOF_LEM_HITBIT_COMBINATIONS)
            {
                return dbal_lem_hitbit_strings[(hitbit & 0x3)];
            }
            break;
        }
        case DBAL_PHYSICAL_TABLE_FEC_1:
        case DBAL_PHYSICAL_TABLE_FEC_2:
        case DBAL_PHYSICAL_TABLE_FEC_3:
        {
            if (hitbit < DBAL_NOF_FEC_HITBIT_COMBINATIONS)
            {
                return dbal_fec_hitbit_strings[(hitbit & 0xF)];
            }
            break;
        }
        case DBAL_PHYSICAL_TABLE_TCAM:
        {
            if (hitbit < DBAL_NOF_TCAM_HITBIT_COMBINATIONS)
            {
                return dbal_tcam_hitbit_strings[hitbit];
            }
            break;
        }
        default:
        {
            break;
        }
    }

    if (hitbit < DBAL_NOF_HITBIT_COMBINATIONS)
    {
        return dbal_hitbit_strings[(hitbit & 0xF)];
    }

    return unknown_string;
}

char *
dbal_entry_attr_to_string(
    int unit,
    uint32 attr_type)
{
    if (attr_type < DBAL_NOF_ENTRY_ATTR_TYPE)
    {
        return dbal_entry_attr_strings[attr_type];
    }

    return unknown_string;
}

char *
dbal_core_mode_to_string(
    int unit,
    dbal_core_mode_e core_mode_id)
{
    if (core_mode_id < DBAL_NOF_CORE_MODE_TYPES)
    {
        return dbal_core_mode_strings[core_mode_id];
    }
    return unknown_string;
}

char *
dbal_condition_to_string(
    int unit,
    dbal_condition_types_e condition_id)
{
    if (condition_id < DBAL_NOF_CONDITION_TYPES)
    {
        return dbal_condition_strings[condition_id];
    }
    return unknown_string;
}

char *
dbal_field_print_type_to_string(
    int unit,
    dbal_field_print_type_e field_print_type_id)
{
    if (field_print_type_id < DBAL_NOF_FIELD_PRINT_TYPES)
    {
        return dbal_field_print_type_strings[field_print_type_id];
    }
    return unknown_string;
}

char *
dbal_field_encode_type_to_string(
    int unit,
    dbal_value_field_encode_types_e encode_type_id)
{
    if (encode_type_id < DBAL_NOF_VALUE_FIELD_ENCODE_TYPES)
    {
        return dbal_field_encode_type_strings[encode_type_id];
    }
    return unknown_string;
}

char *
dbal_action_flags_to_string(
    int unit,
    dbal_entry_action_flags_e action_flag_id)
{
    if (action_flag_id < DBAL_COMMIT_NOF_OPCODES)
    {
        return dbal_action_flag_strings[action_flag_id];
    }
    return unknown_string;
}

char *
dbal_access_method_to_string(
    int unit,
    dbal_access_method_e access_method_id)
{
    if (access_method_id < DBAL_NOF_ACCESS_METHODS)
    {
        return dbal_access_method_strings[access_method_id];
    }
    return unknown_string;
}

char *
dbal_table_status_to_string(
    int unit,
    dbal_table_status_e table_status_id)
{
    if (table_status_id < DBAL_NOF_TABLE_STATUS)
    {
        return dbal_logical_table_status_strings[table_status_id];
    }
    return unknown_string;
}

char *
dbal_table_type_to_string(
    int unit,
    dbal_table_type_e table_type_id)
{
    if (table_type_id < DBAL_NOF_TABLE_TYPES)
    {
        return dbal_logical_table_type_strings[table_type_id];
    }
    return unknown_string;
}

char *
dbal_maturity_level_to_string(
    int unit,
    dbal_maturity_level_e maturity_level_id)
{
    if (maturity_level_id < DBAL_NOF_MATURITY_LEVELS)
    {
        return dbal_maturity_level_strings[maturity_level_id];
    }
    return unknown_string;
}

char *
dbal_stage_to_string(
    int unit,
    dbal_stage_e dbal_stage)
{
    if (dbal_stage < DBAL_NOF_STAGES)
    {
        return dbal_stage_strings[dbal_stage];
    }
    return unknown_string;
}

char *
dbal_field_permission_to_string(
    int unit,
    dbal_field_permission_e permission_id)
{
    if (permission_id < DBAL_NOF_PERMISSIONS)
    {
        return dbal_field_permission_strings[permission_id];
    }
    return unknown_string;
}

char *
dbal_result_type_update_mode_to_string(
    int unit,
    dbal_result_type_update_mode_e mode)
{
    if (mode < DBAL_NOF_RESULT_TYPE_UPDATE_MODES)
    {
        return dbal_result_type_update_mode_strings[mode];
    }
    return unknown_string;
}

static void
dbal_lables_string_build(
    int unit,
    int nof_labels,
    CONST dbal_labels_e * labels,
    char *str)
{
    int ii, is_label_printed = 0, offset = 0;

    sal_snprintf(str, DBAL_MAX_NOF_ENTITY_LABEL_TYPES * DBAL_MAX_SHORT_STRING_LENGTH, "%s", " NONE");

    /** support params print for conditions */
    for (ii = 0; ii < nof_labels; ii++)
    {
        if (labels[ii] != DBAL_LABEL_NONE)
        {
            if (is_label_printed == 1)
            {
                offset =
                    sal_snprintf(str, DBAL_MAX_SHORT_STRING_LENGTH, ", %s", dbal_label_to_string(unit, labels[ii]));
            }
            else
            {
                offset = sal_snprintf(str, DBAL_MAX_SHORT_STRING_LENGTH, "%s", dbal_label_to_string(unit, labels[ii]));
                is_label_printed = 1;
            }
            str += offset;
        }
    }
}

shr_error_e
dbal_full_table_to_string(
    int unit,
    dbal_tables_e table_id,
    dbal_table_string_t * table_string)
{
    CONST dbal_logical_table_t *table;
    char *str_to_print;
    dbal_table_status_e table_status;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    SHR_IF_ERR_EXIT(DBAL_SW_STATE_TBL_PROP.table_status.get(unit, table_id, &table_status));

    dbal_lables_string_build(unit, table->nof_labels, table->table_labels, table_string->labels);
    sal_strcpy(table_string->table_name, table->table_name);

    /** table status */
    str_to_print = dbal_table_status_to_string(unit, table_status);
    if (sal_strlen(str_to_print) > DBAL_MAX_SHORT_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n string too big table status ");
    }
    sal_strcpy(table_string->table_status, str_to_print);

    /** maturity_level*/
    str_to_print = dbal_maturity_level_to_string(unit, table->maturity_level);
    if (sal_strlen(str_to_print) > DBAL_MAX_SHORT_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n string too big maturity_level ");
    }
    sal_strcpy(table_string->maturity_level, str_to_print);

   /** table_type*/
    str_to_print = dbal_table_type_to_string(unit, table->table_type);
    if (sal_strlen(str_to_print) > DBAL_MAX_SHORT_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n string too big table_type ");
    }
    sal_strcpy(table_string->table_type, str_to_print);

    /** access_method*/
    str_to_print = dbal_access_method_to_string(unit, table->access_method);
    if (sal_strlen(str_to_print) > DBAL_MAX_SHORT_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n string too big access_method ");
    }
    sal_strcpy(table_string->access_method, str_to_print);

    if (table->maturity_level == DBAL_MATURITY_LOW)
    {
        /** in this case only basic parameters are dummped */
        sal_strcpy(table_string->core_mode, EMPTY);
        sal_strcpy(table_string->range_set_supported, EMPTY);
        SHR_EXIT();
    }

    /** core_mode*/
    str_to_print = dbal_core_mode_to_string(unit, table->core_mode);
    if (sal_strlen(str_to_print) > DBAL_MAX_SHORT_STRING_LENGTH)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "\n string too big core_mode ");
    }
    sal_strcpy(table_string->core_mode, str_to_print);

    if (SHR_IS_BITSET(table->indications_bm, DBAL_TABLE_IND_RANGE_SET_SUPPORTED))
    {
        sal_strcpy(table_string->range_set_supported, "Bulk mode range supported");
    }
    else
    {
        sal_strcpy(table_string->range_set_supported, "Bulk mode range NOT supported");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbal_hw_entity_group_string_to_id(
    int unit,
    char *str,
    dbal_hw_entity_group_e * group_id)
{
    int group_index = DBAL_HW_ENTITY_GROUP_EMPTY;

    SHR_FUNC_INIT_VARS(unit);

    while (group_index < DBAL_NOF_HW_ENTITY_GROUPS)
    {
        if (sal_strcasecmp(dbal_hw_entity_group_strings[group_index], str) == 0)
        {
            *group_id = (dbal_hw_entity_group_e) group_index;
            SHR_EXIT();
        }
        group_index++;
    }

    SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
exit:
    SHR_FUNC_EXIT;
}

char *
dbal_hw_entity_group_to_string(
    int unit,
    dbal_hw_entity_group_e hw_entity_group_id)
{
    return dbal_hw_entity_group_strings[hw_entity_group_id];
}


