/*
 * $Id: cint_ip_ecmp_rpf_examples.c
 * Exp $
 $Copyright: (c) 2020 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$
 */

/*
 * Example of VIP ECMP configuration
 *
 *
 * cint
 * cint_reset();
 * exit;
 * 
 * cint ../../../../src/examples/dnx/cint_sllb_basic.c
 * cint
 * print sllb_basic_example(0 /* unit */, 32 /* sllb_table_size */, 200 /* in_port */, 201 /* out_port */, 4 /* nof_servers */, 52430 /* first_server_fec_id */, 4095 /* vip_id */);
 * exit;
 * 
 * cint
 * print sllb_basic_example_cleanup(0);
 * exit;
 * 
 * 
 * #### Example ingress packets
 *
 * #### VIP DIP -- DIP is a Virtual IP -- Packets are forwarded using the server_fec resolved by VIP ECMP resolution.
 * 
 * ### Eth: IPv4: TCP: SYN 
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: SYN
 * ###
 * tx 1 psrc=200 length=128 data=0x0011223344550066778899AA9100006408004500002A000000004006E7B30A0A0A007FFFFF1110002000000000000000000050020000B4180000000154D23B15
 * 
 * ### Eth: IPv4: TCP: FIN 
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: FIN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006408004500002A000000004006E7B30A0A0A007FFFFF1110002000000000000000000050010000B4190000000121C7D611
 * 
 * ### Eth: IPv6: TCP: SYN 
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: SYN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006486DD60300000001406FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF03333000000000000000000000000000110002000000000000000000050020000C83500003FCABC0F
 * 
 * ### Eth: IPv6: TCP: FIN
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: FIN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006486DD60300000001406FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF03333000000000000000000000000000110002000000000000000000050010000C83600003FCABC0F
 * 
 *
 * #### Direct DIP -- DIP is not a Virtual IP. Packets are forwded usin the LPM result FEC
 *
 * ### Eth: IPv4: TCP: SYN
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: SYN
 * ###
 * tx 1 psrc=200 length=128 data=0x0011223344550066778899AA9100006408004500002A000000004006E7B30A0A0A007FFFFF2110002000000000000000000050020000B4180000000154D23B15
 *
 * ### Eth: IPv4: TCP: FIN
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: FIN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006408004500002A000000004006E7B30A0A0A007FFFFF2110002000000000000000000050010000B4190000000121C7D611
 *
 * ### Eth: IPv6: TCP: SYN
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: SYN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006486DD60300000001406FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF04444000000000000000000000000000110002000000000000000000050020000C83500003FCABC0F
 *
 * ### Eth: IPv6: TCP: FIN
 * ## DA: 00:11:22:33:44:55
 * ## VID: 100
 * ## DIP: 127.255.255.11
 * ## L4: TCP
 * ## L4 flags: FIN
 * ###
 * tx 1 psrc=200 data=0x0011223344550066778899AA9100006486DD60300000001406FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF04444000000000000000000000000000110002000000000000000000050010000C83600003FCABC0F
 *
 */

int MAX_SLLB_TABLE_SIZE = 16384;
struct sllb_basic_data_s
{
    int                     vip_fec_id_min;
    int                     vip_fec_id_max;
    bcm_gport_t             client_port;
    bcm_gport_t             ingress_vlan_port_id;
    bcm_vrf_t               vrf;
    bcm_vlan_t              client_vsi_rif;
    int                     server_vsi_rif;
    bcm_if_t                vip_ecmp_fec_id;

    /* Virutal IP routes */
    bcm_ip_t                vip_dip4;
    bcm_ip_t                vip_dip4_mask;
    bcm_ip6_t               vip_dip6;
    bcm_ip6_t               vip_dip6_mask;

    /* Direct IP  routes */
    bcm_ip_t                direct_dip4;
    bcm_ip_t                direct_dip4_mask;
    bcm_ip6_t               direct_dip6;
    bcm_ip6_t               direct_dip6_mask;

};

