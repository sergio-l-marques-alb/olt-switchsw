/* ~~~~~~~~~~~~~~~~~~  BFDv6 Max Sessions(back-to-back) ~~~~~~~~~~~~~~~~~~ */
/*
 * 
 * $Id: cint_bfd_ipv6_max_sessions.c,v 1.15 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 
 * SOC Properties:
 * #===============================
 * # OAM SOC
 * num_oamp_ports=1
 * tm_port_header_type_out_232.BCM88650=CPU
 * tm_port_header_type_out_0.BCM88650=CPU
 * ucode_port_40.BCM88650=RCY.0
 * oam_rcy_port=40
 * tm_port_header_type_in_40.BCM88650=TM
 * tm_port_header_type_out_40.BCM88650=ETH
 * counter_engine_source_0.BCM88650=INGRESS_OAM
 * counter_engine_source_1.BCM88650=EGRESS_OAM
 * counter_engine_statistics_0.BCM88650=ALL
 * counter_engine_statistics_1.BCM88650=ALL
 * counter_engine_format_0.BCM88650=PACKETS
 * counter_engine_format_1.BCM88650=PACKETS
 * phy_1588_dpll_frequency_lock.BCM88650=1
 * phy_1588_dpll_phase_initial_lo.BCM88650=0x40000000
 * phy_1588_dpll_phase_initial_hi.BCM88650=0x10000000
 * bcm886xx_next_hop_mac_extension_enable.BCM88650=0
 * mplstp_g8113_channel_type=0x8902
 * bcm886xx_ipv6_tunnel_enable=0
 * 
 * #---------------------
 * # BFD IPv6 SOC
 * mcs_load_uc0=1
 * bfd_ipv6_enable=1
 * bfd_ipv6_trap_port=204
 * num_queues_pci=16
 * num_queues_uc0=8
 * bfd_simple_password_keys=8
 * bfd_sha1_keys=8
 * port_priorities_13.BCM88650=2
 * port_priorities_14.BCM88650=2
 * port_priorities_204.BCM88650=8
 * ucode_port_204.BCM88650=CPU.16:core_0.204
 * tm_port_header_type_in_204.BCM88650=INJECTED_2_PP
 * tm_port_header_type_out_204.BCM88650=CPU
 * bfd_num_sessions=1001
 * bfd_echo_enabled=1
 *
 * Test Environment:
 *     Unit 1 port 14 ------ Unit 2 port 13
 *
 *
 *   cd ../../../../ 
 *   cint src/examples/dpp/utility/cint_utils_global.c
 *   cint src/examples/dpp/utility/cint_utils_mpls.c
 *   cint src/examples/dpp/utility/cint_utils_l3.c
 *   cint src/examples/dpp/utility/cint_utils_vlan.c
 *   cint src/examples/dpp/cint_ip_route.c
 *   cint src/examples/dpp/cint_bfd_ipv6.c
 *   cint src/examples/dpp/cint_field_bfd_ipv6_single_hop.c
 *   cint src/examples/dpp/cint_bfd_ipv6_max_sessions.c
 *   c
 *
 * Test 64@3.3ms + 256@10ms
 *  on unit 1:
 *   print execute_on_unit1(0,64,256,14,1,1,0,0,bcmBFDAuthTypeKeyedSHA1,3); 
 *  on unit 2:
 *   print execute_on_unit2(0,64,256,13,1,1,0,0,bcmBFDAuthTypeKeyedSHA1,3);
 *
 * Test 120 session @ 3.3ms
 *  on unit 1:
 *   print sessions_create_unit1(0,120,14,1,1,0,0,10,5010,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);
 *  on unit 2:
 *   print sessions_create_unit2(0,120,13,1,1,0,0,5010,10,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);
 *
 * Enable BFD Event Register
 *   print bfd_ipv6_event_register(0);
 *
 */

int unit=0;
int session_at_10=256;
int session_at_33=64;
int i;
/* 
* bfd_num_sessions defined the max sessions, if this soc is not configured, the default value is 256.
*/
int max_session=soc_property_get(unit, "bfd_num_sessions", 0);
if (max_session == 0) {
    max_session = 256;
} 
bcm_bfd_endpoint_info_t bfd_endpoint_info[max_session];

