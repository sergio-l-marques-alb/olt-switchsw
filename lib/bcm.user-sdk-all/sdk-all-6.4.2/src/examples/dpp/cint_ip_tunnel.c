/* $Id: cint_ip_tunnel.c,v 1.15 Broadcom SDK $
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
cint ../../../../src/examples/dpp/cint_ip_tunnel.c
cint
int rv; 
ipv4_tunnel_example(0,13,13); 
//set default values  
ip_tunnel_glbl_init(0,0); 
rv = ipv4_tunnel_example(0,13,13);

cint 
*/ 

/* ********* 
  Globals/Aux Variables
 ********** */


/* debug prints */
int verbose = 1;

/********** 
  functions
 ********** */


bcm_mac_t ip_tunnel_my_mac_get() {
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    return mac_address;
}


struct ip_tunnel_s{
    bcm_mac_t da; /* ip tunnel da */
    bcm_mac_t sa; /* ip tunnel my mac */
    int       sip; /* ip tunnel sip */    
    int       dip; /* ip tunnel dip */   
    int       ttl; /* ip tunnel ttl */
    int       dscp; /* ip tunnel dscp */
    bcm_tunnel_dscp_select_t    dscp_sel; /* ip tunnel dscp_sel */
};

/*
 * struct include meta information. 
 * where the cint logic pull data from this struct. 
 * use to control differ runs of the cint, without changing the cint code
 */
struct ip_tunnel_glbl_info_s{
    bcm_tunnel_type_t tunnel_1_type;
    bcm_tunnel_type_t tunnel_2_type;
    ip_tunnel_s tunnel_1;
    ip_tunnel_s tunnel_2;
};

ip_tunnel_glbl_info_s ip_tunnel_glbl_info = 
{
/* tunnel 1 type */
bcmTunnelTypeGreAnyIn4, 
/* tunnel 2 type */
bcmTunnelTypeIpAnyIn4, 
/* tunnel 1  
              da                    |      sa               | sip: 160.0.0.17 | dip: 161.0.0.17 |  ttl   | dscp |   dscp_sel            */ 
{{0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}, ip_tunnel_my_mac_get(), 0xA0000011,        0xA1000011,      60,     10,      bcmTunnelDscpAssign},
/* tunnel 2  
              da                    |      sa               | sip: 170.0.0.17 | dip: 171.0.0.17 |  ttl   | dscp |   dscp_sel            */ 
{{0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}, ip_tunnel_my_mac_get(), 0xAA000011,         0xAB000011,     50,     11,      bcmTunnelDscpAssign}, 
};

/* Initialization of global structs */

void ip_tunnel_glbl_init(int unit, int flags){
    ip_tunnel_glbl_info.tunnel_1_type = bcmTunnelTypeGreAnyIn4;
    ip_tunnel_glbl_info.tunnel_2_type = bcmTunnelTypeIpAnyIn4;
}


void ip_tunnel_info_get(ip_tunnel_s *tunnel_param_1, ip_tunnel_s *tunnel_param_2) {
    sal_memcpy( tunnel_param_1, &ip_tunnel_glbl_info.tunnel_1, sizeof(ip_tunnel_glbl_info.tunnel_1));
    sal_memcpy( tunnel_param_2, &ip_tunnel_glbl_info.tunnel_2, sizeof(ip_tunnel_glbl_info.tunnel_2));
}


/*
 * create ipv4 tunnel
 *  bcm_tunnel_initiator_t *tunnel: include tunnel information
 *  bcm_if_t *itf : (IN/OUT) the placement of the tunnel as encap-id
 *  tunnel->tunnel_id : is update to include the placement of the created tunnel as gport.
 *  
 */
int add_ip_tunnel(int unit,bcm_if_t *itf, bcm_tunnel_initiator_t *tunnel){

    bcm_l3_intf_t l3_intf;
    int rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);

    /* if given ID is set, then use it as placement of the ipv4-tunnel */
    if(*itf != 0) {
        BCM_GPORT_TUNNEL_ID_SET(tunnel->tunnel_id,*itf);
        tunnel->flags |= BCM_TUNNEL_WITH_ID; 
    }

    rv = bcm_tunnel_initiator_create(unit, &l3_intf, tunnel);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    /* include interface-id points to the tunnel */
    *itf = l3_intf.l3a_intf_id;
    return rv;
}

/*
 * Remove ipv4 tunnel
 *  tunnel->tunnel_id : is update to include the placement of the created tunnel as gport.
 *  
 */
int remove_ip_tunnel(int unit,bcm_if_t itf){

    bcm_l3_intf_t l3_intf;
    int rv = BCM_E_NONE;

    bcm_l3_intf_t_init(&l3_intf);

    l3_intf.l3a_intf_id = itf;

    rv = bcm_tunnel_initiator_clear(unit, &l3_intf);
    if(rv != BCM_E_NONE) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }
    return rv;
}


/******* Run example ******/
 
/*
 * IP tunnel example 
 * - build IP tunnels. 
 * - add ip routes/host points to the tunnels
 */
