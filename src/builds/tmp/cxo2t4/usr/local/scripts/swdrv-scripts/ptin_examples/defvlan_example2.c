#include "port.c"
#include "bridge.c"

int port1=9;
int port2=10;
unsigned int defVlan=100;

unsigned int lif1=0x44801000;
unsigned int lif2=0x44801001;

/* Set TPIDs to be used */
tpid_set(0x8100 /*OuterTPID*/, 0x9100 /*InnerTPID*/);
global_init();

bshell(0,"port xe en=1 speed=1000 rxpause=0 txpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

/* Create VLAN */
vlan_create(defVlan);

/* Configure the default VLAN for each physical port */
port_defvlan_set(port1, defVlan);
port_defvlan_set(port2, defVlan);

/* Create LIFs */
vp_add(lif1 /*LIF1*/, BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN /*Criteria*/, port1 /*Port*/, defVlan /*OuterVLAN*/, 0 /*InnerVLAN*/, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);
vp_add(lif2 /*LIF2*/, BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN /*Criteria*/, port2 /*Port*/, defVlan /*OuterVLAN*/, 0 /*InnerVLAN*/, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);

/* Add members to defVlan */
vlan_add(defVlan, port1);
vlan_add(defVlan, port2);

/* Add LIFs to VSI defVlan */
vswitch_add(defVlan, lif1);
vswitch_add(defVlan, lif2);

/* Is this necessary? Configuring default VLAN for LIFs */
port_defvlan_set(lif1, defVlan);
port_defvlan_set(lif2, defVlan);

bshell(0,"l2 clear all");
bshell(0,"sleep 1");
bshell(0,"l2 show");
