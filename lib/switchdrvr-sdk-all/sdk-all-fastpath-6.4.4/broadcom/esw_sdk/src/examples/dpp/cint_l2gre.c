/* $Id: cint_l2gre.c,v 1.15 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

/* 
 * The following soc properties should be set:
 * bcm886xx_l2gre_enable=1
 * bcm886xx_ip4_tunnel_termination_mode=0
 
 * how to run:
 *
cint;
cint_reset();
exit; 
cint ../../../../src/examples/dpp/utility/cint_utils_global.c    
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel.c 
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c  
cint ../../../../src/examples/dpp/cint_mact.c
cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c 
cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
cint ../../../../src/examples/dpp/cint_l2gre.c 
cint
verbose = 3;
int rv; 
print l2_gre_example(0,13,13); 
 
//this to show how to remove/get entries 
print l2gre_example_get_remove (0,1,0); 
 
 * Tunnel side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |   Ethernet (+VLAN):
 *   |   ------------------------------------------------------------------------------------------
 *   |   |        DA         |        SA         | TIPD1  | Prio | VID | EtherType |
 *   |   | 00:0C:00:02:00:00 | 00:00:07:00:01:00 | 0x8100 |      | 20  | 0x0800|    
 *   |   ------------------------------------------------------------------------------------------
 *   |
 *   |   IPv4:
 *   |   ------------------------------------------------------
 *   |    ...  |       SA        |     DA     | Protocol | ... 
 *   |    ...  | 155.155.155.155 | 171.0.0.17 |   GRE    | ...
 *   |   ------------------------------------------------------
 *   |
 *   |   GRE:
 *   |   -----------------------------------------------------------
 *   |    ...  | Key-Present | Protocol |   Key    |  <Contents...>
 *   |    ...  |      1      |  0xFEEF  | 0x138800 |  <Contents...>
 *   |   -----------------------------------------------------------
 *   |
 *   |   Ethernet (Inner):
 *   |   --------------------------------
 *   |    ...  DA            | SA |    | <Contents...>
 *   |    00:00:00:00:00:f0  |    |    | <Contents...>
 *   |   --------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 
 
 the below cint pupulate the MACT with following static entries
 ----------------------------------------------------------------------------------
|                                      MAC Table                                   |
|----------------------------------------------------------------------------------|
|      |                   |   |SA |grp| Destination  |     Additional Info        |
| FID  |      MAC/IP       |Dyn|Drp|   | Type  | Val  |Type |                      |
 ---------------------------------------------------------------------------------- 
| 4500 | 00:00:00:00:00:f2 | X | X | - |fec    |1024  |  EEI |EMPTY |       -      | <-- match packet forwarded to l2gre_port_id[0]
| 4500 | 00:00:00:00:00:f0 | X | X | - |uc_port|13    |OUTLIF|raw    |   8192      | <-- match packet forwarded to vlan_port
| 4500 | 00:00:00:00:00:f1 | X | X | - |uc_port|13    |OUTLIF|raw    |   16384     | <-- match packet forwarded to l2gre_port_id[1]
 -----------------------------------------------------------------------------
*
* L2GRE MC bud node example.
*   Bud node some copies are IP-forwarded, some copies are IP-terminated;
*   and L2-bridged according to the exposed DA.
* The following soc properties should be set:
* tm_port_header_type_in_40.BCM88650=ETH
* tm_port_header_type_out_40.BCM88650=ETH
* ucode_port_40.BCM88650=RCY.0
* ucode_port_41.BCM88650=RCY.1
* ucode_port_42.BCM88650=RCY.2
* bcm886xx_ip4_tunnel_termination_mode=2
* bcm886xx_l2gre_enable=1
* bcm886xx_l2gre_tunnel_lookup_mode=1
* default_logical_interface_ip_tunnel_overlay_mc=16484
* how to run:
* cint cint_ip_route.c
* cint cint_ip_tunnel.c 
* cint cint_ip_tunnel_term.c  
* cint cint_mact.c
* cint cint_vswitch_metro_mp.c 
* cint cint_multi_device_utils.c 
* cint cint_l2gre.c 
* cint cint_ipmc_flows.c
* cint cint_pmf_2pass_snoop.c
* cint cint_field_gre_learn_data_rebuild.c
* cint
* verbose = 3;
* print l2_gre_mc_bud_node_example(0, 13, 13, 14, 40);
*
* Tunnel side packets:
* 
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*   |   Ethernet (+VLAN):
*   |   ------------------------------------------------------------------------------------------
*   |   |        DA         |        SA         | TIPD1  | Prio | VID | EtherType |
*   |   | 01:00:5E:05:05:05 | 00:00:07:00:01:00 | 0x8100 |      | 20  | 0x0800|    
*   |   ------------------------------------------------------------------------------------------
*   |
*   |   IPv4:
*   |   ------------------------------------------------------
*   |    ...  |       SIP       |     DIP    | Protocol | ... 
*   |    ...  |  171.0.0.17     | 224.5.5.5  |   GRE    | ...
*   |   ------------------------------------------------------
*   |
*   |   GRE:
*   |   -----------------------------------------------------------
*   |    ...  | Key-Present | Protocol |   Key    |  <Contents...>
*   |    ...  |      1      |  0xFEEF  | 0x138800 |  <Contents...>
*   |   -----------------------------------------------------------
*   |
*   |   Ethernet (Inner):
*   |   --------------------------------------------------------------
*   |    ...  DA            | SA                  |    | <Contents...>
*   |    00:00:00:00:00:f0  | 00:00:00:00:00:01   |    | <Contents...>
*   |   ---------------------------------------------------------------
*   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* learned information
 ----------------------------------------------------------------------------------
|                                      MAC Table                                   |
|----------------------------------------------------------------------------------|
|      |                   |   |SA |grp| Destination  |     Additional Info        |
| FID  |      MAC/IP       |Dyn|Drp|   | Type  | Val  |Type |                      |
 ---------------------------------------------------------------------------------- 
| 4500 | 00:00:00:00:00:01 | X | X | - |fec    |1024  |  EEI |EMPTY |       -      | 


*/ 

