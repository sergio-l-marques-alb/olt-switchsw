/* $Id: cint_ip_route_rpf.c,v 1.14 Broadcom SDK $
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


/* ********* 
  Globals/Aux Variables
 ********** */

/*
 * run:
 
cint ../../../../src/examples/dpp/utility/cint_utils_global.c
cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c 
cint ../../../../src/examples/dpp/cint_ip_route_rpf.c 
cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
cint ../../../../src/examples/dpp/cint_mim_mp.c
 
from L to R: 
 - for Loose: 
 * packet to send: 
 *  - in syetem port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 130.0.0.0 (not found)
 * Expected:
 *  --> packet discarded, loose check FAIL
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 120.255.250.110 (found, with incorrect RIF)
 * Expected:
 *  --> packet forwarded (as check is loose)
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 127.255.250.110 (found, with correct RIF)
 * Expected:
 *  --> packet forwarded (as check is loose)
 *
 *
 - for Strict: 
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 130.0.0.0 (not found)
 * Expected:
 *  --> packet discarded, strict check FAIL
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 120.255.250.110 (found, with incorrect RIF)
 * Expected:
 *  --> packet discarded (as check is strict)
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 127.255.250.110 (found, with correct RIF)
 * Expected:
 *  --> packet forwarded (as found with correct RIF)
 *
 */

int L3_rpf_uc_loose = 0;
int L3_rpf_uc_strict = 0;
int L3_rpf_mc_explicit = 0;
int L3_rpf_mc_sip = 0;

/********** 
  functions
 ********** */

/******* Run example ******/

/*
 * packet will be routed from in_sysport to out-sysport 
 * HOST: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan 10.
 *  - DA = 00:00:00:02:00:01 {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x7fffff03 (127.255.255.03) or 0x7ffffa00 (127.255.250.0) - 0x7ffffaff (127.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  TTL decremented
 *  
 * Route: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan = 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x78ffff03 (120.255.255.3) or 0x78fffa00 (120.255.250.0) - 0x78fffaff (120.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan = 10.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 int units_ids[2];
 int nof_units = 1;
 int in_sysport = 13;
 int out_sysport = 13;
 int urpf_mode = bcmL3IngressUrpfStrict;
 print example_rpf(units_ids, nof_units, in_sysport, out_sysport, urpf_mode);
 *  
 *  
 */
int example_rpf(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode){
    int rv, i;
    int unit, flags;
    int fec[2] = {0x0,0x0};      
    int itf_flags = BCM_L3_RPF;
    int egr_flags = 0;
    int vrf = 0;

    int host_l = 0x78ffff03; /*120.255.255.3*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l; 

    int host_r = 0x7fffff03; /*127.255.255.3*/
    int subnet_r = 0x7ffffa00; /*127.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r; 

    int encap_id[2]={0x0,0x0};

    create_l3_intf_s intf;

    /* set global definition defined in cint_ip_route.c */
    /* this way all UC-rpf checks is done according to this config */
    L3_uc_rpf_mode = urpf_mode;

    rv = l3_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps \n");
    }

    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, vlan_l, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_l, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
          return rv;
        }

        intf.vsi = vlan_l;
        intf.my_global_mac = mac_address_l;
        intf.my_lsb_mac = mac_address_l;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
		intf.rpf_valid = 1;
		intf.urpf_mode = urpf_mode;
		intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_l = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, vlan_r, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_r, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
          return rv;
        }

        intf.vsi = vlan_r;
        intf.my_global_mac = mac_address_r;
        intf.my_lsb_mac = mac_address_r;
		intf.rpf_valid = 1;
		intf.urpf_mode = urpf_mode;
		intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_r = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
		rv = create_l3_egress(unit,flags,out_sysport,vlan_r,ing_intf_r,mac_address_next_hop_r, &fec[0], &encap_id[0]);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
        flags |= BCM_L3_WITH_ID;
    }


    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
		rv = create_l3_egress(unit,flags,in_sysport,vlan_l,ing_intf_l,mac_address_next_hop_l, &fec[1], &encap_id[1]);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[1]);
            printf("next hop mac at encap-id %08x, \n", encap_id[1]);
        }
        flags |= BCM_L3_WITH_ID;
    }


    /* IPuc add routes/host to Routing table: from L to R*/
	/*** add host point to FEC2 ***/
	for (i = 0 ; i < nof_units ; i++){
		unit = units_ids[i];
		rv = add_host(unit, host_r, vrf, fec[0]);
		if (rv != BCM_E_NONE) {
			printf("Error, add host = 0x%08x, \n", host_r);
		}
		if(verbose >= 1) {
			print_host("add host ", host_r,vrf);
			printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[0], ing_intf_r, out_sysport);
		}
	}

	/*** add route point to FEC2 ***/
	for (i = 0 ; i < nof_units ; i++){
		unit = units_ids[i];
		rv = add_route(unit, subnet_r, subnet_r_mask , vrf, fec[0]);
		if (rv != BCM_E_NONE) {
			printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_r,subnet_r_mask);
		}
		if(verbose >= 1) {
			print_route("add subnet ", subnet_r,subnet_r_mask,vrf);
			printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[0], ing_intf_r, out_sysport);
		}
	}

   /* IPuc add routes/host to Routing table: from R to L*/
	/*** add host point to FEC2 ***/
	for (i = 0 ; i < nof_units ; i++){
		unit = units_ids[i];
		rv = add_host(unit, host_l, vrf, fec[1]);
		if (rv != BCM_E_NONE) {
			printf("Error, add host = 0x%08x, \n", host_l);
		}
		if(verbose >= 1) {
			print_host("add host ", host_l,vrf);
			printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[1], ing_intf_l, in_sysport);
		}
	}

	/*** add route point to FEC2 ***/
	for (i = 0 ; i < nof_units ; i++){
		unit = units_ids[i];
		rv = add_route(unit, subnet_l, subnet_l_mask , vrf, fec[1]);
		if (rv != BCM_E_NONE) {
			printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_l,subnet_l_mask);
		}
		if(verbose >= 1) {
			print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
			printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[1], ing_intf_l, in_sysport);
		}
	}
    return rv;
}

