
/* Configure vxlan port */
struct vxlan_port_add_s {
    /* INPUT */
    uint32      vpn;        /* vpn to which we add the vxlan port  */
    bcm_gport_t in_port;    /* network side port for learning info. */
    bcm_gport_t in_tunnel;  /* ip tunnel terminator gport */
    bcm_gport_t out_tunnel; /* ip tunnel initiator gport */
    bcm_if_t    egress_if;  /* FEC entry, contains ip tunnel encapsulation information */
    uint32      flags;      /* vxlan flags */
    int         set_egress_orientation_using_vxlan_port_add; /* egress orientation can be configured: 
                                                                - using the api bcm_vxlan_port_add, 
                                                                   when egress_tunnel_id (ip tunnel initiator gport, contains outlif) is valid (optional).  
                                                                - using the api bcm_port_class_set, update egress orientation. 
                                                                By default: enabled */
    uint8       add_vxlan_port_to_vsi_flooding_mc_group; /* indicate we'll add the vxlan port to vsi flooding MC group. For ROO, it's not required. */
    int         network_group_id; /* forwarding group for vxlan port. Used for orientation filter */

    /* OUTPUT */
    bcm_gport_t vxlan_port_id;  /* returned vxlan gport */
};

int vxlan_port_s_clear(vxlan_port_add_s* vxlan_port_add) {

    sal_memset(vxlan_port_add, 0, sizeof(*vxlan_port_add));

    /* by default, egress orientatino is set by vxlan port */
    vxlan_port_add->set_egress_orientation_using_vxlan_port_add = 1; 
    /* by default add the vxlan port the the flooding group */
    vxlan_port_add->add_vxlan_port_to_vsi_flooding_mc_group = 1; 
}


int vxlan__vxlan_port_add(int unit, vxlan_port_add_s *vxlan_port_add) {
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t vxlan_port;

    bcm_vxlan_port_t_init(&vxlan_port);

    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = vxlan_port_add->in_port;
    vxlan_port.match_tunnel_id = vxlan_port_add->in_tunnel;
    vxlan_port.flags = vxlan_port_add->flags;
    if(vxlan_port_add->egress_if != 0) {
        vxlan_port.egress_if = vxlan_port_add->egress_if;
        vxlan_port.flags |= BCM_VXLAN_PORT_EGRESS_TUNNEL;
    }
    if (vxlan_port_add->out_tunnel != 0){
        vxlan_port.egress_tunnel_id = vxlan_port_add->out_tunnel;
    }

    /* set orientation, work for Jericho.
     * Until arad+, we use bounce back filter to prevent packets from DC core to go back to DC core.
     * For Jericho, we configure orientation at ingress (inLif) and egress (outLif): 
     * ingress orientation is configured at bcm_vxlan_port_add. 
     * egress orientation can be configured using bcm_vxlan_port_add or at bcm_port_class_set
     */
    if (is_device_or_above(unit, JERICHO)) {

        /* flag indicating network orientation.
           Used to configure ingress orientation and optionally egress orientation */
        vxlan_port.flags |= BCM_VXLAN_PORT_NETWORK; 
        vxlan_port.network_group_id = vxlan_port_add->network_group_id; /* TO FIX g_vxlan.vxlan_network_group_id;  */
        /* if no outlif is provided, can't configure egress orientation */
        if (vxlan_port_add->out_tunnel == 0){ 
            printf("can't configure outlif orientation, since tunnel initiator gport isn't provided \n"); 
        } else {
            /* set network orientation at the egress using bcm_port_class_set */
            if (!vxlan_port_add->set_egress_orientation_using_vxlan_port_add) {  /* TO FIX g_vxlan.set_egress_orientation_using_vxlan_port_add */
                rv = bcm_port_class_set(unit, vxlan_port_add->out_tunnel, bcmPortClassForwardEgress, vxlan_port_add->network_group_id);   
                if (rv != BCM_E_NONE) {
                    printf("Error, in bcm_port_class_set \n");
                    return rv;
                }
                printf("egress orientation using bcm_port_class_set \n");

                /* since egress orientation is already updated, no need to pass outlif to bcm_vxlan_port_add to configure outlif orientation.
                 * Still need to pass outlif to vxlan_port_add if need to configure learning using outlif.
                 * If ip tunnel fec is provided, ip tunnel fec will be used for learning, so no need for outlif
                 */
                if (vxlan_port_add->egress_if != 0) {
                    vxlan_port.egress_tunnel_id = 0;
                }
            }
        }
    }  
    rv = bcm_vxlan_port_add(unit,vxlan_port_add->vpn,&vxlan_port);
    if(rv != BCM_E_NONE) {
        printf("error bcm_vxlan_port_add \n");
        return rv;
    }

    vxlan_port_add->vxlan_port_id = vxlan_port.vxlan_port_id;


    if(verbose >= 2){
        printf("vxlan port created, vxlan_port_id: 0x%08x \n", vxlan_port.vxlan_port_id); 
        printf("vxlan port is config with:\n"); 
        printf("  ip tunnel termination gport:0x%08x  \n", vxlan_port_add->in_tunnel);
        if(vxlan_port_add->egress_if !=0 ) {
            printf("  FEC entry (contains ip tunnel encapsulation information):0x%08x \n",vxlan_port_add->egress_if);
        }
        else
        {
            printf("  ip tunnel initiator gport:0x%08x\n",vxlan_port_add->out_tunnel);
        }
        printf("  vpn: 0x%08x\n\r",vxlan_port_add->vpn);
        printf("  vxlan lif orientation: network_group_id = %d\n\r", vxlan_port_add->network_group_id);
    }

    /* add vxlan port to the VPN flooding MC group */
    if (vxlan_port_add->add_vxlan_port_to_vsi_flooding_mc_group) {
        rv = multicast__vxlan_port_add(unit, vxlan_port_add->vpn, vxlan_port.match_port, vxlan_port.vxlan_port_id, 1 /* is ingress*/);
        if (rv != BCM_E_NONE) {
            printf("Error, multicast_vxlan_port_add\n");
            printf("If multicast_vxlan_port_add fail, it may be because vxlan port is configured with ECMP instead of FEC. \n"); 
            printf("So continue, but note that vxlan port is NOT added to MC group \n");
            /* reset the error code */
            rv = BCM_E_NONE; 
        }
        if(verbose >= 2){
            printf("add vxlan-port 0x%08x to VPN flooding group %d \n\r",vxlan_port.vxlan_port_id, vxlan_port_add->vpn);
        }
    }
    
    return rv;
}



