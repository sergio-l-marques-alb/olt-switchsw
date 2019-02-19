/* $Id: cint_ip_route_tunnel.c,v 1.7 Broadcom SDK $
 * $Copyright: (c) 2017 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

/* 
 * Sequence example to  
 * test: 
 * run: 
 * BCM> cint utility/cint_utils_mpls.c  
 * BCM> cint cint_qos.c 
 * BCM> cint utility/cint_utils_l3.c
 * BCM> cint cint_mpls_lsr.c 
 * BCM> cint cint_ip_route.c 
 * BMC> cint cint_mutli_device_utils.c 
 * BCM> cint cint_ip_route_tunnel.c
 * BCM> cint
 * cint> int nof_units = <nof_units>;
 * cint> int units[nof_units] = {<unit1>, <unit2>,...};    
 * cint> int outP = 13;
 * cint> int inP = 13;
 * cint> int outSysport, inSysport;
 * cint> port_to_system_port(unit1, outP, &outSysport);
 * cint> port_to_system_port(unit2, inP, &inSysport);
 * example_ip_to_tunnel(units_ids, nof_units,<inSysport>, <outSysport>); 
 *  
 *  
 * Note: this cint also includes tests for 4 label push and protection 
 */

/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;


/*
 * When using MPLS tunnel, whether to add EL/EL+ELI
 * 0 - No entropy
 * 1 - EL
 * 2 - EL+ELI
 */
int add_el_eli = 0;

/********** 
  functions
 ********** */

/******* Run example ******/
 
/*
 * packet will be routed from in_port to out-port 
 * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.03)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
 *  
 * Route: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan = 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected: 
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
 */
int preserve_dscp_per_lif = 0;
int mpls_tunnel_used = 0;
int example_ip_to_tunnel(int *units_ids, int nof_units, int in_sysport, int out_sysport){
    int rv;
    int unit, i;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;
    int route;
    int mask; 
    int tunnel_id, tunnel_id2 = 0;
	int flags = 0;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties2;

    create_l3_intf_s intf;

    /*** create ingress router interface ***/
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        if (preserve_dscp_per_lif & 0x1) {
            bcm_vlan_port_t vlan_port;
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.port=in_sysport;
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan = in_vlan;
            vlan_port.egress_vlan = in_vlan;
            vlan_port.vsi = in_vlan;
            rv = bcm_vlan_port_create(unit, &vlan_port);
            if (rv != BCM_E_NONE) {
                printf("fail create VLAN port, port(0x%08x), vlan(%d)\n", in_sysport, in_vlan);
                return rv;
            }
            
            rv = bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
                return rv;
            }
        } else {
            rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
              return rv;
            }
        }

        intf.vsi = in_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
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

        intf.vsi = out_vlan;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_out = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress object 1 ***/
    /*** Create tunnel ***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    tunnel_id = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 0;
		mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
        if (add_el_eli >= 2) {
            mpls_tunnel_properties.flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            if (add_el_eli == 3) {
                mpls_tunnel_properties.label_out = 0x300;
                mpls_tunnel_properties.label_out_2 = 0x320;
                mpls_tunnel_properties.label_out_3 = 0x330;
                mpls_tunnel_properties.flags_out |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
                mpls_tunnel_properties.flags_out_2 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
                mpls_tunnel_properties.flags_out_3 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            }
        }
        else if (add_el_eli == 1) {
            mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
        }
        printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }
        /* having a tunnel id != 0 is equivalent to being WITH_ID*/

        tunnel_id = mpls_tunnel_used = mpls_tunnel_properties.tunnel_id;
        printf("Created Tunnel ID: 0x%08x\n",tunnel_id);

        mpls_tunnel_properties2.label_in = 200;
		mpls_tunnel_properties2.label_out = 0;
		mpls_tunnel_properties2.next_pointer_intf = ing_intf_out;
        if (add_el_eli >= 2) {
            mpls_tunnel_properties2.flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            if (add_el_eli == 3) {
                mpls_tunnel_properties2.label_out = 0x300;
                mpls_tunnel_properties2.label_out_2 = 0x320;
                mpls_tunnel_properties2.label_out_3 = 0x330;
                mpls_tunnel_properties2.flags_out |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
                mpls_tunnel_properties2.flags_out_2 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
                mpls_tunnel_properties2.flags_out_3 |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE|
                                             BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
            }
        }
        else if (add_el_eli == 1) {
            mpls_tunnel_properties2.flags |= BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE;
        }
        printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties2);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }
        /* having a tunnel id != 0 is equivalent to being WITH_ID*/

        tunnel_id2 = mpls_tunnel_properties2.tunnel_id;
        printf("Created Tunnel ID: 0x%08x\n",tunnel_id2);

        if (preserve_dscp_per_lif & 0x2) {
            bcm_gport_t gport;
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, tunnel_id);
            rv = bcm_port_control_set(unit, gport, bcmPortControlPreserveDscpEgress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", gport);
                return rv;
            }
        }
    }

    /*** Create egress object1 with the tunnel_id ***/
    for (i = 0 ; i < nof_units ; i++){
        create_l3_egress_s l3eg;
        unit = units_ids[i]; 
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.allocation_flags = flags;
        l3eg.out_tunnel_or_rif = tunnel_id;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }
    flags = 0;
    
    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    for (i = 0 ; i < nof_units ; i++){
        create_l3_egress_s l3eg1;
        unit = units_ids[i];
        sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg1.allocation_flags = flags; 
        l3eg1.out_tunnel_or_rif = tunnel_id2;
        l3eg1.vlan = out_vlan;
        l3eg1.out_gport = out_sysport;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[1] = l3eg1.fec_id;
        encap_id[1] = l3eg1.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_route(unit, route, mask , vrf, fec[1]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);
        }
    }


    return rv;
}


