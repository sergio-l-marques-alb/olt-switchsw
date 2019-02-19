#include "port.c"
#include "bridge.c"

int port1=9;
int port2=10;
int vsi=2;
int outer_vlan1=100;
int outer_vlan2=333;
int inner_vlan=1000;
unsigned int tpid1=0x8100;
unsigned int tpid2=0x9100;
unsigned int lif1=0x44801000;
unsigned int lif2=0x44801001;

tpid_set(tpid1, tpid2);
global_init();

bshell(0,"port xe en=1 speed=1000 rxpause=0 txpause=0");
bshell(0,"sleep 1");
bshell(0,"ps");

vlan_create(vsi);
vp_add(lif1, BCM_VLAN_PORT_MATCH_PORT_VLAN /*Criteria*/, port1, outer_vlan1, 0, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);
vp_add(lif2, BCM_VLAN_PORT_MATCH_PORT_VLAN /*Criteria*/, port2, outer_vlan2, 0, 0 /*TunnelID*/, 0 /*VSI*/, 0 /*Flags*/);
vlan_add(vsi, port1);
vlan_add(vsi, port2);
vswitch_add(vsi, lif1);
vswitch_add(vsi, lif2);

/* ActionID=2: Replace outer VLAN, nothing done to inner VLAN */
adv_xlate_action_create(2 /*ActionId*/, 1 /*ut_outer=ADD*/, 0 /*ut_inner=NONE*/, 2 /*ot_outer=REPLACE*/, 0 /*ot_inner=NONE*/, 2 /*dt_outer=REPLACE*/, 0 /*dt_inner=NONE*/,
                        tpid1 /*OuterTPID*/, tpid2 /*InnerTPID*/,
                        0 /*NewOVID*/, 0 /*NewIVID*/,
                        0 /*Egress*/);
adv_xlate_action_create(3 /*ActionId*/, 1 /*ut_outer=ADD*/, 1 /*ut_inner=ADD*/, 2 /*ot_outer=REPLACE*/, 1 /*ot_inner=ADD*/, 2 /*dt_outer=REPLACE*/, 1 /*dt_inner=ADD*/,
                        tpid1 /*OuterTPID*/, tpid2 /*InnerTPID*/,
                        0 /*NewOVID*/, 0 /*NewIVID*/,
                        0 /*Egress*/);

/* Associate Tag Format to Action and to VLAN Edit Profile */
/* Tag format 2 -> Single tagged: tpid1=0x8100, tpid2=BCM_PORT_TPID_CLASS_TPID_ANY */
/* Tag format 6 -> Double tagged: tpid1=0x8100, tpid2=0x8100 */
/* VEP: Outer=SWAP Inner=NONE */
adv_xlate_action_class_set(2 /*Single Tag Format*/, 2 /*VEP*/, 2 /*ActionId*/, 0 /*Egress*/);
adv_xlate_action_class_set(6 /*Double Tag Format*/, 2 /*VEP*/, 2 /*ActionId*/, 0 /*Egress*/);
/* VEP: Outer=SWAP Inner=ADD */
adv_xlate_action_class_set(2 /*Single Tag Format*/, 3 /*VEP*/, 3 /*ActionId*/, 0 /*Egress*/);
adv_xlate_action_class_set(6 /*Double Tag Format*/, 3 /*VEP*/, 3 /*ActionId*/, 0 /*Egress*/);

/* VLAN Edit Profile (ID=2) for LIF1 */
adv_xlate_vep(lif1, outer_vlan1, inner_vlan, 2 /*VEP Id*/, 0 /*Egress*/);
/* VLAN Edit Profile (ID=2) for LIF2 */
adv_xlate_vep(lif2, outer_vlan2, inner_vlan, 3 /*VEP Id*/, 0 /*Egress*/);
