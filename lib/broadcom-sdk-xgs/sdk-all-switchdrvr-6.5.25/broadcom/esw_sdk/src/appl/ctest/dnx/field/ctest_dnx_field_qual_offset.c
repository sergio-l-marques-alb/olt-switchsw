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
 * $Copyright: (c) 2021 Broadcom.
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>

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
    {"dnx_data", "type=DNX_DATA", CTEST_POSTCOMMIT},
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
        "Checks the offset of different qualifiers\n"
        "Cleaning up all resources used after the test finishes\n"
        "For 'type=P_BUS':\n"
        "Just get offset for a few qualifiers of a few classes (META, META2, HEADER)\n"
        "and verify that the results are as expected.\n" "For 'type=DNX_DATA' checks the metadata qualifiers DNX DATA.",
    "ctest field qual_offset type=<P_BUS | KEY_TEMPLATE | DNX_DATA>",
    "type=KEY_TEMPLATE\n" "type=P_BUS\n" "type=DNX_DATA\n"
};

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
    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This functions runs verifications for DNX data mapping of qualifiers.
 *
 * \param [in] unit             - Device Id
 * \return
 *   shr_error_e -   Error Type
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
appl_dnx_field_qual_offset_dnx_data(
    int unit)
{
    int max_nof_quals_per_table;
    dnx_field_stage_e stage;
    int qual_ndx;
    int regular_offset;
    uint32 qual_size;
    int pbus_size;
    int nof_mapped_parts;
    int qual_part_ndx;
    int total_size;
    int part_offset;
    int part_size;
    dbal_fields_e field_id = DBAL_FIELD_EMPTY;
    dnx_field_map_stage_info_t field_map_stage_info;
    SHR_FUNC_INIT_VARS(unit);

    max_nof_quals_per_table = dnx_data_field_map.qual.params_info_get(unit)->key_size[1];
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in iPMF1 is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in iPMF2 is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in iPMF3 is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in ePMF is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in iFWD2 is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }
    if (max_nof_quals_per_table < DNX_FIELD_QUAL_NOF)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Maximum number of qualifiers per stage is %d, but maximum number of qualifiers in L4OPS is %d.\n",
                     max_nof_quals_per_table, DNX_FIELD_QUAL_NOF);
    }

    /*
     * Go over regular mappings.
     */
    for (stage = 0; stage < DNX_FIELD_STAGE_NOF; stage++)
    {
        int max_pbus_size_used = 0;
        SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, stage, &field_map_stage_info));
        if (stage == DNX_FIELD_STAGE_IPMF2)
        {
            pbus_size = dnx_data_field.stage.stage_info_get(unit, stage)->nof_bits_native_pbus;
        }
        else
        {
            pbus_size = dnx_data_field.stage.stage_info_get(unit, stage)->nof_bits_main_pbus;
        }
        for (qual_ndx = 0; qual_ndx < max_nof_quals_per_table; qual_ndx++)
        {
            regular_offset = dnx_data_field_map.qual.params_get(unit, stage, qual_ndx)->offset;
            if (regular_offset == (-1))
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.field_id.get(unit, qual_ndx, &field_id));

            if (regular_offset < 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has offset %d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             regular_offset);
            }
            if (regular_offset >= pbus_size)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has offset %d, pbus size %d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             regular_offset, pbus_size);
            }

            qual_size = dnx_data_field_map.qual.params_get(unit, stage, qual_ndx)->size;
            if ((qual_size < 1) || (qual_size > dnx_data_field.qual.max_bits_in_qual_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has size %d, range is %d-%d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             qual_size, 1, dnx_data_field.qual.max_bits_in_qual_get(unit));
            }
            if (regular_offset + qual_size > pbus_size)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has offset %d size %d, pbus size is %d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             regular_offset, qual_size, pbus_size);
            }
            if (regular_offset + qual_size > max_pbus_size_used)
            {
                max_pbus_size_used = regular_offset + qual_size;
            }
            /*
             * Since a regular mapping was found, check that no special mapping is found.
             */
            nof_mapped_parts = dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, qual_ndx)->nof_parts;
            if (nof_mapped_parts != 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has "
                             "regular mapping offset %d and special nof_parts %d. Cannot be mapped in both.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             regular_offset, nof_mapped_parts);
            }
        }
        if (max_pbus_size_used != pbus_size)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "In stage %s, the maximum pbus size used by regular qualifiers is %d. "
                         "Pbus size is %d. Potentially a misconfiguration.\n",
                         dnx_field_stage_text(unit, stage), max_pbus_size_used, pbus_size);
        }
    }

    /*
     * Go over special mappings.
     */
    for (stage = 0; stage < DNX_FIELD_STAGE_NOF; stage++)
    {
        SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, stage, &field_map_stage_info));
        if (stage == DNX_FIELD_STAGE_IPMF2)
        {
            pbus_size = dnx_data_field.stage.stage_info_get(unit, stage)->nof_bits_native_pbus;
        }
        else
        {
            pbus_size = dnx_data_field.stage.stage_info_get(unit, stage)->nof_bits_main_pbus;
        }
        for (qual_ndx = 0; qual_ndx < max_nof_quals_per_table; qual_ndx++)
        {
            nof_mapped_parts = dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, qual_ndx)->nof_parts;
            if (nof_mapped_parts == 0)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.predefined.field_id.get(unit, qual_ndx, &field_id));
            if (nof_mapped_parts < 0)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has nof_parts %d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             nof_mapped_parts);
            }
            if (nof_mapped_parts > DNX_FIELD_QAUL_MAX_NOF_MAPPINGS)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has nof_parts %d, maximum %d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             nof_mapped_parts, DNX_FIELD_QAUL_MAX_NOF_MAPPINGS);
            }
            total_size = 0;
            for (qual_part_ndx = 0; qual_part_ndx < nof_mapped_parts; qual_part_ndx++)
            {
                part_offset =
                    dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, qual_ndx)->offsets[qual_part_ndx];
                part_size =
                    dnx_data_field_map.qual.special_metadata_qual_get(unit, stage, qual_ndx)->sizes[qual_part_ndx];
                if (part_offset >= pbus_size)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s part %d has offset %d, pbus size %d.\n",
                                 dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                                 qual_part_ndx, part_offset, pbus_size);
                }
                if ((part_size < 1) || (part_size > dnx_data_field.qual.max_bits_in_qual_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s part %d has size %d, range is %d-%d.\n",
                                 dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                                 qual_part_ndx, part_size, 1, dnx_data_field.qual.max_bits_in_qual_get(unit));
                }
                if (part_offset + part_size > pbus_size)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s part %d has offset %d size %d, "
                                 "pbus size is %d.\n",
                                 dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                                 qual_part_ndx, part_offset, part_size, pbus_size);
                }
                total_size += part_size;
            }

            if ((total_size < 1) || (total_size > dnx_data_field.qual.max_bits_in_qual_get(unit)))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has size (sum of all parts) %d, "
                             "range is %d-%d.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             total_size, 1, dnx_data_field.qual.max_bits_in_qual_get(unit));
            }

            /*
             * Since a special mapping was found, check that no regular mapping is found.
             */
            regular_offset = dnx_data_field_map.qual.params_get(unit, stage, qual_ndx)->offset;
            if (regular_offset != (-1))
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Qualifier %s(%d) in stage %s has "
                             "special mapping nof_parts %d and regualr mapping offset %d. Cannot be mapped in both.\n",
                             dbal_field_to_string(unit, field_id), qual_ndx, dnx_field_stage_text(unit, stage),
                             nof_mapped_parts, regular_offset);
            }
        }
    }

    /*
     * Go over In_port as containing PP_PORT and core ID in ingress
     */
    {
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);
        int core_max_nof_bits = dnx_data_device.general.core_max_nof_bits_get(unit);
        int pp_port_size_in_pbus = dnx_data_port.general.nof_port_bits_in_pp_bus_get(unit);
        int pp_port_size = dnx_data_port.general.pp_port_bits_size_get(unit);

        if (nof_cores <= 0)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "nof_cores is %d.\n", nof_cores);
        }
        else if (nof_cores == 1)
        {
            if (core_max_nof_bits != 1)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device has one core, expected core_max_nof_bits to be 1, but it is %d.\n",
                             core_max_nof_bits);
            }

        }
        else
        {
            int expected_nof_bits = utilex_log2_round_up(nof_cores);
            if (core_max_nof_bits != expected_nof_bits)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "Device has %d cores, expected %d bits, but it is %d.\n",
                             nof_cores, expected_nof_bits, core_max_nof_bits);
            }
        }

        if (pp_port_size > pp_port_size_in_pbus)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "nof_port_bits_in_pp_bus (%d) is smaller than pp_port_bits_size (%d).\n",
                         pp_port_size_in_pbus, pp_port_size);
        }

        for (stage = 0; stage < DNX_FIELD_STAGE_NOF; stage++)
        {
            int in_port_offset;
            int core_id_offset;
            int pp_port_offset;
            int in_port_size;
            int core_id_size;
            int pp_port_size;

            SHR_IF_ERR_EXIT(dnx_field_map_stage_info_get_dispatch(unit, stage, &field_map_stage_info));
            in_port_offset = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_IN_PORT)->offset;
            core_id_offset = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_CORE_ID)->offset;
            pp_port_offset = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_PP_PORT)->offset;
            in_port_size = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_IN_PORT)->size;
            core_id_size = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_CORE_ID)->size;
            pp_port_size = dnx_data_field_map.qual.params_get(unit, stage, DNX_FIELD_QUAL_PP_PORT)->size;

            if (core_id_offset >= 0)
            {
                if (core_id_size != core_max_nof_bits)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Expected core_id qualifier to have size %d, but it is %d. stage %s\n",
                                 core_max_nof_bits, core_id_size, dnx_field_stage_text(unit, stage));
                }
            }

            if (pp_port_offset >= 0)
            {
                if (pp_port_size != pp_port_size_in_pbus)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "Expected pp_port qualifier to have size %d, but it is %d. stage %s\n",
                                 pp_port_size_in_pbus, pp_port_size, dnx_field_stage_text(unit, stage));
                }
            }

            /*
             * in_port is expected to include the core ID plus pp_port, except when there is only one core,
             * in which case it is only the pp port.
             */

            if (in_port_offset >= 0)
            {
                if ((core_id_offset < 0) || (pp_port_offset < 0))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %s, we have in_port qualifier, "
                                 "so we must also have core_id and pp_port qualifiers.\n",
                                 dnx_field_stage_text(unit, stage));
                }
                if (nof_cores == 1)
                {
                    if (in_port_offset != pp_port_offset)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "We have 1 core, and yet in stage %s in_port qualifier does not "
                                     "have the same offset as pp_port qualifier (%d vs %d).\n",
                                     dnx_field_stage_text(unit, stage), in_port_offset, pp_port_offset);
                    }
                    if (in_port_size != pp_port_size)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "We have one core, and yet in stage %s in_port qualifier does not "
                                     "have the same size as pp_port qualifier (%d vs %d).\n",
                                     dnx_field_stage_text(unit, stage), in_port_size, pp_port_size);
                    }
                }
                else
                {
                    if (in_port_offset != core_id_offset)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "We have more than one core (%d), and yet in stage %s in_port qualifier does not "
                                     "have the same offset as core_id qualifier (%d vs %d).\n",
                                     nof_cores, dnx_field_stage_text(unit, stage), in_port_offset, core_id_offset);
                    }
                    if (in_port_size != pp_port_size + core_id_size)
                    {
                        SHR_ERR_EXIT(_SHR_E_CONFIG,
                                     "We have more than one core (%d), and yet in stage %s in_port qualifier does not "
                                     "have the same size as pp_port qualifier plus core_id qualifier (%d vs %d+%d).\n",
                                     nof_cores, dnx_field_stage_text(unit, stage),
                                     in_port_size, pp_port_size, core_id_size);
                    }
                }
                if (pp_port_offset != (core_id_offset + core_id_size))
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %s we have an in_port qualifier, but pp_port (offset %d) "
                                 "does not come immediately after core_id qualifier (offset %d size %d).\n",
                                 dnx_field_stage_text(unit, stage), pp_port_offset, core_id_offset, core_id_size);
                }
            }

            if ((core_id_offset >= 0) && (pp_port_offset >= 0) && ((core_id_offset + core_id_size) == pp_port_offset))
            {
                if (in_port_offset < 0)
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "In stage %s, we have both core_id qualifier before pp_port qualifier, "
                                 "so we should also have in_port qualifier.\n", dnx_field_stage_text(unit, stage));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Test using bcm_field_group_qualifier_offset_get() or bcm_field_qualifier_info_get().
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
    int offset;
    int src_ip_offset = 0;
    int dst_ip_offset = 32;
    int src_mac_offset = 64;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_ENUM(CTEST_DNX_QUAL_OFFSET_OPTION_TYPE, qual_offset_test_type);
    if (qual_offset_test_type == 2)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_qual_offset_dnx_data(unit));
    }
    else if (qual_offset_test_type == 1)
    {
        /*
         * Create TCAM FG (SrcIP,DstIP,SrcMac) 
         */
        SHR_IF_ERR_EXIT(appl_dnx_field_qual_offset_fg_tcam_create(unit, &fg_tcam_id));
        /*
         * Validate group_qual_offset_get function 
         */
        SHR_IF_ERR_EXIT(bcm_field_group_qualifier_offset_get(unit, 0, fg_tcam_id, bcmFieldQualifySrcIp, &offset));
        if (offset != src_ip_offset)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Offset Mismatch, bcm_field_group_qualifier_offset_get returned %d while source IP offset should be %d\r\n",
                         offset, src_ip_offset);
        }
        SHR_IF_ERR_EXIT(bcm_field_group_qualifier_offset_get(unit, 0, fg_tcam_id, bcmFieldQualifyDstIp, &offset));
        if (offset != dst_ip_offset)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Offset Mismatch, bcm_field_group_qualifier_offset_get returned %d while destination IP offset should be %d\r\n",
                         offset, dst_ip_offset);
        }
        SHR_IF_ERR_EXIT(bcm_field_group_qualifier_offset_get(unit, 0, fg_tcam_id, bcmFieldQualifySrcMac, &offset));
        if (offset != src_mac_offset)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL,
                         "Offset Mismatch, bcm_field_group_qualifier_offset_get returned %d while SrcMac offset should be %d\r\n",
                         offset, src_mac_offset);
        }
        /*
         * Cleanup 
         */
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_tcam_id));
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
                    dnx_data_field_map.qual.params_get(unit, dnx_qual_stage, dnx_qual_id)->offset;

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
