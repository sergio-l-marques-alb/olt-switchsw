
/** \file ctest_dnx_field_compare.c
 *
 * Compare feature testing.
 * Compare is what allows us to do comparision between certain key values that we build using qualifiers.
 * In JR2, there exists two compare componenets, each with the following two modes:
 * 1) Compare a 32b built key with 32b TCAM-result-payload (Single mode)
 * 2) Compare two different 32b keys, in addition to the first key maintaining its comparision result
 *    with TCAM-payload (Double mode)
 *
 * The current two compare components that currently exist, make use of the following resources:
 * 1) Initial Key F as the key to be compared with TCAM-Result-B
 *    Initial Key F along with Initial Key G
 * 2) Initial Key H as the key to be compared with TCAM-Result-D
 *    Initial Key H along with Initial Key I
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_compare.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define CTEST_DNX_FIELD_COMPARE_OPTION_TYPE        "type"

/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define CTEST_DNX_FIELD_COMPARE_OPTION_COUNT              "count"

/**
 * \brief
 *   Keyword for controlling whether to release resources at end of test.
 *   clean can be either 0 or 1, if the ctest will clean-up after its finished setting-up.
 */
#define CTEST_DNX_FIELD_COMPARE_OPTION_CLEAN              "clean"

/**
 * \brief
 *   Options list for 'compare' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_compare_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */
    {CTEST_DNX_FIELD_COMPARE_OPTION_TYPE,        SAL_FIELD_TYPE_ENUM,    "Type (level) of test (dnx or bcm)",        "DNX",     (void *)Field_level_enum_table},
    {CTEST_DNX_FIELD_COMPARE_OPTION_COUNT,       SAL_FIELD_TYPE_UINT32,  "Number of times test will run",            "1"},
    {CTEST_DNX_FIELD_COMPARE_OPTION_CLEAN,      SAL_FIELD_TYPE_BOOL,    "Will test perform HW and SW-state clean-up or not",     "Yes"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'compare' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_compare_tests[] = {
    {"DNX_fld_compare", "type=dnx clean=yes count=6", CTEST_POSTCOMMIT},
    {"BCM_fld_compare", "type=bcm clean=yes count=6", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_compare_man = {
    "Field Compare feature tests",
    "Creating a field group with single qualifier and action\n"
        "Creating a new context with one of the compare modes\n"
        "Creating a new FG in IPMF2 to read the compare result\n"
        "Cleaning up all resources used after the test finishesr\n"
        "The 'count' variable defines how many times the test will run",
    "ctest field compare type=<DNX | BCM> count=1"
};

/**
 * \brief
 *  Function to  call 'dnx_field_context_compare_info_get' using the supplied and 'context_id'
 *  Which is used in the corresponding 'set' operation (dnx_field_context_compare_create())
 *  We compare it to the 'set_compare_info_p' supplied to the function.
 *  We have a single qualifier in the attach info, hence why
 *  we only compare the first element of the corresponding arrays. We do this operation 
 *  for first_key_info and second_key_info.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID
 * \param [in] pair_id            - Pair ID.
 * \param [in] set_compare_info_p   - Context Compare info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_cmp_compare_dnx(
    int unit,
    dnx_field_context_t context_id,
    dnx_field_context_compare_pair_e pair_id,
    dnx_field_key_compare_pair_info_t * set_compare_info_p)
{
    dnx_field_key_compare_pair_info_t cmp_pair_get_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_compare_info_get
                    (unit, DNX_FIELD_STAGE_IPMF1, context_id, pair_id, &cmp_pair_get_info));

    /** COMPARE first_key_info */
    /** Comparing the single DNX Qualifier */
    if (set_compare_info_p->first_key_info.dnx_quals[0] != cmp_pair_get_info.first_key_info.dnx_quals[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p dnx_qual (%s) is not the same as cmp_pair_get_info dnx_qual (%s)\r\n",
                     dnx_field_dnx_qual_text(unit, set_compare_info_p->first_key_info.dnx_quals[0]),
                     dnx_field_dnx_qual_text(unit, cmp_pair_get_info.first_key_info.dnx_quals[0]));
    }

    /** Comparing the single DNX Qualifier Info - input_type */
    if (set_compare_info_p->first_key_info.qual_info[0].input_type !=
        cmp_pair_get_info.first_key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_type (0x%08X) is not the same as cmp_pair_get_info qual input_type (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].input_type,
                     cmp_pair_get_info.first_key_info.qual_info[0].input_type);
    }

    /** Comparing the single DNX Qualifier Info - input_arg */
    if (set_compare_info_p->first_key_info.qual_info[0].input_arg !=
        cmp_pair_get_info.first_key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_arg (0x%08X) is not the same as cmp_pair_get_info qual input_arg (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].input_arg,
                     cmp_pair_get_info.first_key_info.qual_info[0].input_arg);
    }

    /** Comparing the single DNX Qualifier Info - offset */
    if (set_compare_info_p->first_key_info.qual_info[0].offset != cmp_pair_get_info.first_key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual offset (0x%08X) is not the same as cmp_pair_get_info qual offset (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].offset,
                     cmp_pair_get_info.first_key_info.qual_info[0].offset);
    }

    /** COMPARE second_key_info */
    /** Comparing the single DNX Qualifier */
    if (set_compare_info_p->second_key_info.dnx_quals[0] != cmp_pair_get_info.second_key_info.dnx_quals[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p dnx_qual (%s) is not the same as cmp_pair_get_info dnx_qual (%s)\r\n",
                     dnx_field_dnx_qual_text(unit, set_compare_info_p->second_key_info.dnx_quals[0]),
                     dnx_field_dnx_qual_text(unit, cmp_pair_get_info.second_key_info.dnx_quals[0]));
    }

    /** Comparing the single DNX Qualifier Info - input_type */
    if (set_compare_info_p->second_key_info.qual_info[0].input_type !=
        cmp_pair_get_info.second_key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_type (0x%08X) is not the same as cmp_pair_get_info qual input_type (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].input_type,
                     cmp_pair_get_info.second_key_info.qual_info[0].input_type);
    }

    /** Comparing the single DNX Qualifier Info - input_arg */
    if (set_compare_info_p->second_key_info.qual_info[0].input_arg !=
        cmp_pair_get_info.second_key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_arg (0x%08X) is not the same as cmp_pair_get_info qual input_arg (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].input_arg,
                     cmp_pair_get_info.second_key_info.qual_info[0].input_arg);
    }

    /** Comparing the single DNX Qualifier Info - offset */
    if (set_compare_info_p->second_key_info.qual_info[0].offset !=
        cmp_pair_get_info.second_key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual offset (0x%08X) is not the same as cmp_pair_get_info qual offset (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].offset,
                     cmp_pair_get_info.second_key_info.qual_info[0].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Function to  call 'bcm_field_context_compare_info_get' using the supplied and 'context_id'
 *  Which is used in the corresponding 'set' operation (bcm_field_context_compare_create())
 *  We compare it to the 'set_compare_info_p' supplied to the function.
 *  We have a single qualifier in the attach info, hence why
 *  we only compare the first element of the corresponding arrays. We do this operation 
 *  for first_key_info and second_key_info.
 * \param [in] unit             - Device ID
 * \param [in] context_id       - Context ID
 * \param [in] pair_id            - Pair ID.
 * \param [in] set_compare_info_p   - Context Compare info to be compared against.
 *
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
ctest_dnx_field_context_cmp_compare_bcm(
    int unit,
    bcm_field_context_t context_id,
    uint32 pair_id,
    bcm_field_context_compare_info_t * set_compare_info_p)
{
    bcm_field_context_compare_info_t cmp_get_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_compare_info_t_init(&cmp_get_info);

    SHR_IF_ERR_EXIT(bcm_field_context_compare_info_get
                    (unit, bcmFieldStageIngressPMF1, context_id, pair_id, &cmp_get_info));

    /** COMPARE first_key_info */
    /** Comparing the single BCM Qualifier */
    if (set_compare_info_p->first_key_info.qual_types[0] != cmp_get_info.first_key_info.qual_types[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "set_compare_info_p dnx_qual (%s) is not the same as cmp_get_info dnx_qual (%s)\r\n",
                     dnx_field_bcm_qual_text(unit, set_compare_info_p->first_key_info.qual_types[0]),
                     dnx_field_bcm_qual_text(unit, cmp_get_info.first_key_info.qual_types[0]));
    }

    /** Comparing the single BCM Qualifier Info - input_type */
    if (set_compare_info_p->first_key_info.qual_info[0].input_type !=
        cmp_get_info.first_key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_type (0x%08X) is not the same as cmp_get_info qual input_type (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].input_type,
                     cmp_get_info.first_key_info.qual_info[0].input_type);
    }

    /** Comparing the single BCM Qualifier Info - input_arg */
    if (set_compare_info_p->first_key_info.qual_info[0].input_arg != cmp_get_info.first_key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_arg (0x%08X) is not the same as cmp_get_info qual input_arg (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].input_arg,
                     cmp_get_info.first_key_info.qual_info[0].input_arg);
    }

    /** Comparing the single BCM Qualifier Info - offset */
    if (set_compare_info_p->first_key_info.qual_info[0].offset != cmp_get_info.first_key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual offset (0x%08X) is not the same as cmp_get_info qual offset (0x%08X)\r\n",
                     set_compare_info_p->first_key_info.qual_info[0].offset,
                     cmp_get_info.first_key_info.qual_info[0].offset);
    }

    /** COMPARE second_key_info */
    /** Comparing the single BCM Qualifier */
    if (set_compare_info_p->second_key_info.qual_types[0] != cmp_get_info.second_key_info.qual_types[0])
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "set_compare_info_p dnx_qual (%s) is not the same as cmp_get_info dnx_qual (%s)\r\n",
                     dnx_field_bcm_qual_text(unit, set_compare_info_p->second_key_info.qual_types[0]),
                     dnx_field_bcm_qual_text(unit, cmp_get_info.second_key_info.qual_types[0]));
    }

    /** Comparing the single BCM Qualifier Info - input_type */
    if (set_compare_info_p->second_key_info.qual_info[0].input_type !=
        cmp_get_info.second_key_info.qual_info[0].input_type)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_type (0x%08X) is not the same as cmp_get_info qual input_type (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].input_type,
                     cmp_get_info.second_key_info.qual_info[0].input_type);
    }

    /** Comparing the single BCM Qualifier Info - input_arg */
    if (set_compare_info_p->second_key_info.qual_info[0].input_arg !=
        cmp_get_info.second_key_info.qual_info[0].input_arg)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual input_arg (0x%08X) is not the same as cmp_get_info qual input_arg (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].input_arg,
                     cmp_get_info.second_key_info.qual_info[0].input_arg);
    }

    /** Comparing the single BCM Qualifier Info - offset */
    if (set_compare_info_p->second_key_info.qual_info[0].offset != cmp_get_info.second_key_info.qual_info[0].offset)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "set_compare_info_p qual offset (0x%08X) is not the same as cmp_get_info qual offset (0x%08X)\r\n",
                     set_compare_info_p->second_key_info.qual_info[0].offset,
                     cmp_get_info.second_key_info.qual_info[0].offset);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Check correctnes of compare key allocation.
 *  In case compare_id is 0 ---> Key B
 *  In case compare_id is 1 ---> Key D
 *
 * \param [in] unit - Device ID
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_compare_key_allocation_check(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id;
    bcm_field_context_info_t context_info;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_context_t context_id;
    bcm_field_group_attach_info_t attach_info;
    dnx_field_key_id_t key_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) fg_info.name, "cmp_key_alloc_fg", sizeof(fg_info.name));
    fg_info.nof_quals = 1;
    fg_info.nof_actions = 1;
    fg_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 2;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) context_info.name, "cmp_key_alloc_ctx", sizeof(context_info.name));

    bcm_field_context_compare_info_t_init(&compare_info);
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = 2;
    compare_info.first_key_info.qual_info[0].offset = 0;

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.second_key_info.qual_info[0].input_arg = 2;
    compare_info.second_key_info.qual_info[0].offset = 0;

    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeSingle;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));
    attach_info.compare_id = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_B)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Single | CMP_ID: 0 | PAIR_ID: 0, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "B");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Single | CMP_ID: 0 | PAIR_ID: 0 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "B", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 0));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeSingle;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));
    attach_info.compare_id = 1;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_D)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Single | CMP_ID: 1 | PAIR_ID: 1, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "D");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Single | CMP_ID: 1 | PAIR_ID: 1 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "D", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 1));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeSingle;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));
    attach_info.compare_id = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_B)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Double | CMP_ID: 0 | PAIR_ID: 0, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "B");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Double | CMP_ID: 0 | PAIR_ID: 0 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "B", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 0));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));
    attach_info.compare_id = 0;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_B)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Double | CMP_ID: 0 | PAIR_ID: 1, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "B");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Double | CMP_ID: 0 | PAIR_ID: 1 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "B", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 1));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 0, &compare_info));
    attach_info.compare_id = 1;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_D)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Double | CMP_ID: 1 | PAIR_ID: 0, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "D");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Double | CMP_ID: 1 | PAIR_ID: 0 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "D", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 0));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));

    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));
    attach_info.compare_id = 1;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info));

    SHR_IF_ERR_EXIT(dnx_field_group_context_key_id_get
                    (unit, (dnx_field_group_t) fg_id, (dnx_field_context_t) context_id, &key_id));
    if (key_id.id[0] != DBAL_ENUM_FVAL_FIELD_KEY_D)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL,
                     "Returned key '%c', for CMP_MODE: Double | CMP_ID: 1 | PAIR_ID: 1, is different than expected '%s' !\r\n",
                     ('A' + key_id.id[0]), "D");
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "Allocated key for CMP_MODE: Double | CMP_ID: 1 | PAIR_ID: 1 ---> returned: %c  |  expected: %s !\r\n %s%s",
                    ('A' + key_id.id[0]), "D", EMPTY, EMPTY);
    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, context_id));
    SHR_IF_ERR_EXIT(bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, context_id, 1));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Simple Case of compare - We create a new context with double compare mode for
 *  the first pair, then we create a new IPMF2 FG on the same context to read the
 *  compare result and do an action according to the result read.
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_compare_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_field_context_mode_t dnx_context_mode;
    dnx_field_context_t context_id;
    dnx_field_group_info_t fg_info_in;
    dnx_field_group_attach_info_t attach_info;
    dnx_field_group_t grp_id;
    dnx_field_entry_t entry_info;
    dnx_field_key_compare_pair_info_t compare_pair;
    dnx_field_context_compare_pair_e pair_id;
    uint32 entry_handle1;
    uint32 entry_handle2;
    dnx_field_presel_entry_id_t presel_entry_id;
    dnx_field_presel_entry_data_t presel_data;

    bcm_field_context_info_t bcm_context_info;
    bcm_field_context_compare_info_t bcm_compare_info;
    bcm_field_context_t bcm_context_id;

    unsigned int compare_test_clean;
    unsigned int test_type;

    SHR_FUNC_INIT_VARS(unit);

    pair_id = 0;

    compare_test_clean = 0;
    test_type = 0;
    SH_SAND_GET_BOOL(CTEST_DNX_FIELD_COMPARE_OPTION_CLEAN, compare_test_clean);
    SH_SAND_GET_ENUM(CTEST_DNX_FIELD_COMPARE_OPTION_TYPE, test_type);

    /**
     * Check correctness if compare key allocation.
     * Will be run before all other scenarios. Not depending on any inputs.
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_compare_key_allocation_check(unit));

    if (test_type == 0)
    {
        /**
         * Configure Compare mode/keys
         */
        bcm_field_context_info_t_init(&bcm_context_info);
        bcm_context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
        SHR_IF_ERR_EXIT(bcm_field_context_create
                        (unit, 0, bcmFieldStageIngressPMF1, &bcm_context_info, &bcm_context_id));

        /**
         * Init the bcm_compare_info
         */
        bcm_field_context_compare_info_t_init(&bcm_compare_info);
        /**
         * Fill the compare info
         */
        bcm_compare_info.first_key_info.nof_quals = 1;
        bcm_compare_info.first_key_info.qual_types[0] = bcmFieldQualifyL4DstPort;
        bcm_compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        bcm_compare_info.first_key_info.qual_info[0].input_arg = 2;
        bcm_compare_info.first_key_info.qual_info[0].offset = 0;

        bcm_compare_info.second_key_info.nof_quals = 1;
        bcm_compare_info.second_key_info.qual_types[0] = bcmFieldQualifyL4SrcPort;
        bcm_compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        bcm_compare_info.second_key_info.qual_info[0].input_arg = 2;
        bcm_compare_info.second_key_info.qual_info[0].offset = 0;

        pair_id = 0;
        SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                        (unit, 0, bcmFieldStageIngressPMF1, bcm_context_id, pair_id, &bcm_compare_info));

        /** Compare results of the set_compare_info_p with the retrieved from the get API. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_cmp_compare_bcm(unit, bcm_context_id, pair_id, &bcm_compare_info));
    }
    else if (test_type == 1)
    {
        /**
         * Configure Compare mode/keys
         */
        SHR_IF_ERR_EXIT(dnx_field_context_mode_t_init(unit, &dnx_context_mode));

        dnx_context_mode.context_ipmf1_mode.compare_mode_1 = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;

        SHR_IF_ERR_EXIT(dnx_field_context_create(unit, 0, DNX_FIELD_STAGE_IPMF1, &dnx_context_mode, &context_id));

        compare_pair.first_key_info.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_SRC);
        compare_pair.first_key_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        compare_pair.first_key_info.qual_info[0].input_arg = 1;
        compare_pair.first_key_info.qual_info[0].offset = 0;
        compare_pair.first_key_info.dnx_quals[1] = DNX_FIELD_QUAL_TYPE_INVALID;
        compare_pair.first_key_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_INVALID;

        compare_pair.second_key_info.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_HEADER, DNX_FIELD_STAGE_IPMF1, DNX_FIELD_HEADER_QUAL_IPV4_DST);
        compare_pair.second_key_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_LAYER_ABSOLUTE;
        compare_pair.second_key_info.qual_info[0].input_arg = 1;
        compare_pair.second_key_info.qual_info[0].offset = 0;
        compare_pair.second_key_info.dnx_quals[1] = DNX_FIELD_QUAL_TYPE_INVALID;
        compare_pair.second_key_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_INVALID;

        pair_id = DNX_FIELD_CONTEXT_COMPARE_FIRST_PAIR;

        SHR_IF_ERR_EXIT(dnx_field_context_compare_create
                        (unit, 0, DNX_FIELD_STAGE_IPMF1, context_id, pair_id, &compare_pair));

        /**
         * Configure IPMF2 FG that reads compare result and performs DP action
         */
        SHR_IF_ERR_EXIT(dnx_field_group_info_t_init(unit, &fg_info_in));
        fg_info_in.fg_type = DNX_FIELD_GROUP_TYPE_TCAM;
        fg_info_in.field_stage = DNX_FIELD_STAGE_IPMF2;

        fg_info_in.dnx_actions[0] =
            DNX_ACTION(DNX_FIELD_ACTION_CLASS_STATIC, DNX_FIELD_STAGE_IPMF2, DBAL_ENUM_FVAL_IPMF1_ACTION_DP);

        fg_info_in.dnx_quals[0] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_CMP_KEY_0_DECODED);
        
        fg_info_in.dnx_quals[1] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_PMF1_TCAM_ACTION_2);
        fg_info_in.dnx_quals[2] =
            DNX_QUAL(DNX_FIELD_QUAL_CLASS_META2, DNX_FIELD_STAGE_IPMF2, DNX_FIELD_IPMF2_QUAL_TRJ_HASH);

        SHR_IF_ERR_EXIT(dnx_field_group_add(unit, 0, &fg_info_in, &grp_id));

        SHR_IF_ERR_EXIT(dnx_field_group_attach_info_t_init(unit, &attach_info));
        attach_info.dnx_actions[0] = fg_info_in.dnx_actions[0];

        attach_info.action_info[0].priority = BCM_FIELD_ACTION_PRIORITY(0, 10);

        attach_info.dnx_quals[0] = fg_info_in.dnx_quals[0];
        attach_info.qual_info[0].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

        attach_info.dnx_quals[1] = fg_info_in.dnx_quals[1];
        attach_info.qual_info[1].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

        attach_info.dnx_quals[2] = fg_info_in.dnx_quals[2];
        attach_info.qual_info[2].input_type = DNX_FIELD_INPUT_TYPE_META_DATA2;

        SHR_IF_ERR_EXIT(dnx_field_group_context_attach(unit, 0, grp_id, context_id, &attach_info));

        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));

        entry_info.priority = 1;

        entry_info.key_info.qual_info[0].dnx_qual = fg_info_in.dnx_quals[0];

        /**
         * Add first entry to set DP to 2 when IP.src and IP.dst don't match
         *
         * bit number 0 holds (KEY.F != KEY.G) result.
         */
        entry_info.key_info.qual_info[0].qual_value[0] = 0x1;
        entry_info.key_info.qual_info[0].qual_mask[0] = 0x1;

        entry_info.payload_info.action_info[0].dnx_action = fg_info_in.dnx_actions[0];
        entry_info.payload_info.action_info[0].action_value[0] = 2;

        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, grp_id, &entry_info, &entry_handle1));

        /**
         * Add second entry to set DP to 3 when IP.src and IP.dst match
         *
         * bit number 0 holds (KEY.F != KEY.G) result.
         */
        SHR_IF_ERR_EXIT(dnx_field_entry_t_init(unit, &entry_info));
        entry_info.priority = 1;
        entry_info.key_info.qual_info[0].dnx_qual = fg_info_in.dnx_quals[0];

        entry_info.key_info.qual_info[0].qual_value[0] = 0;
        entry_info.key_info.qual_info[0].qual_mask[0] = 0x1;

        entry_info.payload_info.action_info[0].dnx_action = fg_info_in.dnx_actions[0];
        entry_info.payload_info.action_info[0].action_value[0] = 3;

        SHR_IF_ERR_EXIT(dnx_field_entry_tcam_add(unit, 0, grp_id, &entry_info, &entry_handle2));

        presel_entry_id.presel_id = 50;
        presel_entry_id.stage = DNX_FIELD_STAGE_IPMF1;
        presel_data.context_id = context_id;
        presel_data.entry_valid = TRUE;
        presel_data.nof_qualifiers = 1;
        presel_data.qual_data[0].qual_type_dbal_field = DBAL_FIELD_FWD_LAYER_TYPE_0;
        /*
         * 2 for IPv4
         */
        presel_data.qual_data[0].qual_value = 2;
        presel_data.qual_data[0].qual_mask = 0xFF;
        SHR_IF_ERR_EXIT(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));

        /** Compare results of the set_compare_info_p with the retrieved from the get API. */
        SHR_IF_ERR_EXIT(ctest_dnx_field_context_cmp_compare_dnx(unit, context_id, pair_id, &compare_pair));
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'test_type') is not implemented. Illegal parameter. %s\r\n",
                    __FUNCTION__, __LINE__, test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    if (compare_test_clean)
    {
        if (test_type == 1)
        {
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_context_detach(unit, grp_id, context_id));
            /** Delete all entries for the IPMF2 FG_ID */
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_entry_delete_all(unit, grp_id));
            /** Delete the FG and deallocate the FG_ID */
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_group_delete(unit, grp_id));

            /*
             * Destroy the Compare Key - similar to fg_detach in TCAM FG 
             */
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_compare_destroy
                                     (unit, 0, DNX_FIELD_STAGE_IPMF1, context_id, pair_id));

            /*
             * Destroy the context, free all pre-allocated resources 
             */
            presel_data.entry_valid = FALSE;
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_presel_set(unit, 0, &presel_entry_id, &presel_data));
            CTEST_DNX_FIELD_UTIL_ERR(dnx_field_context_destroy(unit, DNX_FIELD_STAGE_IPMF1, context_id));
        }
        else if (test_type == 0)
        {
            bcm_field_context_compare_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id, pair_id);
            bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, bcm_context_id);
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the compare ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field compare"
 */
shr_error_e
sh_dnx_field_compare_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count_iter, count;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32(CTEST_DNX_FIELD_COMPARE_OPTION_COUNT, count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_compare_run(unit, args, sand_control));
    }
exit:
    SHR_FUNC_EXIT;
}
