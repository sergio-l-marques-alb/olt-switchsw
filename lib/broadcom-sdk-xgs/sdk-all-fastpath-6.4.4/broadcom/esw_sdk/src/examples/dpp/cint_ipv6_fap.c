/*
 * $Id: cint_ipv6_fap.c,v 1.16 Broadcom SDK $
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
 * IPv6 Example script
 *
 */

/* 
the cint creates four UC IPV6 entries and one MC IPV6 entry. 
 
run: 
cint utility/cint_utils_mpls.c  
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(0); 
print bcm_l3_ip6(0, <second_unit>, <in_port>, <out_port>);
 
for one device: call bcm_l3_ip6() with second_unit<0
 
for two devices: make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1

traffic example for two devices: 
 
    run ethernet header with DA 0:0:0:0:55:34 (for UC) or 33:33:3:2:1:0 (for compatible-MC) and vlan tag id 17
    and IPV6 header with DIP according to the following:
    
    device 0:
    ---------
      <in_port>:    route  01001600350070000000db0700000000 : 100:1600:3500:7000:0:db07:0:0  
                    mask   0000000000000000ffffffffffffffff : 0:0:0:0:ffff:ffff:ffff:ffff
 
                    route  01001600350064000000db0700000000 : 100:1600:3500:6400:0:db07:0:0 
                    mask   0000000000000000fcffffffffffffff : 0:0:0:0:fcff:ffff:ffff:ffff
                    
                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC)   
                    
    device 1:
    ---------
      <in_port>:    route  01001600550078000000db0700000000 : 100:1600:5500:7800:0:db07:0:0 
                    mask   0000000000000000f0ffffffffffffff : 0:0:0:0:f0ff:ffff:ffff:ffff
                    
                    route  01001600780088000000db0700000000 : 100:1600:7800:8800:0:db07:0:0 
                    mask   0000000000000000c0ffffffffffffff : 0:0:0:0:c0ff:ffff:ffff:ffff
                    
                    route  ff1e0d0c0b0a09080706050403020100 : ff1e:d0c:b0a:908:706:504:302:100 (comaptible-MC) 
                  
UC packets from device 0 will be forwrded to <out_port> on device 1.
UC packets from device 1 will be forwrded to <out_port> on device 0.
MC packet from both devices will be forwrded to device 0 and to device 1 on ports <out_port>,<out_port+1>.
 
UC packets will arrive at out_port with: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:0:0:0:55:34 and vlan tag id 18 
MC packets will arrive at out_port with: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 33:33:3:2:1:0 and vlan tag id 18 
    IPV6 header with same DIP and hop_limit decremented

traffic example for single device:

run same packets from <in_port>. All 4 types of UC packets will arrive at <out_port>.
MC packets will arrive at ports <out_port>,<out_port+1> and will not arrive at <in_port>, because of same-interface filter. 
 
to test IPV6 default route: 
--------------------------- 
call: 
ipv6_default_dest_set(0, <mc>, 1, 14); 
 
and run unknown UC/MC traffic (depends on whether <mc> is set or not). 
this will set the default destination to be a trapm pointing at <out_port>, so the unknown traffic will go there. 
 
now call: 
ipv6_default_dest_set(0, <mc>, 0, 14); 
 
nad run again the same MC/UC IPV6 traffic. 
this will set the default route to be VRF and all traffic will be dropped. 
 
-------------------------------------------------------------------------------- 
 
The cint can be also used to test RIF TTL thershold:
In this example, same out_rif (=18) is created, but this time its TTL threshold is set as well. 
If an IPV6 MC packet arrives with a hop-limit that is lower than the threshold, the packet will be dropped at egress.  
calling sequence: 
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(0); 
print bcm_l3_ip6_with_ttl(0, -1, <in_port>, <out_port>, <ttl_threshold>); 
 
 
The cint can be also used to test mtu by calling modify_l3_interface_ingress(int unit, int mtu, int out_vlan)
after bcm_l3_ip6. In this example out_vlan is set to 18, so to disable mtu use the following sequence:
cint cint_qos_l3.c 
cint cint_ipmc_example.c
cint cint_ipv6_fap.c 
cint 
print qos_map_l3(0); 
print bcm_l3_ip6(0,-1,13,14);
print modify_l3_interface_ingress(0, 0, 18);
where out-vlan 18 is the outgoing interface 
 
 
 
  Note: this cint also includes tests for 4 label push and protection 

*/

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34}; /* incoming DA (myMAc) */

