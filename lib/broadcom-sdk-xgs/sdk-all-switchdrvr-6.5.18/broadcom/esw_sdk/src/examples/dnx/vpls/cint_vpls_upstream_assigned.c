/*
 * $Id: cint_vpls_upstream_assigned.c, Exp $
 $Copyright: (c) 2019 Broadcom.
 Broadcom Proprietary and Confidential. All rights reserved.$ File: cint_vpls_no_mpls.c Purpose: Example of upstream assigned PWE.
 */

/*
 * This example demonstrate 2 configurations of pwe in-lif using upstream assigned matching
 * information.
 *
 * Scenario 1 - PWE termination in the context of the preceding interface (LIF/RIF)
 *   In this scenario, the received VC label may come from several peers and should
 *   be terminated in the context of the specific peer link. In case the link is L2
 *   it would be a RIF, in case the link is L3 (MPLS tunnel, etc.), it would  be  a
 *   LIF.
 *   The example for this scenario defines 2 in-RIFs and 2 MPLS tunnel terminations
 *   and 4 PWE LIFs, each with a different context interface and the same VC label.
 *   Each LIF will assign  a different VSI,  which will affect the out vlan  of the
 *   packet (for validation).
 *
 * +--------------------------------------------------------------------------------------------------------+
 * |                                           +---------------------+                                      |
 * |                                           |                     |                                      |
 * |                                           |      +-------+      +----------+                           |
 * |                                +----------+      |       |      |          |                           |
 * | DA: A0:01::11                  |          |      |  VSI  |      |          |                           |
 * | VID: 0x50    +-----------------------------------> 0x100 +--------------------------------> Native ETH |
 * | PWE: 0x1000                    |          |      |       |      |          |                VID: 0x100 |
 * | Native ETH untagged            |          |      +-------+      |          |                           |
 * |                                |          |                     |          |                           |
 * |                                |          |      +-------+      |          |                           |
 * |                                |          |      |       |      |          |                           |
 * | DA: A0:01::22                  |          |      |  VSI  |      |          |                           |
 * | VID: 0x60    +-----------------------------------> 0x200 +--------------------------------> Native ETH |
 * | PWE: 0x1000                    |          |      |       |      |          |                VID: 0x200 |
 * | Native ETH untagged            |          |      +-------+      |          |                           |
 * |                                | In       |                     |  Out     |                           |
 * |                                | Port     |      +-------+      |  Port    |                           |
 * |                                |          |      |       |      |          |                           |
 * | DA: A0:01::33                  |          |      |  VSI  |      |          |                           |
 * | VID: 0x70    +-----------------------------------> 0x300 +--------------------------------> Native ETH |
 * | MPLS: 0x2100                   |          |      |       |      |          |                VID: 0x300 |
 * | PWE: 0x1000                    |          |      +-------+      |          |                           |
 * | Native ETH untagged            |          |                     |          |                           |
 * |                                |          |      +-------+      |          |                           |
 * |                                |          |      |       |      |          |                           |
 * | DA: A0:01::33                  |          |      |  VSI  |      |          |                           |
 * | VID: 0x70    +-----------------------------------> 0x400 +--------------------------------> Native ETH |
 * | MPLS: 0x2200                   |          |      |       |      |          |                VID: 0x400 |
 * | PWE: 0x1000                    +----------+      +-------+      +----------+                           |
 * | Native ETH untagged                       |                     |                                      |
 * |                                           +---------------------+                                      |
 * +--------------------------------------------------------------------------------------------------------+
 *
 *
 * Scenario 2 - PWE termination coupled with the preceding (one above) MPLS label
 *   In this scenario, the received VC label  may arrive one different MPLS tunnels
 *   and should be terminated  in the context of the preceding label. This protocol
 *   requires that the ETH-Type  of the outer ETH header would be  0x8848. The only
 *   packet structure that is supported for this kind of termination is:
 *   Native-Headers o PWE o MPLS o ETH(with MPLS_UA eth-type)
 *   The MPLS tunnel  should not  be defined as a separate  tunnel terminated MPLS,
 *   but instead, be defined as a context label in the mpls_port object.
 *   The example  for this scenario  defines  2 RIFs and  2 PWE lifs with different
 *   MPLS context labels. Each LIF  will assign a different VSI,  which will affect
 *   the out vlan of the packet for validation.
 *
 * +-------------------------------------------------------------------------------+
 * |                                                                               |
 * |                                 +----------------+                            |
 * |                                 |                |                            |
 * |                                 |  +----------+  |                            |
 * |                                 |  |          |  |                            |
 * | DA:   A0:01::44          +------+  |   VSI    |  +------+                     |
 * | VID:  0x40               |      |  |  0x500   |  |      |                     |
 * | MPLS   + PWE   +------------------->          +------------------> Native ETH |
 * | 0x2300   0x1000          |      |  |          |  |      |          VID: 0x500 |
 * | Native ETH untagged      |      |  +----------+  |      |                     |
 * |                          | In   |                | Out  |                     |
 * |                          | Port |                | Port |                     |
 * |                          |      |                |      |                     |
 * | DA:   A0:01::44          |      |  +----------+  |      |                     |
 * | VID:  0x40               |      |  |          |  |      |                     |
 * | MPLS   + PWE   +------------------->   VSI    +------------------> Native ETH |
 * | 0x2400   0x1000          |      |  |  0x600   |  |      |          VID: 0x600 |
 * | Native ETH untagged      +------+  |          |  +------+                     |
 * |                                 |  |          |  |                            |
 * |                                 |  +----------+  |                            |
 * |                                 |                |                            |
 * |                                 +----------------+                            |
 * |                                                                               |
 * +-------------------------------------------------------------------------------+
 *
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_upstream_assigned.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port = 201;
 *
 * --> Scenario 1 <--
 * rv = vpls_upstream_assigned_main__intf_context(unit, in_port, out_port);
 * print rv;
 *
 * --> Scenario 2 <--
 * rv = vpls_upstream_assigned_main__coupled(unit, in_port, out_port);
 * print rv;
 */

