/*~~~~~~~~~~~~~~~~~~~~~~~Scheduled Fabric+Egress Multicast~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_sch_fab_eg_mc.c,v 1.0 2019/02/19 ivan Yan Exp $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File name: cint_sch_fab_eg_mc.c

Notes:
This cint is intended to show how to implement Scheduled Fabric+Egress MC in Jericho devices.
The packet will enter a device with a MC MAC DA. The MACT will then assign a MC group to the packet.
The MC ID will be configured as an egress MC, which will cause (in case the system has an FE device) the packet
to go to one of the FMQ, and from there to the fabric.
The FE device will then replicate the cells to each relevant FAP device (FAP devices that have ports that are
members of the MC group).
The packet will then enter the egress device's egress pipe, which will perform a lookup at the MC DB. 
The result of this lookup will be a single port -> a dedicated recycle port.
This port's header_type_in and header_type_out must be configured in the SOC properties as follows:
tm_port_header_type_in_220.BCM88675=STACKING
tm_port_header_type_out_220.BCM88675=STACKING
The CUD field in the MC DB is populated with a new MC group ID that will be used in the 2nd pass on the egress device.

Packet will re-enter the ingress pipe of the egress device.
The device will recieve the new MC ID from the Multicast field in the FTMH of the packet coming from the recycle port.
IT will then perform an Ingress lookup at the MC DB and will recieve from there a list of all ports that the packet
needs to be replicated to.
Since now it's an ingress MC - all packets will be replicated and scheduled (they will enter their respective VOQ).

In order to run this cint:

Enable stacking: 
stacking_enable=1

Define a dedicated recycle port in the SOC properties and set its header type:
ucode_port_220.BCM88675=RCY:core_0.220
tm_port_header_type_in_220.BCM88675=STACKING
tm_port_header_type_out_220.BCM88675=STACKING

Call this cint
BCM> cint cint_sch_fab_eg_mc.c

Send a packet with the following:
tx 1 psrc=200 data=010000000001000000000002810000019999450000b2000000000000ba4d0000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d

MAC DA: 01:00:00:00:00:01
Vlan ID: 1
*/

int sch_fabric_egress_mc_2_pass_example(int unit, int in_port, int out_port1, int out_port2, int out_port3){

    int rv = BCM_E_NONE;
    int eg_mc_id = 6000;
    int ing_mc_id = 5000;
    bcm_gport_t dest_ports[3] = {0,0,0};
    int port_count = 3;
    int ing_mc_cud[3] = {0,0,0};
    int recycle_port = 220;
    bcm_l2_addr_t l2add;
    bcm_mac_t dest_mac = {1,0,0,0,0,1};
    bcm_gport_t gport;
    bcm_trunk_member_t trunk;
    bcm_trunk_info_t trunk_info;
    int member_count = 1;
    int tid;

    dest_ports[0] = out_port1;
    dest_ports[1] = out_port2;
    dest_ports[2] = out_port3;

    /* Create stacking port configuration */
    BCM_TRUNK_STACKING_TID_SET(tid, 1);
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trunk_create\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, recycle_port);
    trunk.gport = gport;
    rv = bcm_trunk_set(unit, tid, &trunk_info, member_count, &trunk);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trunk_set\n");
        return rv;
    }

    /* Create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &eg_mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create (ingress)\n");
        return rv;
    }

    /* Add destination port to the egress MC Group */
    rv = bcm_multicast_egress_add (unit, eg_mc_id, recycle_port, ing_mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_egress_add\n");
        return rv;
    }

    /* Create an Ingress MC group and add the final port destinations to it so they will be scheduled */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID, &ing_mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create (ingress)\n");
        return rv;
    }

    /* Add destination ports to the ingress MC Group */
    rv = bcm_multicast_ingress_set (unit, ing_mc_id, port_count, dest_ports, ing_mc_cud);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_ingress_set\n");
        return rv;
    }

    bcm_l2_addr_t_init(&l2add, dest_mac, 1);
    l2add.l2mc_group = eg_mc_id;
    l2add.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2add);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_addr_add\n");
        return rv;
    }

    return BCM_E_NONE;
}

