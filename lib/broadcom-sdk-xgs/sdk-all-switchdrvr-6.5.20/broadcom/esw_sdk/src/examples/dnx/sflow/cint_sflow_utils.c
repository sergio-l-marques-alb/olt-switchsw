/* $Id: cint_sflow_utils.c
*/

/**
 *
 * This cint contains utility functions helping to configure
 * sFlow agent. 
 *
 *
 * How to run: 
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utility.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_advanced.c 
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_extended_gateway.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_advanced.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_extended_gateway.c
 *  
 * See 
 * cint_sflow_basic.c/cint_sflow_advanced.c/cint_sflow_extended_gateway.c
 * for usage. 
 *
 */


/*
 * This function updates the uptime virtual register. It should be called every millisecond to update the timestamp
 * for sFlow packets.
 */
int
cint_sflow_utils_timestamp_update(int unit, uint32 time_msec)
{
    int rv;

    rv = bcm_instru_control_set(unit, 0, bcmInstruControlSFlowUpTime, time_msec);
    if (rv != BCM_E_NONE)
    {
        printf("cint_sflow_utils_timestamp_update: Error (%d), bcm_instru_control_set(type = %d, value = %d)\n", rv, bcmInstruControlSFlowUpTime, time_msec);
        return rv;
    }

    return rv;
}

/*
 * This function creates an ethernet rif + arp + udp tunnel, that the sflow packet will be sent to.
 */
