/** \file appl_ref_srv6_field_init.c
 * $Id$
 *
 * configuring PMFs for SRV6 processing.
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
/** sal */
#include <sal/appl/sal.h>
#include <sal/core/libc.h>

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
} field_srv6_endpoint_info_t;

typedef struct cint_field_srv6_egress_info_s
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
    bcm_field_qualify_t parsing_start_offset_8th_bit_qual;
    bcm_field_qualify_t parsing_index_qual;
    bcm_field_qualify_t bytes_to_remove_header_qual;
    bcm_field_qualify_t bytes_to_remove_fix_qual;
    bcm_field_qualify_t large_parsing_start_offset_efes_valid_qual;
    bcm_field_qualify_t large_parsing_start_offset_valid_and_srh_base_qual;
    bcm_field_qualify_t large_parsing_start_offset_srh_extended_qual;
    bcm_field_qualify_t large_parsing_start_offset_add_64_qual;
    bcm_field_qualify_t large_parsing_start_offset_8th_bit_qual;
    bcm_field_qualify_t large_bytes_to_remove_efes_valid_qual;
    bcm_field_qualify_t large_bytes_to_remove_qual;
    bcm_field_action_t large_parsing_start_offset_action;
    bcm_field_action_t large_bytes_to_remove_action;
} field_srv6_egress_info_t;

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
};

/** using -1 to put all '1' in the field, to mark as invalid */
field_srv6_egress_info_t cint_field_srv6_egress_info = {
    -1, -1, -1, -1,
    {-1, bcmFieldStageCount}, {-1, bcmFieldStageCount},
    bcmFieldQualifyCount, bcmFieldActionCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldQualifyCount, bcmFieldQualifyCount, bcmFieldQualifyCount,
    bcmFieldActionCount, bcmFieldActionCount,
};

/*
 * Functions
 */

static int
field_srv6_config_ipmf1_presel(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t ipmf1_p_data;
    void *dest_char;
    int rv = 0;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_ipmf1", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info,
                                  &cint_field_srv6_endpoint_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_ipmf1", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info,
                                  &cint_field_srv6_egress_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_srv6_endpoint_info.ipmf1_p_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);

    cint_field_srv6_endpoint_info.ipmf1_p_id.presel_id = 11;
    cint_field_srv6_endpoint_info.ipmf1_p_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_srv6_endpoint_info.ipmf1_context_id;
    ipmf1_p_data.nof_qualifiers = 2;

    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    /**SRv6_Beyond is 21*/
    ipmf1_p_data.qual_data[0].qual_arg = 0;
    ipmf1_p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf1_p_data.qual_data[0].qual_mask = 0x1F;

    ipmf1_p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    /**Need to verify that bit 0 of SRV6 layer qualifier is set to 0, indicating that there are still segements left.*/
    ipmf1_p_data.qual_data[1].qual_arg = 0;
    ipmf1_p_data.qual_data[1].qual_value = 0;
    ipmf1_p_data.qual_data[1].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_endpoint_info.ipmf1_p_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set ipmf1 endpoint\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_srv6_egress_info.ipmf1_p_id);
    bcm_field_presel_entry_data_info_init(&ipmf1_p_data);

    cint_field_srv6_egress_info.ipmf1_p_id.presel_id = 12;
    cint_field_srv6_egress_info.ipmf1_p_id.stage = bcmFieldStageIngressPMF1;
    ipmf1_p_data.entry_valid = TRUE;
    ipmf1_p_data.context_id = cint_field_srv6_egress_info.ipmf1_context_id;
    ipmf1_p_data.nof_qualifiers = 2;

    ipmf1_p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    /**SRv6_Beyond is 21*/
    ipmf1_p_data.qual_data[0].qual_arg = 0;
    ipmf1_p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf1_p_data.qual_data[0].qual_mask = 0x1F;

    ipmf1_p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    /**Need to verify that bit 0 of SRV6 layer qualifier is set to 1, indicating that there are no more segements left.*/
    ipmf1_p_data.qual_data[1].qual_arg = 0;
    ipmf1_p_data.qual_data[1].qual_value = 1;
    ipmf1_p_data.qual_data[1].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_egress_info.ipmf1_p_id, &ipmf1_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set ipmf1 egress\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

