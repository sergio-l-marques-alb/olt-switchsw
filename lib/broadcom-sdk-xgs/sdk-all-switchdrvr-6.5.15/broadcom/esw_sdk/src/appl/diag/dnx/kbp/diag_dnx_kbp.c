
/*
 * $Id: diag_sand_dsig.c,v 1.00 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:    diag_sand_dsig.c
 * Purpose:    Routines for handling debug and internal signals
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_KBP_DIAG

#if defined(INCLUDE_KBP)

#include <sal/appl/sal.h>

#include <soc/dnx/kbp/kbp_common.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/sand/sand_signals.h>
#include <soc/drv.h>

#include <bcm/types.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_rop.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_signals.h>

#include "diag_dnx_kbp.h"

/*
 * Globals
 * {
 */
extern shr_error_e kbp_mngr_context_to_opcode_translate(
    int unit,
    uint8 fwd_context,
    uint8 acl_context,
    uint8 *opcode_id);

extern shr_error_e kbp_mngr_opcode_printable_entry_result_parsing(
    int unit,
    uint32 flags,
    uint8 opcode_id,
    uint32 *res_sig_value,
    int res_sig_size,
    uint8 *kbp_entry_print_num,
    kbp_printable_entry_t * entry_print_info);

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

/*
 * }
 */

/*
 * Defines
 * {
 */
/** \brief Long string max length */
#define DNX_KBP_DIAG_MAX_STRING_LENGTH          1024

#define DNX_KBP_DIAG_MAX_NOF_DB                 (64)

#define DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE 40

sh_sand_enum_t dnx_kbp_diag_opcode_dump_enum_table[] = {
    {"ALL", DNX_KBP_MAX_NOF_OPCODES, "All opcodes"},
    {NULL}
};

/*
 * }
 */

/**
 * \brief - callback to check if KBP interface is enabled.
 */
