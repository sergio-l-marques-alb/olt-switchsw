/*
 *
 * $Id: cint_vpls_spoke_mpls_el_eli_ctlwd.c,v $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 * 
 * File: cint_vpls_spoke_mpls_el_eli_ctlwd.c
 *
 * Purpose: example to configure application which insert El/ELI at no-BOS postion with CW on PWE. 
 *
 * Usage:
 * 
 *
        cint examples/dpp/utility/cint_utils_global.c
        cint examples/dpp/utility/cint_utils_mpls.c
        cint examples/dpp/utility/cint_utils_mpls_port.c
        cint examples/dpp/utility/cint_utils_multicast.c
        cint examples/dpp/cint_qos.c
        cint examples/dpp/utility/cint_utils_l2.c
        cint examples/dpp/utility/cint_utils_l3.c
        cint examples/dpp/cint_queue_tests.c
        cint examples/dpp/cint_multi_device_utils.c
        cint examples/dpp/cint_mpls_lsr.c
        cint examples/dpp/cint_mpls_tunnel_initiator.c
        cint examples/dpp/cint_vswitch_cross_connect_p2p.c
        cint examples/dpp/cint_port_tpid.c
        cint examples/dpp/cint_advanced_vlan_translation_mode.c
        cint examples/dpp/cint_ip_route.c
        cint examples/dpp/cint_ipv4_fap.c
        cint examples/dpp/internal/cint_vpls_spoke_mpls_el_eli_ctlwd.c
        c
        ac_port=15;
        pwe_port=200;
        print example_el_eli_cw();
*/
egress_mc = 1;
int       two_label_term_in_db6 = 0;
int       DUNE_NULL_ENCAP_ID = -1;
bcm_if_t  spokesdp_fec = 0;
int       tunnel_eedb_lif_id = 0;
int       tunnel_fec = 0;
int       enableSameInterfaceFilter = 1;
int       pwe_port = 13;
int       network_vlan_id = 30;
int       ac_port = 15;
int       ac_vid = 10;
int       unit = 0;
int       egr_vc_lbl=3000;
int       tun_lbl_1=1000;
int       use_spoke_fec = 0;
int       spoke_fdb_point_to_tunnel = 0;
int       option = 1;
int       l3_eg_map_id = 0;
int       is_php = 0;
int       is_ctl_enable = 1;

int ing_vc_lbl=3000;  /* Incoming PWE */
int term_label1=1000; /* Incoming Outer Transport */
int term_label2=2000; /* Incoming Inner Transport */
int err;
int verbose1 = 1;

/*set egress exp mapping*/
int create_egress_exp_map(int sw_map_id)
{
    bcm_qos_map_t l3_eg_map;
    int flags = 0;
    int result = BCM_E_NONE;
    int fc = 0, dp;

    l3_eg_map_id = sw_map_id;

    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_WITH_ID;
    result = bcm_qos_map_create(unit, flags, &l3_eg_map_id);

    bcm_qos_map_t_init(&l3_eg_map);

    for (fc=0; fc<8; fc++) {
        for (dp = 0; dp < 2; dp++) {
            l3_eg_map.color = dp;
			l3_eg_map.remark_int_pri = fc;
			l3_eg_map.exp = ((dp==0) ? 2 : 3);
			result = bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP, &l3_eg_map, l3_eg_map_id);
			if (result != BCM_E_NONE) {
				printf("Egress Qos map ADD ERRORed. (%s)", bcm_errmsg(result));
				return false;
			}
			result = bcm_qos_map_add(unit, BCM_QOS_MAP_L3 | BCM_QOS_MAP_ENCAP, &l3_eg_map, l3_eg_map_id);
			if (result != BCM_E_NONE) {
				printf("Egress Qos map ADD ERRORed. (%s)", bcm_errmsg(result));
				return false;
			}
		}
	}
	printf("Egress Mpls Map created Unit %d Hw_id 0x%x\n", unit, l3_eg_map_id);
}

