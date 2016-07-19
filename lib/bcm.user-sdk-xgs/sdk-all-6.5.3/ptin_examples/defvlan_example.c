#include "port.c"
#include "bridge.c"

global_init();

bshell(0,"port xe9,xe10 en=1 speed=1000 rxpause=0 txpause=0");
bshell(0,"ps");

port_defvlan_set(9,100);
port_defvlan_set(10,100);

vlan_create(100);
vlan_add(100,9);
vlan_add(100,10);

bshell(0,"l2 clear all");
bshell(0,"sleep 1");
bshell(0,"l2 show");

