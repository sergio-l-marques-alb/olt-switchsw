/* $Id: cint_evpn.c,v 1.15 Broadcom SDK $
 $Copyright: Copyright 2012 Broadcom Corporation.
 This program is the proprietary software of Broadcom Corporation
 and/or its licensors, and may only be used, duplicated, modified
 or distributed pursuant to the terms and conditions of a separate,
 written license agreement executed between you and Broadcom
 (an "Authorized License").  Except as set forth in an Authorized
 License, Broadcom grants no license (express or implied), right
 to use, or waiver of any kind with respect to the Software, and
 Broadcom expressly reserves all rights in and to the Software
 and all intellectual property rights therein.  IF YOU HAVE
 NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 ALL USE OF THE SOFTWARE.  
  
 Except as expressly set forth in the Authorized License,
  
 1.     This program, including its structure, sequence and organization,
 constitutes the valuable trade secrets of Broadcom, and you shall use
 all reasonable efforts to protect the confidentiality thereof,
 and to use this information only in connection with your use of
 Broadcom integrated circuit products.
  
 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 
 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

/*  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                            |
 *  |                                          .            .  .       .       .                                           |
 *  |   Access                               .                                  .                                          |                       
 *  |                                      .                MPLS Core            . .                                       |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                     |             
 *  |   +--------+                     .      | P1  | \                 /   | P3  | \                                      | 
 *  |  /        /|                    .   /   |     |                       |     | \  .                                   | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                                 |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .                                |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+\                   |
 *  | +--------+\ esi  /        /| /        /    |                             |       \    /        /|                    |
 *  |             100\+--------+ |                          \     /                       \+--------+ | \                  |
 *  |             \   |  PE1   | |      /        |                             |        \  |   PE3  | |                    |
 *  |                 |        |/ \                          \ /                           |        |/   \                 |
 *  |               \ +--------+     /           |                             |         \/+--------+                      |
 *  |                 /             \                        / \                                          \                |
 *  |                             /              |                             |        / \           esi    +--------+    |
 *  |               /  \+--------+    \                    /     \                         +--------+ 200   /        /|    |
 *  |                  /        /|\              |                             |      /   /        /|    / +--------+ |    |
 *  |             /   +--------+ |      \                /         \                     +--------+ |      |  CE2   | |    |
 *  |   +--------+ esi|  PE2   | |. \            |                             |    /    |   PE4  | |  /   |        |/     |
 *  |  /        /| 300|        |/  .      \            /             \                   |        |/       +--------+      |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+  /                     |
 *  | |  CE3   | | /                 .      \+-----+  /                 \   +-----+       .                                |
 *  | |        |/                      .\    | P2  |                        | P4  |   / .                                  |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                                   |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+ / .                                    |
 *  |                                     .                                   . .  .                                       |
 *  |                                      .               . .       .      .                                              |
 *  |                                        .  .   .  .  .   .  .  . . . .                                                |
 *  |                                                                                                                      |
 *  |                                                                                                                      |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
                                                
struct cint_evpn_info_s {
    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int p1_port;  /* p1 port (provider) */
    int p2_port;  /* p2 port (provider) */
    int ce1_vlan; /* ce1's vlan */
    int ce2_vlan; /* ce2's vlan */
    int ce3_vlan; /* ce3's vlan */
    int p1_vlan;  /* p1's vlan */
    int p2_vlan;  /* p2's vlan */
    int vsi; /* vsi representing the service in this application */
    int pe2_id; /* pe2's id */
    int pe1_pe2_ce3_esi_label; /* The esi of pe1, pe2, ce3 */
    int pe1_pe2_ce1_esi_label; /* The esi of pe1, pe2, ce1 */
    bcm_mac_t global_mac; /* global mac of PE1 */
    bcm_mac_t my_mac; /* PE1's my mac facing p1 */
    bcm_mac_t ce1_mac;  /* ce1's mac address*/
    bcm_mac_t ce2_mac; /* ce2's mac address*/
    bcm_mac_t ce3_mac ; /* ce3's mac address*/
    bcm_mac_t p1_mac; /* p1's mac address*/
    bcm_mac_t p2_mac; /* p2's mac address*/
    bcm_mac_t pe3_mac; /* pe3's mac address*/
    bcm_if_t pe1_rif; /* rif of flooding domain */
    bcm_gport_t p1_vport_id; /* vlan port id of p1's AC */
    bcm_gport_t ce1_vport_id; /* vlan port id of ce1's AC */
    bcm_gport_t ce3_vport_id; /* vlan port id of ce3's AC */
    bcm_mpls_label_t evpn_label; /* EVPN label for ingress and egress pe */
    bcm_mpls_label_t pe1_pe2_lsp_label; /* lsp label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_lsp_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_lsp_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t pe1_pe2_iml_label; /* iml label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_iml_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_iml_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t iml_range_low_label; /* low bound of iml range in egress pe */
    bcm_mpls_label_t iml_range_high_label; /* high bound of iml range in egress pe */
    bcm_mpls_label_t frr_label; /* frr (fast re-route) label */
    uint32 evpn_profile; /* user defined evpn profile : 0-7 */
    bcm_ip_t ce2_ip; /* ce2's ip */ 
    int evpn_over_lsp_fec; /* FEC pointing to evpn over lsp tunnel */

};


