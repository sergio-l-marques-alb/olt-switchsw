/*
* $Id: cint_dnx_mirror_example.c,v 1.23 Broadcom SDK $
* $Copyright: (c) 2018 Broadcom.
* Broadcom Proprietary and Confidential. All rights reserved.$
*
*
*/
/*
 * Example of a simple-router scenario 
 * Test Scenario 
 *
 * ./bcm.user
 * cint sand/utility/cint_sand_utils_global.c
 * cint sand/utility/cint_sand_utils_tpid.c
 * cint sand/utility/cint_sand_utils_vlan_translate.c
 * cint dnx/field/cint_field_utils.c
 * cint sand/cint_ip_route_basic.c
 * cint dnx/cint_dnx_mirror_example.c
 *
 * Tests:
 *
 * 1. SPAN
 *         cint
 *         mirror_span_example(0, 200, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201/202:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 2. RSPAN
 *         cint
 *         mirror_rspan_example(0, 200, 15, 201, 202, 100);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201/202:
 *             000000000002000000000001810000648100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 3. ERSPANv2
 *         cint
 *         mirror_erspan_example_v2(0, 200, 15, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         expect to receive packet from port 202:
 *             000c0002000000123456789a810000640800450a00a0000000003c2f3d03a0000011a1000011100088be0000000010640000000a00c8
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * 4. ERSPANv3
 *         cint
 *         mirror_erspan_example_v3(0, 200, 15, 201, 202);
 *         exit;
 *
 *         tx 1 l=128 psrc=200 data=0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *         expect to receive packet from port 201:
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *         expect to receive packet from port 202:
 *             000c0002000000123456789a810000640800450a00ac000000003c2f3cf7a0000011a1000011100088be00000000106400001b791d00000083a5165a00c800000079
 *             0000000000020000000000018100000f0899000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

/*
 * Structure to describe an ERSPAN tunnel.
 */
struct erspan_tunnel_info_t
{
    /* if TRUE ERSPAN works in v3 mode */
    int is_erspan_v3;

    /* fields common for ERSPANv2 & v3 header */
    int vlan;
    int cos; /* 3 bits */
    int truncated; /* 1 bit */
    int direction; /* 1 bit */
    int session_id; /* 10 bits */

    /* 'EN' bits in ERSPANv2 and 'BSO' bits in ERSPANv3 */
    int en; /* 2 bits */

    /* fields for ERSPANv2 only */
    int index_17_19; /* 3 bits */

    /* fields for ERSPANv3 only */
    int hw_id; /* 6 bits */
    int switch_id; /* 10 bits */

    /* IPv4 tunnel */
    bcm_ip_t dip;
    bcm_ip_t sip;
    int dscp;
    int ttl;

    /* Link layer */
    bcm_mac_t next_hop;
    bcm_vlan_t vid;

    /* Saved info for clean-up */
    bcm_l3_intf_t rspan_intf;
    bcm_l3_intf_t erspan_intf;
    bcm_l3_intf_t ip_tunnel_intf;
    bcm_if_t next_hop_if;
};

erspan_tunnel_info_t erspan_info = {
/** is_erspan_v3   vlan   cos   truncated   direction   session_id */
    FALSE,        100,  7,     0,         0,      160, 
/** en   index_17_19   hw_id   switch_id      dip                  sip   */
     0,       5,      0x3A,   0x25A, 0xA1000011, 0xA0000011, 
/** dscp   ttl                            next_hop                              vid   */
     10,  60, {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},   100 
};

/*
 * Structure to describe mirror forwarding info
 */
struct mirror_info_t
{
    /* Link layer */
    bcm_mac_t da;
    bcm_vlan_t vlan;
    /* out_vlan for routed packets, equal to intf_out in dnx_basic_example_inner */
    bcm_vlan_t out_vlan;

    /*
    * Those are known tag formats that are defined in the system:
    * DNX_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE/S_TAG/C_TAG/S_C_TAG/S_PRIORITY_C_TAG
    */
    int defined_tag_formats[32];
    int nof_defined_tag_formats;

    int eve_profile;
    int eve_action_id;