sllb_basic_data_s g_sllb_basic_data = {
    /* vip_fec_id_min */
    0,
    /* vip_fec_id_max */
    0,
    /* client_port */
    0,
    /* ingress_vlan_port_id */
    0,
    /* vrf */
    999,
    /* client_vsi_rif */
    100,
    /* server_vsi_rif */
    200,
    /* vip_ecmp_fec_id */
    0,

    /* Virutal IP routes */
    /* vip_dip4 */
    0x7fffff10,
    /* vip_dip4_mask */
    0xfffffff0,
    /* vip_dip6 */
    {0x33, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* vip_dip6_mask */
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0},

    /* Direct IP  routes */
    /* direct_dip4 */
    0x7fffff20,
    /* direct_dip4_mask */
    0xfffffff0,
    /* direct_dip6 */
    {0x44, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* direct_dip6_mask */
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0}
};

struct sllb_basic_arrays_s
{
    int                     arp_intfs[MAX_SLLB_TABLE_SIZE];
    bcm_if_t                server_fecs[MAX_SLLB_TABLE_SIZE];
};

sllb_basic_arrays_s g_sllb_basic_arrays;
/*
 * Configure a user defined trap to be configured as the SLLB default destionation.
 */
int 
sllb_def_dest_trap_set(int unit, bcm_gport_t *sllb_default_destination)
{
    int rv = BCM_E_NONE;
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_def_dest_trap_set: bcm_rx_trap_type_create failed with rv %d\n", rv);
        return rv;
    }

    bcm_rx_trap_config_t_init(&trap_config);
    trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
    trap_config.dest_port = BCM_GPORT_BLACK_HOLE;
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_def_dest_trap_set: bcm_rx_trap_set failed with rv %d\n", rv);
        return rv;
    }

    BCM_GPORT_TRAP_SET(*sllb_default_destination, trap_id, 7, 0);
    return rv;
}

/*
 * Configure the switch level SLLB parameters:
 * bcmSwitchVIPEcmpTableSize: VIP ECMP table size 
 * bcmSwitchVIPL3EgressIDMin: must be in the valid FEC range: (Max ECMP group ID) < bcmSwitchVIPL3EgressIDMin < (maximum supported FEC-Id)
 * bcmSwitchVIPL3EgressIDMax: set to bcmSwitchVIPL3EgressIDMin + 4096 - 1. bcmSwitchVIPL3EgressIDMin must be less than the maximum supported FEC-Id.
 */
int 
sllb_application_config_set(int unit, uint32 sllb_table_size, int *vip_fec_id_min, int *vip_fec_id_max)
{
    int rv = BCM_E_NONE;
    bcm_switch_fec_property_config_t fec_config;
    bcm_gport_t sllb_default_destination;

    rv = bcm_switch_control_set(unit, bcmSwitchVIPEcmpTableSize, sllb_table_size);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_application_config_set: bcm_switch_control_set (bcmSwitchVIPEcmpTableSize) failed with rv %d\n", rv);
        return rv;
    }

    rv = bcm_switch_control_get(unit, bcmSwitchVIPL3EgressIDMin, vip_fec_id_min);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_application_config_set: bcm_switch_control_get (bcmSwitchVIPL3EgressIDMin) failed with rv %d\n", rv);
        return rv;
    }
    
    if (*vip_fec_id_min > 0)
    {
        /* VIP ECMP FEC range has previously been set. It can be set only once after device init. Use configured range. */
        rv = bcm_switch_control_get(unit, bcmSwitchVIPL3EgressIDMax, vip_fec_id_max);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_application_config_set: bcm_switch_control_get (bcmSwitchVIPL3EgressIDMax) failed with rv %d\n", rv);
            return rv;
        }
    }
    else
    {
        /* Configure VIP ECMP FEC range. To avoid reserving valid FECs, reserve FEC-Ids starting with 1st hierarchy end id. */
        fec_config.flags = BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY;
        rv = bcm_switch_fec_property_get(0, &fec_config);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_application_config_set: bcm_switch_fec_property_get (BCM_SWITCH_FEC_PROPERTY_1ST_HIERARCHY) failed with rv %d\n", rv);
            return rv;
        }

        rv = bcm_switch_control_set(unit, bcmSwitchVIPL3EgressIDMin, fec_config.end + 1);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_application_config_set: bcm_switch_control_set (bcmSwitchVIPL3EgressIDMin) failed with rv %d\n", rv);
            return rv;
        }

        rv = bcm_switch_control_get(unit, bcmSwitchVIPL3EgressIDMin, vip_fec_id_min);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_application_config_set: bcm_switch_control_get (bcmSwitchVIPL3EgressIDMin) failed with rv %d\n", rv);
            return rv;
        }

    }
    rv = sllb_def_dest_trap_set(unit, &sllb_default_destination);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_application_config_set: sllb_def_dest_trap_set failed with rv %d\n", rv);
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchSLLBDestinationNotFound, sllb_default_destination);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_application_config_set: bcm_switch_control_set (bcmSwitchSLLBDestinationNotFound) failed with rv %d\n", rv);
        return rv;
    }
    return rv;
}


