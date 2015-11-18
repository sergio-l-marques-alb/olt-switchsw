/* $Id: cint_ip_route.c,v 1.34 Broadcom SDK $
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


/*  Note: this cint also includes tests for 4 label push and protection*/

/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;
int L3_uc_rpf_mode = bcmL3IngressUrpfLoose;
/* Should be set if the SOC property bcm886XX_l3_ingress_urpf_enable is set. */
int urpf_mode_per_rif = 0;

/* Used outside basic_example function */
int ip_route_fec = 0;  
int default_intf = 0;    
int default_vrf = 0;

int default_mtu = 0;
int default_mtu_forwarding = 0;

/********** 
  functions
 ********** */


/* to add support for double tagged/untagged packets override or replace this function*/
int double_tag_hook(int unit, bcm_gport_t port, int vlan){
	int rc = BCM_E_NONE;

	/* call bcm_port_tpid_add */

	/* call bcm_vlan_port_create once for double tagged packet*/

	/* call bcm_vlan_port_create again for untagged packet*/

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
 *  - l3_eg_intf: egress router interface will derive (Vlan, SA(
 *  - next_hop_mac_addr: next hop mac address
 *  - *intf: returned value FEC-id
 *  - *encap_id: returned value outlif point to next_hop_mac_addr.
 *
 */
int create_l3_egress(int unit, uint32 flags, int out_port, int vlan, int l3_eg_intf, bcm_mac_t next_hop_mac_addr, int *intf, int *encap_id) {

  int rc;
  bcm_l3_egress_t l3eg;
  bcm_l3_egress_t_init(&l3eg);

  bcm_if_t l3egid;

  int mod = 0;
  int test_failover_id = 0;
  int test_failover_intf_id = 0;

  l3eg.intf = l3_eg_intf;

  sal_memcpy(l3eg.mac_addr, next_hop_mac_addr, 6);  

  l3eg.vlan   = vlan;
  l3eg.module = mod;
  l3eg.port   = out_port;
  l3eg.failover_id = test_failover_id;
  l3eg.failover_if_id = test_failover_intf_id;
  /* Remove all resource flags */
  l3eg.flags = (~(BCM_L3_REPLACE | BCM_L3_INGRESS_ONLY | BCM_L3_EGRESS_ONLY | BCM_L3_WITH_ID)) & flags;
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
      printf("created FEC-id =0x%08x, ", *intf);
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



/************************************
 * Utilities
 *
 ***********************************/

void print_ip_addr(uint32 host)
{
  int a,b,c,d;

  a = (host >> 24) & 0xff;
  b = (host >> 16) & 0xff;
  c = (host >> 8 ) & 0xff;
  d = host & 0xff;
  printf("%d.%d.%d.%d", a,b,c,d);
}


void l2_print_mac(bcm_mac_t mac_address){
    unsigned int a,b,c,d,e,f;
    a = 0xff & mac_address[0];
    b = 0xff & mac_address[1];
    c = 0xff & mac_address[2];
    d = 0xff & mac_address[3];
    e = 0xff & mac_address[4];
    f = 0xff & mac_address[5];
    printf("%02x:%02x:%02x:%02x:%02x:%02x |",
           a,b,c,
           d,e,f);
}

void print_host(char *type, uint32 host, int vrf)
{
  printf("%s  vrf:%d   ", type, vrf);
  if(verbose >= 1) {
      print_ip_addr(host);
  }
}

void print_route(char *type, uint32 host, uint32 mask, int vrf)
{
    printf("%s  vrf:%d   ", type, vrf);
    print_ip_addr(host);
    printf("/");
    print_ip_addr(mask);
}


/*
 * Add Route <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00
 * - vrf: max value depeneds on soc-property in arad.soc
 * - intf: egress object created using create_l3_egress
 *
 */
int add_route(int unit, uint32 addr, uint32 mask, int vrf, int intf) {
  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(l3rt);

  l3rt.l3a_flags = BCM_L3_RPF;
  l3rt.l3a_subnet = addr;
  l3rt.l3a_ip_mask = mask;
  l3rt.l3a_vrf = vrf;
  l3rt.l3a_intf = intf;
  l3rt.l3a_modid = 0;
  l3rt.l3a_port_tgid = 0;

  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %x \n", rc);
  }

  if(verbose >= 1) {
      print_route("add route", addr, mask,vrf);
      printf("---> egress-object=0x%08x, \n", intf);
  }

  return rc;
}


/*
 * set default Router destination for given VRF
 * - vrf: max value depeneds on soc-property in arad.soc
 * - is_mc: wether this set the default destination for UC packet or MC packet.
 * - intf: egress object created using create_l3_egress
 * - trap_port: trap destination.
 * 
 * if trap_port is vlid trap then it used for default destination. otherwise intf (egress-object) is used
 *
 */
int set_default_route(int unit, int vrf, uint8 is_mc, int intf, bcm_gport_t trap_port) {
  int rc;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(l3rt);

  l3rt.l3a_flags = 0;

  if(is_mc) {
      l3rt.l3a_flags |= BCM_L3_IPMC;
  }

  /* to indicate it's default route, set subnet to zero */
  l3rt.l3a_subnet = 0; 
  l3rt.l3a_ip_mask = 0;

  l3rt.l3a_vrf = vrf;
  l3rt.l3a_intf = intf;
  l3rt.l3a_port_tgid = trap_port;

  rc = bcm_l3_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %x \n", rc);
  }

  return rc;
}



/*
 * Add Route <vrf, addr, mask> --> intf
 * - addr: IP address 32 bit value
 * - mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00
 * - vrf: max value depeneds on soc-property in arad.soc
 * - intf: egress object created using create_l3_egress
 *
 */
int get_route(int unit, uint32 addr, uint32 mask, int vrf, int* intf) {
  int rc;
  int modid;
  bcm_l3_route_t l3rt;

  bcm_l3_route_t_init(l3rt);

  bcm_stk_modid_get(unit, &modid);
  
  l3rt.l3a_flags = BCM_L3_RPF;
  l3rt.l3a_subnet = addr;
  l3rt.l3a_ip_mask = mask;
  l3rt.l3a_vrf = vrf;
  l3rt.l3a_intf = 0;
  l3rt.l3a_modid = modid;
  l3rt.l3a_port_tgid = 0;

  rc = bcm_l3_route_get(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %x \n", rc);
  }

  *intf = l3rt.l3a_intf;

  printf ("interface: 0x%08x \n", *intf);

  return rc;
}


/*
 * Add Host <vrf, addr> --> intf
 * - addr: IP address 32 bit value
 * - vrf: max value depeneds on soc-property in arad.soc
 * - intf: egress object created using create_l3_egress
 */
int add_host(int unit, uint32 addr, int vrf, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(&l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_ip_addr = addr;
  l3host.l3a_vrf = vrf;
  l3host.l3a_intf = intf;
  l3host.l3a_port_tgid = 0;

  rc = bcm_l3_host_add(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %x \n", rc);
  }

  if(verbose >= 1) {
      print_host("add host ", addr,vrf);
      printf("---> egress-object=0x%08x, \n", intf);
  }

  return rc;
}


/*
 * Add Host <vrf, addr> --> intf + dest_port
 * - addr: IP address 32 bit value
 * - vrf: max value depeneds on soc-property in arad.soc
 * - intf: egress interface (vlan or tunnel) 
 * - dest_port: destination port to send packet to
 */
int add_host_direct(int unit, uint32 addr, int vrf, int intf, int dest_port) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_ip_addr = addr;
  l3host.l3a_vrf = vrf;
  l3host.l3a_intf = intf;
  l3host.l3a_modid = 0;
  l3host.l3a_port_tgid = dest_port;
  l3host.encap_id = 0;

  rc = bcm_l3_host_add(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %x \n", rc);
  }

  if(verbose >= 1) {
      print_host("add entry direct", addr,vrf);
      printf("---> egress-intf=0x%08x, port=%d, \n", intf,dest_port);
  }

  return rc;
}

/*
 * delete Host <vrf, addr>
 */
int delete_host(int unit, uint32 addr, int vrf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_ip_addr = addr;
  l3host.l3a_vrf = vrf;
  l3host.l3a_modid = 0;
  l3host.l3a_port_tgid = 0;

  rc = bcm_l3_host_delete(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("delete_host failed: %x \n", rc);
  }
  return rc;
}

/*
 * get Host <vrf, addr> to intf mapping
 */
int get_host(int unit, uint32 addr, int vrf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_ip_addr = addr;
  l3host.l3a_vrf = vrf;
  
  rc = bcm_l3_host_find(unit, &l3host);
  if (rc != BCM_E_NONE) {
    printf ("get_host failed: %x \n", rc);
  }
  print l3host;
  return rc;
}


/* 
   check if host entry was accessed by traffic lookup
   e.g. host_accessed(0,0x7fffff03,0,0);
*/
int host_accessed(int unit, uint32 addr, int vrf,uint8 clear_accessed ) {
    int rc;
    bcm_l3_host_t l3host;

    bcm_l3_host_t_init(l3host);

    if (clear_accessed) {
        l3host.l3a_flags |= BCM_L3_HIT_CLEAR;
    }

    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;

    rc = bcm_l3_host_find(unit, &l3host);
    if (rc != BCM_E_NONE) {
      printf ("get_host failed: %x \n", rc);
    }
    if (l3host.l3a_flags & BCM_L3_HIT) {
        printf ("Accessed \n");
    }
    else{
        printf ("Not Accessed \n");
    }

    return rc;
}

/* 
   check if FEC entry was accessed by traffic lookup
   e.g. fec_accessed(0,0x7fffff03,0,0);
*/
int fec_accessed(int unit,bcm_if_t intf, uint8 clear_accessed ) {
  int rc;
  bcm_l3_egress_t l3eg;
  bcm_l3_egress_t_init(&l3eg);

  if (clear_accessed) {
      l3eg.flags |= BCM_L3_HIT_CLEAR;
  }

  rc = bcm_l3_egress_get(unit,intf,&l3eg);
  if (rc != BCM_E_NONE) {
    printf ("fec_get failed: %x \n", rc);
  }
  if (l3eg.flags & BCM_L3_HIT) {
      printf ("Accessed \n");
  }
  else{
      printf ("Not Accessed \n");
  }

  return rc;
}

/* delete functions */
int delete_all_hosts(int unit, int intf) {
  int rc;
  bcm_l3_host_t l3host;

  bcm_l3_host_t_init(l3host);

  l3host.l3a_flags = 0;
  l3host.l3a_intf = intf;

  rc = bcm_l3_host_delete_all(unit, l3host);
  if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %x \n", rc);
  }
  return rc;
}

/* add route */
int internal_ip_route(int unit, int route, int mask, int vrf, int fec){
	int rv = BCM_E_NONE;
    rv = add_route(unit, route, mask, vrf, fec);
    if (rv != BCM_E_NONE) {
		printf("Error, add_route\n");
		return rv;
    }
	return rv;
}

/******* Run example ******/
 
/*
 * packet will be routed from in_port to out-port 
 * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.03)
 * expected: 
 *  - out port = out_port
 *  - vlan 15.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  
 * Route: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan = 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff00-0x7fffff0f except 0x7fffff03
 * expected: 
 *  - out port = out_port
 *  - vlan = 100.
 *  - DA = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  
 */
int basic_example(int *units_ids, int nof_units, int in_sysport, int out_sysport){
    int rv;
    int i, unit;
    int ing_intf_in; 
	int ing_intf_out; 
    int fec[2] = {0x0,0x0};      
    int flags = 0;
    int flags1 = 0;
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;
    int route;
    int mask;
    int result = 0;
    create_l3_egress_s l3_egress;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x20, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac2 */

    /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);

    if (!urpf_mode_per_rif) {
      /* Set uRPF global configuration */
      rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
      if (rv != BCM_E_NONE) {
        return rv;
      }
    }

    /*** create ingress router interface ***/
    flags = 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        /*rv = create_l3_intf(unit, flags, open_vlan, in_sysport, in_vlan, vrf, mac_address, &ing_intf_in); */
        rv = vlan__open_vlan_per_mc(unit,in_vlan, 0x1); 
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
        }

        rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan);
          return rv;
        }

        create_l3_intf_s intf;
        intf.vsi = in_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.mtu_valid = 1;
        intf.mtu = default_mtu;
        intf.mtu_forwarding = default_mtu_forwarding;
        if (urpf_mode_per_rif) {
            intf.rpf_valid = 1;
            intf.flags |= BCM_L3_RPF;
            intf.urpf_mode = L3_uc_rpf_mode;
        }
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_in = intf.rif;
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        /*rv = create_l3_intf(unit,flags,open_vlan,out_sysport,out_vlan,vrf,mac_address, &ing_intf_out); */        
        rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
        }
        rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
          return rv;
        }

        create_l3_intf_s intf;
        intf.vsi = out_vlan;
        intf.my_global_mac = mac_address;
        intf.my_lsb_mac = mac_address;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.mtu_valid = 1;
        intf.mtu = default_mtu;
        intf.mtu_forwarding = default_mtu_forwarding;
        if (urpf_mode_per_rif) {
            intf.rpf_valid = 1;
            intf.flags |= BCM_L3_RPF;
            intf.urpf_mode = L3_uc_rpf_mode;
        }
        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_out = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** create egress object 1 ***/
    /* out_sysport unit is already first */
    l3_egress.allocation_flags = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        /*rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,ing_intf_out,next_hop_mac, &fec[0], &encap_id[0]); */
        l3_egress.gport = out_sysport;
        l3_egress.vlan = out_vlan;
        l3_egress.next_hop_mac_addr = next_hop_mac;
        l3_egress.intf = ing_intf_out;
        l3_egress.encap_id = encap_id[0];
        l3_egress.l3_egress_id = fec[0];
        
        rv = l3__egress__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
        }
        encap_id[0] = l3_egress.encap_id;
        ip_route_fec = fec[0] = l3_egress.l3_egress_id;

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        l3_egress.allocation_flags |= BCM_L3_WITH_ID;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
    }

    /*** create egress object 2***/
    /* out_sysport unit is already first */
    l3_egress.allocation_flags = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        /*rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,ing_intf_out,next_hop_mac2, &fec[1], &encap_id[1]); */
        l3_egress.gport = out_sysport;
        l3_egress.vlan = out_vlan;
        l3_egress.next_hop_mac_addr = next_hop_mac2;
        l3_egress.intf = ing_intf_out;
        l3_egress.encap_id = encap_id[1];
        l3_egress.l3_egress_id = fec[1];
        
        rv = l3__egress__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
        }
        encap_id[1] = l3_egress.encap_id;
        fec[1] = l3_egress.l3_egress_id;
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        l3_egress.allocation_flags |= BCM_L3_WITH_ID;
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        printf("fec used is  = %d\n", fec[1]);

		rv = internal_ip_route(unit, route, mask, vrf, fec[1]) ;
		if (rv != BCM_E_NONE) {
			printf("Error, in function internal_ip_route, \n");
			return rv;
		}

    }

    return rv;
}



 
/* 
 * set defaul route to send to out-port. 
 * out-port:  egress port for default route. 
 * is_trap: if FALSE default destination is FEC                                                            .
 *          otherwise default is trap-port.
 */
