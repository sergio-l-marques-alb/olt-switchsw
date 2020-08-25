/*
 * $Id: cint_mpls_deep_stack.c, Exp $
 $Copyright: (c) 2018 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_mpls_deep_stack.c Purpose: utility for MPLS deep encapsulation stack.. 
 */

/*
 * 
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/cint_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/cint_mpls_deep_stack.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * int nof_tunnel = 6;
 * rv = mpls_deep_stack_example(unit,in_port,out_port,nof_tunnel);
 * print rv; 
 * 
 * 
 
 *  Scenarios configured in this cint:
 *  In each cint itteration only 1 scenario is configured, based on defined 'mode'
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   mode = 0 - basic with 1 MPLS label
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an 1 MPLS and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic from port=200
 *   ----------------------------->port=200
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+--+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA                |     SA                || ethtype  |        IPv4
 *   |    |00:0c:00:02:00:00       |00:00:07:00:01:00      || 0x800    | 0x4500
 *   |    |                        |                       ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||  MPLS        ||   MPLS     ||  MPLS    ||  .......... ||  MPLS       ||    IPv4  
 *   |    |0c:00:02:00:01        |00:00:00:cd:1d        ||Label:0x6100  ||Label:0x6000||Label:5100||  .......... ||0x1000       ||    0x4500
 *   |    |                      |                       ||Exp:0         ||Exp:0      ||               ......... ||             ||                      ||
 *   |    |                      |                      ||TTL:20          ||TTL:20    ||             ............||             ||                      ||
 *   |    |                      |                      ||<---------------------------------nof_tunnel*2----------------------->||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct mpls_tunnel_initiator_create_s
{
    bcm_mpls_egress_action_t action;    /* MPLS label action, relevant when BCM_MPLS_EGRESS_LABEL_ACTION_VALID is set. */
    bcm_mpls_label_t label[2];
    uint32 flags;                       /* BCM_MPLS_EGRESS_LABEL_xxx. */
    bcm_if_t tunnel_id;                 /* Tunnel Interface. */
    bcm_if_t l3_intf_id;                /* l3 Interface ID. */
    int num_labels;
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    int qos_map_id;                     /* QOS map identifier. */
};

int MAX_NOF_TUNNELS = 8;
int outer_mpls_tunnel_index = 0;
int MIN_LABEL = 0;
int MAX_LABEL = 0x000FFFFF;

mpls_tunnel_initiator_create_s mpls_encap_tunnel[MAX_NOF_TUNNELS];

/**
 * Procedure for mpls initiator data initiation
 * INPUT:
 *   tunnel  - pointer to tunnel structure
 */
void
mpls_tunnel_initiator_create_s_init(mpls_tunnel_initiator_create_s *tunnel)
{
    int i;

    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        tunnel[i].action = BCM_MPLS_EGRESS_ACTION_SWAP;
        tunnel[i].flags = 0;
        tunnel[i].l3_intf_id = 0;
        tunnel[i].num_labels = 0;
        tunnel[i].label[0] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].label[1] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].tunnel_id = 0;
        tunnel[i].encap_access = bcmEncapAccessInvalid;
        tunnel[i].qos_map_id = 0;
    }
}

/**
 * Procedure for L3 interface initiation
 * INPUT:
 *   nof_tunnel  -number of MPLS tunnel
 */
int
mpls_deep_stack_para_init (
    int unit,
    int nof_tunnel)
{
    int rv = BCM_E_NONE;
    int tunnel_index = 0;

    bcm_encap_access_t tunnel_inx_to_acces[MAX_NOF_TUNNELS] = {
                                        bcmEncapAccessNativeArp,
                                        bcmEncapAccessTunnel1,
                                        bcmEncapAccessTunnel2,
                                        bcmEncapAccessTunnel3,
                                        bcmEncapAccessTunnel4,
                                        bcmEncapAccessArp,
                                        bcmEncapAccessInvalid,
                                        bcmEncapAccessInvalid
                                    };

    mpls_tunnel_initiator_create_s_init(&mpls_encap_tunnel);

    for(tunnel_index = 0;tunnel_index < nof_tunnel;tunnel_index++) {
        mpls_encap_tunnel[tunnel_index].num_labels = 2;
        mpls_encap_tunnel[tunnel_index].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_encap_tunnel[tunnel_index].label[0] = 0x1000 * (tunnel_index+1) ;
        mpls_encap_tunnel[tunnel_index].label[1] = mpls_encap_tunnel[tunnel_index].label[0] + 0x100 ;
        mpls_encap_tunnel[tunnel_index].tunnel_id = 8196+tunnel_index;
        mpls_encap_tunnel[tunnel_index].encap_access = tunnel_inx_to_acces[tunnel_index];

        if (tunnel_index&1) {
            mpls_encap_tunnel[tunnel_index].flags |= BCM_MPLS_EGRESS_LABEL_TANDEM;
        } 
    }

    outer_mpls_tunnel_index = nof_tunnel-1;

    return rv;
}

/**
 * Procedure for L3 interface initiation
 * INPUT:
 *   in_port  -traffic incoming port
 *   out_port -traffic outgoing port
 */
