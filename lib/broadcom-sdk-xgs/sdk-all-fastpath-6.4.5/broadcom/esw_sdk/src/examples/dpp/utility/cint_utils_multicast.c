/* $Id: cint_utils_multicast.c,v 1.10 Broadcom SDK $
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

int egress_mc = 0;

/* Adding entries to MC group
 * ipmc_index:  mc group
 * ports: array of ports to add to the mc group
 * nof_ports: number of entries to add to the mc group
 * encap_id: CUD. 
 * is_ingress:  if true, add ingress mc entry, otherwise, add egress mc 
 * see add_ingress_multicast_forwarding from cint_ipmc_flows.c
 * 
   */
int multicast__add_multicast_entry(int unit, int ipmc_index, int *ports, int *cud, int nof_ports, int is_egress) {
    int rv = BCM_E_NONE;
    int i;

    for (i=0;i<nof_ports;i++) {
        /* egress MC */
        if (is_egress) {
            rv = bcm_multicast_egress_add(unit,ipmc_index,ports[i],cud[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_egress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                return rv;
            }
        } 
        /* ingress MC */
        else {
			rv = bcm_multicast_ingress_add(unit,ipmc_index,ports[i],cud[i]);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_multicast_ingress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                return rv;
            }
        }
    }

    return rv;
}

/*
 * add gport of type mpls-port to the multicast
 */
int multicast__mpls_port_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress) {
    int encap_id;
    int rv;
    
    rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vpls_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    return rv;
}

/*
 * add gport of type vlan-port to the multicast
 */
int multicast__vlan_port_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress) {
    int encap_id;
    int rv;
    
    rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_vlan_encap_get mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
	rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
        return rv;
    }
    
    return rv;
}

int multicast__open_mc_group(int unit, int *mc_group_id, int extra_flags) {
    int rv = BCM_E_NONE;
    int flags;
    
    /* destroy before open, to ensure it not exist */
    rv = bcm_multicast_destroy(unit, *mc_group_id);
    
	flags = BCM_MULTICAST_WITH_ID | extra_flags;
    /* create ingress/egress MC */
    if (egress_mc) {
        flags |= BCM_MULTICAST_EGRESS_GROUP;
    } else {
        flags |= BCM_MULTICAST_INGRESS_GROUP;
    }

    rv = bcm_multicast_create(unit, flags, mc_group_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create, flags $flags mc_group_id $mc_group_id \n");
        return rv;
    }

    printf("Created mc_group %d \n", *mc_group_id);

    return rv;
}

int multicast__open_ingress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags) {
	int i;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;

	egress_mc = 0;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    for(i=0;i<num_of_ports;i++) {
        BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id[i]); 
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get, gport_type $gport_type \n");
            return rv;
        }

        rv = multicast__add_multicast_entry(unit, mc_group_id, &gport_info.out_gport, &cud[i], 1, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id dest_gport $gport_info.out_gport \n");
            return rv;
        }
    }
    
    return rv;
}

int multicast__open_ingress_mc_group_with_gports(int unit, int mc_group_id, int *gport, int *cud, int num_of_ports, int extra_flags) {
    bcm_error_t rv = BCM_E_NONE;

	egress_mc = 0;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, gport, cud, num_of_ports, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id \n");
        return rv;
    }
    
    return rv;
}

int multicast__open_egress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags) {
	int i;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;

	egress_mc = 1;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    for(i=0;i<num_of_ports;i++) {
        BCM_GPORT_LOCAL_SET(gport_info.in_gport,dest_local_port_id[i]); 
        rv = bcm_cosq_gport_handle_get(unit,gport_type,&gport_info);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_handle_get, gport_type $gport_type \n");
            return rv;
        }

        rv = multicast__add_multicast_entry(unit, mc_group_id, &gport_info.out_gport, &cud[i], 1, egress_mc);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id dest_gport $gport_info.out_gport \n");
            return rv;
        }
    }
    
    return rv;
}

int multicast__open_egress_mc_group_with_gports(int unit, int mc_group_id, int *gport, int *cud, int num_of_ports, int extra_flags) {
    bcm_error_t rv = BCM_E_NONE;

	egress_mc = 1;

    rv = multicast__open_mc_group(unit, &mc_group_id, extra_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__open_mc_group, extra_flags $extra_flags mc_group $mc_group_id \n");
        return rv;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, gport, cud, num_of_ports, egress_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__add_multicast_entry, mc_group_id $mc_group_id \n");
        return rv;
    }
    
    return rv;
}


/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup                                   .
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.                                                                                                                                                                                                                            .
 */
int multicast__create_forwarding_entry_dip_sip(int unit, bcm_ip_t mc_ip, bcm_ip_t src_ip, int ipmc_index, int vlan, int vrf) {
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);
    data.mc_ip_addr = mc_ip;

    if (src_ip != -1) {
        data.s_ip_addr = src_ip;
		data.s_ip_mask = 0xffffffff;
    }
  
    data.vid = vlan;
    data.flags = 0x0;
    data.group = ipmc_index;
/*  data.vrf = vrf;*/
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, create_forwarding_entry_dip_sip \n");
        return rv;
    }

    return rv;
}
