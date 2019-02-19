/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~IPMC FLOWS~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_ipmc_flows.c $
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
 * File: cint_ipmc_flows.c
 * Purpose: Example of various IPMC flows.
 *
 *
 *        -----------------------                      ------------------                 -------------------------
 *       |  bcmSwitchL3McastL2==1 |  <-- NO  <--------| RIF.IPMC enable? |------> YES --->| ipmc_vpn_lookup_enable |
 *        -----------------------                      ------------------                 -------------------------
 *                    |                                                                                |
 *                    |                                                                                |
 *                    |                                                                                |
 *                    |                                                                                |
 *        ---NO------- -------YES---                                                       ---NO------- -------YES---
 *        |                         |                                                      |                         |
 *        |                         |                                                      |                         |
 *   -----------               ------------                                         -------------------       -------------------
 *  |LEM<FID,DA>|             |LEM<FID,DIP>|                                       | TCAM<RIF,DIP,SIP> |     | TCAM<RIF,DIP,SIP> |
 *   -----------               ------------                                        | LEM<RIF,DIP,SIP>  |     |     LEM<VRF,DIP>  |
 *        |                         |                                               -------------------      -------------------
 *        |        Hit?             |                                                       |                        |
 *         -------------------------                                                        |                        |
 *                     |                                                                    |                        |
 *                     |                                                                     -------------------------
 *                     |                                                                                 |
 *        ---NO-------  -------YES---                                                                    |  Hit?
 *        |                          |                                                                   |
 *        |                          |                                                     ---NO--------- ---------YES----
 *   ------------               -------------                                              |                              |
 *  |L2-FLOODING |             |L2-FORWARDING|                                             |                              |
 *  |   ON VSI   |              -------------                                 ---------------------------------         --------------
 *   ------------                                                            |bcmSwitchUnknownIpmcAsMcast == 1|        |L3-FORWARDING|
 *                                                                            ---------------------------------         --------------
 *                                                                                         |
 *                                                                                         |
 *                                                                              ---NO------- -------YES---
 *                                                                              |                        |
 *                                                                              |                        |
 *                                                                              |                        |
 *                                                                          -------------             ------------
 *                                                                         |L3-FORWARDING|           |L2-FLOODING |
 *                                                                         | VRF DEFAULT |           |   ON VSI   |
 *                                                                          -------------             ------------
 *
 *
 *
 *  Abstract:
 *
 *  When an IPv4oE packet arrives at the ingress with multicast-compatible destination address (that is,
 *  DA[47:23]=={24'h01_00_5E, 1'b0} and Ethernet-Header.DA[22:0] = IP-Header.DIP[22:0] or IPHeader-DIP[31:28] = to 4'hE)
 *  , one of two main flows take place, according to whether RIF.IPMC is enabled.
 *  The flow in which RIF.IPMC is enabled is exemplified in the function ipmc_flows_rif_ipmc_enabled.
 *  The flow in which RIF.IPMC is disabled is exemplified in the function ipmc_flows_rif_ipmc_disabled.
 *
 *  Each function exemplifies sub flows of these two main flows. Each sub flow is defined in a matching
 *  section which also determines a possible traffic flow of a packet.
 *  Each function's header contains exact information required to understand each sub flow and send
 *  a packet according to the sub-flow's stipulations.
 *
 *
 *  We present a pre-configuration for ipmc_flows_rif_ipmc_disabled(), after which we can send traffic
 *  according to sub-flows in this flow. The sub flows are presented in the preamble of the function, each one
 *  exemplified by certain packet sending and receiving.
 *
 *  BCM> cint cint_ipmc_flows.c
 *  BCM> cint
 *  cint> int rv;
 *  cint> int inP = 200;
 *  cint> int outP = 201;
 *  cint> int outP2 = 202;
 *  cint> int outP3= 203;
 *  cint> int vid1 = 4000;
 *  cint> int vid2 = 4001;
 *  cint> int vid3 = 4002;
 *  cint> int vid4 = 4003;
 *  cint> rv = ipmc_flows_rif_ipmc_disabled(0, inP, outP,outP2 ,outP3 ,vid1, vid2, vid3, vid4);
 *
 * Packet sending for ipmc_flows_rif_ipmc_disabled():
 *
 * 1. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid1);
 *    Then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4000; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 2. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid2);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000c (192.168.0.12)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4001; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 3. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid3);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,1);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000104 (224.0.1.4).
 *    Expect it to be forwarded to ports outP, outP2, outP3, with vid == 4002; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 4. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vid4);
 *                  cint> switch_control_set(unit,bcmSwitchL3McastL2,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:03    dst_mac :01:00:5E:00:01:05    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000105 (224.0.1.5).
 *    Expect it to be flooded to ports outP, outP2, with vid == 4003; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 *
 *  We present a pre-configuration for ipmc_flows_rif_ipmc_enabled(), after which we can send traffic
 *  according to sub-flows in this flow. The sub flows are presented in the preamble of the function, each one
 *  exemplified by certain packet sending and receiving.
 *
 *  BCM> cint cint_ipmc_flows.c
 *  BCM> cint
 *  cint> int rv;
 *  cint> int inP = 200;
 *  cint> int outP = 201;
 *  cint> int outP2 = 202;
 *  cint> int outP3= 203;
 *  cint> int rif1 = 4000;
 *  cint> int rif2 = 4001;
 *  cint> int rif3 = 4002;
 *  cint> int rif4 = 4003;
 *  cint> int rif5 = 4009;
 *  cint> int vlan = 4005;
 *  cint> rv = ipmc_flows_rif_ipmc_enabled(0, inP, outP,outP2 ,outP3 , rif1, rif2, rif3, rif4, rif5, vlan);
 *
 * Packet sending for ipmc_flows_rif_ipmc_enabled():
 *
 * 1. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif1);
 *    If ipmc_vpn_lookup_enable ==1, then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2)
 *    Otherwise(ipmc_vpn_lookup_enable==0), Then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.13)    dst_ip: 0xe0000102 (224.0.1.4)
 *    Both them expect to be forwarded to ports outP, outP2, outP3, with vid == 4000; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 2. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif2);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000c (192.168.0.12)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP, outP2, with vid == 4001; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 3. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif3);
 *    Then, send packet with: src_mac: 00:02:05:00:00:01    dst_mac :01:00:5E:00:01:03    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000103 (224.0.1.3).
 *    Expect it to be forwarded to ports outP2, outP3, with vid == 4002; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 4. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif4);
 *    If ipmc_vpn_lookup_enable ==1, then, send packet with: src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000b (192.168.0.11)    dst_ip: 0xe0000102 (224.0.1.2).
 *    Otherwise(ipmc_vpn_lookup_enable==0), then, send packet with : src_mac: 00:02:05:00:00:00    dst_mac :01:00:5E:00:01:02    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000102 (224.0.1.5)
 *    Expect it to be forwarded to ports outP, outP3, with vid == 4003; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 5. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,vlan);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000d (192.168.0.13)    dst_ip: 0xe0000104 (224.0.1.4).
 *    Expect it to be flooded to ports outP, outP2, with vid == 4005; src_mac, dst_mac, src_ip, dst_ip without change.
 *
 * 6. First type :  cint> bcm_port_untagged_vlan_set(unit,inP,rif5);
 *                  cint> switch_control_set(unit,bcmSwitchUnknownIpmcAsMcast,0);
 *    Then, send packet with: src_mac: 00:02:05:00:00:02    dst_mac :01:00:5E:00:01:04    src_ip: 0xc0a8000e (192.168.0.14)    dst_ip: 0xe0000105 (224.0.1.5).
 *    Expect it to be forwarded to ports outP, outP2, with vid == 4009; src_mac, dst_mac, src_ip, dst_ip without change.
 */


