/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Service using EEI~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_sand_service_using_eei.c,v 1.0 2018/05/11 08:05:38 Jingli Guo $
 * $Copyright: (c) 2019 Broadcom.
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
 *  6. VPLS P2P service, AC--PWE(outlif+Dest), get outlif(PWE)+dest from AC.
 *     The dest is a pointer to FEC in which EEI is resolved, and EEI is a pointer
 *     to EEDB for LSP x 2.
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
 * cint ../../src/./examples/dnx/cint_sand_field_fec_id_align_wa.c
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
 *
 *
 * Test Scenario(6)
 *
 * ./bcm.user
 * cint ../../../../src/examples/dpp/utility/cint_utils_global.c
 * cint ../../../../src/examples/dnx/field/cint_field_utils.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/cint_sand_mpls_ingress_push.c
 * cint ../../../../src/examples/sand/cint_sand_service_using_eei.c
 *
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int provider_port = 201;
 * rv = service_using_eei_in_fec_vpls_example(unit,access_port,provider_port);
 *
 * print rv;
 * exit;
 *
 * Access to core: ETH -> ETHoMPLSoMPLSoMPLSoETH
 * tx 1 psrc=200 data=0x00110000011200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *  Received packets on unit 0 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x00000000cd1d000c000200018100001e8847044440ff033330ff00d801ff00110000011200010203040591000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *  The first 46Bytes are verified.
 */

 struct cint_service_using_eei_s
 {
    int vpls_pwe_port_ingress_label[2];  /*pwe incoming label*/
    int vpls_pwe_port_egress_label[2];   /*pwe  outgoing label*/
    int vpls_pwe_port_encap_id ;         /*pwe  encap_id*/

    bcm_ip_t host_ip[2];                    /*host ip for IP host fwd*/
    int      fec_with_arp_id;            /*FEC ID, storing ARP ID*/

    int mpls_label_using_push_cmd;       /*MPLS label encapsulated by MPLS push command*/
    bcm_field_group_t field_wa_fg_id;    /*FG ID for FEC alignment*/ 
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
*  Out-LIF that its forward information is destination + EEI.
*  Used by test AT_Dnx_vswitch_sem_cross_connect_add_replace.
*/
bcm_gport_t term_mpls_port_id_eei_as_mpls_push_command;
bcm_gport_t term_mpls_port_id_eei_as_eedb_pointer;
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
 
     term_mpls_port_id_eei_as_mpls_push_command = mpls_port.mpls_port_id;

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
 
     term_mpls_port_id_eei_as_eedb_pointer = mpls_port.mpls_port_id;
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

    rv = fec_id_alignment_field_wa_add(unit, acP, BCM_L3_ITF_VAL_GET(pwe_term.egress_tunnel_if));
    if (rv != BCM_E_NONE)
    {
        printf("Error, fec_id_alignment_field_wa_entry_add\n");
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
    int is_jr2_mode = is_device_or_above(unit, JERICHO2) ? *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL)) : 0;
    int encap_id;
    if (is_jr2_mode) {
        /** In case of JR2, no needs to pass a 2nd outlif by EEI, just by outlif itself.*/
        BCM_FORWARD_ENCAP_ID_VAL_SET(encap_id, BCM_FORWARD_ENCAP_ID_TYPE_OUTLIF, BCM_FORWARD_ENCAP_ID_OUTLIF_USAGE_GENERAL, cint_mpls_ingress_push_info.mpls_tunnel_id); 
    } else {
        encap_id = cint_mpls_ingress_push_info.mpls_tunnel_id; 
    }
    rv = add_host_ipv4(unit, cint_service_using_eei_info.host_ip[0], 
                        cint_mpls_ingress_push_info.vrf, 
                        cint_service_using_eei_info.fec_with_arp_id, 
                        encap_id,
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

/*
 * Update the mpls tunnel encapsulation info specially for
 * service_using_eei_in_fec_vpls_example
 */
int
service_using_eei_in_fec_tunnel_info_update(int unit)
{
    /*
     * mpls encapsulation tunnel parameters 
     */
    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel);
    mpls_encap_tunnel[0].label[0] = 0x3333;
    mpls_encap_tunnel[0].label[1] = 0x4444;
    mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_encap_tunnel[0].tunnel_id = 8196;
    mpls_encap_tunnel[0].num_labels = 2;

    if (is_device_or_above(unit, JERICHO) && !is_device_or_above(unit, JERICHO2))
    {
        mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    /*
     * mpls tunnel termination parameters 
     */
    mpls_tunnel_switch_create_s_init(mpls_term_tunnel);
    mpls_term_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[0].label = mpls_encap_tunnel[0].label[0];

    mpls_term_tunnel[1].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[1].label = mpls_encap_tunnel[0].label[1];

}

/** create a P2P vlan port */
int
vpls_vswitch_add_p2p_access_port(
    int unit,
    l2_port_properties_s * port,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    char *proc_name = "vpls_vswitch_add_access_port";

    /*
     * add port, according to port_vlan_vlan 
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = vpn;
    vlan_port.egress_vlan = vpn;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    port->vlan_port_id = vlan_port.vlan_port_id;
    port->encap_id = vlan_port.encap_id;

    if(vpls_util_verbose >= 1) {
        printf("%s: vlan_port_id = 0x%x, port = %d, vpn = %d\n", proc_name, vlan_port.vlan_port_id, port->port, vpn);
    }

    return rv;
}


/**
 * Main entrance for EEI carrying outlif in IOP mode.
 * Access to core: get outlif(PWE) and destination(FEC) from in AC.
 * The FEC is resolved to EEI which is a pointer to Tunnel1-Tunnel2->ARP.
 * INPUT:
 *   inP    - incoming Port
 *   outP   - outgoing Port
*/

int
service_using_eei_in_fec_vpls_example(
    int unit,
    int access_port,
    int provider_port)
{
    int rv = BCM_E_NONE;
    int ii;
    char *proc_name;

    proc_name = "vpls_using_eei_in_fec";

    if (!is_device_or_above(unit, JERICHO))
    {
        printf("Only Jericho and above devices are supported\n");
        return BCM_E_UNAVAIL;
    }
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = bcm_port_tpid_add(unit, access_port, 0x9100, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_port_tpid_add\n");
            return rv;
        }
    }

    if (!params_set)
    {
        init_default_vpls_params(unit);
        service_using_eei_in_fec_tunnel_info_update(unit);
    }
    ac_port.port = access_port;
    pwe_port.port = provider_port;
    outer_mpls_tunnel_index = 0;
    mpls_tunnel_in_eei = 1;

    /** Configure control word to be used in case needed for special label */
    rv = bcm_switch_control_set(unit, bcmSwitchMplsPWControlWord, pwe_cw);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_configure_port_properties(unit, ac_port, 0 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties ac_port\n", rv);
        return rv;
    }
    rv = vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties pwe_port\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    rv = bcm_vlan_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, vpn);
        return rv;
    }
    pwe_term.forwarding_domain = vpn;

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    printf("%s(): Going to load mpls_encap_tunnel[%d].l3_intf_id by 0x%08X\r\n",proc_name, outer_mpls_tunnel_index, pwe_port.arp);
    mpls_encap_tunnel[outer_mpls_tunnel_index].l3_intf_id = pwe_port.arp;

    /*
     * Configure a push entry.
     */
    rv = vpls_create_mpls_tunnel(unit, mpls_encap_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_mpls_tunnel\n", rv);
        return rv;
    }
    pwe_port.tunnel_id = mpls_encap_tunnel[0].tunnel_id;

    /*
     * Configure fec entry 
     */
    rv = vpls_create_fec_entry(unit, ac_port.encap_fec_id, mpls_encap_tunnel[0].tunnel_id, pwe_port.port,mpls_tunnel_in_eei?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_fec_entry\n", rv);
        return rv;
    }
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * Configure a termination label for the ingress flow  
     */
    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /** define ac and pwe ports */
    rv = vpls_vswitch_add_p2p_access_port(unit, &ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add cross connect for p2p service
     */
    int l3_intf;
    bcm_vswitch_cross_connect_t cross_connect_gports;
    bcm_vswitch_cross_connect_t_init(&cross_connect_gports);
    cross_connect_gports.port1 = ac_port.vlan_port_id;

    BCM_L3_ITF_SET(l3_intf, BCM_L3_ITF_TYPE_FEC, ac_port.encap_fec_id);
    BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(cross_connect_gports.port2, l3_intf);
    cross_connect_gports.encap2 = pwe_encap.encap_id;
    cross_connect_gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vswitch_cross_connect_add\n", rv);
        return rv;
    }

    if(vpls_util_verbose == 1)
    {
        printf("%s(): EXIT. port1 %d,port2 %d\r\n", proc_name, access_port, provider_port);
    }

    return rv;

}



