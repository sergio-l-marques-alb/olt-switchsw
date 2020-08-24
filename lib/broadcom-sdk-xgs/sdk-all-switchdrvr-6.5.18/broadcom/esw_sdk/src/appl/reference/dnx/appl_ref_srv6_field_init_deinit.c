/** \file appl_ref_srv6_field_init_deinit.c
 * $Id$
 *
 * configuring PMFs for SRV6 processing.
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

#define NOF_QUALIFIERS (2)

#define PRESEL_QUAL_FWD_LAYER_SIZE (5)

#define MIN_NOF_SIDS_PSP (2)

 /*
  * Include files.
  *
  */
/** soc */
#include <soc/schanmsg.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_field.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "appl_ref_srv6_field_init_deinit.h"

/** bcm */
#include <bcm/field.h>
#include <bcm/switch.h>
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

#include <include/bcm/cosq.h>
#include <soc/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_srv6.h>

/*
 * DEFINEs
 */

typedef struct cint_field_srv6_endpoint_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid_qual;
    bcm_field_qualify_t lr_fwd_offset_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_action_t container_void_action;
} field_srv6_endpoint_info_t;

typedef struct cint_field_srv6_egress_usp_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_action_t container_void_action;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_beyond_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid_and_srh_base_qual;
    bcm_field_qualify_t parsing_start_offset_srh_extended_qual;
    bcm_field_qualify_t parsing_index_qual;
    bcm_field_qualify_t bytes_to_remove_header_qual;
    bcm_field_qualify_t bytes_to_remove_fix_qual;
    bcm_field_qualify_t large_parsing_start_offset_efes_valid_qual;
    bcm_field_qualify_t large_parsing_start_offset_valid_and_srh_base_qual;
    bcm_field_qualify_t large_parsing_start_offset_srh_extended_qual;
    bcm_field_qualify_t large_parsing_start_offset_add_64_qual;
    bcm_field_qualify_t large_bytes_to_remove_efes_valid_qual;
    bcm_field_qualify_t large_bytes_to_remove_qual;
    bcm_field_action_t large_parsing_start_offset_action;
    bcm_field_action_t large_bytes_to_remove_action;

    /** Ecologic usage additions */
    bcm_field_qualify_t parsing_start_offset_zero_qual;
    bcm_field_qualify_t large_parsing_start_offset_sids_qual;

} field_srv6_egress_usp_info_t;

typedef struct cint_field_srv6_egress_psp_info_s
{
    bcm_field_group_t ipmf2_fg;
    bcm_field_group_t ipmf3_fg;
    bcm_field_context_t ipmf1_context_id;
    bcm_field_context_t ipmf3_context_id;
    bcm_field_presel_entry_id_t ipmf1_p_id;
    bcm_field_presel_entry_id_t ipmf3_p_id;
    bcm_field_qualify_t udh_qual;
    bcm_field_qualify_t udh_type_qual;
    bcm_field_qualify_t parsing_start_type_valid_qual;
    bcm_field_qualify_t srv6_endpoint_layer_type_qual;
    bcm_field_qualify_t parsing_start_offset_valid_qual;
    bcm_field_qualify_t lr_fwd_offset_qual;
    bcm_field_qualify_t bytes_to_remove_header_qual;
    bcm_field_qualify_t bytes_to_remove_fix_qual;
    bcm_field_qualify_t msb_bits_tm_compensate_size;
    bcm_field_qualify_t parsing_nof_sids_qual;
    bcm_field_qualify_t srh_last_entry_qual;
    bcm_field_action_t container_void_action;
} field_srv6_egress_psp_info_t;

/*
 * MACROs
 */

 /*
  * Global and Static
  */

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_endpoint_info_t cint_field_srv6_endpoint_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_egress_psp_info_t cint_field_srv6_egress_psp_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount,
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_egress_usp_info_t cint_field_srv6_egress_usp_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldActionCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount,
    bcmFieldActionCount, bcmFieldActionCount, bcmFieldQualifyCount, bcmFieldActionCount,
};

typedef enum
{
    EGRESS_USP_ENCAP_NORMAL = 0,
    EGRESS_PSP_ENCAP_NORMAL = 1,
    EGRESS_USP_ENCAP_REDUCED = 2,
    EGRESS_PSP_ENCAP_REDUCED = 3,
    SRV6_NOF_MODES = 4
} dnx_srv6_psp_reduced_enum_t;

/*
 * Functions
 */

/**
 * \brief - create a context in IPMF[1..3] and return its id
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_name - the name of the context
 *   \param [out] ctx_id -  the returned ctx_id that was created
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_context_set(
    int unit,
    bcm_field_stage_t stage,
    char *ctx_name,
    bcm_field_context_t * ctx_id)
{
    bcm_field_context_info_t context_info;
    void *dest_char;
    int rv = 0;

    /** init the structure which holds the parameters of context */
    bcm_field_context_info_t_init(&context_info);

    /** set the user name of the context into the context structure */
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, ctx_name, sizeof(context_info.name));

    rv = bcm_field_context_create(unit, 0, stage, &context_info, ctx_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create, for context %s\n", rv, ctx_name);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - always follows same presels as PMF1-2
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *   \param [in] is_egress -  0 for End-Point node and 1 for Egress node
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_pmf3_presel_set(
    int unit,
    bcm_field_context_t ipmf1_ctx_id,
    bcm_field_context_t pmf3_ctx_id,
    int presel_id)
{
    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;
    int rv = 0;

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF3;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.nof_qualifiers = 1;
    ipmf_presel_entry_data.context_id = pmf3_ctx_id;

     /** set 1st qualifier to both end-point and egress, check if FWD layer is SRv6Beyond */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyContextId;
    ipmf_presel_entry_data.qual_data[0].qual_value = ipmf1_ctx_id;
    ipmf_presel_entry_data.qual_data[0].qual_mask = 0x3f;

    rv = bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set to Presel (%d)\n", rv, presel_id);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - create a Presel in IPMF[1..3] and attach to a IPMF context_id
 *          for SRv6 End-Point and Egress nodes
 *
 *   \param [in] unit - device id
 *   \param [in] stage - the IPMF of interest
 *   \param [in] ctx_id -  the context to which to attach the Presel
 *   \param [in] presel_id -  the allocated number of the Presel
 *   \param [in] is_egress -  0 for End-Point node and 1 for Egress node
 *
 * \return
 *   \retval shr_error_e - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_pmf1_presel_set(
    int unit,
    bcm_field_context_t ctx_id,
    int presel_id,
    uint8 is_egress,
    uint8 is_psp)
{

    bcm_field_presel_entry_id_t ipmf_presel_entry_id;
    bcm_field_presel_entry_data_t ipmf_presel_entry_data;

    int rv = 0;
    int srv6_relative_to_fwd_layer = 0;

    bcm_field_presel_entry_id_info_init(&ipmf_presel_entry_id);
    bcm_field_presel_entry_data_info_init(&ipmf_presel_entry_data);

    /*
     *  when Ecologic is present, (only in Egress USP node) - we increase the FWD layer index in VTT5 to above SRv6,
     *  and need to look at (FWD-1), in order to look at SRv6 layer
     */
    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support) && is_egress && !is_psp)
    {
        srv6_relative_to_fwd_layer = -1;
    }
    else
    {
        srv6_relative_to_fwd_layer = 0;
    }

    ipmf_presel_entry_id.presel_id = presel_id;
    ipmf_presel_entry_id.stage = bcmFieldStageIngressPMF1;
    ipmf_presel_entry_data.entry_valid = TRUE;
    ipmf_presel_entry_data.context_id = ctx_id;
    ipmf_presel_entry_data.nof_qualifiers = NOF_QUALIFIERS;

     /** set 1st qualifier to both end-point and egress, check if FWD layer is SRv6Beyond */
    ipmf_presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    ipmf_presel_entry_data.qual_data[0].qual_arg = srv6_relative_to_fwd_layer;
    ipmf_presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf_presel_entry_data.qual_data[0].qual_mask = (1 << PRESEL_QUAL_FWD_LAYER_SIZE) - 1;

    /** set 2nd qualifier check layer_qualifier[0] of SRv6 if == 1 (SL==0) in egress and otherwise in end-point */
    ipmf_presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    /** if USP, is_psp == 0, and SL will be compared to 0, in case of PSP, is_psp == 1, SL compared to 1*/
    ipmf_presel_entry_data.qual_data[1].qual_arg = srv6_relative_to_fwd_layer;
    /** compare to '1' for egress context and '0' for end-point */
    ipmf_presel_entry_data.qual_data[1].qual_value = (is_egress ? 1 : 0) << is_psp;
    ipmf_presel_entry_data.qual_data[1].qual_mask = 0x1 << is_psp;

    rv = bcm_field_presel_set(unit, 0, &ipmf_presel_entry_id, &ipmf_presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set to Presel (%d)\n", rv, presel_id);
        return rv;
    }

    return BCM_E_NONE;
}

