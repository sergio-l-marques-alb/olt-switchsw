/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Service using EEI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_sand_service_using_eei.c,v 1.0 2018/05/11 08:05:38 Jingli Guo $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File: cint_sand_service_using_eei.c
 * Purpose: Demo service in which EEI could be resolved at ingress.
 *
 * 
 *  1. PWE using EEI+FEC as learning data, EEI point to MPLS push command;
 *  2. PWE using EEI+FEC as learning data, EEI point to PWE EEDB;
 *  3. L3 routing resolving EEI+DEST, EEI point to ARP EEDB.
 *  4. L3 routing resolving EEI+DEST, EEI resolving push-profile+MPLS label.
 *  5. VPWS service, AC--PWE(EEI+Dest), TBD
 *
 *
 *
 * Test Scenario(1,2) 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dpp/utility/cint_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/dnx/cint_dnx_service_using_eei.c
 * cint ../../src/./examples/dpp/internal/systems/cint_utils_jr2-comp-jr1-mode.c
 * cint
 * initialize_required_system_tools(0);
 * service_using_eei_vpls_example(0,200,202);
 * exit;
 *
 *  1: pwe -> ac 
 * tx 1 psrc=202 data=0x000c0002000100000000cd1d884700d050400045714000110000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 202, Destination port: 200 
 *  Data: 0x0000011200010203040581000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x00010203040500110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 202 
 *  Data: 0x0000cd1d000c000200008100001e884700d0500000d05100000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  1: pwe -> ac 
 *  1 psrc=202 data=0x000c0002000100000000cd1d884700d05040008ae14000110000011200010203040681000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 202, Destination port: 200 
 *  Data: 0x0000011200010203040681000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 *  2: ac -> pwe 
 * tx 1 psrc=200 data=0x00010203040600110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 202 
 *  Data: 0x0000cd1d000c000200018100001e884700d050000115c10000010203040600110000011281000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000 
 *
 * Test Scenario(3)
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/./examples/dpp/utility/cint_utils_global.c
 * cint ../../src/./examples/dnx/field/cint_field_utils.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../src/./examples/sand/cint_ip_route_basic.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/./examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/./examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../src/./examples/sand/cint_sand_mpls_ingress_push.c
 * cint ../../src/./examples/sand/cint_sand_service_using_eei.c
 * cint ../../src/./examples/dpp/internal/systems/cint_utils_jr2-comp-jr1-mode.c
 * cint
 * initialize_required_system_tools(0);
 * service_using_eei_l3_route_example(0,200,202);
 * exit;
 *
 * 1: IP host lookup into EEI with MPLS Tunnel: payo127.255.255.14oE  ->  payo127.255.255.14o1000o400o401o402oE. 
 * tx 1 psrc=200 data=0x0000000000110000000000008100706408004500004c00000000403dfb61000000007fffff14000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 202 
 *  Data: 0x000000000022000000000011810000c8884700191014001901144500004c000000003f3dfc61000000007fffff14000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000 
 *
 *  2: IP host lookup into EEI as mpls push commnd: payo127.255.255.15oE  ->  payo127.255.255.15o5555oE 
 *  tx 1 psrc=200 data=0x0000000000110000000000008100706408004500004c00000000403dfb60000000007fffff15000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b00000000
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 202 
 *  Data: 0x00000022000000000011810000c88847015b313f4500004c000000003f3dfc60000000007fffff15000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b000000000000 
 */

 struct cint_service_using_eei_s
 {
    int vpls_pwe_port_ingress_label[2];  /*pwe incoming label*/
    int vpls_pwe_port_egress_label[2];   /*pwe  outgoing label*/
    int vpls_pwe_port_encap_id ;         /*pwe  encap_id*/

    bcm_ip_t host_ip[2];                    /*host ip for IP host fwd*/
    int      fec_with_arp_id;            /*FEC ID, storing ARP ID*/

    int mpls_label_using_push_cmd;       /*MPLS label encapsulated by MPLS push command*/
  };

 cint_service_using_eei_s cint_service_using_eei_info =
{
     
     /*
     * vpls_pwe_port_ingress_label
     */
     {1111,2222}, 
     

     /*
     * vpls_pwe_port_egress_label
     * PWE port 1: egress label is resolved from EEI mpls push command;
     * PWE port 2: egress label is resolved from EEDB, pointed by EEI
     */
     {3333,4444},

     /*
      *vpls_pwe_port_encap_id
      * for PWE port 2
      */     
     9999,

     /*
     * host_ip
     */
     {
        0x7FFFFF14, /* 127.255.255.14 */
        0x7FFFFF15, /* 127.255.255.15 */
     },
     /*
     * fec_with_arp_id
     */
     0, /*auto allocation*/

     /*
     * mpls_label_using_push_cmd
     */
     5555 
};

