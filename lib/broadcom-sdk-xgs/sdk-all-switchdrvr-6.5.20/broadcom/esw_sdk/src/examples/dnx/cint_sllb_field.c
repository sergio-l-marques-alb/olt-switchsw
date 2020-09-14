/*
 * $Id: cint_ip_ecmp_rpf_examples.c
 * Exp $
 $Copyright: (c) 2020 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 *
 * Example of PMF application to snoop SLLB packets for CPU processing
 *
 * cint ../../../../src/examples/dnx/cint_sllb_basic.c
 * cint ../../../../src/examples/dnx/cint_sllb_field.c
 * 
 * cint
 * print sllb_field_example(0);
 * exit;
 * 
 * cint
 * print sllb_field_example_cleanup(0);
 * exit;
 * 
 * The following iPMF1/2 are specific to the SLLB feature:
 * - bcmFieldQualifyVipValid              - indicates if the FWD1 result was a VIP FEC_ID, representing a VIP ECMP Consistent Hash Table
 * - bcmFieldQualifyVipId                 - The VIP ECMP Consistent Hash Table Index
 * - bcmFieldQualifyVIPMemberReference    - The entry accessed in the VIP ECMP Consistent Hash Table
 * - bcmFieldQualifyPccHit                - indication if there was a hit in the PCC table(s)
 *
 * This example creates 
 * - iPMF1 contexts:
 * -- Ipv4 forwarding context
 * -- IPv6 forwarding context
 * - a TCAM DB attached to each context
 *   -- install entries at init to snoop packets with TCP FIN/RST and PCC hit to the CPU. This may be used to notify the host when TCP sessions are termatined for SW aging of the PCC table(s).
 *   -- install entries during server removal with qualifiers (PCC miss, VIP_ID, and server FEC_ID}. Action is not snoop to the host for SW elarning.
 *   -- install entries during server addition with qualifiers (PCC miss, VIP_ID, and MEMBER_REFERENCE}. Action is not snoop to the host for SW learning.
 * - a DE DB attached to each context
 *   -- stamp the UDH with information to support SW learning
 *      UD2 = {server FEC_ID(20), VIP_ID(12)}
 *      UD3 = SIGNATURE(32) = {ECMP-Lb-Key[2](16), Nwk-Lb-Key(16)}
 * 
 */

struct sllb_field_data_s
{
    bcm_gport_t sllb_snoop_gport;
    int snoop_trap_id;
    bcm_gport_t mirror_dest_id;

    bcm_field_qualify_t tcp_flags_qual_id;
    bcm_field_qualify_t udh3_base_qual_id;		
    bcm_field_qualify_t udh2_base_qual_id;		
    bcm_field_qualify_t fec_id_qual_id;	

    bcm_field_group_t tcam_fg_id;
    bcm_field_group_t de_fg_id;

    bcm_field_entry_t sllb_member_reference_tcam_entry_handle;
    bcm_field_entry_t sllb_server_fec_tcam_entry_handle;
    bcm_field_entry_t sllb_tcp_fin_tcam_entry_handle;
    bcm_field_entry_t sllb_tcp_rst_tcam_entry_handle;

    bcm_field_context_t ipv4_context_id;
    bcm_field_context_t ipv6_context_id;

    bcm_field_presel_t ipv4_presel_id;
    bcm_field_presel_t ipv6_presel_id;
};

sllb_field_data_s g_sllb_field_data = {
    /* sllb_snoop_gport */
    0,
    /* snoop_trap_id */
    0,
    /* mirror_dest_id */
    0,

    /* tcp_flags_qual_id */
    0,
    /* udh3_base_qual_id */
    0,
    /* udh2_base_qual_id */
    0,
    /* fec_id_qual_id */
    0,

    /* tcam_fg_id */
    0,
    /* de_fg_id */
    0,

    /* sllb_member_reference_tcam_entry_handle */
    0,
    /* sllb_server_fec_tcam_entry_handle */
    0,
    /* sllb_tcp_fin_tcam_entry_handle */
    0,
    /* sllb_tcp_rst_tcam_entry_handle */
    0,

    /* ipv4_context_id */
    0,
    /* ipv6_context_id */
    0,

    /* ipv4_presel_id = 16 */
    16,
    /* ipv6_presel_id = 17 */
    17
};

