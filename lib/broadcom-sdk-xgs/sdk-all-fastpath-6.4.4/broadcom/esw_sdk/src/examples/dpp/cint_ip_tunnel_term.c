/*$Copyright: (c) 2013 Broadcom Corporation All Rights Reserved.$*/
/* $Id: cint_ip_tunnel_term.c,v 1.13 Broadcom SDK $
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
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
cint
int rv;
verbose = 2; 
rv = ipv4_tunnel_term_example(0,13,13);
 
Tunnels according IP next protocol example: 
SOC proeprty: bcm886xx_ip4_tunnel_termination_mode=5 
cint;
cint_reset();
exit;
cint ../../../../src/examples/dpp/cint_ip_route.c
cint ../../../../src/examples/dpp/cint_ip_tunnel_term.c
cint
int rv;
verbose = 2; 
ipv4_tunnel_term_next_protocol_example(0,13,13); 
 
 
*/ 

/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;

/********** 
  functions
 ********** */



/******* Run example ******/
 
struct ip_tunnel_term_glbl_info_s{
    int tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf;   /* the router interface from RIF (VSI) */
    uint8 plus_1_protection;
    uint8 plus_1_mc_id;
    uint8 skip_ethernet_flag; /* indicate to skip ethernet after terminating ip tunnel. */
    int sip;  /* sip + dip to terminate */
    int dip;  
    int sip2; /* another sip + dip to terminate */
    int dip2; 

};

ip_tunnel_term_glbl_info_s ip_tunnel_term_glbl_info = 
{
/* tunnel_vrf  | rif_vrf | plus_1_protection | plus_1_mc_id | skip_ethernet_flag */	
3,               2,        0,                  6200,          0, 
/* sip + dip to terminate */ 
/* sip: 160.0.0.17 | dip: 161.0.0.17 */
0xA0000011,          0xA1000011, 
/* another sip + dip to terminate */
/* sip:0  | dip: 171.0.0.17 */
0x0,        0xAB000011
}; 



/* check if <DIP,SIP> lookup done in TCAM according to soc property */
int tcam_dip_sip_enabled(int unit){
    if(soc_property_get(unit , "bcm886xx_vxlan_tunnel_lookup_mode",2)==2 &&
       soc_property_get(unit , "bcm886xx_vxlan_enable",0))
        return 1;
    if(soc_property_get(unit , "bcm886xx_l2gre_tunnel_lookup_mode",2)==3 &&
       soc_property_get(unit , "bcm886xx_l2gre_enable",0))
        return 1;

    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==3 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==5)
     return 1;

    return 0;
}


void ip_tunnel_term_print_key(
    char *type, 
    bcm_tunnel_terminator_t *tunnel_term
)
{
    printf("%s  key: ", type);

    if(tunnel_term->type == bcmTunnelTypeIpAnyIn6) {
        printf("IPv6 DIP \n\r");
        return;
    }
    if(tunnel_term->sip_mask == 0) {
        printf("SIP: masked    ");
    }
    else if(tunnel_term->sip_mask == 0xFFFFFFFF) {
        printf("SIP:");
        print_ip_addr(tunnel_term->sip);
    }
    else if(tunnel_term->sip_mask == 0xFFFFFFFF) {
        printf("SIP:");
        print_ip_addr(tunnel_term->sip);
        printf("/0x%08x ",tunnel_term->sip_mask);
    }
    if(tunnel_term->dip_mask == 0) {
        printf("DIP: masked    ");
    }
    else if(tunnel_term->dip_mask == 0xFFFFFFFF) {
        printf("DIP:");
        print_ip_addr(tunnel_term->dip);
    }
    else if(tunnel_term->dip_mask == 0xFFFFFFFF) {
        printf("DIP:");
        print_ip_addr(tunnel_term->dip);
        printf("/0x%08x ",tunnel_term->dip_mask);
    }
    printf("\n\r");
}

int tcam_dip_sip_next_protocol_enabled(int unit){
    if(soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==4 ||
       soc_property_get(unit , "bcm886xx_ip4_tunnel_termination_mode",3)==5)
     return 1;

    return 0;
}