/* ********* 
  Globals/Aux Variables
 ********** */

struct l2gre_s {
    int set_egress_orientation_using_l2gre_port_add; /* egress orientation can be configured: 
                                                        - using the api bcm_l2gre_port_add, 
                                                           when egress_tunnel_id (ip tunnel initiator gport, contains outlif) is valid (optional).  
                                                        - using the api bcm_port_class_set, update egress orientation. */
};

/*                 set_egress_orientation_using_l2gre_port_add   */
l2gre_s g_l2gre = {1
};


/* debug prints */
int verbose = 1;

/********** 
  functions
 ********** */

int set_trap_to_drop(int unit, bcm_rx_trap_t trap) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int trap_id;

    bcm_rx_trap_config_t_init(&config);
             
    config.flags = (BCM_RX_TRAP_UPDATE_DEST); 
    config.trap_strength = 7;
    config.dest_port=BCM_GPORT_BLACK_HOLE;

    rv = bcm_rx_trap_type_create(unit,flags,trap,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap create, trap %d \n", trap_i);
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return rv;
}

/*
 * struct include meta information. 
 * where the cint logic pull data from this struct. 
 * use to control differ runs of the cint, without changing the cint code
 */

void ip_tunnel_glbl_init_gre(int unit, int flags){
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeL2Gre;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeL2Gre;
}



