
/*
cint ../../../../src/examples/sand/cint_ip_route_basic.c
*/

/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 *
 */

/* default */ 
int in_port_ingress = 13;
int out_port_ingress = 14;

int encap_id = 8193;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
	
int
pfcdm_pre_rount_config(
    int unit)
{
    int in_port = in_port_ingress;
    int out_port = out_port_ingress;
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */

    /*
     * Set 'fec' as on the corresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = 0;
    int vrf = 1;

    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;
    int host_format = 2;
    int arp_plus_ac_type = 0;

    proc_name = "pfcdm_pre_rount_config";
    printf("%s(): Enter. in_port %d out_port %d  \r\n",proc_name, in_port, out_port);
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1384;

        /*
         * Jr2 myMac assignment is more flexible than in Jer1
         */
        intf_out_mac_address[0] = 0x00;
        intf_out_mac_address[1] = 0x12;
        intf_out_mac_address[2] = 0x34;
        intf_out_mac_address[3] = 0x56;
        intf_out_mac_address[4] = 0x78;
        intf_out_mac_address[5] = 0x9a;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    if (arp_plus_ac_type)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    if (arp_plus_ac_type == 2)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }


    printf("%s(): Exit\r\n",proc_name);
    return rv;
}



int
pfcdm_rount_config(
    int unit)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    /*
     * Set 'fec' as on the corresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = 0;
    int vrf = 1;
    bcm_gport_t gport;
	
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;
 
    /*
     * Add Route entry
     */
    printf("Add route entry. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n", route, mask, vrf, fec);

    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
        return rv;
    }
	
    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port_ingress);

    rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }
	
    return rv;
}

	
int
pfcdm_background_traffic_config(
    int unit)
{
    int in_port = out_port_ingress;
    int out_port = in_port_ingress;
    int rv;
    int intf_in = 20;           /* Incoming packet ETH-RIF */
    int intf_out = 101;         /* Outgoing packet ETH-RIF */

    /*
     * Set 'fec' as on the corresponding test. See, for example,
     * AT_Dnx_Cint_l3_ip_route_basic_raw
     */
    int fec = 0;
    int vrf = 1;

    bcm_mac_t intf_in_mac_address = { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0e, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0xdd };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 route = 0xC0A80160;
    uint32 host = 0xC0A80164;
    uint32 mask = 0xfffffff0;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 15;
    int failover_is_primary = 0;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;
    int host_format = 2;
    int arp_plus_ac_type = 0;

    proc_name = "pfcdm background traffic";
    printf("%s(): Enter. in_port %d out_port %d  \r\n",proc_name, in_port, out_port);
    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jr2 must be > 2k
         */
        encap_id = 0x1385;

        /*
         * Jr2 myMac assignment is more flexible than in Jer1
         */
        intf_out_mac_address[0] = 0x00;
        intf_out_mac_address[1] = 0x12;
        intf_out_mac_address[2] = 0x34;
        intf_out_mac_address[3] = 0x56;
        intf_out_mac_address[4] = 0x78;
        intf_out_mac_address[5] = 0x9b;
    }

    /*
     * 1. Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }
    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */
    if (arp_plus_ac_type)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    }
    if (arp_plus_ac_type == 2)
    {
        flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * Add Route entry
     */
    printf("Add route entry. route 0x%08X mask 0x%08X vrf %d fec 0x%08X. \n", route, mask, vrf, fec);

    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function %s(), \n",proc_name,err_proc_name);
        return rv;
    }
	
    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port_ingress);

    rv = add_host_ipv4(unit, host, vrf, intf_out, encap_id /* arp id */ , gport);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }
	

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}