int ip_tunnel_term_open_route_interfaces(int unit, int in_port, int out_port, int rif_vrf,int tunnel_vrf,bcm_if_t *tunnel_rif_id, bcm_if_t *egress_intf1, bcm_if_t *egress_intf2){
    int rv;
    int open_vlan = 1;
    /* my-mac for LL termination*/
    bcm_mac_t my_mac  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    int flags=0;   /* intf flags */
    int flags1=0; /* egress flags*/
    /* packets income on this vlan with my-mac will be LL termination*/
    int in_vlan_1 = 10; 
    int in_vlan_2 = 20;
    bcm_if_t ing_intf_1;
    bcm_if_t ing_intf_2;
    

    /* interface to be used for tunnel termination */
    int tunnel_intf = 30;

    /* dummy mac as this interface is resolved due to tunnel termination*/
    bcm_mac_t dummy_mac  = {0x00, 0x0c, 0x00, 0x00, 0x00, 0x00};  

    /* egress interfaces for outgoin side */
    int out_vlan_1 = 100;
    int out_vlan_2 = 200;
    int tunnel_rif = 300;

    bcm_mac_t next_hop_mac_1  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t next_hop_mac_2  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_if_t ing_intf_out_1;
    bcm_if_t ing_intf_out_2;
    int encap_id[2];


    /* create ingress l3-intf 1 */
    rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan_1,rif_vrf,my_mac, &ing_intf_1); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_vlan_1=%d, \n", in_vlan_1);
    }


    /* create ingress l3-intf 2 */
      rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan_2,rif_vrf,my_mac, &ing_intf_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_vlan_2=%d, \n", in_vlan_2);
    }

    /* create ingress l3-intf for tunnel termination */
    *tunnel_rif_id = tunnel_rif;
      rv = create_l3_intf(unit,flags,open_vlan,in_port,tunnel_rif,tunnel_vrf,dummy_mac, tunnel_rif_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_vlan_1=%d, \n", in_vlan_2);
    }


   /* create egress route interfaces for MY-MAC encapsulation */
  /* create egress l3-intf 1 */
  rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan_1,0,my_mac, &ing_intf_out_1); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_vlan_1=%d, \n", in_vlan_1);
    }

  /* create egress l3-intf 1 */
  rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan_2,0,my_mac, &ing_intf_out_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_vlan_1=%d, \n", in_vlan_1);
    }


    /*** create egress object 1 ***/
    rv = create_l3_egress(unit,flags1,out_port,out_vlan_1,ing_intf_out_1,next_hop_mac_1, egress_intf1, &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", *egress_intf1);
        printf("next hop mac at encap-id %08x, \n", encap_id[0]);
    }


    /*** create egress object 2 ***/
    rv = create_l3_egress(unit,flags1,out_port,out_vlan_2,ing_intf_out_2,next_hop_mac_2, egress_intf2, &encap_id[1]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, \n", *egress_intf2);
        printf("next hop mac at encap-id %08x, \n", encap_id[1]);
    }

  return rv;
}

/*
 * buid two IP tunnels termination
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   =  161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  sip   = 170.0.0.17
 *   -  dip   =  171.0.0.17
 *   -  dscp  = 11;
 *   -  ttl   = 50;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + Define FEC point to this tunnel
 *  
 *   returned value:
 *      intf_ids[] : array includes the interface-id to forward to the built tunnels.
 *         intf_ids[0] : is IP-tunnel Id.
 *         intf_ids[1] : is egress-object (FEC) points to the IP-tunnel
 */
int ipv4_tunnel_term_example(int unit, int in_port, int out_port){
    int rv;
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    bcm_gport_t in_tunnel_gports[2]; /* to include IP tunnel interfaces interfaces */

    /*** build tunnel terminations ***/
    rv = ipv4_tunnel_term_build_tunnel_terms(unit,in_port,out_port,eg_intf_ids,in_tunnel_gports); 
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_build_tunnel_terms, in_port=%d, \n", in_port);
    }

    /*** add routes to egress interfaces ***/
    rv = ipv4_tunnel_term_add_routes(unit,eg_intf_ids);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_add_routes, eg_intf_ids=0x%08x,%0x%08x \n", eg_intf_ids[0],eg_intf_ids[1]);
    }

    return rv;
}


