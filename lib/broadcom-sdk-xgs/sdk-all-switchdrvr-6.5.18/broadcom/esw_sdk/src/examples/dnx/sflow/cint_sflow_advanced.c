/* $Id: cint_sflow_advanced.c
*/

/**
 *
 *  This cint calls all the required APIs to configure an
 *  advanced sFlow agent.
 *
 * How to run:
 *  cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 *  cint ../../../../src/examples/dnx/field/cint_field_utils.c
 *  cint ../../../../src/examples/dnx/crps/cint_crps_oam_config.c
 *  cint ../../../../src/examples/dnx/oam/cint_oam_basic.c
 *  cint ../../../../src/examples/sand/cint_ip_route_basic.c
 *  cint ../../../../src/examples/sand/cint_basic_bridge.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_field_advanced.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_utils.c 
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_basic.c
 *  cint ../../../../src/examples/dnx/sflow/cint_sflow_advanced.c
 *  
 *  c
 * 
 *  cint_sflow_advanced_general_init(unit, sflow_out_port);
 *  basic_bridge_main_run(unit, iPort, oPort, c_vid, vsi, d_mac_0, d_mac_1, d_mac_2, d_mac_3, d_mac_4, d_mac_5);
 *
 *  Configure each channel:   
 *  cint_sflow_advanced_eventor_set(unit, sflow_out_port, nof_samples, eventor_id);
 *  cint_sflow_advanced_channel_create(unit, in_port, sflow_out_port, eventor_id, is_aggregated, is_ip_packet, is_short_packet);
 *
 *  And then call activation functions:
 *  cint_sflow_advanced_eventor_activate(unit);
 *  cint_sflow_advanced_main_activate(unit);
 *
 *  To keep the sFlow timestamp up to date, call update_sflow_timestamp every millisecond with the time of day in milliseconds.
 *
 */


/*
 * This function configures the ingress side of the sFlow, including the PMF selector and mirror
 * destination.
 */
int
cint_sflow_advanced_ingress_create(int unit, int in_port, int sflow_out_port, bcm_gport_t sflow_destination, int is_ip_packet, int is_short_packet)
{
    int rv;
    bcm_gport_t sflow_mirror_destination;

    printf("cint_sflow_advanced_ingress_create: in_port = %d, sflow_out_port = %d, sflow_destination = %d, is_ip_packet = %d, is_short_packet = %d \n", 
           in_port, sflow_out_port, sflow_destination, is_ip_packet, is_short_packet);

    /*
     * Create a mirror destination of type stat sampling, including ingress crop, and stat rate,
     * and modified ftmh header with sFlow extension.
     */
    rv = cint_sflow_utils_mirror_create(unit, sflow_out_port, sflow_destination,
            cint_sflow_basic_info.sample_rate_dividend, cint_sflow_basic_info.sample_rate_divisor,
            &sflow_mirror_destination);

    printf("cint_sflow_utils_mirror_create: is_ip_packet = %d, is_short_packet = %d,  mirror dest = %d 0x%x\n", is_ip_packet, is_short_packet,
           sflow_mirror_destination,sflow_mirror_destination);
    if (rv != BCM_E_NONE)
    {
        printf("Error, cint_sflow_utils_mirror_create \n");
        return rv;
    }

    /*
     * Create a field entry for IPoETH
     */
    bcm_field_entry_t field_entry_handle;

    if (is_short_packet == FALSE) {
           /* Add TCAM entries for Long Packet:
         - IPv4 
         - IPv6 
         - Not IP 
          */
       if (is_ip_packet == TRUE) {
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp4, TRUE, 0, FALSE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(bcmFieldLayerTypeIp4, long)\n");
              return rv;
          }
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp6, TRUE, 0, FALSE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(bcmFieldLayerTypeIp6, long)\n");
              return rv;
          }
       }
       else {
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp4, FALSE, 1, FALSE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(Not IP, long)\n");
              return rv;
          }
       }
    }
    else {
       /* Add TCAM entries for Short Packet:
         - IPv4 
         - IPv6 
         - Not IP 
          */
       if (is_ip_packet == TRUE) {
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp4, TRUE, 0, TRUE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(bcmFieldLayerTypeIp4, short)\n");
              return rv;
          }
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp6, TRUE, 0, TRUE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(bcmFieldLayerTypeIp6, short)\n");
              return rv;
          }
       }
       else {
          rv = cint_sflow_field_advanced_entry_add(unit, bcmFieldLayerTypeIp4, FALSE, 1, TRUE, sflow_mirror_destination, &field_entry_handle);
          if (rv != BCM_E_NONE)
          {
              printf("Error, in cint_sflow_field_advanced_entry_add(Not IP, short)\n");
              return rv;
          }
       }
    }

    return BCM_E_NONE;
}

/*
 * This function will create the route from cint_basic_route.c, and will use it as a sample flow
 * to sample and send to sFlow.
 */