    /* Saved info for clean-up */
    bcm_gport_t mirror_dest_id;
    bcm_gport_t vlan_port_id;
    int in_port;
    int out_port;
    int is_mc;
    int mc_id;
};

mirror_info_t mirror_info = {
/**                       da                                           vlan   out_vlan */
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x02},   15,    100,
/** defined_tag_formats nof_defined_tag_formats eve_profile */
    {0, 4, 8, 16, 24},            5,              8
};

/* debug information log level */
int verbose = 2;

/* Current mirroring mode */
int G_mirror_type = BCM_MIRROR_PORT_INGRESS;

/*
 * Set mirroring mode:
 * - outbound: 0 - inbound; 1 - outbound
 */
int set_outbound_mode(int unit, int outbound){
    if(outbound) {
        G_mirror_type = BCM_MIRROR_PORT_EGRESS;
        bshell(unit, "mod IPPE_PRT_PP_PORT_INFO 100 40 PARSING_CONTEXT=1");
    } else {
        G_mirror_type = BCM_MIRROR_PORT_INGRESS;
    }
    return 0;
}

 /*
  * SPAN mirroring example. Mirrors packets received on in_port (inbound mirroring) or
  * transmitted out from out_port (outbound mirroring) to mirror destination port.
  *
  * - in_port : port that packets to be recieved from
  * - out_port : port that packets to be transmitted out
  * - mirror_port : mirror destination port
  */
int mirror_span_example(int unit, int in_port, int out_port, int mirror_port) {
    bcm_mirror_destination_t dest;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;

    bcm_vlan_create(unit, mirror_info.vlan);    

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("Error: bcm_mirror_destination_create %d\n", rv);
        return rv;
    }

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, G_mirror_type, dest.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_add %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    return rv;
}
 
 /*
 * Clean up procedure for mirror_span_example
 */
int mirror_span_example_clean_up(int unit) {
    bcm_pbmp_t p;
    int rv;


    if (rv = bcm_mirror_port_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    BCM_PBMP_PORT_SET(p, mirror_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

    if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
        printf("Error: bcm_vlan_port_remove %d\n", rv);
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }

    return rv;
}

/*
 * RSPAN mirroring example. Mirrors packets received on in_port (inbound mirroring) or
 * transmitted out from out_port (outbound mirroring) to mirror destination port with VLAN tunnel.
 *
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - rspan_tag : VLAN tag of the RSPAN tunnel
 */
int mirror_rspan_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int rspan_tag) {
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_vlan_port_translation_t port_trans;
    bcm_mirror_destination_t dest;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int action_id_1;
    int index;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    bcm_vlan_create(unit, mirror_info.vlan);    

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    if (rv = bcm_vlan_port_create(unit, &vlan_port)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    mirror_info.vlan_port_id = vlan_port.vlan_port_id;

    /*
    * set edit profile for ingress/egress LIF 
    */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = rspan_tag;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = mirror_info.eve_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_port_translation_set %d\n", rv);
        return rv;
    }

    /*
    * Create action ID
    */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_create %d\n", rv);
        return rv;
    }

    mirror_info.eve_action_id = action_id_1;

    /*
    * Set translation action 
    */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_outer_pkt_prio = bcmVlanActionReplace;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_set %d\n", rv);
        return rv;
    }

    /*
    * Set translation action class (map edit_profile & tag_format to action_id) 
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    for (index=0; index<mirror_info.nof_defined_tag_formats; ++index) {
        action_class.tag_format_class_id = mirror_info.defined_tag_formats[index];
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    bcm_l2_addr_t_init(&l2addr, mirror_info.da, mirror_info.vlan);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add %d\n", rv);
        return rv;
    }

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* RSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.out_vport = vlan_port.vlan_port_id;

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", dest.encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    return rv;
}

/*
 * Clean up procedure for mirror_rspan_example
 */
