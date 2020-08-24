/** \file diag_dnx_field_presel.c
 * $Id$
 *
 * PMF preselection (Context Selection) application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /*
  * Include files.
  * {
  */
#include "ctest_dnx_field_presel.h"
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include "ctest_dnx_field_utils.h"

/*
 * }
 */
/*
 * MACROs
 * {
 */
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_ACTION_OPTION_TEST_COUNT        "count"

/*
 * }
 */

 /*
  * Global and Static
  */

/*
 * }
 */

/**
 *  Field Preselector leaf detais
 */
sh_sand_man_t sh_dnx_field_presel_man = {
    "Preselection related test utilities",
    "Set a preselector for each stage, then get and compare values."
};

/**
 * \brief
 *   This function makes a semantic test for PMF Preselection.
 *   Sets a preselector in each of the stages.
 *   Gets the values and compares.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_presel(
    int unit)
{
    uint32 flags = 0;
    bcm_field_presel_entry_id_t entry_id;
    bcm_field_presel_entry_data_t entry_data;
    bcm_field_context_info_t context_info;
    int presel_idx;
    int nof_presels = 5;
    /*
     * Used for global iteration of all used qualifiers
     */
    int qual_idx;
    /*
     * Used for iterating qualifiers of the same presel
     */
    int int_qual_idx;

/* *INDENT-OFF* */
    /*
     * User-provided IDs of each preselector, 0-127. No need to be unique.
     */
    bcm_field_presel_t presel_id[] =           { 51                        , 52                       , 53                      , 54                   , 55          };
    /*
     * Stage of each preselector.
     */
    bcm_field_stage_t stage[] = { bcmFieldStageIngressPMF1, bcmFieldStageIngressPMF1, bcmFieldStageIngressPMF3, bcmFieldStageEgress, bcmFieldStageIngressPMF1};
    /*
     * Entry-valid to be configures with each preselector.
     */
    uint8 entry_valid[] =       { TRUE                     , TRUE                   , TRUE               , TRUE                     , FALSE         };
    /*
     * Program-ID to be configures with each preselector.
     * Range: 0-63.
     */
    bcm_field_context_t context_id[5];
    /*
     * Number of valid qualifiers to be used for each preselector.
     */
    int nof_quals[] =           { 1                        , 1                       , 1                   , 1                      , 1   };

    /*
     * Qualifier attributes for each used qualifier
     */
    /*
     * Type, relevant to its stage.
     */
    bcm_field_qualify_t qual_type[] = { bcmFieldQualifyForwardingLayerIndex,
                                        bcmFieldQualifyVlanFormat,
                                        bcmFieldQualifyRpfEcmpMode,
                                        bcmFieldQualifyContextId,
                                        bcmFieldQualifyVlanFormat,
                                        bcmFieldQualifyForwardingLayerIndex};
    /*
     * Expected qual Types, relevant to its stage.
     */
    bcm_field_qualify_t expected_qual_type[] = { bcmFieldQualifyForwardingLayerIndex,
                                        bcmFieldQualifyVlanFormat,
                                        bcmFieldQualifyRpfEcmpMode,
                                        bcmFieldQualifyContextIdRaw,
                                        bcmFieldQualifyVlanFormat,
                                        bcmFieldQualifyForwardingLayerIndex};

    /*
     * Argument. Random supported argument.
     */
    int qual_arg[] =                  { 0,
                                        0,
                                        0,
                                        0,
                                        0,
                                        0 };
    /*
     * Value. Random supported value.
     */
    uint32 qual_value[] =             { 0x1,
                                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                        2,
                                        7,
                                        BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                        0x1 };

    /*
     * Expected Values.
     */
    uint32 expected_qual_value[] =    { 0x1,
                                        BCM_FIELD_VLAN_FORMAT_INNER_TAGGED,
                                        2,
                                        7,
                                        BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED,
                                        0x1 };

    /*
     * Mask - the max for each used qualifier, all 1's.
     */
    uint32 qual_mask[] =              { 0x3,
                                        0x1f,
                                        0x3,
                                        0x1f,
                                        0x3ff,
                                        0x3 };