/*
 * Configure end-to-end routing flow from VIP enabled InRIF to next hop. VIP ECMP resolution distributes L4oIP packets based on 5-tuple hash
 */
int
sllb_basic_example(int unit,  uint32 sllb_table_size, bcm_gport_t in_port, bcm_gport_t out_port, int nof_servers, int first_server_fec_id, int vip_id)
{
    int                 rv = BCM_E_NONE;
    bcm_l3_intf_t       l3if;
    bcm_mac_t           client_my_mac = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    bcm_if_t            client_rif_intf_id;
    bcm_l3_ingress_t    l3_ing_if;
    bcm_if_t            if_id;
    bcm_vlan_port_t     vp;
    bcm_l3_egress_t     l3eg_arp;
    bcm_l3_egress_t     l3eg_fec;
    bcm_if_t            l3egid_null;
    bcm_mac_t           server_my_mac = {0x00, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};
    bcm_if_t            server_rif_intf_id;
    bcm_if_t            vip_ecmp_intf_array[MAX_SLLB_TABLE_SIZE];	/* Current ECMP group configuartion */
    bcm_mac_t           nh_mac = {0x00, 0x01, 0x01, 0x01, 0x00, 0x00};
    int                 server_iter;
    bcm_l3_egress_ecmp_t    ecmp;
    bcm_l3_route_t          l3rt;


    /** 1. Configure switch SLLB parameters **/
    rv = sllb_application_config_set(unit, sllb_table_size, &g_sllb_basic_data.vip_fec_id_min, &g_sllb_basic_data.vip_fec_id_max);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_application_config: sllb_application_config_set failed with rv %d\n", rv);
        return rv;
    }

    /** 2. Configure the ingress (client) interface **/
    /*** 2.a Create client RIF ***/
    rv = bcm_vswitch_create_with_id(unit, g_sllb_basic_data.client_vsi_rif);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
        printf("\n sllb_basic_example: bcm_vswitch_create (client_vsi_rif) failed with rv %d\n", rv);
        return rv;
    }

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_mac_addr = client_my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = g_sllb_basic_data.client_vsi_rif;
    l3if.dscp_qos.qos_map_id = 0;
    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_basic_example: bcm_l3_intf_create (client_vsi_rif) failed with rv %d\n", rv);
        return rv;
    }
    client_rif_intf_id = l3if.l3a_intf_id;

    /*** 2.b. Create client In-AC mapped to client RIF ***/
    bcm_vlan_port_t_init(&vp);
    vp.port = in_port;
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.match_vlan = g_sllb_basic_data.client_vsi_rif;
    vp.flags |= BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vp.vsi = g_sllb_basic_data.client_vsi_rif;
    rv = bcm_vlan_port_create(unit, vp);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_basic_example: bcm_vlan_port_create failed with rv %d\n", rv);
        return rv;
    }
    g_sllb_basic_data.ingress_vlan_port_id = vp.vlan_port_id;

    rv = bcm_vlan_gport_add(unit, g_sllb_basic_data.client_vsi_rif, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_basic_example: bcm_vlan_gport_add failed with rv %d\n", rv);
        return rv;
    }

    /*** 2.c.  Configured RIF -> VRF ***/
    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.vrf = g_sllb_basic_data.vrf;
    l3_ing_if.qos_map_id = 0;
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    l3_ing_if.flags |= BCM_L3_INGRESS_VIP_ENABLED;
    if_id = client_rif_intf_id;
    rv =  bcm_l3_ingress_create(unit, l3_ing_if, &if_id);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_basic_example: bcm_l3_ingress_create failed, rv %l", rv);
        return rv;
    }


    /** 3. Configure the egress (server) interfaces **/

    /*** 3.a. Create server RIF ***/
    rv = bcm_vswitch_create_with_id(unit, g_sllb_basic_data.server_vsi_rif);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {   
        printf("\n sllb_basic_example: bcm_vswitch_create (server_vsi_rif) failed with rv %d\n", rv);
        return rv;
    }

    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_flags = BCM_L3_WITH_ID;
    l3if.l3a_mac_addr = server_my_mac;
    l3if.l3a_intf_id = l3if.l3a_vid = g_sllb_basic_data.server_vsi_rif;
    l3if.dscp_qos.qos_map_id = 0; 
    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("\n sllb_basic_example: bcm_l3_intf_create (server_vsi_rif) failed with rv %d\n", rv);
        return rv;
    }
    server_rif_intf_id = l3if.l3a_intf_id;

    /*** 3.b. Create server Next Hop and ingress FECs ***/
    for (server_iter = 0; server_iter < nof_servers; server_iter++)
    {

        bcm_l3_egress_t_init(&l3eg_arp);
        l3eg_arp.mac_addr = nh_mac;
        l3eg_arp.vlan = g_sllb_basic_data.server_vsi_rif;
        l3eg_arp.flags = 0;
        l3eg_arp.flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;       /* ARP + AC */
        rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg_arp, &l3egid_null);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_basic_example: bcm_l3_egress_create failed with rv %d\n", rv);
            return rv;
        }
        BCM_L3_ITF_SET(g_sllb_basic_arrays.arp_intfs[server_iter], BCM_L3_ITF_TYPE_LIF, l3eg_arp.encap_id);

        bcm_l3_egress_t_init(&l3eg_fec);
        l3eg_fec.intf = l3eg_arp.encap_id;
        l3eg_fec.encap_id = 0;
        l3eg_fec.failover_id = 0;
        l3eg_fec.port = out_port;
        l3eg_fec.flags = 0;
        l3eg_fec.flags2 = 0;
        g_sllb_basic_arrays.server_fecs[server_iter] = first_server_fec_id+server_iter;
        rv = bcm_l3_egress_create(unit, (BCM_L3_INGRESS_ONLY|BCM_L3_WITH_ID), &l3eg_fec, &g_sllb_basic_arrays.server_fecs[server_iter]);
        if (rv != BCM_E_NONE)
        {
            printf("\n sllb_basic_example: bcm_l3_egress_create failed with rv %d\n", rv);
            return rv;
        }
        nh_mac[5]++;    /* Increment NH DA for each server instance */
        if (nh_mac[5] % 256)
        {
            nh_mac[4]++;
        }

    }
    for (; server_iter < MAX_SLLB_TABLE_SIZE; server_iter++)
    {
        g_sllb_basic_arrays.server_fecs[server_iter] = 0;
    }

    /** 4. Create VIP ECMP group and evenly distrubute the servers in the consistent hash table **/
    g_sllb_basic_data.vip_ecmp_fec_id = vip_id + g_sllb_basic_data.vip_fec_id_min;

    /*** 4.a. Fill the VIP ECMP group intf_array evenly distributed with the server fecs ***/
    int member_reference;       /* Index into the VIP ECMP group intf_array */
    int server_index;	        /* Index into the server FEC-Ids created above. */
    for (member_reference = 0; member_reference < sllb_table_size; member_reference++) {
        server_index = member_reference % nof_servers;
        vip_ecmp_intf_array[member_reference] = g_sllb_basic_arrays.server_fecs[server_index];
    }

    /*** 4.b. Create the VIP ECMP group ***/
    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode=BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    ecmp.flags = BCM_L3_WITH_ID;
    BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_data.vip_ecmp_fec_id);
    rv = bcm_l3_egress_ecmp_create(unit, ecmp, sllb_table_size, vip_ecmp_intf_array);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example: bcm_l3_egress_ecmp_create failed, rv %l", rv);
        return rv;
    }
    g_sllb_basic_data.vip_ecmp_fec_id = ecmp.ecmp_intf;

    /** 5. Add routes for VIP traffic **/

    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = g_sllb_basic_data.vip_dip4;
    l3rt.l3a_ip_mask = g_sllb_basic_data.vip_dip4_mask;
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    BCM_L3_ITF_SET(l3rt.l3a_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_data.vip_ecmp_fec_id);
    l3rt.l3a_flags = BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example: bcm_l3_route_add (vip_dip4) failed, rv %l", rv);
        return rv;
    }

    bcm_l3_route_t_init(l3rt);
    sal_memcpy(l3rt.l3a_ip6_net, g_sllb_basic_data.vip_dip6, 16);
    sal_memcpy(l3rt.l3a_ip6_mask, g_sllb_basic_data.vip_dip6_mask, 16);
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    BCM_L3_ITF_SET(l3rt.l3a_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_data.vip_ecmp_fec_id);
    l3rt.l3a_flags = BCM_L3_IP6|BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example: bcm_l3_route_add (vip_dip6) failed, rv %l", rv);
        return rv;
    }

    /** 6. Add routes for non-VIP (simple routed) traffic **/
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = g_sllb_basic_data.direct_dip4;
    l3rt.l3a_ip_mask = g_sllb_basic_data.direct_dip4_mask;
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    BCM_L3_ITF_SET(l3rt.l3a_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_arrays.server_fecs[0]);
    l3rt.l3a_flags = BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example: bcm_l3_route_add (direct_dip4) failed, rv %l", rv);
        return rv;
    }

    bcm_l3_route_t_init(l3rt);
    sal_memcpy(l3rt.l3a_ip6_net, g_sllb_basic_data.direct_dip6, 16);
    sal_memcpy(l3rt.l3a_ip6_mask, g_sllb_basic_data.direct_dip6_mask, 16);
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    BCM_L3_ITF_SET(l3rt.l3a_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_arrays.server_fecs[0]);
    l3rt.l3a_flags = BCM_L3_IP6|BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example: bcm_l3_route_add (direct_dip6) failed, rv %l", rv);
        return rv;
    }

    return rv;
}

