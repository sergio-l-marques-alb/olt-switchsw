
/*
 * Examples of Jericho2-only tunnel termination
 *
 *
 * Description:
 * -The Cint is full Demo of traversing an IPv4oETH packet through an SRv6 Tunnel/Overlay.
 * -The Demo includes the following scenarios:
 * -------------------------------------
 * 1.IPv4oETH packet traverses through an SRv6 Ingress Node to add SRv6 and IPv6 Encapsulation,
 *   and output IPv4oSRv6_3oIPv6oETH packet (SRv6_3 means with SRH header with x3 SIDs)
 * 2.Packet from scenario (1) IPv4oSRv6_3oIPv6oETH through an SRv6 Endpoint Node that performs
 *   header editing - SRH (update SL) and IPv6 (update DIP to next SID)
 * 3.Packet from scenario (2) IPv4oSRv6_3oIPv6oETH through next SRv6 Endpoint Node that performs
 *   header editing - SRH (update SL) and IPv6 (update DIP to next SID)
 * 4.Packet from scenario (3) IPv4oSRv6_3oIPv6oETH through an SRv6 Egress USP Node that performs
     on 1st Pass - Termination of SRv6 and IPv6 Header
     on 2nd Pass - IPv4 Forwarding
     and creates an IPv4oETH packet
 *********************************
 *       Device  Demo Run        *
 *********************************
 *
 * Add/modify following SOC Properties to "config-jer2.bcm":
 * -----------------------------
 * tm_port_header_type_in_0=ETH
 * tm_port_header_type_in_200=ETH
 * tm_port_header_type_in_201=ETH
 * tm_port_header_type_in_202=ETH
 * tm_port_header_type_in_203=ETH
 * tm_port_header_type_in_40=ETH
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0:core_1.40
 * appl_enable_field_srv6=1
 *
 * Shell Commands:
 * ---------------
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/tunnel/cint_dnx_srv6_tunnel_full_flow_example.c
 *
 * cint;
 * int rv;
 * rv = dnx_basic_example_srv6_full_flow(0,200,202,202,202,202,201,201,40,203);
 * print rv;
 * exit;
 *
 * Srv6 Ingress Node:
 * Send a NoN-PTCH packet (from port=1) data = 000c00020000000007000100080045000035000000008000fa45c08001017fffff0200010203405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 0x00000000CD1D00123456789A8100006586DD
 *        60006DCB006D2B80 123456789ABCEEFFFFEECBA98765432 111112222333344445555666677778888
 *        0406000202000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 End-Point-1 Node:
 * Send a NoN-PTCH packet (from port=1) data = 00000000cd1d00123456789a8100006586dd60006dcb00952b80123456789abceeffffeecba97654321111122223333444455556666777788880406000202000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeff0ff11112222111122223333444455556666777788884500003500000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 00000001CD1D00213456789A8100006686DD
 *        60006DCB00952B7F 123456789ABCEEFFFFEECBA987654321 AAAABBBBCCCCDDDDEEEEFFFF11112222
 *        0406000102000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 End-Point-2 Node:
 * Send a NoN-PTCH packet (from port=1) data = 00000002cd1d00223456789a8100006486dd60006dcb00952b7e123456789abceeffffeecba987654321abcddbca12344321101098985679abc0406000002000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff1111222211112222333344445555660667777888845000035000000007f00fb45c08001017fffff0200010030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 00000002CD1D00223456789A8100006486DD
 *        60006DCB00952B7E 123456789ABCEEFFFFEECBA987654321 ABCDDBCA123443211010989845679ABC
 *        0406000002000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 Egress Node:
 * Send a NoN-PTCH packet (from port=1) data = 000000002cd1d00223456789a8100006486dd60006dcb00952b7e123456789abceeffffeecba97654321abcddbca123443211010989845679abc0406000002000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeff0ff11112222111122223333444455556666777788884500003500000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20*
 * Received packets on unit 0 should be:
 * data = 00000003CD1D00303456789A810000680800
 *        45000035000000007E00FC45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *********************************
 *       C-Model  Demo Run       *
 *********************************
 *
 * Add/modify following SOC Properties to "config-jer2.bcm":
 * -----------------------------
 * tm_port_header_type_in_40=ETH
 * tm_port_header_type_out_40=ETH
 * ucode_port_40=RCY.0:core_0.40
 * appl_enable_field_srv6=1
 *
 * Shell Commands:
 * ---------------
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../src/examples/dnx/tunnel/cint_dnx_srv6_tunnel_full_flow_example.c
 *
 * cint;
 * int rv;
 * rv = dnx_basic_example_srv6_full_flow(0,1,202,1,202,1,201,1,40,203);
 * print rv;
 * exit;
 *
 * Srv6 Ingress Node:
 *
 * Send a NoN-PTCH packet (from port=1) data = 000c00020000000007000100080045000035000000008000fa45c08001017fffff0200010203405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 0x00000000CD1D00123456789A8100006586DD
 *        60006DCB006D2B80 123456789ABCEEFFFFEECBA98765432 111112222333344445555666677778888
 *        0406000202000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 End-Point-1 Node:
 * Send a NoN-PTCH packet (from port=1) data = 00000000cd1d00123456789a8100006586dd60006dcb00952b80123456789abceeffffeecba97654321111122223333444455556666777788880406000202000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeff0ff11112222111122223333444455556666777788884500003500000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 00000001CD1D00213456789A8100006686DD
 *        60006DCB00952B7F 123456789ABCEEFFFFEECBA987654321 AAAABBBBCCCCDDDDEEEEFFFF11112222
 *        0406000102000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 End-Point-2 Node:
 * Send a NoN-PTCH packet (from port=1) data = 00000002cd1d00223456789a8100006486dd60006dcb00952b7e123456789abceeffffeecba987654321abcddbca12344321101098985679abc0406000002000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeffff1111222211112222333344445555660667777888845000035000000007f00fb45c08001017fffff0200010030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Received packets on unit 0 should be:
 * data = 00000002CD1D00223456789A8100006486DD
 *        60006DCB00952B7E 123456789ABCEEFFFFEECBA987654321 ABCDDBCA123443211010989845679ABC
 *        0406000002000000 ABCDDBCA123443211010989845679ABC AAAABBBBCCCCDDDDEEEEFFFF11112222 11112222333344445555666677778888
 *        45000035000000007F00FB45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 *
 * Srv6 Egress Node:
 * Send a NoN-PTCH packet (from port=1) data = 000000002cd1d00223456789a8100006486dd60006dcb00952b7e123456789abceeffffeecba97654321abcddbca123443211010989845679abc0406000002000000abcddbca123443211010989845679abcaaaabbbbccccddddeeeeff0ff11112222111122223333444455556666777788884500003500000007f00fb45c08001017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20*
 * Received packets on unit 0 should be:
 * data = 00000003CD1D00303456789A810000680800
 *        45000035000000007E00FC45C08001017FFFFF02000102030405060708090A0B0C0D0E0F101112131415161718191A1B1C1D1E1F20
 */

