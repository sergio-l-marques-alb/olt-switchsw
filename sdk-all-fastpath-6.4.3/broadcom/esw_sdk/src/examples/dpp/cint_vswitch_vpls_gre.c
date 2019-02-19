/* $Id: cint_vswitch_vpls_gre.c,v 1.5 Broadcom SDK $
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
 * Purpose: Example of VPLS over GRE.
 * 
 * The following soc properties should be set:
 * bcm886xx_l2gre_enable=1
 * bcm886xx_ip4_tunnel_termination_mode=0
 *
 * Additionally, a recycle port must be set up to use this cint.
 * The recycle port is passed to the vpls_l2_gre_example.
 *
 * how to run:
 *
 
 cint;
 cint_reset();
 exit;
 cint ../../../../src/examples/dpp/cint_ip_route.c
 cint ../../../../src/examples/dpp/cint_ip_tunnel.c
 cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
 cint ../../../../src/examples/dpp/cint_port_tpid.c
 cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 cint ../../../../src/examples/dpp/cint_mact.c
 cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c
 cint ../../../../src/examples/dpp/cint_multi_device_utils.c
 cint ../../../../src/examples/dpp/cint_vswitch_vpls_gre.c
 cint ../../../../src/examples/dpp/cint_field_vpls_gre_wa.c
 
 cint
 verbose = 3;
 int rv;
 
 print vpls_l2_gre_example(0, CORE_PORT (e.g. 200), ACCESS_PORT (e.g. 13), RECYCLE_PORT (e.g. 40));
 rv = cint_field_vpls_gre_wa_setup(0,1,1);
 
 exit;
 
 debug +rx
 
 * 
 * Core side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |   Ethernet (+VLAN):
 *   |   --------------------------------------------------------------------
 *   |   |        DA         |        SA         | TIPD1  | Prio | VID | ...
 *   |   | 00:0C:00:02:00:00 | 00:00:07:00:01:00 | 0x8100 |      | 10  | ...
 *   |   --------------------------------------------------------------------
 *   |
 *   |   IPv4:
 *   |   -------------------------------------------------
 *   |    ...  |     SA     |     DA     | Protocol | ... 
 *   |    ...  | 160.0.0.17 | 161.0.0.17 |   GRE    | ...
 *   |   -------------------------------------------------
 *   |
 *   |   GRE:
 *   |   -------------------------------------
 *   |    ...  | Key-Present | Protocol | ...
 *   |    ...  |      1      |  0x8847  | ...
 *   |   -------------------------------------
 *   |
 *   |   MPLS:
 *   |   -------------------
 *   |    ... | Label | ...
 *   |    ... |  100  | ...
 *   |   -------------------
 *   |
 *   |   Ethernet:
 *   |   -----------------------------------------
 *   |    ... |        DA         | <Contents...>
 *   |    ... | 00:00:00:00:00:F0 | <Contents...>
 *   |   -----------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * Access side packets:
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |   Ethernet (+VLAN):
 *   |   ----------------------------------------------------------
 *   |   |        DA         | TIPD1  | Prio | VID | <Contents...>
 *   |   | 00:00:00:00:00:F1 | 0x8100 |      | 510 | <Contents...>
 *   |   ----------------------------------------------------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/ 

/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;

/********** 
  functions
 ********** */

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

