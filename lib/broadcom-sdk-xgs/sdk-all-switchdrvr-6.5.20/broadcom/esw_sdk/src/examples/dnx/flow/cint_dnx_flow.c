/*
 * $Id$
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 *File: cint_flow_gtp.c
 * Purpose: Flow lif wrappers and examples
 */

struct cint_flow_ipv4_tunnel_term_basic_info_s
{
	int arp_id;                         /* Id for ARP entry */
	int rif_vrf;                        /* router instance VRF resolved at VTT1 */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;           /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;           /* tunnel SIP mask */

    uint32 gtp_network_domain;          /* gtp network domain */
    uint32 termination_teid;            /* gtp Tunnel end point for termination */
    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */

    int tunnel_ttl;                     /* ttl */
};

struct cint_flow_ipv4_tunnel_init_basic_info_s
{
	int arp_id;                         /* Id for ARP entry */
    bcm_ip_t tunnel_dip;                /* tunnel DIP */
    bcm_ip_t tunnel_sip;                /* tunnel SIP */
    uint16 udp_dst_port;                /* Destination UDP port */
    uint16 udp_src_port;                /* Source UDP port */
    int tunnel_ttl;                     /* ttl */
    int ipv4_tunnel;               	    /* Outlif to be used in ip tunnel encapsulation */
    uint32 encapsulation_teid;          /* gtp Tunnel end point for encapsulation */
};


cint_flow_ipv4_tunnel_term_basic_info_s cint_flow_ipv4_tunnel_term_basic_info = {
    /** arp_id */
    0,
    /** rif_vrf */
    6,
    /** tunnel_dip */
    0xAB000001, /* 171.0.0.1 */
    /** tunnel_dip_mask */
    0xffffffff,
    /** tunnel_sip */
    0xAC000001, /* 172.0.0.1 */
    /** tunnel_sip_mask */
    0xffffffff,
    /** gtp_network_domain */
    201,
    /** termination_teid */
    1,
    /** udp_dst_port, udp_src_port */
    2152, 2152,
    /** tunnel_ttl */
    64
};

cint_flow_ipv4_tunnel_init_basic_info_s cint_flow_ipv4_tunnel_init_basic_info = {
    /** arp_id */
    0,
    /** tunnel_dip */
    0xAB000001, /* 171.0.0.1 */
    /** tunnel_sip */
    0xAC000001, /* 172.0.0.1 */
    /** udp_dst_port, udp_src_port */
    2152, 2152,
    /** tunnel_ttl */
    64,
    /** ipv4_tunnel */
	0x45346,
    /** encapsulation_teid */
    0xBBBBAAAA
};

/* Create ipv4 ingress tunnel */
int
ipv4_tunnel_term_create(
    int unit)
{
    int rv;
    bcm_tunnel_terminator_t tunnel_term;

    /** Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    tunnel_term.dip = cint_flow_ipv4_tunnel_term_basic_info.tunnel_dip;
    tunnel_term.dip_mask = cint_flow_ipv4_tunnel_term_basic_info.tunnel_dip_mask;
    tunnel_term.sip = cint_flow_ipv4_tunnel_term_basic_info.tunnel_sip;
    tunnel_term.sip_mask = cint_flow_ipv4_tunnel_term_basic_info.tunnel_sip_mask;
    tunnel_term.udp_dst_port = cint_flow_ipv4_tunnel_term_basic_info.udp_dst_port;
    tunnel_term.vrf = cint_flow_ipv4_tunnel_term_basic_info.rif_vrf;
    tunnel_term.type = bcmTunnelTypeUdp;

    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_tunnel_terminator_create failed \n");
        return rv;
    }

    /** update next network_domain of ip tunnel lif. */
    rv = bcm_port_class_set(unit, tunnel_term.tunnel_id, bcmPortClassIngress, cint_flow_ipv4_tunnel_term_basic_info.gtp_network_domain);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_port_class_set failed\n");
        return rv;
    }

    return rv;
}

/* Create ipv4 egress tunnel */
int
ipv4_tunnel_init_create(
    int unit)
{
    int rv;
    bcm_tunnel_initiator_t tunnel_info;
    bcm_l3_intf_t l3_intf;

    /** Create IP tunnel initiator for encapsulating IPv4oETH1 tunnel header */
    bcm_tunnel_initiator_t_init(&tunnel_info);

    tunnel_info.dip = cint_flow_ipv4_tunnel_init_basic_info.tunnel_dip;
    tunnel_info.sip = cint_flow_ipv4_tunnel_init_basic_info.tunnel_sip;

    tunnel_info.flags = 0;
    tunnel_info.type = bcmTunnelTypeUdp;
    tunnel_info.udp_dst_port = cint_flow_ipv4_tunnel_init_basic_info.udp_dst_port;
    tunnel_info.udp_src_port = cint_flow_ipv4_tunnel_init_basic_info.udp_src_port;
    tunnel_info.l3_intf_id = cint_flow_ipv4_tunnel_init_basic_info.arp_id;
    tunnel_info.encap_access = bcmEncapAccessTunnel3;
    tunnel_info.ttl = cint_flow_ipv4_tunnel_init_basic_info.tunnel_ttl;

    BCM_GPORT_TUNNEL_ID_SET(tunnel_info.tunnel_id, cint_flow_ipv4_tunnel_init_basic_info.ipv4_tunnel);
    tunnel_info.flags |= BCM_TUNNEL_WITH_ID;

    tunnel_info.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;

    bcm_l3_intf_t_init(&l3_intf);
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_tunnel_initiator_create \n");
    }
    cint_flow_ipv4_tunnel_init_basic_info.ipv4_tunnel = l3_intf.l3a_intf_id;

    return rv;
}

/* Callback for removing all flow match gtp entries */
int flow_match_delete_all_callback(int unit, bcm_flow_handle_info_t *flow_handle_info, bcm_flow_special_fields_t *key_special_fields, void *user_data)
{
    int rv;

    rv = bcm_flow_match_info_delete(unit, flow_handle_info, key_special_fields);
    if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND))
    {
    	printf("bcm_flow_match_info_delete failed\n");
    }
    return rv;
}

/* Traverse over gtp match entries using flow lif */
int flow_gtp_match_traverse(
    int unit)
{
    int rv;

    bcm_flow_handle_info_t flow_handle_info;
    rv = bcm_flow_match_info_traverse(unit, &flow_handle_info, flow_match_delete_all_callback, "DELETE");
    if (rv == BCM_E_NOT_FOUND)
    {
        rv = BCM_E_NONE;
    }
    else if (rv != BCM_E_NONE)
    {
    	printf("bcm_flow_match_info_traverse failed\n");
    }
    return rv;
}
