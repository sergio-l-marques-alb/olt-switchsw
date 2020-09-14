/** \file ctest_dnx_field_qual_offset.c
 *
 * This ctest is for semantic verification of the BCM level of the function
 * bcm_field_group_qualifier_offset_get() that returns the offset of a given
 * qualifier inside the key of the given field group.
 * Also, a non-comprehensive test of the offset parameter of the API 'bcm_dnx_field_qualifier_info_get()' which
 * provides the offset, on PBUS (also, in some cases, relative) of a specified BCM
 * qualifier.
 */
/*
 * $Copyright: (c) 2020 Broadcom.
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
#include <bcm/field.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_qual_offset.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */
/**
 * Plan for testing of offset returned from bcm_field_qualifier_info_get():
 * Input parameters and expected offset.
 * See bcm_dnx_field_qualifier_info_get
 */
dnx_field_qual_offset_info_t Qual_offset_info[] =
{
    /*
     * Relative qualifier (with repect to header - for example: IP)
     * Not supported on IPMF3
     */
    {.qual_descriptor = "Testing HEADER qualifier", .bcm_stage = bcmFieldStageIngressPMF2,
            .bcm_qualifier = bcmFieldQualifySrcMac, .expected_offset = 48},
    /*
     * Absolute on PBUS. But same for all KBP qualifiers.
     * Only supported on IPMF1/2
     */
    {.qual_descriptor = "Testing KBP qualifier", .bcm_stage = bcmFieldStageIngressPMF2,
            .bcm_qualifier = bcmFieldQualifyExternalHit0, .expected_offset = 2416},
    /*
     * Relative qualifier (w.r.t. layer record).
     */
    {.qual_descriptor = "Testing LAYER RECORD qualifier", .bcm_stage = bcmFieldStageIngressPMF1,
            .bcm_qualifier = bcmFieldQualifyLayerRecordQualifier, .expected_offset = 8},
    /*
     * Absolute qualifier. Meta/Meta2. Dependent on Stage.
     */
    {.qual_descriptor = "Testing META   qualifier, IPMF1", .bcm_stage = bcmFieldStageIngressPMF1,
        .bcm_qualifier = bcmFieldQualifyL4PortRangeCheck, .expected_offset = 1841},
    {.qual_descriptor = "Testing META   qualifier, EPMF", .bcm_stage = bcmFieldStageEgress,
        .bcm_qualifier = bcmFieldQualifyL4PortRangeCheck, .expected_offset = (1214+1167)},
    /*
     * META2 qualifiers get an extra bit which is set to indicate that. QUAL_OFFSET_META2_BIT
     */
    {.qual_descriptor = "Testing META2  qualifier, IPMF2", .bcm_stage = bcmFieldStageIngressPMF2,
        .bcm_qualifier = bcmFieldQualifyHashValue, .expected_offset = (29 | SAL_BIT(QUAL_OFFSET_META2_BIT))},
    /*
     * Absolute qualifier. Meta. Dependent on Stage.
     */
    {.qual_descriptor = "Testing META   qualifier, IFWD2", .bcm_stage = bcmFieldStageExternal,
        .bcm_qualifier = bcmFieldQualifyGeneratedTtl, .expected_offset = 2949},
    {.qual_descriptor = "Testing META   qualifier, IPMF3", .bcm_stage = bcmFieldStageIngressPMF3,
        .bcm_qualifier = bcmFieldQualifyGeneratedTtl, .expected_offset = 1152},
    {.qual_descriptor = "Testing META   qualifier, IPMF1", .bcm_stage = bcmFieldStageIngressPMF1,
        .bcm_qualifier = bcmFieldQualifyGeneratedTtl, .expected_offset = 2012},
    {.qual_descriptor = "Testing META   qualifier, EPMF", .bcm_stage = bcmFieldStageEgress,
        .bcm_qualifier = bcmFieldQualifyGeneratedTtl, .expected_offset = (286 +1167)},

    {.qual_descriptor = "Testing META   qualifier, EPMF", .bcm_stage = bcmFieldStageEgress,
        .bcm_qualifier = bcmFieldQualifyFhei, .expected_offset = (373 +1167)},
    /*
     * End of table. DO NOT remove.
     */
    {.qual_descriptor = NULL, .bcm_stage = 0, .bcm_qualifier = 0, .expected_offset = 0}
};
/**
 * \brief
 *   Options list for 'qual_offset' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_qual_offset_options[] = {
     /* Name */                                 /* Type */              /* Description */                     /* Default */
    {CTEST_DNX_QUAL_OFFSET_OPTION_TYPE,       SAL_FIELD_TYPE_ENUM,    "Type of test (P_BUS or KEY_TEMPLATE)",     "KEY_TEMPLATE",     (void *)Field_offset_enum_table},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'qual_offset' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_qual_offset_tests[] = {
    {"group", "type=KEY_TEMPLATE", CTEST_POSTCOMMIT},
    {"pbus", "type=P_BUS", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_qual_offset_man = {
    "Field Qual Offset feature tests",
    "For 'type=KEY_TEMPLATE':\n"
        "Creating a field group with multiple qualifiers\n"
        "Create field group with quals at MSB part of key (like state table)\n"
        "Check the offset of different qualifiers in both field groups\n"
        "Cleaning up all resources used after the test finishes\n"
        "For 'type=P_BUS':\n"
        "Just get offset for a few qualifiers of a few classes (META, META2, HEADER)\n"
        "and verify that the results are as expected.",
    "ctest field qual_offset type=<P_BUS | KEY_TEMPLATE>",
    "type=KEY_TEMPLATE\n" "type=P_BUS\n"
};

static shr_error_e
appl_dnx_field_qual_offset_fg_st_create(
    int unit,
    bcm_field_group_t * fg_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_qualify_t qual_id;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeStateTable;
    fg_info.stage = bcmFieldStageIngressPMF1;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    /** Below J2P: {1b wr bit}{8bits data}{10 bits address} */
    qual_info.size = 11;
    /** For J2P and above, create an action. */
    if (dnx_data_field.features.state_table_atomic_rmw_get(unit))
    {
        bcm_field_action_info_t action_info;
        bcm_field_action_t action_id;
        bcm_field_action_info_t_init(&action_info);
        action_info.action_type = bcmFieldActionVoid;
        action_info.stage = bcmFieldStageIngressPMF1;
        action_info.size = 8;
        SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &action_id));

        fg_info.nof_actions = 1;
        fg_info.action_types[0] = action_id;
        fg_info.action_with_valid_bit[0] = FALSE;

        /** Above J2P: {5b opcode}{13bits address}{8 bits data} */
        qual_info.size = 18;
    }

    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &qual_id));

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = qual_id;
    fg_info.qual_types[1] = bcmFieldQualifyIp4Ttl;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
