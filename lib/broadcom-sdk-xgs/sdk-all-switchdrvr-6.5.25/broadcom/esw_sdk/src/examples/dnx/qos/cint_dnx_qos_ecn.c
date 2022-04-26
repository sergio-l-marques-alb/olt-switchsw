/*
 * $Id$
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$ 
 *
 *File: cint_dnx_qos_ecn.c
 * Purpose: An example of QOS ECN mapping
 *
 * 1. This examples create ingress ECN QOS profile and add mapping, 
 *       It enables ECN and sets extraction of ECN (Eligible and congestion bits). 
 * 2. This examples create egress remark profile and add mapping with ecn.
 *       It remark output packet's QOS field and update ECN bits with FTMH.CNI and FTMH.ECN-Eligible. 
 * 3. This examples also configure IPV4 forwarding and
 *       apply ingress ECN mapping and egress remark mapping with ECN for output packet QOS.
 *
 *
 * Set up sequence:
 *    1. create ingress ECN profile and add mapping
 *    2. create egress remark profile and add mapping with ecn
 *    3. create IPv4 forward service
 *    4. update qos profile of IPv4 forward service 
 *
 * run traffic: 
 *     IPV4 header with random TOS
 * expected: 
 *     IPV4 header TOS is same as input without application ECN profile at ingress
 *     IPV4 header TOS ECN bits are 3 with application ECN profile at ingress
 * 
 *
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 *    cd ../../../..
 *    cint src/examples/sand/cint_ip_route_basic.c
 *    cint src/examples/dnx/qos/cint_dnx_qos_ecn.c
 *    cint
 *    dnx_ecn_basic_example(unit, <in_port>, <out_port>);
 */

static int in_ecn_map_id=-1;
static int eg_map_id=-1;

static int in_ecn_opcode;
static int eg_ecn_opcode;
static int eg_remark_opcode;

int
qos_map_id_egress_get(int unit)
{
    return eg_map_id;
}

int
qos_ecn_map_id_ingress_get(int unit)
{
    return in_ecn_map_id;
}

/**create egress qos remark profile, add remark mapping without/with ecn,
   TOS 1:1 mapping, output ECN bits should be updated when ingress ECN eligible enabled*/
int
qos_map_egress_remark_without_and_with_ecn(int unit)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int dscp;
    int dp;
    int rv;
 
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK;
    if(eg_map_id !=-1)
    {
        flags |= BCM_QOS_MAP_WITH_ID;
    }
    if(eg_map_id != 0)
    {
       rv = bcm_qos_map_create(unit, flags, &eg_map_id);
    }
    {
        rv = bcm_qos_map_id_get_by_profile(unit, flags, eg_map_id, &eg_map_id);
    }
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress qos profile create\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE | BCM_QOS_MAP_ECN;
    rv = bcm_qos_map_create(unit, flags, &eg_ecn_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_create(unit, flags, &eg_remark_opcode);
    if (rv != BCM_E_NONE) {
        printf("error in ingress l3 opcode bcm_qos_map_create()\n");
        return rv;
    }

    /*
     *start add mapping without ecn
     */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = eg_remark_opcode;
    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    /*add mapping */
    for (dscp=0; dscp<256; dscp++) {
        for (dp=0; dp < 4; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.int_pri = dscp;
            if (dscp < 128) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK;
            }

            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
            }
            /* Set egress DSCP */
            l3_eg_map.dscp = dscp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_remark_opcode);
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 return rv;
            }
        }
    }

    /*
     *end mapping without ecn, start mapping with ecn
     */
    bcm_qos_map_t_init(&l3_eg_map);
    l3_eg_map.opcode = eg_ecn_opcode;
    /** Call map add with BCM_QOS_MAP_OPCODE flag in order to register the internal map ID */ 
    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN | BCM_QOS_MAP_OPCODE;
    rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in L3 egress bcm_qos_map_add()\n");
        return rv;
    }

    /*add qos 1:1 mapping */
    for (dscp=0; dscp<256; dscp++) {
        for (dp=0; dp < 4; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);

            l3_eg_map.int_pri = dscp;
            if (dscp < 128) {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV4 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN;
            } else {
                flags = BCM_QOS_MAP_L3 | BCM_QOS_MAP_IPV6 | BCM_QOS_MAP_REMARK | BCM_QOS_MAP_ECN;
            }

            /* Set internal color */
            if (dp == 0){
                l3_eg_map.color = bcmColorGreen;
            } else {
                l3_eg_map.color = bcmColorYellow;
            }
            /* Set egress DSCP */
            l3_eg_map.dscp = dscp;
            rv = bcm_qos_map_add(unit, flags, &l3_eg_map, eg_ecn_opcode);
            if (rv != BCM_E_NONE) {
                 printf("error in L3 egress bcm_qos_map_add()\n");
                 return rv;
            }
        }
    }

    /*
     *end mapping with ecn
     */
    return rv;
}