int bcm_bfd_cb_new(int unit, uint32 flags, bcm_bfd_event_types_t events_array, bcm_bfd_endpoint_t epid, void *user_data) { 
    bcm_bfd_endpoint_info_t ep; 
    bcm_bfd_endpoint_info_t_init(&ep);     

    int rv = 0; 
    int event; 
    
    /* workaround for the epid returned by system is not the one we have in the system */
    int id=0x800000 + epid;
    
    rv = bcm_bfd_endpoint_get(unit, id, &ep); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    for (event = bcmBFDEventStateChange; event < bcmBFDEventCount; event++) { 
        if (BCM_BFD_EVENT_TYPE_GET(events_array, event)) { 
            switch (event) { 
            case bcmBFDEventStateChange: 
                print "bcmBFDEventStateChange"; 
                print "============id=========";
                print ep.id;
                print "============local=========";
                print ep.local_state;
                print "============remote=========";
                print ep.remote_state;
                break; 
            case bcmBFDEventRemoteStateDiag: 
                print "bcmBFDEventRemoteStateDiag"; 
                break; 
            case bcmBFDEventSessionDiscriminatorChange: 
                print "bcmBFDEventSessionDiscriminatorChange"; 
                break; 
            case bcmBFDEventAuthenticationChange: 
                print "bcmBFDEventAuthenticationChange"; 
                break; 
            case bcmBFDEventParameterChange: 
                print "bcmBFDEventParameterChange"; 
                break; 
            case bcmBFDEventSessionError: 
                print "bcmBFDEventSessionError"; 
                break; 
            default: 
                print "=================Error: Unknown event, why================="; 
                print event; 
                break; 
           } 
        } 
    } 
    print "***************Events Ends***************"; 
    return rv; 
} 

void print_ipv6_addr(char *type, bcm_ip6_t a)
{
  printf("%s = ",type);
  printf("%x:", ((a[0] << 8) | a[1]));
  printf("%x:", ((a[2] << 8) | a[3]));
  printf("%x:", ((a[4] << 8) | a[5]));
  printf("%x:", ((a[6] << 8) | a[7]));
  printf("%x:", ((a[8] << 8) | a[9]));
  printf("%x:", ((a[10] << 8) | a[11]));
  printf("%x:", ((a[12] << 8) | a[13]));
  printf("%x\n", ((a[14] << 8) | a[15]));
}

/* 
 *creating l3 interface - ingress/egress
 */
int bfd_ipv6_intf_init(int unit, int in_vlan, int out_vlan, bcm_mac_t mac_address, bcm_mac_t next_hop_mac, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6){
    
    int rv;
    int flags = 0;    
    int open_vlan=1;

    int vrf = 0;    
    int ing_intf_in; 
    int ing_intf_out; 
    int l3_eg_int;    
    int encap_id; 
    int i;

    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, bfd_in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", bfd_in_port, in_vlan);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, bfd_out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", bfd_out_port, out_vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    l3_eg_int = ing_intf_out;  
   
/*   rv = create_l3_intf(unit, flags, open_vlan, bfd_in_port, in_vlan, vrf, mac_address, &ing_intf_in); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface in unit %d \n", unit);
    }

    rv = create_l3_intf(unit, flags, open_vlan, bfd_out_port, out_vlan, vrf, mac_address, &ing_intf_out); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface in unit %d \n", unit);
    }
    l3_eg_int = ing_intf_out;
*/
    /*** Create egress object1 ***/
    rv = create_l3_egress(unit, flags, bfd_out_port, out_vlan, l3_eg_int, next_hop_mac, &bfd_fec, &encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", sysport, unit);
    }
	
    rv = add_ipv6_host(unit, &src_ipv6, vrf, bfd_fec, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", sysport, unit);
    }
    
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", bfd_fec, unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id, unit);
    }
    
    return rv;
}


