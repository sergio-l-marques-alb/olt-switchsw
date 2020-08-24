/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * $Id: $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
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
    SID3,
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
    int tunnel_vid;                                     /* VID */
    bcm_ip_t route_ipv4_dip;                            /* IPv4 Route DIP */
    bcm_ip6_t tunnel_ip6_dip[NOF_SIDS];                 /* IPv6 Terminated DIPs */
    bcm_ip6_t tunnel_ip6_sip;                           /* IPv6 Tunnel SIP */
    int recycle_port;                                   /* ESR USP RCH Port */
    int srh_next_protocol;                              /* Next protocol above SRv6, per IANA RFC */
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
         * tunnel_fec_id (update on run time)
         */
         {0, 0, 0, 0, 0},
         /*
          * tunnel_global_lifs - SRH Base, 1st SID respectively
          */
         {0x1230, 0x1231},
         /*
          * tunnel_vid
          */
         100,
        /*
         * route_ipv4_dip
         */
        0x7fffff02 /* 127.255.255.02 */,
        /*
         * terminated_ip6_dips
         */
        {{ 0x12, 0x34, 0x56, 0x78, 0x87, 0x65, 0x43, 0x21, 0xa5, 0xa5, 0xa5, 0xa5, 0x5a, 0x5a, 0x5a, 0x5a },
         { 0x11, 0x11, 0x22, 0x22, 0x33, 0x33, 0x44, 0x44, 0x55, 0x55, 0x66, 0x66, 0x77, 0x77, 0x88, 0x88 },
         { 0xaa, 0xaa, 0xbb, 0xbb, 0xcc, 0xcc, 0xdd, 0xdd, 0xee, 0xee, 0xff, 0xff, 0x11, 0x11, 0x22, 0x22 },
         { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc }},
         /*
          * tunnel_ip6_sip
          */
        { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xee, 0xff, 0xff, 0xee, 0xcb, 0xa9, 0x87, 0x65, 0x43, 0x21 },
        /*
         * esr usp rch port
         */
        40,
       /*
        * next protocol above SRv6 which is IPv4
        */
        4
};

int
srv6_endpoint_tunnel(int unit)
{
    bcm_ip6_t ip6_mask = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    bcm_ip6_t ip6_dip = {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0xFF,0xFF,0xFF,0x13};
    bcm_ip6_t ip6_sip = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xAA};
    bcm_tunnel_terminator_t tunnel_term_set;
    l3_ingress_intf ingress_rif;
    int rv;

    bcm_tunnel_terminator_t_init(&tunnel_term_set);
    tunnel_term_set.type = bcmTunnelTypeSR6;
    sal_memcpy(tunnel_term_set.dip6, ip6_dip, 16);
    sal_memcpy(tunnel_term_set.sip6, ip6_sip, 16);
    sal_memcpy(tunnel_term_set.dip6_mask, ip6_mask, 16);
    sal_memcpy(tunnel_term_set.sip6_mask, ip6_mask, 16);
    tunnel_term_set.vrf = 1;
    tunnel_term_set.ingress_qos_model.ingress_ttl = bcmQosIngressModelPipe;
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term_set);
    if(rv != BCM_E_NONE)
    {
        printf("Error bcm_tunnel_terminator_create. rv = %d \n", rv);
        return rv;
    }

    l3_ingress_intf_init(&ingress_rif);
    ingress_rif.vrf = 1;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(ingress_rif.intf_id, tunnel_term_set.tunnel_id);
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error intf_ingress_rif_set. rv = %d \n", rv);
        return rv;
    }

    return BCM_E_NONE;
}
/*
 *    Update the FECs IDs
 */
int
srv6_update_fecs_values(int unit)
{

    int rv;
    int first_fec_in_hier;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID] = first_fec_in_hier + 0x700;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_1_FEC_ID] = first_fec_in_hier + 0x1;
    cint_srv6_tunnel_info.tunnel_fec_id[END_POINT_2_FEC_ID] = first_fec_in_hier + 0x2;
    cint_srv6_tunnel_info.tunnel_fec_id[EGRESS_FEC_ID] = first_fec_in_hier + 0x3;
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, &first_fec_in_hier);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_FIRST_SID_FEC_ID] = first_fec_in_hier + 0x700;

    return rv;
}