int l2_gre_open_vpn(int unit, int vpn, int vpnid){
    int rv = BCM_E_NONE;

    rv = open_ingress_mc_group(unit, vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, open_ingress_mc_group\n");
        return rv;
    }

    if(verbose >= 1){
        printf("created multicast   0x%08x\n\r",vpn);
    }

    bcm_l2gre_vpn_config_t vpn_config;
    bcm_l2gre_vpn_config_t_init(&vpn_config);

    vpn_config.flags = BCM_L2GRE_VPN_ELAN|BCM_L2GRE_VPN_WITH_ID|BCM_L2GRE_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vpnid = vpnid;

    rv = bcm_l2gre_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE) {
        printf("error in bcm_l2gre_vpn_create \n");
        return rv;
    }
    if(verbose >= 1){
        printf("created vpn   0x%08x\n\r",vpn);
    }

    return rv;
}


/*
 * add gport of type vlan-port to the multicast
 */
int multicast_l2gre_port_add(int unit, int mc_group_id, int sys_port, int gport){

    int encap_id;
    int rv;
    
    rv = bcm_multicast_l2gre_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_l2gre_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    rv = bcm_multicast_ingress_add(unit, mc_group_id, sys_port, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_ingress_add mc_group_id:  0x%08x  phy_port:  0x%08x  encap_id:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }
    
    return rv;
}

/*
 * Add l2gre to vpn
 * parameters:
 * in_port: network side port for learning info. 
 * in_tunnel: tunnel termination gport
 * out_tunnel: tunnel creation gport
 * egress_if: FEC entry
 * flags: l2gre flags
 * l2gre_port_id: returned l2gre gport
 */
int l2_gre_add_port(int unit, uint32 vpn, bcm_gport_t in_port, bcm_gport_t in_tunnel, bcm_gport_t out_tunnel, bcm_if_t egress_if, uint32 flags, bcm_gport_t *l2gre_port_id){
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t l2_gre_port;

    bcm_l2gre_port_t_init(&l2_gre_port);

    l2_gre_port.criteria = BCM_L2GRE_PORT_MATCH_VPNID;    
    l2_gre_port.match_port = in_port;
    l2_gre_port.match_tunnel_id = in_tunnel;
    l2_gre_port.flags = flags;
    if(egress_if != 0) {
        l2_gre_port.egress_if = egress_if;
        l2_gre_port.flags |= BCM_L2GRE_PORT_EGRESS_TUNNEL;
    }
    if (out_tunnel != 0) {
        l2_gre_port.egress_tunnel_id = out_tunnel;
    }

    /* set orientation, work for Jericho.
     * Until arad+, we use bounce back filter to prevent packets from DC core to go back to DC core
     * For Jericho, we configure orientation at ingress (inLif) and egress (outLif): 
     * ingress orientation is configured at bcm_l2gre_port_add. 
     * egress orientation can be configured using bcm_l2gre_port_add or at bcm_port_class_set
     */
    if (is_device_or_above(unit, JERICHO)) {
        /* flag indicating network orientation.
           Used to configure ingress orientation and optionally egress orientation */
        l2_gre_port.flags |= BCM_L2GRE_PORT_NETWORK; 
        /* if no outlif is provided, can't configure outlif orientation */
        if (out_tunnel == 0){ 
            printf("can't configure outlif orientation, since tunnel initiator gport isn't provided "); 
        } else {
            /* set network orientation at the egress using bcm_port_class_set */
            if (!g_l2gre.set_egress_orientation_using_l2gre_port_add) {
                int frwrd_group = 1; 
                rv = bcm_port_class_set(unit, out_tunnel, bcmPortClassForwardEgress, frwrd_group); 
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_class_set \n");
                    return rv;
                }
                printf("egress orientation using bcm_port_class_set \n");

                /* since egress orientation is already updated, no need to pass outlif to bcm_l2gre_port_add to configure outlif orientation.
                 * Still need to pass outlif to l2gre_port_add if need to configure learning using outlif.
                 * If ip tunnel fec is provided, ip tunnel fec will be used for learning, so no need for outlif
                 */
                if (egress_if != 0) {
                    l2_gre_port.egress_tunnel_id = 0;
                }
            }
        }

    }
    
    rv = bcm_l2gre_port_add(unit,vpn,&l2_gre_port);
    if(rv != BCM_E_NONE) {
        printf("bcm_l2gre_port_add \n");
        return rv;
    }

    if(verbose >= 2){
        printf("added port in-tunnel:0x%08x ",in_tunnel);
        if(egress_if !=0 ) {
            printf("egress-intf:0x%08x ",egress_if);
        }
        else
        {
            printf("out-tunnel:0x%08x",out_tunnel);
        }
        printf("vpn::0x%08x\n\r",vpn);
    }

    /* update Multicast to have the added port  */
    rv = multicast_l2gre_port_add(unit, vpn, l2_gre_port.match_port , l2_gre_port.l2gre_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast_l2gre_port_add\n");
        return rv;
    }
    if(verbose >= 2){
        printf("add l2gre-port   0x%08x to multicast \n\r",l2_gre_port.l2gre_port_id);
    }

    *l2gre_port_id = l2_gre_port.l2gre_port_id;
    
    return rv;
}


