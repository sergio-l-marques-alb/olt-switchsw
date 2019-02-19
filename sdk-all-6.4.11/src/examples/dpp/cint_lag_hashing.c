/*
 * $Id: cint_lag_hashing.c,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * Lag hashing example script
 *
 */

/* 
the cint creates a Trunk (LAG), containing several ports, and points a MAC-table entry to this LAG. 
 
default LAG hashing example: 
----------------------------
run: 
cint utility/cint_utils_l3.c 
cint utility/cint_utils_vlan.c 
cint cint_ip_route.c
cint cint_lag_hashing.c 
cint 
print lag_hashing_main(unit, <in_port>, <out_port>, <nof_ports>); 
 
traffic example: 
run: 
    1) ethernet header with DA 00:0C:00:02:01:23 and vlan tag id 17
       and IPV4 header with DIP 10.0.255.0 and various SIPs (random)
    2) ethernet header with DA 00:0C:00:02:01:23 and vlan tag id 17
       MPLS header with label_1 44 and various label_2 (incremental)
    3) ethernet header with DA 00:0C:00:02:01:24, vlan tag id 17 and various SA (random)
 
lag_hashing_main() does not change the default LAG hashing configuration, which is to look at all the packet fields: 
for L2 packet - Ether type, Next header, MAC destination address, MAC source address. 
for IPV4 packet - DIP, SIP, protocol, dest L4 port and src L4 port.
for MPLS packet - fisrt label, second label and third label. 
 
however, by default the number of headers to use for LAG hashing is 0, so all traffic will go to the same LAG member (same port). 
 
run: 
config_per_port_lag_hashing(unit, <in_port>, 1, 1); 
 
now the number of headers for LAG hashing is 1 and it will start from the forwarding header. 
traffic will be divided equally between the LAG memebers (different ports). 
 
LAG hashing according to terminated header example:
--------------------------------------------------- 
run: 
    1) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       and IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    2) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       MPLS header with label_1 44 and label_2 55
    3) ethernet header with DA 0:C:0:2:01:24, vlan tag id 17 and various SA (random)
 
LAG hashing is set to look at headers starting from the forwarding header, so changes in the terminated header will not affect the hashing. 
all traffic will go to the same LAG member (same port). 
 
run: 
config_per_port_lag_hashing(unit, <in_port>, 3, 0); 
 
run: 
    1) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       and IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    2) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       MPLS header with label_1 44 and label_2 55 
 
now LAG hashing is set to look at the terminated header (as well as the forward header). 
traffic will be divided equally between the LAG memebers (different ports). 
 
run: 
disable_lag_hashing(unit); 
 
This will disable LAG hashing. No part of the packet will be used for hashing, so all traffic will go to the same LAG member (same port). 
 
run: 
lag_hash_l2_src(unit); 
 
Now LAG hashing is set to look at only the L2 SRC MAC address (SA), so traffic will be divided again equally between the LAG memebers (different ports). 
 
LAG hashing according to L4 example:
------------------------------------
run: 
lag_hash_l4_dest(unit); 
 
run: 
    ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and SA 0
    IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    and TCP header with vairous dest ports (random)
 
now LAG hashing is set to look at 3 headers (terminated - Eth, forward - IPV4 and L4 - TCP). it is also set to look at the L4 dest port.
traffic will be divided equally between the LAG memebers (different ports). 
 
LAG hashing according to SRC port:
----------------------------------
run: 
lag_src_port_enable(unit, 1); 
 
run: 
    ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and SA 0
    IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    from 2 different incoming (src) ports.
 
now LAG hashing is set to look at the source port. traffic from one in-port will be sent to one of the LAG memebers (one port) 
and traffic from the other in-port, will be sent to a different one of the LAG members (different port). 
 
Non polynomial LAG hashing:
---------------------------
 
run: 
disable_lag_hashing(unit); 
lag_hash_func_config(unit, BCM_HASH_CONFIG_ROUND_ROBIN); 
 
Now instead of a polynomial hashing, a counter that is incremented on every packet is used.
Traffic (random or not) will be divided equally between the LAG memebers, although LAG hashing is disabled.  
*/

struct cint_lag_hashing_cfg_s {
  int is_stateful; /* If 1 then lag_hashing_main does stateful load balancing. */
};

cint_lag_hashing_cfg_s cint_lag_hashing_cfg = {
  0, /* is_stateful */
};

