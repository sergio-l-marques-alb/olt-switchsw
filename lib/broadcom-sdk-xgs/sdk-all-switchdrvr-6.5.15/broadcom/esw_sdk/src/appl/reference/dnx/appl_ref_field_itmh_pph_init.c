/** \file appl_ref_itmh_init.c
 * $Id$
 *
 * ITMH Programmable mode application procedures for DNX.
 *
 *
 *
 * ITMH header :
 *        ___________________________  ____  _______  _____  ____  ________________  _____  _________  __________
 * field [ ITMH_BASE_EXTENSION_EXIST ][ TC ][ SNOOP ][ DST ][ DP ][ MIRROR_DISABLE ][ PPH ][ INJ_ASE ][ RESERVED ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~  ~~~~~~~  ~~~~~  ~~~~  ~~~~~~~~~~~~~~~~  ~~~~~  ~~~~~~~~~  ~~~~~~~~~~
 *  bits [           39:39           ][38:36][ 35:31][30:10][ 9:8][       7:7      ][ 6:5 ][   4:4   ][    3:0   ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~  ~~~~~~~  ~~~~~  ~~~~  ~~~~~~~~~~~~~~~~  ~~~~~  ~~~~~~~~~  ~~~~~~~~~~
 *      MSB                                                                                                     LSB
 *
 *
 * ITMH-Extension header :
 *        _______  ______  __________
 * field [ VALUE ][ TYPE ][ RESERVED ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [ 23:2  ][  1:1 ][   0:0    ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                         LSB
 *
 *
 * ASE-OAM header :
 *        ______________  __________  _____________  ________  ______
 * field [ OAM_SUB_TYPE ][ MEP_TYPE ][ OAM_TS_DATA ][ OFFSET ][ TYPE ]
 *        ~~~~~~~~~~~~~~  ~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~  ~~~~~~
 *  bits [     47:44    ][   43     ][    42:9     ][   8:1  ][ 1:0  ]
 *        ~~~~~~~~~~~~~~  ~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~  ~~~~~~
 *      MSB                                                         LSB
 *
 *
 *
 * TSH-J2 header :
 *        ____________
 * field [ TIME_STAMP ]
 *        ~~~~~~~~~~~~
 *  bits [     31:0   ]
 *        ~~~~~~~~~~~~
 *      MSB          LSB
 *
 *
 * PPH-J2 header :
 *        _____________________  ____________________  ______________________  _____________  __________
 * field [ FORWARDING_STRENGTH ][ PARSING_START_TYPE ][ PARSING_START_OFFSET ][ ........... ][ RESERVED ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~~~
 *  bits [          95         ][        94:90       ][          89:83       ][     82:3    ][    2:0   ]
 *        ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~  ~~~~~~~~~~
 *      MSB                                                                                             LSB
 *
 *
 *
 * IPMF2 Key :
 *        _________________  ___________________  ____________  _________________  _______________  _____________________  ____________  ____________________  ___________________  _________________  ______________  __________________  ___________________________
 * field [PARSING_TYPE_QUAL][PARSING_OFFSET_QUAL][OAM_MEP_QUAL][OAM_SUB_TYPE_QUAL][OAM_OFFSET_QUAL][OAM_STAMP_OFFSET_QUAL][FWD_STR_QUAL][FWD_STR_RES_BIT_QUAL][TRAP_CODE_ONES_QUAL][ TIME_STAMP_QUAL ][ ASE_INJ_QUAL ][ OUT_LIF_DST_QUAL ][ TC_SNP_DST_DP_MIR_PPH_QUAL]
 *        ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [     134:130     ][      129:123      ][    122     ][     121:118     ][    117:110    ][      109:102        ][    101     ][      100:98        ][       97:89       ][     88:57       ][     56       ][     55:34        ][             33:0          ]
 *        ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                                                                                                                                                                                                                                                           LSB
 *
 *
 * IPMF3 Key:
 *        ________________________
 * field [PARSING_OFFSET_TYPE_QUAL]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~
 *  bits [         31:0           ]
 *        ~~~~~~~~~~~~~~~~~~~~~~~~
 *      MSB                       LSB
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
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
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#ifdef DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
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

/** IPMF2 Qualifiers info. */
static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_qual_info_array[ITMH_PPH_NOF_IPMF2_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {"tc_sn_dst_dp_mr_pph_q_pph", 0, TC_SNP_DST_DP_MIR_PPH_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, TC_SNP_DST_DP_MIR_PPH_QUAL_OFFSET}},
    {"ext_exist_q_pph", 0, EXT_PRESENT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, EXT_PRESENT_QUAL_OFFSET}},
    {"out_lif_dst_q_pph", 0, OUTLIF_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OUTLIF_QUAL_OFFSET}},
    {"ase_inj_q_pph", 0, ASE_INJ_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, ASE_INJ_QUAL_OFFSET}},
    {"time_stamp_q_pph", 0, TIME_STAMP_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, TIME_STAMP_QUAL_OFFSET}},
    {"fwd_trap_code_zero_q_pph", 0, TRAP_CODE_ZERO_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0, TRAP_CODE_ZERO_QUAL_OFFSET}},
    {"fwd_str_res_bit_q_1", 0, FWD_STR_RES_BIT_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_RES_BIT_QUAL_OFFSET}},
    {"fwd_str_q_pph", 0, FWD_STR_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_QUAL_OFFSET}},
    {"oam_stamp_offset_q_pph", 0, OAM_STAMP_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0, OAM_STAMP_OFFSET_QUAL_OFFSET}},
    {"oam_offset_q_pph", 0, OAM_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OAM_OFFSET_QUAL_OFFSET}},
    {"oam_sub_type_q_pph", 0, OAM_SUB_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OAM_SUB_TYPE_QUAL_OFFSET}},
    {"oam_mep_q_pph", 0, OAM_MEP_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, OAM_MEP_QUAL_OFFSET}}
};

