/*
 * $Id$
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *File: cint_dnx_gtp.c
 * Purpose: An example of GTPU-V1-GPDU encapsulation and termination
 *
 * 1. Example of GTP-U-V1-GPDU termination
 * 2. Example of GTP-U-V1-GPDU encapsulation
 *
 *
 * 1. Example of gtp termination
 *
 * Example for Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/gtp/cint_dnx_gtp.c
 *    cint
 *    call gtp_termination(0, 201, 203, 1, 1);
 *    exit;
 * Run traffic example from BCM shell:
 *    PacKeT TX port=200 PTCH_2 ETH1 IPv4 UDP UDP.Dst_Port=2152 GTP_V1_base GTP_V1_base.version=0x1 GTP_V1_base.PT=0x1 GTP_V1_base.E=0 GTP_V1_base.Message_Type=0xFF GTP_V1_base.S=0 show
 * Expected:
 *     Gtp is terminated
 *
 * 2. Example of GTP MP encapsulation
 *
 * Set up sequence:
 *    cint ../../../src/examples/sand/utility/cint_sand_utils_global.c
 *    cint ../../../src/examples/dnx/field/cint_field_utils.c
 *    cint ../../../src/examples/sand/cint_ip_route_basic.c
 *    cint ../../../src/examples/dnx/gtp/cint_dnx_gtp.c
 *    cint
 *    gtp_encapsulation(0, 201, 203, 1, 1);
 *    exit;
 * Run traffic example from BCM shell:
 *    PacKeT TX port=201 PTCH_2 ETH1 IPv4 show
 * Expected:
 *     IpvXoGtpoUDPoIpv4oETH is generated
 *
 * Note:
 * =====
 * For GTP termination, primary configuration for the entire device must be made (which is not part of this cint):
 * Enable GTP-U collapse UDP, by API bcm_switch_control_indexed_set as follow:
 *      key.index=bcmSwitchL3TunnelGtpU
 *      key.type=bcmSwitchL3TunnelCollapseDisable
 *      value.value=0
 *
 */

struct cint_gtp_basic_info_s
{
    int intf_in;                        /* in RIF */
    int intf_out;                       /* out RIF */
    bcm_mac_t intf_in_mac_address;      /* mac for in RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac;         /* mac for next hop */
    int fec_id;                         /* FEC id */
    int arp_id;                         /* Id for ARP entry */

    int rif_vrf;                        /* router instance VRF resolved at VTT1 */
    int gtp_vrf;                        /* VRF after termination, resolved as Tunn-InLif property */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;           /* tunnel SIP mask */
    bcm_ip6_t tunnel_dip6;              /* tunnel DIP */
    bcm_ip6_t tunnel_dip6_mask;         /* tunnel DIP mask */
    bcm_ip6_t tunnel_sip6;              /* tunnel SIP */
    bcm_ip6_t tunnel_sip6_mask;         /* tunnel SIP mask */

    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */
    bcm_gport_t ip_tunnel_id;           /* ip Tunnel id */
    int ip_tunnel_outlif;               /* OUTLIF to be used in ip tunnel encapsulation */

    uint32 gtp_network_domain;          /* gtp network domain */
    uint32 termination_teid;            /* gtp Tunnel end point for termination */
    uint32 encapsulation_teid;          /* gtp Tunnel end point for encapsulation */    
    int terminator_id;                  /* Gport for termination entry */
    int initiator_id;                   /* Gport for initirator entry */

    bcm_ip_t host_access;               /* ipv4 dip */
    bcm_ip_t host_provider;             /* ipv4 dip */

    bcm_ip6_t host6_access;             /* ipv6 dip  */
    bcm_ip6_t host6_provider;           /* ipv6 dip */
    int tunnel_ttl;                     /* ttl */
};

cint_gtp_basic_info_s cint_gtp_basic_info = {
    /** intf_in | intf_out */
    0x12, 0x65,
    /** intf_in_mac_address */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x33},
    /** intf_out_mac_address */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x44},
    /** arp_next_hop_mac */
    {0x00, 0x00, 0x00, 0x00, 0xcd, 0x3d},
    /** fec_id */
    0xCDDF,
    /** arp_id */
    0,
    /** rif_vrf, gtp_vrf */
    6, 7,
    /** tunnel_dip */
    0xAB000001, /* 171.0.0.1 */
    /** tunnel_dip_mask */
    0xffffffff,
    /** tunnel_sip */
    0xAC000001, /* 172.0.0.1 */
    /** tunnel_sip_mask */
    0xffffffff,
    /** tunnel_dip6 */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x10},
    /** tunnel_dip6_mask */
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    /** tunnel_sip6 */
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x11},
    /* * tunnel_sip6_mask */
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff},
    /** udp_dst_port, udp_src_port */
    2152, 2152,
    /** ip_tunnel_id */
    0,
    /** ip_tunnel_outlif */
    0x45346,
    /** gtp_network_domain */
    201,
    /** termination_teid */
    0xAAAABBBB,
    /** encapsulation_teid */
    0xBBBBAAAA,
    /** terminator_id */
    0,
    /** initiator_id */
    0,
    /** host_access */
    0x7fffff05,
    /** host_provider */
    0x8fffff05,
    /** host6_access */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13},
    /** host6_provider */
    {0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02},
    /** tunnel_ttl */
    64
};

