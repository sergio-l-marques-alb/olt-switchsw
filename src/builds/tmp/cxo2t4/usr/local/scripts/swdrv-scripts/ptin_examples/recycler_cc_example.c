#include "port.c"
#include "bridge.c"

global_init();

bshell(0,"port xe9,xe10,xe11 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create LIFs instantiating the physical ports + VLAN */
vp_add(0x44801000 /*LIF2 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000060 /*Port1*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801001 /*LIF1 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000068 /*Port2*/, 100 /*VLAN*/, 0, 0, 0, 0);
vp_add(0x44801002 /*LIF3 gport*/, 3 /*BCM_VLAN_PORT_MATCH_PORT_VLAN*/, 0x24000338 /*Rcy1*/, 100 /*VLAN*/, 0, 0, 0, 0);

vlan_create(100);
vlan_add(100,9);
vlan_add(100,10);

bcm_vswitch_cross_connect_t cross1, cross2, cross3;
bcm_vswitch_cross_connect_t_init(&cross1);
cross1.port1=0x44801000;
cross1.port2=0x44801002;
cross1.flags=BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
print bcm_vswitch_cross_connect_add(0, &cross1);

bcm_vswitch_cross_connect_t_init(&cross2);
cross2.port1=0x44801002;
cross2.port2=0x44801001;
cross2.flags=BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
print bcm_vswitch_cross_connect_add(0, &cross2);

bcm_vswitch_cross_connect_t_init(&cross3);
cross3.port1=0x44801001;
cross3.port2=0x44801000;
cross3.flags=BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
print bcm_vswitch_cross_connect_add(0, &cross3);