int ipv4_tunnel_example(int unit, int in_port, int out_port){
    int intf_ids[2];
    bcm_gport_t tunnel_gport_ids[2];
    int rv;


    /* build IP tunnels, and get back interfaces */
    rv = ipv4_tunnel_build_tunnels(unit, out_port, intf_ids, tunnel_gport_ids);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }


    /* add IP route host points to the Tunnels */
    rv = ipv4_tunnel_add_routes(unit, in_port, out_port, intf_ids);
    if (rv != BCM_E_NONE) {
        printf("Error, ipv4_tunnel_add_routes, in_port=%d, \n", in_port);
    }

    return rv;
}

int ipv4_tunnel_example_dvapi_run(int unit, int in_port, int out_port){
    ip_tunnel_glbl_init(0,0); 
    return ipv4_tunnel_example(unit,in_port,out_port);
}
 

/* 
 * Create 1:1 mapping from TC to DSCP [for DP = yellow]
 */
int add_TC_to_DSCP_mapping(int unit, bcm_gport_t gport) {

    bcm_qos_map_t map;
    bcm_l3_ingress_t l3_ing_if;

    /* Mapping for ethernet forward */
    uint32 map_flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP;

    int map_id;
    int inPcp;
    int rv = BCM_E_NONE;

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &map_id);
    if (rv != BCM_E_NONE) {
        printf("Error, QOS map create \n");
        return -1;
    }

    bcm_qos_map_t_init(&map);
    rv = bcm_qos_map_add(unit, map_flags, &map ,map_id);
    if (rv != BCM_E_NONE) {
        printf("Error, adding map");
        return -1;
    }

    for (inPcp = 0; inPcp < 8; inPcp++) {

        /* From input PCP value */
        map.remark_int_pri = inPcp; /* TC */
        map.color = bcmColorYellow; /* DP */

        /* To output DSCP value */
        map.dscp = inPcp; 

        rv = bcm_qos_map_add(unit, map_flags, &map ,map_id);
        if (rv != BCM_E_NONE) {
            printf("Error, adding mapping from PCP = %d to DSCP = %d\n", inPcp, inPcp/2);
            return -1;
        }
    }

    return map_id;
}


/* 
 * Sets the global tunnel information of tunnel 2 (only) for
 * mapping the dscp value instead of the default assigning.
 */
int set_tunnel_info_for_mapping() {
  ip_tunnel_glbl_info.tunnel_2.dscp = 0;
  ip_tunnel_glbl_info.tunnel_2.dscp_sel = bcmTunnelDscpMap;

  return BCM_E_NONE;
}



/*
 * buid two IP tunnels with following information:
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
 *      tunnel_gports[] array includes the gport-ids pointing to the IP tunnels
 */
int ipv4_tunnel_build_tunnels(int unit, int out_port, bcm_if_t* intf_ids, bcm_gport_t *tunnel_gports){
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
    bcm_tunnel_initiator_t tunnel_1;

    /* tunnel 1 info*/
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
    tunnel_1.dscp_sel = tunnel_info_1.dscp_sel; /* default: bcmTunnelDscpAssign */
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
    tunnel_2.dscp_sel = tunnel_info_2.dscp_sel; /* default: bcmTunnelDscpAssign */
    tunnel_itf2 = tunnel_gports[1];
    rv = add_ip_tunnel(unit,&tunnel_itf2,&tunnel_2);
    if (rv != BCM_E_NONE) {
        printf("Error, add_ip_tunnel 2\n");
    }
    if(verbose >= 1) {
        printf("created IP tunnel_2 on intf:0x%08x \n",tunnel_itf2);
    }

    /*** using egress object API set MAC address for tunnel 1 interface, without allocating FEC enty ***/
    flags1 |= BCM_L3_EGRESS_ONLY;
    rv = create_l3_egress(unit,flags1,out_port,0,tunnel_itf1,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    if(verbose >= 1) {
        printf("no FEC is allocated FEC-id =0x%08x, \n", fec[0]);
        printf("next hop mac at encap-id 0x%08x, \n", encap_id[0]);
    }

    /*** create egress object 2: points into tunnel 2, with allocating FEC, and da-mac = next_hop_mac2  ***/
    flags1 = 0;
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

/* 
 *  add IPv4 routes pointing to IP-tunnel
 *   - add host points directly to tunnel
 *   - add route entry points egress-object which points to tunnel.
 *  
 *   host:
 *      - DIP: 127.255.255.03
 *   route:
 *      - subnet: 127.255.255.00/28
 *  
 */
int ipv4_tunnel_add_routes(int unit, int in_port, int out_port, int* intf_ids){
    int vrf = 0;
    int host;
    int route;
    int mask; 
    int flags = 0;
    int in_vlan = 2; 
    int ing_intf_in;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;
    /* my-MAC  {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}; */
    bcm_mac_t mac_address  = ip_tunnel_my_mac_get();

    int rv;


    /*** create ingress router interface ***/
    rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan,0,mac_address, &ing_intf_in); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    rv = add_host_direct(unit, 0x7fffff03, vrf, intf_ids[0],out_port); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }

    if(verbose >= 1) {
        print_host("add entry ", host,vrf);
        printf("---> egress-tunnel=0x%08x, port=%d, \n", intf_ids[0], out_port);
    }


    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0; 

    rv = add_route(unit, route, mask , vrf, intf_ids[1]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }

    if(verbose >= 1) {
        print_route("add entry ", route,mask,vrf);
        printf("---> egress-object=0x%08x, port=%d, \n", intf_ids[1], out_port);
    }

    return rv;
}
 