int mirror_rspan_example_clean_up(int unit) {
    bcm_vlan_translate_action_class_t action_class;
    bcm_pbmp_t p;
    int index;
    int rv;

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_vlan_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    /*
    * Set translation action class to default
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = 0;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    for (index=0; index<mirror_info.nof_defined_tag_formats; ++index) {
        action_class.tag_format_class_id = mirror_info.defined_tag_formats[index];
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    /*
    * Clear translation action 
    */
    if (rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, mirror_info.eve_action_id)) {
        printf("Error: bcm_vlan_translate_action_id_destroy %d\n", rv);
    }

    if (rv = bcm_vlan_port_destroy(unit, mirror_info.vlan_port_id)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    BCM_PBMP_PORT_SET(p, mirror_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

    if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
        printf("Error: bcm_vlan_port_remove %d\n", rv);
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }


    return rv;
}

/*
 * RSPAN Advanced mirroring example. Mirrors packets received on in_port (inbound mirroring) or
 * transmitted out from out_port (outbound mirroring) to mirror destination port with VLAN tunnel.
 *
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - rspan_tag : VLAN tag of the RSPAN tunnel
 */
int mirror_rspan_advanced_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int rspan_tag) {
    bcm_tunnel_initiator_t rspan_tunnel;
    bcm_tunnel_initiator_t rspan_tunnel_get;
    bcm_l3_intf_t rspan_tunnel_intf;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_mirror_header_info_t mirror_header_info;
    bcm_vlan_port_translation_t port_trans;
    bcm_mirror_destination_t dest;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int name_space = 0x55;
    int action_id_1;
    int index;
    int rv;
 
    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    bcm_vlan_create(unit, mirror_info.vlan);    

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, mirror_info.vlan, p, u);

    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
    BCM_GPORT_SYSTEM_PORT_ID_SET(vlan_port.port, in_port);
    vlan_port.match_class_id = name_space;
    if (rv = bcm_vlan_port_create(unit, &vlan_port)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    mirror_info.vlan_port_id = vlan_port.vlan_port_id;

    /*
    * set edit profile for ingress/egress LIF 
    */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = rspan_tag;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = mirror_info.eve_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_port_translation_set %d\n", rv);
        return rv;
    }

    /*
    * Create action ID
    */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS, &action_id_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_create %d\n", rv);
        return rv;
    }

    mirror_info.eve_action_id = action_id_1;

    /*
    * Set translation action 
    */
    bcm_vlan_action_set_t_init(&action);
    action.outer_tpid = 0x8100;
    action.dt_outer = bcmVlanActionAdd;
    action.dt_outer_pkt_prio = bcmVlanActionReplace;
    rv = bcm_vlan_translate_action_id_set(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id_1, &action);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_vlan_translate_action_id_set %d\n", rv);
        return rv;
    }

    /*
    * Set translation action class (map edit_profile & tag_format to action_id) 
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = action_id_1;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    for (index=0; index<mirror_info.nof_defined_tag_formats; ++index) {
        action_class.tag_format_class_id = mirror_info.defined_tag_formats[index];
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    bcm_l2_addr_t_init(&l2addr, mirror_info.da, mirror_info.vlan);
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add %d\n", rv);
        return rv;
    }

    bcm_mirror_destination_t_init(&dest);
    dest.gport = mirror_port;

    /* create the destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    /* create RSPAN Advanced tunnel */
    bcm_l3_intf_t_init(&rspan_tunnel_intf);
    bcm_tunnel_initiator_t_init(&rspan_tunnel);

    rspan_tunnel.type = bcmTunnelTypeRspanAdvanced;
    rspan_tunnel.encap_access = bcmEncapAccessTunnel1;
    rspan_tunnel.class_id = name_space;
 
    if (rv = bcm_tunnel_initiator_create(unit, &rspan_tunnel_intf, &rspan_tunnel)) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    erspan_info.rspan_intf = rspan_tunnel_intf;    

    if (rv = bcm_tunnel_initiator_get(unit, &rspan_tunnel_intf, &rspan_tunnel_get)) {
        printf("Error, get RSPAN Advanced tunnel initiator \n");
        return rv;
    }

    if ((rspan_tunnel_get.type != bcmTunnelTypeRspanAdvanced) || (rspan_tunnel_get.class_id != rspan_tunnel.class_id)) {
        printf("Error, RSPAN Advanced tunnel initiator get mismatch\n");
        print rspan_tunnel;
        print rspan_tunnel_get;
        return BCM_E_FAIL;
    }

    /* ERSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.out_vport = rspan_tunnel.tunnel_id;

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_add(unit, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port, in_vlan, G_mirror_type, dest.mirror_dest_id)) {
        printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
        return rv;
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", dest.encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    bshell(unit, "m ETPPC_CFG_ENTRY_IS_EM_BUSTER CFG_ENTRY_IS_EM_BUSTER=0x220004001f50");

    return rv;
}

/*
 * Clean up procedure for mirror_rspan_advanced_example
 */
