/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: L3 Routing~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_ipv4_fap.c,v 1.7 Broadcom SDK $
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
 *
 * File: cint_ipv4_fap.c
 * Purpose: Example of how to deliver IP traffic from one source to multiple receivers.
 *
 * CINT assumes diag_init has been executed
 *
 * for one device call bcm_l3_main(int unit, int second_unit, int dst_port) with second_unit<0
 * Example: bcm_l3_main(0,-1, 1);
 *
 * for two devices when one device configures the second device call bcm_l3_main(0, 1, 1) 
 * make sure that master device is unit 0 with modid 0 and slave device is unit 1 with modid 1
 *
 * traffic example for two devices:
 *
 * the cint generates 4 routes and hosts listed below
 *   device 0:
 *   ---------
 *     port: 1           host   6737170e : 103.55.23.14
 *     port: 1           route  78400000 : 120.64.0.0         mask  ffc00000 : 255.192.0.0
 *     port: 2           host   6837170f : 104.55.23.15
 *     port: 2           route  88400000 : 136.64.0.0         mask  ffe00000 : 255.224.0.0
 * 
 *   device 1:
 *   ---------
 *     port: 100         host   6537170c : 101.55.23.12
 *     port: 100         route  70000000 : 112.0.0.0          mask  ff000000 : 255.0.0.0
 *     port: 101         host   6637170d : 102.55.23.13
 *     port: 101         route  64000000 : 100.0.0.0          mask  ff800000 : 255.128.0.0
 *
 * L3 one simple path application: 
 * Sets up interfaces and route to match packets send from the CPU 
 *
 * Example: 
 * bcm_l3_test(unit);
 * pkt_test1();
 * 
 * L3 application: 
 * Sample usage and random routes. User responsible to set traffic according to settings. 
 * This can be done by changing pkt test script or by using traffic generator with the 
 * matching packet header.
 * 
 * Example:
 *      BCM> cint cint_qos_l3.c
 *      BCM> cint cint_ipv4_fap.c
 *      BCM> cint
 *      cint> bcm_l3_main(unit, second_unit, dst_port);
 * 
 * Note: You can also use the individual functions that are included in the example file.
 */
 
 
/*
 * Reset CINT 
 * Enable logging
 * Setup global variables
 */
int verbose = 1; 
uint32 seed = 0x00001234;

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34};
bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};

/*
 * create l3 interface - egress
 *
 */
int create_l3_egress(int unit, uint32 flags, int port, int vlan, int ingress_intf, int *intf, int *encap_id) {


    int rc;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;

    bcm_l3_egress_t_init(&l3eg);
    
    int mod = 0;
    int test_failover_id = 0;
    int test_failover_intf_id = 0;
    
    l3eg.intf = ingress_intf;
    
    sal_memcpy(l3eg.mac_addr, &mac_l3_egress, 6);  
    
    l3eg.vlan   = vlan;
    l3eg.module = mod;
    l3eg.port   = port;
    l3eg.failover_id = test_failover_id;
    l3eg.failover_if_id = test_failover_intf_id;
    l3eg.encap_id = *encap_id;
    l3egid = *intf; 
    
    rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);
    
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
uint32 sal_rand(void) {
    seed = seed + 0x4321; 
    return (seed);
}

void random_host(uint32 *host)
{
    *host =  ((sal_rand()%192)<<24)+((sal_rand()%192)<<16)+((sal_rand()%192)<<8)+(sal_rand()%192);
}

void print_host(char *type, uint32 host)
{
    int a,b,c,d;
    
    a = (host >> 24) & 0xff;
    b = (host >> 16) & 0xff;
    c = (host >> 8 ) & 0xff;
    d = host & 0xff;
    printf("%s  %x : %d.%d.%d.%d \n", type, host, a,b,c,d);
}

/*
 * Add Route
 *
 */
int add_route(int unit, uint32 addr, uint32 mask, int vrf, int intf) {
    int rc;
    bcm_l3_route_t l3rt;
    
    bcm_l3_route_t_init(&l3rt);
    
    l3rt.l3a_flags = BCM_L3_RPF; 

    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = intf;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    
    rc = bcm_l3_route_add(unit, &l3rt);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_route_add failed: %d \n", rc);
    }
    return rc;
}

/*
 * Add Host
 *
 */
int add_host(int unit, uint32 addr, int vrf, int intf) {
    int rc;
    bcm_l3_host_t l3host;
    
    bcm_l3_host_t_init(&l3host);
    
    l3host.l3a_flags = 0;
    l3host.l3a_ip_addr = addr;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = intf;
    l3host.l3a_modid = 0;
    l3host.l3a_port_tgid = 0; 

    
    rc = bcm_l3_host_add(unit, l3host);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_add failed: %d \n", rc);
    }
    return rc;
}