/*
 * packet will be routed from in_sysport to out-sysport
 * HOST:
 * packet to send:
 *  - in port = in_sysport
 *  - vlan 10.
 *  - DA = 00:00:00:02:00:01 {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = (0100:1600:7800:8800:0000:db07:1200:0000) or (0100:1600:7800:8800:0000:0000:0000:0000) - (0100:1600:7800:8800:ffff:ffff:ffff:ffff)
 * expected:
 *  - out system port = out_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  TTL decremented
 *
 * Route:
 * packet to send:
 *  - in port = in_sysport
 *  - vlan = 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0100:1600:7800:8800:0000:db07:1200:0000 or 0100:1600:7800:8800:0000:0000:0000:0000 - 0100:1600:7800:8800:ffff:ffff:ffff:ffff
 * expected:
 *  - out system port = out_sysport
 *  - vlan = 10.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 int units_ids[2];
 int nof_units = 1;
 int in_sysport = 13;
 int out_sysport = 13;
 int urpf_mode = bcmL3IngressUrpfStrict;
 print example_rpf_ipv6(units_ids, nof_units, in_sysport, out_sysport, urpf_mode);
 *
 *
 */
int example_rpf_ipv6(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode){
    int rv, i;
    int unit, flags;
    int fec[2] = {0x0,0x0};
    int itf_flags = BCM_L3_RPF;
    int egr_flags = 0;
    int vrf = 0;
	create_l3_intf_s intf;

    bcm_ip6_t host_l; /*(0100:1600:7800:8800:0000:db07:1200:0000)*/
    bcm_ip6_t mask_l;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l;

    bcm_ip6_t host_r; /*(0100:1600:7800:8800:0000:db07:1200:0000)*/
    bcm_ip6_t mask_r;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r;

    int encap_id[2]={0x0,0x0};

    /* set global definition defined in cint_ip_route.c */
    /* this way all UC-rpf checks is done according to this config */
    L3_uc_rpf_mode = urpf_mode;

    rv = l3_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps \n");
    }

    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, vlan_l, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_l, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
          return rv;
        }

        intf.vsi = vlan_l;
        intf.my_global_mac = mac_address_l;
        intf.my_lsb_mac = mac_address_l;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
		intf.rpf_valid = 1;
		intf.urpf_mode = urpf_mode;
		intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_l = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

		rv = vlan__open_vlan_per_mc(unit, vlan_r, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_r, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
          return rv;
        }

        intf.vsi = vlan_r;
        intf.my_global_mac = mac_address_r;
        intf.my_lsb_mac = mac_address_r;
		intf.rpf_valid = 1;
		intf.urpf_mode = urpf_mode;
		intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_r = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
		rv = create_l3_egress(unit,flags,out_sysport,vlan_r,ing_intf_r,mac_address_next_hop_r, &fec[0], &encap_id[0]);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
        flags |= BCM_L3_WITH_ID;
    }


    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
		rv = create_l3_egress(unit,flags,in_sysport,vlan_l,ing_intf_l,mac_address_next_hop_l, &fec[1], &encap_id[1]);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", in_sysport);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n", fec[1]);
            printf("next hop mac at encap-id %08x, \n", encap_id[1]);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /* Add SIP Routing Entry / IN-RIF / FEC and OUT-RIF  */
    /* SIP ==    0100:0100:0000:0000:0000:0000:0000:0123 */
    /* UC IPV6 SIP: */
    host_l[15]= 0x23; /* LSB */
    host_l[14]= 0x01;
    host_l[13]= 0x00;
    host_l[12]= 0x00;
    host_l[11]= 0x00;
    host_l[10]= 0x00;
    host_l[9] = 0x00;
    host_l[8] = 0x00;
    host_l[7] = 0x00;
    host_l[6] = 0x00;
    host_l[5] = 0x00;
    host_l[4] = 0x00;
    host_l[3] = 0x00;
    host_l[2] = 0x01;
    host_l[1] = 0x00;
    host_l[0] = 0x01; /* MSB */

    /* DIP ==    0200:0200:0000:0000:0000:0000:0000:0135 */
    /* UC IPV6 DIP: */
    host_r[15]= 0x35; /* LSB */
    host_r[14]= 0x01;
    host_r[13]= 0x00;
    host_r[12]= 0x00;
    host_r[11]= 0x00;
    host_r[10]= 0x00;
    host_r[9] = 0x00;
    host_r[8] = 0x00;
    host_r[7] = 0x00;
    host_r[6] = 0x00;
    host_r[5] = 0x00;
    host_r[4] = 0x00;
    host_r[3] = 0x00;
    host_r[2] = 0x02;
    host_r[1] = 0x00;
    host_r[0] = 0x02; /* MSB */

    /* UC IPV6 SIP MASK: */
    mask_l[15]= 0xff;
    mask_l[14]= 0xff;
    mask_l[13]= 0xff;
    mask_l[12]= 0xff;
    mask_l[11]= 0xff;
    mask_l[10]= 0xff;
    mask_l[9] = 0xff;
    mask_l[8] = 0xff;
    mask_l[7] = 0x00;
    mask_l[6] = 0x00;
    mask_l[5] = 0x00;
    mask_l[4] = 0x00;
    mask_l[3] = 0x00;
    mask_l[2] = 0x00;
    mask_l[1] = 0x00;
    mask_l[0] = 0x00;

    /* UC IPV6 SIP MASK: */
    mask_r[15]= 0xff;
    mask_r[14]= 0xff;
    mask_r[13]= 0xff;
    mask_r[12]= 0xff;
    mask_r[11]= 0xff;
    mask_r[10]= 0xff;
    mask_r[9] = 0xff;
    mask_r[8] = 0xff;
    mask_r[7] = 0x00;
    mask_r[6] = 0x00;
    mask_r[5] = 0x00;
    mask_r[4] = 0x00;
    mask_r[3] = 0x00;
    mask_r[2] = 0x00;
    mask_r[1] = 0x00;
    mask_r[0] = 0x00;

    /* IPuc add routes/host to Routing table: from L to R*/
	{
		/*** add host point to FEC2 ***/
	    for (i = 0 ; i < nof_units ; i++){
			unit = units_ids[i];
			rv = add_ipv6_host(unit, host_r, vrf, fec[0], ing_intf_r);
			if (rv != BCM_E_NONE) {
				printf("Error, add host = 0x%08x, \n", host_r);
			}
			if(verbose >= 1) {
				print_ipv6_addr("add host ", host_r);
				printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, vrf=%d\n", fec[1], ing_intf_l, in_sysport, vrf);
			}
		}

		/*** add route point to FEC2 ***/
	    for (i = 0 ; i < nof_units ; i++){
			unit = units_ids[i];
            rv = add_route_ip6(unit, host_r, mask_r , vrf, fec[0]);
			if (rv != BCM_E_NONE) {
				printf("Error, add subnet = 0x%08x/0x%08x, \n", host_r,mask_r);
			}
			if(verbose >= 1) {
                print_ipv6_addr("add route_r", host_r);
                print_ipv6_addr("with mask_r", mask_r);
				printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[0], ing_intf_r, out_sysport);
			}
		}

	   /* IPuc add routes/host to Routing table: from R to L*/
		/*** add host point to FEC2 ***/
	    for (i = 0 ; i < nof_units ; i++){
			unit = units_ids[i];
			rv = add_ipv6_host(unit, host_l, vrf, fec[1], ing_intf_l);
			if (rv != BCM_E_NONE) {
				printf("Error, add host = 0x%08x, \n", host_l);
			}
			if(verbose >= 1) {
				print_ipv6_addr("add host ", host_l);
				printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, vrf=%d\n", fec[1], ing_intf_l, in_sysport, vrf);
			}
		}

		/*** add route point to FEC2 ***/
	    for (i = 0 ; i < nof_units ; i++){
			unit = units_ids[i];
			rv = add_route_ip6(unit, host_l, mask_l , vrf, fec[1]);
			if (rv != BCM_E_NONE) {
				printf("Error, add subnet = 0x%08x/0x%08x, \n", host_l,mask_l);
			}
			if(verbose >= 1) {
                print_ipv6_addr("add route_l", host_l);
                print_ipv6_addr("with mask_l", mask_l);
				printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[1], ing_intf_l, in_sysport);
			}
		}
	}
    return rv;
}

