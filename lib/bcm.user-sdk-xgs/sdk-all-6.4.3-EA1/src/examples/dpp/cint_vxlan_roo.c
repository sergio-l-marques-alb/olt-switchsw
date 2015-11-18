/* $Id: cint_vxlan_roo.c,v 1.10 Broadcom SDK $
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
 * 
 * 
 * **************************************************************************************************************************************************
 * 
 *  
 * Network diagram
 * 
 * We configure Router A
 *  
 *  
 *          
 *                               _________  
 *                              | ROUTER B|------host1
 *                              |_________|             
 *                                |                           
 *     ___________________________|_______________________________
 *    |                   |         |                             |
 *    |                   | ROUTER A| (intra DC IP)    DC FABRIC  |
 *    |                   |_________|                             |
 *    |                     /       \                             | 
 *    |            _______/___        \                           |
 *    |           |          |          \                         |
 *    |           | ROUTER C |            \                       |
 *    |           |__________|              \                     | 
 *    |            /                          \                   | 
 *    |         /                               \                 | 
 *    |_____ /____________________________________\_______________|
 *     ____/___                                _____\__
 *    |  TOR1 |                               |  TOR2 |                      
 *    |_______|                               |_______|                      
 *    |       |                               |       | 
 *    |       |                               |       | 
 *    |_______|                               |_______| 
 *    | VM:A0 |                               |       | 
 *    |_______|                               |_______| 
 * 
 * 
 * Configuration:
 * 
 * soc properties:                                                        
 * #enable VXLAN according to SIP/DIP                                     
 * #0:none 1:dip_sip termination 2: dip_termination 3: both               
 * bcm886xx_vxlan_enable=1                                                
 * #1:seperated in SEM 2:for joined in TCAM                               
 * bcm886xx_vxlan_tunnel_lookup_mode=1                                    
 * #disable conflicting features                                          
 * bcm886xx_ip4_tunnel_termination_mode=0                                 
 * bcm886xx_l2gre_enable=0                                                
 * bcm886xx_ether_ip_enable=0                                             
 * bcm886xx_auxiliary_table_mode=1                                        
 * #enable ROO for vxlan                                                  
 * bcm886xx_roo_enable=1                                                  
 *                                                                        
 * cint;                                                                  
 * cint_reset();                                                          
 * exit;                                                                  
 *                                                                        
 * cint ../../../../src/examples/dpp/cint_ip_route.c                      
 * cint ../../../../src/examples/dpp/cint_ip_tunnel.c                     
 * cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c                
 * cint ../../../../src/examples/dpp/cint_port_tpid.c                     
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dpp/cint_mact.c                          
 * cint ../../../../src/examples/dpp/cint_vswitch_metro_mp.c              
 * cint ../../../../src/examples/dpp/cint_vxlan.c                         
 * cint ../../../../src/examples/dpp/cint_field_gre_learn_data_rebuild.c  
 * cint                                                                   
 * int unit=0;                                                            
 * vxlan_roo_run(unit, in_port, out_port, vpn_id);                            
 * 
 * 
 *  Traffic from RouterB to VM:A0
 * 
 *  Routing to overlay: host1_mac to VM:A0
 *  Purpose: - check lookup in host table result as vxlan encapsulation and native ethernet encapsulation
 * 
 *    Send:                                           
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerA_mac | routerB_mac |  v1  |  
 *             ------------------------------------
 * 
 *                 ------------------------ 
 *             ip: |   SIP     | DIP      | 
 *                 ------------------------ 
 *                 | host10_ip |  vmA0_ip | 
 *                 ------------------------ 
 *    Receive:
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerC_mac | routerA_mac |  v2  |  
 *             ------------------------------------
 * 
 *                 ----------------------- 
 *             ip: |   SIP     | DIP     | 
 *                 ----------------------- 
 *                 | routerA_ip| tor1_ip | 
 *                 ----------------------- 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ------------------------------------ 
 *                     native eth: |    DA       |     SA      | VLAN | 
 *                                 ------------------------------------ 
 *                                 | tor1_mac    | routerA_mac |  v3  | 
 *                                 ------------------------------------ 
 *                                                                      
 *                                     ----------------------         
 *                         native ip:  |   SIP     | DIP     |        
 *                                     ----------------------        
 *                                     | host10_ip | vmA0_ip |        
 *                                     ----------------------
 *
 *
 * routerA_mac: native DA/SA and ip_tunnel's SA/DA 00:0c:00:02:00:00 see ip_tunnel_my_mac_get from cint_ip_tunnel.c  
 * routerB_mac: native SA/DA: 00:00:00:00:05:81   
 * v1:          native vlan: 15                
 * host10_ip:   native SIP/DIP: 160:17:17:17       
 * vmA0_ip:     native DIP/SIP: 171:17:17:17 
 * 
 * routerC_mac: ip tunnel's DA/SA: 20:00:00:00:cd:1d next_hop_mac2 from cint_ip_tunnel.c 
 * routerA_mac: ip tunnel's SA/DA: 00:0c:00:02:00:00 ip_tunnel_my_mac_get from cint_ip_tunnel.c
 * v2:          ip tunnel's vid: 100 out_vlan from cint_ip_tunnel.c
 * routerA_ip:  ip tunnel's SIP/DIP. 170.0.0.17 See sip2 from cint_ip_tunnel.c
 * tor1_ip:     ip tunnel's DIP/SIP. 171.0.0.17 See dip2 from cint_ip_tunnel.c
 * vni:         vxlan header's vni: 5000 vxlan_key_vni from cint_vxlan.c
   v3:          native vlan: 20 
 * 
 * overlay to routing: VM:A0 to host1_mac 
 * Purpose: - check overlay termination and native ethernet termination.
 *          - check the packet is routed.
 *          - Encapsulated with native ethernet.
 * 
 *    Send:
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerA_mac | routerC_mac |  v2  |  
 *             ------------------------------------
 * 
 *                 ----------------------- 
 *             ip: |   SIP   | DIP       | 
 *                 ----------------------- 
 *                 | tor1_ip | routerA_ip| 
 *                 ----------------------- 
 * 
 *                udp:
 * 
 *                   vxlan:  ---------
 *                           |  VNI  |
 *                           ---------
 *                           |  vni  |
 *                           ---------
 *    
 *                                 ---------------------------------
 *                     native eth: |    DA       |     SA   | VLAN | 
 *                                 ---------------------------------
 *                                 | routerA_mac | tor1_mac |  v3  | 
 *                                 ---------------------------------
 *                                                                      
 *                                     ----------------------         
 *                         native ip:  |   SIP   |   DIP     |        
 *                                     ----------------------        
 *                                     | vmA0_ip | host10_ip |        
 *                                     ----------------------
 * 
 *   
 *    Receive:                                           
 *             ------------------------------------   
 *        eth: |    DA       |     SA      | VLAN |
 *             ------------------------------------
 *             | routerB_mac | routerA_mac |  v1  |  
 *             ------------------------------------
 * 
 *                 ------------------------ 
 *             ip: |   SIP   |   DIP      | 
 *                 ------------------------ 
 *                 | vmA0_ip |  host10_ip | 
 *                 ------------------------  
 * 
 * 
 */