/*
int cint_rx_trap_ingress_ttl0_trap_id = 0;
int cint_rx_trap_ingress_unknowndest_trap_id = 0;

int pfcdm_flow_tt0_trap_cfg(int unit)
{
    int rv = BCM_E_NONE;

    bcm_rx_trap_config_t trap_config;
    int trap_type = bcmRxTrapForwardingIpv4Ttl0;

    cint_rx_trap_ingress_traps_config_utils_s cint_rx_trap_ttl0_traps_config ={
         BCM_RX_TRAP_XXX FLAGS. BCM_RX_TRAP_UPDATE_DEST
        BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST,

         Destination port.
        0,

         Destination group.
        0,

         Internal priority of the packet.
        0,

         Color of the packet.
        0,

         snoop command.
        0,

         Forwarding header position overridden value
        0,

         Encap-ID
        0,

         Encap-ID2
        0,

         Destinations information per core
        NULL,

         core_config_arr length
        0,

         meter command.
        0,

         mapped trap strength
        0,

        * ECN value
        0,

        * VSQ pointer
        0,

        * Latency flow id clear
        0,

        * Visibility value
        0,

        * Statistical objects are to clear
        0,

        * Statistical objects configuration array
        {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},

        * Statistical objects configuration array length
        0,

         Statistical metadata mask
        0,

         Egress parsing index
        0,

         forward trap strength
        3,  15,

         snoop strength
        0
    };

    BCM_GPORT_LOCAL_SET(cint_rx_trap_ttl0_traps_config.dest_port, local_host_cpu);

    rv = trap_config_from_ingress_config_fill(&cint_rx_trap_ttl0_traps_config, &trap_config);
    if(rv != BCM_E_NONE)
    {
        printf("Error, with config struct \n");
        return rv;
    }

     Set the trap
    rv = cint_utils_rx_trap_create_and_set(unit, 0, trap_type, trap_config, &cint_rx_trap_ingress_ttl0_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    printf("TTL0 Trap id %d\n", cint_rx_trap_ingress_ttl0_trap_id);

    return BCM_E_NONE;
}



int pfcdm_flow_unknown_dest_trap_cfg(int unit)
{
    int rv = BCM_E_NONE;

    bcm_rx_trap_config_t trap_config;
    int trap_type = bcmRxTrapUnknownDest;

    cint_rx_trap_ingress_traps_config_utils_s cint_rx_trap_unknow_dest_traps_config ={
         BCM_RX_TRAP_XXX FLAGS. BCM_RX_TRAP_UPDATE_DEST
        BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST,

         Destination port.
        0,

         Destination group.
        0,

         Internal priority of the packet.
        0,

         Color of the packet.
        0,

         snoop command.
        0,

         Forwarding header position overridden value
        0,

         Encap-ID
        0,

         Encap-ID2
        0,

         Destinations information per core
        NULL,

         core_config_arr length
        0,

         meter command.
        0,

         mapped trap strength
        0,

        * ECN value
        0,

        * VSQ pointer
        0,

        * Latency flow id clear
        0,

        * Visibility value
        0,

        * Statistical objects are to clear
        0,

        * Statistical objects configuration array
        {{0},{0},{0},{0},{0},{0},{0},{0},{0},{0}},

        * Statistical objects configuration array length
        0,

         Statistical metadata mask
        0,

         Egress parsing index
        0,

         forward trap strength
        0,  15,

         snoop strength
        0
    };

    BCM_GPORT_LOCAL_SET(cint_rx_trap_unknow_dest_traps_config.dest_port, local_host_cpu);

    rv = trap_config_from_ingress_config_fill(&cint_rx_trap_unknow_dest_traps_config, &trap_config);
    if(rv != BCM_E_NONE)
    {
        printf("Error, with config struct \n");
        return rv;
    }

     Set the trap
    rv = cint_utils_rx_trap_create_and_set(unit, 0, trap_type, trap_config, &cint_rx_trap_ingress_unknowndest_trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_utils_rx_trap_create_and_set \n");
        return rv;
    }

    printf("Unknown Dest Trap id %d\n", cint_rx_trap_ingress_unknowndest_trap_id);

    return BCM_E_NONE;
}*/


/* used for sampling */
bcm_gport_t sampling_mirror_destination;

int pfcdm_sampling_mirror_cfg(int unit)
{
    /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_dividend = 1; /* Must be 1 */
    uint32 sample_rate_divisor = 10; /* for test 10; */
	
    int rv = BCM_E_NONE;

    bcm_mirror_destination_t dest;
    int int_ingress_mirror_profile = 2;
  
    sal_memset(&dest, 0, sizeof(dest));
    bcm_mirror_destination_t_init(&dest);
	
    dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_IS_STAT_SAMPLE;
    dest.packet_copy_size = 256; /* Fixed value. */
    dest.sample_rate_dividend = sample_rate_dividend; 
    dest.sample_rate_divisor = sample_rate_divisor;

    /* set new color */
    dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_COLOR;
    dest.packet_control_updates.color = bcmColorYellow;
    
    BCM_GPORT_LOCAL_SET(dest.gport, local_host_cpu);
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, int_ingress_mirror_profile);

    rv = bcm_mirror_destination_create(unit, &dest);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_destination_create\n");
        return rv;
    }

    /* deliver original packets to Host CPU */
