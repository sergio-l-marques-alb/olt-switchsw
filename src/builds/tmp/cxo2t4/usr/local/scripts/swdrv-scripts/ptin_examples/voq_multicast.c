#include "port.c"
#include "bridge.c"

global_init();

bshell(0,"port xe9,xe10 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000060 /*Port1*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000068 /*Port2*/, 100 /*VLAN*/, 0, 0, 0, 0);

/* Create VSI and attach LIFs */
vswitch_create(4096);
vswitch_add(4096, 0x44801000 /*LIF1*/);
vswitch_add(4096, 0x44801001 /*LIF2*/);

/* Create Multicast group */
multicast_create(4096, 1 /*Ingress*/);
/* Configure egress replication (MCgroup=16781312=0x1001000) */
multicast_ingress_add(0x1001000 /*MC group*/, 0x24000060, 0x44801000);
multicast_ingress_add(0x1001000 /*MC group*/, 0x24000068, 0x44801001);


/* Create Multicast group */
//multicast_create(4096, 0 /*Egress*/);
/* Configure egress replication (MCgroup=16781312=0x1001000) */
//multicast_egress_add(0x1001000 /*MC group*/, 9 , 0x44801000);
//multicast_egress_add(0x1001000 /*MC group*/, 10, 0x44801001);


m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=1
diag cosq non
m IPS_IPS_GENERAL_CONFIGURATIONS DIS_DEQ_CMDS=0