/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization  START     -----------------
 **************************************************************************************************** */

struct vxlan_roo_s {
    int native_dip; 
    int native_out_vlan; 
    bcm_mac_t native_da; 

    /* overlay arp/LL */
    int overlay_ll_outlif; /* outlif (or overlay arp eedb index) */ 
};

vxlan_roo_s g_vxlan_roo = 
/* native headers: 
 native dip: 171:17:17:17 | native vlan | native da                      */
{0xABA1A1A1,                20,           ip_tunnel_glbl_info.tunnel_2.sa, 
/* overlay headers:
 overlay ll outlif */
 0
};


void vxlan_roo_struct_get(vxlan_roo_s *param) {
    sal_memcpy( param, &g_vxlan_roo, sizeof(g_vxlan_roo));
}



/*
 * Function create L3 intf (see cint_ip_route.c) for original.
 * Use this version only if:
 * you don't want to add port to vlan
 * 
 * otherwise use: create_l3_intf from cint_ip_route.c
 * 
 * Description:
 * the parameter add_gport_to_vlan allows to skip "add port to vlan"
 * 
 * For more details on parameters, see create_l3_intf
 */
int vxlan_roo_create_l3_intf(int unit, uint32 flags, uint8 open_vlan, uint8 add_gport_to_vlan, bcm_gport_t port, int vlan, int vrf, bcm_mac_t mac_addr, int native_vsi_compensation, int *intf) {

  int rc, station_id;
  bcm_l2_station_t station; 
  bcm_l3_intf_t l3if, l3if_ori;
  bcm_l3_ingress_t l3_ing_if;
  bcm_vlan_control_vlan_t control_vlan;

  bcm_l3_intf_t_init(&l3if);
  bcm_l3_ingress_t_init(&l3_ing_if);
  bcm_l2_station_t_init(&station); 
  bcm_vlan_control_vlan_t_init(&control_vlan);

  /* set my-Mac global MSB */
  station.flags = 0;
  sal_memcpy(station.dst_mac, mac_addr, 6);
  station.src_port_mask = 0; /* port is not valid */
  station.vlan_mask = 0; /* vsi is not valid */
  station.dst_mac_mask[0] = 0xff; /* dst_mac my-Mac MSB mask is -1 */
  station.dst_mac_mask[1] = 0xff;
  station.dst_mac_mask[2] = 0xff;
  station.dst_mac_mask[3] = 0xff;
  station.dst_mac_mask[4] = 0xff;
  station.dst_mac_mask[5] = 0xff;

  rc = bcm_l2_station_add(unit, &station_id, &station);
  if (rc != BCM_E_NONE) {
    return rc;
  }

  if(open_vlan) {
      rc = bcm_vlan_create(unit, vlan);
      if (rc != BCM_E_NONE) {
        if(verbose >= 3) {
            printf("fail open vlan(%d)", vlan);
            printf("    continue..\n");
        }
      }

      /* Assuming flooding domain MC 1 */
      /* Set VLAN with MC 1 */
      rc = bcm_vlan_control_vlan_get(unit, vlan, &control_vlan);
      if (rc != BCM_E_NONE) {
          printf("fail get control vlan(%d)\n", vlan);
          return rc;
      }

      control_vlan.unknown_unicast_group = 0x1;
      control_vlan.unknown_multicast_group = 0x1;
      control_vlan.broadcast_group = 0x1;
      rc = bcm_vlan_control_vlan_set(unit, vlan, control_vlan);
      if (rc != BCM_E_NONE) {
          printf("fail set control vlan(%d)\n", vlan);
          return rc;
      }
  }
  if (add_gport_to_vlan) {
      rc = bcm_vlan_gport_add(unit, vlan, port, 0);
      if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
          printf("create_l3_intf: fail add port(0x%08x) to vlan(%d)\n", port, vlan);
        return rc;
      }
  }

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  bcm_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = vlan;
  rc = bcm_l3_intf_get(unit, &l3if_ori);
  if (rc == BCM_E_NONE) {
      /* if L3 INTF already exists, replace it*/
      l3if.l3a_flags = flags | BCM_L3_REPLACE | BCM_L3_WITH_ID;
      l3if.l3a_intf_id = vlan;
  }
  else {
      l3if.l3a_flags = flags; 
      l3if.l3a_intf_id = *intf;
  }

  sal_memcpy(l3if.l3a_mac_addr, mac_addr, 6);
  l3if.l3a_vid = vlan;
  l3if.native_routing_vlan_tags = native_vsi_compensation; 

  rc = bcm_l3_intf_create(unit, l3if);
  if (rc != BCM_E_NONE) {
    return rc;
  }

  /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
  if (!urpf_mode_per_rif) {
    /* Set uRPF global configuration */
    rc =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
    if (rc != BCM_E_NONE) {
      return rc;
    }
  }

  /* set RIF enable RPF*/
  if (flags & BCM_L3_RPF) {
      l3_ing_if.urpf_mode = L3_uc_rpf_mode; /* RPF mode is loose has to match global configuration or Disabled */
  }
  else{
      l3_ing_if.urpf_mode = bcmL3IngressUrpfDisable; 
  }
  
  l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
  l3_ing_if.vrf = vrf;
  rc = bcm_l3_ingress_create(unit, l3_ing_if, l3if.l3a_intf_id);
  if (rc != BCM_E_NONE) {
    return rc;
  }
  *intf = l3if.l3a_intf_id;

  if(verbose >= 1) {
      printf("created ingress interface = 0x%08x, on <port,vlan> = <%d,%d> in unit %d\n", l3if.l3a_intf_id,port,vlan, unit);
  }

  if(verbose >= 2) {
      printf("vrf = %d ", vrf);
      printf("mac-address: ");
      l2_print_mac(mac_addr);
      printf("\n\r");
  }

  return rc;
}