/* 
 * Create 2 contexts:
 * 1. Forwarding Layer is IPv4
 * 2. Forwarding Layer is IPv6
 */
int sllb_create_context(int unit, bcm_field_context_t *ipv4_context_id, bcm_field_context_t *ipv6_context_id)
{
    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy(dest_char, "SLLB IPv4    ", 12);
    context_info.name[12] = 0;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, context_info, ipv4_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_create_context: bcm_field_context_create failed, rv %l", rv);
        return rv;
    }

    /* Create presel entry to map relevant traffic to the context */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = g_sllb_field_data.ipv4_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = *ipv4_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp4;
    presel_entry_data.qual_data[0].qual_mask = 0x1f;
    presel_entry_data.qual_data[0].qual_arg = 0;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_create_context: bcm_field_presel_set failed, rv %l", rv);
        return rv;
    }

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy(dest_char, "SLLB IPv6    ", 12);
    context_info.name[12] = 0;
    rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, context_info, ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_create_context: bcm_field_context_create failed, rv %l", rv);
        return rv;
    }

    /* Create presel entry to map relevant traffic to the context */
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = g_sllb_field_data.ipv6_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.context_id = *ipv6_context_id;
    presel_entry_data.entry_valid = TRUE;
    presel_entry_data.nof_qualifiers = 1;

    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeIp6;
    presel_entry_data.qual_data[0].qual_mask = 0x1f;
    presel_entry_data.qual_data[0].qual_arg = 0;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_create_context: bcm_field_presel_set failed, rv %l", rv);
        return rv;
    }
    return rv;
}


/*
 * Create a TCAM DB in IPMF1
 *    This DB will hold entries to match VIP packets and snoop to the host.
 * Qualifiers:
 *  bcmFieldQualifyDstPort: server FEC_ID
 *  bcmFieldQualifyVipValid
 *  bcmFieldQualifyVipId
 *  bcmFieldQualifyVIPMemberReference
 *  bcmFieldQualifyPccHit
 *  bcmFieldQualifyLayerRecordType: to identify packets with TCPoIPvX
 *  user defined qualifier for TCP flags
 * Actions:
 *  bcmFieldActionSnoop
 */
int sllb_snoop_ipmf1_tcam_fg(int unit, bcm_field_context_t ipv4_context_id, bcm_field_context_t ipv6_context_id)
{

    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_group_info_t fg_info;
    bcm_field_group_info_t_init(&fg_info);
    bcm_field_qualifier_info_create_t qual_info;


    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* User defined qualifiers */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=4;
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, "tcp_flags    ", 12);
    qual_info.name[12] = 0;
    rv = bcm_field_qualifier_create(unit, 0, qual_info, &g_sllb_field_data.tcp_flags_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_ipmf1_tcam_fg: bcm_field_qualifier_create failed, rv %l", rv);
        return rv;
    }

    fg_info.nof_quals = 0;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyDstPort;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyVipValid;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyVipId;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyVIPMemberReference;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyPccHit;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[fg_info.nof_quals++] = g_sllb_field_data.tcp_flags_qual_id;

    fg_info.nof_actions = 0;
    fg_info.action_types[fg_info.nof_actions] = bcmFieldActionSnoop;
    fg_info.action_with_valid_bit[fg_info.nof_actions++] = TRUE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "sllb_tcam", 9);
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, &g_sllb_field_data.tcam_fg_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_ipmf1_tcam_fg: bcm_field_group_add failed, rv %l", rv);
        return rv;
    }


    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_attach_info_t_init(&attach_info);

    fg_info.nof_quals = 0;
    /* bcmFieldQualifyDstPort */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyVipValid */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyVipId */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyVIPMemberReference */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyPccHit */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldInputTypeLayerRecordsFwd */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals];
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 1;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* tcp_flags_qual_id  */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeLayerFwd;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 1;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 108;   /* Bit offset of TCP flagas from FWD+1 */

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    fg_info.nof_actions = 0;
    attach_info.payload_info.action_types[fg_info.nof_actions] = fg_info.action_types[fg_info.nof_actions++]; /* bcmFieldActionUDHBase3 */

    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    rv = bcm_field_group_context_attach(unit, 0, g_sllb_field_data.tcam_fg_id, ipv4_context_id, &attach_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_ipmf1_tcam_fg: bcm_field_group_context_attach failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_context_attach(unit, 0, g_sllb_field_data.tcam_fg_id, ipv6_context_id, &attach_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_ipmf1_tcam_fg: bcm_field_group_context_attach failed, rv %l", rv);
        return rv;
    }
    return rv;
}