static int
field_srv6_config_ipmf3_presel(
    int unit)
{
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t ipmf3_p_data;
    void *dest_char;
    int rv = 0;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_endpoint_ipmf3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info,
                                  &cint_field_srv6_endpoint_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress_ipmf3", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF3, &context_info,
                                  &cint_field_srv6_egress_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&cint_field_srv6_endpoint_info.ipmf3_p_id);
    bcm_field_presel_entry_data_info_init(&ipmf3_p_data);

    cint_field_srv6_endpoint_info.ipmf3_p_id.presel_id = 11;
    cint_field_srv6_endpoint_info.ipmf3_p_id.stage = bcmFieldStageIngressPMF3;
    ipmf3_p_data.entry_valid = TRUE;
    ipmf3_p_data.context_id = cint_field_srv6_endpoint_info.ipmf3_context_id;
    ipmf3_p_data.nof_qualifiers = 2;

    ipmf3_p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    /**SRv6_Beyond is 21*/
    ipmf3_p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf3_p_data.qual_data[0].qual_mask = 0x1F;

    ipmf3_p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    /**Need to verify that bit 0 of SRV6 layer qualifier is set to 0, indicating that there are still segements left.*/
    ipmf3_p_data.qual_data[1].qual_arg = 0;
    ipmf3_p_data.qual_data[1].qual_value = 0;
    ipmf3_p_data.qual_data[1].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_endpoint_info.ipmf3_p_id, &ipmf3_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set ipmf3 endpoint\n", rv);
        return rv;
    }

    cint_field_srv6_egress_info.ipmf3_p_id.presel_id = 12;
    cint_field_srv6_egress_info.ipmf3_p_id.stage = bcmFieldStageIngressPMF3;
    ipmf3_p_data.entry_valid = TRUE;
    ipmf3_p_data.context_id = cint_field_srv6_egress_info.ipmf3_context_id;
    ipmf3_p_data.nof_qualifiers = 2;

    ipmf3_p_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    /**SRv6_Beyond is 21*/
    ipmf3_p_data.qual_data[0].qual_value = bcmFieldLayerTypeSrv6Beyond;
    ipmf3_p_data.qual_data[0].qual_mask = 0x1F;

    ipmf3_p_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    /**Need to verify that bit 0 of SRV6 layer qualifier is set to 1, indicating that there are no more segements left.*/
    ipmf3_p_data.qual_data[1].qual_arg = 0;
    ipmf3_p_data.qual_data[1].qual_value = 1;
    ipmf3_p_data.qual_data[1].qual_mask = 0x1;

    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_egress_info.ipmf3_p_id, &ipmf3_p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in dnx_field_presel_set ipmf3 egress\n", rv);
        return rv;
    }

    return BCM_E_NONE;
}

