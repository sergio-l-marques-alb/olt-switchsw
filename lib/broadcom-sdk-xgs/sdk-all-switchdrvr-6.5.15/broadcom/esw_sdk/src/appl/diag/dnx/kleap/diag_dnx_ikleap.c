/** \file diag_dnx_ikleap.c
 *
 * Main diagnostics for dbal applications All CLI commands, that
 * are related to KLEAP, are gathered in this file.
 *
 */
/*
 * $Copyright:.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGKLEAPDNX

/*************
 * INCLUDES  *
 *************/
#include "diag_dnx_ikleap.h"
#include <include/bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <include/bcm_int/dnx/kbp/kbp_mngr.h>
extern stage_t ikleap_stages[KLEAP_NOF_STAGES];
extern fixed_key_lookup_t fixed_key_lookup[NOF_FIXED_KEY_LOOKUP];

/*************
 * DEFINES   *
 *************/


#define STAGE_INFO ikleap_stages[stage_index]

#define DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(__local_handle_id__) \
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &__local_handle_id__))

#define DIAG_IKLEAP_SKIP_STAGE                                                     \
    (all_stages_flag && (stage_index > KLEAP_STAGE_FWD12)) /* skip PMF */ ||       \
    (!all_stages_flag && ((matched_stage_idx != stage_index) || (matched_sub_stage_idx != sub_stage)))

#define DIAG_IKLEAP_SKIP_KBR_CHECK                                                                  \
{                                                                                                   \
    uint32 kbr_stage = 0;                                                                           \
    if (STAGE_INFO.stage_type != SINGLE_STAGE_TYPE)                                                 \
    {                                                                                               \
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get                                         \
                        (unit, rsrcs_map_handle_id, DBAL_FIELD_KBR_SELECTOR, kbr_idx, &kbr_stage)); \
    }                                                                                               \
    if (kbr_stage != sub_stage)                                                                     \
    {                                                                                               \
        continue;                                                                                   \
    }                                                                                               \
    if ((phy_db_id != DBAL_PHYSICAL_TABLE_NONE) && (kbr2phy_mapping[kbr_idx] != phy_db_id))         \
    {                                                                                               \
        continue;                                                                                   \
    }                                                                                               \
}

#define DIAG_IKLEAP_SKIP_KBR_CHECK_2                                        \
{                                                                           \
    uint8 kbr_skip = FALSE, fixed_key_i = 0;                                \
    if (!utilex_bitstream_have_one_in_range(ffc_bitmap, 0, nof_ffc - 1))    \
    {                                                                       \
        has_ffc = FALSE;                                                    \
        kbr_skip = TRUE;                                                    \
    }                                                                       \
    for(fixed_key_i = 0; fixed_key_i < NOF_FIXED_KEY_LOOKUP; fixed_key_i++) \
    {                                                                       \
        int kbr_i;                                                          \
        for(kbr_i = 0; kbr_i < fixed_key_lookup[fixed_key_i].nof_interfaces; kbr_i++) \
        {                                                                   \
            if ((stage_index == fixed_key_lookup[fixed_key_i].stage) &&     \
                (sub_stage == fixed_key_lookup[fixed_key_i].sub_stage) &&   \
                (kbr_idx == fixed_key_lookup[fixed_key_i].kbr_idx[kbr_i]))  \
            {                                                               \
                fixed_lookup_key_indication = TRUE;                         \
                fixed_lookup_key_index = fixed_key_i;                       \
                kbr_skip = FALSE;                                           \
            }                                                               \
        }                                                                   \
    }                                                                       \
    if(kbr_skip)                                                            \
    {                                                                       \
        continue;                                                           \
    }                                                                       \
}

#define DIAG_IKLEAP_CHECK_APP_DB_ID(____kbr_has_app_db_id____)                              \
{                                                                                           \
    dbal_table_field_info_t field_info;                                                     \
    rv = dbal_tables_field_info_get_no_err(unit, STAGE_INFO.dbal_table_kbr_info,            \
                                           DBAL_FIELD_APP_DB_ID, FALSE, 0, 0, &field_info); \
    if (rv == _SHR_E_NONE)                                                                  \
    {                                                                                       \
        ____kbr_has_app_db_id____ = TRUE;                                                   \
    }                                                                                       \
    else if (rv == _SHR_E_NOT_FOUND)                                                        \
    {                                                                                       \
        ____kbr_has_app_db_id____ = FALSE;                                                  \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected Error from DBAL, searching for APP_DB_ID in table\n"); \
    }                                                                                       \
    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)                                \
    {                                                                                       \
        ____kbr_has_app_db_id____ = FALSE;                                                  \
    }                                                                                       \
}

#define DIAG_IKLEAP_KBP_ALIGNER_CHECK                                                               \
do                                                                                                  \
{                                                                                                   \
    if (STAGE_INFO.kbr2physical[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)                                \
    {                                                                                               \
        uint32 fwd12_aligner_mapper_handle_id;                                                      \
        uint32 kbp_kbr_idx = STAGE_INFO.kbr_if_id[kbr_idx];                                         \
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_ALIGNER_MAPPING,             \
                                          &fwd12_aligner_mapper_handle_id));                        \
        dbal_entry_key_field32_set(unit, fwd12_aligner_mapper_handle_id, DBAL_FIELD_KBP_KBR_IDX,    \
                                   kbp_kbr_idx);                                                    \
        dbal_entry_key_field32_set(unit, fwd12_aligner_mapper_handle_id, context_profile_field,     \
                                   context);                                                        \
        SHR_IF_ERR_EXIT(dbal_entry_get                                                              \
                        (unit, fwd12_aligner_mapper_handle_id, DBAL_GET_ALL_FIELDS));               \
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get                                         \
                        (unit, fwd12_aligner_mapper_handle_id, DBAL_FIELD_FWD_KEY_SIZE, INST_SINGLE,\
                         &kbp_aligner_key_size));                                                   \
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, fwd12_aligner_mapper_handle_id));                    \
    }                                                                                               \
}while(0)

#define DIAG_IKLEAP_PRINT_LOGICAL_TABLE_TO_PRT                                  \
{                                                                               \
    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_NONE)                   \
    {                                                                           \
        PRT_CELL_SET("%s", "Direct Extraction(-)");                             \
    }                                                                           \
    else if (diag_dnx_kleap_stage_is_pmf(unit, stage_index))                    \
    {                                                                           \
        if (kbr_has_app_db_id)                                                  \
        {                                                                       \
            PRT_CELL_SET("%s%d", "PMF,APP_DB_ID = ", app_db_id);                \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            PRT_CELL_SET("%s", "PMF,No APP_DB_ID");                             \
        }                                                                       \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        /**  Here if it's non PMF stage */                                      \
        if (kbr_has_app_db_id)                                                  \
        {                                                                       \
            /** this case in this case we provide the logical table using app_db_id */ \
            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get                    \
                            (unit, kbr2phy_mapping[kbr_idx], app_db_id, &logical_table_for_kbr));\
            PRT_CELL_SET("%s(%s,%d)", dbal_logical_table_to_string(unit, logical_table_for_kbr),\
                         phy_db_name, app_db_id);                               \
        }                                                                       \
        else if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)           \
        {                                                                       \
            uint8 opcode_id;                                                    \
            char  opcode_name[DBAL_MAX_STRING_LENGTH];                          \
            SHR_IF_ERR_EXIT(kbp_mngr_context_to_opcode_get(unit, acl_context, acl_context, &opcode_id, opcode_name)); \
            PRT_CELL_SET("KBP,\n opcode %s (%d)", opcode_name, opcode_id);        \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            SHR_ERR_EXIT(_SHR_E_CONFIG,                                         \
                         "not a PMF stage & non KBP KBR & app_db_id is non present - non legal state\n"\
                         "sub-stage: %s, context: %d, kbr: %d\n", stage_name, context, kbr_idx);\
        }                                                                       \
    }                                                                           \
}