/* 
 * Create a DE DB in IPMF2 to stamp the VIP_ID, SIGNATURE, and member FEC-Id on UDH2, UDH3 
 *    For packets snooped to the host, this data can be used for PCC State table SW learning/aging.
 */
int sllb_stamp_udh_ipmf2_de(int unit, bcm_field_context_t ipv4_context_id, bcm_field_context_t ipv6_context_id) 
{

    int rv = BCM_E_NONE;
    void *dest_char;
    bcm_field_qualifier_info_create_t qual_info;
    bcm_field_group_info_t fg_info;
    bcm_field_qualifier_info_get_t get_qual_info;

    /* User defined qualifiers */
    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=2;
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, "udh3_base_pmf", 12);
    qual_info.name[12] = 0;
    rv = bcm_field_qualifier_create(unit, 0, qual_info, &g_sllb_field_data.udh3_base_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_qualifier_create failed, rv %l", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=2;
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, "udh2_base_pmf", 12);
    qual_info.name[12] = 0;
    rv = bcm_field_qualifier_create(unit, 0, qual_info, &g_sllb_field_data.udh2_base_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_qualifier_create failed, rv %l", rv);
        return rv;
    }

    bcm_field_qualifier_info_create_t_init(&qual_info);
    qual_info.size=20;
    dest_char = &(qual_info.name[0]);
    sal_strncpy(dest_char, "fec_id      ", 6);
    qual_info.name[12] = 0;
    rv = bcm_field_qualifier_create(unit, 0, qual_info, &g_sllb_field_data.fec_id_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_qualifier_create failed, rv %l", rv);
        return rv;
    }

    /* User defined actions */

    bcm_field_group_info_t_init(&fg_info);

    fg_info.fg_type = bcmFieldGroupTypeDirectExtraction;
    fg_info.stage = bcmFieldStageIngressPMF2;

    fg_info.nof_quals = 0;
    fg_info.qual_types[fg_info.nof_quals++] = g_sllb_field_data.udh3_base_qual_id;
    fg_info.qual_types[fg_info.nof_quals++] = g_sllb_field_data.udh2_base_qual_id;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyNetworkLoadBalanceKey;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyEcmpLoadBalanceKey2;
    fg_info.qual_types[fg_info.nof_quals++] = bcmFieldQualifyVipId;
    fg_info.qual_types[fg_info.nof_quals++] = g_sllb_field_data.fec_id_qual_id;

    fg_info.nof_actions = 0;
    fg_info.action_types[fg_info.nof_actions] = bcmFieldActionUDHBase3;
    fg_info.action_with_valid_bit[fg_info.nof_actions++] = FALSE;
    fg_info.action_types[fg_info.nof_actions] = bcmFieldActionUDHBase2;
    fg_info.action_with_valid_bit[fg_info.nof_actions++] = FALSE;
    fg_info.action_types[fg_info.nof_actions] = bcmFieldActionUDHData3;
    fg_info.action_with_valid_bit[fg_info.nof_actions++] = FALSE;
    fg_info.action_types[fg_info.nof_actions] = bcmFieldActionUDHData2;
    fg_info.action_with_valid_bit[fg_info.nof_actions++] = FALSE;

    dest_char = &(fg_info.name[0]);
    sal_strncpy(dest_char, "sllb_de", 7);
    fg_info.name[7] = 0;
    rv = bcm_field_group_add(unit, 0, fg_info, &g_sllb_field_data.de_fg_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_group_add failed, rv %l", rv);
        return rv;
    }

    bcm_field_group_attach_info_t attach_info;
    bcm_field_group_attach_info_t_init(&attach_info);


    fg_info.nof_quals = 0;
    /* UDH Base 3 */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 1;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* UDH Base 2 */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeConst;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 1;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyNetworkLoadBalanceKey */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyEcmpLoadBalanceKey2 */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* bcmFieldQualifyVipId */
    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = 0;

    /* fec_id_qual_id */
    rv = bcm_field_qualifier_info_get(0, bcmFieldQualifyDstPort, bcmFieldStageIngressPMF1, &get_qual_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_qualifier_info_get failed, rv %l", rv);
        return rv;
    }

    attach_info.key_info.qual_types[fg_info.nof_quals] = fg_info.qual_types[fg_info.nof_quals]; 
    attach_info.key_info.qual_info[fg_info.nof_quals].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[fg_info.nof_quals].input_arg = 0;
    attach_info.key_info.qual_info[fg_info.nof_quals++].offset = get_qual_info.offset;

    attach_info.key_info.nof_quals = fg_info.nof_quals;

    fg_info.nof_actions = 0;
    attach_info.payload_info.action_types[fg_info.nof_actions] = fg_info.action_types[fg_info.nof_actions++]; /* bcmFieldActionUDHBase3 */
    attach_info.payload_info.action_types[fg_info.nof_actions] = fg_info.action_types[fg_info.nof_actions++]; /* bcmFieldActionUDHBase2 */
    attach_info.payload_info.action_types[fg_info.nof_actions] = fg_info.action_types[fg_info.nof_actions++]; /* bcmFieldActionUDHData3 */
    attach_info.payload_info.action_types[fg_info.nof_actions] = fg_info.action_types[fg_info.nof_actions++]; /* bcmFieldActionUDHData2 */

    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    rv = bcm_field_group_context_attach(unit, 0, g_sllb_field_data.de_fg_id, ipv4_context_id, &attach_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_group_context_attach failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_context_attach(unit, 0, g_sllb_field_data.de_fg_id, ipv6_context_id, &attach_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_stamp_udh_ipmf2_de: bcm_field_group_context_attach failed, rv %l", rv);
        return rv;
    }
    return rv;
}