int bfd_ipv6_ep_create(int unit, int session, int bfd_fec, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6, int endpoint_id, 
                        int is_single_hop, int is_acc, int is_passive, int is_demand, int udp_src_port, bcm_bfd_auth_type_t auth_type, 
                        int auth_index, uint32 local_disc, uint32 remote_disc, bcm_cos_t priority, uint32 local_min_tx, uint32 local_min_rx) {
    int rv;
    int is_echo=soc_property_get(unit, "bfd_echo_enabled", 0);

    /* Adding BFDoIPV6 one hop endpoint */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info[session]);

    bfd_endpoint_info[session].flags = BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IPV6;
    if (!is_single_hop) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }
    if (is_acc) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_IN_HW;
    }
    if (is_passive) {
         bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_PASSIVE;
    }
    if (is_demand) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_DEMAND;
    }
   if (is_echo) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ECHO; 
    }
    
    bfd_endpoint_info[session].id = endpoint_id;
    bfd_endpoint_info[session].type = bcmBFDTunnelTypeUdp;
    bfd_endpoint_info[session].ip_ttl = 255;
    bfd_endpoint_info[session].ip_tos = 0;
    sal_memcpy(&(bfd_endpoint_info[session].src_ip6_addr), &src_ipv6, 16);
    sal_memcpy(&(bfd_endpoint_info[session].dst_ip6_addr), &dst_ipv6, 16);
    bfd_endpoint_info[session].udp_src_port = udp_src_port;
    bfd_endpoint_info[session].auth_index = 0;
    bfd_endpoint_info[session].auth = bcmBFDAuthTypeNone;
    bfd_endpoint_info[session].local_discr = local_disc;
    bfd_endpoint_info[session].local_state = bcmBFDStateDown;
    bfd_endpoint_info[session].local_min_tx = local_min_tx;
    bfd_endpoint_info[session].local_min_rx = local_min_rx;
    bfd_endpoint_info[session].local_detect_mult = 3;
    bfd_endpoint_info[session].remote_discr = remote_disc;
    bfd_endpoint_info[session].egress_if = bfd_fec;
    bfd_endpoint_info[session].auth = auth_type;
    bfd_endpoint_info[session].int_pri = priority;
    if (auth_type != bcmBFDAuthTypeNone) {
        bfd_endpoint_info[session].auth_index = auth_index;
    }  

    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info[session]);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if(verbose >= 1) {
        printf("Created an endpoint with  \n   ***  endpoint_id 0x%x \n", bfd_endpoint_info[session].id);
        printf("   *** local_disc is %d\n",  bfd_endpoint_info[session].local_discr);
        printf("   *** udp_src_port is %d\n", bfd_endpoint_info[session].udp_src_port);
        printf("   *** local_mix_tx is %d, local_min_rx is %d\n", bfd_endpoint_info[session].local_min_tx,bfd_endpoint_info[session].local_min_rx );
        print_ipv6_addr("   *** src_ip6_addr",bfd_endpoint_info[session].src_ip6_addr);
        print_ipv6_addr("   *** dst_ip6_addr",bfd_endpoint_info[session].dst_ip6_addr);
    }
    return rv;
}

int bfd_ipv6_ep_update(int unit, int session, int bfd_fec, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6, 
                        int endpoint_id, int is_single_hop, int is_acc, int is_passive, int is_demand, int udp_src_port, bcm_bfd_auth_type_t auth_type, 
                        int auth_index, uint32 local_disc, uint32 remote_disc, bcm_cos_t priority, uint32 local_min_tx, uint32 local_min_rx) {
    int rv;
    int is_echo=soc_property_get(unit, "bfd_echo_enabled", 0);
    
    /* Adding BFDoIPV6 one hop endpoint */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info[session]);

    bfd_endpoint_info[session].flags = BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IPV6 | BCM_BFD_ENDPOINT_UPDATE;
    if (!is_single_hop) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_MULTIHOP;
    }
    if (is_acc) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_IN_HW;
    }
    if (is_passive) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_PASSIVE;
    }
    if (is_demand) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ENDPOINT_DEMAND;
    }
   if (is_echo) {
        bfd_endpoint_info[session].flags |= BCM_BFD_ECHO; 
    }
    bfd_endpoint_info[session].id = endpoint_id;
    bfd_endpoint_info[session].type = bcmBFDTunnelTypeUdp;
    bfd_endpoint_info[session].ip_ttl = 255;
    bfd_endpoint_info[session].ip_tos = 0;
    sal_memcpy(&(bfd_endpoint_info[session].src_ip6_addr), &src_ipv6, 16);
    sal_memcpy(&(bfd_endpoint_info[session].dst_ip6_addr), &dst_ipv6, 16);
    bfd_endpoint_info[session].udp_src_port = udp_src_port;
    bfd_endpoint_info[session].auth_index = 0;
    bfd_endpoint_info[session].auth = bcmBFDAuthTypeNone;
    bfd_endpoint_info[session].local_discr = local_disc;
    bfd_endpoint_info[session].local_state = bcmBFDStateDown;
    bfd_endpoint_info[session].local_min_tx = local_min_tx;
    bfd_endpoint_info[session].local_min_rx = local_min_rx;
    bfd_endpoint_info[session].local_detect_mult = 3;
    bfd_endpoint_info[session].remote_discr = remote_disc;
    bfd_endpoint_info[session].egress_if = bfd_fec;
    bfd_endpoint_info[session].auth = auth_type;
    bfd_endpoint_info[session].int_pri = priority;
    if (auth_type != bcmBFDAuthTypeNone) {
        bfd_endpoint_info[session].auth_index = auth_index;
    }  

    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info[session]);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_create (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if(verbose >= 1) {
        printf("\n\n Created endpoint with  \n   ***  endpoint_id 0x%x \n", bfd_endpoint_info[session].id);
        printf("   *** local_disc is %d\n",  bfd_endpoint_info[session].local_discr);
        printf("   *** udp_src_port is %d\n", bfd_endpoint_info[session].udp_src_port);
        printf("   *** local_mix_tx is %d, local_min_rx is %d\n", bfd_endpoint_info[session].local_min_tx,bfd_endpoint_info[session].local_min_rx );
        print_ipv6_addr("   *** src_ip6_addr",bfd_endpoint_info[session].src_ip6_addr);
        print_ipv6_addr("   *** dst_ip6_addr",bfd_endpoint_info[session].dst_ip6_addr);
    }
    
    
    printf("\n\n Verify if the endpoint %d(0x%x)is updated correctly.\n", endpoint_id, bfd_endpoint_info[session].id);

    bcm_bfd_endpoint_info_t bfd_endpoint_test_info;
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_test_info);
    printf("bcm_bfd_endpoint_get bfd_endpoint_test_info\n");   
    rv = bcm_bfd_endpoint_get(0, bfd_endpoint_info[session].id, &bfd_endpoint_test_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Here &bfd_endpoint_info[session] and &bfd_endpoint_test_info are compared */
    rv = cmp_structs(&bfd_endpoint_info[session], &bfd_endpoint_test_info,bcmBFDTunnelTypeUdp ) ;
    if (rv != 0) {
        printf("%d wrong fields in BFDoIPV6\n", rv);
        printf("Failed in cmp_structs, some parameter is not updated for endpoint %d(%x). \n", endpoint_id, bfd_endpoint_info[session].id);
        return rv;
    } else {
        printf("\n\nEndpoint info check ok!\n");
    }
    
    return rv;
}


