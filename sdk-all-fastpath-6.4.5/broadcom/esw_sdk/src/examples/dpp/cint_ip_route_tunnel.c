/* $Id: cint_ip_route_tunnel.c,v 1.7 Broadcom SDK $
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

/* 
 * Sequence example to  
 * test: 
 * run: 
 * BCM> cint utility/cint_utils_mpls.c  
 * BCM> cint cint_qos.c 
 * BCM> cint utility/cint_utils_l3.c
 * BCM> cint cint_mpls_lsr.c 
 * BCM> cint cint_ip_route.c 
 * BMC> cint cint_mutli_device_utils.c 
 * BCM> cint cint_ip_route_tunnel.c
 * BCM> cint
 * cint> int nof_units = <nof_units>;
 * cint> int units[nof_units] = {<unit1>, <unit2>,...};    
 * cint> int outP = 13;
 * cint> int inP = 13;
 * cint> int outSysport, inSysport;
 * cint> port_to_system_port(unit1, outP, &outSysport);
 * cint> port_to_system_port(unit2, inP, &inSysport);
 * example_ip_to_tunnel(units_ids, nof_units,<inSysport>, <outSysport>); 
 *  
 *  
 * Note: this cint also includes tests for 4 label push and protection 
 */

/* ********* 
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 1;


/********** 
  functions
 ********** */

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
 *  - vlan 100.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
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
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  MPLS label: label 200, exp 2, ttl 20
 */
int example_ip_to_tunnel(int *units_ids, int nof_units, int in_sysport, int out_sysport){
    int rv;
    int unit, i;
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
    int tunnel_id = 0;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    /*** create ingress router interface ***/
    flags = 0;
    ing_intf_in = 0;
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_intf(unit, flags, open_vlan, in_sysport, in_vlan, vrf, mac_address, &ing_intf_in); 
        if (rv != BCM_E_NONE) {
            printf("Error, create ingress interface-1, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** create egress router interface ***/
    ing_intf_out = 0;
    flags = 0;
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_intf(unit,flags,open_vlan,out_sysport,out_vlan,vrf,mac_address, &ing_intf_out); 
        if (rv != BCM_E_NONE) {
            printf("Error, create ingress interface-1, in_sysport=%d, in unit %d \n", in_sysport, unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /*** create egress object 1 ***/
    /*** Create tunnel ***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */
    tunnel_id = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 0;
		mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
	
			printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }
        /* having a tunnel id != 0 is equivalent to being WITH_ID*/

        tunnel_id = mpls_tunnel_properties.tunnel_id;
    }

    /*** Create egress object1 with the tunnel_id ***/
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,tunnel_id,next_hop_mac, &fec[0], &encap_id[0]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
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
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    flags1 = 0;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_egress(unit,flags1,out_sysport,out_vlan,tunnel_id,next_hop_mac2, &fec[1], &encap_id[1]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, out_sysport=%d, in unit %d\n", out_sysport, unit);
        }
        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, \n in unit %d", fec[1], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
        }
        flags1 |= BCM_L3_WITH_ID;
    }

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    /* Units order does not matter*/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_route(unit, route, mask , vrf, fec[1]); 
        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);
        }
    }


    return rv;
}



