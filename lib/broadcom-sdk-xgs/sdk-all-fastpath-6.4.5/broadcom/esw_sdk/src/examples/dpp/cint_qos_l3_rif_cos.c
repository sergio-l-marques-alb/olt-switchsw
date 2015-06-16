/*
 * $Id: cint_qos_l3_rif_cos.c,v 1.8 Broadcom SDK $
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

/*
 * create l3 interface - ingress/egress 
 *  Creates Router interface 
 *  - packets sent in from this interface identified by <port, vlan> with specificed MAC address is subject of routing
 *  - packets sent out through this interface will be encapsulated with <vlan, mac_addr>
 *  Parmeters:
 *  - flags: special controls set to zero.
 *  - open_vlan - if TRUE create given vlan, FALSE: vlan already opened juts use it
 *  - port - where interface is defined
 *  - vlan - router interface vlan
 *  - mac_addr - my MAC
 *  - qos_profile_id - the in-rif qos-profile-id
 *  - *intf - returned handle of opened l3-interface
 */
int create_l3_intf_qos(int unit, uint32 flags, uint8 open_vlan, uint8 out, bcm_gport_t port, int vlan, bcm_mac_t mac_addr, int qos_profile_id, int *intf) {

  int rc, station_id;
  bcm_l3_intf_t l3if, l3if_ori;
  bcm_l3_ingress_t l3_ing_if;
  bcm_l3_intf_qos_t intf_qos;
  bcm_l2_station_t station; 

  bcm_l3_intf_t_init(&l3if);
  bcm_l3_ingress_t_init(&l3_ing_if);
  bcm_l2_station_t_init(&station); 

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

  if (open_vlan) {
      rc = bcm_vlan_create(unit, vlan);
      if (rc != BCM_E_NONE) {
        printf("fail open vlan(%d)\n", vlan);
        printf("continue..\n");
      }
  }
  rc = bcm_vlan_gport_add(unit, vlan, port, 0);
  if (rc != BCM_E_NONE) {
      printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
    return rc;
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
  if (out) {
      l3if.dscp_qos.qos_map_id = qos_profile_id;
  }

  rc = bcm_l3_intf_create(unit, &l3if);
  if (rc != BCM_E_NONE) {
    printf("Error in bcm_l3_intf_create at create_l3_intf_qos\n");
    return rc;
  }

  /* Set uRPF global configuration */
  print bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, bcmL3IngressUrpfLoose);

  l3_ing_if.urpf_mode = bcmL3IngressUrpfLoose; /* RPF mode is loose */
  l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
  l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE; /* when lookup in VRF fail lookup in global routing table */
  l3_ing_if.flags |= BCM_L3_INGRESS_DSCP_TRUST;
  if (!out) {
      l3_ing_if.qos_map_id = qos_profile_id;
  }

  rc = bcm_l3_ingress_create(unit, &l3_ing_if, &l3if.l3a_intf_id);
  
  *intf = l3if.l3a_intf_id;
  return rc;
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
    int ing_qos_id, eg_qos_id, pcp_vlan_profile;
    int dscp, dp;
    bcm_qos_map_t l3_ing_map, l3_eg_map, l2_map;
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    int fec;      
    int flags = 0;
    int in_vlan = 15, out_vlan = 2; /* in and out vlan */
    int vrf = 0;
    int host;
    int encap_id; 
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* create QoS ID (new cos profile) */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &ing_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_INGRESS\n");
        return rv;
    }
    printf("created QoS-id =0x%08x, \n", ing_qos_id);

    /* set QoS mapping for L3 in ingress:
       map In-DSCP-EXP = IP-Header.TOS - 1 */
    for (dscp=0; dscp<32; dscp++) {
        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_ing_map.int_pri = dscp % 8; /* TC */
        l3_ing_map.color = dscp % 3; /* DP */
        l3_ing_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, ing_qos_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    printf("set ingress QoS mapping for L3\n");

    /* create ingress router interface: Associate In-RIF to QOS profile ID */
    rv = create_l3_intf_qos(unit, flags, FALSE, FALSE, in_port, in_vlan, mac_address, ing_qos_id, &l3_intf_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
        return rv;
    }
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);

    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", eg_qos_id);

    /* set QoS mapping for L3 in egress:
       map Out-DSCP-EXP (TOS) = In-DSCP-EXP + DP */
    for (dscp = 0; dscp < 16; dscp++) {
        for (dp = 0; dp < 3; dp++) {
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

    /* create egress router interface: Associate Out-RIF to QOS profile ID */
    rv = create_l3_intf_qos(unit, flags, TRUE, TRUE, in_port, out_vlan, mac_address, eg_qos_id, &ing_intf_out); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
    }

    /* create egress object (FEC) with the created Out-RIF */
    rv = create_l3_egress(unit, flags, out_port, out_vlan, ing_intf_out, next_hop_mac, &fec, &encap_id); 
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
    int flags = 0;
    int in_vlan = 15, out_vlan = 2;
    bcm_vlan_action_set_t action;
    int vrf = 0;
    int host;
    int encap_id;    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    /* create ingress router interface */
    rv = create_l3_intf_qos(unit, flags, FALSE, FALSE, in_port, in_vlan, mac_address, 0, &l3_intf_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
        return rv;
    }
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);

    /* create egress router interface */
    rv = create_l3_intf_qos(unit, flags, TRUE, FALSE, in_port, out_vlan, mac_address, 0, &ing_intf_out); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, in_port=%d, \n", in_port);
        return rv;
    }
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);

    /* create egress object with the created Out-RIF */
    rv = create_l3_egress(unit, flags, out_port, out_vlan, ing_intf_out, next_hop_mac, &fec, &encap_id); 
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

    /* Create QOS profile (PCP-VLAN) ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_vlan_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flags BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP\n");
        return rv;
    }
    printf("created PCP-profile-id =0x%08x, \n", pcp_vlan_profile);

    /* set QoS mapping for L2: map DSCP => PCP, DEIin */
    for (dscp = 0; dscp < 8; dscp++) {
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

    /* Associate Out-AC to QOS profile ID, by egress vlan editing */
    bcm_vlan_action_set_t_init(&action);
    action.ot_outer = bcmVlanActionReplace; /* Replace VLAN tag */
    action.new_outer_vlan = out_vlan;
    action.ot_outer_pkt_prio = bcmVlanActionAdd; /* priority is set according to pcp_vlan_profile mapping */
    action.priority = pcp_vlan_profile;  

    rv = bcm_vlan_translate_egress_action_add(unit, out_port, in_vlan, out_vlan, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_egress_action_add\n");
        return rv;
    }

    return rv;
}