/** IPMF2 Qualifiers info. */
static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_fg2_qual_info_array[ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {"pars_offset_valid_q_pph", 0, PARSING_OFFSET_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"parsing_offset_q_pph", 0, PARSING_OFFSET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, PARSING_OFFSET_QUAL_OFFSET}},
    {"pars_type_valid_q_pph", 0, PARSING_TYPE_VALID_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 1, 0}},
    {"parsing_type_q_pph", 0, PARSING_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, PARSING_TYPE_QUAL_OFFSET}},
    {"adj_size_layer_idx_q_pph", 0, SYS_HDR_ADJUST_SIZE_FWD_LAYER_INDEX_QUAL_LENGTH,
        {bcmFieldInputTypeConst, 0x7, 0}},
    {"ttl_set_q", 0, TTL_SET_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, TTL_SET_QUAL_OFFSET}},
    {"in_lif_profile_q_pph", 0, IN_LIF_PROFILE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, IN_LIF_PROFILE_QUAL_OFFSET}},
    {"fwd_additional_info_q_pph", 0, FWD_ADDITIONAL_INFO_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FWD_ADDITIONAL_INFO_QUAL_OFFSET}}
};

/** IPMF2 FG3 Qualifiers info. */
static field_itmh_pph_qual_info_t field_itmh_pph_ipmf2_fg3_qual_info_array[ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {"1bitCmpResult_q_pph", 0, 1,
        {bcmFieldInputTypeMetaData, 0, 0}},
	{"fwd_trap_code_q_pph", 0, TRAP_CODE_FHEI_QUAL_LENGTH,
	    {bcmFieldInputTypeLayerAbsolute, 0, TRAP_CODE_FHEI_QUAL_OFFSET}},
	{"fwd_str_res_bit_q_2", 0, FWD_STR_RES_BIT_QUAL_LENGTH,
	    {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_RES_BIT_QUAL_OFFSET}},
	{"fwd_str_q", 0, FWD_STR_QUAL_LENGTH,
	    {bcmFieldInputTypeLayerAbsolute, 0, FWD_STR_QUAL_OFFSET}},
	{"fwd_qualifier_q_pph", 0, FWD_QUALIFIER_QUAL_LENGTH,
			{bcmFieldInputTypeLayerAbsolute, 0, FWD_QUALIFIER_QUAL_OFFSET}}
};

