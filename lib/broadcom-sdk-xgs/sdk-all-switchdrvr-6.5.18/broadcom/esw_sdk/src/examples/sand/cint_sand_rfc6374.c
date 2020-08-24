/*
 * $Id: cint_sand_rfc6374.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_sand_rfc6374.c Purpose: basic examples for RFC-6374.
 */

int port_1 = 13;
int port_2 = 14;

/* Inferred LM indication */
int is_inferred_lm = 0;

int lm_counter_base_id;

/* Disable counting for certain opcode */
int count_disable = 0;

int cpu_trap_code = 0;

bcm_oam_endpoint_info_t rfc6374_mep_info;

/**
 * Create non-accelerated RFC-6374 endpoint
 *
 *
 * @param unit
 * @param type - any of the following types:
 *             - bcmOAMEndpointTypeMplsLmDmLsp
 *             - bcmOAMEndpointTypeMplsLmDmPw
 *             - bcmOAMEndpointTypeMplsLmDmSection
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int rfc6374_endpoint_create(int unit, bcm_oam_endpoint_type_t type, int use_mpls_out_gport, int set_as_accelerated)
{
    bcm_error_t rv;
    bcm_gport_t oamp_port;
    bcm_gport_t out_gport = BCM_GPORT_INVALID;
    bcm_mpls_tunnel_switch_t tunnel_switch;
    int encap_id;
    int gport;
    int intf_id;
    int label;
    int mpls_termination_label_index_enable;
    int tx_port;
    int opcode = bcmOamMplsLmDmOpcodeTypeLm;

    rv = oam__device_type_get(unit, &device_type);
    if (rv != BCM_E_NONE)
    {
        printf("Error in oam__device_type_get: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    if ((device_type <= device_type_arad_plus) || (device_type >= device_type_jericho2))
    {
        printf("Error: RFC-6374 is not supported for Arad, Arad+, Jericho2 and above.\n");
        return -1;
    }

    if (type == bcmOAMEndpointTypeMplsLmDmLsp)
    {
        /* Use the mpls_lsr function */
        printf("Call mpls_lsr_tunnel_example\n");
        rv = mpls_lsr_tunnel_example(&unit, 1, port_1, port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in mpls_lsr_tunnel_example: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Read mpls index soc property */
        mpls_termination_label_index_enable = soc_property_get(unit, "mpls_termination_label_index_enable", 0);
        if (mpls_termination_label_index_enable)
        {
            gport = ingress_tunnel_id_indexed[0];
        }
        else
        {
            gport = ingress_tunnel_id;
        }

        label = 1234;
        BCM_GPORT_TUNNEL_ID_SET(out_gport, mpls_lsr_info_1.ingress_intf);
        intf_id = mpls_lsr_info_1.encap_id;
        tx_port = port_2;
    }
    else if (type == bcmOAMEndpointTypeMplsLmDmPw)
    {
        /* Use the pwe_init function */
        printf("Call pwe_init\n");
        pwe_cw = 1;
        rv = pwe_init(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error in pwe_init: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        label = 1234;
        gport = mpls_lsr_info_1.mpls_port_id;
        out_gport = mpls_lsr_info_1.mpls_port_id;
        intf_id = mpls_lsr_info_1.encap_id; /* out lif: mpls_port->encap_id */
        tx_port = port_2;
    }
    else if (type == bcmOAMEndpointTypeMplsLmDmSection)
    {
        /* Use the mpls_various_scenarios_main function */
        printf("Call mpls_various_scenarios_main\n");
        is_section_oam = 1;
        cint_mpls_various_scenarios_info.tunnel_label = 900;
        rv = mpls_various_scenarios_main(unit, port_1, port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in mpls_various_scenarios_main: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        label = 13;
        gport = cint_mpls_various_scenarios_info.vport_id1;
        intf_id = cint_mpls_various_scenarios_info.arp_encap_id1;
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(out_gport, cint_mpls_various_scenarios_info.arp_encap_id1);
        tx_port = port_1;
    }
    else
    {
        printf("Endpoint type is not supported.\n");
        return -1;
    }

    /* Allocate counter */
    if (type == bcmOAMEndpointTypeMplsLmDmSection)
    {
        rv = set_counter_source_and_engines(unit, &lm_counter_base_id, port_1);
        if (rv != BCM_E_NONE)
        {
            printf("Error in set_counter_source_and_engines: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }
    else
    {
        rv = set_counter_source_and_engines(unit, &lm_counter_base_id, port_2);
        if (rv != BCM_E_NONE)
        {
            printf("Error in set_counter_source_and_engines: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    /* Create RFC-6374 endpoint */
    bcm_oam_endpoint_info_t_init(&rfc6374_mep_info);

    rfc6374_mep_info.type = type;
    rfc6374_mep_info.gport = gport; /* In-LIF */
    BCM_GPORT_SYSTEM_PORT_ID_SET(rfc6374_mep_info.tx_gport, tx_port);
    rfc6374_mep_info.mpls_out_gport = (use_mpls_out_gport ? out_gport : BCM_GPORT_INVALID); /* Out-LIF */

    rfc6374_mep_info.lm_counter_base_id = lm_counter_base_id;
    if (set_as_accelerated)
    {
        rfc6374_mep_info.intf_id = intf_id; /* Out-LIF */
        rfc6374_mep_info.opcode_flags = BCM_OAM_OPCODE_CCM_IN_HW;
        rfc6374_mep_info.session_id = 234;
        rfc6374_mep_info.int_pri = 7;
        rfc6374_mep_info.egress_label.label = label;
        rfc6374_mep_info.egress_label.ttl = 20;
        rfc6374_mep_info.egress_label.exp = 0;
    }

    if (is_inferred_lm)
    {
        rfc6374_mep_info.flags2 |= BCM_OAM_ENDPOINT_FLAGS2_MPLS_LM_DM_ILM;
        opcode = bcmOamMplsLmDmOpcodeTypeIlm;
    }

    /**
     * The default timestamp_format is bcmOAMTimestampFormatNTP,
     * set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1
     */
    rfc6374_mep_info.timestamp_format = (soc_property_get(unit, "oam_dm_ntp_enable", 1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1);

    rv = bcm_oam_endpoint_create(unit, &rfc6374_mep_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    printf("Created RFC-6374 MEP with ID %d\n", rfc6374_mep_info.id);

    if (set_as_accelerated)
    {
        rv = oamp_gport_get(unit, &oamp_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error in rfc6374_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Set action for LM trap to OAMP */
        rv = rfc6374_endpoint_action_set(unit, oamp_port, rfc6374_mep_info.id, bcmOAMActionFwd, opcode);
        if (rv != BCM_E_NONE)
        {
            printf("Error in rfc6374_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Set the action to count LM packets, required only for Jericho device */
        if ((device_type < device_type_jericho_plus) && is_inferred_lm)
        {
            rv = rfc6374_endpoint_action_set(unit, BCM_GPORT_INVALID, rfc6374_mep_info.id, bcmOAMActionCountEnable, opcode);
            if (rv != BCM_E_NONE)
            {
                printf("Error in rfc6374_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
                return rv;
            }
        }

        bcm_oam_loss_t loss_obj;
        bcm_oam_loss_t_init(&loss_obj);

        loss_obj.id = rfc6374_mep_info.id;
        loss_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        loss_obj.peer_da_mac_address[3] = 0xef;
        loss_obj.peer_da_mac_address[1] = 0xab;
        /* use extended statistics*/
        loss_obj.flags = BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_STATISTICS_EXTENDED; /* LMM based loss management*/

        rv = bcm_oam_loss_add(unit,&loss_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_loss_add: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        /* Set action for DM trap to OAMP */
        rv = rfc6374_endpoint_action_set(unit, oamp_port, rfc6374_mep_info.id, bcmOAMActionFwd, bcmOamMplsLmDmOpcodeTypeDm);
        if (rv != BCM_E_NONE)
        {
            printf("Error in rfc6374_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        bcm_oam_delay_t delay_obj;
        bcm_oam_delay_t_init(&delay_obj);

        delay_obj.id = rfc6374_mep_info.id;
        delay_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        delay_obj.timestamp_format = (soc_property_get(unit, "oam_dm_ntp_enable", 1) ? bcmOAMTimestampFormatNTP : bcmOAMTimestampFormatIEEE1588v1);

        rv = bcm_oam_delay_add(unit,&delay_obj);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_oam_delay_add: (%s)\n", bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}

/**
 * Create non-accelerated RFC-6374 over LSP endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int rfc6374_run_with_defaults_lsp(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = rfc6374_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmLsp, use_mpls_out_gport, set_as_accelerated);
    if (rv != BCM_E_NONE)
    {
        printf("Error in rfc6374_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Create non-accelerated RFC-6374 over PWE endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int rfc6374_run_with_defaults_pwe(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = rfc6374_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmPw, use_mpls_out_gport, set_as_accelerated);
    if (rv != BCM_E_NONE)
    {
        printf("Error in rfc6374_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Create non-accelerated Section RFC-6374 endpoint
 *
 *
 * @param unit
 * @param port1 - port on which the MEP resides
 * @param port2
 * @param use_mpls_out_gport - used for TX counting
 * @param set_as_accelerated - set to "1" for accelerated and to "0" for non accelerated MEP
 *
 * @return int
 */
int rfc6374_run_with_defaults_section(int unit, int port1, int port2, int use_mpls_out_gport, int set_as_accelerated) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error in mpls__mpls_pipe_mode_exp_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    rv = rfc6374_endpoint_create(unit, bcmOAMEndpointTypeMplsLmDmSection, use_mpls_out_gport, set_as_accelerated);
    if (rv != BCM_E_NONE)
    {
        printf("Error in rfc6374_endpoint_create: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * RFC-6374 endpoint action set calling sequence example
 *
 *
 * @param unit
 * @param dest_port - Must be BCM_GPORT_INVALID for bcmOAMActionCountEnable.
 *                    Otherwise, it should be a trap.
 * @param endpoint_id - RFC-6374 endpoint ID
 * @param action_type - Action to be applied:
 *                  - bcmOAMActionCountEnable to enable counting.
 *                  - bcmOAMActionFwd to forward the packet to e specified destination.
 *                  - Set count_disable and do not pass any actions to disable counting.
 * @param opcode - RFC-6374 opcode upon which action will be applied:
 *                  - bcmOamMplsLmDmOpcodeTypeLm
 *                  - bcmOamMplsLmDmOpcodeTypeDm
 *                  - bcmOamMplsLmDmOpcodeTypeIlm
 *
 * @return int
 */
int rfc6374_endpoint_action_set(int unit, bcm_gport_t dest_port, bcm_oam_endpoint_t endpoint_id, bcm_oam_action_type_t action_type, bcm_oam_mpls_lm_dm_opcode_type_t opcode) {
    bcm_error_t rv;
    int trap_code;
    bcm_oam_endpoint_action_t action;
    bcm_rx_trap_t trap_type;
    bcm_rx_trap_config_t trap_config;

    bcm_oam_endpoint_action_t_init(&action);

    action.destination = dest_port;
    if ((dest_port != BCM_GPORT_INVALID) && !BCM_GPORT_IS_TRAP(dest_port)) {
        /**
         * action.destination can only receive a trap as destination.
         * Allocate a new trap.
         */
        trap_type = bcmRxTrapUserDefine;
        rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_type_create: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        rv = port_to_system_port(unit, dest_port, &trap_config.dest_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error in port_to_system_port: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_rx_trap_set(unit, trap_code, trap_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_rx_trap_set: (%s)\n", bcm_errmsg(rv));
            return rv;
        }

        printf("Trap set, trap_code = %d\n", trap_code);
        BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

        cpu_trap_code = trap_code;
    }

    BCM_OAM_OPCODE_SET(action, opcode);
    if (!count_disable)
    {
        BCM_OAM_ACTION_SET(action, action_type);
    }
    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_oam_endpoint_action_set: (%s)\n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}
