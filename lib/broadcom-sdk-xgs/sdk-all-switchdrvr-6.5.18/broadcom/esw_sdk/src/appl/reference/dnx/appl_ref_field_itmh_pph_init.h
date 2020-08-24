/** \file appl_ref_field_itmh_pph_init.h
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED

/*
 * }
 */

/*
* Include files.
* {
*/

#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/error.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** Number of Actions (used as array size) */
#define ITMH_PPH_NOF_IPMF2_ACTIONS           12
#define ITMH_PPH_NOF_IPMF2_FG2_ACTIONS       4
#define ITMH_PPH_NOF_IPMF2_FG3_ACTIONS       1
#define ITMH_PPH_NOF_IPMF3_ACTIONS           5
/** Number of Data Qualifiers needed (used as array size) */
#define ITMH_PPH_NOF_IPMF2_QUALIFIERS        12
#define ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS    8
#define ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS    5
#define ITMH_PPH_NOF_CMP_KEY_QUALIFIERS      3
#define ITMH_PPH_NOF_IPMF3_QUALIFIERS        1
/** Number of FEMs in IPMF2 (used as array size) */
#define ITMH_PPH_NOF_IPMF2_FEMS              3
/** Number of Field Groups needed (used as array size) */
#define ITMH_PPH_NOF_FG                      4

/** Size of the ITMH header. */
#define HEADER_0_SIZE        40
/** Size of the ITMH-Ext header. */
#define HEADER_1_SIZE        24
/** Size of the ASE-OAM header. */
#define HEADER_2_SIZE        48
/** Size of the TSH header. */
#define HEADER_3_SIZE        32
/** Size of the PPH header. */
#define HEADER_4_SIZE        96

/**
 * Offset of TC Qualifier for ITMH Header[38:36] (3b)
 * Offset of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Offset of DST Qualifier for ITMH Header[30:10] (21b)
 * Offset of DP Qualifier for ITMH Header[9:8] (2b)
 * Offset of MIRROR Qualifier for ITMH Header[7:7] (1b)
 * Offset of PPH Qualifier for ITMH Header[6:5] (2b)
 */
#define TC_SNP_DST_DP_MIR_PPH_QUAL_OFFSET  5
/** Offset of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define EXT_PRESENT_QUAL_OFFSET  39
/** Offset of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (2b) */
#define OUTLIF_QUAL_OFFSET    (HEADER_0_SIZE + 2)
/** Offset of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define ASE_INJ_QUAL_OFFSET           4
/** Offset of TIME_STAMP Qualifier for TSH Header[31:0] (32b) */
#define TIME_STAMP_QUAL_OFFSET        0
/** Offset of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define TRAP_CODE_ZERO_QUAL_OFFSET    0
/**
 *  Offset of FWD_STRENGTH Qualifier (4b):
 *      - 3 bits taken from LSB part of RESERVED field in ITMH header[2:0]
 *      - 1 bit taken from FORWARDING_STRENGTH field in PPH header[95:95]
 */
#define FWD_STR_RES_BIT_QUAL_OFFSET   0
#define FWD_STR_QUAL_OFFSET           95
#define TRAP_CODE_FHEI_QUAL_OFFSET    (HEADER_4_SIZE + 27)
#define FWD_QUALIFIER_QUAL_OFFSET     (HEADER_4_SIZE + 0)
/** Offset of OAM_MEP_OFFSET Qualifier for ASE_OAM Header[43:43] (1b) */
#define OAM_MEP_QUAL_OFFSET           4
/** Offset of OAM_SUB_TYPE Qualifier for ASE_OAM Header[47:44] (4b) */
#define OAM_SUB_TYPE_QUAL_OFFSET      0
/** Offset of OAM_OFFSET Qualifier for ASE_OAM Header[8:1] (8b) */
#define OAM_OFFSET_QUAL_OFFSET        39
/** Offset of OAM_STAMP_OFFSET Qualifier, used to fill the oam_stamp_offset with constant value (8b) */
#define OAM_STAMP_OFFSET_QUAL_OFFSET  0
/** Offset of PARSING_OFFSET Qualifier for PPH Header[89:83] (7b) */
#define PARSING_OFFSET_QUAL_OFFSET    83
/** Offset of PARSING_TYPE Qualifier for PPH Header[94:90] (5b) */
#define PARSING_TYPE_QUAL_OFFSET      90
/** Offset of FHEI_SIZE Qualifier for PPH Header[78:79] (2b) */
#define FHEI_SIZE_QUAL_OFFSET         78
/** Offset of FHEI_Type Qualifier for FHEI  Header[36:39] (4b) */
#define FHEI_TYPE_QUAL_OFFSET         (HEADER_4_SIZE + 36)
/** IPMF3: Offset of :
 *      - PARSING_OFFSET, PARSING_TYPE Qualifiers (12b)
 *      - SYS_HDR_ADJUST_SIZE Qualifier (1b)
 *      - FWD_LAYER_INDEX Qualifier (3b)
 */
