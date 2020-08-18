/* $Id: cint_sflow_basic.c
*/

/**
 *
 * This cint calls all the required APIs to configure a basic sFlow agent.
 *
 * The general init function initializes general objects like field qualifier and CRPS counters.
 * The egress create function creates the egress side of the sFlow - the encapsulation stack that
 *  builds the datagram and sends it as a UDP packet to the datagram collector.
 * The ingress create function creates the ingress side of the sFlow - creates a mirror destination for stat
 *  sampling, packet crop and other sFflow datagram fields. Then it adds a specific field entry and maps it to
 *  this mirror destination.
 * The main function creates a simple route flow and then calls the ingress and egress creation functions for it.
 *
 * How to run:
 *  cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 *  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  c
 *  cint_sflow_general_init(0, 201);
 *
 *  And then:
 *  cint_sflow_basic_route_main(0, 200, 201, 202, 0xA711);
 *
 *  To keep the sFlow timestamp up to date, call update_sflow_timestamp every millisecond with the time of day in milliseconds.
 *
 */


struct cint_sflow_basic_info_s {
    /* CRPS Properties */
    int counter_if;           /* Stat command for OAM counter */
    int counter_base;       /* Counter base for OAM counter */
    /* Mirror properties. */
    uint32 sample_rate_dividend; /* Sample rate = rate dividend / rate_divisor. Dividend must always be 1. */
    uint32 sample_rate_divisor;

    /* UDP tunnel properties */
    int out_rif;                          /* Outrif for udp tunnel. */
    bcm_mac_t intf_access_mac_address;    /* mac for RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for the next hop in the tunnel to the collector */
    bcm_ip_t tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t tunnel_sip;                  /* tunnel SIP */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    uint16 udp_src_port;                  /* UDP source port */
    uint16 udp_dst_port;                  /* UDP destination port */
    int collector_vlan;                   /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    uint32 sub_agent_id;                  /* Sub agent ID for the sFlow datagram header. */
    uint32 kaps_result;                   /* kaps result for the basic route. */
    uint32 uptime;                        /* Random uptime for the cint example. */
};

cint_sflow_basic_info_s cint_sflow_basic_info = {
    /* Counter interface | Counter base */
               0,           0,
    /* Sample rate dividend | divisor */
               1,              1,
    /* out rif */
       120,
    /* intf_access_mac_address             | tunnel_next_hop_mac */
       {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /* tunnel DIP */
       0xA1000011 /* 161.0.0.17 */,
    /* tunnel SIP */
       0xA0000011 /* 160.0.0.17 */,
    /* tunnel type */
       bcmTunnelTypeUdp,
    /* UDP SRC | DST ports */
       6000, 5000,
    /* collector vlan */
       120,
    /* tunnel_ttl */
       64,
    /* sub_agent_id */
       0x17,
    /* kaps result */
       0xA711,
    /* uptime */
       0x12345678
};

/*
 * This function updates the uptime virtual register. It should be called every millisecond to update the timestamp
 * for sFlow packets.
 */
int
update_sflow_timestamp(int unit, uint32 time_msec)
{
    int rv;

    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowUpTime, time_msec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_instru_control_get\n");
        return rv;
    }

    return rv;
}

/*
 * This function creates an ethernet rif + arp + udp tunnel, that the sflow packet will be sent to.
 */