/*
 * Add entries to the TCAM FG to snoop packets when TCP FIN or RST flags are set.
 * When packets with TCP FIN or RST are detected, PCC State table entries for the session may be deleted.
 */
int sllb_fg_entry_tcp_term_add(int unit, int priority)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t ent_info;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority;
    ent_info.nof_entry_quals = 4; /* tcam_fg_id.nof_quals */
    ent_info.entry_qual[0].type = bcmFieldQualifyVipValid;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 1;
    ent_info.entry_qual[1].type = bcmFieldQualifyPccHit;
    ent_info.entry_qual[1].value[0] = 1;
    ent_info.entry_qual[1].mask[0] = 1;
    ent_info.entry_qual[2].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[2].value[0] = bcmFieldLayerTypeTcp;
    ent_info.entry_qual[2].mask[0] = 0x1f;
    ent_info.entry_qual[3].type = g_sllb_field_data.tcp_flags_qual_id;
    ent_info.entry_qual[3].value[0] = 0x1;
    ent_info.entry_qual[3].mask[0] = 0x1;


    ent_info.nof_entry_actions = 1; /* tcam_fg_id.nof_actions */
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = g_sllb_field_data.sllb_snoop_gport;
    ent_info.entry_action[0].value[1] = 0x11;

    rv = bcm_field_entry_add(unit, 0, g_sllb_field_data.tcam_fg_id, &ent_info, &g_sllb_field_data.sllb_tcp_fin_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_fg_entry_tcp_term_add: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority;
    ent_info.nof_entry_quals = 4; /* tcam_fg_id.nof_quals */
    ent_info.entry_qual[0].type = bcmFieldQualifyVipValid;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 1;
    ent_info.entry_qual[1].type = bcmFieldQualifyPccHit;
    ent_info.entry_qual[1].value[0] = 1;
    ent_info.entry_qual[1].mask[0] = 1;
    ent_info.entry_qual[2].type = bcmFieldQualifyLayerRecordType;
    ent_info.entry_qual[2].value[0] = bcmFieldLayerTypeTcp;
    ent_info.entry_qual[2].mask[0] = 0x1f;
    ent_info.entry_qual[3].type = g_sllb_field_data.tcp_flags_qual_id;
    ent_info.entry_qual[3].value[0] = 0x4;
    ent_info.entry_qual[3].mask[0] = 0x4;


    ent_info.nof_entry_actions = 1; /* tcam_fg_id.nof_actions */
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = g_sllb_field_data.sllb_snoop_gport;
    ent_info.entry_action[0].value[1] = 0x22;

    rv = bcm_field_entry_add(unit, 0, g_sllb_field_data.tcam_fg_id, &ent_info, &g_sllb_field_data.sllb_tcp_rst_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_fg_entry_tcp_term_add: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }

    return rv;
}