int
cint_sflow_utils_udp_tunnel_create(int unit, bcm_gport_t *udp_tunnel)
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
     * Create arp with AC for the udp tunnel. 
     * Note: 
     * SFLOW does two ESEM access to resolve SFLOW input and output interface field.
     * EEDB AC entry must be created to avoid port ESEM access which overrides the above ESEM access.
     */
    rv = create_arp_with_next_ac_type_vlan_translate_none(unit, cint_sflow_basic_info.tunnel_next_hop_mac, &arp_itf, cint_sflow_basic_info.collector_vlan);

    if (rv != BCM_E_NONE){
        printf("Error, create_arp_with_next_ac_type\n");
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
cint_sflow_utils_mirror_create(int unit,
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
     */
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.ase_ext.valid = TRUE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeSFlow;
    mirror_header_info.tm.flow_id_ext.valid = TRUE;
    mirror_header_info.tm.flow_id_ext.flow_id = 0;
    mirror_header_info.tm.flow_id_ext.flow_profile = 0;

    /* We need to encapsulate the sflow encap id with tunnel so the mirror API will recognize it */
    BCM_GPORT_TUNNEL_ID_SET(mirror_header_info.tm.out_vport, sflow_encap_id);

    /* keep original FTMH. It's used to retrieve original source port and destination port */
    rv = bcm_mirror_header_info_set(unit, BCM_MIRROR_DEST_IS_STAT_SAMPLE | BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER, mirror_dest.mirror_dest_id, &mirror_header_info);
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
 * and configures the sFlow encap.
 * in case of DP: configures udp tunnel to which the sflow will be sent.
 */
int
cint_sflow_utils_egress_create(int unit, int in_port, int out_port, bcm_gport_t *sflow_destination, int eventor_id, int is_aggregated)
{
    int rv;
    bcm_gport_t udp_tunnel_id;

    printf("cint_sflow_utils_egress_create: in_port = %d, out_port= %d, eventor_id = %d, is_aggregated = %d\n",
           in_port, out_port, eventor_id, is_aggregated);

    if (!is_aggregated)
    {
       if (cint_sflow_basic_info.udp_tunnel_id == -1) {
           /*
            * Create the sFlow collector UDP tunnel outlif.
            */
           rv = cint_sflow_utils_udp_tunnel_create(unit, &udp_tunnel_id);

           if (rv != BCM_E_NONE)
           {
               printf("Error, in cint_sflow_utils_udp_tunnel_create\n");
               return rv;
           }

           cint_sflow_basic_info.udp_tunnel_id = udp_tunnel_id;
       }
       else 
       {
          udp_tunnel_id = cint_sflow_basic_info.udp_tunnel_id;
       }
    }
    else
    {
       /*
       * UDP tunnel is irrelevant when using eventor
       */
       udp_tunnel_id = 0;
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
    sflow_encap_info.eventor_id= eventor_id;

    if (is_aggregated)
    {
        sflow_encap_info.flags |= BCM_INSTRU_SFLOW_ENCAP_AGGREGATED;
    }

    rv = bcm_instru_sflow_encap_create(unit, &sflow_encap_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_sflow_encap_create\n");
        return rv;
    }

    *sflow_destination = sflow_encap_info.sflow_encap_id;
    printf("cint_sflow_utils_egress_create: Sflow outlif = 0x%x\n", sflow_encap_info.sflow_encap_id);
    return BCM_E_NONE;
}

/*
 * This functions configures the eventor for Sflow - samples size, number of samples per sflow datagram etc.
 * Note:
 * For simplicity - this function sets Eventor RX double buffer to be the pair 
 * eventor_id and eventor_id+1 (modulo 8).
 * Thus, when using multi channel configuration, each channel actually occupies two eventor IDs thus each channel's 
 * eventor_id is +2 from previous channel's eventor_id.
 */
int
cint_sflow_utils_eventor_set(int unit, int eventor_id, int buffer_size, int nof_samples, int eventor_port, int sflow_out_port)
{
    int rv;
    uint32 flags = 0;
    int buffer_size_thr = buffer_size * nof_samples;
    int context;
    int builder;
    uint32 header_length;
    uint32 udp_header_offset;
    uint32 ip_length_offset;
    bcm_instru_eventor_context_conf_t context_conf = {0};
    bcm_instru_eventor_builder_conf_t builder_conf = {0};
    uint8 * header_data;
    /* TX header data (SFLOW_DATAGRAMoUDPoIPv4oETH1oITMHoPTCH2)*/
    uint8 header_data_udp_tunnel[128] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCD, 0x1D,
            0x00, 0x0C, 0x00, 0x02, 0x00, 0x56, 0x81, 0x00, 0x00, 0x78, 0x08, 0x00, 0x45, 0x00, 0x06, 0x38,
            0x00, 0x00, 0x00, 0x00, 0x40, 0x11, 0x39, 0x5E, 0xA0, 0x00, 0x00, 0x11, 0xA1, 0x00, 0x00, 0x11,
            0x17, 0x70, 0x13, 0x88, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x01,
            0xA0, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    /* TX header data (SFLOW_DATAGRAMoUDPoIPv6oETH1oITMHoPTCH2)*/
    uint8 header_data_udp6_tunnel[128] = {
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0xCD, 0x1D,
            0x00, 0x0C, 0x00, 0x02, 0x00, 0x56, 0x81, 0x00, 0x00, 0x78, 0x86, 0xDD, 0x60, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x11, 0x40, 0xEC, 0xEF, 0xEE, 0xED, 0xEC, 0xEB, 0xEA, 0xE9, 0xE8, 0xE7, 0xE6, 0xE5,
            0xE4, 0xE3, 0xE2, 0xE1, 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF,
            0xFE, 0x33, 0x44, 0x55, 0x17, 0x70, 0x13, 0x88, 0x00, 0x59, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05,
            0x00, 0x00, 0x00, 0x01, 0xA0, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x17, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    printf("cint_sflow_utils_eventor_set: eventor_id = %d, eventor_port = %d, buffer_size = %d, nof_samples = %d, buffer_size_thr = %d \n", eventor_id, eventor_port, buffer_size, nof_samples, buffer_size_thr);

    /* in PTCH2, In_PP_Port is 10 bits [9:0]*/
    uint8 eventor_port_high = (eventor_port & 0x3FF) >> 8;
    uint8 eventor_port_low = (eventor_port & 0x3FF);
    /* In ITMH, the destination is 21 bits [29:9], but only 16 bits are system port.
       In addition, need to shift left one bit beause it start from bit 9.
    */
    uint8 sflow_port_high = (sflow_out_port <<1) >> 16;
    uint8 sflow_port_mid = ((sflow_out_port <<1) & 0xFF00) >> 8;
    uint8 sflow_port_low = ((sflow_out_port <<1) & 0xFF);

    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv4oETH1oITMHoPTCH2)*/
        header_data = header_data_udp_tunnel;
        header_length = 72;
        udp_header_offset = 48;
        ip_length_offset = 30;

    }
    else if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp6)
    {
        /* Set TX header size and data (SFLOW_DATAGRAMoUDPoIPv6oETH1oITMHoPTCH2)*/
        header_data = header_data_udp6_tunnel;
        header_length = 92;
        udp_header_offset = 68;
        ip_length_offset = 32;
    }
    else
    {
        printf("cint_sflow_utils_eventor_set: only the following tunnel types are supported: bcmTunnelTypeUdp, bcmTunnelTypeUdp6 \n");
        return BCM_E_FAIL;
    }

    header_data[0] = eventor_port_high;
    header_data[1] = eventor_port_low;

    /* Note: the assumption here is that the Sflow output destination is System Port */
    header_data[3] = 0x18 | sflow_port_high ; /* See destination encoding*/
    header_data[4] = sflow_port_mid;
    header_data[5] = sflow_port_low;

    /* UDP port for SFLOW = 6343 = 0x18C7 */
    header_data[udp_header_offset] = cint_sflow_basic_info.udp_src_port >> 8;
    header_data[udp_header_offset+1] = cint_sflow_basic_info.udp_src_port & 0xFF;
    header_data[udp_header_offset+2] = cint_sflow_basic_info.udp_dst_port >> 8;
    header_data[udp_header_offset+3] = cint_sflow_basic_info.udp_dst_port & 0xFF;
    /* 
      UDP length =
      UDP header (8 bytes) + payload data
      payload data = SFLOW Datagram header (7 words) + aggregated data
       */
    uint32 udp_header_length = 8 + (7 + buffer_size_thr)*4;
    header_data[udp_header_offset+4] = udp_header_length >> 8;
    header_data[udp_header_offset+5] = udp_header_length & 0xFF;

    /* 
      IP Header - total length =
      UDP header + 20 bytes
       */
    uint32 ip_header_total_length = udp_header_length;
    if (cint_sflow_basic_info.tunnel_type == bcmTunnelTypeUdp)
    {
        ip_header_total_length += 20;
    }
    header_data[ip_length_offset] = ip_header_total_length >> 8;
    header_data[ip_length_offset+1] = ip_header_total_length & 0xFF;

    printf("cint_sflow_utils_eventor_set: IP LENGTH = 0x%04X, UDP LENGTH = 0x%04X, udp_src_port = 0x%04X, udp_dst_port = 0x%04X \n", ip_header_total_length, udp_header_length, cint_sflow_basic_info.udp_src_port, cint_sflow_basic_info.udp_dst_port);

    context = builder = eventor_id;
    /* init eventor RX context */

    /*
     * 1. Can simply use different pair of banks per context (but since there are 8 banks, there can be only 4 contexs ...)
     * 2. Can use same pair of banks but with different offset (bank size is 13K bytes)
     * Here, for simplicity, use #1 
     *  
     * Note: performing modulo 8 operation by & 0x7
     */
    context_conf.bank1 = context & 0x7;
    context_conf.bank2 = (context+1) & 0x7;
    printf("cint_sflow_utils_eventor_set: bank1 = %d, bank2 = %d\n", context_conf.bank1, context_conf.bank2);

    context_conf.buffer_size = buffer_size;
    context_conf.buffer1_start = 0;
    context_conf.buffer2_start = 256;

    /* init builder */
    builder_conf.flags = 0;
    builder_conf.thresh_size = buffer_size_thr;
    builder_conf.thresh_time = BCM_INSTRU_EVENTOR_TIMEOUT_NONE;
    builder_conf.header_data = header_data;
    builder_conf.header_length = header_length;

    /*
     * Set eventor Rx context (Rx buffers)
     */
    rv = bcm_instru_eventor_context_set(unit, flags, context, bcmEventorContextTypeRx, &context_conf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_context_set\n");
        return rv;
    }

    /*
     * Set eventor builder (Tx buffers)
     */
    rv = bcm_instru_eventor_builder_set(unit, flags, builder, &builder_conf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_eventor_builder_set\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This functions activates the eventor for Sflow.
 */
int
cint_sflow_utils_eventor_activate(int unit)
{
    int rv;

    uint32 flags = 0;

    printf("cint_sflow_utils_eventor_activate: START\n");

   /*
   * Activate eventor
   */
   rv = bcm_instru_eventor_active_set(unit, flags, 1);
   if (rv != BCM_E_NONE)
   {
      printf("Error, in bcm_instru_eventor_active_set\n");
      return rv;
   }

   printf("cint_sflow_utils_eventor_activate: END\n");

   return BCM_E_NONE;
}

/*
 * Sflow registers registers initialization 
 */
int
cint_sflow_utils_registers_init(int unit)
{
    int rv;

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
    rv = cint_sflow_utils_timestamp_update(unit, cint_sflow_basic_info.uptime);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sflow_timestamp_update\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function creates port-to-interface mapping in SFLOW the egress pipe. 
 * The port can be: 
 *    For input interface:
 *       - system port gport.
 *       - system port value.
 *    Note:  for trunk, need to add each of the trunk member separately.
 *    For output interface:
 *       - system port gport.
 *       - system port value.
 *       - trunk gport.
 *       - Multicast gport.
 *       - Flow-ID gport.
 *  
 * For more information about SFLOW input/output interface please 
 * refer to BCM API bcm_instru_sflow_sample_interface_add. 
 */
int
cint_sflow_utils_egress_interface_create(int unit, int port, int interface, int is_input)
{
    int rv;
    bcm_gport_t gport;
    bcm_instru_sflow_sample_interface_info_t sample_interface_info;

    if (BCM_GPORT_IS_SET(port)) {
       gport = port;
    }
    else {
       BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    }

    printf("cint_sflow_utils_egress_interface_create: port = %d, gport = 0x%08x, interface = 0x%08x,  is_input = %d\n",
           port, gport, interface, is_input);

    sample_interface_info.flags = is_input? BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT:BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT;
    sample_interface_info.port = gport;
    sample_interface_info.interface = interface;

    rv = bcm_instru_sflow_sample_interface_add(unit, &sample_interface_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_sflow_sample_interface_add\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function removes port-to-interface mapping in SFLOW the egress pipe
 */
int
cint_sflow_utils_egress_interface_remove(int unit, int port, int is_input)
{
    int rv;
    bcm_gport_t gport;
    bcm_instru_sflow_sample_interface_info_t sample_interface_info;

    if (BCM_GPORT_IS_SET(port)) {
       gport = port;
    }
    else {
       BCM_GPORT_SYSTEM_PORT_ID_SET(gport, port);
    }

    printf("cint_sflow_utils_egress_interface_remove: port = %d, gport = 0x%08x, is_input = %d\n",
           port, gport, is_input);

    sample_interface_info.flags = is_input? BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_INPUT:BCM_INSTRU_SFLOW_SAMPLE_INTERFACE_OUTPUT;
    sample_interface_info.port = gport;
    sample_interface_info.interface = 0;

    rv = bcm_instru_sflow_sample_interface_remove(unit, &sample_interface_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_instru_sflow_sample_interface_remove\n");
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * This function creates a trunk with 3 port memebers.
 */
cint_trunk_utils_s trunk;
int
cint_sflow_utils_lag_create(int unit, int port1, int port2, int port3)
{
    int rv;

    printf("cint_sflow_utils_lag_create: START\n");

    trunk.ports_nof = 3;
    trunk.ports[0] = port1;
    trunk.ports[1] = port2;
    trunk.ports[2] = port3;
    trunk.pool = 0;
    trunk.group = 10;
    trunk.headers_type_in = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    trunk.headers_type_out = BCM_SWITCH_PORT_HEADER_TYPE_ETH;
    trunk.vlan_domain = 128;
    trunk.vlan = 15; /* see dnx_basic_example_inner - intf_in */

    /* Create trunk  */
    rv = cint_trunk_utils_create(unit, &trunk);
    if (rv != BCM_E_NONE)
    {
       printf("cint_sflow_utils_lag_create: Error (%d), in cint_trunk_utils_create\n", rv);
       return rv;
    }

    printf("cint_sflow_utils_lag_create: END\n");

    return BCM_E_NONE;
}
