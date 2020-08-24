/** \file appl_ref_j1_itmh_pph_init.h
 * J1 ITMH + PPH Programmable mode appl
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

/*
* Include files.
* {
*/
#include <appl/diag/parse.h>
/*
 * }
 */

#ifndef APPL_REF_FIELD_J1_ITMH_PPH_INIT_H_INCLUDED
/*
 * {
 */
#define APPL_REF_FIELD_J1_ITMH_PPH_INIT_H_INCLUDED

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Size of the ITMH header. */
#define J1_ITMH_PPH_HEADER_0_SIZE        32
/** Size of the ITMH-Ext header. */
#define J1_ITMH_PPH_HEADER_1_SIZE        24
/** Size of the OTSH header. */
#define J1_ITMH_PPH_HEADER_2_SIZE        48
/** Size of the PPH header. */
#define J1_ITMH_PPH_HEADER_3_SIZE        56

/** Number of Actions (used as array size) */
#define J1_ITMH_PPH_NOF_IPMF2_ACTIONS           9
/** Number of Data Qualifiers needed (used as array size) */
#define J1_ITMH_PPH_NOF_IPMF2_QUALIFIERS        14
/** Number of FEMs in IPMF2 (used as array size) */
#define J1_ITMH_PPH_NOF_IPMF2_FEMS              3
/** Number of Field Groups needed (used as array size) */
#define J1_ITMH_PPH_NOF_FG                      4
/** ID of field group, which is going to be created in IPMF2 stage */
#define J1_ITMH_PPH_IPMF2_FG_ID                 14

#define J1_ITMH_PPH_NOF_IPMF2_FG2_ACTIONS       4
#define J1_ITMH_PPH_NOF_IPMF2_FG3_ACTIONS       2
#define J1_ITMH_PPH_NOF_IPMF3_ACTIONS           5
/** Number of Data Qualifiers needed (used as array size) */

#define J1_ITMH_PPH_NOF_IPMF2_FG2_QUALIFIERS    8
#define J1_ITMH_PPH_NOF_IPMF2_FG3_QUALIFIERS    7

#define J1_ITMH_PPH_NOF_IPMF3_QUALIFIERS        2

#define J1_ITMH_PPH_NOF_CMP_KEY_QUALIFIERS      3

/** Offset of PPH Qualifier for ITMH Header[1:0] (2b) */
#define J1_ITMH_PPH_PPH_TYPE_QUAL_OFFSET          0
/**
 * Offset of DP Qualifier for ITMH Header[9:8] (2b)
 * Offset of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Offset of TC Qualifier for ITMH Header[38:36] (3b)
 * Offset of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_OFFSET  3
/** Offset of EXTENSION PRESENT qualifier in ITMH header [31:31] (1b) */
#define  J1_ITMH_PPH_EXT_PRESENT_QUAL_OFFSET  31
/** Offset of OUT_LIF Qualifier for ITMH-Ext Header[4:23] (20b) */
#define  J1_ITMH_PPH_OUT_LIF_QUAL_OFFSET    36
/** offset of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define J1_ITMH_PPH_ASE_INJ_QUAL_OFFSET           4
/** Offset of MIRROR Qualifier for ITMH Header[2:2] (1b) */
#define J1_ITMH_PPH_MIRROR_QUAL_OFFSET            2
/** Offset of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_OFFSET    0
/**
 *  Offset of FWD_STRENGTH Qualifier (3b):
 *      - 2 bits taken from SNOOP_CODE_CPU field in PPH header[17:16]
 *      - 1 bit taken from PACKET_IS_CTRL field in PPH header[15:15]
 */
#define J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_OFFSET   16
#define J1_ITMH_PPH_FWD_STR_QUAL_OFFSET           15
/** Offset of OAM_VALID_BIT Qualifier for ITMH Header[0:0] (1b) */
#define J1_ITMH_PPH_OAM_VALID_BIT_QUAL_OFFSET      0