/* Main struct
 * holds data structures relevant for the hereby exemplified flows
 */
struct ipmc_flows_info_s {

    bcm_ip_t mc_ip_arr[4];
    bcm_ip_t src_ip_arr[4];
    int ipmc_indices[8]; /* this array's entries will keep the indices of the groups without change*/
    int ipmc_mc_groups[8]; /* this array's entries will be changed after creating multicast groups*/
    int forwarding_ports_1[3];
    int forwarding_ports_2[2];
    int forwarding_ports_3[2];
    int forwarding_ports_4[2];
};

ipmc_flows_info_s ipmc_flows_info1;

/*my_macs*/
bcm_mac_t mac_address_1 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};
bcm_mac_t mac_address_2 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01};
bcm_mac_t mac_address_3 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
bcm_mac_t mac_address_4 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x03};
bcm_mac_t mac_address_5 = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x04};
bcm_mac_t mac1 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x03};
bcm_mac_t mac2 = {0x01, 0x00, 0x5E, 0x00, 0x01, 0x02};

/*
 * 1: for IPMC packet with VRF !=0  (VPN) forwarding is according to VRF, G
 * 0: for IPMC packet forwarding is according to <RIF,G,SIP> regardless the VRF value
 */
int ipmc_vpn_lookup_enable = 1;