/* delete functions */
int delete_host(int unit, int intf) {
    int rc;
    bcm_l3_host_t l3host;
    
    bcm_l3_host_t_init(&l3host);
    
    l3host.l3a_flags = 0;
    l3host.l3a_intf = intf;
    
    rc = bcm_l3_host_delete_all(unit, l3host);
    if (rc != BCM_E_NONE) {
    printf ("bcm_l3_host_delete_all failed: %d \n", rc);
    }
    return rc;
}

/* 
 * DUNE UI commands to dump/verfiy
 *  -   soc_ppd_api frwrd_ipv4 vrf_route_get vrf_ndx 3 ip_address 001.002.003.000 prefix_len 20 
 *  -   soc_ppd_api frwrd_ipv4 host_get ip_address 010.011.012.1 vrf_ndx 0 
 *
 * L3 IPv4 table commands
 *  -   soc_ppd_api frwrd_ipv4 host_get_block  dumps host table
 *  -   soc_ppd_api frwrd_ipv4 uc_route_get_block type type_fast  dumps uc route table
 *  -   soc_ppd_api frwrd_ipv4 vrf_route_get_block vrf_ndx N type type_fast  dumps vrf route table for vrf index N
 *
 * Diagnostic Shell L3 commands
 *  -   l3 host add vrf=1 ip=2.2.2.2 fec=9
 *  -   l3 route add vrf=5 ip=3.3.3.0 mask=255.255.254.0 fec=5 
 */

int MAX_PORTS=4;
int MAX_HOSTS=100;
int MAX_ROUTES=100;
int MAX_VRF=8;

/*
 * Sample usage and multiple routes. 
 * You responsible to set traffic according to settings.
 */