/** \brief List of SRv6 used instances */
enum srv6_instances_e
{
    INGRESS,
    END_POINT_1,
    END_POINT_2,
    EGRESS,
    RCH
};

 int NOF_INSTANCES = RCH+1;

/** \brief List of SRv6 used FEC ids */
enum srv6_fec_id_e
{
    INGRESS_SRH_FEC_ID,
    INGRESS_FIRST_SID_FEC_ID,
    END_POINT_1_FEC_ID,
    END_POINT_2_FEC_ID,
    EGRESS_FEC_ID
};

int NOF_FEC_ID = EGRESS_FEC_ID+1;

/** \brief List of SRv6 used VIDs */
enum srv6_vid_e
{
    END_POINT_1_VID,
    END_POINT_2_VID,
    EGRESS_VID,
    EGRESS_OUT_VID
};

int NOF_VID = EGRESS_OUT_VID+1;

/** \brief List of SRv6 used Global LIFs */
enum srv6_global_lifs_e
{
    INGRESS_SRH_GLOBAL_LIF,
    INGRESS_FIRST_SID_GLOBAL_LIF
};

int NOF_GLOBAL_LIFS=INGRESS_FIRST_SID_GLOBAL_LIF+1;

/** \brief List of SRv6 used Terminated SIDs */
enum srv6_terminated_sids_e
{
    SID2,
    SID1,
    SID0
};

int NOF_SIDS = SID0+1;


struct cint_srv6_tunnel_info_s
{
    int eth_rif_intf_in[NOF_INSTANCES];                 /* in RIF */
    int eth_rif_intf_out[NOF_INSTANCES-1];              /* out RIF */
    bcm_mac_t intf_in_mac_address[NOF_INSTANCES];       /* mac for in RIF */
    bcm_mac_t intf_out_mac_address[NOF_INSTANCES];      /* mac for in RIF */
    bcm_mac_t arp_next_hop_mac[NOF_INSTANCES];          /* mac for next hop */
    int vrf_in[NOF_INSTANCES];                          /* VRF, resolved as Tunnel-InLif property*/
    int tunnel_arp_id[NOF_INSTANCES-1];                 /* Id for ARP entry */
    int tunnel_fec_id[NOF_FEC_ID];                      /* FEC id */
    int tunnel_global_lif[NOF_GLOBAL_LIFS];             /* Global LIFs */
    int tunnel_vid[NOF_VID];                            /* VID */
    bcm_ip_t route_ipv4_dip;                            /* IPv4 Route DIP */
    bcm_ip6_t tunnel_ip6_dip[NOF_SIDS];  /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_sip;                           /* IPv6 Tunnel SIP */
};


