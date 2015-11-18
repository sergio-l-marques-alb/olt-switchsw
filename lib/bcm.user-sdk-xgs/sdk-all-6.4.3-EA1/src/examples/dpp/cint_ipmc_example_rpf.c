/*~~~~~~~~~~~~~~~~~~~~~~~Bridge Router: IPMC~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_ipmc_example_rpf.c,v 1.6 Broadcom SDK $
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
 * File: cint_ipmc_example_rpf.c
 * Purpose: Example shows how to configure IP multicast for IPv4 and IPv6.
 *
 * Default Settings:
 * There are two examples, one for IPv4 and one IPv6. Both are similar, except the IP structures.
 *
 * The main example configures the following:
 *   -  Create an IP MC Group create_ip_mc_group (unit, flags, ipmc_index, dest_port_id, vlan).
 *   -  Add an IP MC Entry add_ip4mc_entry (unit, mc_ip, src_ip, vlan, mc_id).
 *  Note that the bcm_multicast_t that was created in the create_ip_mc_group() function is saved to pass it
 *  to the add_ip4mc_entry() as an argument. Passing the MC ID alone without the encapsulation
 *  results in an error. Also, trying to create the entry before the group results in an error.
 *   -  Use bcm_ipmc_find to get the entry that was just added.
 *   -  You can also use the individual functions that are included.
 *
 * IPv4 Application: ipv4_mc_example(unit)
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |  | DA  |SA || TIPD1 | Prio | VID 1 ||   DA IPV4   |   SA IPV4   ||  Data |  |
 *  |  |AB:1D| 1 || 0x8100|      |       ||224.224.224.1| 192.128.1.1 ||       |  |
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                     |
 *  |                     | Figure 20: IPv4OEthernet Packet |                     |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Packet will be sent out to port 2
 *
 * Send L3 multicast traffic
 * Configuration:
 *   Destination IP address (MC): 224.224.224.1
 *   Src IP address: 192.128.1.1
 *   Destination mac: 00:00:00:00:ab:1d
 *   Source port is port 1
 * Expected output:
 *   Traffic on port 2
 *
 * IPv6 Application: ipv6_mc_example(unit)
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |  | DA  |SA||TIPD1 |Prio|VID 1||      DA IPV6      |       SA IPV6     ||Data |  |
 *  |  |AB:1D| 1||0x8100|    |     ||FFFF.FFFF.FFFF.FFFF|12DC.0000.0000.0001||     |  |
 *  |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *  |                       +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                       |
 *  |                       | Figure 21: IPv6OEthernet Packet |                       |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Packet will be sent out to port 2.
 *
 * When using the functions individually, please note the order of running,
 * and the returned value in the create_ip_mc_group() function.
 */

/* how to run:
  * cint cint_ipmc_example.c
  * cint cint_ip_route.c
  * cint cint_ip_route_rpf.c
  * cint cint_ipmc_example_rpf.c
  * cint
  * int rv;
  * rv = ipv4_mc_example_rpf(0, 13, 14,1);
  *
  * how to run global ipmc
  * 1. soc property
  *   ipmc_vpn_lookup_enable=0
  * 2. out_port2 is 0, in case RIF,DIP,SIP aren't exact match (sip is masked).
  *   print global_ipv4_mc_example_rpf(0, 13, 14, 0, 1)
  * 3. out_port1 is 0, in case RIF,DIP,SIP are exact match (no mask).
  *   print global_ipv4_mc_example_rpf(0, 13, 0, 14, 1)
  * 4. In case both have exact match and no-exact match, no-exact match has higher priority
  *   print global_ipv4_mc_example_rpf(0, 13, 14, 15, 1)
  *
  * Please deinit/init bcm.user between step2, 3, and 4
  */



int print_level = 2;
print_level = 2;
int ipmc_vpn_lookup_enable = 1;