/*
* BFD session Event register
*/
int bfd_ipv6_event_register(int unit) {
    int rv;
    
   /* bcm_bfd_ipv6_events_register(0);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

int bfd_ipv6_ep_state_print(int unit, int epid) {
    int rv=0;
    bcm_bfd_endpoint_info_t ep; 
    bcm_bfd_endpoint_info_t_init(&ep); 

    int id=0x800000 + epid;
    rv=bcm_bfd_endpoint_get(unit, id, &ep); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print id;
    print ep.local_state;
    print ep.remote_state;
    print ep.udp_src_port;
    print_ipv6_addr("   *** src_ip6_addr",ep.src_ip6_addr);
    print_ipv6_addr("   *** dst_ip6_addr",ep.dst_ip6_addr);
    
    return rv;
}


int bfd_ipv6_ep_state_up(int unit, int epid) {
    int rv=0;
    bcm_bfd_endpoint_info_t ep; 
    bcm_bfd_endpoint_info_t_init(&ep); 

    int id=0x800000 + epid;
    rv=bcm_bfd_endpoint_get(unit, id, &ep); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_get (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    if (ep.local_state != bcmBFDStateUp || ep.remote_state != bcmBFDStateUp) {
        printf("\n\nState of endpoint %d is NOT UP:         \n", epid);
        print ep.local_state ;
        print ep.remote_state ;
        return epid;
    }
   
    return rv;
}

int bfd_ipv6_ep_destroy(int unit, int epid) {
    int rv=0;
    int id=0x800000 + epid;
    
    rv=bcm_bfd_endpoint_destroy(unit, id); 
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_destroy (%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    return rv;
    
}    


int bfd_ipv6_ep_stat_get(int unit, int epid, int seconds, bcm_bfd_endpoint_stat_t *stat) {
    int rv;
    int id=0x800000+epid;

    rv=bcm_bfd_endpoint_stat_get(0, id, stat, 1);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_stat_get (%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    
    sal_sleep(seconds);

    rv=bcm_bfd_endpoint_stat_get(0, id, stat, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_endpoint_stat_get (%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    return rv;    
}    
    
int sessions_recreate(int unit, int endpoint_id, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6, int port, int is_single_hop, 
                            int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc, bcm_bfd_auth_type_t auth_type, 
                            uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    int j;
   
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    
    /* BFD in/out Port Configuration */    
    rv=bfd_ipv6_service_init(0,port,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(0);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);
    
    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }    

    /* Re-Create BFD IPv6 Endpoint  */
    printf("\n### Re-create: endpoint %d at tx %d rx %d ###\n", endpoint_id,local_min_tx,local_min_rx);

    /* ip entry create */
    rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6);       
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_intf_init \n");
        print rv;
        return rv;
    }        
    j=endpoint_id-1;
    in_vlan = 2 + j;
    out_vlan = 200+ j;

    rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, local_min_tx, local_min_rx);   
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create \n");
        print rv;
        return rv;
    }     
    
    return rv;
}