/**
* \brief
*   Create IP tunnel
* \param [in] unit         - Device ID
* \param [in] arp_itf      - tunnel ARP interface Id
* \param [out] tunnel_intf - tunnel interface
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int
ip_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t * tunnel_intf,
    int is_gtp_over_ipv4)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;

    /** Create IP tunnel initiator for encapsulating IPv4oETH1 tunnel header */
    bcm_tunnel_initiator_t_init(&tunnel_info);
    if (is_gtp_over_ipv4)
    {
        tunnel_info.dip = cint_gtp_basic_info.tunnel_dip;
        tunnel_info.sip = cint_gtp_basic_info.tunnel_sip;
    }
    else
    {
        sal_memcpy(tunnel_info.dip6, cint_gtp_basic_info.tunnel_dip6, sizeof(tunnel_info.dip6));
        sal_memcpy(tunnel_info.sip6, cint_gtp_basic_info.tunnel_sip6, sizeof(tunnel_info.sip6));
    }

    tunnel_info.flags = 0;
    tunnel_info.type = is_gtp_over_ipv4 == TRUE ? bcmTunnelTypeUdp : bcmTunnelTypeUdp6;
    tunnel_info.udp_dst_port = cint_gtp_basic_info.udp_dst_port;
    tunnel_info.udp_src_port = cint_gtp_basic_info.udp_src_port;
    tunnel_info.l3_intf_id = arp_itf;
    tunnel_info.encap_access = is_gtp_over_ipv4 ? bcmEncapAccessTunnel4 : bcmEncapAccessTunnel3;
    tunnel_info.ttl = cint_gtp_basic_info.tunnel_ttl;
    if (*tunnel_intf > 0)
    {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_info.tunnel_id, *tunnel_intf);
        tunnel_info.flags |= BCM_TUNNEL_WITH_ID;
    }

    tunnel_info.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }

    *tunnel_intf = l3_intf.l3a_intf_id;

    return rv;
}

/**
* \brief
*   Function that creates the general-case tunnel terminator.
* \param [in] unit         - Device ID
* \param [out ] tunnel_inlif_id - tunnel interface Id
* \param [out] tunnel_id - tunnel id gport
* \return
*   int - Error Type
* \remark
*   None
* \see
*   None
*/
int
ip_tunnel_terminator_create(
    int unit,
    bcm_if_t *tunnel_inlif_id,
    bcm_gport_t *tunnel_id)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_gtp_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_gtp_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_gtp_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_gtp_basic_info.tunnel_sip_mask;
    tunnel_term.udp_dst_port = cint_gtp_basic_info.udp_dst_port;
    tunnel_term.vrf = cint_gtp_basic_info.rif_vrf;
    tunnel_term.type = bcmTunnelTypeUdp;
    
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_terminator_create \n");
        return rv;
    }

    /** update next network_domain of ip tunnel lif. */
    rv = bcm_port_class_set(unit, tunnel_term.tunnel_id, bcmPortClassIngress, cint_gtp_basic_info.gtp_network_domain);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    /** Convert tunnel's GPort ID to intf ID */
    bcm_if_t intf_id;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(intf_id, tunnel_term.tunnel_id);    
    *tunnel_inlif_id = intf_id;

    *tunnel_id = tunnel_term.tunnel_id;
    return rv;
}