/*
 * buid two IP tunnels termination with same DIP,SIP key but different VPN-ID 
 * The different will be in the above header: 
 *  In case header above is IPV4 then use Tunnel 1 
 *  In case header above is GRE then use Tunnel 2
 * 
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   -  port_property = (use_port_property) ? 5 : none
 *  
 * Tunnnel 2: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   -  GRE header
 *  
 *   IPv4 Header above:
 *   -  DIP 1.0.0.17
 *  
 *   Input parameters:
 *   -  In-Port: Incoming port packet is injected to
 *   -  Out-Port: Expected outgoing port after tunnel termination
 *  
 *   Traffic:
 *   Send traffic :
 *    - Stream1 : Packet with DataoIPV4oTunnel1
 *    - Stream2 : Packet with DataoIPV4oTunnel2
 *   Receive traffic:
 *    - Both streams will go to Out-Port
 *    - Stream1 receive packet DataoIPV4oEthernet with VLAN 10 DA 00:0C:00:02:00:02
 *    - Stream2 receive packet DataoIPV4oEthernet with VLAN 20 DA 00:0C:00:02:00:01
 */
int ipv4_tunnel_term_next_protocol_example(int unit, int in_port, int out_port, uint8 use_port_property){
    int rv;
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif;
    bcm_gport_t in_tunnel_gports[2];
    bcm_if_t eg_intf_ids[2]; /* to include egress router interfaces */
    uint32 port_property = 5;
   
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/

    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    int sip1 = ip_tunnel_term_glbl_info.sip; /* default: 160.0.0.17*/
    int dip1 = ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    int sip1_mask = 0xFFFFFFFF; 
    int dip1_mask = 0xFFFFFFFF; 
    int type1 = bcmTunnelTypeIp4In4; /* header above is IPV4 */
    
    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    int sip2 = sip1;
    int dip2 = dip1;
    int sip2_mask = sip1_mask; 
    int dip2_mask = dip1_mask; 
    int type2 = bcmTunnelTypeGreAnyIn4; /* header above is GRE */

    /* 1+1 protection info */
    int failover_id, proection_mc_id = ip_tunnel_term_glbl_info.plus_1_mc_id;

    if (!tcam_dip_sip_next_protocol_enabled(unit)) {
        printf("Error, dip sip next protocol not enabled\n");
        return BCM_E_PARAM;
    }

    /* If using port property as part of the key, configure in_port's port property */
    if (use_port_property) {
        rv = bcm_port_class_set(unit, in_port, bcmPortClassFieldIngressVlanTranslation, port_property);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_class_set\n");
            return rv;
        }
    }

    if(ip_tunnel_term_glbl_info.plus_1_protection){
        rv = plus_1_protection_init(unit,&failover_id,&proection_mc_id);
        if (rv != BCM_E_NONE) {
            printf("Error, plus_1_protection_init\n");
            return rv;
        }
    }
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif,&egress_intf_1,&egress_intf_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);    
    tunnel_term_1.dip = dip1;
    tunnel_term_1.dip_mask = dip1_mask;
    tunnel_term_1.sip = sip1;
    tunnel_term_1.sip_mask = sip1_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = type1; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_1.ingress_failover_id = failover_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    if (use_port_property) {
        BCM_PBMP_PORT_SET(tunnel_term_1.pbmp, port_property); /* Add port property to tunnel.  */
    }
    if (in_tunnel_gports[0] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_1.tunnel_id,in_tunnel_gports[0]);
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, \n", tunnel_term_1.tunnel_id);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.dip = dip2;
    tunnel_term_2.dip_mask = dip2_mask;
    tunnel_term_2.sip = sip2;
    tunnel_term_2.sip_mask = sip2_mask;
    tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
    tunnel_term_2.type = type2;/* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_2.ingress_failover_id = failover_id;
        tunnel_term_2.failover_tunnel_id = tunnel_term_1.tunnel_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    tunnel_term_2.tunnel_id = in_tunnel_gports[1];

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, \n", tunnel_term_2.tunnel_id);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;            
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /*** add routes to egress interfaces ***/
    rv = ipv4_tunnel_term_add_routes(unit,eg_intf_ids);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_term_add_routes, eg_intf_ids=0x%08x,%0x%08x \n", eg_intf_ids[0],eg_intf_ids[1]);
    }

    return rv;
}