int bfd_ipv6_ep_create_example(int unit, int port, int is_init, int is_single_hop, int is_acc, int is_passive, int is_demand, 
                                int endpoint_id, uint32 udp_src_port, bcm_bfd_auth_type_t auth_type, uint32 auth_index, int is_inject ) {
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
   
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    uint32 local_disc = 5010;
    uint32 remote_disc = 10;
    int priority = 5;
    
    uint32 local_min_tx = 10000;
    uint32 local_min_rx = 10000;
    
    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};
           
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    
    if (is_init == 1) {
        /* BFD in/out Port Configuration */    
        rv=bfd_ipv6_service_init(0,port,port);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_service_init\n");
            print rv;
            return rv;
        }
        /* BFD INIT */
        rv=bcm_bfd_init(0);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }    
    }
    
    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }
    
    /* ip entry create */
    rv=bfd_ipv6_intf_init(unit,in_vlan,out_vlan,mac_address,next_hop_mac,src_ipv6,dst_ipv6); 
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_intf_init \n");
        print rv;
        return rv;
    }
        
    /* bfd ipv6 endpoint create(session=0) */
    
    rv=bfd_ipv6_ep_create(unit,0,bfd_fec, src_ipv6, dst_ipv6, endpoint_id, is_single_hop, is_acc, is_passive, is_demand, udp_src_port, auth_type, 0, local_disc, remote_disc, priority, local_min_tx, local_min_rx);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create \n");
        print rv;
        return rv;
    }
    
    /* Verify the endpoint is created as expected or not */
    
    printf("\n\nVerify if the endpoint %d(0x%x)is configured correctly.\n", endpoint_id, bfd_endpoint_info[0].id);
    
    bcm_bfd_endpoint_info_t bfd_endpoint_test_info;
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_test_info);
    
    rv = bcm_bfd_endpoint_get(0, bfd_endpoint_info[0].id, &bfd_endpoint_test_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Here &bfd_endpoint_info[0] and &bfd_endpoint_test_info are compared */
    rv = cmp_structs(&bfd_endpoint_info[0], &bfd_endpoint_test_info,bcmBFDTunnelTypeUdp ) ;
    if (rv != 0) {
        printf("%d wrong fields in BFDoIPV6\n", rv);
        printf("Failed in cmp_structs, some parameter is not updated for endpoint %d(%x). \n", endpoint_id, bfd_endpoint_info[0].id);
        return rv;
    } else {
        printf("\n\nEndpoint info check ok!\n");
    }
    return rv;
    
        
    if (is_init == 1) {    
        rv=bfd_ipv6_single_hop_field_action(unit, is_inject);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_single_hop_field_action \n");
            print rv;
            return rv;
        }
    }
    
    return rv;  
        
} 


/*print execute_on_unit1(0,64,256,14,1,1,0,0,bcmBFDAuthTypeNone,3); */
int execute_on_unit1(int unit, int session_at_33, int session_at_10, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, bcm_cos_t priority)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;

    max_session=session_at_33+session_at_10;
    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    
    uint32 local_disc = 5010;
    uint32 remote_disc = 10;
    
    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};
           
    /* BFD in/out Port Configuration */    
    rv=bfd_ipv6_service_init(0,port,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD INIT */
    rv=bcm_bfd_init(0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(0);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);

    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    int endpoint_id=1;
    int udp_src_port=50000;    
    uint32 min_tx_rx;
    
    /* Create BFD IPv6 ENDPOINT@3.3ms */   
    for (i = 0; i < session_at_33; i++) { 
        printf("\n\n************ Created %d endpoint @3.3ms ************ \n", i);
        min_tx_rx=3300;  
        
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6); 
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }              
        
        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }      
        
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;    
    }
    
    src_ipv6[14]+=1;
    dst_ipv6[14]+=1;
    
    /* Create BFD IPv6 ENDPOINT@10ms */
    for (i = session_at_33; i < max_session; i++) {     
        printf("\n\n************ Created %d endpoint @10ms ************ \n", i);
        min_tx_rx=10000; 
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }        
        
        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }
        
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;  
    }        
    
    rv=bfd_ipv6_single_hop_field_action(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;
}


