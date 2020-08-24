/*
 * $Id: cint_dnx_coe_basic.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 * File: cint_dnx_coe_basic.c
 * Purpose: Example of COE basic configuration.
 */

/*
 * This cint demonstrate configuration of COE.
 *
 * SOC:
 *      ucode_port_0.BCM8869X=CPU.0:core_0.0
 *      ucode_port_200.BCM8869X=CPU.8:core_1.200
 *      ucode_port_201.BCM8869X=CPU.16:core_0.201
 *      ucode_port_202.BCM8869X=CPU.24:core_0.202
 *      ucode_port_203.BCM8869X=CPU.32:core_0.203
 *
 * Usage:
 *
 * cint ../../../../src/examples/dpp/cint_port_extender_dynamic_switching.c
 * cint ../../../../src/examples/dnx/coe/cint_dnx_coe_basic.c
 * cint
 * int unit=0;
 * uint32 pp_port, flags;
 * bcm_port_interface_info_t interface_info;
 * bcm_port_mapping_info_t mapping_info;
 *
 * non_coe_port=200;
 * non_coe_vlan=5;
 * print bcm_port_get(unit,201,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(0,201,mapping_info.pp_port,10,0);
 * print bcm_port_get(unit,202,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(1,202,mapping_info.pp_port,20,0);
 * print bcm_port_get(unit,203,&flags,&interface_info,&mapping_info);
 * print port_extender_dynamic_swithing_coe_info_init_multi_flows(2,203,mapping_info.pp_port,30,0);
 * print port_dnx_coe_basic_service(0);
 *
 * print bcm_vlan_gport_add(unit, 15, 201, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * print bcm_vlan_gport_add(unit, 25, 202, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * print bcm_vlan_gport_add(unit, 35, 203, BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 */