static shr_error_e
dnx_kbp_diag_interface_is_enabled(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
    }
    else
    {   /** one of supported KBP devices must be defined */
        char *kbp_device_type = soc_property_get_str(unit, "ext_tcam_dev_type");
        if ((kbp_device_type == NULL) ||
            (sal_strcasecmp(kbp_device_type, "BCM52311") && sal_strcasecmp(kbp_device_type, "BCM52321")))
        {
            SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        }
        else
        {   /** if kbp_blackhole defined disable command */
            char *kbp_blackhole = soc_property_get_str(unit, "custom_feature_kbp_blackhole");
            if ((kbp_blackhole != NULL) && !sal_strcasecmp(kbp_device_type, "1"))
            {
                SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
            }
        }
    }
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_print_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_print_html(Kbp_app_data[unit]->device_p, kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP device is not locked\n");
    }

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_dump(Kbp_app_data[unit]->device_p, TRUE, kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_device_swstate_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    SHR_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP device is not locked\n");
    }

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_print_sw_state(Kbp_app_data[unit]->device_p, kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)))
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_sdk_ver_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    const char *ver_str = NULL;

    SHR_FUNC_INIT_VARS(unit);

    ver_str = kbp_device_get_sdk_version();

    if (ver_str != NULL)
    {
        bsl_printf("%s\n", ver_str);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error getting KBPSDK version\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_printable_master_key_build(
    int unit,
    uint32 entry_handle_id,
    char master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE])
{
    int index;
    uint32 nof_segments;
    uint32 key_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(master_key, 0x0,
               (sizeof(char) * DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

    for (index = 0; index < nof_segments; index++)
    {
        uint32 segment_type, segment_bytes;
        uint32 segment_name_u32[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS] = { 0 };
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        char master_key_segment[DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
        char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
        int meaningful_segment_type_string_offset = sal_strlen("KBP_KEY_FIELD_TYPE_");

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_MASTER_KEY, index, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_MASTER_KEY, DBAL_FIELD_SEGMENT_NAME, segment_name_u32, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_MASTER_KEY, DBAL_FIELD_SEGMENT_SIZE, &segment_bytes, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_MASTER_KEY, DBAL_FIELD_SEGMENT_TYPE, &segment_type, key_segment[index]));

        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (segment_name_u32, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, (uint8 *) segment_name));

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                        (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

        sal_snprintf(master_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "<%s %d %s> ", segment_name,
                     segment_bytes, (segment_type_string + meaningful_segment_type_string_offset));

        sal_strncat(master_key, master_key_segment,
                    (DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_printable_db_key_build(
    int unit,
    uint32 entry_handle_id,
    char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE])
{
    int index;
    uint32 nof_segments;
    uint32 key_segment[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP][DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { {0} };

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(db_key, 0x0,
               (sizeof(char) * DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_NOF_VALID_SEGMENTS, INST_SINGLE, &nof_segments));

    for (index = 0; index < nof_segments; index++)
    {
        uint32 segment_type, segment_bytes;
        uint32 segment_name_u32[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_WORDS] = { 0 };
        char segment_name[DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES] = { 0 };
        char db_key_segment[DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
        char segment_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
        int meaningful_segment_type_string_offset = sal_strlen("KBP_KEY_FIELD_TYPE_");

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_DB_KEY, index, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_DB_KEY, DBAL_FIELD_SEGMENT_NAME, segment_name_u32, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_DB_KEY, DBAL_FIELD_SEGMENT_SIZE, &segment_bytes, key_segment[index]));

        SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                        (unit, DBAL_FIELD_DB_KEY, DBAL_FIELD_SEGMENT_TYPE, &segment_type, key_segment[index]));

        SHR_IF_ERR_EXIT(utilex_U32_to_U8
                        (segment_name_u32, DNX_KBP_KEY_SEGMENT_NAME_SIZE_IN_BYTES, (uint8 *) segment_name));

        SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                        (unit, "SEGMENT_TYPE", segment_type, segment_type_string));

        sal_snprintf(db_key_segment, (DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1), "<%s %d %s> ", segment_name,
                     segment_bytes, (segment_type_string + meaningful_segment_type_string_offset));

        sal_strncat(db_key, db_key_segment,
                    (DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE - 1));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_opcode_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int index;
    uint32 entry_handle_id;
    uint32 db_entry_handle_id;
    uint32 lkp_entry_handle_id;
    uint32 res_entry_handle_id;
    uint32 ctx_entry_handle_id;
    uint32 lookup_bmp = 0;
    uint32 result_bmp = 0;
    uint32 opcode_id;
    uint8 is_valid;
    uint8 print_one;
    int is_end;
    char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    char master_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_MASTER_KEY * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
    uint32 total_result_bits = 0;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("OPcode", opcode_id);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_LOOKUP_INFO, &lkp_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_RESULT_INFO, &res_entry_handle_id));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_MAPPING, &ctx_entry_handle_id));

    if (opcode_id < DNX_KBP_MAX_NOF_OPCODES)
    {
        /** Print the given opcode */
        dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

        if (!is_valid)
        {
            bsl_printf("Opcode %d is not in use\n", opcode_id);
            SHR_EXIT();
        }

        print_one = TRUE;
        is_end = FALSE;
    }
    else if (opcode_id == DNX_KBP_MAX_NOF_OPCODES)
    {
        /** Initialize iterator to iterate over all opcodes */
        is_valid = TRUE;
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                         &is_valid));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

        print_one = FALSE;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Opcode %d is out of range\n", opcode_id);
    }

    while (!is_end)
    {
        int context_is_end = TRUE;
        uint8 contexts_attached = FALSE;

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_LOOKUP_BITMAP, INST_SINGLE, &lookup_bmp));

        PRT_TITLE_SET("KBP Opcode %d (%s)", opcode_id, opcode_name);
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Master key");
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
        if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
        {
            PRT_INFO_ADD("WARNING: KBP device is not locked");
        }

        SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_master_key_build(unit, entry_handle_id, master_key));
        PRT_CELL_SET("%s", master_key);
        PRT_COMMITX;

        PRT_TITLE_SET("Contexts");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FWD context");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ACL context");

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, ctx_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, ctx_entry_handle_id, &context_is_end));

        while (!context_is_end)
        {
            uint32 mapped_opcode_id = -1;

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, ctx_entry_handle_id, DBAL_FIELD_KBP_OPCODE_ID, INST_SINGLE, &mapped_opcode_id));

            if (opcode_id == mapped_opcode_id)
            {
                uint32 context_id;
                char context_string[DBAL_MAX_STRING_LENGTH] = { 0 };

                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, ctx_entry_handle_id, DBAL_FIELD_FWD2_CONTEXT_ID,
                                &context_id));
                SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                                (unit, "FWD2_CONTEXT_ID", context_id, context_string));
                PRT_CELL_SET("%s (%d)", context_string, context_id);

                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, ctx_entry_handle_id, DBAL_FIELD_ACL_CONTEXT,
                                &context_id));
                SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                                (unit, "ACL_CONTEXT", context_id, context_string));
                PRT_CELL_SET("%s (%d)", context_string, context_id);

                contexts_attached = TRUE;
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, ctx_entry_handle_id, &context_is_end));
        }

        if (!contexts_attached)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("None");
            PRT_CELL_SET("None");
        }

        PRT_COMMITX;

        PRT_TITLE_SET("Lookup info");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Search");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB ID");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB Name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key");

        for (index = 0; index < DNX_KBP_MAX_NOF_LOOKUPS; index++)
        {
            uint8 db_id;
            uint8 result_id;
            uint8 lookup_type;
            char db_name[DBAL_MAX_STRING_LENGTH] = { 0 };
            char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
            char lookup_type_string[DBAL_MAX_STRING_LENGTH] = { 0 };
            int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");

            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
            dbal_entry_key_field8_set(unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_ID, (uint8) index);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, lkp_entry_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_DB, INST_SINGLE, &db_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_RESULT_ID, INST_SINGLE, &result_id));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                            (unit, lkp_entry_handle_id, DBAL_FIELD_LOOKUP_TYPE, INST_SINGLE, &lookup_type));

            SHR_IF_ERR_EXIT(dnx_dbal_fields_string_form_hw_value_get
                            (unit, "LOOKUP_TYPE", lookup_type, lookup_type_string));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            if (utilex_bitstream_test_bit(&lookup_bmp, index))
            {
                dbal_entry_key_field8_set(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, db_id);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, db_entry_handle_id, DBAL_GET_ALL_FIELDS));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                                (unit, db_entry_handle_id, DBAL_FIELD_DB_NAME, INST_SINGLE, (uint8 *) db_name));

                SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_db_key_build(unit, db_entry_handle_id, db_key));

                PRT_CELL_SET("%d", index); /** Search */
                PRT_CELL_SET("%d", result_id); /** Result */
                PRT_CELL_SET("%d", db_id); /** DB ID */
                PRT_CELL_SET("%s", db_name); /** DB Name */
                PRT_CELL_SET("%s", (lookup_type_string + meaningful_lookup_type_string_offset)); /** Type */
                PRT_CELL_SET("%s", db_key); /** Key */
            }
            else
            {
                PRT_CELL_SET("%d", index); /** Search */
                PRT_CELL_SET("N/A"); /** Result */
                PRT_CELL_SET("N/A"); /** DB ID */
                PRT_CELL_SET(""); /** DB Name */
                PRT_CELL_SET("%s", (lookup_type_string + meaningful_lookup_type_string_offset)); /** Type */
                PRT_CELL_SET(""); /** Key */
            }
        }

        PRT_COMMITX;

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_RESULT_BITMAP, INST_SINGLE, &result_bmp));

        SHR_IF_ERR_EXIT(kbp_mngr_opcode_total_result_size_get(unit, opcode_id, &total_result_bits));
        PRT_TITLE_SET("Total result length %d", BITS2BYTES(total_result_bits) + 1); /** +1 including the hit indication */

        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Length");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Offset");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Padding");

        for (index = 0; index < DNX_KBP_MAX_NOF_RESULTS; index++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%d", index); /** Result */

            if (utilex_bitstream_test_bit(&result_bmp, index))
            {
                uint8 size;
                uint8 offset;
                uint8 padding;

                dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode_id);
                dbal_entry_key_field8_set(unit, res_entry_handle_id, DBAL_FIELD_RESULT_ID, (uint8) index);
                SHR_IF_ERR_EXIT(dbal_entry_get(unit, res_entry_handle_id, DBAL_GET_ALL_FIELDS));

                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_SIZE, INST_SINGLE, &size));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_OFFSET, INST_SINGLE, &offset));
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                                (unit, res_entry_handle_id, DBAL_FIELD_RESULT_PADDING, INST_SINGLE, &padding));

                PRT_CELL_SET("%d", size); /** Length */
                PRT_CELL_SET("%d", offset); /** Offset */
                PRT_CELL_SET("%d", padding); /** Padding */

            }
            else
            {
                PRT_CELL_SET("N/A"); /** Length */
                PRT_CELL_SET("N/A"); /** Offset */
                PRT_CELL_SET("N/A"); /** Padding */
            }
        }

        PRT_COMMITX;

        if (print_one)
        {
            /** Print only one opcode. Stop here */
            break;
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_opcode_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint8 is_valid = TRUE;
    int is_end;
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));

    PRT_TITLE_SET("KBP Opcode list");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        PRT_INFO_ADD("WARNING: KBP device is not locked");
    }

    while (!is_end)
    {
        uint8 opcode_id = 0;
        char opcode_name[DBAL_MAX_STRING_LENGTH] = { 0 };

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr8_get(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, &opcode_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_name));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", opcode_id);
        PRT_CELL_SET("%s", opcode_name);

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for getting KBP DB info
 */
