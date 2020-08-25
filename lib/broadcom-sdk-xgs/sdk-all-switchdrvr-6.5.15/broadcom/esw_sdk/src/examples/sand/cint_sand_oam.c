/*
 * $Id: cint_sand_oam.c,v 1.34 11/1/2017 10:54:52 mm949600 Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */



int cross_connect_configuration = 0;
int is_offloaded = 0;

/*
 * OAM snoop configuration parameters
 */
struct oam_snoop_config_s
{
    /* JER1 & JER2 */
    bcm_gport_t dest_snoop_port;
    bcm_oam_opcode_t opcode;
    /* JER1 */
    bcm_oam_endpoint_t endpoint_id;
    bcm_oam_action_type_t action_type;
    /* JER2 */
    bcm_oam_dest_mac_type_t mac_type;
    uint8 counter_increment;
    bcm_oam_profile_t profile_id;
    int is_ingress;
};

/*
 * Used by oam_change_mep_destination_to_snoop
 * to create and configure an additional snoop
 */
oam_snoop_config_s snoop_config;

/*****************************************************************************/
/*                                        OAM BASIC EXAMPLE                                                                               */
/*****************************************************************************/

/**
 * Cint that only uses bcm APIs to create non accelerated Up,
 * Down MEP
 *
 *
 * @param unit
 * @param port1
 * @param vid1 - vlan ID of port1
 * @param port2
 * @param vid2 - vlan ID of port2
 *
 * @return int
 */
