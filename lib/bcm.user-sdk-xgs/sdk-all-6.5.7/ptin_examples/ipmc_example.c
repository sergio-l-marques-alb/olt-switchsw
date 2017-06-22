#include "port.c"
#include "bridge.c"
#include "l3.c"

global_init();

bshell(0,"port xe9,xe10 en=1 speed=1000 fd=1 an=0 txpause=0 rxpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create VLAN and add ports to it (bridge.c) */
vlan_create(100);
vlan_create(101);
vswitch_flood_set(0,100,1,1,1);
vswitch_flood_set(0,101,1,1,1);
vlan_add(100,9);
vlan_add(101,10);

/* Create L3 interface for source port (l3.c) */
int l3_intf;
bcm_mac_t my_mac_addr={0,0,0,0,0,1};
print create_l3_intf(0, 0, 100, my_mac_addr, &l3_intf);
/* In-RIF */
print create_l3_ingress(0, BCM_L3_INGRESS_GLOBAL_ROUTE, l3_intf);

/* Create MC group (bridge.c) */
int mc_group;
mc_group=60000;
multicast_create(&mc_group, 0 /*Egress*/, BCM_MULTICAST_TYPE_L3);
/* Add output port to MC group */
multicast_l3_egress_add(mc_group /*MC group*/, 10,  101);

/* Create IPMC entry */
bcm_ipmc_addr_t data;
bcm_ipmc_addr_t_init(&data);
data.flags = 0;
data.l3a_intf = l3_intf;
data.mc_ip_addr = 0xe0000001;
data.mc_ip_mask = 0xffffffff;
data.s_ip_addr = 0;
data.s_ip_mask = 0;
data.vid = 100;
data.group = mc_group;

print bcm_ipmc_add(0, &data);
