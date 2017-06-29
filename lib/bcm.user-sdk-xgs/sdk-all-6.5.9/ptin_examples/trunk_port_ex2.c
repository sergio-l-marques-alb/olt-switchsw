/* Check if double lb key range is enabled (relevant for gracefull load balancing) */
print bcm_switch_control_set(0, bcmSwitchMcastTrunkIngressCommit, 0);
print bcm_switch_control_set(0, bcmSwitchMcastTrunkEgressCommit, 0);

/* Define VLAN-domains */                                      
print bcm_port_class_set(0,10,bcmPortClassId,10);              
print bcm_port_class_set(0,11,bcmPortClassId,10);
                                                               
/* Create trunk involving physical ports 9 and 10 */           
trunk_create(1, 14 /*PSC_ROUNDROBIN*/);              
trunk_member_add(1, 10 /*Port 10 */);                                                                                       
trunk_member_add(1, 11 /*Port 11*/);                                                                                        

multicast_egress_remove(16781312, 10, 0x44801001);
multicast_egress_remove(16781312, 11, 0x44801002);                                                                                                                            
vp_remove(0x44801001);
vp_remove(0x44801002);

vp_add(0x44801003 /*LIF gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x0c000001 /*Trunk gport*/, 100 /*VLAN*/, 0, 0, 0, 0);
vswitch_add(4096, 0x44801003);                                                                                                        

multicast_egress_add(16781312, 0x0c000001, 0x44801003);
                                                                                                                                      
/* Use MACSA to calculate hash for L2 packets */
//print bcm_switch_control_set(0, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI);