/**********************************************************************************************
 *                                                                                            *
 *                                       SCENARIO 1                                           *
 *                                                                                            *
 **********************************************************************************************/
/*
 * No. in-RIFs to create for scenario 1
 */
int NOF_IN_RIFS = 3;
/*
 * No. in-RIFs that will be used as context for PWE matching for scenario 1
 */
int NOF_IN_RIFS_FOR_CTX = 2;
/*
 * No. of MPLS tunnel terminations that will be used as context for PWE matching for scenario 1
 */
int NOF_CTX_MPLS_TUNNELS = 2;
/*
 * No. out interfaces for scenario 1
 */
int NOF_OUT_VLANS = 4;


struct vpls_ua_per_if_info_s
{
    bcm_vlan_t in_vlans[NOF_IN_RIFS];
    bcm_mac_t in_rif_mac_base;

    bcm_vlan_t out_vlans[NOF_OUT_VLANS];

    bcm_mpls_label_t pwe_label;
    bcm_mpls_label_t mpls_labels[NOF_CTX_MPLS_TUNNELS];
    bcm_if_t mpls_tunnel_ifs[NOF_CTX_MPLS_TUNNELS];
};

vpls_ua_per_if_info_s vpls_ua_per_if_info;

void
vpls_ua_per_if_info_init(
    int unit)
{
    bcm_mac_t rif_mac_base = {0xA0, 0x01, 0x00, 0x00, 0x00, 0x00};

    vpls_ua_per_if_info.in_vlans[0] = 0x50;
    vpls_ua_per_if_info.in_vlans[1] = 0x60;
    vpls_ua_per_if_info.in_vlans[2] = 0x70;

    vpls_ua_per_if_info.in_rif_mac_base = rif_mac_base;

    vpls_ua_per_if_info.out_vlans[0] = 0x100;
    vpls_ua_per_if_info.out_vlans[1] = 0x200;
    vpls_ua_per_if_info.out_vlans[2] = 0x300;
    vpls_ua_per_if_info.out_vlans[3] = 0x400;

    vpls_ua_per_if_info.pwe_label = 0x1000;

    vpls_ua_per_if_info.mpls_labels[0] = 0x2100;
    vpls_ua_per_if_info.mpls_labels[1] = 0x2200;
}