/** IPMF1 compare key Qualifiers info. */
static field_itmh_pph_qual_info_t field_itmh_pph_cmp_key_qual_info_array[ITMH_PPH_NOF_CMP_KEY_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset  */
    {"fhei_type_q_pph", 0, FHEI_TYPE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FHEI_TYPE_QUAL_OFFSET}},
    {"fhei_size_q_pph", 0, FHEI_SIZE_QUAL_LENGTH,
        {bcmFieldInputTypeLayerAbsolute, 0, FHEI_SIZE_QUAL_OFFSET}},
    {"key_gen_var_q_pph", 0, 6,
        {bcmFieldInputTypeMetaData, 0, 0}}
};

/** IPMF3 Qualifiers info. */
static field_itmh_pph_qual_info_t field_itmh_pph_ipmf3_qual_info_array[ITMH_PPH_NOF_IPMF3_QUALIFIERS] = {
  /** qual_name  |  qual_id   |   qual_size     |  input_type    |    input_arg  |  qual_offset   */
    {NULL, bcmFieldQualifyContainer, 0, {bcmFieldInputTypeMetaData, 0, PARS_OFFSET_TYPE_SH_ADJUST_FWD_LAYER_QUAL_OFFSET}}
};

/**
 * IPMF2 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_action_info_array[ITMH_PPH_NOF_IPMF2_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {NULL, bcmFieldActionPrioIntNew, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"snoop_data_a_pph", 0, {0, bcmFieldActionSnoop, SNOOP_DATA_ACTION_LENGTH,
        15, 0x0007F, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"destination_a_pph", 0, {0, bcmFieldActionForward, DST_ACTION_LENGTH,
        11, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionDropPrecedence, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE },
    {"mirror_command_a_pph", 0, {0, bcmFieldActionVoid, MIRROR_ACTION_LENGTH,
        31, 0x00000000, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionPphPresentSet, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"ext_exist_a_pph", 0, {0, bcmFieldActionVoid, EXT_EXIST_ACTION_LENGTH,
        31, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"out_lif_a_pph", 0, {0, bcmFieldActionVoid, OUT_LIF_ACTION_LENGTH,
        10, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"udh3_1bit_ase_a_pph", 0, {0, bcmFieldActionUDHData2, ASE_INJ_ACTION_LENGTH,
        31, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionIngressTimeStampInsert, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"fwd_str_a_pph", 0, {0, bcmFieldActionTrap, FWD_STR_ACTION_LENGTH,
        19, 0x0, {0}}, FALSE, BCM_FIELD_ACTION_PRIORITY(0,10)},
    {NULL, bcmFieldActionOam, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};

/**
 * IPMF2 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_fg2_action_info_array[ITMH_PPH_NOF_IPMF2_FG2_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {"pars_start_sys_adjust_a_pph", 0, {0, bcmFieldActionContainer, PARS_START_SH_ADJUST_FWD_LAYER_ACTION_LENGTH,
        14, 0x0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionTtlSet, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionInVportClass0, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionForwardingAdditionalInfo, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};

/**
 * IPMF2 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_itmh_pph_action_info_t field_itmh_pph_ipmf2_fg3_action_info_array[ITMH_PPH_NOF_IPMF2_FG3_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {NULL, bcmFieldActionTrap, {0}, TRUE, BCM_FIELD_ACTION_PRIORITY(0,20)}
};

/**
 * IPMF3 Action info.
 * For actions, which are not user define, we need only BCM action ID.
 */