int mirror_rspan_advanced_example_clean_up(int unit) {
    bcm_vlan_translate_action_class_t action_class;
    bcm_pbmp_t p;
    int index;
    int rv;

    /* set in-bound mirror on <in_port, in_vlan> to created mirror destination */
    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_vlan_dest_delete %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.rspan_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    /*
    * Set translation action class to default
    */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = mirror_info.eve_profile;
    action_class.vlan_translation_action_id = 0;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;

    for (index=0; index<mirror_info.nof_defined_tag_formats; ++index) {
        action_class.tag_format_class_id = mirror_info.defined_tag_formats[index];
        rv = bcm_vlan_translate_action_class_set(unit, &action_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_translate_action_class_set\n");
            return rv;
        }
    }

    /*
    * Clear translation action 
    */
    if (rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS, mirror_info.eve_action_id)) {
        printf("Error: bcm_vlan_translate_action_id_destroy %d\n", rv);
    }

    if (rv = bcm_vlan_port_destroy(unit, mirror_info.vlan_port_id)) {
        printf("Error: bcm_vlan_port_create %d\n", rv);
        return rv;
    }

    BCM_PBMP_PORT_SET(p, mirror_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

    if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
        printf("Error: bcm_vlan_port_remove %d\n", rv);
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }


    return rv;
}


int mirror_with_span_lag_lb_example(int unit, int in_port, int in_vlan, int dest_port, int trunk_id) {
    int rv;
    bcm_gport_t mirror_port;
    BCM_GPORT_TRUNK_SET(mirror_port,trunk_id);
    return mirror_with_span_example(unit,in_port,in_vlan,dest_port,mirror_port);
}

 /*
  * Change default configuration for ERSPAN tunnel.
  */
 int mirror_erspan_tunnel_init(
     int is_erspan_v3,
     int vlan,
     int cos,
     int truncated,
     int direction,
     int session_id,
     int en,
     int index_17_19,
     int hw_id,
     int switch_id,
     bcm_ip_t dip,
     bcm_ip_t sip,
     int dscp,
     int ttl,
     bcm_mac_t next_hop,
     bcm_vlan_t vid)
 {
     erspan_info.is_erspan_v3 = is_erspan_v3;
     erspan_info.vlan = vlan;
     erspan_info.cos = cos;
     erspan_info.truncated = truncated;
     erspan_info.direction = direction;
     erspan_info.session_id = session_id;
     erspan_info.en = en;
     erspan_info.index_17_19 = index_17_19;
     erspan_info.hw_id = hw_id;
     erspan_info.switch_id = switch_id;
     erspan_info.dip = dip;
     erspan_info.sip = sip;
     erspan_info.dscp = dscp;
     erspan_info.ttl = ttl;
     erspan_info.next_hop = next_hop;
     erspan_info.vid = vid;
 
     return 0;
 }
 
  /*
   * Create basic routing application, RIF, MyMAC...etc.
   * - in_port : port that packets to be recieved from
   * - out_port : port that packets to be transmitted out
   */
 int
 erspan_routing(
     int unit,
     int in_port,
     int out_port)
 {
     int kaps_result = 0xA711;
     return dnx_basic_example(unit, in_port, out_port, kaps_result);
 }
  

/*
 * Create ERSPANv2/v3 tunnel and return the encap_id.
 * - encap_id : point to the created ERSPAN tunnel
 */