int
create_udp_tunnel(int unit, bcm_gport_t *udp_tunnel)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    bcm_if_t arp_itf;

    /*
     * Create rif for the arp.
     */
    rv = intf_eth_rif_create(unit, cint_sflow_basic_info.out_rif,
            cint_sflow_basic_info.intf_access_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create eth_rif_intf_provider\n");
        return rv;
    }


    /*
     * Create arp for the udp tunnel.
     */
    rv = l3__egress_only_encap__create(unit, 0, &arp_itf, cint_sflow_basic_info.tunnel_next_hop_mac,
            cint_sflow_basic_info.collector_vlan);

    if (rv != BCM_E_NONE){
        printf("Error, l3__egress_only_encap__create\n");
        return rv;
    }


    /* Create IP tunnel initiator for encapsulating UDPoIPv4 tunnel header*/
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t_init(&l3_intf);
    bcm_tunnel_initiator_t_init(&tunnel_init);
    tunnel_init.dip        = cint_sflow_basic_info.tunnel_dip;
    tunnel_init.sip        = cint_sflow_basic_info.tunnel_sip;
    tunnel_init.flags      = 0;
    tunnel_init.type       = cint_sflow_basic_info.tunnel_type;
    tunnel_init.udp_dst_port = cint_sflow_basic_info.udp_dst_port;
    tunnel_init.udp_src_port = cint_sflow_basic_info.udp_src_port;
    tunnel_init.ttl        = cint_sflow_basic_info.tunnel_ttl;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.encap_access = bcmEncapAccessTunnel4; /* Use tunnel 4 to allow access from sFlow encap
                                                                    in encap access 1+2*/

    rv = bcm_tunnel_initiator_create(unit,&l3_intf, tunnel_init);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
        return rv;
    }

    *udp_tunnel = tunnel_init.tunnel_id;

    return rv;
}


/*
 * This function creates a mirror destination with a sample rate of dividend / divisor, sets it to
 *  crop the packet and add an sFlow systems header, and sets its destination to the sFlow encap id + out_port.
 */
int
cint_sflow_mirror_create(int unit,
                            int in_port,
                            int out_port,
                            bcm_gport_t sflow_encap_id,
                            int sample_rate_dividend,
                            int sample_rate_divisor,
                            int *mirror_dest_id)
{
    int rv;
    bcm_mirror_destination_t mirror_dest;

    /*
     * Create the mirror destination.
     */
    bcm_mirror_destination_t_init(&mirror_dest);

    mirror_dest.flags = BCM_MIRROR_DEST_IS_STAT_SAMPLE;
    mirror_dest.packet_copy_size = 256; /* Fixed value. */
    mirror_dest.sample_rate_dividend = sample_rate_dividend; /* Must be 1 */
    mirror_dest.sample_rate_divisor = sample_rate_divisor;
    mirror_dest.gport = out_port;
    rv = bcm_mirror_destination_create(unit, &mirror_dest);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mirror_destination_create \n");
        return rv;
    }

    /*
     * Modify FTMH header on mirror destination to the sFlow channel and sFlow systems header.
     * Ase extension is used to pass sFlow data.
     * Flow id extension is used as a workaround to fix a HW bug causing the 1588 protocol not to
     * display properly.
     */
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.ase_ext.valid = TRUE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeSFlow;
    mirror_header_info.tm.flow_id_ext.valid = TRUE;
    mirror_header_info.tm.flow_id_ext.flow_id = 0;
    mirror_header_info.tm.flow_id_ext.flow_profile = 0;

    /* We need to encapsulate the sflow encap id with tunnel so the mirror API will recognize it */
    BCM_GPORT_TUNNEL_ID_SET(mirror_header_info.tm.out_vport, sflow_encap_id);

    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_IS_STAT_SAMPLE, mirror_dest.mirror_dest_id, &mirror_header_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_mirror_header_info_set \n");
        return rv;
    }

    *mirror_dest_id = mirror_dest.mirror_dest_id;

    return rv;
}

/*
 * This function creates the egress pipe configuration for the sFlow - it configures a crps counter,
 * and configures the sFlow encap and the udp tunnel to which the sflow will be sent.
 */