/*
     custom_feature_preserving_dscp_enabled=1 
     custom_feature_dscp_preserve_with_extend_label=1
     mpls_egress_label_extended_encapsulation_mode=1
     logical_port_routing_preserve_dscp=2
    
     cint ../../../../src/examples/dpp/utility/cint_utils_mpls.c  
     cint ../../../../src/examples/dpp/cint_qos.c 
     cint ../../../../src/examples/dpp/utility/cint_utils_global.c
     cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
     cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
     cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c
     cint ../../../../src/examples/dpp/cint_mpls_lsr.c 
     cint ../../../../src/examples/dpp/cint_ip_route.c
     cint ../../../../src/examples/dpp/cint_multi_device_utils.c
     cint ../../../../src/examples/dpp/cint_ip_route_tunnel.c
     cint ../../../../src/examples/dpp/internal/sqa/cint_mpls_l3vpn_comb.c
     cint
     preserve_dscp_per_lif =2;
     int nof_units = 1;
     int units[1] = {0};    
     int outP = 200;
     int inP = 201;
     int outSysport, inSysport;
     print port_to_system_port(units[0], outP, &outSysport);
     print port_to_system_port(units[0], inP, &inSysport);
     print example_ip_to_tunnel_ext_label(units, nof_units, inSysport, outSysport, 4);
     exit;
     debug bcm rx
     tx 1 psrc=201 data=000c000200000000070001008100000f0800458c003d000000008011b11c0a0000057fffff03c351000000290000000102030405060708090a0b0c0d0e0f1011
     
     tx 1 psrc=201 data=000C000200000000070000018100000F86DD60300000002A3BFFFE80000000000000020007FFFE00000120010000000000000000000000000004000102030405
 */


/*
 * encap_mode:
 *  0: C1
 *  1: C1+C2
 *  2: C1+C2+C3
 *  3: B1+C1+C2+C3
 *  4: B1+B2+C1+C2+C3
 *  5: B1+C1+C2
 *  6: B1+B2+C1+C2
 *
 */