/*
 * Main function for scenario 2 (see details in file header).
 */
int
vpls_upstream_assigned_main__intf_context(
    int unit,
    int pwe_port,
    int ac_port)
{
    int rv = BCM_E_NONE;
    int idx;

    printf("Init global info\n");
    vpls_ua_per_if_info_init(unit);

    printf("Set out port\n");
    rv = out_port_set(unit, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in out_port_set\n");
        return rv;
    }

    printf("Add out vlans\n");
    for (idx = 0; idx < NOF_OUT_VLANS; idx++)
    {
        rv = vpls_ua_out_vlan_create(unit, ac_port, vpls_ua_per_if_info.out_vlans[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vpls_ua_out_vlan_create %d\n", idx);
            return rv;
        }
    }

    printf("Set in-port vlan domain\n");
    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set\n");
        return rv;
    }

    printf("Create in RIFs\n");
    for (idx = 0; idx < NOF_IN_RIFS; idx++)
    {
        int l3_ingress_flags = BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
        if (idx >= NOF_IN_RIFS_FOR_CTX)
        {
            l3_ingress_flags = 0;
        }
        bcm_mac_t rif_mac = vpls_ua_per_if_info.in_rif_mac_base;
        rif_mac[5] = 0x11 * (idx + 1);
        rv = vpls_ua_in_eth_link_create(unit, pwe_port,
                                        vpls_ua_per_if_info.in_vlans[idx], /** VID */
                                        vpls_ua_per_if_info.in_vlans[idx], /** VSI */
                                        rif_mac, l3_ingress_flags);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vpls_ua_in_eth_link_create %d\n", idx);
            return rv;
        }
    }

    printf("Create MPLS tunnel terminations\n");
    for (idx = 0; idx < NOF_CTX_MPLS_TUNNELS; idx++)
    {
        rv = vpls_ua_ctx_mpls_tunnel_term_create(unit, vpls_ua_per_if_info.mpls_labels[idx],
                                                 &vpls_ua_per_if_info.mpls_tunnel_ifs[idx]);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vpls_ua_ctx_mpls_tunnel_term_create %d\n", idx);
            return rv;
        }
    }

    printf("Create ingress PWE in RIF contexts\n");
    for (idx = 0; idx < NOF_IN_RIFS_FOR_CTX; idx++)
    {
        bcm_if_t intf = vpls_ua_per_if_info.in_vlans[idx]; /** The RIF was created with the vid as vsi-id */
        int vsi = vpls_ua_per_if_info.out_vlans[idx];
        rv = vpls_ua_intf_ctx_pwe_create(unit, pwe_port, intf, vpls_ua_per_if_info.pwe_label, vsi);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vpls_ua_intf_ctx_pwe_create RIF=0x%x\n", intf);
            return rv;
        }
    }

    printf("Create ingress PWE in MPLS tunnel contexts\n");
    for (idx = 0; idx < NOF_CTX_MPLS_TUNNELS; idx++)
    {
        bcm_if_t intf = vpls_ua_per_if_info.mpls_tunnel_ifs[idx];
        int vsi = vpls_ua_per_if_info.out_vlans[idx + NOF_IN_RIFS_FOR_CTX];
        rv = vpls_ua_intf_ctx_pwe_create(unit, pwe_port, intf, vpls_ua_per_if_info.pwe_label, vsi);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vpls_ua_intf_ctx_pwe_create RIF=0x%x\n", intf);
            return rv;
        }
    }

    return rv;
}

/*
 * Creates an MPLS tunnel termination with the specified 'label' and sets
 * it's interface to signal MPLS per-if lookup.
 * Returns the intf id used for the lookup.
 */