/*IP route ecn example main*/
int dnx_ecn_basic_example(int unit, int in_port, int out_port, int egress_qos_profile)
{
    int rv = BCM_E_NONE;

    if (egress_qos_profile != -1)
    {
        eg_map_id = egress_qos_profile;
    }

    rv = qos_map_egress_remark_without_and_with_ecn(unit);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }
  
    rv = basic_example_qos(unit, in_port, out_port, 0, egress_qos_profile);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }

    printf("dnx_ecn_basic_example: PASS\n\n");
    return rv;
}

/*ip tunnel encap ecn example main*/
int dnx_ecn_ip_tunnel_encap_example(int unit, int in_port, int out_port, int egress_qos_profile)
{
    int rv = BCM_E_NONE;

    if (egress_qos_profile != -1)
    {
        eg_map_id = egress_qos_profile;
    }

    rv = qos_map_egress_remark_without_and_with_ecn(unit);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }
  
    rv = ip_tunnel_encap_vxlan_gpe_basic(unit, in_port, out_port);
    if (rv) {
        printf("error setting up ip tunnel\n");
        return rv;
    }

    rv = ip_tunnel_encap_basic_qos_update(unit, eg_map_id, bcmQosEgressModelPipeNextNameSpace, 32, 0);
    if (rv) {
        printf("error ip_tunnel_encap_basic_qos_update\n");
        return rv;
    }
    return rv;
}

/*ip tunnel termination ecn example main*/
int dnx_ecn_ip_tunnel_term_example(int unit, int in_port, int out_port, int egress_qos_profile)
{
    int rv = BCM_E_NONE;

    if (egress_qos_profile != -1)
    {
        eg_map_id = egress_qos_profile;
    }

    rv = qos_map_egress_remark_without_and_with_ecn(unit);
    if (rv) {
        printf("error setting up egress qos profile\n");
        return rv;
    }
  
    rv = ip_tunnel_term_vxlan_gpe_example(unit, in_port, out_port);
    if (rv) {
        printf("error setting up ip tunnel\n");
        return rv;
    }

    rv = ip_tunnel_term_update_egress_rif_with_qos(unit, eg_map_id);
    if (rv) {
        printf("error ip_tunnel_term_update_egress_rif_with_qos\n");
        return rv;
    }
    return rv;
}

/*
 * For L3 forwarding, update ecn profile in IN-RIF, so that ingress ecn mapping enabled
 * For L2 forwarding, use PMF to set ecn eligible
 */
