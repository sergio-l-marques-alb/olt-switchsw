/* 
 * $Id$
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
 * The cint works like cint_vswitch_cross_connect_p2p, but supports multi device operations. 
 * You can find full documentation in that cint. Below are only the differences for this cint. 
 *  Note that the functions supporting mim and protection are still in the cint, but they are
 *  not yet usable for multi device.
 *  
 * optional ports combinations: 
 * vlan - vlan 
 * vlan - mpls 
 * vlan - mim  - not implemented yet for multi device
 * mpls - mim  - not implemented yet for multi device
 *  
 * type: 1 = vlan, 2 = mpls 
 *  
 *  
 * run: 
 *      BCM> cint cint_mpls_lsr.c 
 *      BCM> cint cint_vswitch_metro_mp.c
 *      BCM> cint cint_advanced_vlan_translation_mode.c
 *      BCM> cint cint_port_tpid.c
 *      BCM> cint cint_multi_device_utils.c  
 *      BCM> cint cint_vswitch_cross_connect_p2p_multi_device.c 
 *      BCM> cint 
 *      cint> int nof_units = 2; 
 *      cint> int units[2] = {0, 2}; 
 *      cint> int sysport1, sysport2;
 *      cint> print port_to_system_port(unit1, <port_1>, &sysport1);
 *      cint> print port_to_system_port(unit2, <port_2>, &sysport2);
 *      cint> run(units, nof_units, <sysport1>, <type1>, <with_protection_1>, <second_port1>, <sysport2>, <type2>, <with_protection_2>, <second_port2>, <facility_protection>); 
 *  
 *  with_protection_1 and with_protection_2 can be only 0.
 *  
 *  second_port1, second_port2 will not be used at all
 *  
 * run packets: 
 * Same as original cint.
 */


uint8 pipe_mode_exp_set = 0;

int exp = 0;

bcm_gport_t vswitch_p2p_multi_device_port_id = 0;

int mpls_pipe_mode_exp_set(int unit) {

    int rv = BCM_E_NONE;

    if (pipe_mode_exp_set) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPipeTunnelLabelExpSet, pipe_mode_exp_set);
    }
    
    return rv;                
}

int verbose = 3;
int configuration_hub = 0; /* set the vlan port to be HUB */

int is_arad_plus(int unit, int *yesno)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *yesno = (((info.device == 0x8660) || (info.device == 0x8670)) ? 1 : 0);

    return rv;
}

struct cross_connect_info_s {

    /* physical ports */
    bcm_port_t sysport_1; 
    bcm_port_t sysport_2; 
    bcm_port_t sysport_3; 
    bcm_port_t sysport_4; 

    bcm_port_t sysport_5; /* for traverse */

    /* logical ports */
    bcm_gport_t vlan_port_1; /* vlan ports */
    bcm_gport_t vlan_port_2; 
    bcm_mim_port_t mim_port_1; /* mim port */
    bcm_mpls_port_t mpls_port_1; /* mpls ports */
    bcm_mpls_port_t mpls_port_2;

    /* logical ports for protection */
    bcm_gport_t vlan_port_3; 
    bcm_gport_t vlan_port_4; 
    bcm_mim_port_t mim_port_2;
    bcm_mpls_port_t mpls_port_3;
    bcm_mpls_port_t mpls_port_4;

    bcm_vswitch_cross_connect_t gports;
    /* 1+1 protection port*/
    bcm_gport_t prim_gport_1;

    /* data for vlan port */
    bcm_vlan_t outer_vlan_base;
    bcm_vlan_t eg_vlan_base;

    /* data for mim port */
    bcm_mim_vpn_config_t bvid_1; /* B-VIDs */
    bcm_mim_vpn_config_t bvid_2;
    bcm_mim_vpn_t bvid_base;
    int isid_base; /* I-SID */
    bcm_mac_t src_bmac; /* B-SA */
    bcm_mac_t dest_bmac; /* B-DA */
    
    /* data for mpls port */
    int in_vc_label_base; /* base for incomming VC label */
    int eg_vc_label_base; /* base for egress VC label */
    int vlan_base;
    int mpls_eg_vlan_base;
    uint8 my_mac[6];
    uint8 nh_mac[6]; /* next hop mac address */

    /* MC for 1+1 protection*/
    int protection_mc1;
    int protection_mc2;

    /* QOS example VPLS */
    int qos_example;
};

cross_connect_info_s cross_connect_info;

struct mpls_info_s {
    int l3_intf1;
    int l3_intf2;
    int tunnel_id;
    int fec;
    int encap_id;
    int mpls_id;
    int with_id;
};

void mpls_info_s_init(mpls_info_s *info){
    info->l3_intf1 = 0;
    info->l3_intf2 = 0;
    info->tunnel_id = 0;
    info->fec = 0;
    info->encap_id = 0;
    info->mpls_id = 0;
    info->with_id = 0;
}