/*print execute_on_unit2(0,64,256,13,1,1,0,0,bcmBFDAuthTypeNone,3); */
int execute_on_unit2(int unit, int session_at_33, int session_at_10, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, bcm_cos_t priority)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
    
    max_session=session_at_33+session_at_10;
    
    bcm_mac_t next_hop_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    uint32 local_disc = 10;
    uint32 remote_disc = 5010;
    
    bcm_ip6_t src_ipv6= {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    bcm_ip6_t dst_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};
            
    /* BFD in/out Port Configuration */    
    rv=bfd_ipv6_service_init(0,port,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD INIT */
    rv=bcm_bfd_init(0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

   /* bcm_bfd_ipv6_events_register(0);*/
    bcm_bfd_event_types_t events;
    BCM_BFD_EVENT_TYPE_CLEAR_ALL(events);
    BCM_BFD_EVENT_TYPE_SET_ALL(events);
    
    rv=bcm_bfd_event_register(unit, events, bcm_bfd_cb_new, NULL);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_bfd_event_register (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    int endpoint_id=1;
    int udp_src_port=50000;    
    uint32 min_tx_rx;
    
    for (i = 0; i < session_at_33; i++) { 
        printf("\n\n************ Created %d endpoint @3.3ms ************ \n", i);
        min_tx_rx=3300;  
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6); 
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }      
        
        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }       
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;
    }
    
    src_ipv6[14]+=1;
    dst_ipv6[14]+=1;
    
    /* BFD SESSIONS @10ms */
    for (i = session_at_33; i < max_session; i++) {     
        printf("\n\n************ Created %d endpoint @10ms ************ \n", i);
        min_tx_rx=10000; 
        
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }        
        
        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, min_tx_rx, min_tx_rx);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }        
        
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;   
   }     
   
    rv=bfd_ipv6_single_hop_field_action(unit, 0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;

}



/*print sessions_create_unit1(0,120,14,1,1,0,0,10,5010,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);*/
int sessions_create_unit1(int unit, int session, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc, 
                            bcm_bfd_auth_type_t auth_type, uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority, 
                            int event_on, int is_inject)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;
   
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    
    /* BFD in/out Port Configuration */    
    rv=bfd_ipv6_service_init(0,port,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD INIT */
    rv=bcm_bfd_init(0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (event_on == 1) {
        rv=bfd_ipv6_event_register(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_event_register (%s) \n",bcm_errmsg(rv));
            return rv;
        } 
        printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");
    } else {
        printf ("\n\n ### BFD EVENT REGISTER is OFF... ###\n");
    }
    
    int endpoint_id=0;
    if (session > 10) {
        verbose=0;
    }
    /* Create BFD IPv6 Endpoint  */
    for (i = 0; i < session; i++) { 
        printf("\n### Create: session %d at tx %d rx %d ###\n", i,local_min_tx,local_min_rx);
        if (i>255 && src_ipv6[14]==0x00) {
           src_ipv6[14]+=1;
        }
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6);       
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }        

        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, local_min_tx, local_min_rx);   
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }    
        
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;    
    }
    
    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action \n");
        print rv;
        return rv;
    }
    return rv;
}

