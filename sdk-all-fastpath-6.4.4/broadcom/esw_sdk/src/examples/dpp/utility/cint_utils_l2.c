/* $Id: cint_utils_l2.c,v 1.10 Broadcom SDK $
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
 * This file provides L2 basic functionality and defines L2 global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */
bcm_mac_t g_l2_global_mac_utils    = {0x00, 0x11, 0x00, 0x00, 0x00, 0x00};

uint8 src_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x12, 0x55};
uint8 dest_mac_address[6] = {0x00, 0x00, 0x00, 0x00, 0x44, 0x88};

static int OLP_DEFAULT_CORE = 0;

/* *************************************************************************************************** */


struct l2__mact_properties_s
{
    int gport_id;
    bcm_mac_t mac_address;
    bcm_vlan_t vlan;
};
    
struct l2__arp_entry_properties_s
{
    uint32 flags;               /* BCM_L2_EGRESS_XXX flags. */
    bcm_mac_t dest_mac;         /* Destination MAC address to replace with. */
    bcm_mac_t src_mac;          /* Source MAC address to replace with. */
    bcm_vlan_t outer_vlan;      /* outer vlan tag to replace with. */
    bcm_gport_t dest_port;      /* Destination gport to associate this l2 egress
                                   entry with */
    bcm_ethertype_t ethertype;  /* Ethertype to replace with. */
    bcm_if_t encap_id;          /* Encapsulation index */
    bcm_if_t l3_intf;           /* L3 interface (tunnel). Used only in Native
                                   Routing overlay protocols. */
};

/*
 *  Setting  Global 36 MSB bits of my-MAC
 */
int l2__global_mac__set(int unit, bcm_mac_t my_global_mac)
{
    int rc, station_id;
    bcm_l2_station_t station;
    
    /* Intitialize an L2 station structure */
    bcm_l2_station_t_init(&station);
    
    if (my_global_mac == NULL)
        sal_memcpy(station.dst_mac, g_l2_global_mac_utils, 6);
    else
        sal_memcpy(station.dst_mac, my_global_mac, 6);


    station.src_port_mask = 0;      /* port is not valid */
    station.dst_mac_mask[0] = 0xff; /* Setting global my-MAC (36 MSB bits) */
    station.dst_mac_mask[1] = 0xff;
    station.dst_mac_mask[2] = 0xff;
    station.dst_mac_mask[3] = 0xff;
    station.dst_mac_mask[4] = 0xff;
    station.dst_mac_mask[5] = 0xff;

    rc = bcm_l2_station_add(unit, &station_id, &station);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l2_station_add\n");
        print rc;
    }
    return BCM_E_NONE;
}

/*
 *  Setting vsi 12 LSB bits of my-MAC
 */
int l2__my_mac_ipv4_vsi__set(int unit, bcm_mac_t my_lsb_mac, int vsi)
{
    int rc, station_id;
    bcm_l2_station_t station;
    
    /* Intitialize an L2 station structure */
    bcm_l2_station_t_init(&station); 
      
    /* set my-Mac LSB */
    station.flags = BCM_L2_STATION_IPV4;
    sal_memcpy(station.dst_mac, my_lsb_mac, 6);

    station.src_port_mask = 0; /* port is not valid */
    station.vlan = vsi;
    station.vlan_mask = 0xffff; /* vsi is  valid */
    station.dst_mac_mask[0] = 0x00; /* setting my-Mac (12 LSB bits) */
    station.dst_mac_mask[1] = 0x00;
    station.dst_mac_mask[2] = 0x00;
    station.dst_mac_mask[3] = 0x00;
    station.dst_mac_mask[4] = 0x0f;
    station.dst_mac_mask[5] = 0xff;

    rc = bcm_l2_station_add(unit, &station_id, &station);
    
    return rc;
    
}

/* Delete L2 Logical port */
int l2__port__delete(int unit, bcm_gport_t gport )
{
    return bcm_vlan_port_destroy(unit, gport);
}
int l2__lif_match_none__create(int unit, int flags,  bcm_gport_t port, int vsi, bcm_gport_t *gport ){
    int rv;
    bcm_vlan_port_t vp;
    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp.port = port;
    vp.vsi = vsi;
    vp.flags = flags;
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }
    *gport = vp.vlan_port_id;
    return BCM_E_NONE;
}

/* Create L2 Logical port according <port>*/
int l2__port__create(int unit, int flags,  bcm_gport_t port, int vsi, bcm_gport_t *gport ){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vp.port = port;
    vp.vsi = vsi; 
    vp.flags = flags;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  

    return BCM_E_NONE;

}

/* Create L2 Logical port according <port,vlan>*/
int l2__port_vlan__create(int unit, int flags, bcm_gport_t port, int vlan, int vsi, bcm_gport_t *gport){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.vsi = vsi; 
    vp.flags = flags;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id; 

    return BCM_E_NONE;

}