cint_evpn_info_s cint_evpn_info = 
/* ports :
   ce1 port | ce3 port | p1 port | p2 port | p3 port port  */
    {200,            201,                202 ,              202,              203,  
/* vlans:
   ce1_vlan | ce2_vlan | ce3_vlan | p1_vlan | p2_vlan*/
    20, 15,  25, 30, 35,
/* vsi: */
    40, 
/* pe2 id: */
    100,
/* The esi of pe1, pe2, ce3 | The esi of pe1, pe2, ce1 */ 
   300, 100,     
/* global_mac | my_mac | ce1_mac | ce2_mac | ce3_mac | p1_mac | p2_mac | pe3_mac  */
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x11}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x12},
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x13}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x14}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x15}, 
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x16}, {0x00, 0x11, 0x00, 0x00, 0x00, 0x17}, 
/*  pe1_rif: rif of flooding domain */ 
   0, 
/* p1_vport_id | ce1_vport_id | ce3_vport_id */
   0, 0 , 0, 
/* evpn_label | pe1_pe2_lsp_label | pe1_pe3_lsp_label | pe1_pe4_lsp_label |
   pe1_pe2_iml_label | pe1_pe3_iml_label | pe1_pe4_iml_label | iml_range_low_label |  iml_range_high_label | frr_label*/                                                                                                                 
   8000, 5100, 5200, 5300, 6100, 6200, 6300, 6000, 7000, 9000, 
/* evpn profile */
   3,
/* ce2_ip*/
   0xE0000102 /* 224.0.1.2 */, 
/* evpn_over_lsp_fec*/
   0, 
};



void evpn_info_init(int unit,
                    int ce1_port,
                    int ce3_port, 
                    int p1_port,
                    int p2_port
                    ) {
    cint_evpn_info.ce1_port = ce1_port; 
    cint_evpn_info.ce3_port = ce3_port; 
    cint_evpn_info.p1_port = p1_port; 
    cint_evpn_info.p2_port = p2_port;
}