static shr_error_e
dnx_kbp_diag_db_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 db_entry_handle_id;
    uint32 db_id;
    uint32 key_width;
    uint32 res_width;
    uint32 clone_db;
    uint32 type;
    uint32 table_size;
    uint32 table_id;
    uint32 meta_prio;
    uint8 is_valid;
    int is_end;
    char db_name[DBAL_MAX_STRING_LENGTH] = { 0 };
    char db_key[DNX_KBP_MAX_NOF_SEGMENTS_PER_LOOKUP * DNX_KBP_DIAG_PRINTABLE_KEY_SEGMENT_SIZE] = { 0 };
    dbal_enum_value_field_kbp_device_status_e kbp_mngr_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_DB_INFO, &db_entry_handle_id));

    /** Initialize iterator to iterate over all db's */
    is_valid = TRUE;
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, db_entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr8_rule_add
                    (unit, db_entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, DBAL_CONDITION_EQUAL_TO, &is_valid,
                     &is_valid));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));

    PRT_TITLE_SET("KBP DB DUMP");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "ID");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Name");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key Width");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result Width");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Clone From");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Table Size");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Table Id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Meta Prio");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DB key");

    SHR_IF_ERR_EXIT(kbp_mngr_status_get(unit, &kbp_mngr_status));
    if (kbp_mngr_status != DBAL_ENUM_FVAL_KBP_DEVICE_STATUS_LOCKED)
    {
        PRT_INFO_ADD("WARNING: KBP device is not locked");
    }
    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, db_entry_handle_id, DBAL_FIELD_DB_ID, &db_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, db_entry_handle_id, DBAL_FIELD_DB_NAME, INST_SINGLE, (uint8 *) db_name));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_KEY_WIDTH, INST_SINGLE, &key_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_RESULT_WIDTH, INST_SINGLE, &res_width));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_CLONE_DB, INST_SINGLE, &clone_db));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_DB_TYPE, INST_SINGLE, &type));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_SIZE, INST_SINGLE, &table_size));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_TABLE_ID, INST_SINGLE, &table_id));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, db_entry_handle_id, DBAL_FIELD_META_PRIORITY, INST_SINGLE, &meta_prio));

        SHR_IF_ERR_EXIT(dnx_kbp_diag_printable_db_key_build(unit, db_entry_handle_id, db_key));
        PRT_CELL_SET("%d", db_id);
        PRT_CELL_SET("%s", db_name);
        PRT_CELL_SET("%d", key_width);
        PRT_CELL_SET("%d", res_width);
        PRT_CELL_SET("%d", clone_db);
        PRT_CELL_SET("%s", dbal_table_type_to_string(unit, type));
        PRT_CELL_SET("%d", table_size);
        PRT_CELL_SET("%s", dbal_logical_table_to_string(unit, table_id));
        PRT_CELL_SET("%d", meta_prio);
        PRT_CELL_SET("%s", db_key);

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, db_entry_handle_id, &is_end));
    }
    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static void
dnx_kbp_diag_rop_last_payload_to_string(
    dnx_kbp_rop_packet_t * rop_packet,
    char payload_string[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1])
{
    int index;

    sal_memset(payload_string, 0x0, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1));
    for (index = 0; index < rop_packet->payload_len; index++)
    {
        char tmp_byte[3] = { 0 };
        sal_snprintf(tmp_byte, 3, "%02X", rop_packet->payload[index]);
        sal_strncat(payload_string, tmp_byte, (DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1));
    }
}

/**
 * \brief
 * defualt unknown opcode string
 */
static char *unknown_opcode_string = "unknown_opcode";

CONST char *
dnx_kbp_diag_managment_opcode_string(
    int unit,
    uint8 opcode)
{
    switch (opcode)
    {
        case DNX_KBP_CPU_WR_LUT_OPCODE:
            return "WR_LUT";
            break;
        case DNX_KBP_CPU_RD_LUT_OPCODE:
            return "RD_LUT";
            break;
        case DNX_KBP_CPU_PIOWR_OPCODE:
            return "PIOWR";
            break;
        case DNX_KBP_CPU_PIORDX_OPCODE:
            return "PIORDX";
            break;
        case DNX_KBP_CPU_PIORDY_OPCODE:
            return "PIORDY";
            break;
        case DNX_KBP_CPU_ERR_OPCODE:
            return "ERR";
            break;
        case DNX_KBP_CPU_BLK_COPY_OPCODE:
            return "BLK_COPY";
            break;
        case DNX_KBP_CPU_BLK_MOVE_OPCODE:
            return "BLK_MOVE";
            break;
        case DNX_KBP_CPU_BLK_CLR_OPCODE:
            return "BLK_CLR";
            break;
        case DNX_KBP_CPU_BLK_EV_OPCODE:
            return "BLK_EV";
            break;
        case DNX_KBP_CPU_CTX_BUFF_WRITE_OPCODE:
            return "CTX_BUFF_WRITE";
            break;
        case DNX_KBP_CPU_PAD_OPCODE:
            return "PAD";
            break;
        default:
            return unknown_opcode_string;
    }
}