void
cross_connect_info_init(int port1, int second_port1, int port2, int second_port2) {

    cross_connect_info.sysport_1 = port1; 
    cross_connect_info.sysport_2 = port2;
    cross_connect_info.sysport_3 = second_port1; 
    cross_connect_info.sysport_4 = second_port2;

    cross_connect_info.sysport_5 = 5;

    cross_connect_info.outer_vlan_base = 100;
    cross_connect_info.eg_vlan_base = 1000;

    cross_connect_info.bvid_base = 5;
    cross_connect_info.isid_base = 5555;

    cross_connect_info.src_bmac[0] = 0x01;
    cross_connect_info.src_bmac[1] = 0x02;
    cross_connect_info.src_bmac[2] = 0x03;
    cross_connect_info.src_bmac[3] = 0x04;
    cross_connect_info.src_bmac[4] = 0x05;
    cross_connect_info.src_bmac[5] = 0x06;

    cross_connect_info.dest_bmac[0] = 0xab;
    cross_connect_info.dest_bmac[1] = 0xcd;
    cross_connect_info.dest_bmac[2] = 0xef;
    cross_connect_info.dest_bmac[3] = 0x12;
    cross_connect_info.dest_bmac[4] = 0x34;
    cross_connect_info.dest_bmac[5] = 0x12;

    cross_connect_info.in_vc_label_base = 20;
    cross_connect_info.eg_vc_label_base = 40; 
    cross_connect_info.vlan_base = 10;
    cross_connect_info.mpls_eg_vlan_base = 20;

    cross_connect_info.my_mac[0] = 0x00;
    cross_connect_info.my_mac[1] = 0x00;
    cross_connect_info.my_mac[2] = 0x00;
    cross_connect_info.my_mac[3] = 0x00;
    cross_connect_info.my_mac[4] = 0x00;
    cross_connect_info.my_mac[5] = 0x11;

    cross_connect_info.nh_mac[0] = 0x00;
    cross_connect_info.nh_mac[1] = 0x00;
    cross_connect_info.nh_mac[2] = 0x00;
    cross_connect_info.nh_mac[3] = 0x00;
    cross_connect_info.nh_mac[4] = 0x00;
    cross_connect_info.nh_mac[5] = 0x22;

    cross_connect_info.protection_mc1 = 6001;
    cross_connect_info.protection_mc2 = 6002;
    
}

/* create vlan_port (Logical interface identified by port-vlan-vlan) */
int
vlan_port_create(
    int unit,
    bcm_port_t port_id,  
    bcm_vlan_t match_vlan, /* outer vlan */
    bcm_vlan_t egress_vlan,
    uint32 flags,
    bcm_gport_t *gport,
    int failover_id,
    bcm_gport_t protect_gport,
    int protect_mc
    ){

    int rv;
    bcm_vlan_port_t vp;

    rv = bcm_vlan_create(unit, match_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", match_vlan);
    }

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = match_vlan; 
    vp.egress_vlan = egress_vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp.failover_id = failover_id;
    vp.failover_mc_group = protect_mc;
    vp.failover_port_id = protect_gport;
    vp.flags = flags;
    vp.flags |= configuration_hub ? BCM_VLAN_PORT_NETWORK : 0;
    vp.vlan_port_id = *gport;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if(verbose >= 2) {
        printf("created vlan-port:0x0%8x  \n", vp.vlan_port_id);
        printf("  encap-id: 0x%8x  \n", vp.encap_id);
        printf("  in-port: %d\n", vp.port);
        printf("  in-vlan: %d\n", vp.match_vlan);
    }

    /* return port id to user */

    *gport = vp.vlan_port_id;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    } else {

        /* set vlan port membership */
        rv = bcm_vlan_gport_add(unit, match_vlan, port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add with vlan %d\n", match_vlan);
            print rv;
            return rv;
        }
          
        rv = bcm_vlan_create(unit, egress_vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_create with vlan %d continue...\n", egress_vlan);
        }

        rv = bcm_vlan_gport_add(unit, egress_vlan, port_id, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add with vlan %d\n", egress_vlan);
            print rv;
            return rv;
        }

    }

    return BCM_E_NONE;
}


/*
 * create l3 interface - ingress
 *
 */
int 
create_l3_intf(int unit, int flags, int port, int vlan, bcm_mac_t my_mac_addr, int *intf) {

    int rc;
    bcm_l3_intf_t l3if;
    bcm_l3_intf_t_init(l3if);
    
    sal_memcpy(l3if.l3a_mac_addr, my_mac_addr, 6);
    l3if.l3a_vid = vlan;
    l3if.l3a_ttl = 31;
    l3if.l3a_mtu = 1524;
    l3if.l3a_intf_id = *intf;
    l3if.l3a_flags = flags;

    if ((flags & BCM_L3_WITH_ID) != 0) {
        l3if.l3a_intf_id = l3if.l3a_vid;
    }
    
    rc = bcm_l3_intf_create(unit, l3if);
    
    return rc;
}

/*
 * create l3 interface - egress
 *
 */