/**
* \brief
*   gtp tunnel termination create
* \param [in] unit              - Device ID
* \param [in] is_mp             - true for multiPoint, false for pointToPoint
* \param [in/out] terminator_id - tunnel id gport
* \return
*   int - Error Type
* \remark
*   None
* \see
*   None
*/
int
gtp_tunnel_terminator_create(
    int unit, 
    int is_mp,
    int * terminator_id)
{    
    int rv = BCM_E_NONE;   
    bcm_flow_handle_t flow_handle;
    bcm_flow_terminator_info_t info;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "GTP_TERMINATION",&flow_handle));
    
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = 0;
    info.valid_elements_set |=
            BCM_FLOW_TERMINATOR_ELEMENT_FLOW_SERVICE_TYPE_VALID | BCM_FLOW_TERMINATOR_ELEMENT_L3_INGRESS_INFO_VALID;
    bcm_flow_l3_ingress_info_t_init(&info.l3_ingress_info);
    if(is_mp)
    {
        info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_VRF_VALID;
        info.vrf = cint_gtp_basic_info.gtp_vrf;
        info.service_type = bcmFlowServiceTypeMultiPoint;
    }
    else
    {
        bcm_gport_t fwd_gport;
        BCM_GPORT_FORWARD_PORT_SET(fwd_gport, cint_gtp_basic_info.fec_id);
        info.valid_elements_set |= BCM_FLOW_TERMINATOR_ELEMENT_FLOW_DEST_INFO_VALID;
        info.dest_info.dest_port = fwd_gport;
        info.dest_info.dest_encap = 0;
        info.service_type = bcmFlowServiceTypeCrossConnect; 
    }
    BCM_IF_ERROR_RETURN(bcm_flow_terminator_info_create(unit, &flow_handle_info, &info, NULL));
    *terminator_id = flow_handle_info.flow_id;
    
    /** point the LIF from the ISEM table */
    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "GTP_CLASSIFICATION", &flow_handle));
    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flow_id = *terminator_id;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
            unit, flow_handle, "TUNNEL_ENDPOINT_IDENTIFIER", &special_fields.special_fields[0].field_id));
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(
            unit, flow_handle, "NEXT_LAYER_NETWORK_DOMAIN", &special_fields.special_fields[1].field_id));
    special_fields.actual_nof_special_fields = 2;
    special_fields.special_fields[0].is_clear = 0;
    special_fields.special_fields[0].shr_var_uint32 = cint_gtp_basic_info.termination_teid;
    special_fields.special_fields[1].is_clear = 0;
    special_fields.special_fields[1].shr_var_uint32 = cint_gtp_basic_info.gtp_network_domain;
    BCM_IF_ERROR_RETURN(bcm_flow_match_info_add(unit, &flow_handle_info, &special_fields));

    printf ("gtp_tunnel_terminator_create pass. terminator_id=%x \n", flow_handle_info.flow_id);
    
    return rv;
}

/**
* \brief
*   gtp tunnel initiator create
* \param [in] unit         - Device ID
* \param [in] is_gtp_over_ipv4 - is gtp over ipv4
* \param [in/out] initiator_id - tunnel id gport
* \return
*   int - Error Type
* \remark
*   None
* \see
*   None
*/
int
gtp_tunnel_initiator_create(
    int unit,
    int is_gtp_over_ipv4,
    int * initiator_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_initiator_info_t info;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_special_fields_t special_fields;
    bcm_flow_field_id_t field_id;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "GTP_INITIATOR",&flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    info.flags = 0;
    info.valid_elements_set =
        (BCM_FLOW_INITIATOR_ELEMENT_ENCAP_ACCESS_VALID | BCM_FLOW_INITIATOR_ELEMENT_L3_INTF_ID_VALID );
    info.l3_intf_id = cint_gtp_basic_info.ip_tunnel_outlif;
    info.encap_access = bcmEncapAccessTunnel2;

    special_fields.actual_nof_special_fields = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "TUNNEL_ENDPOINT_IDENTIFIER", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].shr_var_uint32 = cint_gtp_basic_info.encapsulation_teid;

    /** special fields configuration */
    BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, &info, &special_fields));
    *initiator_id = flow_handle_info.flow_id;

    printf ("bcm_flow_initiator_info_create pass. initiator_id=%d \n", flow_handle_info.flow_id);

    return rv;
}