#define PARS_OFFSET_TYPE_SH_ADJUST_FWD_LAYER_QUAL_OFFSET         0
/** Offset of TTL_SET Qualifier for PPH Header[76:69] (8b) */
#define TTL_SET_QUAL_OFFSET      69
/** Offset of IN_LIF_PROFILE Qualifier for PPH Header[20:13] (8b) */
#define IN_LIF_PROFILE_QUAL_OFFSET      13
/** Offset of FWD_ADDITIONAL_INFO Qualifier for PPH Header[9:6] (4b) */
#define FWD_ADDITIONAL_INFO_QUAL_OFFSET      6

/**
 * Length of TC Qualifier for ITMH Header[38:36] (3b)
 * Length of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Length of DST Qualifier for ITMH Header[30:10] (21b)
 * Length of DP Qualifier for ITMH Header[9:8] (2b)
 * Length of MIRROR Qualifier for ITMH Header[7:7] (1b)
 * Length of PPH Qualifier for ITMH Header[6:5] (2b)
 */
#define TC_SNP_DST_DP_MIR_PPH_QUAL_LENGTH  34
/** Length of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define EXT_PRESENT_QUAL_LENGTH  1
/** Length of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (22b) */
#define OUTLIF_QUAL_LENGTH    22
/** Length of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define ASE_INJ_QUAL_LENGTH           1
/** Length of TIME_STAMP Qualifier for TSH Header[31:0] (32b) */
#define TIME_STAMP_QUAL_LENGTH        32
/** Length of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define TRAP_CODE_ZERO_QUAL_LENGTH    9
/**
 *  Length of FWD_STRENGTH Qualifier (4b):
 *      - 3 bits taken from LSB part of RESERVED field in ITMH header[2:0]
 *      - 1 bit taken from FORWARDING_STRENGTH field in PPH header[95:95]
 */
#define FWD_STR_RES_BIT_QUAL_LENGTH   3
#define FWD_STR_QUAL_LENGTH           1
#define FWD_QUALIFIER_QUAL_LENGTH     19
#define TRAP_CODE_FHEI_QUAL_LENGTH    9
/** Length of OAM_MEP_OFFSET Qualifier for ASE_OAM Header[43:43] (1b) */
#define OAM_MEP_QUAL_LENGTH           1
/** Length of OAM_SUB_TYPE Qualifier for ASE_OAM Header[47:44] (4b) */
#define OAM_SUB_TYPE_QUAL_LENGTH      4
/** Length of OAM_OFFSET Qualifier for ASE_OAM Header[8:1] (8b) */
#define OAM_OFFSET_QUAL_LENGTH        8
/** Length of OAM_STAMP_OFFSET Qualifier, used to fill the oam_stamp_offset with constant value (8b) */
#define OAM_STAMP_OFFSET_QUAL_LENGTH   8
/** Length of PARSING_OFFSET_VALID Qualifier, used to fill the parsing_start_offset_valid with constant value of "1" (1b) */
#define PARSING_OFFSET_VALID_QUAL_LENGTH    1
/** Length of PARSING_OFFSET Qualifier for PPH Header[89:83] (7b) */
#define PARSING_OFFSET_QUAL_LENGTH    7
/** Length of PARSING_TYPE_VALID Qualifier, used to fill the parsing_start_type_valid with constant value of "1" (1b) */
#define PARSING_TYPE_VALID_QUAL_LENGTH    1
/** Length of PARSING_TYPE Qualifier for PPH Header[94:90] (5b) */
#define PARSING_TYPE_QUAL_LENGTH      5
/** IPMF3: Length of PACKET_STRIP Qualifier (9b) */
#define PACKET_STRIP_QUAL_LENGTH      9
/** Length of TTL_SET Qualifier for PPH Header[76:69] (8b) */
#define TTL_SET_QUAL_LENGTH     8
/** Length of IN_LIF_PROFILE Qualifier for PPH Header[20:13] (8b) */
#define IN_LIF_PROFILE_QUAL_LENGTH     8
/** Length of FWD_ADDITIONAL_INFO Qualifier for PPH Header[9:6] (4b) */
#define FWD_ADDITIONAL_INFO_QUAL_LENGTH    4
/**
 * Length of FWD_LAYER_INDEX Qualifier, used to fill the fwd_layer_idex with constant value of "3" (3b)
 * Length of SYS_HDR_ADNUST_SIZE Qualifier, used to fill the sys_hdr_adjust_size with constant value of "1" (1b)
 */