/* Init function. configures relevant data to be placed
 * in the above declared data structures
 */
void
ipmc_flows_init(int outP, int outP2, int outP3){

    int ipmc_index_offset = 4096; /* 4k */
    int ipmc_index_upper_bound = 32768 - ipmc_index_offset;
    int i;

    ipmc_flows_info1.forwarding_ports_1[0] = outP; /* Different combinations of ports representing multiple destinations for a certain mc group*/
    ipmc_flows_info1.forwarding_ports_1[1] = outP2;
    ipmc_flows_info1.forwarding_ports_1[2] = outP3;

    ipmc_flows_info1.forwarding_ports_2[0] = outP ;
    ipmc_flows_info1.forwarding_ports_2[1] = outP2;

    ipmc_flows_info1.forwarding_ports_3[0] = outP2 ;
    ipmc_flows_info1.forwarding_ports_3[1] = outP3;

    ipmc_flows_info1.forwarding_ports_4[0] = outP ;
    ipmc_flows_info1.forwarding_ports_4[1] = outP3;

    ipmc_flows_info1.mc_ip_arr[0] = 0xE0000102; /* 224.0.1.2 */
    ipmc_flows_info1.mc_ip_arr[1] = 0xE0000103; /* 224.0.1.3 */
    ipmc_flows_info1.mc_ip_arr[2] = 0xE0000104; /* 224.0.1.4 */
    ipmc_flows_info1.mc_ip_arr[3] = 0xE0000105; /* 224.0.1.5 */

    ipmc_flows_info1.src_ip_arr[0] = 0xC0A8000B; /* 192.168.0.11 */
    ipmc_flows_info1.src_ip_arr[1] = 0xC0A8000C; /* 192.168.0.12 */
    ipmc_flows_info1.src_ip_arr[2] = 0xC0A8000D; /* 192.168.0.13 */
    ipmc_flows_info1.src_ip_arr[3] = 0xC0A8000E; /* 192.168.0.14 */

    sal_srand(sal_time());
    for (i=0; i <= 5; i++) {  /* picking random values for the mc groups */
        ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset; /* ipmc index will be in range (4k,32k)*/
        while (is_in_array(ipmc_flows_info1.ipmc_mc_groups, i-1, ipmc_flows_info1.ipmc_mc_groups[i])){ /* verifying that we don't get a previous value */
            ipmc_flows_info1.ipmc_mc_groups[i] = (sal_rand() % ipmc_index_upper_bound) + ipmc_index_offset;
        }

        ipmc_flows_info1.ipmc_indices[i] = ipmc_flows_info1.ipmc_mc_groups[i];
    }


}

/* utility function that checks whether a certain value already exists in a given array*/
int is_in_array(int *arr, int last_index, int val){

    int i;

    for (i=0; i <= last_index ; i++) {
        if (val == arr[i]) {
            return 1;
        }
    }

    return 0;
}


/* Destroys objects created in this feature*/
int
ipmc_flows_destroy(int unit, int nof_mc_indices){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i <= nof_mc_indices - 1; i++) {
        rv = bcm_multicast_destroy(unit, ipmc_flows_info1.ipmc_indices[i]);
        if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND)) {
            printf("Error, in multicast_destroy\n");
            return rv;
        }
    }

    rv = bcm_l3_intf_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_intf_delete_all\n");
        return rv;
    }

    return rv;

}




/* Adding ports to MC group*/
int add_ingress_multicast_forwarding(int unit, int ipmc_index, int *ports , int nof_ports, int vlan){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {
        rv = bcm_multicast_ingress_add(unit,ipmc_index, ports[i]  ,vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", ports[i] ,vlan);
            return rv;
        }

    }

    return rv;
}



/* adding ports to VLAN flood MC group
 * In the ipmc flows hereby presented, flooding is determined by an MC group id which is in fact vsi
 */
int add_ingress_multicast_flooding(int unit, int vlan, int *ports , int nof_ports){

    int rv = BCM_E_NONE;
    int i;

    for (i=0; i < nof_ports; i++) {

        printf("port[i]: %d\n", ports[i]);
        rv = bcm_multicast_ingress_add(unit,vlan, ports[i]  ,0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_multicast_ingress_add: port %d vlan: %d \n", ports[i]   ,vlan);
            return rv;
        }

    }

    return rv;
}