int 
create_l3_egress(int unit, uint32 flags, int port, int vlan, int ingress_intf, bcm_mac_t nh_mac_addr, int *intf, int *encap_id) {

    int rc;
    bcm_l3_egress_t l3eg;
    bcm_l3_egress_t_init(l3eg);
    
    bcm_if_t l3egid;
    
    int mod = 0;
    int test_failover_id = 0;
    int test_failover_intf_id = 0;
    
    l3eg.intf = ingress_intf;
    
    sal_memcpy(l3eg.mac_addr, nh_mac_addr, 6);
    l3eg.vlan   = vlan;
    l3eg.module = mod;
    l3eg.port   = port;
    l3eg.failover_id = test_failover_id;
    l3eg.failover_if_id = test_failover_intf_id;
    l3eg.encap_id = *encap_id;
    l3egid = *intf; 
    
    rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    print unit;
    print rc;
    
    *encap_id = l3eg.encap_id;
    *intf = l3egid;
    
    return rc;
}
/* initialize the tunnels for mpls routing */
int
mpls_tunnels_config(
    int unit,
    bcm_port_t port,
    mpls_info_s *info,
    int in_tunnel_label,
    int out_tunnel_label){

    int ingress_intf;
    int encap_id;
    bcm_mpls_egress_label_t label_array[2];
    int rv;
    int flags;
    bcm_vlan_t vlan, eg_vlan;
    int is_arad_plus;

    rv = is_arad_plus(unit, &is_arad_plus);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }
 
    /* open vlan */
    vlan = cross_connect_info.vlan_base;
    printf("open vlan %d in unit %d\n", vlan, unit);
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    if (info->with_id){
        flags = BCM_VLAN_PORT_WITH_ID;
    } else {
        flags = 0;
    }

    rv = bcm_vlan_gport_add(unit, vlan, port, flags);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        print rv;
        return rv;
    }

    if (info->with_id){
        flags = BCM_L3_WITH_ID;
    } else {
        flags = 0;
    }
 
    /* l2 termination for mpls routing: packet received on those VID+MAC will be L2 terminated  */
    rv = create_l3_intf(unit, flags, port, vlan, cross_connect_info.my_mac, &(info->l3_intf1));    
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf\n");
        print rv;
        return rv;
    }

    /* create ingress object, packet will be routed to */
    eg_vlan = cross_connect_info.mpls_eg_vlan_base;
    rv = create_l3_intf(unit, flags /* Still set from before */, port, eg_vlan, cross_connect_info.my_mac, &(info->l3_intf2));
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_intf\n");
        print rv;
        return rv;
    }
  
    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].exp = exp; 
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[0].label = in_tunnel_label;
    label_array[0].ttl = 20;
    label_array[0].tunnel_id = info->tunnel_id;
    if (cross_connect_info.qos_example) {
        label_array[0].qos_map_id = qos_map_id_vpls_remark_egress_get(unit);
    }
    
    label_array[1].exp = exp; 
    label_array[1].flags = (BCM_MPLS_EGRESS_LABEL_TTL_SET|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_arad_plus || pipe_mode_exp_set) {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    }
    label_array[1].label = out_tunnel_label;
    label_array[1].ttl = 40;
    label_array[1].l3_intf_id = info->l3_intf2;
    label_array[1].tunnel_id = info->tunnel_id;

    rv = bcm_mpls_tunnel_initiator_create(unit,0,2,label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        print rv;
        return rv;
    }
  
    /* create egress object points to this tunnel/interface */
    info->tunnel_id = label_array[0].tunnel_id;
    if (info->with_id){
        flags = BCM_L3_WITH_ID;
    } else {
        flags = 0;
    }
    rv = create_l3_egress(unit, flags, port, eg_vlan, info->tunnel_id, cross_connect_info.nh_mac, &(info->fec), &(info->encap_id));
    if (rv != BCM_E_NONE) {
        printf("Error, in create_l3_egress\n");
        return rv;
    }
  
    return rv;
}

int 
mpls_port_create(
    int unit, 
    bcm_port_t port, 
    uint32  flags,
    mpls_info_s *info,
    bcm_mpls_port_t *mpls_port,
    int failover_id,
    bcm_gport_t protect_gport) {

    int rv;
    int egress_intf;
    int in_vc_label = cross_connect_info.in_vc_label_base;
    int eg_vc_label = cross_connect_info.eg_vc_label_base;
    
    rv = mpls_tunnels_config(unit, port, info, in_vc_label, eg_vc_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config\n");
        return rv;
    }

    /* add port, according to VC label */
  
    /* set port attribures */
    mpls_port->mpls_port_id = info->mpls_id;
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = in_vc_label;
    mpls_port->egress_tunnel_if = info->fec;
    mpls_port->flags = flags | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port->port = port;
    mpls_port->egress_label.label = eg_vc_label;    
    mpls_port->failover_id = failover_id;
    mpls_port->failover_port_id = protect_gport;
    if (configuration_hub) {
        mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
    }
  
    /* to create a p2p mpls port that is not connected to p2p vpn - use vpn=0 */
    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }

    info->mpls_id = mpls_port->mpls_port_id;
    vswitch_p2p_multi_device_port_id = info->mpls_id; /* save mpls_port id for global usage*/
    

    printf("mpls_port added in unit %d. mpls_port_id is: %d\n", unit, info->mpls_id);

    return BCM_E_NONE;
}