/**
 * \brief
 * Translate KBP forward opcode to string,
 * this function is for opcode>1 and opcode<200 strings
 */
shr_error_e
dnx_kbp_diag_opcode_id_to_string(
    int unit,
    uint8 opcode,
    char *opcode_str)
{
    uint32 entry_handle_id;
    uint8 is_valid = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KBP_OPCODE_INFO, &entry_handle_id));
    dbal_entry_key_field8_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_ID, opcode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    if (opcode_str)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field8_get
                        (unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &is_valid));

        if (!is_valid)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "opcode (%d) not valid\n", opcode);
        }

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr8_get
                        (unit, entry_handle_id, DBAL_FIELD_OPCODE_NAME, INST_SINGLE, (uint8 *) opcode_str));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Translate KBP opcode to string
 */
shr_error_e
dnx_kbp_diag_opcode_string(
    int unit,
    uint8 opcode,
    char *opcode_str)
{
    SHR_FUNC_INIT_VARS(unit);

    if (opcode == DNX_KBP_CPU_PAD_OPCODE)
    {
        sal_snprintf(opcode_str, DBAL_MAX_STRING_LENGTH, "INVALID");
    }
    else if (opcode < DNX_KBP_CPU_MANAGMENT_OPCODE_FIRST)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_id_to_string(unit, opcode, opcode_str));
    }
    else
    {
        sal_snprintf(opcode_str, DBAL_MAX_STRING_LENGTH, "%s", dnx_kbp_diag_managment_opcode_string(unit, opcode));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_kbp_diag_rop_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char opcode_str[DBAL_MAX_STRING_LENGTH];
    int core;
    uint8 request = 0;
    uint8 response = 0;
    dnx_kbp_rop_packet_t rop_packet = { 0 };
    char payload_string[DBAL_FIELD_ARRAY_MAX_SIZE_IN_BYTES * 2 + 1] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    SH_SAND_GET_BOOL("ReQueST", request);
    SH_SAND_GET_BOOL("ReSPoNSe", response);

#if defined(ADAPTER_SERVER_MODE)
    LOG_CLI((BSL_META("ROP diag is not supported in cmodel\n")));
    SHR_EXIT();
#endif

    if (!request && !response)
    {
        /** Print both by default */
        request = TRUE;
        response = TRUE;
    }

    PRT_TITLE_SET("ROP packet diagnostic");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Core");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Opcode");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Seq num");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Length");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Payload");

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        if (request)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_last_request_get(unit, core, DNX_KBP_ROP_GET_DNX_SIDE, &rop_packet));
            dnx_kbp_diag_rop_last_payload_to_string(&rop_packet, payload_string);
            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, rop_packet.opcode, opcode_str));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Request from DNX");
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%s", opcode_str);
            PRT_CELL_SET("%d", rop_packet.sequence_num);
            PRT_CELL_SET("%d", rop_packet.payload_len);
            PRT_CELL_SET("%s", payload_string);
        }

        if (response)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_last_response_get(unit, core, DNX_KBP_ROP_GET_KBP_SIDE, &rop_packet));
            dnx_kbp_diag_rop_last_payload_to_string(&rop_packet, payload_string);
            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, rop_packet.opcode, opcode_str));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", "Response from KBP");
            PRT_CELL_SET("%d", core);
            PRT_CELL_SET("%s", opcode_str);
            PRT_CELL_SET("%d", rop_packet.sequence_num);
            PRT_CELL_SET("%d", rop_packet.payload_len);
            PRT_CELL_SET("%s", payload_string);
        }
    }

    PRT_COMMITX;

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for ROP write
 */
static shr_error_e
dnx_kbp_diag_rop_write_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 *data, *mask, addr, nbo_addr;
    int core, count, i, core_index;
    dnx_kbp_rop_write_t wr_data;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_UINT32("address", addr);
    SH_SAND_GET_ARRAY32("data", data);
    SH_SAND_GET_ARRAY32("mask", mask);
    SH_SAND_GET_INT32("count", count);

    nbo_addr = soc_htonl(addr);
    sal_memcpy(wr_data.addr, &nbo_addr, sizeof(uint32));

#if defined(BE_HOST)
    dnx_kbp_buffer_endians_fix(unit, data);