/*
 * Add an entry to match on {VIP_ID, MEMBER_REFERENCE}
 * Use case: Snoop packets forwarded to any VIP ECMP member array indices into which the new server will be added.
 */
int sllb_member_reference_tcam_entry_add(int unit, int priority, uint32 vip_id, uint32 member_reference)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t ent_info;

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority;
    ent_info.nof_entry_quals = 4; /* tcam_fg_id.nof_quals */
    ent_info.entry_qual[0].type = bcmFieldQualifyVipValid;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 1;
    ent_info.entry_qual[1].type = bcmFieldQualifyPccHit;
    ent_info.entry_qual[1].value[0] = 0;
    ent_info.entry_qual[1].mask[0] = 1;
    ent_info.entry_qual[2].type = bcmFieldQualifyVipId;
    ent_info.entry_qual[2].value[0] = vip_id;
    ent_info.entry_qual[2].mask[0] = 0xfff;
    ent_info.entry_qual[3].type = bcmFieldQualifyVIPMemberReference;
    ent_info.entry_qual[3].value[0] = member_reference;
    ent_info.entry_qual[3].mask[0] = 0x3fff;

    ent_info.nof_entry_actions = 1; /* tcam_fg_id.nof_actions */
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = g_sllb_field_data.sllb_snoop_gport;
    ent_info.entry_action[0].value[1] = 0x33;

    rv = bcm_field_entry_add(unit, 0, g_sllb_field_data.tcam_fg_id, &ent_info, &g_sllb_field_data.sllb_member_reference_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_member_reference_tcam_entry_add: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    return rv;
}


/*
 * Add an entry to match on {VIP_ID, SERVER FEC-ID}
 * Use case: Snoop packets forwarded to a server which will be removed from the VIP ECMP group.
 */
int sllb_server_fec_tcam_entry_add(int unit, int priority, uint32 vip_id, uint32 fec_id)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_info_t ent_info;


    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = priority;
    ent_info.nof_entry_quals = 4; /* tcam_fg_id.nof_quals */
    ent_info.entry_qual[0].type = bcmFieldQualifyVipValid;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 1;
    ent_info.entry_qual[1].type = bcmFieldQualifyPccHit;
    ent_info.entry_qual[1].value[0] = 0;
    ent_info.entry_qual[1].mask[0] = 1;
    ent_info.entry_qual[2].type = bcmFieldQualifyVipId;
    ent_info.entry_qual[2].value[0] = vip_id;
    ent_info.entry_qual[2].mask[0] = 0xfff;
    ent_info.entry_qual[3].type = bcmFieldQualifyDstPort;
    ent_info.entry_qual[3].value[0] = fec_id;
    ent_info.entry_qual[3].mask[0] = 0xfffff;

    ent_info.nof_entry_actions = 1; /* tcam_fg_id.nof_actions */
    ent_info.entry_action[0].type = bcmFieldActionSnoop;
    ent_info.entry_action[0].value[0] = g_sllb_field_data.sllb_snoop_gport;
    ent_info.entry_action[0].value[1] = 0x44;

    rv = bcm_field_entry_add(unit, 0, g_sllb_field_data.tcam_fg_id, &ent_info, &g_sllb_field_data.sllb_server_fec_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_member_reference_tcam_entry_add: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    return rv;
}