/* Use this version of ipv4_tunnel_build_tunnels only if:
 * you have already built the LL of the tunnel.
 * Otherwise, use ipv4_tunnel_build_tunnels
 * 
 * Description:
 * the ip tunnel will be created and will connect the ip to the LL
 * For more details, see: ipv4_tunnel_build_tunnels
 */
int vxlan_roo_ipv4_tunnel_build_tunnels(int unit, int out_port, bcm_if_t ll_eep_id, bcm_if_t* intf_ids, bcm_gport_t *tunnel_gports){
    int rv;
    int ing_intf_in;
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags = 0;
    int flags1 = 0;
    int out_vlan = 100;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;

    /* my-MAC {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();

    /* tunnel 1 info*/
    bcm_tunnel_initiator_t tunnel_1;
    ip_tunnel_s tunnel_info_1 = ip_tunnel_glbl_info.tunnel_1;
    bcm_mac_t next_hop_mac  = tunnel_info_1.da; /* default: 00:00:00:00:cd:1d} */
    int tunnel_itf1=0;

    /* tunnel 2 info */
    bcm_tunnel_initiator_t tunnel_2;
    ip_tunnel_s tunnel_info_2 = ip_tunnel_glbl_info.tunnel_2;
    bcm_mac_t next_hop_mac2  = tunnel_info_2.da; /* default: {0x20:00:00:00:cd:1d} */
    int tunnel_itf2=0;

	/*** create egress router interface ***/
	rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan,0,mac_address, &ing_intf_out); 
	if (rv != BCM_E_NONE) {
		printf("Error, create ingress interface-1, in_port=%d, \n", out_port);
	}
    
    /*** create IP tunnel 1 ***/
    bcm_tunnel_initiator_t_init(&tunnel_1);
    tunnel_1.dip = tunnel_info_1.dip; /* default: 161.0.0.17*/
    tunnel_1.sip = tunnel_info_1.sip; /* default: 160.0.0.17*/
    tunnel_1.dscp = tunnel_info_1.dscp; /* default: 10 */
    tunnel_1.flags = 0;
    tunnel_1.ttl = tunnel_info_1.ttl; /* default: 60 */
    tunnel_1.type = ip_tunnel_glbl_info.tunnel_1_type; /* default: bcmTunnelTypeGreAnyIn4*/ 
    tunnel_1.vlan = out_vlan;
    tunnel_1.dscp_sel = bcmTunnelDscpAssign;
    tunnel_1.l3_intf_id = ((ll_eep_id > 0)? ll_eep_id :0) ;
    tunnel_itf1 = tunnel_gports[0];
    rv = add_ip_tunnel(unit,&tunnel_itf1,&tunnel_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 1\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_1 on intf:0x%08x \n",tunnel_itf1);
    }

    /*** create tunnel 2 ***/
    bcm_tunnel_initiator_t_init(&tunnel_2);
    tunnel_2.dip = tunnel_info_2.dip; /* default: 171.0.0.17*/
    tunnel_2.sip = tunnel_info_2.sip; /* default: 170.0.0.17*/
    tunnel_2.dscp = tunnel_info_2.dscp; /* default: 11 */
    tunnel_2.flags = 0;
    tunnel_2.ttl = tunnel_info_2.ttl; /* default: 50 */
    tunnel_2.type = ip_tunnel_glbl_info.tunnel_2_type; /* default: bcmTunnelTypeIpAnyIn4 */
    tunnel_2.vlan = out_vlan;
    tunnel_2.dscp_sel = bcmTunnelDscpAssign;
    tunnel_2.l3_intf_id = ((ll_eep_id > 0)? ll_eep_id :0) ;
    tunnel_itf2 = tunnel_gports[1];
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_2 on intf:0x%08x \n",tunnel_itf2);
    }

    /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
    if (ll_eep_id < 0) {
        flags1 |= BCM_L3_EGRESS_ONLY;
        rv = create_l3_egress(unit,flags1,out_port,0,tunnel_itf1,next_hop_mac, &fec[0], &encap_id[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_port=%d, \n", out_port);
        }
        if(verbose >= 1) {
            printf("no FEC is allocated FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id 0x%08x, \n", encap_id[0]);
        }
    } 

    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    if (ll_eep_id < 0) {
        flags1 = 0;
    } else {
        flags1 = BCM_L3_INGRESS_ONLY;
    }
    rv = create_l3_egress(unit,flags1,out_port,out_vlan,tunnel_itf2,next_hop_mac2, &fec[1], &encap_id[1]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", fec[1]);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }

   /* interface for tunnel_1 is IP-tunnel ID */
    intf_ids[0] = tunnel_itf1;

    /* interface for tunnel_2 is egress-object (FEC) */
    intf_ids[1] = fec[1];


    /* refernces to created tunnels as gport */
    tunnel_gports[0] = tunnel_1.tunnel_id;
    tunnel_gports[1] = tunnel_2.tunnel_id;

    return rv;
}


/* modify current configuration to build the native ethernet header with specific number of vlans:
 * - update EVE. Add "nbr_native_vlans" vlans. (up to 2 vlans) 
 * - update native vsi compensation: number of expected native vlan for the vsi. 
 *    Number of native vlan is specified in EVE. EVE is applied after UDP is built, 
 *   so UDP header size field is calculated according to native vsi compensation.
 */
int vxlan_roo_modify_native_vlan_number(int unit, int nbr_native_vlans) {
   int rv; 

   
   /* update EVE according to number of vlan to build */

   /* set action according to number of native vlans to build */
   uint32 flags = BCM_VLAN_ACTION_SET_EGRESS;

   bcm_vlan_action_t outer_action, inner_action; 
   if (nbr_native_vlans == 2) {
       outer_action = bcmVlanActionAdd; 
       inner_action = bcmVlanActionAdd; 
   } else if (nbr_native_vlans == 1) {
       outer_action = bcmVlanActionAdd; 
       inner_action = bcmVlanActionNone; 
   } else if (nbr_native_vlans == 0) {
       outer_action = bcmVlanActionNone; 
       inner_action = bcmVlanActionNone; 
   } else {
       printf("Error, invalid number of native vlans \n");
       return rv; 
   }

   /* overwrite EVE action */
   int action_id = g_eve_edit_utils.action;   /* use global action_id */
   bcm_vlan_action_set_t action;
   
   bcm_vlan_action_set_t_init(&action);
   action.dt_outer = outer_action;
   action.dt_inner = inner_action;
   action.outer_tpid = 0x8100;
   action.inner_tpid = 0x9100; 
   rv = bcm_vlan_translate_action_id_set( unit, 
                                          flags,
                                          action_id,
                                          &action);
   if (rv != BCM_E_NONE) {
      printf("Error, bcm_vlan_translate_action_id_set \n");
      return rv; 
   }

   /* update native vsi compensation */

   /* use replace functionality to update the native vsi compensation */
   bcm_l3_intf_t l3if; 
   bcm_l3_intf_t_init(&l3if);
   int out_vlan = g_vxlan_roo.native_out_vlan;
   l3if.l3a_intf_id = out_vlan;
   rv = bcm_l3_intf_get(unit, &l3if);
   if (rv != BCM_E_NONE) {
        printf("Error, vlan__eve_action_replace__set \n");
        return rv; 
   }
   l3if.l3a_flags = l3if.l3a_flags | BCM_L3_REPLACE | BCM_L3_WITH_ID;
   l3if.native_routing_vlan_tags = nbr_native_vlans; 
   rv = bcm_l3_intf_create(unit, &l3if); 
   if (rv != BCM_E_NONE) {
        printf("Error, bcm_petra_l3_intf_create \n");
        return rv; 
   }
   return rv; 
}


/******* Run example ******/
 
/* 
 * This test is based on vxlan example
 */
int vxlan_roo_run(int unit, int in_port, int out_port, int vpn_id){

    int rv;
    int vpn; 

    /*** init ***/

    /* init ip tunnel global */
    ip_tunnel_s tunnel_1;   /* tunnel 1 is not used in vxlan roo */
    ip_tunnel_s tunnel_2;   /* tunnel 2 info from cint_ip_tunnel.c */
    ip_tunnel_info_get(&tunnel_1, &tunnel_2);


    /* init ip tunnel termination global */
    ip_tunnel_term_glbl_info.skip_ethernet_flag = 1;


    /* init vxlan globals */
    vxlan_s vxlan_param; 
    vxlan_struct_get(&vxlan_param);
    if (vpn_id >=0) {
        vxlan_param.vpn_id = vpn_id;
    }
    vxlan_init(vxlan_param);
    vpn = g_vxlan.vpn_id;

    bcm_gport_t in_tunnel_gports[2];
    bcm_gport_t out_tunnel_gports[2];
    bcm_if_t out_tunnel_intf_ids[2];/* out tunnels interfaces
                                  out_tunnel_intf_ids[0] : is tunnel-interface-id
                                  out_tunnel_intf_ids[1] : is egress-object (FEC) points to tunnel
                              */
    int eg_intf_ids[2];/* interface for routing, not used */
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id[2];
    int vxlan_port_flags = 0;


    /* init ip tunnel info, to fit VXLAN usage */
    ip_tunnel_glbl_init_vxlan(unit,0);

    /* init L2 VXLAN module */
    rv = bcm_vxlan_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
    }

    /* set trap for bounce back filter to drop */
    set_trap_to_drop(unit, bcmRxTrapEgTrillBounceBack);

    /*** build LL for overlay/tunnel ***/

    /* In ROO, the overlay LL encapsulation is built with a different API call 
       (bcm_l2_egress_create instead of bcm_l3_egress create) */
    bcm_l2_egress_t l2_egress_overlay;
    bcm_l2_egress_t_init(&l2_egress_overlay);
     
    l2_egress_overlay.dest_mac   = tunnel_2.da; /* next hop. default: {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d} */
    l2_egress_overlay.src_mac    = tunnel_2.sa; /* my-mac */
    l2_egress_overlay.outer_vlan = 0x064;       /* PCP DEI (0) + outer_vlan (100=0x64)  */
    l2_egress_overlay.ethertype  = 0x800;       /* ethertype for IP */
    l2_egress_overlay.outer_tpid = 0x8100;      /* outer tpid */
    if (g_vxlan_roo.overlay_ll_outlif != 0) {   /* outlif (or overlay arp eedb index) */
        l2_egress_overlay.encap_id = g_vxlan_roo.overlay_ll_outlif ;     
        /* indicate outlif is provided */
        l2_egress_overlay.flags    = BCM_L2_EGRESS_WITH_ID;  
    }
    bcm_l2_egress_create(unit, &l2_egress_overlay);

    if(verbose >= 2){
        printf("Open tunnels: \n\r");
    }
    /*** build tunnel initiators ***/
    rv = vxlan_roo_ipv4_tunnel_build_tunnels(unit, out_port, l2_egress_overlay.encap_id, out_tunnel_intf_ids,out_tunnel_gports);
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


    /*** build L2 VPN ***/

    rv = vxlan_open_vpn(unit,vpn,g_vxlan.vni);
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_open_vpn, vpn=%d, \n", vpn);
    }


    /***  native routing ***/
    
    /* create L3 intf: myMac, VLAN for native routing */
    int in_vlan = vpn;
    int out_vlan = g_vxlan_roo.native_out_vlan;
    int vrf = 123;
    bcm_mac_t native_my_mac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC: 00:0c:00:02:00:00 */
    
    int native_ing_intf; 

    rv = vxlan_roo_create_l3_intf(unit, 
                        0, /* flags ( no rpf check for roo application: so flag BCM_L3_RPF is forbidden here.)  */
                        FALSE, /* don't open vlan */
                        FALSE, /* don't add port to vlan */
                        in_port, /* port to add to vlan */
                        vpn, /* vlan: Use vxlan vpn */
                        vrf, 
                        native_my_mac, /* native my mac */
                        1, /* native ethernet compensation */
                        &native_ing_intf 
    );
    if (rv != BCM_E_NONE) {
        printf("Error, vxlan_roo_create_l3_intf\n");
    }

  /* native router interface: 
     create LL for native: next hop, VLAN for native routing */
  bcm_mac_t native_next_hop = {0x00, 0x00, 0x07, 0x00, 0x01, 0x23}; 
  int native_fec_id;   
  int native_encap_id =  0; /* native LL eedb index */
  rv = create_l3_egress(unit, 
                   0, /* flags */
                   out_port,  
                   out_vlan, /* out-vlan */
                   native_ing_intf, /* l3_intf  */
                   native_next_hop, /* next-hop */
                   &native_fec_id, /* output: fec id. Point to LL eedb entry  */
                   &native_encap_id /* LL eedb index */
                   );
  if (rv != BCM_E_NONE) {
      printf("Error, create_l3_egress\n");
  }
  printf("successfully created LL \n");

  /* add native host entry: add simple routing host to host table */
  int simple_routing_fec; 
  int simple_routing_id;
  int simple_encap_id = 0;
  bcm_mac_t simple_routing_next_hop = {0x00, 0x00, 0x00, 0x00, 0x05, 0x81}; 
  rv = create_l3_egress(unit,                       
                        0,            /* flags             */
                        in_port,                  
                        vpn,                     
                        native_ing_intf,               
                        simple_routing_next_hop,
                        &simple_routing_fec, 
                        &simple_encap_id /* LL eedb index for simple routing */
                        );
  if (rv != BCM_E_NONE) {
      printf("Error, create_l3_egress\n");
  }


  rv = vxlan_add_port(unit,vpn,in_port,in_tunnel_gports[1],out_tunnel_gports[1],out_tunnel_intf_ids[1],vxlan_port_flags,&vxlan_port_id[1]);
  if (rv != BCM_E_NONE) {
      printf("Error, vxlan_add_port 2, in_gport=0x%08x --> out_intf=0x%08x \n", in_tunnel_gports[1],out_tunnel_intf_ids[1]);
  }

  /* port vlan port*/

  /* init vlan port glbl info */
  vswitch_metro_mp_info_init(out_port,0,0);



  rv = l2__port_vlan__create(unit, out_port, out_vlan, out_vlan, &vlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, l2__port_vlan__create, in_intf=0x%08x \n", vlan_port_id);
  }

  rv = vswitch_add_port(unit, vpn,out_port, vlan_port_id);
  if (rv != BCM_E_NONE) {
      printf("Error, vswitch_add_port\n");
      return rv;
  }

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
       /* set egress vlan translation: double tagged */
    rv = vlan__eve_default__set(unit, vlan_port_id, out_vlan, 21, bcmVlanActionAdd, bcmVlanActionNone);
    if (rv != BCM_E_NONE) {                                                                       
        printf("Error, vlan__eve_default__set\n");              
    } 
  }


  printf("vrf:   %x \n", vrf);
  printf("l3_host.l3a_port_tgid: tunnel gport %x \n", vxlan_port_id[1]);
  printf("l3_host.l3a_intf: native out rif    %x \n", native_ing_intf);
  printf("l3_host.encap_id: native arp pointer: eei entry %x \n", native_encap_id);

  /* add native host entry: add entry through vxlan overlay in host table */
  bcm_l3_host_t l3_host;      
  bcm_l3_host_t_init(l3_host);  
  /* key of host entry */
  l3_host.l3a_vrf = vrf;                      /* router interface */ 
  l3_host.l3a_ip_addr = g_vxlan_roo.native_dip;  /* ip host entry */
  /* data of host entry */
  l3_host.l3a_port_tgid = vxlan_port_id[1]; /* overlay tunnel: vxlan gport */
  l3_host.l3a_intf = native_ing_intf;       /* native out rif */
  l3_host.encap_id = native_encap_id;       /* native arp pointer: encap/eei entry */
  bcm_l3_host_add(unit, &l3_host);         
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");
      return rv;
  } 

  /* add simple routing host entry */
  bcm_l3_host_t l3_host_simple_routing; 
  bcm_l3_host_t_init(l3_host_simple_routing);  
  /* key of host entry */
  l3_host_simple_routing.l3a_vrf = vrf;            /* router interface */ 
  l3_host_simple_routing.l3a_ip_addr = 0xA0A1A1A1; /* ip host entry */
  /* data of host entry */
  l3_host_simple_routing.l3a_intf  = simple_routing_fec;
  rv = bcm_l3_host_add(unit, &l3_host_simple_routing);
  if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");
      return rv;
  }
  return rv;
}