/*
 * add access, 
 */
int
l2_gre_vlan_port_add(int unit,  bcm_gport_t in_port, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    
    /* the match criteria is port:1, out-vlan:510   */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp1.port = in_port;
    vp1.match_vlan = 510;
    /* when packet forwarded to this port then it will be set with out-vlan:100,  in-vlan:200 */
    vp1.egress_vlan = 510;
    vp1.flags = 0;
    /* this is relevant only when get the gport, not relevant for creation */
    /* will be pupulated when the gport is added to service, using vswitch_port_add */
    vp1.vsi = 0;
    rv = bcm_vlan_port_create(unit,&vp1);
    
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

    if(verbose >= 2) {
        printf("Add vlan-port-id:0x%08x in-port:0x%08x match_vlan:0x%08x match_inner_vlan:0x%08x\n\r",vp1.vlan_port_id, vp1.port, vp1.match_vlan, vp1.match_inner_vlan);
    }
    
    /* handle of the created gport */
    *port_id = vp1.vlan_port_id;
    
    return rv;
}


/******* Run example ******/
 
/*
 * IP tunnel example 
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 */
uint32 vpn = 4500;
bcm_gport_t out_tunnel_id = 0;
int l2_gre_example(int unit, int in_port, int out_port){

    uint32 gre_key_vpn=5000;
    uint32 tunnel_id_lif = 16384;
    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[2];
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */
    int eg_intf_ids[2];/* interface for routing, not used */
    bcm_gport_t vlan_port_id;
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
    bcm_gport_t l2gre_port_id[2];
    int l2_gre_protocol_type = 0xfeef;
    int rv;
    int l2_gre_port_flags = 0; 

    /* temporary fix until global lif support is ready */
    if (is_device_or_above(unit, JERICHO)) {
        tunnel_id_lif = tunnel_id_lif *2; 
    }


    /* init ip tunnel info, to fit GRE usage */
    ip_tunnel_glbl_init_gre(unit,0);

    /* init L2 Gre module */
    rv = bcm_l2gre_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

    rv = bcm_switch_control_set(unit,bcmSwitchL2GreProtocolType,l2_gre_protocol_type);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);

    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    /*** build tunnel initiators ***/
    out_tunnel_gports[0] = tunnel_id_lif; /* First tunnel In-Tunnel must be equals to Outer-Tunnel */
    rv = ipv4_tunnel_build_tunnels(unit, out_port, out_tunnel_intf_ids,out_tunnel_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
        printf("Open out-tunnel 2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

    BCM_GPORT_TUNNEL_ID_SET(out_tunnel_id, out_tunnel_intf_ids[1]);

    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }
    /*** build tunnel terminations and router interfaces ***/
    in_tunnel_gports[0] = tunnel_id_lif; /* In-Tunnel must be equals to Out-Tunnel */
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }
    if(verbose >= 2){
        printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[0]);
        printf("Open in-tunnel 2 gport-id: 0x%08x FEC-id\n\r", in_tunnel_gports[1]);
    }

    

    /* build L2 VPN */
    rv = l2_gre_open_vpn(unit,vpn, gre_key_vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_open_vpn, vpn=%d, \n", vpn);
    }

    /* build l2 gre ports */

    /* add l2gre port identified with in-tunnel and egress with tunnel id */
    rv = l2_gre_add_port(unit,vpn,in_port,in_tunnel_gports[0],out_tunnel_gports[0],0,l2_gre_port_flags,&l2gre_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_add_port, in_gport=0x%08x --> out_gport=0x%08x \n", in_tunnel_gports[0],out_tunnel_gports[0]);
    }

    /* add l2gre port identified with in-tunnel and egress with egress-object id */
    rv = l2_gre_add_port(unit,vpn,in_port,in_tunnel_gports[1],out_tunnel_gports[1],out_tunnel_intf_ids[1],l2_gre_port_flags,&l2gre_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_add_port 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

    /* port vlan port*/

    rv = l2_gre_vlan_port_add(unit, in_port, &vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n", vlan_port_id);
    }

    rv = vswitch_add_port(unit, vpn,out_port, vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_add_port\n");
        return rv;
    }


    /* add mact entries point to created gports */
    rv = l2_addr_add(unit,mac_address,vpn,vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to vlan port \n");
        return rv;
    }
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,l2gre_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to l2gre_port_id[0]\n");
        return rv;
    }
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,l2gre_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to l2gre_port_id[1]\n");
        return rv;
    }

    return rv;
}
int l2gre_port_get(
    int            unit,
    bcm_vpn_t      l2vpn,
    bcm_gport_t    l2gre_gport_id)
{
    int rv;
    

    bcm_l2gre_port_t l2gre_port;

    l2gre_port.l2gre_port_id  = l2gre_gport_id;

    rv = bcm_l2gre_port_get(
            unit, 
            l2vpn, 
            &l2gre_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2gre_port_get\n");
        return rv;
    }

    print l2gre_port ;

    return rv;
}