/*  Create L3 Intf
 *  vrf <0 implies we create a rif with no vrf
*/
int create_rif(bcm_mac_t mac_address, int unit, int intf_id, bcm_l3_intf_t* intf, bcm_vrf_t vrf){

    int rv = BCM_E_NONE;
    bcm_l3_intf_t intf_ori;
  
    /* Create L3 Intf and disable its IPMC */

    bcm_l3_intf_t_init(intf);

    /* before creating L3 INTF, check whether L3 INTF already exists*/
    bcm_l3_intf_t_init(&intf_ori);
    intf_ori.l3a_intf_id = intf_id;
    rv = bcm_l3_intf_get(unit, &intf_ori);
    if (rv == BCM_E_NONE) {
        /* if L3 INTF already exists, replace it*/
        intf.l3a_flags = BCM_L3_REPLACE | BCM_L3_WITH_ID;
        intf.l3a_intf_id = intf_id;
    }
 
    sal_memcpy(intf->l3a_mac_addr, mac_address, 6);

    intf->l3a_vid = intf->l3a_intf_id = intf_id;

    if (vrf >= 0) {
        intf->l3a_vrf = vrf;
    }

    rv = bcm_l3_intf_create(unit, intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create \n");
        return rv;
    }

    return rv;

}


/* Enable/Disable IPMC for RIF
 * For enabling, pass enable == 1
 * For disabling, pass enable == 0
*/
int enable_rif_ipmc(int unit, bcm_l3_intf_t* intf, int enable){

    int rv = BCM_E_NONE;
    bcm_l3_ingress_t ingress_intf;
    bcm_l3_intf_t aux_intf;

    aux_intf.l3a_vid = intf->l3a_vid;
    aux_intf.l3a_mac_addr = intf->l3a_mac_addr;

    bcm_l3_ingress_t_init(&ingress_intf);
    ingress_intf.flags = BCM_L3_INGRESS_WITH_ID;

    if (!enable) {
        ingress_intf.flags |= BCM_L3_INGRESS_ROUTE_DISABLE_IP4_MCAST | BCM_L3_INGRESS_ROUTE_DISABLE_IP6_MCAST;
    }

    rv = bcm_l3_intf_find(unit, &aux_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_find\n");
        return rv;
    }


    ingress_intf.vrf = aux_intf.l3a_vrf;


    rv = bcm_l3_ingress_create(unit, &ingress_intf, intf->l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_create\n");
        return rv;
    }

    return rv;
}


/* In our context, this function configures one of the following two states:
 * 1. RIF.IPMC disabled: the function configures the lookup in LEM:
 *    In this case, we put bcmSwitchL3McastL2.
 *    if enabled == 0, we perform a lookup with <FID,DA>
 *    if enabled == 1, we perform a lookup with <FID,DIP>
 *
 * 2. RIF.IPMC enabled: the function configures the packet's forwarding following a miss in LEM and TCAM
 *    In this case, we put bcmSwitchUnknownIpmcAsMcast.
 *    if enabled == 0, the packet is L3 MC forwarded with default vrf.
 *    if enabled == 1, the packet is L2 flooded on the vsi.
 *
*/
int switch_control_set(int unit, bcm_switch_control_t switch_control, int enabled){

    int rv;

    rv = bcm_switch_control_set(unit,switch_control,enabled); \
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    return rv;

}


/* Creates a L2 forwarding entry. The entry will be accessed by LEM<FID,DA> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a destination MAC address. The packet will exit with vid == vlan.
 */
int create_l2_forwarding_entry_da(int unit, bcm_mac_t mac, int l2mc_group,int vlan){

    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    /* Configurations relevant for LEM<FID,DA> lookup*/
    l2_addr.flags = BCM_L2_MCAST;
    l2_addr.l2mc_group = l2mc_group;
    l2_addr.vid = vlan;
    l2_addr.mac = mac;

    rv = bcm_l2_addr_add(unit,&l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add \n");
        return rv;
    }

    return rv;

}


/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup                                   .
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.                                                                                                                                                                                                                            .
 */
int create_forwarding_entry_dip_sip(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan){

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;

    if (src_ip != -1) {
        data.s_ip_addr = src_ip;
		data.s_ip_mask = 0xffffffff;
    }
  
    data.vid = vlan;
    data.flags = 0x0;
    data.group = ipmc_index;
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip \n");
        return rv;
    }

    return rv;

}