#endif

    DNX_KBP_ROP_REVERSE_DATA(((uint8 *) data), wr_data.data, NLM_DATA_WIDTH_BYTES);
    sal_memcpy(wr_data.mask, (uint8 *) mask, NLM_DATA_WIDTH_BYTES);

    for (i = 0; i < count; ++i)
    {
        DNXCMN_CORES_ITER(unit, core, core_index)
        {
            SHR_IF_ERR_EXIT(dnx_kbp_rop_write(unit, core_index, &wr_data));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for ROP read
 */
static shr_error_e
dnx_kbp_diag_rop_read_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 addr, nbo_addr;
    int core, i, core_index;
    dnx_kbp_rop_read_t rd_data;
    NlmDnxReadMode datatype;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_UINT32("address", addr);
    SH_SAND_GET_ENUM("datatype", datatype);

    nbo_addr = soc_htonl(addr);
    sal_memcpy(rd_data.addr, &nbo_addr, sizeof(uint32));
    rd_data.dataType = datatype;

    DNXCMN_CORES_ITER(unit, core, core_index)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_rop_read(unit, core_index, &rd_data));
        cli_out("ROP Read for Core %d:\n", core_index);
        cli_out("    Read Status: 0x%02x \n", rd_data.data[0]);
        cli_out("    Read Data:   0x");
        for (i = 1; i < NLM_DATA_WIDTH_BYTES + 1; ++i)
        {
            cli_out("%02x ", rd_data.data[i]);
        }
        cli_out("\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for ROP test
 */
static shr_error_e
dnx_kbp_diag_rop_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{

    int core, count, i, iter, core_index;
    int kbp_mdio_id, kbp_rop_retrans;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core);
    SH_SAND_GET_INT32("count", count);

    kbp_mdio_id = dnx_data_elk.connectivity.mdio_id_get(unit);
    kbp_rop_retrans = dnx_data_elk.recovery.iteration_get(unit);

    DNXCMN_CORES_ITER(unit, core, core_index)
    {
        for (i = 0; i < count; i++)
        {
            SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core_index));

            if (SHR_FUNC_ERR())
            {
                for (iter = 0; iter < kbp_rop_retrans; iter++)
                {
                    SHR_IF_ERR_EXIT(dnx_kbp_recover_run_recovery_sequence(unit, core_index, kbp_mdio_id));

                    SHR_IF_ERR_CONT(dnx_kbp_rop_scratch_write_read(unit, core_index));

                    if (!SHR_FUNC_ERR())
                    {
                        /*
                         * KBP Recovery succeeded
                         */
                        break;
                    }
                }
                /*
                 * KBP Recovery failed
                 */
                if (iter == kbp_rop_retrans)
                {
                    SHR_ERR_EXIT(_SHR_E_NONE, "ROP stability test failed on core %d\n", core_index);
                }
            }
        }

        cli_out("ROP stability test PASSED on core %d\n", core_index);
    }

    cli_out("ROP stability test was completed successfully \n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for dumpping KBP status registers
 */
static shr_error_e
dnx_kbp_diag_status_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *file_path = NULL;
    FILE *kbp_file;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("PATH", file_path);

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)) != 0)
    {
        kbp_file = fopen(file_path, "w");
    }
    else
    {
        kbp_file = stdout;
    }

    DNX_KBP_TRY(kbp_device_interface_print_regs
                (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                 kbp_file));

    if (sal_strncmp(file_path, "stdout", sal_strlen(file_path)) != 0)
    {
        fclose(kbp_file);
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for configuring and enabling KBP prbs
 */
static shr_error_e
dnx_kbp_diag_prbs_set_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int poly, lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("polynomial", poly);
    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);

        cli_out("Enabling PRBS, PRBS poly %d\n", poly);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     poly, 1, lane_bmp));
        cli_out("Clearing PRBS counters, please ignore!\n");
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     lane_bmp));
    }
    else
    {
        cli_out("Enabling PRBS, PRBS poly %d\n", poly);
        DNX_KBP_TRY(kbp_device_interface_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     poly, 1));
        cli_out("Clearing PRBS counters, please ignore!\n");
        DNX_KBP_TRY(kbp_device_interface_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for getting KBP prbs result
 */
static shr_error_e
dnx_kbp_diag_prbs_get_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_prbs_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for disabling KBP prbs
 */
static shr_error_e
dnx_kbp_diag_prbs_clear_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int poly, lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM("polynomial", poly);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    cli_out("Disabling PRBS\n");

    if (lane_is_present)
    {
        /*
         * Per Lane PRBS
         */
        SH_SAND_GET_UINT32("lane", lane_bmp);
        DNX_KBP_TRY(kbp_device_interface_per_lane_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     poly, 0, lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_prbs
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     poly, 0));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for dumpping KBP eyescan for each lane
 */
static shr_error_e
dnx_kbp_diag_eyescan_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_TRY(kbp_device_interface_eyescan_print
                (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for dumpping KBP interface info
 */
static shr_error_e
dnx_kbp_diag_intf_info_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lane_is_present = 0;
    uint32 lane_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT("lane", lane_is_present);

    if (lane_is_present)
    {
        SH_SAND_GET_UINT32("lane", lane_bmp);

        DNX_KBP_TRY(kbp_device_interface_per_lane_diag_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config,
                     lane_bmp));
    }
    else
    {
        DNX_KBP_TRY(kbp_device_interface_diag_print
                    (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags, &Kbp_app_data[unit]->device_config));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for getting KBP result from FWD2 to PMF1,
 * the function prints the parsed payload and related opcode
 */
static shr_error_e
dnx_kbp_diag_vis_result_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int core_id = 0, core_high = 0, core_low = 0;
    int fwd_context;
    int acl_context;
    uint32 *res_sig_value;
    int res_sig_size;
    rhlist_t *sig_list_context;
    rhlist_t *sig_list = NULL;
    signal_output_t *signal_output_context;
    signal_output_t *signal_output = NULL;
    kbp_printable_entry_t kbp_entry_print_info[DNX_KBP_MAX_NOF_RESULTS];
    uint8 kbp_entry_print_num = 0;
    uint8 opcode_id;
    uint8 kbp_entry_index, result_index;
    char  opcode_name[DBAL_MAX_STRING_LENGTH];
    char res_signal_str[DNX_KBP_DIAG_MAX_STRING_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_kbp_device_enabled(unit))
    {
        SHR_ERR_EXIT(_SHR_E_NONE, "KBP was not enabled for the device.\n");
    }

    sal_memset(kbp_entry_print_info, 0x0, DNX_KBP_MAX_NOF_RESULTS * sizeof(kbp_printable_entry_t));

    SH_SAND_GET_INT32("Core", core_id);

    if (core_id == _SHR_CORE_ALL)
    {
        core_low = 0;
        core_high = dnx_data_device.general.nof_cores_get(unit) - 1;
    }
    else if ((core_id < dnx_data_device.general.nof_cores_get(unit)) && (core_id >= 0))
    {
        core_low = core_id;
        core_high = core_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Core %d\n", core_id);
    }

    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        if ((sig_list_context = utilex_rhlist_create("signal sig_list_context", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal sig_list_context\n");
        }

        /*
         * get context signal sig get from=FWD2 show=asic name=context 
         */
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, 0, "", "FWD2", "", "Context", &sig_list_context));
        signal_output_context = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list_context);
        fwd_context = signal_output_context->value[0];

        /*
         * get acl_context signal sig get from=FWD2 show=asic name=acl_context 
         */
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, 0, "", "FWD2", "", "ACL_Context", &sig_list_context));
        signal_output_context = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list_context);
        acl_context = signal_output_context->value[0];
        utilex_rhlist_free_all(sig_list_context);

        /*
         * read opcode from table 
         */

        SHR_IF_ERR_EXIT_EXCEPT_IF(kbp_mngr_context_to_opcode_translate(unit, fwd_context, acl_context, &opcode_id),
                                  _SHR_E_NOT_FOUND);

        if (opcode_id != 255)
        {
            int word_idx, char_count;
            char *res_string = &res_signal_str[0];
            int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");

            SHR_IF_ERR_EXIT(dnx_kbp_diag_opcode_string(unit, opcode_id, opcode_name));
            sig_list = NULL;
            if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
            }

            SHR_IF_ERR_EXIT(sand_signal_find
                            (unit, core_id, SIGNALS_MATCH_ASIC, "", "FWD2", "IPMF1", "Elk_Lkp_Payload", &sig_list));
            signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
            res_sig_value = signal_output->value;
            res_sig_size = signal_output->size;

            /** Print Res as buffer */
            char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "RES(%db)=0x", res_sig_size);
            res_string += char_count;
            for (word_idx = BITS2WORDS(res_sig_size) - 1; word_idx >= 0; word_idx--)
            {
                if (word_idx == (BITS2WORDS(res_sig_size) - 1))
                {
                    int print_width = (3 + (res_sig_size % 32)) / 4;

                    if (print_width == 0)
                    {
                        print_width = 8;
                    }
                    char_count = sal_snprintf(res_string,
                            DNX_KBP_DIAG_MAX_STRING_LENGTH, "%0*x", print_width, res_sig_value[word_idx]);
                    res_string += char_count;
                }
                else
                {
                    char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%08x", res_sig_value[word_idx]);
                    res_string += char_count;
                }
            }
            SHR_IF_ERR_EXIT(kbp_mngr_opcode_printable_entry_result_parsing(
                                unit, 0, opcode_id, res_sig_value, res_sig_size, &kbp_entry_print_num, kbp_entry_print_info));

            PRT_TITLE_SET("KBP VIS packet diagnostic Core (%d) Opcode (%s)", core_id, opcode_name);
            PRT_INFO_ADD("Result value(%s)", res_signal_str);
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Lookup Id");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Type");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DBAL Table");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result values");
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            for (kbp_entry_index=0; kbp_entry_index<kbp_entry_print_num; kbp_entry_index++)
            {
                kbp_printable_entry_t *kbp_entry = &kbp_entry_print_info[kbp_entry_index];

                res_string = &res_signal_str[0];

                for (result_index = 0; result_index < kbp_entry->entry_print_info.nof_res_fields; result_index++)
                {
                    char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%s ",
                            kbp_entry->entry_print_info.res_fields_info[result_index].field_name);
                    res_string += char_count;
                    char_count = sal_snprintf(res_string, DNX_KBP_DIAG_MAX_STRING_LENGTH, "%s\n",
                            kbp_entry->entry_print_info.res_fields_info[result_index].field_print_value);
                    res_string += char_count;
                }
                PRT_CELL_SET("%d", kbp_entry_print_info[kbp_entry_index].lookup_id);

                PRT_CELL_SET("%s", (kbp_entry_print_info[kbp_entry_index].lookup_type_str + meaningful_lookup_type_string_offset));

                PRT_CELL_SET("%s", dbal_logical_table_to_string(unit,kbp_entry_print_info[kbp_entry_index].table_id));

                PRT_CELL_SET("%s", res_signal_str);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            }

            PRT_COMMITX;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}