int oam_standalone_example_with_vlans(int unit,int port1, int vid1, int port2, int vid2) {

    bcm_error_t rv = BCM_E_NONE;
    bcm_vlan_port_t vp1, vp2;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    int md_level_1 = 2;
    int md_level_2 = 5;
    int lm_counter_base_id_1 = 0;
    int lm_counter_base_id_2 = 0;
    bcm_vswitch_cross_connect_t cross_connect;
    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    int counter_if_0 = 0;
    int counter_if_1 = 1;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    /* When oam_up_statistics_enable is enabled, engines are allocated for PMF*/
    if (oam_up_statistics_enable == 0 && (device_type != device_type_jericho2)) {
        /*Endpoints will be created on 2 different LIFs. */
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_1,port1);
        BCM_IF_ERROR_RETURN(rv);
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_2,port2);
        BCM_IF_ERROR_RETURN(rv);
    } 
    else if (device_type == device_type_jericho2)
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(unit, port_1, counter_if_0, &lm_counter_base_id_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_2, counter_if_1, &lm_counter_base_id_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /* Set port classification ID */
    rv = bcm_port_class_set(unit, port1, bcmPortClassId, port1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* Set port classification ID */
    rv = bcm_port_class_set(unit, port2, bcmPortClassId, port2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }


    bcm_vlan_port_t_init(&vp1);
    vp1.flags = BCM_VLAN_PORT_CROSS_CONNECT;
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    if (device_type != device_type_jericho2)
    {
        vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    vp1.port = port1;
    vp1.match_vlan = vid1;
    vp1.egress_vlan = vid1;
    rv=bcm_vlan_port_create(unit,&vp1);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp1.port, vp1.match_vlan,
           vp1.vlan_port_id, vp1.encap_id);
    rv = bcm_vlan_gport_add(unit, 10, port1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("(%s) - bcm_vlan_gport_add \n", bcm_errmsg(rv));
        return rv;
    }

    bcm_vlan_port_t_init(&vp2);
    vp2.flags = BCM_VLAN_PORT_CROSS_CONNECT;
    vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    if (device_type != device_type_jericho2)
    {
        vp2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    vp2.port = port2;
    vp2.match_vlan = vid2;
    vp2.egress_vlan = vid2;
    rv=bcm_vlan_port_create(unit,&vp2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("port=%d, vid = %d, vlan_port_id=0x%08x, encap_id=0x%04x\n", vp2.port, vp2.match_vlan,
           vp2.vlan_port_id, vp2.encap_id);

    /**
    * Cross-connect the ports
    */
    bcm_vswitch_cross_connect_t_init(&cross_connect);

    cross_connect.port1 = vp1.vlan_port_id;
    cross_connect.port2 = vp2.vlan_port_id;

    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

     bcm_oam_group_info_t_init(&group_info_short_ma);
     sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
     rv = bcm_oam_group_create(unit, &group_info_short_ma);
     if( rv != BCM_E_NONE) {
         printf("MEG:\t (%s) \n",bcm_errmsg(rv));
         return rv;
     }


  /*
   * Get default profile encoded values
   * Default profile is 1 and
   * configured on init(application) stage.
   * Relevant for Jer2 ONLY
   */
    if( device_type == device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(egress)\n");
          return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, vp1.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, 0, vp2.vlan_port_id, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
    }

    /* 
    * Adding non acc down MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = group_info_short_ma.id;
    mep_not_acc_info.level = md_level_1;
    mep_not_acc_info.gport = vp1.vlan_port_id;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    if(device_type == device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port1, mac_mep_1);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }
    else
    {
        sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
    }

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
    mep_not_acc_info.lm_counter_if = counter_if_0;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created Down MEP with id %d\n", mep_not_acc_info.id);
    ep1.gport = mep_not_acc_info.gport;
    ep1.id = mep_not_acc_info.id;
    ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;


    /**
    * Adding non acc UP MEP
    */
    bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
    mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_not_acc_info.group = group_info_short_ma.id;
    mep_not_acc_info.level = md_level_2;
    mep_not_acc_info.gport = vp2.vlan_port_id;
    mep_not_acc_info.tx_gport = BCM_GPORT_INVALID;
    mep_not_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_not_acc_info.name = 0;
    mep_not_acc_info.ccm_period = 0;

    if( device_type == device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }
    }
    else
    {
        sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
    }

    mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_2;
    mep_not_acc_info.lm_counter_if = counter_if_1;
    printf("bcm_oam_endpoint_create mep_not_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created UP MEP with id %d\n", mep_not_acc_info.id);
    ep2.gport = mep_not_acc_info.gport;
    ep2.id = mep_not_acc_info.id;
    ep2.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;

    return rv;
}
int oam_standalone_example(int unit,int port1, int port2) {
    return oam_standalone_example_with_vlans(unit, port1, 10, port2, 12);
}

/**
 * Basic OAM example.
 * Creates vswitch on 3 given ports and the folowing endpoint:
 * 1) Default endpoint
 * 2) Non accelerated down MEP on port1
 * 3) Accelerated up MEP on port2 + RMEP
 * 4) Accelerated down MEP on port1 + RMEP
 *
 *
 * @param unit
 * @param port1
 * @param port2
 * @param port3
 *
 * @return int
 */
int oam_run_with_defaults(int unit, int port1, int port2, int port3) {
    bcm_error_t rv;

    single_vlan_tag = 1;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    /*enable oam statistics per mep session*/
    if (oam_up_statistics_enable) {
        rv = oam_tx_up_stat(unit);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    printf("Registering OAM events\n");
    rv = register_events(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = oam_example(unit);
    return rv;
}


/**
 * Basic OAM example. creates the following entities:
 * 1) vswitch on which OAM endpoints are defined.
 * 2) OAM group with short MA name (set on outgoing CCMs for
 * accelerated endpoints).
 * 3) Default endpoint.
 * 4) Non accelerated endpoint
 * 5) Accelerated down MEP
 * 6) Accelerated up MEP
 *
 *
 * @param unit
 *
 * @return int
 */
int oam_example(int unit) {
    bcm_error_t rv;
    bcm_oam_group_info_t group_info_long_ma_test;
    bcm_oam_group_info_t *group_info;
    bcm_oam_endpoint_info_t mep_not_acc_info;
    bcm_oam_endpoint_info_t mep_not_acc_test_info;
    bcm_oam_endpoint_info_t default_info;

    int md_level_1 = 4;
    int md_level_2 = 5;
    int md_level_3 = 2;
    int md_level_4 = 1;
    int lm_counter_base_id_1;
    int lm_counter_base_id_2;
    int counter_if_0 = 0;
    int counter_if_1 = 1;

    uint32 flags=0;
    bcm_oam_profile_t ingress_profile = 1;
    bcm_oam_profile_t egress_profile  = 1;
    bcm_oam_profile_t acc_ingress_profile = 1;
    bcm_oam_profile_t acc_egress_profile = 1;

    int dev_type_val;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    printf("Creating two groups (short, long and 11b-maid name format)\n");
    bcm_oam_group_info_t_init(&group_info_long_ma);
    sal_memcpy(group_info_long_ma.name, long_name, BCM_OAM_GROUP_NAME_LENGTH);
    /* This is meant for QAX and above only */
    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv);
    /* SDK-119938
     * 48B MAID Jericho2 support
     */
    if (device_type >= device_type_qax && (device_type != device_type_jericho2)) {
        group_info_long_ma.group_name_index = 0x2014; /* Bank 8, entry 20 */
    }
    rv = bcm_oam_group_create(unit, &group_info_long_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    /* SDK-119938
     * 48B MAID Jericho2 support
     */
    if (device_type >= device_type_qax && (device_type != device_type_jericho2)) {
        group_info_short_ma.group_name_index = 0x2015; /* Bank 8, entry 21 */
    }
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    
    /* When oam_up_statistics_enable is enabled, engines are allocated for PMF*/
    if (oam_up_statistics_enable == 0 && (device_type != device_type_jericho2)) {
        /*Endpoints will be created on 2 different LIFs. */
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_2,port_2);
        BCM_IF_ERROR_RETURN(rv);
        rv = set_counter_source_and_engines(unit,&lm_counter_base_id_1,port_1);
        BCM_IF_ERROR_RETURN(rv);
    } else if (device_type == device_type_jericho2)
    {
        /*
         * Allocate counter engines, and get counters
         */
        rv = set_counter_resource(unit, port_1, counter_if_0, &lm_counter_base_id_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
        rv = set_counter_resource(unit, port_2, counter_if_1, &lm_counter_base_id_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error no %d, in set_counter_resource.\n", rv);
            return rv;
        }
    }

    /* 11b maid not supported in Jericho2 */
    if (use_11b_maid && (device_type != device_type_jericho2)) {
        /* Add a group with sttring based 11b MAID */
        bcm_oam_group_info_t_init(&group_info_11b_ma);
        sal_memcpy(group_info_11b_ma.name, str_11b_name, BCM_OAM_GROUP_NAME_LENGTH);
        rv = bcm_oam_group_create(unit, &group_info_11b_ma);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /* Set the used group for the MEPs to this group */
        group_info = &group_info_11b_ma;

    } else if (use_48_maid){
        /* Add a group with flexible 48 Byte MAID */
        bcm_oam_group_info_t_init(&group_info_48b_ma);
        sal_memcpy(group_info_48b_ma.name, str_48b_name, BCM_OAM_GROUP_NAME_LENGTH);
        group_info_48b_ma.flags = BCM_OAM_GROUP_FLEXIBLE_MAID_48_BYTE;
        if (device_type >= device_type_qax) {
            group_info_48b_ma.group_name_index = 0x2016; /* Bank 8, entry 22 */
        }
        if (device_type >= device_type_qux) {
            group_info_48b_ma.group_name_index = 0x669;
        }
        rv = bcm_oam_group_create(unit, &group_info_48b_ma);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        /* Set the used group for the MEPs to this group */
         group_info = &group_info_48b_ma;
    }
    else {
        /* Set the used group for the MEPs to the group with the short name */
        group_info = &group_info_short_ma;
    }

    bcm_oam_group_info_t_init(&group_info_long_ma_test);
    printf("bcm_oam_group_get\n");
    rv = bcm_oam_group_get(unit, group_info_long_ma.id, &group_info_long_ma_test);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    printf("bcm_oam_group_destroy\n");
    rv = bcm_oam_group_destroy(unit, group_info_long_ma.id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* SDK-119938
     * 48B Jericho2 support
     */
    if (device_type >= device_type_qax && (device_type != device_type_jericho2)) {
        group_info_long_ma.group_name_index = 0x2017; /* Bank 8, entry 23 */
    }
    rv = bcm_oam_group_create(unit, &group_info_long_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

  /*
   * Get default profile encoded values
   * Default profile is 1 and
   * configured on init(application) stage.
   * Relevant for Jer2 ONLY
   */
    if( device_type == device_type_jericho2)
    {
        rv = bcm_oam_profile_id_get_by_type(unit, ingress_profile, bcmOAMProfileIngressLIF, &flags, &ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, egress_profile, bcmOAMProfileEgressLIF, &flags, &egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(egress)\n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_ingress_profile, bcmOAMProfileIngressAcceleratedEndpoint, &flags, &acc_ingress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(accelerated ingress) \n");
          return rv;
        }

        rv = bcm_oam_profile_id_get_by_type(unit, acc_egress_profile, bcmOAMProfileEgressAcceleratedEndpoint, &flags, &acc_egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_profile_id_get_by_type(accelerated egress) \n");
          return rv;
        }

        rv = bcm_oam_lif_profile_set(unit, 0, gport1, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }
        rv = bcm_oam_lif_profile_set(unit, 0, gport2, ingress_profile, egress_profile);
        if (rv != BCM_E_NONE) {
          printf("bcm_oam_lif_profile_set\n");
          return rv;
        }

        if(cross_connect_configuration==0)
        {
            rv = bcm_oam_lif_profile_set(unit, 0, gport3, ingress_profile, egress_profile);
            if (rv != BCM_E_NONE) {
                printf("bcm_oam_lif_profile_set\n");
                return rv;
            }
        }


    }

    
    if( device_type != device_type_jericho2)
    {
        /*
         * Adding a default MEP
         */
        printf("Add default mep\n");
        bcm_oam_endpoint_info_t_init(&default_info);
        if (device_type < device_type_arad_plus) {
            default_info.id = -1;
        }
        else {
            default_info.id = BCM_OAM_ENDPOINT_DEFAULT_INGRESS0;
        }
        default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        default_info.timestamp_format = get_oam_timestamp_format(unit);
        rv = bcm_oam_endpoint_create(unit, &default_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }

        ep_def.gport = default_info.gport;
        ep_def.id = default_info.id;
    }
    /* Non acc mep associated with maid 48 byte group is not supported on JR2*/
    if(!(use_48_maid && device_type == device_type_jericho2))
    {
        /*
        * Adding non acc MEP
        */
        bcm_oam_endpoint_info_t_init(&mep_not_acc_info);
        mep_not_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_not_acc_info.group = group_info->id;
        mep_not_acc_info.level = md_level_1;
        mep_not_acc_info.gport = gport1;
        mep_not_acc_info.name = 0;
        mep_not_acc_info.ccm_period = 0;

        if( device_type == device_type_jericho2)
        {
            /* Setting My-CFM-MAC (for Jericho2 Only)*/
            if(device_type == device_type_jericho2)
            {
                rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_1);
                if (rv != BCM_E_NONE) {
                    printf("oam__my_cfm_mac_set fail \n");
                    return rv;
                }
            }

            mep_not_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
        }
        else
        {
            sal_memcpy(mep_not_acc_info.dst_mac_address, mac_mep_1, 6);
        }

        mep_not_acc_info.lm_counter_base_id  = lm_counter_base_id_1;
        mep_not_acc_info.lm_counter_if = counter_if_0;


        printf("bcm_oam_endpoint_create mep_not_acc_info\n");
        mep_not_acc_info.timestamp_format = get_oam_timestamp_format(unit);
        rv = bcm_oam_endpoint_create(unit, &mep_not_acc_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("created MEP with id %d\n", mep_not_acc_info.id);
        ep1.gport = mep_not_acc_info.gport;
        ep1.id = mep_not_acc_info.id;
        ep1.lm_counter_base_id = mep_not_acc_info.lm_counter_base_id;
    }
    /*
    * Adding acc MEP - upmep
    */
    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    if (is_offloaded) {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
    }
    else
    {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    }
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_2;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    mep_acc_info.name = 123;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_3MS;
    mep_acc_info.flags |= BCM_OAM_ENDPOINT_UP_FACING;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 5;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.sampling_ratio = sampling_ratio;
    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from scanner*/
        mep_acc_info.flags2 =  BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;

        if (use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
        }
    }

    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if (device_type < device_type_arad_plus) {
        src_mac_mep_2[5] = port_2; /* In devices older than Arad Plus the LSB of the src mac address must equal the local port. No such restriction in Arad+.*/
    }
    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_2, 6);


    if( device_type == device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port_2, mac_mep_2);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_egress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_2, 6);
    }

    /*RX*/
    mep_acc_info.gport = gport2;
    mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;
    mep_acc_info.lm_counter_if = counter_if_1;

    /*
     * When OAM/BFD statistics enabled and MEP ACC is required to be counted,
     * mep acc id 0 can't be used
     */
    if (oam_up_statistics_enable) {
        mep_acc_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        mep_acc_info.id = (device_type == device_type_qux) ? 1024 : 4096;
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);

    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if (is_offloaded) {
        printf("created offloaded MEP with id %d\n", mep_acc_info.id);
    } else {
        printf("created MEP with id %d\n", mep_acc_info.id);
    }

    ep2.gport = mep_acc_info.gport;
    ep2.id = mep_acc_info.id;
    ep2.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    if (device_type >= device_type_arad_plus) {
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if (use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
            rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
        }
    }

    if (remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(unit, &rmep_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created RMEP with id %d\n", rmep_info.id);

    ep2.rmep_id = rmep_info.id;

    /*
    * Adding acc MEP - downmep
    */

    bcm_oam_endpoint_info_t_init(&mep_acc_info);

    /*TX*/
    if (is_offloaded) {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeLmDmOffloadedEntry;
    }
    else
    {
        mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
    }
    mep_acc_info.type = bcmOAMEndpointTypeEthernet;
    mep_acc_info.group = group_info->id;
    mep_acc_info.level = md_level_3;
    BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
    mep_acc_info.name = 456;
    mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
    mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

    mep_acc_info.vlan = 10;
    mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
    mep_acc_info.outer_tpid = 0x8100;
    mep_acc_info.inner_vlan = 0;
    mep_acc_info.inner_pkt_pri = 0;
    mep_acc_info.inner_tpid = 0;
    mep_acc_info.int_pri = 1 + (3<<2);

    if (down_mep_id) {
      mep_acc_info.flags = BCM_OAM_ENDPOINT_WITH_ID;
      mep_acc_info.id = down_mep_id;
    }

    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from RX*/
        mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;

        if (use_port_interface_status) {
            mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
            mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }
    }

    if (device_type == device_type_arad_a0) {
        /* Arad A0 bug.*/
        src_mac_mep_3[5] = port_1;
    }

    /* The MAC address that the CCM packets are sent with*/
    sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_3, 6);

    /*RX*/
    mep_acc_info.gport = gport1;

    mep_acc_info.lm_counter_base_id = lm_counter_base_id_1;
    mep_acc_info.lm_counter_if = counter_if_1;
    mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

    if( device_type == device_type_jericho2)
    {
        /* Setting My-CFM-MAC (for Jericho2 Only)*/
        rv = oam__my_cfm_mac_set(unit, port_1, mac_mep_3);
        if (rv != BCM_E_NONE) {
            printf("oam__my_cfm_mac_set fail \n");
            return rv;
        }

        mep_acc_info.acc_profile_id = acc_ingress_profile;
    }
    else
    {
        sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_3, 6);
    }

    printf("bcm_oam_endpoint_create mep_acc_info\n");
    rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    printf("created MEP with id %d\n", mep_acc_info.id);
    ep3.gport = mep_acc_info.gport;
    ep3.id = mep_acc_info.id;
    ep3.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

    /*
    * Adding Remote MEP
    */
    bcm_oam_endpoint_info_t_init(&rmep_info);
    rmep_info.name = 0x11;
    rmep_info.local_id = mep_acc_info.id;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.id = mep_acc_info.id;

    if (device_type >= device_type_arad_plus) {
        rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
        if (use_port_interface_status) {
            rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE;
            rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
        }
    }

    if (remote_meps_start_in_loc_enable) {
        rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
        rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    }

   printf("bcm_oam_endpoint_create RMEP\n");
   rv = bcm_oam_endpoint_create(unit, &rmep_info);
   if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }
   printf("created RMEP with id %d\n", rmep_info.id);

    ep3.rmep_id = rmep_info.id;

    if (use_port_interface_status) {
        /*
        * Adding acc MEP - downmep
        */

        bcm_oam_endpoint_info_t_init(&mep_acc_info);

        /*TX*/
        mep_acc_info.type = bcmOAMEndpointTypeEthernet;
        mep_acc_info.group = group_info->id;
        mep_acc_info.level = md_level_4;
        BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info.tx_gport, port_1);
        mep_acc_info.name = 789;
        mep_acc_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
        mep_acc_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;

        mep_acc_info.vlan = 10;
        mep_acc_info.pkt_pri = mep_acc_info.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
        mep_acc_info.outer_tpid = 0x8100;
        mep_acc_info.inner_vlan = 0;
        mep_acc_info.inner_pkt_pri = 0;
        mep_acc_info.inner_tpid = 0;
        mep_acc_info.int_pri = 1 + (3<<2);
        mep_acc_info.timestamp_format = get_oam_timestamp_format(unit);

        if (device_type >= device_type_arad_plus) {
            /* Take RDI only from RX*/
            mep_acc_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_LOC_DISABLE ;

            if (use_port_interface_status) {
                mep_acc_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
                mep_acc_info.port_state = BCM_OAM_PORT_TLV_UP;
                mep_acc_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
            }
        }

        /* The MAC address that the CCM packets are sent with*/
        sal_memcpy(mep_acc_info.src_mac_address, src_mac_mep_4, 6);

        if( device_type == device_type_jericho2)
        {
            /* Setting My-CFM-MAC (for Jericho2 Only)*/
            rv = oam__my_cfm_mac_set(unit, port_2, mac_mep_4);
            if (rv != BCM_E_NONE) {
                printf("oam__my_cfm_mac_set fail \n");
                return rv;
            }

            mep_acc_info.acc_profile_id = acc_ingress_profile;
            mep_acc_info.endpoint_memory_type = bcmOamEndpointMemoryTypeSelfContained;
        }
        else
        {
            sal_memcpy(mep_acc_info.dst_mac_address, mac_mep_4, 6);
        }

        /*RX*/
        mep_acc_info.gport = gport2;
        mep_acc_info.lm_counter_base_id = lm_counter_base_id_2;

       printf("bcm_oam_endpoint_create mep_acc_info\n");
       rv = bcm_oam_endpoint_create(unit, &mep_acc_info);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
       }

        printf("created MEP with id %d\n", mep_acc_info.id);
        ep4.gport = mep_acc_info.gport;
        ep4.id = mep_acc_info.id;
        ep4.lm_counter_base_id = mep_acc_info.lm_counter_base_id;

        /*
        * Adding Remote MEP
        */
        bcm_oam_endpoint_info_t_init(&rmep_info);
        rmep_info.name = 0x12;
        rmep_info.local_id = mep_acc_info.id;
        rmep_info.ccm_period = 0;
        rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE;
        rmep_info.loc_clear_threshold = 1;
        rmep_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
        rmep_info.id = mep_acc_info.id;
        if (device_type >= device_type_arad_plus) {
            rmep_info.flags2 = BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI | BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC;
            if (use_port_interface_status) {
                rmep_info.flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_UPDATE|BCM_OAM_ENDPOINT_PORT_STATE_UPDATE;
                rmep_info.interface_state = BCM_OAM_INTERFACE_TLV_UP;
                rmep_info.port_state = BCM_OAM_PORT_TLV_UP;
            }
        }

        if (remote_meps_start_in_loc_enable) {
            rmep_info.faults |= BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT;
            rmep_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        }

        printf("bcm_oam_endpoint_create RMEP\n");
        rv = bcm_oam_endpoint_create(unit, &rmep_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
        printf("created RMEP with id %d\n", rmep_info.id);
    }

    return rv;
}

