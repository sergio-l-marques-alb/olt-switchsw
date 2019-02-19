/* $Id: cint_utils_l3.c,v 1.10 2013/02/03 10:59:10    Mark Exp $
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
 * This file provides L3 basic functionality and defines L3 global variables
 */

/* **************************************************************************************************
  --------------          Global Variables Definition and Initialization            -----------------
 *************************************************************************************************** */

/* Struct definitions */
struct l3_ipv4_route_entry_utils_s
{
    uint32 address;
    uint32 mask;

};

l3_ipv4_route_entry_utils_s g_l3_route_entry_utils    = { 0x01010100, /* address */
                                                          0xffffffff /*  mask    */ };

 /* ************************************************************************************************** */

/* Delete all l3 Interfaces*/
int l3__intf__delete_all(int unit){
    int rc;

    rc=bcm_l3_intf_delete_all(unit);

    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_delete_all\n"); 
    }

    return rc;
}

/* Delete l3 Interface*/
int l3__intf__delete(int unit, int intf)
{
    int rc;
    bcm_l3_intf_t l3if;

    bcm_l3_intf_t_init(l3if);

    l3if.l3a_intf_id = intf;

    rc = bcm_l3_intf_delete(unit, l3if);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_delete\n");
    }

    return rc;
}

/* Creating L3 interface */
int l3__intf_rif__create(int unit, int vlan, bcm_mac_t my_global_mac, bcm_mac_t my_mac, int *rif){

    bcm_l3_intf_t l3if;
    int rc;

   /* Initialize a bcm_l3_intf_t structure. */
    bcm_l3_intf_t_init(&l3if);

    l3if.l3a_flags =  0;
    l3if.l3a_vid = vlan;
    
    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, my_global_mac, 4); /* ovewriting 4 MSB bytes with global MAC configuration*/
    l3if.l3a_mac_addr[4] = (my_global_mac[4] & 0xf0) | (my_mac[4] & 0x0f);

    rc = bcm_l3_intf_create(unit, l3if);
    if (rc != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n"); 
    }

    *rif = l3if.l3a_intf_id;

    return rc;
}


/* Creating L3 interface with given RIF*/
int l3__intf_rif_with_id__create(int unit, bcm_mac_t my_global_mac, bcm_mac_t my_mac, int rif){

    bcm_l3_intf_t l3if;
    int rc;

   /* Initialize a bcm_l3_intf_t structure. */
    bcm_l3_intf_t_init(&l3if);

    l3if.l3a_flags =  BCM_L3_WITH_ID;
    l3if.l3a_intf_id = rif;
    l3if.l3a_vid = rif;

    sal_memcpy(l3if.l3a_mac_addr, my_mac, 6);
    sal_memcpy(l3if.l3a_mac_addr, my_global_mac, 4); /* ovewriting 4 MSB bytes with global MAC configuration*/
    l3if.l3a_mac_addr[4] = (my_global_mac[4] & 0xf0) | (my_mac[4] & 0x0f);

    return bcm_l3_intf_create(unit, l3if);
}


/* Create egress object(FEC and ARP entry)*/
int l3__egress__create(int unit, uint32 flags,  int out_port, int vlan, int l3_eg_intf, bcm_mac_t next_hop_mac_addr, int *intf, int *encap_id){

    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.intf = l3_eg_intf;

    sal_memcpy(l3eg.mac_addr, next_hop_mac_addr, 6);  

    l3eg.vlan   = vlan;  
    l3eg.port   = out_port;  
    l3eg.encap_id = *encap_id;
    l3egid = *intf;   

    rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);

    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rc;
    }
  
    *encap_id = l3eg.encap_id;
    *intf = l3egid;

    if(verbose >= 1) {
        printf("unit: created FEC-id =0x%08x, ", unit, *intf);
        printf("encap-id = %08x", *encap_id);
    }

    if(verbose >= 2) {
        printf("outRIF = 0x%08x out-port =%d", l3_eg_intf, *intf);
    }

    if(verbose >= 1) {
        printf("\n");
    }

    return rc;
}

/* Set an ARP entry, without allocating FEC entry*/
int l3__egress_only_encap__create(int unit, int out_port, int vlan, int l3_eg_intf, bcm_mac_t next_hop_mac_addr, int *encap_id){

    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.flags = BCM_L3_EGRESS_ONLY;
    l3eg.intf = l3_eg_intf;

    sal_memcpy(l3eg.mac_addr, next_hop_mac_addr, 6);  

    l3eg.vlan   = vlan;  
    l3eg.port   = out_port;  
    
    rc = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);

    if (rc != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rc;
    }
  
    *encap_id = l3eg.encap_id;

    if(verbose >= 1) {
        printf("encap-id = %08x\n", *encap_id);
    }

    return rc;
}

/* add ipv4 route entry to FEC*/
int l3__ipv4_route__add(int unit, l3_ipv4_route_entry_utils_s route_entry, int vrf, int intf) {

    int rc;
    bcm_l3_route_t l3rt;

    bcm_l3_route_t_init(l3rt);

    l3rt.l3a_flags = BCM_L3_RPF;
    l3rt.l3a_subnet = route_entry.address;
    l3rt.l3a_ip_mask = route_entry.mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rc = bcm_l3_route_add(unit, l3rt);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %x \n", rc);
    }

    if(verbose >= 1) {
        printf("l3__ipv4_route__add address:0x%x, mask:0x%x, vrf:%d ", route_entry.address, route_entry.mask,vrf);
        printf("---> egress-object=0x%08x, \n", intf);
    }

    return rc;
}

/* add ipv4 route entry to host table */
int l3__ipv4_route_to_overlay_host__add(int unit, uint32 ipv4_address, int vrf, int encap_id, int intf, int fec)
{

    int rc;

    bcm_l3_host_t l3_host;      
    bcm_l3_host_t_init(l3_host);  

    /* key of host entry */
    l3_host.l3a_vrf = vrf;               /* router interface */ 
    l3_host.l3a_ip_addr = ipv4_address;  /* ip host entry */

    /* data of host entry */
    l3_host.l3a_port_tgid = fec;         /* overlay tunnel: vxlan gport */
    l3_host.l3a_intf = intf;             /* out rif */
    l3_host.encap_id = encap_id;         /* arp pointer: encap/eei entry */

    rc = bcm_l3_host_add(unit, &l3_host);         
    print l3_host;
    if (rc != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");      
    }

    return rc;
}

/* add ipv4 route entry to host table */
int l3__ipv4_route__host__add(int unit, uint32 ipv4_address, int vrf, int encap_id,  int fec)
{

    int rc;

    bcm_l3_host_t l3_host;      
    bcm_l3_host_t_init(l3_host);  

    /* key of host entry */
    l3_host.l3a_vrf = vrf;               /* router interface */ 
    l3_host.l3a_ip_addr = ipv4_address;  /* ip host entry */

    /* data of host entry */
    l3_host.l3a_intf = fec;            
    l3_host.encap_id = encap_id;        

    rc = bcm_l3_host_add(unit, &l3_host);         
    
    if (rc != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");      
    }

    return rc;
}

/* setup global my mac MSB */
int l3__setup_global_my_mac(int unit, bcm_mac_t mac_addr) {

    int rv;
    int station_id;

    bcm_l2_station_t station; 

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

    rv = bcm_l2_station_add(unit, &station_id, &station);
    return rv;
}






