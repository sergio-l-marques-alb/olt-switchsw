#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();

bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

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
multicast_create(4096, 1 /*Ingress*/);

/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_ingress_add(16781312, 9,  0x44801000);
multicast_ingress_add(16781312, 10, 0x44801001);
multicast_ingress_add(16781312, 11, 0x44801002);