int oam_gport_cross_conect_config(bcm_gport_t inLif1,bcm_gport_t inLif2, int port1, int port2)
{
    cross_connect_configuration = 1;
    gport1 = inLif1;
    gport2 = inLif2;
    port_1 = port1;
    port_2 = port2;
    return 0;
}

/**
 * Create a snoop and a trap associated with that snoop.
 * JR1: Snoop OAM packets based on opcode
 * to a given destination.
 * JR2: Snoop OAM packets based on opcode and MAC type
 * to a given destination.
 *
 *
 * @param unit
 * @param snoop_config_info
 *
 * @return int
 */
int oam_change_mep_destination_to_snoop(int unit, oam_snoop_config_s * snoop_config_info)
{
    bcm_error_t rv;

    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error in checking the device type.\n");
        return rv;
    }

    if (device_type == device_type_jericho2)
    {
        rv = dnx_oam_change_mep_destination_to_snoop(unit, snoop_config_info->dest_snoop_port, snoop_config_info->opcode,
                                                    snoop_config_info->mac_type, snoop_config_info->counter_increment,
                                                    snoop_config_info->profile_id, snoop_config_info->is_ingress);
        if (rv != BCM_E_NONE)
        {
            printf("Error in dnx_oam_change_mep_destination_to_snoop\n");
            return rv;
        }
    }
    else
    {
        rv = dpp_oam_change_mep_destination_to_snoop(unit, snoop_config_info->dest_snoop_port, snoop_config_info->endpoint_id,
                                                    snoop_config_info->action_type, snoop_config_info->opcode);
        if (rv != BCM_E_NONE)
        {
            printf("Error in dpp_oam_change_mep_destination_to_snoop\n");
            return rv;
        }
    }

    return rv;
}