int l2_gre_add_port(int unit, uint32 vpn, bcm_gport_t core_port, bcm_gport_t in_tunnel, bcm_gport_t out_tunnel, bcm_if_t egress_if, bcm_gport_t *l2gre_port_id){
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t l2_gre_port;
    bcm_l2gre_port_t_init(&l2_gre_port);

    l2_gre_port.criteria = BCM_L2GRE_PORT_MATCH_VPNID;    
    l2_gre_port.match_port = core_port;
    l2_gre_port.match_tunnel_id = in_tunnel;
    if(egress_if != 0) {
        l2_gre_port.egress_if = egress_if;
        l2_gre_port.flags |= BCM_L2GRE_PORT_EGRESS_TUNNEL;
    }
    else{
        l2_gre_port.egress_tunnel_id = out_tunnel;
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
l2_gre_vlan_port_add(int unit,  bcm_gport_t core_port, bcm_gport_t *port_id){
    int rv;
    bcm_vlan_port_t vp1;
    bcm_vlan_port_t_init(&vp1);
    
    /* the match criteria is port:1, out-vlan:510   */
    vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp1.port = core_port;
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

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
   action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
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
 * core_port = Core 
 * access_port = Access 
 */
int vpls_l2_gre_example(int unit, int core_port, int access_port, int recycle_port)
{
    uint32 vpn=4500;
    uint32 gre_key_vpn=5000;
    uint32 tunnel_id_lif = 16384;
	int pwe_label = 100;
	int terminated_rif = 100;
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
    int l2_gre_protocol_type = 0x8847;
    int rv;
	int egress_intf_dummy;
	bcm_mpls_port_t mpls_port_1;
    int mpls_termination_label_index_enable;


    /* init ip tunnel info, to fit GRE usage */
    ip_tunnel_glbl_init_gre(unit,0);

    /* init L2 Gre module */
    rv = bcm_l2gre_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2gre_init \n");
    }

    rv = bcm_switch_control_set(unit,bcmSwitchL2GreProtocolType,l2_gre_protocol_type);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    /*** build tunnel initiators ***/
    out_tunnel_gports[0] = tunnel_id_lif; /* First tunnel In-Tunnel must be equals to Outer-Tunnel */
    rv = ipv4_tunnel_build_tunnels(unit, core_port, out_tunnel_intf_ids,out_tunnel_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, core_port=%d, \n", core_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
        printf("Open out-tunnel 2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }
    /*** build tunnel terminations and router interfaces ***/
    in_tunnel_gports[0] = tunnel_id_lif; /* In-Tunnel must be equals to Out-Tunnel */
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,core_port,core_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, core_port=%d, \n", core_port);
    }
    if(verbose >= 2){
        printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[0]);
        printf("Open in-tunnel 2 gport-id: 0x%08x FEC-id\n\r", in_tunnel_gports[1]);
    }   

    /* build L2 VPN */
    rv = l2_gre_open_vpn(unit,vpn,gre_key_vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_open_vpn, vpn=%d, \n", vpn);
    }

    /* build l2 gre ports */

    /* add l2gre port identified with in-tunnel and egress with tunnel id */
    rv = l2_gre_add_port(unit,vpn,core_port,in_tunnel_gports[0],out_tunnel_gports[0],0, &l2gre_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_add_port, in_gport=0x%08x --> out_gport=0x%08x \n", in_tunnel_gports[0],out_tunnel_gports[0]);
    }

    /* add l2gre port identified with in-tunnel and egress with egress-object id */
    rv = l2_gre_add_port(unit,vpn,core_port,in_tunnel_gports[1],0,out_tunnel_intf_ids[1],&l2gre_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_gre_add_port 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

	port_tpid_init(access_port,1,1);
	rv = port_tpid_set(unit);
	if (rv != BCM_E_NONE) {
		printf("Error, port_tpid_set\n");
		print rv;
		return rv;
	}

	port_tpid_init(recycle_port,1,1);
	rv = port_tpid_set(unit);
	if (rv != BCM_E_NONE) {
		printf("Error, port_tpid_set\n");
		print rv;
		return rv;
	}

	port_tpid_init(core_port,1,1);
	rv = port_tpid_set(unit);
	if (rv != BCM_E_NONE) {
		printf("Error, port_tpid_set\n");
		print rv;
		return rv;
	}

	/* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
	if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    /* port vlan port*/
    rv = l2_gre_vlan_port_add(unit, access_port, &vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vswitch_metro_add_port_1 2, in_intf=0x%08x \n", vlan_port_id);
    }

    rv = vswitch_add_port(unit, vpn,access_port, vlan_port_id);
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
	
	/* VPLS settings */
	/* 1st pass LSR (Core to Access). Send to recycle port */
	rv = mpls_dummy_next_hop(unit, recycle_port, terminated_rif, &egress_intf_dummy);
	if (rv != BCM_E_NONE) {
        printf("Error, mpls_dummy_next_hop\n");
        return rv;
    }

	rv = mpls_add_switch_entry(unit, pwe_label, egress_intf_dummy);
	if (rv != BCM_E_NONE) {
        printf("Error, mpls_add_switch_entry\n");
        return rv;
    }

	/* Create MPLS port */	
	/* add port, according to VC label */
    bcm_mpls_port_t_init(&mpls_port_1);
    
    /* set port attribures, key <BC>*/
    mpls_port_1.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port_1.match_label = pwe_label;
    mpls_port_1.egress_tunnel_if = out_tunnel_intf_ids[1]; /* IP tunnels FEC */
    mpls_port_1.flags = BCM_MPLS_PORT_NETWORK|BCM_MPLS_PORT_EGRESS_TUNNEL;   
    mpls_port_1.port = core_port;
    mpls_port_1.egress_label.label = pwe_label;

    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(&mpls_port_1.match_label,pwe_label,2);
    }
    
    rv = bcm_mpls_port_add(unit, vpn, &mpls_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }

	mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,mpls_port_1.mpls_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to l2gre_port_id[0]\n");
        return rv;
    }

    return rv;
}

int mpls_add_switch_entry(int unit, int pwe_label,  bcm_if_t egress_intf)
{
    int rv;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* incomming label */
    entry.label = pwe_label;
    
    /* egress attribures FEC */
	entry.egress_if = egress_intf;
    
	rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}

/* create egress object */
/* Create two dummy rifs. The src RIF gets an ETHoMPLS
   packet and routes it to the dst RIF using the recycle
   port. The dst RIF terminates the MPLS tunnel and forwards
   the Ethernet packet.
 
  --------------------------------------------------
  | |         -------                     Device   |
  | =========>| src |===========\\                 |
  | ETHoMPLS  -------           ||                 |
  |                             ||  ETHoMPLSoETH   |
  |           -------           || (MPLS Routing)  |
  | <=========| dst |<==========//                 |
  |    ETH    -------                              |
  |            MPLS                                |
  |         Termination                            |
  --------------------------------------------------
 
  Remarks:
  ========
  1. The two dummy RIFs are important, since if only one existed
  then the SA and DA of the packet would be equal, and a trap
  would be triggered.
 
  2. The dst RIF VLAN is (rif), while the src RIF VLAN is (rif+1).
  This is important since there can be only one RIF per vlan.
 
  3. To send a packet from one RIF to the other the VLAN must be
  changed. To do this, VLAN editing is used. If a packet with
  VLAN (rif+1) is sent on the recycle port (egress), then the vlan is
  changed to rif.
 */
int mpls_dummy_next_hop(int unit, int recycle_port, int rif, int* egress_intf)
{
    int encap_id = 0;
    int flags = 0;

    /* 
     * Create two RIFs - one to send from one to receive on. 
     * This allows to use two different mac addrs when sending,
     * so the sa=da trap is avoided. 
     *  
     * mymac_src - mymac of the rif we send the packet from
     * mymac_dst - mymac of the rif we receive the packet from
     */
    uint8 mymac_src[6] = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x05 };
    uint8 mymac_dst[6] = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x06 };
    int dst_rif = rif;
    int src_rif = rif + 1;
    int ingress_intf_src;
    int ingress_intf_dst;
    int rv;

    /* Have new VLAN-domain for the recycle port */
    rv = bcm_port_class_set(unit, recycle_port, bcmPortClassId, recycle_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_class_set\n");
        return rv;
    }

    rv = create_l3_intf(unit, flags, 1, recycle_port, src_rif, 0, mymac_src, &ingress_intf_src);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf\n");
        return rv;
    }

    rv = create_l3_intf(unit, flags, 1, recycle_port, dst_rif, 0, mymac_dst, &ingress_intf_dst);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf\n");
        return rv;
    }

    rv = create_l3_egress(unit, 0, recycle_port, src_rif, ingress_intf_src, mymac_dst, egress_intf, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress\n");
        return rv;
    }
    /* When we get an egress vlan of src_rif, we edit it to be dst_rif */
    if (advanced_vlan_translation_mode) {

        bcm_gport_t lif;
        rv = vlan_port_create(unit, recycle_port, &lif, src_rif, BCM_VLAN_PORT_MATCH_PORT_VLAN);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create\n");
            return rv;
        }
        printf("rv = vlan_port_translation_set(%d, %d, %d, 0x%x, EGRESS_REMOVE_TAGS_PUSH_1_PROFILE, 0);\n", unit, dst_rif, dst_rif, *egress_intf);
        rv = vlan_port_translation_set(unit, dst_rif, dst_rif, lif, EGRESS_REMOVE_TAGS_PUSH_1_PROFILE, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_translation_set\n");
            return rv;
        }
    } else {
        bcm_vlan_action_set_t action;
        bcm_vlan_action_set_t_init(&action);
        action.new_outer_vlan = dst_rif;
        action.ut_outer = bcmVlanActionAdd;
        rv = bcm_vlan_translate_egress_action_add(unit, recycle_port, src_rif, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_translate_action_create\n");
            return rv;
        }
    }

    return rv;
}