int default_route_config(int unit, int in_port, int out_port, int is_trap){
    int rv;
    int ing_intf_in; 
	int ing_intf_out; 
    int fec[2] = {0x0,0x0};      
    int flags = 0;
    int flags1 = 0;
    int in_vlan = 15; 
	int out_vlan = 101;
    int vrf = 0;
    int encap_id[2]={0x0,0x0}; 
    int open_vlan=1;
    bcm_l3_ingress_t l3_ing_if;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    create_l3_intf_s intf;

    /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);

    if (!urpf_mode_per_rif) {
      /* Set uRPF global configuration */
      rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
      if (rv != BCM_E_NONE) {
        return rv;
      }
    }
    
    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit,in_vlan, 0x1); 
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, vlan);
      return rv;
    }
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;    

    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n", in_port);
        return rv;
    }    

    /*** get ingress router interface ***/
    rv = bcm_l3_ingress_get(unit, ing_intf_in, &l3_ing_if);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_ingress_get, ing_intf_in=%d, \n", ing_intf_in);
        return rv;
    }   

    /*** check BCM_L3_INGRESS_GLOBAL_ROUTE is set ***/    
    if ((l3_ing_if.flags & BCM_L3_INGRESS_GLOBAL_ROUTE) == 0) {
        printf("Error, BCM_L3_INGRESS_GLOBAL_ROUTE is not set\n");
        return rv;
    } 	

	/*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, vlan);
      return rv;
    }

    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;    

	if (rv != BCM_E_NONE) {
		printf("Error, l3__intf_rif__create\n");
        return rv;
	}

    /*** create egress object 1 ***/
    rv = create_l3_egress(unit,flags1,out_port,out_vlan,ing_intf_out,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    printf("created FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);

    /* set default route to point to created FEC */
    rv = set_default_route(unit,vrf,0/*is-MC*/,fec[0],0);
    if (rv != BCM_E_NONE) {
        printf("Error, set_default_route\n");
    }
    printf("default route ---> egress-object=0x%08x, port=%d, \n", fec[1], out_port);

    return rv;
}
 
 
 
/*
 * packet will be routed from in_port to out-port 
 * packet processing doesn't consume FEC entry
 * host lookup returns: <dest-port, out-RIF, MAC (pointed by outlif)
 * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x0a00ff00 (10.0.255.00)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 */
int ip_host_no_fec_example(int unit, int in_port, int out_port){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2];
    int encap_id[2];
    int flags = 0;
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host = 0x0a00ff00;
    int open_vlan=1;
    create_l3_intf_s intf;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00}; 
    bcm_l3_host_t l3host;

    /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);

    if (!urpf_mode_per_rif) {
      /* Set uRPF global configuration */
      rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
      if (rv != BCM_E_NONE) {
        return rv;
      }
    }

    /*** create ingress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan,vrf,my_mac_address, &ing_intf_in); */
    rv = vlan__open_vlan_per_mc(unit,in_vlan, 0x1); 
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, vlan);
      return rv;
    }

    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;     

    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan,vrf,my_mac_address, &ing_intf_out); */
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }

    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;     
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /* use egress to set MAC address and outRIF, FEC is not created */

    fec[0] = 0;
    encap_id[0] = 0;
    /*** create egress object 1 ***/
    /* build FEC points to out-RIF and MAC address */

    /* set Flag to indicate not to allocate or update FEC entry */
    flags |= BCM_L3_EGRESS_ONLY;
    rv = create_l3_egress(unit,flags,out_port,out_vlan,ing_intf_out,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    printf("FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);


    /*** add host entry with OUT-RIF + MAC + eg-port ***/

    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = 0; /* no interface VSI is taken from encap-id */
    l3host.l3a_modid = 0;
    l3host.l3a_port_tgid = out_port; /* egress port to send packet to */

    /* as encap id is valid (!=0), host will point to outRIF + outlif (MAC) + port */
    l3host.encap_id = encap_id[0];

    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rv);
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-encap=0x%08x, port=%d, \n", encap_id[0], out_port);

    return rv;
}
 
 

