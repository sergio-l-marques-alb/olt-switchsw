
/*
 * 1. SOC config:
 *        custom_feature_ch_enable=1
 *        custom_feature_conn_to_np_debug=0
 *
 * 2. Calling sequence:
 *   call following API to port-x to CH encapsulation.     
 *        bcm_port_control_set(0,X,bcmPortControlReserved280,1);
 *
 *
 *
 */


bcm_mac_t dest_mac = {0x00,0x00,0x00,0x00,0x00,0x12};
bcm_mac_t next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02};


int config_ch_tc(int unit, bcm_port_t in_port, int tc_value)
{
    int rv = 0;

    rv = bcm_port_vlan_priority_map_set(unit, in_port, 0, 0, tc_value, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_vlan_priority_map_set $rv\n");
        return rv;
    }
    return rv;
}

/* value: 1-enable, 0-disable */
int config_ch_port(int unit, bcm_port_t port, int value)
{
    int rv = 0;

    rv = bcm_port_control_set(unit,port,bcmPortControlReserved280,value);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set $rv\n");
        return rv;
    }
    return rv;
}

/* packet:
    cint ../../../../src/examples/dpp/cint_ch_conn_to_fpga.c
    cint
    int in_port=13;
    int out_port=200;
    int tc_value=3;    
    print config_ch_tc(0,in_port,tc_value);
    print config_ch_port(0,out_port,1);    
    print ch_conn_to_fpga_L2(0,100,in_port,out_port);
    exit;    

    P2P:
    tx 1 psrc=13 data=00000000001100000000fff28100006408004500002E0000000040FF00046465000215670000000102030405060708090A0B0C0D0E0F1011121314151617181920212234BEC62B
    L2 switch:
    tx 1 psrc=14 data=00000000001200000000fff28100006408004500002E0000000040FF00046465000215670000000102030405060708090A0B0C0D0E0F1011121314151617181920212234BEC62B

    PTP packet:
    tx 1 psrc=13 data=0x011b19000000000e030001098100000288f70002002c00000000000000000000000000000000000e03fffe01090001001e00000000559b4e5b241ac0c42719b44b
*/

