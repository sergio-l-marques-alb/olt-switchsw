/* $Id: cint_sand_utils_multicast.c,
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * Multicast utility functions
 */

int egress_mc = 0;

/* Adding entries to MC group
 * ipmc_index:  mc group
 * ports: array of ports to add to the mc group
 * cud 
 * nof_mc_entries: number of entries to add to the mc group
 * is_ingress:  if true, add ingress mc entry, otherwise, add egress mc 
 * 
   */
int multicast__add_multicast_entry(int unit, int ipmc_index, int *ports, int *cud, int nof_mc_entries, int is_egress) {
    int i;
    uint32 flags;
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t rep_array;

    bcm_multicast_replication_t_init(&rep_array);

    for (i=0;i<nof_mc_entries;i++) {
        if (is_device_or_above(unit, JERICHO2)) {
            flags = is_egress ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP; 
            rep_array.port = BCM_GPORT_IS_MODPORT(ports[i]) ? BCM_GPORT_MODPORT_PORT_GET(ports[i]) : ports[i];
            rep_array.encap1= cud[i];

            rv = bcm_multicast_add(unit,ipmc_index,flags,1,&rep_array);
            if (rv != BCM_E_NONE) {
                printf("Error, = : port %d encap_id: %d \n", ports[i], cud[i]);
                return rv;
            }
        }
        else {
            if (is_egress) {
                rv = bcm_multicast_egress_add(unit,ipmc_index,ports[i],cud[i]);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_multicast_egress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                    return rv;
                }
            } 
            else {
                rv = bcm_multicast_ingress_add(unit,ipmc_index,ports[i],cud[i]);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_multicast_ingress_add: port %d encap_id: %d \n", ports[i], cud[i]);
                    return rv;
                }
            }
        }
    }

    return rv;
}

int multicast__open_mc_group(int unit, int *mc_group_id, int extra_flags) {
    int rv = BCM_E_NONE;
    int flags;
    
    /* destroy before open, to ensure it not exist */
    rv = bcm_multicast_destroy(unit, *mc_group_id);

    printf("egress_mc: %d \n", egress_mc);
    
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

int multicast__populate_destid_array(int unit, bcm_port_t port, bcm_module_t *destids, uint32 *destid_count) {
    bcm_error_t rv = BCM_E_NONE;
    uint32 dummy_flags, found = 0;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 core = 0;

    /*
     * For Jericho 2 API bcm_fabric_multicast_set with an array of the cores on which we have replications.
     * In order to do that, check the core of each destination and populate the destids array.
     */
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_E_NONE != rv) {
        printf("bcm_port_get failed \n");
        return rv;
    }

    /*
     * Check if core already exists in array
     */
    for (core = 0; core < *dnxc_data_get(unit, "device", "general", "nof_cores", NULL); core++)
    {
        if(destids[core] == mapping_info.core)
        {
            found = TRUE;
            break;
        }
    }

    /*
     * If the core does not already exists in the array, add it and
     * increment the destid_count.
     */
    if (!found){
        destids[*destid_count] = mapping_info.core;
        *destid_count += 1;
    }
    return rv;
}