/*
 *    Configuring ISR (Ingress SRv6 Tunnel)
 *    This will create encapsulation of SRH,SIDs and IPv4 onto the packet
 *
 *    Specifically:
 *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
 *    - creation of x1 local-out-LIFs to return following LL entries from EEDB:
 *       1. SRH Base -> SID0 -> SID1 -> SID2 -> IPv6 Tunnel (LL)
 */
int
srv6_ingress_tunnel_config(
                            int unit,
                            int in_port,             /** Incoming port of SRV6 Ingress Tunnel */
                            int out_port,            /** Outgoing port of SRV6 Ingress Tunnel */
                            int nof_sids)
{

    int rv;
    int first_fec_in_hier;
    char *proc_name = "srv6_ingress_tunnel_config";

    /*
     * Update FEC IDS
     */
    rv = srv6_update_fecs_values(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, srv6_get_fec_values\n");
        return rv;
    }


    if (nof_sids < 2 || nof_sids > 4)
    {
        printf("%s(): Error, cannot use nof_sids (%d), it has to be 2 to 4 !\n",proc_name, nof_sids);
        return BCM_E_PARAM;
    }


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
    rv = intf_eth_rif_create(unit, cint_srv6_tunnel_info.tunnel_vid, cint_srv6_tunnel_info.intf_out_mac_address[INGRESS]);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
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


    /*
     * 5. Create ARP and set its properties - ARP is Assumed be + AC. Create VLAN ID editing for each case.
     *    We use same port out for the End-Points, therefore the VLAN ID is what separates these cases.
     */

    uint32 flags2 = 0;

    flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;

    /** SRV6 Ingress Tunnel ARP create, with VLAN ID of expected in End-Point-1*/
    rv = l3__egress_only_encap__create_inner(unit, 0, &cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], cint_srv6_tunnel_info.arp_next_hop_mac[INGRESS], cint_srv6_tunnel_info.tunnel_vid, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only, in SRV6 Ingress Tunnel\n",proc_name);
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


    /*
     * 7. Add route entry - we work with specific DIP addresses, so we don't need to add best match route entries.
     */

    /*
     * 8. Add host entry
     */

    /** SRV6 Tunnel Ingress Forwarding to FEC Hierarchy to bring out IPV6, SRH, SIDs*/
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_srv6_tunnel_info.tunnel_fec_id[INGRESS_SRH_FEC_ID]);
    add_host_ipv4(unit, cint_srv6_tunnel_info.route_ipv4_dip, cint_srv6_tunnel_info.vrf_in[INGRESS], 0, cint_srv6_tunnel_info.tunnel_arp_id[INGRESS], gport);

    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4() for SRV6 Ingress Tunnel\n",proc_name);
        return rv;
    }


    /*
     * 9. Configuring ISR SRv6 Tunnel
     *
     *    - creating EEDB entries for SRH Base, SIDs[0..2], IPv6 Tunnel (SRv6 Type)
     *    - creation of x2 local-out-LIFs to return following entries from EEDB:
     *       1. SRH Base
     *       2. SID0 -> SID1 -> SID2 -> IPv6 Tunnel (LL)
     */


    /*
     * create IPv6 Tunnel of SRv6 Type
     */
    bcm_tunnel_initiator_t tunnel_init_set;
    bcm_l3_intf_t intf;


    bcm_tunnel_initiator_t_init(&tunnel_init_set);
    bcm_l3_intf_t_init(&intf);
    tunnel_init_set.type = bcmTunnelTypeSR6;
    /** DIP is not required for this IPv6 Tunnel, which serves the SRv6 layer*/
    sal_memcpy(tunnel_init_set.sip6, cint_srv6_tunnel_info.tunnel_ip6_sip, 16);

    tunnel_init_set.ttl = 128;
    tunnel_init_set.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;

    rv = bcm_tunnel_initiator_create(unit, &intf, &tunnel_init_set);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_tunnel_initiator_create for IPv6 Tunnel \n");
       return rv;
    }

    /*
     * define the x3 SIDs  EEDB entries (linked to one another) and then link to an IPv6 EEDB entry
     */

    /** define the SID structure which holds info on the SID address, EEDB entry order, next EEDB pointer */
    bcm_srv6_sid_initiator_info_t  sid_info;

    /** to store next SID GPORT and init for SID0 to IPv6 Tunnel */
    bcm_gport_t next_sid_tunnel_id = tunnel_init_set.tunnel_id;

    /*
     * Create SID0 EEDB entry and link to IPv6 Tunnel EEDB entry
     */

    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_sids; sid_idx++)
    {

        /** store the previous SID GPORT for SID > 0 (cause 0 is linked to IPv6) */
        if (sid_idx != 0)
        {
            next_sid_tunnel_id = sid_info.tunnel_id;
        }

        /** if this is the final SID to add, this SID is the first SID in list and has the global-LIF to the SID-LL*/
        if (sid_idx == nof_sids - 1)
        {
            /** we will pass the global-out-LIF id for 1st SID */
            sid_info.flags = BCM_SRV6_SID_INITIATOR_WITH_ID;

            /** convert 1st SID global-LIF id to GPORT */
            BCM_GPORT_TUNNEL_ID_SET(sid_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_FIRST_SID_GLOBAL_LIF]);
        }
        else
        {
            /** don't use any special flags */
            sid_info.flags = 0;

            /** must set the tunnel id to 0 if not using WITH_ID flag */
            sid_info.tunnel_id = 0;
        }


        /** set SID address to be last SID0 */
        sal_memcpy(sid_info.sid, cint_srv6_tunnel_info.tunnel_ip6_dip[SID0 - sid_idx], 16);

        /** set to EEDB entry of SID0 */
        sid_info.encap_access = bcmEncapAccessTunnel4 - sid_idx;

        /** next_encap_id set to IPv6 interface - convert it from GPORT to l3_int */
        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(sid_info.next_encap_id, next_sid_tunnel_id);

        /** call the sid_initiator API to create the SID EEDB Entry and return its GPORT Tunnel-id */
        rv = bcm_srv6_sid_initiator_create(unit, &sid_info);

        if (rv != BCM_E_NONE)
        {
           printf("Error, bcm_dnx_srv6_sid_initiator_create for SID%d \n", sid_idx);
           return rv;
        }

    }


    /*
     * define the SRH Base EEDB entry
     */

    /** define the SRH Base structure which holds nof_sids, QOS, and SRH Base GPORT */
    bcm_srv6_srh_base_initiator_info_t srh_base_info;

    /** we will pass the global-out-LIF id*/
    srh_base_info.flags = BCM_SRV6_SRH_BASE_INITIATOR_WITH_ID;

    /** convert SRH global-LIF id to GPORT */
    BCM_GPORT_TUNNEL_ID_SET(srh_base_info.tunnel_id, cint_srv6_tunnel_info.tunnel_global_lif[INGRESS_SRH_GLOBAL_LIF]);

    /** set number of SIDs*/
    srh_base_info.nof_sids = nof_sids;


    /** set TTL and QOS modes */
    srh_base_info.egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    srh_base_info.egress_qos_model.egress_ecn = bcmQosEgressEcnModelInvalid;
    srh_base_info.ttl = 0x0; /** needs to be 0 cause we don't use Pipe mode */

    /** call the srh_base_initiator API to create the SRH EEDB Entry and local-out-LIF for it */
    rv = bcm_srv6_srh_base_initiator_create(unit, &srh_base_info);

    if (rv != BCM_E_NONE)
    {
       printf("Error, bcm_dnx_srv6_srh_base_initiator_create \n");
       return rv;
    }



    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


