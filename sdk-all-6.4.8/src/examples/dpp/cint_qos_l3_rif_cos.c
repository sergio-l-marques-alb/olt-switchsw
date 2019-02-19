/*
 * $Id: cint_qos_l3_rif_cos.c,v 1.8 Broadcom SDK $
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
 */
 
/* Examples for:
 * 1) In-RIF Remark (LIF COS profile) and Egress Forwarding layer (Remark profile) configuration. 
 *      in ingress: - DSCP mapping to in-DSCP
 *                  - in-DSCP mapping to TC,DP 
 *      in egress:  - in-DSCP,DP mapping to out-DSCP  
 *  
 * run: 
 * cint examples/dpp/utility/cint_utils_l3.c 
 * cint examples/dpp/cint_ip_route.c 
 * cint examples/dpp/cint_qos_l3_rif_cos.c
 * cint
 * lif_cos_and_remark_example(0, <in_port>, <out_port>); 
 */
 
  
/********** 
  Globals/Aux Variables
 ********** */

/* debug prints */
verbose = 1;


/********** 
  functions
 ********** */

int ing_qos_id, eg_qos_id, pcp_vlan_profile;
int qos_map_l3_ingress_dscp_profile(int unit)
{   
    bcm_qos_map_t l3_ing_map;
    int dscp;
    int rv;
    
    /* create QoS ID (new cos profile) */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &ing_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_INGRESS\n");
        return rv;
    }
    printf("created QoS-id =0x%08x, \n", ing_qos_id);
    
    /* set QoS mapping for L3 in ingress:
       map In-DSCP-EXP = IP-Header.TOS - 1 */
    for (dscp=0; dscp<256; dscp++) {
        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_ing_map.int_pri = dscp % 8; /* TC */
        l3_ing_map.color = dscp % 2; /* DP */
        l3_ing_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, ing_qos_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    printf("set ingress QoS mapping for L3\n");
    
    return rv;
}
    
int qos_map_l3_egress_dscp_profile(int unit) 
{
    bcm_qos_map_t l3_eg_map;
    int dscp, dp;
    int rv;
    
    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", eg_qos_id);

    /* set QoS mapping for L3 in egress:
       map Out-DSCP-EXP (TOS) = In-DSCP-EXP + DP */
    for (dscp = 0; dscp < 256; dscp++) {
        for (dp = 0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.color = dp; /* Set internal color (DP) */
            l3_eg_map.int_pri = dscp; /* in-DSCP-EXP */
            l3_eg_map.remark_int_pri = (dscp + dp) & 0xff; /* TOS */

            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3, l3_eg_map, eg_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, set egress QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }
        }
    }
    printf("Set egress QoS mapping for L3\n");
    
    return rv;
} 

int qos_map_l3_egress_dscp_to_pri_cfi_profile(int unit)    
{
    bcm_qos_map_t l3_pcp_map;
    int dscp;
    int rv;
    
    /* Create QOS profile (PCP-VLAN) ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_vlan_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flags BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP\n");
        return rv;
    }
    printf("created PCP-profile-id =0x%08x, \n", pcp_vlan_profile);

    /* set QoS mapping for L2: map DSCP => PCP, DEIin */
    for (dscp = 0; dscp < 64; dscp++) {
        bcm_qos_map_t_init(&l3_pcp_map);
        l3_pcp_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_pcp_map.pkt_pri = 2; /* PCP */
        l3_pcp_map.pkt_cfi = 1; /* DEI */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_IPV4, l3_pcp_map, pcp_vlan_profile);
        if (rv != BCM_E_NONE) {
            printf("Error, set L3 PCP QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    
    return rv;
}

int qos_map_l2_egress_tcdp_to_pri_cfi_profile(int unit)
{
    bcm_qos_map_t l2_pcp_map;
    int rv;
    int tc,dp;
    int flags;

    /* Create QOS profile (PCP-VLAN) ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_vlan_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flags BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP $rv \n");
        return rv;
    }
    printf("created PCP-profile-id =0x%08x, \n", pcp_vlan_profile);

    for (tc=0; tc<8; tc++) {
        for (dp=0; dp<2; dp++) {
            flags = BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
            if (tc != 7) { 
                /* Set TC -> PCP  */
                l2_pcp_map.int_pri = tc; /* TC Input */
                l2_pcp_map.color = dp; /* Color Input */
                l2_pcp_map.pkt_pri = tc + 1; /* PCP Output */
                l2_pcp_map.pkt_cfi = dp; /* CFI Output */
            } else { 
                /* Set TC -> PCP  */
                l2_pcp_map.int_pri = tc; /* TC Input */
                l2_pcp_map.color = dp; /* Color Input */
                l2_pcp_map.pkt_pri = tc; /* PCP Output */
                l2_pcp_map.pkt_cfi = dp; /* CFI Output */
            }

            rv = bcm_qos_map_add(unit, flags, &l2_pcp_map, pcp_vlan_profile);    
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_qos_map_add\n");
                return rv;
            }
        }
    }  
}