int
vlan_port_create_vsi(
    int unit,
    bcm_port_t port_id,
    bcm_gport_t *gport,
    bcm_vlan_t vlan /* incoming outer vlan and also outgoing vlan */,
    int vsi
    ){

    int rv;
    bcm_vlan_port_t vp;

    bcm_vlan_port_t_init(&vp);
  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = vlan; 
    vp.egress_vlan = vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = vsi; 
    vp.flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE|BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 
    /*vp.flags = 0;*/
    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n"); 
        print rv;
        return rv;
    }

    *gport = vp.vlan_port_id;  
    return BCM_E_NONE;
}

/*
 * Example for RPF where the mode is per RIF. 
 * example_rpf is first called to do the same setup (see example_rpf). 
 * This will cause both RIFs (10 and 20) to be setup and configured strict 
 * uRPF. 
 * Next, two LIFs are created (LIFs are required for uRPF mode), 
 * and two different uRPF modes are configured for each RIF: 
 * 10 - Configured to perform loose uRPF. 
 * 20 - Configured to perform strict uRPF. 
 *  
 */
int example_rpf_per_rif_urpf_mode(int unit, int in_sysport, int out_sysport) {
    int rv = BCM_E_NONE;
    int vlan_l = 10;
    int vlan_r = 20;
    int i;
    int units_ids[] = {unit};
    bcm_gport_t lif;
    int vlan_arr[] = {vlan_l, vlan_r};
    int vlan_port_arr[] = {in_sysport, out_sysport};
    int vlan_n = 2;
    bcm_l3_ingress_t l3i;
    
    /* Make sure the device is Arad+ as uRPF per RIF is supported only on Arad+ and above. */

    if (!is_device_or_above(unit,ARAD_PLUS)) {
        printf("Error - uRPF mode per RIF is only supported for Arad+.\n");
        return BCM_E_FAIL;
    }

    /* cint_ip_route.c uses this variable to decide whether to call the global uRPF mode switch control set. */
    /* If this variable is non-zero then the call is not made. */
    /* Remark - cint_ip_route.c is used by example_rpf. */
    urpf_mode_per_rif = 1;

    /* Initial setup - setup RIF 10, 20 with the same configuration as the global rpf example. */
    /* Initially both RIFs are configured to perform strict uRPF. */
    rv = example_rpf(units_ids, 1, in_sysport, out_sysport, bcmL3IngressUrpfStrict);
    if (rv != BCM_E_NONE) {
        printf("Error doing example_rpf.\n");
        print rv;
        return rv;
    }

    /* Create a LIF for each RIF. This is important for the uRPF mode to work per RIF. */
    for (i = 0; i < vlan_n; i++) {
        /* Create a lif for <vlan,port>. */
        rv = vlan_port_create_vsi(unit, vlan_port_arr[i], &lif, vlan_arr[i], vlan_arr[i]);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create\n");
            return rv;
        }
    }

    /* Change the uRPF mode of RIF 10 to loose. */

    /* Get the configuration of the InRIF. */
    rv = bcm_l3_ingress_get(unit, vlan_l, l3i);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n"); 
        printf("bcm_l3_ingress_get(%d, %d, l3i)\n", unit, vlan_l);
        print l3i;
        print rv;
        return rv;
    }

    /* Change the RPF mode. */
    l3i.urpf_mode = bcmL3IngressUrpfLoose;
    l3i.flags |= BCM_L3_INGRESS_WITH_ID;

    /* Set the configuration of the InRIF. */
    rv = bcm_l3_ingress_create(unit, l3i, vlan_l);
    if (rv != BCM_E_NONE) {
        printf("Error - The following call failed:\n"); 
        printf("bcm_l3_ingress_create(%d, l3i, %d)\n", unit, vlan_l);
        print l3i;
        print rv;
        return rv;
    }

    return rv;
}

