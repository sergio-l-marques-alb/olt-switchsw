#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();

bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* VLAN-Domain */
print bcm_port_class_set(0, 9  /*Port9 */, bcmPortClassId, 9 );
print bcm_port_class_set(0, 10 /*Port10*/, bcmPortClassId, 10);
print bcm_port_class_set(0, 11 /*Port11*/, bcmPortClassId, 11);

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF0 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000060 /*VoQ-Port9 */, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000068 /*VoQ-Port10*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801002 /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000070 /*VoQ-Port11*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Create VSI and attach LIFs */
vswitch_create(4096);
vswitch_add(4096, 0x44801000 /*LIF0 gport*/);
vswitch_add(4096, 0x44801001 /*LIF1 gport*/);
vswitch_add(4096, 0x44801002 /*LIF2 gport*/);

/* Create Multicast group */
multicast_create(4096, 0 /*Egress*/);

/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_egress_add(16781312, 0x24000060 /*VoQ-Port9 */, 0x44801000 /*LIF0 gport*/);
multicast_egress_add(16781312, 0x24000068 /*VoQ-Port10*/, 0x44801001 /*LIF1 gport*/);
multicast_egress_add(16781312, 0x24000070 /*VoQ-Port11*/, 0x44801002 /*LIF2 gport*/);