/* *INDENT-OFF* */

static sh_sand_man_t dnx_kbp_diag_device_man = {
    .brief = "KBP device diagnostic commands",
    .full = NULL
};
static sh_sand_man_t dnx_kbp_diag_device_print_man = {
    .brief = "Prints device information in HTML format to the specified file. "
             "If option \"path\" is not present, the output will be printed on the screen.",
    .full = NULL,
    .synopsis = "path=<file_path>",
    .examples = "path=kbp_device_print.log"
};
static sh_sand_man_t dnx_kbp_diag_device_dump_man = {
   .brief = "Dumps the current state of the s/w in the XML Format to the specified file. "
            "If option \"path\" is not present, the output will be printed on the screen.",
   .full = NULL,
   .synopsis = "path=<file_path>",
   .examples = "path=kbp_device_dump.log"
};
static sh_sand_man_t dnx_kbp_diag_device_swstate_man = {
   .brief = "Prints the current sw state in HTML format to the specified file. "
            "If option \"path\" is not present, the output will be printed on the screen.",
   .full = NULL,
   .synopsis = "path=<file_path>",
   .examples = "path=kbp_device_swstate.log"
};

static sh_sand_man_t dnx_kbp_diag_sdk_ver_man = {
    .brief = "Prints the KBPSDK version",
    .full = NULL
};

static sh_sand_man_t dnx_kbp_diag_opcode_man = {
    .brief = "KBP opcode diagnostic commands",
    .full = NULL,
};

static sh_sand_man_t dnx_kbp_diag_db_man = {
    .brief = "KBP DB diagnostic commands",
    .full = NULL,
};

static sh_sand_man_t dnx_kbp_diag_opcode_dump_man = {
   .brief = "Dumps all relevant information for a given opcode name or all for all opcodes",
   .full = NULL,
   .synopsis = "opcode=<opcode_id | ALL>",
   .examples = "opcode=1"
};

static sh_sand_man_t dnx_kbp_diag_db_dump_man = {
   .brief = "Dumps all relevant information for a given db",
   .full = NULL,
};

static sh_sand_man_t dnx_kbp_diag_opcode_list_man = {
   .brief = "Lists all valid opcodes",
   .full = NULL,
};