/*
 * Release allocated resources 
 */
int
sllb_basic_example_cleanup(int unit)
{
    int rv = BCM_E_NONE;
    /* Delete routes */
    bcm_l3_route_t l3rt;
    bcm_l3_egress_ecmp_t ecmp;
    int server_iter;
    bcm_l3_intf_t l3_intf;

    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = g_sllb_basic_data.direct_dip4;
    l3rt.l3a_ip_mask = g_sllb_basic_data.direct_dip4_mask;
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    l3rt.l3a_flags = BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_delete(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_route_delete failed, rv %l", rv);
        return rv;
    }
    bcm_l3_route_t_init(l3rt);
    sal_memcpy(l3rt.l3a_ip6_net, g_sllb_basic_data.direct_dip6, 16);
    sal_memcpy(l3rt.l3a_ip6_mask, g_sllb_basic_data.direct_dip6_mask, 16);
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    l3rt.l3a_flags = BCM_L3_IP6|BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_delete(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_route_delete failed, rv %l", rv);
        return rv;
    }

    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = g_sllb_basic_data.vip_dip4;
    l3rt.l3a_ip_mask = g_sllb_basic_data.vip_dip4_mask;
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    l3rt.l3a_flags = BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_delete(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_route_delete failed, rv %l", rv);
        return rv;
    }
    bcm_l3_route_t_init(l3rt);
    sal_memcpy(l3rt.l3a_ip6_net, g_sllb_basic_data.vip_dip6, 16);
    sal_memcpy(l3rt.l3a_ip6_mask, g_sllb_basic_data.vip_dip6_mask, 16);
    l3rt.l3a_vrf = g_sllb_basic_data.vrf;
    l3rt.l3a_flags = BCM_L3_IP6|BCM_L3_INTERNAL_ROUTE;
    rv = bcm_l3_route_delete(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_route_delete failed, rv %l", rv);
        return rv;
    }

    bcm_l3_egress_ecmp_t_init(&ecmp);
    ecmp.dynamic_mode=BCM_L3_ECMP_DYNAMIC_MODE_VIP;
    BCM_L3_ITF_SET(ecmp.ecmp_intf, BCM_L3_ITF_TYPE_FEC, g_sllb_basic_data.vip_ecmp_fec_id);
    rv = bcm_l3_egress_ecmp_destroy(unit, ecmp);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_egress_ecmp_destroy failed, rv %l", rv);
        return rv;
    }

    for (server_iter = 0; server_iter < MAX_SLLB_TABLE_SIZE; server_iter++)
    {
        if (g_sllb_basic_arrays.server_fecs[server_iter] == 0)
        {
            /* Done */
            break;
        }
        rv = bcm_l3_egress_destroy(unit, g_sllb_basic_arrays.arp_intfs[server_iter]);
        if (rv != BCM_E_NONE) {
            printf("\n sllb_basic_example_cleanup: bcm_l3_egress_destroy (arp_intfs) failed, rv %l", rv);
            return rv;
        }
        rv = bcm_l3_egress_destroy(unit, g_sllb_basic_arrays.server_fecs[server_iter]);
        if (rv != BCM_E_NONE) {
            printf("\n sllb_basic_example_cleanup: bcm_l3_egress_destroy (server_fecs) failed, rv %l", rv);
            return rv;
        }
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = g_sllb_basic_data.client_vsi_rif;
    rv = bcm_l3_intf_delete(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_intf_delete (client_vsi_rif) failed, rv %l", rv);
        return rv;
    }

    rv = bcm_vlan_gport_delete(unit, g_sllb_basic_data.client_vsi_rif, g_sllb_basic_data.client_port);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_vlan_gport_delete (in_ac) failed, rv %l", rv);
        return rv;
    }


    rv = bcm_vlan_port_destroy(unit, g_sllb_basic_data.ingress_vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_vlan_port_destroy (in_ac) failed, rv %l", rv);
        return rv;
    }

    rv = bcm_vswitch_destroy(unit, g_sllb_basic_data.client_vsi_rif);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_vswitch_destroy (client_vsi_rif) failed, rv %l", rv);
        return rv;
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = g_sllb_basic_data.server_vsi_rif;
    rv = bcm_l3_intf_delete(unit, &l3_intf);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_l3_intf_delete (server_vsi_rif) failed, rv %l", rv);
        return rv;
    }


    rv = bcm_vswitch_destroy(unit, g_sllb_basic_data.server_vsi_rif);
    if (rv != BCM_E_NONE) {
        printf("\n sllb_basic_example_cleanup: bcm_vswitch_destroy (server_vsi_rif) failed, rv %l", rv);
        return rv;
    }

    return rv;
}