/* 
  1) Global configurations for pe:
     1.1) Configure vsi for this application. will be used for the flooding domain of the application.
     1.2) Open a muilticast group that will serve as the flooding domain of the application. Receives
          as a parameter the above configured vsi.
     1.3) Configure global mac for pe.
 
  2) Create ingress pe known unicast (locally learned destination).                                               
     Packet arrives from ce1, destined for ce3. The mac address
     for ce3 was previously learned at pe1                                                                                                             
     2.1) Create in ac for ce1.                                   
     2.2) Create out ac for ce3.                                                             
                                                                                            
  3) Create ingress pe known unicast traffic with remote learning.
     Packet arrives from ce1, destined for ce2. The mac address
     for ce2 was previously learned at pe3 and distributed to pe1 via BGP
     Packet exits pe1 with evpn over lsp.
     3.1) Create in ac for ce1.
     3.2) Create a (outgoing) rif with pe's my_mac facing p1.
     3.3) Create egress tunnel: evpn over lsp. Set the tunnel to point to the above created rif.
     3.4) Create a MACT entry with ce2's mac, the egress tunnel's gport as destination.
     3.5) Set LL information (next hop for the above created tunnel).
 
  4) Create ingress pe bum traffic: receive a packet from ce1
     with an unknown DA. Replicate the packet with a MC group (flooding domain) comprising of:
     1. A copy to ce3, which belongs to esi 300, of which pe1 is the DF.
     2. Two copies with IML over lsp, for remote pes not attached to ce1's esi.
     3. One copy with ESI over IML over lsp for pe2, which is attached to ce1's esi.   
     4.1) Create in ac for ce1.
     4.2) Set additional data for ce1's ac for pmf processing.
     4.3) Set PMF inlif profile for ce1's ac.
     4.4) Create ac for ce3 (add it to flooding domain).
     4.5) Create a (outgoing) rif with pe's my_mac facing p1.
     4.6) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
          to point to the above created rif.
     4.7) Set LL information (next hop for the above created tunnels).
     4.8) Set additional data for the tunnel of pe1->pe2 (for esi prepending).
 
  5) Create egress pe known unicast. Pe1 receives a packet destined for ce3, SA==ce2_mac.
     ce3's mac was learned by pe3 from pe1 via BGP. Packet is EVPN over LSP.
     Terminate the tunnels and forward to ce3 (pe1 is ce3' DF).
     5.1) Create out ac for ce3.
     5.2) Create in ac for p1.
     5.3) Create ingress tunnel (for termination) with EVPN ovel LSP.
 
  6) Create egress pe bum traffic. Handle two cases:
     1. Receive a packet with IML over LSP from pe3, with unknown destination. Packet should
     be flooded to ce1 (pe1 is it's DF) and to remote pes (will be pruned due to orientation). In fact,
     only the packet to ce1 will exit the device.
     2. Receive a packet with ESI over IML over LSP from pe2, with unknown destination.
     Packet should be flooded to ce1 (will be filtered due to same esi value as in ESI label) and to
     remote PEs (will be pruned due to orientation).
     6.1) Create out ac for ce3.
     6.2) Create in ac for p1.
     6.3) Create a (outgoing) rif with pe's my_mac facing p1.
     6.4) Create egress tunnels: two with IML over lsp, one with esi over iml over lsp. Set them
          to point to the above created rif (eventually they will be pruned due to orientation).
     6.5) Set LL information (next hop for the above created tunnels).
     6.6) Set additional data for the tunnel of pe1->pe2 (for esi prepending).
     6.7) Set IML label range.
     6.8) Create 4 ingress tunnels (representing 2 label stacks) :
          1. for pe2->pe1 create one tunnel with lsp,
            the other with IML (and an indication for ESI).
          2. for pe3->pe1 create one tunnel with lsp, the other with IML (without
             an indication for ESI. 
    
*/
int evpn_pe_config(int unit,
                    int ce1_port,
                    int ce3_port, 
                    int p1_port,
                    int p2_port){

    int rv = BCM_E_NONE;

    evpn_info_init(unit,ce1_port,ce3_port2, p1_port, p2_port); 

    /* Configure vsi for this application */
    rv = bcm_vswitch_create_with_id(unit, vsi); /* 1.1 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vswitch_create_with_id with vid = %d\n", vlan);
        return rv;
    }

    /* Create multicast group (flooding for this service).
       multicast id has to be the same one as the vswitch */
    rv = multicast__open_ingress_mc_group(unit, vsi); /* 1.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in multicast__open_ingress_mc_group\n");
        return rv;
    }

    /* Configure global mac for the device */
    l2__mymac_properties_s mymac_properties;
    sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
    mymac_properties.mymac_type = l2__mymac_type_global_msb;
    sal_memcpy(mymac_properties.mymac, cint_evpn_info.global_mac, 6);
    
    rv = l2__mymac__set(unit, &mymac_properties); /* 1.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mymac__set\n");
        return rv;
    }

    /* Configure L2 information */
    rv = evpn_create_l2_interfaces(unit); /* 2.1, 2.2, 3.1, 4.1, 4.2, 4.3, 4.4, 5.1, 5.2 ,6.1, 6.2*/
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_l2_interfaces\n");
        return rv;
    }

    /* Create egress tunnels for forwarding */
    rv = evpn_create_egress_tunnels(unit,1); /* 3.2, 3.3, 3.4, 3.5, 4.5, 4.6, 4.7, 4.8, 6.3, 6.4, 6.5, 6.6 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_egress_tunnels_and_ll_info\n");
         return rv;
     }

    /* Create ingress tunnels for termination*/
    rv = evpn_create_ingress_tunnels(unit); /* 5.3, 6.7, 6.8 */
    if (rv != BCM_E_NONE) {
         print rv;
         printf("Error, in evpn_create_ingress_tunnels\n");
         return rv;
     }

    return rv;
}