/* Globals */
int _g_bcmRxTrapDefaultMcv6_trap_id=-1;
int _g_bcmRxTrapDefaultUcv6_trap_id=-1;
int default_vrf=0;
int default_intf=0;

/* enable to set / unset UC routes
   Used in case required to run ipv6 MC tests only
   for MC routsb only use:
         bcm_l3_ip6_mc(int unit, int second_unit ,int in_port, int out_port)
   for MC & UC routes use:
         bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port)
*/
int add_ipv6_uc_routes=1;

/*
 * modify l3 interface ingress for mtu check
 *  Parmeters:
 *  - mtu - maximal transmission unit
 *  - out_vlan - out vlan id
 */
int modify_l3_interface_ingress(int unit, int mtu, int out_vlan) {

  int CINT_NO_FLAGS = 0;
  int rc;
  bcm_l3_intf_t l3if, l3if_ori;

  bcm_l3_intf_t_init(&l3if);

  /* before creating L3 INTF, check whether L3 INTF already exists*/
  bcm_l3_intf_t_init(&l3if_ori);
  l3if_ori.l3a_intf_id = out_vlan;
  rc = bcm_l3_intf_get(unit, &l3if_ori);
  if (rc == BCM_E_NONE) {
      /* if L3 INTF already exists, replace it*/
      l3if.l3a_flags = CINT_NO_FLAGS | BCM_L3_REPLACE | BCM_L3_WITH_ID;
      l3if.l3a_intf_id = out_vlan;
  }
  else {
      l3if.l3a_flags = CINT_NO_FLAGS; 
  }

  sal_memcpy(l3if.l3a_mac_addr, mac_l3_ingress, 6);
  l3if.l3a_vid = out_vlan;
  l3if.l3a_ttl = 31;
  l3if.l3a_mtu = mtu;

  rc = bcm_l3_intf_create(unit, l3if);
  return rc;
}

/*
 * create l3 interface - ingress/egress 
 *  Creates Router interface 
 *  - packets sent in from this interface identified by <port, vlan> with MAC address (= mac_l3_ingress) is subject of routing
 *  - packets sent out through this interface will be encapsulated with <vlan, mac_addr>
 *  Parmeters:
 *  - flags: special controls set to zero
 *  - port - where interface is defined
 *  - vlan - router interface vlan
 *  - vrf - VRF to map to
 *  - ttl - TTL/hop-limit threshold for IPV4/6 MC
 *  - *intf - returned handle of opened l3-interface
 */