int 
mim_l2_station_add(
    int unit,
    int global, /* global configuration of per port */
    int port,
    bcm_mac_t mac
    ) {

    int rv, station_id;
    bcm_l2_station_t station; 

    bcm_l2_station_t_init(&station); 

    station.flags = BCM_L2_STATION_MIM;
    sal_memcpy(station.dst_mac, mac, 6);

    if (global) { /* global configuration */
        station.src_port_mask = 0; /* port is not valid */
        station.dst_mac_mask[0] = 0xff; /* dst_mac MSB should be used */
        station.dst_mac_mask[1] = 0xff;
        station.dst_mac_mask[2] = 0xff;
        station.dst_mac_mask[3] = 0xff;
        station.dst_mac_mask[4] = 0xff;
        station.dst_mac_mask[5] = 0x0;
    }
    else { /* per port configuration */
        station.src_port = port;
        station.src_port_mask = -1; /* port is valid */
        station.dst_mac_mask[0] = 0x0; /* dst_mac LSB should be used */
        station.dst_mac_mask[1] = 0x0;
        station.dst_mac_mask[2] = 0x0;
        station.dst_mac_mask[3] = 0x0;
        station.dst_mac_mask[4] = 0x0;
        station.dst_mac_mask[5] = 0xff;
    }

    rv = bcm_l2_station_add(unit, &station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        print rv;
    }
    return BCM_E_NONE;
}

int
mim_bvid_config(
    int unit,
    bcm_mim_vpn_config_t *vpn_config, 
    int vpn_id /* VPN ID */ 
    ) {

    int rv;
    bcm_multicast_t mc_group; /* mc_group = bc_group = uc_group */

    bcm_mim_vpn_config_t_init(vpn_config);

    vpn_config->flags |= BCM_MIM_VPN_WITH_ID; 
    vpn_config->flags |= BCM_MIM_VPN_BVLAN; /* set vpn as B-VID */
    vpn_config->vpn = vpn_id;

    /* open multicast group for the VPN */
    mc_group = vpn_id + 12*1024;  /* for B-VID mc_group = vpn + 12k */

    /* destroy before open, to ensure group does not exist */
    rv = bcm_multicast_destroy(unit, mc_group);
    if (rv != BCM_E_NONE && rv != BCM_E_NOT_FOUND) {
        printf("Error, bcm_multicast_destroy, mc_group $mc_group\n");
        print rv;
        return rv;
    }

    /* create ingress MC group of type mim */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_TYPE_MIM, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create, mc_group $mc_group\n");
        print rv; 
        return rv;
    }
    
    vpn_config->broadcast_group = mc_group;
    vpn_config->unknown_unicast_group = mc_group;
    vpn_config->unknown_multicast_group = mc_group;

    return BCM_E_NONE;
}

int
mim_bvid_create(
    int unit,
    bcm_mim_vpn_config_t *vpn_config,
    int vpn_id, /* VPN ID */ 
    bcm_pbmp_t pbmp, /* port bit map */
    bcm_pbmp_t ubmp /* port bit map for untagged ports */
    ) {

    int rv;

    rv = mim_bvid_config(unit, vpn_config, vpn_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_config\n");
        return rv;
    }

    /* create the B-VID */
    rv = bcm_mim_vpn_create(unit, vpn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_vpn_create\n");
        print rv;
        return rv;
    }
  
    /* set the B-VID port membership */
    rv = bcm_vlan_port_add(unit, vpn_config->vpn, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

/* configure bcm_mim_port_t with the given parameters */
void
mim_port_config(
    bcm_mim_port_t *mim_port, 
    int port, /* Physical port */
    uint16 vpn, /* B-VID */
    bcm_mac_t src_mac, /* In Source MAC address */ 
    uint16 bvid, /* Out B-VID */
    bcm_mac_t dest_mac, /* Out Dest MAC address */
    uint32 isid /* Egress I-SID - relevant for P2P ports only */ 
    ) {

    bcm_mim_port_t_init(mim_port);

    mim_port->flags = 0;
    mim_port->port = port;
    mim_port->match_tunnel_vlan = vpn;
    sal_memcpy(mim_port->match_tunnel_srcmac, src_mac, 6);
    mim_port->egress_tunnel_vlan = bvid; 
    sal_memcpy(mim_port->egress_tunnel_dstmac, dest_mac, 6);
    mim_port->egress_tunnel_service = isid;
}

int 
mim_port_create(
    int unit, 
    bcm_port_t port,
    bcm_mim_port_t *mim_port,
    bcm_mim_vpn_config_t *bvid, /* in and out B-VID */
    int isid, /* 2 mim ports cannot have the same I-SID */
    int failover_id,
    bcm_gport_t protect_gport
    ) {

    int rv;
    bcm_pbmp_t pbmp, ubmp;
    bcm_vlan_action_set_t action;

    /* set physical port as mim (backbone) port */
    rv = bcm_port_control_set(unit, port, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set\n");
        print rv;
        return rv;
    }

    /* make the mim physical port recognize the B-tag ethertype */
    rv = bcm_port_tpid_set(unit, port, 0x81a8);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_set with port_1\n");
        print rv;
        return rv;
    }

    /* set MiM port ingress vlan-editing to remove B-tag if exists */
    rv = bcm_vlan_port_default_action_set(unit, port, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_tpid_set with port_2\n");
        print rv;
        return rv;
    }

    /* prepare the port bit maps to set the B-VID port membership, with the the backbone port (used for filtering) */ 
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_PORT_ADD(pbmp, port);

    bcm_mim_vpn_config_t_init(bvid);

    /* create B-VID */
    rv = mim_bvid_create(unit, bvid, cross_connect_info.bvid_base++, pbmp, ubmp); 
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with bvid\n");
        return rv;
    }

    /* create MiM (backbone) P2P port */
    mim_port_config(mim_port, port, bvid->vpn, cross_connect_info.src_bmac, bvid->vpn, cross_connect_info.dest_bmac, 
                    isid /* I-SID for P2P port */);
    
    /* add protection info */
    mim_port->failover_id = failover_id;
    mim_port->failover_gport_id = protect_gport; 

    rv = bcm_mim_port_add(unit, 0xffff, mim_port); /* use invalid VPN (=0xffff) to create P2P port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int 
mim_init(int unit) {

    int rv;
    bcm_pbmp_t pbmp, ubmp;

    /* init mim on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init\n");
        print rv;
        return rv;
    }

    /* set the global and per port L2 station for the B-DA (MyMac LSB + MSB) */
    rv = mim_l2_station_add(unit, 1 /* global */, 0, cross_connect_info.dest_bmac);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global)\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}