/* *INDENT-ON* */

    SHR_FUNC_INIT_VARS(unit);

    LOG_INFO_EX(BSL_LOG_MODULE, "PRESELECTION SEMANTIC TEST START...\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

    bcm_field_presel_entry_id_info_init(&entry_id);
    bcm_field_presel_entry_data_info_init(&entry_data);
    bcm_field_context_info_t_init(&context_info);

    qual_idx = 0;
    for (presel_idx = 0; presel_idx < nof_presels; presel_idx++)
    {
        if (stage[presel_idx] == bcmFieldStageIngressPMF2)
        {
            context_info.cascaded_from = BCM_FIELD_CONTEXT_ID_DEFAULT;
        }
        bcm_field_context_create(unit, 0, stage[presel_idx], &context_info, &context_id[presel_idx]);

        LOG_INFO_EX(BSL_LOG_MODULE, " Set Preselector %d/%d...\n %s%s", presel_idx + 1, nof_presels, EMPTY, EMPTY);
        /*
         * Fill params structures
         */
        entry_id.presel_id = presel_id[presel_idx];
        entry_id.stage = stage[presel_idx];
        /*
         * Configure qualifiers data
         */
        for (int_qual_idx = 0; int_qual_idx < nof_quals[presel_idx]; int_qual_idx++)
        {
            entry_data.qual_data[int_qual_idx].qual_type = qual_type[qual_idx];
            entry_data.qual_data[int_qual_idx].qual_arg = qual_arg[qual_idx];
            entry_data.qual_data[int_qual_idx].qual_value = qual_value[qual_idx];
            entry_data.qual_data[int_qual_idx].qual_mask = qual_mask[qual_idx];
            qual_idx++;
        }
        /*
         * Assign qualifiers data in params structure
         */
        entry_data.nof_qualifiers = nof_quals[presel_idx];
        entry_data.context_id = context_id[presel_idx];
        entry_data.entry_valid = entry_valid[presel_idx];

        SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, flags, &entry_id, &entry_data));
    }

    /*
     * Initialize the input structures to prepare for filling them with GET
     */
    entry_data.nof_qualifiers = 0;
    entry_data.context_id = 0;
    entry_data.entry_valid = 0;
    sal_memset(&entry_id, 0, sizeof(entry_id));

    qual_idx = 0;
    for (presel_idx = 0; presel_idx < nof_presels; presel_idx++)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, " Get Preselector %d/%d...\n %s%s", presel_idx + 1, nof_presels, EMPTY, EMPTY);
        /*
         * Fill params structures
         */
        entry_id.presel_id = presel_id[presel_idx];
        entry_id.stage = stage[presel_idx];

        SHR_IF_ERR_EXIT(bcm_field_presel_get(unit, flags, &entry_id, &entry_data));

        LOG_INFO_EX(BSL_LOG_MODULE, "  Compare values... %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

        if (entry_data.entry_valid != entry_valid[presel_idx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL,
                         "bcm_field_presel_get: Unexpected data returned.\n"
                         "entry_valid expected %d received %d.\n", entry_valid[presel_idx], entry_data.entry_valid);
        }
        /*
         * Only compare the rest of the information if the entry is valid.
         */
        if (entry_valid[presel_idx])
        {
            if (entry_data.context_id != context_id[presel_idx])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "bcm_field_presel_get: Unexpected data returned.\n"
                             "Context_id expected %d received %d.\n", context_id[presel_idx], entry_data.context_id);
            }
            if (entry_data.nof_qualifiers != nof_quals[presel_idx])
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "bcm_field_presel_get: Unexpected data returned.\n"
                             "nof_qualifiers expected %d received %d.\n", nof_quals[presel_idx],
                             entry_data.nof_qualifiers);
            }

            for (int_qual_idx = 0; int_qual_idx < entry_data.nof_qualifiers; int_qual_idx++)
            {
                if (entry_data.qual_data[int_qual_idx].qual_type != expected_qual_type[qual_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "bcm_field_presel_get: Unexpected data returned.\n"
                                 "qualifier %d type expected %d received %d.\n",
                                 int_qual_idx, expected_qual_type[qual_idx],
                                 entry_data.qual_data[int_qual_idx].qual_type);
                }
                if (entry_data.qual_data[int_qual_idx].qual_arg != qual_arg[qual_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "bcm_field_presel_get: Unexpected data returned.\n"
                                 "qualifier %d Argument expected %d received %d.\n",
                                 int_qual_idx, qual_arg[qual_idx], entry_data.qual_data[int_qual_idx].qual_arg);
                }
                if (entry_data.qual_data[int_qual_idx].qual_value != expected_qual_value[qual_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "bcm_field_presel_get: Unexpected data returned.\n"
                                 "qualifier %d value expected %d received %d.\n",
                                 int_qual_idx, expected_qual_value[qual_idx],
                                 entry_data.qual_data[int_qual_idx].qual_value);
                }
                if (entry_data.qual_data[int_qual_idx].qual_mask != qual_mask[qual_idx])
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "bcm_field_presel_get: Unexpected data returned.\n"
                                 "qualifier %d mask expected %d received %d.\n",
                                 int_qual_idx, qual_mask[qual_idx], entry_data.qual_data[int_qual_idx].qual_mask);
                }
                qual_idx++;
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE, " Done.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);
    }

    LOG_INFO_EX(BSL_LOG_MODULE, "PRESELECTION SEMANTIC TEST END.\n %s%s%s%s", EMPTY, EMPTY, EMPTY, EMPTY);

exit:
    CTEST_DNX_FIELD_UTIL_ERR(ctest_dnx_field_util_clean(unit));
    SHR_FUNC_EXIT;

}
/* *INDENT-OFF* */
/**
 * \brief
 *   Options list for 'presel' shell command
 * \remark
 */
sh_sand_option_t dnx_field_presel_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {DNX_DIAG_ACTION_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'presel' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   NONE
 */
sh_sand_invoke_t sh_dnx_field_presel_tests[] = {
    {"contextselect", "count=3", CTEST_POSTCOMMIT}
    ,
    {NULL}
};
/* *INDENT-ON* */
/**
 * \brief - run PMF Preselection sequence in diag shell
 */
shr_error_e
sh_dnx_field_presel_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_presel(unit));
    }
exit:
    SHR_FUNC_EXIT;
}
#undef BSL_LOG_MODULE