static field_itmh_pph_action_info_t field_itmh_pph_ipmf3_action_info_array[ITMH_PPH_NOF_IPMF3_ACTIONS] = {
/** {action_name}   |  action_id  |    { stage | action_type | size | prefix_size | prefix_value | name  } */
    {"act_pars_offset_a_pph", 0, {0, bcmFieldActionParsingStartOffset, PARSING_START_OFFSET_ACTION_LENGTH, 1, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionParsingStartType, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"act_s_h_adjust_size_a_pph", 0, {0, bcmFieldActionSystemHeaderSizeAdjust, SYS_HDR_ADJUST_SIZE_ACTION_LENGTH, 6, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {NULL, bcmFieldActionForwardingLayerIndex, {0}, FALSE, BCM_FIELD_ACTION_DONT_CARE},
    {"zero_action_a_pph", 0, {0, bcmFieldActionVoid, IPMF3_ZERO_ACTION_LENGTH, 18, 0, {0}}, FALSE, BCM_FIELD_ACTION_DONT_CARE}
};

/**
 * Array with all needed information about actions, which are using FEMs.
 */
static field_itmh_pph_fem_action_info_t field_itmh_pph_ipmf2_fem_action_info_array[ITMH_PPH_NOF_IPMF2_FEMS] = {
/** nof_conditions | condition_indexes | nof_extractions | nof_map_bits | array_id | fem_position_in_array | fem_input | condition_msb | fem_condition_info */
    /** OUT-LIF action */
    {8, {4, 5, 6, 7, 12, 13, 14, 15}, 1, 22, {1, 2}, {{32, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        /** bcm_action  |  extraction_info */
        {{bcmFieldActionOutVport0, {
            /** src_type                 |       offset | force_value */
            {bcmFieldFemExtractionOutputSourceTypeKey, 3, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 4, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 5, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 6, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 7, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 8, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 9, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 10, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 11, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 12, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 13, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 14, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 15, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 16, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 17, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 18, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 19, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 20, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 21, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 22, 0},
            {bcmFieldFemExtractionOutputSourceTypeKey, 23, 0}}, 0}}}},
    /** MIRROR action */
    {8, {2, 3, 6, 7, 10, 11, 14, 15}, 1, 9, {1, 3}, {{0, 0}, 3, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionMirrorIngress, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1}},0}}}},
    /** SystemProfile action */
    {8, {1, 3, 5, 7, 9, 11, 13, 15}, 1, 4, {1, 4}, {{32, 0}, 28, {{0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}, {0, 1}},
        {{bcmFieldActionFabricHeaderSet, {
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 1},
            {bcmFieldFemExtractionOutputSourceTypeForce, 0, 0}}, 0}}}}
};

/**
 * Array, which contains information per field group. Like:
 * fg_id, fg_type, qulas, actions, fem_info.
 */
static field_itmh_pph_fg_info_t itmh_pph_fg_info_array[ITMH_PPH_NOF_FG] = {
    /** IPMF2 field group info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF2,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        ITMH_PPH_NOF_IPMF2_QUALIFIERS,  /** Number of tested qualifiers */
        ITMH_PPH_NOF_IPMF2_ACTIONS, /** Number of tested actions */
        field_itmh_pph_ipmf2_qual_info_array,   /** Qualifiers info */
        field_itmh_pph_ipmf2_action_info_array, /** Actions info */
        field_itmh_pph_ipmf2_fem_action_info_array  /** FEM actions info */
    },
    /** IPMF2 field group 2 info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF2,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS,  /** Number of tested qualifiers */
        ITMH_PPH_NOF_IPMF2_FG2_ACTIONS, /** Number of tested actions */
        field_itmh_pph_ipmf2_fg2_qual_info_array,   /** Qualifiers info */
        field_itmh_pph_ipmf2_fg2_action_info_array, /** Actions info */
        NULL  /** FEM actions info */
    },
    /** IPMF2 field group 3 info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF2,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS,  /** Number of tested qualifiers */
        ITMH_PPH_NOF_IPMF2_FG3_ACTIONS, /** Number of tested actions */
        field_itmh_pph_ipmf2_fg3_qual_info_array,   /** Qualifiers info */
        field_itmh_pph_ipmf2_fg3_action_info_array, /** Actions info */
        NULL  /** FEM actions info */
    },
    /** IPMF3 field group info */
    {
        0,   /** Field group ID */
        bcmFieldStageIngressPMF3,   /** Field stage */
        bcmFieldGroupTypeDirectExtraction,  /** Field group type */
        ITMH_PPH_NOF_IPMF3_QUALIFIERS,  /** Number of tested qualifiers */
        ITMH_PPH_NOF_IPMF3_ACTIONS, /** Number of tested actions */
        field_itmh_pph_ipmf3_qual_info_array,   /** Qualifiers info */
        field_itmh_pph_ipmf3_action_info_array, /** Actions info */
        NULL  /** FEM actions info */
    }
};
/*
 * }
 */

