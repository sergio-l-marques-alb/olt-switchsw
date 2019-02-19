/* $Id: cint_vxlan.c,v 1.10 Broadcom SDK $
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
 * how to run:
 *
 
soc preoperties:
#enable VXLAN according to SIP/DIP 
#0:none 1:dip_sip termination 2: dip_termination 3: both
bcm886xx_vxlan_enable=1
#1:seperated in SEM 2:for joined in TCAM
bcm886xx_vxlan_tunnel_lookup_mode=1 
#disable conflicting features 
bcm886xx_ip4_tunnel_termination_mode=0
bcm886xx_l2gre_enable=0
bcm886xx_ether_ip_enable=0
 
 
 
cint;
cint_reset();
exit; 
cint ../../../../src/examples/dpp/utility/cint_utils_global.c    
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel.c 
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c 
cint ../../../../src/examples/dpp/cint_port_tpid.c 
cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
cint ../../../../src/examples/dpp/cint_mact.c
cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c  
cint ../../../../src/examples/dpp/cint_vxlan.c 
cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c 
cint
verbose = 3;
int rv; 
print vxlan_example(0,13,13,-1); 
 
//this to show how to remove/get entries 
print vxlan_example_get_remove (0,1,0,-1); 
 

 
diag: 
dip lookup
ppd_api diag db_lif_lkup_info_get lkup_num 0 bank_id 1 
sip lookup
ppd_api diag db_lif_lkup_info_get lkup_num 1 bank_id 0 

*/ 

/* ********* 
  Globals/Aux Variables
 ********** */
/* debug prints */
int verbose = 1;


struct vxlan_s {
    int vpn_id;  
    int vni; 
    int set_egress_orientation_using_vxlan_port_add; /* egress orientation can be configured: 
                                                        - using the api bcm_vxlan_port_add, 
                                                           when egress_tunnel_id (ip tunnel initiator gport, contains outlif) is valid (optional).  
                                                        - using the api bcm_port_class_set, update egress orientation. */
};

/*                 vpn_id | vni |       */
vxlan_s g_vxlan = {4500,    5000, 
/*                 set_egress_orientation_using_vxlan_port_add   */
                   1
};

void vxlan_init(vxlan_s *param) {
    if (param != NULL) {
        sal_memcpy(&g_vxlan, param, sizeof(g_vxlan));
    }
}

void vxlan_struct_get(vxlan_s *param) {
    sal_memcpy( param, &g_vxlan, sizeof(g_vxlan));
}


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
        printf("Error, in trap create, trap %d \n", trap_id);
        return rv;
    }

    rv = bcm_rx_trap_set(unit,trap_id,&config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set \n");
        return rv;
    }

    return rv;
}


bcm_mac_t vxlan_my_mac_get() {
    return ip_tunnel_my_mac_get();
}

/*
 * struct include meta information. 
 * where the cint logic pull data from this struct. 
 * use to control differ runs of the cint, without changing the cint code
 */

void ip_tunnel_glbl_init_vxlan(int unit, int flags){

    /* update tunnel info */
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeVxlan;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeVxlan;
}


int vxlan_learn_sip_on_fec(int unit, uint32 sip, uint32 fec){
	int rv;
	bcm_tunnel_terminator_t sip_term;
	sip_term.sip_mask = 0xffffffff;
	sip_term.type = bcmTunnelTypeVxlan;
	sip_term.sip = sip;          
	sip_term.tunnel_id = fec;

	rv = bcm_tunnel_terminator_create(0,&sip_term);
	if (rv != BCM_E_NONE) {
		printf("Error, bcm_tunnel_terminator_create\n");
		return rv;
	}

	if(verbose >= 1){
		printf("learn  0x%08x with FEC:%d\n\r",sip,fec);
	}
	return rv;
}

int vxlan_open_vpn(int unit, int vpn, int vni){
    int rv = BCM_E_NONE;
 
    egress_mc = 0;
    rv = multicast__open_mc_group(unit, &vpn, BCM_MULTICAST_TYPE_L2);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group\n");
        return rv;
    }
    if(verbose >= 1){
        printf("created multicast   0x%08x\n\r",vpn);
    }

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);

    vpn_config.flags = BCM_VXLAN_VPN_ELAN|BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vnid = vni;

    rv = bcm_vxlan_vpn_create(unit,&vpn_config);
    if(rv != BCM_E_NONE) {
        printf("vxlan_open_vpn: error in bcm_vxlan_vpn_create \n");
        return rv;
    }
    if(verbose >= 1){
        printf("vxlan_open_vpn: created vpn   0x%08x\n\r",vpn);
    }

    return rv;
}


/*
 * add gport of type vlan-port to the multicast
 */