/**
 * \brief - The function configures PMF2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *          -Counts NOF SIDs and passes in container to PMF3
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x3 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by IRE
 *                  2. 8bits of Const Type (value 0xAA)
 *                  3. SRH's Segment Left
 *
 *          -x9 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *                  x1 FEM adding to the Segment Left + 1 to get NOF SIDs
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_psp_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;

    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 128;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_psp_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.udh_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_size_psp_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.udh_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH size\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_last_ent_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.srh_last_entry_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create srv6_psp_last_ent_q\n");
        return rv;
    }

    /*
     * -Create a void action (user defined).
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) (action_info.name), "srv6_psp_cont_void_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_psp_info.container_void_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create container_void_action\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_psp", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 3;
    fg_info.qual_types[0] = cint_field_srv6_egress_psp_info.srh_last_entry_qual;
    fg_info.qual_types[1] = cint_field_srv6_egress_psp_info.udh_qual;
    fg_info.qual_types[2] = cint_field_srv6_egress_psp_info.udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 9;

    fg_info.action_types[0] = cint_field_srv6_egress_psp_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[1] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[4] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size through IPPD_UDH_TYPE_TO_SIZE_MAP.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[5] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[7] = FALSE;
    fg_info.action_types[8] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[8] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_psp_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    /*
     * Configure FEM to increment the last segment by one to get the number of segments
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' (action), of the 32bits 'chunk' in 16bits resolution */
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk' and rest 16bits set to '0'
     */
    for (ii = 0; ii < 8; ii++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }

    /** set the remaining bits up to 24bits of the action to 0 */
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }

    /** increment = <value to add> so we add here number +1 to Last Element */
    fem_action_info.fem_extraction[0].increment = 1;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     * Essentially, this is the identifier of the FEM. Caller calculates this value
     * using BCM_FIELD_ACTION_POSITION with 'array_id' set to '1' or '3' and
     * 'position' set to 0->7
     * -FES is located in (0,*), (2,*) and FEMs (1,*), (3,*). Id is the 2nd argument
     */
    rv = bcm_field_fem_action_add(unit, 0, cint_field_srv6_egress_psp_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                                  &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 32;

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the IRE writes to the metadata, offset 0
     */

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        /** Jump to start of SRH Base */
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerFwd;
        /** Indicates the layer relative to FWD layer */
        attach_info.key_info.qual_info[1].input_arg = 0;
        /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
        attach_info.key_info.qual_info[1].offset = 64;
    }
    else
    {
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[1].offset = 0;
    }

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is 0xAA, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 0xAA;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_psp_info.ipmf2_fg,
                                        cint_field_srv6_egress_psp_info.ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF2
 *          -It copies the 128bits IPv6 DIP into x4 UDH (32bits)
 *
 *          -The FG uses DirectExtraction (no TCAM) to write the qualifiers directly to actions
 *          -x2 Qualifiers:
 *                  1. 128bit Metadata (the Pipe's 144B) LSB, which is the DIP written by IRE
 *                  2. 8bits of Const Type (value 0xAA)
 *
 *          -x8 Actions:
 *                  x4 UDH Data writes of the DIP value split into 32bits
 *                  x4 UDH Base writes, which configures the UDH containers to be 32bits
 *
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_end_point_ipmf2_udh(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;

    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 128;
    sal_strncpy_s((char *) (qual_info.name), "srv6_udh_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.udh_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "qual_udh_size_name", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.udh_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH size\n");
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_UDH", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 2;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_info.udh_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_info.udh_type_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 8;

    /*
     * Write the 128 bits to UDH, from MSB to lsb
     * bcmFieldActionUDHData[0-3] actions are used to fill-in each of the 4 UDH data buffers
     */
    fg_info.action_types[0] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionUDHData1;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionUDHData0;
    fg_info.action_with_valid_bit[3] = FALSE;
    /*
     * Write the UDH type for each 32 bits, indicating the UDH size through IPPD_UDH_TYPE_TO_SIZE_MAP.
     * bcmFieldActionUDHBase[0-3] actions are used to set the sizes of the 4 UDH data buffers - can be
     * 0/8/16/32
     */
    fg_info.action_types[4] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[7] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * take the last 128 bit from the packet, i.e metadata that's passed in the pipe
     * the 128 lsb is IPv6 DIP, that the IRE writes to the metadata, offset 0
     */

    if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
    {
        /** Jump to start of SRH Base */
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
        /** Indicates the layer relative to FWD layer */
        attach_info.key_info.qual_info[0].input_arg = 0;
        /** Take from offset of SRH Base + SRH Base - and it'll take the SID (MSB->LSB) */
        attach_info.key_info.qual_info[0].offset = 64;
    }
    else
    {
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
        /** Jump to Medadata which is located right after the 144B of packet */
        attach_info.key_info.qual_info[0].offset = 0;
    }

    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * We write a constand value, which is 0xAA, which maps to configure the UDH to 32bits
     */
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[1].input_arg = 0xAA;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_info.ipmf2_fg,
                                        cint_field_srv6_endpoint_info.ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF2 for Egress node in Ecologic devices
 *          -it gets the number of SIDs in the SRv6 Header, by adding + 1 to
 *           Last Element field in the SRH header
 *          -if (NOF SIDs < 4) it returns SIDs in Bytes length for bytes_to_remove_fix use in PMF3
 *          -else, it returns [SIDs - 60B] cause 60B will be deleted in layer_start_offset
 *
 *          -x1 Qualifier:
 *                  1. 8bits to store the Last Element field from SRH header
 *          -x1 Action:
 *                  1. Action to set to pass NOF SIDs in a container for PMF3 use
 *
 *   Implementation:
 *          - Qualifier = Last Element (8bits) --> (DirectExctract) = Action (VoidType)
 *          - FEM
 *               - construct key from the 8bits of Action (Last Element)
 *               - add + 1 and put this result in container to be used by PMF3
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_usp_ipmf2_ecologic(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_srh_last_ent_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.srh_last_entry_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create srh_last_entry_qual\n");
        return rv;
    }

    /*
     * -Create a void action (user defined).
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) (action_info.name), "srv6_container_void_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_usp_info.container_void_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create container_void_action\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_last_entry", sizeof(fg_info.name));

    /*
     * Set quals
     */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.srh_last_entry_qual;

    /*
     * Set actions
     */
    fg_info.nof_actions = 1;

    fg_info.action_types[0] = cint_field_srv6_egress_usp_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    /*
     * Configure FEM to increment the last segment by one to get the number of segments
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' of the 32bits 'chunk'*/
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < 3; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    for (ii = 3; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 1;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;
    fem_action_info.fem_extraction[1].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk' and rest 16bits set to '0'
     */
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }
    for (ii = 0; ii < 3; ii++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[0].output_bit[ii + 4].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii + 4].offset = ii;
    }
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit
         */
        fem_action_info.fem_extraction[1].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[1].output_bit[ii].forced_value = 0;
    }
    for (ii = 0; ii < 3; ii++)
    {
        /*
         * Use a bit from the input key
         */
        fem_action_info.fem_extraction[1].output_bit[ii + 4].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[1].output_bit[ii + 4].offset = ii;
    }

    fem_action_info.fem_extraction[1].output_bit[4 + 5].forced_value = 1;

    /** increment = <value to add> so we add here number +1 to Last Element */
    fem_action_info.fem_extraction[0].increment = 16;
    fem_action_info.fem_extraction[1].increment = 16 + 128 - 60;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     * Essentially, this is the identifier of the FEM. Caller calculates this value
     * using BCM_FIELD_ACTION_POSITION with 'array_id' set to '1' or '3' and
     * 'position' set to 0->7
     */
    rv = bcm_field_fem_action_add(unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                                  &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     * -In case of J2P with the Ecologic where the FWD layer after VTT5 is IPv4,
     *  take this field from FWD-1 layer, otherwise the FWD layer is SRv6
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].offset = 32;
    attach_info.key_info.qual_info[0].input_arg = -1;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg,
                                        cint_field_srv6_egress_usp_info.ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF2 for Egress node
 *          -it gets the number of SIDs in the SRv6 Header, by adding + 1 to
 *           Last Element field in the SRH header
 *
 *          -x1 Qualifier:
 *                  1. 8bits to store the Last Element field from SRH header
 *          -x1 Action:
 *                  1. Action to set to pass NOF SIDs in a container for PMF3 use
 *
 *   Implementation:
 *          - Qualifier = Last Element (8bits) --> (DirectExctract) = Action (VoidType)
 *          - FEM
 *               - construct key from the 8bits of Action (Last Element)
 *               - add + 1 and put this result in container to be used by PMF3
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_usp_ipmf2(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    bcm_field_fem_action_info_t fem_action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
    sal_strncpy_s((char *) (qual_info.name), "srv6_srh_last_ent_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.srh_last_entry_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create srh_last_entry_qual\n");
        return rv;
    }

    /*
     * -Create a void action (user defined).
     *  Void means is that no HW is involved, performed by SW only
     * -The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    sal_strncpy_s((char *) (action_info.name), "srv6_container_void_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_usp_info.container_void_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create container_void_action\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_last_entry", sizeof(fg_info.name));

    /*
     * Set quals 
     */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.srh_last_entry_qual;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 1;

    fg_info.action_types[0] = cint_field_srv6_egress_usp_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    /*
     * Configure FEM to increment the last segment by one to get the number of segments
     */
    bcm_field_fem_action_info_t_init(&fem_action_info);
    /** this is the offset, on input 'result' of the 32bits 'chunk'*/
    fem_action_info.fem_input.input_offset = 0;
    /** this is MS bit on the 32bits 'chunk' of the 4 bits that are used as 'condition'*/
    fem_action_info.condition_msb = 3;
    /** all 16 possible conditions initialized to be activated, and choose action 0 (of 0..3 possible)*/
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }

    /*
     * -fem_extraction = array of 4 'action' descriptors, we use only [0]
     *
     * -This action will be used as Container in IPMF2, to parse the action buffer,
     *  when performing cascading between IPMF2 and IPMF3
     */
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;

    /*
     * - output_bit = array of 24 'bit descriptor' indication of how to construct action value.
     *   for each bit, indicate whether to take it from the 32bits 'chunk' (bcmFieldFemExtractionOutputSourceTypeKey)
     *   or from value written on this input (bcmFieldFemExtractionOutputSourceTypeForce)
     *
     * - we read first 8bits bit by bit from 'chunk' and rest 16bits set to '0'
     */
    for (ii = 0; ii < 8; ii++)
    {
        /*
         * Use a bit from the input key 
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }

    /** set the remaining bits up to 24bits of the action to 0 */
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        /*
         * Use a constant value for this bit 
         */
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }

    /** increment = <value to add> so we add here number +1 to Last Element */
    fem_action_info.fem_extraction[0].increment = 1;

    /*
     * BCM_FIELD_ACTION_POSITION() - Encoded position of the FEM to add to this FG.
     * Essentially, this is the identifier of the FEM. Caller calculates this value
     * using BCM_FIELD_ACTION_POSITION with 'array_id' set to '1' or '3' and
     * 'position' set to 0->7
     */
    rv = bcm_field_fem_action_add(unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(3, 2),
                                  &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /*
     * Take the "last entry" or "last segment" field from the SRH header,
     * i.e from the FWD layer which is SRv6, and this field is in 32bits offset
     * -In case of J2P with the Ecologic where the FWD layer after VTT5 is IPv4,
     *  take this field from FWD-1 layer, otherwise the FWD layer is SRv6
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].offset = 32;
    attach_info.key_info.qual_info[0].input_arg = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf2_fg,
                                        cint_field_srv6_egress_usp_info.ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF3 for End-Point
 *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
 *          -sets the Parsing-Start-Offset to keep the IPv6 header, by deleting only the ETH
 *
 *          -x4 Qualifiers:
 *                  1. 1bit Qual Valid for Parsing-Start-Type Qual
 *                  2. 5bits for Parsing-Start-Type of const value 20 - which is 'SRv6_endpoint'
 *                  3. 1bit Qual Valid for Parsing-Start-Offset Qual (bytes to skip in Egress)
 *                  3. 8bits Qual for Parsing-Start-Offset
 *          -x2 Actions:
 *                  1. Action to set the Parsing-Start-Type system header signal
 *                  2. Action to set the Parsing-Start-Offset system header signal
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_endpoint_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    int key_gen_var_offset;
    int lr_offset_offset;

    bcm_field_qualifier_info_get_t qual_info_get;

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x34;
    param_info.param_arg = 0;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3, cint_field_srv6_endpoint_info.ipmf3_context_id,
                                     &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    /**We build the number 20 which is hex 0x14 for parsing start type.*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_srep_start_type_val_q", sizeof(qual_info.name));
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_endpoint_layer_type_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_endpoint_layer_type_qual\n", rv);
        return rv;
    }
    /**Need to copy the offset of forwarding layer to parsing offset action*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_start_offset_val_q", sizeof(qual_info.name));
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create lr_fwd_offset_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_lr_fwd_offset_q", sizeof(qual_info.name));
    /**Qualifier for de on fwd offset*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.lr_fwd_offset_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create lr_fwd_offset_qual\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_info.lr_fwd_offset_qual;

    /*
     * Set actions - Width of the signals that the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Parsing-Start-Offset action
     * Q[2] - action inner valid bit
     * Q[3] - value is set to - IPv6 layer offset, meaning to cut the ETH layer in Egress
     */
    fg_info.action_types[1] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[1] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_endpoint_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf3_fg\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    key_gen_var_offset = qual_info_get.offset;
    /**Get the offset (within the layer record) of the layer offset (within the header).*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    lr_offset_offset = qual_info_get.offset;

    /**Get the offset (within the layer record) of the layer record qualifier (within the header).*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF3,
                                      &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 5;

    /*
     * take the layer_offset of IPv6 (SRv6_Beyond, which is the FWD layer -1),
     * Cutting in Egress the ETH, which means ETH is passed and not cut in ITPP
     */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[3].input_arg = -1;
    /**This is the offset of protocol type inside layer records*/
    attach_info.key_info.qual_info[3].offset = lr_offset_offset;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_endpoint_info.ipmf3_fg,
                                        cint_field_srv6_endpoint_info.ipmf3_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf3\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF3 for Egress PSP
 *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
 *           cause PSP is simply a Qualifier in SRv6_endpoint Layer in Egress
 *          -sets the Bytes-to-Remove-Fix to keep the IPv6 header, but delete the ETH in ITPP
 *          -updates signal nwk_header_append_size with IPv6 + SRv6 length to compensate
 *           the Egress deletion of whole IPv6 + Srv6
 *          -sets the Parsing-Start-Offset to 0 to start from IPv6
 *
 *          -x4 Qualifiers:
 *                  1. 1bit Qual Valid for Parsing-Start-Type Qual
 *                  2. 5bits for Parsing-Start-Type of const value 20 - which is 'SRv6_endpoint'
 *                  3. 1bit Qual Valid for Parsing-Start-Offset Qual (bytes to skip in Egress)
 *                  3. 8bits Qual for Parsing-Start-Offset
 *          -x2 Actions:
 *                  1. Action to set the Parsing-Start-Type system header signal
 *                  2. Action to set the Parsing-Start-Offset system header signal
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_psp_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    int key_gen_var_offset;
    int lr_offset_offset;
    int container_offset;

    bcm_field_qualifier_info_get_t qual_info_get;

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit) to be used by
     * the qualifiers later - 0x34 => 1(1|0100 = 20)
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context- Key_Gen_Val */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x34; /** Contains 5 bits with value 20 (LayerTypeSrv6Endpoint) and 1 bit with value 1 (valid bit)*/

    param_info.param_arg = 0;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                     cint_field_srv6_egress_psp_info.ipmf3_context_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    /*
     * -We build the number 20 which is hex 0x14 for parsing start type
     * -Parsing-Start-Type Qualifier for valid bit
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_strtype_val_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_psp_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    /**Parsing-Start-Type Qualifier value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_layer_type_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_psp_info.srv6_endpoint_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_endpoint_layer_type_qual\n", rv);
        return rv;
    }

    /*
     * Bytes-to-Remove Qualifier 1bit MSB - set to '0'.
     * We write layer_fwd_offset of IPv6 which is 8bit qualifier below into action
     * of bytes_to_remove_header (2b) which will be '0' and to bytes_to_remove_header (7b) which
     * will be the layer_fwd_offset (8b), so we're left with only 1MSB for the Bytes-to-Remove-Header
     */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_B_to_rm_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.bytes_to_remove_header_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_header_qual\n", rv);
        return rv;
    }

    /**Qualifier for of fwd offset on FWD-1, to remove EHT in ITPP */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_bts_rmv_fix_q", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.bytes_to_remove_fix_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_psp_bts_rmv_fix_q\n", rv);
        return rv;
    }

    /**NOF SIDs Qualifier - to be used to choose the right TM compensate size entry per number of SIDs */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_nof_sids_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.parsing_nof_sids_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_nof_sids_q\n", rv);
        return rv;
    }

    /** 5 MSB bit for TM compensation size (which is 8bits) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_msb_bit_tm_compensation_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.msb_bits_tm_compensate_size);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_msb_bit_tm_compensation_q\n", rv);
        return rv;
    }

    /*
     * Set Parsting-Start-Offset to 0 to IPv6
     */

    /** Start-Offset valid bit */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_st_offst_vld_q", sizeof(qual_info.name));
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_psp_info.parsing_start_offset_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_psp_st_offst_vld_q\n", rv);
        return rv;
    }
    /** Start-Offset value */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_psp_st_offst_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_psp_info.lr_fwd_offset_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_psp_st_offst_q\n", rv);
        return rv;
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_psp_ipmf3", sizeof(fg_info.name));

    /*
     * Set quals
     * -We pass 1bit of Validate Bit, even though action_with_valid_bit = False,
     * because actions may have 2 valid bits - the above is external - which may be used
     * by user, to differentiate between states.
     * - But, some actions have inner valid bits, as part of FES machines that need be '1'
     * to be active.
     */
    fg_info.nof_quals = 8;
    fg_info.qual_types[0] = cint_field_srv6_egress_psp_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_egress_psp_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_egress_psp_info.bytes_to_remove_header_qual;
    fg_info.qual_types[3] = cint_field_srv6_egress_psp_info.bytes_to_remove_fix_qual;
    fg_info.qual_types[4] = cint_field_srv6_egress_psp_info.parsing_nof_sids_qual;
    fg_info.qual_types[5] = cint_field_srv6_egress_psp_info.msb_bits_tm_compensate_size;
    fg_info.qual_types[6] = cint_field_srv6_egress_psp_info.parsing_start_offset_valid_qual;
    fg_info.qual_types[7] = cint_field_srv6_egress_psp_info.lr_fwd_offset_qual;

    /*
     * Set actions - Width of the signals that the actions write to,
     *               dictate how many bits to take from the Qualifiers
     */

    /*
     * Parsing-Start-Type action
     * Q[0] - action inner valid bit
     * Q[1] - the actual value (20)
     */
    fg_info.nof_actions = 4;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Bytes_to_Remove
     * Q[2] - bytes_to_remove_header (2bits)
     * Q[3] - bytes_to_remove_fix (7bits)
     */
    fg_info.action_types[1] = bcmFieldActionStartPacketStrip;
    fg_info.action_with_valid_bit[1] = FALSE;

    /*
     * TM Compensation Size
     * Q[4] - nof sids - represents the TM entry num  (3bits)
     * Q[5] - msb bits compensation size - '0' (5bits)
     */
    fg_info.action_types[2] = bcmFieldActionAppendPointerCompensation;
    fg_info.action_with_valid_bit[2] = FALSE;

    /*
     * Parsing-Start-Offset action
     * Q[6] - action inner valid bit
     * Q[7] - value is set to - IPv6 layer, which means to set it to 0
     */
    fg_info.action_types[3] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[3] = FALSE;

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_psp_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add Egress PSP ipmf3_fg\n", rv);
        return rv;
    }

    /*
     * copy the information of the FG into the attach structure
     * & adding to it the "input_type" and "offset" - from where
     * to take the qualifier information
     */
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    /**Get the offset of key_gen_var on the PBUS.*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset (within the layer record) of the layer offset (within the header).*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    lr_offset_offset = qual_info_get.offset;

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    container_offset = qual_info_get.offset;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Qual's valid bit*/
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 5 bits, Qual's size which is 20*/
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /** 2 MSB bit of Bytes to Remove - take from the Key Gen Value */
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 0;

    /** 7bits - Bytes to Remove Fix - set to constant '0' */
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[3].input_arg = -1;
    /**This is the offset of protocol type inside layer records*/
    attach_info.key_info.qual_info[3].offset = lr_offset_offset;

    /** 3bits LSB bits of NWK_HEADER_APPEND_SIZE - taking NOF SIDs from IPMF2 container cascading */
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = container_offset;

    /** 5bits MSB bits of the action which writes to NWK_HEADER_APPEND_SIZE pointer, which is 8bits */
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[5].input_arg = 0;

    /** take from MetaData, where the Key_Gen_Val of 0x34 is stored, 1 bit, Offset valid bit*/
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[6].offset = key_gen_var_offset + 5;

    /** set the qualifier value of the Parsing-Start-Offset to 0 */
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[7].input_arg = 0;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_psp_info.ipmf3_fg,
                                        cint_field_srv6_egress_psp_info.ipmf3_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf3\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF3 for Egress USP in device with Ecologic
 *          -it sets the Parsing-Start-Type to 'SRv6_Beyond' for parser context (in case of non - ECO-Logic devices)
 *          -updates the signals Bytes_to_Remove and Parsing_Start_Offset in following way,
 *              (if NOF SIDs <= 7)
 *                 * Cut in ITPP ETH + IPv6 (set Bytes_to_Remove=2, which means cut up to FWD layer which is SRv6)
 *                 * Cut in Egress SRH + SIDs (Parsing_Start_Offset to SRH + 128b * NOF_SIDs)
  *              (if NOF SIDs > 7)
 *                 * Cut in ITPP ETH + IPv6 + 64B (set Bytes_to_Remove=2, Bytes_to_Remove_Fix = 64)
 *                 * Cut in Egress SRH + SIDs -64B (Parsing_Start_Offset to SRH + 128b * NOF_SIDs - 64B)
 *          -sets Egress_Parsing_Index to SRv6 layer
 *
 *          -x14 Qualifiers, which accommodate below actions
 *
 *          -x6 Actions:
 *                  1. set Parsing-Start-Type
 *                  2. set Parsing_Start_Offset (when NOF SIDs <= 7)
 *                  3. set Egress_Parsing_Index
 *                  4. set Bytes_to_Remove (when NOF SIDs <= 7)
 *                  5. set Parsing_Start_Offset (when NOF SIDs > 7)
 *                  6. set Bytes_to_Remove (when NOF SIDs > 7)
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_usp_ipmf3_ecologic(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    int container_offset;
    bcm_field_qualifier_info_get_t qual_info_get;

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Alternatively, const qualifiers or masks can be used.
     */

    /**Parsing-Start-Offset from FWD layer - Qualifier with 4bits of SRH size header (8Bytes) + 1bit action's inner valid bit  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_zero_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.parsing_start_offset_zero_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }

    /** EFES valid bit if NOD SIDs > 3 - then Large-Parsing-Start-Offset will be 60B */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_efes_val_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_efes_valid_qual\n", rv);
        return rv;
    }

    /**Large-Parsing-Start-Offset from FWD layer - Qualifier with 4bits of SRH size header (8Bytes) + 1bit action's inner valid bit  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_lrg_prs_off_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_sids_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }

    /** Bytes-to-Remove Qualifier - ITPP cutting (ETH + IPv6 + SRH) up to FWD layer (above SRv6) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_B_to_rm_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.bytes_to_remove_header_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_header_qual\n", rv);
        return rv;
    }

    /** Bytes-to-Remove Qualifier Fix */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_B_to_rm_fix_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.bytes_to_remove_fix_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_fix_qual\n", rv);
        return rv;
    }

    /*
     * New user action - (if NOF SIDs >3) that will override layer_parsing_start_offset of 0 in this case with 60B
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartOffset;
    action_info.size = 8;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) (action_info.name), "srv6_lar_pars_offset_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info,
                                 &cint_field_srv6_egress_usp_info.large_parsing_start_offset_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create large_parsing_start_offset_action\n", rv);
        return rv;
    }

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    container_offset = qual_info_get.offset;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    /*
     * Set quals
     */
    fg_info.nof_quals = 5;

    /**  Parsing-Start-Offset inner valid bit + default is 0*/
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.parsing_start_offset_zero_qual;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[0].input_arg = 1;

    /**  Large-Parsing-Start-Offset - EFES  valid bit  - in case NOF SIDs >3 and not all can be removed in ITPP */
    fg_info.qual_types[1] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_efes_valid_qual;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = container_offset + 9;

    /**  Large-Parsing-Start-Offset inner valid bit (60B) to remove in Egress - in case NOF SIDs >3 and not all can be removed in ITPP*/
    fg_info.qual_types[2] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_sids_qual;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[2].input_arg = 1 + (60 << 1);

    /** Bytes-to-Remove Qualifier - ITPP cutting (ETH + IPv6 + SRH) up to FWD layer (above SRv6) */
    fg_info.qual_types[3] = cint_field_srv6_egress_usp_info.bytes_to_remove_header_qual;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[3].input_arg = 2;

    /** 7bits - Bytes to Remove Fix - is the NOF sids  - will be [NOF SIDs] if (NOF SIDs < 3), or [SIDs - 60B] in case (NOF SIDs > 3) */
    fg_info.qual_types[4] = cint_field_srv6_egress_usp_info.bytes_to_remove_fix_qual;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = container_offset;

    /*
     * Set actions
     */
    fg_info.nof_actions = 3;

    /*
     * Action for Parsing_Start_Offset - from Qualifier 2 -Q3(3b) - NOF SIDs (7th bit - 128b x
     * NOF_SIDs) -Q2(4b+1b) - original Pasrsing_Start_Offset (SRH size without SIDs) + 1b inner action validate bit
     */
    fg_info.action_types[0] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[0] = FALSE;

    fg_info.action_types[1] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_action;
    /** TRUE becuase we use EFES valid bit to override above action with this user action */
    fg_info.action_with_valid_bit[1] = TRUE;

    /** Bytes_to_Remove  */
    fg_info.action_types[2] = bcmFieldActionStartPacketStrip;
    fg_info.action_with_valid_bit[2] = FALSE;

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf3_fg\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        /*
         * Ensure that later actions have higher priorities than earlier actions.
         */
        attach_info.payload_info.action_info[ii].priority = BCM_FIELD_ACTION_PRIORITY(0, ii);
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf3_fg,
                                        cint_field_srv6_egress_usp_info.ipmf3_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf3\n", rv);
        return rv;
    }

    return 0;
}