/* Create AC, learn it statically, add it to flooding domain */
int evpn_create_ac(int unit, int flags,  int vlan, \
                   int vsi, int port, bcm_gport_t *vport_id, \
                   bcm_mac_t mac_address, int add_to_flooding_domain){

    int rv = BCM_E_NONE;
    l2__mact_properties_s mact_properties;

    /* Create a vlan port*/
    rv = l2__port_vlan__create(unit, flags,  port, vlan, vsi, vport_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__port_vlan__create\n");
        return rv;
    }

    mact_properties.gport_id = *vport_id;
    mact_properties.mac_address = mac_address;
    mact_properties.vlan = vlan;
    /* Create a mact entry */
    rv = l2__mact_entry_create(unit, &mact_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }
    
    /* add the vlan port to the flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__vlan_encap_add(unit, cint_evpn_info.vsi, port, *vport_id);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__vlan_encap_add\n");
            return rv;
        }
    }
    return rv;

}

/* Create routing interface*/
int evpn_create_rif(int unit, int vsi, bcm_mac_t my_mac, bcm_if_t *rif){

    int rv = BCM_E_NONE;

    /* Setting vsi 12 LSB bits of my-MAC */
    l2__mymac_properties_s mymac_properties;
    sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
    mymac_properties.mymac_type = l2__mymac_type_ipv4_vsi;
    sal_memcpy(mymac_properties.mymac, my_mac, 6);
    mymac_properties.vsi = vsi;

    rv = l2__mymac__set(unit, &mymac_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mymac__set\n");
        return rv;
    }

    /* create rif */
    rv = l3__intf_rif__create(unit, vsi, cint_evpn_info.global_mac, my_mac, rif);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__intf_rif__create\n");
        return rv;
    }

    return rv;

}

/* Set egress tunnel */
int evpn_set_egress_tunnel(int unit, int add_to_flooding_domain, int set_split_horizon, \
                            int flags, bcm_mpls_label_t label_in, bcm_mpls_label_t label_out, \
                            bcm_mpls_egress_action_t egress_action, int port, int *tunnel_id, int out_rif){
    
    int rv = BCM_E_NONE;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    mpls_tunnel_properties.flags = flags;
    mpls_tunnel_properties.label_in = label_in;
    mpls_tunnel_properties.label_out = label_out;
    mpls_tunnel_properties.egress_action = egress_action;
    mpls_tunnel_properties.next_pointer_intf = out_rif;

    rv  = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }
    
    *tunnel_id =  mpls_tunnel_properties.tunnel_id;

    /* Adding tunnel_id to flooding domain (cud)*/
    if (add_to_flooding_domain) {
        rv = multicast__mpls_encap_add(unit, cint_evpn_info.vsi, port, mpls_tunnel_properties.tunnel_id){
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in multicast__mpls_encap_add\n");
            return rv;
        }        
    }

    /* configure split horizon for this tunnel */
    if (set_split_horizon) {
        rv = bcm_port_class_set(unit,mpls_tunnel_properties.tunnel_id, bcmPortClassFieldEgress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set\n");
            return rv;
        }
    }

    return rv;
            
}