/*
 * Returns type field in ETH from SRH next type
 */
int
srh_nxt_hdr_to_eth_type(
    int srh_next_header_idx)
{
    int map_array[256] = { 0 };

    /** IPV4 */
    map_array[4] = 0x0800;

    /** IPV6 */
    map_array[41] = 0x86DD;

    /** ETH */
    map_array[59] = 0x1234;

    return map_array[srh_next_header_idx];
}


/*
 * Create a Recycle Entry.
 * unit - relevant unit
 * eth_type - ether type, per what's layer is passed for RCH ETH dummy packet encapsulation
 * recycle_entry_encap_id - returned encap id.
 */
int
create_recycle_entry(
    int unit,
    uint32 eth_type,
    int *recycle_entry_encap_id)
{
    int rv = BCM_E_NONE;

    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.ethertype = eth_type;
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    *recycle_entry_encap_id = recycle_entry.encap_id;

    return rv;
}




/*
 * packet will be routed from in_port to out-port
 *
 * Route:
 * packet to send:
 *  - in port = in_port
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected:
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 *
 */

/*
 * protection info options:
 * failover_id - FEC protection pointer (0 means disable protection)
 * failover_out_port - primary protection port (relevant when failover_id != 0)
 *
 * If 'do_raw' is non-zero then 'fec' is used as is (raw) within 'add_route_ipv4()'
 */
