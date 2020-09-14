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
 *  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 *  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utils.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  
 *  c
 *  cint_sflow_basic_general_init(unit, sflow_out_port, eventor_id,nof_samples, is_aggregated);
 *  
 *  if eventor is used:
 *  sflow_eventor_activate(unit);
 *  
 *  And then:
 *  cint_sflow_basic_route_main(unit, in_port, route_out_port, sflow_out_port, eventor_id, is_aggregated);
 *  
 *  For setting input/output interface value:
 *  cint_sflow_utils_egress_interface_create(unit, port, interface, is_input);
 *
 *  To keep the sFlow timestamp up to date, call sflow_timestamp_update every millisecond with the time of
 *  day in milliseconds.
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
    bcm_gport_t udp_tunnel_id;            /* the created udp tunnel*/
    int out_rif;                          /* Outrif for udp tunnel. */
    bcm_mac_t intf_access_mac_address;    /* mac for RIF */
    bcm_mac_t tunnel_next_hop_mac;        /* mac for the next hop in the tunnel to the collector */
    bcm_ip_t tunnel_dip;                  /* tunnel DIP */
    bcm_ip_t tunnel_sip;                  /* tunnel SIP */
    bcm_ip6_t tunnel_dip6;                /* tunnel DIP v6 */
    bcm_ip6_t tunnel_sip6;                /* tunnel DIP v6 */
    bcm_tunnel_type_t tunnel_type;        /* tunnel type */
    uint16 udp_src_port;                  /* UDP source port */
    uint16 udp_dst_port;                  /* UDP destination port */
    int collector_vlan;                   /* VID assigned to the outgoing packet */
    int tunnel_ttl;                       /* Ip tunnel header TTL */
    uint32 sub_agent_id;                  /* Sub agent ID for the sFlow datagram header. */
    uint32 kaps_result;                   /* kaps result for the basic route. */
    uint32 uptime;                        /* Random uptime for the cint example. */

    /* Eventor properties */
    uint32 buffer_size;
};

cint_sflow_basic_info_s cint_sflow_basic_info = {
    /* Counter interface | Counter base */
               0,           0,
    /* Sample rate dividend | divisor */
               1,              1,
   /*udp_tunnel_id */
      -1,
   /* out rif */
       120,
    /* intf_access_mac_address             | tunnel_next_hop_mac */
       {0x00, 0x0c, 0x00, 0x02, 0x00, 0x56}, {0x00, 0x02, 0x00, 0x00, 0xcd, 0x1d},
    /* tunnel DIP */
       0xA1000011 /* 161.0.0.17 */,
    /* tunnel SIP */
       0xA0000011 /* 160.0.0.17 */,
    /* tunnel IPv6 DIP */
       { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /* tunnel IPv6 SIP */
       { 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5, 0xE4, 0xE3, 0xE2, 0xE1 },
    /* tunnel type */
       bcmTunnelTypeUdp,
    /* UDP SRC | DST ports */
       6343, 6343,
    /* collector vlan */
       120,
    /* tunnel_ttl */
       64,
    /* sub_agent_id */
       0x17,
    /* kaps result */
       0xA711,
    /* uptime */
       0x12345678,
    /* buffer_size: in words (4 bytes)
     * Data: 128 bytes = 32 words 
     * Header record: 6 words 
     * Sample Data Header: 10 words 
     */
       48
};



/*
 * This function configures the ingress side of the sFlow, including the PMF selector and mirror
 * destination.
 */
int
cint_sflow_basic_ingress_create(int unit, int in_port, int sflow_out_port, bcm_gport_t sflow_destination)
{
    int rv;
    bcm_gport_t sflow_mirror_destination;
    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    rv = cint_sflow_utils_mirror_create(unit, sflow_out_port, sflow_destination,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &sflow_mirror_destination);
    printf("cint_sflow_utils_mirror_create - returned mirror dest %d 0x%x\n", sflow_mirror_destination,sflow_mirror_destination);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_sflow_utils_mirror_create \n");
        return rv;
    }

    /*
     * Create a field entry for IPoETH
     */

    bcm_field_entry_t field_entry_handle;

    rv = cint_sflow_field_entry_add(unit, bcmFieldLayerTypeEth, bcmFieldLayerTypeIp4, TRUE, sflow_mirror_destination,
                field_entry_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_entry_add\n");
        return rv;
    }

    return BCM_E_NONE;
}


/*
 * This function will create the route from cint_basic_route.c, and will use it as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_basic_route_main(int unit, int in_port, int route_out_port, int sflow_out_port, int eventor_port, int eventor_id, int is_aggregated)
{
    bcm_gport_t sflow_destination;
    bcm_error_t rv;
    int fec_id;
    
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit,0, &fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_sflow_basic_info.kaps_result = fec_id;

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
    rv = cint_sflow_utils_egress_create(unit, in_port, sflow_out_port, &sflow_destination, eventor_id, is_aggregated);

    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_utils_egress_create\n");
        return rv;
    }

    /*
     * Configure the sFlow ingress pipeline
     */
    if (is_aggregated) {
       rv = cint_sflow_basic_ingress_create(unit, in_port, eventor_port, sflow_destination);
    }
    else {
       rv = cint_sflow_basic_ingress_create(unit, in_port, sflow_out_port, sflow_destination);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_basic_ingress_create\n");
        return rv;
    }


    return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. It will init the CRPS and the field
 * processor qualifiers.
 */
int
cint_sflow_basic_general_init(int unit, int sflow_out_port, int eventor_port, int eventor_id, int nof_samples, int is_aggregated)
{
    int rv;
    /*
     * Configure the CRPS for the sFlow.
     */
    if (is_aggregated) {
       rv = set_counter_resource(unit, eventor_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base);
    } else {
    rv = set_counter_resource(unit, sflow_out_port, cint_sflow_basic_info.counter_if, 0, &cint_sflow_basic_info.counter_base);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in set_counter_resource\n");
        return rv;
    }

    /*
     * Intialize the s flow field configuration, then map the flow's tuple to the mirror destination.
     */
    rv = cint_sflow_field_main(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_main\n");
        return rv;
    }

    /*
     * Set the global virtual register for sFlow.
     */
    rv = cint_sflow_utils_registers_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_utils_registers_init\n");
        return rv;
    }

    if (is_aggregated)
    {
        /*
         * Set eventor configuration.
         */
        rv = cint_sflow_utils_eventor_set(unit, eventor_id, cint_sflow_basic_info.buffer_size, nof_samples, eventor_port, sflow_out_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in cint_sflow_utils_eventor_set\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}
