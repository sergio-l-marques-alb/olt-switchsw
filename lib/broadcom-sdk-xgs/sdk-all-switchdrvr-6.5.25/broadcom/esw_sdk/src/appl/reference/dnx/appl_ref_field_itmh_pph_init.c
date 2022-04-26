/** \file appl_ref_field_itmh_pph_init.c
 * $Id$
 *
 * ITMH Programmable mode application procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2021 Broadcom.
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
/** soc */
#include <soc/schanmsg.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <soc/sand/sand_aux_access.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <bcm_int/dnx/field/field_context.h>
#include "appl_ref_field_itmh_pph_init.h"

/** bcm */
#include <bcm/field.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

/*
 * }
 */

/* *INDENT-OFF* */

/*
 * DEFINEs
 * {
 */
#define ITMH_PPH_APPL_ITMH_LAYER    0
#define ITMH_PPH_APPL_ASE_TSH_LAYER 1
#define ITMH_PPH_APPL_PPH_LAYER     2

#define ITMH_IS_TSH_INJ     "is_tsh_inj"
/** Size of the ITMH */
#define HEADER_ITMH_SIZE    40
/** Size of the PPH */
#define HEADER_PPH_SIZE     96
#define VALID_BIT_LENGTH    1
/*
 * Flags for context IDs
 */
#define FLAG_BIT_ITMH_CONTEXT           0x1
#define FLAG_BIT_PPH_CONTEXT            0x2
#define FLAG_BIT_PPH_FHEI_CONTEXT       0x4
#define FLAG_BIT_PPH_FHEI_VLAN_CONTEXT  0x8
#define FLAG_BIT_ASE_1588_CONTEXT       0x10
/*
 * ITMH
 */
/**
 * TC in ITMH [38:36] (3b)
 * SNOOP_DATA in ITMH [35:31] (5b)
 * DST in ITMH [30:10] (21b)
 * DP in ITMH [9:8] (2b)
 * MIRROR_DISABLE in ITMH [7:7] (1b)
 * Total fields [7:38] (32b)
 */
#define ITMH_FIELDS_LENGTH      32
#define ITMH_FIELDS_OFFSET      7
#define ITMH_DST_LENGTH         21
#define MIRROR_CODE_LENGTH      9
#define SNOOP_DATA_LENGTH       5
/** ITMH_FWD_STR in ITMH [0:2] (3b) */
#define ITMH_FWD_STR_LENGTH     3
#define ITMH_FWD_STR_OFFSET     0
/** ASE_INJ in ITMH [4:4] (1b) */
#define ASE_INJ_LENGTH          1
#define ASE_INJ_OFFSET          4
/** TSH_INJ in ITMH [5:5] (1b) */
#define IS_TSH_INJ_LENGTH       1
#define TSH_INJ_OFFSET          5
/** EXTENSION_PRESENT in ITMH [39:39] (1b) */
#define EXT_PRESENT_LENGTH      1
#define EXT_PRESENT_OFFSET      39
/** OUT_LIF in ITMH-Ext [23:2] (22b) */
#define OUTLIF_LENGTH           22
#define OUTLIF_OFFSET           (HEADER_ITMH_SIZE + 2)
/*
 * ASE-OAM
 */
/** OAM_MEP_OFFSET in ASE_OAM [43:43] (1b) */
#define OAM_MEP_LENGTH          1
#define OAM_MEP_OFFSET          4
/** OAM_SUB_TYPE in ASE_OAM [47:44] (4b) */
#define OAM_SUB_TYPE_LENGTH     4
#define OAM_SUB_TYPE_OFFSET     0
/** OAM_OFFSET in ASE_OAM [8:1] (8b) */
#define OAM_OFFSET_LENGTH       8
#define OAM_OFFSET_OFFSET       39
/*
 * ASE-1588
 */
/** USE_INGRESS_TIME_COMPENSATION in ASE-1588 [30] (1b) */
#define USE_INGRESS_TIME_COMPENSATION_LENGTH     1
#define USE_INGRESS_TIME_COMPENSATION_OFFSET     30
/** USE_INGRESS_TIME_STAMP in ASE-1588 [31] (1b) */
#define USE_INGRESS_TIME_STAMP_LENGTH   1
#define USE_INGRESS_TIME_STAMP_OFFSET   31
/** TS_ENCAPSULATION in ASE-1588 [32] (1b) */
#define TS_ENCAPSULATION_LENGTH         1
#define TS_ENCAPSULATION_OFFSET         32
/** TS_COMMAND in ASE-1588 [34] (2b) */
#define TS_COMMAND_LENGTH               2
#define TS_COMMAND_OFFSET               34
/** OFFSET in ASE-1588 [36:43] (8b) */
#define ASE_1588_OFFSET_LENGTH          8
#define ASE_1588_OFFSET_OFFSET          36
/*
 * TSH
 */
/** TIME_STAMP in TSH [31:0] (32b) */
#define TIME_STAMP_LENGTH        32
/*
 * Time stamp is taken from 32 LSBs in layer 1 (after ASE)
 */
#define TIME_STAMP_OFFSET        -32
/*
 * PPH
 */
/**
 * TTL in PPH [76:69] (8b)
 * NWK_QOS in PPH [68:61] (8b)
 * IN_LIF in PPH [60:39] (22b)
 * FWD_DOMAIN for PPH [38:21] (18b)
 * IN_LIF_PROFILE for PPH [9:6] (4b)
 * END_OF_PKT_EDIT for PPH [12:10] (3b)
 * FWD_ADDITIONAL for PPH [9:6] (4b)
 * total fields[76:6] (71b)
 */
#define PPH_FIELDS_LENGTH       71
#define PPH_FIELDS_OFFSET       6
/** PARSING_OFFSET for PPH [89:83] (7b) */
#define PARSING_OFFSET_LENGTH   7
#define PARSING_OFFSET_OFFSET   83
/** PARSING_TYPE for PPH [94:90] (5b) */
#define PARSING_TYPE_LENGTH     5
#define PARSING_TYPE_OFFSET     90
/** FHEI_SIZE in PPH [78:79] (2b) */
#define FHEI_SIZE_LENGTH        2
#define FHEI_SIZE_OFFSET        78
/** LIF_EXT_TYPE in PPH [80:82] (3b) */
#define LIF_EXT_TYPE_LENGTH     3
#define LIF_EXT_TYPE_OFFSET     80
/** PHH_FWD_STR in PPH [95:95] (1b) */
#define PPH_FWD_STR_LENGTH      1
#define PPH_FWD_STR_OFFSET      95
/*
 * FHEI Trap
 */
/** TRAP_QUALIFIER in FHEI-Trap5B [8:26] */
#define TRAP_QUALIFIER_LENGTH   19
#define TRAP_QUALIFIER_OFFSET   (HEADER_PPH_SIZE + 8)
/** TRAP_CODE in FHEI-Trap5B [27:34] */
#define TRAP_CODE_LENGTH        9
#define TRAP_CODE_OFFSET        (HEADER_PPH_SIZE + 27)
/** FHEI_TRAP_Type in FHEI-Trap5B [36:39] (4b) */
#define FHEI_TRAP_TYPE_LENGTH   4
#define FHEI_TRAP_TYPE_OFFSET   (HEADER_PPH_SIZE + 36)
/*
 * FHEI Vlan
 */