int
create_sflow_egress(int unit, int in_port, int out_port, bcm_gport_t *sflow_destination)
{
    int rv;
    bcm_gport_t udp_tunnel_id;

    /*
     * Create the sFlow collector UDP tunnel outlif.
     */
    rv = create_udp_tunnel(unit, &udp_tunnel_id);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_udp_tunnel\n");
        return rv;
    }

    /*
     * Create the sFlow ETPS entry. UDP tunnel and counter base as input.
     */
    bcm_instru_sflow_encap_info_t sflow_encap_info;
    sal_memset(&sflow_encap_info, 0, sizeof(sflow_encap_info));

    sflow_encap_info.counter_command_id = cint_sflow_basic_info.counter_if;
    sflow_encap_info.stat_cmd = cint_sflow_basic_info.counter_base;
    sflow_encap_info.tunnel_id = udp_tunnel_id;
    sflow_encap_info.sub_agent_id = cint_sflow_basic_info.sub_agent_id;

    rv = bcm_instru_sflow_encap_create(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_sflow_encap_create\n");
        return rv;
    }

    *sflow_destination = sflow_encap_info.sflow_encap_id;
    printf("Sflow outlif = 0x%x\n", sflow_encap_info.sflow_encap_id);
    return BCM_E_NONE;
}


/*
 * This function configures the ingress side of the sFlow, including the PMF selector and mirror
 * destination.
 */
int
create_sflow_ingress(int unit, int in_port, int sflow_out_port, bcm_gport_t sflow_destination)
{
    int rv;
    bcm_gport_t sflow_mirror_destination;
    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    rv = cint_sflow_mirror_create(unit, in_port, sflow_out_port, sflow_destination,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &sflow_mirror_destination);
    printf("cint_sflow_mirror_create - returned mirror dest %d 0x%x\n", sflow_mirror_destination,sflow_mirror_destination);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_sflow_mirror_create \n");
        return rv;
    }

    /*
     * Create a field entry for IPoETH
     */

    bcm_field_entry_t field_entry_handle;

    rv = cint_field_s_flow_entry_add(unit, bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4, TRUE, sflow_mirror_destination,
                field_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_field_s_flow_entry_add\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function will create the route from cint_basic_route.c, and will use it as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_basic_route_main(int unit, int in_port, int route_out_port, int sflow_out_port)
{
    bcm_gport_t sflow_destination;
    bcm_error_t rv;

    /*
     * Create some traffic flow.
     * In this example we use ip route basic, but it could be anything.
     */
    rv = dnx_basic_example(unit, in_port, route_out_port, cint_sflow_basic_info.kaps_result);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in dnx_basic_example\n");
        return rv;
    }

    /*
     * Configure the sFlow egress pipeline.
     */
    rv = create_sflow_egress(unit, in_port, sflow_out_port, &sflow_destination);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_generic_sflow_configuration\n");
        return rv;
    }

    /*
     * Configure the sFlow ingress pipeline
     */
    rv = create_sflow_ingress(unit, in_port, sflow_out_port, sflow_destination);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in create_sflow_ingress\n");
        return rv;
    }


    return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. It will init the CRPS and the field
 * processor qualifiers.
 */
int
cint_sflow_general_init(int unit, int sflow_out_port)
{
    int rv;
    /*
     * Configure the CRPS for the sFlow.
     */
    rv = set_counter_resource(unit, sflow_out_port, cint_sflow_basic_info.counter_if, &cint_sflow_basic_info.counter_base);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in set_counter_resource\n");
        return rv;
    }

    /*
     * Intialize the s flow field configuration, then map the flow's tuple to the mirror destination.
     */
    rv = cint_field_s_flow_main(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_field_s_flow_main\n");
        return rv;
    }

    /*
     * Set the global virtual register for sFlow sampling rate.
     */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowSamplingRate, cint_sflow_basic_info.sample_rate_divisor);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_instru_control_get\n");
        return rv;
    }

    /*
     * Set the global sFlow agent IP address.
     */
    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowAgentIPAddress, cint_sflow_basic_info.tunnel_sip);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_dnx_instru_control_get\n");
        return rv;
    }

    /*
     * For the purpose of the example cint, we'll use a meaningless timestamp.
     */
    rv = update_sflow_timestamp(unit, cint_sflow_basic_info.uptime);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in update_sflow_timestamp\n");
        return rv;
    }


    return BCM_E_NONE;
}