int
aux_run(int *units_ids, int nof_units, int port1, int type1, int protection1, int second_port1 /* used if protection1 is set */, 
    int port2, int type2, int protection2, int second_port2 /* used if protection2 is set */, 
    int facility /* facility or path protection type */, int pipe_mode_exp, int expected) {

    pipe_mode_exp_set = pipe_mode_exp;

    exp = expected;


    return run(units_ids, nof_units, port1,type1, protection1, second_port1 ,port2, type2, protection2,second_port2 , facility);
}


int 
run(int *units_ids, int nof_units, int port1, int type1, int protection1, int second_port1 /* used if protection1 is set */, 
    int port2, int type2, int protection2, int second_port2 /* used if protection2 is set */, 
    int facility /* facility or path protection type */) {

    int rv;
    bcm_failover_t failover_id_1 = 0, failover_id_2 = 0;
    int i, j, unit, port, flags, failover_flags;
    mpls_info_s mpls_info;


    for (i = 0 ; i < nof_units ; i++) {
        rv = mpls_pipe_mode_exp_set(units_ids[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_pipe_mode_exp_set\n");
            return rv;
        }
    }

    cross_connect_info_init(port1, second_port1, port2, second_port2);  

    /* Check valid inputs*/
    if (units_ids == NULL){
        printf("units ids array is null\n");
        return BCM_E_UNIT;
    }

    if (nof_units < 1){
        printf("nof_units must be at least one\n");
        return BCM_E_PARAM;
    }

    if (protection2 == 2) {
        printf("Error, 1+1 protection is supported only for fisrt port \n");
        return BCM_E_PARAM;
    }

    if (type1 == 3 || type2 == 3) { /* mim */
        /* init mim on the device */
        rv = mim_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, mim_init\n");
            return rv;
        }
    }

    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    if (advanced_vlan_translation_mode && type1 == 1 || type2 == 1) {
        port = cross_connect_info.sysport_1;
            
        for (i = 0 ; i < 2 ; i++) {
            units_array_make_local_first(units_ids, nof_units, port);
            for (j = 0 ; j < nof_units; j++) {
                unit = units_ids[j];
                port_tpid_init(port, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with port_1\n");
                    print rv;
                    return rv;
                }
            }
            port = cross_connect_info.sysport_2;
        }

        for (i = 0 ; i < nof_units ; i++) {
            unit = units_ids[i];
            rv = vlan_translation_default_mode_init(unit);
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_translation_default_mode_init\n");
                return rv;
            }
        }

         
    }

    if (protection1 == 1) {
        if (facility) {
            failover_id_1 = BCM_FAILOVER_ID_LOCAL;
        }
        else {
            /* create failover_id for port 1 */
            rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &failover_id_1);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create (failover_id_1)\n");
                print rv;
                return rv;
            }

            if(verbose >= 1) {
                printf("created failover_id_1 0x0%8x\n", failover_id_1);
            }
        }
    }
    /* 1+1 protection */
    else if (protection1 == 2) {
        /* create MC for protection */
        open_ingress_mc_group_update_id(unit,&cross_connect_info.protection_mc1);
        if(verbose >= 1) {
            printf("created protection_mc1 : 0x0%8x\n", cross_connect_info.protection_mc1);
        }
    }
    else{
        cross_connect_info.protection_mc1 = 0;
    }


    if (protection2) {
        if (facility) {
            failover_id_2 = BCM_FAILOVER_ID_LOCAL;
        }
        else {
            if (protection1 == 1) { /* 1:1 Protection */
                failover_flags = BCM_FAILOVER_FEC;
            } else {                /* 1+1 Protection */
                failover_flags = BCM_FAILOVER_INGRESS;
            }

            /* create failover_id for port 2 */
            rv = bcm_failover_create(unit, failover_flags, &failover_id_2);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_failover_create (failover_id_2)\n");
                print rv;
                return rv;
            }
            if(verbose >= 1) {
                printf("created failover_id_2 : 0x0%8x\n", failover_id_2);
            }

        }
    }
    /* 1+1 protection */
    else if (protection2 == 2) {
        /* create MC for protection */
        open_ingress_mc_group_update_id(unit,&cross_connect_info.protection_mc2);
        if(verbose >= 1) {
            printf("created protection_mc2 : 0x0%8x\n", cross_connect_info.protection_mc2);
        }

    }
    else{
        cross_connect_info.protection_mc2 = 0;
    }
        
    switch(type1) {
    case 1:
        /* create vlan port */
        units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_1);
        cross_connect_info.vlan_port_1 = 0;
        flags = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = vlan_port_create(unit, 
                                  cross_connect_info.sysport_1, 
                                  cross_connect_info.outer_vlan_base, 
                                  cross_connect_info.eg_vlan_base, 
                                  flags,
                                  &(cross_connect_info.vlan_port_1), 
                                  failover_id_1, 
                                  0,
                                  cross_connect_info.protection_mc1
                                  );
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_port_create with unit %d and port 0x%x\n", unit, cross_connect_info.sysport_1);
                return rv;
            }
            if(verbose >= 1) {
                printf("created vlan_port_1 : 0x0%8x in unit %d\n", cross_connect_info.vlan_port_1, unit);
            }
            flags |= BCM_VLAN_PORT_WITH_ID;  /* After first iteration, use the same gport*/
        }
        /* Advance vlans*/
        cross_connect_info.outer_vlan_base++;
        cross_connect_info.eg_vlan_base++;

        if (protection1 == 1 || protection1== 2) {
            units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_3);
            cross_connect_info.vlan_port_3 = 0;
            flags = 0;
            for (i = 0 ; i < nof_units ; i++){
                unit = units_ids[i];
                /* create another vlan port - use first port as protection */
               rv = vlan_port_create(unit, 
                                     cross_connect_info.sysport_3, 
                                     cross_connect_info.outer_vlan_base, 
                                     cross_connect_info.eg_vlan_base, 
                                     flags,
                                     &(cross_connect_info.vlan_port_3), 
                                     failover_id_1, 
                                     cross_connect_info.vlan_port_1,
                                     cross_connect_info.protection_mc1
                                     );
                if (rv != BCM_E_NONE) {
                    printf("Error, in vlan_port_create with unit %d and port 0x%x\n", unit, cross_connect_info.sysport_3);
                    return rv;
                }
                if(verbose >= 1) {
                    printf("created vlan_port_3 (protection for 1) : 0x0%8x in unit %d\n", cross_connect_info.vlan_port_3, unit);
                }
                flags |= BCM_VLAN_PORT_WITH_ID;
            }
            /* Advance vlans*/
            cross_connect_info.outer_vlan_base++;
            cross_connect_info.eg_vlan_base++;

            cross_connect_info.gports.port1 = cross_connect_info.vlan_port_3;

            /* 1+1 protection */
            if(protection1 == 2) {
                cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;
                cross_connect_info.prim_gport_1 = cross_connect_info.vlan_port_3;

                /* add gport to protection MC */
                rv = multicast_vlan_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.sysport_1,cross_connect_info.vlan_port_1);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast_vlan_port_add with port 0x%x\n", cross_connect_info.sysport_1);
                    return rv;
                }
                rv = multicast_vlan_port_add(unit,cross_connect_info.protection_mc1,cross_connect_info.sysport_3,cross_connect_info.vlan_port_3);
                if (rv != BCM_E_NONE) {
                    printf("Error, in multicast_vlan_port_add with port 0x%x\n", cross_connect_info.sysport_1);
                    return rv;
                }
            }
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;
        }
        break;

    case 2:
        /* create mpls port */
        units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_1);
        mpls_info_s_init(&mpls_info);
        bcm_mpls_port_t_init(cross_connect_info.mpls_port_1);
        flags = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = mpls_port_create(unit, 
                                  cross_connect_info.sysport_1, 
                                  flags,
                                   &mpls_info,
                                  &(cross_connect_info.mpls_port_1), 
                                  failover_id_1, 
                                  0);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with unit %d and port 0x%x\n", unit, cross_connect_info.sysport_1);
                return rv;
            }
           mpls_info.with_id = 1;
           flags |= BCM_MPLS_PORT_WITH_ID;
        }
        cross_connect_info.in_vc_label_base++;
        cross_connect_info.eg_vc_label_base++;
        cross_connect_info.vlan_base++;
        cross_connect_info.mpls_eg_vlan_base++;

        mpls_info_s_init(&mpls_info);
        flags = 0;

        if (protection1) {
            /* create another mpls port - use first port as protection */
           rv = mpls_port_create(unit, 
                                 cross_connect_info.sysport_3,
                                 flags,
                                 &mpls_info, 
                                 &(cross_connect_info.mpls_port_3), 
                                 failover_id_1, 
                                 cross_connect_info.mpls_port_1.mpls_port_id);

            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with port %d\n", cross_connect_info.sysport_3);
                return rv;
            }

            cross_connect_info.in_vc_label_base++;
            cross_connect_info.eg_vc_label_base++;
            cross_connect_info.vlan_base++;
            cross_connect_info.mpls_eg_vlan_base++;


            cross_connect_info.gports.port1 = cross_connect_info.mpls_port_3.mpls_port_id;
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.mpls_port_1.mpls_port_id;
        }
        break;

    case 3:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.sysport_1, 
                             &(cross_connect_info.mim_port_1), 
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++, 
                             failover_id_1, 
                             0);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.sysport_1);
            return rv;
        }

        if (protection1) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.sysport_3, 
                                 &(cross_connect_info.mim_port_2), 
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 failover_id_1, 
                                 cross_connect_info.mim_port_1.mim_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.sysport_3);
                return rv;
            }

            cross_connect_info.gports.port1 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port1 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    default:
        printf("Error, type1 cannot be <1 or >3\n");
        return BCM_E_PARAM;
    }

    switch(type2) {
    case 1:
        /* create vlan port */
        units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_2);
        cross_connect_info.vlan_port_2 = 0;
        flags = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = vlan_port_create(unit, 
                                  cross_connect_info.sysport_2, 
                                  cross_connect_info.outer_vlan_base, 
                                  cross_connect_info.eg_vlan_base, 
                                  flags,
                                  &(cross_connect_info.vlan_port_2), 
                                  failover_id_2, 
                                  0,
                                  cross_connect_info.protection_mc2
                                  );
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_port_create with unit %d and port 0x%x\n", unit, cross_connect_info.sysport_2);
                return rv;
            }
            if(verbose >= 1) {
                printf("created vlan_port_2 : 0x0%8x in unit %d\n", cross_connect_info.vlan_port_2, unit);
            }
            flags |= BCM_VLAN_PORT_WITH_ID;
        }
        /* Advance vlans*/
        cross_connect_info.outer_vlan_base++;
        cross_connect_info.eg_vlan_base++;

        if (protection2) {
            /* create another vlan port - use first port as protection */
            units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_4);
            cross_connect_info.vlan_port_4 = 0;
            flags = 0;
            for (i = 0 ; i < nof_units  ; i++){
                unit = units_ids[i];
               rv = vlan_port_create(unit, 
                                     cross_connect_info.sysport_4, 
                                     cross_connect_info.outer_vlan_base, 
                                     cross_connect_info.eg_vlan_base, 
                                     flags,
                                     &(cross_connect_info.vlan_port_4), 
                                     failover_id_2, 
                                     cross_connect_info.vlan_port_2,
                                     cross_connect_info.protection_mc2
                                     );
                if (rv != BCM_E_NONE) {
                    printf("Error, in vlan_port_create with unit %d port 0x%x\n", unit, cross_connect_info.sysport_4);
                    return rv;
                }
                flags |= BCM_VLAN_PORT_WITH_ID;
            }
            /* Advance vlans*/
            cross_connect_info.outer_vlan_base++;
            cross_connect_info.eg_vlan_base++;

            cross_connect_info.gports.port2 = cross_connect_info.vlan_port_4;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.vlan_port_2;
        }
        break;

    case 2:
        /* create mpls port */
        units_array_make_local_first(units_ids, nof_units, cross_connect_info.sysport_2);
        mpls_info_s_init(&mpls_info);
        bcm_mpls_port_t_init(cross_connect_info.mpls_port_2);
        cross_connect_info.mpls_port_2.flags |= BCM_MPLS_PORT_NETWORK; /* Second port should be HUB for the case it's PWE2PWE */
        flags = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = mpls_port_create(unit, 
                                  cross_connect_info.sysport_2, 
                                  flags,
                                   &mpls_info,
                                  &(cross_connect_info.mpls_port_2), 
                                  failover_id_2, 
                                  0);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with unit %d and port 0x%x\n", unit, cross_connect_info.sysport_2);
                return rv;
            }
           mpls_info.with_id = 1;
           flags |= BCM_MPLS_PORT_WITH_ID;
        }
        cross_connect_info.in_vc_label_base++;
        cross_connect_info.eg_vc_label_base++;
        cross_connect_info.vlan_base++;
        cross_connect_info.mpls_eg_vlan_base++;

        mpls_info_s_init(&mpls_info);
        flags = 0;

        if (protection2) {
            /* create another mpls port - use first port as protection */
           rv = mpls_port_create(unit, 
                                 cross_connect_info.sysport_4,
                                 flags,
                                 &mpls_info, 
                                 &(cross_connect_info.mpls_port_4), 
                                 failover_id_2, 
                                 cross_connect_info.mpls_port_2.mpls_port_id);

            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create with port %d\n", cross_connect_info.sysport_3);
                return rv;
            }

            cross_connect_info.in_vc_label_base++;
            cross_connect_info.eg_vc_label_base++;
            cross_connect_info.vlan_base++;
            cross_connect_info.mpls_eg_vlan_base++;


            cross_connect_info.gports.port2 = cross_connect_info.mpls_port_4.mpls_port_id;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.mpls_port_2.mpls_port_id;
        }
        break;

    case 3:
        /* create mim port */
        rv = mim_port_create(unit, 
                             cross_connect_info.sysport_2, 
                             &(cross_connect_info.mim_port_1),
                             &(cross_connect_info.bvid_1),
                             cross_connect_info.isid_base++,  
                             failover_id_2, 
                             0);
        if (rv != BCM_E_NONE) {
            printf("Error, in mim_port_create with port %d\n", cross_connect_info.sysport_2);
            return rv;
        }

        if (protection2) {
            /* create another mim port - use first port as protection */
            rv = mim_port_create(unit, 
                                 cross_connect_info.sysport_4, 
                                 &(cross_connect_info.mim_port_2),
                                 &(cross_connect_info.bvid_2),
                                 cross_connect_info.isid_base++, 
                                 failover_id_2, 
                                 cross_connect_info.mim_port_1.mim_port_id);
            if (rv != BCM_E_NONE) {
                printf("Error, in mim_port_create with port %d\n", cross_connect_info.sysport_4);
                return rv;
            }

            cross_connect_info.gports.port2 = cross_connect_info.mim_port_2.mim_port_id;
        }
        else {
            cross_connect_info.gports.port2 = cross_connect_info.mim_port_1.mim_port_id;
        }
        break;
    default:
        printf("Error, type2 cannot be <1 or >3\n");
        return BCM_E_PARAM;
    }

    /* cross connect the 2 ports */
    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", cross_connect_info.gports.port1, cross_connect_info.gports.port2);
    }
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vswitch_cross_connect_add in unit %d\n", unit);
            print rv;
            return rv;
        }

        /* 1+1 connect both protection & primary to another side */
        if (protection1 == 2) {

            bcm_vswitch_cross_connect_t gports2;
            gports2.port1 = cross_connect_info.prim_gport_1;
            gports2.port2 = cross_connect_info.gports.port2;

            if(verbose >= 1) {
                printf("connect port1:0x0%8x with port2:0x0%8x \n", gports2.port1, gports2.port2);
            }
            rv = bcm_vswitch_cross_connect_add(unit, &gports2);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_vswitch_cross_connect_add\n");
                print rv;
                return rv;
            }
        }
    }
    return BCM_E_NONE;
}