/**
 * PWE encapsulation
 * INPUT:
 *   unit    - unit
 *   mpls_port_encap - encapsulation information
*/
 int
 service_using_eei_vpls_pwe_port_encap(
     int unit,
     mpls_port_add_s * mpls_port_encap)
 {
     int rv = BCM_E_NONE;
     bcm_mpls_port_t mpls_port;
     bcm_gport_t gport_id;
     char *proc_name = "service_using_eei_vpls_pwe_port_encap";
 
     bcm_mpls_port_t_init(&mpls_port);
     mpls_port.flags = mpls_port_encap->flags;
     mpls_port.flags2 = mpls_port_encap->flags2;
     mpls_port.encap_id = cint_service_using_eei_info.vpls_pwe_port_encap_id;
     BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_service_using_eei_info.vpls_pwe_port_encap_id);
     BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
     mpls_port.egress_label.label = cint_service_using_eei_info.vpls_pwe_port_egress_label[1];
     mpls_port.egress_label.flags = mpls_port_encap->label_flags;
     mpls_port.egress_label.encap_access = mpls_port_encap->encap_access;
     mpls_port.egress_label.qos_map_id = mpls_port_encap->qos_map_id;
     mpls_port.egress_label.egress_qos_model.egress_qos = mpls_port_encap->egress_qos_model.egress_qos;
     mpls_port.egress_label.egress_qos_model.egress_ttl = mpls_port_encap->egress_qos_model.egress_ttl;
 
     if (mpls_port_encap->nwk_group_valid)
     {
         mpls_port.network_group_id = mpls_port_encap->pwe_nwk_group_id;
     }
 
     rv = bcm_mpls_port_add(unit, 0, mpls_port);
     if (rv != BCM_E_NONE)
     {
         printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
         return rv;
     }
 
     if(vpls_util_verbose >= 1) {
         printf("%s: mpls_port_id = 0x%x, flags = 0x%x, flags2 = 0x%x, label = %d, label_flags = 0x%x, qos_map_id = %d\n", 
             proc_name, mpls_port.mpls_port_id, mpls_port.flags, mpls_port.flags2, cint_service_using_eei_info.vpls_pwe_port_egress_label[1], mpls_port.egress_label.flags, mpls_port.egress_label.qos_map_id);
     }
 
     cint_service_using_eei_info.vpls_pwe_port_encap_id= mpls_port.encap_id;
 
     return rv;
 }