int create_C_tunnel(int unit, int encap_mode, int next_pointer)
{
    int rv;

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    uint64 wide_data;
    
    mpls_tunnel_properties.force_flags=1;
    mpls_tunnel_properties.flags=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    mpls_tunnel_properties.label_in = additional_labels_info.C1_label;
    mpls_tunnel_properties.label_out = encap_mode ? additional_labels_info.C2_label : 0;
    mpls_tunnel_properties.ttl = additional_labels_info.C1_ttl;
    mpls_tunnel_properties.ext_ttl = encap_mode ? additional_labels_info.C2_ttl : 0;
    mpls_tunnel_properties.exp = additional_labels_info.C1_exp;
    mpls_tunnel_properties.ext_exp = encap_mode ? additional_labels_info.C2_exp : 0;
    mpls_tunnel_properties.with_exp = 1;
    mpls_tunnel_properties.next_pointer_intf = next_pointer;
    
    mpls_tunnel_properties.flags |= BCM_MPLS_EGRESS_LABEL_WIDE;
    
    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }
    
    additional_labels_info.C_intf = mpls_tunnel_properties.tunnel_id;
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(additional_labels_info.C_gport, mpls_tunnel_properties.tunnel_id);

    /*set for C3*/
    if ((encap_mode == 2) || (encap_mode == 3) || (encap_mode == 4)) {        
        COMPILER_64_SET(wide_data, 0, additional_labels_info.C3_label);        
        rv = bcm_port_wide_data_set(unit, additional_labels_info.C_gport, BCM_PORT_WIDE_DATA_EGRESS/*flags*/, wide_data);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_wide_data_set\n");
            return rv;
        }        
        rv = bcm_port_control_set(unit, additional_labels_info.C_gport, bcmPortControlMPLSEncapsulateAdditionalLabel, 1);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_control_set\n");
            return rv;
        }
    }

    printf("Create C tunnel with encap_mode[%d]\n", encap_mode);
    return rv;
}

int create_B_tunnel(int unit, int encap_mode)
{
    int rv;
    
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    sal_memset(&mpls_tunnel_properties, 0, sizeof(mpls_tunnel_properties));
    mpls_tunnel_properties.force_flags=1;
    mpls_tunnel_properties.flags=BCM_MPLS_EGRESS_LABEL_TTL_COPY|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    mpls_tunnel_properties.label_in = (encap_mode >= 3) ? additional_labels_info.B1_label : 0;
    mpls_tunnel_properties.label_out = (encap_mode & 4) ? additional_labels_info.B2_label : 0;
    mpls_tunnel_properties.ttl = (encap_mode >= 3) ? additional_labels_info.B1_ttl : 0;
    mpls_tunnel_properties.ext_ttl = (encap_mode & 4) ? additional_labels_info.B2_ttl : 0;
    mpls_tunnel_properties.exp = (encap_mode >= 3) ? additional_labels_info.B1_exp : 0;
    mpls_tunnel_properties.ext_exp = (encap_mode & 4) ? additional_labels_info.B2_exp : 0;
    mpls_tunnel_properties.with_exp = 1;
    mpls_tunnel_properties.next_pointer_intf = additional_labels_info.C_intf;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }
    additional_labels_info.B_intf = mpls_tunnel_properties.tunnel_id;

    printf("Create B tunnel with encap_mode[%d]\n", encap_mode);
    return rv;
}