int
run_with_defaults(){
    int units[2] = {0, 2};
    int nof = 2;
    int ports[2] = {13, 13};
    int gports[2];
    int sysports[2];
    int modid;
    int rv, i;

    for (i = 0 ; i < nof ; i++){

        rv = bcm_stk_modid_get(units[i], &modid);

        BCM_GPORT_MODPORT_SET(gports[i], modid, ports[i]);

        printf("Modport %d is: 0x%x\n", i, gports[i]);

        rv = bcm_stk_gport_sysport_get(units[i], gports[i], &(sysports[i]));

        printf("Sysport %d is: 0x%x\n", i, sysports[i]);

        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_gport_sysport_get\n");
            print rv;
            return rv;
        }
    }

    return run(units,nof, sysports[0], 2, 0, sysports[0], sysports[1], 1, 0, sysports[1], 0); 
}


void
print_gport(bcm_gport_t gport) {
    if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        printf("MPLS port id = %d\n", BCM_GPORT_MPLS_PORT_ID_GET(gport));
    }
    else if (BCM_GPORT_IS_MIM_PORT(gport)) {
        printf("MIM port id = %d\n", BCM_GPORT_MIM_PORT_ID_GET(gport));
    }
    else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        printf("VLAN port id = %d\n", BCM_GPORT_VLAN_PORT_ID_GET(gport));
    }
    else {
        print gport;
    }
}