static int
field_srv6_config_endpoint_ipmf2_udh(
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
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.udh_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create UDH\n");
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
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
     */
    fg_info.action_types[4] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[4] = FALSE;
    fg_info.action_types[5] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[5] = FALSE;
    fg_info.action_types[6] = bcmFieldActionUDHBase1;
    fg_info.action_with_valid_bit[6] = FALSE;
    fg_info.action_types[7] = bcmFieldActionUDHBase0;
    fg_info.action_with_valid_bit[7] = FALSE;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_endpoint_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

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
     * Take the last 128 bit from the packet (the 128 lsb of the header).
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = 0;
    /*
     * Use type 3 for each of the four UDH types to get UDH size 4 for each
     * (assuming UDG type 2 maps to 4 Bytes in IPPD_UDH_TYPE_TO_SIZE_MAP).
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

static int
field_srv6_config_egress_ipmf2(
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
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.srh_last_entry_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_qualifier_create srh_last_entry_qual\n");
        return rv;
    }

    /*
     * Create a void action. The container action will be performed by a FEM.
     */
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionVoid;
    action_info.size = 8;
    action_info.prefix_size = 24;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF2;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_info.container_void_action);
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
    fg_info.qual_types[0] = cint_field_srv6_egress_info.srh_last_entry_qual;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 1;

    fg_info.action_types[0] = cint_field_srv6_egress_info.container_void_action;
    fg_info.action_with_valid_bit[0] = FALSE;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_egress_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf2_fg\n", rv);
        return rv;
    }

    /**Configure FEM to increment the last segment by one to get the number of segments.*/
    bcm_field_fem_action_info_t_init(&fem_action_info);
    fem_action_info.fem_input.input_offset = 0;
    fem_action_info.condition_msb = 3;
    for (ii = 0; ii < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_FEM; ii++)
    {
        fem_action_info.fem_condition[ii].is_action_valid = TRUE;
        fem_action_info.fem_condition[ii].extraction_id = 0;
    }
    fem_action_info.fem_extraction[0].action_type = bcmFieldActionContainer;
    for (ii = 0; ii < 8; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeKey;
        fem_action_info.fem_extraction[0].output_bit[ii].offset = ii;
    }
    for (; ii < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; ii++)
    {
        fem_action_info.fem_extraction[0].output_bit[ii].source_type = bcmFieldFemExtractionOutputSourceTypeForce;
        fem_action_info.fem_extraction[0].output_bit[ii].forced_value = 0;
    }
    fem_action_info.fem_extraction[0].increment = 1;

    rv = bcm_field_fem_action_add(unit, 0, cint_field_srv6_egress_info.ipmf2_fg, BCM_FIELD_ACTION_POSITION(1, 2),
                                  &fem_action_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_fem_action_add ipmf2\n", rv);
        return rv;
    }

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
     * Take the "last entry" or "last segment" field from the header
     */
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[0].input_arg = 0;
    attach_info.key_info.qual_info[0].offset = 32;

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_info.ipmf2_fg,
                                        cint_field_srv6_egress_info.ipmf1_context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf2\n", rv);
        return rv;
    }

    return 0;
}

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
     * the qualifiers later.
     * Alternatively, const qualifiers or masks can be used.
     */
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
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_endpoint_layer_type_qual\n", rv);
        return rv;
    }
    /**Need to copy the offset of forwarding layer to parsing offset action*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create lr_fwd_offset_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 8;
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
     */
    fg_info.nof_quals = 4;
    fg_info.qual_types[0] = cint_field_srv6_endpoint_info.parsing_start_type_valid_qual;
    fg_info.qual_types[1] = cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual;
    fg_info.qual_types[2] = cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual;
    fg_info.qual_types[3] = cint_field_srv6_endpoint_info.lr_fwd_offset_qual;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 2;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[1] = FALSE;

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_endpoint_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add ipmf3_fg\n", rv);
        return rv;
    }

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

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;

    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;

    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 5;

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