int 
l2gre_example_get_remove(int unit, int delete_all, int delete){
    int rv;
    bcm_l2gre_port_t port_array[3];
    int port1 = 0x7c004000;
    int indx;
    int port2 = 0x7c004001;
    int vpn = 4500;
    int port_count;

    printf("get port1 =0x%08x, \n", port1);
    l2gre_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    l2gre_port_get(unit, vpn, port2);


    /*   get all */
    rv = bcm_l2gre_port_get_all(
        unit, 
        vpn, 
        3, 
        port_array, 
        &port_count);
    printf("get all: num-of-ports =%d, \n", port_count);
    for(indx = 0; indx < port_count; ++indx) {
        print port_array[indx];
    }

    if(delete_all) {

        printf("delete all:\n");
        rv = bcm_l2gre_port_delete_all(unit, vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2gre_port_delete_all\n");
            return rv;
        }
    }
    else if(delete){
        printf("delete port1 =0x%08x, \n", port1);
        rv = bcm_l2gre_port_delete(unit, vpn, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_l2gre_port_delete\n");
            return rv;
        }

        printf("delete port2 =0x%08x, \n", port2);
        rv = bcm_l2gre_port_delete(unit, vpn, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_l2gre_port_delete\n");
            return rv;
        }
    }
    printf("get port1 =0x%08x, \n", port1);
    l2gre_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    l2gre_port_get(unit, vpn, port2);

    return rv;
}

/*********************
 * Add a IPMC route rule, so Some copies are forwarded to out_port.
 *
 */
int l2gre_ipv4_mc_route(int unit, int vlan, int* ipmc_index)
{
    int i;
    bcm_error_t rv = BCM_E_NONE;
    int vrf = 0;
   
    bcm_l3_intf_t intf1;
 
    rv = create_rif(mac_address_1, unit, vlan, &intf1, vrf);
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf1, 1); 
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, ipmc_index); 
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    return rv;
}

 