/* call back for traverse */
int
call_back(int unit,  bcm_vswitch_cross_connect_t *t, int* ud) {
    printf("traverse call back no. %d\n", (*ud)++);
    printf("port 1: ");
    print_gport(t->port1);
    printf("port 2: ");
    print_gport(t->port2);
    printf("\n");
    return BCM_E_NONE;
}

int
traverse_run(int unit) {

    int rv, traverse_no = 1;

    /* add another vlan port, that is not cross-connected to any port */
    rv = vlan_port_create(unit, 
                          cross_connect_info.sysport_5, 
                          cross_connect_info.outer_vlan_base++, 
                          cross_connect_info.eg_vlan_base++, 
                          0
                          &(cross_connect_info.vlan_port_2), 
                          0, 
                          0,
                          0,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_create with port 0x%x\n", cross_connect_info.sysport_5);
        return rv;
    }

    /* traverse - print all the cross-connected ports */
    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
delete_all(int unit) {

    int rv, traverse_no = 1;

    rv = bcm_vswitch_cross_connect_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_delete_all\n");
        print rv;
        return rv;
    }

    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* call bcm_qos_port_map_set
   In order to use the globally configured port_id, pass port==0 */
int qos_port_map_set(int unit, bcm_gport_t port, int ing_map, int egr_map){

    if (!port) {
        port = vswitch_p2p_multi_device_port_id;
    }

    return bcm_qos_port_map_set(unit, port, ing_map, egr_map);

}


 