/* In order to run below example:

Enable stacking: 
stacking_enable=1

Define a dedicated recycle port in the SOC properties and set its header type:
ucode_port_220.BCM88675=RCY:core_0.220
tm_port_header_type_in_220.BCM88675=STACKING
tm_port_header_type_out_220.BCM88675=STACKING
custom_feature_two_pass_mc_dest_flow=1

Call this cint
BCM> cint cint_voq_connection.c
BCM> cint cint_sch_fab_eg_mc.c
c
print sch_fabric_egress_mc_2_pass_dest_flow_example(unit, 200, 201, 202, 203);

Send a packet with the following:
tx 1 psrc=200 data=000000000001000000000002810000019999450000b2000000000000ba4d0000000000000000000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f202122232425262728292a2b2c2d2e2f303132333435363738393a3b3c3d3e3f404142434445464748494a4b4c4d

MAC DA: 00:00:00:00:00:01
Vlan ID: 1
*/
int sch_fabric_egress_mc_2_pass_dest_flow_example(int unit, int in_port, int out_port1, int out_port2, int out_port3){

    int rv = BCM_E_NONE;
    int eg_mc_id = 6000;
    int recycle_ports[3] = {220, 220, 220};
    bcm_gport_t dest_ports[3] = {0, 0, 0};
    int port_count = 3;
    int egr_mc_cuds[3] = {10,20,30};
    bcm_l2_addr_t l2add;
    bcm_mac_t dest_mac = {0,0,0,0,0,1};
    bcm_vlan_t vid = 1;

    bcm_gport_t gport;
    bcm_trunk_member_t trunk;
    bcm_trunk_info_t trunk_info;
    int member_count = 1;
    int tid;

    int i = 0;
    int voq_connector_ids[3] = {0xF000, 0xF008, 0xF010};  /* Default for Jericho , allocated with ID by API bcm_cosq_voq_connector_gport_add() */
    bcm_gport_t flow_gport = 0; /*Allocated by API bcm_cosq_gport_add() */
    int number_queue = 8;
    uint32 additional_flags = BCM_COSQ_GPORT_TM_FLOW_ID;
    bcm_switch_encap_dest_t encap_info;
    int is_qax = 0;
    int is_qux = 0;

    dest_ports[0] = out_port1;
    dest_ports[1] = out_port2;
    dest_ports[2] = out_port3;

    /*For QAX/QUX in default there is different for VOQ connector */
    rv = is_qumran_ax_only(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Error, is_qumran_ax_only\n");
        return rv;
    }
    if (is_qax) {
      printf("running over VOQ connector base vals for QAX\n");
      voq_connector_ids[0] = 0x9000;
      voq_connector_ids[1] = 0x9008;
      voq_connector_ids[2] = 0x9010;
      
    }

    rv = is_qumran_ux_only(unit, &is_qux);
    if (rv != BCM_E_NONE) {
      printf("Error, is_qumran_ux_only\n");
      return rv;
    }
    if (is_qux) {
      printf("running over VOQ connector base vals for QUX\n");
      voq_connector_ids[0] = 0x4800;
      voq_connector_ids[1] = 0x4808;
      voq_connector_ids[2] = 0x4810;
    }

    /* Create stacking port configuration */
    BCM_TRUNK_STACKING_TID_SET(tid, 1);
    rv = bcm_trunk_create(unit, BCM_TRUNK_FLAG_WITH_ID, &tid);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trunk_create\n");
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(gport, recycle_ports[0]);
    trunk.gport = gport;
    rv = bcm_trunk_set(unit, tid, &trunk_info, member_count, &trunk);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_trunk_set\n");
        return rv;
    }

    /* Create Egress MC group and add the recycle port as member with CUD set to ingress MC ID */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &eg_mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_create (ingress)\n");
        return rv;
    }

    /* Add destination port to the egress MC Group */
    rv = bcm_multicast_egress_set (unit, eg_mc_id, port_count, recycle_ports, egr_mc_cuds);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_multicast_egress_add\n");
        return rv;
    }

    /* L2 MAC destianation is mcast */
    bcm_l2_addr_t_init(&l2add, dest_mac, vid);
    l2add.l2mc_group = eg_mc_id;
    l2add.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    rv = bcm_l2_addr_add(unit, &l2add);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_l2_addr_add\n");
        return rv;
    }

    /* Set recycle PTC for selecting new FLP program */
    print bcm_port_control_set(0, recycle_ports[0], bcmPortControlOverlayRecycle, 1);

    /* Add two encap id to destinaton mapping entries */
    for (i = 0; i < port_count - 1; i++)
    {
        /* creates and connects voq and voq connector for the given local port */
        print create_and_connect_local_voq_and_connector_example(unit, dest_ports[i], number_queue, &flow_gport, voq_connector_ids[i], additional_flags);

        /* encap id to destinaton mapping */
        encap_info.encap_id = egr_mc_cuds[i];
        encap_info.gport = flow_gport;
        print bcm_switch_encap_dest_map_add(unit, &encap_info);
    }

    return BCM_E_NONE;
}