/** Offset of forwarding offset Qualifier for PPH Header[8:15] (7b) */
#define J1_ITMH_PPH_PARSING_OFFSET_QUAL_OFFSET    8
/** Offset of PARSING_TYPE Qualifier for PPH Header[94:90] (5b) */
#define J1_ITMH_PPH_PARSING_TYPE_QUAL_OFFSET      4
/** Offset of FHEI_SIZE Qualifier for PPH Header[2:3] (2b) */
#define J1_ITMH_PPH_FHEI_SIZE_QUAL_OFFSET         2
/** Offset of FHEI_Type Qualifier for PPH  Header[4:7] (4b) */
#define J1_ITMH_PPH_FHEI_TYPE_QUAL_OFFSET         4

/** Offset of OAM_MEP_OFFSET Qualifier for OTSH Header[42:42] (1b) */
#define J1_ITMH_PPH_OAM_MEP_QUAL_OFFSET           5
/** Offset of OAM_SUB_TYPE Qualifier for OTSH Header[45:43] (3b) */
#define J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_OFFSET      2
/** Offset of OAM_OFFSET Qualifier for OTSH Header[7:0] (8b) */
#define J1_ITMH_PPH_OAM_OFFSET_QUAL_OFFSET        40
/** Offset of OAM_STAMP_OFFSET Qualifier, used to fill the oam_stamp_offset with constant value (8b) */
#define J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_OFFSET  0

#define J1_ITMH_PPH_PARS_OFFSET_TYPE_SH_ADJUST_QUAL_OFFSET         0
/** Offset of TTL_SET Qualifier for PPH_FHEI(IP_Routed) Header[16:23] (8b) */
#define J1_ITMH_PPH_TTL_SET_QUAL_OFFSET      (J1_ITMH_PPH_HEADER_3_SIZE + 16)
/** Offset of IN_LIF_PROFILE Qualifier FHEI(IP_routed).In-LIF profile [1:0] (2b) */
#define J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_OFFSET      (J1_ITMH_PPH_HEADER_3_SIZE + 0)
/** Offset of FWD_ADDITIONAL_INFO Qualifier for PPH Header[4:7] (4b) */
#define J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_OFFSET      4
/** Offset of In-LIF Qualifier for PPH Header[38:55] (18b) */
#define J1_ITMH_PPH_IN_LIF_QUAL_OFFSET         38
/** Offset of FWD_Domain_INFO Qualifier taken from PPH Header.VSI/In-RIF [22:37] (16b) */
#define J1_ITMH_PPH_FWD_DOMAIN_QUAL_OFFSET     22
/** Offset of Network Oos Qualifier from FHEI(IP_routed).In-DSCP [15:8] (8b) */
#define J1_ITMH_PPH_NETWORK_QOS_QUAL_OFFSET   (J1_ITMH_PPH_HEADER_3_SIZE + 8)

/** Length of PPH Qualifier for ITMH Header[6:5] (2b) */
#define J1_ITMH_PPH_PPH_TYPE_QUAL_LENGTH          2
/**
 * Length of DP Qualifier for ITMH Header[9:8] (2b)
 * Length of SNOOP_DATA Qualifier for ITMH Header[35:31] (5b)
 * Length of TC Qualifier for ITMH Header[38:36] (3b)
 * Length of DST Qualifier for ITMH Header[30:10] (21b)
 */
#define  J1_ITMH_PPH_DP_DST_SNOOP_TC_QUAL_LENGTH 28
/** Length of EXTENSION PRESENT qualifier in ITMH header [39:39] (1b) */
#define  J1_ITMH_PPH_EXT_PRESENT_QUAL_LENGTH  1
/** Length of OUT_LIF Qualifier for ITMH-Ext Header[23:2] (22b) */
#define  J1_ITMH_PPH_OUT_LIF_QUAL_LENGTH    20
/** Length of ASE_INJ Qualifier for ITMH Header[4:4] (1b) */
#define J1_ITMH_PPH_ASE_INJ_QUAL_LENGTH           1
/** Length of MIRROR Qualifier for ITMH Header[7:7] (1b) */
#define J1_ITMH_PPH_MIRROR_QUAL_LENGTH            1
/** Length of TRAP_CODE Qualifier, used to fill the trap code with constant value (9b) */
#define J1_ITMH_PPH_TRAP_CODE_ZEROS_QUAL_LENGTH    9
/**
 *  Offset of FWD_STRENGTH Qualifier (3b):
 *      - 2 bits taken from SNOOP_CODE_CPU field in PPH header[17:16]
 *      - 1 bit taken from PACKET_IS_CTRL field in PPH header[15:15]
 */