/* Set ingress tunnel (for termination)*/
int evpn_set_ingress_tunnel(int unit, int set_split_horizon, \
                            int flags, bcm_mpls_label_t label, bcm_mpls_label_t second_label\
                            bcm_mpls_switch_action_t action, bcm_if_t tunnel_if){
    
    int rv = BCM_E_NONE;
    mpls__ingress_tunnel_utils_s mpls_tunnel_properties;


    mpls_tunnel_properties.flags = flags;
    mpls_tunnel_properties.tunnel_if = tunnel_if;
    mpls_tunnel_properties.vpn = cint_evpn_info.vsi;
    mpls_tunnel_properties.action = action;
    mpls_tunnel_properties.label = label;
    mpls_tunnel_properties.second_label = second_label;

    rv  = mpls__add_switch_entry(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    } 
    if (set_split_horizon) {
    
        /* Set split horizon: orientation==1*/
        rv = bcm_port_class_set(unit,mpls_tunnel_properties.tunnel_id, bcmPortClassFieldIngress, 1);
        if (rv != BCM_E_NONE) {
            print rv;
            printf("Error, in bcm_port_class_set\n");
            return rv;
        }
    }

    return rv;
            
}

/* Create egress tunnels with LL information */
int evpn_create_egress_tunnels(int unit,int config_wide_data){

    int rv = BCM_E_NONE;
    uint64 data = 0;
    int tunnel_id1 = 0, tunnel_id2 = 0, tunnel_id3 = 0, tunnel_id4;
    int encap_id = 0;

    /* Create out rif for egress tunnel*/
    rv = evpn_create_rif(unit, cint_evpn_info.vsi, \ /* 3.2, 4.5, 6.3 */
                         cint_evpn_info.my_mac, &(cint_evpn_info.pe1_rif));
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_rif\n");
        return rv;
    }

    /* Create tunnel to pe3, which is not attached to ce1's esi*/
    rv = evpn_set_egress_tunnel(unit, 1, 1, 0, cint_evpn_info.pe1_pe3_iml_label, cint_evpn_info.pe1_pe3_lsp_label, \ /* 4.6, 6.4 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p1_port, &tunnel_id1,cint_evpn_info.pe1_rif){

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }

    /* Create tunnel to pe4, which is not attached to ce1's esi*/
    rv = evpn_set_egress_tunnel(unit, 1, 1, \
                            0, cint_evpn_info.pe1_pe4_iml_label, cint_evpn_info.pe1_pe4_lsp_label, \ /* 4.6, 6.4 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p1_port, &tunnel_id2, cint_evpn_info.pe1_rif){

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }


    /* Create tunnel to pe2, which is attached to ce1's esi*/
    rv = evpn_set_egress_tunnel(unit, 1, 1,  \
                            BCM_MPLS_EGRESS_LABEL_WIDE|BCM_MPLS_EGRESS_LABEL_IML, cint_evpn_info.pe1_pe2_iml_label, cint_evpn_info.pe1_pe2_lsp_label, \ /* 4.6, 6.4 */
                            BCM_MPLS_EGRESS_ACTION_PUSH, cint_evpn_info.p2_port, &tunnel_id3, cint_evpn_info.pe1_rif){

    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }

    /* Create egress tunnel with evpn and lsp labels */
    rv  = evpn_set_egress_tunnel(unit, 0, 0,  \ /* 3.3 */
                            0,cint_evpn_info.evpn_label , cint_evpn_info.pe1_pe3_lsp_label, \
                            BCM_MPLS_EGRESS_ACTION_PUSH, 0, &tunnel_id4 cint_evpn_info.pe1_rif);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_egress_tunnel\n");
        return rv;
    }

    /* Create forwarding entry for DA==ce2_mac. Result is LSP*/
    rv = l2__mact_entry_create(unit, tunnel_id4, cint_evpn_info.ce2_mac, cint_evpn_info.ce1_vlan); /* 3.4 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l2__mact_entry_create\n");
        return rv;
    }

    /* Set LL for egress tunnel */
    rv = l3__egress__create(unit,BCM_L3_EGRESS_ONLY, cint_evpn_info.p1_port, cint_evpn_info.p1_vlan, \ /* 3.5 */
                            tunnel_id4, cint_evpn_info.p1_mac, &cint_evpn_info.evpn_over_lsp_fec, &encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }

    /* Set LL for egress tunnel */
    rv = l3__egress__create(unit,BCM_L3_EGRESS_ONLY, cint_evpn_info.p1_port, cint_evpn_info.p1_vlan, \ /* 4.7 , 6.5*/
                            tunnel_id1, cint_evpn_info.p1_mac, &egid, &encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }
    /* Set LL for egress tunnel */
    rv = l3__egress__create(unit,BCM_L3_EGRESS_ONLY, cint_evpn_info.p1_port, cint_evpn_info.p1_vlan, \ /* 4.7, 6.5 */
                            tunnel_id2, cint_evpn_info.p1_mac, &egid, &encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }

    /* Set LL for egress tunnel */
    rv = l3__egress__create(unit,BCM_L3_EGRESS_ONLY, cint_evpn_info.p2_port, cint_evpn_info.p2_vlan, \ /* 4.7, 6.5 */
                            tunnel_id3, cint_evpn_info.p2_mac, &egid, &encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in l3__egress__create\n");
        return rv;
    }

    /* data == PE id*/
    data = cint_evpn_info.pe2_id;
    rv = bcm_port_wide_data_set(unit,BCM_PORT_WIDE_DATA_EGRESS, tunnel_id3, data); /* 4.8, 6.6 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_wide_data_set with \n");
        return rv;
    }



    return rv;
}

/* Create ingress tunnels (for termination) for egress pe bum traffic */
int evpn_create_ingress_tunnels(int unit){

    int rv = BCM_E_NONE;
    int flags 0 ;
    bcm_mpls_range_action_t action;

    /* Setting IML range*/
    action = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_evpn_info.iml_range_low_label, cint_evpn_info.iml_range_high_label,action); /* 6.7 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_mpls_range_action_add\n");
        return rv;
    }


    /* Set LSP ltermination label of pe3-->pe1 */
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe3_pe1_lsp_label, 0  \ /* 5.3, 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set IML ltermination label of pe3-->pe1 (no esi expected, indicated by BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML|BCM_MPLS_SWITCH_EXPECT_BOS;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe3_pe1_iml_label, 0 \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set LSP termination label of pe2-->pe1 */
    flags = 0;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe2_pe1_lsp_label, 0 \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Set IML ltermination label of pe2-->pe1 (esi is expected, indicated by the absence of BCM_MPLS_SWITCH_EXPECT_BOS) */
    flags = BCM_MPLS_SWITCH_EVPN_IML;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.pe2_pe1_iml_label, 0 \ /* 6.8 */
                            BCM_MPLS_SWITCH_ACTION_POP, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_set_ingress_tunnel\n");
        return rv;
    }

    /* Create ingress tunnel : EVPN over LSP (to be terminated) */
    flags = 0;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.evpn_label, \ /* 5.3 */
                           0,BCM_MPLS_SWITCH_ACTION_POP, BCM_IF_INVALID;){
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    /* Create ingress tunnel : FRR label (to be terminated) */
    flags = 0;
    rv = evpn_set_ingress_tunnel(unit, 1, flags, cint_evpn_info.frr_label, \ 
                           0,BCM_MPLS_SWITCH_ACTION_POP, BCM_IF_INVALID;){
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in mpls__add_switch_entry\n");
        return rv;
    }

    return rv;
}