/**
* \brief
* GTP_V1-GPDU Termination
* +------------+
* |   data     |
* +------------+
* | native  ip |
* +------------+
* |GTP_V1-GPDU |
* +------------+            +-----------+
* |    udp     |            |   data    |
* +------------+            +-----------+
* | public ip  +----------->+ native ip |
* +------------+            +-----------+
* |    eth     |            |   eth     |
* +------------+            +-----------+
* \param [in] unit              - Device ID
* \param [in] provider_port     - in port
* \param [in] access_port       - out port
* \param [in] is_mp             - true for multiPoint, false for pointToPoint
* \param [in] is_ipv4_fwd       - true if the forwarding is IPv4, false for IPv6
* \return
*   int - Error Type
* \remark
*   None
* \see
*   None
*/
int
gtp_termination(
    int unit,
    int provider_port,
    int access_port,
    int is_mp,
    int is_ipv4_fwd)
{
    int rv = BCM_E_NONE;
    char *proc_name = "gtp_termination";    
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t tunnel_inlif_id;

    /** Set provider_port to In ETh-RIF */
    rv = in_port_intf_set(unit, provider_port, cint_gtp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /** Set access_port default properties, in case of ARP+AC no need */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_intf_set intf_out\n", proc_name);
        return rv;
    }

    /** Create ETH-RIF and set its properties */
    rv = intf_eth_rif_create(unit, cint_gtp_basic_info.intf_in, cint_gtp_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_gtp_basic_info.intf_out, cint_gtp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }
    /** Set Incoming ETH-RIF properties */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_gtp_basic_info.rif_vrf;
    ingress_rif.intf_id = cint_gtp_basic_info.intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** Create ip tunnel termination */
    rv = ip_tunnel_terminator_create(unit, &tunnel_inlif_id, &cint_gtp_basic_info.ip_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create ip tunnel termination\n",proc_name);
        return rv;
    }

    /** Create ARP and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &cint_gtp_basic_info.arp_id,
                cint_gtp_basic_info.arp_next_hop_mac, cint_gtp_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }
    
    /** Create FEC and set its properties */
    BCM_GPORT_LOCAL_SET(gport, access_port);
    rv = l3__egress_only_fec__create
        (unit, cint_gtp_basic_info.fec_id, cint_gtp_basic_info.intf_out, cint_gtp_basic_info.arp_id, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }

    /** Create gtp tunnel termination */
    rv = gtp_tunnel_terminator_create(unit, is_mp, &cint_gtp_basic_info.terminator_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, gtp_tunnel_terminator_create\n",proc_name);
        return rv;
    }    

    if(is_mp)
    {
        /** Add host & route entries */
        if (is_ipv4_fwd)
        {
            rv = add_host_ipv4(unit, cint_gtp_basic_info.host_access, cint_gtp_basic_info.gtp_vrf, cint_gtp_basic_info.fec_id, 0, 0);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
                return rv;
            }
            rv = add_route_ipv4(unit, cint_gtp_basic_info.host_access, 0xfffffff0, cint_gtp_basic_info.gtp_vrf, cint_gtp_basic_info.fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_route_ipv4(), \n",proc_name);
                return rv;
            }
        }
        else
        {
            rv = add_host_ipv6(unit, cint_gtp_basic_info.host6_access, cint_gtp_basic_info.gtp_vrf, cint_gtp_basic_info.fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_host_ipv6(), \n",proc_name);
                return rv;
            }
            bcm_ip6_t ipv6_mask={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00};
            rv = add_route_ipv6(unit, cint_gtp_basic_info.host6_access, ipv6_mask, cint_gtp_basic_info.gtp_vrf, cint_gtp_basic_info.fec_id);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in function add_host_ipv6(), \n",proc_name);
                return rv;
            }
        }
    }

    return rv;
}

/**
* \brief
* GTP_V1-GPDU encapsulation
*                            +------------+
*                            |   data     |
*                            +------------+
*                            | native  ip |
*                            +------------+
*                            | GTPV1 GPDU |
* +------------+             +------------+
* |   data     |             |    udp     |
* +------------+             +------------+
* | native  ip +-----------> | public ip  |
* +------------+             +------------+
* |   eth      |             |    eth     |
* +------------+             +------------+
*
* \param [in] unit              - Device ID
* \param [in] access_port       - in port
* \param [in] provider_port     - out port
* \param [in] is_ipv4_fwd       - true if the forwarding is IPv4, false for IPv6
* \param [in] is_gtp_over_ipv4  - true for doing IPv4 tunnel encapsulation, false for IPv6.
* \return
*   int - Error Type
* \remark
*   None
* \see
*   None
*/
int
gtp_encapsulation(
    int unit,
    int access_port,
    int provider_port,
    int is_fwd_ipv4,
    int is_gtp_over_ipv4)

