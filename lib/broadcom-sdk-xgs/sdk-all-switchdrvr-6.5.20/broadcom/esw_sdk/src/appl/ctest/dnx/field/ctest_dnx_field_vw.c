/** \file diag_dnx_field_vw.c
 * $Id$
 *
 * Virtual wire operations, such as name to Id mapping
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>

#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "ctest_dnx_field_vw.h"
#include "ctest_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/sand/sand_signals.h>
#include <include/soc/dnx/pemladrv/pemladrv_virtual_wires.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * The various values available for 'general_test_type' input.
 */
#define TEST_TYPE_STAGE_MAP         "STAGE_MAP"
#define TEST_TYPE_NAME_MAP          "NAME_MAP"

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
/**
 * \brief
 *   Keyword for the type of test to be perfomed
 */
#define DNX_DIAG_VW_OPTION_TEST_TYPE        "type"
/**
 * \brief
 *   Keyword for the number of times the test will run
 */
#define DNX_DIAG_VW_OPTION_TEST_COUNT        "count"

/*
 * }
 */
 /*
  * Global and Static
  * {
  */
/* *INDENT-OFF* */

/**
 * \brief
 *   List of tests for virtual wire shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_vw_tests[] = {
    {"DNX_field_vw_stage_map",   "type=stage_map count=2",           CTEST_POSTCOMMIT}
    ,
    {"DNX_field_name_map",       "type=name_map count=2",            CTEST_POSTCOMMIT}
    ,
    {NULL}
};
/**
 * \brief
 *   Options list for virtual wire shell command
 */
sh_sand_option_t Sh_dnx_field_vw_options[] = {
     /* Name */                              /* Type */              /* Description */                          /* Default */
    {DNX_DIAG_VW_OPTION_TEST_TYPE,          SAL_FIELD_TYPE_STR,     "Type of test (stage_map, name_map)",         NULL},
    {DNX_DIAG_VW_OPTION_TEST_COUNT,         SAL_FIELD_TYPE_UINT32,  "Number of times test will run",              "1"},
    {NULL}      /* End of options list - must be last. */
};
/* *INDENT-ON* */
/*
 * }
 */

/**
 *  The description of the ctest options.
 */
sh_sand_man_t Sh_dnx_field_vw_man = {
    "Virtual wire related test utilities",
    "Activate virtual wire related test utilities.\r\n",
    "ctest field virtual_wire type = <stage_map | name_map>",
    "type=name_map\n",
};