/* Add an IPv4 MC entry
*  This entry contains the MC IP, SRC IP and IPMC Group Index.
*   We will create this group later on (actually, you need to create this group
*   BEFORE you call this function)
*/
int add_ip4mc_entry_with_egress_itf(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int vlan, int egress_itf){
	bcm_error_t rv = BCM_E_NONE;
	bcm_ipmc_addr_t data;
	int flags =  BCM_IPMC_SOURCE_PORT_NOCHECK | BCM_IPMC_REPLACE;

	/* Init the ipmc_addr structure and fill the requiered fields.*/
	bcm_ipmc_addr_t_init(&data);
	data.mc_ip_addr = mc_ip;
	data.s_ip_addr = src_ip;
	data.vid = vlan;
	data.flags = flags;
	data.l3a_intf = egress_itf;

	/* Creates the entry */
	rv = bcm_ipmc_add(unit,&data);
	if (rv != BCM_E_NONE) {
		printf("Error, in ipmc_add, mc_ip $mc_ip src_ip $src_ip\n");
		return rv;
	}

    if(print_level >= 1) {
        printf("G=0x%08x S=0x%08x V=0x%08x -->  0x%08x\n", mc_ip, src_ip, vlan, egress_itf);
    }

	return rv;
}

/* Used for dvapi only*/
int dvapi_run_ipv4_mc_example_rpf(int unit, int in_port, int out_port){
    int L3_rpf_uc_loose = 130;
    int L3_rpf_uc_strict = 186;
    int L3_rpf_mc_explicit = 187;
    int L3_rpf_mc_sip = 188;
    ipv4_mc_example_rpf(unit,in_port,out_port,1);
}

/* Main IPv4 MC Example */
int ipv4_mc_example_rpf(int unit, int in_port, int out_port,int nof_dest_ports){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    /* compatible MAC 01:00:5E:60:e0:01 */
	bcm_ip_t src_ip = 0x0; /* masked */
	int ipmc_index = 6000;
	int dest_local_port_id = 13;
	int source_local_port_id = 13;
	bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0xab, 0x1d};

    int egr_flags = 0;
    bcm_multicast_t mc_id;
    bcm_gport_t mc_gport;
    int nof_entries = 6;
    int idx;

    int fec[10] = {0x0};
    int eg_itf_index = 0;

    int encap_id[10]={0x0};
    int itf_flags = 0;
    int vrf = 0;

    int host_l = 0x78ffff03; /*120.255.255.0*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;
    bcm_gport_t mc_gport_l;
    bcm_multicast_t mc_id_l;

    int host_r = 0x7fffff03; /*128.255.255.3*/
    int subnet_r = 0x7ffffa00; /*128.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    int open_vlan=1;

    rv = l3_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps...continue \n");
    }

	/* Create the IP MC Group */
	bcm_multicast_destroy(0,ipmc_index);
    bcm_multicast_destroy(0,ipmc_index+1);

    /* multicast for R-ports */
	mc_id = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, ipmc_index, out_port, nof_dest_ports, vlan_r);
    BCM_GPORT_MCAST_SET(mc_gport,mc_id);

    /* multicast for L-ports */
	mc_id_l = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, ipmc_index+1, in_port, nof_dest_ports, vlan_l);
    BCM_GPORT_MCAST_SET(mc_gport_l,mc_id_l);

     /* create l3 interface 1 (L) */
    rv = create_l3_intf(unit,itf_flags,open_vlan,in_port,vlan_l,0,mac_address_l, &ing_intf_l);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, on port=%d, \n", in_port);
    }

    /* create l3 interface 2 (R) */
    rv = create_l3_intf(unit,itf_flags,open_vlan,out_port,vlan_r,0,mac_address_r, &ing_intf_r);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-2, on port=%d, \n", out_port);
    }

    /*
     * create egress object with different RPF checks
     */

    /*
                L3 flags	|      Device-config
     ----------------------------------------------
            BCM_L3_RPF	IPMC	|   MC-RPF-Mode
            ---------   ----    |  -----------
                V	    X	    |   SIP-mode
	            X	    X	    |   No check
	            V	    V	    |   Explicit
                                |   intf used as expected RIF
     ----------------------------------------------
     */

    /*
     *  1. no RPF check
     */

    egr_flags = 0;

    /* create egress objects for IPMC with no RPF */
    /*ing_intf_r,mac_address_next_hop_r doesn't affect IPMc packets  */
    rv = create_l3_egress(unit,egr_flags,mc_gport,0,ing_intf_r,mac_address_next_hop_r, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x with no RPF check  \n", fec[eg_itf_index]);
    }
    ++eg_itf_index;

    /*
     * 2. use SIP RPF check
0     */

    /* create egress objects for IPMC with RPF use SIP strict */
    egr_flags = BCM_L3_RPF;
    rv = create_l3_egress(unit,egr_flags,mc_gport,0,ing_intf_r,mac_address_next_hop_r, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x with RPF use SIP \n", fec[eg_itf_index]);
    }
    ++eg_itf_index;
    /*
     * 3. use explicit RPF check
     */

    /* create egress objects for IPMC with RPF incorrect expected inRIF */
    egr_flags = BCM_L3_RPF|BCM_L3_IPMC;
    rv = create_l3_egress(unit,egr_flags,mc_gport,0,ing_intf_r,mac_address_next_hop_r, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_r);
    }
    ++eg_itf_index;

    /* create egress objects for IPMC with RPF correct expected inRIF */
    egr_flags = BCM_L3_RPF|BCM_L3_IPMC;
    rv = create_l3_egress(unit,egr_flags,mc_gport,0,ing_intf_l,mac_address_next_hop_l, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_l);
    }
    ++eg_itf_index;

    for(idx = 0; idx < eg_itf_index; ++idx) {
        /* Create the IP MC entry */
        rv = add_ip4mc_entry_with_egress_itf(unit, mc_ip, src_ip, vlan_l, fec[idx]);
        if (rv != BCM_E_NONE) return rv;
        mc_ip++;

    }

    /* add mc-entry points directly to MC group: bypass RPF  check */
    rv = add_ip4mc_entry(unit, mc_ip, src_ip, vlan_l, mc_id);
    if (rv != BCM_E_NONE) return rv;
    mc_ip++;

    /* add IPuc route/hosts used for SIP lookup */

    /* create egress objects for IPUC (SIP) with incorrect inRIF */
    egr_flags = 0;
    rv = create_l3_egress(unit,egr_flags,out_port,0,ing_intf_r,mac_address_next_hop_r, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_r);
    }


    /* add host points to egress interface with incorrect RIF */
    rv = add_host(unit, host_r, vrf, fec[eg_itf_index]);
    if (rv != BCM_E_NONE) {
        printf("Error, add host = 0x%08x, \n", host_r);
    }
    if(print_level >= 1) {
        print_host("add host ", host_r,vrf);
        printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[eg_itf_index], ing_intf_r, out_port);
    }

    ++eg_itf_index;

    /* create egress objects for IPUC (SIP) with correct inRIF */
    egr_flags = 0;
    rv = create_l3_egress(unit,egr_flags,in_port,0,ing_intf_l,mac_address_next_hop_l, &fec[eg_itf_index], &encap_id[eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", out_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[eg_itf_index],ing_intf_l);
    }
    /* add route points to egress interface with correct RIF */
    rv = add_route(unit, subnet_l, subnet_l_mask , vrf, fec[eg_itf_index]);
    if (rv != BCM_E_NONE) {
        printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_l,subnet_l_mask);
    }
    if(print_level >= 1) {
        print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
        printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[eg_itf_index], ing_intf_l, in_port);
    }

    ++eg_itf_index;


	return rv;
}