static int
field_srv6_config_egress_ipmf3_de(
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
    int lr_offset_offset;
    int container_offset;
    bcm_field_qualifier_info_get_t qual_info_get;
    uint32 bytes_to_remove_header = 2;
    uint32 large_bytes_to_remove_fix = 64;
    uint32 egress_parsing_index = 2;

    /*
     * Use bcm_field_context_param_set() to fill the KeyGenVal.
     * Contains 5 bits with value 21 (LayerTypeSrv6Beyond) and 1 bit with value 1 (valid bits) to be used by
     * the qualifiers later.
     * 4 more bits contain the size of an SRH header without the segments (8) for parsing_start_offset
     * 2 more bits bytes_to_remove_header
     * 7 more bits contain bytes_to_remove_fix for the case of more than 7 segments
     * Alternatively, const qualifiers or masks can be used.
     */
    param_info.param_type = bcmFieldContextParamTypeKeyVal;
    param_info.param_val =
        0x15 + (1 << 5) + (8 << 6) + (bytes_to_remove_header << 10) + (large_bytes_to_remove_fix << 12) +
        (egress_parsing_index << 19);
    param_info.param_arg = 0;
    rv = bcm_field_context_param_set(unit, 0, bcmFieldStageIngressPMF3, cint_field_srv6_egress_info.ipmf3_context_id,
                                     &param_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_param_set\n", rv);
        return rv;
    }

    /**We build the number 21 which is hex 0x15 for parsing start type.*/
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**Qualifier for valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 5;
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.srv6_beyond_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create srv6_beyond_layer_type_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 4;
    /**The size in bytes of the first part of the SRH plus valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    /**The number of segments modulo 8.*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**The 8th bit of parsing start_offset is irrelevant.*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.parsing_start_offset_8th_bit_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_start_offset_8th_bit\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 3;
    /**Qualifier egress parsing index*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.parsing_index_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create parsing_index_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    /**Qualifier for de on fwd offset*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.bytes_to_remove_header_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_header_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 7;
    /**Qualifier for de on fwd offset*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.bytes_to_remove_fix_qual);
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
    /**Both EFES valid bit according to the fourth bit of the number of entries..*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_parsing_start_offset_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_efes_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1 + 4;
    /**The size in bytes of the first part of the SRH_plus valid bit*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2;
    /**The number of segments modulo 4.*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**Add 64 bits to compensate for the lost 128 (64 more will be added using bytes_to_remove_fix.*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_parsing_start_offset_add_64_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_add_64_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**The 8th bit of parsing start_offset is irrelevant.*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_parsing_start_offset_8th_bit_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_parsing_start_offset_8th_bit\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 1;
    /**Qualifier for EFES valid bit rewriting bytes to remove if there are more than 7 segments*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info,
                                    &cint_field_srv6_egress_info.large_bytes_to_remove_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create large_bytes_to_remove_efes_valid_qual\n", rv);
        return rv;
    }
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size = 2 + 7;
    /**Qualifier for rewriting bytes to remove header if there are more than 7 segments*/
    rv = bcm_field_qualifier_create(unit, 0, &qual_info, &cint_field_srv6_egress_info.large_bytes_to_remove_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_qualifier_create bytes_to_remove_qual\n", rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionParsingStartOffset;
    action_info.size = 9;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_info.large_parsing_start_offset_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create large_parsing_start_offset_action\n", rv);
        return rv;
    }
    bcm_field_action_info_t_init(&action_info);
    action_info.action_type = bcmFieldActionStartPacketStrip;
    action_info.size = 9;
    action_info.prefix_size = 0;
    action_info.prefix_value = 0;
    action_info.stage = bcmFieldStageIngressPMF3;
    rv = bcm_field_action_create(unit, 0, &action_info, &cint_field_srv6_egress_info.large_bytes_to_remove_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d) in bcm_field_action_create large_bytes_to_remove_action\n", rv);
        return rv;
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

    /**Get the offset on the PBUS of iPMF1/2 to iPMF3 container.*/
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
    fg_info.nof_quals = 15;
    fg_info.qual_types[0] = cint_field_srv6_egress_info.parsing_start_type_valid_qual;
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[0].offset = key_gen_var_offset + 5;
    fg_info.qual_types[1] = cint_field_srv6_egress_info.srv6_beyond_layer_type_qual;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[1].offset = key_gen_var_offset + 0;
    fg_info.qual_types[2] = cint_field_srv6_egress_info.parsing_start_offset_valid_and_srh_base_qual;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].offset = key_gen_var_offset + 5;
    fg_info.qual_types[3] = cint_field_srv6_egress_info.parsing_start_offset_srh_extended_qual;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].offset = container_offset;
    fg_info.qual_types[4] = cint_field_srv6_egress_info.parsing_start_offset_8th_bit_qual;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[4].input_arg = 0;
    fg_info.qual_types[5] = cint_field_srv6_egress_info.parsing_index_qual;
    attach_info.key_info.qual_info[5].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[5].offset = key_gen_var_offset + 19;
    fg_info.qual_types[6] = cint_field_srv6_egress_info.bytes_to_remove_header_qual;
    attach_info.key_info.qual_info[6].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[6].offset = key_gen_var_offset + 10;
    fg_info.qual_types[7] = cint_field_srv6_egress_info.bytes_to_remove_fix_qual;
    attach_info.key_info.qual_info[7].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[7].input_arg = 0;
    fg_info.qual_types[8] = cint_field_srv6_egress_info.large_parsing_start_offset_efes_valid_qual;
    attach_info.key_info.qual_info[8].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[8].offset = container_offset + 3;
    fg_info.qual_types[9] = cint_field_srv6_egress_info.large_parsing_start_offset_valid_and_srh_base_qual;
    attach_info.key_info.qual_info[9].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[9].offset = key_gen_var_offset + 5;
    fg_info.qual_types[10] = cint_field_srv6_egress_info.large_parsing_start_offset_srh_extended_qual;
    attach_info.key_info.qual_info[10].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[10].offset = container_offset;
    fg_info.qual_types[11] = cint_field_srv6_egress_info.large_parsing_start_offset_add_64_qual;
    attach_info.key_info.qual_info[11].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[11].offset = key_gen_var_offset + 5;
    fg_info.qual_types[12] = cint_field_srv6_egress_info.large_parsing_start_offset_8th_bit_qual;
    attach_info.key_info.qual_info[12].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[12].input_arg = 0;
    fg_info.qual_types[13] = cint_field_srv6_egress_info.large_bytes_to_remove_efes_valid_qual;
    attach_info.key_info.qual_info[13].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[13].offset = container_offset + 3;
    fg_info.qual_types[14] = cint_field_srv6_egress_info.large_bytes_to_remove_qual;
    attach_info.key_info.qual_info[14].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[14].offset = key_gen_var_offset + 10;

    /*
     * Set actions 
     */
    fg_info.nof_actions = 6;
    fg_info.action_types[0] = bcmFieldActionParsingStartType;
    fg_info.action_with_valid_bit[0] = FALSE;
    fg_info.action_types[1] = bcmFieldActionParsingStartOffset;
    fg_info.action_with_valid_bit[1] = FALSE;
    fg_info.action_types[2] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_with_valid_bit[2] = FALSE;
    fg_info.action_types[3] = bcmFieldActionStartPacketStrip;
    fg_info.action_with_valid_bit[3] = FALSE;
    fg_info.action_types[4] = cint_field_srv6_egress_info.large_parsing_start_offset_action;
    fg_info.action_with_valid_bit[4] = TRUE;
    fg_info.action_types[5] = cint_field_srv6_egress_info.large_bytes_to_remove_action;
    fg_info.action_with_valid_bit[5] = TRUE;

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF3;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_egress", sizeof(fg_info.name));

    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &cint_field_srv6_egress_info.ipmf3_fg);
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

    rv = bcm_field_group_context_attach(unit, 0, cint_field_srv6_egress_info.ipmf3_fg,
                                        cint_field_srv6_egress_info.ipmf3_context_id, &attach_info);
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(field_srv6_config_ipmf1_presel(unit));

    SHR_IF_ERR_EXIT(field_srv6_config_ipmf3_presel(unit));

    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_ipmf2_udh(unit));

    SHR_IF_ERR_EXIT(field_srv6_config_egress_ipmf2(unit));

    SHR_IF_ERR_EXIT(field_srv6_config_endpoint_ipmf3_de(unit));

    SHR_IF_ERR_EXIT(field_srv6_config_egress_ipmf3_de(unit));

