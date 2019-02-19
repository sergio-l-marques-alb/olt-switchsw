/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~BFD test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * $Id: cint_bfd_ipv6.c,v 1.15 Broadcom SDK $
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
 * File: cint_bfd_ipv6.c
 * Purpose: Example of using BFD IPv6 APIs that tests for BFD connection to bring seesions UP.
 *
 * Soc Properties configuration:
 *     port_priorities.BCM88650=8 to configure the number of priorities at egq.
 *     num_queues_pci=40 to configure the ARAD cosq numbers, from 0-39.
 *     num_queues_uc0=8 to configure the ARM core 0 cosq numbers, from 40-47. 
 *     ucode_port_204.BCM88650=CPU.40 to configure local port 204 to send the DSP packet. 
 *     tm_port_header_type_in_204.BCM88650=INJECTED_2_PP 
 *     tm_port_header_type_out_204.BCM88650=CPU to trap the BFD packet to uKernel with the FTMH and PPH header
 *
 * Service Model:
 *     Back-to-back tests using 2 ARAD_PLUS(A, B):
 *     1) Send the BFD IPv6 packet from port_a of arad_plus_a to port_b of arad_plus_b.
 *     2) Send the BFD IPv6 packet from port_b of arad_plus_b to port_a of arad_plus_a.
 * Service connection:
 *     --------------                 --------------
 *     | arad_plus_a  |<--------->| arad_plus_b  |
 *     --------------                 --------------
 *                        port_a       port_b
 * 
 * To run IPV6 test:
 * BCM> cint examples/dpp/cint_ip_route.c
 * BCM> cint examples/dpp/cint_bfd.c 
 * BCM> cint examples/dpp/cint_bfd_ipv6.c 
 * BCM> cint 
 * print bfd_ipv6_init(unit, port, vlan, type);
 * print bfd_ipv6_example(unit, endpoint_id);
 * 
 * 
 */
 
 /* debug prints */
 int verbose = 1;

int bfd_port = 13;
int bfd_dsp_port = 204;/* the bfd_dsp_port directly connect to the ARM core 0 */
int bfd_vlan = 100;
int bfd_fec;
uint32 disc[2] = {10, 5010};
bcm_mac_t mac_addr[2] = {
    {0x00, 0x00, 0x00, 0x01, 0x01, 0x01}, 
    {0x00, 0x00, 0x00, 0x02, 0x02, 0x02}};
bcm_ip6_t ipv6[2] = { 
    {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x00,0x01},
    {0xfe,0xc0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x02,0x00,0x02}};
bcm_mac_t *my_mac;
bcm_mac_t *nhp_mac;
bcm_ip6_t *src_ipv6;
bcm_ip6_t *dst_ipv6;
uint32 *local_disc;
uint32 *remote_disc;


/*
 *Initialize the BFD application configuration for local or remote device.
 */
int bfd_ipv6_init(int unit, int port, int vlan, int remote) {
    int rv = 0;

    bfd_port = port;
    bfd_vlan = vlan;

    if (remote) {
        local_disc = &disc[0];
        remote_disc = &disc[1];
        my_mac = &mac_addr[0];
        nhp_mac = &mac_addr[1];
        src_ipv6 = &ipv6[0];
        dst_ipv6 = &ipv6[1];
    } else {
        local_disc = &disc[1];
        remote_disc = &disc[0];
        my_mac = &mac_addr[1];
        nhp_mac = &mac_addr[0];
        src_ipv6 = &ipv6[1];
        dst_ipv6 = &ipv6[0];
    }  

    return rv;
}

/*
 *Indicate whether the device is arad_plus.
 */
int is_arad_plus(int unit, int *yesno)
{
  bcm_info_t info;

  int rv = bcm_info_get(unit, &info);
  if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
  }

  *yesno = (((info.device == 0x8660) || (info.device == 0x8670)) ? 1 : 0);

  return rv;
}

/* 
 *creating l3 interface - ingress/egress
 */
int bfd_intf_init(int unit){
    int rv;
    int flags = 0;    
    int open_vlan=1;
    int vrf = 0;    
    int fec = 0;      
    int encap_id = 0;     
    int ing_intf = 0;    
    int egr_intf = 0;

    /*** create ingress router interface ***/
    rv = create_l3_intf(unit, flags, open_vlan, bfd_port, bfd_vlan, vrf, *my_mac, &ing_intf); 
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress interface in unit %d \n", unit);
    }

    egr_intf = ing_intf;

    /*** Create egress object1 ***/
    rv = create_l3_egress(unit, flags, bfd_port, bfd_vlan, egr_intf, *nhp_mac, &bfd_fec, &encap_id); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, sysport=%d, in unit %d\n", sysport, unit);
    }
    
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", bfd_fec, unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id, unit);
    }
    
    return rv;
}


/*
 *Initialize the BFD IPv6 application.
 */