int create_l3_intf(int unit, uint32 flags, bcm_gport_t port, int vlan, int vrf, bcm_mac_t mac_addr, int ttl, int *intf) {

  int rc, station_id;
  bcm_l3_intf_t l3if, l3if_ori;
  bcm_l3_ingress_t l3_ing_if;
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
  
  rc = bcm_vlan_create(unit, vlan);
  if (rc == BCM_E_EXISTS) {
      printf("vlan (%d) already exists", vlan);
  }
  else if (rc != BCM_E_NONE) {
      printf("fail open vlan (%d)", vlan);
      printf("    continue..\n");
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
  l3if.l3a_ttl = ttl;
  l3if.l3a_mtu = 1524;

  rc = bcm_l3_intf_create(unit, l3if);
  if (rc != BCM_E_NONE) {
    return rc;
  }
  *intf = l3if.l3a_intf_id;

  l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF */
  l3_ing_if.vrf = vrf;

  /* associate ingress map in cint_qos_l3.c */
  l3_ing_if.qos_map_id = qos_map_id_l3_ingress_get(unit);
  printf("cos_profile(0x%08x) rif(%d)\n", l3_ing_if.qos_map_id, l3if.l3a_intf_id);

  rc = bcm_l3_ingress_create(unit, l3_ing_if, l3if.l3a_intf_id);
  if (rc != BCM_E_NONE) {
    return rc;
  }

  return rc;
}

/*
 * create l3 egress object (FEC)
 * object includes next hop information.
 * - packets sent to this interface: will be send through out_port over given l3_eg_intf with next next_hop_mac_addr
 *   VLAN and SA is driven from l3_eg_intf as configure in create_l3_intf().
 * Parameters:
 *  - flags: special controls set to zero.
 *  - out-port: egress port.
 *  - vlan : not in use ignore.
 *  - l3_eg_intf: egress router interface will derive (Vlan, SA)
 *  - *intf: returned value FEC-id
 *  - *encap_id: returned value outlif point to mac_l3_egress.
 */
int create_l3_egress(int unit, uint32 flags, int out_port, int vlan, int l3_eg_intf, bcm_mac_t mac_addr, int *intf, int *encap_id) {

  int rc;
  bcm_l3_egress_t l3eg;
  bcm_if_t l3egid;
  int mod = 0;
  int test_failover_id = 0;
  int test_failover_intf_id = 0;

  bcm_l3_egress_t_init(&l3eg);

  l3eg.intf = l3_eg_intf;
  sal_memcpy(l3eg.mac_addr, mac_addr, 6);  
  l3eg.vlan   = vlan;
  l3eg.module = mod;
  l3eg.port   = out_port;
  l3eg.failover_id = test_failover_id;
  l3eg.failover_if_id = test_failover_intf_id;
  l3eg.encap_id = *encap_id;
  l3egid = *intf; 
  l3eg.flags = flags;

  rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
  if (rc != BCM_E_NONE) {
      printf("Error, create egress object, out_port=%d, \n", out_port);
      return rc;
  }
  *encap_id = l3eg.encap_id;
  *intf = l3egid;

  return rc;
}


/*
 * delete l3 interface
 *
 */
int delete_l3_ingress(int unit) {

  int rc;

  rc = bcm_l3_intf_delete_all(unit);

  return rc;
}



/************************************
 * Utilities
 *
 ***********************************/
void print_host(char *type, bcm_ip6_t host, int vrf)
{
  int a,b,c,d;

  printf("%s  vrf:%d   ", type, vrf);
  print_ipv6_addr("host", host);
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
 * Add Route
 *
 */
int add_route_ip6(int unit, bcm_ip6_t *addr, bcm_ip6_t *mask, int vrf, int intf) {

  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(&l3rt);

  l3rt.l3a_flags = BCM_L3_IP6;
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  l3rt.l3a_vrf = vrf;
  l3rt.l3a_intf = intf;
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %d \n", rc);
  }
  
  bcm_l3_route_t_init(&l3rt);
  l3rt.l3a_flags = BCM_L3_IP6;
  l3rt.l3a_vrf = vrf;
  sal_memcpy(&(l3rt.l3a_ip6_net),(addr),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask),16);
  rc = bcm_l3_route_get(unit, &l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_get failed: %d \n", rc);
  }
  print l3rt;
  
  return rc;
}

/*
 * Add Route default
 *
 */
int add_route_ip6_default(int *units_ids, int nof_units) {

  int rc;
  bcm_l3_route_t l3rt;
  bcm_ip6_t addr_int;
  bcm_ip6_t mask_int;
  int unit, i;

  bcm_l3_route_t_init(&l3rt);

  /* UC IPV6 DIP: */
  addr_int[15]= 0; /* LSB */
  addr_int[14]= 0;
  addr_int[13]= 0;
  addr_int[12]= 0;
  addr_int[11]= 0x7;
  addr_int[10]= 0xdb;
  addr_int[9] = 0;
  addr_int[8] = 0;
  addr_int[7] = 0;
  addr_int[6] = 0x70;
  addr_int[5] = 0;
  addr_int[4] = 0x35;
  addr_int[3] = 0;
  addr_int[2] = 0x16;
  addr_int[1] = 0;
  addr_int[0] = 0x1; /* MSB */

  /* UC IPV6 DIP MASK: */
  mask_int[15]= 0xff;
  mask_int[14]= 0xff;
  mask_int[13]= 0xff;
  mask_int[12]= 0xff;
  mask_int[11]= 0xff;
  mask_int[10]= 0xff;
  mask_int[9] = 0xff;
  mask_int[8] = 0xff;
  mask_int[7] = 0;
  mask_int[6] = 0;
  mask_int[5] = 0;
  mask_int[4] = 0;
  mask_int[3] = 0;
  mask_int[2] = 0;
  mask_int[1] = 0;
  mask_int[0] = 0;

  sal_memcpy(&(l3rt.l3a_ip6_net),(addr_int),16);
  sal_memcpy(&(l3rt.l3a_ip6_mask),(mask_int),16);
  l3rt.l3a_vrf = default_vrf;
  l3rt.l3a_intf = default_intf;

  l3rt.l3a_flags = BCM_L3_IP6;
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  for (i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rc = bcm_l3_route_add(unit, l3rt);
      if (rc != BCM_E_NONE) {
        printf ("bcm_l3_route_add failed: %d \n", rc);
      }
  }

  return rc;
}