/**
 * IN_VLAN_PRI in FHEI-Bridge5B [2:0] (3b)
 * IN_VLAN_CFI in FHEI-Bridge5B [3:3] (1b)
 * IN_VLAN_ID in FHEI-Bridge5B [15:4] (12b)
 */
#define INNER_VLAN_LENGTH      16
#define INNER_VLAN_OFFSET      (HEADER_PPH_SIZE + 0)
/**
 * VLAN_ACTIO in FHEI-Bridge5B [22:16] (7b)
 * OUT_VLAN_PRI in FHEI-Bridge5B [25:23] (3b)
 * OUT_VLAN_CFI in FHEI-Bridge5B [26:26] (1b)
 * OUT_VLAN_ID in FHEI-Bridge5B [38:27] (12b)
 */
#define OUTER_VLAN_LENGTH       23
#define OUTER_VLAN_OFFSET       (HEADER_PPH_SIZE + 16)
/** FHEI_VLAN_Type in FHEI-Bridge5B [39] (1b) */
#define FHEI_VLAN_TYPE_LENGTH   1
#define FHEI_VLAN_TYPE_OFFSET   (HEADER_PPH_SIZE + 39)
/*
 * User defined actions
 */
#define SYS_HDR_PROFILE_LENGTH      4
#define FWD_LAYER_INDEX_LENGTH      3
#define TRAP_STR_LENGTH             4
#define IPMF3_ZERO_PADDING_LENGTH   18
#define EEI_LENGTH                  24
#define TRAP_CODE_AND_STR_LENGTH    (TRAP_CODE_LENGTH + TRAP_STR_LENGTH)
/** Priorities used for forwarding layer index and system header profile */
#define PRIORITY_ITMH               BCM_FIELD_ACTION_PRIORITY(0, 2)
#define PRIORITY_1588               BCM_FIELD_ACTION_PRIORITY(0, 9)
/** Priorities used for Trap action */
#define TRAP_PRIORITY_PPH           BCM_FIELD_ACTION_PRIORITY(0, 10)
#define TRAP_PRIORITY_FHEI_TRAP     BCM_FIELD_ACTION_PRIORITY(0, 20)
/*
 * }
 */

/* *INDENT-ON* */

/*
 * Global and Static
 */
/*
 * {
 */