/*
 * buid two IP tunnels termination
 * Tunnnel 1: 
 *   -  sip   = 160.0.0.17
 *   -  dip   = 161.0.0.17
 *   -  dscp  = 10;
 *   -  ttl   = 60;
 *   -  type  = bcmTunnelTypeIp4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 00:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + No Fec defined over this tunnel
 *  
 * Tunnnel 2: 
 *   -  sip   = 170.0.0.17
 *   -  dip   =  171.0.0.17
 *   -  dscp  = 11;
 *   -  ttl   = 50;
 *   -  type  = bcmTunnelTypeGre4In4;
 *   -  Vlan  = 100
 *   -  Dmac  = 20:00:00:00:cd:1d
 *   -  Smac  = 00:0c:00:02:00:00
 *   + Define FEC point to this tunnel
 *  
 *   returned value:
 *      intf_ids[] : array includes the interface-id to forward to the built tunnels.
 *         intf_ids[0] : is IP-tunnel Id.
 *         intf_ids[1] : is egress-object (FEC) points to the IP-tunnel
 */

int ipv4_tunnel_term_example_dvapi(int unit, int in_port, int out_port){
    return ipv4_tunnel_term_example(unit,in_port,out_port);
}



int tunnel_open_ingress_mc_group(int unit, int* mc_group_id){
    int rv = BCM_E_NONE;
    int flags;
    bcm_multicast_t mc_group = *mc_group_id;
    
    /* destroy before open, to ensure it not exist */
    rv = bcm_multicast_destroy(unit, mc_group);
    
    /* create ingress MC */
    flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_L3;
    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc create, flags $flags mc_group $mc_group \n");
        return rv;
    }

    *mc_group_id = mc_group;
    return rv;
}

int plus_1_protection_init(int unit, int *failover_id, int *mc_id){

    int rv = BCM_E_NONE;
    if(verbose >= 1) {
        printf("run with 1+1 protection! \n");
    }

    rv = bcm_failover_create(unit, BCM_FAILOVER_INGRESS, failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create (failover_id)\n");
        print rv;
        return rv;
    }
    if(verbose >= 1) {
        printf("create Failover-ID: %d\n", *failover_id);
    }

    /* create MC for protection */
    tunnel_open_ingress_mc_group(unit,mc_id);
    if(verbose >= 1) {
        printf("created protection_mc1 : 0x0%8x\n", *mc_id);
    }

    return rv;


}