exit:
    SHR_FUNC_EXIT;
}

static int
field_srv6_endpoint_destroy(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_endpoint_info.ipmf1_p_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set ipmf1_p_id\n", rv);
        return rv;
    }

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_endpoint_info.ipmf3_p_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set ipmf3_p_id\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_srv6_endpoint_info.ipmf2_fg,
                                        cint_field_srv6_endpoint_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipmf2_fg,ipmf1_context_id\n", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, cint_field_srv6_endpoint_info.ipmf3_fg,
                                        cint_field_srv6_endpoint_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipmf3_fg,ipmf3_context_id\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_field_srv6_endpoint_info.ipmf2_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipmf2_fg\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_field_srv6_endpoint_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipmf3_fg\n", rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, cint_field_srv6_endpoint_info.ipmf1_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy\n ipmf1_context_id", rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, cint_field_srv6_endpoint_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf3_context_id\n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.udh_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy udh_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.udh_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy udh_type_qual \n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.srv6_endpoint_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy srv6_endpoint_layer_type_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.parsing_start_offset_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_offset_valid_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_endpoint_info.lr_fwd_offset_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy lr_fwd_offset_qual\n", rv);
        return rv;
    }

    return rv;
}

static int
field_srv6_egress_destroy(
    int unit)
{
    bcm_field_presel_entry_data_t presel_entry_data;
    int rv = BCM_E_NONE;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = 0;
    rv = bcm_field_presel_set(unit, 0, &cint_field_srv6_egress_info.ipmf3_p_id, &presel_entry_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set ipmf3_p_id\n", rv);
        return rv;
    }

    rv = bcm_field_group_context_detach(unit, cint_field_srv6_egress_info.ipmf3_fg,
                                        cint_field_srv6_egress_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_detach ipmf3_fg,ipmf3_context_id\n", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, cint_field_srv6_egress_info.ipmf3_fg);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete ipmf3_fg\n", rv);
        return rv;
    }
    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF3, cint_field_srv6_egress_info.ipmf3_context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_destroy ipmf3_context_id\n", rv);
        return rv;
    }

    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.parsing_start_type_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_type_valid_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.srv6_beyond_layer_type_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy srv6_beyond_layer_type_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.parsing_start_offset_8th_bit_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_start_offset_8th_bit_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.parsing_index_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy parsing_index_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.bytes_to_remove_header_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy bytes_to_remove_header_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.bytes_to_remove_fix_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy bytes_to_remove_fix_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_parsing_start_offset_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_parsing_start_offset_efes_valid_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit,
                                     cint_field_srv6_egress_info.large_parsing_start_offset_valid_and_srh_base_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_parsing_start_offset_valid_and_srh_base_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_parsing_start_offset_srh_extended_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_parsing_start_offset_srh_extended_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_parsing_start_offset_add_64_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_parsing_start_offset_add_64_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_parsing_start_offset_8th_bit_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_parsing_start_offset_8th_bit_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_bytes_to_remove_efes_valid_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_bytes_to_remove_efes_valid_qual\n", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, cint_field_srv6_egress_info.large_bytes_to_remove_qual);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_qualifier_destroy large_bytes_to_remove_qual\n", rv);
        return rv;
    }
    rv = bcm_field_action_destroy(unit, cint_field_srv6_egress_info.large_parsing_start_offset_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy large_parsing_start_offset_action\n", rv);
        return rv;
    }
    rv = bcm_field_action_destroy(unit, cint_field_srv6_egress_info.large_bytes_to_remove_action);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_action_destroy large_bytes_to_remove_action\n", rv);
        return rv;
    }

    return rv;
}

shr_error_e
appl_dnx_field_srv6_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(field_srv6_endpoint_destroy(unit));

    SHR_IF_ERR_EXIT(field_srv6_egress_destroy(unit));

exit:
    SHR_FUNC_EXIT;
}
