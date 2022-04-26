/** \file ctest_dnx_inlif_wide_data.c
 *
 * in_lif wide data unit test.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_LIF

/*****************
 * INCLUDE FILES *
 * ***************
 */
/*
 * {
 */
#include <shared/shrextend/shrextend_debug.h>

#include <bcm/types.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/lif/lif_lib.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/dnx_err_recovery_manager.h>

/**
 * }
 */

/*************
 * DEFINES   *
 *************/
/*
 * {
 */
#define CTEST_DNX_IN_LIF_WIDE_DATA_FWD1_LOOKUP_KBR_ID (8)
extern shr_error_e dbal_tables_table_by_mdb_phy_get(
    int unit,
    dbal_physical_tables_e phy_db_id,
    uint32 app_db_id,
    dbal_tables_e * table_id);
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
#if defined(BCM_DNX2_SUPPORT)
/**
 * \brief
 * Man for inlif test.
 */
sh_sand_man_t dnx_inlif_wide_data_man = {
    .brief = "verify in lif wide data (generic data) functionality",
    .full = NULL,
    .synopsis = NULL,
    .examples = "ctest lif WideData lookup"
};

static sh_sand_man_t dnx_inlif_wide_data_extension_lookup_test_man = {
    .brief = "verify wide data extension lookup is available from all FWD1 contexts ",
    .full = NULL,
    .synopsis = NULL,
    .examples = "ctest lif WideData lookup"
};

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
 * \brief
 * Semantic test to verify wide data extension lookup is available from all FWD1 contexts (except NAT)
 *
 * The resources we compare are:
 * dpc inlif for all covers vs. sbc inlif.
 */
shr_error_e
dnx_inlif_wide_data_extension_lookup_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    uint32 ffc_bmp[BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC)] = { 0 };
    int context_profile, i;
    uint8 is_illegal;
    uint32 context_hw_val;
    uint32 app_db_id;
    dbal_tables_e logical_table;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** run over all contexts. */
    for (context_profile = 0; context_profile < DBAL_NOF_ENUM_FWD1_CONTEXT_ID_VALUES; context_profile++)
    {
        SHR_IF_ERR_EXIT(dbal_fields_is_illegal_value(unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &is_illegal));
        if (is_illegal)
        {
            continue;
        }
        logical_table = DBAL_TABLE_EMPTY;
        /**  The following contexts does not contain wide data extension lookup */
        if (context_profile == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_UPSTREAM ||
            context_profile == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_IPV4___NAT_DOWNSTREAM ||
            context_profile == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR ||
            context_profile == DBAL_ENUM_FVAL_FWD1_CONTEXT_ID_BIER___NON_MPLS_BFR_SA_LKP)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dbal_fields_enum_hw_value_get
                        (unit, DBAL_FIELD_FWD1_CONTEXT_ID, context_profile, &context_hw_val));
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_KLEAP_FWD12_KBR_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FWD12_KBR_ID,
                                   CTEST_DNX_IN_LIF_WIDE_DATA_FWD1_LOOKUP_KBR_ID);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_PROFILE, context_hw_val);

        /** request for value field */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_APP_DB_ID, INST_SINGLE, &app_db_id);
        /** Set the ffc id's bitmap*/
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_FFC_BITMAP, INST_SINGLE, ffc_bmp);
        /** get value */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        /** convert phy db id to logical db */
        SHR_IF_ERR_EXIT(dbal_tables_table_by_mdb_phy_get(unit, DBAL_PHYSICAL_TABLE_SEXEM_2, app_db_id, &logical_table));

        if (logical_table != DBAL_TABLE_EM_PMF_ADDITIONAL_DATA)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed: logical_table=%d, expected %d (DBAL_TABLE_EM_PMF_ADDITIONAL_DATA) context_profile=%d\n",
                         logical_table, DBAL_TABLE_EM_PMF_ADDITIONAL_DATA, context_profile);
        }
        for (i = 0; i < BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC); i++)
        {
            if (ffc_bmp[i] != 0)
            {
                break;
            }
        }
        if (i == BITS2WORDS(DNX_DATA_MAX_FIELD_COMMON_MAX_VAL_NOF_FFC))
        {
            SHR_CLI_EXIT(_SHR_E_FAIL,
                         "Test failed: ffc_bmp=0. Expected ffc in used by EM_PMF_ADDITIONAL_DATA DB, context_profile=%d \n",
                         context_profile);
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }
    LOG_CLI((BSL_META("all relevant contexts contain wide data extension lookup as expected!!! \n")));
exit:

    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif /* BCM_DNX2_SUPPORT */

/**
 * \brief DNX in lif wide data ctests * List of the supported ctests, pointer to command function and
 * command usage function. This is the entry point for in lif wide data tests
 * ctest commands  */
sh_sand_cmd_t sh_dnx_inlif_wide_data_test_cmds[] = {
#if defined(BCM_DNX2_SUPPORT)
    {"lookup", dnx_inlif_wide_data_extension_lookup_test, NULL, NULL, &dnx_inlif_wide_data_extension_lookup_test_man,
     NULL, NULL,
     CTEST_POSTCOMMIT}
    ,
#endif /* BCM_DNX2_SUPPORT */
    {NULL}
};
/*
 * }
 */