/*
 * Create the snoop destination for VIP packets snooped from the PMF TCAM entries
 */
int sllb_snoop_config(int unit, bcm_gport_t * snoop_gport)
{
    int rv = BCM_E_NONE;
    bcm_mirror_destination_t mirror_dest;
    bcm_rx_trap_config_t trap_config;

    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.sample_rate_dividend = 1;
    mirror_dest.sample_rate_divisor = 1;
    BCM_GPORT_LOCAL_SET(mirror_dest.gport, BCM_GPORT_LOCAL_CPU);
    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_config: bcm_mirror_destination_create failed, rv %l", rv);
        return rv;
    }

    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = BCM_GPORT_LOCAL_CPU;
    rv = bcm_mirror_header_info_set(unit, (BCM_MIRROR_DEST_IS_SNOOP|BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER), mirror_dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_config: bcm_mirror_header_info_set failed, rv %l", rv);
        return rv;
    }

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &g_sllb_field_data.snoop_trap_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_config: bcm_rx_trap_type_create failed, rv %l", rv);
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id);
    rv = bcm_rx_trap_set(unit, g_sllb_field_data.snoop_trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_snoop_config: bcm_rx_trap_set failed, rv %l", rv);
        return rv;
    }
    BCM_GPORT_TRAP_SET(snoop_gport, g_sllb_field_data.snoop_trap_id, 0, 7);

    g_sllb_field_data.mirror_dest_id = mirror_dest.mirror_dest_id;
    return rv;
}

/*
 * Application example to snoop packets to support PCC Table SW learning and aging
 */
int sllb_field_example(int unit)
{
    int rv = BCM_E_NONE;

    rv = sllb_snoop_config(unit, &g_sllb_field_data.sllb_snoop_gport);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example: sllb_snoop_config failed, rv %l", rv);
        return rv;
    }

    rv = sllb_create_context(unit, &g_sllb_field_data.ipv4_context_id, &g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example: sllb_create_context failed, rv %l", rv);
        return rv;
    }

    rv = sllb_stamp_udh_ipmf2_de(unit, g_sllb_field_data.ipv4_context_id, g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example: sllb_stamp_udh_ipmf2_de failed, rv %l", rv);
        return rv;
    }

    rv = sllb_snoop_ipmf1_tcam_fg(unit, g_sllb_field_data.ipv4_context_id, g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example: sllb_snoop_ipmf1_tcam_fg failed, rv %l", rv);
        return rv;
    }

    rv = sllb_fg_entry_tcp_term_add(unit, 0xfffffff);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example: sllb_fg_entry_tcp_term_add failed, rv %l", rv);
        return rv;
    }

    return rv;
}


/*
 * Remove the TCP FIN/RST TCAM entries
 */
int sllb_fg_entry_tcp_term_remove(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, g_sllb_field_data.tcam_fg_id, NULL, g_sllb_field_data.sllb_tcp_fin_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_fg_entry_tcp_term_remove: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_entry_delete(unit, g_sllb_field_data.tcam_fg_id, NULL, g_sllb_field_data.sllb_tcp_rst_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_fg_entry_tcp_term_remove: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    return rv;
}

/*
 * Remove the member reference entries
 */
int sllb_member_reference_tcam_entry_remove(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, g_sllb_field_data.tcam_fg_id, NULL, g_sllb_field_data.sllb_member_reference_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_member_reference_tcam_entry_remove: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    return rv;
}

/*
 * Remove the server FEC_ID entries
 */
int sllb_server_fec_tcam_entry_remove(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_entry_delete(unit, g_sllb_field_data.tcam_fg_id, NULL, g_sllb_field_data.sllb_server_fec_tcam_entry_handle);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_server_fec_tcam_entry_remove: bcm_field_entry_add failed, rv %l", rv);
        return rv;
    }
    return rv;
}