/*
 * example:
 int unit = 0;
 int in_port = 200;
 int out_port = 201;
 int in_qos_map_id = 7;
 */
int
dnx_basic_example_ipvx_for_srv6_inner(
    int unit,
    int srh_next_protocol,
    int rch_port,
    int out_port,
    int fec)
{
    int diff_const = 10;        /* this constant is to add to below values to enable IPV4 example run with IPV6 in SRV6 example */
    int rv;
    int intf_in = 15 + diff_const;           /* Incoming packet ETH-RIF */
    int intf_out = 100 + diff_const;         /* Outgoing packet ETH-RIF */
    int vrf = 1;                             /* RCH VRF 2nd Pass is equal here to 1st Pass VRF */
    int vsi = 2;                             /* VSI that's used with L2 o SRv6 case for 2nd Pass MACT Lookup */
    int encap_id = 8193 + diff_const;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
    bcm_mac_t intf_in_mac_address = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0x11 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    uint32 host = 0x7fffff02;
    l3_ingress_intf ingress_rif;
    l3_ingress_intf_init(&ingress_rif);
    uint32 flags2 = 0;
    int vlan = 100 + diff_const;
    /*
     * used to pass fec in host.l3a_intf
     */
    int encoded_fec;

    char *err_proc_name;
    char *proc_name;

    /*
     * Bridge configuration for L2 o SRv6 case
     */
    bcm_mac_t mac_table_fwd_address = { 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c };     /* mact host entry */

    proc_name = "dnx_basic_example_ipvx_for_srv6_inner";
    printf("%s(): Enter. rch_port %d out_port %d  fec 0x%08X\r\n",proc_name, rch_port, out_port, fec);

    /*
     * encap id for jr2 must be > 2k
     */
    encap_id = 0x1384 + diff_const;

    /*
     * Jr2 myMac assignment is more flexible than in Jer1
     */
    intf_out_mac_address[0] = 0x00;
    intf_out_mac_address[1] = 0x12;
    intf_out_mac_address[2] = 0x34;
    intf_out_mac_address[3] = 0x56;
    intf_out_mac_address[4] = 0x78;
    intf_out_mac_address[5] = 0x9a;


    /*
     * 1. Set In-Port to In ETh-RIF
     */

    /* We don't need to add this configuration for RCH port, this is handled with RCH Context */

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }
    /*
     * 4. Set Incoming ETH-RIF properties
     */
    ingress_rif.vrf = vrf;
    ingress_rif.intf_id = intf_in;
    rv = intf_ingress_rif_set(unit, &ingress_rif);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
        return rv;
    }

    /*
     * 5. Create ARP and set its properties
     */

    printf("%s(): Going to call l3__egress_only_encap__create_inner() with encap_id 0x%08X, flags2 0x%08X\n",
                proc_name, encap_id, flags2);
    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, flags2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object ARP only\n",proc_name);
        return rv;
    }

    /*
     * 6. Create FEC and set its properties
     * only in case the host format is not "no-fec"
     */

    printf("%s(): Create main FEC and set its properties.\r\n",proc_name);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    rv = l3__egress_only_fec__create_inner(unit, fec, 0 , encap_id, gport, 0,
                                           0, 0,&encoded_fec);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, create egress object FEC only\n",proc_name);
        return rv;
    }
    printf("%s(): fec 0x%08X encoded_fec 0x%08X.\r\n",proc_name, fec, encoded_fec);

    /*
     * 8. Add host entry
     */
    int _l3_itf;
    BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
    BCM_GPORT_LOCAL_SET(gport, out_port);
    if(srh_next_protocol == 4)
    {
        rv = add_host_ipv4(unit, host, vrf, _l3_itf, intf_out, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    else if(srh_next_protocol == 41)
    {
        bcm_ip6_t route_ipv6_dip = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x13 };
        rv = add_host_ipv6_encap_dest(unit, route_ipv6_dip, vrf, _l3_itf, intf_out, 0);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
            return rv;
        }
    }
    else if(srh_next_protocol == 59) /* L2 o SRv6 case, configure MAC-T address */
    {
        bcm_l2_addr_t l2_addr;

        bcm_l2_addr_t_init(&l2_addr, mac_table_fwd_address, vsi);
        l2_addr.flags = BCM_L2_STATIC;      /* static entry */
        l2_addr.port = out_port;
        rv = bcm_l2_addr_add(unit, &l2_addr);

        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_l2_addr_add\n");
            return rv;
        }

    }
    else
    {
        printf("%s(): Error, invalid next protocol value (%d), \n",proc_name, srh_next_protocol);
        return -1;
    }

    /*
     * 9. Configure the SRv6 Egress USP 1st Pass P2P IN_LIF into RCH Port and Ethernet Encapsulation
     */

    /** Set Out-Port default properties for the recycled packets*/
    rv = out_port_set(unit, rch_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set recycle_port \n");
        return rv;
    }

    /** configure port as recycle port, Part of context selection at 2nd pass VTT1 */
    rv = bcm_port_control_set(unit, rch_port, bcmPortControlOverlayRecycle, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_control_set \n");
        return rv;
    }

    int recycle_entry_encap_id;
    /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
    rv = create_recycle_entry(unit, srh_nxt_hdr_to_eth_type(srh_next_protocol), &recycle_entry_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create_recycle_entry \n");
        return rv;
    }

    /** define the structure which holds the mapping between next_protocol to RCH port and encap_id */
    bcm_srv6_terminator_next_protocol_mapping_t next_protocol_info;

    next_protocol_info.flags = 0;
    next_protocol_info.next_protocol = srh_next_protocol;
    /** Convert Port number to GPORT */
    BCM_GPORT_LOCAL_SET(next_protocol_info.port, rch_port);
    /** Convert from L3_ITF to GPORT */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(next_protocol_info.encap_id , recycle_entry_encap_id);

    /** call the next_protocol API to map to RCH port and encapsulation */
    rv = bcm_srv6_terminator_next_protocol_add(unit, &next_protocol_info);

    if (rv != BCM_E_NONE)
     {
         printf("Error, bcm_srv6_terminator_next_protocol_add \n");
         return rv;
     }


    printf("%s(): Exit\r\n",proc_name);
    return rv;
}


int
dnx_basic_example_ipvx_for_srv6(
    int unit,
    int srh_next_protocol,
    int rch_port,
    int out_port)
{
    int rv;
    char *proc_name;

    int fec;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &fec);
    fec += 30;
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    proc_name = "dnx_basic_example_ipv4_for_srv6";
    printf("%s(): Enter. rch_port %d out_port %d fec 0x%x\r\n",proc_name, rch_port, out_port, fec);
    rv = dnx_basic_example_ipvx_for_srv6_inner(unit, srh_next_protocol,  rch_port, out_port, fec);
    printf("%s(): Exit\r\n",proc_name);
    return rv;
}
