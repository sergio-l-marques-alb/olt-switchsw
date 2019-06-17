/** \file field_map.c
 * $Id$
 *
 * Field database procedures for DNX.
 *
 * This file implements mapping from BCM to DNX qualifiers and access for per DNX qualifier/action information
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDPROCDNX
/*
 * Include files.
 * {
 */

#include <shared/utilex/utilex_rhlist.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/dnx/dbal/dbal_structures.h>
#include <soc/dnx/dbal/dbal_string_mgmt.h>
#include <soc/sand/sand_signals.h>

#include <soc/dnx/swstate/auto_generated/access/dnx_field_qualifier_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_actions_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_apptype_access.h>
#include <soc/dnx/swstate/auto_generated/types/dnx_field_context_types.h>

#include <soc/dnx/swstate/auto_generated/types/dnx_algo_field_types.h>

#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>

#include <bcm/field.h>

#include "field_map_local.h"
/*
 * }Include files
 */

/*
 * See field_map.h
 */
char *
dnx_field_stage_text(
    int unit,
    dnx_field_stage_e stage)
{
    char *stage_n;
    dnx_pp_stage_e pp_stage;

    if (stage == DNX_FIELD_STAGE_INVALID)
    {   /** Intentionally use INVALID(-1) as designation for global objects */
        stage_n = "Global";
    }
    else if (((int) stage < DNX_FIELD_STAGE_FIRST) || (stage >= DNX_FIELD_STAGE_NOF))
    {
        stage_n = "Invalid Stage";
    }
    else if ((pp_stage = dnx_field_map_stage_info[stage].pp_stage) == DNX_PP_STAGE_INVALID)
    {
        stage_n = "Unnamed Stage";
    }
    else
    {
        /*
         * Temporary keep pp_stage in map_stage_info and use it to fetch dnx_data
         * will be obsolete after replacement of dnx_field_stage by dnx_pp-stage
         */
        stage_n = dnx_pp_stage_name(unit, pp_stage);
    }

    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_stage_text(
    bcm_field_stage_t bcm_stage)
{
    char *stage_n;
    char *bcm_stage_text[bcmFieldStageCount] = BCM_FIELD_STAGE_STRINGS;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        stage_n = "Invalid Stage";
    }
    else
    {
        stage_n = bcm_stage_text[bcm_stage];
    }
    return stage_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_layer_type_text(
    bcm_field_layer_type_t bcm_layer_type)
{
    char *layer_type_n;
    char *bcm_layer_type_text[bcmFieldLayerTypeCount] = BCM_FIELD_LAYER_TYPE_STRINGS;

    if ((bcm_layer_type < bcmFieldLayerTypeFirst) || (bcm_layer_type >= bcmFieldLayerTypeCount))
    {
        layer_type_n = "Invalid Stage";
    }
    else
    {
        layer_type_n = bcm_layer_type_text[bcm_layer_type];
    }
    return layer_type_n;
}

/*
 * See field_map.h
 */
char *
dnx_field_input_type_text(
    dnx_field_input_type_e input_type)
{
    char *input_type_name;
    if ((input_type < DNX_FIELD_INPUT_TYPE_LAYER_FWD) || (input_type >= DNX_FIELD_INPUT_TYPE_NOF))
    {
        input_type_name = "Invalid Input Type";
    }
    else if (dnx_field_input_types_names[input_type] == NULL)
    {
        input_type_name = "Unnamed Input Type";
    }
    else
    {
        input_type_name = (char *) dnx_field_input_types_names[input_type];
    }

    return input_type_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_input_type_text(
    bcm_field_input_types_t bcm_input_type)
{
    char *input_type_name;
    char *bcm_input_type_text[bcmFieldInputTypeCount] = BCM_FIELD_INPUT_TYPES_STRINGS;

    if ((bcm_input_type < bcmFieldInputTypeLayerFwd) || (bcm_input_type >= bcmFieldInputTypeCount))
    {
        input_type_name = "Invalid Input Type";
    }
    else
    {
        input_type_name = bcm_input_type_text[bcm_input_type];
    }
    return input_type_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_packet_remove_layer_text(
    bcm_field_packet_remove_layers_t bcm_packet_remove_layer)
{
    char *remove_layer_name;
    char *bcm_remove_layer_text[bcmFieldPacketRemoveLayerCount] = BCM_FIELD_PACKET_REMOVE_LAYER_STRINGS;

    if ((bcm_packet_remove_layer < bcmFieldPacketRemoveLayerOffset0)
        || (bcm_packet_remove_layer >= bcmFieldPacketRemoveLayerCount))
    {
        remove_layer_name = "Invalid Packet Remove Layer";
    }
    else
    {
        remove_layer_name = bcm_remove_layer_text[bcm_packet_remove_layer];
    }
    return remove_layer_name;
}

/*
 * See field_map.h
 */
char *
dnx_field_bcm_tcam_bank_allocation_mode_text(
    bcm_field_tcam_bank_allocation_mode_t bcm_tcam_bank_allocation_mode)
{
    char *tcam_bank_allocation_mode_name;
    /** The array size should be +1, because of the invalid value, which we have in the strings. */
    char *bcm_tcam_bank_allocation_mode_text[bcmFieldTcamBankAllocationModeCount] =
        BCM_FIELD_TCAM_BANK_ALLOCATION_MODE_STRINGS;

    if ((bcm_tcam_bank_allocation_mode < bcmFieldTcamBankAllocationModeAuto)
        || (bcm_tcam_bank_allocation_mode >= bcmFieldTcamBankAllocationModeCount))
    {
        tcam_bank_allocation_mode_name = "Invalid Tcam Bank Allocation Mode";
    }
    else
    {
        tcam_bank_allocation_mode_name = bcm_tcam_bank_allocation_mode_text[bcm_tcam_bank_allocation_mode];
    }
    return tcam_bank_allocation_mode_name;
}

/*
 * See field_map.h
 */
shr_error_e
dnx_field_map_sw_state_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_action_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all action are set to valid=FALSE by default*/
    SHR_IF_ERR_EXIT(dnx_field_qual_sw_db.init(unit));
     /** Assumption all SW state init to 0, hence all qualifier are set to valid=FALSE by default*/

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Init the mapping and information of apptypes and opcodes.
* \param [in] unit  - Device id
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
static shr_error_e
dnx_field_map_kbp_apptype_opcode_init(
    int unit)
{
    dbal_enum_value_field_kbp_fwd_opcode_e i_opcode;
    dbal_enum_value_field_kbp_fwd_opcode_e mapped_opcode;
    bcm_field_AppType_t i_apptype;
    int nof_static_apptypes;
    int nof_static_opcodes;
    int cs_profile_id_counter;
    SHR_FUNC_INIT_VARS(unit);

    nof_static_opcodes = sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0]);
    nof_static_apptypes = sizeof(dnx_field_static_apptype_map) / sizeof(dnx_field_static_apptype_map[0]);

    if (nof_static_opcodes != DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_opcode_info %d. Exepected %d.\n",
                     nof_static_opcodes, DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES);
    }
    if (nof_static_apptypes != bcmFieldAppTypeCount)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Unexpected number of elements in dnx_field_static_apptype_map %d. Exepected %d.\n",
                     nof_static_apptypes, bcmFieldAppTypeCount);
    }
    if (nof_static_opcodes > DNX_KBP_MAX_NOF_OPCODES)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "Number of elements in dnx_field_static_opcode_info is %d, "
                     "Exceeds maximum number of opcodes %d.\n", nof_static_opcodes, DNX_KBP_MAX_NOF_OPCODES);
    }

    if (DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                     "DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID isn't zero, leading to imporoper ititialization of "
                     "dnx_field_static_apptype_map.\n");
    }

    /*
     * Initialize the OPCODE to apptype mapping.
     */
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        dnx_field_static_opcode_info[i_opcode].app_type = DNX_BCM_ID_INVALID;
        /** The following line should have no effect, as the array is initialized.*/
        dnx_field_static_opcode_info[i_opcode].is_valid = FALSE;
    }

    /*
     * Go over all static apptypes and add their mapping to static opcodes.
     */
    for (i_apptype = 0; i_apptype < nof_static_apptypes; i_apptype++)
    {
        mapped_opcode = dnx_field_static_apptype_map[i_apptype].opcode_id;
        if (mapped_opcode == DBAL_ENUM_FVAL_KBP_FWD_OPCODE_INVALID)
        {
            continue;
        }
        if (mapped_opcode < 0 || mapped_opcode >= DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Apptype %d mapped to opcode %d. Maximum static opcode number is %d\n",
                         i_apptype, mapped_opcode, (DBAL_NOF_ENUM_KBP_FWD_OPCODE_VALUES - 1));
        }

        dnx_field_static_opcode_info[mapped_opcode].app_type = i_apptype;
        dnx_field_static_opcode_info[mapped_opcode].is_valid = TRUE;
    }

    /*
     * Allocated profile IDs. We allocate profiles for static apptypes with WITH_ID flag so that it will have the 
     * same mapping for all units in a multi unit system.
     * We do not allocate profile 0, the default profile.
     */
    cs_profile_id_counter = 1;
    for (i_opcode = 0; i_opcode < nof_static_opcodes; i_opcode++)
    {
        if (dnx_field_static_opcode_info[i_opcode].is_valid)
        {
            if (!SOC_WARM_BOOT(unit))
            {
                SHR_IF_ERR_EXIT(dnx_algo_field_entry_kbp_opcode_cs_profile_id_allocate
                                (unit, SW_STATE_ALGO_TEMPLATE_ALLOCATE_WITH_ID, &cs_profile_id_counter));
            }
            dnx_field_static_opcode_info[i_opcode].cs_profile_id = cs_profile_id_counter;
            cs_profile_id_counter++;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_map_kbp_apptype_opcode_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_cs_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_qual_init(unit));
    SHR_IF_ERR_EXIT(dnx_field_map_action_init(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ffc_type_to_field(
    int unit,
    dnx_field_stage_e stage,
    dnx_field_ffc_type_e ffc_type,
    dbal_fields_e * ffc_type_field_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ffc_type_field_p, _SHR_E_PARAM, "ffc_type_field_p");
    DNX_FIELD_STAGE_VERIFY(stage);

    *ffc_type_field_p = dnx_field_map_stage_info[stage].ffc_type_field_a[ffc_type];

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_table(
    int unit,
    dnx_field_stage_e stage,
    dbal_tables_e * ffc_table_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_table_name_p, _SHR_E_PARAM, "ffc_table_name_p");

    /*
     * TBD: Handle Initial ffc's
     */
    *ffc_table_name_p = dnx_field_map_stage_info[stage].ffc_table;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_to_ffc_instruction(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * ffc_instruction_name_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(ffc_instruction_name_p, _SHR_E_PARAM, "ffc_table_name_p");

    *ffc_instruction_name_p = dnx_field_map_stage_info[stage].ffc_instruction;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_stage_e * dnx_stage_p)
{
    dnx_field_stage_e dnx_stage;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_stage_p, _SHR_E_PARAM, "dnx_stage_p");

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Stage:%d \n", bcm_stage);
    }

    DNX_FIELD_STAGE_ITERATOR(dnx_stage)
    {
        if (dnx_field_map_stage_info[dnx_stage].bcm_stage == bcm_stage)
        {
            *dnx_stage_p = dnx_stage;
            SHR_EXIT();
        }
    }

    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "BCM Stage:\"%s\" is not supported\n", dnx_field_bcm_stage_text(bcm_stage));

exit:
    SHR_FUNC_EXIT;
}
/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_ctx_id_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_ctx_id)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_ctx_id, _SHR_E_PARAM, "dbal_ctx_id");

    *dbal_ctx_id = dnx_field_map_stage_info[stage].ctx_id;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_key_to_field(
    int unit,
    dnx_field_stage_e stage,
    dbal_fields_e * dbal_key)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_key, _SHR_E_PARAM, "dbal_key");

    *dbal_key = dnx_field_map_stage_info[stage].key_field;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field.h
 */