/**
 * \brief
 *  This function creates user defined qualifier.
 *
 * \param [in] unit     - The unit number.
 * \param [in] name     - Qualifier name.
 * \param [in] size     - Qualifier size.
 * \param [out] qual_id - Qualifier ID.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_user_qual_create(
    int unit,
    char *name,
    int size,
    bcm_field_qualify_t * qual_id)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), name, sizeof(qual_info.name));
    qual_info.size = size;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, qual_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates user defined action.
 *
 * \param [in] unit         - The unit number.
 * \param [in] name         - Action name.
 * \param [in] type         - Action type.
 * \param [in] stage        - stage.
 * \param [in] size         - Action size.
 * \param [in] prefix_value - Prefix value.
 * \param [out] action_id   - Action ID.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_user_action_create(
    int unit,
    char *name,
    bcm_field_action_t type,
    bcm_field_stage_t stage,
    int size,
    int prefix_value,
    bcm_field_action_t * action_id)
{
    bcm_field_action_info_t action_info, action_info_get;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = type;
    sal_strncpy_s((char *) action_info.name, name, sizeof(action_info.name));
    action_info.size = size;
    action_info.stage = stage;
    action_info.prefix_value = prefix_value;

    bcm_field_action_info_t_init(&action_info_get);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, type, stage, &action_info_get));
    action_info.prefix_size = action_info_get.size - size;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, action_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates ITMH field group, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t tc_snoop_dst_dp_mirror_qual, mirror_code_qual, itmh_ext_exist_qual, out_lif_dst_qual;
    bcm_field_action_t snoop_data_action, destination_action, mirror_code_action;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers and actions
     */
    /** Traffic class, snoop profile, destination, drop precedence, mirror disable valid bit [38:7] (32b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "tc_snoop_dst_dp_mirror", ITMH_FIELDS_LENGTH, &tc_snoop_dst_dp_mirror_qual));
    /** Mirror code disable (in case mirror disable valid bit in ITMH is on) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "mirror_code_default", MIRROR_CODE_LENGTH, &mirror_code_qual));
    /** ITMH base extension present [39:39] (1b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "itmh_ext_exist", EXT_PRESENT_LENGTH, &itmh_ext_exist_qual));
    /** ITMH base extension outlif [23:2] (22b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "out_lif_dst", OUTLIF_LENGTH, &out_lif_dst_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "snoop", bcmFieldActionSnoopRaw, bcmFieldStageIngressPMF2, SNOOP_DATA_LENGTH, 0x0007F,
                     &snoop_data_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "itmh_destination", bcmFieldActionForward, bcmFieldStageIngressPMF2, ITMH_DST_LENGTH, 0,
                     &destination_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "mirror_code_disable", bcmFieldActionMirrorIngressRaw, bcmFieldStageIngressPMF2,
                     MIRROR_CODE_LENGTH, 0, &mirror_code_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = tc_snoop_dst_dp_mirror_qual;
    fg_info.qual_types[1] = mirror_code_qual;
    fg_info.qual_types[2] = itmh_ext_exist_qual;
    fg_info.qual_types[3] = out_lif_dst_qual;

    fg_info.nof_actions = 6;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = snoop_data_action;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = destination_action;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionDropPrecedenceRaw;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = mirror_code_action;
    fg_info.action_with_valid_bit[4] = TRUE;
    fg_info.action_types[5] = bcmFieldActionOutVport0Raw;
    fg_info.action_with_valid_bit[5] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = ITMH_FIELDS_OFFSET;

    /** Mirror code disable (0x1e0) in case of ITMH.Inbound-Mirror-Disable=1*/
    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[1].input_arg = 0x1e0;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[2].offset = EXT_PRESENT_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[3].offset = OUTLIF_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates ITMH field group (not relevant for PPH),
 *  with all relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_only_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t ase_inj_qual, fwd_layer_index_qual, fbr_header_qual;
    bcm_field_action_t trap_str_zero_padding_action, udh3_ase_action;
    bcm_field_group_info_t fg_info;
    int mapping_bits_index, conditions_id;
    bcm_field_fem_action_info_t fem_action_info;
    /*
     * Forced value for FEM is 0x001E09 => Fwd_Action_Strength: 0x0f (max), Fwd_Action_CPU_Trap_Code: 0x9 (none)
     */
    int forced_values[13] = { 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1 };
    bcm_field_action_priority_t action_priority;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers and actions
     */
    /** Indicates ASE presence */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "is_ase_inj", ASE_INJ_LENGTH, &ase_inj_qual));
    /** Set fwd_layer_index to ITMH */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "forwarding_layer_index", FWD_LAYER_INDEX_LENGTH, &fwd_layer_index_qual));
    /**
     * Set System_header_profile to FTMH.
     * In case of unsupported PPH extension, ITMH_PPH context will direct to ITMH context in iPMF2
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "sys_header_profile", SYS_HDR_PROFILE_LENGTH, &fbr_header_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "trap_str_void", bcmFieldActionVoid, bcmFieldStageIngressPMF2, TRAP_STR_LENGTH, 0,
                     &trap_str_zero_padding_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "udh3_1bit_ase", bcmFieldActionUDHData3, bcmFieldStageIngressPMF2, ASE_INJ_LENGTH, 0,
                     &udh3_ase_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-only", sizeof(fg_info.name));

    fg_info.nof_quals = 5;
    /** Qualify upon trap strength for FEM action */
    fg_info.qual_types[0] = bcmFieldQualifyRxTrapStrength;
    /** PPH type indicates PPH and TSH presence */
    fg_info.qual_types[1] = bcmFieldQualifyItmhPphType;
    fg_info.qual_types[2] = ase_inj_qual;
    fg_info.qual_types[3] = fwd_layer_index_qual;
    fg_info.qual_types[4] = fbr_header_qual;

    fg_info.nof_actions = 5;
    fg_info.action_types[0] = trap_str_zero_padding_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionPphPresentSet;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = udh3_ase_action;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[4] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** Offset is 0, since trap qualifier is the first qualifier on the field group */
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    /*
     * For each trap strength between 0 and 15 (not included) set trap code None (0x9) with max strength (0xF)
     * We want to skip ITMH trap code override in case of Trap strength is 0xf, for example when packet is force forwarded
     */
    for (conditions_id = 0; conditions_id < 15; conditions_id++)
    {
        fem_action_info.fem_condition[conditions_id].extraction_id = 0;
        fem_action_info.fem_condition[conditions_id].is_action_valid = 1;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionTrapRaw;
    fem_action_info.fem_extraction[0].increment = 0;
    /** Action value is 0x001E09 => Fwd_Action_Strength: 0x0f (max), Fwd_Action_CPU_Trap_Code: 0x9 (None) */
    for (mapping_bits_index = 0; mapping_bits_index < 13; mapping_bits_index++)
    {
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].offset = 0;
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].forced_value =
            forced_values[mapping_bits_index];
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeForce;
    }

    action_priority = BCM_FIELD_ACTION_DONT_CARE;
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, *fg_id, action_priority, &fem_action_info));

    bcm_field_group_attach_info_t_init(attach_info);
    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ITMH_LAYER;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[2].offset = ASE_INJ_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[3].input_arg = 0x0;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[4].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    /** 1588 context also sets Forwarding layer index and system header profile */
    attach_info->payload_info.action_info[3].priority = PRIORITY_ITMH;
    attach_info->payload_info.action_info[4].priority = PRIORITY_ITMH;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates PPH field group, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_pph_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t pars_offset_valid_qual, pars_offset_qual, pars_type_valid_qual, pars_type_qual;
    bcm_field_qualify_t ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual, fwd_trap_code_qual, itmh_fwd_str_qual,
        pph_fwd_str_qual, is_tsh_inj_qual, hdr_profile_qual;
    bcm_field_action_t container_action, trap_action;
    int trap_code_none;
    bcm_field_name_to_id_info_t name_to_id_info;
    int nof_ids;
    uint32 id_from_name;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers and actions
     */
    /** parsing start type and offset will be cascading to iPMF3 using action container */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_offset_valid", VALID_BIT_LENGTH, &pars_offset_valid_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_offset", PARSING_OFFSET_LENGTH, &pars_offset_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pars_type_valid", VALID_BIT_LENGTH, &pars_type_valid_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "pars_type", PARSING_TYPE_LENGTH, &pars_type_qual));
    /** Ttl, nwk qos, inlif, forward domain, inlif profile, tail editing, forward layer additional info [76:6] (71b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "ttl_qos_inlif_fwddmn_inlifprfl_tail_fai", PPH_FIELDS_LENGTH,
                     &ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual));
    /** In case of no FHEI-Trap, set Trap code NONE */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fwd_trap_code_none", TRAP_CODE_LENGTH, &fwd_trap_code_qual));
    /** 3 LSB of forward action strength from ITMH [2:0] */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "itmh_fwd_str", ITMH_FWD_STR_LENGTH, &itmh_fwd_str_qual));
    /** MSB of forward action strength from PPH [95:95] */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "pph_fwd_str", PPH_FWD_STR_LENGTH, &pph_fwd_str_qual));
    /**If TSH was injected, set system header profile to ftmh_pph_tsh */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    sal_strncpy_s((char *) name_to_id_info.name, ITMH_IS_TSH_INJ, sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    is_tsh_inj_qual = id_from_name;
    sal_strncpy_s((char *) name_to_id_info.name, "sys_header_profile", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    hdr_profile_qual = id_from_name;
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "parsing_container", bcmFieldActionContainer, bcmFieldStageIngressPMF2,
                     VALID_BIT_LENGTH + PARSING_OFFSET_LENGTH + VALID_BIT_LENGTH + PARSING_TYPE_LENGTH, 0,
                     &container_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "trap_code_and_str", bcmFieldActionTrapRaw, bcmFieldStageIngressPMF2,
                     TRAP_CODE_AND_STR_LENGTH, 0, &trap_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH", sizeof(fg_info.name));

    fg_info.nof_quals = 10;
    fg_info.qual_types[0] = pars_offset_valid_qual;
    fg_info.qual_types[1] = pars_offset_qual;
    fg_info.qual_types[2] = pars_type_valid_qual;
    fg_info.qual_types[3] = pars_type_qual;
    fg_info.qual_types[4] = ttl_qos_inlif_fwddmn_inlifprfl_tail_fai_qual;
    fg_info.qual_types[5] = fwd_trap_code_qual;
    fg_info.qual_types[6] = itmh_fwd_str_qual;
    fg_info.qual_types[7] = pph_fwd_str_qual;
    fg_info.qual_types[8] = is_tsh_inj_qual;
    fg_info.qual_types[9] = hdr_profile_qual;

    fg_info.nof_actions = 10;
    fg_info.action_types[0] = container_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionTtlSet;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionNetworkQos;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionInVport0Raw;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionVrfSetRaw;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionInVportClass0;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionIPTProfile;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionForwardingAdditionalInfo;
    fg_info.action_with_valid_bit[7] = FALSE;
    fg_info.action_types[8] = trap_action;
    fg_info.action_with_valid_bit[8] = FALSE;
    fg_info.action_types[9] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[9] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[0].input_arg = 0x1;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = PARSING_OFFSET_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[2].input_arg = 0x1;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[3].offset = PARSING_TYPE_OFFSET;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[4].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[4].offset = PPH_FIELDS_OFFSET;

    /** Get default trap code */
    SHR_IF_ERR_EXIT(bcm_rx_trap_type_get(unit, 0, bcmRxTrapDefault, &trap_code_none));
    attach_info->key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info->key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[5].input_arg = trap_code_none;

    attach_info->key_info.qual_types[6] = fg_info.qual_types[6];
    attach_info->key_info.qual_info[6].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[6].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[6].offset = ITMH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[7] = fg_info.qual_types[7];
    attach_info->key_info.qual_info[7].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[7].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[7].offset = PPH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[8] = fg_info.qual_types[8];
    attach_info->key_info.qual_info[8].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[8].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[8].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[9] = fg_info.qual_types[9];
    attach_info->key_info.qual_info[9].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[9].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH_PPH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];
    attach_info->payload_info.action_types[6] = fg_info.action_types[6];
    attach_info->payload_info.action_types[7] = fg_info.action_types[7];
    attach_info->payload_info.action_types[8] = fg_info.action_types[8];
    attach_info->payload_info.action_info[8].priority = TRAP_PRIORITY_PPH;
    attach_info->payload_info.action_types[9] = fg_info.action_types[9];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates ASE_OAM field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ase_oam_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t oam_stamp_offset_qual, oam_offset_qual, oam_sub_type_qual, oam_mep_qual;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers
     */
    /** Offset in ASE-OAM [8:1] (8b) mapped to OAM_Offset and OAM_Stamp_Offset */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_stamp_offset", OAM_OFFSET_LENGTH, &oam_stamp_offset_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_offset", OAM_OFFSET_LENGTH, &oam_offset_qual));
    /** Sub_type != 0 indicates ASE is type of OAM */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "oam_header_sub_type", OAM_SUB_TYPE_LENGTH, &oam_sub_type_qual));
    /** MEP_Type in ASE-OAM [43] (1b) mapped to OAM_Up_MEP */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "oam_header_mep", OAM_MEP_LENGTH, &oam_mep_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-ASE_OAM", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = oam_stamp_offset_qual;
    fg_info.qual_types[1] = oam_offset_qual;
    fg_info.qual_types[2] = oam_sub_type_qual;
    fg_info.qual_types[3] = oam_mep_qual;

    fg_info.nof_actions = 1;
    /*
     * Changes 4 OAM signals (OAM-Stamp-Offset, OAM-offset, OAM-Sub-Type, OAM-Up-Mep) 
     */
    fg_info.action_types[0] = bcmFieldActionOamRaw;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[0].offset = OAM_OFFSET_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[1].offset = OAM_OFFSET_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[2].offset = OAM_SUB_TYPE_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[3].offset = OAM_MEP_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates ASE_1588 field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ase_1588_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t ase_1588_offset_qual, time_stamp_encapsulation_qual, time_stamp_command_qual,
        use_ingress_time_compensation_qual;
    bcm_field_qualify_t ase_1588_type_qual, fwd_layer_index_qual, is_tsh_inj_qual, hdr_profile_qual;
    bcm_field_action_t void_1588_action;
    bcm_field_name_to_id_info_t name_to_id_info;
    bcm_field_fem_action_info_t fem_action_info;
    bcm_field_action_info_t action_info;
    bcm_field_action_priority_t action_priority;
    int mapping_bits_index, conditions_id;
    int nof_ids;
    int ieee_1588_action_size;
    uint32 id_from_name;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers
     */
    /** Offset in bytes measured from end of system_headers to the start of the 1588 frame [36:43] (8b) mapped to IEEE1588_Header_Offset */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "1588_offset", ASE_1588_OFFSET_LENGTH, &ase_1588_offset_qual));
    /** 1588v2 Encapsulation in ASE-1588v2 Header [32:32] (1b) mapped to IEEE1588_Enc */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_encapsulation", TS_ENCAPSULATION_LENGTH, &time_stamp_encapsulation_qual));
    /** 1588v2 Command in ASE-1588v2 Header [33:35] (3b) mapped to IEEE1588_Cmd */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_command", TS_COMMAND_LENGTH, &time_stamp_command_qual));
    /** Use Ingress time-compensation in the calculation of the 1588 correction [30:30] (1b) mapped to IEEE1588_Compensate_Time_Stamp */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "use_ingress_time_compensation", USE_INGRESS_TIME_COMPENSATION_LENGTH,
                     &use_ingress_time_compensation_qual));
    /** const of 1 bit mapped to Pkt_is_IEEE1588 */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "1588_ase_type", 1, &ase_1588_type_qual));
    /** Set fwd_layer_index to ITMH_TSH */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    sal_strncpy_s((char *) name_to_id_info.name, "forwarding_layer_index", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    fwd_layer_index_qual = id_from_name;
    /** If TSH was injected, set system header profile to FTMH_TSH */
    sal_strncpy_s((char *) name_to_id_info.name, ITMH_IS_TSH_INJ, sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    is_tsh_inj_qual = id_from_name;
    sal_strncpy_s((char *) name_to_id_info.name, "sys_header_profile", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    hdr_profile_qual = id_from_name;
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "void_1588", bcmFieldActionVoid, bcmFieldStageIngressPMF2, 13, 0, &void_1588_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-ASE_1588", sizeof(fg_info.name));

    fg_info.nof_quals = 8;
    fg_info.qual_types[0] = ase_1588_offset_qual;
    fg_info.qual_types[1] = time_stamp_encapsulation_qual;
    fg_info.qual_types[2] = time_stamp_command_qual;
    fg_info.qual_types[3] = use_ingress_time_compensation_qual;
    fg_info.qual_types[4] = ase_1588_type_qual;
    fg_info.qual_types[5] = fwd_layer_index_qual;
    fg_info.qual_types[6] = is_tsh_inj_qual;
    fg_info.qual_types[7] = hdr_profile_qual;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = void_1588_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionFabricHeaderSetRaw;
    fg_info.action_with_valid_bit[2] = TRUE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    /*
     * Adding FEM for 1588 Header fields, 1588_header_offset = 1588_offset + bytes_to_remove, so we need to use increment
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** Offset is 0, since 1588 Data are the first qualifiers on this field group */
    fem_action_info.fem_input.input_offset = 0;
    /** Condition is based on 1 bit of ITMH_IS_TSH_INJ. We'll put it on the MSB of the condition */
    fem_action_info.condition_msb = 16;
    /*
     * Since ITMH_IS_TSH_INJ is on the MSB of the condition, we'll configure the first action for values represented by 0b0XXX (0-7)
     * For each condition between 0 and 8 (not included) increment offset by 13 bytes.
     */
    for (conditions_id = 0; conditions_id < 8; conditions_id++)
    {
        fem_action_info.fem_condition[conditions_id].extraction_id = 0;
        fem_action_info.fem_condition[conditions_id].is_action_valid = 1;
    }
    /*
     * Since ITMH_IS_TSH_INJ is on the MSB of the condition, we'll configure the second action for values represented by 0b1XXX (8-15)
     * For each condition between 8 and 16 (not included) increment offset by 17 bytes.
     */
    for (conditions_id = 8; conditions_id < 16; conditions_id++)
    {
        fem_action_info.fem_condition[conditions_id].extraction_id = 1;
        fem_action_info.fem_condition[conditions_id].is_action_valid = 1;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionIEEE1588;
    fem_action_info.fem_extraction[0].increment = 13;   /* 1588 (6B) + ITMH (5B) + PTCH (2B) */
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionIEEE1588;
    fem_action_info.fem_extraction[1].increment = 17;   /* TSH(4B) + 1588 (6B) + ITMH (5B) + PTCH (2B) */

    bcm_field_action_info_t_init(&action_info);
    SHR_IF_ERR_EXIT(bcm_field_action_info_get(unit, bcmFieldActionIEEE1588, bcmFieldStageIngressPMF2, &action_info));
    ieee_1588_action_size = action_info.size;
    for (mapping_bits_index = 0; mapping_bits_index < ieee_1588_action_size; mapping_bits_index++)
    {
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].offset = mapping_bits_index;
        fem_action_info.fem_extraction[0].output_bit[mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[1].output_bit[mapping_bits_index].offset = mapping_bits_index;
        fem_action_info.fem_extraction[1].output_bit[mapping_bits_index].source_type =
            bcmFieldFemExtractionOutputSourceTypeKey;
    }

    action_priority = BCM_FIELD_ACTION_POSITION(3, 7);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, *fg_id, action_priority, &fem_action_info));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[0].offset = ASE_1588_OFFSET_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[1].offset = TS_ENCAPSULATION_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[2].offset = TS_COMMAND_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_ASE_TSH_LAYER;
    attach_info->key_info.qual_info[3].offset = USE_INGRESS_TIME_COMPENSATION_OFFSET;

    attach_info->key_info.qual_types[4] = fg_info.qual_types[4];
    attach_info->key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[4].input_arg = 0x1;

    attach_info->key_info.qual_types[5] = fg_info.qual_types[5];
    attach_info->key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[5].input_arg = 0x1;

    attach_info->key_info.qual_types[6] = fg_info.qual_types[6];
    attach_info->key_info.qual_info[6].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[6].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[6].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[7] = fg_info.qual_types[7];
    attach_info->key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[7].input_arg = DNX_FIELD_CONTEXT_SYS_HDR_PROFILE_FTMH_TSH;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_info[1].priority = PRIORITY_1588;
    attach_info->payload_info.action_info[2].priority = PRIORITY_1588;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates TSH field group, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_tsh_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t time_stamp_qual, time_stamp_valid_qual;
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers
     */
    /** If TSH was injected, copy it's value */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, ITMH_IS_TSH_INJ, IS_TSH_INJ_LENGTH, &time_stamp_valid_qual));
    /** Time stamp [31:0] (32b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "time_stamp_header", TIME_STAMP_LENGTH, &time_stamp_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-TSH", sizeof(fg_info.name));

    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = time_stamp_valid_qual;
    fg_info.qual_types[1] = time_stamp_qual;

    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionIngressTimeStampInsertValid;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionIngressTimeStampInsert;
    fg_info.action_with_valid_bit[1] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);
    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = TSH_INJ_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = TIME_STAMP_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates FLOW ID field group, with all
 *  relevant information to it. This field group only
 *  relevant for devices with SOC flow_id.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_flow_id_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t is_tsh_inj_qual;
    bcm_field_group_info_t fg_info;
    bcm_field_action_t flow_id_valid_action;
    bcm_field_name_to_id_info_t name_to_id_info;
    int nof_ids;
    uint32 id_from_name;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers and actions
     */
    /** Indicates TSH presence */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    sal_strncpy_s((char *) name_to_id_info.name, ITMH_IS_TSH_INJ, sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    is_tsh_inj_qual = id_from_name;
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "flow_id_valid", bcmFieldActionLatencyFlowId, bcmFieldStageIngressPMF2, IS_TSH_INJ_LENGTH, 0,
                     &flow_id_valid_action));

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FLOW_ID", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = is_tsh_inj_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = flow_id_valid_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[0].offset = TSH_INJ_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates FHEI-Trap field group, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_fhei_trap_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_qualify_t fwd_trap_code_qual, itmh_fwd_str_qual, pph_fwd_str_qual, trap_qualifier_qual;
    bcm_field_name_to_id_info_t name_to_id_info;
    int nof_ids;
    uint32 id_from_name;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers
     */
    /** FHEI trap code [35:27] (9b) */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    sal_strncpy_s((char *) name_to_id_info.name, "fwd_trap_code_none", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    fwd_trap_code_qual = id_from_name;
    /** 3 LSB of forward action strength from ITMH [2:0] */
    sal_strncpy_s((char *) name_to_id_info.name, "itmh_fwd_str", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    itmh_fwd_str_qual = id_from_name;
    /** MSB of forward action strength from PPH [95:95] */
    sal_strncpy_s((char *) name_to_id_info.name, "pph_fwd_str", sizeof(name_to_id_info.name));
    SHR_IF_ERR_EXIT(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_from_name));
    pph_fwd_str_qual = id_from_name;
    /** MSB of forward action strength from PPH [95:95] */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "cpu_trap_qualifier", TRAP_QUALIFIER_LENGTH, &trap_qualifier_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FHEI_Trap", sizeof(fg_info.name));

    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = fwd_trap_code_qual;
    fg_info.qual_types[1] = itmh_fwd_str_qual;
    fg_info.qual_types[2] = pph_fwd_str_qual;
    fg_info.qual_types[3] = trap_qualifier_qual;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionTrapRaw;
    fg_info.action_with_valid_bit[0] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[0].offset = TRAP_CODE_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_ITMH_LAYER;
    attach_info->key_info.qual_info[1].offset = ITMH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[2].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[2].offset = PPH_FWD_STR_OFFSET;

    attach_info->key_info.qual_types[3] = fg_info.qual_types[3];
    attach_info->key_info.qual_info[3].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[3].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[3].offset = TRAP_QUALIFIER_OFFSET;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_info[0].priority = TRAP_PRIORITY_FHEI_TRAP;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates FHEI-Vlan field group, with all
 *  relevant information to it.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_fhei_vlan_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_qualify_t inner_vlan_qual, outer_vlan_qual, eei_qual;
    bcm_field_group_info_t fg_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined qualifiers
     */
    /** FHEI in vlan qualifier [15:0] (16b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "inner_vlan", INNER_VLAN_LENGTH, &inner_vlan_qual));
    /** FHEI out vlan qualifier [38:16] (23b) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "outer_vlan", OUTER_VLAN_LENGTH, &outer_vlan_qual));
    /** EEI = {4'b1111,20'b0} (EEI identifier (4 msbs) of 15 is not used in JR2 system headers mode) */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create(unit, "modify_eei", EEI_LENGTH, &eei_qual));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) fg_info.name, "ITMH-FHEI_Vlan", sizeof(fg_info.name));

    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = inner_vlan_qual;
    fg_info.qual_types[1] = outer_vlan_qual;
    fg_info.qual_types[2] = eei_qual;

    fg_info.nof_actions = 6;
    fg_info.action_types[0] = bcmFieldActionInnerVlanNew;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionInnerVlanPrioNew;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionOuterVlanNew;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionOuterVlanPrioNew;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionVlanActionSetNew;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionSystemHeaderSet;
    fg_info.action_with_valid_bit[5] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[0].offset = INNER_VLAN_OFFSET;

    attach_info->key_info.qual_types[1] = fg_info.qual_types[1];
    attach_info->key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info->key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    attach_info->key_info.qual_info[1].offset = OUTER_VLAN_OFFSET;

    attach_info->key_info.qual_types[2] = fg_info.qual_types[2];
    attach_info->key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info->key_info.qual_info[2].input_arg = 0xF00000;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];
    attach_info->payload_info.action_types[3] = fg_info.action_types[3];
    attach_info->payload_info.action_types[4] = fg_info.action_types[4];
    attach_info->payload_info.action_types[5] = fg_info.action_types[5];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates field group in iPMF3, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ipmf3_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_t pars_offset_action, zero_padding_action;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined actions
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "parse_start_offset", bcmFieldActionParsingStartOffsetRaw, bcmFieldStageIngressPMF3,
                     PARSING_OFFSET_LENGTH + VALID_BIT_LENGTH, 0, &pars_offset_action));
    /** Container qualifier is 32b while the info needed is only 14b, so need to add zero padding on MSB */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "void_zero_padding", bcmFieldActionVoid, bcmFieldStageIngressPMF3, IPMF3_ZERO_PADDING_LENGTH,
                     0, &zero_padding_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH-IPMF3", sizeof(fg_info.name));

    fg_info.nof_quals = 1;
    /** Container qualifier to update parsing start offset and parsing start type */
    fg_info.qual_types[0] = bcmFieldQualifyContainer;

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = pars_offset_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionParsingStartTypeRaw;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = zero_padding_action;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->key_info.nof_quals = fg_info.nof_quals;
    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info->key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates const field group in iPMF3, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit         - The unit number.
 * \param [out] fg_id       - Field group ID created.
 * \param [out] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_ipmf3_const_group_config(
    int unit,
    bcm_field_group_t * fg_id,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_group_info_t fg_info;
    bcm_field_action_t sys_hdr_size_adjust_action, fwd_layer_index_action, egr_fwd_layer_action;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Create user defined actions
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "sys_hdr_size_adjust", bcmFieldActionSystemHeaderSizeAdjust, bcmFieldStageIngressPMF3, 0,
                     0x1, &sys_hdr_size_adjust_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "forwarding_layer_indexx", bcmFieldActionForwardingLayerIndex, bcmFieldStageIngressPMF3, 0,
                     0x3, &fwd_layer_index_action));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_create
                    (unit, "egr_fwd_layer", bcmFieldActionEgressForwardingIndex, bcmFieldStageIngressPMF3, 0, 0x3,
                     &egr_fwd_layer_action));

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeConst;
    fg_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) fg_info.name, "ITMH-PPH_Const-IPMF3", sizeof(fg_info.name));

    fg_info.nof_actions = 3;
    fg_info.action_types[0] = sys_hdr_size_adjust_action;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = fwd_layer_index_action;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = egr_fwd_layer_action;
    fg_info.action_with_valid_bit[2] = FALSE;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, fg_id));

    bcm_field_group_attach_info_t_init(attach_info);

    attach_info->payload_info.nof_actions = fg_info.nof_actions;

    attach_info->payload_info.action_types[0] = fg_info.action_types[0];
    attach_info->payload_info.action_types[1] = fg_info.action_types[1];
    attach_info->payload_info.action_types[2] = fg_info.action_types[2];

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is configuring all needed compare keys.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_cmp_key_config(
    int unit)
{
    bcm_field_qualify_t lif_extension_qual, fhei_trap_type_qual, fhei_vlan_type_qual;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;
    bcm_field_context_t context_id;
    int pair_id;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the offset of KeyGenVar for compare qualifiers. */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));

    /** Create user defined qualifiers */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "lif_extension", LIF_EXT_TYPE_LENGTH, &lif_extension_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fhei_trap_type", FHEI_TRAP_TYPE_LENGTH, &fhei_trap_type_qual));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_create
                    (unit, "fhei_vlan_type", FHEI_VLAN_TYPE_LENGTH, &fhei_vlan_type_qual));

    /**********************
     *  ITMH PPH context  *
     **********************/
    bcm_field_context_compare_info_t_init(&compare_info);
    /*
     * If packet was injected with LIF extension over ITMH_PPH, it will hit ITMH context, and only ITMH will be removed
     */
    compare_info.first_key_info.nof_quals = 1;
    compare_info.first_key_info.qual_types[0] = lif_extension_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = LIF_EXT_TYPE_OFFSET;

    compare_info.second_key_info.nof_quals = 1;
    /*
     * Key gen var includes const value of 0 (0b000) (indicates LIF type is none)
     */
    compare_info.second_key_info.qual_types[0] = lif_extension_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset;

    /** Create the context compare mode on pair ID 0. */
    pair_id = 0;
    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

    /***************************
     *  ITMH PPH FHEI context  *
     ***************************/
    bcm_field_context_compare_info_t_init(&compare_info);
    /*
     * If packet was injected with LIF extension over ITMH_PPH_FHEI-5B, or with unsupported FHEI-5B extension
     * (different from FHEI-Trap or FHEI-Vlan), it will hit ITMH context, and only ITMH will be removed
     */
    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = fhei_trap_type_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = FHEI_TRAP_TYPE_OFFSET;

    compare_info.first_key_info.qual_types[1] = lif_extension_qual;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[1].offset = LIF_EXT_TYPE_OFFSET;

    /*
     * Key gen var indicates LIF type is none and FHEI type is TRAP 
     */
    compare_info.second_key_info.nof_quals = 2;
    compare_info.second_key_info.qual_types[0] = fhei_trap_type_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset;

    compare_info.second_key_info.qual_types[1] = lif_extension_qual;
    compare_info.second_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[1].offset =
        qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH + FHEI_VLAN_TYPE_LENGTH;

    /** Create the context compare mode on pair ID 0 */
    pair_id = 0;
    context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

    bcm_field_context_compare_info_t_init(&compare_info);

    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = fhei_vlan_type_qual;
    compare_info.first_key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[0].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[0].offset = FHEI_VLAN_TYPE_OFFSET;

    compare_info.first_key_info.qual_types[1] = lif_extension_qual;
    compare_info.first_key_info.qual_info[1].input_type = bcmFieldInputTypeLayerAbsolute;
    compare_info.first_key_info.qual_info[1].input_arg = ITMH_PPH_APPL_PPH_LAYER;
    compare_info.first_key_info.qual_info[1].offset = LIF_EXT_TYPE_OFFSET;

    /*
     * Key gen var indicates LIF type is none and FHEI type is TRAP 
     */
    compare_info.second_key_info.nof_quals = 2;
    compare_info.second_key_info.qual_types[0] = fhei_vlan_type_qual;
    compare_info.second_key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH;

    compare_info.second_key_info.qual_types[1] = lif_extension_qual;
    compare_info.second_key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    compare_info.second_key_info.qual_info[1].offset =
        qual_info_get.offset + FHEI_TRAP_TYPE_LENGTH + FHEI_VLAN_TYPE_LENGTH;

    /** Create the context compare mode on pair ID 1 */
    pair_id = 1;
    context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create
                    (unit, 0, bcmFieldStageIngressPMF1, context_id, pair_id, &compare_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create contexts in IPMF1 and
 *  configure the relevant program for ITMH_PPH packets.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_ipmf1_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_context_id, itmh_pph_fhei_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);

    /*********************
     *  ITMH PPH context
     *******************/

    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_pph_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhPph;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_pph_context_id, &param_info));
    /**
     * Set the key Gen Var used for compare
     *   Bits usage:
     *      0-1 (0x00) --->key_gen_var_q_pph qualifier 3b (used for compare)
     *          - bits 0-1 ---> LIF_EXT_TYPE (3b) ---> Const value of 0 (0b000) (indicates LIF type is none)
     */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x0;
    param_info.param_arg = 0;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_pph_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 3;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;
    /** Check that bit 1 (PPH_TYPE) of the LayerQualifier are set to 1. */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0001;
    presel_entry_data.qual_data[1].qual_mask = 0x0001;
    /** Check that bits 5:6 (FHEI_SIZE) of the LayerQualifier are set to 0. */
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[2].qual_arg = 2;
    presel_entry_data.qual_data[2].qual_value = 0x00;
    presel_entry_data.qual_data[2].qual_mask = 0x30;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*********************
     *  ITMH PPH FHEI context
     *******************/
    itmh_pph_fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    bcm_field_context_info_t_init(&context_info);
    context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
    context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_IPMF1", sizeof(context_info.name));

    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_pph_fhei_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhPph;

    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF1, itmh_pph_fhei_context_id, &param_info));
    /**
     * Set the key Gen Var used for compare
     *   Bits usage:
     *      0-7 (0x5) ---> key_gen_var qualifier 7b (used for compare)
     *          - bits 0-3   ---> FHEI_TRAP_TYPE (4b) ---> Const value of 5 (0b0101) (indicates FHEI type is TRAP)
     *          - bit 4      ---> FHEI_VLAN_TYPE (1b) ---> Const value of 0 (0b0) (indicates FHEI type is VLAN)
     *          - bits 5-7   ---> LIF_EXT_TYPE   (3b) ---> Const value of 0 (0b000) (indicates LIF type is none)
     */
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x5;
    param_info.param_arg = 0;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF1, itmh_pph_fhei_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 3;
    presel_entry_data.context_id = itmh_pph_fhei_context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;
    /** Check that bit 1 (PPH_TYPE) of the LayerQualifier are set to 1. */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0001;
    presel_entry_data.qual_data[1].qual_mask = 0x0001;
    /** Check that bits 5:6 (FHEI_SIZE) of the LayerQualifier are set to 2. */
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[2].qual_arg = 2;
    presel_entry_data.qual_data[2].qual_value = 0x20;
    presel_entry_data.qual_data[2].qual_mask = 0x30;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_cmp_key_config(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create contexts in IPMF2 and
 *  configure the relevant program for ITMH_PPH packets.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_ipmf2_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_fhei_context_id, itmh_pph_vlan_context_id, itmh_pph_context_id,
        itmh_pph_lif_context_id, itmh_pph_fhei_lif_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);
    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    itmh_pph_fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    itmh_pph_vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);
    itmh_pph_lif_context_id = dnx_data_field.context.default_itmh_pph_lif_ipmf2_context_get(unit);
    itmh_pph_fhei_lif_context_id = dnx_data_field.context.default_itmh_pph_fhei_lif_ipmf2_context_get(unit);

    /*****************************
     *  ITMH PPH context
     ****************************/
    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, itmh_pph_context_id, &param_info));

    /*****************************
     *  ITMH PPH FHEI_Vlan context
     *****************************/
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_Vlan", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_fhei_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info, &itmh_pph_vlan_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_vlan_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_vlan_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_vlan_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult1;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x2;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*****************************
     *  ITMH PPH FHEI_Trap context
     ****************************/
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x2;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_fhei_context_id, &param_info));

    /********************************
     *  IPMF2 ITMH PPH LIF context  *
     *******************************/
    /*
     * This context is hit when an ITMH_PPH packet has LIF extension
     * In this case only ITMH is being rebuilt
     */
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_LIF", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info, &itmh_pph_lif_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_lif_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_lif_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    /*
     * This context is hit when LIF extension type is greater then (0x4) 0
     */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x4;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    /*************************************
     *  IPMF2 ITMH PPH FHEI LIF context  *
     *************************************/
    /*
     * This context is hit when an ITMH_PPH_FHEI packet has LIF extension
     * In this case only ITMH is being rebuilt
     */
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_FHEI_LIF", sizeof(context_info.name));
    context_info.cascaded_from = itmh_pph_fhei_context_id;
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF2, &context_info,
                     &itmh_pph_fhei_lif_context_id));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set
                    (unit, 0, bcmFieldStageIngressPMF2, itmh_pph_fhei_lif_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_fhei_lif_presel_id_ipmf2_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x1;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = 124;
    presel_entry_id.stage = bcmFieldStageIngressPMF2;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_pph_fhei_lif_context_id;
    presel_entry_data.entry_valid = TRUE;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyCompareKeysResult0;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = 0x4;
    presel_entry_data.qual_data[0].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create contexts in IPMF3 and
 *  configure the relevant program for ITMH_PPH packets.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_ipmf3_contexts(
    int unit)
{
    bcm_field_context_t itmh_pph_context_id, itmh_pph_fhei_trap_context_id, itmh_pph_fhei_vlan_context_id;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;

    SHR_FUNC_INIT_VARS(unit);
    itmh_pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    itmh_pph_fhei_trap_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    itmh_pph_fhei_vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);

    /*********************
     *  ITMH PPH context
     *******************/
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF3", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF3, &context_info, &itmh_pph_context_id));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_2nd_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_fhei_trap_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);

    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_vlan_ipmf3_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF3;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.context_id = itmh_pph_context_id;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = itmh_pph_fhei_vlan_context_id;
    presel_entry_data.qual_data[0].qual_mask = 0x3F;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create contexts in IPMF1 and
 *  configure the relevant program for ITMH packets,
 *  or ITMH_PPH packets with unsupported extensions.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_context(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_t itmh_context_id;

    SHR_FUNC_INIT_VARS(unit);

    itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);

    /************************
     *  IPMF1 ITMH context  *
     ************************/
    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_IPMF1", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &itmh_context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, itmh_context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 1;
    presel_entry_data.context_id = itmh_context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;

    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, itmh_context_id, &param_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create contexts in IPMF1 and
 *  configure the relevant program for ITMH_ASE-1588 packets.
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
field_itmh_1588_context(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_t context_id = dnx_data_field.context.default_itmh_1588_tsh_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_context_info_t_init(&context_info);
    sal_strncpy_s((char *) (context_info.name), "ITMH_1588", sizeof(context_info.name));
    SHR_IF_ERR_EXIT(bcm_field_context_create
                    (unit, BCM_FIELD_FLAG_WITH_ID, bcmFieldStageIngressPMF1, &context_info, &context_id));
    bcm_field_context_param_info_t_init(&param_info);
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
    param_info.param_val = bcmFieldSystemHeaderProfileFtmhTsh;
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF1, context_id, &param_info));

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_1588_tsh_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;

    /** Check that type ASE are set to 1588 (0x2). */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 1;
    presel_entry_data.qual_data[1].qual_value = 0x2;
    presel_entry_data.qual_data[1].qual_mask = 0x7;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

    bcm_field_context_param_info_t_init(&param_info);
    /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
    param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
    param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset1, 0);
    SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function attaches field group to its relevant ITMH contexts.
 *
 * \param [in] unit        - The unit number.
 * \param [in] fg_id       - Field group ID.
 * \param [in] context_ids - Flags of context IDs.
 * \param [in] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_group_context_attach(
    int unit,
    bcm_field_group_t fg_id,
    int context_ids,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_context_t itmh_context_id, itmh_1588_context_id;

    SHR_FUNC_INIT_VARS(unit);

    itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);
    itmh_1588_context_id = dnx_data_field.context.default_itmh_1588_tsh_context_get(unit);

    if (context_ids & FLAG_BIT_ITMH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_context_id, attach_info));
    }
    {
        if (context_ids & FLAG_BIT_ASE_1588_CONTEXT)
        {
            SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_1588_context_id, attach_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function attaches field group to its relevant PPH contexts.
 *
 * \param [in] unit        - The unit number.
 * \param [in] fg_id       - Field group ID.
 * \param [in] context_ids - Flags of context IDs.
 * \param [in] attach_info - Attach info.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_group_context_attach(
    int unit,
    bcm_field_group_t fg_id,
    int context_ids,
    bcm_field_group_attach_info_t * attach_info)
{
    bcm_field_context_t pph_context_id, vlan_context_id, fhei_context_id, itmh_pph_lif_context_id,
        itmh_pph_fhei_lif_context_id;

    SHR_FUNC_INIT_VARS(unit);

    pph_context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);
    fhei_context_id = dnx_data_field.context.default_itmh_pph_fhei_context_get(unit);
    vlan_context_id = dnx_data_field.context.default_itmh_pph_fhei_vlan_ipmf2_context_get(unit);
    itmh_pph_lif_context_id = dnx_data_field.context.default_itmh_pph_lif_ipmf2_context_get(unit);
    itmh_pph_fhei_lif_context_id = dnx_data_field.context.default_itmh_pph_fhei_lif_ipmf2_context_get(unit);

    if (context_ids & FLAG_BIT_PPH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, pph_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_PPH_FHEI_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, fhei_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_PPH_FHEI_VLAN_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, vlan_context_id, attach_info));
    }

    if (context_ids & FLAG_BIT_ITMH_CONTEXT)
    {
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_pph_lif_context_id, attach_info));
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, itmh_pph_fhei_lif_context_id, attach_info));
    }

    /*
     * Attach to ITMH contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach(unit, fg_id, context_ids, attach_info));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates all the contexts relevant for ITMH appl
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_context_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(field_itmh_context(unit));
    {
        SHR_IF_ERR_EXIT(field_itmh_1588_context(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates all the contexts relevant for ITMH_PPH appl
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_context_create(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf1_contexts(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf2_contexts(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_ipmf3_contexts(unit));

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_context_create(unit));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds all the field groups relevant for ITMH appl,
 *  and attached them to the relevant contexts
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_field_group_create(
    int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    int flow_id = dnx_data_field.init.flow_id_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * ITMH field group is relevant for all contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                     FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    /*
     * ITMH-only field group is relevant for ITMH and ASE_1588 contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_only_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id, FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT, &attach_info));

    /*
     * TSH and FLOW-ID field groups are relevant for all contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_tsh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT |
                     FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    if (flow_id)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_flow_id_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach
                        (unit, fg_id,
                         FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                         FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));
    }
    /*
     *  ASE-1588 field group are only relevant for ASE_1588 context
     */
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_ase_1588_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_context_attach(unit, fg_id, FLAG_BIT_ASE_1588_CONTEXT, &attach_info));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function adds all the field groups relevant for ITMH_PPH appl,
 *  and attached them to the relevant contexts
 *
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_field_group_create(
    int unit)
{
    bcm_field_group_t fg_id;
    bcm_field_group_attach_info_t attach_info;
    int flow_id = dnx_data_field.init.flow_id_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * ITMH-only field group is relevant for ITMH and ASE_1588 contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_only_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT, &attach_info));

    /*
     * ITMH field group is relevant for all contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                     FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    /*
     * TSH and FLOW-ID field groups are relevant for all contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_tsh_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id,
                     FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT |
                     FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    /*
     * PPH field group is relevant for all PPH and FHEI contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_pph_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT,
                     &attach_info));

    /*
     * FHEI-Trap field group is only relevant for FHEI-Trap context
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fhei_trap_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_FHEI_CONTEXT, &attach_info));

    /*
     * FHEI-Vlan field group is only relevant for FHEI-Vlan context
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fhei_vlan_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));

    if (flow_id)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_flow_id_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                        (unit, fg_id,
                         FLAG_BIT_ITMH_CONTEXT | FLAG_BIT_ASE_1588_CONTEXT | FLAG_BIT_PPH_CONTEXT |
                         FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT, &attach_info));
    }
    /*
     *  ASE-1588 field group are only relevant for ASE_1588 context
     */
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_ase_1588_group_config(unit, &fg_id, &attach_info));
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                        (unit, fg_id, FLAG_BIT_ASE_1588_CONTEXT, &attach_info));
    }

    /*
     * ASE-OAM field group is relevant for all PPH and FHEI contexts
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_ase_oam_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach
                    (unit, fg_id, FLAG_BIT_PPH_CONTEXT | FLAG_BIT_PPH_FHEI_CONTEXT | FLAG_BIT_PPH_FHEI_VLAN_CONTEXT,
                     &attach_info));

    /*
     * iPMF3 field groups are only relevant for iPMF3 PPH context
     */
    SHR_IF_ERR_EXIT(appl_dnx_field_ipmf3_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_CONTEXT, &attach_info));

    SHR_IF_ERR_EXIT(appl_dnx_field_ipmf3_const_group_config(unit, &fg_id, &attach_info));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_context_attach(unit, fg_id, FLAG_BIT_PPH_CONTEXT, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_itmh_pph_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_context_create(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_field_group_create(unit));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
appl_dnx_field_itmh_init(
    int unit)
{
    uint8 is_alloc;
    bcm_field_context_t itmh_context_id = dnx_data_field.context.default_itmh_context_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, itmh_context_id, &is_alloc));

    /*
     * In case ITMH_PPH appl is enabled, ITMH resources were already allocated
     */
    if (is_alloc)
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_context_create(unit));
    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_field_group_create(unit));

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