/*-----------------------------------------------------------------------------------------
 * Build qualifier set for the group we want to create.  Entries must exist
 * within groups, and groups define what kind of things the entries can
 * match, by their inclusion in the group's qualitier set.
 * 
 * The Field group is done at the ingress stage
 * The Field group qualifiers are EtherType (to match only IPv4 packets),
 * Build the action set for this Field group.  Entries can only do
 * something that is included on this list.
 * 
 * This Field Group is used to snoop matched packet to recycle port
*/
int l2gre_mc_field_group_set( int unit, int in_port, bcm_gport_t snoop_trap_gport_id, bcm_field_group_t *group) {
    int result;
    int auxRes;
    int group_priority = BCM_FIELD_GROUP_PRIO_ANY;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_group_t grp;
    bcm_field_entry_t ent;
    uint8 data, mask;

    printf("\t===============  PMF rule set   ==================\n");
    /* define the qualifier */
    BCM_FIELD_QSET_INIT(&qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyInPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpTunnelHit);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyForwardingType);

    /* define the action */
    BCM_FIELD_ASET_INIT(&aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionSnoop);

    result = bcm_field_group_create(unit, qset, group_priority, &grp);
    if ( result != BCM_E_NONE) {
      printf("Error in bcm_field_group_create\n");
      return result;
    }

    /*  Attach the action set */
    result = bcm_field_group_action_set(unit, grp, aset);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_group_action_set\n");
        return result;
    }

    /*
     * Add multiple entries to the Field group.
     * 
     * Match the packets with IP tunnel lookup hit and  
     * forward type is IPv4 Multicast Routing forwarding
     */
    result = bcm_field_entry_create(unit, grp, &ent);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_entry_create\n");
        return result;
    }

    result = bcm_field_qualify_InPort(unit, ent, in_port, 0xffffffff);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_InPort\n");
        return result;
    }

    /* VNI->VSI lookup is hit */
    data = 1;
    mask = 1;
    result = bcm_field_qualify_IpTunnelHit(unit, ent, data, mask);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_IpTunnelHit\n");
        return result;
    }

    result = bcm_field_qualify_ForwardingType(unit, ent, bcmFieldForwardingTypeIp4Mcast); 
    if (result != BCM_E_NONE) {
        printf("Error in bcm_field_qualify_HeaderFormat\n");
        return result;
    }

    /* Get the trap gport to snoop
     *  A matched frame is snooped to the defined gport.
     */
    result = bcm_field_action_add(unit, ent, bcmFieldActionSnoop, snoop_trap_gport_id, 0);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_action_add\n");
        return result;
    }

    /*
    *  Commit the entire group to hardware.  We could do individual entries,
    *  but since we just created the group here and want the entry/entries all
    *  in hardware at this point, it is quicker to commit the whole group.
    */
    result = bcm_field_group_install(unit, grp);
    if (BCM_E_NONE != result) {
        printf("Error in bcm_field_group_install\n");
        return result;
    }

    /*
    *  Everything went well; return the group ID that we allocated earlier.
    */
    *group = grp; 

    return result;
} 

/********************
 * snoop matched l2gre MC packet to recycle port
 *   1. create snoop command
 *   2. create a FG to qualify l2gre MC packet local to the node  
 */
int l2gre_mc_snoop2recycle_port(int unit, int in_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;
    bcm_field_group_t field;

    rv = two_pass_snoop_set(0, bcmRxTrapUserDefine, recycle_port, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, two_pass_snoop_set\n");
        return rv;
    }

    rv = l2gre_mc_field_group_set(0, in_port, gport, &field);
    if (rv != BCM_E_NONE) {
        printf("Error, l2gre_mc_field_group_set\n");
        return rv;
    }

    rv = bcm_port_control_set(0,recycle_port, bcmPortControlOverlayRecycle, 1);  
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        return rv;
    }

    return rv;
}