/*
 * packet will be routed from in_port to out-port 
 * packet processing use FEC for outRIF, 
 * yet the next-hop mac is pointed directly from host lookup
 * host lookup returns: <dest-port, FEC, MAC (pointed by outlif)>
 * FEC returns out-RIF.
  * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x0a00ff01 (10.0.255.01)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 */
 int ip_host_fec_per_outrif(int unit, int in_port, int out_port){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2];
    int encap_id[2];
    int flags = 0;
    int in_vlan = 15; 
    int out_vlan = 100;
    int vrf = 0;
    int host = 0x0a00ff01;
    int open_vlan=1;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x01};
    bcm_l3_host_t l3host;
    create_l3_intf_s intf;

    /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);

    if (!urpf_mode_per_rif) {
      /* Set uRPF global configuration */
      rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
      if (rv != BCM_E_NONE) {
        return rv;
      }
    }

    /*** create ingress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan,vrf,my_mac_address, &ing_intf_in); */
    rv = vlan__open_vlan_per_mc(unit,in_vlan, 0x1); 
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
      return rv;
    }

    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;     
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan,vrf,my_mac_address, &ing_intf_out); */
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }

    intf.vsi = out_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;     

    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    fec[0] = 0;
    encap_id[0] = 0;
    /*** create egress object 1 ***/
    /* build FEC points to out-RIF and MAC address */
    rv = create_l3_egress(unit,flags,out_port,out_vlan,ing_intf_out,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    printf("created FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);


    /*** add host entry with OUT-RIF + MAC + eg-port ***/
    
	rv = add_host(unit, host, vrf, fec[0]);
	if (rv != BCM_E_NONE) {
        printf("Error, add_host\n");
    }
    print_host("add entry ", host, vrf);
    printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);

    return rv;
}


