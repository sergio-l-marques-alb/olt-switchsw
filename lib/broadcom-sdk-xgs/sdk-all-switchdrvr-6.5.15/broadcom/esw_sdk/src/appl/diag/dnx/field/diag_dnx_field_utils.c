/** \file diag_dnx_field_utils.c
 *
 * Diagnostics utilities to be used for DNX diagnostics of Field Module.
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field_context.h>
#include <appl/diag/dnx/field/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_signals.h>
#include "diag_dnx_field_utils.h"

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */
/*
 * }
 */

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_group_type_conversion(
    int unit,
    dnx_field_group_type_e fg_type,
    char *converted_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            sal_strncpy(converted_fg_type_p, "TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_TABLE_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_TABLE_MDB", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            sal_strncpy(converted_fg_type_p, "EXEM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            sal_strncpy(converted_fg_type_p, "STATE_TABLE", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            sal_strncpy(converted_fg_type_p, "DIRECT_EXTRACTION", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            sal_strncpy(converted_fg_type_p, "EXTERNAL_TCAM", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            sal_strncpy(converted_fg_type_p, "CONST", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Field Group type (%s).", dnx_field_group_type_e_get_name(fg_type));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_key_size_conversion(
    int unit,
    dnx_field_key_length_type_e key_size,
    uint32 *key_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (key_size)
    {
        case DNX_FIELD_KEY_LENGTH_TYPE_DT:
        {
            *key_size_in_bits = dnx_data_field.tcam.dt_max_key_size_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_HALF:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_half_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_SINGLE:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_single_get(unit);
            break;
        }
        case DNX_FIELD_KEY_LENGTH_TYPE_DOUBLE:
        {
            *key_size_in_bits = dnx_data_field.tcam.key_size_double_get(unit);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting key size enum (%d) to TCAM key size", key_size);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_action_size_conversion(
    int unit,
    dnx_field_action_length_type_e action_size,
    uint32 *action_size_in_bits)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (action_size)
    {
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_HALF:
        {
            *action_size_in_bits = dnx_data_field.tcam.action_size_half_get(unit);
            break;
        }
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_SINGLE:
            *action_size_in_bits = dnx_data_field.tcam.action_size_single_get(unit);
            break;
        case DNX_FIELD_ACTION_LENGTH_TYPE_TCAM_DOUBLE:
        {
            *action_size_in_bits = dnx_data_field.tcam.action_size_double_get(unit);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error converting action size enum (%d) to TCAM action size", action_size);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_cmp_hash_state_mode_conversion(
    int unit,
    int mode,
    char *converted_mode_p,
    int is_hash,
    int is_state)
{
    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(converted_mode_p, "", DIAG_DNX_FIELD_UTILS_STR_SIZE);

    if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE && !is_hash && !is_state)
    {
        sal_strncpy(converted_mode_p, "S, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if (mode == DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE)
    {
        sal_strncpy(converted_mode_p, "D, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_HASH_MODE_ENABLED) && is_hash)
    {
        sal_strncpy(converted_mode_p, "HASH.E, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else if ((mode == DNX_FIELD_CONTEXT_STATE_TABLE_MODE_ENABLED) && is_state)
    {
        sal_strncpy(converted_mode_p, "ST.E, ", DIAG_DNX_FIELD_UTILS_STR_SIZE);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Mode.");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_context_modes_string_get(
    int unit,
    dnx_field_context_mode_t context_mode,
    char *mode_string_p)
{
    char mode_prefix_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char converted_mode[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    char mode_string[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
    int is_hash, is_state;

    SHR_FUNC_INIT_VARS(unit);

    is_hash = is_state = 0;
    sal_strncpy(mode_string, "", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

    if (context_mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_1,
                                                                            converted_mode, is_hash, is_state));
        sal_strncpy(mode_prefix_string, "CMP1.", sizeof(mode_prefix_string) - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", mode_prefix_string);
        sal_strncat(mode_string, mode_prefix_string, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat(mode_string, converted_mode, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    }

    if (context_mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.compare_mode_2,
                                                                            converted_mode, is_hash, is_state));
        sal_strncpy(mode_prefix_string, "CMP2.", sizeof(mode_prefix_string) - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", mode_prefix_string);
        sal_strncat(mode_string, mode_prefix_string, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat(mode_string, converted_mode, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    }

    if (context_mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        is_hash = 1;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.context_ipmf1_mode.hash_mode,
                                                                            converted_mode, is_hash, is_state));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat(mode_string, converted_mode, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    }

    if (context_mode.context_ipmf1_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        is_state = 1;
        SHR_IF_ERR_EXIT(diag_dnx_field_utils_cmp_hash_state_mode_conversion(unit,
                                                                            context_mode.
                                                                            context_ipmf1_mode.state_table_mode,
                                                                            converted_mode, is_hash, is_state));
        DIAG_DNX_FIELD_UTILS_STR_INPUT_VERIFY(mode_string, "%s", converted_mode);
        sal_strncat(mode_string, converted_mode, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    }

    sal_strncpy(mode_string_p, mode_string, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_cmp_hash_state_key_usage_info_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_mode_t mode,
    dnx_field_context_ipmf1_sw_info_t * ctx_mode_key_usage_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_ipmf1_sw_info_t_init(unit, ctx_mode_key_usage_info_p));

    if (mode.context_ipmf1_mode.compare_mode_1 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE ||
        mode.context_ipmf1_mode.compare_mode_2 != DNX_FIELD_CONTEXT_COMPARE_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_cmp_info_get(unit, context_id, &(ctx_mode_key_usage_info_p->compare_info)));
    }

    if (mode.context_ipmf1_mode.hash_mode != DNX_FIELD_CONTEXT_HASH_MODE_DISABLED)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_hashing_info_get
                        (unit, context_id, &(ctx_mode_key_usage_info_p->hashing_info)));
    }

    if (mode.context_ipmf1_mode.state_table_mode != DNX_FIELD_CONTEXT_STATE_TABLE_MODE_DISABLED)
    {
        SHR_IF_ERR_EXIT(dnx_field_context_state_table_info_get
                        (unit, context_id, &(ctx_mode_key_usage_info_p->state_table_info)));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See header on diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_ffc_groups_names_get(
    int unit,
    dnx_field_stage_e field_stage,
    unsigned int ffc_group_num,
    char ffc_group_name[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_group_name, _SHR_E_PARAM, "ffc_group_name");

    if (ffc_group_num >= dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_ffc_groups)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "There are only %d FFC groups in stage %s (%d), requested group %d.\r\n",
                     dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_ffc_groups,
                     dnx_field_stage_text(unit, field_stage), field_stage, ffc_group_num);
    }

    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        {
            switch (ffc_group_num)
            {
                case 0:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 1:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 2:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                case 3:
                {
                    sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Stage iPMF1 has more than 4 FFC field groups.\r\n");
                    break;
                }
            }
            break;
        }
        case DNX_FIELD_STAGE_IPMF2:
        {
            /*
            * All stages other than iPMF1 have only one FFC group.
            */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                            dnx_field_stage_text(unit, field_stage), field_stage,
                            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            /*
            * All stages other than iPMF1 have only one FFC group.
            */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                            dnx_field_stage_text(unit, field_stage), field_stage,
                            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData/LayerReord", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            /*
            * All stages other than iPMF1 have only one FFC group.
            */
            if (dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups != 1)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Stage %s (%d) is not iPMF1, but has more than one field group type (%d).\r\n",
                            dnx_field_stage_text(unit, field_stage), field_stage,
                            dnx_data_field.stage.stage_info_get(unit, field_stage)->nof_ffc_groups);
            }
            sal_strncpy_s(ffc_group_name, "MetaData/LayerReord/Header", DIAG_DNX_FIELD_UTILS_STR_SIZE);
            break;
        }
        default:
        {
            bcm_field_stage_t bcm_stage;
            /** Convert DNX to BCM Field Stage */
            SHR_IF_ERR_EXIT(dnx_field_map_stage_dnx_to_bcm(unit, field_stage, &bcm_stage));
            LOG_CLI_EX("\r\n" "Given field stage (%s) is not supported!!%s%s%s\r\n\n",
                      dnx_field_bcm_stage_text(bcm_stage), EMPTY, EMPTY, EMPTY);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_groups_per_context_get(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_stage_e field_stage,
    uint32 *nof_fgs_p,
    dnx_field_group_t * fg_ids_p)
{
    uint8 is_fg_allocated;
    int ctx_id_index, fg_id_index, fg_per_ctx_index;
    dnx_field_stage_e fg_stage;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Iterate over maximum number of FGs and take information for those, which are being
     * attached to the current context.
     */
    fg_per_ctx_index = 0;
    for (fg_id_index = 0; fg_id_index < dnx_data_field.group.nof_fgs_get(unit); fg_id_index++)
    {
        dnx_field_context_t context_id_arr[DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS];
        SHR_IF_ERR_EXIT(dnx_algo_field_group_is_allocated(unit, fg_id_index, &is_fg_allocated));
        if (!is_fg_allocated)
        {
            /** If FG_ ID is not allocated, move to next FG ID */
            continue;
        }

        /** Get information about the current Field Group. */
        SHR_IF_ERR_EXIT(dnx_field_group_field_stage_get(unit, fg_id_index, &fg_stage));
        if (fg_stage != field_stage)
        {
            /**
             * If the current allocated FG ID is not on the same stage as the given context,
             * continue iterating over all FGs.
             */
            continue;
        }

        sal_memset(context_id_arr, DNX_FIELD_CONTEXT_ID_INVALID, sizeof(context_id_arr));

        /** Get the list of all contexts that the FG is attached to. */
        SHR_IF_ERR_EXIT(dnx_field_group_context_id_arr_get(unit, fg_id_index, context_id_arr));

        for (ctx_id_index = 0; ctx_id_index < DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_CONTEXTS &&
             context_id_arr[ctx_id_index] != DNX_FIELD_CONTEXT_ID_INVALID; ctx_id_index++)
        {
            if (context_id_arr[ctx_id_index] == context_id)
            {
                fg_ids_p[fg_per_ctx_index] = fg_id_index;
                fg_per_ctx_index++;
            }
        }
    }

    *nof_fgs_p = fg_per_ctx_index;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_nof_alloc_fems_per_fg_get(
    int unit,
    dnx_field_group_t fg_id,
    dnx_field_fem_id_t * nof_fems_p)
{
    dnx_field_fem_id_t dnx_fem_id, max_nof_fems, nof_alloc_fems;
    dnx_field_fem_program_t fem_program_index, fem_program_max;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_fems = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_id;
    fem_program_max = dnx_data_field.stage.stage_info_get(unit, DNX_FIELD_STAGE_IPMF1)->nof_fem_programs;

    nof_alloc_fems = 0;

    for (dnx_fem_id = 0; dnx_fem_id < max_nof_fems; dnx_fem_id++)
    {
        /*
         * If a match is found then display all four 'actions' corresponding to this FEM id
         */
        for (fem_program_index = 0; fem_program_index < fem_program_max; fem_program_index++)
        {
            dnx_field_group_t returned_fg_id;

            SHR_IF_ERR_EXIT(FEM_INFO_FG_ID_INFO.fg_id.get(unit, dnx_fem_id, fem_program_index, &returned_fg_id));
            if (returned_fg_id == fg_id)
            {
                /*
                 * This 'fem_program' is occupied for specified 'fem_id'.
                 */
                nof_alloc_fems++;
                break;
            }
        }
    }

    /** Return the calculated value. */
    *nof_fems_p = nof_alloc_fems;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_last_key_table_info_get(
    int unit,
    dbal_tables_e table_id,
    bcm_core_t core_id,
    dbal_enum_value_field_field_key_e key_id,
    uint32 *field_key_value_p)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    /*
     * Specifying from which core, the DBAL table info should be taken.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    /*
     * Setting the key ID, for which we want to retrieve an information.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FIELD_KEY, key_id);

    /*
     * Perform the DBAL read
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    /*
     * Get a result fields
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                    (unit, entry_handle_id, DBAL_FIELD_LAST_KEY, INST_SINGLE, field_key_value_p));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_exem_entry_key_info_get(
    int unit,
    dnx_field_group_full_info_t * fg_info_p,
    uint32 *key_values_p,
    dnx_field_entry_key_t * entry_key_p)
{
    unsigned int qual_index;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_index = 0; qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG &&
         fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type != DNX_FIELD_QUAL_TYPE_INVALID;
         qual_index++)
    {
        entry_key_p->qual_info[qual_index].dnx_qual =
            fg_info_p->group_full_info.key_template.key_qual_map[qual_index].qual_type;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(key_values_p,
                                                       fg_info_p->group_full_info.key_template.
                                                       key_qual_map[qual_index].lsb,
                                                       fg_info_p->group_full_info.key_template.
                                                       key_qual_map[qual_index].size,
                                                       entry_key_p->qual_info[qual_index].qual_value));
    }

    if (qual_index < DNX_DATA_MAX_FIELD_GROUP_NOF_QUALS_PER_FG)
    {
        entry_key_p->qual_info[qual_index].dnx_qual = DNX_FIELD_QUAL_TYPE_INVALID;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_dt_available_entries_get(
    int unit,
    dnx_field_group_t fg_id,
    bcm_core_t core_id,
    uint32 *num_available_entries_p)
{
    uint32 entry_iter, num_available_entries;
    uint32 max_nof_dt_entries;
    dnx_field_entry_t entry_info;
    uint32 entry_handle;
    dnx_field_group_type_e fg_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_group_type_get(unit, fg_id, &fg_type));

    if (fg_type != DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM)
    {
        LOG_CLI_EX("\r\n" "Only for DirectTable Field groups are supported!! %s%s%s%s\r\n\n",
                   EMPTY, EMPTY, EMPTY, EMPTY);
        SHR_EXIT();
    }

    num_available_entries = 0;

    /** Set max number of DT entries, to iterate on, depends on the max DT key size. */
    max_nof_dt_entries = utilex_power_of_2(dnx_data_field.tcam.dt_max_key_size_get(unit));

    /** Iterate over max number of DT entries (2k). */
    for (entry_iter = 0; entry_iter < max_nof_dt_entries; entry_iter++)
    {
        entry_handle = DNX_FIELD_TCAM_ENTRY(fg_id, entry_iter, core_id);

        /** Get information about the given Entry. */
        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        SHR_SET_CURRENT_ERR(dnx_field_entry_tcam_get(unit, fg_id, entry_handle, &entry_info));
        /** In case the current entry is not found, then increase the output counter. */
        if (SHR_GET_CURRENT_ERR() == _SHR_E_NOT_FOUND)
        {
            num_available_entries++;
        }
    }

    /** Reset the current error to not break calling functions. */
    SHR_SET_CURRENT_ERR(_SHR_E_NONE);

    *num_available_entries_p = num_available_entries;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_qual_signal_value_raw_data_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    uint32 out_qual_sig_value[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY])
{
    dnx_field_map_qual_signal_info_t qual_signals[DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER];
    uint32 signal_iter;
    int out_buff_offset;

    SHR_FUNC_INIT_VARS(unit);

    out_buff_offset = 0;

    /** Get signals related to the current qualifier. */
    SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_signals(unit, field_stage, dnx_qual, qual_signals));

    /** Iterate over number of signals for current qualifier. */
    for (signal_iter = 0; signal_iter < DNX_DATA_MAX_FIELD_DIAG_NOF_SIGNALS_PER_QUALIFIER; signal_iter++)
    {
        signal_output_t * signal_output = NULL;

        /** Break in case we reach an empty string, which is indication the end of signals array. */
        if (qual_signals[signal_iter].name == 0)
        {
            break;
        }

        sand_signal_info_get(unit, core_id, block_p, qual_signals[signal_iter].from, qual_signals[signal_iter].to, qual_signals[signal_iter].name, &signal_output);

        SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(signal_output->value, out_buff_offset, signal_output->size, out_qual_sig_value));

        out_buff_offset += signal_output->size;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See diag_dnx_field_utils.h
 */
shr_error_e
diag_dnx_field_utils_user_qual_metadata_value_get(
    int unit,
    bcm_core_t core_id,
    char *block_p,
    dnx_field_stage_e field_stage,
    dnx_field_qual_t dnx_qual,
    int qual_attach_offset,
    char out_qual_value[DIAG_DNX_FIELD_UTILS_STR_SIZE])
{
    dnx_field_user_qual_info_t user_qual_info;
    int qual_offset;
    int next_qual_offset;
    int qual_index;
    const dnx_field_qual_map_t * meta_qual_map;
    uint32 user_qual_size;
    uint32 qual_size;
    uint8 qual_found;
    dnx_field_qual_t pbus_dnx_qual;
    int buff_offset, buff_size;
    int val_iter;
    int max_val_iter;
    int max_num_bits_in_ffc;
    uint32 out_qual_value_buff[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
    uint32 out_qual_value_buff_offset;

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(out_qual_value, "0x", DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);
    buff_offset = buff_size = 0;
    qual_offset = 0;
    next_qual_offset = qual_attach_offset;
    out_qual_value_buff_offset = 0;

    /** Get the max num of bits in FFC from DNX data. */
    max_num_bits_in_ffc = dnx_data_field.common_max_val.nof_bits_in_ffc_get(unit);

    /** Get info for the given user qualifier. */
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.info.get(unit, DNX_QUAL_ID(dnx_qual), &user_qual_info));
    user_qual_size = user_qual_info.size;

    /** Flag to indicate if we found a qualifier with matching offset with the given one qual_attach_offset. */
    qual_found = FALSE;

    /** Iterate over all BCM qualifiers, until reach max number bcmFieldQualifyCount, or size of the give user qualifier is equal to 0. */
    for (qual_index = 0; qual_index < bcmFieldQualifyCount && user_qual_size; qual_index++)
    {
        /** Get the qualifier map info for the given stage and current bcm qualifier. */
        meta_qual_map = &dnx_field_map_stage_info[field_stage].meta_qual_map[qual_index];

        /** Check if this bcm qual is mapped on this stage, if not continue. */
        if (meta_qual_map->dnx_qual == 0)
        {
            continue;
        }

        if (field_stage == DNX_FIELD_STAGE_IPMF2)
        {
            pbus_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, field_stage, meta_qual_map->dnx_qual);
        }
        else
        {
            pbus_dnx_qual = DNX_QUAL(DNX_FIELD_QUAL_CLASS_META, field_stage, meta_qual_map->dnx_qual);
        }

        /** Get current qualifier size and offset. */
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_size( unit, field_stage, pbus_dnx_qual, &qual_size));
        SHR_IF_ERR_EXIT(dnx_field_map_dnx_qual_offset(unit, field_stage, pbus_dnx_qual, &qual_offset));

        /**
         * Check if current qualifier is in the correct offset of PBUS.
         * For example:
         *      We have as input parameter the qualifier offset from the attach info
         *      to be (next_qual_offset = qual_attach_offset = 1400). Current qualifier offset is
         *      (qual_offset = 1300) and (qual_size = 200). We should check in which offset
         *      user qualifier resides and its value should be taken. In other hand we check
         *      that sum of the current qualifier offset and size is bigger than next_qual_offset.
         *      (1300 <= 1400) && ((1300 + 200) > 1400) --> TRUE && (1500 > 1400) --> TRUE && TRUE = TRUE
         *      Let say that user_qual_size is 100b. It means that our user qualifier is taking its value:
         *
         *                      1400 (next_qaul_offset = qual_attach_offset)
         *                       |
         *      _________________v________________
         *     |    Qualifier 1                   |
         *     |__________________________________|
         *   1300 (qual_offset)                  1500 (qual_offset + qual_size)
         *
         *   User qualifier value will be the second 100b if the current qualifier.
         *   This is if we have user qualifier, which is getting its value from just one qualifier from PBUS.
         *   In case our user qualifier is 200b, we already have first 100b and the second 100 will be taken from
         *   the next qualifier in the PBUS. Here we should calculate the starting offset of the next qualifier
         *   next_qual_offset = qual_offset + qual_size --> 1300 + 200 = 1500:
         *   Now we have 2 qualifiers (Q1 and Q2) with following parameters:
         *   Q1 --> Q1_qual_offset = 1300 | Q1_qual_size = 200
         *   Q2 --> Q2_qual_offset = 1500 | Q2_qual_size = 300
         *
         *    (next_qual_offset = qual_attach_offset)      (qual_attach_offset + user qualifier size)
         *                     1400                               1600
         *                       |                                 |
         *      _________________v________________ ________________v__________________
         *     |    Qualifier 1                   |    Qualifier 2                   |
         *     |__________________________________|__________________________________|
         *   1300 (Q1_qual_offset)               1500                               1800 (Q2_qual_offset + Q2_qual_size)
         *                      (new calculated Q2_next_qaul_offset)
         *
         *  First 100b of the user qualifier value will be taken from last 100b of the Q1 and
         *  Second 100b of the user qualifier value will be taken from the first 100b of the Q1.
         *  Same logic continue in case we have more qualifier from which we should take the data for user qualiifer.
         */
        if ((qual_offset <= next_qual_offset) && ((qual_offset + qual_size) > next_qual_offset))
        {
            /** Calculate the offset inside the qual_signal_val buffer, from where we will start getting the user qual value. */
            buff_offset = next_qual_offset - qual_offset;
            /** Calculate the size inside the qual_signal_val buffer, how many bits we will take from the current qualifier. */
            if ((qual_size - buff_offset) <= user_qual_size)
            {
                buff_size = qual_size - buff_offset;
            }
            else
            {
                buff_size = user_qual_size;
            }
            /**
             * Extract buffer size from user qualifier size, to see if any bits left from the user_qual_size.
             * If yes we should take them from the next qualifier in the PBUS.
             */
            user_qual_size = user_qual_size - buff_size;
            /** Calculate the next qualifier offset in the PBUS. */
            next_qual_offset = qual_offset + qual_size;
            /**
             * Indicates that we found correct qualifier from which we will extract the data for user qualifier value.
             * The flag will be used to get signal values for the current qualifier.
             */
            qual_found = TRUE;
            /**
             * Set the qual_index to 0, because we take the offset of the current qualifier (qual_offset) from the DNX data.
             * It can be that the qualifiers there are not ordered by offset. Every time we start from the first qualifier and
             * we are searching for the calculated new_qual_offset.
             */
            qual_index = 0;
        }

        /** In case qual_found is TRUE, we can retrieve signal data for the current qualifier. */
        if (qual_found)
        {
            uint32 qual_signal_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
            uint32 curr_qual_val[DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY] = { 0 };
            /** Get the current qualifier signal value as raw data. */
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_qual_signal_value_raw_data_get(unit, core_id, block_p, field_stage, pbus_dnx_qual, qual_signal_val));

            /**
             * Get the exact value from the returned qual_signal_val buffer.
             * For this purpose we are using already calculated buff_offset and buff_size.
             */
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(qual_signal_val, buff_offset, buff_size, curr_qual_val));

            /** Stick together all the values, of the found qualifiers, in the out qual value buffer. Will be converted to string at the end. */
            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field(curr_qual_val, out_qual_value_buff_offset, buff_size, out_qual_value_buff));

            /**
             * On every new found qualifier increase the out_qual_value_buff_offset, to know where we should place the next one.
             * At the end out_qual_value_buff_offset should be equal to user_qual_info.size.
             */
            out_qual_value_buff_offset += buff_size;

            /** Reset the qual_found flag. */
            qual_found = FALSE;
        }
    }

    if (out_qual_value_buff_offset != user_qual_info.size)
    {
        LOG_CLI_EX("\r\n" "The calculated out_qual_value_buff_offset (%d) is not the same as given user qualifier size (%d)!! %s%s\r\n\n",
                   out_qual_value_buff_offset, user_qual_info.size, EMPTY, EMPTY);
        SHR_EXIT();
    }

    /** Set the right, max value to iterate on, depends on current qual_size and max number of bits in FFC. */
    max_val_iter = (user_qual_info.size / max_num_bits_in_ffc);
    if ((max_val_iter % max_num_bits_in_ffc) != 0)
    {
        max_val_iter += 1;
    }
    else
    {
        max_val_iter = DNX_DATA_MAX_FIELD_ENTRY_NOF_QUAL_PARAMS_PER_ENTRY - 4;
    }

    for (val_iter = max_val_iter - 1; val_iter >= 0 && out_qual_value_buff_offset; val_iter--)
    {
        char val_buff[DIAG_DNX_FIELD_UTILS_STR_SIZE] = { 0 };
        int print_size = 0;
        /**
         * Array of strings, which are presenting the format in which the string will be written to val_buff.
         * Depends on current buffer size. Indicated how many nibbles we will present in the output sting.
         * For example:
         *      If we have print_size == 9b, we will need 3 nibbles to present the value 0x123
         *          print_size == 6b ---> number of nibbles == 2
         *          print_size == 32b ---> number of nibbles == 8
         *          print_size == 16b ---> number of nibbles == 4
         *          print_size == 23b ---> number of nibbles == 6
         *
         *      Calculation of number of nibbles per given buffer size is done using
         *      ((print_size + 3) / 4)
         *
         *      The result from the above macro is also used as array index in string_format[((print_size + 3) / 4) - 1].
         */
        char string_format[SAL_UINT8_NOF_BITS][5] = { "%01X", "%02X", "%03X", "%04X", "%05X", "%06X", "%07X", "%08X" };
        if ((out_qual_value_buff_offset % SAL_UINT32_NOF_BITS) != 0)
        {
            print_size = (out_qual_value_buff_offset % SAL_UINT32_NOF_BITS);
        }
        else
        {
            print_size = SAL_UINT32_NOF_BITS;
        }
        sal_snprintf(val_buff, sizeof(val_buff), string_format[((print_size + 3) / 4) - 1], out_qual_value_buff[val_iter]);
        sal_strncat(out_qual_value, val_buff, DIAG_DNX_FIELD_UTILS_STR_SIZE - 1);

        out_qual_value_buff_offset -= print_size;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