/* Don't use create_forwarding_entry_dip_sip() because it isn't working well as description */
int create_forwarding_entry_dip_sip2(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan){

    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;
    data.s_ip_addr = src_ip;
	if ((src_ip != 0)) {
		data.s_ip_mask = 0xffffffff;
	}
    data.vid = vlan;
    data.flags = 0;
    data.group = ipmc_index;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip \n");
        return rv;
    }

    return rv;
}


/* Creates a L3 forwarding entry.
 * vrf != 0: The entry will be accessed by LEM<VRF,DIP> Lookup.
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip and vrf.                                                                                                                                  .
 * vrf == 0: The entry will be accessed following a miss in LEM<VRF,DIP> and TCAM<RIF,SIP,DIP> lookup and                                                                                                                                                                                                                                      .
 * bcmSwitchUnknownIpmcAsMcast == 0.                                                                                                                                                                                                                                                                                                                                             .
 */
int create_forwarding_entry_vrf(int unit, bcm_ip_t mc_ip, int ipmc_index, int vrf, bcm_mac_t mac,  int rif){

    int rv = BCM_E_NONE;
    bcm_l3_host_t data;
    bcm_ipmc_addr_t info;

    /* Create the entry */

    if (vrf != 0) {
        bcm_l3_host_t_init(&data);
        data.l3a_flags = 0x0;
        data.l3a_vrf = vrf;
        data.l3a_intf = rif;
        data.l3a_ip_addr = mc_ip;
        data.l3a_nexthop_mac = mac;
        BCM_GPORT_MCAST_SET(data.l3a_port_tgid, ipmc_index);

        rv = bcm_l3_host_add(unit, &data);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l3_host_add\n");
            return rv;
        }
    }
    else{ /* vrf==0, so we create a default vrf forwarding*/

        bcm_ipmc_addr_t_init(&info);

        info.vid = rif;
        info.flags = 0x0;
        info.group = ipmc_index;
        info.vrf = vrf;
        /* Creates the entry */
        rv = bcm_ipmc_add(unit,&info);

    }


    return rv;

}