int non_coe_port;
bcm_vlan_t non_coe_vlan;


 /*
  * Set coe port mapping. 
  */
 int port_dnx_coe_config(int unit, int coe_vlan, bcm_port_t coe_port, int pp_port, bcm_port_t phy_port)
 {
     int rv = 0;
     bcm_port_extender_mapping_info_t mapping_info;
     bcm_gport_t sys_gport;
 
     mapping_info.vlan = coe_vlan;
     mapping_info.pp_port = pp_port;
     mapping_info.phy_port = phy_port;
 
     /* 
      * Map port and COE VLAN ID to in pp port.
      * Add encapsulation VID.
      */
     rv = bcm_port_extender_mapping_info_set(
                unit,
                BCM_PORT_EXTENDER_MAPPING_INGRESS | BCM_PORT_EXTENDER_MAPPING_EGRESS,
                bcmPortExtenderMappingTypePortVlan,
                &mapping_info);
     if (rv != BCM_E_NONE) {
         printf("bcm_port_extender_mapping_info_set failed $rv\n");
         return rv;
     }
 
     /* 
      * Set the system port of the in pp port
      */
     BCM_GPORT_SYSTEM_PORT_ID_SET(&sys_gport, coe_port);
     rv = bcm_stk_sysport_gport_set(unit, sys_gport, coe_port);
     if (rv != BCM_E_NONE) {
         printf("bcm_stk_sysport_gport_set failed $rv\n");
         return rv;
     }
     
     return rv;
 }


 int port_dnx_create_p2p_service(int unit, int p1, bcm_vlan_t vlan1, int p2, bcm_vlan_t vlan2)
 {
     int rv;
     bcm_vlan_port_t vlan_port_1;
     bcm_vlan_port_t vlan_port_2;
     bcm_vswitch_cross_connect_t gports;
     
     bcm_vlan_port_t_init(&vlan_port_1);
     bcm_vlan_port_t_init(&vlan_port_2);
     vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
     vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;

     vlan_port_1.flags = 0;
     vlan_port_2.flags = 0;
     
     vlan_port_1.port = p1;
     vlan_port_1.match_vlan = vlan1;
     
     vlan_port_2.port = p2;
     vlan_port_2.match_vlan = vlan2;
     
     rv = bcm_vlan_port_create(0, &vlan_port_1);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

     rv = bcm_vlan_port_create(0, &vlan_port_2);
     if (rv != BCM_E_NONE) {
         printf("bcm_vlan_port_create failed!\n");
         return rv;
     }

 
     bcm_vswitch_cross_connect_t_init(&gports);
     gports.port1= vlan_port_1.vlan_port_id;
     gports.port2= vlan_port_2.vlan_port_id;
     rv = bcm_vswitch_cross_connect_add(unit, &gports);
     if (rv != BCM_E_NONE) {
         printf("P2P service create failed!\n");
         return rv;
     }

     return rv;

 }


 int port_dnx_coe_basic_service(int unit)
 {
    int rv = 0;
    int index;

    /*
     * init COE mapping info(at least 3 COE port)
     * Calling port_extender_dynamic_swithing_coe_info_init_multi_flows() for each coe port mapping.
     */

    /** enable port and set mapping*/
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 1);
            if (rv != BCM_E_NONE) {
                printf("Enable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
            
            rv = port_dnx_coe_config(
                        unit,
                        coe_port_info_multi_flows[index].coe_vlan,
                        coe_port_info_multi_flows[index].coe_port,
                        coe_port_info_multi_flows[index].pp_port,
                        coe_port_info_multi_flows[index].phy_port);
            if (rv != BCM_E_NONE) {
                printf("Mapping for COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }

    /*
     * create P2P service 
     *      1: coe_port <---> coe_port
     *      2: regular port <---> coe_port
     */
    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[0].coe_port,
            coe_port_info_multi_flows[0].coe_vlan+5,
            coe_port_info_multi_flows[1].coe_port,
            coe_port_info_multi_flows[1].coe_vlan+5);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->COE failed!\n");
        return rv;
    }

    rv = port_dnx_create_p2p_service(
            unit,
            coe_port_info_multi_flows[2].coe_port,
            coe_port_info_multi_flows[2].coe_vlan+5,
            non_coe_port,
            non_coe_vlan);
    if (rv != BCM_E_NONE) {
        printf("Create P2P service for COE<-->Regular failed!\n");
        return rv;
    }
    return rv;
 }


int port_dnx_coe_basic_service_clean_up(int unit)
{
    int rv = 0;
    int index;

    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = bcm_port_control_set(unit, coe_port_info_multi_flows[index].coe_port, bcmPortControlExtenderEnable, 0);
            if (rv != BCM_E_NONE) {
                printf("Disable COE port[%d] failed!\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }


    return rv;
}


/*
 * EPMF and COE port setting for Injected packet with only IMTH:
 *   print cint_tm_to_coe_main_config(0);  
 */
uint32 pmf_profile = 3;
bcm_field_presel_t presel_id = 62;

int cint_tm_to_coe_context_create(
    int unit,
    int app_type,
    bcm_field_context_t *context_id)
{
    void *dest_char;
    bcm_field_presel_entry_data_t p_data;
    bcm_field_presel_entry_id_t p_id;
    bcm_field_context_info_t context_info;
    int rv;

    bcm_field_context_info_t_init(&context_info);
    dest_char = &(context_info.name[0]);
    sal_strncpy_s(dest_char, "TM_to_COE_ePMF_CTX", sizeof(context_info.name));
    rv = bcm_field_context_create(unit, 0, bcmFieldStageEgress, &context_info, context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_context_create\n", rv);
        return rv;
    }

    bcm_field_presel_entry_id_info_init(&p_id);
    bcm_field_presel_entry_data_info_init(&p_data);

    p_id.presel_id = presel_id;
    p_id.stage = bcmFieldStageEgress;
    p_data.entry_valid = TRUE;
    p_data.context_id = *context_id;
    p_data.nof_qualifiers = 2;

    p_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    p_data.qual_data[0].qual_arg = 0;
    p_data.qual_data[0].qual_value = pmf_profile;
    p_data.qual_data[0].qual_mask = 0x7;

    p_data.qual_data[1].qual_type = bcmFieldQualifyContextId;
    p_data.qual_data[1].qual_arg = 0;
    if (app_type == 0) 
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextTm;
    }
    else if (app_type == 1)
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextIngressTrapLegacy;
    }
    else if (app_type == 2)
    {
        p_data.qual_data[1].qual_value = bcmFieldForwardContextMirrorOrSs;
    }

    p_data.qual_data[1].qual_mask = 0x3F;
    
    /** PPH doesn't exist */
    /*
    p_data.qual_data[1].qual_type = bcmFieldQualifyPphPresent;
    p_data.qual_data[1].qual_arg = 0;
    p_data.qual_data[1].qual_value = 0;
    p_data.qual_data[1].qual_mask = 1;
    */

    rv = bcm_field_presel_set(unit, 0, &p_id, &p_data);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_presel_set\n", rv);
        return rv;
    }

    return rv;
}