int bcm_l3_main(int unit, int second_unit, int dst_port) {
    int CINT_NO_FLAGS = 0;
    int rc;
    int i;  
    int egress_port;
    int vlan = 17;
    int tmp_port;
    int vrf = 3;
    
    int ingress_intf[MAX_PORTS];
    int egress_intf[MAX_PORTS];
    int encap_id[MAX_PORTS];
    
    int egress_port_list[MAX_PORTS] = {100,101,1,2};
    int egress_port_list_one_device[MAX_PORTS] = {dst_port,dst_port,dst_port,dst_port};
    
	bcm_port_config_t c;
    bcm_pbmp_t p,u;
    
    if (second_unit<0)
    {
        for (i=0; i<MAX_PORTS; i++) { 
            egress_port=egress_port_list_one_device[i];
        
		    bcm_port_config_get(0, &c);
		    BCM_PBMP_ASSIGN(p, c.e);
		    rc = bcm_vlan_create(unit, vlan);
		    if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
		    	printf ("bcm_vlan_create failed: %d \n", rc);
		    	return rc;
		    }
		    rc = bcm_vlan_port_add(unit, vlan, p, u);
		    	if (rc != BCM_E_NONE) {
		    	return rc;
		    }
		    
		    create_l3_intf_s intf;
		    intf.vsi = vlan;
		    intf.my_global_mac = mac_l3_ingress;
		    intf.my_lsb_mac = mac_l3_ingress;
		    intf.vrf_valid = 1;
		    intf.vrf = vrf;
		    intf.rpf_valid = 1;
		    intf.flags |= BCM_L3_RPF;
		    intf.urpf_mode = bcmL3IngressUrpfLoose;
		    rc = l3__intf_rif__create(unit, &intf);
		    ingress_intf[i] = intf.rif;
		    if (rc != BCM_E_NONE) {
		    	printf("Error, l3__intf_rif__create");
		    }
		    
            rc = create_l3_egress(unit,CINT_NO_FLAGS, egress_port, vlan, ingress_intf[i], &egress_intf[i], &encap_id[i]);    
            if (rc != BCM_E_NONE) {
              printf ("create_l3_egress failed: %d \n", rc);
              return rc;
            }
        }
    }
    else
    {  
        for (i=0; i<MAX_PORTS; i++) {    
            egress_port=egress_port_list[i];

			rc = bcm_vlan_create(unit, vlan);
			if (rc != BCM_E_NONE && rc!=BCM_E_EXISTS) {
				printf ("bcm_vlan_create failed: %d \n", rc);
				return rc;
			}

			bcm_port_config_get(0, &c);
			BCM_PBMP_ASSIGN(p, c.e);
			rc = bcm_vlan_port_add(unit, vlan, p, u);
				if (rc != BCM_E_NONE) {
				return rc;
			}

		    create_l3_intf_s intf;
			intf.vsi = vlan;
			intf.my_global_mac = mac_l3_ingress;
			intf.my_lsb_mac = mac_l3_ingress;
			intf.vrf_valid = 1;
			intf.vrf = vrf;
			ntf.rpf_valid = 1;
			ntf.flags |= BCM_L3_RPF;
			ntf.urpf_mode = bcmL3IngressUrpfLoose;

            if (i<(MAX_PORTS/2)) /*ingress on dev 0 egress on dev 1*/
            {
			    rc = l3__intf_rif__create(unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    rc = l3__intf_rif__create(second_unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
    		    
                rc = create_l3_egress(second_unit, CINT_NO_FLAGS, egress_port, vlan, ingress_intf[i], &egress_intf[i], &encap_id[i]);
                if (rc != BCM_E_NONE) {
                  printf ("create_l3_egress failed: %d \n", rc);
                  return rc;
                } 
                rc = create_l3_egress(unit, BCM_L3_WITH_ID, egress_port, vlan, ingress_intf[i], &egress_intf[i], &encap_id[i]);          
                if (rc != BCM_E_NONE) {
                  printf ("create_l3_egress failed: %d \n", rc);
                  return rc;
                }
            }
            else /*egress on dev 0 ingress on dev 1*/
            {
			    rc = l3__intf_rif__create(second_unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    rc = l3__intf_rif__create(unit, &intf);
			    ingress_intf[i] = intf.rif;
			    if (rc != BCM_E_NONE) {
			    	printf("Error, l3__intf_rif__create");
			    }
			    
                rc = create_l3_egress(unit, CINT_NO_FLAGS, egress_port, vlan, ingress_intf[i], &egress_intf[i], &encap_id[i]);        
			      if (rc != BCM_E_NONE) {
                 printf ("create_l3_egress failed: %d \n", rc);
                 return rc;
                }
                rc = create_l3_egress(second_unit, BCM_L3_WITH_ID, egress_port, vlan, ingress_intf[i], &egress_intf[i], &encap_id[i]);
			      if (rc != BCM_E_NONE) {
                 printf ("create_l3_egress failed: %d \n", rc);
                 return rc;
                }
            }
        }
    }
    
    
    uint32 host[5];
    uint32 route[5];
    uint32 mask;
    int prefix=1;
    
    host[0] = ((101%192)<<24)+((55%192)<<16)+((23%192)<<8)+(12%192); 
    host[1] = ((102%192)<<24)+((55%192)<<16)+((23%192)<<8)+(13%192);
    host[2] = ((103%192)<<24)+((55%192)<<16)+((23%192)<<8)+(14%192);
    host[3] = ((104%192)<<24)+((55%192)<<16)+((23%192)<<8)+(15%192);
    host[4] = ((105%192)<<24)+((55%192)<<16)+((23%192)<<8)+(16%192);
    
    route[0] = ((112%192)<<24)+((52%192)<<16)+((22%192)<<8)+(1%192);
    route[1] = ((100%192)<<24)+((53%192)<<16)+((22%192)<<8)+(1%192);
    route[2] = ((120%192)<<24)+((85%192)<<16)+((22%192)<<8)+(1%192);
    route[3] = ((136%192)<<24)+((95%192)<<16)+((22%192)<<8)+(1%192);
    route[4] = ((32%192)<<24)+((60%192)<<16)+((22%192)<<8)+(1%192);
    
    
    for (i=0; i<4; i++) {
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS]; 
        } 
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }    
        
        if (verbose) {
            printf("on egress interface (fec): %d      port: %d           ", egress_intf[i%MAX_PORTS], tmp_port);
            print_host("host", host[i]);
        }
        add_host (unit, host[i], vrf, egress_intf[i%MAX_PORTS]);
        if (second_unit >= 0)
        {
            rc = add_host (second_unit, host[i], vrf, egress_intf[i%MAX_PORTS]);
            if (rc != BCM_E_NONE) {
              printf("Error. add_host failed: $d\n", rc);
              return BCM_E_FAIL;
            }    
        }
    }
    
    for (i=0; i<4; i++) { 
        if (second_unit < 0)
        {
            tmp_port = egress_port_list_one_device[i%MAX_PORTS]; 
        } 
        else
        {
            tmp_port = egress_port_list[i%MAX_PORTS];
        }  
        prefix--;
        if (prefix == 0) {
            prefix = 24;
        }    
        mask = 0xffffffff << prefix;
        route[i] = route[i] & mask;
        rc = add_route(0, route[i], 0xfffff000, vrf, egress_intf[i%MAX_PORTS]);
        if (rc != BCM_E_NONE) {
          printf("Error. add_route failed: $d\n", rc);
          return BCM_E_FAIL;
        }    
        if (second_unit >= 0)
        {
            rc = add_route(second_unit, route[i], 0xfffff000, vrf, egress_intf[i%MAX_PORTS]);
            if (rc != BCM_E_NONE) {
              printf("Error. add_route failed: $d\n", rc);
              return BCM_E_FAIL;
            }    
        }    
        if (verbose) {
            printf("on egress interface (fec): %d      port: %d     ", egress_intf[i%MAX_PORTS], tmp_port);
            print_host("route", route[i]);
            printf("                                                ");
            print_host("mask", mask);
        }
    }
    
	return 0;
}