/*
*  This function exhibits the usage of an ipmc flow that is determined by the unsetting of the RIF IPMC. This flow includes four sub-flows,
*  which are employed by different sections in the test.
*
*  Section 1: Disables RIF IPMC to enable L2 MC forwarding.
*  Section 2: Creating a vsi to enable VLAN MC flooding.
*  Section 3: Creating MC groups: first two groups are the destination of L2 MC forwarding. The other two are the destination of VLAN MC flooding.
*  Section 4: Sets information for MC forwarding upon successful hit in LEM. The entries are built in Section 7.
*  Section 5: Sets information for VLAN MC flooding upon miss in LEM. In these cases, packets will be sent with a dip and/or dst_mac that won't be matched in LEM.
*  Section 6: Configures the lookup in LEM: bcmSwitchL3McastL2 == 1 leads to <FID,DIP> lookup, while bcmSwitchL3McastL2 == 0 leads to <FID,DA> lookup.
*  Section 7: Configuring the LEM entries that will be matched with (Multicast compatible) dip or dst_mac.
*
*  We present the following sub-flows:
*
*  1. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is successful: packet is sent with dip that is matched to the entry. Upon this successful hit,
*  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan1.
*  Relevant sections: 1.1, 3.1, 4.1, 6, 7.1.
*
*  2. bcmSwitchL3McastL2 ==	0 && lookup in LEM<FID,DA> is successful: packet is sent with dst_mac that is matched to the entry. Upon this successful hit,
*  the packet will be forwarded according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3 with vid == vlan2.
*  Relevant sections: 1.2, 3.2, 4.2, 6, 7.2.
*
*  3. bcmSwitchL3McastL2 == 1 && lookup in LEM<FID,DIP> is unsuccessful: packet is sent with dip that is unmatched to the entry. To reproduce this
*  scenario, simply send a packet with a different mc_ip than the one  configured in the function. Upon this miss,the packet will be flooded according
*  to the VLAN flood MC group created in this function (vlan == vlan3). It will arrive at ports outP, outP2 with vid == vlan3.
*  Relevant sections: 2.1, 3.3, 5.1, 6.
*
*  4. bcmSwitchL3McastL2 == 0 && lookup in LEM<FID,DA> is unsuccessful: packet is sent with dst_mac that is unmatched to the entry. To reproduce this
*  scenario, simply send a packet with a different mac than the one configured in the function .Upon this miss,
*  the packet will be flooded according to the VLAN flood MC group created in this function (vlan == vlan4). It will arrive at ports outP, outP2 with vid == vlan4.
*  Relevant sections: 2.2, 3.4, 5.2, 6.
*/
int ipmc_flows_rif_ipmc_disabled(int unit, int inP, int outP, int outP2 ,int outP3 ,int vlan1, int vlan2, int vlan3, int vlan4){

    int rv = BCM_E_NONE;
    int SwitchL3McastL2_enabled = 1;

    bcm_l3_intf_t intf1, intf2;

    ipmc_flows_info1.ipmc_indices[6] = vlan3;
    ipmc_flows_info1.ipmc_indices[7] = vlan4;

    ipmc_flows_init(outP, outP2, outP3);


    /* Section 1
     * create a rif, then disable ipmc
     */
    rv = create_rif(mac_address_1, unit , vlan1, &intf1,-1);/* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif\n");
        return rv;
    }

    rv = create_rif(mac_address_2, unit , vlan2, &intf2,-1);/* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf1, 0); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf2, 0); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }


    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan3); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vlan4); /* 2.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }


    /* Section 3:
     * creating MC groups
     */

    /* Create the L2 MC Group with ipmc_index */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with l2mc_group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[5])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan3 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &vlan3); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan4 */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID, &vlan4); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }


    /* Section 4:
     * Setting inforamtion relevant to MC forwarding, upon successful hits in LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<FID,DIP> lookup */
    rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[0], ipmc_flows_info1.forwarding_ports_1, 3, vlan1); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding \n");
        return rv;
    }

    /* set information relevant to MC forwarding: l2mc_group will connect the MC group to LEM<FID,DA> lookup*/
    rv = add_ingress_multicast_forwarding(unit, ipmc_flows_info1.ipmc_mc_groups[5], ipmc_flows_info1.forwarding_ports_1, 3, vlan2); /* 4.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_l2_ingress_multicast_forwarding \n");
        return rv;
    }


    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM
     */

    /* set information relevant to MC flooding with vid == vlan3*/
    rv = add_ingress_multicast_flooding(unit, vlan3, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to MC flooding with vid == vlan4*/
    rv = add_ingress_multicast_flooding(unit, vlan4,  ipmc_flows_info1.forwarding_ports_2, 2); /* 5.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }


    /* Section 6:
     * Configuring the lookup in LEM:
     * if SwitchL3McastL2_enabled == 0, we perform a lookup with <FID,DA>
     * if SwitchL3McastL2_enabled == 1, we perform a lookup with <FID,DIP>
    */
    rv = bcm_switch_control_set(unit,bcmSwitchL3McastL2,SwitchL3McastL2_enabled);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }


    /* Section 7:
     * defining entries for LEM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[0],0x0,ipmc_flows_info1.ipmc_mc_groups[0],vlan1); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip\n");
        return rv;
    }

    rv = create_l2_forwarding_entry_da(unit,mac1,ipmc_flows_info1.ipmc_mc_groups[5],vlan2); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_l2_forwarding_entry_da\n");
        return rv;
    }


    return rv;
}



/*
*  This function exhibits the usage of an ipmc flow that is determined by the setting of the RIF IPMC. This flow includes six sub-flows,
*  which are employed by different sections in the test.
*
*  Section 1: Enables RIF IPMC to enable MC forwarding.
*  Section 2: Creating a vsi to enable VLAN MC flooding.
*  Section 3: Creating MC groups: first four groups are the destination of L3 MC forwarding.
*             The other two are the destination of VLAN MC flooding and L3 forwarding with default VRF.
*  Section 4: Sets information for MC forwarding upon successful hit in LEM and/or TCAM. The entries are built in Section 7.
*  Section 5: Sets information for VLAN MC flooding upon miss in LEM and TCAM or L3 forwarding with default vrf.
*             In these cases, packets will be sent with a dip that won't be matched in LEM and TCAM.
*  Section 6: Configures the sub flow after a miss in LEM and/or TCAM:
*             bcmSwitchUnknownIpmcAsMcast == 1 leads to flooding on vsi, while bcmSwitchUnknownIpmcAsMcast == 0
*             leads to L3 MC forwarding with default vrf.
*  Section 7: Configuring the entries that will be matched with (Multicast compatible) dip (and/or sip in TCAM).
*             This also includes forwarding with default vrf.
*
*  We present the following sub-flows:
*
*  1. If soc property ipmc_vpn_lookup_enable == 1,
*      Packet arrives with a dip that matches a LEM entry. The packet will be forwarded, upon the successful hit,
*      according to the multicast group created in this function. It will arrive at ports outP, outP2, outP3.
*      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with sip + dip that matches a LEM entry.
*      The packet will be forwarded, upon the successful hit, according to the multicast group created in this function.
*      It will arrive at ports outP, outP2, outP3.
*    Relevant sections: 1.1, 3.1, 4.1, 7.4.
*
*  2. Packet arrives with a dip and sip that matches a TCAM entry, configured with DIP + SIP. The packet will be forwarded, upon the successful hit,
*   according to the multicast group created in this function. It will arrive at ports outP2, outP3.
*   Relevant sections: 1.2, 3.2, 4.2, 7.1.
*
*  3. Packet arrives with only a dip that matches a TCAM entry . The packet will be forwarded, upon the successful hit,
*   according to the multicast group created in this function. It will arrive at ports outP, outP2.
*   Relevant sections: 1.3, 3.3, 4.3, 7.2.
*
*  4. If soc property ipmc_vpn_lookup_enable == 1,
*      Packet arrives with a dip that matches both LEM and TCAM entries . The packet will be forwarded, upon the successful hit,
*      according to the multicast group that is defined in the TCAM entry. It will arrive at ports outP, outP3.
*      Otherwise(ipmc_vpn_lookup_enable == 0), Packet arrives with a dip that matches TCAM entries, and sip+dip that matches LEM entries.
*      Because TCAM > LEM in priority in case both match, the packet will be forwarded, upon the successful hit
*      according to the multicast group that is defined in the TCAM entry.It will arrive at ports outP, outP3.
*    Relevant sections: 1.4, 3.4, 4.4, 7.3.
*
*  5. Packet arrives with a dip that is unmatched in the LEM and TCAM . Upon setting bcmSwitchUnknownIpmcAsMcast == 1,
*  The packet will be L2 flooded on the given vsi (vlan). It will arrive at ports outP, outP2.
*  sections: 2.1, 3.5, 5.1, 6.1.
*
*  6. Packet arrives with a dip that is unmatched in the LEM and TCAM . Upon setting bcmSwitchUnknownIpmcAsMcast == 0,
*  The packet will be L3 forwarded on the given vrf. It will arrive at ports outP, outP2.
*  sections: 1.5, 3.6, 6.1 (unset bcmSwitchUnknownIpmcAsMcast), 7.5.
*/
int ipmc_flows_rif_ipmc_enabled(int unit, int inP, int outP, int outP2 ,int outP3 ,int rif1, int rif2, int rif3, int rif4, int rif5, int vlan){

    int rv = BCM_E_NONE;
    int SwitchUnknownIpmcAsMcast_enabled = 1;

    bcm_l3_intf_t intf1, intf2, intf3, intf4, intf5;
    bcm_vrf_t vrf = 2;

    ipmc_flows_info1.ipmc_indices[6] = vlan;
    ipmc_vpn_lookup_enable = soc_property_get(unit , "ipmc_vpn_lookup_enable", 1);

    ipmc_flows_init(outP, outP2, outP3);

    /* Section 1
     * create a rif, then enable ipmc
     */
    rv = create_rif(mac_address_1, unit , rif1, &intf1, vrf); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    rv = create_rif(mac_address_2, unit , rif2, &intf2,vrf); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    rv = create_rif(mac_address_3, unit , rif3, &intf3, vrf); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    rv = create_rif(mac_address_4, unit , rif4, &intf4, vrf); /* 1.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }

    rv = create_rif(mac_address_5, unit , rif5, &intf5, vrf); /* 1.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_rif_with_vrf\n");
        return rv;
    }


    rv = enable_rif_ipmc(unit,&intf1, 1); /* 1.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf2, 1); /* 1.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf3, 1); /* 1.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf4, 1); /* 1.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    rv = enable_rif_ipmc(unit,&intf5, 1); /* 1.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, enable_rif_ipmc, enable == 0\n");
        return rv;
    }

    /* Section 2:
     * creating a vsi; will be used as a parameter for L2 flooding
     */
    rv = bcm_vswitch_create_with_id(unit, vlan); /* 2.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_create\n");
        return rv;
    }

    /* Section 3:
     * creating MC groups
     */

    /* In the following 4 calls we create the L3 MC Group with an index taken from the main struct's data structure */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[0])); /* 3.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[1])); /* 3.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[2])); /* 3.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[3])); /* 3.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L2 MC Group with vlan; This group will be a flooding domain with vid == vlan */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &vlan); /* 3.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Create the L3 MC Group with default vrf */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &(ipmc_flows_info1.ipmc_mc_groups[4])); /* 3.6 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    /* Section 4:
     * Setting inforamtion relevant to L3 MC forwarding, upon successful hits in TCAM and/or LEM
     */

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to LEM<vrf,dip> lookup */
    rv = add_ingress_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[0],ipmc_flows_info1.forwarding_ports_1, 3 ,rif1); /* 4.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }
    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip> lookup with sip + dip given */
    rv = add_ingress_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[1],ipmc_flows_info1.forwarding_ports_2, 2 ,rif2); /* 4.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip> lookup with only dip given */
    rv = add_ingress_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[2],ipmc_flows_info1.forwarding_ports_3, 2 ,rif3); /* 4.3 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }

    /* set information relevant to MC forwarding: ipmc_index will connect the MC group to TCAM<rif,sip,dip>, LEM<vrf,dip> lookup. TCAM is chosen. */
    rv = add_ingress_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[3],ipmc_flows_info1.forwarding_ports_4, 2 ,rif4); /* 4.4 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }

    /* Section 5:
     * Setting inforamtion relevant to VLAN MC flooding, upon misses in LEM and TCAM
     */

    /* set information relevant to MC flooding with vid == vlan*/
    rv = add_ingress_multicast_flooding(unit, vlan, ipmc_flows_info1.forwarding_ports_2, 2); /* 5.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_flooding \n");
        return rv;
    }

    /* set information relevant to L3 MC forwarding with default vrf,  vid == rif5*/
    rv = add_ingress_multicast_forwarding(unit,ipmc_flows_info1.ipmc_mc_groups[4],ipmc_flows_info1.forwarding_ports_2, 2 ,rif5); /* 5.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, add_ingress_multicast_forwarding\n");
        return rv;
    }

    /* Section 6:
     * Configuring the sub flow taken after a miss in LEM and TCAM:
     * if bcmSwitchUnknownIpmcAsMcast == 1, the packet will be flooded on vsi.
     * if bcmSwitchUnknownIpmcAsMcast == 0, the packet will be L3 MC forwarded with default vrf.
    */
    rv = bcm_switch_control_set(unit,bcmSwitchUnknownIpmcAsMcast,SwitchUnknownIpmcAsMcast_enabled); /* 6.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set \n");
        return rv;
    }

    /* Section 7:
     * defining entries for LEM and TCAM which will forward the packet to the relevant mc group
     */
    rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[1],ipmc_flows_info1.src_ip_arr[1],ipmc_flows_info1.ipmc_mc_groups[1],rif2); /* 7.1 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip, step - 7.1\n");
        return rv;
    }

    rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[1],-1,ipmc_flows_info1.ipmc_mc_groups[2],rif3); /* 7.2 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip, step - 7.2\n");
        return rv;
    }

    if (ipmc_vpn_lookup_enable) {
        rv = create_forwarding_entry_dip_sip(unit,ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.src_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[3],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3\n");
            return rv;
        }

        rv = create_forwarding_entry_vrf(unit, ipmc_flows_info1.mc_ip_arr[0],ipmc_flows_info1.ipmc_mc_groups[0], vrf, mac2,rif1); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_vrf, step - 7.4\n");
            return rv;
        }
    } else {
        /* Add a none-exact match IPMC entry in TCAM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],0,ipmc_flows_info1.ipmc_mc_groups[3],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.1\n");
            return rv;
        }
        /* Add a exact match IPMC entry in LEM */
        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[3],ipmc_flows_info1.src_ip_arr[3],ipmc_flows_info1.ipmc_mc_groups[0],rif4); /* 7.3 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.3.2\n");
            return rv;
        }

        rv = create_forwarding_entry_dip_sip2(unit,ipmc_flows_info1.mc_ip_arr[2],ipmc_flows_info1.src_ip_arr[2],ipmc_flows_info1.ipmc_mc_groups[0],rif1); /* 7.4 */
        if (rv != BCM_E_NONE) {
            printf("Error, create_forwarding_entry_dip_sip, step - 7.4\n");
            return rv;
        }
    }

    rv = create_forwarding_entry_vrf(unit, 0 ,ipmc_flows_info1.ipmc_mc_groups[4],0, mac2,rif5);  /* 7.5 */
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_vrf\n");
        return rv;
    }

    return rv;
}