int cint_tm_to_coe_fp_create(
    int unit,
    bcm_field_context_t context_id,
    bcm_field_group_t * fg_id,
    bcm_field_group_info_t * fg_info,
    uint32 *ace_entry_handle)
{
    bcm_field_group_attach_info_t attach_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_format_t ace_format_id;
    int qual_index, action_index;
    void *dest_char;
    int rv;

    /** define ace action */
    bcm_field_ace_format_info_t_init(&ace_format_info);
    ace_format_info.nof_actions = 1;
    ace_format_info.action_types[0] = bcmFieldActionAceContextValue;
    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
    }
    
    bcm_field_ace_entry_info_t_init(&ace_entry_info);
    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;
    ace_entry_info.entry_action[0].type = bcmFieldActionAceContextValue;
    ace_entry_info.entry_action[0].value[0] = bcmFieldAceContextTmToCOE;
    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /** Field Group: to set Context value */
    bcm_field_group_info_t_init(fg_info);
    fg_info->fg_type = bcmFieldGroupTypeTcam;
    fg_info->stage = bcmFieldStageEgress;
    fg_info->nof_quals = 1;
    fg_info->qual_types[0] = bcmFieldQualifyDstPort;

    fg_info->nof_actions = 1;
    fg_info->action_types[0] = bcmFieldActionAceEntryId;
    dest_char = &(fg_info->name[0]);
    sal_strncpy_s(dest_char, "ACE ctxt value: TM to COE", sizeof(fg_info->name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_add failed - Map SSP to TRAP\n");
        return rv;
    }

    /** attach program to given context */
    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info->nof_quals;
    attach_info.payload_info.nof_actions = fg_info->nof_actions;

    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        attach_info.key_info.qual_types[qual_index] = fg_info->qual_types[qual_index];
        attach_info.key_info.qual_info[qual_index].input_type = bcmFieldInputTypeMetaData;
    }

    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        attach_info.payload_info.action_types[action_index] = fg_info->action_types[action_index];
    }
    rv = bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_attach failed \n");
        return rv;
    }
    
    return rv;
}


int cint_tm_to_coe_fp_entry_add(
    int unit,
    bcm_field_group_t fg_id,
    bcm_field_group_info_t *fg_info,
    uint32 ace_entry_handle)
{
    int qual_index, action_index;
    bcm_field_entry_info_t entry_info;
    bcm_field_entry_t entry_handle;
    int rv;
    
    /** add entry */
    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = fg_info->nof_quals;
    entry_info.nof_entry_actions = fg_info->nof_actions;
    
    for (qual_index = 0; qual_index < fg_info->nof_quals; qual_index++)
    {
        entry_info.entry_qual[qual_index].type = fg_info->qual_types[qual_index];
    }

    for (action_index = 0; action_index < fg_info->nof_actions; action_index++)
    {
        entry_info.entry_action[action_index].type = fg_info->action_types[action_index];
    }
    /** don't care of key */
    entry_info.entry_qual[0].value[0] = 0;
    entry_info.entry_qual[0].mask[0] = 0;
    entry_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, &entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    
    return rv;
}