/**
 * \brief
 *    Tests that the mapping between PP stage an pemla stage is symetric.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_vw_stage_map(
    int unit)
{
    dnx_pp_stage_e pp_stage_ndx;
    PEMLA_STAGES_ENUM_e pemla_stage_ndx;
    int ret_stage_1;
    int ret_stage_2;
    int nof_pp_stages = 0;
    int nof_pemla_stages = 0;
    shr_error_e rv;
    bsl_severity_t original_severity;

    SHR_FUNC_INIT_VARS(unit);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_INIT_PP, original_severity);

    for (pp_stage_ndx = 0; pp_stage_ndx < DNX_PP_STAGE_NOF; pp_stage_ndx++)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_INIT_PP, bslSeverityFatal);
        rv = pemladrv_vw_stages_sdk_to_pemla(unit, pp_stage_ndx, &ret_stage_1);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_INIT_PP, original_severity);
        if (rv == _SHR_E_INTERNAL)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "Unexpected error type.%s%s%s\n", EMPTY, EMPTY, EMPTY);
            SHR_IF_ERR_EXIT_WITH_LOG(pemladrv_vw_stages_pemla_to_sdk(unit, ret_stage_1, &ret_stage_2),
                                     "PP stage %d maps to PEMLA stage %d, but mapping is not symmetrical.%s\n",
                                     pp_stage_ndx, ret_stage_1, EMPTY);
            if (ret_stage_2 != pp_stage_ndx)
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "PP stage %d maps to PEMLA stage %d, but reverse mapping is %d\n",
                             pp_stage_ndx, ret_stage_1, ret_stage_2);
            }
            nof_pp_stages++;
        }
    }

    for (pemla_stage_ndx = 0; pemla_stage_ndx < PEMLA_UNDEFINED; pemla_stage_ndx++)
    {
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_INIT_PP, bslSeverityFatal);
        rv = pemladrv_vw_stages_pemla_to_sdk(unit, pemla_stage_ndx, &ret_stage_1);
        SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SOCDNX_INIT_PP, original_severity);
        if (rv == _SHR_E_INTERNAL)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT_WITH_LOG(rv, "Unexpected error type.%s%s%s\n", EMPTY, EMPTY, EMPTY);
            if (ret_stage_1 != DNX_PP_STAGE_INVALID)
            {
                SHR_IF_ERR_EXIT_WITH_LOG(pemladrv_vw_stages_sdk_to_pemla(unit, ret_stage_1, &ret_stage_2),
                                         "PEMLA stage %d maps to PP stage %d, but mapping is not symmetrical.%s\n",
                                         pemla_stage_ndx, ret_stage_1, EMPTY);
                if (ret_stage_2 != pemla_stage_ndx)
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "PEMLA stage %d maps to PP stage %d, but reverse mapping is %d\n",
                                 pemla_stage_ndx, ret_stage_1, ret_stage_2);
                }
                nof_pemla_stages++;
            }
        }
    }

    if (nof_pp_stages != nof_pemla_stages)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof_pp_stages %d nof_pemla_stages %d, not symmetrical.\n",
                     nof_pp_stages, nof_pemla_stages);
    }

    if (nof_pp_stages == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "No stages mapped.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Tests bcm_field_name_to_id.
 * \param [in] unit -
 *    Identifier of HW platform.
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
ctest_dnx_vw_name_map(
    int unit)
{
    bcm_field_action_info_t action_info;
    bcm_field_action_t action_id_1;
    bcm_field_action_t action_id_2;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id_1;
    bcm_field_qualify_t qual_id_2;
    bcm_field_context_info_t context_info;
    bcm_field_context_t context_id_1;
    bcm_field_context_t context_id_2;
    bcm_field_group_info_t fg_info;
    bcm_field_group_t fg_id_1;
    bcm_field_group_t fg_id_2;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id_1;
    bcm_field_ace_format_t ace_format_id_2;
    bcm_field_name_to_id_info_t name_to_id_info;
    uint32 id;
    shr_error_e rv;
    bsl_severity_t original_severity;
    int nof_ids;

    SHR_FUNC_INIT_VARS(unit);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);

    /*
     * Configure.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionUDHData2;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.size = 32;
    action_info.stage = bcmFieldStageIngressPMF1;
    sal_strncpy_s((char *) action_info.name, "action_name_1", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id_1));
    sal_strncpy_s((char *) action_info.name, "action_name_2", sizeof(action_info.name));
    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id_2));

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 6;
    sal_strncpy_s((char *) qual_info.name, "qual_name_1", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_1));
    sal_strncpy_s((char *) qual_info.name, "qual_name_2", sizeof(qual_info.name));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id_2));

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) context_info.name, "context_name_1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id_1));
    sal_strncpy_s((char *) context_info.name, "context_name_2", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id_2));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.stage = bcmFieldStageIngressPMF1;
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyIntPriority;
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    sal_strncpy_s((char *) fg_info.name, "fg_name_1", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_1));
    sal_strncpy_s((char *) fg_info.name, "fg_name_2", sizeof(fg_info.name));
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id_2));

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id_1, context_id_1, &attach_info));

    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionPrioIntNew;
    sal_strncpy_s((char *) ace_format_info.name, "ace_name_1", sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id_1));
    sal_strncpy_s((char *) ace_format_info.name, "ace_name_2", sizeof(ace_format_info.name));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id_2));

    /*
     * Check names
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "action_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != action_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of action name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "action_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != action_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of action name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "qual_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != qual_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of qual name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "qual_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != qual_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of qual name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF1;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != context_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of context name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF1;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != context_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of context name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != context_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of context name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF2;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != context_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of context name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "fg_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != fg_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of field group name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "fg_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != fg_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of field group name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "ace_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAceFormat;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != ace_format_id_1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of ACE format name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "ace_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAceFormat;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != ace_format_id_2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of ACE format name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "IntPriority", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != bcmFieldQualifyIntPriority)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of bcmFieldQualifyIntPriority name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "PrioIntNew", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id));
    if (id != bcmFieldActionPrioIntNew)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Incorrect reading of bcmFieldActionPrioIntNew name.\n");
    }
    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 1.\n", nof_ids);
    }

    /*
     * Delete
     */
    SHR_IF_ERR_EXIT(bcm_field_ace_format_delete(unit, ace_format_id_1));
    SHR_IF_ERR_EXIT(bcm_field_ace_format_delete(unit, ace_format_id_2));
    SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id_1, context_id_1));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id_1));
    SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id_2));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id_1));
    SHR_IF_ERR_EXIT(bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, context_id_2));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, qual_id_2));
    SHR_IF_ERR_EXIT(bcm_field_qualifier_destroy(unit, qual_id_1));
    SHR_IF_ERR_EXIT(bcm_field_action_destroy(unit, action_id_1));
    SHR_IF_ERR_EXIT(bcm_field_action_destroy(unit, action_id_2));

    /*
     * Negative test 
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "action_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "action_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "qual_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "qual_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF1;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF1;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF2;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "context_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF2;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "fg_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "fg_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "ace_name_1", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAceFormat;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "ace_name_2", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAceFormat;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF1;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdContext;
    name_to_id_info.stage = bcmFieldStageIngressPMF2;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    nof_ids = -1;
    sal_strncpy_s((char *) name_to_id_info.name, "hgtvgnbg", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAceFormat;
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
    rv = bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity);
    SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
    if (rv != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found non existing name.\n");
    }
    if (nof_ids != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "nof ids (%d) isn't 0.\n", nof_ids);
    }

    /*
     * Test that no signal name repeats itself in the signal mapping.
     */
    {
        int signals_nof = dnx_data_field.virtual_wire.signals_nof_get(unit);
        int signal_index_1;
        int signal_index_2;
        dnx_field_stage_e dnx_field_stage_it;

        DNX_FIELD_STAGE_ITERATOR(dnx_field_stage_it)
        {
            for (signal_index_1 = 0; signal_index_1 < signals_nof; signal_index_1++)
            {
                if (sal_strncmp
                    ("EMPTY",
                     dnx_data_field.virtual_wire.signal_mapping_get(unit, dnx_field_stage_it,
                                                                    signal_index_1)->signal_name, MAX_VW_NAME_LENGTH))
                {
                    for (signal_index_2 = 0; signal_index_2 < signal_index_1; signal_index_2++)
                    {
                        /** Note we do not use sal_strncmp() because the size of the string is in SW state.*/
                        if (0 ==
                            sal_strncmp(dnx_data_field.virtual_wire.signal_mapping_get(unit, dnx_field_stage_it,
                                                                                       signal_index_1)->signal_name,
                                        dnx_data_field.virtual_wire.signal_mapping_get(unit,
                                                                                       dnx_field_stage_it,
                                                                                       signal_index_2)->signal_name,
                                        MAX_VW_NAME_LENGTH))
                        {
                            SHR_ERR_EXIT(_SHR_E_INTERNAL, "The signal name \"%.*s\" in stage \"%s\" appears more than "
                                         "once (in lines %d and %d).\n",
                                         MAX_VW_NAME_LENGTH,
                                         dnx_data_field.virtual_wire.signal_mapping_get(unit,
                                                                                        dnx_field_stage_it,
                                                                                        signal_index_1)->signal_name,
                                         dnx_field_stage_text(unit, dnx_field_stage_it), signal_index_2,
                                         signal_index_1);
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function is the basic virtual wire testing
 *   application.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_vw_starter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *vw_test_type_name;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_STR(DNX_DIAG_VW_OPTION_TEST_TYPE, vw_test_type_name);

    if (sal_strcasecmp(vw_test_type_name, TEST_TYPE_STAGE_MAP) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d. Tesing 'stage_map'. %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_vw_stage_map(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d. Testing 'stage_map' returned 'success' flag.%s%s\r\n",
                    __func__, __LINE__, EMPTY, EMPTY);
    }
    else if (sal_strcasecmp(vw_test_type_name, TEST_TYPE_NAME_MAP) == 0)
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d. Tesing 'name_map'. %s%s\r\n", __func__, __LINE__, EMPTY, EMPTY);
        SHR_IF_ERR_EXIT(ctest_dnx_vw_name_map(unit));
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d. Testing 'name_map' returned 'success' flag.%s%s\r\n",
                    __func__, __LINE__, EMPTY, EMPTY);
    }
    else
    {
        LOG_INFO_EX(BSL_LOG_MODULE, "%s(), line %d, Option %s is not implemented. Illegal parameter. %s\r\n",
                    __func__, __LINE__, vw_test_type_name, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - run virtual wire sequence in diag shell
 */
shr_error_e
sh_dnx_field_vw_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 count, count_iter;
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(args, _SHR_E_PARAM, "args");
    SHR_NULL_CHECK(sand_control, _SHR_E_PARAM, "sand_control");
    SH_SAND_GET_UINT32("count", count);
    for (count_iter = 0; count_iter < count; count_iter++)
    {
        SHR_IF_ERR_EXIT(ctest_dnx_vw_starter(unit, args, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}