/*
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);    
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, dest.mirror_dest_id, &mirror_header_info);    
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_header_info_set\n");
        return rv;
    }
*/

    /* sampling destination */
    sampling_mirror_destination = dest.mirror_dest_id;

    return rv;
}


/* For congestion sampling packet */
bcm_gport_t sampling_mirror_destination_2;

int pfcdm_congest_sampling_mirror_cfg(int unit)
{
    /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_dividend = 1; /* Must be 1 */
    uint32 sample_rate_divisor = 100000; /* for test 100000; */
	
    int rv = BCM_E_NONE;

    bcm_mirror_destination_t dest;
    int int_ingress_mirror_profile = 3;
  
    sal_memset(&dest, 0, sizeof(dest));
    bcm_mirror_destination_t_init(&dest);
	
    dest.flags = BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_IS_STAT_SAMPLE;
    dest.packet_copy_size = 256; /* Fixed value. */
    dest.sample_rate_dividend = sample_rate_dividend; 
    dest.sample_rate_divisor = sample_rate_divisor;

    /*It seems the following flag does not take effect */
    dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    
    BCM_GPORT_LOCAL_SET(dest.gport, local_host_cpu);
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, int_ingress_mirror_profile);

    rv = bcm_mirror_destination_create(unit, &dest);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_destination_create\n");
        return rv;
    }

    /* deliver original packets to Host CPU */
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);    
    rv = bcm_mirror_header_info_set(unit, 
        BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, 
        dest.mirror_dest_id, &mirror_header_info);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_header_info_set\n");
        return rv;
    }

    /* sampling destination */
    sampling_mirror_destination_2 = dest.mirror_dest_id;

    return rv;
}


bcm_gport_t pfcdm_snoop_gport_id = 0;

int pfcdm_snoop_cfg(int unit)
{
    int rv = BCM_E_NONE;
    int dest_local_port = local_host_cpu;

    int trap_id = 0;
    int auxRes;
    int flags = 0; /* Do not specify any ID for the snoop command / trap */
    int snoop_command; /* Snoop command */
    int trap_dest_strength = 0; /* snoop to CPU */
    int trap_snoop_strength = 7; /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config; /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;
	
    bcm_mirror_destination_t mirror_dest;

    /* Initialize a mirror destination structure */
    bcm_mirror_destination_t_init(&mirror_dest);
    mirror_dest.flags |= BCM_MIRROR_DEST_IS_SNOOP;
    mirror_dest.packet_copy_size = 256; /* Fixed value. */

    mirror_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
	
    /* set the gport id of mirror port */
    BCM_GPORT_LOCAL_SET(mirror_dest.gport,dest_local_port);

    /* create a mirror destination */
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_mirror_destination_create $rv\n");
        return rv;
    }

    /*
     * Create a User-defined trap for snooping
     */
    rv = bcm_rx_trap_type_create(unit, flags, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_type_create\n");
        auxRes = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return rv;
    }

    /*
     * Configure the trap to the snoop command
     */
    bcm_rx_trap_config_t_init(&trap_config);

    /*for port dest change*/
    trap_config.trap_strength = trap_dest_strength;
    trap_config.snoop_cmnd = BCM_GPORT_MIRROR_GET(mirror_dest.mirror_dest_id); /* Snoop any frame matched by this trap */

    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_rx_trap_set\n");
        auxRes = bcm_mirror_destination_destroy(unit, mirror_dest.mirror_dest_id);
        auxRes = bcm_rx_trap_type_destroy(unit, trap_id);
        return rv;
    }

    /* Get the trap gport to snoop */
    BCM_GPORT_TRAP_SET(&pfcdm_snoop_gport_id, trap_id, trap_dest_strength, trap_snoop_strength);

    printf("created gport trap=%d\n", pfcdm_snoop_gport_id);

    return rv;
}