#define J1_ITMH_PPH_FWD_STR_RES_BIT_QUAL_LENGTH   2
#define J1_ITMH_PPH_FWD_STR_QUAL_LENGTH           1
/** Length of OAM_VALID_BIT Qualifier (1b) */
#define J1_ITMH_PPH_OAM_VALID_BIT_QUAL_LENGTH   1

/** Length of user defined SNOOP_DATA action (4b) */
#define  J1_ITMH_PPH_SNOOP_DATA_ACTION_LENGTH         4
/** Length of user defined DESTINATION action (19b) */
#define  J1_ITMH_PPH_DST_ACTION_LENGTH                19
/** Length of user defined EXT_EXIST action (1b) */
#define  J1_ITMH_PPH_EXT_EXIST_ACTION_LENGTH          1
/** Length of user defined OUT_LIF action (21b) */
#define  J1_ITMH_PPH_OUT_LIF_ACTION_LENGTH            20
/** Length of user defined ASE_INJ action (1b) */
#define  J1_ITMH_PPH_ASE_INJ_ACTION_LENGTH            1
/** Length of user defined MIRROR action (1b) */
#define  J1_ITMH_PPH_MIRROR_ACTION_LENGTH             1
/** Length of user defined FWD_STRENGTH action (12b) */
#define J1_ITMH_PPH_FWD_STR_ACTION_LENGTH           12

/** Length of OAM_MEP_OFFSET Qualifier for ASE_OAM Header[43:43] (1b) */
#define J1_ITMH_PPH_OAM_MEP_QUAL_LENGTH           1
/** Length of OAM_SUB_TYPE Qualifier for ASE_OAM Header[47:44] (4b) */
#define J1_ITMH_PPH_OAM_SUB_TYPE_QUAL_LENGTH      3
/** Length of OAM_OFFSET Qualifier for ASE_OAM Header[8:1] (8b) */
#define J1_ITMH_PPH_OAM_OFFSET_QUAL_LENGTH        8
/** Length of OAM_STAMP_OFFSET Qualifier, used to fill the oam_stamp_offset with constant value (8b) */
#define J1_ITMH_PPH_OAM_STAMP_OFFSET_QUAL_LENGTH   8

/** Length of PARSING_OFFSET_VALID Qualifier, used to fill the parsing_start_offset_valid with constant value of "1" (1b) */
#define J1_ITMH_PPH_PARSING_OFFSET_VALID_QUAL_LENGTH    1
/** Length of PARSING_OFFSET Qualifier for PPH Header[89:83] (7b) */
#define J1_ITMH_PPH_PARSING_OFFSET_QUAL_LENGTH    7
/** Length of PARSING_TYPE_VALID Qualifier, used to fill the parsing_start_type_valid with constant value of "1" (1b) */
#define J1_ITMH_PPH_PARSING_TYPE_VALID_QUAL_LENGTH    1
/** Length of PARSING_TYPE Qualifier for PPH Header[94:90] (5b) */
#define J1_ITMH_PPH_PARSING_TYPE_QUAL_LENGTH      4
/** IPMF3: Length of PACKET_STRIP Qualifier (9b) */
#define J1_ITMH_PPH_PACKET_STRIP_QUAL_LENGTH      9
/** Length of TTL_SET Qualifier for PPH Header[76:69] (8b) */
#define J1_ITMH_PPH_TTL_SET_QUAL_LENGTH     8
/** Length of Network Oos Qualifier from FHEI(IP_routed).In-DSCP [15:8] (8b) */
#define J1_ITMH_PPH_NETWORK_QOS_QUAL_LENGTH 8
/** Length of IN_LIF_PROFILE Qualifier for PPH Header[20:13] (8b) */
#define J1_ITMH_PPH_IN_LIF_PROFILE_QUAL_LENGTH     2
/** Length of FWD_ADDITIONAL_INFO Qualifier for PPH Header[4:7] (4b) */
#define J1_ITMH_PPH_FWD_ADDITIONAL_INFO_QUAL_LENGTH    4
/** Length of In-LIF Qualifier for PPH Header[38:55] (18b) */
#define J1_ITMH_PPH_IN_LIF_QUAL_LENGTH    18
/** Length of FWD_Domain_INFO Qualifier taken from PPH Header.VSI/In-RIF [22:37] (16b) */
#define J1_ITMH_PPH_FWD_DOMAIN_QUAL_LENGTH 16
/** Length of FWD_LAYER_INDEX Qualifier Constant value of 0x2 (3b) */
#define J1_ITMH_PPH_FWD_LAYER_INDEX_QUAL_LENGTH 3

