#include <port.c>
#include <bridge.c>
#include <trunk.c>

global_init();

bshell(0,"port xe en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

trunk_create(1,9);
trunk_member_add(1,10);
trunk_member_add(1,9);

vp_add(0x44801000,3,0xc000001,100,0,0,0,0);
vp_add(0x44801002,3,11,100,0,0,0,0);

vswitch_create(4096);
vswitch_add(4096,0x44801000);
vswitch_add(4096,0x44801002);

multicast_create(4096);
vswitch_flood_set(0,4096,16781312,16781312,16781312);
multicast_vlan_add(16781312,0xc000001,0x44801000);
/*multicast_vlan_add(16781312,9,0x44801000);
multicast_vlan_add(16781312,10,0x44801000);*/
multicast_vlan_add(16781312,11,0x44801002);