/*print sessions_create_unit2(0,120,13,1,1,0,0,5010,10,bcmBFDAuthTypeSimplePassword,3300,3300,50000,3,0,0);*/
int sessions_create_unit2(int unit, int session, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, uint32 local_disc, uint32 remote_disc, 
                            bcm_bfd_auth_type_t auth_type, uint32 local_min_tx, uint32 local_min_rx, uint32 udp_src_port, bcm_cos_t priority,
                            int event_on, int is_inject)
{
    int in_vlan = 2;
    int out_vlan = 200;
    int rv;

    bcm_mac_t next_hop_mac = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x02};
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x01, 0x02, 0x03};
    
    /* BFD event count */
    uint32 bfd_event_count[12] = {0};
    
    bcm_ip6_t src_ipv6 = {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01};
    bcm_ip6_t dst_ipv6= {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02};
    
    /* BFD in/out Port Configuration */    
    rv=bfd_ipv6_service_init(0,port,port);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_service_init\n");
        print rv;
        return rv;
    }

    /* BFD INIT */
    rv=bcm_bfd_init(0);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* BFD AuthType Configuration */
    if (auth_type == bcmBFDAuthTypeSimplePassword) {
        printf("Step: bfd_ipv6_auth_sp_set\n");
        rv = bfd_ipv6_auth_sp_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sp_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    } else if (auth_type == bcmBFDAuthTypeKeyedSHA1) {
        printf("Step: bfd_ipv6_auth_sha1_set\n");
        rv = bfd_ipv6_auth_sha1_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_auth_sha1_set (%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    if (event_on == 1) {
       /* BFD EVENT REGISTER */
        rv=bfd_ipv6_event_register(unit);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_event_register (%s) \n",bcm_errmsg(rv));
            return rv;
        } 
        printf ("\n\n ### BFD EVENT REGISTER is ON... ###\n");
    } else {
        printf ("\n\n ### BFD EVENT REGISTER is OFF... ###\n");
    }
    
    int endpoint_id=0;
    if (session > 10) {
        verbose=0;
    }
   
    /*  Create BFD Endpoint */
    for (i = 0; i < session; i++) { 
        printf("\n### Create: session %d at tx %d rx %d ###\n", i,local_min_tx,local_min_rx);
        if (i>255 && src_ipv6[14]==0x00) {
           src_ipv6[14]+=1;
        }
        
        /* ip entry create */
        rv=bfd_ipv6_intf_init(unit,in_vlan++,out_vlan++,mac_address,next_hop_mac,src_ipv6,dst_ipv6); 
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_intf_init \n");
            print rv;
            return rv;
        }
        
        /* bfd ipv6 endpoint create */
        rv=bfd_ipv6_ep_create(unit,i,bfd_fec, src_ipv6, dst_ipv6, endpoint_id++, is_single_hop, is_acc, is_passive, is_demand, udp_src_port++, auth_type, 0, local_disc + i, remote_disc + i, priority, local_min_tx, local_min_rx);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create \n");
            print rv;
            return rv;
        }
        
        src_ipv6[15]+=1;
        dst_ipv6[15]+=1;        
    }
    
    printf("\n\n ### bfd_ipv6_single_hop_field_action ### \n");
    rv=bfd_ipv6_single_hop_field_action(unit, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_single_hop_field_action\n");
        print rv;
        return rv;
    }
    
    return rv;
}


int bfd_ipv6_ep_id_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, uint32 auth_index, int is_inject ) {
    
    int rv;
    uint32 udp_src_port=50000;
    verbose=0;

    /* select random endpoint id to test */
    int endpoint_id_zero=0;
    int endpoint_id_rand=sal_rand()%16;
    
    /* 1. endpoint_id_zero */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_zero);
    rv=bfd_ipv6_ep_create_example(unit,port,1,is_single_hop,is_acc,is_passive,is_demand,endpoint_id_zero, udp_src_port, auth_type, auth_index, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }
    
    /* 1.1 create the existing entry */
    printf("\nTest: create an existing entry \n");
    rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand,endpoint_id_zero, udp_src_port, auth_type, auth_index, is_inject);
    if (rv != BCM_E_EXISTS) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }
    
    /* 1.2 destroy the existing entry */
    printf("\nTest: destroy an existing entry \n");
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_zero);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }
    /* 1.3 destroy the non-existing entry */
    printf("\nTest: destroy the non-existing entry \n");
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_zero);
    if (rv != BCM_E_NOT_FOUND) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_zero);
        print rv;
        return rv;
    }
        
    /* 2. Test endpoint id out-of-range */
    int endpoint_id_invalid=-1;
    int endpoint_id_out_of_range=2047;
    /* 2.1 */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_invalid);
    rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand,endpoint_id_invalid, udp_src_port, auth_type, auth_index, is_inject);
    if (rv != BCM_E_PARAM) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_invalid);
        print rv;
        return rv;
    }
    /* 2.2 */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_out_of_range);
    rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand, endpoint_id_out_of_range, udp_src_port, auth_type, auth_index, is_inject);
    if (rv != BCM_E_PARAM) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_out_of_range);
        print rv;
        return rv;
    }
    
    /* 3. endpoint_id_rand */
    printf("\n\n--------------- %d -------------- \n", endpoint_id_rand);
    rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand,endpoint_id_rand, udp_src_port, auth_type, auth_index, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id_rand);
        print rv;
        return rv;
    }
              
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id_rand);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id_rand);
        print rv;
        return rv;
    }  
    
    return rv;
}