int example_ip_to_tunnel_ext_label(int *units_ids, int nof_units, int in_sysport, int out_sysport, int encap_mode){
    int rv;
    int unit, i;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;
    int route;
    int mask; 
    int tunnel_id, tunnel_id2 = 0;
	int flags = 0;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties2;

    create_l3_intf_s intf;
    uint64 wide_data;
    
    additional_labels_info.B1_label = 0x11;
    additional_labels_info.B2_label = 0x22;
    additional_labels_info.C1_label = 0x33;
    additional_labels_info.C2_label = 0x44;
    additional_labels_info.C3_label = 0x55;
    additional_labels_info.D1_label = 0x66;

    additional_labels_info.B1_ttl = 8;
    additional_labels_info.B2_ttl = 16;
    additional_labels_info.C1_ttl = 32;
    additional_labels_info.C2_ttl = 64;
    additional_labels_info.B1_exp = 4;
    additional_labels_info.B2_exp = 3;
    additional_labels_info.C1_exp = 2;
    additional_labels_info.C2_exp = 1;

    /*** create ingress router interface ***/
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        if (preserve_dscp_per_lif & 0x1) {
            bcm_vlan_port_t vlan_port;
            bcm_vlan_port_t_init(&vlan_port);
            vlan_port.port=in_sysport;
            vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
            vlan_port.match_vlan = in_vlan;
            vlan_port.egress_vlan = in_vlan;
            vlan_port.vsi = in_vlan;
            rv = bcm_vlan_port_create(unit, &vlan_port);
            if (rv != BCM_E_NONE) {
                printf("fail create VLAN port, port(0x%08x), vlan(%d)\n", in_sysport, in_vlan);
                return rv;
            }
            
            rv = bcm_port_control_set(0, vlan_port.vlan_port_id, bcmPortControlPreserveDscpIngress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpIngress failed, port(0x08x)\n", vlan_port.vlan_port_id);
                return rv;
            }
        } else {
            rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
            if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
                printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
              return rv;
            }
        }

        intf.vsi = in_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
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

        intf.vsi = out_vlan;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_out = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /*** create egress object 1 ***/
    /*** Create tunnel ***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    tunnel_id = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];


        rv = create_C_tunnel(unit, encap_mode, ing_intf_out);
        if (rv != BCM_E_NONE) {
            printf("Error, in create_C_tunnel\n");
            return rv;
        }
        
        if (encap_mode >= 3) {
            rv = create_B_tunnel(unit, encap_mode);
            if (rv != BCM_E_NONE) {
                printf("Error, in create_B_tunnel\n");
                return rv;
            }
        }

        if (preserve_dscp_per_lif & 0x2) {
            rv = bcm_port_control_set(unit, additional_labels_info.C_gport, bcmPortControlPreserveDscpEgress, 1);
            if (rv != BCM_E_NONE) {
                printf("bcm_port_control_set bcmPortControlPreserveDscpEgress failed, port(0x08x)\n", gport);
                return rv;
            }
        }
    }

    /*** Create egress object1 with the tunnel_id ***/
    for (i = 0 ; i < nof_units ; i++){
        create_l3_egress_s l3eg;
        unit = units_ids[i]; 
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
        l3eg.allocation_flags = flags;
        l3eg.out_tunnel_or_rif = (encap_mode > 2) ? additional_labels_info.B_intf : additional_labels_info.C_intf;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = fec[0];
        l3eg.arp_encap_id = encap_id[0];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[0] = l3eg.fec_id;
        encap_id[0] = l3eg.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }
    flags = 0;
    
    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    for (i = 0 ; i < nof_units ; i++){
        create_l3_egress_s l3eg1;
        unit = units_ids[i];
        sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
        l3eg1.allocation_flags = flags; 
        l3eg1.out_tunnel_or_rif = tunnel_id2;
        l3eg1.vlan = out_vlan;
        l3eg1.out_gport = out_sysport;
        l3eg1.fec_id = fec[1];
        l3eg1.arp_encap_id = encap_id[1];

        rv = l3__egress__create(unit,&l3eg1);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[1] = l3eg1.fec_id;
        encap_id[1] = l3eg1.arp_encap_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_route(unit, route, mask , vrf, fec[1]); 
        if (rv != BCM_E_NONE) {
            printf("Error, add_route\n");
        }
        rv = add_route_ip6(unit, host_r, mask_l, 0, fec[0]);
        if (rv != BCM_E_NONE) {
            printf("Error, add_route_ip6\n");
        }
    }


    return rv;
}