int ipv4_tunnel_term_build_tunnel_terms(int unit, int in_port, int out_port,bcm_if_t *eg_intf_ids, bcm_gport_t *in_tunnel_gports){
    int rv;

    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */
    int tunnel_rif; /* RIF from tunnel termination*/

    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/

    /* tunnel term 1 info*/
    bcm_tunnel_terminator_t tunnel_term_1;
    int sip1 = ip_tunnel_term_glbl_info.sip; /* default: 160.0.0.17*/
    int dip1 = ip_tunnel_term_glbl_info.dip; /* default: 161.0.0.17*/
    int sip1_mask = 0xFFFFFFFF; 
    int dip1_mask = 0xFFFFFFFF; 

    /* tunnel term 2 info: according to DIP  only*/
    bcm_tunnel_terminator_t tunnel_term_2;
    int sip2 = ip_tunnel_term_glbl_info.sip2; /* 0.0.0.10*/
    int dip2 = ip_tunnel_term_glbl_info.dip2; /* 171.0.0.17*/
    int sip2_mask = 0x0; 
    int dip2_mask = 0xFFFFFFFF; 

    /* 1+1 protection info */
    int failover_id, proection_mc_id = ip_tunnel_term_glbl_info.plus_1_mc_id;

    /* if TCAM not enabled reset SIP from key1 */
    if (!tcam_dip_sip_enabled(unit)) {
        sip1 = sip1_mask = 0;
    }

    if(ip_tunnel_term_glbl_info.plus_1_protection){
        rv = plus_1_protection_init(unit,&failover_id,&proection_mc_id);
        if (rv != BCM_E_NONE) {
            printf("Error, plus_1_protection_init\n");
        }
    }
    /*** build router interface ***/
    rv = ip_tunnel_term_open_route_interfaces(unit,in_port,out_port,rif_vrf, tunnel_vrf, &tunnel_rif,&egress_intf_1,&egress_intf_2); 
    if (rv != BCM_E_NONE) {
        printf("Error, cip_tunnel_term_open_route_interfaces, in_port=%d, \n", in_port);
    }

    printf("TT tunnelrif=%d\n", tunnel_rif);

    /* store egress interfaces, for routing */
    eg_intf_ids[0] = egress_intf_1;
    eg_intf_ids[1] = egress_intf_2;

    /* create IP tunnel terminators */
    /*** create IP tunnel terminator 1 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_1);    
    tunnel_term_1.dip = dip1;
    tunnel_term_1.dip_mask = dip1_mask;
    tunnel_term_1.sip = sip1;
    tunnel_term_1.sip_mask = sip1_mask;
    tunnel_term_1.tunnel_if = tunnel_rif;
    tunnel_term_1.type = bcmTunnelTypeIpAnyIn4; /* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_1.ingress_failover_id = failover_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }

    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    if (in_tunnel_gports[0] != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel_term_1.tunnel_id,in_tunnel_gports[0]);
        tunnel_term_1.flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_1, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_1 =0x%08x, \n", tunnel_term_1.tunnel_id);
        ip_tunnel_term_print_key("created term 1",&tunnel_term_1);
    }

    in_tunnel_gports[0] = tunnel_term_1.tunnel_id;
    
    /*** create IP tunnel terminator 2 ***/
    bcm_tunnel_terminator_t_init(&tunnel_term_2);
    tunnel_term_2.dip = dip2;
    tunnel_term_2.dip_mask = dip2_mask;
    tunnel_term_2.sip = sip2;
    tunnel_term_2.sip_mask = sip2_mask;
    tunnel_term_2.tunnel_if = -1; /* means don't overwite RIF */
    tunnel_term_2.type = bcmTunnelTypeIpAnyIn4;/* this is IPv4 termination */
    if(ip_tunnel_term_glbl_info.plus_1_protection){
        tunnel_term_2.ingress_failover_id = failover_id;
        tunnel_term_2.failover_tunnel_id = tunnel_term_1.tunnel_id;
        tunnel_term_1.failover_mc_group = proection_mc_id;
    }
    tunnel_term_2.tunnel_id = in_tunnel_gports[1];

    /* check skip ethernet */
    if (ip_tunnel_term_glbl_info.skip_ethernet_flag) {
        tunnel_term_2.flags |= BCM_TUNNEL_TERM_ETHERNET;
    }

    rv = bcm_tunnel_terminator_create(unit,&tunnel_term_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_tunnel_terminator_create_2, in_port=%d, \n", in_port);
    }
    if(verbose >= 1) {
        printf("created tunnel terminator_2 =0x%08x, \n", tunnel_term_2.tunnel_id);
        ip_tunnel_term_print_key("created term 2",&tunnel_term_2);
    }
    in_tunnel_gports[1] = tunnel_term_2.tunnel_id;

    return rv;
}


int ipv4_tunnel_term_add_routes(int unit, int *eg_intf_ids){
    int rv = BCM_E_NONE;

    /* internal IP after termination */
    int inter_subnet = 0x01000011; /* 1.0.0.17*/
    int inter_mask =    0xffffff00; /* 255.255.255.0*/
    int egress_intf_1; /* egress object 1*/
    int egress_intf_2; /* egress object 2*/
    int tunnel_vrf = ip_tunnel_term_glbl_info.tunnel_vrf; /* the router interface due to tunnel termination */
    int rif_vrf = ip_tunnel_term_glbl_info.rif_vrf;   /* the router interface from RIF (VSI) */

    egress_intf_2 = eg_intf_ids[0];
    egress_intf_1 = eg_intf_ids[1];

    /* add subnet point to created egress object 1 */
    rv = add_route(unit,inter_subnet,inter_mask,rif_vrf, egress_intf_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route 1, egress_intf_2=%d, \n", egress_intf_2);
    }

    /* add subnet point to created egress object 2*/
    rv = add_route(unit,inter_subnet,inter_mask,tunnel_vrf, egress_intf_1);
    if (rv != BCM_E_NONE) {
        printf("Error, add_route 2 , egress_intf_1=%d, \n", egress_intf_1);
    }

    return rv;
}