appl_dnx_field_qual_offset_fg_tcam_create(
    int unit,
    bcm_field_group_t * fg_id)
{
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /*
     * Set quals 
     */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyDstIp;
    fg_info.qual_types[2] = bcmFieldQualifySrcMac;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionDropPrecedence;
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Simple case of qual_offset - configures two FGs, one with the key starting from LSB and
 *  one that is not, and then calls bcm_field_group_qualifier_info_get to verify that it
 *  returns the right offset for qualifiers in both these FGs.
 *
 * \param [in] unit - Device ID
 * \param [in] args - Extra arguments
 * \param [in] sand_control - Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *  \retval _SHR_E_NONE - success
 */
static shr_error_e
appl_dnx_field_qual_offset_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    unsigned int qual_offset_test_type;
    bcm_field_group_t fg_tcam_id;
    bcm_field_group_t fg_st_id;
    int offset;
    int src_mac_offset = 64;
    int ttl_offset = 152;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM(CTEST_DNX_QUAL_OFFSET_OPTION_TYPE, qual_offset_test_type);
    if (qual_offset_test_type == 1)
    {
        /*
         * Create TCAM FG (SrcIP,DstIP,SrcMac) 
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_qual_offset_fg_tcam_create(unit, &fg_tcam_id));
        /*
         * Create State Table FG (11b qual, TTL) 
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_qual_offset_fg_st_create(unit, &fg_st_id));
        /*
         * Validate group_qual_offset_get function 
         */
        SHR_IF_ERR_EXIT(bcm_field_group_qualifier_offset_get(unit, 0, fg_tcam_id, bcmFieldQualifySrcMac, &offset));
        if (offset != src_mac_offset)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Offset Mismatch, bcm_field_group_qualifier_offset_get returned %d while SrcMac offset should be %d\r\n",
                         offset, src_mac_offset);
        }
        SHR_IF_ERR_EXIT(bcm_field_group_qualifier_offset_get(unit, 0, fg_st_id, bcmFieldQualifyIp4Ttl, &offset));
        if (offset != ttl_offset)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Offset Mismatch, bcm_field_group_qualifier_offset_get returned %d while TTL offset should be %d\r\n",
                         offset, ttl_offset);
        }
        /*
         * Cleanup 
         */
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_tcam_id));
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_st_id));
    }
    else if (qual_offset_test_type == 0)
    {
        bcm_field_stage_t bcm_stage;
        bcm_field_qualify_t bcm_qualifier;
        int test_index;
        bcm_field_qualifier_info_get_t qual_info_get;
        /*
         * Test the following qualifiers:
         * bcmFieldQualifySrcMac (All stages),  DNX_FIELD_QUAL_CLASS_HEADER,
         * bcmFieldQualifyLayerRecordQualifier, DNX_FIELD_QUAL_CLASS_LAYER_RECORD,
         * bcmFieldQualifyL4PortRangeCheck,     META, IPMF1
         * bcmFieldQualifyHashValue,            META2, IPMF2
         * bcmFieldQualifyGeneratedTtl,         META, IPMF3
         * bcmFieldQualifyFhei,                 META, EPMF
         */
        for (test_index = 0;; test_index++)
        {
            dnx_field_stage_e dnx_stage;
            dnx_field_qual_t dnx_qual;
            dnx_field_qual_class_e dnx_qual_class;
            if (Qual_offset_info[test_index].qual_descriptor == NULL)
            {
                break;
            }
            bcm_stage = Qual_offset_info[test_index].bcm_stage;
            bcm_qualifier = Qual_offset_info[test_index].bcm_qualifier;
            SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get(unit, bcm_qualifier, bcm_stage, &qual_info_get));
            SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));
            SHR_IF_ERR_EXIT(dnx_field_map_qual_bcm_to_dnx(unit, dnx_stage, bcm_qualifier, &dnx_qual));
            dnx_qual_class = DNX_QUAL_CLASS(dnx_qual);
            if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_META || dnx_qual_class == DNX_FIELD_QUAL_CLASS_META2)
            {
                dnx_field_stage_e dnx_qual_stage = DNX_QUAL_STAGE(dnx_qual);
                int dnx_qual_id = DNX_QUAL_ID(dnx_qual);
                if ((dnx_qual_stage == DNX_FIELD_STAGE_IPMF2) && (dnx_qual_class != DNX_FIELD_QUAL_CLASS_META2))
                {
                    /** Switch to info on IPMF1 qualifiers. */
                    dnx_qual_stage = DNX_FIELD_STAGE_IPMF1;
                }
                Qual_offset_info[test_index].expected_offset =
                    dnx_data_field.qual.params_get(unit, dnx_qual_stage, dnx_qual_id)->offset;

                if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_META2)
                {
                    Qual_offset_info[test_index].expected_offset =
                        (Qual_offset_info[test_index].expected_offset | SAL_BIT(QUAL_OFFSET_META2_BIT));
                }
            }
            if (qual_info_get.offset != Qual_offset_info[test_index].expected_offset)
            {
                if (dnx_qual_class == DNX_FIELD_QUAL_CLASS_META2)
                {
                    LOG_ERROR_EX(BSL_LOG_MODULE,
                                 "Note the %s is a META2 qualifier and should have bit %d set %s%s\r\n",
                                 dnx_field_bcm_qual_text(unit, bcm_qualifier), QUAL_OFFSET_META2_BIT, EMPTY, EMPTY);
                }
                SHR_ERR_EXIT(_SHR_E_FAIL,
                             "\r\n"
                             "Offset Mismatch on BCM qualifier %s on stage %s,\r\n"
                             "bcm_field_qualifier_info_get returned offset %d while expected offset should be %d\r\n",
                             dnx_field_bcm_qual_text(unit, bcm_qualifier),
                             dnx_field_bcm_stage_text(bcm_stage), qual_info_get.offset,
                             Qual_offset_info[test_index].expected_offset);
            }
            else
            {
                LOG_INFO_EX(BSL_LOG_MODULE,
                            "%s: BCM qualifier %s on stage %s is on offset %d\r\n",
                            Qual_offset_info[test_index].qual_descriptor,
                            dnx_field_bcm_qual_text(unit, bcm_qualifier),
                            dnx_field_bcm_stage_text(bcm_stage),
                            qual_info_get.offset & SAL_RBIT(QUAL_OFFSET_META2_BIT));
            }
        }
        LOG_INFO_EX(BSL_LOG_MODULE, "NO error detected. All is OK. %s%s%s%s\r\n", EMPTY, EMPTY, EMPTY, EMPTY);
    }
    else
    {
        /*
         * This should not happen since CLI machine should not allow these values.
         */
        LOG_INFO_EX(BSL_LOG_MODULE,
                    "%s(), line %d, Option %d (for 'qual_offset_test_type') is not implemented. Illegal parameter. %s\r\n",
                    __func__, __LINE__, qual_offset_test_type, EMPTY);
        SHR_IF_ERR_EXIT(_SHR_E_PARAM);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - runs the qual_offset ctest
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "ctest field qual_offset"
 */
shr_error_e
sh_dnx_field_qual_offset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_qual_offset_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