int
mpls_deep_stack_l3_intf_init(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int intf_in = 15;           /* Incoming packet ETH-RIF */
    int intf_out = 100;         /* Outgoing packet ETH-RIF */
    int encap_id = 900;         /* ARP-Link-layer */
    int host_encap_id = 0;
    bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
    bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
    bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
    bcm_gport_t gport;
    l3_ingress_intf ingress_rif;
    int vrf = 1;
    l3_ingress_intf_init(&ingress_rif);
    int vlan = 100;

    if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * encap id for jer2 must be > 2k
         */
        encap_id = 0;
    }

    /*
     * 1. Set In-Port to In ETh-RIF 
     */
    rv = in_port_intf_set(unit, in_port, intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * 2. Set Out-Port default properties, in case of ARP+AC no need
     */
    rv = out_port_set(unit, out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }

    /*
     * 3. Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
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
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    rv = l3__egress_only_encap__create_inner(unit, 0, &encap_id, arp_next_hop_mac, vlan, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    mpls_encap_tunnel[outer_mpls_tunnel_index].l3_intf_id = encap_id;
    return rv;
}

/**
 * Procedure for MPLS tunnel configuration.
 * INPUT:
 *   mpls_tunnel   - mpls tunnel data
 * Note
 *  By default, code is assumed to occupy GLEM entries only when
 *  necessary (and not just when it does no harm). 
 */
int
mpls_deep_stack_create_mpls_tunnel(
    int unit,
    mpls_tunnel_initiator_create_s *mpls_tunnel)
{
    bcm_mpls_egress_label_t label_array[2];
    int rv, i;
    char *proc_name;
    char *occupy_glem;

    proc_name = "mpls_deep_stack_create_mpls_tunnel";
    /* printf("%s(): ENTER. mpls_tunnel[0].label[0] %d\n", proc_name, mpls_tunnel[0].label[0]); */

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
    {
        if ((mpls_tunnel[i].label[0] >= MIN_LABEL) && (mpls_tunnel[i].label[0] <= MAX_LABEL))
        {
            label_array[0].label = mpls_tunnel[i].label[0];
            label_array[0].flags = mpls_tunnel[i].flags;
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[i].tunnel_id);
            label_array[0].l3_intf_id = mpls_tunnel[i].l3_intf_id;
            label_array[0].action = mpls_tunnel[i].action;
            label_array[0].encap_access = mpls_tunnel[i].encap_access;
            label_array[0].qos_map_id = mpls_tunnel[i].qos_map_id;

            label_array[1].label = mpls_tunnel[i].label[1];
            label_array[1].flags = mpls_tunnel[i].flags;
            BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[i].tunnel_id);
            label_array[1].l3_intf_id = mpls_tunnel[i].l3_intf_id;
            label_array[1].action = mpls_tunnel[i].action;
            label_array[1].encap_access = mpls_tunnel[i].encap_access;
            label_array[1].qos_map_id = mpls_tunnel[i].qos_map_id;

            /*
             * By default, GLEM occupation should be minimized and if this cycle is not
             * the 'root' of the linked list (for which GLEM entry is required) then do
             * not load GLEM. This is achieved by setting the BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED
             * flag.
             */
            occupy_glem = "YES";
            if (i != 0)
            {
                label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED;
                occupy_glem = "NO";
            }
            printf("%s(): Going to call bcm_mpls_tunnel_initiator_create() for i = %d, occupy_glem = %s\n", proc_name, i, occupy_glem);

            rv = bcm_mpls_tunnel_initiator_create(unit, 0, mpls_tunnel[i].num_labels, label_array);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_mpls_tunnel_initiator_create\n");
                return rv;
            }

            mpls_tunnel[i].tunnel_id = label_array[0].tunnel_id;
            if (i != 0)
            {
                mpls_tunnel[i-1].l3_intf_id = mpls_tunnel[i].tunnel_id;
            }
        }
    }
    
    /* printf("%s(): EXIT. mpls_tunnel[0].label[0] %d\n", proc_name, mpls_tunnel[0].label[0]); */
    return rv;
}

/**
 * Procedure for IPv4 routing configuration.
 * INPUT:
 *   in_port   - traffic incoming port
 *   out_port -traffic outgoing port
 */
int
ipv4_route_into_deep_mpls_stack(
    int unit,
    int in_port,
    int out_port) 
{
    int rv = 0;
    int fec = 40961;
    int vrf = 1;
    bcm_gport_t gport;
    uint32 route = 0x7fffff00;
    uint32 host = 0x7fffff02;
    uint32 mask = 0xfffffff0;

    BCM_GPORT_LOCAL_SET(gport, out_port);

    rv = l3__egress_only_fec__create_inner(unit, fec, 0, mpls_encap_tunnel[0].tunnel_id, gport, 0, 0,
            0,NULL);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }


    /*
     * Add Route entry
     */
    rv = add_route_ipv4(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function internal_ip_route, \n");
        return rv;
    }

    return 0;
}

/**
 * Main entrance for mpls deep stack configuration
 * INPUT:
 *   in_port   - traffic incoming port 
 *   out_port - traffic outgoing port
 *   nof_tunnel  - number of mpls tunnel(1~6 is expected)
 */
int
mpls_deep_stack_example(
    int unit,
    int in_port,
    int out_port,
    int nof_tunnel)
{
    int rv = 0;

    rv = mpls_deep_stack_para_init(unit,nof_tunnel);
    if (rv) {
       printf("Error, mpls_deep_stack_para_init\n");
        return rv;
    }

    rv = mpls_deep_stack_l3_intf_init(unit,in_port,out_port);
    if (rv) {
       printf("Error, mpls_deep_stack_l3_intf_init\n");
        return rv;
    }

    rv = mpls_deep_stack_create_mpls_tunnel(unit,mpls_encap_tunnel);
    if (rv) {
       printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    rv = ipv4_route_into_deep_mpls_stack(unit,in_port,out_port);
    if (rv) {
       printf("Error, create_mpls_tunnel\n");
        return rv;
    }

    return rv;
}