shr_error_e
dnx_field_map_stage_dnx_to_bcm(
    int unit,
    dnx_field_stage_e dnx_stage,
    bcm_field_stage_t * bcm_stage_p)
{
    bcm_field_stage_t bcm_stage;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_stage_p, _SHR_E_PARAM, "bcm_stage_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    bcm_stage = dnx_field_map_stage_info[dnx_stage].bcm_stage;

    if ((bcm_stage < bcmFieldStageFirst) || (bcm_stage >= bcmFieldStageCount))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "DNX Stage:\"%s\" has no BCM mapping\n", dnx_field_stage_text(unit, dnx_stage));
    }
    *bcm_stage_p = bcm_stage;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_bcm_to_dnx(
    int unit,
    bcm_field_group_type_t bcm_fg_type,
    dnx_field_group_type_e * dnx_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_fg_type_p, _SHR_E_PARAM, "dnx_fg_type_p");

    switch (bcm_fg_type)
    {
        case bcmFieldGroupTypeTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM;
            break;
        }
        case bcmFieldGroupTypeDirectExtraction:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION;
            break;
        }
        case bcmFieldGroupTypeExactMatch:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_EXEM;
            break;
        }

        case bcmFieldGroupTypeStateTable:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_STATE_TABLE;
            break;
        }
        case bcmFieldGroupTypeDirectMdb:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB;
            break;
        }
        case bcmFieldGroupTypeExternalTcam:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_KBP;
            break;
        }
        case bcmFieldGroupTypeConst:
        {
            *dnx_fg_type_p = DNX_FIELD_GROUP_TYPE_CONST;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Field Group Type:%d \n", bcm_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_group_type_dnx_to_bcm(
    int unit,
    dnx_field_group_type_e dnx_fg_type,
    bcm_field_group_type_t * bcm_fg_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_fg_type_p, _SHR_E_PARAM, "bcm_fg_type_p");

    switch (dnx_fg_type)
    {
        case DNX_FIELD_GROUP_TYPE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_TCAM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_EXTRACTION:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectExtraction;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_EXEM:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExactMatch;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_STATE_TABLE:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeStateTable;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_DIRECT_TABLE_MDB:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeDirectMdb;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_KBP:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeExternalTcam;
            break;
        }
        case DNX_FIELD_GROUP_TYPE_CONST:
        {
            *bcm_fg_type_p = bcmFieldGroupTypeConst;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Field Group Type:%d \n", dnx_fg_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_bcm_to_dnx(
    int unit,
    bcm_field_tcam_bank_allocation_mode_t bcm_bank_allocation_mode,
    dnx_field_tcam_bank_allocation_mode_e * dnx_bank_allocation_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (bcm_bank_allocation_mode)
    {
        case bcmFieldTcamBankAllocationModeAuto:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO;
            break;
        }
        case bcmFieldTcamBankAllocationModeSelect:
        {
            *dnx_bank_allocation_mode = DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Bank Allocation Mode:%d \n", bcm_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_bank_allocation_mode_dnx_to_bcm(
    int unit,
    dnx_field_tcam_bank_allocation_mode_e dnx_bank_allocation_mode,
    bcm_field_tcam_bank_allocation_mode_t * bcm_bank_allocation_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_bank_allocation_mode_p, _SHR_E_PARAM, "bcm_bank_allocation_mode_p");

    switch (dnx_bank_allocation_mode)
    {
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_AUTO:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeAuto;
            break;
        }
        case DNX_FIELD_TCAM_BANK_ALLOCATION_MODE_SELECT:
        {
            *bcm_bank_allocation_mode_p = bcmFieldTcamBankAllocationModeSelect;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Bank Allocation Mode:%d \n", dnx_bank_allocation_mode);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_bcm_to_dnx(
    int unit,
    bcm_field_context_compare_type_t bcm_context_compare_type,
    dnx_field_context_compare_mode_e * dnx_context_compare_mode_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_context_compare_mode_p, _SHR_E_PARAM, "dnx_context_compare_mode_p");

    switch (bcm_context_compare_type)
    {
        case bcmFieldContextCompareTypeNone:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_NONE;
            break;
        }
        case bcmFieldContextCompareTypeSingle:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE;
            break;
        }
        case bcmFieldContextCompareTypeDouble:
        {
            *dnx_context_compare_mode_p = DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid BCM Context Compare Type:%d \n", bcm_context_compare_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_context_compare_type_dnx_to_bcm(
    int unit,
    dnx_field_context_compare_mode_e dnx_context_compare_mode,
    bcm_field_context_compare_type_t * bcm_context_compare_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_compare_type_p, _SHR_E_PARAM, "bcm_context_compare_type_p");

    switch (dnx_context_compare_mode)
    {
        case DNX_FIELD_CONTEXT_COMPARE_MODE_NONE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeNone;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_SINGLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeSingle;
            break;
        }
        case DNX_FIELD_CONTEXT_COMPARE_MODE_DOUBLE:
        {
            *bcm_context_compare_type_p = bcmFieldContextCompareTypeDouble;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Context Compare Mode:%d \n", dnx_context_compare_mode);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_layer_type_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_layer_types_e dnx_layer_type,
    bcm_field_layer_type_t * bcm_layer_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_layer_type_p, _SHR_E_PARAM, "bcm_layer_type_p");

    switch (dnx_layer_type)
    {
        case DBAL_ENUM_FVAL_LAYER_TYPES_ETHERNET:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeEth;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV4:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp4;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPV6:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIp6;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_MPLS_UA:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeMplsUa;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ARP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeArp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_FCOE:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeFcoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_TCP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTcp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UDP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUdp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_SINGLE_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdSingleHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BFD_MULTI_HOP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBfdMultiHop;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PTP_EVENT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePtpEvent;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PTP_GENERAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePtpGeneral;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_Y_1731:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeY1731;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ICMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIcmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BIER_TI:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierTi;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_BIER_MPLS:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeBierMpls;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_RCH:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeRch;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_PPPOE_SESSION:
        {
            *bcm_layer_type_p = bcmFieldLayerTypePppoe;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_ENDPOINT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Endpoint;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_SRV6_BEYOND:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeSrv6Beyond;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IGMP:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIgmp;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPVX:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIpAny;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_IPT:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeIpt;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_VAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTm;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_ITMH_J1_VAL:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeTmLegacy;
            break;
        }
        case DBAL_ENUM_FVAL_LAYER_TYPES_UNKNOWN:
        {
            *bcm_layer_type_p = bcmFieldLayerTypeUnknown;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid DNX Layer Type:%d \n", dnx_layer_type);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_vlan_format_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_incoming_tag_structure_e dnx_vlan_format,
    uint32 *bcm_vlan_format_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_vlan_format_p, _SHR_E_PARAM, "bcm_vlan_format_p");

    switch (dnx_vlan_format)
    {
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_UNTAGGED_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_UNTAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_C_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
            break;
        }
        case DBAL_ENUM_FVAL_INCOMING_TAG_STRUCTURE_S_PRIORITY_C_TAG_1:
        {
            *bcm_vlan_format_p = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
            break;
        }
        default:
        {
            /*
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Vlan Format:\"%d\" is not supported\n", dnx_vlan_format);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_lookup_dbal_table_get(
    int unit,
    dnx_field_stage_e dnx_stage,
    dbal_tables_e * table_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(table_id_p, _SHR_E_PARAM, "table_id_p");

    DNX_FIELD_STAGE_VERIFY(dnx_stage);

    *table_id_p = dnx_field_map_stage_info[dnx_stage].lookup_enabler_table_id;
exit:
    SHR_FUNC_EXIT;
}

/*
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_compare_id_bcm_to_dnx(
    int unit,
    uint8 compare_id,
    dnx_field_group_compare_id_e * dnx_compare_id_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_compare_id_p, _SHR_E_PARAM, "dnx_compare_id_p");

    switch (compare_id)
    {
        case 0:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_NONE;
            break;
        }
        case 1:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_1;
            break;
        }
        case 2:
        {
            *dnx_compare_id_p = DNX_FIELD_GROUP_COMPARE_ID_CMP_2;
            break;
        }
        default:
        {
            /*
             * None of the supported compare Ids
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid comapre_id:%d \n", compare_id);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_context_param_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    dnx_field_context_t context_id,
    bcm_field_context_param_info_t * bcm_context_param_set_p,
    dnx_field_dbal_entry_t * field_dbal_entry_p)
{
    dnx_field_context_param_t *dnx_context_param_set;
    dnx_field_stage_e dnx_stage;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_context_param_set_p, _SHR_E_PARAM, "bcm_context_param_set_p");
    SHR_NULL_CHECK(field_dbal_entry_p, _SHR_E_PARAM, "field_dbal_entry_p");

    /** Convert BCM to DNX Field Stage. And verify it on way*/
    SHR_IF_ERR_EXIT(dnx_field_map_stage_bcm_to_dnx(unit, bcm_stage, &dnx_stage));

    dnx_context_param_set =
        (dnx_field_context_param_t *) & context_param_set[bcm_context_param_set_p->param_type][bcm_stage];
    if (dnx_context_param_set->table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Context Parameter Type %d for stage:%s is not supported",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
    /*
     * First of all initialize to NULL
     */
    sal_memset(field_dbal_entry_p, 0, sizeof(*field_dbal_entry_p));
    /*
     * Fetch table id
     */
    field_dbal_entry_p->table_id = dnx_context_param_set->table_id;
    if (dnx_context_param_set->conversion_cb != NULL)
    {
        /*
         * Conversion callback will take care of filling dbal fields and their values
         */
        SHR_IF_ERR_EXIT(dnx_context_param_set->conversion_cb(unit, dnx_stage, context_id,
                                                             bcm_context_param_set_p->param_val, dnx_context_param_set,
                                                             field_dbal_entry_p));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "No Data Conversion CB for Context Parameter:\"%d\" stage:\"%s\"\n",
                     bcm_context_param_set_p->param_type, dnx_field_bcm_stage_text(bcm_stage));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_range_bcm_to_dnx(
    int unit,
    bcm_field_stage_t bcm_stage,
    uint32 range_id,
    bcm_field_range_info_t * bcm_range_info_p,
    dnx_field_dbal_entry_t * field_dbal_entry_p)
{
    dnx_field_range_map_t *dnx_range_map;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_range_info_p, _SHR_E_PARAM, "bcm_range_info_p");
    SHR_NULL_CHECK(field_dbal_entry_p, _SHR_E_PARAM, "field_dbal_entry_p");

    dnx_range_map = (dnx_field_range_map_t *) & range_map[bcm_range_info_p->range_type][bcm_stage];
    if (dnx_range_map->table_id == DBAL_TABLE_EMPTY)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Range Type %d for stage:%s is not supported",
                     bcm_range_info_p->range_type, dnx_field_bcm_stage_text(bcm_stage));
    }
    /**
     * First of all initialize to NULL
     */
    sal_memset(field_dbal_entry_p, 0, sizeof(*field_dbal_entry_p));
    /**
     * Fetch table id
     */
    field_dbal_entry_p->table_id = dnx_range_map->table_id;
    /**
     *  Fill the dbal fields.
     */
    field_dbal_entry_p->key_dbal_pair[0].field_id = dnx_range_map->key_field_id[0];
    field_dbal_entry_p->key_dbal_pair[0].value = range_id;
    field_dbal_entry_p->res_dbal_pair[0].field_id = dnx_range_map->res_field_id[0];
    field_dbal_entry_p->res_dbal_pair[0].value = bcm_range_info_p->min_val;
    field_dbal_entry_p->res_dbal_pair[1].field_id = dnx_range_map->res_field_id[1];
    field_dbal_entry_p->res_dbal_pair[1].value = bcm_range_info_p->max_val;
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_function_t bcm_hash_func,
    dbal_enum_value_field_context_hash_function_e * dbal_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_func_p, _SHR_E_PARAM, "dbal_hash_func_p");
    switch (bcm_hash_func)
    {
        case bcmFieldContextHashFunctionFirstReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionSecondReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionThirdReserved:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Bisync:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor1:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor2:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor4:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Xor8:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8;
            break;
        }
        case bcmFieldContextHashFunctionXor16:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16;
            break;
        }
        case bcmFieldContextHashFunctionCrc16Ccitt:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT;
            break;
        }
        case bcmFieldContextHashFunctionCrc32ALow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32Ahigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BLow:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW;
            break;
        }
        case bcmFieldContextHashFunctionCrc32BHigh:
        {
            *dbal_hash_func_p = DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Function type: %d . Valid ranges from: %d to %d \n",
                         bcm_hash_func, bcmFieldContextHashFunctionFirst, (bcmFieldContextHashFunctionCount - 1));
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_func_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_context_hash_function_e dbal_hash_func,
    bcm_field_context_hash_function_t * bcm_hash_func_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_func_p, _SHR_E_PARAM, "bcm_hash_func_p");
    switch (dbal_hash_func)
    {
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_FIRST_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionFirstReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_SECOND_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionSecondReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_THIRD_RESERVED:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionThirdReserved;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_BISYNC:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Bisync;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR1:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor1;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR2:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor2;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR4:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor4;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_XOR8:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Xor8;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_XOR16:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionXor16;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC16_CCITT:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc16Ccitt;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_ALOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32ALow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_AHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32Ahigh;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BLOW:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BLow;
            break;
        }
        case DBAL_ENUM_FVAL_CONTEXT_HASH_FUNCTION_CRC32_BHIGH:
        {
            *bcm_hash_func_p = bcmFieldContextHashFunctionCrc32BHigh;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Function type: %d. \n", dbal_hash_func);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_bcm_to_dbal(
    int unit,
    bcm_field_context_hash_action_key_t bcm_hash_lb_key,
    dbal_enum_value_field_field_hash_lb_key_e * dbal_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dbal_hash_lb_key_p, _SHR_E_PARAM, "dbal_hash_lb_key_p");
    switch (bcm_hash_lb_key)
    {
        case bcmFieldContextHashActionKeyEcmpLbKey0:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey1:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB;
            break;
        }
        case bcmFieldContextHashActionKeyEcmpLbKey2:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB;
            break;
        }
        case bcmFieldContextHashActionKeyNetworkLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB;
            break;
        }
        case bcmFieldContextHashActionKeyLagLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB;
            break;
        }
        case bcmFieldContextHashActionKeyAdditionalLbKey:
        {
            *dbal_hash_lb_key_p = DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d ."
                         " Valid values from: %d to %d . \n",
                         bcm_hash_lb_key, bcmFieldContextHashActionKeyFirst, (bcmFieldContextHashActionKeyCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_lb_key_dbal_to_bcm(
    int unit,
    dbal_enum_value_field_field_hash_lb_key_e dbal_hash_lb_key,
    bcm_field_context_hash_action_key_t * bcm_hash_lb_key_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_lb_key_p, _SHR_E_PARAM, "bcm_hash_lb_key_p");
    switch (dbal_hash_lb_key)
    {
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP0_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey0;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP1_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey1;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ECMP2_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyEcmpLbKey2;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_NWK_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyNetworkLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_LAG_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyLagLbKey;
            break;
        }
        case DBAL_ENUM_FVAL_FIELD_HASH_LB_KEY_ADDITIONAL_LB:
        {
            *bcm_hash_lb_key_p = bcmFieldContextHashActionKeyAdditionalLbKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hashing Load Balancing Key type: %d. \n", dbal_hash_lb_key);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_bcm_to_dnx(
    int unit,
    bcm_field_context_hash_action_value_t bcm_hash_action,
    dnx_field_context_hash_action_e * dnx_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(dnx_hash_action_p, _SHR_E_PARAM, "dnx_hash_action_p");
    switch (bcm_hash_action)
    {
        case bcmFieldContextHashActionValueNone:
        {
            *dnx_hash_action_p = DNX_FIELD_CONTEXT_HASH_ACTION_NONE;
            break;
        }
        case bcmFieldContextHashActionValueReplaceCrc:
        {
            *dnx_hash_action_p = DNX_FIELD_CONTEXT_HASH_ACTION_REPLACE_CRC16;
            break;
        }
        case bcmFieldContextHashActionValueAugmentCrc:
        {
            *dnx_hash_action_p = DNX_FIELD_CONTEXT_HASH_ACTION_AUGMENT_CRC16;
            break;
        }
        case bcmFieldContextHashActionValueAugmentKey:
        {
            *dnx_hash_action_p = DNX_FIELD_CONTEXT_HASH_ACTION_AUGMENT_KEY_J;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. Valid values from: %d to %d . \n",
                         bcm_hash_action,
                         bcmFieldContextHashActionValueFirst, (bcmFieldContextHashActionValueCount - 1));
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_hash_action_dnx_to_bcm(
    int unit,
    dnx_field_context_hash_action_e dnx_hash_action,
    bcm_field_context_hash_action_value_t * bcm_hash_action_p)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bcm_hash_action_p, _SHR_E_PARAM, "bcm_hash_action_p");
    switch (dnx_hash_action)
    {
        case DNX_FIELD_CONTEXT_HASH_ACTION_NONE:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueNone;
            break;
        }
        case DNX_FIELD_CONTEXT_HASH_ACTION_REPLACE_CRC16:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueReplaceCrc;
            break;
        }
        case DNX_FIELD_CONTEXT_HASH_ACTION_AUGMENT_CRC16:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentCrc;
            break;
        }
        case DNX_FIELD_CONTEXT_HASH_ACTION_AUGMENT_KEY_J:
        {
            *bcm_hash_action_p = bcmFieldContextHashActionValueAugmentKey;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Hash Action type: %d. \n", dnx_hash_action);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_info_bcm_to_dnx(
    int unit,
    bcm_field_range_info_t bcm_range_info,
    dnx_field_range_info_t * dnx_range_info_p)
{
    uint32 max_val;
    SHR_FUNC_INIT_VARS(unit);
    max_val = -1;

    SHR_NULL_CHECK(dnx_range_info_p, _SHR_E_PARAM, "dnx_range_info_p");
    switch (bcm_range_info.range_type)
    {
        case bcmFieldRangeTypeL4SrcPort:
        {
            dnx_range_info_p->range_type = DNX_FIELD_RANGE_TYPE_L4_SRC_PORT;
            max_val = DNX_FIELD_RANGE_L4_OPS_MAX_VAL;
            break;
        }
        case bcmFieldRangeTypeL4DstPort:
        {
            dnx_range_info_p->range_type = DNX_FIELD_RANGE_TYPE_L4_DST_PORT;
            max_val = DNX_FIELD_RANGE_L4_OPS_MAX_VAL;
            break;
        }
        case bcmFieldRangeTypeOutVport:
        {
            dnx_range_info_p->range_type = DNX_FIELD_RANGE_TYPE_OUT_LIF;
            max_val = DNX_FIELD_RANGE_OUT_LIF_MAX_VAL;
            break;
        }
        case bcmFieldRangeTypePacketHeaderSize:
        {
            dnx_range_info_p->range_type = DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE;
            max_val = DNX_FIELD_RANGE_PKT_HDR_MAX_VAL;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Range Type:%d \n", bcm_range_info.range_type);
            break;
        }
    }
    /**
     * If the min_val was not initialized, set the specific minimum for this range!
     * Else set the min_val from the bcm structure!
     */
    if (bcm_range_info.min_val == DNX_FIELD_RANGE_ID_INVALID)
    {
        dnx_range_info_p->min_val = DNX_FIELD_RANGE_QUALS_MIN_VAL;
    }
    else
    {
        dnx_range_info_p->min_val = bcm_range_info.min_val;
    }
    /**
     * If the max_val was not initialized, set the specific maximum for this range!
     * Else set the max_val from the bcm structure!
     */
    if (bcm_range_info.max_val == DNX_FIELD_RANGE_ID_INVALID)
    {
        dnx_range_info_p->max_val = max_val;
    }
    else
    {
        dnx_range_info_p->max_val = bcm_range_info.max_val;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_range_info_dnx_to_bcm(
    int unit,
    dnx_field_range_info_t dnx_range_info,
    bcm_field_range_info_t * bcm_range_info_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(bcm_range_info_p, _SHR_E_PARAM, "bcm_range_info_p");

    switch (dnx_range_info.range_type)
    {
        case DNX_FIELD_RANGE_TYPE_L4_SRC_PORT:
        {
            bcm_range_info_p->range_type = bcmFieldRangeTypeL4SrcPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_L4_DST_PORT:
        {
            bcm_range_info_p->range_type = bcmFieldRangeTypeL4DstPort;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_OUT_LIF:
        {
            bcm_range_info_p->range_type = bcmFieldRangeTypeOutVport;
            break;
        }
        case DNX_FIELD_RANGE_TYPE_PKT_HDR_SIZE:
        {
            bcm_range_info_p->range_type = bcmFieldRangeTypePacketHeaderSize;
            break;
        }
        default:
        {
            /**
             * None of the supported types
             */
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Invalid Range Type:%d \n", dnx_range_info.range_type);
            break;
        }
    }
    /**
     * Map the min and max value.
     */
    bcm_range_info_p->min_val = dnx_range_info.min_val;
    bcm_range_info_p->max_val = dnx_range_info.max_val;
exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_apptype_to_opcode_bcm_to_dnx(
    int unit,
    bcm_field_AppType_t app_type,
    dbal_enum_value_field_kbp_fwd_opcode_e * opcode_id_p)
{
    int is_dynamic = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(opcode_id_p, _SHR_E_PARAM, "opcode_id_p");

    if (app_type < 0 || app_type >= (sizeof(dnx_field_static_apptype_map) / sizeof(dnx_field_static_apptype_map[0])))
    {
        if (app_type >= dnx_data_field.kbp.apptype_user_1st_get(unit) &&
            app_type < dnx_data_field.kbp.apptype_user_1st_get(unit) + dnx_data_field.kbp.apptype_user_nof_get(unit))
        {
            is_dynamic = TRUE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Apptype %d is out of range. ranges are %d-%d for static Apptypes, and "
                         "%d-%d for dynamic.\n",
                         app_type, 0,
                         (int) ((sizeof(dnx_field_static_apptype_map) / sizeof(dnx_field_static_apptype_map[0])) - 1),
                         dnx_data_field.kbp.apptype_user_1st_get(unit),
                         dnx_data_field.kbp.apptype_user_1st_get(unit) + dnx_data_field.kbp.apptype_user_nof_get(unit)
                         - 1);
        }
    }
    if (is_dynamic)
    {
        int relative_dyn_apptype = app_type - dnx_data_field.kbp.apptype_user_1st_get(unit);
        uint8 is_alloc;
        uint8 opcode_id_uint8;
        SHR_IF_ERR_EXIT(dnx_algo_field_context_apptype_is_allocated(unit, app_type, &is_alloc));
        if (is_alloc == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Dynamic Apptype %d is not allocated.\n", app_type);
        }
        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.info.opcode_id.get(unit, relative_dyn_apptype, &opcode_id_uint8));
        (*opcode_id_p) = opcode_id_uint8;
    }
    else
    {
        if (dnx_field_static_apptype_map[app_type].opcode_id == DNX_BCM_ID_INVALID)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Unmapped apptype %d.\n", app_type);
        }
        (*opcode_id_p) = dnx_field_static_apptype_map[app_type].opcode_id;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_opcode_to_apptype_dnx_to_bcm(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    bcm_field_AppType_t * app_type_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(app_type_p, _SHR_E_PARAM, "app_type_p");

    

    if (opcode_id < 0 || opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode_id %d is out of range. Maximum %d.\n",
                     opcode_id,
                     (int) ((sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])) - 1));
    }

    if (dnx_field_static_opcode_info[opcode_id].is_valid)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "opcode_id %d not found.\n", opcode_id);
    }

    (*app_type_p) = dnx_field_static_opcode_info[opcode_id].app_type;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_opcode_to_profile_get(
    int unit,
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id,
    uint32 *cd_profile_id_p)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cd_profile_id_p, _SHR_E_PARAM, "cd_profile_id_p");

    

    if (opcode_id < 0 || opcode_id >= (sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "opcode_id %d is out of range. Maximum %d.\n",
                     opcode_id,
                     (int) ((sizeof(dnx_field_static_opcode_info) / sizeof(dnx_field_static_opcode_info[0])) - 1));
    }

    if (dnx_field_static_opcode_info[opcode_id].is_valid == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "opcode_id %d not found.\n", opcode_id);
    }

    (*cd_profile_id_p) = dnx_field_static_opcode_info[opcode_id].cs_profile_id;

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_field_map_apptype_to_profile_get(
    int unit,
    bcm_field_AppType_t app_type,
    uint32 *cd_profile_id_p)
{
    
    dbal_enum_value_field_kbp_fwd_opcode_e opcode_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(cd_profile_id_p, _SHR_E_PARAM, "cd_profile_id_p");

    SHR_IF_ERR_EXIT(dnx_field_map_apptype_to_opcode_bcm_to_dnx(unit, app_type, &opcode_id));

    if (app_type >= 0 && app_type < bcmFieldAppTypeCount)
    {
        /** Static Apptypes.*/
        SHR_IF_ERR_EXIT(dnx_field_map_opcode_to_profile_get(unit, opcode_id, cd_profile_id_p));
    }
    else
    {
        int relative_dyn_apptype = app_type - dnx_data_field.kbp.apptype_user_1st_get(unit);
        uint8 prodile_id_uint8;

        SHR_IF_ERR_EXIT(dnx_field_apptype_sw.info.profile_id.get(unit, relative_dyn_apptype, &prodile_id_uint8));
        (*cd_profile_id_p) = prodile_id_uint8;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_port_profile_type_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_port_porfile_type_e * port_profile_type_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    (*port_profile_type_p) = dnx_field_map_port_profile_info[bcm_port_class].port_profile_type;

exit:
    SHR_FUNC_EXIT;
}

/**
 * See procedure header in field_map.h
 */
shr_error_e
dnx_field_map_port_profile_info_get(
    int unit,
    bcm_port_class_t bcm_port_class,
    dnx_field_map_port_profile_info_t * port_profile_info_p)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FIELD_NCM_PORT_CLASS_VERIFY(bcm_port_class);

    sal_memcpy(port_profile_info_p, &dnx_field_map_port_profile_info[bcm_port_class], sizeof(*port_profile_info_p));

exit:
    SHR_FUNC_EXIT;
}