/*
 * Add IPV6 Host
 *
 */
int add_ipv6_host(int unit, bcm_ip6_t *addr, int vrf, int fec, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags =  BCM_L3_IP6;
  sal_memcpy(&(l3host.l3a_ip6_addr),(addr),16);
  l3host.l3a_vrf = vrf;
  /* dest is FEC + OutLif */
  l3host.l3a_intf = fec; /* fec */
  l3host.encap_id = intf; /* outlif */
  l3host.l3a_modid = 0;
  l3host.l3a_port_tgid = 0;

  rc = bcm_l3_host_add(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %d \n", rc);
  }
  return rc;
}

/*
 * Set IPV6 default dest 
 *      mc: if set config default dest for IPV6 MC, else UC 
 *      trap: if set make default dest according to trap code, else VRF default
 *      out_port: used only if trap is set, as the trap dest
 */
int ipv6_default_dest_set(int unit, int mc, int trap, int out_port) {
  int rc;
  int trap_id;
  bcm_rx_trap_t trap_type;
  bcm_rx_trap_config_t trap_cfg;

  if (mc) {
      trap_type = bcmRxTrapDefaultMcv6;
	  if (_g_bcmRxTrapDefaultMcv6_trap_id >= 0) {
          /* Trap already set, using existing */
          trap_id = _g_bcmRxTrapDefaultMcv6_trap_id;
	  } else {
		  /* Trap no set yet */
		  rc = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id);
		  _g_bcmRxTrapDefaultMcv6_trap_id = trap_id;
		  if (rc != BCM_E_NONE) {
			printf ("bcm_rx_trap_type_create failed: %d \n", rc);
		  }
	  }
  }
  else { /* UC */
      trap_type = bcmRxTrapDefaultUcv6;
      if (_g_bcmRxTrapDefaultUcv6_trap_id >= 0) {
          /* Trap already set, using existing */
          trap_id = _g_bcmRxTrapDefaultUcv6_trap_id;
	  } else {
		  /* Trap no set yet */
		  rc = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_id);
		  _g_bcmRxTrapDefaultUcv6_trap_id = trap_id;
		  if (rc != BCM_E_NONE) {
			printf ("bcm_rx_trap_type_create failed: %d \n", rc);
		  }
	  }
  }

  bcm_rx_trap_config_t_init(&trap_cfg);
  if (trap) {
      trap_cfg.trap_strength = 7;
  }
  else { /* VRF */
      trap_cfg.trap_strength = 0;
  }
  trap_cfg.flags |= BCM_RX_TRAP_UPDATE_DEST;
  trap_cfg.dest_port = out_port;

  rc = bcm_rx_trap_set(unit, trap_id, &trap_cfg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_rx_trap_set failed: %d \n", rc);
  }
  return rc;
}

/* delete functions */
int delete_host(int unit, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_intf = intf;

  rc = bcm_l3_host_delete_all(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %d \n", rc);
  }
  return rc;
}


int MAX_PORTS=2;

int bcm_l3_ip6(int unit, int second_unit ,int in_port, int out_port) {

    return bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 255);
}