/* Create L2 Logical port according <port,vlan,vlan>*/
int l2__port_vlan_vlan__create(int unit, int flags, bcm_gport_t port, int vlan, int inner_vlan, int vsi, bcm_gport_t *gport){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.match_inner_vlan = inner_vlan;
    vp.vsi = vsi; 
    vp.flags = flags;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  

    return BCM_E_NONE;
}


/* Create non protection L2 forward group logical port according <port,vlan,vlan> */
int l2__fec_forward_group_port_vlan_vlan__create(int unit, bcm_gport_t port, int vlan, int inner_vlan, int vsi, int *fec, int *out_lif){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.match_inner_vlan = inner_vlan;
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_FORWARD_GROUP;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }
    print vp;
    *fec = vp.vlan_port_id;
    *out_lif = vp.encap_id;

    return BCM_E_NONE;
}

/* Create non protection L2 forward group logical port according <port,vlan> */
int l2__fec_forward_group_port_vlan__create(int unit, bcm_gport_t port, int vlan, int vsi, int *fec, int *out_lif){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port;
    vp.match_vlan = vlan; 
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_FORWARD_GROUP;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }
    print vp;
    *fec = vp.vlan_port_id;
    *out_lif = vp.encap_id;

    return BCM_E_NONE;
}

/* Return the default OLP port */
int l2__olp_port__get(int unit, bcm_gport_t *port){

    bcm_gport_t olp[BCM_PIPES_MAX];
    int count, rv = BCM_E_NONE;

    rv = bcm_port_internal_get(unit,BCM_PORT_INTERNAL_OLP,BCM_PIPES_MAX,olp,&count);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__olp_port__get\n");
        return rv;
    }
    if (count < 1) {
        printf("Error, No olp port configured\n");
        return BCM_E_NOF_FOUND;
    }

    *port = olp[OLP_DEFAULT_CORE];

    return BCM_E_NONE;
}


int l2__learning__config(int unit){

    int rv;
    bcm_l2_learn_msgs_config_t learn_msgs;
    int age_seconds;
    bcm_gport_t olp_port;

    rv = l2__olp_port__get(unit, &olp_port);
    if (rv != BCM_E_NONE) {
        printf("Error, l2__olp_port__get failed\n");
        return rv;
    }

    printf("olp_port: %d\n", olp_port);

    /* set format of learning messages */
    bcm_l2_learn_msgs_config_t_init(&learn_msgs);
    learn_msgs.flags = BCM_L2_LEARN_MSG_LEARNING;

    learn_msgs.dest_port = olp_port;

    /* follow attributes set the encapsulation of the learning messages */
    /* learning message encapsulated with ethernet header */
    learn_msgs.flags |= BCM_L2_LEARN_MSG_ETH_ENCAP;
    learn_msgs.ether_type = 0xab00;
    sal_memcpy(learn_msgs.src_mac_addr, src_mac_address, 6);
    sal_memcpy(learn_msgs.dst_mac_addr, dest_mac_address, 6);

    rv = bcm_l2_learn_msgs_config_set(unit, &learn_msgs);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_learn_msgs_config_set \n");
        return rv;
    }

    return BCM_E_NONE;
}

/* Create a mac table entry with da, vsi and port (gport) as inputs */
int l2__mact_entry_create(int unit, l2__mact_properties_s *mact_properties){

    int rv;
    bcm_l2_addr_t l2_address;

    /* local: MACT, add entry points to local-port */
    bcm_l2_addr_t_init(&l2_address,mact_properties->mac_address,mact_properties->vlan); 
    l2_address.flags = BCM_L2_STATIC; /* static entry */
    l2_address.port = mact_properties->gport_id;
    rv = bcm_l2_addr_add(0,&l2_address);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        return rv;
    }
    return rv;
    

    return BCM_E_NONE;
}



/* Create an ARP entry in EEDB
   In ROO, the overlay LL encapsulation is built with a different API call 
   (bcm_l2_egress_create instead of bcm_l3_egress create) */
int l2__egress_create(int unit, l2__arp_entry_properties_s *arp_entry_properties){

    int rv = BCM_E_NONE;
    bcm_l2_egress_t arp_entry;

    bcm_l2_egress_t_init(&arp_entry);

    arp_entry.dest_mac   = arp_entry_properties->dest_mac; /* next hop */
    arp_entry.src_mac    = arp_entry_properties->src_mac; /* my-mac */
    arp_entry.outer_vlan = arp_entry_properties->outer_vlan;      
    arp_entry.ethertype  = arp_entry_properties->ethertype;       /* ethertype for IP */
    arp_entry.outer_tpid = 0x8100;      /* outer tpid */


    rv = bcm_l2_egress_create(0,&arp_entry);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_egress_create\n");
        return rv;
    }

    return rv;
}