/* 
config_per_port_lag_hashing(): 
set per-port LAG hashing properties: 
1) nof_headers - the number of headers to use for LAG hashing. range: 0-3 
2) forward - first header for LAG hashing. if forward==1 first header will be the forwarding header, 
             else first header will be the header below the forwarding header (the terminated header) 
*/ 
int config_per_port_lag_hashing(int unit, int in_port, int nof_headers, int forward) {
  int rc;
  int arg;
  bcm_switch_control_t type;

  /* select which starting header to use for LAG hashing */
  type = bcmSwitchTrunkHashPktHeaderSelect;
  if (forward) { /* first header for LAG hashing will be the forwarding header */
      arg = BCM_HASH_HEADER_FORWARD; 
  }
  else { /* first header will be the header below the forwarding header (the terminated header) */
      arg = BCM_HASH_HEADER_TERMINATED;
  }

  rc = bcm_switch_control_port_set(unit, in_port, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);
  }

  /* select number of headers to consider in LAG hashing */
  type = bcmSwitchTrunkHashPktHeaderCount;
  arg = nof_headers;

  rc = bcm_switch_control_port_set(unit, in_port, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
disable_lag_hashing(): 
set LAG hashing to "look at nothing". 
no part of the header will be used in hashing. 
in this case, hashing result will be the same for every packet that arrives.
*/ 
int disable_lag_hashing(int unit) {
  int rc;
  int arg = 0; /* arg = 0 so no field in the Eth header will be looked at */
  bcm_switch_control_t type;

  /* disable L2 hashing */
  type = bcmSwitchHashL2Field0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 failed: %d \n", rc);
  }

  /* disable IPV4 hashing */
  type = bcmSwitchHashIP4Field0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 failed: %d \n", rc);
  }

  /* disable MPLS hashing */
  type = bcmSwitchHashMPLSField0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
  }

  type = bcmSwitchHashMPLSField1;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField1 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_l4_dest(): 