int mirror_erspan_tunnel_create(int unit, bcm_if_t *encap_id) {
    bcm_tunnel_initiator_t ip_tunnel;
    bcm_tunnel_initiator_t erspan_tunnel;
    bcm_tunnel_initiator_t erspan_tunnel_get;
    bcm_l3_intf_t ip_tunnel_intf;
    bcm_l3_intf_t erspan_tunnel_intf;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null = 0;
    bcm_gport_t arp_ac_gport;
    int vlan_edit_profile = 7;
    int rv;

    /* create IPv4 tunnel for ERSPAN */
    bcm_tunnel_initiator_t_init(&ip_tunnel);
    ip_tunnel.dip = erspan_info.dip;
    ip_tunnel.sip = erspan_info.sip;
    ip_tunnel.dscp = erspan_info.dscp;
    ip_tunnel.ttl = erspan_info.ttl;
    ip_tunnel.type = bcmTunnelTypeGreAnyIn4;
    ip_tunnel.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    ip_tunnel.encap_access = bcmEncapAccessTunnel2;
    ip_tunnel.flags = BCM_TUNNEL_INIT_GRE_WITH_SN;

    bcm_l3_intf_t_init(&ip_tunnel_intf);
    if (rv = bcm_tunnel_initiator_create(unit, &ip_tunnel_intf, &ip_tunnel)) {
        printf("Error, bcm_tunnel_initiator_create %d\n", rv);
        return rv;
    }

    erspan_info.ip_tunnel_intf = ip_tunnel_intf;

    if (verbose >= 1) {
        printf("created IP tunnel on intf: 0x%08x \n", ip_tunnel_intf.l3a_intf_id);
    }

    /* Next hop */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = erspan_info.next_hop;
    l3eg.intf = ip_tunnel_intf.l3a_intf_id;
    l3eg.flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    l3eg.vlan = erspan_info.vid;

    if (rv = bcm_l3_egress_create(unit, BCM_L3_EGRESS_ONLY, &l3eg, &l3egid_null)) {
        printf("Error: bcm_l3_egress_create %d\n", rv);
        return rv;
    }

    erspan_info.next_hop_if = l3eg.encap_id;

    if (verbose >= 1) {
        printf("next hop mac at encap-id 0x%08x, \n", l3eg.encap_id);
    }

    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_ac_gport, BCM_L3_ITF_VAL_GET(l3eg.encap_id));
    BCM_GPORT_VLAN_PORT_ID_SET(arp_ac_gport, arp_ac_gport);

    /*
    * Perform vlan editing for the ARP gport.
    * This will add erspan_info.vid as the VLAN tag
    */
    rv = vlan_translate_ive_eve_translation_set(unit, arp_ac_gport,         /* lif  */
                                                      0x8100,               /* outer_tpid */
                                                      0x8100,               /* inner_tpid */
                                                      bcmVlanActionMappedReplace,  /* outer_action */
                                                      bcmVlanActionNone,                 /* inner_action */
                                                      0,                 /* new_outer_vid */
                                                      0,               /* new_inner_vid */
                                                      vlan_edit_profile,    /* vlan_edit_profile */
                                                      0,                    /* tag_format - must be untag! */
                                                      FALSE                 /* is_ive */
                                                      );

    if (rv != BCM_E_NONE) {
        printf("Error: vlan_translate_ive_eve_translation_set %d\n", rv);
        return rv;
    }

    /* create ERSPAN tunnel */
    bcm_l3_intf_t_init(&erspan_tunnel_intf);
    bcm_tunnel_initiator_t_init(&erspan_tunnel);

    erspan_tunnel.type = bcmTunnelTypeErspan;
    erspan_tunnel.l3_intf_id = ip_tunnel_intf.l3a_intf_id;
    erspan_tunnel.encap_access = bcmEncapAccessTunnel1;

    if (erspan_info.is_erspan_v3) {
        erspan_tunnel.flags |= BCM_TUNNEL_INIT_ERSPAN_TYPE3;
        erspan_tunnel.hw_id = erspan_info.hw_id;
        erspan_tunnel.switch_id = erspan_info.switch_id;
    } else {
        erspan_tunnel.aux_data = erspan_info.index_17_19;
    }

    if (rv = bcm_tunnel_initiator_create(unit, &erspan_tunnel_intf, &erspan_tunnel)) {
        printf("Error, create tunnel initiator \n");
        return rv;
    }

    erspan_info.erspan_intf = erspan_tunnel_intf;

    if (verbose >= 1) {
        printf("created ERSPAN tunnel on intf: 0x%08x \n", erspan_tunnel.tunnel_id);
    }

    if (rv = bcm_tunnel_initiator_get(unit, &erspan_tunnel_intf, &erspan_tunnel_get)) {
        printf("Error, get ERSPAN tunnel initiator \n");
        return rv;
    }

    if ((erspan_info.is_erspan_v3 
            && (((erspan_tunnel_get.flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3) != BCM_TUNNEL_INIT_ERSPAN_TYPE3)
                || (erspan_tunnel_get.type != bcmTunnelTypeErspan)
                || (erspan_tunnel_get.hw_id != erspan_tunnel.hw_id)
                || (erspan_tunnel_get.switch_id != erspan_tunnel.switch_id)))
        ||
        (!erspan_info.is_erspan_v3 
            && (((erspan_tunnel_get.flags & BCM_TUNNEL_INIT_ERSPAN_TYPE3) != 0)
                || (erspan_tunnel_get.aux_data != erspan_tunnel.aux_data)))) {
        printf("Error, ERSPAN tunnel initiator get mismatch\n");
        print erspan_tunnel;
        print erspan_tunnel_get;
        return BCM_E_FAIL;
    }

    *encap_id = erspan_tunnel.tunnel_id;

    return rv;
}