int
 l3_ip_rpf_config_traps(int *units_ids, int nof_units) {
 	int rv = BCM_E_NONE, i;
	bcm_rx_trap_config_t config;
	int flags = 0;
    int unit;


    /* 
     * RPF types:
     */

    /* bcmRxTrapUcLooseRpfFail,            : Forwarding Code is IPv4 UC and RPF FEC Pointer Valid is not set. */
    /* bcmRxTrapUcStrictRpfFail,           : UC-RPF-Mode is 'Strict' and OutRIF is not equal to packet InRIF . */
    /* bcmRxTrapMcExplicitRpfFail,         : RPF-Entry. Expected-InRIF is not equal to packet InRIF Relevant when FEC-Entry. MC-RPF-Mode is 'Explicit'. */
    /* bcmRxTrapMcUseSipRpfFail,           : Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP' Note: . */

    /* set uc strict to drop packet */
    if(L3_rpf_uc_strict == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapUcStrictRpfFail,&L3_rpf_uc_strict);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapUcStrictRpfFail \n");
                return rv;
            }
        }
    }
    /* set uc loose to drop packet */
    if(L3_rpf_uc_loose == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapUcLooseRpfFail,&L3_rpf_uc_loose);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapUcLooseRpfFail \n");
                return rv;
            }
        }
    }

    /* set mc SIP to drop packet */
    if(L3_rpf_mc_sip == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapMcUseSipRpfFail,&L3_rpf_mc_sip);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapMcUseSipRpfFail \n");
                return rv;
            }
        }
    }

    /* set mc explict to drop packet */
    if(L3_rpf_mc_explicit == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapMcExplicitRpfFail,&L3_rpf_mc_explicit);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapMcExplicitRpfFail \n");
                return rv;
            }
        }
    }

  /*change dest port for trap */
  bcm_rx_trap_config_t_init(&config);
  config.flags |= (BCM_RX_TRAP_UPDATE_DEST); 
  config.trap_strength = 5;
  config.dest_port=BCM_GPORT_BLACK_HOLE; 
  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_uc_loose,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_uc_loose \n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_uc_strict,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_uc_strict\n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_mc_explicit,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_mc_explicit\n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_mc_sip,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_mc_sip\n");
          return rv;
      }
  }

  return rv;
}