/** Length of FHEI_SIZE Qualifier for PPH Header[78:79] (2b) */
#define J1_ITMH_PPH_FHEI_SIZE_QUAL_LENGTH      2
/** Length of FHEI_Type Qualifier for FHEI Header[36:39] (4b) */
#define J1_ITMH_PPH_FHEI_TYPE_QUAL_LENGTH      4
/**
 * Length of SYS_HDR_ADJUST_SIZE Qualifier, used to fill the sys_hdr_adjust_size with constant value of "1" (1b)
 */
#define J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_QUAL_LENGTH    1

/**
 * Length of user defined action which encodes:
 *      - PARSING action (14b)
 *      - SYS HDR ADJUST (1b) ---> Const value of 1
 */
#define J1_ITMH_PPH_PARS_START_SH_ADJUST_ACTION_LENGTH     14

#define J1_ITMH_PPH_IPMF3_ZERO_ACTION_LENGTH                18
/** IPMF3: Length of user defined SYS HDR ADJUST (1b) */
#define J1_ITMH_PPH_SYS_HDR_ADJUST_SIZE_ACTION_LENGTH       1
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

} field_j1_itmh_pph_qual_info_t;

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

} field_j1_itmh_pph_action_info_t;

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
/*    bcm_field_fem_id_t fem_id; #### REMOVE */
    /*
     * Structure replacing FEM_ID to indicate selected FEM by
     * its priority in the general FES/FEM structure.
     * See DNX_FIELD_ACTION_PRIORITY_POSITION_FEM_ID_GET
     */
    bcm_field_array_n_position_t field_array_n_position;
    bcm_field_fem_action_info_t fem_action_info;

} field_j1_itmh_pph_fem_action_info_t;

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
    field_j1_itmh_pph_qual_info_t *itmh_qual_info;
    field_j1_itmh_pph_action_info_t *itmh_action_info;
    int nof_fems;
    field_j1_itmh_pph_fem_action_info_t *itmh_pph_fem_action_info;
} field_j1_itmh_pph_fg_info_t;

/*
 * }
 */
/**
 * \brief
 *  Callback for J1 ITMH+PPH application which determines whether to run the application
 *
 * \param [in] unit - The unit number.
 *        [out] dynamic_flags - application use
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_j1_itmh_pph_cb(
    int unit,
    int *dynamic_flags);

/**
 * \brief - This function initialize J1 mode ITMH PPH application:
 *          1. Create User-Defined qualifiers for ITMH and ITMH-extension
 *          2. Create field groups to parse the ITMH header / extension and update signals
 *
 *\param [in] unit - Number of hardware unit used.
 *\return
 *   \retval Non-zero (!= BCM_E_NONE) in case of an error
 *   \retval Zero (= BCM_E_NONE) in case of NO ERROR
 *\see
 *   shr_error_e
 */
shr_error_e appl_dnx_field_j1_itmh_pph_init(
    int unit);

/*
 * }
 */
#endif /* APPL_REF_FIELD_J1_ITMH_PPH_INIT_H_INCLUDED */