cint_srv6_tunnel_info_s cint_srv6_tunnel_info =
{
        /*
         * eth_rif_intf_in
         */
         {15, 16, 17, 18, 19},
        /*
         * eth_rif_intf_out
         */
         {100, 101, 102, 103},
        /*
         * intf_in_mac_address
         */
         {{ 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 },  { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },  { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 }},
        /*
         * intf_out_mac_address
         */
         {{ 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x21, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x22, 0x34, 0x56, 0x78, 0x9a },
          { 0x00, 0x30, 0x34, 0x56, 0x78, 0x9a }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * arp_next_hop_mac
         */
         {{ 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x01, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x02, 0xcd, 0x1d },
          { 0x00, 0x00, 0x00, 0x03, 0xcd, 0x1d }, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } },
        /*
         * In VRF
         */
         {1, 11, 12, 14, 14},
         /*
          * tunnel_arp_id
          */
         {0x1384, 0x1385, 0x1386, 0x1387},
        /*
         * tunnel_fec_id
         */
         {0xA711, 0xD711, 0xA001, 0xA002, 0xA003},
         /*
          * tunnel_global_lifs
          */
         {0x1230, 0x1231},
         /*
          * tunnel_vid
          */
         {101, 102, 100, 104},
        /*
         * route_ipv4_dip
         */
        0x7fffff02 /* 127.255.255.02 */,
        /*
         * terminated_ip6_dips
         */
        {{ 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 },
         { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x22, 0x22 },
         { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc }},
         /*
          * tunnel_ip6_sip
          */
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 }
};

/*
 * Set In-Port default ETH-RIF:
 * - in_port: Incoming port ID
 * - in_port: Incoming VLAN ID
 * - eth_rif: ETH-RIF
 */
int
in_port_vid_intf_set(
    int unit,
    int in_port,
    int in_vid,
    int eth_rif)
{
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;

    proc_name = "in_port_vid_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.match_vlan = in_vid;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.vsi = eth_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }

    printf("%s(): port = %d, in_lif = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vlan_port_id);
    rc = bcm_vlan_gport_add(unit, in_vid, in_port, 0);
    if (rc != BCM_E_NONE)
    {
        printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
        return rc;
    }

    return rc;
}

/*
 * Configures MyDip of an IPv6 address in an SRv6 tunnel
 */
int
srv6_tunnel_termination(
                    int unit,
                    int end_point_id,
                    int vrf)
{

    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    bcm_ip6_t ip6_dip  = {0};
    bcm_ip6_t ip6_sip  = {0};

    print(cint_srv6_tunnel_info);

    sal_memcpy(ip6_sip, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);
    sal_memcpy(ip6_dip, cint_srv6_tunnel_info.tunnel_ip6_dip[end_point_id], 16);

    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = vrf;
    /* Following is for configuring for the IPv6 LIF, to increase the strength of its TTL
     * so that terminated IPv6 header's TTL would get to sysh, from which the IPv6 TTL decrease
     * would decrease from
     */
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d, end-point-%d\n", rv, end_point_id);
        return rv;
    }

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = vrf;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }

    return BCM_E_NONE;
}