/*
 * Configure pipeline to mirror the packet to ERSPAN tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - is_mc : set if mirror destination is a multicast group
 * - mc_id : multicast ID in case mirror destination is a multicast group
 * - encap_id : point to the ERSPAN tunnel
 */
int mirror_erspan_destination_create(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port, int is_mc, int mc_id, bcm_if_t encap_id) {
    bcm_mirror_header_info_t mirror_header_info;
    bcm_mirror_header_info_t mirror_header_info_get;
    bcm_mirror_destination_t dest;
    uint32 flags = 0;
    int rv;

    bcm_mirror_destination_t_init(&dest);
    if (is_mc == TRUE) {
        /* create multicat group and replicate to ERSPAN tunnel */
        bcm_multicast_replication_t replications;
        if (rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP, mc_id)) {
            printf("Error: bcm_multicast_create %d\n", rv);
            return rv;
        }

        bcm_multicast_replication_t_init(&replications);
        replications.encap1 = BCM_GPORT_TUNNEL_ID_GET(encap_id);
        replications.port = mirror_port;

        if (rv = bcm_multicast_add(unit, mc_id, BCM_MULTICAST_EGRESS_GROUP, 1, &replications)) {
            printf("Error, create ergress multicast\n");
            return rv;
        }

        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1;
        if (rv = bcm_fabric_multicast_set(unit, mc_id, 0, 2, mreps)) {
            printf("Error: bcm_fabric_multicast_set %d\n", rv);
            return rv;
        }

        BCM_GPORT_MCAST_SET(dest.gport, mc_id);
    }
    else {
        dest.gport = mirror_port;
    }

    /* create mirror destination */
    if (rv = bcm_mirror_destination_create(unit, &dest)) {
        printf("failed to create mirror destination, return value %d\n", rv);
        return rv;
    }

    if (verbose >= 2 ) {
        printf("created mirrod destination: ID:0x%08x  mirror-port:0x%08x \n", dest.mirror_dest_id, mirror_port);
    }

    mirror_info.mirror_dest_id = dest.mirror_dest_id;

    /* ERSPAN header editing */
    bcm_mirror_header_info_t_init(&mirror_header_info);
    mirror_header_info.tm.src_sysport = in_port;
    mirror_header_info.tm.ase_ext.valid = TRUE;
    mirror_header_info.tm.ase_ext.ase_type = bcmPktDnxAseTypeErspan;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.vlan = erspan_info.vlan;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.cos = erspan_info.cos;
    mirror_header_info.tm.ase_ext.ase_info.erspan_info.direction = (G_mirror_type == BCM_MIRROR_PORT_INGRESS)?0:1;

    if (is_mc == TRUE) {
        mirror_header_info.tm.is_mc_traffic = TRUE;
        BCM_GPORT_MCAST_SET(mirror_header_info.tm.mc_id, mc_id);
    } else {
        mirror_header_info.tm.out_vport = encap_id;
    }

    if (rv = bcm_mirror_header_info_set(unit, 0, dest.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
        return rv;
    }

    bcm_mirror_header_info_t_init(&mirror_header_info_get);
    if (rv = bcm_mirror_header_info_get(unit, dest.mirror_dest_id, &flags, &mirror_header_info_get)) {
        printf("Failed bcm_mirror_header_info_get, return value %d\n", rv);
        return rv;
    }

    if ((mirror_header_info.tm.ase_ext.valid != mirror_header_info_get.tm.ase_ext.valid) ||
        (mirror_header_info.tm.ase_ext.ase_type != mirror_header_info_get.tm.ase_ext.ase_type) ||
        (mirror_header_info.tm.ase_ext.ase_info.erspan_info.vlan != mirror_header_info_get.tm.ase_ext.ase_info.erspan_info.vlan)) {
        printf("Failed to check erspan information.\n");
        return BCM_E_FAIL;
    }

    /* set <port,vlan> mirroring */
    if (G_mirror_type == BCM_MIRROR_PORT_INGRESS) {
        if (rv = bcm_mirror_port_vlan_dest_add(unit, in_port, mirror_info.vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }
    }
    else {
        /* out_port + in_vlan, for bridged packets */
        if (rv = bcm_mirror_port_vlan_dest_add(unit, out_port, mirror_info.vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }

        /* out_port + out_vlan, for routed packets */
        if (rv = bcm_mirror_port_vlan_dest_add(unit, out_port, mirror_info.out_vlan, G_mirror_type, dest.mirror_dest_id)) {
            printf("Failed to add inbound in_port+vlan1 to be mirrored to mirr_dest2, return value %d\n", rv);
            return rv;
        }
    }

    if(verbose >= 2 ) {
        printf("created mirror, encap-id: 0x%08x  dest:0x%08x \n", encap_id, dest.mirror_dest_id);
        if(G_mirror_type & BCM_MIRROR_PORT_INGRESS) {
            printf("ingress mirror \n" );
        }

        if(G_mirror_type & BCM_MIRROR_PORT_EGRESS) {
            printf("Egress mirror \n" );
        }

        printf("mirror cond : vlan:%d  port:0x%08x \n", in_vlan, (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? in_port : out_port);
    }

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv2/v3 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port){
    bcm_if_t encap_id;
    bcm_l2_addr_t l2addr;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;

    erspan_routing(unit, in_port, out_port);

    bcm_vlan_create(unit, in_vlan);    

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, in_vlan, p, u);

    mirror_info.vlan = in_vlan;

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    /* build IP tunnels, and get back interfaces */
    if (rv = mirror_erspan_tunnel_create(unit, &encap_id)) {
        printf("Error, mirror_erspan_tunnel_create \n");
        return rv;
    }

    /* add inbound mirror to send out of the create ip tunnel with span encapsulation */
    rv = mirror_erspan_destination_create(unit, in_port, in_vlan, out_port, mirror_port, 0, 0, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in mirror_erspan_destination_create, in_port=%d, \n", in_port);
        return rv;
    }

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv2 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example_v2(int unit, int in_port, bcm_vlan_t in_vlan, int out_port, int mirror_port){
    int rv;

    erspan_info.is_erspan_v3 = FALSE;

    rv = mirror_erspan_example(unit, in_port, in_vlan, out_port, mirror_port);

    return rv;
}

/*
 * Example of mirroring packet to a ERSPANv3 tunnel.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 */
int mirror_erspan_example_v3(int unit, int in_port, int in_vlan, int out_port, int mirror_port){
    int rv;

    erspan_info.is_erspan_v3 = TRUE;

    rv = mirror_erspan_example(unit, in_port, in_vlan, out_port, mirror_port);

    return rv;
}

/*
 * Example of mirroring packet to a ERSPAN tunnel using egress multicast.
 * - in_port : port that packets to be recieved from
 * - in_vlan : packets received with in_vlan will be mirrored
 * - out_port : port that packets to be transmitted out
 * - mirror_port : mirror destination port
 * - mc_id : multicast ID of the mirror destination
 */
int mirror_erspan_multicast_example(int unit, int in_port, int in_vlan, int out_port, int mirror_port, int mc_id){
    bcm_l2_addr_t l2addr;
    bcm_if_t encap_id;
    bcm_pbmp_t p, u;
    int rv;

    mirror_info.in_port = in_port;
    mirror_info.out_port = out_port;
    mirror_info.vlan = in_vlan;
    mirror_info.is_mc = 1;
    mirror_info.mc_id = mc_id;

    erspan_routing(unit, in_port, out_port);

    bcm_vlan_create(unit, in_vlan);

    BCM_PBMP_PORT_SET(p, in_port);
    BCM_PBMP_PORT_ADD(p, out_port);
    BCM_PBMP_CLEAR(u);
    rv = bcm_vlan_port_add(unit, in_vlan, p, u);

    l2addr.mac = mirror_info.da;
    l2addr.vid = mirror_info.vlan;
    l2addr.flags = BCM_L2_STATIC;
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);

    /* build IP tunnels, and get back interfaces */
    if (rv = mirror_erspan_tunnel_create(unit, &encap_id)) {
        printf("Error, mirror_erspan_tunnel_create \n");
        return rv;
    }

    /* add inbound mirror to send out of the create ip tunnel with span encapsulation */
    rv = mirror_erspan_destination_create(unit, in_port, in_vlan, out_port, mirror_port, 1, mc_id, encap_id);
    if (rv != BCM_E_NONE) {
        printf("Error in mirror_erspan_destination_create, in_port=%d, \n", in_port);
        return rv;
    }

    return rv;
}

/*
 * Clean up procedure for mirror_erspan_example
 */
int mirror_erspan_example_clean_up(int unit){
    bcm_mirror_header_info_t mirror_header_info;
    bcm_pbmp_t p;
    int rv;

    if (rv = bcm_mirror_port_vlan_dest_delete(unit, 
            (G_mirror_type & BCM_MIRROR_PORT_INGRESS) ? mirror_info.in_port : mirror_info.out_port, 
            mirror_info.vlan, G_mirror_type, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    bcm_mirror_header_info_t_init(&mirror_header_info);
    if (rv = bcm_mirror_header_info_set(unit, 0, mirror_info.mirror_dest_id, &mirror_header_info)) {
        printf("Error: bcm_mirror_header_info_set %d\n", rv);
    }

    if (rv = bcm_mirror_destination_destroy(unit, mirror_info.mirror_dest_id)) {
        printf("Error: bcm_mirror_port_dest_delete %d\n", rv);
    }

    if (mirror_info.is_mc) {
        if (rv=bcm_multicast_destroy(unit, mirror_info.mc_id)) {
            printf("Error: bcm_multicast_destroy %d\n", rv);
        }
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.erspan_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv = bcm_l3_egress_destroy(unit, erspan_info.next_hop_if)) {
        printf("Error: bcm_l3_egress_destroy %d\n", rv);
    }

    if (rv = bcm_tunnel_initiator_clear(unit, &erspan_info.ip_tunnel_intf)) {
        printf("Error: bcm_tunnel_initiator_clear %d\n", rv);
    }

    if (rv=bcm_l2_addr_delete(unit, mirror_info.da, mirror_info.vlan)) {
        printf("Error: bcm_l2_addr_delete %d\n", rv);
    }

    BCM_PBMP_PORT_SET(p, mirror_info.in_port);
    BCM_PBMP_PORT_ADD(p, mirror_info.out_port);

    if (rv=bcm_vlan_port_remove(unit, mirror_info.vlan, p)) {
        printf("Error: bcm_vlan_port_remove %d\n", rv);
    }

    if (rv=bcm_vlan_destroy(unit, mirror_info.vlan)) {
        printf("Error: bcm_vlan_destroy %d\n", rv);
    }

    return BCM_E_NONE;
}

 