int cint_tm_to_coe_port_config(
    int unit,
    bcm_port_t coe_port)
{
    int rv;

    /** set epmf selection: pmf_profile(TM_to_CoE_Temp) */
    rv = bcm_port_class_set(unit, coe_port, bcmPortClassFieldEgressPacketProcessingPortCs, pmf_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_port_class_set failed\n");
        return rv;
    }

    return rv;
}


/*
 * Main configuration
 * app_type == 2: config for Mirror application
 * app_type == 1: config for Trap application
 * app_type == 0: config for TM application
 */
int cint_tm_to_coe_main_config(int unit, int app_type)
{
    bcm_field_context_t context_id;
    bcm_field_group_t fg_id;
    bcm_field_group_info_t fg_info;
    uint32 ace_entry_handle;
    int rv, index;

    /** PMF configure */
    rv = cint_tm_to_coe_context_create(unit, app_type, &context_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_context_create failed\n");
        return rv;
    }
    rv = cint_tm_to_coe_fp_create(unit, context_id, &fg_id, &fg_info, &ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_fp_create failed\n");
        return rv;
    }
    rv = cint_tm_to_coe_fp_entry_add(unit, fg_id, &fg_info, ace_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_tm_to_coe_fp_entry_add failed\n");
        return rv;
    }

    /** COE port setting */
    for (index = 0; index < nof_coe_ports_multi_flows; index++) {
        if (coe_port_info_multi_flows[index].coe_port != 0) {
            rv = cint_tm_to_coe_port_config(unit, coe_port_info_multi_flows[index].coe_port);
            if (rv != BCM_E_NONE)
            {
                printf("Error in cint_tm_to_coe_port_config failed, COE port[%d]\n", coe_port_info_multi_flows[index].coe_port);
                return rv;
            }
        }
    }
    return rv;

}


bcm_gport_t mirror_dest_id;

/*
 * ingress mirror to COE port should be different: need to append new system header
 * flag: BCM_MIRROR_PORT_INGRESS / BCM_MIRROR_PORT_EGRESS
 */
int cint_dnx_mirror_to_coe_mirror_create(int unit, bcm_port_t mir_src, bcm_port_t mir_dest, int flags)
{
    int rv;
    bcm_mirror_destination_t dest;
    bcm_mirror_header_info_t mirror_header_info;

    bcm_mirror_destination_t_init(&dest);

    /* check the flags parameter, it must be BCM_MIRROR_PORT_INGRESS or BCM_MIRROR_PORT_EGRESS */
    if (flags != BCM_MIRROR_PORT_INGRESS && flags != BCM_MIRROR_PORT_EGRESS)
    {
        printf("Only support flag: BCM_MIRROR_PORT_INGRESS  or BCM_MIRROR_PORT_EGRESS\n");
        return BCM_E_PARAM;
    }

    BCM_GPORT_LOCAL_SET(dest.gport, mir_dest);
    rv = bcm_mirror_destination_create(unit, &dest);
    if(rv != BCM_E_NONE)
    {
        printf("failed to create mirror destination , return value %d\n", rv);
        return rv;
    }
    
    mirror_dest_id = dest.mirror_dest_id;
    
    rv = bcm_mirror_port_dest_add(unit, mir_src, flags, mirror_dest_id);
    if(rv != BCM_E_NONE)
    {
        printf("failed to add mir_src in bcm_mirror_port_dest_add\n");
        return rv;
    }

    /** for ingress mirror, should build new system header */
    if (flags == BCM_MIRROR_PORT_INGRESS) {
        bcm_mirror_header_info_t_init(&mirror_header_info);
        mirror_header_info.tm.src_sysport = mir_src;

        rv = bcm_mirror_header_info_set(unit, 0, mirror_dest_id, &mirror_header_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error: bcm_mirror_header_info_set %d\n", rv);
            return rv;
        }
    }

    return rv;
}