#define DIAG_IKLEAP_VIS_SET_CONTEXT_AND_NASID                                   \
{                                                                               \
    uint8 to_skip = FALSE;                                                      \
    uint32 current_stage_index;                                                 \
    rhlist_t *sig_list_context_nasid;                                           \
    signal_output_t *signal_output_context_nasid;                               \
    current_stage_index = STAGE_INFO.stage_index_in_pipe[sub_stage];            \
    sig_list_context_nasid = NULL;                                              \
    context = 0;                                                                \
    nasid = 0;                                                                  \
    if ((sig_list_context_nasid =                                               \
        utilex_rhlist_create("signal sig_list_context_nasid", sizeof(signal_output_t), 0)) == NULL)\
    {                                                                           \
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal sig_list_context_nasid\n");\
    }                                                                           \
    if (stage_index != KLEAP_STAGE_VT1)                                         \
    {                                                                           \
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, 0, "", "", stage_name, "Nasid", &sig_list_context_nasid));\
        signal_output_context_nasid = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list_context_nasid);\
        nasid = signal_output_context_nasid->value[0];                          \
    }                                                                           \
    if ((current_stage_index != 0) && (nasid > current_stage_index))            \
    {                                                                           \
        LOG_CLI((BSL_META("NASID is set to %d, stage %s(%d) is skipped\n"), nasid, stage_name, current_stage_index));\
        to_skip = TRUE;                                                         \
    }                                                                           \
    if(!to_skip)                                                                \
    {                                                                           \
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, 0, "", "", stage_name, "Context", &sig_list_context_nasid));\
        signal_output_context_nasid = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list_context_nasid);\
        context = signal_output_context_nasid->value[0];                        \
        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, 0, "", "FWD2", "", "ACL_Context", &sig_list_context_nasid));\
        signal_output_context_nasid = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list_context_nasid);\
        acl_context = signal_output_context_nasid->value[0];                    \
        utilex_rhlist_free_all(sig_list_context_nasid);                         \
    }                                                                           \
    else                                                                        \
    {                                                                           \
        utilex_rhlist_free_all(sig_list_context_nasid);                         \
        continue;                                                               \
    }                                                                           \
}

#define DIAG_IKELAP_COPY_STAGE_INFO_TO_LOCAL_PARAMS                             \
do                                                                              \
{                                                                               \
    context_profile_field = STAGE_INFO.dbal_context_profile_field_per_stage;    \
    kbr_idx_field = STAGE_INFO.dbal_kbr_idx_field_per_stage;                    \
    kbr2phy_mapping = STAGE_INFO.kbr2physical;                                  \
    nof_kbr = STAGE_INFO.nof_kbrs;                                              \
    nof_ffc = STAGE_INFO.nof_ffc;                                               \
}while (0)


/*************
 * TYPEDEFS  *
 *************/
static char key_signal_str[DIAG_DNX_KLEAP_MAX_STRING_LENGTH];
static char res_signal_str[DIAG_DNX_KLEAP_MAX_STRING_LENGTH];


#define IKEAP_KAPS_STRENGTH_WIDTH   3
/*************
 * FUNCTIONS *
 *************/
/*
 * \brief - check if stage is a PMF stage or not. return TRUE if PMF stage, otherwise FALSE
 */
static uint8
diag_dnx_kleap_stage_is_pmf(
    int unit,
    kleap_stages_e stage_name)
{
    if ((stage_name == KLEAP_STAGE_IPMF1) || (stage_name == KLEAP_STAGE_IPMF2)
        || (stage_name == KLEAP_STAGE_IPMF3)
        || (stage_name == KLEAP_STAGE_EPMF) || (stage_name == KLEAP_STAGE_IPMF1_INITIAL))
    {
        return TRUE;
    }
    return FALSE;
}


/*
 * \brief - update stage parameters. decide whether to run on all stages or specific stage
 */
static shr_error_e
diag_kleap_decide_stage(
    int unit,
    char *stage_name,
    int *matched_stage_idx,
    int *matched_sub_stage_idx,
    int *all_stages_flag)
{
    int stage_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(stage_name, "", DBAL_MAX_STRING_LENGTH))
    {
        *all_stages_flag = 1;
        SHR_EXIT();
    }

    for (stage_index = 0; stage_index < KLEAP_NOF_STAGES; stage_index++)
    {
        int sub_stage;
        for (sub_stage = 0; sub_stage <= STAGE_INFO.stage_type; sub_stage++)
        {
            if (!sal_strncasecmp(STAGE_INFO.kleap_sub_stages_names[sub_stage], stage_name, DBAL_MAX_STRING_LENGTH))
            {
                *matched_stage_idx = stage_index;
                *matched_sub_stage_idx = sub_stage;
                SHR_EXIT();
            }
        }
    }

    SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal Stage %s\n", stage_name);

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - update phydb id parameters. decide whether to run on all physical DBs or specific DB
 */
static shr_error_e
diag_kleap_decide_phy_db_id(
    int unit,
    char *phy_db_name,
    dbal_physical_tables_e * phy_db_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!sal_strncmp(phy_db_name, "", DBAL_MAX_STRING_LENGTH))
    {
        *phy_db_id = DBAL_PHYSICAL_TABLE_NONE;
    }
    else
    {
        SHR_IF_ERR_EXIT(dbal_physical_table_string_to_id(unit, phy_db_name, phy_db_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - update context id parameters. decide whether to run on all contexts or specific ones
 */
static shr_error_e
diag_kleap_decide_core_id(
    int unit,
    int core_id,
    int *core_id_low,
    int *core_id_high)
{
    SHR_FUNC_INIT_VARS(unit);

    if (core_id == _SHR_CORE_ALL)
    {
        *core_id_low = 0;
        *core_id_high = dnx_data_device.general.nof_cores_get(unit) - 1;
    }
    else if ((core_id < dnx_data_device.general.nof_cores_get(unit)) && (core_id >= 0))
    {
        *core_id_low = core_id;
        *core_id_high = core_id;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Core %d\n", core_id);
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
diag_kleap_vis_kbr_add_prt_columns(
    int unit,
    int stage_index,
    int sub_stage,
    int context,
    int core_id,
    prt_control_t ** prt_ctr_ptr)
{
    prt_control_t *prt_ctr = NULL;
    char context_name[DIAG_DNX_KLEAP_MAX_STRING_LENGTH];
    dbal_field_types_defs_e context_enum;
    CONST dbal_field_types_basic_info_t *context_field_type;

    SHR_FUNC_INIT_VARS(unit);

    context_enum = STAGE_INFO.dbal_context_enum[sub_stage];
    SHR_IF_ERR_EXIT(dbal_field_types_info_get(unit, context_enum, &context_field_type));

    if (context >= context_field_type->nof_enum_values)
    {
        sal_strncpy_s(context_name, "unknown context", DIAG_DNX_KLEAP_MAX_STRING_LENGTH - 1);
    }
    else
    {
        sal_strncpy_s(context_name, context_field_type->enum_val_info[context].name,
                      DIAG_DNX_KLEAP_MAX_STRING_LENGTH - 1);
    }

    /** Set the tables columns  */
    PRT_TITLE_SET("Context %s(%d), Core %d", context_name, context, core_id);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical DB (phyDB, AppDbId)");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Key values");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Result values");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Error");

    *prt_ctr_ptr = prt_ctr;
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_kleap_vis_check_core_validity(
    int unit,
    int core_id,
    uint8 *core_is_valid)
{
    int rv;
    rhlist_t *sig_list;
    signal_output_t *signal_output;

    SHR_FUNC_INIT_VARS(unit);

    sig_list = NULL;
    if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
    }

    rv = sand_signal_find(unit, core_id, 0, "", "VTT1", "VTT2", "Context", &sig_list);
    if ((rv != _SHR_E_NONE) && (rv != _SHR_E_NOT_FOUND))
    {
        SHR_IF_ERR_EXIT(rv);
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        *core_is_valid = 0;
        SHR_EXIT();
    }
    signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
    if (sal_strcmp(signal_output->print_value, "N/A") == 0)
    {
        *core_is_valid = 0;
    }
    else
    {
        *core_is_valid = 1;
    }
    utilex_rhlist_free_all(sig_list);

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
diag_dnx_ikleap_find_last_local_lif(
    int unit,
    kleap_stages_e stage_index,
    int sub_stage,
    int core_id,
    int context,
    uint8 first_kbr_printed,
    uint32 *local_in_lif,
    uint32 *lif_dbal_result_type,
    dbal_tables_e * lif_dbal_table)
{
    uint8 valid_ingress_signals_on_core = 0;
    dbal_tables_e pd_info_table;
    uint32 pd_info_handle;
    uint32 tt_pd_idx;
    uint32 kbr_idx, nof_kbrs;
    uint32 order = 0, const_strength = 0;
    int max_order_value = 0;
    uint8 default_supported;
    char *stage_name;
    int ii, lif_strength_idx;
    uint8 local_lif_found = FALSE;
    dbal_physical_tables_e dbal_phy_db;

/* *INDENT-OFF* */
#define KBR_ELEMENT 1
#define LIF_ELEMENT 0
#define NOF_ELEMENTS 2
#define MAX_SUPPORTED_STRENGTH 100
    uint32 lif_arr_by_strength[MAX_SUPPORTED_STRENGTH][NOF_ELEMENTS] = { { 0 ,0 } };
/* *INDENT-ON* */

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(local_in_lif, _SHR_E_PARAM, "local_in_lif");
    SHR_NULL_CHECK(lif_dbal_table, _SHR_E_PARAM, "lif_dbal_table");

    if ((stage_index != KLEAP_STAGE_VT1) && (stage_index != KLEAP_STAGE_VT23) && (stage_index != KLEAP_STAGE_VT45))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "last local lif is valid for VT stages only. given ikleap stage %d is not supported",
                     stage_index);
    }
    else if (stage_index == KLEAP_STAGE_VT1)
    {
        tt_pd_idx = 0;
        default_supported = TRUE;
        dbal_phy_db = DBAL_PHYSICAL_TABLE_INLIF_1;
    }
    else
    {
        default_supported = FALSE;
        dbal_phy_db = DBAL_PHYSICAL_TABLE_INLIF_2;
        if (sub_stage == 0)
        {
            tt_pd_idx = 0;
        }
        else if (sub_stage == 1)
        {
            tt_pd_idx = 2;
            /*
             * FIXME ARCHAD-135 support VTT5 inner eth default lif
             * if(stage_index == KLEAP_STAGE_VT45)
             * {
             *     default_supported = TRUE;
             * }
             *
             */
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "sub_stage %d is invalid for ikleap stage %d", sub_stage, stage_index);
        }
    }

    SHR_IF_ERR_EXIT(diag_kleap_vis_check_core_validity(unit, core_id, &valid_ingress_signals_on_core));
    if (!valid_ingress_signals_on_core)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Ingress signals are invalid for core %d, cannot find ast local lif", core_id);
    }

    pd_info_table = STAGE_INFO.dbal_table_pd_info;
    nof_kbrs = STAGE_INFO.nof_kbrs;

    SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, pd_info_table, DBAL_FIELD_PD_ORDER, FALSE, 0, 0,
                                                    &max_order_value));

    stage_name = STAGE_INFO.kleap_sub_stages_names[sub_stage];

    /*
     * First, find the KBR which performed an TT lookup with valid results, and sort by strength
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, pd_info_table, &pd_info_handle));
    dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_CONTEXT_PROFILE, context);
    dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_PD_IDX, tt_pd_idx);
    if (first_kbr_printed)
    {
        for (kbr_idx = 2; kbr_idx < nof_kbrs; kbr_idx++)
        {
            uint32 lif_strength = 0, local_lif_value = 0;
            rhlist_t *sig_list;
            signal_output_t *signal_output;
            char *phy_db_name;
            char res_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
            char *index_as_str[MAX_NUM_INTERFACES_PER_PHY_DB] = { "0", "1", "2", "3" };

            dbal_entry_key_field32_set(unit, pd_info_handle, DBAL_FIELD_KBR_IDX, kbr_idx);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, pd_info_handle, DBAL_GET_ALL_FIELDS));

            /*
             * Consider adding strength width to the calculation
             */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, pd_info_handle,
                                                                DBAL_FIELD_PD_CONST_STRENGTH,
                                                                INST_SINGLE, &const_strength));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, pd_info_handle, DBAL_FIELD_PD_ORDER, INST_SINGLE, &order));

            if (order == 0)
            {
                continue;
            }

            lif_strength = max_order_value * const_strength + order;
            if (lif_strength >= MAX_SUPPORTED_STRENGTH)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "lif strength %d is larger than supported. should be updated",
                             lif_strength);
            }

            if (lif_arr_by_strength[lif_strength][LIF_ELEMENT] != 0)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Two KBRs (%d and %d) with the same strength. cannot resolve priority",
                             kbr_idx, lif_arr_by_strength[lif_strength][KBR_ELEMENT]);
            }
            phy_db_name = dbal_physical_table_to_string(unit, STAGE_INFO.kbr2physical[kbr_idx]);

            sal_strncpy_s(res_sig_name, "Result", sizeof(res_sig_name));
            if (STAGE_INFO.kbr_if_id[kbr_idx] >= IF_0)
            {
                sal_strncat(res_sig_name, "_", 2);
                sal_strncat(res_sig_name, index_as_str[STAGE_INFO.kbr_if_id[kbr_idx]], 2);
            }

            sig_list = NULL;
            if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
            }
            SHR_IF_ERR_EXIT(sand_signal_find
                            (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, res_sig_name, &sig_list));
            signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
            local_lif_value = signal_output->value[0] >> (signal_output->size - 20);
            lif_arr_by_strength[lif_strength][KBR_ELEMENT] = kbr_idx;
            lif_arr_by_strength[lif_strength][LIF_ELEMENT] = local_lif_value;
            utilex_rhlist_free_all(sig_list);
        }
    }
    DBAL_HANDLE_FREE(unit, pd_info_handle);

    if (default_supported)
    {
        uint32 in_pp_port;
        uint32 ing_pp_port_handle;
        uint32 default_lif_value;

        /*
         * VTT1
         * IppfPhysicalCfg::VttInPpPortConfig0 vtt_in_pp_port_cfg(IppfPhysicalCfg::get_instance()->read_vtt_in_pp_port_config0(packet.get_in_port()));
         * packet.set_default_port_lif(vtt_in_pp_port_cfg.default_port_lif);
         */
        rhlist_t *sig_list;
        signal_output_t *signal_output;

        sig_list = NULL;
        if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
        }

        SHR_IF_ERR_EXIT(sand_signal_find(unit, core_id, SIGNALS_MATCH_ASIC, "", "vtt5", "", "In_Port", &sig_list));
        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
        in_pp_port = signal_output->value[0];
        utilex_rhlist_free_all(sig_list);
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &ing_pp_port_handle));
        dbal_entry_key_field32_set(unit, ing_pp_port_handle, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, ing_pp_port_handle, DBAL_FIELD_PP_PORT, in_pp_port);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, ing_pp_port_handle, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, ing_pp_port_handle,
                                                            DBAL_FIELD_DEFAULT_LIF, INST_SINGLE, &default_lif_value));
        DBAL_HANDLE_FREE(unit, ing_pp_port_handle);
        lif_arr_by_strength[0][LIF_ELEMENT] = default_lif_value;

        /*
         * VTT5
         * FIXME ARCHAD-135 support VTT5 inner eth default lif
         */
    }

    /** go over all found lifs and choose the one with the highest priority */
    for (ii = MAX_SUPPORTED_STRENGTH - 1; ii >= 0; ii--)
    {
        if (lif_arr_by_strength[ii][LIF_ELEMENT] != 0)
        {
            local_lif_found = TRUE;
            lif_strength_idx = ii;
            break;
        }
    }

    /*
     * Read the founded lif information
     */
    if (local_lif_found)
    {
        *local_in_lif = lif_arr_by_strength[lif_strength_idx][LIF_ELEMENT];
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit,
                                                                                lif_arr_by_strength[lif_strength_idx]
                                                                                [LIF_ELEMENT], core_id, dbal_phy_db,
                                                                                lif_dbal_table, lif_dbal_result_type));
    }
    else
    {
        *local_in_lif = 0;
        *lif_dbal_result_type = 0;
        *lif_dbal_table = DBAL_TABLE_EMPTY;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_ikleap_vis_build_key_and_result_strings(
    int unit,
    int context,
    int acl_context,
    uint32 *key_sig_value,
    uint32 *res_sig_value,
    int key_sig_size,
    int res_sig_size,
    char *head_key_string,
    char *head_res_string,
    dbal_physical_tables_e phyDb,
    dbal_tables_e logical_table_for_kbr,
    int core_id,
    char **updated_res_string)
{
    int word_idx, char_count;
    char *key_string = head_key_string;
    char *res_string = head_res_string;
    int meaningful_lookup_type_string_offset = sal_strlen("KBP_LOOKUP_TYPE_");

    SHR_FUNC_INIT_VARS(unit);

    /** In case phy DB is KAPS, 3b strength are added to MSB, so for DBAL parsing the result size is 3b shorter */
    if ((phyDb == DBAL_PHYSICAL_TABLE_KAPS_1) || (phyDb == DBAL_PHYSICAL_TABLE_KAPS_2))
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_reset_bit_range(res_sig_value,
                                                         res_sig_size - IKEAP_KAPS_STRENGTH_WIDTH, res_sig_size - 1));
        res_sig_size -= IKEAP_KAPS_STRENGTH_WIDTH;
    }

    char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "KEY(%db)=0x", key_sig_size);
    key_string += char_count;
    for (word_idx = BITS2WORDS(key_sig_size) - 1; word_idx >= 0; word_idx--)
    {
        if (word_idx == (BITS2WORDS(key_sig_size) - 1))
        {
            int print_width = (3 + (key_sig_size % 32)) / 4;

            if (print_width == 0)
            {
                print_width = 8;
            }
            char_count = sal_snprintf(key_string,
                                      DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%0*x", print_width, key_sig_value[word_idx]);
            key_string += char_count;
        }
        else
        {
            char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%08x", key_sig_value[word_idx]);
            key_string += char_count;
        }
    }
    char_count = sal_snprintf(key_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "\n");
    key_string += char_count;

    /** Print Res as buffer */
    char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "RES(%db)=0x", res_sig_size);
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
                                      DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%0*x", print_width, res_sig_value[word_idx]);
            res_string += char_count;
        }
        else
        {
            char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "%08x", res_sig_value[word_idx]);
            res_string += char_count;
        }
    }
    char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH, "\n");
    res_string += char_count;

    /** Start key-result parsing using DBAL */
    {
        int key_idx, res_idx;
        dbal_printable_entry_t entry_print_info;
        kbp_printable_entry_t entry_print_info_kbp[2];
        uint8 nof_print_info;
        sal_memset(&entry_print_info, 0, sizeof(dbal_printable_entry_t));
        sal_memset(entry_print_info_kbp, 0, 2 * sizeof(dbal_printable_entry_t));

        if (phyDb != DBAL_PHYSICAL_TABLE_KBP)
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(dbal_table_printable_entry_get(unit, logical_table_for_kbr,
                                                                     key_sig_value, core_id,
                                                                     res_sig_size, res_sig_value,
                                                                     &entry_print_info), _SHR_E_NOT_FOUND);
        }
        else
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(kbp_mngr_opcode_printable_entry_get(unit, 0, context, acl_context,
                                                                          key_sig_value, key_sig_size, res_sig_value,
                                                                          res_sig_size, &nof_print_info,
                                                                          entry_print_info_kbp), _SHR_E_NOT_FOUND);
        }
        if (phyDb != DBAL_PHYSICAL_TABLE_KBP)
        {
            for (key_idx = 0; key_idx < entry_print_info.nof_key_fields; key_idx++)
            {
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s ", entry_print_info.key_fields_info[key_idx].field_name);
                key_string += char_count;
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s\n", entry_print_info.key_fields_info[key_idx].field_print_value);
                key_string += char_count;
            }

            for (res_idx = 0; res_idx < entry_print_info.nof_res_fields; res_idx++)
            {
                char_count =
                    sal_snprintf(res_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s ", entry_print_info.res_fields_info[res_idx].field_name);
                res_string += char_count;
                char_count =
                    sal_snprintf(res_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s\n", entry_print_info.res_fields_info[res_idx].field_print_value);
                res_string += char_count;
            }
        }
        else
        {
            int kbp_lkp_index = 0;

            for (key_idx = 0; key_idx < entry_print_info_kbp[0].entry_print_info.nof_key_fields; key_idx++)
            {
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s ", entry_print_info_kbp[0].entry_print_info.key_fields_info[key_idx].field_name);
                key_string += char_count;
                char_count =
                    sal_snprintf(key_string,
                                 DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                 "%s\n",
                                 entry_print_info_kbp[0].entry_print_info.key_fields_info[key_idx].field_print_value);
                key_string += char_count;
            }

            for (kbp_lkp_index = 0; kbp_lkp_index < 2; kbp_lkp_index++)
            {
                if (entry_print_info_kbp[kbp_lkp_index].entry_print_info.nof_res_fields > 0)
                {
                    char_count =
                        sal_snprintf(res_string,
                                     DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                     "%s%s:\n",
                                     (entry_print_info_kbp[kbp_lkp_index].lookup_type_str +
                                      meaningful_lookup_type_string_offset), " LOOKUP");
                    res_string += char_count;
                    for (res_idx = 0; res_idx < entry_print_info_kbp[kbp_lkp_index].entry_print_info.nof_res_fields;
                         res_idx++)
                    {
                        char_count =
                            sal_snprintf(res_string,
                                         DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                         "%s ",
                                         entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                         res_fields_info[res_idx].field_name);
                        res_string += char_count;
                        char_count =
                            sal_snprintf(res_string,
                                         DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                         "%s\n",
                                         entry_print_info_kbp[kbp_lkp_index].entry_print_info.
                                         res_fields_info[res_idx].field_print_value);
                        res_string += char_count;
                    }
                }
            }
        }
    }

    *updated_res_string = res_string;

exit:
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

static sh_sand_man_t dnx_ikleap_visib_man = {.brief = "Show last packet key and payload per lookup",
    .full = "Show last packet key and payload per lookup",
    .synopsis = "[stage=<fwd1(2)/vtt1(2,3,4,5)>] [PhyDB=<phyDbName>]",
    .examples = "stage=fwd1 phyDB=LEM\n" "stage=vtt1 silence=Yes\n" "stage=fwd1 SHORT"
};
static sh_sand_option_t dnx_ikleap_visib_options[] = {
    {"STAGE", SAL_FIELD_TYPE_STR, "KLEAP stage", ""},
    {"PhyDb", SAL_FIELD_TYPE_STR, "Physical DB", ""},
    {"silence", SAL_FIELD_TYPE_BOOL, NULL, "Yes"},
    {"SHORT", SAL_FIELD_TYPE_BOOL, "Minimized Diagnostics", "No"},
    {NULL}
};

/*
 * \brief - Diagnostic to see the key build instruction for ingress KLEAP lookups.
 * Shows key, result, hit indication
 */
static shr_error_e
sh_dnx_ikleap_visib_cmds(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     *  Diagnostic inputs
     */
    char *stage_name = NULL;
    char *phy_db_name = NULL;

    /*
     * Diagnostic flow control parameters, resolved from inputs
     */
    uint32 stage_index, sub_stage;
    int all_stages_flag = 0, matched_stage_idx = 0, matched_sub_stage_idx = 0;
    int core = 0, core_id = 0, core_high = 0, core_low = 0;
    dbal_physical_tables_e phy_db_id;

    /*
     * DBAL handlers to hard logic tables, used to read info
     */
    uint32 rsrcs_map_handle_id;
    uint32 kbr_info_handle_id;

    /*
     * HW parameters per stage
     */
    int nof_kbr;
    int nof_ffc;
    dbal_fields_e kbr_idx_field;
    dbal_fields_e context_profile_field;
    dbal_physical_tables_e *kbr2phy_mapping;

    /** Local parameters for saving multiple data per KBR */
    int rv = 0, ii;
    uint32 kbr_idx = 0;

    /** usually used for intergration in a larger diagnostics commands such as 'diag pp vis last' */
    uint8 short_mode = FALSE;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR("STaGe", stage_name);
    SH_SAND_GET_STR("PhyDb", phy_db_name);
    SH_SAND_GET_INT32("Core", core);
    SH_SAND_GET_BOOL("SHORT", short_mode);


    /*
     * Update diagnostic control parameters according to inputs
     */
    SHR_IF_ERR_EXIT(diag_kleap_decide_stage(unit, stage_name, &matched_stage_idx,
                                            &matched_sub_stage_idx, &all_stages_flag));
    SHR_IF_ERR_EXIT(diag_kleap_decide_phy_db_id(unit, phy_db_name, &phy_db_id));
    SHR_IF_ERR_EXIT(diag_kleap_decide_core_id(unit, core, &core_low, &core_high));

    DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(kbr_info_handle_id);
    DIAG_DNX_IKLEAP_DUMMY_DBAL_TABLE_ALLOC(rsrcs_map_handle_id);

    for (core_id = core_low; core_id <= core_high; core_id++)
    {
        uint8 valid_ingress_signals_on_core = 0;
        SHR_IF_ERR_EXIT(diag_kleap_vis_check_core_validity(unit, core_id, &valid_ingress_signals_on_core));
        if (!valid_ingress_signals_on_core)
        {
            /** print out only if not in a SHORT mode */
            if (!short_mode)
            {
                LOG_CLI((BSL_META("Ingress signals are invalid for core %d\n"), core_id));
            }
            continue;
        }

        for (stage_index = 0; stage_index < KLEAP_NOF_STAGES; stage_index++)
        {
            for (sub_stage = 0; sub_stage <= STAGE_INFO.stage_type; sub_stage++)
            {
                uint32 context, acl_context;
                uint32 nasid;
                uint8 first_kbr_printed = FALSE;

                if (DIAG_IKLEAP_SKIP_STAGE)
                {
                    continue;
                }
                stage_name = STAGE_INFO.kleap_sub_stages_names[sub_stage];

                            /** Init Hw parameters (per stage) */
                DIAG_IKELAP_COPY_STAGE_INFO_TO_LOCAL_PARAMS;

                            /** Alloc handles for DBAL tables */
                SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, STAGE_INFO.dbal_table_kbr_info, kbr_info_handle_id));

                            /** Get the resources configuration of the stage */
                if (STAGE_INFO.stage_type != SINGLE_STAGE_TYPE)
                {
                    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR
                                    (unit, STAGE_INFO.dbal_table_resource_mapping, rsrcs_map_handle_id));
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, rsrcs_map_handle_id, DBAL_GET_ALL_FIELDS));
                }

                DIAG_IKLEAP_VIS_SET_CONTEXT_AND_NASID;
                SHR_IF_ERR_EXIT(diag_kleap_vis_kbr_add_prt_columns
                                (unit, stage_index, sub_stage, context, core_id, &prt_ctr));

                /*
                 *  Loop over all KBR of the stage
                 *  only the valid ones should be printed
                 */
                for (kbr_idx = 2; kbr_idx < nof_kbr; kbr_idx++)
                {
                    int nof_key_signals_to_concatanate = 1, first_interface_in_key = 0;
                    uint8 kbr_has_app_db_id;
                    uint8 fixed_lookup_key_indication = FALSE;
                    uint8 fixed_lookup_key_index = 0;
                    uint32 app_db_id = 0;
                    uint8 has_ffc = TRUE;
                    uint32 ffc_bitmap[DIAG_DNX_KLEAP_FFC_U32_BITMAP_SIZE] = { 0 };
                    dbal_tables_e logical_table_for_kbr = DBAL_NOF_TABLES;
                    uint32 kbp_aligner_key_size = 0;

                    /*
                     * Signals call variables
                     */
                    char *key_string = NULL, *res_string = NULL;
                    uint32 key_sig_size = 0, res_sig_size = 0;
                    uint32 key_sig_value[DSIG_MAX_SIZE_UINT32] = { 0 }, *res_sig_value = NULL, *app_sig_value =
                        NULL, *hit_sig_value = NULL, *err_sig_value = NULL;
                    char key_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
                    char res_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
                    char app_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
                    char hit_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
                    char err_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };
                    char error_sig_name[SIGNALS_MAX_NAME_LENGTH] = { "" };

                    char *index_as_str[MAX_NUM_INTERFACES_PER_PHY_DB] = { "0", "1", "2", "3" };
                    char signal_prefix[2] = { "" };
                    char *phy_db_name = NULL;
                    rhlist_t *sig_list = NULL;
                    signal_output_t *signal_output = NULL;

                    DIAG_IKLEAP_SKIP_KBR_CHECK;
                    DIAG_IKLEAP_CHECK_APP_DB_ID(kbr_has_app_db_id);

                    sig_list = NULL;
                    if ((sig_list = utilex_rhlist_create("signal", sizeof(signal_output_t), 0)) == NULL)
                    {
                        SHR_ERR_EXIT(_SHR_E_MEMORY, "No memory for signal\n");
                    }

                    phy_db_name = dbal_physical_table_to_string(unit, kbr2phy_mapping[kbr_idx]);

                    /*
                     * Get the KBR info, AppDbId and FFC bitmap
                     */
                    dbal_entry_key_field32_set(unit, kbr_info_handle_id, context_profile_field, context);
                    dbal_entry_key_field32_set(unit, kbr_info_handle_id, kbr_idx_field, kbr_idx);

                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, kbr_info_handle_id, DBAL_GET_ALL_FIELDS));
                    if (kbr_has_app_db_id)
                    {
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, kbr_info_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, &app_db_id));
                    }
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                    (unit, kbr_info_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bitmap));

                    DIAG_IKLEAP_SKIP_KBR_CHECK_2;

                    first_kbr_printed = TRUE;
                    if (fixed_lookup_key_indication)
                    {
                        /*
                         * check if the lookup is enabled per context
                         */
                        uint32 context_handle_id;
                        uint32 enable_bit = 0;
                        dbal_tables_e table = fixed_key_lookup[fixed_lookup_key_index].context_enablers_table;
                        dbal_fields_e key_field = fixed_key_lookup[fixed_lookup_key_index].key_field;
                        dbal_fields_e enabler_field = fixed_key_lookup[fixed_lookup_key_index].context_enablers_field;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table, &context_handle_id));
                        dbal_entry_key_field32_set(unit, context_handle_id, key_field, context);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, context_handle_id, DBAL_GET_ALL_FIELDS));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, enabler_field, INST_SINGLE, &enable_bit));
                        DBAL_HANDLE_FREE(unit, context_handle_id);
                        if (enable_bit)
                        {
                            nof_key_signals_to_concatanate = fixed_key_lookup[fixed_lookup_key_index].nof_interfaces;
                            first_interface_in_key =
                                STAGE_INFO.kbr_if_id[fixed_key_lookup[fixed_lookup_key_index].kbr_idx
                                                     [nof_key_signals_to_concatanate - 1]];
                            sal_strncpy_s(key_sig_name, fixed_key_lookup[fixed_lookup_key_index].key_sig_name,
                                          sizeof(key_sig_name));
                            sal_strncpy_s(app_sig_name, fixed_key_lookup[fixed_lookup_key_index].app_sig_name,
                                          sizeof(app_sig_name));
                        }
                        else
                        {
                            fixed_lookup_key_indication = FALSE;
                            if (!has_ffc)
                            {
                                continue;
                            }
                        }
                    }
                    else
                    {
                        if (!has_ffc)
                        {
                            continue;
                        }
                    }

                    if (!fixed_lookup_key_indication)
                    {
                        sal_strncpy_s(key_sig_name, "Key", sizeof(key_sig_name));
                        sal_strncpy_s(app_sig_name, "AppDb", sizeof(app_sig_name));
                    }

                    if (!fixed_lookup_key_indication)
                    {
                        if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_TCAM)
                        {
                            CONST dbal_logical_table_t *dbal_table;
                            SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get
                                            (unit, kbr2phy_mapping[kbr_idx], app_db_id, &logical_table_for_kbr));

                            SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, logical_table_for_kbr, &dbal_table));
                            if (dbal_table->key_size > 160)
                            {
                                /*
                                 *  Need to:
                                 *  1. read two keys and concatenate
                                 *  2. check which interface holds the result/hit/error
                                 */
                                if ((kbr_idx % 2) == 1)
                                {
                                    continue;
                                }
                                nof_key_signals_to_concatanate = 2;
                                first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx + 1];
                            }
                            else
                            {
                                first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx];
                            }
                        }
                        else if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                        {
                            /*
                             *  Need to:
                             *  1. read X keys and concatenate - according to aligner
                             *  2. check which interface holds the result/hit/error
                             */
                            DIAG_IKLEAP_KBP_ALIGNER_CHECK;

                            if (kbr_idx != 4)
                            {
                                continue;
                            }
                            nof_key_signals_to_concatanate = 4;
                            first_interface_in_key = 0;
                        }
                        else
                        {
                            first_interface_in_key = STAGE_INFO.kbr_if_id[kbr_idx];
                        }
                    }

                    sal_strncpy_s(signal_prefix, "_", sizeof(signal_prefix));
                    sal_strncpy_s(res_sig_name, "Result", sizeof(res_sig_name));
                    sal_strncpy_s(err_sig_name, "Err", sizeof(err_sig_name));
                    sal_strncpy_s(error_sig_name, "Error", sizeof(error_sig_name));
                    sal_strncpy_s(hit_sig_name, "Hit", sizeof(hit_sig_name));

                    if (first_interface_in_key == INVALID_IF)
                    {
                        continue;
                    }

                    if (first_interface_in_key >= IF_0)
                    {
                        sal_strncat(signal_prefix, index_as_str[first_interface_in_key], 1);
                        sal_strncat(key_sig_name, signal_prefix, sizeof(signal_prefix));
                        sal_strncat(res_sig_name, signal_prefix, sizeof(signal_prefix));
                        sal_strncat(hit_sig_name, signal_prefix, sizeof(signal_prefix));
                        sal_strncat(err_sig_name, signal_prefix, sizeof(signal_prefix));
                        sal_strncat(error_sig_name, signal_prefix, sizeof(signal_prefix));
                        sal_strncat(app_sig_name, signal_prefix, sizeof(signal_prefix));
                    }

                    if (kbr_has_app_db_id)
                    {
                        /*
                         * Get the APP_DB_ID sent signal
                         */
                        SHR_IF_ERR_EXIT(sand_signal_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", stage_name, phy_db_name,
                                         app_sig_name, &sig_list));
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
                        app_sig_value = signal_output->value;
                        if ((app_sig_value[0] != app_db_id) && (nof_key_signals_to_concatanate == 1))
                        {
                            if (app_sig_value[0] == 0)
                            {
                                /*
                                 * Probably not a real lookup, print info message and continue
                                 */
                                LOG_CLI((BSL_META
                                         ("The sent App Db Id (%d) is not equal to the App Db Id (%d) on the KBR. stage  %s, kbr idx = %d\n"),
                                         app_sig_value[0], app_db_id, stage_name, kbr_idx));
                                continue;
                            }
                            else
                            {
                                SHR_ERR_EXIT(_SHR_E_CONFIG,
                                             "The sent App Db Id (%d) is not equal to the App Db Id (%d) on the KBR. stage  %s, kbr idx = %d\n",
                                             app_sig_value[0], app_db_id, stage_name, kbr_idx);
                            }
                        }
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    DIAG_IKLEAP_PRINT_LOGICAL_TABLE_TO_PRT;

                    sal_strncpy_s(key_signal_str, EMPTY, 1);
                    sal_strncpy_s(res_signal_str, EMPTY, 1);
                    key_string = &key_signal_str[0];
                    res_string = &res_signal_str[0];

                    /*
                     * Get the KEY sent signal
                     */
                    for (ii = 0; ii < nof_key_signals_to_concatanate; ii++)
                    {
                        if (ii != 0)
                        {
                            if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                            {
                                key_sig_name[sal_strlen(key_sig_name) - 1] = '0' + ii;
                            }
                            else
                            {
                                key_sig_name[sal_strlen(key_sig_name) - 1] = '0' + first_interface_in_key - ii;
                            }
                        }
                        SHR_IF_ERR_EXIT(sand_signal_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", stage_name, phy_db_name, key_sig_name,
                                         &sig_list));
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);

                        if (ii == 0)
                        {
                            SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                            (signal_output->value, 0, signal_output->size, key_sig_value));
                            key_sig_size = signal_output->size;
                        }
                        else
                        {
                            uint32 temp_key_sig_value[DSIG_MAX_SIZE_UINT32] = { 0 };

                            if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                            {
                                uint32 aligner_size_in_bits = BYTES2BITS(kbp_aligner_key_size);

                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (key_sig_value, 0, key_sig_size, temp_key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (temp_key_sig_value, aligner_size_in_bits, key_sig_size,
                                                 key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (signal_output->value, signal_output->size - aligner_size_in_bits,
                                                 aligner_size_in_bits, key_sig_value));
                                key_sig_size += aligner_size_in_bits;
                            }
                            else
                            {
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (key_sig_value, 0, key_sig_size, temp_key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (temp_key_sig_value, signal_output->size, key_sig_size, key_sig_value));
                                SHR_IF_ERR_EXIT(utilex_bitstream_set_any_field
                                                (signal_output->value, 0, signal_output->size, key_sig_value));
                                key_sig_size += signal_output->size;
                            }
                        }
                    }

                    /*
                     * Get the RESULT sent signal
                     */
                    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                    {
                        SHR_IF_ERR_EXIT(sand_signal_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", "FWD2", "IPMF1", "Elk_Lkp_Payload",
                                         &sig_list));
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
                        res_sig_value = signal_output->value;
                        res_sig_size = signal_output->size;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(sand_signal_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, res_sig_name,
                                         &sig_list));
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
                        res_sig_value = signal_output->value;
                        res_sig_size = signal_output->size;

                        /*
                         * Get the HIT sent signal
                         */
                        SHR_IF_ERR_EXIT(sand_signal_find
                                        (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name, hit_sig_name,
                                         &sig_list));
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
                        hit_sig_value = signal_output->value;

                        /*
                         * Get the ERROR sent signal
                         */
                        rv = sand_signal_find(unit, core_id, SIGNALS_MATCH_ASIC, "",
                                              phy_db_name, stage_name, err_sig_name, &sig_list);
                        if (rv == _SHR_E_NOT_FOUND)
                        {
                            SHR_IF_ERR_EXIT(sand_signal_find
                                            (unit, core_id, SIGNALS_MATCH_ASIC, "", phy_db_name, stage_name,
                                             error_sig_name, &sig_list));
                        }
                        else
                        {
                            SHR_IF_ERR_EXIT(rv);
                        }
                        signal_output = (signal_output_t *) utilex_rhlist_entry_get_last(sig_list);
                        err_sig_value = signal_output->value;
                    }

                    SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                    (unit, context, acl_context, key_sig_value, res_sig_value, key_sig_size,
                                     res_sig_size, key_string, res_string, kbr2phy_mapping[kbr_idx],
                                     logical_table_for_kbr, core_id, &res_string));

                    /*
                     *  Print info to table:
                     *  KEY - always printed
                     *  RESULT -
                     *      if hit indication is valid, print according to hit indication
                     *      if hit indication is invalid (sub stage == 0), print result with warning
                     *  ERR - printed only if exists
                     */
                    PRT_CELL_SET("%s", &key_signal_str[0]);
                    if (kbr2phy_mapping[kbr_idx] == DBAL_PHYSICAL_TABLE_KBP)
                    {
                        PRT_CELL_SET("%s", &res_signal_str[0]);
                    }
                    else if (sub_stage == 1)
                    {
                        if (hit_sig_value[0] != 0)
                        {
                            PRT_CELL_SET("%s", &res_signal_str[0]);
                        }
                        else
                        {
                            PRT_CELL_SET("%s", "No Match");
                        }
                        PRT_CELL_SET("%s", err_sig_value[0] == 0 ? " " : "Err!");
                    }
                    else
                    {
                        int char_count = sal_snprintf(res_string, DIAG_DNX_KLEAP_MAX_STRING_LENGTH,
                                                      "\n%s\n", "Unknown Match");
                        res_string += char_count;
                        PRT_CELL_SET("%s", &res_signal_str[0]);
                        PRT_CELL_SET("%s", err_sig_value[0] == 0 ? " " : "Err!");
                    }
                    utilex_rhlist_free_all(sig_list);
                }/** of for on KBR*/

                /*
                 * Additional fixed lookups: Lif and VSI
                 * VT stages only
                 */
                {
                    uint8 is_vt_stage = FALSE;
                    uint32 context_handle_id;
                    dbal_tables_e context_table;
                    dbal_fields_e context_field;
                    uint32 vsi_enabler = 0, lif_enabler = 0;

                    /*
                     * check that the vsi lookup enabler is enable
                     */
                    if (stage_index == KLEAP_STAGE_VT1)
                    {
                        context_table = DBAL_TABLE_INGRESS_IRPP_VT1_CONTEXT_PROPERTIES;
                        context_field = DBAL_FIELD_VT1_CONTEXT_ID;
                        is_vt_stage = TRUE;
                    }
                    else if (stage_index == KLEAP_STAGE_VT23)
                    {
                        if (sub_stage == 0)
                        {
                            context_table = DBAL_TABLE_INGRESS_IRPP_VT2_CONTEXT_PROPERTIES;
                            context_field = DBAL_FIELD_VT2_CONTEXT_ID;
                        }
                        else
                        {
                            context_table = DBAL_TABLE_INGRESS_IRPP_VT3_CONTEXT_PROPERTIES;
                            context_field = DBAL_FIELD_VT3_CONTEXT_ID;
                        }
                        is_vt_stage = TRUE;
                    }
                    else if (stage_index == KLEAP_STAGE_VT45)
                    {
                        if (sub_stage == 0)
                        {
                            context_table = DBAL_TABLE_INGRESS_IRPP_VT4_CONTEXT_PROPERTIES;
                            context_field = DBAL_FIELD_VT4_CONTEXT_ID;
                        }
                        else
                        {
                            context_table = DBAL_TABLE_INGRESS_IRPP_VT5_CONTEXT_PROPERTIES;
                            context_field = DBAL_FIELD_VT5_CONTEXT_ID;
                        }
                        is_vt_stage = TRUE;
                    }

                    if (is_vt_stage)
                    {
                        uint8 lif_has_vsi = FALSE;
                        char *key_string = NULL, *res_string = NULL;
                        dbal_table_field_info_t field_info;

                        uint32 local_in_lif = 0;
                        uint32 vsi_value = 0, fodo_value = 0;
                        dbal_tables_e lif_dbal_table = DBAL_TABLE_EMPTY;
                        uint32 lif_dbal_result_type = 0;
                        uint32 lif_handle_id, vsi_handle_id;
                        dbal_entry_handle_t *entry_handle;

                        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, context_table, &context_handle_id));
                        dbal_entry_key_field32_set(unit, context_handle_id, context_field, context);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, context_handle_id, DBAL_GET_ALL_FIELDS));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, DBAL_FIELD_VSI_LOOKUP_ENABLE, INST_SINGLE,
                                         &vsi_enabler));
                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                        (unit, context_handle_id, DBAL_FIELD_LIF_ARR_RESOLUTION_ENABLE, INST_SINGLE,
                                         &lif_enabler));
                        DBAL_HANDLE_FREE(unit, context_handle_id);

                        if (lif_enabler)
                        {
                            SHR_IF_ERR_EXIT(diag_dnx_ikleap_find_last_local_lif
                                            (unit, stage_index, sub_stage, core_id, context, first_kbr_printed,
                                             &local_in_lif, &lif_dbal_result_type, &lif_dbal_table));
                            if (local_in_lif != 0)
                            {
                                SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, lif_dbal_table, &lif_handle_id));
                                dbal_entry_key_field32_set(unit, lif_handle_id, DBAL_FIELD_IN_LIF, local_in_lif);
                                dbal_entry_value_field32_set(unit, lif_handle_id, DBAL_FIELD_RESULT_TYPE, 0,
                                                             lif_dbal_result_type);
                                SHR_IF_ERR_EXIT(dbal_entry_get(unit, lif_handle_id, DBAL_GET_ALL_FIELDS));
                                SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, lif_handle_id, &entry_handle));

                                sal_strncpy_s(key_signal_str, EMPTY, 1);
                                sal_strncpy_s(res_signal_str, EMPTY, 1);
                                key_string = &key_signal_str[0];
                                res_string = &res_signal_str[0];

                                SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                                (unit, context, 0, entry_handle->phy_entry.key,
                                                 entry_handle->phy_entry.payload, entry_handle->phy_entry.key_size,
                                                 entry_handle->phy_entry.payload_size, key_string, res_string,
                                                 DBAL_PHYSICAL_TABLE_INLIF_1, lif_dbal_table, core_id, &res_string));

                                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                PRT_CELL_SET("%s(%s%s)", dbal_logical_table_to_string(unit, lif_dbal_table), "INLIF_",
                                             stage_index == KLEAP_STAGE_VT1 ? "1" : "2");
                                PRT_CELL_SET("%s", &key_signal_str[0]);
                                PRT_CELL_SET("%s", &res_signal_str[0]);

                                /*
                                 * check if vsi exists on lif
                                 */
                                rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_FODO, 0,
                                                                       lif_dbal_result_type, 0, &field_info);
                                if (rv == _SHR_E_NOT_FOUND)
                                {
                                    rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table, DBAL_FIELD_VSI, 0,
                                                                           lif_dbal_result_type, 0, &field_info);
                                    if (rv != _SHR_E_NOT_FOUND)
                                    {
                                        SHR_IF_ERR_EXIT(rv);
                                        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                                        (unit, lif_handle_id, DBAL_FIELD_VSI, 0, &vsi_value));
                                        lif_has_vsi = TRUE;
                                    }
                                }
                                else
                                {
                                    dbal_fields_e sub_field_id = DBAL_FIELD_EMPTY;
                                    SHR_IF_ERR_EXIT(rv);
                                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                                    (unit, lif_handle_id, DBAL_FIELD_FODO, 0, &fodo_value));
                                    SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get(unit, DBAL_FIELD_FODO,
                                                                                          fodo_value, &sub_field_id,
                                                                                          &vsi_value));
                                    if (sub_field_id == DBAL_FIELD_VSI)
                                    {
                                        lif_has_vsi = TRUE;
                                    }
                                }
                            }
                        }

                        if (vsi_enabler && lif_has_vsi)
                        {
                            rv = dbal_tables_field_info_get_no_err(unit, lif_dbal_table,
                                                                   DBAL_FIELD_FORWARD_DOMAIN_ASSIGNMENT_MODE, 0,
                                                                   lif_dbal_result_type, 0, &field_info);
                            if (rv == _SHR_E_NONE)
                            {
                                uint32 fodo_assignement_mode = 0;
                                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                                (unit, lif_handle_id, DBAL_FIELD_FORWARD_DOMAIN_ASSIGNMENT_MODE, 0,
                                                 &fodo_assignement_mode));
                                /** FIXME SDK-161167 parse vsi with assignment mode != from_lif */
                                if (fodo_assignement_mode ==
                                    DBAL_ENUM_FVAL_FORWARD_DOMAIN_ASSIGNMENT_MODE_FORWARD_DOMAIN_FROM_LIF)
                                {
                                    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                                                    (unit, DBAL_TABLE_ING_VSI_INFO_DB, &vsi_handle_id));
                                    dbal_entry_key_field32_set(unit, vsi_handle_id, DBAL_FIELD_VSI, vsi_value);
                                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, vsi_handle_id, DBAL_GET_ALL_FIELDS));
                                    SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, vsi_handle_id, &entry_handle));

                                    sal_strncpy_s(key_signal_str, EMPTY, 1);
                                    sal_strncpy_s(res_signal_str, EMPTY, 1);
                                    key_string = &key_signal_str[0];
                                    res_string = &res_signal_str[0];

                                    SHR_IF_ERR_EXIT(diag_ikleap_vis_build_key_and_result_strings
                                                    (unit, context, 0, entry_handle->phy_entry.key,
                                                     entry_handle->phy_entry.payload, entry_handle->phy_entry.key_size,
                                                     entry_handle->phy_entry.payload_size, key_string, res_string,
                                                     DBAL_PHYSICAL_TABLE_IVSI, DBAL_TABLE_ING_VSI_INFO_DB, core_id,
                                                     &res_string));

                                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                                    PRT_CELL_SET("%s(%s)", "ING_VSI_INFO_DB", "IVSI");
                                    PRT_CELL_SET("%s", &key_signal_str[0]);
                                    PRT_CELL_SET("%s", &res_signal_str[0]);
                                }
                            }
                            else if (rv != _SHR_E_NOT_FOUND)
                            {
                                SHR_IF_ERR_EXIT(rv);
                            }
                        }
                    }
                }
                PRT_COMMITX;
            }/** of for of sub_stage */
        } /** of for of stage */
    }     /** of for on Core ID*/

exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */

/**
 * \brief DNX ING KLEAP diagnostic pack
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for KLEAP diagnostic commands
 */

sh_sand_man_t sh_dnx_ikleap_info_man = {
#if 1
    .brief = "KLEAP Diagnostic - Show contexts configurations per stage",
#endif
    .full = NULL
};

sh_sand_man_t sh_dnx_ikleap_man = {
    .brief = "iKLEAP Diagnostic - Show lookups information for Ingress PP pipeline",
    .full = NULL
};

sh_sand_cmd_t sh_dnx_ikleap_cmds[] = {
    /********************************************************************************************************************************
     * CMD_NAME *     CMD_ACTION            * Next                    *        Options                 *         MAN                *
     *          *                           * Level                   *                                *                            *
     *          *                           * CMD                     *                                *                            *
     ********************************************************************************************************************************/
    {"VISibility", sh_dnx_ikleap_visib_cmds, NULL, dnx_ikleap_visib_options, &dnx_ikleap_visib_man},
    {NULL}
};
/* *INDENT-ON* */