set LAG hashing to done according to destination L4 port. 
*/ 
int lag_hash_l4_dest(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_DSTL4;
  bcm_switch_control_t type = bcmSwitchHashIP4Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_hash_l2_src(): 
set LAG hashing to done according to L2 SA. 
*/ 
int lag_hash_l2_src(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI;
  bcm_switch_control_t type = bcmSwitchHashL2Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_hash_l2_dst(): 
set LAG hashing to done according to L2 DA. 
*/ 
int lag_hash_l2_dst(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI;
  bcm_switch_control_t type = bcmSwitchHashL2Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_src_port_enable(): 
Make the Source port a part of the LAG hash.
arg = 1- enable, 0- disable. 
*/ 
int lag_src_port_enable(int unit, int arg) {
  int rc;
  bcm_switch_control_t type = bcmSwitchTrunkHashSrcPortEnable;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchTrunkHashSrcPortEnable failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_func_config(): 
set LAG hashing function (polynomial). 
bcm_hash_config = BCM_HASH_CONFIG_*
*/ 
int lag_hash_func_config(int unit, int bcm_hash_config) {
  int rc;
  bcm_switch_control_t type = bcmSwitchTrunkHashConfig;

  rc = bcm_switch_control_set(unit, type, bcm_hash_config);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchTrunkHashConfig failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_label5(): 
set LAG hashing to be done according to MPLS label5. 
in this case, hashing result will be the same for every packet with the same label5.
*/ 
int lag_hash_label5(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_2ND_LABEL;
  bcm_switch_control_t type = bcmSwitchHashMPLSField1;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField1 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_label2(): 
set LAG hashing to be done according to MPLS label2. 
in this case, hashing result will be the same for every packet with the same label2.
*/ 
int lag_hash_label2(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_2ND_LABEL;
  bcm_switch_control_t type = bcmSwitchHashMPLSField0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hashing_main():
1) create LAG and add ports to it
2) Add IPV4 host entry and point to LAG.
3) Add switch entry to swap MPLS labels and also send to LAG
 
nof_ports = the number of ports that will be added to the LAG (members)
out_port  = the out ports numbers will be: <out_port>, <out_port> + 1, ... , <out_port> + <nof_ports> -1 
*/ 
int lag_hashing_main(int unit, int in_port, int out_port, int nof_ports) {

  int CINT_NO_FLAGS = 0;
  int rc, i;
  int vrf = 5;
  int in_vlan = 17;
  int out_vlan = 18;
  int ingress_intfs[2]; /* in-RIF and out-RIF */
  bcm_if_t egress_intf; /* FEC */
  int encap_id;

  bcm_trunk_t trunk_id; /* trunk */
  bcm_trunk_member_t member;
  int trunk_gport;

  bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};  /* my-MAC */
  bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

  int host = 0x0a00ff00; /* 10.0.255.0 */
  bcm_l3_host_t l3host;

  int in_label = 44;
  int eg_label = 66;
  bcm_mpls_tunnel_switch_t mpls_tunnel_info;

  bcm_mac_t dest_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x24};  /* dest-MAC */
  bcm_l2_addr_t l2addr;

  /* create in-RIF */
  rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
  if (rc != BCM_E_NONE) {
  	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
  }
  rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
  	printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
    return rc;
  }

  create_l3_intf_s intf;
  intf.vsi = in_vlan;
  intf.my_global_mac = mac_address;
  intf.my_lsb_mac = mac_address;
  intf.vrf_valid = 1;
  intf.vrf = vrf;

  rc = l3__intf_rif__create(unit, &intf);
  ingress_intfs[0] = intf.rif;        
  if (rc != BCM_E_NONE) {
  	printf("Error, l3__intf_rif__create\n");
  }

  /* create out-RIF */
  rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
  if (rc != BCM_E_NONE) {
  	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
  }
  rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
  if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
  	printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
    return rc;
  }

  intf.vsi = out_vlan;

  rc = l3__intf_rif__create(unit, &intf);
  ingress_intfs[1] = intf.rif;        
  if (rc != BCM_E_NONE) {
  	printf("Error, l3__intf_rif__create\n");
  }

  /* create trunk */
  rc = bcm_trunk_create(unit, CINT_NO_FLAGS, &trunk_id);
  if (rc != BCM_E_NONE) {
    printf ("bcm_trunk_create failed: %d \n", rc);
    return rc;
  }

  if (cint_lag_hashing_cfg.is_stateful) {
    rc = bcm_trunk_psc_set(unit, trunk_id, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);
    if (rc != BCM_E_NONE) {
      printf ("The following call failed: \n", rc);
      printf ("bcm_trunk_psc_set(%d, %d, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);\n", unit, trunk_id);
      print rc;
      return rc;
    }
  }

  /* add ports to trunk */
  bcm_trunk_member_t_init(&member);
  out_port--;
  for (i = 0; i < nof_ports; i++) {

      out_port++;
      BCM_GPORT_LOCAL_SET(member.gport, out_port); /* phy port to local port */
      
      rc = bcm_trunk_member_add(unit, trunk_id, &member);
      if (rc != BCM_E_NONE) {
        printf ("bcm_trunk_member_add with port %d failed: %d \n", out_port, rc);
        return rc;
      }
  }

  /* create FEC and send to LAG (instead of out-port) */
  BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id); /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */
  rc = create_l3_egress(unit, CINT_NO_FLAGS, trunk_gport, out_vlan, ingress_intfs[1], next_mac_address, &egress_intf, &encap_id);      
  if (rc != BCM_E_NONE) {
    printf ("create_l3_egress no. %d failed: %d \n", i, rc);
    return rc;
  }

  /* add host entry and point to the FEC pointing at LAG */
  bcm_l3_host_t_init(&l3host);
  l3host.l3a_ip_addr = host;
  l3host.l3a_vrf = vrf;
  l3host.l3a_intf = egress_intf; /* FEC */

  rc = bcm_l3_host_add(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %x \n", rc);
    return rc;
  }

  /* add switch entry to swap labels and map to LAG (FEC pointing at LAG) */
  bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
  mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
  mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
  mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
  mpls_tunnel_info.label = in_label; /* incomming label */
  mpls_tunnel_info.egress_label.label = eg_label; /* outgoing (egress) label */
  mpls_tunnel_info.egress_if = egress_intf; /* FEC */

  rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
  if (rc != BCM_E_NONE) {
    printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
    return rc;
  }

  /* add Mac table entry and point to the LAG, for bridging */
  sal_memcpy(l2addr.mac, dest_mac_address, 6);
  l2addr.vid = in_vlan;
  l2addr.port = trunk_gport; /* dest is LAG */

  bcm_mac_t bc_mac_address  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};  /* dest-MAC */
  rc = bcm_l2_addr_add(unit, &l2addr);
  if (rc != BCM_E_NONE) {
      printf("bcm_l2_addr_add failed: %x \n", rc);
      return rc;
  }

  sal_memcpy(l2addr.mac, bc_mac_address, 6);
  rc = bcm_l2_addr_add(unit, &l2addr);
  if (rc != BCM_E_NONE) {
      printf("bcm_l2_addr_add 2 failed: %x \n", rc);
      return rc;
  }

  return 0;
}

/*
 * This functions is used for cleanup after running the cint. It destroys all trunks on the device.
 */

int
lag_hashing_trunk_destroy(int unit){
    int rv;

    rv = bcm_trunk_detach(unit);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_trunk_detach\n");
        return rv;
    }

    rv = bcm_trunk_init(unit);

    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trunk_init\n");
        return rv;
    }

    return rv;
}