/**
 * \brief - The function configures PMF3 for Egress USP
 *          -it sets the Parsing-Start-Type to 'SRv6_Beyond' for parser context (in case of non - ECO-Logic devices)
 *          -updates the signals Bytes_to_Remove and Parsing_Start_Offset in following way,
 *              (if NOF SIDs <= 7)
 *                 * Cut in ITPP ETH + IPv6 (set Bytes_to_Remove=2, which means cut up to FWD layer which is SRv6)
 *                 * Cut in Egress SRH + SIDs (Parsing_Start_Offset to SRH + 128b * NOF_SIDs)
  *              (if NOF SIDs > 7)
 *                 * Cut in ITPP ETH + IPv6 + 64B (set Bytes_to_Remove=2, Bytes_to_Remove_Fix = 64)
 *                 * Cut in Egress SRH + SIDs -64B (Parsing_Start_Offset to SRH + 128b * NOF_SIDs - 64B)
 *          -sets Egress_Parsing_Index to SRv6 layer
 *
 *          -x14 Qualifiers, which accommodate below actions
 *
 *          -x6 Actions:
 *                  1. set Parsing-Start-Type
 *                  2. set Parsing_Start_Offset (when NOF SIDs <= 7)
 *                  3. set Egress_Parsing_Index
 *                  4. set Bytes_to_Remove (when NOF SIDs <= 7)
 *                  5. set Parsing_Start_Offset (when NOF SIDs > 7)
 *                  6. set Bytes_to_Remove (when NOF SIDs > 7)
 *
 *   \param [in] unit - device id
 *
 * \return
 *   \retval int - negative in case of an error, zero in case all ok.
 */