/****************************
 * L2GRE MC bud node example.
 *   Bud node some copies are IP-forwarded, some copies are IP-terminated;
 *   and L2-bridged according to the exposed DA.
 *
 * - build IPMC rule for IP-forwarded
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 * - create a FG to snoop matched l2gre MC to recycle port.
 * - build learn information
 * - create a FG to rebuild the Learn-Data according to the FEC-Pointer
 *
 */
int l2_gre_mc_bud_node_example(int unit, int in_port, int de_encap_out_port, int route_out_port, int recycle_port)
{
    int rv = BCM_E_NONE;
    bcm_gport_t l2gre_port_id;
    bcm_tunnel_terminator_t tunnel_term_2;
    int ip_tunnel_overlay_mc_lif = 0;
    bcm_gport_t ip_tunnel_overlay_mc_tunnel = 0;

    int vlan = 20;

    /* add port to vlan */
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, in_port);
    rv = bcm_vlan_port_add (unit, vlan, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_untagged_vlan_set\n");
        return rv;
    }

    int ipmc_index = 40960;
    rv = l2gre_ipv4_mc_route(unit, vlan, &ipmc_index);
    if (rv != BCM_E_NONE) {
        printf("Error, l2gre_ipv4_mc_route\n");
        return rv;
    }
    
	  /* add ports to mc group */    
    int port_member_of_mc[1] = {route_out_port}; 
    rv = add_ingress_multicast_forwarding(unit,ipmc_index, port_member_of_mc, 1, vlan); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }
    
    /* add routing table entry to mc group */
    bcm_ip_t mc_ip = 0xe0050505; 
    bcm_ip_t src_ip = 0;
    rv = create_forwarding_entry_dip_sip(unit, mc_ip, src_ip, ipmc_index, vlan); 
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip\n");
        return rv;
    }


    
    

    rv = l2_gre_example(unit, in_port, de_encap_out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_example\n");
        return rv;
    }

    /* If lookup{DIP} result is an IP-LIF, then the L2GRE header will be terminated.
     * In case of bud node and the traffic from recycle port, we always want to terminate
     * L2GRE header and forward with bridge, so we add an IP-LIF-dummy when bcm.user start. 
     * and here get the LIF-ID of IP-lIF-dummy and add it to VPN.
     */
    ip_tunnel_overlay_mc_lif = soc_property_get(unit , "default_logical_interface_ip_tunnel_overlay_mc",0);
    BCM_GPORT_TUNNEL_ID_SET(ip_tunnel_overlay_mc_tunnel, ip_tunnel_overlay_mc_lif);
    rv = l2_gre_add_port(unit,vpn, recycle_port, ip_tunnel_overlay_mc_tunnel, 0, 0, 0, &l2gre_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_add_port\n");
        return rv;
    }

    rv = l2gre_mc_snoop2recycle_port(unit, in_port, recycle_port);
    if (rv != BCM_E_NONE) {
        printf("Error, l2gre_mc_snoop2recycle_port\n");
        return rv;
    }

    /* SIP termination for l2gre:
     * the taget of this termination is mapping SIP to Learning info
     */
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.sip = 0xaa000011; 
    tunnel_term_2.sip_mask = 0xffffffff;
    tunnel_term_2.type = bcmTunnelTypeL2Gre;
    tunnel_term_2.tunnel_id = BCM_GPORT_TUNNEL_ID_GET(out_tunnel_id);
    rv = bcm_tunnel_terminator_create(0,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create\n");
        return rv;
    }
    /* Rebuild the Learn-Data according to the FEC-Pointer */
    rv = gre_learn_data_rebuild_example(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, gre_learn_data_rebuild_example\n");
        return rv;
    }

    return rv;
}