static sh_sand_man_t dnx_kbp_diag_rop_man = {
    .brief = "ROP diagnostic commands",
    .full = "Print last ROP request packet from DNX and/or response packet from KBP",
    .synopsis = "ReQueST=0/1 ReSPoNSe=0/1",
    .examples = "ReQueST=1 ReSPoNSe=1"
};

static sh_sand_man_t dnx_kbp_diag_vis_result_man = {
        .brief = "KBP visibility result diagnostic command",
        .full = "Dumps the parsed KBP result from FWD2 to IPMF1 and its opcode"
};

static sh_sand_man_t dnx_kbp_diag_rop_write_man = {
    .brief    = "KBP ROP write operation",
    .full     = "KBP ROP write operation. The data and mask length should be 10 bytes. "
                "If the given data or mask length is less than 10 bytes, it will be padded "
                "to 10 bytes with 0. If the given data or mask length is more than 10 bytes, "
                "only the first 10 bytes should be valid.",
    .synopsis = "address=<addr> [core=<core>] [data=<data>] [mask=<mask>] [count=<count>]",
    .examples = "address=0xe data=0x11223344556677889900",
};
static sh_sand_man_t dnx_kbp_diag_rop_read_man = {
    .brief    = "KBP ROP Read operation",
    .full     = "KBP ROP Read operation",
    .synopsis = "address=<addr> [core=<core>]",
    .examples = "address=0xe core=0",
};
static sh_sand_man_t dnx_kbp_diag_rop_test_man = {
    .brief    = "KBP ROP Test operation",
    .full     = "KBP ROP Test operation",
    .synopsis = "[core=<core>] [count=<count>]",
    .examples = "core=0 count=5000",
};
static sh_sand_man_t dnx_kbp_diag_status_man = {
    .brief    = "Dump KBP status registers",
    .full     = "Dump KBP status registers. If option \"path\" is not present, "
                "the output will be printed on the screen.",
    .synopsis = "[path=<file_path>]",
    .examples = "",
};
static sh_sand_man_t dnx_kbp_diag_prbs_set_man = {
    .brief    = "Enable and configure KBP PRBS",
    .full     = "Enable and configure KBP PRBS. "
                "Support Per lane PRBS. Please specify the lane bitmp for "
                "option \"lane\" if per lane PRBS is required.",
    .synopsis = "[poly=<poly>][lane=<lane_bitmap>]",
    .examples = "poly=5",
};
static sh_sand_man_t dnx_kbp_diag_prbs_get_man = {
    .brief    = "Get KBP PRBS status",
    .full     = "Get KBP PRBS status. "
                "If per lane PRBS is enabled, please also specify "
                "the lane bitmap for get command.",
    .synopsis = "[lane=<lane_bitmap>]",
    .examples = "",
};
static sh_sand_man_t dnx_kbp_diag_prbs_clear_man = {
    .brief    = "Disable KBP PRBS",
    .full     = "Disable KBP PRBS. "
                "If per lane prbs is enabled, please also specify "
                "the lane bitmap for clear command",
    .synopsis = "[poly=<poly>][lane=<lane_bitmap>]",
    .examples = "poly=5",
};
static sh_sand_man_t dnx_kbp_diag_prbs_man = {
    .brief = "KBP PRBS",
    .full = NULL
};
static sh_sand_man_t dnx_kbp_diag_eyescan_man = {
    .brief    = "KBP eyescan",
    .full     = "KBP eyescan for all the active lanes",
    .synopsis = NULL,
    .examples = "",
};
static sh_sand_man_t dnx_kbp_diag_intf_info_man = {
    .brief    = "Dump KBP interface info.",
    .full     = "Dump KBP interface info.",
    .synopsis = "[lane=<lane_bitmap>]",
    .examples = "",
};
static sh_sand_man_t dnx_kbp_diag_intf_man = {
    .brief = "KBP Interface Diag",
    .full = NULL
};

static sh_sand_man_t dnx_kbp_diag_vis_man = {
    .brief = "KBP vis Interface Diag",
    .full = NULL
};

sh_sand_man_t sh_dnx_kbp_man = {
    .brief = "KBP diagnostic commands",
    .full = NULL
};

