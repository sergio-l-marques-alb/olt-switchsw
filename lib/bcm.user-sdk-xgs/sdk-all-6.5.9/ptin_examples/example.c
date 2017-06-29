#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();

bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Check if double lb key range is enabled (relevant for gracefull load balancing) */             
print bcm_switch_control_set(0, bcmSwitchMcastTrunkIngressCommit, 0);
print bcm_switch_control_set(0, bcmSwitchMcastTrunkEgressCommit, 0);

/* Define VLAN-domains */
//print bcm_port_class_set(0,9,bcmPortClassId,2);
//print bcm_port_class_set(0,10,bcmPortClassId,3);
//print bcm_port_class_set(0,11,bcmPortClassId,3);

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 9  /*Port*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 10 /*Port*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801002 /*LIF gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 11 /*Port*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Create VSI and attach LIFs */
vswitch_create(4096);
vswitch_add(4096, 0x44801000);
vswitch_add(4096, 0x44801001);
vswitch_add(4096, 0x44801002);

/* Create Multicast group */
multicast_create(4096, 0 /*Egress*/);

/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_egress_add(16781312, 9,  0x44801000);
multicast_egress_add(16781312, 10, 0x44801001);
multicast_egress_add(16781312, 11, 0x44801002);

/* Create trunk involving physical ports 9 and 10 */
//trunk_create(1, 14 /*PSC_ROUNDROBIN*/);
//trunk_member_add(1, 10 /*Port 10 */);
//trunk_member_add(1, 11 /*Port 11*/);

/* Define VLAN-domains */
//print bcm_port_class_set(0,9,bcmPortClassId,2);
//print bcm_port_class_set(0,10,bcmPortClassId,30);
//print bcm_port_class_set(0,11,bcmPortClassId,30);

//vp_add(0x44801003 /*LIF gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0xc000001 /*Port*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Use MACSA to calculate hash for L2 packets */
//print bcm_switch_control_set(0, bcmSwitchHashL2Field0, BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI);