int dnx_ecn_ingress_eligible_set(int unit, int is_ecn_eligible, int is_L3, int in_port)
{
    int rv;
    
    if (is_L3)
    {
        int intf_in = 15;           /* Incoming packet ETH-RIF */
        int vrf = 1;
        bcm_l3_intf_t l3_intf;

        l3_intf.l3a_intf_id = intf_in;
        rv = bcm_l3_intf_get(unit, &l3_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_get\n");
            return rv;
        }
        l3_intf.l3a_flags |= BCM_L3_REPLACE | BCM_L3_WITH_ID;  
        l3_intf.ingress_qos_model.ingress_ecn = is_ecn_eligible;
    
        rv = bcm_l3_intf_create(unit, &l3_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l3_ingress_create\n");
            return rv;
        }
    }
	else
	{
	    printf("dnx_ecn_ingress_eligible_set L2\n\n");
        uint32 ecn_lif_profile = 2;
        bcm_field_context_t context_id;
        bcm_field_context_info_t context_info;
        bcm_field_presel_entry_id_t presel_entry_id;
        bcm_field_presel_entry_data_t presel_entry_data;
        bcm_field_context_param_info_t param_info;
        void *dest_char;

        /**set recycle port profile*/
        bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, ecn_lif_profile);

        /********* Create PRESELECTOR *************/
        bcm_field_context_info_t_init(&context_info);
        dest_char = &(context_info.name[0]);
        sal_strncpy_s(dest_char, "QOS ECN", sizeof(context_info.name));
        rv = bcm_field_context_create(unit, 0, bcmFieldStageIngressPMF1, &context_info, &context_id);
        if(rv != BCM_E_NONE) {
            printf("dnx_qos_l3_dscp_preserve_per_inlif_set: Error in bcm_field_context_create context %d\n", context_id);
            return rv;
        }

        bcm_field_presel_entry_id_info_init(&presel_entry_id);
        presel_entry_id.presel_id = 58;
        presel_entry_id.stage = bcmFieldStageIngressPMF1;

        bcm_field_presel_entry_data_info_init(&presel_entry_data);
        presel_entry_data.nof_qualifiers = 1;
        presel_entry_data.context_id = context_id;
        presel_entry_data.entry_valid = TRUE;
        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyPortClassPacketProcessing;
        presel_entry_data.qual_data[0].qual_value = ecn_lif_profile;
        presel_entry_data.qual_data[0].qual_mask = 0x7;
        rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
        if(rv != BCM_E_NONE) {
            printf("cint_field_evpn_context_create Error in bcm_field_presel_set  \n");
            return rv;
        }

        /********* Add QUALIFIERS *********/
        bcm_field_group_info_t fg_info;
        int qual_and_action_index;
        char *proc_name;
        void *dst_char;
        bcm_field_group_t ingress_fg_id;
        bcm_field_qualify_t ecn_qual_id;
        proc_name = "field_ecn_remark";

        /** Init the fg_info structure. */
        bcm_field_group_info_t_init(&fg_info);
        /*
         * Create user define qualifier
         */
        bcm_field_qualifier_info_create_t qual_info;
        bcm_field_qualifier_info_create_t_init(&qual_info);
        qual_info.size = 32;
        dst_char = &(qual_info.name[0]);
        sal_strncpy_s(dst_char, "ECN_REMARK", sizeof(qual_info.name));
        rv = bcm_field_qualifier_create(unit, 0, &qual_info, &ecn_qual_id);
        if(rv != BCM_E_NONE) {
            printf("%s Error in bcm_field_qualifier_create qual_id_fai_2_0\n", proc_name);
            return rv;
        }

        /*
         *  Fill the fg_info structure, Add the FG. 
         */
        fg_info.stage = bcmFieldStageIngressPMF1;
        fg_info.fg_type = bcmFieldGroupTypeTcam;
        fg_info.nof_quals = 1;
        fg_info.qual_types[0] = bcmFieldQualifyInVPort0;
        fg_info.nof_actions = 1;
        fg_info.action_types[0] = bcmFieldActionEcnNew;
        fg_info.action_with_valid_bit[0] = TRUE;
        rv = bcm_field_group_add(unit, 0, &fg_info, &ingress_fg_id);
        if (rv != BCM_E_NONE) {
            printf("%s Error (%d), in bcm_field_group_add\n", proc_name, rv);
            return rv;
        }

        /*
         * Attach the field group ID to the context.
         */
        int qual_idx = 0;
        int act_idx = 0;
        bcm_field_group_attach_info_t attach_info;
        bcm_field_qualifier_info_get_t qual_info_get;
        bcm_field_entry_info_t entry_info;
        bcm_field_entry_t entry_handle;

        rv = bcm_field_qualifier_info_get(unit, bcmFieldQualifyInVPort0, bcmFieldStageIngressPMF1, &qual_info_get);
        if (rv != BCM_E_NONE)
        {
            printf("%s Error (%d), in bcm_field_qualifier_info_get\n", proc_name, rv);
            return rv;
        }
        bcm_field_group_info_t_init(&fg_info);
        bcm_field_group_info_get(unit, ingress_fg_id, &fg_info);

        bcm_field_group_attach_info_t_init(&attach_info);
        attach_info.key_info.nof_quals = fg_info.nof_quals;
        attach_info.payload_info.nof_actions = fg_info.nof_actions;
        attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
        attach_info.payload_info.action_types[0] = fg_info.action_types[0];
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
        attach_info.key_info.qual_info[0].input_arg = 0; 
        attach_info.key_info.qual_info[0].offset = qual_info_get.offset;

        rv = bcm_field_group_context_attach(unit, 0, ingress_fg_id, context_id, &attach_info);
        if(rv != BCM_E_NONE) {
            printf("Error in bcm_field_group_context_attach fg_id %d context_id %d\n", ingress_fg_id, context_id);
            return rv;
        }

        bcm_field_entry_info_t_init(&entry_info);
        entry_info.nof_entry_quals = fg_info.nof_quals;
        entry_info.nof_entry_actions = fg_info.nof_actions;
        entry_info.entry_qual[0].type = fg_info.qual_types[0];
        entry_info.entry_qual[0].value[0] = g_l2_basic_bridge.inlif & 0x3FFFFF;
        entry_info.entry_action[0].type = fg_info.action_types[0];
        entry_info.entry_action[0].value[0] = is_ecn_eligible;
        rv = bcm_field_entry_add(unit, 0, ingress_fg_id, &entry_info, &entry_handle);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add\n", rv);
            return rv;
        }
        return rv;  
	}
    printf("dnx_ecn_ingress_eligible_set: PASS\n\n");
    return rv;
}

/*
 * For L2 forwarding, fwd+1 ECN remark, update out AC qos model
 */
int dnx_ecn_egress_layer_eligible_set(int unit, int ecn_eligible)
{
    bcm_vlan_port_t vlan_port;
    int rv = BCM_E_NONE;

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vlan_port_id = g_l2_basic_bridge.outlif;
    rv =  bcm_vlan_port_find(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vlan_port_find\n", rv);
        return rv;
    }
    vlan_port.flags |= (BCM_VLAN_PORT_WITH_ID | BCM_VLAN_PORT_REPLACE);
    vlan_port.egress_qos_model.egress_ecn = ecn_eligible;
    rv =  bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    return rv;
}