int ch_conn_to_fpga_L2(int unit, uint16 vid, bcm_gport_t in_port, bcm_gport_t out_port)
{
    int rv = 0;

    /*static L2*/
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, dest_mac, vid);
    l2addr.port = out_port;
    l2addr.vid = vid;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.encap_id = BCM_FORWARD_ENCAP_ID_INVALID;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }

    /*P2P*/
    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vlan_port_t_init(&vlan_port_1);
    bcm_vlan_port_t_init(&vlan_port_2);
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port_1.port = in_port;
    vlan_port_2.port = out_port;
    
    rv = bcm_vlan_port_create(0, &vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_port_create(0, &vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    bcm_vswitch_cross_connect_t gports;
    gports.port1= vlan_port_1.vlan_port_id;
    gports.port2= vlan_port_2.vlan_port_id;
    gports.encap1=BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2=BCM_FORWARD_ENCAP_ID_INVALID;
    rv = bcm_vswitch_cross_connect_add(0, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        return rv;
    }
    
    return BCM_E_NONE;
}

/*

    cint ../../../../src/examples/dpp/utility/cint_utils_global.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l2.c
    cint ../../../../src/examples/dpp/utility/cint_utils_mpls.c
    cint ../../../../src/examples/dpp/cint_multi_device_utils.c
    cint ../../../../src/examples/dpp/cint_qos.c
    cint ../../../../src/examples/dpp/cint_mpls_lsr.c
    cint ../../../../src/examples/dpp/cint_ch_conn_to_fpga.c

    c
    int in_port=13;
    int out_port=200;
    int tc_value=3;
    print config_ch_tc(0,in_port,tc_value);
    print config_ch_port(0,out_port,1);
    print ch_conn_to_fpga_mpls(0,in_port,out_port);
    exit;
    debug bcm rx

    tx 1 psrc=13 data=0000000000110000030007008100000a8847001900ff003e81ff000102030405060708090A0B0C0D0E0F10111213141516171819000102030405060708090A0B0C0D0E0F10111213141516171819

*/

int ch_conn_to_fpga_mpls(int unit, bcm_gport_t in_port, bcm_gport_t out_port)
{
    return lsr_basic_example_single_unit(unit, in_port, out_port);
}


/*
    cint ../../../../src/examples/dpp/cint_ipfpm.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
    cint ../../../../src/examples/dpp/cint_ipmc.c
    cint ../../../../src/examples/dpp/cint_ch_conn_to_fpga.c
    cint
    int in_port=13;
    int out_port=200;
    int tc_value=3;
    print config_ch_tc(0,in_port,tc_value);
    print config_ch_port(0,out_port,1);
    print ch_conn_to_fpga_ipv4(0,in_port,out_port);
    exit;

    UC:
    tx 1 psrc=13 data=0000000000010000000000028100006408004506002E0000000041FFFEFD6465000215670000000102030405060708090A0B0C0D0E0F10111213141516171819B5982A78
    MC:
    tx 1 psrc=13 data=000000000012000000000002810000c808004506002E0000000041FFFEFD00000001E0000001000102030405060708090A0B0C0D0E0F10111213141516171819B5982A78
    PTP:
    tx 1 psrc=13 data=0000000000010000000000668100006408004500004E00000000401100d26465000215670000013f013f03400000002002c00000400000000000000000000000000000000fffe0000660001226800fa00000000009a062f7c0000
*/
int ch_conn_to_fpga_ipv4(int unit, bcm_gport_t in_port, bcm_gport_t out_port)
{
    int rv = 0;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    int ud;

    /*ipv4 unicast, vlan=100*/
    rv = config_ipv4_route(unit, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, config_ipv4_route\n");
        return rv;
    }

    /*ipv4 multicast*/
    create_l3_intf_s intf;
    intf.vsi = 200;
    intf.my_global_mac = dest_mac;
    intf.my_lsb_mac = dest_mac;
    intf.vrf_valid = 1;
    intf.vrf = 200;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    MulticastNum = 1;    
    mcL3Create(unit);
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add IPv4 MC entries */
    addMulticast(unit, 200);
    /* Traverse IPv4 IPMC table */
    rv = bcm_ipmc_traverse(unit, 0, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_traverse\n");
        return rv;
    }

    mc_id=mc.ipmc_index;
    
    BCM_GPORT_LOCAL_SET(dest_gport,out_port);
    rv = bcm_multicast_ingress_add(unit, mc_id, dest_gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    return rv;
}

/*
    cint ../../../../src/examples/dpp/utility/cint_utils_global.c
    cint ../../../../src/examples/dpp/cint_ipfpm.c
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
    cint ../../../../src/examples/dpp/cint_ipmc.c
    cint ../../../../src/examples/dpp/cint_ip_route.c
    cint ../../../../src/examples/dpp/cint_ipv6_fap.c
    cint ../../../../src/examples/dpp/cint_ch_conn_to_fpga.c
    c
    int in_port=13;
    int out_port=200;
    int tc_value=3;
    print config_ch_tc(0,in_port,tc_value);
    print config_ch_port(0,out_port,1);
    print ch_conn_to_fpga_ipv6(0,in_port,out_port);
    exit;
    debug bcm rx
    MC:
    tx 1 psrc=13 data=000000000012000003000700810000C886DD6030000000263BFFFE80000000000000020003FFFE000700ff000000000000000000000000000001000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425459BFE93
    UC:
    tx 1 psrc=13 data=0000000000120000030007008100006486DD6030000000263BFFFE80000000000000020003FFFE000700fec00000000000000000000000000002000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F202122232425459BFE93
*/

int ch_conn_to_fpga_ipv6(int unit, bcm_gport_t in_port, bcm_gport_t out_port)
{
    int rv = 0;
    bcm_gport_t dest_gport;
    bcm_multicast_t mc_id;
    int ud;

    int fec; 
    int encap_id; 

    /*ipv6 unicast*/
    create_l3_intf_s intf;
    intf.vsi = 100;
    intf.my_global_mac = dest_mac;
    intf.my_lsb_mac = dest_mac;
    intf.vrf_valid = 1;
    intf.vrf = 100;
    
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    intf.vsi = 300;    
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** Create egress object1 ***/
    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = intf.rif;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac, 6);
    l3eg.vlan   = 300;
    l3eg.allocation_flags = 0; 
    l3eg.out_gport = out_port;

    rv = l3__egress__create(unit,&l3eg);
     if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
    }

    encap_id = l3eg.arp_encap_id;
    fec = l3eg.fec_id;

    rv = add_ipv6_host(unit, &dst_ipv6, 100, fec, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, add ipv6 host, in unit %d\n", unit);
    }

    /*ipv6 multicast*/
    intf.vsi = 200;
    intf.my_global_mac = dest_mac;
    intf.my_lsb_mac = dest_mac;
    intf.vrf_valid = 1;
    intf.vrf = 200;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    MulticastNum = 1;    
    mcL3Create(unit);
    rv = bcm_ipmc_remove_all(unit);
    if (rv != BCM_E_NONE) {
        printf("bcm_ipmc_delete_all returned with failure code '%s'\n", bcm_errmsg(rv));
        return rv;
    }

    /* Add IPv6 MC entries */
    addMulticastv6(unit, 200);
    /* Traverse IPv6 IPMC table */
    rv = bcm_ipmc_traverse(unit, BCM_IPMC_IP6, ipmcTraversePrintCb, &ud);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_traverse\n");
        return rv;
    }

    mc_id=mc.ipmc_index;

    BCM_GPORT_LOCAL_SET(dest_gport,out_port);
    rv = bcm_multicast_ingress_add(unit, mc_id, dest_gport, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    return rv;
}


/* config for egress mirror from CR header port */
int ch_conn_to_fpga_egress_mirror(int unit, bcm_port_t in_port, bcm_port_t out_port, bcm_port_t mir_port)
{
    int rv = 0;
    bcm_pbmp_t pbmp,ubmp;
    int vlan=100;
    int mc_group1;
    bcm_vlan_port_t vp = {0};  /*AC1*/
    int gport; /* Ingress gport */ 

    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    BCM_PBMP_PORT_ADD(pbmp, out_port);
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_create\n");
        return rv;
    }
    rv = bcm_vlan_port_add(unit, vlan , pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add\n");
        return rv;
    }

    vp.port = in_port;   
    vp.match_vlan = vlan;
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    mc_group1 = vlan;
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &mc_group1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create\n");
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vlan, vp.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    rv = bcm_multicast_ingress_add (unit, mc_group1, in_port, vp.encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    rv = bcm_multicast_ingress_add(unit, mc_group1, out_port, -1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_ingress_add\n");
        return rv;
    }

    /*create egress mirror*/
    rv = set_port_mirror(unit, out_port, mir_port, BCM_MIRROR_PORT_EGRESS); 
    if (rv != BCM_E_NONE) {
        printf("Error, set_port_mirror\n");
        return rv;
    }

    return rv;
}