/*Disable Trap*/
int duneDisablePortTrap(int unit, bcm_rx_trap_t trap)
{
    int rv;
    int trap_id;
    bcm_rx_trap_config_t config;

    bcm_rx_trap_config_t_init(&config);
    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.trap_strength = 0;

    rv = bcm_rx_trap_type_create(unit, 0, trap, &trap_id);
    BCM_IF_ERROR_RETURN(rv);

    bcm_rx_trap_set(unit, trap_id, &config);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

int add_egress_multicast_member(
    bcm_gport_t      gport,
    bcm_if_t         encap_id1,
    bcm_if_t         encap_id2,
    int              vpn
)
{
    int SINGLE_REPLICATION = 1;
    int    rv;
    uint32 flags = 0;
    bcm_multicast_replication_t reps[SINGLE_REPLICATION];

    sal_memset(reps, 0, sizeof(reps));

    bcm_multicast_replication_t_init(reps);

    reps[0].port   = gport;
    reps[0].encap1 = encap_id1;

    if (encap_id2 != DUNE_NULL_ENCAP_ID)
    {
        reps[0].encap2 = encap_id2;
        reps[0].flags  = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;
    }

    rv = bcm_multicast_add(unit, vpn, flags, SINGLE_REPLICATION, reps);

    if (rv != BCM_E_NONE)
    {
        printf ("Error (%s) during bcm_multicast_add for gport 0x%x, encap_id1 0x%x/encap_id2 0x%x, mcGrp 0x%x!\n",
                     bcm_errmsg(rv),
                     gport,
                     encap_id1,
                     encap_id2,
                     vpn);
        return -1;
    }

    printf ("bcm_multicast_add for gport 0x%x, encap_id1 0x%x/encap_id2 0x%x, mcGrp 0x%x is successful!\n",
                     gport,
                     encap_id1,
                     encap_id2,
                     vpn);
    return 0;
}

/*Create FEC with encap ID*/
void program_ingress_spokesdp_fec(int encap_id)
{
    int              MPLS_INVALID_FEC  = -1;
    int              bcm_error = BCM_E_NONE;
    bcm_l3_egress_t  l3eg;
    unsigned int     flags = (BCM_L3_INGRESS_ONLY);

    /* initialize the l3 egress structure */
    bcm_l3_egress_t_init(&l3eg);

    BCM_GPORT_FORWARD_PORT_SET(l3eg.port, tunnel_fec);

    BCM_L3_ITF_SET(l3eg.intf, BCM_L3_ITF_TYPE_LIF, encap_id);
    l3eg.encap_id = 0;
    l3eg.flags    = 0;

    bcm_error = bcm_l3_egress_create(unit, flags, &l3eg, &spokesdp_fec);
    if (bcm_error != BCM_E_NONE)
       {
           printf("FAIL: L3 egress create for tunnel_fec 0x%x, encap_id 0x%x: %s\n",
                     tunnel_fec, encap_id, bcm_errmsg(bcm_error));
           return MPLS_INVALID_FEC;
       }

    printf("program_ingress_spokesdp_fec: spokesdp_fec 0x%x, tunnel_fec 0x%x, encap_id 0x%x\n",
            spokesdp_fec, tunnel_fec, encap_id);

}

/*create tunnel and rif*/
void create_l3_intf_rif_and_tunnel(void)
{
    int rv = 0;
    int flags = 0;

    bcm_pbmp_t pbmp, ubmp;
    bcm_mac_t my_mac_addr = {0, 0, 0, 0, 0, 0x31};
    bcm_mac_t primary_nh_mac = {0, 0, 0, 0, 0, 0x55};
    int       mpls_vlan_id = 100;


    /* Create the VLAN */
    BCM_PBMP_CLEAR(ubmp);
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, pwe_port);
    print bcm_vlan_destroy(unit, mpls_vlan_id);
    print bcm_vlan_create(unit, mpls_vlan_id);
    print bcm_vlan_port_add(unit, mpls_vlan_id, pbmp, ubmp);

    /* Configure the VSI My-MAC */
    /* Used for L2 termination (ingress) and L2 encapsulation (egress) */
    bcm_l3_intf_t l3_ingress;
    bcm_l3_intf_t_init(&l3_ingress);
    l3_ingress.l3a_ttl = 31;
    l3_ingress.l3a_mtu = 1524;
    l3_ingress.l3a_vid = mpls_vlan_id;
    sal_memcpy(l3_ingress.l3a_mac_addr, my_mac_addr, 6);
    print bcm_l3_intf_create(unit, l3_ingress);

    bcm_l3_intf_t l3_egress;
    bcm_l3_intf_t_init(&l3_egress);
    l3_egress.l3a_ttl = 31;
    l3_egress.l3a_mtu = 1524;
    l3_egress.l3a_vid = network_vlan_id;

    sal_memcpy(l3_egress.l3a_mac_addr, my_mac_addr, 6);

    print bcm_l3_intf_create(unit, l3_egress);

    /**************************************************************/
    /* Allocates the MPLS PUSH EEDB entry*/
    bcm_mpls_egress_label_t push_label_array[2];

    bcm_mpls_egress_label_t_init(&push_label_array[0]);
    push_label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);
    push_label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    push_label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
    push_label_array[0].flags |= (BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE | BCM_MPLS_EGRESS_LABEL_ENTROPY_INDICATION_ENABLE);
    if(is_php)
    {
        push_label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP; /*REAL push*/
    }
    else
    {
        push_label_array[0].action = BCM_MPLS_EGRESS_ACTION_PUSH; /* REAL push*/
    }
    push_label_array[0].label = tun_lbl_1;
    push_label_array[0].l3_intf_id =  l3_egress.l3a_intf_id;  /* Point to RIF */


    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, push_label_array);
    if(rv) print "ERROR ..................";

	print push_label_array;
    tunnel_eedb_lif_id = push_label_array[0].tunnel_id;

    /* Allocates the FEC entry pointing to tunnel entry */
    bcm_if_t primary_l3egid;
	bcm_l3_egress_t primary_l3eg;
    bcm_l3_egress_t_init(&primary_l3eg);
    sal_memcpy(primary_l3eg.mac_addr, primary_nh_mac, 6);
    if(is_php)
    {
       primary_l3eg.vlan    = l3_egress.l3a_intf_id;
       primary_l3eg.intf 	= 0;
    }
    else
    {
        primary_l3eg.vlan   = 0;
        primary_l3eg.intf 	= push_label_array[0].tunnel_id;
    }
    primary_l3eg.port   = pwe_port;
    primary_l3eg.flags = BCM_L3_CASCADED | BCM_L3_ENCAP_SPACE_OPTIMIZED;
    flags = 0;

    rv = bcm_l3_egress_create(unit, flags, &primary_l3eg, &primary_l3egid);
    if(rv) print "ERROR ..................";
    tunnel_fec = primary_l3egid;
    printf("tunnel fec 0x%x has been created with tunnel id 0x%x!\n", tunnel_fec, tunnel_eedb_lif_id);
}