int multicast_vxlan_port_add(int unit, int mc_group_id, int sys_port, int gport){

    int encap_id;
    int rv;
    
    rv = bcm_multicast_vxlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vxlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
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
 * Add vxlan port to vpn 
 * parameters:
 * in_port: network side port for learning info. 
 * in_tunnel: ip tunnel termination gport
 * out_tunnel: ip tunnel creation gport 
 * egress_if: FEC entry, contains ip tunnel encapsulation information
 * flags: vxlan flags
 * vxlan_port_id: returned vxlan gport
 */
int vxlan_add_port(int unit, uint32 vpn, bcm_gport_t in_port, bcm_gport_t in_tunnel, bcm_gport_t out_tunnel, bcm_if_t egress_if, uint32 flags, bcm_gport_t *vxlan_port_id){
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t vxlan_port;

    bcm_vxlan_port_t_init(&vxlan_port);

    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = in_port;
    vxlan_port.match_tunnel_id = in_tunnel;
    vxlan_port.flags = flags;
    if(egress_if != 0) {
        vxlan_port.egress_if = egress_if;
        vxlan_port.flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
    }
    if (out_tunnel != 0){
        vxlan_port.egress_tunnel_id = out_tunnel;
    }

    /* set orientation, work for Jericho.
     * Until arad+, we use bounce back filter to prevent packets from DC core to go back to DC core.
     * For Jericho, we configure orientation at ingress (inLif) and egress (outLif): 
     * ingress orientation is configured at bcm_vxlan_port_add. 
     * egress orientation can be configured using bcm_vxlan_port_add or at bcm_port_class_set
     */
    if (is_device_or_above(unit, JERICHO)) {
        /* flag indicating network orientation.
           Used to configure ingress orientation and optionally egress orientation */
        vxlan_port.flags |= BCM_VXLAN_PORT_NETWORK; 
        /* if no outlif is provided, can't configure egress orientation */
        if (out_tunnel == 0){ 
            printf("can't configure outlif orientation, since tunnel initiator gport isn't provided "); 
        } else {
            /* set network orientation at the egress using bcm_port_class_set */
            if (!g_vxlan.set_egress_orientation_using_vxlan_port_add) {
                int frwrd_group = 1; 
                rv = bcm_port_class_set(unit, out_tunnel, bcmPortClassForwardEgress, frwrd_group); 
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_class_set \n");
                    return rv;
                }
                printf("egress orientation using bcm_port_class_set \n");

                /* since egress orientation is already updated, no need to pass outlif to bcm_vxlan_port_add to configure outlif orientation.
                 * Still need to pass outlif to vxlan_port_add if need to configure learning using outlif.
                 * If ip tunnel fec is provided, ip tunnel fec will be used for learning, so no need for outlif
                 */
                if (egress_if != 0) {
                    vxlan_port.egress_tunnel_id = 0;
                }
            }
        }
    }  
    rv = bcm_vxlan_port_add(unit,vpn,&vxlan_port);
    if(rv != BCM_E_NONE) {
        printf("error bcm_vxlan_port_add \n");
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
    rv = multicast_vxlan_port_add(unit, vpn, vxlan_port.match_port , vxlan_port.vxlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast_vxlan_port_add\n");
        return rv;
    }
    if(verbose >= 2){
        printf("add vxlan-port   0x%08x to multicast \n\r",vxlan_port.vxlan_port_id);
    }

    *vxlan_port_id = vxlan_port.vxlan_port_id;
    
    return rv;
}


/*
 * add access, 
 */
int
vxlan_vlan_port_add(int unit,  bcm_gport_t in_port, bcm_gport_t *port_id){
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

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp1);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_vlan_port_create_to_translation\n");
            return rv;
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
 */
int vxlan_example(int unit, int in_port, int out_port, int vpn_id){

    uint32 vpn;

    /* init vxlan global */
    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    if (vpn_id >=0) {
        vxlan_param.vpn_id = vpn_id;
    }
    vxlan_init(vxlan_param);
    vpn = g_vxlan.vpn_id;


    printf("Parameters \n");
    printf("inPort: %d \n", in_port);
    printf("outPort: %d \n", out_port);
    printf("vpn: %d \n", vpn_id);


    
    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[2];
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */
    int eg_intf_ids[2];/* interface for routing, not used */
    bcm_gport_t vlan_port_id;
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x00, 0x00, 0xf0};
    bcm_gport_t vxlan_port_id[2];
	uint32 tunnel_sip = 0x0a050001;
	uint32 sip_learned_fec = 0x1234;
    int vxlan_protocol_type = 0xfeef;
    int rv;
    int vxlan_port_flags = 0;

    verbose = 10;

    bcm_ip_t native_dip = 0xABA1A1A1; /* ip address: 171:17:17:17 */ 

    /* init ip tunnel info, to fit VXLAN usage */
    ip_tunnel_glbl_init_vxlan(unit,0);

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

	/* work around to learn SIP lookup result as FEC  */
	gre_learn_data_rebuild_example(unit); 

	rv = vxlan_learn_sip_on_fec(unit,tunnel_sip,sip_learned_fec);
	if (rv != BCM_E_NONE) {
		printf("Error, vxlan_learn_sip_on_fec\n");
		return rv;
	}

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);


    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    /*** build tunnel initiators ***/
    rv = ipv4_tunnel_build_tunnels(unit, in_port, out_tunnel_intf_ids,out_tunnel_gports);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }
    if(verbose >= 2){
        printf("Open out-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", out_tunnel_gports[0]);
        printf("Open out-tunnel 2 gport-id: 0x%08x FEC-id:0x%08x\n\r", out_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }

    if(verbose >= 2){
        printf("Open tunnels tunnel terminators: \n\r");
    }
    /*** build tunnel terminations and router interfaces ***/
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }
    if(verbose >= 2){
        printf("Open in-tunnel 1 gport-id: 0x%08x (No FEC)\n\r", in_tunnel_gports[0]);
        printf("Open in-tunnel 2 gport-id: 0x%08x FEC-id\n\r", in_tunnel_gports[1]);
    }


    /* build L2 VPN */

    rv = vxlan_open_vpn(unit,vpn,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_open_vpn, vpn=%d, \n", vpn);
    }


    /* build l2 vxlan ports */

    /* add vxlan port identified with in-tunnel and egress with tunnel id 
    rv = vxlan_add_port(unit,vpn,in_port,in_tunnel_gports[0],out_tunnel_gports[0],0, &vxlan_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_add_port, in_gport=0x%08x --> out_gport=0x%08x \n", in_tunnel_gports[0],out_tunnel_gports[0]);
    }*/

    /* add vxlan port identified with in-tunnel and egress with egress-object id */

    rv = vxlan_add_port(unit,vpn,in_port,in_tunnel_gports[1],out_tunnel_gports[1],out_tunnel_intf_ids[1],vxlan_port_flags,&vxlan_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_add_port 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
    }
    if (verbose >=2 ) {
        printf("vxlan port created, vxlan_port_id: 0x%08x \n", vxlan_port_id[1]); 
    }

    /* port vlan port*/

    /* init vlan port glbl info */
    vswitch_metro_mp_info_init(out_port,0,0);

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        port_tpid_init(in_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }

        port_tpid_init(out_port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, vlan_translation_default_mode_init\n");
            print rv;
            return rv;
        }
    }



                                                                                    
    rv = vxlan_vlan_port_add(unit, in_port, &vlan_port_id);                           
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

    rv = l2_addr_add(unit,mac_address,vpn,vxlan_port_id[0]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to vxlan_port_id[0]\n");
        return rv;
    }
    mac_inc(mac_address);

    rv = l2_addr_add(unit,mac_address,vpn,vxlan_port_id[1]);
    if (rv != BCM_E_NONE) {
        printf("Error, l2_addr_add to vxlan_port_id[1]\n");
        return rv;
    }
    return rv;
}