int multicast__open_egress_mc_group_with_local_ports(int unit, int mc_group_id, int *dest_local_port_id, int *cud, int num_of_ports, int extra_flags) {
    int i;
    bcm_error_t rv = BCM_E_NONE;
    bcm_cosq_gport_info_t gport_info;
    bcm_cosq_gport_type_t gport_type = bcmCosqGportTypeLocalPort;
    uint32 destid_count;
    int nof_destids = is_device_or_above(unit, JERICHO2) ? *dnxc_data_get(unit, "device", "general", "nof_cores", NULL) : 1;
    bcm_module_t destids[nof_destids];

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

        if (is_device_or_above(unit, JERICHO2)) {
            sal_memset(destids, 0xFF, sizeof(destids));
            /*
             * Populate the destids array with only the needed destination IDs
             */
            rv = multicast__populate_destid_array(unit, dest_local_port_id[i], destids, &destid_count);
            if (rv != BCM_E_NONE) {
                printf("Error, multicast__populate_destid_array\n");
                return rv;
            }
        }
    }

    if (is_device_or_above(unit, JERICHO2)) {
        rv = bcm_fabric_multicast_set(unit, mc_group_id, 0, destid_count, destids);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set, mc_group_id $mc_group_id \n");
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


/*
 * add gport to the multicast
 */
int multicast__gport_encap_add(int unit, int mc_group_id, int sys_port, int gport, uint8 is_egress)
{
    int encap_id;
    int rv = BCM_E_NONE;

    /* update Multicast to have the added port  */
    if (BCM_GPORT_IS_SET(gport)) {
        if (BCM_GPORT_IS_VLAN_PORT(gport)) {
            rv = bcm_multicast_vlan_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_MPLS_PORT(gport)) {
            rv = bcm_multicast_vpls_encap_get(unit, mc_group_id, sys_port, gport, &encap_id);
        } else if (BCM_GPORT_IS_TUNNEL(gport)) {
            encap_id = BCM_GPORT_TUNNEL_ID_GET(gport);
        } else {
            rv = BCM_E_UNAVAIL;
        }
        if (rv != BCM_E_NONE) {
            printf("Error, in multicast__gport_encap_add mc_group_id:  0x%08x  phy_port:  0x%08x  gport:  0x%08x \n", mc_group_id, sys_port, gport);
            return rv;
        }

    } else {
        encap_id = BCM_IF_INVALID;
    }

    rv = multicast__add_multicast_entry(unit, mc_group_id, &sys_port, &encap_id, 1, is_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, in multicast__add_multicast_entry mc_group_id:  0x%08x  phy_port:  0x%08x  encap:  0x%08x \n", mc_group_id, sys_port, encap_id);
        return rv;
    }
    
    return rv;
}


struct multicast_forwarding_entry_dip_sip_s {
    bcm_ip_t mc_ip;
    bcm_ip_t src_ip;
    bcm_ip6_t mc_ip6;
    bcm_ip6_t src_ip6;
    uint8 use_ipv6;
};

/* Creates a forwarding entry. src_ip == 0x0 implies L2 forwarding. The entry will be accessed by LEM<FID,DIP> Lookup.
 * The entry determines information relevant for MC L2 forwarding given a (MC) destination ip.
 * src_ip > 0x0 implies L3 forwarding. The entry will be accessed by TCAM <RIF, SIP, DIP> Lookup.
 * The entry determines information relevant for MC L3 forwarding given a (MC) destination ip.                                                                                                                                 .
 * src_ip == -1 implies creation of entry without sip.                                                                                                                                                                                                                            .
 */
int multicast__create_forwarding_entry_dip_sip(int unit, multicast_forwarding_entry_dip_sip_s *entry, int ipmc_index, int vlan, int vrf) {
    int rv = BCM_E_NONE;
    bcm_ipmc_addr_t data;
    bcm_ip6_t ip6_full_mask  = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

    /* Configurations relevant for LEM<FID,DIP> lookup*/
    bcm_ipmc_addr_t_init(&data);


    data.flags = 0x0;
    data.group = ipmc_index;
/*  data.vrf = vrf;*/

    if (entry->use_ipv6) {
        data.flags |= BCM_IPMC_IP6;
        sal_memcpy(data.mc_ip6_addr, entry->mc_ip6, sizeof(entry->mc_ip6));
        sal_memcpy(data.mc_ip6_mask, ip6_full_mask, sizeof(entry->mc_ip6));
        
        if (entry->src_ip != -1 && entry->src_ip != 0) {
            data.vid = vlan;
            sal_memcpy(data.s_ip6_addr, entry->src_ip6, sizeof(data.s_ip6_addr));
            sal_memset(data.s_ip_mask, 0xff, sizeof(data.s_ip_mask));
        }
    } else {
        data.mc_ip_addr = entry->mc_ip;
        data.mc_ip_mask = 0xffffffff;
    
        if (entry->src_ip != -1 && entry->src_ip != 0) {
            data.vid = vlan;
            data.s_ip_addr = entry->src_ip;
            data.s_ip_mask = 0xffffffff;
        }
    }
  
    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    return rv;
}


/*
 * Multicast utility object -- end
 */