static int
field_srv6_config_egress_usp_ipmf3_de(
    int unit)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_context_param_info_t param_info;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_action_info_t action_info;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0;
    int key_gen_var_offset;
    int container_offset;
    bcm_field_qualifier_info_get_t qual_info_get;

    /** remove in ITPP up to the Ingress FWD layer (SRv6), i.e remove ETH + IPv6 */
    uint32 bytes_to_remove_header = 2;
    /** In case NOF SIDs > 7, 64B addition to Bytes_to_Remove in ITPP */
    uint32 large_bytes_to_remove_fix = 64;
    /** from which Layer to start Parsing in Egress (AnyoSRv6oIPv6oETH) - so 2 is SRv6 */
    uint32 egress_parsing_index = 2;

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Alternatively, const qualifiers or masks can be used.
     */

    /** Set a constant value that can be used as qualifier for lookups performed by the context. */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val = 0x15 + /** 5 bits with value 21 (LayerTypeSrv6Beyond) */
        (1 << 5) +                /** 1 bit with value 1 to be used as qualifier valid bits for actions (FES inner valid bit) */
        (8 << 6) +                /** 4 bits contain the size of an SRH header without the segments (8Bytes) for parsing_start_offset */
        (bytes_to_remove_header << 10) +    /** 2 bits bytes_to_remove_header */
        (large_bytes_to_remove_fix << 12) + /** 7 bits contain bytes_to_remove_fix for the case of more than 7 segments */
        (egress_parsing_index << 19);       /** 2 bits egress_parsing_index */
    param_info.param_arg = 0;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3,
                                     cint_field_srv6_egress_usp_info.ipmf3_context_id, &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    /**Parsing-Start-Type - Qualifier with 1b valid bit to be used in Parsing-Start-Type action's inner FES valid bit for */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_prs_start_type_val_q", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_type_valid_qual\n", rv);
        return rv;
    }

    /**Parsing-Start-Type - Qualifier with 5bits to be 0x15 = 21 (LayerTypeSrv6Beyond) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    sal_strncpy_s((char *) (qual_info.name), "srv6_beyond_layer_type_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.srv6_beyond_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_beyond_layer_type_qual\n", rv);
        return rv;
    }

    /**Parsing-Start-Offset from FWD layer - Qualifier with 4bits of SRH size header (8Bytes) + 1bit action's inner valid bit  */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 4;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_val_srh_base_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }

    /**NOF SIDs Qualifier - Indication if we're above 7 SIDs or not */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_srh_extended_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }

    /** Qualifier Egress-Parsing-Index (FWD Layer Idx) - start parsing from SRv6 Layer (skip ETH, IPv6)*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_index_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.parsing_index_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_index_qual\n", rv);
        return rv;
    }

    /** Bytes-to-Remove Qualifier - ITPP cutting (ETH + IPv6 + (64B if SIDs > 7)) */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_B_to_rm_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.bytes_to_remove_header_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_header_qual\n", rv);
        return rv;
    }

    /** Bytes-to-Remove Qualifier Fix */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_B_to_rm_fix_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.bytes_to_remove_fix_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_fix_qual\n", rv);
        return rv;
    }

    /*
     * Qualifiers and actions for the case where the number of segments is larger than 7 (but smaller than 12)
     */

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_prs_off_efes_val_q", sizeof(qual_info.name));
    /**Both EFES valid bit according to the fourth bit of the number of entries..*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_efes_valid_qual\n", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 4;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_prs_off_val_srh_base_q", sizeof(qual_info.name));
    /**The size in bytes of the first part of the SRH_plus valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }

    /** (Number of Segments Modulo 4) Qualifier */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_prs_off_srh_extended_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }

    /**Add 64 bits to compensate for the lost 128 (64 more will be added using bytes_to_remove_fix */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_prs_offs_add_64_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_parsing_start_offset_add_64_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_add_64_qual\n", rv);
        return rv;
    }

    /**Qualifier for EFES valid bit rewriting bytes to remove if there are more than 7 segments*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_large_bytes_to_rem_efes_val_q", sizeof(qual_info.name));
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_usp_info.large_bytes_to_remove_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_bytes_to_remove_efes_valid_qual\n", rv);
        return rv;
    }

    /**Qualifier for rewriting Bytes To Remove header if there are more than 7 segments*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2 + 7;
    sal_strncpy_s((char *) (qual_info.name), "srv6_egr_large_B_to_rem_q", sizeof(qual_info.name));

    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_usp_info.large_bytes_to_remove_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_qual\n", rv);
        return rv;
    }

    /*
     * New user action - (if NOF SIDs >7) Indicate from which offset Egress Parser should start parsing
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartOffset;
    action_info.size = 8;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) (action_info.name), "srv6_lar_pars_offset_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info,
                                 &cint_field_srv6_egress_usp_info.large_parsing_start_offset_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create large_parsing_start_offset_action\n", rv);
        return rv;
    }

    /*
     * New user action - (if NOF SIDs >7) Bytes to Remove action
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionStartPacketStrip;
    action_info.size = 9;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    sal_strncpy_s((char *) (action_info.name), "srv6_lar_B_to_remove_a", sizeof(action_info.name));
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_usp_info.large_bytes_to_remove_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create large_bytes_to_remove_action\n", rv);
        return rv;
    }

    /**Get the offset of key_gen_var on the PBUS - get the Key Gen Value */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyKeyGenVar, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    key_gen_var_offset = qual_info_get.offset;

    /**Get the offset (within the layer record) of the layer offset (within the header).*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordOffset, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    /**Get the offset (within the layer record) of the layer record qualifier (within the header).*/
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyLayerRecordQualifier, bcmFieldStageIngressPMF3,
                                      &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container, where we keep the calculated NOF SIDS */
    rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyContainer, bcmFieldStageIngressPMF3, &qual_info_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_info_get\n", rv);
        return rv;
    }
    container_offset = qual_info_get.offset;

    bcm_field_group_info_t_init(&fg_info);
    bcm_field_group_attach_info_t_init(&attach_info);

    /*
     * Set quals 
     */
    fg_info.nof_quals = 13;

    /** 1bit Parsing-Start-Type valid bit - take from the Key Gen Value */
    fg_info.qual_types[0] = cint_field_srv6_egress_usp_info.parsing_start_type_valid_qual;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    /** 5bits Parsing-Start-Type value - take the  from the Key Gen Value we created be SRv6 (21)*/
    fg_info.qual_types[1] = cint_field_srv6_egress_usp_info.srv6_beyond_layer_type_qual;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    /**  Parsing-Start-Offset (1bit valid bit + 4bit value) - take from the Key Gen Value*/
    fg_info.qual_types[2] = cint_field_srv6_egress_usp_info.parsing_start_offset_valid_and_srh_base_qual;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 5;

    /** 3bits - taking NOF SIDs from IPMF2 container cascading */
    fg_info.qual_types[3] = cint_field_srv6_egress_usp_info.parsing_start_offset_srh_extended_qual;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = container_offset;

    /** 3bits - Egress Parsing Index (FWD Layer Idx) - take from the Key Gen Value */
    fg_info.qual_types[4] = cint_field_srv6_egress_usp_info.parsing_index_qual;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].offset = key_gen_var_offset + 19;

    /** 2bits - Bytes to Remove - take from the Key Gen Value */
    fg_info.qual_types[5] = cint_field_srv6_egress_usp_info.bytes_to_remove_header_qual;
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[5].offset = key_gen_var_offset + 10;

    /** 7bits - Bytes to Remove Fix - set to constant '0' - the addition is '0' in NOF SIDs <= 7 */
    fg_info.qual_types[6] = cint_field_srv6_egress_usp_info.bytes_to_remove_fix_qual;
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[6].input_arg = 0;

    /*
     * Below qualifiers settings for SIDs > 7
     * -Q[8-12] Large Parsing_Start_Offset construction
     * -Q[13-14] Larger Bytes_to_Remove construction (+64B)
     */

    /** 1bit - check that NOF SIDs > 7, if there's 4th bit set to '1' in NOF SIDs IPMF2 container */
    fg_info.qual_types[7] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_efes_valid_qual;
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[7].offset = container_offset + 3;

    /** Parsing-Start-Offset (1bit valid bit + 4bit value) - take from the Key Gen Value  */
    fg_info.qual_types[8] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_valid_and_srh_base_qual;
    attach_info.key_info.qual_info[8].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[8].offset = key_gen_var_offset + 5;

    /** 2bits - (Segments modulo 4) - taking NOF SIDs from IPMF2 container cascading */
    fg_info.qual_types[9] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_srh_extended_qual;
    attach_info.key_info.qual_info[9].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[9].offset = container_offset;

    /** 1bit - add 64bits to compensate for the lost 128bits (64 more will be added using bytes_to_remove_fix) */
    fg_info.qual_types[10] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_add_64_qual;
    attach_info.key_info.qual_info[10].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[10].offset = key_gen_var_offset + 5;

    /** 1bit - Qualifier for EFES valid bit rewriting bytes to remove if NOF SIDS > 7 (4th bit of NOF SIDs is set) */
    fg_info.qual_types[11] = cint_field_srv6_egress_usp_info.large_bytes_to_remove_efes_valid_qual;
    attach_info.key_info.qual_info[11].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[11].offset = container_offset + 3;

    /** 2+7bits - 2bits for Bytes_to_Remove + 7bits contain Bytes_to_Remove_Fix for the case of more than 7 segments*/
    fg_info.qual_types[12] = cint_field_srv6_egress_usp_info.large_bytes_to_remove_qual;
    attach_info.key_info.qual_info[12].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[12].offset = key_gen_var_offset + 10;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 6;

    /** Parsing_Start_Type - from Qualifiers 0,1 */
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;

    /*
     * Action for Parsing_Start_Offset - from Qualifier 2 -Q3(3b) - NOF SIDs (7th bit - 128b x
     * NOF_SIDs) -Q2(4b+1b) - original Pasrsing_Start_Offset (SRH size without SIDs) + 1b inner action validate bit 
     */
    fg_info.action_types[1] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[1] = FALSE;

    /** Egress_Parsing_Index - from Qualifiers 5 */
    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;

    /** Bytes_to_Remove - */
    fg_info.action_types[3] = bcmFieldActionStartPacketStrip;
    fg_info.action_with_valid_bit[3] = FALSE;

    /*
     * Using valid-bits only for case of (NOF SIDs > 7)
     */

    /*
     * Action gets Parsing_Start_Offset
     * -Q8(1b) - valid - if (NOF SIDs > 7)
     * -Q12(1b) - '0' - 8th bit
     * -Q11(1b) - add 64bits to compensate for the lost 128bits
     * -Q10(1b) - (NOF SIDs modulo 4)
     * -Q9(1+4b) - original Pasrsing_Start_Offset - SRH size without SIDs
     */
    fg_info.action_types[4] = cint_field_srv6_egress_usp_info.large_parsing_start_offset_action;
    fg_info.action_with_valid_bit[4] = TRUE;

    /** Action gets Bytes_to_Remove(2b) + 64Bytes (7b) - from Qual (13 - valid) + Qual 14 (value)*/
    fg_info.action_types[5] = cint_field_srv6_egress_usp_info.large_bytes_to_remove_action;
    fg_info.action_with_valid_bit[5] = TRUE;

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, 0, &fg_info, &cint_field_srv6_egress_usp_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf3_fg\n", rv);
        return rv;
    }

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
        /*
         * Ensure that later actions have higher priorities than earlier actions.
         */
        attach_info.payload_info.action_info[ii].priority = BCM_FIELD_ACTION_PRIORITY(0, ii);
    }

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_usp_info.ipmf3_fg,
                                        cint_field_srv6_egress_usp_info.ipmf3_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf3\n", rv);
        return rv;
    }

    return 0;
}