int bcm_l3_ip6_mc_with_ttl(int unit, int second_unit, int in_port, int out_port, int ttl_threshold) {

    int rv;
    add_ipv6_uc_routes=0;

    rv = bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, ttl_threshold);
    /* restore add UC entries default */
    add_ipv6_uc_routes=1;
    return rv;
}

int bcm_l3_ip6_mc(int unit, int second_unit ,int in_port, int out_port) {

    int rv;
    add_ipv6_uc_routes=0;

    rv = bcm_l3_ip6_with_ttl(unit, second_unit, in_port, out_port, 255);
    /* restore add UC entries default */
    add_ipv6_uc_routes=1;
    return rv;
}

int bcm_l3_ip6_with_ttl(int unit, int second_unit, int in_port, int out_port, int ttl_threshold) {

  int CINT_NO_FLAGS = 0;
  int vrf = 3;
  int rc;
  int in_vlan = 17;
  int out_vlan = 18;
  int nof_mc_members = 2;
  int ipmc_index[MAX_PORTS] = {6010, 6011};

  int ingress_intf[MAX_PORTS];
  int egress_intf[MAX_PORTS];
  int encap_id[MAX_PORTS];
  bcm_multicast_t mc_id[MAX_PORTS];

  /* used for multi device: */
  int second_ingress_intf[MAX_PORTS];
  int ingress_port_list[MAX_PORTS];
  int egress_port_list[MAX_PORTS];

  bcm_if_t cud;
  int trap_id, port_ndx;
  bcm_rx_trap_config_t trap_config;

  bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

  
  /*bshell(0, "dbm bcm +l3 +api +verbose");*/

  if (second_unit<0)
  {
      /* create in-rif */
      /* TTL/hop-limit threshold in valid only only in the egress, so for in-rif it is set to max - 255 */
      rc = create_l3_intf(unit ,CINT_NO_FLAGS, in_port, in_vlan, vrf, mac_l3_ingress, 255, &ingress_intf[0]);
      if (rc != BCM_E_NONE) {
        printf ("create_l3_intf failed: %d \n", rc);
        return rc;
      }

      /* create out-rif */
      rc = create_l3_intf(unit ,CINT_NO_FLAGS, out_port, out_vlan, vrf, mac_l3_ingress, ttl_threshold, &ingress_intf[1]);
      if (rc != BCM_E_NONE) {
        printf ("create_l3_intf failed: %d \n", rc);
        return rc;
      }

      /* create fec for UC IPV6 entry */
      rc = create_l3_egress(unit,CINT_NO_FLAGS, out_port, out_vlan, ingress_intf[1], mac_l3_egress, &egress_intf[0], &encap_id[0]);      
      if (rc != BCM_E_NONE) {
        printf ("create_l3_egress failed: %d \n", rc);
        return rc;
      }

      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      mc_id[0] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], out_port, nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan);

      /* Add vlan member to each outgoing port */
      for(port_ndx = 0; port_ndx < nof_mc_members; ++port_ndx) {        
        rc = bcm_vlan_gport_add(unit, out_vlan, out_port+port_ndx, 0);
        if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_port+port_ndx, out_vlan);
          return rc;
        }
      }

      /* add also the in_port (with the in_vlan) to the MC group */
      rc = bcm_multicast_l3_encap_get(unit, mc_id[0], in_port, in_vlan, &cud);
      if (rc != BCM_E_NONE) {
            printf("Error, in bcm_petra_multicast_l3_encap_get, mc_group mc_group \n");
            return rc;
      }
      rc = bcm_multicast_ingress_add(unit, mc_id[0], in_port, cud);
      if (rc != BCM_E_NONE) {
         printf("Error, in bcm_multicast_egress_add, ipmc_index %d dest_gport $in_port \n", mc_id[0]);
         return rc;
      }
  }
  else /* second_unit>=0 */
  {  
      /* ingress on dev 0 egress on dev 1 */
      BCM_GPORT_MODPORT_SET(ingress_port_list[0], unit, in_port);
      BCM_GPORT_MODPORT_SET(egress_port_list[0], second_unit, out_port);

      /* create in-rif */
      rc = create_l3_intf(unit, CINT_NO_FLAGS, ingress_port_list[0], in_vlan, vrf, 255, &ingress_intf[0]);
      if (rc != BCM_E_NONE) {
         printf ("create_l3_intf failed: %d \n", rc);
         return rc;
      }

      /* create out-rif */
      rc = create_l3_intf(second_unit, CINT_NO_FLAGS, egress_port_list[0], out_vlan, vrf, ttl_threshold, &second_ingress_intf[0]);
      if (rc != BCM_E_NONE) {
         printf ("create_l3_intf failed: %d \n", rc);
         return rc;
      }

      /* create fec for UC IPV6 entry */
      rc = create_l3_egress(second_unit, CINT_NO_FLAGS, egress_port_list[0], out_vlan, second_ingress_intf[0], &egress_intf[0], &encap_id[0]);
      if (rc != BCM_E_NONE) {
          printf ("create_l3_egress failed: %d \n", rc);
          return rc;
      } 

      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      mc_id[0] = create_ip_mc_group(second_unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[0], egress_port_list[0], nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[0], ipmc_index[0], nof_mc_members, out_vlan);

      /* egress on dev 0 ingress on dev 1 */
      BCM_GPORT_MODPORT_SET(egress_port_list[1], unit, out_port);
      BCM_GPORT_MODPORT_SET(ingress_port_list[1], second_unit, in_port);

      /* create in-rif */
      rc = create_l3_intf(second_unit ,CINT_NO_FLAGS, ingress_port_list[1], in_vlan, vrf, 255, &ingress_intf[1]);
      if (rc != BCM_E_NONE) {
         printf ("create_l3_intf failed: %d \n", rc);
         return rc;
      }

      /* create out-rif */
      rc = create_l3_intf(unit ,CINT_NO_FLAGS, ingress_port_list[1], out_vlan, vrf, ttl_threshold, &second_ingress_intf[1]);
      if (rc != BCM_E_NONE) {
         printf ("create_l3_intf failed: %d \n", rc);
         return rc;
      }

      /* create fec for UC IPV6 entry */
      rc = create_l3_egress(unit, CINT_NO_FLAGS, egress_port_list[1], out_vlan, second_ingress_intf[1], &egress_intf[1], &encap_id[1]);
      if (rc != BCM_E_NONE) {
          printf ("create_l3_egress failed: %d \n", rc);
          return rc;
      } 

      /* create MC group for MC IPV6 entry:
       * this group will contain out_port, out_port+1, ... , out_port+(nof_mc_members-1) */
      mc_id[1] = create_ip_mc_group(unit, BCM_MULTICAST_INGRESS_GROUP, ipmc_index[1], egress_port_list[1], nof_mc_members, out_vlan);
      printf("mc_id %d  ipmc_index %d nof_mc_members %d vlan %d\n", mc_id[1], ipmc_index[1], nof_mc_members, out_vlan);
  }
 
  bcm_ip6_t route0;
  bcm_ip6_t route1;
  bcm_ip6_t route2;
  bcm_ip6_t route3;
  bcm_ip6_t route4;
  bcm_ip6_t mask;

  /* UC IPV6 DIP: */
  route0[15]= 0; /* LSB */
  route0[14]= 0;
  route0[13]= 0;
  route0[12]= 0;
  route0[11]= 0x7;
  route0[10]= 0xdb;
  route0[9] = 0;
  route0[8] = 0;
  route0[7] = 0;
  route0[6] = 0x70;
  route0[5] = 0;
  route0[4] = 0x35;
  route0[3] = 0;
  route0[2] = 0x16;
  route0[1] = 0;
  route0[0] = 0x1; /* MSB */

  route1[15]= 0;
  route1[14]= 0;
  route1[13]= 0;
  route1[12]= 0;
  route1[11]= 0x7;
  route1[10]= 0xdb;
  route1[9] = 0;
  route1[8] = 0;
  route1[7] = 0;
  route1[6] = 0x64;
  route1[5] = 0;
  route1[4] = 0x35;
  route1[3] = 0;
  route1[2] = 0x16;
  route1[1] = 0;
  route1[0] = 0x1;

  route2[15]= 0;
  route2[14]= 0;
  route2[13]= 0;
  route2[12]= 0;
  route2[11]= 0x7;
  route2[10]= 0xdb;
  route2[9] = 0;
  route2[8] = 0;
  route2[7] = 0;
  route2[6] = 0x78;
  route2[5] = 0;
  route2[4] = 0x55;
  route2[3] = 0;
  route2[2] = 0x16;
  route2[1] = 0;
  route2[0] = 0x1;

  route3[15]= 0;
  route3[14]= 0;
  route3[13]= 0;
  route3[12]= 0;
  route3[11]= 0x7;
  route3[10]= 0xdb;
  route3[9] = 0;
  route3[8] = 0;
  route3[7] = 0;
  route3[6] = 0x88;
  route3[5] = 0;
  route3[4] = 0x78;
  route3[3] = 0;
  route3[2] = 0x16;
  route3[1] = 0;
  route3[0] = 0x1;

  /* UC IPV6 DIP MASK: */
  mask[15]= 0xff;
  mask[14]= 0xff;
  mask[13]= 0xff;
  mask[12]= 0xff;
  mask[11]= 0xff;
  mask[10]= 0xff;
  mask[9] = 0xff;
  mask[8] = 0xff;
  mask[7] = 0;
  mask[6] = 0;
  mask[5] = 0;
  mask[4] = 0;
  mask[3] = 0;
  mask[2] = 0;
  mask[1] = 0;
  mask[0] = 0;

  /* MC IPV6 DIP */
  route4[15]= 0x0;
  route4[14]= 0x1;
  route4[13]= 0x2;
  route4[12]= 0x3;
  route4[11]= 0x4;
  route4[10]= 0x5;
  route4[9] = 0x6;
  route4[8] = 0x7;
  route4[7] = 0x8;
  route4[6] = 0x9;
  route4[5] = 0xa;
  route4[4] = 0xb;
  route4[3] = 0xc;
  route4[2] = 0xd;
  route4[1] = 0x1e;
  route4[0] = 0xff;

  int i;
  for (i=0; i<5; i++) { 

    if (i == 4) {
        /* add MC entry */
        rc = find_ip6mc_entry(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0]);
        if (rc != BCM_E_NONE)
        {
            rc = add_ip6mc_entry(unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[0], vrf);            
            if (rc != BCM_E_NONE) {
                printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                return BCM_E_FAIL;
            }   
        }
        if (second_unit>=0)
        {
            rc = find_ip6mc_entry(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
            if (rc != BCM_E_NONE) {
                rc = add_ip6mc_entry(second_unit, route4 /* MC DIP */, route0 /* UC SIP */, in_vlan, mc_id[1]);
                if (rc != BCM_E_NONE) {
                    printf("Error. add_ip6mc_entry failed: $rc\n", rc);
                    return BCM_E_FAIL;
                } 
            }
        }
    }
    else {
        if (add_ipv6_uc_routes==1) {
            /* change the prefix length, for IPV6 UC */
            if (i == 1) {
                mask[8] = 0xfc;
            }
            else if (i == 2) {
                mask[8] = 0xf0;
            }
            else if (i == 3) {
                mask[8] = 0xc0;
            }

            /* add UC entries */
            if (second_unit<0) {
                if (i == 0) { 
                    rc = add_route_ip6(unit, route0, mask, vrf, egress_intf[0]);
                }
                else if (i == 1) {
                    rc = add_route_ip6(unit, route1, mask, vrf, egress_intf[0]);
                }
                else if (i == 2) {
                    rc = add_route_ip6(unit, route2, mask, vrf, egress_intf[0]);
                }
                else { /*i == 3*/
                    rc = add_route_ip6(unit, route3, mask, vrf, egress_intf[0]);
                }
            }
            else /* second_unit>=0 */
            {
                if (i == 0) 
                  rc = add_route_ip6(unit, route0, mask, vrf, egress_intf[i%MAX_PORTS]);
                else if (i == 1) 
                  rc = add_route_ip6(unit, route1, mask, vrf, egress_intf[i%MAX_PORTS]);
                else if (i == 2) 
                  rc = add_route_ip6(second_unit, route2, mask, vrf, egress_intf[i%MAX_PORTS]);
                else /*i == 3*/
                  rc = add_route_ip6(second_unit, route3, mask, vrf, egress_intf[i%MAX_PORTS]);
            }
        }
        if (rc != BCM_E_NONE) {
            printf("Error. add_route_ip6 failed: $rc\n", rc);
            return BCM_E_FAIL;
        }    
    }

    if (add_ipv6_uc_routes==1) {
        if (i == 0) 
    	    print_ipv6_addr("route", route0);
        else if (i == 1) 
    	    print_ipv6_addr("route", route1);
        else if (i == 2) 
    	    print_ipv6_addr("route", route2);
        else /* (i == 3) */
    	    print_ipv6_addr("route", route3);

        if (i != 4) {
          printf("                                                ");
          print_ipv6_addr("mask", mask);
        }
    }
    if  (i == 4) /*i == 4 MC*/
	    print_ipv6_addr("route", route4);
 
  } /* for loop*/

  if (ttl_threshold != 255) {

      /* set egress Action-Profile-TTL-Scope trap to drop all packets MC with TTL/hop-limit that is lower than the threshold */
      rc = bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgIpmcTtlErr, &trap_id);
      if (rc != BCM_E_NONE) {
          printf("Error. bcm_rx_trap_type_create failed: $rc\n", rc);
          return BCM_E_FAIL;
      }

      sal_memset(&trap_config, 0, sizeof(trap_config));
      trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;
      trap_config.dest_port = BCM_GPORT_BLACK_HOLE; /* drop */

      rc = bcm_rx_trap_set(unit, trap_id, &trap_config);
      if (rc != BCM_E_NONE) {
          printf("Error. bcm_rx_trap_set failed: $rc\n", rc);
          return BCM_E_FAIL;
      }
  }

  /*bshell(0, "dbm bcm -api");  */
  return 0;
}