int bfd_ipv6_example_init(int unit) {
    int rv;

    rv = bfd_intf_init(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    if(verbose >= 1) {
        printf("bcm_bfd_init\n");
    }
    rv = bcm_bfd_init(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

/*
 *Trap the BFD IPv6.
 *  - ARAD hardware don't support the BFD IPv6.
 *  - Trap the BFD IPv6 single-hop packet by UDP destination port(3784) and IP ttl(255) to uKernel.
 */
int bfd_ipv6_trap(int unit) {
    
    int trap_id=0;
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t trap_config;
    bcm_field_qset_t qset;
    bcm_field_aset_t aset;
    bcm_field_entry_t ent;
    bcm_field_group_t grp;
    bcm_gport_t gport;
    
    
    /* create trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    /* configure trap attribute tot update destination */
    sal_memset(&trap_config, 0, sizeof(trap_config));
    trap_config.flags = (BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP | BCM_RX_TRAP_REPLACE);
    trap_config.trap_strength = 0;
    trap_config.dest_port = bfd_dsp_port;
    
    rv = bcm_rx_trap_set(unit, trap_id, &trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }    
    
    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIpProtocol);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyL4DstPort);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyIp6HopLimit);    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionTrap);
    
    rv = bcm_field_group_create(unit, qset, 2, &grp);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  
    
    rv = bcm_field_group_action_set(unit, grp, aset);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  
    
    BCM_GPORT_TRAP_SET(gport, trap_id, 7, 0);
    
    rv = bcm_field_entry_create(unit, grp, &ent); 
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  

    /* IP protocal: 17(UDP) */
    rv = bcm_field_qualify_IpProtocol(unit, ent, 0x11, 0xff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* UDP destionation port: 3784(1-hop) */
    rv = bcm_field_qualify_L4DstPort(unit, ent, 3784, 0xffff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    } 
    /* IP TTL: 255(1-hop) */
    rv = bcm_field_qualify_Ip6HopLimit(unit, ent, 255, 0xff);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  
    
    rv = bcm_field_action_add(unit, ent, bcmFieldActionTrap, gport, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  
    
    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }  

    return rv;    
}


/*
 *BFD IPv6 test example.
 *  - Test the BFD IPv6 APIs whether works which include bcm_bfd_endpoint_create, 
 *     bcm_bfd_endpoint_get and bcm_bfd_endpoint_destroy.
 *  - Create the BFD IPv6 endpoint that will send the BFD IPv6 packet to remote BFD IPv6 
 *     device and receive BFD IPv6 packet from remote BFD IPv6 device.
 */
int bfd_ipv6_example(int unit, int endpoint_id) {
    int rv;
    bcm_bfd_endpoint_info_t bfd_endpoint_info = {0};
    bcm_bfd_endpoint_info_t bfd_endpoint_test_info = {0};
    int ret;
    int is_aradplus;

    rv = is_arad_plus(unit, &is_aradplus);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (!is_aradplus) {
        printf("Error BFD IPv6 only support on arad+.\n");
        return BCM_E_INTERNAL;
    }    

    rv = bfd_ipv6_example_init(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("bfd endport create \n");
    }

    /* Adding BFDoIPV6 one hop endpoint */
    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_info);

    bfd_endpoint_info.flags = BCM_BFD_ENDPOINT_WITH_ID | BCM_BFD_ENDPOINT_IPV6;
    bfd_endpoint_info.id = endpoint_id;
    bfd_endpoint_info.type = bcmBFDTunnelTypeUdp;
    bfd_endpoint_info.ip_ttl = 255;
    bfd_endpoint_info.ip_tos = 0;
    sal_memcpy(&(bfd_endpoint_info.src_ip6_addr), src_ipv6, 16);
	sal_memcpy(&(bfd_endpoint_info.dst_ip6_addr), dst_ipv6, 16);
    bfd_endpoint_info.udp_src_port = 50000;
    bfd_endpoint_info.auth_index = 0;
    bfd_endpoint_info.auth = bcmBFDAuthTypeNone;
    bfd_endpoint_info.local_discr = *local_disc;
    bfd_endpoint_info.local_min_tx = 100000;
    bfd_endpoint_info.local_min_rx = 100000;
    bfd_endpoint_info.local_detect_mult = 3;
	bfd_endpoint_info.remote_discr = *remote_disc;
    bfd_endpoint_info.egress_if = bfd_fec;

    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
	
	if (verbose >= 1) {
        printf("bfd endport id:0x%x\n", bfd_endpoint_info.id);
    }

    if (verbose >= 1) {
        printf("bfd endport get \n");
    }

    bcm_bfd_endpoint_info_t_init(&bfd_endpoint_test_info);
    rv = bcm_bfd_endpoint_get(unit, bfd_endpoint_info.id, &bfd_endpoint_test_info);
    if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
    }

    if (verbose >= 1) {
        printf("bfd endport compare \n");
    }

    /* Here mep_not_acc_info and mep_not_acc_test_info are compared */
    ret = cmp_structs(&bfd_endpoint_info, &bfd_endpoint_test_info,bcmBFDTunnelTypeUdp ) ;
    if (ret != 0) {
        printf("%d different fields in BFDoIPV4\n", ret);
    }

    if (verbose >= 1) {
        printf("bfd endport destroy \n");
    }

    rv = bcm_bfd_endpoint_destroy(unit, bfd_endpoint_info.id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("bfd endport re-create \n");
    }

    rv = bcm_bfd_endpoint_create(unit, &bfd_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if (verbose >= 1) {
        printf("bfd trap \n");
    }

    rv = bfd_ipv6_trap(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