shr_error_e
appl_dnx_field_srv6_init(
    int unit)
{

    int appl_psp_red_param;
    uint8 is_psp;

    SHR_FUNC_INIT_VARS(unit);

    /** get the psp and reduced mode parameter from the config file */
    appl_psp_red_param = soc_property_suffix_num_get_only_suffix(unit, -1, spn_APPL_PARAM, "psp_reduced_mode", 0);

    switch (appl_psp_red_param)
    {
        case EGRESS_USP_ENCAP_NORMAL:
        {
            /** Egress node USP, Normal encap - no need to set any SRv6 mode */
            is_psp = 0;
            break;
        }
        case EGRESS_PSP_ENCAP_NORMAL:
        {
            /** Egress node PSP, Normal encap */
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6EgressPSPEnable, 1));
            is_psp = 1;
            break;
        }
        case EGRESS_USP_ENCAP_REDUCED:
        {
            /** Egress node USP, Reduced encap */
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6ReducedModeEnable, 1));
            is_psp = 0;
            break;
        }
        case EGRESS_PSP_ENCAP_REDUCED:
        {
            /** Egress node PSP, Reduced encap */
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6EgressPSPEnable, 1));
            SHR_IF_ERR_EXIT(bcm_switch_control_set(unit, bcmSwitchSRV6ReducedModeEnable, 1));
            is_psp = 1;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "SRv6 Egress/Reduced mode (%d) not supported - need to be [0-3]!",
                         appl_psp_red_param);
        }

    }

    /*
     * End-Point (PSP/USP) - PMF1-2 configurations
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL >= 1/2 (case of USP/PSP)
     * -Actions:
     *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USP/PSP, Egress PSP)
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL >= 2 (LyrQ[1]==0)
     * -In case of End-Point USP, will be performed cause SL >= 1 (LyrQ[0]==0)
     */

    /** End-Point - create the context in PMF1 and name it */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF1, "SRv6_srep_udh_pmf1",
                     &cint_field_srv6_endpoint_info.ipmf1_context_id));

    /** End-Point - create the Presel and attach it to the above context */
    SHR_IF_ERR_EXIT(field_srv6_pmf1_presel_set(unit, cint_field_srv6_endpoint_info.ipmf1_context_id, 11, 0, is_psp));

    /** End-Point- FG Action: Save IPv6 DIP in x4 UDH Containers in PMF2 */
    SHR_IF_ERR_EXIT(field_srv6_config_end_point_ipmf2_udh(unit));

    /*
     * PMF3 configurations - End-Point (PSP/USP)
     * -Presel Qualifiers: FWD layer is SRv6 + expected SL > 1/2 (case of USP/PSP)
     * -Actions:
     *          -sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
     *          -sets the Parsing-Start-Offset to keep the IPv6 header, by deleting only the ETH in Egress
     *
     * Will be peformed for following cases:
     * -In case of End-Point PSP, will be performed cause SL > 2 (LyrQ[1]==0)
     * -In case of End-Point USP, will be performed cause SL > 1 (LyrQ[0]==0)
     */

    /** End-Point in PMF3 Context */
    SHR_IF_ERR_EXIT(field_srv6_context_set
                    (unit, bcmFieldStageIngressPMF3, "SRv6_srep_pmf3",
                     &cint_field_srv6_endpoint_info.ipmf3_context_id));

    /** End-Point in PMF3 Qualifies according to PMF1 context (same conditiions for these contexts in both PMFs) */
    SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                    (unit, cint_field_srv6_endpoint_info.ipmf1_context_id,
                     cint_field_srv6_endpoint_info.ipmf3_context_id, 11));

    /*
     * End-Point in PMF3 FG - Parsting-Start-Type, Parsing-Start-Offset 
     */
    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_ipmf3_de(unit));

    if (!is_psp)
    {
        /*
         *Case of Egress [USP/ECO Last Node]
         */

        if (dnx_data_dev_init.general.feature_get(unit, dnx_data_dev_init_general_ecologic_support))
        {

            /************************/
            /** Egress ECO PMF1-2 **/
            /**********************/

            /** create the context in PMF1 and name it */
            SHR_IF_ERR_EXIT(field_srv6_context_set
                            (unit, bcmFieldStageIngressPMF1, "SRv6_egrs_ECO_pmf1",
                             &cint_field_srv6_egress_usp_info.ipmf1_context_id));

            /** create the Presel and attach it to the above context */
            SHR_IF_ERR_EXIT(field_srv6_pmf1_presel_set
                            (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id, 12, 1, is_psp));

            /** Egress with Ecologic - Configure FG in PMF2, if (NOF SIDs < 3) pass [SIDs length in Bytes], else [SIDs - 60B] for bytes_to_remove_fix use */
            SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf2_ecologic(unit));

            /**********************/
            /** Egress ECO PMF3 **/
            /*********************/

            /** create the context in PMF3 and name it */
            SHR_IF_ERR_EXIT(field_srv6_context_set
                            (unit, bcmFieldStageIngressPMF3, "SRv6_egrs_ECO_pmf3",
                             &cint_field_srv6_egress_usp_info.ipmf3_context_id));

            /** create the Presel and attach it to the above context */
            SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                            (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                             cint_field_srv6_egress_usp_info.ipmf3_context_id, 12));

            /** configure FG in PMF3 */
            SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf3_ecologic(unit));
        }
        else
        {
            /************************/
            /** Egress USP PMF1-2 **/
            /**********************/

            /*
             * PMF1-2 configurations - Egress (USP)
             * -Presel Qualifiers: FWD layer is SRv6 + expected SL value == 1
             * -Actions:
             *        - get the NOF SIDs and store for PMF3
             *
             * Will be peformed for following cases:
             *        -In case of Egress USP, will be performed cause SL == 0 (LyrQ[0]==1)
             */

            /** create the context in PMF1 and name it */
            SHR_IF_ERR_EXIT(field_srv6_context_set
                            (unit, bcmFieldStageIngressPMF1, "SRv6_egrs_USP_pmf1",
                             &cint_field_srv6_egress_usp_info.ipmf1_context_id));

            /** create the Presel and attach it to the above context */
            SHR_IF_ERR_EXIT(field_srv6_pmf1_presel_set
                            (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id, 12, 1, is_psp));

            /** Egress - Configure FG in PMF2, get the NOF SIDs and store for PMF3 */
            SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf2(unit));

            /**********************/
            /** Egress USP PMF3 **/
            /*********************/

            /*
             * PMF3 configurations - Egress (USP)
             * -Presel Qualifiers: FWD layer is SRv6 + expected SL value == 0
             * -Actions:
             *         -it sets the Parsing-Start-Type to 'SRv6_Beyond' for parser context
             *         -updates the signals Bytes_to_Remove and Parsing_Start_Offset to terminate SRv6 and IPv6
             *
             * Will be peformed for following cases:
             *        -In case of Egress USP, will be performed cause SL == 0 (LyrQ[0]==1)
             */

            /** create the context in PMF3 and name it */
            SHR_IF_ERR_EXIT(field_srv6_context_set
                            (unit, bcmFieldStageIngressPMF3, "SRv6_egrs_USP_pmf3",
                             &cint_field_srv6_egress_usp_info.ipmf3_context_id));

            /** create the Presel and attach it to the above context */
            SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                            (unit, cint_field_srv6_egress_usp_info.ipmf1_context_id,
                             cint_field_srv6_egress_usp_info.ipmf3_context_id, 12));

            /** configure FG in PMF3 */
            SHR_IF_ERR_EXIT(field_srv6_config_egress_usp_ipmf3_de(unit));

        }

    }
    else
    {

        /*
         * Case of Egress PSP
         *
         * -Since in Egress PSP we cut all Bytes in Egress, we must update TM with size of Bytes we are to cut.
         * -We do this by using PMF to write into network-header-append-size-ptr which is 8bits and usually is
         *  set by 8 MSB bits of the Outlif. These bits are key to TM table which sets per entry the compensated value.
         * -Since we want to use same Outlif for all cases of NOF SIDs, we want to override with PMF the key to represent
         *  the SIDs number - entries 1,2,3,4,5,6 and have the adequate compensation.
         */

        int sid_compensation_size_entry;
        bcm_cosq_pkt_size_adjust_info_t adjust_info;
        int egress_basic_compensation_size = 48; /** compensation in Bytes without SIDs - IPv6 (40B) + SRH (8B)*/

        /*
         * PMF1-2 configurations - Egress PSP
         * -Presel Qualifiers: (FWD layer is SRv6 && expected SL == 1 && is_psp soc property)
         * -Actions:
         *         - saves IPv6 DIP in x4 UDH Containers  (used for End-Point USP/PSP, Egress PSP)
         *
         * Will be peformed for following cases:
         *         -In case of Egres PSP, will be performed cause SL == 1 (LyrQ[1]==1)
         */

         /** Egress PSP - create the context in PMF1 and name it */
        SHR_IF_ERR_EXIT(field_srv6_context_set
                        (unit, bcmFieldStageIngressPMF1, "SRv6_psp_udh_pmf1",
                         &cint_field_srv6_egress_psp_info.ipmf1_context_id));

         /** Egress PSP - create the Presel and attach it to the above context */
        SHR_IF_ERR_EXIT(field_srv6_pmf1_presel_set
                        (unit, cint_field_srv6_egress_psp_info.ipmf1_context_id, 14, 1, is_psp));

         /** Egress PSP - FG Action - Save IPv6 DIP in x4 UDH Containers in PMF2 */
        SHR_IF_ERR_EXIT(field_srv6_config_egress_psp_ipmf2_udh(unit));

        /*
         * Create TM Profiles per each NOF SIDs case
         * -Using entries 2-6 to represent the number of available SIDs in the packet
         */

        /** Configure Compensation  */
        adjust_info.source_info.source_type = bcmCosqPktSizeAdjustSourceGlobal;         /** compensation type*/
        adjust_info.flags = 0;
        adjust_info.cosq = 0;
        adjust_info.source_info.source_id = 0;         /** irrelevant */

        for (sid_compensation_size_entry = MIN_NOF_SIDS_PSP;
             sid_compensation_size_entry <= dnx_data_srv6.termination.max_nof_terminated_sids_psp_get(unit);
             sid_compensation_size_entry++)
        {
            int compensation_size = egress_basic_compensation_size + sid_compensation_size_entry * 16;

            BCM_GPORT_PROFILE_SET(adjust_info.gport, sid_compensation_size_entry);

            /*
             * updating egress compensation size to include each time the SID numbers x 16B
             * the TM compensation field is limited to 127, so not passing it
             */
            SHR_IF_ERR_EXIT(bcm_cosq_gport_pkt_size_adjust_set(unit, &adjust_info,
                                                               (compensation_size > 127 ? 127 : compensation_size)));
        }

        /*
         * PMF3 configurations - Egress (PSP)
         * -Presel Qualifiers: (FWD layer is SRv6 + expected SL value == 1 && is_psp soc property)
         * -Actions:
         *          -it sets the Parsing-Start-Type to 'SRv6_endpoint' for parser context
         *           cause PSP is simply a Qualifier in SRv6_endpoint Layer in Egress
         *          -sets the Bytes-to-Remove-Fix to keep the IPv6 header, but delete the ETH in ITPP
         *          -updates TM nwk_header_append_size with IPv6 + SRv6 length to compensate
         *           the Egress deletion of whole IPv6 + Srv6
         *
         * Will be peformed for following cases:
         *         -In case of Egres PSP, will be performed cause SL == 1 (LyrQ[1]==1)*
         */

        /** Egress - create the context in PMF3 and name it */
        SHR_IF_ERR_EXIT(field_srv6_context_set
                        (unit, bcmFieldStageIngressPMF3, "SRv6_psp_ipmf3",
                         &cint_field_srv6_egress_psp_info.ipmf3_context_id));

        /** Egress - create the Presel and attach it to the above context */
        SHR_IF_ERR_EXIT(field_srv6_pmf3_presel_set
                        (unit, cint_field_srv6_egress_psp_info.ipmf1_context_id,
                         cint_field_srv6_egress_psp_info.ipmf3_context_id, 12));

        /** Egress - Configure FG in PMF3 */
        SHR_IF_ERR_EXIT(field_srv6_config_egress_psp_ipmf3_de(unit));

    }

exit:
    SHR_FUNC_EXIT;
}