int cint_dnx_mirror_to_coe_mirror_destroy(int unit, int mir_src, uint32 flags)
{
    int rv;

    rv = bcm_mirror_port_dest_delete(unit, mir_src, flags, mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_destroy\n");
        return rv;
    }

    rv = bcm_mirror_destination_destroy(unit, mirror_dest_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_mirror_destination_destroy\n");
        return rv;
    }

    return rv;
}


/*
 *  Config L3 service(ipv4/ipv6/mpls) for mirror
 */
int cint_dnx_mirror_to_coe_l3_config(int unit, int vsi, int out_port)
{
    int rv;

    rv = inlif_large_wide_data_l3_fwd_config(unit, vsi, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in inlif_large_wide_data_l3_fwd_config\n", rv);
        return rv;
    }

    return rv;
}



/*
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dpp/utility/cint_utils_global.c
 * cint ../../src/./examples/dpp/cint_port_extender_dynamic_switching.c
 * cint ../../src/./examples/dnx/extender/cint_dnx_coe_basic.c
 * cint ../../src/./examples/dnx/cint_inlif_wide_data.c
 * cint
 * non_coe_port=16;
 * non_coe_vlan=5;
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(0,200,24,10,0);
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(1,14,1,20,14);
 * port_extender_dynamic_swithing_coe_info_init_multi_flows(2,15,2,30,15);
 * port_dnx_coe_basic_service(0);
 * bcm_vlan_gport_add(0,100,201,BCM_VLAN_GPORT_ADD_INGRESS_ONLY);
 * cint_dnx_mirror_to_coe_main(0,100,201,200,13,BCM_MIRROR_PORT_INGRESS);
 * exit;
 *
 * Sending IPv4 forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c0002000100000000002281000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a81000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending IPv6 forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c000200010000000000228100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a8100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending mpls forward packet for ingress mirror
 * tx 1 psrc=201 data=0x000c00020001000000000022810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000c000200010000000000228100000a810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 * cint
 * cint_dnx_mirror_to_coe_mirror_destroy(0,201,BCM_MIRROR_PORT_INGRESS);
 * cint_dnx_mirror_to_coe_mirror_create(0,13,200,BCM_MIRROR_PORT_EGRESS);
 * exit;
 *
 * Sending IPv4 forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c0002000100000000002281000064080045000035000000008000fb1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100000a810000c8080045000035000000007f00fc1dc0a800027fffff01000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending IPv6 forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c000200010000000000228100006486dd6000000000210680fe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100000a810000c886dd600000000021067ffe800000000000000000000000000001fe800000000000000000000000000001000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Sending mpls forward packet for egress mirror
 * tx 1 psrc=201 data=0x000c00020001000000000022810000648847003e83ff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * Data: 0x000102030405000c000200028100100a810010c88847007d03fe000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f2000000000000000
 *
 *
 */

/**Config for L3 service mirror test*/
int cint_dnx_mirror_to_coe_main(int unit, int vsi, bcm_port_t mir_src, bcm_port_t mir_dest, bcm_port_t fwd_dest, int flags)
{
    int rv;
    
    rv = cint_dnx_mirror_to_coe_l3_config(unit, vsi, fwd_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_mirror_to_coe_l3_config\n", rv);
        return rv;
    }

    rv = cint_dnx_mirror_to_coe_mirror_create(unit, mir_src, mir_dest, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_dnx_mirror_to_coe_mirror_create\n", rv);
        return rv;
    }

    rv = cint_tm_to_coe_main_config(unit, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in cint_tm_to_coe_main_config\n", rv);
        return rv;
    }
    
    return rv;

}