int
vpls_ua_ctx_mpls_tunnel_term_create(
    int unit,
    bcm_mpls_label_t label,
    bcm_if_t * intf)
{
    int rv = BCM_E_NONE;

    bcm_mpls_tunnel_switch_t entry;
    bcm_if_t l3_intf_id;
    l3_ingress_intf ingr_itf;

    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_POP;
    entry.label = label;
    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_mpls_tunnel_switch_create\n", rv);
        return rv;
    }

    /*
     * Set Incoming Tunnel-IF properties
     */
    l3_ingress_intf_init(&ingr_itf);
    ingr_itf.flags = BCM_L3_INGRESS_MPLS_INTF_NAMESPACE;
    BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(l3_intf_id, entry.tunnel_id);
    ingr_itf.intf_id = l3_intf_id;
    ingr_itf.vrf = 2;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), intf_ingress_rif_set\n", rv);
        return rv;
    }

    *intf = ingr_itf.intf_id;

    printf("Terminated label %d (0x%05x) created: tunnel_id = 0x%x, intf = 0x%x\n",
           entry.label, entry.label, entry.tunnel_id, ingr_itf.intf_id);


    return rv;
}


/*
 * Creates a PWE + Context-IF label termination. The LIF will assign the given 'vsi'.
 * The 'port' is used for forwarding information, but is not expected to be used as this is a termination only object.
 */
int
vpls_ua_intf_ctx_pwe_create(
    int unit,
    int port,
    bcm_if_t ctx_intf,
    bcm_mpls_label_t pwe_label,
    int vsi)
{
    int rv = BCM_E_NONE;

    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL_L3_INGRESS_INTF;
    mpls_port.match_label = pwe_label;
    mpls_port.ingress_if = ctx_intf;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    mpls_port.port = port;
    rv = bcm_mpls_port_add(unit, vsi, &mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_port_add\n");
        return rv;
    }

    return rv;
}


/**********************************************************************************************
 *                                                                                            *
 *                                       SCENARIO 2                                           *
 *                                                                                            *
 **********************************************************************************************/

struct vpls_ua_coupled_info_s
{
    bcm_vlan_t in_vlan;
    bcm_mac_t in_rif_mac;

    bcm_vlan_t out_vlan_1;
    bcm_vlan_t out_vlan_2;

    bcm_mpls_label_t pwe_label;
    bcm_mpls_label_t mpls_label_1;
    bcm_mpls_label_t mpls_label_2;
};

vpls_ua_coupled_info_s vpls_ua_coupled_info;

void
vpls_ua_coupled_info_init(
    int unit)
{
    bcm_mac_t my_mac = {0xA0, 0x01, 0x00, 0x00, 0x00, 0x44};

    vpls_ua_coupled_info.in_vlan = 0x80;
    vpls_ua_coupled_info.in_rif_mac = my_mac;

    vpls_ua_coupled_info.pwe_label = 0x1000;
    vpls_ua_coupled_info.mpls_label_1 = 0x2300;
    vpls_ua_coupled_info.mpls_label_2 = 0x2400;

    vpls_ua_coupled_info.out_vlan_1 = 0x500;
    vpls_ua_coupled_info.out_vlan_2 = 0x600;
}

/*
 * Main function for scenario 2 (see details in file header).
 */