/* 
 * Destroy the user defined qualifiers created for this example
 */
int sllb_field_qualifier_destroy(int unit)
{
    int rv = BCM_E_NONE;

    rv = bcm_field_qualifier_destroy(unit, g_sllb_field_data.tcp_flags_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_qualifier_destroy: bcm_field_qualifier_destroy failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, g_sllb_field_data.udh3_base_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_qualifier_destroy: bcm_field_qualifier_destroy failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, g_sllb_field_data.udh2_base_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_qualifier_destroy: bcm_field_qualifier_destroy failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_qualifier_destroy(unit, g_sllb_field_data.fec_id_qual_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_qualifier_destroy: bcm_field_qualifier_destroy failed, rv %l", rv);
        return rv;
    }
    return rv;
}

/*
 * Destroy the i]PMF1 contexts allocated for this example
 */
int sllb_destroy_context(int unit)
{
    int rv = BCM_E_NONE;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_presel_entry_data_t presel_entry_data;

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = g_sllb_field_data.ipv4_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    rv = bcm_field_presel_get(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_presel_get failed, rv %l", rv);
        return rv;
    }

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_presel_set failed, rv %l", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_id.presel_id = g_sllb_field_data.ipv6_presel_id;
    presel_entry_id.stage = bcmFieldStageIngressPMF1;
    rv = bcm_field_presel_get(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_presel_get failed, rv %l", rv);
        return rv;
    }

    presel_entry_data.entry_valid = FALSE;
    presel_entry_data.context_id = BCM_FIELD_CONTEXT_ID_DEFAULT;
    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_presel_set failed, rv %l", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, g_sllb_field_data.ipv4_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_context_destroy failed, rv %l", rv);
        return rv;
    }

    rv = bcm_field_context_destroy(unit, bcmFieldStageIngressPMF1, g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_destroy_context: bcm_field_context_destroy failed, rv %l", rv);
        return rv;
    }

    return rv;
}

/* 
 * Free all resources allocated for this example 
 */
int sllb_field_example_cleanup(int unit)
{
    int rv = BCM_E_NONE;

    /* Remove TCAM entries */
    rv = sllb_fg_entry_tcp_term_remove(unit);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: sllb_fg_entry_tcp_term_remove failed, rv %l", rv);
        return rv;
    }

    /* Detach FGs from contexts */
    rv = bcm_field_group_context_detach(unit, g_sllb_field_data.tcam_fg_id, g_sllb_field_data.ipv4_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_context_detach failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, g_sllb_field_data.tcam_fg_id, g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_context_detach failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, g_sllb_field_data.de_fg_id, g_sllb_field_data.ipv4_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_context_detach failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_context_detach(unit, g_sllb_field_data.de_fg_id, g_sllb_field_data.ipv6_context_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_context_detach failed, rv %l", rv);
        return rv;
    }

    /* Delete field groups */
    rv = bcm_field_group_delete(unit, g_sllb_field_data.tcam_fg_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_delete failed, rv %l", rv);
        return rv;
    }
    rv = bcm_field_group_delete(unit, g_sllb_field_data.de_fg_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_field_group_delete failed, rv %l", rv);
        return rv;
    }

    /* Destroy contexts */
    rv = sllb_destroy_context(unit);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: sllb_destroy_context failed, rv %l", rv);
        return rv;
    }

    /* Delete user defined qualifiers */
    rv = sllb_field_qualifier_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: sllb_field_qualifier_destroy failed, rv %l", rv);
        return rv;
    }

    /* Delete Snoop code */
    rv = bcm_rx_trap_type_destroy(unit, g_sllb_field_data.snoop_trap_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_rx_trap_type_destroy failed, rv %l", rv);
        return rv;
    }

    /* Delete mirror destination */
    rv = bcm_mirror_destination_destroy(unit, g_sllb_field_data.mirror_dest_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_field_example_cleanup: bcm_mirror_destination_destroy failed, rv %l", rv);
        return rv;
    }
    return rv;
}