/* *INDENT-ON* */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/**
 * \brief
 *  Used to configure the user qualifiers.
 *
 * \param [in] unit - The unit number.
 * \param [in] itmh_pph_qual_info - Contains all need information
 *                              for one user qualifier to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_user_qual_config(
    int unit,
    field_itmh_pph_qual_info_t * itmh_pph_qual_info)
{
    bcm_field_qualifier_info_create_t qual_info;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_qualifier_info_create_t_init(&qual_info);
    sal_strncpy_s((char *) (qual_info.name), itmh_pph_qual_info->name, sizeof(qual_info.name));
    qual_info.size = itmh_pph_qual_info->qual_length;
    SHR_IF_ERR_EXIT(bcm_field_qualifier_create(unit, 0, &qual_info, &(itmh_pph_qual_info->qual_id)));

exit:
    SHR_FUNC_EXIT;
}

 /*
  * Global and Static
  */

static shr_error_e
appl_dnx_field_itmh_pph_cmp_key_config(
    int unit)
{
    int qual_index;
    bcm_field_context_t context_id;
    bcm_field_context_compare_info_t compare_info;
    bcm_field_qualifier_info_get_t qual_info_get;

    SHR_FUNC_INIT_VARS(unit);

    for (qual_index = 0; qual_index < ITMH_PPH_NOF_CMP_KEY_QUALIFIERS; qual_index++)
    {
        if (field_itmh_pph_cmp_key_qual_info_array[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_config
                            (unit, &(field_itmh_pph_cmp_key_qual_info_array[qual_index])));
        }
    }

    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);

        /**
    * Init the compare_info
    */
    bcm_field_context_compare_info_t_init(&compare_info);
        /**
    * Fill the compare info
    */
    compare_info.first_key_info.nof_quals = 2;
    compare_info.first_key_info.qual_types[0] = field_itmh_pph_cmp_key_qual_info_array[0].qual_id;
    compare_info.first_key_info.qual_info[0].input_type =
        field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_type;
    compare_info.first_key_info.qual_info[0].input_arg =
        field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.input_arg;
    compare_info.first_key_info.qual_info[0].offset = field_itmh_pph_cmp_key_qual_info_array[0].qual_attach_info.offset;
    compare_info.first_key_info.qual_types[1] = field_itmh_pph_cmp_key_qual_info_array[1].qual_id;
    compare_info.first_key_info.qual_info[1].input_type =
        field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_type;
    compare_info.first_key_info.qual_info[1].input_arg =
        field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.input_arg;
    compare_info.first_key_info.qual_info[1].offset = field_itmh_pph_cmp_key_qual_info_array[1].qual_attach_info.offset;

    /**Get the offset of GLOB_OUT_LIF_0*/
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF1, &qual_info_get));

    compare_info.second_key_info.nof_quals = 1;
    compare_info.second_key_info.qual_types[0] = field_itmh_pph_cmp_key_qual_info_array[2].qual_id;
    compare_info.second_key_info.qual_info[0].input_type =
        field_itmh_pph_cmp_key_qual_info_array[2].qual_attach_info.input_type;
    compare_info.second_key_info.qual_info[0].offset = qual_info_get.offset;

   /**
    * Create the context compare mode.
    */
    SHR_IF_ERR_EXIT(bcm_field_context_compare_create(unit, 0, bcmFieldStageIngressPMF1, context_id, 1, &compare_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is used to create a context and configure
 *  the relevant program for the testing.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - Field stage on which context will be create.
 * \param [in] context_id - Context id to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_context(
    int unit,
    bcm_field_stage_t stage,
    bcm_field_context_t context_id)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    uint8 is_allocated;

    SHR_FUNC_INIT_VARS(unit);

    if (stage == bcmFieldStageIngressPMF2)
    {
        stage = bcmFieldStageIngressPMF1;
        /** Check if the specified Context ID is allocated. */
        SHR_IF_ERR_EXIT(dnx_algo_field_context_id_is_allocated(unit, DNX_FIELD_STAGE_IPMF1, context_id, &is_allocated));
        if (is_allocated)
        {
            goto exit;
        }
    }

    bcm_field_context_info_t_init(&context_info);
    if (stage == bcmFieldStageIngressPMF1)
    {
        /*
         * context_info.context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeDouble;
         */
        context_info.context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeDouble;
        sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF1", sizeof(context_info.name));
    }
    else
    {
        sal_strncpy_s((char *) (context_info.name), "ITMH_PPH_IPMF3", sizeof(context_info.name));
    }

    SHR_IF_ERR_EXIT(bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, stage, &context_info, &context_id));

    bcm_field_context_param_info_t_init(&param_info);
    if (stage == bcmFieldStageIngressPMF1)
    {
        param_info.param_type = bcmFieldContextParamTypeSystemHeaderProfile;
        param_info.param_val = bcmFieldSystemHeaderProfileFtmhTshPph;
    }
    else
    {
        /** Context in IPMF2 stage will configure bytes to remove 1 layer. */
        param_info.param_type = bcmFieldContextParamTypeSystemHeaderStrip;
        param_info.param_val = BCM_FIELD_PACKET_STRIP(bcmFieldPacketRemoveLayerForwardingOffset0, 0);
    }
    if (stage == bcmFieldStageIngressPMF1)
    {
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, stage, context_id, &param_info));
        /*
         * Set the key Gen Var to use it in compare 
         */
        bcm_field_context_param_info_t_init(&param_info);
        param_info.param_type = bcmFieldContextParamTypeKeyVal;
        param_info.param_val = 0x25;
        param_info.param_arg = 0;
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, stage, context_id, &param_info));

        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_cmp_key_config(unit));
    }
    else
    {
        /** bcmFieldContextParamTypeSystemHeaderStrip is a property of stage iPMF2 not iPMF3 */
        SHR_IF_ERR_EXIT(bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF2, context_id, &param_info));
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    /** For iPMF2, iPMF1 presel must be configured. */
    presel_entry_id.presel_id = dnx_data_field.preselector.default_itmh_pph_presel_id_ipmf1_get(unit);
    presel_entry_id.stage = stage;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 2;
    presel_entry_data.context_id = context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = 0x1F;
    /**
     * Check that bit 1 (FEC destination) and bit 2 (PPH_TYPE) of the LayerQualifier
     * are set to 1.
     */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0006;
    presel_entry_data.qual_data[1].qual_mask = 0x0006;
    SHR_IF_ERR_EXIT(bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Used to configure the user defined actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] field_stage - There is no meaning of stage in
 *                           user actions, but it should be parsed to the structure.
 * \param [in] itmh_pph_action_info - Contains all need information
 *                                for one user action to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_user_action_config(
    int unit,
    bcm_field_stage_t field_stage,
    field_itmh_pph_action_info_t * itmh_pph_action_info)
{
    bcm_field_action_info_t action_info;

    SHR_FUNC_INIT_VARS(unit);

    action_info.action_type = itmh_pph_action_info->action_info.action_type;
    sal_strncpy_s((char *) action_info.name, itmh_pph_action_info->name, sizeof(action_info.name));
    action_info.size = itmh_pph_action_info->action_info.size;
    action_info.stage = field_stage;
    action_info.prefix_size = itmh_pph_action_info->action_info.prefix_size;
    action_info.prefix_value = itmh_pph_action_info->action_info.prefix_value;

    SHR_IF_ERR_EXIT(bcm_field_action_create(unit, 0, &action_info, &(itmh_pph_action_info->action_id)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function is configuring all needed FEM actions.
 *
 * \param [in] unit - The unit number.
 * \param [in] fg_id - The field group ID, to which the FEM action
 *                     should be added.
 * \param [in] itmh_pph_fem_action_info - Contains all need information
 *                                    for one FEM action to be constructed.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_fem_action_config(
    int unit,
    bcm_field_group_t fg_id,
    field_itmh_pph_fem_action_info_t * itmh_pph_fem_action_info)
{
    int mapping_bits_index, conditions_index, extractions_index;
    bcm_field_fem_action_info_t fem_action_info;
    int *condition_id = itmh_pph_fem_action_info->condition_ids;
    bcm_field_action_priority_t action_priority;

    SHR_FUNC_INIT_VARS(unit);

    bcm_field_fem_action_info_t_init(&fem_action_info);

    fem_action_info.fem_input.input_offset = itmh_pph_fem_action_info->fem_action_info.fem_input.input_offset;
    fem_action_info.condition_msb = itmh_pph_fem_action_info->fem_action_info.condition_msb;
    for (conditions_index = 0; conditions_index < itmh_pph_fem_action_info->nof_conditions; conditions_index++)
    {
        fem_action_info.fem_condition[condition_id[conditions_index]].extraction_id =
            itmh_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].extraction_id;
        fem_action_info.fem_condition[condition_id[conditions_index]].is_action_valid =
            itmh_pph_fem_action_info->fem_action_info.fem_condition[conditions_index].is_action_valid;
    }
    for (extractions_index = 0; extractions_index < itmh_pph_fem_action_info->nof_extractions; extractions_index++)
    {
        fem_action_info.fem_extraction[extractions_index].action_type =
            itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].action_type;
        fem_action_info.fem_extraction[extractions_index].increment =
            itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].increment;
        for (mapping_bits_index = 0; mapping_bits_index < itmh_pph_fem_action_info->nof_mapping_bits;
             mapping_bits_index++)
        {
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].offset =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].offset;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].forced_value =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].forced_value;
            fem_action_info.fem_extraction[extractions_index].output_bit[mapping_bits_index].source_type =
                itmh_pph_fem_action_info->fem_action_info.fem_extraction[extractions_index].
                output_bit[mapping_bits_index].source_type;
        }
    }

    action_priority = BCM_FIELD_ACTION_POSITION(itmh_pph_fem_action_info->field_array_n_position.array_id,
                                                itmh_pph_fem_action_info->field_array_n_position.fem_position_in_array);
    SHR_IF_ERR_EXIT(bcm_field_fem_action_add(unit, 0, fg_id, action_priority, &fem_action_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This function creates field group, with all
 *  relevant information to it and adding it to the context.
 *
 * \param [in] unit - The unit number.
 * \param [in] itmh_pph_fg_info - Contains all need information
 *                            for one group to be created.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
appl_dnx_field_itmh_pph_group_config(
    int unit,
    field_itmh_pph_fg_info_t * itmh_pph_fg_info)
{
    int qual_index, action_index, fem_index;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_t context_id;
    bcm_field_qualifier_info_get_t qual_info_get;
    int op_offset;

    SHR_FUNC_INIT_VARS(unit);

    context_id = dnx_data_field.context.default_itmh_pph_context_get(unit);

    SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_context(unit, itmh_pph_fg_info->stage, context_id));

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.fg_type = itmh_pph_fg_info->fg_type;
    fg_info.stage = itmh_pph_fg_info->stage;
    if (fg_info.stage == bcmFieldStageIngressPMF2)
    {
        sal_strncpy_s((char *) (fg_info.name), "ITMH_PPH_IPMF2", sizeof(fg_info.name));
    }
    else
    {
        sal_strncpy_s((char *) (fg_info.name), "ITMH_PPH_IPMF3", sizeof(fg_info.name));
    }
    fg_info.nof_quals = itmh_pph_fg_info->nof_quals;
    fg_info.nof_actions = itmh_pph_fg_info->nof_actions;
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    /**
     * Iterate over all qualifiers for current field group. Using field_itmh_pph_user_qual_config()
     * to create all user define qualifiers internal function.
     */
    for (qual_index = 0; qual_index < fg_info.nof_quals; qual_index++)
    {
        if (itmh_pph_fg_info->itmh_pph_qual_info[qual_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_qual_config
                            (unit, &(itmh_pph_fg_info->itmh_pph_qual_info[qual_index])));
        }
        fg_info.qual_types[qual_index] = itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_types[qual_index] = itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_id;
        attach_info.key_info.qual_info[qual_index].input_type =
            itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.input_type;
        attach_info.key_info.qual_info[qual_index].input_arg =
            itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.input_arg;
        attach_info.key_info.qual_info[qual_index].offset =
            itmh_pph_fg_info->itmh_pph_qual_info[qual_index].qual_attach_info.offset;
    }

    /**
     * Iterate over all actions for current field group. For those, which are user define,
     * use field_itmh_pph_user_action_config() internal function.
     */
    for (action_index = 0; action_index < fg_info.nof_actions; action_index++)
    {
        if (itmh_pph_fg_info->itmh_pph_action_info[action_index].name != NULL)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_user_action_config
                            (unit, fg_info.stage, &(itmh_pph_fg_info->itmh_pph_action_info[action_index])));
        }
        fg_info.action_types[action_index] = itmh_pph_fg_info->itmh_pph_action_info[action_index].action_id;
        fg_info.action_with_valid_bit[action_index] =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_with_valid_bit;
        attach_info.payload_info.action_types[action_index] =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_id;
        attach_info.payload_info.action_info[action_index].priority =
            itmh_pph_fg_info->itmh_pph_action_info[action_index].action_priority;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &(itmh_pph_fg_info->fg_id)));

    /** Configuring the FEM actions. */
    if (fg_info.stage == bcmFieldStageIngressPMF2 && itmh_pph_fg_info->itmh_pph_fem_action_info != NULL)
    {
        for (fem_index = 0; fem_index < ITMH_PPH_NOF_IPMF2_FEMS; fem_index++)
        {
            SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_fem_action_config
                            (unit, itmh_pph_fg_info->fg_id, &(itmh_pph_fg_info->itmh_pph_fem_action_info[fem_index])));
        }
    }

    /**
    * Retrieve the desired result offset of the qualifier.
    */
    SHR_IF_ERR_EXIT(bcm_field_qualifier_info_get
                    (unit, bcmFieldQualifyCompareKeysResult0, bcmFieldStageIngressPMF2, &qual_info_get));

   /**
    * Get the compare opernad offset.
    */
    SHR_IF_ERR_EXIT(bcm_field_compare_operand_offset_get(unit, 1, bcmFieldCompareOperandEqual, &op_offset));

    /*
     * for iPMF2 FG3 we need to add the offset of the comapre operand 
     */
    if ((itmh_pph_fg_info->itmh_pph_qual_info[0].qual_attach_info.input_type == bcmFieldInputTypeMetaData) &&
        (itmh_pph_fg_info->itmh_pph_qual_info[0].name != NULL))
    {
        attach_info.key_info.qual_info[0].offset =
            itmh_pph_fg_info->itmh_pph_qual_info[0].qual_attach_info.offset + qual_info_get.offset + op_offset;

    }

    SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, itmh_pph_fg_info->fg_id, context_id, &attach_info));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function initializes the ITMH programmable mode
 *   application, extention to ITMH context that might include ASE_TSH_PPH headers.
 *   This function sets all required HW configuration for ITMH
 *   processing when working in ITMH programmable mode.
 *   The function configures field database of direct extraction
 *   mode in order to extract all itmh information form the
 *   header.
 *   The header information is used to perform actions (such as
 *   mirror, snoop command, change traffic class, drop
 *   precedence) In adding the new header profile is set
 *   according to the type (unicast/multicast).
 *  In case ASE_TSH_PPH header exist will also set the HW to parse them as well
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e
appl_dnx_field_itmh_pph_init(
    int unit)
{
    int fg_index;
/*
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_param_info_t param_info;
*/
    SHR_FUNC_INIT_VARS(unit);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit) > 1)
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Skipping initializing field ITMH PPH in emulation to save time%s%s%s\n", EMPTY,
                          EMPTY, EMPTY);
    }
#endif

    /**
     * Iterate over 3 DE field groups and create them one by one.
     * Parsing the global array "itmh_pph_fg_info_array[]" with all needed
     * information for both field groups.
     */
    for (fg_index = 0; fg_index < ITMH_PPH_NOF_FG; fg_index++)
    {
        SHR_IF_ERR_EXIT(appl_dnx_field_itmh_pph_group_config(unit, &itmh_pph_fg_info_array[fg_index]));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