/* Create attachment circuits for this application*/
int evpn_create_l2_interfaces(int unit){

    int rv = BCM_E_NONE;
    uint64 data = 0;

    /* Create ac for ce1, learn it statically */
    rv = bcm_vlan_create(unit, cint_evpn_info.ce1_vlan); /* 2.1, 3.1, 4.1 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", cint_mldp_info.p1_vlan);
        return rv;
    }
    rv = evpn_create_ac(unit, BCM_VLAN_PORT_INGRESS_WIDE, \ /* 2.1, 3.1 */
         cint_evpn_info.ce1_vlan, cint_evpn_info.ce1_vlan,cint_evpn_info.ce1_port, \
         &(cint_evpn_info.ce1_vport_id), cint_evpn_info.ce1_mac,0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_and_statically_learn_ac with ce1\n");
        return rv;
    }

    /* Set additional info: PE id + esi*/
    /* data == esi label + PE id*/
    data = cint_evpn_info.pe2_id;
    data <<= 20; /* shift by an mpls label size */
    data |= cint_evpn_info.pe1_pe2_ce1_esi_label;
    rv = bcm_port_wide_data_set(unit,BCM_PORT_WIDE_DATA_INGRESS, cint_evpn_info.ce1_vport_id, data); /* 4.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_wide_data_set with \n");
        return rv;
    }

    /* Set PMF inLif profile*/
    rv = bcm_port_class_set(unit,cint_evpn_info.ce1_vport_id, bcmPortClassFieldIngress, cint_evpn_info.evpn_profile); /* 4.3 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_port_wide_data_set with \n");
        return rv;
    }    
    
    /* Create ac for ce3, learn it statically, add it to flooding domain */
    rv = evpn_create_ac(unit,BCM_VLAN_PORT_INGRESS_WIDE, cint_evpn_info.ce3_vlan, \ /* 2.2, 4.4, 6.1 */
                        cint_evpn_info.vsi,cint_evpn_info.ce3_port,&(cint_evpn_info.ce3_vport_id), cint_evpn_info.ce3_mac, 1);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_and_statically_learn_ac with ce1\n");
        return rv;
    }

    /* Set p1 as ac*/
    /* Create ac for ce1, learn it statically */
    rv = bcm_vlan_create(unit, cint_evpn_info.p1_vlan); /* 5.2 */
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in bcm_vlan_create with vid = %d\n", cint_mldp_info.p1_vlan);
        return rv;
    }
    rv = evpn_create_ac(unit,0, cint_evpn_info.p1_vlan, cint_evpn_info.p1_vlan ,cint_evpn_info.p1_port, \ /* 5.2, 6.2 */
         &(cint_evpn_info.p1_vport_id),cint_evpn_info.p1_mac, 0);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }



    return rv;
}

/* Not complete */
int evpn_set_ingress_roo(int unit){

    int rv = BCM_NONE;
    int native_encap_id;
    int fec;
    int gport_forward_fec = 0;

    /* Create native ARP entry, next hop is pe3 */
    rv = l3__egress_only_encap__create(unit, cint_evpn_info.p1_port, cint_evpn_info.ce2_vlan, cint_evpn_info.pe1_rif, cint_evpn_info.pe3_mac, &native_encap_id);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }

    /* Convert FEC id to gport */
    BCM_GPORT_FORWARD_PORT_SET(gport_forward_fec, cint_evpn_info.evpn_over_lsp_fec);

    /* Create L3 host entry (route to native DIP, with DIP == ce2_ip */
    rv = l3__ipv4_route_to_overlay_host__add(unit, cint_evpn_info.ce2_ip,0, native_encap_id, cint_evpn_info.pe1_rif, cint_evpn_info.evpn_over_lsp_fec);
    if (rv != BCM_E_NONE) {
        print rv;
        printf("Error, in evpn_create_ac with ce1\n");
        return rv;
    }
    return rv;
    
}