#define SYS_HDR_ADJUST_SIZE_FWD_LAYER_INDEX_QUAL_LENGTH    4
/** Length of FHEI_SIZE Qualifier for PPH Header[78:79] (2b) */
#define FHEI_SIZE_QUAL_LENGTH      2
/** Length of FHEI_Type Qualifier for FHEI Header[36:39] (4b) */
#define FHEI_TYPE_QUAL_LENGTH      4

/** Length of user defined SNOOP_DATA action (5b) */
#define SNOOP_DATA_ACTION_LENGTH         5
/** Length of user defined DESTINATION action (21b) */
#define DST_ACTION_LENGTH                21
/** Length of user defined EXT_EXIST action (1b) */
#define EXT_EXIST_ACTION_LENGTH          1
/** Length of user defined OUT_LIF action (21b) */
#define OUT_LIF_ACTION_LENGTH            22
/** Length of user defined ASE_INJ action (1b) */
#define ASE_INJ_ACTION_LENGTH            1
/** Length of user defined MIRROR action (1b) */
#define MIRROR_ACTION_LENGTH             1
/** Length of user defined FWD_STRENGTH action (13b) */
#define FWD_STR_ACTION_LENGTH           13
/** Length of user defined OAM_DATA action (13b) */
#define OAM_DATA_ACTION_LENGTH          13
/**
 * Length of user defined action which encodes:
 *      - PARSING action (14b)
 *      - SYS HDR ADJUST (1b) ---> Const value of 1
 *      - FWD_LAYER_INDEX (3b) ---> Const value of 3
 */
#define PARS_START_SH_ADJUST_FWD_LAYER_ACTION_LENGTH     18

#define IPMF3_ZERO_ACTION_LENGTH                14
/** IPMF3: Length of user defined SYS HDR ADJUST (1b) */
#define SYS_HDR_ADJUST_SIZE_ACTION_LENGTH       1

/**
 * Structure, which contains information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_qualify_t qual_id;
    uint32 qual_length;
    bcm_field_qualify_attach_info_t qual_attach_info;
} field_itmh_pph_qual_info_t;

/**
 * Structure, which contains need information,
 * for creating of user actions and predefined one.
 */
typedef struct
{
    char *name;
    bcm_field_action_t action_id;
    bcm_field_action_info_t action_info;
    uint8 action_with_valid_bit;
    bcm_field_action_priority_t action_priority;
} field_itmh_pph_action_info_t;

/**
 * Structure, which contains need information
 * for configuring of FEM condition actions.
 */
typedef struct
{
    int nof_conditions;
    int condition_ids[16];
    int nof_extractions;
    int nof_mapping_bits;
    /*
     * Structure replacing FEM_ID to indicate selected FEM by
     * its priority in the general FES/FEM structure.
     * See DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
     */
    bcm_field_array_n_position_t field_array_n_position;
    bcm_field_fem_action_info_t fem_action_info;
} field_itmh_pph_fem_action_info_t;

/**
 * Structure, which contains all needed information
 * about configuring of one field group.
 */
typedef struct
{
    bcm_field_group_t fg_id;
    bcm_field_stage_t stage;
    bcm_field_group_type_t fg_type;
    int nof_quals;
    int nof_actions;
    field_itmh_pph_qual_info_t *itmh_pph_qual_info;
    field_itmh_pph_action_info_t *itmh_pph_action_info;
    field_itmh_pph_fem_action_info_t *itmh_pph_fem_action_info;
} field_itmh_pph_fg_info_t;

/*
 * }
 */

/**
 * \brief
 * configure PMF context to handle TM program to parse PPH over ITMH packets
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   bcm_error_e
 */
shr_error_e appl_dnx_field_itmh_pph_init(
    int unit);
/*
 * }
 */
#endif /* APPL_REF_FIELD_ITMH_PPH_INIT_H_INCLUDED */