int cint_vpls_sap_spoke(int ing_vc_lbl, int egr_vc_lbl, int tun_lbl_1, int term_label1, int term_label2, int ac_port, int pwe_port)
 {
    int my_mac_lsb = 0x11;  /* set MAC to 00:00:00:00:00:11 */
    int next_hop_lsb = 0x22; /* set MAC to 00:00:00:00:00:22 */
    int rv;
    int vpn = 6202;
    int unit = 0;
    int flags = 0;
    int nof_outer_tpids;
    int nof_inner_tpids;
    uint16 router_vsi_idx = 0;
    bcm_mac_t                 sap_fdb_mac = {0x00,0x00,0x00,0x00,0x00,0xa0};
    bcm_mac_t                 spoke_fdb_mac = {0x00,0x00,0x00,0x00,0x00,0xb0};
    bcm_l2_addr_t             l2_addr;
     bcm_vlan_port_t vlan_port_1;

   int mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);

   bcm_multicast_t mc_group;

   uint8 mac_1[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
   uint8 mac_2[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

   mac_1[5] = my_mac_lsb;
   mac_2[5] = next_hop_lsb;

    port_tpid_init(pwe_port,1,0);
    rv = port_tpid_set(0);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    port_tpid_init(ac_port,1,1);
    rv = port_tpid_set(0);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set1\n");
        return rv;
    }

   rv = mpls_port__vswitch_vpls_vpn_create__set(unit, vpn);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__vswitch_vpls_vpn_create__set\n");
        return rv;
    }

   /*Create Ingress and egress vlans*/
   rv = mpls__mpls_pipe_mode_exp_set(unit);
   if (rv != BCM_E_NONE) {
       printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
       return rv;
   }

    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

	create_l3_intf_rif_and_tunnel();


  printf("Created tunnel initiator \n");

  /*Configure MPLS tunnel POP Entry for incoming traffic*/
  bcm_mpls_tunnel_switch_t entry;
  uint32 next_prtcl = 0; /*Unset MPLS*/

   bcm_mpls_tunnel_switch_t_init(&entry);
   entry.action = BCM_MPLS_SWITCH_ACTION_POP;

   entry.flags = 0;
   entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;

   if (is_device_or_above(unit,ARAD_PLUS)) {
       if (next_prtcl & BCM_MPLS_SWITCH_EXPECT_BOS) {
           entry.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
       }
   }
   else {
       entry.flags |= next_prtcl;
   }

   /* outer label */
   if (mpls_termination_label_index_enable) {
       BCM_MPLS_INDEXED_LABEL_SET(&entry.label, term_label1, 1);
   }else
       entry.label = term_label1;

   entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);

   entry.inlif_counting_profile = -1;
   rv = bcm_mpls_tunnel_switch_create(unit,&entry);
   if (rv != BCM_E_NONE) {
       printf("Error, in bcm_mpls_tunnel_switch_create for outer label\n");
       return rv;
   }

   /* Next terminate the inner label if present*/
    if (term_label2) {
        bcm_mpls_tunnel_switch_t_init(&entry);
        entry.action = BCM_MPLS_SWITCH_ACTION_POP;
        entry.flags = 0;
        entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
        if (is_device_or_above(unit,ARAD_PLUS)) {
            if (next_prtcl & BCM_MPLS_SWITCH_EXPECT_BOS) {
                entry.flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
            }
        }  else {
            entry.flags |= next_prtcl;
        }
        /* inner label */
        if (mpls_termination_label_index_enable) {
            printf("\n INDEXING ENABLED....\n");
            BCM_MPLS_INDEXED_LABEL_SET(&entry.label, term_label2, 1);
        }else
            entry.label = term_label2;
        entry.qos_map_id = qos_map_id_mpls_ingress_get(unit);
        entry.inlif_counting_profile = -1;
        rv = bcm_mpls_tunnel_switch_create(unit,&entry);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_mpls_tunnel_switch_create for inner label\n");
            return rv;
        }
    }
	bshell(0, "echo after tunnel termination create; diag dbal dbd 3");
    bshell(0, "echo after tunnel termination create; diag dbal dbd 4");

   printf("\n Tunnel Termination configured\n");
   /*Create the AC, i.e. SAP*/

   /* add port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port_1);

    /* set port attribures, key <port-vlan-vlan>*/
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_1.port = ac_port;
    vlan_port_1.match_vlan = ac_vid;
    vlan_port_1.match_inner_vlan = 0;
    vlan_port_1.flags = 0;
    vlan_port_1.ingress_network_group_id = 0;
    vlan_port_1.egress_network_group_id = 0;
    rv = bcm_vlan_port_create(unit, &vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }

     if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }


    rv = bcm_vswitch_port_add(unit, vpn, vlan_port_1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    rv = bcm_multicast_egress_add(unit, vpn, ac_port, vlan_port_1.encap_id);
    if (rv != BCM_E_NONE) {
       printf("Error (%s), bcm_multicast_egress_add: port %d encap_id to vpls %u: %d \n",
               bcm_errmsg(rv), ac_port, vlan_port_1.encap_id, vpn);
       return rv;
    }

    bcm_l2_addr_t_init(&l2_addr,sap_fdb_mac,vpn);

    l2_addr.flags = BCM_L2_STATIC; /* static entry */
    l2_addr.port = vlan_port_1.vlan_port_id;

    rv = bcm_l2_addr_add(unit,&l2_addr);

    if (rv != BCM_E_NONE) {
        printf("Error (%s), in bcm_l2_addr_add for sap in vpn %u\n",  bcm_errmsg(rv), vpn);
    } else {
	    printf("bcm_l2_addr_add is ok for sap fdb mac\n");
	}

    printf("\n AC configuration complete\n");
    /*Create the PWE*/

	int mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2);

	printf("Current MPLS termination database mode : %u\n", mpls_termination_label_index_database_mode);

    bcm_mpls_port_t mpls_port_1;

    /* add port, according to VC label */
    bcm_mpls_port_t_init(&mpls_port_1);

    /* Set parameters for both ingress and egress */
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(&mpls_port_1.match_label, ing_vc_lbl, 3);
    } else
        mpls_port_1.match_label =ing_vc_lbl;

    mpls_port_1.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port_1.network_group_id = 0;
    mpls_port_1.port = BCM_GPORT_INVALID;
    mpls_port_1.egress_label.label = egr_vc_lbl;
    mpls_port_1.egress_label.ttl = 10;
    mpls_port_1.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    mpls_port_1.egress_label.qos_map_id = l3_eg_map_id;


    mpls_port_1.failover_id =0;
    mpls_port_1.failover_port_id = 0;
    mpls_port_1.flags |= BCM_MPLS_PORT_COUNTED;
    mpls_port_1.flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE;
    mpls_port_1.flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
    mpls_port_1.flags |= BCM_MPLS_PORT_SERVICE_TAGGED;
    if(is_ctl_enable)
    {
        mpls_port_1.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    if(mpls_port_1.flags & BCM_MPLS_PORT_NETWORK) {
        if (is_device_or_above(unit,JERICHO) && soc_property_get(unit, "split_horizon_forwarding_groups_mode", 1)) {
            mpls_port_1.network_group_id = 1;
        }
    }

    mpls_port_1.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port_1.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port_1.mpls_port_id = 0x18803015;
    mpls_port_1.encap_id = 0x3015;

	if (use_spoke_fec)
	{
	    program_ingress_spokesdp_fec(mpls_port_1.encap_id);
        mpls_port_1.egress_tunnel_if = spokesdp_fec;
	} else
	{
        mpls_port_1.egress_tunnel_if = tunnel_fec;
	}

    rv = bcm_mpls_port_add(unit, vpn, &mpls_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }

    print " ------- Beg: mpls_port_1 ";
    print mpls_port_1;
    print " ------ End: mpls_port_1 ------ ";

    if (enableSameInterfaceFilter)
    {
        rv = bcm_port_control_set(unit, mpls_port_1.mpls_port_id, bcmPortControlBridge, 1);

        if (rv != BCM_E_NONE)
        {
            printf("Error %s - bcmPortControlBridge failed for mplsPortId 0x%x, Encap:0x%x",
                  bcm_errmsg(rv),
                  mpls_port_1.mpls_port_id,
                  mpls_port_1.encap_id);
        } else {
            printf("bcmPortControlBridge has been enabled!\n");
        }
    }


    printf("add port 0x%08x to vpn \n\r", mpls_port_1.mpls_port_id);
    printf("ENCAP ID: a%d \n\r", mpls_port_1.encap_id);

    int tm_port,modid,modport;
    /* update Multicast to have the added port  */
    rv = get_core_and_tm_port_from_port(unit, pwe_port, &modid, &tm_port);
    if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
    }
    BCM_GPORT_MODPORT_SET(modport, modid, tm_port);

    add_egress_multicast_member(modport, mpls_port_1.encap_id, BCM_L3_ITF_VAL_GET(tunnel_eedb_lif_id),vpn);


    bcm_l2_addr_t_init(&l2_addr,spoke_fdb_mac,vpn);

    l2_addr.flags    = BCM_L2_STATIC; /* static entry */

    bcm_l2_gport_forward_info_t forwardInfo;
    bcm_l2_gport_forward_info_t_init(&forwardInfo);
    bcm_l2_gport_forward_info_get(unit, mpls_port_1.mpls_port_id, &forwardInfo);

    if (spoke_fdb_point_to_tunnel)
    {
      l2_addr.port     = 0x98001000;
    } else {
        l2_addr.port     = forwardInfo.phy_gport;
    }
    l2_addr.encap_id = forwardInfo.encap_id;

    rv = bcm_l2_addr_add(unit,&l2_addr);

    if (rv != BCM_E_NONE) {
        printf("Error (%s), in bcm_l2_addr_add for spoke in vpn %u\n",  bcm_errmsg(rv), vpn);
    } else {
        printf("bcm_l2_addr_add is ok for spoke fdb mac (port 0x%x instead of 0x%x, encap_id 0x%x instead of 0x%x)\n",
               l2_addr.port, modport, l2_addr.encap_id, mpls_port_1.encap_id);
    }

    return rv;
 }


/* exapmle of appilcation */
int example_el_eli_cw(void)
{
    err = duneDisablePortTrap(0, bcmRxTrapSameInterface);
    printf("duneDisablePortTrap bcmRxTrapSameInterface, rc=0x%x \n", err);

    err = cint_vpls_sap_spoke(ing_vc_lbl, egr_vc_lbl, tun_lbl_1, term_label1, term_label2, ac_port,pwe_port);
    printf("cint_vpls_sap_spoke, rc=0x%x \n", err);

    bshell(0, "diag dbal dbd 3; diag dbal dbd 4");
    return 0;
}