static sh_sand_option_t dnx_kbp_diag_device_options[] = {
    {"PATH", SAL_FIELD_TYPE_STR, "File path to the output file", "stdout"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_opcode_dump_options[] = {
    {"OPcode", SAL_FIELD_TYPE_UINT32, "ID of the opcode", "ALL", (void*)dnx_kbp_diag_opcode_dump_enum_table},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_db_dump_options[] = {
    {NULL}
};


static sh_sand_option_t dnx_kbp_diag_opcode_list_options[] = {
    {NULL}
};


static sh_sand_option_t dnx_kbp_diag_rop_options[] = {
    {"ReQueST", SAL_FIELD_TYPE_BOOL, "Request from DNX", "FALSE"},
    {"ReSPoNSe", SAL_FIELD_TYPE_BOOL, "Response from KBP", "FALSE"},
    {NULL}
};

static sh_sand_option_t dnx_kbp_diag_rop_write_options[] = {
    {"ADdRess", SAL_FIELD_TYPE_UINT32, "Address", NULL},
    {"DaTa", SAL_FIELD_TYPE_ARRAY32, "Date", "0"},
    {"MAsK", SAL_FIELD_TYPE_ARRAY32, "Mask", "0"},
    {"CouNT", SAL_FIELD_TYPE_INT32, "Number of iterations for rop write", "1"},
    {NULL}
};

static sh_sand_enum_t dnx_kbp_diag_rop_read_data_type_enum_table[] = {
    {"0",     NLM_DNX_READ_MODE_DATA_X},
    {"1",     NLM_DNX_READ_MODE_DATA_Y},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_rop_read_options[] = {
    {"ADdRess", SAL_FIELD_TYPE_UINT32, "Address", NULL},
    {"DaTaTyPe", SAL_FIELD_TYPE_ENUM, "Data Type", "0", (void *) dnx_kbp_diag_rop_read_data_type_enum_table},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_rop_test_options[] = {
    {"CouNT", SAL_FIELD_TYPE_INT32, "Number of iterations for rop test", "10000"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_status_options[] = {
    {"PATH", SAL_FIELD_TYPE_STR, "File path for the output file", "stdout"},
    {NULL}
};
static sh_sand_enum_t dnx_kbp_diag_prbs_poly_enum_table[] = {
    {"0", KBP_PRBS_7},
    {"1", KBP_PRBS_9},
    {"2", KBP_PRBS_11},
    {"3", KBP_PRBS_15},
    {"4", KBP_PRBS_23},
    {"5", KBP_PRBS_31},
    {"6", KBP_PRBS_58},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_set_options[] = {
    {"POLYnomial", SAL_FIELD_TYPE_ENUM, "PRBS poly", "5", (void *) dnx_kbp_diag_prbs_poly_enum_table},
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_get_options[] = {
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_prbs_clear_options[] = {
    {"POLYnomial", SAL_FIELD_TYPE_ENUM, "PRBS poly", "5", (void *) dnx_kbp_diag_prbs_poly_enum_table},
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};
static sh_sand_option_t dnx_kbp_diag_intf_info_options[] = {
    {"Lane", SAL_FIELD_TYPE_UINT32, "Lane Bitmap", "1"},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_device_cmds[] = {
    {"PRinT",   dnx_kbp_diag_device_print_cmd,      NULL,   dnx_kbp_diag_device_options,    &dnx_kbp_diag_device_print_man},
    {"DuMP",    dnx_kbp_diag_device_dump_cmd,       NULL,   dnx_kbp_diag_device_options,    &dnx_kbp_diag_device_dump_man},
    {"SWSTate", dnx_kbp_diag_device_swstate_cmd,    NULL,   dnx_kbp_diag_device_options,    &dnx_kbp_diag_device_swstate_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_opcode_cmds[] = {
    {"DuMP",    dnx_kbp_diag_opcode_dump_cmd,   NULL,   dnx_kbp_diag_opcode_dump_options,   &dnx_kbp_diag_opcode_dump_man},
    {"LIST",    dnx_kbp_diag_opcode_list_cmd,   NULL,   dnx_kbp_diag_opcode_list_options,   &dnx_kbp_diag_opcode_list_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_db_cmds[] = {
    {"DuMP",    dnx_kbp_diag_db_dump_cmd,   NULL,   dnx_kbp_diag_db_dump_options,   &dnx_kbp_diag_db_dump_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_vis_cmds[] = {
    {"RESult",  dnx_kbp_diag_vis_result_cmd,    NULL,   NULL,   &dnx_kbp_diag_vis_result_man},
    {NULL}
};

static sh_sand_cmd_t dnx_kbp_diag_prbs_cmds[] = {
    {"set",     dnx_kbp_diag_prbs_set_cmd,   NULL,   dnx_kbp_diag_prbs_set_options,   &dnx_kbp_diag_prbs_set_man},
    {"get",     dnx_kbp_diag_prbs_get_cmd,   NULL,   dnx_kbp_diag_prbs_get_options,   &dnx_kbp_diag_prbs_get_man},
    {"clear",   dnx_kbp_diag_prbs_clear_cmd, NULL,   dnx_kbp_diag_prbs_clear_options, &dnx_kbp_diag_prbs_clear_man},
    {NULL}
};
static sh_sand_cmd_t dnx_kbp_diag_intf_cmds[] = {
    {"info",    dnx_kbp_diag_intf_info_cmd,  NULL,   dnx_kbp_diag_intf_info_options,   &dnx_kbp_diag_intf_info_man},
    {NULL}
};
sh_sand_cmd_t sh_dnx_kbp_cmds[] = {
    {"DeViCe",     NULL,       dnx_kbp_diag_device_cmds,   NULL,                            &dnx_kbp_diag_device_man},
    {"VERSion",    dnx_kbp_diag_sdk_ver_cmd,       NULL,   NULL,                            &dnx_kbp_diag_sdk_ver_man},
    {"OPcode",     NULL,       dnx_kbp_diag_opcode_cmds,   NULL,                            &dnx_kbp_diag_opcode_man},
    {"DB",         NULL,       dnx_kbp_diag_db_cmds,       NULL,                            &dnx_kbp_diag_db_man},
    {"ROP",        dnx_kbp_diag_rop_cmd,           NULL,   dnx_kbp_diag_rop_options,        &dnx_kbp_diag_rop_man},
    {"ROP_WRite",  dnx_kbp_diag_rop_write_cmd,     NULL,   dnx_kbp_diag_rop_write_options,  &dnx_kbp_diag_rop_write_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"ROP_ReaD",   dnx_kbp_diag_rop_read_cmd,      NULL,   dnx_kbp_diag_rop_read_options,   &dnx_kbp_diag_rop_read_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"ROP_TeST",   dnx_kbp_diag_rop_test_cmd,      NULL,   dnx_kbp_diag_rop_test_options,   &dnx_kbp_diag_rop_test_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"StaTuS",     dnx_kbp_diag_status_cmd,        NULL,   dnx_kbp_diag_status_options,     &dnx_kbp_diag_status_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"PRBS",       NULL,         dnx_kbp_diag_prbs_cmds,   NULL,                            &dnx_kbp_diag_prbs_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"EYEScan",    dnx_kbp_diag_eyescan_cmd,       NULL,   NULL,                            &dnx_kbp_diag_eyescan_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"INTerFace",  NULL,         dnx_kbp_diag_intf_cmds,   NULL,                            &dnx_kbp_diag_intf_man, NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {"VISibility", NULL,         dnx_kbp_diag_vis_cmds,    NULL,                            &dnx_kbp_diag_vis_man,  NULL, NULL, SH_CMD_CONDITIONAL, dnx_kbp_diag_interface_is_enabled},
    {NULL}
};

/* *INDENT-ON* */

/*
 * }
 */
#else

/** This is DUMMY code. It is only for compilers that do not accept empty files and is never to be used. */
/* *INDENT-OFF* */
void dnx_kbp_diag_dummy_empty_function_to_make_compiler_happy() { return; }
/* *INDENT-ON* */

#endif /** defined(INCLUDE_KBP) */