int
pfcdm_ingress_mirror_gport_get(int unit)
{
    bcm_mirror_destination_t dest;
    int int_ingress_mirror_profile = 2;
    int rv;
    int mirror_dest = local_host_cpu;
	
    sal_memset(&dest, 0, sizeof(dest));
    bcm_mirror_destination_t_init(&dest);
	
    dest.packet_copy_size = 256; /* Fixed value. */
    dest.flags = BCM_MIRROR_DEST_WITH_ID;
    BCM_GPORT_LOCAL_SET(dest.gport, mirror_dest);
    BCM_GPORT_MIRROR_SET(dest.mirror_dest_id, int_ingress_mirror_profile);

    dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;

    rv = bcm_mirror_destination_create(unit, &dest);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_destination_create\n");
        return rv;
    }

    pfcdm_snoop_gport_id = dest.mirror_dest_id;

    /* deliver original packets to R5 */
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mirror_header_info_set\n");
        return rv;
    }
	
    return 0;
};




/* an hash result generation example  */
/* CRC-16=x^16+x^15+x^2+1 */
const uint16_t CRC16_BISYNC = 0x8005;
const uint16_t polynom = CRC16_BISYNC;

/* IPPC_TRJ_CFG.CRC_16_BISYNC_INITIAL_VAL == 0 */

void pfcdm_set_lsb_bit(uint16_t *crc_result, uint8_t bit_offset, uint8_t bit_value)
{
    uint16_t tmp = *crc_result;
    tmp = tmp & (~ (1 << bit_offset));
    tmp = tmp | (bit_value << bit_offset);
	
    *crc_result = tmp;
}


uint16_t pfcdm_crc16_base(uint8_t *input, uint8_t input_len, uint32_t my_polynom, uint16_t crc_initial_value)
{
    uint16_t crc_result = 0;
    int i = 0;
    int j = 0;
    
    uint8_t crc_lsb = 0;
    uint8_t input_bit = 0;
    uint8_t new_byte = 0;
    uint16_t tmp = 0;

    /* set initial value */
    for (i = 0; i < 16; i ++)
    {
        pfcdm_set_lsb_bit(&crc_result, i, (crc_initial_value >> (15 - i )) & 0x1);
	}

    for (i = 0; i < input_len; i ++)
    {
        new_byte = input[i];
        printf(" new_byte 0x%02x\n", new_byte);	
		
        for (j = 0; j < 8; j ++)
        {
            tmp = crc_result;
            if (tmp & 0x8000) /* if the uppermost bit a 1 */
            {
                tmp = (tmp ) ^ (my_polynom >> 1);
                crc_result = tmp;
		    }
		
            /* shift the next bit to the message into the remainder */
            crc_lsb = tmp & 0x1;
            crc_result = crc_result << 1;
        
            input_bit = (new_byte >> j) & 0x01;

            crc_result = crc_result | ((input_bit ^ crc_lsb) & 0x01);
            printf(" %d-%d : input_val %d, crc_lsb %d, tmp %d\n", i, j, input_bit, crc_lsb, crc_result);
        }
	}

    return crc_result;
}


void pfcdm_crc16_test()
{
	uint8_t input[40];
    uint8_t input_len = 40;
	uint32_t my_polynom = polynom;
	uint16_t crc_initial_value = 0;
    uint16_t crc_result = 0;
	
    sal_memset(input, 0, sizeof(40));
    input[0] = 0x02;
    input[1] = 0x01;
    input[2] = 0xA8;
    input[3] = 0xC0;
    input[4] = 0x02;
    input[5] = 0xFF;
    input[6] = 0xFF;
    input[7] = 0x7F;
    input[8] = 0x11;
    input[9] = 0x3F;
    input[10] = 0x00;
    input[10] = 0x64;
    input[11] = 0x00;

    crc_result = pfcdm_crc16_base(input, input_len, my_polynom, crc_initial_value);
    print crc_result;
}



void pfcdm_crc16_test_2()
{
	uint8_t input[40];
    uint8_t input_len = 40;
	uint32_t my_polynom = polynom;
	uint16_t crc_initial_value = 0;
    uint16_t crc_result = 0;
	
    sal_memset(input, 0, sizeof(40));
    input[0] = 0xC0;
    input[1] = 0xA8;
    input[2] = 0x01;
    input[3] = 0x02;
    input[4] = 0x7F;
    input[5] = 0xFF;
    input[6] = 0xFF;
    input[7] = 0x02;
    input[8] = 0x11;
    input[9] = 0x00;
    input[10] = 0x3F;
    input[10] = 0x00;
    input[11] = 0x64;

    crc_result = pfcdm_crc16_base(input, input_len, my_polynom, crc_initial_value);
    print crc_result;
}