int
cint_sflow_advanced_channel_create(int unit, int in_port, int sflow_out_port, int eventor_port, int eventor_id, int is_aggregated, int is_ip_packet, int is_short_packet)
{
    bcm_gport_t sflow_destination;
    bcm_field_context_t bridge_context_id, route_context_id;
    bcm_error_t rv;

    printf("cint_sflow_advanced_channel_create: START, eventor_id = %d, is_aggregated = %d, is_ip_packet = %d, is_short_packet = %d\n", 
            eventor_id, is_aggregated, is_ip_packet, is_short_packet);

    /*
     * Configure the CRPS for the sFlow.
     */
    rv = set_counter_resource(unit, sflow_out_port, cint_sflow_basic_info.counter_if, &cint_sflow_basic_info.counter_base);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in set_counter_resource\n");
        return rv;
    }

    printf("set_counter_resource: counter_if = %d, counter_base = %d \n", cint_sflow_basic_info.counter_if, cint_sflow_basic_info.counter_base);

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
       rv = cint_sflow_advanced_ingress_create(unit, in_port, eventor_port, sflow_destination, is_ip_packet, is_short_packet);
    }
    else {
       rv = cint_sflow_advanced_ingress_create(unit, in_port, sflow_out_port, sflow_destination, is_ip_packet, is_short_packet);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_advanced_ingress_create\n");
        return rv;
    }

    printf("cint_sflow_advanced_channel_create: END\n");

    return BCM_E_NONE;
}

/*
 * This function creates sflow field group contexts, attach field groups
 * to context and set it's preselector.
 */
int
cint_sflow_advanced_main_activate(int unit)
{
    bcm_field_context_t bridge_context_id, route_context_id;
    bcm_error_t rv;

    printf("cint_sflow_advanced_main_activate: START \n");

    /* Create different context for Bridge and Route*/
    rv = cint_sflow_field_advanced_group_context_create(unit, &bridge_context_id, "Bridge");
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_create(Bridge)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_context_create(unit, &route_context_id, "Route");
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_create(Route)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_attach(unit, bridge_context_id, TRUE /*is_bridge*/);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_attach(Bridge)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_attach(unit, route_context_id, FALSE /*is_bridge*/);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_attach(Route)\n");
        return rv;
    }

    rv = cint_sflow_field_advanced_group_context_presel_set(unit, bridge_context_id, sflow_header_sampling_base_presel_id, bcmFieldAppTypeL2);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_presel_set(Bridge)\n");
        return rv;
    }

    rv = cint_sflow_field_advanced_group_context_presel_set(unit, route_context_id, sflow_header_sampling_base_presel_id +1, bcmFieldAppTypeIp4UcastRpf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_presel_set(Route, IPv4 unicast)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_context_presel_set(unit, route_context_id, sflow_header_sampling_base_presel_id +2, bcmFieldAppTypeIp6UcastRpf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_presel_set(Route, IPv6 unicast)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_context_presel_set(unit, route_context_id, sflow_header_sampling_base_presel_id +3, bcmFieldAppTypeIp4McastRpf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_presel_set(Route, IPv4 multicast)\n");
        return rv;
    }
    rv = cint_sflow_field_advanced_group_context_presel_set(unit, route_context_id, sflow_header_sampling_base_presel_id +4, bcmFieldAppTypeIp6McastRpf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_context_presel_set(Route, IPv6 multicast)\n");
        return rv;
    }

    printf("cint_sflow_advanced_main_activate: END \n");

    return BCM_E_NONE;
}


/*
 * Call this function before calling any other main functions. It will init the CRPS and the field
 * processor qualifiers.
 */
int
cint_sflow_advanced_eventor_set(int unit, int sflow_out_port, int eventor_port, int nof_samples, int eventor_id)
{
    int rv;

    printf("cint_sflow_advanced_eventor_set: START, sflow_out_port=%d, eventor_port = %d, nof_samples=%d, eventor_id=%d \n",
           sflow_out_port, eventor_port, nof_samples, eventor_id);

   /*
   * Set eventor configuration.
   */
   rv = cint_sflow_utils_eventor_set(unit, eventor_id, cint_sflow_basic_info.buffer_size, nof_samples, eventor_port, sflow_out_port);
   if (rv != BCM_E_NONE)
   {
      printf("Error, in cint_sflow_utils_eventor_set\n");
      return rv;
   }

   printf("cint_sflow_advanced_eventor_set: END \n");

   return BCM_E_NONE;
}

/*
 * Call this function before calling any other main functions. It inits the field
 * processor qualifiers and the S-Flow virtual registers.
 */
int
cint_sflow_advanced_general_init(int unit)
{
    int rv;

    printf("cint_sflow_advanced_general_init: START\n");

    /*
     * Creates the the TCAM field group (qualifiers and actions).
     */
    rv = cint_sflow_field_advanced_group_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in cint_sflow_field_advanced_group_create\n");
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

    printf("cint_sflow_advanced_general_init: END\n");

    return BCM_E_NONE;
}