int eve_action(int unit, int out_port, bcm_vlan_t out_vlan)
{
    int rv;
    bcm_vlan_action_set_t action;

    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    /* Associate Out-AC to QOS profile ID, by egress vlan editing */
    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = bcmVlanActionAdd; /* Replace VLAN tag */
    action.new_outer_vlan = out_vlan;
    action.ut_outer_pkt_prio = bcmVlanActionAdd;
    action.priority = pcp_vlan_profile;
   
    rv = bcm_vlan_translate_egress_action_add(unit, out_port, out_vlan, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_egress_action_add\n");
        return rv;
    }

    return rv;
}

int qos_delete_all_maps(int unit, int map_id) 
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_in_map;
    int dscp = 0;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    for (dscp = 0; dscp < 64; dscp++) {
        l3_in_map.dscp = dscp;
        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_IPV4, l3_in_map, map_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_qos_map_delete $rv \n");
            return rv;
        }
    }
    return rv;
}

int qos_map_id_destroy(int unit, int map_id) 
{
    int rv;
    rv = bcm_qos_map_destroy(unit, map_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_qos_map_destroy $rv \n");
        return rv;
    }
    return rv;
}


/******* Run examples ******/
 
/* 
 * Example of: 
 * In-RIF Remark (LIF COS profile) and Egress Forwarding layer (Remark profile). 
 *  
 * packet will be routed from in_port to out-port 
 * in ingress: packet DSCP will be mapped to in-DSCP, which will be mapped to TC,DP 
 * in egress: in-DSCP,DP will be mapped to out-DSCP 
 *  
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.3)
 *  - TOS between 0-31
 *  
 * expected: 
 *  - out port = out_port
 *  - vlan 2.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  IPV4 TOS according to mapping
 */

int lif_cos_and_remark_example(int unit, int in_port, int out_port){
    int rv;
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    int fec;
    int in_vlan = 15, out_vlan = 2; /* in and out vlan */
    int vrf = 0;
    int host;
    int encap_id;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    rv = qos_map_l3_ingress_dscp_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("error in qos_map_l3_ingress_dscp_profile $rv");
        return rv;
    }
    
    /* create ingress router interface: Associate In-RIF to QOS profile ID */
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
        return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;

    intf.qos_map_valid = 1;
    intf.qos_map_id = ing_qos_id;

    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    rv = l3__intf_rif__create(unit, &intf);
    l3_intf_id = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    rv = qos_map_l3_egress_dscp_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("error in qos_map_l3_egress_dscp_profile $rv");
        return rv;
    }
    
    /* create egress router interface: Associate Out-RIF to QOS profile ID */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
      printf("fail open vlan(%d)\n", out_vlan);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
      return rv;
    }

    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    /* create egress object (FEC) with the created Out-RIF */
    rv = create_l3_egress(unit, 0, out_port, out_vlan, ing_intf_out, next_hop_mac, &fec, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rv;
    }
    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add host and point to FEC */
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
        return rv;
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    return rv;
}

 
/* 
 * Example of: 
 * EVE PCP-DEI. 
 *  
 * packet will be routed from in_port to out-port 
 * 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.3)
 *  
 * expected: 
 *  - out port = out_port
 *  - vlan 2.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  IPV4 TOS according to mapping
 */
int pcp_dei_example(int unit, int in_port, int out_port){
    int rv;
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    int pcp_vlan_profile;
    bcm_qos_map_t l3_pcp_map;
    int dscp;
    int fec;
    int in_vlan = 15, out_vlan = 2;
    bcm_vlan_action_set_t action;
    int vrf = 0;
    int host;
    int encap_id;    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* create ingress router interface */
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
        return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    
    intf.qos_map_valid = 1;
    intf.qos_map_id = ing_qos_id;
    
    rv = l3__intf_rif__create(unit, &intf);
    l3_intf_id = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);
    
    /* create egress router interface */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
      printf("fail open vlan(%d)\n", out_vlan);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    
    intf.qos_map_valid = 1;
    intf.qos_map_id = 0;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);

    /* create egress object with the created Out-RIF */
    rv = create_l3_egress(unit, 0, out_port, out_vlan, ing_intf_out, next_hop_mac, &fec, &encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rv;
    }
    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add host point to FEC */
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
        return rv;
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    rv = qos_map_l3_egress_dscp_to_pri_cfi_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_map_l3_egress_dscp_to_pri_cfi_profile\n");
        return rv;
    }
    
    rv = eve_action(unit, out_port, out_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, eve_action\n");
        return rv;
    }

    return rv;
}

