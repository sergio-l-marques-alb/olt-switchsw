/*
 * $Id: cint_src_mac_encap.c v 1.0 02/06/2016 skoparan Exp $
 *
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: cint_src_mac_encap.c
 * Purpose: Example L2 source mac encapsulation.
 *
 * Fuctions:
 * Main function:
 *      l2_src_encap_example_dvapi()
 *
 *
 */

int ac_action_add(int unit, int out_vlan, int out_sysport) {
    int rv;
    int is_adv_vt, action_id;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_t vlan_port;
    bcm_port_match_info_t port_match_info;
    bcm_vlan_translate_action_class_t action_class;

    is_adv_vt = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    if (is_adv_vt) {
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = out_sysport;
        vlan_port.vsi = out_vlan;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
        }

        rv = vlan_port_translation_set(unit, (out_vlan+1), 0, vlan_port.vlan_port_id, 2, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_port_translation_set\n");
        }

        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create\n");
        }

        bcm_vlan_action_set_t_init(&action);
        action.dt_outer = bcmVlanActionAdd;
        action.new_outer_vlan = out_vlan+1;
        rv = bcm_vlan_translate_action_id_set(unit,BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_set\n");
        }

        action_class.vlan_edit_class_id = 2;
        action_class.tag_format_class_id = 0;
        action_class.vlan_translation_action_id = action_id;
        action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
        rv= bcm_vlan_translate_action_class_set( unit,  &action_class);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_class_set\n");
        }
    } else {
        bcm_vlan_port_t_init(&vlan_port);

        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vlan_port.match_vlan = out_vlan;
        vlan_port.port = out_sysport;
        vlan_port.egress_vlan = out_vlan + 1;

        rv = bcm_vlan_port_create(unit, &vlan_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_port_create\n");
        }

        port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
        port_match_info.match = BCM_PORT_MATCH_PORT;
        port_match_info.port = out_sysport;
        port_match_info.match_vlan = BCM_VLAN_ALL;

        rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &port_match_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_match_add\n");
        }
    }

    return rv;
}

int l2_src_encap_example_dvapi(int *units_ids, int nof_units, int in_sysport, int out_sysport) {
    int rv;
    int i, unit, host;
    int ing_intf_in, ing_intf_out;
    int fec[2] = {0x0,0x0};
    int flags = 0, flags1 = 0;
    int in_vlan = 15, out_vlan = 100, open_vlan = 1;
    int vrf = 0;
    int encap_id[2]={0x0,0x0};
    int route, mask;
    create_l3_egress_s l3_egress;
    bcm_mac_t mac_address  = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };  /* my-MAC */
    bcm_mac_t next_hop_mac  = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }; /* next_hop_mac1 */
    int fwd_group_cascaded_fec;
    bcm_tunnel_initiator_t tunnel;
    bcm_mac_t custom_mac = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x15}; /* Custom mac */
    bcm_failover_t failover_id_fec = 0;

    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);
    if (!urpf_mode_per_rif) {
        rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
        if (rv != BCM_E_NONE) {
            return rv;
        }
    }

    flags = 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
            return rv;
        }

        create_l3_intf_s intf;
        intf.vsi = in_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.mtu_valid = 1;
        intf.mtu = default_mtu;
        intf.mtu_forwarding = default_mtu_forwarding;
        if (urpf_mode_per_rif) {
            intf.rpf_valid = 1;
            intf.flags |= BCM_L3_RPF;
            intf.urpf_mode = L3_uc_rpf_mode;
        }
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0; i < nof_units; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        }
        rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
            return rv;
        }

        create_l3_intf_s intf;
        intf.vsi = out_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.mtu_valid = 1;
        intf.mtu = default_mtu;
        intf.mtu_forwarding = default_mtu_forwarding;
        if (urpf_mode_per_rif) {
            intf.rpf_valid = 1;
            intf.flags |= BCM_L3_RPF;
            intf.urpf_mode = L3_uc_rpf_mode;
        }
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_out = out_rif_used = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
        flags |= BCM_L3_WITH_ID;
    }

    l3_egress.allocation_flags = 0;
    for (i = 0; i < nof_units; i++){
        unit = units_ids[i];

        bcm_l3_intf_t l3_intf_tunnel;
        bcm_l3_intf_t_init(&l3_intf_tunnel);
        bcm_tunnel_initiator_t_init(&tunnel);
        tunnel.type = bcmTunnelTypeL2SrcEncap;
        tunnel.smac = custom_mac;
        rv = bcm_tunnel_initiator_create(unit, &l3_intf_tunnel, &tunnel);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_tunnel_initiator_create\n");
        }

        l3_egress.l3_flags = BCM_L3_CASCADED;
        l3_egress.allocation_flags = BCM_L3_CASCADED;
        l3_egress.out_gport = out_sysport;
        l3_egress.vlan = out_vlan;
        l3_egress.next_hop_mac_addr = next_hop_mac;
        l3_egress.out_tunnel_or_rif = l3_intf_tunnel.l3a_intf_id;
        l3_egress.arp_encap_id = encap_id[1];
        if (ip_route_fec_with_id2 != 0) {
            fec[1] = ip_route_fec_with_id2;
            l3_egress.allocation_flags |= BCM_L3_WITH_ID;
        }

        l3_egress.fec_id = fec[1];
        rv = l3__egress__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
        }

        encap_id[1] = l3_egress.arp_encap_id;
        BCM_GPORT_FORWARD_PORT_SET(fwd_group_cascaded_fec, l3_egress.fec_id);

        l3_egress.out_tunnel_or_rif = ing_intf_out;
        l3_egress.out_gport = fwd_group_cascaded_fec;
        l3_egress.vlan = out_vlan;
        l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
        l3_egress.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
        l3_egress.fec_id = fec[1];
        if (!is_device_or_above(unit,JERICHO_B0)) {
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_fec);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create\n");
                return rv;
            }
        }

        l3_egress.failover_id = failover_id_fec;
        l3_egress.failover_if_id = 0;

        if (!is_device_or_above(unit,JERICHO_B0)) {
            l3_egress.arp_encap_id = 0;
            rv = l3__egress__create(unit,&l3_egress);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }
            l3_egress.failover_if_id = l3_egress.fec_id;
        }

        l3_egress.arp_encap_id = encap_id[1];
        rv = l3__egress__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[1] = l3_egress.fec_id;
        fec_id_used = fec[1];
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }

        rv = ac_action_add(unit, out_vlan, out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, ac_action_add\n");
            return rv;
        }
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    if(soc_property_get(unit , "enhanced_fib_scale_prefix_length",0)){
        mask  = 0xffff0000;
    }
    else{
        mask  = 0xfffffff0;
    }
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        printf("fec used is  = 0x%x (%d)\n", fec[1], fec[1]);

        rv = internal_ip_route(unit, route, mask, vrf, fec[1], 0) ;
        if (rv != BCM_E_NONE) {
            printf("Error, in function internal_ip_route, \n");
            return rv;
        }
    }

    return rv;
}