{
    int rv = BCM_E_NONE;
    char *proc_name = "gtp_encapsulation";
    l3_ingress_intf ingress_rif;
    bcm_gport_t gport;
    bcm_if_t l3_intf_id = 0;

    /** Set In-Port to In ETh-RIF */
    rv = in_port_intf_set(unit, access_port, cint_gtp_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in_port_intf_set intf_in\n", proc_name);
        return rv;
    }

    /** Set out_port default properties */
    rv = out_port_set(unit, provider_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set \n", proc_name);
        return rv;
    }

    /*
     * Create routing interface for the routing's IP.
     * We use it as ingress ETH-RIF to perform ETH termination (my-mac procedure), to enable IP routing for this ETH-RIF and to set the VRF.
     */
    rv = intf_eth_rif_create(unit, cint_gtp_basic_info.intf_in, cint_gtp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in\n", proc_name);
        return rv;
    }

    /*
     * Create egress routing interface used for routing after the tunnel encapsulation.
     * We are using it as egress ETH-RIF, providing the link layer SA.
     */
    rv = intf_eth_rif_create(unit, cint_gtp_basic_info.intf_out, cint_gtp_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n", proc_name);
        return rv;
    }

    /** Set Incoming ETH-RIF properties, VRF is updated for the rif */
    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = cint_gtp_basic_info.rif_vrf;
    ingress_rif.intf_id = cint_gtp_basic_info.intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /** Create ARP entry and set its properties */
    rv = l3__egress_only_encap__create(unit, 0, &cint_gtp_basic_info.arp_id,
                cint_gtp_basic_info.arp_next_hop_mac, cint_gtp_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /** Create UDP IP tunnel, and set next-lif-pointer to the ARP entry */
    rv = ip_tunnel_initiator_create
            (unit, cint_gtp_basic_info.arp_id, &cint_gtp_basic_info.ip_tunnel_outlif, is_gtp_over_ipv4);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create gtp ip tunnel\n",proc_name);
        return rv;
    }

    /** Create GTP encapsulation */
    rv = gtp_tunnel_initiator_create(unit, is_gtp_over_ipv4, &cint_gtp_basic_info.initiator_id);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, gtp_tunnel_initiator_create\n", proc_name);
        return rv;
    }

    /** Create FEC and set its properties */
    BCM_GPORT_LOCAL_SET(gport, provider_port);
    /** Convert tunnel's GPort ID to intf ID */
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, cint_gtp_basic_info.initiator_id);

    rv = l3__egress_only_fec__create(unit, cint_gtp_basic_info.fec_id, l3_intf_id, 0, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n", proc_name);
        return rv;
    }

    /** Add host entries */
    int fec_id;
    BCM_L3_ITF_SET(&fec_id, BCM_L3_ITF_TYPE_FEC, cint_gtp_basic_info.fec_id);
    if (is_fwd_ipv4)
    {
        rv = add_host_ipv4(unit, cint_gtp_basic_info.host_provider, cint_gtp_basic_info.rif_vrf, fec_id, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n", proc_name);
            return rv;
        }
    }
    else
    {
        rv = add_host_ipv6(unit, cint_gtp_basic_info.host6_provider, cint_gtp_basic_info.rif_vrf, cint_gtp_basic_info.fec_id);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv6(), \n",proc_name);
            return rv;
        }
    }
    return rv;
}

/* Replace part of the LIF common properties*/
int
gtp_tunnnel_terminator_replace(
    int unit,
    int global_lif,
    bcm_flow_terminator_info_t info)
{
    int rv = BCM_E_NONE;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "GTP_TERMINATION",&flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
    flow_handle_info.flow_id = global_lif;

    BCM_IF_ERROR_RETURN(bcm_flow_terminator_info_create(unit, &flow_handle_info, info, NULL));

    return rv;
}

/* Replace part of the LIF common properties*/
int
gtp_tunnnel_initiator_replace(
    int unit,
    int global_lif,
    bcm_flow_initiator_info_t info)
{
    int rv = BCM_E_NONE;
    bcm_flow_handle_t flow_handle;
    bcm_flow_handle_info_t flow_handle_info;
    bcm_flow_field_id_t field_id;
    bcm_flow_special_fields_t special_fields;

    BCM_IF_ERROR_RETURN(bcm_flow_handle_get(unit, "GTP_INITIATOR",&flow_handle));

    flow_handle_info.flow_handle = flow_handle;
    flow_handle_info.flags = BCM_FLOW_HANDLE_INFO_REPLACE;
    flow_handle_info.flow_id = global_lif;

    /* Keep TUNNEL_ENDPOINT_IDENTIFIER original value from the previous flow create */
    special_fields.actual_nof_special_fields = 1;
    BCM_IF_ERROR_RETURN(bcm_flow_logical_field_id_get(unit, flow_handle, "TUNNEL_ENDPOINT_IDENTIFIER", &field_id));
    special_fields.special_fields[0].field_id = field_id;
    special_fields.special_fields[0].is_clear = 1;

    BCM_IF_ERROR_RETURN(bcm_flow_initiator_info_create(unit, &flow_handle_info, info, &special_fields));

    return rv;
}