int bfd_ipv6_ep_udpport_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, bcm_bfd_auth_type_t auth_type, uint32 auth_index, int is_inject ) {
    int rv;
    int endpoint_id=0; 

    verbose=0;

    /* select random endpoint id to test */
    int udp_src_port_low=49152;
    int udp_src_port_high=65535;
    
    /* 1. udp_src_port_low */
    printf("\n\n--------------- udp_src_port : %d -------------- \n", udp_src_port_low);
    rv=bfd_ipv6_ep_create_example(unit,port,1,is_single_hop,is_acc,is_passive,is_demand,endpoint_id, udp_src_port_low, auth_type, auth_index, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_low);
        print rv;
        return rv;
    }
    
    rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    }  
    
    /* 2. udp_src_port_high */
    printf("\n\n--------------- udp_src_port : %d -------------- \n", udp_src_port_high);
    rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand,endpoint_id, udp_src_port_high, auth_type, auth_index, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_high);
        print rv;
        return rv;
    }

    rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    } 
    return rv;
}


int bfd_ipv6_ep_auth_index_test(int unit, int port, int is_single_hop, int is_acc, int is_passive, int is_demand, int is_inject ) {
    int rv;
    int endpoint_id=0; 
    int udp_src_port=50000;
    int is_init=1;
    uint32 auth_index;
    verbose=0;
    
    /* 1. bcmBFDAuthTypeKeyedSHA1 */
    for ( auth_index=0; auth_index < auth_sha1_num; auth_index++) {
        printf("\n\n--------------- auth_index : %d -------------- \n", auth_index);
        rv=bfd_ipv6_ep_create_example(unit,port,is_init,is_single_hop,is_acc,is_passive,is_demand,endpoint_id, udp_src_port, bcmBFDAuthTypeKeyedSHA1, auth_index, is_inject);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_low);
            print rv;
            return rv;
        }
        
        is_init=0;
        
        rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
            print rv;
            return rv;
        }  
    }
    
    /* 2. bcmBFDAuthTypeSimplePassword */
    for (auth_index=0; auth_index < auth_sp_num; auth_index++) {
        printf("\n\n--------------- auth_index : %d -------------- \n", auth_index);
        rv=bfd_ipv6_ep_create_example(unit,port,0,is_single_hop,is_acc,is_passive,is_demand,endpoint_id, udp_src_port, bcmBFDAuthTypeSimplePassword, auth_index, is_inject);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_create_example for endpoint %d, udp_src_port %d \n", endpoint_id, udp_src_port_low);
            print rv;
            return rv;
        }

        rv=bfd_ipv6_ep_destroy(unit,endpoint_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bfd_ipv6_ep_destroy for endpoint id %d\n", endpoint_id);
            print rv;
            return rv;
        }  
    }
    return rv;
}

int bfd_ipv6_ep_update_test(int unit, int port, bcm_ip6_t src_ipv6, bcm_ip6_t dst_ipv6, int endpoint_id, int is_single_hop, int is_acc, int is_passive, int is_demand, 
                            int udp_src_port, bcm_bfd_auth_type_t auth_type, int auth_index, uint32 local_disc, uint32 remote_disc, bcm_cos_t priority, 
                            uint32 local_min_tx, uint32 local_min_rx, int is_inject ) {
    int rv;
    
    /* 1. endpoint create */
    printf("\n\n----------- Create Ednpoint %d -------------- \n", endpoint_id);
    uint32 udp_src_port_old=50000;
    int is_single_hop_old=0;
    int is_acc_old=0;
    int is_passive_old=0;
    int is_demand_old=0;
    bcm_bfd_auth_type_t auth_type_old=bcmBFDAuthTypeKeyedSHA1;
    uint32 auth_index_old=3;
    printf("is_single_hop = %d, is acc= %d, is_passive = %d, is_demand = %d, udp_src_port = %d\n", is_single_hop_old, is_acc_old, is_passive_old, is_demand_old, udp_src_port_old);
    printf("auth_type = bcmBFDAuthTypeKeyedSHA1 , auth_index = %d", auth_index_old);     
    verbose=1;
    rv=bfd_ipv6_ep_create_example(unit,port,1,is_single_hop_old,is_acc_old,is_passive_old,is_demand_old,endpoint_id, udp_src_port_old, auth_type_old, auth_index_old, is_inject);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_create_example for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    }
    
    /*2. endpoint update*/
    printf("\n\n----------- Update Ednpoint %d -------------- \n", endpoint_id);
    
    rv=bfd_ipv6_ep_update(unit, 0, bfd_fec, src_ipv6, dst_ipv6, endpoint_id, is_single_hop, is_acc, is_passive, is_demand, 
                        udp_src_port, auth_type, auth_index, local_disc, remote_disc, priority, local_min_tx, local_min_rx);
    if (rv != BCM_E_NONE) {
        printf("Error in bfd_ipv6_ep_update for endpoint id %d\n", endpoint_id);
        print rv;
        return rv;
    }
    
    return rv;
    
}    