/*
 * packet will be routed from in_port to out-port 
 * packet processing doesn't use FEC for outRIF, 
 * yet the next-hop mac is pointed directly from host lookup
 * host lookup returns: <dest-port, FEC, MAC (pointed by outlif)
 * FEC returns out-RIF.
  * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 2.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - IPV6 DIP = 01001600350070000000db0712000000 (0100:1600:7800:8800:0000:db07:1200:0000)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}
 */
 int ipv6_host_fec_per_outrif(int unit, int in_port, int out_port){

    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2];
    int encap_id[2];
    int flags = 0;
    int in_vlan = 2; 
    int out_vlan = 100;
    int vrf = 0;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01};
    bcm_l3_host_t l3host;
    bcm_ip6_t host;  

    host[15]= 0;
    host[14]= 0;
    host[13]= 0;
    host[12]= 0x12;
    host[11]= 0x7;
    host[10]= 0xdb;
    host[9] = 0;
    host[8] = 0;
    host[7] = 0;
    host[6] = 0x88;
    host[5] = 0;
    host[4] = 0x78;
    host[3] = 0;
    host[2] = 0x16;
    host[1] = 0;
    host[0] = 0x1;

    /*** create ingress router interface ***/
    rv = create_l3_intf(unit, flags, in_port, in_vlan, vrf, my_mac_address, 62, &ing_intf_in); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface, in_port=%d, \n", in_port);
    }

    /*** create egress router interface ***/
    rv = create_l3_intf(unit, flags, out_port, out_vlan, vrf, my_mac_address, 62, &ing_intf_out); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress enterface, out_port=%d, \n", out_port);
    }

    /*** create egress object 1 ***/
    fec[0] = 0;
    encap_id[0] = 0;
    /* built FEC points to out-RIF and MAC address */
    rv = create_l3_egress(unit, flags, out_port, out_vlan, ing_intf_out, next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    printf("created FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);

    /*** add IPV6 host entry with OUT-RIF + MAC + eg-port ***/
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = BCM_L3_IP6;
    sal_memcpy(&(l3host.l3a_ip6_addr),(host),16);
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec[0]; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;
    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC) */
    l3host.encap_id = encap_id[0];
    sal_memcpy(l3host.l3a_nexthop_mac, next_hop_mac, 6); /* next hop mac attached directly */

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rv);
    }
    print_host("added ", host, vrf);
    printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);

    return rv;
}
 