/*
 * packet will be routed from in_port to out-port 
 * packet processing use FEC for outRIF, 
 * yet the next-hop mac is pointed directly from host lookup
 * host lookup returns: <dest-port, FEC, MAC (pointed by outlif), MAC-LSB 4b (carried as is)
 * FEC returns out-RIF.
 * HOST: 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 1.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x0a00ff01 (10.0.255.01)
 * expected: 
 *  - out port = out_port
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00 + mac_lsb}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  Parameters:
 *  - Incoming port (in_port)
 *  - Outgoing port (out_port)
 *  - MAC LSB (mac_lsb). Expected DA will be DA + mac_lsb.
 *  For example: mac_lsb = 1 then DA = {0x00, 0x00, 0x00, 0x03, 0x00, 0x02}
 */
 int ip_host_fec_per_outrif_arp_extension(int unit, int in_port, int out_port, int mac_lsb){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2];
    int encap_id[2];
    int flags = 0;
    int in_vlan = 1; 
    int out_vlan = 100;
    int vrf = 0;
    int host = 0x0a00ff01;
    int open_vlan=1;
    bcm_mac_t my_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x03, 0x00, 0x00};
    bcm_l3_host_t l3host;
    create_l3_intf_s intf;

    /* In Arad+ the urpf mode is per RIF (if the SOC property bcm886XX_l3_ingress_urpf_enable is set). */
    urpf_mode_per_rif = soc_property_get(unit , "bcm886xx_l3_ingress_urpf_enable",0);

    if (!urpf_mode_per_rif) {
      /* Set uRPF global configuration */
      rv =  bcm_switch_control_set(unit, bcmSwitchL3UrpfMode, L3_uc_rpf_mode);
      if (rv != BCM_E_NONE) {
        return rv;
      }
    }
    /*** create ingress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,in_port,in_vlan,vrf,my_mac_address, &ing_intf_in); */
    rv = vlan__open_vlan_per_mc(unit,in_vlan, 0x1); 
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
      return rv;
    }

    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;     
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
    /*rv = create_l3_intf(unit,flags,open_vlan,out_port,out_vlan,vrf,my_mac_address, &ing_intf_out); */
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }    
    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rv;
    }

    intf.vsi = out_vlan;
    intf.my_global_mac = my_mac_address;
    intf.my_lsb_mac = my_mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    if (urpf_mode_per_rif) {
        intf.rpf_valid = 1;
        intf.flags |= BCM_L3_RPF;
        intf.urpf_mode = L3_uc_rpf_mode;
    }
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;    

    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }

    fec[0] = 0;
    encap_id[0] = 0;
    /*** create egress object 1 ***/
    /* build FEC points to out-RIF and MAC address */
    rv = create_l3_egress(unit,flags,out_port,out_vlan,ing_intf_out,next_hop_mac, &fec[0], &encap_id[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
    }
    printf("created FEC-id =0x%08x, \n", fec[0]);
    printf("next hop mac at encap-id %08x, \n", encap_id[0]);


    /*** add host entry with OUT-RIF + MAC + MAC-LSB + eg-port ***/
    
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = fec[0]; /* point to FEC to get out-interface  */
    l3host.l3a_modid = 0;
    l3host.l3a_nexthop_mac[5] = mac_lsb; /* MAC-LSB 4b carried as is to egress */
    /* set encap id to point to MAC address */
    /* as encap id is valid (!=0), host will point to FEC + outlif (MAC), and FEC will be "fixed" not to overwrite outlif */
    l3host.encap_id = encap_id[0];
    
    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rv);
        printf("Error, create egress object, in_port=%d, \n", in_port);
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-intf=0x%08x, egress-mac: port=%d, \n", ing_intf_out, out_port);

    return rv;
}

/*
 *  Runs the regular basic_example on one unit instead of an array of units
 *  Used to call this functionality from Dvapi tests
 */
int basic_example_single_unit(int unit, int in_port, int out_port) {
    int unit_ids[1];
    unit_ids[0] = unit;
    return basic_example(unit_ids,1,in_port,out_port);
}