int vxlan_port_get(
    int            unit,
    bcm_vpn_t      l2vpn,
    bcm_gport_t    vxlan_gport_id)
{
    int rv;
    

    bcm_vxlan_port_t vxlan_port;

    vxlan_port.vxlan_port_id  = vxlan_gport_id;

    rv = bcm_vxlan_port_get(
            unit, 
            l2vpn, 
            &vxlan_port);

    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vxlan_port_get\n");
        return rv;
    }

    print vxlan_port ;

    return rv;
}

int 
vxlan_example_get_remove(int unit, int delete_all, int delete, int vpn_id){
    int rv;
    bcm_vxlan_port_t port_array[3];
    int port1 = 0x7c001000;
    int indx;
    int port2 = 0x7c001001;
    int vpn;
    int port_count;

    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    if (vpn_id >=0) {
        vxlan_param.vpn_id = vpn_id;
    }
    vxlan_init(vxlan_param);

    vpn = g_vxlan.vpn_id;

    printf("get port1 =0x%08x, \n", port1);
    vxlan_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    vxlan_port_get(unit, vpn, port2);


    /*   get all */
    rv = bcm_vxlan_port_get_all(
        unit, 
        vpn, 
        3, 
        port_array, 
        &port_count);
    printf("get all: num-of-ports =%d, \n", port_count);
    for(indx = 0; indx < port_count; ++indx) {
        print port_array[indx];
    }

    if (port_count != 2) {
        printf("In this CINT script, should have 2 ports\n");
        return BCM_E_UNAVAIL;
    }

    port1 = port_array[0].vxlan_port_id;
    port2 = port_array[1].vxlan_port_id;

    if(delete_all) {

        printf("delete all:\n");
        rv = bcm_vxlan_port_delete_all(unit, vpn);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vxlan_port_delete_all\n");
            return rv;
        }
    }
    else if(delete){
        printf("delete port1 =0x%08x, \n", port1);
        rv = bcm_vxlan_port_delete(unit, vpn, port1);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_vxlan_port_delete\n");
            return rv;
        }

        printf("delete port2 =0x%08x, \n", port2);
        rv = bcm_vxlan_port_delete(unit, vpn, port2);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_petra_vxlan_port_delete\n");
            return rv;
        }
    }
    printf("get port1 =0x%08x, \n", port1);
    vxlan_port_get(unit, vpn, port1);
    printf("get port2 =0x%08x, \n", port2);
    vxlan_port_get(unit, vpn, port2);

    return rv;
}