int
vpls_upstream_assigned_main__coupled(
    int unit,
    int pwe_port,
    int ac_port)
{
    int rv = BCM_E_NONE;

    printf("Init global info\n");
    vpls_ua_coupled_info_init(unit);

    printf("Set out port\n");
    rv = out_port_set(unit, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in out_port_set\n");
        return rv;
    }

    printf("Add out vlans\n");
    rv = vpls_ua_out_vlan_create(unit, ac_port, vpls_ua_coupled_info.out_vlan_1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vpls_ua_out_vlan_create 1\n");
        return rv;
    }
    rv = vpls_ua_out_vlan_create(unit, ac_port, vpls_ua_coupled_info.out_vlan_2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vpls_ua_out_vlan_create 2\n");
        return rv;
    }

    printf("Set in-port vlan domain\n");
    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set\n");
        return rv;
    }

    printf("Create in RIF\n");
    rv = vpls_ua_in_eth_link_create(unit, pwe_port, vpls_ua_coupled_info.in_vlan, vpls_ua_coupled_info.in_vlan,
                                    vpls_ua_coupled_info.in_rif_mac, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vpls_ua_in_eth_link_create\n");
        return rv;
    }

    printf("Create ingress coupled PWEs\n");
    rv = vpls_ua_coupled_pwe_create(unit, pwe_port, vpls_ua_coupled_info.mpls_label_1, vpls_ua_coupled_info.pwe_label,
                                    vpls_ua_coupled_info.out_vlan_1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vpls_ua_coupled_pwe_create 1\n");
        return rv;
    }
    rv = vpls_ua_coupled_pwe_create(unit, pwe_port, vpls_ua_coupled_info.mpls_label_2, vpls_ua_coupled_info.pwe_label,
                                    vpls_ua_coupled_info.out_vlan_2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in vpls_ua_coupled_pwe_create 2\n");
        return rv;
    }

    return rv;
}

/*
 * Creates a PWE+MPLS label termination. The LIF will assign the given 'vsi'.
 * The 'port' is used for forwarding information, but is not expected to be used as this is a termination only object.
 */
int
vpls_ua_coupled_pwe_create(
    int unit,
    int port,
    bcm_mpls_label_t mpls_label,
    bcm_mpls_label_t pwe_label,
    int vsi)
{
    int rv = BCM_E_NONE;

    bcm_mpls_port_t mpls_port;
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL_CONTEXT_LABEL;
    mpls_port.match_label = pwe_label;
    mpls_port.context_label = mpls_label;
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    mpls_port.port = port;
    rv = bcm_mpls_port_add(unit, vsi, &mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_port_add\n");
        return rv;
    }

    return rv;
}



/**********************************************************************************************
 *                                                                                            *
 *                               SHARED UTILITIES                                             *
 *                                                                                            *
 **********************************************************************************************/

/*
 * Setup a vlan port for the specific VSI. Create a flooding group for the VSI and add the
 * vlan port to it.
 */
int
vpls_ua_out_vlan_create(
    int unit,
    int port,
    bcm_vlan_t vid)
{
    int rv = BCM_E_NONE;

    bcm_vlan_port_t vlan_port;
    int mcid = vid;
    int vsi = vid;

    rv = bcm_vlan_create(unit, vid);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_create\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = port;
    vlan_port.match_vlan = vid;
    vlan_port.egress_vlan = vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = multicast__open_mc_group(unit, &mcid, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mcid, &port, &vlan_port.encap_id, 1, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__add_multicast_entry\n");
        return rv;
    }

    return rv;
}

/*
 * Setup the port's incoming vlan domain.
 * Creates a vlan port to deduce the outer eth VSI.
 * Creates in-RIF to terminate the outer ETH.
 */
int
vpls_ua_in_eth_link_create(
    int unit,
    int port,
    bcm_vlan_t vid,
    bcm_vlan_t vsi,
    bcm_mac_t rif_mac,
    int rif_ingress_flags)
{
    int rv = BCM_E_NONE;

    bcm_vlan_port_t vlan_port;

    sand_utils_l3_eth_rif_s l3_eth_rif;
    int rif_id = vsi;
    int vrf = 1;

    rv = bcm_vlan_gport_add(unit, vid, port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }

    rv = bcm_vlan_create(unit, vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_create\n");
        return rv;
    }

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = port;
    vlan_port.match_vlan = vid;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vswitch_port_add\n");
        return rv;
    }

    sand_utils_l3_eth_rif_s_common_init(unit, 0, &l3_eth_rif, rif_id, 0, rif_ingress_flags, rif_mac, vrf);
    rv = sand_utils_l3_eth_rif_create(unit, &l3_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in sand_utils_l3_eth_rif_create\n");
        return rv;
    }

    return rv;
}