int
dnx_basic_example_srv6_full_flow(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int in_port_end1,        /** Incoming port of SRV6 End Point 1 */
                            int out_port_end1,       /** Outgoing port of SRV6 End Point 1 */
                            int in_port_end2,        /** Incoming port of SRV6 End Point 2 */
                            int out_port_end2,       /** Outgoing port of SRV6 End Point 2 */
                            int in_port_egress,      /** Incoming port of SRV6 Egress */
                            int rch_port,            /** Rycling port number for 1st Pass USP in Egress */
                            int out_port_egress)     /** Outgoing port of SRV6 Egress */
{

    int rv;
    char *proc_name = "dnx_basic_example_srv6_full_flow";

    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /** SRV6 Ingress Tunnel Eth RIF based on Port only */
    rv = in_port_intf_set(unit, in_port, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
        return rv;
    }

    /** End-Point-1 Eth RIF based on Port + VLAN ID so that we can re-use same port */
    rv = in_port_vid_intf_set(unit, in_port_end1, cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port_end1\n",proc_name);
        return rv;
    }

    /** End-Point-2 Eth RIF based on Port + VLAN ID so that we can re-use same port */
    rv = in_port_vid_intf_set(unit, in_port_end2, cint_srv6_tunnel_info.tunnel_vid[END_POINT_2_VID], cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_2]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port_end2\n",proc_name);
        return rv;
    }

    /** Egress Eth RIF based on Port + VLAN ID so that we can re-use same port */
    rv = in_port_vid_intf_set(unit, in_port_egress, cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set in_port = %d, intf_in = %d, err_id = %d\n", proc_name, in_port_egress, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS],
               rv);
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */

    /** In SRV6 Ingress and End-Point-1 use the same out_port, (End-Point-1 and End-Point-2 receive from same Port, with different VLAN) */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of SRV6 Ingress Tunnel %d\n",proc_name,out_port);
        return rv;
    }

    /** End-Point-2 out_port is different from above, to initiate Egress Tunnel */
    rv = out_port_set(unit, out_port_end2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port of End-Point-2 %d\n",proc_name,out_port_end2);
        return rv;
    }

    /** Egress out_port */
    rv = out_port_set(unit, out_port_egress);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set out_port = %d, err_id = %d\n", proc_name, out_port_egress, rv);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */

    /** SRV6 Ingress Tunnel My-MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS], cint_srv6_tunnel_info.intf_in_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS]);
        return rv;
    }

    /** SRV6 Ingress Tunnel out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** End-Point-1 My-MAC - set to SRV6 Ingress Tunnel next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1], cint_srv6_tunnel_info.intf_in_mac_address[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1]);
        return rv;
    }

    /** End-Point-1 out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.intf_out_mac_address[END_POINT_1]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** End-Point-2 My-MAC - set to End-Point-1 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_2], cint_srv6_tunnel_info.intf_in_mac_address[END_POINT_2]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_2]);
        return rv;
    }

    /** End-Point-2 out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_2], cint_srv6_tunnel_info.intf_out_mac_address[END_POINT_2]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** Egress My-MAC - set to End-Point-2 next hop MAC */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.intf_in_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], rv);
        return rv;
    }

    /** RCH Eth_Rif My MAC  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_in[RCH], cint_srv6_tunnel_info.intf_in_mac_address[RCH]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,cint_srv6_tunnel_info.eth_rif_intf_in[RCH]);
        return rv;
    }

    /** Egress out-port SA  */
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], cint_srv6_tunnel_info.intf_out_mac_address[EGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], rv);
        return rv;
    }

    /*
     * 4. Set Incoming ETH-RIF properties
     */

    l3_ingress_intf ingress_rif;

    /** SRV6 Ingress Tunnel VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[INGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[INGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** End-Point-1 VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_1];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_1];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create End-Point-1\n",proc_name);
        return rv;
    }

    /** End-Point-2 VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[END_POINT_2];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[END_POINT_2];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create End-Point-2\n",proc_name);
        return rv;
    }

    /** Egress Tunnel - 1st pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[EGRESS];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in = %d, vrf = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.eth_rif_intf_in[EGRESS], cint_srv6_tunnel_info.vrf_in[EGRESS], rv);
        return rv;
    }

    /** Egress Tunnel - RCH - 2nd pass VRF */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_srv6_tunnel_info.vrf_in[RCH];
    ingress_rif.intf_id = cint_srv6_tunnel_info.eth_rif_intf_in[RCH];
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid[END_POINT_1_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** End-Point-1 ARP create, with VLAN ID of expected in End-Point-2*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], cint_srv6_tunnel_info.arp_next_hop_mac[END_POINT_1], cint_srv6_tunnel_info.tunnel_vid[END_POINT_2_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in End-Point-1\n",proc_name);
        return rv;
    }

    /** End-Point-2 ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_2], cint_srv6_tunnel_info.arp_next_hop_mac[END_POINT_2], cint_srv6_tunnel_info.tunnel_vid[EGRESS_VID], 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in End-Point-2\n",proc_name);
        return rv;
    }

    /** Egress ARP create, with VLAN ID of Egress Tunnel, however there we will look on only the Port */
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], 0,flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only: encap_id = %d, vsi = %d, err_id = %d\n", proc_name,
                cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], cint_srv6_tunnel_info.tunnel_vid[EGRESS_OUT_VID], rv);
        return rv;
    }



    /*
     * 6. SRV6 FEC Entries
     */

    bcm_gport_t gport;

    int srv6_basic_lif_encode;
    int encoded_fec1;
    BCM_L3_ITF_SET(srv6_basic_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID]);

    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID], 0 , srv6_basic_lif_encode, gport, 0,
                                           0, 0,&encoded_fec1);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC_1 only\n",proc_name);
        return rv;
    }

    int srv6_first_lif_encode;
    int encoded_fec2;
    BCM_L3_ITF_SET(srv6_first_lif_encode, BCM_L3_ITF_TYPE_LIF, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_GLOBAL_LIF]);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID], 0 , srv6_first_lif_encode, gport, BCM_L3_2ND_HIERARCHY,
                                           0, 0,&encoded_fec2);

    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC_2 only, fec\n");
        return rv;
    }

    /** End-Point-1 FEC Entry - tying end-point-1 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port_end1);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only for End-Point-1: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_1], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_1], out_port_end1, rv);
        return rv;
    }

    /** End-Point-2 FEC Entry - tying end-point-2 OUT-RIF with its OUT_Port, (ARP - DA and VLAN ID) */
    BCM_GPORT_LOCAL_SET(gport, out_port_end2);
    rv = l3__egress_only_fec__create(unit, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_2], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_2], gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf
            ("%s(): Error, create egress object FEC only for End-Point-2: fec = %d, intf_out = %d, encap_id = %d, out_port = %d, err_id = %d\n",
             proc_name, cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], cint_srv6_tunnel_info.eth_rif_intf_out[END_POINT_2], cint_srv6_tunnel_info.tunnel_arp_id[END_POINT_2], out_port_end2, rv);
        return rv;
    }

    /*
     * On 1st Pass Egress, no FEC entry  because no more SIDs left to enter FWD on and then FEC,
     * in VTT5 there's LIF P2P which gives the destination right away
     */


    /** Egress 2nd Pass FEC Entry - due to Forwarding on IPv4 Layer above SRv6  */
    int egress_encoded_fec1;
    BCM_GPORT_LOCAL_SET(gport, out_port_egress);
    rv = l3__egress_only_fec__create_inner(unit, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID], 0 , cint_srv6_tunnel_info.tunnel_arp_id[EGRESS], gport, 0, 0, 0,&egress_encoded_fec1);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], cint_srv6_tunnel_info.eth_rif_intf_out[INGRESS], cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }

    /** add End-Point-2's (SID1) as host to do forwarding on to FEC */
    rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[SID1], cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6 for End-Point-1: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[END_POINT_1], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID], rv);
        return rv;
    }

    /** add Egress (SID0) as host to do forwarding on to FEC */
    rv = add_host_ipv6(unit, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0], cint_srv6_tunnel_info.vrf_in[END_POINT_2], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv6 for End-Point-2: vrf = %d, fec = %d, err_id = %d\n", proc_name, cint_srv6_tunnel_info.vrf_in[END_POINT_2], cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID], rv);
        return rv;
    }

    /** No Egress 1-Pass host, because no more SIDs left to enter FWD on, in VTT5 there's LIF P2P which gives the destination right away */

    /** add Egress 2nd-Pass, IPv4 Packet DIP as host to do forwarding on to FEC */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID]);
    rv = add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[RCH], _l3_itf, cint_srv6_tunnel_info.eth_rif_intf_out[EGRESS], 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    /** Call SRv6 Ingress tunnel config - Done via diagnostic shell command until valid APIs */
    {
        /** defines SRv6 Encapsulation*/
        bshell(unit, "srv6 tunnelConfig NofSegments=3 Test_MoDe=1");
    }

    /** Configure the SRv6 Egress 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation */
    {
        bshell(unit, "srv6 TUNnelConFiGEgress Test_MoDe=1 PROTOcol=0x4 Core=0 Port=40");
    }

    /** End-Point configurations */
    {
        /** End-Points: Add MyDIP according to the SIDs, because we need to terminate the IPV6 header with the current SID */
        srv6_tunnel_termination(unit, SID2, cint_srv6_tunnel_info.vrf_in[END_POINT_1]);
        srv6_tunnel_termination(unit, SID1, cint_srv6_tunnel_info.vrf_in[END_POINT_2]);

        /** Egress: Add MyDIP according to the SIDs, because we need to terminate the IPV6 header with the current SID */
        srv6_tunnel_termination(unit, SID0, cint_srv6_tunnel_info.vrf_in[EGRESS]);

    }

    printf("%s(): Exit\r\n",proc_name);
    return rv;
}