/**
 * PWE termination
 * INPUT:
 *   unit    - unit
 *   port_term - termination information
*/
 int
 service_using_eei_vpls_pwe_port_term(
     int unit,
     mpls_port_add_s * port_term)
 {
     int rv = BCM_E_NONE;
     bcm_mpls_port_t mpls_port;
     bcm_gport_t gport_id;
     char *proc_name = "service_using_eei_vpls_pwe_port_term";

     /*Case 1: PWE port using EEI as MPLS push command*/ 
     /* Step 1 : PWE port 1*/
     bcm_mpls_port_t_init(&mpls_port);
     mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
     mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
     mpls_port.ingress_qos_model.ingress_phb = port_term->ingress_qos_model.ingress_phb;
     mpls_port.ingress_qos_model.ingress_remark = port_term->ingress_qos_model.ingress_remark;
     mpls_port.ingress_qos_model.ingress_ttl = port_term->ingress_qos_model.ingress_ttl;
     mpls_port.criteria = port_term->criteria;
     mpls_port.match_label = cint_service_using_eei_info.vpls_pwe_port_ingress_label[0];
     mpls_port.egress_label.label = cint_service_using_eei_info.vpls_pwe_port_egress_label[0];
     mpls_port.nof_service_tags = port_term->nof_service_tags;
 
     if (port_term->nwk_group_valid)
     {
         mpls_port.network_group_id = port_term->pwe_nwk_group_id;
     }
 
     /** Set protection parameters if we have VPLS Ingress 1+1 Protection */
     mpls_port.ingress_failover_id = port_term->failover_id;
     mpls_port.ingress_failover_port_id = port_term->failover_port_id;
     /** connect PWE entry to created Failover MC group for learning */
     mpls_port.failover_mc_group = port_term->failover_mc_group;
 
     /* 
      * In the case of Ingres protection the egress_tunnel_if and encap_id must be 0.
      * The learning information is taken from the failover_mc_group
      */
     if (port_term->failover_id == 0)
     {
         /** connect PWE entry to created MPLS encapsulation entry for learning */
         BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, port_term->egress_tunnel_if);
     }
 
     mpls_port.vccv_type = port_term->vccv_type;
 
     rv = bcm_mpls_port_add(unit, port_term->forwarding_domain, mpls_port);
     if (rv != BCM_E_NONE)
     {
         printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
         return rv;
     }
 
     if(vpls_util_verbose >= 1) {
         printf("%s: mpls_port_id = 0x%x, flags = 0x%x, flags2 = 0x%x, match_label = %d, egress_tunnel_if = 0x%x\n", 
             proc_name, mpls_port.mpls_port_id, mpls_port.flags, mpls_port.flags2, mpls_port.match_label, mpls_port.egress_tunnel_if);
     }

     /*Case 2: PWE port using EEI as EEDB pointer*/
     bcm_mpls_port_t_init(&mpls_port);
     mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
     mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_LEARN_ENCAP_EEI;
     mpls_port.ingress_qos_model.ingress_phb = port_term->ingress_qos_model.ingress_phb;
     mpls_port.ingress_qos_model.ingress_remark = port_term->ingress_qos_model.ingress_remark;
     mpls_port.ingress_qos_model.ingress_ttl = port_term->ingress_qos_model.ingress_ttl;
     mpls_port.criteria = port_term->criteria;
     mpls_port.match_label = cint_service_using_eei_info.vpls_pwe_port_ingress_label[1];
     mpls_port.nof_service_tags = port_term->nof_service_tags;
 
     if (port_term->nwk_group_valid)
     {
         mpls_port.network_group_id = port_term->pwe_nwk_group_id;
     }
 
     /** Set protection parameters if we have VPLS Ingress 1+1 Protection */
     mpls_port.ingress_failover_id = port_term->failover_id;
     mpls_port.ingress_failover_port_id = port_term->failover_port_id;
     /** connect PWE entry to created Failover MC group for learning */
     mpls_port.failover_mc_group = port_term->failover_mc_group;
 
     /* 
      * In the case of Ingres protection the egress_tunnel_if and encap_id must be 0.
      * The learning information is taken from the failover_mc_group
      */
     if (port_term->failover_id == 0)
     {
         /** connect PWE entry to created MPLS encapsulation entry for learning */
         BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, port_term->egress_tunnel_if);
         /*
          * encap_id is the egress outlif - used for learning
          */
         mpls_port.encap_id = cint_service_using_eei_info.vpls_pwe_port_encap_id;
     }
 
     mpls_port.vccv_type = port_term->vccv_type;
 
     rv = bcm_mpls_port_add(unit, port_term->forwarding_domain, mpls_port);
     if (rv != BCM_E_NONE)
     {
         printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
         return rv;
     }
 
     if(vpls_util_verbose >= 1) {
         printf("%s: mpls_port_id = 0x%x, flags = 0x%x, flags2 = 0x%x, match_label = %d, egress_tunnel_if = 0x%x\n", 
             proc_name, mpls_port.mpls_port_id, mpls_port.flags, mpls_port.flags2, mpls_port.match_label, mpls_port.egress_tunnel_if);
     } 
 
     return rv;
 }


 /**
  * Main entrance for VPLS service example
  * INPUT:
  *   acP    - ac Port
  *   pweP   - pwe Port
 */
  int
  service_using_eei_vpls_example(
     int unit,
     int acP,
     int pweP)
 {
    int rv;


    rv = vpls_main(unit,acP,pweP);
    if (rv != BCM_E_NONE)
    {
    printf("Error, vpls_main\n");
    return rv;
    }

    rv = service_using_eei_vpls_pwe_port_encap(unit,&pwe_encap);
    if (rv != BCM_E_NONE)
    {
     printf("Error, service_using_eei_vpls_pwe_port_term\n");
     return rv;
    }


    rv = service_using_eei_vpls_pwe_port_term(unit,&pwe_term);
    if (rv != BCM_E_NONE)
    {
    printf("Error, service_using_eei_vpls_pwe_port_term\n");
    return rv;
    }
    return rv;

}

 /**
  * Main entrance for L3 service example
  * INPUT:
  *   inP    - incoming Port
  *   outP   - outgoing Port
 */
  int
  service_using_eei_l3_route_example(
     int unit,
     int inP,
     int outP)
 {
    int rv;
    sand_utils_l3_fec_s fec_structure;
    bcm_l3_host_t l3host;
    int   eei;
    char *proc_name;

    proc_name = "service_using_eei_l3_route_example";

    /*
     * when EEI is used for L3 fwd, there are 2 scenarios:
     * 1. IP host table lookup-------->EEI+destination(FEC)
     * 2. IP route table lookup-------->FEC(EEI+destination)
     * the item 2 is demonstrated by cint_sand_multi_device_ipv4_uc.c 
     */


    /*
     * Note:
     * it only need the L3 basic setup(L3 Intf, ARP)
     * and mpls tunnel encapsulation function.
     */
    rv = mpls_ingress_push_l3_example(unit,inP,outP);
    if (rv != BCM_E_NONE)
    {
    printf("Error, mpls_ingress_push_l3_example\n");
    return rv;
    }

    /*
     * Case 3, EEI is used as EEDB pointer
     */
    /*
     * step 1: create FEC for arp, format is Out-LIF + destination
     */
    sand_utils_l3_fec_s_init(unit, 0x0, &fec_structure);

    fec_structure.destination = outP;
    fec_structure.tunnel_gport = cint_mpls_ingress_push_info.out_arp_id;
    rv = sand_utils_l3_fec_create(unit, &fec_structure);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    cint_service_using_eei_info.fec_with_arp_id = fec_structure.l3eg_fec_id;


    /*
     * step 2: add host table entry, with EEI+FEC result type
     */
    rv = add_host_ipv4(unit, cint_service_using_eei_info.host_ip[0], 
                        cint_mpls_ingress_push_info.vrf, 
                        cint_service_using_eei_info.fec_with_arp_id, 
                        cint_mpls_ingress_push_info.mpls_tunnel_id,
                        0);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
        return rv;
    }

    /*
     * Case 4, EEI is used as MPLS push command
     */

    /*
     * Step 1:add entry into host table
     */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = cint_service_using_eei_info.host_ip[1];
    l3host.l3a_vrf = cint_mpls_ingress_push_info.vrf;
    l3host.l3a_intf = cint_service_using_eei_info.fec_with_arp_id;
    l3host.l3a_flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    /*Use the default MPLS push command 0 */
    BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei, cint_service_using_eei_info.mpls_label_using_push_cmd,0);
    BCM_FORWARD_ENCAP_ID_VAL_SET(l3host.encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT, eei);
    rv = bcm_l3_host_add(unit, &l3host);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_host_add on unit %d\n",unit);
        return rv;
    }

    return rv;

}






