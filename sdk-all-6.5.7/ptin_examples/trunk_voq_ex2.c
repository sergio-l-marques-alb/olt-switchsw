/* Check if double lb key range is enabled (relevant for gracefull load balancing) */
print bcm_switch_control_set(0, bcmSwitchMcastTrunkIngressCommit, 0);
print bcm_switch_control_set(0, bcmSwitchMcastTrunkEgressCommit, 0);

/* Define VLAN-domains */
print bcm_port_class_set(0, 10 /*Port10*/, bcmPortClassId, 10);
print bcm_port_class_set(0, 11 /*Port11*/, bcmPortClassId, 10);
print bcm_port_class_set(0,0x24000068 /*VoQ-Port10*/, bcmPortClassId, 10);
print bcm_port_class_set(0,0x24000070 /*VoQ-Port11*/, bcmPortClassId, 10);

multicast_egress_remove(16781312, 0x24000068 /*VoQ-Port10*/, 0x44801001 /*LIF1 gport*/);
multicast_egress_remove(16781312, 0x24000070 /*VoQ-Port11*/, 0x44801002 /*LIF2 gport*/);

/* Create trunk involving physical ports 9 and 10 */
trunk_create(1, 14 /*PSC_ROUNDROBIN*/);
trunk_member_add(1, 0x24000068 /*VoQ-Port10*/);
trunk_member_add(1, 0x24000070 /*VoQ-Port11*/);

vp_add(0x44801003 /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x0c000001 /*Trunk gport*/, 100 /*VLAN*/, 0, 0, 0, 0);
vswitch_add(4096, 0x44801003 /*LIF3 gport*/);

multicast_egress_add(16781312, 0x0c000001 /*Trunk gport*/, 0x44801003 /*LIF3 gport*/);
//multicast_egress_add(16781312, 0x24000068 /*VoQ-Port10*/, 0x44801003 /*LIF3 gport*/);
//multicast_egress_add(16781312, 0x24000070 /*VoQ-Port11*/, 0x44801003 /*LIF3 gport*/);

/* Use MACSA to calculate hash for L2 packets */                                                                                      
//print bcm_switch_control_set(0, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI);