/*
* l3_ip_rpf_config_mc_traps_test configuration
* ipv4-mc rpf trapped packets sent to recycle port. 
* to use this make sure you set the right soc properties 
* set the port <port> as recycle port.
*  	parameters:
*   port - the chosen recycle port  
*/
int
 l3_ip_rpf_config_mc_traps_test(int uint, int port) {
 	int rv = BCM_E_NONE;
	bcm_rx_trap_config_t config;
	int flags = 0;
    int unit;


    /* 
     * RPF types:
     */

    /* bcmRxTrapMcExplicitRpfFail,         : RPF-Entry. Expected-InRIF is not equal to packet InRIF Relevant when FEC-Entry. MC-RPF-Mode is 'Explicit'. */
    /* bcmRxTrapMcUseSipRpfFail,           : Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP' Note: . */


	bcm_rx_trap_config_t_init(&config);
	BCM_GPORT_SYSTEM_PORT_ID_SET(config.dest_port, port);
	print config.dest_port;
	config.flags = BCM_RX_TRAP_UPDATE_DEST;
	config.trap_strength = 5;


	rv = bcm_rx_trap_set(0, L3_rpf_mc_explicit, config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set bcm_rx_trap_set\n");
          return rv;
      }
	rv = bcm_rx_trap_set(0, L3_rpf_mc_sip, config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_McUseSip\n");
          return rv;
      }

  return rv;
}