/* Main Global IPv4 MC Example
 *  out_port1, In case  RIF,DIP,SIP are not exact match (have mask, sip is mask).
 *  out_port2, In case RIF,DIP,SIP are exact match (no mask).
 */
int global_ipv4_mc_example_rpf(int unit, int in_port, int out_port1, int out_port2, int nof_dest_ports){
	bcm_error_t rv = BCM_E_NONE;
	int ingress_intf;

	bcm_ip_t mc_ip = 0xE0E0E001; /* 224.224.224.1 */
    /* compatible MAC 01:00:5E:60:e0:01 */
	bcm_ip_t src_ip = 0x0; /* masked */
	int ipmc_index[2];
	int ipmc_index_l = 8000;

    int egr_flags = 0;
    bcm_multicast_t mc_id[2];
    bcm_gport_t mc_gport[2];
    int idx;

    int fec[2][10];
    int eg_itf_index = 0;
    int eg_itf_rpf_sip_index = 0;

    int encap_id[2][10];
    int itf_flags = 0;
    int vrf = 0;

    int host_l = 0x78ffff03; /*120.255.255.0*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;
    bcm_gport_t mc_gport_l;
    bcm_multicast_t mc_id_l;

    int host_r = 0x7fffff03; /*128.255.255.3*/
    int subnet_r = 0x7ffffa00; /*128.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r[2];
    bcm_mac_t mac_address_r[2];  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r[2];
    bcm_ip_t rpf_sip = 0x78fffa0e; /* An IP include by subnet_l*/
    int open_vlan=1;
    int out_port[2];
    int i = 0;

    out_port[0] = out_port1;
    out_port[1] = out_port2;
    ipmc_index[0] = 6000;
    ipmc_index[1] = 6001;
    vlan_r[0] = 20;
    vlan_r[1] = 30;
    /* my-MAC */
    mac_address_r[0][0] = 0x00;
    mac_address_r[0][1] = 0x00;
    mac_address_r[0][2] = 0x00;
    mac_address_r[0][3] = 0x02;
    mac_address_r[0][4] = 0x00;
    mac_address_r[0][5] = 0x02;
    /* my-MAC */
    mac_address_r[1][0] = 0x00;
    mac_address_r[1][1] = 0x00;
    mac_address_r[1][2] = 0x00;
    mac_address_r[1][3] = 0x02;
    mac_address_r[1][4] = 0x00;
    mac_address_r[1][5] = 0x03;

    for (i = 0; i < 2; i++) {
        for (idx = 0; idx < 10; idx++) {
            fec[i][idx] = 0;
            encap_id[i][idx] = 0;
        }
    }

    rv = l3_ip_rpf_config_traps(&unit, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps...continue \n");
    }

	/* Create the IP MC Group */
	bcm_multicast_destroy(0,ipmc_index[0]);
	bcm_multicast_destroy(0,ipmc_index[1]);
    bcm_multicast_destroy(0,ipmc_index_l);

    /* multicast for L-ports */
	mc_id_l = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, ipmc_index_l, in_port, nof_dest_ports, vlan_l);
    BCM_GPORT_MCAST_SET(mc_gport_l,mc_id_l);

     /* create l3 interface 1 (L) */
    rv = create_l3_intf(unit,itf_flags,open_vlan,in_port,vlan_l,0,mac_address_l, &ing_intf_l);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface-1, on port=%d, \n", in_port);
    }

    for (i = 0; i < 2; i++) {
        mc_ip = 0xE0E0E001;
        if (out_port[i] == 0){
            continue;
        }

        /*  In case RIF,DIP,SIP are exact match (no mask). */
        if (out_port2 && (i==1)) {
            src_ip = 0xC0800101; /* 192.128.1.1 */
        }

        /* multicast for R-ports */
    	mc_id[i] = create_ip_mc_group(unit, BCM_MULTICAST_EGRESS_GROUP, ipmc_index[i], out_port[i], nof_dest_ports, vlan_r[i]);
        BCM_GPORT_MCAST_SET(mc_gport[i],mc_id[i]);

        /* create l3 interface 2 (R) */
        rv = create_l3_intf(unit,itf_flags,open_vlan,out_port[i],vlan_r[i],0,mac_address_r[i], &ing_intf_r[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, create ingress interface-2, on port=%d, \n", out_port[i]);
        }

        /*
         * create egress object with different RPF checks
         */

        /*
                    L3 flags	|      Device-config
         ----------------------------------------------
                BCM_L3_RPF	IPMC	|   MC-RPF-Mode
                ---------   ----    |  -----------
                    V	    X	    |   SIP-mode
    	            X	    X	    |   No check
    	            V	    V	    |   Explicit
                                    |   intf used as expected RIF
         ----------------------------------------------
         */

        /*
         *  1. no RPF check
         */

        egr_flags = 0;
        eg_itf_index = 0;

        /* create egress objects for IPMC with no RPF */
        /*ing_intf_r,mac_address_next_hop_r doesn't affect IPMc packets  */
        rv = create_l3_egress(unit,egr_flags,mc_gport[i],0,ing_intf_r[i],mac_address_next_hop_r, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
           printf("Error, create egress object, on port=%d, \n", out_port[i]);
        }
        if(print_level >= 2) {
            printf("created egress object 0x%08x with no RPF check  \n", fec[i][eg_itf_index]);
        }
        ++eg_itf_index;

        /*
         * 2. use SIP RPF check
         */
        /* create egress objects for IPMC with RPF use SIP strict */
        eg_itf_rpf_sip_index = eg_itf_index;
        egr_flags = BCM_L3_RPF;
        rv = create_l3_egress(unit,egr_flags,mc_gport[i],0,ing_intf_r[i],mac_address_next_hop_r, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
           printf("Error, create egress object, on port=%d, \n", out_port[i]);
        }
        if(print_level >= 2) {
            printf("created egress object 0x%08x with RPF use SIP \n", fec[i][eg_itf_index]);
        }
        ++eg_itf_index;
        /*
         * 3. use explicit RPF check
         */

        /* create egress objects for IPMC with RPF incorrect expected inRIF */
        egr_flags = BCM_L3_RPF|BCM_L3_IPMC;
        rv = create_l3_egress(unit,egr_flags,mc_gport[i],0,ing_intf_r[i],mac_address_next_hop_r, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
           printf("Error, create egress object, on port=%d, \n", out_port[i]);
        }
        if(print_level >= 2) {
            printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[i][eg_itf_index],ing_intf_r[i]);
        }
        ++eg_itf_index;

        /* create egress objects for IPMC with RPF correct expected inRIF */
        egr_flags = BCM_L3_RPF|BCM_L3_IPMC;
        rv = create_l3_egress(unit,egr_flags,mc_gport[i],0,ing_intf_l,mac_address_next_hop_l, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
           printf("Error, create egress object, on port=%d, \n", out_port[i]);
        }
        if(print_level >= 2) {
            printf("created egress object 0x%08x with expected inRIF 0x%08x \n", fec[i][eg_itf_index],ing_intf_l);
        }
        ++eg_itf_index;

        for(idx = 0; idx < eg_itf_index; ++idx) {
            /* Create the IP MC entry */
            if (src_ip && idx == eg_itf_rpf_sip_index) {
              /* RPF sip mode, so the SIP must be included by RPF subnet */
              rv = add_ip4mc_entry_with_egress_itf(unit, mc_ip, rpf_sip, vlan_l, fec[i][idx]);
            } else {
              rv = add_ip4mc_entry_with_egress_itf(unit, mc_ip, src_ip, vlan_l, fec[i][idx]);
            }
            if (rv != BCM_E_NONE) return rv;
            mc_ip++;
            if (src_ip) {
              src_ip++;
            }
        }

        /* add mc-entry points directly to MC group: bypass RPF  check */
        rv = add_ip4mc_entry(unit, mc_ip, src_ip, vlan_l, mc_id[i]);
        if (rv != BCM_E_NONE) return rv;
        mc_ip++;
        if (src_ip) {
          src_ip++;
        }

        /* add IPuc route/hosts used for SIP lookup */

        /* create egress objects for IPUC (SIP) with incorrect inRIF */
        egr_flags = 0;
        rv = create_l3_egress(unit,egr_flags,out_port[i],0,ing_intf_r[i],mac_address_next_hop_r, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
           printf("Error, create egress object, on port=%d, \n", out_port[i]);
        }
        if(print_level >= 2) {
            printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[i][eg_itf_index],ing_intf_r[i]);
        }

        /* add host points to egress interface with incorrect RIF */
        rv = add_host(unit, host_r, vrf, fec[i][eg_itf_index]);
        if (rv != BCM_E_NONE) {
            printf("Error, add host = 0x%08x, \n", host_r);
        }
        if(print_level >= 1) {
            print_host("add host ", host_r,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[i][eg_itf_index], ing_intf_r[i], out_port[i]);
        }
        ++eg_itf_index;
    }


    /* create egress objects for IPUC (SIP) with correct inRIF */
    egr_flags = 0;
    i = 1;
    rv = create_l3_egress(unit,egr_flags,in_port,0,ing_intf_l,mac_address_next_hop_l, &fec[i][eg_itf_index], &encap_id[i][eg_itf_index]);
    if (rv != BCM_E_NONE) {
       printf("Error, create egress object, on port=%d, \n", in_port);
    }
    if(print_level >= 2) {
        printf("created egress object 0x%08x for SIP with inRIF 0x%08x \n", fec[i][eg_itf_index],ing_intf_l);
    }
    /* add route points to egress interface with correct RIF */
    rv = add_route(unit, subnet_l, subnet_l_mask , vrf, fec[i][eg_itf_index]);
    if (rv != BCM_E_NONE) {
        printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_l,subnet_l_mask);
    }
    if(print_level >= 1) {
        print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
        printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[i][eg_itf_index], ing_intf_l, in_port);
    }

    ++eg_itf_index;

	return rv;
}


